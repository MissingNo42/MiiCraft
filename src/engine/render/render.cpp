//
// Created by Romain on 15/02/2024.
//

#include "world/verticalChunk.h"
#include "render/cache.h"
#include "render/block.h"
#include "world/world.h"
#include "world/coord.h"


union LightContact {
	u16 contact;
	struct {
		u16 A: 4, B: 4, C: 4, D: 4;
	};
	struct {
		u16 G: 4, H: 4, E: 4, F: 4;
	};
};


enum LightContactIndex: u16 {
	A = 0b0001000000000000,
	B = 0b0000000100000000,
	C = 0b0000000000010000,
	D = 0b0000000000000001,
	E = C,
	F = D,
	G = A,
	H = B,
};



// shifted space from 16x16 (4-bits) to 64x64 (6-bits)
// color idx in 'Lights' array is the sum of at most 4 extendedLight's values:
// extendedLight[block.light = 0bnnnnaaaa] = 0b0000_00(nnnn + 1)_00(aaaa + 1) // the +1 is to apply a bonus on non-occluded lights
static const u16 extendedLight[0x100] = {
	0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047, 0x048, 0x049, 0x04A, 0x04B, 0x04C, 0x04D, 0x04E, 0x04F, 0x050,
	0x081, 0x082, 0x083, 0x084, 0x085, 0x086, 0x087, 0x088, 0x089, 0x08A, 0x08B, 0x08C, 0x08D, 0x08E, 0x08F, 0x090,
	0x0C1, 0x0C2, 0x0C3, 0x0C4, 0x0C5, 0x0C6, 0x0C7, 0x0C8, 0x0C9, 0x0CA, 0x0CB, 0x0CC, 0x0CD, 0x0CE, 0x0CF, 0x0D0,
	0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108, 0x109, 0x10A, 0x10B, 0x10C, 0x10D, 0x10E, 0x10F, 0x110,
	0x141, 0x142, 0x143, 0x144, 0x145, 0x146, 0x147, 0x148, 0x149, 0x14A, 0x14B, 0x14C, 0x14D, 0x14E, 0x14F, 0x150,
	0x181, 0x182, 0x183, 0x184, 0x185, 0x186, 0x187, 0x188, 0x189, 0x18A, 0x18B, 0x18C, 0x18D, 0x18E, 0x18F, 0x190,
	0x1C1, 0x1C2, 0x1C3, 0x1C4, 0x1C5, 0x1C6, 0x1C7, 0x1C8, 0x1C9, 0x1CA, 0x1CB, 0x1CC, 0x1CD, 0x1CE, 0x1CF, 0x1D0,
	0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208, 0x209, 0x20A, 0x20B, 0x20C, 0x20D, 0x20E, 0x20F, 0x210,
	0x241, 0x242, 0x243, 0x244, 0x245, 0x246, 0x247, 0x248, 0x249, 0x24A, 0x24B, 0x24C, 0x24D, 0x24E, 0x24F, 0x250,
	0x281, 0x282, 0x283, 0x284, 0x285, 0x286, 0x287, 0x288, 0x289, 0x28A, 0x28B, 0x28C, 0x28D, 0x28E, 0x28F, 0x290,
	0x2C1, 0x2C2, 0x2C3, 0x2C4, 0x2C5, 0x2C6, 0x2C7, 0x2C8, 0x2C9, 0x2CA, 0x2CB, 0x2CC, 0x2CD, 0x2CE, 0x2CF, 0x2D0,
	0x301, 0x302, 0x303, 0x304, 0x305, 0x306, 0x307, 0x308, 0x309, 0x30A, 0x30B, 0x30C, 0x30D, 0x30E, 0x30F, 0x310,
	0x341, 0x342, 0x343, 0x344, 0x345, 0x346, 0x347, 0x348, 0x349, 0x34A, 0x34B, 0x34C, 0x34D, 0x34E, 0x34F, 0x350,
	0x381, 0x382, 0x383, 0x384, 0x385, 0x386, 0x387, 0x388, 0x389, 0x38A, 0x38B, 0x38C, 0x38D, 0x38E, 0x38F, 0x390,
	0x3C1, 0x3C2, 0x3C3, 0x3C4, 0x3C5, 0x3C6, 0x3C7, 0x3C8, 0x3C9, 0x3CA, 0x3CB, 0x3CC, 0x3CD, 0x3CE, 0x3CF, 0x3D0,
	0x401, 0x402, 0x403, 0x404, 0x405, 0x406, 0x407, 0x408, 0x409, 0x40A, 0x40B, 0x40C, 0x40D, 0x40E, 0x40F, 0x410,
};

static constexpr u16 initLight  = 0b1111111110111111; // ~0000_01_0000_01 | 1
static constexpr u16 bonusLight = 0b0000000001000001;

inline void renderVertex(f32 x, f32 y, f32 z, u16 tc, u16 color, u8 alpha) {
    //GX_Position3f32(x, y, z);
    //GX_Normal1x8(normal);
    //GX_Color1u32(color);
    //GX_TexCoord2f32(tx, ty);
	ChunkCache::addVertex(x, y, z, color, tc, alpha);
}

inline void renderRawNorth(f32 x, f32 y, f32 z, f32 mx, f32 my, f32, u16 tcA, u16 tcD, u16 tcH, u16 tcE, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	renderVertex(mx, y, z, tcA, c1, alpha); // A
	renderVertex(x, y, z, tcD, c2, alpha); // D
	renderVertex(x, my, z, tcH, c3, alpha); // H
	renderVertex(mx, my, z, tcE, c4, alpha); // E
}

inline void renderRawSouth(f32 x, f32 y, f32, f32 mx, f32 my, f32 mz, u16 tcG, u16 tcC, u16 tcB, u16 tcF, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	renderVertex(x, my, mz, tcG, c1, alpha); // G
	renderVertex(x, y, mz, tcC, c2, alpha); // C
	renderVertex(mx, y, mz, tcB, c3, alpha); // B
	renderVertex(mx, my, mz, tcF, c4, alpha); // F
}

