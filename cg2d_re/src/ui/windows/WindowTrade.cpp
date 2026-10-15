#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

InputContext gInputContextCC21E8 {}; // 原全局：0x00CC21E8。
} // namespace cg::input

namespace cg::ui {
namespace detail {

WindowInputPrototype gTradeInputPrototype5803D0 { // 原全局：0x005803D0。
    {{&input::gInputContextCC21E8}, WindowInputReferenceKind::Context},
    0, -100, 3, 8, 127, {}, 1, 0, 0, 0,
};

std::array<char, 2 * 5 * 0x5FC> gTradePetsC9C108 {}; // 原全局：0x00C9C108。

static_assert(sizeof(gTradePetsC9C108) == 0x3BD8);

std::int32_t gTradeValueCC4224 = 0; // 原全局：0x00CC4224。

std::int32_t gTradeValueCF4688 = 0; // 原全局：0x00CF4688。

std::int32_t gTradeValueD046D0 = 0; // 原全局：0x00D046D0。

std::int32_t gTradeValueD2E2B0 = 0; // 原全局：0x00D2E2B0。

std::uint8_t gTradeValueD67B80 = 0; // 原全局：0x00D67B80。

login::YobiAction* gTradeWindowActionD67B7C = nullptr; // 原全局：0x00D67B7C。

std::int32_t gTradeValueCF4A1C = 0; // 原全局：0x00CF4A1C。

std::int32_t gTradeValueD673E0 = 0; // 原全局：0x00D673E0。

std::int32_t gTradeValueCC4C94 = 0; // 原全局：0x00CC4C94。

std::int32_t gTradeValueC8AF68 = 0; // 原全局：0x00C8AF68。

std::int32_t gTradeValueCB03CC = 0; // 原全局：0x00CB03CC。

std::int32_t gTradeValueCF4AB8 = 0; // 原全局：0x00CF4AB8。

std::int32_t gTradeValueD67CB8 = 0; // 原全局：0x00D67CB8。

std::int32_t gTradeValueD67CBC = 0; // 原全局：0x00D67CBC。

std::int32_t gTradeValueCC3EC4 = 0; // 原全局：0x00CC3EC4。

std::int32_t gTradeValueD66E08 = 0; // 原全局：0x00D66E08。

std::int32_t gTradeValueCC2344 = 0; // 原全局：0x00CC2344。

std::array<char, 17> gTradeNameD2D654 {}; // 原全局：0x00D2D654。

static_assert(sizeof(gTradeNameD2D654) == 17);

std::array<std::uint8_t, 20> gTradeItemSlotsCF3AE8 {}; // 原全局：0x00CF3AE8。

std::array<std::uint8_t, 28> gTradeItemMarksD2D61C {}; // 原全局：0x00D2D61C。

std::array<std::uint8_t, 5> gTradePetSlotsCC4D28 {}; // 原全局：0x00CC4D28。

std::array<std::uint8_t, 5> gTradePetMarksCF3AFC {}; // 原全局：0x00CF3AFC。

std::uint8_t gTradePhaseCAFEC4 = 0; // 原全局：0x00CAFEC4。

std::uint8_t gTradeStepD3040C = 0; // 原全局：0x00D3040C。

std::uint8_t gTradeValueD2EA20 = 0; // 原全局：0x00D2EA20。

std::uint8_t gTradeValueD6742C = 0; // 原全局：0x00D6742C。

std::uint8_t gTradeValueC8AF81 = 0; // 原全局：0x00C8AF81。

std::uint8_t gTradeTabCC4D1A = 0; // 原全局：0x00CC4D1A。

std::int32_t gTradeHoverSlotC8B314 = 0; // 原全局：0x00C8B314。

std::int32_t gTradeHoverSlotC8AA78 = 0; // 原全局：0x00C8AA78。

std::int32_t gTradeHoverSlotCB0A4C = 0; // 原全局：0x00CB0A4C。

std::int32_t gTradeScrollCF3B0C = 0; // 原全局：0x00CF3B0C。

std::int32_t gTradeScrollCC4C90 = 0; // 原全局：0x00CC4C90。

std::int32_t gTradeScrollD67430 = 0; // 原全局：0x00D67430。

std::int32_t gTradePreviewCC2E64 = 0; // 原全局：0x00CC2E64。

std::uint8_t gTradeTooltipPageCB10D4 = 0; // 原全局：0x00CB10D4。

const std::array<const wchar_t*, 6> gTradeNumberHints5805FC {{ // 原全局：0x005805FC。
    L"指定金額。",
    L"指定提領金額。",
    L"指定金額數量。",
    L"減少一個位數。",
    L"指定全部金額。",
    L"把指定金額歸零。",
}};

// 原函数：0x004E5DA0。
void WindowTrade_SwapRemoteItems(
    const std::int32_t side, const std::int32_t first, const std::int32_t second)
{
    if (first < 8 || first >= 28 || second < 8 || second >= 28) {
        return;
    }
    char* const left = gTradeItemsD460D0.data() + (side * 20 + first - 8) * 0xC4C;
    char* const right = gTradeItemsD460D0.data() + (side * 20 + second - 8) * 0xC4C;
    std::array<char, 0xC4C> temporary;
    std::memcpy(temporary.data(), left, temporary.size());
    std::memmove(left, right, temporary.size());
    std::memcpy(right, temporary.data(), temporary.size());
}

// 原函数：0x004E1840。
void WindowTrade_SwapDisplayItems(
    const std::int32_t side, const std::int32_t first, const std::int32_t second)
{
    if (first < 8 || first >= 28 || second < 8 || second >= 28) {
        return;
    }
    char* const left = gItemDetailStorageCC4D58.data() + side * 0xFBF8 + (first - 8) * 0xC44;
    char* const right = gItemDetailStorageCC4D58.data() + side * 0xFBF8 + (second - 8) * 0xC44;
    std::array<char, 0xC44> temporary;
    std::memcpy(temporary.data(), left, temporary.size());
    std::memmove(left, right, temporary.size());
    std::memcpy(right, temporary.data(), temporary.size());
}

// 原函数：0x004B0000。
void WindowTrade_ResetSelection(const std::int32_t first, const std::int32_t second)
{
    if (gWindowStates[WindowId_Trade].window == nullptr) {
        return;
    }
    for (std::uint8_t& slot : gTradeItemSlotsCF3AE8) {
        const std::int32_t value = std::bit_cast<std::int8_t>(slot);
        if (value == first) {
            slot = static_cast<std::uint8_t>(second);
        } else if (value == second) {
            slot = static_cast<std::uint8_t>(first);
        }
    }
    const std::uint8_t firstMark = gTradeItemMarksD2D61C[first];
    const std::uint8_t secondMark = gTradeItemMarksD2D61C[second];
    gTradeItemMarksD2D61C[first] = secondMark;
    gTradeItemMarksD2D61C[second] = firstMark;
}

// 原函数：0x004AD060。
void WindowTrade_RemapTextListSelection(const std::int32_t first, const std::int32_t second)
{
    if (gWindowStates[WindowId_TextList].window != nullptr && gSelectedItem57EE0C == first) {
        gSelectedItem57EE0C = second;
    }
}

// 原函数：0x004E54E0。
void WindowTrade_CopyOwnItem(const std::int32_t side, const std::int32_t index)
{
    if (index < 0 || index >= 20) {
        return;
    }
    char* const record = gTradeItemsD460D0.data() + (index + side * 20) * 0xC4C;
    std::memset(record, 0, 0xC4C);
    const world::detail::PlayerItemRecord& source = world::detail::BorrowPlayerItemsFDA8A4()[index + 8];
    if (source.used == 0) {
        return;
    }
    const std::int32_t imageNo = source.imageNo;
    const std::int32_t count = source.count;
    WindowTrade_WriteValue(record, 0, std::uint16_t{1});
    const std::uint16_t flags = source.flags;
    WindowTrade_WriteValue(record, 4, imageNo);
    const std::int32_t itemId = source.itemId;
    WindowTrade_WriteValue(record, 8, count);
    WindowTrade_WriteValue(record, 0x0C, itemId);
    WindowTrade_WriteValue(record, 0xC34,
        static_cast<std::int32_t>(std::bit_cast<std::int16_t>(flags)));
    if (!source.memo.empty()) {
        cg::text::StoreGbk(record + 0xC38, 17, source.memo);
    }
    cg::text::StoreGbk(record + 0x10, 29, source.name);
    if ((flags & 1u) == 0) {
        WindowTrade_WriteValue(record, 2, std::uint16_t{0xFFFF});
    }
}

// 原函数：0x004E5600。
void WindowTrade_SyncOwnItems(const std::int32_t side)
{
    for (std::int32_t index = 0; index < 20; ++index) {
        WindowTrade_CopyOwnItem(side, index);
    }
}

// 原函数：0x004B0110。
login::YobiAction* WindowTrade_Open()
{
    gTradeValueD67B80 = 0;
    login::YobiAction* const action = WindowRuntime_CreateAnimation(WindowId_Trade, 1, 0, nullptr);
    gTradeWindowActionD67B7C = action;
    return action;
}

// 原函数：0x004B0130。
void WindowTrade_MovePanel(
    WindowInstance* const window, const std::int32_t index,
    const std::int32_t x, const std::int32_t y)
{
    const std::int32_t shortX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const std::int32_t shortY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    window->elements[index].x = shortX;
    window->elements[index].y = shortY;
    window->elements[index - 1].x = shortX + 252;
    window->elements[index - 1].y = shortY + 37;
    window->elements[index - 2].x = shortX + 252;
}

// 原函数：0x004B0180。
void WindowTrade_SetPanelVisible(
    WindowInstance* const window, const std::int32_t index, const std::int32_t visible)
{
    window->elements[index].visible = visible;
    if (visible == 0) {
        window->elements[index - 1].visible = 0;
        window->elements[index - 2].visible = 0;
    }
}

// 原函数：0x004B01B0。
void WindowTrade_SetPanelColor(
    WindowInstance* const window, const std::int32_t index, const std::uint32_t color)
{
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    graphic->color = color;
}

// 原函数：0x004AFF30。
std::int32_t WindowTrade_AddItem(const std::int32_t index, const std::int32_t slot)
{
    if (std::bit_cast<std::int8_t>(gTradeItemSlotsCF3AE8[index]) >= 0) {
        return 0;
    }
    gTradeItemSlotsCF3AE8[index] = static_cast<std::uint8_t>(slot);
    auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    const std::uint8_t previous = hover[slot];
    gTradeItemMarksD2D61C[slot] = 1;
    hover[slot] = static_cast<std::uint8_t>(previous + 1);
    return 1;
}

// 原函数：0x004AFFB0。
std::int32_t WindowTrade_SwapSelectedItems(const std::int32_t first, const std::int32_t second)
{
    const std::uint8_t previous = gTradeItemSlotsCF3AE8[first];
    gTradeItemSlotsCF3AE8[first] = gTradeItemSlotsCF3AE8[second];
    gTradeItemSlotsCF3AE8[second] = previous;
    return first;
}

// 原函数：0x004B0060。
std::int32_t WindowTrade_AddPet(const std::int32_t index, const std::int32_t slot)
{
    if (std::bit_cast<std::int8_t>(gTradePetSlotsCC4D28[index]) >= 0) {
        return 0;
    }
    gTradePetSlotsCC4D28[index] = static_cast<std::uint8_t>(slot);
    gTradePetMarksCF3AFC[slot] = 1;
    return 1;
}

// 原函数：0x004B00C0。
std::int32_t WindowTrade_SwapSelectedPets(const std::int32_t first, const std::int32_t second)
{
    const std::uint8_t previous = gTradePetSlotsCC4D28[first];
    gTradePetSlotsCC4D28[first] = gTradePetSlotsCC4D28[second];
    gTradePetSlotsCC4D28[second] = previous;
    return first;
}

// 原函数：0x004AFF70。
std::int32_t WindowTrade_RemoveItem(const std::int32_t index)
{
    const std::int32_t slot = std::bit_cast<std::int8_t>(gTradeItemSlotsCF3AE8[index]);
    if (slot < 0) {
        return -1;
    }
    gTradeItemSlotsCF3AE8[index] = 0xFF;
    auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    const std::uint8_t previous = hover[slot];
    gTradeItemMarksD2D61C[slot] = 0;
    hover[slot] = static_cast<std::uint8_t>(previous - 1);
    return slot;
}

// 原函数：0x004B0090。
std::int32_t WindowTrade_RemovePet(const std::int32_t index)
{
    const std::int32_t slot = std::bit_cast<std::int8_t>(gTradePetSlotsCC4D28[index]);
    if (slot < 0) {
        return -1;
    }
    gTradePetSlotsCC4D28[index] = 0xFF;
    gTradePetMarksCF3AFC[slot] = 0;
    return slot;
}

// 原函数：0x004B0200。
void WindowTrade_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_Quantity].window;
    if (window != nullptr && gWindowStates[WindowId_Quantity].value20 == 43) {
        window->state |= 0x8000u;
    }
    if (gTradeValueD67B80 == 0) {
        static_cast<void>(network::Sub_512270(platform::gConnectionSocket, 0));
        gTradeValueD67CBC = 1;
    }
    for (std::int32_t index = 0; index < 20; ++index) {
        static_cast<void>(WindowTrade_RemoveItem(index));
    }
    for (std::int32_t index = 0; index < 5; ++index) {
        static_cast<void>(WindowTrade_RemovePet(index));
    }
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    gTradeValueD67B80 = 0;
}

