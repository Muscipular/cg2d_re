#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace cg::audio {

// LBG01：replace/takeover共享前台，independent各自并行；详见文件BGM SPEC。
enum class FileBgmMode { Replace, Takeover, Independent };
enum class FileBgmState { Playing, Stopped, Finished, Replaced };
enum class FileBgmOwner { Ui, Battle };

// 单调递增身份兼作代次，不复用旧handle；0表示无播放。
using FileBgmHandle = std::uint64_t;
struct FileBgmResult {
    FileBgmHandle handle {};
    std::string error; // UTF-8，脚本边界转换为GBK。
};

// 只接受luaUI/audio根内相对路径；均在主线程调用。
[[nodiscard]] FileBgmResult PlayFileBgm(std::wstring_view path, FileBgmMode mode,
    bool loop, FileBgmOwner owner);
void StopFileBgm(FileBgmHandle handle) noexcept;
[[nodiscard]] FileBgmState GetFileBgmState(FileBgmHandle handle) noexcept;
void ForgetFileBgm(FileBgmHandle handle) noexcept; // Lua GC：停止并移除轻量状态。
void UpdateFileBgm() noexcept; // 主线程轮询结束并同步音乐开关/音量。
void OnSceneBgmPlayback() noexcept; // 旧入口通过门禁后的实际切曲/停止。
void ShutdownFileBgm() noexcept; // Lua关闭和旧音频关闭均可幂等调用。

} // namespace cg::audio
