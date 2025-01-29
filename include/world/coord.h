#pragma once

#include <gctypes.h>
#include <memory>

constexpr u16 CHUNK_HEIGHT = 0x80u;
constexpr u8 CHUNK_LIMIT  = CHUNK_HEIGHT - 1u; // Must be maskable, see BlockCoord::isValid() below

enum Direction : u8 {
	NORTH = 0,
	EAST  = 1,
	SOUTH = 2,
	WEST  = 3
};


struct ChunkCoord {
	s32 x = 0, z = 0;

	[[nodiscard]] bool operator==(const ChunkCoord & p) const {
		return x == p.x && z == p.z;
	}

	[[nodiscard]] bool operator<(const ChunkCoord & p) const {
		return p.x == x ? z < p.z : x < p.x;
	}
};


struct BlockCoord {
	s32 y, x, z;

	[[nodiscard]] bool operator!=(const BlockCoord & coord) const {
		return y != coord.y || x != coord.x || z != coord.z;
	}

	[[nodiscard]] bool operator==(const BlockCoord & coord) const {
		return y == coord.y && x == coord.x && z == coord.z;
	}

	[[nodiscard]] bool operator<(const BlockCoord & coord) const {
		return coord.y == y ? (coord.x == x ? z < coord.z : x < coord.x) : y < coord.y;
	}

	[[nodiscard]] ChunkCoord toChunkCoord() const {
		return {x >> 4, z >> 4};
	}

	void normalized() {
		x &= 0x0F;
		z &= 0x0F;
	}

	/**
	 * @return true if usable as in-chunk block position (if normalized and y valid)
	 */
	[[nodiscard]] bool isValid() const {
		return (y & CHUNK_LIMIT) == y && (x & 0x0F) == x && (z & 0x0F) == z;
	}
};


template <>
struct std::hash<BlockCoord> {
	std::size_t operator()(const BlockCoord & coord) const noexcept {
		return static_cast<u16>(coord.y) << 16 | static_cast<u8>(coord.x) << 8 | static_cast<u8>(coord.z);
	}
};
