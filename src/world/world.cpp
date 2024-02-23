//
// Created by paulo on 06/02/2024.
//

#include "world.h"

PerlinWorldGenerator World::gen;
std::map<ChunkCoord, u16> World::loadedChunk;
std::set<ChunkCoord> World::savedChunk;
std::queue<BlockCoord> World::lightQueue;

//VerticalChunk World::chunkSlots[LOADED_CHUNKS];
VerticalChunk * World::chunkSlots = new VerticalChunk[LOADED_CHUNKS];
u16 World::usedSlots = 0;

BlockType World::getBlockAt(BlockCoord coord)  {
    ChunkCoord chunk_pos = coord.toChunkCoord();
	try {
        return chunkSlots[loadedChunk.at(chunk_pos)].GetBlock(coord);
	} catch (...) {
        return BlockType::Air;
    }
}

void World::setBlockAt(BlockCoord coord, BlockType block) {
    ChunkCoord chunk_pos = coord.toChunkCoord();
	VerticalChunk& c = chunkSlots[loadedChunk[chunk_pos]];
    c.SetBlock(coord, block);
    initLight(c, lightQueue);
    propagateLight(c, lightQueue);
}

VerticalChunk& World::getChunkAt(ChunkCoord pos, bool generate) {
	try {
        return chunkSlots[loadedChunk.at(pos)];
	} catch (...) {
		if (generate) {
			requestChunk(pos);
			return getChunkAt(pos, false);
		}
		return chunkSlots[EMPTY_CHUNK];
	}
}

//void World::addChunk(ChunkCoord pos, VerticalChunk& chunk) {
//    loadedChunk[pos] = chunk;
//}
void World::setBlockAt(t_coord coord, BlockType block, bool calculLight) {
    loadedChunk[to_chunk_pos(coord)]->VC_SetBlock(coord, block);

    if (calculLight) {
        initLight(loadedChunk[to_chunk_pos(coord)], loadedChunk[to_chunk_pos(coord)]->lightQueue);
        handleLightBlock(loadedChunk[to_chunk_pos(coord)]);
        propagateLight(loadedChunk[to_chunk_pos(coord)]);
    }


}


void World::setNeighboors(VerticalChunk& chunk) {
    ChunkCoord coord(chunk.coord.x + 1, chunk.coord.y);
	
    if(loadedChunk.find(coord) != loadedChunk.end()){
		u16 id = loadedChunk[coord];
        chunk.SetNeighboor(0, id);
		chunkSlots[id].SetNeighboor(2, chunk.id);
    }
	
    coord = ChunkCoord(chunk.coord.x - 1, chunk.coord.y);
    if(loadedChunk.find(coord) != loadedChunk.end()){
		u16 id = loadedChunk[coord];
        chunk.SetNeighboor(2, id);
        chunkSlots[id].SetNeighboor(0, chunk.id);
    }
	
    coord = ChunkCoord(chunk.coord.x, chunk.coord.y + 1);
    if(loadedChunk.find(coord) != loadedChunk.end()){
		u16 id = loadedChunk[coord];
        chunk.SetNeighboor(1, id);
        chunkSlots[id].SetNeighboor(3, chunk.id);
    }
	
    coord = ChunkCoord(chunk.coord.x, chunk.coord.y - 1);
    if(loadedChunk.find(coord) != loadedChunk.end()){
		u16 id = loadedChunk[coord];
        chunk.SetNeighboor(3, id);
        chunkSlots[id].SetNeighboor(1, chunk.id);
    }
}


void World::requestChunks(ChunkCoord pos) {
	const short range = 2;
    for(short x = pos.x - range; x <=  pos.x + range; x++){
        for(short y = pos.y - range; y <= pos.y + range; y++){
            ChunkCoord p(x, y);
			requestChunk(p);
        }
    }
}

void World::requestChunk(ChunkCoord pos) {
	if (loadedChunk.contains(pos)) return;
	if (savedChunk.contains(pos)) {
		//TODO
	} else {
		u16 slot = getFreeSlot();
		if (slot > 0) {
			VerticalChunk& vc = chunkSlots[slot];
			vc.coord = pos;
			vc.recache = 1;
			vc.loaded = 1;
			loadedChunk[pos] = slot;
			usedSlots++;
			setNeighboors(vc);
			vc.fillWith(WoodOak);
            gen.generateChunk(vc);
			vc.dirty = 0;
		} else {
			//TODO
		}
	}
}



