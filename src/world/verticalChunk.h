//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_VERTICALCHUNK_H
#define MIICRAFT_VERTICALCHUNK_H

#include "coord.h"
#include "horizontalChunck.h"

struct ChunkPos{
    short x;
    short y;
    ChunkPos();
    ChunkPos(short x, short y);
    bool const operator==(const ChunkPos& p) const;
    bool const operator<(const ChunkPos& p) const;
};


class VerticalChunk {

private:
//    t_coord coords;
    std::vector<HorizontalChunk> chunks;
public:
    BlockType testType;
    VerticalChunk(BlockType t);


};


#endif //MIICRAFT_VERTICALCHUNK_H
