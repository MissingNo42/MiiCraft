//
// Created by Romain on 15/02/2024.
//

#include "engine/render/block.h"
#include "engine/render/cache.h"
#include "engine/render/tables.h"
#include "world/Chunk.h"
#include "world/coord.h"


/**
 * @brief Count the number of 'solid' blocks around a particular block vertex
 * @note just increment contact by OR'ed LightContactIndex values. (multi inc in only 1 op)
 * */
union LightContact {
	u8 contact; // write
	struct {
		u8 A: 2, B: 2, C: 2, D: 2; // read
	};
	struct {
		u8 G: 2, H: 2, E: 2, F: 2; // read
	};
};

union LightContacts {
	u64 contacts; // write
	struct {
		[[maybe_unused]] u16 _unused; // padding
		LightContact T, B, L, R, F, K; // 48 bits, read
	};
};

enum LightContactIndex : u64 {
	Vertex_tA = 0b010000000000000000000000000000000000000000000000,
	Vertex_tB = 0b000100000000000000000000000000000000000000000000,
	Vertex_tC = 0b000001000000000000000000000000000000000000000000,
	Vertex_tD = 0b000000010000000000000000000000000000000000000000,

	Vertex_bG = 0b000000000100000000000000000000000000000000000000,
	Vertex_bH = 0b000000000001000000000000000000000000000000000000,
	Vertex_bE = 0b000000000000010000000000000000000000000000000000,
	Vertex_bF = 0b000000000000000100000000000000000000000000000000,

	Vertex_lA = 0b000000000000000001000000000000000000000000000000,
	Vertex_lB = 0b000000000000000000010000000000000000000000000000,
	Vertex_lE = 0b000000000000000000000100000000000000000000000000,
	Vertex_lF = 0b000000000000000000000001000000000000000000000000,

	Vertex_rG = 0b000000000000000000000000010000000000000000000000,
	Vertex_rH = 0b000000000000000000000000000100000000000000000000,
	Vertex_rC = 0b000000000000000000000000000001000000000000000000,
	Vertex_rD = 0b000000000000000000000000000000010000000000000000,

	Vertex_fA = 0b000000000000000000000000000000000100000000000000,
	Vertex_fH = 0b000000000000000000000000000000000001000000000000,
	Vertex_fE = 0b000000000000000000000000000000000000010000000000,
	Vertex_fD = 0b000000000000000000000000000000000000000100000000,

	Vertex_kG = 0b000000000000000000000000000000000000000001000000,
	Vertex_kB = 0b000000000000000000000000000000000000000000010000,
	Vertex_kC = 0b000000000000000000000000000000000000000000000100,
	Vertex_kF = 0b000000000000000000000000000000000000000000000001,

	Vertex_A = Vertex_tA | Vertex_lA | Vertex_fA,
	Vertex_B = Vertex_tB | Vertex_lB | Vertex_kB,
	Vertex_C = Vertex_tC | Vertex_rC | Vertex_kC,
	Vertex_D = Vertex_tD | Vertex_rD | Vertex_fD,
	Vertex_G = Vertex_bG | Vertex_rG | Vertex_kG,
	Vertex_H = Vertex_bH | Vertex_rH | Vertex_fH,
	Vertex_E = Vertex_bE | Vertex_lE | Vertex_fE,
	Vertex_F = Vertex_bF | Vertex_lF | Vertex_kF,
};


static constexpr u16 initLight = 0b1111111110111111; // ~0000_01_0000_01 | 1


inline void renderVertex(f32 x, f32 y, f32 z, u16 color, u16 tc, RenderChannel channel) {
	//GX_Position3f32(x, y, z);
	//GX_Normal1x8(normal);
	//GX_Color1u32(color);
	//GX_TexCoord2f32(tx, ty);
	ChunkCache::addVertex(Vertex(x, y, z, color, tc), channel);
}

inline void
renderRawNorth(f32 x, f32 y, f32 z, f32 mx, f32 my, f32, u16 tcA, u16 tcD, u16 tcH, u16 tcE, u16 c1, u16 c2, u16 c3,
               u16 c4, RenderChannel channel) {
	renderVertex(mx, y, z, c1, tcA, channel); // A
	renderVertex(x, y, z, c2, tcD, channel); // D
	renderVertex(x, my, z, c3, tcH, channel); // H
	renderVertex(mx, my, z, c4, tcE, channel); // E
}

inline void
renderRawSouth(f32 x, f32 y, f32, f32 mx, f32 my, f32 mz, u16 tcG, u16 tcC, u16 tcB, u16 tcF, u16 c1, u16 c2, u16 c3,
               u16 c4, RenderChannel channel) {
	renderVertex(x, my, mz, c1, tcG, channel); // G
	renderVertex(x, y, mz, c2, tcC, channel); // C
	renderVertex(mx, y, mz, c3, tcB, channel); // B
	renderVertex(mx, my, mz, c4, tcF, channel); // F
}

inline void
renderRawTop(f32 x, f32 y, f32 z, f32 mx, f32, f32 mz, u16 tcC, u16 tcD, u16 tcA, u16 tcB, u16 c1, u16 c2, u16 c3,
             u16 c4, RenderChannel channel) {
	renderVertex(x, y, mz, c1, tcC, channel); // C
	renderVertex(x, y, z, c2, tcD, channel); // D
	renderVertex(mx, y, z, c3, tcA, channel); // A
	renderVertex(mx, y, mz, c4, tcB, channel); // B
}

inline void
renderRawBottom(f32 x, f32, f32 z, f32 mx, f32 my, f32 mz, u16 tcE, u16 tcH, u16 tcG, u16 tcF, u16 c1, u16 c2, u16 c3,
                u16 c4, RenderChannel channel) {
	renderVertex(mx, my, z, c1, tcE, channel); // E
	renderVertex(x, my, z, c2, tcH, channel); // H
	renderVertex(x, my, mz, c3, tcG, channel); // G
	renderVertex(mx, my, mz, c4, tcF, channel); // F
}

inline void
renderRawWest(f32, f32 y, f32 z, f32 mx, f32 my, f32 mz, u16 tcB, u16 tcA, u16 tcE, u16 tcF, u16 c1, u16 c2, u16 c3,
              u16 c4, RenderChannel channel) {
	renderVertex(mx, y, mz, c1, tcB, channel); // B
	renderVertex(mx, y, z, c2, tcA, channel); // A
	renderVertex(mx, my, z, c3, tcE, channel); // E
	renderVertex(mx, my, mz, c4, tcF, channel); // F
}

inline void
renderRawEast(f32 x, f32 y, f32 z, f32, f32 my, f32 mz, u16 tcH, u16 tcD, u16 tcC, u16 tcG, u16 c1, u16 c2, u16 c3,
              u16 c4, RenderChannel channel) {
	renderVertex(x, my, z, c1, tcH, channel); // H
	renderVertex(x, y, z, c2, tcD, channel); // D
	renderVertex(x, y, mz, c3, tcC, channel); // C
	renderVertex(x, my, mz, c4, tcG, channel); // G
}