inline void renderRawTop(f32 x, f32 y, f32 z, f32 mx, f32, f32 mz, u16 tcC, u16 tcD, u16 tcA, u16 tcB, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	renderVertex(x, y, mz, tcC, c1, alpha); // C
	renderVertex(x, y, z, tcD, c2, alpha); // D
	renderVertex(mx, y, z, tcA, c3, alpha); // A
	renderVertex(mx, y, mz, tcB, c4, alpha); // B
}

inline void renderRawBottom(f32 x, f32, f32 z, f32 mx, f32 my, f32 mz, u16 tcE, u16 tcH, u16 tcG, u16 tcF, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	renderVertex(mx, my, z, tcE, c1, alpha); // E
	renderVertex(x, my, z, tcH, c2, alpha); // H
	renderVertex(x, my, mz, tcG, c3, alpha); // G
	renderVertex(mx, my, mz, tcF, c4, alpha); // F
}

inline void renderRawWest(f32, f32 y, f32 z, f32 mx, f32 my, f32 mz, u16 tcB, u16 tcA, u16 tcE, u16 tcF, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	renderVertex(mx, y, mz, tcB, c1, alpha); // B
	renderVertex(mx, y, z, tcA, c2, alpha); // A
	renderVertex(mx, my, z, tcE, c3, alpha); // E
	renderVertex(mx, my, mz, tcF, c4, alpha); // F
}

inline void renderRawEast(f32 x, f32 y, f32 z, f32, f32 my, f32 mz, u16 tcH, u16 tcD, u16 tcC, u16 tcG, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	renderVertex(x, my, z, tcH, c1, alpha); // H
	renderVertex(x, y, z, tcD, c2, alpha); // D
	renderVertex(x, y, mz, tcC, c3, alpha); // C
	renderVertex(x, my, mz, tcG, c4, alpha); // G
}

inline void renderFront(f32 x, f32 y, f32 z, f32 mx, f32 my, f32, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	u16 tc = blockData[type].tc[BlockFace::North];
    renderVertex(mx, y, z, tc, c1, alpha); // A
    renderVertex(x, y, z, tc + 17, c2, alpha); // D
    renderVertex(x, my, z, tc + 18, c3, alpha); // H
    renderVertex(mx, my, z, tc + 1, c4, alpha); // E
}

inline void renderBack(f32 x, f32 y, f32, f32 mx, f32 my, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	u16 tc = blockData[type].tc[BlockFace::South];
    renderVertex(x, my, mz, tc + 1, c1, alpha); // G
    renderVertex(x, y, mz, tc, c2, alpha); // C
    renderVertex(mx, y, mz, tc + 17, c3, alpha); // B
    renderVertex(mx, my, mz, tc + 18, c4, alpha); // F
}

inline void renderTop(f32 x, f32 y, f32 z, f32 mx, f32, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
    u16 tc = blockData[type].tc[BlockFace::Top];
    renderVertex(x, y, mz, tc + 17, c1, alpha); // C
    renderVertex(x, y, z, tc + 18, c2, alpha); // D
    renderVertex(mx, y, z, tc + 1, c3, alpha); // A
    renderVertex(mx, y, mz, tc, c4, alpha); // B
}

inline void renderBottom(f32 x, f32, f32 z, f32 mx, f32 my, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	u16 tc = blockData[type].tc[BlockFace::Bottom];
    renderVertex(mx, my, z, tc + 18, c1, alpha); // E
    renderVertex(x, my, z, tc + 1, c2, alpha); // H
    renderVertex(x, my, mz, tc, c3, alpha); // G
    renderVertex(mx, my, mz, tc + 17, c4, alpha); // F
}

inline void renderLeft(f32, f32 y, f32 z, f32 mx, f32 my, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	u16 tc = blockData[type].tc[BlockFace::West];
    renderVertex(mx, y, mz, tc, c1, alpha); // B
    renderVertex(mx, y, z, tc + 17, c2, alpha); // A
    renderVertex(mx, my, z, tc + 18, c3, alpha); // E
    renderVertex(mx, my, mz, tc + 1, c4, alpha); // F
}

