#include "WindowRuntimeState.hpp"

#include "../world/WorldUpdateDependencies.hpp"
#include "cg/Input.hpp"
#include "cg/YobiRuntime.hpp"

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

namespace {

// 原54EFE8的71项S8类别，与窗口编号一一对应。
constexpr std::array<std::int8_t, kWindowCount> kWindowCategories {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 4, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 4, 4, 0, 1, 2, 2, 0, 1, 2, 4, 2, 2, 2, 2, 2,
    1, 1, 2, 4, 2, 2, 2, 1, 2, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 2, 2, 2,
};

} // namespace

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

std::uint8_t ReadValueD67BE0() noexcept
{
    return detail::gValueD67BE0;
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

} // namespace cg::ui
