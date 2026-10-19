#include "../LuaUI.hpp"

namespace cg::script {
UIPngImageControl::UIPngImageControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, PngImageData data, LuaCallbackRefs callbacks)
    : UIControl(winId, generation, id, TYPE_PNG_IMAGE, name, x, y, w, h, visible, callbacks), data(std::move(data)) {
}

bool UIPngImageControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UIPngImageControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (visible != 1) {
        return true;
    }
    const auto* state = &data.image;
    if (mouseState == UIControl::MOUSE_PRESS && !data.imagePress.path.empty()) {
        state = &data.imagePress;
    } else if (mouseState == UIControl::MOUSE_HOVER && !data.imageHover.path.empty()) {
        state = &data.imageHover;
    }
    if (!state->path.empty()) {
        RECT16 rect = state->rect;
        RendererDrawPngClipped(state->path.c_str(), state->hasRect ? &rect : nullptr, ox + x, oy + y, w, h, data.color, LAYER_UI, clipRect);
    }
    return true;
}

UIControl::PngImageData* UIPngImageControl::pngImageData() {
    return &data;
}

const UIControl::PngImageData* UIPngImageControl::pngImageData() const {
    return &data;
}
} // namespace cg::script
