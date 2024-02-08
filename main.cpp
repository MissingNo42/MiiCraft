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
    Block b;
    for(int offsetX = 0; offsetX<=2; offsetX ++){
        for(int offsetY= 0; offsetY<=2; offsetY++){
            for (int i = 0; i < 16; ++i) {
                pos.x = i+ offsetX * 16;
                for (int j = 0; j < 128; ++j) {
                    pos.y = j;
                    for (int k = 0; k < 16; ++k) {
                        pos.z = k + offsetY * 16;
                        b = w.getBlockAt(pos);
                        if (b.type != BlockType::Air)
                        {
                            renderer.renderBloc({static_cast<f32>(i + offsetX * 16), static_cast<f32>(j), static_cast<f32>(k +  offsetY * 16)}, b.type, true, true, true, true, true, true);

                        }
                    }
                }
            }
        }
    }
}



void renderChunk(VerticalChunk& c, Renderer& renderer, t_pos2D pos){
	//int f[16][128][16][6];
	
	int px = pos.x << 4;
	int pz = pos.y << 4;
	int x, y, z;
	
	for (y = 0; y < 128; y++) { // for each vertical levels
		
		// X 0 Z 0
		
		if (c.blocks[0][y][0].type != BlockType::Air) {

			renderer.renderBloc({(f32)px, (f32)y, (f32)pz}, c.blocks[0][y][0].type ,
				c.blocks[0][y+1][0].type == BlockType::Air,
				c.blocks[0][y-1][0].type == BlockType::Air,
				true,
				c.blocks[1][y][0].type == BlockType::Air,
				c.blocks[0][y][1].type == BlockType::Air,
				true
				);
		}
		
		// X 0 Z 15
		
		if (c.blocks[0][y][15].type != BlockType::Air) {
			renderer.renderBloc({(f32)px, (f32)y, (f32)(15 + pz)}, c.blocks[0][y][15].type ,
				c.blocks[0][y+1][15].type == BlockType::Air,
				c.blocks[0][y-1][15].type == BlockType::Air,
				true,
				c.blocks[1][y][15].type == BlockType::Air,
				true,
				c.blocks[0][y][14].type == BlockType::Air
				);
		}
		
		// X 15 Z 0
		
		if (c.blocks[15][y][0].type != BlockType::Air) {
			renderer.renderBloc({(f32)(15 + px), (f32)y, (f32)pz},c.blocks[15][y][0].type ,
				c.blocks[15][y+1][0].type == BlockType::Air,
				c.blocks[15][y-1][0].type == BlockType::Air,
				c.blocks[14][y][0].type == BlockType::Air,
				true,
				c.blocks[15][y][1].type == BlockType::Air,
				true
				);
		}
		
		// X 15 Z 15
		
		if (c.blocks[15][y][15].type != BlockType::Air) {
			renderer.renderBloc({(f32)(15 + px), (f32)y, (f32)(15 + pz)}, c.blocks[15][y][15].type ,
				c.blocks[15][y+1][15].type == BlockType::Air,
				c.blocks[15][y-1][15].type == BlockType::Air,
				c.blocks[14][y][15].type == BlockType::Air,
				true,
				true,
				c.blocks[15][y][14].type == BlockType::Air
				);
		}
		
		
		// X 0
		
		for (z = 1; z < 15; z++)
			if (c.blocks[0][y][z].type != BlockType::Air) {
				renderer.renderBloc({(f32)px, (f32)y, (f32)(z + pz)}, c.blocks[0][y][z].type ,
					c.blocks[0][y+1][z].type == BlockType::Air,
					c.blocks[0][y-1][z].type == BlockType::Air,
					true,
					c.blocks[1][y][z].type == BlockType::Air,
					c.blocks[0][y][z+1].type == BlockType::Air,
					c.blocks[0][y][z-1].type == BlockType::Air
					);
			}
		
		// X 15
		
		for (z = 1; z < 15; z++)
			if (c.blocks[15][y][z].type != BlockType::Air) {
				renderer.renderBloc({(f32)(px + 15), (f32)y, (f32)(z + pz)}, c.blocks[15][y][z].type,
					c.blocks[15][y+1][z].type == BlockType::Air,
					c.blocks[15][y-1][z].type == BlockType::Air,
					c.blocks[14][y][z].type == BlockType::Air,
					true,
					c.blocks[15][y][z+1].type == BlockType::Air,
					c.blocks[15][y][z-1].type == BlockType::Air
					);
			}
		
		// Z 0
		
		for (x = 1; x < 15; x++)
			if (c.blocks[x][y][0].type != BlockType::Air) {
				renderer.renderBloc({(f32)(x + px), (f32)y, (f32)pz}, c.blocks[x][y][0].type ,
					c.blocks[x][y+1][0].type == BlockType::Air,
					c.blocks[x][y-1][0].type == BlockType::Air,
					c.blocks[x - 1][y][0].type == BlockType::Air,
					c.blocks[x + 1][y][0].type == BlockType::Air,
					c.blocks[x][y][1].type == BlockType::Air,
					true
					);
			}
		
		// Z 15
		
		for (x = 1; x < 15; x++)
			if (c.blocks[x][y][15].type != BlockType::Air) {
				renderer.renderBloc({(f32)(x + px), (f32)y, (f32)(15 + pz)}, c.blocks[x][y][15].type ,
					c.blocks[x][y+1][15].type == BlockType::Air,
					c.blocks[x][y-1][15].type == BlockType::Air,
					c.blocks[x - 1][y][15].type == BlockType::Air,
					c.blocks[x + 1][y][15].type == BlockType::Air,
					true,
					c.blocks[x][y][14].type == BlockType::Air
					);
			}
		
		
		for (x = 1; x < 15; x++) {
			for (z = 1; z < 15; z++) {
				if (c.blocks[x][y][z].type != BlockType::Air) {
					renderer.renderBloc({(f32)(x + px), (f32)y, (f32)(z + pz)}, c.blocks[x][y][z].type  ,
						c.blocks[x][y+1][z].type == BlockType::Air,
						c.blocks[x][y-1][z].type == BlockType::Air,
						c.blocks[x-1][y][z].type == BlockType::Air,
						c.blocks[x+1][y][z].type == BlockType::Air,
						c.blocks[x][y][z+1].type == BlockType::Air,
						c.blocks[x][y][z-1].type == BlockType::Air
						);
				}
			}
		}
	}
}




