//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_BLOCK_H
#define MIICRAFT_BLOCK_H

enum BlockType {
    Air =0,
    Dirt,
    Grass,
    Stone
};

class Block {
public:
    BlockType type;
};

#endif //MIICRAFT_BLOCK_H

