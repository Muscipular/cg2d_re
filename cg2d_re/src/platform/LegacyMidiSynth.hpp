#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace cg::platform {

// 固定音乐的MIDI/DLS到PCM边界；样本和声音状态独占，实际输出由SdlAudioBackend承接。
class LegacyMidiSynth final {
public:
    static constexpr int kSampleRate = 22050; // 原454C30端口采样率。
    LegacyMidiSynth() noexcept;
    ~LegacyMidiSynth();
    bool LoadBank(const char* utf8Path) noexcept;
    bool LoadSequence(std::span<const std::uint8_t> bytes) noexcept;
    void Start(std::uint32_t repeats) noexcept;
    void Stop() noexcept;
    void UnloadSequence() noexcept; // 原旧segment释放；保留默认音色bank。
    void Clear() noexcept; // 关闭音乐后端时释放序列、区域及全部样本。
    bool IsPlaying() const noexcept;
    void Render(float* stereo, std::size_t frames) noexcept;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cg::platform
