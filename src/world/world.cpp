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
    if (c.x < 0) {c.x-= VerticalChunk::CHUNK_WIDTH;}
    if (c.z < 0) {c.z-= VerticalChunk::CHUNK_WIDTH;}
    return t_pos2D(c.x / VerticalChunk::CHUNK_WIDTH,
                   c.z / VerticalChunk::CHUNK_WIDTH);
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

