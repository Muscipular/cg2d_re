#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

std::array<WindowCrystalItem, 20> gWindow69ItemsCB2230 {}; // 原全局：0x00CB2230。

std::int32_t gWindow69ValueD67B98 = 0; // 原全局：0x00D67B98。

std::uint8_t gWindow69FlagD2EA5B = 0; // 原全局：0x00D2EA5B。

std::int32_t gWindow69ImageCB01F0 = 0; // 原全局：0x00CB01F0。

std::int32_t gWindow69DialogTypeD2D810 = 0; // 原全局：0x00D2D810。

std::int32_t gWindow69ScrollD2D604 = 0; // 原全局：0x00D2D604。

std::int32_t gWindow69DirtyD67B9C = 0; // 原全局：0x00D67B9C。

std::int32_t gWindow69CachedScroll580858 = -1; // 原全局：0x00580858。

std::int32_t gWindow69TotalD45E70 = 0; // 原全局：0x00D45E70。

std::int32_t gWindow69SlotsCA0AC8 = 0; // 原全局：0x00CA0AC8。

std::array<std::wstring, 5> gWindow69PriceTextsD2D7CC {}; // 原全局：0x00D2D7CC。

std::array<std::wstring, 5> gWindow69CountTextsD2EA24 {}; // 原全局：0x00D2EA24。

std::wstring gWindow69TotalTextCC48B0 {}; // 原全局：0x00CC48B0。

std::int32_t gWindow69Tooltip580850 = -1; // 原全局：0x00580850。

std::int32_t gWindow69Message580854 = -1; // 原全局：0x00580854。

std::int32_t gWindow69TooltipPageD67B94 = 0; // 原全局：0x00D67B94。

std::int32_t gWindow69InventoryHover58085C = -2; // 原全局：0x0058085C。

std::wstring gWindow69BalanceTextD2E448 {}; // 原全局：0x00D2E448。

WindowValueCallback gWindow69ConfirmCallbackCF4E48 = nullptr; // 原全局：0x00CF4E48。

std::array<WindowConfirmationItem, 20> gWindow69ConfirmItemsD2E458 {}; // 原全局：0x00D2E458。

std::array<WindowConfirmationAmount, 2> gWindow69ConfirmAmountsD2E2B8 {}; // 原全局：0x00D2E2B8。

std::wstring gWindow69ConfirmHeadingCC3F68 {}; // 原全局：0x00CC3F68。

std::wstring gWindow69ConfirmQuestionCC3F04 {}; // 原全局：0x00CC3F04。

std::wstring gWindow69ConfirmAcceptD2E81C {}; // 原全局：0x00D2E81C。

std::wstring gWindow69ConfirmCancelCB076C {}; // 原全局：0x00CB076C。

static_assert(sizeof(gWindow69ItemsCB2230) == 0xF690);

// 原函数：0x004C1390。
void WindowCrystalShop_Release()
{
    const std::int32_t type = gWindow69DialogTypeD2D810;
    if (type == world::detail::gValue581800) {
        world::detail::gValue581800 = -1;
    }
}

