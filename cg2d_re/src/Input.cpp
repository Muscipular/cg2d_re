#include "cg/Input.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/render/Font.hpp"
#include "cg/Blacklist.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/ui/WindowRuntime.hpp"

#include "InputKeyDispatchDependencies.hpp"
#include "platform/LegacyFormat.hpp"
#include "platform/LegacyStdio.hpp"
#include "platform/WindowProcedureState.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <cstring>
#include <format>
#include <type_traits>
#include <algorithm>
#include <string>
#include <system_error>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BB02、0x0053BC41、0x0053BCCD、0x0053C680；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053BC0F；直接沿MSVC CRT。
#include <cctype>
#include <direct.h> // 原CRT接口：0x0053C654、0x0053BEC0、0x0053C790、0x0053BE40、0x0053BE17、0x0053BDEF；原参数／返回合同由直接调用承接。

namespace cg {

// UTE01：原仅单RET，无读取/写入，真实空操作保持；其它父残值机器ABI待。
// 原函数：0x00509080。
void NullSub_509080() noexcept
{
}

} // namespace cg

namespace cg::input {

InputContext::InputContext(LegacyInputStorage* const legacy) noexcept
    : legacy_(legacy), x(fields().x),
      y(fields().y),
      caretByteOffset(fields().caretByteOffset),
      totalByteLength(fields().totalByteLength),
      field130(fields().field130),
      field134(fields().field134),
      field138(fields().field138),
      field13C(fields().field13C),
      field140(fields().field140),
      maxLine(fields().maxLine),
      maxCharPerLine(fields().maxCharPerLine),
      field14C(fields().field14C),
      encrypt(fields().encrypt),
      field154(fields().field154),
      field158(fields().field158) {}
InputContext::InputContext() noexcept : InputContext(nullptr) {}
InputContext::InputContext(LegacyInputStorage& legacy) noexcept : InputContext(&legacy) {}
InputContext::InputContext(const InputContext& other) : InputContext() { *this = other; }
InputContext& InputContext::operator=(const InputContext& other)
{
    if (this == &other) { return *this; }
    const std::wstring contents = ContextText(other);
    setText(contents);
    x = other.x;
    y = other.y;
    const std::size_t caret = ContextUtf16Offset(other, other.caretByteOffset);
    caretByteOffset = static_cast<std::int32_t>(legacy_ != nullptr ? cg::text::ToGbk(std::wstring_view(contents).substr(0, caret)).size() : caret);
    totalByteLength = static_cast<std::int32_t>(legacy_ != nullptr ? cg::text::ToGbk(contents).size() : contents.size());
    field130 = other.field130;
    field134 = other.field134;
    field13C = other.field13C;
    field140 = other.field140;
    maxLine = other.maxLine;
    maxCharPerLine = other.maxCharPerLine;
    field14C = other.field14C;
    encrypt = other.encrypt;
    field154 = other.field154;
    const std::size_t visible = ContextUtf16Offset(other, other.field158);
    field158 = static_cast<std::int32_t>(legacy_ != nullptr ? cg::text::ToGbk(std::wstring_view(contents).substr(0, visible)).size() : visible);
    std::memcpy(field138, other.field138, sizeof(field138));
    return *this;
}
bool InputContext::empty() const noexcept
{
    return legacy_ != nullptr ? legacy_->text[0] == '\0' : text.empty();
}
std::size_t InputContext::length() const { return ContextText(*this).size(); }
void InputContext::clearText() noexcept
{
    if (legacy_ != nullptr) { legacy_->text[0] = '\0'; }
    text.clear();
}
void InputContext::reset() noexcept
{
    clearText();
    fields() = {};
}
void InputContext::setText(const std::wstring_view value)
{
    if (legacy_ != nullptr) { cg::text::StoreGbk(legacy_->text, sizeof(legacy_->text), value); }
    else { text.assign(value); }
}

InputContext gInputContext626AA0 {}; // 原全局：0x00626AA0。
InputContext gInputContexts5F9758[24] {}; // 原全局：0x005F9758。
InputContext gInputContextD66CA8 {}; // 原全局：0x00D66CA8。
InputContext gInputContextCB03F0 {}; // 原全局：0x00CB03F0。
InputContext gInputContextCC46E0 {}; // 原全局：0x00CC46E0。
InputContext gInputContextCC2080 {}; // 原全局：0x00CC2080。
InputContext gInputContextD2D828 {}; // 原全局：0x00D2D828。
InputContext gInputContextCA0180 {}; // 原全局：0x00CA0180。
InputContext gInputContextCB0610 {}; // 原全局：0x00CB0610。
InputContext gInputContextCB0028 {}; // 原全局：0x00CB0028。
InputContext gInputContextCC2378 {}; // 原全局：0x00CC2378。
InputContext gInputContextCF4E60 {}; // 原全局：0x00CF4E60。
InputContext gInputContextD2DE70 {}; // 原全局：0x00D2DE70。
InputContext gInputContextD2DFD0 {}; // 原全局：0x00D2DFD0。
InputContext gInputContextD2DC10 {}; // 原全局：0x00D2DC10。
InputContext gInputContextD2D4A0 {}; // 原全局：0x00D2D4A0。
InputContext gInputContextD64CB0 {}; // 原全局：0x00D64CB0。
InputContext gInputContextD2D670 {}; // 原全局：0x00D2D670。
InputContext gInputContextCAFEC8 {}; // 原全局：0x00CAFEC8。
InputContext gInputContextC8B1B8 {}; // 原全局：0x00C8B1B8。
InputContext gInputContextCC21E8 {}; // 原全局：0x00CC21E8。
InputContext gInputContextCB0878 {}; // 原全局：0x00CB0878。
InputContext gInputContextCA0018 {}; // 原全局：0x00CA0018。
InputContext gInputContextCC49C0 {}; // 原全局：0x00CC49C0。
InputContext gInputContextCA02E0 {}; // 原全局：0x00CA02E0。
InputContext gInputContextD2E130 {}; // 原全局：0x00D2E130。
InputContext gInputContextD2E2E0 {}; // 原全局：0x00D2E2E0。
InputContext gInputContextCB01F8 {}; // 原全局：0x00CB01F8。
InputContext gInputContextCF47E0 {}; // 原全局：0x00CF47E0。
InputContext gInputContext1050B80 {}; // 原全局：0x01050B80。
InputContext gInputContext1051348 {}; // 原全局：0x01051348。
InputContext gInputContexts1050F18[3] {}; // 原全局：0x01050F18。
InputContext* gCurrentInputData = nullptr; // 原全局：0x00626C04。
std::int32_t gValueC9A96C = 0; // 原全局：0x00C9A96C。
std::int32_t gValue626C0C = 0; // 原全局：0x00626C0C。
std::int32_t gValue626C14 = 0; // 原全局：0x00626C14。
std::int32_t gValue626C20 = 0; // 原全局：0x00626C20。
std::int32_t gValue626C28 = 0; // 原全局：0x00626C28。


namespace {

struct LegacyInput593850 {
    char prefix[8];
    LegacyInputStorage context;
};

LegacyInput593850 gLegacyInput593850 {}; // 原全局：0x00593850。
static_assert(offsetof(LegacyInput593850, context) == 8);
static_assert(sizeof(LegacyInput593850) == 0x164);

constexpr std::array<std::uint8_t, 8> kEscapePairs58E944 {{ // 原常量参考：0x0058E944。
    '\n', 'n', ',', 'c', '|', 'z', '\\', 'y',
}};

constexpr wchar_t kBlacklistSelfText554938[] = // 原常量参考：0x00554938。
    L"您…如此厭惡自己的聲音嗎？";
constexpr wchar_t kBlacklistExistingFormat554928[] = // 原常量参考：0x00554928。
    L"{}已在黑名單中";
constexpr wchar_t kBlacklistAddedFormat5532C4[] = // 原常量参考：0x005532C4。
    L"已將{}加入黑名單";
constexpr wchar_t kBlacklistFailedFormat5532B0[] = // 原常量参考：0x005532B0。
    L"{}加入黑名單失敗";

std::int8_t SignedByte(const char value)
{
    return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
}

// UTL02：旧预设视图借用同一聊天GBK字节owner；原生上下文使用动态UTF-16。
bool HasLegacyTextStorage(const InputContext& context) noexcept
{
    return context.legacyStorage() != nullptr;
}

std::size_t StoredOffset(const InputContext& context, const std::wstring_view utf16, const std::size_t offset)
{
    const std::size_t prefix = text::Prefix(utf16, offset);
    return HasLegacyTextStorage(context) ? text::ToGbk(utf16.substr(0, prefix)).size() : prefix;
}

class EditText final {
public:
    explicit EditText(InputContext& value)
        : context(value), contents(ContextText(value)),
          caret(ContextUtf16Offset(value, value.caretByteOffset)),
          visible(ContextUtf16Offset(value, value.field158)) {}
    bool commit()
    {
        context.setText(contents);
        context.totalByteLength = static_cast<std::int32_t>(HasLegacyTextStorage(context) ? text::ToGbk(contents).size() : contents.size());
        context.caretByteOffset = static_cast<std::int32_t>(StoredOffset(context, contents, caret));
        context.field158 = static_cast<std::int32_t>(StoredOffset(context, contents, visible));
        return true;
    }
    InputContext& context;
    std::wstring contents;
    std::size_t caret;
    std::size_t visible;
};

} // namespace

std::wstring ContextText(const InputContext& context)
{
    return HasLegacyTextStorage(context) ? text::FromGbkWide(context.legacyStorage()->text) : context.text;
}

std::size_t ContextUtf16Offset(const InputContext& context, const std::int32_t storedByteOffset)
{
    const std::size_t offset = static_cast<std::size_t>(storedByteOffset);
    if (HasLegacyTextStorage(context)) {
        const std::string_view stored(context.legacyStorage()->text);
        return text::FromGbkWide(stored.substr(0, (std::min)(offset, stored.size()))).size();
    }
    return text::Prefix(context.text, offset);
}

void SetVisibleUtf16Offset(InputContext& context, const std::size_t utf16Offset)
{
    const std::wstring utf16 = ContextText(context);
    context.field158 = static_cast<std::int32_t>(StoredOffset(context, utf16, utf16Offset));
}

std::vector<TextLine> SplitLines(const InputContext& context)
{
    const std::wstring contents = ContextText(context);
    const std::int32_t widthLimit = context.maxCharPerLine * render::gFontMetrics[static_cast<std::size_t>(context.field13C)].sWidth;
    std::vector<TextLine> lines;
    std::size_t start = 0;
    do {
        std::size_t end = start;
        while (end < contents.size() && contents[end] != '\r') {
            const std::size_t next = text::Next(contents, end);
            if (widthLimit > 0 && end > start
                && render::Font::measure(contents.c_str() + start, context.field13C, static_cast<std::int32_t>(next - start)) > widthLimit) {
                break;
            }
            end = next;
        }
        const std::size_t nextStart = end < contents.size() && contents[end] == '\r' ? end + 1 : end;
        lines.push_back({start, end, nextStart});
        start = nextStart;
    } while (start < contents.size());
    if (!contents.empty() && contents.back() == '\r') {
        lines.push_back({contents.size(), contents.size(), contents.size()});
    }
    return lines;
}

void MoveCaretLine(const std::int32_t direction)
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr || platform::gImeComposition[0] != '\0') {
        return;
    }
    gValue626C0C = 20;
    if (context->maxLine <= 1) {
        return;
    }
    EditText edit(*context);
    const auto lines = SplitLines(*context);
    std::size_t row = 0;
    while (row + 1 < lines.size() && edit.caret >= lines[row].nextStart) {
        ++row;
    }
    const auto destination = static_cast<std::ptrdiff_t>(row) + direction;
    if (destination < 0 || destination >= static_cast<std::ptrdiff_t>(lines.size())) {
        return;
    }
    const std::int32_t columnWidth = render::Font::measure(edit.contents.c_str() + lines[row].start, context->field13C,
        static_cast<std::int32_t>(edit.caret - lines[row].start));
    const TextLine& line = lines[static_cast<std::size_t>(destination)];
    edit.caret = line.start;
    while (edit.caret < line.end) {
        const std::size_t next = text::Next(edit.contents, edit.caret);
        if (render::Font::measure(edit.contents.c_str() + line.start, context->field13C, static_cast<std::int32_t>(next - line.start)) > columnWidth) {
            break;
        }
        edit.caret = next;
    }
    static_cast<void>(edit.commit());
}

