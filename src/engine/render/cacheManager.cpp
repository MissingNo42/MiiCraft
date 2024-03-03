//
// Created by Romain on 19/02/2024.
//
#include <malloc.h>
#include "render/cacheManager.h"
#include "world/world.h"

DisplayList ChunkCache::lists[LIST_NUM] ATTRIBUTE_ALIGN(32);

u16 ChunkCache::current[2] = { 0, 0 };
s32 ChunkCache::used = 0;
u8 ChunkCache::full = 0;
s16 ChunkCache::limit = 0;

std::set<u16> ChunkCache::cached; // used to check if a chunk is cached
std::set<u16> ChunkCache::toRelease; // chunks that CAN be released if needed
std::set<ChunkCoord> ChunkCache::toCacheSet; // used only to unify the assoc. queue
std::queue<ChunkCoord> ChunkCache::toCacheQueue; // chunks to cache

void ChunkCache::render(Camera& cam) {
	for (auto & list : lists) if (list.id && list.type == RENDER_OPAQUE) {
		auto c = World::chunkSlots[list.id].coord;
		if (cam.isChunkVisible(c) && std::max(std::abs(c.x - ((int)cam.pos.x >> 4)), std::abs(c.y - ((int)cam.pos.z >> 4))) < limit) list.render();
	}
	for (auto & list : lists) if (list.id && list.type == RENDER_TRANSPARENT) {
		auto c = World::chunkSlots[list.id].coord;
		if (cam.isChunkVisible(c) && std::max(std::abs(c.x - ((int)cam.pos.x >> 4)), std::abs(c.y - ((int)cam.pos.z >> 4))) < limit) list.render();
	}
}

void ChunkCache::reset() {
	for (auto & list : lists) list.reset();
	current[0] = current[1] = 0;
	used = 0;
	cached.clear();
	toRelease.clear();
	toCacheSet.clear();
	while (!toCacheQueue.empty()) toCacheQueue.pop();
}

void ChunkCache::init() {
	//lists = (DisplayList *)memalign(32, sizeof(DisplayList) * LIST_NUM);
	printf("Display lists: %p\r", (void *)lists);
}

void ChunkCache::release(u32 id) {
	for (auto & list : lists) if (list.id == id) {
		list.reset();
		used--;
		full = 0;
	}
	cached.erase((u16)id);
}

u8 ChunkCache::cache(VerticalChunk& vc) {
	if (vc.id == 0) return 0;
	try {
		if (cached.contains(vc.id)) {
			if (!vc.recache) return 0; // no need to recache
			release(vc.id); // release the old cache
		}
		if (full) return 1; // no more space
		
		int i;
		retry0: for (i = 0; i < LIST_NUM; i++) {
			if (lists[i].id == 0) {
				current[0] = i;
				lists[i].reset(RENDER_OPAQUE, vc.id);
				used++;
				break;
			}
		}
		
		if (i == LIST_NUM) { // (no recursion, release -> always find)
			if (!toRelease.empty()) {
				release(*toRelease.begin());
				toRelease.erase(toRelease.begin());
				goto retry0;
			} else return 1; // FAILED
		}
		
		retry1: for (; i < LIST_NUM; i++) {
			if (lists[i].id == 0) {
				current[1] = i;
				lists[i].reset(RENDER_TRANSPARENT, vc.id);
				used++;
				break;
			}
		}
		
		if (i == LIST_NUM) { // (no recursion, release -> always find)
			if (!toRelease.empty()) {
				release(*toRelease.begin());
				toRelease.erase(toRelease.begin());
				goto retry1;
			} else { // FAILED
				if (lists[current[0]].seal()) used--; // release the first list
				return 1;
			}
		}
		
		Renderer::renderChunk(vc); // render the chunk
		
		if (lists[current[0]].seal()) used--; // seal the opaque list
		if (lists[current[1]].seal()) used--; // seal the alpha list
		
		vc.recache = 0;
		cached.insert(vc.id);
		return 0;
		
	} catch (...) {
		printf("Caching chunk %d at %d %d FAILED\r", vc.id, vc.coord.x, vc.coord.y);
		release(vc.id); // release partial cache
		return 1;
	}
}