// 原函数：0x004C13B0。
login::YobiAction* WindowCrystalShop_Open(const char* const text)
{
    gWindow69ValueD67B98 = 0;
    gWindow69FlagD2EA5B = 0;
    gWindow69ImageCB01F0 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char titleGbkBuffer[3072]{};
    char* const title = titleGbkBuffer;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gNpcDialogLinesCC24D4[0].text = cg::text::FromGbkWide(title);
    std::array<char, 2304> buffer;
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            static_cast<void>(input::ExtractGbkTextLine(gNpcDialogLinesCC24D4[group * 4 + row + 1].text, buffer.data(), row, 20));
        }
    }
    gShopItemCountCB1E10 = 0;
    std::memset(gWindow69ItemsCB2230.data(), 0, sizeof(gWindow69ItemsCB2230));
    std::int32_t field = 8;
    for (WindowCrystalItem* item = gWindow69ItemsCB2230.data();
         item != gWindow69ItemsCB2230.data() + gWindow69ItemsCB2230.size(); ++item, field += 7) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field - 2, 767, buffer.data()) == 1) {
            break;
        }
        gWindow69ItemsCB2230[gShopItemCountCB1E10].used = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        if (std::strlen(buffer.data()) > 28) {
            std::strcpy(item->name.data(), "???");
        } else {
            std::strcpy(gWindow69ItemsCB2230[gShopItemCountCB1E10].name.data(), buffer.data());
        }
        item->imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field - 1);
        item->valueC24 = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        item->limitC4C = network::ParseDelimitedInteger_52CFA0(text, '|', field + 1);
        item->valueC2C = network::ParseDelimitedInteger_52CFA0(text, '|', field + 2);
        const std::int32_t remaining = network::ParseDelimitedInteger_52CFA0(text, '|', field + 3);
        item->remainingC50 = remaining;
        if (remaining != -1) {
            WindowCrystalItem& current = gWindow69ItemsCB2230[gShopItemCountCB1E10];
            if (remaining < current.limitC4C) {
                current.limitC4C = current.remainingC50;
            }
        }
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field + 4, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::int32_t currentCount = gShopItemCountCB1E10;
        for (std::int32_t row = 0; row < 32; ++row) {
            const std::int32_t copied = input::ExtractLegacyLine_503AB0(
                gWindow69ItemsCB2230[currentCount].description.data() + row * 96,
                3072, buffer.data(), row, 32);
            currentCount = gShopItemCountCB1E10;
            if (copied == 0) {
                gWindow69ItemsCB2230[currentCount].description[row * 96] = '\0';
            }
        }
        gShopItemCountCB1E10 = currentCount + 1;
    }
    const std::int32_t type = world::detail::gValue581800;
    gValueD2D608 = -1;
    gWindow69DialogTypeD2D810 = type;
    return WindowRuntime_CreateAnimation(WindowId_CrystalShop, 1, 0, nullptr);
}

// 原函数：0x004C1700。
void WindowCrystalShop_RefreshRemaining()
{
    for (std::int32_t row = 0; row < 5; ++row) {
        gCurrentWindow->elements[row + 31].visible = 0;
        const WindowCrystalItem& item = gWindow69ItemsCB2230[gWindow69ScrollD2D604 + row];
        if (item.used != 0 && item.remainingC50 >= 0) {
            gCurrentWindow->elements[row + 31].visible = 1;
            const std::int32_t index = gWindow69ScrollD2D604 + row;
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t remaining = gWindow69ItemsCB2230[index].remainingC50;
            cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[row + 31].data)->text, L"剩餘 {:2} 個", remaining);
        }
    }
}

// 原函数：0x004C17B0。
void WindowCrystalShop_RefreshRows()
{
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        WindowInstance* const initial = gCurrentWindow;
        std::wstring& price = gWindow69PriceTextsD2D7CC[row];
        price.clear();
        static_cast<WindowTextData*>(initial->elements[row + 16].data)->text.clear();
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 21].data)->text.clear();
        gCurrentWindow->elements[row + 26].visible = 0;
        const WindowCrystalItem* item = &gWindow69ItemsCB2230[gWindow69ScrollD2D604 + row];
        if (item->used == 0) {
            continue;
        }
        cg::text::FormatTo(price, L"{:10}", item->valueC24);
        item = &gWindow69ItemsCB2230[gWindow69ScrollD2D604 + row];
        if ((item->flags & 0x100u) != 0) {
            buffer = cg::text::FromGbkWide(item->tips.data());
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 16].data)->bytes00[0] =
                std::byte{5};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 21].data)->bytes00[0] =
                std::byte{5};
        } else {
            const std::int32_t stack = item->valueC2C;
            if (stack > 1) {
                static_cast<void>(cg::text::FormatTo(buffer, L"{}({}{})", cg::text::FromGbkWide(item->name.data()).c_str(), L"堆疊數", stack));
            } else {
                static_cast<void>(cg::text::FormatTo(buffer, L"{}({})", cg::text::FromGbkWide(item->name.data()).c_str(), L"不可堆疊"));
            }
            const std::uint32_t flags =
                gWindow69ItemsCB2230[gWindow69ScrollD2D604 + row].flags;
            const std::byte color = (flags & 0x200u) != 0 ? std::byte{1} : std::byte{0};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 16].data)->bytes00[0] = color;
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 21].data)->bytes00[0] = color;
        }
        WindowRuntime_FillTwoTextLines(static_cast<WindowTextData*>(gCurrentWindow->elements[row + 16].data), static_cast<WindowTextData*>(gCurrentWindow->elements[row + 21].data), buffer);
    }
}

