//
// Created by guill on 06/02/2024.
//

#include "game.h"
#include "PerlinWorldGenerator.h"


Game::Game() {

    gen = new PerlinWorldGenerator();
    t_pos2D pos = {0, 0};
    for (int x = 0 ; x<3; x++){
        for (int z = 0 ; z<3; z++){
            pos.x = x;
            pos.y = z;
            gen->generateChunk(w, pos);
        }
    }


//    gen->generateChunk(w, {1, 0});
//    gen->generateChunk(w, {0, 1});
//    gen->generateChunk(w, {1, 1});
//    gen->generateChunk(w, {0, -1});
//    gen->generateChunk(w, {-1, 0});
//    gen->generateChunk(w, {-1, -1});
//    gen->generateChunk(w, {1, -1});
//    gen->generateChunk(w, {-1, 1});

    std::cout << w.getBlockAt({0, 0, 0}).toString() << std::endl;
    std::cout << w.getBlockAt({32, 0, 0}).toString() << std::endl;
   std::cout <<  w.getBlockAt({0, 0, 32}).toString() << std::endl;
    std::cout << w.getBlockAt({32, 0, 32}).toString() << std::endl;
    std::cout << w.getBlockAt({0, 0, 16}).toString() << std::endl;
    std::cout << w.getBlockAt({16, 0, 0}).toString() << std::endl;
    std::cout << w.getBlockAt({16, 0, 16}).toString() << std::endl;
    std::cout << w.getBlockAt({32, 0, 16}).toString() << std::endl;
    std::cout << w.getBlockAt({16, 0, 32}).toString() << std::endl;

}

World &Game::getWorld() {
    return w;
}
