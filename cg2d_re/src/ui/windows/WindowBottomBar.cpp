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
static std::int32_t WindowBottomBar_HandleButtons(std::int32_t index, std::int32_t state); // 窗口6完整九分支按钮。

// HWT01：窗口6按原显示/复制阶段消费共享悬停文本，颜色只读低BYTE，尾部清槽并返S32 1。
// 原函数：0x004731E0。
static std::int32_t WindowBottomBar_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

std::int32_t gHoverObjectValueD68AC8 = 0; // 原全局：0x00D68AC8。

std::wstring gHoverTitleD68BF4 {}; // 原全局：0x00D68BF4。

std::int32_t gHoverTitleValueD69748 = 0; // 原全局：0x00D69748。

// 原全局：0x00D2EA70。 473840的case0写U8(state)&1，4731E0严格判1。
static std::int32_t gWindow6HoverD2EA70 = 0; // 原全局：0x00D2EA70。

static std::uint64_t gWindow6RequestTimeD67AF0 = 0; // 原全局：0x00D67AF0。 CTM01保持完整64位秒差作30秒门禁。

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
static std::int32_t WindowBottomBar_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    switch (index) {
    case 0:
        gWindow6HoverD2EA70 = static_cast<std::uint8_t>(state) & 1u;
        return 0;
    case 1: {
        if (!((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_2]))) { return {}; }
        auto* const graphic = (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_2]->data.graphic;
        graphic->imageNo = (state & 1) != 0 ? 0x3B960 : 0x3B95F;
        if ((state & 1) != 0) {
            consumed = 1;
        }
        WindowInstance* first = gWindowStates[WindowId_PlayerProfile].window;
        WindowInstance* second;
        WindowInstance* third;
        // 短路命中后重取9/8/7；未命中时三个局部均已由条件赋值。
        if (first != nullptr
            || (second = gWindowStates[WindowId_PlayerAttributes].window) != nullptr
            || gWindowStates[WindowId_OwnProfile].window != nullptr
            || (third = gWindowStates[WindowId_PlayerTitles].window) != nullptr) {
            graphic->imageNo = 0x3B95E;
            third = gWindowStates[WindowId_PlayerTitles].window;
            second = gWindowStates[WindowId_PlayerAttributes].window;
            first = gWindowStates[WindowId_PlayerProfile].window;
        }
        if ((state & 2) == 0) {
            return consumed;
        }

        // 到达末尾发布前，各存活窗口路径均覆盖坐标；无窗口路径提前返回。
        std::int32_t savedX = 0;
        std::int32_t savedY = 0;
        if (first != nullptr || second != nullptr) {
            if (first != nullptr) {
                first->state |= 0x8000u;
                WindowInstance* const current = gWindowStates[WindowId_PlayerProfile].window;
                savedX = current->x;
                savedY = current->y;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
                second = gWindowStates[WindowId_PlayerAttributes].window;
            }
            if (second != nullptr) {
                second->state |= 0x8000u;
                WindowInstance* const current = gWindowStates[WindowId_PlayerAttributes].window;
                savedX = current->x;
                savedY = current->y;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
            }
            third = gWindowStates[WindowId_PlayerTitles].window;
        } else if (gWindowStates[WindowId_OwnProfile].window != nullptr) {
            // 原47397A在仅58路径用完整state暂存坐标，后续关闭仍可覆盖。
            savedX = state;
            savedY = state;
        } else if (third == nullptr) {
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PlayerProfile, 1, 0, nullptr));
            return 1;
        }
        if (third != nullptr) {
            third->state |= 0x8000u;
            WindowInstance* const current = gWindowStates[WindowId_PlayerTitles].window;
            savedX = current->x;
            savedY = current->y;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
        WindowInstance* const last = gWindowStates[WindowId_OwnProfile].window;
        if (last != nullptr) {
            last->state |= 0x8000u;
            WindowInstance* const current = gWindowStates[WindowId_OwnProfile].window;
            savedX = current->x;
            savedY = current->y;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
        gWindowStates[WindowId_PlayerProfile].y = savedY;
        gWindowStates[WindowId_PlayerAttributes].y = savedY;
        gWindowStates[WindowId_PlayerTitles].y = savedY;
        gWindowStates[WindowId_OwnProfile].y = savedY;
        gWindowStates[WindowId_PlayerProfile].x = savedX;
        gWindowStates[WindowId_PlayerAttributes].x = savedX;
        gWindowStates[WindowId_PlayerTitles].x = savedX;
        gWindowStates[WindowId_OwnProfile].x = savedX;
        return 1;
    }
    case 2: {
        if (!((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_3]))) { return {}; }
        auto* const graphic = (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_3]->data.graphic;
        graphic->imageNo = (state & 1) != 0 ? 0x3B963 : 0x3B962;
        if ((state & 1) != 0) {
            consumed = 1;
        }
        if (world::gCurrentSceneType != 10) {
            if (gWindowStates[WindowId_Skill].window != nullptr) {
                graphic->imageNo = 0x3B961;
            }
        } else {
            switch (WindowBattleCommand_CanSelectSkill()) {
            case 0: graphic->imageNo = 0x3B961; break;
            case 1: graphic->imageNo = 0x3B962; break;
            case 2: graphic->imageNo = 0x3B963; break;
            default: break;
            }
        }
        if ((state & 2) != 0) {
            if (world::gCurrentSceneType != 10) {
                WindowInstance* const window = gWindowStates[WindowId_Skill].window;
                if (window != nullptr) {
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                } else {
                    static_cast<void>(WindowSkill_Open());
                }
            } else {
                WindowBattleCommand_OpenSkills();
            }
            return 1;
        }
        return consumed;
    }
    case 3: {
        if (!((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_4]))) { return {}; }
        auto* const graphic = (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_4]->data.graphic;
        graphic->imageNo = (state & 1) != 0 ? 0x3B966 : 0x3B965;
        if ((state & 1) != 0) {
            consumed = 1;
        }
        if (world::gCurrentSceneType != 10) {
            if (gWindowStates[WindowId_Inventory].window != nullptr) {
                graphic->imageNo = 0x3B964;
            }
        } else {
            switch (WindowBattleCommand_CanSelectItem()) {
            case 0: graphic->imageNo = 0x3B964; break;
            case 1: graphic->imageNo = 0x3B965; break;
            case 2: graphic->imageNo = 0x3B966; break;
            default: break;
            }
        }
        if ((state & 2) != 0) {
            if (world::gCurrentSceneType != 10) {
                WindowInstance* const window = gWindowStates[WindowId_Inventory].window;
                if (window != nullptr) {
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                } else {
                    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Inventory, 1, 0, nullptr));
                }
            } else {
                WindowBattleCommand_OpenItems();
            }
            return 1;
        }
        return consumed;
    }
    case 4: {
        if (!((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_5]))) { return {}; }
        auto* const graphic = (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_5]->data.graphic;
        graphic->imageNo = (state & 1) != 0 ? 0x3B969 : 0x3B968;
        if ((state & 1) != 0) {
            consumed = 1;
        }
        if (world::gCurrentSceneType != 10) {
            if (gWindowStates[WindowId_PetList].window != nullptr) {
                graphic->imageNo = 0x3B967;
            }
        } else {
            switch (WindowBattleCommand_CanSelectPet()) {
            case 0: graphic->imageNo = 0x3B967; break;
            case 1: graphic->imageNo = 0x3B968; break;
            case 2: graphic->imageNo = 0x3B969; break;
            default: break;
            }
        }
        if ((state & 2) != 0) {
            if (world::gCurrentSceneType != 10) {
                WindowInstance* const window = gWindowStates[WindowId_PetList].window;
                if (window != nullptr) {
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                } else {
                    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PetList, 1, 0, nullptr));
                }
            } else {
                WindowBattleCommand_OpenPets();
            }
            return 1;
        }
        return consumed;
    }
    case 5:
    case 6:
    case 7: {
        const std::int32_t windowId = index == 5 ? 21 : (index == 6 ? 19 : 24);
        const std::int32_t normalImage = 0x3B96B + (index - 5) * 3;
        if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
        auto* const graphic = (*gCurrentWindow)[NativeElementKey(index)]->data.graphic;
        graphic->imageNo = (state & 1) != 0 ? normalImage + 1 : normalImage;
        if ((state & 1) != 0) {
            consumed = 1;
        }
        WindowInstance* window = gWindowStates[windowId].window;
        if (window != nullptr) {
            graphic->imageNo = normalImage - 1;
            window = gWindowStates[windowId].window;
        }
        if ((state & 2) != 0) {
            if (window == nullptr) {
                static_cast<void>(WindowRuntime_CreateAnimation(windowId, 1, 0, nullptr));
            } else {
                window->state |= 0x8000u;
                if (index != 7) {
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
            }
            return 1;
        }
        return consumed;
    }
    case 8: {
        if (!((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_9]))) { return {}; }
        auto* const graphic = (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_9]->data.graphic;
        graphic->imageNo = 0x18666;
        if ((state & 1) != 0) {
            consumed = 1;
        }
        if ((state & 2) != 0) {
            const std::uint64_t now = platform::ReadSystemSeconds_4589D0();
            const std::uint64_t previous = gWindow6RequestTimeD67AF0;
            const std::int64_t elapsed = std::bit_cast<std::int64_t>(now - previous);
            if (elapsed >= 30) {
                static_cast<void>(network::SendIuEPacket_516D70(platform::gConnectionSocket));
                gWindow6RequestTimeD67AF0 = platform::ReadSystemSeconds_4589D0();
            } else {
                world::AppendChatMessage_44B050(L"请等待30秒后再操作", 4, 3);
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
static std::int32_t WindowBottomBar_Update(const std::int32_t)
{
    if (gWindow6HoverD2EA70 == 1 || gHoverTextD68AE8.empty()) {
        for (std::int32_t index = 1; index <= 7; ++index) {
            if ((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)])) (*gCurrentWindow)[NativeElementKey(index)]->visible = 1;
        }
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_9])) (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_9]->visible = 0;
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::Element11])) (*gCurrentWindow)[WindowBottomBarElementId::Element11]->visible = 1;
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::Element12])) (*gCurrentWindow)[WindowBottomBarElementId::Element12]->visible = 0;
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::Element13])) (*gCurrentWindow)[WindowBottomBarElementId::Element13]->visible = 0;
    } else {
        if (gHoverTextD68AE8.find(L"[han]") != std::wstring::npos) {
            if (!gHoverTextD68AE8.empty()) {
                gHoverTextD68AE8.resize(gHoverTextD68AE8.size() - 5);
            }
            gHoverObjectValueD68AC8 = 1;
        } else if (gHoverTextD68AE8.find(L"[kok]") != std::wstring::npos) {
            if (!gHoverTextD68AE8.empty()) {
                gHoverTextD68AE8.resize(gHoverTextD68AE8.size() - 5);
            }
            gHoverObjectValueD68AC8 = 5;
        }

        auto* mainElementsWindow = gCurrentWindow;
        if (!((mainElementsWindow && (*mainElementsWindow)[WindowBottomBarElementId::Element12]))) { return {}; }
        auto* const mainText = (*mainElementsWindow)[WindowBottomBarElementId::Element12]->data.text;
        mainText->text = gHoverTextD68AE8;
        mainText->style.color = *reinterpret_cast<const std::byte*>(&gHoverObjectValueD68AC8);
        if (gHoverTitleD68BF4.size() != 0) {
            if ((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::Element13])) (*gCurrentWindow)[WindowBottomBarElementId::Element13]->visible = 1;
        }
        auto* titleElementsWindow = gCurrentWindow;
        if (!((titleElementsWindow && (*titleElementsWindow)[WindowBottomBarElementId::Element13]))) { return {}; }
        auto* const titleText = (*titleElementsWindow)[WindowBottomBarElementId::Element13]->data.text;
        titleText->text = gHoverTitleD68BF4;
        titleText->style.color = *reinterpret_cast<const std::byte*>(&gHoverTitleValueD69748);
        for (std::int32_t index = 1; index <= 7; ++index) {
            if ((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)])) (*gCurrentWindow)[NativeElementKey(index)]->visible = 0;
        }
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_9])) (*gCurrentWindow)[WindowBottomBarElementId::HandleButtons_9]->visible = 0;
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::Element11])) (*gCurrentWindow)[WindowBottomBarElementId::Element11]->visible = 0;
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBottomBarElementId::Element12])) (*gCurrentWindow)[WindowBottomBarElementId::Element12]->visible = 1;
    }

    const std::wstring_view description = platform::QueryWindowImeDescription_4734F0();
    auto* elementsWindow = gCurrentWindow;
    if (!((elementsWindow && (*elementsWindow)[WindowBottomBarElementId::Element10]))) { return {}; }
    auto* const text = (*elementsWindow)[WindowBottomBarElementId::Element10]->data.text;
    text->text = description;
    gHoverTextD68AE8.clear();
    gHoverObjectValueD68AC8 = 0;
    gHoverTitleD68BF4.clear();
    gHoverTitleValueD69748 = 0;
    return 1;
}

} // namespace detail

} // namespace cg::ui