// 原函数：0x004C1AA0。
void WindowCrystalShop_RefreshQuantities()
{
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t scroll = gWindow69ScrollD2D604;
        std::wstring& text = gWindow69CountTextsD2EA24[row];
        text.clear();
        const WindowCrystalItem& item = gWindow69ItemsCB2230[scroll + row];
        if (item.used != 0) {
            cg::text::FormatTo(text, L"{:2}", item.selectedCount);
        }
    }
}

// 原函数：0x004C1B00。
void WindowCrystalShop_RefreshAmount()
{
    std::int32_t total = 0;
    gWindow69TotalD45E70 = 0;
    for (const WindowCrystalItem& item : gWindow69ItemsCB2230) {
        if (item.used != 0) {
            const std::int32_t quantity = item.selectedCount;
            total += quantity * item.valueC24;
        }
    }
    gWindow69TotalD45E70 = total;
    cg::text::FormatTo(gWindow69TotalTextCC48B0, L"{:10}", total);
    const std::int32_t currentTotal = gWindow69TotalD45E70;
    const std::int32_t owned = world::detail::ReadPlayerValue15BCC();
    auto* const number = static_cast<WindowNumberData*>(gCurrentWindow->elements[6].data);
    number->color = currentTotal <= owned ? 10 : 6;
}

// 原函数：0x004C1B70。
void WindowCrystalShop_CountSlots()
{
    std::int32_t slots = 0;
    gWindow69SlotsCA0AC8 = 0;
    for (const WindowCrystalItem& item : gWindow69ItemsCB2230) {
        if (item.used != 0) {
            std::int32_t stack = item.valueC2C;
            const std::int32_t quantity = item.selectedCount;
            if (stack <= 0) {
                stack = 2;
            }
            slots += (stack + quantity - 1) / stack;
        }
    }
    gWindow69SlotsCA0AC8 = slots;
}

// 原函数：0x004C1BC0。
void WindowCrystalShop_Refresh()
{
    const std::int32_t initialized = gWindow69ValueD67B98;
    const std::int32_t scroll = gWindow69ScrollD2D604;
    if (initialized != 0 && gWindow69CachedScroll580858 == scroll) {
        if (gWindow69DirtyD67B9C != 0) {
            gWindow69DirtyD67B9C = 0;
            WindowCrystalShop_RefreshAmount();
            WindowCrystalShop_RefreshQuantities();
            WindowCrystalShop_CountSlots();
        }
        return;
    }
    gWindow69ValueD67B98 = 1;
    gWindow69CachedScroll580858 = scroll;
    WindowCrystalShop_RefreshRows();
    WindowCrystalShop_RefreshAmount();
    WindowCrystalShop_RefreshQuantities();
    WindowCrystalShop_CountSlots();
    WindowCrystalShop_RefreshRemaining();
    const std::int32_t position = gWindow69ScrollD2D604;
    WindowInstance* const window = gCurrentWindow;
    const std::int32_t maximum = gShopItemCountCB1E10 - 5;
    static_cast<void>(WindowScroll_SetPosition(window->elements + 10, maximum, position));
}

