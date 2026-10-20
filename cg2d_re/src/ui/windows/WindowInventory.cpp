#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowInventoryElementId : ElementId {
    ButtonClose = 1,
    HandleMoney = 2,
    RefreshMoney = 3,
    RefreshExtraValue = 4,
    HandleItems = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    TogglePreview = 9,
    CycleEquipment = 10,
    RotatePreview = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    HandleEquipment = 18,
    TabEquipment = 19,
    TabPet = 20,
    TabPet_21 = 21,
    TabPet_22 = 22,
    HandleSideArea = 23,
    HandleSideArea_24 = 24,
};



void WindowRuntime_SetInventoryPages(
    const std::int32_t bagPageCount, const std::int32_t bankPageCount,
    const std::int32_t bagCurrentPage)
{
    detail::gLoginWindowRuntime.bagPageCount = bagPageCount;
    detail::gLoginWindowRuntime.bankPageCount = bankPageCount;
    detail::gLoginWindowRuntime.bagCurrentPage = bagCurrentPage;
}

namespace detail {

// G07：九页的普通／选中／悬停图，与参考invExt.cpp共用资源编号。
static constexpr std::array<std::array<std::int32_t, 3>, 9> kInventoryPageImages {{
    {243173, 243174, 243175}, {243176, 243177, 243178}, {243179, 243180, 243181},
    {243182, 243183, 243184}, {243185, 243186, 243187}, {243188, 243189, 243190},
    {243191, 243192, 243193}, {243194, 243195, 243196}, {243197, 243198, 243199},
}};

// 同组页按钮立即刷新选中态；存储顺序及原窗口控件索引保持。
static void RefreshInventoryPageSelection(
    WindowInstance* const window, const WindowElementCallback callback,
    const std::int32_t selectedPage)
{
    WindowElementCursor cursor(*window);
    while (auto* current = cursor.next()) {
        WindowElement& element = *current;
        if (element.mouseCallback == callback) {
            auto* const graphic = element.data.graphic;
            const std::int32_t page = (graphic->imageNo - kInventoryPageImages[0][0]) / 3;
            graphic->imageNo = kInventoryPageImages[page][page == selectedPage ? 1 : 0];
        }
    }
}

// 背包、战斗背包、合成与银行右侧共用页号；银行内只切页，其余入口支持拖物跨页。
static std::int32_t WindowInventory_SwitchBagPage(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (elementPtr) {
        WindowElement& element = (*elementPtr);
        auto* const graphic = element.data.graphic;
        const std::int32_t page = (graphic->imageNo - kInventoryPageImages[0][0]) / 3;
        auto& currentPage = gLoginWindowRuntime.bagCurrentPage;
        if ((state & 1) != 0) {
            cg::text::FormatTo(gHoverTextD68AE8,
                window->id == WindowId_Bank ? L"切换到第{}页" : L"切换背包{}", page + 1);
        }
        if (window->id != WindowId_Bank && WindowDrag_ReadSourceIndex() >= 0 && page != currentPage) {
            if ((state & 0x20) != 0) {
                cg::text::FormatTo(gHoverTextD68AE8, L"移动到背包{}", page + 1);
                const std::int32_t left = window->x + element.x;
                const std::int32_t top = window->y + element.y;
                render::DrawGraphicHover(static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(left + element.width),
                    static_cast<std::uint16_t>(top + element.height),
                    0xDF, static_cast<std::uint8_t>(render::gHoverFramePaletteIndex_D69768), 0);
            }
            if ((state & 0x40) != 0) {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                const auto* const source = network::FormatProtocolInteger_517670(WindowDrag_ReadSourceIndex());
                char packet[64];
                std::snprintf(packet, sizeof(packet), "SWITMP %s %d -1",
                    reinterpret_cast<const char*>(source), page);
                // SendProtocolBuffer负责追加唯一LF，数组保留结尾空间。
                static_cast<void>(network::SendProtocolBuffer_517480(platform::gConnectionSocket, packet));
                WindowDrag_Reset();
            }
        }
        if ((state & 2) != 0 && page != currentPage) {
            currentPage = page;
            RefreshInventoryPageSelection(window, &WindowInventory_SwitchBagPage, page);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            char packet[64];
            std::snprintf(packet, sizeof(packet), "SWITM %d", page);
            static_cast<void>(network::SendProtocolBuffer_517480(platform::gConnectionSocket, packet));
        }
        graphic->imageNo = kInventoryPageImages[page][page == currentPage ? 1 : (state & 1) != 0 ? 2 : 0];
    }
    return 0;
}

// 银行左侧页按钮；点击当前页也沿参考重新请求银行数据。
static std::int32_t WindowInventory_SwitchBankPage(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t page = (graphic->imageNo - kInventoryPageImages[0][0]) / 3;
        auto& currentPage = gLoginWindowRuntime.bankCurrentPage;
        if ((state & 1) != 0) {
            cg::text::FormatTo(gHoverTextD68AE8, L"切换到第{}页", page + 1);
        }
        if ((state & 2) != 0) {
            currentPage = page;
            RefreshInventoryPageSelection(window, &WindowInventory_SwitchBankPage, page);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            char packet[64];
            std::snprintf(packet, sizeof(packet), "rh %d", page + 1);
            static_cast<void>(network::SendProtocolBuffer_517480(platform::gConnectionSocket, packet));
        }
        graphic->imageNo = kInventoryPageImages[page][page == currentPage ? 1 : (state & 1) != 0 ? 2 : 0];
    }
    return 0;
}

std::int32_t WindowInventory_GetPageElementCount(const std::int32_t windowId)
{
    switch (windowId) {
    case WindowId_Inventory:
    case WindowId_BattleItems:
    case WindowId_Craft:
        return gLoginWindowRuntime.bagPageCount > 1 ? gLoginWindowRuntime.bagPageCount : 0;
    case WindowId_Bank: {
        const std::int32_t count = gLoginWindowRuntime.bankPageCount + gLoginWindowRuntime.bagPageCount;
        return count > 1 ? count : 0;
    }
    default:
        return 0;
    }
}

bool WindowInventory_CreatePageElements(WindowInstance* const window, const std::int32_t originalCount)
{
    for (std::int32_t index = originalCount; index < window->elementCount; ++index) {
        const std::int32_t offset = index - originalCount;
        const bool bank = window->id == WindowId_Bank;
        const bool bankPage = bank && offset < gLoginWindowRuntime.bankPageCount;
        const std::int32_t page = bank && !bankPage ? offset - gLoginWindowRuntime.bankPageCount : offset;
        auto* const elementPtr = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
        if (elementPtr) {
            WindowElement& element = (*elementPtr);
            element.type = WindowElementType_Graphic;
            element.seqNo = (bank ? 2 : 0) + offset;
            element.visible = 1;
            element.mouseCallback = bankPage ? &WindowInventory_SwitchBankPage : &WindowInventory_SwitchBagPage;
            auto* const graphic = static_cast<WindowGraphicData*>(std::calloc(1, sizeof(WindowGraphicData)));
            element.data.graphic = graphic;
            if (graphic == nullptr) {
                return false; // 沿WindowRuntime_Open原分配失败回收路径。
            }
            graphic->imageNo = kInventoryPageImages[page][0];
            graphic->color = 0xFFFFFFFFu;
            if (bank) {
                element.x = 26 + 29 * page + (bankPage ? 0 : 29 * 9);
                element.y = 307;
                element.width = 24;
                element.height = 19;
            } else if (window->id == WindowId_Craft) {
                element.x = 195 + 32 * (page % 3);
                element.y = 27 + 22 * (page / 3);
                element.width = 30;
                element.height = 26;
                graphic->offsetX = -5;
                graphic->offsetY = -3;
                graphic->width = 30;
                graphic->height = 24;
            } else {
                element.x = 24 + 50 * page;
                element.y = 248;
                element.width = 47;
                element.height = 30;
                graphic->offsetX = -12;
                graphic->offsetY = -6;
                graphic->width = 50;
                graphic->height = 30;
            }
        }
    }
    return true;
}

