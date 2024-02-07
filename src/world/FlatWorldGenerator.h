//
// Created by guill on 06/02/2024.
//

#ifndef WII_FLATWORLDGENERATOR_H
#define WII_FLATWORLDGENERATOR_H


#include "WorldGenerator.h"
#include "world.h"

class FlatWorldGenerator : public WorldGenerator{

public:

    void generateChunk(World, const t_pos2D) override;




};


#endif //WII_FLATWORLDGENERATOR_H