void AppendText(const std::wstring_view utf16)
{
    const std::wstring source(utf16); // 源可与当前上下文重叠；先取得一次完整Unicode文字。
    for (std::size_t begin = 0; begin < source.size();) {
        const std::size_t end = text::Next(source, begin);
        const std::wstring_view unit(source.data() + begin, end - begin);
        begin = end;
        const auto first = static_cast<std::uint16_t>(unit[0]);
        if (first == 0 || (first <= 0x1F && first != '\r')) {
            continue;
        }
        InputContext* const context = gCurrentInputData;
        if (context == nullptr) {
            return;
        }
        EditText edit(*context);
        const std::wstring old = edit.contents;
        const std::size_t oldCaret = edit.caret;
        edit.contents.insert(edit.caret, unit);
        edit.caret += unit.size();
        try {
            if (!edit.commit()) {
                gValue626C28 = 1;
                return;
            }
        } catch (const std::system_error&) {
            platform::ShowWideMessage(L"这项文字无法保存为GBK，或超过字段容量。", L"文字输入", 0x10U);
            return;
        }
        const auto lines = SplitLines(*context);
        if (context->maxLine > 0 && lines.size() > static_cast<std::size_t>(context->maxLine)) {
            if (gValue626C20 == 0) {
                edit.contents.resize(lines[static_cast<std::size_t>(context->maxLine - 1)].end);
                edit.caret = (std::min)(edit.caret, edit.contents.size());
                static_cast<void>(edit.commit());
            } else if (context->caretByteOffset == context->totalByteLength - 1) {
                edit.contents = old;
                edit.caret = oldCaret;
                static_cast<void>(edit.commit());
                gValue626C28 = 1;
            }
        }
        gValue626C0C = 20;
    }
}

const char* LegacyText593850() noexcept
{
    // 原端文本读取可从前缀延续到紧邻的上下文。
    return reinterpret_cast<const char*>(&gLegacyInput593850);
}

InputContext* BorrowLegacyBlacklistContext593858() noexcept
{
    static InputContext view(gLegacyInput593850.context);
    return &view;
}

// WIC01：原滚动零/坐标模式颜色发布序与真实context返回，见doc/cg2d_window_input_context_dispatch_spec.md。
// 原函数：0x0045D720。
InputContext* Sub_45D720(
    InputContext* const context, const std::int32_t x, const std::int32_t y,
    [[maybe_unused]] const std::int32_t unused,
    const std::int32_t color, const std::int32_t renderMode)
{
    context->field154 = 0;
    context->field158 = 0;
    context->field134 = y;
    context->field130 = x;
    std::memcpy(context->field138, &renderMode, sizeof(renderMode));
    context->field140 = color;
    return context;
}

// WIC01：原坐标颜色模式字体发布序与真实context返回，见doc/cg2d_window_input_context_dispatch_spec.md。
// 原函数：0x0045D760。
InputContext* Sub_45D760(
    InputContext* const context, const std::int32_t x, const std::int32_t y,
    const std::int32_t color, const std::int32_t renderMode,
    const std::int32_t fontType)
{
    context->field130 = x;
    context->field134 = y;
    context->field140 = color;
    std::memcpy(context->field138, &renderMode, sizeof(renderMode));
    context->field13C = fontType;
    return context;
}

