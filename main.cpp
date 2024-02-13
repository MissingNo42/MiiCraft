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


#include "src/world/game.h"
#include "engine/render/block.h"
#include "src/system/saveManager.h"


int exiting = 0;

//Calling the function will end the while loop and properly exit the program to the HBChannel.
void reload(u32, void *) {
	exiting = 1;
}

//Calling the function will end the while loop and then properly shutdown the system
void shutdown() {
	exiting = 2;
}


void renderChunk(VerticalChunk& c, Renderer& renderer, t_pos2D pos){
	//int f[16][128][16][6];

	int px = pos.x << 4;
	int pz = pos.y << 4;
	int x, y, z;
	
    VerticalChunk& cnorth = *c.neighboors[CHUNK_NORTH];
    VerticalChunk& csouth = *c.neighboors[CHUNK_SOUTH];
    VerticalChunk& cest = *c.neighboors[CHUNK_EST];
    VerticalChunk& cwest = *c.neighboors[CHUNK_WEST];

	for (y = 1; y < 127; y++) { // for each vertical levels (except 1st and last)
		
		// X 0 Z 0

		if (c.blocks[0][y][0].type != BlockType::Air) {

			renderer.renderBloc({(f32)px, (f32)y, (f32)pz}, c.blocks[0][y][0].type ,
				c.blocks[0][y+1][0].type == BlockType::Air,
				c.blocks[0][y-1][0].type == BlockType::Air,
				cwest.blocks[15][y][0].type == BlockType::Air,
				c.blocks[1][y][0].type == BlockType::Air,
				c.blocks[0][y][1].type == BlockType::Air,
				csouth.blocks[0][y][15].type == BlockType::Air
				);
		}

		// X 0 Z 15

		if (c.blocks[0][y][15].type != BlockType::Air) {
			renderer.renderBloc({(f32)px, (f32)y, (f32)(15 + pz)}, c.blocks[0][y][15].type ,
				c.blocks[0][y+1][15].type == BlockType::Air,
				c.blocks[0][y-1][15].type == BlockType::Air,
				cwest.blocks[15][y][15].type == BlockType::Air,
				c.blocks[1][y][15].type == BlockType::Air,
				cnorth.blocks[0][y][0].type == BlockType::Air,
				c.blocks[0][y][14].type == BlockType::Air
				);
		}

		// X 15 Z 0

		if (c.blocks[15][y][0].type != BlockType::Air) {
			renderer.renderBloc({(f32)(15 + px), (f32)y, (f32)pz},c.blocks[15][y][0].type ,
				c.blocks[15][y+1][0].type == BlockType::Air,
				c.blocks[15][y-1][0].type == BlockType::Air,
				c.blocks[14][y][0].type == BlockType::Air,
				cest.blocks[0][y][0].type == BlockType::Air,
				c.blocks[15][y][1].type == BlockType::Air,
				csouth.blocks[15][y][15].type == BlockType::Air
				);
		}

		// X 15 Z 15

		if (c.blocks[15][y][15].type != BlockType::Air) {
			renderer.renderBloc({(f32)(15 + px), (f32)y, (f32)(15 + pz)}, c.blocks[15][y][15].type ,
				c.blocks[15][y+1][15].type == BlockType::Air,
				c.blocks[15][y-1][15].type == BlockType::Air,
				c.blocks[14][y][15].type == BlockType::Air,
				cest.blocks[0][y][15].type == BlockType::Air,
				cnorth.blocks[15][y][0].type == BlockType::Air,
				c.blocks[15][y][14].type == BlockType::Air
				);
		}


		// X 0

		for (z = 1; z < 15; z++)
			if (c.blocks[0][y][z].type != BlockType::Air) {
				renderer.renderBloc({(f32)px, (f32)y, (f32)(z + pz)}, c.blocks[0][y][z].type ,
					c.blocks[0][y+1][z].type == BlockType::Air,
					c.blocks[0][y-1][z].type == BlockType::Air,
					cwest.blocks[15][y][z].type == BlockType::Air,
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
					cest.blocks[0][y][z].type == BlockType::Air,
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
					csouth.blocks[x][y][15].type == BlockType::Air
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
					cnorth.blocks[x][y][0].type == BlockType::Air,
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


void renderWorld(World& w, Renderer& renderer, t_pos2D posCam) {
	t_pos2D pos;
	for (pos.x = posCam.x-1; pos.x < posCam.x + 2; pos.x++) {
		for (pos.y = posCam.y - 1 ;  pos.y < posCam.y + 2 ; pos.y++) {
			renderChunk(w.getChunkAt(pos), renderer, pos);
		}
	}
}

int main(int, char **) {
	
	WPAD_Init();
	
    //pour rediriger stdout dans dolphin
    SYS_STDIO_Report(true);
	
	Renderer::setupVideo();
	Renderer::setupVtxDesc();
	Renderer::setupTexture();
	
	//Light light;
	//GX_InvalidateTexAll();
	Renderer renderer;
	
	//GX_SetTevIndTile()

	Wiimote wiimote;

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
	//SYS_STDIO_Report(true);
    renderer.camera.pos.z = 5;
    renderer.camera.pos.x = -0.5;
    renderer.camera.pos.y = 0.5;

    t_coord pos(0,0,0);
    World& w = Game::getInstance()->getWorld();
    renderer.camera.pos.y = 40;
    renderer.camera.pos.x = 8;
    renderer.camera.pos.z = 8;

	renderer.camera.pos = {8, 40, 9};

    while (!exiting) {
		f32 x = 0.1, y = 0.1;
		
		renderer.camera.loadPerspective(); // restore for next 3D
		
        pos.x = renderer.camera.pos.x;
        pos.y = renderer.camera.pos.y;
        pos.z = renderer.camera.pos.z;
        Game::getInstance()->requestChunk(w.to_chunk_pos(pos));

		//renderer.camera.rotateV(-0.10);
		//renderer.camera.rotateH(0.50);
		//camera.rotateH(1);
        wiimote.update(renderer.camera);
		renderer.camera.update(false);

        renderWorld(w, renderer, w.to_chunk_pos(pos));
		
		
		renderer.camera.loadOrtho(); // set for 2D
		
		renderer.camera.applyTransform2D();
		x = 0.1, y = 0.1;
		GX_Begin(GX_QUADS, GX_VTXFMT0, 8); // Start drawing
		
		GX_Position3f32(-x, y, 0);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(BLOCK_COORD(11), BLOCK_COORD(10)); // Top left
		
		GX_Position3f32(x, y, 0);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(BLOCK_COORD(12), BLOCK_COORD(10)); // Top right
		
		GX_Position3f32(x, -y, 0);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(BLOCK_COORD(12), BLOCK_COORD(9)); // Bottom right
		
		GX_Position3f32(-x, -y, 0);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(BLOCK_COORD(11), BLOCK_COORD(9)); // Bottom left
		
		f32 a = 0, b = 0;
		auto wd = wiimote.wd;
		if (wd->ir.valid) {
			a = wd->ir.x / (f32)Renderer::rmode->fbWidth - .5;
			b = -wd->ir.y / (f32)Renderer::rmode->xfbHeight + .5;
		}
		a -= x / 2;
		b += y / 2;
		
		GX_Position3f32(-x+a, y+b, 0);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(BLOCK_COORD(11), BLOCK_COORD(10)); // Top left
		
		GX_Position3f32(x+a, y+b, 0);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(BLOCK_COORD(12), BLOCK_COORD(10)); // Top right
		
		GX_Position3f32(x+a, -y+b, 0);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(BLOCK_COORD(12), BLOCK_COORD(9)); // Bottom right
		
		GX_Position3f32(-x+a, -y+b, 0);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(BLOCK_COORD(11), BLOCK_COORD(9)); // Bottom left

		GX_End();

		Renderer::endFrame();
	}
	
	if (exiting == 2) SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	
	return 0;
}
