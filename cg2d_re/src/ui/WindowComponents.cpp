#include "WindowComponents.hpp"

namespace cg::ui {
using namespace component_detail;

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

WindowComponentStore::~WindowComponentStore() {
    // 元素内存须晚于组件析构，输入焦点及事件解绑会访问当前元素。
    pending.clear();
    components.clear();
    window.elements = nullptr;
    window.elementCount = 0;
}

UIControl* WindowComponentStore::find(int id) const {
    for (const auto* list : {&components, &pending})
        for (const auto& item : *list)
            if (item->attached && item->id == id) return item.get();
    return nullptr;
}

UIControl* WindowComponentStore::add(std::unique_ptr<UIControl> component, int parentId) {
    if (!component) return nullptr;
    if (parentId != 0) {
        const auto* parent = find(parentId);
        if (!parent || parent->type != UIControl::TYPE_SCROLL_VIEW) return nullptr;
    }
    component->windowOwner = &window;
    component->parentId = parentId;
    component->bindPayload();
    pending.push_back(std::move(component));
    changed = true;
    return pending.back().get();
}

void WindowComponentStore::clear() {
    // 不改变正在遍历的数组；先使全部代理失效，再归还焦点。
    for (auto* list : {&components, &pending})
        for (auto& item : *list) item->attached = false;
    for (auto* list : {&components, &pending})
        for (auto& item : *list) item->blurTextInput();
    changed = true;
}

void WindowComponentStore::publish() {
    if (!changed) return;
    // 先完成所有可能分配失败的操作，再迁移owner和借用地址。
    std::vector<WindowElement> next;
    next.reserve(components.size() + pending.size());
    components.reserve(components.size() + pending.size());
    for (const auto* list : {&components, &pending})
        for (const auto& item : *list) if (item->attached) next.push_back(item->element());
    std::erase_if(components, [](const auto& item) { return !item->attached; });
    std::erase_if(pending, [](const auto& item) { return !item->attached; });
    for (auto& item : pending) components.push_back(std::move(item));
    pending.clear();
    elements.swap(next);
    window.elements = elements.data();
    window.elementCount = static_cast<int>(elements.size());
    for (std::size_t i = 0; i < components.size(); ++i) {
        components[i]->currentElement = &elements[i];
        components[i]->draft = {}; // 退役草稿不再保留属性表/载荷别名。
        components[i]->bindPayload();
    }
    window.elementsSortedSource = nullptr;
    changed = false;
}

int WindowComponentStore::rootCount() const {
    int result = 0;
    for (const auto* list : {&components, &pending})
        for (const auto& item : *list) if (item->attached && item->parentId == 0) ++result;
    return result;
}

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
    for (int i = 0; i < window.elementsSortedCount; ++i) {
        auto* component = window.elementsSorted[i]->component;
        if (component && component->attached && component->parentId == parentId) result.push_back(component);
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
    std::vector<UIControl*> snapshot;
    for (const auto* list : {&window.components->components, &window.components->pending})
        for (const auto& item : *list) if (item->attached) snapshot.push_back(item.get());
    for (auto* item : snapshot) if (item->attached) UIResetControlProcessState(item);
}

int WindowRuntime_ProcessComponents(WindowInstance& window, int hasFocused) {
    int captured = 0;
    for (auto* control : WindowRuntime_Children(window, 0)) {
        if ((window.state & WIN_STATE_CLOSE) || !(window.state & WIN_STATE_ACTIVE)) break;
        if (!control->attached) continue;
        if (window.binding) window.binding->elementUpdate(control->element());
        if (!control->attached || (window.state & WIN_STATE_CLOSE) || !(window.state & WIN_STATE_ACTIVE)) continue;
        const auto index = static_cast<int>(&control->element() - window.elements);
        if (control->process(index, hasFocused, captured)) captured = 1;
    }
    return captured;
}

} // namespace cg::ui
