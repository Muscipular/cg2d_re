#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <string>

namespace cg::world {

// 宠物技能数据与索引表容量；技能索引为0—19，沿用32位战斗可用掩码。
inline constexpr std::int32_t kPetTechniqueSlotCount = 20;
static_assert(kPetTechniqueSlotCount <= 32);

namespace detail {

// 原宠物+D8的8C技能值记录，50B0D0生产，4E52B0按索引对复制到交易区。
struct PetTechniqueRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t field; // 原成员偏移：+0x02。
    std::uint16_t target; // 原成员偏移：+0x04。
    std::wstring name; // 原成员偏移：+0x06。
    std::wstring description; // 原成员偏移：+0x1F。
    std::int32_t techniqueId; // 原成员偏移：+0x80。
    std::int32_t fp; // 原成员偏移：+0x84。
    std::int32_t sequence; // 原成员偏移：+0x88。
};


} // namespace detail
} // namespace cg::world
