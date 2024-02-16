//
// Created by Romain on 07/02/2024.
//

#ifndef MIICRAFT_BLOC_H
#define MIICRAFT_BLOC_H

#define OFFSET 0.03125f

#define BLOC_FACE_TOP 0
#define BLOC_FACE_BOTTOM 1
#define BLOC_FACE_RIGHT 2
#define BLOC_FACE_LEFT 3
#define BLOC_FACE_FRONT 4
#define BLOC_FACE_BACK 5

#define BLOCK_COORD(x) ((x) * OFFSET)
#define BLOCK_COORDS(a, b, c, d, e, f) { BLOCK_COORD(a), BLOCK_COORD(b), BLOCK_COORD(c), BLOCK_COORD(d), BLOCK_COORD(e), BLOCK_COORD(f) }


struct BlocData {
	float x[6];
	float y[6];
};

extern BlocData blocData[];

#endif //MIICRAFT_BLOC_H
