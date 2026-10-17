#pragma once

#include "base.hpp"

// 本组内部共享声明；跨模块接口见 cg/Yobi/YobiRuntime.hpp。

namespace cg::yobi::detail {
// 原函数：0x00406E10。
void UpdateBattleAction15_406E10(YobiAction* action); // B15P01：独立事件/音效持样，回旋返回发布和258低档/高档循环保原序。
// 原函数：0x00408170。
void UpdateBattleAction18_408170(YobiAction* action); // BSP01：258三图/22050范围，单次门禁和音效event-X；95/3019重播。
// 原函数：0x004082F0。
void UpdateBattleAction19_4082F0(YobiAction* action); // BSP01：探测后33速率/22090范围，单次门禁和音效event-X；仅95重播。
// 原函数：0x004070E0。
void UpdateBattleAction70_4070E0(YobiAction* action); // BTE01：播放后一次事件/逐轮投射/末声清事件。
// 原函数：0x00417B80。
void UpdateBattleAction241_417B80(YobiAction* action);
// 原函数：0x00417CC0。
void UpdateBattleAction242_417CC0(YobiAction* action);
// 原函数：0x004071C0。
void UpdateBattleAction249_4071C0(YobiAction* action);
// 原函数：0x00409680。
void UpdateBattleAction14_409680(YobiAction* action); // B14H01：四类命中/六归零复制持样、独立BYTE读取与14-28当帧延时/FD4尾。
// 原函数：0x004065E0。
std::int32_t ProbeBattleAttackEvent_4065E0(YobiAction* action); // BSP01：终帧优先/一次事件/原无上限循环，末依次清上次动作与事件。
// 原函数：0x004072E0。
void UpdateBattleAction16_4072E0(YobiAction* action); // 投射命中与退场；保原空/索引门禁、目标重取/特殊推进和双播放尾。
// 原函数：0x004093B0。
void UpdateBattleAction17_4093B0(YobiAction* action); // 本叶仅移动/超界标死；404DB0仍执行公共尾，404C50/404D10后续摘链/释放（BPEX01）。
// 原函数：0x00408410。
void UpdateBattleAction20_408410(YobiAction* action); // B20H01：近距命中保低BYTE/HP归零取样，免疫跳过扣血/激活位，标死后仍移动。
// 原函数：0x00408750。
std::int32_t UpdateBattleAction21_408750(YobiAction* action); // B22L01：两次Y-X取样，距20以内转phase22，移动后返回播放结果。
// 原函数：0x004087C0。
std::int32_t UpdateBattleAction22_4087C0(YobiAction* action); // B22H01：回旋命中保独立HP/低BYTE与取样、3/7帧延迟及真实返回。
// 原函数：0x004090F0。
std::int32_t UpdateBattleAction23_4090F0(YobiAction* action); // B22L01：固定caster/父载荷与266列表，标死后仍移动/播放并返回其结果。
// 原函数：0x00409380。
void UpdateBattleAction24_409380(YobiAction* action); // BPAW01：新FF0>20，先归零后推进，无播放。
// 原函数：0x00406BA0。
void UpdateBattleAction25_406BA0(YobiAction* action); // BPAW01：X/Y两次查目标发布；等待归零后fresh BYTE门禁。
// 原函数：0x0040B410。
void UpdateBattleAction32_40B410(YobiAction* action); // BSEX01：8/100，严格终帧才推进。
// 原函数：0x0040B450。
void UpdateBattleAction33_40B450(YobiAction* action); // BSEX01：9/100，严格终帧才推进。
// 原函数：0x0040B5E0。
std::int32_t UpdateBattleAction34_40B5E0(YobiAction* action); // BSEX01：低WORD/BYTE、四边反弹退场，播放直到非零。
// 原函数：0x0040B930。
std::int32_t UpdateBattleAction35_40B930(YobiAction* action); // BSEX01：持样边界/40次散射，每帧尾迹后直线退场。
// 原函数：0x0040BAD0。
void UpdateBattleAction36_40BAD0(YobiAction* action); // BSEX01：低BYTE门禁/播放后fresh FF0，>=30归零推进。
// 原函数：0x0040BB20。
void UpdateBattleAction37_40BB20(YobiAction* action); // BWT01：低BYTE门禁/播放后fresh计数，>=60归零推进。
// 原函数：0x0040BB70。
void UpdateBattleAction38_40BB70(YobiAction* action); // BWT01：低BYTE门禁/播放后fresh计数，>=120归零推进。
// 原函数：0x0040BBC0。
std::int32_t UpdateBattleAction39_40BBC0(YobiAction* action); // BWT01：36短路/原声和两方向，末连续三播放。
// 原函数：0x0040BD40。
void UpdateBattleAction40_40BD40(YobiAction* action); // BWT01：恢复方向后推进，不增播放。
// 原函数：0x00416220。
std::int32_t UpdateBattleAction232_416220(YobiAction* action); // 动态目标命中，先推进自身动作再推进效果，每帧原rand尾迹。
// 原函数：0x00416860。
std::int32_t UpdateBattleAction233_416860(YobiAction* action); // 无位移和减速的分段旋转，保留入口FDC与旧计数。
// 原函数：0x00416AB0。
std::int32_t UpdateBattleAction234_416AB0(YobiAction* action); // 首帧散射及四边反弹，播放至非零，不加循环上限。
// 原函数：0x00416DD0。
std::int32_t UpdateBattleAction237_416DD0(YobiAction* action); // 完整动作237，返回最终PlayFrame结果。
// 原函数：0x00417410。
void UpdateBattleActions238_239_417410(YobiAction* action); // 动态效果命中，FD0归零当帧双播放，按写回F32速度转阶段。
// 原函数：0x00417AC0。
void UpdateBattleAction240_417AC0(YobiAction* action); // 播放后消费帧事件，清施法特效后重取+10决定平铺。
// 原函数：0x00417D80。
void UpdateBattleAction243_417D80(YobiAction* action); // 技能图号及动态flags命中，普通路径不扣HP/显示伤害数字。
// 原函数：0x00418550。
void UpdateBattleAction244_418550(YobiAction* action); // FDC置2、门禁和FF8置1后推进动作。
// 原函数：0x00418580。
void UpdateBattleAction245_418580(YobiAction* action); // 当前效果指定平铺类型/寿命，创建后推进效果及动作。
// 原函数：0x004185C0。
void UpdateBattleAction246_4185C0(YobiAction* action); // position3乘原F32比例设置速度，再推进效果及动作。
// 原函数：0x004185F0。
void UpdateBattleAction247_4185F0(YobiAction* action); // 原F32坐标按X/Y分别截断到载荷10/14。
// 原函数：0x00418620。
std::int32_t UpdateBattleAction248_418620(YobiAction* action); // 返回载荷10/14坐标，距离<5时重读坐标并推进。
// 原函数：0x004186B0。
void UpdateBattleAction255_4186B0(YobiAction* action); // 每轮固定效果记录，治疗用目标1DC，保留空目标收尾。
// 原函数：0x00418B50。
void UpdateBattleAction256_418B50(YobiAction* unused); // 对象实参未使用，仅顺序写5ABB1C/5ABB20。

} // namespace cg::yobi::detail
