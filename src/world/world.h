//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_WORLD_H
#define MIICRAFT_WORLD_H

#include "verticalChunk.h"
 #include <map>

class World{
private:
    std::map<t_pos2D, VerticalChunk*> loadedChunk;
    
public:
    World();
    ~World();
    void generate(int chunk_rad);
    Block getBlockAt(t_coord coord);
    void setBlockAt(t_coord coord, BlockType block);
};


#endif //MIICRAFT_WORLD_H
