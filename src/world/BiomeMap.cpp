//
// Created by Martin on 09/02/2024.
//

#include "BiomeMap.h"
#include "../system/Random.h"

BiomeType BiomeMap::GetBiomeAtBlock(float x, float y) {
    float biomeValue = biomeNoise.GetNoise(x, y);
    if (biomeValue <= 0.1)
        {return Ocean;}
    else
        {return Hills;}
    return Hills;
}

BiomeMap::BiomeMap() {
    InitNoise();
}

void BiomeMap::InitNoise() {
    biomeNoise.SetFrequency(.01);
    biomeNoise.SetSeed(Random::getSeed());//  Game::getInstance()->getSeed());

    biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    biomeNoise.SetFractalType(FastNoiseLite::FractalType_None);
    biomeNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Hybrid);
    biomeNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
    biomeNoise.SetCellularJitter(1.);
}
