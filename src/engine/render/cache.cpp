//
// Created by Romain on 19/02/2024.
//
#include <malloc.h>

#include "engine/render/cache.h"
#include "world/world.h"


ChunkCache::DL ChunkCache::lists[list_num];
ChunkCache::UnitId ChunkCache::links[list_num];

s32 ChunkCache::used        = 0;
s32 ChunkCache::render_dist = 0;
ChunkCache::CacheUnit ChunkCache::activeCacheUnit;

std::queue<ChunkCache::UnitId> ChunkCache::availableUnits{std::move(std::deque<UnitId>(list_num))};
std::queue<ChunkCoord> ChunkCache::cacheQueue{std::move(std::deque<ChunkCoord>(list_num))};
std::queue<ChunkCache::SlotId> ChunkCache::releasableSlots{std::move(std::deque<SlotId>(list_num))};

std::unordered_set<ChunkCoord> ChunkCache::isQueued;
std::unordered_map<ChunkCache::SlotId, ChunkCache::CacheUnit> ChunkCache::cached;


void ChunkCache::render(Camera & cam) {
	for (s32 i = 0; i < static_cast<s32>(RenderChannel::CHANNELS); i++) {
		for (auto & [sid, units] : cached) {
			if (const auto c = World::chunkSlots[sid].coord; cam.isChunkVisible(c) && std::max(
																 std::abs(c.x - (static_cast<s32>(cam.pos.x) >> 4)),
																 std::abs(c.z - (static_cast<s32>(cam.pos.z) >> 4))
															 ) < render_dist) {
				UnitId id = units.units[i];

				while (id != INVALID_UNIT) {
					lists[id].render();
					id = links[id];
				}
			}
		}
	}
}


void ChunkCache::reset() {
	printf("Display lists at %p (%u kB)\r", static_cast<void *>(lists), sizeof(lists) >> 10);

	for (auto & list : lists) list.reset();

	for (s32 channel = 0; channel < static_cast<s32>(RenderChannel::CHANNELS); channel++) {
		activeCacheUnit.units[channel] = INVALID_UNIT;
	}

	used        = 0;
	render_dist = 0;

	while (!cacheQueue.empty()) cacheQueue.pop();
	while (!releasableSlots.empty()) releasableSlots.pop();
	while (!availableUnits.empty()) availableUnits.pop();
	for (UnitId i = 0; i < list_num; i++) availableUnits.push(i);

	cached.clear();
}

bool ChunkCache::prerender(Chunk & vc) {
	if (vc.id == 0) return false;

	if (cached.contains(vc.id)) {
		if (!vc.recache) return false; // no need to recache
		releaseSlot(vc.id);            // release the old cache
	}

	if (availableUnits.empty() && releasableSlots.empty()) return true; // no more space

	for (auto & uid : activeCacheUnit.units) {
		uid = INVALID_UNIT;
	}

	bool full    = false;
	bool nonvoid = false;

	try {
		Renderer::renderChunk(vc); // render the chunk
	}
	catch (...) {
		full = true;
	}

	for (const UnitId uid : activeCacheUnit.units) {
		if (uid != INVALID_UNIT) {
			lists[uid].pad();
			nonvoid = true;
		}
	}

	if (nonvoid) {
		cached.insert({vc.id, activeCacheUnit});
	}

	if (full) {
		releaseSlot(vc.id);  // release partially cached chunk
		return true;
	}

	vc.recache = full;
	return full;
}


void ChunkCache::prerender(Player players[4]) {
	/// init view centers
	ChunkCoord pos{0, 0};
	ChunkCoord poss[4]{ChunkCoord{0, 0}, ChunkCoord{0, 0}, ChunkCoord{0, 0}, ChunkCoord{0, 0}};
	Camera * cams[4] = {nullptr, nullptr, nullptr, nullptr};

	s32 c = 0;

	for (s32 i = 0; i < 4; i++) {
		if (players[i].wiimote.connected) {
			poss[c].x = static_cast<s32>(players[i].renderer.camera.pos.x) >> 4;
			poss[c].z = static_cast<s32>(players[i].renderer.camera.pos.z) >> 4;
			cams[c]   = &players[i].renderer.camera;
			c++;
		}
	}

	// get the center of the view
	for (s32 i = 0; i < c; i++) {
		pos = poss[i];

		if (!isQueued.contains(pos)) {
			isQueued.insert(pos);
			cacheQueue.push(pos);
		}
	}

	render_dist = max_render_dist;
	if (c == 2) render_dist -= 1;
	else if (c > 2) render_dist -= 2; // reduce pression on the cache

	// get the surrounding visible chunks
	for (s32 n = 1; n < render_dist; n++) { // for each dist level

		for (s32 p = 0; p < c; p++) { // and for each player
			auto [x, z]  = poss[p];
			Camera & cam = *cams[p];
			s32 i, j     = -n;

			for (i = -n; i <= n; i++) {
				pos.x = x + i;
				pos.z = z + j;
				if (!isQueued.contains(pos) && cam.isChunkVisible(pos)) {
					isQueued.insert(pos);
					cacheQueue.push(pos);
				}
			}
			j = n;
			for (i = -n; i <= n; i++) {
				pos.x = x + i;
				pos.z = z + j;
				if (!isQueued.contains(pos) && cam.isChunkVisible(pos)) {
					isQueued.insert(pos);
					cacheQueue.push(pos);
				}
			}
			i = -n;
			for (j = 1 - n; j < n; j++) {
				pos.x = x + i;
				pos.z = z + j;
				if (!isQueued.contains(pos) && cam.isChunkVisible(pos)) {
					isQueued.insert(pos);
					cacheQueue.push(pos);
				}
			}
			i = n;
			for (j = 1 - n; j < n; j++) {
				pos.x = x + i;
				pos.z = z + j;
				if (!isQueued.contains(pos) && cam.isChunkVisible(pos)) {
					isQueued.insert(pos);
					cacheQueue.push(pos);
				}
			}
		}
	}

	// check which chunks can be released
	std::queue<SlotId> toRelease;
	for (auto & id : cached) {
		pos = World::chunkSlots[id.first].coord;

		if (!isQueued.contains(pos)) {

			if (World::chunkSlots[id.first].recache) {  // just drop it if not up to date
				toRelease.push(id.first);             // differ the release to not invalidate the iterator
			} else {
				releasableSlots.push(id.first);
			}
		}
	}

	// release the chunks
	while (!toRelease.empty()) {  // release not up to date chunks
		releaseSlot(toRelease.front());
		toRelease.pop();
	}

	// cache the chunks
	while (!cacheQueue.empty()) {
		pos = cacheQueue.front();
		cacheQueue.pop();
		if (prerender(World::getChunkAt(pos, true))) {
			printf("Cache limit reached (still to do : %u)\r", cacheQueue.size());
			break;
		}
	}

	while (!cacheQueue.empty()) cacheQueue.pop();
	while (!releasableSlots.empty()) releasableSlots.pop();
	isQueued.clear();

	printf(
		"Cache Slots %d / %d (%.1f %%) with %d / %d (%.1f %%) loaded chunks\r",
		used,
		list_num,
		static_cast<f32>(used) / static_cast<f32>(list_num) * 100.0f,
		World::usedSlots,
		LOADED_CHUNKS,
		static_cast<f32>(World::usedSlots) / static_cast<f32>(LOADED_CHUNKS) * 100.0f
	);
}
