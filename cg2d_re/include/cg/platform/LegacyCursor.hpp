#pragma once

namespace cg::platform {

// 原 sub_506DD0@[0x506DD0,0x506DDB)，只清旧鼠标状态尾字段。
void ResetLegacyMouseTail_506DD0();

// 原 World::CursorUpdate@[0x506E00,0x507083)。
void UpdateCursor_506E00();

} // namespace cg::platform
