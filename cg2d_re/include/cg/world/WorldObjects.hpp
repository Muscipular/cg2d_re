#pragma once

#include <cstdint>

namespace cg::world {

// 原0x0043D760：CRT初始化已由WorldObjectsStaticInitializer构造／析构承接。
// 原43D770/43D7A0：由统一静态接线调用，不由查询或重置入口懒构造。
void ConstructWorldObjects();
// 原函数：0x0043D7A0。
void DestroyWorldObjects();

} // namespace cg::world
