//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_VERTICALCHUNK_H
#define MIICRAFT_VERTICALCHUNK_H

#include "coord.h"
#include "block.h"
#include <cstdint>


#define CHUNK_NORTH 1
#define CHUNK_SOUTH 3
#define CHUNK_EAST 0
#define CHUNK_WEST 2


class VerticalChunk {

private:
    static inline int cpt = 0;
	
public:
    u16 neighboors[4]{0, 0, 0, 0};
	u8 recache = 1; // 1 if the chunk needs to be recached
	u8 loaded = 0;
	u8 dirty = 0;
	u16 id;
	
	ChunkCoord coord;
    BlockType blocks[16][128][16];

    void SetBlock(BlockCoord coord, BlockType block);
    BlockType GetBlock(BlockCoord coord);
	
    void SetNeighboor(u8 indice, u16 chunk);
    u16 GetNeighboor(int indice);
	VerticalChunk& GetNeighboorChunk(int indice);
	
    void fillWith(BlockType block = BlockType::Bedrock);
	
	u16 Count(BlockType block);
};


#endif //MIICRAFT_VERTICALCHUNK_H
