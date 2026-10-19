#pragma once
#include "UIControl.hpp"

namespace cg::ui {
struct UIImageControl : UIControl {
    ImageData data;

    UIImageControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, ImageData data, WindowEventBinding callbacks);

    bool render(short ox, short oy) override;
    bool render(short ox, short oy, const UIClipRect* clipRect) override;

    ImageData* imageData() override;
    const ImageData* imageData() const override;
};
} // namespace cg::ui
