//
// Created by paulo on 06/02/2024.
//

#include "block.h"

Block::Block(BlockType bt) : type(bt) {}

Block::Block(): Block(BlockType::Air) {}

std::string Block::toString() const {
    switch (type) {
        case BlockType::Air:
            return "Air";
        case BlockType::Dirt:
            return "Dirt";
        case BlockType::Grass:
            return "Grass";
        case BlockType::Stone:
            return "Stone";
        case BlockType::Bedrock:
            return "Bedrock";
        case BlockType::Log:
            return "Log";
        case BlockType::Leaves:
            return "Leaves";
        default:
            return "inconnu au bataillon";
    }
}
