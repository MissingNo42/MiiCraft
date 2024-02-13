//
// Created by Romain on 07/02/2024.
//
#include "engine/render/block.h"

// TOP 0
// BOTTOM 1
// RIGHT 2
// LEFT 3
// FRONT 4
// BACK 5

BlocData blocData[8] {
        {
            //Air
            .x = { 0, 0, 0, 0, 0, 0 },
            .y = { 0, 0, 0, 0, 0, 0 }
        },
		{
            //Dirt
            .x = BLOCK_COORDS(2, 2, 2, 2, 2, 2),
			.y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
		},
		{
            //Grass
            .x = BLOCK_COORDS(0, 2, 3, 3, 3, 3),
			.y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
		},
		{
            //Stone
            .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
			.y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
		},
		{
            //Bedrock
            .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
            .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
		},
        {
            //Log
            .x = BLOCK_COORDS(5, 5, 4, 4, 4, 4),
            .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        },
        {
            //Leaves
            .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
            .y = BLOCK_COORDS(11, 11, 11, 11, 11, 11)
        },
        {
            //Water
            .x = BLOCK_COORDS(13, 13, 13, 13, 13, 13),
            .y = BLOCK_COORDS(12, 12, 12, 12, 12, 12)
        }
};
