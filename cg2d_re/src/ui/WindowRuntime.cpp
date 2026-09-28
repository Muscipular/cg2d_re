#include "WindowRuntimeState.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../platform/WindowProcedureState.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace cg::ui {
namespace detail {

std::array<WindowInstance*, kWindowCount> gWindows {};
std::array<WindowState, kWindowCount> gWindowStates {};
WindowInstance* gCurrentWindow = nullptr;
WindowState* gCurrentWindowState = nullptr;
std::uint8_t gValueD67BE0 = 0;
std::int32_t gDragItemIndex584DA4 = -1;
void* gFocusedElementDataD45F94 = nullptr;
std::int32_t gMouseValue57C110 = 0;
std::int32_t gDragSourceIndex57C114 = 0;
WindowDragCallback gDragCallback57C118 = nullptr;
std::int16_t gDragX57C11C = 0;
std::int16_t gDragY57C11E = 0;
std::int32_t gFocusWindowId57C120 = 0;
std::int32_t gFocusElementIndex57C124 = -1;
std::int32_t gMousePhase57C128 = 0;
WindowInputPrototype gChatInputPrototype57E970 {
    {{&input::gInputContext626AA0}, WindowInputReferenceKind::Context},
    8, 5, 3, 9, 0, {}, 1, 76, 0, 0,
};
std::int32_t gChatWindowModeD66E04 = 0;
std::uint8_t gValue5F9754 = 0;
std::int32_t gChatWidthUnitsC9A930 = 0;
std::int32_t gChatHeightUnitsC9A938 = 0;
std::int32_t gChatScrollOffsetC9A940 = 0;

namespace {

// 原54EFE8的71项S8类别，与窗口编号一一对应。
constexpr std::array<std::int8_t, kWindowCount> kWindowCategories {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 4, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 4, 4, 0, 1, 2, 2, 0, 1, 2, 4, 2, 2, 2, 2, 2,
    1, 1, 2, 4, 2, 2, 2, 1, 2, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 2, 2, 2,
};

constexpr std::array<std::int32_t, 52> kNumberDigitImages54F0D8 {
    243300, 243310, 243320, 243330, 243340, 243350, 243360,
    243370, 243380, 243390, 245500, 245500, 245500,
    243500, 243510, 243520, 243530, 243540, 243550, 243560,
    243570, 243580, 243590, 243500, 243500, 243500,
    243700, 243710, 243720, 243730, 243740, 243750, 243760,
    243770, 243780, 243790, 243700, 243700, 243700,
    245520, 245520, 245520, 245520, 245520, 245520, 245560,
    245580, 245520, 245520, 245540, 245540, 245540,
};

constexpr std::array<std::int32_t, 52> kNumberSymbolImages54F1A8 {
    243400, 243410, 243420, 243430, 243440, 243450, 243460,
    243470, 243480, 243490, 245510, 245510, 245510,
    243600, 243610, 243620, 243630, 243640, 243650, 243660,
    243670, 243680, 243690, 243600, 243600, 243600,
    243800, 243810, 243820, 243830, 243840, 243850, 243860,
    243870, 243880, 243890, 243800, 243800, 243800,
    245530, 245530, 245530, 245530, 245530, 245530, 245570,
    245580, 245530, 245530, 245550, 245550, 245550,
};

constexpr std::array<std::int32_t, 12> kNumberOffsets54F278 {
    2, 2, 5, 5, 4, 8, 5, 5, 8, 2, 3, 5,
};
constexpr std::array<std::int32_t, 4> kNumberSpacingDeductions54F2A8 {2, 2, 2, 1};
std::array<std::int32_t, 4> gNumberAdvancesD67AC4 {};

// 原5FB7F4是最后一条输入上下文的尾槽，不是日志511的font。
std::int32_t& PreviousChatFont(const std::int32_t index)
{
    if (index == 0) {
        return input::gInputContexts5F9758[23].field158;
    }
    return world::detail::gRecords5FB7F8[index - 1].fontType;
}

// 两个原入口只在宽度来源上不同；测量IME及取宽度仍由各入口按序执行。
void ScrollInputAndDraw(input::InputContext* const context, const std::int32_t width)
{
    const std::int32_t caret = context->caretByteOffset;
    const std::int32_t offset = context->field158;
    context->field154 = width;
    if (caret < offset) {
        context->field158 = caret;
    } else {
        for (;;) {
            const std::int32_t currentOffset = context->field158;
            const std::int32_t length = context->caretByteOffset - currentOffset;
            if (length < 0) {
                break;
            }
            const std::int32_t textWidth = render::Font::measure(
                context->text + currentOffset, context->field13C, length);
            if (context->field154 >= textWidth) {
                break;
            }
            const std::uint8_t value = static_cast<std::uint8_t>(
                context->text[context->field158]);
            context->field158 += (value & 0x80u) != 0 ? 2 : 1;
        }
    }
    render::DrawInputText_4568A0(context);
}

} // namespace

void Sub_4A7590(input::InputContext* const context)
{
    const std::int32_t font = input::gCurrentInputData->field13C;
    static_cast<void>(login::detail::TextMeasure(platform::gImeComposition, font));
    const std::int32_t width = render::gFontMetrics[9].doubleWidth * gChatWidthUnitsC9A930;
    ScrollInputAndDraw(context, width);
}

void Sub_4A7640(input::InputContext* const context)
{
    const std::int32_t font = input::gCurrentInputData->field13C;
    static_cast<void>(login::detail::TextMeasure(platform::gImeComposition, font));
    ScrollInputAndDraw(context, 1024);
}

void Sub_4A7750(const std::int32_t x, const std::int32_t y)
{
    std::int32_t totalHeight = 0;
    const std::int32_t width = gChatWindowModeD66E04 == 1
        ? render::gFontMetrics[9].doubleWidth * 44
        : (gChatWidthUnitsC9A930 + 7) * render::gFontMetrics[9].doubleWidth;
    std::int32_t logIndex = world::detail::gWriteIndex626C08;
    logIndex -= gChatScrollOffsetC9A940;
    --logIndex;
    if (logIndex < 0) {
        logIndex = 511;
    }
    for (auto& record : world::detail::gRecords5FB7F8) {
        if (record.fontType > 14 || PreviousChatFont(logIndex) < 0) {
            record.fontType = 0;
        }
    }
    std::int32_t drawY = y - render::gFontMetrics[
        world::detail::gRecords5FB7F8[logIndex].fontType].doubleHeight;
    std::int32_t processedRecords = 0;
    char scratch[0xC70];
    char* const rows = scratch + 0x124;
    static_assert(0x124 + 10 * 0x121 + 2 == sizeof(scratch));
    for (;;) {
        auto& source = world::detail::gRecords5FB7F8[logIndex];
        if (source.text[0] == '\0') {
            return;
        }
        std::strcpy(scratch, source.text);
        const auto textLength = static_cast<std::int32_t>(std::strlen(scratch));
        std::int32_t* fontSlot = &source.fontType;
        std::int32_t lineCount = login::detail::TextMeasure(scratch, *fontSlot) / width;
        if (login::detail::TextMeasure(scratch, *fontSlot) % width != 0) {
            ++lineCount;
        }

        std::int32_t sourceOffset = 0;
        for (std::int32_t lineIndex = 0; lineIndex < lineCount; ++lineIndex) {
            std::int32_t column = 0;
            if (textLength > 0) {
                std::int32_t rowOffset = lineIndex * 0x121;
                for (;;) {
                    char* const outputByte = rows + rowOffset + column;
                    *outputByte = scratch[sourceOffset];
                    outputByte[1] = '\0';
                    outputByte[2] = '\0';
                    if (logIndex > 512) {
                        logIndex = world::detail::gWriteIndex626C08;
                        fontSlot = &world::detail::gRecords5FB7F8[logIndex].fontType;
                    }
                    char* const currentRow = rows + rowOffset;
                    const std::int32_t rowWidth =
                        login::detail::TextMeasure(currentRow, *fontSlot);
                    if (width < rowWidth) {
                        if (input::ClassifyLastAnsiUnit_44B190(currentRow) == 3) {
                            *outputByte = '\0';
                            --sourceOffset;
                        }
                        rowOffset += 0x121;
                        ++lineIndex;
                        column = -1;
                    }
                    ++sourceOffset;
                    if (sourceOffset > textLength) {
                        break;
                    }
                    ++column;
                    if (column >= textLength) {
                        break;
                    }
                }
            }
            if (sourceOffset > textLength) {
                break;
            }
        }

        if (lineCount > 0) {
            auto& record = world::detail::gRecords5FB7F8[logIndex];
            const char* row = rows + (lineCount - 1) * 0x121;
            for (std::int32_t line = 0; line < lineCount; ++line) {
                const std::int32_t font = record.fontType;
                totalHeight += render::gFontMetrics[font].doubleHeight;
                const std::int32_t heightLimit =
                    render::gFontMetrics[9].doubleHeight * gChatHeightUnitsC9A938;
                if (totalHeight > heightLimit) {
                    return;
                }
                if (*row != '\0') {
                    const std::int32_t color = record.color;
                    static_cast<void>(render::DrawText2_456670(
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                        3,
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(font)),
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(color)),
                        row, 0));
                }

                std::int32_t spacingFont;
                if (line + 1 == lineCount) {
                    const std::int32_t previous = PreviousChatFont(logIndex);
                    if (previous > 14 || previous < 0) {
                        PreviousChatFont(logIndex) = 0;
                    }
                    spacingFont = PreviousChatFont(logIndex);
                } else {
                    if (record.fontType > 14 || PreviousChatFont(logIndex) < 0) {
                        record.fontType = 0;
                    }
                    spacingFont = record.fontType;
                }
                row -= 0x121;
                drawY -= render::gFontMetrics[spacingFont].doubleHeight;
            }
        }