// 回调与内部函数前置声明。

// 原函数：0x00488800。
static void WindowItem_TransferQuantity(
    std::int32_t sourceMode, std::int32_t source,
    std::int32_t targetMode, std::int32_t target); // IMT01：两低U8模式/原-1及同mode两fresh组读取保持，发送原不消费；全互通待。

// 原函数：0x0048B930。
static std::int32_t WindowItem_DrawDescriptionLines(
    std::int32_t x, std::int32_t y, const wchar_t* title, std::int8_t titleColor,
    const std::wstring_view* lines, std::int32_t unusedStride, std::int32_t lineCount,
    const wchar_t* footer, std::int8_t footerColor); // 原连续NUL分段改为宽行表，空行仍推进，不使用stride。

// 原函数：0x00488930。
static std::int32_t WindowInventory_TogglePreview(std::int32_t unusedIndex, std::int32_t state);

// 原函数：0x00488950。
static std::int32_t WindowInventory_CycleEquipment(std::int32_t unusedIndex, std::int32_t state);

// 原函数：0x00488970。
static std::int32_t WindowInventory_RotatePreview(std::int32_t unusedIndex, std::int32_t state);

// 原函数：0x00488EA0。
static std::int32_t WindowInventory_TabEquipment(std::int32_t unusedIndex, std::int32_t state);

// 原函数：0x00488EC0。
static std::int32_t WindowInventory_TabPet(std::int32_t index, std::int32_t state); // SAP01：库存宠物页签，原低AL归一为S32，五对/无效图处理保持。

// 原函数：0x00488C50。
static std::int32_t WindowInventory_ButtonClose(std::int32_t index, std::int32_t state); // 库存关闭按钮。

// 原函数：0x00488CF0。
static void WindowInventory_DropMoney(std::int32_t amount); // MDR01：原动作BYTE/场景门槛短路，Y-X发送金额，void副作用/完整ABI待。

// 原函数：0x00488D30。
static std::int32_t WindowInventory_HandleMoney(std::int32_t index, std::int32_t state); // MDR01：三次fresh窗口匹配/实时金额同步，held graphic及0/1返回保。

// 原函数：0x00488E60。
static std::int32_t WindowInventory_RefreshMoney(std::int32_t unusedIndex, std::int32_t unusedState);

// 原函数：0x00488E80。
static std::int32_t WindowInventory_RefreshExtraValue(std::int32_t unusedIndex, std::int32_t unusedState);

// 原函数：0x00489EE0。
static std::int32_t WindowInventory_TabPlayerAttributes(std::int32_t unusedIndex, std::int32_t state);

// 原函数：0x00488510。
static void WindowInventory_Release();

// 原函数：0x00488990。
static std::int32_t WindowInventory_Update(std::int32_t value);

// 原函数：0x00488B10。
static std::int32_t WindowInventory_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。



// 原全局：0x00CB086C。 488E60格式化，type7载荷借用。
static std::wstring gInventoryMoneyTextCB086C {}; // 原全局：0x00CB086C。

// 原全局：0x00CC3ECC。 488E80格式化FF02CC，独立于输入区。
static std::wstring gInventoryValueTextCC3ECC {}; // 原全局：0x00CC3ECC。

static WindowGraphicData gWindowGraphic562B98 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562B98。

static WindowGraphicData gWindowGraphic562BB0 {244290, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562BB0。

static WindowGraphicData gWindowGraphic562BC8 {244291, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562BC8。

static WindowGraphicData gWindowGraphic562BE0 {244294, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562BE0。

static WindowGraphicData gWindowGraphic562BF8 {98131, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562BF8。

static WindowGraphicData gWindowGraphic562C10 {98140, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562C10。

static WindowGraphicData gWindowGraphic562C28 {98143, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562C28。

static WindowGraphicData gWindowGraphic562C40 {98146, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562C40。

static WindowGraphicData gWindowGraphic562C58 {244297, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562C58。

static std::int32_t gWindowAnimationImage562C80 = 100000; // 原全局：0x00562C80。

static WindowNumberData gWindowNumber562CB0 {&gInventoryMoneyTextCB086C, 10, 3, 1, 0}; // 原全局：0x00562CB0。

static WindowNumberData gWindowNumber562CB8 {&gInventoryValueTextCC3ECC, 10, 3, 1, 0}; // 原全局：0x00562CB8。

static WindowElementPrototype gWindowElements562CD8[24] { // 原全局：0x00562CD8。
    {WindowElementType_Graphic, 258, 8, 11, 11, 1, {.graphic = &gWindowGraphic562B98}, &WindowInventory_ButtonClose, -1, static_cast<ElementId>(WindowInventoryElementId::ButtonClose)},
    {WindowElementType_Graphic, 221, 28, 49, 17, 1, {.graphic = &gWindowGraphic562BC8}, &WindowInventory_HandleMoney, -1, static_cast<ElementId>(WindowInventoryElementId::HandleMoney)},
    {WindowElementType_Number, 198, 31, 0, 0, 1, {.number = &gWindowNumber562CB0}, &WindowInventory_RefreshMoney, -1, static_cast<ElementId>(WindowInventoryElementId::RefreshMoney)},
    {WindowElementType_Number, 99, 31, 0, 0, 1, {.number = &gWindowNumber562CB8}, &WindowInventory_RefreshExtraValue, -1, static_cast<ElementId>(WindowInventoryElementId::RefreshExtraValue)},
    {WindowElementType_Graphic, 8, 0, 278, 259, 1, {.graphic = &gWindowGraphic562BB0}, &WindowInventory_HandleItems, -1, static_cast<ElementId>(WindowInventoryElementId::HandleItems)},
    {WindowElementType_Animation, -81, 163, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage562C80}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowInventoryElementId::Element6)},
    {WindowElementType_Text, -81, 28, 0, 0, 0, {.text = &gWindowText562C70}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowInventoryElementId::Element7)},
    {WindowElementType_Text, -81, 48, 0, 0, 0, {.text = &gWindowText562C78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowInventoryElementId::Element8)},
    {WindowElementType_EventArea, -114, 188, 33, 17, 0, {.unusedValue = 0}, &WindowInventory_TogglePreview, -1, static_cast<ElementId>(WindowInventoryElementId::TogglePreview)},
    {WindowElementType_EventArea, -81, 188, 33, 17, 0, {.unusedValue = 0}, &WindowInventory_CycleEquipment, -1, static_cast<ElementId>(WindowInventoryElementId::CycleEquipment)},
    {WindowElementType_EventArea, -121, 73, 78, 113, 0, {.unusedValue = 0}, &WindowInventory_RotatePreview, -1, static_cast<ElementId>(WindowInventoryElementId::RotatePreview)},
    {WindowElementType_Graphic, -170, 73, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowInventoryElementId::Element12)},
    {WindowElementType_Graphic, -41, 73, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowInventoryElementId::Element13)},
    {WindowElementType_Graphic, -170, 123, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowInventoryElementId::Element14)},
    {WindowElementType_Graphic, -41, 123, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowInventoryElementId::Element15)},
    {WindowElementType_Graphic, -170, 23, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowInventoryElementId::Element16)},
    {WindowElementType_Graphic, -41, 23, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowInventoryElementId::Element17)},
    {WindowElementType_Graphic, -16, 17, 293, 222, 0, {.graphic = &gWindowGraphic562C40}, &WindowInventory_HandleEquipment, -1, static_cast<ElementId>(WindowInventoryElementId::HandleEquipment)},
    {WindowElementType_Graphic, 2, 17, 19, 46, 1, {.graphic = &gWindowGraphic562BE0}, &WindowInventory_TabEquipment, -1, static_cast<ElementId>(WindowInventoryElementId::TabEquipment)},
    {WindowElementType_Graphic, 2, 62, 19, 46, 1, {.graphic = &gWindowGraphic562BF8}, &WindowInventory_TabPet, -1, static_cast<ElementId>(WindowInventoryElementId::TabPet)},
    {WindowElementType_Graphic, 2, 107, 19, 46, 1, {.graphic = &gWindowGraphic562C10}, &WindowInventory_TabPet, -1, static_cast<ElementId>(WindowInventoryElementId::TabPet_21)},
    {WindowElementType_Graphic, 2, 152, 19, 46, 1, {.graphic = &gWindowGraphic562C28}, &WindowInventory_TabPet, -1, static_cast<ElementId>(WindowInventoryElementId::TabPet_22)},
    {WindowElementType_EventArea, -197, 17, 293, 222, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowInventoryElementId::HandleSideArea)},
    {WindowElementType_EventArea, 282, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowInventoryElementId::HandleSideArea_24)},
};