// 原函数：0x004C1C50。
std::int32_t WindowCrystalShop_Update(const std::int32_t value)
{
    WindowInstance* const entryWindow = gCurrentWindow;
    if (value == -1) {
        gWindow69ScrollD2D604 = 0;
        gShopAvailableCF4A24 = -1;
        entryWindow->elements[0].visible = 0;
        for (std::int32_t row = 0; row < 5; ++row) {
            gCurrentWindow->elements[16 + row].visible = 1;
            gCurrentWindow->elements[21 + row].visible = 1;
            gCurrentWindow->elements[26 + row].visible = 0;
            gCurrentWindow->elements[36 + row].visible = 1;
            gCurrentWindow->elements[41 + row].visible = 1;
            gCurrentWindow->elements[46 + row].visible = 1;
            gCurrentWindow->elements[51 + row].visible = 1;
            gCurrentWindow->elements[56 + row].visible = 1;
            gCurrentWindow->elements[61 + row].visible = 1;
            gCurrentWindow->elements[56 + row].mouseCallback = &WindowCrystalShop_HandleItemIcon;
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[61 + row].data)->imageNo =
                98100;
        }
        gCurrentWindow->elements[4].visible = 0;
        WindowInstance* const window = gCurrentWindow;
        gShopRowKeysC9A954[0] = -2;
        gWindow69Tooltip580850 = -1;
        gShopRowKeysC9A954[1] = -2;
        gWindow69Message580854 = -1;
        gShopRowKeysC9A954[2] = -2;
        gWindow69TooltipPageD67B94 = 0;
        gShopRowKeysC9A954[3] = -2;
        gShopRowKeysC9A954[4] = -2;
        auto* const title = static_cast<WindowTextData*>(window->elements[5].data);
        title->text = gNpcDialogLinesCC24D4[0].text;
        auto* animation = static_cast<WindowAnimationData*>(gCurrentWindow->elements[8].data);
        const std::int32_t imageNo = gWindow69ImageCB01F0;
        login::detail::SetYobiActionField2A8(animation->action, imageNo);
        animation = static_cast<WindowAnimationData*>(gCurrentWindow->elements[8].data);
        login::detail::SetYobiActionField2B8(animation->action, 5);
    }
    if (WindowProfile_CheckDistance(2, 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
    }
    const std::int32_t type = gWindow69DialogTypeD2D810;
    if (type != world::detail::gValue581800) {
        gCurrentWindow->state |= 0x8000u;
    }
    gCurrentWindow->elements[9].visible = 1;
    if (gShopItemCountCB1E10 - 5 <= 0) {
        gCurrentWindow->elements[9].visible = 0;
    }
    WindowElement* const elements = gCurrentWindow->elements;
    if ((static_cast<const WindowMouseData*>(elements[10].data)->flags & 1u) != 0) {
        gWindow69ScrollD2D604 = WindowScroll_ReadPosition(elements + 10, gShopItemCountCB1E10 - 5);
    }
    cg::text::FormatTo(gWindow69BalanceTextD2E448, L"{:10}", world::detail::ReadPlayerValue15BCC());
    if (gWindow69Message580854 >= 0) {
        gWindow69Message580854 = -1;
    }
    gWindow69Tooltip580850 = -1;
    return 1;
}

// 原函数：0x004C1EC0。
std::int32_t WindowCrystalShop_Draw(const std::int32_t unused)
{
    static_cast<void>(unused);
    const WindowInstance* const window = gCurrentWindow;
    const std::int32_t x = window->x + 298;
    std::int32_t y = window->y + 50;
    const std::int32_t tooltip = gWindow69Tooltip580850;
    if (tooltip >= 0) {
        if (tooltip < 100) {
            static_cast<void>(WindowItem_DrawEquipmentTooltip(tooltip, gWindow69TooltipPageD67B94));
        } else if (tooltip < 200) {
            const std::uint8_t flag = gWindow69FlagD2EA5B;
            const std::int32_t page = gWindow69TooltipPageD67B94;
            static_cast<void>(WindowCrystalShop_ShowTooltip(tooltip - 100, page, flag == 0));
        } else {
            static_cast<void>(WindowItem_DrawWorkTooltip(gWindow69TooltipPageD67B94));
        }
    }
    const std::int32_t message = gWindow69Message580854;
    gTooltipColorD67D30 = 0;
    const WindowDialogTextLine* line = gNpcDialogLinesCC24D4.data() + 1 + 4 * (-1 - message);
    for (std::int32_t row = 0; row < 4; ++row) {
        if (!line->text.empty()) {
            static_cast<void>(WindowItem_SplitTooltipLines(x, y, 3, 8, 0, line->text.data(), 0, 0));
        }
        ++line;
        y += 10;
    }
    WindowCrystalShop_Refresh();
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004C2060。
void WindowCrystalShop_Submit(const std::int32_t value)
{
    if (static_cast<std::uint8_t>(value) != 0) {
        return;
    }
    std::wstring fragment;
    std::wstring text;
    std::wstring escaped;
    text.clear();
    std::int32_t count = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const WindowCrystalItem& item = gWindow69ItemsCB2230[index];
        if (item.used != 0) {
            const std::int32_t quantity = item.selectedCount;
            if (quantity != 0) {
                if (count > 0) {
                    text += L"|";
                }
                cg::text::FormatTo(fragment, L"{}|{}", index, quantity);
                text += fragment;
                ++count;
            }
        }
    }
    escaped = input::EscapeTextAndNotify(text, 255);
    const std::int32_t target = world::detail::gDialogTargetD2DE6C;
    const std::int32_t request = world::detail::gDialogRequestCC21E0;
    const std::int32_t y =
        world::detail::BorrowPlayerYStorage()->value();
    const std::int32_t x =
        world::detail::BorrowPlayerXStorage()->value();
    static_cast<void>(network::Sub_512620(
        platform::gConnectionSocket, x, y, request, target, 0, escaped));
    gCurrentWindow->state |= 0x8000u;
}

