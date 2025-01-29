#pragma once

#include <cmath>
#include <cstring>
#include <map>
#include <set>
#include "PerlinWorldGenerator.h"
#include "Chunk.h"

#define LOADED_CHUNKS 601 // XxX chunks + empty chunk
#define EMPTY_CHUNK 0


struct World {
	static int globalSeed;
	static PerlinWorldGenerator gen;
	static std::map<ChunkCoord, u16> loadedChunk;
	static std::set<ChunkCoord> savedChunk;
	//static Chunk chunkSlots[LOADED_CHUNKS];  // RAM1 chip cannot be used at compile time
	static Chunk * chunkSlots;
	static u16 usedSlots;

	static void Init() {
		try {
			chunkSlots = new Chunk[LOADED_CHUNKS];
			printf("Chunk allocated at: %p\r", static_cast<void *>(chunkSlots));
		}
		catch (const std::bad_alloc & e) {
			printf("Chunk allocation failed: %s\r", e.what());
			throw;
		}

		for (u16 i = 0; i < LOADED_CHUNKS; i++) chunkSlots[i].id = i;

		chunkSlots[EMPTY_CHUNK].loaded  = true;
		chunkSlots[EMPTY_CHUNK].recache = false;
		chunkSlots[EMPTY_CHUNK].fillWith(Bedrock); // fill with bedrock for collision
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

	static void setBlockAt(BlockCoord coord, Block block);

	static void setBlockTypeAt(BlockCoord coord, BlockType block);

	static Chunk & getChunkAt(ChunkCoord pos, bool generate = false);

	static void setNeighboors(Chunk & chunk);

	static Chunk & requestChunk(ChunkCoord pos);

	static void requestChunks(ChunkCoord pos, s32 range = 2);
};
