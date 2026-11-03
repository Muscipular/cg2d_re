#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowPetDetailElementId : ElementId {
    EditName = 1,
    ButtonClose = 2,
    Element3 = 3,
    SwitchPage = 4,
    SwitchPage_5 = 5,
    HandleButtons = 6,
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
    Element60 = 60,
    Element61 = 61,
    Element62 = 62,
    Element63 = 63,
    Element64 = 64,
    HandleButtons_65 = 65,
    HandleSideArea = 66,
    SwitchEquipment = 67, // 四个宠物详情页的装备页签统一ID。
};

namespace detail {

// 控件载荷、窗口状态、组件表与原型。

// 回调与内部函数前置声明。

// 原函数：0x0048DD80。
static std::int32_t WindowPetDetail_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 宠物详情名称/放地/选择/旋转。

// 原函数：0x0048D9A0。
static std::int32_t WindowPetDetail_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x0048DCC0。
static void WindowPetDetail_Release(WindowInstance&);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic563388 {0x3B5D6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563388。

static WindowGraphicData gWindowGraphic565668 {0x3BAEF, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565668。

static WindowGraphicData gWindowGraphic565938 {0x3B614, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565938。

static WindowTextPrototype gWindowText565958 { // 原全局：0x00565958。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"10000000/10000000", // 原常量参考：0x00586020。
};

static WindowTextPrototype gWindowText565960 { // 原全局：0x00565960。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"100000/100000", // 原常量参考：0x00586034。
};

static WindowTextPrototype gWindowText565968 { // 原全局：0x00565968。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"100000/100000", // 原常量参考：0x00586034。
};

static WindowTextPrototype gWindowText565970 { // 原全局：0x00565970。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"种类", // 原常量参考：0x00586018。
};

static WindowTextPrototype gWindowText565978 { // 原全局：0x00565978。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"999", // 原常量参考：0x00586014。
};

static WindowTextPrototype gWindowText565980 { // 原全局：0x00565980。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"99", // 原常量参考：0x00586010。
};

static WindowTextPrototype gWindowText5659A8 { // 原全局：0x005659A8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"99", // 原常量参考：0x00586010。
};

static WindowTextPrototype gWindowText5659B0 { // 原全局：0x005659B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"0/0", // 原常量参考：0x00585FF0。
};

static std::uint32_t gWindowAnimationImage5659B8 = 100000; // 原全局：0x005659B8。

static std::uint32_t gWindowMouseFlagsD677B0 = 0; // 原全局：0x00D677B0。

static WindowElementPrototype gWindowElements5659C0[67] { // 原全局：0x005659C0。
    {WindowElementType_Graphic, 186, 275, 35, 25, 1, {.graphic = &gPetEquipmentPageGraphic}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetDetailElementId::SwitchEquipment)},
    {WindowElementType_Input, 66, 31, 96, 12, 1, {.unusedValue = 0}, &WindowPetDetail_EditName, -1, static_cast<ElementId>(WindowPetDetailElementId::EditName)},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPetDetailElementId::ButtonClose)},
    {WindowElementType_Graphic, 27, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element3)},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetDetailElementId::SwitchPage)},
    {WindowElementType_Graphic, 133, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetDetailElementId::SwitchPage_5)},
    {WindowElementType_Graphic, 187, 28, 32, 16, 1, {.graphic = &gWindowGraphic565650}, &WindowPetDetail_HandleButtons, -1, static_cast<ElementId>(WindowPetDetailElementId::HandleButtons)},
    {WindowElementType_Graphic, 175, 253, 49, 15, 1, {.graphic = &gWindowGraphic5658C0}, &WindowPetDetail_HandleButtons, -1, static_cast<ElementId>(WindowPetDetailElementId::HandleButtons_7)},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetDetail_HandleButtons, -1, static_cast<ElementId>(WindowPetDetailElementId::HandleButtons_8)},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetDetail_HandleButtons, -1, static_cast<ElementId>(WindowPetDetailElementId::HandleButtons_9)},
    {WindowElementType_Text, 66, 31, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element10)},
    {WindowElementType_Graphic, 200, 141, 0, 0, 1, {.graphic = &gWindowGraphic565938}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element11)},
    {WindowElementType_Text, 63, 65, 0, 0, 1, {.text = &gWindowText565958}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element12)},
    {WindowElementType_Text, 48, 82, 0, 0, 1, {.text = &gWindowText565960}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element13)},
    {WindowElementType_Text, 142, 82, 0, 0, 1, {.text = &gWindowText565968}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element14)},
    {WindowElementType_Text, 62, 99, 0, 0, 1, {.text = &gWindowText565970}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element15)},
    {WindowElementType_Text, 150, 99, 0, 0, 1, {.text = &gWindowText565978}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element16)},
    {WindowElementType_Text, 206, 99, 0, 0, 1, {.text = &gWindowText565980}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element17)},
    {WindowElementType_Text, 44, 256, 0, 0, 1, {.text = &gWindowText565988}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element18)},
    {WindowElementType_Text, 63, 48, 0, 0, 1, {.text = &gWindowText5659A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element19)},
    {WindowElementType_Text, 39, 115, 0, 0, 1, {.text = &gWindowText5659B0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element20)},
    {WindowElementType_Animation, 80, 220, 0, 0, 1, {.animationImageNo = reinterpret_cast<std::int32_t*>(&gWindowAnimationImage5659B8)}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element21)},
    {WindowElementType_Graphic, 157, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element22)},
    {WindowElementType_Graphic, 163, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element23)},
    {WindowElementType_Graphic, 169, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element24)},
    {WindowElementType_Graphic, 175, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element25)},
    {WindowElementType_Graphic, 181, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element26)},
    {WindowElementType_Graphic, 187, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element27)},
    {WindowElementType_Graphic, 193, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element28)},
    {WindowElementType_Graphic, 199, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element29)},
    {WindowElementType_Graphic, 205, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element30)},
    {WindowElementType_Graphic, 211, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element31)},
    {WindowElementType_Graphic, 157, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element32)},
    {WindowElementType_Graphic, 163, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element33)},
    {WindowElementType_Graphic, 169, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element34)},
    {WindowElementType_Graphic, 175, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element35)},
    {WindowElementType_Graphic, 181, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element36)},
    {WindowElementType_Graphic, 187, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element37)},
    {WindowElementType_Graphic, 193, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element38)},
    {WindowElementType_Graphic, 199, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element39)},
    {WindowElementType_Graphic, 205, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element40)},
    {WindowElementType_Graphic, 211, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element41)},
    {WindowElementType_Graphic, 157, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element42)},
    {WindowElementType_Graphic, 163, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element43)},
    {WindowElementType_Graphic, 169, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element44)},
    {WindowElementType_Graphic, 175, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element45)},
    {WindowElementType_Graphic, 181, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element46)},
    {WindowElementType_Graphic, 187, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element47)},
    {WindowElementType_Graphic, 193, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element48)},
    {WindowElementType_Graphic, 199, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element49)},
    {WindowElementType_Graphic, 205, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element50)},
    {WindowElementType_Graphic, 211, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element51)},
    {WindowElementType_Graphic, 157, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element52)},
    {WindowElementType_Graphic, 163, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element53)},
    {WindowElementType_Graphic, 169, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element54)},
    {WindowElementType_Graphic, 175, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element55)},
    {WindowElementType_Graphic, 181, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element56)},
    {WindowElementType_Graphic, 187, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element57)},
    {WindowElementType_Graphic, 193, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element58)},
    {WindowElementType_Graphic, 199, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element59)},
    {WindowElementType_Graphic, 205, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element60)},
    {WindowElementType_Graphic, 211, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element61)},
    {WindowElementType_Graphic, 23, 32, 0, 0, 1, {.graphic = &gWindowGraphic565668}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element62)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic565608}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element63)},
    {WindowElementType_Graphic, 14, 25, 0, 0, 1, {.graphic = &gWindowGraphic565620}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetDetailElementId::Element64)},
    {WindowElementType_MouseArea, 19, 132, 114, 110, 1, {.mouseFlags = &gWindowMouseFlagsD677B0}, &WindowPetDetail_HandleButtons, -1, static_cast<ElementId>(WindowPetDetailElementId::HandleButtons_65)},
    {WindowElementType_EventArea, 250, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPetDetailElementId::HandleSideArea)},
};