// 原函数：0x004B0290。
std::int32_t WindowTrade_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowInstance* const initialWindow = gCurrentWindow;
        gTradeWindowActionD67B7C = nullptr;
        gTradeValueCC4224 = 0;
        gTradeValueD2E2B0 = 0;
        gTradePhaseCAFEC4 = 0;
        WindowTrade_SetPanelVisible(initialWindow, 11, 1);
        WindowTrade_MovePanel(gCurrentWindow, 11, 287, 28);
        WindowTrade_SetPanelColor(gCurrentWindow, 11, 0xFFFFFFFFu);
        WindowTrade_SetPanelVisible(gCurrentWindow, 8, 1);
        WindowTrade_MovePanel(gCurrentWindow, 8, 15, 28);
        WindowTrade_SetPanelColor(gCurrentWindow, 8, 0xFFFFFFFFu);
        WindowTrade_SetPanelVisible(gCurrentWindow, 14, 0);
        WindowTrade_MovePanel(gCurrentWindow, 14, 15, 28);
        WindowTrade_SetPanelColor(gCurrentWindow, 14, 0xFFFFFFFFu);
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[3].visible = 0;
        gCurrentWindow->elements[4].visible = 0;
        WindowInstance* const scrollWindow = gCurrentWindow;
        gTradeValueD2EA20 = 0xFF;
        gTradeValueD6742C = 0;
        gTradeValueC8AF81 = 0;
        gTradeScrollCF3B0C = 0;
        gTradeScrollCC4C90 = 0;
        gTradeScrollD67430 = 0;
        static_cast<void>(WindowScroll_SetPosition(&scrollWindow->elements[10], 2, 0));
        const std::int32_t firstScroll = gTradeScrollCC4C90;
        static_cast<void>(WindowScroll_SetPosition(&gCurrentWindow->elements[7], 2, firstScroll));
        WindowInstance* const lastScrollWindow = gCurrentWindow;
        const std::int32_t lastScroll = gTradeScrollD67430;
        static_cast<void>(WindowScroll_SetPosition(&lastScrollWindow->elements[13], 2, lastScroll));
        gTradeItemMarksD2D61C.fill(0);
        // 原W32/U8交错写入同一数组，保留4B0424的窗口快照时点。
        std::memset(gTradePetMarksCF3AFC.data(), 0, 4);
        std::memset(gTradeItemSlotsCF3AE8.data(), 0xFF, 4);
        std::memset(gTradeItemSlotsCF3AE8.data() + 4, 0xFF, 4);
        gTradePetMarksCF3AFC[4] = 0;
        WindowInstance* const textWindow = gCurrentWindow;
        std::memset(gTradeItemSlotsCF3AE8.data() + 8, 0xFF, 4);
        std::memset(gTradeItemSlotsCF3AE8.data() + 12, 0xFF, 4);
        std::memset(gTradePetSlotsCC4D28.data(), 0xFF, 4);
        std::memset(gTradeItemSlotsCF3AE8.data() + 16, 0xFF, 4);
        gTradePetSlotsCC4D28[4] = 0xFF;
        auto* const otherName = static_cast<WindowTextData*>(textWindow->elements[15].data);
        otherName->text = std::format(L"{:>16}", cg::text::FromGbkWide(gTradeNameD2D654.data()));
        WindowElement* const elements = gCurrentWindow->elements;
        const char* const playerName = world::detail::BorrowPlayerDisplayText(1);
        auto* const separator = static_cast<WindowTextData*>(elements[16].data);
        separator->text = L"<=>";
        auto* const ownName = static_cast<WindowTextData*>(gCurrentWindow->elements[17].data);
        ownName->text = std::format(L"{:<16}", cg::text::FromGbkWide(playerName));
        const std::int32_t y =
            world::detail::BorrowPlayerYStorage()->value();
        const std::int32_t x =
            world::detail::BorrowPlayerXStorage()->value();
        static_cast<void>(WindowProfile_SaveOrigin(x, y));
    }
    if (WindowProfile_CheckDistance(1, 1) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (gTradeValueD67B80 != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    switch (std::bit_cast<std::int8_t>(gTradePhaseCAFEC4)) {
    case 0:
        if (gTradeValueD2E2B0 > 0) {
            WindowInstance* const window = gCurrentWindow;
            gTradePhaseCAFEC4 = 1;
            WindowTrade_SetPanelVisible(window, 14, 1);
            WindowTrade_MovePanel(gCurrentWindow, 14, 15, 28);
            WindowTrade_SetPanelColor(gCurrentWindow, 14, 0x00FFFFFFu);
            gTradeStepD3040C = 23;
        }
        break;
    case 1: {
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t step = std::bit_cast<std::int8_t>(gTradeStepD3040C);
        if (window->elements[8].x + step < 287) {
            WindowTrade_SetPanelVisible(window, 8, 1);
            WindowInstance* const moveWindow = gCurrentWindow;
            const std::int32_t moveStep = std::bit_cast<std::int8_t>(gTradeStepD3040C);
            const std::int32_t x = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(moveWindow->elements[8].x));
            WindowTrade_MovePanel(moveWindow, 8, x + moveStep, 28);
            const std::uint8_t oldStep = gTradeStepD3040C;
            WindowInstance* const firstWindow = gCurrentWindow;
            gTradeStepD3040C = static_cast<std::uint8_t>(oldStep - 1);
            const auto* first = static_cast<WindowGraphicData*>(firstWindow->elements[11].data);
            std::int32_t alpha = static_cast<std::int32_t>(first->color >> 24) - 10;
            if (alpha < 0) {
                alpha = 0;
            }
            WindowTrade_SetPanelColor(firstWindow, 11,
                (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
            WindowInstance* const secondWindow = gCurrentWindow;
            const auto* second = static_cast<WindowGraphicData*>(secondWindow->elements[14].data);
            alpha = static_cast<std::int32_t>(second->color >> 24) + 10;
            if (alpha > 255) {
                alpha = 255;
            }
            WindowTrade_SetPanelColor(secondWindow, 14,
                (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
        } else {
            WindowTrade_SetPanelColor(window, 14, 0xFFFFFFFFu);
            WindowTrade_SetPanelVisible(gCurrentWindow, 8, 1);
            WindowTrade_MovePanel(gCurrentWindow, 8, 287, 28);
            WindowTrade_SetPanelVisible(gCurrentWindow, 11, 0);
            gCurrentWindow->elements[2].visible = 0;
            gCurrentWindow->elements[3].visible = 1;
            gCurrentWindow->elements[4].visible = 1;
            gTradePhaseCAFEC4 = 3;
        }
        break;
    }
    case 2: {
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t step = std::bit_cast<std::int8_t>(gTradeStepD3040C);
        if (window->elements[8].x - step > 15) {
            WindowTrade_SetPanelVisible(window, 8, 1);
            WindowInstance* const moveWindow = gCurrentWindow;
            const std::int32_t moveStep = std::bit_cast<std::int8_t>(gTradeStepD3040C);
            const std::int32_t x = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(moveWindow->elements[8].x));
            WindowTrade_MovePanel(moveWindow, 8, x - moveStep, 28);
            const std::uint8_t oldStep = gTradeStepD3040C;
            WindowInstance* const firstWindow = gCurrentWindow;
            gTradeStepD3040C = static_cast<std::uint8_t>(oldStep - 1);
            const auto* first = static_cast<WindowGraphicData*>(firstWindow->elements[11].data);
            std::int32_t alpha = static_cast<std::int32_t>(first->color >> 24) + 10;
            if (alpha > 255) {
                alpha = 255;
            }
            WindowTrade_SetPanelColor(firstWindow, 11,
                (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
            WindowInstance* const secondWindow = gCurrentWindow;
            const auto* second = static_cast<WindowGraphicData*>(secondWindow->elements[14].data);
            alpha = static_cast<std::int32_t>(second->color >> 24) - 10;
            if (alpha < 0) {
                alpha = 0;
            }
            WindowTrade_SetPanelColor(secondWindow, 14,
                (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
        } else {
            WindowTrade_SetPanelColor(window, 11, 0xFFFFFFFFu);
            WindowTrade_SetPanelVisible(gCurrentWindow, 8, 1);
            WindowTrade_MovePanel(gCurrentWindow, 8, 15, 28);
            WindowTrade_SetPanelVisible(gCurrentWindow, 14, 0);
            gCurrentWindow->elements[2].visible = 1;
            gCurrentWindow->elements[3].visible = 0;
            gCurrentWindow->elements[4].visible = 0;
            gTradePhaseCAFEC4 = 0;
        }
        break;
    }
    case 3:
        if (gTradeValueD2E2B0 <= 0) {
            WindowInstance* const window = gCurrentWindow;
            gTradePhaseCAFEC4 = 2;
            WindowTrade_SetPanelVisible(window, 11, 1);
            WindowTrade_MovePanel(gCurrentWindow, 11, 287, 28);
            WindowTrade_SetPanelColor(gCurrentWindow, 11, 0x00FFFFFFu);
            gTradeStepD3040C = 23;
        }
        break;
    default:
        break;
    }
    gTradePreviewCC2E64 = -1;
    return 1;
}

// 原函数：0x004B08D0。
std::int32_t WindowTrade_Draw(const std::int32_t)
{
    std::int32_t preview = gTradePreviewCC2E64;
    if (preview >= 0) {
        if (preview < 300) {
            const bool ownItem = preview < 100;
            if (!ownItem) {
                preview -= 200;
                gTradePreviewCC2E64 = preview;
            }
            if (preview >= 0) {
                const std::int32_t page = std::bit_cast<std::int8_t>(gTradeTooltipPageCB10D4);
                if (ownItem) {
                    static_cast<void>(WindowItem_DrawEquipmentTooltip(preview, page));
                } else {
                    static_cast<void>(WindowItem_DrawPetTooltip(0, preview, page));
                }
            }
        } else {
            preview -= 300;
            gTradePreviewCC2E64 = preview;
            // 原4B0970在异常分类>=600时读未初始化字节；不补零或指定分支。
            std::uint8_t ownPet;
            if (preview < 100) {
                ownPet = 1;
            } else if (preview < 300) {
                ownPet = 0;
                preview -= 200;
                gTradePreviewCC2E64 = preview;
            }
            if (preview >= 0) {
                const bool left = (platform::BorrowLegacyMouseStateStorage()->mouseX < 320);
                const std::int32_t x = left ? 413 : 0;
                const std::uint8_t index = static_cast<std::uint8_t>(gTradePreviewCC2E64);
                if (ownPet != 0) {
                    static_cast<void>(WindowTrade_DrawOwnPetTooltip(x, 0, index));
                } else {
                    static_cast<void>(WindowTrade_DrawSelectedPetTooltip(x, 0, 0, index));
                }
            }
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004B0F20。
std::int32_t WindowTrade_DrawPetRow(
    const std::int16_t x, const std::int16_t y, const std::int8_t side,
    const std::int32_t index, const std::uint8_t textLayer, const std::uint8_t graphicLayer,
    const render::GraphicDescriptor16* const descriptor)
{
    const char* const record = gTradePetsC9C108.data() + (index + side * 5) * 0x5FC;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used != 0) {
        constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{138});
        static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
        static_cast<void>(render::DrawText_456780(
            0, 0, textLayer, 8, color, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 45)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 5)),
            textLayer, 8, color, cg::text::FromGbkWide(record + 0x5D8).c_str(), 0, 0));
        std::wstring buffer;
        const auto drawNumber = [&](const std::size_t offset, const std::wstring_view format,
                                    const std::int32_t dx, const std::int32_t dy) {
            std::int32_t value;
            std::memcpy(&value, record + offset, sizeof(value));
            cg::text::FormatTo(buffer, format, value);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + dx)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + dy)),
                textLayer, 8, color, buffer, 0, 0));
        };
        drawNumber(0x0C, L"{:3}", 35, 22);
        std::int16_t tribe;
        std::memcpy(&tribe, record + 4, sizeof(tribe));
        if (tribe >= 0 && tribe < 20) {
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 100)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 22)),
                textLayer, 8, color, gTribeNames57CB54[tribe], 0, 0));
        }
        drawNumber(0x14, L"{:4}", 210, 5);
        drawNumber(0x1C, L"{:4}", 210, 22);
        drawNumber(0x20, L"{:3}", 95, 40);
        drawNumber(0x24, L"{:3}", 155, 40);
        drawNumber(0x28, L"{:3}", 215, 40);
        drawNumber(0x2C, L"{:3}", 95, 55);
        drawNumber(0x30, L"{:3}", 155, 55);
        drawNumber(0x34, L"{:3}", 215, 55);
        drawNumber(0x5C, L"{:2}", 45, 55);
    }
    return render::DrawGraphic3_520970(
        x, y, graphicLayer, 243141, 0, 1, 0, descriptor);
}

