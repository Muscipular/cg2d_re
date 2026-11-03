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

// 原 [0x5FB7F8, 0x621FF8)：512 项、每项 0x134 字节，保留未明字段。
struct Record5FB7F8 {
    std::wstring text; // 原成员偏移：+0x00。
    std::int32_t color; // 原成员偏移：+0x124。
    std::int32_t state; // 原成员偏移：+0x128。
    std::int32_t field12C; // 原成员偏移：+0x12C。
    std::int32_t fontType; // 原成员偏移：+0x130。
};

extern Record5FB7F8 gRecords5FB7F8[512]; // 原全局：0x005FB7F8。
extern volatile std::int32_t gChatWriteIndex_626C08; // 原全局：0x00626C08。
extern volatile std::int32_t gChatRecordCount_C9A944; // 原全局：0x00C9A944。
extern input::InputContext gInputContext626840; // 原全局：0x00626840。
extern std::uint32_t gChatRangeOverlayFrames_626BFC; // 原全局：0x00626BFC。
extern std::wstring gLogFileName554648; // 原全局：0x00554648。

// 原 0xD2F788/0xFF0C10：504A30 重置的独立 S32/W16 槽。
extern std::int32_t gPlayerUnspentAttributePoints_D2F788; // 原全局：0x00D2F788。
extern std::uint16_t gPendingResultWindowMode_FF0C10; // 原全局：0x00FF0C10。

// 原函数：0x0044B050。
void AppendChatMessage_44B050(std::wstring_view text, std::int32_t color, std::int32_t fontType);
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
extern std::wstring gLogPath62699C; // 原全局：0x0062699C。
extern std::FILE* volatile gLogStream626C1C; // 原全局：0x00626C1C。
extern volatile std::uint32_t gValue626C10; // 原全局：0x00626C10。
// 原函数：0x004A76E0。
std::int32_t IncrementChatRecordCount_4A76E0();
// 原函数：0x00449DA0。
void __cdecl InitializeChatInputAndLog_449DA0();                 // 原 [0x449DA0,0x449F9C)。
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
