#include "LoginInputDependencies.hpp"

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace {

struct SavedPath final {
    char text[0x100];
};

struct SavedRecordPart final {
    std::int32_t value0;
    char text[316];
    std::uint8_t tailBytes[4];
};

struct SavedPhysicalRecord final {
    SavedRecordPart parts[4];
};

struct SavedStateStorage final {
    SavedPath paths[10];
    SavedPhysicalRecord records[10];
    std::int32_t active[10];
    std::int32_t mappingCount;
    std::int32_t visibleCount;
    std::int32_t newPhysical;
    std::int32_t logicalToPhysical[10];
    std::int32_t selected;
};

static_assert(std::is_standard_layout_v<SavedPath>);
static_assert(std::is_trivially_copyable_v<SavedPath>);
static_assert(sizeof(SavedPath) == 0x100);
static_assert(offsetof(SavedPath, text) == 0x0);

static_assert(std::is_standard_layout_v<SavedRecordPart>);
static_assert(std::is_trivially_copyable_v<SavedRecordPart>);
static_assert(sizeof(SavedRecordPart) == 0x144);
static_assert(offsetof(SavedRecordPart, value0) == 0x0);
static_assert(offsetof(SavedRecordPart, text) == 0x4);
static_assert(offsetof(SavedRecordPart, tailBytes) == 0x140);

static_assert(std::is_standard_layout_v<SavedPhysicalRecord>);
static_assert(std::is_trivially_copyable_v<SavedPhysicalRecord>);
static_assert(sizeof(SavedPhysicalRecord) == 0x510);
static_assert(offsetof(SavedPhysicalRecord, parts) == 0x0);

static_assert(std::is_standard_layout_v<SavedStateStorage>);
static_assert(std::is_trivially_copyable_v<SavedStateStorage>);
static_assert(sizeof(SavedStateStorage) == 0x3D00);
static_assert(offsetof(SavedStateStorage, paths) == 0x0000);
static_assert(offsetof(SavedStateStorage, records) == 0x0A00);
static_assert(offsetof(SavedStateStorage, active) == 0x3CA0);
static_assert(offsetof(SavedStateStorage, mappingCount) == 0x3CC8);
static_assert(offsetof(SavedStateStorage, visibleCount) == 0x3CCC);
static_assert(offsetof(SavedStateStorage, newPhysical) == 0x3CD0);
static_assert(offsetof(SavedStateStorage, logicalToPhysical) == 0x3CD4);
static_assert(offsetof(SavedStateStorage, selected) == 0x3CFC);

// 原连续区间 [0x1058698,0x105C398)，PE 中为 virtual-only 零填充。
SavedStateStorage gSavedState {};

// 原 sub_5338F0@[0x5338F0,0x53390C)：查找首个未占用物理槽。
[[maybe_unused]] std::int32_t
FindFreeSavedPhysicalSlot_5338F0() noexcept
{
    for (std::int32_t index = 0; index < 10; ++index) {
        if (gSavedState.active[index] == 0) {
            return index;
        }
    }
    return -1;
}

} // namespace

namespace cg::login::detail {

std::int32_t& gSelectedSavedSlot105C394 = gSavedState.selected;
std::int32_t& gVisibleSavedSlotCount105C364 = gSavedState.visibleCount;

char* Sub_5335F0(
    char* const output,
    const std::int32_t logicalSlot)
{
    if (output == nullptr) {
        return output;
    }

    if (gSavedState.logicalToPhysical[logicalSlot] == -1) {
        output[0] = '\0';
        return output;
    }

    for (std::int32_t index = 0; index < 51; ++index) {
        const std::int32_t physicalSlot =
            gSavedState.logicalToPhysical[logicalSlot];
        const char value =
            gSavedState.records[physicalSlot].parts[0].text[0x33 + index];
        output[index] = value;
        if (value == '\0') {
            return output;
        }
    }
    return output;
}

} // namespace cg::login::detail