inline void renderRight(f32 x, f32 y, f32 z, f32, f32 my, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, u8 alpha) {
	u16 tc = blockData[type].tc[BlockFace::East];
    renderVertex(x, my, z, tc + 1, c1, alpha); // H
    renderVertex(x, y, z, tc, c2, alpha); // D
    renderVertex(x, y, mz, tc + 17, c3, alpha); // C
    renderVertex(x, my, mz, tc + 18, c4, alpha); // G
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
	s32 px = c.coord.x << 4;
	s32 pz = c.coord.y << 4;
	s32 x, y, z, sz; // block coord and number of rendered faces (for regurlar)
	s32 mx, my, mz, Mx, My, Mz; // alternate block coord
	void * target = nullptr, * endTarget = nullptr; // renderer sub-functions jumpers

	Block A, B, C, D, E, F, G, H; // render cubes with X vertex in common
	Block AB, BC, CD, DA,  // render cubes with X, Y vertices in common
	      EF, FG, GH, HE,
		  AE, BF, CG, DH;
	Block fT, fB, fL, fR, fF, fK, block; // block type of the 6 adjacent blocks + the current one
	bool tT, tB, tL, tR, tF, tK; // true if the current block's faces must be rendered
	u8 lT, lB, lL, lR, lF, lK; // light level of the 6 adjacent blocks
	f32 fx, fy, fz, fmx, fmy, fmz; // float block coord
	
	u16 l_tC = 0, l_tD = 0, l_tA = 0, l_tB = 0;
	u16 l_bE = 0, l_bH = 0, l_bG = 0, l_bF = 0;
	u16 l_lB = 0, l_lA = 0, l_lE = 0, l_lF = 0;
	u16 l_rH = 0, l_rD = 0, l_rC = 0, l_rG = 0;
	u16 l_fA = 0, l_fD = 0, l_fH = 0, l_fE = 0;
	u16 l_kG = 0, l_kC = 0, l_kB = 0, l_kF = 0;
	
	LightContact cT, cB, cL, cR, cF, cK; // light contacts of the 6 faces

	VerticalChunk &cnorth = World::chunkSlots[c.neighboors[Direction::NORTH]];
	VerticalChunk &csouth = World::chunkSlots[c.neighboors[Direction::SOUTH]];
	VerticalChunk &ceast = World::chunkSlots[c.neighboors[Direction::EAST]];
	VerticalChunk &cwest = World::chunkSlots[c.neighboors[Direction::WEST]];

	VerticalChunk &cnortheast = World::chunkSlots[cnorth.neighboors[Direction::EAST]];
	VerticalChunk &cnorthwest = World::chunkSlots[cnorth.neighboors[Direction::WEST]];
	VerticalChunk &csoutheast = World::chunkSlots[csouth.neighboors[Direction::EAST]];
	VerticalChunk &csouthwest = World::chunkSlots[csouth.neighboors[Direction::WEST]];


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

#pragma region autogen // python auto-gen code (gen-render.py)
	        // X 0 Z 0
        if ((block = c.blocks[0][y][0]).type) {

            fT = c.blocks[0][My][0];
            fB = c.blocks[0][my][0];
            fL = cwest.blocks[15][y][0];
            fR = c.blocks[1][y][0];
            fF = c.blocks[0][y][1];
            fK = csouth.blocks[0][y][15];

            target = &&light_x0_z0;
            endTarget = &&end_x0_z0;
            goto prepare;

            light_x0_z0:

            //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
            A = cwest.blocks[15][My][1];
            B = csouthwest.blocks[15][My][15];
            C = csouth.blocks[1][My][15];
            D = c.blocks[1][My][1];
            E = cwest.blocks[15][my][1];
            F = csouthwest.blocks[15][my][15];
            G = csouth.blocks[1][my][15];
            H = c.blocks[1][my][1];

            AB = cwest.blocks[15][My][0];
            BC = csouth.blocks[0][My][15];
            CD = c.blocks[1][My][0];
            DA = c.blocks[0][My][1];

            EF = cwest.blocks[15][my][0];
            FG = csouth.blocks[0][my][15];
            GH = c.blocks[1][my][0];
            HE = c.blocks[0][my][1];

            AE = cwest.blocks[15][y][1];
            BF = csouthwest.blocks[15][y][15];
            CG = csouth.blocks[1][y][15];
            DH = c.blocks[1][y][1];
            //} else if (isTransparent(type.type)) {
            //    // TODO: try some fun here
            //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
            //}

            fx = (f32)(px),
            fy = (f32)y,
            fz = (f32)(pz),
            fmx = (f32)(-1 + px),
            fmy = (f32)my,
            fmz = (f32)(-1 + pz);

            goto render;
            end_x0_z0:;
        }


        // X 0 Z 15
        if ((block = c.blocks[0][y][15]).type) {

            fT = c.blocks[0][My][15];
            fB = c.blocks[0][my][15];
            fL = cwest.blocks[15][y][15];
            fR = c.blocks[1][y][15];
            fF = cnorth.blocks[0][y][0];
            fK = c.blocks[0][y][14];

            target = &&light_x0_z15;
            endTarget = &&end_x0_z15;
            goto prepare;

            light_x0_z15:

            //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
            A = cnorthwest.blocks[15][My][0];
            B = cwest.blocks[15][My][14];
            C = c.blocks[1][My][14];
            D = cnorth.blocks[1][My][0];
            E = cnorthwest.blocks[15][my][0];
            F = cwest.blocks[15][my][14];
            G = c.blocks[1][my][14];
            H = cnorth.blocks[1][my][0];

            AB = cwest.blocks[15][My][15];
            BC = c.blocks[0][My][14];
            CD = c.blocks[1][My][15];
            DA = cnorth.blocks[0][My][0];

            EF = cwest.blocks[15][my][15];
            FG = c.blocks[0][my][14];
            GH = c.blocks[1][my][15];
            HE = cnorth.blocks[0][my][0];

            AE = cnorthwest.blocks[15][y][0];
            BF = cwest.blocks[15][y][14];
            CG = c.blocks[1][y][14];
            DH = cnorth.blocks[1][y][0];
            //} else if (isTransparent(type.type)) {
            //    // TODO: try some fun here
            //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
            //}

            fx = (f32)(px),
            fy = (f32)y,
            fz = (f32)(15 + pz),
            fmx = (f32)(-1 + px),
            fmy = (f32)my,
            fmz = (f32)(14 + pz);

            goto render;
            end_x0_z15:;
        }


        // X 15 Z 0
        if ((block = c.blocks[15][y][0]).type) {

            fT = c.blocks[15][My][0];
            fB = c.blocks[15][my][0];
            fL = c.blocks[14][y][0];
            fR = ceast.blocks[0][y][0];
            fF = c.blocks[15][y][1];
            fK = csouth.blocks[15][y][15];

            target = &&light_x15_z0;
            endTarget = &&end_x15_z0;
            goto prepare;

            light_x15_z0:

            //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
            A = c.blocks[14][My][1];
            B = csouth.blocks[14][My][15];
            C = csoutheast.blocks[0][My][15];
            D = ceast.blocks[0][My][1];
            E = c.blocks[14][my][1];
            F = csouth.blocks[14][my][15];
            G = csoutheast.blocks[0][my][15];
            H = ceast.blocks[0][my][1];

            AB = c.blocks[14][My][0];
            BC = csouth.blocks[15][My][15];
            CD = ceast.blocks[0][My][0];
            DA = c.blocks[15][My][1];

            EF = c.blocks[14][my][0];
            FG = csouth.blocks[15][my][15];
            GH = ceast.blocks[0][my][0];
            HE = c.blocks[15][my][1];

            AE = c.blocks[14][y][1];
            BF = csouth.blocks[14][y][15];
            CG = csoutheast.blocks[0][y][15];
            DH = ceast.blocks[0][y][1];
            //} else if (isTransparent(type.type)) {
            //    // TODO: try some fun here
            //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
            //}

            fx = (f32)(15 + px),
            fy = (f32)y,
            fz = (f32)(pz),
            fmx = (f32)(14 + px),
            fmy = (f32)my,
            fmz = (f32)(-1 + pz);

            goto render;
            end_x15_z0:;
        }


        // X 15 Z 15
        if ((block = c.blocks[15][y][15]).type) {

            fT = c.blocks[15][My][15];
            fB = c.blocks[15][my][15];
            fL = c.blocks[14][y][15];
            fR = ceast.blocks[0][y][15];
            fF = cnorth.blocks[15][y][0];
            fK = c.blocks[15][y][14];

            target = &&light_x15_z15;
            endTarget = &&end_x15_z15;
            goto prepare;

            light_x15_z15:

            //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
            A = cnorth.blocks[14][My][0];
            B = c.blocks[14][My][14];
            C = ceast.blocks[0][My][14];
            D = cnortheast.blocks[0][My][0];
            E = cnorth.blocks[14][my][0];
            F = c.blocks[14][my][14];
            G = ceast.blocks[0][my][14];
            H = cnortheast.blocks[0][my][0];

            AB = c.blocks[14][My][15];
            BC = c.blocks[15][My][14];
            CD = ceast.blocks[0][My][15];
            DA = cnorth.blocks[15][My][0];

            EF = c.blocks[14][my][15];
            FG = c.blocks[15][my][14];
            GH = ceast.blocks[0][my][15];
            HE = cnorth.blocks[15][my][0];

            AE = cnorth.blocks[14][y][0];
            BF = c.blocks[14][y][14];
            CG = ceast.blocks[0][y][14];
            DH = cnortheast.blocks[0][y][0];
            //} else if (isTransparent(type.type)) {
            //    // TODO: try some fun here
            //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
            //}

            fx = (f32)(15 + px),
            fy = (f32)y,
            fz = (f32)(15 + pz),
            fmx = (f32)(14 + px),
            fmy = (f32)my,
            fmz = (f32)(14 + pz);

            goto render;
            end_x15_z15:;
        }


        // X x Z 0
        for(x = 1; x < 15; x++) {
            if ((block = c.blocks[x][y][0]).type) {
                mx = x - 1;
                Mx = x + 1;

                fT = c.blocks[x][My][0];
                fB = c.blocks[x][my][0];
                fL = c.blocks[mx][y][0];
                fR = c.blocks[Mx][y][0];
                fF = c.blocks[x][y][1];
                fK = csouth.blocks[x][y][15];

                target = &&light_xx_z0;
                endTarget = &&end_xx_z0;
                goto prepare;

                light_xx_z0:

                //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                A = c.blocks[mx][My][1];
                B = csouth.blocks[mx][My][15];
                C = csouth.blocks[Mx][My][15];
                D = c.blocks[Mx][My][1];
                E = c.blocks[mx][my][1];
                F = csouth.blocks[mx][my][15];
                G = csouth.blocks[Mx][my][15];
                H = c.blocks[Mx][my][1];

                AB = c.blocks[mx][My][0];
                BC = csouth.blocks[x][My][15];
                CD = c.blocks[Mx][My][0];
                DA = c.blocks[x][My][1];

                EF = c.blocks[mx][my][0];
                FG = csouth.blocks[x][my][15];
                GH = c.blocks[Mx][my][0];
                HE = c.blocks[x][my][1];

                AE = c.blocks[mx][y][1];
                BF = csouth.blocks[mx][y][15];
                CG = csouth.blocks[Mx][y][15];
                DH = c.blocks[Mx][y][1];
                //} else if (isTransparent(type.type)) {
                //    // TODO: try some fun here
                //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                //}

                fx = (f32)(x + px),
                fy = (f32)y,
                fz = (f32)(pz),
                fmx = (f32)(mx + px),
                fmy = (f32)my,
                fmz = (f32)(-1 + pz);

                goto render;
                end_xx_z0:;
            }
        }


        // X x Z 15
        for(x = 1; x < 15; x++) {
            if ((block = c.blocks[x][y][15]).type) {
                mx = x - 1;
                Mx = x + 1;

                fT = c.blocks[x][My][15];
                fB = c.blocks[x][my][15];
                fL = c.blocks[mx][y][15];
                fR = c.blocks[Mx][y][15];
                fF = cnorth.blocks[x][y][0];
                fK = c.blocks[x][y][14];

                target = &&light_xx_z15;
                endTarget = &&end_xx_z15;
                goto prepare;

                light_xx_z15:

                //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                A = cnorth.blocks[mx][My][0];
                B = c.blocks[mx][My][14];
                C = c.blocks[Mx][My][14];
                D = cnorth.blocks[Mx][My][0];
                E = cnorth.blocks[mx][my][0];
                F = c.blocks[mx][my][14];
                G = c.blocks[Mx][my][14];
                H = cnorth.blocks[Mx][my][0];

                AB = c.blocks[mx][My][15];
                BC = c.blocks[x][My][14];
                CD = c.blocks[Mx][My][15];
                DA = cnorth.blocks[x][My][0];

                EF = c.blocks[mx][my][15];
                FG = c.blocks[x][my][14];
                GH = c.blocks[Mx][my][15];
                HE = cnorth.blocks[x][my][0];

                AE = cnorth.blocks[mx][y][0];
                BF = c.blocks[mx][y][14];
                CG = c.blocks[Mx][y][14];
                DH = cnorth.blocks[Mx][y][0];
                //} else if (isTransparent(type.type)) {
                //    // TODO: try some fun here
                //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                //}

                fx = (f32)(x + px),
                fy = (f32)y,
                fz = (f32)(15 + pz),
                fmx = (f32)(mx + px),
                fmy = (f32)my,
                fmz = (f32)(14 + pz);

                goto render;
                end_xx_z15:;
            }
        }


        // X 0 Z z
        for(z = 1; z < 15; z++) {
            if ((block = c.blocks[0][y][z]).type) {
                mz = z - 1;
                Mz = z + 1;

                fT = c.blocks[0][My][z];
                fB = c.blocks[0][my][z];
                fL = cwest.blocks[15][y][z];
                fR = c.blocks[1][y][z];
                fF = c.blocks[0][y][Mz];
                fK = c.blocks[0][y][mz];

                target = &&light_x0_zz;
                endTarget = &&end_x0_zz;
                goto prepare;

                light_x0_zz:

                //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                A = cwest.blocks[15][My][Mz];
                B = cwest.blocks[15][My][mz];
                C = c.blocks[1][My][mz];
                D = c.blocks[1][My][Mz];
                E = cwest.blocks[15][my][Mz];
                F = cwest.blocks[15][my][mz];
                G = c.blocks[1][my][mz];
                H = c.blocks[1][my][Mz];

                AB = cwest.blocks[15][My][z];
                BC = c.blocks[0][My][mz];
                CD = c.blocks[1][My][z];
                DA = c.blocks[0][My][Mz];

                EF = cwest.blocks[15][my][z];
                FG = c.blocks[0][my][mz];
                GH = c.blocks[1][my][z];
                HE = c.blocks[0][my][Mz];

                AE = cwest.blocks[15][y][Mz];
                BF = cwest.blocks[15][y][mz];
                CG = c.blocks[1][y][mz];
                DH = c.blocks[1][y][Mz];
                //} else if (isTransparent(type.type)) {
                //    // TODO: try some fun here
                //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                //}

                fx = (f32)(px),
                fy = (f32)y,
                fz = (f32)(z + pz),
                fmx = (f32)(-1 + px),
                fmy = (f32)my,
                fmz = (f32)(mz + pz);

                goto render;
                end_x0_zz:;
            }
        }


        // X 15 Z z
        for(z = 1; z < 15; z++) {
            if ((block = c.blocks[15][y][z]).type) {
                mz = z - 1;
                Mz = z + 1;

                fT = c.blocks[15][My][z];
                fB = c.blocks[15][my][z];
                fL = c.blocks[14][y][z];
                fR = ceast.blocks[0][y][z];
                fF = c.blocks[15][y][Mz];
                fK = c.blocks[15][y][mz];

                target = &&light_x15_zz;
                endTarget = &&end_x15_zz;
                goto prepare;

                light_x15_zz:

                //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                A = c.blocks[14][My][Mz];
                B = c.blocks[14][My][mz];
                C = ceast.blocks[0][My][mz];
                D = ceast.blocks[0][My][Mz];
                E = c.blocks[14][my][Mz];
                F = c.blocks[14][my][mz];
                G = ceast.blocks[0][my][mz];
                H = ceast.blocks[0][my][Mz];

                AB = c.blocks[14][My][z];
                BC = c.blocks[15][My][mz];
                CD = ceast.blocks[0][My][z];
                DA = c.blocks[15][My][Mz];

                EF = c.blocks[14][my][z];
                FG = c.blocks[15][my][mz];
                GH = ceast.blocks[0][my][z];
                HE = c.blocks[15][my][Mz];

                AE = c.blocks[14][y][Mz];
                BF = c.blocks[14][y][mz];
                CG = ceast.blocks[0][y][mz];
                DH = ceast.blocks[0][y][Mz];
                //} else if (isTransparent(type.type)) {
                //    // TODO: try some fun here
                //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                //}

                fx = (f32)(15 + px),
                fy = (f32)y,
                fz = (f32)(z + pz),
                fmx = (f32)(14 + px),
                fmy = (f32)my,
                fmz = (f32)(mz + pz);

                goto render;
                end_x15_zz:;
            }
        }


        center:

        // X x Z z
        for(x = 1; x < 15; x++) {
            for(z = 1; z < 15; z++) {
                if ((block = c.blocks[x][y][z]).type) {
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

                    target = &&light_xx_zz;
                    endTarget = &&end_xx_zz;
                    goto prepare;

                    light_xx_zz:

                    //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                    A = c.blocks[mx][My][Mz];
                    B = c.blocks[mx][My][mz];
                    C = c.blocks[Mx][My][mz];
                    D = c.blocks[Mx][My][Mz];
                    E = c.blocks[mx][my][Mz];
                    F = c.blocks[mx][my][mz];
                    G = c.blocks[Mx][my][mz];
                    H = c.blocks[Mx][my][Mz];

                    AB = c.blocks[mx][My][z];
                    BC = c.blocks[x][My][mz];
                    CD = c.blocks[Mx][My][z];
                    DA = c.blocks[x][My][Mz];

                    EF = c.blocks[mx][my][z];
                    FG = c.blocks[x][my][mz];
                    GH = c.blocks[Mx][my][z];
                    HE = c.blocks[x][my][Mz];

                    AE = c.blocks[mx][y][Mz];
                    BF = c.blocks[mx][y][mz];
                    CG = c.blocks[Mx][y][mz];
                    DH = c.blocks[Mx][y][Mz];
                    //} else if (isTransparent(type.type)) {
                    //    // TODO: try some fun here
                    //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                    //}

                    fx = (f32)(x + px),
                    fy = (f32)y,
                    fz = (f32)(z + pz),
                    fmx = (f32)(mx + px),
                    fmy = (f32)my,
                    fmz = (f32)(mz + pz);

                    goto render;
                    end_xx_zz:;
                }
            }
        }
	}
