//
// Created by Romain on 19/02/2024.
//

#ifndef MIICRAFTTEST_CACHEMANAGER_H
#define MIICRAFTTEST_CACHEMANAGER_H


#include <gctypes.h>
#include <set>
#include <queue>
#include "world/verticalChunk.h"
#include "renderer.h"
#include "cache.h"
#include "player.h"

#define MAX_RENDER_DIST 8

class ChunkCache {
	static DisplayList lists[LIST_NUM] ATTRIBUTE_ALIGN(32);
	static u16 current[2];
	static s32 used;
	static s16 limit;
	static u8 full;
	
	static std::set<u16> cached; // used to check if a chunk is cached
	static std::set<u16> toRelease; // chunks that CAN be released if needed
	static std::set<ChunkCoord> toCacheSet; // used only to unify the assoc. queue
	static std::queue<ChunkCoord> toCacheQueue; // chunks to cache
	
public:
	static void render(Camera& cam);
	
	static void init();
	
	static void reset();
	
	/**
	 * @brief invalidate the cache of a given chunk
	 * @param[in] id the id of the chunk
	 * */
	static void release(u32 id);
	
	static u8 cache(VerticalChunk& vc);
	
	static void addVertex(f32 x, f32 y, f32 z, u8 c, u16 tc, u8 alpha);
	
	static void cache(Player players[4]);
};


#endif //MIICRAFTTEST_CACHEMANAGER_H