// 原型：窗口16；原全局：0x0054E570。
WindowPrototype gWindowPrototypePetDetail {0, 4, 365, 83, 245, 306, 0x80010101u, 67, gWindowElements5659C0, &WindowPetDetail_Update, &WindowRuntime_DrawDefault, &WindowPetDetail_Release}; // 16: 54E570

// 窗口函数实现。

// 原函数：0x00492260。
void WindowPetDetail_ResetDraftFlag()
{
    world::gPetDetailDraftChanged_D67B28 = 0;
}

// 原函数：0x00492270。
std::int32_t WindowPetDetail_ReadDraftFlag()
{
    return world::gPetDetailDraftChanged_D67B28 == 1 ? 1 : 0;
}

// 原函数：0x00492280。
void WindowPetDetail_InitializeDraft(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    WindowInstance* const window = currentWindow;
    // 九处caller均属于详情页16/17/18；异常ID的原未写栈下标不专项模拟。
    constexpr std::int32_t nameElements[] {9, 14, 11};
    const std::int32_t element = nameElements[window->id - 16];
    const auto selected = std::bit_cast<std::int16_t>(
        world::gPetPairsDE0B08[business.selectedPetCC3E5C].index);
    const auto& pet = world::gPetRuntimeRecords[selected];
    const wchar_t* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
    business.petNameDraftD45E74 = name;
    if (auto* const indexedElement = window->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->data.text->text = name;
}

// 原函数：0x00492350。
std::int32_t WindowPetDetail_SavePetSequences(session::ClientSession& session)
{
    auto& business = session.UiFrame().native.business;
    std::int32_t sequence;
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto index = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[row].index);
        sequence = world::gPetRuntimeRecords[index].value6D0;
        business.petSequencesCAFEB0[row] = sequence;
    }
    return sequence;
}