#pragma endregion

	return;

#pragma region prepare

	prepare:
    if (isOpaque(block.type)) {
        tT = !isOpaque(fT.type);
        tB = !isOpaque(fB.type);
        tL = !isOpaque(fL.type);
        tR = !isOpaque(fR.type);
        tF = !isOpaque(fF.type);
        tK = !isOpaque(fK.type);
    }
    else if (isTransparent(block.type)) {
        tT = fT.type != block.type; // !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
        tB = fB.type != block.type; // !isTransparent(fB.type);
        tL = fL.type != block.type; // !isTransparent(fL.type);
        tR = fR.type != block.type; // !isTransparent(fR.type);
        tF = fF.type != block.type; // !isTransparent(fF.type);
        tK = fK.type != block.type; // !isTransparent(fK.type);
    } else if (isSemiTransparent(block.type)) {
        tT = tB = tL = tR = tF = tK = true;
    } else {
        tT = tB = tL = tR = tF = tK = true;
    }

    sz = tT + tB + tL + tR + tF + tK;
    if (sz) {
	    lT = fT.light; // undefined corrupted light if 'fT' is not transparent (context warranty: may never happen on non-culled faces)
	    lB = fB.light;
	    lL = fL.light;
	    lR = fR.light;
	    lF = fF.light;
	    lK = fK.light;

		goto *target;
    }

	goto *endTarget;

