//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_BLOCK_H
#define MIICRAFT_BLOCK_H
#include <iostream>

enum BlockType : char {
    Air =0,
    Dirt =1,
    Grass =2,
    Stone =3,
    Bedrock =4,
    Log =5,
    TopLog =6,
    Leaves =7
};

class Block {
public:
    BlockType type;
    Block(BlockType bt);
    Block();
};

#endif //MIICRAFT_BLOCK_H

