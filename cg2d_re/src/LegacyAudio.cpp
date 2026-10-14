#include "cg/LegacyAudio.hpp"

#include "LoginStateDependencies.hpp"
#include "network/NetworkDispatchDependencies.hpp"
#include "platform/MainLoopDependencies.hpp"
#include "platform/LegacyFormat.hpp"
#include "platform/LegacyAudioMath.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/ResourcePaths.hpp"
#include "cg/platform/SdlAudioBackend.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio> // 原格式化／扫描接口：0x0053B304、0x0053CADC；直接沿MSVC CRT。
#include <cstring>
#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <mmsystem.h>

namespace cg::login::detail {

// 原函数：0x00520750。
std::int32_t Sub_520750(std::int32_t selected);
// 原函数：0x00520200。
std::int32_t Sub_520200();
// 原函数：0x0052BE60。
std::int32_t LoadAndPlayBGM(std::uint32_t bgmId);

} // namespace cg::login::detail

namespace {

// 原 104DFD0 起的 BGM 播放记录；52B990 读取每项 +0x40。
struct LegacyAudioRecord final {
    char path[0x40];
    std::int32_t value40; // 原成员偏移：+0x40。
    std::uint8_t value44; // 原成员偏移：+0x44。
    std::uint8_t opaque45[3]; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
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
std::uint32_t gBgmActive1050808 = 0; // 原全局：0x01050808。 原播放状态 W32；52BE60 置位，52B940 清理。
std::uint32_t gBgmMode1050820 = 0; // 原全局：0x01050820。 原 BGM 流式模式 W32；52B8E0 消费。
std::atomic<std::uint8_t> gBgmStopNotice105081C {0}; // 原全局：0x0105081C。

// 内置预转换音乐的RCDATA标识，与platform/FixedMusic.rc.in一致。
constexpr int kFixedMusicResourceId = 1001;

// 原454C30为固定音乐生成的独立128槽，不能借普通WAV BGM的另一音量表。
std::array<std::int32_t, 128> gFixedMusicVolume62B0C8 {}; // 原全局：0x0062B0C8。
std::uint8_t gValue62B2DC = 0xFF; // 原全局：0x0062B2DC。 原加载W8=FFH，4553B0在实际播放前清0；不当作SDL通知。

static_assert(sizeof(gRecords) == 0x2600);

} // namespace

namespace cg::audio {

// 原 [57C368,57CB24)：handler只保留原地址证据编号，运行期对象槽改用非拥有T*。
struct SoundRecord57C368 final {
    std::uint32_t id;
    std::uint32_t handlerToken;
    std::uint32_t categoryBits;
    login::YobiAction* runtimeAction = nullptr;
    std::uint8_t dirty = 0;
    std::array<std::uint8_t, 3> reserved {};
};

static_assert(offsetof(SoundRecord57C368, id) == 0);
static_assert(offsetof(SoundRecord57C368, handlerToken) == 4);
static_assert(offsetof(SoundRecord57C368, categoryBits) == 8);
static_assert(offsetof(SoundRecord57C368, runtimeAction) == 16);
static_assert(offsetof(SoundRecord57C368, dirty) == 24);
static_assert(sizeof(SoundRecord57C368) == 32);

// 原 [104A3B8,104C9B8)：与 104E010 播放记录分离的 128 项配置表。
struct BgmConfigRecord final {
    char path[0x40];
    std::int32_t value40; // 原成员偏移：+0x40。
    std::uint8_t value44; // 原成员偏移：+0x44。
    std::uint8_t opaque45[3]; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
};

static_assert(offsetof(BgmConfigRecord, value40) == 0x40);
static_assert(offsetof(BgmConfigRecord, value44) == 0x44);
static_assert(offsetof(BgmConfigRecord, value48) == 0x48);
static_assert(sizeof(BgmConfigRecord) == 0x4C);

// 原 [0x104C9B8,0x104D0C0)：音效请求表，独立于空间坐标记录。
struct SoundRequestRecord final {
    std::int32_t channelIndex; // 原成员偏移：+0x00。
    std::int32_t seNo; // 原成员偏移：+0x04。
    std::int32_t requestCount; // 原成员偏移：+0x08。
    std::int32_t distanceOrAtten; // 原成员偏移：+0x0C。
    std::int32_t targetVolumeIndex; // 原成员偏移：+0x10。
    std::int32_t appliedVolumeIndex; // 原成员偏移：+0x14。
    std::int32_t screenX; // 原成员偏移：+0x18。
    std::int32_t targetPanIndex; // 原成员偏移：+0x1C。
    std::int32_t appliedPanIndex; // 原成员偏移：+0x20。
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
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t currentSeNo; // 原成员偏移：+0x04。
    std::array<std::byte, 0x28> opaque08; // 原成员偏移：+0x08。
    std::int32_t value30; // 原成员偏移：+0x30。
    std::array<std::byte, 0x08> opaque34; // 原成员偏移：+0x34。
};

static_assert(offsetof(SoundChannelState, value00) == 0x00);
static_assert(offsetof(SoundChannelState, currentSeNo) == 0x04);
static_assert(offsetof(SoundChannelState, value30) == 0x30);
static_assert(sizeof(SoundChannelState) == 0x3C);

// 原 [0x103E7B0,0x104A330)：+04/+1C 尚未定性，保留 W32 原位。
struct SoundEffectDefinition final {
    std::int32_t sampleIndex;
    std::uint32_t unknown04; // 原成员偏移：+0x04。
    std::int32_t loop;
    std::int32_t volume; // 原成员偏移：+0x0C。
    std::int32_t pitchIndex;
    std::int32_t cooldown; // 原成员偏移：+0x14。
    std::int32_t baseFrequency;
    std::uint32_t unknown1C; // 原成员偏移：+0x1C。
    char path[0x40]; // 原成员偏移：+0x20。
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

// 原52B710：同一RIFF父块内定位fmt/data，完整18B格式读取后才发布W32数据长度。
std::int32_t ReadPcmWaveHeader_52B710(
    const HMMIO input, WAVEFORMATEX* const format, std::uint32_t* const dataLength)
{
    static_assert(sizeof(MMCKINFO) == 20);
    static_assert(sizeof(WAVEFORMATEX) == 18);
    MMCKINFO riff {};
    MMCKINFO chunk = riff;
    riff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (::mmioDescend(input, &riff, nullptr, MMIO_FINDRIFF) != 0) {
        return 0;
    }
    chunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (::mmioDescend(input, &chunk, &riff, MMIO_FINDCHUNK) != 0) {
        return 0;
    }
    if (::mmioRead(input, reinterpret_cast<HPSTR>(format), 18) != 18) {
        return 0;
    }
    if (format->wFormatTag != WAVE_FORMAT_PCM) {
        return 0;
    }
    if (::mmioAscend(input, &chunk, 0) != 0) {
        return 0;
    }
    chunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (::mmioDescend(input, &chunk, &riff, MMIO_FINDCHUNK) != 0) {
        return 0;
    }
    *dataLength = chunk.cksize;
    return 1;
}

// 音效与BGM共用原头读取；仅探测文件，样本解码/缓冲与播放继续由SDL持有。
bool ReadPcmWaveDataLength(const char* const path, std::uint32_t& dataLength)
{
    const HMMIO input = ::mmioOpenA(const_cast<char*>(path), nullptr, MMIO_ALLOCBUF);
    if (input == nullptr) {
        return false;
    }
    WAVEFORMATEX format;
    const std::int32_t result = ReadPcmWaveHeader_52B710(input, &format, &dataLength);
    static_cast<void>(::mmioClose(input, 0));
    return result != 0;
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

std::array<SpatialAudioRecord, 50> gSpatialRecords103D1C0 {}; // 原全局：0x0103D1C0。
static_assert(sizeof(gSpatialRecords103D1C0) == 0x3E8);

std::array<std::int32_t, 512> gBgmMapping103DFAC {}; // 原全局：0x0103DFAC。
std::array<BgmConfigRecord, 128> gBgmConfigRecords104A3B8 {}; // 原全局：0x0104A3B8。
std::array<SoundRequestRecord, 50> gSoundRequests104C9B8 {}; // 原全局：0x0104C9B8。
std::array<SoundEffectDefinition, 500> gSoundEffects103E7B0 {}; // 原全局：0x0103E7B0。
std::array<SoundChannelState, 64> gSoundChannels104D0C8 {}; // 原全局：0x0104D0C8。
std::array<std::byte, 0x80> gAudioBytes103DDFC {}; // 原全局：0x0103DDFC。
std::array<std::byte, 0x80> gAudioBytes103DD7C {}; // 原全局：0x0103DD7C。
std::array<std::byte, 0x80> gAudioBytes104A334 {}; // 原全局：0x0104A334。
std::array<std::int32_t, 128> gPanVal103CFBC {}; // 原全局：0x0103CFBC。
std::array<std::int32_t, 128> gSoundVolValue10505D0 {}; // 原全局：0x010505D0。
// 原 [58D3D4,58D4F8)：52C450/52C680 以 baseFrequency+pitchIndex 读取。
std::array<std::uint32_t, 73> gPitchFrequencies58D3D4 {{ // 原全局：0x0058D3D4。
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
std::array<std::uint32_t, 36> gBgmFrequencies58D468 {{ // 原全局：0x0058D468。
    22050, 23360, 24750, 26221, 27780, 29432, 30929, 33037,
    35002, 37082, 39287, 41623, 44100, 46721, 49500, 52443,
    55561, 58865, 61859, 66074, 70004, 74165, 78575, 83247,
    88200, 93442, 99001, 104887, 111123, 117731, 123718, 132149,
    140008, 148331, 157150, 166495,
}};
std::int32_t gNextSeChannel105082C = 0; // 原全局：0x0105082C。
std::int32_t gLastSeNo58D4FC = -1; // 原全局：0x0058D4FC。
std::int32_t gSampleCount104DFCC = 0; // 原全局：0x0104DFCC。
std::atomic<std::uint32_t> gBgmReady105083C {0}; // 原全局：0x0105083C。
std::uint32_t gBgmLoopStart103CFB8 = 0; // 原全局：0x0103CFB8。
std::uint32_t gBgmLoopEnd103DEA4 = 0; // 原全局：0x0103DEA4。
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

std::array<SoundRecord57C368, 99> gSoundRecords57C368 {{ // 原全局：0x0057C368。
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
std::array<std::uint32_t, 3> gSoundSelectionCC236C {}; // 原全局：0x00CC236C。
static_assert(sizeof(gSoundRecords57C368) == 99 * sizeof(SoundRecord57C368));
static_assert(sizeof(gSoundSelectionCC236C) == 3 * sizeof(std::uint32_t));

// 原52C160的SDL创建适配：首段按完整PCM预填；smpl终点由52BE60在ready后发布。
std::int32_t LoadStreamingBgm_52C160(
    const char* const path, const std::uint32_t startFrame)
{
    if (!gSdlAudioBackend.LoadBgmWav(path, startFrame, 0)) {
        return 0;
    }
    gBgmLoopEnd103DEA4 = gSdlAudioBackend.ReadBgmDataLength();
    if (!gSdlAudioBackend.StartBgmStopWorker(
            gBgmStopNotice105081C, gBgmReady105083C)) {
        return 0;
    }
    std::uint32_t ready;
    while ((ready = gBgmReady105083C.load(std::memory_order_acquire)) == 0) {
    }
    if (ready != 1) {
        gSdlAudioBackend.ReleaseBgm();
        return 0;
    }
    return 1;
}

} // namespace

std::uint32_t gValue103DE80 = 0; // 原全局：0x0103DE80。
std::uint32_t gValue10507D4 = 0; // 原全局：0x010507D4。
std::int32_t gBgmTarget1050818 = 0; // 原全局：0x01050818。
std::int32_t gBgmRequest1050810 = 0; // 原全局：0x01050810。
std::int32_t gBgmStage1050814 = 0; // 原全局：0x01050814。
volatile std::uint32_t gMciDeviceId = 0; // 原全局：0x010507EC。
std::uint32_t gMciCloseResult = 0; // 原全局：0x010507FC。

// 原函数：0x0052B9E0。
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
        if (entry.id > 500) {
            gSdlAudioBackend.Close();
            return 0;
        }

        SoundEffectDefinition& effect =
            gSoundEffects103E7B0[static_cast<std::size_t>(entry.id)];
        effect.sampleIndex = gSampleCount104DFCC;
        char openPath[256] {};
        static_cast<void>(std::sprintf(
            openPath, "%s\\%s", static_cast<const char*>(resource::gBinPath),
            static_cast<const char*>(entry.path)));
        static_cast<void>(std::sprintf(
            effect.path, "%s\\%s", static_cast<const char*>(resource::gBinPath),
            static_cast<const char*>(entry.path)));
        // 原52B262先检查音量，再发布；音高失败仍保留此前样本/路径/音量。
        const std::int32_t volume = static_cast<std::int8_t>(entry.volume);
        if (volume < 0) {
            gSdlAudioBackend.Close();
            return 0;
        }
        effect.volume = volume;
        const std::int32_t pitch = static_cast<std::int8_t>(entry.pitch);
        if (pitch > 60) {
            gSdlAudioBackend.Close();
            return 0;
        }
        const std::int32_t loop = static_cast<std::int8_t>(entry.loop);
        effect.pitchIndex = pitch;
        effect.loop = loop;

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
        std::uint32_t dataLength;
        if (utf8Path.empty() || !ReadPcmWaveDataLength(openPath, dataLength)
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
        static_cast<void>(std::sprintf(
            record.path, "%s\\%s", static_cast<const char*>(resource::gBinPath),
            static_cast<const char*>(config.path)));
        // 原52B53D先持有三个值，再按40/44/48发布到同源BGM运行表。
        const std::int32_t value40 = config.value40;
        const std::uint8_t value44 = config.value44;
        const std::int32_t value48 = config.value48;
        record.value40 = value40;
        record.value44 = value44;
        record.value48 = value48;
    }
    return 1;
}

// 原函数：0x005241A0。
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

// 原函数：0x00524060。
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
    // 原var_B5 W8与紧邻var_B4 S32；53CADC的两个%d目标各写4B。
    std::array<std::uint8_t, 5> values44And48 {};
    char line[80] {};
    char path[80] {};

    while (std::fgets(line, sizeof(line), file) != nullptr) {
        const char first = line[0];
        if (first == '#' || first == ' ' || first == '\n' || first == '-') {
            continue;
        }

        static_cast<void>(std::sscanf(
            line, "%d %d %s %d %d %d",
            static_cast<void*>(&recordIndex), static_cast<void*>(&mappingIndex),
            static_cast<void*>(path), static_cast<void*>(&value40),
            static_cast<void*>(values44And48.data()),
            static_cast<void*>(values44And48.data() + 1)));

        const std::int32_t selectedRecordIndex = recordIndex;
        BgmConfigRecord& record =
            gBgmConfigRecords104A3B8[static_cast<std::size_t>(selectedRecordIndex)];
        const std::size_t bytes = std::strlen(path) + 1;
        const std::size_t wordBytes = bytes & ~std::size_t{3};
        std::memcpy(record.path, path, wordBytes);
        const std::uint8_t value44 = values44And48[0];
        std::memcpy(record.path + wordBytes, path + wordBytes, bytes & 3u);
        record.value40 = value40;
        std::int32_t value48;
        std::memcpy(&value48, values44And48.data() + 1, sizeof(value48));
        record.value44 = value44;
        record.value48 = value48;
        gBgmMapping103DFAC[static_cast<std::size_t>(mappingIndex)] =
            selectedRecordIndex;
    }

    std::fclose(file);
    return 1;
}

// 原函数：0x0052C9C0。
std::int32_t SelectBgmTarget_52C9C0(const std::int32_t index)
{
    const std::int32_t target =
        gBgmMapping103DFAC[static_cast<std::size_t>(index)];
    if (target >= 0) {
        gBgmTarget1050818 = target;
    }
    return 0;
}

// 原函数：0x0052C910。
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

login::YobiAction*& BorrowLegacyPanelAction57C374(const std::int32_t index) noexcept
{
    return gSoundRecords57C368[index].runtimeAction;
}

std::uint8_t& BorrowLegacyPanelDirty57C378(const std::int32_t index) noexcept
{
    return gSoundRecords57C368[index].dirty;
}

// 原函数：0x00504460。
std::int32_t Sub_504460(const std::uint32_t id)
{
    const std::int32_t index = Sub_5044C0(id);
    if (index < 0) {
        return -1;
    }
    if (gSoundRecords57C368[index].runtimeAction == nullptr) {
        static_cast<void>(Sub_504180(id));
        return 1;
    }
    static_cast<void>(Sub_5042D0(id));
    static_cast<void>(PlaySe_52C450(57, 320));
    return 0;
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

    const std::uint8_t categoryGate =
        static_cast<std::uint8_t>(record.categoryBits);
    const std::uint32_t previousSelection = gSoundSelectionCC236C[0];
    if (categoryGate == 2) {
        if (previousSelection == id) {
            return 0;
        }
        gSoundSelectionCC236C[0] = id;
        record.dirty = 1;
        gSoundSelectionCC236C[1] = 0;
        return 1;
    }

    const std::int32_t previousIndex = Sub_5044C0(previousSelection);
    if (previousIndex < 0) {
        return 0;
    }
    if (gSoundSelectionCC236C[0] != 0
        && static_cast<std::uint8_t>(
            gSoundRecords57C368[static_cast<std::size_t>(previousIndex)].categoryBits)
            == 2) {
        const std::int8_t category = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(record.categoryBits));
        gSoundSelectionCC236C[0] = 0;
        gSoundSelectionCC236C[1] = 0;
        gSoundSelectionCC236C[static_cast<std::size_t>(category)] = id;
        record.dirty = 1;
        return 1;
    }

    const std::int8_t category = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(record.categoryBits));
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

std::uint32_t ReadLegacyPanelSelectionCC236C() noexcept
{
    return gSoundSelectionCC236C[0];
}

std::uint32_t ReadLegacyPanelSelectionCC2370() noexcept
{
    return gSoundSelectionCC236C[1];
}

// 原函数：0x0052CCE0。
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

// 原函数：0x0052CD40。
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

    platform::Cg2dInitializeSoundTablesX87(gPanVal103CFBC.data(), gSoundVolValue10505D0.data());
    return Sub_52CD40();
}

// 原 [0x52CDE0,0x52CE97)；在同一空间音频表发布更近的屏幕坐标与距离。
std::int32_t __cdecl Sub_52CDE0(
    const std::int32_t index,
    const std::int32_t sourceX,
    const std::int32_t sourceY)
{
    float worldX;
    float worldY;
    platform::Cg2dScaleSpatialSourceX87(sourceX, sourceY, &worldX, &worldY);
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);
    return platform::Cg2dPublishSpatialProjectionX87(
        gSpatialRecords103D1C0.data(), index, &screenX, &screenY);
}

// 原函数：0x0052C9E0。
std::int32_t __cdecl Sub_52C9E0(
    const std::int32_t id, const std::int32_t x, const std::int32_t y)
{
    if (id < 250 || id > 299) {
        return 0;
    }

    std::int32_t distance;
    const std::int32_t quotient =
        platform::Cg2dComputeSpatialSoundX87(x, y, &distance);
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
    // 原52C58F先取当前通道，再取音量；各属性调用后重新取通道。
    const std::int32_t volumeChannel = gNextSeChannel105082C;
    const std::int32_t effectVolume = effect.volume;
    const std::int32_t scaledVolume = (effectVolume * 127) / 127;
    const std::int32_t volumeIndex =
        (scaledVolume * platform::gEffectVolume) / 15;
    static_cast<void>(gSdlAudioBackend.SetChannelVolume(
        static_cast<std::size_t>(volumeChannel),
        gSoundVolValue10505D0[static_cast<std::size_t>(volumeIndex)]));
    const std::int32_t panChannel = gNextSeChannel105082C;
    const std::int32_t panValue =
        gPanVal103CFBC[static_cast<std::size_t>(panIndex)];
    static_cast<void>(gSdlAudioBackend.SetChannelPan(
        static_cast<std::size_t>(panChannel), panValue));
    // 原52C601只读一次音高，非零时按基频→通道→频率表取样。
    const std::int32_t pitchIndex = effect.pitchIndex;
    if (pitchIndex != 0) {
        const std::int32_t baseFrequency = effect.baseFrequency;
        const std::int32_t frequencyChannel = gNextSeChannel105082C;
        const std::uint32_t frequency =
            gPitchFrequencies58D3D4[static_cast<std::size_t>(
                baseFrequency + pitchIndex)];
        static_cast<void>(gSdlAudioBackend.SetChannelFrequency(
            static_cast<std::size_t>(frequencyChannel), frequency));
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

    const std::int32_t targetVolumeIndex = request.targetVolumeIndex;
    const std::int32_t appliedVolumeIndex = request.appliedVolumeIndex;
    if (targetVolumeIndex != appliedVolumeIndex) {
        const std::int32_t volumeChannel = request.channelIndex;
        const std::int32_t volumeValue =
            gSoundVolValue10505D0[static_cast<std::size_t>(targetVolumeIndex)];
        static_cast<void>(gSdlAudioBackend.SetChannelVolume(
            static_cast<std::size_t>(volumeChannel), volumeValue));
        // 原52C7F4在属性调用后重新读目标，再发布已应用缓存。
        request.appliedVolumeIndex = request.targetVolumeIndex;
    }
    const std::int32_t targetPanIndex = request.targetPanIndex;
    const std::int32_t appliedPanIndex = request.appliedPanIndex;
    if (targetPanIndex != appliedPanIndex) {
        const std::int32_t panChannel = request.channelIndex;
        const std::int32_t panValue =
            gPanVal103CFBC[static_cast<std::size_t>(targetPanIndex)];
        static_cast<void>(gSdlAudioBackend.SetChannelPan(
            static_cast<std::size_t>(panChannel), panValue));
        request.appliedPanIndex = request.targetPanIndex;
    }

    const std::int32_t currentSeNo = request.seNo;
    const std::int32_t pitch =
        gSoundEffects103E7B0[static_cast<std::size_t>(currentSeNo)].pitchIndex;
    if (pitch != 0) {
        const std::int32_t base =
            gSoundEffects103E7B0[static_cast<std::size_t>(
                currentSeNo)].baseFrequency;
        const std::int32_t frequencyChannel = request.channelIndex;
        const std::uint32_t frequency =
            gPitchFrequencies58D3D4[static_cast<std::size_t>(base + pitch)];
        static_cast<void>(gSdlAudioBackend.SetChannelFrequency(
            static_cast<std::size_t>(frequencyChannel), frequency));
    }

    if (isNew != 0) {
        static_cast<void>(gSdlAudioBackend.RewindChannel(
            static_cast<std::size_t>(request.channelIndex)));
        const bool loop =
            gSoundEffects103E7B0[static_cast<std::size_t>(request.seNo)].loop != 0;
        const std::int32_t playChannel = request.channelIndex;
        if (loop) {
            gSoundChannels104D0C8[static_cast<std::size_t>(
                playChannel)].value30 = 1;
        }
        static_cast<void>(gSdlAudioBackend.PlayChannel(
            static_cast<std::size_t>(playChannel), loop));

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

        std::int32_t desired;
        if (platform::gUseStereo == 0) {
            desired = 64;
            // 原52CBCA先归中；现有请求也立即消费64，不再渐变。
            request.targetPanIndex = desired;
        } else {
            desired = (std::clamp)(request.screenX / 5, 0, 127);
        }
        if (isNew) {
            request.targetPanIndex = desired;
        } else {
            const std::int32_t previousPanIndex = request.targetPanIndex;
            request.targetPanIndex = previousPanIndex + (std::clamp)(
                desired - previousPanIndex, -2, 2);
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
        const std::int32_t quotient =
            platform::Cg2dComputeSpatialScanQuotientX87(x, y);
        if (quotient >= 127 || quotient < 20) {
            return;
        }

        static_cast<void>(Sub_52C9E0(id + 249, x, y));
        record.value00 = 1;
    }
}

std::int32_t ReadBgmRecordIndex58D4F8() noexcept
{
    return gRecordIndex;
}

// 原函数：0x0052B990。
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

namespace cg::login::detail {

// 原 World::LoadAndPlayBGM@[52BE60,52C15C)；仅原生缓冲与播放机制映射 SDL3。
std::int32_t LoadAndPlayBGM(const std::uint32_t bgmId)
{
    if (std::bit_cast<std::uint32_t>(gRecordIndex) == bgmId && gFlag == 0) {
        return 1;
    }
    gFlag = 0;
    if (audio::gValue10507D4 != 0) {
        return -1;
    }
    if (gRecordIndex != -1) {
        audio::Sub_52B8E0();
    }
    if (bgmId == 0xFFFFFFFEu) {
        gRecordIndex = -2;
        return -1;
    }
    if (bgmId >= 128 || platform::gEnableMusic == 0) {
        return -1;
    }

    const LegacyAudioRecord& record =
        gRecords[static_cast<std::size_t>(bgmId)];
    std::uint32_t dataLength = 0;
    if (!audio::ReadPcmWaveDataLength(record.path, dataLength)) {
        return 0;
    }
    const std::string path = audio::AudioPathToUtf8(record.path);
    if (record.value44 == 2) {
        gBgmMode1050820 = 1;
        std::FILE* const file = std::fopen(record.path, "rb");
        if (file == nullptr) {
            return 0;
        }
        static_cast<void>(std::fseek(file, 32, SEEK_CUR));
        const std::uint32_t blockAlign =
            static_cast<std::uint32_t>(std::fgetc(file))
            | static_cast<std::uint32_t>(std::fgetc(file)) << 8;
        const long loopOffset = static_cast<long>(
            dataLength + 14u + (dataLength & 1u) + 48u);
        static_cast<void>(std::fseek(file, loopOffset, SEEK_CUR));
        std::uint32_t startFrame = 0;
        std::uint32_t endFrame = 0;
        for (int shift = 0; shift < 32; shift += 8) {
            startFrame |= static_cast<std::uint32_t>(std::fgetc(file)) << shift;
        }
        for (int shift = 0; shift < 32; shift += 8) {
            endFrame |= static_cast<std::uint32_t>(std::fgetc(file)) << shift;
        }
        std::fclose(file);
        audio::gBgmLoopStart103CFB8 = blockAlign * startFrame;
        if (audio::LoadStreamingBgm_52C160(path.c_str(), startFrame) == 0) {
            return 0;
        }
        audio::gBgmLoopEnd103DEA4 = blockAlign * endFrame;
        static_cast<void>(audio::gSdlAudioBackend.SetBgmLoopEnd(endFrame));
    } else {
        gBgmMode1050820 = 0;
        if (!audio::gSdlAudioBackend.LoadBgmWav(path.c_str(), 0, 0)) {
            return 0;
        }
    }

    const std::int8_t frequencyIndex =
        platform::ReadBgmFrequencySelectionByte(
            static_cast<std::uint8_t>(bgmId));
    static_cast<void>(audio::gSdlAudioBackend.SetBgmFrequency(
        audio::gBgmFrequencies58D468[
            static_cast<std::size_t>(frequencyIndex)]));
    const std::int32_t volumeIndex =
        (platform::gMusicVolume * record.value40) / 15;
    static_cast<void>(audio::gSdlAudioBackend.SetBgmVolume(
        audio::gSoundVolValue10505D0[static_cast<std::size_t>(volumeIndex)]));
    static_cast<void>(audio::gSdlAudioBackend.PlayBgm(record.value44 != 0));
    gRecordIndex = static_cast<std::int32_t>(bgmId);
    platform::gLastRenderDt =
        static_cast<double>(platform::ReadTickCount32());
    gBgmActive1050808 = 1;
    return 1;
}

} // namespace cg::login::detail

namespace cg::audio {

// 原52C3D0：先失败标志/当前号/enable，再原0..15钳制；实际缓冲不存在透传level。
std::int32_t Sub_52C3D0()
{
    std::int32_t result = std::bit_cast<std::int32_t>(gValue10507D4);
    if (result == 0 && gRecordIndex != -1) {
        result = platform::gEnableMusic;
        if (result != 0) {
            result = platform::gMusicVolume;
            if (result > 15) { result = 15; platform::gMusicVolume = result; }
            else if (result < 0) { result = 0; platform::gMusicVolume = result; }
            if (gSdlAudioBackend.HasBgm()) {
                const auto index = result * gRecords[static_cast<std::size_t>(gRecordIndex)].value40 / 15;
                return gSdlAudioBackend.SetBgmVolume(gSoundVolValue10505D0[static_cast<std::size_t>(index)])
                    ? 0 : std::bit_cast<std::int32_t>(0x80004005u);
            }
        }
    }
    return result;
}

// 原455030：两个S32生成Waves四F32参数，效果失败不改变父的加载/播放返回。
void SetFixedMusicReverb_455030(const std::int32_t decay, const std::int32_t mix)
{
    static_cast<void>(gSdlAudioBackend.SetFixedMusicReverb(decay, mix));
}

// 原455400：按同源音乐音量缩放，只在原0..127域应用独立固定音乐表。
void ApplyFixedMusicVolume_455400(const std::int32_t scale)
{
    const std::int32_t index = scale * platform::gMusicVolume / 15;
    if (static_cast<std::uint32_t>(index) < 128u) {
        static_cast<void>(gSdlAudioBackend.SetFixedMusicVolume(
            gFixedMusicVolume62B0C8[static_cast<std::size_t>(index)]));
    }
}

// 原455450：不存在返回0；实际停止后不消费后端失败，返回1。
std::int32_t StopFixedMusic_455450()
{
    if (!gSdlAudioBackend.HasFixedMusic()) { return 0; }
    gSdlAudioBackend.StopFixedMusic();
    return 1;
}

// 原455470：仅当前已准备片段接重复值，原本体不消费设置HRESULT。
std::int32_t SetFixedMusicRepeats_455470(const std::uint32_t repeats)
{
    if (!gSdlAudioBackend.HasFixedMusicPlayback()) { return 0; }
    static_cast<void>(gSdlAudioBackend.SetFixedMusicRepeats(repeats));
    return 1;
}

// 原4553B0：两存在门槛→清原W8→音量→实际播放结果；父455280不消费该返回。
std::int32_t PlayFixedMusicSegment_4553B0()
{
    if (!gSdlAudioBackend.HasFixedMusic()) { return 0; }
    if (!gSdlAudioBackend.HasFixedMusicPlayback()) { return 0; }
    gValue62B2DC = 0;
    ApplyFixedMusicVolume_455400(127);
    return gSdlAudioBackend.PlayFixedMusic() ? 1 : 0;
}

// 原455280：加载失败0；加载成功后设置重复、调用播放入口，原父不消费两返回。
std::int32_t PlayFixedMusic_455280(const std::int32_t type, const std::uint32_t repeats)
{
    if (!gSdlAudioBackend.HasFixedMusic()) { return 0; }
    static_cast<void>(StopFixedMusic_455450());
    gSdlAudioBackend.ReleaseFixedMusicPlayback();
    // 用户将原16B片段指定为空音频：停止旧播放后立即成功，不产生零时长循环。
    if (type == 1) { return 1; }
    if (type != 0) { return 0; }
    if (!gSdlAudioBackend.PrepareFixedMusic()) { return 0; }
    SetFixedMusicReverb_455030(30, 7);
    static_cast<void>(SetFixedMusicRepeats_455470(repeats));
    static_cast<void>(PlayFixedMusicSegment_4553B0());
    return 1;
}

} // namespace cg::audio

namespace cg::network::detail {

// 原50E680只取socket后的type/repeats；第三已解析字段不被原本体读取。
void Sub_50E680(std::uintptr_t, std::int32_t type, std::int32_t repeats, std::int32_t)
{
    if (type < 1000) { static_cast<void>(audio::PlaySe_52C450(type, repeats)); }
    else { static_cast<void>(audio::PlayFixedMusic_455280(type - 1000, std::bit_cast<std::uint32_t>(repeats))); }
}

} // namespace cg::network::detail

namespace cg::platform {

// 原458AE0的DX设备检查改为真实SDL音频设备；独立DINPUT入口观察保留。
std::int32_t Sub_458AE0()
{
    SdlAudioBackend probe;
    if (!audio::gSdlAudioBackend.IsOpen() && !probe.Open()) {
        ::OutputDebugStringA("SDL audio device is unavailable\n"); return 0;
    }
    HMODULE input = ::LoadLibraryA("DINPUT.DLL");
    if (input == nullptr) { ::OutputDebugStringA("Couldn't LoadLibrary DInput\r\n"); return 0x200; }
    const bool available = ::GetProcAddress(input, "DirectInputCreateA") != nullptr;
    ::FreeLibrary(input);
    if (!available) { ::OutputDebugStringA("Couldn't GetProcAddress DInputCreate\r\n"); return 0x200; }
    return 0x600;
}

namespace shutdown_detail {
// 原函数：0x00454EE0。
std::int32_t Sub_454EE0()
{
    audio::gSdlAudioBackend.StopFixedMusic();
    audio::gSdlAudioBackend.ShutdownFixedMusic();
    // WAV初始化失败时，SDL设备只由音乐持有；正常WAV owner仍由52B630释放。
    if (audio::gValue10507D4 != 0) { audio::gSdlAudioBackend.Close(); }
    return 0;
}
} // namespace shutdown_detail

namespace initialization_detail {

// 原函数：0x00454C30。
std::int32_t InitializeMusic_454C30(std::uintptr_t)
{
    if (CheckLegacyDxCapability_458DC0() != 0) { return 1; }
    static_cast<void>(shutdown_detail::Sub_454EE0());
    if (!audio::gSdlAudioBackend.IsOpen() && !audio::gSdlAudioBackend.Open()) { return 1; }
    // 用户要求预转换为WAV；从本客户端内置资源载入，不再访问系统音色或解析MIDI。
    const HMODULE module = ::GetModuleHandleW(nullptr);
    const HRSRC resource = ::FindResourceW(module, MAKEINTRESOURCEW(kFixedMusicResourceId), MAKEINTRESOURCEW(10));
    const HGLOBAL loaded = resource != nullptr ? ::LoadResource(module, resource) : nullptr;
    const void* wav = loaded != nullptr ? ::LockResource(loaded) : nullptr;
    const DWORD size = resource != nullptr ? ::SizeofResource(module, resource) : 0;
    if (wav == nullptr || size == 0 || !audio::gSdlAudioBackend.InitializeFixedMusic(
            {static_cast<const std::uint8_t*>(wav), static_cast<std::size_t>(size)})) { return 2; }
    Cg2dInitializeFixedMusicVolumeX87(gFixedMusicVolume62B0C8.data());
    audio::SetFixedMusicReverb_455030(25, 6);
    return 0;
}

} // namespace initialization_detail
} // namespace cg::platform
