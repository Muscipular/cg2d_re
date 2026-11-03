#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0049D8F0。
static void WindowProfile_RouteReply(
    std::int32_t type, std::int32_t value, std::int32_t request,
    std::int32_t target, char* text); // 先发布同源对话状态，再生产并打开60/61/62。

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00CC4D34。 4D3020保存x/y数值，非CryptInt对象。
static std::int32_t gWindowInteractionOriginX_CC4D34 = 0; // 原全局：0x00CC4D34。

static std::int32_t gWindowInteractionOriginY_CC4D3C = 0; // 原全局：0x00CC4D3C。

// 窗口函数实现。

// 原函数：0x00478330。
std::int32_t WindowProfile_HandleButtons(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    switch (elementId) {
    case NativeElementKey(17): {
        const bool visible = world::detail::SelectedProfileValue04() != 0;
        if ((state & MouseFlag_Hover) != 0) {
            gHoverTextD68AE8 = visible
                ? L"不公开玩家简介"
                : L"公开玩家简介";
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(cg::network::GetGameConnection().SetProfileVisibilityRequest(visible ? 0 : 1));
            if (!visible) {
                static_cast<void>(world::detail::PublishPlayerProfile_533C80(1));
            }
            world::detail::SelectedProfileValue04() = visible ? 0 : 1;
        }
        return consumed;
    }
    case NativeElementKey(19):
    case NativeElementKey(20):
    case NativeElementKey(21): {
        constexpr std::array<std::int32_t, 3> images {244405, 243075, 244408};
        constexpr const wchar_t* hints[] {
            L"切换成玩家状态视窗。",
            L"切换成玩家明细视窗。",
            L"切换成玩家称号视窗。",
        };
        const std::int32_t position = index - 19;
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = images[position] + 2;
                gHoverTextD68AE8 = hints[position];
                consumed = 1;
            } else {
                graphic->imageNo = images[position];
            }
        }
        const std::int32_t target = index - 12;
        if ((state & MouseFlag_LDownEvent) != 0 && WindowRuntime_States()[target].window == nullptr) {
            currentWindow->state |= 0x8000u;
            static_cast<void>(WindowRuntime_Open(target));
            // 原三切页每轴先取当前/目标指针，再读坐标；见CAC10。
            WindowInstance* const sourceForX = currentWindow;
            WindowInstance* const targetForX = WindowRuntime_States()[target].window;
            const std::int32_t x = sourceForX->x;
            targetForX->x = x;
            WindowInstance* const sourceForY = currentWindow;
            WindowInstance* const targetForY = WindowRuntime_States()[target].window;
            const std::int32_t y = sourceForY->y;
            targetForY->y = y;
            static_cast<void>(audio::PlaySe_52C450(56, 320));
            WindowRuntime_QueuePagePresentation(*currentWindow, *targetForY);
            return 1;
        }
        return consumed;
    }
    case NativeElementKey(23):
    case NativeElementKey(24):
    case NativeElementKey(25): {
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = 243035;
                gHoverTextD68AE8 = L"开启目录视窗";
                consumed = 1;
            } else {
                graphic->imageNo = 243033;
            }
        }
        const std::int32_t category = index - 23;
        if ((state & MouseFlag_LDownEvent) != 0) {
            // 原检查窗口9，却写窗口59记录flags；第二检查仅是固定全局地址非零。
            if (WindowRuntime_States()[WindowId_PlayerTitles].window != nullptr) {
                WindowRuntime_States()[WindowId_OwnProfileCategory].flags |= 0x8000u;
            }
            gProfileCategory1085DA0 = category;
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_OwnProfileCategory, 1, 0, nullptr));
            static_cast<void>(audio::PlaySe_52C450(56, 320));
            consumed = 1;
        }
        if (WindowRuntime_States()[WindowId_OwnProfileCategory].window != nullptr && gProfileCategory1085DA0 == category) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 243034;
        }
        return consumed;
    }
    case NativeElementKey(29): {
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = 244420;
                gHoverTextD68AE8 = L"决定玩家简介";
                consumed = 1;
            } else {
                graphic->imageNo = 244418;
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(WindowChat_RestoreInput());
            static_cast<void>(world::detail::PublishPlayerProfile_533C80(0));
            return 1;
        }
        return consumed;
    }
    default:
        return 0;
    }
}

// 原函数：0x004D3020。
std::int32_t WindowProfile_SaveOrigin(const std::int32_t x, const std::int32_t y)
{
    gWindowInteractionOriginX_CC4D34 = x;
    gWindowInteractionOriginY_CC4D3C = y;
    return x;
}

// 原函数：0x004D3040。
std::int32_t WindowProfile_CheckDistance(const std::int32_t rangeX, const std::int32_t rangeY)
{
    const double playerX = static_cast<std::int32_t>(world::gPlayerX) * 64.0;
    if (playerX != std::bit_cast<float>(
            static_cast<std::uint32_t>(world::gWorldCurrentXBits))) {
        return 0;
    }
    const double playerY = static_cast<std::int32_t>(world::gPlayerY) * 64.0;
    if (playerY != std::bit_cast<float>(
            static_cast<std::uint32_t>(world::gWorldCurrentYBits))) {
        return 0;
    }
    double distanceX = static_cast<double>(gWindowInteractionOriginX_CC4D34 * 64)
        - std::bit_cast<float>(
            static_cast<std::uint32_t>(world::gWorldCurrentXBits));
    if (distanceX < 0.0) {
        distanceX = -distanceX;
    }
    if (distanceX >= rangeX * 64) {
        return 1;
    }
    double distanceY = static_cast<double>(gWindowInteractionOriginY_CC4D3C * 64)
        - std::bit_cast<float>(
            static_cast<std::uint32_t>(world::gWorldCurrentYBits));
    if (distanceY < 0.0) {
        distanceY = -distanceY;
    }
    return distanceY >= rangeY * 64 ? 1 : 0;
}

// 原函数：0x0049D8F0。
static void WindowProfile_RouteReply(
    const std::int32_t type, const std::int32_t value, const std::int32_t request,
    const std::int32_t target, char* const text)
{
    if (world::gCurrentDialogType_581800 >= 0) {
        switch (request) {
        case 326:
        case 327:
        case 369:
        case 388:
        case 389:
        case 390:
            if (type != 32) {
                WindowRuntime_CloseByCategory(2);
                WindowRuntime_CloseByCategory(3);
            }
            break;
        default:
            break;
        }
    }
    world::gCurrentDialogType_581800 = type;
    gDialogValueCF4E5C = value;
    world::gDialogRequestCC21E0 = request;
    world::gDialogTargetD2DE6C = target;
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    switch (type) {
    case 38:
        world::detail::HandleProfileCategorySelectionReply_4C9E40(value, text);
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_ProfileCategory, 1, 0, nullptr));
        return;
    case 39:
        world::detail::HandleProfileSearchResultsReply_4CA330(value, text);
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_ProfileSearch, 1, 0, nullptr));
        return;
    case 40:
        static_cast<void>(world::detail::HandleViewedPlayerProfileReply_4CAC60(value, text));
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_ProfileView, 1, 0, nullptr));
        return;
    default:
        return;
    }
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x0050F990。
void WindowProfile_ReceiveReply(cg::network::CgConnection&, const std::int32_t type, const std::int32_t value,
    const std::int32_t request, const std::int32_t target, char* const text)
{
    if (world::gGameplayRepliesSuppressed_9A6E0C != 0) {
        return;
    }
    if (world::gLogoutToTownPending_D67E78 != 0) {
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    detail::WindowProfile_RouteReply(type, value, request, target, text);
}

} // namespace cg::ui
