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
#include "engine/render/render.h"


#include "src/world/game.h"
#include "engine/render/bloc.h"
#include "src/system/saveManager.h"
#include "player.h"


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


void renderChunk2(VerticalChunk& c, Renderer& renderer, t_pos2D pos){
	//int f[16][128][16][6];

	int px = pos.x << 4;
	int pz = pos.y << 4;
	int x, y, z;
	
    VerticalChunk& cnorth = *c.neighboors[CHUNK_NORTH];
    VerticalChunk& csouth = *c.neighboors[CHUNK_SOUTH];
    VerticalChunk& cest = *c.neighboors[CHUNK_EAST];
    VerticalChunk& cwest = *c.neighboors[CHUNK_WEST];

	for (y = 1; y < 127; y++) { // for each vertical levels (except 1st and last)
		
		// X 0 Z 0

		if (c.blocks[0][y][0].type > BlockType::Air) {

			renderer.renderBloc({(f32)px, (f32)y, (f32)pz}, c.blocks[0][y][0].type ,
				c.blocks[0][y+1][0].type <= BlockType::Air,
				c.blocks[0][y-1][0].type <= BlockType::Air ,
				cwest.blocks[15][y][0].type <= BlockType::Air,
				c.blocks[1][y][0].type <= BlockType::Air,
				c.blocks[0][y][1].type <= BlockType::Air,
				csouth.blocks[0][y][15].type <= BlockType::Air,
                                c.blocks[0][y+1][0].type <= BlockType::Air14,
                                c.blocks[0][y-1][0].type <= BlockType::Air14,
                                cwest.blocks[15][y][0].type <= BlockType::Air14,
                                c.blocks[1][y][0].type <= BlockType::Air14,
                                c.blocks[0][y][1].type <= BlockType::Air14,
                                csouth.blocks[0][y][15].type <= BlockType::Air14,
                                            c.blocks[0][y+1][0].type,
                                            c.blocks[0][y-1][0].type,
                                            cwest.blocks[15][y][0].type,
                                            c.blocks[1][y][0].type,
                                            c.blocks[0][y][1].type,
                                            csouth.blocks[0][y][15].type

				);
		}

		// X 0 Z 15

		if (c.blocks[0][y][15].type > BlockType::Air) {
			renderer.renderBloc({(f32)px, (f32)y, (f32)(15 + pz)}, c.blocks[0][y][15].type ,
				c.blocks[0][y+1][15].type <= BlockType::Air,
				c.blocks[0][y-1][15].type <= BlockType::Air,
				cwest.blocks[15][y][15].type<= BlockType::Air,
				c.blocks[1][y][15].type <= BlockType::Air,
				cnorth.blocks[0][y][0].type <= BlockType::Air,
				c.blocks[0][y][14].type <= BlockType::Air,
                                c.blocks[0][y+1][15].type <= BlockType::Air14,
                                c.blocks[0][y-1][15].type <= BlockType::Air14,
                                cwest.blocks[15][y][15].type <= BlockType::Air14,
                                c.blocks[1][y][15].type <= BlockType::Air14,
                                cnorth.blocks[0][y][0].type <= BlockType::Air14,
                                c.blocks[0][y][14].type <= BlockType::Air14,
                                            c.blocks[0][y+1][15].type,
                                            c.blocks[0][y-1][15].type,
                                            cwest.blocks[15][y][15].type,
                                            c.blocks[1][y][15].type,
                                            cnorth.blocks[0][y][0].type,
                                            c.blocks[0][y][14].type

				);
		}

		// X 15 Z 0

		if (c.blocks[15][y][0].type > BlockType::Air) {
			renderer.renderBloc({(f32)(15 + px), (f32)y, (f32)pz},c.blocks[15][y][0].type ,
				c.blocks[15][y+1][0].type <= BlockType::Air,
				c.blocks[15][y-1][0].type <= BlockType::Air,
				c.blocks[14][y][0].type <= BlockType::Air,
				cest.blocks[0][y][0].type <= BlockType::Air,
				c.blocks[15][y][1].type <= BlockType::Air,
				csouth.blocks[15][y][15].type <= BlockType::Air,
                                c.blocks[15][y+1][0].type <= BlockType::Air14,
                                c.blocks[15][y-1][0].type <= BlockType::Air14,
                                c.blocks[14][y][0].type <= BlockType::Air14,
                                cest.blocks[0][y][0].type <= BlockType::Air14,
                                c.blocks[15][y][1].type <= BlockType::Air14,
                                csouth.blocks[15][y][15].type <= BlockType::Air14,
                                            c.blocks[15][y+1][0].type,
                                            c.blocks[15][y-1][0].type,
                                            c.blocks[14][y][0].type,
                                            cest.blocks[0][y][0].type,
                                            c.blocks[15][y][1].type,
                                            csouth.blocks[15][y][15].type
				);
		}

		// X 15 Z 15

		if (c.blocks[15][y][15].type > BlockType::Air) {
			renderer.renderBloc({(f32)(15 + px), (f32)y, (f32)(15 + pz)}, c.blocks[15][y][15].type ,
				c.blocks[15][y+1][15].type <= BlockType::Air,
				c.blocks[15][y-1][15].type <= BlockType::Air,
				c.blocks[14][y][15].type <= BlockType::Air,
				cest.blocks[0][y][15].type <= BlockType::Air,
				cnorth.blocks[15][y][0].type <= BlockType::Air,
				c.blocks[15][y][14].type <= BlockType::Air,
                                c.blocks[15][y+1][15].type <= BlockType::Air14,
                                c.blocks[15][y-1][15].type <= BlockType::Air14,
                                c.blocks[14][y][15].type <= BlockType::Air14,
                                cest.blocks[0][y][15].type <= BlockType::Air14,
                                cnorth.blocks[15][y][0].type <= BlockType::Air14,
                                c.blocks[15][y][14].type <= BlockType::Air14,
                                            c.blocks[15][y+1][15].type,
                                            c.blocks[15][y-1][15].type,
                                            c.blocks[14][y][15].type,
                                            cest.blocks[0][y][15].type,
                                            cnorth.blocks[15][y][0].type,
                                            c.blocks[15][y][14].type
				);
		}


		// X 0

		for (z = 1; z < 15; z++)
			if (c.blocks[0][y][z].type > BlockType::Air) {
				renderer.renderBloc({(f32)px, (f32)y, (f32)(z + pz)}, c.blocks[0][y][z].type ,
					c.blocks[0][y+1][z].type <= BlockType::Air,
					c.blocks[0][y-1][z].type <= BlockType::Air,
					cwest.blocks[15][y][z].type <= BlockType::Air,
					c.blocks[1][y][z].type <= BlockType::Air,
					c.blocks[0][y][z+1].type <= BlockType::Air,
					c.blocks[0][y][z-1].type <= BlockType::Air,
                                        c.blocks[0][y+1][z].type <= BlockType::Air14,
                                        c.blocks[0][y-1][z].type <= BlockType::Air14,
                                        cwest.blocks[15][y][z].type <= BlockType::Air14,
                                        c.blocks[1][y][z].type <= BlockType::Air14,
                                        c.blocks[0][y][z+1].type <= BlockType::Air14,
                                        c.blocks[0][y][z-1].type <= BlockType::Air14,
                                                    c.blocks[0][y+1][z].type,
                                                    c.blocks[0][y-1][z].type,
                                                    cwest.blocks[15][y][z].type,
                                                    c.blocks[1][y][z].type,
                                                    c.blocks[0][y][z+1].type,
                                                    c.blocks[0][y][z-1].type
					);
			}

		// X 15

		for (z = 1; z < 15; z++)
			if (c.blocks[15][y][z].type > BlockType::Air) {
				renderer.renderBloc({(f32)(px + 15), (f32)y, (f32)(z + pz)}, c.blocks[15][y][z].type,
					c.blocks[15][y+1][z].type <= BlockType::Air,
					c.blocks[15][y-1][z].type <= BlockType::Air,
					c.blocks[14][y][z].type <= BlockType::Air,
					cest.blocks[0][y][z].type <= BlockType::Air,
					c.blocks[15][y][z+1].type <= BlockType::Air,
					c.blocks[15][y][z-1].type <= BlockType::Air,
                                        c.blocks[15][y+1][z].type <= BlockType::Air14,
                                        c.blocks[15][y-1][z].type <= BlockType::Air14,
                                        c.blocks[14][y][z].type <= BlockType::Air14,
                                        cest.blocks[0][y][z].type <= BlockType::Air14,
                                        c.blocks[15][y][z+1].type <= BlockType::Air14,
                                        c.blocks[15][y][z-1].type <= BlockType::Air14,
                                                        c.blocks[15][y+1][z].type,
                                                    c.blocks[15][y-1][z].type,
                                                    c.blocks[14][y][z].type,
                                                    cest.blocks[0][y][z].type,
                                                    c.blocks[15][y][z+1].type,
                                                    c.blocks[15][y][z-1].type

					);
			}

		// Z 0

		for (x = 1; x < 15; x++)
			if (c.blocks[x][y][0].type > BlockType::Air) {
				renderer.renderBloc({(f32)(x + px), (f32)y, (f32)pz}, c.blocks[x][y][0].type ,
					c.blocks[x][y+1][0].type <= BlockType::Air,
					c.blocks[x][y-1][0].type <= BlockType::Air,
					c.blocks[x - 1][y][0].type <= BlockType::Air,
					c.blocks[x + 1][y][0].type <= BlockType::Air,
					c.blocks[x][y][1].type <= BlockType::Air,
					csouth.blocks[x][y][15].type <= BlockType::Air,
                                        c.blocks[x][y+1][0].type <= BlockType::Air14,
                                        c.blocks[x][y-1][0].type <= BlockType::Air14,
                                        c.blocks[x - 1][y][0].type <= BlockType::Air14,
                                        c.blocks[x + 1][y][0].type <= BlockType::Air14,
                                        c.blocks[x][y][1].type <= BlockType::Air14,
                                        csouth.blocks[x][y][15].type <= BlockType::Air14,
                                                    c.blocks[x][y+1][0].type,
                                                    c.blocks[x][y-1][0].type,
                                                    c.blocks[x - 1][y][0].type,
                                                    c.blocks[x + 1][y][0].type,
                                                    c.blocks[x][y][1].type,
                                                    csouth.blocks[x][y][15].type
					);
			}

		// Z 15

		for (x = 1; x < 15; x++)
			if (c.blocks[x][y][15].type > BlockType::Air) {
				renderer.renderBloc({(f32)(x + px), (f32)y, (f32)(15 + pz)}, c.blocks[x][y][15].type ,
					c.blocks[x][y+1][15].type <= BlockType::Air,
					c.blocks[x][y-1][15].type <= BlockType::Air,
					c.blocks[x - 1][y][15].type <= BlockType::Air,
					c.blocks[x + 1][y][15].type <= BlockType::Air,
					cnorth.blocks[x][y][0].type <= BlockType::Air,
					c.blocks[x][y][14].type <= BlockType::Air,
                                        c.blocks[x][y+1][15].type <= BlockType::Air14,
                                        c.blocks[x][y-1][15].type <= BlockType::Air14,
                                        c.blocks[x - 1][y][15].type <= BlockType::Air14,
                                        c.blocks[x + 1][y][15].type <= BlockType::Air14,
                                        cnorth.blocks[x][y][0].type <= BlockType::Air14,
                                        c.blocks[x][y][14].type <= BlockType::Air14,
                                                    c.blocks[x][y+1][15].type,
                                                    c.blocks[x][y-1][15].type,
                                                    c.blocks[x - 1][y][15].type,
                                                    c.blocks[x + 1][y][15].type,
                                                    cnorth.blocks[x][y][0].type,
                                                    c.blocks[x][y][14].type
					);
			}


		for (x = 1; x < 15; x++) {
			for (z = 1; z < 15; z++) {
				if (c.blocks[x][y][z].type > BlockType::Air) {
					renderer.renderBloc({(f32)(x + px), (f32)y, (f32)(z + pz)}, c.blocks[x][y][z].type  ,
						c.blocks[x][y+1][z].type <= BlockType::Air,
						c.blocks[x][y-1][z].type <= BlockType::Air,
						c.blocks[x-1][y][z].type <= BlockType::Air,
						c.blocks[x+1][y][z].type <= BlockType::Air,
						c.blocks[x][y][z+1].type <= BlockType::Air,
						c.blocks[x][y][z-1].type <= BlockType::Air,
                                                c.blocks[x][y+1][z].type <= BlockType::Air14,
                                                c.blocks[x][y-1][z].type <= BlockType::Air14,
                                                c.blocks[x-1][y][z].type <= BlockType::Air14,
                                                c.blocks[x+1][y][z].type <= BlockType::Air14,
                                                c.blocks[x][y][z+1].type <= BlockType::Air14,
                                                c.blocks[x][y][z-1].type <= BlockType::Air14,
                                                            c.blocks[x][y+1][z].type,
                                                            c.blocks[x][y-1][z].type,
                                                            c.blocks[x-1][y][z].type,
                                                            c.blocks[x+1][y][z].type,
                                                            c.blocks[x][y][z+1].type,
                                                            c.blocks[x][y][z-1].type
						);
				}
			}
		}
	}
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
	PAD_Init();
	WPAD_Init();
	
	Renderer::setupVideo();
	Renderer::setupVtxDesc();
    Renderer::setupTexture();


	//Light light;
	//GX_InvalidateTexAll();
    Player player(8, 120, 8);

	//GX_InitTexObjFilterMode(&texture, GX_NEAR, GX_NEAR);
	//GX_SetTevIndTile()

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
	//SYS_STDIO_Report(true);

    //pour rediriger stdout dans dolphin
    SYS_STDIO_Report(true);

    Wiimote wiimote;


    t_coord pos(0,0,0);
    World& w = Game::getInstance()->getWorld();

    //printf("end init\r");
    while (!exiting) {

        player.renderer.camera.loadPerspective();

        pos.x = floor(player.renderer.camera.pos.x);
        pos.y = floor(player.renderer.camera.pos.y);
        pos.z = floor(player.renderer.camera.pos.z);

        //printf("pos : %d %d %d\r", pos.x & 15, pos.y &15, pos.z &15);
        //printf(">lk : %.2f %.2f %.2f\r", player.renderer.camera.look.x,
		//	   player.renderer.camera.look.y, player.renderer.camera.look.z);




        player.renderer.camera.update(true);
		
        Game::getInstance()->requestChunk(w.to_chunk_pos(pos));

        renderWorld(w, player.renderer, w.to_chunk_pos(pos));
        wiimote.update(player, w);

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

        u32 white = 0xFFFFFFFF;

        player.renderer.camera.loadOrtho(); // set for 2D drawing
        player.renderer.camera.applyTransform2D();
        f32 x,y, a, b;
        x = 0.05, y = 0.05;

        // inventory

        if (player.inventory_open){
            GX_Begin(GX_QUADS, GX_VTXFMT0, 148); // Start drawing

            GX_Position3f32(-0.8,0.65,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(2)); // Top left

            GX_Position3f32(0.8,0.65,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(27), BLOCK_COORD(2)); // Top right

            GX_Position3f32(0.8,-0.65,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(27), BLOCK_COORD(12)); // Bottom right

            GX_Position3f32(-0.8,-0.65,0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(12)); // Bottom left

            for ( int i = 0; i <27 ; i++){
                GX_Position3f32(-0.682 + (i % 9) * 0.158 ,-0.02 - (i / 9) * 0.142,0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(i%9), BLOCK_COORD(i/9)); // Top left

                GX_Position3f32(-0.582+ i % 9 * 0.158,-0.02 - (i / 9) * 0.142,0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(i%9 + 1), BLOCK_COORD(i/9)); // Top right

                GX_Position3f32(-0.582 + i % 9 * 0.158,-0.12 - (i / 9) * 0.142,0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(i%9 + 1), BLOCK_COORD(i/9 +1)); // Bottom right

                GX_Position3f32(-0.682 + i % 9 * 0.158,-0.12 - (i / 9) * 0.142,0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD(i%9), BLOCK_COORD(i/9 +1)); // Bottom left
            }

            for (int i = 0; i < 9; i++){
                GX_Position3f32(-0.682 + i * 0.158 ,-0.475,0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD((27 + i)%9), BLOCK_COORD((27+i)/9)); // Top left

                GX_Position3f32(-0.582+ i * 0.158,-0.475 ,0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD((27 + i)%9 + 1), BLOCK_COORD((27+i)/9)); // Top right

                GX_Position3f32(-0.582 + i  * 0.158,-0.575 ,0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD((27 + i)%9 + 1), BLOCK_COORD((27+i)/9 +1)); // Bottom right

                GX_Position3f32(-0.682 + i  * 0.158,-0.575 ,0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(BLOCK_COORD((27 + i)%9), BLOCK_COORD((27+i)/9 +1)); // Bottom left
            }


        }

        else {


            GX_Begin(GX_QUADS, GX_VTXFMT0, 44); // Start drawing

            GX_Position3f32(-0.47, -0.6, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(0)); // Top left

            GX_Position3f32(0.47, -0.6, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(25), BLOCK_COORD(0)); // Top right

            GX_Position3f32(0.47, -0.7, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(25), BLOCK_COORD(1)); // Bottom right

            GX_Position3f32(-0.47, -0.7, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(1)); // Bottom left





            GX_Position3f32(-0.43 + 0.094 * player.selected_spot, -0.6, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(1)); // Top left

            GX_Position3f32(-0.32 + 0.094 * player.selected_spot, -0.6, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(17), BLOCK_COORD(1)); // Top right

            GX_Position3f32(-0.32 + 0.094 * player.selected_spot, -0.7, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(17), BLOCK_COORD(2)); // Bottom right

            GX_Position3f32(-0.43 + 0.094 * player.selected_spot, -0.7, 0);
            GX_Normal1x8(4);
            GX_Color1u32(white);
            GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(2)); // Bottom left




            for (int i = 0; i < 9; i++) {
                GX_Position3f32(-0.40 + i * 0.094, -0.62, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.hotbar[i]].x[0], blocData[player.hotbar[i]].y[0]); // Top left

                GX_Position3f32(-0.35 + i * 0.094, -0.62, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.hotbar[i]].x[0] + OFFSET, blocData[player.hotbar[i]].y[0]); // Top right

                GX_Position3f32(-0.35 + i * 0.094, -0.68, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.hotbar[i]].x[0] + OFFSET,
                                blocData[player.hotbar[i]].y[0] + OFFSET); // Bottom right

                GX_Position3f32(-0.40 + i * 0.094, -0.68, 0);
                GX_Normal1x8(4);
                GX_Color1u32(white);
                GX_TexCoord2f32(blocData[player.hotbar[i]].x[0],
                                blocData[player.hotbar[i]].y[0] + OFFSET); // Bottom left
            }
        }

        GX_End();


        GX_Begin(GX_QUADS, GX_VTXFMT0, 8); // Start drawing

        GX_Position3f32(-x, y, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(15)); // Top left

        GX_Position3f32(x, y, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(15)); // Top right

        GX_Position3f32(x, -y, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(16)); // Bottom right

        GX_Position3f32(-x, -y, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
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
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(15)); // Top left

        GX_Position3f32(x+a, y+b, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(15)); // Top right

        GX_Position3f32(x+a, -y+b, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(16)); // Bottom right

        GX_Position3f32(-x+a, -y+b, 0);
        GX_Normal1x8(4);
        GX_Color1u32(white);
        GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(16)); // Bottom left

        GX_End();

		Renderer::endFrame();
        //printf("end frame\r");
	}
	
	if (exiting == 2) SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	
	return 0;
}
