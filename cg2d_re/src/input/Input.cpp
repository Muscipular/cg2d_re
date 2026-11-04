#include "cg/core/LegacyNoOp.hpp"
#include "cg/session/ClientSession.hpp"
#include "../world/ChatHistory.hpp"
#include "../ui/WindowRuntimeState.hpp"
#include "cg/input/Input.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/render/Font.hpp"
#include "cg/world/Blacklist.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/ui/WindowRuntime.hpp"

#include "InputKeyDispatchDependencies.hpp"
#include "../platform/WindowProcedureState.hpp"
#include "../world/PlayerState.hpp"

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

#include "cg/core/m_log.hpp"

namespace cg::input {

InputContext::InputContext() noexcept
    : x(ownFields_.x),
      y(ownFields_.y),
      caretByteOffset(ownFields_.caretByteOffset),
      totalByteLength(ownFields_.totalByteLength),
      textX(ownFields_.textX),
      textY(ownFields_.textY),
      renderMode(ownFields_.renderMode),
      fontType(ownFields_.fontType),
      color(ownFields_.color),
      maxLine(ownFields_.maxLine),
      maxCharPerLine(ownFields_.maxCharPerLine),
      lineHeight(ownFields_.lineHeight),
      encrypt(ownFields_.encrypt),
      visibleWidth(ownFields_.visibleWidth),
      visibleStartOffset(ownFields_.visibleStartOffset) {}
InputContext::InputContext(const InputContext& other) : InputContext() { *this = other; }
InputContext& InputContext::operator=(const InputContext& other)
{
    if (this == &other) { return *this; }
    const std::wstring contents = ContextText(other);
    setText(contents);
    x = other.x;
    y = other.y;
    const std::size_t caret = ContextUtf16Offset(other, other.caretByteOffset);
    caretByteOffset = static_cast<std::int32_t>(caret);
    totalByteLength = static_cast<std::int32_t>(contents.size());
    textX = other.textX;
    textY = other.textY;
    fontType = other.fontType;
    color = other.color;
    maxLine = other.maxLine;
    maxCharPerLine = other.maxCharPerLine;
    lineHeight = other.lineHeight;
    encrypt = other.encrypt;
    visibleWidth = other.visibleWidth;
    const std::size_t visible = ContextUtf16Offset(other, other.visibleStartOffset);
    visibleStartOffset = static_cast<std::int32_t>(visible);
    renderMode = other.renderMode;
    return *this;
}
bool InputContext::empty() const noexcept
{
    return text.empty();
}
std::size_t InputContext::length() const { return text.size(); }
void InputContext::clearText() noexcept
{
    text.clear();
}
void InputContext::reset() noexcept
{
    clearText();
    ownFields_ = {};
}
void InputContext::setText(const std::wstring_view value)
{
    text.assign(value);
}

static constexpr std::array<std::uint8_t, 8> kEscapePairs58E944 {{ // 原常量参考：0x0058E944。
    '\n', 'n', ',', 'c', '|', 'z', '\\', 'y',
}};

class EditText final {
public:
    explicit EditText(InputContext& value)
        : context(value), contents(ContextText(value)),
          caret(ContextUtf16Offset(value, value.caretByteOffset)),
          visible(ContextUtf16Offset(value, value.visibleStartOffset)) {}
    bool commit()
    {
        context.setText(contents);
        context.totalByteLength = static_cast<std::int32_t>(contents.size());
        context.caretByteOffset = static_cast<std::int32_t>(text::Prefix(contents, caret));
        context.visibleStartOffset = static_cast<std::int32_t>(text::Prefix(contents, visible));
        return true;
    }
    InputContext& context;
    std::wstring contents;
    std::size_t caret;
    std::size_t visible;
};

std::wstring ContextText(const InputContext& context)
{
    return context.text;
}

std::size_t ContextUtf16Offset(const InputContext& context, const std::int32_t unitOffset)
{
    const std::size_t offset = static_cast<std::size_t>(unitOffset);
    return text::Prefix(context.text, offset);
}

void SetVisibleUtf16Offset(InputContext& context, const std::size_t utf16Offset)
{
    const std::wstring utf16 = ContextText(context);
    context.visibleStartOffset = static_cast<std::int32_t>(text::Prefix(utf16, utf16Offset));
}

std::vector<TextLine> SplitLines(const InputContext& context)
{
    const std::wstring contents = ContextText(context);
    const std::int32_t widthLimit = context.maxCharPerLine * render::GetFontMetrics(context.fontType).sWidth;
    std::vector<TextLine> lines;
    std::size_t start = 0;
    do {
        std::size_t end = start;
        while (end < contents.size() && contents[end] != '\r') {
            const std::size_t next = text::Next(contents, end);
            if (widthLimit > 0 && end > start
                && render::Font::measure(contents.c_str() + start, context.fontType, static_cast<std::int32_t>(next - start)) > widthLimit) {
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

void MoveCaretLine(session::ClientSession& session, const std::int32_t direction)
{
    InputContext* const context = session.UiFrame().inputState.currentContext;
    if (context == nullptr || platform::gImeComposition[0] != '\0') {
        return;
    }
    session.UiFrame().inputState.caretBlinkPhase = 20;
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
    const std::int32_t columnWidth = render::Font::measure(edit.contents.c_str() + lines[row].start, context->fontType,
        static_cast<std::int32_t>(edit.caret - lines[row].start));
    const TextLine& line = lines[static_cast<std::size_t>(destination)];
    edit.caret = line.start;
    while (edit.caret < line.end) {
        const std::size_t next = text::Next(edit.contents, edit.caret);
        if (render::Font::measure(edit.contents.c_str() + line.start, context->fontType, static_cast<std::int32_t>(next - line.start)) > columnWidth) {
            break;
        }
        edit.caret = next;
    }
    static_cast<void>(edit.commit());
}

void AppendText(session::ClientSession& session, const std::wstring_view utf16)
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
        InputContext* const context = session.UiFrame().inputState.currentContext;
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
                session.UiFrame().inputState.insertRejected = 1;
                return;
            }
        } catch (const std::system_error&) {
            m_log_iw(L"这项文字无法保存为GBK，或超过字段容量。");
            return;
        }
        const auto lines = SplitLines(*context);
        if (context->maxLine > 0 && lines.size() > static_cast<std::size_t>(context->maxLine)) {
            if (session.UiFrame().inputState.lineOverflowRollbackMode == 0) {
                edit.contents.resize(lines[static_cast<std::size_t>(context->maxLine - 1)].end);
                edit.caret = (std::min)(edit.caret, edit.contents.size());
                static_cast<void>(edit.commit());
            } else if (context->caretByteOffset == context->totalByteLength - 1) {
                edit.contents = old;
                edit.caret = oldCaret;
                static_cast<void>(edit.commit());
                session.UiFrame().inputState.insertRejected = 1;
            }
        }
        session.UiFrame().inputState.caretBlinkPhase = 20;
    }
}

// WIC01：原滚动零/坐标模式颜色发布序与真实context返回，见doc/cg2d_window_input_context_dispatch_spec.md。
// 原函数：0x0045D720。
InputContext* ConfigureInputLayoutAndResetScroll_45D720(
    InputContext* const context, const std::int32_t x, const std::int32_t y,
    [[maybe_unused]] const std::int32_t unused,
    const std::int32_t color, const std::int32_t renderMode)
{
    context->visibleWidth = 0;
    context->visibleStartOffset = 0;
    context->textY = y;
    context->textX = x;
    context->renderMode = renderMode;
    context->color = color;
    return context;
}

// WIC01：原坐标颜色模式字体发布序与真实context返回，见doc/cg2d_window_input_context_dispatch_spec.md。
// 原函数：0x0045D760。
InputContext* SetInputPositionAndStyle_45D760(
    InputContext* const context, const std::int32_t x, const std::int32_t y,
    const std::int32_t color, const std::int32_t renderMode,
    const std::int32_t fontType)
{
    context->textX = x;
    context->textY = y;
    context->color = color;
    context->renderMode = renderMode;
    context->fontType = fontType;
    return context;
}

// 原 sub_44B190@[0x44B190,0x44B1C5)。
std::int32_t ClassifyLastAnsiUnit_44B190(const char* text)
{
    std::int32_t result = 0;
    const char* current = text;
    while (*current != '\0') {
        if (static_cast<std::int8_t>(*current) < 0) {
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

// 原44B570固定扫描40字节；UWF01按UTF-16码元预算保留完整字符边界。
std::int32_t ClassifyChatSplit_44B570(const std::wstring_view text, const std::int32_t)
{
    return cg::text::Prefix(text, 40) == (std::min)(std::size_t{40}, text.size()) ? 2 : 0;
}

// 原 [0x44B440,0x44B49D)：仅 0x81..0x9F 与 0xE0..0xFC 为双字节首字节。
// UWF01：现按UTF-16码元定位，保S16偏移和低BYTE 0/1/2类别。
std::uint8_t ClassifyLegacyBoundary_44B440(
    const std::wstring_view text,
    const std::int16_t unitOffset)
{
    const std::size_t offset = static_cast<std::size_t>(unitOffset);
    if (cg::text::Prefix(text, offset) != offset) {
        return 2;
    }
    return offset < text.size() && text[offset] >= 0x80 ? 1 : 0;
}

// 原 [0x44B4A0,0x44B561)：目标首字节经 movsx 后，双字节目标分支对 W8 输入不可达。
char* FindLegacyByte_44B4A0(char* const text, const char* const target)
{
    const std::int32_t wanted = static_cast<std::int8_t>(target[0]);
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

// 原 sub_44B1D0@[0x44B1D0,0x44B244)。
[[maybe_unused]] static std::int32_t ClassifyTextBoundary_44B1D0(
    const std::wstring_view source,
    const std::int32_t count)
{
    const std::wstring_view contents(source);
    const std::size_t end = text::Prefix(contents, static_cast<std::size_t>(count));
    return static_cast<std::int32_t>(end - text::Previous(contents, end));
}

namespace detail {

// 原 [0x44A5D0,0x44A7CD)：按 CR 和最大字节宽度扫描行后上移光标。
void MoveInputCaretUp_44A5D0(session::ClientSession& session)
{
    MoveCaretLine(session, -1);
}

// 原 [0x44A540,0x44A580)：未移动时返回上下文指针，移动时返回边界分类。
std::uintptr_t MoveInputCaretLeft_44A540(session::ClientSession& session)
{
    InputContext* const context = session.UiFrame().inputState.currentContext;
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
    session.UiFrame().inputState.caretBlinkPhase = 20;
    return width;
}

// 原 [0x44A580,0x44A5C8)：按光标处字节的最高位移动一或两字节。
std::uintptr_t MoveInputCaretRight_44A580(session::ClientSession& session)
{
    InputContext* const context = session.UiFrame().inputState.currentContext;
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
    session.UiFrame().inputState.caretBlinkPhase = 20;
    return width;
}

// 原 [0x44A7D0,0x44A989)：按 CR 和最大字节宽度扫描行后下移光标。
void MoveInputCaretDown_44A7D0(session::ClientSession& session)
{
    MoveCaretLine(session, 1);
}

// 原 [0x44A990,0x44AA8D)：删除光标前一个旧 ANSI 单元。
void DeleteCharacterBeforeInputCaret_44A990(session::ClientSession& session)
{
    InputContext* const context = session.UiFrame().inputState.currentContext;
    if (context == nullptr || context->caretByteOffset <= 0) {
        return;
    }
    EditText edit(*context);
    const std::size_t previous = text::Previous(edit.contents, edit.caret);
    edit.contents.erase(previous, edit.caret - previous);
    edit.caret = previous;
    static_cast<void>(edit.commit());
    session.UiFrame().inputState.caretBlinkPhase = 20;
}

// 原 [0x44AA90,0x44AB59)：删除光标处一个旧 ANSI 单元，返回当前上下文。
InputContext* DeleteCharacterAtInputCaret_44AA90(session::ClientSession& session)
{
    InputContext* const context = session.UiFrame().inputState.currentContext;
    if (context == nullptr || context->caretByteOffset >= context->totalByteLength) {
        return context;
    }
    EditText edit(*context);
    edit.contents.erase(edit.caret, text::Next(edit.contents, edit.caret) - edit.caret);
    static_cast<void>(edit.commit());
    session.UiFrame().inputState.caretBlinkPhase = 20;
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
    if (const_cast<char*>(std::strstr(legacyText, cg::text::ToGbk(L"　").c_str())) != nullptr) {
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
        if (std::iscntrl(static_cast<std::int8_t>(legacyText[index])) != 0) {
            return true;
        }
    }

    return const_cast<char*>(std::strstr(legacyText, "[GP]")) != nullptr;
}

// 原函数：0x00476740。
std::int32_t IsValidDelimitedFieldText_476740(const std::wstring_view text)
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
std::int32_t ContainsPipeOrBackslash_45E770(const std::wstring_view text)
{
    const std::wstring_view contents(text);
    for (std::size_t offset = 0; offset < contents.size(); offset = cg::text::Next(contents, offset)) {
        if (contents[offset] == L'|' || contents[offset] == L'\\') { return 1; }
    }
    return 0;
}

// 原 [0x52D1B0,0x52D2C6)：动态UTF-16转义；原字节转义表保持，旧协议容量由提交边界检查。
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
    cg::core::NullSub_509080();
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

// 原 [0x52D2D0,0x52D2F0)：输入上下文的动态UTF-16适配，空叶及转义顺序由EscapeTextAndNotify保持。
std::wstring EscapeInputText_52D2D0(
    const InputContext* const context,
    const std::size_t gbkCapacity)
{
    return EscapeTextAndNotify(ContextText(*context), gbkCapacity);
}

// 原 [0x52D120,0x52D144)：未命中表的转义字母原样返回。
// UTE01：原四对BYTE映射/S32计数，未知字母低BYTE原返。
static char DecodeEscapeLetter_52D120(const char letter)
{
    for (std::int32_t pair = 0; pair < 4; ++pair) {
        if (static_cast<std::uint8_t>(letter)
            == kEscapePairs58E944[pair * 2 + 1]) {
            return static_cast<char>(kEscapePairs58E944[pair * 2]);
        }
    }
    return letter;
}

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
    cg::core::NullSub_509080();
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

// 原 [0x52D570,0x52D5BB)：UWF01按完整UTF-16字符与删除集合比较，更新动态正文。
char RemoveLegacyBytes_52D570(
    std::wstring& text,
    const std::wstring_view excluded)
{
    const std::wstring contents(text);
    const std::wstring_view skipped(excluded);
    std::wstring retained;
    for (std::size_t offset = 0; offset < contents.size();) {
        const std::size_t next = cg::text::Next(contents, offset);
        const auto unit = std::wstring_view(contents).substr(offset, next - offset);
        bool remove = false;
        for (std::size_t item = 0; item < skipped.size(); item = cg::text::Next(skipped, item)) {
            if (unit == skipped.substr(item, cg::text::Next(skipped, item) - item)) { remove = true; break; }
        }
        if (!remove) { retained.append(unit); }
        offset = next;
    }
    text = std::move(retained);
    return 0;
}

// 原函数：0x0044A160。
void LoadInitialText_44A160(session::ClientSession& session, const std::wstring_view initialText)
{
    if (session.UiFrame().inputState.currentContext != nullptr) { AppendText(session, initialText); }
}

// 原函数：0x0044ADB0。
void AppendChar_44ADB0(session::ClientSession& session, const wchar_t character)
{
    AppendText(session, std::wstring_view(&character, 1));
}

// 原函数：0x0044B340。
std::int32_t ProcessPresetHotkeys_44B340(session::ClientSession& session)
{
    auto& inputs = session.UiFrame().native.inputs;
    std::int32_t base = 0;
    if ((session.UiFrame().keyStates[0x10] & 2u) != 0) {
        base = 16;
    } else if ((session.UiFrame().keyStates[0x11] & 2u) != 0) {
        base = 8;
    }
    std::int32_t selected = -1;
    std::int32_t result = 0;
    for (std::int32_t index = 0; index < 8; ++index) {
        result = platform::ReadKeyPressFlag_4597B0(session, 0x70 + index);
        if (result != 0) {
            selected = base + index;
        }
    }
    if (selected >= 0) {
        LoadInitialText_44A160(session, ContextText(inputs.hotkeys[selected]).c_str());
        return audio::PlaySe_52C450(51, 320);
    }
    return result;
}

// 原函数：0x0044B330。
void SetContext_44B330(session::ClientSession& session, InputContext* const context)
{
    session.UiFrame().inputState.currentContext = context;
}

// 原函数：0x00503E40。
std::int32_t IsCurrentContext_503E40(session::ClientSession& session, const InputContext* const context)
{
    return session.UiFrame().inputState.currentContext == context ? 1 : 0;
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

std::int32_t ExtractDialogTextLine(
    std::wstring& destination, const char* source,
    const std::int32_t requestedLine, const std::int32_t width)
{
    if (requestedLine < 0 || width <= 0) { return 0; }
    const std::wstring contents = text::FromGbkWide(source);
    std::wstring selected;
    std::int32_t line = 0;
    std::int32_t column = 0;
    for (std::size_t offset = 0; offset < contents.size() && line <= requestedLine;) {
        const wchar_t current = contents[offset];
        if (current == L'\r' || current == L'\n' || current == 1) {
            if (line == requestedLine) { break; }
            line = current == 1 ? 8 * (line / 8) + 8 : line + 1;
            column = 0;
            ++offset;
            continue;
        }
        bool escapedDollar = false;
        if (current == L'$' && offset + 1 < contents.size()) {
            const wchar_t code = contents[offset + 1];
            if (text::InlineColorIndex(code) >= 0) {
                if (line == requestedLine) { selected.append(contents, offset, 2); }
                offset += 2;
                continue;
            }
            escapedDollar = code == L'$';
        }
        const std::size_t next = escapedDollar ? offset + 2 : text::Next(contents, offset);
        const std::int32_t columns = current < 0x80 ? 1 : 2;
        // 满行后遇到下一可见字符才自动换行，保留随后的显式换行／分页含义。
        if (column != 0 && column + columns > width) {
            ++line;
            column = 0;
            continue;
        }
        if (line == requestedLine) { selected.append(contents, offset, next - offset); }
        column += columns;
        offset = next;
    }
    destination = std::move(selected);
    return 1;
}

// 原0x00449CF0：初始正文直接使用UTF-16。
void InitializeContext_449CF0(
    session::ClientSession& session,
    InputContext* const context,
    const std::int32_t textX,
    const std::int32_t textY,
    const std::int32_t renderMode,
    const std::int32_t fontType,
    const std::int32_t color,
    const std::wstring_view initialText,
    const std::int32_t maxLine,
    const std::int32_t maxCharPerLine,
    const std::int32_t lineHeight,
    const std::int32_t encrypt)
{
    context->textX = textX;
    context->renderMode = renderMode;
    context->color = color;
    context->textY = textY;
    context->maxCharPerLine = maxCharPerLine;
    context->fontType = fontType;
    context->encrypt = encrypt;
    context->maxLine = maxLine;
    context->lineHeight = lineHeight;
    context->clearText();
    context->totalByteLength = 0;
    context->caretByteOffset = 0;
    context->visibleWidth = 0;
    context->visibleStartOffset = 0;

    if (!initialText.empty()) {
        InputContext* const savedContext = session.UiFrame().inputState.currentContext;
        SetContext_44B330(session, context);
        LoadInitialText_44A160(session, initialText);
        SetContext_44B330(session, savedContext);
    }
}

// HBN01：原历史选择/独立游标与现场context清写、字节滚动尾，正确正文保持。
// 原函数：0x0044A1B0。
void BrowseChatHistory_44A1B0(session::ClientSession& session)
{
    auto& inputs = session.UiFrame().native.inputs;
    auto& history = session.WorldScene().chatInputHistory;
    cg::core::NullSub_509080();
    InputContext* const initialContext = session.UiFrame().inputState.currentContext;
    if (initialContext != &inputs.chat || platform::gImeComposition[0] != '\0') {
        return;
    }
    const std::uint8_t control = session.UiFrame().keyStates[17] & 2u;
    if (control == 0 && (session.UiFrame().keyStates[16] & 2u) == 0
        && (session.UiFrame().keyStates[38] & 0x10u) != 0) {
        const std::int32_t previous = history.cursor62683C;
        const auto tail = std::bit_cast<std::int32_t>(history.tail626838);
        std::int32_t selected = previous == -1 ? tail : previous - 1;
        history.cursor62683C = selected;
        if (selected < 0) {
            selected = 63;
            history.cursor62683C = selected;
        }
        if (history.records[selected].empty()
            || (selected == tail && previous != -1)) {
            history.cursor62683C = previous;
            return;
        }
    } else {
        // 向下复用已读Ctrl位，不增加Shift门禁或空记录检查。
        if (control != 0 || (session.UiFrame().keyStates[40] & 0x10u) == 0) {
            return;
        }
        const std::int32_t selected = history.cursor62683C;
        if (selected == -1) {
            return;
        }
        if (selected == std::bit_cast<std::int32_t>(history.tail626838)) {
            history.cursor62683C = -1;
            initialContext->totalByteLength = 0;
            session.UiFrame().inputState.currentContext->clearText();
            session.UiFrame().inputState.currentContext->caretByteOffset = 0;
            return;
        }
        const std::int32_t next = selected + 1;
        history.cursor62683C = next;
        if (next >= 64) {
            history.cursor62683C = 0;
        }
    }
    initialContext->totalByteLength = 0;
    session.UiFrame().inputState.currentContext->clearText();
    session.UiFrame().inputState.currentContext->caretByteOffset = 0;
    LoadInitialText_44A160(session, history.records[history.cursor62683C].c_str());
    const std::wstring contents = ContextText(*session.UiFrame().inputState.currentContext);
    const std::size_t width = static_cast<std::size_t>(ui::WindowChat_ReadInputByteWidth(session));
    std::size_t offset = contents.size() > width ? contents.size() - width : 0;
    offset = text::Prefix(contents, offset);
    SetVisibleUtf16Offset(*session.UiFrame().inputState.currentContext, offset);
}

namespace detail {

// 原函数：0x0044AB60。
void HandleInputEnterKey_44AB60(session::ClientSession& session)
{
    auto& inputs = session.UiFrame().native.inputs;
    if (platform::gImeComposition[0] != '\0') {
        return;
    }
    InputContext* context = session.UiFrame().inputState.currentContext;
    if (context == &inputs.chat) {
        if (context->totalByteLength == 0) {
            return;
        }
        std::string stored;
        try {
            stored = cg::text::ToGbk(ContextText(*context));
            if (stored.size() >= 0x121) {
                throw cg::text::GbkStorageError(122, std::system_category(), "文字超过聊天历史GBK容量");
            }
            // 先验证P|之后的转义正文，再发布历史；此预检不调用原通知空叶。
            static_cast<void>(EscapeText(ContextText(*context), 1022));
        }
        catch (const std::system_error&) {
            m_log_iw(L"这项文字无法保存为GBK，或超过字段容量。");
            return;
        }
        auto& history = session.WorldScene().chatInputHistory;
        const std::uint32_t oldTail = history.tail626838;
        const std::int32_t oldIndex = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(oldTail));
        std::int32_t nextIndex = std::bit_cast<std::int32_t>(oldTail) + 1;
        history.tail626838 = static_cast<std::uint32_t>(nextIndex);
        if (nextIndex >= 64) {
            nextIndex = 0;
            history.tail626838 = 0;
        }
        const bool sameAsPrevious = ContextText(*context) == history.records[oldIndex];
        if (sameAsPrevious) {
            history.tail626838 = static_cast<std::uint32_t>(oldIndex);
        } else {
            history.records[nextIndex] = ContextText(*context);
            static_cast<void>(world::SaveChatInputHistoryEntry_449FA0(
                session, std::bit_cast<std::int32_t>(history.tail626838)));
            context = session.UiFrame().inputState.currentContext;
        }
        const std::int32_t color = inputs.chat.color;
        history.cursor62683C = -1;
        static_cast<void>(world::SendPlayerChatInput_50C940(session, context, color));
        session.UiFrame().inputState.currentContext->totalByteLength = 0;
        session.UiFrame().inputState.currentContext->caretByteOffset = 0;
        session.UiFrame().inputState.currentContext->clearText();
        context = session.UiFrame().inputState.currentContext;
    } else if (context == &inputs.mail
               || context == &inputs.contextCB03F0
               || context == &session.UiFrame().inputState.contextCC46E0
               || context == &inputs.messageBoard
               || context == &inputs.profileDescription
               || context == &inputs.miniMail) {
        AppendChar_44ADB0(session, L'\r');
        context = session.UiFrame().inputState.currentContext;
    }
    if (context == &inputs.blacklist) {
        std::wstring message;
        const std::wstring text = ContextText(inputs.blacklist);
        const std::wstring playerName = world::GetPlayerDisplayText(session, 1);
        const bool targetsSelf = text == playerName;
        if (targetsSelf) {
            message = L"您…如此厌恶自己的声音吗？" /* 原常量参考：0x00554938。 */;
        } else if (cg::world::blacklist::ContainsBlacklistRecord_4013D0(session, text) != 0) {
            message = std::format(L"{}已在黑名单中" /* 原常量参考：0x00554928。 */, text);
        } else {
            message = cg::world::blacklist::AddBlacklistRecord_401270(session, text) != 0
                ? std::format(L"已将{}加入黑名单" /* 原常量参考：0x005532C4。 */, text)
                : std::format(L"{}加入黑名单失败" /* 原常量参考：0x005532B0。 */, text);
        }
        inputs.blacklist.clearText();
        inputs.blacklist.caretByteOffset = 0;
        inputs.blacklist.totalByteLength = 0;
        world::AppendChatMessage_44B050(session, message, 4, 3);
        SetContext_44B330(session, &inputs.chat);
    }
    session.UiFrame().inputState.caretBlinkPhase = 20;
}

} // namespace detail

} // namespace cg::input