void ChunkCache::addVertex(f32 x, f32 y, f32 z, u8 c, u16 tc, u8 alpha) {
	if (lists[current[alpha]].addVertex(x, y, z, c, tc)) {
		lists[current[alpha]].seal();
		
		int i;
		retry: for (i = 0; i < LIST_NUM; i++) {
			if (lists[i].id == 0) {
				lists[i].reset(lists[current[alpha]].type, lists[current[alpha]].id);
				current[alpha] = i;
				used++;
				return;
			}
		}
		
		if (!toRelease.empty()) {
			release(*toRelease.begin());
			toRelease.erase(toRelease.begin());
			goto retry;
		}
		
		//if (lists[current[alpha ^ 1]].seal()) used--;
		full = 1;
		throw std::exception();
	}
}

void ChunkCache::cache(Player players[4]) {
	/// init view centers
	ChunkCoord pos {0, 0};
	ChunkCoord poss[4] {ChunkCoord{0, 0}, ChunkCoord{0, 0}, ChunkCoord{0, 0}, ChunkCoord{0, 0}};
	Camera * cams[4] = {nullptr, nullptr, nullptr, nullptr};
	
	int c = 0;
	
	for (int i = 0; i < 4; i++) {
		if (players[i].wiimote.connected) {
			poss[c].x = (short)(((int)players[i].renderer.camera.pos.x) >> 4);
			poss[c].y = (short)(((int)players[i].renderer.camera.pos.z) >> 4);
			cams[c] = &players[i].renderer.camera;
			c++;
		}
	}
	
	/// get the cache list
	
	// get the center of the view
	for (int i = 0; i < c; i++) {
		pos = poss[i];
		if (!toCacheSet.contains(pos)) {
			toCacheSet.insert(pos);
			toCacheQueue.push(pos);
		}
	}
	
	limit = MAX_RENDER_DIST;
	if (c == 2) limit -= 1;
	else if (c > 2) limit -= 2; // reduce pression on the cache
	
	// get the surrounding visible chunks
	for (short n = 1; n < limit; n++) { // for each dist level
		for (int p = 0; p < c; p++) { // and for each player
			ChunkCoord cpos = poss[p];
			Camera& cam = *cams[p];
			int i, j = -n;
			
			for (i = -n; i <= n; i++) {
				pos.x = (short)(cpos.x + i);
				pos.y = (short)(cpos.y + j);
				if (cam.isChunkVisible(pos) && !toCacheSet.contains(pos)) {
					toCacheSet.insert(pos);
					toCacheQueue.push(pos);
				}
			}
			j = n;
			for (i = -n; i <= n; i++) {
				pos.x = (short)(cpos.x + i);
				pos.y = (short)(cpos.y + j);
				if (cam.isChunkVisible(pos) && !toCacheSet.contains(pos)) {
					toCacheSet.insert(pos);
					toCacheQueue.push(pos);
				}
			}
			i = -n;
			for (j = 1 - n; j < n; j++) {
				pos.x = (short)(cpos.x + i);
				pos.y = (short)(cpos.y + j);
				if (cam.isChunkVisible(pos) && !toCacheSet.contains(pos)) {
					toCacheSet.insert(pos);
					toCacheQueue.push(pos);
				}
			}
			i = n;
			for (j = 1 - n; j < n; j++) {
				pos.x = (short)(cpos.x + i);
				pos.y = (short)(cpos.y + j);
				if (cam.isChunkVisible(pos) && !toCacheSet.contains(pos)) {
					toCacheSet.insert(pos);
					toCacheQueue.push(pos);
				}
			}
		}
	}
	
	// check which chunks can be released
	for (auto & id : cached) {
		pos = World::chunkSlots[id].coord;
		if (!toCacheSet.contains(pos)) toRelease.insert(id);
	}
	
	// cache the chunks
	while (!toCacheQueue.empty()) {
		pos = toCacheQueue.front();
		toCacheQueue.pop();
		if (cache(World::getChunkAt(pos, true))) break;
	}
	if (!toCacheQueue.empty()) {
		printf("Cache limit reached\r");
		while (!toCacheQueue.empty()) toCacheQueue.pop();
	}
	
	// cleanup
	toRelease.clear();
	toCacheSet.clear();
	
	printf("Cache Slots %d / %d (%.1f %%) with %d / %d (%.1f %%) loaded chunks\r",
		   used, LIST_NUM, (f32)used / (f32)LIST_NUM * 100.0f,
		   World::usedSlots, LOADED_CHUNKS, (f32)World::usedSlots / (f32)LOADED_CHUNKS * 100.0f
   );
}
