//
// Created by Romain on 05/02/2024.
//

#include "utils/matrix.h"


void MtxRotAxis(Mtx m, Mtx out, guVector axis, float deg){
	Mtx i;
	guMtxIdentity(i);
	guMtxRotAxisDeg(i, &axis, deg);
	guMtxConcat(i, m, out);
}

void VecRotAxis(guVector * v, guVector axis, float deg){
	Mtx m;
	guMtxIdentity(m);
	guMtxRotAxisDeg(m, &axis, deg);
	guVecMultiply(m, v, v);
}
