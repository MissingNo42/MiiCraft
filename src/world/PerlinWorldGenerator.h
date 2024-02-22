//
// Created by guill on 07/02/2024.
//

#ifndef WII_PERLINWORLDGENERATOR_H
#define WII_PERLINWORLDGENERATOR_H


#include "utils/FastNoiseLite.h"
#include "verticalChunk.h"
#include "world/biome/Tergen.h"
#include "WorldGenerator.h"
#include <fstream>
#include <queue>

#include <iostream>
#include <algorithm>


class PerlinWorldGenerator : public WorldGenerator {
private:
//    FastNoiseLite noise;

    FastNoiseLite noiseErosion;
    FastNoiseLite noiseTemperature;
    FastNoiseLite noiseHumidity;
    FastNoiseLite noiseAltitude;
    FastNoiseLite noiseContinental;
	
	void buildTree(BlockCoord pos, VerticalChunk& vc);
public:
    PerlinWorldGenerator();
    void initNoise();

    BiomeType guessBiome(float ero, float temp, float hum, float cont, int height);
    void generateChunk(VerticalChunk& vc) override;
	
};


#endif //WII_PERLINWORLDGENERATOR_H
