//
// Created by Romain on 15/02/2024.
//

#include "world/verticalChunk.h"
#include "render/cache.h"
#include "render/block.h"
#include "world/world.h"


inline void renderVertex(f32 x, f32 y, f32 z, u16 tc, u8 color, u8 alpha) {
    //GX_Position3f32(x, y, z);
    //GX_Normal1x8(normal);
    //GX_Color1u32(color);
    //GX_TexCoord2f32(tx, ty);
	ChunkCache::addVertex(x, y, z, color, tc, alpha);
}

inline void renderFront(f32 x, f32 y, f32 z, f32 mx, f32 my, f32, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    u16 tx = blockData[type].x[BLOC_FACE_FRONT];
    u16 ty = blockData[type].y[BLOC_FACE_FRONT];
	u16 tc = TXCOORD(tx, ty);
    renderVertex(mx, y, z, tc, (light << 2) + c1, alpha); // A
    renderVertex(x, y, z, tc + 17, (light << 2) + c2, alpha); // D
    renderVertex(x, my, z, tc + 18, (light << 2) + c3, alpha); // H
    renderVertex(mx, my, z, tc + 1, (light << 2) + c4, alpha); // E
}

inline void renderBack(f32 x, f32 y, f32, f32 mx, f32 my, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    u16 tx = blockData[type].x[BLOC_FACE_BACK];
    u16 ty = blockData[type].y[BLOC_FACE_BACK];
	u16 tc = TXCOORD(tx, ty);
    renderVertex(x, my, mz, tc + 1, (light << 2) + c1, alpha); // G
    renderVertex(x, y, mz, tc, (light << 2) + c2, alpha); // C
    renderVertex(mx, y, mz, tc + 17, (light << 2) + c3, alpha); // B
    renderVertex(mx, my, mz, tc + 18, (light << 2) + c4, alpha); // F
}

inline void renderTop(f32 x, f32 y, f32 z, f32 mx, f32, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    u16 tx = blockData[type].x[BLOC_FACE_TOP];
    u16 ty = blockData[type].y[BLOC_FACE_TOP];
	u16 tc = TXCOORD(tx, ty);
    renderVertex(x, y, mz, tc + 17, (light << 2) + c1, alpha); // C
    renderVertex(x, y, z, tc + 18, (light << 2) + c2, alpha); // D
    renderVertex(mx, y, z, tc + 1, (light << 2) + c3, alpha); // A
    renderVertex(mx, y, mz, tc, (light << 2) + c4, alpha); // B
}

inline void renderBottom(f32 x, f32, f32 z, f32 mx, f32 my, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    u16 tx = blockData[type].x[BLOC_FACE_BOTTOM];
    u16 ty = blockData[type].y[BLOC_FACE_BOTTOM];
	u16 tc = TXCOORD(tx, ty);
    renderVertex(mx, my, z, tc + 18, (light << 2) + c1, alpha); // E
    renderVertex(x, my, z, tc + 1, (light << 2) + c2, alpha); // H
    renderVertex(x, my, mz, tc, (light << 2) + c3, alpha); // G
    renderVertex(mx, my, mz, tc + 17, (light << 2) + c4, alpha); // F
}

inline void renderLeft(f32, f32 y, f32 z, f32 mx, f32 my, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    u16 tx = blockData[type].x[BLOC_FACE_LEFT];
    u16 ty = blockData[type].y[BLOC_FACE_LEFT];
	u16 tc = TXCOORD(tx, ty);
    renderVertex(mx, y, mz, tc, (light << 2) + c1, alpha); // B
    renderVertex(mx, y, z, tc + 17, (light << 2) + c2, alpha); // A
    renderVertex(mx, my, z, tc + 18, (light << 2) + c3, alpha); // E
    renderVertex(mx, my, mz, tc + 1, (light << 2) + c4, alpha); // F
}