// TQN01：原4B1260完整524条；低W16坐标/scroll BYTE、滚动值先元素及前一滚动框坐标原序，原callback/格式历史保。
// 原函数：0x004B1260。
std::int32_t WindowTrade_HandlePanel(
    WindowInstance* const window, const std::int32_t index, const std::int32_t state,
    const std::int32_t* const backgroundImages, std::uint8_t* const tab,
    const std::int32_t scrollElement, std::int32_t* const scrollValue,
    const std::int32_t numericElement, const std::int32_t maximum,
    std::int32_t itemCount, std::int32_t petCount, std::int32_t amount,
    render::GraphicDescriptor16* const descriptor, const WindowTradePanelCallback callback)
{
    WindowElement* const elements = window->elements;
    std::uint16_t elementX;
    std::uint16_t elementY;
    std::uint16_t windowX;
    std::uint16_t windowY;
    std::memcpy(&elementX, &elements[index].x, sizeof(elementX));
    std::memcpy(&elementY, &elements[index].y, sizeof(elementY));
    std::memcpy(&windowX, &window->x, sizeof(windowX));
    const std::int16_t x = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(elementX + windowX));
    std::memcpy(&windowY, &window->y, sizeof(windowY));
    const std::int16_t y = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(elementY + windowY));
    if ((state & 0x21) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        const std::int32_t scrollPosition = *scrollValue;
        WindowElement* const wheelElement = &elements[scrollElement];
        *scrollValue = WindowScroll_ApplyWheel(wheelElement, scrollPosition, 2, wheel);
    }
    WindowElement* const scroll = &window->elements[scrollElement];
    const auto scrollFlags = reinterpret_cast<const unsigned char*>(
        &static_cast<const WindowMouseData*>(scroll->data)->flags)[0];
    if ((scrollFlags & 1u) != 0) {
        *scrollValue = WindowScroll_ReadPosition(scroll, 2);
    }
    static constexpr std::array<std::array<std::int16_t, 4>, 5> rectangles54F508 {{
        {12, 240, 80, 15}, {97, 240, 80, 15}, {182, 240, 80, 15},
        {252, 28, 11, 11}, {252, 220, 11, 11},
    }};
    std::int32_t selected = -1;
    if ((state & 1) != 0) {
        for (std::int32_t part = 0; part < 5; ++part) {
            const auto& rectangle = rectangles54F508[part];
            if (WindowTrade_HitTestOwnPetRow(x + rectangle[0], y + rectangle[1], rectangle[2], rectangle[3])) {
                selected = part;
            }
        }
    }
    const std::int32_t result = state & 2;
    if (result != 0 && selected >= 0 && selected <= 2) {
        static_cast<void>(audio::PlaySe_52C450(
            std::bit_cast<std::int8_t>(*tab) == selected ? 55 : 51, 320));
        *tab = static_cast<std::uint8_t>(selected);
        static_cast<void>(WindowTrade_HandleAmountInput(0, 2));
    }
    if ((state & 0x400) != 0 && *tab == 1) {
        if (selected == 3) {
            if (*scrollValue > 0) {
                --*scrollValue;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t scrollPosition = *scrollValue;
            WindowElement* const scrollTarget = &window->elements[scrollElement];
            static_cast<void>(WindowScroll_SetPosition(scrollTarget, 2, scrollPosition));
        } else if (selected == 4) {
            if (*scrollValue < 2) {
                ++*scrollValue;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            const std::int32_t scrollPosition = *scrollValue;
            WindowElement* const scrollTarget = &window->elements[scrollElement];
            static_cast<void>(WindowScroll_SetPosition(scrollTarget, 2, scrollPosition));
        }
    }
    std::uint8_t numericResult = 0;
    if (numericElement >= 0 && *tab == 2) {
        std::uint32_t color;
        std::memcpy(&color, descriptor->bytes.data() + 8, sizeof(color));
        const std::uint8_t flags =
            input::gCurrentInputData == &input::gInputContextCC21E8 ? 3 : 1;
        numericResult = WindowInput_HandleNumber(
            window, numericElement, state, maximum, 223, gTradeNumberHints5805FC.data(),
            flags, color);
    }
    std::int32_t displayAmount; // 原回调负责写入，不为无回调的异常路径补零。
    if (callback != nullptr) {
        // 仅适配已证不消费该参数的-1哨兵；正索引仍借用同一W32，见窗口SPEC。
        std::uint32_t* const valueSlot =
            numericElement == -1 ? nullptr : &window->elements[numericElement].state;
        const std::int32_t scrollSnapshot = *scrollValue;
        const std::int32_t mouseY =
            platform::BorrowLegacyMouseStateStorage()->mouseY.value();
        const std::int32_t mouseX =
            platform::BorrowLegacyMouseStateStorage()->mouseX.value();
        callback(x, y, mouseX, mouseY, state, tab, scrollSnapshot, numericResult, valueSlot,
                 &itemCount, &petCount, &amount, &displayAmount, descriptor);
    }
    std::int32_t imageNo = *tab == 0 ? 243028 : (selected == 0 ? 243029 : 243027);
    static_cast<void>(render::DrawGraphic3_520970(
        x + 12, y + 240, 223, imageNo, 0, 1, 0, descriptor));
    imageNo = *tab == 1 ? 243022 : (selected == 1 ? 243023 : 243021);
    static_cast<void>(render::DrawGraphic3_520970(
        x + 97, y + 240, 223, imageNo, 0, 1, 0, descriptor));
    imageNo = *tab == 2 ? 243025 : (selected == 2 ? 243026 : 243024);
    static_cast<void>(render::DrawGraphic3_520970(
        x + 182, y + 240, 223, imageNo, 0, 1, 0, descriptor));
    window->elements[scrollElement].visible = 0;
    window->elements[scrollElement - 1].visible = 0;
    if (*tab == 1) {
        WindowElement* const previous = &window->elements[scrollElement - 1];
        const std::int32_t currentWindowY = window->y;
        const std::int32_t previousY = previous->y;
        const std::int32_t drawY = currentWindowY + previousY;
        const std::int32_t previousX = previous->x;
        const std::int32_t currentWindowX = window->x;
        const std::int32_t drawX = currentWindowX + previousX;
        static_cast<void>(render::DrawGraphic3_520970(
            drawX, drawY, 223, 243096, 0, 1, 0, descriptor));
        imageNo = selected == 3 ? ((state & 8) != 0 ? 243091 : 243092) : 243090;
        static_cast<void>(render::DrawGraphic3_520970(
            x + 252, y + 28, 223, imageNo, 0, 1, 0, descriptor));
        imageNo = selected == 4 ? ((state & 8) != 0 ? 243094 : 243095) : 243093;
        static_cast<void>(render::DrawGraphic3_520970(
            x + 252, y + 220, 223, imageNo, 0, 1, 0, descriptor));
        window->elements[scrollElement].visible = 1;
        window->elements[scrollElement - 1].visible = 1;
    }
    std::wstring buffer;
    if (*tab == 2) {
        cg::text::FormatTo(buffer, L"{:9}", displayAmount);
        WindowNumber_Draw(x + 179, y + 88, buffer, 3, 0, 1, 223);
    }
    cg::text::FormatTo(buffer, L"{:3}", itemCount);
    WindowNumber_Draw(x + 71, y + 258, buffer, 3, 0, 1, 223);
    cg::text::FormatTo(buffer, L"{:3}", petCount);
    WindowNumber_Draw(x + 156, y + 258, buffer, 3, 0, 1, 223);
    cg::text::FormatTo(buffer, L"{:10}", amount);
    WindowNumber_Draw(x + 241, y + 258, buffer, 3, 0, 1, 223);
    imageNo = backgroundImages[std::bit_cast<std::int8_t>(*tab)];
    static_cast<void>(render::DrawGraphic3_520970(
        x, y, 223, imageNo, 0, 1, 0, descriptor));
    return result;
}

// 原函数：0x004B1940。
void WindowTrade_HandleOwnInventory(
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const render::GraphicDescriptor16* const descriptor)
{
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(dimmed.bytes.data() + 8, &color, sizeof(color));
    dimmed.bytes[13] = 4;
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 2) {
        events &= 0x9Fu;
    }
    if (gTradePhaseCAFEC4 != 0) {
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX();
        const std::int32_t dropY = WindowDrag_ReadY();
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const std::int32_t left = x + 50 * (position % 5) + 13;
            const std::int32_t top = y + 52 * (position / 5) + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (source != position + 8) {
                WindowItem_RequestTransfer(43, 11, source, position + 8);
            }
            break;
        }
        WindowDrag_Reset();
    }
    if (*tab != 0) {
        return;
    }
    std::int32_t hovered = -1;
    const bool hover = (events & 0x21u) != 0;
    if (hover) {
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const std::int32_t left = x + 50 * (position % 5) + 13;
            const std::int32_t top = y + 52 * (position / 5) + 26;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position + 8;
                break;
            }
        }
        if (hovered >= 0 && gTradePhaseCAFEC4 == 0
            && world::detail::BorrowPlayerItemsFDA8A4()[hovered].used != 0) {
            if ((world::detail::BorrowPlayerItemsFDA8A4()[hovered].flags & 1u) == 0) {
                gHoverTextD68AE8 = L"此物品無法交易。";
            } else if (gTradeItemMarksD2D61C[hovered] == 0) {
                gHoverTextD68AE8 = L"用滑鼠拖放方式，登錄此筆物品交易。";
            }
        }
    }
    if (gTradeValueD2EA20 == 0) {
        if (gTradeHoverSlotCB0A4C != hovered) {
            gTradeTooltipPageCB10D4 = 0;
        }
        gTradeHoverSlotCB0A4C = hovered;
    }
    if (hovered >= 0 && (platform::BorrowLegacyMouseStateStorage()->edgeButtons & 0x10u) != 0
        && (WindowDrag_ReadFocusWindowId() == 43 || WindowDrag_ReadFocusWindowId() == 0)) {
        if (world::detail::BorrowPlayerItemsFDA8A4()[hovered].used != 0
            && (world::detail::BorrowPlayerItemsFDA8A4()[hovered].flags & 1u) != 0
            && gTradeItemMarksD2D61C[hovered] == 0) {
            std::int32_t target = 0;
            while (target < 20 && WindowTrade_ReadSelectedItem(target) >= 0) {
                ++target;
            }
            if (target < 20) {
                static_cast<void>(WindowTrade_RemoveItem(target));
                static_cast<void>(WindowTrade_AddItem(target, hovered));
                *tab = 0;
            }
            if (gTradeValueC8AF81 != 0) {
                gTradeValueC8AF81 = 0;
            }
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        if (gTradePhaseCAFEC4 == 0 && (events & 2u) != 0
            && world::detail::BorrowPlayerItemsFDA8A4()[hovered].used != 0
            && gTradeItemMarksD2D61C[hovered] == 0) {
            static_cast<void>(WindowItemDrag_Start(hovered, 1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((events & 4u) != 0 && world::detail::BorrowPlayerItemsFDA8A4()[hovered].used != 0) {
            const std::int32_t pages = world::detail::BorrowPlayerItemsFDA8A4()[hovered].pages;
            gTradeTooltipPageCB10D4 = static_cast<std::uint8_t>(gTradeTooltipPageCB10D4 + 1);
            if (std::bit_cast<std::int8_t>(gTradeTooltipPageCB10D4) >= pages) {
                gTradeTooltipPageCB10D4 = 0;
            }
        }
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (WindowDrag_ReadFocusWindowId() == 43 && WindowDrag_ReadFocusElement() == 11) {
            const std::int32_t left = x + 50 * ((source - 8) % 5) + 13;
            const std::int32_t top = y + 52 * ((source - 8) / 5) + 26;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, 254, 0);
        }
    }
    std::wstring buffer;
    if (hover && hovered >= 0) {
        const std::int32_t left = x + 50 * ((hovered - 8) % 5) + 13;
        const std::int32_t top = y + 52 * ((hovered - 8) / 5) + 26;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        const world::detail::PlayerItemRecord& item = world::detail::BorrowPlayerItemsFDA8A4()[hovered];
        if (item.used != 0) {
            const std::size_t memoLength = item.memo.size();
            gTradePreviewCC2E64 = hovered;
            if (memoLength != 0 && (item.flags & 0x100u) != 0) {
                gHoverTextD68AE8 = item.name + L"[kok]";
            } else if ((item.flags & 0x200u) != 0) {
                gHoverTextD68AE8 = item.memo + L"[han]";
            }
        }
        gTradeValueD2EA20 = 0;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    dimmed = *descriptor;
    dimmed.bytes[11] >>= 1;
    for (std::int32_t position = 0; position < 20; ++position) {
        const std::int32_t slot = position + 8;
        const world::detail::PlayerItemRecord& item = world::detail::BorrowPlayerItemsFDA8A4()[slot];
        if (item.used == 0) {
            continue;
        }
        const std::int32_t centerX = x + 50 * (position % 5) + 37;
        const std::int32_t centerY = y + 52 * (position / 5) + 51;
        if (std::bit_cast<std::int8_t>(gTradeItemMarksD2D61C[slot]) > 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(centerX - 21), static_cast<std::uint16_t>(centerY - 21),
                static_cast<std::uint16_t>(centerX + 21), static_cast<std::uint16_t>(centerY + 21),
                223, 251, 0);
        } else if (occupancy[slot] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(centerX - 24), static_cast<std::uint16_t>(centerY - 24),
                static_cast<std::uint16_t>(centerX + 24), static_cast<std::uint16_t>(centerY + 24),
                223, 249, 0);
        }
        if ((item.flags & 1u) == 0) {
            static_cast<void>(login::detail::DrawGraphic(centerX, centerY, 223, 23040, 0, nullptr));
        }
        const render::GraphicDescriptor16* const mode =
            std::bit_cast<std::int8_t>(gTradeItemMarksD2D61C[slot]) > 0 || occupancy[slot] != 0
            ? &dimmed : descriptor;
        static_cast<void>(login::detail::DrawGraphic(centerX, centerY, 223, item.imageNo, 0, mode));
        const std::int32_t count = item.count;
        if (count > 0) {
            cg::text::FormatTo(buffer, L"{:3}", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerX - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerY + 7)),
                3, 1, 1, buffer, 0, 0));
        }
    }
}

