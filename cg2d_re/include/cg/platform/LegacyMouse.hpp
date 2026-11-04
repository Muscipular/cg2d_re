#pragma once

#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/session/SessionStorage.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>

namespace cg::session { class ClientSession; }

namespace cg::platform {

// heldButtons 表示持续按住，pulseButtons 表示当前脉冲；二者共用按钮位。
enum LegacyMouseButton : std::uint32_t {
    LegacyMouseButton_None = 0x00, // 无按钮位。
    LegacyMouseButton_Left = 0x01, // 左键。
    LegacyMouseButton_Right = 0x02, // 右键。
};

// edgeButtons 的边沿事件；双击同时包含对应的按下位。
enum LegacyMouseEdge : std::uint32_t {
    LegacyMouseEdge_None = 0x00, // 无边沿。
    LegacyMouseEdge_LDown = 0x01, // 左键按下。
    LegacyMouseEdge_RDown = 0x02, // 右键按下。
    LegacyMouseEdge_LUp = 0x04, // 左键抬起。
    LegacyMouseEdge_RUp = 0x08, // 右键抬起。
    LegacyMouseEdge_LDblClick = 0x10, // 左键双击。
    LegacyMouseEdge_RDblClick = 0x20, // 右键双击。
};

// 原 [0xD68CF8,0xD68DC4) 的旧鼠标状态；末尾光标请求不属于本结构。
struct LegacyMouseState {
    // 原0x00506460、0x00506530：成员正序构造及失败前缀清理由C++承接。
    LegacyMouseState() = default;
    // 原0x00506380：成员自动逆序析构。
    ~LegacyMouseState() = default;
    LegacyCryptInt mouseX; // 原成员偏移：+0x00。
    LegacyCryptInt mouseY; // 原成员偏移：+0x10。
    LegacyCryptInt buttonCoordinates[8]; // 原成员偏移：+0x20。
    std::uint32_t heldButtons; // 原成员偏移：+0xA0；LegacyMouseButton 持续按住位。
    std::uint32_t edgeButtons; // 原成员偏移：+0xA4；LegacyMouseEdge 按帧边沿位。
    std::uint32_t pulseButtons; // 原成员偏移：+0xA8；LegacyMouseButton 按下及重复脉冲位。
    std::uint64_t leftDownTick; // 原成员偏移：+0xAC。
    std::uint64_t leftElapsed; // 原成员偏移：+0xB0。
    std::uint64_t rightDownTick; // 原成员偏移：+0xB4。
    std::uint64_t rightElapsed; // 原成员偏移：+0xB8。
    std::uint32_t valueD68DB4; // 原全局：0x00D68DB4。
    std::int32_t valueD68DB8; // 原全局：0x00D68DB8。
    std::uint32_t drawingSuppression; // 原成员偏移：+0xC4。
    std::uint32_t tailReset; // 原成员偏移：+0xC8。
};

static_assert(std::is_standard_layout_v<LegacyMouseState>);
static_assert(!std::is_trivially_destructible_v<LegacyMouseState>);
static_assert(offsetof(LegacyMouseState, mouseX) == 0x00);
static_assert(offsetof(LegacyMouseState, mouseY) == 0x10);
static_assert(offsetof(LegacyMouseState, buttonCoordinates) == 0x20);
static_assert(sizeof(LegacyMouseState::buttonCoordinates) == 0x80);
static_assert(offsetof(LegacyMouseState, heldButtons) == 0xA0);
static_assert(offsetof(LegacyMouseState, edgeButtons) == 0xA4);
static_assert(offsetof(LegacyMouseState, pulseButtons) == 0xA8);
// CTM01：时间槽改为64位，后续本地字段不再约束为旧端字节偏移。
static_assert(sizeof(LegacyMouseState::leftDownTick) == 8);
static_assert(sizeof(LegacyMouseState::leftElapsed) == 8);
static_assert(sizeof(LegacyMouseState::rightDownTick) == 8);
static_assert(sizeof(LegacyMouseState::rightElapsed) == 8);

// 鼠标记录在原中央位置构造，外层会话不提前消费十次CryptInt构造RNG。
struct MouseRuntimeState {
    MouseRuntimeState() = default;
    ~MouseRuntimeState() = default;
    MouseRuntimeState(const MouseRuntimeState&) = delete;
    MouseRuntimeState& operator=(const MouseRuntimeState&) = delete;
    MouseRuntimeState(MouseRuntimeState&&) = delete;
    MouseRuntimeState& operator=(MouseRuntimeState&&) = delete;
    session::SessionStoragePtr<LegacyMouseState> state; // 原D68CF8完整实际记录。
    std::uint64_t rightRepeatAnchorMs = 0; // 原D68BF0，不并入中间连续状态。
    std::uint64_t leftRepeatAnchorMs = 0; // 原D68DC8。
    std::uint32_t requestedCursor = 0; // 原D68DC4，帧末消费后清零。
    std::uint32_t consumedCursor = 0; // 原9A6F44，供原生窗口设置当前光标。
};

void InitializeLegacyMouseStorage(session::ClientSession& session);

// 原 sub_506870@[0x506870,0x50688B)。返回值保持为 0。
std::int32_t ResetLegacyMouseState_506870(session::ClientSession& session);

// 原 MouseEvent::Reset_0@[0x506890,0x5068FF)，严格先赋 X、后赋 Y。
void ResetLegacyMousePosition_506890(session::ClientSession& session, std::int32_t x, std::int32_t y);

// 原 MouseEvent::UpdateLDown@[0x506900,0x5069DD)。
// 更新普通鼠标状态；重复点击计数／上报按COR10移除。
void UpdateLegacyMouseLeftDown_506900(
    session::ClientSession& session,
    std::int32_t x,
    std::int32_t y);

// 原 MouseEvent::UpdateLUp@[0x5069E0,0x506A8B)。
std::int32_t UpdateLegacyMouseLeftUp_5069E0(
    session::ClientSession& session,
    std::int32_t x,
    std::int32_t y);

// 原 MouseEvent::UpdateLDbClick@[0x506A90,0x506B44)。
std::uint32_t UpdateLegacyMouseLeftDoubleClick_506A90(
    session::ClientSession& session,
    std::int32_t x,
    std::int32_t y);

// 原 MouseEvent::UpdateRDown@[0x506B50,0x506C32)。
// 更新普通鼠标状态；重复点击计数／上报按COR10移除。
void UpdateLegacyMouseRightDown_506B50(
    session::ClientSession& session,
    std::int32_t x,
    std::int32_t y);

// 原 sub_506C40@[0x506C40,0x506CEB)，对应右键抬起。
std::int32_t UpdateLegacyMouseRightUp_506C40(
    session::ClientSession& session,
    std::int32_t x,
    std::int32_t y);

// 原 sub_506CF0@[0x506CF0,0x506DA4)，对应右键双击。
std::uint32_t UpdateLegacyMouseRightDoubleClick_506CF0(
    session::ClientSession& session,
    std::int32_t x,
    std::int32_t y);

// 原 sub_506DB0@[0x506DB0,0x506DC3)；按 32 位回绕累加滚轮增量并返回原增量。
std::int32_t AddLegacyMouseWheelDelta_506DB0(session::ClientSession& session, std::int32_t delta) noexcept;

} // namespace cg::platform
