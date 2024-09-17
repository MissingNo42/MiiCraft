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

#define LOADED_CHUNKS 601 // XxX chunks + empty chunk
#define EMPTY_CHUNK 0

struct World {
	
    static int globalSeed;
    static PerlinWorldGenerator gen;
    static std::map<ChunkCoord, u16> loadedChunk;
    static std::set<ChunkCoord> savedChunk;
	//static VerticalChunk chunkSlots[LOADED_CHUNKS];  // RAM1 chip cannot be used at compile time
	static VerticalChunk * chunkSlots;
	static u16 usedSlots;
	
	static void Init() {
		chunkSlots = new VerticalChunk[LOADED_CHUNKS];
		for (u16 i = 0; i < LOADED_CHUNKS; i++) {
			chunkSlots[i].id = i;
			printf("Chunk %d, N %d %d %d %d\r\n",
				   i,
				   chunkSlots[i].neighboors[0],
				   chunkSlots[i].neighboors[1],
				   chunkSlots[i].neighboors[2],
				   chunkSlots[i].neighboors[3]);
		}
		chunkSlots[EMPTY_CHUNK].loaded = 1;
		chunkSlots[EMPTY_CHUNK].recache = 0;
		chunkSlots[EMPTY_CHUNK].fillWith(BlockType::Bedrock);
	}
	
	static u16 getFreeSlot() {
		for (u16 i = 1; i < LOADED_CHUNKS; i++) {
			if (chunkSlots[i].loaded == 0) return i;
		}
		return 0;
	}
	
	// Must not be used for massive R/W, get the working chunk first and R/W directly on it
    static Block getBlockAt(BlockCoord coord);
    static BlockType getBlockTypeAt(BlockCoord coord);
    static void setBlockAt(BlockCoord coord, Block block, bool calculLight = true);
    static void setBlockTypeAt(BlockCoord coord, BlockType block, bool calculLight = true);

    static VerticalChunk& getChunkAt(ChunkCoord pos, bool generate = false);
	
    static void setNeighboors(VerticalChunk& chunk);
    static void initLight(VerticalChunk& c);
    static void propagateLight(VerticalChunk& c);
	
    static void handleLightBlock(VerticalChunk& vc);
	
	static VerticalChunk& requestChunk(ChunkCoord pos);
	static void requestChunks(ChunkCoord pos, short range = 2);
};

#endif //MIICRAFT_WORLD_H
