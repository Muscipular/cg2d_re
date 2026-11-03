#include "cg/network/CgConnection.hpp"
#include "cg/ui/HitTest.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"
#include <iterator>

namespace cg::ui {

// 页签ID3／4／5／67与WindowPetDetail_SwitchPage共用，其余仅属于装备页。
enum class WindowPetEquipmentElementId : ElementId {
    Name = 1,
    ButtonClose = 2,
    TabStatus = 3,
    TabAttributes = 4,
    TabSkills = 5,
    EquipmentArea = 6,
    NameLabel = 7,
    PreviousPet = 8,
    NextPet = 9,
    PetNumber = 10,
    CrystalSlot = 11,
    ArmorSlot = 12,
    CollarSlot = 13,
    AccessorySlot1 = 14,
    AccessorySlot2 = 15,
    CrystalLabel = 17,
    ArmorLabel = 18,
    CollarLabel = 19,
    AccessoryLabel1 = 20,
    AccessoryLabel2 = 21,
    Frame = 23,
    Panel = 24,
    SideArea = 25,
    TabEquipment = 67,
};

namespace detail {

// 同步宠物名字、排序位置及说明状态，宠物消失时关闭装备页。
static std::int32_t WindowPetEquipment_Update(std::int32_t value);
// 共用物品提示和宠物外框，绘制本页五个装备槽。
static std::int32_t WindowPetEquipment_Draw(std::int32_t value);
// 将装备页位置同步给另外三个宠物详情页。
static void WindowPetEquipment_Release();
// 前后跳过空宠物槽，沿用详情页的当前排序位置。
static std::int32_t WindowPetEquipment_SelectPet(ElementId elementId, std::int32_t state);
// 背包装入、双击卸下及右键说明翻页；装备变动交给既有Cpa回包。
static std::int32_t WindowPetEquipment_HandleItems(ElementId elementId, std::int32_t state);

// 将原装备栏的两列三层中心坐标平移到宠物框，图标、槽底及鼠标命中保持同源。
static constexpr std::int32_t kPetEquipmentOriginX = 22;
static constexpr std::int32_t kPetEquipmentOriginY = 55;
// 五个实例分别裁出图98147的对应装备格，初始化时按共享中心表填写源偏移。
static WindowGraphicData gPetEquipmentSlotGraphic {98147, 0, 0, 48, 48, 0xFFFFFFFFu};

WindowGraphicData gPetEquipmentPageGraphic {244474, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gPetEquipmentActivePageGraphic {244475, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowTextPrototype gPetEquipmentName {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L""};
static WindowTextPrototype gPetEquipmentNumber {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"1/5"};
// 本页只展示原槽0～4，饰品3（槽5）不参与绘制或命中。
static WindowTextPrototype gPetEquipmentLabels[] {
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"晶石"},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"防具"},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"项圈"},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"饰品1"},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"饰品2"},
};

