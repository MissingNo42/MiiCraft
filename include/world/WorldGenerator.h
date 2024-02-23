//
// Created by guill on 06/02/2024.
//

#ifndef WII_WORLDGENERATOR_H
#define WII_WORLDGENERATOR_H


#include "verticalChunk.h"

class WorldGenerator {

public:

    virtual ~WorldGenerator() = default;

    virtual void generateChunk(VerticalChunk& vc) = 0;
};


#endif //WII_WORLDGENERATOR_H
