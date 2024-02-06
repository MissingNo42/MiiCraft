//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_WORLD_H
#define MIICRAFT_WORLD_H

#include "verticalChunk.h"

class World{

public:
    BlockType getBlockAt(t_coord coord);
    void setBlockAt(t_coord coord, BlockType block);
};


#endif //MIICRAFT_WORLD_H
