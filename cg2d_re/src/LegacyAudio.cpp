#include "cg/LegacyAudio.hpp"

#include "LoginStateDependencies.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/world/SceneState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <mmsystem.h>

namespace cg::login::detail {

std::int32_t Sub_520750(std::int32_t selected);
std::int32_t Sub_520200();
std::int32_t LoadAndPlayBGM(std::uint32_t bgmId);

} // namespace cg::login::detail

namespace {

struct LegacyAudioRecord final {
    std::int32_t value00;
    std::array<std::byte, 0x48> opaque;
};

static_assert(offsetof(LegacyAudioRecord, value00) == 0x00);
static_assert(sizeof(LegacyAudioRecord) == 0x4C);

constexpr std::uint32_t kRecordCount = 128;

std::int32_t gRecordIndex = -1;
std::array<LegacyAudioRecord, kRecordCount> gRecords {};
std::int32_t gFlag = 0;
std::int32_t gQuotient = 0;

static_assert(sizeof(gRecords) == 0x2600);

} // namespace

namespace cg::audio {

// 原 0x103D1C0 的 50 项记录；52CD40 只写 +0x04/+0x00，后三个 S32 保留。
struct SpatialAudioRecord final {
    std::uint32_t value00;
    std::int32_t value04;
    std::int32_t value08;
    std::int32_t value0C;
    std::int32_t value10;
};

static_assert(offsetof(SpatialAudioRecord, value00) == 0x00);
static_assert(offsetof(SpatialAudioRecord, value04) == 0x04);
static_assert(offsetof(SpatialAudioRecord, value08) == 0x08);
static_assert(offsetof(SpatialAudioRecord, value0C) == 0x0C);
static_assert(offsetof(SpatialAudioRecord, value10) == 0x10);
static_assert(sizeof(SpatialAudioRecord) == 0x14);

// 原 [57C368,57CB24)：保留 x86 handler 地址位型，不作为 x64 函数指针。
struct SoundRecord57C368 final {
    std::uint32_t id;
    std::uint32_t handlerToken;
    std::uint32_t categoryBits;
    std::uint32_t runtimeToken = 0;
    std::uint8_t dirty = 0;
    std::array<std::uint8_t, 3> reserved {};
};

static_assert(offsetof(SoundRecord57C368, id) == 0);
static_assert(offsetof(SoundRecord57C368, handlerToken) == 4);
static_assert(offsetof(SoundRecord57C368, categoryBits) == 8);
static_assert(offsetof(SoundRecord57C368, runtimeToken) == 12);
static_assert(offsetof(SoundRecord57C368, dirty) == 16);
static_assert(sizeof(SoundRecord57C368) == 0x14);

// 原 [104A3B8,104C9B8)：与 104E010 播放记录分离的 128 项配置表。
struct BgmConfigRecord final {
    char path[0x40];
    std::int32_t value40;
    std::uint8_t value44;
    std::uint8_t opaque45[3];
    std::int32_t value48;
};

static_assert(offsetof(BgmConfigRecord, value40) == 0x40);
static_assert(offsetof(BgmConfigRecord, value44) == 0x44);
static_assert(offsetof(BgmConfigRecord, value48) == 0x48);
static_assert(sizeof(BgmConfigRecord) == 0x4C);

namespace {

std::array<SpatialAudioRecord, 50> gSpatialRecords103D1C0 {};
static_assert(sizeof(gSpatialRecords103D1C0) == 0x3E8);

std::array<std::int32_t, 512> gBgmMapping103DFAC {};
std::array<BgmConfigRecord, 128> gBgmConfigRecords104A3B8 {};
static_assert(sizeof(gBgmMapping103DFAC) == 0x800);
static_assert(sizeof(gBgmConfigRecords104A3B8) == 0x2600);

std::array<SoundRecord57C368, 99> gSoundRecords57C368 {{
    {0, 0, 2},
    {1, 0x5029B0, 1},
    {2, 0x5006F0, 0},
    {18, 0x500370, 0},
    {19, 0x4FF650, 0},
    {14, 0x4FAEF0, 1},
    {15, 0x4FBB40, 1},
    {16, 0x4F0F70, 1},
    {20, 0x4F7150, 0},
    {21, 0x4F83B0, 0},
    {22, 0x4FA050, 0},
    {25, 0x4F5A60, 0},
    {26, 0x4F51D0, 0},
    {27, 0x4F4790, 1},
    {29, 0x505C20, 0},
    {30, 0x505170, 1},
    {28, 0x4F3ED0, 0},
    {31, 0x4F2A30, 1},
    {32, 0x4F2130, 1},
    {33, 0x4F1C40, 0},
    {34, 0x4F0B40, 2},
    {35, 0x4F0460, 2},
    {36, 0x4EFB70, 2},
    {37, 0x4EF0F0, 0},
    {38, 0x4EE380, 0},
    {39, 0x4EDB20, 0},
    {40, 0x4ED020, 2},
    {41, 0x4ECE00, 2},
    {42, 0x4EC240, 0},
    {43, 0x4EBD20, 0},
    {44, 0x4EBAA0, 0},
    {45, 0x4EB890, 0},
    {46, 0x4EB1C0, 0},
    {47, 0x4EAAA0, 0},
    {48, 0x4EA690, 0},
    {49, 0x4EA1B0, 0},
    {50, 0x4E9C80, 1},
    {94, 0x52FD60, 0},
    {93, 0x52F910, 0},
    {92, 0x52F670, 0},
    {91, 0x52F460, 0},
    {90, 0x52ED50, 0},
    {89, 0x52E650, 0},
    {88, 0x52E240, 0},
    {87, 0x52DCE0, 0},
    {86, 0x52D7B0, 1},
    {95, 0x530BA0, 0},
    {96, 0x532450, 0},
    {51, 0x4E4EA0, 2},
    {52, 0x4E5E40, 2},
    {23, 0x4F6CA0, 0},
    {24, 0x4F6220, 0},
    {3, 0x4FC730, 0},
    {4, 0x4FC8F0, 0},
    {5, 0x4FD1A0, 0},
    {6, 0x4FD6A0, 0},
    {7, 0x4FDBD0, 0},
    {8, 0x4FE0E0, 0},
    {9, 0x4FE860, 0},
    {10, 0x4FEF30, 0},
    {11, 0x4FF1A0, 0},
    {12, 0x4FF4C0, 0},
    {17, 0x4FA350, 1},
    {53, 0x4D3210, 2},
    {54, 0x4D3410, 2},
    {55, 0x4D6710, 2},
    {56, 0x4D6A60, 2},
    {57, 0x4D6EA0, 2},
    {62, 0x4D8DA0, 2},
    {63, 0x4D9220, 2},
    {65, 0x4DA580, 2},
    {64, 0x4DAA40, 2},
    {66, 0x4DBD30, 2},
    {67, 0x4DC290, 2},
    {68, 0x4DDFC0, 2},
    {69, 0x4DF300, 2},
    {70, 0x4DC7E0, 2},
    {71, 0x4DDB20, 2},
    {72, 0x4E0380, 2},
    {73, 0x4DF730, 2},
    {58, 0x4D7470, 2},
    {59, 0x4D7C20, 2},
    {60, 0x4D8230, 2},
    {61, 0x4D88C0, 2},
    {74, 0x4D4F00, 2},
    {75, 0x4D36F0, 2},
    {76, 0x4D4A50, 2},
    {77, 0x4E4A50, 2},
    {78, 0x4E4460, 2},
    {79, 0x4E92A0, 1},
    {80, 0x4E97E0, 1},
    {81, 0x4F0CD0, 2},
    {82, 0x4F0E20, 2},
    {83, 0x4D5250, 2},
    {84, 0x4D55C0, 2},
    {85, 0x4D62A0, 2},
    {98, 0x4021A0, 2},
    {99, 0x4014B0, 2},
    {100, 0, 2},
}};
std::array<std::uint32_t, 3> gSoundSelectionCC236C {};
static_assert(sizeof(gSoundRecords57C368) == 99 * 0x14);
static_assert(sizeof(gSoundSelectionCC236C) == 3 * sizeof(std::uint32_t));

} // namespace

std::uint32_t gValue103DE80 = 0;
std::uint32_t gValue10507D4 = 0;
std::int32_t gBgmTarget1050818 = 0;
std::int32_t gBgmRequest1050810 = 0;
std::int32_t gBgmStage1050814 = 0;
volatile std::uint32_t gMciDeviceId = 0;
std::uint32_t gMciCloseResult = 0;

std::int32_t Sub_524230(); // 原 DirectSound/音效文件初始化，尚未落位。
SpatialAudioRecord* Sub_52B9E0(); // 原通道、请求与音量表初始化，尚未落位。

std::int32_t InitializeAudio_5241A0()
{
    if ((platform::gRendererFlags & 0x20u) != 0) {
        gValue10507D4 = 1;
        return 0;
    }

    static_cast<void>(LoadBgmConfig_524060());
    if ((static_cast<std::uint16_t>(::GetAsyncKeyState(VK_SHIFT)) & 0x8000u) != 0) {
        gValue10507D4 = 1;
        return 0;
    }

    static_cast<void>(login::detail::Sub_44B5C0("\tinitDSound() Start"));
    const std::int32_t result = Sub_524230();
    if (result == 0) {
        gValue10507D4 = 1;
        return result;
    }

    static_cast<void>(login::detail::Sub_44B5C0("\tmusic_init() Start"));
    static_cast<void>(Sub_52B9E0());
    gValue10507D4 = 0;
    static_cast<void>(login::detail::Sub_44B5C0("t_music_init OK"));
    return 1;
}

std::int32_t LoadBgmConfig_524060()
{
    std::FILE* const file = std::fopen("bin\\bgm\\bgm.cf", "r");
    if (file == nullptr) {
        return 0;
    }

    gBgmMapping103DFAC.fill(-1);
    std::int32_t recordIndex = 0;
    std::int32_t mappingIndex = 0;
    std::int32_t value40 = 0;
    std::int32_t value48 = 0;
    std::uint8_t value44 = 0;
    char line[80] {};
    char path[80] {};

    while (std::fgets(line, sizeof(line), file) != nullptr) {
        const char first = line[0];
        if (first == '#' || first == ' ' || first == '\n' || first == '-') {
            continue;
        }

        std::int32_t parsed44 = value44;
        const int parsed = std::sscanf(
            line, "%d %d %s %d %d %d",
            &recordIndex, &mappingIndex, path, &value40, &parsed44, &value48);
        // 原第五项写入单字节槽时覆盖紧邻第六项的低三个字节。
        if (parsed == 5) {
            const std::uint32_t previous =
                std::bit_cast<std::uint32_t>(value48);
            const std::uint32_t fifth =
                std::bit_cast<std::uint32_t>(parsed44);
            value48 = std::bit_cast<std::int32_t>(
                (previous & 0xFF000000u) | (fifth >> 8));
        }
        if (parsed >= 5) {
            value44 = static_cast<std::uint8_t>(parsed44);
        }

        BgmConfigRecord& record =
            gBgmConfigRecords104A3B8[static_cast<std::size_t>(recordIndex)];
        const std::size_t bytes = std::strlen(path) + 1;
        std::memcpy(record.path, path, bytes);
        record.value40 = value40;
        record.value44 = value44;
        record.value48 = value48;
        gBgmMapping103DFAC[static_cast<std::size_t>(mappingIndex)] =
            recordIndex;
    }

    std::fclose(file);
    return 1;
}

std::int32_t SelectBgmTarget_52C9C0(const std::int32_t index)
{
    const std::int32_t target =
        gBgmMapping103DFAC[static_cast<std::size_t>(index)];
    if (target >= 0) {
        gBgmTarget1050818 = target;
    }
    return 0;
}

std::int32_t UpdateBgmRequest_52C910()
{
    const std::int32_t stage = gBgmStage1050814;
    switch (stage) {
    case 0: {
        const std::int32_t request = gBgmRequest1050810;
        std::int32_t result = request;
        if (request != 0) {
            const std::int32_t current = gRecordIndex;
            const std::int32_t target = gBgmTarget1050818;
            result = current;
            if (current != target) {
                const volatile std::int16_t& selected =
                    login::gSelectedCharacterIndex;
                const std::int32_t selectedValue = selected;
                gBgmStage1050814 = 1;
                static_cast<void>(login::detail::Sub_520750(selectedValue));
                result = login::detail::Sub_520200();
            }
        }
        gBgmRequest1050810 = 0;
        return result;
    }
    case 1: {
        const std::uint32_t result = Sub_52B990();
        ++gBgmStage1050814;
        return std::bit_cast<std::int32_t>(result);
    }
    case 2: {
        const std::int32_t flag = gFlag;
        if (flag == 0) {
            gBgmStage1050814 = 3;
        }
        return flag;
    }
    case 3: {
        const std::int32_t target = gBgmTarget1050818;
        const std::int32_t result = login::detail::LoadAndPlayBGM(
            std::bit_cast<std::uint32_t>(target));
        gBgmStage1050814 = 0;
        return result;
    }
    default:
        return stage;
    }
}

// 原 [0x5044C0,0x5044DF)：顺序扫描完整的 99 项可写表。
std::int32_t __cdecl Sub_5044C0(const std::uint32_t id)
{
    for (std::size_t index = 0; index < gSoundRecords57C368.size(); ++index) {
        if (gSoundRecords57C368[index].id == id) {
            return static_cast<std::int32_t>(index);
        }
    }
    return -1;
}

// 原 [0x504410,0x50445F)：类别 2 读首槽，其他类别以有符号 W8 索引共享槽。
std::int32_t __cdecl Sub_504410(const std::uint32_t id)
{
    if (id == 0 || id >= 100) {
        return 0;
    }
    const std::int32_t index = Sub_5044C0(id);
    if (index < 0) {
        return 0;
    }
    const std::int8_t category = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(
            gSoundRecords57C368[static_cast<std::size_t>(index)].categoryBits));
    if (category == 2) {
        return gSoundSelectionCC236C[0] == id;
    }
    return gSoundSelectionCC236C[static_cast<std::size_t>(category)] == id;
}

