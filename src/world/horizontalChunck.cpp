//
// Created by paulo on 06/02/2024.
//

#include "horizontalChunck.h"

#include <utility>

HorizontalChunk::HorizontalChunk(std::vector<Block> blocks) : blocks(std::move(blocks)) {}

HorizontalChunk::HorizontalChunk(int height, Block t) {
    for (int i = 0; i < (16-height) * CHUNK_HEIGHT * CHUNK_WIDTH; i++) {
        blocks.emplace_back(Air);
    }
    for (int i = 0; i < height; i++) {
        blocks.emplace_back(t);
    }
}

HorizontalChunk::HorizontalChunk(int height) : HorizontalChunk(height, Dirt) {}

HorizontalChunk::HorizontalChunk() : HorizontalChunk(0, Air) {}


Block HorizontalChunk::getBlock(int i) {
    return blocks[i];
}

void HorizontalChunk::HC_SetBlock(t_coord coord, BlockType block) {
    blocks[coord.x + coord.y * CHUNK_WIDTH + coord.z * CHUNK_WIDTH * CHUNK_HEIGHT] = block;
}