// 原函数：0x004C21B0。
std::int32_t WindowCrystalShop_ButtonOk(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    std::int32_t consumed = 0;
    if (hover) {
        constexpr wchar_t hint587FA8[] = L"買進指定的物品。";
        gHoverTextD68AE8 = hint587FA8;
    }
    if (gWindow69SlotsCA0AC8 <= 0) {
        if ((state & 2) != 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    } else if ((state & 2) != 0) {
        const std::int32_t balance = world::detail::ReadPlayerValue15BCC();
        if (balance < gWindow69TotalD45E70) {
            gWindow69Message580854 = -2;
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            const std::int32_t freeSlots = world::detail::Sub_518C60();
            if (gWindow69SlotsCA0AC8 > freeSlots) {
                gWindow69Message580854 = -3;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                gWindow69ConfirmCallbackCF4E48 = &WindowCrystalShop_Submit;
                std::int32_t count = 0;
                for (const WindowCrystalItem& item : gWindow69ItemsCB2230) {
                    if (item.used == 0) {
                        continue;
                    }
                    const std::int32_t quantity = item.selectedCount;
                    if (quantity == 0) {
                        continue;
                    }
                    WindowConfirmationItem& preview = gWindow69ConfirmItemsD2E458[count];
                    preview.visible = 1;
                    std::strcpy(preview.name.data(), item.name.data());
                    const std::int32_t imageNo = item.imageNo;
                    preview.color = 0;
                    preview.imageNo = imageNo;
                    preview.quantity = quantity;
                    preview.factor = 0;
                    ++count;
                }
                for (; count < 20; ++count) {
                    gWindow69ConfirmItemsD2E458[count].visible = 0;
                }
                WindowConfirmationAmount& left = gWindow69ConfirmAmountsD2E2B8[0];
                WindowConfirmationAmount& right = gWindow69ConfirmAmountsD2E2B8[1];
                const std::int32_t currentBalance = world::detail::ReadPlayerValue15BCC();
                left.y = 343;
                right.y = 343;
                const std::int32_t total = gWindow69TotalD45E70;
                left.visible = 1;
                left.x = 296;
                left.value = currentBalance;
                right.visible = 1;
                right.x = 454;
                right.value = total;
                constexpr wchar_t heading588378[] =
                    L"持有水晶            顆     花費水晶            顆";
                constexpr wchar_t question587F0C[] =
                    L"確認要購買這些物品？";
                constexpr wchar_t accept587F00[] = L"買進物品。";
                constexpr wchar_t cancel585AD0[] = L"還是算了。";
                (gWindow69ConfirmHeadingCC3F68 = heading588378);
                (gWindow69ConfirmQuestionCC3F04 = question587F0C);
                gWindow69ConfirmAcceptD2E81C = accept587F00;
                gWindow69ConfirmCancelCB076C = cancel585AD0;
                gCurrentWindow->elements[0].visible = 1;
                gCurrentWindow->state |= 2u;
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            }
        }
        consumed = 1;
    }
    graphic->imageNo = 243048;
    if (hover) {
        graphic->imageNo = 243050;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243049;
    }
    if (gWindow69SlotsCA0AC8 <= 0) {
        graphic->imageNo = 243049;
    }
    return consumed;
}

// 原函数：0x004C2530。
std::int32_t WindowCrystalShop_ButtonPreviousRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t position = gWindow69ScrollD2D604;
        if (position > 0) {
            gWindow69ScrollD2D604 = position - 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return consumed;
}

// 原函数：0x004C25B0。
std::int32_t WindowCrystalShop_ButtonNextRow(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t count = gShopItemCountCB1E10;
        const std::int32_t position = gWindow69ScrollD2D604;
        if (position < count - 5) {
            gWindow69ScrollD2D604 = position + 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        consumed = 1;
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return consumed;
}

// 原函数：0x004C2630。
std::int32_t WindowCrystalShop_ButtonPreviousPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(gWindow69ScrollD2D604 > 0 ? 51 : 55, 320));
        const std::int32_t position = gWindow69ScrollD2D604 - 5;
        gWindow69ScrollD2D604 = position;
        if (position < 0) {
            gWindow69ScrollD2D604 = 0;
        }
        consumed = 1;
    }
    graphic->imageNo = 243030;
    if ((state & 1) != 0) {
        graphic->imageNo = 243032;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243031;
    }
    return consumed;
}

