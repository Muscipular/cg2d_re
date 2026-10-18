#pragma once

#include "../world/PetTechniqueRecord.hpp"

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
    std::array<char, 29> name; // 原成员偏移：+0x0C。
    std::array<std::array<char, 96>, 32> description; // 原成员偏移：+0x29。
    std::array<char, 17> memo; // 原成员偏移：+0xC29。
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
    std::array<char, 17> name; // 原成员偏移：+0x5D4。
    std::array<char, 17> speciesName; // 原成员偏移：+0x5E5。
    std::array<std::byte, 2> reserved5F6; // 原成员偏移：+0x5F6。
    std::int32_t weight; // 原成员偏移：+0x5F8。
    std::int32_t maxWeight; // 原成员偏移：+0x5FC。
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
    std::array<std::array<char, 25>, 10> techniqueNames; // 原成员偏移：+0x48。
    std::array<char, 17> name; // 原成员偏移：+0x142。
    std::byte reserved153; // 原成员偏移：+0x153。
};

// 原CC4D58两组共享展示区；金额独立于物品／宠物区的清零操作。
struct WindowItemDetailGroup {
    std::array<WindowStorageItem, 20> items; // 原组内偏移：+0x0000。
    std::array<WindowStoragePetSummary, 5> pets; // 原组内偏移：+0xF550。
    std::int32_t amount; // 原组内偏移：+0xFBF4。
};

// 原CC3210独立详情；4B77E0整区清零／生产，六个S32仍属同一对象。
struct WindowShopDetail {
    std::array<char, 29> name; // 原成员偏移：+0x00。
    std::array<std::array<char, 96>, 32> description; // 原成员偏移：+0x1D。
    std::array<std::byte, 3> reservedC1D; // 原成员偏移：+0xC1D。
    std::int32_t imageNo; // 原成员偏移：+0xC20。
    std::int32_t quantity; // 原成员偏移：+0xC24。
    std::int32_t itemId; // 原成员偏移：+0xC28。
    std::int32_t price; // 原成员偏移：+0xC2C。
    std::int32_t capacity; // 原成员偏移：+0xC30。
    std::int32_t valueC34; // 原成员偏移：+0xC34；原分支显示值／现有数量，不归并用途。
};

// 原CC4894材料区与CC48A8商店模式；窗口共享同一24B对象，整区清零包含尾部。
struct WindowMaterialState {
    std::array<std::int32_t, 5> materialValues; // 原成员偏移：+0x00；材料占用值，打包时存材料等级。
    char shopMode; // 原成员偏移：+0x14；原全局：0x00CC48A8。 商店模式，保留原S8解释。
    std::array<std::byte, 3> reserved15; // 原成员偏移：+0x15；未确认用途，随整区清零。
};


static_assert(std::is_standard_layout_v<WindowStorageItem>);
static_assert(std::is_trivially_copyable_v<WindowStorageItem>);
static_assert(sizeof(WindowStorageItem) == 0xC44);
static_assert(offsetof(WindowStorageItem, used) == 0x00);
static_assert(offsetof(WindowStorageItem, selection) == 0x02);
static_assert(offsetof(WindowStorageItem, imageNo) == 0x04);
static_assert(offsetof(WindowStorageItem, count) == 0x08);
static_assert(offsetof(WindowStorageItem, name) == 0x0C);
static_assert(offsetof(WindowStorageItem, description) == 0x29);
static_assert(offsetof(WindowStorageItem, memo) == 0xC29);
static_assert(offsetof(WindowStorageItem, reservedC3A) == 0xC3A);
static_assert(offsetof(WindowStorageItem, pages) == 0xC3C);
static_assert(offsetof(WindowStorageItem, flags) == 0xC40);


