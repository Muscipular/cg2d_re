#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContextCA0018 {}; // 原全局：0x00CA0018。


// 回调与内部函数前置声明。

// 原函数：0x004E02B0。
static void WindowItem_BuildStorageDescription(std::int32_t index, std::int32_t side, const char* text); // IDR01：外部物品32行描述/S32页数生产；正常正文及原回扫顺序保持。

// 原函数：0x004A92D0。
static std::int32_t WindowBank_FocusAmountInput(std::int32_t unused, std::int32_t state); // 存取数值输入焦点。

// 原函数：0x004A93E0。
static std::int32_t WindowBank_ButtonFirstPage(std::int32_t index, std::int32_t state); // 存放第一页请求。

// 原函数：0x004A94A0。
static std::int32_t WindowBank_ButtonSecondPage(std::int32_t index, std::int32_t state); // 存放第二页请求。

// 原函数：0x004A9560。
static std::int32_t WindowBank_DrawFirstPageSelection(std::int32_t index, std::int32_t unused); // 第一页选中图。

// 原函数：0x004A9580。
static std::int32_t WindowBank_DrawSecondPageSelection(std::int32_t index, std::int32_t unused); // 第二页选中图。

// 原函数：0x004A9B00。
static std::int32_t WindowBank_DrawCounts(std::int32_t unusedIndex, std::int32_t unusedState); // 双侧数量和金额。

// 原函数：0x004A95A0。
static std::int32_t WindowBank_TabItems(std::int32_t index, std::int32_t state); // 物品页签。

// 原函数：0x004A96D0。
static std::int32_t WindowBank_TabPets(std::int32_t index, std::int32_t state); // 宠物页签。

// 原函数：0x004A9820。
static std::int32_t WindowBank_TabDeposit(std::int32_t index, std::int32_t state); // 存款页签。

// 原函数：0x004A9990。
static std::int32_t WindowBank_TabWithdraw(std::int32_t index, std::int32_t state); // 取款页签。

// 原函数：0x004A9D10。
static std::int32_t WindowBank_HandleOwnItems(std::int32_t index, std::int32_t state); // 自有存放物品面板。

// 原函数：0x004AA3D0。
static std::int32_t WindowBank_HandleStoredItems(std::int32_t index, std::int32_t state); // SEP01：完整窗口41外部物品消费，低BYTE/坐标原序与S32 consumed保持。

// 原函数：0x004AA9C0。
static std::int32_t WindowBank_ButtonPreviousPet(std::int32_t index, std::int32_t state); // 宠物上一行。

// 原函数：0x004AAA70。
static std::int32_t WindowBank_ButtonNextPet(std::int32_t index, std::int32_t state); // 宠物下一行。

// 原函数：0x004AAB20。
static std::int32_t WindowBank_ScrollPets(std::int32_t index, std::int32_t state); // 两侧宠物滚轮。

// 原函数：0x004ABB20。
static std::int32_t WindowBank_HandleAmount(std::int32_t index, std::int32_t state); // SDM01：存取原上限/AL确认、发送后fresh金额再清槽/双文本，返回0。

// 原函数：0x004AAEC0。
static std::int32_t WindowBank_HandleOwnPetRow(std::int32_t index, std::int32_t state); // 自有宠物存放行。

// 原函数：0x004AB710。
static std::int32_t WindowBank_HandleStoredPetRow(std::int32_t index, std::int32_t state); // 外部宠物提领行。

// 原函数：0x004A83D0。
static void WindowBank_Release();

// 原函数：0x004A9020。
static std::int32_t WindowBank_Update(std::int32_t value);

// 原函数：0x004A9160。
static std::int32_t WindowBank_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowInputPrototype gStorageInputPrototype57EBA0 { // 原全局：0x0057EBA0。
    {{&cg::ui::detail::gInputContextCA0018}, WindowInputReferenceKind::Context},
    0, -100, 3, 12, 127, {}, 1, 0, 0, 0,
};

// 原全局：0x00CC4218。 存放窗口41每帧清零、元素13绘制的数值。
static std::int32_t gStorageValueCC4218 = 0; // 原全局：0x00CC4218。

static std::int32_t gBankOwnHover57EDCC = -2; // 原全局：0x0057EDCC。

static std::int32_t gBankExternalHover57EDD0 = -2; // 原全局：0x0057EDD0。

static const std::array<std::int32_t, 2> gStorageTitleImages54F4E8 {244390, 244320}; // 原全局：0x0054F4E8。

static const std::array<std::int32_t, 2> gStoragePanelImages54F4F0 {244391, 244321}; // 原全局：0x0054F4F0。

static const std::array<std::int32_t, 2> gStoragePetImages54F4F8 {244392, 244323}; // 原全局：0x0054F4F8。

static const std::array<std::int32_t, 2> gStorageMoneyImages54F500 {244393, 244322}; // 原全局：0x0054F500。

static std::array<std::int32_t, 2> gStorageLastAmountsD67B68 {}; // 原全局：0x00D67B68。

// 原全局：0x00CB1DF8。 动态宽文本，控件借稳定owner。
static std::array<std::wstring, 2> gStorageAmountTextsCB1DF8 {}; // 原全局：0x00CB1DF8。

static const std::array<const wchar_t*, 6> gStorageDepositHints57EDD4 {{ // 原全局：0x0057EDD4。
    L"指定金额。",
    L"指定存入金额。",
    L"请指定金额。",
    L"减少一个位数。",
    L"指定全部金额。",
    L"把指定金额归零。",
}};

static const std::array<const wchar_t*, 6> gStorageWithdrawHints57EDEC {{ // 原全局：0x0057EDEC。
    L"指定金额。",
    L"指定提领金额。",
    L"指定金额数量。",
    L"减少一个位数。",
    L"指定全部金额。",
    L"把指定金额归零。",
}};

