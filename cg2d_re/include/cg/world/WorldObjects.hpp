#pragma once

#include <cstdint>

namespace cg::world {

// 原CRT 553014：同一千项表构造后宿主std::atexit即时登记整体退出，传出真实S32；全应用序待。
std::int32_t InitializeLegacyWorldObjects_43D760();

// 原43D770/43D7A0：由统一静态接线调用，不由查询或重置入口懒构造。
void ConstructWorldObjects();
void DestroyWorldObjects();

} // namespace cg::world
