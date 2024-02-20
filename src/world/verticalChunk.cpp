//
// Created by paulo on 06/02/2024.
//

#include "verticalChunk.h"
#include <cstring>

t_pos2D::t_pos2D() : t_pos2D(0, 0) {}
t_pos2D::t_pos2D(short x, short y) : x(x), y(y) {}

bool t_pos2D::operator==(const t_pos2D &p) const {
    return x == p.x && y == p.y;
}

bool t_pos2D::operator<(const t_pos2D &p) const {
    if (p.x == x){return y < p.y;}
    else {return x < p.x;}
}


VerticalChunk VerticalChunk::emptyChunk;

VerticalChunk::VerticalChunk() : id(cpt++) {
    for(auto & neighboor : neighboors){
        neighboor = &VerticalChunk::emptyChunk;
    }
}


void VerticalChunk::VC_SetBlock(t_coord coord, BlockType block) {
	recache = 1;
    t_coord c = {coord.x & 15, coord.y, coord.z & 15};
    blocks[c.x][c.y][c.z].type = block;
}

Block VerticalChunk::VC_GetBlock(t_coord coord) {
    t_coord c = {coord.x  & 15, coord.y, coord.z & 15};
    return blocks[c.x][c.y][c.z];
}

void VerticalChunk::VC_SetNeighboors(int indice, VerticalChunk *chunk) {
    neighboors[indice] = chunk;
}





const char* VerticalChunk::toString() {
    return (char*) blocks;
}

void VerticalChunk::fillWithBedrock() {
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 128; ++j) {
            for (int k = 0; k < 16; ++k) {
                blocks[i][j][k] = BlockType::Bedrock;
            }
        }
    }
}

VerticalChunk *VerticalChunk::VC_GetNeighboors() {
    return *neighboors;
}

VerticalChunk* VerticalChunk::VC_GetNeighboor(int indice ) {
    return neighboors[indice];
}