static WindowGraphicData gWindowGraphic571018 {244390, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571018。

static WindowGraphicData gWindowGraphic571030 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571030。

static WindowGraphicData gWindowGraphic571048 {243027, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571048。

static WindowGraphicData gWindowGraphic571060 {243021, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571060。

static WindowGraphicData gWindowGraphic571078 {243024, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571078。

static WindowGraphicData gWindowGraphic571090 {243143, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571090。

static WindowGraphicData gWindowGraphic5710A8 {244391, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005710A8。

static WindowGraphicData gWindowGraphic5710C0 {243141, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005710C0。

static WindowGraphicData gWindowGraphic5710D8 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005710D8。

static WindowGraphicData gWindowGraphic5710F0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005710F0。

static WindowGraphicData gWindowGraphic571108 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571108。

static WindowGraphicData gWindowGraphic571120 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571120。

static WindowTextPrototype gWindowText571148 { // 原全局：0x00571148。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"",
};

static WindowNumberData gWindowNumber571150 {&gStorageAmountTextsCB1DF8[0], 0, 3, 1, 0}; // 原全局：0x00571150。

static WindowNumberData gWindowNumber571158 {&gStorageAmountTextsCB1DF8[1], 0, 3, 1, 0}; // 原全局：0x00571158。

static std::uint32_t gWindowMouseFlagsD67880 = 0; // 原全局：0x00D67880。

static std::uint32_t gWindowMouseFlagsD6788C = 0; // 原全局：0x00D6788C。

static WindowElementPrototype gWindowElements571160[38] { // 原全局：0x00571160。
    {WindowElementType_Input, 0, 0, 567, 339, 1, {.unusedValue = 0}, &WindowBank_FocusAmountInput},
    {WindowElementType_Graphic, 547, 9, 11, 11, 1, {.graphic = &gWindowGraphic571120}, &WindowBank_ButtonCloseIcon},
    {WindowElementType_Graphic, 476, 308, 66, 17, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_ButtonClose},
    {WindowElementType_Graphic, 26, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_ButtonFirstPage},
    {WindowElementType_Graphic, 55, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_ButtonSecondPage},
    {WindowElementType_Graphic, 26, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_DrawFirstPageSelection},
    {WindowElementType_Graphic, 55, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_DrawSecondPageSelection},
    {WindowElementType_Graphic, 298, 264, 80, 15, 1, {.graphic = &gWindowGraphic571048}, &WindowBank_TabItems},
    {WindowElementType_Graphic, 383, 264, 80, 15, 1, {.graphic = &gWindowGraphic571060}, &WindowBank_TabPets},
    {WindowElementType_Graphic, 469, 264, 80, 15, 1, {.graphic = &gWindowGraphic571078}, &WindowBank_TabDeposit},
    {WindowElementType_Graphic, 26, 264, 80, 15, 1, {.graphic = &gWindowGraphic571048}, &WindowBank_TabItems},
    {WindowElementType_Graphic, 111, 264, 80, 15, 1, {.graphic = &gWindowGraphic571060}, &WindowBank_TabPets},
    {WindowElementType_Graphic, 197, 264, 80, 15, 1, {.graphic = &gWindowGraphic571078}, &WindowBank_TabWithdraw},
    {WindowElementType_Text, 478, 328, 80, 15, 1, {.text = &gWindowText571148}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 297, 53, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleOwnPetRow},
    {WindowElementType_Graphic, 297, 122, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleOwnPetRow},
    {WindowElementType_Graphic, 297, 191, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleOwnPetRow},
    {WindowElementType_Graphic, 25, 53, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleStoredPetRow},
    {WindowElementType_Graphic, 25, 122, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleStoredPetRow},
    {WindowElementType_Graphic, 25, 191, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleStoredPetRow},
    {WindowElementType_Graphic, 538, 65, 0, 14, 1, {.graphic = &gWindowGraphic5710D8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 538, 65, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD67880}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 538, 55, 11, 11, 1, {.graphic = &gWindowGraphic5710F0}, &WindowBank_ButtonPreviousPet},
    {WindowElementType_Graphic, 538, 247, 11, 11, 1, {.graphic = &gWindowGraphic571108}, &WindowBank_ButtonNextPet},
    {WindowElementType_EventArea, 286, 27, 273, 280, 1, {.unusedValue = 0}, &WindowBank_ScrollPets},
    {WindowElementType_Graphic, 266, 65, 0, 14, 1, {.graphic = &gWindowGraphic5710D8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 266, 65, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD6788C}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 266, 55, 11, 11, 1, {.graphic = &gWindowGraphic5710F0}, &WindowBank_ButtonPreviousPet},
    {WindowElementType_Graphic, 266, 247, 11, 11, 1, {.graphic = &gWindowGraphic571108}, &WindowBank_ButtonNextPet},
    {WindowElementType_EventArea, 14, 27, 273, 280, 1, {.unusedValue = 0}, &WindowBank_ScrollPets},
    {WindowElementType_EventArea, 375, 135, 106, 85, 1, {.unusedValue = 0}, &WindowBank_HandleAmount},
    {WindowElementType_EventArea, 103, 135, 106, 85, 1, {.unusedValue = 0}, &WindowBank_HandleAmount},
    {WindowElementType_Number, 465, 115, 0, 0, 1, {.number = &gWindowNumber571150}, &WindowElement_NoOp},
    {WindowElementType_Number, 193, 115, 0, 0, 1, {.number = &gWindowNumber571158}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 286, 27, 273, 280, 1, {.graphic = &gWindowGraphic571090}, &WindowBank_HandleOwnItems},
    {WindowElementType_Graphic, 14, 27, 273, 280, 1, {.graphic = &gWindowGraphic5710A8}, &WindowBank_HandleStoredItems},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic571018}, &WindowBank_DrawCounts},
    {WindowElementType_EventArea, 571, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口41；原全局：0x0054EB10。
WindowPrototype gWindowPrototypeBank {0, 4, 20, 25, 567, 339, 0x80010101u, 38, gWindowElements571160, &WindowBank_Update, &WindowBank_Draw, &WindowBank_Release}; // 41: 54EB10

// 窗口函数实现。

// IDR01：原4E02B0完整83条；32行提取、仅行首清零及从last前一行回扫/S32页数发布保持。
// 原函数：0x004E02B0。
static void WindowItem_BuildStorageDescription(
    const std::int32_t index, const std::int32_t side, const char* const text)
{
    auto* const record = &gItemDetailStorageCC4D58[side].items[index];
    auto& rows = record->description;
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char* const row = rows[line].data();
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            last = line;
        } else {
            row[0] = '\0';
        }
    }
    // 原回扫从最后成功行的前一行开始，空行不清整行残留。
    while (last > 0 && std::strlen(rows[last - 1].data()) == 0) {
        --last;
    }
    record->pages = last < 0 ? std::int32_t{0} : last / 8 + 1;
}

// 原函数：0x004AAB70。
void WindowBank_DrawOwnPetTooltip(
    const std::int16_t x, const std::int16_t y, const std::int32_t index,
    const std::uint8_t textLayer)
{
    world::PetRuntimeRecord& pet = world::gPetRuntimeRecords[index];
    if (pet.used == 0) {
        return;
    }
    constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{138});
    static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
    static_cast<void>(render::DrawText_456780(
        0, 0, textLayer, 13, 0, L"", 0, 0));
    const char* const name =
        std::strlen(pet.name6BD.data()) != 0 ? pet.name6BD.data() : pet.name6AC.data();
    static_cast<void>(render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 45)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 5)),
        textLayer, 13, color, cg::text::FromGbkWide(name).c_str(), 0, 0));
    std::wstring buffer;
    const auto drawNumber = [&](const std::int32_t value, const std::wstring_view format,
                                const std::int32_t dx, const std::int32_t dy) {
        cg::text::FormatTo(buffer, format, value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + dx)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + dy)),
            textLayer, 13, color, buffer, 0, 0));
    };
    drawNumber(pet.value08, L"{:3}", 35, 22);
    const std::int16_t tribe = std::bit_cast<std::int16_t>(pet.value04);
    if (tribe >= 0 && tribe < 20) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 100)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 22)),
            textLayer, 13, color, gTribeNames57CB54[tribe], 0, 0));
    }
    drawNumber(static_cast<std::int32_t>(pet.values0C[1]), L"{:4}", 210, 5);
    drawNumber(static_cast<std::int32_t>(pet.values0C[3]), L"{:4}", 210, 22);
    drawNumber(pet.values70[0], L"{:3}", 95, 40);
    drawNumber(pet.values70[1], L"{:3}", 155, 40);
    drawNumber(pet.values70[2], L"{:3}", 215, 40);
    drawNumber(pet.values70[3], L"{:3}", 95, 55);
    drawNumber(pet.values70[4], L"{:3}", 155, 55);
    drawNumber(pet.values70[10], L"{:3}", 215, 55);
    drawNumber(pet.value6A4, L"{:2}", 45, 55);
}

