#include "cg/session/ClientSession.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/ChatHistory.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowBlacklistElementId : ElementId {
    ButtonClose = 1,
    Element2 = 2,
    DragScroll = 3,
    HandleButtons = 4,
    HandleButtons_5 = 5,
    HandleButtons_6 = 6,
    HandleButtons_7 = 7,
    HandleButtons_8 = 8,
    HandleButtons_9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    Element20 = 20,
    Element21 = 21,
    Element22 = 22,
    Element23 = 23,
    Element24 = 24,
    Element25 = 25,
    Element26 = 26,
    Element27 = 27,
    Element28 = 28,
    Element29 = 29,
    Element30 = 30,
    Element31 = 31,
    Element32 = 32,
    Element33 = 33,
    Element34 = 34,
    Element35 = 35,
    Element36 = 36,
    Element37 = 37,
    Element38 = 38,
    Element39 = 39,
    Element40 = 40,
    Element41 = 41,
    Element42 = 42,
    Element43 = 43,
    Element44 = 44,
    Element45 = 45,
    Element46 = 46,
    Element47 = 47,
    Element48 = 48,
    Element49 = 49,
    Element50 = 50,
    Element51 = 51,
    Element52 = 52,
    Element53 = 53,
    Element54 = 54,
    Element55 = 55,
    Element56 = 56,
    Element57 = 57,
    Element58 = 58,
    Element59 = 59,
    EditRow = 60,
    EditRow_61 = 61,
    EditRow_62 = 62,
    EditRow_63 = 63,
    EditRow_64 = 64,
    EditRow_65 = 65,
    EditRow_66 = 66,
    EditRow_67 = 67,
    EditRow_68 = 68,
    EditRow_69 = 69,
    Element70 = 70,
    Element71 = 71,
};


namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContextC8B1B8 {}; // 原全局：0x00C8B1B8。


// 回调与内部函数前置声明。

// 原函数：0x004CCED0。
static void WindowBlacklist_Refresh(); // 窗口65十行序号/名称与活动输入刷新。

