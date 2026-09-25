#pragma once

#include <cstdint>

namespace cg::login::detail {

// 原 4FCBA0 设置表的五个无参回调；返回完整 EAX32=0。
std::int32_t Sub_4FD4C0();
std::int32_t Sub_4FD510();
std::int32_t Sub_4FD5C0();
std::int32_t Sub_4FD5F0();
std::int32_t Sub_4FD640();

} // namespace cg::login::detail
