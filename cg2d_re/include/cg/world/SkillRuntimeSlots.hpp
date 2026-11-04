#pragma once

#include "cg/world/PlayerSlots.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace cg::world {

// 原技能+3C的94H技术记录；50A120生产，数值语义未证的字段保持地址命名。
struct SkillTechniqueRecord {
    std::wstring name; // 原成员偏移：+0x00。
    std::wstring description; // 原成员偏移：+0x19。
    std::array<std::byte, 2> bytes7A; // 原成员偏移：+0x7A。
    std::int32_t value7C; // 原成员偏移：+0x7C。
    std::int32_t value80; // 原成员偏移：+0x80。
    std::int32_t value84; // 原成员偏移：+0x84。
    std::uint8_t used88; // 原成员偏移：+0x88。
    std::array<std::byte, 3> bytes89; // 原成员偏移：+0x89。
    std::int32_t value8C; // 原成员偏移：+0x8C。
    std::int32_t value90; // 原成员偏移：+0x90。
};

// 原技能配方材料0x28字节，50C580生产、519950只清itemId、4F4630查count。
struct SkillRecipeItem {
    std::int32_t itemId;
    std::int32_t count; // 原成员偏移：+0x04。
    std::wstring name; // 原成员偏移：+0x08。
    std::array<std::byte, 3> bytes25; // 原成员偏移：+0x25。
};

struct SkillRecipeRecord {
    std::int32_t itemId;
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t level; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。
    std::wstring name; // 原成员偏移：+0x14。
    std::wstring description; // 原成员偏移：+0x31。
    std::array<std::byte, 2> bytes6A; // 原成员偏移：+0x6A。
    std::array<SkillRecipeItem, 5> items6C; // 原成员偏移：+0x6C。
};

// 原F1BAFC起的技能记录；未消费字段不计最终业务映射。
struct SkillRuntimeRecord {
    std::wstring name;
    std::array<std::uint8_t, 2> requiredSlots19; // 原成员偏移：+0x19。 回复写低U8；4740F0读S16，窗口54读低S8。
    std::byte byte1B; // 原成员偏移：+0x1B。
    std::int32_t level; // 原成员偏移：+0x1C。
    std::int32_t maxLevel; // 原成员偏移：+0x20。
    std::int32_t experience; // 原成员偏移：+0x24。
    std::int32_t nextExperience; // 原成员偏移：+0x28。
    std::int32_t canRebirth; // 原成员偏移：+0x2C。
    std::int32_t skillId; // 原成员偏移：+0x30。
    std::int32_t fpReduce; // 原成员偏移：+0x34。
    std::int32_t sequence; // 原成员偏移：+0x38。
    std::array<SkillTechniqueRecord, kTechniqueSlotCount> techniques3C; // 原成员偏移：+0x3C；现扩为30栏。
    std::array<SkillRecipeRecord, 56> recipes8E8; // 原成员偏移：+0x8E8。
    std::int32_t rebirthState4C48; // 原F20744；不是旧文档的+744。
};

// 原第四清零块[F1BAE0,F633EC)，既有技能/索引表改为本owner成员别名。
struct SkillRuntimeSlots {
    std::wstring jobName;
    std::array<SkillRuntimeRecord, kSkillSlotCount> skills;
    std::array<SkillPairRecord, kSkillSlotCount> pairs;
    std::uint32_t valueF633E8; // 原全局：0x00F633E8。 509E30只写低U8，保留其余字节。
};

} // namespace cg::world
