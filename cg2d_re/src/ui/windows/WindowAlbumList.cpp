#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00492430。
static std::int32_t WindowAlbumList_SelectEntry(std::int32_t index, std::int32_t state); // 图鉴列表打开选中详情。

// 原函数：0x00492520。
static std::int32_t WindowAlbumList_HandleScrollButtons(std::int32_t index, std::int32_t state); // 单行/十行滚动按钮。

// 原函数：0x00492800。
static std::int32_t WindowAlbumList_DragScroll(std::int32_t index, std::int32_t state); // WSP01：原950参数拖动换算；旧X代码1961。

// 原函数：0x00492840。
static std::int32_t WindowAlbumList_Scroll(std::int32_t index, std::int32_t state); // 图鉴滚轮，恒返0。

// 原函数：0x004923F0。
static std::int32_t WindowAlbumList_Update(std::int32_t value);

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
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"寵物名稱", // 原常量参考：0x00586048。
};

static std::uint32_t gWindowMouseFlagsD677C0 = 0; // 原全局：0x00D677C0。

static WindowElementPrototype gWindowElements567028[71] { // 原全局：0x00567028。
    {WindowElementType_Graphic, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic566F18}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic566F90}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD677C0}, &WindowAlbumList_DragScroll},
    {WindowElementType_Graphic, 206, 30, 11, 11, 1, {.graphic = &gWindowGraphic566FA8}, &WindowAlbumList_HandleScrollButtons},
    {WindowElementType_Graphic, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic566FC0}, &WindowAlbumList_HandleScrollButtons},
    {WindowElementType_Graphic, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic566FF0}, &WindowAlbumList_HandleScrollButtons},
    {WindowElementType_Graphic, 170, 189, 20, 20, 1, {.graphic = &gWindowGraphic567008}, &WindowAlbumList_HandleScrollButtons},
    {WindowElementType_EventArea, 0, 0, 225, 220, 1, {.unusedValue = 0}, &WindowAlbumList_Scroll},
    {WindowElementType_Graphic, 19, 31, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 31, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 31, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 47, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 47, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 47, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 63, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 63, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 63, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 79, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 79, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 79, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 95, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 95, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 95, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 111, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 111, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 111, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 127, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 127, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 127, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 143, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 143, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 143, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 159, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 159, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 159, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 175, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 175, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 175, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 31, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 47, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 63, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 79, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 95, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 111, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 127, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 143, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 159, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 175, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 29, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 45, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 61, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 77, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 93, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 109, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 125, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 141, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 157, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 173, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic566F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic566F48}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 225, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口19；原全局：0x0054E600。
WindowPrototype gWindowPrototypeAlbumList {0, 4, 38, 95, 225, 220, 0x80010101u, 71, gWindowElements567028, &WindowAlbumList_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 19: 54E600

// 窗口函数实现。

// 原末行分支分别返回窗口或元素指针；不截断成原x86整数。
// 原函数：0x00492C80。
static void* WindowAlbumList_Refresh()
{
    render::GraphicDescriptor16 descriptor {};
    void* result = nullptr;
    for (std::int32_t row = 0; row < 10; ++row) {
        const std::byte* const records = world::detail::BorrowSelectedAlbumRecords();
        const std::int32_t scroll = gLoginWindowRuntime.valueC8A95C;
        const bool used = records[(scroll + row) * 0x80] != std::byte{0};
        const std::int32_t firstDigit = 8 + row * 3;
        gCurrentWindow->elements[firstDigit].visible = 0;
        gCurrentWindow->elements[firstDigit + 1].visible = 0;
        gCurrentWindow->elements[firstDigit + 2].visible = 0;
        if (used) {
            const std::int32_t number = scroll + row + 1;
            const std::uint32_t color = 0xFFFFFFFFu;
            std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
            descriptor.bytes[13] = 4;
            for (std::int32_t digit = 0; digit < 2; ++digit) {
                WindowInstance* const window = gCurrentWindow;
                WindowElement* const elements = window->elements;
                const std::int32_t image = 0x3B6B4
                    + (digit == 0 ? number % 1000 / 100 : number % 100 / 10);
                const std::int32_t windowY = window->y;
                const std::int32_t windowX = window->x;
                const std::int32_t elementY = elements[firstDigit + digit].y;
                const std::int32_t elementX = elements[firstDigit + digit].x;
                static_cast<void>(login::detail::DrawGraphic(
                    elementX + windowX + 1, elementY + windowY + 4, 223, image, 0, &descriptor));
            }
            WindowInstance* const window = gCurrentWindow;
            WindowElement* const elements = window->elements;
            const std::int32_t image = 0x3B6B4 + number % 10;
            const std::int32_t elementY = elements[firstDigit + 2].y;
            const std::int32_t windowY = window->y;
            const std::int32_t elementX = elements[firstDigit + 2].x;
            const std::int32_t windowX = window->x;
            static_cast<void>(login::detail::DrawGraphic(
                elementX + windowX + 1, elementY + windowY + 4, 223, image, 0, &descriptor));

            const std::byte* const nameRecords = world::detail::BorrowSelectedAlbumRecords();
            WindowInstance* const nameWindow = gCurrentWindow;
            const std::int32_t nameScroll = gLoginWindowRuntime.valueC8A95C;
            auto* const text = static_cast<WindowTextData*>(nameWindow->elements[48 + row].data);
            const char* const name = reinterpret_cast<const char*>(nameRecords + (nameScroll + row) * 0x80 + 0x34);
            text->text = cg::text::FromGbkWide(name);
            const std::byte* const flagRecords = world::detail::BorrowSelectedAlbumRecords();
            const std::int32_t flagScroll = gLoginWindowRuntime.valueC8A95C;
            const bool fresh = flagRecords[(flagScroll + row) * 0x80 + 0x7F] != std::byte{0};
            WindowInstance* const flagWindow = gCurrentWindow;
            flagWindow->elements[38 + row].visible = fresh ? 1 : 0;
            result = flagWindow;
        } else {
            auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[48 + row].data);
            const char* const source = input::LegacyText593850();
            text->text = cg::text::FromGbkWide(source);
            WindowElement* const elements = gCurrentWindow->elements;
            elements[38 + row].visible = 0;
            result = elements;
        }
    }
    return result;
}