// 原型：窗口14；原全局：0x0054E4E0。
WindowPrototype gWindowPrototypeInventory {0, 4, 340, 120, 278, 259, 0x80010101u, 24, gWindowElements562CD8, &WindowInventory_Update, &WindowInventory_Draw, &WindowInventory_Release}; // 14: 54E4E0

// 窗口函数实现。

// IMT01：原70条，target=-1先声/原门槛与低U8模式、同mode两fresh组读取；正确正文保，全互通待。
// 原函数：0x00488800。
static void WindowItem_TransferQuantity(
    const std::int32_t sourceMode, const std::int32_t source,
    const std::int32_t targetMode, const std::int32_t target)
{
    if (target == -1) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (source < 0 || target < -1) {
        return;
    }
    const std::uint8_t from = static_cast<std::uint8_t>(sourceMode);
    const std::uint8_t to = static_cast<std::uint8_t>(targetMode);
    if (from != 0) {
        if (from == to) {
            const std::int32_t targetGroup = WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10) + 1;
            const std::int32_t sourceGroup = WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10) + 1;
            static_cast<void>(network::MoveItemBetweenOwnersRequest_516C90(
                platform::gConnectionSocket, sourceGroup, source, targetGroup, target, -1));
        } else {
            const std::int32_t sourceGroup = WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10) + 1;
            static_cast<void>(network::MoveItemBetweenOwnersRequest_516C90(
                platform::gConnectionSocket, sourceGroup, source, 0, target, -1));
        }
    } else if (to != 0) {
        const std::int32_t targetGroup = WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10) + 1;
        static_cast<void>(network::MoveItemBetweenOwnersRequest_516C90(
            platform::gConnectionSocket, 0, source, targetGroup, target, -1));
    }
}

// 原函数：0x0048B930。
static std::int32_t WindowItem_DrawDescriptionLines(
    const std::int32_t x, const std::int32_t y,
    const wchar_t* const title, const std::int8_t titleColor,
    const std::wstring_view* lines, const std::int32_t, const std::int32_t lineCount,
    const wchar_t* const footer, const std::int8_t footerColor)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, L"", 0, 0));
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 10)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 10)),
            5, 12, titleColor, title, 0, 0));
    }
    gTooltipColorD67D30 = 0;
    std::int32_t lineY = 0;
    for (std::int32_t line = 0; line < lineCount; ++line) {
        if (!lines->empty()) {
            const std::int32_t shortY =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
            const std::int32_t shortX =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
            static_cast<void>(WindowItem_SplitTooltipLines(
                shortX + 10, shortY + lineY + 22, 5, 12, 0, *lines, 0, 0));
        }
        lineY += 13;
        ++lines;
    }
    if (footer != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 10)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 127)),
            5, 12, footerColor, footer, 0, 0));
    }
    return render::DrawGraphic3_520970(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)),
        230, 244317, 0, 1, 0, &descriptor);
}

// ISP01：借同一列表/原动态count，state低BYTE消费；循环尾先重取count再推进槽。
// 原函数：0x0048A6C0。
std::int32_t WindowInventory_DrawBlockedEquipment(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    descriptor.color = color;
    dimmed.color = dimmedColor;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    dimmed.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    world::PlayerItemRecord* items;
    std::int32_t count;
    if (!WindowItemSelector_GetItemList_4C72C0(&gItemSelectorCF3B10, &items, &count)) {
        return 0;
    }
    if ((*reinterpret_cast<const unsigned char*>(&state) & 2u) != 0) {
        currentWindow->state |= 0x2000u;
    }
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    std::int32_t currentCount = count;
    std::int32_t slot = 0;
    while (slot < currentCount) {
        const world::PlayerItemRecord* const item = items + slot;
        if (item->used != 0) {
            const std::int32_t x = WindowItemSelector_ReadScreenX(&gItemSelectorCF3B10, index, slot, 0);
            const std::int32_t y = WindowItemSelector_ReadScreenY(&gItemSelectorCF3B10, index, slot, 0);
            static_cast<void>(cg::render::DrawGraphic(x, y, 223, 23040, 0, nullptr));
            if (occupancy[slot] != 0) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                    static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                    223, 249, 0);
                static_cast<void>(cg::render::DrawGraphic(x, y, 223, item->imageNo, 0, &dimmed));
            } else {
                static_cast<void>(cg::render::DrawGraphic(x, y, 223, item->imageNo, 0, &descriptor));
            }
        }
        currentCount = count;
        ++slot;
    }
    return 0;
}

