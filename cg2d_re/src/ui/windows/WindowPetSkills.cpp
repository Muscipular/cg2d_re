#include "cg/network/CgConnection.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../WindowExtension.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"
#include <algorithm>

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowPetSkillsElementId : ElementId {
    EditName = 1,
    ButtonClose = 2,
    SwitchPage = 3,
    SwitchPage_4 = 4,
    Element5 = 5,
    HandleButtons = 6,
    HandleButtons_7 = 7,
    HandleButtons_8 = 8,
    HandleButtons_9 = 9,
    HandleButtons_10 = 10,
    HandleButtons_11 = 11,
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
    HandleSkillRow = 38,
    HandleSkillRow_39 = 39,
    HandleSkillRow_40 = 40,
    HandleSkillRow_41 = 41,
    HandleSkillRow_42 = 42,
    HandleSkillRow_43 = 43,
    HandleSkillRow_44 = 44,
    HandleSkillRow_45 = 45,
    HandleSkillRow_46 = 46,
    HandleSkillRow_47 = 47,
    Element48 = 48,
    HandleScroll = 49,
    Element50 = 50,
    Element51 = 51,
    Element52 = 52,
    HandleSideArea = 53,
    SwitchEquipment = 67, // 四个宠物详情页的装备页签统一ID。
    HandleMouseWheel = 68, // 技能列表及滚动条范围的滚轮事件区域。
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0048ECA0。
static std::int32_t WindowPetSkills_HandleButtons(ElementId elementId, std::int32_t state); // 宠物技能页六按钮。

// 原函数：0x0048F6C0。
static std::int32_t WindowPetSkills_HandleSkillRow(ElementId elementId, std::int32_t state); // 描述、技能换位与拖动。

// 新增宠物技能滑块：复用垂直滚动处理，将位置同步到十行技能列表。
static std::int32_t WindowPetSkills_HandleScroll(ElementId elementId, std::int32_t state);

// 列表悬停时消费共享滚轮，每格滚动一行并同步现有滑块。
static std::int32_t WindowPetSkills_HandleMouseWheel(ElementId elementId, std::int32_t state);

// 原函数：0x0048DBB0。
static std::int32_t WindowPetSkills_Update(std::int32_t value);

// 原函数：0x0048DCB0。
static std::int32_t WindowPetSkills_Draw(std::int32_t value);

// 原函数：0x0048DD40。
static void WindowPetSkills_Release();

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00D66C34。 动态宽文本，控件借稳定owner。
static std::array<std::wstring, 10> gPetTechniqueCostsD66C34 {}; // 原全局：0x00D66C34。
// 普通宠物技能页保留10个可见行；描述及拖动索引均加上该滚动起点。
static std::int32_t gPetTechniqueScroll = 0;
static WindowGraphicData gPetTechniqueScrollThumb {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 复用技能窗口滑块图。
static std::uint32_t gPetTechniqueScrollMouseFlags = 0; // MouseArea原型初始标记，运行态由窗口持有。
constexpr std::int32_t kPetTechniqueScrollThumbElement = 47;
constexpr std::int32_t kPetTechniqueScrollTrackElement = 48;

static WindowGraphicData gWindowGraphic565698 {0x3BAEE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565698。

static WindowGraphicData gWindowGraphic5658A8 {0x3B5B7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005658A8。

static WindowGraphicData gWindowGraphic565908 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565908。

static WindowGraphicData gWindowGraphic565920 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565920。

static WindowTextPrototype gWindowText565990 { // 原全局：0x00565990。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"技能", // 原常量参考：0x00586004。
};

static WindowTextPrototype gWindowText5659A0 { // 原全局：0x005659A0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"进行攻击", // 原常量参考：0x00585FF4。
};

static WindowNumberData gWindowNumbers5667C0[10] { // 原全局：0x005667C0。
    {&gPetTechniqueCostsD66C34[0], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[1], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[2], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[3], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[4], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[5], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[6], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[7], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[8], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[9], 0, 0, 0, 0},
};

static WindowElementPrototype gWindowElements566810[55] { // 原全局：0x00566810；扩充滑块、装备页签与滚轮区域。
    {WindowElementType_EventArea, 20, 50, 216, 160, 1, {.unusedValue = 0}, &WindowPetSkills_HandleMouseWheel, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleMouseWheel)},
    {WindowElementType_Graphic, 186, 275, 35, 25, 1, {.graphic = &gPetEquipmentPageGraphic}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetSkillsElementId::SwitchEquipment)},
    {WindowElementType_Input, 66, 31, 96, 12, 1, {.unusedValue = 0}, &WindowPetDetail_EditName, -1, static_cast<ElementId>(WindowPetSkillsElementId::EditName)},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPetSkillsElementId::ButtonClose)},
    {WindowElementType_Graphic, 27, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetSkillsElementId::SwitchPage)},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetSkillsElementId::SwitchPage_4)},
    {WindowElementType_Graphic, 133, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element5)},
    {WindowElementType_Graphic, 187, 28, 32, 16, 1, {.graphic = &gWindowGraphic565650}, &WindowPetSkills_HandleButtons, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons)},
    {WindowElementType_Graphic, 175, 253, 49, 15, 1, {.graphic = &gWindowGraphic5658C0}, &WindowPetSkills_HandleButtons, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_7)},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetSkills_HandleButtons, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_8)},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetSkills_HandleButtons, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_9)},
    {WindowElementType_Graphic, 225, 50, 11, 11, 1, {.graphic = &gWindowGraphic565908}, &WindowPetSkills_HandleButtons, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_10)},
    {WindowElementType_Graphic, 225, 195, 11, 11, 1, {.graphic = &gWindowGraphic565920}, &WindowPetSkills_HandleButtons, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_11)},
    {WindowElementType_Text, 66, 31, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element12)},
    {WindowElementType_Text, 44, 256, 0, 0, 1, {.text = &gWindowText565988}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element13)},
    {WindowElementType_Text, 20, 50, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element14)},
    {WindowElementType_Text, 20, 66, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element15)},
    {WindowElementType_Text, 20, 82, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element16)},
    {WindowElementType_Text, 20, 98, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element17)},
    {WindowElementType_Text, 20, 114, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element18)},
    {WindowElementType_Text, 20, 130, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element19)},
    {WindowElementType_Text, 20, 146, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element20)},
    {WindowElementType_Text, 20, 162, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element21)},
    {WindowElementType_Text, 20, 178, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element22)},
    {WindowElementType_Text, 20, 194, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element23)},
    {WindowElementType_Number, 201, 51, 0, 0, 1, {.number = &gWindowNumbers5667C0[0]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element24)},
    {WindowElementType_Number, 201, 67, 0, 0, 1, {.number = &gWindowNumbers5667C0[1]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element25)},
    {WindowElementType_Number, 201, 83, 0, 0, 1, {.number = &gWindowNumbers5667C0[2]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element26)},
    {WindowElementType_Number, 201, 99, 0, 0, 1, {.number = &gWindowNumbers5667C0[3]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element27)},
    {WindowElementType_Number, 201, 115, 0, 0, 1, {.number = &gWindowNumbers5667C0[4]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element28)},
    {WindowElementType_Number, 201, 131, 0, 0, 1, {.number = &gWindowNumbers5667C0[5]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element29)},
    {WindowElementType_Number, 201, 147, 0, 0, 1, {.number = &gWindowNumbers5667C0[6]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element30)},
    {WindowElementType_Number, 201, 163, 0, 0, 1, {.number = &gWindowNumbers5667C0[7]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element31)},
    {WindowElementType_Number, 201, 179, 0, 0, 1, {.number = &gWindowNumbers5667C0[8]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element32)},
    {WindowElementType_Number, 201, 195, 0, 0, 1, {.number = &gWindowNumbers5667C0[9]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element33)},
    {WindowElementType_Text, 25, 213, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element34)},
    {WindowElementType_Text, 25, 225, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element35)},
    {WindowElementType_Text, 25, 237, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element36)},
    {WindowElementType_Text, 25, 249, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element37)},
    {WindowElementType_Graphic, 17, 48, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow)},
    {WindowElementType_Graphic, 17, 64, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow_39)},
    {WindowElementType_Graphic, 17, 80, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow_40)},
    {WindowElementType_Graphic, 17, 96, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow_41)},
    {WindowElementType_Graphic, 17, 112, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow_42)},
    {WindowElementType_Graphic, 17, 128, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow_43)},
    {WindowElementType_Graphic, 17, 144, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow_44)},
    {WindowElementType_Graphic, 17, 160, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow_45)},
    {WindowElementType_Graphic, 17, 176, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow_46)},
    {WindowElementType_Graphic, 17, 192, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow_47)},
    // 同层图形按提交索引降序绘制；滑块必须先于覆盖其区域的0x3BAEE面板提交。
    {WindowElementType_Graphic, 225, 61, 0, 14, 0, {.graphic = &gPetTechniqueScrollThumb}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element48)},
    {WindowElementType_MouseArea, 225, 61, 11, 134, 0, {.mouseFlags = &gPetTechniqueScrollMouseFlags}, &WindowPetSkills_HandleScroll, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleScroll)},
    {WindowElementType_Graphic, 17, 32, 0, 0, 1, {.graphic = &gWindowGraphic565698}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element50)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic565608}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element51)},
    {WindowElementType_Graphic, 14, 25, 0, 0, 1, {.graphic = &gWindowGraphic565620}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetSkillsElementId::Element52)},
    {WindowElementType_EventArea, 250, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPetSkillsElementId::HandleSideArea)},
};

