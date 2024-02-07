//
// Created by guill on 06/02/2024.
//

#include "game.h"
#include "PerlinWorldGenerator.h"


Game::Game() {

    gen = new PerlinWorldGenerator();
    gen->generateChunk(w, {0, 0});
//    gen->generateChunk(w, {1, 0});
//    gen->generateChunk(w, {0, 1});
//    gen->generateChunk(w, {1, 1});
//    gen->generateChunk(w, {0, -1});
//    gen->generateChunk(w, {-1, 0});
//    gen->generateChunk(w, {-1, -1});
//    gen->generateChunk(w, {1, -1});
//    gen->generateChunk(w, {-1, 1});
}

World &Game::getWorld() {
    return w;
}
