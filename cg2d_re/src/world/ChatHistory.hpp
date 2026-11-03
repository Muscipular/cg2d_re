#pragma once

namespace cg::network { class CgConnection; }
namespace cg::session { class ClientSession; }

#include <cstddef>
#include <cstdio>
#include <string_view>
#include <string>
#include <array>
#include "cg/Yobi/YobiRuntime.hpp"

#include <cstdint>

namespace cg::input {
class InputContext;
}

namespace cg::world {

// 原0xD2F788：504A30重置的独立S32槽。
extern std::int32_t gPlayerUnspentAttributePoints_D2F788; // 原全局：0x00D2F788。

// 原函数：0x0044B050。
void AppendChatMessage_44B050(
    session::ClientSession& session, std::wstring_view text, std::int32_t color, std::int32_t fontType);
// 原函数：0x0050C940。
std::int32_t SendPlayerChatInput_50C940(
    session::ClientSession& session, const input::InputContext* context, std::int32_t color); // 聊天时间/选项门禁后转义发送。 // CTS01：见聊天发送完整合同。
// 原函数：0x0050C7A0。
std::uint32_t HandlePlayerTimerReply_50C7A0(cg::network::CgConnection& networkConnection, std::int32_t seconds, std::int32_t mode); // PTP01：原首缩放/mode/剩余缩放/单 tick 序，返完整 gate/tick。
// 原函数：0x0050CE10。
std::int32_t HandleSystemChatReply_50CE10(cg::network::CgConnection& networkConnection, const char* text); // 系统提示及可选内嵌音效。
// 原函数：0x0050CAE0。
void HandlePlayerChatReply_50CAE0(cg::network::CgConnection& networkConnection, std::int32_t objectIndex, const char* payload,
    std::int32_t color, std::int32_t font); // 聊天回复：原名字过滤、日志与头顶生产。
// 原函数：0x0043D860。
void FaceWorldActorTowardPlayer_43D860(cg::yobi::YobiAction* action); // 指定地图模式下，类别1对象面向玩家。
// 原函数：0x00449FA0。
std::int32_t SaveChatInputHistoryEntry_449FA0(session::ClientSession& session, std::int32_t index); // 原 [0x449FA0,0x44A084)，写入一项历史和尾索引。

namespace detail {
// 原函数：0x004A76E0。
std::int32_t IncrementChatRecordCount_4A76E0(session::ClientSession& session);
// 原函数：0x00449DA0。
void __cdecl InitializeChatInputAndLog_449DA0(session::ClientSession& session); // 原 [0x449DA0,0x449F9C)。
void MarkPlayerChatActive() noexcept; // 51A6A0在固定载荷前置flags18A的2000。
// 原函数：0x00442DC0。
void SetWorldActorChatBubble_442DC0(
    cg::yobi::YobiAction* action, std::int32_t duration, std::wstring_view text,
    std::int32_t color, std::int32_t font);
// 原函数：0x0051A6A0。
void SetPlayerChatBubble_51A6A0(
    std::int32_t duration, std::wstring_view text, std::int32_t color,
    std::int32_t font); // 同源玩家头顶文本生产。
// 原函数：0x0044A090。
std::int32_t LoadChatInputHistory_44A090(session::ClientSession& session); // 原 [0x44A090,0x44A15B)，读取或创建 chathis.dat。

} // namespace detail

} // namespace cg::world
