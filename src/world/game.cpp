//
// Created by guill on 06/02/2024.
//

#include "game.h"
#include "PerlinWorldGenerator.h"


Game::Game() {
    VerticalChunk::emptyChunk->fillWithBedrock();
    gen = new PerlinWorldGenerator();
    /*
 t_pos2D pos;
 pos.x = 0;
 pos.y = 0;

 for (int x = 0 ; x<20; x++){
     for (int z = 0 ; z<20; z++){
         pos.x = x;
         pos.y = z;
         gen->generateChunk(w, pos);

     }
 }
 */


//    gen->generateChunk(w, {1, 0});
//    gen->generateChunk(w, {0, 1});
//    gen->generateChunk(w, {1, 1});
//    gen->generateChunk(w, {0, -1});
//    gen->generateChunk(w, {-1, 0});
//    gen->generateChunk(w, {-1, -1});
//    gen->generateChunk(w, {1, -1});
//    gen->generateChunk(w, {-1, 1});

}

void Game::requestChunk(t_pos2D pos) {
    for(int x = pos.x -2; x <  pos.x + 3; x++){
        for(int y = pos.y -2; y < pos.y + 3; y++){
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
