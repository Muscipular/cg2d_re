#pragma once

#include "base.hpp"

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::login {

namespace detail {

struct NineGridDescriptorFields {
    std::int16_t sourceX;
    std::int16_t sourceY;
    std::int16_t width;
    std::int16_t height;
    std::int32_t color;
    std::uint8_t clip;
    std::uint8_t mixFlags; // render::GraphicMixFlag 位组合。
    std::uint16_t paletteCacheIndex;
};

static_assert(std::is_standard_layout_v<NineGridDescriptorFields>);

static_assert(std::is_trivially_copyable_v<NineGridDescriptorFields>);

static_assert(sizeof(NineGridDescriptorFields) == 0x10);

static_assert(offsetof(NineGridDescriptorFields, color) == 0x08);

static_assert(offsetof(NineGridDescriptorFields, clip) == 0x0C);

static_assert(offsetof(NineGridDescriptorFields, mixFlags) == 0x0D);

static_assert(offsetof(NineGridDescriptorFields, paletteCacheIndex) == 0x0E);

// 九宫格字段按原16字节拷贝为绘制描述符，供地图绘制共用。
render::GraphicDescriptor16 ToGraphicDescriptor(
    const NineGridDescriptorFields& fields);

} // namespace detail

} // namespace cg::login
