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

#define INIT_GENERATOR \
t_coord pos(block_x, 0, block_z); \
int i = 0;

#define APPLY_BEDROCK \
APPLY_BLOCK(BlockType::Bedrock); \
pos.y = i++; \
APPLY_BLOCK(BlockType::Bedrock);

//#define APPLY_BOTTOM \
//for (i++; i < BiomeGenerator::bottomLevel ; ++i) {
//    pos.y = i;
//    if (i < 3) { APPLY_BLOCK( rand() %2 ? Bedrock : Stone);}
//    else if (i < height /1.2) { APPLY_BLOCK(BlockType::Stone);}
//    else if (i < height) { APPLY_BLOCK(BlockType::Dirt);}
//    else if (i == height) { APPLY_BLOCK(BlockType::Sand);}
//    else {APPLY_BLOCK(BlockType::Air);}
//}
#define APPLY_BLOCK(BLOCK_TYPE) \
chunk->VC_SetBlock(pos, BLOCK_TYPE);

class BiomeGenerator {
private:
public:

    constexpr static const float bottomLevel = 20; // absolu
    constexpr static const float seaLevel = 20; // relatif a bottomLevel
    constexpr static const float continentLevel = 5; // relatif a seaLevel
    constexpr static const float peakAmplitude = 30. / 2.;

    inline static void generateVoid(VerticalChunk *chunk, int block_x, int block_z, int){
        INIT_GENERATOR;
        APPLY_BEDROCK;

        for (i = 1; i < VerticalChunk::CHUNK_HEIGHT; ++i) {
            pos.y = i;
            APPLY_BLOCK(BlockType::Air);
        }
    }

    inline static void generateDesert(VerticalChunk *chunk, int block_x, int block_z, int height, std::queue<t_coord>& lightQueue){
        INIT_GENERATOR;
        APPLY_BEDROCK;

        for (i = 2; i < VerticalChunk::CHUNK_HEIGHT; ++i) {
            pos.y = i;
            if (i < 5) { APPLY_BLOCK( rand() %2 ? Bedrock : Stone);}
            else if (i < height /1.2) { APPLY_BLOCK(BlockType::Stone);}
            else if (i < height) { APPLY_BLOCK(BlockType::Dirt);}
            else if (i == height) { APPLY_BLOCK(BlockType::Sand);}
            else if (i==VerticalChunk::CHUNK_HEIGHT-1){APPLY_BLOCK(BlockType::Air); lightQueue.push({block_x, i, block_z});}
            else { APPLY_BLOCK(BlockType::Air0);}
        }
    }
};
const int seaLevel = 16;
//void generateVoid( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generateDesert( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generatePlain( VerticalChunk* chunk, int block_x, int block_z, int height);


#endif //MIICRAFTTEST_BIOMEGENERATOR_H
