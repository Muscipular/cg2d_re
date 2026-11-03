#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowPetAttributesElementId : ElementId {
    EditName = 1,
    ButtonClose = 2,
    SwitchPage = 3,
    Element4 = 4,
    SwitchPage_5 = 5,
    HandleButtons = 6,
    HandleButtons_7 = 7,
    HandleButtons_8 = 8,
    HandleButtons_9 = 9,
    HandleButtons_10 = 10,
    HandleButtons_11 = 11,
    HandleButtons_12 = 12,
    HandleButtons_13 = 13,
    HandleButtons_14 = 14,
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
    HandleSideArea = 46,
    SwitchEquipment = 67, // 四个宠物详情页的装备页签统一ID。
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0048E430。
static std::int32_t WindowPetAttributes_HandleButtons(ElementId elementId, std::int32_t state); // 宠物详细属性九按钮，9..13发送加点。

// 原函数：0x0048DAB0。
static std::int32_t WindowPetAttributes_Update(std::int32_t value);

// 原函数：0x0048DD00。
static void WindowPetAttributes_Release();

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic565680 {0x3BAF0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565680。

static WindowGraphicData gWindowGraphic5658D8 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005658D8。

static WindowElementPrototype gWindowElements566200[47] { // 原全局：0x00566200。
    {WindowElementType_Graphic, 186, 275, 35, 25, 1, {.graphic = &gPetEquipmentPageGraphic}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetAttributesElementId::SwitchEquipment)},
    {WindowElementType_Input, 66, 31, 96, 12, 1, {.unusedValue = 0}, &WindowPetDetail_EditName, -1, static_cast<ElementId>(WindowPetAttributesElementId::EditName)},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPetAttributesElementId::ButtonClose)},
    {WindowElementType_Graphic, 27, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetAttributesElementId::SwitchPage)},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element4)},
    {WindowElementType_Graphic, 133, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetAttributesElementId::SwitchPage_5)},
    {WindowElementType_Graphic, 187, 28, 32, 16, 1, {.graphic = &gWindowGraphic565650}, &WindowPetAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons)},
    {WindowElementType_Graphic, 175, 253, 49, 15, 1, {.graphic = &gWindowGraphic5658C0}, &WindowPetAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_7)},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_8)},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_9)},
    {WindowElementType_Graphic, 61, 113, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_10)},
    {WindowElementType_Graphic, 61, 128, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_11)},
    {WindowElementType_Graphic, 61, 143, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_12)},
    {WindowElementType_Graphic, 61, 158, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_13)},
    {WindowElementType_Graphic, 61, 173, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_14)},
    {WindowElementType_Text, 66, 31, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element15)},
    {WindowElementType_Text, 63, 48, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element16)},
    {WindowElementType_Text, 55, 65, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element17)},
    {WindowElementType_Text, 140, 65, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element18)},
    {WindowElementType_Text, 44, 256, 0, 0, 1, {.text = &gWindowText565988}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element19)},
    {WindowElementType_Text, 93, 98, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element20)},
    {WindowElementType_Text, 93, 114, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element21)},
    {WindowElementType_Text, 93, 129, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element22)},
    {WindowElementType_Text, 93, 144, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element23)},
    {WindowElementType_Text, 93, 159, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element24)},
    {WindowElementType_Text, 93, 174, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element25)},
    {WindowElementType_Text, 93, 197, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element26)},
    {WindowElementType_Text, 93, 212, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element27)},
    {WindowElementType_Text, 155, 97, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element28)},
    {WindowElementType_Text, 205, 98, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element29)},
    {WindowElementType_Text, 155, 114, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element30)},
    {WindowElementType_Text, 205, 113, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element31)},
    {WindowElementType_Text, 155, 144, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element32)},
    {WindowElementType_Text, 205, 144, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element33)},
    {WindowElementType_Text, 155, 159, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element34)},
    {WindowElementType_Text, 205, 159, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element35)},
    {WindowElementType_Text, 155, 174, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element36)},
    {WindowElementType_Text, 205, 174, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element37)},
    {WindowElementType_Text, 155, 197, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element38)},
    {WindowElementType_Text, 205, 197, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element39)},
    {WindowElementType_Text, 155, 212, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element40)},
    {WindowElementType_Text, 205, 212, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element41)},
    {WindowElementType_Text, 155, 227, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element42)},
    {WindowElementType_Graphic, 18, 32, 0, 0, 1, {.graphic = &gWindowGraphic565680}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element43)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic565608}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element44)},
    {WindowElementType_Graphic, 14, 25, 0, 0, 1, {.graphic = &gWindowGraphic565620}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetAttributesElementId::Element45)},
    {WindowElementType_EventArea, 250, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPetAttributesElementId::HandleSideArea)},
};