static_assert(std::is_standard_layout_v<WindowStoragePet>);
static_assert(std::is_trivially_copyable_v<WindowStoragePet>);
static_assert(sizeof(WindowStoragePet) == 0x600 + sizeof(world::detail::PetTechniqueRecord) * (world::kPetTechniqueSlotCount - 10));
static_assert(offsetof(WindowStoragePet, used) == 0x00);
static_assert(offsetof(WindowStoragePet, tribe) == 0x02);
static_assert(offsetof(WindowStoragePet, imageNo) == 0x04);
static_assert(offsetof(WindowStoragePet, level) == 0x08);
static_assert(offsetof(WindowStoragePet, hp) == 0x0C);
static_assert(offsetof(WindowStoragePet, maxHp) == 0x10);
static_assert(offsetof(WindowStoragePet, fp) == 0x14);
static_assert(offsetof(WindowStoragePet, maxFp) == 0x18);
static_assert(offsetof(WindowStoragePet, combatStats) == 0x1C);
static_assert(offsetof(WindowStoragePet, baseStats) == 0x30);
static_assert(offsetof(WindowStoragePet, combatValue44) == 0x44);
static_assert(offsetof(WindowStoragePet, elements) == 0x48);
static_assert(offsetof(WindowStoragePet, value58) == 0x58);
static_assert(offsetof(WindowStoragePet, techniques) == 0x5C);
static_assert(offsetof(WindowStoragePet, name) == 0x5D4 + sizeof(world::detail::PetTechniqueRecord) * (world::kPetTechniqueSlotCount - 10));
static_assert(offsetof(WindowStoragePet, speciesName) == 0x5E5 + sizeof(world::detail::PetTechniqueRecord) * (world::kPetTechniqueSlotCount - 10));
static_assert(offsetof(WindowStoragePet, reserved5F6) == 0x5F6 + sizeof(world::detail::PetTechniqueRecord) * (world::kPetTechniqueSlotCount - 10));
static_assert(offsetof(WindowStoragePet, weight) == 0x5F8 + sizeof(world::detail::PetTechniqueRecord) * (world::kPetTechniqueSlotCount - 10));
static_assert(offsetof(WindowStoragePet, maxWeight) == 0x5FC + sizeof(world::detail::PetTechniqueRecord) * (world::kPetTechniqueSlotCount - 10));

static_assert(std::is_standard_layout_v<WindowStoragePetSummary>);
static_assert(std::is_trivially_copyable_v<WindowStoragePetSummary>);
static_assert(sizeof(WindowStoragePetSummary) == 0x154);
static_assert(offsetof(WindowStoragePetSummary, used) == 0x00);
static_assert(offsetof(WindowStoragePetSummary, selection) == 0x02);
static_assert(offsetof(WindowStoragePetSummary, tribe) == 0x04);
static_assert(offsetof(WindowStoragePetSummary, reserved06) == 0x06);
static_assert(offsetof(WindowStoragePetSummary, imageNo) == 0x08);
static_assert(offsetof(WindowStoragePetSummary, level) == 0x0C);
static_assert(offsetof(WindowStoragePetSummary, hp) == 0x10);
static_assert(offsetof(WindowStoragePetSummary, maxHp) == 0x14);
static_assert(offsetof(WindowStoragePetSummary, fp) == 0x18);
static_assert(offsetof(WindowStoragePetSummary, maxFp) == 0x1C);
static_assert(offsetof(WindowStoragePetSummary, combatStats) == 0x20);
static_assert(offsetof(WindowStoragePetSummary, elements) == 0x34);
static_assert(offsetof(WindowStoragePetSummary, value44) == 0x44);
static_assert(offsetof(WindowStoragePetSummary, techniqueNames) == 0x48);
static_assert(offsetof(WindowStoragePetSummary, name) == 0x142);
static_assert(offsetof(WindowStoragePetSummary, reserved153) == 0x153);

static_assert(std::is_standard_layout_v<WindowItemDetailGroup>);
static_assert(std::is_trivially_copyable_v<WindowItemDetailGroup>);
static_assert(sizeof(WindowItemDetailGroup) == 0xFBF8);
static_assert(offsetof(WindowItemDetailGroup, items) == 0x0000);
static_assert(offsetof(WindowItemDetailGroup, pets) == 0xF550);
static_assert(offsetof(WindowItemDetailGroup, amount) == 0xFBF4);


static_assert(std::is_standard_layout_v<WindowShopDetail>);
static_assert(std::is_trivially_copyable_v<WindowShopDetail>);
static_assert(sizeof(WindowShopDetail) == 0xC38);
static_assert(offsetof(WindowShopDetail, name) == 0x00);
static_assert(offsetof(WindowShopDetail, description) == 0x1D);
static_assert(offsetof(WindowShopDetail, reservedC1D) == 0xC1D);
static_assert(offsetof(WindowShopDetail, imageNo) == 0xC20);
static_assert(offsetof(WindowShopDetail, quantity) == 0xC24);
static_assert(offsetof(WindowShopDetail, itemId) == 0xC28);
static_assert(offsetof(WindowShopDetail, price) == 0xC2C);
static_assert(offsetof(WindowShopDetail, capacity) == 0xC30);
static_assert(offsetof(WindowShopDetail, valueC34) == 0xC34);

static_assert(std::is_standard_layout_v<WindowMaterialState>);
static_assert(std::is_trivially_copyable_v<WindowMaterialState>);
static_assert(sizeof(WindowMaterialState) == 0x18);
static_assert(alignof(WindowMaterialState) == alignof(std::int32_t));
static_assert(offsetof(WindowMaterialState, materialValues) == 0x00);
static_assert(offsetof(WindowMaterialState, shopMode) == 0x14);
static_assert(offsetof(WindowMaterialState, reserved15) == 0x15);

} // namespace cg::ui::detail
