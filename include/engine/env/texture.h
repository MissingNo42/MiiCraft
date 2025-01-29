#pragma once

#include <gctypes.h>

#include <ogc/cache.h>

#include "engine/render/block.h"
#include "engine/render/vertex.h"

class Environment;
class TextureControl;

#define TEXCOORD_IDX(name) name, name##_LT = name, name##_RT
#define ENUM_PASS(name, value) name, name##_XXXJ = name + value - 1
#define ENUM_NO_FORWARD(name) name, name##_XXXB = name - 1

/// Texture coordinates regions controller
class TileRegion final {
	TexCoord region[17][17]{};

	consteval TileRegion() {
		for (u8 i = 0; i < 17; i++) for (u8 j = 0; j < 17; j++) region[i][j] = {TILE_COORD(i), TILE_COORD(j)};
	}

	friend class TextureControl;
}
	ATTRIBUTE_PACKED;


enum class TextureIndex: u16 {
	XXX_START = sizeof(TileRegion) / sizeof(TexCoord) - 1,
	ENUM_NO_FORWARD(CUSTOM_REGION),
	TEXCOORD_IDX(WATER),
	TEXCOORD_IDX(WOOD_DOOR_NORTH),
	TEXCOORD_IDX(WOOD_DOOR_EAST),
	TEXCOORD_IDX(WOOD_DOOR_SOUTH),
	TEXCOORD_IDX(WOOD_DOOR_WEST),
	TEXCOORD_IDX(WOOD_DOOR_TOP),
	TEXCOORD_IDX(WOOD_DOOR_BOTTOM),
	ENUM_PASS(XXX_PASS_CUSTOM, 20),
	CRAFT_MENU_LB,
	CRAFT_MENU_RT,
	CRAFT_MENU_RB,
	MENU_RT,
	MENU_RB,
	MENU_MII_LT,
	MENU_MII_LB,
	MENU_MII_RT,
	MENU_MII_RB,
	NUMBER_0T,
	NUMBER_1T,
	NUMBER_2T,
	NUMBER_3T,
	NUMBER_4T,
	NUMBER_5T,
	NUMBER_6T,
	NUMBER_7T,
	NUMBER_8T,
	NUMBER_9T,
	NUMBER_9RT,
	NUMBER_0B,
	NUMBER_1B,
	NUMBER_2B,
	NUMBER_3B,
	NUMBER_4B,
	NUMBER_5B,
	NUMBER_6B,
	NUMBER_7B,
	NUMBER_8B,
	NUMBER_9B,
	NUMBER_9RB,
	HOTBAR_LB,
	HOTBAR_RT,
	HOTBAR_RB,
	HOTBAR_SELECTOR_LT,
	HOTBAR_SELECTOR_LB,
	HOTBAR_SELECTOR_RT,
	HOTBAR_SELECTOR_RB,
	CLOUD_LB,
	CLOUD_RB,

	ENUM_NO_FORWARD(SKYMAP),
};