// 原 sub_44B190@[0x44B190,0x44B1C5)。
std::int32_t ClassifyLastAnsiUnit_44B190(const char* text)
{
    std::int32_t result = 0;
    const char* current = text;
    while (*current != '\0') {
        if (SignedByte(*current) < 0) {
            if (current[1] == '\0') {
                return 3;
            }
            current += 2;
            result = 2;
        } else {
            ++current;
            result = 1;
        }
    }
    return result;
}

// 原44B570固定扫描40字节；与通常的双字节分类规则不同。
std::int32_t ClassifyChatSplit_44B570(const char* const text, const std::int32_t)
{
    const std::string_view utf8(text);
    return cg::text::Prefix(utf8, 40) == (std::min)(std::size_t{40}, utf8.size()) ? 2 : 0;
}

// 原 [0x44B440,0x44B49D)：仅 0x81..0x9F 与 0xE0..0xFC 为双字节首字节。
// HBN01：原S16偏移/S32计数与固定BYTE范围，低BYTE 0/1/2类别，不改编码判据。
std::uint8_t ClassifyLegacyBoundary_44B440(
    const char* const text,
    const std::int16_t byteOffset)
{
    const std::string_view utf8(text);
    const std::size_t offset = static_cast<std::size_t>(byteOffset);
    if (cg::text::Prefix(utf8, offset) != offset) {
        return 2;
    }
    return offset < utf8.size() && static_cast<unsigned char>(utf8[offset]) >= 0x80 ? 1 : 0;
}

// 原 [0x44B4A0,0x44B561)：目标首字节经 movsx 后，双字节目标分支对 W8 输入不可达。
char* FindLegacyByte_44B4A0(char* const text, const char* const target)
{
    const std::int32_t wanted = SignedByte(target[0]);
    bool trailingByte = false;
    for (char* current = text; *current != '\0'; ++current) {
        if (trailingByte) {
            trailingByte = false;
            continue;
        }

        const auto byte = static_cast<std::uint8_t>(*current);
        if ((byte >= 0x81u && byte <= 0x9Fu)
            || (byte >= 0xE0u && byte <= 0xFCu)) {
            trailingByte = true;
            continue;
        }
        if (static_cast<std::int32_t>(byte) == wanted) {
            return current;
        }
    }
    return nullptr;
}

namespace {

// 原 sub_44B1D0@[0x44B1D0,0x44B244)。
std::int32_t ClassifyAnsiBoundary_44B1D0(
    const char* source,
    const std::int32_t count)
{
    const std::string_view utf8(source);
    const std::size_t end = text::Prefix(utf8, static_cast<std::size_t>(count));
    return static_cast<std::int32_t>(end - text::Previous(utf8, end));
}

} // namespace

// 原 sub_44A160。
void LoadInitialText_44A160(const char* const initialText)
{
    if (initialText != nullptr && gCurrentInputData != nullptr) { AppendText(text::ToUtf16(initialText)); }
}

namespace detail {

// 原 [0x44A5D0,0x44A7CD)：按 CR 和最大字节宽度扫描行后上移光标。
void Sub_44A5D0()
{
    MoveCaretLine(-1);
}

// 原 [0x44A540,0x44A580)：未移动时返回上下文指针，移动时返回边界分类。
std::uintptr_t Sub_44A540()
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr || platform::gImeComposition[0] != '\0') {
        return reinterpret_cast<std::uintptr_t>(context);
    }
    EditText edit(*context);
    if (edit.caret == 0) {
        return reinterpret_cast<std::uintptr_t>(context);
    }
    const std::size_t next = text::Previous(edit.contents, edit.caret);
    const std::size_t width = edit.caret - next;
    edit.caret = next;
    static_cast<void>(edit.commit());
    gValue626C0C = 20;
    return width;
}

// 原 [0x44A580,0x44A5C8)：按光标处字节的最高位移动一或两字节。
std::uintptr_t Sub_44A580()
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr || platform::gImeComposition[0] != '\0') {
        return reinterpret_cast<std::uintptr_t>(context);
    }
    EditText edit(*context);
    if (edit.caret >= edit.contents.size()) {
        return reinterpret_cast<std::uintptr_t>(context);
    }
    const std::size_t next = text::Next(edit.contents, edit.caret);
    const std::size_t width = next - edit.caret;
    edit.caret = next;
    static_cast<void>(edit.commit());
    gValue626C0C = 20;
    return width;
}

// 原 [0x44A7D0,0x44A989)：按 CR 和最大字节宽度扫描行后下移光标。
void Sub_44A7D0()
{
    MoveCaretLine(1);
}

// 原 [0x44A990,0x44AA8D)：删除光标前一个旧 ANSI 单元。
void Sub_44A990()
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr || context->caretByteOffset <= 0) {
        return;
    }
    EditText edit(*context);
    const std::size_t previous = text::Previous(edit.contents, edit.caret);
    edit.contents.erase(previous, edit.caret - previous);
    edit.caret = previous;
    static_cast<void>(edit.commit());
    gValue626C0C = 20;
}

// 原 [0x44AA90,0x44AB59)：删除光标处一个旧 ANSI 单元，返回当前上下文。
InputContext* Sub_44AA90()
{
    InputContext* const context = gCurrentInputData;
    if (context == nullptr || context->caretByteOffset >= context->totalByteLength) {
        return context;
    }
    EditText edit(*context);
    edit.contents.erase(edit.caret, text::Next(edit.contents, edit.caret) - edit.caret);
    static_cast<void>(edit.commit());
    gValue626C0C = 20;
    return context;
}

} // namespace detail

// 原函数：0x00404060。
bool IsRejectedInputText_404060(const std::wstring_view text)
{
    // 名称约束仍按原GBK字节域判断，UTF-16输入只在此边界编码。
    std::string stored;
    try { stored = cg::text::ToGbk(text); }
    catch (const std::system_error&) { return true; }
    const char* const legacyText = stored.c_str();
    // 原EDI/ESI为S32；控制分类在空白分类调用后独立重读源字节。
    const std::int32_t length = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(std::strlen(legacyText)));
    if (const_cast<char*>(std::strstr(legacyText, cg::text::ToGbk("　").c_str())) != nullptr) {
        return true;
    }
    if (length == 0) {
        return true;
    }

    for (std::int32_t index = 0; index < length; ++index) {
        const std::uint8_t firstByte = static_cast<std::uint8_t>(
            static_cast<unsigned char>(legacyText[index]));
        if ((firstByte & 0x80U) != 0U) {
            const std::uint8_t secondByte = static_cast<std::uint8_t>(
                static_cast<unsigned char>(legacyText[index + 1]));
            const std::uint16_t legacyUnit = static_cast<std::uint16_t>(
                (static_cast<std::uint16_t>(firstByte) << 8U)
                | static_cast<std::uint16_t>(secondByte));
            if ((legacyUnit >= 0xA140U && legacyUnit <= 0xA1FEU)
                || (legacyUnit >= 0xA240U && legacyUnit <= 0xA2FEU)
                || (legacyUnit >= 0xA340U && legacyUnit <= 0xA3E0U)) {
                return true;
            }
            ++index;
            continue;
        }

        if (firstByte == static_cast<std::uint8_t>('|')) {
            return true;
        }
        if (firstByte == static_cast<std::uint8_t>('\\')) {
            return true;
        }
        if (firstByte == static_cast<std::uint8_t>('/')) {
            return true;
        }
        if (std::isspace(firstByte) != 0) {
            return true;
        }
        if (std::iscntrl(SignedByte(legacyText[index])) != 0) {
            return true;
        }
    }

    return const_cast<char*>(std::strstr(legacyText, "[GP]")) != nullptr;
}

