//
// Created by paulo on 06/02/2024.
//

#include "world.h"
#include <stdexcept>

World::World() : loadedChunk() {
}

World::~World() {

    for (auto& pair : loadedChunk) {
        delete pair.second;
    }

    loadedChunk.clear();
}

t_pos2D World::to_chunk_pos(t_coord& c)
{
    t_pos2D res;
    if (c.x < 0)
        res.x = -( - (c.x - VerticalChunk::CHUNK_WIDTH +1) / VerticalChunk::CHUNK_WIDTH);
    else
        res.x = c.x / VerticalChunk::CHUNK_WIDTH;
    if (c.z < 0)
        res.y = -( - (c.z - VerticalChunk::CHUNK_WIDTH +1) / VerticalChunk::CHUNK_WIDTH);
    else
        res.y = c.z / VerticalChunk::CHUNK_WIDTH;
    return res;
}

Block World::getBlockAt(t_coord coord)  {
    t_pos2D chunk_pos = to_chunk_pos(coord);
    if (loadedChunk.find(chunk_pos) == loadedChunk.end()) {
        return {BlockType::Air};
    }
    return loadedChunk[chunk_pos]->VC_GetBlock(coord);
}

void World::setBlockAt(t_coord coord, BlockType block) {
    loadedChunk[to_chunk_pos(coord)]->VC_SetBlock(coord, block);
}

VerticalChunk& World::getChunkAt(t_pos2D pos) {
    return *loadedChunk.at(pos);
}

void World::addChunk(t_pos2D pos, VerticalChunk* chunk) {
    loadedChunk[pos] = chunk;
}

void World::setNeighboors(t_pos2D newcoord, VerticalChunk *pChunk) {
    t_pos2D coord(newcoord.x + 1, newcoord.y);
    if(loadedChunk.find(coord) != loadedChunk.end()){
        pChunk->VC_SetNeighboors(0, loadedChunk[coord]);
        loadedChunk[coord]->VC_SetNeighboors(2, pChunk);
    }
    coord = t_pos2D(newcoord.x - 1, newcoord.y);
    if(loadedChunk.find(coord) != loadedChunk.end()){
        pChunk->VC_SetNeighboors(2, loadedChunk[coord]);
        loadedChunk[coord]->VC_SetNeighboors(0, pChunk);
    }
    coord = t_pos2D(newcoord.x, newcoord.y + 1);
    if(loadedChunk.find(coord) != loadedChunk.end()){
        pChunk->VC_SetNeighboors(1, loadedChunk[coord]);
        loadedChunk[coord]->VC_SetNeighboors(3, pChunk);
    }
    coord = t_pos2D(newcoord.x, newcoord.y - 1);
    if(loadedChunk.find(coord) != loadedChunk.end()){
        pChunk->VC_SetNeighboors(3, loadedChunk[coord]);
        loadedChunk[coord]->VC_SetNeighboors(1, pChunk);
    }
}

std::map<t_pos2D, VerticalChunk *>& World::getLoadedChunk() {
    return loadedChunk;
}



