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
    ui::WindowRuntime_UpdateElementOrder(window->window);
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

// 原生窗口统一使用kWindowCategories，Lua扩展窗口没有原生类别。
static bool IsDefaultWindow(int id)
{
    return id >= 0 && id < WIN_MAX && ui::WindowRuntime_GetCategory(id) == 0;
}

void CloseNonDefaultWindows(int exceptId)
{
    const auto close = [exceptId](UIWindowInstance& window) {
        if (window.id != exceptId && !IsDefaultWindow(window.id))
            window.state |= WIN_STATE_CLOSE;
    };
    for (auto* window : ui::detail::gWindows) if (window) close(*window);
    for (const auto& entry : windows) close(entry->window);
}

// 层号较小的窗口在前，同层按既有置前次序；关闭与绘制使用相同规则。
static bool IsWindowAbove(const UIWindowInstance& first, const UIWindowInstance& second)
{
    if (first.layer != second.layer) return first.layer < second.layer;
    return windowOrder[first.id] > windowOrder[second.id];
}

bool CloseTopmostNonDefaultWindow()
{
    UIWindowInstance* target = nullptr;
    const auto consider = [&](UIWindowInstance& window) {
        if ((window.state & WIN_STATE_CLOSE) || IsDefaultWindow(window.id)) return;
        if (!target || IsWindowAbove(window, *target)) target = &window;
    };
    for (auto* window : ui::detail::gWindows) if (window) consider(*window);
    for (const auto& entry : windows)
        if (entry->window.state & WIN_STATE_ACTIVE) consider(entry->window);
    if (!target) return false;
    target->state |= WIN_STATE_CLOSE;
    return true;
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
        return IsWindowAbove(*a.window, *b.window);
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
