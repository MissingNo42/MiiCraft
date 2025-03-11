#pragma once

#include <gctypes.h>
#include <queue>
#include <unordered_map>

#include "player.h"
#include "renderer.h"
#include "world/Chunk.h"


/**
 * @brief Render channel are rendered separetely, in order
 * @note The cache may not be designed to handle efficiently to many channels
 */
enum class RenderChannel: u8 {  // u3 max for opti
	Opaque = 0, Transparent = 1, CHANNELS
};


class ChunkCache final {
	using UnitId = u16; // Cache unit id for prerendered chunk
	using SlotId = u16; // Slot id for chunk in the chunk controller

	struct CacheUnit final {
		UnitId units[static_cast<u32>(RenderChannel::CHANNELS)]; // list of the last display lists for each channel
	};


	constexpr static u32 max_render_dist = 8;
	constexpr static u32 quads_per_list  = 261;  // arbitrary
	constexpr static u32 list_size       = 4 * quads_per_list;
	constexpr static u32 list_num        = 600;  // arbitrary
	constexpr static UnitId INVALID_UNIT = static_cast<UnitId>(-1u);

	using DL = DisplayList<list_size, Vertex, GX_QUADS | GX_VTXFMT0>;

	/// Data
	static DL lists[list_num];      // display lists prerendered GPU ready data
	static UnitId links[list_num];  // lists are identified by their id and linked together by channels and chunks

	/// Metadata
	static s32 used;
	static s32 render_dist;
	static CacheUnit activeCacheUnit;                    // current cache unit of the rendering chunk

	static std::queue<UnitId> availableUnits;            // available cache units
	static std::queue<ChunkCoord> cacheQueue;            // ordered queue of chunks to cache (center to far away)
	static std::queue<SlotId> releasableSlots;           // chunks cache that CAN be released if needed

	static std::unordered_set<ChunkCoord> isQueued;      // used to keep unicity in cacheQueue
	static std::unordered_map<SlotId, CacheUnit> cached; // cached chunks

public:
	static void render(Camera & cam);

	static void reset();

	static bool prerender(Chunk & vc);

	static void prerender(Player players[4]);

	/**
	 * @brief invalidate the cache of a given cached chunk
	 * @param[in] sid the slot id of the chunk
	 * @note the chunk is assumed to be cached
	 * */
	static void releaseSlot(const SlotId sid) {
		const auto it        = cached.find(sid);

		for (auto uid : it->second.units) {
			while (uid != INVALID_UNIT) {
				lists[uid].reset();        // reset the list
				availableUnits.push(uid);  // mark it as available
				uid = links[uid];          // get the next list
				used--;					   // decrease the used counter
			}
		}

		cached.erase(it);  // remove the chunk from the cache
	}

	static UnitId getAvailableUnit() {
		if (availableUnits.empty()) { // need to free a list

			if (releasableSlots.empty()) throw std::exception(); // no list to release

			const SlotId sid = releasableSlots.front();
			releasableSlots.pop();
			releaseSlot(sid);
		}

		const UnitId id = availableUnits.front();
		availableUnits.pop();
		return id;
	}

	static void addVertex(const Vertex & vtx, RenderChannel channel) {
		const UnitId id = activeCacheUnit.units[static_cast<s32>(channel)];
		auto * list     = lists + id;

		if (id != INVALID_UNIT) {
			if (list->size == list_size) {
				list->flush(); // need a new list
				goto new_list;
			}
		}
		else {
		new_list:
			const UnitId nid = getAvailableUnit();
			list                                             = lists + nid;
			links[nid]                                       = id;  // make the new list point to the previous one
			activeCacheUnit.units[static_cast<s32>(channel)] = nid; // update the active list
			used++;													// increase the used counter
		}

		list->vertices[list->size++] = vtx;
	}
};