// 原函数：0x00492390。
std::int32_t WindowPetDetail_HasPetSequenceChanges(session::ClientSession& session)
{
    auto& business = session.UiFrame().native.business;
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto index = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[row].index);
        const std::int32_t previous = business.petSequencesCAFEB0[row];
        if (previous != world::gPetRuntimeRecords[index].value6D0) {
            return 1;
        }
    }
    return 0;
}

// 窗口16完整刷新；复制取样和空宠物显隐差异见window_16.md。
// 原函数：0x0048F8F0。
static std::uint32_t WindowPetDetail_Refresh(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    auto& inputs = currentWindow->ownerSession->UiFrame().native.inputs;
    const auto selectedPet = [&business]() -> world::PetRuntimeRecord& {
        const auto index = std::bit_cast<std::int16_t>(
            world::gPetPairsDE0B08[business.selectedPetCC3E5C].index);
        return world::gPetRuntimeRecords[index];
    };
    // 保留原复制段对elements或data的取样阶段；正文直接使用UTF-16。
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
    std::wstring buffer;
    if (selectedPet().used != 0) {
        constexpr std::int32_t visibleElements[] {9, 11, 12, 13, 14, 15, 16, 17, 20, 10, 18, 19};
        for (const std::int32_t element : visibleElements) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 1;
        }
        const bool customName = selectedPet().name6BD[0] != '\0';
        input::InputContext* const activeInput = currentWindow->ownerSession->UiFrame().inputState.currentContext;
        if (activeInput == &inputs.petName) {
            if (auto* const element10 = currentWindow->FindElementById(WindowPetDetailElementId::Element10); element10) element10->visible = 0;
            const std::wstring text = input::ContextText(*inputs.petNamePrototype.input.context);
            WindowInstance* const window = currentWindow;
            business.petNameDraftD45E74 = text;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(*currentWindow->ownerSession, &inputs.petNamePrototype, x, y, 1));
        } else {
            if (auto* const element10 = currentWindow->FindElementById(WindowPetDetailElementId::Element10); element10) element10->visible = 1;
            if (customName) {
                static_cast<void>(copyTextBeforeScan(9, business.petNameDraftD45E74));
            } else {
                copyTextAfterScan(9, business.petNameDraftD45E74);
            }
            const auto& compared = selectedPet();
            const wchar_t* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
            const bool changed = business.petNameDraftD45E74.compare(name) != 0;
            auto* const element102 = currentWindow->FindElementById(WindowPetDetailElementId::Element10);
            if (element102) {
                auto* const data = element102->data.text;
                data->style.color = changed ? std::byte{6} : std::byte{0};
            }
        }
        const auto& compared = selectedPet();
        const wchar_t* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
        const bool changed = business.petNameDraftD45E74.compare(name) != 0;
        if (auto* const handleButtonsElement = currentWindow->FindElementById(WindowPetDetailElementId::HandleButtons); handleButtonsElement) handleButtonsElement->visible = changed ? 1 : 0;

        if ((selectedPet().values0C[5] == -1)) {
            const std::int32_t experience = static_cast<std::int32_t>(selectedPet().values0C[4]);
            buffer = std::format(L"{:>8}/       -", experience);
        } else {
            const std::int32_t nextExperience = static_cast<std::int32_t>(selectedPet().values0C[5]);
            const std::int32_t experience = static_cast<std::int32_t>(selectedPet().values0C[4]);
            buffer = std::format(L"{:>8}/{:>8}", experience, nextExperience);
        }
        copyTextAfterScan(11, buffer);
        const std::int32_t maxHp = static_cast<std::int32_t>(selectedPet().values0C[1]);
        const std::int32_t hp = static_cast<std::int32_t>(selectedPet().values0C[0]);
        buffer = std::format(L"{:>5}/{:>5}", hp, maxHp);
        copyTextAfterScan(12, buffer);
        const std::int32_t maxFp = static_cast<std::int32_t>(selectedPet().values0C[3]);
        const std::int32_t fp = static_cast<std::int32_t>(selectedPet().values0C[2]);
        buffer = std::format(L"{:>5}/{:>5}", fp, maxFp);
        copyTextAfterScan(13, buffer);

        const auto& tribePet = selectedPet();
        WindowInstance* const tribeWindow = currentWindow;
        const auto tribe = std::bit_cast<std::int16_t>(tribePet.value04);
        auto* tribeElementsWindow = tribeWindow;
        const wchar_t* const tribeName = gTribeNames57CB54[tribe];
        auto* const element15 = tribeElementsWindow->FindElementById(WindowPetDetailElementId::Element15);
        if (element15) {
            auto* const tribeData = element15->data.text;
            tribeData->text = tribeName;
        }
        buffer = std::format(L"{:>3}", selectedPet().values70[10]);
        static_cast<void>(copyTextBeforeScan(15, buffer));
        buffer = std::format(L"{:>2}", selectedPet().value6A4);
        static_cast<void>(copyTextBeforeScan(16, buffer));
        buffer = std::format(L"{:>3}", selectedPet().value08);
        static_cast<void>(copyTextBeforeScan(18, buffer));
        const auto& weightPet = selectedPet();
        const std::int32_t maxWeight = weightPet.values6D8[2];
        const std::int32_t weight = weightPet.values6D8[1];
        buffer = std::format(L"{:>6}/{:>6}", weight, maxWeight);
        static_cast<void>(copyTextBeforeScan(19, buffer));

        const auto& imagePet = selectedPet();
        auto* imageElementsWindow = currentWindow;
        const std::int32_t imageNo = imagePet.imageNo6A0;
        auto* const element21 = imageElementsWindow->FindElementById(WindowPetDetailElementId::Element21);
        if (element21) {
            auto* const imageData = element21->data.animation;
            cg::yobi::SetYobiActionField2A8(imageData->action, imageNo);
        }
        auto* const element212 = currentWindow->FindElementById(WindowPetDetailElementId::Element21);
        if (element212) {
            auto* const directionData = element212->data.animation;
            const std::int32_t direction = business.valueD673D0;
            cg::yobi::SetYobiActionField2B8(directionData->action, direction);
        }

        const std::int32_t injury = selectedPet().values70[25];
        std::int32_t injuryImage;
        if (injury == 0) {
            injuryImage = 0x3B614;
        } else if (injury <= 25) {
            injuryImage = 0x3B615;
        } else if (injury <= 50) {
            injuryImage = 0x3B616;
        } else if (injury <= 75) {
            injuryImage = 0x3B617;
        } else {
            injuryImage = 0x3B618;
        }
        if (auto* const element11 = currentWindow->FindElementById(WindowPetDetailElementId::Element11); element11) element11->data.graphic->imageNo = injuryImage;

        const auto showSegment = [currentWindow](const std::int32_t element, const std::int32_t image) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->data.graphic->imageNo = image;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(element)); indexedElement2) indexedElement2->visible = 1;
        };
        for (std::int32_t attribute = 0; attribute < 4; ++attribute) {
            const std::int32_t base = 21 + attribute * 10;
            for (std::int32_t segment = 0; segment < 10; ++segment) {
                if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(base + segment)); indexedElement) indexedElement->visible = 0;
            }
            const std::int32_t segments = selectedPet().values70[21 + attribute] / 10;
            const auto& images = gPetBarImages57E434[attribute];
            if (segments == 1) {
                showSegment(base, images[3]);
            } else {
                for (std::int32_t segment = 0; segment < segments; ++segment) {
                    showSegment(base + segment, images[1]);
                    if (segment == 0) {
                        showSegment(base, images[0]);
                    }
                    if (segment == segments - 1) {
                        showSegment(base + segment, images[2]);
                    }
                }
            }
        }
    } else {
        constexpr std::int32_t hiddenElements[] {9, 11, 12, 13, 14, 15, 16, 20, 18, 19};
        for (const std::int32_t element : hiddenElements) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(element)); indexedElement) indexedElement->visible = 0;
        }
        for (std::int32_t segment = 0; segment < 10; ++segment) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(21 + segment)); indexedElement) indexedElement->visible = 0;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(31 + segment)); indexedElement2) indexedElement2->visible = 0;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(41 + segment)); indexedElement3) indexedElement3->visible = 0;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(51 + segment)); indexedElement4) indexedElement4->visible = 0;
        }
        if (auto* const element11 = currentWindow->FindElementById(WindowPetDetailElementId::Element11); element11) element11->visible = 0;
    }
    buffer = std::format(L"{}/{}", business.selectedPetCC3E5C + 1, 5);
    return static_cast<std::uint32_t>(copyTextBeforeScan(17, buffer));
}

