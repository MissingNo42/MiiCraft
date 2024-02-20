#include <cstdio>
#include <gccore.h>
#include <cmath>
#include <wiiuse/wpad.h>
#include <iostream>


#include "wiimote.h"
#include "engine/render/renderer.h"
#include "engine/render/render.h"


#include "src/world/game.h"
#include "engine/render/bloc.h"
#include "src/system/saveManager.h"
#include "player.h"
#include "render/cacheManager.h"


int exiting = 0;

//Calling the function will end the while loop and properly exit the program to the HBChannel.
void reload(u32, void *) {
	exiting = 1;
}

//Calling the function will end the while loop and then properly shutdown the system
void shutdown() {
	exiting = 2;
}



void renderWorld(World& w, Renderer& renderer, t_pos2D posCam) {
//	t_pos2D pos;
//	for (pos.x = posCam.x-1; pos.x < posCam.x + 2; pos.x++) {
//		for (pos.y = posCam.y - 1 ;  pos.y < posCam.y + 2 ; pos.y++) {
//			renderChunk(w.getChunkAt(pos), renderer, pos);
//		}
//	}
    t_pos2D pos;

        for (pos.x = posCam.x - 1; pos.x < posCam.x + 2; pos.x++) {
            for (pos.y = posCam.y - 1; pos.y < posCam.y + 2; pos.y++)
                renderChunk(w.getChunkAt(pos), renderer, pos);
        }

}

int main(int, char **) {
    SYS_STDIO_Report(true);
	
	PAD_Init();
	WPAD_Init();
	
	Renderer::setupVideo();
	Renderer::setupVtxDesc();
    Renderer::setupTexture();
	
	//GX_InvalidateTexAll();
    Player player(8, 120, 8);

	//GX_InitTexObjFilterMode(&texture, GX_NEAR, GX_NEAR);

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
	
    Wiimote wiimote;

    //printf("end init\r");
    t_coord pos(0,0,0);
    World& w = Game::getInstance()->getWorld();

    printf("end init\r");
	ChunkCache::reset();
	
    while (!exiting) {

        player.renderer.camera.loadPerspective();

        pos.x = floor(player.renderer.camera.pos.x);
        pos.y = floor(player.renderer.camera.pos.y);
        pos.z = floor(player.renderer.camera.pos.z);

        printf("pos : %d %d %d\r", pos.x & 15, pos.y &15, pos.z &15);
        printf(">lk : %.2f %.2f %.2f\r", player.renderer.camera.look.x,
			   player.renderer.camera.look.y, player.renderer.camera.look.z);

        // Calculate used memory
		u32 mem1 = SYS_GetArena1Size();
		u32 mem2 = SYS_GetArena2Size();
		printf("Memory : MEM1 %d (%d)\tMEM2 %d (%d)\r", mem1, mem1 / sizeof(VerticalChunk), mem2, mem2 / sizeof(VerticalChunk));

        wiimote.update(player, w);

        player.renderer.camera.update(true);
		
        Game::getInstance()->requestChunk(w.to_chunk_pos(pos));
		printf("caching\r");
        //renderWorld(w, player.renderer, w.to_chunk_pos(pos));
		
		ChunkCache::cache(w, player.renderer);
		
		printf("rendering\r");
		
		ChunkCache::render();
		
		printf("rendered\r");
		
        u32 white = 0xFFFFFFFF;

        player.renderer.camera.loadOrtho(); // set for 2D drawing
        player.renderer.camera.applyTransform2D(false);
        f32 x,y, a, b;
        x = 0.05, y = 0.05;
		
        GX_Begin(GX_QUADS, GX_VTXFMT0, 8); // Start drawing

        GX_Position3f32(-x, y, 0);
        GX_Color1x8(WHITE);
        GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(15)); // Top left

        GX_Position3f32(x, y, 0);
        GX_Color1x8(WHITE);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(15)); // Top right

        GX_Position3f32(x, -y, 0);
        GX_Color1x8(WHITE);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(16)); // Bottom right

        GX_Position3f32(-x, -y, 0);
        GX_Color1x8(WHITE);
        GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(16)); // Bottom left

        a = 0, b = 0;
        auto wd = wiimote.wd;
        if (wd->ir.valid) {
            a = 2 * wd->ir.x / (f32)Renderer::rmode->fbWidth - 1;
            b = -2 * wd->ir.y / (f32)Renderer::rmode->xfbHeight + 1;
        }
        a -= x / 2;
        b += y / 2;

        GX_Position3f32(-x+a, y+b, 0);
        GX_Color1x8(WHITE);
        GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(15)); // Top left

        GX_Position3f32(x+a, y+b, 0);
        GX_Color1x8(WHITE);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(15)); // Top right

        GX_Position3f32(x+a, -y+b, 0);
        GX_Color1x8(WHITE);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(16)); // Bottom right

        GX_Position3f32(-x+a, -y+b, 0);
        GX_Color1x8(WHITE);
        GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(16)); // Bottom left

        GX_End();

		Renderer::endFrame();
	}
	
	if (exiting == 2) SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	
	return 0;
}
