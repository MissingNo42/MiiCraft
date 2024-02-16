//
// Created by Romain on 15/02/2024.
//

#include "engine/render/render.h"
#include "world/verticalChunk.h"
#include "render/renderer.h"
#include "render/cache.h"

static int LightLevel = 0;

inline void renderVertex(f32 x, f32 y, f32 z, f32 tx, f32 ty, u32 color, u8 normal) {
	GX_Position3f32(x, y, z);
	GX_Normal1x8(normal);
	GX_Color1u32(color);
	GX_TexCoord2f32(tx, ty);
}

inline void renderFront(f32 x, f32 y, f32 z, f32 mx, f32 my, f32 mz, f32 tx, f32 ty, u8 c1, u8 c2, u8 c3, u8 c4) {
	renderVertex(mx, y, z, tx, ty, Lights[LightLevel][c1], 4); // A
	renderVertex(x, y, z, tx + OFFSET, ty, Lights[LightLevel][c2], 4); // D
	renderVertex(x, my, z, tx + OFFSET, ty + OFFSET, Lights[LightLevel][c3], 4); // H
	renderVertex(mx, my, z, tx, ty + OFFSET, Lights[LightLevel][c4], 4); // E
}

inline void renderBack(f32 x, f32 y, f32 z, f32 mx, f32 my, f32 mz, f32 tx, f32 ty, u8 c1, u8 c2, u8 c3, u8 c4) {
	renderVertex(x, my, mz, tx, ty + OFFSET, Lights[LightLevel][c1], 5); // G
	renderVertex(x, y, mz, tx, ty, Lights[LightLevel][c2], 5); // C
	renderVertex(mx, y, mz, tx + OFFSET, ty, Lights[LightLevel][c3], 5); // B
	renderVertex(mx, my, mz, tx + OFFSET, ty + OFFSET, Lights[LightLevel][c4], 5); // F
}

inline void renderTop(f32 x, f32 y, f32 z, f32 mx, f32 my, f32 mz, f32 tx, f32 ty, u8 c1, u8 c2, u8 c3, u8 c4) {
	renderVertex(x, y, mz, tx + OFFSET, ty, Lights[LightLevel][c1], 0); // C
	renderVertex(x, y, z, tx + OFFSET, ty + OFFSET, Lights[LightLevel][c2], 0); // D
	renderVertex(mx, y, z, tx, ty + OFFSET, Lights[LightLevel][c3], 0); // A
	renderVertex(mx, y, mz, tx, ty, Lights[LightLevel][c4], 0); // B
}

inline void renderBottom(f32 x, f32 y, f32 z, f32 mx, f32 my, f32 mz, f32 tx, f32 ty, u8 c1, u8 c2, u8 c3, u8 c4) {
	renderVertex(mx, my, z, tx + OFFSET, ty + OFFSET, Lights[LightLevel][c1], 1); // E
	renderVertex(x, my, z, tx, ty + OFFSET, Lights[LightLevel][c2], 1); // H
	renderVertex(x, my, mz, tx, ty, Lights[LightLevel][c3], 1); // G
	renderVertex(mx, my, mz, tx + OFFSET, ty, Lights[LightLevel][c4], 1); // F
}

inline void renderLeft(f32 x, f32 y, f32 z, f32 mx, f32 my, f32 mz, f32 tx, f32 ty, u8 c1, u8 c2, u8 c3, u8 c4) {
	renderVertex(mx, y, mz, tx + OFFSET, ty, Lights[LightLevel][c1], 3); // B
	renderVertex(mx, y, z, tx, ty, Lights[LightLevel][c2], 3); // A
	renderVertex(mx, my, z, tx, ty + OFFSET, Lights[LightLevel][c3], 3); // E
	renderVertex(mx, my, mz, tx + OFFSET, ty + OFFSET, Lights[LightLevel][c4], 3); // F
}

inline void renderRight(f32 x, f32 y, f32 z, f32 mx, f32 my, f32 mz, f32 tx, f32 ty, u8 c1, u8 c2, u8 c3, u8 c4) {
	renderVertex(x, my, z, tx + OFFSET, ty + OFFSET, Lights[LightLevel][c1], 2); // H
	renderVertex(x, y, z, tx + OFFSET, ty, Lights[LightLevel][c2], 2); // D
	renderVertex(x, y, mz, tx, ty, Lights[LightLevel][c3], 2); // C
	renderVertex(x, my, mz, tx, ty + OFFSET, Lights[LightLevel][c4], 2); // G
}

/**
 *     B ------ C
	  /       / |
	 /       /  |
	A ------ D  G
	|   F   |  /
	|       | /
	E ------ H
 * */
