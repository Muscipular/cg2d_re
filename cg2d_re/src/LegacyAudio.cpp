#include "cg/LegacyAudio.hpp"

#include "LoginStateDependencies.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/ResourcePaths.hpp"
#include "cg/platform/SdlAudioBackend.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/world/SceneState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>

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

// 原 104DFD0 起的 BGM 播放记录；52B990 读取每项 +0x40。
struct LegacyAudioRecord final {
    char path[0x40];
    std::int32_t value40;
    std::uint8_t value44;
    std::uint8_t opaque45[3];
    std::int32_t value48;
};

static_assert(offsetof(LegacyAudioRecord, value40) == 0x40);
static_assert(offsetof(LegacyAudioRecord, value44) == 0x44);
static_assert(offsetof(LegacyAudioRecord, value48) == 0x48);
static_assert(sizeof(LegacyAudioRecord) == 0x4C);

constexpr std::uint32_t kRecordCount = 128;

std::int32_t gRecordIndex = -1;
std::array<LegacyAudioRecord, kRecordCount> gRecords {};
std::int32_t gFlag = 0;
std::int32_t gQuotient = 0;
std::uint32_t gBgmActive1050808 = 0; // 原播放状态 W32；52BE60 置位，52B940 清理。
std::uint32_t gBgmMode1050820 = 0; // 原 BGM 流式模式 W32；52B8E0 消费。
std::atomic<std::uint8_t> gBgmStopNotice105081C {0};

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

// 原 [0x104C9B8,0x104D0C0)：音效请求表，独立于空间坐标记录。
struct SoundRequestRecord final {
    std::int32_t channelIndex;
    std::int32_t seNo;
    std::int32_t requestCount;
    std::int32_t distanceOrAtten;
    std::int32_t targetVolumeIndex;
    std::int32_t appliedVolumeIndex;
    std::int32_t screenX;
    std::int32_t targetPanIndex;
    std::int32_t appliedPanIndex;
};

static_assert(offsetof(SoundRequestRecord, channelIndex) == 0x00);
static_assert(offsetof(SoundRequestRecord, seNo) == 0x04);
static_assert(offsetof(SoundRequestRecord, requestCount) == 0x08);
static_assert(offsetof(SoundRequestRecord, distanceOrAtten) == 0x0C);
static_assert(offsetof(SoundRequestRecord, targetVolumeIndex) == 0x10);
static_assert(offsetof(SoundRequestRecord, appliedVolumeIndex) == 0x14);
static_assert(offsetof(SoundRequestRecord, screenX) == 0x18);
static_assert(offsetof(SoundRequestRecord, targetPanIndex) == 0x1C);
static_assert(offsetof(SoundRequestRecord, appliedPanIndex) == 0x20);
static_assert(sizeof(SoundRequestRecord) == 0x24);

// 原 [0x104D0C8,0x104DFC8)：64 项通道；104DFCC 是独立样本计数槽。
struct SoundChannelState final {
    std::int32_t value00;
    std::int32_t currentSeNo;
    std::array<std::byte, 0x28> opaque08;
    std::int32_t value30;
    std::array<std::byte, 0x08> opaque34;
};

static_assert(offsetof(SoundChannelState, value00) == 0x00);
static_assert(offsetof(SoundChannelState, currentSeNo) == 0x04);
static_assert(offsetof(SoundChannelState, value30) == 0x30);
static_assert(sizeof(SoundChannelState) == 0x3C);

// 原 [0x103E7B0,0x104A330)：+04/+1C 尚未定性，保留 W32 原位。
struct SoundEffectDefinition final {
    std::int32_t sampleIndex;
    std::uint32_t unknown04;
    std::int32_t loop;
    std::int32_t volume;
    std::int32_t pitchIndex;
    std::int32_t cooldown;
    std::int32_t baseFrequency;
    std::uint32_t unknown1C;
    char path[0x40];
};

static_assert(offsetof(SoundEffectDefinition, volume) == 0x0C);
static_assert(offsetof(SoundEffectDefinition, cooldown) == 0x14);
static_assert(offsetof(SoundEffectDefinition, path) == 0x20);
static_assert(sizeof(SoundEffectDefinition) == 0x60);

