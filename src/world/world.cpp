//
// Created by paulo on 06/02/2024.
//

#include "world.h"
#include <stdexcept>


World::World() {}

World::~World() {}
/*
void World::generate(int chunk_rad) 
{
    for (int i = -chunk_rad + 1; i < chunk_rad; ++i) {
        for (int j = -chunk_rad + 1; j < chunk_rad; ++j) {
            VerticalChunk* vc = new VerticalChunk{};
            t_pos2D key(i, j);
            loadedChunk[key] = vc;
        }
    }
}*/

t_pos2D World::to_chunk_pos(t_coord& c)
{
    if (c.x < 0) {c.x-= VerticalChunk::CHUNK_WIDTH;}
    if (c.z < 0) {c.z-= VerticalChunk::CHUNK_WIDTH;}
    return t_pos2D(c.x / VerticalChunk::CHUNK_WIDTH,
                   c.z / VerticalChunk::CHUNK_WIDTH);
}

Block World::getBlockAt(t_coord coord) {

    Block b;


    try{
        loadedChunk.at(to_chunk_pos(coord));
        b.type = BlockType::Dirt;
    }
    catch (std::out_of_range)
    {
        b.type = BlockType::Air;
    }
    return b;
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
