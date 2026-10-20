#include "cg/render/Font.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/ChatHistory.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowChatElementId : ElementId {
    HandleInput = 1,
    Element2 = 2,
    Scroll = 3,
    HandleButtons = 4,
    HandleButtons_5 = 5,
    HandleButtons_6 = 6,
    HandleButtons_7 = 7,
    HandleButtons_8 = 8,
    Element9 = 9,
    DragScroll = 10,
    HandleButtons_11 = 11,
    HandleButtons_12 = 12,
    HandleButtons_13 = 13,
    HandleButtons_14 = 14,
    HandleButtons_15 = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    HandleButtons_20 = 20,
};

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowChatElementId_gWindowElements570FD8 : ElementId {
    HandleInput = 1,
    Element2 = 2,
};


namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContext626AA0 {}; // 原全局：0x00626AA0。


// 回调与内部函数前置声明。

// 两个原入口只在宽度来源上不同；测量IME及取宽度仍由各入口按序执行。
// CIG01：共享两原根的现场偏移/宽度与低BYTE步长合同；见doc/cg2d_input_scroll_draw_spec.md。
static void WindowInput_ScrollAndDraw(input::InputContext* const context, const std::int32_t width);

// CFD01：聊天点击创建与35槽焦点/当前窗context分离，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A6EA0。
static std::int32_t WindowChat_HandleInput(std::int32_t unused, std::int32_t state); // 共用聊天点击，焦点取35槽、上下文写当前窗。

// 原函数：0x004A80F0。
static std::int32_t WindowChat_DragScroll(std::int32_t index, std::int32_t state); // WSP01：日志数量前后分别取样，返回滚动结果。

// CFD01：首窗口滚轮按原门槛和调用后新count发布scroll，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A81D0。
static std::int32_t WindowChat_Scroll(std::int32_t unused, std::int32_t state); // 首窗口滚轮，零日志只调整滑块Y。

// 原函数：0x004A7C40。
static void WindowChat_RefreshGeometry(); // 窗口35宽高循环与完整几何，见window_35.md。 // CWP01：见聊天几何生产完整合同。

// 原函数：0x004A8140。
static void WindowChat_SyncScrollThumb(); // 查显示中的35，按回看比例更新滑块。 // CWC01：见聊天窗口控制完整合同。

// 原函数：0x004A6FC0。
static std::int32_t WindowChat_HandleButtons(std::int32_t index, std::int32_t state); // 聊天宽高、范围、字体、颜色与翻页。 // CWC01：见聊天窗口控制完整合同。

// 原函数：0x004A7700。
static std::int32_t WindowChat_ResetScroll(); // 不清日志正文或写指针。

// 原函数：0x004A82E0。
static std::int32_t WindowChat_RefreshCompactHeight(); // 窗口57高度循环调整并清两个请求。

// 原函数：0x004A6DD0。
static std::int32_t WindowChat_Update(std::int32_t value); // CWC01：见聊天窗口控制完整合同。

// 原函数：0x004A8240。
static std::int32_t WindowCompactChat_Update(std::int32_t value);

// CFD01：窗口35/57共用C9A96C绘制门禁与原返1，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A82C0。
static std::int32_t WindowChat_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowInputPrototype gChatInputPrototype57E970 { // 原全局：0x0057E970。
    {{&cg::ui::detail::gInputContext626AA0}, WindowInputReferenceKind::Context},
    8, 5, 3, 9, 0, {}, 1, 76, 0, 0,
};

// 原全局：0x00C9A934。 原宽度调整请求，独立于实际宽度。
static std::int32_t gChatWidthRequestC9A934 = 0; // 原全局：0x00C9A934。

// 原全局：0x00C9A93C。 原高度调整请求，消费后清零。
static std::int32_t gChatHeightRequestC9A93C = 0; // 原全局：0x00C9A93C。

static WindowGraphicData gWindowGraphic570B10 {245167, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B10。

static WindowGraphicData gWindowGraphic570B28 {245160, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B28。

static WindowGraphicData gWindowGraphic570B40 {245187, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B40。

static WindowGraphicData gWindowGraphic570B58 {245172, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B58。

static WindowGraphicData gWindowGraphic570B70 {245206, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B70。

static WindowGraphicData gWindowGraphic570B88 {245192, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B88。

static WindowGraphicData gWindowGraphic570BA0 {245161, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570BA0。

static WindowGraphicData gWindowGraphic570BD0 {245171, 0, 0, 0, 0, 0x30FFFFFFu}; // 原全局：0x00570BD0。

static WindowGraphicData gWindowGraphic570BE8 {245169, 0, 0, 0, 0, 0x30FFFFFFu}; // 原全局：0x00570BE8。

static WindowGraphicData gWindowGraphic570C00 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C00。

static WindowGraphicData gWindowGraphic570C18 {245165, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C18。

static WindowGraphicData gWindowGraphic570C30 {245164, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C30。

static WindowGraphicData gWindowGraphic570C48 {245166, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C48。

static WindowGraphicData gWindowGraphic570C60 {245212, 0, 0, 0, 0, 0xA0FFFFFFu}; // 原全局：0x00570C60。

static WindowGraphicData gWindowGraphic570C78 {243091, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C78。

static WindowGraphicData gWindowGraphic570C90 {243094, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C90。

static std::uint32_t gWindowMouseFlagsD67870 = 0; // 原全局：0x00D67870。

// 原全局：0x00570CB0。 几何更新所读五按钮原型坐标。
static WindowElementPrototype gWindowElements570CB0[20] { // 原全局：0x00570CB0。
    {WindowElementType_Input, 0, 0, 0, 0, 1, {}, &WindowChat_HandleInput, -1, static_cast<ElementId>(WindowChatElementId::HandleInput)},
    {WindowElementType_InputCaret, 0, 0, 0, 0, 1, {}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowChatElementId::Element2)},
    {WindowElementType_EventArea, 0, 0, 0, 0, 1, {}, &WindowChat_Scroll, -1, static_cast<ElementId>(WindowChatElementId::Scroll)},
    {WindowElementType_Graphic, 7, 3, 18, 16, 1, {.graphic = &gWindowGraphic570BA0}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons)},
    {WindowElementType_Graphic, 27, 3, 18, 16, 1, {.graphic = &gWindowGraphic570B40}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_5)},
    {WindowElementType_Graphic, 48, 3, 18, 16, 1, {.graphic = &gWindowGraphic570B58}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_6)},
    {WindowElementType_Graphic, 68, 3, 18, 16, 1, {.graphic = &gWindowGraphic570B70}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_7)},
    {WindowElementType_Graphic, 87, 3, 18, 16, 1, {.graphic = &gWindowGraphic570B88}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_8)},
    {WindowElementType_Graphic, 0, 0, 11, 14, 1, {.graphic = &gWindowGraphic570C00}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowChatElementId::Element9)},
    {WindowElementType_MouseArea, 239, 104, 11, 100, 1, {.mouseFlags = &gWindowMouseFlagsD67870}, &WindowChat_DragScroll, -1, static_cast<ElementId>(WindowChatElementId::DragScroll)},
    {WindowElementType_Graphic, 0, 0, 14, 534, 1, {.graphic = &gWindowGraphic570C18}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_11)},
    {WindowElementType_Graphic, 0, 0, 14, 16, 1, {.graphic = &gWindowGraphic570C78}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_12)},
    {WindowElementType_Graphic, 0, 0, 14, 16, 1, {.graphic = &gWindowGraphic570C90}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_13)},
    {WindowElementType_Graphic, 0, 0, 14, 16, 1, {.graphic = &gWindowGraphic570C30}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_14)},
    {WindowElementType_Graphic, 0, 0, 14, 16, 1, {.graphic = &gWindowGraphic570C48}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_15)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic570B10}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowChatElementId::Element16)},
    {WindowElementType_Graphic, 454, 0, 0, 0, 1, {.graphic = &gWindowGraphic570B28}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowChatElementId::Element17)},
    {WindowElementType_Graphic, 0, -370, 0, 0, 1, {.graphic = &gWindowGraphic570BD0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowChatElementId::Element18)},
    {WindowElementType_Graphic, 0, -386, 0, 0, 1, {.graphic = &gWindowGraphic570BE8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowChatElementId::Element19)},
    {WindowElementType_Graphic, 1, 1, 0, 0, 1, {.graphic = &gWindowGraphic570C60}, &WindowChat_HandleButtons, -1, static_cast<ElementId>(WindowChatElementId::HandleButtons_20)},
};

