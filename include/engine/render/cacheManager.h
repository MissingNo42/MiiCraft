//
// Created by Romain on 19/02/2024.
//

#ifndef MIICRAFTTEST_CACHEMANAGER_H
#define MIICRAFTTEST_CACHEMANAGER_H


#include <gctypes.h>
#include "world/verticalChunk.h"
#include "renderer.h"
#include "cache.h"

class ChunkCache {
	static DisplayList lists[LIST_NUM] ATTRIBUTE_ALIGN(32);
	//static DisplayList * lists;
	static u16 current[2];
	static s32 used;
	static u8 full;
	
public:
	static void render();
	
	static void init();
	
	static void reset();
	
	/**
	 * @brief invalidate the cache of a given chunk
	 * @param[in] id the id of the chunk
	 * */
	static void release(u32 id);
	
	static void cache(VerticalChunk& vc, Renderer& renderer);
	
	static u8 isCached(u32 id);
	
	static void addVertex(f32 x, f32 y, f32 z, u8 c, f32 u, f32 v, u8 alpha);
	
	static void cache(Renderer& renderer);
};


#endif //MIICRAFTTEST_CACHEMANAGER_H
