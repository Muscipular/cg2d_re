#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::world::detail {

// 原宠物+D8的8C技能值记录，50B0D0生产，4E52B0按索引对复制到交易区。
struct PetTechniqueRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t field; // 原成员偏移：+0x02。
    std::uint16_t target; // 原成员偏移：+0x04。
    std::array<char, 25> name; // 原成员偏移：+0x06。
    std::array<char, 97> description; // 原成员偏移：+0x1F。
    std::int32_t techniqueId; // 原成员偏移：+0x80。
    std::int32_t fp; // 原成员偏移：+0x84。
    std::int32_t sequence; // 原成员偏移：+0x88。
};

static_assert(std::is_standard_layout_v<PetTechniqueRecord>);
static_assert(std::is_trivially_copyable_v<PetTechniqueRecord>);
static_assert(offsetof(PetTechniqueRecord, used) == 0);
static_assert(offsetof(PetTechniqueRecord, field) == 2);
static_assert(offsetof(PetTechniqueRecord, target) == 4);
static_assert(offsetof(PetTechniqueRecord, name) == 6);
static_assert(offsetof(PetTechniqueRecord, description) == 0x1F);
static_assert(offsetof(PetTechniqueRecord, techniqueId) == 0x80);
static_assert(offsetof(PetTechniqueRecord, fp) == 0x84);
static_assert(offsetof(PetTechniqueRecord, sequence) == 0x88);
static_assert(sizeof(PetTechniqueRecord) == 0x8C);

} // namespace cg::world::detail
