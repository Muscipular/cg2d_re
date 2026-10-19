#include "cg/core/LegacyNoOp.hpp"
#include "LoginSavedState.hpp"
#include "../character_select/CharacterSelectSceneDependencies.hpp"
#include "LoginInputDependencies.hpp"
#include "LoginSceneDependencies.hpp"
#include "../../ui/WindowRuntimeState.hpp"

#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/Startup.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include <Windows.h>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BCCD、0x0053C680；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

#include "cg/core/m_log.hpp"

namespace cg::login::detail {

struct SavedPath final {
    char text[0x100]; // 原成员偏移：+0x00。
};

struct SavedRecordPart final {
    std::int32_t value0; // 原成员偏移：+0x00。
    char text[316]; // 原成员偏移：+0x04。
    std::uint8_t tailBytes[4]; // 原成员偏移：+0x140。
};

struct SavedPhysicalRecord final {
    SavedRecordPart parts[4]; // 原成员偏移：+0x00。
};

struct SavedStateStorage final {
    SavedPath paths[10]; // 原成员偏移：+0x00。
    SavedPhysicalRecord records[10]; // 原成员偏移：+0xA00。
    std::int32_t active[10]; // 原成员偏移：+0x3CA0。
    std::int32_t mappingCount; // 原成员偏移：+0x3CC8。
    std::int32_t visibleCount; // 原成员偏移：+0x3CCC。
    std::int32_t newPhysical; // 原成员偏移：+0x3CD0。
    std::int32_t logicalToPhysical[10]; // 原成员偏移：+0x3CD4。
    std::int32_t selected; // 原成员偏移：+0x3CFC。
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
static SavedStateStorage gSavedState {}; // 原全局：0x01058698。

// 原当前保存记录 [0xFF4918,0xFF4E28)，完整 0x510 字节，PE virtual-only 零填充。
static SavedPhysicalRecord gCurrentSavedRecord {}; // 原全局：0x00FF4918。

// ALM02：只标记自动参数已写入的物理账号槽，凭据仍由上述两个既有记录持有。
static std::int32_t gAutoLoginCachedPhysicalSlot = -1;
static_assert(LoginInputValueCapacity == 0x33);

// 原 sub_52D310@[0x52D310,0x52D457)：编码旧保存数据字节流。
void EncodeLegacyBytes_52D310(
    std::uint8_t* const source,
    const std::int32_t sourceBytes,
    const std::int32_t mode,
    std::uint8_t* const destination,
    std::int32_t* const encodedBytes,
    const std::int32_t destinationCapacity)
{
    const std::int32_t required = sourceBytes + 1;
    std::uint8_t control = 0;

    if (required > destinationCapacity) {
        *encodedBytes = destinationCapacity;
        if (destinationCapacity > 0) {
            std::int32_t index = 0;
            do {
                const std::uint8_t value = source[index];
                destination[index] = value;

                ++index;
                const std::int32_t currentEncodedBytes = *encodedBytes;
                if (index >= currentEncodedBytes) {
                    break;
                }
            } while (true);
        }
        return;
    }

    *encodedBytes = required;
    if (sourceBytes > 0) {
        const std::int32_t modeModulo7 = mode % 7;
        for (std::int32_t index = 0; index < sourceBytes; ++index) {
            const std::uint8_t original = source[index];
            control = static_cast<std::uint8_t>(
                static_cast<std::uint32_t>(control) +
                static_cast<std::uint32_t>(original));

            const std::int32_t indexModulo5 = index % 5;
            bool shouldInvert = modeModulo7 == indexModulo5;
            if (!shouldInvert) {
                const std::int32_t indexModulo2 = index % 2;
                const std::int32_t modeModulo2 = mode % 2;
                shouldInvert = modeModulo2 == indexModulo2;
            }
            if (shouldInvert) {
                source[index] = static_cast<std::uint8_t>(~original);
            }
        }
    }

    const std::int32_t initialEncodedBytes = *encodedBytes;
    if (initialEncodedBytes <= 0) {
        return;
    }

    const std::int32_t remainder = mode % sourceBytes;
    const std::int32_t controlIndex =
        remainder < 0 ? -remainder : remainder;
    std::int32_t index = 0;
    do {
        if (index < controlIndex) {
            const std::int32_t adjustment = control * ((index * index) % 3);
            const std::uint8_t value = source[index];
            destination[index] = static_cast<std::uint8_t>(value + adjustment);
        } else if (index == controlIndex) {
            destination[index] = control;
        } else {
            const std::int32_t adjustment = control * ((index * index) % 7);
            const std::uint8_t value = source[index - 1];
            destination[index] = static_cast<std::uint8_t>(value + adjustment);
        }

        ++index;
        const std::int32_t currentEncodedBytes = *encodedBytes;
        if (index >= currentEncodedBytes) {
            break;
        }
    } while (true);
}

// 原 sub_52D460@[0x52D460,0x52D569)：解码旧保存数据字节流。
std::int32_t DecodeLegacyBytes_52D460(
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
static std::int32_t LoadSavedPhysicalRecord_532ED0(
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

    std::FILE* const file =
        std::fopen(path, "rb+");
    if (file == nullptr) {
        return 0;
    }

    const std::uint32_t bytesRead =
        static_cast<std::uint32_t>(std::fread(encoded, 1, 0xA20, file));
    // 原532F3F/532F45只读取flags低U8并测试20H错误位。
    if (std::ferror(file) != 0) {
        std::fclose(file);
        return 0;
    }

    std::fclose(file);
    (void)cg::login::detail::DecodeLegacyBytes_52D460(
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

// 原 sub_532FC0@[0x532FC0,0x53319A)：写出一份默认保存记录。
static std::int32_t WriteDefaultSavedFile_532FC0(
    const char* const fileName)
{
    SavedPhysicalRecord raw;
    std::memset(&raw, 0, sizeof(raw));
    auto* const rawBytes = reinterpret_cast<std::uint8_t*>(&raw);

    raw.parts[0].value0 = 0x7002;
    rawBytes[0x6A] = 0;
    rawBytes[0x37] = 0;

    const std::uint8_t effectVolume =
        static_cast<std::uint8_t>(cg::platform::gEffectVolume);
    const std::uint8_t musicVolume =
        static_cast<std::uint8_t>(cg::platform::gMusicVolume);
    rawBytes[0x04] = 0;

    const std::uint8_t useStereo =
        static_cast<std::uint8_t>(cg::platform::gUseStereo);
    rawBytes[0x9E] = effectVolume;

    std::uint32_t bgmDwordAt4;
    std::memcpy(
        &bgmDwordAt4,
        cg::platform::gBgmFrequencySelectionPrefix + 4,
        sizeof(bgmDwordAt4));
    rawBytes[0x9D] = useStereo;

    std::uint32_t bgmDwordAt0;
    std::memcpy(
        &bgmDwordAt0,
        cg::platform::gBgmFrequencySelectionPrefix,
        sizeof(bgmDwordAt0));
    rawBytes[0x9F] = musicVolume;

    std::uint32_t bgmDwordAt8;
    std::memcpy(
        &bgmDwordAt8,
        cg::platform::gBgmFrequencySelectionPrefix + 8,
        sizeof(bgmDwordAt8));
    std::memcpy(
        rawBytes + 0xA0,
        &bgmDwordAt0,
        sizeof(bgmDwordAt0));

    std::uint32_t bgmDwordAt12;
    std::memcpy(
        &bgmDwordAt12,
        cg::platform::gBgmFrequencySelectionPrefix + 12,
        sizeof(bgmDwordAt12));
    std::memcpy(
        rawBytes + 0xA4,
        &bgmDwordAt4,
        sizeof(bgmDwordAt4));

    const std::uint8_t scanlineSyncMode =
        static_cast<std::uint8_t>(cg::platform::gScanlineSyncMode);
    std::memcpy(
        rawBytes + 0xA8,
        &bgmDwordAt8,
        sizeof(bgmDwordAt8));
    std::memcpy(
        rawBytes + 0xAC,
        &bgmDwordAt12,
        sizeof(bgmDwordAt12));
    rawBytes[0xB0] = 0;
    rawBytes[0xB1] = 0;
    rawBytes[0x31A] = scanlineSyncMode;

    for (std::int32_t index = 0; index < 2; ++index) {
        rawBytes[0x30C + index] = 8;
        rawBytes[0x30E + index] = 5;
        rawBytes[0x310 + index] = 1;
    }

    std::uint8_t* cursor = rawBytes + 0xB2;
    for (std::int32_t index = 0; index < 24; ++index) {
        *cursor = 0;
        cursor += 0x19;
    }
    rawBytes[0x312] = 0;
    rawBytes[0x313] = 0;
    rawBytes[0x314] = 0;

    std::uint8_t encoded[0xA20];
    std::int32_t encodedBytes;
    cg::login::detail::EncodeLegacyBytes_52D310(
        rawBytes,
        0x510,
        0,
        encoded,
        &encodedBytes,
        0xA20);

    std::FILE* const file =
        std::fopen(fileName, "wb+");
    if (file == nullptr) {
        return 0;
    }

    const std::uint32_t bytesWritten = static_cast<std::uint32_t>(std::fwrite(
        encoded,
        1,
        static_cast<std::uint32_t>(encodedBytes),
        file));
    if (bytesWritten < static_cast<std::uint32_t>(encodedBytes)) {
        return 0;
    }

    std::fclose(file);
    return 1;
}

// 原 sub_5331A0@[0x5331A0,0x5332F6)：初始化一条物理保存记录。
static char* InitializeSavedPhysicalRecord_5331A0(
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
static std::int32_t ResetSavedState_533360()
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
static std::int32_t ProbeSavedStateLocation_5333D0()
{
    char path[0x400];
    for (std::int32_t index = 0; index < 10; ++index) {
        std::sprintf(
            path,
            "%s\\%s",
            gSavedState.paths[index].text,
            "save.dat");
        std::FILE* const file =
            std::fopen(path, "rb+");
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
static std::int32_t
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
static std::int32_t RebuildSavedSlotMap_533470()
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
static std::int32_t MigrateLegacySavedFiles_533530()
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

// 原 sub_5336D0@[0x5336D0,0x5336F5)：载入当前选中物理记录。
static std::int32_t LoadSelectedPhysicalRecord_5336D0()
{
    const std::int32_t selected = gSavedState.selected;
    const std::uint32_t resultBits =
        std::bit_cast<std::uint32_t>(selected) * 9U;
    std::memcpy(
        &gCurrentSavedRecord,
        &gSavedState.records[selected],
        sizeof(gCurrentSavedRecord));
    return std::bit_cast<std::int32_t>(resultBits);
}

// 原 sub_533700@[0x533700,0x533725)：把当前记录写回选中物理记录。
static std::int32_t StoreCurrentRecordToSelectedPhysical_533700()
{
    const std::int32_t selected = gSavedState.selected;
    const std::uint32_t resultBits =
        std::bit_cast<std::uint32_t>(selected) * 9U;
    std::memcpy(
        &gSavedState.records[selected],
        &gCurrentSavedRecord,
        sizeof(gCurrentSavedRecord));
    return std::bit_cast<std::int32_t>(resultBits);
}

// 原 sub_520200@[0x520200,0x5202DE)：持久化当前保存记录。
static std::int32_t PersistCurrentSavedRecord_520200()
{
    SavedPhysicalRecord localRecord;
    char path[0x400];
    std::uint8_t encoded[0xA20];
    std::int32_t encodedBytes;

    std::memcpy(
        &localRecord,
        &gCurrentSavedRecord,
        sizeof(localRecord));
    auto* const localBytes =
        reinterpret_cast<std::uint8_t*>(&localRecord);
    std::memset(localBytes + 0x37, 0, 0x33);
    cg::login::detail::EncodeLegacyBytes_52D310(
        localBytes,
        0x510,
        0,
        encoded,
        &encodedBytes,
        0xA20);

    const std::int32_t selected = gSavedState.selected;
    const std::int32_t physicalSlot =
        gSavedState.logicalToPhysical[selected];
    std::sprintf(
        path,
        "%s\\%s",
        gSavedState.paths[physicalSlot].text,
        "save.dat");

    std::FILE* const file =
        std::fopen(path, "wb+");
    std::int32_t result = file != nullptr ? 1 : 0;
    const std::uint32_t bytesWritten = static_cast<std::uint32_t>(std::fwrite(
        encoded,
        1,
        static_cast<std::uint32_t>(encodedBytes),
        file));
    const std::uint32_t expectedBytes =
        static_cast<std::uint32_t>(encodedBytes);
    if (bytesWritten < expectedBytes) {
        result = 0;
    }
    std::fclose(file);
    (void)StoreCurrentRecordToSelectedPhysical_533700();
    return result;
}

// 原 sub_533860@[0x533860,0x5338E1)：删除一组物理槽保存文件。
static std::int32_t DeleteSavedFiles_533860(
    const std::int32_t physicalSlot)
{
    if (gAutoLoginCachedPhysicalSlot == physicalSlot) {
        gAutoLoginCachedPhysicalSlot = -1;
    }
    char path[0x400];

    std::sprintf(
        path,
        "%s\\%s",
        gSavedState.paths[physicalSlot].text,
        "save.dat");
    (void)::DeleteFileA(path);

    std::sprintf(
        path,
        "%s\\%s",
        gSavedState.paths[physicalSlot].text,
        "mail.dat");
    (void)::DeleteFileA(path);

    std::sprintf(
        path,
        "%s\\%s",
        gSavedState.paths[physicalSlot].text,
        "guildmail.dat");
    return static_cast<std::int32_t>(::DeleteFileA(path));
}

// 存档无参调用适配；原4A4210不读取参数，共用窗口侧真实实现。
static std::int32_t ReturnOne_4A4210() noexcept
{
    return cg::ui::detail::WindowRuntime_ReturnOne(0);
}

// 原 sub_5204C0@[0x5204C0,0x520502)：从记录恢复音频设置。
static std::int32_t RestoreAudioSettings_5204C0(
    const std::uint8_t* const currentBytes)
{
    cg::platform::gUseStereo =
        static_cast<std::int32_t>(currentBytes[0x9D]);
    cg::platform::gEffectVolume =
        static_cast<std::int32_t>(currentBytes[0x9E]);
    cg::platform::gMusicVolume =
        static_cast<std::int32_t>(currentBytes[0x9F]);

    std::int32_t index = 0;
    for (; index < 16; ++index) {
        const std::uint8_t value = currentBytes[0xA0 + index];
        cg::platform::gBgmFrequencySelectionPrefix[index] = value;
    }
    return index;
}

// 原 sub_5205E0@[0x5205E0,0x52063B)：从记录恢复六项标量设置。
static std::uint8_t* RestoreSavedScalarSettings_5205E0(
    std::uint8_t* const source)
{
    cg::platform::gValueD67AE4 =
        static_cast<std::uint32_t>(source[0xB0]);
    cg::platform::gValue73ECBC =
        static_cast<std::int32_t>(source[0xB1]);
    cg::platform::gValueD67AE8 =
        static_cast<std::uint32_t>(source[0x312]);
    cg::platform::gValue5F974C =
        static_cast<std::int16_t>(source[0x314]);
    cg::platform::gScanlineSyncMode =
        static_cast<std::uint16_t>(source[0x31A]);
    cg::platform::gValueD68858 =
        static_cast<std::uint32_t>(source[0x31B]);
    return source;
}

// 原 sub_520690@[0x520690,0x5206EE)：恢复 24 项保存输入文本。
static std::int32_t RestoreSavedInputContexts_520690(
    const std::uint8_t* const source)
{
    const std::uint8_t* sourceText = source + 0xB2;
    for (std::size_t contextIndex = 0; contextIndex < 24;
         ++contextIndex, sourceText += 0x19) {
        const std::uint8_t* sourceEnd = sourceText;
        while (*sourceEnd != 0) {
            ++sourceEnd;
        }

        cg::input::InputContext& context = cg::ui::detail::gInputContexts5F9758[contextIndex];
        context.setText(cg::text::FromGbkWide(std::string_view(reinterpret_cast<const char*>(sourceText), static_cast<std::size_t>(sourceEnd - sourceText))));
        context.totalByteLength = static_cast<std::int32_t>(context.text.size());
        context.caretByteOffset = context.totalByteLength;
    }
    return 0;
}

// 原 sub_520700@[0x520700,0x520712)：从记录恢复单字节状态。
static std::uint32_t RestoreSavedByteState_520700(
    const std::uint8_t* const source)
{
    const std::uint32_t value =
        static_cast<std::uint32_t>(source[0x313]);
    cg::platform::gContactSortMode_D67CD0 = value;
    return value;
}

// 原 sub_520730@[0x520730,0x520744)：从当前记录恢复条件式单字节值。
static std::uint8_t RestoreCurrentSavedValue_520730()
{
    const auto* const currentBytes =
        reinterpret_cast<const std::uint8_t*>(&gCurrentSavedRecord);
    const std::uint8_t value = currentBytes[0x315];
    if (value != 0) {
        cg::platform::gValue553438 = static_cast<std::int32_t>(value);
    }
    return value;
}

// 原函数：0x005201B0。
std::int32_t RestoreCurrentSavedState_5201B0()
{
    if (ReturnOne_4A4210() == 0) {
        return 0;
    }

    auto* const currentBytes =
        reinterpret_cast<std::uint8_t*>(&gCurrentSavedRecord);
    (void)RestoreAudioSettings_5204C0(currentBytes);
    (void)RestoreSavedScalarSettings_5205E0(currentBytes);
    (void)RestoreSavedInputContexts_520690(currentBytes);
    (void)RestoreSavedByteState_520700(currentBytes);
    (void)RestoreCurrentSavedValue_520730();
    return 1;
}

// 原函数：0x00533300。
std::int32_t InitializeSavedState_533300()
{
    // 返回入口会重读磁盘；仅保留自动登录槽的两个内存字段，不将密码写入存档。
    const std::int32_t cachedPhysicalSlot = gAutoLoginCachedPhysicalSlot;
    char cachedCredentials[2 * LoginInputValueCapacity];
    if (cachedPhysicalSlot != -1) {
        std::memcpy(cachedCredentials,
            gSavedState.records[cachedPhysicalSlot].parts[0].text + 0x33,
            sizeof(cachedCredentials));
    }
    (void)ResetSavedState_533360();
    const std::int32_t location = ProbeSavedStateLocation_5333D0();
    if (location == 0) {
        (void)RebuildSavedSlotMap_533470();
    } else if (location == 1) {
        (void)MigrateLegacySavedFiles_533530();
        (void)RebuildSavedSlotMap_533470();
    } else if (location == 2) {
        (void)WriteDefaultSavedFile_532FC0("data\\0\\save.dat");
        (void)RebuildSavedSlotMap_533470();
    }

    if (cachedPhysicalSlot != -1) {
        std::memcpy(
            gSavedState.records[cachedPhysicalSlot].parts[0].text + 0x33,
            cachedCredentials, sizeof(cachedCredentials));
    }
    const std::int32_t physicalSlot =
        gSavedState.logicalToPhysical[0];
    std::memcpy(
        &gCurrentSavedRecord,
        &gSavedState.records[physicalSlot],
        sizeof(gCurrentSavedRecord));
    return 1;
}

// 原 [0x520510,0x520545)：四项显示状态交错读写当前保存记录。
std::int32_t CaptureCharacterDisplaySettings_520510(const std::int32_t selected)
{
    auto* const currentBytes =
        reinterpret_cast<std::uint8_t*>(&gCurrentSavedRecord);
    const volatile std::int32_t& color =
        cg::ui::detail::gInputContext626AA0.field140;
    const volatile std::int32_t& first = cg::platform::gValue55463C;
    const volatile std::int32_t& second = cg::platform::gValue554640;
    const volatile std::int32_t& third = cg::platform::gValue554644;
    const std::uint8_t colorByte = static_cast<std::uint8_t>(color);
    const std::uint8_t firstByte = static_cast<std::uint8_t>(first);
    currentBytes[0x30A + selected] = colorByte;
    const std::uint8_t secondByte = static_cast<std::uint8_t>(second);
    currentBytes[0x30C + selected] = firstByte;
    const std::uint8_t thirdByte = static_cast<std::uint8_t>(third);
    currentBytes[0x30E + selected] = secondByte;
    currentBytes[0x310 + selected] = thirdByte;
    return selected;
}

// 原 [0x520550,0x52058D)：按两组读取、零扩展并发布四项显示状态。
std::int32_t RestoreCharacterDisplaySettings_520550(const std::int32_t selected)
{
    const auto* const currentBytes =
        reinterpret_cast<const std::uint8_t*>(&gCurrentSavedRecord);
    const std::int32_t color = currentBytes[0x30A + selected];
    const std::int32_t first = currentBytes[0x30C + selected];
    cg::ui::detail::gInputContext626AA0.field140 = color;
    cg::platform::gValue55463C = first;
    const std::int32_t second = currentBytes[0x30E + selected];
    const std::int32_t third = currentBytes[0x310 + selected];
    cg::platform::gValue554640 = second;
    cg::platform::gValue554644 = third;
    return selected;
}

// 原 [0x520750,0x520764)：只存 BGM 目标低 W16，返回原索引。
std::int32_t CaptureCharacterBgmTarget_520750(const std::int32_t selected)
{
    auto* const currentBytes =
        reinterpret_cast<std::uint8_t*>(&gCurrentSavedRecord);
    const std::uint16_t target =
        static_cast<std::uint16_t>(cg::audio::gBgmTarget1050818);
    std::memcpy(currentBytes + 0x316 + selected * 2, &target, sizeof(target));
    return selected;
}

// 原 [0x520770,0x520784)：从当前记录的 W16 零扩展恢复同一 BGM 目标。
std::int32_t RestoreCharacterBgmTarget_520770(const std::int32_t selected)
{
    const auto* const currentBytes =
        reinterpret_cast<const std::uint8_t*>(&gCurrentSavedRecord);
    std::uint16_t target;
    std::memcpy(&target, currentBytes + 0x316 + selected * 2, sizeof(target));
    cg::audio::gBgmTarget1050818 = target;
    return target;
}

// 原 sub_520470@[0x520470,0x5204BF)：将音频设置写入当前保存记录。
std::int32_t CaptureAudioPreferences_520470()
{
    auto* const currentBytes =
        reinterpret_cast<std::uint8_t*>(&gCurrentSavedRecord);
    const std::uint8_t stereo =
        static_cast<std::uint8_t>(cg::platform::gUseStereo);
    const std::uint8_t effect =
        static_cast<std::uint8_t>(cg::platform::gEffectVolume);
    const std::uint8_t music =
        static_cast<std::uint8_t>(cg::platform::gMusicVolume);
    const std::uint8_t* const frequencies =
        cg::platform::gBgmFrequencySelectionPrefix;
    std::uint32_t first;
    std::uint32_t second;
    std::uint32_t third;
    std::uint32_t fourth;

    currentBytes[0x9D] = stereo;
    std::memcpy(&first, frequencies, sizeof(first));
    currentBytes[0x9E] = effect;
    std::memcpy(&second, frequencies + 4, sizeof(second));
    currentBytes[0x9F] = music;
    std::memcpy(&third, frequencies + 8, sizeof(third));
    std::memcpy(currentBytes + 0xA0, &first, sizeof(first));
    std::memcpy(&fourth, frequencies + 12, sizeof(fourth));
    std::memcpy(currentBytes + 0xA4, &second, sizeof(second));
    std::memcpy(currentBytes + 0xA8, &third, sizeof(third));
    std::memcpy(currentBytes + 0xAC, &fourth, sizeof(fourth));
    return std::bit_cast<std::int32_t>(fourth);
}

// 原 sub_520590@[0x520590,0x5205D5)：将六项标量低字节写入当前记录。
std::uint8_t CaptureClientPreferenceBytes_520590()
{
    auto* const currentBytes =
        reinterpret_cast<std::uint8_t*>(&gCurrentSavedRecord);
    const std::uint8_t show =
        static_cast<std::uint8_t>(cg::platform::gValueD67AE4);
    const std::uint8_t value73ECBC =
        static_cast<std::uint8_t>(cg::platform::gValue73ECBC);
    const std::uint8_t valueD67AE8 =
        static_cast<std::uint8_t>(cg::platform::gValueD67AE8);
    currentBytes[0xB0] = show;
    const std::uint8_t value5F974C =
        static_cast<std::uint8_t>(cg::platform::gValue5F974C);
    currentBytes[0xB1] = value73ECBC;
    const std::uint8_t scanlineSyncMode =
        static_cast<std::uint8_t>(cg::platform::gScanlineSyncMode);
    currentBytes[0x312] = valueD67AE8;
    const std::uint8_t valueD68858 =
        static_cast<std::uint8_t>(cg::platform::gValueD68858);
    currentBytes[0x314] = value5F974C;
    currentBytes[0x31A] = scanlineSyncMode;
    currentBytes[0x31B] = valueD68858;
    return value5F974C;
}

// 原 sub_520640@[0x520640,0x520681)：保存 24 个输入上下文的零结尾文本。
std::uint32_t CapturePresetChatTexts_520640()
{
    auto* const currentBytes =
        reinterpret_cast<std::uint8_t*>(&gCurrentSavedRecord);
    std::array<std::string, 24> storedTexts;
    for (std::size_t index = 0; index < storedTexts.size(); ++index) {
        storedTexts[index] = cg::text::ToGbk(cg::input::ContextText(cg::ui::detail::gInputContexts5F9758[index]));
        if (storedTexts[index].size() >= 0x19) {
            throw cg::text::GbkStorageError(122, std::system_category(), "快捷文字超过原存档字段容量");
        }
    }
    std::uint32_t result = 0;
    for (std::size_t index = 0; index < 24; ++index) {
        const char* const text = storedTexts[index].c_str();
        const std::size_t bytes = std::strlen(text) + 1;
        result = static_cast<std::uint32_t>(bytes);
        std::memcpy(currentBytes + 0xB2 + index * 0x19, text, bytes);
    }
    return result;
}

// 原 sub_5206F0@[0x5206F0,0x5206FB)：保存单字节状态并返回 AL。
std::uint8_t CaptureContactSortMode_5206F0()
{
    const std::uint8_t value =
        static_cast<std::uint8_t>(cg::platform::gContactSortMode_D67CD0);
    auto* const currentBytes =
        reinterpret_cast<std::uint8_t*>(&gCurrentSavedRecord);
    currentBytes[0x313] = value;
    return value;
}

// 原 sub_520720@[0x520720,0x52072B)：保存 +0x315 的低字节并返回 AL。
std::uint8_t Sub_520720()
{
    const std::uint8_t value =
        static_cast<std::uint8_t>(cg::platform::gValue553438);
    auto* const currentBytes =
        reinterpret_cast<std::uint8_t*>(&gCurrentSavedRecord);
    currentBytes[0x315] = value;
    return value;
}

// 原 [0x520160,0x5201A8)：各保存叶按序执行，末尾透传持久化返回。
std::int32_t SaveCurrentClientPreferences_520160()
try {
    const volatile std::int16_t& selectedCharacter =
        cg::login::gSelectedCharacterIndex;
    static_cast<void>(CaptureAudioPreferences_520470());
    static_cast<void>(CaptureClientPreferenceBytes_520590());

    const std::int32_t ignoredSelected = selectedCharacter;
    static_cast<void>(ignoredSelected);
    cg::core::NullSub_509080();

    const std::int32_t selectedForDisplay = selectedCharacter;
    static_cast<void>(CaptureCharacterDisplaySettings_520510(selectedForDisplay));
    static_cast<void>(CapturePresetChatTexts_520640());
    static_cast<void>(CaptureContactSortMode_5206F0());

    const std::int32_t selectedForAudio = selectedCharacter;
    static_cast<void>(CaptureCharacterBgmTarget_520750(selectedForAudio));
    static_cast<void>(Sub_520720());
    return SaveCurrentLoginRecord_520200();
}
catch (const cg::text::GbkStorageError&) {
    m_log_iw(L"快捷文字无法按GBK保存，或已超出存档容量。");
    return -1;
}

// 原函数：0x00520200。
std::int32_t SaveCurrentLoginRecord_520200()
{
    return PersistCurrentSavedRecord_520200();
}

std::int32_t ReadNewSavedPhysicalSlot105C368()
{
    return gSavedState.newPhysical;
}

std::int32_t ReadMappedSavedPhysicalSlot105C36C(
    const std::int32_t index)
{
    return gSavedState.logicalToPhysical[index];
}

const char* ReadSelectedSavedDirectory()
{
    const std::int32_t selected = gSavedState.selected;
    const std::int32_t physicalSlot = gSavedState.logicalToPhysical[selected];
    return gSavedState.paths[physicalSlot].text;
}

// 原函数：0x005336D0。
std::int32_t LoadSelectedLoginRecord_5336D0()
{
    return LoadSelectedPhysicalRecord_5336D0();
}

// 原函数：0x00533860。
std::int32_t DeleteSavedAccountFiles_533860(const std::int32_t physicalSlot)
{
    return DeleteSavedFiles_533860(physicalSlot);
}

void RestoreCapability4CurrentSavedRecord()
{
    auto* const currentBytes =
        reinterpret_cast<std::uint8_t*>(&gCurrentSavedRecord);
    (void)RestoreAudioSettings_5204C0(currentBytes);
    (void)RestoreSavedScalarSettings_5205E0(currentBytes);
    (void)RestoreSavedInputContexts_520690(currentBytes);
    (void)RestoreSavedByteState_520700(currentBytes);
}

void CacheAutoLoginInputValues(const char* const account, const char* const password)
{
    const std::int32_t selected = gSavedState.selected;
    WriteSavedPassword_5335B0(password, selected);
    WriteSavedAccount_533640(account, selected);
    std::strcpy(GetCurrentSavedInputValueFF494F(), password);
    std::strcpy(GetCurrentSavedInputValueFF4982(), account);
    gAutoLoginCachedPhysicalSlot = gSavedState.logicalToPhysical[selected];
}

char* GetCurrentSavedInputValueFF494F()
{
    return gCurrentSavedRecord.parts[0].text + 0x33;
}

char* GetCurrentSavedInputValueFF4982()
{
    return gCurrentSavedRecord.parts[0].text + 0x66;
}

char* GetCurrentSavedStallNameFF4D18()
{
    return reinterpret_cast<char*>(&gCurrentSavedRecord) + 0x400;
}

char* GetCurrentSavedStallDescriptionFF4D28()
{
    return reinterpret_cast<char*>(&gCurrentSavedRecord) + 0x410;
}

std::int32_t& gSelectedSavedSlot105C394 = gSavedState.selected; // 原全局：0x0105C394。
std::int32_t& gVisibleSavedSlotCount105C364 = gSavedState.visibleCount; // 原全局：0x0105C364。

// 原函数：0x00533570。
void Sub_533570(
    const char* const source,
    const std::int32_t logicalSlot)
{
    if (source == nullptr) {
        return;
    }

    if (std::strlen(source) >= 51) {
        throw cg::text::GbkStorageError(122, std::system_category(), "文字超过登录GBK字段容量");
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

// 原函数：0x005335B0。
void WriteSavedPassword_5335B0(
    const char* const source,
    const std::int32_t logicalSlot)
{
    if (source == nullptr) {
        return;
    }

    if (std::strlen(source) >= 51) {
        throw cg::text::GbkStorageError(122, std::system_category(), "文字超过登录GBK字段容量");
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

// 原函数：0x00533640。
void WriteSavedAccount_533640(
    const char* const source,
    const std::int32_t logicalSlot)
{
    if (source == nullptr) {
        return;
    }

    if (std::strlen(source) >= 51) {
        throw cg::text::GbkStorageError(122, std::system_category(), "文字超过登录GBK字段容量");
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

// 原函数：0x005335F0。
char* ReadSavedPassword_5335F0(
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

// 原函数：0x00533680。
char* ReadSavedAccount_533680(
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

// 原函数：0x00533730。
void SaveCredentialRecordsWithoutPasswords_533730()
{
    SavedPhysicalRecord localRecord;
    std::uint8_t encoded[0xA20];
    std::int32_t encodedBytes;
    char path[0x400];

    for (std::int32_t physicalSlot = 0;
         physicalSlot < 10;
         ++physicalSlot) {
        if (gSavedState.active[physicalSlot] != 1) {
            continue;
        }

        const auto* const recordBytes =
            reinterpret_cast<const std::uint8_t*>(
                &gSavedState.records[physicalSlot]);
        if (recordBytes[0x6A] == 0) {
            continue;
        }
        if (recordBytes[0x37] == 0) {
            continue;
        }

        std::memcpy(
            &localRecord,
            &gSavedState.records[physicalSlot],
            sizeof(localRecord));
        auto* const localBytes =
            reinterpret_cast<std::uint8_t*>(&localRecord);
        std::memset(localBytes + 0x37, 0, 0x33);
        localBytes[0x37] = 0;

        cg::login::detail::EncodeLegacyBytes_52D310(
            localBytes,
            0x510,
            0,
            encoded,
            &encodedBytes,
            0xA20);
        std::sprintf(
            path,
            "%s\\%s",
            gSavedState.paths[physicalSlot].text,
            "save.dat");

        std::FILE* const file =
            std::fopen(path, "wb+");
        if (file == nullptr) {
            return;
        }

        const std::uint32_t bytesWritten = static_cast<std::uint32_t>(std::fwrite(
            encoded,
            1,
            static_cast<std::uint32_t>(encodedBytes),
            file));
        const std::uint32_t expectedBytes =
            static_cast<std::uint32_t>(encodedBytes);
        if (bytesWritten < expectedBytes) {
            std::fclose(file);
            return;
        }
        std::fclose(file);
    }
}

} // namespace cg::login::detail