// 原型：窗口17；原全局：0x0054E5A0。
WindowPrototype gWindowPrototypePetAttributes {0, 4, 365, 83, 245, 306, 0x80010101u, 47, gWindowElements566200, &WindowPetAttributes_Update, &WindowRuntime_DrawDefault, &WindowPetAttributes_Release}; // 17: 54E5A0

// 窗口函数实现。

// 窗口17完整属性刷新；显示逐项取号，加点判断单独固定宠物，见window_17.md。
// 原函数：0x00490780。
static std::uint32_t WindowPetAttributes_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const auto selectedPet = []() -> world::PetRuntimeRecord& {
        const auto index = std::bit_cast<std::int16_t>(
            world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
        return world::gPetRuntimeRecords[index];
    };
    const auto copyTextAfterScan = [currentWindow](const std::int32_t element, const std::wstring_view text) {
        auto* elementsWindow = currentWindow;
        auto* const dataElement = elementsWindow->FindElementById(NativeElementKey(element));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = text;
        }
        return;
    };
    const auto copyTextBeforeScan = [currentWindow](const std::int32_t element, const std::wstring_view text) -> std::size_t {
        auto* const dataElement = currentWindow->FindElementById(NativeElementKey(element));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            const std::size_t length = text.size() + 1;
            data->text = text;
            return length;
        }
        return 0;
    };
    constexpr std::int32_t valueElements[] {
        14, 15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
        29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
    };
    std::wstring buffer;
    const auto writeNumber = [&](const std::int32_t element, const std::int32_t value) {
        buffer = std::format(L"{:>3}", value);
        static_cast<void>(copyTextBeforeScan(element, buffer));
    };
    if (selectedPet().used != 0) {
        for (const std::int32_t element : valueElements) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 1;
        }
        const bool customName = selectedPet().name6BD[0] != '\0';
        input::InputContext* const activeInput = input::gCurrentInputData;
        // 原名分支在输入身份判断前固定elements，自定义名分支在判断后读取。
        auto* originalNameElement = customName ? nullptr : currentWindow->FindElementById(WindowPetAttributesElementId::Element15);
        if (activeInput == &cg::ui::detail::gInputContextD2D4A0) {
            if (customName) {
                if (auto* const element15 = currentWindow->FindElementById(WindowPetAttributesElementId::Element15); element15) element15->visible = 0;
            } else {
                if (originalNameElement) originalNameElement->visible = 0;
            }
            const std::wstring text = input::ContextText(*gPetNameInputPrototype57DCF8.input.context);
            WindowInstance* const window = currentWindow;
            gLoginWindowRuntime.petNameDraftD45E74 = text;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 1));
        } else {
            if (customName) {
                if (auto* const element15 = currentWindow->FindElementById(WindowPetAttributesElementId::Element15); element15) element15->visible = 1;
                static_cast<void>(copyTextBeforeScan(14, gLoginWindowRuntime.petNameDraftD45E74));
            } else {
                if (originalNameElement) originalNameElement->visible = 1;
                copyTextAfterScan(14, gLoginWindowRuntime.petNameDraftD45E74);
            }
            const auto& compared = selectedPet();
            const wchar_t* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
            const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(name) != 0;
            auto* const element15 = currentWindow->FindElementById(WindowPetAttributesElementId::Element15);
            if (element15) {
                auto* const data = element15->data.text;
                data->style.color = changed ? std::byte{6} : std::byte{0};
            }
        }
        const auto& compared = selectedPet();
        const wchar_t* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
        const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(name) != 0;
        if (auto* const handleButtonsElement = currentWindow->FindElementById(WindowPetAttributesElementId::HandleButtons); handleButtonsElement) handleButtonsElement->visible = changed ? 1 : 0;

        writeNumber(15, selectedPet().value08);
        const std::int32_t maxHp = static_cast<std::int32_t>(selectedPet().values0C[1]);
        const std::int32_t hp = static_cast<std::int32_t>(selectedPet().values0C[0]);
        buffer = std::format(L"{:>4}/{:>4}", hp, maxHp);
        copyTextAfterScan(16, buffer);
        const std::int32_t maxFp = static_cast<std::int32_t>(selectedPet().values0C[3]);
        const std::int32_t fp = static_cast<std::int32_t>(selectedPet().values0C[2]);
        buffer = std::format(L"{:>4}/{:>4}", fp, maxFp);
        copyTextAfterScan(17, buffer);
        writeNumber(19, selectedPet().lvFlag6C);
        for (std::int32_t attribute = 0; attribute < 5; ++attribute) {
            writeNumber(20 + attribute, selectedPet().values70[attribute]);
        }
        writeNumber(25, selectedPet().values70[10]);
        writeNumber(26, selectedPet().value6A4);
        constexpr std::int32_t valueIndexes[] {17, 19, 18, 20, 11, 14, 12, 15, 13, 16, 5, 8, 6, 9, 7};
        for (std::int32_t index = 0; index < 15; ++index) {
            writeNumber(27 + index, selectedPet().values70[valueIndexes[index]]);
        }

        for (std::int32_t element = 9; element <= 13; ++element) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
        }
        const auto& allocationPet = selectedPet();
        if (allocationPet.lvFlag6C > 0) {
            const std::array<std::int32_t, 6> values {
                allocationPet.values70[0], allocationPet.values70[1], allocationPet.values70[2],
                allocationPet.values70[3], allocationPet.values70[4], allocationPet.lvFlag6C,
            };
            std::int32_t total = 0;
            for (const std::int32_t value : values) {
                total += value;
            }
            const std::int32_t threshold = total / 2;
            for (std::int32_t attribute = 0; attribute < 5; ++attribute) {
                if (values[attribute] < threshold) {
                    if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(9 + attribute)); indexedElement) indexedElement->visible = 1;
                }
            }
        }
    } else {
        for (const std::int32_t element : valueElements) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
        }
        for (std::int32_t element = 9; element <= 13; ++element) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
        }
    }
    buffer = std::format(L"{}/{}", gLoginWindowRuntime.selectedPetCC3E5C + 1, 5);
    return static_cast<std::uint32_t>(copyTextBeforeScan(18, buffer));
}

