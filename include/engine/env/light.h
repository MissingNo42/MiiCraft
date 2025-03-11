#pragma once

#include <cmath>
#include <numbers>
#include <ogc/gx.h>

#include "assets/assets.h"
#include "utils/time.h"
#include "utils/utils.h"

class Environment;


class ATTRIBUTE_ALIGN(32) LightControl final {
public:
	GXColor lights[0x1000]{}; // day/night light values
	const GXColor colors[6] {
		{0xff, 0xff, 0xff, 0xff},
		{0x00, 0x00, 0xff, 0xff},
		{0x00, 0x00, 0x00, 0xff},
		{0xa8, 0xb1, 0xc0, 0xff},
		{0xd2, 0xdb, 0xea, 0xff},
		{0xbd, 0xc6, 0xd5, 0xff}
	};

	static constexpr f32 lightFadeInStart  = ntime(4, 0, 0);
	static constexpr f32 lightFadeInEnd    = ntime(6, 0, 0);
	static constexpr f32 lightFadeOutStart = ntime(18, 0, 0); //inaccurate
	static constexpr f32 lightFadeOutEnd   = ntime(19, 0, 0);

	consteval LightControl() = default;

	void colorMap(const bool enable) const {
		if (enable) {
			GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
			GX_SetArray(GX_VA_CLR0, const_cast<GXColor *>(lights), 4);
		}
		else {
			GX_SetArray(GX_VA_CLR0, nullptr, 4);
		}
	}

	void update(const f32 time) {
		setLight(time);
	}

public:
	void setLight(const f32 time) {
		const f32 day_value = timeFrame(time, lightFadeInStart, lightFadeInEnd, lightFadeOutStart, lightFadeOutEnd);
		const f32 day       = (1.f - std::cos(day_value * std::numbers::pi)) / 2.f; // smoothed
		const f32 night     = 1.f - day;

		for (s32 a = 0; a < 64; a++) {
			for (s32 n = 0; n < 64; n++) {
				const s32 idx = (n << 6) | a;
				GXColor color;
				color.r     = static_cast<u8>(std::round(static_cast<f32>(dayLight[idx].r) * day + static_cast<f32>(nightLight[idx].r) * night));
				color.g     = static_cast<u8>(std::round(static_cast<f32>(dayLight[idx].g) * day + static_cast<f32>(nightLight[idx].g) * night));
				color.b     = static_cast<u8>(std::round(static_cast<f32>(dayLight[idx].b) * day + static_cast<f32>(nightLight[idx].b) * night));
				color.a     = 0xff;
				lights[idx] = color;
			}
		}

		DCFlushRange(lights, sizeof(lights));
	}


	enum class LightColor: u16 {
		COLOR_REGION = sizeof(lights) / sizeof(GXColor),
		WHITE        = COLOR_REGION,
		BLUE,
		BLACK,
		Cloud,
		CloudBorderX,
		CloudBorderZ,
	};


	friend Environment;
};


static_assert(std::is_standard_layout_v<LightControl>);

using LightColor = LightControl::LightColor;
