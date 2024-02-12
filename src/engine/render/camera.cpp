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

 guVector Camera::InverseVector(const guVector& v){
    guVector vtemp;
    vtemp.x = - v.x;
    vtemp.y = - v.y;
    vtemp.z = - v.z;

    return vtemp;
}

void Camera::goLeft(guVector& normalizedLook, int speed, bool collision, World& w) {
    guVector move = {0,0,0};
    guVecCross(&normalizedLook, &up, &move);
    move = InverseVector(move);
    if (collision) {
        if (w.getBlockAt({(int)ceil(pos.x + move.x), (int) pos.y, (int)ceil(pos.z + move.z)}).type == BlockType::Air
        && w.getBlockAt({(int)ceil(pos.x + move.x), (int) pos.y+1, (int)ceil(pos.z + move.z)}).type == BlockType::Air){
            pos.x += move.x / 10 * (f32) speed;
            pos.z += move.z / 10 * (f32) speed;
        }
    }
    else {
        pos.x += move.x / 10 * (f32) speed;
        pos.z += move.z / 10 * (f32) speed;
    }
    update(true);
}

void Camera::goRight(guVector& normalizedLook, int speed, bool collision, World& w) {
    guVector move = {0,0,0};
    guVecCross(&normalizedLook, &up, &move);
    if (collision) {
        if (w.getBlockAt({(int)ceil( pos.x + move.x), (int) pos.y, (int)ceil(pos.z+move.z)}).type == BlockType::Air
        && w.getBlockAt({(int)ceil( pos.x + move.x), (int) pos.y+1, (int)ceil(pos.z+move.z)}).type == BlockType::Air){
            pos.x += move.x / 10 * (f32) speed;
            pos.z += move.z / 10 * (f32) speed;
        }
    }
    else {
        pos.x += move.x / 10 * (f32) speed;
        pos.z += move.z / 10 * (f32) speed;
    }
    update(true);
}

void Camera::goForward(guVector& normalizedLook, int speed, bool collision, World& w) {
    if (collision) {
        if (w.getBlockAt({(int)ceil(pos.x + look.x), (int) pos.y, (int)ceil(pos.z+look.z)}).type == BlockType::Air
        && w.getBlockAt({(int)ceil(pos.x + look.x), (int) pos.y+1, (int)ceil(pos.z+look.z)}).type == BlockType::Air){
            pos.x += normalizedLook.x/10 * (f32) speed;
            pos.z += normalizedLook.z/10 * (f32) speed;
        }
    }
    else {
        pos.x += normalizedLook.x / 10 * (f32) speed;
        pos.z += normalizedLook.z / 10 * (f32) speed;
    }
    update(true);
}

void Camera::goBackward(guVector& normalizedLook, int speed, bool collision, World& w) {
    guVector move = InverseVector(normalizedLook);
    if (collision) {
        if (w.getBlockAt({(int)ceil( pos.x+1 + move.x), (int) pos.y, (int) ceil(pos.z+move.z)}).type == BlockType::Air
        && w.getBlockAt({(int)ceil( pos.x+1 + move.x), (int) pos.y+1, (int) ceil(pos.z+move.z)}).type == BlockType::Air){
            pos.x += move.x/10 * (f32) speed;
            pos.z += move.z/10 * (f32) speed;
        }
    }
    else {
        pos.x += move.x/10 * (f32) speed;
        pos.z += move.z/10 * (f32) speed;
    }
    update(true);
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