// 窗口16主更新，入口-1负责共享详情拖动槽的初始化。
// 原函数：0x0048D9A0。
static std::int32_t WindowPetDetail_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        business.previousPetCC4888 = business.selectedPetCC3E5C;
        static_cast<void>(WindowPetDetail_SavePetSequences(*callbackWindow.ownerSession));
        WindowPetDetail_InitializeDraft(&callbackWindow);
        business.petTechniqueD2D66C = -1;
        business.petTechniqueValueD2F784 = -1;
        business.petTechniqueDragD460CC = 0;
        return 1;
    }
    if (business.selectedPetCC3E5C != business.previousPetCC4888) {
        WindowPetDetail_InitializeDraft(&callbackWindow);
    }
    business.previousPetCC4888 = business.selectedPetCC3E5C;
    if (WindowPetDetail_HasPetSequenceChanges(*callbackWindow.ownerSession) != 0 || WindowPetDetail_ReadDraftFlag() != 0) {
        static_cast<void>(WindowPetDetail_SavePetSequences(*callbackWindow.ownerSession));
        WindowPetDetail_InitializeDraft(&callbackWindow);
        WindowPetDetail_ResetDraftFlag();
        inputs.petNamePrototype.text = business.petNameDraftD45E74;
        WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.petNamePrototype, x, y, 0));
    }
    const auto selected = std::bit_cast<std::int16_t>(
        world::gPetPairsDE0B08[business.selectedPetCC3E5C].index);
    if (world::gPetRuntimeRecords[selected].used == 0) {
        currentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    static_cast<void>(WindowPetDetail_Refresh(&callbackWindow));
    return 1;
}