// 原函数：0x004B2060。
void WindowTrade_HandleOwnPets(
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const std::int32_t scroll, const render::GraphicDescriptor16* const descriptor)
{
    std::int16_t rowY[3] {26, 95, 164};
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 3) {
        events &= 0x9Fu;
    }
    if (gTradePhaseCAFEC4 != 0) {
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX();
        const std::int32_t dropY = WindowDrag_ReadY();
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        // 原三坐标却扫描五次，沿既定异常边界，不补坐标或模拟栈残值。
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t top = y + rowY[row];
            if (dropX >= x + 251 || x + 11 > dropX || dropY >= top + 69 || top > dropY) {
                continue;
            }
            const std::int32_t position = scroll + row;
            if (position >= 0) {
                const std::int32_t target =
                    std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index);
                if (gTradePetMarksCF3AFC[target] == 0) {
                    WindowItem_RequestPetTransfer(43, 11, source, target);
                }
            }
            break;
        }
        WindowDrag_Reset();
    }
    if (*tab != 1) {
        return;
    }
    std::int32_t hovered = -1;
    const bool hover = (events & 0x21u) != 0;
    if (hover) {
        for (std::int32_t row = 0; row < 3; ++row) {
            const std::int32_t top = y + rowY[row];
            if (login::detail::Sub_5049A0(x + 11, top, x + 251, top + 69, -1) != 0) {
                hovered = scroll + row;
                break;
            }
        }
    }
    std::int32_t petIndex = -1;
    if (hovered >= 0) {
        petIndex = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[hovered].index);
        if (petIndex >= 0) {
            gHoverTextD68AE8 = L"用滑鼠拖放方式，登錄此筆寵物交易。";
            if ((platform::BorrowLegacyMouseStateStorage()->edgeButtons & 0x10u) != 0
                && (WindowDrag_ReadFocusWindowId() == 43 || WindowDrag_ReadFocusWindowId() == 0)) {
                if ((*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].used != 0
                    && gTradePetMarksCF3AFC[petIndex] == 0) {
                    std::int32_t target = 0;
                    while (target < 5 && WindowTrade_ReadSelectedPet(target) >= 0) {
                        ++target;
                    }
                    if (target < 5) {
                        static_cast<void>(WindowTrade_RemovePet(target));
                        static_cast<void>(WindowTrade_AddPet(target, petIndex));
                        *tab = 1;
                    }
                    if (gTradeValueC8AF81 != 1) {
                        gTradeValueC8AF81 = 1;
                    }
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
            }
        }
    }
    if ((events & 1u) != 0 && petIndex >= 0 && gTradePhaseCAFEC4 == 0
        && (events & 2u) != 0 && (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].used != 0
        && gTradePetMarksCF3AFC[petIndex] == 0) {
        static_cast<void>(WindowPetDrag_Start(petIndex, 1));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (WindowDrag_ReadKind() == 3) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (WindowDrag_ReadFocusWindowId() == 43 && WindowDrag_ReadFocusElement() == 11) {
            const std::int32_t row = WindowItem_FindSortedPetIndex(source) - scroll;
            if (row >= 0 && row < 3) {
                const std::int32_t top = y + rowY[row];
                login::detail::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                    223, 254, 0);
            }
        }
    }
    if (hover) {
        const std::int32_t row = hovered - scroll;
        if (row >= 0 && row < 3) {
            const std::int32_t top = y + rowY[row];
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                223, login::detail::gValueD69768, 0);
            if ((*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].used != 0) {
                gTradePreviewCC2E64 = petIndex + 300;
            }
        }
    }
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t slot =
            std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[scroll + row].index);
        const std::int32_t left = x + 11;
        const std::int32_t top = y + rowY[row];
        if (std::bit_cast<std::int8_t>(gTradePetMarksCF3AFC[slot]) > 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 238), static_cast<std::uint16_t>(top + 67),
                223, 251, 0);
        }
        const std::int32_t current =
            std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[scroll + row].index);
        WindowBank_DrawOwnPetTooltip(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(left)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(top)), current, 3);
        static_cast<void>(login::detail::DrawGraphic(
            left + 120, top + 35, 223, 243141, 0, descriptor));
    }
}

// 原函数：0x004B2480。
std::int32_t WindowTrade_HandleOwnPanel(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* const window = gCurrentWindow;
    const auto* const graphic = static_cast<const WindowGraphicData*>(window->elements[index].data);
    const std::uint32_t color = graphic->color;
    descriptor.bytes[13] = 4;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    static constexpr std::array<std::int32_t, 3> backgrounds54F530 {243143, 243145, 243144};
    const std::int32_t amount = world::detail::ReadPlayerValueFC();
    return WindowTrade_HandlePanel(
        window, index, state, backgrounds54F530.data(), &gTradeValueD6742C,
        10, &gTradeScrollCF3B0C, 5, amount, 0, 0, amount, &descriptor, WindowTrade_RefreshOwnPanel);
}

// 原函数：0x004B2500。
void WindowTrade_RefreshOwnPanel(
    const std::int16_t x, const std::int16_t y,
    const std::int32_t mouseX, const std::int32_t mouseY,
    const std::int32_t state, std::uint8_t* const tab, const std::int32_t scroll,
    const std::uint8_t numericResult, std::uint32_t* const valueSlot,
    std::int32_t* const itemCount, std::int32_t* const petCount, std::int32_t*,
    std::int32_t* const displayAmount, render::GraphicDescriptor16* const descriptor)
{
    const bool confirmed = (numericResult & 2u) != 0;
    if (confirmed && gTradeValueC8AF81 != 2) {
        gTradeValueC8AF81 = 2;
    }
    WindowTrade_HandleOwnInventory(tab, x, y, mouseX, mouseY, state, descriptor);
    WindowTrade_HandleOwnPets(tab, x, y, mouseX, mouseY, state, scroll, descriptor);
    if (confirmed) {
        gTradeValueD046D0 = std::bit_cast<std::int32_t>(*valueSlot);
        *valueSlot = 0;
    }
    *displayAmount = std::bit_cast<std::int32_t>(*valueSlot);
    std::int32_t items = 0;
    for (std::int32_t position = 0; position < 20; ++position) {
        std::uint16_t used;
        std::memcpy(&used, gTradeItemsD460D0.data() + (position + 20) * 0xC4C, sizeof(used));
        if (used != 0) {
            ++items;
        }
    }
    *itemCount = items;
    std::int32_t pets = 0;
    for (std::int32_t position = 0; position < 5; ++position) {
        std::uint16_t used;
        std::memcpy(&used, gTradePetsC9C108.data() + (position + 5) * 0x5FC, sizeof(used));
        if (used != 0) {
            ++pets;
        }
    }
    *petCount = pets;
}

// 原函数：0x004B25D0。
void WindowTrade_HandleOwnItemDrag(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (message != 0) {
        if (message == 2 || message == 4) {
            static_cast<void>(WindowTrade_RemoveItem(sourceIndex));
        }
        return;
    }
    if (WindowTrade_ReadSelectedItem(sourceIndex) < 0
        || world::detail::BorrowPlayerItemsFDA8A4()[WindowTrade_ReadSelectedItem(sourceIndex)].used == 0) {
        static_cast<void>(WindowDrag_ResetMouse());
    }
    if ((platform::BorrowLegacyMouseStateStorage()->edgeButtons & 1u) != 0) {
        static_cast<void>(WindowDrag_ResetMouse());
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseX.value()));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseY.value()));
        static_cast<void>(WindowDrag_StartAtPosition(6, sourceIndex, x, y, WindowTrade_HandleOwnItemDrag));
    }
    if ((platform::BorrowLegacyMouseStateStorage()->edgeButtons & 2u) != 0) {
        WindowDrag_Cancel();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t imageNo =
        world::detail::BorrowPlayerItemsFDA8A4()[WindowTrade_ReadSelectedItem(sourceIndex)].imageNo;
    const std::int32_t y =
        platform::BorrowLegacyMouseStateStorage()->mouseY.value();
    const std::int32_t x =
        platform::BorrowLegacyMouseStateStorage()->mouseX.value();
    static_cast<void>(login::detail::DrawGraphic(x, y, 230, imageNo, 0, &descriptor));
}

// 原函数：0x004B2E00。
void WindowTrade_HandleOwnPetDrag(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (message != 0) {
        if (message == 2 || message == 4) {
            static_cast<void>(WindowTrade_RemovePet(sourceIndex));
        }
        return;
    }
    if (WindowTrade_ReadSelectedPet(sourceIndex) < 0
        || (*world::detail::BorrowPetRuntimeF633F8Storage())[WindowTrade_ReadSelectedPet(sourceIndex)].used == 0) {
        static_cast<void>(WindowDrag_ResetMouse());
    }
    if ((platform::BorrowLegacyMouseStateStorage()->edgeButtons & 1u) != 0) {
        static_cast<void>(WindowDrag_ResetMouse());
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseX.value()));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseY.value()));
        static_cast<void>(WindowDrag_StartAtPosition(7, sourceIndex, x, y, WindowTrade_HandleOwnPetDrag));
    }
    if ((platform::BorrowLegacyMouseStateStorage()->edgeButtons & 2u) != 0) {
        WindowDrag_Cancel();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t slot = WindowTrade_ReadSelectedPet(sourceIndex);
    const auto petY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        (platform::BorrowLegacyMouseStateStorage()->mouseY - 35)));
    const auto petX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        (platform::BorrowLegacyMouseStateStorage()->mouseX - 120)));
    WindowBank_DrawOwnPetTooltip(petX, petY, slot, 5);
    const std::int32_t y =
        platform::BorrowLegacyMouseStateStorage()->mouseY.value();
    const std::int32_t x =
        platform::BorrowLegacyMouseStateStorage()->mouseX.value();
    static_cast<void>(login::detail::DrawGraphic(x, y, 230, 243141, 0, &descriptor));
}

