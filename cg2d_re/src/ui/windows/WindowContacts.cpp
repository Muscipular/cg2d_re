#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {
namespace {


constexpr wchar_t kMailDetailedHint587444[] = L"切換為詳細顯示。"; // 原常量参考：0x00587444。

constexpr wchar_t kMailPreviewHint587458[] = L"切換為預覽顯示。"; // 原常量参考：0x00587458。

constexpr wchar_t kMailSortHint58746C[] = L"切換名片的排序方式。"; // 原常量参考：0x0058746C。

constexpr wchar_t kMailGroupHint58749C[] = L"對一群人寄送郵件。"; // 原常量参考：0x0058749C。

constexpr wchar_t kMailContactsHint5874B0[] = L"切換至名片視窗。"; // 原常量参考：0x005874B0。

constexpr wchar_t kMailGuildHint5874C4[] = L"切換至家族名簿視窗。"; // 原常量参考：0x005874C4。

constexpr wchar_t kMailMiniHint5874DC[] = L"切換到迷你郵件群組。"; // 原常量参考：0x005874DC。

constexpr wchar_t kMailGuildDetailsHint587544[] = L"開啟家族詳細資料視窗。"; // 原常量参考：0x00587544。

constexpr wchar_t kMailRankHint587484[] = L"變更此家族成員的稱號。"; // 原常量参考：0x00587484。

constexpr wchar_t kMailReadContactHint5874F4[] = L"察看此名片之主人的郵件紀錄"; // 原常量参考：0x005874F4。

constexpr wchar_t kMailReadGuildHint587510[] = L"察看此家族成員的郵件紀錄"; // 原常量参考：0x00587510。

constexpr wchar_t kMailReadMiniHint58752C[] = L"瀏覽此迷你郵件的歷程。"; // 原常量参考：0x0058752C。

constexpr wchar_t kMailWriteContactHint5876B0[] = L"寄送郵件給此名片的主人。"; // 原常量参考：0x005876B0。

constexpr wchar_t kMailWriteGuildHint5876CC[] = L"對此家族成員寄送郵件。"; // 原常量参考：0x005876CC。

constexpr wchar_t kMailWriteMiniHint5876E4[] = L"傳送迷你郵件。"; // 原常量参考：0x005876E4。

constexpr wchar_t kMailDeleteContactHint58755C[] = L"刪除此名片。"; // 原常量参考：0x0058755C。

constexpr wchar_t kMailDeleteGuildHint58756C[] = L"對此家族成員進行除名。"; // 原常量参考：0x0058756C。

constexpr wchar_t kMailDeleteMiniHint587584[] = L"刪除此迷你郵件的名片。"; // 原常量参考：0x00587584。

constexpr wchar_t kMailDeleteContactFormat58759C[] = // 原常量参考：0x0058759C。
    L"" LR"(
)" L"     確定將" LR"(
)" L"　" LR"(
)" L"　{}" LR"(
)" L"　" LR"(
)" L"　從名片裡刪除嗎？" LR"(
)" L" " LR"(
)" L" (確定刪除時，請按住" LR"(
)" L"  CTRL鍵再選擇「是」)";

constexpr wchar_t kMailDeleteGuildFormat5875F4[] = // 原常量参考：0x005875F4。
    LR"(
     確定將
　
　{}
　
從家族會員名單中刪除嗎？

 (確定刪除時，請按住
  CTRL鍵再選擇「是」))";

constexpr wchar_t kMailDeleteMiniFormat587654[] = // 原常量参考：0x00587654。
    LR"(
     確定將
　
　{}
　
從迷你郵件中刪除嗎？

 (確定刪除時，請按住
  CTRL鍵再選擇「是」))";

constexpr std::array<std::int32_t, 15> kMailSortImages54F4C0 { // 原常量参考：0x0054F4C0。
    0x3B60C, 0x3B60D, 0x3B60F, 0x3B610, 0x3B613,
    0x3B60C, 0x3B60E, 0x3B60F, 0x3B610, 0x3B613,
    0x3BAA6, 0x3BA60, 0x3BAA7, 0x3BA61, 0x3BAA8,
};

constexpr std::array<std::array<std::int32_t, 2>, 3> kMailImageAnchors56F790 {{ // 原常量参考：0x0056F790。
    {62, 115}, {62, 202}, {62, 289},
}};

constexpr wchar_t kMailNumberFormat553FCC[] = L"{:3}"; // 原常量参考：0x00553FCC。
} // namespace


// 原函数：0x004A0970。
std::int32_t WindowContacts_Update(const std::int32_t value)
{
    if (value == -1) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t mode = gValueD67B38;
        const std::uint32_t sortMode = platform::gValueD67CD0;
        gLoginWindowRuntime.mailSortModeD67B3C = sortMode;
        WindowContacts_SetDisplayMode(mode, category);
        static_cast<void>(world::detail::Sub_518DA0());
        WindowContacts_ClampScroll();
    }
    return 1;
}

