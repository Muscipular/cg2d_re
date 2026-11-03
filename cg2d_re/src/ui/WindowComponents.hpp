#pragma once
#include "WindowExtension.hpp"
#include "controls/UIControl.hpp"
#include "controls/ImageControl.hpp"
#include "controls/AnimeControl.hpp"
#include "controls/TextControl.hpp"
#include "controls/PngImageControl.hpp"
#include "controls/TextInputControl.hpp"
#include "controls/ScrollViewControl.hpp"
#include "controls/MapExControl.hpp"
#include "controls/CanvasControl.hpp"

namespace cg::ui {

// 元素身份、父子访问和资源清理共用原生存储。
std::vector<WindowElementRef> WindowRuntime_Children(WindowInstance& window, ElementId parentId);
void WindowRuntime_BlurElement(WindowElement& element);
void WindowRuntime_ResetElement(WindowElement& element);
void WindowRuntime_ResetElements(WindowInstance& window);
int WindowRuntime_ProcessElements(WindowInstance& window);
void WindowRuntime_DestroyPayload(WindowElementType type, WindowElementData& data);
void WindowRuntime_DrawElement(WindowInstance& window, ElementId id, int ox, int oy, const UIClipRect* clip);
int WindowRuntime_DispatchElementMouse(WindowInstance& window, ElementId id, int ox, int oy, const UIClipRect* clip);

// LUP02：按帧捕获控件左键拖动，与原生物品拖放phase隔离。
void WindowRuntime_BeginPointerDragFrame(session::ClientSession& session);
bool WindowRuntime_PointerDragBlocksMouse(session::ClientSession& session);
bool WindowRuntime_PointerDragBlocksMouse(const WindowInstance& window);
bool WindowRuntime_BeginPointerDrag(WindowInstance& window, WindowElement& element, int events);
int WindowRuntime_ProcessPointerDrag(WindowInstance& window);
void WindowRuntime_CancelPointerDrag(WindowInstance& window);
void WindowRuntime_EndPointerDragFrame(session::ClientSession& session);
bool WindowRuntime_IsElementInteractive(const WindowInstance& window, const WindowElement& element);

} // namespace cg::ui
