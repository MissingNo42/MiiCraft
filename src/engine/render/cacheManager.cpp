//
// Created by Romain on 19/02/2024.
//
#include "render/cacheManager.h"

DisplayList ChunkCache::lists[LIST_NUM] ATTRIBUTE_ALIGN(32);
int ChunkCache::current[2] = {0, 0 };
int ChunkCache::used = 0;

void ChunkCache::render() {
	printf("USED %d\r", used);
	int r = 0, s = 0;
	for (auto & list : lists) {
		if (list.id) {
			r++;
			s += list.sealed != 0;
		}
	}
	printf("RS %d %d\r", r, s);
	
	for (auto & list : lists) if (list.id && list.type == RENDER_OPAQUE) list.render();
	for (auto & list : lists) if (list.id && list.type == RENDER_TRANSPARENT) list.render();
}

void ChunkCache::reset() {
	for (auto & list : lists) list.reset();
	current[0] = current[1] = 0;
	used = 0;
}

void ChunkCache::release(u32 id) {
	for (auto & list : lists) if (list.id == id) {
		list.reset();
		used--;
	}
}

void ChunkCache::cache(VerticalChunk& vc, Renderer& renderer, t_pos2D pos) {
	if (vc.id == 0) return;
	if (vc.recache) {
		vc.recache = 0;
		if (isCached(vc.id)) release(vc.id);
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
				lists[i].reset(RENDER_OPAQUE, vc.id);
				used++;
				break;
			}
		}
		if (i == LIST_NUM) return;
		renderChunk(vc, renderer, pos);
		if (lists[current[0]].seal()) used--;
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
	}
}

void ChunkCache::cache(World& world, Renderer& renderer) {
	f32 x = renderer.camera.pos.x, y = renderer.camera.pos.y;
	short cx = (short)((int)x >> 4), cy = (short)((int)y >> 4);
	
	VerticalChunk& vc = world.getChunkAt({cx, cy});
	cache(vc, renderer, {cx, cy}); // cache the current chunk
	
	for (int i = -1; i < 2; i++) {  // cache the surrounding chunks
		for (int j = -1; j < 2; j++) if (i != j) {
			t_pos2D pos = {(short)(cx + i), (short)(cy + j)};
			VerticalChunk& vn = world.getChunkAt(pos);
			cache(vn, renderer, pos);
		}
	}
	
	for (short n = 2; n < 4; n++) {
		int i;
		int j = -n;
		for (i = -n; i <= n; i++) {
			t_pos2D pos = {(short)(cx + i), (short)(cy + j)};
			if (renderer.camera.isChunkVisible(pos.x, pos.y)) {
				VerticalChunk& vn = world.getChunkAt(pos);
				cache(vn, renderer, pos);
			}
		}
		j = n;
		for (i = -n; i <= n; i++) {
			t_pos2D pos = {(short)(cx + i), (short)(cy + j)};
			if (renderer.camera.isChunkVisible(pos.x, pos.y))
			{
				VerticalChunk &vn = world.getChunkAt(pos);
				cache(vn, renderer, pos);
			}
		}
		i = -n;
		for (j = 1 - n; j < n; j++) {
			t_pos2D pos = {(short)(cx + i), (short)(cy + j)};
			if (renderer.camera.isChunkVisible(pos.x, pos.y)) {
				VerticalChunk &vn = world.getChunkAt(pos);
				cache(vn, renderer, pos);
			}
		}
		i = n;
		for (j = 1 - n; j < n; j++) {
			t_pos2D pos = {(short)(cx + i), (short)(cy + j)};
			if (renderer.camera.isChunkVisible(pos.x, pos.y)) {
				VerticalChunk &vn = world.getChunkAt(pos);
				cache(vn, renderer, pos);
			}
		}
	}
}
