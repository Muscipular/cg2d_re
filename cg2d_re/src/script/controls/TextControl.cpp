#include "../LuaUI.hpp"

namespace cg::script {
UITextControl::UITextControl(int winId, uint32_t generation, int id, const char* name, short x, short y, short w, short h, uint8_t visible, TextData data, LuaCallbackRefs callbacks)
    : UIControl(winId, generation, id, TYPE_TEXT, name, x, y, w, h, visible, callbacks), data(std::move(data)) {
    hitable = false;
}

bool UITextControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UITextControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (visible != 1) {
        return true;
    }
    RendererDrawTextClipped(ox + x, oy + y, FONT_LAYER_WINDOW, data.font, data.color, data.text, 0, 0, clipRect);
    return true;
}

UIControl::TextData* UITextControl::textData() {
    return &data;
}

const UIControl::TextData* UITextControl::textData() const {
    return &data;
}
} // namespace cg::script