static WindowElementPrototype gWindowElements570FD8[2] { // 原全局：0x00570FD8。
    {WindowElementType_Input, 0, 0, 0, 0, 1, {}, &WindowChat_HandleInput, -1, static_cast<ElementId>(WindowChatElementId_gWindowElements570FD8::HandleInput)},
    {WindowElementType_InputCaret, 0, 0, 0, 0, 1, {}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowChatElementId_gWindowElements570FD8::Element2)},
};

// 原型：窗口35；原全局：0x0054EAB0。
WindowPrototype gWindowPrototypeChat {0, 4, 364, 428, 640, 25, 0x80010101u, 20, gWindowElements570CB0, &WindowChat_Update, &WindowChat_Draw, &WindowRuntime_ReleaseDefault}; // 35: 54EAB0

// 原型：窗口57；原全局：0x0054EAE0。
WindowPrototype gWindowPrototypeCompactChat {0, 4, 0, 428, 0, 0, 0x80010101u, 2, gWindowElements570FD8, &WindowCompactChat_Update, &WindowChat_Draw, &WindowRuntime_ReleaseDefault}; // 57: 54EAE0

// 窗口函数实现。

// 两个原入口只在宽度来源上不同；测量IME及取宽度仍由各入口按序执行。
// CIG01：共享两原根的现场偏移/宽度与低BYTE步长合同；见doc/cg2d_input_scroll_draw_spec.md。
static void WindowInput_ScrollAndDraw(input::InputContext* const context, const std::int32_t width)
{
    const std::wstring contents = input::ContextText(*context);
    const std::size_t caret = input::ContextUtf16Offset(*context, context->caretByteOffset);
    std::size_t offset = input::ContextUtf16Offset(*context, context->field158);
    context->field154 = width;
    if (caret < offset) { offset = caret; }
    else {
        while (offset < caret && render::Font::measure(contents.c_str() + offset, context->field13C,
            static_cast<std::int32_t>(caret - offset)) > context->field154) { offset = cg::text::Next(contents, offset); }
    }
    input::SetVisibleUtf16Offset(*context, offset);
    render::DrawInputText_4568A0(context);
}

// 原5FB7F4是最后一条输入上下文的尾槽，不是日志511的font。
// CLD01：索引0借第24项输入field158，保原前项字体别名；见doc/cg2d_chat_log_line_drawing_spec.md。
static std::int32_t& WindowChat_PreviousFont(const std::int32_t index)
{
    if (index == 0) {
        return cg::ui::detail::gInputContexts5F9758[23].field158;
    }
    return world::gRecords5FB7F8[index - 1].fontType;
}

// 原函数：0x004A6D80。
std::int32_t WindowChat_ResetState()
{
    platform::gChatRange_554640 = 1;
    gChatScrollOffsetC9A940 = 0;
    world::gChatRecordCount_C9A944 = 0;
    gChatWidthRequestC9A934 = 0;
    gChatHeightRequestC9A93C = 0;
    cg::ui::detail::gInputContext626AA0.field140 = 0;
    return 0;
}

// 原函数：0x004A6DB0。
std::int32_t WindowChat_ResetScrollRequests()
{
    gChatScrollOffsetC9A940 = 0;
    gChatWidthRequestC9A934 = 0;
    world::gChatRecordCount_C9A944 = 0;
    gChatHeightRequestC9A93C = 0;
    return 0;
}

// 原函数：0x004A8330。
WindowInstance* WindowChat_SwitchMode()
{
    WindowInstance* result;
    if (gChatWindowModeD66E04 == 0) {
        WindowInstance* const window = gWindowStates[WindowId_Chat].window;
        if (window == nullptr) {
            return nullptr;
        }
        window->state |= 0x8000u;
        result = WindowRuntime_Open(WindowId_CompactChat);
        gChatWindowModeD66E04 = 1;
    } else {
        WindowInstance* const window = gWindowStates[WindowId_CompactChat].window;
        if (window == nullptr) {
            return nullptr;
        }
        window->state |= 0x8000u;
        result = WindowRuntime_Open(WindowId_Chat);
        gChatWindowModeD66E04 = 0;
    }
    input::gChatLogSuppressed_626C14 = 0;
    input::gChatPanelVisible_C9A96C = 1;
    return result;
}