        --logIndex;
        ++processedRecords;
        if (logIndex < 0) {
            logIndex = 511;
        }
        if (processedRecords >= world::detail::gCountC9A944) {
            return;
        }
    }
}

void Sub_4771B0(
    const std::int32_t x, const std::int32_t y, const char* const text,
    const std::int8_t style, const std::int8_t color,
    const std::uint8_t flags, const std::uint8_t layer)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFFu;
    descriptor.bytes[9] = 0xFFu;
    descriptor.bytes[10] = 0xFFu;
    descriptor.bytes[11] = 0xFFu;
    descriptor.bytes[13] = 4;

    std::int32_t drawX;
    std::int32_t drawY;
    std::int32_t step;
    const bool reverse = (flags & 1u) != 0;
    if (reverse) {
        drawX = x - kNumberOffsets54F278[style * 3 + 1];
        drawY = kNumberOffsets54F278[style * 3 + 2];
        step = -gNumberAdvancesD67AC4[style];
        drawY += y;
    } else {
        step = gNumberAdvancesD67AC4[style];
        drawX = kNumberOffsets54F278[style * 3] + x;
        drawY = kNumberOffsets54F278[style * 3 + 2] + y;
    }

    const auto drawByte = [&](const char character) {
        std::int32_t imageNo;
        if (character >= '0' && character <= '9') {
            imageNo = kNumberDigitImages54F0D8[style * 13 + color] + character - '0';
        } else {
            std::int32_t symbol;
            switch (character) {
            case '+': symbol = 0; break;
            case '-': symbol = 1; break;
            case ':': symbol = 2; break;
            case '/': symbol = 3; break;
            default: return;
            }
            imageNo = kNumberSymbolImages54F1A8[style * 13 + color] + symbol;
        }
        static_cast<void>(login::detail::DrawGraphic(
            drawX, drawY, layer, imageNo, 0, &descriptor));
    };

    const char* cursor = text;
    if (reverse) {
        while (*cursor != '\0') {
            ++cursor;
        }
        while (cursor != text) {
            --cursor;
            drawByte(*cursor);
            drawX += step;
        }
    } else {
        while (*cursor != '\0') {
            drawByte(*cursor);
            drawX += step;
            ++cursor;
        }
    }
}