// 原函数：0x004A09B0。
std::int32_t WindowContacts_Draw(const std::int32_t value)
{
    static_cast<void>(WindowContacts_DragScroll(value));
    static_cast<void>(WindowMail_AdvanceUnreadBlink());
    static_cast<void>(WindowContacts_Refresh());
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004A09E0。
std::int32_t WindowContacts_Refresh()
{
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::uint32_t sortMode = gLoginWindowRuntime.mailSortModeD67B3C;
    auto* const sortGraphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[8].data);
    sortGraphic->imageNo = kMailSortImages54F4C0[category * 5 + sortMode];
    WindowContacts_Sort();
    if (gValueD67B38 == 0) {
        const std::uint32_t page = world::detail::gValueCB1384;
        render::GraphicDescriptor16 descriptor {};
        const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
        world::detail::gValueCB0354 = page;
        if (currentCategory == 1) {
            for (std::int32_t index = 21; index < 27; ++index) {
                const std::int32_t allowed = WindowGuildDetails_ReadRankPermissions();
                gCurrentWindow->elements[index].visible = allowed != 0 ? 1 : 0;
            }
            if (WindowGuildDetails_ReadRankPermissions() != 0) {
                for (std::int32_t row = 0; row < 3; ++row) {
                    const std::int32_t rowCategory = gLoginWindowRuntime.valuesD67B40[6];
                    const std::int32_t scroll = gLoginWindowRuntime.valuesD67B40[rowCategory];
                    const std::int32_t memberIndex = gLoginWindowRuntime.mailIndicesC9FCE4[scroll + row];
                    const std::int32_t rank = login::detail::gOrderedRecords1051B14[memberIndex].value00;
                    const std::uint8_t flags = login::detail::ReadGuildRankFlagsByte1051514(rank);
                    gCurrentWindow->elements[21 + row * 2].visible = 0;
                    gCurrentWindow->elements[22 + row * 2].visible = (flags & 0x80) == 0 ? 1 : 0;
                }
            } else {
                for (std::int32_t row = 0; row < 3; ++row) {
                    gCurrentWindow->elements[21 + row * 2].visible = 0;
                    gCurrentWindow->elements[22 + row * 2].visible = 0;
                }
            }
            for (std::int32_t index = 29; index < 46; index += 8) {
                const std::int32_t allowed = WindowGuildDetails_CanEdit(2);
                gCurrentWindow->elements[index].visible = allowed != 0 ? 1 : 0;
            }
            if (login::detail::ReadGuildStatus10514AC() == -1) {
                gCurrentWindow->elements[20].visible = 0;
            } else {
                gCurrentWindow->elements[20].visible = 1;
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t status = login::detail::ReadGuildStatus10514AC();
                auto* const data = static_cast<WindowTextData*>(window->elements[20].data);
                WindowMail_WriteGuildStatus(data, status);
            }
        }

        // 原类别未覆盖时沿用未定义/旧借用；正常三类每行只绑定指针，不缓存字段值。
        MailContactView record;
        std::int32_t privileged;
        std::wstring numberText;
        for (std::int32_t row = 0; row < 3; ++row) {
            const std::int32_t rowCategory = gLoginWindowRuntime.valuesD67B40[6];
            const std::int32_t scroll = gLoginWindowRuntime.valuesD67B40[rowCategory];
            WindowInstance* const entryWindow = gCurrentWindow;
            WindowElement* const entryElements = entryWindow->elements;
            const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[scroll + row];
            entryElements[29 + row * 8].visible = 1;
            gCurrentWindow->elements[28 + row * 8].visible = 1;
            gCurrentWindow->elements[27 + row * 8].visible = 1;
            switch (gLoginWindowRuntime.valuesD67B40[6]) {
            case 0:
                record = WindowMail_BorrowContactView(world::detail::gContactsF19FB0[recordIndex]);
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[51 + row].data)->imageNo = 0x3B9E0;
                break;
            case 1:
                record = WindowMail_BorrowContactView(login::detail::gOrderedRecords1051B14[recordIndex]);
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[51 + row].data)->imageNo = 0x3B9E8;
                break;
            case 2:
                record = WindowMail_BorrowContactView(world::detail::ReadMiniMailContact1075A90(recordIndex));
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[51 + row].data)->imageNo = 0x3B9E0;
                break;
            }
            const std::int32_t textIndex = 30 + row * 8;
            if (*record.used == 0) {
                gCurrentWindow->elements[29 + row * 8].visible = 0;
                gCurrentWindow->elements[28 + row * 8].visible = 0;
                gCurrentWindow->elements[27 + row * 8].visible = 0;
                for (std::int32_t index = textIndex; index < textIndex + 5; ++index) {
                    auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
                    data->text = cg::text::FromGbkWide(input::LegacyText593850());
                }
                continue;
            }
            const std::uint32_t color = 0xFFFFFFFFu;
            std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
            descriptor.bytes[13] = 4;
            const std::int32_t image = character_select::detail::Sub_51BC50(*record.imageNo);
            const std::int32_t anchorY = kMailImageAnchors56F790[row][1];
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t y = window->y + anchorY - 5;
            const std::int32_t windowX = window->x;
            const std::int32_t anchorX = kMailImageAnchors56F790[row][0];
            static_cast<void>(login::detail::DrawGraphic(
                windowX + anchorX - 7, y, 223, image, 0, &descriptor));
            auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex].data);
            nameData->text = cg::text::FromGbkWide(record.name);
            cg::text::FormatTo(numberText, kMailNumberFormat553FCC, *record.number);
            auto* const numberData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 1].data);
            numberData->text = numberText;
            switch (gLoginWindowRuntime.valuesD67B40[6]) {
            case 0:
            case 2: {
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 2].data);
                data->text = cg::text::FromGbkWide(record.text32);
                break;
            }
            case 1: {
                const auto* const member = &login::detail::gOrderedRecords1051B14[recordIndex];
                privileged = WindowGuildDetails_CanEditRank(member->value00, 0x80);
                const std::int32_t rank = member->value00;
                const char* const text = rank == -1
                    ? input::LegacyText593850() : login::detail::BorrowGuildRankText1051518(rank);
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 2].data);
                data->text = cg::text::FromGbkWide(text);
                break;
            }
            }
            auto* const textData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 3].data);
            textData->text = cg::text::FromGbkWide(record.text21);
            WindowElement* const statusElements = gCurrentWindow->elements;
            const std::int16_t status = record.ReadStatus();
            auto* const statusData = static_cast<WindowTextData*>(statusElements[textIndex + 4].data);
            WindowMail_WriteContactStatus(record, statusData, status);
            switch (gLoginWindowRuntime.valuesD67B40[6]) {
            case 0:
                if (world::detail::Sub_519090(recordIndex) != 0) {
                    WindowMail_SetUnreadGraphic(27 + row * 8);
                }
                break;
            case 1:
                if (WindowGuildDetails_CanEdit(2) != 0) {
                    gCurrentWindow->elements[29 + row * 8].visible = privileged == 0 ? 1 : 0;
                } else {
                    const std::int32_t ownKey = login::detail::ReadGuildOwnKey10514B8();
                    const std::int32_t memberKey = login::detail::gOrderedRecords1051B14[recordIndex].value0C;
                    gCurrentWindow->elements[29 + row * 8].visible = memberKey == ownKey ? 1 : 0;
                }
                if (world::detail::Sub_5190F0(recordIndex) != 0) {
                    WindowMail_SetUnreadGraphic(27 + row * 8);
                }
                break;
            case 2:
                if (world::detail::Sub_519170(recordIndex) != 0) {
                    WindowMail_SetUnreadGraphic(27 + row * 8);
                }
                break;
            }
        }
        return 108;
    }

    if (gLoginWindowRuntime.valuesD67B40[6] == 1) {
        gCurrentWindow->elements[20].visible = 1;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t status = login::detail::ReadGuildStatus10514AC();
        auto* const data = static_cast<WindowTextData*>(window->elements[20].data);
        WindowMail_WriteGuildStatus(data, status);
    } else {
        gCurrentWindow->elements[20].visible = 0;
    }
    MailContactView record;
    for (std::int32_t row = 0; row < 9; ++row) {
        const std::int32_t rowCategory = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t scroll = gLoginWindowRuntime.valuesD67B40[3 + rowCategory];
        WindowInstance* const entryWindow = gCurrentWindow;
        const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[scroll + row];
        entryWindow->elements[56 + row * 5].visible = 1;
        gCurrentWindow->elements[55 + row * 5].visible = 1;
        gCurrentWindow->elements[54 + row * 5].visible = 1;
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0:
            record = WindowMail_BorrowContactView(world::detail::gContactsF19FB0[recordIndex]);
            gCurrentWindow->elements[56 + row * 5].visible = 1;
            break;
        case 1: {
            const auto* const member = &login::detail::gOrderedRecords1051B14[recordIndex];
            record = WindowMail_BorrowContactView(*member);
            if (WindowGuildDetails_CanEdit(2) != 0) {
                const std::int32_t privileged = WindowGuildDetails_CanEditRank(member->value00, 0x80);
                gCurrentWindow->elements[56 + row * 5].visible = privileged == 0 ? 1 : 0;
            } else {
                const std::int32_t memberKey = member->value0C;
                const std::int32_t ownKey = login::detail::ReadGuildOwnKey10514B8();
                gCurrentWindow->elements[56 + row * 5].visible = memberKey == ownKey ? 1 : 0;
            }
            break;
        }
        case 2:
            record = WindowMail_BorrowContactView(world::detail::ReadMiniMailContact1075A90(recordIndex));
            break;
        }
        const std::int32_t textIndex = 57 + row * 5;
        if (*record.used == 0) {
            gCurrentWindow->elements[56 + row * 5].visible = 0;
            gCurrentWindow->elements[55 + row * 5].visible = 0;
            gCurrentWindow->elements[54 + row * 5].visible = 0;
            for (std::int32_t index = textIndex; index < textIndex + 2; ++index) {
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
                data->text = cg::text::FromGbkWide(input::LegacyText593850());
            }
            continue;
        }
        auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex].data);
        nameData->text = cg::text::FromGbkWide(record.name);
        auto* const statusData = static_cast<WindowTextData*>(gCurrentWindow->elements[textIndex + 1].data);
        const std::int16_t status = record.ReadStatus();
        WindowMail_WriteContactStatus(record, statusData, status);
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0:
            if (world::detail::Sub_519090(recordIndex) != 0) {
                WindowMail_SetUnreadGraphic(54 + row * 5);
            }
            break;
        case 1:
            if (world::detail::Sub_5190F0(recordIndex) != 0) {
                const std::int32_t blink = gLoginWindowRuntime.mailBlinkFlagD67B60;
                auto* const graphic = static_cast<WindowGraphicData*>(
                    gCurrentWindow->elements[54 + row * 5].data);
                graphic->imageNo = blink != 0 ? 0x3B549 : 0x3B547;
            }
            break;
        case 2:
            if (world::detail::Sub_519170(recordIndex) != 0) {
                WindowMail_SetUnreadGraphic(54 + row * 5);
            }
            break;
        }
    }
    return 3672;
}

