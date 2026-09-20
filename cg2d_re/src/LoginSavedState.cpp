#include "LoginInputDependencies.hpp"

#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/Startup.hpp"

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <type_traits>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

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

// 原 sub_52D460@[0x52D460,0x52D569)：解码旧保存数据字节流。
[[maybe_unused]] std::int32_t DecodeLegacyBytes_52D460(
    const std::uint8_t* const source,
    const std::int32_t sourceBytes,
    const std::int32_t mode,
    std::uint8_t* const destination,
    std::int32_t* const decodedBytes)
{
    const std::int32_t outputBytes = sourceBytes - 1;
    const std::int32_t remainder = mode % outputBytes;
    *decodedBytes = outputBytes;

    const std::int32_t controlIndex =
        remainder < 0 ? -remainder : remainder;
    const std::uint8_t control = source[controlIndex];

    for (std::int32_t index = 0; index < sourceBytes; ++index) {
        const std::int32_t firstRemainder = mode % *decodedBytes;
        const std::int32_t firstControlIndex =
            firstRemainder < 0 ? -firstRemainder : firstRemainder;
        if (index < firstControlIndex) {
            destination[index] = static_cast<std::uint8_t>(
                source[index] -
                control * ((index * index) % 3));
        }

        const std::int32_t secondRemainder = mode % *decodedBytes;
        const std::int32_t secondControlIndex =
            secondRemainder < 0 ? -secondRemainder : secondRemainder;
        if (index > secondControlIndex) {
            destination[index - 1] = static_cast<std::uint8_t>(
                source[index] -
                control * ((index * index) % 7));
        }
    }

    std::int32_t lastDecodedBytes = *decodedBytes;
    std::int32_t index = 0;
    if (lastDecodedBytes <= 0) {
        return lastDecodedBytes;
    }

    const std::int32_t modeModulo7 = mode % 7;
    do {
        const std::int32_t indexModulo5 = index % 5;
        bool shouldInvert = modeModulo7 == indexModulo5;
        if (!shouldInvert) {
            const std::int32_t indexModulo2 = index % 2;
            const std::int32_t modeModulo2 = mode % 2;
            shouldInvert = modeModulo2 == indexModulo2;
        }
        if (shouldInvert) {
            destination[index] =
                static_cast<std::uint8_t>(~destination[index]);
        }

        lastDecodedBytes = *decodedBytes;
        ++index;
    } while (index < lastDecodedBytes);
    return lastDecodedBytes;
}

// 原 sub_532ED0@[0x532ED0,0x532FB9)：载入一条物理保存记录。
[[maybe_unused]] std::int32_t LoadSavedPhysicalRecord_532ED0(
    const std::int32_t physicalSlot)
{
    cg::platform::gValueFF4E28 = 0;

    char path[0x400];
    std::uint8_t encoded[0xA20];
    std::int32_t decodedBytes;
    std::sprintf(
        path,
        "%s\\%s",
        gSavedState.paths[physicalSlot].text,
        "save.dat");

    std::FILE* const file = std::fopen(path, "rb+");
    if (file == nullptr) {
        return 0;
    }

    const std::size_t bytesRead =
        std::fread(encoded, 1, 0xA20, file);
    if (std::ferror(file) != 0) {
        std::fclose(file);
        return 0;
    }

    std::fclose(file);
    (void)DecodeLegacyBytes_52D460(
        encoded,
        static_cast<std::int32_t>(bytesRead),
        0,
        reinterpret_cast<std::uint8_t*>(
            &gSavedState.records[physicalSlot]),
        &decodedBytes);
    if (decodedBytes != 0x510) {
        return 0;
    }
    return gSavedState.records[physicalSlot].parts[0].value0 == 0x7002
        ? 1
        : 0;
}

