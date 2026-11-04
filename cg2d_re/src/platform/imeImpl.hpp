#pragma once

#include <cstdint>
#include <string_view>

namespace cg::session { class ClientSession; }

namespace cg::platform {

// 原 sub_458FD0@[0x458FD0,0x459032)：初始化并关联 IME context。
int InitializeIme_458FD0(session::ClientSession& session);

// NSM01：只有本上下文实际关联窗口时才恢复默认基线，随后销毁自己创建的上下文。
// 原 sub_459040@[0x459040,0x459081)：恢复旧 context 并释放候选表。
void ShutdownIme_459040(session::ClientSession& session);

// 宿主切换显式关联目标上下文；通知按窗口实际句柄匹配稳定会话，不回退当前项。
[[nodiscard]] bool AssociateSessionIme(session::ClientSession& session) noexcept;
[[nodiscard]] session::ClientSession* FindWindowImeSession() noexcept;

// 原 sub_459090@[0x459090,0x45909F)：保留 ImmIsIME 的完整 Win32 BOOL 位值。
// HWT01：当前线程布局先查询，再返回ImmIsIME完整Win32 BOOL，不转bool或补默认值。
[[nodiscard]] std::int32_t IsImeKeyboardLayout_459090() noexcept;

// 原 sub_4590A0@[0x4590A0,0x4590C6)：IME 时依次关闭、重新打开 context。
[[nodiscard]] std::int32_t ResetImeOpenStatus_4590A0(session::ClientSession& session) noexcept;

// 原4734F0..473500的窄平台边界；保留IME查询及未使用键表采样后再借用描述。
// HWT01：这是4731E0共用尾的窄平台helper；保IME查询、256B未用键表采样和描述借用顺序。
[[nodiscard]] std::wstring_view QueryWindowImeDescription_4734F0(session::ClientSession& session) noexcept;

// 原 sub_459580@[0x459580,0x4596C8)：处理 WM_IME_COMPOSITION 标志。
char HandleImeCompositionFlags_459580(session::ClientSession& session, std::int16_t flags);

} // namespace cg::platform
