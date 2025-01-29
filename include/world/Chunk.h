#pragma once

#include <cstdint>
#include <gccore.h>
#include <unordered_set>
#include "block.h"
#include "coord.h"
#include "engine/render/block.h"


/**
 * @class Chunk
 * @brief One of these fixed unit stores a currently loaded chunk
 * The represented loaded chunk change to one another during the game lifetime
 * */
class Chunk {
	std::unordered_set<BlockCoord> lightQueue;
	std::unordered_set<BlockCoord> shadowQueue;

public:
	u16 neighboors[4]{}; // readonly from the outside
	u16 id; // const

	bool recache = true; // 1 if the chunk needs to be recached
	bool loaded  = false;
	bool dirty   = false;

	ChunkCoord coord;  // readonly from the outside
	Block blocks[CHUNK_HEIGHT][16][16]; // y, x, z // y=0 must be Bedrock & y=CHUNK_LIMIT must be Air


	void setBlock(const BlockCoord & coord, Block block) noexcept;

	void setBlockType(const BlockCoord & bcoord, const BlockType block) noexcept {
		setBlock(bcoord, {block, {}});
	}

	void setNeighboor(const Direction neighboor, const u16 chunk) noexcept {
		recache               = neighboors[neighboor] != chunk;
		neighboors[neighboor] = chunk;
	}

	[[nodiscard]] Chunk & getNeighboorChunk(Direction neighboor) const noexcept;
	[[nodiscard]] Chunk & getDiagonalNeighboorChunk(Direction vertical, Direction horizontal) const noexcept;

	void fillWith(const Block block) noexcept {
		for (auto & y : blocks) {
			for (auto & x : y) {
				for (auto & z : x) z = block;
			}
		}
	}

	void fillWith(const BlockType block = Bedrock) noexcept {
		fillWith({block, {}});
	}