static WindowElementPrototype gPetEquipmentElements[] {
    {WindowElementType_Text, 49, 31, 147, 16, 1, {.text = &gPetEquipmentName}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::Name)},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPetEquipmentElementId::ButtonClose)},
    {WindowElementType_Graphic, 27, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetEquipmentElementId::TabStatus)},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetEquipmentElementId::TabAttributes)},
    {WindowElementType_Graphic, 133, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetEquipmentElementId::TabSkills)},
    {WindowElementType_EventArea, 34, 61, 177, 148, 1, {.unusedValue = 0}, &WindowPetEquipment_HandleItems, -1, static_cast<ElementId>(WindowPetEquipmentElementId::EquipmentArea)},
    {WindowElementType_Graphic, 0, 0, 48, 48, 1, {.graphic = &gPetEquipmentSlotGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::CrystalSlot)},
    {WindowElementType_Graphic, 0, 0, 48, 48, 1, {.graphic = &gPetEquipmentSlotGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::ArmorSlot)},
    {WindowElementType_Graphic, 0, 0, 48, 48, 1, {.graphic = &gPetEquipmentSlotGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::CollarSlot)},
    {WindowElementType_Graphic, 0, 0, 48, 48, 1, {.graphic = &gPetEquipmentSlotGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::AccessorySlot1)},
    {WindowElementType_Graphic, 0, 0, 48, 48, 1, {.graphic = &gPetEquipmentSlotGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::AccessorySlot2)},
    {WindowElementType_Text, 23, 31, 24, 16, 0, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::NameLabel)},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetEquipment_SelectPet, -1, static_cast<ElementId>(WindowPetEquipmentElementId::PreviousPet)},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetEquipment_SelectPet, -1, static_cast<ElementId>(WindowPetEquipmentElementId::NextPet)},
    {WindowElementType_Text, 41, 256, 24, 16, 1, {.text = &gPetEquipmentNumber}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::PetNumber)},
    {WindowElementType_Text, 0, 0, 36, 16, 1, {.text = &gPetEquipmentLabels[0]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::CrystalLabel)},
    {WindowElementType_Text, 0, 0, 36, 16, 1, {.text = &gPetEquipmentLabels[1]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::ArmorLabel)},
    {WindowElementType_Text, 0, 0, 36, 16, 1, {.text = &gPetEquipmentLabels[2]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::CollarLabel)},
    {WindowElementType_Text, 0, 0, 36, 16, 1, {.text = &gPetEquipmentLabels[3]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::AccessoryLabel1)},
    {WindowElementType_Text, 0, 0, 36, 16, 1, {.text = &gPetEquipmentLabels[4]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::AccessoryLabel2)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic565608}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::Frame)},
    {WindowElementType_Graphic, 14, 25, 0, 0, 1, {.graphic = &gWindowGraphic565620}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::Panel)},
    {WindowElementType_EventArea, 250, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPetEquipmentElementId::SideArea)},
    {WindowElementType_Graphic, 186, 275, 35, 25, 1, {.graphic = &gPetEquipmentActivePageGraphic}, &WindowElement_NoOp, 0, static_cast<ElementId>(WindowPetEquipmentElementId::TabEquipment)},
};

// 窗口72：与三页详情共用尺寸和外框，独立保存装备提示状态。
WindowPrototype gWindowPrototypePetEquipment {0, 4, 365, 83, 245, 306, 0x80010101u,
    static_cast<std::int32_t>(std::size(gPetEquipmentElements)), gPetEquipmentElements,
    &WindowPetEquipment_Update, &WindowPetEquipment_Draw, &WindowPetEquipment_Release};