// 原函数：0x004AB400。
void WindowBank_DrawStoredPetTooltip(
    const std::int16_t x, const std::int16_t y, const std::int32_t index,
    const std::uint8_t textLayer)
{
    const auto* const record = &gPetStorageD64E18[index];
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return;
    }
    constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{138});
    static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
    static_cast<void>(render::DrawText_456780(
        0, 0, textLayer, 13, color, L"", 0, 0));
    static_cast<void>(render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 45)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 5)),
        textLayer, 13, color, cg::text::FromGbkWide(record->name.data()).c_str(), 0, 0));
    std::wstring buffer;
    const auto drawNumber = [&](const std::int32_t value, const std::wstring_view format,
                                const std::int32_t dx, const std::int32_t dy) {
        cg::text::FormatTo(buffer, format, value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + dx)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + dy)),
            textLayer, 13, color, buffer, 0, 0));
    };
    drawNumber(record->level, L"{:3}", 35, 22);
    std::int16_t tribe;
    std::memcpy(&tribe, &record->tribe, sizeof(tribe));
    if (tribe >= 0 && tribe < 20) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 100)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 22)),
            textLayer, 13, color, gTribeNames57CB54[tribe], 0, 0));
    }
    drawNumber(record->maxHp, L"{:4}", 210, 5);
    drawNumber(record->maxFp, L"{:4}", 210, 22);
    drawNumber(record->combatStats[0], L"{:3}", 95, 40);
    drawNumber(record->combatStats[1], L"{:3}", 155, 40);
    drawNumber(record->combatStats[2], L"{:3}", 215, 40);
    drawNumber(record->combatStats[3], L"{:3}", 95, 55);
    drawNumber(record->combatStats[4], L"{:3}", 155, 55);
    drawNumber(record->combatValue44, L"{:3}", 215, 55);
    drawNumber(record->value58, L"{:2}", 45, 55);
}

// 原函数：0x004A83A0。
WindowInstance* WindowBank_Open(
    const std::int32_t kind, const std::int32_t value14, const std::int32_t userState)
{
    gWindowStates[WindowId_Bank].value10 = static_cast<std::uint32_t>(kind);
    WindowInstance* const window = WindowRuntime_Open(WindowId_Bank);
    gWindowStates[WindowId_Bank].window = window;
    gWindowStates[WindowId_Bank].value14 = static_cast<std::uint32_t>(value14);
    gWindowStates[WindowId_Bank].userState.value = static_cast<std::uint32_t>(userState);
    return window;
}

// 原函数：0x004A83D0。
static void WindowBank_Release()
{
    const std::uint32_t kind = gWindowStates[WindowId_Bank].value10;
    if ((kind == 0 && world::gValue581800 == 23)
        || (kind == 1 && world::gValue581800 == 33)) {
        world::gValue581800 = -1;
    }
    std::wstring buffer;
    buffer = input::EscapeTextAndNotify(L"C", 255);
    const std::int32_t userState = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].userState.value);
    const std::int32_t value14 = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value14);
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(network::SendDialogResponse_512620(
        platform::gConnectionSocket, x, y, value14, userState, 0, buffer));
    WindowInstance* const numberWindow = gWindowStates[WindowId_Quantity].window;
    if (numberWindow != nullptr && gWindowStates[WindowId_Quantity].value20 == 41) {
        numberWindow->state |= 0x8000u;
    }
    if (input::IsCurrentContext_503E40(nullptr)) {
        input::SetContext_44B330(&cg::ui::detail::gInputContext626AA0);
    }
}

// 原函数：0x004A9020。
static std::int32_t WindowBank_Update(const std::int32_t value)
{
    if (value == -1) {
        auto* const title = static_cast<WindowGraphicData*>(gCurrentWindow->elements[36].data);
        title->imageNo = gStorageTitleImages54F4E8[gWindowStates[WindowId_Bank].value10];
        WindowInstance* const initialWindow = gCurrentWindow;
        gWindowStates[WindowId_Bank].value28 = 0;
        gWindowStates[WindowId_Bank].value24 = 0;
        auto* const ownPanel = static_cast<WindowGraphicData*>(initialWindow->elements[34].data);
        ownPanel->imageNo = 243143;
        auto* const otherPanel = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        otherPanel->imageNo = gStoragePanelImages54F4F0[gWindowStates[WindowId_Bank].value10];
        for (std::int32_t index = 14; index < 34; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        gCurrentWindow->elements[13].visible = 0;
    }
    if (WindowProfile_CheckDistance(1, 1) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (gWindowStates[WindowId_Bank].value10 == 0 && world::gValue581800 != 23) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (gWindowStates[WindowId_Bank].value10 == 1 && world::gValue581800 != 33) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    gStorageValueCC4218 = 0;
    gWindowStates[WindowId_Bank].actionFlags1C = 0xFFFFFFFFu;
    return 1;
}

// SPD01：原4A9160完整114条；两滚动flags只读BYTE，金额/预览/鼠标RNG原序及所有绘制返回1保持。
// 原函数：0x004A9160。
static std::int32_t WindowBank_Draw(const std::int32_t)
{
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:10}", gStorageValueCC4218);
    auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[13].data);
    text->text = buffer;
    WindowElement* elements = gCurrentWindow->elements;
    const auto* const firstScroll = static_cast<WindowMouseData*>(elements[21].data);
    if ((*reinterpret_cast<const unsigned char*>(&firstScroll->flags) & 1u) != 0) {
        gWindowStates[WindowId_Bank].value24 = static_cast<std::uint32_t>(WindowScroll_ReadPosition(elements + 21, 2));
    }
    elements = gCurrentWindow->elements;
    const auto* const secondScroll = static_cast<WindowMouseData*>(elements[26].data);
    if ((*reinterpret_cast<const unsigned char*>(&secondScroll->flags) & 1u) != 0) {
        gWindowStates[WindowId_Bank].value28 = static_cast<std::uint32_t>(WindowScroll_ReadPosition(elements + 26, 2));
    }
    const std::int32_t preview = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].actionFlags1C);
    if (preview >= 0) {
        if (preview >= 200) {
            gWindowStates[WindowId_Bank].actionFlags1C = static_cast<std::uint32_t>(preview - 200);
            const bool left = (platform::gLegacyMouseState.mouseX < 320);
            const std::int32_t x = left ? 413 : 0;
            const std::int32_t index =
                std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].actionFlags1C);
            if (index < 100) {
                static_cast<void>(WindowTrade_DrawOwnPetTooltip(x, 0, index, true));
            } else {
                const std::uint8_t externalIndex = static_cast<std::uint8_t>(
                    static_cast<std::uint8_t>(index) - 100);
                static_cast<void>(WindowTrade_DrawRemotePetTooltip(x, 0, externalIndex));
            }
        } else {
            const std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value20);
            if (preview >= 100) {
                static_cast<void>(WindowItem_DrawDescriptionTooltip(preview - 100, page));
            } else {
                static_cast<void>(WindowItem_DrawEquipmentTooltip(preview, page));
            }
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004A92D0。
static std::int32_t WindowBank_FocusAmountInput(const std::int32_t, const std::int32_t state)
{
    const WindowElement* const elements = gCurrentWindow->elements;
    if (elements[30].visible == 0 && elements[31].visible == 0) {
        if (input::gCurrentInputData == &cg::ui::detail::gInputContextCA0018) {
            static_cast<void>(WindowChat_RestoreInput());
        }
        return 0;
    }
    if ((state & 2) != 0) {
        WindowInstance* const window = gWindowStates[WindowId_Bank].window;
        static_cast<void>(WindowInput_ActivateElement(&gStorageInputPrototype57EBA0, 0, -100, 0));
        input::SetContext_44B330(&cg::ui::detail::gInputContextCA0018);
        gFocusedElementDataD45F94 = window->elements[0].data;
        static_cast<WindowInputData*>(window->elements[0].data)->context = &cg::ui::detail::gInputContextCA0018;
    }
    return 0;
}

// 原函数：0x004A93E0。
static std::int32_t WindowBank_ButtonFirstPage(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    if (gLoginWindowRuntime.valueD67618 == 0) {
        elements[3].visible = 0;
        gCurrentWindow->elements[4].visible = 1;
    }
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"请选择银行分页";
    }
    if ((state & 2) != 0) {
        gLoginWindowRuntime.valueD67618 = 0;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        static_cast<void>(network::SelectBankPageRequest_5127B0(platform::gConnectionSocket, 1));
    }
    graphic->imageNo = 243173;
    if ((state & 1) != 0) {
        graphic->imageNo = 243175;
    }
    return 0;
}

