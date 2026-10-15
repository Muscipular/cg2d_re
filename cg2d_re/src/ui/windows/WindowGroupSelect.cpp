#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x004B4B00。
std::int32_t WindowGroupSelect_Update(const std::int32_t value)
{
    if (value == -1) {
        if (gLoginWindowRuntime.mailGroupCC1A30.initialize == 0) {
            gLoginWindowRuntime.mailGroupScrollD67B88 = 0;
            WindowGroupSelect_ResetRecipients();
            gLoginWindowRuntime.mailGroupCC1A30.mode = 0;
            WindowGroupSelect_BuildRecipients(0);
            gCurrentWindow->elements[19].visible = 1;
        }
        gLoginWindowRuntime.mailGroupCC1A30.initialize = 0;
        return 1;
    }
    if (gWindowStates[WindowId_MailCompose].window == nullptr) {
        gWindowStates[WindowId_GroupSelect].window->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x004B4B70。
void WindowGroupSelect_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_MailCompose].window;
    if (window != nullptr && gLoginWindowRuntime.prefixD2F0B8.value10 == 1) {
        window->state |= 0x8000u;
    }
}

// 原函数：0x004B4B90。
std::int32_t WindowGroupSelect_Draw(const std::int32_t value)
{
    static_cast<void>(WindowGroupSelect_DragScroll(value));
    WindowGroupSelect_Refresh();
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004B4BB0。
std::int32_t WindowGroupSelect_HandlePageButtons(const std::int32_t index, const std::int32_t state)
{
    if (index != 6 && index != 7) {
        return 0;
    }
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t idle = index == 6 ? 0x3B556 : 0x3B559;
    graphic->imageNo = idle;
    std::int32_t result = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = idle + 2;
        result = 1;
    }
    if ((state & 2) != 0) {
        graphic->imageNo = idle + 1;
        const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
        gLoginWindowRuntime.mailGroupScrollD67B88 = position + (index == 6 ? -7 : 7);
        static_cast<void>(WindowGroupSelect_ClampScroll());
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if ((state & 8) != 0) {
        graphic->imageNo = idle + 1;
    }
    return result;
}

// 原函数：0x004B4CA0。
std::int32_t WindowGroupSelect_HandleScrollButtons(const std::int32_t index, const std::int32_t state)
{
    if (index != 1 && index != 2) {
        return 0;
    }
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t idle = index == 1 ? 0x3B592 : 0x3B595;
    graphic->imageNo = idle;
    std::int32_t result = 0;
    if ((state & 1) != 0) {
        graphic->imageNo = idle + 2;
        result = 1;
    }
    if ((state & 0x400) != 0) {
        graphic->imageNo = idle + 1;
        std::int32_t sound = 51;
        if (index == 1) {
            const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88 - 1;
            gLoginWindowRuntime.mailGroupScrollD67B88 = position;
            if (position < 0) {
                gLoginWindowRuntime.mailGroupScrollD67B88 = 0;
                sound = 55;
            }
        } else {
            const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
            const std::int32_t maximum = gLoginWindowRuntime.mailGroupCC1A30.count - 7;
            gLoginWindowRuntime.mailGroupScrollD67B88 = position + 1;
            if (position + 1 > maximum) {
                gLoginWindowRuntime.mailGroupScrollD67B88 = maximum;
                sound = 55;
            }
        }
        static_cast<void>(audio::PlaySe_52C450(sound, 320));
        static_cast<void>(WindowGroupSelect_ClampScroll());
        result = 1;
    }
    return result;
}

// 原函数：0x004B4DB0。
std::int32_t WindowGroupSelect_ClampScroll()
{
    std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
    if (position < 0) {
        position = 0;
        gLoginWindowRuntime.mailGroupScrollD67B88 = position;
    }
    const std::int32_t maximum = gLoginWindowRuntime.mailGroupCC1A30.count - 7;
    if (position > maximum) {
        position = maximum;
        gLoginWindowRuntime.mailGroupScrollD67B88 = position;
    }
    return WindowScroll_SetPosition(&gCurrentWindow->elements[4], maximum, position);
}

// 原函数：0x004B4DF0。
std::int32_t WindowGroupSelect_DragScroll(const std::int32_t)
{
    const std::int32_t count = gLoginWindowRuntime.mailGroupCC1A30.count;
    WindowInstance* const window = gCurrentWindow;
    const std::int32_t maximum = count - 7;
    const std::int32_t position = WindowScroll_ReadPosition(&window->elements[4], maximum < 0 ? 0 : maximum);
    gLoginWindowRuntime.mailGroupScrollD67B88 = position;
    return position;
}

// 原函数：0x004B4E20。
void WindowGroupSelect_Refresh()
{
    const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode;
    if (mode != 0 && mode != 1 && mode != 2) {
        return;
    }
    WindowInstance* const titleWindow = gCurrentWindow;
    const std::wstring& title = mode == 0 ? gWindowText572F78.text
        : mode == 1 ? gWindowText572F80.text : gWindowText572F88.text;
    auto* const titleData = static_cast<WindowTextData*>(titleWindow->elements[10].data);
    titleData->text = title;
    WindowInstance* const subtitleWindow = gCurrentWindow;
    const std::wstring& subtitle = mode == 0 ? gWindowText572F58.text
        : mode == 1 ? gWindowText572F60.text : gWindowText572F68.text;
    auto* const subtitleData = static_cast<WindowTextData*>(subtitleWindow->elements[11].data);
    subtitleData->text = subtitle;
    for (std::int32_t row = 0; row < 7; ++row) {
        const std::int32_t scroll = gLoginWindowRuntime.mailGroupScrollD67B88;
        const std::int32_t position = scroll + row;
        const std::int32_t source = gLoginWindowRuntime.mailGroupCC1A30.indices[position];
        const std::int32_t visible = source != -1 ? 1 : 0;
        if (visible != 0) {
            WindowInstance* const window = gCurrentWindow;
            const char* const name = mode == 0 ? world::detail::gContactsF19FB0[source].name10.data()
                : mode == 1 ? login::detail::gOrderedRecords1051B14[source].text18
                : login::detail::BorrowGuildRankText1051518(position);
            auto* const text = static_cast<WindowTextData*>(window->elements[12 + row].data);
            text->text = cg::text::FromGbkWide(name);
        }
        gCurrentWindow->elements[12 + row].visible = visible;
        gCurrentWindow->elements[20 + row].visible = visible;
        gCurrentWindow->elements[28 + row].visible = visible;
    }
}

// 原函数：0x004B5210。
std::int32_t WindowGroupSelect_TabCategory(const std::int32_t index, const std::int32_t state)
{
    if (index != 8 && index != 9) {
        return 0;
    }
    const std::int32_t idle = index == 8 ? 0x3B556 : 0x3B559;
    std::int32_t result = 0;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if ((state & 1) != 0) {
        graphic->imageNo = idle + 2;
        gHoverTextD68AE8 = L"切換集團郵件。";
        result = 1;
    } else {
        graphic->imageNo = idle;
    }
    if ((state & 2) != 0) {
        if (index == 8) {
            const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode - 1;
            gLoginWindowRuntime.mailGroupCC1A30.mode = mode;
            if (mode < 0) {
                gLoginWindowRuntime.mailGroupCC1A30.mode = 2;
            }
            WindowGroupSelect_BuildRecipients(gLoginWindowRuntime.mailGroupCC1A30.mode);
        } else {
            std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode + 1;
            gLoginWindowRuntime.mailGroupCC1A30.mode = mode;
            if (mode > 2) {
                mode = 0;
                gLoginWindowRuntime.mailGroupCC1A30.mode = mode;
            }
            WindowGroupSelect_BuildRecipients(mode);
        }
        gLoginWindowRuntime.mailGroupScrollD67B88 = 0;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        result = 1;
    }
    if ((state & 8) != 0) {
        auto* const current = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        current->imageNo = idle + 1;
    }
    return result;
}

// 原函数：0x004B53C0。
std::int32_t WindowGroupSelect_ButtonSelectAll(const std::int32_t index, const std::int32_t state)
{
    std::int32_t result = 0;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3BA20;
        const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode;
        if (mode == 0 || mode == 1 || mode == 2) {
            gHoverTextD68AE8 = L"準備寄送郵件給此集團。";
        }
        result = 1;
    } else {
        graphic->imageNo = 0x3BA1E;
    }
    if ((state & 2) != 0) {
        const std::int32_t mode = gLoginWindowRuntime.mailGroupCC1A30.mode;
        if (mode == 0 || mode == 1 || mode == 2) {
            std::int32_t count = gLoginWindowRuntime.mailGroupCC1A30.count;
            for (std::int32_t row = 0; row < count; ++row) {
                const std::int32_t source = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                if (source != -1) {
                    if (mode == 2) {
                        gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 1;
                    } else if (mode == 1) {
                        if (login::detail::gOrderedRecords1051B14[source].value0A != 0) {
                            gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 1;
                        }
                    } else {
                        // 原普通分支查CC2E6C排序表，不用CC1A38名单值取联系人。
                        const std::int32_t ordered = gLoginWindowRuntime.mailGroupOrderCC2E6C[row];
                        if (world::detail::gContactsF19FB0[ordered].value02 != 0) {
                            gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 1;
                        }
                    }
                }
                count = gLoginWindowRuntime.mailGroupCC1A30.count;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        auto* const current = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        current->imageNo = 0x3BA1F;
    }
    return result;
}

// 原函数：0x004B5550。
std::int32_t WindowGroupSelect_SelectRow(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t scroll = gLoginWindowRuntime.mailGroupScrollD67B88;
    const std::int32_t selected = gLoginWindowRuntime.mailGroupCC1A30.selected[scroll + index - 20];
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = selected == 1 ? 0x3BA21 : 0x3BA22;
    std::int32_t result = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = selected == 1
            ? L"對方的郵件寄過來了。"
            : L"準備寄送郵件給對方。";
        result = 1;
    }
    if ((state & 2) != 0) {
        const std::int32_t currentScroll = gLoginWindowRuntime.mailGroupScrollD67B88;
        const std::int32_t row = currentScroll + index - 20;
        const std::int32_t current = gLoginWindowRuntime.mailGroupCC1A30.selected[row];
        if (current != 0) {
            gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 0;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            gLoginWindowRuntime.mailGroupCC1A30.selected[row] = 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    return result;
}

// 原函数：0x004B5A40。
std::int32_t WindowGroupSelect_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gLoginWindowRuntime.mailGroupCC1A30.count;
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        const std::int32_t maximum = count - 7;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t position = gLoginWindowRuntime.mailGroupScrollD67B88;
        gLoginWindowRuntime.mailGroupScrollD67B88 =
            WindowScroll_ApplyWheel(&window->elements[4], position, maximum < 0 ? 0 : maximum, wheel);
    }
    return 0;
}

// 原函数：0x004B5630。
void WindowGroupSelect_ResetRecipients()
{
    gLoginWindowRuntime.mailGroupCC1A30.count = 0;
    gLoginWindowRuntime.mailGroupCC1A30.mode = 0;
    for (std::int32_t index = 0; index < 200; ++index) {
        gLoginWindowRuntime.mailGroupCC1A30.indices[index] = 0;
        gLoginWindowRuntime.mailGroupCC1A30.selected[index] = 0;
    }
}

// 原函数：0x004B5660。
void WindowGroupSelect_BuildRecipients(const std::int32_t mode)
{
    gLoginWindowRuntime.mailGroupCC1A30.count = 0;
    gLoginWindowRuntime.mailGroupCC1A30.mode = mode;
    for (std::int32_t index = 0; index < 200; ++index) {
        gLoginWindowRuntime.mailGroupCC1A30.indices[index] = -1;
        gLoginWindowRuntime.mailGroupCC1A30.selected[index] = 0;
    }
    switch (mode) {
    case 0:
        WindowGroupSelect_SortRecipients(0);
        for (std::int32_t index = 0; index < 60; ++index) {
            const std::int32_t source = gLoginWindowRuntime.mailGroupOrderCC2E6C[index];
            if (world::detail::gContactsF19FB0[source].used == 1) {
                gLoginWindowRuntime.mailGroupCC1A30.indices[index] = source;
                ++gLoginWindowRuntime.mailGroupCC1A30.count;
            }
        }
        break;
    case 1:
        WindowGroupSelect_SortRecipients(1);
        for (std::int32_t index = 0; index < 200; ++index) {
            const std::int32_t source = gLoginWindowRuntime.mailGroupOrderCC2E6C[index];
            if (login::detail::gOrderedRecords1051B14[source].value08 == 1) {
                gLoginWindowRuntime.mailGroupCC1A30.indices[index] = source;
                ++gLoginWindowRuntime.mailGroupCC1A30.count;
            }
        }
        break;
    case 2:
        for (std::int32_t index = 0; index < 32; ++index) {
            if (login::detail::ReadGuildRankFlags1051514(index) != 0) {
                gLoginWindowRuntime.mailGroupCC1A30.indices[index] = index;
                ++gLoginWindowRuntime.mailGroupCC1A30.count;
            }
        }
        break;
    }
}

// 原函数：0x004B5760。
void WindowGroupSelect_SortRecipients(const std::int32_t category)
{
    gLoginWindowRuntime.mailGroupSortCategoryCB0600 = category;
    for (std::int32_t index = 0; index < 200; ++index) {
        gLoginWindowRuntime.mailGroupOrderCC2E6C[index] = index;
    }
    WindowMail_SortIndices(gLoginWindowRuntime.mailGroupOrderCC2E6C.data(),
        category == 0 ? 60u : 200u, WindowGroupSelect_CompareRecipients);
}

// 原函数：0x004B57B0。
std::int32_t WindowGroupSelect_CompareRecipients(const std::int32_t* const left, const std::int32_t* const right)
{
    const std::int32_t category = gLoginWindowRuntime.mailGroupSortCategoryCB0600;
    const std::int32_t rightIndex = *right;
    const std::int32_t leftIndex = *left;
    const bool leftUsed = category == 0
        ? world::detail::gContactsF19FB0[leftIndex].used != 0
        : login::detail::gOrderedRecords1051B14[leftIndex].value08 != 0;
    const bool rightUsed = category == 0
        ? world::detail::gContactsF19FB0[rightIndex].used != 0
        : login::detail::gOrderedRecords1051B14[rightIndex].value08 != 0;
    if (leftUsed != rightUsed) {
        return leftUsed ? -1 : 1;
    }
    if (!leftUsed) {
        return 0;
    }
    const std::uint32_t mode = platform::gValueD67CD0;
    if (mode > 4) {
        return 0;
    }
    if (category == 0) {
        const auto* const other = &world::detail::gContactsF19FB0[rightIndex];
        const auto* const current = &world::detail::gContactsF19FB0[leftIndex];
        switch (mode) {
        case 0: return WindowContacts_CompareContactName(current, other);
        case 1: return WindowContacts_CompareContactTitle(current, other);
        case 2: return WindowContacts_CompareContactValue(current, other);
        case 3: return WindowContacts_CompareRecordKey(&current->value04, &other->value04);
        case 4: return WindowContacts_CompareContactNumber(current, other);
        }
    } else {
        const auto* const other = &login::detail::gOrderedRecords1051B14[rightIndex];
        const auto* const current = &login::detail::gOrderedRecords1051B14[leftIndex];
        switch (mode) {
        case 0: return WindowContacts_CompareGuildName(current, other);
        case 1: return WindowContacts_CompareGuildValue(current, other);
        case 2: return WindowContacts_CompareGuildStatus(current, other);
        case 3: return WindowContacts_CompareRecordKey(&current->value04, &other->value04);
        case 4: return WindowContacts_CompareGuildNumber(current, other);
        }
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