inline void
renderFront(f32 x, f32 y, f32 z, f32 mx, f32 my, f32, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, RenderChannel channel, bool reverse) {
	u16 tc = blockData[type].tc[BlockFace::North];
	if (reverse) {
		renderVertex(x, y, z, c2, tc + 17, channel); // D
		renderVertex(x, my, z, c3, tc + 18, channel); // H
		renderVertex(mx, my, z, c4, tc + 1, channel); // E
		renderVertex(mx, y, z, c1, tc, channel); // A
	} else {
		renderVertex(mx, y, z, c1, tc, channel); // A
		renderVertex(x, y, z, c2, tc + 17, channel); // D
		renderVertex(x, my, z, c3, tc + 18, channel); // H
		renderVertex(mx, my, z, c4, tc + 1, channel); // E
	}
}

inline void
renderBack(f32 x, f32 y, f32, f32 mx, f32 my, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, RenderChannel channel, bool reverse) {
	u16 tc = blockData[type].tc[BlockFace::South];
	if (reverse) {
		renderVertex(x, y, mz, c2, tc, channel); // C
		renderVertex(mx, y, mz, c3, tc + 17, channel); // B
		renderVertex(mx, my, mz, c4, tc + 18, channel); // F
		renderVertex(x, my, mz, c1, tc + 1, channel); // G
	} else {
		renderVertex(x, my, mz, c1, tc + 1, channel); // G
		renderVertex(x, y, mz, c2, tc, channel); // C
		renderVertex(mx, y, mz, c3, tc + 17, channel); // B
		renderVertex(mx, my, mz, c4, tc + 18, channel); // F
	}
}

inline void
renderTop(f32 x, f32 y, f32 z, f32 mx, f32, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, RenderChannel channel, bool reverse) {
	u16 tc = blockData[type].tc[BlockFace::Top];
	if (reverse) {
		renderVertex(x, y, z, c2, tc + 18, channel); // D
		renderVertex(mx, y, z, c3, tc + 1, channel); // A
		renderVertex(mx, y, mz, c4, tc, channel); // B
		renderVertex(x, y, mz, c1, tc + 17, channel); // C
	} else {
		renderVertex(x, y, mz, c1, tc + 17, channel); // C
		renderVertex(x, y, z, c2, tc + 18, channel); // D
		renderVertex(mx, y, z, c3, tc + 1, channel); // A
		renderVertex(mx, y, mz, c4, tc, channel); // B
	}
}

inline void
renderBottom(f32 x, f32, f32 z, f32 mx, f32 my, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, RenderChannel channel, bool reverse) {
	u16 tc = blockData[type].tc[BlockFace::Bottom];
	if (reverse) {
		renderVertex(x, my, z, c2, tc + 1, channel); // H
		renderVertex(x, my, mz, c3, tc, channel); // G
		renderVertex(mx, my, mz, c4, tc + 17, channel); // F
		renderVertex(mx, my, z, c1, tc + 18, channel); // E
	} else {
		renderVertex(mx, my, z, c1, tc + 18, channel); // E
		renderVertex(x, my, z, c2, tc + 1, channel); // H
		renderVertex(x, my, mz, c3, tc, channel); // G
		renderVertex(mx, my, mz, c4, tc + 17, channel); // F
	}
}

inline void
renderLeft(f32, f32 y, f32 z, f32 mx, f32 my, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, RenderChannel channel, bool reverse) {
	u16 tc = blockData[type].tc[BlockFace::West];
	if (reverse) {
		renderVertex(mx, y, z, c2, tc + 17, channel); // A
		renderVertex(mx, my, z, c3, tc + 18, channel); // E
		renderVertex(mx, my, mz, c4, tc + 1, channel); // F
		renderVertex(mx, y, mz, c1, tc, channel); // B
	} else {
		renderVertex(mx, y, mz, c1, tc, channel); // B
		renderVertex(mx, y, z, c2, tc + 17, channel); // A
		renderVertex(mx, my, z, c3, tc + 18, channel); // E
		renderVertex(mx, my, mz, c4, tc + 1, channel); // F
	}
}

