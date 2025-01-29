#pragma once

#include "Chunk.h"

class WorldGenerator {

public:

    virtual ~WorldGenerator() = default;

    virtual void generateChunk(Chunk& vc) = 0;
};