// ISP01：对话29选物先占用/发布两个回调与mode2，启动返回丢弃，再透传焦点绑定S32结果。
// 原函数：0x004888C0。
std::int32_t WindowInventory_OpenSpecialSelection(const char* const text)
{
    const std::int32_t selected = std::atoi(text);
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    const unsigned char previous = occupancy[selected];
    gSelectedItemCB0A44 = selected;
    occupancy[selected] = static_cast<unsigned char>(previous + 1);
    if (auto* const handleItemsElement = gWindowStates[WindowId_Inventory].window ? gWindowStates[WindowId_Inventory].window->FindElementById(WindowInventoryElementId::HandleItems) : nullptr; handleItemsElement) handleItemsElement->mouseCallback = WindowInventory_HandleItemSelection;
    if (auto* const handleEquipmentElement = gWindowStates[WindowId_Inventory].window ? gWindowStates[WindowId_Inventory].window->FindElementById(WindowInventoryElementId::HandleEquipment) : nullptr; handleEquipmentElement) handleEquipmentElement->mouseCallback = WindowInventory_DrawBlockedEquipment;
    gItemSelectionModeCC40D4 = 2;
    static_cast<void>(WindowDrag_Start(1, 0, nullptr));
    return WindowDrag_SetFocus(14, 0);
}

// 原函数：0x00488930。
static std::int32_t WindowInventory_TogglePreview(const std::int32_t, const std::int32_t state)
{
    return WindowItemSelector_TogglePreview(&gItemSelectorCF3B10, state) ? 1 : 0;
}

// 原函数：0x00488950。
static std::int32_t WindowInventory_CycleEquipment(const std::int32_t, const std::int32_t state)
{
    return WindowItemSelector_CycleEquipment(&gItemSelectorCF3B10, state) ? 1 : 0;
}

// 原函数：0x00488970。
static std::int32_t WindowInventory_RotatePreview(const std::int32_t, const std::int32_t state)
{
    static_cast<void>(WindowItemSelector_RotatePreview(&gItemSelectorCF3B10, state));
    return 0;
}

// 原函数：0x00488EA0。
static std::int32_t WindowInventory_TabEquipment(const std::int32_t, const std::int32_t state)
{
    return WindowItemSelector_HandleTab(&gItemSelectorCF3B10, 0, state) ? 1 : 0;
}

// SAP01：原81条；五对used/索引/flags与S8模式保持，成功低AL归一、无效图/声效按原，运行待。
// 原函数：0x00488EC0。
static std::int32_t WindowInventory_TabPet(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    const std::int32_t ordinal = index - 19;
    std::int32_t remaining = ordinal;
    bool found = false;
    for (std::int32_t position = 0; position < 5; ++position) {
        const auto& pair = world::gPetPairsDE0B08[position];
        if (pair.used == 0) {
            continue;
        }
        const std::int32_t pet = std::bit_cast<std::int16_t>(pair.index);
        const std::uint16_t flags = world::gPetRuntimeRecords[pet].flags6AA;
        if (flags == 1 || flags == 2) {
            --remaining;
            if (remaining < 0) {
                found = true;
                break;
            }
        }
    }
    if (found) {
        if (ordinal >= 0) {
            return WindowItemSelector_HandleTab(&gItemSelectorCF3B10, ordinal + 1, state) ? 1 : 0;
        }
    } else if (std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28) == ordinal + 1) {
        --gInventoryActorSelectionMode_57DA28;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        consumed = 1;
    }
    if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index)); indexedElement) indexedElement->data.graphic->imageNo = 244306;
    return consumed;
}

// 原函数：0x00488510。
static void WindowInventory_Release()
{
    WindowInstance* const quantityWindow = gWindowStates[WindowId_Quantity].window;
    if (quantityWindow != nullptr && gWindowStates[WindowId_Quantity].value20 == 14) {
        quantityWindow->state |= 0x8000u;
    }
    const std::uint8_t mode = gItemSelectionModeCC40D4;
    if (mode == 1 || mode == 2) {
        WindowInstance* const window = gWindowStates[WindowId_Inventory].window;
        gItemSelectionModeCC40D4 = 0;
        if (auto* const handleItemsElement = window ? window->FindElementById(WindowInventoryElementId::HandleItems) : nullptr; handleItemsElement) handleItemsElement->mouseCallback = WindowInventory_HandleItems;
        if (auto* const handleEquipmentElement = gWindowStates[WindowId_Inventory].window ? gWindowStates[WindowId_Inventory].window->FindElementById(WindowInventoryElementId::HandleEquipment) : nullptr; handleEquipmentElement) handleEquipmentElement->mouseCallback = WindowInventory_HandleEquipment;
        const std::int32_t selected = gSelectedItemCB0A44;
        gSelectedItemCB0A44 = -1;
        --reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data())[selected];
        static_cast<void>(WindowDrag_ResetMouse());
    }
    if (world::gCurrentDialogType_581800 == 29) {
        world::gCurrentDialogType_581800 = -1;
    }
    gItemSelectorCF3B10.initialized3E8 = 0;
}

// 原函数：0x00488990。
static std::int32_t WindowInventory_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    std::int32_t selected;
    if (value == -1) {
        WindowItemSelector_BindElements(&gItemSelectorCF3B10, 0);
        WindowInstance* const window = currentWindow;
        gItemSelectorCF3B10.initialized3E8 = 0;
        gWindowStates[WindowId_Inventory].value14 = 0;
        gItemSelectionModeCC40D4 = 0;
        if (auto* const handleItemsElement = window->FindElementById(WindowInventoryElementId::HandleItems); handleItemsElement) handleItemsElement->mouseCallback = WindowInventory_HandleItems;
        selected = -1;
        if (auto* const handleEquipmentElement = currentWindow->FindElementById(WindowInventoryElementId::HandleEquipment); handleEquipmentElement) handleEquipmentElement->mouseCallback = WindowInventory_HandleEquipment;
        gSelectedItemCB0A44 = -1;
    } else {
        selected = gSelectedItemCB0A44;
    }
    if (gItemSelectionModeCC40D4 == 1) {
        if (world::gPlayerRuntimeSlots.items[selected].imageNo == 26569) {
            goto updateSelector;
        }
        WindowInstance* const window = currentWindow;
        gItemSelectionModeCC40D4 = 0;
        if (auto* const handleItemsElement = window->FindElementById(WindowInventoryElementId::HandleItems); handleItemsElement) handleItemsElement->mouseCallback = WindowInventory_HandleItems;
        if (auto* const handleEquipmentElement = currentWindow->FindElementById(WindowInventoryElementId::HandleEquipment); handleEquipmentElement) handleEquipmentElement->mouseCallback = WindowInventory_HandleEquipment;
        const std::int32_t oldSelection = gSelectedItemCB0A44;
        gSelectedItemCB0A44 = -1;
        --reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data())[oldSelection];
        if (world::gCurrentDialogType_581800 == 29) {
            world::gCurrentDialogType_581800 = -1;
        }
        static_cast<void>(WindowDrag_ResetMouse());
    }
    if (gItemSelectionModeCC40D4 == 2 && world::gCurrentDialogType_581800 != 29) {
        if (auto* const handleItemsElement = currentWindow->FindElementById(WindowInventoryElementId::HandleItems); handleItemsElement) handleItemsElement->mouseCallback = WindowInventory_HandleItems;
        if (auto* const handleEquipmentElement = currentWindow->FindElementById(WindowInventoryElementId::HandleEquipment); handleEquipmentElement) handleEquipmentElement->mouseCallback = WindowInventory_HandleEquipment;
        const std::int32_t oldSelection = gSelectedItemCB0A44;
        gSelectedItemCB0A44 = -1;
        gItemSelectionModeCC40D4 = 0;
        --reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data())[oldSelection];
        static_cast<void>(WindowDrag_ResetMouse());
    }
