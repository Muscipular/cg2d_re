#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowQuantityElementId : ElementId {
    HandleInput = 1,
    ButtonClose = 2,
    ButtonOk = 3,
    ButtonIncrease = 4,
    ButtonDecrease = 5,
    Element6 = 6,
    Element7 = 7,
    HandleSideArea = 8,
};


namespace detail {

// 控件载荷、窗口状态、组件表与原型。

input::InputContext gInputContextCB0878 {}; // 原全局：0x00CB0878。


// 回调与内部函数前置声明。

// 原函数：0x0048A8C0。
static std::int32_t WindowQuantity_Update(std::int32_t value); // QVC01：-1初始化后总返回1。

// 原函数：0x0048A900。
static std::int32_t WindowQuantity_HandleInput(std::int32_t unusedIndex, std::int32_t state); // QVC01：输入bit2切换/焦点发布，返回0。

// 原函数：0x0048A950。
static std::int32_t WindowQuantity_ButtonOk(std::int32_t index, std::int32_t state); // QVC01：确认回调后复读取窗/关闭/格式，返回0或1。

// 原函数：0x0048AA60。
static std::int32_t WindowQuantity_ButtonIncrease(std::int32_t index, std::int32_t state); // QVC01：增加数量先上限后数值，原图号/声效。

// 原函数：0x0048AB10。
static std::int32_t WindowQuantity_ButtonDecrease(std::int32_t index, std::int32_t state); // QVC01：减少数量S32正值门槛，原图号/声效。

// 原函数：0x0048A8C0。
static std::int32_t WindowQuantity_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowInputPrototype gNumberInputPrototype57DA70 { // 原全局：0x0057DA70。
    {{&cg::ui::detail::gInputContextCB0878}, WindowInputReferenceKind::Context},
    0, -100, 3, 8, 127, {}, 1, 0, 0, 0,
};

// 原CC4D1C至宠物槽CC4D28前的文本区。
static std::wstring gNumberWindowTextCC4D1C {}; // 原全局：0x00CC4D1C。

static const std::array<const wchar_t*, 6> gNumberWindowHints57DC9C {{ // 原全局：0x0057DC9C。
    L"指定金额。",
    L"将指定物金额放地上。",
    L"请指定金额。",
    L"减少一位数。",
    L"指定全额。",
    L"将指定金额归零。",
}};

static WindowGraphicData gWindowGraphic563080 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563080。

static WindowGraphicData gWindowGraphic563098 {244309, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563098。

static WindowGraphicData gWindowGraphic5630B0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005630B0。

static WindowGraphicData gWindowGraphic5630C8 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005630C8。

static WindowNumberData gWindowNumber5630E8 {&gNumberWindowTextCC4D1C, 10, 3, 1, 0}; // 原全局：0x005630E8。

static WindowElementPrototype gWindowElements5630F0[8] { // 原全局：0x005630F0。
    {WindowElementType_Input, 0, 0, 126, 136, 1, {.unusedValue = 0}, &WindowQuantity_HandleInput, -1, static_cast<ElementId>(WindowQuantityElementId::HandleInput)},
    {WindowElementType_Graphic, 106, 8, 11, 11, 1, {.graphic = &gWindowGraphic563080}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowQuantityElementId::ButtonClose)},
    {WindowElementType_EventArea, 18, 46, 106, 85, 1, {.unusedValue = 0}, &WindowQuantity_ButtonOk, -1, static_cast<ElementId>(WindowQuantityElementId::ButtonOk)},
    {WindowElementType_Graphic, 92, 29, 11, 11, 1, {.graphic = &gWindowGraphic5630B0}, &WindowQuantity_ButtonIncrease, -1, static_cast<ElementId>(WindowQuantityElementId::ButtonIncrease)},
    {WindowElementType_Graphic, 105, 29, 11, 11, 1, {.graphic = &gWindowGraphic5630C8}, &WindowQuantity_ButtonDecrease, -1, static_cast<ElementId>(WindowQuantityElementId::ButtonDecrease)},
    {WindowElementType_Number, 85, 30, 0, 0, 1, {.number = &gWindowNumber5630E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowQuantityElementId::Element6)},
    {WindowElementType_Graphic, 0, 0, 126, 136, 1, {.graphic = &gWindowGraphic563098}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowQuantityElementId::Element7)},
    {WindowElementType_EventArea, 128, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowQuantityElementId::HandleSideArea)},
};

