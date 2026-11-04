#include "cg/audio/FileBgm.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/FileBgmBackend.hpp"
#include "cg/core/m_log.hpp"
#include "cg/session/ClientHost.hpp"

#include <cwchar>
#include <filesystem>
#include <limits>
#include <map>

namespace cg::audio {

struct FileBgmRecord {
    session::ClientSession* session; // 实际track的稳定owner；移除会话前须释放。
    FileBgmMode mode;
    FileBgmOwner owner;
    FileBgmState state {FileBgmState::Playing};
    float gain = 0.0f;
};

static platform::FileBgmBackend gFileBgmBackend;
static std::map<FileBgmHandle, FileBgmRecord> gFileBgms;
static FileBgmHandle gNextFileBgmHandle = 1;
static bool gSceneBgmRestorePending = false; // 输出设置失败时由后续主线程更新重试。

static void RestoreSceneBgm() noexcept
{
    gSceneBgmRestorePending = !SetSceneBgmSuppressed(false);
    if (gSceneBgmRestorePending) m_log_iw(L"[Lua BGM] 无法恢复场景音乐增益，将重试\n");
}

// 仅当前前台身份释放场景抑制；旧handle的Stop/GC不会解除后来者。
static void EndPlayback(session::ClientSession& session, const FileBgmHandle handle,
    const FileBgmState state) noexcept
{
    const auto it = gFileBgms.find(handle);
    if (it == gFileBgms.end() || it->second.session != &session
        || it->second.state != FileBgmState::Playing) return;
    gFileBgmBackend.Release(handle);
    it->second.state = state;
    if (session.Audio().foregroundFileBgm == handle) {
        session.Audio().foregroundFileBgm = 0;
        if (cg::session::GetClientHost().CurrentSession() == &session) RestoreSceneBgm();
    }
}

// 先处理所有切出track，避免恢复新会话增益时仍混入旧会话。
static void MuteBackgroundFileBgms() noexcept
{
    const auto* const current = cg::session::GetClientHost().CurrentSession();
    for (auto& [id, record] : gFileBgms) {
        if (record.state != FileBgmState::Playing || record.session == current || record.gain == 0.0f)
            continue;
        if (!gFileBgmBackend.SetGain(id, 0.0f)) {
            m_log_iw(L"[Lua BGM] 后台静音失败，停止对应文件音乐\n");
            EndPlayback(*record.session, id, FileBgmState::Stopped);
        } else record.gain = 0.0f;
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

FileBgmResult PlayFileBgm(session::ClientSession& session, const std::wstring_view path, const FileBgmMode mode,
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
    const bool audible = cg::session::GetClientHost().CurrentSession() == &session;
    const auto gain = audible ? GetFileBgmGain() : 0.0f;
    try {
        gFileBgms.emplace(id, FileBgmRecord {&session, mode, owner, FileBgmState::Playing, gain});
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
    const auto previous = session.Audio().foregroundFileBgm;
    if (foreground && audible && !SetSceneBgmSuppressed(true)) {
        gFileBgmBackend.Release(id);
        gFileBgms.erase(id);
        result.error = "Cannot suppress scene BGM";
        return result;
    }
    if (!gFileBgmBackend.SetGain(id, gain)) {
        gFileBgmBackend.Release(id);
        gFileBgms.erase(id);
        if (foreground && audible && !previous) RestoreSceneBgm();
        result.error = "Cannot set file BGM gain";
        return result;
    }
    if (foreground) {
        // 先发布新owner再销毁旧track，旧身份不能解除当前抑制。
        session.Audio().foregroundFileBgm = id;
        if (audible) gSceneBgmRestorePending = false;
        EndPlayback(session, previous, FileBgmState::Replaced);
    }
    result.handle = id;
    return result;
}

void StopFileBgm(session::ClientSession& session, const FileBgmHandle handle) noexcept
{
    EndPlayback(session, handle, FileBgmState::Stopped);
}

FileBgmState GetFileBgmState(session::ClientSession& session, const FileBgmHandle handle) noexcept
{
    UpdateFileBgm();
    const auto it = gFileBgms.find(handle);
    return it != gFileBgms.end() && it->second.session == &session
        ? it->second.state : FileBgmState::Stopped;
}

void ForgetFileBgm(session::ClientSession& session, const FileBgmHandle handle) noexcept
{
    StopFileBgm(session, handle);
    const auto it = gFileBgms.find(handle);
    if (it != gFileBgms.end() && it->second.session == &session) gFileBgms.erase(it);
}

void UpdateFileBgm() noexcept
{
    MuteBackgroundFileBgms();
    auto* const current = cg::session::GetClientHost().CurrentSession();
    if (gSceneBgmRestorePending && (current == nullptr || current->Audio().foregroundFileBgm == 0))
        gSceneBgmRestorePending = !SetSceneBgmSuppressed(false);
    const auto gain = GetFileBgmGain();
    const bool available = IsFileBgmAudioAvailable() && gFileBgmBackend.IsAvailable();
    for (auto& [id, record] : gFileBgms) {
        if (record.state != FileBgmState::Playing) continue;
        if (!available) { EndPlayback(*record.session, id, FileBgmState::Stopped); continue; }
        if (!gFileBgmBackend.IsPlaying(id)) { EndPlayback(*record.session, id, FileBgmState::Finished); continue; }
        const auto targetGain = record.session == current ? gain : 0.0f;
        if (record.gain != targetGain) {
            if (!gFileBgmBackend.SetGain(id, targetGain)) {
                m_log_iw(L"[Lua BGM] 音量同步失败，停止对应文件音乐\n");
                EndPlayback(*record.session, id, FileBgmState::Stopped);
            } else record.gain = targetGain;
        }
    }
    if (!available) gFileBgmBackend.Close();
}

void SelectFileBgmOutput() noexcept
{
    MuteBackgroundFileBgms();
    auto* const current = cg::session::GetClientHost().CurrentSession();
    const FileBgmHandle foreground = current != nullptr ? current->Audio().foregroundFileBgm : 0;
    if (foreground != 0) {
        if (!SetSceneBgmSuppressed(true)) {
            m_log_iw(L"[Lua BGM] 切换时无法抑制场景音乐，停止对应文件音乐\n");
            EndPlayback(*current, foreground, FileBgmState::Stopped);
        } else gSceneBgmRestorePending = false;
    } else RestoreSceneBgm();
    UpdateFileBgm();
}

void OnSceneBgmPlayback(session::ClientSession& session) noexcept
{
    const auto it = gFileBgms.find(session.Audio().foregroundFileBgm);
    if (it != gFileBgms.end() && it->second.mode == FileBgmMode::Replace)
        EndPlayback(session, it->first, FileBgmState::Replaced);
}

void ShutdownFileBgm(session::ClientSession& session) noexcept
{
    for (auto it = gFileBgms.begin(); it != gFileBgms.end();) {
        if (it->second.session != &session) { ++it; continue; }
        gFileBgmBackend.Release(it->first);
        it = gFileBgms.erase(it);
    }
    if (cg::session::GetClientHost().CurrentSession() == &session
        && (session.Audio().foregroundFileBgm || gSceneBgmRestorePending)) RestoreSceneBgm();
    session.Audio().foregroundFileBgm = 0;
    if (gFileBgms.empty()) gFileBgmBackend.Close();
}

void ShutdownAllFileBgm() noexcept
{
    gFileBgmBackend.Close();
    bool hadForeground = false;
    for (auto& [id, record] : gFileBgms) {
        if (record.session->Audio().foregroundFileBgm == id) {
            record.session->Audio().foregroundFileBgm = 0;
            hadForeground = true;
        }
    }
    gFileBgms.clear();
    if (hadForeground || gSceneBgmRestorePending) RestoreSceneBgm();
    // 不重置身份代次；再次创建Lua状态也不会复用旧handle。
}

} // namespace cg::audio
