//
// Created by paulo on 06/02/2024.
//

#include "verticalChunk.h"

t_pos2D::t_pos2D() : t_pos2D(0, 0) {}
t_pos2D::t_pos2D(short x, short y) : x(x), y(y) {}

bool const t_pos2D::operator==(const t_pos2D &p) const {
    return x == p.x && y == p.y;
}

bool const t_pos2D::operator<(const t_pos2D &p) const {
    if (p.x == x){return y < p.y;}
    else {return x < p.x;}
}
VerticalChunk::VerticalChunk() {}


void VerticalChunk::VC_SetBlock(t_coord coord, BlockType block) {
    t_coord c = {coord.x % 16, coord.y % 16, coord.z % 16};
    chunks[c.y]->HC_SetBlock(c, block);
}