// 原函数：0x004A94A0。
static std::int32_t WindowBank_ButtonSecondPage(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    if (gLoginWindowRuntime.valueD67618 == 1) {
        elements[3].visible = 1;
        gCurrentWindow->elements[4].visible = 0;
    }
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"请选择银行分页";
    }
    if ((state & 2) != 0) {
        gLoginWindowRuntime.valueD67618 = 1;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        static_cast<void>(network::SelectBankPageRequest_5127B0(platform::gConnectionSocket, 2));
    }
    graphic->imageNo = 243176;
    if ((state & 1) != 0) {
        graphic->imageNo = 243178;
    }
    return 0;
}

// 原函数：0x004A9560。
static std::int32_t WindowBank_DrawFirstPageSelection(const std::int32_t index, const std::int32_t)
{
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 243174;
    return 0;
}

// 原函数：0x004A9580。
static std::int32_t WindowBank_DrawSecondPageSelection(const std::int32_t index, const std::int32_t)
{
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 243177;
    return 0;
}

// 原函数：0x004A95A0。
static std::int32_t WindowBank_TabItems(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"存放物品。";
    }
    if ((state & 2) != 0) {
        const auto* const panel = static_cast<WindowGraphicData*>(window->elements[34].data);
        static_cast<void>(audio::PlaySe_52C450(panel->imageNo == 243143 ? 55 : 51, 320));
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo = 243143;
        auto* const external = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        external->imageNo = gStoragePanelImages54F4F0[gWindowStates[WindowId_Bank].value10];
        for (std::int32_t element = 14; element < 34; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gCurrentWindow->elements[34].mouseCallback = WindowBank_HandleOwnItems;
        gCurrentWindow->elements[35].mouseCallback = WindowBank_HandleStoredItems;
    }
    graphic->imageNo = 243027;
    if ((state & 1) != 0) {
        graphic->imageNo = 243029;
    }
    if (static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo == 243143) {
        graphic->imageNo = 243028;
    }
    return 0;
}

// 原函数：0x004A96D0。
static std::int32_t WindowBank_TabPets(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"存放宠物。";
    }
    if ((state & 2) != 0) {
        const auto* const panel = static_cast<WindowGraphicData*>(window->elements[34].data);
        static_cast<void>(audio::PlaySe_52C450(panel->imageNo == 243145 ? 55 : 51, 320));
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo = 243145;
        auto* const external = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        external->imageNo = gStoragePetImages54F4F8[gWindowStates[WindowId_Bank].value10];
        for (std::int32_t element = 14; element < 34; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        for (std::int32_t element = 14; element <= 29; ++element) {
            gCurrentWindow->elements[element].visible = 1;
        }
        gCurrentWindow->elements[34].mouseCallback = WindowElement_NoOp;
        gCurrentWindow->elements[35].mouseCallback = WindowElement_NoOp;
    }
    graphic->imageNo = 243021;
    if ((state & 1) != 0) {
        graphic->imageNo = 243023;
    }
    if (static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo == 243145) {
        graphic->imageNo = 243022;
    }
    return 0;
}

// 原函数：0x004A9820。
static std::int32_t WindowBank_TabDeposit(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"存放金钱。";
    }
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(window->elements[30].visible != 0 ? 55 : 51, 320));
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo = 243144;
        auto* const external = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        external->imageNo = gStorageMoneyImages54F500[gWindowStates[WindowId_Bank].value10];
        for (std::int32_t element = 14; element < 34; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gCurrentWindow->elements[30].visible = 1;
        gCurrentWindow->elements[32].visible = 1;
        gCurrentWindow->elements[33].visible = 1;
        gCurrentWindow->elements[34].mouseCallback = WindowElement_NoOp;
        gCurrentWindow->elements[35].mouseCallback = WindowElement_NoOp;
        gCurrentWindow->elements[30].state = 0;
        static_cast<void>(WindowBank_FocusAmountInput(0, 2));
    }
    graphic->imageNo = 243024;
    if ((state & 1) != 0) {
        graphic->imageNo = 243026;
    }
    if (gCurrentWindow->elements[30].visible != 0) {
        graphic->imageNo = 243025;
    }
    return 0;
}

// 原函数：0x004A9990。
static std::int32_t WindowBank_TabWithdraw(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"提领金钱。";
    }
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(window->elements[31].visible != 0 ? 55 : 51, 320));
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo = 243144;
        auto* const external = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        external->imageNo = gStorageMoneyImages54F500[gWindowStates[WindowId_Bank].value10];
        for (std::int32_t element = 14; element < 34; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gCurrentWindow->elements[31].visible = 1;
        gCurrentWindow->elements[32].visible = 1;
        gCurrentWindow->elements[33].visible = 1;
        gCurrentWindow->elements[34].mouseCallback = WindowElement_NoOp;
        gCurrentWindow->elements[35].mouseCallback = WindowElement_NoOp;
        gCurrentWindow->elements[31].state = 0;
        static_cast<void>(WindowBank_FocusAmountInput(0, 2));
    }
    graphic->imageNo = 243024;
    if ((state & 1) != 0) {
        graphic->imageNo = 243026;
    }
    if (gCurrentWindow->elements[31].visible != 0) {
        graphic->imageNo = 243025;
    }
    return 0;
}