// CWC01：持有实际窗口/新count与elements，原浮点比例/低S32位置和正常分支保持。
// 原函数：0x004A8140。
static void WindowChat_SyncScrollThumb()
{
    WindowInstance* const window = WindowRuntime_FindById(WindowId_Chat);
    if (window == nullptr) {
        return;
    }
    const std::int32_t scroll = gChatScrollOffsetC9A940;
    auto* beforeWindow = window;
    const std::int32_t count = world::gChatRecordCount_C9A944;
    auto* const dragScrollElement = beforeWindow ? beforeWindow->FindElementById(WindowChatElementId::DragScroll) : nullptr;
    if (dragScrollElement) {
        static_cast<void>(WindowScroll_SetPosition(dragScrollElement, count, scroll));
    }
    const std::int32_t currentCount = world::gChatRecordCount_C9A944;
    auto* elementsWindow = window;
    if (currentCount == 0) {
        if (auto* const element9 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element9) : nullptr, * const handleButtons_15Element = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::HandleButtons_15) : nullptr; element9 && handleButtons_15Element) element9->y = handleButtons_15Element->y - 14;
        return;
    }
    const double currentScroll = gChatScrollOffsetC9A940;
    auto* const handleButtons_11Element = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::HandleButtons_11) : nullptr;
    if (!handleButtons_11Element) { return; }
    const std::int32_t span = handleButtons_11Element->height - 14;
    const double ratio = currentScroll / world::gChatRecordCount_C9A944;
    const double pixels = ratio * span;
    // 原PC53除法/乘法及__ftol取低S32，不能改为整数先乘后除。
    const auto position = static_cast<std::int32_t>(static_cast<std::int64_t>(pixels));
    if (auto* const element9 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element9) : nullptr, * const handleButtons_15Element = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::HandleButtons_15) : nullptr; element9 && handleButtons_15Element) element9->y = handleButtons_15Element->y - position - 14;
}

