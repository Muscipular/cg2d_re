#include "WindowComponents.hpp"
#include "cg/platform/Startup.hpp"

namespace cg::ui {
using namespace component_detail;

// 捕获只保存身份，动态数组发布/排序不会使捕获引用悬空。
static struct ElementPointerDrag {
    int windowId = -1;
    std::uint32_t generation = 0;
    int componentId = 0;
    int elementIndex = -1;
    int lastX = 0;
    int lastY = 0;
    bool moved = false;
    bool startedThisFrame = false;
} pointerDrag;
static bool pointerDragReserved = false; // 含松开/取消的当前帧，防止事件穿透。

bool WindowRuntime_IsElementInteractive(const WindowInstance& window, const WindowElement& element) {
    if (element.removed || (window.state & WIN_STATE_CLOSE) || window.scriptHidden || element.visible != 1) return false;
    if (const auto* component = element.component) {
        if (!component->attached || !component->hitable || (window.id >= WIN_MAX && !(window.state & WIN_STATE_ACTIVE))
            || !window.components) return false;
        for (int parentId = component->parentId; parentId != 0;) {
            const auto* parent = window.components->find(parentId);
            if (!parent || !parent->hitable || parent->element().visible != 1) return false;
            parentId = parent->parentId;
        }
    }
    return true;
}

static WindowElement* ResolvePointerDrag(WindowInstance*& window) {
    window = WindowRuntime_FindInstance(pointerDrag.windowId);
    if (!window || window->generation != pointerDrag.generation || (window->state & WIN_STATE_CLOSE)
        || window->scriptHidden || platform::gWindowActiveState != 2) return nullptr;
    WindowElement* element = window->FindElementById(static_cast<ElementId>(pointerDrag.elementIndex));
    return element && element->refDrag >= 0 && WindowRuntime_IsElementInteractive(*window, *element) ? element : nullptr;
}

static void CancelPointerDrag() {
    WindowInstance* window = nullptr;
    if (auto* element = ResolvePointerDrag(window); element && element->component)
        element->component->mouseState = UIControl::MOUSE_NONE;
    pointerDrag = {};
}

static void ConsumePointerDragMouse() {
    pointerDragReserved = true;
    gMouseEventState = MES_CAPTURED;
    gWindowHasFocus = 1;
}

void WindowRuntime_BeginPointerDragFrame() {
    pointerDragReserved = pointerDrag.windowId >= 0;
    pointerDrag.startedThisFrame = false;
    if (WindowDrag_ReadMousePhase() != 0) {
        CancelPointerDrag();
        pointerDragReserved = false; // 原生物品拖放保持其既有调度。
        return;
    }
    WindowInstance* window = nullptr;
    if (!ResolvePointerDrag(window)) CancelPointerDrag();
    if (pointerDragReserved) ConsumePointerDragMouse();
}

bool WindowRuntime_PointerDragBlocksMouse() { return pointerDragReserved; }

bool WindowRuntime_BeginPointerDrag(WindowInstance& window, WindowElement& element, int events) {
    if (pointerDragReserved || element.refDrag < 0 || !(events & MouseFlag_LDownEvent)
        || !(gMouseBtnDown2 & platform::LegacyMouseButton_Left) || WindowDrag_ReadMousePhase() != 0
        || platform::gWindowActiveState != 2) return false;
    pointerDrag.windowId = window.id;
    pointerDrag.generation = window.generation;
    pointerDrag.componentId = element.component ? element.component->id : 0;
    pointerDrag.elementIndex = static_cast<int>(element.elementId);
    pointerDrag.lastX = platform::gLegacyMouseState.buttonCoordinates[0];
    pointerDrag.lastY = platform::gLegacyMouseState.buttonCoordinates[1];
    pointerDrag.moved = false;
    pointerDrag.startedThisFrame = true;
    window.state &= ~std::uint32_t{0x800}; // 已命中的控件接管本次UI窗口位置拖动。
    ConsumePointerDragMouse();
    return true;
}

static void EmitPointerDrag(WindowInstance& window, WindowElement& element, int flags) {
    if (auto* component = element.component) {
        const auto callbacks = component->callbacks;
        if (callbacks) callbacks->mouse(component->ref(), flags);
    } else if (window.binding) {
        const int index = static_cast<int>(element.elementId);
        window.binding->elementEvent(element, index, flags);
        if (WindowRuntime_IsElementInteractive(window, element)) window.binding->event(index, flags);
    }
}

int WindowRuntime_ProcessPointerDrag(WindowInstance& window) {
    if (!pointerDragReserved) return 0;
    ConsumePointerDragMouse();
    if (pointerDrag.windowId != window.id || pointerDrag.generation != window.generation) return 1;
    WindowInstance* owner = nullptr;
    auto* element = ResolvePointerDrag(owner);
    if (!element || WindowDrag_ReadMousePhase() != 0) { CancelPointerDrag(); return 1; }
    const bool released = (gMouseClickState & platform::LegacyMouseEdge_LUp) != 0;
    const bool held = (gMouseBtnDown2 & platform::LegacyMouseButton_Left) != 0;
    if (!held && !released) { CancelPointerDrag(); return 1; }
    const int x = released ? static_cast<int>(platform::gLegacyMouseState.buttonCoordinates[2]) : static_cast<int>(gMouseX);
    const int y = released ? static_cast<int>(platform::gLegacyMouseState.buttonCoordinates[3]) : static_cast<int>(gMouseY);
    const bool moved = x != pointerDrag.lastX || y != pointerDrag.lastY;
    pointerDrag.lastX = x; pointerDrag.lastY = y;
    pointerDrag.moved |= moved;
    if (element->component) element->component->mouseState = UIControl::MOUSE_PRESS;
    if (pointerDrag.startedThisFrame && !moved && !released) return 1; // 按下本身已派发onPress。
    if (released) platform::ResetLegacyMousePosition_506890(x, y);
    int flags = held && !released ? MouseFlag_LHoldPhysical : MouseFlag_None;
    if (moved) flags |= MouseFlag_Dragging;
    if (flags) EmitPointerDrag(window, *element, flags);
    // 回调可隐藏、清空、关闭或取消onDrag；重新解析，不持有旧元素地址。
    element = ResolvePointerDrag(owner);
    if (!element || WindowDrag_ReadMousePhase() != 0) { CancelPointerDrag(); return 1; }
    if (released) {
        const bool finish = pointerDrag.moved;
        CancelPointerDrag(); // 先释放捕获，回调不能导致第二次结束。
        if (finish) EmitPointerDrag(window, *element, MouseFlag_DragFinish);
    }
    return 1;
}

void WindowRuntime_CancelPointerDrag(WindowInstance& window) {
    if (pointerDrag.windowId == window.id && pointerDrag.generation == window.generation) CancelPointerDrag();
}

void WindowRuntime_EndPointerDragFrame() {
    WindowInstance* window = nullptr;
    if (!ResolvePointerDrag(window) || WindowDrag_ReadMousePhase() != 0) CancelPointerDrag();
}

void UIControl::bindPayload() {
    auto& value = element();
    value.component = this;
    switch (value.type) {
    case WindowElementType_Graphic: value.data.graphic = imageData(); break;
    case WindowElementType_Text: value.data.text = textData(); break;
    case WindowElementType_Animation:
        animationPayload.action = static_cast<UIAnimeControl*>(this)->action;
        value.data.animation = &animationPayload;
        break;
    case WindowElementType_Input:
        inputPayload.context = &textInputData()->context;
        value.data.input = &inputPayload;
        break;
    case WindowElementType_PngImage: value.data.pngImage = static_cast<UIPngImageControl*>(this); break;
    case WindowElementType_Canvas: value.data.canvas = static_cast<UICanvasControl*>(this); break;
    case WindowElementType_MapEx: value.data.mapEx = static_cast<UIMapExControl*>(this); break;
    case WindowElementType_ScrollView: value.data.scrollView = static_cast<UIScrollViewControl*>(this); break;
    default: break;
    }
    if (callbacks) callbacks->attach(value);
}

WindowComponentStore::~WindowComponentStore() = default;

UIControl* WindowComponentStore::find(int id) const {
    auto* element = window.FindElementById(static_cast<ElementId>(id));
    return element && element->component && element->component->attached ? element->component : nullptr;
}

UIControl* WindowComponentStore::add(std::unique_ptr<UIControl> component, int parentId) {
    if (!component) return nullptr;
    if (parentId) {
        auto* parent = find(parentId);
        if (!parent || parent->type != UIControl::TYPE_SCROLL_VIEW) return nullptr;
    }
    auto& element = WindowRuntime_AddElement(window, static_cast<ElementId>(component->id));
    const auto elementId = element.elementId;
    const auto serial = element.creationSerial;
    element = component->draft;
    element.elementId = elementId;
    element.owner = &window;
    element.creationSerial = serial;
    element.seqNo = -nextId++; // 默认按创建顺序，显式elementId不影响新控件优先级。
    component->windowOwner = &window;
    component->parentId = parentId;
    component->currentElement = &element;
    component->draft = {};
    component->bindPayload();
    changed = true;
    return component.release();
}
void WindowComponentStore::clear() { WindowRuntime_ClearElements(window); changed = true; }
void WindowComponentStore::publish() { WindowRuntime_UpdateElementOrder(window); changed = false; }
int WindowComponentStore::rootCount() const { return static_cast<int>(window.sortedMap.size()); }

WindowComponentStore& WindowRuntime_Components(WindowInstance& window) {
    if (!window.components) window.components = new WindowComponentStore(window);
    return *window.components;
}

bool IsControlAttached(const WindowControlRef& ref) {
    const auto* window = WindowRuntime_FindInstance(ref.winId);
    return window && window->generation == ref.generation && !(window->state & WIN_STATE_CLOSE)
        && window->components && window->components->find(ref.controlId);
}

std::vector<UIControl*> WindowRuntime_Children(WindowInstance& window, int parentId) {
    std::vector<UIControl*> result;
    WindowElementCursor cursor(window);
    while (auto* element = cursor.next()) {
        auto* component = element ? element->component : nullptr;
        if (component && component->attached && component->parentId == static_cast<ElementId>(parentId)) result.push_back(component);
    }
    return result;
}

void UIResetControlProcessState(UIControl* control) {
    if (!control) return;
    const auto previous = control->mouseState;
    control->mouseState = UIControl::MOUSE_NONE;
    control->blurTextInput();
    UIEmitControlLeaveIfNeeded(control->ref(), control->callbacks, previous, control->mouseState);
    if (control->type == UIControl::TYPE_SCROLL_VIEW)
        static_cast<UIScrollViewControl*>(control)->draggingThumb = false;
}

void WindowRuntime_ResetComponents(WindowInstance& window) {
    if (!window.components) return;
    WindowElementCursor cursor(window);
    while (auto* element = cursor.next()) {
        if (element && element->component) UIResetControlProcessState(element->component);
    }
}

int WindowRuntime_ProcessComponents(WindowInstance& window, int hasFocused) {
    int captured = WindowRuntime_PointerDragBlocksMouse() ? 1 : 0;
    WindowElementCursor cursor(window);
    while (auto* element = cursor.next()) {
        auto* control = element->component;
        if (!control || control->parentId) continue;
        if ((window.state & WIN_STATE_CLOSE) || !(window.state & WIN_STATE_ACTIVE)) break;
        if (!control->attached) continue;
        if (window.binding) window.binding->elementUpdate(control->element());
        if (!control->attached || (window.state & WIN_STATE_CLOSE) || !(window.state & WIN_STATE_ACTIVE)) continue;
        const auto index = static_cast<int>(control->element().elementId);
        if (control->process(index, hasFocused, captured)) captured = 1;
    }
    return captured | WindowRuntime_ProcessPointerDrag(window);
}

} // namespace cg::ui
