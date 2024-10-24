//
// Created by guill on 06/02/2024.
//

#ifndef WORLD_COORD_H
#define WORLD_COORD_H

#include <queue>
#include <cstdint>
#include "block.h"
#include <gctypes.h>

enum Direction : u8 {
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};

struct ChunkCoord {
    s32 x = 0, y = 0;
	
    [[nodiscard]] inline bool operator==(const ChunkCoord& p) const {
	    return x == p.x && y == p.y;
	}
	
    [[nodiscard]] inline bool operator<(const ChunkCoord& p) const {
	    return p.x == x ? y < p.y: x < p.x;
	}
};

struct BlockCoord {
    s32 x, y, z;
	
    [[nodiscard]] inline bool operator==(const BlockCoord& coord) const {
	    return (x == coord.x && y == coord.y && z == coord.z);
	}
	
    [[nodiscard]] inline bool operator<(const BlockCoord& coord) const {
	    return coord.x == x ? (coord.y == y ? z < coord.z: y < coord.y): x < coord.x;
	}
	
	[[nodiscard]] inline ChunkCoord toChunkCoord() const {
	    return {x >> 4, z >> 4};
	}
};

#endif //WORLD_COORD_H