// 原函数：0x0048DCC0。
static void WindowPetDetail_Release(WindowInstance& releasedWindow)
{
    WindowInstance* const window = WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PetDetail].window;
    if (window != nullptr) {
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PetAttributes].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PetAttributes].y = window->y;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PetSkills].x = window->x;
        WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_PetSkills].y = window->y;
    }
}

// 原函数：0x0048DD80。
static std::int32_t WindowPetDetail_HandleButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    const auto setImage = [elementId, currentWindow](const std::int32_t image) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = image;
    };
    const auto petIndex = [](const std::int32_t position) {
        return std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[position].index);
    };
    switch (static_cast<WindowPetDetailElementId>(elementId)) {
    case WindowPetDetailElementId::HandleButtons:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3B570);
            uiFrame.hoverText = kPetRenameHint586D38;
            consumed = 1;
        } else {
            setImage(0x3B56E);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::wstring text = input::ContextText(inputs.petName);
            const std::wstring escapedText = input::EscapeTextAndNotify(text, 256);
            const std::int32_t position = business.selectedPetCC3E5C;
            const auto networkSession = callbackWindow.ownerSession->Connection().CaptureSession();
            const std::int16_t pet = petIndex(position);
            static_cast<void>(callbackWindow.ownerSession->Connection().RenamePetRequest(pet, escapedText));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
            WindowChat_RestoreInput(*callbackWindow.ownerSession);
            if (text.empty()) {
                const std::int16_t selected = petIndex(business.selectedPetCC3E5C);
                const wchar_t* const name = world::gPetRuntimeRecords[selected].name6AC.data();
                business.petNameDraftD45E74 = name;
                auto* const element10 = currentWindow->FindElementById(WindowPetDetailElementId::Element10);
                if (element10) {
                    auto* const data = element10->data.text;
                    data->text = name;
                }
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B56F);
        }
        return consumed;
    case WindowPetDetailElementId::HandleButtons_7:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(0x3BAF9);
            uiFrame.hoverText = kPetPlaceHint586C14;
            consumed = 1;
        } else {
            setImage(0x3BAF7);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (world::gPetPairsDE0B08[business.selectedPetCC3E5C].used != 0
                && (world::ReadPlayerActionFlags(*callbackWindow.ownerSession) & 0x80u) == 0
                && world::IsObservationSceneTransitionActive_440B50() == 0) {
                const std::int16_t pet = petIndex(business.selectedPetCC3E5C);
                const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
                const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
                const auto networkSession = callbackWindow.ownerSession->Connection().CaptureSession();
                static_cast<void>(callbackWindow.ownerSession->Connection().DropPetRequest(x, y, pet));
                static_cast<void>(audio::PlaySe_52C450(79, 320));
                const std::int16_t selected = petIndex(business.selectedPetCC3E5C);
                if (world::gPetRuntimeRecords[selected].flags6AA == 2) {
                    WindowCraft_ResetItemSelection(*callbackWindow.ownerSession);
                }
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3BAF8);
        }
        return consumed;
    case WindowPetDetailElementId::HandleButtons_8:
    case WindowPetDetailElementId::HandleButtons_9:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowPetDetailElementId::HandleButtons_8) ? 0x3B558 : 0x3B55B);
            uiFrame.hoverText = kPetSelectHint586D2C;
            consumed = 1;
        } else {
            setImage(elementId == static_cast<ElementId>(WindowPetDetailElementId::HandleButtons_8) ? 0x3B556 : 0x3B559);
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            std::int32_t position = business.selectedPetCC3E5C;
            for (std::int32_t attempt = 0; attempt < 6; ++attempt) {
                if (elementId == static_cast<ElementId>(WindowPetDetailElementId::HandleButtons_8)) {
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
            business.selectedPetCC3E5C = position;
            const auto& pet = world::gPetRuntimeRecords[selected];
            const wchar_t* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            inputs.petNamePrototype.text = name;
            WindowInstance* const window = currentWindow;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.petNamePrototype, x, y, 0));
            static_cast<void>(WindowPetDetail_Update(*currentWindow, -1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowPetDetailElementId::HandleButtons_8) ? 0x3B557 : 0x3B55A);
        }
        return consumed;
    case WindowPetDetailElementId::HandleButtons_65:
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t direction = business.valueD673D0 + 1;
            business.valueD673D0 = direction;
            if (direction > 7) {
                business.valueD673D0 = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_RRepeatDown) != 0) {
            const std::int32_t direction = business.valueD673D0 - 1;
            business.valueD673D0 = direction;
            if (direction < 0) {
                business.valueD673D0 = 7;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        return consumed;
    default:
        return 0;
    }
}

