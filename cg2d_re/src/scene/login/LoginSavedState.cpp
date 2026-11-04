#include "cg/session/ClientSession.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "LoginSavedState.hpp"
#include "../character_select/CharacterSelectSceneDependencies.hpp"
#include "LoginInputDependencies.hpp"
#include "LoginSceneDependencies.hpp"
#include "../../ui/WindowRuntimeState.hpp"

#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/Startup.hpp"

#include <bit>
#include <algorithm>
#include <format>
#include <string>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include <Windows.h>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BCCD、0x0053C680；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

#include "cg/core/m_log.hpp"

namespace cg::login::detail {

struct SavedPath final {
    std::wstring text; // 原成员偏移：+0x00。
};

// 旧save.dat的0x510字节磁盘格式。未解释区域按原字节保留。
struct SavedDiskRecord final {
    std::int32_t version;
    std::array<std::array<char, 51>, 3> inputs04;
    std::uint8_t stereo9D;
    std::uint8_t effect9E;
    std::uint8_t music9F;
    std::array<std::uint8_t, 16> frequenciesA0;
    std::uint8_t cursorB0;
    std::uint8_t chatSizeB1;
    std::array<std::array<char, 25>, 24> presetsB2;
    std::array<std::uint8_t, 8> display30A;
    std::uint8_t split312;
    std::uint8_t sort313;
    std::uint8_t overhead314;
    std::uint8_t value315;
    std::array<std::uint16_t, 2> bgm316;
    std::uint8_t scanline31A;
    std::uint8_t value31B;
    std::array<std::uint8_t, 0xE4> reserved31C;
    std::array<char, 16> stallName400;
    std::array<char, 113> stallDescription410;
    std::array<std::uint8_t, 0x8F> reserved481;
};
static_assert(sizeof(SavedDiskRecord) == 0x510);
static_assert(offsetof(SavedDiskRecord, inputs04) == 4);
static_assert(offsetof(SavedDiskRecord, frequenciesA0) == 0xA0);
static_assert(offsetof(SavedDiskRecord, presetsB2) == 0xB2);
static_assert(offsetof(SavedDiskRecord, display30A) == 0x30A);
static_assert(offsetof(SavedDiskRecord, bgm316) == 0x316);
static_assert(offsetof(SavedDiskRecord, stallName400) == 0x400);
static_assert(offsetof(SavedDiskRecord, stallDescription410) == 0x410);
static_assert(std::is_trivially_copyable_v<SavedDiskRecord>);

struct SavedPhysicalRecord final {
    std::int32_t version = 0;
    std::array<std::wstring, 3> inputs04;
    std::uint8_t stereo9D = 0;
    std::uint8_t effect9E = 0;
    std::uint8_t music9F = 0;
    std::array<std::uint8_t, 16> frequenciesA0{};
    std::uint8_t cursorB0 = 0;
    std::uint8_t chatSizeB1 = 0;
    std::array<std::wstring, 24> presetsB2;
    std::array<std::uint8_t, 8> display30A{};
    std::uint8_t split312 = 0;
    std::uint8_t sort313 = 0;
    std::uint8_t overhead314 = 0;
    std::uint8_t value315 = 0;
    std::array<std::uint16_t, 2> bgm316{};
    std::uint8_t scanline31A = 0;
    std::uint8_t value31B = 0;
    std::array<std::uint8_t, 0xE4> reserved31C{};
    std::wstring stallName400;
    std::wstring stallDescription410;
    std::array<std::uint8_t, 0x8F> reserved481{};
};

template<std::size_t N>
static std::wstring DecodeSavedText(const std::array<char, N>& bytes)
{
    const auto end = std::find(bytes.begin(), bytes.end(), '\0');
    return cg::text::FromGbkWide(std::string_view(bytes.data(), static_cast<std::size_t>(end - bytes.begin())));
}

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

static SavedDiskRecord EncodeSavedRecord(const SavedPhysicalRecord& record, const bool savePassword = true)
{
    SavedDiskRecord disk{};
    disk.version = record.version;
    disk.stereo9D = record.stereo9D;
    disk.effect9E = record.effect9E;
    disk.music9F = record.music9F;
    disk.frequenciesA0 = record.frequenciesA0;
    disk.cursorB0 = record.cursorB0;
    disk.chatSizeB1 = record.chatSizeB1;
    disk.display30A = record.display30A;
    disk.split312 = record.split312;
    disk.sort313 = record.sort313;
    disk.overhead314 = record.overhead314;
    disk.value315 = record.value315;
    disk.bgm316 = record.bgm316;
    disk.scanline31A = record.scanline31A;
    disk.value31B = record.value31B;
    disk.reserved31C = record.reserved31C;
    disk.reserved481 = record.reserved481;
    for (std::size_t i = 0; i < disk.inputs04.size(); ++i) {
        if (i != 1 || savePassword) { cg::text::StoreGbk(disk.inputs04[i].data(), disk.inputs04[i].size(), record.inputs04[i]); }
    }
    for (std::size_t i = 0; i < disk.presetsB2.size(); ++i) {
        cg::text::StoreGbk(disk.presetsB2[i].data(), disk.presetsB2[i].size(), record.presetsB2[i]);
    }
    const std::string name = cg::text::ToGbk(record.stallName400);
    std::size_t length = 0;
    while (length < name.size()) {
        const auto lead = static_cast<unsigned char>(name[length]);
        const std::size_t next = length + (lead >= 0x81 && lead <= 0xFE ? 2 : 1);
        if (next >= disk.stallName400.size() || next > name.size()) { break; }
        length = next;
    }
    std::copy_n(name.data(), length, disk.stallName400.data());
    cg::text::StoreGbk(disk.stallDescription410.data(), disk.stallDescription410.size(), record.stallDescription410);
    return disk;
}

static void DecodeSavedRecord(SavedPhysicalRecord& record, const SavedDiskRecord& disk, const std::size_t bytes)
{
    if (bytes > offsetof(SavedDiskRecord, version)) { record.version = disk.version; }
    if (bytes > offsetof(SavedDiskRecord, stereo9D)) { record.stereo9D = disk.stereo9D; }
    if (bytes > offsetof(SavedDiskRecord, effect9E)) { record.effect9E = disk.effect9E; }
    if (bytes > offsetof(SavedDiskRecord, music9F)) { record.music9F = disk.music9F; }
    if (bytes > offsetof(SavedDiskRecord, frequenciesA0)) { record.frequenciesA0 = disk.frequenciesA0; }
    if (bytes > offsetof(SavedDiskRecord, cursorB0)) { record.cursorB0 = disk.cursorB0; }
    if (bytes > offsetof(SavedDiskRecord, chatSizeB1)) { record.chatSizeB1 = disk.chatSizeB1; }
    if (bytes > offsetof(SavedDiskRecord, display30A)) { record.display30A = disk.display30A; }
    if (bytes > offsetof(SavedDiskRecord, split312)) { record.split312 = disk.split312; }
    if (bytes > offsetof(SavedDiskRecord, sort313)) { record.sort313 = disk.sort313; }
    if (bytes > offsetof(SavedDiskRecord, overhead314)) { record.overhead314 = disk.overhead314; }
    if (bytes > offsetof(SavedDiskRecord, value315)) { record.value315 = disk.value315; }
    if (bytes > offsetof(SavedDiskRecord, bgm316)) { record.bgm316 = disk.bgm316; }
    if (bytes > offsetof(SavedDiskRecord, scanline31A)) { record.scanline31A = disk.scanline31A; }
    if (bytes > offsetof(SavedDiskRecord, value31B)) { record.value31B = disk.value31B; }
    if (bytes > offsetof(SavedDiskRecord, reserved31C)) { record.reserved31C = disk.reserved31C; }
    if (bytes > offsetof(SavedDiskRecord, reserved481)) { record.reserved481 = disk.reserved481; }
    for (std::size_t i = 0; i < disk.inputs04.size(); ++i) {
        if (bytes > offsetof(SavedDiskRecord, inputs04) + i * 51) { record.inputs04[i] = DecodeSavedText(disk.inputs04[i]); }
    }
    for (std::size_t i = 0; i < disk.presetsB2.size(); ++i) {
        if (bytes > offsetof(SavedDiskRecord, presetsB2) + i * 25) { record.presetsB2[i] = DecodeSavedText(disk.presetsB2[i]); }
    }
    if (bytes > offsetof(SavedDiskRecord, stallName400)) { record.stallName400 = DecodeSavedText(disk.stallName400); }
    if (bytes > offsetof(SavedDiskRecord, stallDescription410)) { record.stallDescription410 = DecodeSavedText(disk.stallDescription410); }
}

// 默认值只建立内部对象；文件编码留在写出边界。
static SavedPhysicalRecord MakeDefaultSavedRecord()
{
    SavedPhysicalRecord record{};
    record.version = 0x7002;
    record.effect9E = static_cast<std::uint8_t>(cg::platform::gEffectVolume);
    record.stereo9D = static_cast<std::uint8_t>(cg::platform::gUseStereo);
    record.music9F = static_cast<std::uint8_t>(cg::platform::gMusicVolume);
    std::copy_n(cg::platform::gBgmFrequencySelectionPrefix, record.frequenciesA0.size(), record.frequenciesA0.begin());
    record.scanline31A = static_cast<std::uint8_t>(cg::platform::gScanlineSyncMode);
    for (std::size_t i = 0; i < 2; ++i) {
        record.display30A[2 + i] = 8;
        record.display30A[4 + i] = 5;
        record.display30A[6 + i] = 1;
    }
    return record;
}

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
    cg::platform::gSavedStateLoadErrorCode_FF4E28 = 0;

