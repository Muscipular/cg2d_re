#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowAttributeTargetElementId : ElementId {
    ButtonCancel = 1,
    ButtonBack = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    SelectRow = 17,
    SelectRow_18 = 18,
    SelectRow_19 = 19,
    SelectRow_20 = 20,
    SelectRow_21 = 21,
    SelectRow_22 = 22,
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
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0049F790。
static void WindowAttributeTarget_Release();

// 原函数：0x0049F7B0。
static std::int32_t WindowAttributeTarget_Update(std::int32_t value);

// 原函数：0x0049F7E0。
static void WindowAttributeTarget_Refresh(std::int32_t offset); // 六行名称/等级/HP/FP及伤势图标。

// 原函数：0x0049FA40。
static std::int32_t WindowAttributeTarget_ButtonCancel(std::int32_t index, std::int32_t state);

// 原函数：0x0049FB00。
static std::int32_t WindowAttributeTarget_ButtonBack(std::int32_t index, std::int32_t state);

// 原函数：0x0049FBC0。
static std::int32_t WindowAttributeTarget_SelectRow(std::int32_t index, std::int32_t state);

// 控件载荷、窗口状态、组件表与原型。

static const WindowStatusChoiceRecord* gStatusChoiceRecordsCB03D0 = nullptr; // 原全局：0x00CB03D0。

static std::int32_t gStatusChoiceCountCC2364 = 0; // 原全局：0x00CC2364。

static WindowTextChoiceCallback gStatusChoiceCallbackCF47D4 = nullptr; // 原全局：0x00CF47D4。

static std::int32_t gStatusChoiceOffsetD671D4 = 0; // 原全局：0x00D671D4。

static std::uint8_t gStatusChoiceEnabledC9A970 = 0; // 原全局：0x00C9A970。

static WindowGraphicData gWindowGraphic56D310 {245320, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D310。

static WindowGraphicData gWindowGraphic56D328 {245332, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D328。

static WindowGraphicData gWindowGraphic56D340 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D340。

static WindowGraphicData gWindowGraphic56D358 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D358。

static WindowGraphicData gWindowGraphic56D370 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D370。

static WindowGraphicData gWindowGraphic56D388 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D388。

static WindowGraphicData gWindowGraphic56D3A0 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D3A0。

static WindowGraphicData gWindowGraphic56D3B8 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D3B8。

static WindowTextPrototype gWindowText56D3D0 { // 原全局：0x0056D3D0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■", // 原常量参考：0x00585F64。
};

static WindowTextPrototype gWindowText56D3D8 { // 原全局：0x0056D3D8。
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, L"请选择对象。", // 原常量参考：0x00585F54。
};

static WindowTextPrototype gWindowText56D3E0 { // 原全局：0x0056D3E0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}},
    L"健康程度  等级 名称             体力            魔力",
};

static WindowElementPrototype gWindowElements56D3E8[34] { // 原全局：0x0056D3E8。
    {WindowElementType_Graphic, 151, 200, 66, 17, 1, {.graphic = &gWindowGraphic56D310}, &WindowAttributeTarget_ButtonCancel, -1, static_cast<ElementId>(WindowAttributeTargetElementId::ButtonCancel)},
    {WindowElementType_Graphic, 303, 200, 66, 17, 1, {.graphic = &gWindowGraphic56D328}, &WindowAttributeTarget_ButtonBack, -1, static_cast<ElementId>(WindowAttributeTargetElementId::ButtonBack)},
    {WindowElementType_Text, 180, 25, 0, 0, 1, {.text = &gWindowText56D3D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element3)},
    {WindowElementType_Text, 40, 45, 0, 0, 1, {.text = &gWindowText56D3E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element4)},
    {WindowElementType_Graphic, 57, 72, 0, 0, 1, {.graphic = &gWindowGraphic56D340}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element5)},
    {WindowElementType_Graphic, 57, 92, 0, 0, 1, {.graphic = &gWindowGraphic56D358}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element6)},
    {WindowElementType_Graphic, 57, 112, 0, 0, 1, {.graphic = &gWindowGraphic56D370}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element7)},
    {WindowElementType_Graphic, 57, 132, 0, 0, 1, {.graphic = &gWindowGraphic56D388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element8)},
    {WindowElementType_Graphic, 57, 152, 0, 0, 1, {.graphic = &gWindowGraphic56D3A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element9)},
    {WindowElementType_Graphic, 57, 172, 0, 0, 1, {.graphic = &gWindowGraphic56D3B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element10)},
    {WindowElementType_Text, 40, 70, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element11)},
    {WindowElementType_Text, 40, 90, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element12)},
    {WindowElementType_Text, 40, 110, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element13)},
    {WindowElementType_Text, 40, 130, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element14)},
    {WindowElementType_Text, 40, 150, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element15)},
    {WindowElementType_Text, 40, 170, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element16)},
    {WindowElementType_EventArea, 40, 70, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow, -1, static_cast<ElementId>(WindowAttributeTargetElementId::SelectRow)},
    {WindowElementType_EventArea, 40, 90, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow, -1, static_cast<ElementId>(WindowAttributeTargetElementId::SelectRow_18)},
    {WindowElementType_EventArea, 40, 110, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow, -1, static_cast<ElementId>(WindowAttributeTargetElementId::SelectRow_19)},
    {WindowElementType_EventArea, 40, 130, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow, -1, static_cast<ElementId>(WindowAttributeTargetElementId::SelectRow_20)},
    {WindowElementType_EventArea, 40, 150, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow, -1, static_cast<ElementId>(WindowAttributeTargetElementId::SelectRow_21)},
    {WindowElementType_EventArea, 40, 170, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow, -1, static_cast<ElementId>(WindowAttributeTargetElementId::SelectRow_22)},
    {WindowElementType_Text, 40, 70, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element23)},
    {WindowElementType_Text, 40, 90, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element24)},
    {WindowElementType_Text, 40, 110, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element25)},
    {WindowElementType_Text, 40, 130, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element26)},
    {WindowElementType_Text, 40, 150, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element27)},
    {WindowElementType_Text, 40, 170, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element28)},
    {WindowElementType_Text, 40, 70, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element29)},
    {WindowElementType_Text, 40, 90, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element30)},
    {WindowElementType_Text, 40, 110, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element31)},
    {WindowElementType_Text, 40, 130, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element32)},
    {WindowElementType_Text, 40, 150, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element33)},
    {WindowElementType_Text, 40, 170, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAttributeTargetElementId::Element34)},
};