// 原函数：0x004B2710。
std::int32_t WindowTrade_StartOwnItemDrag(const std::int32_t index)
{
    return WindowDrag_Start(6, index, WindowTrade_HandleOwnItemDrag);
}

// 原函数：0x004B2F50。
std::int32_t WindowTrade_StartOwnPetDrag(const std::int32_t index)
{
    return WindowDrag_Start(7, index, WindowTrade_HandleOwnPetDrag);
}

// 原函数：0x004B2730。
void WindowTrade_HandleSelectedItem(
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const render::GraphicDescriptor16* const descriptor)
{
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 2 && WindowDrag_ReadKind() != 6) {
        events &= 0x9Fu;
    }
    if (gTradePhaseCAFEC4 != 0) {
        events &= 0x9Fu;
    }
    if (*tab != 0) {
        if ((events & 0x40u) != 0) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            if (WindowItem_ClassifyItemSlot(source) == 2) {
                std::int32_t target = 0;
                while (target < 20 && WindowTrade_ReadSelectedItem(target) >= 0) {
                    ++target;
                }
                if (target < 20 && (world::detail::BorrowPlayerItemsFDA8A4()[source].flags & 1u) != 0) {
                    static_cast<void>(WindowTrade_RemoveItem(target));
                    static_cast<void>(WindowTrade_AddItem(target, source));
                    *tab = 0;
                }
            }
            WindowDrag_Reset();
        }
        if ((events & 0x20u) != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 273), static_cast<std::uint16_t>(y + 280),
                223, login::detail::gValueD69768, 0);
        }
        return;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX();
        const std::int32_t dropY = WindowDrag_ReadY();
        const std::int32_t kind = WindowDrag_ReadKind();
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        for (std::int32_t target = 0; target < 20; ++target) {
            const std::int32_t left = x + 50 * (target % 5) + 13;
            const std::int32_t top = y + 52 * (target / 5) + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (kind == 2) {
                if (WindowItem_ClassifyItemSlot(source) == 2
                    && (world::detail::BorrowPlayerItemsFDA8A4()[source].flags & 1u) != 0) {
                    static_cast<void>(WindowTrade_RemoveItem(target));
                    static_cast<void>(WindowTrade_AddItem(target, source));
                }
            } else if (kind == 6) {
                static_cast<void>(WindowTrade_SwapSelectedItems(source, target));
            }
            WindowDrag_Reset();
            break;
        }
    }
    std::int32_t hovered = -1;
    const bool hover = (events & 0x21u) != 0;
    if (hover) {
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            const std::int32_t left = x + 50 * (slot % 5) + 13;
            const std::int32_t top = y + 52 * (slot / 5) + 26;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = slot;
                break;
            }
        }
    }
    const std::uint8_t phase = gTradePhaseCAFEC4;
    if (phase == 0 && hovered >= 0) {
        gHoverTextD68AE8 = L"用滑鼠拖放方式，取消此筆物品交易。";
    }
    if (gTradeValueD2EA20 == 1) {
        if (gTradeHoverSlotC8AA78 != hovered) {
            gTradeTooltipPageCB10D4 = 0;
        }
        gTradeHoverSlotC8AA78 = hovered;
    }
    if (hovered >= 0 && (platform::BorrowLegacyMouseStateStorage()->edgeButtons & 0x10u) != 0
        && (phase != 0 || ((WindowDrag_ReadFocusWindowId() == 43 || WindowDrag_ReadFocusWindowId() == 0)
                           && WindowTrade_RemoveItem(hovered) < 0))) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    if ((events & 1u) != 0 && hovered >= 0 && WindowTrade_ReadSelectedItem(hovered) >= 0) {
        if (gTradePhaseCAFEC4 == 0 && (events & 2u) != 0) {
            static_cast<void>(WindowTrade_StartOwnItemDrag(hovered));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((events & 4u) != 0) {
            gTradeTooltipPageCB10D4 = static_cast<std::uint8_t>(gTradeTooltipPageCB10D4 + 1);
            const std::int32_t slot = WindowTrade_ReadSelectedItem(hovered);
            if (std::bit_cast<std::int8_t>(gTradeTooltipPageCB10D4)
                >= world::detail::BorrowPlayerItemsFDA8A4()[slot].pages) {
                gTradeTooltipPageCB10D4 = 0;
            }
        }
    }
    if (WindowDrag_ReadKind() == 6 && WindowDrag_ReadFocusWindowId() == 43 && WindowDrag_ReadFocusElement() == 8) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (WindowTrade_ReadSelectedItem(source) >= 0
            && world::detail::BorrowPlayerItemsFDA8A4()[WindowTrade_ReadSelectedItem(source)].used != 0) {
            const std::int32_t left = x + 50 * (source % 5) + 13;
            const std::int32_t top = y + 52 * (source / 5) + 26;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    std::wstring buffer;
    if (hover && hovered >= 0) {
        const std::int32_t left = x + 50 * (hovered % 5) + 13;
        const std::int32_t top = y + 52 * (hovered / 5) + 26;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        const std::int32_t slot = WindowTrade_ReadSelectedItem(hovered);
        if (slot >= 0) {
            const world::detail::PlayerItemRecord& item = world::detail::BorrowPlayerItemsFDA8A4()[slot];
            if (item.used != 0) {
                gTradePreviewCC2E64 = slot;
                if (!item.memo.empty() && (item.flags & 0x100u) != 0) {
                    gHoverTextD68AE8 = item.name + L"[kok]";
                } else if ((item.flags & 0x200u) != 0) {
                    gHoverTextD68AE8 = item.memo + L"[han]";
                }
            }
        }
        gTradeValueD2EA20 = 1;
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        const std::int32_t slot = WindowTrade_ReadSelectedItem(position);
        if (slot >= 0 && world::detail::BorrowPlayerItemsFDA8A4()[slot].used != 0
            && (world::detail::BorrowPlayerItemsFDA8A4()[slot].flags & 1u) == 0) {
            static_cast<void>(WindowTrade_RemoveItem(position));
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    for (std::int32_t position = 0; position < 20; ++position) {
        const std::int32_t slot = WindowTrade_ReadSelectedItem(position);
        if (slot < 0) {
            continue;
        }
        const world::detail::PlayerItemRecord& item = world::detail::BorrowPlayerItemsFDA8A4()[slot];
        if (item.used == 0) {
            continue;
        }
        const std::int32_t centerX = x + 50 * (position % 5) + 37;
        const std::int32_t centerY = y + 52 * (position / 5) + 51;
        if ((item.flags & 1u) == 0) {
            static_cast<void>(login::detail::DrawGraphic(
                centerX, centerY, 223, 23040, 0, nullptr));
        }
        static_cast<void>(login::detail::DrawGraphic(
            centerX, centerY, 223, item.imageNo, 0, descriptor));
        const std::int32_t count = item.count;
        if (count > 0) {
            cg::text::FormatTo(buffer, L"{:3}", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerX - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerY + 7)),
                3, 1, 1, buffer, 0, 0));
        }
    }
}

// 原函数：0x004B2F70。
void WindowTrade_HandleSelectedPet(
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const std::int32_t scroll, const render::GraphicDescriptor16* const descriptor)
{
    std::int16_t rowY[3] {26, 95, 164};
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 3 && WindowDrag_ReadKind() != 7) {
        events &= 0x9Fu;
    }
    if (gTradePhaseCAFEC4 != 0) {
        events &= 0x9Fu;
    }
    if (*tab != 1) {
        if ((events & 0x40u) != 0) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            if (WindowItem_ClassifyPetSlot(source) == 1) {
                std::int32_t target = 0;
                while (target < 5 && WindowTrade_ReadSelectedPet(target) >= 0) {
                    ++target;
                }
                if (target < 5) {
                    static_cast<void>(WindowTrade_RemovePet(target));
                    static_cast<void>(WindowTrade_AddPet(target, source));
                    *tab = 1;
                }
            }
            WindowDrag_Reset();
        }
        if ((events & 0x20u) != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 273), static_cast<std::uint16_t>(y + 280),
                223, login::detail::gValueD69768, 0);
        }
        return;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX();
        const std::int32_t dropY = WindowDrag_ReadY();
        const std::int32_t kind = WindowDrag_ReadKind();
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        // 原仅初始化三项却扫描五次；不补坐标或模拟x86栈残值，异常边界见SPEC。
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t top = y + rowY[row];
            if (dropX >= x + 251 || x + 11 > dropX || dropY >= top + 69 || top > dropY) {
                continue;
            }
            const std::int32_t target = scroll + row;
            if (target >= 0) {
                if (kind == 3) {
                    if (WindowItem_ClassifyPetSlot(source) == 1) {
                        static_cast<void>(WindowTrade_RemovePet(target));
                        static_cast<void>(WindowTrade_AddPet(target, source));
                    }
                } else if (kind == 7) {
                    static_cast<void>(WindowTrade_SwapSelectedPets(source, target));
                }
                WindowDrag_Reset();
            }
            break;
        }
    }
    if (*tab != 1) {
        return;
    }
    std::int32_t hovered = -1;
    const bool hover = (events & 0x21u) != 0;
    if (hover) {
        for (std::int32_t row = 0; row < 3; ++row) {
            const std::int32_t top = y + rowY[row];
            if (login::detail::Sub_5049A0(x + 11, top, x + 251, top + 69, -1) != 0) {
                hovered = scroll + row;
                break;
            }
        }
    }
    const std::uint8_t phase = gTradePhaseCAFEC4;
    if (phase == 0 && hovered >= 0) {
        gHoverTextD68AE8 = L"用滑鼠拖放方式，取消此筆寵物交易。";
    }
    if (hovered >= 0 && (platform::BorrowLegacyMouseStateStorage()->edgeButtons & 0x10u) != 0
        && (phase != 0 || ((WindowDrag_ReadFocusWindowId() == 43 || WindowDrag_ReadFocusWindowId() == 0)
                           && WindowTrade_RemovePet(hovered) < 0))) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    if ((events & 1u) != 0 && hovered >= 0 && WindowTrade_ReadSelectedPet(hovered) >= 0
        && gTradePhaseCAFEC4 == 0 && (events & 2u) != 0) {
        static_cast<void>(WindowTrade_StartOwnPetDrag(hovered));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (WindowDrag_ReadKind() == 7 && WindowDrag_ReadFocusWindowId() == 43 && WindowDrag_ReadFocusElement() == 8) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (WindowTrade_ReadSelectedPet(source) >= 0
            && (*world::detail::BorrowPetRuntimeF633F8Storage())[WindowTrade_ReadSelectedPet(source)].used != 0) {
            const std::int32_t row = source - scroll;
            if (row >= 0 && row < 3) {
                const std::int32_t top = y + rowY[row];
                login::detail::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                    223, 254, 0);
            }
        }
    }
    if (hover) {
        const std::int32_t row = hovered - scroll;
        if (row >= 0 && row < 3) {
            const std::int32_t top = y + rowY[row];
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                223, login::detail::gValueD69768, 0);
            const std::int32_t slot = WindowTrade_ReadSelectedPet(hovered);
            if (slot >= 0 && (*world::detail::BorrowPetRuntimeF633F8Storage())[slot].used != 0) {
                gTradePreviewCC2E64 = slot + 300;
            }
        }
    }
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t left = x + 11;
        const std::int32_t top = y + rowY[row];
        const std::int32_t slot = WindowTrade_ReadSelectedPet(scroll + row);
        if (slot >= 0 && (*world::detail::BorrowPetRuntimeF633F8Storage())[slot].used != 0) {
            WindowBank_DrawOwnPetTooltip(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(left)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(top)), slot, 3);
            static_cast<void>(login::detail::DrawGraphic(
                left + 120, top + 35, 223, 243141, 0, descriptor));
        } else {
            static_cast<void>(render::DrawGraphic3_520970(
                left, top, 223, 243141, 0, 1, 0, descriptor));
        }
    }
}

// 原函数：0x004B3430。
std::int32_t WindowTrade_HandleSelectedPanel(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* const window = gCurrentWindow;
    const auto* const graphic = static_cast<const WindowGraphicData*>(window->elements[index].data);
    const std::uint32_t color = graphic->color;
    descriptor.bytes[13] = 4;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    if (gTradeValueD2E2B0 == 2) {
        descriptor.bytes[11] >>= 1;
    }
    static constexpr std::array<std::int32_t, 3> backgrounds54F53C {244367, 244371, 244369};
    const std::int32_t amount = world::detail::ReadPlayerValueFC();
    return WindowTrade_HandlePanel(
        window, index, state, backgrounds54F53C.data(), &gTradeValueC8AF81,
        7, &gTradeScrollCC4C90, -1, 0, 0, 0, amount, &descriptor, WindowTrade_RefreshSelectedPanel);
}

