#include "../WindowComponents.hpp"

namespace cg::ui {
using namespace component_detail;
UITextControl::UITextControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, TextData data, WindowEventBinding callbacks)
    : UIControl(winId, generation, id, TYPE_TEXT, name, x, y, w, h, visible, callbacks), data(std::move(data)) {
    hitable = false;
}

bool UITextControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UITextControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (element().visible != 1) {
        return true;
    }
    RendererDrawTextClipped(ox + element().x, oy + element().y, FONT_LAYER_WINDOW, std::to_integer<uint8_t>(data.style.font), std::to_integer<uint8_t>(data.style.color), data.text, 0, 0, clipRect);
    return true;
}

UIControl::TextData* UITextControl::textData() {
    return &data;
}

const UIControl::TextData* UITextControl::textData() const {
    return &data;
}
} // namespace cg::ui