// 原 sub_5331A0@[0x5331A0,0x5332F6)：初始化一条物理保存记录。
[[maybe_unused]] char* InitializeSavedPhysicalRecord_5331A0(
    const std::int32_t physicalSlot)
{
    SavedPhysicalRecord& record = gSavedState.records[physicalSlot];
    std::memset(&record, 0, sizeof(record));
    auto* const recordBytes = reinterpret_cast<std::uint8_t*>(&record);

    record.parts[0].value0 = 0x7002;
    recordBytes[0x6A] = 0;
    recordBytes[0x37] = 0;

    const std::uint8_t effectVolume =
        static_cast<std::uint8_t>(cg::platform::gEffectVolume);
    recordBytes[0x04] = 0;

    const std::uint8_t useStereo =
        static_cast<std::uint8_t>(cg::platform::gUseStereo);
    recordBytes[0x9D] = useStereo;

    const std::uint8_t musicVolume =
        static_cast<std::uint8_t>(cg::platform::gMusicVolume);
    recordBytes[0x9E] = effectVolume;

    std::uint32_t bgmDword0;
    std::memcpy(
        &bgmDword0,
        cg::platform::gBgmFrequencySelectionPrefix,
        sizeof(bgmDword0));
    recordBytes[0x9F] = musicVolume;

    std::uint32_t bgmDword1;
    std::memcpy(
        &bgmDword1,
        cg::platform::gBgmFrequencySelectionPrefix + 4,
        sizeof(bgmDword1));
    std::memcpy(recordBytes + 0xA0, &bgmDword0, sizeof(bgmDword0));

    std::uint32_t bgmDword2;
    std::memcpy(
        &bgmDword2,
        cg::platform::gBgmFrequencySelectionPrefix + 8,
        sizeof(bgmDword2));
    std::memcpy(recordBytes + 0xA4, &bgmDword1, sizeof(bgmDword1));

    std::uint32_t bgmDword3;
    std::memcpy(
        &bgmDword3,
        cg::platform::gBgmFrequencySelectionPrefix + 12,
        sizeof(bgmDword3));
    std::memcpy(recordBytes + 0xA8, &bgmDword2, sizeof(bgmDword2));

    const std::uint8_t scanlineSyncMode =
        static_cast<std::uint8_t>(cg::platform::gScanlineSyncMode);
    std::memcpy(recordBytes + 0xAC, &bgmDword3, sizeof(bgmDword3));
    recordBytes[0xB0] = 0;
    recordBytes[0xB1] = 0;
    recordBytes[0x31A] = scanlineSyncMode;

    for (std::int32_t index = 0; index < 2; ++index) {
        recordBytes[0x30C + index] = 8;
        recordBytes[0x30E + index] = 5;
        recordBytes[0x310 + index] = 1;
    }

    char* cursor = reinterpret_cast<char*>(recordBytes + 0xB2);
    for (std::int32_t index = 0; index < 24; ++index) {
        *cursor = 0;
        cursor += 0x19;
    }
    recordBytes[0x312] = 0;
    recordBytes[0x313] = 0;
    recordBytes[0x314] = 0;
    return cursor;
}

// 原 sub_533360@[0x533360,0x5333CF)：复位保存状态。
[[maybe_unused]] std::int32_t ResetSavedState_533360()
{
    gSavedState.newPhysical = -1;
    gSavedState.visibleCount = 0;
    gSavedState.selected = 0;
    gSavedState.mappingCount = 0;

    for (std::int32_t index = 0; index < 10; ++index) {
        std::sprintf(
            gSavedState.paths[index].text,
            "data\\%d",
            index);
        gSavedState.active[index] = 0;
        gSavedState.logicalToPhysical[index] = -1;
        std::memset(
            &gSavedState.records[0],
            0,
            sizeof(gSavedState.records[0]));
    }
    return 0;
}

// 原 sub_5333D0@[0x5333D0,0x533461)：探测当前与旧版保存位置。
[[maybe_unused]] std::int32_t ProbeSavedStateLocation_5333D0()
{
    char path[0x400];
    for (std::int32_t index = 0; index < 10; ++index) {
        std::sprintf(
            path,
            "%s\\%s",
            gSavedState.paths[index].text,
            "save.dat");
        std::FILE* const file = std::fopen(path, "rb+");
        if (file != nullptr) {
            std::fclose(file);
            return 0;
        }
    }

    std::FILE* const legacyFile =
        std::fopen("data\\save.dat", "rb+");
    if (legacyFile != nullptr) {
        std::fclose(legacyFile);
        return 1;
    }
    return 2;
}

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