//TODO : REMOVE THIS FUNCTION AND ADAPT IT IN THE WORLD GENERATOR SO THAT WE DONT ITERATE TWICE ON AIR BLOCKS
void World::initLight(VerticalChunk& c, std::queue<BlockCoord>& lightQueue) {

    for (int y = 127; y >= 0; y--) {
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {

                BlockCoord p = {x, y, z};
                if (c.GetBlock(p) < 16) {
                    if (y == 127) {
                        c.SetBlock(p, static_cast<BlockType>(15));
                        lightQueue.push(p);
                    } else {
                        c.SetBlock(p, static_cast<BlockType>(0));
                    }

                if(x == 15) {
                    auto newChunk = c->VC_GetNeighboor(CHUNK_EAST);
                    if(newChunk  != VerticalChunk::emptyChunk){
                        int LightLvl = newChunk->VC_GetBlock({0, y, z}).type;
                        if (LightLvl< 15) {
                            c->VC_SetBlock(p, static_cast<BlockType>( LightLvl - 1 <= 0 ? 0 : LightLvl - 1));
                            lightQueue.push(p);
                        }
                    }
                }
                if(x == 0) {
                    auto newChunk = c->VC_GetNeighboor(CHUNK_WEST);
                    if (newChunk != VerticalChunk::emptyChunk) {
                        int LightLvl = newChunk->VC_GetBlock({15, y, z}).type;
                        if ( LightLvl  < 15) {
                            c->VC_SetBlock(p, static_cast<BlockType>(LightLvl - 1 <= 0 ? 0 : LightLvl - 1));
                            lightQueue.push(p);
                        }
                    }
                }
                if(z == 15) {
                    auto newChunk = c->VC_GetNeighboor(CHUNK_NORTH);
                    if (newChunk != VerticalChunk::emptyChunk) {
                        int LightLvl = newChunk->VC_GetBlock({x, y, 0}).type;
                        if ( LightLvl < 15) {
                            c->VC_SetBlock(p, static_cast<BlockType>(LightLvl - 1 <= 0 ? 0 : LightLvl - 1 ));
                            lightQueue.push(p);
                        }
                    }
                }
                if(z == 0) {
                    auto newChunk = c->VC_GetNeighboor(CHUNK_SOUTH);
                    if (newChunk != VerticalChunk::emptyChunk) {
                        int LightLvl = newChunk->VC_GetBlock({x, y, 15}).type;
                        if ( LightLvl  < 15) {
                            c->VC_SetBlock(p, static_cast<BlockType>(LightLvl - 1 <= 0 ? 0 : LightLvl - 1));
                            lightQueue.push(p);
                        }
                    }
                }


                }

//                }
//                else if(c->GetBlock({x, y, z}) < 16 && c->GetBlock({x, y , z}) >= 0){
//                    c->SetBlock({x, y, z}, static_cast<BlockType>(0));
//
//                }

            }
        }
    }
    for(int y = 127; y >= 0; y--){
        for(int x = 0; x < 16; x++){
            for(int z = 0; z < 16; z++){
                t_coord p = {x, y, z};
                if(c->VC_GetBlock(p).type == RedstoneLamp){
                    if(p.x + 1 < 16){

                        if(c->VC_GetBlock({p.x + 1, p.y, p.z}).type < 15){
                            c->VC_SetBlock({p.x + 1, p.y, p.z}, Air);
                            c->blockLightQueue.push({p.x + 1, p.y, p.z});
                        }
                    }
                    if(p.x - 1 >= 0){


                        if(c->VC_GetBlock({p.x - 1, p.y, p.z}).type < 15){
                            c->VC_SetBlock({p.x - 1, p.y, p.z}, Air);
                            c->blockLightQueue.push({p.x - 1, p.y, p.z});
                        }
                    }
                    if(p.z + 1 < 16){

                        if(c->VC_GetBlock({p.x, p.y, p.z + 1}).type < 15){
                            c->VC_SetBlock({p.x, p.y, p.z + 1}, Air);
                            c->blockLightQueue.push({p.x, p.y, p.z + 1});
                        }
                    }
                    if(p.z - 1 >= 0){

                        if(c->VC_GetBlock({p.x, p.y, p.z - 1}).type < 15){
                            c->VC_SetBlock({p.x, p.y, p.z - 1}, Air);
                            c->blockLightQueue.push({p.x, p.y, p.z - 1});
                        }
                    }
                    if(p.y + 1 < 128){
                        if (c->VC_GetBlock({p.x, p.y + 1, p.z}).type < 15) {
                            c->VC_SetBlock({p.x, p.y + 1, p.z}, Air);
                            c->blockLightQueue.push({p.x, p.y + 1, p.z});
                        }
                    }
                    if(p.y - 1 >= 0){
                        if (c->VC_GetBlock({p.x, p.y - 1, p.z}).type < 15) {
                            c->VC_SetBlock({p.x, p.y - 1, p.z}, Air);
                            c->blockLightQueue.push({p.x, p.y - 1, p.z});
                        }
                    }
                }
            }
        }
    }
}