// 原函数：0x004CC6E0。
static std::int32_t WindowBlacklist_EditRow(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 删除后重读行，再决定提示或编辑。

// 原函数：0x004CC9B0。
static std::int32_t WindowBlacklist_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 六个黑名单按钮及原消费差异。

// 原函数：0x004CCE90。
static std::int32_t WindowBlacklist_DragScroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // WSP01：原90参数拖动换算；旧X代码1999。

// 原函数：0x004CD150。
static cg::yobi::YobiAction* WindowBlacklist_ConfirmClear(session::ClientSession& session); // 生产窗口47的清空确认及两个真实回调。

// 原函数：0x004CD1E0。
static std::int32_t WindowBlacklist_Clear(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState);

// 原函数：0x004CD220。
static std::int32_t WindowBlacklist_CancelClear(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState);

// 原函数：0x004CC6C0。
static std::int32_t WindowBlacklist_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowInputPrototype gBlacklistInputPrototype580EF0 { // 原全局：0x00580EF0。
    {{&cg::ui::detail::gInputContextC8B1B8}, WindowInputReferenceKind::Context},
    75, 30, 3, 9, 0, {}, 1, 20, 0, 0,
};

static WindowGraphicData gWindowGraphic57A310 {243001, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A310。

static WindowGraphicData gWindowGraphic57A328 {36813, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A328。

static WindowGraphicData gWindowGraphic57A340 {244481, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0057A340。

static WindowGraphicData gWindowGraphic57A358 {244483, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A358。

static WindowGraphicData gWindowGraphic57A370 {243380, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A370。

static WindowGraphicData gWindowGraphic57A388 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A388。

static WindowGraphicData gWindowGraphic57A3A0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3A0。

static WindowGraphicData gWindowGraphic57A3B8 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3B8。

static WindowGraphicData gWindowGraphic57A3D0 {244485, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3D0。

static WindowGraphicData gWindowGraphic57A3E8 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3E8。

static WindowGraphicData gWindowGraphic57A400 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A400。

static WindowGraphicData gWindowGraphic57A418 {36806, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A418。

static WindowGraphicData gWindowGraphic57A430 {36809, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A430。

static WindowTextPrototype gWindowText57A448 { // 原全局：0x0057A448。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"???",
};

static std::uint32_t gWindowMouseFlagsD679B8 = 0; // 原全局：0x00D679B8。

static WindowElementPrototype gWindowElements57A450[71] { // 原全局：0x0057A450。
    {WindowElementType_Graphic, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic57A310}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowBlacklistElementId::ButtonClose)},
    {WindowElementType_Graphic, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic57A388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element2)},
    {WindowElementType_MouseArea, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD679B8}, &WindowBlacklist_DragScroll, -1, static_cast<ElementId>(WindowBlacklistElementId::DragScroll)},
    {WindowElementType_Graphic, 206, 31, 11, 11, 1, {.graphic = &gWindowGraphic57A3A0}, &WindowBlacklist_HandleButtons, -1, static_cast<ElementId>(WindowBlacklistElementId::HandleButtons)},
    {WindowElementType_Graphic, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic57A3B8}, &WindowBlacklist_HandleButtons, -1, static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_5)},
    {WindowElementType_Graphic, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic57A3E8}, &WindowBlacklist_HandleButtons, -1, static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_6)},
    {WindowElementType_Graphic, 60, 189, 20, 20, 1, {.graphic = &gWindowGraphic57A400}, &WindowBlacklist_HandleButtons, -1, static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_7)},
    {WindowElementType_Graphic, 100, 189, 38, 25, 1, {.graphic = &gWindowGraphic57A418}, &WindowBlacklist_HandleButtons, -1, static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_8)},
    {WindowElementType_Graphic, 155, 189, 38, 25, 1, {.graphic = &gWindowGraphic57A430}, &WindowBlacklist_HandleButtons, -1, static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_9)},
    {WindowElementType_Graphic, 19, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element10)},
    {WindowElementType_Graphic, 25, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element11)},
    {WindowElementType_Graphic, 31, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element12)},
    {WindowElementType_Graphic, 19, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element13)},
    {WindowElementType_Graphic, 25, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element14)},
    {WindowElementType_Graphic, 31, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element15)},
    {WindowElementType_Graphic, 19, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element16)},
    {WindowElementType_Graphic, 25, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element17)},
    {WindowElementType_Graphic, 31, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element18)},
    {WindowElementType_Graphic, 19, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element19)},
    {WindowElementType_Graphic, 25, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element20)},
    {WindowElementType_Graphic, 31, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element21)},
    {WindowElementType_Graphic, 19, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element22)},
    {WindowElementType_Graphic, 25, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element23)},
    {WindowElementType_Graphic, 31, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element24)},
    {WindowElementType_Graphic, 19, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element25)},
    {WindowElementType_Graphic, 25, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element26)},
    {WindowElementType_Graphic, 31, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element27)},
    {WindowElementType_Graphic, 19, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element28)},
    {WindowElementType_Graphic, 25, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element29)},
    {WindowElementType_Graphic, 31, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element30)},
    {WindowElementType_Graphic, 19, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element31)},
    {WindowElementType_Graphic, 25, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element32)},
    {WindowElementType_Graphic, 31, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element33)},
    {WindowElementType_Graphic, 19, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element34)},
    {WindowElementType_Graphic, 25, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element35)},
    {WindowElementType_Graphic, 31, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element36)},
    {WindowElementType_Graphic, 19, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element37)},
    {WindowElementType_Graphic, 25, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element38)},
    {WindowElementType_Graphic, 31, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element39)},
    {WindowElementType_Graphic, 47, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element40)},
    {WindowElementType_Graphic, 47, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element41)},
    {WindowElementType_Graphic, 47, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element42)},
    {WindowElementType_Graphic, 47, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element43)},
    {WindowElementType_Graphic, 47, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element44)},
    {WindowElementType_Graphic, 47, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element45)},
    {WindowElementType_Graphic, 47, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element46)},
    {WindowElementType_Graphic, 47, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element47)},
    {WindowElementType_Graphic, 47, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element48)},
    {WindowElementType_Graphic, 47, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element49)},
    {WindowElementType_Text, 75, 29, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element50)},
    {WindowElementType_Text, 75, 45, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element51)},
    {WindowElementType_Text, 75, 61, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element52)},
    {WindowElementType_Text, 75, 77, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element53)},
    {WindowElementType_Text, 75, 93, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element54)},
    {WindowElementType_Text, 75, 109, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element55)},
    {WindowElementType_Text, 75, 125, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element56)},
    {WindowElementType_Text, 75, 141, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element57)},
    {WindowElementType_Text, 75, 157, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element58)},
    {WindowElementType_Text, 75, 173, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element59)},
    {WindowElementType_Graphic, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow)},
    {WindowElementType_Graphic, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow_61)},
    {WindowElementType_Graphic, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow_62)},
    {WindowElementType_Graphic, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow_63)},
    {WindowElementType_Graphic, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow_64)},
    {WindowElementType_Graphic, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow_65)},
    {WindowElementType_Graphic, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow_66)},
    {WindowElementType_Graphic, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow_67)},
    {WindowElementType_Graphic, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow_68)},
    {WindowElementType_Graphic, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow, -1, static_cast<ElementId>(WindowBlacklistElementId::EditRow_69)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic57A328}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element70)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic57A340}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBlacklistElementId::Element71)},
};

