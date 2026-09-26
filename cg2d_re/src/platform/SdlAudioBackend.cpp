#include "cg/platform/SdlAudioBackend.hpp"

#include <SDL3/SDL.h>

#include <array>
#include <new>
#include <utility>

namespace cg::platform {

struct SdlAudioBackend::Impl {
    struct Sample {
        SDL_AudioSpec spec {};
        Uint8* data {};
        Uint32 length {};
    };

    bool audioInitialized {};
    SDL_AudioDeviceID device {};
    std::array<Sample, kSampleSlots> samples {};

    ~Impl()
    {
        for (Sample& sample : samples) {
            SDL_free(std::exchange(sample.data, nullptr));
            sample.spec = {};
            sample.length = 0;
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

bool SdlAudioBackend::LoadWav(
    const std::size_t slot, const char* const path) noexcept
{
    if (impl_ == nullptr || slot >= kSampleSlots || path == nullptr) {
        return false;
    }

    SDL_AudioSpec spec {};
    Uint8* data = nullptr;
    Uint32 length = 0;
    if (!SDL_LoadWAV(path, &spec, &data, &length)) {
        return false;
    }

    Impl::Sample& sample = impl_->samples[slot];
    SDL_free(sample.data);
    sample.spec = spec;
    sample.data = data;
    sample.length = length;
    return true;
}

bool SdlAudioBackend::HasSample(const std::size_t slot) const noexcept
{
    return impl_ != nullptr && slot < kSampleSlots
        && impl_->samples[slot].data != nullptr;
}

void SdlAudioBackend::Close() noexcept
{
    impl_.reset();
}

} // namespace cg::platform