// 原 sub_533470@[0x533470,0x533528)：重建逻辑槽到物理槽映射。
[[maybe_unused]] std::int32_t RebuildSavedSlotMap_533470()
{
    for (std::int32_t physicalSlot = 0;
         physicalSlot < 10;
         ++physicalSlot) {
        if (LoadSavedPhysicalRecord_532ED0(physicalSlot) != 0) {
            const std::int32_t mappingIndex = gSavedState.mappingCount;
            gSavedState.logicalToPhysical[mappingIndex] = physicalSlot;
            gSavedState.active[physicalSlot] = 1;

            const std::int32_t currentMappingCount =
                gSavedState.mappingCount;
            const std::int32_t currentVisibleCount =
                gSavedState.visibleCount;
            const std::int32_t nextMappingCount = currentMappingCount + 1;
            const std::int32_t nextVisibleCount = currentVisibleCount + 1;
            gSavedState.mappingCount = nextMappingCount;
            gSavedState.visibleCount = nextVisibleCount;
        } else {
            gSavedState.active[physicalSlot] = 0;
        }
    }

    const bool skipAllocation = gSavedState.mappingCount > 10;
    if (!skipAllocation) {
        const std::int32_t freePhysicalSlot =
            FindFreeSavedPhysicalSlot_5338F0();
        if (freePhysicalSlot != -1) {
            const std::int32_t mappingIndex = gSavedState.mappingCount;
            gSavedState.logicalToPhysical[mappingIndex] = freePhysicalSlot;
            gSavedState.active[freePhysicalSlot] = 1;
            (void)InitializeSavedPhysicalRecord_5331A0(freePhysicalSlot);

            const std::int32_t currentMappingCount =
                gSavedState.mappingCount;
            const std::int32_t currentVisibleCount =
                gSavedState.visibleCount;
            const std::int32_t nextMappingCount = currentMappingCount + 1;
            const std::int32_t nextVisibleCount = currentVisibleCount + 1;
            gSavedState.newPhysical = freePhysicalSlot;
            gSavedState.mappingCount = nextMappingCount;
            gSavedState.visibleCount = nextVisibleCount;
        }
    }

    const std::int32_t finalMappingCount = gSavedState.mappingCount;
    return finalMappingCount > 0 ? 1 : 0;
}

// 原 sub_533530@[0x533530,0x533563)：迁移三份旧保存文件。
[[maybe_unused]] std::int32_t MigrateLegacySavedFiles_533530()
{
    (void)::CopyFileA(
        "data\\save.dat",
        "data\\0\\save.dat",
        FALSE);
    (void)::CopyFileA(
        "data\\mail.dat",
        "data\\0\\mail.dat",
        FALSE);
    return static_cast<std::int32_t>(::CopyFileA(
        "data\\guildmail.dat",
        "data\\0\\guildmail.dat",
        FALSE));
}

} // namespace

namespace cg::login::detail {

std::int32_t& gSelectedSavedSlot105C394 = gSavedState.selected;
std::int32_t& gVisibleSavedSlotCount105C364 = gSavedState.visibleCount;

void Sub_533570(
    const char* const source,
    const std::int32_t logicalSlot)
{
    if (source == nullptr) {
        return;
    }

    for (std::int32_t index = 0; index < 51; ++index) {
        const std::int32_t physicalSlot =
            gSavedState.logicalToPhysical[logicalSlot];
        gSavedState.records[physicalSlot].parts[0].text[index] =
            source[index];
        if (source[index] == '\0') {
            return;
        }
    }
}

void Sub_5335B0(
    const char* const source,
    const std::int32_t logicalSlot)
{
    if (source == nullptr) {
        return;
    }

    for (std::int32_t index = 0; index < 51; ++index) {
        const std::int32_t physicalSlot =
            gSavedState.logicalToPhysical[logicalSlot];
        gSavedState.records[physicalSlot].parts[0].text[0x33 + index] =
            source[index];
        if (source[index] == '\0') {
            return;
        }
    }
}

void Sub_533640(
    const char* const source,
    const std::int32_t logicalSlot)
{
    if (source == nullptr) {
        return;
    }

    for (std::int32_t index = 0; index < 51; ++index) {
        const std::int32_t physicalSlot =
            gSavedState.logicalToPhysical[logicalSlot];
        gSavedState.records[physicalSlot].parts[0].text[0x66 + index] =
            source[index];
        if (source[index] == '\0') {
            return;
        }
    }
}

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

char* Sub_533680(
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
            gSavedState.records[physicalSlot].parts[0].text[0x66 + index];
        output[index] = value;
        if (value == '\0') {
            return output;
        }
    }
    return output;
}

} // namespace cg::login::detail
