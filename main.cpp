//code by WinterMute
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <wiiuse/wpad.h>
#include <iostream>

#include "wiimote.h"
#include "engine/render/renderer.h"
#include "engine/render/camera.h"
#include "engine/render/light.h"

#include "texture.c"

#include "src/world/game.h"


int exiting = 0;

//Calling the function will end the while loop and properly exit the program to the HBChannel.
void reload(u32, void *) {
	exiting = 1;
}

//Calling the function will end the while loop and then properly shutdown the system
void shutdown() {
	exiting = 2;
}

TPLFile TPLfile;
GXTexObj texture;

void renderChunk(World& w, Renderer& renderer){
    t_coord pos(0,0,0);
    for(int offsetX = -1; offsetX<=1; offsetX ++){
        for(int offsetY= -1; offsetY<=1; offsetY++){
            for (int i = 0; i < 16; ++i) {

                pos.x = i+ offsetX * 16 + offsetY * 16;
                for (int j = 0; j < 128; ++j) {
                    pos.y = j;
                    for (int k = 0; k < 16; ++k) {
                        pos.z = k + offsetX * 16 + offsetY * 16;
                        if (w.getBlockAt(pos).type != BlockType::Air)
                        {
                            //printf("Start Rendering : x : %d, y: %d, z: %d\r", i, j, k);

                            renderer.renderBloc({static_cast<f32>(i + offsetX * 16 + offsetY * 16), static_cast<f32>(j), static_cast<f32>(k + offsetX * 16 + offsetY * 16)}, 1);
                            //printf("End Rendering : x : %d, y: %d, z: %d\r", i, j, k);

                        }
                    }
                }
            }
        }
    }


}


int main(int argc, char ** argv) {
	PAD_Init();
	WPAD_Init();
	
	Renderer::setupVideo();
	Renderer::setupVtxDesc();
	Renderer::setupMisc();
	

	//Light light;
	//GX_InvalidateTexAll();
	Renderer renderer;
	
	TPL_OpenTPLFromMemory(&TPLfile, (void *)texture_data, texture_sz);
	TPL_GetTexture(&TPLfile, 0, &texture);
    GX_InitTexObjLOD(&texture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
	GX_SetTevOp(GX_TEVSTAGE0,GX_MODULATE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	
	GX_LoadTexObj(&texture, GX_TEXMAP0);
	//GX_InitTexObjFilterMode(&texture, GX_NEAR, GX_NEAR);
	
	Wiimote wiimote;

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
	//SYS_STDIO_Report(true);
    renderer.camera.pos.z = 5;
    renderer.camera.pos.x = -0.5;
    renderer.camera.pos.y = 0.5;

    //pour rediriger stdout dans dolphin
    SYS_STDIO_Report(true);
    Game g;
    t_coord pos(0,0,0);
    World w = g.getWorld();

	while (!exiting) {

		//renderer.camera.rotateV(-0.10);
		//renderer.camera.rotateH(0.50);
		//camera.rotateH(1);
        wiimote.update(renderer);
		renderer.camera.update(false);
		//renderer.renderBloc({-1, 0, 0}, 1);
		//renderer.renderBloc({0, 0, -1}, 1);
		//renderer.renderBloc({0, -1, 0}, 1);
		renderer.renderBloc({1, 0, 0}, 1);
		renderer.renderBloc({0, 0, }, 1);
        //renderChunk(w, renderer);
		//renderer.renderBloc({4, 0, 0}, 1);
		//renderer.renderBloc({7, -1, 0}, 1);
		//renderer.renderBloc({8, 0, 0}, 1);
		//renderer.renderBloc({9, -1, 0}, 1);
		//renderer.renderBloc({1, -1, 0}, 2);
		//renderer.renderBloc({0, -1, 1}, 2);
		//renderer.renderBloc({0, 0, 1}, 3);


		//for (int X = -20; X < 20; X++) {
		//	for (int Z = -20; Z < 20; Z++) {
		//		renderer.renderBloc({static_cast<f32>(X), 0, static_cast<f32>(Z)}, 1);
		//	}
		//}
		//light.update(camera.viewMatrix);
		
		//testRender();
		//setupDebugConsole();
		
		//drawdot(rmode->fbWidth, rmode->xfbHeight, 0, 0, COLOR_YELLOW);

		//Renderer::setupDebugConsole();

		Renderer::endFrame();
	}
	
	if (exiting == 2) SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	
	return 0;
}
