//
// Created by guill on 06/02/2024.
//

#include "game.h"
#include "PerlinWorldGenerator.h"


Game::Game() {

    gen = new PerlinWorldGenerator();
    gen->generateChunk(w, {0, 0});
    gen->generateChunk(w, {1, 0});
    gen->generateChunk(w, {2, 0});
    gen->generateChunk(w, {0, 1});
    gen->generateChunk(w, {1, 1});
    gen->generateChunk(w, {2, 1});
    gen->generateChunk(w, {0, 2});
    gen->generateChunk(w, {1, 2});
    gen->generateChunk(w, {2, 2});



    std::cout << w.getBlockAt({0, 0, 0}).toString() << std::endl;
    std::cout << w.getBlockAt({16, 0, 0}).toString() << std::endl;
   std::cout <<  w.getBlockAt({32, 0, 0}).toString() << std::endl;
    std::cout << w.getBlockAt({0, 0, 16}).toString() << std::endl;
    std::cout << w.getBlockAt({16, 0, 16}).toString() << std::endl;
    std::cout << w.getBlockAt({32, 0, 16}).toString() << std::endl;
    std::cout << w.getBlockAt({0, 0, 32}).toString() << std::endl;
    std::cout << w.getBlockAt({16, 0, 32}).toString() << std::endl;
    std::cout << w.getBlockAt({32, 0, 32}).toString() << std::endl;

}

World &Game::getWorld() {
    return w;
}
