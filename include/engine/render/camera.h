//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_CAMERA_H
#define MIICRAFT_CAMERA_H

#include <gccore.h>
#include <ogc/gu.h>

#include "utils/matrix.h"

class Camera {

public:
	Mtx44 perspective;
    Mtx viewMatrix;
    guVector up = {0.0F, 1.0F, 0.0F};
	guVector pos = {0.0F, 0.0F, 0.0F},
    look = {0.0F, 0.0F, 1.0F};

	/** Constructor
	 * @param fov Field of view (half angle in degrees)
	 * @param min Minimum render distance
	 * @param max Maximum render distance
	 */
	explicit Camera(f32 fov = 45, f32 min = .1, f32 max = 300);
	
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

    static guVector InverseVector(const guVector& v){
        guVector vtemp;
        vtemp.x = - v.x;
        vtemp.y = - v.y;
        vtemp.z = - v.z;

        return vtemp;
    }

    void goLeft(guVector& normalizedLook, int speed) {
        guVector move = {0,0,0};
        guVecCross(&normalizedLook, &up, &move);
        move = InverseVector(move);
        pos.x += move.x/10 * (f32) speed;
        pos.z += move.z/10 * (f32) speed;
    }

    void goRight(guVector& normalizedLook, int speed) {
        guVector move = {0,0,0};
        guVecCross(&normalizedLook, &up, &move);
        pos.x += move.x/10 * (f32) speed;
        pos.z += move.z/10 * (f32) speed;
    }

    void goForward(guVector& normalizedLook, int speed) {
        pos.x += normalizedLook.x/10 * (f32) speed;
        pos.z += normalizedLook.z/10 * (f32) speed;
    }

    void goBackward(guVector& normalizedLook, int speed) {
        guVector move = InverseVector(normalizedLook);
        pos.x += move.x/10 * (f32) speed;
        pos.z += move.z/10 * (f32) speed;
    }
};


#endif //MIICRAFT_CAMERA_H