class CustomRegion final {
	TexCoord region[74]{
		/// Customs tile coords (LT, RT, <15>, LB, RB)
		{TILE_COORDS(10, 0)}, // LT Water
		{TILE_COORDS(10, 0)}, // RT Water
		{TILE_COORDS(1, 10)}, // LT Door North
		{TILE_COORDS(1, 12)}, // LB Door North
		{TILE_COORDS(1, 10)}, // LT Door East
		{TILE_COORDS(1, 12)}, // LB Door East
		{TILE_COORDS(2, 10)}, // LT Door South
		{TILE_COORDS(2, 10)}, // LB Door South
		{TILE_COORDS(1, 10)}, // LT Door West
		{TILE_COORDS(1, 12)}, // LB Door West
		{TILE_COORDS(1, 10)}, // LT Door Top
		{TILE_COORDS(1, 10 + 3.f / 16.f)}, // LB Door Top
		{TILE_COORDS(1, 10 + 3.f / 16.f)}, // LB Door Bottom
		{TILE_COORDS(1, 10)}, // LT Door Bottom
		{TILE_COORDS(0, 0)}, {TILE_COORDS(0, 0)}, {TILE_COORDS(0, 0)}, {TILE_COORDS(11, 0)}, // LB Water
		{TILE_COORDS(11, 0)}, // RB Water
		{TILE_COORDS(2, 10)}, // RT Door North
		{TILE_COORDS(2, 12)}, // RB Door North
		{TILE_COORDS(1 + 3.f / 16.f, 10)}, // RT Door East
		{TILE_COORDS(1 + 3.f / 16.f, 12)}, // RB Door East
		{TILE_COORDS(1, 10)}, // RT Door South
		{TILE_COORDS(1, 10)}, // RB Door South
		{TILE_COORDS(1 + 3.f / 16.f, 10)}, // RT Door West
		{TILE_COORDS(1 + 3.f / 16.f, 12)}, // RB Door West
		{TILE_COORDS(2, 10)}, // RT Door Top
		{TILE_COORDS(2, 10 + 3.f / 16.f)}, // RB Door Top
		{TILE_COORDS(2, 10)}, // RT Door Bottom
		{TILE_COORDS(2, 10 + 3.f / 16.f)}, // RB Door Bottom
		{0, 0}, {0, 0}, {0, 0},

		/// Customs coords
		{TILE_COORDS(16, 27)}, {TILE_COORDS(27, 16)}, {TILE_COORDS(27, 27)}, {TILE_COORDS(27, 0)}, {TILE_COORDS(27, 11)}, {TILE_COORDS(27, 0)},
		{TILE_COORDS(27, 11)}, {TILE_COORDS(32, 0)}, {TILE_COORDS(32, 11)}, {TILE_COORDS(16, 12)}, {TILE_COORDS(17, 12)}, {TILE_COORDS(18, 12)},
		{TILE_COORDS(19, 12)}, {TILE_COORDS(20, 12)}, {TILE_COORDS(21, 12)}, {TILE_COORDS(22, 12)}, {TILE_COORDS(23, 12)}, {TILE_COORDS(24, 12)},
		{TILE_COORDS(25, 12)}, {TILE_COORDS(26, 12)}, {TILE_COORDS(16, 13)}, {TILE_COORDS(17, 13)}, {TILE_COORDS(18, 13)}, {TILE_COORDS(19, 13)},
		{TILE_COORDS(20, 13)}, {TILE_COORDS(21, 13)}, {TILE_COORDS(22, 13)}, {TILE_COORDS(23, 13)}, {TILE_COORDS(24, 13)}, {TILE_COORDS(25, 13)},
		{TILE_COORDS(26, 13)}, {TILE_COORDS(16, 14.375)}, {TILE_COORDS(27.375f, 13)}, {TILE_COORDS(27.375f, 14.375f)}, {TILE_COORDS(16, 14.375f)},
		{TILE_COORDS(16, 15.875f)}, {TILE_COORDS(17.5f, 14.375f)}, {TILE_COORDS(17.5f, 15.875f)}, {TILE_COORDS(0, 32)}, {TILE_COORDS(16, 32)},
	};

	consteval CustomRegion() = default;

public:
	void animateWater() {
		static u8 waterFrame = 1;
		static u8 waterStep  = static_cast<u8>(-1);

		waterFrame--;

		if (!waterFrame) {
			waterFrame = 4;
			waterStep  = (waterStep + 1) % 14;

			region[17].v = region[0].v = TILE_COORD(static_cast<f32>(waterStep));
			region[18].v = region[1].v = TILE_COORD(static_cast<f32>(waterStep + 1));
			DCFlushRange(&region[0].v, 4);
			DCFlushRange(&region[1].v, 4);
			DCFlushRange(&region[17].v, 4);
			DCFlushRange(&region[18].v, 4);
		}
	}

	friend class TextureControl;
}
	ATTRIBUTE_PACKED;

