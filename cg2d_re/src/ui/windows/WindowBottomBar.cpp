#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/ChatHistory.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowBottomBarElementId : ElementId {
    HandleButtons = 1,
    HandleButtons_2 = 2,
    HandleButtons_3 = 3,
    HandleButtons_4 = 4,
    HandleButtons_5 = 5,
    HandleButtons_6 = 6,
    HandleButtons_7 = 7,
    HandleButtons_8 = 8,
    HandleButtons_9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00473840。
static std::int32_t WindowBottomBar_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口6完整九分支按钮。

// HWT01：窗口6按原显示/复制阶段消费共享悬停文本，颜色只读低BYTE，尾部清槽并返S32 1。
// 原函数：0x004731E0。
static std::int32_t WindowBottomBar_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic55F4C0 {0x3B95F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F4C0。

static WindowGraphicData gWindowGraphic55F4D8 {0x3B962, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F4D8。

static WindowGraphicData gWindowGraphic55F4F0 {0x3B965, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F4F0。

static WindowGraphicData gWindowGraphic55F508 {0x3B968, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F508。

static WindowGraphicData gWindowGraphic55F520 {0x3B96B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F520。

static WindowGraphicData gWindowGraphic55F538 {0x3B96E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F538。

static WindowGraphicData gWindowGraphic55F550 {0x3B971, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F550。

static WindowGraphicData gWindowGraphic55F568 {0x18665, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F568。

static WindowGraphicData gWindowGraphic55F580 {0x3B95C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F580。

static WindowGraphicData gWindowGraphic55F598 {0x3B95D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F598。

static WindowTextPrototype gWindowText55F5B0 { // 原全局：0x0055F5B0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}},
    L"相关资讯",
};

static WindowTextPrototype gWindowText55F5B8 { // 原全局：0x0055F5B8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"--------",
};

WindowPrototype& gWindowPrototype54E330 = gWindowPrototypeBottomBar; // 原全局：0x0054E330。

static WindowElementPrototype gWindowElements55F5C0[14] { // 原全局：0x0055F5C0。
    {WindowElementType_EventArea, 0, 0, 640, 30, 1, {.unusedValue = 0}, &WindowBottomBar_HandleButtons, -1, static_cast<ElementId>(WindowBottomBarElementId::HandleButtons)},
    {WindowElementType_Graphic, 10, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F4C0}, &WindowBottomBar_HandleButtons, -1, static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_2)},
    {WindowElementType_Graphic, 86, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F4D8}, &WindowBottomBar_HandleButtons, -1, static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_3)},
    {WindowElementType_Graphic, 163, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F4F0}, &WindowBottomBar_HandleButtons, -1, static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_4)},
    {WindowElementType_Graphic, 239, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F508}, &WindowBottomBar_HandleButtons, -1, static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_5)},
    {WindowElementType_Graphic, 316, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F520}, &WindowBottomBar_HandleButtons, -1, static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_6)},
    {WindowElementType_Graphic, 392, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F538}, &WindowBottomBar_HandleButtons, -1, static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_7)},
    {WindowElementType_Graphic, 464, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F550}, &WindowBottomBar_HandleButtons, -1, static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_8)},
    {WindowElementType_Graphic, 555, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F568}, &WindowBottomBar_HandleButtons, -1, static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_9)},
    {WindowElementType_Text, 565, 11, 0, 0, 1, {.text = &gWindowText55F5B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBottomBarElementId::Element10)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F580}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBottomBarElementId::Element11)},
    {WindowElementType_Text, 21, 10, 0, 0, 1, {.text = &gWindowText55F5B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBottomBarElementId::Element12)},
    {WindowElementType_Text, 408, 10, 0, 0, 1, {.text = &gWindowText55F5B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBottomBarElementId::Element13)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F598}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBottomBarElementId::Element14)},
};