// 原函数：0x004A9D10。
static std::int32_t WindowBank_HandleOwnItems(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    dimmed.color = dimmedColor;
    dimmed.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gValueD67B08.data());
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
        const WindowInstance* const window = gCurrentWindow;
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 13;
            const std::int32_t top = element.y + 52 * (position / 5) + window->y + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (source != position + 8) {
                WindowItem_RequestTransfer(41, index, source, position + 8);
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
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 13;
            const std::int32_t top = element.y + 52 * (position / 5) + window->y + 26;
            if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
    }
    if ((events & 1u) != 0 && hovered >= 0
        && world::gPlayerRuntimeSlots.items[hovered + 8].used != 0) {
        if ((world::gPlayerRuntimeSlots.items[hovered + 8].flags & 0x60u) != 0) {
            gHoverTextD68AE8 = L"此物品无法存放。";
        } else {
            gHoverTextD68AE8 = L"用滑鼠拖放来存放物品。";
        }
    }
    if (WindowDrag_ReadKind() != 0 && (events & 0x20u) != 0 && WindowDrag_ReadSourceIndex() >= 100
        && world::gPlayerRuntimeSlots.items[hovered + 8].used != 0
        && (world::gPlayerRuntimeSlots.items[hovered + 8].flags & 0x60u) != 0) {
        hovered = -1;
    }
    if (gBankOwnHover57EDCC != hovered) {
        gWindowStates[WindowId_Bank].value20 = 0;
    }
    gBankOwnHover57EDCC = hovered;
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (WindowDrag_ReadFocusWindowId() == 41 || WindowDrag_ReadFocusWindowId() == 0) {
            static_cast<void>(WindowDrag_ResetMouse());
            std::int32_t target = 0;
            for (; target < 20; ++target) {
                std::uint16_t used;
                std::memcpy(&used, &gItemDetailStorageCC4D58[0].items[target], sizeof(used));
                if (used == 0) {
                    break;
                }
            }
            if (target >= 20 || world::gPlayerRuntimeSlots.items[hovered + 8].used == 0) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                WindowItem_RequestTransfer(41, index, hovered + 8, target + 100);
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
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value20);
                const std::int32_t pages = world::gPlayerRuntimeSlots.items[hovered + 8].pages;
                ++page;
                gWindowStates[WindowId_Bank].value20 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[WindowId_Bank].value20 = 0;
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
            const std::int32_t left = element.x + 50 * ((source - 8) % 5) + window->x + 13;
            const std::int32_t top = element.y + 52 * ((source - 8) / 5) + window->y + 26;
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 13;
        const std::int32_t top = element.y + 52 * (hovered / 5) + window->y + 26;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, cg::render::gValueD69768, 0);
        if (world::gPlayerRuntimeSlots.items[hovered + 8].used != 0) {
            gWindowStates[WindowId_Bank].actionFlags1C = static_cast<std::uint32_t>(hovered + 8);
            if (WindowDrag_ReadKind() == 2 && WindowDrag_ReadSourceIndex() >= 100) {
                ++gStorageValueCC4218;
            }
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, L"", 0, 0));
    descriptor.color = color;
    std::wstring buffer;
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = world::gPlayerRuntimeSlots.items[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (position % 5) + window->x + 37;
        const std::int32_t y = element.y + 52 * (position / 5) + window->y + 51;
        if ((item.flags & 0x60u) != 0) {
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
    return consumed;
}

// SEP01：原4AA3D0完整478条；flags只读低BYTE，坐标按原S32取样序，窗口41转移/翻页/数量显示保持。
// 原函数：0x004AA3D0。
static std::int32_t WindowBank_HandleStoredItems(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && (WindowDrag_ReadKind() != 2 || WindowDrag_ReadSourceIndex() < 8)) {
        events &= 0x9Fu;
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
            const std::int32_t left = windowX + 50 * (position % 5) + elementX + 13;
            const std::int32_t top = windowY + 52 * (position / 5) + elementY + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (source != position + 100
                && (source >= 100 || (*reinterpret_cast<const unsigned char*>(
                    &world::gPlayerRuntimeSlots.items[source].flags) & 0x60u) == 0)) {
                WindowItem_RequestTransfer(41, index, source, position + 100);
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
            const std::int32_t left = elementX + 50 * (position % 5) + windowX + 13;
            const std::int32_t elementY = element.y;
            const std::int32_t windowY = window->y;
            const std::int32_t top = elementY + 52 * (position / 5) + windowY + 26;
            if (cg::ui::HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
    }
    if (WindowDrag_ReadKind() != 0 && (events & 0x20u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source < 100 && (*reinterpret_cast<const unsigned char*>(
                    &world::gPlayerRuntimeSlots.items[source].flags) & 0x60u) != 0) {
            hovered = -1;
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        std::uint16_t used;
        std::memcpy(&used, &gItemDetailStorageCC4D58[0].items[hovered], sizeof(used));
        if (used != 0) {
            gHoverTextD68AE8 = L"用滑鼠拖放来提领物品。";
        }
    }
    if (gBankExternalHover57EDD0 != hovered) {
        gWindowStates[WindowId_Bank].value20 = 0;
    }
    gBankExternalHover57EDD0 = hovered;
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (WindowDrag_ReadFocusWindowId() == 41 || WindowDrag_ReadFocusWindowId() == 0) {
            static_cast<void>(WindowDrag_ResetMouse());
            std::int32_t target = 0;
            while (target < 20 && world::gPlayerRuntimeSlots.items[target + 8].used != 0) {
                ++target;
            }
            if (target >= 20) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                std::uint16_t used;
                std::memcpy(&used, &gItemDetailStorageCC4D58[0].items[hovered], sizeof(used));
                if (used == 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    WindowItem_RequestTransfer(41, index, hovered + 100, target + 8);
                }
            }
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        const auto* const record = &gItemDetailStorageCC4D58[0].items[hovered];
        if ((events & 2u) != 0) {
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                static_cast<void>(WindowItemDrag_Start(hovered + 100, 0));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value20);
                std::int32_t pages;
                std::memcpy(&pages, &record->pages, sizeof(pages));
                ++page;
                gWindowStates[WindowId_Bank].value20 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[WindowId_Bank].value20 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (WindowDrag_ReadKind() == 2) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 100) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t elementX = element.x;
            const std::int32_t windowX = window->x;
            const std::int32_t left = elementX + 50 * ((source - 100) % 5) + windowX + 13;
            const std::int32_t elementY = element.y;
            const std::int32_t windowY = window->y;
            const std::int32_t top = elementY + 52 * ((source - 100) / 5) + windowY + 26;
            cg::render::DrawGraphicHover(
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
        const std::int32_t left = elementX + 50 * (hovered % 5) + windowX + 13;
        const std::int32_t elementY = element.y;
        const std::int32_t windowY = window->y;
        const std::int32_t top = elementY + 52 * (hovered / 5) + windowY + 26;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, cg::render::gValueD69768, 0);
        std::uint16_t used;
        std::memcpy(&used, &gItemDetailStorageCC4D58[0].items[hovered], sizeof(used));
        if (used != 0) {
            gWindowStates[WindowId_Bank].actionFlags1C = static_cast<std::uint32_t>(hovered + 100);
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, L"", 0, 0));
    descriptor.color = color;
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
        const std::int32_t x = elementX + 50 * (position % 5) + windowX + 37;
        const std::int32_t elementY = element.y;
        const std::int32_t windowY = window->y;
        const std::int32_t y = elementY + 52 * (position / 5) + windowY + 51;
        std::int32_t imageNo;
        std::memcpy(&imageNo, &record->imageNo, sizeof(imageNo));
        static_cast<void>(cg::render::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
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

// 原函数：0x004AA9C0。
static std::int32_t WindowBank_ButtonPreviousPet(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    WindowState* const windowState = gCurrentWindowState;
    std::uint32_t* const scroll = index >= 25 ? &windowState->value28 : &windowState->value24;
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t value = std::bit_cast<std::int32_t>(*scroll);
        if (value > 0) {
            *scroll = static_cast<std::uint32_t>(value - 1);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t current = std::bit_cast<std::int32_t>(*scroll);
        static_cast<void>(WindowScroll_SetPosition(&window->elements[index - 1], 2, current));
        result = 1;
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return result;
}

// 原函数：0x004AAA70。
static std::int32_t WindowBank_ButtonNextPet(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    WindowState* const windowState = gCurrentWindowState;
    std::uint32_t* const scroll = index >= 25 ? &windowState->value28 : &windowState->value24;
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t value = std::bit_cast<std::int32_t>(*scroll);
        if (value < 2) {
            *scroll = static_cast<std::uint32_t>(value + 1);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(*scroll);
        WindowInstance* const window = gCurrentWindow;
        static_cast<void>(WindowScroll_SetPosition(&window->elements[index - 2], 2, current));
        result = 1;
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return result;
}

// 原函数：0x004AAB20。
static std::int32_t WindowBank_ScrollPets(const std::int32_t index, const std::int32_t state)
{
    WindowState* const windowState = gCurrentWindowState;
    std::uint32_t* const scroll = index >= 25 ? &windowState->value28 : &windowState->value24;
    if ((state & 0x21) != 0) {
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t current = std::bit_cast<std::int32_t>(*scroll);
        *scroll = static_cast<std::uint32_t>(
            WindowScroll_ApplyWheel(&gCurrentWindow->elements[index - 3], current, 2, wheel));
    }
    return 0;
}

// 原函数：0x004AAEC0。
static std::int32_t WindowBank_HandleOwnPetRow(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t scroll = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value24);
    const std::int32_t petIndex = std::bit_cast<std::int16_t>(
        world::gPetPairsDE0B08[index + scroll - 14].index);
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    // 原4AAEEC/4AB245各执行一次，三次索引查询和费用累计都不能复用前次结果。
    const auto drawDragSource = [index]() {
        if (WindowDrag_ReadKind() == 3) {
            const std::int32_t source = WindowDrag_ReadSourceIndex();
            if (source < 100 && WindowDrag_ReadFocusWindowId() == 41 && WindowDrag_ReadFocusElement() == index) {
                const std::int32_t firstPosition = WindowItem_FindSortedPetIndex(source);
                const std::int32_t firstScroll = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value24);
                if (firstPosition - firstScroll >= 0) {
                    const std::int32_t secondPosition = WindowItem_FindSortedPetIndex(source);
                    const std::int32_t secondScroll =
                        std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value24);
                    if (secondPosition - secondScroll < 3) {
                        const std::int32_t position = WindowItem_FindSortedPetIndex(source);
                        const std::int32_t currentScroll =
                            std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value24);
                        const std::int32_t elementIndex = 14 - currentScroll + position;
                        const WindowInstance* const window = gCurrentWindow;
                        const std::int32_t y = window->y;
                        const std::int32_t x = window->x;
                        const WindowElement& element = window->elements[elementIndex];
                        cg::render::DrawGraphicHover(
                            static_cast<std::uint16_t>(x + element.x + 2),
                            static_cast<std::uint16_t>(y + element.y + 2),
                            static_cast<std::uint16_t>(x + element.width + element.x - 2),
                            static_cast<std::uint16_t>(y + element.height + element.y - 2),
                            223, 254, 0);
                    }
                }
                const std::int32_t level = world::gPlayerRuntimeSlots.level;
                gStorageValueCC4218 += level;
            }
        }
    };
    drawDragSource();
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 3) {
        consumed = 1;
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source != petIndex) {
            bool allowed = true;
            if (gWindowStates[WindowId_Bank].value10 == 1 && petIndex < 5 && source >= 100) {
                const auto& pet = world::gPetRuntimeRecords[petIndex];
                const std::int32_t itemSlots = pet.values6D8[0];
                bool empty = true;
                for (std::int32_t slot = 0; slot < itemSlots; ++slot) {
                    if (pet.items6E4[slot].used != 0) {
                        empty = false;
                        break;
                    }
                }
                allowed = pet.values6D8[1] <= 0 && empty;
            }
            if (allowed) {
                WindowItem_RequestPetTransfer(41, index, source, petIndex);
            } else {
                world::Sub_44B050(L"宠物欲放入家族宠物库，请卸下宠物身上的装备后再操作。" /* 原常量参考：0x00587900。 */, 4, 3);
            }
        }
        WindowDrag_Reset();
    }
    const bool hover = (events & 1u) != 0;
    if (hover && world::gPetRuntimeRecords[petIndex].used != 0) {
        const std::int32_t level = world::gPlayerRuntimeSlots.level;
        const std::int32_t amount = world::gPlayerRuntimeSlots.valueFC;
        if (amount >= level) {
            gHoverTextD68AE8 = L"用滑鼠拖放来存放宠物。";
        }
    }
    const auto& pet = world::gPetRuntimeRecords[petIndex];
    const std::uint16_t used = pet.used;
    if (used != 0 && (events & 0x21u) != 0
        && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (WindowDrag_ReadFocusWindowId() == 41 || WindowDrag_ReadFocusWindowId() == 0) {
            static_cast<void>(WindowDrag_ResetMouse());
            std::int32_t target = 0;
            for (; target < 5; ++target) {
                std::uint16_t targetUsed;
                std::memcpy(&targetUsed, &gPetStorageD64E18[target], sizeof(targetUsed));
                if (targetUsed == 0) {
                    break;
                }
            }
            if (target < 5) {
                bool allowed = true;
                if (gWindowStates[WindowId_Bank].value10 == 1 && petIndex < 5) {
                    const std::int32_t itemSlots = pet.values6D8[0];
                    bool empty = true;
                    for (std::int32_t slot = 0; slot < itemSlots; ++slot) {
                        if (pet.items6E4[slot].used != 0) {
                            empty = false;
                            break;
                        }
                    }
                    allowed = pet.values6D8[1] <= 0 && empty;
                }
                if (allowed) {
                    WindowItem_RequestPetTransfer(41, index, petIndex, target + 100);
                } else {
                    world::Sub_44B050(L"宠物欲放入家族宠物库，请卸下宠物身上的装备后再操作。" /* 原常量参考：0x00587900。 */, 4, 3);
                }
            }
        }
        consumed = 1;
    } else if (hover && (events & 2u) != 0) {
        if (used != 0) {
            static_cast<void>(WindowPetDrag_Start(petIndex, 1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    drawDragSource();
    if (hover) {
        const std::uint32_t imageNo = cg::render::gValueD69768;
        WindowElement_DrawHoverFrame(&gCurrentWindow->elements[index], 1, imageNo);
        if (pet.used != 0) {
            gWindowStates[WindowId_Bank].actionFlags1C = static_cast<std::uint32_t>(petIndex + 200);
        }
    }
    if ((events & 0x20u) != 0) {
        const std::uint32_t imageNo = cg::render::gValueD69768;
        WindowElement_DrawHoverFrame(&gCurrentWindow->elements[index], 1, imageNo);
        if (WindowDrag_ReadKind() == 3 && WindowDrag_ReadSourceIndex() >= 100) {
            const std::int32_t level = world::gPlayerRuntimeSlots.level;
            gStorageValueCC4218 += level;
        }
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement& element = window->elements[index];
    const std::int16_t y = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(element.y + window->y));
    const std::int16_t x = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(element.x + window->x));
    WindowBank_DrawOwnPetTooltip(x, y, petIndex, 3);
    return consumed;
}

// 原函数：0x004AB710。
static std::int32_t WindowBank_HandleStoredPetRow(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t scroll = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value28);
    const std::int32_t petIndex = scroll + index - 17;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    std::wstring buffer;
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 3) {
        consumed = 1;
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source != petIndex + 100) {
            bool allowed = true;
            if (gWindowStates[WindowId_Bank].value10 == 1 && source < 5) {
                const auto& sourcePet = world::gPetRuntimeRecords[source];
                const std::int32_t weight = sourcePet.values6D8[1];
                const std::int32_t itemSlots = sourcePet.values6D8[0];
                const bool hasWeight = weight > 0;
                // 此路径仅在重量与有效装备同时存在时拒绝，原循环固定六槽。
                for (std::int32_t slot = 0; slot < 6; ++slot) {
                    if (slot < itemSlots && sourcePet.items6E4[slot].used != 0) {
                        allowed = !hasWeight;
                        break;
                    }
                }
            }
            if (allowed) {
                WindowItem_RequestPetTransfer(41, index, source, petIndex + 100);
            } else {
                world::Sub_44B050(L"宠物欲放入家族宠物库，请卸下宠物身上的装备后再操作。" /* 原常量参考：0x00587900。 */, 4, 3);
            }
        }
        WindowDrag_Reset();
    }
    const bool hover = (events & 1u) != 0;
    if (hover) {
        std::uint16_t used;
        std::memcpy(&used, &gPetStorageD64E18[petIndex], sizeof(used));
        if (used != 0) {
            gHoverTextD68AE8 = L"用滑鼠拖放来提领宠物。";
        }
    }
    const auto* const record = &gPetStorageD64E18[petIndex];
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used != 0 && (events & 0x21u) != 0
        && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (WindowDrag_ReadFocusWindowId() == 41 || WindowDrag_ReadFocusWindowId() == 0) {
            static_cast<void>(WindowDrag_ResetMouse());
            std::int32_t position = 0;
            for (; position < 5; ++position) {
                const std::int32_t target = std::bit_cast<std::int16_t>(
                    world::gPetPairsDE0B08[position].index);
                if (world::gPetRuntimeRecords[target].used == 0) {
                    break;
                }
            }
            if (position < 5) {
                const std::int32_t target = std::bit_cast<std::int16_t>(
                    world::gPetPairsDE0B08[position].index);
                cg::text::FormatTo(buffer, L"P|{}|{}", petIndex + 100, target);
                const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 255);
                const std::int32_t userState =
                    std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].userState.value);
                const std::int32_t value14 = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value14);
                const std::int32_t y =
                    static_cast<std::int32_t>(world::gPlayerY);
                const std::int32_t x =
                    static_cast<std::int32_t>(world::gPlayerX);
                static_cast<void>(network::SendDialogResponse_512620(
                    platform::gConnectionSocket, x, y, value14, userState, 0, escapedReply));
            }
        }
        consumed = 1;
    } else if (hover && (events & 2u) != 0) {
        if (used != 0) {
            static_cast<void>(WindowPetDrag_Start(petIndex + 100, 1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (WindowDrag_ReadKind() == 3) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source < 100) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t level = world::gPlayerRuntimeSlots.level;
            auto* const text = static_cast<WindowTextData*>(window->elements[13].data);
            cg::text::FormatTo(text->text, L"{:10}", level);
        } else {
            const std::int32_t slot = source - 100;
            if (WindowDrag_ReadFocusWindowId() == 41 && WindowDrag_ReadFocusElement() == index) {
                const std::int32_t currentScroll =
                    std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value28);
                const std::int32_t row = slot - currentScroll;
                if (row >= 0 && row < 3) {
                    const WindowInstance* const window = gCurrentWindow;
                    const std::int32_t y = window->y;
                    const std::int32_t x = window->x;
                    const WindowElement& element = window->elements[row + 17];
                    cg::render::DrawGraphicHover(
                        static_cast<std::uint16_t>(x + element.x + 2),
                        static_cast<std::uint16_t>(y + element.y + 2),
                        static_cast<std::uint16_t>(x + element.width + element.x - 2),
                        static_cast<std::uint16_t>(y + element.height + element.y - 2),
                        223, 254, 0);
                }
            }
        }
    }
    if (hover) {
        std::memcpy(&used, record, sizeof(used));
        if (used != 0) {
            const std::uint32_t imageNo = cg::render::gValueD69768;
            WindowElement_DrawHoverFrame(&gCurrentWindow->elements[index], 1, imageNo);
            gWindowStates[WindowId_Bank].actionFlags1C = static_cast<std::uint32_t>(petIndex + 300);
        }
    }
    if ((events & 0x20u) != 0) {
        const std::uint32_t imageNo = cg::render::gValueD69768;
        WindowElement_DrawHoverFrame(&gCurrentWindow->elements[index], 1, imageNo);
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement& element = window->elements[index];
    const std::int16_t y = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(element.y + window->y));
    const std::int16_t x = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(element.x + window->x));
    WindowBank_DrawStoredPetTooltip(x, y, petIndex, 3);
    return consumed;
}

