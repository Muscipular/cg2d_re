#pragma once

#include <cstddef>
#include <cstdint>

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

} // namespace cg::world
