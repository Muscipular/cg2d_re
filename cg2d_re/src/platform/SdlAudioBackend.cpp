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
        std::size_t cursor {};
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

        while (additionalAmount > 0 && channel.playing) {
            const std::size_t requested = static_cast<std::size_t>(
                additionalAmount / kFrameBytes
                + (additionalAmount % kFrameBytes != 0));
            const std::size_t limit = (std::min)(requested, kBatchFrames);
            std::size_t count = 0;
            for (; count < limit; ++count) {
                if (channel.cursor == sample.frames) {
                    if (!channel.loop) {
                        break;
                    }
                    channel.cursor = 0;
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
                return;
            }
            additionalAmount -= static_cast<int>(count) * kFrameBytes;
            if (count < limit || (!channel.loop && channel.cursor == sample.frames)) {
                channel.playing = false;
                static_cast<void>(SDL_FlushAudioStream(stream));
            }
        }
    }

    ~Impl()
    {
        for (Channel& channel : channels) {
            if (channel.stream != nullptr) {
                SDL_DestroyAudioStream(std::exchange(channel.stream, nullptr));
            }
        }
        for (Sample& sample : samples) {
            SDL_free(std::exchange(sample.data, nullptr));
            SDL_free(std::exchange(sample.pcmStereo, nullptr));
            sample.spec = {};
            sample.length = 0;
            sample.mixSpec = {};
            sample.frames = 0;
        }
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

    SDL_free(sample.data);
    SDL_free(sample.pcmStereo);
    sample.spec = spec;
    sample.data = data;
    sample.length = length;
    sample.mixSpec = mixSpec;
    sample.pcmStereo = pcmStereo;
    sample.frames = static_cast<std::size_t>(pcmLength / kFrameBytes);
    return true;
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

void SdlAudioBackend::Close() noexcept
{
    impl_.reset();
}

} // namespace cg::platform
