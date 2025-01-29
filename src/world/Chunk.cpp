#include "world/Chunk.h"
#include "world/world.h"

void Chunk::pushNeighboor(const BlockCoord & bcoord, const bool shadow) {
	BlockCoord target;
	auto & queue = shadow ? shadowQueue : lightQueue;

	if (bcoord.y < CHUNK_LIMIT) {
		target = bcoord;
		target.y++;
		queue.emplace(target);
	}

	if (bcoord.y) {
		target = bcoord;
		target.y--;
		queue.emplace(target);
	}

	target = bcoord;
	if (bcoord.x) {
		target.x--;
		queue.emplace(target);
	}
	else {
		if (auto & westChunk = getNeighboorChunk(WEST); westChunk.id) {
			target.x = 15;
			(shadow ? westChunk.shadowQueue : westChunk.lightQueue).emplace(target);
		}
	}

	target = bcoord;
	if (bcoord.x < 15) {
		target = bcoord;
		target.x++;
		queue.emplace(target);
	}
	else {
		if (auto & eastChunk = getNeighboorChunk(EAST); eastChunk.id) {
			target.x = 0;
			(shadow ? eastChunk.shadowQueue : eastChunk.lightQueue).emplace(target);
		}
	}

	target = bcoord;
	if (bcoord.z) {
		target = bcoord;
		target.z--;
		queue.emplace(target);
	}
	else {
		if (auto & southChunk = getNeighboorChunk(SOUTH); southChunk.id) {
			target.z = 15;
			(shadow ? southChunk.shadowQueue : southChunk.lightQueue).emplace(target);
		}
	}

	target = bcoord;
	if (bcoord.z < 15) {
		target = bcoord;
		target.z++;
		queue.emplace(target);
	}
	else {
		if (auto & northChunk = getNeighboorChunk(NORTH); northChunk.id) {
			target.z = 0;
			(shadow ? northChunk.shadowQueue : northChunk.lightQueue).emplace(target);
		}
	}
}

void Chunk::setBlock(const BlockCoord & coord, const Block block) noexcept {
	const Block old = blocks[coord.y][coord.x][coord.z];

	if (old.type == block.type && old.flags == block.flags) return;

	recache = true;
	dirty   = true;

	/// Recache neighboors if needed
	if (!coord.x) {
		auto & westChunk  = getNeighboorChunk(WEST);
		westChunk.recache = true;

		// needed to update light occlusion in corners
		if (!coord.z) westChunk.getNeighboorChunk(SOUTH).recache = true;
		else if (coord.z == 15) westChunk.getNeighboorChunk(NORTH).recache = true;
	}
	else if (coord.x == 15) {
		auto & eastChunk  = getNeighboorChunk(EAST);
		eastChunk.recache = true;

		// needed to update light occlusion in corners
		if (!coord.z) eastChunk.getNeighboorChunk(SOUTH).recache = true;
		else if (coord.z == 15) eastChunk.getNeighboorChunk(NORTH).recache = true;
	}

	if (!coord.z) getNeighboorChunk(SOUTH).recache = true;
	else if (coord.z == 15) getNeighboorChunk(NORTH).recache = true;

	/// update light
	const u8 oldEmitted = blockData[old.type].emittedLight;
	const u8 newEmitted = blockData[block.type].emittedLight;

	const bool oldTransp = blockData[old.type].isTransparent;
	const bool newTransp = blockData[block.type].isTransparent;

	if (newEmitted < oldEmitted || (!newTransp && oldTransp)) {
		if (oldTransp) shadowQueue.emplace(coord);
		else pushNeighboor(coord, true);
		propagate(true);
	}

	blocks[coord.y][coord.x][coord.z] = block;

	if (newEmitted != oldEmitted || newTransp != oldTransp) {
		lightQueue.emplace(coord); // ensure propagation
		if (!oldTransp && newTransp && newEmitted >= oldEmitted) pushNeighboor(coord, false);
		propagate(false);
	}
}


Chunk & Chunk::getNeighboorChunk(const Direction neighboor) const noexcept {
	return World::chunkSlots[neighboors[neighboor]];
}

[[nodiscard]] Chunk & Chunk::getDiagonalNeighboorChunk(const Direction vertical, const Direction horizontal) const noexcept {
	Direction n1, n2;
	if (neighboors[vertical]) n1 = vertical, n2 = horizontal;
	else n1 = horizontal, n2 = vertical;

	return getNeighboorChunk(n1).getNeighboorChunk(n2);
}


