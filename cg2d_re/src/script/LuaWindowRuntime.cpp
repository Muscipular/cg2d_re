#include "LuaUI.hpp"
#include "cg/script/LuaRuntime.hpp"
#include <unordered_map>

namespace cg::script {

static std::vector<std::shared_ptr<UIWindowInstanceEx>> windows;
static std::unordered_map<int, std::uint64_t> windowOrder;
static std::uint64_t orderSequence = 0;
static int focusedWindow = -1;
static int lastScene = -1, lastSceneState = -1;

std::shared_ptr<UIWindowInstanceEx> GetExtWindowById(int id)
{
    for (const auto& window : windows) if (window->window.id == id) return window;
    return nullptr;
}

void WindowExOpen(std::shared_ptr<UIWindowInstanceEx>& window)
{
    windows.push_back(window);
    windowOrder[window->window.id] = ++orderSequence;
    window->window.state |= WIN_STATE_FOCUS;
}

void CgWindowClose(int id)
{
    if (id >= 0 && id < WIN_MAX) {
        if (auto* window = gWindowState[id].window) window->state |= WIN_STATE_CLOSE;
    } else if (auto window = GetExtWindowById(id)) {
        // 回调期间仅标记；当前调度快照持有对象，下一帧统一释放。
        window->window.state |= WIN_STATE_CLOSE;
    }
}

void NativeLuaWindowClosed(int id)
{
    windowOrder.erase(id);
    LuaProcessWinMgrWindowClose(id);
}

void UpdateLuaScene()
{
    const int scene = world::gCurrentSceneType, state = world::gCurrentSceneState;
    if (scene != lastScene || state != lastSceneState) {
        lastScene = scene;
        lastSceneState = state;
        LuaProcessWinMgrSceneStateChanged(scene, state);
    }
}

void PrepareLuaWindowFrame()
{
    UpdateLuaScene();
    // 先移除再通知，关闭事件可以安全重建同ID窗口。
    std::vector<int> closed;
    std::erase_if(windows, [&](const auto& entry) {
        if (!(entry->window.state & WIN_STATE_CLOSE)) return false;
        closed.push_back(entry->window.id);
        windowOrder.erase(entry->window.id);
        return true;
    });
    for (int id : closed) LuaProcessWinMgrWindowClose(id);

    int nextFocus = -1;
    const auto track = [&](UIWindowInstance& window) {
        if (window.state & WIN_STATE_CLOSE) return;
        if (!windowOrder.contains(window.id)) windowOrder[window.id] = ++orderSequence;
        if (window.state & WIN_STATE_FOCUS) {
            windowOrder[window.id] = ++orderSequence;
            nextFocus = window.id;
        }
    };
    for (auto* window : ui::detail::gWindows) if (window) track(*window);
    const auto current = windows;
    for (const auto& entry : current) {
        track(entry->window);
        entry->window.state &= ~WIN_STATE_FOCUS;
    }
    if (nextFocus >= 0 && focusedWindow != nextFocus) {
        const int previous = focusedWindow;
        focusedWindow = nextFocus;
        LuaProcessWinMgrWindowFocusChanged(nextFocus, previous);
    }
}

std::vector<ScheduledLuaWindow> MergeLuaWindowOrder(int nativeCount)
{
    std::vector<ScheduledLuaWindow> result;
    for (int i = 0; i < nativeCount; ++i) result.push_back({ui::detail::gWindows[i]->id, ui::detail::gWindows[i], {}});
    for (const auto& entry : windows) {
        if (!(entry->window.state & WIN_STATE_CLOSE) && (entry->window.state & WIN_STATE_ACTIVE))
            result.push_back({entry->window.id, &entry->window, entry});
    }
    std::stable_sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        if (a.window->layer != b.window->layer) return a.window->layer < b.window->layer;
        return windowOrder[a.window->id] > windowOrder[b.window->id];
    });
    return result;
}

void FinishLuaWindowFrame()
{
    LuaProcessWinMgrKeyPress();
    LuaConsumeMouseWheelDelta();
    for (const auto& entry : windows) {
        if (entry->window.id > WIN_NORMAL_END)
            static_cast<LuaWindow*>(entry.get())->retiredControls.clear();
    }
    AutoBattleProc();
    CopilotProc();
}

void ShutdownLuaUI()
{
    ShutdownLuaUIEvents();
    windows.clear(); // 先归还输入上下文/释放纹理和registry，再由LuaRuntime关闭state。
    windowOrder.clear();
    orderSequence = 0;
    focusedWindow = -1;
    lastScene = lastSceneState = -1;
}

} // namespace cg::script
