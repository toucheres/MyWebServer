#pragma once

// 检测反射头文件
#if __has_include("ylt/struct_pack/reflection.hpp")
#include "reflection_ylt.hpp"
#define HAS_YLT_REFLECTION 1
#elif __has_include("reflection_self.hpp")
#include "reflection_self.hpp"
#define HAS_CUSTOM_REFLECTION 1
#else
#error "No reflection header found"
#endif