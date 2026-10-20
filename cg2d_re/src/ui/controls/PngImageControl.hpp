#pragma once
#include "UIControl.hpp"

namespace cg::ui {
struct UIPngImageControl : UIControl {
    PngImageData data;

    UIPngImageControl(int winId, uint32_t generation, uint32_t id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, PngImageData data, WindowEventBinding callbacks);

    bool render(short ox, short oy) override;
    bool render(short ox, short oy, const UIClipRect* clipRect) override;

    PngImageData* pngImageData() override;
    const PngImageData* pngImageData() const override;
};
} // namespace cg::ui
