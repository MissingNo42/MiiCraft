//
// Created by Romain on 14/02/2024.
//

#ifndef MIICRAFTTEST_CACHE_H
#define MIICRAFTTEST_CACHE_H

#include <gctypes.h>
#include <gcutil.h>
#include <ogc/gx.h>
#include "camera.h"
#include "renderer.h"
#include "render.h"

#define LIST_SIZE 1036 // op + sz + vtx[LIST_SIZE] = 32 N
#define LIST_NUM 256

#define WHITE 64

extern const s8 Normals[] ATTRIBUTE_ALIGN(32);

extern const u32 Lights[][4] ATTRIBUTE_ALIGN(32);


enum RenderType {
	RENDER_OPAQUE = 0,
	RENDER_TRANSPARENT = 1
};


struct VextexCache {
	f32 x, y, z;
	u8 c;
	f32 u, v;
} __attribute__((packed));


struct DisplayList {
	u8 opcode = GX_QUADS | GX_VTXFMT0;
	u16 size;
	VextexCache vertex[LIST_SIZE];
	u8 pad[24] = {0};
	u8 sealed = 0;
	u32 id = 0;
	RenderType type = RENDER_OPAQUE;
	
	void reset(RenderType rtype = RENDER_OPAQUE, u32 owner = 0) {
		size = 0;
		sealed = 0;
		id = owner;
		type = rtype;
	}
	
	u8 addVertex(f32 x, f32 y, f32 z, u8 c, f32 u, f32 v) {
		vertex[size].x = x;
		vertex[size].y = y;
		vertex[size].z = z;
		vertex[size].c = c;
		vertex[size].u = u;
		vertex[size].v = v;
		size++;
		return size == LIST_SIZE; // True if the list is full
	}
	
	/**
	 * @brief seal the display list, pad it to 32 bytes or release it if empty
	 * @return 1 if released, 0 otherwise
	 * */
	u8 seal() {
		if (size == 0) {
			id = 0; // release the list
			return 1;
		}
		
		if (sealed) return 0;
		sealed = 1;
		// pad the list to 32 bytes with NOP
		u8 sz = 32 - ((size * sizeof(VextexCache) + 3) & 31); // 3 = opcode + size
		if (sz) {
			u8 * end = (u8 *) &vertex + size;
			for (u8 i = 0; i < sz; i++) *end++ = 0;
		}
		return 0;
	}
	
	void render() {
		GX_CallDispList(&opcode, size * sizeof(VextexCache) + 3);
	}
	
} __attribute__((packed));


#endif //MIICRAFTTEST_CACHE_H
