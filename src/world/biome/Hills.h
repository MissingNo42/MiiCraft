//
// Created by Martin on 09/02/2024.
//

#ifndef MIICRAFTTEST_HILLS_H
#define MIICRAFTTEST_HILLS_H


#include "BiomeTemplate.h"

class Hills : BiomeTemplate{
public:
    virtual void generateBlockColumn(int landHeight, BiomeType biome, int x, int z) override;

};


#endif //MIICRAFTTEST_HILLS_H
