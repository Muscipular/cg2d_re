#pragma once
#include <string_view>
#include "LuaHost.hpp"
#include "controls/UIControl.hpp"
#include "controls/ImageControl.hpp"
#include "controls/AnimeControl.hpp"
#include "controls/TextControl.hpp"
#include "controls/PngImageControl.hpp"
#include "controls/TextInputControl.hpp"
#include "controls/ScrollViewControl.hpp"
#include "controls/MapExControl.hpp"
#include "controls/CanvasControl.hpp"

namespace cg::script {
struct UIWindowInstanceEx {
    UIWindowInstance window{};
    UIWindowState state{};
    UIWindowInstanceEx() { state.window = &window; }
    virtual ~UIWindowInstanceEx() = default;
    virtual int render(int) = 0;
    virtual int update(int, int) = 0;
};
struct LuaWindow : public UIWindowInstanceEx {
    lua_State* owner = nullptr;
    std::vector<std::unique_ptr<UIControl>> retiredControls; // 回调结束前保留被清除控件。
    uint32_t generation = 0;
    int nextControlId = 1;
    int updateRef = -1;
    int drawRef = -1;
    int dragMove = 0;
    bool dragMoving = false;
    int dragMoveLastX = 0;
    int dragMoveLastY = 0;
    std::vector<std::unique_ptr<UIControl>> controls;

    LuaWindow();

    ~LuaWindow() override;

    int render(int st) override;

    int update(int st, int hasFocused) override;

    UIControl* FindControl(int controlId);

    void ClearChildren();

    UIControl* AddControl(std::unique_ptr<UIControl> control, int parentId);

    UIControl* AddImage(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::ImageData data, LuaCallbackRefs callbacks, int parentId = 0);

    UIControl* AddAnime(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::AnimeData data, LuaCallbackRefs callbacks, int parentId = 0);

    UIControl* AddText(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::TextData data, LuaCallbackRefs callbacks, int parentId = 0);

    UIControl* AddPngImage(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::PngImageData data, LuaCallbackRefs callbacks, int parentId = 0);

    UIControl* AddCanvas(std::wstring_view name, int x, int y, int w, int h, int visible, std::vector<CanvasShape> shapes, LuaCallbackRefs callbacks, int parentId = 0);

    UIControl* AddTextInput(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::TextInputData data, LuaCallbackRefs callbacks, int parentId = 0);

    UIControl* AddScrollView(std::wstring_view name, int x, int y, int w, int h, int visible, int contentHeight, int scrollY, int barWidth, int scrollStep, LuaCallbackRefs callbacks, int parentId = 0);

    UIControl* AddMapEx(std::wstring_view name, int x, int y, int w, int h, int visible, int mapX, int mapY, int mapId, int floor, LuaCallbackRefs callbacks, int parentId = 0);

    [[nodiscard]] LuaWindowRef ref() const;
};
std::shared_ptr<UIWindowInstanceEx> GetExtWindowById(int winId);
void WindowExOpen(std::shared_ptr<UIWindowInstanceEx>& window);
void CgWindowClose(int winId);
void RegisterLuaUI(lua_State* L);
void RegUIConst(lua_State* L);
void RegLuaIO(lua_State* L);
void ShutdownLuaUI();
void LuaBridgeSetAutoBattleState(lua_State* L);
void LuaProcessWinMgrKeyPress();
void LuaProcessWinMgrSceneStateChanged(int type, int state);
int LuaProcessWinMgrChatMessage(std::wstring_view text);
void LuaProcessWinMgrWindowFocusChanged(int focus, int blur);
void LuaProcessWinMgrWindowClose(int winId);
int LuaPeekMouseWheelDelta();
void LuaConsumeMouseWheelDelta();
void startCopilot(int x, int y);
void stopCopilot();
int getAutoCopliotState();
std::vector<std::pair<int,int>> getAutoCopliotRoute();
void CopilotProc();
void AutoBattleProc();
void InitializeAutoBattle();
void ShutdownAutoBattle();
void LoadLuaUIEntry();
void ShutdownLuaUIEvents();
bool IsLuaControlAttached(const LuaControlRef& ref);
// 回调可增删控件；当帧遍历借用快照，已移除对象由窗口保留至帧末。
inline std::vector<UIControl*> SnapshotControls(const std::vector<std::unique_ptr<UIControl>>& controls) {
    std::vector<UIControl*> result;
    result.reserve(controls.size());
    for (const auto& control : controls) if (control) result.push_back(control.get());
    return result;
}
// L03：当帧快照持有扩展窗口；回调期间创建的窗口在下一帧参与调度。
struct ScheduledLuaWindow {
    int id;
    UIWindowInstance* window;
    std::shared_ptr<UIWindowInstanceEx> extension;
};
void PrepareLuaWindowFrame();
// 沿场景自动开启名单保留状态栏、快捷栏、底栏和聊天栏；只标记关闭。
void CloseNonDefaultWindows(int exceptId);
bool CloseFocusedWindow();
std::vector<ScheduledLuaWindow> MergeLuaWindowOrder(int nativeCount);
void FinishLuaWindowFrame();
void NativeLuaWindowClosed(int id);
} // namespace cg::script
