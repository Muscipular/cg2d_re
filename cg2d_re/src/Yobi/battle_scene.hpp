#pragma once

#include "base.hpp"

#include <cstdint>

namespace cg::yobi {

namespace detail {

// 共享原状态，只在本组实现中定义；原全局：0x005ABB24。
extern std::int32_t gBattlePendingGroupCount5ABB24;

// 原43B360完整事件调度，4383B0已绑定；424620攻击生产已落位，完整事件/数据与运行另验。
void DispatchBattleEvents_43B360(YobiAction* scene);
// 原43B610：绑定完整427470、原序初始化22位置并发布32槽事件T*。
YobiAction* NewBattleEvent_43B610(
    std::int32_t kind, std::int32_t slot, std::uint32_t participants, std::int32_t group);
// 原函数：0x004383B0。
YobiAction* NewBattleScene_4383B0(); // 优先级1，原生场景载荷、完整43B360绑定及A8全位置1。
// 原函数：0x0043B6C0。
void FinishBattleEvent_43B6C0(YobiAction* action); // 推进同一事件消息或注销当前槽后MarkDeath。
// 原函数：0x0043C0A0。
void DrawBattleEntryHover_43C0A0(std::int32_t index); // 固定可选对象，按DX尺寸和播放偏移画框。

} // namespace detail

} // namespace cg::yobi