// 原型：窗口6；原全局：0x0054E330。
WindowPrototype gWindowPrototypeBottomBar {2, 5, 0, 452, 640, 30, 0x80010101u, 14, gWindowElements55F5C0, &WindowBottomBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 6: 54E330

// 窗口函数实现。

// 原函数：0x00473840。
static std::int32_t WindowBottomBar_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& interaction = callbackWindow.ownerSession->UiFrame().native.interaction;
    auto& session = *currentWindow->ownerSession;
    std::int32_t consumed = 0;
    switch (static_cast<WindowBottomBarElementId>(elementId)) {
    case WindowBottomBarElementId::HandleButtons:
        interaction.bottomHover = static_cast<std::uint8_t>(state) & MouseFlag_Hover;
        return 0;
    case WindowBottomBarElementId::HandleButtons_2: {
        auto* const handleButtons_2Element = currentWindow->FindElementById(WindowBottomBarElementId::HandleButtons_2);
        if (handleButtons_2Element) {
            auto* const graphic = handleButtons_2Element->data.graphic;
            graphic->imageNo = (state & MouseFlag_Hover) != 0 ? 0x3B960 : 0x3B95F;
            if ((state & MouseFlag_Hover) != 0) {
                consumed = 1;
            }
            WindowInstance* first = WindowRuntime_States(session)[WindowId_PlayerProfile].window;
            WindowInstance* second;
            WindowInstance* third;
            // 短路命中后重取9/8/7；未命中时三个局部均已由条件赋值。
            if (first != nullptr
                || (second = WindowRuntime_States(session)[WindowId_PlayerAttributes].window) != nullptr
                || WindowRuntime_States(session)[WindowId_OwnProfile].window != nullptr
                || (third = WindowRuntime_States(session)[WindowId_PlayerTitles].window) != nullptr) {
                graphic->imageNo = 0x3B95E;
                third = WindowRuntime_States(session)[WindowId_PlayerTitles].window;
                second = WindowRuntime_States(session)[WindowId_PlayerAttributes].window;
                first = WindowRuntime_States(session)[WindowId_PlayerProfile].window;
            }
            if ((state & MouseFlag_LDownEvent) == 0) {
                return consumed;
            }

            // 到达末尾发布前，各存活窗口路径均覆盖坐标；无窗口路径提前返回。
            std::int32_t savedX = 0;
            std::int32_t savedY = 0;
            if (first != nullptr || second != nullptr) {
                if (first != nullptr) {
                    first->state |= 0x8000u;
                    WindowInstance* const current = WindowRuntime_States(session)[WindowId_PlayerProfile].window;
                    savedX = current->x;
                    savedY = current->y;
                    static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
                    second = WindowRuntime_States(session)[WindowId_PlayerAttributes].window;
                }
                if (second != nullptr) {
                    second->state |= 0x8000u;
                    WindowInstance* const current = WindowRuntime_States(session)[WindowId_PlayerAttributes].window;
                    savedX = current->x;
                    savedY = current->y;
                    static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
                }
                third = WindowRuntime_States(session)[WindowId_PlayerTitles].window;
            } else if (WindowRuntime_States(session)[WindowId_OwnProfile].window != nullptr) {
                // 原47397A在仅58路径用完整state暂存坐标，后续关闭仍可覆盖。
                savedX = state;
                savedY = state;
            } else if (third == nullptr) {
                static_cast<void>(WindowRuntime_CreateAnimation(session, WindowId_PlayerProfile, 1, 0, nullptr));
                return 1;
            }
            if (third != nullptr) {
                third->state |= 0x8000u;
                WindowInstance* const current = WindowRuntime_States(session)[WindowId_PlayerTitles].window;
                savedX = current->x;
                savedY = current->y;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
            }
            WindowInstance* const last = WindowRuntime_States(session)[WindowId_OwnProfile].window;
            if (last != nullptr) {
                last->state |= 0x8000u;
                WindowInstance* const current = WindowRuntime_States(session)[WindowId_OwnProfile].window;
                savedX = current->x;
                savedY = current->y;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
            }
            WindowRuntime_States(session)[WindowId_PlayerProfile].y = savedY;
            WindowRuntime_States(session)[WindowId_PlayerAttributes].y = savedY;
            WindowRuntime_States(session)[WindowId_PlayerTitles].y = savedY;
            WindowRuntime_States(session)[WindowId_OwnProfile].y = savedY;
            WindowRuntime_States(session)[WindowId_PlayerProfile].x = savedX;
            WindowRuntime_States(session)[WindowId_PlayerAttributes].x = savedX;
            WindowRuntime_States(session)[WindowId_PlayerTitles].x = savedX;
            WindowRuntime_States(session)[WindowId_OwnProfile].x = savedX;
        }
        return 1;
    }
    case WindowBottomBarElementId::HandleButtons_3: {
        auto* const handleButtons_3Element = currentWindow->FindElementById(WindowBottomBarElementId::HandleButtons_3);
        if (!handleButtons_3Element) { return 0; }
        auto* const graphic = handleButtons_3Element->data.graphic;
        graphic->imageNo = (state & MouseFlag_Hover) != 0 ? 0x3B963 : 0x3B962;
        if ((state & MouseFlag_Hover) != 0) {
            consumed = 1;
        }
        if (callbackWindow.ownerSession->WorldScene().sceneType != 10) {
            if (WindowRuntime_States(session)[WindowId_Skill].window != nullptr) {
                graphic->imageNo = 0x3B961;
            }
        } else {
            switch (WindowBattleCommand_CanSelectSkill(session)) {
            case 0: graphic->imageNo = 0x3B961; break;
            case 1: graphic->imageNo = 0x3B962; break;
            case 2: graphic->imageNo = 0x3B963; break;
            default: break;
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (callbackWindow.ownerSession->WorldScene().sceneType != 10) {
                WindowInstance* const window = WindowRuntime_States(session)[WindowId_Skill].window;
                if (window != nullptr) {
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
                } else {
                    static_cast<void>(WindowSkill_Open(session));
                }
            } else {
                WindowBattleCommand_OpenSkills(session);
            }
            return 1;
        }
        return consumed;
    }
    case WindowBottomBarElementId::HandleButtons_4: {
        auto* const handleButtons_4Element = currentWindow->FindElementById(WindowBottomBarElementId::HandleButtons_4);
        if (!handleButtons_4Element) { return 0; }
        auto* const graphic = handleButtons_4Element->data.graphic;
        graphic->imageNo = (state & MouseFlag_Hover) != 0 ? 0x3B966 : 0x3B965;
        if ((state & MouseFlag_Hover) != 0) {
            consumed = 1;
        }
        if (callbackWindow.ownerSession->WorldScene().sceneType != 10) {
            if (WindowRuntime_States(session)[WindowId_Inventory].window != nullptr) {
                graphic->imageNo = 0x3B964;
            }
        } else {
            switch (WindowBattleCommand_CanSelectItem(session)) {
            case 0: graphic->imageNo = 0x3B964; break;
            case 1: graphic->imageNo = 0x3B965; break;
            case 2: graphic->imageNo = 0x3B966; break;
            default: break;
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (callbackWindow.ownerSession->WorldScene().sceneType != 10) {
                WindowInstance* const window = WindowRuntime_States(session)[WindowId_Inventory].window;
                if (window != nullptr) {
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
                } else {
                    static_cast<void>(WindowRuntime_CreateAnimation(session, WindowId_Inventory, 1, 0, nullptr));
                }
            } else {
                WindowBattleCommand_OpenItems(session);
            }
            return 1;
        }
        return consumed;
    }
    case WindowBottomBarElementId::HandleButtons_5: {
        auto* const handleButtons_5Element = currentWindow->FindElementById(WindowBottomBarElementId::HandleButtons_5);
        if (!handleButtons_5Element) { return 0; }
        auto* const graphic = handleButtons_5Element->data.graphic;
        graphic->imageNo = (state & MouseFlag_Hover) != 0 ? 0x3B969 : 0x3B968;
        if ((state & MouseFlag_Hover) != 0) {
            consumed = 1;
        }
        if (callbackWindow.ownerSession->WorldScene().sceneType != 10) {
            if (WindowRuntime_States(session)[WindowId_PetList].window != nullptr) {
                graphic->imageNo = 0x3B967;
            }
        } else {
            switch (WindowBattleCommand_CanSelectPet(session)) {
            case 0: graphic->imageNo = 0x3B967; break;
            case 1: graphic->imageNo = 0x3B968; break;
            case 2: graphic->imageNo = 0x3B969; break;
            default: break;
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (callbackWindow.ownerSession->WorldScene().sceneType != 10) {
                WindowInstance* const window = WindowRuntime_States(session)[WindowId_PetList].window;
                if (window != nullptr) {
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
                } else {
                    static_cast<void>(WindowRuntime_CreateAnimation(session, WindowId_PetList, 1, 0, nullptr));
                }
            } else {
                WindowBattleCommand_OpenPets(session);
            }
            return 1;
        }
        return consumed;
    }
    case WindowBottomBarElementId::HandleButtons_6:
    case WindowBottomBarElementId::HandleButtons_7:
    case WindowBottomBarElementId::HandleButtons_8: {
        const std::int32_t windowId = elementId == static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_6) ? 21 : (elementId == static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_7) ? 19 : 24);
        const std::int32_t normalImage = 0x3B96B + (index - 5) * 3;
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            graphic->imageNo = (state & MouseFlag_Hover) != 0 ? normalImage + 1 : normalImage;
            if ((state & MouseFlag_Hover) != 0) {
                consumed = 1;
            }
            WindowInstance* window = WindowRuntime_States(session)[windowId].window;
            if (window != nullptr) {
                graphic->imageNo = normalImage - 1;
                window = WindowRuntime_States(session)[windowId].window;
            }
            if ((state & MouseFlag_LDownEvent) != 0) {
                if (window == nullptr) {
                    static_cast<void>(WindowRuntime_CreateAnimation(session, windowId, 1, 0, nullptr));
                } else {
                    window->state |= 0x8000u;
                    if (elementId != static_cast<ElementId>(WindowBottomBarElementId::HandleButtons_8)) {
                        static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
                    }
                }
                return 1;
            }
        }
        return consumed;
    }
    case WindowBottomBarElementId::HandleButtons_9: {
        auto* const handleButtons_9Element = currentWindow->FindElementById(WindowBottomBarElementId::HandleButtons_9);
        if (handleButtons_9Element) {
            auto* const graphic = handleButtons_9Element->data.graphic;
            graphic->imageNo = 0x18666;
        }
        if ((state & MouseFlag_Hover) != 0) {
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::uint64_t now = platform::ReadSystemSeconds_4589D0();
            const std::uint64_t previous = interaction.bottomRequestTime;
            const std::int64_t elapsed = std::bit_cast<std::int64_t>(now - previous);
            if (elapsed >= 30) {
                static_cast<void>(session.Connection().SendIuEPacket());
                interaction.bottomRequestTime = platform::ReadSystemSeconds_4589D0();
            } else {
                world::AppendChatMessage_44B050(*callbackWindow.ownerSession, L"请等待30秒后再操作", 4, 3);
            }
            return 1;
        }
        return consumed;
    }
    default:
        return 0;
    }
}

