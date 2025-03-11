#pragma once

#include <type_traits>

template<typename T>
concept RawStruct = std::is_standard_layout_v<T> && std::is_trivial_v<T>;
