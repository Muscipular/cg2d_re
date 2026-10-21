#include "WindowComponents.hpp"
#include "../script/LuaUI.hpp"
#include "cg/script/LuaRuntime.hpp"
#include <unordered_map>
#include <utility>

namespace cg::ui {
using namespace component_detail;

static std::vector<std::shared_ptr<WindowExtension>> windows;
static std::unordered_map<int, std::uint64_t> windowOrder;
static std::uint64_t orderSequence = 0;
static int focusedWindow = -1;
static int lastScene = -1, lastSceneState = -1;
static std::vector<std::pair<int, std::uint32_t>> pendingWindowOpens; // ID和实例代次，不持有窗口。


static std::uint32_t nextGeneration = 0;
static int dragOwner = -1;
static int mouseWheelDelta = 0;

int WindowPeekMouseWheelDelta() { return mouseWheelDelta; }
void WindowConsumeMouseWheelDelta() { mouseWheelDelta = 0; }
void WindowAddMouseWheelDelta(int delta) { mouseWheelDelta += delta; }

void WindowRuntime_EnsureIdentity(WindowInstance& window) {
    if (!window.generation) {
        window.generation = ++nextGeneration;
        window.refUpdate = window.refDraw = window.refEvent = -1;
    }
}
WindowInstance* WindowRuntime_FindInstance(int id) {
    if (id >= 0 && id < WIN_MAX) return gWindowState[id].window;
    const auto extension = GetExtWindowById(id);
    return extension ? &extension->window : nullptr;
}
WindowExtension::WindowExtension() {
    state.window = &window;
    WindowRuntime_EnsureIdentity(window);
}
void WindowRuntime_ReleaseBindings(WindowInstance& window) {
    WindowRuntime_CancelPointerDrag(window);
    delete window.binding;
    window.binding = nullptr;
    delete window.components;
    window.components = nullptr;
    delete window.properties;
    window.properties = nullptr;
}
WindowExtension::~WindowExtension() {
    if (dragOwner == window.id) dragOwner = -1;
    WindowRuntime_DestroyElements(window);
    WindowRuntime_ReleaseBindings(window);
}

void WindowRuntime_SetVisible(WindowInstance& window, bool visible) {
    if (!visible) WindowRuntime_CancelPointerDrag(window);
    if (window.id >= 0 && window.id < WIN_MAX) {
        window.scriptHidden = !visible;
        if (!visible) {
            WindowElementCursor cursor(window);
            while (auto* element = cursor.next()) {
                if (element->type == WindowElementType_Input && element->data.input
                    && gCurrentInputData == element->data.input->context) detail::WindowChat_RestoreInput();
            }
            if (gCurrentWindow == &window) WindowDrag_Cancel();
        }
        return;
    }
    if (visible) window.state |= WIN_STATE_ACTIVE;
    else {
        window.state &= ~WIN_STATE_ACTIVE;
        if (dragOwner == window.id) dragOwner = -1;
        if (auto extension = GetExtWindowById(window.id)) extension->dragMoving = false;
        WindowRuntime_ResetComponents(window);
    }
}
int WindowExtension::update(int, int hasFocused) {
    if ((window.state & WIN_STATE_CLOSE) || !(window.state & WIN_STATE_ACTIVE)) {
        if (dragOwner == window.id) dragOwner = -1;
        dragMoving = false;
        WindowRuntime_ResetComponents(window);
        return 1;
    }
    if ((window.state & WIN_STATE_CLOSE) || !(window.state & WIN_STATE_ACTIVE)) {
        WindowRuntime_ResetComponents(window);
        return 1;
    }
    if (window.components) window.components->publish();
    WindowRuntime_UpdateElementOrder(window);
    const int captured = WindowRuntime_ProcessComponents(window, hasFocused);
    if (!(window.state & WIN_STATE_CLOSE) && (window.state & WIN_STATE_ACTIVE)
        && dragMove == 1 && (gMouseBtnDown2 & platform::LegacyMouseButton_Left) && !WindowRuntime_PointerDragBlocksMouse()) {
        if (dragMoving && dragOwner == window.id) {
            window.x += gMouseX - dragMoveLastX; window.y += gMouseY - dragMoveLastY;
            dragMoveLastX = gMouseX; dragMoveLastY = gMouseY;
        } else if (dragMoving) dragMoving = false;
        else if (hasFocused && !captured && dragOwner == -1 && (gMouseClickState & platform::LegacyMouseEdge_LDown)) {
            dragOwner = window.id; dragMoving = true;
            dragMoveLastX = gMouseX; dragMoveLastY = gMouseY;
        }
    } else {
        if (dragOwner == window.id) dragOwner = -1;
        dragMoving = false;
    }
    if (window.components) window.components->publish();
    WindowRuntime_UpdateElementOrder(window);
    return 1;
}
int WindowExtension::render(int) {
    if ((window.state & WIN_STATE_CLOSE) || !(window.state & WIN_STATE_ACTIVE)) return 1;
    WindowRuntime_DrawCurrent();
    RendererBreakWindowTextBatch();
    return 1;
}

std::shared_ptr<WindowExtension> GetExtWindowById(int id)
{
    for (const auto& window : windows) if (window->window.id == id) return window;
    return nullptr;
}

void WindowExOpen(std::shared_ptr<WindowExtension>& window)
{
    WindowRuntime_EnsureIdentity(window->window);
    WindowRuntime_UpdateElementOrder(window->window);
    windows.push_back(window);
    windowOrder[window->window.id] = ++orderSequence;
    window->window.state |= WIN_STATE_FOCUS;
    WindowRuntime_QueueWindowOpen(window->window);
}

void WindowRuntime_QueueWindowOpen(const WindowInstance& window)
{
    pendingWindowOpens.emplace_back(window.id, window.generation);
}

void CgWindowClose(int id)
{
    if (id >= 0 && id < WIN_MAX) {
        if (auto* window = gWindowState[id].window) {
            WindowRuntime_CancelPointerDrag(*window);
            window->state |= WIN_STATE_CLOSE;
        }
    } else if (auto window = GetExtWindowById(id)) {
        // 回调期间仅标记；当前调度快照持有对象，下一帧统一释放。
        WindowRuntime_CancelPointerDrag(window->window);
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
        if ((window.state & WIN_STATE_CLOSE) || window.scriptHidden || IsDefaultWindow(window.id)) return;
        if (!target || IsWindowAbove(window, *target)) target = &window;
    };
    for (auto* window : ui::detail::gWindows) if (window) consider(*window);
    for (const auto& entry : windows)
        if (entry->window.state & WIN_STATE_ACTIVE) consider(entry->window);
    if (!target) return false;
    target->state |= WIN_STATE_CLOSE;
    return true;
}

void NativeWindowClosed(int id)
{
    windowOrder.erase(id);
    script::LuaProcessWinMgrWindowClose(id);
}

void WindowRuntime_UpdateScene()
{
    const int scene = world::gCurrentSceneType, state = world::gCurrentSceneState;
    if (scene != lastScene || state != lastSceneState) {
        lastScene = scene;
        lastSceneState = state;
        script::LuaProcessWinMgrSceneStateChanged(scene, state);
    }
}

void PrepareWindowFrame()
{
    for (auto* window : detail::gWindows) if (window) WindowRuntime_CollectElements(*window);
    for (const auto& entry : windows) WindowRuntime_CollectElements(entry->window);
    WindowRuntime_UpdateScene();
    // 先移除再通知，关闭事件可以安全重建同ID窗口。
    std::vector<int> closed;
    std::erase_if(windows, [&](const auto& entry) {
        if (!(entry->window.state & WIN_STATE_CLOSE)) return false;
        closed.push_back(entry->window.id);
        windowOrder.erase(entry->window.id);
        return true;
    });
    for (int id : closed) script::LuaProcessWinMgrWindowClose(id);

    // 移走当前批次；打开回调中新建的窗口留到下一次UI更新，避免递归派发。
    std::vector<std::pair<int, std::uint32_t>> opened;
    opened.swap(pendingWindowOpens);
    for (const auto& [id, generation] : opened) {
        const auto* window = WindowRuntime_FindInstance(id);
        if (window && window->generation == generation && !(window->state & WIN_STATE_CLOSE))
            script::LuaProcessWinMgrWindowOpen(id);
    }

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
        script::LuaProcessWinMgrWindowFocusChanged(nextFocus, previous);
    }
}

