#pragma once
#include <string_view>
#include <cstdlib>
#include "LuaHost.hpp"
#include "LuaUIRefs.hpp"
#include "../ui/WindowComponents.hpp"

namespace cg::script {
using UIWindowInstanceEx = ui::WindowExtension;
ui::WindowEventBinding MakeLuaElementEvents(LuaCallbackRefs refs);
struct LuaWindow final : ui::WindowBinding {
    UIWindowInstance& window; // 非拥有，原生窗口持有本绑定。
    lua_State* owner = nullptr;
    const uint32_t& generation;
    LuaWindow(UIWindowInstance& value, lua_State* state);
    ~LuaWindow() override;
    void update() override;
    void draw() override;
    int event(int elementIndex, int flags) override;
    void elementUpdate(ui::WindowElement& element) override;
    void elementDraw(ui::WindowElement& element) override;
    int elementEvent(ui::WindowElement& element, int index, int flags) override;

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
UIControl* ResolveControl(const LuaControlRef& ref);
void ReleaseLuaElementRefs(lua_State* L, ui::WindowElement& element);
LuaWindow* EnsureLuaWindowBinding(lua_State* L, ui::WindowInstance& window);
bool LuaPushElementProperty(lua_State* L, ui::WindowElement& element, const char* key);
bool LuaPushWindowProperty(lua_State* L, ui::WindowInstance& window, const char* key);
void LuaApplyElementExtras(lua_State* L, int table, UIControl& control);
void LuaApplyWindowExtras(lua_State* L, int table, ui::WindowInstance& window);
void RegisterLuaUIProxy(lua_State* L);
void PushLuaElement(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element);
int CallLuaElementEvent(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element, int flags);
} // namespace cg::script
