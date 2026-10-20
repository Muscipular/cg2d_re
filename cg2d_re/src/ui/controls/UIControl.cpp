#include "../WindowComponents.hpp"

namespace cg::ui {
using namespace component_detail;

bool UIClipRectValid(const UIClipRect& rect) {
    return rect.right > rect.left && rect.bottom > rect.top;
}

bool UIIntersectClipRect(UIClipRect& out, const UIClipRect* a, const UIClipRect& b) {
    if (a) {
        out.left = std::max(a->left, b.left);
        out.top = std::max(a->top, b.top);
        out.right = std::min(a->right, b.right);
        out.bottom = std::min(a->bottom, b.bottom);
    } else {
        out = b;
    }
    return UIClipRectValid(out);
}

bool UIPointInClipRect(int x, int y, const UIClipRect* clipRect) {
    return !clipRect || (x >= clipRect->left && x < clipRect->right && y >= clipRect->top && y < clipRect->bottom);
}

bool UIRectIntersectsClipRect(int x, int y, int w, int h, const UIClipRect* clipRect) {
    if (!clipRect) {
        return w > 0 && h > 0;
    }
    return x < clipRect->right && x + w > clipRect->left && y < clipRect->bottom && y + h > clipRect->top;
}

static bool UIClipDestRect(int& x, int& y, int& w, int& h, const UIClipRect* clipRect, int* dx = nullptr, int* dy = nullptr) {
    if (w <= 0 || h <= 0) {
        return false;
    }
    if (!clipRect) {
        if (dx) {
            *dx = 0;
        }
        if (dy) {
            *dy = 0;
        }
        return true;
    }

    int left = std::max(x, static_cast<int>(clipRect->left));
    int top = std::max(y, static_cast<int>(clipRect->top));
    int right = std::min(x + w, static_cast<int>(clipRect->right));
    int bottom = std::min(y + h, static_cast<int>(clipRect->bottom));
    if (right <= left || bottom <= top) {
        return false;
    }
    if (dx) {
        *dx = left - x;
    }
    if (dy) {
        *dy = top - y;
    }
    x = left;
    y = top;
    w = right - left;
    h = bottom - top;
    return true;
}

int RendererDrawRectClipped(int x, int y, int w, int h, uint8_t layer, COLORREF color, const UIClipRect* clipRect) {
    int drawX = x;
    int drawY = y;
    int drawW = w;
    int drawH = h;
    if (!UIClipDestRect(drawX, drawY, drawW, drawH, clipRect)) {
        return -1;
    }
    return RendererDrawRect(static_cast<int16_t>(drawX), static_cast<int16_t>(drawY), static_cast<int16_t>(drawW), static_cast<int16_t>(drawH), layer, color);
}

int RendererDrawGraphicClipped(int x, int y, uint8_t layer, int imageNo, int w, int h, COLORREF color, const UIClipRect* clipRect) {
    if (clipRect && (x < clipRect->left || y < clipRect->top || x + w > clipRect->right || y + h > clipRect->bottom)) {
        return -1;
    }

    KRECT krect{0, 0, 0, 0, color, 0, 4, 0};
    return render::DrawGraphic3_520970(x, y, layer, imageNo, 0, 1, BATCH_SPRITE, &krect);
}



// 与字体测宽共用UTF-16步进；裁剪仅保留完整字符，不拆开代理对。
void RendererDrawTextClipped(int x, int y, unsigned __int8 layer, char fontType, uint8_t colorType, std::wstring_view msg, char hoverMode, char hoverFrame, const UIClipRect* clipRect) {
    if (msg.empty()) {
        return;
    }
    if (!clipRect) {
        InfoRenderText(static_cast<int16_t>(x), static_cast<int16_t>(y), layer, fontType, colorType, msg, hoverMode, hoverFrame);
        return;
    }

    const auto& font = render::GetFontMetrics(fontType);
    int fontHeight = font.fontHeight > 0 ? font.fontHeight : font.sHeight;
    if (fontHeight <= 0) {
        fontHeight = 16;
    }
    if (y < clipRect->top || y + fontHeight > clipRect->bottom) {
        return;
    }

    std::wstring clippedText;
    int drawX = x;
    int currentX = x;
    for (std::size_t offset = 0; offset < msg.size();) {
        const std::size_t next = text::Next(msg, offset);
        const auto glyph = msg.substr(offset, next - offset);
        int charWidth = FontMeasure(glyph, fontType);
        if (charWidth <= 0) {
            // 保留原半角/全角度量回退，不把UTF-16码元数当作GBK字节数。
            charWidth = font.sWidth * (msg[offset] < 0x80 ? 1 : 2);
        }
        const int nextX = currentX + charWidth;
        if (currentX >= clipRect->left && nextX <= clipRect->right) {
            if (clippedText.empty()) drawX = currentX;
            clippedText.append(glyph);
        }
        currentX = nextX;
        offset = next;
    }

    if (!clippedText.empty()) {
        InfoRenderText(static_cast<int16_t>(drawX), static_cast<int16_t>(y), layer, fontType, colorType, clippedText, hoverMode, hoverFrame);
    }
}

void UIEmitControlLeaveIfNeeded(WindowControlRef ref, WindowEventBinding callbacks,
    uint8_t previousMouseState, uint8_t mouseState) {
    if (previousMouseState == UIControl::MOUSE_HOVER && mouseState == UIControl::MOUSE_NONE
        && callbacks && IsControlAttached(ref)) callbacks->mouse(ref, MouseFlag_Leave);
}
static int EmitControlEvents(WindowControlRef ref, WindowEventBinding callbacks, int flags) {
    return flags && callbacks && IsControlAttached(ref) ? callbacks->mouse(ref, flags) : 0;
}

UIControl::UIControl(int winId, uint32_t generation, uint32_t id, TYPE type, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, WindowEventBinding callbacks)
    : winId(winId), generation(generation), id(id), type(type), mouseState(MOUSE_NONE), name(name), callbacks(std::move(callbacks)) {
    draft.x = x; draft.y = y; draft.width = w; draft.height = h; draft.visible = visible;
    draft.component = this;
    draft.seqNo = -static_cast<std::int32_t>(id % 1000000000u); // 旧Lua控件后加入者优先。
    static constexpr WindowElementType types[] = {WindowElementType_EventArea,
        WindowElementType_Graphic, WindowElementType_Text, WindowElementType_PngImage,
        WindowElementType_Input, WindowElementType_Animation, WindowElementType_ScrollView,
        WindowElementType_MapEx, WindowElementType_Canvas};
    draft.type = types[type];
}

UIControl::~UIControl() {
    if (callbacks) callbacks->release(element());
    delete element().properties;
}

bool UIControl::render(short /*offsetX*/, short /*offsetY*/) {
    return true;
}

bool UIControl::render(short offsetX, short offsetY, const UIClipRect* clipRect) {
    (void) clipRect;
    return render(offsetX, offsetY);
}

bool UIControl::isHitable() const {
    if (element().visible != 1) {
        return false;
    }
    return hitable;
}

UIControl::ImageData* UIControl::imageData() {
    return nullptr;
}

const UIControl::ImageData* UIControl::imageData() const {
    return nullptr;
}

UIControl::AnimeData* UIControl::animeData() {
    return nullptr;
}

const UIControl::AnimeData* UIControl::animeData() const {
    return nullptr;
}

UIControl::TextData* UIControl::textData() {
    return nullptr;
}

const UIControl::TextData* UIControl::textData() const {
    return nullptr;
}

UIControl::PngImageData* UIControl::pngImageData() {
    return nullptr;
}

const UIControl::PngImageData* UIControl::pngImageData() const {
    return nullptr;
}

UIControl::TextInputData* UIControl::textInputData() {
    return nullptr;
}

const UIControl::TextInputData* UIControl::textInputData() const {
    return nullptr;
}

bool UIControl::isTextInputFocused() {
    return false;
}

void UIControl::blurTextInput() {
}

void UIControl::focusTextInput() {
}

void UIControl::setTextInputText(std::wstring_view text) {
    (void) text;
}

void UIControl::syncTextInputText() {
}

bool UIControl::process(int index, int hasFocused, int captured) {
    return process(index, hasFocused, captured, nullptr, static_cast<short>(gCurrentWindow->x), static_cast<short>(gCurrentWindow->y));
}

bool UIControl::process(int /*index*/, int hasFocused, int captured, const UIClipRect* clipRect, short ox, short oy) {
    if (!attached || (windowOwner && ((windowOwner->state & WIN_STATE_CLOSE) || (windowOwner->id >= WIN_MAX && !(windowOwner->state & WIN_STATE_ACTIVE))))) return false;
    if (WindowRuntime_PointerDragBlocksMouse()) {
        mouseState = MOUSE_NONE; // 捕获源的按下状态由统一捕获派发恢复。
        return false;
    }
    const int absX = element().x + ox;
    const int absY = element().y + oy;
    const int width = element().width;
    const int height = element().height;
    BOOL isMouseInRect = MouseIsInRect(absX, absY, width, height) && UIPointInClipRect(gMouseX, gMouseY, clipRect);
    bool isFullyInClip = !clipRect || (absX >= clipRect->left && absY >= clipRect->top && absX + width <= clipRect->right && absY + height <= clipRect->bottom);
    const uint8_t previousMouseState = mouseState;

    if (type == TYPE_TEXT_INPUT && (element().visible != 1 || !hitable)) {
        blurTextInput();
        mouseState = MOUSE_NONE;
        UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
        return false;
    }
    if (type == TYPE_TEXT_INPUT) {
        auto inputData = textInputData();
        syncTextInputText();
        if (!attached || (windowOwner && ((windowOwner->state & WIN_STATE_CLOSE) || (windowOwner->id >= WIN_MAX && !(windowOwner->state & WIN_STATE_ACTIVE))))) return false;
        if (inputData && inputData->focused && !isFullyInClip) {
            blurTextInput();
            mouseState = MOUSE_NONE;
            UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
            return false;
        }
        if ((gMouseClickState & platform::LegacyMouseEdge_LDown) && inputData && inputData->focused && !isMouseInRect) {
            blurTextInput();
        }
    }
    if (element().visible != 1 || !isHitable()) {
        mouseState = MOUSE_NONE;
        UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
        return false;
    }
    int eventFlags = MouseFlag_None;
    if (hasFocused && captured) {
        mouseState = MOUSE_NONE;
    } else if (hasFocused) {
        int currentMouseState = ui::WindowDrag_ReadMousePhase();

        const bool isDragOrPressState = currentMouseState == MCS_CALLBACK_A_ACTIVE || currentMouseState == MCS_CALLBACK_A_ENDING || currentMouseState == MCS_CALLBACK_B_ACTIVE || currentMouseState == MCS_CALLBACK_B_ENDING;

        if (isDragOrPressState) {
            if (gMouseEventState < MES_CAPTURE_THRESHOLD && isMouseInRect) {
                if (currentMouseState == MCS_CALLBACK_A_ACTIVE || currentMouseState == MCS_CALLBACK_B_ACTIVE)
                    eventFlags = MouseFlag_Dragging;
            }
        } else if (!gWindowHasFocus) {
            if (isMouseInRect) {
                eventFlags = MouseFlag_Hover;

                if (gMouseClickState & platform::LegacyMouseEdge_LDown) eventFlags = MouseFlag_Hover | MouseFlag_LDownEvent;
                if (gMouseClickState & platform::LegacyMouseEdge_RDown) eventFlags |= MouseFlag_RDownEvent;
                if (gMouseBtnDown2 & platform::LegacyMouseButton_Left) eventFlags |= MouseFlag_LHoldPhysical;
                if (gMouseBtnDown2 & platform::LegacyMouseButton_Right) eventFlags |= MouseFlag_RHoldPhysical;

                if (gMouseClickState & platform::LegacyMouseEdge_LDblClick) eventFlags |= MouseFlag_LDblClickEvent;
                if (gMouseClickState & platform::LegacyMouseEdge_RDblClick) eventFlags |= MouseFlag_RDblClickEvent;
                if (gMouseBtnDown & platform::LegacyMouseButton_Left) eventFlags |= MouseFlag_LRepeatDown;
                if (gMouseBtnDown & platform::LegacyMouseButton_Right) eventFlags |= MouseFlag_RRepeatDown;
            }
        }

        if (currentMouseState == MCS_CALLBACK_B_ACTIVE && UIPointInClipRect(ui::detail::WindowDrag_ReadX(), ui::detail::WindowDrag_ReadY(), clipRect) && (ui::detail::WindowDrag_ReadX() - absX) >= 0 && (ui::detail::WindowDrag_ReadX() - absX) < width && (ui::detail::WindowDrag_ReadY() - absY) >= 0 && (ui::detail::WindowDrag_ReadY() - absY) < height) {
            eventFlags |= MouseFlag_DragFinish;
        }

        const bool isPress = (eventFlags & (MouseFlag_Dragging | MouseFlag_LDownEvent | MouseFlag_LHoldPhysical | MouseFlag_LRepeatDown)) != 0;
        if (isPress) {
            this->mouseState = UIControl::MOUSE_PRESS;
        } else if (isMouseInRect) {
            this->mouseState = UIControl::MOUSE_HOVER;
        } else {
            this->mouseState = UIControl::MOUSE_NONE;
        }

        if (type == TYPE_TEXT_INPUT) {
            if (eventFlags & MouseFlag_LDownEvent) {
                focusTextInput();
            } else if ((gMouseClickState & platform::LegacyMouseEdge_LDown) && isTextInputFocused() && !isMouseInRect) {
                blurTextInput();
            }
        }
    } else {
        mouseState = MOUSE_NONE;
    }

    WindowControlRef controlRef{winId, generation, id};
    WindowEventBinding callbackCopy = callbacks;
    const bool pointerCaptured = windowOwner && WindowRuntime_BeginPointerDrag(*windowOwner, element(), eventFlags);
    UIEmitControlLeaveIfNeeded(controlRef, callbackCopy, previousMouseState, mouseState);
    int result = EmitControlEvents(controlRef, callbackCopy, eventFlags) | (pointerCaptured ? 1 : 0);
    if (result) {
        gMouseEventState = MES_CAPTURED;
        gWindowHasFocus = 1;

        if (ui::WindowDrag_ReadMousePhase() == MCS_CALLBACK_B_ACTIVE) ui::WindowDrag_ConsumeDrop();
    }
    return result;
}

int RendererDrawPngClipped(const wchar_t* path, RECT16* rect, int x, int y, int w, int h, COLORREF color, int layer, const UIClipRect* clip)
{
    render::PngRectangle source{}, target{};
    if (rect) source = {rect->left, rect->top, rect->right, rect->bottom};
    if (clip) target = {clip->left, clip->top, clip->right, clip->bottom};
    return render::DrawPngImage(path, rect ? &source : nullptr,
        x, y, w, h, color, static_cast<uint8_t>(layer), clip ? &target : nullptr);
}
PngTexture* RendererCreateSurfaceTexture(int w, int h) { return render::CreatePixelImage(w, h); }
bool RendererUpdateSurfaceTexture(PngTexture* texture, const uint32_t* pixels, int w, int h) { return render::UpdatePixelImage(texture, pixels, w, h); }
void RendererReleaseSurfaceTexture(PngTexture* texture) { render::ReleasePixelImage(texture); }
int RendererDrawSurfaceTextureClipped(PngTexture* texture, int x, int y, int w, int h, COLORREF color, int layer, const UIClipRect* clip)
{
    render::PngRectangle target{};
    if (clip) target = {clip->left, clip->top, clip->right, clip->bottom};
    return render::DrawPixelImage(texture, x, y, w, h, color, static_cast<uint8_t>(layer), clip ? &target : nullptr);
}
int RendererDrawCanvasClipped(int x, int y, int w, int h, uint8_t layer, const std::vector<CanvasShape>& shapes, const UIClipRect* clip)
{
    render::CanvasRectangle target{};
    if (clip) target = {clip->left, clip->top, clip->right, clip->bottom};
    return render::DrawCanvasClipped(x, y, w, h, layer, shapes, clip ? &target : nullptr);
}
} // namespace cg::ui
