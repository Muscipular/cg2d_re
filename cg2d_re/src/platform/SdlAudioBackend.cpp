#include "cg/platform/SdlAudioBackend.hpp"
#include "cg/platform/LegacyWavesReverb.hpp"

#include <SDL3/SDL.h>

#include "cg/core/m_log.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <new>
#include <string_view>
#include <utility>

namespace cg::platform {

struct SdlAudioBackend::Impl {
    struct Sample {
        SDL_AudioSpec spec {};
        Uint8* data {};
        Uint32 length {};
        SDL_AudioSpec mixSpec {};
        Uint8* pcmStereo {};
        std::size_t frames {};
        bool traceWarpEffect {}; // 临时诊断cgsys06.wav，识别其他音效编号误用同一样本。
    };

    struct Channel {
        SDL_AudioStream* stream {};
        Sample* sample {};
        SDL_Semaphore* failureSignal {};
        std::size_t cursor {};
        std::size_t loopStart {};
        std::size_t loopEnd {};
        float panLeft {1.0f};
        float panRight {1.0f};
        bool loop {};
        std::uint32_t remainingRepeats {}; // 固定WAV的有限重复次数；普通音效/BGM仍由loop控制。
        bool playing {};
        bool feedFailed {};
        LegacyWavesReverb* reverb {}; // 仅固定WAV借同一Impl效果owner，普通BGM/音效为空。
        std::uint64_t outputFrames {}; // 固定效果连续时点，样本游标回环时不重置。
        bool reverbHasTail {}; // 当前效果返回S_FALSE，尚有尾音。
        bool drainingReverb {}; // 最后一遍已结束，后续只给效果零输入。
    };

    bool audioInitialized {};
    SDL_AudioDeviceID device {};
    std::array<Sample, kSampleSlots> samples {};
    std::array<Channel, kChannelCount> channels {};
    Sample bgmSample {};
    Channel bgm {};
    SDL_Semaphore* bgmStopRequest {};
    SDL_Thread* bgmWorker {};
    std::atomic<std::uint8_t>* bgmStopNotice {};
    std::atomic<std::uint32_t>* bgmReady {};
    std::atomic<bool> bgmAbort {false};
    bool bgmBound {};
    Sample fixedMusicSample;
    Channel fixedMusic;
    bool fixedMusicPrepared {};
    LegacyWavesReverb fixedMusicReverb; // 唯一固定效果线程owner，流退出回调后Close。

    static int SDLCALL RunBgmStopWorker(void* const userdata)
    {
        auto& self = *static_cast<Impl*>(userdata);
        if (SDL_LockAudioStream(self.bgm.stream)) {
            Channel& channel = self.bgm;
            channel.loop = true;
            channel.playing = true;
            constexpr int kFrameBytes = 2 * sizeof(float);
            const int initialFrames = (std::min)(
                channel.sample->mixSpec.freq,
                (std::numeric_limits<int>::max)() / kFrameBytes);
            FeedChannel(&channel, channel.stream, initialFrames * kFrameBytes, 0);
            self.bgmReady->store(1, std::memory_order_release);
            if (channel.feedFailed) {
                SDL_SignalSemaphore(self.bgmStopRequest);
            }
            static_cast<void>(SDL_UnlockAudioStream(channel.stream));
        } else {
            self.bgmReady->store(2, std::memory_order_release);
        }
        SDL_WaitSemaphore(self.bgmStopRequest);
        if (self.bgmAbort.load(std::memory_order_acquire)) {
            return 0;
        }
        if (!SDL_LockAudioStream(self.bgm.stream)) {
            return 0;
        }
        if (!self.bgmAbort.load(std::memory_order_acquire)) {
            self.bgmStopNotice->store(1, std::memory_order_release);
        }
        static_cast<void>(SDL_UnlockAudioStream(self.bgm.stream));
        return 0;
    }

    void StopBgmWorker()
    {
        if (bgmWorker != nullptr) {
            bgmAbort.store(true, std::memory_order_release);
            SDL_SignalSemaphore(bgmStopRequest);
            SDL_WaitThread(std::exchange(bgmWorker, nullptr), nullptr);
        }
        bgmStopNotice = nullptr;
        bgmReady = nullptr;
    }