// 原函数：0x004A1690。
void WindowContacts_SetDisplayMode(const std::int32_t mode, const std::int32_t category)
{
    WindowContacts_ApplyDisplayLayout(mode);
    WindowContacts_ApplyCategoryLayout(category);
}

// 原函数：0x004A16B0。
void WindowContacts_ApplyDisplayLayout(const std::int32_t mode)
{
    for (std::int32_t index = 0; index < 21; ++index) {
        gCurrentWindow->elements[index].visible = 1;
    }
    if (mode == 0) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[7].data);
        graphic->imageNo = 0x3B612;
        for (std::int32_t index = 21; index < 54; ++index) {
            gCurrentWindow->elements[index].visible = 1;
        }
        for (std::int32_t index = 54; index < 108; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
    } else if (mode == 1) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[7].data);
        graphic->imageNo = 0x3B611;
        for (std::int32_t index = 21; index < 54; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        for (std::int32_t index = 54; index < 108; ++index) {
            gCurrentWindow->elements[index].visible = 1;
        }
    }
}

// 原函数：0x004A1780。
void WindowContacts_ApplyCategoryLayout(const std::int32_t category)
{
    if (category == 0) {
        for (std::int32_t index = 21; index < 27; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        gCurrentWindow->elements[13].visible = 0;
        gCurrentWindow->elements[20].visible = 0;
        for (const std::int32_t index : {32, 40, 48}) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t x = gWindowElements56DB38[32].x;
            window->elements[index].x = x - 24;
        }
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[10].data);
        graphic->imageNo = 0x3B57E;
        for (std::int32_t index = 21; index < 27; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
    } else if (category == 1) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[11].data);
        graphic->imageNo = 0x3B581;
        gCurrentWindow->elements[13].visible = 0;
        for (const std::int32_t index : {32, 40, 48}) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t x = gWindowElements56DB38[32].x;
            window->elements[index].x = x;
        }
    } else if (category == 2) {
        for (std::int32_t index = 21; index < 27; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        gCurrentWindow->elements[13].visible = 1;
        for (const std::int32_t index : {34, 42, 50, 58, 63, 68, 73, 78, 83, 88, 93, 98, 20}) {
            gCurrentWindow->elements[index].visible = 0;
        }
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[12].data);
        graphic->imageNo = 0x3C116;
        for (const std::int32_t index : {32, 40, 48}) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t x = gWindowElements56DB38[32].x;
            window->elements[index].x = x - 24;
        }
        for (const std::int32_t index : {34, 42, 50, 0, 3, 4, 8, 19, 20}) {
            gCurrentWindow->elements[index].visible = 0;
        }
        gCurrentWindow->elements[5].visible = 1;
        gCurrentWindow->elements[6].visible = 1;
        for (std::int32_t index = 21; index < 27; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
    } else {
        return;
    }

    for (const std::int32_t index : {1, 2, 7}) {
        WindowInstance* const xWindow = gCurrentWindow;
        const std::int32_t x = gWindowElements56DB38[index].x;
        xWindow->elements[index].x = x + (category == 2 ? 110 : 0);
        WindowInstance* const yWindow = gCurrentWindow;
        const std::int32_t y = gWindowElements56DB38[index].y;
        yWindow->elements[index].y = y + (category == 2 ? 17 : 0);
    }
}