	/**
	 * @brief reset the light of the chunk
	 * @warning assume neighboors are correctly lighted, never create 2 chunks at the same time and reset both only after
     * this include case where the current chunk light a neighboor chunk which is not considered as correctly lighted
     * @note intended to be used once after a chunk is loaded or created, not modify
     * @note the light state is incrementaly updated by setBlock (calling propagate), there is no defined way to perform a hard reset working in all cases
     * @note a hard reset could be done by reseting first all the 9 neighboor chunks (temporarily linked to chunk-0 to avoid propagation)
	 */
	void resetLight() noexcept {
		BlockCoord p{};
		recache = true;

		u8 lightMap[16][16]; // high of the first non "sun-lighted" block (or 0)

		// Manually apply sun light on each COLUMNS
		for (p.x = 0; p.x < 16; p.x++) {
			for (p.z = 0; p.z < 16; p.z++) {
				u8 value           = 15;
				lightMap[p.x][p.z] = CHUNK_LIMIT;

				for (p.y = CHUNK_LIMIT; p.y >= 0; p.y--) {
					auto & block = blocks[p.y][p.x][p.z];
					auto & data  = blockData[block.type];
					bool push    = data.emittedLight > 0;

					// push if the block is a light source
					if (data.isTransparent) {
						if (data.isLightFilter) { // sun light is soft-blocked
							if (value) lightMap[p.x][p.z] = p.y;
							value = 0;
							lightQueue.emplace(p.y + 1, p.x, p.z); // the block above will be pushed
						}

						Light lastLight{.light = block.light};
						block.naturalLight    = value;
						block.artificialLight = data.emittedLight;
						// not intended to be propagated but to light adjacent blocks if transparent too
						bool needRecache = lastLight.light != block.light;

						if (!p.x) {
							if (auto & neighboorChunk = getNeighboorChunk(WEST); neighboorChunk.id) {
								if (needRecache) neighboorChunk.recache = true; // Always recache if light changed even if neighboor is not altered

								Block nb     = neighboorChunk.blocks[p.y][15][p.z];
								auto & nData = blockData[nb.type];

								if (nData.emittedLight > block.artificialLight) {
									block.artificialLight = nData.emittedLight;
									push                  = true; // propagation target: self
								} // else "if block is light emitter" already covered by 'push' init at "emit > 0"

								if (nData.isTransparent) {
									if (nb.artificialLight - 1 > block.artificialLight) {
										block.artificialLight = nb.artificialLight - 1;
										push                  = true; // propagation target: self
									}
									else if (block.artificialLight - 1 > nb.artificialLight) push = true; // propagation target: neighboor

									if (value) {
										if (14 > nb.naturalLight) push = true; // propagation target: neighboor
									}
									else if (nb.naturalLight > 1) {
										block.naturalLight = nb.naturalLight - 1;
										push               = true; // propagation target: self
									}
								}
							}
						}
						else if (p.x == 15) {
							if (auto & neighboorChunk = getNeighboorChunk(EAST); neighboorChunk.id) {
								if (needRecache) neighboorChunk.recache = true; // Always recache if light changed even if neighboor is not altered

								Block nb     = neighboorChunk.blocks[p.y][0][p.z];
								auto & nData = blockData[nb.type];

								if (nData.emittedLight > block.artificialLight) {
									block.artificialLight = nData.emittedLight;
									push                  = true; // propagation target: self
								} // else "if block is light emitter" already covered by 'push' init at "emit > 0"

								if (nData.isTransparent) {
									if (nb.artificialLight - 1 > block.artificialLight) {
										block.artificialLight = nb.artificialLight - 1;
										push                  = true; // propagation target: self
									}
									else if (block.artificialLight - 1 > nb.artificialLight) push = true; // propagation target: neighboor

									if (value) {
										if (14 > nb.naturalLight) push = true; // propagation target: neighboor
									}
									else if (nb.naturalLight > 1) {
										block.naturalLight = nb.naturalLight - 1;
										push               = true; // propagation target: self
									}
								}
							}
						}

						if (!p.z) {
							if (auto & neighboorChunk = getNeighboorChunk(SOUTH); neighboorChunk.id) {
								if (needRecache) neighboorChunk.recache = true; // Always recache if light changed even if neighboor is not altered

								Block nb     = neighboorChunk.blocks[p.y][p.x][15];
								auto & nData = blockData[nb.type];

								if (nData.emittedLight > block.artificialLight) {
									block.artificialLight = nData.emittedLight;
									push                  = true; // propagation target: self
								} // else "if block is light emitter" already covered by 'push' init at "emit > 0"

								if (nData.isTransparent) {
									if (nb.artificialLight - 1 > block.artificialLight) {
										block.artificialLight = nb.artificialLight - 1;
										push                  = true; // propagation target: self
									}
									else if (block.artificialLight - 1 > nb.artificialLight) push = true; // propagation target: neighboor

									if (value) {
										if (14 > nb.naturalLight) push = true; // propagation target: neighboor
									}
									else if (nb.naturalLight > 1) {
										block.naturalLight = nb.naturalLight - 1;
										push               = true; // propagation target: self
									}
								}
							}
						}
						else if (p.z == 15) {
							if (auto & neighboorChunk = getNeighboorChunk(NORTH); neighboorChunk.id) {
								if (needRecache) neighboorChunk.recache = true; // Always recache if light changed even if neighboor is not altered

								Block nb     = neighboorChunk.blocks[p.y][p.x][0];
								auto & nData = blockData[nb.type];

								if (nData.emittedLight > block.artificialLight) {
									block.artificialLight = nData.emittedLight;
									push                  = true; // propagation target: self
								} // else "if block is light emitter" already covered by 'push' init at "emit > 0"

								if (nData.isTransparent) {
									if (nb.artificialLight - 1 > block.artificialLight) {
										block.artificialLight = nb.artificialLight - 1;
										push                  = true; // propagation target: self
									}
									else if (block.artificialLight - 1 > nb.artificialLight) push = true; // propagation target: neighboor

									if (value) {
										if (14 > nb.naturalLight) push = true; // propagation target: neighboor
									}
									else if (nb.naturalLight > 1) {
										block.naturalLight = nb.naturalLight - 1;
										push               = true; // propagation target: self
									}
								}
							}
						}
					}
					else { // sun light is blocked
						if (value) lightMap[p.x][p.z] = p.y;
						value = 0;
						lightQueue.emplace(p.y + 1, p.x, p.z); // the block above will be pushed
					}

					if (push) lightQueue.insert(p);
				}
			}
		}

		for (p.x = 0; p.x < 16; p.x++) {
			for (p.z = 0; p.z < 16; p.z++) {
				u8 high = lightMap[p.x][p.z]; // get max light unaligned high

				if (p.x) high = std::max(high, lightMap[p.x - 1][p.z]);
				if (p.x < 15) high = std::max(high, lightMap[p.x + 1][p.z]);
				if (p.z) high = std::max(high, lightMap[p.x][p.z - 1]);
				if (p.z < 15) high = std::max(high, lightMap[p.x][p.z + 1]);

				for (p.y = lightMap[p.x][p.z] + 1; p.y <= high; p.y++) lightQueue.insert(p); // propagate unaligned light to adjacent columns
			}
		}

		propagate(false);
	}

private:
	/**
	 * @brief push all the 6 neighboors of the specified block to the specified queue
	 * @param[in] bcoord the block to push the neighboors (must be normalized)
	 * @param[in] shadow if true, the block will be pushed to the shadowQueue, else to the lightQueue
     * */
	void pushNeighboor(const BlockCoord & bcoord, bool shadow);

	/**
	 * @brief flush the specified queue througth the light propagation system
	 * @param[in] shadow if true, the shadowQueue will be propagated, else the lightQueue
	 * @note shadow propagation must be followed manually by a light propagation
	 * @note may only be called if the target queue is not empty (safe too otherwise, just waste of time)
	 * @note both queue accept all block type, but the shadow requires the 'old' light value to be kept
	 * @note shadow propagation updates the current block, while
	 * the light propagation updates the neighboors of the current block
	 */
	void propagate(bool shadow);
};
