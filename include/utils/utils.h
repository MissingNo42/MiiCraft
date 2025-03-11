#pragma once
#include <cmath>
#include <gctypes.h>

inline u8 ColorF32ToU8(const f32 color) {
	return static_cast<u8>(std::round(color * 0xff));
}

constexpr auto roundUp32(const auto value) {
	return (value + 31) & ~31;
}

constexpr auto offset32(const auto value) {
	return (32 - value) & 31;
}

constexpr auto modulo(auto a, auto b) {
	return (a % b + b) % b;
}
