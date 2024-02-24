//
// Created by paulo on 06/02/2024.
//

#include "world/world.h"

PerlinWorldGenerator World::gen;
std::map<ChunkCoord, u16> World::loadedChunk;
std::set<ChunkCoord> World::savedChunk;

//VerticalChunk World::chunkSlots[LOADED_CHUNKS];
VerticalChunk * World::chunkSlots;
u16 World::usedSlots = 0;

BlockType World::getBlockAt(BlockCoord coord)  {
    ChunkCoord chunk_pos = coord.toChunkCoord();
	try {
        return chunkSlots[loadedChunk.at(chunk_pos)].GetBlock(coord);
	} catch (...) {
        return BlockType::Air;
    }
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
void World::setBlockAt(BlockCoord coord, BlockType block, bool calculLight) {
    ChunkCoord chunk_pos = coord.toChunkCoord();
	VerticalChunk& c = chunkSlots[loadedChunk[chunk_pos]];
    c.SetBlock(coord, block);
//    initLight(c);
//    propagateLight(c);

    if (calculLight) {
        initLight(c);
        handleLightBlock(c);
        propagateLight(c);
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
void World::initLight(VerticalChunk& c) {

    for (int y = 127; y >= 0; y--) {
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {

                BlockCoord p = {x, y, z};
                if (c.GetBlock(p) < 16) {
                    if (y == 127) {
                        c.SetBlock(p, static_cast<BlockType>(15));
                        c.lightQueue.push(p);
                    } else {
                        c.SetBlock(p, static_cast<BlockType>(0));
                    }

                if(x == 15) {
                    auto& newChunk = c.GetNeighboorChunk(CHUNK_EAST);
                    if(newChunk.id){
                        int LightLvl = newChunk.GetBlock({0, y, z});
                        if (LightLvl< 15) {
                            c.SetBlock(p, static_cast<BlockType>( LightLvl - 1 <= 0 ? 0 : LightLvl - 1));
                            c.lightQueue.push(p);
                        }
                    }
                }
                if(x == 0) {
                    auto& newChunk = c.GetNeighboorChunk(CHUNK_WEST);
                    if (newChunk.id) {
                        int LightLvl = newChunk.GetBlock({15, y, z});
                        if ( LightLvl  < 15) {
                            c.SetBlock(p, static_cast<BlockType>(LightLvl - 1 <= 0 ? 0 : LightLvl - 1));
                            c.lightQueue.push(p);
                        }
                    }
                }
                if(z == 15) {
                    auto& newChunk = c.GetNeighboorChunk(CHUNK_NORTH);
                    if (newChunk.id) {
                        int LightLvl = newChunk.GetBlock({x, y, 0});
                        if ( LightLvl < 15) {
                            c.SetBlock(p, static_cast<BlockType>(LightLvl - 1 <= 0 ? 0 : LightLvl - 1 ));
                            c.lightQueue.push(p);
                        }
                    }
                }
                if(z == 0) {
                    auto& newChunk = c.GetNeighboorChunk(CHUNK_SOUTH);
                    if (newChunk.id) {
                        int LightLvl = newChunk.GetBlock({x, y, 15});
                        if ( LightLvl  < 15) {
                            c.SetBlock(p, static_cast<BlockType>(LightLvl - 1 <= 0 ? 0 : LightLvl - 1));
                            c.lightQueue.push(p);
                        }
                    }
                }


                }

//                }
//                else if(c.GetBlock({x, y, z}) < 16 && c.GetBlock({x, y , z}) >= 0){
//                    c.SetBlock({x, y, z}, static_cast<BlockType>(0));
//
//                }

            }
        }
    }
    for(int y = 127; y >= 0; y--){
        for(int x = 0; x < 16; x++){
            for(int z = 0; z < 16; z++){
                BlockCoord p = {x, y, z};
                if(c.GetBlock(p) == RedstoneLamp){
                    if(p.x + 1 < 16){

                        if(c.GetBlock({p.x + 1, p.y, p.z}) < 15){
                            c.SetBlock({p.x + 1, p.y, p.z}, Air);
                            c.blockLightQueue.emplace(p.x + 1, p.y, p.z);
                        }
                    }
                    if(p.x - 1 >= 0){


                        if(c.GetBlock({p.x - 1, p.y, p.z}) < 15){
                            c.SetBlock({p.x - 1, p.y, p.z}, Air);
                            c.blockLightQueue.emplace(p.x - 1, p.y, p.z);
                        }
                    }
                    if(p.z + 1 < 16){

                        if(c.GetBlock({p.x, p.y, p.z + 1}) < 15){
                            c.SetBlock({p.x, p.y, p.z + 1}, Air);
                            c.blockLightQueue.emplace(p.x, p.y, p.z + 1);
                        }
                    }
                    if(p.z - 1 >= 0){

                        if(c.GetBlock({p.x, p.y, p.z - 1}) < 15){
                            c.SetBlock({p.x, p.y, p.z - 1}, Air);
                            c.blockLightQueue.emplace(p.x, p.y, p.z - 1);
                        }
                    }
                    if(p.y + 1 < 128){
                        if (c.GetBlock({p.x, p.y + 1, p.z}) < 15) {
                            c.SetBlock({p.x, p.y + 1, p.z}, Air);
                            c.blockLightQueue.emplace(p.x, p.y + 1, p.z);
                        }
                    }
                    if(p.y - 1 >= 0){
                        if (c.GetBlock({p.x, p.y - 1, p.z}) < 15) {
                            c.SetBlock({p.x, p.y - 1, p.z}, Air);
                            c.blockLightQueue.emplace(p.x, p.y - 1, p.z);
                        }
                    }
                }
            }
        }
    }
}


void World::propagateLight(VerticalChunk& c) {
    while(!c.lightQueue.empty()){
        BlockCoord p = c.lightQueue.front();

        c.lightQueue.pop();
        int CurrentLightValue = c.GetBlock(p);
        if(CurrentLightValue <= 0 || CurrentLightValue > 15){
            continue;
        }
        else
        {
            if(p.x + 1 < 16) {// right neighboor
                if (c.GetBlock({p.x + 1, p.y, p.z}) < CurrentLightValue - 1 || c.GetBlock({p.x + 1, p.y, p.z}) > 15 ) {
                    c.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c.GetBlock({p.x + 1, p.y, p.z}) <= 15){
                        c.SetBlock({p.x + 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        c.lightQueue.emplace(p.x + 1, p.y, p.z);
                    }

                }
            }
			
            if(p.x+1>=16){
                auto& newChunk = c.GetNeighboorChunk(CHUNK_EAST);
                if(newChunk.id){
                    if (newChunk.GetBlock({0, p.y, p.z}) < CurrentLightValue - 1 || newChunk.GetBlock({0, p.y, p.z}) > 15 ) {
                        c.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk.GetBlock({0, p.y, p.z}) <= 15){
                            newChunk.SetBlock({0, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk.lightQueue.emplace(0 , p.y, p.z);
                            propagateLight(newChunk);
                        }
                    }
                }
            }
			
            if(p.x - 1 >= 0) {// left neighboor
                if (c.GetBlock({p.x - 1, p.y, p.z}) < CurrentLightValue - 1 || c.GetBlock({p.x - 1, p.y, p.z}) > 15) {
                    c.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c.GetBlock({p.x - 1, p.y, p.z}) <= 15){
                        c.SetBlock({p.x - 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        c.lightQueue.emplace(p.x - 1, p.y, p.z);
                    }

                }
            }
			
            if(p.x-1<0){
                auto& newChunk = c.GetNeighboorChunk(CHUNK_WEST);
                if(newChunk.id){
                    if (newChunk.GetBlock({15, p.y, p.z}) < CurrentLightValue - 1 || newChunk.GetBlock({15, p.y, p.z}) > 15 ) {
                        c.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk.GetBlock({15, p.y, p.z}) <= 15){
                            newChunk.SetBlock({15, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk.lightQueue.emplace(15 , p.y, p.z);
                            propagateLight(newChunk);
                        }
                    }
                }
            }
			
            if(p.z + 1 < 16) {// front neighboor
                if (c.GetBlock({p.x, p.y, p.z + 1}) < CurrentLightValue - 1 || c.GetBlock({p.x, p.y, p.z + 1}) > 15 ) {
                    c.SetBlock({p.x, p.y, p.z }, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c.GetBlock({p.x, p.y, p.z + 1}) <= 15){
                        c.SetBlock({p.x, p.y, p.z + 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        c.lightQueue.emplace(p.x, p.y, p.z + 1);
                    }

                }
            }
			
            if(p.z+1>=16){
                auto& newChunk = c.GetNeighboorChunk(CHUNK_NORTH);
                if(newChunk.id){
                    if (newChunk.GetBlock({p.x, p.y, 0}) < CurrentLightValue - 1 || newChunk.GetBlock({p.x, p.y, 0}) > 15 ) {
                        c.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk.GetBlock({p.x, p.y, 0}) <= 15){
                            newChunk.SetBlock({p.x, p.y, 0}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk.lightQueue.emplace(p.x , p.y, 0);
                            propagateLight(newChunk);
                        }
                    }
                }
            }
			
            if(p.z - 1 >= 0) {// back neighboor
                if (c.GetBlock({p.x, p.y, p.z - 1}) < CurrentLightValue - 1 || c.GetBlock({p.x, p.y, p.z - 1}) > 15 ) {
                    c.SetBlock({p.x, p.y, p.z }, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c.GetBlock({p.x, p.y, p.z - 1}) <= 15){
                        c.SetBlock({p.x, p.y, p.z - 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        c.lightQueue.emplace(p.x, p.y, p.z - 1);
                    }

                }
            }
			
            if(p.z-1<0){
                auto& newChunk = c.GetNeighboorChunk(CHUNK_SOUTH);
                if(newChunk.id){
                     if (newChunk.GetBlock({p.x, p.y, 15}) < CurrentLightValue - 1 || newChunk.GetBlock({p.x, p.y, 15}) > 15 ) {
                         c.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk.GetBlock({p.x, p.y, 15}) <= 15){
                            newChunk.SetBlock({p.x, p.y, 15}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk.lightQueue.emplace(p.x , p.y, 15);
                            propagateLight(newChunk);
                        }
                    }
                }
            }
			
            if(p.y + 1 < 128) {// top neighboor
                if (c.GetBlock({p.x, p.y + 1, p.z}) < CurrentLightValue - 1 || c.GetBlock({p.x, p.y + 1, p.z}) > 15 ) {
                    c.SetBlock({p.x, p.y , p.z} , static_cast<BlockType>(CurrentLightValue - 1));
                    if(c.GetBlock({p.x, p.y + 1, p.z}) <= 15){
                        c.SetBlock({p.x, p.y + 1, p.z} , static_cast<BlockType>(CurrentLightValue - 1));
                        c.lightQueue.emplace(p.x, p.y + 1, p.z);

                    }
                }
            }
			
            if(p.y - 1 >= 0) {// bottom neighboor
                if (c.GetBlock({p.x, p.y - 1, p.z}) < CurrentLightValue || c.GetBlock({p.x, p.y - 1, p.z}) > 15) {
                    c.SetBlock({p.x, p.y , p.z} , static_cast<BlockType>(CurrentLightValue));
                    if(c.GetBlock({p.x, p.y - 1, p.z}) <= 15){
                        c.SetBlock({p.x, p.y - 1, p.z} , static_cast<BlockType>(CurrentLightValue));
                        c.lightQueue.emplace(p.x, p.y - 1, p.z);
                    }
                }
            }
        }
    }
}


void World::handleLightBlock(VerticalChunk& vc){
    while(!vc.blockLightQueue.empty()){
        BlockCoord p = vc.blockLightQueue.front();
        vc.blockLightQueue.pop();
        int CurrentLightValue = vc.GetBlock(p);
        if(CurrentLightValue == RedstoneLamp){
            CurrentLightValue = 15;
        }
        if(CurrentLightValue <= 0 || CurrentLightValue > 15){
            continue;
        }
        else {
            if (p.x + 1 < 16) {// right neighboor
                if ((vc.GetBlock({p.x + 1, p.y, p.z}) < CurrentLightValue - 1 ))  {
//                    vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1 ));
                    if (vc.GetBlock({p.x + 1, p.y, p.z}) <= 15 ) {
                        vc.SetBlock({p.x + 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc.blockLightQueue.emplace(p.x + 1, p.y, p.z);
                    }
                }
            }
            if(p.x+1>=16){
                auto& newChunk = vc.GetNeighboorChunk(CHUNK_EAST);
                if(newChunk.id){
                    if (newChunk.GetBlock({0, p.y, p.z}) < CurrentLightValue - 1 || newChunk.GetBlock({0, p.y, p.z}) > 15 ) {
                        vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk.GetBlock({0, p.y, p.z}) <= 15){
                            newChunk.SetBlock({0, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk.blockLightQueue.emplace(0 , p.y, p.z);
                            handleLightBlock(newChunk);
                        }
                    }
                }
            }
            if (p.x - 1 >= 0) {// left neighboor
                if ((vc.GetBlock({p.x - 1, p.y, p.z}) < CurrentLightValue - 1 ) ) {
//                    vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1 ));
                    if (vc.GetBlock({p.x - 1, p.y, p.z}) <= 15) {
                        vc.SetBlock({p.x - 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc.blockLightQueue.emplace(p.x - 1, p.y, p.z);
                    }
                }
            }
            if(p.x-1<0){
                auto& newChunk = vc.GetNeighboorChunk(CHUNK_WEST);
                if(newChunk.id){
                    if (newChunk.GetBlock({15, p.y, p.z}) < CurrentLightValue - 1 || newChunk.GetBlock({15, p.y, p.z}) > 15 ) {
                        vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk.GetBlock({15, p.y, p.z}) <= 15){
                            newChunk.SetBlock({15, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk.blockLightQueue.emplace(15 , p.y, p.z);
                            handleLightBlock(newChunk);
                        }
                    }
                }
            }
            if (p.z + 1 < 16) {// front neighboor
                if ((vc.GetBlock({p.x, p.y, p.z + 1}) < CurrentLightValue - 1 )) {
//                    vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1 ));
                    if (vc.GetBlock({p.x, p.y, p.z + 1}) <= 15) {
                        vc.SetBlock({p.x, p.y, p.z + 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc.blockLightQueue.emplace(p.x, p.y, p.z + 1);
                    }

                }
            }
            if(p.z+1>=16){
                auto& newChunk = vc.GetNeighboorChunk(CHUNK_NORTH);
                if(newChunk.id){
                    if (newChunk.GetBlock({p.x, p.y, 0}) < CurrentLightValue - 1 || newChunk.GetBlock({p.x, p.y, 0}) > 15 ) {
                        vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk.GetBlock({p.x, p.y, 0}) <= 15){
                            newChunk.SetBlock({p.x, p.y, 0}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk.blockLightQueue.emplace(p.x , p.y, 0);
                            handleLightBlock(newChunk);
                        }
                    }
                }
            }

            if (p.z - 1 >= 0) {// back neighboor
                if ((vc.GetBlock({p.x, p.y, p.z - 1}) < CurrentLightValue - 1 )) {
//                    vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                    if (vc.GetBlock({p.x, p.y, p.z - 1}) <= 15) {
                        vc.SetBlock({p.x, p.y, p.z - 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc.blockLightQueue.emplace(p.x, p.y, p.z - 1);
                    }

                }
            }
            if(p.z-1<0){
                auto& newChunk = vc.GetNeighboorChunk(CHUNK_SOUTH);
                if(newChunk.id){
                    if (newChunk.GetBlock({p.x, p.y, 15}) < CurrentLightValue - 1 || newChunk.GetBlock({p.x, p.y, 15}) > 15 ) {
                        vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        if(newChunk.GetBlock({p.x, p.y, 15}) <= 15){
                            newChunk.SetBlock({p.x, p.y, 15}, static_cast<BlockType>(CurrentLightValue - 1));
                            newChunk.blockLightQueue.emplace(p.x , p.y, 15);
                            handleLightBlock(newChunk);
                        }
                    }
                }
            }

            if (p.y + 1 < 128) {// top neighboor
                if ((vc.GetBlock({p.x, p.y + 1, p.z}) < CurrentLightValue - 1 )) {
//                    vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                    if (vc.GetBlock({p.x, p.y + 1, p.z}) <= 15) {
                        vc.SetBlock({p.x, p.y + 1, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        vc.blockLightQueue.emplace(p.x, p.y + 1, p.z);
                    }

                }
            }
            if (p.y - 1 >= 0) {// bottom neighboor
                if ((vc.GetBlock({p.x, p.y - 1, p.z}) < CurrentLightValue - 1 )) {
//                    vc.SetBlock({p.x, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1  ));
                    if (vc.GetBlock({p.x, p.y - 1, p.z}) <= 15) {
                        vc.SetBlock({p.x, p.y - 1, p.z}, static_cast<BlockType>(CurrentLightValue ));
                        vc.blockLightQueue.emplace(p.x, p.y - 1, p.z);
                    }
                }
            }
        }
    }
}

