#include "../WindowComponents.hpp"
namespace cg::ui {
void WindowElement_DrawPng(const WindowElement& element, int ox, int oy, const UIClipRect* clip) {
    const auto& data = *element.data.pngImage;
    const auto* state = &data.image;
    if (element.mouseState == WindowMouseState_Press && !data.imagePress.path.empty()) state = &data.imagePress;
    else if (element.mouseState == WindowMouseState_Hover && !data.imageHover.path.empty()) state = &data.imageHover;
    if (!state->path.empty()) {
        auto rect = state->rect;
        RendererDrawPngClipped(state->path.c_str(), state->hasRect ? &rect : nullptr,
            ox + element.x, oy + element.y, element.width, element.height, data.color, component_detail::LAYER_UI, clip);
    }
}
}
