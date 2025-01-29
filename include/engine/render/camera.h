#pragma once

#include <gccore.h>
#include <ogc/gu.h>
#include "world/coord.h"

class Camera {

	void rotate();

public:
	enum Format {
		QuarterTL = 1,
		QuarterTR = 2,
		QuarterBL = 4,
		QuarterBR = 8,

		SplitTop = 3,
		SplitBottom = 12,

		SplitLeft = 5,
		SplitRight = 10,

		FullScreen = 15
	};

	/// Read only
    f32 angleH = 0, angleV = 0;
	f32 fovy;
	f32 fovx;
	const f32 fovyDefault;
	f32 ratio;
    Mtx44 view3D, perspective, ortho, view2D, view2Dsquare;
    guVector up = {0.0F, 1.0F, 0.0F};
    guVector pos = {0.0F, 0.0F, 0.0F},
            look = {0.0F, 0.0F, 1.0F};
	Direction direction = NORTH;
	Format format;

	static constexpr f32 min = .1, max = 1024.;
	static constexpr f32 skyboxDistance = max / 1.7320508075688772 * 0.95; // sqrt(3) * 95% (margin)
	static constexpr f32 limitV = 89;

    /** Constructor
     * @param fov Field of view (half angle in degrees)
     */
    explicit Camera(f32 fov = 90);

    void update(bool applyTransform);

    void applyTransform();

    void applyTransform2D(const bool square = true) {
        GX_LoadPosMtxImm(square ? view2Dsquare: view2D, GX_PNMTX0);
    }

	void applyScissor() const;

    void loadOrtho() {
        GX_LoadProjectionMtx(ortho, GX_ORTHOGRAPHIC);
    }

    void loadPerspective() {
        GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
    }

	void resize(Format format);

    void rotateH(f32 deg);

    void rotateV(f32 deg);

	u8 isVisible(const guVector& p);
	bool isChunkVisible(ChunkCoord coord);
};