    std::wstring path;
    std::uint8_t encoded[0xA20];
    std::int32_t decodedBytes;
    path = std::format(L"{}\\save.dat", gSavedState.paths[physicalSlot].text);

    std::FILE* const file =
        _wfopen(path.c_str(), L"rb+");
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
    if (bytesRead <= 1 || bytesRead > sizeof(SavedDiskRecord) + 1) { return 0; }
    auto disk = EncodeSavedRecord(gSavedState.records[physicalSlot]);
    (void)cg::login::detail::DecodeLegacyBytes_52D460(
        encoded,
        static_cast<std::int32_t>(bytesRead),
        0,
        reinterpret_cast<std::uint8_t*>(&disk),
        &decodedBytes);
    DecodeSavedRecord(gSavedState.records[physicalSlot], disk, static_cast<std::size_t>(decodedBytes));
    if (decodedBytes != 0x510) {
        return 0;
    }
    return gSavedState.records[physicalSlot].version == 0x7002
        ? 1
        : 0;
}

// 原 sub_532FC0@[0x532FC0,0x53319A)：写出一份默认保存记录。
static std::int32_t WriteDefaultSavedFile_532FC0(
    const wchar_t* const fileName)
{
    auto raw = EncodeSavedRecord(MakeDefaultSavedRecord());
    auto* const rawBytes = reinterpret_cast<std::uint8_t*>(&raw);

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
        _wfopen(fileName, L"wb+");
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
static void InitializeSavedPhysicalRecord_5331A0(const std::int32_t physicalSlot)
{
    gSavedState.records[physicalSlot] = MakeDefaultSavedRecord();
}

// 原 sub_533360@[0x533360,0x5333CF)：复位保存状态。
static std::int32_t ResetSavedState_533360()
{
    gSavedState.newPhysical = -1;
    gSavedState.visibleCount = 0;
    gSavedState.selected = 0;
    gSavedState.mappingCount = 0;

    for (std::int32_t index = 0; index < 10; ++index) {
        gSavedState.paths[index].text = std::format(L"data\\{}", index);
        gSavedState.active[index] = 0;
        gSavedState.logicalToPhysical[index] = -1;
        gSavedState.records[0] = {};
    }
    return 0;
}

// 原 sub_5333D0@[0x5333D0,0x533461)：探测当前与旧版保存位置。
static std::int32_t ProbeSavedStateLocation_5333D0()
{
    std::wstring path;
    for (std::int32_t index = 0; index < 10; ++index) {
        path = std::format(L"{}\\save.dat", gSavedState.paths[index].text);
        std::FILE* const file =
            _wfopen(path.c_str(), L"rb+");
        if (file != nullptr) {
            std::fclose(file);
            return 0;
        }
    }

    std::FILE* const legacyFile =
        _wfopen(L"data\\save.dat", L"rb+");
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
    (void)::CopyFileW(
        L"data\\save.dat",
        L"data\\0\\save.dat",
        FALSE);
    (void)::CopyFileW(
        L"data\\mail.dat",
        L"data\\0\\mail.dat",
        FALSE);
    return static_cast<std::int32_t>(::CopyFileW(
        L"data\\guildmail.dat",
        L"data\\0\\guildmail.dat",
        FALSE));
}

// 原 sub_5336D0@[0x5336D0,0x5336F5)：载入当前选中物理记录。
static std::int32_t LoadSelectedPhysicalRecord_5336D0()
{
    const std::int32_t selected = gSavedState.selected;
    const std::uint32_t resultBits =
        std::bit_cast<std::uint32_t>(selected) * 9U;
    if (selected < 0 || selected >= 10) { return -1; }
    gCurrentSavedRecord = gSavedState.records[selected];
    return std::bit_cast<std::int32_t>(resultBits);
}

// 原 sub_533700@[0x533700,0x533725)：把当前记录写回选中物理记录。
static std::int32_t StoreCurrentRecordToSelectedPhysical_533700()
{
    const std::int32_t selected = gSavedState.selected;
    const std::uint32_t resultBits =
        std::bit_cast<std::uint32_t>(selected) * 9U;
    if (selected < 0 || selected >= 10) { return -1; }
    gSavedState.records[selected] = gCurrentSavedRecord;
    return std::bit_cast<std::int32_t>(resultBits);
}

// 原 sub_520200@[0x520200,0x5202DE)：持久化当前保存记录。
static std::int32_t PersistCurrentSavedRecord_520200()
{
    auto localRecord = EncodeSavedRecord(gCurrentSavedRecord, false);
    std::wstring path;
    std::uint8_t encoded[0xA20];
    std::int32_t encodedBytes;


    auto* const localBytes =
        reinterpret_cast<std::uint8_t*>(&localRecord);
    cg::login::detail::EncodeLegacyBytes_52D310(
        localBytes,
        0x510,
        0,
        encoded,
        &encodedBytes,
        0xA20);

    const std::int32_t selected = gSavedState.selected;
    if (selected < 0 || selected >= 10) { return 0; }
    const std::int32_t physicalSlot =
        gSavedState.logicalToPhysical[selected];
    if (physicalSlot < 0 || physicalSlot >= 10) { return 0; }
    path = std::format(L"{}\\save.dat", gSavedState.paths[physicalSlot].text);

    std::FILE* const file =
        _wfopen(path.c_str(), L"wb+");
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
    if (physicalSlot < 0 || physicalSlot >= 10) { return 0; }
    if (gAutoLoginCachedPhysicalSlot == physicalSlot) {
        gAutoLoginCachedPhysicalSlot = -1;
    }
    std::wstring path;

    path = std::format(L"{}\\save.dat", gSavedState.paths[physicalSlot].text);
    (void)::DeleteFileW(path.c_str());

    path = std::format(L"{}\\mail.dat", gSavedState.paths[physicalSlot].text);
    (void)::DeleteFileW(path.c_str());

    path = std::format(L"{}\\guildmail.dat", gSavedState.paths[physicalSlot].text);
    return static_cast<std::int32_t>(::DeleteFileW(path.c_str()));
}

// 存档无参调用适配；原4A4210不读取参数，共用窗口侧真实实现。
static std::int32_t ReturnOne_4A4210() noexcept
{
    return cg::ui::detail::WindowRuntime_ReturnOne(0);
}

// 原 sub_5204C0@[0x5204C0,0x520502)：从记录恢复音频设置。
static std::int32_t RestoreAudioSettings_5204C0(
    const SavedPhysicalRecord& current)
{
    cg::platform::gUseStereo =
        static_cast<std::int32_t>(current.stereo9D);
    cg::platform::gEffectVolume =
        static_cast<std::int32_t>(current.effect9E);
    cg::platform::gMusicVolume =
        static_cast<std::int32_t>(current.music9F);

    std::int32_t index = 0;
    for (; index < 16; ++index) {
        const std::uint8_t value = current.frequenciesA0[index];
        cg::platform::gBgmFrequencySelectionPrefix[index] = value;
    }
    return index;
}

// 原 sub_5205E0@[0x5205E0,0x52063B)：从记录恢复六项标量设置。
static void RestoreSavedScalarSettings_5205E0(
    const SavedPhysicalRecord& current)
{
    cg::platform::gNativeCursorEnabled_D67AE4 =
        static_cast<std::uint32_t>(current.cursorB0);
    cg::platform::gChatTextSizeMode_73ECBC =
        static_cast<std::int32_t>(current.chatSizeB1);
    cg::platform::gInvertItemSplitModifier_D67AE8 =
        static_cast<std::uint32_t>(current.split312);
    cg::platform::gActorOverheadTextMode_5F974C =
        static_cast<std::int16_t>(current.overhead314);
    cg::platform::gScanlineSyncMode =
        static_cast<std::uint16_t>(current.scanline31A);
    cg::platform::gValueD68858 =
        static_cast<std::uint32_t>(current.value31B);
}

// 原 sub_520690@[0x520690,0x5206EE)：恢复 24 项保存输入文本。
static std::int32_t RestoreSavedInputContexts_520690(session::ClientSession& session, const SavedPhysicalRecord& current)
{
    auto& inputs = session.UiFrame().native.inputs;
    for (std::size_t i = 0; i < current.presetsB2.size(); ++i) {
        auto& context = inputs.hotkeys[i];
        context.setText(current.presetsB2[i]);
        context.totalByteLength = static_cast<std::int32_t>(context.text.size());
        context.caretByteOffset = context.totalByteLength;
    }
    return 0;
}

// 原 sub_520700@[0x520700,0x520712)：从记录恢复单字节状态。
static std::uint32_t RestoreSavedByteState_520700(
    const SavedPhysicalRecord& current)
{
    const std::uint32_t value =
        static_cast<std::uint32_t>(current.sort313);
    cg::platform::gContactSortMode_D67CD0 = value;
    return value;
}

// 原 sub_520730@[0x520730,0x520744)：从当前记录恢复条件式单字节值。
static std::uint8_t RestoreCurrentSavedValue_520730()
{
    auto& current = gCurrentSavedRecord;
    const std::uint8_t value = current.value315;
    if (value != 0) {
        cg::platform::gValue553438 = static_cast<std::int32_t>(value);
    }
    return value;
}

// 原函数：0x005201B0。
std::int32_t RestoreCurrentSavedState_5201B0(session::ClientSession& session)
{
    if (ReturnOne_4A4210() == 0) {
        return 0;
    }

    auto& current = gCurrentSavedRecord;
    (void)RestoreAudioSettings_5204C0(current);
    (void)RestoreSavedScalarSettings_5205E0(current);
    (void)RestoreSavedInputContexts_520690(session, current);
    (void)RestoreSavedByteState_520700(current);
    (void)RestoreCurrentSavedValue_520730();
    return 1;
}

// 原函数：0x00533300。
std::int32_t InitializeSavedState_533300()
{
    // 返回入口会重读磁盘；仅保留自动登录槽的两个内存字段，不将密码写入存档。
    const std::int32_t cachedPhysicalSlot = gAutoLoginCachedPhysicalSlot;
    std::array<std::wstring, 2> cachedCredentials;
    if (cachedPhysicalSlot != -1) {
        cachedCredentials[0] = gSavedState.records[cachedPhysicalSlot].inputs04[1];
        cachedCredentials[1] = gSavedState.records[cachedPhysicalSlot].inputs04[2];
    }
    (void)ResetSavedState_533360();
    const std::int32_t location = ProbeSavedStateLocation_5333D0();
    if (location == 0) {
        (void)RebuildSavedSlotMap_533470();
    } else if (location == 1) {
        (void)MigrateLegacySavedFiles_533530();
        (void)RebuildSavedSlotMap_533470();
    } else if (location == 2) {
        (void)WriteDefaultSavedFile_532FC0(L"data\\0\\save.dat");
        (void)RebuildSavedSlotMap_533470();
    }

    if (cachedPhysicalSlot != -1) {
        gSavedState.records[cachedPhysicalSlot].inputs04[1] = cachedCredentials[0];
        gSavedState.records[cachedPhysicalSlot].inputs04[2] = cachedCredentials[1];
    }
    const std::int32_t physicalSlot =
        gSavedState.logicalToPhysical[0];
    if (physicalSlot < 0 || physicalSlot >= 10) { return 0; }
    gCurrentSavedRecord = gSavedState.records[physicalSlot];
    return 1;
}

// 原 [0x520510,0x520545)：四项显示状态交错读写当前保存记录。
std::int32_t CaptureCharacterDisplaySettings_520510(session::ClientSession& session, const std::int32_t selected)
{
    auto& chatDisplay = session.WorldScene().chatDisplay;
    auto& inputs = session.UiFrame().native.inputs;
    if (selected < 0 || selected >= 2) { return -1; }
    auto& current = gCurrentSavedRecord;
    const volatile std::int32_t& color =
        inputs.chat.color;
    const volatile std::int32_t& first = cg::platform::gValue55463C;
    const volatile std::int32_t& second = chatDisplay.range;
    const volatile std::int32_t& third = cg::platform::gChatNotificationSoundEnabled_554644;
    const std::uint8_t colorByte = static_cast<std::uint8_t>(color);
    const std::uint8_t firstByte = static_cast<std::uint8_t>(first);
    current.display30A[0 + selected] = colorByte;
    const std::uint8_t secondByte = static_cast<std::uint8_t>(second);
    current.display30A[2 + selected] = firstByte;
    const std::uint8_t thirdByte = static_cast<std::uint8_t>(third);
    current.display30A[4 + selected] = secondByte;
    current.display30A[6 + selected] = thirdByte;
    return selected;
}

// 原 [0x520550,0x52058D)：按两组读取、零扩展并发布四项显示状态。
std::int32_t RestoreCharacterDisplaySettings_520550(session::ClientSession& session, const std::int32_t selected)
{
    auto& chatDisplay = session.WorldScene().chatDisplay;
    auto& inputs = session.UiFrame().native.inputs;
    if (selected < 0 || selected >= 2) { return -1; }
    auto& current = gCurrentSavedRecord;
    const std::int32_t color = current.display30A[0 + selected];
    const std::int32_t first = current.display30A[2 + selected];
    inputs.chat.color = color;
    cg::platform::gValue55463C = first;
    const std::int32_t second = current.display30A[4 + selected];
    const std::int32_t third = current.display30A[6 + selected];
    chatDisplay.range = second;
    cg::platform::gChatNotificationSoundEnabled_554644 = third;
    return selected;
}

// 原 [0x520750,0x520764)：只存 BGM 目标低 W16，返回原索引。
std::int32_t CaptureCharacterBgmTarget_520750(session::ClientSession& session, const std::int32_t selected)
{
    if (selected < 0 || selected >= 2) { return -1; }
    auto& current = gCurrentSavedRecord;
    const std::uint16_t target =
        static_cast<std::uint16_t>(session.Audio().bgmTarget);
    current.bgm316[selected] = target;
    return selected;
}

// 原 [0x520770,0x520784)：从当前记录的 W16 零扩展恢复同一 BGM 目标。
std::int32_t RestoreCharacterBgmTarget_520770(session::ClientSession& session, const std::int32_t selected)
{
    if (selected < 0 || selected >= 2) { return -1; }
    auto& current = gCurrentSavedRecord;
    std::uint16_t target;
    target = current.bgm316[selected];
    session.Audio().bgmTarget = target;
    return target;
}

// 原 sub_520470@[0x520470,0x5204BF)：将音频设置写入当前保存记录。
std::int32_t CaptureAudioPreferences_520470()
{
    auto& current = gCurrentSavedRecord;
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

    current.stereo9D = stereo;
    std::memcpy(&first, frequencies, sizeof(first));
    current.effect9E = effect;
    std::memcpy(&second, frequencies + 4, sizeof(second));
    current.music9F = music;
    std::memcpy(&third, frequencies + 8, sizeof(third));
    std::memcpy(current.frequenciesA0.data() + 0, &first, sizeof(first));
    std::memcpy(&fourth, frequencies + 12, sizeof(fourth));
    std::memcpy(current.frequenciesA0.data() + 4, &second, sizeof(second));
    std::memcpy(current.frequenciesA0.data() + 8, &third, sizeof(third));
    std::memcpy(current.frequenciesA0.data() + 12, &fourth, sizeof(fourth));
    return std::bit_cast<std::int32_t>(fourth);
}

// 原 sub_520590@[0x520590,0x5205D5)：将六项标量低字节写入当前记录。
std::uint8_t CaptureClientPreferenceBytes_520590()
{
    auto& current = gCurrentSavedRecord;
    const std::uint8_t show =
        static_cast<std::uint8_t>(cg::platform::gNativeCursorEnabled_D67AE4);
    const std::uint8_t value73ECBC =
        static_cast<std::uint8_t>(cg::platform::gChatTextSizeMode_73ECBC);
    const std::uint8_t valueD67AE8 =
        static_cast<std::uint8_t>(cg::platform::gInvertItemSplitModifier_D67AE8);
    current.cursorB0 = show;
    const std::uint8_t value5F974C =
        static_cast<std::uint8_t>(cg::platform::gActorOverheadTextMode_5F974C);
    current.chatSizeB1 = value73ECBC;
    const std::uint8_t scanlineSyncMode =
        static_cast<std::uint8_t>(cg::platform::gScanlineSyncMode);
    current.split312 = valueD67AE8;
    const std::uint8_t valueD68858 =
        static_cast<std::uint8_t>(cg::platform::gValueD68858);
    current.overhead314 = value5F974C;
    current.scanline31A = scanlineSyncMode;
    current.value31B = valueD68858;
    return value5F974C;
}

// 原 sub_520640@[0x520640,0x520681)：保存 24 个输入上下文的零结尾文本。
std::uint32_t CapturePresetChatTexts_520640(session::ClientSession& session)
{
    auto& inputs = session.UiFrame().native.inputs;
    std::array<std::wstring, 24> texts;
    std::uint32_t result = 0;
    for (std::size_t i = 0; i < texts.size(); ++i) {
        texts[i] = cg::input::ContextText(inputs.hotkeys[i]);
        const auto bytes = cg::text::ToGbk(texts[i]).size();
        if (bytes >= 0x19) {
            throw cg::text::GbkStorageError(122, std::system_category(), "快捷文字超过原存档字段容量");
        }
        result = static_cast<std::uint32_t>(bytes + 1);
    }
    gCurrentSavedRecord.presetsB2 = std::move(texts);
    return result;
}

// 原 sub_5206F0@[0x5206F0,0x5206FB)：保存单字节状态并返回 AL。
std::uint8_t CaptureContactSortMode_5206F0()
{
    const std::uint8_t value =
        static_cast<std::uint8_t>(cg::platform::gContactSortMode_D67CD0);
    auto& current = gCurrentSavedRecord;
    current.sort313 = value;
    return value;
}

// 原 sub_520720@[0x520720,0x52072B)：保存 +0x315 的低字节并返回 AL。
std::uint8_t Sub_520720()
{
    const std::uint8_t value =
        static_cast<std::uint8_t>(cg::platform::gValue553438);
    auto& current = gCurrentSavedRecord;
    current.value315 = value;
    return value;
}

// 原 [0x520160,0x5201A8)：各保存叶按序执行，末尾透传持久化返回。
std::int32_t SaveCurrentClientPreferences_520160(network::CgConnection& connection)
try {
    const volatile std::int16_t& selectedCharacter =
        connection.LoginRequests().selectedCharacterIndex;
    static_cast<void>(CaptureAudioPreferences_520470());
    static_cast<void>(CaptureClientPreferenceBytes_520590());

    const std::int32_t ignoredSelected = selectedCharacter;
    static_cast<void>(ignoredSelected);
    cg::core::NullSub_509080();

    const std::int32_t selectedForDisplay = selectedCharacter;
    static_cast<void>(CaptureCharacterDisplaySettings_520510(connection.Owner(), selectedForDisplay));
    static_cast<void>(CapturePresetChatTexts_520640(connection.Owner()));
    static_cast<void>(CaptureContactSortMode_5206F0());

    const std::int32_t selectedForAudio = selectedCharacter;
    static_cast<void>(CaptureCharacterBgmTarget_520750(connection.Owner(), selectedForAudio));
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
    if (index < 0 || index >= 10) { return -1; }
    return gSavedState.logicalToPhysical[index];
}

const wchar_t* ReadSelectedSavedDirectory()
{
    const std::int32_t selected = gSavedState.selected;
    if (selected < 0 || selected >= 10) { return L""; }
    const std::int32_t physicalSlot = gSavedState.logicalToPhysical[selected];
    if (physicalSlot < 0 || physicalSlot >= 10) { return L""; }
    return gSavedState.paths[physicalSlot].text.c_str();
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

void RestoreCapability4CurrentSavedRecord(session::ClientSession& session)
{
    auto& current = gCurrentSavedRecord;
    (void)RestoreAudioSettings_5204C0(current);
    (void)RestoreSavedScalarSettings_5205E0(current);
    (void)RestoreSavedInputContexts_520690(session, current);
    (void)RestoreSavedByteState_520700(current);
}

void CacheAutoLoginInputValues(const std::wstring_view account, const std::wstring_view password)
{
    const std::int32_t selected = gSavedState.selected;
    if (selected < 0 || selected >= 10) { return; }
    WriteSavedPassword_5335B0(password, selected);
    WriteSavedAccount_533640(account, selected);
    GetCurrentSavedInputValueFF494F() = password;
    GetCurrentSavedInputValueFF4982() = account;
    gAutoLoginCachedPhysicalSlot = gSavedState.logicalToPhysical[selected];
}

std::wstring& GetCurrentSavedInputValueFF494F()
{
    return gCurrentSavedRecord.inputs04[1];
}

std::wstring& GetCurrentSavedInputValueFF4982()
{
    return gCurrentSavedRecord.inputs04[2];
}

std::wstring& GetCurrentSavedStallNameFF4D18()
{
    return gCurrentSavedRecord.stallName400;
}

std::wstring& GetCurrentSavedStallDescriptionFF4D28()
{
    return gCurrentSavedRecord.stallDescription410;
}

std::int32_t& gSelectedSavedSlot105C394 = gSavedState.selected; // 原全局：0x0105C394。
std::int32_t& gVisibleSavedSlotCount105C364 = gSavedState.visibleCount; // 原全局：0x0105C364。

// 原函数：0x00533570。
void Sub_533570(const std::wstring_view source, const std::int32_t logicalSlot)
{
    if (logicalSlot < 0 || logicalSlot >= 10) { return; }
    const std::int32_t physicalSlot = gSavedState.logicalToPhysical[logicalSlot];
    if (physicalSlot < 0 || physicalSlot >= 10) { return; }
    if (cg::text::ToGbk(source).size() >= 51) {
        throw cg::text::GbkStorageError(122, std::system_category(), "文字超过登录GBK字段容量");
    }
    gSavedState.records[physicalSlot].inputs04[0] = source;
}

// 原函数：0x005335B0。
void WriteSavedPassword_5335B0(const std::wstring_view source, const std::int32_t logicalSlot)
{
    if (logicalSlot < 0 || logicalSlot >= 10) { return; }
    const std::int32_t physicalSlot = gSavedState.logicalToPhysical[logicalSlot];
    if (physicalSlot < 0 || physicalSlot >= 10) { return; }
    if (cg::text::ToGbk(source).size() >= 51) {
        throw cg::text::GbkStorageError(122, std::system_category(), "文字超过登录GBK字段容量");
    }
    gSavedState.records[physicalSlot].inputs04[1] = source;
}

// 原函数：0x00533640。
void WriteSavedAccount_533640(const std::wstring_view source, const std::int32_t logicalSlot)
{
    if (logicalSlot < 0 || logicalSlot >= 10) { return; }
    const std::int32_t physicalSlot = gSavedState.logicalToPhysical[logicalSlot];
    if (physicalSlot < 0 || physicalSlot >= 10) { return; }
    if (cg::text::ToGbk(source).size() >= 51) {
        throw cg::text::GbkStorageError(122, std::system_category(), "文字超过登录GBK字段容量");
    }
    gSavedState.records[physicalSlot].inputs04[2] = source;
}

// 原函数：0x005335F0。
std::wstring& ReadSavedPassword_5335F0(std::wstring& output, const std::int32_t logicalSlot)
{
    output.clear();
    if (logicalSlot < 0 || logicalSlot >= 10) { return output; }
    const std::int32_t physicalSlot = gSavedState.logicalToPhysical[logicalSlot];
    if (physicalSlot < 0 || physicalSlot >= 10) { return output; }
    output = gSavedState.records[physicalSlot].inputs04[1];
    return output;
}

// 原函数：0x00533680。
std::wstring& ReadSavedAccount_533680(std::wstring& output, const std::int32_t logicalSlot)
{
    output.clear();
    if (logicalSlot < 0 || logicalSlot >= 10) { return output; }
    const std::int32_t physicalSlot = gSavedState.logicalToPhysical[logicalSlot];
    if (physicalSlot < 0 || physicalSlot >= 10) { return output; }
    output = gSavedState.records[physicalSlot].inputs04[2];
    return output;
}

// 原函数：0x00533730。
void SaveCredentialRecordsWithoutPasswords_533730()
{
    SavedDiskRecord localRecord;
    std::uint8_t encoded[0xA20];
    std::int32_t encodedBytes;
    std::wstring path;

    for (std::int32_t physicalSlot = 0;
         physicalSlot < 10;
         ++physicalSlot) {
        if (gSavedState.active[physicalSlot] != 1) {
            continue;
        }

        const auto& record = gSavedState.records[physicalSlot];
        if (record.inputs04[2].empty() || record.inputs04[1].empty()) { continue; }
        localRecord = EncodeSavedRecord(record, false);
        auto* const localBytes = reinterpret_cast<std::uint8_t*>(&localRecord);

        cg::login::detail::EncodeLegacyBytes_52D310(
            localBytes,
            0x510,
            0,
            encoded,
            &encodedBytes,
            0xA20);
        path = std::format(L"{}\\save.dat", gSavedState.paths[physicalSlot].text);

        std::FILE* const file =
            _wfopen(path.c_str(), L"wb+");
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
