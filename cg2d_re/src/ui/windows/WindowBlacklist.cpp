#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

InputContext gInputContextC8B1B8 {}; // 原全局：0x00C8B1B8。
} // namespace cg::input

namespace cg::ui {
namespace detail {

WindowInputPrototype gBlacklistInputPrototype580EF0 { // 原全局：0x00580EF0。
    {{&input::gInputContextC8B1B8}, WindowInputReferenceKind::Context},
    75, 30, 3, 9, 0, {}, 1, 20, 0, 0,
};
namespace {

constexpr wchar_t kBlacklistRejectHint588668[] = // 原常量参考：0x00588668。
    L"拒絕接收此人的發話。";

constexpr wchar_t kBlacklistDeleteRowHint588654[] = // 原常量参考：0x00588654。
    L"刪除此筆黑名單。";

constexpr wchar_t kBlacklistOccupied5885FC[] = // 原常量参考：0x005885FC。
    L"此欄位已有名單";

constexpr wchar_t kBlacklistPreviousHint5886CC[] = // 原常量参考：0x005886CC。
    L"顯示前十筆的黑名單。";

constexpr wchar_t kBlacklistNextHint5886B4[] = // 原常量参考：0x005886B4。
    L"顯示後十筆的黑名單。";

constexpr wchar_t kBlacklistDeleteHint58869C[] = // 原常量参考：0x0058869C。
    L"刪除所選擇的黑名單。";

constexpr wchar_t kBlacklistClearHint588680[] = // 原常量参考：0x00588680。
    L"可一次刪除所有的黑名單。";

constexpr wchar_t kBlacklistRemoved58863C[] = // 原常量参考：0x0058863C。
    L"已從黑名單列表中移除{}";

constexpr wchar_t kBlacklistRemoveFailed588620[] = // 原常量参考：0x00588620。
    L"無法將{}從黑名單列表中移除";

constexpr wchar_t kBlacklistAbsent58860C[] = // 原常量参考：0x0058860C。
    L"{}不在黑名單列表中";

constexpr wchar_t kBlacklistClearConfirm5886E4[] = // 原常量参考：0x005886E4。
    L"" LR"(
)" L"  " LR"(
)" L"    確定刪除" LR"(
)" L"   所有的黑名單嗎？" LR"(
)" L" " LR"(
)" L" (確定刪除時，請按住" LR"(
)" L" CTRL鍵再選擇「是」)";

constexpr wchar_t kBlacklistCleared588754[] = // 原常量参考：0x00588754。
    L"已刪除所有的黑名單。";

constexpr wchar_t kBlacklistEmpty588738[] = // 原常量参考：0x00588738。
    L"黑名單並沒有內容可刪除。";
} // namespace


// 原函数：0x004CC6A0。
std::int32_t WindowBlacklist_ResetState()
{
    gLoginWindowRuntime.valueCC31A4 = 5;
    gLoginWindowRuntime.valueD2E444 = 0;
    gLoginWindowRuntime.valueCB0604 = 0;
    return 0;
}

// 原函数：0x004CCED0。
void WindowBlacklist_Refresh()
{
    render::GraphicDescriptor16 descriptor {};
    for (std::int32_t row = 0; row < 10; ++row) {
        gCurrentWindow->elements[9 + row * 3].visible = 0;
        gCurrentWindow->elements[10 + row * 3].visible = 0;
        gCurrentWindow->elements[11 + row * 3].visible = 0;
        const char* const recordAddress =
            reinterpret_cast<const char*>(blacklist::gRecords[row].data());
        const char* text;
        WindowTextData* destination;
        // 原门禁比较静态记录地址，不是记录首字节；空记录也绘行号。
        if (recordAddress != nullptr) {
            const std::int32_t page = gLoginWindowRuntime.valueD2E444;
            const std::uint32_t color = 0xFFFFFFFFu;
            std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
            descriptor.bytes[13] = 4;
            const std::int32_t number = page + row + 1;
            for (std::int32_t column = 0; column < 3; ++column) {
                WindowInstance* const window = gCurrentWindow;
                WindowElement* const elements = window->elements;
                const std::int32_t digit = column == 0 ? (number % 1000) / 100
                    : column == 1 ? (number % 100) / 10 : number % 10;
                const WindowElement& element = elements[9 + row * 3 + column];
                const std::int32_t y = element.y + window->y + 4;
                const std::int32_t x = element.x + window->x + 1;
                static_cast<void>(login::detail::DrawGraphic(
                    x, y, 223, digit + 0x3B6B4, 0, &descriptor));
            }
            const std::int32_t currentPage = gLoginWindowRuntime.valueD2E444;
            text = reinterpret_cast<const char*>(blacklist::gRecords[currentPage + row].data());
            destination = static_cast<WindowTextData*>(gCurrentWindow->elements[49 + row].data);
        } else {
            WindowInstance* const window = gCurrentWindow;
            text = input::LegacyText593850();
            destination = static_cast<WindowTextData*>(window->elements[49 + row].data);
        }
        destination->text = cg::text::FromGbkWide(text);
        gCurrentWindow->elements[39 + row].visible = 0;
    }
    if (input::gCurrentInputData == &input::gInputContextC8B1B8) {
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gBlacklistInputPrototype580EF0, x, y, 1));
        render::DrawInputText_4568A0(&input::gInputContextC8B1B8);
    }
}

