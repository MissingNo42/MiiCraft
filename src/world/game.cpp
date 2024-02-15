//
// Created by guill on 06/02/2024.
//

#include "game.h"
#include "PerlinWorldGenerator.h"


Game::Game() {
    VerticalChunk::emptyChunk->fillWithBedrock();
    gen = new PerlinWorldGenerator();
}





//    gen->generateChunk(w, {1, 0});
//    gen->generateChunk(w, {0, 1});
//    gen->generateChunk(w, {1, 1});
//    gen->generateChunk(w, {0, -1});
//    gen->generateChunk(w, {-1, 0});
//    gen->generateChunk(w, {-1, -1});
//    gen->generateChunk(w, {1, -1});
//    gen->generateChunk(w, {-1, 1});

void Game::requestChunk(t_pos2D pos) {
    for(int x = pos.x -4; x <  pos.x + 5; x++){
        for(int y = pos.y -4; y < pos.y + 5; y++){
            t_pos2D p;
            p.x = x;
            p.y = y;

            if (w.getLoadedChunk().find(p) == w.getLoadedChunk().end() ) {
                gen->generateChunk(w, p);
           }
        }
    }
}

World& Game::getWorld() {
    return w;
}


