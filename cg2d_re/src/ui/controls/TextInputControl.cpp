#include "../WindowComponents.hpp"

namespace cg::ui {
using namespace component_detail;
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
static void SetInputContextText(InputContext& context, std::wstring_view value, int maxLength)
{
    ApplyTextInputLimit(context, maxLength);
    // maxLength是Lua旧GBK字节容量；仅超限时截断，正常内部赋值不经窄编码往返。
    const auto bytes = text::ToGbk(value);
    const auto length = GbkPrefixLength(bytes, ClampTextInputMaxLength(maxLength));
    context.setText(length == bytes.size() ? std::wstring(value) : text::FromGbkWide(std::string_view(bytes).substr(0, length)));
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
UITextInputControl::UITextInputControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, TextInputData data, WindowEventBinding callbacks)
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

void UITextInputControl::setTextInputText(std::wstring_view text) {
    const std::wstring value(text); // 调用方可借用context.text，先持有再更新context。
    data.maxLength = ClampTextInputMaxLength(data.maxLength);
    SetInputContextText(data.context, value.c_str(), data.maxLength);
    data.lastText = data.context.text;
}

void UITextInputControl::syncTextInputText() {
    bool enter = RemoveInputContextLineBreaks(data.context);
    ClampInputContextText(data.context, data.maxLength);
    if (isTextInputFocused() && (platform::gKeyStates[0x0D] & KSF_TRIGGER)) {
        enter = true;
    }

    const std::wstring value = data.context.text;
    WindowControlRef controlRef{winId, generation, id};
    if (value != data.lastText) {
        data.lastText = value;
        if (callbacks && IsControlAttached(controlRef)) callbacks->text(controlRef, false, data.lastText);
    }
    if (enter) {
        if (callbacks && IsControlAttached(controlRef)) callbacks->text(controlRef, true, value);
    }
}

bool UITextInputControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UITextInputControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (element().visible != 1) {
        blurTextInput();
        return true;
    }
    const int absX = ox + element().x;
    const int absY = oy + element().y;
    const bool fullyVisible = !clipRect || (absX >= clipRect->left && absY >= clipRect->top && absX + element().width <= clipRect->right && absY + element().height <= clipRect->bottom);
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
        data.context.field14C = element().height;
        data.context.encrypt = 0;
        data.context.field130 = element().width > 6 ? element().width - 6 : 0;
        UpdateTextInputLineStart(data.context, data.maxLength);
        RendererDrawInputText(&data.context);
    }
    if (element().width > 0 && element().height > 0) {
        auto rectX = static_cast<int16_t>(absX);
        auto rectY = static_cast<int16_t>(absY);
        COLORREF background = focused ? static_cast<COLORREF>(0xa0000000) : static_cast<COLORREF>(0x70000000);
        RendererDrawRectClipped(rectX, rectY, element().width, element().height, LAYER_UI, background, clipRect);
        COLORREF border = focused ? 0xffffffff : 0xff707070;
        RendererDrawRectClipped(rectX, rectY, element().width, 1, LAYER_UI, border, clipRect);
        RendererDrawRectClipped(rectX, static_cast<int16_t>(absY + element().height - 1), element().width, 1, LAYER_UI, border, clipRect);
        RendererDrawRectClipped(rectX, rectY, 1, element().height, LAYER_UI, border, clipRect);
        RendererDrawRectClipped(static_cast<int16_t>(absX + element().width - 1), rectY, 1, element().height, LAYER_UI, border, clipRect);
    }
    return true;
}
} // namespace cg::ui