// 原函数：0x004CC6C0。
std::int32_t WindowBlacklist_Update(const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.valueD2E444 = 0;
    }
    WindowBlacklist_Refresh();
    return 1;
}

// 原函数：0x004CC6E0。
std::int32_t WindowBlacklist_EditRow(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const wchar_t* const hint = gLoginWindowRuntime.blacklistDeleteModeD67BC4 != 0
            ? kBlacklistDeleteRowHint588654 : kBlacklistRejectHint588668;
        gHoverTextD68AE8 = hint;
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FCC;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB02;
    }
    if ((state & 2) == 0) {
        return consumed;
    }
    if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 1) {
        const std::int32_t recordIndex = gLoginWindowRuntime.valueD2E444 + index - 59;
        const char* const record = reinterpret_cast<const char*>(blacklist::gRecords[recordIndex].data());
        if (std::strcmp(record, input::LegacyText593850()) != 0) {
            std::wstring message;
            char name[17] {};
            std::strcpy(name, record);
            if (blacklist::ContainsBlacklistRecord_4013D0(cg::text::FromGbkWide(name)) != 0) {
                const wchar_t* const format = blacklist::RemoveBlacklistRecord_4012F0(cg::text::FromGbkWide(name)) != 0
                    ? kBlacklistRemoved58863C : kBlacklistRemoveFailed588620;
                cg::text::FormatTo(message, format, cg::text::FromGbkWide(name).c_str());
            } else {
                cg::text::FormatTo(message, kBlacklistAbsent58860C, cg::text::FromGbkWide(name).c_str());
            }
            world::detail::Sub_44B050(message, 4, 3);
        }
    }
    const std::int32_t currentIndex = gLoginWindowRuntime.valueD2E444 + index - 59;
    const char* const currentRecord = reinterpret_cast<const char*>(blacklist::gRecords[currentIndex].data());
    if (std::strlen(currentRecord) != 0) {
        input::gInputContextC8B1B8.reset();
        input::SetContext_44B330(&input::gInputContext626AA0);
        world::detail::Sub_44B050(kBlacklistOccupied5885FC, 4, 3);
        return 1;
    }
    WindowInstance* const window = gCurrentWindow;
    void* const focused = gFocusedElementDataD45F94;
    WindowElement* const elements = window->elements;
    void* const target = elements[0].data;
    const std::int32_t y = elements[index].y;
    const bool changeFocus = focused != target;
    gBlacklistInputPrototype580EF0.y = y + 1;
    const std::int32_t windowY = window->y;
    const std::int32_t windowX = window->x;
    static_cast<void>(WindowInput_ActivateElement(&gBlacklistInputPrototype580EF0, windowX, windowY, 0));
    input::SetContext_44B330(&input::gInputContextC8B1B8);
    if (changeFocus) {
        gFocusedElementDataD45F94 = gCurrentWindow->elements[0].data;
    }
    return 1;
}

