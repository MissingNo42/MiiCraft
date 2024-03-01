//
// Created by Romain on 15/02/2024.
//

#include "world/verticalChunk.h"
#include "render/cacheManager.h"
#include "render/bloc.h"
#include "world/world.h"


inline void renderVertex(f32 x, f32 y, f32 z, f32 tx, f32 ty, u8 color, u8 alpha) {
    //GX_Position3f32(x, y, z);
    //GX_Normal1x8(normal);
    //GX_Color1u32(color);
    //GX_TexCoord2f32(tx, ty);
	ChunkCache::addVertex(x, y, z, color, tx, ty, alpha);
}

inline void renderFront(f32 x, f32 y, f32 z, f32 mx, f32 my, f32, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    f32 tx = blocData[type].x[BLOC_FACE_FRONT];
    f32 ty = blocData[type].y[BLOC_FACE_FRONT];
    renderVertex(mx, y, z, tx, ty, (light << 2) + c1, alpha); // A
    renderVertex(x, y, z, tx + OFFSET, ty, (light << 2) + c2, alpha); // D
    renderVertex(x, my, z, tx + OFFSET, ty + OFFSET, (light << 2) + c3, alpha); // H
    renderVertex(mx, my, z, tx, ty + OFFSET, (light << 2) + c4, alpha); // E
}

inline void renderBack(f32 x, f32 y, f32, f32 mx, f32 my, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    f32 tx = blocData[type].x[BLOC_FACE_BACK];
    f32 ty = blocData[type].y[BLOC_FACE_BACK];
    renderVertex(x, my, mz, tx, ty + OFFSET, (light << 2) + c1, alpha); // G
    renderVertex(x, y, mz, tx, ty, (light << 2) + c2, alpha); // C
    renderVertex(mx, y, mz, tx + OFFSET, ty, (light << 2) + c3, alpha); // B
    renderVertex(mx, my, mz, tx + OFFSET, ty + OFFSET, (light << 2) + c4, alpha); // F
}

inline void renderTop(f32 x, f32 y, f32 z, f32 mx, f32, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    f32 tx = blocData[type].x[BLOC_FACE_TOP];
    f32 ty = blocData[type].y[BLOC_FACE_TOP];
    renderVertex(x, y, mz, tx + OFFSET, ty, (light << 2) + c1, alpha); // C
    renderVertex(x, y, z, tx + OFFSET, ty + OFFSET, (light << 2) + c2, alpha); // D
    renderVertex(mx, y, z, tx, ty + OFFSET, (light << 2) + c3, alpha); // A
    renderVertex(mx, y, mz, tx, ty, (light << 2) + c4, alpha); // B
}

inline void renderBottom(f32 x, f32, f32 z, f32 mx, f32 my, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    f32 tx = blocData[type].x[BLOC_FACE_BOTTOM];
    f32 ty = blocData[type].y[BLOC_FACE_BOTTOM];
    renderVertex(mx, my, z, tx + OFFSET, ty + OFFSET, (light << 2) + c1, alpha); // E
    renderVertex(x, my, z, tx, ty + OFFSET, (light << 2) + c2, alpha); // H
    renderVertex(x, my, mz, tx, ty, (light << 2) + c3, alpha); // G
    renderVertex(mx, my, mz, tx + OFFSET, ty, (light << 2) + c4, alpha); // F
}

inline void renderLeft(f32, f32 y, f32 z, f32 mx, f32 my, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    f32 tx = blocData[type].x[BLOC_FACE_LEFT];
    f32 ty = blocData[type].y[BLOC_FACE_LEFT];
    renderVertex(mx, y, mz, tx, ty, (light << 2) + c1, alpha); // B
    renderVertex(mx, y, z, tx + OFFSET, ty, (light << 2) + c2, alpha); // A
    renderVertex(mx, my, z, tx + OFFSET, ty + OFFSET, (light << 2) + c3, alpha); // E
    renderVertex(mx, my, mz, tx, ty + OFFSET, (light << 2) + c4, alpha); // F
}

