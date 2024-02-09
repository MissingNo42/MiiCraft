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
    static inline Game* instance = nullptr;
public:

    static Game* getInstance(){
        if(instance == nullptr){
            instance = new Game();
        }
        return instance;
    }

    Game();
    World& getWorld();
	WorldGenerator& getWorldGenerator() {
		return *gen;
	}

    void requestChunk(t_pos2D pos);
};


#endif //WII_GAME_H
