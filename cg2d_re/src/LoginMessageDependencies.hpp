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

// 原 mouseX@0xD68CF8 与 mouseY@0xD68D08 的单次 CryptInt 读取边界。
std::int32_t ReadLegacyMouseX();
std::int32_t ReadLegacyMouseY();

// 原 sub_5049A0@[0x5049A0,0x504A25)。
std::int32_t Sub_5049A0(
    std::int32_t left,
    std::int32_t top,
    std::int32_t right,
    std::int32_t bottom,
    std::int32_t layer);

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

// Renderer::DrawGraphic_Hover@0x522180 的还原入口。
void DrawGraphicHover(
    std::uint16_t left,
    std::uint16_t top,
    std::uint16_t right,
    std::uint16_t bottom,
    std::uint8_t layer,
    std::uint32_t imageNo,
    std::int32_t step);

// x64 队列适配：以原 DrawGraphic 返回的 legacy 索引写待提交记录的 batchType。
// index 包括原失败值 -2；调用层不得将其改成数组下标或提前过滤。
void SetPendingGraphicBatchTypeAtLegacyIndex(
    std::int32_t index,
    std::uint8_t type);

// 原共享系统存储；本模块只按注释所列宽度读取，不建立第二份定义。
// 原 0xD69768；角色选择 modal 完整转交 R32，既有登录消息消费者只读低 8 位。
extern std::uint32_t gValueD69768;
extern std::uint32_t& gValueD68D9C; // 原 0xD68D9C，本函数读取低 8 位。
extern std::uint32_t& gValueD68D98; // 原 0xD68D98，sub_503990 读取低 8 位。
extern std::uint32_t& gValueD68DA0; // 原 0xD68DA0，sub_503990 读取低 8 位。
extern std::int32_t gValue558F78;  // 原 gFontList.doubleHeight@0x558F78，R32。

} // namespace cg::login::detail