// 原型：窗口65；原全局：0x0054EE80。
WindowPrototype gWindowPrototypeBlacklist {0, 4, 38, 95, 251, 210, 0x80010101u, 71, gWindowElements57A450, &WindowBlacklist_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 65: 54EE80

// 窗口函数实现。

// 原函数：0x004CC6A0。
std::int32_t WindowBlacklist_ResetState()
{
    gLoginWindowRuntime.valueCC31A4 = 5;
    gLoginWindowRuntime.valueD2E444 = 0;
    gLoginWindowRuntime.valueCB0604 = 0;
    return 0;
}

// 原函数：0x004CCED0。
static void WindowBlacklist_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    render::GraphicDescriptor16 descriptor {};
    for (std::int32_t row = 0; row < 10; ++row) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(9 + row * 3)); indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(10 + row * 3)); indexedElement2) indexedElement2->visible = 0;
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(11 + row * 3)); indexedElement3) indexedElement3->visible = 0;
        const wchar_t* const recordAddress =
            cg::world::blacklist::gRecords[row].c_str();
        const wchar_t* text;
        WindowTextData* destination;
        // 原门禁比较静态记录地址，不是记录首字节；空记录也绘行号。
        if (recordAddress != nullptr) {
            const std::int32_t page = gLoginWindowRuntime.valueD2E444;
            const std::uint32_t color = 0xFFFFFFFFu;
            descriptor.color = color;
            descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
            const std::int32_t number = page + row + 1;
            for (std::int32_t column = 0; column < 3; ++column) {
                WindowInstance* const window = currentWindow;
                auto* elementsWindow = window;
                const std::int32_t digit = column == 0 ? (number % 1000) / 100
                    : column == 1 ? (number % 100) / 10 : number % 10;
                auto* const elementPtr = elementsWindow->FindElementById(NativeElementKey(9 + row * 3 + column));
                if (elementPtr) {
                    const WindowElement& element = (*elementPtr);
                    const std::int32_t y = element.y + window->y + 4;
                    const std::int32_t x = element.x + window->x + 1;
                    static_cast<void>(cg::render::DrawGraphic(
                        x, y, 223, digit + 0x3B6B4, 0, &descriptor));
                }
            }
            const std::int32_t currentPage = gLoginWindowRuntime.valueD2E444;
            text = cg::world::blacklist::gRecords[currentPage + row].c_str();
            auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(49 + row));
            destination = indexedElement4 ? indexedElement4->data.text : nullptr;
        } else {
            WindowInstance* const window = currentWindow;
            text = L"";
            auto* const indexedElement4 = window->FindElementById(NativeElementKey(49 + row));
            destination = indexedElement4 ? indexedElement4->data.text : nullptr;
        }
        if (destination) destination->text = text;
        if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(39 + row)); indexedElement4) indexedElement4->visible = 0;
    }
    if (input::gCurrentInputData == &cg::ui::detail::gInputContextC8B1B8) {
        WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gBlacklistInputPrototype580EF0, x, y, 1));
        render::DrawInputText_4568A0(&cg::ui::detail::gInputContextC8B1B8);
    }
}

