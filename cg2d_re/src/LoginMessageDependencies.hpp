#pragma once

#include "LoginStateDependencies.hpp"

#include <array>
#include <cstdint>

namespace cg::login::detail {

// sub_45CE10@0x45CE10 尚未实现的直接目标。
std::int32_t Sub_45D7A0(const std::int32_t* values, std::int32_t count);
std::int32_t PlaySe(
    std::int32_t value0,
    std::int32_t value1,
    std::int32_t value2);
std::int32_t Sub_5049A0(
    std::int32_t left,
    std::int32_t top,
    std::int32_t right,
    std::int32_t bottom,
    std::int32_t value4);

struct GraphicDescriptor16 {
    std::array<std::uint8_t, 16> bytes;
};

static_assert(sizeof(GraphicDescriptor16) == 16);

std::int32_t DrawGraphic(
    std::int32_t x,
    std::int32_t y,
    std::uint8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    const GraphicDescriptor16* descriptor);

// 原共享系统存储；本模块只按注释所列宽度读取，不建立第二份定义。
extern std::uint32_t gValueD69768; // 原 0xD69768，本函数读取低 8 位。
extern std::uint32_t gValueD68D9C; // 原 0xD68D9C，本函数读取低 8 位。
extern std::int32_t gValue558F78;  // 原 gFontList.doubleHeight@0x558F78，R32。

} // namespace cg::login::detail
