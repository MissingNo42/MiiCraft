//
// Created by Romain on 07/02/2024.
//

#include <cmath>
#include <cstdio>
#include "engine/render/camera.h"
#include "engine/render/renderer.h"

Camera::Camera(f32 fov, f32 min, f32 max) {
	f32 ratio = (f32)Renderer::rmode->fbWidth / (f32)Renderer::rmode->xfbHeight;
	guPerspective(perspective, fov, ratio, min, max);
	guOrtho(ortho, 1, -1, -1, 1, 0, 300);
	guLookAt(view3D, &pos, &up, &look);
	
	guMtxIdentity(view2D);
	guMtxIdentity(view2Dsquare);
	guMtxRowCol(view2Dsquare, 1, 1) = ratio;
	
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
}

