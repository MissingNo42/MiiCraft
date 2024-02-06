//
// Created by paulo on 06/02/2024.
//

#include "world.h"
#include <stdexcept>


World::World() {}

World::~World() {}

void World::generate(int chunk_rad) 
{
    for (int i = -chunk_rad + 1; i < chunk_rad; ++i) {
        for (int j = -chunk_rad + 1; j < chunk_rad; ++j) {
            VerticalChunk* vc = new VerticalChunk{};
            t_pos2D key(i, j);
            loadedChunk[key] = vc;
        }
    }
}

Block World::getBlockAt(t_coord coord) {
    // if (coord.x < 0) {coord.x-= HorizontalChunk::CHUNK_WIDTH;}
    // if (coord.y < 0) {coord.y-= HorizontalChunk::CHUNK_WIDTH;}
    t_pos2D pos((coord.x) / HorizontalChunk::CHUNK_WIDTH,
                coord.z / HorizontalChunk::CHUNK_WIDTH);
    Block b;
    // if (pos.x == 0 && pos.y == 0)
    // {
    //     printf("[]");
    // }
    // else
    // {
    //     printf("__");
    // }

    try{
        loadedChunk.at(pos);
        b.type = BlockType::Dirt;
    }
    catch (std::out_of_range)
    {
        b.type = BlockType::Air;
    }
    return b;
}

void World::setBlockAt(t_coord coord, BlockType block) {

}
