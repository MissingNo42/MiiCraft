#pragma once

#include <gctypes.h>
#include "world/block.h"
#include "world/coord.h"

constexpr f32 TEXPIX = 1.f / 512.f;
constexpr f32 OFFSET = 16.f / 512.f; // 0.03125f

enum BlockFace: u8 {
	North = 0,  // +z: Front
	East = 1,   // +x: Right
	South = 2,  // -z: Back
	West = 3,   // -x: Left
	Top = 4,    // +y: Top
	Bottom = 5  // -y: Bottom
};

consteval f32 PIX_COORD(const u16 pix) {
	return static_cast<f32>(pix) * TEXPIX;
}

#define TXCOORD(x, y) (17 * (x) + (y))

#define TILE_COORD(x) ((x) * OFFSET)
#define TILE_COORDS(x, y) (x) * OFFSET, (y) * OFFSET
#define TILE_INDEX_ALL(x, y) { TXCOORD(x, y), TXCOORD(x, y), TXCOORD(x, y), TXCOORD(x, y), TXCOORD(x, y), TXCOORD(x, y) }
#define TILE_INDEX_SIDE(x, y, tx, ty, bx, by) { TXCOORD(x, y), TXCOORD(x, y), TXCOORD(x, y), TXCOORD(x, y), TXCOORD(tx, ty), TXCOORD(bx, by) }
#define TILE_INDEX_FACE(x, y, tx, ty, nx, ny) { TXCOORD(nx, ny), TXCOORD(x, y), TXCOORD(x, y), TXCOORD(x, y), TXCOORD(tx, ty), TXCOORD(tx, ty) }

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
	u16 tc[6] = {0};         // TL coords for each face (for the order, see BlockFace enum)
	u8 isOrientable: 1 = 0;  // stairs, doors, etc. -> managed by the engine, no need for custom Spawner/Renderer // TODO true?
	u8 isSelectable: 1 = 1;  // can be selected by the player (raycast hit)
	u8 isBurnable: 1 = 0;    // can be destroyed by fire
	u8 isFloor: 1 = 1;       // block can be considered as a floor (e.g. can't put doors on flowers, water...)
	u8 allowAbove: 1 = 1;    // block can be placed above it
	u8 needFloor: 1 = 0;     // block needs a floor to be placed
	u8 isTransparent: 1 = 0; // block transmit light (e.g. glass)
	u8 isLightFilter: 1 = 1; // (if isTransparent) block decrease light intensity
	u8 emittedLight: 4 = 0;  // light emitted by the block
	Interactive interactive = nullptr;    // crafting table, furnace, doors, TNT, etc.
	Spawner spawner = spawnDefault;       // a setup function (e.g. door for placing two blocks)
	Despawner despawner = despawnDefault; // a teardown function (e.g. door for removing two blocks)
	Render render = nullptr;              // a custom renderer function TODO use a Render Goto Code ID ?
};

extern const BlockData blockData[] ;