void World::propagateLight(VerticalChunk* c) {
    while(!c->lightQueue.empty()){
        t_coord p = c->lightQueue.front();

        c->lightQueue.pop();
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
                        c->lightQueue.push({p.x + 1, p.y, p.z});
                    }

                }
            }
            if(p.x+1>=16){
                auto newChunk = c->VC_GetNeighboor(CHUNK_EAST);
                if(newChunk  != VerticalChunk::emptyChunk){
                    if (newChunk->VC_GetBlock({0, p.y, p.z}).type < CurrentLightValue - 1 || newChunk->VC_GetBlock({0, p.y, p.z}).type > 15 ) {
                        c->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk->VC_GetBlock({0, p.y, p.z}).type <= 15){
                            newChunk->VC_SetBlock({0, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk->lightQueue.push({0 , p.y, p.z});
                            propagateLight(newChunk);
                        }
                    }
                }
            }
            if(p.x - 1 >= 0) {// left neighboor
                if (c->VC_GetBlock({p.x - 1, p.y, p.z}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x - 1, p.y, p.z}).type > 15) {
                    c->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c->VC_GetBlock({p.x - 1, p.y, p.z}).type <= 15){
                        c->VC_SetBlock({p.x - 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        c->lightQueue.push({p.x - 1, p.y, p.z});
                    }

                }
            }
            if(p.x-1<0){
                auto newChunk = c->VC_GetNeighboor(CHUNK_WEST);
                if(newChunk  !=  VerticalChunk::emptyChunk){
                    if (newChunk->VC_GetBlock({15, p.y, p.z}).type < CurrentLightValue - 1 || newChunk->VC_GetBlock({15, p.y, p.z}).type > 15 ) {
                        c->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk->VC_GetBlock({15, p.y, p.z}).type <= 15){
                            newChunk->VC_SetBlock({15, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk->lightQueue.push({15 , p.y, p.z});
                            propagateLight(newChunk);
                        }
                    }
                }
            }
            if(p.z + 1 < 16) {// front neighboor
                if (c->VC_GetBlock({p.x, p.y, p.z + 1}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x, p.y, p.z + 1}).type > 15 ) {
                    c->VC_SetBlock({p.x, p.y, p.z }, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c->VC_GetBlock({p.x, p.y, p.z + 1}).type <= 15){
                        c->VC_SetBlock({p.x, p.y, p.z + 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        c->lightQueue.push({p.x, p.y, p.z + 1});
                    }

                }
            }
            if(p.z+1>=16){
                auto newChunk = c->VC_GetNeighboor(CHUNK_NORTH);
                if(newChunk  !=  VerticalChunk::emptyChunk){
                    if (newChunk->VC_GetBlock({p.x, p.y, 0}).type < CurrentLightValue - 1 || newChunk->VC_GetBlock({p.x, p.y, 0}).type > 15 ) {
                        c->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk->VC_GetBlock({p.x, p.y, 0}).type <= 15){
                            newChunk->VC_SetBlock({p.x, p.y, 0}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk->lightQueue.push({p.x , p.y, 0});
                            propagateLight(newChunk);
                        }
                    }
                }
            }
            if(p.z - 1 >= 0) {// back neighboor
                if (c->VC_GetBlock({p.x, p.y, p.z - 1}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x, p.y, p.z - 1}).type > 15 ) {
                    c->VC_SetBlock({p.x, p.y, p.z }, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c->VC_GetBlock({p.x, p.y, p.z - 1}).type <= 15){
                        c->VC_SetBlock({p.x, p.y, p.z - 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        c->lightQueue.push({p.x, p.y, p.z - 1});
                    }

                }
            }
            if(p.z-1<0){
                auto newChunk = c->VC_GetNeighboor(CHUNK_SOUTH);
                if(newChunk  !=  VerticalChunk::emptyChunk){
                     if (newChunk->VC_GetBlock({p.x, p.y, 15}).type < CurrentLightValue - 1 || newChunk->VC_GetBlock({p.x, p.y, 15}).type > 15 ) {
                         c->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk->VC_GetBlock({p.x, p.y, 15}).type <= 15){
                            newChunk->VC_SetBlock({p.x, p.y, 15}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk->lightQueue.push({p.x , p.y, 15});
                            propagateLight(newChunk);
                        }
                    }
                }
            }
            if(p.y + 1 < 128) {// top neighboor
                if (c->VC_GetBlock({p.x, p.y + 1, p.z}).type < CurrentLightValue - 1 || c->VC_GetBlock({p.x, p.y + 1, p.z}).type > 15 ) {
                    c->VC_SetBlock({p.x, p.y , p.z} , static_cast<BlockType>(CurrentLightValue - 1));
                    if(c->VC_GetBlock({p.x, p.y + 1, p.z}).type <= 15){
                        c->VC_SetBlock({p.x, p.y + 1, p.z} , static_cast<BlockType>(CurrentLightValue - 1));
                        c->lightQueue.push({p.x, p.y + 1, p.z});

                    }
                }
            }
            if(p.y - 1 >= 0) {// bottom neighboor
                if (c->VC_GetBlock({p.x, p.y - 1, p.z}).type < CurrentLightValue || c->VC_GetBlock({p.x, p.y - 1, p.z}).type > 15) {
                    c->VC_SetBlock({p.x, p.y , p.z} , static_cast<BlockType>(CurrentLightValue));
                    if(c->VC_GetBlock({p.x, p.y - 1, p.z}).type <= 15){
                        c->VC_SetBlock({p.x, p.y - 1, p.z} , static_cast<BlockType>(CurrentLightValue));
                        c->lightQueue.push({p.x, p.y - 1, p.z});
                    }
                }
            }
        }
    }
}


void World::handleLightBlock(VerticalChunk *vc){
    while(!vc->blockLightQueue.empty()){
        t_coord p = vc->blockLightQueue.front();
        vc->blockLightQueue.pop();
        int CurrentLightValue = vc->VC_GetBlock(p).type;
        if(CurrentLightValue == RedstoneLamp){
            CurrentLightValue = 15;
        }
        if(CurrentLightValue <= 0 || CurrentLightValue > 15){
            continue;
        }
        else {
            if (p.x + 1 < 16) {// right neighboor
                if ((vc->VC_GetBlock({p.x + 1, p.y, p.z}).type < CurrentLightValue - 1 ))  {
//                    vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1 ));
                    if (vc->VC_GetBlock({p.x + 1, p.y, p.z}).type <= 15 ) {
                        vc->VC_SetBlock({p.x + 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc->blockLightQueue.push({p.x + 1, p.y, p.z});
                    }
                }
            }
            if(p.x+1>=16){
                auto newChunk = vc->VC_GetNeighboor(CHUNK_EAST);
                if(newChunk  != VerticalChunk::emptyChunk){
                    if (newChunk->VC_GetBlock({0, p.y, p.z}).type < CurrentLightValue - 1 || newChunk->VC_GetBlock({0, p.y, p.z}).type > 15 ) {
                        vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk->VC_GetBlock({0, p.y, p.z}).type <= 15){
                            newChunk->VC_SetBlock({0, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk->blockLightQueue.push({0 , p.y, p.z});
                            handleLightBlock(newChunk);
                        }
                    }
                }
            }
            if (p.x - 1 >= 0) {// left neighboor
                if ((vc->VC_GetBlock({p.x - 1, p.y, p.z}).type < CurrentLightValue - 1 ) ) {
//                    vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1 ));
                    if (vc->VC_GetBlock({p.x - 1, p.y, p.z}).type <= 15) {
                        vc->VC_SetBlock({p.x - 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc->blockLightQueue.push({p.x - 1, p.y, p.z});
                    }
                }
            }
            if(p.x-1<0){
                auto newChunk = vc->VC_GetNeighboor(CHUNK_WEST);
                if(newChunk  !=  VerticalChunk::emptyChunk){
                    if (newChunk->VC_GetBlock({15, p.y, p.z}).type < CurrentLightValue - 1 || newChunk->VC_GetBlock({15, p.y, p.z}).type > 15 ) {
                        vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk->VC_GetBlock({15, p.y, p.z}).type <= 15){
                            newChunk->VC_SetBlock({15, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk->blockLightQueue.push({15 , p.y, p.z});
                            handleLightBlock(newChunk);
                        }
                    }
                }
            }
            if (p.z + 1 < 16) {// front neighboor
                if ((vc->VC_GetBlock({p.x, p.y, p.z + 1}).type < CurrentLightValue - 1 )) {
//                    vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1 ));
                    if (vc->VC_GetBlock({p.x, p.y, p.z + 1}).type <= 15) {
                        vc->VC_SetBlock({p.x, p.y, p.z + 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc->blockLightQueue.push({p.x, p.y, p.z + 1});
                    }

                }
            }
            if(p.z+1>=16){
                auto newChunk = vc->VC_GetNeighboor(CHUNK_NORTH);
                if(newChunk  !=  VerticalChunk::emptyChunk){
                    if (newChunk->VC_GetBlock({p.x, p.y, 0}).type < CurrentLightValue - 1 || newChunk->VC_GetBlock({p.x, p.y, 0}).type > 15 ) {
                        vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk->VC_GetBlock({p.x, p.y, 0}).type <= 15){
                            newChunk->VC_SetBlock({p.x, p.y, 0}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk->blockLightQueue.push({p.x , p.y, 0});
                            handleLightBlock(newChunk);
                        }
                    }
                }
            }

            if (p.z - 1 >= 0) {// back neighboor
                if ((vc->VC_GetBlock({p.x, p.y, p.z - 1}).type < CurrentLightValue - 1 )) {
//                    vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                    if (vc->VC_GetBlock({p.x, p.y, p.z - 1}).type <= 15) {
                        vc->VC_SetBlock({p.x, p.y, p.z - 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc->blockLightQueue.push({p.x, p.y, p.z - 1});
                    }

                }
            }
            if(p.z-1<0){
                auto newChunk = vc->VC_GetNeighboor(CHUNK_SOUTH);
                if(newChunk  !=  VerticalChunk::emptyChunk){
                    if (newChunk->VC_GetBlock({p.x, p.y, 15}).type < CurrentLightValue - 1 || newChunk->VC_GetBlock({p.x, p.y, 15}).type > 15 ) {
                        vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk->VC_GetBlock({p.x, p.y, 15}).type <= 15){
                            newChunk->VC_SetBlock({p.x, p.y, 15}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk->blockLightQueue.push({p.x , p.y, 15});
                            handleLightBlock(newChunk);
                        }
                    }
                }
            }

            if (p.y + 1 < 128) {// top neighboor
                if ((vc->VC_GetBlock({p.x, p.y + 1, p.z}).type < CurrentLightValue - 1 )) {
//                    vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                    if (vc->VC_GetBlock({p.x, p.y + 1, p.z}).type <= 15) {
                        vc->VC_SetBlock({p.x, p.y + 1, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc->blockLightQueue.push({p.x, p.y + 1, p.z});
                    }

                }
            }
            if (p.y - 1 >= 0) {// bottom neighboor
                if ((vc->VC_GetBlock({p.x, p.y - 1, p.z}).type < CurrentLightValue - 1 )) {
//                    vc->VC_SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1  ));
                    if (vc->VC_GetBlock({p.x, p.y - 1, p.z}).type <= 15) {
                        vc->VC_SetBlock({p.x, p.y - 1, p.z}, static_cast<BlockType>(CurrentLightValue )); //TODO : check if we need to decrement this
                        vc->blockLightQueue.push({p.x, p.y - 1, p.z});
                    }

                }
            }
        }
    }
}

