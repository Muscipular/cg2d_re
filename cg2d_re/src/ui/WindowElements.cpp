#include "WindowComponents.hpp"
#include "../script/LuaUI.hpp"
#include <limits>
#include <stdexcept>

namespace cg::ui {
static std::uint64_t nextElementId = 10000;
static std::uint64_t nextCreationSerial = 0;

void SortedElementMap::notifyUpdate() {
    if (traversalDepth) {
        m_log_iw(L"[UI] sortedMap.notifyUpdate: 正在遍历元素，拒绝更新排序\n");
        throw std::runtime_error("sortedMap.notifyUpdate during element traversal");
    }
    for (auto it = values.begin(); it != values.end();) {
        auto current = it++;
        const auto key = std::make_pair(current->second.seqNo, current->second.elementId);
        if (current->first == key) continue;
        auto node = values.extract(current);
        node.key() = key;
        values.insert(std::move(node)); // 节点地址不变，ID索引无需更新。
    }
}

WindowElementCursor::WindowElementCursor(WindowInstance& value) : window(value), limit(nextCreationSerial) {
    window.sortedMap.beginTraversal();
}
WindowElement* WindowElementCursor::next() {
    auto it = started ? window.sortedMap.upper_bound(key) : window.sortedMap.begin();
    started = true;
    for (; it != window.sortedMap.end(); ++it) {
        key = it->first;
        if (!it->second.removed && it->second.creationSerial <= limit) return &it->second;
    }
    return nullptr;
}

ElementId WindowRuntime_AllocateElementId(const WindowInstance& window, ElementId requested) {
    if (requested) {
        if (window.FindElementById(requested)) throw std::runtime_error("elementId already exists");
        return requested;
    }
    while (nextElementId <= std::numeric_limits<ElementId>::max()) {
        const auto id = static_cast<ElementId>(nextElementId++);
        if (!window.FindElementById(id)) return id;
    }
    throw std::runtime_error("elementId exhausted");
}

WindowElement& WindowRuntime_AddElement(WindowInstance& window, ElementId requested) {
    const ElementId id = WindowRuntime_AllocateElementId(window, requested);
    auto [it, inserted] = window.sortedMap.try_emplace(std::make_pair(0, id), WindowElement{});
    if (!inserted) throw std::runtime_error("elementId already exists");
    auto& element = it->second;
    element.elementId = id;
    element.owner = &window;
    element.creationSerial = ++nextCreationSerial;
    window.elementCount = static_cast<int>(window.sortedMap.size());
    return element;
}

bool WindowRuntime_RemoveElement(WindowInstance& window, ElementId id) {
    auto* element = window.FindElementById(id);
    if (!element) return false;
    WindowRuntime_CancelPointerDrag(window);
    if (element->component) {
        element->component->attached = false;
        element->component->blurTextInput();
    }
    // 先逻辑删除，再处理子孙和资源；退役节点保持回调正在借用的地址。
    element->removed = true;
    WindowElementCursor children(window);
    while (auto* child = children.next()) {
        if (child->component && static_cast<ElementId>(child->component->parentId) == id)
            WindowRuntime_RemoveElement(window, child->elementId);
    }
    if (element->type == WindowElementType_Input && element->data.input
        && component_detail::gCurrentInputData == element->data.input->context)
        detail::WindowChat_RestoreInput();
    for (auto it = window.sortedMap.begin(); it != window.sortedMap.end(); ++it) {
        if (it->first.second != id) continue;
        window.retiredElements.push_back(window.sortedMap.extract(it));
        break;
    }
    window.elementCount = static_cast<int>(window.sortedMap.size());
    return true;
}

void WindowRuntime_ClearElements(WindowInstance& window) {
    while (!window.sortedMap.empty()) WindowRuntime_RemoveElement(window, window.sortedMap.begin()->first.second);
}

static void ReleaseElement(WindowElement& element) {
    if (element.component) {
        delete element.component; // 组件析构释放自身载荷与Lua引用。
        element.component = nullptr;
        element.properties = nullptr;
        return;
    }
    if (element.owner && element.owner->binding) {
        auto* binding = static_cast<script::LuaWindowBinding*>(element.owner->binding);
        script::ReleaseLuaElementRefs(binding->owner, element);
    }
    delete element.properties;
    element.properties = nullptr;
    if (!element.ownsPayload) return;
    switch (element.type) {
    case WindowElementType_Graphic: std::free(element.data.graphic); break;
    case WindowElementType_Input: std::free(element.data.input); break;
    case WindowElementType_Animation:
        if (element.data.animation) {
            if (element.data.animation->action) cg::yobi::MarkDeathUnchecked(element.data.animation->action);
            std::free(element.data.animation);
        }
        break;
    case WindowElementType_Text: delete element.data.text; break;
    case WindowElementType_MouseArea: std::free(element.data.mouse); break;
    case WindowElementType_InputCaret: std::free(element.data.inputCaret); break;
    case WindowElementType_Number: std::free(element.data.number); break;
    default: break;
    }
}

void WindowRuntime_CollectElements(WindowInstance& window) {
    auto retired = std::move(window.retiredElements);
    window.retiredElements.clear();
    for (auto& node : retired) ReleaseElement(node.mapped());
}
void WindowRuntime_DestroyElements(WindowInstance& window) {
    WindowRuntime_ClearElements(window);
    WindowRuntime_CollectElements(window);
}
} // namespace cg::ui
