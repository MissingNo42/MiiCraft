//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_MATRIX_H
#define MIICRAFT_MATRIX_H

#include <ogc/gu.h>

void MtxRotAxis(Mtx m, guVector axis, float rad);
void VecRotAxis(guVector * v, guVector axis, float rad);

#endif //MIICRAFT_MATRIX_H
