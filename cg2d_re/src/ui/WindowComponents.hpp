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

// 连续元素数组仅在调度安全点发布；组件地址稳定，element()随发布重绑。
struct WindowComponentStore {
    WindowInstance& window;
    std::vector<WindowElement> elements;
    std::vector<std::unique_ptr<UIControl>> components;
    std::vector<std::unique_ptr<UIControl>> pending;
    int nextId = 1;
    bool changed = false;
    explicit WindowComponentStore(WindowInstance& value) : window(value) {}
    ~WindowComponentStore();
    UIControl* find(int id) const;
    UIControl* add(std::unique_ptr<UIControl> component, int parentId);
    void clear();
    void publish();
    int rootCount() const;
};

WindowComponentStore& WindowRuntime_Components(WindowInstance& window);
std::vector<UIControl*> WindowRuntime_Children(WindowInstance& window, int parentId);
void WindowRuntime_ResetComponents(WindowInstance& window);
int WindowRuntime_ProcessComponents(WindowInstance& window, int hasFocused);

} // namespace cg::ui
