//
// Created by paulo on 06/02/2024.
//

#include "world/world.h"
#include "render/block.h"

PerlinWorldGenerator World::gen;
std::map<ChunkCoord, u16> World::loadedChunk;
std::set<ChunkCoord> World::savedChunk;

//VerticalChunk World::chunkSlots[LOADED_CHUNKS];
VerticalChunk * World::chunkSlots;
u16 World::usedSlots = 0;

Block World::getBlockAt(BlockCoord coord) {
	ChunkCoord chunk_pos = coord.toChunkCoord();
	try {
		return chunkSlots[loadedChunk.at(chunk_pos)].GetBlock(coord);
	} catch (...) {
		return {BlockType::Air, {0}};
	}
}

BlockType World::getBlockTypeAt(BlockCoord coord) {
	ChunkCoord chunk_pos = coord.toChunkCoord();
	try {
		return chunkSlots[loadedChunk.at(chunk_pos)].GetBlockType(coord);
	} catch (...) {
		return BlockType::Air;
	}
}

VerticalChunk &World::getChunkAt(ChunkCoord pos, bool generate) {
	try {
		return chunkSlots[loadedChunk.at(pos)];
	} catch (...) {
		if (generate) return requestChunk(pos);
		return chunkSlots[EMPTY_CHUNK];
	}
}

void World::setBlockAt(BlockCoord coord, Block block, bool calculLight) {
	ChunkCoord chunk_pos = coord.toChunkCoord();
	VerticalChunk &c = chunkSlots[loadedChunk[chunk_pos]];
	c.SetBlock(coord, block);
	
	if (calculLight) {
		resetLight(c);
	}
}

void World::setBlockTypeAt(BlockCoord coord, BlockType block, bool calculLight) {
	ChunkCoord chunk_pos = coord.toChunkCoord();
	VerticalChunk &c = chunkSlots[loadedChunk[chunk_pos]];
	c.SetBlockType(coord, block);
	
	if (calculLight) {
		resetLight(c);
	}
}

void World::setNeighboors(VerticalChunk &chunk) {
	ChunkCoord coord(chunk.coord.x + 1, chunk.coord.y);
	
	if (loadedChunk.find(coord) != loadedChunk.end()) {
		u16 id = loadedChunk[coord];
		chunk.SetNeighboor(Direction::EAST, id);
		chunkSlots[id].SetNeighboor(Direction::WEST, chunk.id);
	}
	
	coord.x -= 2;
	if (loadedChunk.find(coord) != loadedChunk.end()) {
		u16 id = loadedChunk[coord];
		chunk.SetNeighboor(Direction::WEST, id);
		chunkSlots[id].SetNeighboor(Direction::EAST, chunk.id);
	}
	
	coord.x++;
	coord.y++;
	if (loadedChunk.find(coord) != loadedChunk.end()) {
		u16 id = loadedChunk[coord];
		chunk.SetNeighboor(Direction::NORTH, id);
		chunkSlots[id].SetNeighboor(Direction::SOUTH, chunk.id);
	}
	
	coord.y -= 2;
	if (loadedChunk.find(coord) != loadedChunk.end()) {
		u16 id = loadedChunk[coord];
		chunk.SetNeighboor(Direction::SOUTH, id);
		chunkSlots[id].SetNeighboor(Direction::NORTH, chunk.id);
	}
}


void World::requestChunks(ChunkCoord pos, s32 range) {
	for (s32 x = pos.x - range; x <= pos.x + range; x++) {
		for (s32 y = pos.y - range; y <= pos.y + range; y++) {
			requestChunk({x, y});
		}
	}
}

VerticalChunk &World::requestChunk(ChunkCoord pos) {
	try {
		return chunkSlots[loadedChunk.at(pos)];
	} catch (...) {
		u16 slot = getFreeSlot();
		if (slot > 0) {
			VerticalChunk &vc = chunkSlots[slot];
			vc.coord = pos;
			vc.recache = 1;
			vc.loaded = 1;
			loadedChunk[pos] = slot;
			usedSlots++;
			setNeighboors(vc);
			//vc.fillWith(WoodOak); // TODO: check WTF is that?
			gen.generateChunk(vc);
			vc.dirty = 0; // TODO: check this cause no issue ( 0-> no save for non-edited chunk, intended behavior?)
			return vc;
		} else {
			return chunkSlots[EMPTY_CHUNK];    //TODO: save chunk to disk and release a slot
		}
	}
}