// 原函数：0x004A1C30。
std::int32_t WindowContacts_ScrollRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t imageOffset = index == 14 ? 0 : 3;
    graphic->imageNo = 0x3B592 + imageOffset;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B594 + imageOffset;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B593 + imageOffset;
    }
    if ((state & 0x400) == 0) {
        return 0;
    }

    const std::int32_t mode = gValueD67B38;
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    std::int32_t& position = gLoginWindowRuntime.valuesD67B40[(mode == 0 ? 0 : 3) + category];
    position += index == 14 ? -1 : 1;
    if ((mode == 0 || mode == 1) && category >= 0 && category <= 2) {
        std::int32_t maximum;
        if (category == 0) {
            maximum = mode == 0 ? 57 : 51;
        } else if (category == 1) {
            const std::int32_t count = login::detail::gOrderedRecordCount1051510;
            maximum = count < 100 ? (mode == 0 ? 97 : 91) : count - (mode == 0 ? 3 : 9);
        } else {
            maximum = mode == 0 ? 17 : 11;
        }
        std::int32_t sound = 51;
        if (position < 0) {
            position = 0;
            sound = 55;
        } else if (position > maximum) {
            position = maximum;
            sound = 55;
        }
        static_cast<void>(audio::PlaySe_52C450(sound, 320));
    }
    WindowContacts_ClampScroll();
    login::detail::Sub_520200();
    return 1;
}

// 原函数：0x004A1EF0。
std::int32_t WindowContacts_HandleNavigation(const std::int32_t index, const std::int32_t state)
{
    if (index >= 51 && index <= 53) {
        if ((state & 4) == 0) {
            return 0;
        }
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0: {
            const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
            if (world::detail::gContactsF19FB0[recordIndex].used == 0) {
                return 1;
            }
            static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 0, recordIndex));
            break;
        }
        case 1: {
            const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
            const auto& record = login::detail::gOrderedRecords1051B14[recordIndex];
            if (record.value08 == 0) {
                return 1;
            }
            const std::int32_t target = record.value0C;
            static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 1, target));
            break;
        }
        case 2: {
            const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
            const auto& record = world::detail::ReadMiniMailContact1075A90(recordIndex);
            if (record.used == 0) {
                return 1;
            }
            const std::int32_t target = record.key04;
            static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 2, target));
            break;
        }
        }
        return 0;
    }
    if (index < 1 || index > 6) {
        return 0;
    }

    const std::int32_t imageOffset = (index & 1) != 0 ? 0 : 3;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558 + imageOffset;
        if (index <= 2) {
            gHoverTextD68AE8 = gValueD67B38 != 0 ? kMailDetailedHint587444 : kMailPreviewHint587458;
        } else if (index <= 4) {
            gHoverTextD68AE8 = kMailSortHint58746C;
        }
    } else {
        graphic->imageNo = 0x3B556 + imageOffset;
    }

    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        if (index <= 2) {
            const std::int32_t oldMode = gValueD67B38;
            const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
            const std::int32_t mode = oldMode == 0 ? 1 : 0;
            gValueD67B38 = mode;
            WindowContacts_SetDisplayMode(mode, category);
            const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
            gLoginWindowRuntime.valuesD67B40[3 + currentCategory] = 0;
            gLoginWindowRuntime.valuesD67B40[currentCategory] = 0;
            WindowContacts_ClampScroll();
        } else if (index <= 4) {
            const std::int32_t oldSort = std::bit_cast<std::int32_t>(gLoginWindowRuntime.mailSortModeD67B3C);
            const std::int32_t sort = (oldSort + (index == 3 ? 4 : 6)) % 5;
            gLoginWindowRuntime.mailSortModeD67B3C = std::bit_cast<std::uint32_t>(sort);
            platform::gValueD67CD0 = std::bit_cast<std::uint32_t>(sort);
        } else {
            const std::int32_t mode = gValueD67B38;
            const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
            const std::int32_t step = mode == 0 ? 3 : 9;
            gLoginWindowRuntime.valuesD67B40[(mode == 0 ? 0 : 3) + category] += index == 5 ? -step : step;
            WindowContacts_ClampScroll();
        }
        login::detail::Sub_520200();
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        // 业务调用可能改变当前窗口，按原尾路径重新借用载荷。
        auto* const pressed = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        pressed->imageNo = 0x3B557 + imageOffset;
    }
    return consumed;
}

// 原函数：0x004A2610。
std::int32_t WindowContacts_ButtonGroupMail(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B541;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B543;
        gHoverTextD68AE8 = kMailGroupHint58749C;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B542;
        WindowInstance* const window = gWindowStates[WindowId_GroupSelect].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        } else {
            gLoginWindowRuntime.prefixD2F0B8.value10 = 1;
            gLoginWindowRuntime.prefixD2F0B8.value00 = 0;
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailCompose, 1, 0, nullptr));
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B542;
    }
    return consumed;
}

