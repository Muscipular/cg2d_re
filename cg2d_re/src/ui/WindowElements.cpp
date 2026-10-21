#include "WindowComponents.hpp"
#include "../script/LuaUI.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>

namespace cg::ui {
static std::uint64_t nextElementId = 10000;
static std::uint64_t nextCreationSerial = 0;

SortedElementMap::iterator SortedElementMap::upper_bound(const OrderKey& key) {
    return std::upper_bound(values.begin(), values.end(), key,
        [](const OrderKey& value, const auto& entry) { return value < entry.first; });
}

WindowElement* SortedElementMap::insert(ElementId id) {
    if (byId.contains(id)) return nullptr;
    auto owner = std::make_unique<WindowElement>();
    owner->elementId = id;
    const OrderKey key{0, id};
    const auto position = std::lower_bound(values.begin(), values.end(), key,
        [](const auto& entry, const OrderKey& value) { return entry.first < value; });
    const auto entry = values.emplace(position, key, std::move(owner));
    auto* const element = entry->second.get();
    try {
        if (!byId.emplace(id, element).second) {
            values.erase(entry);
            return nullptr;
        }
    } catch (...) {
        values.erase(entry);
        throw;
    }
    return element;
}

SortedElementMap::ElementOwner SortedElementMap::extract(WindowElement& element) noexcept {
    const auto indexed = byId.find(element.elementId);
    if (indexed == byId.end() || indexed->second != &element) return nullptr;
    const auto entry = std::find_if(values.begin(), values.end(),
        [&element](const auto& value) { return value.second.get() == &element; });
    if (entry == values.end()) return nullptr;
    auto owner = std::move(entry->second);
    values.erase(entry);
    byId.erase(indexed);
    return owner;
}

void SortedElementMap::notifyUpdate() {
    if (traversalDepth) {
        m_log_iw(L"[UI] sortedMap.notifyUpdate: 正在遍历元素，拒绝更新排序\n");
        throw std::runtime_error("sortedMap.notifyUpdate during element traversal");
    }
    bool changed = false;
    for (auto& [key, element] : values) {
        const auto nextKey = std::make_pair(element->seqNo, element->elementId);
        if (key == nextKey) continue;
        key = nextKey;
        changed = true;
    }
    if (!changed) return;
    std::sort(values.begin(), values.end(),
        [](const auto& left, const auto& right) { return left.first < right.first; });
    // 仅移动键与owner，对象地址不变，byId无需更新。
}

WindowElementCursor::WindowElementCursor(WindowInstance& value) : window(value), limit(nextCreationSerial) {
    window.sortedMap.beginTraversal();
}
WindowElement* WindowElementCursor::next() {
    auto it = started ? window.sortedMap.upper_bound(key) : window.sortedMap.begin();
    started = true;
    for (; it != window.sortedMap.end(); ++it) {
        key = it->first;
        if (!it->second->removed && it->second->creationSerial <= limit) return it->second.get();
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
    auto* const inserted = window.sortedMap.insert(id);
    if (!inserted) throw std::runtime_error("elementId already exists");
    auto& element = *inserted;
    element.owner = &window;
    element.creationSerial = ++nextCreationSerial;
    window.elementCount = static_cast<int>(window.sortedMap.size());
    return element;
}

bool WindowRuntime_RemoveElement(WindowInstance& window, ElementId id) {
    auto* element = window.FindElementById(id);
    if (!element) return false;
    // 在失焦等删除副作用之前准备退役容量；分配失败时保持活动对象及索引完整。
    window.retiredElements.reserve(window.retiredElements.size() + 1);
    WindowRuntime_CancelPointerDrag(window);
    element->detaching = true;
    WindowElement_ForgetInputRestore(*element);
    WindowRuntime_BlurElement(*element);
    // 游标覆盖整个删除过程，回收器不能释放递归中仍在借用的对象。
    WindowElementCursor children(window);
    auto owner = window.sortedMap.extract(*element);
    if (!owner) return false;
    element->removed = true;
    window.retiredElements.push_back(std::move(owner));
    window.elementCount = static_cast<int>(window.sortedMap.size());
    // 父对象先进入退役存储以供重入解绑访问，结束或异常退出时排到子孙之后。
    struct RetireParentLast {
        WindowInstance& window;
        WindowElement* element;
        ~RetireParentLast() noexcept {
            auto& retired = window.retiredElements;
            const auto parent = std::find_if(retired.begin(), retired.end(),
                [this](const auto& value) { return value.get() == element; });
            if (parent != retired.end()) std::rotate(parent, parent + 1, retired.end());
        }
    } retireParentLast{window, element};
    while (auto* child = children.next()) {
        if (child->parentId == id)
            WindowRuntime_RemoveElement(window, child->elementId);
    }
    if (element->type == WindowElementType_Input && element->data.input
        && component_detail::gCurrentInputData == element->data.input->context)
        detail::WindowChat_RestoreInput();
    return true;
}

void WindowRuntime_ClearElements(WindowInstance& window) {
    while (!window.sortedMap.empty()) WindowRuntime_RemoveElement(window, window.sortedMap.begin()->first.second);
}

static void ReleaseElement(WindowElement& element) {
    if (element.owner && element.owner->binding) {
        auto* binding = static_cast<script::LuaWindowBinding*>(element.owner->binding);
        script::ReleaseLuaElementRefs(binding->owner, element);
    }
    delete element.properties;
    element.properties = nullptr;
    if (!element.ownsPayload) return;
    WindowRuntime_DestroyPayload(element.type, element.data);
}

void WindowRuntime_CollectElements(WindowInstance& window) {
    if (window.sortedMap.isTraversing()) return;
    auto retired = std::move(window.retiredElements);
    window.retiredElements.clear();
    for (auto& element : retired) ReleaseElement(*element);
}
void WindowRuntime_DestroyElements(WindowInstance& window) {
    WindowRuntime_ClearElements(window);
    WindowRuntime_CollectElements(window);
}
} // namespace cg::ui
