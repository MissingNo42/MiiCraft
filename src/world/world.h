//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_WORLD_H
#define MIICRAFT_WORLD_H

#include "verticalChunk.h"
 #include <map>
#include <queue>

#include <math.h>

class World{

private:
    std::map<t_pos2D, VerticalChunk*> loadedChunk;

public:
    World();
    ~World();


    std::map<t_pos2D, VerticalChunk*>& getLoadedChunk();

    Block getBlockAt(t_coord coord) ;
    void setBlockAt(t_coord coord, BlockType block, bool calculLight = true);

    static t_pos2D to_chunk_pos(t_coord& c);
    VerticalChunk& getChunkAt(t_pos2D pos);
    void addChunk(t_pos2D pos, VerticalChunk* chunk);

    void setNeighboors(t_pos2D coord, VerticalChunk *pChunk);

    void initLight(VerticalChunk* c, std::queue<t_coord>& lightQueue);

    void propagateLight(VerticalChunk *c, std::queue<t_coord>& lightQueue);

    std::queue<t_coord> lightQueue;
};





#endif //MIICRAFT_WORLD_H