// 原型：窗口18；原全局：0x0054E5D0。
WindowPrototype gWindowPrototypePetSkills {0, 4, 365, 83, 245, 306, 0x80010101u, 55, gWindowElements566810, &WindowPetSkills_Update, &WindowPetSkills_Draw, &WindowPetSkills_Release}; // 18: 54E5D0

// 窗口函数实现。

std::int32_t WindowPetSkills_GetPreviewPage(const void* const identity, const std::int32_t pageCount)
{
    static const void* previousIdentity = nullptr; // 仅比较身份，不解引用或拥有记录。
    static std::int32_t previousPageCount = 0;
    static std::int32_t page = 0;
    if (previousIdentity != identity || previousPageCount != pageCount) {
        previousIdentity = identity;
        previousPageCount = pageCount;
        page = 0;
    }
    if ((platform::gKeyStates[0x21] & 4u) != 0 && page > 0) { // PageUp。
        --page;
    }
    if ((platform::gKeyStates[0x22] & 4u) != 0 && page + 1 < pageCount) { // PageDown。
        ++page;
    }
    return page;
}

// 原函数：0x00491AA0。
static WindowElement* WindowPetSkills_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return nullptr;
    const auto selectedPet = []() -> world::PetRuntimeRecord& {
        const auto index = std::bit_cast<std::int16_t>(
            world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
        return world::gPetRuntimeRecords[index];
    };
    const auto& initialPet = selectedPet();
    std::wstring buffer;
    if (initialPet.used != 0) {
        const bool customName = initialPet.name6BD[0] != '\0';
        input::InputContext* const activeInput = input::gCurrentInputData;
        if (activeInput == &cg::ui::detail::gInputContextD2D4A0) {
            if (auto* const element12 = currentWindow->FindElementById(WindowPetSkillsElementId::Element12); element12) element12->visible = 0;
            const std::wstring text = input::ContextText(*gPetNameInputPrototype57DCF8.input.context);
            WindowInstance* window;
            if (customName) {
                gLoginWindowRuntime.petNameDraftD45E74 = text;
                window = currentWindow;
            } else {
                window = currentWindow;
                gLoginWindowRuntime.petNameDraftD45E74 = text;
            }
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 1));
        } else {
            if (auto* const element12 = currentWindow->FindElementById(WindowPetSkillsElementId::Element12); element12) element12->visible = 1;
            auto* const element122 = currentWindow->FindElementById(WindowPetSkillsElementId::Element12);
            if (element122) {
                auto* const data = element122->data.text;
                data->text = gLoginWindowRuntime.petNameDraftD45E74;
            }
            const auto& compared = selectedPet();
            const wchar_t* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
            const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(name) != 0;
            if (auto* const handleButtonsElement = currentWindow->FindElementById(WindowPetSkillsElementId::HandleButtons); handleButtonsElement) handleButtonsElement->visible = changed ? 1 : 0;
            auto* const element123 = currentWindow->FindElementById(WindowPetSkillsElementId::Element12);
            if (element123) {
                auto* const colorData = element123->data.text;
                colorData->style.color = changed ? std::byte{6} : std::byte{0};
            }
        }
        const auto& compared = selectedPet();
        const wchar_t* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
        const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(name) != 0;
        if (auto* const handleButtonsElement = currentWindow->FindElementById(WindowPetSkillsElementId::HandleButtons); handleButtonsElement) handleButtonsElement->visible = changed ? 1 : 0;

        for (std::int32_t row = 0; row < 10; ++row) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(37 + row)); indexedElement) indexedElement->visible = 0;
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(13 + row)); indexedElement2) indexedElement2->visible = 0;
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(23 + row)); indexedElement3) indexedElement3->visible = 0;
        }
        const std::int32_t slotCount = std::clamp(selectedPet().value6A4, 0, world::kPetTechniqueSlotCount);
        const std::int32_t maximum = std::max(0, slotCount - 10);
        gPetTechniqueScroll = std::clamp(gPetTechniqueScroll, 0, maximum);
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(kPetTechniqueScrollThumbElement)); indexedElement) indexedElement->visible = maximum > 0 ? 1 : 0;
        auto* const trackElement = currentWindow->FindElementById(NativeElementKey(kPetTechniqueScrollTrackElement));
        if (trackElement) {
            WindowElement& track = (*trackElement);
            track.visible = maximum > 0 ? 1 : 0;
            auto* const mouse = track.data.mouse;
            if (maximum == 0) {
                mouse->flags = 0;
            }
            if ((mouse->flags & 2u) == 0) {
                static_cast<void>(WindowScroll_SetPosition(&track, maximum, gPetTechniqueScroll));
            }
        }
        for (std::int32_t shown = 0; shown < std::min(10, slotCount - gPetTechniqueScroll); ++shown) {
            if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(37 + shown)); indexedElement2) indexedElement2->visible = 1;
        }
        for (std::int32_t row = 0; row < 10; ++row) {
            const std::int32_t techniqueIndex = gPetTechniqueScroll + row;
            if (selectedPet().techniquesD8[techniqueIndex].used != 0) {
                if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(13 + row)); indexedElement2) indexedElement2->visible = 1;
                const auto& techniqueName = selectedPet().techniquesD8[techniqueIndex].name;
                auto* const dataElement = currentWindow->FindElementById(NativeElementKey(13 + row));
                if (dataElement) {
                    auto* const data = dataElement->data.text;
                    data->text = techniqueName;
                }
                if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(23 + row)); indexedElement3) indexedElement3->visible = 1;
                const std::int32_t fp = selectedPet().techniquesD8[techniqueIndex].fp;
                cg::text::FormatTo(buffer, L"{:3}", fp);
                gPetTechniqueCostsD66C34[row] = buffer;
            }
        }
    } else {
        if (auto* const element12 = currentWindow->FindElementById(WindowPetSkillsElementId::Element12); element12) element12->visible = 0;
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(kPetTechniqueScrollThumbElement)); indexedElement) indexedElement->visible = 0;
        if (auto* const indexedElement2 = currentWindow->FindElementById(NativeElementKey(kPetTechniqueScrollTrackElement)); indexedElement2) indexedElement2->visible = 0;
        for (std::int32_t row = 0; row < 10; ++row) {
            if (auto* const indexedElement3 = currentWindow->FindElementById(NativeElementKey(37 + row)); indexedElement3) indexedElement3->visible = 0;
            if (auto* const indexedElement4 = currentWindow->FindElementById(NativeElementKey(13 + row)); indexedElement4) indexedElement4->visible = 0;
            if (auto* const indexedElement5 = currentWindow->FindElementById(NativeElementKey(23 + row)); indexedElement5) indexedElement5->visible = 0;
        }
    }
    cg::text::FormatTo(buffer, L"{}/{}", gLoginWindowRuntime.selectedPetCC3E5C + 1, 5);
    auto* const element13 = currentWindow->FindElementById(WindowPetSkillsElementId::Element13);
    if (element13) {
        auto* const page = element13->data.text;
        page->text = buffer;
    }
    if (auto* const element34 = currentWindow->FindElementById(WindowPetSkillsElementId::Element34); element34) element34->visible = 0;
    if (auto* const element35 = currentWindow->FindElementById(WindowPetSkillsElementId::Element35); element35) element35->visible = 0;
    auto* resultWindow = currentWindow;
    if (auto* const element36 = resultWindow->FindElementById(WindowPetSkillsElementId::Element36); element36) element36->visible = 0;
    if (auto* const element37 = currentWindow->FindElementById(WindowPetSkillsElementId::Element37); element37) element37->visible = 0;
    return resultWindow->FindElementById(WindowPetSkillsElementId::EditName);
}