// 原函数：0x0048F380。
std::int32_t WindowPetDetail_EditName(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if ((state & MouseFlag_LDownEvent) == 0) {
        return 0;
    }
    inputs.petNamePrototype.text = business.petNameDraftD45E74;
    WindowInstance* const window = currentWindow;
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.petNamePrototype, x, y, 0));
    input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.petName);
    WindowInstance* const focused = currentWindow;
    auto* const indexedElement = focused->FindElementById(elementId);
    if (indexedElement) {
        callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = indexedElement->data.input;
    }
    if (auto* const indexedElement2 = focused->FindElementById(elementId); indexedElement2) indexedElement2->data.input->context = &inputs.petName;
    return 1;
}

// 原函数：0x0048F410。
std::int32_t WindowPetDetail_SwitchPage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t target;
    std::int32_t idle;
    std::int32_t hover;
    const wchar_t* hint;
    switch (static_cast<WindowPetDetailElementId>(elementId)) {
    case WindowPetDetailElementId::Element3:
        target = 16; idle = 0x3BAF4; hover = 0x3BAF6; hint = L"切换至此宠物的能力视窗。" /* 原常量参考：0x00585958。 */;
        break;
    case WindowPetDetailElementId::SwitchPage:
        target = 17; idle = 0x3B583; hover = 0x3B585; hint = L"切换至此宠物的详细视窗。" /* 原常量参考：0x0058593C。 */;
        break;
    case WindowPetDetailElementId::SwitchEquipment:
        target = WindowId_PetEquipment; idle = 244474; hover = 244476; hint = L"切换至此宠物的装备视窗。";
        break;
    case WindowPetDetailElementId::SwitchPage_5:
        target = 18; idle = 0x3BAF1; hover = 0x3BAF3; hint = L"切换至此宠物的技能视窗。" /* 原常量参考：0x00585920。 */;
        break;
    default:
        return 0;
    }
    std::int32_t consumed = 0;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = target == WindowId_PetEquipment && (state & MouseFlag_LHoldPhysical) != 0
                ? 244475 : hover;
            uiFrame.hoverText = hint;
            consumed = 1;
        } else {
            graphic->imageNo = idle;
        }
    }
    if ((state & MouseFlag_LDownEvent) != 0) {
        WindowInstance* const targetWindow = WindowRuntime_Open(*callbackWindow.ownerSession, target);
        if (!targetWindow) return 1;
        currentWindow->state |= 0x8000u;
        targetWindow->x = currentWindow->x;
        targetWindow->y = currentWindow->y;
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        WindowRuntime_QueuePagePresentation(*currentWindow, *targetWindow);
        return 1;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
