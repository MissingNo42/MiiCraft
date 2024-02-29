//
// Created by Romain on 19/02/2024.
//
#include <malloc.h>
#include "render/cacheManager.h"
#include "world/world.h"

DisplayList ChunkCache::lists[LIST_NUM] ATTRIBUTE_ALIGN(32);
//DisplayList * ChunkCache::lists;
u16 ChunkCache::current[2] = { 0, 0 };
s32 ChunkCache::used = 0;
u8 ChunkCache::full = 0;

void ChunkCache::render() {
	u16 used2 = 0;
	for (auto & list : lists) if (list.id) {
		used2++;
	}
	
	printf("USED %d | %d\r", used, used2);
	printf("CHUNKS %d\r", World::usedSlots);
	for (auto & list : lists) if (list.id && list.type == RENDER_OPAQUE) list.render();
	for (auto & list : lists) if (list.id && list.type == RENDER_TRANSPARENT) list.render();
}

void ChunkCache::reset() {
	for (auto & list : lists) list.reset();
	current[0] = current[1] = 0;
	used = 0;
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
}

void ChunkCache::cache(VerticalChunk& vc, Renderer& renderer) {
	if (vc.id == 0) return;
	try {
		if (isCached(vc.id)) {
			if (!vc.recache) return;
			release(vc.id);
		}
		if (full) return;
		int i;
		for (i = 0; i < LIST_NUM; i++) {
			if (lists[i].id == 0) {
				current[0] = i;
				lists[i].reset(RENDER_OPAQUE, vc.id);
				used++;
				break;
			}
		}
		for (; i < LIST_NUM; i++) {
			if (lists[i].id == 0) {
				current[1] = i;
				lists[i].reset(RENDER_TRANSPARENT, vc.id);
				used++;
				break;
			}
		}
		if (i == LIST_NUM) return;
		renderChunk(vc, renderer);
		if (lists[current[0]].seal()) used--;
		if (lists[current[1]].seal()) used--;
		vc.recache = 0;
	} catch (...) {
		printf("Caching %d %d FAILED\r", vc.coord.x, vc.coord.y);
	}
}

u8 ChunkCache::isCached(u32 id) {
	for (auto & list : lists) if (list.id == id) return 1;
	return 0;
}

void ChunkCache::addVertex(f32 x, f32 y, f32 z, u8 c, f32 u, f32 v, u8 alpha) {
	if (lists[current[alpha]].addVertex(x, y, z, c, u, v)) {
		lists[current[alpha]].seal();
		
		for (int i = 0; i < LIST_NUM; i++) {
			if (lists[i].id == 0) {
				lists[i].reset(lists[current[alpha]].type, lists[current[alpha]].id);
				current[alpha] = i;
				used++;
				return;
			}
		}
		
		if (lists[current[alpha ^ 1]].seal()) used--;
		full = 1;
		throw std::exception();
	}
}

void ChunkCache::cache(Renderer& renderer) {
	f32 x = renderer.camera.pos.x, y = renderer.camera.pos.z;
	short cx = (short)(((int)x) >> 4), cy = (short)(((int)y) >> 4);
	ChunkCoord pos {cx, cy};
	
	VerticalChunk& vc = World::getChunkAt(pos, true);
	cache(vc, renderer); // cache the current chunk
	
	for (int i = -1; i < 2; i++) {  // cache the surrounding chunks
		for (int j = -1; j < 2; j++) if (i || j) {
			pos.x = (short)(cx + i);
			pos.y = (short)(cy + j);
			VerticalChunk& vn = World::getChunkAt(pos, true);
			cache(vn, renderer);
		}
	}
	
	const short limit = 7;
	
	
	for (short n = 2; n <= limit; n++) {
		int i;
		int j = -n;
		for (i = -n; i <= n; i++) {
			pos.x = (short)(cx + i);
			pos.y = (short)(cy + j);
			if (!renderer.camera.isChunkVisible(pos.x, pos.y))
				release(World::getChunkAt(pos, false).id);
		}
		j = n;
		for (i = -n; i <= n; i++) {
			pos.x = (short)(cx + i);
			pos.y = (short)(cy + j);
			if (!renderer.camera.isChunkVisible(pos.x, pos.y))
				release(World::getChunkAt(pos, false).id);
		}
		i = -n;
		for (j = 1 - n; j < n; j++) {
			pos.x = (short)(cx + i);
			pos.y = (short)(cy + j);
			if (!renderer.camera.isChunkVisible(pos.x, pos.y))
				release(World::getChunkAt(pos, false).id);
			
		}
		i = n;
		for (j = 1 - n; j < n; j++) {
			pos.x = (short)(cx + i);
			pos.y = (short)(cy + j);
			if (!renderer.camera.isChunkVisible(pos.x, pos.y))
				release(World::getChunkAt(pos, false).id);
		}
	}
	
	for (short n = 2; n < limit; n++) {
		int i;
		int j = -n;
		for (i = -n; i <= n; i++) {
			pos.x = (short)(cx + i);
			pos.y = (short)(cy + j);
			if (renderer.camera.isChunkVisible(pos.x, pos.y)) {
				VerticalChunk& vn = World::getChunkAt(pos, true);
				cache(vn, renderer);
			}
		}
		j = n;
		for (i = -n; i <= n; i++) {
			pos.x = (short)(cx + i);
			pos.y = (short)(cy + j);
			if (renderer.camera.isChunkVisible(pos.x, pos.y)) {
				VerticalChunk &vn = World::getChunkAt(pos, true);
				cache(vn, renderer);
			}
		}
		i = -n;
		for (j = 1 - n; j < n; j++) {
			pos.x = (short)(cx + i);
			pos.y = (short)(cy + j);
			if (renderer.camera.isChunkVisible(pos.x, pos.y)) {
				VerticalChunk &vn = World::getChunkAt(pos, true);
				cache(vn, renderer);
			}
		}
		i = n;
		for (j = 1 - n; j < n; j++) {
			pos.x = (short)(cx + i);
			pos.y = (short)(cy + j);
			if (renderer.camera.isChunkVisible(pos.x, pos.y)) {
				VerticalChunk &vn = World::getChunkAt(pos, true);
				cache(vn, renderer);
			}
		}
	}
}
