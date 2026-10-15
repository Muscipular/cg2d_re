#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {
namespace detail {

// 原函数：0x004A53D0。
WindowInstance* WindowMailCompose_Open()
{
    gLoginWindowRuntime.prefixD2F0B8.value26C = 1;
    return WindowRuntime_Open(WindowId_MailCompose);
}
namespace {

constexpr wchar_t kPetMailHint58774C[] = L"使用寵物郵件。"; // 原常量参考：0x0058774C。

constexpr wchar_t kGroupMailText587780[] = L"群組郵件"; // 原常量参考：0x00587780。
} // namespace


// 原函数：0x004A53F0。
std::int32_t WindowMailCompose_Update(const std::int32_t value)
{
    if (value == -1) {
        if (gLoginWindowRuntime.prefixD2F0B8.value10 != 1) {
            WindowInstance* const window = gWindowStates[WindowId_GroupSelect].window;
            if (window != nullptr) {
                window->state |= 0x8000u;
            }
        } else {
            if (gLoginWindowRuntime.prefixD2F0B8.value26C != 0) {
                gLoginWindowRuntime.mailGroupCC1A30.initialize = 1;
            }
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_GroupSelect, 1, 0, nullptr));
        }
        WindowInstance* const petWindow = gWindowStates[WindowId_PetMailCompose].window;
        if (petWindow != nullptr) {
            petWindow->state |= 0x8000u;
            const std::int32_t oldSlot = gLoginWindowRuntime.prefixD2F0B8.value0C;
            if (oldSlot != -1) {
                auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
                auto count = occupancy[oldSlot];
                gLoginWindowRuntime.prefixD2F0B8.value0C = -1;
                --count;
                gLoginWindowRuntime.prefixD2F0B8.value08 = -1;
                occupancy[oldSlot] = count;
            }
        }
        const WindowRuntimePrefixD2F0B8 previous = gLoginWindowRuntime.mailDraftCF4BC8;
        const WindowRuntimePrefixD2F0B8 current = gLoginWindowRuntime.prefixD2F0B8;
        if (WindowRuntime_MatchesRuntimePrefix(current, previous) != 0) {
            gMailInputPrototype57E740.text = cg::text::FromGbkWide(reinterpret_cast<const char*>(gLoginWindowRuntime.mailDraftCF4BC8.bytes14.data()));
        }
        const WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        if (WindowInput_ActivateElement(&gMailInputPrototype57E740, x, y, 0) == 0) {
            gMailInputPrototype57E740.input.context = &input::gInputContextD66CA8;
            gMailInputPrototype57E740.input.kind = WindowInputReferenceKind::Context;
        }
        input::gInputContextD66CA8.maxCharPerLine = 28;
        gMailInputPrototype57E740.maxCharPerLine = 28;
        input::gInputContextD66CA8.maxLine = 10;
        gMailInputPrototype57E740.maxLine = 10;
        input::gInputContextD66CA8.field14C = 12;
        gMailInputPrototype57E740.value224 = 12;
        input::gInputContextD66CA8.encrypt = 0;
        gMailInputPrototype57E740.encrypt = 0;
        input::SetContext_44B330(&input::gInputContextD66CA8);
        WindowInstance* const focused = gCurrentWindow;
        gMailInputPrototype57E740.text.clear();
        gFocusedElementDataD45F94 = focused->elements[1].data;
        static_cast<WindowInputData*>(focused->elements[1].data)->context = &input::gInputContextD66CA8;
        gLoginWindowRuntime.prefixD2F0B8.value26C = 0;
    }
    gLoginWindowRuntime.choicesCC4CA0.value00 = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        const auto petIndex = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[index].index);
        if ((*world::detail::BorrowPetRuntimeF633F8Storage())[petIndex].used == 0) {
            continue;
        }
        const std::int32_t available = world::detail::Sub_52E1D0(petIndex);
        if (available == 1) {
            gLoginWindowRuntime.choicesCC4CA0.pairs18[index][1] = available;
            gLoginWindowRuntime.choicesCC4CA0.pairs18[index][0] = petIndex;
            const std::int32_t output = gLoginWindowRuntime.choicesCC4CA0.value00;
            gLoginWindowRuntime.choicesCC4CA0.values04[output] = petIndex;
            const std::int32_t count = gLoginWindowRuntime.choicesCC4CA0.value00;
            gLoginWindowRuntime.choicesCC4CA0.value00 = count + 1;
        } else {
            gLoginWindowRuntime.choicesCC4CA0.pairs18[index][1] = 0;
            gLoginWindowRuntime.choicesCC4CA0.pairs18[index][0] = petIndex;
        }
    }
    if (world::gCurrentSceneType == 10) {
        for (auto& pair : gLoginWindowRuntime.choicesCC4CA0.pairs18) {
            pair[1] = 0;
        }
        gLoginWindowRuntime.choicesCC4CA0.value00 = 0;
    }
    if ((world::detail::ReadPlayerActionFlags() & 0x80u) != 0
        || world::detail::Sub_440B50() != 0) {
        for (auto& pair : gLoginWindowRuntime.choicesCC4CA0.pairs18) {
            pair[1] = 0;
        }
        gLoginWindowRuntime.choicesCC4CA0.value00 = 0;
    }
    static_cast<void>(WindowMailCompose_RefreshRecipients());
    WindowInstance* const groupWindow = gWindowStates[WindowId_GroupSelect].window;
    if (groupWindow != nullptr && gLoginWindowRuntime.prefixD2F0B8.value10 != 1) {
        groupWindow->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x004A5650。
void WindowMailCompose_Release()
{
    const std::int32_t category = gLoginWindowRuntime.prefixD2F0B8.value00;
    const std::int32_t target = gLoginWindowRuntime.prefixD2F0B8.value04;
    gLoginWindowRuntime.mailDraftCF4BC8.value00 = category;
    const std::int32_t mode = gLoginWindowRuntime.prefixD2F0B8.value10;
    gLoginWindowRuntime.mailDraftCF4BC8.value04 = target;
    gLoginWindowRuntime.mailDraftCF4BC8.value10 = mode;
    const std::int32_t pet = gLoginWindowRuntime.prefixD2F0B8.value08;
    gLoginWindowRuntime.mailDraftCF4BC8.value08 = pet;
    const std::int32_t oldSlot = gLoginWindowRuntime.prefixD2F0B8.value0C;
    gLoginWindowRuntime.mailDraftCF4BC8.value0C = oldSlot;
    cg::text::StoreGbk(reinterpret_cast<char*>(gLoginWindowRuntime.mailDraftCF4BC8.bytes14.data()), gLoginWindowRuntime.mailDraftCF4BC8.bytes14.size(), input::ContextText(input::gInputContextD66CA8));
    if (oldSlot != -1) {
        auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
        auto count = occupancy[oldSlot];
        gLoginWindowRuntime.prefixD2F0B8.value0C = -1;
        --count;
        gLoginWindowRuntime.prefixD2F0B8.value08 = -1;
        occupancy[oldSlot] = count;
    }
    WindowInstance* const groupWindow = gWindowStates[WindowId_GroupSelect].window;
    if (groupWindow != nullptr) {
        groupWindow->state |= 0x8000u;
    }
}

// 原函数：0x004A58E0。
std::int32_t WindowMailCompose_HandleInput(const std::int32_t, const std::int32_t state)
{
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        WindowInstance* const window = gCurrentWindow;
        if (gFocusedElementDataD45F94 != window->elements[1].data) {
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            static_cast<void>(WindowInput_ActivateElement(&gMailInputPrototype57E740, x, y, 1));
            input::SetContext_44B330(&input::gInputContextD66CA8);
            WindowInstance* const focused = gCurrentWindow;
            gFocusedElementDataD45F94 = focused->elements[1].data;
            static_cast<WindowInputData*>(focused->elements[1].data)->context = &input::gInputContextD66CA8;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (input::gCurrentInputData != &input::gInputContextD66CA8) {
        render::DrawInputText_4568A0(&input::gInputContextD66CA8);
    }
    return consumed;
}

// 原函数：0x004A5970。
std::int32_t WindowMailCompose_HandleRecipients(const std::int32_t index, const std::int32_t state)
{
    if (index != 2 && index != 3) {
        return 0;
    }
    std::int32_t consumed = 0;
    if (index == 2) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[2].data);
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3BA0A;
            gHoverTextD68AE8 = kPetMailHint58774C;
            consumed = 1;
        } else {
            graphic->imageNo = 0x3BA08;
        }
        if ((state & 2) != 0) {
            cg::text::StoreGbk(reinterpret_cast<char*>(gLoginWindowRuntime.mailDraftCF4BC8.bytes14.data()),
                gLoginWindowRuntime.mailDraftCF4BC8.bytes14.size(), input::ContextText(input::gInputContextD66CA8));
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PetMailCompose, 1, 0, nullptr));
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Inventory, 1, 0, nullptr));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[2].data)->imageNo = 0x3BA09;
        }
        return consumed;
    }
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[3].data);
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3BA1D;
        gHoverTextD68AE8 = kReplyMailHint587730;
        consumed = 1;
    } else {
        graphic->imageNo = 0x3BA1B;
    }
    char timestamp[64];
    std::wstring packet;
    std::wstring combined;
    std::array<char, 300> storedText; // 原邮件0x140字节记录的+0x14正文，含NUL。
    bool closeAfterSend = false;
    const std::int32_t mode = gLoginWindowRuntime.prefixD2F0B8.value10;
    if (mode != 0) {
        if ((state & 2) != 0) {
            const std::int32_t groupMode = gLoginWindowRuntime.mailGroupCC1A30.mode;
            if (groupMode == 0 || groupMode == 1 || groupMode == 2) {
                if (input::gInputContextD66CA8.empty()
                    || gLoginWindowRuntime.mailGroupCC1A30.count <= 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    combined = input::ContextText(input::gInputContextD66CA8);
                    cg::text::StoreGbk(storedText.data(), storedText.size(), combined);
                    packet = input::EscapeTextAndNotify(combined, 1024);
                    for (std::int32_t row = 0; row < gLoginWindowRuntime.mailGroupCC1A30.count; ++row) {
                        if (gLoginWindowRuntime.mailGroupCC1A30.selected[row] != 1) {
                            continue;
                        }
                        if (groupMode == 0) {
                            const std::int32_t contact = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                            const std::uintptr_t socket = platform::gConnectionSocket;
                            static_cast<void>(network::Sub_510FA0(socket, contact, packet, 0));
                            WindowMail_FormatTimestamp(timestamp);
                            world::detail::Sub_5191C0(contact, 1, timestamp, storedText.data());
                        } else if (groupMode == 1) {
                            const std::int32_t member = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                            const std::uintptr_t socket = platform::gConnectionSocket;
                            const std::int32_t key = login::detail::gOrderedRecords1051B14[member].value0C;
                            static_cast<void>(network::Sub_511480(socket, key, packet, 0));
                            WindowMail_FormatTimestamp(timestamp);
                            static_cast<void>(world::detail::Sub_519370(
                                key, 1, timestamp, storedText.data()));
                        } else {
                            const std::int32_t group = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                            const std::uintptr_t socket = platform::gConnectionSocket;
                            static_cast<void>(network::Sub_511480(socket, group + 0x400, packet, 0));
                            for (std::int32_t member = 0; member < 200; ++member) {
                                const std::int32_t memberGroup = login::detail::gOrderedRecords1051B14[member].value00;
                                const std::int32_t currentGroup = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                                if (memberGroup == currentGroup) {
                                    WindowMail_FormatTimestamp(timestamp);
                                    const std::int32_t key = login::detail::gOrderedRecords1051B14[member].value0C;
                                    static_cast<void>(world::detail::Sub_519370(
                                        key, 1, timestamp, storedText.data()));
                                }
                            }
                        }
                    }
                    static_cast<void>(world::detail::Sub_519300());
                    static_cast<void>(world::detail::Sub_519550());
                    static_cast<void>(audio::PlaySe_52C450(78, 320));
                    closeAfterSend = true;
                }
            }
            consumed = 1;
        }
    } else {
        const std::int32_t category = gLoginWindowRuntime.prefixD2F0B8.value00;
        if ((category == 0 || category == 1 || category == 2) && (state & 2) != 0) {
            if (input::gInputContextD66CA8.empty()) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else if (category == 2) {
                const std::int32_t contact = gLoginWindowRuntime.prefixD2F0B8.value04;
                const auto& record = world::detail::ReadMiniMailContact1075A90(contact);
                const std::int32_t key = record.key04;
                cg::text::StoreGbk(storedText.data(), storedText.size(), input::ContextText(input::gInputContextD66CA8));
                const std::wstring escapedName = input::EscapeText(cg::text::FromGbkWide(record.name10.data()), 256);
                const std::wstring escapedText = input::EscapeText(input::ContextText(input::gInputContextD66CA8), 1024);
                combined = std::to_wstring(key) + L"|" + escapedName + L"|" + escapedText;
                packet = input::EscapeTextAndNotify(combined, 1024);
                static_cast<void>(network::Sub_511840(platform::gConnectionSocket, packet, 0));
                static_cast<void>(world::detail::Sub_534520(timestamp));
                const std::int32_t currentContact = gLoginWindowRuntime.prefixD2F0B8.value04;
                world::detail::Sub_5343C0(currentContact, 1, timestamp, storedText.data());
                static_cast<void>(audio::PlaySe_52C450(78, 320));
                closeAfterSend = true;
            } else {
                std::int32_t target;
                if (category == 1) {
                    const std::int32_t member = gLoginWindowRuntime.prefixD2F0B8.value04;
                    target = login::detail::gOrderedRecords1051B14[member].value0C;
                    combined = input::ContextText(input::gInputContextD66CA8);
                    cg::text::StoreGbk(storedText.data(), storedText.size(), combined);
                } else {
                    target = gLoginWindowRuntime.prefixD2F0B8.value04;
                    combined = input::ContextText(input::gInputContextD66CA8);
                    cg::text::StoreGbk(storedText.data(), storedText.size(), combined);
                }
                packet = input::EscapeTextAndNotify(combined, 1024);
                const std::uintptr_t socket = platform::gConnectionSocket;
                if (category == 1) {
                    static_cast<void>(network::Sub_511480(socket, target, packet, 0));
                } else {
                    static_cast<void>(network::Sub_510FA0(socket, target, packet, 0));
                }
                WindowMail_FormatTimestamp(timestamp);
                if (category == 1) {
                    static_cast<void>(world::detail::Sub_519370(
                        target, 1, timestamp, storedText.data()));
                } else {
                    world::detail::Sub_5191C0(target, 1, timestamp, storedText.data());
                }
                static_cast<void>(world::detail::Sub_519300());
                static_cast<void>(world::detail::Sub_519550());
                static_cast<void>(audio::PlaySe_52C450(78, 320));
                closeAfterSend = true;
            }
            consumed = 1;
        }
    }
    if (closeAfterSend) {
        WindowInstance* const window = gCurrentWindow;
        input::gInputContextD66CA8.clearText();
        window->state |= 0x8000u;
    }
    if ((state & 8) != 0) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[3].data)->imageNo = 0x3BA1C;
    }
    return consumed;
}

