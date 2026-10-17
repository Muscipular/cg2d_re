#include "../LuaUI.hpp"

namespace cg::script {
static constexpr int LUA_REF_NONE = -1;

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

int RendererDrawRectClipped(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t layer, COLORREF color, const UIClipRect* clipRect) {
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



static int GetAcpCharLength(const char* text, int offset) {
    if (!text || !text[offset]) {
        return 0;
    }
    if ((static_cast<unsigned char>(text[offset]) & 0x80) && text[offset + 1]) {
        return 2;
    }
    return 1;
}

static int MeasureAcpChar(const char* text, int offset, int length, char fontType) {
    char ch[3] = {};
    memcpy(ch, text + offset, length);
    int width = FontMeasure(ch, fontType, length);
    if (width <= 0) {
        width = render::gFontMetrics[static_cast<uint8_t>(fontType)].sWidth * length;
    }
    return width;
}

void RendererDrawTextClipped(__int16 x, __int16 y, unsigned __int8 layer, char fontType, uint8_t colorType, const char* msg, char hoverMode, char hoverFrame, const UIClipRect* clipRect) {
    if (!msg || !msg[0]) {
        return;
    }
    if (!clipRect) {
        InfoRenderText(x, y, layer, fontType, colorType, msg, hoverMode, hoverFrame);
        return;
    }

    const auto& font = render::gFontMetrics[static_cast<uint8_t>(fontType)];
    int fontHeight = font.fontHeight > 0 ? font.fontHeight : font.sHeight;
    if (fontHeight <= 0) {
        fontHeight = 16;
    }
    if (y < clipRect->top || y + fontHeight > clipRect->bottom) {
        return;
    }

    std::string text;
    int drawX = x;
    int currentX = x;
    int len = static_cast<int>(strlen(msg));
    for (int i = 0; i < len;) {
        int charLen = GetAcpCharLength(msg, i);
        if (charLen <= 0 || i + charLen > len) {
            break;
        }
        int charWidth = MeasureAcpChar(msg, i, charLen, fontType);
        int nextX = currentX + charWidth;
        if (nextX <= clipRect->left) {
            currentX = nextX;
            i += charLen;
            continue;
        }
        if (currentX < clipRect->left || nextX > clipRect->right) {
            currentX = nextX;
            i += charLen;
            continue;
        }
        if (text.empty()) {
            drawX = currentX;
        }
        text.append(msg + i, charLen);
        currentX = nextX;
        i += charLen;
    }

    if (!text.empty()) {
        InfoRenderText(static_cast<__int16>(drawX), y, layer, fontType, colorType, text.c_str(), hoverMode, hoverFrame);
    }
}

static void PushControlRef(lua_State* L, LuaControlRef ref) {
    auto data = static_cast<LuaControlRef*>(lua_newuserdata(L, sizeof(LuaControlRef)));
    *data = ref;
    luaL_setmetatable(L, CName(LuaControlRef));
}

static int CallControlCallback(lua_State* L, int callbackRef, const char* name, LuaControlRef controlRef, int eventFlags) {
    if (!IsLuaControlAttached(controlRef)) return 0;
    if (callbackRef == LUA_REF_NONE) {
        return 1;
    }
    int top = lua_gettop(L);
    int result = 0;
    lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
    if (lua_isfunction(L, -1)) {
        PushControlRef(L, controlRef);
        lua_pushinteger(L, eventFlags);
        if (lua_pcall(L, 2, 1, 0) != 0) {
            m_log_i("[LUA] call lua %s error: %s\n", name, lua_tostring(L, -1));
        } else if (!lua_isnil(L, -1)) {
            result = lua_toboolean(L, -1) ? 1 : 0;
        }
    }
    lua_settop(L, top);
    return result;
}

static int EmitControlEvents(LuaControlRef controlRef, LuaCallbackRefs callbacks, int eventFlags) {
    if (eventFlags == MouseFlag_None) {
        return 0;
    }
    int result = 0;
    do {
        result |= CallControlCallback(callbacks.owner, callbacks.onEvent, "UIControl.onEvent", controlRef, eventFlags);
        if (result & 2) {
            break;
        }
        if (eventFlags & (MouseFlag_Hover | MouseFlag_DragFinish)) {
            result |= CallControlCallback(callbacks.owner, callbacks.onHover, "UIControl.onHover", controlRef, eventFlags);
            if (result & 2) {
                break;
            }
        }
        if (eventFlags & MouseFlag_LDownEvent) {
            result |= CallControlCallback(callbacks.owner, callbacks.onClick, "UIControl.onClick", controlRef, eventFlags);
            if (result & 2) {
                break;
            }
        }
        if (eventFlags & (MouseFlag_Dragging | MouseFlag_LDownEvent | MouseFlag_LHoldPhysical | MouseFlag_LRepeatDown)) {
            result |= CallControlCallback(callbacks.owner, callbacks.onPress, "UIControl.onPress", controlRef, eventFlags);
            if (result & 2) {
                break;
            }
        }
        if (eventFlags & MouseFlag_Dragging) {
            result |= CallControlCallback(callbacks.owner, callbacks.onDrag, "UIControl.onDrag", controlRef, eventFlags);
            if (result & 2) {
                break;
            }
        }
        if (eventFlags & MouseFlag_DragFinish) {
            result |= CallControlCallback(callbacks.owner, callbacks.onDrop, "UIControl.onDrop", controlRef, eventFlags);
        }
    } while (false);
    return result;
}

void UIEmitControlLeaveIfNeeded(LuaControlRef controlRef, LuaCallbackRefs callbacks, uint8_t previousMouseState, uint8_t mouseState) {
    if (previousMouseState != UIControl::MOUSE_HOVER || mouseState != UIControl::MOUSE_NONE) {
        return;
    }
    CallControlCallback(callbacks.owner, callbacks.onLeave, "UIControl.onLeave", controlRef, MouseFlag_Leave);
}

UIControl::UIControl(int winId, uint32_t generation, int id, TYPE type, const char* name, short x, short y, short w, short h, uint8_t visible, LuaCallbackRefs callbacks)
    : winId(winId), generation(generation), id(id), type(type), visible(visible), mouseState(MOUSE_NONE), name(name == nullptr ? "" : name), x(x), y(y), w(w), h(h), callbacks(callbacks) {
}

UIControl::~UIControl() {
    callbacks.clear(callbacks.owner);
}

bool UIControl::render(short x, short y) {
    (void) x;
    (void) y;
    return true;
}

bool UIControl::render(short x, short y, const UIClipRect* clipRect) {
    (void) clipRect;
    return render(x, y);
}

bool UIControl::isHitable() const {
    if (visible != 1) {
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

void UIControl::setTextInputText(const char* text) {
    (void) text;
}

void UIControl::syncTextInputText() {
}

bool UIControl::process(int index, int hasFocused, int captured) {
    return process(index, hasFocused, captured, nullptr, static_cast<short>(gCurrentWindow->x), static_cast<short>(gCurrentWindow->y));
}

bool UIControl::process(int index, int hasFocused, int captured, const UIClipRect* clipRect, short ox, short oy) {
    const int absX = this->x + ox;
    const int absY = this->y + oy;
    const int width = this->w;
    const int height = this->h;
    BOOL isMouseInRect = MouseIsInRect(absX, absY, width, height) && UIPointInClipRect(gMouseX, gMouseY, clipRect);
    bool isFullyInClip = !clipRect || (absX >= clipRect->left && absY >= clipRect->top && absX + width <= clipRect->right && absY + height <= clipRect->bottom);
    const uint8_t previousMouseState = mouseState;

    if (type == TYPE_TEXT_INPUT && (visible != 1 || !hitable)) {
        blurTextInput();
        mouseState = MOUSE_NONE;
        UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
        return false;
    }
    if (type == TYPE_TEXT_INPUT) {
        auto inputData = textInputData();
        syncTextInputText();
        if (inputData && inputData->focused && !isFullyInClip) {
            blurTextInput();
            mouseState = MOUSE_NONE;
            UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
            return false;
        }
        if ((gMouseClickState & 0x01) && inputData && inputData->focused && !isMouseInRect) {
            blurTextInput();
        }
    }
    if (visible != 1 || !isHitable()) {
        mouseState = MOUSE_NONE;
        UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
        return false;
    }
    int eventFlags = MouseFlag_None;
    if (hasFocused && captured) {
        mouseState = MOUSE_NONE;
    } else if (hasFocused) {
        int currentMouseState = ReadMousePhase();

        const bool isDragOrPressState = currentMouseState == MCS_CALLBACK_A_ACTIVE || currentMouseState == MCS_CALLBACK_A_ENDING || currentMouseState == MCS_CALLBACK_B_ACTIVE || currentMouseState == MCS_CALLBACK_B_ENDING;

        if (isDragOrPressState) {
            if (gMouseEventState < MES_CAPTURE_THRESHOLD && isMouseInRect) {
                if (currentMouseState == MCS_CALLBACK_A_ACTIVE || currentMouseState == MCS_CALLBACK_B_ACTIVE)
                    eventFlags = MouseFlag_Dragging;
            }
        } else if (!gWindowHasFocus) {
            if (isMouseInRect) {
                eventFlags = MouseFlag_Hover;

                if (gMouseClickState & 0x01) eventFlags = MouseFlag_Hover | MouseFlag_LDownEvent;
                if (gMouseClickState & 0x02) eventFlags |= MouseFlag_RDownEvent;
                if (gMouseBtnDown2 & 0x01) eventFlags |= MouseFlag_LHoldPhysical;
                if (gMouseBtnDown2 & 0x02) eventFlags |= MouseFlag_RHoldPhysical;

                if (gMouseClickState & 0x10) eventFlags |= MouseFlag_LDblClickEvent;
                if (gMouseClickState & 0x20) eventFlags |= MouseFlag_RDblClickEvent;
                if (gMouseBtnDown & 0x01) eventFlags |= MouseFlag_LRepeatDown;
                if (gMouseBtnDown & 0x02) eventFlags |= MouseFlag_RRepeatDown;
            }
        }

        if (currentMouseState == MCS_CALLBACK_B_ACTIVE && UIPointInClipRect(ui::detail::WindowDrag_ReadX(), ui::detail::WindowDrag_ReadY(), clipRect) && (ui::detail::WindowDrag_ReadX() - absX) >= 0 && (ui::detail::WindowDrag_ReadX() - absX) < width && (ui::detail::WindowDrag_ReadY() - absY) >= 0 && (ui::detail::WindowDrag_ReadY() - absY) < height) {
            eventFlags |= MouseFlag_DragFinish;
        }

        const bool isPress = (eventFlags & (MouseFlag_Dragging | MouseFlag_LDownEvent | MouseFlag_LHoldPhysical | MouseFlag_LRepeatDown)) != 0;
        const bool isHover = (eventFlags & (MouseFlag_Hover | MouseFlag_DragFinish)) != 0;
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
            } else if ((gMouseClickState & 0x01) && isTextInputFocused() && !isMouseInRect) {
                blurTextInput();
            }
        }
    } else {
        mouseState = MOUSE_NONE;
    }

    LuaControlRef controlRef{winId, generation, id};
    LuaCallbackRefs callbackCopy = callbacks;
    UIEmitControlLeaveIfNeeded(controlRef, callbackCopy, previousMouseState, mouseState);
    int result = EmitControlEvents(controlRef, callbackCopy, eventFlags);
    if (result) {
        gMouseEventState = MES_CAPTURED;
        gWindowHasFocus = 1;

        if (ReadMousePhase() == MCS_CALLBACK_B_ACTIVE) ConsumeLuaDrop();
    }
    return result;
}

int RendererDrawPngClipped(const char* path, RECT16* rect, int x, int y, int w, int h, COLORREF color, int layer, const UIClipRect* clip)
{
    render::PngRectangle source{}, target{};
    if (rect) source = {rect->left, rect->top, rect->right, rect->bottom};
    if (clip) target = {clip->left, clip->top, clip->right, clip->bottom};
    return render::DrawPngImage(text::FromGbkWide(path).c_str(), rect ? &source : nullptr,
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
} // namespace cg::script