namespace {

// 524230 的内联 0x46 字节栈项；最后一个未写填充字节不参与加载。
struct SeInitEntry final {
    std::int16_t id;
    const char* path;
    std::uint8_t volume;
    std::uint8_t pitch;
    std::uint8_t loop;
};

constexpr std::array<SeInitEntry, 270> kSeInit524230 {{
#include "LegacySeDefinitions.inc"
}};

// 原 52B710 的 RIFF/WAVE、PCM fmt 和后续 data 门禁，样本解码仍交给 SDL。
bool IsPcmWave(const char* const path)
{
    std::FILE* const file = std::fopen(path, "rb");
    if (file == nullptr) {
        return false;
    }

    std::uint8_t header[12] {};
    bool valid = false;
    if (std::fread(header, 1, sizeof(header), file) == sizeof(header)
        && std::memcmp(header, "RIFF", 4) == 0
        && std::memcmp(header + 8, "WAVE", 4) == 0) {
        bool pcmFormat = false;
        std::uint8_t chunk[8] {};
        while (std::fread(chunk, 1, sizeof(chunk), file) == sizeof(chunk)) {
            const std::uint32_t size =
                static_cast<std::uint32_t>(chunk[4])
                | static_cast<std::uint32_t>(chunk[5]) << 8
                | static_cast<std::uint32_t>(chunk[6]) << 16
                | static_cast<std::uint32_t>(chunk[7]) << 24;
            if (std::memcmp(chunk, "data", 4) == 0 && pcmFormat) {
                valid = true;
                break;
            }
            if (std::memcmp(chunk, "fmt ", 4) == 0) {
                std::uint8_t format[16] {};
                if (size < sizeof(format)
                    || std::fread(format, 1, sizeof(format), file) != sizeof(format)
                    || format[0] != 1 || format[1] != 0) {
                    break;
                }
                pcmFormat = true;
                if (size - sizeof(format)
                    > static_cast<std::uint32_t>((std::numeric_limits<long>::max)() - 1)
                    || std::fseek(file,
                        static_cast<long>(size - sizeof(format) + (size & 1u)),
                        SEEK_CUR) != 0) {
                    break;
                }
            } else if (size > static_cast<std::uint32_t>(
                    (std::numeric_limits<long>::max)() - 1)
                || std::fseek(file, static_cast<long>(size + (size & 1u)),
                    SEEK_CUR) != 0) {
                break;
            }
        }
    }
    std::fclose(file);
    return valid;
}

std::string AudioPathToUtf8(const char* const path)
{
    const int wideLength =
        ::MultiByteToWideChar(CP_ACP, 0, path, -1, nullptr, 0);
    if (wideLength <= 0) {
        return {};
    }
    std::wstring wide(static_cast<std::size_t>(wideLength), L'\0');
    if (::MultiByteToWideChar(
            CP_ACP, 0, path, -1, wide.data(), wideLength) == 0) {
        return {};
    }
    const int utf8Length = ::WideCharToMultiByte(
        CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Length <= 0) {
        return {};
    }
    std::string utf8(static_cast<std::size_t>(utf8Length), '\0');
    if (::WideCharToMultiByte(
            CP_UTF8, 0, wide.c_str(), -1, utf8.data(), utf8Length,
            nullptr, nullptr) == 0) {
        return {};
    }
    utf8.pop_back();
    return utf8;
}

std::array<SpatialAudioRecord, 50> gSpatialRecords103D1C0 {};
static_assert(sizeof(gSpatialRecords103D1C0) == 0x3E8);

std::array<std::int32_t, 512> gBgmMapping103DFAC {};
std::array<BgmConfigRecord, 128> gBgmConfigRecords104A3B8 {};
std::array<SoundRequestRecord, 50> gSoundRequests104C9B8 {};
std::array<SoundEffectDefinition, 500> gSoundEffects103E7B0 {};
std::array<SoundChannelState, 64> gSoundChannels104D0C8 {};
std::array<std::byte, 0x80> gAudioBytes103DDFC {};
std::array<std::byte, 0x80> gAudioBytes103DD7C {};
std::array<std::byte, 0x80> gAudioBytes104A334 {};
std::array<std::int32_t, 128> gPanVal103CFBC {};
std::array<std::int32_t, 128> gSoundVolValue10505D0 {};
// 原 [58D3D4,58D4F8)：52C450/52C680 以 baseFrequency+pitchIndex 读取。
std::array<std::uint32_t, 73> gPitchFrequencies58D3D4 {{
    1460, 2756, 2920, 3093, 3277, 3472, 3679, 3866,
    4129, 4375, 4635, 4910, 5202, 5512, 5840, 6187,
    6555, 6945, 7358, 7732, 8259, 8750, 9270, 9821,
    10405, 11025, 11680, 12375, 13110, 13890, 14716, 15464,
    16518, 17501, 18541, 19643, 20811, 22050, 23360, 24750,
    26221, 27780, 29432, 30929, 33037, 35002, 37082, 39287,
    41623, 44100, 46721, 49500, 52443, 55561, 58865, 61859,
    66074, 70004, 74165, 78575, 83247, 88200, 93442, 99001,
    104887, 111123, 117731, 123718, 132149, 140008, 148331, 157150,
    166495,
}};
// 原 [58D468,58D4F8)：52BE60 用 BGM 编号选出的有符号 W8 读取频率。
std::array<std::uint32_t, 36> gBgmFrequencies58D468 {{
    22050, 23360, 24750, 26221, 27780, 29432, 30929, 33037,
    35002, 37082, 39287, 41623, 44100, 46721, 49500, 52443,
    55561, 58865, 61859, 66074, 70004, 74165, 78575, 83247,
    88200, 93442, 99001, 104887, 111123, 117731, 123718, 132149,
    140008, 148331, 157150, 166495,
}};
std::int32_t gNextSeChannel105082C = 0;
std::int32_t gLastSeNo58D4FC = -1;
std::int32_t gSampleCount104DFCC = 0;
platform::SdlAudioBackend gSdlAudioBackend;
static_assert(sizeof(gBgmMapping103DFAC) == 0x800);
static_assert(sizeof(gBgmConfigRecords104A3B8) == 0x2600);
static_assert(sizeof(gSoundRequests104C9B8) == 0x708);
static_assert(sizeof(gSoundEffects103E7B0) == 0xBB80);
static_assert(sizeof(gSoundChannels104D0C8) == 0xF00);
static_assert(sizeof(gPanVal103CFBC) == 0x200);
static_assert(sizeof(gSoundVolValue10505D0) == 0x200);
static_assert(sizeof(gPitchFrequencies58D3D4) == 73 * sizeof(std::uint32_t));
static_assert(sizeof(gBgmFrequencies58D468) == 0x90);

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

SpatialAudioRecord* __cdecl Sub_52B9E0(); // 原通道、请求与两张运行表初始化。
// 原 [52B8E0,52B93F)：通知由独立 SDL 工作线程生产，未启动时不伪造完成。
void Sub_52B8E0()
{
    if (gBgmMode1050820 == 0 || !gSdlAudioBackend.HasBgm()) {
        return;
    }

    static_cast<void>(gSdlAudioBackend.StopBgm());
    if (!gSdlAudioBackend.RequestBgmStop()) {
        return;
    }
    while (gBgmStopNotice105081C.load(std::memory_order_acquire) != 1) {
    }
    gBgmMode1050820 = 0;
    gBgmStopNotice105081C.store(0, std::memory_order_release);
    gSdlAudioBackend.ReleaseBgm();
}

// 原 [0x52B940,0x52B98E)：两路资源清理后依次清三个共享状态。
void Sub_52B940()
{
    if (gBgmMode1050820 != 0) {
        Sub_52B8E0();
    } else if (gSdlAudioBackend.HasBgm()) {
        gSdlAudioBackend.ReleaseBgm();
    }
    gFlag = 0;
    gBgmActive1050808 = 0;
    gBgmStage1050814 = 0;
}

// 原 [0x52B630,0x52B709)：保留禁用门禁与 BGM、通道、样本、设备的释放次序。
void Sub_52B630()
{
    if (gValue10507D4 != 0) {
        return;
    }

    Sub_52B8E0();
    if (gSdlAudioBackend.HasBgm()) {
        gSdlAudioBackend.ReleaseBgm();
    }
    for (std::size_t index = 0; index < platform::SdlAudioBackend::kChannelCount;
         ++index) {
        if (gSdlAudioBackend.HasChannel(index)) {
            static_cast<void>(gSdlAudioBackend.StopChannel(index));
            gSdlAudioBackend.ReleaseChannel(index);
        }
    }
    gSdlAudioBackend.Close();
}

// 原 [524230,52B627)：配置项按原栈表顺序加载；设备/样本/通道机制使用 SDL3。
std::int32_t Sub_524230()
{
    if (!gSdlAudioBackend.Open()) {
        return 0;
    }

    gSampleCount104DFCC = 0;
    for (SoundEffectDefinition& effect : gSoundEffects103E7B0) {
        effect.sampleIndex = -1;
    }

    for (const SeInitEntry& entry : kSeInit524230) {
        if (entry.id > 500
            || static_cast<std::int8_t>(entry.volume) < 0
            || static_cast<std::int8_t>(entry.pitch) > 60) {
            gSdlAudioBackend.Close();
            return 0;
        }

        SoundEffectDefinition& effect =
            gSoundEffects103E7B0[static_cast<std::size_t>(entry.id)];
        effect.sampleIndex = gSampleCount104DFCC;
        char openPath[256] {};
        std::snprintf(openPath, sizeof(openPath), "%s\\%s",
            resource::gBinPath, entry.path);
        std::snprintf(effect.path, sizeof(effect.path), "%s\\%s",
            resource::gBinPath, entry.path);
        effect.volume = static_cast<std::int8_t>(entry.volume);
        effect.pitchIndex = static_cast<std::int8_t>(entry.pitch);
        effect.loop = static_cast<std::int8_t>(entry.loop);

        std::FILE* const file = std::fopen(openPath, "rb");
        if (file == nullptr) {
            continue;
        }
        static_cast<void>(std::fseek(file, 24, SEEK_SET));
        std::uint32_t frequency = 0;
        for (int shift = 0; shift < 32; shift += 8) {
            frequency |= static_cast<std::uint32_t>(
                std::fgetc(file)) << shift;
        }
        std::fclose(file);
        if (frequency == 11025) {
            effect.baseFrequency = 25;
        } else if (frequency == 22050) {
            effect.baseFrequency = 37;
        } else if (frequency == 44100) {
            effect.baseFrequency = 49;
        }

        const std::string utf8Path = AudioPathToUtf8(openPath);
        if (utf8Path.empty() || !IsPcmWave(openPath)
            || !gSdlAudioBackend.LoadWav(
                static_cast<std::size_t>(gSampleCount104DFCC),
                utf8Path.c_str())) {
            gSdlAudioBackend.Close();
            return 0;
        }
        ++gSampleCount104DFCC;
    }

    for (std::size_t channel = 0;
        channel < platform::SdlAudioBackend::kChannelCount; ++channel) {
        if (!gSdlAudioBackend.PrepareChannel(channel, 0)) {
            gSdlAudioBackend.Close();
            return 0;
        }
    }

    for (std::size_t index = 0; index < gRecords.size(); ++index) {
        const BgmConfigRecord& config = gBgmConfigRecords104A3B8[index];
        LegacyAudioRecord& record = gRecords[index];
        std::snprintf(record.path, sizeof(record.path), "%s\\%s",
            resource::gBinPath, config.path);
        record.value40 = config.value40;
        record.value44 = config.value44;
        record.value48 = config.value48;
    }
    return 1;
}

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

// 原 [0x52B9E0,0x52BBD6)：按通道、清零区、请求、声像/音量表顺序初始化。
SpatialAudioRecord* __cdecl Sub_52B9E0()
{
    for (SoundChannelState& channel : gSoundChannels104D0C8) {
        channel.value00 = 0;
        channel.currentSeNo = -1;
        channel.value30 = 0;
    }

    gAudioBytes103DDFC.fill(std::byte {0});
    gAudioBytes103DD7C.fill(std::byte {0});
    gAudioBytes104A334.fill(std::byte {0});

    for (SoundRequestRecord& request : gSoundRequests104C9B8) {
        request.channelIndex = -1;
        request.requestCount = 0;
    }

    const double logBase = std::log(100.0);
    const double logMax = std::log(127.0) / logBase;
    const std::int32_t panMax =
        static_cast<std::int32_t>(logMax * 5000.0);
    std::size_t panIndex = 64;
    for (std::int32_t level = 127; level >= 1; level -= 2) {
        std::int32_t pan =
            panMax - static_cast<std::int32_t>(
                std::log(static_cast<double>(level)) / logBase * 5000.0);
        if (pan > 5000) {
            pan = 10000;
        }
        gPanVal103CFBC[panIndex++] = pan;
    }

    panIndex = 64;
    for (std::int32_t level = 127; level >= 1; level -= 2) {
        std::int32_t pan =
            panMax - static_cast<std::int32_t>(
                std::log(static_cast<double>(level)) / logBase * 5000.0);
        if (pan > 5000) {
            pan = 10000;
        }
        gPanVal103CFBC[panIndex--] = -pan;
    }
    gPanVal103CFBC[0] = -10000;
    gPanVal103CFBC[127] = 10000;

    const std::int32_t volumeMax =
        static_cast<std::int32_t>(logMax * -10000.0);
    for (std::int32_t level = 127; level >= 1; --level) {
        std::int32_t volume =
            -volumeMax - static_cast<std::int32_t>(
                std::log(static_cast<double>(level)) / logBase * 10000.0);
        if (volume > 10000) {
            volume = 10000;
        }
        gSoundVolValue10505D0[static_cast<std::size_t>(level)] = -volume;
    }
    gSoundVolValue10505D0[0] = gSoundVolValue10505D0[1];
    return Sub_52CD40();
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

std::int32_t __cdecl Sub_52C9E0(
    const std::int32_t id, const std::int32_t x, const std::int32_t y)
{
    if (id < 250 || id > 299) {
        return 0;
    }

    const std::int32_t dx = x - 320;
    const std::int32_t dy = y - 240;
    const std::int32_t distance = static_cast<std::int32_t>(
        std::sqrt(
            static_cast<double>(dx) * dx
            + static_cast<double>(dy) * dy));
    const std::int32_t quotient = (distance * 16) / 106;
    if (quotient >= 127) {
        return 0;
    }

    SoundRequestRecord& request =
        gSoundRequests104C9B8[static_cast<std::size_t>(id - 250)];
    const std::int32_t count = request.requestCount;
    if (count != 0 && request.distanceOrAtten <= distance) {
        ++request.requestCount;
        return 0;
    }

    const bool newRequest = count == 0;
    request.distanceOrAtten = distance;
    const volatile std::int32_t& effectVolume =
        gSoundEffects103E7B0[static_cast<std::size_t>(id - 249)].volume;
    const std::int32_t attenuated =
        ((127 - quotient) * effectVolume) / 127;
    const volatile std::int32_t& masterVolume = platform::gEffectVolume;
    request.targetVolumeIndex = (attenuated * masterVolume) / 15;
    if (newRequest && request.channelIndex == -1) {
        request.seNo = id - 249;
    }
    request.screenX = x;
    ++request.requestCount;
    return 0;
}

// 原 [0x52C450,0x52C677)：直接播放，冷却非零不触碰通道。
std::int32_t __cdecl PlaySe_52C450(
    const std::int32_t seNo, const std::int32_t screenX)
{
    gLastSeNo58D4FC = seNo;
    if (gValue10507D4 != 0 || platform::gEnableSound == 0
        || seNo < 0 || seNo > 500 || !gSdlAudioBackend.IsOpen()) {
        return -1;
    }

    SoundEffectDefinition& effect =
        gSoundEffects103E7B0[static_cast<std::size_t>(seNo)];
    if (effect.sampleIndex == -1) {
        return -1;
    }
    if (effect.cooldown != 0) {
        return 0;
    }

    gNextSeChannel105082C =
        static_cast<std::int32_t>(
            static_cast<std::uint32_t>(gNextSeChannel105082C) & 0x3Fu);
    const std::size_t channelIndex =
        static_cast<std::size_t>(gNextSeChannel105082C);
    SoundChannelState& channel = gSoundChannels104D0C8[channelIndex];
    if (channel.currentSeNo == seNo) {
        static_cast<void>(gSdlAudioBackend.RewindChannel(channelIndex));
    } else {
        const std::int32_t previous = channel.currentSeNo;
        channel.currentSeNo = seNo;
        if (previous != -2 && gSdlAudioBackend.HasChannel(channelIndex)) {
            gSdlAudioBackend.ReleaseChannel(channelIndex);
        }
        if (!gSdlAudioBackend.PrepareChannel(
                static_cast<std::size_t>(gNextSeChannel105082C),
                static_cast<std::size_t>(effect.sampleIndex))) {
            gSoundChannels104D0C8[static_cast<std::size_t>(
                gNextSeChannel105082C)].currentSeNo = -2;
            return -1;
        }
    }

    const std::int32_t panIndex = platform::gUseStereo == 0
        ? 64
        : (std::clamp)(screenX / 5, 0, 127);
    const std::int32_t scaledVolume = (effect.volume * 127) / 127;
    const std::int32_t volumeIndex =
        (scaledVolume * platform::gEffectVolume) / 15;
    static_cast<void>(gSdlAudioBackend.SetChannelVolume(
        static_cast<std::size_t>(gNextSeChannel105082C),
        gSoundVolValue10505D0[static_cast<std::size_t>(volumeIndex)]));
    static_cast<void>(gSdlAudioBackend.SetChannelPan(
        static_cast<std::size_t>(gNextSeChannel105082C),
        gPanVal103CFBC[static_cast<std::size_t>(panIndex)]));
    if (effect.pitchIndex != 0) {
        static_cast<void>(gSdlAudioBackend.SetChannelFrequency(
            static_cast<std::size_t>(gNextSeChannel105082C),
            gPitchFrequencies58D3D4[static_cast<std::size_t>(
                effect.pitchIndex + effect.baseFrequency)]));
    }
    static_cast<void>(gSdlAudioBackend.PlayChannel(
        static_cast<std::size_t>(gNextSeChannel105082C), false));

    std::uint32_t next =
        static_cast<std::uint32_t>(gNextSeChannel105082C);
    do {
        next = (next + 1u) & 0x3Fu;
    } while (gSoundChannels104D0C8[next].value30 != 0);
    effect.cooldown = 5;
    gNextSeChannel105082C = static_cast<std::int32_t>(next);
    return 0;
}

// 原 [0x52C680,0x52C90D)：请求已发布后更新通道及设备，只有复制失败被视为错误。
std::int32_t __cdecl Sub_52C680(
    const std::int32_t requestIndex, const std::int32_t isNew)
{
    if (gValue10507D4 != 0 || platform::gEnableSound == 0
        || requestIndex < 0 || requestIndex >= 50) {
        return -1;
    }

    SoundRequestRecord& request =
        gSoundRequests104C9B8[static_cast<std::size_t>(requestIndex)];
    const std::int32_t seNo = request.seNo;
    if (seNo > 500) {
        return -1;
    }
    SoundEffectDefinition& effect =
        gSoundEffects103E7B0[static_cast<std::size_t>(seNo)];
    if (effect.sampleIndex == -1
        || request.channelIndex < 0 || request.channelIndex > 64) {
        return -1;
    }

    const std::int32_t quotient = (request.distanceOrAtten * 16) / 106;
    request.distanceOrAtten = quotient;
    if (quotient >= 127) {
        request.distanceOrAtten = 127;
    }

    if (isNew != 0) {
        const std::int32_t channelIndex = request.channelIndex;
        SoundChannelState& channel =
            gSoundChannels104D0C8[static_cast<std::size_t>(channelIndex)];
        if (channel.currentSeNo != request.seNo) {
            const std::int32_t previous = channel.currentSeNo;
            channel.currentSeNo = request.seNo;
            if (previous != -2
                && gSdlAudioBackend.HasChannel(
                    static_cast<std::size_t>(channelIndex))) {
                gSdlAudioBackend.ReleaseChannel(
                    static_cast<std::size_t>(channelIndex));
            }
            if (!gSdlAudioBackend.PrepareChannel(
                    static_cast<std::size_t>(request.channelIndex),
                    static_cast<std::size_t>(
                        gSoundEffects103E7B0[static_cast<std::size_t>(
                            request.seNo)].sampleIndex))) {
                gSoundChannels104D0C8[static_cast<std::size_t>(
                    request.channelIndex)].currentSeNo = -2;
                return -1;
            }
        }
    }

    if (request.targetVolumeIndex != request.appliedVolumeIndex) {
        static_cast<void>(gSdlAudioBackend.SetChannelVolume(
            static_cast<std::size_t>(request.channelIndex),
            gSoundVolValue10505D0[static_cast<std::size_t>(
                request.targetVolumeIndex)]));
        request.appliedVolumeIndex = request.targetVolumeIndex;
    }
    if (request.targetPanIndex != request.appliedPanIndex) {
        static_cast<void>(gSdlAudioBackend.SetChannelPan(
            static_cast<std::size_t>(request.channelIndex),
            gPanVal103CFBC[static_cast<std::size_t>(
                request.targetPanIndex)]));
        request.appliedPanIndex = request.targetPanIndex;
    }

    const std::int32_t currentSeNo = request.seNo;
    const std::int32_t pitch =
        gSoundEffects103E7B0[static_cast<std::size_t>(currentSeNo)].pitchIndex;
    if (pitch != 0) {
        const std::int32_t base =
            gSoundEffects103E7B0[static_cast<std::size_t>(
                currentSeNo)].baseFrequency;
        static_cast<void>(gSdlAudioBackend.SetChannelFrequency(
            static_cast<std::size_t>(request.channelIndex),
            gPitchFrequencies58D3D4[static_cast<std::size_t>(base + pitch)]));
    }

    if (isNew != 0) {
        static_cast<void>(gSdlAudioBackend.RewindChannel(
            static_cast<std::size_t>(request.channelIndex)));
        const bool loop =
            gSoundEffects103E7B0[static_cast<std::size_t>(request.seNo)].loop != 0;
        if (loop) {
            gSoundChannels104D0C8[static_cast<std::size_t>(
                request.channelIndex)].value30 = 1;
        }
        static_cast<void>(gSdlAudioBackend.PlayChannel(
            static_cast<std::size_t>(request.channelIndex), loop));

        std::uint32_t next =
            static_cast<std::uint32_t>(gNextSeChannel105082C);
        do {
            next = (next + 1u) & 0x3Fu;
        } while (gSoundChannels104D0C8[next].value30 != 0);
        gNextSeChannel105082C = static_cast<std::int32_t>(next);
    }
    return 0;
}

// 原 [0x52CB50,0x52CC3D)：遍历唯一请求表；失败的播放请求仍清计数。
std::int32_t __cdecl Sub_52CB50()
{
    std::int32_t result = -1;
    for (std::size_t index = 0; index < gSoundRequests104C9B8.size(); ++index) {
        SoundRequestRecord& request = gSoundRequests104C9B8[index];
        if (request.requestCount == 0) {
            const std::int32_t channel = request.channelIndex;
            result = channel;
            if (channel == -1) {
                continue;
            }
            if (channel >= 0 && channel < 64) {
                static_cast<void>(gSdlAudioBackend.StopChannel(
                    static_cast<std::size_t>(channel)));
                const std::int32_t reread = request.channelIndex;
                gSoundChannels104D0C8[static_cast<std::size_t>(
                    reread)].value30 = 0;
                result = reread * 3;
            }
            request.channelIndex = -1;
            continue;
        }

        const bool isNew = request.channelIndex == -1;
        if (isNew) {
            request.channelIndex = gNextSeChannel105082C;
            request.appliedVolumeIndex = -1;
            request.appliedPanIndex = -1;
        }

        const std::int32_t desired = platform::gUseStereo == 0
            ? 64
            : (std::clamp)(request.screenX / 5, 0, 127);
        if (isNew) {
            request.targetPanIndex = desired;
        } else {
            request.targetPanIndex += (std::clamp)(
                desired - request.targetPanIndex, -2, 2);
        }
        result = Sub_52C680(
            static_cast<std::int32_t>(index), static_cast<std::int32_t>(isNew));
        request.requestCount = 0;
    }
    return result;
}

// 原 [0x52CCA0,0x52CCD1) 与独占尾块 [0x52CC40,0x52CC96)。
void Sub_52CCA0()
{
    if (gValue10507D4 != 0) {
        return;
    }

    for (SoundEffectDefinition& effect : gSoundEffects103E7B0) {
        if (effect.cooldown != 0) {
            --effect.cooldown;
        }
    }
    static_cast<void>(Sub_52CB50());
    static_cast<void>(UpdateBgmRequest_52C910());

    if (gBgmActive1050808 == 0) {
        gFlag = 0;
        return;
    }
    if (gFlag == 0) {
        return;
    }
    if (gQuotient != 0 && gSdlAudioBackend.HasBgm()) {
        --gQuotient;
        static_cast<void>(gSdlAudioBackend.SetBgmVolume(
            gSoundVolValue10505D0[static_cast<std::size_t>(gQuotient)]));
        return;
    }

    Sub_52B940();
    gRecordIndex = -1;
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
    const std::int32_t value00 = gRecords[static_cast<std::size_t>(index)].value40;
    const std::int32_t musicVolume = cg::platform::gMusicVolume;
    const std::uint32_t productBits =
        std::bit_cast<std::uint32_t>(value00)
        * std::bit_cast<std::uint32_t>(musicVolume);
    const std::int32_t product = std::bit_cast<std::int32_t>(productBits);
    gQuotient = product / 15;
    return productBits >> 31;
}

} // namespace cg::audio