void Chunk::propagate(const bool shadow) {
	auto & mainQueue = shadow ? shadowQueue : lightQueue;
	if (mainQueue.empty()) return;

	auto & westChunk  = getNeighboorChunk(WEST);
	auto & eastChunk  = getNeighboorChunk(EAST);
	auto & northChunk = getNeighboorChunk(NORTH);
	auto & southChunk = getNeighboorChunk(SOUTH);

	recache = true; // will need a re-render, but not a re-save

	u8 nLight, aLight;
	void * next; // next goto/jump target
	const void * propagate = shadow ? &&propagate_shadow : &&propagate_light;

	do {
		std::unordered_set currentQueue{std::move(mainQueue)};
		mainQueue = std::unordered_set<BlockCoord>{}; // full clear

		for (const BlockCoord p : currentQueue) {
			Block & block = blocks[p.y][p.x][p.z];

			if (shadow) {
				if (!blockData[block.type].isTransparent) {
					if (blockData[block.type].emittedLight) lightQueue.emplace(p);
					continue;
				}

				/// faster than bitfield for compare
				nLight = block.naturalLight;
				aLight = block.artificialLight;

				/// switch off
				block.naturalLight    = 0;
				block.artificialLight = blockData[block.type].emittedLight;
			}
			else {
				const auto & data = blockData[block.type];

				nLight = data.isTransparent && block.naturalLight ? block.naturalLight - 1 : 0;
				aLight = std::max(data.isTransparent ? block.artificialLight - 1 : 0, static_cast<s32>(data.emittedLight));
			}

			BlockCoord dst = p;
			auto target    = this;

			/// retropropagate
			if (p.y < CHUNK_LIMIT) {
				dst.y++;

				next = &&next_left;
				goto *propagate;
			}

		next_left:
			if (p.x) dst.x--;
			else if (westChunk.id) {
				dst.x  = 15;
				target = &westChunk;
			}
			else goto next_right;

			next = &&next_right;
			goto *propagate;

		next_right:
			if (p.x < 15) dst.x++;
			else if (eastChunk.id) {
				dst.x  = 0;
				target = &eastChunk;
			}
			else goto next_back;

			next = &&next_back;
			goto *propagate;

		next_back:
			if (p.z) dst.z--;
			else if (southChunk.id) {
				dst.z  = 15;
				target = &southChunk;
			}
			else goto next_front;

			next = &&next_front;
			goto *propagate;

		next_front:
			if (p.z < 15) dst.z++;
			else if (northChunk.id) {
				dst.z  = 0;
				target = &northChunk;
			}
			else goto next_bottom;

			next = &&next_bottom;
			goto *propagate;

		next_bottom:
			if (p.y) {
				dst.y--;

				/// Special rule: sun light not decrease
				if (shadow) {
					if (nLight == 15) nLight++; // 16 will accept 15 as decayed light gradient
				}
				else if (const auto & data                                                        = blockData[block.type];
					!data.isLightFilter && data.isTransparent && block.naturalLight == 15) nLight = block.naturalLight;

				next = &&end;
				goto *propagate;
			}

			continue;

			/// Propagator sub-functions (deadcode, called by goto)
		propagate_light:
			if (Block & nb = target->blocks[dst.y][dst.x][dst.z]; blockData[nb.type].isTransparent) {
				if (nb.naturalLight < nLight) {
					nb.naturalLight = nLight;
					target->lightQueue.emplace(dst);
				}
				if (nb.artificialLight < aLight) {
					nb.artificialLight = aLight;
					target->lightQueue.emplace(dst);
				}
			}

			target = this;
			dst    = p;
			goto *next;

		propagate_shadow:
			if (const Block nb = target->blocks[dst.y][dst.x][dst.z]; blockData[nb.type].isTransparent) {
				if (nb.naturalLight < nLight) target->shadowQueue.emplace(dst);
				else target->lightQueue.emplace(dst);

				if (nb.artificialLight < aLight) target->shadowQueue.emplace(dst);
				else target->lightQueue.emplace(dst);
			}
			else if (blockData[nb.type].emittedLight) {
				target->lightQueue.emplace(dst);
			}

			target = this;
			dst    = p;
			goto *next;

		end:;
		}
	} while (!mainQueue.empty());

	// propagate to neighboors
	// recursion depth is (implicitly) limited to ~5 (light level decreasing, chunk's limit snake pattern)
	westChunk.propagate(shadow);
	eastChunk.propagate(shadow);
	northChunk.propagate(shadow);
	southChunk.propagate(shadow);
}