// 原 [0x504180,0x5042C6)：按表中可写类别发布选择与标记。
std::int32_t __cdecl Sub_504180(const std::uint32_t id)
{
    if (world::gCurrentSceneType == 10
        && (id == 1 || id == 2 || id == 18 || id == 14
            || id == 20 || id == 17)) {
        return 0;
    }
    if (id == 0 || id >= 100) {
        return 0;
    }
    const std::int32_t index = Sub_5044C0(id);
    if (index < 0) {
        return 0;
    }
    SoundRecord57C368& record =
        gSoundRecords57C368[static_cast<std::size_t>(index)];
    const std::uint8_t categoryByte =
        static_cast<std::uint8_t>(record.categoryBits);
    if (categoryByte != 0 && world::detail::gValueCAFEA0 == 0) {
        world::detail::gValueCAFEA0 =
            static_cast<std::uint32_t>(Sub_504410(1));
    }

    if (categoryByte == 2) {
        if (gSoundSelectionCC236C[0] == id) {
            return 0;
        }
        gSoundSelectionCC236C[0] = id;
        record.dirty = 1;
        gSoundSelectionCC236C[1] = 0;
        return 1;
    }

    const std::int32_t previousIndex = Sub_5044C0(gSoundSelectionCC236C[0]);
    if (previousIndex < 0) {
        return 0;
    }
    if (gSoundSelectionCC236C[0] != 0
        && static_cast<std::uint8_t>(
            gSoundRecords57C368[static_cast<std::size_t>(previousIndex)].categoryBits)
            == 2) {
        const std::int8_t category = std::bit_cast<std::int8_t>(categoryByte);
        gSoundSelectionCC236C[0] = 0;
        gSoundSelectionCC236C[1] = 0;
        gSoundSelectionCC236C[static_cast<std::size_t>(category)] = id;
        record.dirty = 1;
        return 1;
    }

    const std::int8_t category = std::bit_cast<std::int8_t>(categoryByte);
    if (gSoundSelectionCC236C[static_cast<std::size_t>(category)] == id) {
        return 0;
    }
    gSoundSelectionCC236C[static_cast<std::size_t>(category)] = id;
    record.dirty = 1;
    return 1;
}