static std::int32_t WindowPetSkills_HandleMouseWheel(const ElementId, const std::int32_t state)
{
    if ((state & MouseFlag_Hover) == 0 || WindowDrag_ReadMousePhase() != 0) return 0;
    const std::int32_t wheel = WindowPeekMouseWheelDelta();
    if (wheel == 0) return 0;
    const auto selected = std::bit_cast<std::int16_t>(
        world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const std::int32_t slotCount = std::clamp(world::gPetRuntimeRecords[selected].value6A4, 0, world::kPetTechniqueSlotCount);
    const std::int32_t step = wheel / 120 != 0 ? wheel / 120 : wheel > 0 ? 1 : -1;
    gPetTechniqueScroll = std::clamp(gPetTechniqueScroll - step, 0, std::max(0, slotCount - 10));
    WindowConsumeMouseWheelDelta();
    static_cast<void>(WindowPetSkills_Refresh());
    return 1;
}

static std::int32_t WindowPetSkills_HandleScroll(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t consumed = WindowElement_HandleVerticalScroll(elementId, state);
    auto* const trackElement = currentWindow->FindElementById(elementId);
    if (trackElement) {
        WindowElement& track = (*trackElement);
        const auto* const mouse = track.data.mouse;
        if ((mouse->flags & 1u) != 0) {
            const auto selected = std::bit_cast<std::int16_t>(
                world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
            const std::int32_t slotCount = std::clamp(world::gPetRuntimeRecords[selected].value6A4, 0, world::kPetTechniqueSlotCount);
            gPetTechniqueScroll = WindowScroll_ReadPosition(&track, std::max(0, slotCount - 10));
            static_cast<void>(WindowPetSkills_Refresh());
        }
    }
    return consumed;
}

// 原函数：0x004920F0。
static std::int32_t WindowPetSkills_DrawDraggedSkillTooltip()
{
    if (gLoginWindowRuntime.petTechniqueDragD460CC == 0) {
        return 0;
    }
    const std::int32_t x = (platform::gLegacyMouseState.mouseX - 105);
    const std::int32_t y = (platform::gLegacyMouseState.mouseY - 8);
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(0, 0, 5, 12, 0, L"", 0, 0));
    const auto namePet = std::bit_cast<std::int16_t>(
        world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const std::int32_t nameTechnique = gLoginWindowRuntime.petTechniqueD2D66C;
    static_cast<void>(render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 5)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
        5, 12, 0, world::gPetRuntimeRecords[namePet].techniquesD8[nameTechnique].name,
        0, 0));
    const auto costPet = std::bit_cast<std::int16_t>(
        world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const std::int32_t costTechnique = gLoginWindowRuntime.petTechniqueD2D66C;
    const std::int32_t fp = world::gPetRuntimeRecords[costPet].techniquesD8[costTechnique].fp;
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", fp);
    static_cast<void>(render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 187)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
        5, 12, 0, buffer, 0, 0));
    const auto graphicY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const auto graphicX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return cg::render::DrawGraphic(graphicX + 105, graphicY + 8, 230, 0x3B5B7, 0, &descriptor);
}

