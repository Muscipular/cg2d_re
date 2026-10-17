#pragma once

#include "base.hpp"

#include <array>
#include <cstdint>

namespace cg::yobi {

namespace detail {

inline constexpr std::array<std::array<std::int32_t, 4>, 4> kRebirthPersistentImages4113EB {{
    {110426, 110427, 110428, 110429}, {110438, 110439, 110440, 110441},
    {110450, 110451, 110452, 110453}, {110462, 110463, 110464, 110465},
}};

// 原函数：0x0040A9E0。
void UpdateBattleAction26_40A9E0(YobiAction* action); // BHRC01：低WORD门禁，角度持样反转/移动/播放，无计数推进。
// 原函数：0x0040AA70。
std::int32_t UpdateBattleAction27_40AA70(YobiAction* action); // BHRC01：入口FDC时长与fresh边界分开；计数先于FF8清零。
// 原函数：0x0040ACF0。
std::int32_t UpdateBattleAction28_40ACF0(YobiAction* action); // BHRC01：132音效后fresh FCC，14/34/54三段与原负速归零。
// 原函数：0x0040AE50。
std::int32_t UpdateBattleAction29_40AE50(YobiAction* action); // BHRC01：新count<14后退，>=44归零/推进，末播放。
// 原函数：0x0040AF30。
std::int32_t UpdateBattleAction30_40AF30(YobiAction* action); // BHRC01：count>=14才fresh完整FDC选14/28阈值。
// 原函数：0x0040B020。
std::int32_t UpdateBattleAction31_40B020(YobiAction* action); // BHRC01：偶数旋转/fresh FCC，等待other14，方向后fresh低WORD。
// 原函数：0x00405B60。
void ConsumeBattleFpCost_405B60(YobiAction* action); // BSA01：正cost扣FP/负值零归/末-1，保token。
// 原函数：0x004107C0。
void UpdateBattleAction159_4107C0(YobiAction* action); // 1034选择HP/FP操作，独立比较/复制后推进动作。
// 原函数：0x00410B00。
void UpdateBattleAction160_410B00(YobiAction* action); // 同159数值处理，但只转phase37，不推进动作队列。
// 原函数：0x00410E30。
void UpdateBattleAction162_410E30(YobiAction* action); // 1024严格2才清通知/隐藏/2D8并推进。
// 原函数：0x004113A0。
void UpdateBattleAction163_4113A0(YobiAction* action); // 转生进入/退出、四槽发布与差异透明度/清理尾。
// 原函数：0x00411A50。
void UpdateBattleAction164_411A50(YobiAction* action); // 强制退出转生，完成时恢复FF0保存的动作阶段。
// 原函数：0x00411D30。
std::int32_t UpdateBattleAction165_411D30(YobiAction* action); // 无条件旋转受击，保留入口FDC和未舍入运动比较。
// 原函数：0x00411CB0。
void UpdateBattleAction166_411CB0(YobiAction* action); // 同源计时扣减、浮动时间及队列/动作依次推进。
// 原函数：0x00411FA0。
void UpdateBattleAction167_411FA0(YobiAction* action); // batch8后推进动作。
// 原函数：0x00411FC0。
void UpdateBattleAction168_411FC0(YobiAction* action); // batch0后推进动作。
// 原函数：0x00411FE0。
void UpdateBattleAction169_411FE0(YobiAction* action); // 仅309=255后推进动作。
// 原函数：0x00412000。
void UpdateBattleAction170_412000(YobiAction* action); // 仅309=0后推进动作。
// 原函数：0x00412020。
void UpdateBattleAction171_412020(YobiAction* action); // 自身及九附属槽淡入，完成后推进。
// 原函数：0x004120F0。
void UpdateBattleActions172_173_4120F0(YobiAction* action); // phase173减2，其余减3，完成尾保留附属8000位。
// 原函数：0x004121D0。
void UpdateBattleAction174_4121D0(YobiAction* action); // 当前效果选择调色板，再推进效果/动作。
// 原函数：0x00412210。
void UpdateBattleAction175_412210(YobiAction* action); // 1060背景淡变创建/复用，重取对象令phase0。
// 原函数：0x00412260。
void UpdateBattleAction176_412260(YobiAction* action); // 1060令phase2并清槽，不标死。
// 原函数：0x00412290。
void UpdateBattleAction177_412290(YobiAction* action); // 创建发布背景的淡变对象，不保存1060。
// 原函数：0x004122C0。
void UpdateBattleAction178_4122C0(YobiAction* action); // 推进后覆盖self.phase为0。
// 原函数：0x004122E0。
void UpdateBattleAction179_4122E0(YobiAction* action); // 动作6，播放严格返回1才推进。
// 原函数：0x00412310。
void UpdateBattleActions180_203_412310(YobiAction* action); // 23种工厂映射，188不创建，统一转204。
// 原函数：0x004124E0。
void UpdateBattleAction204_4124E0(YobiAction* action); // 九槽淡入、HP/FP结算及独立等待/完成检查。
// 原函数：0x004127B0。
void UpdateBattleAction205_4127B0(YobiAction* action); // 动作16完成后清帧索引/剩余时长。
// 原函数：0x004127F0。
void UpdateBattleAction206_4127F0(YobiAction* action); // 朝当前效果目标转向后推进。
// 原函数：0x00412830。
void UpdateBattleAction207_412830(YobiAction* action); // 当前效果写FE8/FEC，推进效果后转phase6。

} // namespace detail

} // namespace cg::yobi
