//
// Created by Martin on 09/02/2024.
//

#ifndef MIICRAFTTEST_TERGEN_H
#define MIICRAFTTEST_TERGEN_H

#include <queue>
#include "../block.h"
#include "../verticalChunk.h"


const int BIOME_COUNT = 19;

enum BiomeType : u16{
    Void = 0,
    Ocean,
    Beach,
    Savanna,
    Tundra,
    Desert,
    Plain,
    WoodedPlain,
    Hills,
    WoodedHills,
    StonyLand,
    Badlands,
    WoodedBadlands,
    Jungle,
    DarkForest,
    IcePeak,
    Taiga,
    StonyShore,
    RedBeach,
};

#define INIT_GENERATOR \
BlockCoord pos(block_x, 0, block_z);

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
for (; pos.y < 128-1; pos.y++){ \
    APPLY_BLOCK(BlockType::Air0);\
} APPLY_BLOCK(BlockType::Air);                          \
chunk.lightQueue.push(pos);

#define APPLY_BLOCK(BLOCK_TYPE) \
chunk.SetBlock(pos, BLOCK_TYPE);

class Tergen {
private:
public:

    constexpr static const float bottomLevel = 5; // absolu
    constexpr static const float seaLevel = 20; // relatif a bottomLevel
    constexpr static const float continentLevel = 5; // relatif a seaLevel
    constexpr static const float peakAmplitude = 25.;

    inline static void generateVoid(VerticalChunk& chunk, int block_x, int block_z, int){
        INIT_GENERATOR;
        APPLY_BEDROCK;

        for (; pos.y < 128; ++pos.y) {
            APPLY_BLOCK(BlockType::Air);
        }
    }

    inline static void generateDesert(VerticalChunk& chunk, int block_x, int block_z, int height){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, SandStone);
        APPLY_BLOCK(BlockType::Sand);
        pos.y++;
        APPLY_SKY;
    }

    inline static void generateTundra(VerticalChunk& chunk, int block_x, int block_z, int height){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassSnow);
        pos.y++;
        APPLY_SKY;
    }

    inline static void generateSavanna(VerticalChunk& chunk, int block_x, int block_z, int height){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassSavanna);
        pos.y++;
        APPLY_SKY;
    }

    inline static void generatePlain(VerticalChunk& chunk, int block_x, int block_z, int height){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassTemperate);
        pos.y++;
        APPLY_SKY;
    }

     inline static void generateForest(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassCold);
        pos.y++;
        APPLY_SKY;
    }

     inline static void generateBadLand(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

         for (; pos.y < seaLevel + bottomLevel; ++pos.y) {
             APPLY_BLOCK(Stone);
         }
         int bl = seaLevel + bottomLevel;
         for (; pos.y < height + 1; ++pos.y) {
             int badlandY = pos.y % 30;
             int dice = rand()%100;
             if (dice == 0) {badlandY++;}
             else if (dice == 1) {
                 badlandY--;
                 if (badlandY < 0) {badlandY = 30;}
             }

             APPLY_BLOCK(BADLANDS_STRATS[badlandY]);
         }
        pos.y++;
        APPLY_SKY;
    }

    static void generateDarkForest(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassDark);
        pos.y++;
        APPLY_SKY;
    }
    static void generateTaiga(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassTaiga);
        pos.y++;
        APPLY_SKY;
    }
     static void generateWindSwept(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Stone);
        APPLY_BLOCK(Stone);
        pos.y++;
        APPLY_SKY;
    }

     static void generateIcy(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Andesite, StdIce);
        APPLY_BLOCK(ClearIce);
        pos.y++;
        APPLY_SKY;
    }

     static void generateFlowerLand(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassSakura);
        pos.y++;
        APPLY_SKY;
    }

    static void generateJungle(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Dirt);
        APPLY_BLOCK(GrassJungle);
        pos.y++;
        APPLY_SKY;
    }
     static void generateOcean(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Sand);
        for (; pos.y < seaLevel + bottomLevel; ++pos.y) {
            APPLY_BLOCK(Water);
        }
        APPLY_SKY;
    }

     static void generateBeach(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Sand);
        APPLY_BLOCK(BlockType::Sand);
        pos.y++;
        APPLY_SKY;
    }

     static void generateStonyShore(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, Andesite);
        APPLY_BLOCK(BlockType::Gravel);
        pos.y++;
        APPLY_SKY;
    }

    static void generateRedBeach(VerticalChunk& chunk, int block_x, int block_z, int height ){
        INIT_GENERATOR;
        APPLY_BEDROCK;
        APPLY_BOTTOM;

        APPLY_CONTINENT(Stone, RedSand);
        APPLY_BLOCK(BlockType::RedSand);
        pos.y++;
        APPLY_SKY;
    }
    constexpr static const BlockType BADLANDS_STRATS[30] = {
            Clay,
            Clay,
            ClayRed,
            Clay,
            ClayBrown,
            ClayYellow,
            ClayYellow,
            Clay,
            Clay,
            ClayRed,
            ClayBrown,
            ClayWhite,
            ClayLightGray,
            Clay,
            ClayRed,
            ClayWhite,
            ClayLightGray,
            Clay,
            Clay,
            ClayLightGray,
            ClayWhite,
            ClayBrown,
            Clay,
            ClayRed,
            Clay,
            Clay,
            ClayRed,
            Clay,
            ClayGray,
            Clay,
    };
};
//void generateVoid( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generateDesert( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generatePlain( VerticalChunk* chunk, int block_x, int block_z, int height);


#endif //MIICRAFTTEST_TERGEN_H