// 原函数：0x00476740。
std::int32_t Sub_476740(const std::wstring_view text)
{
    const std::wstring_view contents(text);
    bool hasText = false;
    for (std::size_t offset = 0; offset < contents.size();) {
        const std::size_t next = cg::text::Next(contents, offset);
        const auto unit = contents.substr(offset, next - offset);
        if (unit == L"," || unit == L"|" || unit == LR"(\)" ) { return 0; }
        if (unit != L" " && unit != L"　") { hasText = true; }
        offset = next;
    }
    return hasText ? 1 : 0;
}

// 原函数：0x0045E770。
std::int32_t Sub_45E770(const std::wstring_view text)
{
    const std::wstring_view contents(text);
    for (std::size_t offset = 0; offset < contents.size(); offset = cg::text::Next(contents, offset)) {
        if (contents[offset] == L'|' || contents[offset] == L'\\') { return 1; }
    }
    return 0;
}

// 原 [0x52D1B0,0x52D2C6)：原字节表按输入/转义字母交错存放。
// ETE01：原首BYTE持有/后继与索引更新序，转义字母早退前读取，两写后更新索引。
char* EscapeLegacyText_52D1B0(
    const char* const source,
    char* const destination,
    const std::int32_t capacity)
{
    // 仅供固定ASCII组包的兼容接口；UTF-16正文使用动态转义入口。
    const std::wstring escaped = EscapeText(cg::text::ToUtf16(source), capacity > 0 ? static_cast<std::size_t>(capacity) : 0);
    const std::string utf8 = cg::text::FromUtf16(escaped);
    if (capacity <= 0 || utf8.size() >= static_cast<std::size_t>(capacity)) {
        throw cg::text::GbkStorageError(122, std::system_category(), "转义结果超过临时字段容量");
    }
    std::memcpy(destination, utf8.c_str(), utf8.size() + 1);
    return destination;
}

// 原0x0052D1B0：界面UTF-16动态转义；旧协议容量由提交边界检查。
std::wstring EscapeText(const std::wstring_view source)
{
    std::wstring escaped;
    for (std::size_t offset = 0; offset < source.size();) {
        const std::size_t next = cg::text::Next(source, offset);
        std::int32_t pair = 4;
        if (source[offset] < 0x80) {
            pair = 0;
            while (pair < 4 && source[offset] != kEscapePairs58E944[pair * 2]) { ++pair; }
        }
        if (pair == 4) { escaped.append(source.substr(offset, next - offset)); }
        else { escaped += L'\\'; escaped += static_cast<wchar_t>(kEscapePairs58E944[pair * 2 + 1]); }
        offset = next;
    }
    return escaped;
}

// 原0x0052D1B0：容量只约束完整转义结果的GBK字节（含NUL）。
std::wstring EscapeText(const std::wstring_view source, const std::size_t gbkCapacity)
{
    std::wstring escaped = EscapeText(source);
    if (cg::text::ToGbk(escaped).size() >= gbkCapacity) {
        throw cg::text::GbkStorageError(122, std::system_category(), "转义结果超过GBK字段容量");
    }
    return escaped;
}

// 原0x0052D2D0：保留空叶调用，随后完整转义，失败不发布。
std::wstring EscapeTextAndNotify(const std::wstring_view source, const std::size_t gbkCapacity)
{
    NullSub_509080();
    return EscapeText(source, gbkCapacity);
}

// 原0x0052D1B0的GBK字节算法；仅旧字节域使用，界面转义入口为动态UTF-16。
char* EscapeGbkText(const char* const source, char* const destination, const std::int32_t capacity)
{
    const std::int32_t sourceLength =
        static_cast<std::int32_t>(std::strlen(source));
    std::int32_t sourceIndex = 0;
    std::int32_t destinationIndex = 0;
    std::int32_t used = 1;
    std::int32_t nextWidth = 2;
    std::int32_t adjustedLength = sourceLength;

    if (sourceLength > 0) {
        while (used < capacity) {
            const std::uint8_t current =
                static_cast<std::uint8_t>(source[sourceIndex]);
            if ((current & 0x80u) != 0u) {
                if (nextWidth >= capacity || used >= adjustedLength) {
                    break;
                }
                destination[destinationIndex] = static_cast<char>(current);
                ++destinationIndex;
                ++nextWidth;
                const char next = source[sourceIndex + 1];
                ++used;
                ++sourceIndex;
                destination[destinationIndex] = next;
                ++destinationIndex;
                ++nextWidth;
                ++used;
            } else {
                std::int32_t pair = 0;
                while (pair < 4
                    && current != kEscapePairs58E944[pair * 2]) {
                    ++pair;
                }
                if (pair == 4) {
                    destination[destinationIndex] = static_cast<char>(current);
                    ++destinationIndex;
                    ++nextWidth;
                    ++used;
                } else {
                    const char letter = static_cast<char>(
                        kEscapePairs58E944[pair * 2 + 1]);
                    if (nextWidth >= capacity) {
                        break;
                    }
                    destination[destinationIndex] = '\\';
                    destination[destinationIndex + 1] = letter;
                    destinationIndex += 2;
                    nextWidth += 2;
                    used += 2;
                    ++adjustedLength;
                }
            }
            if (++sourceIndex >= sourceLength) {
                break;
            }
        }
    }

    destination[destinationIndex] = '\0';
    return destination;
}


// 原 [0x52D2D0,0x52D2F0)：空叶不消费上下文，后继完整透传缓冲指针。
// ETE01：真实空操作未读source，先空操作再转义并透传destination，正确正文保持。
char* EscapeLegacyTextAndNotify_52D2D0(
    const char* const source,
    char* const destination,
    const std::int32_t capacity)
{
    NullSub_509080();
    return EscapeLegacyText_52D1B0(source, destination, capacity);
}

// ETE01：现有InputContext::text便利适配，同一52D2D0原根，不新增原身份。
std::wstring EscapeInputText_52D2D0(
    const InputContext* const context,
    const std::size_t gbkCapacity)
{
    return EscapeTextAndNotify(ContextText(*context), gbkCapacity);
}

namespace {

// 原 [0x52D120,0x52D144)：未命中表的转义字母原样返回。
// UTE01：原四对BYTE映射/S32计数，未知字母低BYTE原返。
char DecodeEscapeLetter_52D120(const char letter)
{
    for (std::int32_t pair = 0; pair < 4; ++pair) {
        if (static_cast<std::uint8_t>(letter)
            == kEscapePairs58E944[pair * 2 + 1]) {
            return static_cast<char>(kEscapePairs58E944[pair * 2]);
        }
    }
    return letter;
}

} // namespace

// 原 [0x52D150,0x52D1A6)：以入口长度为界，原位压缩旧字节转义。
// UTE01：原首BYTE持有/后继先读后递增/共尾写，保持固定字节长度与原末NUL。
char* UnescapeLegacyText_52D150(char* const text)
{
    const std::int32_t length =
        static_cast<std::int32_t>(std::strlen(text));
    std::int32_t destinationIndex = 0;
    std::int32_t sourceIndex = 0;
    while (sourceIndex < length) {
        const char current = text[sourceIndex];
        char decoded = current;
        if ((static_cast<std::uint8_t>(current) & 0x80u) != 0u) {
            text[destinationIndex] = current;
            decoded = text[sourceIndex + 1];
            ++destinationIndex;
            ++sourceIndex;
        } else if (current == '\\') {
            const char letter = text[sourceIndex + 1];
            ++sourceIndex;
            decoded = DecodeEscapeLetter_52D120(letter);
        }
        text[destinationIndex] = decoded;
        ++destinationIndex;
        ++sourceIndex;
    }
    text[destinationIndex] = '\0';
    return text;
}

// 原 [0x52D2F0,0x52D308)：空叶调用不改变转义后的文本或原指针。
// UTE01：原反转义→真实空操作→同text返回；空操作未读原text参数，不增加通知。
char* UnescapeLegacyTextAndNotify_52D2F0(char* const text)
{
    UnescapeLegacyText_52D150(text);
    NullSub_509080();
    return text;
}

