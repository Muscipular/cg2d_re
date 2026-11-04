#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace cg::yobi { struct YobiAction; }
namespace cg::world {

// 角色技能及每项技能的技术栏位容量；协议索引仍从0开始。
inline constexpr std::int32_t kSkillSlotCount = 30;
inline constexpr std::int32_t kTechniqueSlotCount = 30;
static_assert(kSkillSlotCount <= 32); // 战斗P封包以32位掩码表示技能可用状态。

struct SkillPairRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t index; // 原成员偏移：+0x02。 原技能/宠物技术表下标，UI按S16消费，不是地址。
    std::int32_t sequence; // 原成员偏移：+0x04。
};

// IWT01：以上为内部对象，文本宽化后不再断言旧字节布局；SkillPair仍为纯数值记录。
static_assert(offsetof(SkillPairRecord, used) == 0);
static_assert(offsetof(SkillPairRecord, index) == 2);
static_assert(offsetof(SkillPairRecord, sequence) == 4);
static_assert(sizeof(SkillPairRecord) == 8);

// 原 FF0508/DE03F8 两张称号表共用记录；地址不存入数值字段。
struct TitleRecord {
    std::int32_t id; // 原 +0；负值表示无称号。
    std::int32_t value; // 原 +4；50A300第三字段。
    std::wstring text; // 原 +8；内部称号文本，协议入口保留原限制。
};

// 原 [FF02E0,FF03D0) 的五槽表；与玩家首块分属不同清零区。
struct PetMapCharaSlot {
    std::uint16_t flags;
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::uint32_t objectIndex;
    std::int32_t level08; // 原成员偏移：+0x08。 50B380玩家分支同步第0槽。
    std::int32_t value0C; // 原成员偏移：+0x0C。 50A3E0按掩码0x11写。
    std::int32_t value10; // 原成员偏移：+0x10。 50A3E0按掩码0x09写，在value0C之前消费。
    std::int32_t value14; // 原成员偏移：+0x14。 50A3E0按掩码0x21写。
    std::wstring name18; // 原+18，内部队伍名称。
    std::array<std::byte, 3> bytes29; // 原成员偏移：+0x29。
    cg::yobi::YobiAction* action; // 原 +2C，非拥有。
};

static_assert(offsetof(PetMapCharaSlot, flags) == 0);
static_assert(offsetof(PetMapCharaSlot, objectIndex) == 4);
static_assert(offsetof(PetMapCharaSlot, level08) == 8);
static_assert(offsetof(PetMapCharaSlot, value0C) == 0x0C);
static_assert(offsetof(PetMapCharaSlot, value10) == 0x10);
static_assert(offsetof(PetMapCharaSlot, value14) == 0x14);
static_assert(offsetof(PetMapCharaSlot, name18) == 0x18);

} // namespace cg::world