// 原函数：0x004C26B0。
std::int32_t WindowCrystalShop_ButtonNextPage(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const std::int32_t oldMaximum = gShopItemCountCB1E10 - 5;
        const bool canAdvance = oldMaximum > 0 && gWindow69ScrollD2D604 < oldMaximum;
        static_cast<void>(audio::PlaySe_52C450(canAdvance ? 51 : 55, 320));
        std::int32_t position = gWindow69ScrollD2D604 + 5;
        const std::int32_t maximum = gShopItemCountCB1E10 - 5;
        gWindow69ScrollD2D604 = position;
        if (position > maximum) {
            position = maximum;
            gWindow69ScrollD2D604 = position;
        }
        if (position < 0) {
            gWindow69ScrollD2D604 = 0;
        }
        consumed = 1;
    }
    graphic->imageNo = 243033;
    if ((state & 1) != 0) {
        graphic->imageNo = 243035;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243034;
    }
    return consumed;
}

// 原函数：0x004C2760。
std::int32_t WindowCrystalShop_Scroll(const std::int32_t index, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gShopItemCountCB1E10;
        const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        const std::int32_t position = gWindow69ScrollD2D604;
        WindowInstance* const window = gCurrentWindow;
        gWindow69ScrollD2D604 =
            WindowScroll_ApplyWheel(window->elements + index - 5, position, count - 5, wheel);
    }
    return 0;
}

