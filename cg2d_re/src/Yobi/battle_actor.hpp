#pragma once

#include "base.hpp"

// 本组内部共享声明；跨模块接口见 cg/Yobi/YobiRuntime.hpp。

namespace cg::yobi::detail {
// 原函数：0x00438290。
YobiAction* NewBattleActor_438290(std::int32_t position); // 基础创建与404DB0绑定；完整角色属性由各caller生产。
// 原函数：0x00423800。
std::int32_t MatchBattleAlbum_423800(const YobiAction* action); // 名称原字节与四W16零/非零状态。
// 原函数：0x00404DB0。
void UpdateBattleActor_404DB0(YobiAction* action); // 完整257槽与固定入口载荷公共尾，场景7直接返回。
// 原函数：0x00406650。
void UpdateBattleAction0_406650(YobiAction* action); // BIDLE01：完整FDC持样/CL消费与归零后fresh BYTE、特殊上一项门禁。
// 原函数：0x00406720。
void UpdateBattleAction1_406720(YobiAction* action); // BIDLE01：动作归零后独立FDC低BYTE60播放门禁，不套动作0条件。
// 原函数：0x00406760。
std::int32_t UpdateBattleAction2_406760(YobiAction* action); // BRCP01：回00/04，X发布/Y取样/phase0，末播放。
// 原函数：0x00406C30。
std::int32_t UpdateBattleAction3_406C30(YobiAction* action); // BRCP01：转向后fresh目标；距离<70推进，另一支移动。
// 原函数：0x00406D70。
std::int32_t UpdateBattleAction4_406D70(YobiAction* action); // BRCP01：转向后fresh目标；距离<8推进，另一支移动。
// 原函数：0x0040CFA0。
std::int32_t UpdateBattleAction5_40CFA0(YobiAction* action); // BPAW01：比较旧FF0，推进后清零/末播放。
// 原函数：0x00406CD0。
std::int32_t UpdateBattleAction6_406CD0(YobiAction* action); // BRCP01：FE8/FEC两组取样，抵达X先发布再取Y/推进。
// 原函数：0x00409410。
void UpdateBattleAction7_409410(YobiAction* action); // BPAW01：持position转向；终帧后fresh BYTE调速/推进。
// 原函数：0x004094A0。
void UpdateBattleAction8_4094A0(YobiAction* action); // BPAW01：FE8/FEC转向；终帧后fresh BYTE调速/推进。
// 原函数：0x00409510。
std::int32_t UpdateBattleAction9_409510(YobiAction* action); // BPAW01：前两项14回位<5；其它fresh目标<70，未抵达才移动。
// 原函数：0x00409610。
void UpdateBattleAction10_409610(YobiAction* action); // BPAW01：终帧后上一项11/低BYTE门禁，重查本位恢复方向。
// 原函数：0x004067F0。
void UpdateBattleAction11_4067F0(YobiAction* action); // BRCP01：骑乘双T*低BYTE短路；回位尾fresh动作播放。
// 原函数：0x00406930。
void UpdateBattleAction12_406930(YobiAction* action); // B12P01：入口低BYTE；Y-X随机、旧FF0周期/held速率与fresh FCC门禁。
// 原函数：0x00406A80。
void UpdateBattleAction13_406A80(YobiAction* action); // B12P01：入口低BYTE；先播放/转向/移动，<5后X发布再读Y，末不补播放。
// 原函数：0x00406630。
void UpdateBattleAction236_406630(YobiAction* action); // 清phase/FD8后执行完整动作0。

} // namespace cg::yobi::detail
