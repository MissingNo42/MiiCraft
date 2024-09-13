//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_VERTICALCHUNK_H
#define MIICRAFT_VERTICALCHUNK_H

#include "coord.h"
#include "block.h"
#include <cstdint>
#include <queue>

enum Neighboor : u8 {
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};


/**
 * @class VerticalChunk
 * @brief One of these fixed unit stores a currently loaded chunk
 * The represented loaded chunk change to one another during the game lifetime
 * */
class VerticalChunk {

public:
	std::queue<BlockCoord> lightQueue;
	std::queue<BlockCoord> blockLightQueue;
	u16 neighboors[4]{0, 0, 0, 0};
	u16 id;
	
	u8 recache = 1; // 1 if the chunk needs to be recached
	u8 loaded = 0;
	u8 dirty = 0;
	
	ChunkCoord coord;
	Block blocks[16][128][16];
	
	void SetBlock(BlockCoord coord, Block block);
	void SetBlockType(BlockCoord coord, BlockType block);
	
	inline void SetBlock(BlockCoord bcoord, BlockType block) {
		SetBlock(bcoord, {block, {0}});
	}
	
	[[nodiscard]] inline Block GetBlock(BlockCoord bcoord) const noexcept {
		return blocks[bcoord.x & 15][bcoord.y][bcoord.z & 15];
	}
	
	[[nodiscard]] inline BlockType GetBlockType(BlockCoord bcoord) const noexcept {
		return blocks[bcoord.x & 15][bcoord.y][bcoord.z & 15].type;
	}
	
	void inline SetNeighboor(Neighboor neighboor, u16 chunk) noexcept {
		recache = 1;
		neighboors[neighboor] = chunk;
	}
	
	[[nodiscard]] inline VerticalChunk& GetNeighboorChunk(Neighboor neighboor) const noexcept;
	
	void inline fillWith(Block block = {BlockType::Bedrock, {0}}) noexcept {
		std::wmemset((wchar_t *) (blocks), *(wchar_t *) &block, sizeof(blocks));
	}
	
	void inline fillWith(BlockType block) noexcept {
		fillWith({block, {0}});
	}
};


#endif //MIICRAFT_VERTICALCHUNK_H