// 原函数：0x004C27B0。
std::int32_t WindowCrystalShop_ButtonDecrease(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    WindowCrystalItem* const item = &gWindow69ItemsCB2230[gWindow69ScrollD2D604 + index - 46];
    std::int32_t consumed = 0;
    if (hover) {
        const wchar_t* const hint = item->selectedCount > 0
            ? L"減少物品的數量。"
            : L"無法再減少物品的數量了。";
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        const std::int32_t count = item->selectedCount;
        if (count > 0) {
            item->selectedCount = count - 1;
            gWindow69DirtyD67B9C = 1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        gWindow69Message580854 = -1;
        consumed = 1;
    }
    graphic->imageNo = 243006;
    if (hover) {
        graphic->imageNo = 243008;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243007;
    }
    return consumed;
}

// 原函数：0x004C28C0。
std::int32_t WindowCrystalShop_ButtonIncrease(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    WindowCrystalItem* const item = &gWindow69ItemsCB2230[gWindow69ScrollD2D604 + index - 51];
    std::int32_t consumed = 0;
    if (hover) {
        const std::int32_t limit = item->limitC4C;
        const std::int32_t count = item->selectedCount;
        const wchar_t* hint;
        if (count < limit) {
            hint = L"增加物品的數量。";
        } else if (limit == 0) {
            hint = L"已經賣完了！";
        } else {
            hint = L"無法再增加物品的數量了。";
        }
        gHoverTextD68AE8 = hint;
    }
    if ((state & 0x400) != 0) {
        if (item->used != 0) {
            const std::int32_t count = item->selectedCount;
            const std::int32_t limit = item->limitC4C;
            if (count < limit) {
                item->selectedCount = count + 1;
                gWindow69DirtyD67B9C = 1;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
        }
        consumed = 1;
    }
    graphic->imageNo = 243003;
    if (hover) {
        graphic->imageNo = 243005;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243004;
    }
    return consumed;
}

// 原函数：0x004C29E0。
std::int32_t WindowCrystalShop_HandleItemIcon(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t selected = gWindow69ScrollD2D604 + index - 56;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t consumed = 0;
    const WindowCrystalItem* const item = &gWindow69ItemsCB2230[selected];
    if (item->used == 0) {
        return 0;
    }
    if ((state & 1) != 0) {
        if (gShopRowKeysC9A954[index - 56] != selected) {
            gWindow69TooltipPageD67B94 = 0;
        }
        gShopRowKeysC9A954[index - 56] = selected;
        if ((state & 4) != 0) {
            ++gWindow69TooltipPageD67B94;
            if (gWindow69TooltipPageD67B94 >= 4) {
                gWindow69TooltipPageD67B94 = 0;
            }
            consumed = 1;
        }
        gWindow69Tooltip580850 = selected + 100;
    }
    if (item->remainingC50 == 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t windowY = window->y;
        const std::int32_t y = element.y + windowY + 24;
        const std::int32_t elementX = element.x;
        const std::int32_t x = elementX + window->x + 24;
        static_cast<void>(login::detail::DrawGraphic(x, y, 223, 98102, 0, &descriptor));
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement& element = window->elements[index];
    const std::int32_t windowY = window->y;
    const std::int32_t imageNo = item->imageNo;
    const std::int32_t y = element.y + windowY + 24;
    const std::int32_t elementX = element.x;
    const std::int32_t x = elementX + window->x + 24;
    static_cast<void>(login::detail::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
    return consumed;
}

// 原函数：0x004C2B20。
std::int32_t WindowCrystalShop_HandleInventory(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    auto events = static_cast<std::uint32_t>(state);
    std::int32_t consumed = 0;
    std::int32_t hovered = -1;
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 2) {
        events &= ~0x60u;
        consumed = 1;
    }
    if ((events & 0x21u) != 0) {
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (slot % 5) + window->x + 2;
            const std::int32_t top = element.y + 50 * (slot / 5) + window->y + 2;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = slot;
                break;
            }
        }
    }
    if (gWindow69InventoryHover58085C != hovered) {
        gWindow69TooltipPageD67B94 = 0;
        gWindow69InventoryHover58085C = hovered;
    }
    const auto& items = world::detail::BorrowPlayerItemsFDA8A4();
    if (hovered >= 0 && (platform::BorrowLegacyMouseStateStorage()->edgeButtons & 0x10u) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    } else if ((events & 1u) != 0 && hovered >= 0 && (events & 4u) != 0) {
        if (items[hovered + 8].used != 0) {
            std::int32_t page = gWindow69TooltipPageD67B94;
            const std::int32_t pages = items[hovered + 8].pages;
            ++page;
            gWindow69TooltipPageD67B94 = page;
            if (page >= pages) {
                gWindow69TooltipPageD67B94 = 0;
            }
        }
        consumed = 1;
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 8) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * ((source - 8) % 5) + window->x + 2;
            const std::int32_t top = element.y + 50 * ((source - 8) / 5) + window->y + 2;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    std::wstring buffer;
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 2;
        const std::int32_t top = element.y + 50 * (hovered / 5) + window->y + 2;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        const world::detail::PlayerItemRecord& item = items[hovered + 8];
        if (item.used != 0) {
            gWindow69Tooltip580850 = hovered + 8;
            if (!item.memo.empty() && (item.flags & 0x100u) != 0) {
                gHoverTextD68AE8 = item.name + L"[kok]";
            } else if ((item.flags & 0x200u) != 0) {
                gHoverTextD68AE8 = item.memo + L"[han]";
            }
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        const world::detail::PlayerItemRecord& item = items[slot + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (slot % 5) + window->x + 26;
        const std::int32_t y = element.y + 50 * (slot / 5) + window->y + 26;
        const std::int32_t itemId = item.itemId;
        if (itemId == gValueD2D608) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 22), static_cast<std::uint16_t>(y - 22),
                static_cast<std::uint16_t>(x + 22), static_cast<std::uint16_t>(y + 22),
                223, 254, 0);
        }
        static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
        const std::int32_t count = item.count;
        if (count > 0) {
            cg::text::FormatTo(buffer, L"{:3}", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                3, 1, 1, buffer, 0, 0));
        }
    }
    return consumed;
}