// 原函数：0x004A2870。
std::int32_t WindowContacts_TabCategory(const std::int32_t index, const std::int32_t state)
{
    if (index < 10 || index > 12) {
        return 0;
    }
    const std::int32_t category = index - 10;
    if (gLoginWindowRuntime.valuesD67B40[6] == category) {
        return 0;
    }
    const std::int32_t idle = category == 0 ? 0x3B57D : (category == 1 ? 0x3B580 : 0x3C115);
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = idle + 2;
        const wchar_t* const hint = category == 0 ? kMailContactsHint5874B0
            : (category == 1 ? kMailGuildHint5874C4 : kMailMiniHint5874DC);
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        graphic->imageNo = idle;
    }
    if ((state & 2) != 0) {
        if (category == 2) {
            auto* const selected = static_cast<WindowGraphicData*>(gCurrentWindow->elements[12].data);
            selected->imageNo = 0x3C116;
        } else {
            auto* const title = static_cast<WindowGraphicData*>(gCurrentWindow->elements[108].data);
            title->imageNo = category == 0 ? 0x3B9DE : 0x3B9E3;
        }
        gCurrentWindow->elements[0].visible = category == 1 ? 1 : 0;
        gLoginWindowRuntime.valuesD67B40[6] = category;
        WindowContacts_ClampScroll();
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t mode = gValueD67B38;
        WindowContacts_SetDisplayMode(mode, currentCategory);
        consumed = 1;
    }
    return consumed;
}

// 原函数：0x004A2BD0。
std::int32_t WindowContacts_ButtonGuildDetails(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B9E5;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9E7;
        gHoverTextD68AE8 = kMailGuildDetailsHint587544;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B9E6;
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_GuildDetails, 1, 0, nullptr));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B9E6;
    }
    return consumed;
}

// 原函数：0x004A37E0。
std::int32_t WindowContacts_ResolveRecordIndex(const std::int32_t index)
{
    if (gValueD67B38 == 0) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t position = gLoginWindowRuntime.valuesD67B40[category];
        return gLoginWindowRuntime.mailIndicesC9FCE4[position + index - 51];
    }
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[3 + category];
    return gLoginWindowRuntime.mailIndicesC9FCE4[position + index - 99];
}

// 原函数：0x004A40C0。
std::int32_t WindowContacts_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) == 0) {
        return 0;
    }
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    std::int32_t count;
    switch (category) {
    case 0:
        count = 60;
        break;
    case 1:
        count = login::detail::gOrderedRecordCount1051510;
        if (count < 100) {
            count = 100;
        }
        break;
    case 2:
        count = 20;
        break;
    default:
        count = state;
        break;
    }
    const std::int32_t mode = gValueD67B38;
    const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[(mode == 0 ? 0 : 3) + category];
    const std::int32_t result = WindowScroll_ApplyWheel(
        &gCurrentWindow->elements[17], position, count - (mode == 0 ? 3 : 9), wheel);
    const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
    gLoginWindowRuntime.valuesD67B40[(mode == 0 ? 0 : 3) + currentCategory] = result;
    return 0;
}

// 原函数：0x004A2530。
std::int32_t WindowContacts_HandleRankButton(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t parity = (index - 21) % 2;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t imageOffset = parity != 0 ? 3 : 0;
    graphic->imageNo = 0x3B556 + imageOffset;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558 + imageOffset;
        gHoverTextD68AE8 = kMailRankHint587484;
    }
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        graphic->imageNo = 0x3B557 + imageOffset;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        WindowContacts_OpenRankSelection(index);
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B557 + imageOffset;
    }
    return consumed;
}

// 原函数：0x004A38C0。
void WindowContacts_OpenRankSelection(const std::int32_t index)
{
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::int32_t row = (index - 21) / 2;
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[category];
    const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[position + row];
    const std::int32_t key = login::detail::gOrderedRecords1051B14[recordIndex].value0C;
    const std::int32_t memberIndex = login::detail::FindActiveOrderedRecordIndex_5195C0(key);
    const std::int32_t currentCategory = gLoginWindowRuntime.valuesD67B40[6];
    gLoginWindowRuntime.mailMemberIndexCB1D18 = memberIndex;
    const std::int32_t currentPosition = gLoginWindowRuntime.valuesD67B40[currentCategory];
    const std::int32_t currentIndex = gLoginWindowRuntime.mailIndicesC9FCE4[currentPosition + row];
    gLoginWindowRuntime.mailMemberKeyD45F88 = login::detail::gOrderedRecords1051B14[currentIndex].value0C;
    gLoginWindowRuntime.mailRankCF4A14 = login::detail::gOrderedRecords1051B14[memberIndex].value00 - 1;
    static_cast<void>(WindowRuntime_Open(WindowId_GuildRank));
}

// 原函数：0x004A2B00。
std::int32_t WindowContacts_OpenMail(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B547;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B549;
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0:
            gHoverTextD68AE8 = kMailReadContactHint5874F4;
            break;
        case 1:
            gHoverTextD68AE8 = kMailReadGuildHint587510;
            break;
        case 2:
            gHoverTextD68AE8 = kMailReadMiniHint58752C;
            break;
        }
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B548;
        world::detail::gValueCF4FC0 = WindowContacts_ResolveButtonRecord(index);
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        gLoginWindowRuntime.mailCategoryD67B64 = category;
        gLoginWindowRuntime.mailCategoryCC4D50 = category;
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailRead, 1, 0, nullptr));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B548;
    }
    return consumed;
}

// 原函数：0x004A3780。
std::int32_t WindowContacts_ResolveButtonRecord(const std::int32_t index)
{
    if (gValueD67B38 == 0) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        const std::int32_t row = (index - 27) / 8;
        const std::int32_t position = gLoginWindowRuntime.valuesD67B40[category];
        return gLoginWindowRuntime.mailIndicesC9FCE4[position + row];
    }
    const std::int32_t row = (index - 54) / 5;
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[3 + category];
    return gLoginWindowRuntime.mailIndicesC9FCE4[position + row];
}