// value10为悬停槽（-1无），value14为说明页，value20记录真实宠物槽；不拥有或复制装备记录。
static std::int32_t WindowPetEquipment_Update(const std::int32_t value)
{
    WindowInstance* const window = gCurrentWindow;
    if (!window) return 0;
    if (value == -1) {
        // NativeAnchor会减去控件宽高；新页面的尺寸表示文字框，不沿用旧锚点。
        WindowElementCursor cursor(*window);
        while (auto* element = cursor.next()) {
            if (element->type != WindowElementType_Text) continue;
            element->data.text->layout = WindowTextLayout::Box;
            element->data.text->align = element->elementId == static_cast<ElementId>(WindowPetEquipmentElementId::NameLabel)
                ? WindowTextAlign::Left : WindowTextAlign::Center;
        }
        for (std::int32_t slot = 0; slot < static_cast<std::int32_t>(std::size(gPetEquipmentLabels)); ++slot) {
            auto* const background = window->FindElementById(static_cast<ElementId>(WindowPetEquipmentElementId::CrystalSlot) + slot);
            if (!background) continue;
            background->x = kPetEquipmentOriginX + gSelectorPetX54F5C4[slot] - 24;
            background->y = kPetEquipmentOriginY + gSelectorPetY54F5DC[slot] - 24;
            background->data.graphic->offsetX = gSelectorPetX54F5C4[slot] - 24;
            background->data.graphic->offsetY = gSelectorPetY54F5DC[slot] - 24;
            if (auto* const label = window->FindElementById(static_cast<ElementId>(WindowPetEquipmentElementId::CrystalLabel) + slot)) {
                label->x = slot % 2 == 0 ? background->x + 52 : background->x - label->width - 4;
                label->y = background->y + 16;
            }
        }
    }
    auto& state = WindowRuntime_States()[WindowId_PetEquipment];
    if (value == -1 && input::gCurrentInputData == &gInputContextD2D4A0) {
        static_cast<void>(WindowChat_RestoreInput());
    }
    const auto petIndex = std::bit_cast<std::int16_t>(
        world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    if (value == -1 || state.value20 != static_cast<std::uint32_t>(petIndex)) {
        state.value10 = static_cast<std::uint32_t>(-1);
        state.value14 = 0;
        state.value20 = static_cast<std::uint32_t>(petIndex);
    }
    const auto& pet = world::gPetRuntimeRecords[petIndex];
    if (pet.used == 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        return 1;
    }
    if (auto* const element = window->FindElementById(WindowPetEquipmentElementId::Name)) {
        element->data.text->text = pet.name6BD[0] != L'\0' ? pet.name6BD.data() : pet.name6AC.data();
    }
    if (auto* const element = window->FindElementById(WindowPetEquipmentElementId::PetNumber)) {
        element->data.text->text = std::format(L"{}/5", gLoginWindowRuntime.selectedPetCC3E5C + 1);
    }
    return 1;
}

static std::int32_t WindowPetEquipment_SelectPet(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    if (!window) return 0;
    const bool previous = elementId == static_cast<ElementId>(WindowPetEquipmentElementId::PreviousPet);
    const std::int32_t idle = previous ? 0x3B556 : 0x3B559;
    if (auto* const element = window->FindElementById(elementId)) {
        element->data.graphic->imageNo = idle + ((state & MouseFlag_LHoldPhysical) != 0 ? 1 : (state & MouseFlag_Hover) != 0 ? 2 : 0);
    }
    if ((state & MouseFlag_Hover) != 0) gHoverTextD68AE8 = kPetSelectHint586D2C;
    if ((state & MouseFlag_LDownEvent) != 0) {
        std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
        for (std::int32_t attempt = 0; attempt < 5; ++attempt) {
            position = (position + (previous ? 4 : 1)) % 5;
            const auto petIndex = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[position].index);
            if (world::gPetRuntimeRecords[petIndex].used != 0) break;
        }
        gLoginWindowRuntime.selectedPetCC3E5C = position;
        static_cast<void>(WindowPetEquipment_Update(-1));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return 1;
    }
    return (state & MouseFlag_Hover) != 0 ? 1 : 0;
}

static std::int32_t WindowPetEquipment_HandleItems(const ElementId, const std::int32_t events)
{
    WindowInstance* const window = gCurrentWindow;
    if (!window) return 0;
    auto& state = WindowRuntime_States()[WindowId_PetEquipment];
    const auto petIndex = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const auto& pet = world::gPetRuntimeRecords[petIndex];
    std::int32_t hovered = -1;
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        for (std::int32_t slot = 0; slot < static_cast<std::int32_t>(std::size(gPetEquipmentLabels)) && slot < pet.values6D8[0]; ++slot) {
            const std::int32_t left = window->x + kPetEquipmentOriginX + gSelectorPetX54F5C4[slot] - 24;
            const std::int32_t top = window->y + kPetEquipmentOriginY + gSelectorPetY54F5DC[slot] - 24;
            if (HitTestMouseRect_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = slot;
                break;
            }
        }
    }
    if (state.value10 != static_cast<std::uint32_t>(hovered)) state.value14 = 0;
    state.value10 = static_cast<std::uint32_t>(hovered);
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        // kind2与8～27明确表示背包物品；落点重建后的焦点不再是稳定来源。
        if (WindowDrag_ReadKind() == 2
            && source >= 8 && source < static_cast<std::int32_t>(world::gPlayerRuntimeSlots.items.size())
            && world::gPlayerRuntimeSlots.items[source].used != 0
            && (world::ReadPlayerActionFlagsByte() & 0x80u) == 0
            && world::IsObservationSceneTransitionActive_440B50() == 0) {
            const std::int32_t x = WindowDrag_ReadX();
            const std::int32_t y = WindowDrag_ReadY();
            for (std::int32_t slot = 0; slot < static_cast<std::int32_t>(std::size(gPetEquipmentLabels)) && slot < pet.values6D8[0]; ++slot) {
                const std::int32_t left = window->x + kPetEquipmentOriginX + gSelectorPetX54F5C4[slot] - 24;
                const std::int32_t top = window->y + kPetEquipmentOriginY + gSelectorPetY54F5DC[slot] - 24;
                if (x >= left && x < left + 48 && y >= top && y < top + 48) {
                    static_cast<void>(network::GetGameConnection().MoveItemBetweenOwnersRequest(0, source, petIndex + 1, slot, -1));
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                    break;
                }
            }
        }
        WindowDrag_Reset();
        return 1;
    }
    if (hovered < 0) return 0;
    const auto& item = pet.items6E4[hovered];
    gHoverTextD68AE8 = item.used != 0 ? item.name : gPetEquipmentLabels[hovered].text;
    if (item.used != 0 && (events & MouseFlag_LDblClickEvent) != 0) {
        if ((world::ReadPlayerActionFlagsByte() & 0x80u) == 0
            && world::IsObservationSceneTransitionActive_440B50() == 0) {
            static_cast<void>(network::GetGameConnection().MoveItemBetweenOwnersRequest(petIndex + 1, hovered, 0, -1, -1));
            static_cast<void>(audio::PlaySe_52C450(77, 320));
        }
    } else if (item.used != 0 && (events & MouseFlag_RDownEvent) != 0) {
        if (++state.value14 >= static_cast<std::uint32_t>(item.pages)) state.value14 = 0;
    }
    return 1;
}

