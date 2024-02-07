//
// Created by guill on 06/02/2024.
//

#ifndef WII_GAME_H
#define WII_GAME_H


#include "world.h"
#include "WorldGenerator.h"

class Game {
public:
    Game();
private:
    World w;
    WorldGenerator* gen;

};


#endif //WII_GAME_H
