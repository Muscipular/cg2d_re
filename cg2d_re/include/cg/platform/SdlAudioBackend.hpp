#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

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

    // 原固定音乐预转换的内置WAV，独立于普通BGM与64音效通道；初始化复制并独占PCM。
    // 关联原入口：0x00454FE0／0x00455100，固定WAV预载承接旧音色库装载／下载职责。
    [[nodiscard]] bool InitializeFixedMusic(std::span<const std::uint8_t> wav) noexcept;
    [[nodiscard]] bool HasFixedMusic() const noexcept;
    [[nodiscard]] bool PrepareFixedMusic() noexcept;
    [[nodiscard]] bool HasFixedMusicPlayback() const noexcept; // 对应原已加载片段，区别于初始化的固定流。
    [[nodiscard]] bool SetFixedMusicRepeats(std::uint32_t repeats) noexcept; // 原W32额外重复次数；全1为无限。
    [[nodiscard]] bool PlayFixedMusic() noexcept; // 重复由上一阶段设置，本阶段只启动实际播放。
    [[nodiscard]] bool SetFixedMusicVolume(std::int32_t hundredthDb) noexcept;
    [[nodiscard]] bool SetFixedMusicReverb(std::int32_t decay, std::int32_t mix) noexcept; // 原455030两个S32。
    void StopFixedMusic() noexcept;
    void ReleaseFixedMusicPlayback() noexcept;
    // 关联原入口：0x00455260，旧装载器释放职责归并到固定WAV清理。
    void ShutdownFixedMusic() noexcept;

    // path 为UTF-16路径；SDL要求的编码转换仅在实现中的文件打开边界完成。
    // 保留各 WAV 自身的格式和 PCM 数据；失败不改已有槽内容。
    [[nodiscard]] bool LoadWav(std::size_t slot, const wchar_t* path) noexcept;
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

    // BGM 独立于 64 个音效通道。endFrame 为 0 时使用 WAV 完整帧数；
    // 非零时按原 smpl 端点作为排他循环终点。
    [[nodiscard]] bool LoadBgmWav(
        const wchar_t* path, std::size_t startFrame, std::size_t endFrame) noexcept;
    // 原52BE60在流式首段预填/ready之后发布smpl终点；不清空已排队PCM或重置游标。
    [[nodiscard]] bool SetBgmLoopEnd(std::size_t endFrame) noexcept;
    [[nodiscard]] std::uint32_t ReadBgmDataLength() const noexcept; // 原流式创建先发布实际读取的PCM字节数。
    [[nodiscard]] bool HasBgm() const noexcept;
    [[nodiscard]] bool SetBgmVolume(std::int32_t hundredthDb) noexcept;
    [[nodiscard]] bool SetBgmFrequency(std::uint32_t hz) noexcept;
    [[nodiscard]] bool PlayBgm(bool loop) noexcept;
    [[nodiscard]] bool StopBgm() noexcept;
    // 流式 BGM 的独立线程在 SDL 回调退出临界区后写唯一停止通知。
    // 由上层流式创建入口按原时点启动；失败不产生通知。
    [[nodiscard]] bool StartBgmStopWorker(
        std::atomic<std::uint8_t>& notice,
        std::atomic<std::uint32_t>& ready) noexcept;
    [[nodiscard]] bool RequestBgmStop() noexcept;
    void ReleaseBgm() noexcept;

    // 先释放 BGM 与音效流，再释放全部样本、设备及本对象持有的 AUDIO 引用。
    // 不得从本对象的 SDL 音频回调中关闭或销毁。
    void Close() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cg::platform
