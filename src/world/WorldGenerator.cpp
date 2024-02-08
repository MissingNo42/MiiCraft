//
// Created by guill on 06/02/2024.
//

#include "WorldGenerator.h"
#include "block.h"

void WorldGenerator::buildTree(World &world, t_coord pos) {
    for (int y = 0; y < 5; y++) {
        world.setBlockAt({pos.x, pos.y + y, pos.z}, BlockType::Log);
    }
    for (int x = -2; x < 3; x++) {
        for (int z = -2; z < 3; z++) {
            world.setBlockAt({pos.x + x, pos.y + 5, pos.z + z}, BlockType::Leaves);
        }
    }
    for (int x = -1; x < 2; x++) {
        for (int z = -1; z < 2; z++) {
            world.setBlockAt({pos.x + x, pos.y + 6, pos.z + z}, BlockType::Leaves);
        }
    }
    world.setBlockAt({pos.x, pos.y + 7, pos.z}, BlockType::Leaves)
}