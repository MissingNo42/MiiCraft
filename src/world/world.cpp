//
// Created by paulo on 06/02/2024.
//

#include "world.h"
#include <stdexcept>


World::World() : loadedChunk() {
}

World::~World() {

    for (auto& pair : loadedChunk) {
        delete pair.second;
    }

    loadedChunk.clear();
}

t_pos2D World::to_chunk_pos(t_coord& c)
{
    if (c.x < 0) {c.x-= VerticalChunk::CHUNK_WIDTH;}
    if (c.z < 0) {c.z-= VerticalChunk::CHUNK_WIDTH;}
    return t_pos2D(c.x / VerticalChunk::CHUNK_WIDTH,
                   c.z / VerticalChunk::CHUNK_WIDTH);
}

Block World::getBlockAt(t_coord coord)  {
    t_pos2D chunk_pos = to_chunk_pos(coord);
    if (loadedChunk.find(chunk_pos) == loadedChunk.end()) {
        return {BlockType::Air};
    }
    return loadedChunk[chunk_pos]->VC_GetBlock(coord);

}

void World::setBlockAt(t_coord coord, BlockType block) {
    loadedChunk[to_chunk_pos(coord)]->VC_SetBlock(coord, block);
    initLight(loadedChunk[to_chunk_pos(coord)]);
}

VerticalChunk& World::getChunkAt(t_pos2D pos) {
    return *loadedChunk.at(pos);
}

void World::addChunk(t_pos2D pos, VerticalChunk* chunk) {
    loadedChunk[pos] = chunk;
}

void World::setNeighboors(t_pos2D newcoord, VerticalChunk *pChunk) {
    t_pos2D coord(newcoord.x + 1, newcoord.y);
    if(loadedChunk.find(coord) != loadedChunk.end()){
        pChunk->VC_SetNeighboors(0, loadedChunk[coord]);
        loadedChunk[coord]->VC_SetNeighboors(2, pChunk);
    }
    coord = t_pos2D(newcoord.x - 1, newcoord.y);
    if(loadedChunk.find(coord) != loadedChunk.end()){
        pChunk->VC_SetNeighboors(2, loadedChunk[coord]);
        loadedChunk[coord]->VC_SetNeighboors(0, pChunk);
    }
    coord = t_pos2D(newcoord.x, newcoord.y + 1);
    if(loadedChunk.find(coord) != loadedChunk.end()){
        pChunk->VC_SetNeighboors(1, loadedChunk[coord]);
        loadedChunk[coord]->VC_SetNeighboors(3, pChunk);
    }
    coord = t_pos2D(newcoord.x, newcoord.y - 1);
    if(loadedChunk.find(coord) != loadedChunk.end()){
        pChunk->VC_SetNeighboors(3, loadedChunk[coord]);
        loadedChunk[coord]->VC_SetNeighboors(1, pChunk);
    }
}

std::map<t_pos2D, VerticalChunk *>& World::getLoadedChunk() {
    return loadedChunk;
}




void World::initLight(VerticalChunk* c) {
    std::queue<t_coord> lightQueue;



    for(int y = 127; y >= 0; y--) {
        for(int x = 0; x <16; x++){
            for(int z = 0; z < 16; z++){

                t_coord p = {x, y, z};
                if (c->VC_GetBlock(p).type < 16) {
                    if(y==127){
                        c->VC_SetBlock(p, static_cast<BlockType>(15));
                        lightQueue.push(p);
                    }
                    else {
                        c->VC_SetBlock(p, static_cast<BlockType>(0));
                    }
                }
//                }
//                else if(c->VC_GetBlock({x, y, z}).type < 16 && c->VC_GetBlock({x, y , z}).type >= 0){
//                    c->VC_SetBlock({x, y, z}, static_cast<BlockType>(0));
//
//                }

            }
        }
    }

    while(!lightQueue.empty()){
        t_coord p = lightQueue.front();
        lightQueue.pop();
        int CurrentLightValue = c->VC_GetBlock(p).type;
        if(CurrentLightValue <= 0 || CurrentLightValue > 15){
            continue;
        }
        else
        {
            if(p.x + 1 < 16) {// right neighboor
                if (c->VC_GetBlock({p.x + 1, p.y, p.z}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x + 1, p.y, p.z}).type > 15 ) {
                    c->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c->VC_GetBlock({p.x + 1, p.y, p.z}).type <= 15){
                        c->VC_SetBlock({p.x + 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        lightQueue.push({p.x + 1, p.y, p.z});
                    }

                }
            }
            if(p.x - 1 >= 0) {// left neighboor
                if (c->VC_GetBlock({p.x - 1, p.y, p.z}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x - 1, p.y, p.z}).type > 15) {
                    c->VC_GetBlock({p.x, p.y, p.z}), static_cast<BlockType>(CurrentLightValue - 1);
                    if(c->VC_GetBlock({p.x - 1, p.y, p.z}).type <= 15){
                        c->VC_SetBlock({p.x - 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        lightQueue.push({p.x - 1, p.y, p.z});
                    }

                }
            }
            if(p.z + 1 < 16) {// front neighboor
                if (c->VC_GetBlock({p.x, p.y, p.z + 1}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x, p.y, p.z + 1}).type > 15 ) {
                    c->VC_SetBlock({p.x, p.y, p.z }, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c->VC_GetBlock({p.x, p.y, p.z + 1}).type <= 15){
                        c->VC_SetBlock({p.x, p.y, p.z + 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        lightQueue.push({p.x, p.y, p.z + 1});
                    }

                }
            }
            if(p.z - 1 >= 0) {// back neighboor
                if (c->VC_GetBlock({p.x, p.y, p.z - 1}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x, p.y, p.z - 1}).type > 15 ) {
                    c->VC_SetBlock({p.x, p.y, p.z }, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c->VC_GetBlock({p.x, p.y, p.z - 1}).type <= 15){
                        c->VC_SetBlock({p.x, p.y, p.z - 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        lightQueue.push({p.x, p.y, p.z - 1});
                    }

                }
            }
            if(p.y + 1 < 128) {// top neighboor
                if (c->VC_GetBlock({p.x, p.y + 1, p.z}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x, p.y + 1, p.z}).type > 15 ) {
                    c->VC_SetBlock({p.x, p.y , p.z} , static_cast<BlockType>(CurrentLightValue - 1));
                    if(c->VC_GetBlock({p.x, p.y + 1, p.z}).type <= 15){
                        c->VC_SetBlock({p.x, p.y + 1, p.z} , static_cast<BlockType>(CurrentLightValue - 1));
                        lightQueue.push({p.x, p.y + 1, p.z});

                    }

                }
            }
            if(p.y - 1 >= 0) {// bottom neighboor
                if (c->VC_GetBlock({p.x, p.y - 1, p.z}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x, p.y - 1, p.z}).type > 15) {
                    c->VC_SetBlock({p.x, p.y , p.z} , static_cast<BlockType>(CurrentLightValue));
                    if(c->VC_GetBlock({p.x, p.y - 1, p.z}).type <= 15){
                        c->VC_SetBlock({p.x, p.y - 1, p.z} , static_cast<BlockType>(CurrentLightValue));
                        lightQueue.push({p.x, p.y - 1, p.z});
                    }

                }
            }

        }


    }


}


