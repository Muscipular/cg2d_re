#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

InputContext gInputContextD2D4A0 {}; // 原全局：0x00D2D4A0。
} // namespace cg::input

namespace cg::ui {
namespace detail {

// 原函数：0x00492260。
void WindowPetDetail_ResetDraftFlag()
{
    world::detail::gValueD67B28 = 0;
}

// 原函数：0x00492270。
std::int32_t WindowPetDetail_ReadDraftFlag()
{
    return world::detail::gValueD67B28 == 1 ? 1 : 0;
}

// 原函数：0x00492280。
void WindowPetDetail_InitializeDraft()
{
    WindowInstance* const window = gCurrentWindow;
    // 九处caller均属于详情页16/17/18；异常ID的原未写栈下标不专项模拟。
    constexpr std::int32_t nameElements[] {9, 14, 11};
    const std::int32_t element = nameElements[window->id - 16];
    const auto selected = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
    const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[selected];
    const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
    gLoginWindowRuntime.petNameDraftD45E74 = cg::text::FromGbkWide(name);
    static_cast<WindowTextData*>(window->elements[element].data)->text = cg::text::FromGbkWide(name);
}

// 原函数：0x00492350。
std::int32_t WindowPetDetail_SavePetSequences()
{
    std::int32_t sequence;
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
        sequence = (*world::detail::BorrowPetRuntimeF633F8Storage())[index].value6D0;
        gLoginWindowRuntime.petSequencesCAFEB0[row] = sequence;
    }
    return sequence;
}

// 原函数：0x00492390。
std::int32_t WindowPetDetail_HasPetSequenceChanges()
{
    for (std::int32_t row = 0; row < 5; ++row) {
        const auto index = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[row].index);
        const std::int32_t previous = gLoginWindowRuntime.petSequencesCAFEB0[row];
        if (previous != (*world::detail::BorrowPetRuntimeF633F8Storage())[index].value6D0) {
            return 1;
        }
    }
    return 0;
}

