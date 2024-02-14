//
// Created by Martin on 09/02/2024.
//

#ifndef MIICRAFTTEST_BIOMEGENERATOR_H
#define MIICRAFTTEST_BIOMEGENERATOR_H

#include "../block.h"
#include "../verticalChunk.h"

enum BiomeType{
    Ocean = 0,
    Beach,
    Savanna,
    Tundra,
    Desert,
    Plain,
    WoodedPlain,
    Hills,
    WoodedHills,
    Badlands,
    WoodedBadlands,
    Jungle,
    Mushroom,
    IcePeak,
    Taiga
};

#define APPLY_BEDROCK \
APPLY_BLOCK(BlockType::Bedrock); \
pos.y = 1; \
APPLY_BLOCK(BlockType::Bedrock);

#define APPLY_BLOCK(BLOCK_TYPE) \
chunk->VC_SetBlock(pos, BLOCK_TYPE);

class BiomeGenerator {
private:
public:
    inline static void generateVoid(VerticalChunk *chunk, int block_x, int block_z, int){
        t_coord pos(block_x, 0, block_z);

        APPLY_BEDROCK;

        for (int i = 1; i < VerticalChunk::CHUNK_HEIGHT; ++i) {
            pos.y = i;
            APPLY_BLOCK(BlockType::Air);
        }
    }

    inline static void generateDesert(VerticalChunk *chunk, int block_x, int block_z, int height){
        t_coord pos(block_x, 0, block_z);

        APPLY_BEDROCK;

        for (int i = 2; i < VerticalChunk::CHUNK_HEIGHT; ++i) {
            pos.y = i;
            if (i < 5) { APPLY_BLOCK( rand() %2 ? Bedrock : Stone);}
            else if (i < height /1.2) { APPLY_BLOCK(BlockType::Stone);}
            else if (i < height) { APPLY_BLOCK(BlockType::Dirt);}
            else if (i == height) { APPLY_BLOCK(BlockType::Grass);}
            else {APPLY_BLOCK(BlockType::Air);}
        }
    }
};
const int seaLevel = 16;
//void generateVoid( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generateDesert( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generatePlain( VerticalChunk* chunk, int block_x, int block_z, int height);


#endif //MIICRAFTTEST_BIOMEGENERATOR_H