// CWP01：原请求/字体高度各点顺序取样，尺寸环/现场窗口载荷及完整几何保持。
// 原函数：0x004A7C40。
static void WindowChat_RefreshGeometry()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    const std::int32_t previousHeight = gChatHeightUnitsC9A938;
    const std::int32_t heightRequest = gChatHeightRequestC9A93C;
    const std::int32_t height = previousHeight + heightRequest;
    gChatHeightUnitsC9A938 = height;
    if (height > 26) {
        gChatHeightUnitsC9A938 = 4;
    } else if (height < 4) {
        gChatHeightUnitsC9A938 = 26;
    }
    const std::int32_t previousWidth = gChatWidthUnitsC9A930;
    const std::int32_t widthRequest = gChatWidthRequestC9A934;
    const std::int32_t width = previousWidth + widthRequest;
    gChatHeightRequestC9A93C = 0;
    gChatWidthUnitsC9A930 = width;
    if (width > 37) {
        const std::int32_t fontWidth = render::GetFontMetrics(9).doubleWidth;
        WindowInstance* const window = gWindowStates[WindowId_Chat].window;
        gChatWidthUnitsC9A930 = 12;
        window->x += 26 * fontWidth;
    }
    if (gChatWidthUnitsC9A930 < 12) {
        const std::int32_t fontWidth = render::GetFontMetrics(9).doubleWidth;
        WindowInstance* const window = gWindowStates[WindowId_Chat].window;
        gChatWidthUnitsC9A930 = 37;
        window->x -= 26 * fontWidth;
    }
    if (gChatWidthRequestC9A934 == 1) {
        WindowInstance* const window = gWindowStates[WindowId_Chat].window;
        window->x -= render::GetFontMetrics(9).doubleWidth;
    }
    if (gChatWidthRequestC9A934 == -1) {
        WindowInstance* const window = gWindowStates[WindowId_Chat].window;
        window->x += render::GetFontMetrics(9).doubleWidth;
    }
    const std::int32_t currentWidth = gChatWidthUnitsC9A930;
    WindowInstance* const firstWindow = currentWindow;
    gChatWidthRequestC9A934 = 0;
    auto* firstElementsWindow = firstWindow;
    const std::int32_t inputWidth = (currentWidth + 1) * render::GetFontMetrics(9).doubleWidth;
    auto* const element16 = firstElementsWindow->FindElementById(WindowChatElementId::Element16);
    const std::int32_t dragWidth = inputWidth + 111;
    if (element16) {
        auto* const graphic = element16->data.graphic;
        graphic->width = inputWidth;
        graphic->height = 25;
    }
    auto* const handleButtons_20Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_20);
    if (handleButtons_20Element) {
        auto* const graphic = handleButtons_20Element->data.graphic;
        graphic->width = inputWidth + 20;
        graphic->height = 25;
    }
    if (auto* const element17 = currentWindow->FindElementById(WindowChatElementId::Element17); element17) element17->x = inputWidth;
    auto* const element18 = currentWindow->FindElementById(WindowChatElementId::Element18);
    const std::int32_t backgroundRows = gChatHeightUnitsC9A938 - 1;
    const std::int32_t backgroundHeight =
        backgroundRows * render::GetFontMetrics(9).doubleWidth;
    if (element18) {
        auto* const graphic = element18->data.graphic;
        graphic->width = dragWidth;
        graphic->height = backgroundHeight;
        element18->y = -backgroundHeight;
    }
    auto* elementsWindow = currentWindow;
    const std::int32_t backgroundWidth = gChatWidthUnitsC9A930;
    auto* const element19 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element19) : nullptr;
    if (element19) {
        auto* const graphic = element19->data.graphic;
        graphic->width = backgroundWidth * 12 + 111;
        graphic->height = gChatHeightUnitsC9A938 == 0 ? 0 : 12;
    }
    elementsWindow = currentWindow;
    if (auto* const element192 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element19) : nullptr, * const element183 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element18) : nullptr; element192 && element183) element192->y = element183->y - 12;
    elementsWindow = currentWindow;
    if (auto* const scrollElement = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Scroll) : nullptr, * const element193 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element19) : nullptr; scrollElement && element193) scrollElement->x = element193->x;
    elementsWindow = currentWindow;
    if (auto* const scrollElement2 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Scroll) : nullptr, * const element194 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element19) : nullptr; scrollElement2 && element194) scrollElement2->y = element194->y;
    if (auto* const scrollElement3 = currentWindow->FindElementById(WindowChatElementId::Scroll); scrollElement3) scrollElement3->width = inputWidth + 125;
    const std::int32_t rows = gChatHeightUnitsC9A938 - 1;
    WindowInstance* const hitWindow = currentWindow;
    const std::int32_t hitHeight = rows * render::GetFontMetrics(9).doubleWidth;
    const std::int32_t scrollX = inputWidth + 97;
    if (auto* const scrollElement4 = hitWindow->FindElementById(WindowChatElementId::Scroll); scrollElement4) scrollElement4->height = hitHeight + 16;
    if (auto* const dragScrollElement = currentWindow->FindElementById(WindowChatElementId::DragScroll); dragScrollElement) dragScrollElement->x = scrollX;
    const std::int32_t scrollFontWidth = render::GetFontMetrics(9).doubleWidth;
    const std::int32_t scrollY = 17 - scrollFontWidth * gChatHeightUnitsC9A938;
    if (auto* const dragScrollElement2 = currentWindow->FindElementById(WindowChatElementId::DragScroll); dragScrollElement2) dragScrollElement2->y = scrollY;
    if (auto* const dragScrollElement3 = currentWindow->FindElementById(WindowChatElementId::DragScroll); dragScrollElement3) dragScrollElement3->width = 14;
    const std::int32_t scrollHeightFontWidth = render::GetFontMetrics(9).doubleWidth;
    const std::int32_t scrollHeight =
        scrollHeightFontWidth * gChatHeightUnitsC9A938 - 28;
    if (auto* const dragScrollElement4 = currentWindow->FindElementById(WindowChatElementId::DragScroll); dragScrollElement4) dragScrollElement4->height = scrollHeight;
    if (auto* const handleButtons_11Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_11); handleButtons_11Element) handleButtons_11Element->x = scrollX;
    const std::int32_t trackYFontWidth = render::GetFontMetrics(9).doubleWidth;
    const std::int32_t trackY = 17 - trackYFontWidth * gChatHeightUnitsC9A938;
    if (auto* const handleButtons_11Element2 = currentWindow->FindElementById(WindowChatElementId::HandleButtons_11); handleButtons_11Element2) handleButtons_11Element2->y = trackY;
    if (auto* const handleButtons_11Element3 = currentWindow->FindElementById(WindowChatElementId::HandleButtons_11); handleButtons_11Element3) handleButtons_11Element3->width = 14;
    const std::int32_t trackHeightFontWidth = render::GetFontMetrics(9).doubleWidth;
    const std::int32_t trackHeight =
        trackHeightFontWidth * gChatHeightUnitsC9A938 - 28;
    if (auto* const handleButtons_11Element4 = currentWindow->FindElementById(WindowChatElementId::HandleButtons_11); handleButtons_11Element4) handleButtons_11Element4->height = trackHeight;
    if (auto* const handleButtons_11Element5 = currentWindow->FindElementById(WindowChatElementId::HandleButtons_11); handleButtons_11Element5) handleButtons_11Element5->data.graphic->width = 14;
    const std::int32_t trackGraphicFontWidth = render::GetFontMetrics(9).doubleWidth;
    const std::int32_t trackGraphicHeight =
        trackGraphicFontWidth * gChatHeightUnitsC9A938 - 28;
    if (auto* const handleButtons_11Element6 = currentWindow->FindElementById(WindowChatElementId::HandleButtons_11); handleButtons_11Element6) handleButtons_11Element6->data.graphic->height = trackGraphicHeight;
    if (auto* const handleButtons_12Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_12); handleButtons_12Element) handleButtons_12Element->x = scrollX;
    const std::int32_t upFontWidth = render::GetFontMetrics(9).doubleWidth;
    const std::int32_t upY = 4 - upFontWidth * gChatHeightUnitsC9A938;
    if (auto* const handleButtons_12Element2 = currentWindow->FindElementById(WindowChatElementId::HandleButtons_12); handleButtons_12Element2) handleButtons_12Element2->y = upY;
    if (auto* const handleButtons_13Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_13); handleButtons_13Element) handleButtons_13Element->x = scrollX;
    if (auto* const handleButtons_13Element2 = currentWindow->FindElementById(WindowChatElementId::HandleButtons_13); handleButtons_13Element2) handleButtons_13Element2->y = -12;
    if (auto* const handleButtons_14Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_14); handleButtons_14Element) handleButtons_14Element->x = scrollX;
    const std::int32_t upHitFontWidth = render::GetFontMetrics(9).doubleWidth;
    const std::int32_t upHitY = -upHitFontWidth * gChatHeightUnitsC9A938;
    if (auto* const handleButtons_14Element2 = currentWindow->FindElementById(WindowChatElementId::HandleButtons_14); handleButtons_14Element2) handleButtons_14Element2->y = upHitY;
    if (auto* const handleButtons_15Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_15); handleButtons_15Element) handleButtons_15Element->x = scrollX;
    if (auto* const handleButtons_15Element2 = currentWindow->FindElementById(WindowChatElementId::HandleButtons_15); handleButtons_15Element2) handleButtons_15Element2->y = -11;
    if (auto* const element9 = currentWindow->FindElementById(WindowChatElementId::Element9); element9) element9->x = scrollX;
    WindowChat_SyncScrollThumb();
    currentWindow->dragRects40[0].width = dragWidth;
    const std::int32_t finalWidth = gChatWidthUnitsC9A930;
    currentWindow->width = finalWidth * 12 + 123;
    if (auto* const handleInputElement = currentWindow->FindElementById(WindowChatElementId::HandleInput); handleInputElement) handleInputElement->width = inputWidth;
    if (auto* const handleInputElement2 = currentWindow->FindElementById(WindowChatElementId::HandleInput); handleInputElement2) handleInputElement2->height = 25;
    elementsWindow = currentWindow;
    const std::int32_t button3X = gWindowElements570CB0[3].x;
    if (auto* const handleButtonsElement = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::HandleButtons) : nullptr, * const element172 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element17) : nullptr; handleButtonsElement && element172) handleButtonsElement->x = element172->x + button3X;
    WindowInstance* window = currentWindow;
    const std::int32_t button4X = gWindowElements570CB0[4].x;
    elementsWindow = window;
    if (auto* const handleButtons_5Element = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::HandleButtons_5) : nullptr, * const element173 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element17) : nullptr; handleButtons_5Element && element173) handleButtons_5Element->x = element173->x + button4X;
    window = currentWindow;
    const std::int32_t button5X = gWindowElements570CB0[5].x;
    elementsWindow = window;
    if (auto* const handleButtons_6Element = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::HandleButtons_6) : nullptr, * const element174 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element17) : nullptr; handleButtons_6Element && element174) handleButtons_6Element->x = element174->x + button5X;
    window = currentWindow;
    const std::int32_t button6X = gWindowElements570CB0[6].x;
    elementsWindow = window;
    if (auto* const handleButtons_7Element = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::HandleButtons_7) : nullptr, * const element175 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element17) : nullptr; handleButtons_7Element && element175) handleButtons_7Element->x = element175->x + button6X;
    elementsWindow = currentWindow;
    const std::int32_t button7X = gWindowElements570CB0[7].x;
    if (auto* const handleButtons_8Element = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::HandleButtons_8) : nullptr, * const element176 = elementsWindow ? elementsWindow->FindElementById(WindowChatElementId::Element17) : nullptr; handleButtons_8Element && element176) handleButtons_8Element->x = element176->x + button7X;
    window = currentWindow;
    const std::int32_t color = cg::ui::detail::gInputContext626AA0.field140;
    if (auto* const handleButtons_8Element2 = window ? window->FindElementById(WindowChatElementId::HandleButtons_8) : nullptr; handleButtons_8Element2) handleButtons_8Element2->data.graphic->imageNo = color + 245192;
}

