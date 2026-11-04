#include "cg/session/ClientSession.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/AlbumState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowAlbumListElementId : ElementId {
    ButtonClose = 1,
    Element2 = 2,
    DragScroll = 3,
    HandleScrollButtons = 4,
    HandleScrollButtons_5 = 5,
    HandleScrollButtons_6 = 6,
    HandleScrollButtons_7 = 7,
    Scroll = 8,
    Element9 = 9,
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
    SelectEntry = 59,
    SelectEntry_60 = 60,
    SelectEntry_61 = 61,
    SelectEntry_62 = 62,
    SelectEntry_63 = 63,
    SelectEntry_64 = 64,
    SelectEntry_65 = 65,
    SelectEntry_66 = 66,
    SelectEntry_67 = 67,
    SelectEntry_68 = 68,
    Element69 = 69,
    Element70 = 70,
    HandleSideArea = 71,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00492430。
static std::int32_t WindowAlbumList_SelectEntry(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 图鉴列表打开选中详情。

// 原函数：0x00492520。
static std::int32_t WindowAlbumList_HandleScrollButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 单行/十行滚动按钮。

// 原函数：0x00492800。
static std::int32_t WindowAlbumList_DragScroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // WSP01：原950参数拖动换算；旧X代码1961。

// 原函数：0x00492840。
static std::int32_t WindowAlbumList_Scroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 图鉴滚轮，恒返0。

// 原函数：0x004923F0。
static std::int32_t WindowAlbumList_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic566F18 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F18。

static WindowGraphicData gWindowGraphic566F30 {0x3BB00, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F30。

static WindowGraphicData gWindowGraphic566F48 {0x3BB01, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00566F48。

static WindowGraphicData gWindowGraphic566F60 {0x3BB03, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F60。

static WindowGraphicData gWindowGraphic566F78 {0x3B6B4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F78。

static WindowGraphicData gWindowGraphic566F90 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F90。

static WindowGraphicData gWindowGraphic566FA8 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566FA8。

static WindowGraphicData gWindowGraphic566FC0 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566FC0。

static WindowGraphicData gWindowGraphic566FD8 {0x3BB05, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566FD8。

static WindowGraphicData gWindowGraphic566FF0 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566FF0。

static WindowGraphicData gWindowGraphic567008 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567008。

static WindowTextPrototype gWindowText567020 { // 原全局：0x00567020。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"宠物名称", // 原常量参考：0x00586048。
};

static std::uint32_t gWindowMouseFlagsD677C0 = 0; // 原全局：0x00D677C0。

static WindowElementPrototype gWindowElements567028[71] { // 原全局：0x00567028。
    {WindowElementType_Graphic, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic566F18}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowAlbumListElementId::ButtonClose)},
    {WindowElementType_Graphic, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic566F90}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element2)},
    {WindowElementType_MouseArea, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD677C0}, &WindowAlbumList_DragScroll, -1, static_cast<ElementId>(WindowAlbumListElementId::DragScroll)},
    {WindowElementType_Graphic, 206, 30, 11, 11, 1, {.graphic = &gWindowGraphic566FA8}, &WindowAlbumList_HandleScrollButtons, -1, static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons)},
    {WindowElementType_Graphic, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic566FC0}, &WindowAlbumList_HandleScrollButtons, -1, static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons_5)},
    {WindowElementType_Graphic, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic566FF0}, &WindowAlbumList_HandleScrollButtons, -1, static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons_6)},
    {WindowElementType_Graphic, 170, 189, 20, 20, 1, {.graphic = &gWindowGraphic567008}, &WindowAlbumList_HandleScrollButtons, -1, static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons_7)},
    {WindowElementType_EventArea, 0, 0, 225, 220, 1, {.unusedValue = 0}, &WindowAlbumList_Scroll, -1, static_cast<ElementId>(WindowAlbumListElementId::Scroll)},
    {WindowElementType_Graphic, 19, 31, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element9)},
    {WindowElementType_Graphic, 25, 31, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element10)},
    {WindowElementType_Graphic, 31, 31, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element11)},
    {WindowElementType_Graphic, 19, 47, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element12)},
    {WindowElementType_Graphic, 25, 47, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element13)},
    {WindowElementType_Graphic, 31, 47, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element14)},
    {WindowElementType_Graphic, 19, 63, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element15)},
    {WindowElementType_Graphic, 25, 63, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element16)},
    {WindowElementType_Graphic, 31, 63, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element17)},
    {WindowElementType_Graphic, 19, 79, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element18)},
    {WindowElementType_Graphic, 25, 79, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element19)},
    {WindowElementType_Graphic, 31, 79, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element20)},
    {WindowElementType_Graphic, 19, 95, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element21)},
    {WindowElementType_Graphic, 25, 95, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element22)},
    {WindowElementType_Graphic, 31, 95, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element23)},
    {WindowElementType_Graphic, 19, 111, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element24)},
    {WindowElementType_Graphic, 25, 111, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element25)},
    {WindowElementType_Graphic, 31, 111, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element26)},
    {WindowElementType_Graphic, 19, 127, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element27)},
    {WindowElementType_Graphic, 25, 127, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element28)},
    {WindowElementType_Graphic, 31, 127, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element29)},
    {WindowElementType_Graphic, 19, 143, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element30)},
    {WindowElementType_Graphic, 25, 143, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element31)},
    {WindowElementType_Graphic, 31, 143, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element32)},
    {WindowElementType_Graphic, 19, 159, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element33)},
    {WindowElementType_Graphic, 25, 159, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element34)},
    {WindowElementType_Graphic, 31, 159, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element35)},
    {WindowElementType_Graphic, 19, 175, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element36)},
    {WindowElementType_Graphic, 25, 175, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element37)},
    {WindowElementType_Graphic, 31, 175, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element38)},
    {WindowElementType_Graphic, 47, 31, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element39)},
    {WindowElementType_Graphic, 47, 47, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element40)},
    {WindowElementType_Graphic, 47, 63, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element41)},
    {WindowElementType_Graphic, 47, 79, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element42)},
    {WindowElementType_Graphic, 47, 95, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element43)},
    {WindowElementType_Graphic, 47, 111, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element44)},
    {WindowElementType_Graphic, 47, 127, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element45)},
    {WindowElementType_Graphic, 47, 143, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element46)},
    {WindowElementType_Graphic, 47, 159, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element47)},
    {WindowElementType_Graphic, 47, 175, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element48)},
    {WindowElementType_Text, 75, 29, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element49)},
    {WindowElementType_Text, 75, 45, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element50)},
    {WindowElementType_Text, 75, 61, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element51)},
    {WindowElementType_Text, 75, 77, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element52)},
    {WindowElementType_Text, 75, 93, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element53)},
    {WindowElementType_Text, 75, 109, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element54)},
    {WindowElementType_Text, 75, 125, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element55)},
    {WindowElementType_Text, 75, 141, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element56)},
    {WindowElementType_Text, 75, 157, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element57)},
    {WindowElementType_Text, 75, 173, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element58)},
    {WindowElementType_Graphic, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry)},
    {WindowElementType_Graphic, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry_60)},
    {WindowElementType_Graphic, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry_61)},
    {WindowElementType_Graphic, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry_62)},
    {WindowElementType_Graphic, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry_63)},
    {WindowElementType_Graphic, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry_64)},
    {WindowElementType_Graphic, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry_65)},
    {WindowElementType_Graphic, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry_66)},
    {WindowElementType_Graphic, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry_67)},
    {WindowElementType_Graphic, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry, -1, static_cast<ElementId>(WindowAlbumListElementId::SelectEntry_68)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic566F30}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element69)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic566F48}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAlbumListElementId::Element70)},
    {WindowElementType_EventArea, 225, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowAlbumListElementId::HandleSideArea)},
};

