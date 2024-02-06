//
// Created by paulo on 06/02/2024.
//

#include "world.h"
#include <stdexcept>


World::World() {}

World::~World() {}

void World::generate(int block_rad) 
{
    for (int i = -block_rad + 1; i < block_rad; ++i) {
       for (int j = -block_rad + 1; j < block_rad; ++j) {
            VerticalChunk* vc = new VerticalChunk{j==2 ? BlockType::Grass : BlockType::Dirt};
            ChunkPos key(i, j);
            loadedChunk[key] = vc;
       }
   }
}

Block World::getBlockAt(t_coord coord) {
    ChunkPos pos(coord.x, coord.y);
    Block b;

    try{
        b.type = loadedChunk.at(pos)->testType;
    }
    catch (std::out_of_range)
    {
        b.type = BlockType::Air;
    }
    return b;
}

void World::setBlockAt(t_coord coord, BlockType block) {
    ChunkPos pos(coord.x/16, coord.z/16);
    loadedChunk[pos]->VC_SetBlock(coord, block);
}
