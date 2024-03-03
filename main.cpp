#include <cstdio>
#include <gccore.h>
#include <cmath>
#include <wiiuse/wpad.h>
#include <iostream>


#include "wiimote.h"
#include "engine/render/renderer.h"

#include "engine/render/bloc.h"
#include "src/system/saveManager.h"
#include "player.h"
#include "render/cacheManager.h"
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
	
	World::Init();
	
	Renderer::setupVideo();
	Renderer::setupVtxDesc();
    Renderer::setupTexture();
	
	Wiimote::setup();
	
    Player players[4] {Player(WPAD_CHAN_0),
					   Player(WPAD_CHAN_1),
					   Player(WPAD_CHAN_2),
					   Player(WPAD_CHAN_3)};
	
	//GX_InitTexObjFilterMode(&texture, GX_NEAR, GX_NEAR);

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
	
	ChunkCache::init();
	ChunkCache::reset();
	
	for (int i = 0; i < 4; i++) {
		players[i].setPos(0, 126, 0);
		players[i].renderer.camera.rotateH(45.0f * (f32)i);
	}
	
	/// Splash Screen
    players[0].renderer.camera.loadOrtho(); // set for 2D drawing
    players[0].renderer.camera.applyTransform2D();
	Renderer::renderSplashScreen();
	Renderer::endFrame();
	
	
	World::requestChunks(ChunkCoord(0, 0), 8);
	Renderer::setClearColor();
	
    while (!exiting) {
		
		
        /// Calculate used memory
        
		u32 mem1 = SYS_GetArena1Size();
		u32 mem2 = SYS_GetArena2Size();
		
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
		printf("caching\r");
		
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
	
	        if (player.isUnderwater()) player.Underwater();
	
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
