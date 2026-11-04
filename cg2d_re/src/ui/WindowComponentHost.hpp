#pragma once

#include "cg/session/ClientSession.hpp"
#include "cg/text/Encoding.hpp"
#include <string_view>
#include <vector>
#include "WindowRuntimeState.hpp"
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
#include "cg/Yobi/YobiRuntime.hpp"
#include <algorithm>
#include <memory>
#include <map>
#include <utility>
#include <cstring>
#include <limits>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace cg::ui::component_detail {

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
inline constexpr int CG_MBTN_LEFTDOWN = platform::LegacyMouseButton_Left; // 兼容脚本侧既有按钮位引用。
inline constexpr int KSF_TRIGGER = 0x10;
inline constexpr int KEY_UP_EDGE = 1, KEY_DOWN = 2, KEY_DOWN_EDGE = 4, KEY_DOWN_LATCH = 8, KEY_DOWN_MESSAGE = 16;
inline constexpr int MES_CAPTURED = 212, MES_CAPTURE_THRESHOLD = 212;
inline constexpr int MCS_CALLBACK_A_ACTIVE = 1, MCS_CALLBACK_A_ENDING = 2, MCS_CALLBACK_B_ACTIVE = 4, MCS_CALLBACK_B_ENDING = 6;
using ui::detail::WindowRuntime_States;

inline bool MouseIsInRect(session::ClientSession& session, int x, int y, int w, int h) {
    if (!platform::IsSessionInputTarget(session)) return false;
    const int mx = static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX), my = static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY);
    return mx >= x && mx < x + w && my >= y && my < y + h;
}
inline void InputSetContext(session::ClientSession& session, InputContext* context) { input::SetContext_44B330(session, context); }
inline void RendererDrawInputText(session::ClientSession& session, InputContext* context) { render::DrawInputText_4568A0(session, context); }
inline int FontMeasure(std::wstring_view value, int font) {
    return render::TextMeasure(value, font);
}
inline void RendererBreakWindowTextBatch() { render::gGraphicCache4Queue[FONT_LAYER_WINDOW] = 0; }
inline int RendererDrawRect(std::int16_t x, std::int16_t y, std::int16_t w, std::int16_t h, std::uint8_t layer, COLORREF color) {
    return render::DrawRectangle(x, y, w, h, layer, color);
}
inline void YobiMarkDeath(YobiAction* action) { if (action) action->deathFlag = 1; }
inline void InfoRenderText(std::int16_t x, std::int16_t y, std::uint8_t layer, char font, std::uint8_t color, std::wstring_view value, char hover, char frame) {
    render::DrawColoredText(x, y, layer, font, color, value, hover, frame);
}

} // namespace cg::ui::component_detail
