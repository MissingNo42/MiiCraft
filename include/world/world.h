//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_WORLD_H
#define MIICRAFT_WORLD_H

#include "verticalChunk.h"
#include "PerlinWorldGenerator.h"
#include <map>
#include <set>
#include <queue>
#include <cmath>

#define LOADED_CHUNKS 1501 // XxX chunks + empty chunk
#define EMPTY_CHUNK 0

class World {
private:

public:
    static int globalSeed;
    static PerlinWorldGenerator gen;
    static std::map<ChunkCoord, u16> loadedChunk;
    static std::set<ChunkCoord> savedChunk;
	//static VerticalChunk chunkSlots[LOADED_CHUNKS];
	static VerticalChunk * chunkSlots;
	static u16 usedSlots;
	
	static void Init() {
		chunkSlots = new VerticalChunk[LOADED_CHUNKS];
		chunkSlots[EMPTY_CHUNK].loaded = 1;
		chunkSlots[EMPTY_CHUNK].recache = 0;
		chunkSlots[EMPTY_CHUNK].fillWith(BlockType::Bedrock);
		for (int i = 0; i < LOADED_CHUNKS; i++) {
			chunkSlots[i].id = i;
		}
	}
	
	static u16 getFreeSlot() {
		for (u16 i = 1; i < LOADED_CHUNKS; i++) {
			if (chunkSlots[i].loaded == 0) return i;
		}
		return 0;
	}
	
    static BlockType getBlockAt(BlockCoord coord);
    static void setBlockAt(BlockCoord coord, BlockType block, bool calculLight = true);

    static VerticalChunk& getChunkAt(ChunkCoord pos, bool generate = false);
	
    static void setNeighboors(VerticalChunk& chunk);
    static void initLight(VerticalChunk& c);
    static void propagateLight(VerticalChunk& c);
	
    static void handleLightBlock(VerticalChunk& vc);
	
	static void requestChunk(ChunkCoord pos);
	static void requestChunks(ChunkCoord pos, short range = 2);
};

#endif //MIICRAFT_WORLD_H