// 原型：窗口42；原全局：0x0054E510。
WindowPrototype gWindowPrototypeQuantity {0, 4, 100, 100, 126, 136, 0x80010101u, 8, gWindowElements5630F0, &WindowQuantity_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 42: 54E510

// 窗口函数实现。

// QIP01：原48A830完整37条；原窗口42复用/开启，元素6图号、状态窗口及返回指针分时取样。
// 原函数：0x0048A830。
WindowInstance* WindowQuantity_Open(
    const std::int32_t sourceWindow, const std::int32_t sourceElement,
    const std::int32_t imageNo, const std::int32_t maximum, const std::int32_t value14,
    const std::int32_t initialValue, const WindowValueCallback callback)
{
    if (gWindowStates[WindowId_Quantity].window != nullptr) {
        gWindowStates[WindowId_Quantity].window->state &= ~std::uint32_t{0x8000};
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else {
        gWindowStates[WindowId_Quantity].window = WindowRuntime_Open(WindowId_Quantity);
    }
    WindowInstance* const positionedWindow = gWindowStates[WindowId_Quantity].window;
    static_cast<void>(CenterPopupWindowPosition(WindowId_Quantity,
        positionedWindow->width, positionedWindow->height, positionedWindow->x, positionedWindow->y));
    if (auto* const element7 = gWindowStates[WindowId_Quantity].window ? gWindowStates[WindowId_Quantity].window->FindElementById(WindowQuantityElementId::Element7) : nullptr; element7) element7->data.graphic->imageNo = imageNo;
    WindowInstance* const stateWindow = gWindowStates[WindowId_Quantity].window;
    gWindowStates[WindowId_Quantity].value10 = static_cast<std::uint32_t>(maximum);
    gWindowStates[WindowId_Quantity].value14 = static_cast<std::uint32_t>(value14);
    gWindowStates[WindowId_Quantity].userState.callback = callback;
    if (auto* const buttonOkElement = stateWindow ? stateWindow->FindElementById(WindowQuantityElementId::ButtonOk) : nullptr; buttonOkElement) buttonOkElement->state = static_cast<std::uint32_t>(initialValue);
    WindowInstance* const result = gWindowStates[WindowId_Quantity].window;
    gWindowStates[WindowId_Quantity].value20 = static_cast<std::uint32_t>(sourceWindow);
    gWindowStates[WindowId_Quantity].value24 = static_cast<std::uint32_t>(sourceElement);
    return result;
}

// QVC01：原48A8C0完整15条；仅-1重置上限/参数/callback与当前元素2，然后激活输入，总返回1。
// 原函数：0x0048A8C0。
static std::int32_t WindowQuantity_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    if (value == -1) {
        WindowInstance* const window = currentWindow;
        gWindowStates[WindowId_Quantity].value10 = 999999999;
        gWindowStates[WindowId_Quantity].value14 = 0;
        gWindowStates[WindowId_Quantity].userState.callback = nullptr;
        if (auto* const buttonOkElement = window->FindElementById(WindowQuantityElementId::ButtonOk); buttonOkElement) buttonOkElement->state = 0;
        static_cast<void>(WindowQuantity_HandleInput(0, 2));
    }
    return 1;
}

// QVC01：原48A900完整19条；bit2才创建/切换CB0878，窗口持址后发布焦点并重取元素data，返回0。
// 原函数：0x0048A900。
static std::int32_t WindowQuantity_HandleInput(const std::int32_t, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    if ((state & 2) != 0) {
        static_cast<void>(WindowInput_ActivateElement(&gNumberInputPrototype57DA70, 0, -100, 0));
        input::SetContext_44B330(&cg::ui::detail::gInputContextCB0878);
        WindowInstance* const window = currentWindow;
        auto* const handleInputElement = window->FindElementById(WindowQuantityElementId::HandleInput);
        if (handleInputElement) {
            gFocusedElementDataD45F94 = handleInputElement->data.input;
        }
        if (auto* const handleInputElement2 = window->FindElementById(WindowQuantityElementId::HandleInput); handleInputElement2) handleInputElement2->data.input->context = &cg::ui::detail::gInputContextCB0878;
    }
    return 0;
}

// QVC01：原48A950完整83条；低BYTE/参数先窗口取样，两次S32钳回、callback与SE后复读取窗/格式保，返回0/1。
// 原函数：0x0048A950。
static std::int32_t WindowQuantity_ButtonOk(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    const input::InputContext* const currentInput = input::gCurrentInputData;
    const std::uint8_t flags = reinterpret_cast<const unsigned char*>(
        &gWindowStates[WindowId_Quantity].actionFlags1C)[0]
        | (currentInput == &cg::ui::detail::gInputContextCB0878 ? 2u : 0u);
    const std::int32_t value14 = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Quantity].value14);
    WindowInstance* const numericWindow = currentWindow;
    const std::uint8_t numeric = WindowInput_HandleNumber(
        numericWindow, index, state, value14,
        223, gNumberWindowHints57DC9C.data(), flags, 0xFFFFFFFFu);
    WindowInstance* window = currentWindow;
    std::int32_t maximum = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Quantity].value10);
    auto* const indexedElement = window->FindElementById(NativeElementKey(index));
    if (indexedElement) {
        if (std::bit_cast<std::int32_t>(indexedElement->state) > maximum) {
            if (auto* const indexedElement2 = window->FindElementById(NativeElementKey(index)); indexedElement2) indexedElement2->state = static_cast<std::uint32_t>(maximum);
            maximum = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Quantity].value10);
            window = currentWindow;
        }
    }
    std::int32_t result = 0;
    if ((numeric & 1u) != 0) {
        auto* const indexedElement2 = window->FindElementById(NativeElementKey(index));
        if (indexedElement2) {
            if (std::bit_cast<std::int32_t>(indexedElement2->state) > maximum) {
                if (auto* const indexedElement3 = window->FindElementById(NativeElementKey(index)); indexedElement3) indexedElement3->state = static_cast<std::uint32_t>(maximum);
                window = currentWindow;
            }
        }
        result = 1;
    }
    if ((numeric & 2u) != 0) {
        const WindowValueCallback callback = gWindowStates[WindowId_Quantity].userState.callback;
        if (callback != nullptr) {
            auto* const indexedElement2 = window->FindElementById(NativeElementKey(index));
            if (indexedElement2) {
                callback(std::bit_cast<std::int32_t>(indexedElement2->state));
            }
            window = currentWindow;
        }
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        window = currentWindow;
        result = 1;
    }
    auto* const indexedElement2 = window->FindElementById(NativeElementKey(index));
    if (indexedElement2) {
        cg::text::FormatTo(gNumberWindowTextCC4D1C, L"{:9}", std::bit_cast<std::int32_t>(indexedElement2->state));
    }
    return result;
}

