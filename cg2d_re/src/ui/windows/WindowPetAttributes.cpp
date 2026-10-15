#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 窗口17完整属性刷新；显示逐项取号，加点判断单独固定宠物，见window_17.md。
// 原函数：0x00490780。
std::uint32_t WindowPetAttributes_Refresh()
{
    const auto selectedPet = []() -> world::detail::PetRuntimeRecord& {
        const auto index = std::bit_cast<std::int16_t>(
            world::detail::gPetPairsDE0B08[gLoginWindowRuntime.selectedPetCC3E5C].index);
        return (*world::detail::BorrowPetRuntimeF633F8Storage())[index];
    };
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
            gCurrentWindow->elements[element].visible = 1;
        }
        const bool customName = selectedPet().name6BD[0] != '\0';
        input::InputContext* const activeInput = input::gCurrentInputData;
        // 原名分支在输入身份判断前固定elements，自定义名分支在判断后读取。
        WindowElement* const originalNameElements = customName ? nullptr : gCurrentWindow->elements;
        if (activeInput == &input::gInputContextD2D4A0) {
            if (customName) {
                gCurrentWindow->elements[14].visible = 0;
            } else {
                originalNameElements[14].visible = 0;
            }
            const std::wstring text = input::ContextText(*gPetNameInputPrototype57DCF8.input.context);
            WindowInstance* const window = gCurrentWindow;
            gLoginWindowRuntime.petNameDraftD45E74 = text;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gPetNameInputPrototype57DCF8, x, y, 1));
        } else {
            if (customName) {
                gCurrentWindow->elements[14].visible = 1;
                static_cast<void>(copyTextBeforeScan(14, gLoginWindowRuntime.petNameDraftD45E74));
            } else {
                originalNameElements[14].visible = 1;
                copyTextAfterScan(14, gLoginWindowRuntime.petNameDraftD45E74);
            }
            const auto& compared = selectedPet();
            const char* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
            const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(cg::text::FromGbkWide(name)) != 0;
            auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[14].data);
            data->bytes00[0] = changed ? std::byte{6} : std::byte{0};
        }
        const auto& compared = selectedPet();
        const char* const name = customName ? compared.name6BD.data() : compared.name6AC.data();
        const bool changed = gLoginWindowRuntime.petNameDraftD45E74.compare(cg::text::FromGbkWide(name)) != 0;
        gCurrentWindow->elements[5].visible = changed ? 1 : 0;

        writeNumber(15, selectedPet().value08);
        const std::int32_t maxHp = selectedPet().values0C[1].value();
        const std::int32_t hp = selectedPet().values0C[0].value();
        buffer = std::format(L"{:>4}/{:>4}", hp, maxHp);
        copyTextAfterScan(16, buffer);
        const std::int32_t maxFp = selectedPet().values0C[3].value();
        const std::int32_t fp = selectedPet().values0C[2].value();
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
            gCurrentWindow->elements[element].visible = 0;
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
                    gCurrentWindow->elements[9 + attribute].visible = 1;
                }
            }
        }
    } else {
        for (const std::int32_t element : valueElements) {
            gCurrentWindow->elements[element].visible = 0;
        }
        for (std::int32_t element = 9; element <= 13; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
    }
    buffer = std::format(L"{}/{}", gLoginWindowRuntime.selectedPetCC3E5C + 1, 5);
    return static_cast<std::uint32_t>(copyTextBeforeScan(18, buffer));
}

// 窗口17初始化仅采样名称/序号并发声，不代替窗口16的拖动槽初始化。
// 原函数：0x0048DAB0。
std::int32_t WindowPetAttributes_Update(const std::int32_t value)
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
    static_cast<void>(WindowPetAttributes_Refresh());
    return 1;
}

// 原函数：0x0048DD00。
void WindowPetAttributes_Release()
{
    WindowInstance* const window = gWindowStates[WindowId_PetAttributes].window;
    if (window != nullptr) {
        gWindowStates[WindowId_PetDetail].x = window->x;
        gWindowStates[WindowId_PetDetail].y = window->y;
        gWindowStates[WindowId_PetSkills].x = window->x;
        gWindowStates[WindowId_PetSkills].y = window->y;
    }
}
namespace {

constexpr wchar_t kPetAllocateHint5863A4[] = L"分配點數至目前的能力數值。"; // 原常量参考：0x005863A4。
} // namespace


// 原函数：0x0048E430。
std::int32_t WindowPetAttributes_HandleButtons(const std::int32_t index, const std::int32_t state)
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
                auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[14].data);
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
            static_cast<void>(WindowPetAttributes_Update(-1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            setImage(index == 7 ? 0x3B557 : 0x3B55A);
        }
        return consumed;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        if ((state & 1) != 0) {
            setImage(0x3B53D);
            gHoverTextD68AE8 = kPetAllocateHint5863A4;
            consumed = 1;
        } else {
            setImage(0x3B53B);
        }
        if ((state & 2) != 0) {
            const std::int32_t attribute = index - 9;
            const std::int32_t position = gLoginWindowRuntime.selectedPetCC3E5C;
            const std::uintptr_t socket = platform::gConnectionSocket;
            const std::int16_t pet = petIndex(position);
            static_cast<void>(network::Sub_512400(socket, pet, attribute));
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

namespace cg::ui::detail {

namespace {

WindowGraphicData gWindowGraphic565680 {0x3BAF0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565680。

WindowGraphicData gWindowGraphic5658D8 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005658D8。

} // namespace

// 原型：窗口17；原全局：0x0054E5A0。
WindowPrototype gWindowPrototypePetAttributes {0, 4, 365, 83, 245, 306, 0x80010101u, 46, gWindowElements566200, &WindowPetAttributes_Update, &WindowRuntime_DrawDefault, &WindowPetAttributes_Release}; // 17: 54E5A0

WindowElementPrototype gWindowElements566200[46] { // 原全局：0x00566200。
    {WindowElementType_Input, 66, 31, 96, 12, 1, {.unusedValue = 0}, &WindowPetDetail_EditName},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 133, 276, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 186, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 187, 28, 32, 16, 1, {.graphic = &gWindowGraphic565650}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 175, 253, 49, 15, 1, {.graphic = &gWindowGraphic5658C0}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 113, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 128, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 143, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 158, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 173, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Text, 66, 31, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 63, 48, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 55, 65, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 65, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 44, 256, 0, 0, 1, {.text = &gWindowText565988}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 98, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 114, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 129, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 144, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 159, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 174, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 197, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 212, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 97, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 98, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 114, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 113, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 144, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 144, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 159, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 159, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 174, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 174, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 197, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 197, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 212, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 212, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 227, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 18, 32, 0, 0, 1, {.graphic = &gWindowGraphic565680}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic565608}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 25, 0, 0, 1, {.graphic = &gWindowGraphic565620}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 250, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

} // namespace cg::ui::detail
