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

BlocData blocData[4] {
		{
			.x = { 0, OFFSET, OFFSET, 0, 0, OFFSET},
			.y = { 0, 0, 0, 0, 0, 0 }
		},
		{
			.x = { 0, OFFSET, OFFSET, OFFSET, OFFSET, OFFSET },
			.y = { 0, 0, 0, 0, 0, 0 }
		},
		{
			.x = { OFFSET, OFFSET, OFFSET, OFFSET, OFFSET, OFFSET },
			.y = { 0, 0, 0, 0, 0, 0 }
		},
		{
			.x = { OFFSET * 3, OFFSET * 3, OFFSET * 2, OFFSET * 2, OFFSET * 2, OFFSET * 2 },
			.y = { 0, 0, 0, 0, 0, 0 }
		}
};
