#pragma once

#include <cstdint>

namespace cg::input {
struct InputContext;
}

namespace cg::input::detail {

// sub_4596D0@0x4596D0 的直接目标；其余未实现项只保留调用契约。
void Sub_44A5D0();
void Sub_44A7D0();
std::uintptr_t Sub_44A540();
std::uintptr_t Sub_44A580();
void Sub_44A990();
InputContext* Sub_44AA90();
void Sub_44AB60();
std::int32_t Sub_5043D0(std::int32_t value);
std::int32_t Sub_5042D0(std::uint32_t value);
std::int32_t Sub_504180(std::uint32_t value);

} // namespace cg::input::detail
