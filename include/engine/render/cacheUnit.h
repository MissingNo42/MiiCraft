#pragma once

#include <cstring>
#include <gctypes.h>
#include <gcutil.h>
#include <ogc/gx.h>
#include <ogc/cache.h>
#include "vertex.h"

#define LIST_SIZE 1044 // op + sz + vtx[LIST_SIZE] = 32 N
#define LIST_NUM 600


enum class RenderChannel: u8 {
	Opaque = 0,
	Transparent = 1
};


struct DisplayList { // must be 32 N bytes to keep GP aligned
	/// Display list structure: 32 N bytes
	// display list header
	u8 opcode = GX_QUADS | GX_VTXFMT0;
	u16 size;

	// display list body
	Vextex vertex[LIST_SIZE]; // LIST_SIZE vertices: must be multiple of 4 (because quads)
	u8 padding[29] = {}; // display list end zero-padding (32 N)

	/// non-display list handling data: must be 32 N bytes to keep the next list aligned
	RenderChannel type = RenderChannel::Opaque;
	u8 sealed       = 0;
	u32 id          = 0;
	u8 align[26];

	/**
	 * @brief clear & reset the display list
	 * */
	void reset(const RenderChannel rtype = RenderChannel::Opaque, const u32 owner = 0) {
		size   = 0;
		sealed = 0;
		id     = owner;
		type   = rtype;
	}

	bool addVertex(f32 x, f32 y, f32 z, u16 c, u16 tc) {
		vertex[size].x  = x;
		vertex[size].y  = y;
		vertex[size].z  = z;
		vertex[size].c  = c;
		vertex[size].tc = tc;
		size++;
		return size == LIST_SIZE; // True if the list is full
	}

	/**
	 * @brief seal the display list, pad it to 32 bytes or release it if empty
	 * @return 1 if released, 0 otherwise
	 * */
	bool seal() {
		if (size == 0) {
			const bool r = id != 0;
			id           = 0; // release the list
			return r;
		}

		if (sealed) return false;
		sealed = 1;

		// pad the list to 32 bytes with NOP
		const u16 csz = size * sizeof(Vextex) + 3; // 3 = opcode + size
		const u8 sz   = (32 - csz) & 31;
		if (sz) {
			memset(vertex + size, 0, sz);
		}

		DCFlushRange(&opcode, csz + sz);
		return false;
	}

	/**
	 * @brief render the display list
	 * @note must be called only on sealed lists (corruption will happen otherwise)
	 * */
	void render() {
		const u16 csz = size * sizeof(Vextex) + 3;
		const u8 sz   = (32 - csz) & 31; // 3 = opcode + size

		GX_CallDispList(&opcode, csz + sz);
	}
}
	ATTRIBUTE_PACKED;
