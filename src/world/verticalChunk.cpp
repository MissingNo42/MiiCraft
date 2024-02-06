//
// Created by paulo on 06/02/2024.
//

#include "verticalChunk.h"

ChunkPos::ChunkPos() : ChunkPos(0, 0) {}
ChunkPos::ChunkPos(short x, short y) : x(x), y(y) {}

bool const ChunkPos::operator==(const ChunkPos &p) const {
    return x == p.x && y == p.y;
}

bool const ChunkPos::operator<(const ChunkPos &p) const {
    if (p.x == x){return y < p.y;}
    else {return x < p.x;}
}
VerticalChunk::VerticalChunk() : testType(BlockType::Air) {}

VerticalChunk::VerticalChunk(BlockType t) : testType(t) {}
