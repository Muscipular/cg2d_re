#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004AC290。
static std::int32_t WindowItemStorage_HandleOwnItems(std::int32_t index, std::int32_t state); // 窗口38自有物品面板。

// 原函数：0x004AC890。
static std::int32_t WindowItemStorage_HandleStoredItems(std::int32_t index, std::int32_t state); // SEP01：完整窗口38外部物品消费，低BYTE/坐标原序与S32 consumed保持。

// 原函数：0x004AC0F0。
static void WindowItemStorage_Release();

// 原函数：0x004AC140。
static std::int32_t WindowItemStorage_Update(std::int32_t value);

// 原函数：0x004AC1B0。
// 关联原入口：0x004AC1EC（共享尾已并入本函数正文）。
static std::int32_t WindowItemStorage_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static std::int32_t gStorageOwnHover57EE04 = -2; // 原全局：0x0057EE04。

static std::int32_t gStorageExternalHover57EE08 = -2; // 原全局：0x0057EE08。

static WindowGraphicData gWindowGraphic5716C8 {245221, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005716C8。

static WindowGraphicData gWindowGraphic5716E0 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005716E0。

static WindowGraphicData gWindowGraphic5716F8 {245222, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005716F8。

static WindowGraphicData gWindowGraphic571710 {245220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571710。

static WindowGraphicData gWindowGraphic571728 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571728。

static WindowElementPrototype gWindowElements571740[6] { // 原全局：0x00571740。
    {WindowElementType_Graphic, 547, 9, 11, 11, 1, {.graphic = &gWindowGraphic571728}, &WindowBank_ButtonCloseIcon},
    {WindowElementType_Graphic, 476, 308, 66, 17, 1, {.graphic = &gWindowGraphic5716E0}, &WindowBank_ButtonClose},
    {WindowElementType_Graphic, 286, 27, 273, 280, 1, {.graphic = &gWindowGraphic5716F8}, &WindowItemStorage_HandleOwnItems},
    {WindowElementType_Graphic, 14, 27, 273, 280, 1, {.graphic = &gWindowGraphic571710}, &WindowItemStorage_HandleStoredItems},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5716C8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 571, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口38；原全局：0x0054EB40。
WindowPrototype gWindowPrototypeItemStorage {0, 4, 300, 82, 567, 339, 0x80010101u, 6, gWindowElements571740, &WindowItemStorage_Update, &WindowItemStorage_Draw, &WindowItemStorage_Release}; // 38: 54EB40

// 窗口函数实现。

// 原函数：0x004AC0C0。
WindowInstance* WindowItemStorage_Open(
    const std::int32_t kind, const std::int32_t value14,
    const std::int32_t userState, const std::int32_t)
{
    gWindowStates[WindowId_ItemStorage].value10 = static_cast<std::uint32_t>(kind);
    WindowInstance* const window = WindowRuntime_Open(WindowId_ItemStorage);
    gWindowStates[WindowId_ItemStorage].window = window;
    gWindowStates[WindowId_ItemStorage].value14 = static_cast<std::uint32_t>(value14);
    gWindowStates[WindowId_ItemStorage].userState.value = static_cast<std::uint32_t>(userState);
    return window;
}

// 原函数：0x004AC0F0。
static void WindowItemStorage_Release()
{
    if (world::detail::gValue581800 == 34) {
        world::detail::gValue581800 = -1;
    }
    WindowInstance* const numberWindow = gWindowStates[WindowId_Quantity].window;
    if (numberWindow != nullptr && gWindowStates[WindowId_Quantity].value20 == 41) {
        numberWindow->state |= 0x8000u;
    }
    if (input::IsCurrentContext_503E40(nullptr)) {
        input::SetContext_44B330(&input::gInputContext626AA0);
    }
}

// 原函数：0x004AC140。
static std::int32_t WindowItemStorage_Update(const std::int32_t)
{
    if (WindowProfile_CheckDistance(1, 1) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (world::detail::gValue581800 != 34) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    gWindowStates[WindowId_ItemStorage].actionFlags1C = 0xFFFFFFFFu;
    return 1;
}

// SPD01：原4AC1B0完整19条及4AC1EC三条共享尾；页数/内外物品预览分派后绘制并返回1。
// 原函数：0x004AC1B0。
static std::int32_t WindowItemStorage_Draw(const std::int32_t)
{
    const std::int32_t preview = std::bit_cast<std::int32_t>(gWindowStates[WindowId_ItemStorage].actionFlags1C);
    if (preview >= 0) {
        const std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[WindowId_ItemStorage].value20);
        if (preview < 100) {
            static_cast<void>(WindowItem_DrawEquipmentTooltip(preview, page));
        } else {
            static_cast<void>(WindowItem_DrawDescriptionTooltip(preview - 100, page));
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004AC290。
static std::int32_t WindowItemStorage_HandleOwnItems(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::memcpy(dimmed.bytes.data() + 8, &dimmedColor, sizeof(dimmedColor));
    dimmed.bytes[13] = 4;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 2) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX();
        const std::int32_t dropY = WindowDrag_ReadY();
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source < 100) {
            const WindowInstance* const window = gCurrentWindow;
            const std::int32_t windowX = window->x;
            const std::int32_t windowY = window->y;
            const std::int32_t elementX = window->elements[index].x;
            const std::int32_t elementY = window->elements[index].y;
            for (std::int32_t position = 0; position < 20; ++position) {
                const std::int32_t left = windowX + 50 * (position % 5) + elementX + 12;
                const std::int32_t top = windowY + 52 * (position / 5) + elementY + 39;
                if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                    continue;
                }
                if (source != position + 8) {
                    WindowItem_RequestTransfer(38, index, source, position + 8);
                }
                break;
            }
        }
        WindowDrag_Reset();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 12;
            const std::int32_t top = element.y + 52 * (position / 5) + window->y + 39;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        gHoverTextD68AE8 = L"把身上的物品用滑鼠拖放到飼料箱。";
    }
    if (WindowDrag_ReadKind() != 0 && (events & 0x20u) != 0 && WindowDrag_ReadSourceIndex() >= 100
        && (world::detail::BorrowPlayerItemsFDA8A4()[hovered + 8].flags & 0x60u) != 0) {
        hovered = -1;
    }
    if (gStorageOwnHover57EE04 != hovered) {
        gWindowStates[WindowId_ItemStorage].value20 = 0;
    }
    gStorageOwnHover57EE04 = hovered;
    if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            if (world::detail::BorrowPlayerItemsFDA8A4()[hovered + 8].used != 0) {
                static_cast<void>(WindowItemDrag_Start(hovered + 8, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            if (world::detail::BorrowPlayerItemsFDA8A4()[hovered + 8].used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[WindowId_ItemStorage].value20);
                const std::int32_t pages = world::detail::BorrowPlayerItemsFDA8A4()[hovered + 8].pages;
                ++page;
                gWindowStates[WindowId_ItemStorage].value20 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[WindowId_ItemStorage].value20 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 8) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * ((source - 8) % 5) + window->x + 12;
            const std::int32_t top = element.y + 52 * ((source - 8) / 5) + window->y + 39;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 12;
        const std::int32_t top = element.y + 52 * (hovered / 5) + window->y + 39;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        if (world::detail::BorrowPlayerItemsFDA8A4()[hovered + 8].used != 0) {
            gWindowStates[WindowId_ItemStorage].actionFlags1C = static_cast<std::uint32_t>(hovered + 8);
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gValueD67B08.data());
    std::wstring buffer;
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = world::detail::BorrowPlayerItemsFDA8A4()[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (position % 5) + window->x + 36;
        const std::int32_t y = element.y + 52 * (position / 5) + window->y + 64;
        if ((item.flags & 0x60u) != 0
            || item.itemType == 38 || item.itemType == 52 || item.itemType == 54) {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, 23040, 0, nullptr));
        }
        if (occupancy[position + 8] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
        }
        // 原4AC80F在两分支后均再次绘制普通图，不合并这次入队。
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

// SEP01：原4AC890完整426条；flags只读低BYTE，坐标按原S32取样序，窗口38类型门槛/翻页/数量显示保持。
// 原函数：0x004AC890。
static std::int32_t WindowItemStorage_HandleStoredItems(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0
        && ((WindowDrag_ReadKind() != 2 && WindowDrag_ReadKind() != 5) || WindowDrag_ReadSourceIndex() < 8)) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX();
        const std::int32_t dropY = WindowDrag_ReadY();
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t elementX = element.x;
        const std::int32_t windowX = window->x;
        const std::int32_t windowY = window->y;
        const std::int32_t elementY = element.y;
        for (std::int32_t position = 0; position < 20; ++position) {
            const std::int32_t left = windowX + 50 * (position % 5) + elementX + 12;
            const std::int32_t top = windowY + 52 * (position / 5) + elementY + 39;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (source != position + 100
                && (source >= 100 || (*reinterpret_cast<const unsigned char*>(
                    &world::detail::BorrowPlayerItemsFDA8A4()[source].flags) & 0x60u) == 0)) {
                // 原4AC9E6始终按玩家表取类型；不为外部索引补造另一来源。
                const std::int32_t itemType = world::detail::BorrowPlayerItemsFDA8A4()[source].itemType;
                if (itemType != 38 && itemType != 52) {
                    WindowItem_RequestTransfer(38, index, source, position + 100);
                }
            }
            break;
        }
        WindowDrag_Reset();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t elementX = element.x;
            const std::int32_t windowX = window->x;
            const std::int32_t left = elementX + 50 * (position % 5) + windowX + 12;
            const std::int32_t elementY = element.y;
            const std::int32_t windowY = window->y;
            const std::int32_t top = elementY + 52 * (position / 5) + windowY + 39;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
    }
    if (WindowDrag_ReadKind() != 0 && (events & 0x20u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source < 100 && (*reinterpret_cast<const unsigned char*>(
                    &world::detail::BorrowPlayerItemsFDA8A4()[source].flags) & 0x60u) != 0) {
            hovered = -1;
        }
    }
    if (gStorageExternalHover57EE08 != hovered) {
        gWindowStates[WindowId_ItemStorage].value20 = 0;
    }
    gStorageExternalHover57EE08 = hovered;
    if ((events & 1u) != 0 && hovered >= 0) {
        const auto* const record = &gItemDetailStorageCC4D58[0].items[hovered];
        if ((events & 2u) != 0) {
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                static_cast<void>(WindowItemDrag_Start(hovered + 100, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[WindowId_ItemStorage].value20);
                std::int32_t pages;
                std::memcpy(&pages, &record->pages, sizeof(pages));
                ++page;
                gWindowStates[WindowId_ItemStorage].value20 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[WindowId_ItemStorage].value20 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (WindowDrag_ReadKind() == 2 || WindowDrag_ReadKind() == 5) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 100) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t elementX = element.x;
            const std::int32_t windowX = window->x;
            const std::int32_t left = elementX + 50 * ((source - 100) % 5) + windowX + 12;
            const std::int32_t elementY = element.y;
            const std::int32_t windowY = window->y;
            const std::int32_t top = elementY + 52 * ((source - 100) / 5) + windowY + 39;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t elementX = element.x;
        const std::int32_t windowX = window->x;
        const std::int32_t left = elementX + 50 * (hovered % 5) + windowX + 12;
        const std::int32_t elementY = element.y;
        const std::int32_t windowY = window->y;
        const std::int32_t top = elementY + 52 * (hovered / 5) + windowY + 39;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        std::uint16_t used;
        std::memcpy(&used, &gItemDetailStorageCC4D58[0].items[hovered], sizeof(used));
        if (used != 0) {
            gWindowStates[WindowId_ItemStorage].actionFlags1C = static_cast<std::uint32_t>(hovered + 100);
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::wstring buffer;
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto* const record = &gItemDetailStorageCC4D58[0].items[position];
        std::uint16_t used;
        std::memcpy(&used, record, sizeof(used));
        if (used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t elementX = element.x;
        const std::int32_t windowX = window->x;
        const std::int32_t x = elementX + 50 * (position % 5) + windowX + 36;
        const std::int32_t elementY = element.y;
        const std::int32_t windowY = window->y;
        const std::int32_t y = elementY + 52 * (position / 5) + windowY + 64;
        std::int32_t imageNo;
        std::memcpy(&imageNo, &record->imageNo, sizeof(imageNo));
        static_cast<void>(login::detail::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
        std::int32_t count;
        std::memcpy(&count, &record->count, sizeof(count));
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

} // namespace detail

} // namespace cg::ui