// 窗口17初始化仅采样名称/序号并发声，不代替窗口16的拖动槽初始化。
// 原函数：0x0048DAB0。
static std::int32_t WindowPetAttributes_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        static_cast<void>(WindowPetDetail_SavePetSequences());
        WindowPetDetail_InitializeDraft();
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        return 1;
    }
    if (gLoginWindowRuntime.selectedPetCC3E5C != gLoginWindowRuntime.previousPetCC4888) {
        WindowPetDetail_InitializeDraft();
    }
    gLoginWindowRuntime.previousPetCC4888 = gLoginWindowRuntime.selectedPetCC3E5C;
    if (WindowPetDetail_HasPetSequenceChanges() != 0 || WindowPetDetail_ReadDraftFlag() != 0) {
        static_cast<void>(WindowPetDetail_SavePetSequences());
        WindowPetDetail_InitializeDraft();
        WindowPetDetail_ResetDraftFlag();
        WindowInstance* const window = currentWindow;
        gPetNameInputPrototype57DCF8.text = gLoginWindowRuntime.petNameDraftD45E74;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
    }
    const auto selected = std::bit_cast<std::int16_t>(
        world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    if (world::gPetRuntimeRecords[selected].used == 0) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    static_cast<void>(WindowPetAttributes_Refresh());
    return 1;
}

// 原函数：0x0048DD00。
static void WindowPetAttributes_Release()
{
    WindowInstance* const window = WindowRuntime_States()[WindowId_PetAttributes].window;
    if (window != nullptr) {
        WindowRuntime_States()[WindowId_PetDetail].x = window->x;
        WindowRuntime_States()[WindowId_PetDetail].y = window->y;
        WindowRuntime_States()[WindowId_PetSkills].x = window->x;
        WindowRuntime_States()[WindowId_PetSkills].y = window->y;
    }
}