// 原函数：0x004B34C0。
void WindowTrade_RefreshSelectedPanel(
    const std::int16_t x, const std::int16_t y,
    const std::int32_t mouseX, const std::int32_t mouseY, const std::int32_t state,
    std::uint8_t* const tab, const std::int32_t scroll,
    const std::uint8_t numericResult, std::uint32_t*,
    std::int32_t* const itemCount, std::int32_t* const petCount, std::int32_t* const amount,
    std::int32_t* const displayAmount, render::GraphicDescriptor16* const descriptor)
{
    if ((numericResult & 2u) != 0 && gTradeValueD6742C != 2) {
        gTradeValueD6742C = 2;
    }
    WindowTrade_HandleSelectedItem(tab, x, y, mouseX, mouseY, state, descriptor);
    WindowTrade_HandleSelectedPet(tab, x, y, mouseX, mouseY, state, scroll, descriptor);
    *displayAmount = gTradeValueD046D0;
    std::int32_t items = 0;
    for (std::int32_t position = 0; position < 20; ++position) {
        if (WindowTrade_ReadSelectedItem(position) >= 0) {
            ++items;
        }
    }
    *itemCount = items;
    std::int32_t pets = 0;
    for (std::int32_t position = 0; position < 5; ++position) {
        if (WindowTrade_ReadSelectedPet(position) >= 0) {
            ++pets;
        }
    }
    *petCount = pets;
    *amount = gTradeValueD046D0;
}

// 原函数：0x004B3580。
std::int32_t WindowTrade_HandleRemoteSelection(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* const window = gCurrentWindow;
    const auto* const graphic = static_cast<const WindowGraphicData*>(window->elements[index].data);
    const std::uint32_t color = graphic->color;
    descriptor.bytes[13] = 4;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    if (gTradeValueCC4224 == 2) {
        descriptor.bytes[11] >>= 1;
    }
    static constexpr std::array<std::int32_t, 3> backgrounds54F548 {244368, 244372, 244370};
    const std::int32_t amount = world::detail::ReadPlayerValueFC();
    return WindowTrade_HandlePanel(
        window, index, state, backgrounds54F548.data(), &gTradeTabCC4D1A,
        13, &gTradeScrollD67430, -1, 0, 0, 0, amount, &descriptor, WindowTrade_HandleRemoteItem);
}

// 原函数：0x004B3610。
void WindowTrade_HandleRemoteItem(
    const std::int16_t x, const std::int16_t y, const std::int32_t, const std::int32_t,
    const std::int32_t state, std::uint8_t* const tab, const std::int32_t scroll,
    const std::uint8_t, std::uint32_t*,
    std::int32_t* const itemCount, std::int32_t* const petCount, std::int32_t* const amount,
    std::int32_t* const displayAmount, render::GraphicDescriptor16* const descriptor)
{
    const std::uint8_t page = *tab;
    std::wstring buffer;
    if (page == 0) {
        std::int32_t hovered = -1;
        if ((state & 1) != 0) {
            for (std::int32_t slot = 0; slot < 20; ++slot) {
                const std::int32_t left = x + 50 * (slot % 5) + 13;
                const std::int32_t top = y + 52 * (slot / 5) + 26;
                if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    hovered = slot;
                    std::uint16_t used;
                    std::memcpy(&used, gTradeItemsD460D0.data() + slot * 0xC4C, sizeof(used));
                    if (used != 0) {
                        gHoverTextD68AE8 = L"以此物品向對方提出交易。";
                    }
                    break;
                }
            }
        }
        if (gTradeValueD2EA20 == 2) {
            if (gTradeHoverSlotC8B314 != hovered) {
                gTradeTooltipPageCB10D4 = 0;
            }
            gTradeHoverSlotC8B314 = hovered;
        }
        if (hovered >= 0 && (state & 4) != 0) {
            const char* const record = gTradeItemsD460D0.data() + hovered * 0xC4C;
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                std::int32_t pages;
                std::memcpy(&pages, record + 0xC30, sizeof(pages));
                gTradeTooltipPageCB10D4 = static_cast<std::uint8_t>(gTradeTooltipPageCB10D4 + 1);
                if (std::bit_cast<std::int8_t>(gTradeTooltipPageCB10D4) >= pages) {
                    gTradeTooltipPageCB10D4 = 0;
                }
            }
        }
        if ((state & 1) != 0 && hovered >= 0) {
            const std::int32_t left = x + 50 * (hovered % 5) + 13;
            const std::int32_t top = y + 52 * (hovered / 5) + 25;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, login::detail::gValueD69768, 0);
            const char* const record = gTradeItemsD460D0.data() + hovered * 0xC4C;
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                const char* const memo = record + 0xC38;
                const std::size_t memoLength = std::strlen(memo);
                gTradePreviewCC2E64 = hovered + 200;
                bool showName = false;
                if (memoLength != 0) {
                    std::uint32_t flags;
                    std::memcpy(&flags, record + 0xC34, sizeof(flags));
                    showName = (flags & 0x100u) != 0;
                }
                if (showName) {
                    cg::text::FormatTo(buffer, L"{}[kok]", cg::text::FromGbkWide(record + 0x10).c_str());
                    gHoverTextD68AE8 = buffer;
                } else {
                    std::uint32_t flags;
                    std::memcpy(&flags, record + 0xC34, sizeof(flags));
                    if ((flags & 0x200u) != 0) {
                        cg::text::FormatTo(buffer, L"{}[han]", cg::text::FromGbkWide(memo).c_str());
                        gHoverTextD68AE8 = buffer;
                    }
                }
            }
            gTradeValueD2EA20 = 2;
        }
        static_cast<void>(render::ClearTextQueueTail_456D50(3));
        static_cast<void>(render::DrawText_456780(
            0, 0, 3, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            const char* const record = gTradeItemsD460D0.data() + slot * 0xC4C;
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used == 0) {
                continue;
            }
            const std::int32_t centerX = x + 50 * (slot % 5) + 37;
            const std::int32_t centerY = y + 52 * (slot / 5) + 51;
            std::int16_t value02;
            std::memcpy(&value02, record + 2, sizeof(value02));
            if (value02 < 0) {
                static_cast<void>(login::detail::DrawGraphic(
                    centerX, centerY, 223, 23040, 0, nullptr));
            }
            std::int32_t imageNo;
            std::memcpy(&imageNo, record + 4, sizeof(imageNo));
            static_cast<void>(login::detail::DrawGraphic(
                centerX, centerY, 223, imageNo, 0, descriptor));
            std::int32_t count;
            std::memcpy(&count, record + 8, sizeof(count));
            if (count > 0) {
                cg::text::FormatTo(buffer, L"{:3}", count);
                static_cast<void>(render::DrawText_456780(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerX - 3)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerY + 7)),
                    3, 1, 1, buffer, 0, 0));
            }
        }
    } else if (page == 1) {
        constexpr std::array<std::int16_t, 3> rowY {26, 95, 164};
        if ((state & 1) != 0) {
            for (std::int32_t row = 0; row < 3; ++row) {
                const std::int32_t top = y + rowY[row];
                if (login::detail::Sub_5049A0(x + 11, top, x + 251, top + 69, -1) != 0) {
                    const std::int32_t slot = scroll + row;
                    if (slot >= 0) {
                        std::uint16_t used;
                        std::memcpy(&used, gTradePetsC9C108.data() + slot * 0x5FC, sizeof(used));
                        if (used != 0) {
                            gHoverTextD68AE8 = L"以此寵物向對方提出交易。";
                            gTradePreviewCC2E64 = slot + 500;
                        }
                    }
                    break;
                }
            }
        }
        for (std::int32_t row = 0; row < 3; ++row) {
            static_cast<void>(WindowTrade_DrawPetRow(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 11)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + rowY[row])),
                0, scroll + row, 3, 223, descriptor));
        }
    }
    *displayAmount = gTradeValueCF4688;
    std::int32_t items = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        std::uint16_t used;
        std::memcpy(&used, gTradeItemsD460D0.data() + slot * 0xC4C, sizeof(used));
        if (used != 0) {
            ++items;
        }
    }
    *itemCount = items;
    std::int32_t pets = 0;
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        std::uint16_t used;
        std::memcpy(&used, gTradePetsC9C108.data() + slot * 0x5FC, sizeof(used));
        if (used != 0) {
            ++pets;
        }
    }
    *petCount = pets;
    *amount = gTradeValueCF4688;
}

// 原函数：0x004B1900。
bool WindowTrade_HitTestOwnPetRow(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height)
{
    const std::int32_t relativeX =
        (platform::BorrowLegacyMouseStateStorage()->mouseX - static_cast<std::int32_t>(x));
    if (relativeX < 0 || relativeX >= width) {
        return false;
    }
    const std::int32_t relativeY =
        (platform::BorrowLegacyMouseStateStorage()->mouseY - static_cast<std::int32_t>(y));
    return relativeY >= 0 && relativeY < height;
}

// 原函数：0x004B09E0。
std::int32_t WindowTrade_HandleAmountInput(const std::int32_t, const std::int32_t state)
{
    if (gTradeValueD6742C == 2) {
        if ((state & 2) != 0) {
            WindowInstance* const window = gWindowStates[WindowId_Trade].window;
            static_cast<void>(WindowInput_ActivateElement(&gTradeInputPrototype5803D0, 0, -100, 0));
            input::SetContext_44B330(&input::gInputContextCC21E8);
            gFocusedElementDataD45F94 = window->elements[0].data;
            auto* const data = static_cast<WindowInputData*>(window->elements[0].data);
            data->context = &input::gInputContextCC21E8;
        }
    } else if (input::gCurrentInputData == &input::gInputContextCC21E8) {
        static_cast<void>(WindowChat_RestoreInput());
    }
    return 0;
}

// 原函数：0x004B4070。
std::int32_t WindowTrade_DrawRemotePetSkillRow(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t skill)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const char* const text = gPetStorageD64E18.data() + index * 0x600 + 0x62 + skill * 0x8C;
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 7)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
            5, 12, 0, cg::text::FromGbkWide(text), 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return login::detail::DrawGraphic(
        shortX + 105, shortY + 8, 0xE6, 0x3B59C, 0, &descriptor);
}

// 原函数：0x004B4140。
std::int32_t WindowTrade_DrawRemotePetTooltip(
    const std::int32_t x, const std::int32_t y, const std::int32_t index)
{
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortIndex =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(index));
    const char* const record = gPetStorageD64E18.data() + shortIndex * 0x600;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return 0;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    const char* const title = record + 0x5E5;
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 60)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 11)),
            5, 12, 0, cg::text::FromGbkWide(title).c_str(), 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::int32_t value;
        std::memcpy(&value, record + 0x30 + row * 4, sizeof(value));
        cg::text::FormatTo(buffer, L"{:10}", value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 144)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(shortY + 23 + row * 15)),
            5, 12, 0, buffer, 0, 0));
    }
    std::int32_t maximumWeight;
    std::memcpy(&maximumWeight, record + 0x5FC, sizeof(maximumWeight));
    if (maximumWeight > 0) {
        std::int32_t weight;
        std::memcpy(&weight, record + 0x5F8, sizeof(weight));
        cg::text::FormatTo(buffer, L"{:3} / {:3}", weight, maximumWeight);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 61)),
            31, 5, 12, 0, buffer, 0, 0));
    }
    if (gPreviewActionD67B84 == nullptr) {
        gPreviewActionD67B84 = login::detail::NewYobiAction_404AE0(2, 0);
    }
    std::int32_t imageNo;
    std::memcpy(&imageNo, record + 4, sizeof(imageNo));
    login::detail::ConfigureWindowPetPreview(gPreviewActionD67B84, imageNo);
    static_cast<void>(login::detail::Sub_517880(gPreviewActionD67B84, 100, 0, 0));
    login::YobiAction* const action = gPreviewActionD67B84;
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const render::GraphicDescriptor16* const mode =
        login::detail::BorrowYobiActionDrawMode(action);
    const std::int32_t graphicIndex = login::detail::GetYobiActionField20(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 0xE6, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        std::int32_t value;
        std::memcpy(&value, record + 0x48 + row * 4, sizeof(value));
        const std::int32_t segments = value / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 0xE6, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 0xE6, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[3], 0, 1, 0, &descriptor));
        }
    }
    for (std::int32_t skill = 0; skill < 10; ++skill) {
        static_cast<void>(WindowTrade_DrawRemotePetSkillRow(
            shortX + 10, shortY + 141 + skill * 16, shortIndex, skill));
    }
    return render::DrawGraphic3_520970(
        shortX, shortY, 0xE6, 0x3BA95, 0, 1, 0, &descriptor);
}