inline void renderRight(f32 x, f32 y, f32 z, f32, f32 my, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    f32 tx = blocData[type].x[BLOC_FACE_RIGHT];
    f32 ty = blocData[type].y[BLOC_FACE_RIGHT];
    renderVertex(x, my, z, tx, ty + OFFSET, (light << 2) + c1, alpha); // H
    renderVertex(x, y, z, tx, ty, (light << 2) + c2, alpha); // D
    renderVertex(x, y, mz, tx + OFFSET, ty, (light << 2) + c3, alpha); // C
    renderVertex(x, my, mz, tx + OFFSET, ty + OFFSET, (light << 2) + c4, alpha); // G
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
void Renderer::renderChunk(VerticalChunk& c){
    //int f[16][128][16][6];

    int px = c.coord.x << 4;
    int pz = c.coord.y << 4;
    int x, y, z, sz;
    int mx, my, mz, Mx, My, Mz;

    bool A, B, C, D, E, F, G, H; // render cubes with X vertex in common
    bool AB, BC, CD, DA,  // render cubes with X, Y vertices in common
            EF, FG, GH, HE,
            AE, BF, CG, DH;
    BlockType fT, fB, fL, fR, fF, fK, type; // block type of the 6 adjacent blocks + the current one
    bool tT, tB, tL, tR, tF, tK; // true if the current block's faces must be rendered
	u8 lT, lB, lL, lR, lF, lK; // light level of the 6 adjacent blocks

    VerticalChunk& cnorth = World::chunkSlots[c.neighboors[CHUNK_NORTH]];
    VerticalChunk& csouth = World::chunkSlots[c.neighboors[CHUNK_SOUTH]];
    VerticalChunk& ceast =  World::chunkSlots[c.neighboors[CHUNK_EAST]];
    VerticalChunk& cwest =  World::chunkSlots[c.neighboors[CHUNK_WEST]];

    VerticalChunk& cnortheast = World::chunkSlots[cnorth.neighboors[CHUNK_EAST]];
    VerticalChunk& cnorthwest = World::chunkSlots[cnorth.neighboors[CHUNK_WEST]];
    VerticalChunk& csoutheast = World::chunkSlots[csouth.neighboors[CHUNK_EAST]];
    VerticalChunk& csouthwest = World::chunkSlots[csouth.neighboors[CHUNK_WEST]];

    for (y = 1; y < 127; y++) { // for each vertical levels (except 1st and last)

        my = y - 1;
        My = y + 1;

		//goto center;
	
        // X 0 Z 0

        if ((type = c.blocks[0][y][0]) > BlockType::Air) {

            fT = c.blocks[0][My][0];
            fB = c.blocks[0][my][0];
            fL = cwest.blocks[15][y][0];
            fR = c.blocks[1][y][0];
            fF = c.blocks[0][y][1];
            fK = csouth.blocks[0][y][15];

            
				if (isOpaque(type)) {
                    tT = !isOpaque(fT);
                    tB = !isOpaque(fB);
                    tL = !isOpaque(fL);
                    tR = !isOpaque(fR);
                    tF = !isOpaque(fF);
                    tK = !isOpaque(fK);
				}
				else if (isTransparent(type)) {
                    tT = !isTransparent(fT);
                    tB = !isTransparent(fB);
                    tL = !isTransparent(fL);
                    tR = !isTransparent(fR);
                    tF = !isTransparent(fF);
                    tK = !isTransparent(fK);
				} else if (isSemiTransparent(type)) {
                    tT = tB = tL = tR = tF = tK = true;
				} else {
					printf("Render failed : %d\r", type); // TODO call special render mode
				}

                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
					
					lT = isAir(fT) ? fT : BlockType::Air8;
					lB = isAir(fB) ? fB : BlockType::Air8;
					lL = isAir(fL) ? fL : BlockType::Air8;
					lR = isAir(fR) ? fR : BlockType::Air8;
					lF = isAir(fF) ? fF : BlockType::Air8;
					lK = isAir(fK) ? fK : BlockType::Air8;

					if (isOpaque(type) || isSemiTransparent(type)) {
						A = !isAir(cwest.blocks[15][My][1]);
						B = !isAir(csouthwest.blocks[15][My][15]);
						C = !isAir(csouth.blocks[1][My][15]);
						D = !isAir(c.blocks[1][My][1]);
						E = !isAir(cwest.blocks[15][my][1]);
						F = !isAir(csouthwest.blocks[15][my][15]);
						G = !isAir(csouth.blocks[1][my][15]);
						H = !isAir(c.blocks[1][my][1]);
						
						AB = !isAir(cwest.blocks[15][My][0]);
						BC = !isAir(csouth.blocks[0][My][15]);
						CD = !isAir(c.blocks[1][My][0]);
						DA = !isAir(c.blocks[0][My][1]);
						
						EF = !isAir(cwest.blocks[15][my][0]);
						FG = !isAir(csouth.blocks[0][my][15]);
						GH = !isAir(c.blocks[1][my][0]);
						HE = !isAir(c.blocks[0][my][1]);
						
						AE = !isAir(cwest.blocks[15][y][1]);
						BF = !isAir(csouthwest.blocks[15][y][15]);
						CG = !isAir(csouth.blocks[1][y][15]);
						DH = !isAir(c.blocks[1][y][1]);
					} else if (isTransparent(type)) {
						A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
					}


                //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                if (tT) renderTop   ((f32) (0 + px), (f32) y, (f32) (0 + pz), -1 + px, my, -1 + pz, type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type)); // CDAB
                if (tB) renderBottom((f32) (0 + px), (f32) y, (f32) (0 + pz), -1 + px, my, -1 + pz, type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type)); // EHGF
                if (tL) renderLeft  ((f32) (0 + px), (f32) y, (f32) (0 + pz), -1 + px, my, -1 + pz, type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type)); // BAEF
                if (tR) renderRight ((f32) (0 + px), (f32) y, (f32) (0 + pz), -1 + px, my, -1 + pz, type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type)); // HDCG
                if (tF) renderFront ((f32) (0 + px), (f32) y, (f32) (0 + pz), -1 + px, my, -1 + pz, type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type)); // ADHE
                if (tK) renderBack  ((f32) (0 + px), (f32) y, (f32) (0 + pz), -1 + px, my, -1 + pz, type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type)); // GCBF
                //GX_End();
            }
        }
		
        // X 0 Z 15
		
        if ((type = c.blocks[0][y][15]) > BlockType::Air) {


            fT = c.blocks[0][My][15];
            fB = c.blocks[0][my][15];
            fL = cwest.blocks[15][y][15];
            fR = c.blocks[1][y][15];
            fF = cnorth.blocks[0][y][0];
            fK = c.blocks[0][y][14];

            
				if (isOpaque(type)) {
                    tT = !isOpaque(fT);
                    tB = !isOpaque(fB);
                    tL = !isOpaque(fL);
                    tR = !isOpaque(fR);
                    tF = !isOpaque(fF);
                    tK = !isOpaque(fK);
				}
				else if (isTransparent(type)) {
                    tT = !isTransparent(fT);
                    tB = !isTransparent(fB);
                    tL = !isTransparent(fL);
                    tR = !isTransparent(fR);
                    tF = !isTransparent(fF);
                    tK = !isTransparent(fK);
				} else if (isSemiTransparent(type)) {
                    tT = tB = tL = tR = tF = tK = true;
				} else {
					printf("Render failed : %d\r", type); // TODO call special render mode
				}

                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
					
					lT = isAir(fT) ? fT : BlockType::Air8;
					lB = isAir(fB) ? fB : BlockType::Air8;
					lL = isAir(fL) ? fL : BlockType::Air8;
					lR = isAir(fR) ? fR : BlockType::Air8;
					lF = isAir(fF) ? fF : BlockType::Air8;
					lK = isAir(fK) ? fK : BlockType::Air8;

					if (isOpaque(type) || isSemiTransparent(type)) {
						A = !isAir(cnorthwest.blocks[15][My][0]);
						B = !isAir(cwest.blocks[15][My][14]);
						C = !isAir(c.blocks[1][My][14]);
						D = !isAir(cnorth.blocks[1][My][0]);
						E = !isAir(cnorthwest.blocks[15][my][0]);
						F = !isAir(cwest.blocks[15][my][14]);
						G = !isAir(c.blocks[1][my][14]);
						H = !isAir(cnorth.blocks[1][my][0]);
						
						AB = !isAir(cwest.blocks[15][My][15]);
						BC = !isAir(c.blocks[0][My][14]);
						CD = !isAir(c.blocks[1][My][15]);
						DA = !isAir(cnorth.blocks[0][My][0]);
						
						EF = !isAir(cwest.blocks[15][my][15]);
						FG = !isAir(c.blocks[0][my][14]);
						GH = !isAir(c.blocks[1][my][15]);
						HE = !isAir(cnorth.blocks[0][my][0]);
						
						AE = !isAir(cnorthwest.blocks[15][y][0]);
						BF = !isAir(cwest.blocks[15][y][14]);
						CG = !isAir(c.blocks[1][y][14]);
						DH = !isAir(cnorth.blocks[1][y][0]);
					} else if (isTransparent(type)) {
						A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
					}

                //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                
                if (tT) renderTop   ((f32) (0 + px), (f32) y, (f32) (15 + pz), -1 + px, my, 14 + pz, type, C + BC + CD,D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type)); // CDAB
                if (tB) renderBottom((f32) (0 + px), (f32) y, (f32) (15 + pz), -1 + px, my, 14 + pz, type, E + EF + HE,H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type)); // EHGF
                if (tL) renderLeft  ((f32) (0 + px), (f32) y, (f32) (15 + pz), -1 + px, my, 14 + pz, type, B + AB + BF,A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type)); // BAEF
                if (tR) renderRight ((f32) (0 + px), (f32) y, (f32) (15 + pz), -1 + px, my, 14 + pz, type, H + DH + GH,D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type)); // HDCG
                if (tF) renderFront ((f32) (0 + px), (f32) y, (f32) (15 + pz), -1 + px, my, 14 + pz, type, A + AE + DA,D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type)); // ADHE
                if (tK) renderBack  ((f32) (0 + px), (f32) y, (f32) (15 + pz), -1 + px, my, 14 + pz, type, G + CG + FG,C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type)); // GCBF
                //GX_End();
            }
        }

		
        // X 15 Z 0

        if ((type = c.blocks[15][y][0]) > BlockType::Air) {


            fT = c.blocks[15][My][0];
            fB = c.blocks[15][my][0];
            fL = c.blocks[14][y][0];
            fR = ceast.blocks[0][y][0];
            fF = c.blocks[15][y][1];
            fK = csouth.blocks[15][y][15];

            
				if (isOpaque(type)) {
                    tT = !isOpaque(fT);
                    tB = !isOpaque(fB);
                    tL = !isOpaque(fL);
                    tR = !isOpaque(fR);
                    tF = !isOpaque(fF);
                    tK = !isOpaque(fK);
				}
				else if (isTransparent(type)) {
                    tT = !isTransparent(fT);
                    tB = !isTransparent(fB);
                    tL = !isTransparent(fL);
                    tR = !isTransparent(fR);
                    tF = !isTransparent(fF);
                    tK = !isTransparent(fK);
				} else if (isSemiTransparent(type)) {
                    tT = tB = tL = tR = tF = tK = true;
				} else {
					printf("Render failed : %d\r", type); // TODO call special render mode
				}

                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
					
					lT = isAir(fT) ? fT : BlockType::Air8;
					lB = isAir(fB) ? fB : BlockType::Air8;
					lL = isAir(fL) ? fL : BlockType::Air8;
					lR = isAir(fR) ? fR : BlockType::Air8;
					lF = isAir(fF) ? fF : BlockType::Air8;
					lK = isAir(fK) ? fK : BlockType::Air8;

					if (isOpaque(type) || isSemiTransparent(type)) {
						A = !isAir(c.blocks[14][My][1]);
						B = !isAir(csouth.blocks[14][My][15]);
						C = !isAir(csoutheast.blocks[0][My][15]);
						D = !isAir(ceast.blocks[0][My][1]);
						E = !isAir(c.blocks[14][my][1]);
						F = !isAir(csouth.blocks[14][my][15]);
						G = !isAir(csoutheast.blocks[0][my][15]);
						H = !isAir(ceast.blocks[0][my][1]);
						
						AB = !isAir(c.blocks[14][My][0]);
						BC = !isAir(csouth.blocks[15][My][15]);
						CD = !isAir(ceast.blocks[0][My][0]);
						DA = !isAir(c.blocks[15][My][1]);
						
						EF = !isAir(c.blocks[14][my][0]);
						FG = !isAir(csouth.blocks[15][my][15]);
						GH = !isAir(ceast.blocks[0][my][0]);
						HE = !isAir(c.blocks[15][my][1]);
						
						AE = !isAir(c.blocks[14][y][1]);
						BF = !isAir(csouth.blocks[14][y][15]);
						CG = !isAir(csoutheast.blocks[0][y][15]);
						DH = !isAir(ceast.blocks[0][y][1]);
					} else if (isTransparent(type)) {
						A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
					}


                //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                
                if (tT) renderTop   ((f32) (15 + px), (f32) y, (f32) (0 + pz), 14 + px, my, -1 + pz, type, C + BC + CD,D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type)); // CDAB
                if (tB) renderBottom((f32) (15 + px), (f32) y, (f32) (0 + pz), 14 + px, my, -1 + pz, type, E + EF + HE,H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type)); // EHGF
                if (tL) renderLeft  ((f32) (15 + px), (f32) y, (f32) (0 + pz), 14 + px, my, -1 + pz, type, B + AB + BF,A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type)); // BAEF
                if (tR) renderRight ((f32) (15 + px), (f32) y, (f32) (0 + pz), 14 + px, my, -1 + pz, type, H + DH + GH,D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type)); // HDCG
                if (tF) renderFront ((f32) (15 + px), (f32) y, (f32) (0 + pz), 14 + px, my, -1 + pz, type, A + AE + DA,D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type)); // ADHE
                if (tK) renderBack  ((f32) (15 + px), (f32) y, (f32) (0 + pz), 14 + px, my, -1 + pz, type, G + CG + FG,C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type)); // GCBF
                //GX_End();
            }
        }
		

        // X 15 Z 15

        if ((type = c.blocks[15][y][15]) > BlockType::Air) {


            fT = c.blocks[15][My][15];
            fB = c.blocks[15][my][15];
            fL = c.blocks[14][y][15];
            fR = ceast.blocks[0][y][15];
            fF = cnorth.blocks[15][y][0];
            fK = c.blocks[15][y][14];

            
				if (isOpaque(type)) {
                    tT = !isOpaque(fT);
                    tB = !isOpaque(fB);
                    tL = !isOpaque(fL);
                    tR = !isOpaque(fR);
                    tF = !isOpaque(fF);
                    tK = !isOpaque(fK);
				}
				else if (isTransparent(type)) {
                    tT = !isTransparent(fT);
                    tB = !isTransparent(fB);
                    tL = !isTransparent(fL);
                    tR = !isTransparent(fR);
                    tF = !isTransparent(fF);
                    tK = !isTransparent(fK);
				} else if (isSemiTransparent(type)) {
                    tT = tB = tL = tR = tF = tK = true;
				} else {
					printf("Render failed : %d\r", type); // TODO call special render mode
				}

                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
					
					lT = isAir(fT) ? fT : BlockType::Air8;
					lB = isAir(fB) ? fB : BlockType::Air8;
					lL = isAir(fL) ? fL : BlockType::Air8;
					lR = isAir(fR) ? fR : BlockType::Air8;
					lF = isAir(fF) ? fF : BlockType::Air8;
					lK = isAir(fK) ? fK : BlockType::Air8;

					if (isOpaque(type) || isSemiTransparent(type)) {
						A = !isAir(cnorth.blocks[14][My][0]);
						B = !isAir(c.blocks[14][My][14]);
						C = !isAir(ceast.blocks[0][My][14]);
						D = !isAir(cnortheast.blocks[0][My][0]);
						E = !isAir(cnorth.blocks[14][my][0]);
						F = !isAir(c.blocks[14][my][14]);
						G = !isAir(ceast.blocks[0][my][14]);
						H = !isAir(cnortheast.blocks[0][my][0]);
						
						AB = !isAir(c.blocks[14][My][15]);
						BC = !isAir(c.blocks[15][My][14]);
						CD = !isAir(ceast.blocks[0][My][15]);
						DA = !isAir(cnorth.blocks[15][My][0]);
						
						EF = !isAir(c.blocks[14][my][15]);
						FG = !isAir(c.blocks[15][my][14]);
						GH = !isAir(ceast.blocks[0][my][15]);
						HE = !isAir(cnorth.blocks[15][my][0]);
						
						AE = !isAir(cnorth.blocks[14][y][0]);
						BF = !isAir(c.blocks[14][y][14]);
						CG = !isAir(ceast.blocks[0][y][14]);
						DH = !isAir(cnortheast.blocks[0][y][0]);
					} else if (isTransparent(type)) {
						A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
					}
	
	
				//GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                if (tT) renderTop   ((f32) (15 + px), (f32) y, (f32) (15 + pz), 14 + px, my, 14 + pz, type, C + BC + CD,D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type)); // CDAB
                if (tB) renderBottom((f32) (15 + px), (f32) y, (f32) (15 + pz), 14 + px, my, 14 + pz, type, E + EF + HE,H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type)); // EHGF
                if (tL) renderLeft  ((f32) (15 + px), (f32) y, (f32) (15 + pz), 14 + px, my, 14 + pz, type, B + AB + BF,A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type)); // BAEF
                if (tR) renderRight ((f32) (15 + px), (f32) y, (f32) (15 + pz), 14 + px, my, 14 + pz, type, H + DH + GH,D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type)); // HDCG
                if (tF) renderFront ((f32) (15 + px), (f32) y, (f32) (15 + pz), 14 + px, my, 14 + pz, type, A + AE + DA,D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type)); // ADHE
                if (tK) renderBack  ((f32) (15 + px), (f32) y, (f32) (15 + pz), 14 + px, my, 14 + pz, type, G + CG + FG,C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type)); // GCBF
                //GX_End();
            }
        }

        // X 0
		
        for(z = 1; z < 15; z++){
            if ((type = c.blocks[0][y][z]) > BlockType::Air) {
                mz = z - 1;
                Mz = z + 1;

                fT = c.blocks[0][My][z];
                fB = c.blocks[0][my][z];
                fL = cwest.blocks[15][y][z];
                fR = c.blocks[1][y][z];
                fF = c.blocks[0][y][Mz];
                fK = c.blocks[0][y][mz];

               
				if (isOpaque(type)) {
                    tT = !isOpaque(fT);
                    tB = !isOpaque(fB);
                    tL = !isOpaque(fL);
                    tR = !isOpaque(fR);
                    tF = !isOpaque(fF);
                    tK = !isOpaque(fK);
				}
				else if (isTransparent(type)) {
                    tT = !isTransparent(fT);
                    tB = !isTransparent(fB);
                    tL = !isTransparent(fL);
                    tR = !isTransparent(fR);
                    tF = !isTransparent(fF);
                    tK = !isTransparent(fK);
				} else if (isSemiTransparent(type)) {
                    tT = tB = tL = tR = tF = tK = true;
				} else {
					printf("Render failed : %d\r", type); // TODO call special render mode
				}

                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
					
					lT = isAir(fT) ? fT : BlockType::Air8;
					lB = isAir(fB) ? fB : BlockType::Air8;
					lL = isAir(fL) ? fL : BlockType::Air8;
					lR = isAir(fR) ? fR : BlockType::Air8;
					lF = isAir(fF) ? fF : BlockType::Air8;
					lK = isAir(fK) ? fK : BlockType::Air8;

					if (isOpaque(type) || isSemiTransparent(type)) {
						A = !isAir(cwest.blocks[15][My][Mz]);
						B = !isAir(cwest.blocks[15][My][mz]);
						C = !isAir(c.blocks[1][My][mz]);
						D = !isAir(c.blocks[1][My][Mz]);
						E = !isAir(cwest.blocks[15][my][Mz]);
						F = !isAir(cwest.blocks[15][my][mz]);
						G = !isAir(c.blocks[1][my][mz]);
						H = !isAir(c.blocks[1][my][Mz]);
						
						AB = !isAir(cwest.blocks[15][My][z]);
						BC = !isAir(c.blocks[0][My][mz]);
						CD = !isAir(c.blocks[1][My][z]);
						DA = !isAir(c.blocks[0][My][Mz]);
						
						EF = !isAir(cwest.blocks[15][my][z]);
						FG = !isAir(c.blocks[0][my][mz]);
						GH = !isAir(c.blocks[1][my][z]);
						HE = !isAir(c.blocks[0][my][Mz]);
						
						AE = !isAir(cwest.blocks[15][y][Mz]);
						BF = !isAir(cwest.blocks[15][y][mz]);
						CG = !isAir(c.blocks[1][y][mz]);
						DH = !isAir(c.blocks[1][y][Mz]);
					} else if (isTransparent(type)) {
						A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
					}


                    //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                    if (tT) renderTop   ((f32) (0 + px), (f32) y, (f32) (z + pz), -1 + px, my, mz + pz, type, C + BC + CD,D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type)); // CDAB
                    if (tB) renderBottom((f32) (0 + px), (f32) y, (f32) (z + pz), -1 + px, my, mz + pz, type, E + EF + HE,H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type)); // EHGF
                    if (tL) renderLeft  ((f32) (0 + px), (f32) y, (f32) (z + pz), -1 + px, my, mz + pz, type, B + AB + BF,A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type)); // BAEF
                    if (tR) renderRight ((f32) (0 + px), (f32) y, (f32) (z + pz), -1 + px, my, mz + pz, type, H + DH + GH,D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type)); // HDCG
                    if (tF) renderFront ((f32) (0 + px), (f32) y, (f32) (z + pz), -1 + px, my, mz + pz, type, A + AE + DA,D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type)); // ADHE
                    if (tK) renderBack  ((f32) (0 + px), (f32) y, (f32) (z + pz), -1 + px, my, mz + pz, type, G + CG + FG,C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type)); // GCBF
                    //GX_End();
                }
            }
        }
		

        // X 15
		
        for(z = 1; z < 15; z++){
            if ((type = c.blocks[15][y][z]) > BlockType::Air) {
                mz = z - 1;
                Mz = z + 1;

                fT = c.blocks[15][My][z];
                fB = c.blocks[15][my][z];
                fL = c.blocks[14][y][z];
                fR = ceast.blocks[0][y][z];
                fF = c.blocks[15][y][Mz];
                fK = c.blocks[15][y][mz];

                
				if (isOpaque(type)) {
                    tT = !isOpaque(fT);
                    tB = !isOpaque(fB);
                    tL = !isOpaque(fL);
                    tR = !isOpaque(fR);
                    tF = !isOpaque(fF);
                    tK = !isOpaque(fK);
				}
				else if (isTransparent(type)) {
                    tT = !isTransparent(fT);
                    tB = !isTransparent(fB);
                    tL = !isTransparent(fL);
                    tR = !isTransparent(fR);
                    tF = !isTransparent(fF);
                    tK = !isTransparent(fK);
				} else if (isSemiTransparent(type)) {
                    tT = tB = tL = tR = tF = tK = true;
				} else {
					printf("Render failed : %d\r", type); // TODO call special render mode
				}

                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
					
					lT = isAir(fT) ? fT : BlockType::Air8;
					lB = isAir(fB) ? fB : BlockType::Air8;
					lL = isAir(fL) ? fL : BlockType::Air8;
					lR = isAir(fR) ? fR : BlockType::Air8;
					lF = isAir(fF) ? fF : BlockType::Air8;
					lK = isAir(fK) ? fK : BlockType::Air8;

					if (isOpaque(type) || isSemiTransparent(type)) {
						A = !isAir(c.blocks[14][My][Mz]);
						B = !isAir(c.blocks[14][My][mz]);
						C = !isAir(ceast.blocks[0][My][mz]);
						D = !isAir(ceast.blocks[0][My][Mz]);
						E = !isAir(c.blocks[14][my][Mz]);
						F = !isAir(c.blocks[14][my][mz]);
						G = !isAir(ceast.blocks[0][my][mz]);
						H = !isAir(ceast.blocks[0][my][Mz]);
						
						AB = !isAir(c.blocks[14][My][z]);
						BC = !isAir(c.blocks[15][My][mz]);
						CD = !isAir(ceast.blocks[0][My][z]);
						DA = !isAir(c.blocks[15][My][Mz]);
						
						EF = !isAir(c.blocks[14][my][z]);
						FG = !isAir(c.blocks[15][my][mz]);
						GH = !isAir(ceast.blocks[0][my][z]);
						HE = !isAir(c.blocks[15][my][Mz]);
						
						AE = !isAir(c.blocks[14][y][Mz]);
						BF = !isAir(c.blocks[14][y][mz]);
						CG = !isAir(ceast.blocks[0][y][mz]);
						DH = !isAir(ceast.blocks[0][y][Mz]);
					} else if (isTransparent(type)) {
						A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
					}

                    //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                    if (tT) renderTop   ((f32) (15 + px), (f32) y, (f32) (z + pz), 14 + px, my, mz + pz, type, C + BC + CD,D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type)); // CDAB
                    if (tB) renderBottom((f32) (15 + px), (f32) y, (f32) (z + pz), 14 + px, my, mz + pz, type, E + EF + HE,H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type)); // EHGF
                    if (tL) renderLeft  ((f32) (15 + px), (f32) y, (f32) (z + pz), 14 + px, my, mz + pz, type, B + AB + BF,A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type)); // BAEF
                    if (tR) renderRight ((f32) (15 + px), (f32) y, (f32) (z + pz), 14 + px, my, mz + pz, type, H + DH + GH,D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type)); // HDCG
                    if (tF) renderFront ((f32) (15 + px), (f32) y, (f32) (z + pz), 14 + px, my, mz + pz, type, A + AE + DA,D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type)); // ADHE
                    if (tK) renderBack  ((f32) (15 + px), (f32) y, (f32) (z + pz), 14 + px, my, mz + pz, type, G + CG + FG,C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type)); // GCBF
                    //GX_End();
                }
            }


        }
		

        // Z 0

        for(x=1; x<15; x++){
            if ((type = c.blocks[x][y][0]) > BlockType::Air) {
                mx = x - 1;
                Mx = x + 1;
                fT = c.blocks[x][My][0];
                fB = c.blocks[x][my][0];
                fL = c.blocks[mx][y][0];
                fR = c.blocks[Mx][y][0];
                fF = c.blocks[x][y][1];
                fK = csouth.blocks[x][y][15];

               
				if (isOpaque(type)) {
                    tT = !isOpaque(fT);
                    tB = !isOpaque(fB);
                    tL = !isOpaque(fL);
                    tR = !isOpaque(fR);
                    tF = !isOpaque(fF);
                    tK = !isOpaque(fK);
				}
				else if (isTransparent(type)) {
                    tT = !isTransparent(fT);
                    tB = !isTransparent(fB);
                    tL = !isTransparent(fL);
                    tR = !isTransparent(fR);
                    tF = !isTransparent(fF);
                    tK = !isTransparent(fK);
				} else if (isSemiTransparent(type)) {
                    tT = tB = tL = tR = tF = tK = true;
				} else {
					printf("Render failed : %d\r", type); // TODO call special render mode
				}

                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
					
					lT = isAir(fT) ? fT : BlockType::Air8;
					lB = isAir(fB) ? fB : BlockType::Air8;
					lL = isAir(fL) ? fL : BlockType::Air8;
					lR = isAir(fR) ? fR : BlockType::Air8;
					lF = isAir(fF) ? fF : BlockType::Air8;
					lK = isAir(fK) ? fK : BlockType::Air8;

					if (isOpaque(type) || isSemiTransparent(type)) {
						A = !isAir(c.blocks[mx][My][1]);
						B = !isAir(csouth.blocks[mx][My][15]);
						C = !isAir(csouth.blocks[Mx][My][15]);
						D = !isAir(c.blocks[Mx][My][1]);
						E = !isAir(c.blocks[mx][my][1]);
						F = !isAir(csouth.blocks[mx][my][15]);
						G = !isAir(csouth.blocks[Mx][my][15]);
						H = !isAir(c.blocks[Mx][my][1]);
						
						AB = !isAir(c.blocks[mx][My][0]);
						BC = !isAir(csouth.blocks[x][My][15]);
						CD = !isAir(c.blocks[Mx][My][0]);
						DA = !isAir(c.blocks[x][My][1]);
						
						EF = !isAir(c.blocks[mx][my][0]);
						FG = !isAir(csouth.blocks[x][my][15]);
						GH = !isAir(c.blocks[Mx][my][0]);
						HE = !isAir(c.blocks[x][my][1]);
						
						AE = !isAir(c.blocks[mx][y][1]);
						BF = !isAir(csouth.blocks[mx][y][15]);
						CG = !isAir(csouth.blocks[Mx][y][15]);
						DH = !isAir(c.blocks[Mx][y][1]);
					} else if (isTransparent(type)) {
						A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
					}
					
                    //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                    if (tT) renderTop   ((f32) (x + px), (f32) y, (f32) (0 + pz), mx + px, my, -1 + pz, type, C + BC + CD,D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type)); // CDAB
                    if (tB) renderBottom((f32) (x + px), (f32) y, (f32) (0 + pz), mx + px, my, -1 + pz, type, E + EF + HE,H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type)); // EHGF
                    if (tL) renderLeft  ((f32) (x + px), (f32) y, (f32) (0 + pz), mx + px, my, -1 + pz, type, B + AB + BF,A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type)); // BAEF
                    if (tR) renderRight ((f32) (x + px), (f32) y, (f32) (0 + pz), mx + px, my, -1 + pz, type, H + DH + GH,D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type)); // HDCG
                    if (tF) renderFront ((f32) (x + px), (f32) y, (f32) (0 + pz), mx + px, my, -1 + pz, type, A + AE + DA,D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type)); // ADHE
                    if (tK) renderBack  ((f32) (x + px), (f32) y, (f32) (0 + pz), mx + px, my, -1 + pz, type, G + CG + FG,C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type)); // GCBF
                    //GX_End();
                }
            }
        }
		

        // Z 15
        for(x = 1; x < 15; x++){
            if ((type = c.blocks[x][y][15]) > BlockType::Air) {
                mx = x - 1;
                Mx = x + 1;

                fT = c.blocks[x][My][15];
                fB = c.blocks[x][my][15];
                fL = c.blocks[mx][y][15];
                fR = c.blocks[Mx][y][15];
                fF = cnorth.blocks[x][y][0];
                fK = c.blocks[x][y][14];

				if (isOpaque(type)) {
                    tT = !isOpaque(fT);
                    tB = !isOpaque(fB);
                    tL = !isOpaque(fL);
                    tR = !isOpaque(fR);
                    tF = !isOpaque(fF);
                    tK = !isOpaque(fK);
				}
				else if (isTransparent(type)) {
                    tT = !isTransparent(fT);
                    tB = !isTransparent(fB);
                    tL = !isTransparent(fL);
                    tR = !isTransparent(fR);
                    tF = !isTransparent(fF);
                    tK = !isTransparent(fK);
				} else if (isSemiTransparent(type)) {
                    tT = tB = tL = tR = tF = tK = true;
				} else {
					printf("Render failed : %d\r", type); // TODO call special render mode
				}

                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
					
					lT = isAir(fT) ? fT : BlockType::Air8;
					lB = isAir(fB) ? fB : BlockType::Air8;
					lL = isAir(fL) ? fL : BlockType::Air8;
					lR = isAir(fR) ? fR : BlockType::Air8;
					lF = isAir(fF) ? fF : BlockType::Air8;
					lK = isAir(fK) ? fK : BlockType::Air8;

					if (isOpaque(type) || isSemiTransparent(type)) {
						A = !isAir(cnorth.blocks[mx][My][0]);
						B = !isAir(c.blocks[mx][My][14]);
						C = !isAir(c.blocks[Mx][My][14]);
						D = !isAir(cnorth.blocks[Mx][My][0]);
						E = !isAir(cnorth.blocks[mx][my][0]);
						F = !isAir(c.blocks[mx][my][14]);
						G = !isAir(c.blocks[Mx][my][14]);
						H = !isAir(cnorth.blocks[Mx][my][0]);
						
						AB = !isAir(c.blocks[mx][My][15]);
						BC = !isAir(c.blocks[x][My][14]);
						CD = !isAir(c.blocks[Mx][My][15]);
						DA = !isAir(cnorth.blocks[x][My][0]);
						
						EF = !isAir(c.blocks[mx][my][15]);
						FG = !isAir(c.blocks[x][my][14]);
						GH = !isAir(c.blocks[Mx][my][15]);
						HE = !isAir(cnorth.blocks[x][my][0]);
						
						AE = !isAir(cnorth.blocks[mx][y][0]);
						BF = !isAir(c.blocks[mx][y][14]);
						CG = !isAir(c.blocks[Mx][y][14]);
						DH = !isAir(cnorth.blocks[Mx][y][0]);
					} else if (isTransparent(type)) {
						A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
					}
					
                    //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                    if (tT) renderTop   ((f32) (x + px), (f32) y, (f32) (15 + pz), mx + px, my, 14 + pz, type, C + BC + CD,D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type)); // CDAB
                    if (tB) renderBottom((f32) (x + px), (f32) y, (f32) (15 + pz), mx + px, my, 14 + pz, type, E + EF + HE,H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type)); // EHGF
                    if (tL) renderLeft  ((f32) (x + px), (f32) y, (f32) (15 + pz), mx + px, my, 14 + pz, type, B + AB + BF,A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type)); // BAEF
                    if (tR) renderRight ((f32) (x + px), (f32) y, (f32) (15 + pz), mx + px, my, 14 + pz, type, H + DH + GH,D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type)); // HDCG
                    if (tF) renderFront ((f32) (x + px), (f32) y, (f32) (15 + pz), mx + px, my, 14 + pz, type, A + AE + DA,D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type)); // ADHE
                    if (tK) renderBack  ((f32) (x + px), (f32) y, (f32) (15 + pz), mx + px, my, 14 + pz, type, G + CG + FG,C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type)); // GCBF
                    //GX_End();
                }
            }
        }

        for (x = 1; x < 15; x++) {
            for (z = 1; z < 15; z++) {
                if ((type = c.blocks[x][y][z]) > BlockType::Air) {
                    mx = x - 1;
                    mz = z - 1;
                    Mx = x + 1;
                    Mz = z + 1;

                    fT = c.blocks[x][My][z];
                    fB = c.blocks[x][my][z];
                    fL = c.blocks[mx][y][z];
                    fR = c.blocks[Mx][y][z];
                    fF = c.blocks[x][y][Mz];
                    fK = c.blocks[x][y][mz];
					
					if (isOpaque(type)) {
	                    tT = !isOpaque(fT);
	                    tB = !isOpaque(fB);
	                    tL = !isOpaque(fL);
	                    tR = !isOpaque(fR);
	                    tF = !isOpaque(fF);
	                    tK = !isOpaque(fK);
					}
					else if (isTransparent(type)) {
	                    tT = !isTransparent(fT);
	                    tB = !isTransparent(fB);
	                    tL = !isTransparent(fL);
	                    tR = !isTransparent(fR);
	                    tF = !isTransparent(fF);
	                    tK = !isTransparent(fK);
					} else if (isSemiTransparent(type)) {
	                    tT = tB = tL = tR = tF = tK = true;
					} else {
						printf("Render failed : %d\r", type); // TODO call special render mode
					}

                    sz = tT + tB + tL + tR + tF + tK;
                    if (sz) {
						
						lT = isAir(fT) ? fT : BlockType::Air8;
						lB = isAir(fB) ? fB : BlockType::Air8;
						lL = isAir(fL) ? fL : BlockType::Air8;
						lR = isAir(fR) ? fR : BlockType::Air8;
						lF = isAir(fF) ? fF : BlockType::Air8;
						lK = isAir(fK) ? fK : BlockType::Air8;

						if (isOpaque(type) || isSemiTransparent(type)) {
							A =  !isAir(c.blocks[mx][My][Mz]);
							B =  !isAir(c.blocks[mx][My][mz]);
							C =  !isAir(c.blocks[Mx][My][mz]);
							D =  !isAir(c.blocks[Mx][My][Mz]);
							E =  !isAir(c.blocks[mx][my][Mz]);
							F =  !isAir(c.blocks[mx][my][mz]);
							G =  !isAir(c.blocks[Mx][my][mz]);
							H =  !isAir(c.blocks[Mx][my][Mz]);
							
							AB = !isAir(c.blocks[mx][My][z]);
							BC = !isAir(c.blocks[x][My][mz]);
							CD = !isAir(c.blocks[Mx][My][z]);
							DA = !isAir(c.blocks[x][My][Mz]);
							
							EF = !isAir(c.blocks[mx][my][z]);
							FG = !isAir(c.blocks[x][my][mz]);
							GH = !isAir(c.blocks[Mx][my][z]);
							HE = !isAir(c.blocks[x][my][Mz]);
							
							AE = !isAir(c.blocks[mx][y][Mz]);
							BF = !isAir(c.blocks[mx][y][mz]);
							CG = !isAir(c.blocks[Mx][y][mz]);
							DH = !isAir(c.blocks[Mx][y][Mz]);
						} else if (isTransparent(type)) {
							A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
						}


                        //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                        if (tT) renderTop   ((f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type)); // CDAB
                        if (tB) renderBottom((f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type)); // EHGF
                        if (tL) renderLeft  ((f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type)); // BAEF
                        if (tR) renderRight ((f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type)); // HDCG
                        if (tF) renderFront ((f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type)); // ADHE
                        if (tK) renderBack  ((f32)(x + px), (f32)y, (f32)(z + pz), mx+px, my, mz+pz, type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type)); // GCBF
                        //GX_End();
                    }
                }
            }
        }
    }
}
