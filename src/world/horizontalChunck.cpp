//
// Created by paulo on 06/02/2024.
//

#include "horizontalChunck.h"


HorizontalChunk::HorizontalChunk(int height, BlockType t) {
    for (int i = 0; i < height; i++) {
        blocks.emplace_back(t);
    }
}

HorizontalChunk::HorizontalChunk(int height) {
    for (int i = 0; i < height; i++) {
        blocks.emplace_back(BlockType::Dirt);
    }
}

HorizontalChunk::HorizontalChunk() {
    for (int i = 0; i < CHUNK_WIDTH; i++) {
        blocks.emplace_back(BlockType::Air);
    }
}

Block HorizontalChunk::getBlock(int i) {
    return blocks[i];
}


const int HorizontalChunk::CHUNK_WIDTH = 16;