// 原函数：0x004B45A0。
std::int32_t WindowTrade_DrawSelectedPetSkillRow(
    const std::int32_t x, const std::int32_t y, const std::int32_t side,
    const std::int32_t index, const std::int32_t skill)
{
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortSide =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(side));
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const char* const text =
        gTradePetsC9C108.data() + (index + shortSide * 5) * 0x5FC + 0x66 + skill * 0x8C;
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 7)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
            5, 12, 0, cg::text::FromGbkWide(text), 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return login::detail::DrawGraphic(
        shortX + 105, shortY + 8, 0xE6, 0x3B59C, 0, &descriptor);
}

// 原函数：0x004B4680。
std::int32_t WindowTrade_DrawSelectedPetTooltip(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t index)
{
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortIndex =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(index));
    const std::int32_t shortSide =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(side));
    const std::int32_t slot = shortIndex + shortSide * 5;
    const char* const record = gTradePetsC9C108.data() + slot * 0x5FC;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return slot;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    const char* const title = record + 0x5E9;
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 60)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 11)),
            5, 12, 0, cg::text::FromGbkWide(title).c_str(), 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::int32_t value;
        std::memcpy(&value, record + 0x38 + row * 4, sizeof(value));
        cg::text::FormatTo(buffer, L"{:10}", value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 144)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(shortY + 23 + row * 15)),
            5, 12, 0, buffer, 0, 0));
    }
    if (gPreviewActionD67B84 == nullptr) {
        gPreviewActionD67B84 = login::detail::NewYobiAction_404AE0(2, 0);
    }
    std::int32_t imageNo;
    std::memcpy(&imageNo, record + 8, sizeof(imageNo));
    login::detail::ConfigureWindowPetPreview(gPreviewActionD67B84, imageNo);
    static_cast<void>(login::detail::Sub_517880(gPreviewActionD67B84, 100, 0, 0));
    login::YobiAction* const action = gPreviewActionD67B84;
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const render::GraphicDescriptor16* const mode =
        login::detail::BorrowYobiActionDrawMode(action);
    const std::int32_t graphicIndex = login::detail::GetYobiActionField20(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 0xE6, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        std::int32_t value;
        std::memcpy(&value, record + 0x4C + row * 4, sizeof(value));
        const std::int32_t segments = value / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 0xE6, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 0xE6, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[3], 0, 1, 0, &descriptor));
        }
    }
    for (std::int32_t skill = 0; skill < 10; ++skill) {
        static_cast<void>(WindowTrade_DrawSelectedPetSkillRow(
            shortX + 10, shortY + 141 + skill * 16, side, shortIndex, skill));
    }
    return render::DrawGraphic3_520970(
        shortX, shortY, 0xE6, 0x3BA95, 0, 1, 0, &descriptor);
}

// 原函数：0x004B3B00。
std::int32_t WindowTrade_DrawOwnPetSkillRow(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t petIndex, const std::int32_t skillPosition)
{
    render::GraphicDescriptor16 descriptor {};
    const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex];
    const std::int16_t skillIndex =
        std::bit_cast<std::int16_t>(pet.techniquePairs650[skillPosition].index);
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const char* const text = pet.techniquesD8[skillIndex].name.data();
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 7)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
            5, 12, 0, cg::text::FromGbkWide(text).c_str(), 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return login::detail::DrawGraphic(
        shortX + 105, shortY + 8, 0xE6, 0x3B59C, 0, &descriptor);
}

// 原函数：0x004B3BF0。
std::int32_t WindowTrade_DrawOwnPetTooltip(
    const std::int32_t x, const std::int32_t y, const std::int32_t index)
{
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortIndex =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(index));
    const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[shortIndex];
    if (pet.used == 0) {
        return 0;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    const char* const title = pet.name6AC.data();
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 60)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 11)),
            5, 12, 0, cg::text::FromGbkWide(title).c_str(), 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    std::wstring buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t value = pet.values70[5 + row];
        cg::text::FormatTo(buffer, L"{:10}", value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 144)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(shortY + 23 + row * 15)),
            5, 12, 0, buffer, 0, 0));
    }
    const std::int32_t maximumWeight = pet.values6D8[2];
    if (maximumWeight > 0) {
        const std::int32_t weight = pet.values6D8[1];
        cg::text::FormatTo(buffer, L"{:3} / {:3}", weight, maximumWeight);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 61)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 31)),
            5, 12, 0, buffer, 0, 0));
    }
    if (gPreviewActionD67B84 == nullptr) {
        gPreviewActionD67B84 = login::detail::NewYobiAction_404AE0(2, 0);
    }
    const std::int32_t imageNo = pet.imageNo6A0;
    login::detail::ConfigureWindowPetPreview(gPreviewActionD67B84, imageNo);
    static_cast<void>(login::detail::Sub_517880(gPreviewActionD67B84, 100, 0, 0));
    login::YobiAction* const action = gPreviewActionD67B84;
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const render::GraphicDescriptor16* const mode =
        login::detail::BorrowYobiActionDrawMode(action);
    const std::int32_t graphicIndex = login::detail::GetYobiActionField20(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 0xE6, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t segments = pet.values70[21 + row] / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 0xE6, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 0xE6, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[3], 0, 1, 0, &descriptor));
        }
    }
    for (std::int32_t skill = 0; skill < 10; ++skill) {
        static_cast<void>(WindowTrade_DrawOwnPetSkillRow(
            shortX + 10, shortY + 141 + skill * 16, shortIndex, skill));
    }
    return render::DrawGraphic3_520970(
        shortX, shortY, 0xE6, 0x3BA95, 0, 1, 0, &descriptor);
}

// 原函数：0x004AFFE0。
std::int32_t WindowTrade_ReadSelectedItem(const std::int32_t index)
{
    const std::int32_t slot = std::bit_cast<std::int8_t>(gTradeItemSlotsCF3AE8[index]);
    return slot < 0 ? -1 : slot;
}

// 原函数：0x004B00F0。
std::int32_t WindowTrade_ReadSelectedPet(const std::int32_t index)
{
    const std::int32_t slot = std::bit_cast<std::int8_t>(gTradePetSlotsCC4D28[index]);
    return slot < 0 ? -1 : slot;
}

// 原函数：0x004B0A50。
std::int32_t WindowTrade_ButtonPublish(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    if (hover) {
        gHoverTextD68AE8 = L"向對方公開交易內容。";
    }
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        std::array<char, 156> buffer;
        std::array<char, 1024> petSource;
        std::array<char, 1024> itemSource;
        std::array<char, 1024> petCopy;
        std::array<char, 1024> itemCopy;
        itemSource[0] = '\0';
        petSource[0] = '\0';
        std::int32_t count = 0;
        for (std::int32_t position = 0; position < 20; ++position) {
            const std::int32_t slot = WindowTrade_ReadSelectedItem(position);
            if (slot >= 0) {
                if (count != 0) {
                    std::strcat(itemSource.data(), "|");
                }
                std::sprintf(buffer.data(), "%d|%d", slot, position);
                std::strcat(itemSource.data(), buffer.data());
                ++count;
            }
        }
        std::strncpy(itemCopy.data(), itemSource.data(), static_cast<std::uint32_t>(1024));
        count = 0;
        for (std::int32_t position = 0; position < 5; ++position) {
            const std::int32_t slot = WindowTrade_ReadSelectedPet(position);
            if (slot >= 0) {
                if (count != 0) {
                    std::strcat(petSource.data(), "|");
                }
                std::sprintf(buffer.data(), "%d|%d", slot, position);
                std::strcat(petSource.data(), buffer.data());
                ++count;
            }
        }
        std::strncpy(petCopy.data(), petSource.data(), static_cast<std::uint32_t>(1024));
        const std::int32_t amount = gTradeValueD046D0;
        static_cast<void>(network::Sub_512040(
            platform::gConnectionSocket, cg::text::FromGbkWide(itemCopy.data()), cg::text::FromGbkWide(petCopy.data()), amount));
        WindowInstance* const window = gCurrentWindow;
        gTradeValueD2E2B0 = 1;
        gTradeScrollD67430 = 0;
        static_cast<void>(WindowScroll_SetPosition(&window->elements[13], 2, 0));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        result = 1;
    }
    graphic->imageNo = 0x3BA89;
    if (hover) {
        graphic->imageNo = 0x3BA8B;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3BA8A;
    }
    return result;
}

// 原函数：0x004B0CF0。
std::int32_t WindowTrade_ButtonWithdraw(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    if (hover) {
        gHoverTextD68AE8 = L"還是算了。";
    }
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        static_cast<void>(network::Sub_512230(platform::gConnectionSocket));
        const std::int32_t previous = gTradeValueCC4224;
        gTradeValueD2E2B0 = 0;
        if (previous == 2) {
            gTradeValueCC4224 = 1;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        gTradeStepD3040C = 23;
        result = 1;
    }
    graphic->imageNo = 0x3B562;
    if (hover) {
        graphic->imageNo = 0x3B564;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B563;
    }
    return result;
}

// 原函数：0x004B0DB0。
std::int32_t WindowTrade_ButtonConfirm(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    if (hover) {
        gHoverTextD68AE8 = L"確定交易。";
    }
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        const std::int32_t phase = gTradeValueD2E2B0;
        if ((phase == 1 && gTradeValueCC4224 >= 1) || phase == 2) {
            const std::uint32_t tick = platform::ReadTickCount32();
            if (gWindowActionTick62B5AC + 500u < tick) {
                if (phase == 1) {
                    static_cast<void>(network::Sub_512270(platform::gConnectionSocket, 1));
                    gTradeValueD2E2B0 = 2;
                    gTradeValueD67CB8 = 1;
                    gTradeValueD67CBC = 1;
                } else {
                    static_cast<void>(network::Sub_512230(platform::gConnectionSocket));
                    const std::int32_t previous = gTradeValueCC4224;
                    gTradeValueD2E2B0 = 0;
                    if (previous == 2) {
                        gTradeValueCC4224 = 1;
                    }
                }
                gWindowActionTick62B5AC = platform::ReadTickCount32();
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            }
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        result = 1;
    }
    graphic->imageNo = 0x3BA8C;
    if (hover) {
        graphic->imageNo = 0x3BA8E;
    }
    if (gTradeValueD2E2B0 == 2) {
        graphic->imageNo = 0x3BA8D;
    }
    return result;
}

// 原函数：0x004E5620。
login::YobiAction* WindowTrade_ReceiveStart(const char* const name)
{
    gTradeItemsD460D0.fill('\0');
    gTradePetsC9C108.fill('\0');
    gTradeValueCF4A1C = 0;
    gTradeValueD673E0 = 0;
    gTradeValueCC4C94 = 0;
    gTradeValueC8AF68 = 0;
    gTradeValueCF4688 = 0;
    gTradeValueCB03CC = 0;
    gTradeValueCF4AB8 = 0;
    gTradeValueD046D0 = 0;
    gTradeValueCC4224 = 0;
    gTradeValueD2E2B0 = 0;
    WindowTrade_SyncOwnItems(1);
    WindowTrade_CopyOwnPet(1);
    std::strcpy(gTradeNameD2D654.data(), std::strlen(name) <= 16 ? name : "???");
    gTradeValueD67CB8 = 0;
    gTradeValueD67CBC = 0;
    gTradeValueCC3EC4 = -1;
    gTradeValueD66E08 = 0;
    gTradeValueCC2344 = 0;
    return WindowTrade_Open();
}

// 原函数：0x004E4E60。
void WindowTrade_RequestTargetList(const std::int32_t index, const char* const name)
{
    static_cast<void>(network::Sub_511FF0(platform::gConnectionSocket, index));
    static_cast<void>(WindowTrade_ReceiveStart(name));
}

// 原函数：0x004E4E80。
login::YobiAction* WindowTrade_OpenTargetList(const char* const text)
{
    return WindowTradeTarget_ReceiveTargets(text, &WindowTrade_RequestTargetList);
}

