#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace cg::platform {

// 仅供一次性离线MIDI/DLS到WAV转换；不参与客户端编译或运行。
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
    std::uint64_t FrameCount() const noexcept; // 仅离线转换使用：完整序列帧数，保留轨道尾部时长。
    void Render(float* stereo, std::size_t frames) noexcept;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cg::platform
