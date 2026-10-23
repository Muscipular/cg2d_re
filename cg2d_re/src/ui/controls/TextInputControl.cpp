#include "../WindowComponents.hpp"

namespace cg::ui {
using namespace component_detail;
static WindowElementRef focusedInput;
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
    context.visibleStartOffset = 0;
}
static bool RemoveInputContextLineBreaks(InputContext& context)
{
    const auto oldSize = context.text.size();
    std::erase_if(context.text, [](wchar_t ch) { return ch == L'\r' || ch == L'\n'; });
    if (context.text.size() == oldSize) return false;
    context.totalByteLength = static_cast<int>(context.text.size());
    context.caretByteOffset = context.totalByteLength;
    context.visibleStartOffset = 0;
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
    context.visibleStartOffset = static_cast<int>(text::Prefix(context.text, std::min(context.visibleStartOffset, context.caretByteOffset)));
    return changed;
}
static void UpdateTextInputLineStart(InputContext& context, int maxLength)
{
    ClampInputContextText(context, maxLength);
    if (context.visibleWidth <= 0) { context.visibleStartOffset = 0; return; }
    while (context.visibleStartOffset < context.caretByteOffset) {
        const std::wstring_view visible(context.text.data() + context.visibleStartOffset, context.caretByteOffset - context.visibleStartOffset);
        if (render::TextMeasure(visible, context.fontType) <= context.visibleWidth) break;
        context.visibleStartOffset = static_cast<int>(text::Next(context.text, context.visibleStartOffset));
    }
}
void WindowElement_ForgetInputRestore(const WindowElement& element) {
    auto* window = WindowRuntime_FindInstance(focusedInput.winId);
    if (!window || window->generation != focusedInput.generation) return;
    auto* focused = window->FindElementById(focusedInput.elementId);
    if (!focused || focused->creationSerial != focusedInput.creationSerial || focused == &element
        || focused->type != WindowElementType_Input || !focused->data.input) return;
    auto* state = focused->data.input->ownedInput;
    if (!state) return;
    // 原生焦点槽实际借用Input或Graphic载荷，不读取联合体的非活动成员。
    const void* payload = element.type == WindowElementType_Input ? static_cast<void*>(element.data.input)
        : element.type == WindowElementType_Graphic ? static_cast<void*>(element.data.graphic) : nullptr;
    if (payload && state->previousElement == payload) state->previousElement = nullptr;
    if (element.type == WindowElementType_Input && element.data.input
        && state->previousInput == element.data.input->context) state->previousInput = nullptr;
}
void WindowElement_BlurInput(WindowElement& element) {
    if (element.type != WindowElementType_Input || !element.data.input || !element.data.input->ownedInput) return;
    auto& data = *element.data.input->ownedInput;
    if (!data.focused && gCurrentInputData != &data.context) return;
    auto* restoreInput = data.previousInput ? data.previousInput : gDefaultInputData;
    data.focused = false;
    data.previousInput = nullptr;
    if (focusedInput.winId == element.owner->id && focusedInput.elementId == element.elementId) focusedInput = {};
    if (detail::gFocusedElementDataD45F94 == element.data.input) detail::gFocusedElementDataD45F94 = data.previousElement;
    data.previousElement = nullptr;
    if (gCurrentInputData == &data.context || !gCurrentInputData) InputSetContext(restoreInput);
}
void WindowElement_FocusInput(WindowElement& element) {
    if (element.type != WindowElementType_Input || !element.data.input || !element.data.input->ownedInput) return;
    if (auto* previous = WindowRuntime_ResolveElement(focusedInput); previous && previous != &element) WindowElement_BlurInput(*previous);
    auto& data = *element.data.input->ownedInput;
    if (!data.focused) {
        data.previousInput = gCurrentInputData && gCurrentInputData != &data.context ? gCurrentInputData : gDefaultInputData;
        data.previousElement = detail::gFocusedElementDataD45F94;
        data.focused = true;
    }
    focusedInput = WindowRuntime_ElementRef(element);
    ClampInputContextText(data.context, data.maxLength);
    detail::gFocusedElementDataD45F94 = element.data.input;
    InputSetContext(&data.context);
}
void WindowInput_SetText(WindowInputData& input, std::wstring_view text) {
    if (!input.context) return;
    const std::wstring value(text);
    if (auto* data = input.ownedInput) {
        data->maxLength = ClampTextInputMaxLength(data->maxLength);
        SetInputContextText(data->context, value.c_str(), data->maxLength);
        data->lastText = data->context.text;
    } else input.context->setText(value);
}
void WindowElement_SyncInputText(WindowElement& element) {
    auto* data = element.data.input ? element.data.input->ownedInput : nullptr;
    if (!data) return;
    bool enter = RemoveInputContextLineBreaks(data->context);
    ClampInputContextText(data->context, data->maxLength);
    if (data->focused && gCurrentInputData == &data->context && (platform::gKeyStates[0x0D] & KSF_TRIGGER)) enter = true;
    const std::wstring value = data->context.text;
    const auto ref = WindowRuntime_ElementRef(element);
    if (value != data->lastText) {
        data->lastText = value;
        if (element.owner->binding) element.owner->binding->elementText(element, false, value);
    }
    if (enter) if (auto* live = WindowRuntime_ResolveElement(ref); live && !live->detaching && live->owner->binding)
        live->owner->binding->elementText(*live, true, value);
}
void WindowElement_PrepareInput(WindowElement& element, int ox, int oy, const UIClipRect* clip) {
    auto* data = element.data.input ? element.data.input->ownedInput : nullptr;
    if (!data) return;
    const int x = ox + element.x, y = oy + element.y;
    if (element.visible != 1 || !element.hitable || (clip && (x < clip->left || y < clip->top
        || x + element.width > clip->right || y + element.height > clip->bottom))) WindowElement_BlurInput(element);
    // 原生DrawInputText从field130/134取正文原点，再写x/y供光标与IME使用。
    data->context.textX = x + 3;
    data->context.textY = y + 2;
    data->context.x = data->context.textX;
    data->context.y = data->context.textY;
    data->context.renderMode = FONT_LAYER_WINDOW;
    data->context.fontType = data->font;
    data->context.color = data->color;
    ApplyTextInputLimit(data->context, data->maxLength);
    data->context.lineHeight = element.height;
    data->context.encrypt = 0;
    data->context.visibleWidth = element.width > 6 ? element.width - 6 : 0;
    UpdateTextInputLineStart(data->context, data->maxLength);
}
} // namespace cg::ui