// 原 [0x504AF0,0x504B0E)：先采现值，非零时重入 504180 后才清槽。
std::int32_t __cdecl Sub_504AF0()
{
    const std::uint32_t current = world::detail::gValueCAFEA0;
    if (current == 0) {
        return 0;
    }
    const std::int32_t result = Sub_504180(1);
    world::detail::gValueCAFEA0 = 0;
    return result;
}

// 原 [0x5042D0,0x504373)：保留编号 100、类别 2 的不同返回与清理路径。
std::int32_t __cdecl Sub_5042D0(const std::uint32_t id)
{
    const std::uint32_t previousSecond = gSoundSelectionCC236C[1];
    if (id == 100) {
        gSoundSelectionCC236C[0] = 0;
        gSoundSelectionCC236C[1] = 0;
        if (previousSecond != 1 && (platform::gKeyStates[27] & 4u) != 0) {
            static_cast<void>(Sub_504AF0());
        }
        return 1;
    }
    if (id == 0 || id > 100) {
        return 0;
    }
    const std::int32_t index = Sub_5044C0(id);
    if (index < 0) {
        return 0;
    }
    const std::int8_t category = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(
            gSoundRecords57C368[static_cast<std::size_t>(index)].categoryBits));
    if (category == 2 && gSoundSelectionCC236C[0] == id) {
        gSoundSelectionCC236C[0] = 0;
        gSoundSelectionCC236C[1] = 0;
        if (previousSecond != 1) {
            static_cast<void>(Sub_504AF0());
        }
        return 0;
    }
    if (gSoundSelectionCC236C[static_cast<std::size_t>(category)] != id) {
        return 0;
    }
    gSoundSelectionCC236C[static_cast<std::size_t>(category)] = 0;
    if (previousSecond != 1 && category != 0) {
        static_cast<void>(Sub_504AF0());
    }
    return 1;
}

