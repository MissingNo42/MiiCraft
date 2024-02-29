//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_CAMERA_H
#define MIICRAFT_CAMERA_H

#include <gccore.h>
#include <ogc/gu.h>

class Camera {
	
	void rotate();
	
public:
	/// Read only
    f32 angleH, angleV;
	f32 fovy;
	f32 fovx;
	f32 min, max, radius;
    Mtx44 view3D, perspective, ortho, view2D, view2Dsquare;
    guVector up = {0.0F, 1.0F, 0.0F};
    guVector pos = {0.0F, 0.0F, 0.0F},
            look = {0.0F, 0.0F, 1.0F};

	static const inline f32 limitV = 89;
	
    /** Constructor
     * @param fov Field of view (half angle in degrees)
     * @param min Minimum render distance
     * @param max Maximum render distance
     */
    explicit Camera(f32 fov = 45, f32 min = .1, f32 max = 256);

    ~Camera() = default;

    void update(bool applyTransform);

    void applyTransform();

    void applyTransform2D(bool square = true) {
        GX_LoadPosMtxImm(square ? view2Dsquare: view2D, GX_PNMTX0);
    }

    void loadOrtho() {
        GX_LoadProjectionMtx(ortho, GX_ORTHOGRAPHIC);
    }

    void loadPerspective() {
        GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
    }

    void rotateH(f32 deg);

    void rotateV(f32 deg);

    void rotateToH(f32 deg);

    void rotateToV(f32 deg);
	
	u8 isVisible(const guVector& p);
	u8 isChunkVisible(s16 x, s16 z);
};


#endif //MIICRAFT_CAMERA_H
