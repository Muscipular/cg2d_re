#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

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

} // namespace cg::world