// SDM01：原4ABB20完整233条；存/取原上限、AL钳回、转义后字段/Y-X与发送后fresh金额发布保，真实返回0。
// 原函数：0x004ABB20。
static std::int32_t WindowBank_HandleAmount(const std::int32_t index, const std::int32_t state)
{
    // 原未识别kind/side或非30/31元素走未初始化局部路径，不补默认值。
    std::int32_t maximum;
    std::uint8_t result;
    if (index == 30) {
        const std::uint32_t kind = gWindowStates[WindowId_Bank].value10;
        if (kind == 0 || kind == 1) {
            const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
            if (side == 0 || side == 1) {
                if (kind == 0) {
                    const std::int32_t limit = world::gPlayerRuntimeSlots.values15BD0[static_cast<std::int32_t>(side + 1)];
                    std::int32_t amount;
                    std::memcpy(&amount,
                        &gItemDetailStorageCC4D58[side].amount, sizeof(amount));
                    maximum = limit - amount;
                } else {
                    std::int32_t amount;
                    std::memcpy(&amount,
                        &gItemDetailStorageCC4D58[side].amount, sizeof(amount));
                    maximum = 1000000 - amount;
                }
            }
        }
        const std::int32_t amount = world::gPlayerRuntimeSlots.valueFC;
        if (maximum > amount) {
            maximum = amount;
        }
        const std::uint8_t keyboard =
            input::gCurrentInputData == &cg::ui::detail::gInputContextCA0018 ? 2 : 0;
        result = WindowInput_HandleNumber(gCurrentWindow, 30, state, maximum, 223,
            gStorageDepositHints57EDD4.data(), keyboard, 0xFFFFFFFFu);
    } else if (index == 31) {
        const std::int32_t limit = world::gPlayerRuntimeSlots.values15BD0[static_cast<std::int32_t>(0)];
        maximum = limit - world::gPlayerRuntimeSlots.valueFC;
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        std::int32_t amount;
        std::memcpy(&amount,
            &gItemDetailStorageCC4D58[side].amount, sizeof(amount));
        if (maximum > amount) {
            maximum = amount;
        }
        const std::uint8_t keyboard =
            input::gCurrentInputData == &cg::ui::detail::gInputContextCA0018 ? 2 : 0;
        result = WindowInput_HandleNumber(gCurrentWindow, 31, state, maximum, 223,
            gStorageWithdrawHints57EDEC.data(), keyboard, 0xFFFFFFFFu);
    }
    WindowInstance* window;
    if ((result & 1u) != 0) {
        window = gCurrentWindow;
        WindowElement* const element = &window->elements[index];
        if (std::bit_cast<std::int32_t>(element->state) > maximum) {
            element->state = static_cast<std::uint32_t>(maximum);
            window = gCurrentWindow;
        }
    } else {
        window = gCurrentWindow;
    }
    if ((result & 2u) != 0) {
        if (index == 30 || index == 31) {
            std::wstring buffer;
            const std::int32_t value = std::bit_cast<std::int32_t>(window->elements[index].state);
            cg::text::FormatTo(buffer, index == 30 ? L"G|{}" : L"D|{}", value);
            const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 255);
            const std::int32_t userState =
                std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].userState.value);
            const std::int32_t value14 = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value14);
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(network::SendDialogResponse_512620(
                platform::gConnectionSocket, x, y, value14, userState, 0, escapedReply));
            window = gCurrentWindow;
            const std::int32_t confirmed = std::bit_cast<std::int32_t>(window->elements[index].state);
            const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
            gStorageLastAmountsD67B68[side] = confirmed;
        }
        window->elements[index].state = 0;
        window = gCurrentWindow;
    }
    if (index == 30) {
        cg::text::FormatTo(gStorageAmountTextsCB1DF8[0], L"{:9}", std::bit_cast<std::int32_t>(window->elements[30].state));
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        cg::text::FormatTo(gStorageAmountTextsCB1DF8[1], L"{:9}", gStorageLastAmountsD67B68[side]);
    } else if (index == 31) {
        cg::text::FormatTo(gStorageAmountTextsCB1DF8[1], L"{:9}", std::bit_cast<std::int32_t>(window->elements[31].state));
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        cg::text::FormatTo(gStorageAmountTextsCB1DF8[0], L"{:9}", gStorageLastAmountsD67B68[side]);
    }
    return 0;
}

