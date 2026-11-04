#pragma once

#include "cg/render/GraphicPendingQueue.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::yobi { struct YobiAction; }

namespace cg::world {

// 原C8A73C起的共用场景配置；半字别名仍读写同一W32成员。
struct MapCharaSceneSlots {
    std::uint16_t valueC8A73C; // 原全局：0x00C8A73C。
    std::uint16_t valueC8A73E; // 原全局：0x00C8A73E。
    std::uint16_t valueC8A740; // 原全局：0x00C8A740。
    std::uint16_t valueC8A742; // 原全局：0x00C8A742。
    std::uint32_t valueC8A744; // 原全局：0x00C8A744。
    std::uint16_t valueC8A748; // 原全局：0x00C8A748。
    std::uint16_t value55DE98 {0xFFFF}; // 原全局：0x0055DE98。
    std::uint16_t valueC8A74A; // 原全局：0x00C8A74A。
    std::uint16_t valueC8A74C; // 原全局：0x00C8A74C。
    std::uint16_t valueC8A74E; // 原全局：0x00C8A74E。
    std::uint16_t valueC8A750; // 原全局：0x00C8A750。
    std::uint16_t valueC8A752; // 原全局：0x00C8A752。
    std::uint16_t valueC8A754; // 原全局：0x00C8A754。
    std::uint16_t valueC8A756; // 原全局：0x00C8A756。
    std::uint32_t valueC8A758; // 原全局：0x00C8A758。
    std::uint32_t valueC8A75C; // 原全局：0x00C8A75C。
    std::int16_t valueC8A760; // 原全局：0x00C8A760。
    std::int16_t valueC8A762; // 原全局：0x00C8A762。
    cg::yobi::YobiAction* actionC8A764; // 原全局：0x00C8A764。
    std::uint32_t valueC8A768 = 0; // 原全局：0x00C8A768。 原PE未初始化区装载清零；413070仅覆写命中半字。
    std::uint32_t valueC8A76C = 0; // 原全局：0x00C8A76C。
    float value55DE9C {5.0F}; // 原全局：0x0055DE9C。
    std::int16_t value55DEA0 {315}; // 原全局：0x0055DEA0。
    std::int16_t value55DEA2 {-1}; // 原全局：0x0055DEA2。
};

// 原[C82258,C8A728)共500项，44H载荷及两链接；只拓宽链接。
struct MapEffectRecord {
    std::int32_t x; // +00：当前位置。
    std::int32_t y; // +04：当前位置。
    std::int32_t value08; // +08：类别1/2/4的X速度，类别6/7/8的定点X。
    std::int32_t value0C; // +0C：类别1/2/4的Y速度，类别6/7/8的定点Y。
    std::int32_t speed; // +10：类别6/7/8的运动速度。
    std::int32_t graphicIndex; // +14：类别6/7/8的图形参数。
    std::array<std::byte, 8> reserved18; // +18：尚无字段依据。
    std::int32_t value20; // +20：类别4绘制局部X。
    std::int32_t value24; // +24：类别4绘制局部Y。
    std::int32_t value28; // +28：类别4绘制宽度。
    std::int32_t value2C; // +2C：类别4更新写入值，渲染作为高度消费。
    std::uint32_t remaining; // +30：类别4更新倒计数。
    std::array<std::byte, 8> reserved34; // +34：尚无字段依据。
    std::uint16_t kind; // +3C：效果类别。
    std::uint16_t phase; // +3E：类别4的颜色阶段。
    std::uint16_t variant; // +40：图案／颜色分支。
    std::array<std::byte, 2> reserved42; // +42：保留原字节。
    MapEffectRecord* prev; // 原+44，x64 +48：活动／空闲链前项。
    MapEffectRecord* next; // 原+48，x64 +50：活动／空闲链后项。
};
static_assert(std::is_standard_layout_v<MapEffectRecord>);
static_assert(std::is_trivially_copyable_v<MapEffectRecord>);
static_assert(offsetof(MapEffectRecord, x) == 0x0);
static_assert(offsetof(MapEffectRecord, y) == 0x4);
static_assert(offsetof(MapEffectRecord, value08) == 0x8);
static_assert(offsetof(MapEffectRecord, value0C) == 0xC);
static_assert(offsetof(MapEffectRecord, speed) == 0x10);
static_assert(offsetof(MapEffectRecord, graphicIndex) == 0x14);
static_assert(offsetof(MapEffectRecord, reserved18) == 0x18);
static_assert(offsetof(MapEffectRecord, value20) == 0x20);
static_assert(offsetof(MapEffectRecord, value24) == 0x24);
static_assert(offsetof(MapEffectRecord, value28) == 0x28);
static_assert(offsetof(MapEffectRecord, value2C) == 0x2C);
static_assert(offsetof(MapEffectRecord, remaining) == 0x30);
static_assert(offsetof(MapEffectRecord, reserved34) == 0x34);
static_assert(offsetof(MapEffectRecord, kind) == 0x3C);
static_assert(offsetof(MapEffectRecord, phase) == 0x3E);
static_assert(offsetof(MapEffectRecord, variant) == 0x40);
static_assert(offsetof(MapEffectRecord, reserved42) == 0x42);
static_assert(offsetof(MapEffectRecord, prev) == 0x48);
static_assert(offsetof(MapEffectRecord, next) == 0x50);
static_assert(sizeof(MapEffectRecord) == 0x58);
static_assert(alignof(MapEffectRecord) == 8);

// 配置、固定池及两条链头同属稳定会话；借用链接禁止随聚合复制或移动。
struct MapEffectRuntimeState {
    MapEffectRuntimeState() = default;
    MapEffectRuntimeState(const MapEffectRuntimeState&) = delete;
    MapEffectRuntimeState& operator=(const MapEffectRuntimeState&) = delete;
    MapEffectRuntimeState(MapEffectRuntimeState&&) = delete;
    MapEffectRuntimeState& operator=(MapEffectRuntimeState&&) = delete;

