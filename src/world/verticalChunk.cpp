//
// Created by paulo on 06/02/2024.
//

#include "verticalChunk.h"
#include "world.h"
#include <cstring>



void VerticalChunk::SetBlock(BlockCoord cd, BlockType block) {
	recache = 1;
	dirty = 1;
    blocks[cd.x & 15][cd.y][cd.z & 15] = block;
}

BlockType VerticalChunk::GetBlock(BlockCoord c) {
    return blocks[c.x & 15][c.y][c.z & 15];
}

void VerticalChunk::SetNeighboor(u8 indice, u16 chunk) {
	recache = 1;
    neighboors[indice] = chunk;
}


void VerticalChunk::fillWith(BlockType block) {
	memset(blocks, block, sizeof(blocks));
}

u16 VerticalChunk::GetNeighboor(int indice) {
    return neighboors[indice];
}

VerticalChunk& VerticalChunk::GetNeighboorChunk(int indice) {
    return World::chunkSlots[neighboors[indice]];
}

u16 VerticalChunk::Count(BlockType block) {
	u16 r = 0;
	for (int i = 0; i < 16; ++i) {
		for (int j = 0; j < 128; ++j) {
			for (int k = 0; k < 16; ++k) {
				if (blocks[i][j][k] == block) r++;
			}
		}
	}
	return r;
}
