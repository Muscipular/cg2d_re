#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

InputContext gInputContextD2DC10 {}; // 原全局：0x00D2DC10。
} // namespace cg::input

namespace cg::ui {
namespace detail {

std::wstring gTitleDraftD45FC0 {}; // 原全局：0x00D45FC0。

WindowInputPrototype gTitleInputPrototype57CCB8 { // 原全局：0x0057CCB8。
    {{&input::gInputContextD2DC10}, WindowInputReferenceKind::Context},
    105, 31, 3, 12, 6, {}, 1, 16, 0, 0,
};

static constexpr wchar_t kTitleEditHint5863C4[] = L"設定自己的稱號。"; // 原常量参考：0x005863C4。

static constexpr wchar_t kTitleSelectHint5863D8[] = L"設定此稱號。"; // 原常量参考：0x005863D8。

static constexpr wchar_t kTitleStatusHint5858D8[] = L"切換成玩家狀態視窗。"; // 原常量参考：0x005858D8。

static constexpr wchar_t kTitleDetailsHint5858C0[] = L"切換成玩家明細視窗。"; // 原常量参考：0x005858C0。

static constexpr wchar_t kTitleProfileHint58637C[] = L"切換成玩家簡介視窗"; // 原常量参考：0x0058637C。

static constexpr wchar_t kTitleCancelHint5863FC[] = L"取消所選擇的稱號。"; // 原常量参考：0x005863FC。

static constexpr wchar_t kTitleDeleteHint5863E8[] = L"刪除所選擇的稱號。"; // 原常量参考：0x005863E8。

// 原函数：0x00477540。
std::int32_t WindowPlayerTitles_EditDraft(const std::int32_t, const std::int32_t state)
{
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kTitleEditHint5863C4;
    }
    if ((state & 2) == 0) {
        return 0;
    }
    WindowInstance* const window = gCurrentWindow;
    if (gFocusedElementDataD45F94 != window->elements[0].data) {
        const std::size_t length = gTitleDraftD45FC0.size() + 1;
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
std::int32_t WindowPlayerTitles_SelectTitle(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B59E;
        gHoverTextD68AE8 = kTitleSelectHint5863D8;
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
std::int32_t WindowPlayerTitles_HandleButtons(const std::int32_t index, const std::int32_t state)
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
            gHoverTextD68AE8 = index == 17 ? kTitleStatusHint5858D8
                            : index == 18 ? kTitleDetailsHint5858C0 : kTitleProfileHint58637C;
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
            gHoverTextD68AE8 = kTitleSelectHint5863D8;
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
            gHoverTextD68AE8 = kTitleCancelHint5863FC;
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
            gHoverTextD68AE8 = kTitleDeleteHint5863E8;
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
std::int32_t WindowPlayerTitles_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t page = gTitlePageCF4E54;
        gTitlePageCF4E54 = WindowScroll_ApplyWheel(window->elements + 2, page, 58, wheel);
    }
    return 0;
}

// WSP01：旧X片段1959；滚动非零才反求称号页码，原型/代码身份沿CAC08。
// 原函数：0x004782E0。
std::int32_t WindowPlayerTitles_DragScroll(const std::int32_t index, const std::int32_t state)
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
void WindowPlayerTitles_Refresh()
{
    auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[6].data);
    const char* const name = world::detail::BorrowPlayerDisplayText(1);
    const std::size_t nameLength = std::strlen(name) + 1;
    nameData->text = cg::text::FromGbkWide(name);

    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", world::detail::ReadPlayerLevel());
    auto* const levelData = static_cast<WindowTextData*>(gCurrentWindow->elements[7].data);
    const std::size_t levelLength = buffer.size() + 1;
    levelData->text = buffer;

    const std::int32_t titleIndex = world::detail::ReadPlayerTitleIndex();
    auto* const titleData = static_cast<WindowTextData*>(gCurrentWindow->elements[9].data);
    const char* const title = world::detail::BorrowTitleText(titleIndex);
    const std::size_t titleLength = std::strlen(title) + 1;
    titleData->text = cg::text::FromGbkWide(title);

    if (input::gCurrentInputData != &input::gInputContextD2DC10) {
        gCurrentWindow->elements[8].visible = 1;
        auto* const draftData = static_cast<WindowTextData*>(gCurrentWindow->elements[8].data);
        draftData->text = gTitleDraftD45FC0;
        const bool unchanged = gTitleDraftD45FC0.compare(cg::text::FromGbkWide(world::detail::BorrowPlayerDisplayText(2))) == 0;
        auto* const colorData = static_cast<WindowTextData*>(gCurrentWindow->elements[8].data);
        colorData->bytes00[0] = unchanged ? std::byte{0} : std::byte{6};
    } else {
        gTitleDraftD45FC0 = input::ContextText(*gTitleInputPrototype57CCB8.input.context);
        gCurrentWindow->elements[8].visible = 0;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gTitleInputPrototype57CCB8, x, y, 1));
    }

    const bool unchanged = gTitleDraftD45FC0.compare(cg::text::FromGbkWide(world::detail::BorrowPlayerDisplayText(2))) == 0;
    gCurrentWindow->elements[21].visible = unchanged ? 0 : 1;

    const world::detail::TitleRecord* record =
        world::detail::gSortedTitlesDE03F8.data() + gTitlePageCF4E54;
    for (std::int32_t index = 10; index < 16; ++index) {
        WindowInstance* const window = gCurrentWindow;
        const char* const text = record->text.data();
        ++record;
        auto* const data = static_cast<WindowTextData*>(window->elements[index].data);
        const std::size_t length = std::strlen(text) + 1;
        data->text = cg::text::FromGbkWide(text);
    }
}

// 原4774C0及47752C公共尾，IDA分段不改变同一主更新的正常出口。
// 原函数：0x004774C0。
std::int32_t WindowPlayerTitles_Update(const std::int32_t value)
{
    if (value == -1) {
        const char* const ownTitle = world::detail::BorrowPlayerDisplayText(2);
        gTitlePageCF4E54 = 0;
        const std::size_t draftLength = std::strlen(ownTitle) + 1;
        gTitleDraftD45FC0 = cg::text::FromGbkWide(ownTitle);

        WindowElement* const elements = gCurrentWindow->elements;
        const char* const displayTitle = world::detail::BorrowPlayerDisplayText(2);
        const std::size_t displayLength = std::strlen(displayTitle) + 1;
        auto* const data = static_cast<WindowTextData*>(elements[8].data);
        data->text = cg::text::FromGbkWide(displayTitle);
    }
    WindowPlayerTitles_Refresh();
    return 1;
}

// 原函数：0x004744A0。
void WindowPlayerTitles_Release()
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
