#include <cstdio>
#include <gccore.h>
#include <cmath>
#include <wiiuse/wpad.h>
#include <iostream>


#include "wiimote.h"
#include "engine/render/renderer.h"
#include "engine/render/render.h"
#include "engine/render/interface.h"

#include "engine/render/bloc.h"
#include "src/system/saveManager.h"
#include "player.h"
#include "render/cacheManager.h"


int exiting = 0;
const bool PRINT_PLAYER_POS = false;

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
	
	PAD_Init();
	WPAD_Init();
	
	World::Init();
	
	Renderer::setupVideo();
	Renderer::setupVtxDesc();
    Renderer::setupTexture();
	
    Player player(8, 120, 8);
    GUI guy;

	//GX_InitTexObjFilterMode(&texture, GX_NEAR, GX_NEAR);

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
	
    Wiimote wiimote;

    //printf("end init\r");
    BlockCoord pos(0, 0, 0);

    printf("end init\r");
	ChunkCache::init();
	ChunkCache::reset();
	
    while (!exiting) {

        player.renderer.camera.loadPerspective();

		player.renderer.renderSky();
		
        pos.x = floor(player.renderer.camera.pos.x);
        pos.y = floor(player.renderer.camera.pos.y);
        pos.z = floor(player.renderer.camera.pos.z);

        if (PRINT_PLAYER_POS)
        {
	        printf("pos : %d %d %d  ", pos.x & 15, pos.y &15, pos.z &15);
	        printf(">lk : %.2f %.2f %.2f\r", player.renderer.camera.look.x,
				   player.renderer.camera.look.y, player.renderer.camera.look.z);
        }

        // Calculate used memory
		u32 mem1 = SYS_GetArena1Size();
		u32 mem2 = SYS_GetArena2Size();
		printf("Memory : MEM1 %d (%d)\tMEM2 %d (%d)\r", mem1, mem1 / sizeof(VerticalChunk), mem2, mem2 / sizeof(VerticalChunk));
		
        player.renderer.camera.update(true);
		
        World::requestChunks(pos.toChunkCoord());
		
		printf("caching\r");
		
		ChunkCache::cache(player.renderer);
		
		printf("rendering\r");
		
		ChunkCache::render();
		
		printf("rendered\r");
		
        wiimote.update(player);
		
        if (player.creative)
            player.inventory.resetInventory();

        player.renderer.camera.loadOrtho(); // set for 2D drawing
        player.renderer.camera.applyTransform2D();

        if (player.isUnderwater(w)){
            guy.Underwater(player);
            printf("Underwater\r");
        }


        guy.renderInventory(player);
        guy.renderCursor(player, wiimote);



		Renderer::endFrame();

	}
	
	if (exiting == 2) SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	
	return 0;
}