// 原型：窗口19；原全局：0x0054E600。
WindowPrototype gWindowPrototypeAlbumList {0, 4, 38, 95, 225, 220, 0x80010101u, 71, gWindowElements567028, &WindowAlbumList_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 19: 54E600

// 窗口函数实现。

// 原末行分支分别返回窗口或元素指针；不截断成原x86整数。
// 原函数：0x00492C80。
static void* WindowAlbumList_Refresh(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return nullptr;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    render::GraphicDescriptor16 descriptor {};
    void* result = nullptr;
    for (std::int32_t row = 0; row < 10; ++row) {
        const world::AlbumRecord* const records = world::GetSelectedAlbumRecords(*callbackWindow->ownerSession);
        const std::int32_t scroll = business.valueC8A95C;
        const bool used = records[scroll + row].used != 0;
        const std::int32_t firstDigit = 8 + row * 3;
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(firstDigit)); indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(firstDigit + 1)); indexedElement2) indexedElement2->visible = 0;
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(firstDigit + 2)); indexedElement3) indexedElement3->visible = 0;
        if (used) {
            const std::int32_t number = scroll + row + 1;
            const std::uint32_t color = 0xFFFFFFFFu;
            descriptor.color = color;
            descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
            for (std::int32_t digit = 0; digit < 2; ++digit) {
                WindowInstance* const window = currentWindow;
                auto* elementsWindow = window;
                const std::int32_t image = 0x3B6B4
                    + (digit == 0 ? number % 1000 / 100 : number % 100 / 10);
                const std::int32_t windowY = window->y;
                const std::int32_t windowX = window->x;
                auto* const elementYElement = elementsWindow->FindElementById(NativeElementKey(firstDigit + digit));
                if (elementYElement) {
                    const std::int32_t elementY = elementYElement->y;
                    auto* const elementXElement = elementsWindow->FindElementById(NativeElementKey(firstDigit + digit));
                    if (elementXElement) {
                        const std::int32_t elementX = elementXElement->x;
                        static_cast<void>(cg::render::DrawGraphic(*callbackWindow->ownerSession,
                            elementX + windowX + 1, elementY + windowY + 4, 223, image, 0, &descriptor));
                    }
                }
            }
            WindowInstance* const window = currentWindow;
            auto* elementsWindow = window;
            const std::int32_t image = 0x3B6B4 + number % 10;
            auto* const elementYElement = elementsWindow->FindElementById(NativeElementKey(firstDigit + 2));
            if (elementYElement) {
                const std::int32_t elementY = elementYElement->y;
                const std::int32_t windowY = window->y;
                auto* const elementXElement = elementsWindow->FindElementById(NativeElementKey(firstDigit + 2));
                if (elementXElement) {
                    const std::int32_t elementX = elementXElement->x;
                    const std::int32_t windowX = window->x;
                    static_cast<void>(cg::render::DrawGraphic(*callbackWindow->ownerSession,
                        elementX + windowX + 1, elementY + windowY + 4, 223, image, 0, &descriptor));
                }
            }

            const world::AlbumRecord* const nameRecords = world::GetSelectedAlbumRecords(*callbackWindow->ownerSession);
            WindowInstance* const nameWindow = currentWindow;
            const std::int32_t nameScroll = business.valueC8A95C;
            auto* const textElement = nameWindow->FindElementById(NativeElementKey(48 + row));
            if (textElement) {
                auto* const text = textElement->data.text;
                const auto& name = nameRecords[nameScroll + row].name;
                text->text = name;
            }
            const world::AlbumRecord* const flagRecords = world::GetSelectedAlbumRecords(*callbackWindow->ownerSession);
            const std::int32_t flagScroll = business.valueC8A95C;
            const bool fresh = flagRecords[flagScroll + row].fresh != 0;
            WindowInstance* const flagWindow = currentWindow;
            if (auto* const indexedElement4 = flagWindow->FindElementById(NativeElementKey(38 + row)); indexedElement4) indexedElement4->visible = fresh ? 1 : 0;
            result = flagWindow;
        } else {
            auto* const textElement = currentWindow->FindElementById(NativeElementKey(48 + row));
            if (textElement) {
                auto* const text = textElement->data.text;
                text->text.clear();
            }
            auto* elementsWindow = currentWindow;
            if (auto* const indexedElement4 = elementsWindow->FindElementById(NativeElementKey(38 + row)); indexedElement4) indexedElement4->visible = 0;
            result = elementsWindow->FindElementById(WindowAlbumListElementId::ButtonClose);
        }
    }
    return result;
}