// 原 [0x504380,0x5043C6)：分别比较首两槽，不合并两次写入。
std::int32_t __cdecl Sub_504380(const std::uint32_t id)
{
    if (id == 0 || id >= 100) {
        return 0;
    }
    std::int32_t result = 0;
    if (gSoundSelectionCC236C[0] != id) {
        gSoundSelectionCC236C[0] = 0;
        result = 1;
    }
    if (gSoundSelectionCC236C[1] != id) {
        gSoundSelectionCC236C[1] = 0;
        result = 1;
    }
    return result;
}

// 原 [0x5043D0,0x504404)：索引 2 单独查首两槽，其余读同一连续存储。
std::int32_t __cdecl Sub_5043D0(const std::int32_t index)
{
    if (index == 2) {
        return gSoundSelectionCC236C[0] != 0
            || gSoundSelectionCC236C[1] != 0;
    }
    return gSoundSelectionCC236C[static_cast<std::size_t>(index)] != 0;
}

std::uint32_t Sub_52CCE0() noexcept
{
    if (gValue103DE80 == 1) {
        return 1;
    }

    mciSendCommandA(gMciDeviceId, MCI_STOP, 0, 0);
    const std::uint32_t result = mciSendCommandA(gMciDeviceId, MCI_CLOSE, 0, 0);
    gMciCloseResult = result;
    return (result & 0xFFFFFF00u) | static_cast<std::uint32_t>(result == 0);
}

