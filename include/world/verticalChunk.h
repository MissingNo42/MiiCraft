//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_VERTICALCHUNK_H
#define MIICRAFT_VERTICALCHUNK_H

#include "coord.h"
#include "block.h"
#include <cstdint>
#include <queue>


/**
 * @class VerticalChunk
 * @brief One of these fixed unit stores a currently loaded chunk
 * The represented loaded chunk change to one another during the game lifetime
 * */
class VerticalChunk {

public:
	std::queue<BlockCoord> lightQueue;
	u16 neighboors[4]{0, 0, 0, 0};
	u16 id;
	
	u8 recache = 1; // 1 if the chunk needs to be recached
	u8 loaded = 0;
	u8 dirty = 0;
	
	ChunkCoord coord;
	Block blocks[16][128][16]; // y=0 must be Bedrock & y=127 must be Air
	u8 lightMap[16][16]; // high of the first non "sun-lighted" block (or 0)
	
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
	
	void inline SetNeighboor(Direction neighboor, u16 chunk) noexcept {
		recache = 1;
		neighboors[neighboor] = chunk;
	}
	
	[[nodiscard]] VerticalChunk& GetNeighboorChunk(Direction neighboor) const noexcept;
	
	void inline fillWith(Block block = {BlockType::Bedrock, {0}}) noexcept {
		for (s32 i = sizeof(blocks) / sizeof(Block); i--;) {
			((Block *)blocks)[i] = block;
		}
	}
	
	void inline fillWith(BlockType block) noexcept {
		fillWith({block, {0}});
	}
};


#endif //MIICRAFT_VERTICALCHUNK_H
