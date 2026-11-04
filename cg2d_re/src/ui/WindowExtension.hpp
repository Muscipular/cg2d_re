#pragma once
#include "WindowRuntimeState.hpp"
#include <memory>
#include <vector>

namespace cg::ui {

// LUP01：外部语言仅绑定事件，不拥有窗口；窗口释放时先解除绑定。
struct WindowBinding {
    virtual ~WindowBinding() = default;
    virtual void update() {}
    virtual void draw() {}
    virtual int event(int, int) { return 0; }
    virtual void elementUpdate(WindowElement&) {}
    virtual void elementDraw(WindowElement&) {}
    virtual int elementEvent(WindowElement&, int, int) { return 0; }
    virtual int elementText(WindowElement&, bool, std::wstring_view) { return 0; } // 文本变更/回车统一从窗口绑定进入脚本。
};

// 与旧原生窗口共用WindowInstance和帧调度，动态窗口由UI管理器拥有。
struct WindowExtension {
    WindowInstance window{};
    detail::WindowState state{};
    int dragMove = 0;
    bool dragMoving = false;
    int dragMoveLastX = 0;
    int dragMoveLastY = 0;
    explicit WindowExtension(session::ClientSession& session);
    virtual ~WindowExtension();
    virtual int render(int stage);
    virtual int update(int stage, int hasFocused);
};

struct ScheduledWindow {
    int id;
    WindowInstance* window;
    std::shared_ptr<WindowExtension> extension; // 当帧租借，避免回调期间释放窗口。
};

int WindowPeekMouseWheelDelta(session::ClientSession& session);
void WindowConsumeMouseWheelDelta(session::ClientSession& session);
void WindowAddMouseWheelDelta(session::ClientSession& session, int delta);
void WindowRuntime_EnsureIdentity(WindowInstance& window);
WindowInstance* WindowRuntime_FindInstance(session::ClientSession& session, int id);
std::shared_ptr<WindowExtension> GetExtWindowById(session::ClientSession& session, int id);
void WindowExOpen(std::shared_ptr<WindowExtension>& window);
// 创建成功后排队，在下一次UI更新中按实例代次派发打开事件。
void WindowRuntime_QueueWindowOpen(const WindowInstance& window);
void CgWindowClose(session::ClientSession& session, int id);
void CloseNonDefaultWindows(session::ClientSession& session, int exceptId);
bool CloseTopmostNonDefaultWindow(session::ClientSession& session);
void NativeWindowClosed(WindowInstance& window);
void WindowRuntime_UpdateScene(session::ClientSession& session);
void PrepareWindowFrame(session::ClientSession& session);
std::vector<ScheduledWindow> MergeWindowOrder(session::ClientSession& session, int nativeCount);
void FinishWindowFrame(session::ClientSession& session);
void ShutdownExtensionWindows(session::ClientSession& session);
void WindowRuntime_SetVisible(WindowInstance& window, bool visible);
void WindowRuntime_ReleaseBindings(WindowInstance& window);

} // namespace cg::ui
