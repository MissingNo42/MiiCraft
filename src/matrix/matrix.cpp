//
// Created by Romain on 05/02/2024.
//

#include "utils/matrix.h"


void MtxRotAxis(Mtx m, Mtx out, guVector axis, float rad){
	Mtx i;
	guMtxIdentity(i);
	guMtxRotAxisDeg(i, &axis, rad);
	guMtxConcat(i, m, out);
}

void VecRotAxis(guVector * v, guVector axis, float rad){
	Mtx m;
	guMtxIdentity(m);
	guMtxRotAxisDeg(m, &axis, rad);
	guVecMultiply(m, v, v);
}
