#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::input {

// 控件载荷、窗口状态、组件表与原型。

InputContext gInputContextD2DC10 {}; // 原全局：0x00D2DC10。

} // namespace cg::input

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00477540。
static std::int32_t WindowPlayerTitles_EditDraft(std::int32_t unusedIndex, std::int32_t state); // 草稿编辑焦点。

// 原函数：0x00477600。
static std::int32_t WindowPlayerTitles_SelectTitle(std::int32_t index, std::int32_t state); // 六行称号选择/删除。

// 原函数：0x00477710。
static std::int32_t WindowPlayerTitles_HandleButtons(std::int32_t index, std::int32_t state); // 完整按钮分派与背景清模式。

// 原函数：0x004782A0。
static std::int32_t WindowPlayerTitles_Scroll(std::int32_t unusedIndex, std::int32_t state); // 称号滚轮，恒返0。

// 原函数：0x004782E0。
static std::int32_t WindowPlayerTitles_DragScroll(std::int32_t index, std::int32_t state); // WSP01：原称号拖动页码；旧X代码1959。

// 原函数：0x004744A0。
static void WindowPlayerTitles_Release();

// 原函数：0x004774C0。
// 关联原入口：0x0047752C（共享尾已并入本函数正文）。
static std::int32_t WindowPlayerTitles_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00D45FC0。 窗口9草稿，区别于玩家ownTitle和原型内嵌文本。
static std::wstring gTitleDraftD45FC0 {}; // 原全局：0x00D45FC0。

// 原全局：0x0057CCB8。 原型当前context为编辑正文来源。
static WindowInputPrototype gTitleInputPrototype57CCB8 { // 原全局：0x0057CCB8。
    {{&input::gInputContextD2DC10}, WindowInputReferenceKind::Context},
    105, 31, 3, 12, 6, {}, 1, 16, 0, 0,
};

static WindowGraphicData gWindowGraphic560DA0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DA0。

static WindowGraphicData gWindowGraphic560DB8 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DB8。

static WindowGraphicData gWindowGraphic560DD0 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DD0。

static WindowGraphicData gWindowGraphic560DE8 {0x3BAB9, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DE8。

static WindowGraphicData gWindowGraphic560E00 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E00。

static WindowGraphicData gWindowGraphic560E18 {0x3B574, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E18。

static WindowGraphicData gWindowGraphic560E30 {0x3B577, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E30。

static WindowGraphicData gWindowGraphic560E48 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E48。

static WindowGraphicData gWindowGraphic560E60 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E60。

static WindowGraphicData gWindowGraphic560E78 {0x3B59D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E78。

static WindowGraphicData gWindowGraphic560E90 {0x3BAB3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E90。

static WindowGraphicData gWindowGraphic560EA8 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560EA8。

static WindowGraphicData gWindowGraphic560EC0 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x00560EC0。

static WindowGraphicData gWindowGraphic560ED8 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560ED8。

static WindowGraphicData gWindowGraphic560EF0 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560EF0。

static WindowGraphicData gWindowGraphic560F08 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560F08。

static WindowGraphicData gWindowGraphic560F20 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560F20。

static WindowTextPrototype gWindowText560F38 { // 原全局：0x00560F38。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};

static WindowTextPrototype gWindowText560F40 { // 原全局：0x00560F40。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00586228。
};

static WindowTextPrototype gWindowText560F48 { // 原全局：0x00560F48。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"稱號", // 原常量参考：0x005860D0。
};

static std::uint32_t gWindowMouseFlagsD676F0 = 0; // 原全局：0x00D676F0。

static std::uint32_t gWindowMouseFlagsD6772C = 0; // 原全局：0x00D6772C。

static WindowElementPrototype gWindowElements560F50[37] { // 原全局：0x00560F50。
    {WindowElementType_Input, 105, 31, 144, 12, 1, {.unusedValue = 0}, &WindowPlayerTitles_EditDraft},
    {WindowElementType_Graphic, 239, 104, 0, 14, 1, {.graphic = &gWindowGraphic560ED8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 239, 104, 11, 72, 1, {.mouseFlags = &gWindowMouseFlagsD676F0}, &WindowPlayerTitles_DragScroll},
    {WindowElementType_Graphic, 239, 93, 11, 11, 1, {.graphic = &gWindowGraphic560EF0}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 239, 175, 11, 11, 1, {.graphic = &gWindowGraphic560F08}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_EventArea, 0, 0, 315, 261, 1, {.unusedValue = 0}, &WindowPlayerTitles_Scroll},
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText560F38}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText560F40}, &WindowElement_NoOp},
    {WindowElementType_Text, 105, 31, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 105, 64, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 94, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 110, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 126, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 142, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 158, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 174, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic560DA0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic560DB8}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic560DD0}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic560DE8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic560F20}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 260, 27, 49, 17, 1, {.graphic = &gWindowGraphic560E00}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 260, 61, 49, 17, 1, {.graphic = &gWindowGraphic560E18}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 260, 175, 49, 17, 1, {.graphic = &gWindowGraphic560E30}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 220, 203, 18, 18, 1, {.graphic = &gWindowGraphic560E48}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 199, 203, 18, 18, 1, {.graphic = &gWindowGraphic560E60}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 23, 92, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 108, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 124, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 140, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 156, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 172, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic560E90}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic560EA8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic560EC0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 0, 0, 374, 281, 1, {.mouseFlags = &gWindowMouseFlagsD6772C}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口9；原全局：0x0054E3C0。
WindowPrototype gWindowPrototypePlayerTitles {0, 4, 15, 97, 315, 261, 0x80000080u, 37, gWindowElements560F50, &WindowPlayerTitles_Update, &WindowRuntime_DrawDefault, &WindowPlayerTitles_Release}; // 9: 54E3C0

// 窗口函数实现。

// 原函数：0x00477540。
static std::int32_t WindowPlayerTitles_EditDraft(const std::int32_t, const std::int32_t state)
{
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"設定自己的稱號。" /* 原常量参考：0x005863C4。 */;
    }
    if ((state & 2) == 0) {
        return 0;
    }
    WindowInstance* const window = gCurrentWindow;
    if (gFocusedElementDataD45F94 != window->elements[0].data) {
        gTitleInputPrototype57CCB8.text = gTitleDraftD45FC0;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gTitleInputPrototype57CCB8, x, y, 0));
        input::SetContext_44B330(&input::gInputContextD2DC10);
        WindowInstance* const focused = gCurrentWindow;
        gFocusedElementDataD45F94 = focused->elements[0].data;
        static_cast<WindowInputData*>(focused->elements[0].data)->context =
            &input::gInputContextD2DC10;
    }
    return 1;
}