// 原函数：0x004C2FA0。
std::int32_t WindowCrystalShop_HandleConfirmation(const std::int32_t index, const std::int32_t unusedState)
{
    static_cast<void>(unusedState);
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const bool acceptHover = login::detail::Sub_5049A0(100, 381, 166, 398, -1) != 0;
    const bool cancelHover = login::detail::Sub_5049A0(510, 381, 576, 398, -1) != 0;
    std::int32_t acceptImage = 245323;
    if (acceptHover) {
        acceptImage = 245325;
        const std::size_t length = gWindow69ConfirmAcceptD2E81C.size() + 1;
        const auto buttons = static_cast<std::uint8_t>(platform::BorrowLegacyMouseStateStorage()->edgeButtons);
        gHoverTextD68AE8 = gWindow69ConfirmAcceptD2E81C;
        if ((buttons & 1u) != 0) {
            gCurrentWindow->elements[index].visible = 0;
            const WindowValueCallback callback = gWindow69ConfirmCallbackCF4E48;
            if (callback != nullptr) {
                callback(0);
            }
            acceptImage = 245324;
            gCurrentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    std::int32_t cancelImage = 245326;
    if (cancelHover) {
        cancelImage = 245328;
        const std::size_t length = gWindow69ConfirmCancelCB076C.size() + 1;
        const std::size_t prefix = length & ~std::size_t{3};
        gHoverTextD68AE8 = gWindow69ConfirmCancelCB076C;
        const auto buttons = static_cast<std::uint8_t>(platform::BorrowLegacyMouseStateStorage()->edgeButtons);
        static_cast<void>(0);
        if ((buttons & 1u) != 0) {
            gCurrentWindow->elements[index].visible = 0;
            const WindowValueCallback callback = gWindow69ConfirmCallbackCF4E48;
            if (callback != nullptr) {
                callback(1);
            }
            cancelImage = 245327;
            gCurrentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    static_cast<void>(render::DrawGraphic3_520970(100, 381, 226, acceptImage, 0, 1, 0, &descriptor));
    static_cast<void>(render::DrawGraphic3_520970(510, 381, 226, cancelImage, 0, 1, 0, &descriptor));
    std::wstring buffer;
    for (std::int32_t column = 0; column < 4; ++column) {
        const std::int32_t x = 18 + column * 152;
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t y = 47 + row * 65;
            const WindowConfirmationItem& item = gWindow69ConfirmItemsD2E458[column * 5 + row];
            if (item.visible != 0) {
                static_cast<void>(login::detail::DrawGraphic(
                    x + 24, y + 24, 226, item.imageNo, 0, &descriptor));
                const std::int32_t factor = item.factor;
                if (factor != 0) {
                    cg::text::FormatTo(buffer, L"{:2} X {:2}", factor, item.quantity);
                } else {
                    cg::text::FormatTo(buffer, L"X {:2}", item.quantity);
                }
                const auto countFrame = static_cast<std::uint8_t>(login::detail::gValueD69768);
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(x + 57), static_cast<std::int16_t>(y + 33),
                    4, 12, 0, buffer, 0, countFrame));
                const auto nameFrame = static_cast<std::uint8_t>(login::detail::gValueD69768);
                const auto nameColor = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(item.color));
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(x), static_cast<std::int16_t>(y + 50),
                    4, 12, nameColor, cg::text::FromGbkWide(item.name.data()).c_str(), 0, nameFrame));
            }
            static_cast<void>(login::detail::DrawGraphic(
                x + 24, y + 24, 226, 243139, 0, &descriptor));
        }
    }
    static_cast<void>(render::DrawText_456780(190, 377, 4, 12, 0, gWindow69ConfirmHeadingCC3F68, 0, static_cast<std::uint8_t>(login::detail::gValueD69768)));
    static_cast<void>(render::DrawText_456780(190, 393, 4, 12, 0, gWindow69ConfirmQuestionCC3F04, 0, static_cast<std::uint8_t>(login::detail::gValueD69768)));
    for (const WindowConfirmationAmount& amount : gWindow69ConfirmAmountsD2E2B8) {
        if (amount.visible != 0) {
            cg::text::FormatTo(buffer, L"{}", amount.value);
            const std::int32_t y = amount.y;
            const std::int32_t x = amount.x;
            WindowNumber_Draw(x, y + 35, buffer, 0, 4, 1, 226);
        }
    }
    login::detail::DrawNineGrid_49BE00(
        245300, 0, 35, 640, 383, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x004C33B0。
std::int32_t WindowCrystalShop_ShowTooltip(
    const std::int32_t index, const std::int32_t page, const std::int32_t unused)
{
    static_cast<void>(unused);
    const std::int32_t key = index + 300;
    if (gTooltipSourceD303FC != key) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseX.value());
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseY.value());
        const bool left =
            (platform::BorrowLegacyMouseStateStorage()->mouseX < 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            (platform::BorrowLegacyMouseStateStorage()->mouseY < 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    gTooltipSourceD303FC = key;
    const WindowCrystalItem* const item = &gWindow69ItemsCB2230[index];
    const std::uint32_t flags = item->flags;
    std::wstring title = cg::text::FromGbkWide(item->tips.data());
    std::int8_t titleColor;
    if ((flags & 0x100u) != 0) {
        titleColor = 5;
    } else {
        title = cg::text::FromGbkWide(item->name.data());
        titleColor = static_cast<std::int8_t>((flags >> 9) & 1u);
    }
    const char* const lines = item->description.data() + page * 768;
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    return WindowItem_DrawTooltip(x, y, title.c_str(), titleColor, lines, 96, 8, nullptr, 4);
}

} // namespace detail

} // namespace cg::ui
