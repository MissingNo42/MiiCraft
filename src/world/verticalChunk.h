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
    bool operator==(const t_pos2D& p) const;
    bool operator<(const t_pos2D& p) const;
};

#define CHUNK_NORTH 1
#define CHUNK_SOUTH 3
#define CHUNK_EAST 0
#define CHUNK_WEST 2

class VerticalChunk {

private:
    static inline int cpt = 0;




public:
    int id;
    VerticalChunk* neighboors[4];
    Block blocks[16][128][16];
    VerticalChunk();

    static VerticalChunk* emptyChunk;



    const char* toString();


    void VC_SetBlock(t_coord coord, BlockType block);
    Block VC_GetBlock(t_coord coord);



    VerticalChunk* VC_GetNeighboors();
    void  VC_SetNeighboors(int indice, VerticalChunk* chunk);


    int static const CHUNK_WIDTH = 16;
    int static const CHUNK_LENGTH = 16;
    int static const CHUNK_HEIGHT = 128;

    void fillWithBedrock();

    VerticalChunk *VC_GetNeighboor(int indice);
};


#endif //MIICRAFT_VERTICALCHUNK_H
