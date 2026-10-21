#pragma once
#include <string_view>
#include <cstdlib>
#include "LuaHost.hpp"
#include "LuaUIRefs.hpp"
#include "../ui/WindowComponents.hpp"

namespace cg::script {
using UIWindowInstanceEx = ui::WindowExtension;
void AttachLuaElementRefs(LuaCallbackRefs& refs, ui::WindowElement& element);
struct LuaWindowBinding final : ui::WindowBinding {
    UIWindowInstance& window; // 非拥有，原生窗口持有本绑定。
    lua_State* owner = nullptr;
    LuaWindowBinding(UIWindowInstance& value, lua_State* state) : window(value), owner(state) {}
    ~LuaWindowBinding() override;
    LuaWindowRef ref() const { return {window.id}; }
    void update() override;
    void draw() override;
    int event(int elementIndex, int flags) override;
    void elementUpdate(ui::WindowElement& element) override;
    void elementDraw(ui::WindowElement& element) override;
    int elementEvent(ui::WindowElement& element, int index, int flags) override;
    int elementText(ui::WindowElement& element, bool enter, std::wstring_view value) override;

};
// 每次API调用临时借用UI，不挂到窗口，也不控制窗口生命周期。
struct LuaWindow {
    UIWindowInstance& window;
    lua_State* owner = nullptr;
    const uint32_t& generation;
    ui::ElementId requestedElementId = 0;
    LuaWindow(UIWindowInstance& value, lua_State* state);
    ui::WindowElement* FindControl(int controlId);

    void ClearChildren();

    ui::WindowElement* AddControl(std::wstring_view name, int x, int y, int w, int h, int visible,
        ui::WindowElementType type, ui::WindowElementData& payload, LuaCallbackRefs& callbacks, int parentId);

    ui::WindowElement* AddImage(std::wstring_view name, int x, int y, int w, int h, int visible, ui::WindowGraphicData data, LuaCallbackRefs callbacks, int parentId = 0);

    ui::WindowElement* AddAnime(std::wstring_view name, int x, int y, int w, int h, int visible, ui::WindowAnimationOptions data, LuaCallbackRefs callbacks, int parentId = 0);

    ui::WindowElement* AddText(std::wstring_view name, int x, int y, int w, int h, int visible, ui::WindowTextData data, LuaCallbackRefs callbacks, int parentId = 0);

    ui::WindowElement* AddPngImage(std::wstring_view name, int x, int y, int w, int h, int visible, ui::WindowPngImageData data, LuaCallbackRefs callbacks, int parentId = 0);

    ui::WindowElement* AddCanvas(std::wstring_view name, int x, int y, int w, int h, int visible, std::vector<CanvasShape> shapes, LuaCallbackRefs callbacks, int parentId = 0);

    ui::WindowElement* AddTextInput(std::wstring_view name, int x, int y, int w, int h, int visible, ui::WindowTextInputState data, LuaCallbackRefs callbacks, int parentId = 0);

    ui::WindowElement* AddScrollView(std::wstring_view name, int x, int y, int w, int h, int visible, int contentHeight, int scrollY, int barWidth, int scrollStep, LuaCallbackRefs callbacks, int parentId = 0);

    ui::WindowElement* AddMapEx(std::wstring_view name, int x, int y, int w, int h, int visible, int mapX, int mapY, int mapId, int floor, LuaCallbackRefs callbacks, int parentId = 0);

    [[nodiscard]] LuaWindowRef ref() const;
};
void RegisterLuaUI(lua_State* L);
void RegUIConst(lua_State* L);
void RegLuaIO(lua_State* L);
void RegLuaAudio(lua_State* L); // LBG01：文件BGM及其userdata，不改旧编号接口。
void ShutdownLuaUI();
void LuaBridgeSetAutoBattleState(lua_State* L);
void LuaProcessWinMgrKeyPress();
void LuaProcessWinMgrSceneStateChanged(int type, int state);
int LuaProcessWinMgrChatMessage(std::wstring_view text);
void LuaProcessWinMgrWindowFocusChanged(int focus, int blur);
void LuaProcessWinMgrWindowOpen(int winId);
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
void ReleaseLuaElementRefs(lua_State* L, ui::WindowElement& element);
LuaWindowBinding* EnsureLuaWindowBinding(lua_State* L, ui::WindowInstance& window);
bool LuaPushElementProperty(lua_State* L, ui::WindowElement& element, const char* key);
bool LuaPushWindowProperty(lua_State* L, ui::WindowInstance& window, const char* key);
void LuaApplyElementExtras(lua_State* L, int table, ui::WindowElement& control);
void LuaApplyWindowExtras(lua_State* L, int table, ui::WindowInstance& window);
void RegisterLuaUIProxy(lua_State* L);
ui::WindowElement* ResolveLuaElement(const LuaControlRef& ref);
int LuaElementRead(lua_State* L);
void PushLuaElement(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element);
int CallLuaElementEvent(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element, int flags);
} // namespace cg::script
