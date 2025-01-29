#pragma once

#include <gctypes.h>
#include <queue>
#include <set>
#include "cacheUnit.h"
#include "player.h"
#include "renderer.h"
#include "world/Chunk.h"

#define MAX_RENDER_DIST 8

class ChunkCache {
	static DisplayList lists[LIST_NUM] ATTRIBUTE_ALIGN(32);
	static u16 current[2];
	static s32 used;
	static s32 limit;
	static u8 full;
public:
	static std::set<ChunkCoord> toCacheSet; // used only to unify the assoc. queue
	static std::set<u16> cached; // used to check if a chunk is cached
	static std::set<u16> toRelease; // chunks that CAN be released if needed
	static std::queue<ChunkCoord> toCacheQueue; // chunks to cache

	static void render(Camera& cam);

	static void init();

	static void reset();

	/**
	 * @brief invalidate the cache of a given chunk
	 * @param[in] id the id of the chunk
	 * */
	static void release(u32 id);

	static u8 cache(Chunk& vc);

	static void addVertex(f32 x, f32 y, f32 z, u16 c, u16 tc, u8 alpha);

	static void cache(Player players[4]);
};