static std::int32_t WindowPetEquipment_Draw(const std::int32_t)
{
    WindowInstance* const window = gCurrentWindow;
    if (!window) return 0;
    auto& state = WindowRuntime_States()[WindowId_PetEquipment];
    const auto petIndex = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const auto& pet = world::gPetRuntimeRecords[petIndex];
    const auto hovered = std::bit_cast<std::int32_t>(state.value10);
    if (hovered >= 0 && WindowDrag_ReadKind() == 0) {
        const auto& item = pet.items6E4[hovered];
        if (item.used != 0) {
            if (state.value14 >= static_cast<std::uint32_t>(item.pages)) state.value14 = 0;
            const std::int32_t mouseX = platform::gLegacyMouseState.mouseX;
            const std::int32_t mouseY = platform::gLegacyMouseState.mouseY;
            const bool specialTitle = (item.flags & 0x100u) != 0;
            static_cast<void>(WindowItem_DrawTooltip(
                mouseX + (mouseX < 320 ? 60 : -257), mouseY + (mouseY < 240 ? 60 : -174),
                (specialTitle ? item.memo : item.name).c_str(),
                specialTitle ? 5 : static_cast<std::int8_t>((item.flags & 0x200u) >> 9),
                item.description.data() + state.value14 * 8, 8, nullptr, 0));
        }
    }
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = 0xFFFFFFFFu;
    descriptor.mixFlags = render::GraphicMixFlag_OriginalPalette;
    for (std::int32_t slot = 0; slot < static_cast<std::int32_t>(std::size(gPetEquipmentLabels)); ++slot) {
        const std::int32_t left = window->x + kPetEquipmentOriginX + gSelectorPetX54F5C4[slot] - 24;
        const std::int32_t top = window->y + kPetEquipmentOriginY + gSelectorPetY54F5DC[slot] - 24;
        if (slot >= pet.values6D8[0]) {
            static_cast<void>(render::DrawGraphic3_520970(left, top, 223, 244297, 0, 1, 0, &descriptor));
            continue;
        }
        if (slot == hovered) {
            render::DrawGraphicHover(static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, render::gHoverFramePaletteIndex_D69768, 0);
        }
        if (pet.items6E4[slot].used != 0) {
            static_cast<void>(render::DrawGraphic(left + 24, top + 24, 223, pet.items6E4[slot].imageNo, 0, &descriptor));
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

static void WindowPetEquipment_Release()
{
    if (const auto* const window = WindowRuntime_States()[WindowId_PetEquipment].window) {
        for (const auto id : {WindowId_PetDetail, WindowId_PetAttributes, WindowId_PetSkills}) {
            WindowRuntime_States()[id].x = window->x;
            WindowRuntime_States()[id].y = window->y;
        }
    }
}

} // namespace detail
} // namespace cg::ui
