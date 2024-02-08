//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_VERTICALCHUNK_H
#define MIICRAFT_VERTICALCHUNK_H

#include "coord.h"
#include "block.h"
#include <cstdint>



struct t_pos2D{
    short x;
    short y;
    t_pos2D();
    t_pos2D(short x, short y);
    bool const operator==(const t_pos2D& p) const;
    bool const operator<(const t_pos2D& p) const;
};


class VerticalChunk {

private:

    Block blocks[16][128][16];

public:


    explicit VerticalChunk(Block b[16][128][16]);

    const char* toString();

    void VC_SetBlock(t_coord coord, BlockType block);
    Block VC_GetBlock(t_coord coord);

    int static const CHUNK_WIDTH = 16;
    int static const CHUNK_LENGTH = 16;
    int static const CHUNK_HEIGHT = 128;
};


#endif //MIICRAFT_VERTICALCHUNK_H
