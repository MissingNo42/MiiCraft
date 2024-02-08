//
// Created by Romain on 07/02/2024.
//
#include "engine/render/bloc.h"

#define BLOC_FACE_TOP 0
#define BLOC_FACE_BOTTOM 1
#define BLOC_FACE_RIGHT 2
#define BLOC_FACE_LEFT 3
#define BLOC_FACE_FRONT 4
#define BLOC_FACE_BACK 5

BlocData blocData[7] {
        {
            //Air
            .x = { 0, 0, 0, 0, 0, 0 },
            .y = { 0, 0, 0, 0, 0, 0 }
        },
		{
            //Dirt
            .x = { OFFSET, OFFSET, OFFSET, OFFSET, OFFSET, OFFSET },
			.y = { 0, 0, 0, 0, 0, 0 }
		},
		{
            //Grass
			.x = { 0, OFFSET, 0, 0, 0, 0 },
			.y = { 0, 0, OFFSET, OFFSET, OFFSET, OFFSET }
		},
		{
            //Stone
			.x = { OFFSET * 5, OFFSET *5, OFFSET*5, OFFSET*5, OFFSET*5, OFFSET*5},
			.y = { 0, 0, 0, 0, 0, 0 }
		},
		{
            //Bedrock
			.x = { OFFSET * 6, OFFSET * 6, OFFSET * 6, OFFSET * 6, OFFSET * 6, OFFSET * 6 },
			.y = { 0, 0, 0, 0, 0, 0 }
		},
        {
            //Log
            .x = { OFFSET * 3, OFFSET * 3, OFFSET * 2, OFFSET * 2, OFFSET * 2, OFFSET * 2 },
            .y = { 0, 0, 0, 0, 0, 0 }
        },
        {
            //Leaves
            .x = { OFFSET * 4, OFFSET * 4, OFFSET * 4, OFFSET * 4, OFFSET * 4, OFFSET * 4 },
            .y = { OFFSET, OFFSET, OFFSET, OFFSET, OFFSET, OFFSET }
        }
};
