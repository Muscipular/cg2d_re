#pragma once

#include <cstdint>

namespace cg::input {
class InputContext;
}

namespace cg::input::detail {

// sub_4596D0@0x4596D0 的直接目标；44AB60完整提交由既有Input.cpp承接。
// 原函数：0x0044A5D0。
void Sub_44A5D0();
// 原函数：0x0044A7D0。
void Sub_44A7D0();
// 原函数：0x0044A540。
std::uintptr_t Sub_44A540();
// 原函数：0x0044A580。
std::uintptr_t Sub_44A580();
// 原函数：0x0044A990。
void Sub_44A990();
// 原函数：0x0044AA90。
InputContext* Sub_44AA90();
// 原函数：0x0044AB60。
void Sub_44AB60();

} // namespace cg::input::detail