void World::resetLight(
		VerticalChunk &c) { // TODO: when destroying artificial light, reset artificial but natural light only if needed
	BlockCoord p;
	
	c.recache = 1;
	
	// Manually apply sun light on each COLUMNS
	for (p.x = 0; p.x < 16; p.x++) {
		for (p.z = 0; p.z < 16; p.z++) {
			
			u8 value = 15;
			c.lightMap[p.x][p.z] = 127;
			
			for (p.y = 127; p.y >= 0; p.y--) {
				
				auto &block = c.blocks[p.x][p.y][p.z];
				auto &data = blockData[block.type];
				bool push = data.emittedLight > 0; // push if the block is a light source
				
				if (data.isTransparent) {
					
					if (data.isLightFilter) { // sun light is soft-blocked
						if (value) c.lightMap[p.x][p.z] = p.y;
						value = 0;
						c.lightQueue.emplace(p.x, p.y + 1, p.z); // the block above will be pushed
					}
					
					block.naturalLight = value;
					block.artificialLight = data.emittedLight; // not intended to be propagated but to light adjacent blocks if transparent too
					
					
					if (!p.x) {
						auto &neighboorChunk = c.GetNeighboorChunk(Direction::WEST);
						if (neighboorChunk.id) {
							Block nb = neighboorChunk.GetBlock({15, p.y, p.z});
							auto &nData = blockData[nb.type];
							
							if (nData.emittedLight > block.artificialLight) {
								block.artificialLight = nData.emittedLight;
								push = true; // propagation target: self
							}
							
							if (nData.isTransparent) {
								if (nb.artificialLight - 1 > block.artificialLight) {
									block.artificialLight = nb.artificialLight - 1;
									push = true; // propagation target: self
								}
								
								if (value) {
									if (14 > nb.naturalLight) push = true; // propagation target: neighboor
								} else {
									if (nb.naturalLight > 1) {
										block.naturalLight = nb.naturalLight - 1;
										push = true; // propagation target: self
									}
								}
							}
							
						}
					} else if (p.x == 15) {
						auto &neighboorChunk = c.GetNeighboorChunk(Direction::EAST);
						if (neighboorChunk.id) {
							Block nb = neighboorChunk.GetBlock({0, p.y, p.z});
							auto &nData = blockData[nb.type];
							
							if (nData.emittedLight > block.artificialLight) {
								block.artificialLight = nData.emittedLight;
								push = true; // propagation target: self
							}
							
							if (nData.isTransparent) {
								if (nb.artificialLight - 1 > block.artificialLight) {
									block.artificialLight = nb.artificialLight - 1;
									push = true; // propagation target: self
								}
								
								if (value) {
									if (14 > nb.naturalLight) push = true; // propagation target: neighboor
								} else {
									if (nb.naturalLight > 1) {
										block.naturalLight = nb.naturalLight - 1;
										push = true; // propagation target: self
									}
								}
							}
						}
					}
					
					if (!p.z) {
						auto &neighboorChunk = c.GetNeighboorChunk(Direction::SOUTH);
						if (neighboorChunk.id) {
							Block nb = neighboorChunk.GetBlock({p.x, p.y, 15});
							auto &nData = blockData[nb.type];
							
							if (nData.emittedLight > block.artificialLight) {
								block.artificialLight = nData.emittedLight;
								push = true; // propagation target: self
							}
							
							if (nData.isTransparent) {
								if (nb.artificialLight - 1 > block.artificialLight) {
									block.artificialLight = nb.artificialLight - 1;
									push = true; // propagation target: self
								}
								
								if (value) {
									if (14 > nb.naturalLight) push = true; // propagation target: neighboor
								} else {
									if (nb.naturalLight - 1 > block.naturalLight) {
										block.naturalLight = nb.naturalLight - 1;
										push = true; // propagation target: self
									}
								}
							}
						}
					} else if (p.z == 15) {
						auto &neighboorChunk = c.GetNeighboorChunk(Direction::NORTH);
						if (neighboorChunk.id) {
							Block nb = neighboorChunk.GetBlock({p.x, p.y, 0});
							auto &nData = blockData[nb.type];
							
							if (nData.emittedLight > block.artificialLight) {
								block.artificialLight = nData.emittedLight;
								push = true; // propagation target: self
							}
							
							if (nData.isTransparent) {
								if (nb.artificialLight - 1 > block.artificialLight) {
									block.artificialLight = nb.artificialLight - 1;
									push = true; // propagation target: self
								}
								
								if (value) {
									if (14 > nb.naturalLight) push = true; // propagation target: neighboor
								} else {
									if (nb.naturalLight - 1 > block.naturalLight) {
										block.naturalLight = nb.naturalLight - 1;
										push = true; // propagation target: self
									}
								}
							}
						}
					}
				}
				else { // sun light is blocked
					if (value) c.lightMap[p.x][p.z] = p.y;
					value = 0;
					c.lightQueue.emplace(p.x, p.y + 1, p.z); // the block above will be pushed
				}
				
				if (push) {
					c.lightQueue.push(p);
				}
			}
		}
	}
	
	
	for (p.x = 0; p.x < 16; p.x++) {
		
		for (p.z = 0; p.z < 16; p.z++) {
			
			u8 high = c.lightMap[p.x][p.z]; // get max light unaligned high
			
			if (p.x) high = std::max(high, c.lightMap[p.x - 1][p.z]);
			else high = std::max(high, c.GetNeighboorChunk(Direction::WEST).lightMap[15][p.z]);
			
			if (p.x < 15) high = std::max(high, c.lightMap[p.x + 1][p.z]);
			else high = std::max(high, c.GetNeighboorChunk(Direction::EAST).lightMap[0][p.z]);
			
			if (p.z) high = std::max(high, c.lightMap[p.x][p.z - 1]);
			else high = std::max(high, c.GetNeighboorChunk(Direction::SOUTH).lightMap[p.x][15]);
			
			if (p.z < 15) high = std::max(high, c.lightMap[p.x][p.z + 1]);
			else high = std::max(high, c.GetNeighboorChunk(Direction::NORTH).lightMap[p.x][0]);
			
			for (p.y = c.lightMap[p.x][p.z] + 1; p.y <= high; p.y++) { // propagate unaligned light to adjacent columns
				c.lightQueue.push(p);
			}
		}
	}
	
	//while (!c.lightQueue.empty())
	//	c.lightQueue.pop();
	propagateLight(c);
}