    MapCharaSceneSlots scene {};
    std::array<MapEffectRecord, 500> records {};
    MapEffectRecord* active = nullptr; // 原C8A72C，借本池节点。
    MapEffectRecord* free = nullptr; // 原C82244，借本池节点。
    std::uint8_t startedC8A728 = 0;
    std::uint64_t tickC8A730 = 0;
    std::uint8_t startedC82240 = 0;
    std::uint64_t tickC82250 = 0;
    std::uint8_t startedC8224C = 0;
    std::uint64_t tickC8A734 = 0;
    std::int32_t projectedX = -1; // 原55DEA4。
    std::int32_t projectedY = -1; // 原55DEA8。
    std::array<render::GraphicDescriptor16, 6> drawModes {{ // 原55DEB0，保留前/后各三项。
        {0, -240, 320, 290, 0xFFFFFFFFu, 1, render::GraphicMixFlag_None, 0},
        {-320, 64, 320, 270, 0xFFFFFFFFu, 1, render::GraphicMixFlag_None, 0},
        {0, 64, 320, 370, 0xFFFFFFFFu, 1, render::GraphicMixFlag_None, 0},
        {-320, -480, 320, 290, 0xFFFFFFFFu, 1, render::GraphicMixFlag_None, 0},
        {0, -480, 320, 240, 0xFFFFFFFFu, 1, render::GraphicMixFlag_None, 0},
        {-320, -181, 320, 240, 0xFFFFFFFFu, 1, render::GraphicMixFlag_None, 0},
    }};
};
static_assert(sizeof(MapEffectRuntimeState::records) == 500 * sizeof(MapEffectRecord));
static_assert(sizeof(MapEffectRuntimeState::drawModes) == 0x60);

} // namespace cg::world