// 原函数：0x004A9B00。
static std::int32_t WindowBank_DrawCounts(const std::int32_t, const std::int32_t)
{
    std::wstring buffer;
    std::int32_t count = 0;
    for (std::int32_t index = 8; index < 28; ++index) {
        if (world::gPlayerRuntimeSlots.items[index].used != 0) {
            ++count;
        }
    }
    cg::text::FormatTo(buffer, L"{:3}", count);
    WindowInstance* window = gCurrentWindow;
    std::int32_t y = window->y + 285;
    std::int32_t x = window->x + 357;
    WindowNumber_Draw(x, y, buffer, 3, 0, 1, 223);
    count = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        if (world::gPetRuntimeRecords[index].used != 0) {
            ++count;
        }
    }
    cg::text::FormatTo(buffer, L"{:3}", count);
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 442;
    WindowNumber_Draw(x, y, buffer, 3, 0, 1, 223);
    cg::text::FormatTo(buffer, L"{:10}", world::gPlayerRuntimeSlots.valueFC);
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 527;
    WindowNumber_Draw(x, y, buffer, 3, 0, 1, 223);
    count = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        std::uint16_t used;
        std::memcpy(&used, &gItemDetailStorageCC4D58[0].items[index], sizeof(used));
        if (used != 0) {
            ++count;
        }
    }
    cg::text::FormatTo(buffer, L"{:3}", count);
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 85;
    WindowNumber_Draw(x, y, buffer, 3, 0, 1, 223);
    count = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        std::uint16_t used;
        std::memcpy(&used, &gPetStorageD64E18[index], sizeof(used));
        if (used != 0) {
            ++count;
        }
    }
    cg::text::FormatTo(buffer, L"{:3}", count);
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 170;
    WindowNumber_Draw(x, y, buffer, 3, 0, 1, 223);
    const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
    std::int32_t amount;
    std::memcpy(&amount,
        &gItemDetailStorageCC4D58[side].amount, sizeof(amount));
    cg::text::FormatTo(buffer, L"{:10}", amount);
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 255;
    WindowNumber_Draw(x, y, buffer, 3, 0, 1, 223);
    return 0;
}

