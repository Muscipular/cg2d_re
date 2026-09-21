#pragma once

#include "cg/render/TextPendingQueue.hpp"
#include "cg/YobiRuntime.hpp"

#include <cstdint>

namespace cg::login::detail {

// sub_45E960@0x45E960 中仍未实现的直接目标；只保留当前 IDA 调用契约，不提供空体。
struct UIWindowInstance;
UIWindowInstance* Sub_532D80();

std::int32_t Sub_534290();

// sub_45D990@0x45D990 的直接目标。
std::int32_t TextMeasure(const char* text, std::int32_t fontType);
void Sub_5081A0();
std::int32_t Sub_508140();
std::int32_t Sub_508360();
inline std::int32_t DrawText(
    const std::int16_t x,
    const std::int16_t y,
    const std::uint8_t value2,
    const std::int8_t value3,
    const std::int8_t value4,
    const char* const text,
    const std::int8_t value6,
    const std::int8_t value7)
{
    return cg::render::DrawText_456780(
        x,
        y,
        value2,
        value3,
        value4,
        text,
        value6,
        value7);
}

// sub_45EA60@0x45EA60 的绘制目标。
inline std::int32_t DrawText3(
    const std::int16_t x,
    const std::int16_t y,
    const std::int8_t renderMode,
    const std::int8_t color,
    const char* const text,
    const std::int8_t value7)
{
    return cg::render::DrawText3_456560(
        x,
        y,
        renderMode,
        color,
        text,
        value7);
}

} // namespace cg::login::detail
