#pragma once
#include "UIControl.hpp"

namespace cg::script {
struct UIPngImageControl : UIControl {
    PngImageData data;

    UIPngImageControl(int winId, uint32_t generation, int id, const char* name, short x, short y, short w, short h, uint8_t visible, PngImageData data, LuaCallbackRefs callbacks);

    bool render(short ox, short oy) override;
    bool render(short ox, short oy, const UIClipRect* clipRect) override;

    PngImageData* pngImageData() override;
    const PngImageData* pngImageData() const override;
};
} // namespace cg::script
