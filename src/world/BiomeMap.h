//
// Created by Martin on 09/02/2024.
//

#ifndef MIICRAFTTEST_BIOMEMAP_H
#define MIICRAFTTEST_BIOMEMAP_H

#include "utils/FastNoiseLite.h"
#include "biome/BiomeTemplate.h"


class BiomeMap {
    FastNoiseLite biomeNoise;
public:
    BiomeMap();
    void InitNoise();
    BiomeType GetBiomeAtBlock(float x, float y);


};


#endif //MIICRAFTTEST_BIOMEMAP_H
