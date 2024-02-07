//
// Created by guill on 06/02/2024.
//

#include "FlatWorldGenerator.h"

BlockType FlatWorldGenerator::guessBlockAt(const t_coord pos) {
    if(pos.y > 64){
        return BlockType::Air;
    }
    if(pos.y == 64){
        return BlockType::Grass;
    }
    if(pos.y < 64 && pos.y > 32 ){
        return BlockType::Dirt;
    }
    if(pos.y <= 32){
        return BlockType::Stone;
    }
    return BlockType::Air;
}

void FlatWorldGenerator::generateChunk(World w, const t_pos2D pos) {





}