// 原型：窗口46；原全局：0x0054E930。
WindowPrototype gWindowPrototypeAttributeTarget {0, 4, 60, 105, 520, 243, 0x80010101u, 34, gWindowElements56D3E8, &WindowAttributeTarget_Update, &WindowRuntime_DrawLargeFrame, &WindowAttributeTarget_Release}; // 46: 54E930

// 窗口函数实现。

// 原函数：0x0049F760。
cg::yobi::YobiAction* WindowAttributeTarget_Open(
    const WindowStatusChoiceRecord* const records, const std::int32_t count,
    const WindowTextChoiceCallback callback)
{
    gStatusChoiceRecordsCB03D0 = records;
    gStatusChoiceCountCC2364 = count;
    gStatusChoiceCallbackCF47D4 = callback;
    return WindowRuntime_CreateAnimation(WindowId_AttributeTarget, 1, 0, nullptr);
}

// 原函数：0x0049F790。
static void WindowAttributeTarget_Release()
{
    const WindowTextChoiceCallback callback = gStatusChoiceCallbackCF47D4;
    if (callback != nullptr) {
        static_cast<void>(callback(-1, nullptr));
    }
}

// 原函数：0x0049F7B0。
static std::int32_t WindowAttributeTarget_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowAttributeTarget_Refresh(gStatusChoiceOffsetD671D4);
        gStatusChoiceEnabledC9A970 = 1;
    }
    return 1;
}