// 原函数：0x004A39C0。
std::int32_t WindowContacts_HandleAvatar(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[3 + category] + index - 99;
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t* recordImage = nullptr;
    switch (category) {
    case 0:
        if (position < 0 || position > 59) {
            return 0;
        }
        if ((state & 1) != 0) {
            const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[position];
            const auto& record = world::detail::gContactsF19FB0[recordIndex];
            if (record.used != 0) {
                recordImage = &record.value0C;
            }
        }
        break;
    case 1:
        if (position < 0) {
            return 0;
        }
        if (position > login::detail::gOrderedRecordCount1051510 - 1) {
            return 0;
        }
        if ((state & 1) != 0) {
            const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[position];
            const auto& record = login::detail::gOrderedRecords1051B14[recordIndex];
            if (record.value08 != 0) {
                recordImage = &record.value14;
            }
        }
        break;
    case 2:
        if ((state & 1) != 0) {
            const std::int32_t recordIndex = gLoginWindowRuntime.mailIndicesC9FCE4[position];
            const auto& record = world::detail::ReadMiniMailContact1075A90(recordIndex);
            if (record.used != 0) {
                recordImage = &record.value0C;
            }
        }
        break;
    }
    std::int32_t consumed = 0;
    if (recordImage != nullptr) {
        WindowInstance* const firstWindow = gCurrentWindow;
        const std::uint32_t color = 0xFFFFFFFFu;
        std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
        descriptor.bytes[13] = 4;
        WindowElement* const firstElements = firstWindow->elements;
        const std::int32_t firstWindowY = firstWindow->y;
        const std::int32_t firstElementY = firstElements[index].y;
        const std::int32_t firstElementX = firstElements[index].x;
        const std::int32_t firstY = firstElementY + firstWindowY + 55;
        const std::int32_t firstWindowX = firstWindow->x;
        static_cast<void>(login::detail::DrawGraphic(
            firstElementX + firstWindowX + 182, firstY, 223, 0x3BD32, 0, &descriptor));

        // 先固定第一次绘图后的元素；图号转换后仅重取当前窗口，不更换此元素。
        WindowElement* const element = &gCurrentWindow->elements[index];
        const std::int32_t image = character_select::detail::Sub_51BC50(*recordImage);
        const std::int32_t elementY = element->y;
        WindowInstance* const currentWindow = gCurrentWindow;
        const std::int32_t windowY = currentWindow->y;
        const std::int32_t y = elementY + windowY + 55;
        const std::int32_t elementX = element->x;
        const std::int32_t windowX = currentWindow->x;
        static_cast<void>(login::detail::DrawGraphic(
            elementX + windowX + 182, y, 223, image, 0, &descriptor));
        consumed = 1;
    }
    if ((state & 4) == 0) {
        return consumed;
    }
    switch (gLoginWindowRuntime.valuesD67B40[6]) {
    case 0: {
        const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
        if (world::detail::gContactsF19FB0[recordIndex].used == 0) {
            return 1;
        }
        static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 0, recordIndex));
        break;
    }
    case 1: {
        const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
        const auto& record = login::detail::gOrderedRecords1051B14[recordIndex];
        if (record.value08 == 0) {
            return 1;
        }
        const std::int32_t target = record.value0C;
        static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 1, target));
        break;
    }
    case 2: {
        const std::int32_t recordIndex = WindowContacts_ResolveRecordIndex(index);
        const auto& record = world::detail::ReadMiniMailContact1075A90(recordIndex);
        if (record.used == 0) {
            return 1;
        }
        const std::int32_t target = record.key04;
        static_cast<void>(network::Sub_511760(platform::gConnectionSocket, 2, target));
        break;
    }
    }
    return consumed;
}

// 原函数：0x004A3C70。
std::int32_t WindowContacts_ButtonCompose(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    graphic->imageNo = hover ? 0x3B546 : 0x3B544;
    const std::int32_t consumed = hover ? 1 : 0;
    bool nineRows;
    std::int32_t row;
    switch (index) {
    case 28: nineRows = false; row = 0; break;
    case 36: nineRows = false; row = 1; break;
    case 44: nineRows = false; row = 2; break;
    case 55: nineRows = true; row = 0; break;
    case 60: nineRows = true; row = 1; break;
    case 65: nineRows = true; row = 2; break;
    case 70: nineRows = true; row = 3; break;
    case 75: nineRows = true; row = 4; break;
    case 80: nineRows = true; row = 5; break;
    case 85: nineRows = true; row = 6; break;
    case 90: nineRows = true; row = 7; break;
    case 95: nineRows = true; row = 8; break;
    default: return consumed;
    }
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    if (!nineRows && (category < 0 || category > 2)) {
        return consumed;
    }
    if (hover) {
        switch (category) {
        case 0:
            gHoverTextD68AE8 = kMailWriteContactHint5876B0;
            break;
        case 1:
            gHoverTextD68AE8 = kMailWriteGuildHint5876CC;
            break;
        case 2:
            gHoverTextD68AE8 = kMailWriteMiniHint5876E4;
            break;
        }
    }
    if ((state & 2) == 0) {
        return consumed;
    }
    gLoginWindowRuntime.prefixD2F0B8.value10 = 0;
    gLoginWindowRuntime.prefixD2F0B8.value00 = category;
    const std::int32_t position = gLoginWindowRuntime.valuesD67B40[(nineRows ? 3 : 0) + category];
    gLoginWindowRuntime.prefixD2F0B8.value04 = gLoginWindowRuntime.mailIndicesC9FCE4[position + row];
    static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailCompose, 1, 0, nullptr));
    return 1;
}

// 原函数：0x004A32E0。
std::int32_t WindowContacts_ButtonDelete(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B54A;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B54C;
        switch (gLoginWindowRuntime.valuesD67B40[6]) {
        case 0:
            gHoverTextD68AE8 = kMailDeleteContactHint58755C;
            break;
        case 1:
            gHoverTextD68AE8 = kMailDeleteGuildHint58756C;
            break;
        case 2:
            gHoverTextD68AE8 = kMailDeleteMiniHint587584;
            break;
        }
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const std::int32_t recordIndex = WindowContacts_ResolveButtonRecord(index);
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        gLoginWindowRuntime.mailDeleteIndexD2DA8C = recordIndex;
        switch (category) {
        case 0: static_cast<void>(WindowContacts_ConfirmDeleteContact(recordIndex)); break;
        case 1: static_cast<void>(WindowContacts_ConfirmDeleteGuildMember(recordIndex)); break;
        case 2: static_cast<void>(WindowContacts_ConfirmDeleteMiniMail(recordIndex)); break;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B54B;
    }
    return consumed;
}