// 原函数：0x004923F0。
static std::int32_t WindowAlbumList_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    if (value == -1) {
        business.valueC8A95C = 0;
    }
    static_cast<void>(WindowAlbumList_Refresh(&callbackWindow));
    return 1;
}

// 原函数：0x00492430。
static std::int32_t WindowAlbumList_SelectEntry(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_Hover) != 0) {
        uiFrame.hoverText = L"开启此魔物的图鉴视窗。";
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x3BB04;
        consumed = 1;
    } else {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x3BB02;
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        const world::AlbumRecord* const records = world::GetSelectedAlbumRecords(*callbackWindow.ownerSession);
        const std::int32_t scroll = business.valueC8A95C;
        if (records[scroll + index - 58].used != 0) {
            if (WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_AlbumDetail].window == nullptr) {
                business.valueCB2200 = scroll + index - 58;
                static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_AlbumDetail, 1, 0, nullptr));
                return 1;
            }
            business.valueCB2200 = scroll + index - 58;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 56, 320));
        }
        return 1;
    }
    return consumed;
}

// 原函数：0x00492520。
static std::int32_t WindowAlbumList_HandleScrollButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const auto setImage = [elementId, currentWindow](const std::int32_t image) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = image;
    };
    std::int32_t consumed = 0;
    switch (static_cast<WindowAlbumListElementId>(elementId)) {
    case WindowAlbumListElementId::HandleScrollButtons:
    case WindowAlbumListElementId::HandleScrollButtons_5:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons) ? 0x3B594 : 0x3B597);
            consumed = 1;
        } else {
            setImage(elementId == static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons) ? 0x3B592 : 0x3B595);
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons) ? 0x3B593 : 0x3B596);
            consumed = 1;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t next = business.valueC8A95C + (elementId == static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons) ? -1 : 1);
            business.valueC8A95C = next;
            if (elementId == static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons) ? next < 0 : next > 949) {
                business.valueC8A95C = elementId == static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons) ? 0 : 950;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
            }
            WindowInstance* const window = currentWindow;
            const std::int32_t scroll = business.valueC8A95C;
            auto* const dragScrollElement = window->FindElementById(WindowAlbumListElementId::DragScroll);
            if (dragScrollElement) {
                static_cast<void>(WindowScroll_SetPosition(dragScrollElement, 950, scroll));
            }
            consumed = 1;
        }
        return consumed;
    case WindowAlbumListElementId::HandleScrollButtons_6:
    case WindowAlbumListElementId::HandleScrollButtons_7:
        setImage(elementId == static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons_6) ? ((state & MouseFlag_Hover) != 0 ? 0x3B558 : 0x3B556)
                           : ((state & MouseFlag_Hover) != 0 ? 0x3B55B : 0x3B559));
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (elementId == static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons_6)) {
                const std::int32_t next = business.valueC8A95C - 10;
                business.valueC8A95C = next;
                if (next < 0) {
                    business.valueC8A95C = 0;
                }
                WindowInstance* const window = currentWindow;
                const std::int32_t scroll = business.valueC8A95C;
                auto* const dragScrollElement = window->FindElementById(WindowAlbumListElementId::DragScroll);
                if (dragScrollElement) {
                    static_cast<void>(WindowScroll_SetPosition(dragScrollElement, 950, scroll));
                }
            } else {
                std::int32_t next = business.valueC8A95C + 10;
                business.valueC8A95C = next;
                if (next > 949) {
                    next = 950;
                    business.valueC8A95C = next;
                }
                WindowInstance* const window = currentWindow;
                auto* const dragScrollElement = window->FindElementById(WindowAlbumListElementId::DragScroll);
                if (dragScrollElement) {
                    static_cast<void>(WindowScroll_SetPosition(dragScrollElement, 950, next));
                }
            }
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowAlbumListElementId::HandleScrollButtons_6) ? 0x3B557 : 0x3B55A);
        }
        return consumed;
    default:
        return 0;
    }
}

// WSP01：旧X片段1961；结果恰1才按原950参数换算，不由此推容量。
// 原函数：0x00492800。
static std::int32_t WindowAlbumList_DragScroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t result = WindowElement_HandleVerticalScroll(callbackWindow, elementId, state);
    if (result == 1) {
        WindowInstance* const window = currentWindow;
        auto* const dragScrollElement = window->FindElementById(WindowAlbumListElementId::DragScroll);
        if (dragScrollElement) {
            business.valueC8A95C = WindowScroll_ReadPosition(dragScrollElement, 950);
        }
    }
    return result;
}

// 原函数：0x00492840。
static std::int32_t WindowAlbumList_Scroll(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::ReadSessionMouseWheelDelta(*callbackWindow.ownerSession));
        WindowInstance* const window = currentWindow;
        const std::int32_t scroll = business.valueC8A95C;
        auto* const dragScrollElement = window->FindElementById(WindowAlbumListElementId::DragScroll);
        if (dragScrollElement) {
            business.valueC8A95C = WindowScroll_ApplyWheel(dragScrollElement, scroll, 950, wheel);
        }
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