// 原[503AB0,503B6D)：旧字节分行，参数失败不写目标，正常尾始终写NUL。
std::int32_t ExtractLegacyLine_503AB0(
    char* destination, const std::int32_t capacity, const char* source,
    const std::int32_t requestedLine, const std::int32_t width)
{
    if (requestedLine < 0 || width <= 0 || capacity <= width) {
        return 0;
    }
    std::int32_t line = 0;
    std::int32_t column = 0;
    for (;;) {
        // 原503ADD/503B55每轮只取一次首U8，分流和写出均复用该值。
        const volatile char* const sampledSource = source;
        const char value = *sampledSource;
        if (value == '\0') {
            break;
        }
        const auto byte = static_cast<std::uint8_t>(value);
        if ((byte & 0x80u) != 0) {
            column += 2;
            if (column > width) {
                ++line;
                column = 0;
                continue;
            }
            if (line == requestedLine) {
                destination[0] = value;
                // 原503AFD先写首字节，再503AFF取尾；保持源/目标可能重叠的原序。
                destination[1] = sampledSource[1];
                destination += 2;
            }
            source += 2;
        } else if (value == '\r' || value == '\n' || value == 1) {
            if (value == 1) {
                if (line >= requestedLine) {
                    break;
                }
                line = 8 * (line / 8) + 8;
            } else {
                if (line == requestedLine) {
                    break;
                }
                ++line;
            }
            column = 0;
            ++source;
            continue;
        } else {
            if (line == requestedLine) {
                *destination++ = value;
            }
            ++source;
            ++column;
        }
        if (column >= width) {
            if (line == requestedLine) {
                break;
            }
            ++line;
            column = 0;
        }
    }
    *destination = '\0';
    return 1;
}

// 原 [0x52D570,0x52D5BB)：每个源字节依序与删除集合比较，再原位压缩。
char RemoveLegacyBytes_52D570(
    char* const text,
    const char* const excluded)
{
    const std::string contents(text);
    const std::string_view skipped(excluded);
    std::string retained;
    for (std::size_t offset = 0; offset < contents.size();) {
        const std::size_t next = cg::text::Next(contents, offset);
        const auto unit = std::string_view(contents).substr(offset, next - offset);
        bool remove = false;
        for (std::size_t item = 0; item < skipped.size(); item = cg::text::Next(skipped, item)) {
            if (unit == skipped.substr(item, cg::text::Next(skipped, item) - item)) { remove = true; break; }
        }
        if (!remove) { retained.append(unit); }
        offset = next;
    }
    std::memcpy(text, retained.c_str(), retained.size() + 1);
    return 0;
}

// 原函数：0x0044ADB0。
void AppendText(const std::string_view utf8) { AppendText(text::ToUtf16(utf8)); }
void LoadInitialText_44A160(const std::wstring_view initialText)
{
    if (gCurrentInputData != nullptr) { AppendText(initialText); }
}

void AppendChar_44ADB0(const char character)
{
    AppendText(std::string_view(&character, 1));
}

// 原函数：0x0044B340。
std::int32_t ProcessPresetHotkeys_44B340()
{
    std::int32_t base = 0;
    if ((platform::gKeyStates[0x10] & 2u) != 0) {
        base = 16;
    } else if ((platform::gKeyStates[0x11] & 2u) != 0) {
        base = 8;
    }
    std::int32_t selected = -1;
    std::int32_t result = 0;
    for (std::int32_t index = 0; index < 8; ++index) {
        result = platform::ReadKeyPressFlag_4597B0(0x70 + index);
        if (result != 0) {
            selected = base + index;
        }
    }
    if (selected >= 0) {
        LoadInitialText_44A160(ContextText(gInputContexts5F9758[selected]).c_str());
        return audio::PlaySe_52C450(51, 320);
    }
    return result;
}

// 原函数：0x0044B330。
void SetContext_44B330(InputContext* const context)
{
    gCurrentInputData = context;
}

// 原函数：0x00503E40。
std::int32_t IsCurrentContext_503E40(const InputContext* const context)
{
    return gCurrentInputData == context ? 1 : 0;
}


// UTL01：按Unicode完整单元保留旧ASCII一列／非ASCII两列预算；容量仍是输出字节。
// GBK显示边界：原0x00503AB0逻辑列语义，输出完整UTF-8字符。
std::int32_t ExtractGbkTextLine(
    char* destination, const std::int32_t capacity, const char* source,
    const std::int32_t requestedLine, const std::int32_t width)
{
    const std::string utf8 = text::FromGbk(source);
    return ExtractTextLine(destination, capacity, utf8.c_str(), requestedLine, width);
}

std::int32_t ExtractTextLine(
    char* destination, const std::int32_t capacity, const char* source,
    const std::int32_t requestedLine, const std::int32_t width)
{
    if (requestedLine < 0 || width <= 0 || capacity <= 0) {
        return 0;
    }
    const std::string contents(source);
    std::string selected;
    std::int32_t line = 0;
    std::int32_t column = 0;
    for (std::size_t offset = 0; offset < contents.size();) {
        const char current = contents[offset];
        if (current == '\r' || current == '\n' || current == 1) {
            if (line == requestedLine) {
                break;
            }
            line = current == 1 ? 8 * (line / 8) + 8 : line + 1;
            column = 0;
            ++offset;
            continue;
        }
        const std::size_t next = text::Next(contents, offset);
        const std::int32_t columns = next - offset == 1 ? 1 : 2;
        if (column != 0 && column + columns > width) {
            if (line == requestedLine) {
                break;
            }
            ++line;
            column = 0;
            continue;
        }
        if (line == requestedLine) {
            selected.append(contents, offset, next - offset);
        }
        offset = next;
        column += columns;
        if (column >= width) {
            if (line == requestedLine) {
                break;
            }
            ++line;
            column = 0;
        }
    }
    text::Copy(destination, static_cast<std::size_t>(capacity), selected);
    return 1;
}

// 仅旧绘制包装的局部UTF-8输出视图，权威分行正文仍为UTF-16。
std::int32_t ExtractTextLine(
    char* destination, const std::int32_t capacity, const std::wstring_view source,
    const std::int32_t requestedLine, const std::int32_t width)
{
    std::wstring selected;
    const std::int32_t result = ExtractTextLine(selected, source, requestedLine, width);
    if (result != 0 && capacity > 0) { text::Copy(destination, static_cast<std::size_t>(capacity), text::FromUtf16(selected)); }
    return result;
}



// 原0x00503AB0显示分行：动态UTF-16正文，完整代理对及原逻辑列预算。
std::int32_t ExtractTextLine(
    std::wstring& destination, const std::wstring_view source,
    const std::int32_t requestedLine, const std::int32_t width)
{
    if (requestedLine < 0 || width <= 0) {
        return 0;
    }
    const std::wstring_view contents(source);
    std::wstring selected;
    std::int32_t line = 0;
    std::int32_t column = 0;
    for (std::size_t offset = 0; offset < contents.size();) {
        const wchar_t current = contents[offset];
        if (current == L'\r' || current == L'\n' || current == 1) {
            if (line == requestedLine) {
                break;
            }
            line = current == 1 ? 8 * (line / 8) + 8 : line + 1;
            column = 0;
            ++offset;
            continue;
        }
        const std::size_t next = text::Next(contents, offset);
        const std::int32_t columns = current < 0x80 ? 1 : 2;
        if (column != 0 && column + columns > width) {
            if (line == requestedLine) {
                break;
            }
            ++line;
            column = 0;
            continue;
        }
        if (line == requestedLine) {
            selected.append(contents.substr(offset, next - offset));
        }
        offset = next;
        column += columns;
        if (column >= width) {
            if (line == requestedLine) {
                break;
            }
            ++line;
            column = 0;
        }
    }
    destination = std::move(selected);
    return 1;
}