updateSelector:
    WindowInstance* const quantityWindow = gWindowStates[WindowId_Quantity].window;
    gWindowStates[WindowId_Inventory].value10 = 0xFFFFFFFFu;
    if (quantityWindow != nullptr
        && gWindowStates[WindowId_Quantity].value20 == 14 && gWindowStates[WindowId_Quantity].value24 == 1) {
        const std::int32_t amount = world::gPlayerRuntimeSlots.valueFC;
        gWindowStates[WindowId_Quantity].value10 = static_cast<std::uint32_t>(amount);
        gWindowStates[WindowId_Quantity].value14 = static_cast<std::uint32_t>(amount);
    }
    WindowItemSelector_Update(&gItemSelectorCF3B10);
    return 1;
}

// 原函数：0x00488B10。
static std::int32_t WindowInventory_Draw(const std::int32_t)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    static constexpr std::wstring_view selectionText586B14[] { // 原常量参考：0x00586B14。
        L"选好想盖印的物品，", L"单点滑鼠左键，", L"可盖下印记。", L"单点滑鼠右键即结束。",
    };
    static constexpr std::wstring_view selectionText586B5C[] { // 原常量参考：0x00586B5C。
        L"选好想掩埋的物品，", L"单点滑鼠左键可掩埋物品。", L"单点滑鼠右键即结束。", L"",
    };
    const std::uint8_t mode = gItemSelectionModeCC40D4;
    if (mode == 0) {
        const std::int32_t selected = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Inventory].value10);
        if (selected >= 0) {
            const std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Inventory].value14);
            static_cast<void>(WindowItem_DrawEquipmentTooltip(selected, page));
        }
    } else if (mode == 1 || mode == 2) {
        const std::wstring_view* const text = mode == 1 ? selectionText586B14 : selectionText586B5C;
        const bool top =
            (platform::gLegacyMouseState.mouseY < 240);
        const std::int32_t y = (platform::gLegacyMouseState.mouseY + static_cast<std::int32_t>(top ? 60 : -174));
        const bool left =
            (platform::gLegacyMouseState.mouseX < 320);
        const std::int32_t x = (platform::gLegacyMouseState.mouseX + static_cast<std::int32_t>(left ? 60 : -257));
        static_cast<void>(WindowItem_DrawDescriptionLines(
            x, y, L"", 0, text, 31, 4, nullptr, 0));
    }
    const std::int32_t mappedImage =
        character_select::ResolveCharacterImageForVersion(world::gPlayerRuntimeSlots.imageNo);
    auto* const element6 = currentWindow->FindElementById(WindowInventoryElementId::Element6);
    if (element6) {
        cg::yobi::SetYobiActionField2A8(
            element6->data.animation->action, mappedImage);
    }
    auto* const element62 = currentWindow->FindElementById(WindowInventoryElementId::Element6);
    if (element62) {
        cg::yobi::SetYobiActionField2B8(
            element62->data.animation->action, 5);
    }
    const std::int32_t imageNo = world::gPlayerRuntimeSlots.imageNo;
    if (imageNo >= 110350 && imageNo <= 110373) {
        auto* const element63 = currentWindow->FindElementById(WindowInventoryElementId::Element6);
        if (element63) {
            cg::yobi::AddYobiActionDrawFlags(
                element63->data.animation->action, 4);
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x00488C50。
static std::int32_t WindowInventory_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        graphic->imageNo = 243000;
        if ((state & 1) != 0) {
            graphic->imageNo = 243002;
            gHoverTextD68AE8 = L"关闭这个视窗。";
        }
        if ((state & 2) == 0) {
            return 0;
        }
        gItemSelectorCF3B10.initialized3E8 = 0;
        graphic->imageNo = 243001;
    }
    currentWindow->state |= 0x8000u;
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    return 1;
}

// MDR01：原488CF0完整18条；动作BYTE bit7/场景门槛原短路，玩家Y后X→woz金额；void只承副作用，全callback ABI待。
// 原函数：0x00488CF0。
static void WindowInventory_DropMoney(const std::int32_t amount)
{
    if ((world::ReadPlayerActionFlagsByte() & 0x80u) == 0
        && world::IsObservationSceneTransitionActive_440B50() == 0) {
        const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
        static_cast<void>(network::DropGoldRequest_510DA0(platform::gConnectionSocket, x, y, amount));
    }
}

// MDR01：原488D30完整91条；held graphic/三fresh窗口匹配，开框后重读金额、图号优先序及S32 0/1保持。
// 原函数：0x00488D30。
static std::int32_t WindowInventory_HandleMoney(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"想放钱在地面时使用此按键。";
        }
        if ((state & 2) != 0) {
            if (gWindowStates[WindowId_Quantity].window != nullptr && gWindowStates[WindowId_Quantity].value20 == 14
                && gWindowStates[WindowId_Quantity].value24 == static_cast<std::uint32_t>(index)) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                const std::int32_t amount = world::gPlayerRuntimeSlots.valueFC;
                static_cast<void>(cg::yobi::CreateQuantityWindowOpeningAnimation_48AC60(
                    14, index, 244310, amount, amount, 0, WindowInventory_DropMoney, 1, 0, 0));
            }
            consumed = 1;
        }
        if (gWindowStates[WindowId_Quantity].window != nullptr && gWindowStates[WindowId_Quantity].value20 == 14
            && gWindowStates[WindowId_Quantity].value24 == static_cast<std::uint32_t>(index)) {
            const std::int32_t amount = world::gPlayerRuntimeSlots.valueFC;
            gWindowStates[WindowId_Quantity].value10 = static_cast<std::uint32_t>(amount);
            gWindowStates[WindowId_Quantity].value14 = static_cast<std::uint32_t>(amount);
        }
        graphic->imageNo = 244291;
        if ((state & 1) != 0) {
            graphic->imageNo = 244293;
        }
        if (gWindowStates[WindowId_Quantity].window != nullptr && gWindowStates[WindowId_Quantity].value20 == 14
            && gWindowStates[WindowId_Quantity].value24 == static_cast<std::uint32_t>(index)) {
            graphic->imageNo = 244292;
        }
        return consumed;
    }
    return {};
}

// 原函数：0x00488E60。
static std::int32_t WindowInventory_RefreshMoney(const std::int32_t, const std::int32_t)
{
    cg::text::FormatTo(gInventoryMoneyTextCB086C, L"{:10}", world::gPlayerRuntimeSlots.valueFC);
    return 0;
}

// 原函数：0x00488E80。
static std::int32_t WindowInventory_RefreshExtraValue(const std::int32_t, const std::int32_t)
{
    cg::text::FormatTo(gInventoryValueTextCC3ECC, L"{:10}", world::gPlayerRuntimeSlots.value15BCC);
    return 0;
}