#define TEXCOORD_SKY_COORD(x, y, tx, ty, sz, mx, my) {PIX_COORD((x) + (tx) * (sz) + (mx)), PIX_COORD((y) + (ty) * (sz) + (my))}
#define TEXCOORD_SKY_FACE(x, y, sz, m) \
	TEXCOORD_SKY_COORD((x), (y), 0, 0, (sz),  (m),  (m)), \
	TEXCOORD_SKY_COORD((x), (y), 0, 1, (sz),  (m), -(m)), \
	TEXCOORD_SKY_COORD((x), (y), 1, 1, (sz), -(m), -(m)), \
	TEXCOORD_SKY_COORD((x), (y), 1, 0, (sz), -(m),  (m))
#define TEXCOORD_NIGHT_FACE() {0.f, 0.f}, {0.f, 1.f}, {1.f, 1.f}, {1.f, 0.f}
#define TEXCOORD_NIGHT() TEXCOORD_NIGHT_FACE(), TEXCOORD_NIGHT_FACE(), TEXCOORD_NIGHT_FACE(), TEXCOORD_NIGHT_FACE(), TEXCOORD_NIGHT_FACE(), TEXCOORD_NIGHT_FACE()
#define TEXCOORD_SKY(SKY) \
TEXCOORD_SKY_FACE(CUBEMAP_##SKY##_N_X, CUBEMAP_##SKY##_N_Y, CUBEMAP_##SKY##_N_SZ, CUBEMAP_##SKY##_N_SZ != 1), \
TEXCOORD_SKY_FACE(CUBEMAP_##SKY##_E_X, CUBEMAP_##SKY##_E_Y, CUBEMAP_##SKY##_E_SZ, CUBEMAP_##SKY##_E_SZ != 1), \
TEXCOORD_SKY_FACE(CUBEMAP_##SKY##_S_X, CUBEMAP_##SKY##_S_Y, CUBEMAP_##SKY##_S_SZ, CUBEMAP_##SKY##_S_SZ != 1), \
TEXCOORD_SKY_FACE(CUBEMAP_##SKY##_W_X, CUBEMAP_##SKY##_W_Y, CUBEMAP_##SKY##_W_SZ, CUBEMAP_##SKY##_W_SZ != 1), \
TEXCOORD_SKY_FACE(CUBEMAP_##SKY##_T_X, CUBEMAP_##SKY##_T_Y, CUBEMAP_##SKY##_T_SZ, CUBEMAP_##SKY##_T_SZ != 1), \
TEXCOORD_SKY_FACE(CUBEMAP_##SKY##_B_X, CUBEMAP_##SKY##_B_Y, CUBEMAP_##SKY##_B_SZ, CUBEMAP_##SKY##_B_SZ != 1)
// north, east, south, west, top, bottom

class SkyRegion final {
	TexCoord region[144] {
		TEXCOORD_NIGHT(),
		TEXCOORD_SKY(DAY),
		TEXCOORD_SKY(SUNRISE),
		TEXCOORD_SKY(SUNSET),
		TEXCOORD_SKY(SUNRISE_EFFECT),
		TEXCOORD_SKY(SUNSET_EFFECT),
	};

	consteval SkyRegion() = default;

	friend class TextureControl;
}
	ATTRIBUTE_PACKED;


class TextureControl final {
	const TileRegion tileRegion{};
	CustomRegion customRegion{};
	const SkyRegion skyRegion{};

	consteval TextureControl() = default;

	void bindGPU(const bool advanced) const {
		if (advanced) {
			GX_SetArray(GX_VA_TEX1, const_cast<TextureControl *>(this), 8);
			GX_SetArray(GX_VA_TEX2, const_cast<TextureControl *>(this), 8);
			GX_SetArray(GX_VA_TEX3, const_cast<TextureControl *>(this), 8);
		} else {
			GX_SetArray(GX_VA_TEX1, nullptr, 8);
			GX_SetArray(GX_VA_TEX2, nullptr, 8);
			GX_SetArray(GX_VA_TEX3, nullptr, 8);
		}

		GX_SetArray(GX_VA_TEX0, const_cast<TextureControl *>(this), 8);
	}

	public:

	void animateWater() {
		customRegion.animateWater();
	}

	friend Environment;
} ATTRIBUTE_PACKED;