// 原函数：0x00477600。
static std::int32_t WindowPlayerTitles_SelectTitle(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B59E;
        gHoverTextD68AE8 = L"設定此稱號。" /* 原常量参考：0x005863D8。 */;
        consumed = 1;
    } else {
        graphic->imageNo = 0x3B59D;
    }
    if ((state & 2) == 0) {
        return consumed;
    }
    const std::int32_t mode = gTitleModeCF4E50;
    if (mode == 0 || mode == 1) {
        const std::int32_t id = world::detail::gSortedTitlesDE03F8[gTitlePageCF4E54 + index - 26].id;
        // 删除模式直接查原表，只有普通选择先拒绝负id。
        if (mode == 1 || id >= 0) {
            const std::int32_t originalIndex = world::detail::Sub_519840(id);
            if (originalIndex >= 0) {
                if (mode == 1) {
                    static_cast<void>(network::Sub_512310(platform::gConnectionSocket, originalIndex));
                } else {
                    static_cast<void>(network::Sub_5122C0(platform::gConnectionSocket, originalIndex));
                }
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
    }
    return 1;
}

// 原函数：0x00477710。
static std::int32_t WindowPlayerTitles_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    const auto setImage = [index](const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = image;
    };
    switch (index) {
    case 17:
    case 18:
    case 20: {
        if (index == 20 && character_select::state::gPukVersion < 8u) {
            setImage(0x3C111);
            return 0;
        }
        const std::int32_t target = index == 17 ? 7 : index == 18 ? 8 : 58;
        const std::int32_t idle = index == 17 ? 0x3BAB5 : index == 18 ? 0x3B583 : 0x3C111;
        const std::int32_t hover = index == 17 ? 0x3BAB7 : index == 18 ? 0x3B585 : 0x3C112;
        if ((state & 1) != 0) {
            setImage(hover);
            gHoverTextD68AE8 = index == 17 ? L"切換成玩家狀態視窗。" /* 原常量参考：0x005858D8。 */
                            : index == 18 ? L"切換成玩家明細視窗。" /* 原常量参考：0x005858C0。 */ : L"切換成玩家簡介視窗" /* 原常量参考：0x0058637C。 */;
            consumed = 1;
        } else {
            setImage(idle);
        }
        if ((state & 2) != 0 && gWindowStates[target].window == nullptr) {
            gCurrentWindow->state |= 0x8000u;
            static_cast<void>(WindowRuntime_Open(target));
            WindowInstance* const xSource = gCurrentWindow;
            WindowInstance* const xTarget = gWindowStates[target].window;
            xTarget->x = xSource->x;
            WindowInstance* const ySource = gCurrentWindow;
            WindowInstance* const yTarget = gWindowStates[target].window;
            yTarget->y = ySource->y;
            static_cast<void>(audio::PlaySe_52C450(56, 320));
            consumed = 1;
        }
        return consumed;
    }
    case 21: {
        if ((state & 1) != 0) {
            setImage(0x3BAC4);
            gHoverTextD68AE8 = L"設定此稱號。" /* 原常量参考：0x005863D8。 */;
            consumed = 1;
        } else {
            setImage(0x3BAC2);
        }
        if ((state & 2) != 0) {
            const std::wstring escapedText = input::EscapeTextAndNotify(gTitleDraftD45FC0, 128);
            static_cast<void>(network::Sub_512360(platform::gConnectionSocket, escapedText));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            static_cast<void>(WindowChat_RestoreInput());
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(0x3BAC3);
        }
        return consumed;
    }
    case 22:
        if ((state & 1) != 0) {
            setImage(0x3B576);
            gHoverTextD68AE8 = L"取消所選擇的稱號。" /* 原常量参考：0x005863FC。 */;
            consumed = 1;
        } else {
            setImage(0x3B574);
        }
        if ((state & 2) != 0) {
            static_cast<void>(network::Sub_5122C0(platform::gConnectionSocket, -1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & 8) != 0) {
            setImage(0x3B575);
        }
        return consumed;
    case 23: {
        if (gTitleModeCF4E50 == 1) {
            setImage(0x3B578);
            gHoverTextD68AE8 = L"刪除所選擇的稱號。" /* 原常量参考：0x005863E8。 */;
        }
        const std::int32_t mode = gTitleModeCF4E50;
        if ((state & 1) != 0) {
            if (mode == 0) {
                setImage(0x3B579);
            }
            consumed = 1;
        } else if (mode == 0) {
            setImage(0x3B577);
        }
        if ((state & 2) != 0) {
            gTitleModeCF4E50 = gTitleModeCF4E50 != 1 ? 1 : 0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        return consumed;
    }
    case 3:
    case 4:
        if ((state & 1) != 0) {
            setImage(index == 3 ? 0x3B594 : 0x3B597);
            consumed = 1;
        } else {
            setImage(index == 3 ? 0x3B592 : 0x3B595);
        }
        if ((state & 0x400) != 0) {
            const std::int32_t page = gTitlePageCF4E54;
            if (index == 3 && page > 0) {
                gTitlePageCF4E54 = page - 1;
                WindowInstance* const window = gCurrentWindow;
                static_cast<void>(WindowScroll_SetPosition(window->elements + 2, 58, page - 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else if (index == 4 && page < 58) {
                WindowInstance* const window = gCurrentWindow;
                gTitlePageCF4E54 = page + 1;
                static_cast<void>(WindowScroll_SetPosition(window->elements + 2, 58, page + 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(index == 3 ? 0x3B593 : 0x3B596);
            consumed = 1;
        }
        return consumed;
    case 24:
        if ((state & 1) != 0) {
            setImage(0x3B55B);
            consumed = 1;
        } else {
            setImage(0x3B559);
        }
        if ((state & 2) != 0) {
            std::int32_t page = gTitlePageCF4E54 + 6;
            gTitlePageCF4E54 = page;
            if (page > 58) {
                page = 58;
                gTitlePageCF4E54 = page;
            }
            WindowInstance* const window = gCurrentWindow;
            static_cast<void>(WindowScroll_SetPosition(window->elements + 2, 58, page));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & 8) != 0) {
            setImage(0x3B55A);
        }
        return consumed;
    case 25:
        if ((state & 1) != 0) {
            setImage(0x3B558);
            consumed = 1;
        } else {
            setImage(0x3B556);
        }
        if ((state & 2) != 0) {
            const std::int32_t previousPage = gTitlePageCF4E54 - 6;
            gTitlePageCF4E54 = previousPage;
            if (previousPage < 0) {
                gTitlePageCF4E54 = 0;
            }
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t page = gTitlePageCF4E54;
            static_cast<void>(WindowScroll_SetPosition(window->elements + 2, 58, page));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & 8) != 0) {
            setImage(0x3B557);
        }
        return consumed;
    case 35:
        if ((state & 2) != 0) {
            gTitleModeCF4E50 = 0;
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

// 原函数：0x004782A0。
static std::int32_t WindowPlayerTitles_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t page = gTitlePageCF4E54;
        gTitlePageCF4E54 = WindowScroll_ApplyWheel(window->elements + 2, page, 58, wheel);
    }
    return 0;
}

// WSP01：旧X片段1959；滚动非零才反求称号页码，原型/代码身份沿CAC08。
// 原函数：0x004782E0。
static std::int32_t WindowPlayerTitles_DragScroll(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t result = WindowElement_HandleVerticalScroll(index, state);
    if (result != 0) {
        WindowInstance* const window = gCurrentWindow;
        gTitlePageCF4E54 = WindowScroll_ReadPosition(window->elements + 2, 58);
    }
    return result;
}

// 原478010：草稿比较分别控制颜色与确认显隐，六行固定表起点、逐行重取窗口。
// 原函数：0x00478010。
static void WindowPlayerTitles_Refresh()
{
    auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[6].data);
    const char* const name = world::detail::GetPlayerDisplayText(1);
    nameData->text = cg::text::FromGbkWide(name);

    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", world::detail::gPlayerRuntimeSlots.level);
    auto* const levelData = static_cast<WindowTextData*>(gCurrentWindow->elements[7].data);
    levelData->text = buffer;

    const std::int32_t titleIndex = world::detail::gPlayerRuntimeSlots.titleIndex100;
    auto* const titleData = static_cast<WindowTextData*>(gCurrentWindow->elements[9].data);
    const char* const title = world::detail::GetTitleText(titleIndex);
    titleData->text = cg::text::FromGbkWide(title);

    if (input::gCurrentInputData != &input::gInputContextD2DC10) {
        gCurrentWindow->elements[8].visible = 1;
        auto* const draftData = static_cast<WindowTextData*>(gCurrentWindow->elements[8].data);
        draftData->text = gTitleDraftD45FC0;
        const bool unchanged = gTitleDraftD45FC0.compare(cg::text::FromGbkWide(world::detail::GetPlayerDisplayText(2))) == 0;
        auto* const colorData = static_cast<WindowTextData*>(gCurrentWindow->elements[8].data);
        colorData->style.color = unchanged ? std::byte{0} : std::byte{6};
    } else {
        gTitleDraftD45FC0 = input::ContextText(*gTitleInputPrototype57CCB8.input.context);
        gCurrentWindow->elements[8].visible = 0;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gTitleInputPrototype57CCB8, x, y, 1));
    }

    const bool unchanged = gTitleDraftD45FC0.compare(cg::text::FromGbkWide(world::detail::GetPlayerDisplayText(2))) == 0;
    gCurrentWindow->elements[21].visible = unchanged ? 0 : 1;

    const world::detail::TitleRecord* record =
        world::detail::gSortedTitlesDE03F8.data() + gTitlePageCF4E54;
    for (std::int32_t index = 10; index < 16; ++index) {
        WindowInstance* const window = gCurrentWindow;
        const char* const text = record->text.data();
        ++record;
        auto* const data = static_cast<WindowTextData*>(window->elements[index].data);
        data->text = cg::text::FromGbkWide(text);
    }
}

// 原4774C0及47752C公共尾，IDA分段不改变同一主更新的正常出口。
// 原函数：0x004774C0。
static std::int32_t WindowPlayerTitles_Update(const std::int32_t value)
{
    if (value == -1) {
        const char* const ownTitle = world::detail::GetPlayerDisplayText(2);
        gTitlePageCF4E54 = 0;
        gTitleDraftD45FC0 = cg::text::FromGbkWide(ownTitle);

        WindowElement* const elements = gCurrentWindow->elements;
        const char* const displayTitle = world::detail::GetPlayerDisplayText(2);
        auto* const data = static_cast<WindowTextData*>(elements[8].data);
        data->text = cg::text::FromGbkWide(displayTitle);
    }
    WindowPlayerTitles_Refresh();
    return 1;
}

// 原函数：0x004744A0。
static void WindowPlayerTitles_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_PlayerTitles].window;
    if (window != nullptr) {
        gWindowStates[WindowId_PlayerProfile].x = window->x;
        gWindowStates[WindowId_PlayerProfile].y = window->y;
        gWindowStates[WindowId_PlayerAttributes].x = window->x;
        gWindowStates[WindowId_PlayerAttributes].y = window->y;
        gWindowStates[WindowId_OwnProfile].x = window->x;
        gWindowStates[WindowId_OwnProfile].y = window->y;
    }
}

} // namespace detail

} // namespace cg::ui
