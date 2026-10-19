#pragma once
#include "UIControl.hpp"

namespace cg::ui {
struct UITextControl : UIControl {
    TextData data;

    UITextControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, TextData data, WindowEventBinding callbacks);

    bool render(short ox, short oy) override;
    bool render(short ox, short oy, const UIClipRect* clipRect) override;

    TextData* textData() override;
    const TextData* textData() const override;
};
} // namespace cg::ui