    static void FreeSample(Sample& sample)
    {
        SDL_free(std::exchange(sample.data, nullptr));
        SDL_free(std::exchange(sample.pcmStereo, nullptr));
        sample = {};
    }

    static bool LoadSample(Sample& sample, SDL_IOStream* const input)
    {
        SDL_AudioSpec spec {};
        Uint8* data = nullptr;
        Uint32 length = 0;
        if (input == nullptr || !SDL_LoadWAV_IO(input, true, &spec, &data, &length)) {
            return false;
        }

        SDL_AudioSpec mixSpec {SDL_AUDIO_F32, 2, spec.freq};
        Uint8* pcmStereo = nullptr;
        int pcmLength = 0;
        constexpr int kFrameBytes = 2 * sizeof(float);
        if (length > static_cast<Uint32>((std::numeric_limits<int>::max)())
            || !SDL_ConvertAudioSamples(
                &spec, data, static_cast<int>(length),
                &mixSpec, &pcmStereo, &pcmLength)
            || pcmLength <= 0 || pcmLength % kFrameBytes != 0) {
            SDL_free(pcmStereo);
            SDL_free(data);
            return false;
        }

        FreeSample(sample);
        sample.spec = spec;
        sample.data = data;
        sample.length = length;
        sample.mixSpec = mixSpec;
        sample.pcmStereo = pcmStereo;
        sample.frames = static_cast<std::size_t>(pcmLength / kFrameBytes);
        return true;
    }

    static bool LoadSample(Sample& sample, const wchar_t* const path)
    {
        // SDL文件API只接收UTF-8；转换缓冲不离开此边界。
        char* const encodedPath = SDL_iconv_wchar_utf8(path);
        if (encodedPath == nullptr) {
            return false;
        }
        SDL_IOStream* const input = SDL_IOFromFile(encodedPath, "rb");
        SDL_free(encodedPath);
        return LoadSample(sample, input);
    }

    static void SDLCALL FeedChannel(
        void* userdata, SDL_AudioStream* stream, int additionalAmount, int)
    {
        auto& channel = *static_cast<Channel*>(userdata);
        if (!channel.playing || additionalAmount <= 0) {
            return;
        }

        constexpr std::size_t kBatchFrames = 512;
        constexpr int kFrameBytes = 2 * sizeof(float);
        std::array<float, kBatchFrames * 2> buffer {};
        const Sample& sample = *channel.sample;
        const auto* source = reinterpret_cast<const float*>(sample.pcmStereo);
        const std::size_t end = channel.loopEnd;

        while (additionalAmount > 0 && channel.playing) {
            const std::size_t requested = static_cast<std::size_t>(
                additionalAmount / kFrameBytes
                + (additionalAmount % kFrameBytes != 0));
            const std::size_t limit = (std::min)(requested, kBatchFrames);
            std::size_t count = 0;
            for (; !channel.drainingReverb && count < limit; ++count) {
                if (channel.cursor == end) {
                    if (!channel.loop) {
                        if (channel.remainingRepeats == 0) { break; }
                        --channel.remainingRepeats;
                    }
                    channel.cursor = channel.loopStart;
                }
                const std::size_t offset = channel.cursor++ * 2;
                buffer[count * 2] =
                    source[offset] * channel.panLeft;
                buffer[count * 2 + 1] =
                    source[offset + 1] * channel.panRight;
            }

            if (channel.drainingReverb) {
                buffer.fill(0.0f);
                count = limit;
            }
            if (channel.reverb != nullptr && count != 0) {
                if (!channel.reverb->Process({buffer.data(), count * 2}, channel.outputFrames,
                        channel.drainingReverb, channel.reverbHasTail)) {
                    channel.playing = false;
                    channel.feedFailed = true;
                    return;
                }
                channel.outputFrames += count;
            }
            if (count != 0 && !SDL_PutAudioStreamData(
                    stream, buffer.data(), static_cast<int>(count) * kFrameBytes)) {
                channel.playing = false;
                channel.feedFailed = true;
                if (channel.failureSignal != nullptr) {
                    SDL_SignalSemaphore(channel.failureSignal);
                }
                return;
            }
            additionalAmount -= static_cast<int>(count) * kFrameBytes;
            if (channel.drainingReverb) {
                if (!channel.reverbHasTail) {
                    channel.playing = false;
                    static_cast<void>(SDL_FlushAudioStream(stream));
                }
            } else if (count < limit || (!channel.loop && channel.remainingRepeats == 0 && channel.cursor == end)) {
                if (channel.reverb != nullptr && channel.reverbHasTail) {
                    channel.drainingReverb = true;
                } else {
                    channel.playing = false;
                    if (sample.traceWarpEffect) {
                        m_log("[audio][sample57-feed-end] channel=%p cursor=%zu frames=%zu\n",
                            static_cast<void*>(&channel), channel.cursor, end);
                    }
                    static_cast<void>(SDL_FlushAudioStream(stream));
                }
            }
        }
    }

