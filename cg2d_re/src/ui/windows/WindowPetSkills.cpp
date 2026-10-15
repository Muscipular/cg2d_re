#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x0048ECA0。
static std::int32_t WindowPetSkills_HandleButtons(std::int32_t index, std::int32_t state); // 宠物技能页六按钮。

// 原函数：0x0048F6C0。
static std::int32_t WindowPetSkills_HandleSkillRow(std::int32_t index, std::int32_t state); // 描述、技能换位与拖动。

// 原函数：0x0048DBB0。
static std::int32_t WindowPetSkills_Update(std::int32_t value);

// 原函数：0x0048DCB0。
static std::int32_t WindowPetSkills_Draw(std::int32_t value);

// 原函数：0x0048DD40。
static void WindowPetSkills_Release();

// 控件载荷、窗口状态、组件表与原型。

// 原全局：0x00D66C34。 动态宽文本，控件借稳定owner。
static std::array<std::wstring, 10> gPetTechniqueCostsD66C34 {}; // 原全局：0x00D66C34。

static WindowGraphicData gWindowGraphic565698 {0x3BAEE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565698。

static WindowGraphicData gWindowGraphic5658A8 {0x3B5B7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005658A8。

static WindowGraphicData gWindowGraphic565908 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565908。

static WindowGraphicData gWindowGraphic565920 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565920。

static WindowTextPrototype gWindowText565990 { // 原全局：0x00565990。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"技能", // 原常量参考：0x00586004。
};

static WindowTextPrototype gWindowText5659A0 { // 原全局：0x005659A0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"進行攻擊", // 原常量参考：0x00585FF4。
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

static WindowElementPrototype gWindowElements566810[51] { // 原全局：0x00566810。
    {WindowElementType_Input, 66, 31, 96, 12, 1, {.unusedValue = 0}, &WindowPetDetail_EditName},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 133, 276, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 186, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 187, 28, 32, 16, 1, {.graphic = &gWindowGraphic565650}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 175, 253, 49, 15, 1, {.graphic = &gWindowGraphic5658C0}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 225, 50, 11, 11, 1, {.graphic = &gWindowGraphic565908}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 225, 195, 11, 11, 1, {.graphic = &gWindowGraphic565920}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Text, 66, 31, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 44, 256, 0, 0, 1, {.text = &gWindowText565988}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 50, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 66, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 82, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 98, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 114, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 130, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 146, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 162, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 178, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 194, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 51, 0, 0, 1, {.number = &gWindowNumbers5667C0[0]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 67, 0, 0, 1, {.number = &gWindowNumbers5667C0[1]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 83, 0, 0, 1, {.number = &gWindowNumbers5667C0[2]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 99, 0, 0, 1, {.number = &gWindowNumbers5667C0[3]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 115, 0, 0, 1, {.number = &gWindowNumbers5667C0[4]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 131, 0, 0, 1, {.number = &gWindowNumbers5667C0[5]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 147, 0, 0, 1, {.number = &gWindowNumbers5667C0[6]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 163, 0, 0, 1, {.number = &gWindowNumbers5667C0[7]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 179, 0, 0, 1, {.number = &gWindowNumbers5667C0[8]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 195, 0, 0, 1, {.number = &gWindowNumbers5667C0[9]}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 213, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 225, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 237, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 249, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 17, 48, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 64, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 80, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 96, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 112, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 128, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 144, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 160, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 176, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 192, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 32, 0, 0, 1, {.graphic = &gWindowGraphic565698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic565608}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 25, 0, 0, 1, {.graphic = &gWindowGraphic565620}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 250, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原型：窗口18；原全局：0x0054E5D0。
WindowPrototype gWindowPrototypePetSkills {0, 4, 365, 83, 245, 306, 0x80010101u, 51, gWindowElements566810, &WindowPetSkills_Update, &WindowPetSkills_Draw, &WindowPetSkills_Release}; // 18: 54E5D0

// 窗口函数实现。

// 原函数：0x00491AA0。
static WindowElement* WindowPetSkills_Refresh()
{
    const auto selectedPet = []() -> world::detail::PetRuntimeRecord& {
        const auto index = std::bit_cast<std::int16_t>(
            world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
        return world::detail::gPetRuntimeRecords[index];
    };
    const auto& initialPet = selectedPet();
    std::wstring buffer;
    if (initialPet.used != 0) {
        const bool customName = initialPet.name6BD[0] != '\0';
        input::InputContext* const activeInput = input::gCurrentInputData;
        if (activeInput == &cg::ui::detail::gInputContextD2D4A0) {
            gCurrentWindow->elements[11].visible = 0;
            const std::wstring text = input::ContextText(*gPetNameInputPrototype57DCF8.input.context);
            WindowInstance* window;
            if (customName) {
                gLoginWindowRuntime.petNameDraftD45E74 = text;
                window = gCurrentWindow;
            } else {
                window = gCurrentWindow;
                gLoginWindowRuntime.petNameDraftD45E74 = text;
            }
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 1));
        } else {
            gCurrentWindow->elements[11].visible = 1;
            auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[11].data);
            data->text = gLoginWindowRuntime.petNameDraftD45E74;
            const auto& compared = selectedPet();
            const char* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
            const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(cg::text::FromGbkWide(name)) != 0;
            gCurrentWindow->elements[5].visible = changed ? 1 : 0;
            auto* const colorData = static_cast<WindowTextData*>(gCurrentWindow->elements[11].data);
            colorData->style.color = changed ? std::byte{6} : std::byte{0};
        }
        const auto& compared = selectedPet();
        const char* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
        const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(cg::text::FromGbkWide(name)) != 0;
        gCurrentWindow->elements[5].visible = changed ? 1 : 0;

        for (std::int32_t row = 0; row < 10; ++row) {
            gCurrentWindow->elements[37 + row].visible = 0;
            gCurrentWindow->elements[13 + row].visible = 0;
            gCurrentWindow->elements[23 + row].visible = 0;
        }
        std::int32_t shown = 0;
        if (selectedPet().value6A4 > 0) {
            do {
                gCurrentWindow->elements[37 + shown].visible = 1;
                ++shown;
            } while (shown < selectedPet().value6A4);
        }
        for (std::int32_t row = 0; row < 10; ++row) {
            if (selectedPet().techniquesD8[row].used != 0) {
                gCurrentWindow->elements[13 + row].visible = 1;
                const char* const techniqueName = selectedPet().techniquesD8[row].name.data();
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[13 + row].data);
                data->text = cg::text::FromGbkWide(techniqueName);
                gCurrentWindow->elements[23 + row].visible = 1;
                const std::int32_t fp = selectedPet().techniquesD8[row].fp;
                cg::text::FormatTo(buffer, L"{:3}", fp);
                gPetTechniqueCostsD66C34[row] = buffer;
            }
        }
    } else {
        gCurrentWindow->elements[11].visible = 0;
        for (std::int32_t row = 0; row < 10; ++row) {
            gCurrentWindow->elements[37 + row].visible = 0;
            gCurrentWindow->elements[13 + row].visible = 0;
            gCurrentWindow->elements[23 + row].visible = 0;
        }
    }
    cg::text::FormatTo(buffer, L"{}/{}", gLoginWindowRuntime.selectedPetCC3E5C + 1, 5);
    auto* const page = static_cast<WindowTextData*>(gCurrentWindow->elements[12].data);
    page->text = buffer;
    gCurrentWindow->elements[33].visible = 0;
    gCurrentWindow->elements[34].visible = 0;
    WindowElement* const result = gCurrentWindow->elements;
    result[35].visible = 0;
    gCurrentWindow->elements[36].visible = 0;
    return result;
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
        world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const std::int32_t nameTechnique = gLoginWindowRuntime.petTechniqueD2D66C;
    static_cast<void>(render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 5)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
        5, 12, 0, cg::text::FromGbkWide(world::detail::gPetRuntimeRecords[namePet].techniquesD8[nameTechnique].name.data()).c_str(),
        0, 0));
    const auto costPet = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const std::int32_t costTechnique = gLoginWindowRuntime.petTechniqueD2D66C;
    const std::int32_t fp = world::detail::gPetRuntimeRecords[costPet].techniquesD8[costTechnique].fp;
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
        WindowInstance* const window = gCurrentWindow;
        gPetNameInputPrototype57DCF8.text = gLoginWindowRuntime.petNameDraftD45E74;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
    }
    const auto selected = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    if (world::detail::gPetRuntimeRecords[selected].used == 0) {
        gCurrentWindow->state |= 0x8000u;
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
static std::int32_t WindowPetSkills_HandleButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    const auto setImage = [index](const std::int32_t image) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = image;
    };
    const auto petIndex = [](const std::int32_t position) {
        return std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index);
    };
    switch (index) {
    case 5:
        if ((state & 1) != 0) {
            setImage(0x3B570);
            gHoverTextD68AE8 = kPetRenameHint586D38;
            consumed = 1;
        } else {
            setImage(0x3B56E);
        }
        if ((state & 2) != 0) {
            const std::wstring text = input::ContextText(cg::ui::detail::gInputContextD2D4A0);
            const std::wstring escapedText = input::EscapeTextAndNotify(text, 256);
            const std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
            const std::uintptr_t socket = platform::gConnectionSocket;
            const std::int16_t pet = petIndex(position);
            static_cast<void>(network::Sub_5125B0(socket, pet, escapedText));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
            WindowChat_RestoreInput();
            if (text.empty()) {
                const std::int16_t selected = petIndex(gLoginWindowRuntime.selectedPetCC3E5C);
                const char* const name = world::detail::gPetRuntimeRecords[selected].name6AC.data();
                gLoginWindowRuntime.petNameDraftD45E74 = cg::text::FromGbkWide(name);
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[11].data);
                data->text = cg::text::FromGbkWide(name);
            }
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(0x3B56F);
        }
        return consumed;
    case 6:
        if ((state & 1) != 0) {
            setImage(0x3BAF9);
            gHoverTextD68AE8 = kPetPlaceHint586C14;
            consumed = 1;
        } else {
            setImage(0x3BAF7);
        }
        if ((state & 2) != 0) {
            if (world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].used != 0
                && (world::detail::ReadPlayerActionFlags() & 0x80u) == 0
                && world::detail::Sub_440B50() == 0) {
                const std::int16_t pet = petIndex(gLoginWindowRuntime.selectedPetCC3E5C);
                const std::int32_t y = world::detail::gPlayerY.value();
                const std::int32_t x = world::detail::gPlayerX.value();
                const std::uintptr_t socket = platform::gConnectionSocket;
                static_cast<void>(network::Sub_510E30(socket, x, y, pet));
                static_cast<void>(audio::PlaySe_52C450(79, 320));
                const std::int16_t selected = petIndex(gLoginWindowRuntime.selectedPetCC3E5C);
                if (world::detail::gPetRuntimeRecords[selected].flags6AA == 2) {
                    WindowCraft_ResetItemSelection();
                }
            }
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(0x3BAF8);
        }
        return consumed;
    case 7:
    case 8: {
        if ((state & 1) != 0) {
            setImage(index == 7 ? 0x3B558 : 0x3B55B);
            gHoverTextD68AE8 = kPetSelectHint586D2C;
            consumed = 1;
        } else {
            setImage(index == 7 ? 0x3B556 : 0x3B559);
        }
        // 下一只按钮即使未点击也检查>=5；上一只只在点击后取选择。
        if (index == 8 || (state & 2) != 0) {
            std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
            if ((state & 2) != 0 || position >= 5) {
                for (std::int32_t attempt = 0; attempt < 6; ++attempt) {
                    if (index == 7) {
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
                    if (world::detail::gPetRuntimeRecords[petIndex(position)].used != 0) {
                        break;
                    }
                }
                std::int16_t selected;
                if (index == 7) {
                    selected = petIndex(position);
                    gLoginWindowRuntime.selectedPetCC3E5C = position;
                } else {
                    gLoginWindowRuntime.selectedPetCC3E5C = position;
                    selected = petIndex(position);
                }
                const auto& pet = world::detail::gPetRuntimeRecords[selected];
                const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                gPetNameInputPrototype57DCF8.text = cg::text::FromGbkWide(name);
                WindowInstance* const window = gCurrentWindow;
                const std::int32_t y = window->y;
                const std::int32_t x = window->x;
                static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
                static_cast<void>(WindowPetSkills_Update(-1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                consumed = 1;
            }
        }
        if ((state & 8) != 0) {
            setImage(index == 7 ? 0x3B557 : 0x3B55A);
        }
        return consumed;
    }
    case 9:
    case 10:
        if ((state & 1) != 0) {
            setImage(index == 9 ? 0x3B594 : 0x3B597);
        } else {
            setImage(index == 9 ? 0x3B592 : 0x3B595);
        }
        if ((state & 0x400) != 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            setImage(index == 9 ? 0x3B593 : 0x3B596);
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
    if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
        static_cast<void>(WindowDrag_ResetMouse());
        const auto x = static_cast<std::int16_t>(
            platform::gLegacyMouseState.mouseX.value());
        const auto y = static_cast<std::int16_t>(
            platform::gLegacyMouseState.mouseY.value());
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
static std::int32_t WindowPetSkills_HandleSkillRow(const std::int32_t index, const std::int32_t state)
{
    std::uint32_t events = static_cast<std::uint32_t>(state);
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 11) {
        events &= ~0x60u;
    }
    if (index == 37) {
        for (std::int32_t line = 0; line < 4; ++line) {
            gCurrentWindow->elements[33 + line].visible = 0;
        }
    }
    const bool hover = (events & 1u) != 0;
    if (hover) {
        for (std::int32_t line = 0; line < 4; ++line) {
            gCurrentWindow->elements[33 + line].visible = 1;
        }
        std::wstring buffer;
        for (std::int32_t line = 0; line < 4; ++line) {
            const auto selected = std::bit_cast<std::int16_t>(
                world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
            const auto& pet = world::detail::gPetRuntimeRecords[selected];
            const auto technique = std::bit_cast<std::int16_t>(pet.techniquePairs650[index - 37].index);
            const char* const description = pet.techniquesD8[technique].description.data();
            if (input::ExtractGbkTextLine(buffer, description, line, 26) != 0) {
                auto* const elements = gCurrentWindow->elements;
                auto* const data = static_cast<WindowTextData*>(elements[33 + line].data);
                data->text = buffer;
            }
        }
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        const std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
        gLoginWindowRuntime.petTechniqueValueD2F784 = index - 37;
        gLoginWindowRuntime.petTechniqueD2D66C = source;
        const auto selected = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index);
        const auto& pet = world::detail::gPetRuntimeRecords[selected];
        const auto targetTechnique = std::bit_cast<std::int16_t>(pet.techniquePairs650[index - 37].index);
        const auto sourceTechnique = std::bit_cast<std::int16_t>(pet.techniquePairs650[source].index);
        const std::uintptr_t socket = platform::gConnectionSocket;
        static_cast<void>(network::Sub_5124E0(socket, selected, sourceTechnique, targetTechnique));
        WindowDrag_Reset();
    }
    if ((events & 4u) != 0 && hover && gCurrentWindow->elements[index - 24].visible != 0) {
        static_cast<void>(WindowPetSkills_StartDrag(index - 37));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    return hover ? 1 : 0;
}

} // namespace detail

} // namespace cg::ui