// 原4731E0：窗口6主更新；忽略参数，复制描述后仅清共享缓冲首字节。
// HWT01：窗口6按原显示/复制阶段消费共享悬停文本，颜色只读低BYTE，尾部清槽并返S32 1。
// 原函数：0x004731E0。
static std::int32_t WindowBottomBar_Update(WindowInstance& callbackWindow, const std::int32_t)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& interaction = callbackWindow.ownerSession->UiFrame().native.interaction;
    if (interaction.bottomHover == 1 || uiFrame.hoverText.empty()) {
        for (std::int32_t index = 1; index <= 7; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 1;
        }
        if (auto* const handleButtons_9Element = currentWindow->FindElementById(WindowBottomBarElementId::HandleButtons_9); handleButtons_9Element) handleButtons_9Element->visible = 0;
        if (auto* const element11 = currentWindow->FindElementById(WindowBottomBarElementId::Element11); element11) element11->visible = 1;
        if (auto* const element12 = currentWindow->FindElementById(WindowBottomBarElementId::Element12); element12) element12->visible = 0;
        if (auto* const element13 = currentWindow->FindElementById(WindowBottomBarElementId::Element13); element13) element13->visible = 0;
    } else {
        if (uiFrame.hoverText.find(L"[han]") != std::wstring::npos) {
            if (!uiFrame.hoverText.empty()) {
                uiFrame.hoverText.resize(uiFrame.hoverText.size() - 5);
            }
            interaction.hoverObjectValue = 1;
        } else if (uiFrame.hoverText.find(L"[kok]") != std::wstring::npos) {
            if (!uiFrame.hoverText.empty()) {
                uiFrame.hoverText.resize(uiFrame.hoverText.size() - 5);
            }
            interaction.hoverObjectValue = 5;
        }

        auto* mainElementsWindow = currentWindow;
        auto* const element12 = mainElementsWindow->FindElementById(WindowBottomBarElementId::Element12);
        if (element12) {
            auto* const mainText = element12->data.text;
            mainText->text = uiFrame.hoverText;
            mainText->style.color = *reinterpret_cast<const std::byte*>(&interaction.hoverObjectValue);
        }
        if (interaction.hoverTitle.size() != 0) {
            if (auto* const element13 = currentWindow->FindElementById(WindowBottomBarElementId::Element13); element13) element13->visible = 1;
        }
        auto* titleElementsWindow = currentWindow;
        auto* const element13 = titleElementsWindow->FindElementById(WindowBottomBarElementId::Element13);
        if (element13) {
            auto* const titleText = element13->data.text;
            titleText->text = interaction.hoverTitle;
            titleText->style.color = *reinterpret_cast<const std::byte*>(&interaction.hoverTitleValue);
        }
        for (std::int32_t index = 1; index <= 7; ++index) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->visible = 0;
        }
        if (auto* const handleButtons_9Element = currentWindow->FindElementById(WindowBottomBarElementId::HandleButtons_9); handleButtons_9Element) handleButtons_9Element->visible = 0;
        if (auto* const element11 = currentWindow->FindElementById(WindowBottomBarElementId::Element11); element11) element11->visible = 0;
        if (auto* const element122 = currentWindow->FindElementById(WindowBottomBarElementId::Element12); element122) element122->visible = 1;
    }

    const std::wstring_view description = platform::QueryWindowImeDescription_4734F0(*callbackWindow.ownerSession);
    auto* elementsWindow = currentWindow;
    auto* const element10 = elementsWindow->FindElementById(WindowBottomBarElementId::Element10);
    if (element10) {
        auto* const text = element10->data.text;
        text->text = description;
    }
    uiFrame.hoverText.clear();
    interaction.hoverObjectValue = 0;
    interaction.hoverTitle.clear();
    interaction.hoverTitleValue = 0;
    return 1;
}

} // namespace detail

} // namespace cg::ui
