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
    DarkForest,
    IcePeak,
    Taiga
};

#define APPLY_BEDROCK \
APPLY_BLOCK(BlockType::Bedrock); \
pos.y = 1; \
APPLY_BLOCK(BlockType::Bedrock);

#define APPLY_BLOCK(BLOCK_TYPE) \
chunk->VC_SetBlock(pos, BLOCK_TYPE);


const int bottomLevel = 20; // absolu
const int seaLevel = 20; // relatif a bottomLevel
const int continentLevel = 15; // relatif a seaLevel
const int peakAmplitude = 20;

class BiomeGenerator {
private:
//    inline static int seaLevel = 45;
public:
    inline static void generateVoid(BiomeType, VerticalChunk *chunk, int block_x, int block_z, int, float){
        t_coord pos(block_x, 0, block_z);

        APPLY_BEDROCK;

        for (int i = 1; i < VerticalChunk::CHUNK_HEIGHT; ++i) {
            pos.y = i;
            APPLY_BLOCK(BlockType::Air);
        }
    }

    inline static void generateLand(BiomeType biome, VerticalChunk *chunk, int block_x, int block_z, int height, float noiseVariation = 0){
        t_coord pos(block_x, 0, block_z);

        APPLY_BEDROCK;
        int i = 2;
        for (i = 2; i <= height; ++i) {
            pos.y = i;
            if (i < 5) { APPLY_BLOCK( rand() %2 ? Bedrock : Stone);}
            else if (i < height /1.2) { APPLY_BLOCK(BlockType::Air);}
            else if (i < height) { APPLY_BLOCK(BlockType::Air);}
            else if (i == height) {
//                APPLY_BLOCK(BlockType::Sand);
                switch (biome) {
                    case Ocean:
                        APPLY_BLOCK(Water);
                        break;
                    case Beach:
                        APPLY_BLOCK(Sand);
                        break;
                    case Savanna:
                        APPLY_BLOCK(GrassSavanna);
                        break;
                    case Tundra:
                        APPLY_BLOCK(GrassSnow);
                        break;
                    case Desert:
                        APPLY_BLOCK(SandStone);
                        break;
                    case Plain:
                        APPLY_BLOCK(GrassTemperate);
                        break;
                    case WoodedPlain:
                        APPLY_BLOCK(WoodOak);
                        break;
                    case Hills:
                        APPLY_BLOCK(Andesite);
                        break;
                    case WoodedHills:
                        APPLY_BLOCK(WoodBirch);
                        break;
                    case Badlands:
                        APPLY_BLOCK(Clay);
                        break;
                    case WoodedBadlands:
                        APPLY_BLOCK(WoodAcacia);
                        break;
                    case Jungle:
                        APPLY_BLOCK(GrassJungle);
                        break;
                    case DarkForest:
                        APPLY_BLOCK(GrassDark);
                        break;
                    case IcePeak:
                        APPLY_BLOCK(BlueIce);
                        break;
                    case Taiga:
                        APPLY_BLOCK(WoodSpruce);
                        break;
                }
            }
        }
        if (biome == BiomeType::Ocean)
        {
            for(i=i; i<= seaLevel + bottomLevel; i++)
            {
                pos.y = i;
                APPLY_BLOCK(Water);
            }
        }
        for (i = i; i < VerticalChunk::CHUNK_HEIGHT; ++i) {
            pos.y = i;
            APPLY_BLOCK(BlockType::Air);
        }

    }


    /// Dolphin exclusive.
    inline static void generateBlockShowCase(BiomeType, VerticalChunk *chunk, int block_x, int block_z, int height, float)
    {
        t_coord pos(block_x, 0, block_z);
        BlockType id = BlockType::Air;

        if (block_x % 2 == 1 && block_z % 2 == 1)
        {
            for (int k = 0; k < VerticalChunk::CHUNK_HEIGHT; ++k) {
                chunk->VC_SetBlock(pos, id);
                u8 temp = (u8)id + 1;
                if (temp > BlockTypeCount)
                {
                    id = BlockType::Air;
                }
                else
                {
                    id = (BlockType)(temp);
                }
                pos.y++;
            }
        }
    }
};
//void generateVoid( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generateDesert( VerticalChunk* chunk, int block_x, int block_z, int height);
//void generatePlain( VerticalChunk* chunk, int block_x, int block_z, int height);


#endif //MIICRAFTTEST_BIOMEGENERATOR_H
