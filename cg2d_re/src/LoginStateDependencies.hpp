#pragma once

#include <cstdint>

namespace cg::login::detail {

// sub_45E960@0x45E960 的未实现直接目标；只保留当前 IDA 调用契约，不提供空体。
void Sub_45EA50();

struct UIWindowInstance;
UIWindowInstance* Sub_532D80();

std::int32_t Sub_45EA60();
std::int32_t Sub_534290();
void Sub_45CE00();
std::int32_t Sub_45CE10(const char* text);

} // namespace cg::login::detail