// 原函数：0x004CC9B0。
std::int32_t WindowBlacklist_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    switch (index) {
    case 3:
    case 4: {
        const std::int32_t base = index == 3 ? 0x3B592 : 0x3B595;
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo =
            base + ((state & 1) != 0 ? 2 : 0);
        if ((state & 1) != 0) {
            consumed = 1;
        }
        if ((state & 8) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = base + 1;
            consumed = 1;
        }
        if ((state & 0x400) != 0) {
            const std::int32_t page = gLoginWindowRuntime.valueD2E444 + (index == 3 ? -1 : 1);
            gLoginWindowRuntime.valueD2E444 = page;
            const bool boundary = index == 3 ? page < 0 : page > 89;
            if (boundary) {
                gLoginWindowRuntime.valueD2E444 = index == 3 ? 0 : 90;
            }
            static_cast<void>(audio::PlaySe_52C450(boundary ? 55 : 51, 320));
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t currentPage = gLoginWindowRuntime.valueD2E444;
            static_cast<void>(WindowScroll_SetPosition(&window->elements[2], 90, currentPage));
            consumed = 1;
        }
        break;
    }
    case 5:
    case 6: {
        const std::int32_t base = index == 5 ? 0x3B556 : 0x3B559;
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo =
            base + ((state & 1) != 0 ? 2 : 0);
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = index == 5 ? kBlacklistPreviousHint5886CC : kBlacklistNextHint5886B4;
        }
        if ((state & 0x400) != 0) {
            const std::int32_t page = gLoginWindowRuntime.valueD2E444 + (index == 5 ? -10 : 10);
            gLoginWindowRuntime.valueD2E444 = page;
            const bool boundary = index == 5 ? page < 0 : page > 90;
            if (boundary) {
                gLoginWindowRuntime.valueD2E444 = index == 5 ? 0 : 90;
            }
            static_cast<void>(audio::PlaySe_52C450(boundary ? 55 : 51, 320));
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t currentPage = gLoginWindowRuntime.valueD2E444;
            static_cast<void>(WindowScroll_SetPosition(&window->elements[2], 90, currentPage));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = base + 1;
        }
        break;
    }
    case 7:
        if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 1) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FC7;
            gHoverTextD68AE8 = kBlacklistDeleteHint58869C;
        }
        if ((state & 1) != 0) {
            if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 0) {
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FC8;
            }
            gHoverTextD68AE8 = kBlacklistDeleteHint58869C;
            consumed = 1;
        } else if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FC6;
        }
        if ((state & 2) != 0) {
            gLoginWindowRuntime.blacklistDeleteModeD67BC4 =
                gLoginWindowRuntime.blacklistDeleteModeD67BC4 != 1 ? 1 : 0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        break;
    case 8:
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo =
            (state & 1) != 0 ? 0x8FCB : 0x8FC9;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = kBlacklistClearHint588680;
            consumed = 1;
        }
        if ((state & 2) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x8FCA;
            static_cast<void>(WindowBlacklist_ConfirmClear());
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        break;
    default:
        break;
    }
    return consumed;
}

// WSP01：旧X片段1999；恰1才按原90参数更新，保原X身份与返回。
// 原函数：0x004CCE90。
std::int32_t WindowBlacklist_DragScroll(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t consumed = WindowElement_HandleVerticalScroll(index, state);
    if (consumed == 1) {
        gLoginWindowRuntime.valueD2E444 = WindowScroll_ReadPosition(&gCurrentWindow->elements[2], 90);
    }
    return consumed;
}

// 原函数：0x004CD150。
login::YobiAction* WindowBlacklist_ConfirmClear()
{
    gWindow47Runtime.visibleMaskCC4240 = 3;
    gWindow47Runtime.widthUnitsCC4244 = 3;
    gWindow47Runtime.valueCC424C = 0;
    gWindow47Runtime.valueCC4248 = 0;
    gWindow47Runtime.initializeCC4250 = nullptr;
    gWindow47Runtime.actionsCC4254[5] = nullptr;
    gWindow47Runtime.actionsCC4254[4] = nullptr;
    gWindow47Runtime.actionsCC4254[3] = nullptr;
    gWindow47Runtime.actionsCC4254[2] = nullptr;
    gWindow47Runtime.actionsCC4254[0] = &WindowBlacklist_Clear;
    gWindow47Runtime.actionsCC4254[1] = &WindowBlacklist_CancelClear;
    gWindow47Runtime.textCC426C = kBlacklistClearConfirm5886E4;
    return WindowRuntime_CreateAnimation(WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004CD1E0。
std::int32_t WindowBlacklist_Clear(const std::int32_t, const std::int32_t)
{
    const wchar_t* const message = blacklist::ClearBlacklistRecords_401470() != 0
        ? kBlacklistCleared588754 : kBlacklistEmpty588738;
    world::detail::Sub_44B050(message, 4, 3);
    static_cast<void>(audio::PlaySe_52C450(78, 320));
    return 1;
}

// 原函数：0x004CD220。
std::int32_t WindowBlacklist_CancelClear(const std::int32_t, const std::int32_t)
{
    if (gWindowStates[WindowId_Blacklist].window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Blacklist, 1, 0, nullptr));
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
