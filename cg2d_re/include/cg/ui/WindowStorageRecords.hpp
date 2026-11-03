#pragma once

#include "cg/world/PetTechniqueRecord.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::ui::detail {

// 原CC4D58组内的物品；4E0100／4A84A0生产，旧72和窗口38／41共享。
struct WindowStorageItem {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::int16_t selection; // 原成员偏移：+0x02。
    std::int32_t imageNo; // 原成员偏移：+0x04。
    std::int32_t count; // 原成员偏移：+0x08。
    std::wstring name; // 原成员偏移：+0x0C。
    std::array<std::wstring, 32> description; // 原成员偏移：+0x29。
    std::wstring memo; // 原成员偏移：+0xC29。
    std::array<std::byte, 2> reservedC3A; // 原成员偏移：+0xC3A。
    std::int32_t pages; // 原成员偏移：+0xC3C；004E0253写DWORD。
    std::int32_t flags; // 原成员偏移：+0xC40。
};

// 原D64E18存放宠物；4A84A0回复生产，保留与交易宠物不同的布局。
struct WindowStoragePet {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::uint16_t tribe; // 原成员偏移：+0x02；显示时按S16解释。
    std::int32_t imageNo; // 原成员偏移：+0x04。
    std::int32_t level; // 原成员偏移：+0x08。
    std::int32_t hp; // 原成员偏移：+0x0C。
    std::int32_t maxHp; // 原成员偏移：+0x10。
    std::int32_t fp; // 原成员偏移：+0x14。
    std::int32_t maxFp; // 原成员偏移：+0x18。
    std::array<std::int32_t, 5> combatStats; // 原成员偏移：+0x1C。
    std::array<std::int32_t, 5> baseStats; // 原成员偏移：+0x30。
    std::int32_t combatValue44; // 原成员偏移：+0x44；窗口显示的第六项战斗数值。
    std::array<std::int32_t, 4> elements; // 原成员偏移：+0x48。
    std::int32_t value58; // 原成员偏移：+0x58；回复第11项数值。
    std::array<world::detail::PetTechniqueRecord, world::kPetTechniqueSlotCount> techniques; // 原成员偏移：+0x5C；回复只写技能名。
    std::wstring name; // 原成员偏移：+0x5D4。
    std::wstring speciesName; // 原成员偏移：+0x5E5。
    std::array<std::byte, 2> reserved5F6; // 原成员偏移：+0x5F6。
    std::int32_t weight; // 原成员偏移：+0x5F8。
    std::int32_t maxWeight; // 原成员偏移：+0x5FC。
    std::array<std::wstring, 6> equipmentNames; // 银行新增字段：六个装备槽的GBK物品名，空槽为空字符串。
};

// 原CC4D58每组+F550的旧面板宠物摘要；4E1B10／4E1EE0消费。
struct WindowStoragePetSummary {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::int16_t selection; // 原成员偏移：+0x02。
    std::uint16_t tribe; // 原成员偏移：+0x04。
    std::array<std::byte, 2> reserved06; // 原成员偏移：+0x06。
    std::int32_t imageNo; // 原成员偏移：+0x08。
    std::int32_t level; // 原成员偏移：+0x0C。
    std::int32_t hp; // 原成员偏移：+0x10。
    std::int32_t maxHp; // 原成员偏移：+0x14。
    std::int32_t fp; // 原成员偏移：+0x18。
    std::int32_t maxFp; // 原成员偏移：+0x1C。
    std::array<std::int32_t, 5> combatStats; // 原成员偏移：+0x20。
    std::array<std::int32_t, 4> elements; // 原成员偏移：+0x34。
    std::int32_t value44; // 原成员偏移：+0x44。
    std::array<std::wstring, 10> techniqueNames; // 原成员偏移：+0x48。
    std::wstring name; // 原成员偏移：+0x142。
    std::byte reserved153; // 原成员偏移：+0x153。
};

// 原CC4D58两组共享展示区；金额独立于物品／宠物区的清零操作。
struct WindowItemDetailGroup {
    std::array<WindowStorageItem, 20> items; // 原组内偏移：+0x0000。
    std::array<WindowStoragePetSummary, 5> pets; // 原组内偏移：+0xF550。
    std::int32_t amount; // 原组内偏移：+0xFBF4。
};

} // namespace cg::ui::detail