std::int32_t Sub_477460(const char* const text, const std::int8_t style)
{
    const auto length = static_cast<std::int32_t>(std::strlen(text));
    if (length <= 0) {
        return 0;
    }
    return gNumberAdvancesD67AC4[style] * length - kNumberSpacingDeductions54F2A8[style];
}

std::int32_t Sub_46FBC0(const std::int32_t)
{
    DrawCurrentWindow_4729B0();
    return 1;
}

std::uint8_t Sub_476570(
    WindowInputPrototype* const prototype, const std::int32_t x,
    const std::int32_t y, const std::uint32_t mode)
{
    if (prototype->input.kind == WindowInputReferenceKind::RejectedValue) {
        return 0;
    }
    input::InputContext* const context = prototype->input.context;
    if (context == nullptr) {
        return 0;
    }
    switch (mode) {
    case 0: {
        const std::int32_t encrypt = prototype->encrypt;
        const std::int32_t value224 = prototype->value224;
        const std::int32_t maxCharPerLine = prototype->maxCharPerLine;
        const std::int32_t maxLine = prototype->maxLine;
        const char* const text = prototype->text.data();
        const std::int32_t color = prototype->color;
        const std::int32_t fontType = prototype->fontType;
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t inputY = prototype->y + y;
        const std::int32_t inputX = prototype->x + x;
        input::InitializeContext_449CF0(
            context, inputX, inputY, renderMode, fontType, color, text,
            maxLine, maxCharPerLine, value224, encrypt);
        return 1;
    }
    case 1: {
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t color = prototype->color;
        const std::int32_t maxCharPerLine = prototype->maxCharPerLine;
        const std::int32_t inputY = prototype->y + y;
        const std::int32_t inputX = prototype->x + x;
        input::Sub_45D720(context, inputX, inputY, maxCharPerLine, color, renderMode);
        [[fallthrough]];
    }
    case 2: {
        const std::int32_t inputX = prototype->x + x;
        prototype->input.context->field130 = inputX;
        const std::int32_t inputY = prototype->y + y;
        prototype->input.context->field134 = inputY;
        return 1;
    }
    case 3: {
        const std::int32_t fontType = prototype->fontType;
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t color = context->field140;
        const std::int32_t inputY = prototype->y + y;
        const std::int32_t inputX = prototype->x + x;
        input::Sub_45D760(context, inputX, inputY, color, renderMode, fontType);
        break;
    }
    default:
        break;
    }
    return 1;
}