SpatialAudioRecord* __cdecl Sub_52CD40()
{
    for (auto& record : gSpatialRecords103D1C0) {
        record.value04 = -1;
        record.value00 = 1;
    }
    return gSpatialRecords103D1C0.data() + gSpatialRecords103D1C0.size();
}

// 原 [0x52CDE0,0x52CE97)；在同一空间音频表发布更近的屏幕坐标与距离。
std::int32_t __cdecl Sub_52CDE0(
    const std::int32_t index,
    const std::int32_t sourceX,
    const std::int32_t sourceY)
{
    const float worldY = static_cast<float>(static_cast<double>(sourceY) * 64.0);
    const float worldX = static_cast<float>(static_cast<double>(sourceX) * 64.0);
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);

    const double offsetX = static_cast<double>(screenX) - 320.0;
    const double offsetY = static_cast<double>(screenY) - 240.0;
    const std::int32_t distance = static_cast<std::int32_t>(
        std::sqrt(offsetY * offsetY + offsetX * offsetX));

    SpatialAudioRecord& record =
        gSpatialRecords103D1C0[static_cast<std::size_t>(index)];
    if (record.value00 == 0 && record.value04 != -1
        && distance >= record.value10) {
        return index;
    }

    const double projectedX = static_cast<double>(screenX);
    record.value04 = index;
    const std::int32_t x = static_cast<std::int32_t>(projectedX);
    const double projectedY = static_cast<double>(screenY);
    record.value08 = x;
    const std::int32_t y = static_cast<std::int32_t>(projectedY);
    record.value0C = y;
    record.value10 = distance;
    record.value00 = 0;
    return y;
}