// 原函数：0x0048DBB0。
static std::int32_t WindowPetSkills_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        gPetTechniqueScroll = 0;
        static_cast<void>(WindowPetDetail_SavePetSequences());
        WindowPetDetail_InitializeDraft();
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        return 1;
    }
    if (gLoginWindowRuntime.selectedPetCC3E5C != gLoginWindowRuntime.previousPetCC4888) {
        gPetTechniqueScroll = 0;
        if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(kPetTechniqueScrollTrackElement)); indexedElement) indexedElement->data.mouse->flags = 0;
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
    static_cast<void>(WindowPetSkills_Refresh());
    return 1;
}

// 原函数：0x0048DCB0。
static std::int32_t WindowPetSkills_Draw(const std::int32_t)
{
    static_cast<void>(WindowPetSkills_DrawDraggedSkillTooltip());
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x0048DD40。
static void WindowPetSkills_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_PetSkills].window;
    if (window != nullptr) {
        gWindowStates[WindowId_PetDetail].x = window->x;
        gWindowStates[WindowId_PetDetail].y = window->y;
        gWindowStates[WindowId_PetAttributes].x = window->x;
        gWindowStates[WindowId_PetAttributes].y = window->y;
    }
}

// 原函数：0x0048ECA0。
static std::int32_t WindowPetSkills_HandleButtons(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    const auto setImage = [elementId, currentWindow](const std::int32_t image) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = image;
    };
    const auto petIndex = [](const std::int32_t position) {
        return std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[position].index);
    };
    switch (static_cast<WindowPetSkillsElementId>(elementId)) {
    case WindowPetSkillsElementId::HandleButtons:
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
                auto* const element12 = currentWindow->FindElementById(WindowPetSkillsElementId::Element12);
                if (element12) {
                    auto* const data = element12->data.text;
                    data->text = name;
                }
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(0x3B56F);
        }
        return consumed;
    case WindowPetSkillsElementId::HandleButtons_7:
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
    case WindowPetSkillsElementId::HandleButtons_8:
    case WindowPetSkillsElementId::HandleButtons_9: {
        if ((state & MouseFlag_Hover) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_8) ? 0x3B558 : 0x3B55B);
            gHoverTextD68AE8 = kPetSelectHint586D2C;
            consumed = 1;
        } else {
            setImage(elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_8) ? 0x3B556 : 0x3B559);
        }
        // 下一只按钮即使未点击也检查>=5；上一只只在点击后取选择。
        if (elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_9) || (state & MouseFlag_LDownEvent) != 0) {
            std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
            if ((state & MouseFlag_LDownEvent) != 0 || position >= 5) {
                for (std::int32_t attempt = 0; attempt < 6; ++attempt) {
                    if (elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_8)) {
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
                std::int16_t selected;
                if (elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_8)) {
                    selected = petIndex(position);
                    gLoginWindowRuntime.selectedPetCC3E5C = position;
                } else {
                    gLoginWindowRuntime.selectedPetCC3E5C = position;
                    selected = petIndex(position);
                }
                const auto& pet = world::gPetRuntimeRecords[selected];
                const wchar_t* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                gPetNameInputPrototype57DCF8.text = name;
                WindowInstance* const window = currentWindow;
                const std::int32_t y = window->y;
                const std::int32_t x = window->x;
                static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
                static_cast<void>(WindowPetSkills_Update(-1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                consumed = 1;
            }
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_8) ? 0x3B557 : 0x3B55A);
        }
        return consumed;
    }
    case WindowPetSkillsElementId::HandleButtons_10:
    case WindowPetSkillsElementId::HandleButtons_11:
        if ((state & MouseFlag_Hover) != 0) {
            setImage(elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_10) ? 0x3B594 : 0x3B597);
        } else {
            setImage(elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_10) ? 0x3B592 : 0x3B595);
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const auto selected = petIndex(gLoginWindowRuntime.selectedPetCC3E5C);
            const std::int32_t slotCount = std::clamp(world::gPetRuntimeRecords[selected].value6A4, 0, world::kPetTechniqueSlotCount);
            const std::int32_t position = std::clamp(gPetTechniqueScroll + (elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_10) ? -1 : 1), 0, std::max(0, slotCount - 10));
            static_cast<void>(audio::PlaySe_52C450(position != gPetTechniqueScroll ? 51 : 55, 320));
            gPetTechniqueScroll = position;
            setImage(elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleButtons_10) ? 0x3B593 : 0x3B596);
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