// 原函数：0x0048E430。
static std::int32_t WindowPetAttributes_HandleButtons(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    const auto setImage = [elementId, currentWindow](const std::int32_t image) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = image;
    };
    const auto petIndex = [](const std::int32_t position) {
        return std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[position].index);
    };
    switch (static_cast<WindowPetAttributesElementId>(elementId)) {
    case WindowPetAttributesElementId::HandleButtons:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B570);
            gHoverTextD68AE8 = kPetRenameHint586D38;
            consumed = 1;
        } else {
            setImage(0x3B56E);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::wstring text = input::ContextText(cg::ui::detail::gInputContextD2D4A0);
            const std::wstring escapedText = input::EscapeTextAndNotify(text, 256);
            const std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
            const auto networkSession = cg::network::GetGameConnection().CaptureSession();
            const std::int16_t pet = petIndex(position);
            static_cast<void>(cg::network::GetGameConnection().RenamePetRequest(pet, escapedText));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
            WindowChat_RestoreInput();
            if (text.empty()) {
                const std::int16_t selected = petIndex(gLoginWindowRuntime.selectedPetCC3E5C);
                const wchar_t* const name = world::gPetRuntimeRecords[selected].name6AC.data();
                gLoginWindowRuntime.petNameDraftD45E74 = name;
                auto* const element15 = currentWindow->FindElementById(WindowPetAttributesElementId::Element15);
                if (element15) {
                    auto* const data = element15->data.text;
                    data->text = name;
                }
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B56F);
        }
        return consumed;
    case WindowPetAttributesElementId::HandleButtons_7:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3BAF9);
            gHoverTextD68AE8 = kPetPlaceHint586C14;
            consumed = 1;
        } else {
            setImage(0x3BAF7);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].used != 0
                && (world::ReadPlayerActionFlags() & 0x80u) == 0
                && world::IsObservationSceneTransitionActive_440B50() == 0) {
                const std::int16_t pet = petIndex(gLoginWindowRuntime.selectedPetCC3E5C);
                const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
                const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
                const auto networkSession = cg::network::GetGameConnection().CaptureSession();
                static_cast<void>(cg::network::GetGameConnection().DropPetRequest(x, y, pet));
                static_cast<void>(audio::PlaySe_52C450(79, 320));
                const std::int16_t selected = petIndex(gLoginWindowRuntime.selectedPetCC3E5C);
                if (world::gPetRuntimeRecords[selected].flags6AA == 2) {
                    WindowCraft_ResetItemSelection();
                }
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3BAF8);
        }
        return consumed;
    case WindowPetAttributesElementId::HandleButtons_8:
    case WindowPetAttributesElementId::HandleButtons_9:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_8) ? 0x3B558 : 0x3B55B);
            gHoverTextD68AE8 = kPetSelectHint586D2C;
            consumed = 1;
        } else {
            setImage(elementId == static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_8) ? 0x3B556 : 0x3B559);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
            for (std::int32_t attempt = 0; attempt < 6; ++attempt) {
                if (elementId == static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_8)) {
                    --position;
                    if (position < 0) {
                        position = 4;
                    }
                } else {
                    ++position;
                    if (position >= 5) {
                        position = 0;
                    }
                }
                if (world::gPetRuntimeRecords[petIndex(position)].used != 0) {
                    break;
                }
            }
            const std::int16_t selected = petIndex(position);
            gLoginWindowRuntime.selectedPetCC3E5C = position;
            const auto& pet = world::gPetRuntimeRecords[selected];
            const wchar_t* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            gPetNameInputPrototype57DCF8.text = name;
            WindowInstance* const window = currentWindow;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
            static_cast<void>(WindowPetAttributes_Update(-1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowPetAttributesElementId::HandleButtons_8) ? 0x3B557 : 0x3B55A);
        }
        return consumed;
    case WindowPetAttributesElementId::HandleButtons_10:
    case WindowPetAttributesElementId::HandleButtons_11:
    case WindowPetAttributesElementId::HandleButtons_12:
    case WindowPetAttributesElementId::HandleButtons_13:
    case WindowPetAttributesElementId::HandleButtons_14:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B53D);
            gHoverTextD68AE8 = L"分配点数至目前的能力数值。" /* 原常量参考：0x005863A4。 */;
            consumed = 1;
        } else {
            setImage(0x3B53B);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t attribute = index - 9;
            const std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
            const auto networkSession = cg::network::GetGameConnection().CaptureSession();
            const std::int16_t pet = petIndex(position);
            static_cast<void>(cg::network::GetGameConnection().IncreasePetAttributeRequest(pet, attribute));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
            consumed = 1;
        }
        return consumed;
    default:
        return 0;
    }
}

} // namespace detail

} // namespace cg::ui
