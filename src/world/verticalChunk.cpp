//
// Created by paulo on 06/02/2024.
//

#include "world/verticalChunk.h"
#include "world/world.h"
#include "world/coord.h"

void VerticalChunk::SetBlock(BlockCoord cd, Block block) {
	recache = 1;
	dirty = 1;
	cd.x &= 15; // normalize in chunk coord
	cd.z &= 15; // normalize in chunk coord
	blocks[cd.x][cd.y][cd.z] = block;
	
	if (!cd.x) {
		u16 u = neighboors[Direction::WEST];
		if (u) World::chunkSlots[u].recache = 1;
	} else if (cd.x == 15) {
		u16 u = neighboors[Direction::EAST];
		if (u) World::chunkSlots[u].recache = 1;
	}
	
	if (!cd.z) {
		u16 u = neighboors[Direction::SOUTH];
		if (u) World::chunkSlots[u].recache = 1;
	} else if (cd.z == 15) {
		u16 u = neighboors[Direction::NORTH];
		if (u) World::chunkSlots[u].recache = 1;
	}
}

void VerticalChunk::SetBlockType(BlockCoord cd, BlockType block) {
	recache = 1;
	dirty = 1;
	cd.x &= 15; // normalize in chunk coord
	cd.z &= 15; // normalize in chunk coord
	blocks[cd.x][cd.y][cd.z] = {block, {0}};
	
	if (!cd.x) {
		u16 u = neighboors[Direction::WEST];
		if (u) World::chunkSlots[u].recache = 1;
	} else if (cd.x == 15) {
		u16 u = neighboors[Direction::EAST];
		if (u) World::chunkSlots[u].recache = 1;
	}
	
	if (!cd.z) {
		u16 u = neighboors[Direction::SOUTH];
		if (u) World::chunkSlots[u].recache = 1;
	} else if (cd.z == 15) {
		u16 u = neighboors[Direction::NORTH];
		if (u) World::chunkSlots[u].recache = 1;
	}
}

VerticalChunk& VerticalChunk::GetNeighboorChunk(Direction neighboor) const noexcept {
	return World::chunkSlots[neighboors[neighboor]];
}

