//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_HORIZONTALCHUNCK_H
#define MIICRAFT_HORIZONTALCHUNCK_H

#include "coord.h"
#include "block.h"
#include <vector>



class HorizontalChunk {

private:
    std::vector<Block> blocks;

public:

    static const int CHUNK_HEIGHT = 16;
    static const int CHUNK_WIDTH = 16;


    HorizontalChunk();
    explicit HorizontalChunk(int height);
    HorizontalChunk(int height, Block t);
    explicit HorizontalChunk( std::vector<Block> blocks);

    Block getBlock(int i);
    void HC_SetBlock(t_coord coord, BlockType block);


};

#endif //MIICRAFT_HORIZONTALCHUNCK_H
