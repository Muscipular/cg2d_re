#include "../WindowComponents.hpp"

namespace cg::ui {
using namespace component_detail;
UIImageControl::UIImageControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, ImageData data, WindowEventBinding callbacks)
    : UIControl(winId, generation, id, TYPE_IMAGE, name, x, y, w, h, visible, callbacks), data(data) {
}

bool UIImageControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UIImageControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (element().visible != 1) {
        return true;
    }
    int imageNo = data.imageNo;
    if (mouseState == UIControl::MOUSE_HOVER && data.imageHover > 0) {
        imageNo = data.imageHover;
    } else if (mouseState == UIControl::MOUSE_PRESS && data.imagePress > 0) {
        imageNo = data.imagePress;
    }
    RendererDrawGraphicClipped(ox + element().x, oy + element().y, LAYER_UI, imageNo, element().width, element().height, data.color, clipRect);
    return true;
}

UIControl::ImageData* UIImageControl::imageData() {
    return &data;
}

const UIControl::ImageData* UIImageControl::imageData() const {
    return &data;
}
} // namespace cg::ui
