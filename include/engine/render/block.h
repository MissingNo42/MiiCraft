//
// Created by Romain on 07/02/2024.
//

#ifndef MIICRAFT_BLOC_H
#define MIICRAFT_BLOC_H

#include <gctypes.h>
#include <map>
#include "world/coord.h"
#include "world/block.h"

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

using Interactive = bool(*)(BlockCoord, Interaction);
using Spawner = bool(*)(BlockCoord, BlockType, Direction); // true if spawned, false if not
using Despawner = void(*)(BlockCoord);
using Render = void(*)();

[[nodiscard]] bool spawnDefault(BlockCoord, BlockType, Direction);
void despawnDefault(BlockCoord);

struct BlockData {
	u16 x[6] = {0};
	u16 y[6] = {0};
	u8 isOrientable: 1 = 0;  // stairs, doors, etc. -> managed by the engine, no need for custom Spawner/Renderer
	u8 isSelectable: 1 = 1; // can be selected by the player
	u8 isBurnable: 1 = 0;   // can be destroyed by fire
	u8 isFloor: 1 = 1;      // block can be considered as a floor (e.g. can't put doors on flowers, water...)
	u8 allowAbove: 1 = 1;   // block can be placed above it
	u8 needFloor: 1 = 0;    // block needs a floor to be placed
	Interactive interactive = nullptr;    // crafting table, furnace, doors, TNT, etc.
	Spawner spawner = spawnDefault;       // a setup function (e.g. door for placing two blocks)
	Despawner despawner = despawnDefault; // a teardown function (e.g. door for removing two blocks)
	Render render = nullptr;              // a custom renderer function
};

extern BlockData blockData[];

#endif //MIICRAFT_BLOC_H
