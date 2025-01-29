#include <cmath>

#include "engine/render/camera.h"
#include "engine/render/renderer.h"
#include "utils/matrix.h"

Camera::Camera(f32 fov) : fovy(fov * (f32)M_PI / 180.f), fovyDefault(fovy) {
	resize(FullScreen);

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


void Camera::resize(Format fm) {
	if (fm == format) return;
	format = fm;

    ratio = (f32)Renderer::rmode->fbWidth / (f32)Renderer::rmode->xfbHeight;
	if (format == SplitTop || format == SplitBottom) { // wide screen
		ratio *= 2;
		fovy = fovyDefault / 2.f;
	} else {
		fovy = fovyDefault;
	}

	fovx = 2 * atanf(tanf(fovy / 2) * ratio);

    guPerspective(perspective, fovy * 180.f / (f32)M_PI,
				  (fm == SplitTop || fm == SplitBottom) ? ratio * 2: ratio,
				  min, max);
    guOrtho(ortho,
			format & SplitTop ? 1: 0,
			format & SplitBottom ? -1: 0,
			format & SplitLeft ? -1 : 0,
			format & SplitRight ? 1 : 0,
			0, max);

    guLookAt(view3D, &pos, &up, &look);

	bool allW = format & SplitLeft && format & SplitRight,
		 allH = format & SplitTop && format & SplitBottom;

    guMtxIdentity(view2D);
	guMtxTransApply(view2D, view2D,
					allW ? 0.f : (format & SplitLeft ? -1.f: 1.f),
					allH ? 0.f : (format & SplitTop ? 1.f: -1.f),
					0.f);
	guMtxScaleApply(view2D, view2D,
					allW && allH ? 1.f: .5f,
					allH ? 1.f: .5f,
					1.f);
	guMtxCopy(view2D, view2Dsquare);

	guMtxRowCol(view2Dsquare, 1, 1) *= ratio;
}

void Camera::update(bool applyTransform) {
    guVector pl = {pos.x + look.x, pos.y + look.y, pos.z + look.z};

    guLookAt(view3D, &pos, &up, &pl);
    if (applyTransform) GX_LoadPosMtxImm(view3D, GX_PNMTX0);
}

void Camera::applyTransform() {
    GX_LoadPosMtxImm(view3D, GX_PNMTX0);
}

void Camera::rotate() {
	guVector line = {0, 0, 1}, right;

    VecRotAxis(&line, up, angleH); // rotH

    guVecCross(&up, &line, &right); // Calculate the right axis (cross product of look-at and up)

    VecRotAxis(&line, right, angleV); // rotV
	look = line;
}

void Camera::rotateH(f32 deg) {
	angleH += deg;
	while (angleH >= 360) angleH -= 360;
	while (angleH < 0) angleH += 360;

	if (angleH < 45) direction = NORTH;
	else if (angleH < 135) direction = EAST;
	else if (angleH < 225) direction = SOUTH;
	else direction = WEST;

	rotate();
}

void Camera::rotateV(f32 deg) {
	angleV += deg;
	if (angleV > limitV || angleV < -limitV) {
		angleV = (deg > 0) ? limitV: -limitV;
	}
	rotate();
}

u8 Camera::isVisible(const guVector &p) {
	guVector pt = {p.x - pos.x, 0, p.z - pos.z},
	         ct = {look.x, 0, look.z};

	guVecNormalize(&pt);
	guVecNormalize(&ct);
	f32 a = guVecDotProduct(&pt, &ct);
	f32 b = acosf(a) * 2;

	return b < fovx * (1 + std::abs(look.y));
}

bool Camera::isChunkVisible(ChunkCoord coord) {
	if (coord == ChunkCoord{(s32)pos.x >> 4, (s32)pos.z >> 4}) return true;

	guVector p = {(f32)coord.x * 16, 0, (f32)coord.z * 16},
	         a = {p.x, 0, p.z + 16},
			 b = {p.x + 16, 0, a.z},
			 c = {b.x, 0, p.z};

	if (isVisible(p) || isVisible(a) || isVisible(b) || isVisible(c)) return true;

	return false;
}

void Camera::applyScissor() const {
	auto W = Renderer::rmode->fbWidth, H = Renderer::rmode->efbHeight;
	GX_SetScissor(format & SplitLeft ? 0: W >> 1,
				  format & SplitTop ? 0: H >> 1,
				  format & SplitRight && format & SplitLeft ? W: W >> 1,
				  format & SplitBottom && format & SplitTop ? H: H >> 1);

	GX_SetViewport(format & SplitLeft ? 0: (f32)W / 2.f,
				  format & SplitTop ? 0: (f32)H / 2.f,
				  format & SplitRight && format & SplitLeft ? (f32)W: (f32)W / 2.f,
				  format & SplitBottom && format & SplitTop ? (f32)H: (f32)H / 2.f, 0, 1);
}