// 窗口16完整刷新；复制取样和空宠物显隐差异见window_16.md。
// 原函数：0x0048F8F0。
std::uint32_t WindowPetDetail_Refresh()
{
    const auto selectedPet = []() -> world::detail::PetRuntimeRecord& {
        const auto index = std::bit_cast<std::int16_t>(
            world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
        return (*world::detail::BorrowPetRuntimeF633F8Storage())[index];
    };
    // 保留原复制段对elements或data的取样阶段；正文直接使用UTF-16。
    const auto copyTextAfterScan = [](const std::int32_t element, const std::wstring_view text) {
        WindowElement* const elements = gCurrentWindow->elements;
        auto* const data = static_cast<WindowTextData*>(elements[element].data);
        data->text = text;
    };
    const auto copyTextBeforeScan = [](const std::int32_t element, const std::wstring_view text) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[element].data);
        const std::size_t length = text.size() + 1;
        data->text = text;
        return length;
    };
    std::wstring buffer;
    if (selectedPet().used != 0) {
        constexpr std::int32_t visibleElements[] {9, 11, 12, 13, 14, 15, 16, 17, 20, 10, 18, 19};
        for (const std::int32_t element : visibleElements) {
            gCurrentWindow->elements[element].visible = 1;
        }
        const bool customName = selectedPet().name6BD[0] != '\0';
        input::InputContext* const activeInput = input::gCurrentInputData;
        if (activeInput == &input::gInputContextD2D4A0) {
            gCurrentWindow->elements[9].visible = 0;
            const std::wstring text = input::ContextText(*gPetNameInputPrototype57DCF8.input.context);
            WindowInstance* const window = gCurrentWindow;
            gLoginWindowRuntime.petNameDraftD45E74 = text;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 1));
        } else {
            gCurrentWindow->elements[9].visible = 1;
            if (customName) {
                static_cast<void>(copyTextBeforeScan(9, gLoginWindowRuntime.petNameDraftD45E74));
            } else {
                copyTextAfterScan(9, gLoginWindowRuntime.petNameDraftD45E74);
            }
            const auto& compared = selectedPet();
            const char* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
            const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(cg::text::FromGbkWide(name)) != 0;
            auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[9].data);
            data->bytes00[0] = changed ? std::byte{6} : std::byte{0};
        }
        const auto& compared = selectedPet();
        const char* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
        const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(cg::text::FromGbkWide(name)) != 0;
        gCurrentWindow->elements[5].visible = changed ? 1 : 0;

        if ((selectedPet().values0C[5] == -1)) {
            const std::int32_t experience = selectedPet().values0C[4].value();
            buffer = std::format(L"{:>8}/       -", experience);
        } else {
            const std::int32_t nextExperience = selectedPet().values0C[5].value();
            const std::int32_t experience = selectedPet().values0C[4].value();
            buffer = std::format(L"{:>8}/{:>8}", experience, nextExperience);
        }
        copyTextAfterScan(11, buffer);
        const std::int32_t maxHp = selectedPet().values0C[1].value();
        const std::int32_t hp = selectedPet().values0C[0].value();
        buffer = std::format(L"{:>5}/{:>5}", hp, maxHp);
        copyTextAfterScan(12, buffer);
        const std::int32_t maxFp = selectedPet().values0C[3].value();
        const std::int32_t fp = selectedPet().values0C[2].value();
        buffer = std::format(L"{:>5}/{:>5}", fp, maxFp);
        copyTextAfterScan(13, buffer);

        const auto& tribePet = selectedPet();
        WindowInstance* const tribeWindow = gCurrentWindow;
        const auto tribe = std::bit_cast<std::int16_t>(tribePet.value04);
        WindowElement* const tribeElements = tribeWindow->elements;
        const wchar_t* const tribeName = gTribeNames57CB54[tribe];
        auto* const tribeData = static_cast<WindowTextData*>(tribeElements[14].data);
        tribeData->text = tribeName;
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
        WindowElement* const imageElements = gCurrentWindow->elements;
        const std::int32_t imageNo = imagePet.imageNo6A0;
        auto* const imageData = static_cast<WindowAnimationData*>(imageElements[20].data);
        login::detail::SetYobiActionField2A8(imageData->action, imageNo);
        auto* const directionData = static_cast<WindowAnimationData*>(gCurrentWindow->elements[20].data);
        const std::int32_t direction = gLoginWindowRuntime.valueD673D0;
        login::detail::SetYobiActionField2B8(directionData->action, direction);

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
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[10].data)->imageNo = injuryImage;

        const auto showSegment = [](const std::int32_t element, const std::int32_t image) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[element].data)->imageNo = image;
            gCurrentWindow->elements[element].visible = 1;
        };
        for (std::int32_t attribute = 0; attribute < 4; ++attribute) {
            const std::int32_t base = 21 + attribute * 10;
            for (std::int32_t segment = 0; segment < 10; ++segment) {
                gCurrentWindow->elements[base + segment].visible = 0;
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
            gCurrentWindow->elements[element].visible = 0;
        }
        for (std::int32_t segment = 0; segment < 10; ++segment) {
            gCurrentWindow->elements[21 + segment].visible = 0;
            gCurrentWindow->elements[31 + segment].visible = 0;
            gCurrentWindow->elements[41 + segment].visible = 0;
            gCurrentWindow->elements[51 + segment].visible = 0;
        }
        gCurrentWindow->elements[10].visible = 0;
    }
    buffer = std::format(L"{}/{}", gLoginWindowRuntime.selectedPetCC3E5C + 1, 5);
    return static_cast<std::uint32_t>(copyTextBeforeScan(17, buffer));
}

// 窗口16主更新，入口-1负责共享详情拖动槽的初始化。
// 原函数：0x0048D9A0。
std::int32_t WindowPetDetail_Update(const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.previousPetCC4888 = gLoginWindowRuntime.selectedPetCC3E5C;
        static_cast<void>(WindowPetDetail_SavePetSequences());
        WindowPetDetail_InitializeDraft();
        gLoginWindowRuntime.petTechniqueD2D66C = -1;
        gLoginWindowRuntime.petTechniqueValueD2F784 = -1;
        gLoginWindowRuntime.petTechniqueDragD460CC = 0;
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
        gPetNameInputPrototype57DCF8.text = gLoginWindowRuntime.petNameDraftD45E74;
        WindowInstance* const window = gCurrentWindow;
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
    static_cast<void>(WindowPetDetail_Refresh());
    return 1;
}

