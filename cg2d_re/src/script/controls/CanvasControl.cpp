#include "../LuaUI.hpp"

namespace cg::script {
UICanvasControl::UICanvasControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, std::vector<CanvasShape> shapes, LuaCallbackRefs callbacks)
    : UIControl(winId, generation, id, TYPE_CANVAS, name, x, y, w, h, visible, callbacks) {
    hitable = false;
    SetShapes(std::move(shapes));
}

bool UICanvasControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UICanvasControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (visible != 1) {
        return true;
    }

    const int absX = ox + x;
    const int absY = oy + y;
    UIClipRect controlRect{
        static_cast<short>(absX),
        static_cast<short>(absY),
        static_cast<short>(absX + w),
        static_cast<short>(absY + h)
    };
    UIClipRect controlClip{};
    if (!UIIntersectClipRect(controlClip, clipRect, controlRect)) {
        return true;
    }

    RendererDrawCanvasClipped(absX, absY, w, h, LAYER_UI, shapes, &controlClip);
    return true;
}

void UICanvasControl::SetShapes(std::vector<CanvasShape> newShapes) {
    for (auto& shape: newShapes) {
        render::PrepareCanvasShape(shape);
    }
    shapes = std::move(newShapes);
}
} // namespace cg::script
