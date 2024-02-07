//
// Created by guill on 06/02/2024.
//

#ifndef WII_GAME_H
#define WII_GAME_H


#include "world.h"
#include "WorldGenerator.h"

class Game {
protected:
    Game();
private:
    World w;
    WorldGenerator* gen;
    //Singleton design pattern
    static Game* instance;
    static Game* getInstance(){
        if(instance == nullptr){
            instance = new Game();
        }
        return instance;
    }

};


#endif //WII_GAME_H
