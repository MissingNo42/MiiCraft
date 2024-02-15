//
// Created by guill on 07/02/2024.
//

#ifndef WII_PERLINWORLDGENERATOR_H
#define WII_PERLINWORLDGENERATOR_H


#include "WorldGenerator.h"
#include "utils/FastNoiseLite.h"
#include "biome/BiomeGenerator.h"
#include "../system/Random.h"
#include <fstream>
#include <iostream>
#include <algorithm>

#define APPLY_BLOCK(BLOCK_TYPE) \
chunk->VC_SetBlock(pos, BLOCK_TYPE);


class PerlinWorldGenerator : public WorldGenerator {
private:
//    FastNoiseLite noise;

    FastNoiseLite noiseErosion;
    FastNoiseLite noiseTemperature;
    FastNoiseLite noiseHumidity;
    FastNoiseLite noiseAltitude;
    FastNoiseLite noiseContinental;

    void buildTree(t_coord pos, VerticalChunk* vc);
public:
    PerlinWorldGenerator();
    void initNoise();

    BiomeType guessBiome(float ero, float temp, float hum, float cont, int height);
    void generateChunk(World&, const t_pos2D) override;
};


#endif //WII_PERLINWORLDGENERATOR_H
