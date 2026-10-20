#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace cg::platform {

// LBG01：主线程拥有独立mixer设备；SDL/MIX类型不越过本边界。
class FileBgmBackend final {
public:
    FileBgmBackend() noexcept;
    ~FileBgmBackend();
    FileBgmBackend(const FileBgmBackend&) = delete;
    FileBgmBackend& operator=(const FileBgmBackend&) = delete;

    // 新身份先以零增益启动，业务层完成前台仲裁后再设置实际增益。
    // error为UTF-8；原文件缓存、按需解码，忽略内嵌循环点。
    [[nodiscard]] bool Start(std::uint64_t id, const wchar_t* path, bool loop, std::string& error);
    [[nodiscard]] bool SetGain(std::uint64_t id, float gain) noexcept;
    [[nodiscard]] bool IsPlaying(std::uint64_t id) const noexcept;
    [[nodiscard]] bool IsAvailable() const noexcept; // 设备失效由主线程统一回收。
    void Release(std::uint64_t id) noexcept;
    void Close() noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace cg::platform
