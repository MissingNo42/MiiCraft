//
// Created by Martin on 09/02/2024.
//

#ifndef MIICRAFTTEST_TERGEN_H
#define MIICRAFTTEST_TERGEN_H

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
t_coord pos(block_x, 0, block_z);

#define APPLY_BEDROCK \
APPLY_BLOCK(BlockType::Bedrock); \
pos.y ++; \
APPLY_BLOCK(BlockType::Bedrock); \
pos.y ++;

#define APPLY_BOTTOM \
for (; pos.y < Tergen::bottomLevel ; pos.y++) {\
    if (pos.y < 3) { APPLY_BLOCK( rand() %2 ? Bedrock : Stone);}\
    else { APPLY_BLOCK(BlockType::Stone);} }

#define APPLY_CONTINENT(STONE, SOIL) \
for (; pos.y < height; ++pos.y) {\
    if (pos.y < height /1.2) { APPLY_BLOCK(STONE);}\
    else { APPLY_BLOCK(SOIL);}}      \


#define APPLY_SKY \
for (; pos.y < VerticalChunk::CHUNK_HEIGHT-1; pos.y++){ \
    APPLY_BLOCK(BlockType::Air0);\
} APPLY_BLOCK(BlockType::Air);

#define APPLY_BLOCK(BLOCK_TYPE) \
chunk->VC_SetBlock(pos, BLOCK_TYPE);

class Tergen {
private:
public:

    constexpr static const float bottomLevel = 20; // absolu
    constexpr static const float seaLevel = 20; // relatif a bottomLevel
    constexpr static const float continentLevel = 5; // relatif a seaLevel
    constexpr static const float peakAmplitude = 25.;

    inline static void generateVoid(VerticalChunk *chunk, int block_x, int block_z, int){
        INIT_GENERATOR;
        APPLY_BEDROCK;

        for (; pos.y < VerticalChunk::CHUNK_HEIGHT; ++pos.y) {
            APPLY_BLOCK(BlockType::Air);
        }
    }

    inline static void generateDesert(VerticalChunk *chunk, int block_x, int block_z, int height){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, SandStone);
        APPLY_BLOCK(BlockType::GrassDark);
        pos.y++;
        APPLY_SKY;
    }

    inline static void generateTundra(VerticalChunk *chunk, int block_x, int block_z, int height){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassSnow);
        pos.y++;
        APPLY_SKY;
    }

    inline static void generateSavanna(VerticalChunk *chunk, int block_x, int block_z, int height){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassSavanna);
        pos.y++;
        APPLY_SKY;
    }

    inline static void generatePlain(VerticalChunk *chunk, int block_x, int block_z, int height){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassTemperate);
        pos.y++;
        APPLY_SKY;
    }

    inline static void generateOcean(VerticalChunk *chunk, int block_x, int block_z, int height){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_BLOCK(Gravel);
        for (; pos.y < seaLevel + bottomLevel; ++pos.y) {
            APPLY_BLOCK(Water);
        }
        APPLY_SKY;
    }

};
//void generateVoid( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generateDesert( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generatePlain( VerticalChunk* chunk, int block_x, int block_z, int height);


#endif //MIICRAFTTEST_TERGEN_H