// 原函数：0x00488FB0。
std::int32_t WindowInventory_HandleItems(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    static std::int32_t previousHoveredSlot = -2; // 原全局：0x0057DA2C。
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    descriptor.color = color;
    dimmed.color = dimmedColor;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    dimmed.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if (state != 0) {
        gItemSelectorCF3B10.value3FC = 0;
    }
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 2) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX();
        const std::int32_t dropY = WindowDrag_ReadY();
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        const WindowInstance* const window = currentWindow;
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            auto* const elementPtr = window->FindElementById(NativeElementKey(index));
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t left = element.x + 50 * (position % 5) + window->x + 15;
                const std::int32_t top = element.y + 50 * (position / 5) + window->y + 48;
                if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                    continue;
                }
            }
            if (std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28) > 0 && source < 6) {
                WindowItem_TransferQuantity(1, source, 0, position + 8);
            } else if (source != position + 8) {
                WindowItem_RequestTransfer(14, index, source, position + 8);
            }
            break;
        }
        WindowDrag_Reset();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(NativeElementKey(index));
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t left = element.x + 50 * (position % 5) + window->x + 15;
                const std::int32_t top = element.y + 50 * (position / 5) + window->y + 48;
                if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    hovered = position;
                    break;
                }
            }
        }
    }
    if (WindowDrag_ReadKind() != 0 && (events & 0x20u) != 0 && WindowDrag_ReadSourceIndex() >= 100
        && (world::gPlayerRuntimeSlots.items[hovered + 8].flags & 0x60u) != 0) {
        hovered = -1;
    }
    if ((events & 2u) != 0) {
        currentWindow->state |= 0x2000u;
    }
    if (previousHoveredSlot != hovered) {
        gWindowStates[WindowId_Inventory].value14 = 0;
    }
    previousHoveredSlot = hovered;
    if (gSelectedItemCB1DF4 < 0 && hovered >= 0
        && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (WindowDrag_ReadFocusWindowId() == 14 || WindowDrag_ReadFocusWindowId() == 0) {
            const auto& item = world::gPlayerRuntimeSlots.items[hovered + 8];
            if (item.used != 0 && (item.canUseInField & 1u) != 0) {
                if (item.imageNo == 26569) {
                    WindowInstance* const window = currentWindow;
                    gItemSelectionModeCC40D4 = 1;
                    if (auto* const handleItemsElement = window->FindElementById(WindowInventoryElementId::HandleItems); handleItemsElement) handleItemsElement->mouseCallback = WindowInventory_HandleItemSelection;
                    if (auto* const handleEquipmentElement = currentWindow->FindElementById(WindowInventoryElementId::HandleEquipment); handleEquipmentElement) handleEquipmentElement->mouseCallback = WindowInventory_DrawBlockedEquipment;
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                    gSelectedItemCB0A44 = hovered + 8;
                    ++occupancy[gSelectedItemCB0A44];
                    static_cast<void>(WindowDrag_ResetMouse());
                    static_cast<void>(WindowDrag_Start(1, 0, nullptr));
                } else {
                    if ((world::ReadPlayerActionFlags() & 0x80u) == 0
                        && world::IsObservationSceneTransitionActive_440B50() == 0) {
                        const std::int8_t mode = std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28);
                        gSelectedItemCB0A44 = hovered + 8;
                        const std::int32_t group = mode > 0
                            ? WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10) + 1 : 0;
                        const std::int32_t y =
                            static_cast<std::int32_t>(world::gPlayerY);
                        const std::int32_t x =
                            static_cast<std::int32_t>(world::gPlayerX);
                        static_cast<void>(network::BeginItemUseRequest_510A00(
                            platform::gConnectionSocket, x, y, hovered + 8, group));
                        static_cast<void>(audio::PlaySe_52C450(77, 320));
                    }
                    static_cast<void>(WindowDrag_ResetMouse());
                }
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                static_cast<void>(WindowDrag_ResetMouse());
            }
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            if (world::gPlayerRuntimeSlots.items[hovered + 8].used != 0) {
                static_cast<void>(WindowItemDrag_Start(hovered + 8, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            if (world::gPlayerRuntimeSlots.items[hovered + 8].used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Inventory].value14);
                const std::int32_t pages = world::gPlayerRuntimeSlots.items[hovered + 8].pages;
                ++page;
                gWindowStates[WindowId_Inventory].value14 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[WindowId_Inventory].value14 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 8) {
            const WindowInstance* const window = currentWindow;
            auto* const elementPtr = window->FindElementById(NativeElementKey(index));
            if (elementPtr) {
                const WindowElement& element = (*elementPtr);
                const std::int32_t left = element.x + 50 * ((source - 8) % 5) + window->x + 15;
                const std::int32_t top = element.y + 50 * ((source - 8) / 5) + window->y + 48;
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                    static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                    223, 254, 0);
            }
        }
    }
    std::wstring buffer;
    if (hovered >= 0) {
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(NativeElementKey(index));
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 15;
            const std::int32_t top = element.y + 50 * (hovered / 5) + window->y + 48;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        }
        const auto& item = world::gPlayerRuntimeSlots.items[hovered + 8];
        if (item.used != 0) {
            gWindowStates[WindowId_Inventory].value10 = static_cast<std::uint32_t>(hovered + 8);
            if (!item.memo.empty() && (item.flags & 0x100u) != 0) {
                gHoverTextD68AE8 = item.name + L"[kok]";
            } else if ((item.flags & 0x200u) != 0) {
                gHoverTextD68AE8 = item.memo + L"[han]";
            }
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(0, 0, 3, 1, 0, L"", 0, 0));
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = world::gPlayerRuntimeSlots.items[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(NativeElementKey(index));
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = element.x + 50 * (position % 5) + window->x + 39;
            const std::int32_t y = element.y + 50 * (position / 5) + window->y + 73;
            if (occupancy[position + 8] != 0) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                    static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                    223, 249, 0);
                static_cast<void>(cg::render::DrawGraphic(x, y, 223, item.imageNo, 0, &dimmed));
            } else {
                static_cast<void>(cg::render::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
            }
            const std::int32_t count = item.count;
            if (count > 0) {
                cg::text::FormatTo(buffer, L"{:3}", count);
                static_cast<void>(render::DrawText_456780(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                    3, 1, 1, buffer, 0, 0));
            }
        }
    }
    return consumed;
}

