#pragma once

#include <gctypes.h>
#include <gcutil.h>


struct Vextex final {
	f32 x, y, z; // 3D coordinates
	u16 c, tc;   // color & texture index
}
	ATTRIBUTE_PACKED;


struct AdvancedVextex final {
	f32 x, y, z;            // 3D coordinates
	u16 tc0, tc1, tc2, tc3; // texture index
}
	ATTRIBUTE_PACKED;


union TexCoord final {
	struct {
		f32 u, v;
	};

	f32 uv[2];
}
	ATTRIBUTE_PACKED;