// 原 [0x52CD60,0x52CDDB)；首个距离阈值不符的有效记录立即结束扫描。
void __cdecl Sub_52CD60()
{
    for (SpatialAudioRecord& record : gSpatialRecords103D1C0) {
        const std::int32_t id = record.value04;
        if (id == -1) {
            continue;
        }

        const std::int32_t x = record.value08;
        const std::int32_t y = record.value0C;
        const std::uint32_t sumBits =
            static_cast<std::uint32_t>(x) * static_cast<std::uint32_t>(x)
            + static_cast<std::uint32_t>(y) * static_cast<std::uint32_t>(y);
        const std::int32_t distance = static_cast<std::int32_t>(
            std::sqrt(static_cast<double>(std::bit_cast<std::int32_t>(sumBits))));
        const std::int32_t quotient = (distance * 16) / 106;
        if (quotient >= 127 || quotient < 20) {
            return;
        }

        static_cast<void>(Sub_52C9E0(id + 249, x, y));
        record.value00 = 1;
    }
}

std::uint32_t Sub_52B990() noexcept
{
    const std::int32_t index = gRecordIndex;
    if (index < 0 || static_cast<std::uint32_t>(index) >= kRecordCount) {
        return std::bit_cast<std::uint32_t>(index);
    }

    gFlag = 1;
    const std::int32_t value00 = gRecords[static_cast<std::size_t>(index)].value00;
    const std::int32_t musicVolume = cg::platform::gMusicVolume;
    const std::uint32_t productBits =
        std::bit_cast<std::uint32_t>(value00)
        * std::bit_cast<std::uint32_t>(musicVolume);
    const std::int32_t product = std::bit_cast<std::int32_t>(productBits);
    gQuotient = product / 15;
    return productBits >> 31;
}

} // namespace cg::audio