inline void renderRight(f32 x, f32 y, f32 z, f32, f32 my, f32 mz, BlockType type, u8 c1, u8 c2, u8 c3, u8 c4, u8 light, u8 alpha) {
    u16 tx = blockData[type].x[BLOC_FACE_RIGHT];
    u16 ty = blockData[type].y[BLOC_FACE_RIGHT];
	u16 tc = TXCOORD(tx, ty);
    renderVertex(x, my, z, tc + 1, (light << 2) + c1, alpha); // H
    renderVertex(x, y, z, tc, (light << 2) + c2, alpha); // D
    renderVertex(x, y, mz, tc + 17, (light << 2) + c3, alpha); // C
    renderVertex(x, my, mz, tc + 18, (light << 2) + c4, alpha); // G
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
void Renderer::renderChunk(VerticalChunk& c) {
	//int f[16][128][16][6];
	
	int px = c.coord.x << 4;
	int pz = c.coord.y << 4;
	int x, y, z, sz;
	int mx, my, mz, Mx, My, Mz;
	
	bool A, B, C, D, E, F, G, H; // render cubes with X vertex in common
	bool AB, BC, CD, DA,  // render cubes with X, Y vertices in common
		 EF, FG, GH, HE,
		 AE, BF, CG, DH;
	Block fT, fB, fL, fR, fF, fK, type; // block type of the 6 adjacent blocks + the current one
	bool tT, tB, tL, tR, tF, tK; // true if the current block's faces must be rendered
	u8 lT, lB, lL, lR, lF, lK; // light level of the 6 adjacent blocks
	
	VerticalChunk &cnorth = World::chunkSlots[c.neighboors[Neighboor::NORTH]];
	VerticalChunk &csouth = World::chunkSlots[c.neighboors[Neighboor::SOUTH]];
	VerticalChunk &ceast = World::chunkSlots[c.neighboors[Neighboor::EAST]];
	VerticalChunk &cwest = World::chunkSlots[c.neighboors[Neighboor::WEST]];
	
	VerticalChunk &cnortheast = World::chunkSlots[cnorth.neighboors[Neighboor::EAST]];
	VerticalChunk &cnorthwest = World::chunkSlots[cnorth.neighboors[Neighboor::WEST]];
	VerticalChunk &csoutheast = World::chunkSlots[csouth.neighboors[Neighboor::EAST]];
	VerticalChunk &csouthwest = World::chunkSlots[csouth.neighboors[Neighboor::WEST]];
	
	for (y = 1; y < 127; y++) { // for each vertical levels (except 1st and last)
		
		my = y - 1;
		My = y + 1;
		
		//goto center; // test-only shortcut
		
		
		/**  All the 9 render cases:
		 * Z 15
		 * ^
		 * |
		 * |-------------------------------------|
		 * | 0 15 |         X 15         | 15 15 |
		 * |-------------------------------------|
		 * |      |                      |       |
		 * |      |                      |       |
		 * | 0  Z |         X  Z         | 15  Z |
		 * |      |                      |       |
		 * |      |                      |       |
		 * |-------------------------------------|
		 * | 0  0 |         X  0         | 15  0 |
		 * 0-----------------------------------------> X 15
		 * */
		 
        rendering:
        // X 0 Z 0
        if ((type = c.blocks[0][y][0]).type) {
        
            fT = c.blocks[0][My][0];
            fB = c.blocks[0][my][0];
            fL = cwest.blocks[15][y][0];
            fR = c.blocks[1][y][0];
            fF = c.blocks[0][y][1];
            fK = csouth.blocks[0][y][15];
        
            if (isOpaque(type.type)) {
                tT = !isOpaque(fT.type);
                tB = !isOpaque(fB.type);
                tL = !isOpaque(fL.type);
                tR = !isOpaque(fR.type);
                tF = !isOpaque(fF.type);
                tK = !isOpaque(fK.type);
            }
            else if (isTransparent(type.type)) {
                tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
                tB = fB.type != type.type; // !isTransparent(fB.type);
                tL = fL.type != type.type; // !isTransparent(fL.type);
                tR = fR.type != type.type; // !isTransparent(fR.type);
                tF = fF.type != type.type; // !isTransparent(fF.type);
                tK = fK.type != type.type; // !isTransparent(fK.type);
            } else if (isSemiTransparent(type.type)) {
                tT = tB = tL = tR = tF = tK = true;
            } else {
                blockData[type.type].render();
                printf("Render failed : %d\n", type.type); // TODO call special render mode
            }
        
            sz = tT + tB + tL + tR + tF + tK;
            if (sz) {
        
                lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
                lB = fB.naturalLight;
                lL = fL.naturalLight;
                lR = fR.naturalLight;
                lF = fF.naturalLight;
                lK = fK.naturalLight;
        
                if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                    A = !isTransparent(cwest.blocks[15][My][1].type);
                    B = !isTransparent(csouthwest.blocks[15][My][15].type);
                    C = !isTransparent(csouth.blocks[1][My][15].type);
                    D = !isTransparent(c.blocks[1][My][1].type);
                    E = !isTransparent(cwest.blocks[15][my][1].type);
                    F = !isTransparent(csouthwest.blocks[15][my][15].type);
                    G = !isTransparent(csouth.blocks[1][my][15].type);
                    H = !isTransparent(c.blocks[1][my][1].type);
        
                    AB = !isTransparent(cwest.blocks[15][My][0].type);
                    BC = !isTransparent(csouth.blocks[0][My][15].type);
                    CD = !isTransparent(c.blocks[1][My][0].type);
                    DA = !isTransparent(c.blocks[0][My][1].type);
        
                    EF = !isTransparent(cwest.blocks[15][my][0].type);
                    FG = !isTransparent(csouth.blocks[0][my][15].type);
                    GH = !isTransparent(c.blocks[1][my][0].type);
                    HE = !isTransparent(c.blocks[0][my][1].type);
        
                    AE = !isTransparent(cwest.blocks[15][y][1].type);
                    BF = !isTransparent(csouthwest.blocks[15][y][15].type);
                    CG = !isTransparent(csouth.blocks[1][y][15].type);
                    DH = !isTransparent(c.blocks[1][y][1].type);
                } else if (isTransparent(type.type)) {
                    // TODO: try some fun here
                    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                }
        
        
                //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                f32 fx = (f32)(px),
                    fy = (f32)y,
                    fz = (f32)(pz),
                    fmx = (f32)(-1 + px),
                    fmy = (f32)my,
                    fmz = (f32)(-1 + pz);
                    
                if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
                if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
                if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
                if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
                if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
                if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
                //GX_End();
            }
        }


        // X 0 Z 15
        if ((type = c.blocks[0][y][15]).type) {
        
            fT = c.blocks[0][My][15];
            fB = c.blocks[0][my][15];
            fL = cwest.blocks[15][y][15];
            fR = c.blocks[1][y][15];
            fF = cnorth.blocks[0][y][0];
            fK = c.blocks[0][y][14];
        
            if (isOpaque(type.type)) {
                tT = !isOpaque(fT.type);
                tB = !isOpaque(fB.type);
                tL = !isOpaque(fL.type);
                tR = !isOpaque(fR.type);
                tF = !isOpaque(fF.type);
                tK = !isOpaque(fK.type);
            }
            else if (isTransparent(type.type)) {
                tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
                tB = fB.type != type.type; // !isTransparent(fB.type);
                tL = fL.type != type.type; // !isTransparent(fL.type);
                tR = fR.type != type.type; // !isTransparent(fR.type);
                tF = fF.type != type.type; // !isTransparent(fF.type);
                tK = fK.type != type.type; // !isTransparent(fK.type);
            } else if (isSemiTransparent(type.type)) {
                tT = tB = tL = tR = tF = tK = true;
            } else {
                blockData[type.type].render();
                printf("Render failed : %d\n", type.type); // TODO call special render mode
            }
        
            sz = tT + tB + tL + tR + tF + tK;
            if (sz) {
        
                lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
                lB = fB.naturalLight;
                lL = fL.naturalLight;
                lR = fR.naturalLight;
                lF = fF.naturalLight;
                lK = fK.naturalLight;
        
                if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                    A = !isTransparent(cnorthwest.blocks[15][My][0].type);
                    B = !isTransparent(cwest.blocks[15][My][14].type);
                    C = !isTransparent(c.blocks[1][My][14].type);
                    D = !isTransparent(cnorth.blocks[1][My][0].type);
                    E = !isTransparent(cnorthwest.blocks[15][my][0].type);
                    F = !isTransparent(cwest.blocks[15][my][14].type);
                    G = !isTransparent(c.blocks[1][my][14].type);
                    H = !isTransparent(cnorth.blocks[1][my][0].type);
        
                    AB = !isTransparent(cwest.blocks[15][My][15].type);
                    BC = !isTransparent(c.blocks[0][My][14].type);
                    CD = !isTransparent(c.blocks[1][My][15].type);
                    DA = !isTransparent(cnorth.blocks[0][My][0].type);
        
                    EF = !isTransparent(cwest.blocks[15][my][15].type);
                    FG = !isTransparent(c.blocks[0][my][14].type);
                    GH = !isTransparent(c.blocks[1][my][15].type);
                    HE = !isTransparent(cnorth.blocks[0][my][0].type);
        
                    AE = !isTransparent(cnorthwest.blocks[15][y][0].type);
                    BF = !isTransparent(cwest.blocks[15][y][14].type);
                    CG = !isTransparent(c.blocks[1][y][14].type);
                    DH = !isTransparent(cnorth.blocks[1][y][0].type);
                } else if (isTransparent(type.type)) {
                    // TODO: try some fun here
                    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                }
        
        
                //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                f32 fx = (f32)(px),
                    fy = (f32)y,
                    fz = (f32)(15 + pz),
                    fmx = (f32)(-1 + px),
                    fmy = (f32)my,
                    fmz = (f32)(14 + pz);
                    
                if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
                if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
                if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
                if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
                if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
                if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
                //GX_End();
            }
        }


        // X 15 Z 0
        if ((type = c.blocks[15][y][0]).type) {
        
            fT = c.blocks[15][My][0];
            fB = c.blocks[15][my][0];
            fL = c.blocks[14][y][0];
            fR = ceast.blocks[0][y][0];
            fF = c.blocks[15][y][1];
            fK = csouth.blocks[15][y][15];
        
            if (isOpaque(type.type)) {
                tT = !isOpaque(fT.type);
                tB = !isOpaque(fB.type);
                tL = !isOpaque(fL.type);
                tR = !isOpaque(fR.type);
                tF = !isOpaque(fF.type);
                tK = !isOpaque(fK.type);
            }
            else if (isTransparent(type.type)) {
                tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
                tB = fB.type != type.type; // !isTransparent(fB.type);
                tL = fL.type != type.type; // !isTransparent(fL.type);
                tR = fR.type != type.type; // !isTransparent(fR.type);
                tF = fF.type != type.type; // !isTransparent(fF.type);
                tK = fK.type != type.type; // !isTransparent(fK.type);
            } else if (isSemiTransparent(type.type)) {
                tT = tB = tL = tR = tF = tK = true;
            } else {
                blockData[type.type].render();
                printf("Render failed : %d\n", type.type); // TODO call special render mode
            }
        
            sz = tT + tB + tL + tR + tF + tK;
            if (sz) {
        
                lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
                lB = fB.naturalLight;
                lL = fL.naturalLight;
                lR = fR.naturalLight;
                lF = fF.naturalLight;
                lK = fK.naturalLight;
        
                if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                    A = !isTransparent(c.blocks[14][My][1].type);
                    B = !isTransparent(csouth.blocks[14][My][15].type);
                    C = !isTransparent(csoutheast.blocks[0][My][15].type);
                    D = !isTransparent(ceast.blocks[0][My][1].type);
                    E = !isTransparent(c.blocks[14][my][1].type);
                    F = !isTransparent(csouth.blocks[14][my][15].type);
                    G = !isTransparent(csoutheast.blocks[0][my][15].type);
                    H = !isTransparent(ceast.blocks[0][my][1].type);
        
                    AB = !isTransparent(c.blocks[14][My][0].type);
                    BC = !isTransparent(csouth.blocks[15][My][15].type);
                    CD = !isTransparent(ceast.blocks[0][My][0].type);
                    DA = !isTransparent(c.blocks[15][My][1].type);
        
                    EF = !isTransparent(c.blocks[14][my][0].type);
                    FG = !isTransparent(csouth.blocks[15][my][15].type);
                    GH = !isTransparent(ceast.blocks[0][my][0].type);
                    HE = !isTransparent(c.blocks[15][my][1].type);
        
                    AE = !isTransparent(c.blocks[14][y][1].type);
                    BF = !isTransparent(csouth.blocks[14][y][15].type);
                    CG = !isTransparent(csoutheast.blocks[0][y][15].type);
                    DH = !isTransparent(ceast.blocks[0][y][1].type);
                } else if (isTransparent(type.type)) {
                    // TODO: try some fun here
                    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                }
        
        
                //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                f32 fx = (f32)(15 + px),
                    fy = (f32)y,
                    fz = (f32)(pz),
                    fmx = (f32)(14 + px),
                    fmy = (f32)my,
                    fmz = (f32)(-1 + pz);
                    
                if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
                if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
                if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
                if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
                if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
                if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
                //GX_End();
            }
        }


        // X 15 Z 15
        if ((type = c.blocks[15][y][15]).type) {
        
            fT = c.blocks[15][My][15];
            fB = c.blocks[15][my][15];
            fL = c.blocks[14][y][15];
            fR = ceast.blocks[0][y][15];
            fF = cnorth.blocks[15][y][0];
            fK = c.blocks[15][y][14];
        
            if (isOpaque(type.type)) {
                tT = !isOpaque(fT.type);
                tB = !isOpaque(fB.type);
                tL = !isOpaque(fL.type);
                tR = !isOpaque(fR.type);
                tF = !isOpaque(fF.type);
                tK = !isOpaque(fK.type);
            }
            else if (isTransparent(type.type)) {
                tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
                tB = fB.type != type.type; // !isTransparent(fB.type);
                tL = fL.type != type.type; // !isTransparent(fL.type);
                tR = fR.type != type.type; // !isTransparent(fR.type);
                tF = fF.type != type.type; // !isTransparent(fF.type);
                tK = fK.type != type.type; // !isTransparent(fK.type);
            } else if (isSemiTransparent(type.type)) {
                tT = tB = tL = tR = tF = tK = true;
            } else {
                blockData[type.type].render();
                printf("Render failed : %d\n", type.type); // TODO call special render mode
            }
        
            sz = tT + tB + tL + tR + tF + tK;
            if (sz) {
        
                lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
                lB = fB.naturalLight;
                lL = fL.naturalLight;
                lR = fR.naturalLight;
                lF = fF.naturalLight;
                lK = fK.naturalLight;
        
                if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                    A = !isTransparent(cnorth.blocks[14][My][0].type);
                    B = !isTransparent(c.blocks[14][My][14].type);
                    C = !isTransparent(ceast.blocks[0][My][14].type);
                    D = !isTransparent(cnortheast.blocks[0][My][0].type);
                    E = !isTransparent(cnorth.blocks[14][my][0].type);
                    F = !isTransparent(c.blocks[14][my][14].type);
                    G = !isTransparent(ceast.blocks[0][my][14].type);
                    H = !isTransparent(cnortheast.blocks[0][my][0].type);
        
                    AB = !isTransparent(c.blocks[14][My][15].type);
                    BC = !isTransparent(c.blocks[15][My][14].type);
                    CD = !isTransparent(ceast.blocks[0][My][15].type);
                    DA = !isTransparent(cnorth.blocks[15][My][0].type);
        
                    EF = !isTransparent(c.blocks[14][my][15].type);
                    FG = !isTransparent(c.blocks[15][my][14].type);
                    GH = !isTransparent(ceast.blocks[0][my][15].type);
                    HE = !isTransparent(cnorth.blocks[15][my][0].type);
        
                    AE = !isTransparent(cnorth.blocks[14][y][0].type);
                    BF = !isTransparent(c.blocks[14][y][14].type);
                    CG = !isTransparent(ceast.blocks[0][y][14].type);
                    DH = !isTransparent(cnortheast.blocks[0][y][0].type);
                } else if (isTransparent(type.type)) {
                    // TODO: try some fun here
                    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                }
        
        
                //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                f32 fx = (f32)(15 + px),
                    fy = (f32)y,
                    fz = (f32)(15 + pz),
                    fmx = (f32)(14 + px),
                    fmy = (f32)my,
                    fmz = (f32)(14 + pz);
                    
                if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
                if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
                if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
                if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
                if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
                if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
                //GX_End();
            }
        }


        // X x Z 0
        for(x = 1; x < 15; x++) {
            if ((type = c.blocks[x][y][0]).type) {
                mx = x - 1;
                Mx = x + 1;
            
                fT = c.blocks[x][My][0];
                fB = c.blocks[x][my][0];
                fL = c.blocks[mx][y][0];
                fR = c.blocks[Mx][y][0];
                fF = c.blocks[x][y][1];
                fK = csouth.blocks[x][y][15];
            
                if (isOpaque(type.type)) {
                    tT = !isOpaque(fT.type);
                    tB = !isOpaque(fB.type);
                    tL = !isOpaque(fL.type);
                    tR = !isOpaque(fR.type);
                    tF = !isOpaque(fF.type);
                    tK = !isOpaque(fK.type);
                }
                else if (isTransparent(type.type)) {
                    tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
                    tB = fB.type != type.type; // !isTransparent(fB.type);
                    tL = fL.type != type.type; // !isTransparent(fL.type);
                    tR = fR.type != type.type; // !isTransparent(fR.type);
                    tF = fF.type != type.type; // !isTransparent(fF.type);
                    tK = fK.type != type.type; // !isTransparent(fK.type);
                } else if (isSemiTransparent(type.type)) {
                    tT = tB = tL = tR = tF = tK = true;
                } else {
                    blockData[type.type].render();
                    printf("Render failed : %d\n", type.type); // TODO call special render mode
                }
            
                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
            
                    lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
                    lB = fB.naturalLight;
                    lL = fL.naturalLight;
                    lR = fR.naturalLight;
                    lF = fF.naturalLight;
                    lK = fK.naturalLight;
            
                    if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                        A = !isTransparent(c.blocks[mx][My][1].type);
                        B = !isTransparent(csouth.blocks[mx][My][15].type);
                        C = !isTransparent(csouth.blocks[Mx][My][15].type);
                        D = !isTransparent(c.blocks[Mx][My][1].type);
                        E = !isTransparent(c.blocks[mx][my][1].type);
                        F = !isTransparent(csouth.blocks[mx][my][15].type);
                        G = !isTransparent(csouth.blocks[Mx][my][15].type);
                        H = !isTransparent(c.blocks[Mx][my][1].type);
            
                        AB = !isTransparent(c.blocks[mx][My][0].type);
                        BC = !isTransparent(csouth.blocks[x][My][15].type);
                        CD = !isTransparent(c.blocks[Mx][My][0].type);
                        DA = !isTransparent(c.blocks[x][My][1].type);
            
                        EF = !isTransparent(c.blocks[mx][my][0].type);
                        FG = !isTransparent(csouth.blocks[x][my][15].type);
                        GH = !isTransparent(c.blocks[Mx][my][0].type);
                        HE = !isTransparent(c.blocks[x][my][1].type);
            
                        AE = !isTransparent(c.blocks[mx][y][1].type);
                        BF = !isTransparent(csouth.blocks[mx][y][15].type);
                        CG = !isTransparent(csouth.blocks[Mx][y][15].type);
                        DH = !isTransparent(c.blocks[Mx][y][1].type);
                    } else if (isTransparent(type.type)) {
                        // TODO: try some fun here
                        A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                    }
            
            
                    //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                    f32 fx = (f32)(x + px),
                        fy = (f32)y,
                        fz = (f32)(pz),
                        fmx = (f32)(mx + px),
                        fmy = (f32)my,
                        fmz = (f32)(-1 + pz);
                        
                    if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
                    if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
                    if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
                    if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
                    if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
                    if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
                    //GX_End();
                }
            }
        }


        // X x Z 15
        for(x = 1; x < 15; x++) {
            if ((type = c.blocks[x][y][15]).type) {
                mx = x - 1;
                Mx = x + 1;
            
                fT = c.blocks[x][My][15];
                fB = c.blocks[x][my][15];
                fL = c.blocks[mx][y][15];
                fR = c.blocks[Mx][y][15];
                fF = cnorth.blocks[x][y][0];
                fK = c.blocks[x][y][14];
            
                if (isOpaque(type.type)) {
                    tT = !isOpaque(fT.type);
                    tB = !isOpaque(fB.type);
                    tL = !isOpaque(fL.type);
                    tR = !isOpaque(fR.type);
                    tF = !isOpaque(fF.type);
                    tK = !isOpaque(fK.type);
                }
                else if (isTransparent(type.type)) {
                    tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
                    tB = fB.type != type.type; // !isTransparent(fB.type);
                    tL = fL.type != type.type; // !isTransparent(fL.type);
                    tR = fR.type != type.type; // !isTransparent(fR.type);
                    tF = fF.type != type.type; // !isTransparent(fF.type);
                    tK = fK.type != type.type; // !isTransparent(fK.type);
                } else if (isSemiTransparent(type.type)) {
                    tT = tB = tL = tR = tF = tK = true;
                } else {
                    blockData[type.type].render();
                    printf("Render failed : %d\n", type.type); // TODO call special render mode
                }
            
                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
            
                    lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
                    lB = fB.naturalLight;
                    lL = fL.naturalLight;
                    lR = fR.naturalLight;
                    lF = fF.naturalLight;
                    lK = fK.naturalLight;
            
                    if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                        A = !isTransparent(cnorth.blocks[mx][My][0].type);
                        B = !isTransparent(c.blocks[mx][My][14].type);
                        C = !isTransparent(c.blocks[Mx][My][14].type);
                        D = !isTransparent(cnorth.blocks[Mx][My][0].type);
                        E = !isTransparent(cnorth.blocks[mx][my][0].type);
                        F = !isTransparent(c.blocks[mx][my][14].type);
                        G = !isTransparent(c.blocks[Mx][my][14].type);
                        H = !isTransparent(cnorth.blocks[Mx][my][0].type);
            
                        AB = !isTransparent(c.blocks[mx][My][15].type);
                        BC = !isTransparent(c.blocks[x][My][14].type);
                        CD = !isTransparent(c.blocks[Mx][My][15].type);
                        DA = !isTransparent(cnorth.blocks[x][My][0].type);
            
                        EF = !isTransparent(c.blocks[mx][my][15].type);
                        FG = !isTransparent(c.blocks[x][my][14].type);
                        GH = !isTransparent(c.blocks[Mx][my][15].type);
                        HE = !isTransparent(cnorth.blocks[x][my][0].type);
            
                        AE = !isTransparent(cnorth.blocks[mx][y][0].type);
                        BF = !isTransparent(c.blocks[mx][y][14].type);
                        CG = !isTransparent(c.blocks[Mx][y][14].type);
                        DH = !isTransparent(cnorth.blocks[Mx][y][0].type);
                    } else if (isTransparent(type.type)) {
                        // TODO: try some fun here
                        A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                    }
            
            
                    //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                    f32 fx = (f32)(x + px),
                        fy = (f32)y,
                        fz = (f32)(15 + pz),
                        fmx = (f32)(mx + px),
                        fmy = (f32)my,
                        fmz = (f32)(14 + pz);
                        
                    if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
                    if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
                    if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
                    if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
                    if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
                    if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
                    //GX_End();
                }
            }
        }


        // X 0 Z z
        for(z = 1; z < 15; z++) {
            if ((type = c.blocks[0][y][z]).type) {
                mz = z - 1;
                Mz = z + 1;
            
                fT = c.blocks[0][My][z];
                fB = c.blocks[0][my][z];
                fL = cwest.blocks[15][y][z];
                fR = c.blocks[1][y][z];
                fF = c.blocks[0][y][Mz];
                fK = c.blocks[0][y][mz];
            
                if (isOpaque(type.type)) {
                    tT = !isOpaque(fT.type);
                    tB = !isOpaque(fB.type);
                    tL = !isOpaque(fL.type);
                    tR = !isOpaque(fR.type);
                    tF = !isOpaque(fF.type);
                    tK = !isOpaque(fK.type);
                }
                else if (isTransparent(type.type)) {
                    tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
                    tB = fB.type != type.type; // !isTransparent(fB.type);
                    tL = fL.type != type.type; // !isTransparent(fL.type);
                    tR = fR.type != type.type; // !isTransparent(fR.type);
                    tF = fF.type != type.type; // !isTransparent(fF.type);
                    tK = fK.type != type.type; // !isTransparent(fK.type);
                } else if (isSemiTransparent(type.type)) {
                    tT = tB = tL = tR = tF = tK = true;
                } else {
                    blockData[type.type].render();
                    printf("Render failed : %d\n", type.type); // TODO call special render mode
                }
            
                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
            
                    lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
                    lB = fB.naturalLight;
                    lL = fL.naturalLight;
                    lR = fR.naturalLight;
                    lF = fF.naturalLight;
                    lK = fK.naturalLight;
            
                    if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                        A = !isTransparent(cwest.blocks[15][My][Mz].type);
                        B = !isTransparent(cwest.blocks[15][My][mz].type);
                        C = !isTransparent(c.blocks[1][My][mz].type);
                        D = !isTransparent(c.blocks[1][My][Mz].type);
                        E = !isTransparent(cwest.blocks[15][my][Mz].type);
                        F = !isTransparent(cwest.blocks[15][my][mz].type);
                        G = !isTransparent(c.blocks[1][my][mz].type);
                        H = !isTransparent(c.blocks[1][my][Mz].type);
            
                        AB = !isTransparent(cwest.blocks[15][My][z].type);
                        BC = !isTransparent(c.blocks[0][My][mz].type);
                        CD = !isTransparent(c.blocks[1][My][z].type);
                        DA = !isTransparent(c.blocks[0][My][Mz].type);
            
                        EF = !isTransparent(cwest.blocks[15][my][z].type);
                        FG = !isTransparent(c.blocks[0][my][mz].type);
                        GH = !isTransparent(c.blocks[1][my][z].type);
                        HE = !isTransparent(c.blocks[0][my][Mz].type);
            
                        AE = !isTransparent(cwest.blocks[15][y][Mz].type);
                        BF = !isTransparent(cwest.blocks[15][y][mz].type);
                        CG = !isTransparent(c.blocks[1][y][mz].type);
                        DH = !isTransparent(c.blocks[1][y][Mz].type);
                    } else if (isTransparent(type.type)) {
                        // TODO: try some fun here
                        A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                    }
            
            
                    //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                    f32 fx = (f32)(px),
                        fy = (f32)y,
                        fz = (f32)(z + pz),
                        fmx = (f32)(-1 + px),
                        fmy = (f32)my,
                        fmz = (f32)(mz + pz);
                        
                    if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
                    if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
                    if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
                    if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
                    if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
                    if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
                    //GX_End();
                }
            }
        }


        // X 15 Z z
        for(z = 1; z < 15; z++) {
            if ((type = c.blocks[15][y][z]).type) {
                mz = z - 1;
                Mz = z + 1;
            
                fT = c.blocks[15][My][z];
                fB = c.blocks[15][my][z];
                fL = c.blocks[14][y][z];
                fR = ceast.blocks[0][y][z];
                fF = c.blocks[15][y][Mz];
                fK = c.blocks[15][y][mz];
            
                if (isOpaque(type.type)) {
                    tT = !isOpaque(fT.type);
                    tB = !isOpaque(fB.type);
                    tL = !isOpaque(fL.type);
                    tR = !isOpaque(fR.type);
                    tF = !isOpaque(fF.type);
                    tK = !isOpaque(fK.type);
                }
                else if (isTransparent(type.type)) {
                    tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
                    tB = fB.type != type.type; // !isTransparent(fB.type);
                    tL = fL.type != type.type; // !isTransparent(fL.type);
                    tR = fR.type != type.type; // !isTransparent(fR.type);
                    tF = fF.type != type.type; // !isTransparent(fF.type);
                    tK = fK.type != type.type; // !isTransparent(fK.type);
                } else if (isSemiTransparent(type.type)) {
                    tT = tB = tL = tR = tF = tK = true;
                } else {
                    blockData[type.type].render();
                    printf("Render failed : %d\n", type.type); // TODO call special render mode
                }
            
                sz = tT + tB + tL + tR + tF + tK;
                if (sz) {
            
                    lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
                    lB = fB.naturalLight;
                    lL = fL.naturalLight;
                    lR = fR.naturalLight;
                    lF = fF.naturalLight;
                    lK = fK.naturalLight;
            
                    if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                        A = !isTransparent(c.blocks[14][My][Mz].type);
                        B = !isTransparent(c.blocks[14][My][mz].type);
                        C = !isTransparent(ceast.blocks[0][My][mz].type);
                        D = !isTransparent(ceast.blocks[0][My][Mz].type);
                        E = !isTransparent(c.blocks[14][my][Mz].type);
                        F = !isTransparent(c.blocks[14][my][mz].type);
                        G = !isTransparent(ceast.blocks[0][my][mz].type);
                        H = !isTransparent(ceast.blocks[0][my][Mz].type);
            
                        AB = !isTransparent(c.blocks[14][My][z].type);
                        BC = !isTransparent(c.blocks[15][My][mz].type);
                        CD = !isTransparent(ceast.blocks[0][My][z].type);
                        DA = !isTransparent(c.blocks[15][My][Mz].type);
            
                        EF = !isTransparent(c.blocks[14][my][z].type);
                        FG = !isTransparent(c.blocks[15][my][mz].type);
                        GH = !isTransparent(ceast.blocks[0][my][z].type);
                        HE = !isTransparent(c.blocks[15][my][Mz].type);
            
                        AE = !isTransparent(c.blocks[14][y][Mz].type);
                        BF = !isTransparent(c.blocks[14][y][mz].type);
                        CG = !isTransparent(ceast.blocks[0][y][mz].type);
                        DH = !isTransparent(ceast.blocks[0][y][Mz].type);
                    } else if (isTransparent(type.type)) {
                        // TODO: try some fun here
                        A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                    }
            
            
                    //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                    f32 fx = (f32)(15 + px),
                        fy = (f32)y,
                        fz = (f32)(z + pz),
                        fmx = (f32)(14 + px),
                        fmy = (f32)my,
                        fmz = (f32)(mz + pz);
                        
                    if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
                    if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
                    if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
                    if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
                    if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
                    if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
                    //GX_End();
                }
            }
        }


        center:
        
        // X x Z z
        for(x = 1; x < 15; x++) {
            for(z = 1; z < 15; z++) {
                if ((type = c.blocks[x][y][z]).type) {
                    mx = x - 1;
                    Mx = x + 1;
                    mz = z - 1;
                    Mz = z + 1;
                
                    fT = c.blocks[x][My][z];
                    fB = c.blocks[x][my][z];
                    fL = c.blocks[mx][y][z];
                    fR = c.blocks[Mx][y][z];
                    fF = c.blocks[x][y][Mz];
                    fK = c.blocks[x][y][mz];
                
                    if (isOpaque(type.type)) {
                        tT = !isOpaque(fT.type);
                        tB = !isOpaque(fB.type);
                        tL = !isOpaque(fL.type);
                        tR = !isOpaque(fR.type);
                        tF = !isOpaque(fF.type);
                        tK = !isOpaque(fK.type);
                    }
                    else if (isTransparent(type.type)) {
                        tT = fT.type != type.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
                        tB = fB.type != type.type; // !isTransparent(fB.type);
                        tL = fL.type != type.type; // !isTransparent(fL.type);
                        tR = fR.type != type.type; // !isTransparent(fR.type);
                        tF = fF.type != type.type; // !isTransparent(fF.type);
                        tK = fK.type != type.type; // !isTransparent(fK.type);
                    } else if (isSemiTransparent(type.type)) {
                        tT = tB = tL = tR = tF = tK = true;
                    } else {
                        blockData[type.type].render();
                        printf("Render failed : %d\n", type.type); // TODO call special render mode
                    }
                
                    sz = tT + tB + tL + tR + tF + tK;
                    if (sz) {
                
                        lT = fT.naturalLight; // undefined corrupted light if 'fT' is not (semi-)transparent (context warranty: may never happen)
                        lB = fB.naturalLight;
                        lL = fL.naturalLight;
                        lR = fR.naturalLight;
                        lF = fF.naturalLight;
                        lK = fK.naturalLight;
                
                        if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                            A = !isTransparent(c.blocks[mx][My][Mz].type);
                            B = !isTransparent(c.blocks[mx][My][mz].type);
                            C = !isTransparent(c.blocks[Mx][My][mz].type);
                            D = !isTransparent(c.blocks[Mx][My][Mz].type);
                            E = !isTransparent(c.blocks[mx][my][Mz].type);
                            F = !isTransparent(c.blocks[mx][my][mz].type);
                            G = !isTransparent(c.blocks[Mx][my][mz].type);
                            H = !isTransparent(c.blocks[Mx][my][Mz].type);
                
                            AB = !isTransparent(c.blocks[mx][My][z].type);
                            BC = !isTransparent(c.blocks[x][My][mz].type);
                            CD = !isTransparent(c.blocks[Mx][My][z].type);
                            DA = !isTransparent(c.blocks[x][My][Mz].type);
                
                            EF = !isTransparent(c.blocks[mx][my][z].type);
                            FG = !isTransparent(c.blocks[x][my][mz].type);
                            GH = !isTransparent(c.blocks[Mx][my][z].type);
                            HE = !isTransparent(c.blocks[x][my][Mz].type);
                
                            AE = !isTransparent(c.blocks[mx][y][Mz].type);
                            BF = !isTransparent(c.blocks[mx][y][mz].type);
                            CG = !isTransparent(c.blocks[Mx][y][mz].type);
                            DH = !isTransparent(c.blocks[Mx][y][Mz].type);
                        } else if (isTransparent(type.type)) {
                            // TODO: try some fun here
                            A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                        }
                
                
                        //GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
                        f32 fx = (f32)(x + px),
                            fy = (f32)y,
                            fz = (f32)(z + pz),
                            fmx = (f32)(mx + px),
                            fmy = (f32)my,
                            fmz = (f32)(mz + pz);
                            
                        if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, type.type, C + BC + CD, D + CD + DA, A + AB + DA, B + BC + AB, lT, isTransparent(type.type)); // CDAB
                        if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, type.type, E + EF + HE, H + GH + HE, G + FG + GH, F + FG + EF, lB, isTransparent(type.type)); // EHGF
                        if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, type.type, B + AB + BF, A + AB + AE, E + EF + AE, F + BF + EF, lL, isTransparent(type.type)); // BAEF
                        if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, type.type, H + DH + GH, D + CD + DH, C + CD + CG, G + CG + GH, lR, isTransparent(type.type)); // HDCG
                        if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, type.type, A + AE + DA, D + DA + DH, H + DH + HE, E + HE + AE, lF, isTransparent(type.type)); // ADHE
                        if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, type.type, G + CG + FG, C + BC + CG, B + BC + BF, F + FG + BF, lK, isTransparent(type.type)); // GCBF
                        //GX_End();
                    }
                }
            }
        }
	}
}
