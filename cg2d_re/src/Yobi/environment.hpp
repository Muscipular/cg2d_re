#pragma once

#include "base.hpp"

// 本组内部共享声明；跨模块接口见 cg/Yobi/YobiRuntime.hpp。

namespace cg::yobi::detail {

// 原 sub_448370@[0x448370,0x44839C)；查找首个 callback 匹配且 deathFlag 为零的普通节点。
// 调用前提为已初始化的有效链；允许空 callback，返回非拥有指针，未命中返回空。
YobiAction* Sub_448370(void (*callback)(YobiAction*));

} // namespace cg::yobi::detail
