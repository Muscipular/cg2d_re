#pragma once

#include <cstdint>
#include <string_view>

namespace cg::platform {

// 原 sub_458FD0@[0x458FD0,0x459032)：初始化并关联 IME context。
int InitializeIme_458FD0();

// 原 sub_459040@[0x459040,0x459081)：恢复旧 context 并释放候选表。
void ShutdownIme_459040();

// 原 sub_459090@[0x459090,0x45909F)：保留 ImmIsIME 的完整 Win32 BOOL 位值。
// HWT01：当前线程布局先查询，再返回ImmIsIME完整Win32 BOOL，不转bool或补默认值。
[[nodiscard]] std::int32_t IsImeKeyboardLayout_459090() noexcept;

// 原 sub_4590A0@[0x4590A0,0x4590C6)：IME 时依次关闭、重新打开 context。
[[nodiscard]] std::int32_t ResetImeOpenStatus_4590A0() noexcept;

// 原4590D0：借用既有100B输入法描述，不复制或更新内容。
// UTL02：原0x004590D0借用动态UTF-16描述，owner不变，视图只读。
[[nodiscard]] std::wstring_view GetImeDescription_4590D0() noexcept;
// 原4734F0..473500的窄平台边界；保留IME查询及未使用键表采样后再借用描述。
// HWT01：这是4731E0共用尾的窄平台helper；保IME查询、256B未用键表采样和描述借用顺序。
[[nodiscard]] std::wstring_view QueryWindowImeDescription_4734F0() noexcept;

} // namespace cg::platform