inline void
renderRight(f32 x, f32 y, f32 z, f32, f32 my, f32 mz, BlockType type, u16 c1, u16 c2, u16 c3, u16 c4, RenderChannel channel, bool reverse) {
	const u16 tc = blockData[type].tc[BlockFace::East];
	if (reverse) {
		renderVertex(x, y, z, c2, tc, channel); // D
		renderVertex(x, y, mz, c3, tc + 17, channel); // C
		renderVertex(x, my, mz, c4, tc + 18, channel); // G
		renderVertex(x, my, z, c1, tc + 1, channel); // H
	} else {
		renderVertex(x, my, z, c1, tc + 1, channel); // H
		renderVertex(x, y, z, c2, tc, channel); // D
		renderVertex(x, y, mz, c3, tc + 17, channel); // C
		renderVertex(x, my, mz, c4, tc + 18, channel); // G
	}
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-label-as-value" // usage for computed goto for optimization purpose
/**
 *     B ------ C
	  /       / |
	 /       /  |
	A ------ D  G
	|   F   |  /
	|       | /
	E ------ H
 * */
void Renderer::renderChunk(Chunk &c) {
	s32 px = c.coord.x << 4;
	s32 pz = c.coord.z << 4;
	s32 x, y, z, sz; // block coord and number of rendered faces (for regular)
	s32 mx, my, mz, Mx, My, Mz; // alternate block coord
	void * target = nullptr, * endTarget = nullptr; // renderer sub-functions jumpers

	Block A, B, C, D, E, F, G, H; // render cubes with X vertex in common
	Block AB, BC, CD, DA,  // render cubes with X, Y vertices in common
	EF, FG, GH, HE,
			AE, BF, CG, DH;
	Block fT, fB, fL, fR, fF, fK, block; // block type of the 6 adjacent blocks + the current one
	bool tT, tB, tL, tR, tF, tK; // true if the current block's faces must be rendered
	f32 fx, fy, fz, fmx, fmy, fmz; // float block coord

	u16 l_tC = 0, l_tD = 0, l_tA = 0, l_tB = 0;
	u16 l_bE = 0, l_bH = 0, l_bG = 0, l_bF = 0;
	u16 l_lB = 0, l_lA = 0, l_lE = 0, l_lF = 0;
	u16 l_rH = 0, l_rD = 0, l_rC = 0, l_rG = 0;
	u16 l_fA = 0, l_fD = 0, l_fH = 0, l_fE = 0;
	u16 l_kG = 0, l_kC = 0, l_kB = 0, l_kF = 0;

	const void * renderers[] = {&&render_Furnace, &&render_Door, &&render_Void};

	LightContacts lc; // light contacts of the 6 faces
	LightContacts le; // light contacts of the 6 faces for emitted neighboors

	Chunk &cnorth = c.getNeighboorChunk(NORTH);
	Chunk &csouth = c.getNeighboorChunk(SOUTH);
	Chunk &cwest = c.getNeighboorChunk(WEST);
	Chunk &ceast = c.getNeighboorChunk(EAST);

	Chunk &cnortheast = c.getDiagonalNeighboorChunk(NORTH, EAST);
	Chunk &cnorthwest = c.getDiagonalNeighboorChunk(NORTH, WEST);
	Chunk &csoutheast = c.getDiagonalNeighboorChunk(SOUTH, EAST);
	Chunk &csouthwest = c.getDiagonalNeighboorChunk(SOUTH, WEST);

	for (y = 1; y < CHUNK_LIMIT; y++) { // for each vertical levels (except 1st and last)

		my = y ? y - 1: 0; // allow the 1st level to be rendered by adding a fake -1 level
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
		if ((block = c.blocks[y][0][0]).type) {

			fT = c.blocks[My][0][0];
			fB = c.blocks[my][0][0];
			fL = cwest.blocks[y][15][0];
			fR = c.blocks[y][1][0];
			fF = c.blocks[y][0][1];
			fK = csouth.blocks[y][0][15];

			target = &&light_x0_z0;
			endTarget = &&end_x0_z0;
			goto prepare;

			light_x0_z0:

			//if (isOpaque(type.type) || isSemiTransparent(type.type)) {
			A = cwest.blocks[My][15][1];
			B = csouthwest.blocks[My][15][15];
			C = csouth.blocks[My][1][15];
			D = c.blocks[My][1][1];
			E = cwest.blocks[my][15][1];
			F = csouthwest.blocks[my][15][15];
			G = csouth.blocks[my][1][15];
			H = c.blocks[my][1][1];

			AB = cwest.blocks[My][15][0];
			BC = csouth.blocks[My][0][15];
			CD = c.blocks[My][1][0];
			DA = c.blocks[My][0][1];

			EF = cwest.blocks[my][15][0];
			FG = csouth.blocks[my][0][15];
			GH = c.blocks[my][1][0];
			HE = c.blocks[my][0][1];

			AE = cwest.blocks[y][15][1];
			BF = csouthwest.blocks[y][15][15];
			CG = csouth.blocks[y][1][15];
			DH = c.blocks[y][1][1];
			//} else if (isTransparent(type.type)) {
			//    // TODO: try some fun here
			//    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
			//}

			fx = static_cast<f32>(px),
			fy = static_cast<f32>(y),
			fz = static_cast<f32>(pz),
			fmx = static_cast<f32>(-1 + px),
			fmy = static_cast<f32>(my),
			fmz = static_cast<f32>(-1 + pz);

			goto render;
			end_x0_z0:;
		}


		// X 0 Z 15
		if ((block = c.blocks[y][0][15]).type) {

			fT = c.blocks[My][0][15];
			fB = c.blocks[my][0][15];
			fL = cwest.blocks[y][15][15];
			fR = c.blocks[y][1][15];
			fF = cnorth.blocks[y][0][0];
			fK = c.blocks[y][0][14];

			target = &&light_x0_z15;
			endTarget = &&end_x0_z15;
			goto prepare;

			light_x0_z15:

			//if (isOpaque(type.type) || isSemiTransparent(type.type)) {
			A = cnorthwest.blocks[My][15][0];
			B = cwest.blocks[My][15][14];
			C = c.blocks[My][1][14];
			D = cnorth.blocks[My][1][0];
			E = cnorthwest.blocks[my][15][0];
			F = cwest.blocks[my][15][14];
			G = c.blocks[my][1][14];
			H = cnorth.blocks[my][1][0];

			AB = cwest.blocks[My][15][15];
			BC = c.blocks[My][0][14];
			CD = c.blocks[My][1][15];
			DA = cnorth.blocks[My][0][0];

			EF = cwest.blocks[my][15][15];
			FG = c.blocks[my][0][14];
			GH = c.blocks[my][1][15];
			HE = cnorth.blocks[my][0][0];

			AE = cnorthwest.blocks[y][15][0];
			BF = cwest.blocks[y][15][14];
			CG = c.blocks[y][1][14];
			DH = cnorth.blocks[y][1][0];
			//} else if (isTransparent(type.type)) {
			//    // TODO: try some fun here
			//    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
			//}

			fx = static_cast<f32>(px),
			fy = static_cast<f32>(y),
			fz = static_cast<f32>(15 + pz),
			fmx = static_cast<f32>(-1 + px),
			fmy = static_cast<f32>(my),
			fmz = static_cast<f32>(14 + pz);

			goto render;
			end_x0_z15:;
		}


		// X 15 Z 0
		if ((block = c.blocks[y][15][0]).type) {

			fT = c.blocks[My][15][0];
			fB = c.blocks[my][15][0];
			fL = c.blocks[y][14][0];
			fR = ceast.blocks[y][0][0];
			fF = c.blocks[y][15][1];
			fK = csouth.blocks[y][15][15];

			target = &&light_x15_z0;
			endTarget = &&end_x15_z0;
			goto prepare;

			light_x15_z0:

			//if (isOpaque(type.type) || isSemiTransparent(type.type)) {
			A = c.blocks[My][14][1];
			B = csouth.blocks[My][14][15];
			C = csoutheast.blocks[My][0][15];
			D = ceast.blocks[My][0][1];
			E = c.blocks[my][14][1];
			F = csouth.blocks[my][14][15];
			G = csoutheast.blocks[my][0][15];
			H = ceast.blocks[my][0][1];

			AB = c.blocks[My][14][0];
			BC = csouth.blocks[My][15][15];
			CD = ceast.blocks[My][0][0];
			DA = c.blocks[My][15][1];

			EF = c.blocks[my][14][0];
			FG = csouth.blocks[my][15][15];
			GH = ceast.blocks[my][0][0];
			HE = c.blocks[my][15][1];

			AE = c.blocks[y][14][1];
			BF = csouth.blocks[y][14][15];
			CG = csoutheast.blocks[y][0][15];
			DH = ceast.blocks[y][0][1];
			//} else if (isTransparent(type.type)) {
			//    // TODO: try some fun here
			//    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
			//}

			fx = static_cast<f32>(15 + px),
			fy = static_cast<f32>(y),
			fz = static_cast<f32>(pz),
			fmx = static_cast<f32>(14 + px),
			fmy = static_cast<f32>(my),
			fmz = static_cast<f32>(-1 + pz);

			goto render;
			end_x15_z0:;
		}


		// X 15 Z 15
		if ((block = c.blocks[y][15][15]).type) {

			fT = c.blocks[My][15][15];
			fB = c.blocks[my][15][15];
			fL = c.blocks[y][14][15];
			fR = ceast.blocks[y][0][15];
			fF = cnorth.blocks[y][15][0];
			fK = c.blocks[y][15][14];

			target = &&light_x15_z15;
			endTarget = &&end_x15_z15;
			goto prepare;

			light_x15_z15:

			//if (isOpaque(type.type) || isSemiTransparent(type.type)) {
			A = cnorth.blocks[My][14][0];
			B = c.blocks[My][14][14];
			C = ceast.blocks[My][0][14];
			D = cnortheast.blocks[My][0][0];
			E = cnorth.blocks[my][14][0];
			F = c.blocks[my][14][14];
			G = ceast.blocks[my][0][14];
			H = cnortheast.blocks[my][0][0];

			AB = c.blocks[My][14][15];
			BC = c.blocks[My][15][14];
			CD = ceast.blocks[My][0][15];
			DA = cnorth.blocks[My][15][0];

			EF = c.blocks[my][14][15];
			FG = c.blocks[my][15][14];
			GH = ceast.blocks[my][0][15];
			HE = cnorth.blocks[my][15][0];

			AE = cnorth.blocks[y][14][0];
			BF = c.blocks[y][14][14];
			CG = ceast.blocks[y][0][14];
			DH = cnortheast.blocks[y][0][0];
			//} else if (isTransparent(type.type)) {
			//    // TODO: try some fun here
			//    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
			//}

			fx = static_cast<f32>(15 + px),
			fy = static_cast<f32>(y),
			fz = static_cast<f32>(15 + pz),
			fmx = static_cast<f32>(14 + px),
			fmy = static_cast<f32>(my),
			fmz = static_cast<f32>(14 + pz);

			goto render;
			end_x15_z15:;
		}


        // X x Z 0
        for(x = 1; x < 15; x++) {
            if ((block = c.blocks[y][x][0]).type) {
                mx = x - 1;
                Mx = x + 1;

                fT = c.blocks[My][x][0];
                fB = c.blocks[my][x][0];
                fL = c.blocks[y][mx][0];
                fR = c.blocks[y][Mx][0];
                fF = c.blocks[y][x][1];
                fK = csouth.blocks[y][x][15];

                target = &&light_xx_z0;
                endTarget = &&end_xx_z0;
                goto prepare;

                light_xx_z0:

                //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                A = c.blocks[My][mx][1];
                B = csouth.blocks[My][mx][15];
                C = csouth.blocks[My][Mx][15];
                D = c.blocks[My][Mx][1];
                E = c.blocks[my][mx][1];
                F = csouth.blocks[my][mx][15];
                G = csouth.blocks[my][Mx][15];
                H = c.blocks[my][Mx][1];

                AB = c.blocks[My][mx][0];
                BC = csouth.blocks[My][x][15];
                CD = c.blocks[My][Mx][0];
                DA = c.blocks[My][x][1];

                EF = c.blocks[my][mx][0];
                FG = csouth.blocks[my][x][15];
                GH = c.blocks[my][Mx][0];
                HE = c.blocks[my][x][1];

                AE = c.blocks[y][mx][1];
                BF = csouth.blocks[y][mx][15];
                CG = csouth.blocks[y][Mx][15];
                DH = c.blocks[y][Mx][1];
                //} else if (isTransparent(type.type)) {
                //    // TODO: try some fun here
                //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                //}

                fx = static_cast<f32>(x + px),
                fy = static_cast<f32>(y),
                fz = static_cast<f32>(pz),
                fmx = static_cast<f32>(mx + px),
                fmy = static_cast<f32>(my),
                fmz = static_cast<f32>(-1 + pz);

                goto render;
                end_xx_z0:;
            }
        }


        // X x Z 15
        for(x = 1; x < 15; x++) {
            if ((block = c.blocks[y][x][15]).type) {
                mx = x - 1;
                Mx = x + 1;

                fT = c.blocks[My][x][15];
                fB = c.blocks[my][x][15];
                fL = c.blocks[y][mx][15];
                fR = c.blocks[y][Mx][15];
                fF = cnorth.blocks[y][x][0];
                fK = c.blocks[y][x][14];

                target = &&light_xx_z15;
                endTarget = &&end_xx_z15;
                goto prepare;

                light_xx_z15:

                //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                A = cnorth.blocks[My][mx][0];
                B = c.blocks[My][mx][14];
                C = c.blocks[My][Mx][14];
                D = cnorth.blocks[My][Mx][0];
                E = cnorth.blocks[my][mx][0];
                F = c.blocks[my][mx][14];
                G = c.blocks[my][Mx][14];
                H = cnorth.blocks[my][Mx][0];

                AB = c.blocks[My][mx][15];
                BC = c.blocks[My][x][14];
                CD = c.blocks[My][Mx][15];
                DA = cnorth.blocks[My][x][0];

                EF = c.blocks[my][mx][15];
                FG = c.blocks[my][x][14];
                GH = c.blocks[my][Mx][15];
                HE = cnorth.blocks[my][x][0];

                AE = cnorth.blocks[y][mx][0];
                BF = c.blocks[y][mx][14];
                CG = c.blocks[y][Mx][14];
                DH = cnorth.blocks[y][Mx][0];
                //} else if (isTransparent(type.type)) {
                //    // TODO: try some fun here
                //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                //}

                fx = static_cast<f32>(x + px),
                fy = static_cast<f32>(y),
                fz = static_cast<f32>(15 + pz),
                fmx = static_cast<f32>(mx + px),
                fmy = static_cast<f32>(my),
                fmz = static_cast<f32>(14 + pz);

                goto render;
                end_xx_z15:;
            }
        }


        // X 0 Z z
        for(z = 1; z < 15; z++) {
            if ((block = c.blocks[y][0][z]).type) {
                mz = z - 1;
                Mz = z + 1;

                fT = c.blocks[My][0][z];
                fB = c.blocks[my][0][z];
                fL = cwest.blocks[y][15][z];
                fR = c.blocks[y][1][z];
                fF = c.blocks[y][0][Mz];
                fK = c.blocks[y][0][mz];

                target = &&light_x0_zz;
                endTarget = &&end_x0_zz;
                goto prepare;

                light_x0_zz:

                //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                A = cwest.blocks[My][15][Mz];
                B = cwest.blocks[My][15][mz];
                C = c.blocks[My][1][mz];
                D = c.blocks[My][1][Mz];
                E = cwest.blocks[my][15][Mz];
                F = cwest.blocks[my][15][mz];
                G = c.blocks[my][1][mz];
                H = c.blocks[my][1][Mz];

                AB = cwest.blocks[My][15][z];
                BC = c.blocks[My][0][mz];
                CD = c.blocks[My][1][z];
                DA = c.blocks[My][0][Mz];

                EF = cwest.blocks[my][15][z];
                FG = c.blocks[my][0][mz];
                GH = c.blocks[my][1][z];
                HE = c.blocks[my][0][Mz];

                AE = cwest.blocks[y][15][Mz];
                BF = cwest.blocks[y][15][mz];
                CG = c.blocks[y][1][mz];
                DH = c.blocks[y][1][Mz];
                //} else if (isTransparent(type.type)) {
                //    // TODO: try some fun here
                //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                //}

                fx = static_cast<f32>(px),
                fy = static_cast<f32>(y),
                fz = static_cast<f32>(z + pz),
                fmx = static_cast<f32>(-1 + px),
                fmy = static_cast<f32>(my),
                fmz = static_cast<f32>(mz + pz);

                goto render;
                end_x0_zz:;
            }
        }


        // X 15 Z z
        for(z = 1; z < 15; z++) {
            if ((block = c.blocks[y][15][z]).type) {
                mz = z - 1;
                Mz = z + 1;

                fT = c.blocks[My][15][z];
                fB = c.blocks[my][15][z];
                fL = c.blocks[y][14][z];
                fR = ceast.blocks[y][0][z];
                fF = c.blocks[y][15][Mz];
                fK = c.blocks[y][15][mz];

                target = &&light_x15_zz;
                endTarget = &&end_x15_zz;
                goto prepare;

                light_x15_zz:

                //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                A = c.blocks[My][14][Mz];
                B = c.blocks[My][14][mz];
                C = ceast.blocks[My][0][mz];
                D = ceast.blocks[My][0][Mz];
                E = c.blocks[my][14][Mz];
                F = c.blocks[my][14][mz];
                G = ceast.blocks[my][0][mz];
                H = ceast.blocks[my][0][Mz];

                AB = c.blocks[My][14][z];
                BC = c.blocks[My][15][mz];
                CD = ceast.blocks[My][0][z];
                DA = c.blocks[My][15][Mz];

                EF = c.blocks[my][14][z];
                FG = c.blocks[my][15][mz];
                GH = ceast.blocks[my][0][z];
                HE = c.blocks[my][15][Mz];

                AE = c.blocks[y][14][Mz];
                BF = c.blocks[y][14][mz];
                CG = ceast.blocks[y][0][mz];
                DH = ceast.blocks[y][0][Mz];
                //} else if (isTransparent(type.type)) {
                //    // TODO: try some fun here
                //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                //}

                fx = static_cast<f32>(15 + px),
                fy = static_cast<f32>(y),
                fz = static_cast<f32>(z + pz),
                fmx = static_cast<f32>(14 + px),
                fmy = static_cast<f32>(my),
                fmz = static_cast<f32>(mz + pz);

                goto render;
                end_x15_zz:;
            }
        }


		center:

        // X x Z z
        for(x = 1; x < 15; x++) {
            for(z = 1; z < 15; z++) {
                if ((block = c.blocks[y][x][z]).type) {
                    mx = x - 1;
                    Mx = x + 1;
                    mz = z - 1;
                    Mz = z + 1;

                    fT = c.blocks[My][x][z];
                    fB = c.blocks[my][x][z];
                    fL = c.blocks[y][mx][z];
                    fR = c.blocks[y][Mx][z];
                    fF = c.blocks[y][x][Mz];
                    fK = c.blocks[y][x][mz];

                    target = &&light_xx_zz;
                    endTarget = &&end_xx_zz;
                    goto prepare;

                    light_xx_zz:

                    //if (isOpaque(type.type) || isSemiTransparent(type.type)) {
                    A = c.blocks[My][mx][Mz];
                    B = c.blocks[My][mx][mz];
                    C = c.blocks[My][Mx][mz];
                    D = c.blocks[My][Mx][Mz];
                    E = c.blocks[my][mx][Mz];
                    F = c.blocks[my][mx][mz];
                    G = c.blocks[my][Mx][mz];
                    H = c.blocks[my][Mx][Mz];

                    AB = c.blocks[My][mx][z];
                    BC = c.blocks[My][x][mz];
                    CD = c.blocks[My][Mx][z];
                    DA = c.blocks[My][x][Mz];

                    EF = c.blocks[my][mx][z];
                    FG = c.blocks[my][x][mz];
                    GH = c.blocks[my][Mx][z];
                    HE = c.blocks[my][x][Mz];

                    AE = c.blocks[y][mx][Mz];
                    BF = c.blocks[y][mx][mz];
                    CG = c.blocks[y][Mx][mz];
                    DH = c.blocks[y][Mx][Mz];
                    //} else if (isTransparent(type.type)) {
                    //    // TODO: try some fun here
                    //    A = B = C = D = E = F = G = H = AB = BC = CD = DA = EF = FG = GH = HE = AE = BF = CG = DH = false;
                    //}

                    fx = static_cast<f32>(x + px),
                    fy = static_cast<f32>(y),
                    fz = static_cast<f32>(z + pz),
                    fmx = static_cast<f32>(mx + px),
                    fmy = static_cast<f32>(my),
                    fmz = static_cast<f32>(mz + pz);

                    goto render;
                    end_xx_zz:;
                }
            }
        }
	}
#pragma endregion

	// debug only: render chunk limits
	// renderLeft(c.coord.x * 16-1, 0, c.coord.z * 16-1, c.coord.x * 16, 128, c.coord.z * 16 + 16, BlockType::Water, 0, 0, 0, 0, 1, false);
	// renderRight(c.coord.x * 16 + 15, 0, c.coord.z * 16-1, c.coord.x * 16 + 16, 128, c.coord.z * 16 + 16, BlockType::Water, 0, 0, 0, 0, 1, false);
	// renderFront(c.coord.x * 16-1, 0, c.coord.z * 16-1, c.coord.x * 16 + 16, 128, c.coord.z * 16, BlockType::Water, 0, 0, 0, 0, 1, false);
	// renderBack(c.coord.x * 16-1, 0, c.coord.z * 16 + 15, c.coord.x * 16 + 16, 128, c.coord.z * 16 + 16, BlockType::Water, 0, 0, 0, 0, 1, false);
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
	} else if (isTransparent(block.type)) {
		tT = fT.type != block.type; // && !isTransparent(fT.type); // AIR/WATER conflict? -> use fT.type != type.type instead
		tB = fB.type != block.type; // && !isTransparent(fB.type);
		tL = fL.type != block.type; // && !isTransparent(fL.type);
		tR = fR.type != block.type; // && !isTransparent(fR.type);
		tF = fF.type != block.type; // && !isTransparent(fF.type);
		tK = fK.type != block.type; // && !isTransparent(fK.type);
	} else if (isSemiTransparent(block.type)) {
		tT = tB = tL = tR = tF = tK = true;
	} else {
		tT = tB = tL = tR = tF = tK = true;
	}

	sz = tT + tB + tL + tR + tF + tK;
	if (sz) {
		goto *target;
	}

	goto
	*endTarget;

#pragma endregion

	render:

	l_tC = initLight, l_tD = initLight, l_tA = initLight, l_tB = initLight;
	l_bE = initLight, l_bH = initLight, l_bG = initLight, l_bF = initLight;
	l_lB = initLight, l_lA = initLight, l_lE = initLight, l_lF = initLight;
	l_rH = initLight, l_rD = initLight, l_rC = initLight, l_rG = initLight;
	l_fA = initLight, l_fD = initLight, l_fH = initLight, l_fE = initLight;
	l_kG = initLight, l_kC = initLight, l_kB = initLight, l_kF = initLight;

	#define xLight(target) (blockData[target.type].isTransparent ? extendedLight[target.light] : (blockData[target.type].emittedLight ? extendedLight[blockData[target.type].emittedLight] : 0))
	#define isEmitter(target) (blockData[target.type].emittedLight)

	lc.contacts = 0;
	le.contacts = isEmitter(block) ? Vertex_A | Vertex_B | Vertex_C | Vertex_D | Vertex_E | Vertex_F | Vertex_G | Vertex_H : 0;

	u16 xlight;


	// Contact face lights
	xlight = xLight(fT);
	if (xlight) {
		l_tC += xlight;
		l_tA += xlight;
		l_tB += xlight;
		l_tD += xlight;

		if (isEmitter(fT)) {
			le.contacts |= Vertex_A | Vertex_B | Vertex_C | Vertex_D;
		}
	}

	xlight = xLight(fB);
	if (xlight) {
		l_bE += xlight;
		l_bH += xlight;
		l_bG += xlight;
		l_bF += xlight;

		if (isEmitter(fB)) {
			le.contacts |= Vertex_E | Vertex_F | Vertex_G | Vertex_H;
		}
	}

	xlight = xLight(fL);
	if (xlight) {
		l_lB += xlight;
		l_lA += xlight;
		l_lE += xlight;
		l_lF += xlight;

		if (isEmitter(fL)) {
			le.contacts |= Vertex_A | Vertex_E | Vertex_B | Vertex_F;
		}
	}

	xlight = xLight(fR);
	if (xlight) {
		l_rH += xlight;
		l_rD += xlight;
		l_rC += xlight;
		l_rG += xlight;

		if (isEmitter(fR)) {
			le.contacts |= Vertex_C | Vertex_G | Vertex_D | Vertex_H;
		}
	}

	xlight = xLight(fF);
	if (xlight) {
		l_fA += xlight;
		l_fD += xlight;
		l_fH += xlight;
		l_fE += xlight;

		if (isEmitter(fF)) {
			le.contacts |= Vertex_A | Vertex_D | Vertex_E | Vertex_H;
		}
	}

	xlight = xLight(fK);
	if (xlight) {
		l_kG += xlight;
		l_kC += xlight;
		l_kB += xlight;
		l_kF += xlight;

		if (isEmitter(fK)) {
			le.contacts |= Vertex_B | Vertex_F | Vertex_C | Vertex_G;
		}
	}

	// Contact line lights
	xlight = xLight(AB);
	if (xlight) {
		l_tA += xlight;
		l_lA += xlight;
		l_tB += xlight;
		l_lB += xlight;

		lc.contacts += Vertex_tA | Vertex_tB | Vertex_lA | Vertex_lB;

		if (isEmitter(AB)) {
			le.contacts |= Vertex_A | Vertex_B;
		}
	}

	xlight = xLight(BC);
	if (xlight) {
		l_tC += xlight;
		l_kC += xlight;
		l_tB += xlight;
		l_kB += xlight;

		lc.contacts += Vertex_tC | Vertex_tB | Vertex_kC | Vertex_kB;

		if (isEmitter(BC)) {
			le.contacts |= Vertex_B | Vertex_C;
		}
	}

	xlight = xLight(CD);
	if (xlight) {
		l_tD += xlight;
		l_rD += xlight;
		l_tC += xlight;
		l_rC += xlight;

		lc.contacts += Vertex_tD | Vertex_tC | Vertex_rD | Vertex_rC;

		if (isEmitter(CD)) {
			le.contacts |= Vertex_C | Vertex_D;
		}
	}

	xlight = xLight(DA);
	if (xlight) {
		l_tA += xlight;
		l_fA += xlight;
		l_tD += xlight;
		l_fD += xlight;

		lc.contacts += Vertex_tA | Vertex_tD | Vertex_fA | Vertex_fD;

		if (isEmitter(DA)) {
			le.contacts |= Vertex_A | Vertex_D;
		}
	}

	xlight = xLight(EF);
	if (xlight) {
		l_bE += xlight;
		l_lE += xlight;
		l_bF += xlight;
		l_lF += xlight;

		lc.contacts += Vertex_bE | Vertex_bF | Vertex_lE | Vertex_lF;

		if (isEmitter(EF)) {
			le.contacts |= Vertex_E | Vertex_F;
		}
	}

	xlight = xLight(FG);
	if (xlight) {
		l_bF += xlight;
		l_kF += xlight;
		l_bG += xlight;
		l_kG += xlight;

		lc.contacts += Vertex_bF | Vertex_bG | Vertex_kF | Vertex_kG;

		if (isEmitter(FG)) {
			le.contacts |= Vertex_F | Vertex_G;
		}
	}

	xlight = xLight(GH);
	if (xlight) {
		l_bG += xlight;
		l_rG += xlight;
		l_bH += xlight;
		l_rH += xlight;

		lc.contacts += Vertex_bG | Vertex_bH | Vertex_rG | Vertex_rH;

		if (isEmitter(GH)) {
			le.contacts |= Vertex_G | Vertex_H;
		}
	}

	xlight = xLight(HE);
	if (xlight) {
		l_bH += xlight;
		l_fH += xlight;
		l_bE += xlight;
		l_fE += xlight;

		lc.contacts += Vertex_bH | Vertex_bE | Vertex_fH | Vertex_fE;

		if (isEmitter(HE)) {
			le.contacts |= Vertex_H | Vertex_E;
		}
	}

	xlight = xLight(AE);
	if (xlight) {
		l_lA += xlight;
		l_fA += xlight;
		l_lE += xlight;
		l_fE += xlight;

		lc.contacts += Vertex_lA | Vertex_lE | Vertex_fA | Vertex_fE;

		if (isEmitter(AE)) {
			le.contacts |= Vertex_A | Vertex_E;
		}
	}

	xlight = xLight(BF);
	if (xlight) {
		l_lB += xlight;
		l_kB += xlight;
		l_lF += xlight;
		l_kF += xlight;

		lc.contacts += Vertex_lB | Vertex_lF | Vertex_kB | Vertex_kF;

		if (isEmitter(BF)) {
			le.contacts |= Vertex_B | Vertex_F;
		}
	}

	xlight = xLight(CG);
	if (xlight) {
		l_rC += xlight;
		l_kC += xlight;
		l_rG += xlight;
		l_kG += xlight;

		lc.contacts += Vertex_rC | Vertex_rG | Vertex_kC | Vertex_kG;

		if (isEmitter(CG)) {
			le.contacts |= Vertex_C | Vertex_G;
		}
	}

	xlight = xLight(DH);
	if (xlight) {
		l_rD += xlight;
		l_fD += xlight;
		l_rH += xlight;
		l_fH += xlight;

		lc.contacts += Vertex_rD | Vertex_rH | Vertex_fD | Vertex_fH;

		if (isEmitter(DH)) {
			le.contacts |= Vertex_D | Vertex_H;
		}
	}

	// contactspoint lights
	xlight = xLight(A);
	if (xlight) {

		bool emitter = isEmitter(A);

		if (lc.T.A) {
			l_tA += xlight;
			lc.contacts += Vertex_tA;
			if (emitter) le.contacts |= Vertex_tA;
		}
		if (lc.L.A) {
			l_lA += xlight;
			lc.contacts += Vertex_lA;
			if (emitter) le.contacts |= Vertex_lA;
		}
		if (lc.F.A) {
			l_fA += xlight;
			lc.contacts += Vertex_fA;
			if (emitter) le.contacts |= Vertex_fA;
		}
	}

	xlight = xLight(B);
	if (xlight) {

		bool emitter = isEmitter(B);

		if (lc.T.B) {
			l_tB += xlight;
			lc.contacts += Vertex_tB;
			if (emitter) le.contacts |= Vertex_tB;
		}
		if (lc.L.B) {
			l_lB += xlight;
			lc.contacts += Vertex_lB;
			if (emitter) le.contacts |= Vertex_lB;
		}
		if (lc.K.B) {
			l_kB += xlight;
			lc.contacts += Vertex_kB;
			if (emitter) le.contacts |= Vertex_kB;
		}
	}

	xlight = xLight(C);
	if (xlight) {

		bool emitter = isEmitter(C);

		if (lc.T.C) {
			l_tC += xlight;
			lc.contacts += Vertex_tC;
			if (emitter) le.contacts |= Vertex_tC;
		}
		if (lc.R.C) {
			l_rC += xlight;
			lc.contacts += Vertex_rC;
			if (emitter) le.contacts |= Vertex_rC;
		}
		if (lc.K.C) {
			l_kC += xlight;
			lc.contacts += Vertex_kC;
			if (emitter) le.contacts |= Vertex_kC;
		}
	}

	xlight = xLight(D);
	if (xlight) {

		bool emitter = isEmitter(D);

		if (lc.T.D) {
			l_tD += xlight;
			lc.contacts += Vertex_tD;
			if (emitter) le.contacts |= Vertex_tD;
		}
		if (lc.R.D) {
			l_rD += xlight;
			lc.contacts += Vertex_rD;
			if (emitter) le.contacts |= Vertex_rD;
		}
		if (lc.F.D) {
			l_fD += xlight;
			lc.contacts += Vertex_fD;
			if (emitter) le.contacts |= Vertex_fD;
		}
	}

	xlight = xLight(E);
	if (xlight) {

		bool emitter = isEmitter(E);

		if (lc.B.E) {
			l_bE += xlight;
			lc.contacts += Vertex_bE;
			if (emitter) le.contacts |= Vertex_bE;
		}
		if (lc.L.E) {
			l_lE += xlight;
			lc.contacts += Vertex_lE;
			if (emitter) le.contacts |= Vertex_lE;
		}
		if (lc.F.E) {
			l_fE += xlight;
			lc.contacts += Vertex_fE;
			if (emitter) le.contacts |= Vertex_fE;
		}
	}

	xlight = xLight(F);
	if (xlight) {

		bool emitter = isEmitter(F);

		if (lc.B.F) {
			l_bF += xlight;
			lc.contacts += Vertex_bF;
			if (emitter) le.contacts |= Vertex_bF;
		}
		if (lc.L.F) {
			l_lF += xlight;
			lc.contacts += Vertex_lF;
			if (emitter) le.contacts |= Vertex_lF;
		}
		if (lc.K.F) {
			l_kF += xlight;
			lc.contacts += Vertex_kF;
			if (emitter) le.contacts |= Vertex_kF;
		}
	}

	xlight = xLight(G);
	if (xlight) {

		bool emitter = isEmitter(G);

		if (lc.B.G) {
			l_bG += xlight;
			lc.contacts += Vertex_bG;
			if (emitter) le.contacts |= Vertex_bG;
		}
		if (lc.R.G) {
			l_rG += xlight;
			lc.contacts += Vertex_rG;
			if (emitter) le.contacts |= Vertex_rG;
		}
		if (lc.K.G) {
			l_kG += xlight;
			lc.contacts += Vertex_kG;
			if (emitter) le.contacts |= Vertex_kG;
		}
	}

	xlight = xLight(H);
	if (xlight) {

		bool emitter = isEmitter(H);

		if (lc.B.H) {
			l_bH += xlight;
			lc.contacts += Vertex_bH;
			if (emitter) le.contacts |= Vertex_bH;
		}
		if (lc.R.H) {
			l_rH += xlight;
			lc.contacts += Vertex_rH;
			if (emitter) le.contacts |= Vertex_rH;
		}
		if (lc.F.H) {
			l_fH += xlight;
			lc.contacts += Vertex_fH;
			if (emitter) le.contacts |= Vertex_fH;
		}
	}

	/// Ambient occlusion correction ('le' controls the correction channel to select, if 'le' flag set: the occlusion is fully canceled)
	if (lc.T.C != 3) l_tC += correctedLight[le.T.C][lc.T.C][reducedLight[l_tC]];
	if (lc.T.D != 3) l_tD += correctedLight[le.T.D][lc.T.D][reducedLight[l_tD]];
	if (lc.T.A != 3) l_tA += correctedLight[le.T.A][lc.T.A][reducedLight[l_tA]];
	if (lc.T.B != 3) l_tB += correctedLight[le.T.B][lc.T.B][reducedLight[l_tB]];

	if (lc.B.E != 3) l_bE += correctedLight[le.B.E][lc.B.E][reducedLight[l_bE]];
	if (lc.B.H != 3) l_bH += correctedLight[le.B.H][lc.B.H][reducedLight[l_bH]];
	if (lc.B.G != 3) l_bG += correctedLight[le.B.G][lc.B.G][reducedLight[l_bG]];
	if (lc.B.F != 3) l_bF += correctedLight[le.B.F][lc.B.F][reducedLight[l_bF]];

	if (lc.L.B != 3) l_lB += correctedLight[le.L.B][lc.L.B][reducedLight[l_lB]];
	if (lc.L.A != 3) l_lA += correctedLight[le.L.A][lc.L.A][reducedLight[l_lA]];
	if (lc.L.E != 3) l_lE += correctedLight[le.L.E][lc.L.E][reducedLight[l_lE]];
	if (lc.L.F != 3) l_lF += correctedLight[le.L.F][lc.L.F][reducedLight[l_lF]];

	if (lc.R.H != 3) l_rH += correctedLight[le.R.H][lc.R.H][reducedLight[l_rH]];
	if (lc.R.D != 3) l_rD += correctedLight[le.R.D][lc.R.D][reducedLight[l_rD]];
	if (lc.R.C != 3) l_rC += correctedLight[le.R.C][lc.R.C][reducedLight[l_rC]];
	if (lc.R.G != 3) l_rG += correctedLight[le.R.G][lc.R.G][reducedLight[l_rG]];

	if (lc.F.A != 3) l_fA += correctedLight[le.F.A][lc.F.A][reducedLight[l_fA]];
	if (lc.F.D != 3) l_fD += correctedLight[le.F.D][lc.F.D][reducedLight[l_fD]];
	if (lc.F.H != 3) l_fH += correctedLight[le.F.H][lc.F.H][reducedLight[l_fH]];
	if (lc.F.E != 3) l_fE += correctedLight[le.F.E][lc.F.E][reducedLight[l_fE]];

	if (lc.K.G != 3) l_kG += correctedLight[le.K.G][lc.K.G][reducedLight[l_kG]];
	if (lc.K.C != 3) l_kC += correctedLight[le.K.C][lc.K.C][reducedLight[l_kC]];
	if (lc.K.B != 3) l_kB += correctedLight[le.K.B][lc.K.B][reducedLight[l_kB]];
	if (lc.K.F != 3) l_kF += correctedLight[le.K.F][lc.K.F][reducedLight[l_kF]];

	if (isRegular(block.type)) {
		//GX_Begin(GX_QUADS, GX_VTXFMT0, sz << 2); // Start drawing
		if (tT) renderTop   (fx, fy, fz, fmx, fmy, fmz, block.type, l_tC, l_tD, l_tA, l_tB, static_cast<RenderChannel>(isTransparent(block.type)), reverseQuadTB[lc.T.contact]); // CDAB
		if (tB) renderBottom(fx, fy, fz, fmx, fmy, fmz, block.type, l_bE, l_bH, l_bG, l_bF, static_cast<RenderChannel>(isTransparent(block.type)), reverseQuadTB[lc.B.contact]); // EHGF
		if (tL) renderLeft  (fx, fy, fz, fmx, fmy, fmz, block.type, l_lB, l_lA, l_lE, l_lF, static_cast<RenderChannel>(isTransparent(block.type)), reverseQuadLR[lc.L.contact]); // BAEF
		if (tR) renderRight (fx, fy, fz, fmx, fmy, fmz, block.type, l_rH, l_rD, l_rC, l_rG, static_cast<RenderChannel>(isTransparent(block.type)), reverseQuadLR[lc.R.contact]); // HDCG
		if (tF) renderFront (fx, fy, fz, fmx, fmy, fmz, block.type, l_fA, l_fD, l_fH, l_fE, static_cast<RenderChannel>(isTransparent(block.type)), reverseQuadFK[lc.F.contact]); // ADHE
		if (tK) renderBack  (fx, fy, fz, fmx, fmy, fmz, block.type, l_kG, l_kC, l_kB, l_kF, static_cast<RenderChannel>(isTransparent(block.type)), reverseQuadFK[lc.K.contact]); // GCBF
		//GX_End();
	} else {


		goto *renderers[block.type - BlockType::IRREGULAR];

		render_Furnace:
		{
			auto &data = blockData[block.type];
			if (tT) {
				u16 tc = data.tc[BlockFace::Top];
				renderRawTop(fx, fy, fz, fmx, fmy, fmz, tc + 17, tc + 18, tc + 1, tc, l_tC, l_tD, l_tA, l_tB, static_cast<RenderChannel>(isTransparent(block.type))); // CDAB
			}
			if (tB) {
				u16 tc = data.tc[BlockFace::Bottom];
				renderRawBottom(fx, fy, fz, fmx, fmy, fmz, tc + 18, tc + 1, tc, tc + 17, l_bE, l_bH, l_bG, l_bF, static_cast<RenderChannel>(isTransparent(block.type))); // EHGF
			}
			if (tL) {
				u16 tc = data.tc[(BlockFace::West + block.orient) & 0b11];
				renderRawWest(fx, fy, fz, fmx, fmy, fmz, tc, tc + 17, tc + 18, tc + 1, l_lB, l_lA, l_lE, l_lF, static_cast<RenderChannel>(isTransparent(block.type))); // BAEF
			}
			if (tR) {
				u16 tc = data.tc[(BlockFace::East + block.orient) & 0b11];
				renderRawEast(fx, fy, fz, fmx, fmy, fmz, tc + 1, tc, tc + 17, tc + 18, l_rH, l_rD, l_rC, l_rG, static_cast<RenderChannel>(isTransparent(block.type))); // HDCG
			}
			if (tF) {
				u16 tc = data.tc[(BlockFace::North + block.orient) & 0b11];
				renderRawNorth(fx, fy, fz, fmx, fmy, fmz, tc, tc + 17, tc + 18, tc + 1, l_fA, l_fD, l_fH, l_fE, static_cast<RenderChannel>(isTransparent(block.type))); // ADHE
			}
			if (tK) {
				u16 tc = data.tc[(BlockFace::South + block.orient) & 0b11];
				renderRawSouth(fx, fy, fz, fmx, fmy, fmz, tc + 1, tc, tc + 17, tc + 18, l_kG, l_kC, l_kB, l_kF, static_cast<RenderChannel>(isTransparent(block.type))); // GCBF
			}
			goto
			*endTarget;
		}

		render_Door:
		{
			fy += 1.f;
			switch (static_cast<Direction>((block.orient + (block.state & 1)) & 0b11)) {
				case NORTH: fmz = fz - 3.f / 16.f;
					break;
				case EAST: fmx = fx - 3.f / 16.f;
					break;
				case SOUTH: fz = fmz + 3.f / 16.f;
					break;
				case WEST: fx = fmx + 3.f / 16.f;
					break;
			}

			auto &data = blockData[block.type];
			if (tT) {
				u16 tc = data.tc[BlockFace::Top];
				renderRawTop(fx, fy, fz, fmx, fmy, fmz, tc + 17, tc + 18, tc + 1, tc, l_tC, l_tD, l_tA, l_tB, static_cast<RenderChannel>(isTransparent(block.type))); // CDAB
			}
			if (tB) {
				u16 tc = data.tc[BlockFace::Bottom];
				renderRawBottom(fx, fy, fz, fmx, fmy, fmz, tc + 18, tc + 1, tc, tc + 17, l_bE, l_bH, l_bG, l_bF, static_cast<RenderChannel>(isTransparent(block.type))); // EHGF
			}
			if (tL) {
				u16 tc = data.tc[(BlockFace::West + block.orient) & 0b11];
				renderRawWest(fx, fy, fz, fmx, fmy, fmz, tc, tc + 17, tc + 18, tc + 1, l_lB, l_lA, l_lE, l_lF, static_cast<RenderChannel>(isTransparent(block.type))); // BAEF
			}
			if (tR) {
				u16 tc = data.tc[(BlockFace::East + block.orient) & 0b11];
				renderRawEast(fx, fy, fz, fmx, fmy, fmz, tc + 1, tc, tc + 17, tc + 18, l_rH, l_rD, l_rC, l_rG, static_cast<RenderChannel>(isTransparent(block.type))); // HDCG
			}
			if (tF) {
				u16 tc = data.tc[(BlockFace::North + block.orient) & 0b11];
				renderRawNorth(fx, fy, fz, fmx, fmy, fmz, tc, tc + 17, tc + 18, tc + 1, l_fA, l_fD, l_fH, l_fE, static_cast<RenderChannel>(isTransparent(block.type))); // ADHE
			}
			if (tK) {
				u16 tc = data.tc[(BlockFace::South + block.orient) & 0b11];
				renderRawSouth(fx, fy, fz, fmx, fmy, fmz, tc + 1, tc, tc + 17, tc + 18, l_kG, l_kC, l_kB, l_kF, static_cast<RenderChannel>(isTransparent(block.type))); // GCBF
			}
			goto
			*endTarget;
		}
	}

	render_Void:
	goto
	*endTarget;

}
#pragma clang diagnostic pop
