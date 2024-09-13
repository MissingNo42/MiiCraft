//
// Created by Romain on 07/02/2024.
//

#ifndef MIICRAFT_BLOC_H
#define MIICRAFT_BLOC_H

#include <gctypes.h>
#include <map>

#define OFFSET 0.03125f // 1/32 = 16/512

#define BLOC_FACE_TOP 0 // +y
#define BLOC_FACE_BOTTOM 1 // -y
#define BLOC_FACE_RIGHT 2 // +x
#define BLOC_FACE_LEFT 3 // -x
#define BLOC_FACE_FRONT 4 // +z
#define BLOC_FACE_BACK 5 // -z

#define TXCOORD(x, y) (17 * (x) + (y))

#define BLOCK_COORD(x) (x)
#define BLOCK_COORDS_ALL(a) { BLOCK_COORD(a), BLOCK_COORD(a), BLOCK_COORD(a), BLOCK_COORD(a), BLOCK_COORD(a), BLOCK_COORD(a) }
#define BLOCK_COORDS_SIDE(a, t, b) { BLOCK_COORD(t), BLOCK_COORD(b), BLOCK_COORD(a), BLOCK_COORD(a), BLOCK_COORD(a), BLOCK_COORD(a) }
#define BLOCK_COORDS(a, b, c, d, e, f) { BLOCK_COORD(a), BLOCK_COORD(b), BLOCK_COORD(c), BLOCK_COORD(d), BLOCK_COORD(e), BLOCK_COORD(f) }

enum Interaction: u8 {
	// Player triggers
	Hand = 0, // default, player hand (!= fire maker)
	
	// Environment triggers
	Redstone = 1, // redstone wire, torch, etc.
};

using Interactive = bool(*)();
using Spawner = void(*)();
using Despawner = void(*)();
using Render = void(*)();

struct BlockData {
	u16 x[6] = {0};
	u16 y[6] = {0};
	u8 isOrientable: 1 = 0;  // stairs, doors, etc. -> managed by the engine, no need for custom Spawner/Renderer
	u8 isInteractive: 1 = 0;  // crafting table, furnace, doors, TNT, etc.
	u8 hasSpawner: 1 = 0;  // has a setup function (e.g. door for placing two blocks)
	u8 hasDespawner: 1 = 0;  // has a teardown function (e.g. door for removing two blocks)
	u8 hasRenderer: 1 = 0; // has a custom renderer function
	Interactive interactive = nullptr;
	Spawner spawner = nullptr;
	Despawner despawner = nullptr;
	Render render = nullptr;
	static void blockInit();
};

extern BlockData blockData[];

#endif //MIICRAFT_BLOC_H
