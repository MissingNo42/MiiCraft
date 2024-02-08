//
// Created by guill on 06/02/2024.
//

#include "game.h"
#include "PerlinWorldGenerator.h"


Game::Game() {

    gen = new PerlinWorldGenerator();
}

World &Game::getWorld() {
    return w;
}
