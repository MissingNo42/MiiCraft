//
// Created by Martin on 09/02/2024.
//

#ifndef MIICRAFTTEST_BIOMETEMPLATE_H
#define MIICRAFTTEST_BIOMETEMPLATE_H

#include "../block.h"

enum BiomeType{
    Hills = 0,
    Ocean = 1
};

class BiomeTemplate {
    static const int seaLevel;
public:
    virtual void generateBlockColumn(int landHeight, BiomeType biome, int x, int z) = 0;
};


#endif //MIICRAFTTEST_BIOMETEMPLATE_H
