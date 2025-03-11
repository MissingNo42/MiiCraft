#pragma once

#include <algorithm>

#include <ogc/gx.h>

#include "texture.h"
#include "engine/render/block.h"
#include "engine/render/camera.h"
#include "engine/render/vertex.h"
#include "utils/matrix.h"
#include "assets/cloud.h"

class Environment;


class CloudControl final {
	static constexpr u32 cloudMapSize    = 256;
	static constexpr u32 scaleFactorH     = 12;
	static constexpr u32 scaleFactorV     = 4;
	static constexpr s32 cloudAreaSize   = cloudMapSize * scaleFactorH;
	static constexpr s32 cloudRegionSize = cloudAreaSize / CLOUD_REGION_SIZE;
	static constexpr s32 renderRadius    = 5;
	static constexpr f32 cloudAltitude   = 110.f;

	// TODO: blackhole
	// TODO: culling
	// TODO: broken vertex

	f32 shift = 0.f;

	consteval CloudControl() = default;

	void update(const f32 time) {
		shift += 0.0003f;
		if (shift > 1.f) {
			shift = 0.f;
		}
		//shift = time;
	}

public:
	void render(const Camera & camera) const {
		const s32 cx = camera.pos.x + cloudAreaSize * shift;
		const s32 cz = camera.pos.z;

		const s32 dx = cx - modulo(cx, cloudRegionSize);
		const s32 dz = cz - modulo(cz, cloudRegionSize);

		const s32 mx = dx - modulo(dx, cloudAreaSize);
		const s32 mz = dz - modulo(dz, cloudAreaSize);

		const s32 sectorX = (dx - mx) / cloudRegionSize;
		const s32 sectorZ = (dz - mz) / cloudRegionSize;

		Mtx modelview, model;

		for (s32 x = -renderRadius; x <= renderRadius; x++) {
			for (s32 z = -renderRadius; z <= renderRadius; z++) {

				const s32 sx = modulo(sectorX + x, CLOUD_REGION_SIZE);
				const s32 sz = modulo(sectorZ + z, CLOUD_REGION_SIZE);

				guMtxScale(model, scaleFactorH, scaleFactorV, scaleFactorH);
				guMtxTransApply(
					model,
					model,
					dx + x * cloudRegionSize - shift * cloudAreaSize,
					cloudAltitude,
					dz + z * cloudRegionSize
				);
				guMtxConcat(camera.view3D, model, modelview);
				GX_LoadPosMtxImm(modelview, GX_PNMTX0);

				printf("%d\t%d : %d\t%d\t ", sx, sz, sectorX, sectorZ);

				const void * region = CloudRegions[sx][sz];

				if (region) {
					GX_SetCullMode(GX_CULL_NONE);
					static_cast<const DisplayList<1, ColoredVertex> *>(region)->render();
					printf("rendered\r");
					GX_SetCullMode(GX_CULL_BACK);
				}
				else {
					printf("no region\r");
				}
			}
		}
	}

	friend Environment;
};
