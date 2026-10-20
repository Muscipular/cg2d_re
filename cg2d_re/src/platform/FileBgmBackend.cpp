#include "cg/platform/FileBgmBackend.hpp"
#include "cg/core/m_log.hpp"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <filesystem>
#include <map>
#include <atomic>

namespace cg::platform {

struct FileBgmBackend::Impl {
    // track先销毁，等待混音线程释放音频引用后再释放原文件缓存。
    struct Playback {
        MIX_Audio* audio {};
        MIX_Track* track {};
        ~Playback() { MIX_DestroyTrack(track); MIX_DestroyAudio(audio); }
    };
    struct Properties {
        SDL_PropertiesID id {SDL_CreateProperties()};
        ~Properties() { SDL_DestroyProperties(id); }
    };
    bool initialized {};
    MIX_Mixer* mixer {};
    SDL_AudioDeviceID device {};
    bool watching {};
    std::atomic<bool> deviceRemoved {false};
    std::map<std::uint64_t, Playback> playbacks;

    // SDL事件可能来自其他线程，只发布标记，不改场景或调用Lua。
    static bool SDLCALL WatchDevice(void* userdata, SDL_Event* event)
    {
        auto& self = *static_cast<Impl*>(userdata);
        if (event->type == SDL_EVENT_AUDIO_DEVICE_REMOVED && !event->adevice.recording
            && event->adevice.which == self.device) self.deviceRemoved.store(true);
        return true;
    }

    ~Impl()
    {
        if (watching) SDL_RemoveEventWatch(WatchDevice, this);
        playbacks.clear();
        MIX_DestroyMixer(mixer);
        if (initialized) MIX_Quit();
    }
};

FileBgmBackend::FileBgmBackend() noexcept = default;
FileBgmBackend::~FileBgmBackend() = default;

bool FileBgmBackend::Start(const std::uint64_t id, const wchar_t* path,
    const bool loop, std::string& error)
{
    if (!impl_) {
        auto pending = std::make_unique<Impl>();
        pending->initialized = MIX_Init();
        if (!pending->initialized) { error = SDL_GetError(); return false; }
        // 构建配置与实际可用解码器双重核对，禁止只交付其中一种格式。
        bool wave = false, mp3 = false, vorbis = false;
        for (int i = 0; i < MIX_GetNumAudioDecoders(); ++i) {
            const char* decoder = MIX_GetAudioDecoder(i);
            wave |= SDL_strcmp(decoder, "WAV") == 0;
            mp3 |= SDL_strcmp(decoder, "DRMP3") == 0;
            vorbis |= SDL_strcmp(decoder, "VORBIS") == 0;
        }
        if (!wave || !mp3 || !vorbis) {
            error = "WAV, MP3 and Ogg Vorbis decoders are all required";
            return false;
        }
        pending->mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!pending->mixer) { error = SDL_GetError(); return false; }
        pending->device = static_cast<SDL_AudioDeviceID>(SDL_GetNumberProperty(
            MIX_GetMixerProperties(pending->mixer), MIX_PROP_MIXER_DEVICE_NUMBER, 0));
        pending->watching = SDL_AddEventWatch(Impl::WatchDevice, pending.get());
        if (!pending->device || !pending->watching) { error = SDL_GetError(); return false; }
        impl_ = std::move(pending);
    }
    if (!IsAvailable()) { error = "File BGM audio device is unavailable"; return false; }

    const auto utf8 = std::filesystem::path(path).u8string();
    Impl::Properties props;
    if (!props.id) { error = SDL_GetError(); return false; }
    // IO由本函数RAII持有，加载结束即关闭，不在播放期间持有文件。
    const auto closeIo = [](SDL_IOStream* io) { SDL_CloseIO(io); };
    std::unique_ptr<SDL_IOStream, decltype(closeIo)> io(
        SDL_IOFromFile(reinterpret_cast<const char*>(utf8.c_str()), "rb"), closeIo);
    if (!io) { error = SDL_GetError(); return false; }
    if (!SDL_SetPointerProperty(props.id, MIX_PROP_AUDIO_LOAD_IOSTREAM_POINTER, io.get())
        || !SDL_SetBooleanProperty(props.id, MIX_PROP_AUDIO_LOAD_CLOSEIO_BOOLEAN, false)
        || !SDL_SetBooleanProperty(props.id, MIX_PROP_AUDIO_LOAD_PREDECODE_BOOLEAN, false)
        || !SDL_SetBooleanProperty(props.id, MIX_PROP_AUDIO_LOAD_IGNORE_LOOPS_BOOLEAN, true)
        || !SDL_SetPointerProperty(props.id, MIX_PROP_AUDIO_LOAD_PREFERRED_MIXER_POINTER, impl_->mixer)
        || !SDL_SetNumberProperty(props.id, MIX_PROP_PLAY_LOOPS_NUMBER, loop ? -1 : 0)) {
        error = SDL_GetError();
        return false;
    }
    auto [it, inserted] = impl_->playbacks.try_emplace(id);
    if (!inserted) { error = "BGM playback identity already exists"; return false; }
    auto& playback = it->second;
    playback.audio = MIX_LoadAudioWithProperties(props.id);
    if (playback.audio && MIX_GetAudioDuration(playback.audio) == 0) {
        error = "BGM file contains no audio frames";
        impl_->playbacks.erase(it);
        return false;
    }
    if (playback.audio) playback.track = MIX_CreateTrack(impl_->mixer);
    if (!playback.audio || !playback.track
        || !MIX_SetTrackAudio(playback.track, playback.audio)
        || !MIX_SetTrackGain(playback.track, 0.0f)
        || !MIX_PlayTrack(playback.track, props.id)) {
        error = SDL_GetError();
        impl_->playbacks.erase(it);
        return false;
    }
    return true;
}

bool FileBgmBackend::SetGain(const std::uint64_t id, const float gain) noexcept
{
    if (!impl_) return false;
    const auto it = impl_->playbacks.find(id);
    if (it == impl_->playbacks.end()) return false;
    if (MIX_SetTrackGain(it->second.track, gain)) return true;
    m_log_iw(L"[Lua BGM] MIX_SetTrackGain: %hs\n", SDL_GetError());
    return false;
}

bool FileBgmBackend::IsPlaying(const std::uint64_t id) const noexcept
{
    if (!impl_) return false;
    const auto it = impl_->playbacks.find(id);
    return it != impl_->playbacks.end() && MIX_TrackPlaying(it->second.track);
}

void FileBgmBackend::Release(const std::uint64_t id) noexcept
{
    if (impl_) impl_->playbacks.erase(id);
}

bool FileBgmBackend::IsAvailable() const noexcept
{
    if (!impl_ || impl_->deviceRemoved.load()) return false;
    SDL_AudioSpec spec {};
    // 默认设备可能保留失联逻辑流等待迁移，同时检查当前默认输出是否存在。
    return SDL_GetAudioDeviceFormat(impl_->device, &spec, nullptr)
        && SDL_GetAudioDeviceFormat(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr);
}

void FileBgmBackend::Close() noexcept { impl_.reset(); }

} // namespace cg::platform
