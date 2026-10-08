#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace cg::platform {

// 原455030 Waves效果的PCM提供者；SDK对象只在独立线程内持有/调用/释放。
class LegacyWavesReverb final {
public:
    LegacyWavesReverb() noexcept;
    ~LegacyWavesReverb();
    LegacyWavesReverb(const LegacyWavesReverb&) = delete;
    LegacyWavesReverb& operator=(const LegacyWavesReverb&) = delete;

    bool Open(int sampleRate) noexcept; // 固定WAV已静止时调用，不打开播放设备。
    bool Configure(std::int32_t decay, std::int32_t mix) noexcept; // 原两个S32参数，转换保持原序。
    bool Reset() noexcept; // 清本段效果历史，保留当前参数。
    bool Process(std::span<float> stereo, std::uint64_t startFrame,
        bool zeroInput, bool& hasTail) noexcept; // 同步借PCM，返回前工作线程已停止使用该缓冲。
    void Close() noexcept; // 音频流回调退出后调用，join后释放线程/信号。

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cg::platform
