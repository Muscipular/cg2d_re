#include "../LuaUI.hpp"

namespace cg::script {
static constexpr int LUA_REF_NONE = -1;
static UITextInputControl* gFocusedTextInput = nullptr;
int ClampTextInputMaxLength(int length) { return length <= 0 || length > TEXT_INPUT_MAX_LENGTH ? TEXT_INPUT_MAX_LENGTH : length; }
static std::size_t GbkPrefixLength(std::string_view value, int limit)
{
    std::size_t end = 0;
    while (end < value.size()) {
        const std::size_t step = static_cast<unsigned char>(value[end]) & 0x80 ? 2 : 1;
        if (end + step > value.size() || end + step > static_cast<std::size_t>(limit)) break;
        end += step;
    }
    return end;
}
static void ApplyTextInputLimit(InputContext& context, int maxLength)
{
    context.maxLine = 1;
    context.maxCharPerLine = ClampTextInputMaxLength(maxLength);
}
static void SetInputContextText(InputContext& context, const char* value, int maxLength)
{
    ApplyTextInputLimit(context, maxLength);
    const std::string_view bytes = value ? value : "";
    context.setText(text::FromGbkWide(bytes.substr(0, GbkPrefixLength(bytes, ClampTextInputMaxLength(maxLength)))));
    context.caretByteOffset = context.totalByteLength;
    context.field158 = 0;
}
static bool RemoveInputContextLineBreaks(InputContext& context)
{
    const auto oldSize = context.text.size();
    std::erase_if(context.text, [](wchar_t ch) { return ch == L'\r' || ch == L'\n'; });
    if (context.text.size() == oldSize) return false;
    context.totalByteLength = static_cast<int>(context.text.size());
    context.caretByteOffset = context.totalByteLength;
    context.field158 = 0;
    return true;
}
static bool ClampInputContextText(InputContext& context, int maxLength)
{
    ApplyTextInputLimit(context, maxLength);
    const auto bytes = text::ToGbk(context.text);
    const auto length = GbkPrefixLength(bytes, ClampTextInputMaxLength(maxLength));
    const bool changed = length != bytes.size();
    if (changed) context.text = text::FromGbkWide(std::string_view(bytes).substr(0, length));
    context.totalByteLength = static_cast<int>(context.text.size());
    context.caretByteOffset = static_cast<int>(text::Prefix(context.text, std::min<std::size_t>(context.text.size(), std::max(0, context.caretByteOffset))));
    context.field158 = static_cast<int>(text::Prefix(context.text, std::min(context.field158, context.caretByteOffset)));
    return changed;
}
static void UpdateTextInputLineStart(InputContext& context, int maxLength)
{
    ClampInputContextText(context, maxLength);
    if (context.field130 <= 0) { context.field158 = 0; return; }
    while (context.field158 < context.caretByteOffset) {
        const std::wstring_view visible(context.text.data() + context.field158, context.caretByteOffset - context.field158);
        if (render::TextMeasure(visible, context.field13C) <= context.field130) break;
        context.field158 = static_cast<int>(text::Next(context.text, context.field158));
    }
}
static void PushControlRef(lua_State* L, LuaControlRef ref) {
    auto data = static_cast<LuaControlRef*>(lua_newuserdata(L, sizeof(LuaControlRef)));
    *data = ref;
    luaL_setmetatable(L, CName(LuaControlRef));
}

static int CallControlTextCallback(lua_State* L, int callbackRef, const char* name, LuaControlRef controlRef, const char* text) {
    if (!IsLuaControlAttached(controlRef)) return 0;
    if (callbackRef == LUA_REF_NONE) {
        return 0;
    }
    int top = lua_gettop(L);
    int result = 0;
    lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
    if (lua_isfunction(L, -1)) {
        PushControlRef(L, controlRef);
        lua_pushstring(L, text ? text : "");
        if (lua_pcall(L, 2, 1, 0) != 0) {
            m_log_i("[LUA] call lua %s error: %s\n", name, lua_tostring(L, -1));
        } else if (!lua_isnil(L, -1)) {
            result = lua_toboolean(L, -1) ? 1 : 0;
        }
    }
    lua_settop(L, top);
    return result;
}

UITextInputControl::UITextInputControl(int winId, uint32_t generation, int id, const char* name, short x, short y, short w, short h, uint8_t visible, TextInputData data, LuaCallbackRefs callbacks)
    : UIControl(winId, generation, id, TYPE_TEXT_INPUT, name, x, y, w, h, visible, callbacks), data(std::move(data)) {
    this->data.maxLength = ClampTextInputMaxLength(this->data.maxLength);
    setTextInputText(this->data.lastText.c_str());
}

UITextInputControl::~UITextInputControl() {
    blurTextInput();
}

UIControl::TextInputData* UITextInputControl::textInputData() {
    return &data;
}

const UIControl::TextInputData* UITextInputControl::textInputData() const {
    return &data;
}

bool UITextInputControl::isTextInputFocused() {
    return data.focused && gCurrentInputData == &data.context;
}

void UITextInputControl::blurTextInput() {
    if (!data.focused && gFocusedTextInput != this && gCurrentInputData != &data.context) {
        return;
    }
    InputContext* restoreInput = data.previousInput ? data.previousInput : gDefaultInputData;
    data.focused = false;
    data.previousInput = nullptr;
    if (gFocusedTextInput == this) {
        gFocusedTextInput = nullptr;
    }
    if (gCurrentInputData == &data.context || !gCurrentInputData) {
        InputSetContext(restoreInput);
    }
}

void UITextInputControl::focusTextInput() {
    if (gFocusedTextInput && gFocusedTextInput != this) {
        gFocusedTextInput->blurTextInput();
    }
    if (!data.focused) {
        data.previousInput = gCurrentInputData && gCurrentInputData != &data.context ? gCurrentInputData : gDefaultInputData;
        data.focused = true;
    }
    gFocusedTextInput = this;
    ClampInputContextText(data.context, data.maxLength);
    InputSetContext(&data.context);
}

void UITextInputControl::setTextInputText(const char* text) {
    std::string value = text ? text : "";
    data.maxLength = ClampTextInputMaxLength(data.maxLength);
    SetInputContextText(data.context, value.c_str(), data.maxLength);
    data.lastText = text::ToGbk(data.context.text);
}

void UITextInputControl::syncTextInputText() {
    bool enter = RemoveInputContextLineBreaks(data.context);
    ClampInputContextText(data.context, data.maxLength);
    if (isTextInputFocused() && (platform::gKeyStates[0x0D] & KSF_TRIGGER)) {
        enter = true;
    }

    std::string value = text::ToGbk(data.context.text);
    LuaControlRef controlRef{winId, generation, id};
    if (value != data.lastText) {
        data.lastText = value;
        CallControlTextCallback(callbacks.owner, callbacks.onChange, "UIControl.onChange", controlRef, data.lastText.c_str());
    }
    if (enter) {
        CallControlTextCallback(callbacks.owner, callbacks.onEnter, "UIControl.onEnter", controlRef, value.c_str());
    }
}

bool UITextInputControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UITextInputControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (visible != 1) {
        blurTextInput();
        return true;
    }
    const int absX = ox + x;
    const int absY = oy + y;
    const bool fullyVisible = !clipRect || (absX >= clipRect->left && absY >= clipRect->top && absX + w <= clipRect->right && absY + h <= clipRect->bottom);
    if (!fullyVisible) {
        blurTextInput();
    }
    const bool focused = isTextInputFocused();
    if (fullyVisible) {
        RendererBreakWindowTextBatch();
        data.context.x = absX + 3;
        data.context.y = absY + 2;
        data.context.field138 = FONT_LAYER_WINDOW;
        data.context.field13C = data.font;
        data.context.field140 = data.color;
        ApplyTextInputLimit(data.context, data.maxLength);
        data.context.field14C = h;
        data.context.encrypt = 0;
        data.context.field130 = w > 6 ? w - 6 : 0;
        UpdateTextInputLineStart(data.context, data.maxLength);
        RendererDrawInputText(&data.context);
    }
    if (w > 0 && h > 0) {
        auto rectX = static_cast<int16_t>(absX);
        auto rectY = static_cast<int16_t>(absY);
        COLORREF background = focused ? static_cast<COLORREF>(0xa0000000) : static_cast<COLORREF>(0x70000000);
        RendererDrawRectClipped(rectX, rectY, w, h, LAYER_UI, background, clipRect);
        COLORREF border = focused ? 0xffffffff : 0xff707070;
        RendererDrawRectClipped(rectX, rectY, w, 1, LAYER_UI, border, clipRect);
        RendererDrawRectClipped(rectX, static_cast<int16_t>(absY + h - 1), w, 1, LAYER_UI, border, clipRect);
        RendererDrawRectClipped(rectX, rectY, 1, h, LAYER_UI, border, clipRect);
        RendererDrawRectClipped(static_cast<int16_t>(absX + w - 1), rectY, 1, h, LAYER_UI, border, clipRect);
    }
    return true;
}
} // namespace cg::script
