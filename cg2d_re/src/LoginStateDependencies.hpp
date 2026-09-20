#pragma once

#include "cg/YobiRuntime.hpp"

#include <cstdint>

namespace cg::login::detail {

// sub_45E960@0x45E960 中仍未实现的直接目标；只保留当前 IDA 调用契约，不提供空体。
struct UIWindowInstance;
UIWindowInstance* Sub_532D80();

std::int32_t Sub_534290();

// sub_45D990@0x45D990 的直接目标。
std::int32_t TextMeasure(const char* text, std::int32_t fontType);
std::int32_t Sub_50FC30();
std::int32_t Sub_50FE30();
std::int32_t Sub_50FF10();
std::int32_t Sub_50FF70();
std::int32_t Sub_510120();
void Sub_5081A0();
std::int8_t Sub_401000(std::int32_t value);
std::int32_t Sub_508140();
std::int32_t Sub_508350();
std::int32_t Sub_508360();
std::int8_t Sub_401030(std::int32_t value);
std::int32_t DrawText(
    std::int16_t x,
    std::int16_t y,
    std::uint8_t value2,
    std::int8_t value3,
    std::int8_t value4,
    const char* text,
    std::int8_t value6,
    std::int8_t value7);

// sub_45EA60@0x45EA60 的绘制目标。
std::int32_t DrawText3(
    std::int16_t x,
    std::int16_t y,
    std::int8_t value2,
    std::int8_t value3,
    const char* text,
    std::int8_t value5);

} // namespace cg::login::detail
