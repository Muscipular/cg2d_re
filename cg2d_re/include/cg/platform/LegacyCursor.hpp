#pragma once

#include <cstdint>

namespace cg::platform {

// 原 sub_506DE0@[0x506DE0,0x506DFB)。force 非零时无条件写入请求状态；
// force 为零时仅在当前请求状态为零时写入。
std::int32_t RequestCursorState_506DE0(
    std::int32_t requestedState,
    std::int32_t force) noexcept;

// 原 sub_506DD0@[0x506DD0,0x506DDB)，只清旧鼠标状态尾字段。
void ResetLegacyMouseTail_506DD0();

// 原 World::CursorUpdate@[0x506E00,0x507083)。
void UpdateCursor_506E00();

} // namespace cg::platform
