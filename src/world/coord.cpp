#include "world/coord.h"

BlockCoord::BlockCoord(int x, int y, int z) : x(x), y(y), z(z) {}


ChunkCoord::ChunkCoord(short x, short y) : x(x), y(y) {}

bool ChunkCoord::operator==(const ChunkCoord &p) const {
    return x == p.x && y == p.y;
}

bool ChunkCoord::operator<(const ChunkCoord &p) const {
    if (p.x == x){return y < p.y;}
    else { return x < p.x; }
}

bool BlockCoord::operator==(const BlockCoord& coord) const {
    return (x == coord.x && y == coord.y && z == coord.z);
}

ChunkCoord BlockCoord::toChunkCoord() const {
	ChunkCoord r((short)(x >> 4), (short)(z >> 4));
    return r;
}
