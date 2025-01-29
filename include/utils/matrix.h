#pragma once

#include <ogc/gu.h>

inline void MtxRotAxis(Mtx m, Mtx out, guVector axis, float deg){
	Mtx i;
	guMtxIdentity(i);
	guMtxRotAxisDeg(i, &axis, deg);
	guMtxConcat(i, m, out);
}

inline void VecRotAxis(guVector * v, guVector axis, float deg){
	Mtx m;
	guMtxIdentity(m);
	guMtxRotAxisDeg(m, &axis, deg);
	guVecMultiply(m, v, v);
}
