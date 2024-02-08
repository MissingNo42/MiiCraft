//
// Created by guill on 06/02/2024.
//

#ifndef WII_GAME_H
#define WII_GAME_H


#include "world.h"
#include "WorldGenerator.h"

class Game {
protected:
private:
    World w;
    WorldGenerator* gen;
    //Singleton design pattern
    static Game* instance;
public:
//    static Game* getInstance(){
//        if(instance == nullptr){
//            instance = new Game();
//        }
//        return instance;
//    }

    Game();
    World& getWorld();
	WorldGenerator& getWorldGenerator() {
		return *gen;
	}
};


#endif //WII_GAME_H
