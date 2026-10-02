#pragma once

#include "cg/LegacyRecordOrder.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/YobiRuntime.hpp"

#include <cstdint>

namespace cg::login::detail {

// sub_45D990@0x45D990 的直接目标。
std::int32_t TextMeasure(const char* text, std::int32_t fontType);
std::int32_t MeasureDefaultText_44B260(const char* text); // 非空入口快照字体0双宽/单宽，无字节预算。

// sub_508360@0x508360 的直接目标；只声明真实调用契约。
std::int32_t GetServerAddressInfo_507D90(
    std::int32_t serverIndex,
    char* serverAddress,
    std::uint16_t* port);
std::uint32_t Sub_44B5C0(const char* text);
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
