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

public:
    BlockType getBlock;

private:
    std::vector<Block> blocks;
};

#endif //MIICRAFT_HORIZONTALCHUNCK_H