// 原函数：0x004CC6C0。
static std::int32_t WindowBlacklist_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.valueD2E444 = 0;
    }
    WindowBlacklist_Refresh();
    return 1;
}

// 原函数：0x004CC6E0。
static std::int32_t WindowBlacklist_EditRow(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        const wchar_t* const hint = gLoginWindowRuntime.blacklistDeleteModeD67BC4 != 0
            ? L"删除此笔黑名单。" /* 原常量参考：0x00588654。 */ : L"拒绝接收此人的发话。" /* 原常量参考：0x00588668。 */;
        gHoverTextD68AE8 = hint;
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x8FCC;
        consumed = 1;
    } else {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x3BB02;
    }
    if ((state & MouseFlag_LDownEvent) == 0) {
        return consumed;
    }
    if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 1) {
        const std::int32_t recordIndex = gLoginWindowRuntime.valueD2E444 + index - 59;
        const wchar_t* const record = cg::world::blacklist::gRecords[recordIndex].c_str();
        if (record[0] != L'\0') {
            std::wstring message;
            const std::wstring name(record);
            if (cg::world::blacklist::ContainsBlacklistRecord_4013D0(name) != 0) {
                const wchar_t* const format = cg::world::blacklist::RemoveBlacklistRecord_4012F0(name) != 0
                    ? L"已从黑名单列表中移除{}" /* 原常量参考：0x0058863C。 */ : L"无法将{}从黑名单列表中移除" /* 原常量参考：0x00588620。 */;
                cg::text::FormatTo(message, format, name.c_str());
            } else {
                cg::text::FormatTo(message, L"{}不在黑名单列表中" /* 原常量参考：0x0058860C。 */, name.c_str());
            }
            world::AppendChatMessage_44B050(message, 4, 3);
        }
    }
    const std::int32_t currentIndex = gLoginWindowRuntime.valueD2E444 + index - 59;
    const wchar_t* const currentRecord = cg::world::blacklist::gRecords[currentIndex].c_str();
    if (currentRecord[0] != L'\0') {
        cg::ui::detail::gInputContextC8B1B8.reset();
        input::SetContext_44B330(&cg::ui::detail::gInputContext626AA0);
        world::AppendChatMessage_44B050(L"此栏位已有名单" /* 原常量参考：0x005885FC。 */, 4, 3);
        return 1;
    }
    WindowInstance* const window = currentWindow;
    void* const focused = callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData;
    auto* elementsWindow = window;
    auto* const buttonCloseElement = elementsWindow->FindElementById(WindowBlacklistElementId::ButtonClose);
    if (!buttonCloseElement) { return 0; }
    void* const target = buttonCloseElement->data.graphic;
    auto* const yElement = elementsWindow->FindElementById(elementId);
    if (!yElement) { return 0; }
    const std::int32_t y = yElement->y;
    const bool changeFocus = focused != target;
    gBlacklistInputPrototype580EF0.y = y + 1;
    const std::int32_t windowY = window->y;
    const std::int32_t windowX = window->x;
    static_cast<void>(WindowInput_ActivateElement(&gBlacklistInputPrototype580EF0, windowX, windowY, 0));
    input::SetContext_44B330(&cg::ui::detail::gInputContextC8B1B8);
    if (changeFocus) {
        auto* const buttonCloseElement2 = currentWindow->FindElementById(WindowBlacklistElementId::ButtonClose);
        if (buttonCloseElement2) {
            callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = buttonCloseElement2->data.graphic;
        }
    }
    return 1;
}

