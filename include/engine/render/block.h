//
// Created by Romain on 07/02/2024.
//

#ifndef MIICRAFT_BLOC_H
#define MIICRAFT_BLOC_H

#include <gctypes.h>
#include <map>

#define OFFSET 0.03125f // 1/32 = 16/512

#define BLOCK_FACE_TOP 0 // +y
#define BLOCK_FACE_BOTTOM 1 // -y
#define BLOCK_FACE_RIGHT 2 // +x
#define BLOCK_FACE_LEFT 3 // -x
#define BLOCK_FACE_FRONT 4 // +z
#define BLOCK_FACE_BACK 5 // -z

#define TXCOORD(x, y) (17 * (x) + (y))

#define TILE_COORD(x) ((x) * OFFSET)
#define TILE_COORDS(x, y) (x) * OFFSET, (y) * OFFSET
#define BLOCK_COORDS_ALL(a) { (a), (a), (a), (a), (a), (a) }
#define BLOCK_COORDS_SIDE(a, t, b) { (t), (b), (a), (a), (a), (a) }
#define BLOCK_COORDS(a, b, c, d, e, f) { (a), (b), (c), (d), (e), (f) }

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