#pragma endregion

	render:

	l_tC = initLight, l_tD = initLight, l_tA = initLight, l_tB = initLight;
	l_bE = initLight, l_bH = initLight, l_bG = initLight, l_bF = initLight;
	l_lB = initLight, l_lA = initLight, l_lE = initLight, l_lF = initLight;
	l_rH = initLight, l_rD = initLight, l_rC = initLight, l_rG = initLight;
	l_fA = initLight, l_fD = initLight, l_fH = initLight, l_fE = initLight;
	l_kG = initLight, l_kC = initLight, l_kB = initLight, l_kF = initLight;
	cT.contact = 0, cB.contact = 0, cL.contact = 0, cR.contact = 0, cF.contact = 0, cK.contact = 0;

	u16 xlight;

#define xLight(target) (blockData[target.type].isTransparent ? extendedLight[target.light] : (blockData[target.type].emittedLight ? extendedLight[blockData[target.type].emittedLight] : 0))
	
	// Contact face lights
	xlight = xLight(fT);
	if (xlight) {
		l_tC += xlight;
		l_tA += xlight;
		l_tB += xlight;
		l_tD += xlight;
		
		cT.contact = LightContactIndex::C | LightContactIndex::A | LightContactIndex::B | LightContactIndex::D;
	}
	
	xlight = xLight(fB);
	if (xlight) {
		l_bE += xlight;
		l_bH += xlight;
		l_bG += xlight;
		l_bF += xlight;
		
		cB.contact = LightContactIndex::E | LightContactIndex::H | LightContactIndex::G | LightContactIndex::F;
	}
	
	xlight = xLight(fL);
	if (xlight) {
		l_lB += xlight;
		l_lA += xlight;
		l_lE += xlight;
		l_lF += xlight;
		
		cL.contact = LightContactIndex::B | LightContactIndex::A | LightContactIndex::E | LightContactIndex::F;
	}

	xlight = xLight(fR);
	if (xlight) {
		l_rH += xlight;
		l_rD += xlight;
		l_rC += xlight;
		l_rG += xlight;
		
		cR.contact = LightContactIndex::H | LightContactIndex::D | LightContactIndex::C | LightContactIndex::G;
	}

	xlight = xLight(fF);
	if (xlight) {
		l_fA += xlight;
		l_fD += xlight;
		l_fH += xlight;
		l_fE += xlight;
		
		cF.contact = LightContactIndex::A | LightContactIndex::D | LightContactIndex::H | LightContactIndex::E;
	}

	xlight = xLight(fK);
	if (xlight) {
		l_kG += xlight;
		l_kC += xlight;
		l_kB += xlight;
		l_kF += xlight;
		
		cK.contact = LightContactIndex::G | LightContactIndex::C | LightContactIndex::B | LightContactIndex::F;
	}
	
	// Contact line lights
	xlight = xLight(AB);
	if (xlight) {
		l_tA += xlight;
		l_lA += xlight;
		l_tB += xlight;
		l_lB += xlight;
		
		cT.contact += LightContactIndex::A | LightContactIndex::B;
		cL.contact += LightContactIndex::A | LightContactIndex::B;
	}

	xlight = xLight(BC);
	if (xlight) {
		l_tC += xlight;
		l_kC += xlight;
		l_tB += xlight;
		l_kB += xlight;
		
		cT.contact += LightContactIndex::C | LightContactIndex::B;
		cK.contact += LightContactIndex::C | LightContactIndex::B;
	}

	xlight = xLight(CD);
	if (xlight) {
		l_tD += xlight;
		l_rD += xlight;
		l_tC += xlight;
		l_rC += xlight;
		
		cT.contact += LightContactIndex::D | LightContactIndex::C;
		cR.contact += LightContactIndex::D | LightContactIndex::C;
	}

	xlight = xLight(DA);
	if (xlight) {
		l_tA += xlight;
		l_fA += xlight;
		l_tD += xlight;
		l_fD += xlight;
		
		cT.contact += LightContactIndex::A | LightContactIndex::D;
		cF.contact += LightContactIndex::A | LightContactIndex::D;
	}

	xlight = xLight(EF);
	if (xlight) {
		l_bE += xlight;
		l_lE += xlight;
		l_bF += xlight;
		l_lF += xlight;
		
		cB.contact += LightContactIndex::E | LightContactIndex::F;
		cL.contact += LightContactIndex::E | LightContactIndex::F;
	}

	xlight = xLight(FG);
	if (xlight) {
		l_bF += xlight;
		l_kF += xlight;
		l_bG += xlight;
		l_kG += xlight;
		
		cB.contact += LightContactIndex::F | LightContactIndex::G;
		cK.contact += LightContactIndex::F | LightContactIndex::G;
	}

	xlight = xLight(GH);
	if (xlight) {
		l_bG += xlight;
		l_rG += xlight;
		l_bH += xlight;
		l_rH += xlight;
		
		cB.contact += LightContactIndex::G | LightContactIndex::H;
		cR.contact += LightContactIndex::G | LightContactIndex::H;
	}
	
	xlight = xLight(HE);
	if (xlight) {
		l_bH += xlight;
		l_fH += xlight;
		l_bE += xlight;
		l_fE += xlight;
		
		cB.contact += LightContactIndex::H | LightContactIndex::E;
		cF.contact += LightContactIndex::H | LightContactIndex::E;
	}
	
	xlight = xLight(AE);
	if (xlight) {
		l_lA += xlight;
		l_fA += xlight;
		l_lE += xlight;
		l_fE += xlight;
		
		cL.contact += LightContactIndex::A | LightContactIndex::E;
		cF.contact += LightContactIndex::A | LightContactIndex::E;
	}

	xlight = xLight(BF);
	if (xlight) {
		l_lB += xlight;
		l_kB += xlight;
		l_lF += xlight;
		l_kF += xlight;
		
		cL.contact += LightContactIndex::B | LightContactIndex::F;
		cK.contact += LightContactIndex::B | LightContactIndex::F;
	}

	xlight = xLight(CG);
	if (xlight) {
		l_rC += xlight;
		l_kC += xlight;
		l_rG += xlight;
		l_kG += xlight;
		
		cR.contact += LightContactIndex::C | LightContactIndex::G;
		cK.contact += LightContactIndex::C | LightContactIndex::G;
	}

	xlight = xLight(DH);
	if (xlight) {
		l_rD += xlight;
		l_fD += xlight;
		l_rH += xlight;
		l_fH += xlight;
		
		cR.contact += LightContactIndex::D | LightContactIndex::H;
		cF.contact += LightContactIndex::D | LightContactIndex::H;
	}
	
	// Contact point lights
	xlight = xLight(A);
	if (xlight) {
		if (cT.A & 2) l_tA += xlight;
		if (cL.A & 2) l_lA += xlight;
		if (cF.A & 2) l_fA += xlight;
		
		cT.contact += LightContactIndex::A;
		cL.contact += LightContactIndex::A;
		cF.contact += LightContactIndex::A;
	}
	
	xlight = xLight(B);
	if (xlight) {
		if (cT.B & 2) l_tB += xlight;
		if (cL.B & 2) l_lB += xlight;
		if (cK.B & 2) l_kB += xlight;
		
		cT.contact += LightContactIndex::B;
		cL.contact += LightContactIndex::B;
		cK.contact += LightContactIndex::B;
	}
	
	xlight = xLight(C);
	if (xlight) {
		if (cT.C & 2) l_tC += xlight;
		if (cR.C & 2) l_rC += xlight;
		if (cK.C & 2) l_kC += xlight;
		
		cT.contact += LightContactIndex::C;
		cR.contact += LightContactIndex::C;
		cK.contact += LightContactIndex::C;
	}

	xlight = xLight(D);
	if (xlight) {
		if (cT.D & 2) l_tD += xlight;
		if (cR.D & 2) l_rD += xlight;
		if (cF.D & 2) l_fD += xlight;
		
		cT.contact += LightContactIndex::D;
		cR.contact += LightContactIndex::D;
		cF.contact += LightContactIndex::D;
	}

	xlight = xLight(E);
	if (xlight) {
		if (cB.E & 2) l_bE += xlight;
		if (cL.E & 2) l_lE += xlight;
		if (cF.E & 2) l_fE += xlight;
		
		cB.contact += LightContactIndex::E;
		cL.contact += LightContactIndex::E;
		cF.contact += LightContactIndex::E;
	}

	xlight = xLight(F);
	if (xlight) {
		if (cB.F & 2) l_bF += xlight;
		if (cL.F & 2) l_lF += xlight;
		if (cK.F & 2) l_kF += xlight;
		
		cB.contact += LightContactIndex::F;
		cL.contact += LightContactIndex::F;
		cK.contact += LightContactIndex::F;
	}

	xlight = xLight(G);
	if (xlight) {
		if (cB.G & 2) l_bG += xlight;
		if (cR.G & 2) l_rG += xlight;
		if (cK.G & 2) l_kG += xlight;
		
		cB.contact += LightContactIndex::G;
		cR.contact += LightContactIndex::G;
		cK.contact += LightContactIndex::G;
	}

	xlight = xLight(H);
	if (xlight) {
		if (cB.H & 2) l_bH += xlight;
		if (cR.H & 2) l_rH += xlight;
		if (cF.H & 2) l_fH += xlight;
		
		cB.contact += LightContactIndex::H;
		cR.contact += LightContactIndex::H;
		cF.contact += LightContactIndex::H;
	}
	
	// Ambient Occlusion Correction
	if (cT.C <= 1) l_tC += xLight(fT);
	if (cT.D <= 1) l_tD += xLight(fT);
	if (cT.A <= 1) l_tA += xLight(fT);
	if (cT.B <= 1) l_tB += xLight(fT);
	
	if (cB.E <= 1) l_bE += xLight(fB);
	if (cB.H <= 1) l_bH += xLight(fB);
	if (cB.G <= 1) l_bG += xLight(fB);
	if (cB.F <= 1) l_bF += xLight(fB);
	
	if (cL.B <= 1) l_lB += xLight(fL);
	if (cL.A <= 1) l_lA += xLight(fL);
	if (cL.E <= 1) l_lE += xLight(fL);
	if (cL.F <= 1) l_lF += xLight(fL);
	
	if (cR.H <= 1) l_rH += xLight(fR);
	if (cR.D <= 1) l_rD += xLight(fR);
	if (cR.C <= 1) l_rC += xLight(fR);
	if (cR.G <= 1) l_rG += xLight(fR);
	
	if (cF.A <= 1) l_fA += xLight(fF);
	if (cF.D <= 1) l_fD += xLight(fF);
	if (cF.H <= 1) l_fH += xLight(fF);
	if (cF.E <= 1) l_fE += xLight(fF);
	
	if (cK.G <= 1) l_kG += xLight(fK);
	if (cK.C <= 1) l_kC += xLight(fK);
	if (cK.B <= 1) l_kB += xLight(fK);
	if (cK.F <= 1) l_kF += xLight(fK);
	
	if (isRegular(block.type)) {
		//GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
	    if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, block.type, l_tC, l_tD, l_tA, l_tB, isTransparent(block.type)); // CDAB
	    if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, block.type, l_bE, l_bH, l_bG, l_bF, isTransparent(block.type)); // EHGF
	    if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, block.type, l_lB, l_lA, l_lE, l_lF, isTransparent(block.type)); // BAEF
	    if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, block.type, l_rH, l_rD, l_rC, l_rG, isTransparent(block.type)); // HDCG
	    if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, block.type, l_fA, l_fD, l_fH, l_fE, isTransparent(block.type)); // ADHE
	    if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, block.type, l_kG, l_kC, l_kB, l_kF, isTransparent(block.type)); // GCBF
	    //GX_End();
	} else {

		const void * renderers[] = {&&render_Furnace, &&render_Door, &&render_Void};

		goto *renderers[block.type - BlockType::IRREGULAR];

		render_Furnace:
		{
			auto & data = blockData[block.type];
			if (tT) {
				u16 tc = data.tc[BlockFace::Top];
				renderRawTop   (fx, fy, fz, fmx, fmy, fmz, tc + 17, tc + 18, tc + 1, tc, l_tC, l_tD, l_tA, l_tB, isTransparent(block.type)); // CDAB
			}
			if (tB) {
				u16 tc = data.tc[BlockFace::Bottom];
				renderRawBottom(fx, fy, fz, fmx, fmy, fmz, tc + 18, tc + 1, tc, tc + 17, l_bE, l_bH, l_bG, l_bF,  isTransparent(block.type)); // EHGF
			}
			if (tL) {
				u16 tc = data.tc[(BlockFace::West + block.orient) & 0b11];
				renderRawWest  (fx, fy, fz, fmx, fmy, fmz, tc, tc + 17, tc + 18, tc + 1, l_lB, l_lA, l_lE, l_lF, isTransparent(block.type)); // BAEF
			}
			if (tR) {
				u16 tc = data.tc[(BlockFace::East + block.orient) & 0b11];
				renderRawEast  (fx, fy, fz, fmx, fmy, fmz, tc + 1, tc, tc + 17, tc + 18, l_rH, l_rD, l_rC, l_rG, isTransparent(block.type)); // HDCG
			}
			if (tF) {
				u16 tc = data.tc[(BlockFace::North + block.orient) & 0b11];
				renderRawNorth (fx, fy, fz, fmx, fmy, fmz, tc, tc + 17, tc + 18, tc + 1, l_fA, l_fD, l_fH, l_fE, isTransparent(block.type)); // ADHE
			}
			if (tK) {
				u16 tc = data.tc[(BlockFace::South + block.orient) & 0b11];
				renderRawSouth (fx, fy, fz, fmx, fmy, fmz, tc + 1, tc, tc + 17, tc + 18, l_kG, l_kC, l_kB, l_kF,  isTransparent(block.type)); // GCBF
			}
			goto *endTarget;
		}

		render_Door:
		{
			fy += 1.f;
			switch ((Direction)((block.orient + (block.state & 1)) & 0b11)) {
				case NORTH:
					fmz = fz - 3.f/16.f;
					break;
				case EAST:
					fmx = fx - 3.f/16.f;
					break;
				case SOUTH:
					fz = fmz + 3.f/16.f;
					break;
				case WEST:
					fx = fmx + 3.f/16.f;
					break;
			}

			auto & data = blockData[block.type];
			if (tT) {
				u16 tc = data.tc[BlockFace::Top];
				renderRawTop   (fx, fy, fz, fmx, fmy, fmz, tc + 17, tc + 18, tc + 1, tc, l_tC, l_tD, l_tA, l_tB, isTransparent(block.type)); // CDAB
			}
			if (tB) {
				u16 tc = data.tc[BlockFace::Bottom];
				renderRawBottom(fx, fy, fz, fmx, fmy, fmz, tc + 18, tc + 1, tc, tc + 17, l_bE, l_bH, l_bG, l_bF, isTransparent(block.type)); // EHGF
			}
			if (tL) {
				u16 tc = data.tc[(BlockFace::West + block.orient) & 0b11];
				renderRawWest  (fx, fy, fz, fmx, fmy, fmz, tc, tc + 17, tc + 18, tc + 1, l_lB, l_lA, l_lE, l_lF, isTransparent(block.type)); // BAEF
			}
			if (tR) {
				u16 tc = data.tc[(BlockFace::East + block.orient) & 0b11];
				renderRawEast  (fx, fy, fz, fmx, fmy, fmz, tc + 1, tc, tc + 17, tc + 18, l_rH, l_rD, l_rC, l_rG,isTransparent(block.type)); // HDCG
			}
			if (tF) {
				u16 tc = data.tc[(BlockFace::North + block.orient) & 0b11];
				renderRawNorth (fx, fy, fz, fmx, fmy, fmz, tc, tc + 17, tc + 18, tc + 1, l_fA, l_fD, l_fH, l_fE, isTransparent(block.type)); // ADHE
			}
			if (tK) {
				u16 tc = data.tc[(BlockFace::South + block.orient) & 0b11];
				renderRawSouth (fx, fy, fz, fmx, fmy, fmz, tc + 1, tc, tc + 17, tc + 18, l_kG, l_kC, l_kB, l_kF,isTransparent(block.type)); // GCBF
			}
			goto *endTarget;
		}

	}


	render_Void:
	goto *endTarget;
}