// 原函数：0x0048F620。
static void WindowPetSkills_HandleDrag(const std::int32_t mode, const std::int32_t, const std::int32_t source)
{
    if (mode != 0) {
        if (mode > 0 && mode <= 4) {
            gLoginWindowRuntime.petTechniqueDragD460CC = 0;
            gLoginWindowRuntime.petTechniqueD2D66C = -1;
            gLoginWindowRuntime.petTechniqueValueD2F784 = -1;
        }
        return;
    }
    if ((platform::gLegacyMouseState.edgeButtons & platform::LegacyMouseEdge_RDown) != 0) {
        static_cast<void>(WindowDrag_ResetMouse());
        const auto x = static_cast<std::int16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX));
        const auto y = static_cast<std::int16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY));
        static_cast<void>(WindowDrag_StartAtPosition(11, source, x, y, WindowPetSkills_HandleDrag));
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
}

// 原函数：0x0048F690。
static std::int32_t WindowPetSkills_StartDrag(const std::int32_t source)
{
    gLoginWindowRuntime.petTechniqueD2D66C = source;
    gLoginWindowRuntime.petTechniqueDragD460CC = 1;
    return WindowDrag_Start(11, source, WindowPetSkills_HandleDrag);
}

// 原函数：0x0048F6C0。
static std::int32_t WindowPetSkills_HandleSkillRow(const ElementId elementId, const std::int32_t state)
{
    const std::int32_t index = static_cast<std::int32_t>(elementId) - 1; // 原生业务索引，独立于元素排序。
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    std::uint32_t events = static_cast<std::uint32_t>(state);
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind() != 11) {
        events &= ~(MouseFlag_Dragging | MouseFlag_DragFinish);
    }
    if (elementId == static_cast<ElementId>(WindowPetSkillsElementId::HandleSkillRow)) {
        for (std::int32_t line = 0; line < 4; ++line) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(33 + line)); indexedElement) indexedElement->visible = 0;
        }
    }
    const bool hover = (events & MouseFlag_Hover) != 0;
    if (hover) {
        for (std::int32_t line = 0; line < 4; ++line) {
            if (auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(33 + line)); indexedElement) indexedElement->visible = 1;
        }
        std::wstring buffer;
        for (std::int32_t line = 0; line < 4; ++line) {
            const auto selected = std::bit_cast<std::int16_t>(
                world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
            const auto& pet = world::gPetRuntimeRecords[selected];
            const auto technique = std::bit_cast<std::int16_t>(pet.techniquePairs650[gPetTechniqueScroll + index - 37].index);
            const auto& description = pet.techniquesD8[technique].description;
            if (input::ExtractTextLine(buffer, description, line, 26) != 0) {
                auto* elementsWindow = currentWindow;
                auto* const dataElement = elementsWindow->FindElementById(NativeElementKey(33 + line));
                if (dataElement) {
                    auto* const data = dataElement->data.text;
                    data->text = buffer;
                }
            }
        }
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        const std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
        gLoginWindowRuntime.petTechniqueValueD2F784 = gPetTechniqueScroll + index - 37;
        gLoginWindowRuntime.petTechniqueD2D66C = source;
        const auto selected = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[position].index);
        const auto& pet = world::gPetRuntimeRecords[selected];
        const auto targetTechnique = std::bit_cast<std::int16_t>(pet.techniquePairs650[gPetTechniqueScroll + index - 37].index);
        const auto sourceTechnique = std::bit_cast<std::int16_t>(pet.techniquePairs650[source].index);
        const auto networkSession = cg::network::GetGameConnection().CaptureSession();
        static_cast<void>(cg::network::GetGameConnection().SwapPetSkillOrderRequest(selected, sourceTechnique, targetTechnique));
        WindowDrag_Reset();
    }
    auto* const indexedElement = currentWindow->FindElementById(NativeElementKey(index - 24));
    if (indexedElement) {
        if ((events & MouseFlag_RDownEvent) != 0 && hover && indexedElement->visible != 0) {
            static_cast<void>(WindowPetSkills_StartDrag(gPetTechniqueScroll + index - 37));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
    }
    return hover ? 1 : 0;
}

} // namespace detail

} // namespace cg::ui
