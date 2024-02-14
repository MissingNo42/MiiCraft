//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_BLOCK_H
#define MIICRAFT_BLOCK_H
#include <iostream>
#include <gctypes.h>
#define SOLIDBLOCK 2
enum BlockType : u8 {

    Air0 =      0,
    Air1 =      1,
    Air2 =      2,
    Air3 =      3,
    Air4 =      4,
    Air5 =      5,
    Air6 =      6,
    Air7 =      7,
    Air8 =      8,
    Air9 =      9,
    Air10 =     10,
    Air11 =     11,
    Air12 =     12,
    Air13 =     13,
    Air14 =     14,
    Air =       15,
    Leaves =    16,
    Dirt =      17,
    Grass =     18,
    Stone =     19,
    Bedrock =   20,
    Log =       21,

};

class Block {
public:
    BlockType type;
    Block(BlockType bt);
    Block();
    std::string toString() const;
};

#endif //MIICRAFT_BLOCK_H

