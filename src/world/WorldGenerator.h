//
// Created by guill on 06/02/2024.
//

#ifndef WII_WORLDGENERATOR_H
#define WII_WORLDGENERATOR_H


#include "block.h"
#include "coord.h"
#include "verticalChunk.h"
#include "world.h"

class WorldGenerator {

public:

    virtual ~WorldGenerator() = default;

    virtual void generateChunk(World, const t_pos2D ) = 0;


};


#endif //WII_WORLDGENERATOR_H
