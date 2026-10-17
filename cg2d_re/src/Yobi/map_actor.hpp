#pragma once

#include "base.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <string>

namespace cg::yobi {

namespace detail {

// 原 441DB0 的 0x50 载荷，与上方观察链载荷不同；未消费区不猜业务类型。
struct MapActorPayload {
    std::int32_t index; // 原成员偏移：+0x00。
    YobiAction* action04; // 原成员偏移：+0x04。
    YobiAction* action08; // 原成员偏移：+0x08。
    YobiAction* action0C; // 原成员偏移：+0x0C。
    YobiAction* action10; // 原成员偏移：+0x10。
    std::uint64_t chatDeadline14; // 原成员偏移：+0x14。
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

// CTM01：载荷按sizeof分配，64位时间槽之后采用本地布局。
static_assert(sizeof(MapActorPayload::chatDeadline14) == 8);

// 原函数：0x004424B0。
void Sub_4424B0(YobiAction* action); // 完整逐帧推进与当前位置 CryptInt 更新。
// 原43E490：完整阶段0..3及默认分支；非零阻断4417E0后续处理。
std::int32_t Sub_43E490(YobiAction* action);
// 原43DD00：按字段消费EF挂件、骑乘及附属实体，返回下一字段序号。
std::int32_t Sub_43DD00(YobiAction* action, const char* payload, std::int32_t field);

} // namespace detail

} // namespace cg::yobi
