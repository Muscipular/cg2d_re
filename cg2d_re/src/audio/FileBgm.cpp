#include "cg/audio/FileBgm.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/FileBgmBackend.hpp"
#include "cg/core/m_log.hpp"

#include <cwchar>
#include <filesystem>
#include <limits>
#include <map>

namespace cg::audio {

struct FileBgmRecord {
    FileBgmMode mode;
    FileBgmOwner owner;
    FileBgmState state {FileBgmState::Playing};
    float gain {};
};

static platform::FileBgmBackend gFileBgmBackend;
static std::map<FileBgmHandle, FileBgmRecord> gFileBgms;
static FileBgmHandle gNextFileBgmHandle = 1;
static FileBgmHandle gForegroundFileBgm = 0;
static bool gSceneBgmRestorePending = false; // 输出设置失败时由后续主线程更新重试。

static void RestoreSceneBgm() noexcept
{
    gSceneBgmRestorePending = !SetSceneBgmSuppressed(false);
    if (gSceneBgmRestorePending) m_log_iw(L"[Lua BGM] 无法恢复场景音乐增益，将重试\n");
}

// 仅当前前台身份释放场景抑制；旧handle的Stop/GC不会解除后来者。
static void EndPlayback(const FileBgmHandle handle, const FileBgmState state) noexcept
{
    const auto it = gFileBgms.find(handle);
    if (it == gFileBgms.end() || it->second.state != FileBgmState::Playing) return;
    gFileBgmBackend.Release(handle);
    it->second.state = state;
    if (gForegroundFileBgm == handle) {
        gForegroundFileBgm = 0;
        RestoreSceneBgm();
    }
}

// canonical同时解析目录联接/符号链接；逐组件比较避免相似前缀逃出资源根。
static bool ResolveAudioPath(const std::wstring_view path, std::filesystem::path& resolved,
    std::string& error)
{
    if (path.empty() || path.find(L'\0') != path.npos || path.find(L':') != path.npos) {
        error = "BGM path must be a local relative path";
        return false;
    }
    const std::filesystem::path relative(path);
    if (relative.has_root_path()) { error = "Absolute BGM paths are not allowed"; return false; }
    std::error_code ec;
    const auto root = std::filesystem::canonical(L"luaUI/audio", ec);
    if (ec) { error = "Cannot open luaUI/audio directory"; return false; }
    resolved = std::filesystem::canonical(root / relative, ec);
    if (ec) { error = "Cannot find BGM file"; return false; }
    auto candidate = resolved.begin();
    for (const auto& component : root) {
        if (candidate == resolved.end() || ::_wcsicmp(component.c_str(), candidate->c_str()) != 0) {
            error = "BGM path escapes luaUI/audio";
            return false;
        }
        ++candidate;
    }
    if (!std::filesystem::is_regular_file(resolved, ec) || ec) {
        error = "BGM path is not a readable regular file";
        return false;
    }
    return true;
}

FileBgmResult PlayFileBgm(const std::wstring_view path, const FileBgmMode mode,
    const bool loop, const FileBgmOwner owner)
{
    FileBgmResult result;
    if (!IsFileBgmAudioAvailable()) { result.error = "Audio device is not ready"; return result; }
    std::filesystem::path resolved;
    if (!ResolveAudioPath(path, resolved, result.error)) return result;
    if (gNextFileBgmHandle == (std::numeric_limits<FileBgmHandle>::max)()) {
        result.error = "BGM playback identities exhausted";
        return result;
    }
    const auto id = gNextFileBgmHandle++;
    const auto gain = GetFileBgmGain();
    try {
        gFileBgms.emplace(id, FileBgmRecord {mode, owner, FileBgmState::Playing, gain});
        if (!gFileBgmBackend.Start(id, resolved.c_str(), loop, result.error)) {
            gFileBgmBackend.Release(id);
            gFileBgms.erase(id);
            return result;
        }
    } catch (...) {
        gFileBgmBackend.Release(id);
        gFileBgms.erase(id);
        throw;
    }
    const bool foreground = mode != FileBgmMode::Independent;
    const auto previous = gForegroundFileBgm;
    if (foreground && !SetSceneBgmSuppressed(true)) {
        gFileBgmBackend.Release(id);
        gFileBgms.erase(id);
        result.error = "Cannot suppress scene BGM";
        return result;
    }
    if (!gFileBgmBackend.SetGain(id, gain)) {
        gFileBgmBackend.Release(id);
        gFileBgms.erase(id);
        if (foreground && !previous) RestoreSceneBgm();
        result.error = "Cannot set file BGM gain";
        return result;
    }
    if (foreground) {
        // 先发布新owner再销毁旧track，旧身份不能解除当前抑制。
        gForegroundFileBgm = id;
        gSceneBgmRestorePending = false;
        EndPlayback(previous, FileBgmState::Replaced);
    }
    result.handle = id;
    return result;
}

void StopFileBgm(const FileBgmHandle handle) noexcept { EndPlayback(handle, FileBgmState::Stopped); }

FileBgmState GetFileBgmState(const FileBgmHandle handle) noexcept
{
    UpdateFileBgm();
    const auto it = gFileBgms.find(handle);
    return it != gFileBgms.end() ? it->second.state : FileBgmState::Stopped;
}

void ForgetFileBgm(const FileBgmHandle handle) noexcept
{
    StopFileBgm(handle);
    gFileBgms.erase(handle);
}

void UpdateFileBgm() noexcept
{
    if (gSceneBgmRestorePending && !gForegroundFileBgm)
        gSceneBgmRestorePending = !SetSceneBgmSuppressed(false);
    const auto gain = GetFileBgmGain();
    const bool available = IsFileBgmAudioAvailable() && gFileBgmBackend.IsAvailable();
    for (auto& [id, record] : gFileBgms) {
        if (record.state != FileBgmState::Playing) continue;
        if (!available) { EndPlayback(id, FileBgmState::Stopped); continue; }
        if (!gFileBgmBackend.IsPlaying(id)) { EndPlayback(id, FileBgmState::Finished); continue; }
        if (record.gain != gain) {
            if (!gFileBgmBackend.SetGain(id, gain)) {
                m_log_iw(L"[Lua BGM] 音量同步失败，停止对应文件音乐\n");
                EndPlayback(id, FileBgmState::Stopped);
            } else record.gain = gain;
        }
    }
    if (!available) gFileBgmBackend.Close();
}

void OnSceneBgmPlayback() noexcept
{
    const auto it = gFileBgms.find(gForegroundFileBgm);
    if (it != gFileBgms.end() && it->second.mode == FileBgmMode::Replace)
        EndPlayback(it->first, FileBgmState::Replaced);
}

void ShutdownFileBgm() noexcept
{
    gFileBgmBackend.Close();
    gFileBgms.clear();
    if (gForegroundFileBgm || gSceneBgmRestorePending) RestoreSceneBgm();
    gForegroundFileBgm = 0;
    // 不重置身份代次；再次创建Lua状态也不会复用旧handle。
}

} // namespace cg::audio