// 原函数：0x0049F7E0。
static void WindowAttributeTarget_Refresh(const std::int32_t offset)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    std::int32_t row = 0;
    for (; row < 6 && offset + row < gStatusChoiceCountCC2364; ++row) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 4)); indexedElement) indexedElement->visible = 0;
        auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 10));
        if (indexedElement2) {
            indexedElement2->data.text->text.clear();
        }
        auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(row + 22));
        if (indexedElement3) {
            indexedElement3->data.text->text.clear();
        }
        auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 28));
        if (indexedElement4) {
            indexedElement4->data.text->text.clear();
        }
        const wchar_t* const name = gStatusChoiceRecordsCB03D0[offset + row].name;
        if (name != nullptr) {
            auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(row + 10));
            if (indexedElement5) {
                cg::text::FormatTo(indexedElement5->data.text->text, L"           {}", name);
            }
            auto* const indexedElement6 = currentWindow->FindElementById(NativeElementKey(row + 10));
            if (indexedElement6) {
                indexedElement6->data.text->style.color =
                    static_cast<std::byte>(gStatusChoiceRecordsCB03D0[offset + row].color);
            }
            if (auto* const indexedElement7 = currentWindow->FindElementById(NativeElementKey(row + 4)); indexedElement7) indexedElement7->visible = 1;
            const std::int32_t imageIndex = WindowItem_HealthImageLevel(gStatusChoiceRecordsCB03D0[offset + row].injury);
            auto* const indexedElement8 = currentWindow->FindElementById(NativeElementKey(row + 4));
            if (indexedElement8) {
                indexedElement8->data.graphic->imageNo =
                    gHealthImages57E6A4[imageIndex];
            }
            const std::int32_t level = gStatusChoiceRecordsCB03D0[offset + row].level;
            auto* const indexedElement9 = currentWindow->FindElementById(NativeElementKey(row + 22));
            if (indexedElement9) {
                cg::text::FormatTo(indexedElement9->data.text->text, L"{:10}", level);
            }
            const WindowStatusChoiceRecord* const record = gStatusChoiceRecordsCB03D0 + offset + row;
            const std::int32_t maxFp = record->maxFp;
            const std::int32_t fp = record->fp;
            const std::int32_t maxHp = record->maxHp;
            const std::int32_t hp = record->hp;
            auto* const indexedElement10 = currentWindow->FindElementById(NativeElementKey(row + 28));
            if (indexedElement10) {
                cg::text::FormatTo(indexedElement10->data.text->text, L"                            {:6}/{:6} {:6}/{:6}", hp, maxHp, fp, maxFp);
            }
        }
    }
    for (; row < 6; ++row) {
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(row + 4)); indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(row + 10)); indexedElement2) indexedElement2->data.text->text = L"";
        if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(row + 22)); indexedElement3) indexedElement3->data.text->text = L"";
        if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(row + 28)); indexedElement4) indexedElement4->data.text->text = L"";
    }
}

// 原函数：0x0049FA40。
static std::int32_t WindowAttributeTarget_ButtonCancel(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            gHoverTextD68AE8 = L"关闭此视窗";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (gStatusChoiceEnabledC9A970 != 0) {
                const WindowTextChoiceCallback callback = gStatusChoiceCallbackCF47D4;
                if (callback != nullptr) {
                    if (callback(-2, nullptr) != 0) {
                        currentWindow->state |= 0x8000u;
                        static_cast<void>(audio::PlaySe_52C450(57, 320));
                    }
                    gStatusChoiceEnabledC9A970 = 0;
                }
            }
            consumed = 1;
        }
        graphic->imageNo = 245320;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 245322;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 245321;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0049FB00。
static std::int32_t WindowAttributeTarget_ButtonBack(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* const graphicElement = currentWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            gHoverTextD68AE8 = L"回到前一页视窗。";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (gStatusChoiceEnabledC9A970 != 0) {
                const WindowTextChoiceCallback callback = gStatusChoiceCallbackCF47D4;
                if (callback != nullptr) {
                    if (callback(-3, nullptr) != 0) {
                        currentWindow->state |= 0x8000u;
                        static_cast<void>(audio::PlaySe_52C450(57, 320));
                    }
                    gStatusChoiceEnabledC9A970 = 0;
                }
            }
            consumed = 1;
        }
        graphic->imageNo = 245332;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 245334;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 245333;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0049FBC0。
static std::int32_t WindowAttributeTarget_SelectRow(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t selected = gStatusChoiceOffsetD671D4 + index - 16;
    if (index - 16 >= gStatusChoiceCountCC2364) {
        return 0;
    }
    if ((state & MouseFlag_Hover) != 0) {
        auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index));
        if (indexedElement) {
            WindowElement_DrawHoverFrame(indexedElement, state, cg::render::gHoverFramePaletteIndex_D69768);
        }
        gHoverTextD68AE8 = L"选择此人物。";
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        if (gStatusChoiceEnabledC9A970 != 0) {
            const WindowTextChoiceCallback callback = gStatusChoiceCallbackCF47D4;
            if (callback != nullptr) {
                if (callback(selected, gStatusChoiceRecordsCB03D0[selected].name) != 0) {
                    currentWindow->state |= 0x8000u;
                }
                gStatusChoiceEnabledC9A970 = 0;
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            }
        }
        return 1;
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
