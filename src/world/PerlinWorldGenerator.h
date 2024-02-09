//
// Created by guill on 07/02/2024.
//

#ifndef WII_PERLINWORLDGENERATOR_H
#define WII_PERLINWORLDGENERATOR_H


#include "WorldGenerator.h"
#include "utils/FastNoiseLite.h"

class PerlinWorldGenerator : public WorldGenerator {
private:
    FastNoiseLite noise;

public:
    void generateChunk(World&, const t_pos2D) override;

    void generateNoise();

    int noiseToInteger(float floatValue);

    PerlinWorldGenerator();

    void buildTree(t_coord pos, VerticalChunk* vc);
};


#endif //WII_PERLINWORLDGENERATOR_H
