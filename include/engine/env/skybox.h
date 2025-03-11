#pragma once

#include <bits/algorithmfwd.h>

#include <ogc/gx.h>

#include "texture.h"
#include "engine/render/block.h"
#include "engine/render/camera.h"
#include "engine/render/vertex.h"
#include "utils/matrix.h"

class Environment;


struct SkyBox final {
	constexpr static u32 resolution    = 5;
	constexpr static f32 radius        = 1;//Camera::max * 3. / 4.;
	constexpr static u32 verticesCount = 6 * 4;

	/// Display list
	const u8 opcode ATTRIBUTE_ALIGN(32) = GX_QUADS | GX_VTXFMT1;
	const u16 size                      = verticesCount;
	AdvancedVertex vertices[verticesCount]{};
	const u8 padding[(32 - sizeof(vertices) - 3) & 31]{};

	/// Texture coordinates
	TexCoord texCoord[verticesCount]{};  // const after setup (normalized in [0, 1]²)

	explicit consteval SkyBox() {
		// 6 faces
		u8 verticesIndex = 0;
		constexpr f32 r  = radius;

		u16 tc0 = static_cast<u16>(TextureIndex::SKYMAP); // night sky
		u16 tc1 = tc0 + 24; // blue sky
		u16 tc2 = tc1 + 24; // sunrise sky
		u16 tc3 = tc2 + 48; // sunrise effect
		//tc0=tc1;//test

		// north
		vertices[verticesIndex++] = {-r, +r, +r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {-r, -r, +r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {+r, -r, +r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {+r, +r, +r, tc0++, tc1++, tc2++, tc3++};

		// east
		vertices[verticesIndex++] = {+r, +r, +r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {+r, -r, +r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {+r, -r, -r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {+r, +r, -r, tc0++, tc1++, tc2++, tc3++};

		// south
		vertices[verticesIndex++] = {+r, +r, -r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {+r, -r, -r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {-r, -r, -r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {-r, +r, -r, tc0++, tc1++, tc2++, tc3++};

		// west
		vertices[verticesIndex++] = {-r, +r, -r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {-r, -r, -r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {-r, -r, +r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {-r, +r, +r, tc0++, tc1++, tc2++, tc3++};

		// top
		vertices[verticesIndex++] = {-r, +r, -r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {-r, +r, +r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {+r, +r, +r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {+r, +r, -r, tc0++, tc1++, tc2++, tc3++};

		// bottom
		vertices[verticesIndex++] = {-r, -r, +r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {-r, -r, -r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex++] = {+r, -r, -r, tc0++, tc1++, tc2++, tc3++};
		vertices[verticesIndex]   = {+r, -r, +r, tc0, tc1, tc2, tc3};
	}

	void render() const {
		GX_CallDispList(const_cast<SkyBox *>(this), sizeof(SkyBox) - sizeof(texCoord));
	}

	void remap(const bool sunrise) {
		u16 tc2 = static_cast<u16>(TextureIndex::SKYMAP) + (sunrise ? 48 : 72); // map sky
		u16 tc3 = tc2 + 48; // map effect

		for (auto & vertex : vertices) {
			vertex.tc2 = tc2++;
			vertex.tc3 = tc3++;
		}
	}
}
	ATTRIBUTE_PACKED;


class SkyControl final {
	SkyBox skyBox{};
	bool sunriseMapped = true;
	f32 angle{};

	consteval SkyControl() = default;

	static constexpr f32 nightFadeInStart  = ntime(18, 30, 0);
	static constexpr f32 nightFadeInEnd    = ntime(19, 0, 0);
	static constexpr f32 nightFadeOutStart = ntime(5, 0, 0); //inaccurate
	static constexpr f32 nightFadeOutEnd   = ntime(5, 40, 0);

	static constexpr f32 skyFadeInStart  = ntime(4, 0, 0);
	static constexpr f32 skyFadeInEnd    = ntime(6, 0, 0);
	static constexpr f32 skyFadeOutStart = ntime(18, 30, 0); //inaccurate
	static constexpr f32 skyFadeOutEnd   = ntime(19, 0, 0);

	static constexpr f32 sunriseFadeInStart  = ntime(3, 0, 0);
	static constexpr f32 sunriseFadeInEnd    = ntime(4, 40, 0);
	static constexpr f32 sunriseFadeOutStart = ntime(5, 30, 0); //inaccurate
	static constexpr f32 sunriseFadeOutEnd   = ntime(6, 0, 0);

	static constexpr f32 sunriseEffectFadeInStart  = ntime(3, 50, 0);
	static constexpr f32 sunriseEffectFadeInEnd    = ntime(4, 50, 0);
	static constexpr f32 sunriseEffectFadeOutStart = ntime(6, 0, 0); //inaccurate
	static constexpr f32 sunriseEffectFadeOutEnd   = ntime(6, 20, 0);

	static constexpr f32 sunsetFadeInStart  = ntime(18, 0, 0);
	static constexpr f32 sunsetFadeInEnd    = ntime(18, 30, 0);
	static constexpr f32 sunsetFadeOutStart = ntime(18, 55, 0); //inaccurate
	static constexpr f32 sunsetFadeOutEnd   = ntime(20, 0, 0);

	static constexpr f32 sunsetEffectFadeInStart  = ntime(17, 30, 0);
	static constexpr f32 sunsetEffectFadeInEnd    = ntime(18, 0, 0);
	static constexpr f32 sunsetEffectFadeOutStart = ntime(18, 0, 0); //inaccurate
	static constexpr f32 sunsetEffectFadeOutEnd   = ntime(19, 40, 0);


	void update(const f32 time) {
		u8 c          = ColorF32ToU8(timeFrame(time, nightFadeInStart, nightFadeInEnd, nightFadeOutStart, nightFadeOutEnd));
		GXColor color = {c, c, c, c};
		GX_SetTevKColor(GX_KCOLOR0, color); // Night color

		c     = ColorF32ToU8(timeFrame(time, skyFadeInStart, skyFadeInEnd, skyFadeOutStart, skyFadeOutEnd));
		color = {c, c, c, c};
		GX_SetTevKColor(GX_KCOLOR1, color); // Sky color

		if (time > ntime(12, 0, 0)) {
			if (sunriseMapped) {
				skyBox.remap(false);
				sunriseMapped = false;
			}
		}
		else if (!sunriseMapped) {
			skyBox.remap(true);
			sunriseMapped = true;
		}

		if (sunriseMapped) {
			c     = ColorF32ToU8(timeFrame(time, sunriseFadeInStart, sunriseFadeInEnd, sunriseFadeOutStart, sunriseFadeOutEnd));
			color = {c, c, c, c};
			GX_SetTevKColor(GX_KCOLOR2, color); // Sunrise color

			c = ColorF32ToU8(timeFrame(time, sunriseEffectFadeInStart, sunriseEffectFadeInEnd, sunriseEffectFadeOutStart, sunriseEffectFadeOutEnd));
			color = {c, c, c, c};
			GX_SetTevKColor(GX_KCOLOR3, color); // Sunrise effect color
		}
		else {
			c     = ColorF32ToU8(timeFrame(time, sunsetFadeInStart, sunsetFadeInEnd, sunsetFadeOutStart, sunsetFadeOutEnd));
			color = {c, c, c, c};
			GX_SetTevKColor(GX_KCOLOR2, color); // Sunset color

			c     = ColorF32ToU8(timeFrame(time, sunsetEffectFadeInStart, sunsetEffectFadeInEnd, sunsetEffectFadeOutStart, sunsetEffectFadeOutEnd));
			color = {c, c, c, c};
			GX_SetTevKColor(GX_KCOLOR3, color); // Sunset effect color
		}

		const f32 v = std::fmod(1.f + time - sunriseEffectFadeOutEnd, 1.f);
		f32 n;
		if (constexpr f32 duration = sunriseEffectFadeOutEnd + 1 - sunsetEffectFadeInStart; v < 1 - duration) n = 0;
		else n                     = v / duration + 1 - 1 / duration;
		angle = n * 2 * std::numbers::pi;
	}

public:
	void render(const Camera & camera) const {
		Mtx modelview, model;
		guVector axis = {1.f, 0.f, 0.f};

		guMtxRotRad(model, 'z', angle);
		guMtxScaleApply(model, model, Camera::skyboxDistance, Camera::skyboxDistance, Camera::skyboxDistance);
		guMtxTransApply(model, model, camera.pos.x, camera.pos.y, camera.pos.z);
		guMtxConcat(camera.view3D, model, modelview);
		GX_LoadPosMtxImm(modelview, GX_PNMTX0);

		skyBox.render();
	}

	friend Environment;
};
