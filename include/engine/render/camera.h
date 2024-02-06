//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_CAMERA_H
#define MIICRAFT_CAMERA_H

#include <gccore.h>
#include <ogc/gu.h>

#include "utils/matrix.h"

class Camera {
	guVector up = {0.0F, 1.0F, 0.0F};
	
public:
	Mtx44 viewMatrix, perspective;
	guVector pos = {0.0F, 0.0F, 0.0F},
	look = {0.0F, 0.0F, -1.0F};
	
	/** Constructor
	 * @param fov Field of view (half angle in degrees)
	 * @param min Minimum render distance
	 * @param max Maximum render distance
	 */
	Camera(int fov = 45, f32 min = .1, f32 max = 300) {
		
		guPerspective(perspective, fov, (f32)Renderer::rmode->fbWidth / Renderer::rmode->xfbHeight, 0.1F, 300.0F);
		GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
		
		guLookAt(viewMatrix, &pos, &up, &look);
	}
	
	~Camera() = default;

	void update(bool applyTransform = true) {
		guVector pl = {pos.x + look.x, pos.y + look.y, pos.z + look.z};
		guLookAt(viewMatrix, &pos, &up, &pl);
		if (applyTransform) GX_LoadPosMtxImm(viewMatrix, GX_PNMTX0);
	}
	
	void applyTransform() {
		GX_LoadPosMtxImm(viewMatrix, GX_PNMTX0);
	}
	
	void rotateH(f32 rad) {
	    // Calculate the right axis (cross product of look-at and up)
	    guVector right, rotup;
	    guVecCross(&up, &look, &right);
		guVecCross(&look, &right, &rotup);
		
		VecRotAxis(&look, rotup, rad);
	}
	
	void rotateV(f32 rad) {
	    // Calculate the right axis (cross product of look-at and up)
	    guVector right;
	    guVecCross(&up, &look, &right);
	
		VecRotAxis(&look, right, rad);
	}
};


#endif //MIICRAFT_CAMERA_H