// 原函数：0x004E0100。
void WindowTrade_CopyDisplayItems(const std::int32_t side, const std::int32_t index)
{
    if (index < 0 || index >= 20) {
        return;
    }
    const world::detail::PlayerItemRecord& source = world::detail::BorrowPlayerItemsFDA8A4()[index + 8];
    const bool used = source.used != 0;
    char* const record = gItemDetailStorageCC4D58.data() + side * 0xFBF8 + index * 0xC44;
    std::memset(record, 0, 0xC44);
    if (!used) {
        return;
    }
    const std::int32_t imageNo = source.imageNo;
    const std::int32_t count = source.count;
    WindowTrade_WriteValue(record, 0, std::uint16_t{1});
    WindowTrade_WriteValue(record, 4, imageNo);
    WindowTrade_WriteValue(record, 8, count);
    const std::uint16_t flags = source.flags;
    WindowTrade_WriteValue(record, 0xC40,
        static_cast<std::int32_t>(std::bit_cast<std::int16_t>(flags)));
    if (!source.memo.empty()) {
        cg::text::StoreGbk(record + 0xC29, 17, source.memo);
    }
    cg::text::StoreGbk(record + 0x0C, 29, source.name);
    for (std::int32_t line = 0; line < 32; ++line) {
        cg::text::StoreGbk(record + 0x29 + line * 96, 96, source.description[line]);
    }
    WindowTrade_WriteValue(record, 0xC3C, source.pages);
    if ((flags & 0x60u) != 0) {
        WindowTrade_WriteValue(record, 2, std::uint16_t{0xFFFF});
    }
}

// 原函数：0x004E52B0。
void WindowTrade_CopyOwnPet(const std::int32_t side)
{
    for (std::int32_t index = 0; index < 5; ++index) {
        world::detail::PetRuntimeRecord& source = (*world::detail::BorrowPetRuntimeF633F8Storage())[index];
        if (source.used == 0) {
            continue;
        }
        char* const record = gTradePetsC9C108.data() + (index + side * 5) * 0x5FC;
        const bool hasUserName = source.name6BD[0] != '\0';
        WindowTrade_WriteValue(record, 0, std::uint16_t{1});
        std::strcpy(record + 0x5D8,
            hasUserName ? source.name6BD.data() : source.name6AC.data());
        const std::int32_t imageNo = source.imageNo6A0;
        const std::int32_t value08 = source.value08;
        std::strcpy(record + 0x5E9, source.name6AC.data());
        WindowTrade_WriteValue(record, 4, source.value04);
        WindowTrade_WriteValue(record, 8, imageNo);
        WindowTrade_WriteValue(record, 0x0C, value08);
        for (std::int32_t value = 0; value < 3; ++value) {
            WindowTrade_WriteValue(record, 0x10 + value * 4,
                source.values0C[value].value());
        }
        const std::int32_t fourth =
            source.values0C[3].value();
        std::int32_t first = source.values70[5];
        std::int32_t second = source.values70[6];
        WindowTrade_WriteValue(record, 0x1C, fourth);
        std::int32_t third = source.values70[7];
        WindowTrade_WriteValue(record, 0x38, first);
        first = source.values70[8];
        WindowTrade_WriteValue(record, 0x3C, second);
        second = source.values70[9];
        WindowTrade_WriteValue(record, 0x40, third);
        third = source.values70[0];
        WindowTrade_WriteValue(record, 0x44, first);
        first = source.values70[1];
        WindowTrade_WriteValue(record, 0x48, second);
        second = source.values70[2];
        WindowTrade_WriteValue(record, 0x20, third);
        third = source.values70[3];
        WindowTrade_WriteValue(record, 0x24, first);
        first = source.values70[4];
        WindowTrade_WriteValue(record, 0x28, second);
        second = source.values70[10];
        WindowTrade_WriteValue(record, 0x2C, third);
        third = source.value6A4;
        WindowTrade_WriteValue(record, 0x30, first);
        WindowTrade_WriteValue(record, 0x34, second);
        WindowTrade_WriteValue(record, 0x5C, third);
        for (std::int32_t value = 0; value < 4; ++value) {
            WindowTrade_WriteValue(record, 0x4C + value * 4, source.values70[21 + value]);
        }
        for (std::int32_t skill = 0; skill < 10; ++skill) {
            const std::int16_t sourceIndex =
                std::bit_cast<std::int16_t>(source.techniquePairs650[skill].index);
            std::memcpy(
                record + 0x60 + skill * 0x8C,
                &source.techniquesD8[sourceIndex], sizeof(world::detail::PetTechniqueRecord));
        }
    }
}

// 原函数：0x004E5710。
std::int32_t WindowTrade_ResetRemoteItems(const std::int32_t side)
{
    if (side == 0) {
        gTradeValueCF4688 = 0;
    } else {
        gTradeValueD046D0 = 0;
    }
    std::memset(gTradeItemsD460D0.data() + side * 0xF5F0, 0, 0xF5F0);
    std::memset(gTradePetsC9C108.data() + side * 0x1DEC, 0, 0x1DEC);
    return 0;
}

// 原函数：0x004E5770。
void WindowTrade_BuildRemoteItemDescription(
    const std::int32_t index, const std::int32_t side, const char* const text)
{
    char* const record = gTradeItemsD460D0.data() + (index + side * 20) * 0xC4C;
    char* const rows = record + 0x2D;
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char* const row = rows + line * 96;
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            last = line;
        } else {
            row[0] = '\0';
        }
    }
    // 原回扫从最后成功行的前一行开始，不检查最后成功行本身。
    while (last > 0 && std::strlen(rows + (last - 1) * 96) == 0) {
        --last;
    }
    WindowTrade_WriteValue(record, 0xC30, last < 0 ? std::int32_t{0} : last / 8 + 1);
}

// 原函数：0x004E5830。
std::int32_t WindowTrade_ReceiveRemoteItems(const char* const text)
{
    char name[256];
    char memo[256];
    char description[2304];
    std::int32_t field = 1;
    std::int32_t index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
    std::int32_t status =
        network::ExtractDelimitedField_52CED0(text, '|', field + 1, 255, name);
    while (status != 1) {
        char* const record = gTradeItemsD460D0.data() + index * 0xC4C;
        std::memset(record, 0, 0xC4C);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        WindowTrade_WriteValue(record, 0, std::uint16_t{1});
        if (std::strlen(name) <= 28) {
            std::strcpy(record + 0x10, name);
        }
        network::ExtractDelimitedField_52CED0(text, '|', field + 3, 767, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        WindowTrade_BuildRemoteItemDescription(index, 0, description);
        network::ExtractDelimitedField_52CED0(text, '|', field + 13, 255, memo);
        input::UnescapeLegacyTextAndNotify_52D2F0(memo);
        if (std::strlen(memo) <= 16) {
            std::strcpy(record + 0xC38, memo);
        }
        WindowTrade_WriteValue(record, 4,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 4));
        WindowTrade_WriteValue(record, 0x0C,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 10));
        WindowTrade_WriteValue(record, 8,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 12));
        WindowTrade_WriteValue(record, 0xC34,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 9));
        field += 14;
        index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        status = network::ExtractDelimitedField_52CED0(text, '|', field + 1, 255, name);
    }
    return status;
}

// 原函数：0x004E5A40。
std::int32_t WindowTrade_ReceiveRemotePet(const std::int32_t index, const char* const text)
{
    char* const record = gTradePetsC9C108.data() + index * 0x5FC;
    std::memset(record, 0, 0x5FC);
    char name[256];
    std::int32_t result =
        network::ExtractDelimitedField_52CED0(text, '|', 1, 255, name);
    if (result != 0) {
        return result;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    std::strcpy(record + 0x5D8, std::strlen(name) <= 16 ? name : "???");
    result = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    WindowTrade_WriteValue(record, 0x0C, result);
    if (result < 0) {
        return result;
    }
    WindowTrade_WriteValue(record, 0, std::uint16_t{1});
    WindowTrade_WriteValue(record, 4, static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(text, '|', 3)));
    WindowTrade_WriteValue(record, 0x14, network::ParseDelimitedInteger_52CFA0(text, '|', 4));
    for (std::int32_t field = 5; field <= 11; ++field) {
        WindowTrade_WriteValue(record, 0x1C + (field - 5) * 4,
            network::ParseDelimitedInteger_52CFA0(text, '|', field));
    }
    WindowTrade_WriteValue(record, 0x5C, network::ParseDelimitedInteger_52CFA0(text, '|', 12));
    network::ExtractDelimitedField_52CED0(text, '|', 13, 255, name);
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    std::strcpy(record + 0x5E9, std::strlen(name) <= 16 ? name : "???");
    WindowTrade_WriteValue(record, 8, network::ParseDelimitedInteger_52CFA0(text, '|', 14));
    for (std::int32_t field = 15; field <= 23; ++field) {
        result = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        WindowTrade_WriteValue(record, 0x38 + (field - 15) * 4, result);
    }
    return result;
}

// 原函数：0x004E5CD0。
std::int32_t WindowTrade_ReceiveRemotePetSkills(const std::int32_t index, const char* const text)
{
    if (index < 0 || index >= 5) {
        return index;
    }
    char* const record = gTradePetsC9C108.data() + index * 0x5FC;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return index;
    }
    char* skill = record + 0x60;
    std::memset(skill, 0, 0x8C);
    char name[256];
    for (std::int32_t field = 1; field <= 10; ++field) {
        const std::int32_t status =
            network::ExtractDelimitedField_52CED0(text, '|', field, 255, name);
        if (status == 1) {
            return status;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::size_t length = std::strlen(name);
        WindowTrade_WriteValue(skill, 0, std::uint16_t{1});
        if (length <= 24) {
            std::strcpy(skill + 6, name);
        }
        skill += 0x8C;
    }
    return 10;
}
} // namespace detail


// 原函数：0x0050E3C0。
void WindowTrade_ReceiveTargetReply(const std::uintptr_t, const char* const text)
{
    if (world::gValue9A6E0C != 0) {
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    static_cast<void>(detail::WindowTrade_OpenTargetList(text));
}

// 原函数：0x0050E3F0。
void WindowTrade_ReceiveStartReply(const std::uintptr_t, char* const text, const std::int32_t)
{
    if (world::gValue9A6E0C != 0) {
        return;
    }
    if (world::gValueD67E78 != 0) {
        static_cast<void>(network::Sub_512270(platform::gConnectionSocket, 0));
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    static_cast<void>(detail::WindowTrade_ReceiveStart(text));
}

// 原函数：0x0050E440。
std::int32_t WindowTrade_ReceiveCancelReply(const std::uintptr_t)
{
    const std::int32_t scene = world::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    detail::gTradeValueCC4224 = 1;
    return gate;
}

// 原函数：0x0050E470。
std::int32_t WindowTrade_ReceiveStateReply(const std::uintptr_t, const std::int32_t value)
{
    const std::int32_t scene = world::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    detail::gTradeValueCF4688 = value;
    return value;
}

// 原函数：0x0050E4A0。
std::int32_t WindowTrade_ReceiveItemReply(const std::uintptr_t, const char* const text)
{
    const std::int32_t scene = world::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    return detail::WindowTrade_ReceiveRemoteItems(text);
}

// 原函数：0x0050E4D0。
std::int32_t WindowTrade_ReceivePetReply(
    const std::uintptr_t, const std::int32_t index, const char* const text)
{
    const std::int32_t scene = world::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    return detail::WindowTrade_ReceiveRemotePet(index, text);
}

// 原函数：0x0050E500。
std::int32_t WindowTrade_ReceivePetSkillReply(
    const std::uintptr_t, const std::int32_t index, const char* const text)
{
    const std::int32_t scene = world::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    return detail::WindowTrade_ReceiveRemotePetSkills(index, text);
}

// 原函数：0x0050E530。
std::int32_t WindowTrade_ReceiveRemoteResetReply(const std::uintptr_t)
{
    const std::int32_t scene = world::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    detail::gTradeValueCC4224 = 0;
    static_cast<void>(detail::WindowTrade_ResetRemoteItems(0));
    const std::int32_t previous = detail::gTradeValueD2E2B0;
    if (previous == 2) {
        detail::gTradeValueD2E2B0 = 1;
    }
    return previous;
}

// 原函数：0x0050E570。
void WindowTrade_ReceiveCloseReply(const std::uintptr_t, const std::int32_t value)
{
    if (world::gValue9A6E0C != 0) {
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    if (value == 1) {
        detail::gTradeValueCC4224 = 2;
    } else if (value == 0 || value == 2) {
        WindowInstance* const window = detail::gWindowStates[WindowId_Trade].window;
        detail::gTradeValueD67B80 = 1;
        if (window != nullptr) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
}

} // namespace cg::ui
