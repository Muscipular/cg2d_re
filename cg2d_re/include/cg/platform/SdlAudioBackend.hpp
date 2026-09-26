#pragma once

#include <cstddef>
#include <memory>

namespace cg::platform {

// 原音频设备与样本缓冲的 SDL3 承载；上层仍负责原函数的分支和写序。
// SDL 设备、样本格式与数据只在实现文件中持有，不暴露 SDL 指针。
class SdlAudioBackend final {
public:
    static constexpr std::size_t kSampleSlots = 500;

    SdlAudioBackend() noexcept;
    ~SdlAudioBackend();

    SdlAudioBackend(const SdlAudioBackend&) = delete;
    SdlAudioBackend& operator=(const SdlAudioBackend&) = delete;
    SdlAudioBackend(SdlAudioBackend&&) = delete;
    SdlAudioBackend& operator=(SdlAudioBackend&&) = delete;

    // 获取本对象独占的默认逻辑播放设备；失败不保留 AUDIO 子系统引用。
    [[nodiscard]] bool Open() noexcept;

    // path 是现代 SDL 文件边界的 UTF-8 路径；旧原字节路径须由调用者另行转换。
    // 保留各 WAV 自身的格式和 PCM 数据；失败不改已有槽内容。
    [[nodiscard]] bool LoadWav(std::size_t slot, const char* path) noexcept;
    [[nodiscard]] bool HasSample(std::size_t slot) const noexcept;

    // 先释放全部样本，再关闭逻辑设备及本对象持有的 AUDIO 引用。
    void Close() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cg::platform
