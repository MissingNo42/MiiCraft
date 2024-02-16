//
// Created by Romain on 07/02/2024.
//

#include <cmath>
#include <cstdio>
#include "engine/render/camera.h"
#include "engine/render/renderer.h"

Camera::Camera(f32 fov, f32 min, f32 max) : angleY(0){
    f32 ratio = (f32)Renderer::rmode->fbWidth / (f32)Renderer::rmode->xfbHeight;
    guPerspective(perspective, fov, ratio, min, max);
    guOrtho(ortho, 1, -1, -1, 1, 0, 300);
    guLookAt(view3D, &pos, &up, &look);

    guMtxIdentity(view2D);
    guMtxIdentity(view2Dsquare);
    guMtxRowCol(view2Dsquare, 1, 1) = ratio;

    GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
}

void Camera::update(bool applyTransform) {
    guVector pl = {pos.x + look.x, pos.y + look.y, pos.z + look.z};
    guLookAt(view3D, &pos, &up, &pl);
    if (applyTransform) GX_LoadPosMtxImm(view3D, GX_PNMTX0);
}

void Camera::applyTransform() {
    GX_LoadPosMtxImm(view3D, GX_PNMTX0);
}

void Camera::rotateH(f32 rad) {
    // Calculate the right axis (cross product of look-at and up)
    guVector right, rotup;
    guVecCross(&up, &look, &right);
    guVecCross(&look, &right, &rotup);

    VecRotAxis(&look, rotup, rad);
}

void Camera::rotateV(f32 rad) {
    //printf("angleY + rad: %f\r", angleY + rad);
    if(angleY + rad < 88 && angleY + rad > -88){
        // Calculate the right axis (cross product of look-at and up)
        guVector right;
        guVecCross(&up, &look, &right);

        VecRotAxis(&look, right, rad);
        angleY += rad;
    }

}

//void Camera::goUp(int speed, bool collision, World& w) {
//    if ( collision ){
//        coord.y += 1;
//        if (w.getBlockAt(coord).type == BlockType::Air)
//            renderer.camera.pos.y += 0.1;
//    }
//    else
//        renderer.camera.pos.y += 0.1;
//}