    // 音效与固定WAV共用绑定流程；采样资源由各自owner持有，回调不借临时对象。
    static bool PrepareChannel(Channel& channel, Sample& sample, SDL_AudioDeviceID device)
    {
        if (channel.stream != nullptr || sample.pcmStereo == nullptr) { return false; }
        SDL_AudioStream* const stream = SDL_CreateAudioStream(&sample.mixSpec, nullptr);
        if (stream == nullptr) { return false; }
        channel.sample = &sample;
        channel.stream = stream;
        channel.loopEnd = sample.frames;
        if (!SDL_SetAudioStreamGetCallback(stream, &Impl::FeedChannel, &channel)
            || !SDL_BindAudioStream(device, stream)) {
            SDL_DestroyAudioStream(stream);
            channel = {};
            return false;
        }
        return true;
    }

    ~Impl()
    {
        if (fixedMusic.stream != nullptr) { SDL_DestroyAudioStream(std::exchange(fixedMusic.stream, nullptr)); }
        fixedMusicReverb.Close();
        FreeSample(fixedMusicSample);
        StopBgmWorker();
        if (bgm.stream != nullptr) {
            SDL_DestroyAudioStream(std::exchange(bgm.stream, nullptr));
        }
        if (bgmStopRequest != nullptr) {
            SDL_DestroySemaphore(std::exchange(bgmStopRequest, nullptr));
        }
        for (Channel& channel : channels) {
            if (channel.stream != nullptr) {
                SDL_DestroyAudioStream(std::exchange(channel.stream, nullptr));
            }
        }
        for (Sample& sample : samples) {
            FreeSample(sample);
        }
        FreeSample(bgmSample);
        if (device != 0) {
            SDL_CloseAudioDevice(device);
        }
        if (audioInitialized) {
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }
    }
};

SdlAudioBackend::SdlAudioBackend() noexcept = default;

SdlAudioBackend::~SdlAudioBackend() = default;

bool SdlAudioBackend::Open() noexcept
{
    if (impl_ != nullptr) {
        return false;
    }

    std::unique_ptr<Impl> pending(new (std::nothrow) Impl);
    if (pending == nullptr || !SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        return false;
    }
    pending->audioInitialized = true;
    pending->device =
        SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (pending->device == 0) {
        return false;
    }

    impl_ = std::move(pending);
    return true;
}

bool SdlAudioBackend::IsOpen() const noexcept
{
    return impl_ != nullptr;
}

bool SdlAudioBackend::LoadWav(
    const std::size_t slot, const wchar_t* const path) noexcept
{
    if (impl_ == nullptr || slot >= kSampleSlots || path == nullptr) {
        return false;
    }
    Impl::Sample& sample = impl_->samples[slot];
    for (const Impl::Channel& channel : impl_->channels) {
        if (channel.stream != nullptr && channel.sample == &sample) {
            return false;
        }
    }

    const bool loaded = Impl::LoadSample(sample, path);
    if (loaded) {
        sample.traceWarpEffect = std::wstring_view{path}.ends_with(L"cgsys06.wav");
        if (sample.traceWarpEffect) {
            m_log("[audio][sample57-load] slot=%zu frames=%zu rate=%d\n",
                slot, sample.frames, sample.mixSpec.freq);
        }
    }
    return loaded;
}

bool SdlAudioBackend::HasSample(const std::size_t slot) const noexcept
{
    return impl_ != nullptr && slot < kSampleSlots
        && impl_->samples[slot].data != nullptr;
}

bool SdlAudioBackend::PrepareChannel(
    const std::size_t channelIndex, const std::size_t sampleIndex) noexcept
{
    if (impl_ == nullptr || channelIndex >= kChannelCount
        || sampleIndex >= kSampleSlots) {
        return false;
    }
    Impl::Channel& channel = impl_->channels[channelIndex];
    Impl::Sample& sample = impl_->samples[sampleIndex];
    return Impl::PrepareChannel(channel, sample, impl_->device);
}

bool SdlAudioBackend::HasChannel(const std::size_t index) const noexcept
{
    return impl_ != nullptr && index < kChannelCount
        && impl_->channels[index].stream != nullptr;
}

bool SdlAudioBackend::StopChannel(const std::size_t index) noexcept
{
    if (!HasChannel(index)) {
        return false;
    }
    Impl::Channel& channel = impl_->channels[index];
    if (!SDL_LockAudioStream(channel.stream)) {
        return false;
    }
    channel.playing = false;
    const bool cleared = SDL_ClearAudioStream(channel.stream);
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return cleared;
}

bool SdlAudioBackend::RewindChannel(const std::size_t index) noexcept
{
    if (!HasChannel(index)) {
        return false;
    }
    Impl::Channel& channel = impl_->channels[index];
    if (!SDL_LockAudioStream(channel.stream)) {
        return false;
    }
    const bool cleared = SDL_ClearAudioStream(channel.stream);
    if (cleared) {
        channel.cursor = 0;
        channel.playing = false;
    }
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return cleared;
}

void SdlAudioBackend::ReleaseChannel(const std::size_t index) noexcept
{
    if (!HasChannel(index)) {
        return;
    }
    Impl::Channel& channel = impl_->channels[index];
    SDL_DestroyAudioStream(channel.stream);
    channel = {};
}

bool SdlAudioBackend::SetChannelVolume(
    const std::size_t index, const std::int32_t hundredthDb) noexcept
{
    if (!HasChannel(index) || hundredthDb < -10000 || hundredthDb > 0) {
        return false;
    }
    // 增益在SDL取出数据时应用，音量变化也覆盖尚未消费的已排队PCM。
    return SDL_SetAudioStreamGain(
        impl_->channels[index].stream, static_cast<float>(
            std::pow(10.0, hundredthDb / 2000.0)));
}

bool SdlAudioBackend::SetChannelPan(
    const std::size_t index, const std::int32_t hundredthDb) noexcept
{
    if (!HasChannel(index) || hundredthDb < -10000 || hundredthDb > 10000) {
        return false;
    }
    Impl::Channel& channel = impl_->channels[index];
    if (!SDL_LockAudioStream(channel.stream)) {
        return false;
    }
    channel.panLeft = hundredthDb <= 0 ? 1.0f
        : static_cast<float>(std::pow(10.0, -static_cast<double>(hundredthDb) / 2000.0));
    channel.panRight = hundredthDb >= 0 ? 1.0f
        : static_cast<float>(std::pow(10.0, hundredthDb / 2000.0));
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return true;
}

bool SdlAudioBackend::SetChannelFrequency(
    const std::size_t index, const std::uint32_t hz) noexcept
{
    if (!HasChannel(index)) {
        return false;
    }
    Impl::Channel& channel = impl_->channels[index];
    const float ratio = static_cast<float>(hz)
        / static_cast<float>(channel.sample->mixSpec.freq);
    return SDL_SetAudioStreamFrequencyRatio(channel.stream, ratio);
}

bool SdlAudioBackend::PlayChannel(
    const std::size_t index, const bool loop) noexcept
{
    if (!HasChannel(index)) {
        return false;
    }
    Impl::Channel& channel = impl_->channels[index];
    if (!SDL_LockAudioStream(channel.stream)) {
        return false;
    }
    channel.loop = loop;
    channel.playing = true;
    if (channel.sample->traceWarpEffect) {
        m_log("[audio][sample57-start] channel=%p index=%zu cursor=%zu frames=%zu loop=%d repeats=%u\n",
            static_cast<void*>(&channel), index, channel.cursor, channel.loopEnd,
            loop ? 1 : 0, channel.remainingRepeats);
    }
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return SDL_ResumeAudioDevice(impl_->device);
}

bool SdlAudioBackend::LoadBgmWav(
    const wchar_t* const path,
    const std::size_t startFrame,
    const std::size_t endFrame) noexcept
{
    if (impl_ == nullptr || path == nullptr || impl_->bgmWorker != nullptr) {
        return false;
    }
    Impl::Sample pending {};
    if (!Impl::LoadSample(pending, path)) {
        return false;
    }
    const std::size_t end = endFrame == 0 ? pending.frames : endFrame;
    if (startFrame >= end || end > pending.frames) {
        Impl::FreeSample(pending);
        return false;
    }

    SDL_AudioStream* const stream = SDL_CreateAudioStream(&pending.mixSpec, nullptr);
    if (stream == nullptr) {
        Impl::FreeSample(pending);
        return false;
    }
    if (!SDL_SetAudioStreamGetCallback(
            stream, &Impl::FeedChannel, &impl_->bgm)) {
        SDL_DestroyAudioStream(stream);
        Impl::FreeSample(pending);
        return false;
    }

    ReleaseBgm();
    impl_->bgmSample = pending;
    Impl::Channel& channel = impl_->bgm;
    channel.stream = stream;
    channel.sample = &impl_->bgmSample;
    channel.loopStart = startFrame;
    channel.loopEnd = end;
    return true;
}

bool SdlAudioBackend::SetBgmLoopEnd(const std::size_t endFrame) noexcept
{
    if (!HasBgm() || !SDL_LockAudioStream(impl_->bgm.stream)) {
        return false;
    }
    // 原创建入口先按完整PCM预填；调用父在ready后才发布当前循环终点。
    impl_->bgm.loopEnd = endFrame;
    static_cast<void>(SDL_UnlockAudioStream(impl_->bgm.stream));
    return true;
}

std::uint32_t SdlAudioBackend::ReadBgmDataLength() const noexcept
{
    return HasBgm() ? impl_->bgmSample.length : 0;
}

bool SdlAudioBackend::HasBgm() const noexcept
{
    return impl_ != nullptr && impl_->bgm.stream != nullptr;
}

bool SdlAudioBackend::SetBgmVolume(
    const std::int32_t hundredthDb) noexcept
{
    if (!HasBgm() || hundredthDb < -10000 || hundredthDb > 0) {
        return false;
    }
    // 流式首段已在设置音量前预填；不将旧音量烘焙进PCM或重复衰减。
    return SDL_SetAudioStreamGain(
        impl_->bgm.stream, static_cast<float>(
            std::pow(10.0, hundredthDb / 2000.0)));
}

bool SdlAudioBackend::SetBgmFrequency(const std::uint32_t hz) noexcept
{
    if (!HasBgm()) {
        return false;
    }
    const Impl::Channel& channel = impl_->bgm;
    const float ratio = static_cast<float>(hz)
        / static_cast<float>(channel.sample->mixSpec.freq);
    return SDL_SetAudioStreamFrequencyRatio(channel.stream, ratio);
}

bool SdlAudioBackend::PlayBgm(const bool loop) noexcept
{
    if (!HasBgm()) {
        return false;
    }
    Impl::Channel& channel = impl_->bgm;
    if (!SDL_LockAudioStream(channel.stream)) {
        return false;
    }
    if (impl_->bgmWorker == nullptr) {
        channel.cursor = 0;
    }
    channel.loop = loop;
    channel.playing = true;
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    if (!impl_->bgmBound) {
        if (!SDL_BindAudioStream(impl_->device, channel.stream)) {
            return false;
        }
        impl_->bgmBound = true;
    }
    return SDL_ResumeAudioDevice(impl_->device);
}

bool SdlAudioBackend::StopBgm() noexcept
{
    if (!HasBgm()) {
        return false;
    }
    Impl::Channel& channel = impl_->bgm;
    if (!SDL_LockAudioStream(channel.stream)) {
        return false;
    }
    channel.playing = false;
    const bool cleared = SDL_ClearAudioStream(channel.stream);
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return cleared;
}

bool SdlAudioBackend::StartBgmStopWorker(
    std::atomic<std::uint8_t>& notice,
    std::atomic<std::uint32_t>& ready) noexcept
{
    if (!HasBgm() || impl_->bgmWorker != nullptr) {
        return false;
    }
    Impl& state = *impl_;
    state.bgmStopRequest = SDL_CreateSemaphore(0);
    if (state.bgmStopRequest == nullptr) {
        return false;
    }
    state.bgmAbort.store(false, std::memory_order_release);
    state.bgmStopNotice = &notice;
    state.bgmReady = &ready;
    ready.store(0, std::memory_order_release);
    state.bgm.failureSignal = state.bgmStopRequest;
    state.bgmWorker =
        SDL_CreateThread(&Impl::RunBgmStopWorker, "cg2d-bgm-stop", &state);
    if (state.bgmWorker == nullptr) {
        state.bgmStopNotice = nullptr;
        state.bgmReady = nullptr;
        state.bgm.failureSignal = nullptr;
        SDL_DestroySemaphore(std::exchange(state.bgmStopRequest, nullptr));
        return false;
    }
    return true;
}

bool SdlAudioBackend::RequestBgmStop() noexcept
{
    if (!HasBgm() || impl_->bgmWorker == nullptr) {
        return false;
    }
    SDL_SignalSemaphore(impl_->bgmStopRequest);
    return true;
}

void SdlAudioBackend::ReleaseBgm() noexcept
{
    if (impl_ == nullptr) {
        return;
    }
    Impl::Channel& channel = impl_->bgm;
    impl_->StopBgmWorker();
    if (channel.stream != nullptr) {
        SDL_DestroyAudioStream(channel.stream);
    }
    if (impl_->bgmStopRequest != nullptr) {
        SDL_DestroySemaphore(std::exchange(impl_->bgmStopRequest, nullptr));
    }
    channel = {};
    impl_->bgmBound = false;
    Impl::FreeSample(impl_->bgmSample);
}

void SdlAudioBackend::Close() noexcept
{
    impl_.reset();
}

// 关联原入口：0x00454FE0／0x00455100，固定WAV预载承接旧音色库装载／下载职责。
bool SdlAudioBackend::InitializeFixedMusic(std::span<const std::uint8_t> wav) noexcept
{
    if (impl_ == nullptr) { return false; }
    ShutdownFixedMusic();
    if (!Impl::LoadSample(impl_->fixedMusicSample, SDL_IOFromConstMem(wav.data(), wav.size()))
        || !Impl::PrepareChannel(impl_->fixedMusic, impl_->fixedMusicSample, impl_->device)) {
        ShutdownFixedMusic();
        return false;
    }
    // 原455030属性失败不使音乐初始化失败；真实配置成功后才接入PCM效果。
    static_cast<void>(impl_->fixedMusicReverb.Open(impl_->fixedMusicSample.mixSpec.freq));
    return true;
}

bool SdlAudioBackend::HasFixedMusic() const noexcept
{
    return impl_ != nullptr && impl_->fixedMusic.stream != nullptr;
}

bool SdlAudioBackend::PrepareFixedMusic() noexcept
{
    if (!HasFixedMusic() || !SDL_LockAudioStream(impl_->fixedMusic.stream)) { return false; }
    auto& channel = impl_->fixedMusic;
    channel.playing = false;
    const bool cleared = SDL_ClearAudioStream(channel.stream);
    channel.cursor = 0;
    channel.loop = false;
    channel.remainingRepeats = 0;
    channel.feedFailed = false;
    channel.outputFrames = 0;
    channel.reverbHasTail = false;
    channel.drainingReverb = false;
    if (channel.reverb != nullptr) { static_cast<void>(channel.reverb->Reset()); }
    impl_->fixedMusicPrepared = cleared;
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return cleared;
}

bool SdlAudioBackend::HasFixedMusicPlayback() const noexcept
{
    return HasFixedMusic() && impl_->fixedMusicPrepared;
}

bool SdlAudioBackend::SetFixedMusicRepeats(const std::uint32_t repeats) noexcept
{
    if (!HasFixedMusic() || !SDL_LockAudioStream(impl_->fixedMusic.stream)) { return false; }
    auto& channel = impl_->fixedMusic;
    const bool prepared = impl_->fixedMusicPrepared;
    if (prepared) {
        channel.loop = repeats == 0xFFFFFFFFu;
        channel.remainingRepeats = repeats;
    }
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return prepared;
}

bool SdlAudioBackend::PlayFixedMusic() noexcept
{
    if (!HasFixedMusic() || !SDL_LockAudioStream(impl_->fixedMusic.stream)) { return false; }
    auto& channel = impl_->fixedMusic;
    const bool prepared = impl_->fixedMusicPrepared;
    if (prepared) {
        channel.playing = true;
    }
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return prepared && SDL_ResumeAudioDevice(impl_->device);
}

bool SdlAudioBackend::SetFixedMusicVolume(std::int32_t hundredthDb) noexcept
{
    if (!HasFixedMusic() || hundredthDb < -10000 || hundredthDb > 0) { return false; }
    return SDL_SetAudioStreamGain(impl_->fixedMusic.stream, static_cast<float>(std::pow(10.0, hundredthDb / 2000.0)));
}

bool SdlAudioBackend::SetFixedMusicReverb(const std::int32_t decay, const std::int32_t mix) noexcept
{
    if (!HasFixedMusic() || !SDL_LockAudioStream(impl_->fixedMusic.stream)) { return false; }
    const bool configured = impl_->fixedMusicReverb.Configure(decay, mix);
    if (configured) { impl_->fixedMusic.reverb = &impl_->fixedMusicReverb; }
    static_cast<void>(SDL_UnlockAudioStream(impl_->fixedMusic.stream));
    return configured;
}

void SdlAudioBackend::StopFixedMusic() noexcept
{
    if (HasFixedMusic() && SDL_LockAudioStream(impl_->fixedMusic.stream)) {
        impl_->fixedMusic.playing = false;
        static_cast<void>(SDL_ClearAudioStream(impl_->fixedMusic.stream));
        impl_->fixedMusic.outputFrames = 0;
        impl_->fixedMusic.reverbHasTail = false;
        impl_->fixedMusic.drainingReverb = false;
        if (impl_->fixedMusic.reverb != nullptr) { static_cast<void>(impl_->fixedMusic.reverb->Reset()); }
        static_cast<void>(SDL_UnlockAudioStream(impl_->fixedMusic.stream));
    }
}

void SdlAudioBackend::ReleaseFixedMusicPlayback() noexcept
{
    if (HasFixedMusic() && SDL_LockAudioStream(impl_->fixedMusic.stream)) {
        impl_->fixedMusic.playing = false;
        impl_->fixedMusic.cursor = 0;
        impl_->fixedMusicPrepared = false;
        static_cast<void>(SDL_ClearAudioStream(impl_->fixedMusic.stream));
        static_cast<void>(SDL_UnlockAudioStream(impl_->fixedMusic.stream));
    }
}

// 关联原入口：0x00455260，旧装载器释放职责归并到固定WAV清理。
void SdlAudioBackend::ShutdownFixedMusic() noexcept
{
    if (impl_ == nullptr) { return; }
    if (impl_->fixedMusic.stream != nullptr) { SDL_DestroyAudioStream(std::exchange(impl_->fixedMusic.stream, nullptr)); }
    impl_->fixedMusicReverb.Close();
    impl_->fixedMusic = {};
    impl_->fixedMusicPrepared = false;
    Impl::FreeSample(impl_->fixedMusicSample);
}

} // namespace cg::platform
