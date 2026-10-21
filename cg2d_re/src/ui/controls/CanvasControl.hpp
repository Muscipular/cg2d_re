#pragma once
#include "UIControl.hpp"
namespace cg::ui {
struct WindowCanvasData {
    std::vector<CanvasShape> shapes;
    void SetShapes(std::vector<CanvasShape> values);
};
void WindowElement_DrawCanvas(const WindowElement& element, int ox, int oy, const UIClipRect* clip);
}
