#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

std::array<std::wstring, 10> gPetTechniqueCostsD66C34 {}; // 原全局：0x00D66C34。

// 原函数：0x00491AA0。
WindowElement* WindowPetSkills_Refresh()
{
    const auto selectedPet = []() -> world::detail::PetRuntimeRecord& {
        const auto index = std::bit_cast<std::int16_t>(
            world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
        return (*world::detail::BorrowPetRuntimeF633F8Storage())[index];
    };
    const auto& initialPet = selectedPet();
    std::wstring buffer;
    if (initialPet.used != 0) {
        const bool customName = initialPet.name6BD[0] != '\0';
        input::InputContext* const activeInput = input::gCurrentInputData;
        if (activeInput == &input::gInputContextD2D4A0) {
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
            colorData->bytes00[0] = changed ? std::byte{6} : std::byte{0};
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
std::int32_t WindowPetSkills_DrawDraggedSkillTooltip()
{
    if (gLoginWindowRuntime.petTechniqueDragD460CC == 0) {
        return 0;
    }
    const std::int32_t x = (platform::BorrowLegacyMouseStateStorage()->mouseX - 105);
    const std::int32_t y = (platform::BorrowLegacyMouseStateStorage()->mouseY - 8);
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(0, 0, 5, 12, 0, cg::text::FromGbkWide(input::LegacyText593850()), 0, 0));
    const auto namePet = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const std::int32_t nameTechnique = gLoginWindowRuntime.petTechniqueD2D66C;
    static_cast<void>(render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 5)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
        5, 12, 0, cg::text::FromGbkWide((*world::detail::BorrowPetRuntimeF633F8Storage())[namePet].techniquesD8[nameTechnique].name.data()).c_str(),
        0, 0));
    const auto costPet = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const std::int32_t costTechnique = gLoginWindowRuntime.petTechniqueD2D66C;
    const std::int32_t fp = (*world::detail::BorrowPetRuntimeF633F8Storage())[costPet].techniquesD8[costTechnique].fp;
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:3}", fp);
    static_cast<void>(render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 187)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
        5, 12, 0, buffer, 0, 0));
    const auto graphicY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const auto graphicX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return login::detail::DrawGraphic(graphicX + 105, graphicY + 8, 230, 0x3B5B7, 0, &descriptor);
}

// 原函数：0x0048DBB0。
std::int32_t WindowPetSkills_Update(const std::int32_t value)
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
        const std::size_t length = gLoginWindowRuntime.petNameDraftD45E74.size() + 1;
        WindowInstance* const window = gCurrentWindow;
        gPetNameInputPrototype57DCF8.text = gLoginWindowRuntime.petNameDraftD45E74;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
    }
    const auto selected = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    if ((*world::detail::BorrowPetRuntimeF633F8Storage())[selected].used == 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    static_cast<void>(WindowPetSkills_Refresh());
    return 1;
}

// 原函数：0x0048DCB0。
std::int32_t WindowPetSkills_Draw(const std::int32_t)
{
    static_cast<void>(WindowPetSkills_DrawDraggedSkillTooltip());
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x0048DD40。
void WindowPetSkills_Release()
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
std::int32_t WindowPetSkills_HandleButtons(const std::int32_t index, const std::int32_t state)
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
            const std::wstring text = input::ContextText(input::gInputContextD2D4A0);
            const std::wstring escapedText = input::EscapeTextAndNotify(text, 256);
            const std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
            const std::uintptr_t socket = platform::gConnectionSocket;
            const std::int16_t pet = petIndex(position);
            static_cast<void>(network::Sub_5125B0(socket, pet, escapedText));
            static_cast<void>(audio::PlaySe_52C450(78, 320));
            WindowChat_RestoreInput();
            if (text.empty()) {
                const std::int16_t selected = petIndex(gLoginWindowRuntime.selectedPetCC3E5C);
                const char* const name = (*world::detail::BorrowPetRuntimeF633F8Storage())[selected].name6AC.data();
                gLoginWindowRuntime.petNameDraftD45E74 = cg::text::FromGbkWide(name);
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[11].data);
                const std::size_t length = std::strlen(name) + 1;
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
                const std::int32_t y = world::detail::BorrowPlayerYStorage()->value();
                const std::int32_t x = world::detail::BorrowPlayerXStorage()->value();
                const std::uintptr_t socket = platform::gConnectionSocket;
                static_cast<void>(network::Sub_510E30(socket, x, y, pet));
                static_cast<void>(audio::PlaySe_52C450(79, 320));
                const std::int16_t selected = petIndex(gLoginWindowRuntime.selectedPetCC3E5C);
                if ((*world::detail::BorrowPetRuntimeF633F8Storage())[selected].flags6AA == 2) {
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
                    if ((*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex(position)].used != 0) {
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
                const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[selected];
                const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
                const std::size_t length = std::strlen(name) + 1;
                const std::size_t whole = length & ~std::size_t{3};
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
void WindowPetSkills_HandleDrag(const std::int32_t mode, const std::int32_t, const std::int32_t source)
{
    if (mode != 0) {
        if (mode > 0 && mode <= 4) {
            gLoginWindowRuntime.petTechniqueDragD460CC = 0;
            gLoginWindowRuntime.petTechniqueD2D66C = -1;
            gLoginWindowRuntime.petTechniqueValueD2F784 = -1;
        }
        return;
    }
    if ((platform::BorrowLegacyMouseStateStorage()->edgeButtons & 2u) != 0) {
        static_cast<void>(WindowDrag_ResetMouse());
        const auto x = static_cast<std::int16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseX.value());
        const auto y = static_cast<std::int16_t>(
            platform::BorrowLegacyMouseStateStorage()->mouseY.value());
        static_cast<void>(WindowDrag_StartAtPosition(11, source, x, y, WindowPetSkills_HandleDrag));
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
}

// 原函数：0x0048F690。
std::int32_t WindowPetSkills_StartDrag(const std::int32_t source)
{
    gLoginWindowRuntime.petTechniqueD2D66C = source;
    gLoginWindowRuntime.petTechniqueDragD460CC = 1;
    return WindowDrag_Start(11, source, WindowPetSkills_HandleDrag);
}

// 原函数：0x0048F6C0。
std::int32_t WindowPetSkills_HandleSkillRow(const std::int32_t index, const std::int32_t state)
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
            const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[selected];
            const auto technique = std::bit_cast<std::int16_t>(pet.techniquePairs650[index - 37].index);
            const char* const description = pet.techniquesD8[technique].description.data();
            if (input::ExtractGbkTextLine(buffer, description, line, 26) != 0) {
                auto* const elements = gCurrentWindow->elements;
                const std::size_t length = buffer.size() + 1;
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
        const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[selected];
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
