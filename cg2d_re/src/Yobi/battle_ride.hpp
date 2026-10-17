#pragma once

#include "base.hpp"

// 本组内部共享声明；跨模块接口见 cg/Yobi/YobiRuntime.hpp。

namespace cg::yobi::detail {
// 原函数：0x00414050。
void UpdateBattleAction224_414050(YobiAction* action);
// 原函数：0x004154C0。
void UpdateBattleAction226_4154C0(YobiAction* action);
// 原函数：0x00415940。
void UpdateBattleAction227_415940(YobiAction* action);
// 原函数：0x004150F0。
void UpdateBattleAction225_4150F0(YobiAction* action); // 骑乘副本抛物线及1060坐标/原图恢复。
// 原函数：0x00416000。
std::int32_t UpdateBattleAction228_416000(YobiAction* action); // 清FF0后立即执行完整229。
// 原函数：0x00416020。
std::int32_t UpdateBattleAction229_416020(YobiAction* action); // 旧FF0决定播放速率，满60推进后仍播放。
// 原函数：0x004160A0。
void UpdateBattleAction230_4160A0(YobiAction* action); // 三段重取效果坐标，flags按S32转F32。
// 原函数：0x00416150。
std::int32_t UpdateBattleAction231_416150(YobiAction* action); // 未舍入减速、两个独立尾迹条件，返回旧FF0。

} // namespace cg::yobi::detail
