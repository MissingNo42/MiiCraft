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


    Block getBlockAt(t_coord coord);
    void setBlockAt(t_coord coord, BlockType block);

    t_pos2D to_chunk_pos(t_coord& c);
    VerticalChunk& getChunkAt(t_pos2D pos);
    void addChunk(t_pos2D pos, VerticalChunk* chunk);
};





#endif //MIICRAFT_WORLD_H