std::int32_t ExtractGbkTextLine(
    std::wstring& destination, const char* source,
    const std::int32_t requestedLine, const std::int32_t width)
{
    return ExtractTextLine(destination, text::FromGbkWide(source), requestedLine, width);
}


// 原函数：0x00503E60。
std::int32_t IsChatContext_503E60()
{
    return gCurrentInputData == &gInputContext626AA0 ? 1 : 0;
}

InputContext* BorrowLegacyPresetContext5F9758(const std::int32_t index) noexcept
{
    if (index < 24) { return &gInputContexts5F9758[index]; }
    // 原15CH步长只用于GBK存储；适配对象只借原字段，正文每次操作从原owner读取。
    static_assert(54 * sizeof(LegacyInputStorage) <= sizeof(world::detail::gRecords5FB7F8));
    static std::array<std::optional<InputContext>, 54> views;
    auto& view = views[static_cast<std::size_t>(index - 24)];
    if (!view.has_value()) {
        auto* const bytes = reinterpret_cast<std::byte*>(world::detail::gRecords5FB7F8);
        auto* const storage = reinterpret_cast<LegacyInputStorage*>(bytes + static_cast<std::ptrdiff_t>(index - 24) * sizeof(LegacyInputStorage));
        view.emplace(*storage);
    }
    return &*view;
}

// 原函数：0x00449CF0。
void InitializeContext_449CF0(
    InputContext* const context,
    const std::int32_t field130,
    const std::int32_t field134,
    const std::int32_t renderMode,
    const std::int32_t fontType,
    const std::int32_t color,
    const char* const initialText,
    const std::int32_t maxLine,
    const std::int32_t maxCharPerLine,
    const std::int32_t field14C,
    const std::int32_t encrypt)
{
    context->field130 = field130;
    std::memcpy(context->field138, &renderMode, sizeof(renderMode));
    context->field140 = color;
    context->field134 = field134;
    context->maxCharPerLine = maxCharPerLine;
    context->field13C = fontType;
    context->encrypt = encrypt;
    context->maxLine = maxLine;
    context->field14C = field14C;
    context->clearText();
    context->totalByteLength = 0;
    context->caretByteOffset = 0;
    context->field154 = 0;
    context->field158 = 0;

    if (initialText != nullptr) {
        InputContext* const savedContext = gCurrentInputData;
        SetContext_44B330(context);
        LoadInitialText_44A160(initialText);
        SetContext_44B330(savedContext);
    }
}

// 原0x00449CF0：UTF-16输入原型直接初始化，窄入口仅为局部兼容适配。
void InitializeContext_449CF0(
    InputContext* const context,
    const std::int32_t field130,
    const std::int32_t field134,
    const std::int32_t renderMode,
    const std::int32_t fontType,
    const std::int32_t color,
    const std::wstring_view initialText,
    const std::int32_t maxLine,
    const std::int32_t maxCharPerLine,
    const std::int32_t field14C,
    const std::int32_t encrypt)
{
    context->field130 = field130;
    std::memcpy(context->field138, &renderMode, sizeof(renderMode));
    context->field140 = color;
    context->field134 = field134;
    context->maxCharPerLine = maxCharPerLine;
    context->field13C = fontType;
    context->encrypt = encrypt;
    context->maxLine = maxLine;
    context->field14C = field14C;
    context->clearText();
    context->totalByteLength = 0;
    context->caretByteOffset = 0;
    context->field154 = 0;
    context->field158 = 0;

    if (!initialText.empty()) {
        InputContext* const savedContext = gCurrentInputData;
        SetContext_44B330(context);
        LoadInitialText_44A160(initialText);
        SetContext_44B330(savedContext);
    }
}

} // namespace cg::input

namespace cg::world::detail {

Record5FB7F8 gRecords5FB7F8[512] {}; // 原全局：0x005FB7F8。
volatile std::int32_t gWriteIndex626C08 = 0; // 原全局：0x00626C08。
char gLogPath62699C[260] {}; // 原全局：0x0062699C。
platform::LegacyFile* volatile gLogStream626C1C = nullptr; // 原全局：0x00626C1C。
volatile std::int32_t gCountC9A944 = 0; // 原全局：0x00C9A944。
volatile std::uint32_t gValue626C10 = 0; // 原全局：0x00626C10。
input::InputContext gInputContext626840 {}; // 原全局：0x00626840。
std::uint32_t gValue626C18 = 0; // 原全局：0x00626C18。
std::uint32_t gValue626BFC = 0; // 原全局：0x00626BFC。
char gLogFileName554648[16] = "chat_000000.txt"; // 原全局：0x00554648。

// 原 [0x621FF8,0x62683C)：64 项记录与尾槽一次传输，游标不在文件块内。
struct ChatHistory621FF8 {
    char records[64][0x121];
    std::uint32_t tail626838; // 原全局：0x00626838。
};

ChatHistory621FF8 gChatHistory621FF8 {}; // 原全局：0x00621FF8。
std::int32_t gCursor62683C = 0; // 原全局：0x0062683C。

static_assert(sizeof(gRecords5FB7F8) == 512 * 0x134);
static_assert(std::is_standard_layout_v<ChatHistory621FF8>);
static_assert(std::is_trivially_copyable_v<ChatHistory621FF8>);
static_assert(sizeof(gChatHistory621FF8.records[0]) == 0x121);
static_assert(sizeof(gChatHistory621FF8.records) == 0x4840);
static_assert(offsetof(ChatHistory621FF8, tail626838) == 0x4840);
static_assert(sizeof(ChatHistory621FF8) == 0x4844);

namespace {

constexpr char kLogDirectory_554658[] = "Log"; // 原常量参考：0x00554658。
constexpr char kFileFormat_55351C[] = "chat_%02d%02d%02d.txt"; // 原常量参考：0x0055351C。
constexpr char kPathFormat_553514[] = "%s\\%s"; // 原常量参考：0x00553514。
constexpr char kReadMode_554908[] = "r"; // 原常量参考：0x00554908。
constexpr char kAppendMode_554904[] = "a"; // 原常量参考：0x00554904。
constexpr char kChatHistoryName554910[] = "chathis.dat"; // 原常量参考：0x00554910。
constexpr char kChatHistoryReadMode[] = "rb";
constexpr char kChatHistoryWriteMode[] = "wb";
constexpr char kChatHistoryUpdateMode[] = "r+b";

constexpr auto kFirstSeparator_5548CC = [] { // 原常量参考：0x005548CC。
    std::array<char, 56> text {};
    for (std::size_t i = 0; i < 8; ++i) {
        text[i] = ' ';
    }
    for (std::size_t i = 8; i < 31; ++i) {
        text[i] = '>';
    }
    for (std::size_t i = 31; i < 54; ++i) {
        text[i] = '<';
    }
    text[54] = '\n';
    return text;
}();

constexpr char kDateTitle_55488C[] = // 原常量参考：0x0055488C。
    R"(        >>  魔 力 寶 貝 冒 險 者 日 誌 [ %02d/%02d/%02d ]  <<
)";

constexpr auto kBeforeSession_554848 = [] { // 原常量参考：0x00554848。
    std::array<char, 65> text {};
    text[0] = '\n';
    text[1] = ' ';
    text[2] = ' ';
    for (std::size_t i = 3; i < 63; ++i) {
        text[i] = '-';
    }
    text[63] = '\n';
    return text;
}();

constexpr char kSessionTime_554820[] = // 原常量参考：0x00554820。
    R"( ■ %02d/%02d/%02d %02d:%02d:%02d 紀錄
)";

constexpr auto kAfterSession_5547DC = [] { // 原常量参考：0x005547DC。
    std::array<char, 65> text {};
    text[0] = ' ';
    text[1] = ' ';
    for (std::size_t i = 2; i < 63; ++i) {
        text[i] = '-';
    }
    text[63] = '\n';
    return text;
}();


} // namespace

