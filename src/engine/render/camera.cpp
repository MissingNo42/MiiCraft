//
// Created by Romain on 07/02/2024.
//

#include "engine/render/camera.h"
#include "engine/render/renderer.h"

Camera::Camera(f32 fov, f32 min, f32 max) {
	guPerspective(perspective, fov, (f32)Renderer::rmode->fbWidth / Renderer::rmode->xfbHeight, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
	
	guLookAt(viewMatrix, &pos, &up, &look);
}