void Sub_4740F0(char* const destination)
{
    std::int32_t count = 0;
    for (std::size_t position = 0; position < 10; ++position) {
        const auto& pair = world::detail::gSkillPairsF63370[position];
        if (pair.used != 0) {
            const std::int16_t index = std::bit_cast<std::int16_t>(pair.index);
            const auto& skill = world::detail::gSkillsF1BAFC[index];
            std::int16_t requiredSlots;
            std::memcpy(&requiredSlots, skill.requiredSlots19.data(), sizeof(requiredSlots));
            count += requiredSlots;
        }
    }
    char text[256];
    std::sprintf(text, "%2d/10", count);
    std::strcpy(destination, text);
}

std::int32_t Sub_4A7B00()
{
    const std::int32_t mode = gChatWindowModeD66E04;
    if (mode == 1) {
        WindowInstance* const window = gWindowStates[57].window;
        if (window == nullptr) {
            return 0;
        }
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(Sub_476570(&gChatInputPrototype57E970, x, y, 3));
        gFocusedElementDataD45F94 = gWindowStates[57].window->elements[0].data;
        input::SetContext_44B330(&input::gInputContext626AA0);
        return 1;
    }
    if (mode != 0) {
        return 0;
    }
    WindowInstance* const window = gWindowStates[35].window;
    if (window == nullptr) {
        return 0;
    }
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    if (Sub_476570(&gChatInputPrototype57E970, x, y, 3) == 0) {
        for (;;) {
            WindowInstance* const currentWindow = gWindowStates[35].window;
            gChatInputPrototype57E970.input.context = &input::gInputContext626AA0;
            gChatInputPrototype57E970.input.kind = WindowInputReferenceKind::Context;
            gChatInputPrototype57E970.x = 8;
            std::memcpy(
                &input::gInputContext626AA0, &world::detail::gInputContext626840,
                sizeof(input::InputContext));
            gChatInputPrototype57E970.y = 5;
            gChatInputPrototype57E970.renderMode = 3;
            gChatInputPrototype57E970.fontType = 9;
            gChatInputPrototype57E970.color = 0;
            gChatInputPrototype57E970.maxLine = 1;
            gChatInputPrototype57E970.maxCharPerLine = 76;
            gChatInputPrototype57E970.value224 = 0;
            gChatInputPrototype57E970.encrypt = 0;
            const std::int32_t currentY = currentWindow->y;
            const std::int32_t currentX = currentWindow->x;
            if (Sub_476570(&gChatInputPrototype57E970, currentX, currentY, 3) != 0) {
                break;
            }
        }
    }
    gFocusedElementDataD45F94 = gWindowStates[35].window->elements[0].data;
    input::SetContext_44B330(&input::gInputContext626AA0);
    return 1;
}

} // namespace detail