// 原函数：0x0048DCC0。
void WindowPetDetail_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_PetDetail].window;
    if (window != nullptr) {
        gWindowStates[WindowId_PetAttributes].x = window->x;
        gWindowStates[WindowId_PetAttributes].y = window->y;
        gWindowStates[WindowId_PetSkills].x = window->x;
        gWindowStates[WindowId_PetSkills].y = window->y;
    }
}
namespace {


constexpr wchar_t kPetSkillPageHint585920[] = L"切換至此寵物的技能視窗。"; // 原常量参考：0x00585920。

constexpr wchar_t kPetDetailPageHint58593C[] = L"切換至此寵物的詳細視窗。"; // 原常量参考：0x0058593C。

constexpr wchar_t kPetAbilityPageHint585958[] = L"切換至此寵物的能力視窗。"; // 原常量参考：0x00585958。
} // namespace


// 原函数：0x0048DD80。
std::int32_t WindowPetDetail_HandleButtons(const std::int32_t index, const std::int32_t state)
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
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[9].data);
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
    case 8:
        if ((state & 1) != 0) {
            setImage(index == 7 ? 0x3B558 : 0x3B55B);
            gHoverTextD68AE8 = kPetSelectHint586D2C;
            consumed = 1;
        } else {
            setImage(index == 7 ? 0x3B556 : 0x3B559);
        }
        if ((state & 2) != 0) {
            std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
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
            const std::int16_t selected = petIndex(position);
            gLoginWindowRuntime.selectedPetCC3E5C = position;
            const auto& pet = (*world::detail::BorrowPetRuntimeF633F8Storage())[selected];
            const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
            const std::size_t length = std::strlen(name) + 1;
            const std::size_t whole = length & ~std::size_t{3};
            gPetNameInputPrototype57DCF8.text = cg::text::FromGbkWide(name);
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
            static_cast<void>(WindowPetDetail_Update(-1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(index == 7 ? 0x3B557 : 0x3B55A);
        }
        return consumed;
    case 64:
        if ((state & 0x400) != 0) {
            const std::int32_t direction = gLoginWindowRuntime.valueD673D0 + 1;
            gLoginWindowRuntime.valueD673D0 = direction;
            if (direction > 7) {
                gLoginWindowRuntime.valueD673D0 = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & 0x800) != 0) {
            const std::int32_t direction = gLoginWindowRuntime.valueD673D0 - 1;
            gLoginWindowRuntime.valueD673D0 = direction;
            if (direction < 0) {
                gLoginWindowRuntime.valueD673D0 = 7;
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
std::int32_t WindowPetDetail_EditName(const std::int32_t index, const std::int32_t state)
{
    if ((state & 2) == 0) {
        return 0;
    }
    gPetNameInputPrototype57DCF8.text = gLoginWindowRuntime.petNameDraftD45E74;
    WindowInstance* const window = gCurrentWindow;
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 0));
    input::SetContext_44B330(&input::gInputContextD2D4A0);
    WindowInstance* const focused = gCurrentWindow;
    gFocusedElementDataD45F94 = focused->elements[index].data;
    static_cast<WindowInputData*>(focused->elements[index].data)->context = &input::gInputContextD2D4A0;
    return 1;
}

// 原函数：0x0048F410。
std::int32_t WindowPetDetail_SwitchPage(const std::int32_t index, const std::int32_t state)
{
    std::int32_t target;
    std::int32_t idle;
    std::int32_t hover;
    const wchar_t* hint;
    switch (index) {
    case 2:
        target = 16; idle = 0x3BAF4; hover = 0x3BAF6; hint = kPetAbilityPageHint585958;
        break;
    case 3:
        target = 17; idle = 0x3B583; hover = 0x3B585; hint = kPetDetailPageHint58593C;
        break;
    case 4:
        target = 18; idle = 0x3BAF1; hover = 0x3BAF3; hint = kPetSkillPageHint585920;
        break;
    default:
        return 0;
    }
    std::int32_t consumed = 0;
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if ((state & 1) != 0) {
        graphic->imageNo = hover;
        gHoverTextD68AE8 = hint;
        consumed = 1;
    } else {
        graphic->imageNo = idle;
    }
    if ((state & 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(WindowRuntime_Open(target));
        WindowInstance* const xSource = gCurrentWindow;
        WindowInstance* const xTarget = gWindowStates[target].window;
        xTarget->x = xSource->x;
        WindowInstance* const ySource = gCurrentWindow;
        WindowInstance* const yTarget = gWindowStates[target].window;
        yTarget->y = ySource->y;
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        return 1;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
