#pragma once

#include <cstdint>

namespace cg::world {

// 角色技能及每项技能的技术栏位容量；协议索引仍从0开始。
inline constexpr std::int32_t kSkillSlotCount = 30;
inline constexpr std::int32_t kTechniqueSlotCount = 30;
static_assert(kSkillSlotCount <= 32); // 战斗P封包以32位掩码表示技能可用状态。

} // namespace cg::world