void renderChunk(VerticalChunk& c, Renderer& renderer, t_pos2D pos){
	//int f[16][128][16][6];

	int px = pos.x << 4;
	int pz = pos.y << 4;
	int x, y, z, sz;
	int mx, my, mz, Mx, My, Mz;
	
	bool A, B, C, D, E, F, G, H;
	bool AB, BC, CD, DA,
	          EF, FG, GH, HE,
			  AE, BF, CG, DH;
	BlockType fT, fB, fL, fR, fF, fK, type;
	bool tT, tB, tL, tR, tF, tK;
	
    VerticalChunk& cnorth = *c.neighboors[CHUNK_NORTH];
    VerticalChunk& csouth = *c.neighboors[CHUNK_SOUTH];
    VerticalChunk& cest = *c.neighboors[CHUNK_EST];
    VerticalChunk& cwest = *c.neighboors[CHUNK_WEST];

	for (y = 1; y < 127; y++) { // for each vertical levels (except 1st and last)
		
		// X 0 Z 0
		/*
		if ((type = c.blocks[0][y][0].type) > BlockType::Air) {

			renderer.renderBloc({(f32)px, (f32)y, (f32)pz}, type,
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
*/

		my = y - 1;
		My = y + 1;
		for (x = 1; x < 15; x++) {
			for (z = 1; z < 15; z++) {
				if ((type = c.blocks[x][y][z].type) > BlockType::Air) {
					mx = x - 1;
					mz = z - 1;
					Mx = x + 1;
					Mz = z + 1;
					
					fT = c.blocks[x][My][z].type;
					fB = c.blocks[x][my][z].type;
					fL = c.blocks[mx][y][z].type;
					fR = c.blocks[Mx][y][z].type;
					fF = c.blocks[x][y][Mz].type;
					fK = c.blocks[x][y][mz].type;
					
					tT = fT <= BlockType::Air;
					tB = fB <= BlockType::Air;
					tL = fL <= BlockType::Air;
					tR = fR <= BlockType::Air;
					tF = fF <= BlockType::Air;
					tK = fK <= BlockType::Air;
					
					sz = tT + tB + tL + tR + tF + tK;
					if (sz) {
						
						A = c.blocks[mx][My][Mz].type > BlockType::Air;
						B = c.blocks[mx][My][mz].type > BlockType::Air;
						C = c.blocks[Mx][My][mz].type > BlockType::Air;
						D = c.blocks[Mx][My][Mz].type > BlockType::Air;
						E = c.blocks[mx][my][Mz].type > BlockType::Air;
						F = c.blocks[mx][my][mz].type > BlockType::Air;
						G = c.blocks[Mx][my][mz].type > BlockType::Air;
						H = c.blocks[Mx][my][Mz].type > BlockType::Air;
						
						AB = c.blocks[mx][My][z].type > BlockType::Air;
						BC = c.blocks[x][My][mz].type > BlockType::Air;
						CD = c.blocks[Mx][My][z].type > BlockType::Air;
						DA = c.blocks[x][My][Mz].type > BlockType::Air;
						
						EF = c.blocks[mx][my][z].type > BlockType::Air;
						FG = c.blocks[x][my][mz].type > BlockType::Air;
						GH = c.blocks[Mx][my][z].type > BlockType::Air;
						HE = c.blocks[x][my][Mz].type > BlockType::Air;
						
						AE = c.blocks[mx][y][Mz].type > BlockType::Air;
						BF = c.blocks[mx][y][mz].type > BlockType::Air;
						CG = c.blocks[Mx][y][mz].type > BlockType::Air;
						DH = c.blocks[Mx][y][Mz].type > BlockType::Air;
						
						
                        GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
						f32 tx, ty;
						if (tT) {
							tx = blocData[type].x[BLOC_FACE_TOP];
							ty = blocData[type].y[BLOC_FACE_TOP];
							LightLevel = fT;
							renderTop(   (f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, tx, ty, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB); // CDAB
						}
						if (tB) {
							tx = blocData[type].x[BLOC_FACE_BOTTOM];
							ty = blocData[type].y[BLOC_FACE_BOTTOM];
							LightLevel = fB;
							renderBottom((f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, tx, ty, E + EF + HE, H + GH + FG, G + FG + GH, F + FG + EF); // EHGF
						}
						if (tL) {
							tx = blocData[type].x[BLOC_FACE_LEFT];
							ty = blocData[type].y[BLOC_FACE_LEFT];
							LightLevel = fL;
							renderLeft(  (f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, tx, ty, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF); // BAEF
						}
						if (tR) {
							tx = blocData[type].x[BLOC_FACE_RIGHT];
							ty = blocData[type].y[BLOC_FACE_RIGHT];
							LightLevel = fR;
							renderRight( (f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, tx, ty, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH); // HDCG
						}
						if (tF) {
							tx = blocData[type].x[BLOC_FACE_FRONT];
							ty = blocData[type].y[BLOC_FACE_FRONT];
							LightLevel = fF;
							renderFront( (f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, tx, ty, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE); // ADHE
						}
						if (tK) {
							tx = blocData[type].x[BLOC_FACE_BACK];
							ty = blocData[type].y[BLOC_FACE_BACK];
							LightLevel = fK;
							renderBack(  (f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, tx, ty, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF); // GCBF
						}
						GX_End();
					}
				}
			}
		}
	}
}
