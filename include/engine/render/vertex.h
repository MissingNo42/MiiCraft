#pragma once

#include <cstring>
#include <gctypes.h>
#include <gcutil.h>

#include <ogc/cache.h>
#include <ogc/gx.h>

#include "utils/concepts.h"
#include "utils/utils.h"


template<typename V>
concept IVertex = requires() {
	{ V::mapVertexFormat() } -> std::same_as<void>;
	{ V::mapVertexShader() } -> std::same_as<void>;
};


struct ColoredVertex final {
	f32 x, y, z; // 3D coordinates
	u16 c;       // color index

	static constexpr u32 vtxfmt = GX_VTXFMT2;
	static void mapVertexFormat();
	static void mapVertexShader();
}
	ATTRIBUTE_PACKED;


struct Vertex final {
	f32 x, y, z; // 3D coordinates
	u16 c, tc;   // color & texture index

	static constexpr u32 vtxfmt = GX_VTXFMT0;
	static void mapVertexFormat();
	static void mapVertexShader();
}
	ATTRIBUTE_PACKED;


struct AdvancedVertex final {
	f32 x, y, z;            // 3D coordinates
	u16 tc0, tc1, tc2, tc3; // texture index

	static constexpr u32 vtxfmt = GX_VTXFMT1;
	static void mapVertexFormat();
	static void mapVertexShader();
}
	ATTRIBUTE_PACKED;


union TexCoord final {
	struct {
		f32 u, v;
	};


	f32 uv[2];
}
	ATTRIBUTE_PACKED;


template <u16 SZ, RawStruct V = Vertex, u8 OPCODE = GX_TRIANGLES | GX_VTXFMT2>
struct DisplayList final {
	const u8 opcode ATTRIBUTE_ALIGN(32) = OPCODE;
	u16 size                            = SZ;
	V vertices[SZ];

	static constexpr u32 header = sizeof(opcode) + sizeof(size);
	const u8 padding[offset32(sizeof(vertices) + header)]{};

	/**
	 * @brief reset the display list
	 * */
	void reset() {
		size = 0;
	}

	/**
	 * @brief pad the display list to 32 bytes (GX NOP) and flush it
	 * */
	void pad() {
		const u16 csz = sizeof(V[size]) + header; // 3 = opcode + size
		const u8 sz   = offset32(csz);

		if (sz) {
			std::memset(static_cast<void *>(vertices + size), 0, sz);
		}

		flush(csz + sz);
	}

	/**
	 * @brief flush the display list out of the CPU cache to main memory
	 * @param[in] size the size of the list (default is the full size of the list)
	 * */
	void flush(const u32 size = sizeof(DisplayList)) const {
		DCFlushRange(const_cast<u8 *>(&opcode), size);
	}

	/**
	 * @brief render the display list
	 * @note must be called only flushed and padded (corruption will happen otherwise)
	 * */
	void render() const {
		GX_CallDispList(const_cast<u8 *>(&opcode), roundUp32(sizeof(V[size]) + header));
	}
}
	ATTRIBUTE_PACKED;
