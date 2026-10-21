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

// 动态组件直接挂在sortedMap独占的稳定对象上；本入口不另外拥有元素或组件数组。
struct WindowComponentStore {
    WindowInstance& window;
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

// LUP02：按帧捕获控件左键拖动，与原生物品拖放phase隔离。
void WindowRuntime_BeginPointerDragFrame();
bool WindowRuntime_PointerDragBlocksMouse();
bool WindowRuntime_BeginPointerDrag(WindowInstance& window, WindowElement& element, int events);
int WindowRuntime_ProcessPointerDrag(WindowInstance& window);
void WindowRuntime_CancelPointerDrag(WindowInstance& window);
void WindowRuntime_EndPointerDragFrame();
bool WindowRuntime_IsElementInteractive(const WindowInstance& window, const WindowElement& element);

} // namespace cg::ui