void InitializeNumberSpacing_477170()
{
    using namespace detail;
    const std::int32_t first = kNumberSpacingDeductions54F2A8[0];
    const std::int32_t second = kNumberSpacingDeductions54F2A8[1];
    const std::int32_t third = kNumberSpacingDeductions54F2A8[2];
    gNumberAdvancesD67AC4[0] = first + 5;
    const std::int32_t fourth = kNumberSpacingDeductions54F2A8[3];
    gNumberAdvancesD67AC4[1] = second + 10;
    gNumberAdvancesD67AC4[2] = third + 11;
    gNumberAdvancesD67AC4[3] = fourth + 7;
}

std::uint8_t ReadValueD67BE0() noexcept
{
    return detail::gValueD67BE0;
}

std::uint8_t ReadValue5F9754() noexcept
{
    return detail::gValue5F9754;
}

std::int32_t ReadDragItemIndex_584DA4() noexcept
{
    return detail::gDragItemIndex584DA4;
}

WindowInstance* Sub_4CD3E0()
{
    WindowInstance* const window = detail::gWindowStates[68].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

WindowInstance* Sub_4CD400()
{
    WindowInstance* const window = detail::gWindowStates[67].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

void CancelWindowDrag_476410()
{
    using namespace detail;
    std::int32_t phase = gMousePhase57C128;
    if (phase == 0) {
        return;
    }
    const WindowDragCallback callback = gDragCallback57C118;
    if (callback != nullptr) {
        callback(phase == 1 ? 1 : 4, 0, gDragSourceIndex57C114);
        phase = gMousePhase57C128;
    }
    gMouseValue57C110 = 0;
    gDragSourceIndex57C114 = 0;
    gDragCallback57C118 = nullptr;
    gDragX57C11C = 0;
    gDragY57C11E = 0;
    gFocusWindowId57C120 = 0;
    gFocusElementIndex57C124 = -1;
    gMousePhase57C128 = phase == 1 ? 2 : 6;
}

void FreeWindow_471270(WindowInstance* const window)
{
    using namespace detail;
    window->release();
    gCurrentWindowState = &gWindowStates[window->id];
    gCurrentWindowState->x = window->x;
    gCurrentWindowState->y = window->y;
    gCurrentWindowState->flags &= ~std::uint32_t{0x80};
    gCurrentWindowState->flags |= 1u;
    gCurrentWindowState->window = nullptr;
    if (gFocusWindowId57C120 == window->id) {
        CancelWindowDrag_476410();
    }
    for (std::int32_t index = 0; index < window->elementCount; ++index) {
        WindowElement* const element = &window->elements[index];
        switch (element->type) {
        case 1:
        case 5:
            if (element->data == gFocusedElementDataD45F94) {
                static_cast<void>(Sub_4A7B00());
            }
            if (void* const data = window->elements[index].data; data != nullptr) {
                std::free(data);
                window->elements[index].data = nullptr;
            }
            break;
        case 2:
            if (element->data != nullptr) {
                const auto* const data = static_cast<const WindowAnimationData*>(element->data);
                login::detail::MarkDeath(data->action);
                std::free(window->elements[index].data);
                window->elements[index].data = nullptr;
            }
            break;
        case 3: {
            auto* const data = static_cast<WindowTextData*>(element->data);
            if (data->text != nullptr) {
                std::free(data->text);
                static_cast<WindowTextData*>(window->elements[index].data)->text = nullptr;
            }
            if (void* const current = window->elements[index].data; current != nullptr) {
                std::free(current);
                window->elements[index].data = nullptr;
            }
            break;
        }
        case 4:
        case 6:
        case 7:
            if (element->data != nullptr) {
                std::free(element->data);
                window->elements[index].data = nullptr;
            }
            break;
        default:
            break;
        }
    }
    if (window->elements != nullptr) {
        std::free(window->elements);
    }
    window->elements = nullptr;
    std::free(window);
}

void CloseWindowsByCategory_472240(const std::uint32_t mask)
{
    using namespace detail;
    for (std::size_t index = 0; index < gWindows.size(); ++index) {
        WindowInstance* const window = gWindows[index];
        if (window == nullptr) {
            continue;
        }
        const std::int32_t id = window->id;
        const auto category = static_cast<std::uint32_t>(
            static_cast<std::int32_t>(kWindowCategories[id]));
        if ((mask & category) != 0) {
            gCurrentWindowState = &gWindowStates[id];
            gCurrentWindowState->flags |= 2u;
            FreeWindow_471270(gWindows[index]);
            gWindows[index] = nullptr;
        }
    }
}

void DrawCurrentWindow_4729B0()
{
    using namespace detail;
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* window = gCurrentWindow;
    for (std::int32_t index = 0; index < window->elementCount; ++index) {
        WindowElement* const element = &window->elements[index];
        if (element->visible == 1) {
            const std::int32_t x = window->x + element->x;
            const std::int32_t y = window->y + element->y;
            switch (element->type) {
            case 1: {
                const auto* const data = static_cast<const WindowGraphicData*>(element->data);
                if (data->imageNo == 0) {
                    break;
                }
                std::array<std::uint16_t, 4> rectangle {};
                static_assert(sizeof(rectangle) == 8);
                std::uint8_t cropped = 0;
                if (data->offsetX != 0 || data->offsetY != 0
                    || data->width != 0 || data->height != 0) {
                    std::int32_t graphicIndex;
                    resource::GetGraphicMapNoIndex(data->imageNo, &graphicIndex);
                    std::int16_t graphicX;
                    std::int16_t graphicY;
                    resource::GetGraphicXY(
                        static_cast<std::uint32_t>(graphicIndex), &graphicX, &graphicY);
                    rectangle[0] = static_cast<std::uint16_t>(
                        static_cast<std::uint16_t>(data->offsetX) + graphicX);
                    rectangle[1] = static_cast<std::uint16_t>(
                        static_cast<std::uint16_t>(data->offsetY) + graphicY);
                    rectangle[2] = static_cast<std::uint16_t>(data->width);
                    rectangle[3] = static_cast<std::uint16_t>(data->height);
                    cropped = 1;
                }
                std::memcpy(descriptor.bytes.data(), rectangle.data(), sizeof(rectangle));
                std::memcpy(descriptor.bytes.data() + 8, &data->color, sizeof(data->color));
                descriptor.bytes[12] = cropped;
                descriptor.bytes[13] = 4;
                descriptor.bytes[14] = 0;
                descriptor.bytes[15] = 0;
                WindowInstance* const drawWindow = gCurrentWindow;
                const std::int32_t imageNo = data->imageNo;
                const std::int32_t drawY = drawWindow->y + element->y;
                const std::int32_t drawX = drawWindow->x + element->x;
                static_cast<void>(render::DrawGraphic3_520970(
                    drawX, drawY, 0xDF, imageNo, 0, 1, 0, &descriptor));
                break;
            }
            case 2: {
                const auto* const data = static_cast<const WindowAnimationData*>(element->data);
                const std::uint32_t stop = data->value04;
                login::YobiAction* const action = data->action;
                static_cast<void>(login::detail::Sub_517880(action, 100, stop != 0, 0));
                const std::int32_t graphicIndex = login::detail::GetYobiActionField20(action);
                if (graphicIndex == 0) {
                    break;
                }
                const render::GraphicDescriptor16* const mode =
                    login::detail::ApplyWindowAnimationDrawMode(action);
                const std::int32_t offsetY = login::detail::Sub_5182B0(action, 1);
                WindowInstance* const verticalWindow = gCurrentWindow;
                const std::int32_t elementY = element->y;
                const std::int32_t drawY = offsetY + verticalWindow->y + elementY;
                const std::int32_t offsetX = login::detail::Sub_5182B0(action, 0);
                WindowInstance* const horizontalWindow = gCurrentWindow;
                const std::int32_t elementX = element->x;
                const std::int32_t drawX = offsetX + horizontalWindow->x + elementX;
                static_cast<void>(render::DrawGraphic2_521DD0(
                    drawX, drawY, 0xDF, graphicIndex, 0, 0, mode));
                break;
            }
            case 3: {
                const auto* const data = static_cast<const WindowTextData*>(element->data);
                const char* const text = data->text;
                if (text != nullptr) {
                    const std::int32_t drawY = y - element->height;
                    const auto color = std::bit_cast<std::int8_t>(
                        std::to_integer<std::uint8_t>(data->bytes00[0]));
                    const auto font = std::bit_cast<std::int8_t>(
                        std::to_integer<std::uint8_t>(data->bytes00[1]));
                    const std::int32_t drawX = x - element->width;
                    static_cast<void>(render::DrawText2_456670(
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                        3, font, color, text, 0));
                }
                break;
            }
            case 5: {
                const auto* const data = static_cast<const WindowInputData*>(element->data);
                if (data == gFocusedElementDataD45F94) {
                    if (input::gCurrentInputData == &input::gInputContext626AA0) {
                        if (input::gValueC9A96C == 1) {
                            Sub_4A7590(data->context);
                        }
                    } else if (data != nullptr) {
                        Sub_4A7640(data->context);
                    }
                }
                break;
            }
            case 6:
                Sub_4A7750(x + 5, y);
                break;
            case 7: {
                const auto* const data = static_cast<const WindowNumberData*>(element->data);
                if (data != nullptr) {
                    Sub_4771B0(
                        x, y, data->text, data->style,
                        std::bit_cast<std::int8_t>(data->color), data->flags, 0xDF);
                }
                break;
            }
            default:
                break;
            }
        }
        window = gCurrentWindow;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    world::detail::Sub_476680();
}

void ApplyObservationInputGate_440ADB()
{
    auto* const bytes = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    if (detail::gValue5F9754 != 0) {
        const unsigned char previous = bytes[7];
        detail::gValue5F9754 = 0;
        bytes[7] = static_cast<unsigned char>(previous - 1);
    }
    for (std::size_t index = 0; index < 8; ++index) {
        bytes[index] = static_cast<unsigned char>(bytes[index] + 1);
    }
}

} // namespace cg::ui