// 原函数：0x004A33C0。
login::YobiAction* WindowContacts_ConfirmDeleteContact(const std::int32_t index)
{
    WindowMail_InitializeDeleteConfirmation(&WindowMail_DeleteContact);
    gWindow47Runtime.textCC426C = std::format(kMailDeleteContactFormat58759C, cg::text::FromGbkWide(world::detail::gContactsF19FB0[index].name10.data()).c_str());
    return WindowRuntime_CreateAnimation(WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004A3480。
login::YobiAction* WindowContacts_ConfirmDeleteGuildMember(const std::int32_t index)
{
    WindowMail_InitializeDeleteConfirmation(&WindowMail_DeleteGuildMember);
    gWindow47Runtime.textCC426C = std::format(kMailDeleteGuildFormat5875F4, cg::text::FromGbkWide(login::detail::gOrderedRecords1051B14[index].text18).c_str());
    return WindowRuntime_CreateAnimation(WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004A3540。
login::YobiAction* WindowContacts_ConfirmDeleteMiniMail(const std::int32_t index)
{
    WindowMail_InitializeDeleteConfirmation(&WindowMail_DeleteMiniMailContact);
    gWindow47Runtime.textCC426C = std::format(kMailDeleteMiniFormat587654, cg::text::FromGbkWide(world::detail::ReadMiniMailContact1075A90(index).name10.data()).c_str());
    return WindowRuntime_CreateAnimation(WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004A2700。
void WindowContacts_ClampScroll()
{
    // 原无效模式/类别读取入口寄存器暂存；不补默认计数或模拟异常结果。
    std::int32_t maximum;
    std::int32_t position;
    const std::int32_t mode = gValueD67B38;
    if (mode == 0) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        if (gLoginWindowRuntime.valuesD67B40[category] < 0) {
            gLoginWindowRuntime.valuesD67B40[category] = 0;
        }
        switch (category) {
        case 0:
            maximum = 57;
            if (gLoginWindowRuntime.valuesD67B40[0] > maximum) {
                gLoginWindowRuntime.valuesD67B40[0] = maximum;
            }
            break;
        case 1: {
            const std::int32_t count = login::detail::gOrderedRecordCount1051510;
            maximum = count < 100 ? 97 : count - 3;
            if (gLoginWindowRuntime.valuesD67B40[1] > maximum) {
                gLoginWindowRuntime.valuesD67B40[1] = maximum;
            }
            break;
        }
        case 2:
            maximum = 17;
            if (gLoginWindowRuntime.valuesD67B40[2] > maximum) {
                gLoginWindowRuntime.valuesD67B40[2] = maximum;
            }
            break;
        }
        position = gLoginWindowRuntime.valuesD67B40[category];
    } else if (mode == 1) {
        const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
        if (gLoginWindowRuntime.valuesD67B40[3 + category] < 0) {
            gLoginWindowRuntime.valuesD67B40[3 + category] = 0;
        }
        switch (category) {
        case 0:
            maximum = 51;
            if (gLoginWindowRuntime.valuesD67B40[3] > maximum) {
                gLoginWindowRuntime.valuesD67B40[3] = maximum;
            }
            break;
        case 1: {
            const std::int32_t count = login::detail::gOrderedRecordCount1051510;
            maximum = count < 100 ? 91 : count - 9;
            if (gLoginWindowRuntime.valuesD67B40[4] > maximum) {
                gLoginWindowRuntime.valuesD67B40[4] = maximum;
            }
            break;
        }
        case 2:
            maximum = 11;
            if (gLoginWindowRuntime.valuesD67B40[5] > maximum) {
                gLoginWindowRuntime.valuesD67B40[5] = maximum;
            }
            break;
        }
        position = gLoginWindowRuntime.valuesD67B40[3 + category];
    }
    WindowScroll_SetPosition(&gCurrentWindow->elements[17], maximum, position);
}

// 原函数：0x004A2C60。
void WindowContacts_Sort()
{
    for (std::int32_t index = 0; index < 200; ++index) {
        gLoginWindowRuntime.mailIndicesC9FCE4[index] = index;
    }
    // 原异常类别的入口寄存器暂存不专项模拟，不补默认项数。
    std::uint32_t count;
    switch (gLoginWindowRuntime.valuesD67B40[6]) {
    case 0:
        count = 60;
        break;
    case 1:
        count = 200;
        break;
    case 2:
        count = 20;
        break;
    }
    WindowMail_SortIndices(gLoginWindowRuntime.mailIndicesC9FCE4.data(), count, WindowContacts_CompareRecords);
}

// 原函数：0x004A2CC0。
std::int32_t WindowContacts_CompareRecords(const std::int32_t* const left, const std::int32_t* const right)
{
    const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
    if (category == 0 || category == 1 || category == 2) {
        bool leftUsed;
        bool rightUsed;
        if (category == 0) {
            rightUsed = world::detail::gContactsF19FB0[*right].used != 0;
            leftUsed = world::detail::gContactsF19FB0[*left].used != 0;
        } else if (category == 1) {
            rightUsed = login::detail::gOrderedRecords1051B14[*right].value08 != 0;
            leftUsed = login::detail::gOrderedRecords1051B14[*left].value08 != 0;
        } else {
            rightUsed = world::detail::ReadMiniMailContact1075A90(*right).used != 0;
            leftUsed = world::detail::ReadMiniMailContact1075A90(*left).used != 0;
        }
        if (leftUsed != rightUsed) {
            return leftUsed ? -1 : 1;
        }
        if (!leftUsed) {
            return 0;
        }
        if (category == 2) {
            const std::int32_t rightIndex = *right;
            const std::int32_t leftIndex = *left;
            const auto* const other = &world::detail::ReadMiniMailContact1075A90(rightIndex);
            const auto* const current = &world::detail::ReadMiniMailContact1075A90(leftIndex);
            return WindowContacts_CompareMiniMailNumber(current, other);
        }
    }

    const std::uint32_t mode = platform::gValueD67CD0;
    if (mode > 4) {
        return 0;
    }
    const std::int32_t rightIndex = *right;
    if (category == 0) {
        const std::int32_t leftIndex = *left;
        const auto* const other = &world::detail::gContactsF19FB0[rightIndex];
        const auto* const current = &world::detail::gContactsF19FB0[leftIndex];
        switch (mode) {
        case 0:
            return WindowContacts_CompareContactName(current, other);
        case 1:
            return WindowContacts_CompareContactTitle(current, other);
        case 2:
            return WindowContacts_CompareContactValue(current, other);
        case 3:
            return WindowContacts_CompareRecordKey(&current->value04, &other->value04);
        case 4:
            return WindowContacts_CompareContactNumber(current, other);
        }
    } else {
        const std::int32_t leftIndex = *left;
        const auto* const other = &login::detail::gOrderedRecords1051B14[rightIndex];
        const auto* const current = &login::detail::gOrderedRecords1051B14[leftIndex];
        switch (mode) {
        case 0:
            return WindowContacts_CompareGuildName(current, other);
        case 1:
            return WindowContacts_CompareGuildValue(current, other);
        case 2:
            return WindowContacts_CompareGuildStatus(current, other);
        case 3:
            return WindowContacts_CompareRecordKey(&current->value04, &other->value04);
        case 4:
            return WindowContacts_CompareGuildNumber(current, other);
        }
    }
    return 0;
}

// 原函数：0x004A3010。
std::int32_t WindowContacts_CompareContactName(
    const world::detail::ContactRuntimeRecord* const left,
    const world::detail::ContactRuntimeRecord* const right)
{
    const int order = std::strcmp(left->name10.data(), right->name10.data());
    if (order != 0) {
        return order < 0 ? -1 : 1;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3070。
std::int32_t WindowContacts_CompareGuildName(
    const login::detail::OrderedRecord1051B14* const left,
    const login::detail::OrderedRecord1051B14* const right)
{
    const int order = std::strcmp(left->text18, right->text18);
    if (order != 0) {
        return order < 0 ? -1 : 1;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A30D0。
std::int32_t WindowContacts_CompareGuildValue(
    const login::detail::OrderedRecord1051B14* const left,
    const login::detail::OrderedRecord1051B14* const right)
{
    const std::int32_t leftValue = left->value00;
    const std::int32_t rightValue = right->value00;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3100。
std::int32_t WindowContacts_CompareContactTitle(
    const world::detail::ContactRuntimeRecord* const left,
    const world::detail::ContactRuntimeRecord* const right)
{
    const int order = std::strcmp(left->text32.data(), right->text32.data());
    if (order != 0) {
        return order < 0 ? -1 : 1;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3160。
std::int32_t WindowContacts_CompareContactValue(
    const world::detail::ContactRuntimeRecord* const left,
    const world::detail::ContactRuntimeRecord* const right)
{
    const std::int32_t leftValue = left->value08;
    const std::int32_t rightValue = right->value08;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3190。
std::int32_t WindowContacts_CompareGuildStatus(
    const login::detail::OrderedRecord1051B14* const left,
    const login::detail::OrderedRecord1051B14* const right)
{
    const std::int32_t leftValue = left->value10;
    const std::int32_t rightValue = right->value10;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A31C0。
std::int32_t WindowContacts_CompareRecordKey(
    const std::int32_t* const leftValue04, const std::int32_t* const rightValue04)
{
    const std::int32_t leftSequence = *leftValue04;
    const std::int32_t rightSequence = *rightValue04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A31E0。
std::int32_t WindowContacts_CompareContactNumber(
    const world::detail::ContactRuntimeRecord* const left,
    const world::detail::ContactRuntimeRecord* const right)
{
    const std::int16_t leftShort = left->value02;
    const std::int32_t leftValue = leftShort == 0 ? 0x7FFFFFFF : leftShort;
    const std::int16_t rightShort = right->value02;
    const std::int32_t rightValue = rightShort == 0 ? 0x7FFFFFFF : rightShort;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3230。
std::int32_t WindowContacts_CompareMiniMailNumber(
    const world::detail::MiniMailContactRecord* const left,
    const world::detail::MiniMailContactRecord* const right)
{
    const std::int32_t leftValue = std::bit_cast<std::int16_t>(left->sequence02);
    const std::int32_t rightValue = std::bit_cast<std::int16_t>(right->sequence02);
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->key04;
    const std::int32_t rightSequence = right->key04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3260。
std::int32_t WindowContacts_CompareGuildNumber(
    const login::detail::OrderedRecord1051B14* const left,
    const login::detail::OrderedRecord1051B14* const right)
{
    const std::int16_t leftShort = left->value0A;
    const std::int32_t leftValue = leftShort == 0 ? 0x7FFFFFFF : leftShort;
    const std::int16_t rightShort = right->value0A;
    const std::int32_t rightValue = rightShort == 0 ? 0x7FFFFFFF : rightShort;
    const std::int32_t order = (leftValue > rightValue) - (leftValue < rightValue);
    if (order != 0) {
        return order;
    }
    const std::int32_t leftSequence = left->value04;
    const std::int32_t rightSequence = right->value04;
    return (leftSequence > rightSequence) - (leftSequence < rightSequence);
}

// 原函数：0x004A3820。
std::int32_t WindowContacts_DragScroll(const std::int32_t)
{
    // 类别0/1/2均有真实计数；无效类别的入口寄存器值不专项模拟。
    std::int32_t count;
    switch (gLoginWindowRuntime.valuesD67B40[6]) {
    case 0:
        count = 60;
        break;
    case 1:
        count = login::detail::gOrderedRecordCount1051510;
        if (count < 100) {
            count = 100;
        }
        break;
    case 2:
        count = 20;
        break;
    }
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const mouse = static_cast<WindowMouseData*>(elements[17].data);
    if ((mouse->flags & 1) != 0) {
        if (gValueD67B38 == 0) {
            count = WindowScroll_ReadPosition(&elements[17], count - 3);
            const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
            gLoginWindowRuntime.valuesD67B40[category] = count;
        } else {
            count = WindowScroll_ReadPosition(&elements[17], count - 9);
            const std::int32_t category = gLoginWindowRuntime.valuesD67B40[6];
            gLoginWindowRuntime.valuesD67B40[3 + category] = count;
        }
    }
    return count;
}

} // namespace detail

} // namespace cg::ui
