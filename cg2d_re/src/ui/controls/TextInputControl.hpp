#pragma once
#include "UIControl.hpp"
namespace cg::ui {
inline constexpr int TEXT_INPUT_MAX_LENGTH = 287;
int ClampTextInputMaxLength(int length);
void WindowElement_BlurInput(WindowElement& element);
void WindowElement_ForgetInputRestore(const WindowElement& element); // 删除借用目标时取消焦点恢复引用。
void WindowElement_FocusInput(WindowElement& element);
void WindowInput_SetText(WindowInputData& input, std::wstring_view text);
void WindowElement_SyncInputText(WindowElement& element);
void WindowElement_PrepareInput(WindowElement& element, int ox, int oy, const UIClipRect* clip);
}
