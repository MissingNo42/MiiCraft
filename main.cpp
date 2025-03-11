#include <cmath>
#include <cstdio>
#include <gccore.h>
#include <iostream>
#include <wiiuse/wpad.h>


#include "wiimote.h"
#include "engine/render/renderer.h"
#include "player.h"
#include "engine/render/cache.h"
#include "src/system/saveManager.h"
#include "world/world.h"


static int exiting = 0;

//Calling the function will end the while loop and properly exit the program to the HBChannel.
void reload(u32, void *) {
	exiting = 1;
}

//Calling the function will end the while loop and then properly shutdown the system
void shutdown() {
	exiting = 2;
}


void checkmem() {

	constexpr union {
		u32 ab[2];
		struct {
			u32 A, B;
		};
	} check_array = {.A = 0xffffffff, .B = 0};

	if (check_array.ab[0] != 0xffffffff || check_array.ab[1] != 0) {
		printf("Error: struct byte alignment is not working properly, check host endian (%08X | %08X)\r", check_array.A, check_array.B);
		exit(1);
	}

	constexpr union {
		u16 i;
		struct {
			u8 X, Y;
		};
	} check_bytes = {.i = 0xff00};

	if (check_bytes.X != 0xff || check_bytes.Y != 0) {
		printf("Error: struct byte alignment is not working properly, check host endian (%02X | %02X)\r", check_bytes.X, check_bytes.Y);
		exit(1);
	}

	constexpr union {
		u8 i;
		struct {
			u8 X: 2, Y: 6;
		};
	} check_bits = {.i = 0b11000000};

	if (check_bits.X != 3 || check_bits.Y != 0) {
		printf("Error: bitfield is not working properly, check host endian (%02X | %02X)\r", check_bits.X, check_bits.Y);
		exit(1);
	}
}


int main(int, char **) {
	SYS_STDIO_Report(true);

	checkmem();

	World::Init();

	Renderer::setupVideo();

	Renderer::setupVertexAttributeTable();

	Renderer::setupTexture();

	Wiimote::setup();

	Player players[4]{Player(WPAD_CHAN_0),
	                  Player(WPAD_CHAN_1),
	                  Player(WPAD_CHAN_2),
	                  Player(WPAD_CHAN_3)};

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);

	ChunkCache::reset();
	printf("NNNNNNN\r");

	f32 initpos = 0.00001f; // float limit for i++: 16777215 (9007199254740992 for double), no rendering beyond this point;

	for (int i = 0; i < 4; i++) {
		players[i].setPos(initpos, 126, initpos);
		players[i].renderer.camera.rotateH(45.0f * (f32) i);
	}

	/// Splash Screen
	players[0].renderer.camera.loadOrtho(); // set for 2D drawing
	players[0].renderer.camera.applyTransform2D();
	printf("XXXXX\r");
	Renderer::renderSplashScreen();
	printf("YYYYY\r");
	Renderer::endFrame();

	printf("Loading world...\r");
	World::requestChunks(ChunkCoord((int) players[0].renderer.camera.pos.x >> 4, (int) players[0].renderer.camera.pos.y >> 4), 8);
	printf("Loaded world!\r");
	Renderer::setClearColor();

	players[0].inventory.addItem(BlockType::Furnace, 64);
	players[0].inventory.addItem(BlockType::DoorLow, 64);
	players[0].inventory.addItem(BlockType::DoorHigh, 64);
	players[0].inventory.addItem(BlockType::Glowstone, 64);
	players[0].inventory.addItem(BlockType::Lava, 64);


	//Renderer::setRegion(TILE_COORDS(8, 9), TILE_COORDS(9, 9), TILE_COORDS(8, 10), TILE_COORDS(9, 10));

	//exit(0);

	while (!exiting) {
		Renderer::update();

		Renderer::animateWater();

		/// Calculate used memory

		u32 mem1 = SYS_GetArena1Size();
		u32 mem2 = SYS_GetArena2Size();

		printf("Coord : %d %d %d [%d, %d]: %.1f %d\r",
		       (int) players[0].renderer.camera.pos.x, (int) players[0].renderer.camera.pos.y, (int) players[0].renderer.camera.pos.z,
		       (int) players[0].renderer.camera.pos.x >> 4, (int) players[0].renderer.camera.pos.z >> 4,
		       players[0].renderer.camera.angleH,
		       players[0].renderer.camera.direction);
		printf("Memory : MEM1 %d (%d)\tMEM2 %d (%d)\r", mem1, mem1 / sizeof(Chunk), mem2, mem2 / sizeof(Chunk));


		/// Update Wiimotes & Run Engine

		Wiimote::sync();

		for (auto &player: players) {
			player.wiimote.update();
			player.update();
		}


		/// Update Viewport Layout

		s8 S[4] = {0, 0, 0, 0}, c = 0;
		for (int i = 0; i < 4; i++) if (players[i].wiimote.connected) S[c++] = (s8) i;

		if (c == 1) players[S[0]].renderer.camera.resize(Camera::FullScreen);
		else if (c == 2) {
			players[S[0]].renderer.camera.resize(Camera::SplitTop);
			players[S[1]].renderer.camera.resize(Camera::SplitBottom);
		} else if (c == 3) {
			players[S[0]].renderer.camera.resize(Camera::SplitTop);
			players[S[1]].renderer.camera.resize(Camera::QuarterBL);
			players[S[2]].renderer.camera.resize(Camera::QuarterBR);
		} else for (int i = 0; i < 4; i++) players[S[i]].renderer.camera.resize(static_cast<Camera::Format>(1 << i));

		/// Cache
		printf("caching %.2f\r", players[0].renderer.camera.angleH);

		ChunkCache::prerender(players);


		/// Render
		printf("rendering\r");

		for (int i = 0; i < c; i++) {
			auto &player = players[S[i]];
			player.renderer.camera.applyScissor();
			player.renderer.camera.loadPerspective();

			player.renderer.camera.update(true);

			//PoC vertex shader leaves waving
			//guMtxRowCol(player.renderer.camera.view3D, 0, 3) += std::sin(Renderer::environment.time * 2.f * M_PI) / 3.f;
			//guMtxRowCol(player.renderer.camera.view3D, 1, 3) += std::cos(Renderer::environment.time * 200.f * M_PI) / 3.f;
			//guMtxRowCol(player.renderer.camera.view3D, 2, 3) += std::sin(Renderer::environment.time * 200.f * M_PI) / 3.f;
			//player.renderer.camera.applyTransform();

			ChunkCache::render(player.renderer.camera);

			Renderer::setVertexFormat<AdvancedVertex>();
			Renderer::setVertexShader<AdvancedVertex>();
			player.renderer.renderSky();

			Renderer::setVertexFormat<ColoredVertex>();
			Renderer::setVertexShader<ColoredVertex>();
			player.renderer.renderCloud();

			Renderer::setVertexFormat<Vertex>();
			Renderer::setVertexShader<Vertex>();
			//player.renderer.renderSky();

			player.renderFocus();
			player.renderDestroy();

			if (player.creative) player.inventory.resetInventory();

			player.renderer.camera.loadOrtho(); // set for 2D drawing
			player.renderer.camera.applyTransform2D();

			if (player.isUnderwater()) Renderer::Underwater();

			player.renderInventory();
		}

		GX_DrawDone(); // after this point the EFB is ready to external editing (pixel shader)

		for (int i = 0; i < c; i++) {
			players[S[i]].renderCursor();
		}

		printf("rendered\r");
		Renderer::endFrame();

		if (Wiimote::quit) exiting = 1;
	}

	if (exiting == 2) SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);

	return 0;
}