// QVC01：原48AA60完整57条；悬停持有graphic，增加先读上限再元素2，原声/三图号优先序保，返回0。
// 原函数：0x0048AA60。
static std::int32_t WindowQuantity_ButtonIncrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"增加显示数量。";
    }
    if ((state & 2) != 0) {
        const std::int32_t maximum = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Quantity].value10);
        auto* const buttonOkElement = window->FindElementById(WindowQuantityElementId::ButtonOk);
        if (buttonOkElement) {
            const std::int32_t value = std::bit_cast<std::int32_t>(buttonOkElement->state);
            if (value < maximum) {
                if (auto* const buttonOkElement2 = window->FindElementById(WindowQuantityElementId::ButtonOk); buttonOkElement2) buttonOkElement2->state = static_cast<std::uint32_t>(value + 1);
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
        }
    }
    if (graphic) graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        if (graphic) graphic->imageNo = 243091;
    }
    return 0;
}

// QVC01：原48AB10完整56条；悬停持有graphic，元素2的S32正值门槛减少，原声/三图号优先序保，返回0。
// 原函数：0x0048AB10。
static std::int32_t WindowQuantity_ButtonDecrease(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    auto* const graphic = graphicElement ? graphicElement->data.graphic : nullptr;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"减少显示数量。";
    }
    if ((state & 2) != 0) {
        auto* const buttonOkElement = window->FindElementById(WindowQuantityElementId::ButtonOk);
        if (buttonOkElement) {
            const std::int32_t value = std::bit_cast<std::int32_t>(buttonOkElement->state);
            if (value > 0) {
                if (auto* const buttonOkElement2 = window->FindElementById(WindowQuantityElementId::ButtonOk); buttonOkElement2) buttonOkElement2->state = static_cast<std::uint32_t>(value - 1);
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
        }
    }
    if (graphic) graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        if (graphic) graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        if (graphic) graphic->imageNo = 243094;
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