void World::propagateLight(VerticalChunk &c) {
	
	std::set<BlockCoord> toPropagate; // TODO check with unordered_set
	std::set<BlockCoord> toPropagateWest;
	std::set<BlockCoord> toPropagateEast;
	std::set<BlockCoord> toPropagateNorth;
	std::set<BlockCoord> toPropagateSouth;
	
	auto &westChunk = c.GetNeighboorChunk(Direction::WEST);
	auto &eastChunk = c.GetNeighboorChunk(Direction::EAST);
	auto &northChunk = c.GetNeighboorChunk(Direction::NORTH);
	auto &southChunk = c.GetNeighboorChunk(Direction::SOUTH);
	
	static BlockType BLOCK = BlockType::LeaveAcacia;
	BLOCK = static_cast<BlockType>(BLOCK + 1);
	if (BLOCK > BlockType::LeaveSnow) BLOCK = BlockType::LeaveAcacia;
	
	if (!c.lightQueue.empty()){
		c.recache = 1; // will need a re-render, but not a re-save
	}
	
	printf("START\r");
	goto propagate; // jump the setup push for the first loop
	
	while (!toPropagate.empty()) {
		// Push
		for (auto p: toPropagate) {
			c.lightQueue.push(p);
		}
		
		toPropagate.clear();
		
		printf("LOOP\r");
		
		// Propagate
		propagate:
		while (!c.lightQueue.empty()) {
			BlockCoord p = c.lightQueue.front();
			c.lightQueue.pop();
			
			if (p.x < 0 || p.x > 15 || p.y < 0 || p.y > 127 || p.z < 0 || p.z > 15) {
				printf("ERROR: %d %d %d\r", p.x, p.y, p.z);
				exit(1);
			}
			const Block block = c.blocks[p.x][p.y][p.z];
			auto &data = blockData[block.type];
			
			
			
			u8 nLight = (data.isTransparent && block.naturalLight) ? block.naturalLight - 1 : 0;
			u8 aLight = std::max(data.isTransparent ? block.artificialLight - 1 : 0, (s32) data.emittedLight);
			
			if (p.y < 127) { // y++
				Block &nb = c.blocks[p.x][p.y + 1][p.z];
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nLight) {
						nb.naturalLight = nLight;
						toPropagate.emplace(p.x, p.y + 1, p.z);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagate.emplace(p.x, p.y + 1, p.z);
					}
				}
			}
			
			if (p.y) { // y--
				Block &nb = c.blocks[p.x][p.y - 1][p.z];
				
				s32 nl = (data.isLightFilter || !data.isTransparent || block.naturalLight < 15) ? nLight
				                                                                                : block.naturalLight;
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nl) {
						nb.naturalLight = nl;
						toPropagate.emplace(p.x, p.y - 1, p.z);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagate.emplace(p.x, p.y - 1, p.z);
					}
				}
			}
			
			if (p.x) { // x--
				Block &nb = c.blocks[p.x - 1][p.y][p.z];
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nLight) {
						nb.naturalLight = nLight;
						toPropagate.emplace(p.x - 1, p.y, p.z);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagate.emplace(p.x - 1, p.y, p.z);
					}
				}
			} else if (westChunk.id) {
				Block &nb = westChunk.blocks[15][p.y][p.z];
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nLight) {
						nb.naturalLight = nLight;
						toPropagateWest.emplace(15, p.y, p.z);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagateWest.emplace(15, p.y, p.z);
					}
				}
			}
			
			if (p.x < 15) { // x++
				Block &nb = c.blocks[p.x + 1][p.y][p.z];
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nLight) {
						nb.naturalLight = nLight;
						toPropagate.emplace(p.x + 1, p.y, p.z);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagate.emplace(p.x + 1, p.y, p.z);
					}
				}
			} else if (eastChunk.id) {
				Block &nb = eastChunk.blocks[0][p.y][p.z];
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nLight) {
						nb.naturalLight = nLight;
						toPropagateEast.emplace(0, p.y, p.z);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagateEast.emplace(0, p.y, p.z);
					}
				}
			}
			
			if (p.z) { // z--
				Block &nb = c.blocks[p.x][p.y][p.z - 1];
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nLight) {
						nb.naturalLight = nLight;
						toPropagate.emplace(p.x, p.y, p.z - 1);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagate.emplace(p.x, p.y, p.z - 1);
					}
				}
			} else if (southChunk.id) {
				Block &nb = southChunk.blocks[p.x][p.y][15];
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nLight) {
						nb.naturalLight = nLight;
						toPropagateSouth.emplace(p.x, p.y, 15);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagateSouth.emplace(p.x, p.y, 15);
					}
				}
			}
			
			if (p.z < 15) { // z++
				Block &nb = c.blocks[p.x][p.y][p.z + 1];
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nLight) {
						nb.naturalLight = nLight;
						toPropagate.emplace(p.x, p.y, p.z + 1);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagate.emplace(p.x, p.y, p.z + 1);
					}
				}
			} else if (northChunk.id) {
				Block &nb = northChunk.blocks[p.x][p.y][0];
				
				if (blockData[nb.type].isTransparent) {
					if (nb.naturalLight < nLight) {
						nb.naturalLight = nLight;
						toPropagateNorth.emplace(p.x, p.y, 0);
					}
					if (nb.artificialLight < aLight) {
						nb.artificialLight = aLight;
						toPropagateNorth.emplace(p.x, p.y, 0);
					}
				}
			}
			
			
			//c.blocks[p.x][p.y][p.z].type = BLOCK;
		}
	}
	
	printf("END\r");
	
	// propagate light to neighboors
	
	bool runWest = !toPropagateWest.empty();
	bool runEast = !toPropagateEast.empty();
	bool runNorth = !toPropagateNorth.empty();
	bool runSouth = !toPropagateSouth.empty();
	
	if (runWest) {
		for (auto &p: toPropagateWest) {
			westChunk.lightQueue.push(p);
		}
		
		toPropagateWest.clear();
	}
	
	if (runEast) {
		for (auto &p: toPropagateEast) {
			eastChunk.lightQueue.push(p);
		}
		
		toPropagateEast.clear();
	}
	
	if (runNorth) {
		for (auto &p: toPropagateNorth) {
			northChunk.lightQueue.push(p);
		}
		
		toPropagateNorth.clear();
	}
	
	if (runSouth) {
		for (auto &p: toPropagateSouth) {
			southChunk.lightQueue.push(p);
		}
		
		toPropagateSouth.clear();
	}
	
	// recursion depth is limited to 5 (light level decreasing, chunk's limit snake pattern)
	if (runWest)
		propagateLight(westChunk);
	if (runEast)
		propagateLight(eastChunk);
	if (runNorth)
		propagateLight(northChunk);
	if (runSouth)
		propagateLight(southChunk);
}
