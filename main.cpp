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
#include "engine/render/bloc.h"
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

TPLFile TPLfile;
GXTexObj texture;


void SetLightDiff(int i, guVector vecf, float d, float d1, int i1);

void renderChunk(VerticalChunk& c, Renderer& renderer, t_pos2D pos){
	//int f[16][128][16][6];

	int px = pos.x << 4;
	int pz = pos.y << 4;
	int x, y, z;
	
    VerticalChunk& cnorth = *c.neighboors[CHUNK_NORTH];
    VerticalChunk& csouth = *c.neighboors[CHUNK_SOUTH];
    VerticalChunk& cest = *c.neighboors[CHUNK_EST];
    VerticalChunk& cwest = *c.neighboors[CHUNK_WEST];

	for (y = 126; y > 1; y--) { // for each vertical levels (except 1st and last)
		
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
	PAD_Init();
	WPAD_Init();
	
	Renderer::setupVideo();
	Renderer::setupVtxDesc3D();
	Renderer::setupMisc();
	

	//Light light;
	GX_InvalidateTexAll();
	Renderer renderer;
	
	TPL_OpenTPLFromMemory(&TPLfile, (void *)texture_data, texture_sz);
	TPL_GetTexture(&TPLfile, 0, &texture);
    GX_InitTexObjLOD(&texture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
//	GX_SetTevOp(GX_TEVSTAGE0,GX_MODULATE);
//	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
//
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
    World& w = Game::getInstance()->getWorld();
    renderer.camera.pos.y = 40;
    renderer.camera.pos.x = 8;
    renderer.camera.pos.z = 8;

    /*
    GX_InitLightPos(&light, 80, 300, 80);
    GX_InitLightDistAttn(&light, 100, 1.0f, GX_DA_GENTLE);
    //GX_InitLightSpot(&light, 30, GX_SP_FLAT);
    GX_InitLightColor(&light, (GXColor){255, 255, 255, 255});
    GX_InitLightShininess(&light, 64);
    GX_LoadLightObj(&light, GX_LIGHT0);
    GX_SetChanCtrl(GX_COLOR0A0, GX_TRUE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
*/

    f32 l2 = 0.0f;



    while (!exiting) {




        pos.x = renderer.camera.pos.x;
        pos.y = renderer.camera.pos.y;
        pos.z = renderer.camera.pos.z;
        Game::getInstance()->requestChunk(w.to_chunk_pos(pos));

		//renderer.camera.rotateV(-0.10);
		//renderer.camera.rotateH(0.50);
		//camera.rotateH(1);
        wiimote.update(renderer);
		renderer.camera.update(false);


//
//        GXLightObj MyLight;
//        guVector lpos = {(f32)pos.x, (f32)pos.y, (f32)pos.z};
//
//        GXColor lightColor = {0, 255, 255, 255};
//
//        f32 brightness = 1.0f;
//        f32 distattn = 20.0f;
//
//        guVecMultiply(renderer.camera.viewMatrix, &lpos, &lpos);
//        GX_InitLightPos(&MyLight, lpos.x, lpos.y, lpos.z);
//        GX_InitLightColor(&MyLight, lightColor);
//        GX_InitLightSpot(&MyLight, 90.0f, GX_SP_OFF);
//        GX_InitLightDistAttn(&MyLight, distattn, brightness, GX_DA_MEDIUM); // DistAttn = 20.0  &  Brightness=1.0f (full)
//        GX_LoadLightObj(&MyLight, (1<<0));
//
//        // Turn light ON
//        GX_SetNumChans(1);
//        GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_CLAMP, GX_AF_SPOT);
//
//

        GX_SetChanAmbColor(GX_COLOR0A0, (GXColor) { 0x33, 0x33, 0x33, 0xFF});

        Renderer::setupVtxDesc3D();

        //Set lightoff
        GX_SetNumTevStages(1);
        GX_SetTevOp  (GX_TEVSTAGE0, GX_PASSCLR);
        GX_SetNumChans(1);
        GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);


        GXLightObj MyLight;
        guVector lpos = {renderer.camera.pos.x, renderer.camera.pos.y, renderer.camera.pos.z};

        //GRRLIB_Settings.lights |= (1<<num);

        f32 distattn = 900.0f;
        f32 brightness = 1.0f;

        guVecMultiply(renderer.camera.viewMatrix, &lpos, &lpos);
        GX_InitLightPos(&MyLight, 8.0f, 50.0f, 8.0f);
        GX_InitLightColor(&MyLight, (GXColor) { 0xff, 0xff, 0xff, 0xFF });
        GX_InitLightDir(&MyLight, -1.0f, -1.0f, -1.0f);
        GX_InitLightSpot(&MyLight, 45.0f, GX_SP_COS);
        GX_InitLightDistAttn(&MyLight, distattn, brightness, GX_DA_MEDIUM); // DistAttn = 20.0  &  Brightness=1.0f (full)
        GX_LoadLightObj(&MyLight, (1<<0));

        // Turn light ON
        GX_SetNumChans(1);
        GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, 1<<0, GX_DF_CLAMP, GX_AF_SPOT);



        renderWorld(w, renderer, w.to_chunk_pos(pos));



        //renderer.renderBloc({4, 0, 0}, 1);
		//renderer.renderBloc({7, -1, 0}, 1);
		
		
		
		
		//Renderer::setupVtxDesc2D();
		//
		//Mtx GXmodelView2D, perspective;
	    //guOrtho(perspective,0,479,0,639,0,300);
	    //GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);
		//guMtxIdentity(GXmodelView2D);
		//guMtxTransApply (GXmodelView2D, GXmodelView2D, 0.0F, 0.0F, -5.0F);
		//GX_LoadPosMtxImm(GXmodelView2D,GX_PNMTX0);
		//GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Start drawing
	
		//GX_Position2f32(0, 0);
		//GX_TexCoord2f32(OFFSET, OFFSET); // Top right
		//
		//GX_Position2f32(100, 0);
		//GX_TexCoord2f32(0, OFFSET); // Top left
		//
		//GX_Position2f32(100, 100);
		//GX_TexCoord2f32(0, 0); // Bottom left
		//
		//GX_Position2f32(0, 100);
		//GX_TexCoord2f32(OFFSET, 0); // Bottom right
	
		//GX_End();

		
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

