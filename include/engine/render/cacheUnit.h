//
// Created by Romain on 14/02/2024.
//

#ifndef MIICRAFTTEST_CACHEUNIT_H
#define MIICRAFTTEST_CACHEUNIT_H

#include <gctypes.h>
#include <gcutil.h>
#include <ogc/gx.h>
#include <cstring>
#include "camera.h"
#include "renderer.h"

#define LIST_SIZE 1044 // op + sz + vtx[LIST_SIZE] = 32 N
#define LIST_NUM 800

#define WHITE 64
#define BLUE 65
#define BLACK 66

enum TextureIndex: u16 {
	CUSTOM_TILE_TEXCOORDS = 289,
	WATER = CUSTOM_TILE_TEXCOORDS,
	
	CUSTOM_TEXCOORDS = 323,
	CRAFT_MENU_LB = CUSTOM_TEXCOORDS,
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
};

extern const u32 Lights[][4] ATTRIBUTE_ALIGN(32);
extern f32 TexCoord[][2] ATTRIBUTE_ALIGN(32);

void runWater();

enum RenderType: u8 {
	RENDER_OPAQUE = 0,
	RENDER_TRANSPARENT = 1
};


struct VextexCache {
	f32 x, y, z; // 3D coordinates
	u8 c;        // color index
	u16 tc;      // texture index
} __attribute__((packed));


struct DisplayList { // must be 32 N bytes to keep GP aligned
	/// Display list structure: 32 N bytes
	// display list header
	u8 opcode = GX_QUADS | GX_VTXFMT0;
	u16 size;
	
	// display list body
	VextexCache vertex[LIST_SIZE]; // LIST_SIZE vertices: must be multiple of 4 (because quads)
	u8 padding[17] = {0}; // display list end zero-padding (32 N)
	
	/// non-display list handling data: must be 32 N bytes to keep the next list aligned
	RenderType type = RENDER_OPAQUE;
	u8 sealed = 0;
	u32 id = 0;
	u8 align[26];
	
	/**
	 * @brief clear & reset the display list
	 * */
	void reset(RenderType rtype = RENDER_OPAQUE, u32 owner = 0) {
		size = 0;
		sealed = 0;
		id = owner;
		type = rtype;
	}
	
	u8 addVertex(f32 x, f32 y, f32 z, u8 c, u16 tc) {
		vertex[size].x = x;
		vertex[size].y = y;
		vertex[size].z = z;
		vertex[size].c = c;
		vertex[size].tc = tc;
		size++;
		return size == LIST_SIZE; // True if the list is full
	}
	
	/**
	 * @brief seal the display list, pad it to 32 bytes or release it if empty
	 * @return 1 if released, 0 otherwise
	 * */
	u8 seal() {
		if (size == 0) {
			u8 r = id != 0;
			id = 0; // release the list
			return r;
		}
		
		if (sealed) return 0;
		sealed = 1;
		
		// pad the list to 32 bytes with NOP
		u16 csz = size * sizeof(VextexCache) + 3; // 3 = opcode + size
		u8 sz = (32 - csz) & 31;
		if (sz) {
			memset(vertex + size, 0, sz);
		}
		
		DCFlushRange(&opcode, csz + sz);
		return 0;
	}
	
	/**
	 * @brief render the display list
	 * @note must be called only on sealed lists (corruption will happen otherwise)
	 * */
	void render() {
		u16 csz = size * sizeof(VextexCache) + 3;
		u8 sz = (32 - csz) & 31; // 3 = opcode + size
		
		GX_CallDispList(&opcode, csz + sz);
	}
	
} __attribute__((packed));


#endif //MIICRAFTTEST_CACHEUNIT_H
