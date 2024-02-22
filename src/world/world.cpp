//
// Created by paulo on 06/02/2024.
//

#include "world.h"

PerlinWorldGenerator World::gen;
std::map<ChunkCoord, u16> World::loadedChunk;
std::set<ChunkCoord> World::savedChunk;
std::queue<BlockCoord> World::lightQueue;
VerticalChunk World::chunkSlots[LOADED_CHUNKS];
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

VerticalChunk& World::getChunkAt(ChunkCoord pos) {
	try {
        return chunkSlots[loadedChunk.at(pos)];
	} catch (...) {
		//printf("Get chunk at %d %d FAILED\r", pos.x, pos.y);
		return chunkSlots[EMPTY_CHUNK];
	}
}

//void World::addChunk(ChunkCoord pos, VerticalChunk& chunk) {
//    loadedChunk[pos] = chunk;
//}

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
	printf(">>>\r");
	if (savedChunk.contains(pos)) {
		//TODO
	} else {
		u16 slot = getFreeSlot();
		
		printf("--- %d\r", slot);
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
			printf("Chunk %d %d generated %p\r", pos.x, pos.y, &getChunkAt(pos));
		} else {
			//TODO
		}
	}
	printf("<<<\r");
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
                }
//                }
//                else if(c->GetBlock({x, y, z}) < 16 && c->GetBlock({x, y , z}) >= 0){
//                    c->SetBlock({x, y, z}, static_cast<BlockType>(0));
//
//                }

            }
        }
    }
}

void World::propagateLight(VerticalChunk& c, std::queue<BlockCoord>& lightQueue) {
	//return;
    while(!lightQueue.empty()){
        BlockCoord p = lightQueue.front();
        lightQueue.pop();
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
                        lightQueue.push({p.x + 1, p.y, p.z});
                    }
                }
            }
            if(p.x - 1 >= 0) {// left neighboor
                if (c.GetBlock({p.x - 1, p.y, p.z}) < CurrentLightValue - 1 || c.GetBlock({p.x - 1, p.y, p.z}) > 15) {
                    c.GetBlock({p.x, p.y, p.z}), static_cast<BlockType>(CurrentLightValue - 1);
                    if(c.GetBlock({p.x - 1, p.y, p.z}) <= 15){
                        c.SetBlock({p.x - 1, p.y, p.z}, static_cast<BlockType>(CurrentLightValue - 1));
                        lightQueue.push({p.x - 1, p.y, p.z});
                    }
                }
            }
            if(p.z + 1 < 16) {// front neighboor
                if (c.GetBlock({p.x, p.y, p.z + 1}) < CurrentLightValue - 1 || c.GetBlock({p.x, p.y, p.z + 1}) > 15 ) {
                    c.SetBlock({p.x, p.y, p.z }, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c.GetBlock({p.x, p.y, p.z + 1}) <= 15){
                        c.SetBlock({p.x, p.y, p.z + 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        lightQueue.push({p.x, p.y, p.z + 1});
                    }
                }
            }
            if(p.z - 1 >= 0) {// back neighboor
                if (c.GetBlock({p.x, p.y, p.z - 1}) < CurrentLightValue - 1 || c.GetBlock({p.x, p.y, p.z - 1}) > 15 ) {
                    c.SetBlock({p.x, p.y, p.z }, static_cast<BlockType>(CurrentLightValue - 1));
                    if(c.GetBlock({p.x, p.y, p.z - 1}) <= 15){
                        c.SetBlock({p.x, p.y, p.z - 1}, static_cast<BlockType>(CurrentLightValue - 1));
                        lightQueue.push({p.x, p.y, p.z - 1});
                    }
                }
            }
            if(p.y + 1 < 128) {// top neighboor
                if (c.GetBlock({p.x, p.y + 1, p.z}) < CurrentLightValue - 1 || c.GetBlock({p.x, p.y + 1, p.z}) > 15 ) {
                    c.SetBlock({p.x, p.y , p.z} , static_cast<BlockType>(CurrentLightValue - 1));
                    if(c.GetBlock({p.x, p.y + 1, p.z}) <= 15){
                        c.SetBlock({p.x, p.y + 1, p.z} , static_cast<BlockType>(CurrentLightValue - 1));
                        lightQueue.push({p.x, p.y + 1, p.z});
                    }
                }
            }
            if(p.y - 1 >= 0) {// bottom neighboor
                if (c.GetBlock({p.x, p.y - 1, p.z}) < CurrentLightValue - 1 || c.GetBlock({p.x, p.y - 1, p.z}) > 15) {
                    c.SetBlock({p.x, p.y , p.z} , static_cast<BlockType>(CurrentLightValue));
                    if(c.GetBlock({p.x, p.y - 1, p.z}) <= 15){
                        c.SetBlock({p.x, p.y - 1, p.z} , static_cast<BlockType>(CurrentLightValue));
                        lightQueue.push({p.x, p.y - 1, p.z});
                    }
                }
            }
        }
    }
}