// 原 [0x44A090,0x44A15B)：保留短读已写入的字节及 rb 失败后创建文件的顺序。
std::int32_t Sub_44A090()
{
    gCursor62683C = -1;
    char path[128];
    std::sprintf(
        path, kPathFormat_553514, platform::gDataPath, kChatHistoryName554910);

    platform::LegacyFile* const readStream = std::fopen(path, kChatHistoryReadMode);
    if (readStream != nullptr) {
        const std::uint32_t count =
            static_cast<std::uint32_t>(std::fread(&gChatHistory621FF8, 0x4844, 1, readStream));
        std::fclose(readStream);
        return count >= 1 ? 1 : 0;
    }

    platform::LegacyFile* const writeStream = std::fopen(path, kChatHistoryWriteMode);
    if (writeStream != nullptr) {
        static_cast<std::uint32_t>(std::fwrite(&gChatHistory621FF8, 0x4844, 1, writeStream));
        std::fclose(writeStream);
    }
    gCursor62683C = 63;
    return 0;
}

// 原 [0x449FA0,0x44A084)：按索引原位写入聊天历史记录及尾部 W32。
std::int32_t Sub_449FA0(const std::int32_t index)
{
    char path[128];
    std::sprintf(
        path, kPathFormat_553514, platform::gDataPath, kChatHistoryName554910);

    platform::LegacyFile* const stream = std::fopen(path, kChatHistoryUpdateMode);
    if (stream == nullptr) {
        return 0;
    }

    std::fseek(stream, static_cast<std::int32_t>(index * 0x121), 0);
    if (static_cast<std::uint32_t>(std::fwrite(gChatHistory621FF8.records[index], 0x121, 1, stream)) < 1) {
        std::fclose(stream);
        return 0;
    }

    std::fseek(stream, 0x4840, 0);
    const std::uint32_t written = static_cast<std::uint32_t>(std::fwrite(&index, static_cast<std::uint32_t>(sizeof(index)), 1, stream));
    std::fclose(stream);
    return written >= 1 ? 1 : 0;
}

// 原 sub_449DA0@[0x449DA0,0x449F9C)：输入记录与聊天日志初始化。
void __cdecl Sub_449DA0()
{
    std::memset(gRecords5FB7F8, 0, sizeof(gRecords5FB7F8));
    input::gInputContext626AA0.reset();
    input::InitializeContext_449CF0(
        &input::gInputContext626AA0, 8, 440, 0, 1, 0, nullptr, 1, 78, 0, 0);
    gInputContext626840 = input::gInputContext626AA0;

    for (std::size_t index = 0; index < 19; ++index) {
        gRecords5FB7F8[index].state = 1;
    }
    gValue626C18 = 0;
    gValue626BFC = 0;

    std::int32_t timeValue = 0;
    platform::LegacyFile* const existingStream = gLogStream626C1C;
    if (existingStream != nullptr) {
        return;
    }

    ::_mkdir(kLogDirectory_554658);
    platform::ReadLegacyTime32_53BA26(&timeValue);
    const platform::LegacyTm53B8C6* const legacyTime =
        platform::LocalTime_53B8C6(&timeValue);
    const auto* const timeBytes =
        reinterpret_cast<const unsigned char*>(legacyTime);
    const auto readTimeField = [timeBytes](const std::size_t offset) {
        std::int32_t value;
        std::memcpy(&value, timeBytes + offset, sizeof(value));
        return value;
    };

    const std::int32_t fileMonth = readTimeField(0x10);
    const std::int32_t fileDay = readTimeField(0x0C);
    const std::int32_t fileYear = readTimeField(0x14);
    std::sprintf(
        gLogFileName554648,
        kFileFormat_55351C,
        fileYear % 100, fileMonth + 1, fileDay);
    std::sprintf(
        gLogPath62699C,
        kPathFormat_553514,
        kLogDirectory_554658,
        gLogFileName554648);

    platform::LegacyFile* const readStream =
        std::fopen(gLogPath62699C, kReadMode_554908);
    gLogStream626C1C = readStream;
    platform::LegacyFile* gate;
    if (readStream == nullptr) {
        platform::LegacyFile* const appendStream =
            std::fopen(gLogPath62699C, kAppendMode_554904);
        gLogStream626C1C = appendStream;
        std::fprintf(appendStream, kFirstSeparator_5548CC.data());

        const std::int32_t titleMonth = readTimeField(0x10);
        const std::int32_t titleDay = readTimeField(0x0C);
        const std::int32_t titleYear = readTimeField(0x14);
        std::fprintf(
            gLogStream626C1C,
            cg::text::ToGbk(kDateTitle_55488C).c_str(),
            titleYear % 100, titleMonth + 1, titleDay);
        std::fprintf(gLogStream626C1C, kFirstSeparator_5548CC.data());
        gate = gLogStream626C1C;
    } else {
        std::fclose(readStream);
        platform::LegacyFile* const appendStream =
            std::fopen(gLogPath62699C, kAppendMode_554904);
        gLogStream626C1C = appendStream;
        gate = appendStream;
    }

    if (gate != nullptr) {
        std::fprintf(gate, kBeforeSession_554848.data());

        const std::int32_t sessionSecond = readTimeField(0x00);
        const std::int32_t sessionMinute = readTimeField(0x04);
        const std::int32_t sessionHour = readTimeField(0x08);
        const std::int32_t sessionDay = readTimeField(0x0C);
        const std::int32_t sessionMonth = readTimeField(0x10);
        const std::int32_t sessionYear = readTimeField(0x14);
        std::fprintf(
            gLogStream626C1C,
            cg::text::ToGbk(kSessionTime_554820).c_str(),
            sessionYear % 100, sessionMonth + 1, sessionDay,
            sessionHour, sessionMinute, sessionSecond);
        std::fprintf(gLogStream626C1C, kAfterSession_5547DC.data());
    }
    std::fflush(gLogStream626C1C);
}

// 原 sub_4A76E0@[0x4A76E0,0x4A76FD)：先写未封顶值，超过 512 再写封顶值。
std::int32_t Sub_4A76E0()
{
    const std::int32_t increased = gCountC9A944 + 1;
    gCountC9A944 = increased;
    if (increased > 512) {
        gCountC9A944 = 512;
    }
    return increased;
}

// 原 sub_44B050@[0x44B050,0x44B170)。
void Sub_44B050(
    const std::wstring_view message,
    const std::int32_t color,
    const std::int32_t fontType)
{
    if (color >= 13) {
        return;
    }
    if (fontType >= 14) {
        return;
    }

    std::string stored;
    try {
        stored = cg::text::ToGbk(message);
        if (stored.size() >= sizeof(gRecords5FB7F8[0].text)) {
            throw cg::text::GbkStorageError(122, std::system_category(), "文字超过聊天记录GBK容量");
        }
    }
    catch (const std::system_error&) {
        platform::ShowWideMessage(L"这项文字无法保存为GBK，或超过字段容量。", L"文字编码", 0x10U);
        return;
    }
    const char* const text = stored.c_str();
    std::int32_t index = gWriteIndex626C08;
    char* const destination = gRecords5FB7F8[index].text;
    const std::size_t length = std::strlen(text) + 1;
    const std::size_t wordCount = length / sizeof(std::uint32_t);
    for (std::size_t word = 0; word < wordCount; ++word) {
        const std::size_t offset = word * sizeof(std::uint32_t);
        std::uint32_t copied;
        std::memcpy(&copied, text + offset, sizeof(copied));
        std::memcpy(destination + offset, &copied, sizeof(copied));
    }

    const platform::LegacyFile* const gate = gLogStream626C1C;
    for (std::size_t offset = wordCount * sizeof(std::uint32_t);
         offset < length; ++offset) {
        destination[offset] = text[offset];
    }

    if (gate != nullptr) {
        std::int32_t timeValue = color;
        platform::ReadLegacyTime32_53BA26(&timeValue);
        const platform::LegacyTm53B8C6* const legacyTime =
            platform::LocalTime_53B8C6(&timeValue);
        index = gWriteIndex626C08;
        const auto* const timeBytes =
            reinterpret_cast<const unsigned char*>(legacyTime);
        std::int32_t second;
        std::int32_t minute;
        std::int32_t hour;
        std::memcpy(&second, timeBytes, sizeof(second));
        std::memcpy(&minute, timeBytes + 4, sizeof(minute));
        std::memcpy(&hour, timeBytes + 8, sizeof(hour));

        std::fprintf(
            gLogStream626C1C,
            " %02d:%02d:%02d\x81\x40%s\n",
            hour, minute, second, gRecords5FB7F8[index].text);
        std::fclose(gLogStream626C1C);
        platform::LegacyFile* const newStream = std::fopen(gLogPath62699C, "a");
        index = gWriteIndex626C08;
        gLogStream626C1C = newStream;
    }

    Record5FB7F8& record = gRecords5FB7F8[index];
    record.color = color;
    record.fontType = fontType;
    record.state = 0;
    const std::int32_t nextIndex = index + 1;
    gWriteIndex626C08 = nextIndex;
    if (nextIndex >= 512) {
        gWriteIndex626C08 = 0;
    }
    Sub_4A76E0();
    gValue626C10 = 20;
}

} // namespace cg::world::detail

