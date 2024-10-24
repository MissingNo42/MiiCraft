#include <cstdio>
#include <gccore.h>
#include <cmath>
#include <wiiuse/wpad.h>
#include <iostream>


#include "wiimote.h"
#include "engine/render/renderer.h"

#include "engine/render/block.h"
#include "src/system/saveManager.h"
#include "player.h"
#include "render/cache.h"
#include "world/world.h"


int exiting = 0;

//Calling the function will end the while loop and properly exit the program to the HBChannel.
void reload(u32, void *) {
	exiting = 1;
}

//Calling the function will end the while loop and then properly shutdown the system
void shutdown() {
	exiting = 2;
}


int main(int, char **) {
    SYS_STDIO_Report(true);

	u32 X =0;
	for (int Y=0; Y<(int)sizeof(std::set<ChunkCache>); Y++) {
		X += ((u8*)&ChunkCache::toCacheSet)[Y];
	}


	World::Init();

	X =0;
	for (int Y=0; Y<(int)sizeof(std::set<ChunkCache>); Y++) {
		X += ((u8*)&ChunkCache::toCacheSet)[Y];
	}

	Renderer::setupVideo();
	X =0;
	for (int Y=0; Y<(int)sizeof(std::set<ChunkCache>); Y++) {
		X += ((u8*)&ChunkCache::toCacheSet)[Y];
	}

	Renderer::setupVtxDesc();
	X =0;
	for (int Y=0; Y<(int)sizeof(std::set<ChunkCache>); Y++) {
		X += ((u8*)&ChunkCache::toCacheSet)[Y];
	}

    Renderer::setupTexture();

	X =0;
	for (int Y=0; Y<(int)sizeof(std::set<ChunkCache>); Y++) {
		X += ((u8*)&ChunkCache::toCacheSet)[Y];
	}

	Wiimote::setup();

	X =0;
	for (int Y=0; Y<(int)sizeof(std::set<ChunkCache>); Y++) {
		X += ((u8*)&ChunkCache::toCacheSet)[Y];
	}

    Player players[4] {Player(WPAD_CHAN_0),
					   Player(WPAD_CHAN_1),
					   Player(WPAD_CHAN_2),
					   Player(WPAD_CHAN_3)};

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);


	X =0;
	for (int Y=0; Y<(int)sizeof(std::set<ChunkCache>); Y++) {
		X += ((u8*)&ChunkCache::toCacheSet)[Y];
	}

	printf("X: %d\r", X);
	ChunkCache::init();
	printf("ZZZZZZ\r");

	X =0;
	for (int Y=0; Y<(int)sizeof(std::set<ChunkCache>); Y++) {
		X += ((u8*)&ChunkCache::toCacheSet)[Y];
	}

	printf("X: %d\r", X);

	ChunkCache::reset();
	printf("NNNNNNN\r");

	f32 initpos = 0.00001f; // float limit for i++: 16777215 (9007199254740992 for double), no rendering beyond this point;

	for (int i = 0; i < 4; i++) {
		players[i].setPos(initpos, 126, initpos);
		players[i].renderer.camera.rotateH(45.0f * (f32)i);
	}

	/// Splash Screen
    players[0].renderer.camera.loadOrtho(); // set for 2D drawing
    players[0].renderer.camera.applyTransform2D();
	Renderer::renderSplashScreen();
	Renderer::endFrame();

	printf("Loading world...\r");
	World::requestChunks(ChunkCoord((int)players[0].renderer.camera.pos.x >> 4, (int)players[0].renderer.camera.pos.y >> 4), 8);
	printf("Loaded world!\r");
	Renderer::setClearColor();

	players[0].inventory.addItem(BlockType::Furnace, 64);
	players[0].inventory.addItem(BlockType::DoorLow, 64);
	players[0].inventory.addItem(BlockType::DoorHigh, 64);
	players[0].inventory.addItem(BlockType::Glowstone, 64);
	players[0].inventory.addItem(BlockType::Lava, 64);

	// print Lights[16][16] array
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			printf("%02X ", Lights[(i << 4) | j].r);
		}
		printf("\r");
	}


	 //exit(0);

    while (!exiting) {

		static f32 day = 0;
		//Renderer::setLight((std::cos(day) + 1.f) / 2.f, true);
		//day += 0.005f;

		runWater();

        /// Calculate used memory

		u32 mem1 = SYS_GetArena1Size();
		u32 mem2 = SYS_GetArena2Size();

		printf("Coord : %d %d %d [%d, %d]: %.1f %d\r",
			   (int)players[0].renderer.camera.pos.x, (int)players[0].renderer.camera.pos.y, (int)players[0].renderer.camera.pos.z,
			   (int)players[0].renderer.camera.pos.x >> 4, (int)players[0].renderer.camera.pos.z >> 4,
			   players[0].renderer.camera.angleH,
			   players[0].renderer.camera.direction);
		printf("Memory : MEM1 %d (%d)\tMEM2 %d (%d)\r", mem1, mem1 / sizeof(VerticalChunk), mem2, mem2 / sizeof(VerticalChunk));


		/// Update Wiimotes & Run Engine

		Wiimote::sync();

		for (auto & player : players) {
			player.wiimote.update();
			player.update();
		}


		/// Update Viewport Layout

		s8 S[4] = {0, 0, 0, 0 }, c = 0;
		for (int i = 0; i < 4; i++) if (players[i].wiimote.connected) S[c++] = (s8)i;

		if (c == 1) players[S[0]].renderer.camera.resize(Camera::FullScreen);
		else if (c == 2) {
			players[S[0]].renderer.camera.resize(Camera::SplitTop);
			players[S[1]].renderer.camera.resize(Camera::SplitBottom);
		} else if (c == 3) {
			players[S[0]].renderer.camera.resize(Camera::SplitTop);
			players[S[1]].renderer.camera.resize(Camera::QuarterBL);
			players[S[2]].renderer.camera.resize(Camera::QuarterBR);
		}
		else for (int i = 0; i < 4; i++) players[S[i]].renderer.camera.resize((Camera::Format)(1 << i));


		/// Cache
		printf("caching %.2f\r", players[0].renderer.camera.angleH);

		//LightControl::computeNaturalLight((sinf(4 * DegToRad(players[0].renderer.camera.angleH)) + 1) * .5);
		//LightControl::computeNaturalLight((sinf(4 * DegToRad(players[0].renderer.camera.angleH)) + 1) * .5);
		//LightControl::flush();

		ChunkCache::cache(players);


		/// Render
		printf("rendering\r");

		for (int i = 0; i < c; i++) {
			auto& player = players[S[i]];
			player.renderer.camera.applyScissor();
	        player.renderer.camera.loadPerspective();

	        player.renderer.camera.update(true);

			player.renderer.renderSky();

			ChunkCache::render(player.renderer.camera);

			player.renderFocus();
			player.renderDestroy();

	        if (player.creative) player.inventory.resetInventory();

	        player.renderer.camera.loadOrtho(); // set for 2D drawing
	        player.renderer.camera.applyTransform2D();

	        if (player.isUnderwater()) Renderer::Underwater();

	        player.renderInventory();
	        player.renderCursor();
		}

		printf("rendered\r");
		Renderer::endFrame();

		if (Wiimote::quit) exiting = 1;
	}

	if (exiting == 2) SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);

	return 0;
}
