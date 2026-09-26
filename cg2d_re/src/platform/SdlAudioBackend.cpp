#include "cg/platform/SdlAudioBackend.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <new>
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
    };

    struct Channel {
        SDL_AudioStream* stream {};
        Sample* sample {};
        SDL_Semaphore* failureSignal {};
        std::size_t cursor {};
        std::size_t loopStart {};
        std::size_t loopEnd {};
        float volume {1.0f};
        float panLeft {1.0f};
        float panRight {1.0f};
        bool loop {};
        bool playing {};
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
    std::atomic<bool> bgmAbort {false};

    static int SDLCALL RunBgmStopWorker(void* const userdata)
    {
        auto& self = *static_cast<Impl*>(userdata);
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
    }

    static void FreeSample(Sample& sample)
    {
        SDL_free(std::exchange(sample.data, nullptr));
        SDL_free(std::exchange(sample.pcmStereo, nullptr));
        sample = {};
    }

    static bool LoadSample(Sample& sample, const char* const path)
    {
        SDL_AudioSpec spec {};
        Uint8* data = nullptr;
        Uint32 length = 0;
        if (!SDL_LoadWAV(path, &spec, &data, &length)) {
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
        const std::size_t end =
            channel.loopEnd == 0 ? sample.frames : channel.loopEnd;

        while (additionalAmount > 0 && channel.playing) {
            const std::size_t requested = static_cast<std::size_t>(
                additionalAmount / kFrameBytes
                + (additionalAmount % kFrameBytes != 0));
            const std::size_t limit = (std::min)(requested, kBatchFrames);
            std::size_t count = 0;
            for (; count < limit; ++count) {
                if (channel.cursor == end) {
                    if (!channel.loop) {
                        break;
                    }
                    channel.cursor = channel.loopStart;
                }
                const std::size_t offset = channel.cursor++ * 2;
                buffer[count * 2] =
                    source[offset] * channel.volume * channel.panLeft;
                buffer[count * 2 + 1] =
                    source[offset + 1] * channel.volume * channel.panRight;
            }

            if (count != 0 && !SDL_PutAudioStreamData(
                    stream, buffer.data(), static_cast<int>(count) * kFrameBytes)) {
                channel.playing = false;
                if (channel.failureSignal != nullptr) {
                    SDL_SignalSemaphore(channel.failureSignal);
                }
                return;
            }
            additionalAmount -= static_cast<int>(count) * kFrameBytes;
            if (count < limit || (!channel.loop && channel.cursor == end)) {
                channel.playing = false;
                static_cast<void>(SDL_FlushAudioStream(stream));
            }
        }
    }

    ~Impl()
    {
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
    const std::size_t slot, const char* const path) noexcept
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

    return Impl::LoadSample(sample, path);
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
    if (channel.stream != nullptr || sample.pcmStereo == nullptr) {
        return false;
    }

    SDL_AudioStream* const stream = SDL_CreateAudioStream(&sample.mixSpec, nullptr);
    if (stream == nullptr) {
        return false;
    }
    channel.sample = &sample;
    channel.stream = stream;
    if (!SDL_SetAudioStreamGetCallback(stream, &Impl::FeedChannel, &channel)
        || !SDL_BindAudioStream(impl_->device, stream)) {
        SDL_DestroyAudioStream(stream);
        channel = {};
        return false;
    }
    return true;
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
    Impl::Channel& channel = impl_->channels[index];
    if (!SDL_LockAudioStream(channel.stream)) {
        return false;
    }
    channel.volume = static_cast<float>(
        std::pow(10.0, static_cast<double>(hundredthDb) / 2000.0));
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return true;
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
        : static_cast<float>(std::pow(10.0, static_cast<double>(hundredthDb) / 2000.0));
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
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return SDL_ResumeAudioDevice(impl_->device);
}

bool SdlAudioBackend::LoadBgmWav(
    const char* const path,
    const std::size_t startFrame,
    const std::size_t endFrame) noexcept
{
    if (impl_ == nullptr || path == nullptr || impl_->bgm.stream != nullptr
        || !Impl::LoadSample(impl_->bgmSample, path)) {
        return false;
    }
    Impl::Sample& sample = impl_->bgmSample;
    const std::size_t end = endFrame == 0 ? sample.frames : endFrame;
    if (startFrame >= end || end > sample.frames) {
        Impl::FreeSample(sample);
        return false;
    }

    Impl::Channel& channel = impl_->bgm;
    channel.stream = SDL_CreateAudioStream(&sample.mixSpec, nullptr);
    if (channel.stream == nullptr) {
        Impl::FreeSample(sample);
        return false;
    }
    channel.sample = &sample;
    channel.loopStart = startFrame;
    channel.loopEnd = end;
    if (!SDL_SetAudioStreamGetCallback(
            channel.stream, &Impl::FeedChannel, &channel)
        || !SDL_BindAudioStream(impl_->device, channel.stream)) {
        ReleaseBgm();
        return false;
    }
    return true;
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
    Impl::Channel& channel = impl_->bgm;
    if (!SDL_LockAudioStream(channel.stream)) {
        return false;
    }
    channel.volume = static_cast<float>(
        std::pow(10.0, static_cast<double>(hundredthDb) / 2000.0));
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
    return true;
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
    channel.cursor = 0;
    channel.loop = loop;
    channel.playing = true;
    static_cast<void>(SDL_UnlockAudioStream(channel.stream));
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
    std::atomic<std::uint8_t>& notice) noexcept
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
    state.bgmWorker =
        SDL_CreateThread(&Impl::RunBgmStopWorker, "cg2d-bgm-stop", &state);
    if (state.bgmWorker == nullptr) {
        state.bgmStopNotice = nullptr;
        SDL_DestroySemaphore(std::exchange(state.bgmStopRequest, nullptr));
        return false;
    }
    state.bgm.failureSignal = state.bgmStopRequest;
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
    Impl::FreeSample(impl_->bgmSample);
}

void SdlAudioBackend::Close() noexcept
{
    impl_.reset();
}

} // namespace cg::platform
