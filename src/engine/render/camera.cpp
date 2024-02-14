//
// Created by Romain on 07/02/2024.
//

#include <cmath>
#include "engine/render/camera.h"
#include "engine/render/renderer.h"

Camera::Camera(f32 fov, f32, f32) {
	guPerspective(perspective, fov, (f32)Renderer::rmode->fbWidth / (f32)Renderer::rmode->xfbHeight, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
	
	guLookAt(viewMatrix, &pos, &up, &look);
}

void Camera::update(bool applyTransform = true) {
    guVector pl = {pos.x + look.x, pos.y + look.y, pos.z + look.z};
    guLookAt(viewMatrix, &pos, &up, &pl);
    if (applyTransform) GX_LoadPosMtxImm(viewMatrix, GX_PNMTX0);
}

void Camera::applyTransform() {
    GX_LoadPosMtxImm(viewMatrix, GX_PNMTX0);
}

void Camera::rotateH(f32 rad) {
    // Calculate the right axis (cross product of look-at and up)
    guVector right, rotup;
    guVecCross(&up, &look, &right);
    guVecCross(&look, &right, &rotup);


    VecRotAxis(&look, rotup, rad);
}

void Camera::rotateV(f32 rad) {
    // Calculate the right axis (cross product of look-at and up)
    guVector right;
    guVecCross(&up, &look, &right);

    VecRotAxis(&look, right, rad);
}


