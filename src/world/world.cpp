//
// Created by paulo on 06/02/2024.
//

#include "world/world.h"
#include "engine/render/block.h"

PerlinWorldGenerator World::gen;
std::map<ChunkCoord, u16> World::loadedChunk;
std::set<ChunkCoord> World::savedChunk;

//Chunk World::chunkSlots[LOADED_CHUNKS];
Chunk * World::chunkSlots;
u16 World::usedSlots = 0;

Block World::getBlockAt(BlockCoord coord) {
	ChunkCoord chunk_pos = coord.toChunkCoord();
	try {
		return chunkSlots[loadedChunk.at(chunk_pos)].blocks[coord.y][coord.x & 15][coord.z & 15];
	} catch (...) {
		return {BlockType::Air, {0}};
	}
}

BlockType World::getBlockTypeAt(BlockCoord coord) {
    return getBlockAt(coord).type;
}

Chunk &World::getChunkAt(ChunkCoord pos, bool generate) {
	try {
		return chunkSlots[loadedChunk.at(pos)];
	} catch (...) {
		if (generate) return requestChunk(pos);
		return chunkSlots[EMPTY_CHUNK];
	}
}

void World::setBlockAt(BlockCoord coord, Block block) {
	ChunkCoord chunk_pos = coord.toChunkCoord();
	Chunk &c = chunkSlots[loadedChunk[chunk_pos]];
	coord.normalized();
	c.setBlock(coord, block);
}

void World::setBlockTypeAt(BlockCoord coord, BlockType block) {
	ChunkCoord chunk_pos = coord.toChunkCoord();
	Chunk &c = chunkSlots[loadedChunk[chunk_pos]];
	coord.normalized();
	c.setBlockType(coord, block);
}

void World::setNeighboors(Chunk &chunk) {
	ChunkCoord coord(chunk.coord.x + 1, chunk.coord.z);

	if (loadedChunk.find(coord) != loadedChunk.end()) {
		u16 id = loadedChunk[coord];
		chunk.setNeighboor(Direction::EAST, id);
		chunkSlots[id].setNeighboor(Direction::WEST, chunk.id);
	}

	coord.x -= 2;
	if (loadedChunk.find(coord) != loadedChunk.end()) {
		u16 id = loadedChunk[coord];
		chunk.setNeighboor(Direction::WEST, id);
		chunkSlots[id].setNeighboor(Direction::EAST, chunk.id);
	}

	coord.x++;
	coord.z++;
	if (loadedChunk.find(coord) != loadedChunk.end()) {
		u16 id = loadedChunk[coord];
		chunk.setNeighboor(Direction::NORTH, id);
		chunkSlots[id].setNeighboor(Direction::SOUTH, chunk.id);
	}

	coord.z -= 2;
	if (loadedChunk.find(coord) != loadedChunk.end()) {
		u16 id = loadedChunk[coord];
		chunk.setNeighboor(Direction::SOUTH, id);
		chunkSlots[id].setNeighboor(Direction::NORTH, chunk.id);
	}
}


void World::requestChunks(ChunkCoord pos, s32 range) {
	for (s32 x = pos.x - range; x <= pos.x + range; x++) {
		for (s32 y = pos.z - range; y <= pos.z + range; y++) {
			printf(">> Requesting chunk %d %d\r", x, y);
			requestChunk({x, y});
			printf("<< Requested chunk %d %d\r", x, y);
		}
	}
}

Chunk &World::requestChunk(ChunkCoord pos) {
	try {
		return chunkSlots[loadedChunk.at(pos)];
	} catch (...) {
		u16 slot = getFreeSlot();
		if (slot > 0) {
			Chunk &vc = chunkSlots[slot];
			vc.coord = pos;
			vc.loaded = 1;
			loadedChunk[pos] = slot;
			usedSlots++;
			setNeighboors(vc);
			vc.fillWith();
			gen.generateChunk(vc);
			return vc;
		} else {
			return chunkSlots[EMPTY_CHUNK];    //TODO: save chunk to disk and release a slot
		}
	}
}