std::vector<ScheduledWindow> MergeWindowOrder(int nativeCount)
{
    std::vector<ScheduledWindow> result;
    for (int i = 0; i < nativeCount; ++i)
        if (!detail::gWindows[i]->scriptHidden) result.push_back({detail::gWindows[i]->id, detail::gWindows[i], {}});
    for (const auto& entry : windows) {
        if (!(entry->window.state & WIN_STATE_CLOSE) && (entry->window.state & WIN_STATE_ACTIVE))
            result.push_back({entry->window.id, &entry->window, entry});
    }
    std::stable_sort(result.begin(), result.end(), [](const auto& a, const auto& b) {
        return IsWindowAbove(*a.window, *b.window);
    });
    return result;
}

void FinishWindowFrame()
{
    script::LuaProcessWinMgrKeyPress();
    WindowRuntime_EndPointerDragFrame();
    WindowConsumeMouseWheelDelta();
    script::AutoBattleProc();
    script::CopilotProc();
}

void ShutdownExtensionWindows()
{
    pendingWindowOpens.clear();
    windows.clear(); // UI销毁组件/绑定后，调用方才可关闭Lua state。
    dragOwner = -1;
    mouseWheelDelta = 0;
    for (auto* window : detail::gWindows) if (window) {
        delete window->binding;
        window->binding = nullptr;
    }
    windowOrder.clear();
    orderSequence = 0;
    focusedWindow = -1;
    lastScene = lastSceneState = -1;
}

} // namespace cg::ui