// 原函数：0x004A6160。
std::uint8_t WindowMailCompose_RefreshRecipients()
{
    const std::int32_t mode = gLoginWindowRuntime.prefixD2F0B8.value10;
    if (mode != 0) {
        const std::int32_t groupMode = gLoginWindowRuntime.mailGroupCC1A30.mode;
        if (groupMode == 0 || groupMode == 1 || groupMode == 2) {
            auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[4].data);
            text->text = kGroupMailText587780;
            gCurrentWindow->elements[2].visible = 0;
        }
    } else {
        const std::int32_t category = gLoginWindowRuntime.prefixD2F0B8.value00;
        const char* name;
        switch (category) {
        case 0:
            name = world::detail::gContactsF19FB0[gLoginWindowRuntime.prefixD2F0B8.value04].name10.data();
            break;
        case 1:
            name = login::detail::gOrderedRecords1051B14[gLoginWindowRuntime.prefixD2F0B8.value04].text18;
            break;
        case 2:
            name = world::detail::ReadMiniMailContact1075A90(
                gLoginWindowRuntime.prefixD2F0B8.value04).name10.data();
            break;
        }
        if (category == 0 || category == 1 || category == 2) {
            auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[4].data);
            text->text = cg::text::FromGbkWide(name);
        }
        if (gLoginWindowRuntime.prefixD2F0B8.value00 == 2) {
            gCurrentWindow->elements[2].visible = 0;
        } else {
            const std::int32_t count = gLoginWindowRuntime.choicesCC4CA0.value00;
            gCurrentWindow->elements[2].visible = count != 0 ? 1 : 0;
        }
    }
    const WindowInstance* const window = gCurrentWindow;
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    return WindowInput_ActivateElement(&gMailInputPrototype57E740, x, y, 2);
}

} // namespace detail

} // namespace cg::ui

