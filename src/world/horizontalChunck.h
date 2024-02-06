//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_HORIZONTALCHUNCK_H
#define MIICRAFT_HORIZONTALCHUNCK_H

#include "coord.h"
#include "block.h"
#include <vector>



class HorizontalChunk {
public:
    //Constantes 
    static const int CHUNK_WIDTH;

private:
    std::vector<Block> blocks;

public:


    HorizontalChunk();
    HorizontalChunk(int height);
    HorizontalChunk(int height, BlockType t);

    Block getBlock(int i);


};

#endif //MIICRAFT_HORIZONTALCHUNCK_H
