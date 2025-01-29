#pragma once
#include <cmath>
#include <gctypes.h>

inline u8 ColorF32ToU8(const f32 color) {
	return static_cast<u8>(std::round(color * 0xff));
}