// CWC01：原输入创建/可见性/焦点与context接线、现场坐标和返1保持。
// 原函数：0x004A6DD0。
static std::int32_t WindowChat_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        WindowInstance* const window = currentWindow;
        // R02：仅当前位置仍在适配后的默认位置±10内时贴右，保留拖动后的位置。
        const std::int32_t adaptedX = platform::gWindowWidth - (gChatWidthUnitsC9A930 * 12 + 123) - 9;
        const std::int32_t adaptedY = gWindowPrototypes57C130[WindowId_Chat]->defaultY;
        const std::int64_t offsetX = static_cast<std::int64_t>(window->x) - adaptedX;
        const std::int64_t offsetY = static_cast<std::int64_t>(window->y) - adaptedY;
        if (offsetX >= -10 && offsetX <= 10 && offsetY >= -10 && offsetY <= 10) {
            window->x = adaptedX;
        }
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gChatInputPrototype57E970, x, y, 3));
        WindowInstance* const current = currentWindow;
        auto* const handleInputElement = current->FindElementById(WindowChatElementId::HandleInput);
        if (handleInputElement) {
            gFocusedElementDataD45F94 = handleInputElement->data.input;
        }
        auto* const handleInputElement2 = current->FindElementById(WindowChatElementId::HandleInput);
        if (handleInputElement2) {
            handleInputElement2->data.input->context =
                &cg::ui::detail::gInputContext626AA0;
        }
        input::SetContext_44B330(&cg::ui::detail::gInputContext626AA0);
        return 1;
    }
    if (render::HasLegacy3DDevice_44B610() == 1) {
        if (auto* const element19 = currentWindow->FindElementById(WindowChatElementId::Element19); element19) element19->visible = 1;
        if (auto* const element18 = currentWindow->FindElementById(WindowChatElementId::Element18); element18) element18->visible = 1;
    } else {
        if (auto* const element19 = currentWindow->FindElementById(WindowChatElementId::Element19); element19) element19->visible = 0;
        if (auto* const element18 = currentWindow->FindElementById(WindowChatElementId::Element18); element18) element18->visible = 0;
    }
    WindowChat_RefreshGeometry();
    WindowInstance* const window = currentWindow;
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    static_cast<void>(WindowInput_ActivateElement(&gChatInputPrototype57E970, x, y, 2));
    return 1;
}

