#pragma once

namespace cg::world {

// 原43D770/43D7A0：由统一静态接线调用，不由查询或重置入口懒构造。
void ConstructWorldObjects();
void DestroyWorldObjects();

} // namespace cg::world
