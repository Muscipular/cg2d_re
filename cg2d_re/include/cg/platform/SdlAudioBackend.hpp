#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

namespace cg::platform {

// 原音频设备与样本缓冲的 SDL3 承载；上层仍负责原函数的分支和写序。
// SDL 设备、样本格式与数据只在实现文件中持有，不暴露 SDL 指针。
class SdlAudioBackend final {
public:
    static constexpr std::size_t kSampleSlots = 500;
    static constexpr std::size_t kChannelCount = 64;

    SdlAudioBackend() noexcept;
    ~SdlAudioBackend();

    SdlAudioBackend(const SdlAudioBackend&) = delete;
    SdlAudioBackend& operator=(const SdlAudioBackend&) = delete;
    SdlAudioBackend(SdlAudioBackend&&) = delete;
    SdlAudioBackend& operator=(SdlAudioBackend&&) = delete;

    // 获取本对象独占的默认逻辑播放设备；失败不保留 AUDIO 子系统引用。
    [[nodiscard]] bool Open() noexcept;
    [[nodiscard]] bool IsOpen() const noexcept;

    // path 是现代 SDL 文件边界的 UTF-8 路径；旧原字节路径须由调用者另行转换。
    // 保留各 WAV 自身的格式和 PCM 数据；失败不改已有槽内容。
    [[nodiscard]] bool LoadWav(std::size_t slot, const char* path) noexcept;
    [[nodiscard]] bool HasSample(std::size_t slot) const noexcept;

    // 复制样本引用到独立通道；调用者在重绑前须先释放旧通道。
    [[nodiscard]] bool PrepareChannel(std::size_t channel, std::size_t sample) noexcept;
    [[nodiscard]] bool HasChannel(std::size_t channel) const noexcept;
    [[nodiscard]] bool StopChannel(std::size_t channel) noexcept;
    [[nodiscard]] bool RewindChannel(std::size_t channel) noexcept;
    void ReleaseChannel(std::size_t channel) noexcept;

    // DirectSound 原单位：百分之一分贝与绝对 Hz；调用方决定是否忽略失败。
    [[nodiscard]] bool SetChannelVolume(std::size_t channel, std::int32_t hundredthDb) noexcept;
    [[nodiscard]] bool SetChannelPan(std::size_t channel, std::int32_t hundredthDb) noexcept;
    [[nodiscard]] bool SetChannelFrequency(std::size_t channel, std::uint32_t hz) noexcept;
    // 从当前帧开始；原 SetCurrentPosition(0) 由 RewindChannel 单独承接。
    [[nodiscard]] bool PlayChannel(std::size_t channel, bool loop) noexcept;

    // 先停止并释放通道，再释放全部样本、设备及本对象持有的 AUDIO 引用。
    // 不得从本对象的 SDL 音频回调中关闭或销毁。
    void Close() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cg::platform