// 原函数：0x004CC9B0。
static std::int32_t WindowBlacklist_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    switch (static_cast<WindowBlacklistElementId>(elementId)) {
    case WindowBlacklistElementId::HandleButtons:
    case WindowBlacklistElementId::HandleButtons_5: {
        const std::int32_t base = elementId == static_cast<ElementId>(WindowBlacklistElementId::HandleButtons) ? 0x3B592 : 0x3B595;
        auto* const indexedElement = currentWindow->FindElementById(elementId);
        if (indexedElement) {
            indexedElement->data.graphic->imageNo =
                base + ((state & MouseFlag_Hover) != 0 ? 2 : 0);
        }
        if ((state & MouseFlag_Hover) != 0) {
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            if (auto* const indexedElement2 = currentWindow->FindElementById(elementId); indexedElement2) indexedElement2->data.graphic->imageNo = base + 1;
            consumed = 1;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t page = gLoginWindowRuntime.valueD2E444 + (elementId == static_cast<ElementId>(WindowBlacklistElementId::HandleButtons) ? -1 : 1);
            gLoginWindowRuntime.valueD2E444 = page;
            const bool boundary = elementId == static_cast<ElementId>(WindowBlacklistElementId::HandleButtons) ? page < 0 : page > 89;
            if (boundary) {
                gLoginWindowRuntime.valueD2E444 = elementId == static_cast<ElementId>(WindowBlacklistElementId::HandleButtons) ? 0 : 90;
            }
            static_cast<void>(audio::PlaySe_52C450(boundary ? 55 : 51, 320));
            WindowInstance* const window = currentWindow;
            const std::int32_t currentPage = gLoginWindowRuntime.valueD2E444;
            auto* const dragScrollElement = window->FindElementById(WindowBlacklistElementId::DragScroll);
            if (dragScrollElement) {
                static_cast<void>(WindowScroll_SetPosition(dragScrollElement, 90, currentPage));
            }
            consumed = 1;
        }
        break;
    }
    case WindowBlacklistElementId::HandleButtons_6:
    case WindowBlacklistElementId::HandleButtons_7: {
        const std::int32_t base = elementId == static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_6) ? 0x3B556 : 0x3B559;
        auto* const indexedElement = currentWindow->FindElementById(elementId);
        if (indexedElement) {
            indexedElement->data.graphic->imageNo =
                base + ((state & MouseFlag_Hover) != 0 ? 2 : 0);
        }
        if ((state & MouseFlag_Hover) != 0) {
            gHoverTextD68AE8 = elementId == static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_6) ? L"显示前十笔的黑名单。" /* 原常量参考：0x005886CC。 */ : L"显示后十笔的黑名单。" /* 原常量参考：0x005886B4。 */;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t page = gLoginWindowRuntime.valueD2E444 + (elementId == static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_6) ? -10 : 10);
            gLoginWindowRuntime.valueD2E444 = page;
            const bool boundary = elementId == static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_6) ? page < 0 : page > 90;
            if (boundary) {
                gLoginWindowRuntime.valueD2E444 = elementId == static_cast<ElementId>(WindowBlacklistElementId::HandleButtons_6) ? 0 : 90;
            }
            static_cast<void>(audio::PlaySe_52C450(boundary ? 55 : 51, 320));
            WindowInstance* const window = currentWindow;
            const std::int32_t currentPage = gLoginWindowRuntime.valueD2E444;
            auto* const dragScrollElement = window->FindElementById(WindowBlacklistElementId::DragScroll);
            if (dragScrollElement) {
                static_cast<void>(WindowScroll_SetPosition(dragScrollElement, 90, currentPage));
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            if (auto* const indexedElement2 = currentWindow->FindElementById(elementId); indexedElement2) indexedElement2->data.graphic->imageNo = base + 1;
        }
        break;
    }
    case WindowBlacklistElementId::HandleButtons_8:
        if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 1) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x8FC7;
            gHoverTextD68AE8 = L"删除所选择的黑名单。" /* 原常量参考：0x0058869C。 */;
        }
        if ((state & MouseFlag_Hover) != 0) {
            if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 0) {
                if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x8FC8;
            }
            gHoverTextD68AE8 = L"删除所选择的黑名单。" /* 原常量参考：0x0058869C。 */;
            consumed = 1;
        } else if (gLoginWindowRuntime.blacklistDeleteModeD67BC4 == 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x8FC6;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            gLoginWindowRuntime.blacklistDeleteModeD67BC4 =
                gLoginWindowRuntime.blacklistDeleteModeD67BC4 != 1 ? 1 : 0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        break;
    case WindowBlacklistElementId::HandleButtons_9: {
        auto* const indexedElement = currentWindow->FindElementById(elementId);
        if (indexedElement) {
            indexedElement->data.graphic->imageNo =
                (state & MouseFlag_Hover) != 0 ? 0x8FCB : 0x8FC9;
        }
        if ((state & MouseFlag_Hover) != 0) {
            gHoverTextD68AE8 = L"可一次删除所有的黑名单。" /* 原常量参考：0x00588680。 */;
            consumed = 1;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (auto* const indexedElement2 = currentWindow->FindElementById(elementId); indexedElement2) indexedElement2->data.graphic->imageNo = 0x8FCA;
            static_cast<void>(WindowBlacklist_ConfirmClear(*callbackWindow.ownerSession));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        break;
    }
    default:
        break;
    }
    return consumed;
}

