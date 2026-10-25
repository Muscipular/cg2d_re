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
    YobiAction* action04; // 原+04；442F70创建／跟随的头顶标记，非拥有引用。
    YobiAction* action08; // 原+08；43E490过渡阶段创建／绘制的附属图像，非拥有引用。
    YobiAction* action0C; // 原+0C；状态位4驱动的0x1AF4D图标，具体业务未核，非拥有引用。
    YobiAction* action10; // 原+10；状态位1驱动的0x1AF4E图标，具体业务未核，非拥有引用。
    std::uint64_t chatDeadline14; // 原成员偏移：+0x14。
    std::int32_t itemCount18; // 原+18；445D20发布地面物品数量，悬停绘制名称后的数量。
    std::array<std::wstring, 2>* text1C; // 原+0x1C；UTL02拥有两行动态UTF-16正文，载荷仍为指针。
    std::int32_t color20; // 原+20生产完整S32，绘制消费低S8。
    std::int32_t textLength24; // 原成员偏移：+0x24。
    std::int32_t font28; // 原成员偏移：+0x28。
    YobiAction* action2C; // 原+2C；43DD00解析的附属地图实体，非拥有引用，不猜具体宠物类别。
    YobiAction* action30; // 原+30；主挂件，当前4.06接线由DrawMapMainGear406绘制，非拥有引用。
    YobiAction* action34; // 原+34；kind3C对应的第一路附属挂件，非拥有引用。
    YobiAction* action38; // 原+38；kind40对应的第二路附属挂件，非拥有引用。
    std::int32_t kind3C; // 原+3C；action34的挂件种类，4412D0/441430/441570消费。
    std::int32_t kind40; // 原+40；action38的挂件种类。
    YobiAction* action44; // 原+44；骑乘对象，复用地图实体运动／绘制，非拥有引用。
    std::int32_t counter48; // 原+48；43E490有符号比较/递增，46A700清零。
    YobiAction* action4C; // 原+4C；按地图状态选图的动态图标，具体业务未核，非拥有引用。
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
void UpdateMapActorPathMovement_4424B0(YobiAction* action); // 完整逐帧推进与当前位置 CryptInt 更新。
// 原43E490：完整阶段0..3及默认分支；非零阻断4417E0后续处理。
std::int32_t UpdateMapActorTransitionPhase_43E490(YobiAction* action);
// 原43DD00：按字段消费EF挂件、骑乘及附属实体，返回下一字段序号。
std::int32_t ParseMapActorAttachments_43DD00(YobiAction* action, const char* payload, std::int32_t field);

} // namespace detail

} // namespace cg::yobi
