//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_LIGHT_H
#define MIICRAFT_LIGHT_H

#include "render.h"

class Light {
public:
	Mtx perspective;
	GXColor color{ 0xff, 0xff, 0xff, 0xff}, ambient{ 0x80, 0x80, 0x80, 0xFF }, material{ 0x80, 0x80, 0x80, 0xFF };
	
	Light(f32 fov = 45) {
		Mtx translation;
		guLightPerspective(perspective, fov, (f32)rmode->fbWidth / rmode->xfbHeight, 1.05F, 1.0F, 0.0F, 0.0F);
		guMtxTrans(translation, 0.0F, 0.0F, -1.0F);
		guMtxConcat(perspective, translation, perspective);
		GX_LoadTexMtxImm(perspective, GX_TEXMTX0, GX_MTX3x4);
	}
	
	
	void update(Mtx view) // originally written by shagkur
	{
		guVector lpos;
		GXLightObj lobj;
	
		lpos.x = 0;
		lpos.y = 0;
		lpos.z = 2.0f;
	
		guVecMultiply(view, &lpos, &lpos);
	
		GX_InitLightPos(&lobj, lpos.x, lpos.y, lpos.z);
		GX_InitLightColor(&lobj, color);
		GX_LoadLightObj(&lobj, GX_LIGHT0);
	
		// set number of rasterized color channels
		GX_SetNumChans(1);
		GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
		GX_SetChanAmbColor(GX_COLOR0A0, ambient);
		GX_SetChanMatColor(GX_COLOR0A0, material);
	}
};

#endif //MIICRAFT_LIGHT_H
