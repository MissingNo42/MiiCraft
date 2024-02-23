//
// Created by Romain on 07/02/2024.
//

#include <cmath>
#include <cstdio>
#include <malloc.h>
#include "engine/render/camera.h"
#include "engine/render/renderer.h"

Camera::Camera(f32 fov, f32 min, f32 max) : angleY(0), fovy(fov * M_PI / 180.f), min(min), max(max) {
    f32 ratio = (f32)Renderer::rmode->fbWidth / (f32)Renderer::rmode->xfbHeight;
	fovx = atan(tan(fovy) * ratio);
	radius = tan(fovy) * max;
    guPerspective(perspective, fov, ratio, min, max);
    guOrtho(ortho, 1, -1, -1, 1, 0, 300);
    guLookAt(view3D, &pos, &up, &look);

    guMtxIdentity(view2D);
    guMtxIdentity(view2Dsquare);
    guMtxRowCol(view2Dsquare, 1, 1) = ratio;

    GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
	
	/// Fog setup
	/*
    GXColor greyBackground = {0xd0, 0xd0, 0xe0, 0xff};
    GX_SetFog(GX_FOG_PERSP_LIN, 900, 990, 2, 1200, greyBackground);

    GXFogAdjTbl* fogTable = (GXFogAdjTbl*)memalign(32, 8 * sizeof(GXFogAdjTbl));

    f32 projmtx[4][4] = {
            {2.0f / (f32)Renderer::rmode->fbWidth, 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / (f32)Renderer::rmode->efbHeight, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {-1.0f, -1.0f, 0.0f, 1.0f}
    };

    GX_InitFogAdjTable(fogTable, Renderer::rmode->fbWidth, projmtx);
    GX_SetFogRangeAdj(true, 500, fogTable);*/
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
    //if(angleY + rad < 88 && angleY + rad > -88){
        // Calculate the right axis (cross product of look-at and up)
        guVector right;
        guVecCross(&up, &look, &right);

        VecRotAxis(&look, right, rad);
        //angleY += rad;
    //}

}

u8 Camera::isVisible(const guVector &p) {
	/*
Calculate the vector from the center of the cone to the query point.
	 Normalize the vector to be of length 1,
	 Take the center vector of the cone and normalize
	 this as well to the length of 1.
Now take the dot product between the vectors.
	 The dot product between two normalized vectors is the cosinus
	 of the angle between them. Take the arccos (acos in most languages)
	 of the dot product and you'll get the angle. compare this angle to the cone's
	 angle (half angle in your description). if its lower, then point in question is inside the cone.
	 * */
	
	guVector pt = {p.x - pos.x, 0, p.z - pos.z},
	         ct = {look.x, 0, look.z};
	
	guVecNormalize(&pt);
	guVecNormalize(&ct);
	f32 a = guVecDotProduct(&pt, &ct);
	f32 b = acosf(a);
	return b < fovx;
	
	/*
	guVector s, m;
	guVecSub(&p, &pos, &s);
	f32 cone_dist = guVecDotProduct(&s, &look);
	
	if (0.0f > cone_dist  || cone_dist > max) return 0;
	f32 cone_radius = (cone_dist / max) * radius;
	guVecScale(&look, &m, cone_dist);
	guVecSub(&s, &m, &s);
	return s.x * s.x + s.y * s.y + s.z * s.z < cone_radius * cone_radius;*/
}

u8 Camera::isChunkVisible(s16 x, s16 z) {
	guVector p = {(f32)x * 16, 0, (f32)z * 16},
	         a = {p.x, 0, p.z + 16},
			 b = {p.x + 16, 0, p.z + 16},
			 c = {p.x + 16, 0, p.z};
	
	if (isVisible(p) || isVisible(a) || isVisible(b) || isVisible(c)) return 1;
	
	return 0;
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


