//
// Created by Romain on 14/02/2024.
//

#include "engine/render/cache.h"

const s8 Normals[] ATTRIBUTE_ALIGN(32) = {
		0, 1, 0,    // 0
		0, -1, 0,   // 1
		1, 0, 0,    // 2
		-1, 0, 0, // 3
		0, 0, 1, // 4
		0, 0, -1, // 5
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const s8 Vertices[] ATTRIBUTE_ALIGN(32) = {
		-1, -1, -1, // 0
		-1, -1, 0, // 1
		-1, 0, -1, // 2
		-1, 0, 0, // 3
		0, -1, -1, // 4
		0, -1, 0, // 5
		0, 0, -1, // 6
		0, 0, 0, // 7
		0, 0, 0, 0, 0, 0, 0, 0
};


// No opti -> 14 fps
