//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_BLOCK_H
#define MIICRAFT_BLOCK_H
#include <iostream>
#include <gctypes.h>
#define SOLIDBLOCK 2
enum BlockType : u8 {
    Air =0,
    Leaves =1,
    Dirt =2,
    Grass =3,
    Stone =4,
    Bedrock =5,
    Log =6,

};

class Block {
public:
    BlockType type;
    Block(BlockType bt);
    Block();
    std::string toString() const;
};

#endif //MIICRAFT_BLOCK_H