namespace cg::ui::detail {

namespace {

WindowGraphicData gWindowGraphic5700B8 {0x3BA0B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005700B8。

WindowGraphicData gWindowGraphic5700D0 {0x3BA06, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005700D0。

WindowGraphicData gWindowGraphic5700E8 {0x3BA07, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005700E8。

WindowGraphicData gWindowGraphic570148 {0x3BA08, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570148。

} // namespace

// 原型：窗口22；原全局：0x0054EA20。
WindowPrototype gWindowPrototypeMailCompose {0, 4, 383, 81, 211, 210, 0x80010101u, 9, gWindowElements5701F8, &WindowMailCompose_Update, &WindowRuntime_DrawDefault, &WindowMailCompose_Release}; // 22: 54EA20

WindowElementPrototype gWindowElements5701F8[9] { // 原全局：0x005701F8。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic570190}, &WindowRuntime_ButtonClose},
    {WindowElementType_Input, 22, 70, 170, 120, 1, {}, &WindowMailCompose_HandleInput},
    {WindowElementType_Graphic, 16, 28, 56, 16, 1, {.graphic = &gWindowGraphic570148}, &WindowMailCompose_HandleRecipients},
    {WindowElementType_Graphic, 154, 28, 50, 18, 1, {.graphic = &gWindowGraphic570160}, &WindowMailCompose_HandleRecipients},
    {WindowElementType_Text, 21, 48, 0, 0, 1, {.text = &gWindowText5701F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 46, 0, 0, 1, {.graphic = &gWindowGraphic5700B8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5700D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 23, 0, 0, 1, {.graphic = &gWindowGraphic5700E8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 207, 0, 25, 100, 1, {}, &WindowElement_HandleSideArea},
};

} // namespace cg::ui::detail
