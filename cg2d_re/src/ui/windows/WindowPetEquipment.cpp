#include "cg/network/CgConnection.hpp"
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
    CrystalLabel = 17,
    ArmorLabel = 18,
    CollarLabel = 19,
    AccessoryLabel1 = 20,
    AccessoryLabel2 = 21,
    AccessoryLabel3 = 22,
    Frame = 23,
    Panel = 24,
    SideArea = 25,
    TabEquipment = 67,
};

namespace detail {

// 同步宠物名字、排序位置及说明状态，宠物消失时关闭装备页。
static std::int32_t WindowPetEquipment_Update(std::int32_t value);
// 共用物品提示和宠物外框，绘制六槽图标及空格边框。
static std::int32_t WindowPetEquipment_Draw(std::int32_t value);
// 将装备页位置同步给另外三个宠物详情页。
static void WindowPetEquipment_Release();
// 前后跳过空宠物槽，沿用详情页的当前排序位置。
static std::int32_t WindowPetEquipment_SelectPet(ElementId elementId, std::int32_t state);
// 背包装入、双击卸下及右键说明翻页；装备变动交给既有Cpa回包。
static std::int32_t WindowPetEquipment_HandleItems(ElementId elementId, std::int32_t state);

// 真实装备顺序沿items6E4：晶石、防具、项圈、三个饰品；坐标是48像素物品格左上角。
static constexpr std::array<std::array<std::int32_t, 2>, 6> kPetEquipmentSlots {{
    {26, 85}, {98, 85}, {170, 85}, {26, 165}, {98, 165}, {170, 165},
}};

WindowGraphicData gPetEquipmentPageGraphic {244474, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gPetEquipmentActivePageGraphic {244475, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowTextPrototype gPetEquipmentName {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L""};
static WindowTextPrototype gPetEquipmentNumber {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"1/5"};
static WindowTextPrototype gPetEquipmentLabels[] {
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"晶石"},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"防具"},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"项圈"},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"饰品1"},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"饰品2"},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"饰品3"},
};

static WindowElementPrototype gPetEquipmentElements[] {
    {WindowElementType_Text, 66, 31, 140, 12, 1, {.text = &gPetEquipmentName}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::Name)},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPetEquipmentElementId::ButtonClose)},
    {WindowElementType_Graphic, 27, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetEquipmentElementId::TabStatus)},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetEquipmentElementId::TabAttributes)},
    {WindowElementType_Graphic, 133, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowPetDetail_SwitchPage, -1, static_cast<ElementId>(WindowPetEquipmentElementId::TabSkills)},
    {WindowElementType_EventArea, 26, 85, 192, 128, 1, {.unusedValue = 0}, &WindowPetEquipment_HandleItems, -1, static_cast<ElementId>(WindowPetEquipmentElementId::EquipmentArea)},
    {WindowElementType_Text, 23, 31, 24, 12, 1, {.text = &gWindowText565950}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::NameLabel)},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetEquipment_SelectPet, -1, static_cast<ElementId>(WindowPetEquipmentElementId::PreviousPet)},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetEquipment_SelectPet, -1, static_cast<ElementId>(WindowPetEquipmentElementId::NextPet)},
    {WindowElementType_Text, 38, 254, 24, 12, 1, {.text = &gPetEquipmentNumber}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::PetNumber)},
    {WindowElementType_Text, 38, 69, 24, 12, 1, {.text = &gPetEquipmentLabels[0]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::CrystalLabel)},
    {WindowElementType_Text, 110, 69, 24, 12, 1, {.text = &gPetEquipmentLabels[1]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::ArmorLabel)},
    {WindowElementType_Text, 182, 69, 24, 12, 1, {.text = &gPetEquipmentLabels[2]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::CollarLabel)},
    {WindowElementType_Text, 35, 149, 30, 12, 1, {.text = &gPetEquipmentLabels[3]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::AccessoryLabel1)},
    {WindowElementType_Text, 107, 149, 30, 12, 1, {.text = &gPetEquipmentLabels[4]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::AccessoryLabel2)},
    {WindowElementType_Text, 179, 149, 30, 12, 1, {.text = &gPetEquipmentLabels[5]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetEquipmentElementId::AccessoryLabel3)},
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
    auto& state = gWindowStates[WindowId_PetEquipment];
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
    auto& state = gWindowStates[WindowId_PetEquipment];
    const auto petIndex = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const auto& pet = world::gPetRuntimeRecords[petIndex];
    std::int32_t hovered = -1;
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        for (std::int32_t slot = 0; slot < static_cast<std::int32_t>(kPetEquipmentSlots.size()); ++slot) {
            const std::int32_t left = window->x + kPetEquipmentSlots[slot][0];
            const std::int32_t top = window->y + kPetEquipmentSlots[slot][1];
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
        if (WindowDrag_ReadKind() == 2 && WindowDrag_ReadFocusWindowId() == WindowId_Inventory
            && source >= 8 && source < static_cast<std::int32_t>(world::gPlayerRuntimeSlots.items.size())
            && world::gPlayerRuntimeSlots.items[source].used != 0
            && (world::ReadPlayerActionFlagsByte() & 0x80u) == 0
            && world::IsObservationSceneTransitionActive_440B50() == 0) {
            const std::int32_t x = WindowDrag_ReadX();
            const std::int32_t y = WindowDrag_ReadY();
            for (std::int32_t slot = 0; slot < static_cast<std::int32_t>(kPetEquipmentSlots.size()); ++slot) {
                const std::int32_t left = window->x + kPetEquipmentSlots[slot][0];
                const std::int32_t top = window->y + kPetEquipmentSlots[slot][1];
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
    auto& state = gWindowStates[WindowId_PetEquipment];
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
    for (std::int32_t slot = 0; slot < static_cast<std::int32_t>(kPetEquipmentSlots.size()); ++slot) {
        const std::int32_t left = window->x + kPetEquipmentSlots[slot][0];
        const std::int32_t top = window->y + kPetEquipmentSlots[slot][1];
        render::DrawGraphicHover(static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, slot == hovered ? render::gHoverFramePaletteIndex_D69768 : 249, 0);
        if (pet.items6E4[slot].used != 0) {
            static_cast<void>(render::DrawGraphic(left + 24, top + 24, 223, pet.items6E4[slot].imageNo, 0, &descriptor));
        }
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

static void WindowPetEquipment_Release()
{
    if (const auto* const window = gWindowStates[WindowId_PetEquipment].window) {
        for (const auto id : {WindowId_PetDetail, WindowId_PetAttributes, WindowId_PetSkills}) {
            gWindowStates[id].x = window->x;
            gWindowStates[id].y = window->y;
        }
    }
}

} // namespace detail
} // namespace cg::ui