// 原函数：0x004923F0。
static std::int32_t WindowAlbumList_Update(const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.valueC8A95C = 0;
    }
    static_cast<void>(WindowAlbumList_Refresh());
    return 1;
}

// 原函数：0x00492430。
static std::int32_t WindowAlbumList_SelectEntry(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"開啟此魔物的圖鑑視窗。";
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB04;
        consumed = 1;
    } else {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BB02;
    }
    if ((state & 2) != 0) {
        const std::byte* const records = world::detail::BorrowSelectedAlbumRecords();
        const std::int32_t scroll = gLoginWindowRuntime.valueC8A95C;
        if (records[(scroll + index - 58) * 0x80] != std::byte{0}) {
            if (gWindowStates[WindowId_AlbumDetail].window == nullptr) {
                gLoginWindowRuntime.valueCB2200 = scroll + index - 58;
                static_cast<void>(WindowRuntime_CreateAnimation(WindowId_AlbumDetail, 1, 0, nullptr));
                return 1;
            }
            gLoginWindowRuntime.valueCB2200 = scroll + index - 58;
            static_cast<void>(audio::PlaySe_52C450(56, 320));
        }
        return 1;
    }
    return consumed;
}

// 原函数：0x00492520。
static std::int32_t WindowAlbumList_HandleScrollButtons(const std::int32_t index, const std::int32_t state)
{
    const auto setImage = [index](const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = image;
    };
    std::int32_t consumed = 0;
    switch (index) {
    case 3:
    case 4:
        if ((state & 1) != 0) {
            setImage(index == 3 ? 0x3B594 : 0x3B597);
            consumed = 1;
        } else {
            setImage(index == 3 ? 0x3B592 : 0x3B595);
        }
        if ((state & 8) != 0) {
            setImage(index == 3 ? 0x3B593 : 0x3B596);
            consumed = 1;
        }
        if ((state & 0x400) != 0) {
            const std::int32_t next = gLoginWindowRuntime.valueC8A95C + (index == 3 ? -1 : 1);
            gLoginWindowRuntime.valueC8A95C = next;
            if (index == 3 ? next < 0 : next > 949) {
                gLoginWindowRuntime.valueC8A95C = index == 3 ? 0 : 950;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t scroll = gLoginWindowRuntime.valueC8A95C;
            static_cast<void>(WindowScroll_SetPosition(window->elements + 2, 950, scroll));
            consumed = 1;
        }
        return consumed;
    case 5:
    case 6:
        setImage(index == 5 ? ((state & 1) != 0 ? 0x3B558 : 0x3B556)
                           : ((state & 1) != 0 ? 0x3B55B : 0x3B559));
        if ((state & 2) != 0) {
            if (index == 5) {
                const std::int32_t next = gLoginWindowRuntime.valueC8A95C - 10;
                gLoginWindowRuntime.valueC8A95C = next;
                if (next < 0) {
                    gLoginWindowRuntime.valueC8A95C = 0;
                }
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t scroll = gLoginWindowRuntime.valueC8A95C;
                static_cast<void>(WindowScroll_SetPosition(window->elements + 2, 950, scroll));
            } else {
                std::int32_t next = gLoginWindowRuntime.valueC8A95C + 10;
                gLoginWindowRuntime.valueC8A95C = next;
                if (next > 949) {
                    next = 950;
                    gLoginWindowRuntime.valueC8A95C = next;
                }
                WindowInstance* const window = gCurrentWindow;
                static_cast<void>(WindowScroll_SetPosition(window->elements + 2, 950, next));
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(index == 5 ? 0x3B557 : 0x3B55A);
        }
        return consumed;
    default:
        return 0;
    }
}

// WSP01：旧X片段1961；结果恰1才按原950参数换算，不由此推容量。
// 原函数：0x00492800。
static std::int32_t WindowAlbumList_DragScroll(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t result = WindowElement_HandleVerticalScroll(index, state);
    if (result == 1) {
        WindowInstance* const window = gCurrentWindow;
        gLoginWindowRuntime.valueC8A95C = WindowScroll_ReadPosition(window->elements + 2, 950);
    }
    return result;
}

// 原函数：0x00492840。
static std::int32_t WindowAlbumList_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t scroll = gLoginWindowRuntime.valueC8A95C;
        gLoginWindowRuntime.valueC8A95C = WindowScroll_ApplyWheel(window->elements + 2, scroll, 950, wheel);
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