namespace cg::input {

// HBN01：原历史选择/独立游标与现场context清写、字节滚动尾，正确正文保持。
// 原函数：0x0044A1B0。
void BrowseChatHistory_44A1B0()
{
    using world::detail::gChatHistory621FF8;
    using world::detail::gCursor62683C;
    NullSub_509080();
    InputContext* const initialContext = gCurrentInputData;
    if (initialContext != &gInputContext626AA0 || platform::gImeComposition[0] != '\0') {
        return;
    }
    const std::uint8_t control = platform::gKeyStates[17] & 2u;
    if (control == 0 && (platform::gKeyStates[16] & 2u) == 0
        && (platform::gKeyStates[38] & 0x10u) != 0) {
        const std::int32_t previous = gCursor62683C;
        const auto tail = std::bit_cast<std::int32_t>(gChatHistory621FF8.tail626838);
        std::int32_t selected = previous == -1 ? tail : previous - 1;
        gCursor62683C = selected;
        if (selected < 0) {
            selected = 63;
            gCursor62683C = selected;
        }
        if (gChatHistory621FF8.records[selected][0] == '\0'
            || (selected == tail && previous != -1)) {
            gCursor62683C = previous;
            return;
        }
    } else {
        // 向下复用已读Ctrl位，不增加Shift门禁或空记录检查。
        if (control != 0 || (platform::gKeyStates[40] & 0x10u) == 0) {
            return;
        }
        const std::int32_t selected = gCursor62683C;
        if (selected == -1) {
            return;
        }
        if (selected == std::bit_cast<std::int32_t>(gChatHistory621FF8.tail626838)) {
            gCursor62683C = -1;
            initialContext->totalByteLength = 0;
            gCurrentInputData->clearText();
            gCurrentInputData->caretByteOffset = 0;
            return;
        }
        const std::int32_t next = selected + 1;
        gCursor62683C = next;
        if (next >= 64) {
            gCursor62683C = 0;
        }
    }
    initialContext->totalByteLength = 0;
    gCurrentInputData->clearText();
    gCurrentInputData->caretByteOffset = 0;
    LoadInitialText_44A160(cg::text::FromGbk(gChatHistory621FF8.records[gCursor62683C]).c_str());
    const std::wstring contents = ContextText(*gCurrentInputData);
    const std::size_t width = static_cast<std::size_t>(ui::ReadChatInputByteWidth_4A80E0());
    std::size_t offset = contents.size() > width ? contents.size() - width : 0;
    offset = text::Prefix(contents, offset);
    SetVisibleUtf16Offset(*gCurrentInputData, offset);
}

} // namespace cg::input

namespace cg::input::detail {

// 原函数：0x0044AB60。
void Sub_44AB60()
{
    if (platform::gImeComposition[0] != '\0') {
        return;
    }
    InputContext* context = gCurrentInputData;
    if (context == &gInputContext626AA0) {
        if (context->totalByteLength == 0) {
            return;
        }
        std::string stored;
        try {
            stored = cg::text::ToGbk(ContextText(*context));
            if (stored.size() >= sizeof(world::detail::gChatHistory621FF8.records[0])) {
                throw cg::text::GbkStorageError(122, std::system_category(), "文字超过聊天历史GBK容量");
            }
            // 先验证P|之后的转义正文，再发布历史；此预检不调用原通知空叶。
            static_cast<void>(EscapeText(ContextText(*context), 1022));
        }
        catch (const std::system_error&) {
            platform::ShowWideMessage(L"这项文字无法保存为GBK，或超过字段容量。", L"文字编码", 0x10U);
            return;
        }
        auto& history = world::detail::gChatHistory621FF8;
        const std::uint32_t oldTail = history.tail626838;
        const std::int32_t oldIndex = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(oldTail));
        std::int32_t nextIndex = std::bit_cast<std::int32_t>(oldTail) + 1;
        history.tail626838 = static_cast<std::uint32_t>(nextIndex);
        if (nextIndex >= 64) {
            nextIndex = 0;
            history.tail626838 = 0;
        }
        const char* const previousText = history.records[oldIndex];
        bool sameAsPrevious = false;
        std::size_t comparisonIndex = 0;
        for (;;) {
            const auto inputByte =
                static_cast<unsigned char>(stored[comparisonIndex]);
            const auto previousByte =
                static_cast<unsigned char>(previousText[comparisonIndex]);
            if (inputByte != previousByte) {
                break;
            }
            if (inputByte == 0) {
                sameAsPrevious = true;
                break;
            }
            ++comparisonIndex;
        }
        if (sameAsPrevious) {
            history.tail626838 = static_cast<std::uint32_t>(oldIndex);
        } else {
            std::strcpy(history.records[nextIndex], stored.c_str());
            static_cast<void>(world::detail::Sub_449FA0(
                std::bit_cast<std::int32_t>(history.tail626838)));
            context = gCurrentInputData;
        }
        const std::int32_t color = gInputContext626AA0.field140;
        world::detail::gCursor62683C = -1;
        static_cast<void>(world::detail::Sub_50C940(context, color));
        gCurrentInputData->totalByteLength = 0;
        gCurrentInputData->caretByteOffset = 0;
        gCurrentInputData->clearText();
        context = gCurrentInputData;
    } else if (context == &gInputContextD66CA8
               || context == &gInputContextCB03F0
               || context == &gInputContextCC46E0
               || context == &gInputContextCC2080
               || context == &gInputContextD2D828
               || context == &gInputContextCA0180) {
        AppendChar_44ADB0('\r');
        context = gCurrentInputData;
    }
    if (context == &gInputContextC8B1B8) {
        std::wstring message;
        const std::wstring text = ContextText(gInputContextC8B1B8);
        const std::wstring playerName = cg::text::FromGbkWide(world::detail::BorrowPlayerDisplayText(1));
        const bool targetsSelf = text == playerName;
        if (targetsSelf) {
            message = kBlacklistSelfText554938;
        } else if (blacklist::ContainsBlacklistRecord_4013D0(text) != 0) {
            message = std::format(kBlacklistExistingFormat554928, text);
        } else {
            message = blacklist::AddBlacklistRecord_401270(text) != 0
                ? std::format(kBlacklistAddedFormat5532C4, text)
                : std::format(kBlacklistFailedFormat5532B0, text);
        }
        gInputContextC8B1B8.clearText();
        gInputContextC8B1B8.caretByteOffset = 0;
        gInputContextC8B1B8.totalByteLength = 0;
        world::detail::Sub_44B050(message, 4, 3);
        SetContext_44B330(&gInputContext626AA0);
    }
    gValue626C0C = 20;
}

} // namespace cg::input::detail
