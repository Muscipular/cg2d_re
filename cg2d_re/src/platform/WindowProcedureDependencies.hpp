#pragma once

#include <cstdint>

namespace cg::render {

// 原 Renderer::IsLost@0x4536D0；真实资源遍历实现尚未还原。
int IsLost_4536D0();

} // namespace cg::render

namespace cg::input {

// 原 Input::AppendChar@0x44ADB0；真实输入实现尚未还原。
void AppendChar_44ADB0(char character);

} // namespace cg::input

namespace cg::platform {

// 原 sub_459580@0x459580；真实 IME 组合串处理尚未还原。
char Sub_459580(std::int16_t flags);

} // namespace cg::platform
