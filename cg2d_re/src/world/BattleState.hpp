#pragma once

namespace cg::network { class CgConnection; }
namespace cg::session { class ClientSession; }

#include <array>
#include <string>
#include <cstddef>
#include <cstdint>
#include <span>
#include "PlayerState.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/world/WorldSceneRuntimeState.hpp"

// 战斗状态、协议结果与场景绘制的共享声明。

namespace cg::world {

// 原 0x5ABB70/0x5ABB6C/0x553F90/0x553F84：43C8F0 顺序重置的独立 S32 槽。
// 原函数：0x0050D120。
void HandleBattleEntryReply_50D120(cg::network::CgConnection& networkConnection, std::int32_t type, std::int32_t imageNo); // 完整战斗进入回复，socket未用。
// 原函数：0x0050D3C0。
void HandleBattleStreamReply_50D3C0(cg::network::CgConnection& networkConnection, const char* text); // C/P/A/第二字节U/M完整分派。
// 原函数：0x0050D500。
void HandleBattleResultsReply_50D500(cg::network::CgConnection& networkConnection, const char* payload); // 三阶段战斗结果生产，socket未用。
void ConsumeBattlePlayerTimer(session::ClientSession& session, std::int32_t seconds); // 原43BBCE..43BBF2，先扣毫秒再按S32负值清零。
extern const std::array<std::int32_t, 5> gBattleValueThresholds54D93C; // 原全局：0x0054D93C。
extern const std::array<std::int32_t, 22> gBattlePointIndices5536FC; // 原全局：0x005536FC。
// R03：窗口初始化调整这一份阵位，战斗动作／效果继续借用。
void CenterBattleLayoutToWindow(session::ClientSession& session);
// 原函数：0x00405B40。
std::int32_t GetBattlePetPosition_405B40(std::int32_t position);
// 原函数：0x00518C60。
std::int32_t CountFreeInventorySlots_518C60(session::ClientSession& session); // BEI01：所属会话玩家slot8..27逐used W16统计空槽，S32返回；运行待。

namespace detail {
// 原函数：0x0043CB10。
void CaptureBattleBackgroundSnapshot_43CB10(session::ClientSession& session);
// 原函数：0x004ED670。
std::int32_t ShouldPlayBattleResultSound_4ED670(session::ClientSession& session); // 四角色/宠物与技能结果字段恰1检查，完整扫描不提前返回。
// 原函数：0x00504A50。
void OpenPendingBattleResultWindow_504A50(session::ClientSession& session); // S16结果模式分派30/31或既有BGM控制，原唯一caller弃返回。
// 原函数：0x0043C8F0。
std::int32_t ResetBattleCommandSelection_43C8F0(session::ClientSession& session);                 // 原 [0x43C8F0,0x43C90A)，四槽顺序写入并返回 -1。
// 原函数：0x0043C4E0。
void BeginBattleCommandSelection_43C4E0(session::ClientSession& session); // 完整命令计时初始化、对象标志与技能选择同步。
// 原函数：0x0043CBB0。
void DrawBattleScene_43CBB0(session::ClientSession& session); // 完整十状态及绘制/退出尾；427470/完整事件寿命与画面运行门禁仍未闭合。

} // namespace detail

} // namespace cg::world
