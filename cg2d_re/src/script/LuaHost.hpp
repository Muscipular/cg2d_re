#pragma once

#include "LuaInternal.hpp"
#include "../ui/WindowRuntimeState.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "../Yobi/base.hpp"
#include "cg/core/m_log.hpp"
#include "cg/input/Input.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/Canvas.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/PngImage.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"
#include <algorithm>
#include <memory>
#include <map>
#include <utility>
#include <cstring>
#include <limits>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace cg::script {

// L02–L08内部适配名；全部指向本线owner/API，不保留原客户端地址。
using InputContext = input::InputContext;
using YobiAction = yobi::YobiAction;
using UIWindowInstance = ui::WindowInstance;
using UIWindowState = ui::detail::WindowState;
using COLORREF = std::uint32_t;
using BOOL = int;
using DWORD = std::uint32_t;
using KRECT = render::GraphicDescriptor16;
using CanvasPoint = render::CanvasPoint;
using CanvasShape = render::CanvasShape;
using CanvasShapeType = render::CanvasShapeType;
using PngTexture = render::PixelImage;
struct RECT16 { std::int16_t left, right, top, bottom; };
inline constexpr int BATCH_SPRITE = 0;
inline constexpr int LAYER_UI = 0xDF;
inline constexpr int FONT_LAYER_WINDOW = 3;
inline constexpr int WIN_MAX = static_cast<int>(ui::detail::kWindowCount);
inline constexpr int WIN_NORMAL_END = 100, WIN_LUA_START = 101;
inline constexpr int WIN_AUTOBATTLE = 80, WIN_AUTOBATTLE_SIMPLE = 81;
inline constexpr int WIN_STATE_ACTIVE = 2, WIN_STATE_CLOSE = 0x8000, WIN_STATE_FOCUS = 0x2000;
inline constexpr int SCENE_NORMAL = 9, SCENE_BATTLE = 10;
inline constexpr int CG_MBTN_LEFTDOWN = 1, KSF_TRIGGER = 0x10;
inline constexpr int KEY_UP_EDGE = 1, KEY_DOWN = 2, KEY_DOWN_EDGE = 4, KEY_DOWN_LATCH = 8, KEY_DOWN_MESSAGE = 16;
inline constexpr int MES_CAPTURED = 212, MES_CAPTURE_THRESHOLD = 212;
inline constexpr int MCS_CALLBACK_A_ACTIVE = 1, MCS_CALLBACK_A_ENDING = 2, MCS_CALLBACK_B_ACTIVE = 4, MCS_CALLBACK_B_ENDING = 6;
enum MouseFlags : std::uint32_t {
    MouseFlag_None = 0, MouseFlag_Hover = 1, MouseFlag_LDownEvent = 2,
    MouseFlag_RDownEvent = 4, MouseFlag_LHoldPhysical = 8, MouseFlag_RHoldPhysical = 16,
    MouseFlag_Dragging = 32, MouseFlag_DragFinish = 64, MouseFlag_Leave = 128,
    MouseFlag_LDblClickEvent = 256, MouseFlag_RDblClickEvent = 512,
    MouseFlag_LRepeatDown = 1024, MouseFlag_RRepeatDown = 2048,
};
inline auto& gMouseX = platform::gLegacyMouseState.mouseX;
inline auto& gMouseY = platform::gLegacyMouseState.mouseY;
inline auto& gMouseBtnDown2 = platform::gLegacyMouseState.heldButtons;
inline auto& gMouseClickState = platform::gLegacyMouseState.edgeButtons;
inline auto& gMouseBtnDown = platform::gLegacyMouseState.pulseButtons;
inline auto& gMouseEventState = platform::gLegacyMouseState.valueD68DB4;
inline auto& gWindowHasFocus = ui::detail::gWindowHasFocusCA0AB4;
inline auto& gCurrentWindow = ui::detail::gCurrentWindow;
inline auto& gCurrentWindowState = ui::detail::gCurrentWindowState;
inline auto& gWindowState = ui::detail::gWindowStates;
inline auto& gCurrentInputData = input::gCurrentInputData;
inline auto* const gDefaultInputData = &ui::detail::gInputContext626AA0;

// 原生拖动私有状态只在WindowRuntime中读取/消费，不公开可写别名。
int ReadMousePhase();
void ConsumeLuaDrop();
inline bool MouseIsInRect(int x, int y, int w, int h) {
    const int mx = gMouseX.value(), my = gMouseY.value();
    return mx >= x && mx < x + w && my >= y && my < y + h;
}
inline void InputSetContext(InputContext* context) { input::SetContext_44B330(context); }
inline void RendererDrawInputText(InputContext* context) { render::DrawInputText_4568A0(context); }
inline int FontMeasure(const char* value, int font, int length) {
    return render::TextMeasure(text::FromGbkWide(std::string_view(value, length)), font);
}
inline void RendererBreakWindowTextBatch() { render::gGraphicCache4Queue[FONT_LAYER_WINDOW] = 0; }
inline int RendererDrawRect(std::int16_t x, std::int16_t y, std::int16_t w, std::int16_t h, std::uint8_t layer, COLORREF color) {
    return render::DrawRectangle(x, y, w, h, layer, color);
}
inline void YobiMarkDeath(YobiAction* action) { if (action) action->deathFlag = 1; }
inline void InfoRenderText(std::int16_t x, std::int16_t y, std::uint8_t layer, char font, std::uint8_t color, const char* value, char hover, char frame) {
    render::DrawColoredText(x, y, layer, font, color, text::FromGbkWide(value), hover, frame);
}

#define LuaFunction(A) int A(lua_State* L)
#define LuaFunctionEx(P, A) int Lua_##P##_##A(lua_State* L)
#define LuaCFunction(P, A) SafeLuaCall<Lua_##P##_##A>
#define CName(T) #T
#define LuaPush Push

} // namespace cg::script
