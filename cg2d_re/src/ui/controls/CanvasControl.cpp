#include "../WindowComponents.hpp"
namespace cg::ui {
void WindowCanvasData::SetShapes(std::vector<CanvasShape> values) {
    for (auto& shape : values) render::PrepareCanvasShape(shape);
    shapes = std::move(values);
}
void WindowElement_DrawCanvas(const WindowElement& element, int ox, int oy, const UIClipRect* clip) {
    const int x = ox + element.x, y = oy + element.y;
    UIClipRect bounds{static_cast<short>(x), static_cast<short>(y),
        static_cast<short>(x + element.width), static_cast<short>(y + element.height)}, clipped{};
    if (UIIntersectClipRect(clipped, clip, bounds))
        RendererDrawCanvasClipped(*element.owner->ownerSession, x, y, element.width, element.height, component_detail::LAYER_UI, element.data.canvas->shapes, &clipped);
}
}