void renderWorld(World& w, Renderer& renderer) {
	t_pos2D pos;
	for (pos.x = 0; pos.x <= 3; pos.x++) {
		for (pos.y = 0; pos.y <= 3; pos.y++) {
			renderChunk(w.getChunkAt(pos), renderer, pos);
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
	//GX_SetTevIndTile()

	Wiimote wiimote;

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
	//SYS_STDIO_Report(true);
    renderer.camera.pos.z = 5;
    renderer.camera.pos.x = -0.5;
    renderer.camera.pos.y = 0.5;

    //pour rediriger stdout dans dolphin
    SYS_STDIO_Report(true);

    t_coord pos(0,0,0);
    World w = Game::getInstance()->getWorld();
    renderer.camera.pos.y = 30;
    renderer.camera.pos.x = 48;
    renderer.camera.pos.z = 48;
	while (!exiting) {

		//renderer.camera.rotateV(-0.10);
		//renderer.camera.rotateH(0.50);
		//camera.rotateH(1);
        wiimote.update(renderer);
		renderer.camera.update(false);
		//renderer.renderBloc({-1, 0, 0}, 1);
		//renderer.renderBloc({0, 0, -1}, 1);
		//renderer.renderBloc({0, -1, 0}, 1);
		//renderer.renderBloc({1, 0, 0}, 1);
		//renderer.renderBloc({0, 0, }, 1);
		//renderer.renderBloc({-1, 0, 0}, 2, true, true, true, true, true, true);
		//renderer.renderBloc({0, 0, 0}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({1, 0, 0}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({2, 0, 0}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({3, 0, 0}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({0, 1, 0}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({0, 2, 0}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({0, 3, 0}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({0, 0, 1}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({0, 0, 2}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({0, 0, 3}, 1, true, true, true, true, true, true);
		//renderer.renderBloc({0, -1, 0}, 1, false, true, true, true, true, true);
		//renderer.renderBloc({1, 0, 0}, 1, false, true, true, true, true, true);
		//renderer.renderBloc({0, 0, 1}, 1, false, true, true, true, true, true);
        renderWorld(w, renderer);
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
