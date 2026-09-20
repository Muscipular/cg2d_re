#pragma once

#include <cstdint>

namespace cg::platform {

// 原 sub_458FD0@[0x458FD0,0x459032)：初始化并关联 IME context。
int InitializeIme_458FD0();

// 原 sub_459040@[0x459040,0x459081)：恢复旧 context 并释放候选表。
void ShutdownIme_459040();

// 原 sub_459090@[0x459090,0x45909F)：保留 ImmIsIME 的完整 Win32 BOOL 位值。
[[nodiscard]] std::int32_t IsImeKeyboardLayout_459090() noexcept;

// 原 sub_4590A0@[0x4590A0,0x4590C6)：IME 时依次关闭、重新打开 context。
[[nodiscard]] std::int32_t ResetImeOpenStatus_4590A0() noexcept;

} // namespace cg::platform
