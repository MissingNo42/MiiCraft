//
// Created by paulo on 06/02/2024.
//

#include "block.h"

Block::Block(BlockType bt) : type(bt) {}

Block::Block(): Block(BlockType::Air) {}