// IER01：原584条；装备拖放/使用/显示保持，两物品BYTE及动作BYTE明确，selected先于hover写回；全生产消费/运行待。
// 原函数：0x00489760。
std::int32_t WindowInventory_HandleEquipment(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    static std::int32_t previousHoveredSlot = -2; // 原全局：0x0057DA68。
    static constexpr const wchar_t* playerHints[] {
        L"装备头部防具的位置。",
        L"装备身体防具的位置。",
        L"装备武器跟盾的位置。",
        L"装备武器跟盾的位置。",
        L"装备脚部防具的位置。",
        L"装备装饰品的位置。",
        L"装备装饰品的位置。",
        L"装备水晶的位置。",
    };
    static constexpr const wchar_t* petHints[] {
        L"装备晶石的位置。",
        L"装备防具的位置。",
        L"装备项圈的位置。",
        L"装备饰品的位置。",
        L"装备饰品的位置。",
        L"装备饰品的位置。",
    };
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    descriptor.color = color;
    dimmed.color = dimmedColor;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    dimmed.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    world::PlayerItemRecord* items;
    std::int32_t count;
    if (!WindowItemSelector_GetItemList_4C72C0(&gItemSelectorCF3B10, &items, &count)) {
        return 0;
    }
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && (WindowDrag_ReadKind() != 2 || WindowDrag_ReadSourceIndex() >= 100)) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = WindowDrag_ReadX();
        const std::int32_t dropY = WindowDrag_ReadY();
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        for (std::int32_t position = 0; position < count; ++position) {
            if (occupancy[position] != 0) {
                continue;
            }
            const std::int32_t left = WindowItemSelector_ReadScreenX(&gItemSelectorCF3B10, index, position, -24);
            const std::int32_t top = WindowItemSelector_ReadScreenY(&gItemSelectorCF3B10, index, position, -24);
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28) <= 0) {
                if (source != position) {
                    WindowItem_RequestTransfer(14, index, source, position);
                }
            } else if (source < 6) {
                if (position >= 6) {
                    WindowItem_TransferQuantity(1, source, 0, position);
                }
            } else if (position < 6) {
                WindowItem_TransferQuantity(0, source, 1, position);
            } else {
                WindowItem_RequestTransfer(14, index, source, position);
            }
            break;
        }
        WindowDrag_Reset();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < count; ++position) {
            if (occupancy[position] != 0) {
                continue;
            }
            const std::int32_t left = WindowItemSelector_ReadScreenX(&gItemSelectorCF3B10, index, position, -24);
            const std::int32_t top = WindowItemSelector_ReadScreenY(&gItemSelectorCF3B10, index, position, -24);
            if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                gItemSelectorCF3B10.hoverIndex3F8 = position;
                break;
            }
        }
    }
    // 后续清hover不改变原来选中的物品引用及用于绘框的命中索引。
    auto* const item = items + hovered;
    const std::int32_t hitIndex = hovered;
    if ((events & 1u) != 0 && hovered >= 0) {
        gHoverTextD68AE8 = gInventoryActorSelectionMode_57DA28 != 0 ? petHints[hovered] : playerHints[hovered];
    }
    if (WindowDrag_ReadKind() != 0 && (events & 0x20u) != 0 && WindowDrag_ReadSourceIndex() >= 100) {
        hovered = -1;
    }
    if ((events & 2u) != 0) {
        currentWindow->state |= 0x2000u;
        if (hovered == -1 && world::gReplyRecordF7C950.used != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            WindowInstance* const window = currentWindow;
            gItemSelectorCF3B10.value3FC = 1;
            if (auto* const handleEquipmentElement = window->FindElementById(WindowInventoryElementId::HandleEquipment); handleEquipmentElement) handleEquipmentElement->mouseCallback = WindowInventory_TabPlayerAttributes;
            return consumed;
        }
    }
    if (previousHoveredSlot != hovered) {
        gWindowStates[WindowId_Inventory].value14 = 0;
    }
    const std::int32_t selectedItem = gSelectedItemCB1DF4;
    previousHoveredSlot = hovered;
    if (selectedItem < 0 && hovered >= 0
        && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (WindowDrag_ReadFocusWindowId() == 14 || WindowDrag_ReadFocusWindowId() == 0) {
            if (item->used != 0 && (*reinterpret_cast<const unsigned char*>(&item->canUseInField) & 1u) != 0) {
                if (item->imageNo == 26569) {
                    WindowInstance* const window = currentWindow;
                    gItemSelectionModeCC40D4 = 1;
                    if (auto* const handleItemsElement = window->FindElementById(WindowInventoryElementId::HandleItems); handleItemsElement) handleItemsElement->mouseCallback = WindowInventory_HandleItemSelection;
                    if (auto* const handleEquipmentElement = currentWindow->FindElementById(WindowInventoryElementId::HandleEquipment); handleEquipmentElement) handleEquipmentElement->mouseCallback = WindowInventory_DrawBlockedEquipment;
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                    const std::uint8_t previousOccupancy = occupancy[hovered];
                    gSelectedItemCB0A44 = hovered;
                    occupancy[hovered] = static_cast<std::uint8_t>(previousOccupancy + 1u);
                    static_cast<void>(WindowDrag_ResetMouse());
                    static_cast<void>(WindowDrag_Start(1, 0, nullptr));
                } else {
                    if ((world::ReadPlayerActionFlagsByte() & 0x80u) == 0
                        && world::IsObservationSceneTransitionActive_440B50() == 0) {
                        const std::int8_t mode = std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28);
                        gSelectedItemCB0A44 = hovered;
                        if (mode > 0) {
                            const std::int32_t group = WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10) + 1;
                            static_cast<void>(network::MoveItemBetweenOwnersRequest_516C90(
                                platform::gConnectionSocket, group, hovered, 0, -1, -1));
                        } else {
                            const std::int32_t y =
                                static_cast<std::int32_t>(world::gPlayerY);
                            const std::int32_t x =
                                static_cast<std::int32_t>(world::gPlayerX);
                            static_cast<void>(network::BeginItemUseRequest_510A00(
                                platform::gConnectionSocket, x, y, hovered, 0));
                        }
                        static_cast<void>(audio::PlaySe_52C450(77, 320));
                    }
                    static_cast<void>(WindowDrag_ResetMouse());
                }
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                static_cast<void>(WindowDrag_ResetMouse());
            }
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            if (item->used != 0) {
                static_cast<void>(WindowItemDrag_Start(hovered, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            if (item->used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Inventory].value14);
                ++page;
                gWindowStates[WindowId_Inventory].value14 = static_cast<std::uint32_t>(page);
                if (page >= item->pages) {
                    gWindowStates[WindowId_Inventory].value14 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source < count) {
            const std::int32_t left = WindowItemSelector_ReadScreenX(&gItemSelectorCF3B10, index, source, -24);
            const std::int32_t top = WindowItemSelector_ReadScreenY(&gItemSelectorCF3B10, index, source, -24);
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if (hovered >= 0) {
        const std::int32_t left = WindowItemSelector_ReadScreenX(&gItemSelectorCF3B10, index, hitIndex, -24);
        const std::int32_t top = WindowItemSelector_ReadScreenY(&gItemSelectorCF3B10, index, hitIndex, -24);
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        if (item->used != 0) {
            gWindowStates[WindowId_Inventory].value10 = static_cast<std::uint32_t>(hovered);
            if (!item->memo.empty() && (item->flags & 0x100u) != 0) {
                gHoverTextD68AE8 = item->name + L"[kok]";
            } else if ((item->flags & 0x200u) != 0) {
                gHoverTextD68AE8 = item->memo + L"[han]";
            }
        }
    }
    for (std::int32_t position = 0; position < count; ++position) {
        const auto& record = items[position];
        if (record.used == 0) {
            continue;
        }
        const std::int32_t x = WindowItemSelector_ReadScreenX(&gItemSelectorCF3B10, index, position, 0);
        const std::int32_t y = WindowItemSelector_ReadScreenY(&gItemSelectorCF3B10, index, position, 0);
        if ((*reinterpret_cast<const unsigned char*>(&record.flags) & 0x80u) == 0) {
            static_cast<void>(cg::render::DrawGraphic(
                x + 105, y + 93, 223, 22739, 0, &descriptor));
        }
        if (occupancy[position] != 0) {
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(cg::render::DrawGraphic(x, y, 223, record.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(cg::render::DrawGraphic(x, y, 223, record.imageNo, 0, &descriptor));
        }
    }
    return consumed;
}

// 原函数：0x00489EE0。
static std::int32_t WindowInventory_TabPlayerAttributes(const std::int32_t, const std::int32_t state)
{
    return WindowItemSelector_HandlePlayerAttributes(&gItemSelectorCF3B10, state) ? 1 : 0;
}

// ISP01：原两mode选物/发送/独立取消及绘制保持，按钮和mode2 flags只读原BYTE。
// 原函数：0x00489F00。
std::int32_t WindowInventory_HandleItemSelection(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    descriptor.color = color;
    dimmed.color = dimmedColor;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    dimmed.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    std::int32_t hovered = -1;
    std::int32_t consumed = 0;
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    if ((events & 0x20u) != 0 && WindowDrag_ReadKind() == 1 && WindowDrag_ReadFocusWindowId() == 14) {
        events |= 1u;
        if ((*reinterpret_cast<const unsigned char*>(&platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
            events |= 2u;
        }
    }
    const auto isMode1Blocked = [](const world::PlayerItemRecord& item) {
        if (item.itemType == 47) {
            return true;
        }
        const std::uint16_t flags = item.flags;
        return (flags & 0x100u) != 0 || item.count > 0
            || ((flags & 0x200u) != 0 && (flags & 0x400u) == 0);
    };
    if ((events & 1u) != 0) {
        const std::int8_t mode = std::bit_cast<std::int8_t>(gItemSelectionModeCC40D4);
        if (mode == 1 || mode == 2) {
            for (std::int32_t position = 0; position < 20; ++position) {
                const auto& item = world::gPlayerRuntimeSlots.items[position + 8];
                if (item.used == 0 || occupancy[position + 8] != 0) {
                    continue;
                }
                if (mode == 1 ? isMode1Blocked(item) : (*reinterpret_cast<const unsigned char*>(&item.flags) & 0x60u) != 0) {
                    continue;
                }
                const WindowInstance* const window = currentWindow;
                auto* const elementPtr = window->FindElementById(NativeElementKey(index));
                if (elementPtr) {
                    const WindowElement& element = (*elementPtr);
                    const std::int32_t left = element.x + 50 * (position % 5) + window->x + 15;
                    const std::int32_t top = element.y + 50 * (position / 5) + window->y + 48;
                    if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                        hovered = position;
                        break;
                    }
                }
            }
        }
    }
    if ((events & 2u) != 0) {
        currentWindow->state |= 0x2000u;
    }

    // 发送完成与右键取消各自执行；第二次退出不得因前面已清理而省略。
    const auto leaveSelection = [&]() {
        WindowInstance* const window = currentWindow;
        gItemSelectionModeCC40D4 = 0;
        if (auto* const handleItemsElement = window->FindElementById(WindowInventoryElementId::HandleItems); handleItemsElement) handleItemsElement->mouseCallback = WindowInventory_HandleItems;
        if (auto* const handleEquipmentElement = currentWindow->FindElementById(WindowInventoryElementId::HandleEquipment); handleEquipmentElement) handleEquipmentElement->mouseCallback = WindowInventory_HandleEquipment;
        const std::int32_t selected = gSelectedItemCB0A44;
        const std::int32_t dialog = world::gCurrentDialogType_581800;
        --occupancy[selected];
        gSelectedItemCB0A44 = -1;
        if (dialog == 29) {
            world::gCurrentDialogType_581800 = -1;
        }
        static_cast<void>(WindowDrag_ResetMouse());
    };
    std::wstring buffer;
    std::wstring escaped;
    if (hovered >= 0 && (events & 2u) != 0) {
        if (world::gPlayerRuntimeSlots.items[hovered + 8].used != 0) {
            const std::uint8_t mode = gItemSelectionModeCC40D4;
            if (mode == 1) {
                const std::int32_t selected = gSelectedItemCB0A44;
                static_cast<void>(network::SendItemTargetPair_511D60(
                    platform::gConnectionSocket, hovered + 8, selected));
            } else if (mode == 2) {
                cg::text::FormatTo(buffer, L"{}", hovered + 8);
                escaped = input::EscapeTextAndNotify(buffer, 15);
                const std::int32_t target = world::gDialogTargetD2DE6C;
                const std::int32_t request = world::gDialogRequestCC21E0;
                const std::int32_t y =
                    static_cast<std::int32_t>(world::gPlayerY);
                const std::int32_t x =
                    static_cast<std::int32_t>(world::gPlayerX);
                static_cast<void>(network::SendDialogResponse_512620(
                    platform::gConnectionSocket, x, y, request, target, 0, escaped));
                leaveSelection();
            }
        }
        consumed = 1;
    }
    if ((*reinterpret_cast<const unsigned char*>(&platform::gLegacyMouseState.edgeButtons) & 2u) != 0) {
        leaveSelection();
    }
    const std::int32_t imageNo = world::gPlayerRuntimeSlots.items[gSelectedItemCB0A44].imageNo;
    const std::int32_t mouseY =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
    const std::int32_t mouseX =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
    static_cast<void>(cg::render::DrawGraphic(mouseX, mouseY, 230, imageNo, 0, &descriptor));
    if (hovered >= 0) {
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(NativeElementKey(index));
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 15;
            const std::int32_t top = element.y + 50 * (hovered / 5) + window->y + 48;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, cg::render::gHoverFramePaletteIndex_D69768, 0);
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(0, 0, 3, 1, 0, L"", 0, 0));
    const std::int8_t mode = std::bit_cast<std::int8_t>(gItemSelectionModeCC40D4);
    if (mode != 1 && mode != 2) {
        return consumed;
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = world::gPlayerRuntimeSlots.items[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(NativeElementKey(index));
        if (elementPtr) {
            const WindowElement& element = (*elementPtr);
            const std::int32_t x = element.x + 50 * (position % 5) + window->x + 39;
            const std::int32_t y = element.y + 50 * (position / 5) + window->y + 73;
            if (mode == 1 ? isMode1Blocked(item) : (*reinterpret_cast<const unsigned char*>(&item.flags) & 0x60u) != 0) {
                static_cast<void>(cg::render::DrawGraphic(x, y, 223, 23040, 0, nullptr));
            }
            if (occupancy[position + 8] != 0) {
                cg::render::DrawGraphicHover(
                    static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                    static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                    223, 249, 0);
                static_cast<void>(cg::render::DrawGraphic(x, y, 223, item.imageNo, 0, &dimmed));
            } else {
                static_cast<void>(cg::render::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
            }
            const std::int32_t count = item.count;
            if (count > 0) {
                cg::text::FormatTo(buffer, L"{:3}", count);
                static_cast<void>(render::DrawText_456780(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                    3, 1, 1, buffer, 0, 0));
            }
        }
    }
    return consumed;
}

} // namespace detail

// 窗口函数实现。

// 原488400固定CF3B10；原内嵌+78预览沿已有唯一静态对象绑定。
// 原函数：0x00488400。
static detail::WindowItemSelectorState* WindowInventory_ConstructSelector()
{
    return detail::WindowItemSelector_ConstructPreview(&detail::gItemSelectorCF3B10, 0);
}

// 原函数：0x004883F0；LCI01启动拥有者替代CRT构造／退出登记。
FirstItemSelectorStartup::FirstItemSelectorStartup()
{
    WindowInventory_ConstructSelector();
}

} // namespace cg::ui