// CWC01：原三个hover窗口/元素及scroll→count取样，按钮/SE/发布和返回序保持。
// 原函数：0x004A6FC0。
static std::int32_t WindowChat_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (input::gChatPanelVisible_C9A96C == 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    switch (index) {
    case 4:
        if ((state & MouseFlag_Hover) != 0) {
            if (auto* const handleButtons_5Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_5); handleButtons_5Element) handleButtons_5Element->data.graphic->imageNo = 0x3BDC3;
            gHoverTextD68AE8 = L"点击滑鼠左键增加文字显示行数，点击滑鼠右键减少文字讯息显示行数。";
            consumed = 1;
        } else {
            if (auto* const handleButtons_5Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_5); handleButtons_5Element) handleButtons_5Element->data.graphic->imageNo = 0x3BDC4;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            WindowChat_RequestHeightIncrease();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_RRepeatDown) != 0) {
            WindowChat_RequestHeightDecrease();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        break;
    case 5:
        if ((state & MouseFlag_Hover) != 0) {
            const std::int32_t image = platform::gChatRange_554640 * 3 + 0x3BDB1;
            if (auto* const handleButtons_6Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_6); handleButtons_6Element) handleButtons_6Element->data.graphic->imageNo = image;
            gHoverTextD68AE8 = L"切换发话的范围。";
            consumed = 1;
        } else {
            WindowInstance* const window = currentWindow;
            const std::int32_t range = platform::gChatRange_554640;
            if (auto* const handleButtons_6Element = window->FindElementById(WindowChatElementId::HandleButtons_6); handleButtons_6Element) handleButtons_6Element->data.graphic->imageNo = range * 3 + 0x3BDB2;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t range = platform::gChatRange_554640 + 1;
            platform::gChatRange_554640 = range;
            if (range > 5) {
                platform::gChatRange_554640 = 1;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            world::gChatRangeOverlayFrames_626BFC = 180;
            consumed = 1;
        }
        if ((state & MouseFlag_RDownEvent) != 0) {
            const std::int32_t range = platform::gChatRange_554640 - 1;
            platform::gChatRange_554640 = range;
            if (range < 1) {
                platform::gChatRange_554640 = 5;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            world::gChatRangeOverlayFrames_626BFC = 180;
            consumed = 1;
        }
        break;
    case 6: {
        WindowInstance* const hoverWindow = currentWindow;
        const bool hovered = (state & MouseFlag_Hover) != 0;
        auto* const handleButtons_7Element = hoverWindow->FindElementById(WindowChatElementId::HandleButtons_7);
        if (!handleButtons_7Element) { return 0; }
        auto* const graphic = handleButtons_7Element->data.graphic;
        if (hovered) {
            gHoverTextD68AE8 = L"改变聊天时的文字大小。";
            consumed = 1;
            switch (platform::gChatTextSizeMode_73ECBC) {
            case 0: graphic->imageNo = 0x3BDD5; break;
            case 1: graphic->imageNo = 0x3BDDB; break;
            case 2: graphic->imageNo = 0x3BDD8; break;
            default: break;
            }
        } else {
            switch (platform::gChatTextSizeMode_73ECBC) {
            case 0: graphic->imageNo = 0x3BDD4; break;
            case 1: graphic->imageNo = 0x3BDDA; break;
            case 2: graphic->imageNo = 0x3BDD7; break;
            default: break;
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            static_cast<void>(WindowRuntime_CycleNameMode());
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_RDownEvent) != 0) {
            const std::int32_t mode = platform::gChatTextSizeMode_73ECBC + 1;
            platform::gChatTextSizeMode_73ECBC = mode;
            if (mode > 2) {
                platform::gChatTextSizeMode_73ECBC = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        break;
    }
    case 3: {
        WindowInstance* const hoverWindow = currentWindow;
        const bool hovered = (state & MouseFlag_Hover) != 0;
        auto* hoverElementsWindow = hoverWindow;
        if (hovered) {
            if (auto* const handleButtonsElement = hoverElementsWindow->FindElementById(WindowChatElementId::HandleButtons); handleButtonsElement) handleButtonsElement->data.graphic->imageNo = 0x3BDA9;
            gHoverTextD68AE8 = L"点击滑鼠左键增加文字显示字数，点击滑鼠右键减少文字讯息显示字数。";
            consumed = 1;
        } else {
            if (auto* const handleButtonsElement = hoverElementsWindow->FindElementById(WindowChatElementId::HandleButtons); handleButtonsElement) handleButtonsElement->data.graphic->imageNo = 0x3BDAA;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            WindowChat_RequestWidthIncrease();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_RRepeatDown) != 0) {
            WindowChat_RequestWidthDecrease();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        break;
    }
    case 7: {
        if ((state & MouseFlag_Hover) != 0) {
            gHoverTextD68AE8 = L"变更文字的颜色。";
            consumed = 1;
        }
        std::int32_t color;
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t next = cg::ui::detail::gInputContext626AA0.field140 + 1;
            cg::ui::detail::gInputContext626AA0.field140 = next;
            if (next == 4) {
                cg::ui::detail::gInputContext626AA0.field140 = 5;
            } else if (next > 9) {
                cg::ui::detail::gInputContext626AA0.field140 = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            color = cg::ui::detail::gInputContext626AA0.field140;
            world::gInputContext626840.field140 = color;
            consumed = 1;
        } else {
            color = cg::ui::detail::gInputContext626AA0.field140;
        }
        if ((state & MouseFlag_RDownEvent) != 0) {
            --color;
            cg::ui::detail::gInputContext626AA0.field140 = color;
            if (color == 4) {
                cg::ui::detail::gInputContext626AA0.field140 = 3;
            } else if (color < 0) {
                cg::ui::detail::gInputContext626AA0.field140 = 9;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            world::gInputContext626840.field140 = cg::ui::detail::gInputContext626AA0.field140;
            consumed = 1;
        }
        break;
    }
    case 13: {
        WindowInstance* const hoverWindow = currentWindow;
        const bool hovered = (state & MouseFlag_Hover) != 0;
        auto* const handleButtons_12Element = hoverWindow->FindElementById(WindowChatElementId::HandleButtons_12);
        if (handleButtons_12Element) {
            handleButtons_12Element->data.graphic->imageNo =
                hovered ? 0x3B594 : 0x3B592;
        }
        consumed = hovered ? 1 : 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t previousScroll = gChatScrollOffsetC9A940;
            const std::int32_t count = world::gChatRecordCount_C9A944;
            const std::int32_t scroll = previousScroll + 1;
            gChatScrollOffsetC9A940 = scroll;
            if (scroll > count) {
                WindowInstance* const window = currentWindow;
                gChatScrollOffsetC9A940 = count;
                if (auto* const handleButtons_12Element2 = window->FindElementById(WindowChatElementId::HandleButtons_12); handleButtons_12Element2) handleButtons_12Element2->data.graphic->imageNo = 0x3B593;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            WindowChat_SyncScrollThumb();
            consumed = 1;
        }
        break;
    }
    case 14: {
        auto* const handleButtons_13Element = currentWindow->FindElementById(WindowChatElementId::HandleButtons_13);
        if (handleButtons_13Element) {
            handleButtons_13Element->data.graphic->imageNo =
                (state & MouseFlag_Hover) != 0 ? 0x3B597 : 0x3B595;
        }
        consumed = (state & MouseFlag_Hover) != 0 ? 1 : 0;
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t scroll = gChatScrollOffsetC9A940 - 1;
            gChatScrollOffsetC9A940 = scroll;
            if (scroll < 0) {
                WindowInstance* const window = currentWindow;
                gChatScrollOffsetC9A940 = 0;
                if (auto* const handleButtons_13Element2 = window->FindElementById(WindowChatElementId::HandleButtons_13); handleButtons_13Element2) handleButtons_13Element2->data.graphic->imageNo = 0x3B596;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            WindowChat_SyncScrollThumb();
            consumed = 1;
        }
        break;
    }
    case 10:
        consumed = (state & MouseFlag_Hover) != 0 ? 1 : 0;
        break;
    default:
        break;
    }
    return consumed;
}

// CFD01：聊天点击创建与35槽焦点/当前窗context分离，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A6EA0。
static std::int32_t WindowChat_HandleInput(const std::int32_t, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (input::gChatPanelVisible_C9A96C == 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        if (WindowInput_ActivateElement(&gChatInputPrototype57E970, x, y, 3) == 0) {
            for (;;) {
                WindowInstance* const current = currentWindow;
                gChatInputPrototype57E970.input.context = &cg::ui::detail::gInputContext626AA0;
                gChatInputPrototype57E970.input.kind = WindowInputReferenceKind::Context;
                gChatInputPrototype57E970.x = 8;
                cg::ui::detail::gInputContext626AA0 = world::gInputContext626840;
                gChatInputPrototype57E970.y = 5;
                gChatInputPrototype57E970.renderMode = 3;
                gChatInputPrototype57E970.fontType = 9;
                gChatInputPrototype57E970.color = 0;
                gChatInputPrototype57E970.maxLine = 1;
                gChatInputPrototype57E970.maxCharPerLine = 76;
                gChatInputPrototype57E970.value224 = 0;
                gChatInputPrototype57E970.encrypt = 0;
                const std::int32_t currentY = current->y;
                const std::int32_t currentX = current->x;
                if (WindowInput_ActivateElement(&gChatInputPrototype57E970, currentX, currentY, 3) != 0) {
                    break;
                }
            }
        }
        auto* focusedElementsWindow = gWindowStates[WindowId_Chat].window;
        WindowInstance* const current = currentWindow;
        auto* const handleInputElement = focusedElementsWindow ? focusedElementsWindow->FindElementById(WindowChatElementId::HandleInput) : nullptr;
        if (handleInputElement) {
            gFocusedElementDataD45F94 = handleInputElement->data.input;
        }
        auto* const handleInputElement2 = current->FindElementById(WindowChatElementId::HandleInput);
        if (handleInputElement2) {
            handleInputElement2->data.input->context =
                &cg::ui::detail::gInputContext626AA0;
        }
        input::SetContext_44B330(&cg::ui::detail::gInputContext626AA0);
        consumed = 1;
    }
    if (input::gCurrentInputData != &cg::ui::detail::gInputContext626AA0) {
        WindowChat_DrawInput(&cg::ui::detail::gInputContext626AA0);
    }
    return consumed;
}

// 原函数：0x004A8240。
static std::int32_t WindowCompactChat_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    if (value == -1) {
        static_cast<void>(WindowInput_ActivateElement(&gChatInputPrototype57E970, x, y, 3));
        WindowInstance* const current = currentWindow;
        auto* const handleInputElement = current->FindElementById(WindowChatElementId::HandleInput);
        if (handleInputElement) {
            gFocusedElementDataD45F94 = handleInputElement->data.input;
        }
        auto* const handleInputElement2 = current->FindElementById(WindowChatElementId::HandleInput);
        if (handleInputElement2) {
            handleInputElement2->data.input->context =
                &cg::ui::detail::gInputContext626AA0;
        }
        input::SetContext_44B330(&cg::ui::detail::gInputContext626AA0);
    } else {
        static_cast<void>(WindowInput_ActivateElement(&gChatInputPrototype57E970, x, y, 2));
        static_cast<void>(WindowChat_RefreshCompactHeight());
    }
    return 1;
}

// CFD01：窗口35/57共用C9A96C绘制门禁与原返1，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A82C0。
static std::int32_t WindowChat_Draw(const std::int32_t)
{
    if (input::gChatPanelVisible_C9A96C != 0) {
        WindowRuntime_DrawCurrent();
    }
    return 1;
}

// WSP01：原state bit3控制日志位置换算，换算后重读count；仍返回滚动原结果。
// 原函数：0x004A80F0。
static std::int32_t WindowChat_DragScroll(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t consumed = WindowElement_HandleVerticalScroll(index, state);
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        WindowInstance* const window = currentWindow;
        const std::int32_t count = world::gChatRecordCount_C9A944;
        auto* const dragScrollElement = window->FindElementById(WindowChatElementId::DragScroll);
        if (dragScrollElement) {
            const std::int32_t position = WindowScroll_ReadPosition(dragScrollElement, count);
            gChatScrollOffsetC9A940 = world::gChatRecordCount_C9A944 - position;
        }
    }
    return consumed;
}

// CFD01：首窗口滚轮按原门槛和调用后新count发布scroll，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A81D0。
static std::int32_t WindowChat_Scroll(const std::int32_t, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    if (window != gWindows[WindowId_None] || (state & (MouseFlag_Hover | MouseFlag_Dragging)) == 0) {
        return 0;
    }
    const std::int32_t count = world::gChatRecordCount_C9A944;
    if (count == 0) {
        if (platform::gLegacyMouseState.tailReset != 0) {
            auto* elementsWindow = window;
            if (auto* const element9 = elementsWindow->FindElementById(WindowChatElementId::Element9), * const handleButtons_15Element = elementsWindow->FindElementById(WindowChatElementId::HandleButtons_15); element9 && handleButtons_15Element) element9->y = handleButtons_15Element->y - 14;
        }
        return 0;
    }
    const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
    const std::int32_t position = count - gChatScrollOffsetC9A940;
    auto* const dragScrollElement = window->FindElementById(WindowChatElementId::DragScroll);
    if (dragScrollElement) {
        const std::int32_t next = WindowScroll_ApplyWheel(dragScrollElement, position, count, wheel);
        gChatScrollOffsetC9A940 = world::gChatRecordCount_C9A944 - next;
    }
    return 0;
}

// 原函数：0x004A7700。
static std::int32_t WindowChat_ResetScroll()
{
    world::gChatRecordCount_C9A944 = 0;
    gChatScrollOffsetC9A940 = 0;
    return 0;
}

// 原函数：0x0044B170。
std::int32_t WindowChat_ClearHistory()
{
    for (auto& record : world::gRecords5FB7F8) {
        record.text[0] = '\0';
    }
    return WindowChat_ResetScroll();
}

// 原函数：0x004A7710。
void WindowChat_RequestHeightIncrease()
{
    gChatHeightRequestC9A93C = 1;
}

// 原函数：0x004A7720。
void WindowChat_RequestHeightDecrease()
{
    gChatHeightRequestC9A93C = -1;
}

// 原函数：0x004A7730。
void WindowChat_RequestWidthIncrease()
{
    gChatWidthRequestC9A934 = 1;
}

// 原函数：0x004A7740。
void WindowChat_RequestWidthDecrease()
{
    gChatWidthRequestC9A934 = -1;
}

// 原函数：0x004A82E0。
static std::int32_t WindowChat_RefreshCompactHeight()
{
    const std::int32_t height = gChatHeightUnitsC9A938 + gChatHeightRequestC9A93C;
    gChatHeightUnitsC9A938 = height;
    if (height > 26) {
        gChatHeightUnitsC9A938 = 4;
    } else if (height < 4) {
        gChatHeightUnitsC9A938 = 26;
    }
    gChatHeightRequestC9A93C = 0;
    gChatWidthRequestC9A934 = 0;
    return 0;
}

// CIG01：IME测量后先字体宽度再聊天单位取样；见doc/cg2d_input_scroll_draw_spec.md。
// 原函数：0x004A7590。
void WindowChat_DrawInput(input::InputContext* const context)
{
    const std::int32_t font = input::gCurrentInputData->field13C;
    static_cast<void>(cg::render::TextMeasure(platform::gImeComposition, font));
    const std::int32_t fontWidth = render::GetFontMetrics(9).doubleWidth;
    const std::int32_t widthUnits = gChatWidthUnitsC9A930;
    const std::int32_t width = fontWidth * widthUnits;
    WindowInput_ScrollAndDraw(context, width);
}

// CIG01：固定1024宽度及滚动绘制正文保持；见doc/cg2d_input_scroll_draw_spec.md。
// 原函数：0x004A7640。
void WindowChat_DrawFixedWidthInput(input::InputContext* const context)
{
    const std::int32_t font = input::gCurrentInputData->field13C;
    static_cast<void>(cg::render::TextMeasure(platform::gImeComposition, font));
    WindowInput_ScrollAndDraw(context, 1024);
}

// CLD01：原字节分行/逆序绘制与现场尺寸取样；见doc/cg2d_chat_log_line_drawing_spec.md。
// 原函数：0x004A7750。
void WindowChat_DrawHistory(const std::int32_t x, const std::int32_t y)
{
    std::int32_t totalHeight = 0;
    std::int32_t width;
    if (gChatWindowModeD66E04 == 1) {
        width = render::GetFontMetrics(9).doubleWidth * 44;
    } else {
        const std::int32_t widthUnits = gChatWidthUnitsC9A930 + 7;
        const std::int32_t fontWidth = render::GetFontMetrics(9).doubleWidth;
        width = widthUnits * fontWidth;
    }
    std::int32_t logIndex = world::gChatWriteIndex_626C08;
    logIndex -= gChatScrollOffsetC9A940;
    --logIndex;
    if (logIndex < 0) {
        logIndex = 511;
    }
    for (auto& record : world::gRecords5FB7F8) {
        record.fontType = render::ResolveFontId(record.fontType);
    }
    std::int32_t drawY = y - render::GetFontMetrics(world::gRecords5FB7F8[logIndex].fontType).doubleHeight;
    std::int32_t processedRecords = 0;
    for (;;) {
        auto& source = world::gRecords5FB7F8[logIndex];
        if (source.text.empty()) { return; }
        const auto& contents = source.text;
        const auto rows = render::Font::split(contents, source.fontType, width);
        const std::int32_t lineCount = static_cast<std::int32_t>(rows.size());

        if (lineCount > 0) {
            auto& record = world::gRecords5FB7F8[logIndex];
            const wchar_t* row = rows.back().c_str();
            for (std::int32_t line = 0; line < lineCount; ++line) {
                const std::int32_t font = record.fontType;
                totalHeight += render::GetFontMetrics(font).doubleHeight;
                const std::int32_t fontHeight = render::GetFontMetrics(9).doubleHeight;
                const std::int32_t heightUnits = gChatHeightUnitsC9A938;
                const std::int32_t heightLimit = fontHeight * heightUnits;
                if (totalHeight > heightLimit) {
                    return;
                }
                if (*row != L'\0') {
                    const std::int32_t color = record.color;
                    static_cast<void>(render::DrawText2_456670(
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                        3,
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(font)),
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(color)),
                        row, 0));
                }

                std::int32_t spacingFont;
                if (line + 1 == lineCount) {
                    spacingFont = render::ResolveFontId(WindowChat_PreviousFont(logIndex));
                } else {
                    spacingFont = render::ResolveFontId(record.fontType);
                }
                if (line + 1 < lineCount) { row = rows[static_cast<std::size_t>(lineCount - line - 2)].c_str(); }
                drawY -= render::GetFontMetrics(spacingFont).doubleHeight;
            }
        }

        --logIndex;
        ++processedRecords;
        if (logIndex < 0) {
            logIndex = 511;
        }
        if (processedRecords >= world::gChatRecordCount_C9A944) {
            return;
        }
    }
}