// 原函数：0x004AC200。
std::int32_t WindowBank_ButtonCloseIcon(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"关闭这个视窗。";
    }
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    graphic->imageNo = 243000;
    if ((state & 1) != 0) {
        graphic->imageNo = 243002;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243001;
    }
    return 0;
}

// 原函数：0x004A9350。
std::int32_t WindowBank_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"关闭此视窗";
    }
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    graphic->imageNo = 243048;
    if ((state & 1) != 0) {
        graphic->imageNo = 243050;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243049;
    }
    return 0;
}

// SGM01：原4A84A0完整1002条；A/G/I/P及记录失败续行、金额解析后fresh组写入保，仅A返回S32 1。
// 原函数：0x004A84A0。
std::int32_t WindowBank_ReceiveStorageData(const char* const text)
{
    char buffer[3072];
    if (network::ExtractDelimitedField_52CED0(text, '|', 1, 1023, buffer) != 0) {
        return 0;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
    if (std::strlen(buffer) != 1) {
        return 0;
    }
    const auto readItem = [&](WindowStorageItem* const record, const std::int32_t index,
                              const std::int32_t field, const bool requireDescriptionField) {
        const std::int32_t imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        if (imageNo <= 0) {
            return;
        }
        record->used = std::uint16_t{1};
        record->imageNo = imageNo;
        record->count = network::ParseDelimitedInteger_52CFA0(text, '|', field + 1);
        record->flags = network::ParseDelimitedInteger_52CFA0(text, '|', field + 2);
        if (network::ExtractDelimitedField_52CED0(text, '|', field + 3, 1023, buffer) != 0) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        std::strcpy(record->name.data(), std::strlen(buffer) > 28 ? "???" : buffer);
        if (network::ExtractDelimitedField_52CED0(text, '|', field + 4, 1023, buffer) != 0) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        if (std::strlen(buffer) != 0) {
            std::strcpy(record->memo.data(), buffer);
        }
        const std::int32_t descriptionStatus =
            network::ExtractDelimitedField_52CED0(text, '|', field + 5, 1023, buffer);
        if (requireDescriptionField && descriptionStatus != 0) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        WindowItem_BuildStorageDescription(index, 0, buffer);
    };
    const auto readPet = [&](WindowStoragePet* const record, const std::int32_t field) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field, 1023, buffer) != 0) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        std::strcpy(record->name.data(), std::strlen(buffer) > 16 ? "???" : buffer);
        const std::int32_t level = network::ParseDelimitedInteger_52CFA0(text, '|', field + 1);
        record->level = level;
        if (level < 0) {
            return;
        }
        record->used = std::uint16_t{1};
        record->tribe = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 2));
        const std::array<std::int32_t*, 9> firstFields {
            &record->maxHp, &record->maxFp, &record->combatStats[0], &record->combatStats[1], &record->combatStats[2], &record->combatStats[3], &record->combatStats[4], &record->combatValue44, &record->value58,
        };
        for (std::int32_t value = 0; value < 9; ++value) {
            *firstFields[value] = network::ParseDelimitedInteger_52CFA0(text, '|', field + 3 + value);
        }
        network::ExtractDelimitedField_52CED0(text, '|', field + 12, 1023, buffer);
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        std::strcpy(record->speciesName.data(), buffer);
        const std::array<std::int32_t*, 12> lastFields {
            &record->imageNo, &record->baseStats[0], &record->baseStats[1], &record->baseStats[2], &record->baseStats[3], &record->baseStats[4], &record->elements[0], &record->elements[1], &record->elements[2], &record->elements[3], &record->weight, &record->maxWeight,
        };
        for (std::int32_t value = 0; value < 12; ++value) {
            *lastFields[value] = network::ParseDelimitedInteger_52CFA0(text, '|', field + 13 + value);
        }
        for (std::int32_t skill = 0; skill < world::kPetTechniqueSlotCount; ++skill) {
            network::ExtractDelimitedField_52CED0(text, '|', field + 25 + skill, 1023, buffer);
            input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
            std::strcpy(record->techniques[skill].name.data(), std::strlen(buffer) > 24 ? "???" : buffer);
        }
        for (std::size_t slot = 0; slot < record->equipmentNames.size(); ++slot) {
            network::ExtractDelimitedField_52CED0(text, '|',
                field + 25 + world::kPetTechniqueSlotCount + static_cast<std::int32_t>(slot), 1023, buffer);
            input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
            std::strcpy(record->equipmentNames[slot].data(), std::strlen(buffer) > 28 ? "???" : buffer);
        }
    };
    // 已确认银行格式：25个基本字段、20个技能名、6个装备名。
    constexpr std::int32_t petFieldCount = 25 + world::kPetTechniqueSlotCount + 6;
    switch (buffer[0]) {
    case 'A': {
        std::memset(gItemDetailStorageCC4D58[0].items.data(), 0, sizeof(gItemDetailStorageCC4D58[0].items));
        for (std::int32_t index = 0; index < 20; ++index) {
            readItem(&gItemDetailStorageCC4D58[0].items[index], index, 2 + index * 6, true);
        }
        std::memset(gPetStorageD64E18.data(), 0, sizeof(gPetStorageD64E18));
        for (std::int32_t index = 0; index < 5; ++index) {
            readPet(&gPetStorageD64E18[index], 122 + index * petFieldCount);
        }
        const std::int32_t amount = network::ParseDelimitedInteger_52CFA0(text, '|', 122 + 5 * petFieldCount);
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        gItemDetailStorageCC4D58[side].amount = amount;
        return 1;
    }
    case 'G': {
        const std::int32_t amount = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        gItemDetailStorageCC4D58[side].amount = amount;
        return 0;
    }
    case 'I': {
        std::int32_t field = 2;
        std::int32_t index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        while (index >= 0) {
            auto* const record = &gItemDetailStorageCC4D58[0].items[index];
            std::memset(record, 0, 0xC44);
            readItem(record, index, field + 1, false);
            field += 7;
            index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        }
        return 0;
    }
    case 'P': {
        std::int32_t field = 2;
        std::int32_t index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        while (index >= 0) {
            auto* const record = &gPetStorageD64E18[index];
            std::memset(record, 0, sizeof(*record));
            readPet(record, field + 1);
            field += 1 + petFieldCount;
            index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        }
        return 0;
    }
    default:
        return 0;
    }
}

} // namespace detail

} // namespace cg::ui