// WSP01：旧X片段1999；恰1才按原90参数更新，保原X身份与返回。
// 原函数：0x004CCE90。
static std::int32_t WindowBlacklist_DragScroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t consumed = WindowElement_HandleVerticalScroll(callbackWindow, elementId, state);
    if (consumed == 1) {
        auto* const dragScrollElement = currentWindow->FindElementById(WindowBlacklistElementId::DragScroll);
        if (dragScrollElement) {
            gLoginWindowRuntime.valueD2E444 = WindowScroll_ReadPosition(dragScrollElement, 90);
        }
    }
    return consumed;
}

// 原函数：0x004CD150。
static cg::yobi::YobiAction* WindowBlacklist_ConfirmClear(session::ClientSession& session)
{
    auto& confirmation = session.UiFrame().native.confirmation;
    confirmation.visibleMaskCC4240 = 3;
    confirmation.widthUnitsCC4244 = 3;
    confirmation.valueCC424C = 0;
    confirmation.valueCC4248 = 0;
    confirmation.initializeCC4250 = nullptr;
    confirmation.actionsCC4254[5] = nullptr;
    confirmation.actionsCC4254[4] = nullptr;
    confirmation.actionsCC4254[3] = nullptr;
    confirmation.actionsCC4254[2] = nullptr;
    confirmation.actionsCC4254[0] = &WindowBlacklist_Clear;
    confirmation.actionsCC4254[1] = &WindowBlacklist_CancelClear;
    confirmation.textCC426C = L"" LR"(
)" L"  " LR"(
)" L"    确定删除" LR"(
)" L"   所有的黑名单吗？" LR"(
)" L" " LR"(
)" L" (确定删除时，请按住" LR"(
)" L" CTRL键再选择「是」)" /* 原常量参考：0x005886E4。 */;
    return WindowRuntime_CreateAnimation(session, WindowId_Confirmation, 1, 0, nullptr);
}

// 原函数：0x004CD1E0。
static std::int32_t WindowBlacklist_Clear(WindowInstance& callbackWindow, const ElementId, const std::int32_t)
{
    const wchar_t* const message = cg::world::blacklist::ClearBlacklistRecords_401470() != 0
        ? L"已删除所有的黑名单。" /* 原常量参考：0x00588754。 */ : L"黑名单并没有内容可删除。" /* 原常量参考：0x00588738。 */;
    world::AppendChatMessage_44B050(message, 4, 3);
    static_cast<void>(audio::PlaySe_52C450(78, 320));
    return 1;
}

// 原函数：0x004CD220。
static std::int32_t WindowBlacklist_CancelClear(WindowInstance& callbackWindow, const ElementId, const std::int32_t)
{
    if (WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_Blacklist].window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_Blacklist, 1, 0, nullptr));
    }
    return 1;
}

} // namespace detail

} // namespace cg::ui
