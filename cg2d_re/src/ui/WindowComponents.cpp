#include "WindowComponents.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/session/ClientSession.hpp"

namespace cg::ui {
using namespace component_detail;

static session::ClientSession& WindowOwner(const WindowInstance& window) {
    return *window.ownerSession;
}

bool WindowRuntime_IsElementInteractive(const WindowInstance& window, const WindowElement& element) {
    if (element.removed || element.detaching || !element.hitable || (window.state & WIN_STATE_CLOSE)
        || window.scriptHidden || element.visible != 1 || (window.id >= WIN_MAX && !(window.state & WIN_STATE_ACTIVE))) return false;
    for (ElementId parentId = element.parentId; parentId != 0;) {
        const auto* parent = window.FindElementById(parentId);
        if (!parent || parent->removed || parent->detaching || !parent->hitable || parent->visible != 1) return false;
        parentId = parent->parentId;
    }
    return true;
}

static WindowElement* ResolvePointerDrag(session::ClientSession& session, WindowInstance*& window) {
    const auto& pointerDrag = session.UiFrame().pointerDrag;
    window = WindowRuntime_FindInstance(session, pointerDrag.windowId);
    if (!window || window->generation != pointerDrag.generation || (window->state & WIN_STATE_CLOSE)
        || window->scriptHidden || platform::gWindowActiveState != 2) return nullptr;
    WindowElement* element = window->FindElementById(static_cast<ElementId>(pointerDrag.elementId));
    return element && element->creationSerial == pointerDrag.creationSerial && element->refDrag >= 0 && WindowRuntime_IsElementInteractive(*window, *element) ? element : nullptr;
}

static void CancelPointerDrag(session::ClientSession& session) {
    WindowInstance* window = nullptr;
    if (auto* element = ResolvePointerDrag(session, window)) element->mouseState = WindowMouseState_None;
    session.UiFrame().pointerDrag = {};
}

static void ConsumePointerDragMouse(session::ClientSession& session) {
    auto& uiFrame = session.UiFrame();
    session.UiFrame().pointerDragReserved = true;
    session.UiFrame().mouse.state->valueD68DB4 = MES_CAPTURED;
    uiFrame.windowHasFocus = 1;
}

void WindowRuntime_BeginPointerDragFrame(session::ClientSession& session) {
    auto& pointerDrag = session.UiFrame().pointerDrag;
    auto& pointerDragReserved = session.UiFrame().pointerDragReserved;
    pointerDragReserved = pointerDrag.windowId >= 0;
    pointerDrag.startedThisFrame = false;
    if (WindowDrag_ReadMousePhase(session) != 0) {
        CancelPointerDrag(session);
        pointerDragReserved = false; // 原生物品拖放保持其既有调度。
        return;
    }
    WindowInstance* window = nullptr;
    if (!ResolvePointerDrag(session, window)) CancelPointerDrag(session);
    if (pointerDragReserved) ConsumePointerDragMouse(session);
}

bool WindowRuntime_PointerDragBlocksMouse(session::ClientSession& session) {
    return session.UiFrame().pointerDragReserved;
}
bool WindowRuntime_PointerDragBlocksMouse(const WindowInstance& window) {
    return WindowRuntime_PointerDragBlocksMouse(WindowOwner(window));
}

bool WindowRuntime_BeginPointerDrag(WindowInstance& window, WindowElement& element, int events) {
    auto& session = WindowOwner(window);
    auto& pointerDrag = session.UiFrame().pointerDrag;
    auto& pointerDragReserved = session.UiFrame().pointerDragReserved;
    if (pointerDragReserved || element.refDrag < 0 || !(events & MouseFlag_LDownEvent)
        || !(window.ownerSession->UiFrame().mouse.state->heldButtons & platform::LegacyMouseButton_Left) || WindowDrag_ReadMousePhase(session) != 0
        || platform::gWindowActiveState != 2) return false;
    pointerDrag.windowId = window.id;
    pointerDrag.generation = window.generation;
    pointerDrag.elementId = element.elementId;
    pointerDrag.creationSerial = element.creationSerial;
    pointerDrag.lastX = window.ownerSession->UiFrame().mouse.state->buttonCoordinates[0];
    pointerDrag.lastY = window.ownerSession->UiFrame().mouse.state->buttonCoordinates[1];
    pointerDrag.moved = false;
    pointerDrag.startedThisFrame = true;
    window.state &= ~std::uint32_t{0x800}; // 已命中的控件接管本次UI窗口位置拖动。
    ConsumePointerDragMouse(session);
    return true;
}

static void EmitPointerDrag(WindowInstance& window, WindowElement& element, int flags) {
    if (!window.binding) return;
    const auto ref = WindowRuntime_ElementRef(element);
    window.binding->elementEvent(element, static_cast<int>(element.elementId), flags);
    if (auto* live = WindowRuntime_ResolveElement(ref); live && window.binding && WindowRuntime_IsElementInteractive(window, *live))
        window.binding->event(static_cast<int>(live->elementId), flags);
}

int WindowRuntime_ProcessPointerDrag(WindowInstance& window) {
    auto& session = WindowOwner(window);
    auto& pointerDrag = session.UiFrame().pointerDrag;
    const bool pointerDragReserved = session.UiFrame().pointerDragReserved;
    if (!pointerDragReserved) return 0;
    WindowElementCursor lease(window);
    ConsumePointerDragMouse(session);
    if (pointerDrag.windowId != window.id || pointerDrag.generation != window.generation) return 1;
    WindowInstance* owner = nullptr;
    auto* element = ResolvePointerDrag(session, owner);
    if (!element || WindowDrag_ReadMousePhase(session) != 0) { CancelPointerDrag(session); return 1; }
    const bool released = (window.ownerSession->UiFrame().mouse.state->edgeButtons & platform::LegacyMouseEdge_LUp) != 0;
    const bool held = (window.ownerSession->UiFrame().mouse.state->heldButtons & platform::LegacyMouseButton_Left) != 0;
    if (!held && !released) { CancelPointerDrag(session); return 1; }
    const int x = released ? static_cast<int>(window.ownerSession->UiFrame().mouse.state->buttonCoordinates[2]) : static_cast<int>(window.ownerSession->UiFrame().mouse.state->mouseX);
    const int y = released ? static_cast<int>(window.ownerSession->UiFrame().mouse.state->buttonCoordinates[3]) : static_cast<int>(window.ownerSession->UiFrame().mouse.state->mouseY);
    const bool moved = x != pointerDrag.lastX || y != pointerDrag.lastY;
    pointerDrag.lastX = x; pointerDrag.lastY = y;
    pointerDrag.moved |= moved;
    element->mouseState = WindowMouseState_Press;
    if (pointerDrag.startedThisFrame && !moved && !released) return 1; // 按下本身已派发onPress。
    if (released) platform::ResetLegacyMousePosition_506890(*window.ownerSession, x, y);
    int flags = held && !released ? MouseFlag_LHoldPhysical : MouseFlag_None;
    if (moved) flags |= MouseFlag_Dragging;
    if (flags) EmitPointerDrag(window, *element, flags);
    // 回调可隐藏、清空、关闭或取消onDrag；重新解析，不持有旧元素地址。
    element = ResolvePointerDrag(session, owner);
    if (!element || WindowDrag_ReadMousePhase(session) != 0) { CancelPointerDrag(session); return 1; }
    if (released) {
        const bool finish = pointerDrag.moved;
        CancelPointerDrag(session); // 先释放捕获，回调不能导致第二次结束。
        if (finish) EmitPointerDrag(window, *element, MouseFlag_DragFinish);
    }
    return 1;
}

void WindowRuntime_CancelPointerDrag(WindowInstance& window) {
    auto& session = WindowOwner(window);
    const auto& pointerDrag = session.UiFrame().pointerDrag;
    if (pointerDrag.windowId == window.id && pointerDrag.generation == window.generation)
        CancelPointerDrag(session);
}

void WindowRuntime_EndPointerDragFrame(session::ClientSession& session) {
    WindowInstance* window = nullptr;
    if (!ResolvePointerDrag(session, window) || WindowDrag_ReadMousePhase(session) != 0) CancelPointerDrag(session);
}



WindowElementRef WindowRuntime_ElementRef(const WindowElement& element) {
    return {element.owner->id, element.owner->generation, element.elementId,
        element.creationSerial, element.owner->ownerSession};
}
WindowElement* WindowRuntime_ResolveElement(const WindowElementRef& ref) {
    auto* window = ref.ownerSession ? WindowRuntime_FindInstance(*ref.ownerSession, ref.winId) : nullptr;
    if (!window || window->generation != ref.generation || (window->state & WIN_STATE_CLOSE)) return nullptr;
    auto* element = window->FindElementById(ref.elementId);
    return element && element->creationSerial == ref.creationSerial ? element : nullptr;
}
std::vector<WindowElementRef> WindowRuntime_Children(WindowInstance& window, ElementId parentId) {
    std::vector<WindowElementRef> result;
    WindowElementCursor cursor(window);
    while (auto* element = cursor.next()) if (element->parentId == parentId && !element->detaching) result.push_back(WindowRuntime_ElementRef(*element));
    return result;
}
void WindowRuntime_BlurElement(WindowElement& element) {
    WindowElementCursor lease(*element.owner);
    WindowElement_BlurInput(element);
    if (element.type != WindowElementType_ScrollView) return; // 只有滚动容器拥有子元素，普通隐藏元素无需扫描全窗口。
    for (const auto& ref : WindowRuntime_Children(*element.owner, element.elementId))
        if (auto* child = WindowRuntime_ResolveElement(ref)) WindowRuntime_BlurElement(*child);
}
void WindowRuntime_ResetElement(WindowElement& element) {
    auto& window = *element.owner;
    WindowElementCursor lease(window);
    const auto ref = WindowRuntime_ElementRef(element);
    const auto previous = element.mouseState;
    element.mouseState = WindowMouseState_None;
    if (element.type == WindowElementType_Graphic) WindowElement_UpdateImageState(element);
    WindowElement_BlurInput(element);
    if (element.type == WindowElementType_ScrollView && element.data.scrollView) element.data.scrollView->draggingThumb = false;
    if (previous == WindowMouseState_Hover && !element.detaching && window.binding)
        window.binding->elementEvent(element, static_cast<int>(element.elementId), MouseFlag_Leave);
    if (element.type != WindowElementType_ScrollView || !WindowRuntime_ResolveElement(ref)) return;
    for (const auto& childRef : WindowRuntime_Children(window, element.elementId))
        if (auto* child = WindowRuntime_ResolveElement(childRef)) WindowRuntime_ResetElement(*child);
}
void WindowRuntime_ResetElements(WindowInstance& window) {
    WindowElementCursor cursor(window);
    while (auto* element = cursor.next()) if (!element->parentId) WindowRuntime_ResetElement(*element);
}
int WindowRuntime_ProcessElements(WindowInstance& window) {
    int handled = 0;
    WindowElementCursor cursor(window);
    while (auto* element = cursor.next()) {
        if (element->parentId) continue;
        if ((window.state & WIN_STATE_CLOSE) || !(window.state & WIN_STATE_ACTIVE)) break;
        const auto ref = WindowRuntime_ElementRef(*element);
        if (window.binding) window.binding->elementUpdate(*element);
        if (auto* live = WindowRuntime_ResolveElement(ref)) handled |= WindowRuntime_DispatchElementMouse(window, live->elementId, window.x, window.y, nullptr);
    }
    return handled | WindowRuntime_ProcessPointerDrag(window);
}
void WindowRuntime_DestroyPayload(WindowElementType type, WindowElementData& data) {
    switch (type) {
    case WindowElementType_Graphic: std::free(data.graphic); break;
    case WindowElementType_Text: delete data.text; break;
    case WindowElementType_Animation:
        if (data.animation) { if (data.animation->action) yobi::MarkDeath(data.animation->action); delete data.animation->options; std::free(data.animation); }
        break;
    case WindowElementType_Input:
        if (data.input) { delete data.input->ownedInput; std::free(data.input); }
        break;
    case WindowElementType_MouseArea: std::free(data.mouse); break;
    case WindowElementType_InputCaret: std::free(data.inputCaret); break;
    case WindowElementType_Number: std::free(data.number); break;
    case WindowElementType_PngImage: delete data.pngImage; break;
    case WindowElementType_Canvas: delete data.canvas; break;
    case WindowElementType_MapEx: delete data.mapEx; break;
    case WindowElementType_ScrollView: delete data.scrollView; break;
    default: break;
    }
    data = {};
}
} // namespace cg::ui
