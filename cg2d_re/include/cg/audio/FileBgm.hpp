#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace cg::session { class ClientSession; }

namespace cg::audio {

// LBG01：replace/takeover共享前台，independent各自并行；详见文件BGM SPEC。
enum class FileBgmMode { Replace, Takeover, Independent };
enum class FileBgmState { Playing, Stopped, Finished, Replaced };
enum class FileBgmOwner { Ui, Battle };

// 单调递增身份兼作代次，不复用旧handle；0表示无播放。
using FileBgmHandle = std::uint64_t;
struct FileBgmResult {
    FileBgmHandle handle = 0;
    std::string error; // UTF-8，脚本边界转换为GBK。
};

// 只接受luaUI/audio根内相对路径；均在主线程调用。
[[nodiscard]] FileBgmResult PlayFileBgm(session::ClientSession& session, std::wstring_view path, FileBgmMode mode,
    bool loop, FileBgmOwner owner);
void StopFileBgm(session::ClientSession& session, FileBgmHandle handle) noexcept;
[[nodiscard]] FileBgmState GetFileBgmState(session::ClientSession& session, FileBgmHandle handle) noexcept;
void ForgetFileBgm(session::ClientSession& session, FileBgmHandle handle) noexcept; // Lua GC：只清所属身份。
void UpdateFileBgm() noexcept; // 进程音频帧轮询实际track，后台增益保持零。
void SelectFileBgmOutput() noexcept; // 宿主发布当前项后，释放旧音效、静音切出音乐，再恢复当前输出。
void OnSceneBgmPlayback(session::ClientSession& session) noexcept;
void ShutdownFileBgm(session::ClientSession& session) noexcept; // Lua关闭只释放所属记录。
void ShutdownAllFileBgm() noexcept; // 进程音频退出时统一关闭设备。

} // namespace cg::audio
