//
// Created by guill on 06/02/2024.
//

#ifndef WII_COORD_H
#define WII_COORD_H

struct ChunkCoord {
    short x, y;
	
    explicit ChunkCoord(short x = 0, short y = 0);
    bool operator==(const ChunkCoord& p) const;
    bool operator<(const ChunkCoord& p) const;
};

struct BlockCoord {
    int x, y, z;
	
    BlockCoord(int x, int y, int z);
    bool operator==(const BlockCoord& coord) const;
	[[nodiscard]] ChunkCoord toChunkCoord() const;
};

#endif //WII_COORD_H
