#pragma once

#include "base.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <string>

namespace cg::login {

namespace detail {

// 原 441DB0 的 0x50 载荷，与上方观察链载荷不同；未消费区不猜业务类型。
struct MapActorPayload {
    std::int32_t index; // 原成员偏移：+0x00。
    YobiAction* action04; // 原成员偏移：+0x04。
    YobiAction* action08; // 原成员偏移：+0x08。
    YobiAction* action0C; // 原成员偏移：+0x0C。
    YobiAction* action10; // 原成员偏移：+0x10。
    std::uint32_t chatDeadline14; // 原成员偏移：+0x14。
    std::int32_t value18; // 原+18，445D20发布协议数值value50。
    std::array<std::wstring, 2>* text1C; // 原+0x1C；UTL02拥有两行动态UTF-16正文，载荷仍为指针。
    std::int32_t color20; // 原+20生产完整S32，绘制消费低S8。
    std::int32_t textLength24; // 原成员偏移：+0x24。
    std::int32_t font28; // 原成员偏移：+0x28。
    YobiAction* action2C; // 原成员偏移：+0x2C。
    YobiAction* action30; // 原成员偏移：+0x30。
    YobiAction* action34; // 原成员偏移：+0x34。
    YobiAction* action38; // 原成员偏移：+0x38。
    std::int32_t kind3C; // 原+3C；action34的挂件种类，4412D0/441430/441570消费。
    std::int32_t kind40; // 原+40；action38的挂件种类。
    YobiAction* action44; // 原成员偏移：+0x44。
    std::int32_t counter48; // 原+48；43E490有符号比较/递增，46A700清零。
    YobiAction* action4C; // 原成员偏移：+0x4C。
};

static_assert(std::is_standard_layout_v<MapActorPayload>);

static_assert(std::is_trivially_copyable_v<MapActorPayload>);

static_assert(offsetof(MapActorPayload, action04) == 0x08);

static_assert(offsetof(MapActorPayload, action08) == 0x10);

static_assert(offsetof(MapActorPayload, action0C) == 0x18);

static_assert(offsetof(MapActorPayload, action10) == 0x20);

static_assert(offsetof(MapActorPayload, chatDeadline14) == 0x28);

static_assert(offsetof(MapActorPayload, value18) == 0x2C);

static_assert(offsetof(MapActorPayload, text1C) == 0x30);

static_assert(offsetof(MapActorPayload, color20) == 0x38);

static_assert(offsetof(MapActorPayload, textLength24) == 0x3C);

static_assert(offsetof(MapActorPayload, font28) == 0x40);

static_assert(offsetof(MapActorPayload, action2C) == 0x48);

static_assert(offsetof(MapActorPayload, action30) == 0x50);

static_assert(offsetof(MapActorPayload, action34) == 0x58);

static_assert(offsetof(MapActorPayload, action38) == 0x60);

static_assert(offsetof(MapActorPayload, kind3C) == 0x68);

static_assert(offsetof(MapActorPayload, kind40) == 0x6C);

static_assert(offsetof(MapActorPayload, action44) == 0x70);

static_assert(offsetof(MapActorPayload, counter48) == 0x78);

static_assert(offsetof(MapActorPayload, action4C) == 0x80);

static_assert(sizeof(MapActorPayload) == 0x88);

} // namespace detail

} // namespace cg::login
