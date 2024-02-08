//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_BLOCK_H
#define MIICRAFT_BLOCK_H
#include <iostream>
#include <gctypes.h>

enum BlockType : u8 {
    Log =5,
    Leaves =6,
    Air =0,
    Dirt =1,
    Grass =2,
    Stone =3,
    Bedrock =4
};

class Block {
public:
    BlockType type;
    Block(BlockType bt);
    Block();
    std::string toString() const;
};

#endif //MIICRAFT_BLOCK_H

