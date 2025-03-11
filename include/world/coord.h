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
	static constexpr s32 size2 = sizeof(size_t) << 2; // half size in bit
	static constexpr s32 size4 = sizeof(size_t) << 1; // quarter size in bit
	static constexpr s32 mask2 = (1 << size2) - 1;
	static constexpr s32 mask4 = (1 << size4) - 1;

	size_t operator()(const BlockCoord & coord) const noexcept {
		return (coord.y & mask2) << size2 | (coord.x & mask4) << size4 | (coord.z & mask4);
	}
};

template <>
struct std::hash<ChunkCoord> {
	static constexpr s32 size2 = sizeof(size_t) << 2; // half size in bit
	static constexpr s32 mask2 = (1 << size2) - 1;

	size_t operator()(const ChunkCoord & coord) const noexcept {
		return (coord.x & mask2) << size2 | (coord.z & mask2);
	}
};
