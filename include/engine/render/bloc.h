//
// Created by Romain on 07/02/2024.
//

#ifndef MIICRAFT_BLOC_H
#define MIICRAFT_BLOC_H

#include <gctypes.h>

#define OFFSET 0.03125f

#define BLOC_FACE_TOP 0 // +y
#define BLOC_FACE_BOTTOM 1 // -y
#define BLOC_FACE_RIGHT 2 // +x
#define BLOC_FACE_LEFT 3 // -x
#define BLOC_FACE_FRONT 4 // +z
#define BLOC_FACE_BACK 5 // -z

#define TXCOORD(x, y) (17 * (x) + (y))

#define BLOCK_COORD(x) (x)
#define BLOCK_COORDS(a, b, c, d, e, f) { BLOCK_COORD(a), BLOCK_COORD(b), BLOCK_COORD(c), BLOCK_COORD(d), BLOCK_COORD(e), BLOCK_COORD(f) }

struct BlocData {
	u16 x[6];
	u16 y[6];
};

extern BlocData blocData[];

#endif //MIICRAFT_BLOC_H
