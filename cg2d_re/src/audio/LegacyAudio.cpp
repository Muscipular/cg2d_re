#include "../scene/login/LoginSavedState.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/audio/FileBgm.hpp"

#include "../scene/login/LoginStateDependencies.hpp"
#include "../network/NetworkDispatchDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "../platform/LegacyAudioMath.hpp"

#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/session/ClientHost.hpp"
#include "cg/core/m_log.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/resource/ResourcePaths.hpp"
#include "cg/platform/SdlAudioBackend.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio> // 原格式化／扫描接口：0x0053B304、0x0053CADC；直接沿MSVC CRT。
#include <format>
#include <string>

#include <Windows.h>
#include <mmsystem.h>

namespace cg::audio {

// 原 104DFD0 起的 BGM 播放记录；52B990 读取每项 +0x40。
// 路径由原生UTF-16字符串拥有；原偏移仅供追溯，不沿用旧字节布局。
struct LegacyAudioRecord final {
    std::wstring path;
    std::int32_t value40; // 原成员偏移：+0x40。
    std::uint8_t value44; // 原成员偏移：+0x44。
    std::uint8_t opaque45[3]; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
};

static constexpr std::uint32_t kRecordCount = 128;

static std::array<LegacyAudioRecord, kRecordCount> gRecords {};

// 内置预转换音乐的RCDATA标识，与platform/FixedMusic.rc.in一致。
static constexpr int kFixedMusicResourceId = 1001;

// 原454C30为固定音乐生成的独立128槽，不能借普通WAV BGM的另一音量表。
static std::array<std::int32_t, 128> gFixedMusicVolume62B0C8 {}; // 原全局：0x0062B0C8。

// 原 [104A3B8,104C9B8)：与 104E010 播放记录分离的 128 项配置表。
// 配置路径读入后即转为UTF-16，数值字段仍保持原宽度。
struct BgmConfigRecord final {
    std::wstring path;
    std::int32_t value40; // 原成员偏移：+0x40。
    std::uint8_t value44; // 原成员偏移：+0x44。
    std::uint8_t opaque45[3]; // 原成员偏移：+0x45。
    std::int32_t value48; // 原成员偏移：+0x48。
};

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

// 原 [0x103E7B0,0x104A330)：+04/+1C 尚未定性，保留 W32 数值字段。
// 路径采用std::wstring；原成员偏移为历史定位，不承诺整项字节布局。
struct SoundEffectDefinition final {
    std::int32_t sampleIndex;
    std::uint32_t unknown04; // 原成员偏移：+0x04。
    std::int32_t loop;
    std::int32_t volume; // 原成员偏移：+0x0C。
    std::int32_t pitchIndex;
    std::int32_t cooldown; // 原成员偏移：+0x14。
    std::int32_t baseFrequency;
    std::uint32_t unknown1C; // 原成员偏移：+0x1C。
    std::wstring path; // 原成员偏移：+0x20。
};

// 524230 的内联 0x46 字节栈项；最后一个未写填充字节不参与加载。
struct SeInitEntry final {
    std::int16_t id;
    const wchar_t* path;
    std::uint8_t volume;
    std::uint8_t pitch;
    std::uint8_t loop;
};

static constexpr std::array<SeInitEntry, 270> kSeInit524230 {{
#include "LegacySeDefinitions.inc"
}};

// 原52B710：同一RIFF父块内定位fmt/data，完整18B格式读取后才发布W32数据长度。
static std::int32_t ReadPcmWaveHeader_52B710(
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
static bool ReadPcmWaveDataLength(const wchar_t* const path, std::uint32_t& dataLength)
{
    const HMMIO input = ::mmioOpenW(const_cast<wchar_t*>(path), nullptr, MMIO_ALLOCBUF);
    if (input == nullptr) {
        return false;
    }
    WAVEFORMATEX format;
    const std::int32_t result = ReadPcmWaveHeader_52B710(input, &format, &dataLength);
    static_cast<void>(::mmioClose(input, 0));
    return result != 0;
}

// 仅在旧目录参数及配置路径读入边界，沿既有CP_ACP解释转换为UTF-16。
static std::wstring AudioPathToWide(const char* const path)
{
    const int wideLength =
        ::MultiByteToWideChar(CP_ACP, 0, path, -1, nullptr, 0);
    if (wideLength <= 0) {
        return L"";
    }
    std::wstring wide(static_cast<std::size_t>(wideLength), L'\0');
    if (::MultiByteToWideChar(
            CP_ACP, 0, path, -1, wide.data(), wideLength) == 0) {
        return L"";
    }
    wide.pop_back();
    return wide;
}

static std::array<std::int32_t, 512> gBgmMapping103DFAC {}; // 原全局：0x0103DFAC。
static std::array<BgmConfigRecord, 128> gBgmConfigRecords104A3B8 {}; // 原全局：0x0104A3B8。
static std::array<SoundEffectDefinition, 500> gSoundEffects103E7B0 {}; // 原全局：0x0103E7B0。
static std::array<SoundChannelState, 64> gSoundChannels104D0C8 {}; // 原全局：0x0104D0C8。
static std::array<std::byte, 0x80> gAudioBytes103DDFC {}; // 原全局：0x0103DDFC。
static std::array<std::byte, 0x80> gAudioBytes103DD7C {}; // 原全局：0x0103DD7C。
static std::array<std::byte, 0x80> gAudioBytes104A334 {}; // 原全局：0x0104A334。
static std::array<std::int32_t, 128> gPanVal103CFBC {}; // 原全局：0x0103CFBC。
static std::array<std::int32_t, 128> gSoundVolValue10505D0 {}; // 原全局：0x010505D0。
// 原 [58D3D4,58D4F8)：52C450/52C680 以 baseFrequency+pitchIndex 读取。
static std::array<std::uint32_t, 73> gPitchFrequencies58D3D4 {{ // 原全局：0x0058D3D4。
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
static std::array<std::uint32_t, 36> gBgmFrequencies58D468 {{ // 原全局：0x0058D468。
    22050, 23360, 24750, 26221, 27780, 29432, 30929, 33037,
    35002, 37082, 39287, 41623, 44100, 46721, 49500, 52443,
    55561, 58865, 61859, 66074, 70004, 74165, 78575, 83247,
    88200, 93442, 99001, 104887, 111123, 117731, 123718, 132149,
    140008, 148331, 157150, 166495,
}};
static std::int32_t gNextSeChannel105082C = 0; // 原全局：0x0105082C。
static std::int32_t gLastSeNo58D4FC = -1; // 原全局：0x0058D4FC。
static std::int32_t gSampleCount104DFCC = 0; // 原全局：0x0104DFCC。
static platform::SdlAudioBackend gSdlAudioBackend;
static_assert(sizeof(gBgmMapping103DFAC) == 0x800);
static_assert(sizeof(gSoundChannels104D0C8) == 0xF00);
static_assert(sizeof(gPanVal103CFBC) == 0x200);
static_assert(sizeof(gSoundVolValue10505D0) == 0x200);
static_assert(sizeof(gPitchFrequencies58D3D4) == 73 * sizeof(std::uint32_t));
static_assert(sizeof(gBgmFrequencies58D468) == 0x90);

// 新增输出选择失败只释放对应真实流；停止线程退出后才清其握手量。
static bool SetSessionBgmOutput(session::ClientSession& session, const bool enabled) noexcept
{
    auto& state = session.Audio();
    if (gSdlAudioBackend.SetBgmOutputEnabled(state.bgmPlayback, enabled)) return true;
    m_log_iw(L"[BGM] 输出切换失败，停止对应场景音乐\n");
    gSdlAudioBackend.ReleaseBgm(state.bgmPlayback);
    state.bgmMode = 0;
    state.bgmActive = 0;
    state.bgmFadeOutActive = 0;
    state.bgmStage = 0;
    state.bgmRecordIndex = -1;
    state.bgmStopNotice.store(0, std::memory_order_release);
    state.bgmReady.store(0, std::memory_order_release);
    return false;
}

// 原52C160的SDL创建适配：首段按完整PCM预填；smpl终点由52BE60在ready后发布。
static std::int32_t LoadStreamingBgm_52C160(
    session::ClientSession& session, const wchar_t* const path, const std::uint32_t startFrame)
{
    if (!gSdlAudioBackend.LoadBgmWav(session.Audio().bgmPlayback, path, startFrame, 0)) {
        return 0;
    }
    session.Audio().bgmLoopEnd = gSdlAudioBackend.ReadBgmDataLength(session.Audio().bgmPlayback);
    if (!gSdlAudioBackend.StartBgmStopWorker(
            session.Audio().bgmPlayback, session.Audio().bgmStopNotice, session.Audio().bgmReady)) {
        return 0;
    }
    std::uint32_t ready;
    while ((ready = session.Audio().bgmReady.load(std::memory_order_acquire)) == 0) {
    }
    if (ready != 1) {
        gSdlAudioBackend.ReleaseBgm(session.Audio().bgmPlayback);
        return 0;
    }
    return 1;
}

std::uint32_t gSkipMciDeviceClose_103DE80 = 0; // 原全局：0x0103DE80。
std::uint32_t gAudioDisabled_10507D4 = 0; // 原全局：0x010507D4。
volatile std::uint32_t gMciDeviceId = 0; // 原全局：0x010507EC。
std::uint32_t gMciCloseResult = 0; // 原全局：0x010507FC。

// 原函数：0x0052B9E0。
SpatialAudioRecord* __cdecl InitializeSoundRuntimeState_52B9E0(session::ClientSession& session); // 进程启动时初始化通道、首会话请求与运行表。
// 原 [52B8E0,52B93F)：通知由独立 SDL 工作线程生产，未启动时不伪造完成。
void StopAndReleaseStreamingBgm_52B8E0(session::ClientSession& session)
{
    if (session.Audio().bgmMode == 0 || !gSdlAudioBackend.HasBgm(session.Audio().bgmPlayback)) {
        return;
    }

    static_cast<void>(gSdlAudioBackend.StopBgm(session.Audio().bgmPlayback));
    if (!gSdlAudioBackend.RequestBgmStop(session.Audio().bgmPlayback)) {
        return;
    }
    while (session.Audio().bgmStopNotice.load(std::memory_order_acquire) != 1) {
    }
    session.Audio().bgmMode = 0;
    session.Audio().bgmStopNotice.store(0, std::memory_order_release);
    gSdlAudioBackend.ReleaseBgm(session.Audio().bgmPlayback);
}

// 原 [0x52B940,0x52B98E)：两路资源清理后依次清淡出、播放和所属请求阶段。
void ResetBgmPlayback_52B940(session::ClientSession& session)
{
    if (session.Audio().bgmMode != 0) {
        StopAndReleaseStreamingBgm_52B8E0(session);
    } else if (gSdlAudioBackend.HasBgm(session.Audio().bgmPlayback)) {
        gSdlAudioBackend.ReleaseBgm(session.Audio().bgmPlayback);
    }
    session.Audio().bgmFadeOutActive = 0;
    session.Audio().bgmActive = 0;
    session.Audio().bgmStage = 0;
}

// 原 [0x52B630,0x52B709)：保留禁用门禁与 BGM、通道、样本、设备的释放次序。
void ShutdownAudio_52B630()
{
    ShutdownAllFileBgm();
    if (gAudioDisabled_10507D4 != 0) {
        return;
    }

    for (auto& entry : cg::session::GetClientHost().Sessions()) {
        auto& session = entry.session;
        StopAndReleaseStreamingBgm_52B8E0(session);
        if (gSdlAudioBackend.HasBgm(session.Audio().bgmPlayback)) {
            gSdlAudioBackend.ReleaseBgm(session.Audio().bgmPlayback);
        }
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
std::int32_t InitializeAudioDeviceAndSamples_524230()
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
        const std::wstring openPath = std::format(
            L"{}\\{}", resource::gBinPath, entry.path);
        effect.path = std::format(
            L"{}\\{}", resource::gBinPath, entry.path);
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

        std::FILE* const file = ::_wfopen(openPath.c_str(), L"rb");
        if (file == nullptr) {
            continue;
        }
        static_cast<void>(::_fseeki64(file, 24, SEEK_SET));
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

        std::uint32_t dataLength;
        if (openPath.empty() || !ReadPcmWaveDataLength(openPath.c_str(), dataLength)
            || !gSdlAudioBackend.LoadWav(
                static_cast<std::size_t>(gSampleCount104DFCC),
                openPath.c_str())) {
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
        record.path = std::format(
            L"{}\\{}", resource::gBinPath, config.path);
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
std::int32_t InitializeAudio_5241A0(session::ClientSession& session)
{
    if ((platform::gRendererFlags & 0x20u) != 0) {
        gAudioDisabled_10507D4 = 1;
        return 0;
    }

    static_cast<void>(LoadBgmConfig_524060());
    if ((static_cast<std::uint16_t>(::GetAsyncKeyState(VK_SHIFT)) & 0x8000u) != 0) {
        gAudioDisabled_10507D4 = 1;
        return 0;
    }

    static_cast<void>(login::detail::AppendSystemLog_44B5C0(L"\tinitDSound() Start"));
    const std::int32_t result = InitializeAudioDeviceAndSamples_524230();
    if (result == 0) {
        gAudioDisabled_10507D4 = 1;
        return result;
    }

    static_cast<void>(login::detail::AppendSystemLog_44B5C0(L"\tmusic_init() Start"));
    static_cast<void>(InitializeSoundRuntimeState_52B9E0(session));
    gAudioDisabled_10507D4 = 0;
    static_cast<void>(login::detail::AppendSystemLog_44B5C0(L"t_music_init OK"));
    return 1;
}

// 原函数：0x00524060。
std::int32_t LoadBgmConfig_524060()
{
    std::FILE* const file = ::_wfopen(L"bin\\bgm\\bgm.cf", L"r");
    if (file == nullptr) {
        return 0;
    }

    gBgmMapping103DFAC.fill(-1);
    std::int32_t recordIndex = 0;
    std::int32_t mappingIndex = 0;
    std::int32_t value40 = 0;
    // 末两列独立读取；缺失字段沿用旧值，第五列写入时再收窄为U8。
    int value44 = 0;
    int value48 = 0;
    char line[80] {};
    char path[80] {};

    while (std::fgets(line, sizeof(line), file) != nullptr) {
        const char first = line[0];
        if (first == '#' || first == ' ' || first == '\n' || first == '-') {
            continue;
        }

        std::sscanf(
            line, "%d %d %s %d %d %d",
            &recordIndex, &mappingIndex, path, &value40, &value44, &value48);

        const std::int32_t selectedRecordIndex = recordIndex;
        BgmConfigRecord& record =
            gBgmConfigRecords104A3B8[static_cast<std::size_t>(selectedRecordIndex)];
        record.path = AudioPathToWide(path);
        record.value40 = value40;
        record.value44 = static_cast<std::uint8_t>(value44);
        record.value48 = value48;
        gBgmMapping103DFAC[static_cast<std::size_t>(mappingIndex)] =
            selectedRecordIndex;
    }

    std::fclose(file);
    return 1;
}

std::int32_t LoadMappedBgm(session::ClientSession& session, const std::int32_t index)
{
    if (index < 0 || index >= static_cast<std::int32_t>(gBgmMapping103DFAC.size())) {
        return -1;
    }
    const auto target = gBgmMapping103DFAC[static_cast<std::size_t>(index)];
    return target >= 0 && target < 128 ? LoadAndPlayBGM(session, target) : -1;
}

// 原函数：0x0052C9C0。
std::int32_t SelectBgmTarget_52C9C0(session::ClientSession& session, const std::int32_t index)
{
    const std::int32_t target =
        gBgmMapping103DFAC[static_cast<std::size_t>(index)];
    if (target >= 0) {
        session.Audio().bgmTarget = target;
    }
    return 0;
}

// 原函数：0x0052C910。
std::int32_t UpdateBgmRequest_52C910(session::ClientSession& session)
{
    const std::int32_t stage = session.Audio().bgmStage;
    switch (stage) {
    case 0: {
        const std::int32_t request = session.Audio().bgmRequest;
        std::int32_t result = request;
        if (request != 0) {
            const std::int32_t current = session.Audio().bgmRecordIndex;
            const std::int32_t target = session.Audio().bgmTarget;
            result = current;
            if (current != target) {
                const volatile std::int16_t& selected =
                    session.Connection().LoginRequests().selectedCharacterIndex;
                const std::int32_t selectedValue = selected;
                session.Audio().bgmStage = 1;
                static_cast<void>(login::detail::CaptureCharacterBgmTarget_520750(session, selectedValue));
                result = login::detail::SaveCurrentLoginRecord_520200(session);
            }
        }
        session.Audio().bgmRequest = 0;
        return result;
    }
    case 1: {
        const std::uint32_t result = BeginBgmFadeOut_52B990(session);
        ++session.Audio().bgmStage;
        return std::bit_cast<std::int32_t>(result);
    }
    case 2: {
        const std::int32_t flag = session.Audio().bgmFadeOutActive;
        if (flag == 0) {
            session.Audio().bgmStage = 3;
        }
        return flag;
    }
    case 3: {
        const std::int32_t target = session.Audio().bgmTarget;
        const std::int32_t result = LoadAndPlayBGM(
            session, std::bit_cast<std::uint32_t>(target));
        session.Audio().bgmStage = 0;
        return result;
    }
    default:
        return stage;
    }
}

// 原函数：0x0052CCE0。
std::uint32_t StopAndCloseMciDevice_52CCE0() noexcept
{
    if (gSkipMciDeviceClose_103DE80 == 1) {
        return 1;
    }

    mciSendCommandA(gMciDeviceId, MCI_STOP, 0, 0);
    const std::uint32_t result = mciSendCommandA(gMciDeviceId, MCI_CLOSE, 0, 0);
    gMciCloseResult = result;
    return (result & 0xFFFFFF00u) | static_cast<std::uint32_t>(result == 0);
}

// 原函数：0x0052CD40。
SpatialAudioRecord* __cdecl ResetSpatialSoundSources_52CD40(session::ClientSession& session)
{
    for (auto& record : session.Audio().spatialSources) {
        record.value04 = -1;
        record.value00 = 1;
    }
    return session.Audio().spatialSources.data() + session.Audio().spatialSources.size();
}

// 原 [0x52B9E0,0x52BBD6)：按通道、清零区、请求、声像/音量表顺序初始化。
SpatialAudioRecord* __cdecl InitializeSoundRuntimeState_52B9E0(session::ClientSession& session)
{
    for (SoundChannelState& channel : gSoundChannels104D0C8) {
        channel.value00 = 0;
        channel.currentSeNo = -1;
        channel.value30 = 0;
    }

    gAudioBytes103DDFC.fill(std::byte {0});
    gAudioBytes103DD7C.fill(std::byte {0});
    gAudioBytes104A334.fill(std::byte {0});

    for (SoundRequestRecord& request : session.Audio().soundRequests) {
        request.channelIndex = -1;
        request.requestCount = 0;
    }

    platform::Cg2dInitializeSoundTablesX87(gPanVal103CFBC.data(), gSoundVolValue10505D0.data());
    return ResetSpatialSoundSources_52CD40(session);
}

// 原 [0x52CDE0,0x52CE97)；在同一空间音频表发布更近的屏幕坐标与距离。
std::int32_t __cdecl UpdateNearestSpatialSoundSource_52CDE0(session::ClientSession& session,
    const std::int32_t index,
    const std::int32_t sourceX,
    const std::int32_t sourceY)
{
    float worldX;
    float worldY;
    platform::Cg2dScaleSpatialSourceX87(sourceX, sourceY, &worldX, &worldY);
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(session, worldX, worldY, &screenX, &screenY);
    return platform::Cg2dPublishSpatialProjectionX87(
        session.Audio().spatialSources.data(), index, &screenX, &screenY);
}

// 原函数：0x0052C9E0。
std::int32_t __cdecl AccumulateSpatialSoundRequest_52C9E0(
    session::ClientSession& session, const std::int32_t id, const std::int32_t x, const std::int32_t y)
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
        session.Audio().soundRequests[static_cast<std::size_t>(id - 250)];
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
    if (gAudioDisabled_10507D4 != 0 || platform::gEnableSound == 0
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
        if (!gSdlAudioBackend.RewindChannel(channelIndex)) {
            gSdlAudioBackend.ReleaseChannel(channelIndex);
            channel.currentSeNo = -2;
            channel.value30 = 0;
            return -1;
        }
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
    if (!gSdlAudioBackend.PlayChannel(static_cast<std::size_t>(gNextSeChannel105082C), false)) {
        gSdlAudioBackend.ReleaseChannel(channelIndex);
        channel.currentSeNo = -2;
        channel.value30 = 0;
        return -1; // RMO01：缓存重新加载失败不发布冷却或假成功。
    }

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
std::int32_t __cdecl ApplySpatialSoundRequest_52C680(
    session::ClientSession& session, const std::int32_t requestIndex, const std::int32_t isNew)
{
    if (gAudioDisabled_10507D4 != 0 || platform::gEnableSound == 0
        || requestIndex < 0 || requestIndex >= 50) {
        return -1;
    }

    SoundRequestRecord& request =
        session.Audio().soundRequests[static_cast<std::size_t>(requestIndex)];
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
        const auto rewindChannel = static_cast<std::size_t>(request.channelIndex);
        if (!gSdlAudioBackend.RewindChannel(rewindChannel)) {
            gSdlAudioBackend.ReleaseChannel(rewindChannel);
            gSoundChannels104D0C8[rewindChannel].currentSeNo = -2;
            gSoundChannels104D0C8[rewindChannel].value30 = 0;
            request.channelIndex = -1;
            return -1;
        }
        const bool loop =
            gSoundEffects103E7B0[static_cast<std::size_t>(request.seNo)].loop != 0;
        const std::int32_t playChannel = request.channelIndex;
        if (loop) {
            gSoundChannels104D0C8[static_cast<std::size_t>(
                playChannel)].value30 = 1;
        }
        if (!gSdlAudioBackend.PlayChannel(static_cast<std::size_t>(playChannel), loop)) {
            gSdlAudioBackend.ReleaseChannel(static_cast<std::size_t>(playChannel));
            gSoundChannels104D0C8[static_cast<std::size_t>(playChannel)].currentSeNo = -2;
            gSoundChannels104D0C8[static_cast<std::size_t>(playChannel)].value30 = 0;
            request.channelIndex = -1;
            return -1;
        }

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
std::int32_t __cdecl ProcessSpatialSoundRequests_52CB50(session::ClientSession& session)
{
    std::int32_t result = -1;
    for (std::size_t index = 0; index < session.Audio().soundRequests.size(); ++index) {
        SoundRequestRecord& request = session.Audio().soundRequests[index];
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
        result = ApplySpatialSoundRequest_52C680(
            session, static_cast<std::int32_t>(index), static_cast<std::int32_t>(isNew));
        request.requestCount = 0;
    }
    return result;
}

// 原 [0x52CCA0,0x52CCD1) 与独占尾块 [0x52CC40,0x52CC96)。
void UpdateAudioPlayback_52CCA0(session::ClientSession& session)
{
    UpdateFileBgm();
    if (gAudioDisabled_10507D4 != 0) {
        return;
    }

    for (SoundEffectDefinition& effect : gSoundEffects103E7B0) {
        if (effect.cooldown != 0) {
            --effect.cooldown;
        }
    }
    static_cast<void>(ProcessSpatialSoundRequests_52CB50(session));
    static_cast<void>(UpdateBgmRequest_52C910(session));

    if (session.Audio().bgmActive == 0) {
        session.Audio().bgmFadeOutActive = 0;
        return;
    }
    if (session.Audio().bgmFadeOutActive == 0) {
        return;
    }
    if (session.Audio().bgmFadeVolumeIndex != 0 && gSdlAudioBackend.HasBgm(session.Audio().bgmPlayback)) {
        --session.Audio().bgmFadeVolumeIndex;
        static_cast<void>(gSdlAudioBackend.SetBgmVolume(
            session.Audio().bgmPlayback, gSoundVolValue10505D0[static_cast<std::size_t>(session.Audio().bgmFadeVolumeIndex)]));
        return;
    }

    ResetBgmPlayback_52B940(session);
    session.Audio().bgmRecordIndex = -1;
}

// 原 [0x52CD60,0x52CDDB)；首个距离阈值不符的有效记录立即结束扫描。
void __cdecl SubmitSpatialSoundSources_52CD60(session::ClientSession& session)
{
    for (SpatialAudioRecord& record : session.Audio().spatialSources) {
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

        static_cast<void>(AccumulateSpatialSoundRequest_52C9E0(session, id + 249, x, y));
        record.value00 = 1;
    }
}

std::int32_t ReadBgmRecordIndex58D4F8(session::ClientSession& session) noexcept
{
    return session.Audio().bgmRecordIndex;
}

// 原函数：0x0052B990。
std::uint32_t BeginBgmFadeOut_52B990(session::ClientSession& session) noexcept
{
    const std::int32_t index = session.Audio().bgmRecordIndex;
    if (index < 0 || static_cast<std::uint32_t>(index) >= kRecordCount) {
        return std::bit_cast<std::uint32_t>(index);
    }

    session.Audio().bgmFadeOutActive = 1;
    const std::int32_t value00 = gRecords[static_cast<std::size_t>(index)].value40;
    const std::int32_t musicVolume = cg::platform::gMusicVolume;
    const std::uint32_t productBits =
        std::bit_cast<std::uint32_t>(value00)
        * std::bit_cast<std::uint32_t>(musicVolume);
    const std::int32_t product = std::bit_cast<std::int32_t>(productBits);
    session.Audio().bgmFadeVolumeIndex = product / 15;
    return productBits >> 31;
}

// 原 World::LoadAndPlayBGM@[52BE60,52C15C)；仅原生缓冲与播放机制映射 SDL3。
std::int32_t LoadAndPlayBGM(session::ClientSession& session, const std::uint32_t bgmId)
{
    if (std::bit_cast<std::uint32_t>(session.Audio().bgmRecordIndex) == bgmId && session.Audio().bgmFadeOutActive == 0) {
        return 1;
    }
    session.Audio().bgmFadeOutActive = 0;
    if (audio::gAudioDisabled_10507D4 != 0) {
        return -1;
    }
    if (session.Audio().bgmRecordIndex != -1) {
        audio::StopAndReleaseStreamingBgm_52B8E0(session);
    }
    if (bgmId == 0xFFFFFFFEu) {
        OnSceneBgmPlayback(session);
        session.Audio().bgmRecordIndex = -2;
        return -1;
    }
    if (bgmId >= 128 || platform::gEnableMusic == 0) {
        return -1;
    }

    OnSceneBgmPlayback(session);
    const LegacyAudioRecord& record =
        gRecords[static_cast<std::size_t>(bgmId)];
    std::uint32_t dataLength = 0;
    if (!audio::ReadPcmWaveDataLength(record.path.c_str(), dataLength)) {
        return 0;
    }
    if (!SetSessionBgmOutput(session,
            cg::session::GetClientHost().CurrentSession() == &session)) {
        return 0;
    }
    const std::wstring path = record.path;
    if (record.value44 == 2) {
        session.Audio().bgmMode = 1;
        std::FILE* const file = ::_wfopen(record.path.c_str(), L"rb");
        if (file == nullptr) {
            return 0;
        }
        static_cast<void>(::_fseeki64(file, 32, SEEK_CUR));
        const std::uint32_t blockAlign =
            static_cast<std::uint32_t>(std::fgetc(file))
            | static_cast<std::uint32_t>(std::fgetc(file)) << 8;
        const long loopOffset = static_cast<long>(
            dataLength + 14u + (dataLength & 1u) + 48u);
        static_cast<void>(::_fseeki64(file, static_cast<std::int64_t>(loopOffset), SEEK_CUR));
        std::uint32_t startFrame = 0;
        std::uint32_t endFrame = 0;
        for (int shift = 0; shift < 32; shift += 8) {
            startFrame |= static_cast<std::uint32_t>(std::fgetc(file)) << shift;
        }
        for (int shift = 0; shift < 32; shift += 8) {
            endFrame |= static_cast<std::uint32_t>(std::fgetc(file)) << shift;
        }
        std::fclose(file);
        session.Audio().bgmLoopStart = blockAlign * startFrame;
        if (audio::LoadStreamingBgm_52C160(session, path.c_str(), startFrame) == 0) {
            return 0;
        }
        session.Audio().bgmLoopEnd = blockAlign * endFrame;
        static_cast<void>(audio::gSdlAudioBackend.SetBgmLoopEnd(session.Audio().bgmPlayback, endFrame));
    } else {
        session.Audio().bgmMode = 0;
        if (!audio::gSdlAudioBackend.LoadBgmWav(session.Audio().bgmPlayback, path.c_str(), 0, 0)) {
            return 0;
        }
    }

    const std::int8_t frequencyIndex =
        platform::ReadBgmFrequencySelectionByte(
            static_cast<std::uint8_t>(bgmId));
    static_cast<void>(audio::gSdlAudioBackend.SetBgmFrequency(
        session.Audio().bgmPlayback,
        audio::gBgmFrequencies58D468[
            static_cast<std::size_t>(frequencyIndex)]));
    const std::int32_t volumeIndex =
        (platform::gMusicVolume * record.value40) / 15;
    static_cast<void>(audio::gSdlAudioBackend.SetBgmVolume(
        session.Audio().bgmPlayback, audio::gSoundVolValue10505D0[static_cast<std::size_t>(volumeIndex)]));
    static_cast<void>(audio::gSdlAudioBackend.PlayBgm(session.Audio().bgmPlayback, record.value44 != 0));
    session.Audio().bgmRecordIndex = static_cast<std::int32_t>(bgmId);
    platform::gLastRenderDt =
        static_cast<double>(platform::ReadTickCount64());
    session.Audio().bgmActive = 1;
    return 1;
}

// 原52C3D0：先失败标志/当前号/enable，再原0..15钳制；实际缓冲不存在透传level。
static std::int32_t ApplySessionBgmVolume(session::ClientSession& session)
{
    std::int32_t result = std::bit_cast<std::int32_t>(gAudioDisabled_10507D4);
    if (result == 0 && session.Audio().bgmRecordIndex != -1) {
        result = platform::gEnableMusic;
        if (result != 0) {
            result = platform::gMusicVolume;
            if (result > 15) { result = 15; platform::gMusicVolume = result; }
            else if (result < 0) { result = 0; platform::gMusicVolume = result; }
            if (gSdlAudioBackend.HasBgm(session.Audio().bgmPlayback)) {
                const auto index = result * gRecords[static_cast<std::size_t>(session.Audio().bgmRecordIndex)].value40 / 15;
                return gSdlAudioBackend.SetBgmVolume(session.Audio().bgmPlayback, gSoundVolValue10505D0[static_cast<std::size_t>(index)])
                    ? 0 : std::bit_cast<std::int32_t>(0x80004005u);
            }
        }
    }
    return result;
}

std::int32_t ApplyBgmVolume_52C3D0(session::ClientSession& session)
{
    UpdateFileBgm();
    const std::int32_t result = ApplySessionBgmVolume(session);
    for (auto& entry : cg::session::GetClientHost().Sessions()) {
        if (&entry.session != &session) {
            static_cast<void>(ApplySessionBgmVolume(entry.session));
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

// 原函数：0x00454EE0。
std::int32_t ShutdownFixedMusic_454EE0()
{
    audio::gSdlAudioBackend.StopFixedMusic();
    audio::gSdlAudioBackend.ShutdownFixedMusic();
    // WAV初始化失败时，SDL设备只由音乐持有；正常WAV owner仍由52B630释放。
    if (audio::gAudioDisabled_10507D4 != 0) { audio::gSdlAudioBackend.Close(); }
    return 0;
}

// 原函数：0x00454C30。
std::int32_t InitializeMusic_454C30(std::uintptr_t)
{
    if (platform::CheckLegacyDxCapability_458DC0() != 0) { return 1; }
    static_cast<void>(ShutdownFixedMusic_454EE0());
    if (!audio::gSdlAudioBackend.IsOpen() && !audio::gSdlAudioBackend.Open()) { return 1; }
    // 用户要求预转换为WAV；从本客户端内置资源载入，不再访问系统音色或解析MIDI。
    const HMODULE module = ::GetModuleHandleW(nullptr);
    const HRSRC resource = ::FindResourceW(module, MAKEINTRESOURCEW(kFixedMusicResourceId), MAKEINTRESOURCEW(10));
    const HGLOBAL loaded = resource != nullptr ? ::LoadResource(module, resource) : nullptr;
    const void* wav = loaded != nullptr ? ::LockResource(loaded) : nullptr;
    const DWORD size = resource != nullptr ? ::SizeofResource(module, resource) : 0;
    if (wav == nullptr || size == 0 || !audio::gSdlAudioBackend.InitializeFixedMusic(
            {static_cast<const std::uint8_t*>(wav), static_cast<std::size_t>(size)})) { return 2; }
    platform::Cg2dInitializeFixedMusicVolumeX87(gFixedMusicVolume62B0C8.data());
    audio::SetFixedMusicReverb_455030(25, 6);
    return 0;
}

// 原458AE0的DX设备检查改为真实SDL音频设备；独立DINPUT入口观察保留。
std::int32_t CheckAudioAndInputAvailability_458AE0()
{
    platform::SdlAudioBackend probe;
    if (!audio::gSdlAudioBackend.IsOpen() && !probe.Open()) {
        ::OutputDebugStringW(L"SDL audio device is unavailable\n"); return 0;
    }
    HMODULE input = ::LoadLibraryW(L"DINPUT.DLL");
    if (input == nullptr) { ::OutputDebugStringW(L"Couldn't LoadLibrary DInput\r\n"); return 0x200; }
    const bool available = ::GetProcAddress(input, "DirectInputCreateA") != nullptr;
    ::FreeLibrary(input);
    if (!available) { ::OutputDebugStringW(L"Couldn't GetProcAddress DInputCreate\r\n"); return 0x200; }
    return 0x600;
}

bool IsFileBgmAudioAvailable() noexcept
{
    return gAudioDisabled_10507D4 == 0 && gSdlAudioBackend.IsOpen();
}

float GetFileBgmGain() noexcept
{
    const auto level = std::clamp(platform::gMusicVolume, 0, 15);
    if (platform::gEnableMusic == 0 || level == 0) return 0.0f;
    const auto db = gSoundVolValue10505D0[static_cast<std::size_t>(level * 127 / 15)];
    return static_cast<float>(std::pow(10.0, db / 2000.0));
}

void SelectBgmOutput() noexcept
{
    auto& host = cg::session::GetClientHost();
    auto* const current = host.CurrentSession();
    for (auto& entry : host.Sessions()) {
        if (&entry.session != current) static_cast<void>(SetSessionBgmOutput(entry.session, false));
    }
    if (current != nullptr) static_cast<void>(SetSessionBgmOutput(*current, true));
}

bool SetSceneBgmSuppressed(session::ClientSession& session, const bool suppressed) noexcept
{
    // 后台文件音乐结束时也会恢复所属抑制；必须先确认其实际流静音。
    if (cg::session::GetClientHost().CurrentSession() != &session
        && !SetSessionBgmOutput(session, false)) return false;
    return gSdlAudioBackend.SetBgmSuppressed(session.Audio().bgmPlayback, suppressed);
}

} // namespace cg::audio