// 原函数：0x004A7B00。
std::int32_t WindowChat_RestoreInput()
{
    const std::int32_t mode = gChatWindowModeD66E04;
    if (mode == 1) {
        WindowInstance* const window = gWindowStates[WindowId_CompactChat].window;
        if (window == nullptr) {
            return 0;
        }
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gChatInputPrototype57E970, x, y, 3));
        auto* const handleInputElement = gWindowStates[WindowId_CompactChat].window ? gWindowStates[WindowId_CompactChat].window->FindElementById(WindowChatElementId::HandleInput) : nullptr;
        if (handleInputElement) {
            gFocusedElementDataD45F94 = handleInputElement->data.input;
        }
        input::SetContext_44B330(&cg::ui::detail::gInputContext626AA0);
        return 1;
    }
    if (mode != 0) {
        return 0;
    }
    WindowInstance* const window = gWindowStates[WindowId_Chat].window;
    if (window == nullptr) {
        return 0;
    }
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    if (WindowInput_ActivateElement(&gChatInputPrototype57E970, x, y, 3) == 0) {
        for (;;) {
            WindowInstance* const currentWindow = gWindowStates[WindowId_Chat].window;
            gChatInputPrototype57E970.input.context = &cg::ui::detail::gInputContext626AA0;
            gChatInputPrototype57E970.input.kind = WindowInputReferenceKind::Context;
            gChatInputPrototype57E970.x = 8;
            cg::ui::detail::gInputContext626AA0 = world::gInputContext626840;
            gChatInputPrototype57E970.y = 5;
            gChatInputPrototype57E970.renderMode = 3;
            gChatInputPrototype57E970.fontType = 9;
            gChatInputPrototype57E970.color = 0;
            gChatInputPrototype57E970.maxLine = 1;
            gChatInputPrototype57E970.maxCharPerLine = 76;
            gChatInputPrototype57E970.value224 = 0;
            gChatInputPrototype57E970.encrypt = 0;
            const std::int32_t currentY = currentWindow->y;
            const std::int32_t currentX = currentWindow->x;
            if (WindowInput_ActivateElement(&gChatInputPrototype57E970, currentX, currentY, 3) != 0) {
                break;
            }
        }
    }
    auto* const handleInputElement = gWindowStates[WindowId_Chat].window ? gWindowStates[WindowId_Chat].window->FindElementById(WindowChatElementId::HandleInput) : nullptr;
    if (handleInputElement) {
        gFocusedElementDataD45F94 = handleInputElement->data.input;
    }
    input::SetContext_44B330(&cg::ui::detail::gInputContext626AA0);
    return 1;
}

} // namespace detail

// 窗口函数实现。

// HBN01：同一C9A930 W32正常两倍返S32，正确正文保持，不新增数值模型。
// 原函数：0x004A80E0。
std::int32_t WindowChat_ReadInputByteWidth()
{
    return detail::gChatWidthUnitsC9A930 * 2;
}

} // namespace cg::ui
