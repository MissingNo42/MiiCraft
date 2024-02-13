//
// Created by Martin on 09/02/2024.
//

#include "system/Random.h"

int Random::globalSeed = 1234;

int Random::getSeed() {
    return globalSeed;
}
