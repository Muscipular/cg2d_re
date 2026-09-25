#pragma once

#include <cstdint>

namespace cg::input {
struct InputContext;
}

namespace cg::input::detail {

// sub_4596D0@0x4596D0 的直接目标；编辑叶已落位，提交目标仍只声明真实契约。
void Sub_44A5D0();
void Sub_44A7D0();
std::uintptr_t Sub_44A540();
std::uintptr_t Sub_44A580();
void Sub_44A990();
InputContext* Sub_44AA90();
void Sub_44AB60();

} // namespace cg::input::detail
