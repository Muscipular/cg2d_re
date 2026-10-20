#include "../WindowRuntimeInternal.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowMailComposeElementId : ElementId {
    ButtonClose = 1,
    HandleInput = 2,
    HandleRecipients = 3,
    HandleRecipients_4 = 4,
    Element5 = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    HandleSideArea = 9,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004A53F0。
static std::int32_t WindowMailCompose_Update(std::int32_t value); // 发信初始化、宠物候选和收件人刷新。

// 原函数：0x004A5970。
static std::int32_t WindowMailCompose_HandleRecipients(std::int32_t index, std::int32_t state); // 宠物邮件切换、单发/群发完整六路径。

// 原函数：0x004A6160。
static std::uint8_t WindowMailCompose_RefreshRecipients(); // 收件人/宠物按钮显隐及输入位置刷新，透传476570低W8。

// 原函数：0x004A53F0。
static std::int32_t WindowMailCompose_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic5700B8 {0x3BA0B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005700B8。

static WindowGraphicData gWindowGraphic5700D0 {0x3BA06, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005700D0。

static WindowGraphicData gWindowGraphic5700E8 {0x3BA07, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005700E8。

static WindowGraphicData gWindowGraphic570148 {0x3BA08, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570148。

static WindowElementPrototype gWindowElements5701F8[9] { // 原全局：0x005701F8。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic570190}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowMailComposeElementId::ButtonClose)},
    {WindowElementType_Input, 22, 70, 170, 120, 1, {}, &WindowMailCompose_HandleInput, -1, static_cast<ElementId>(WindowMailComposeElementId::HandleInput)},
    {WindowElementType_Graphic, 16, 28, 56, 16, 1, {.graphic = &gWindowGraphic570148}, &WindowMailCompose_HandleRecipients, -1, static_cast<ElementId>(WindowMailComposeElementId::HandleRecipients)},
    {WindowElementType_Graphic, 154, 28, 50, 18, 1, {.graphic = &gWindowGraphic570160}, &WindowMailCompose_HandleRecipients, -1, static_cast<ElementId>(WindowMailComposeElementId::HandleRecipients_4)},
    {WindowElementType_Text, 21, 48, 0, 0, 1, {.text = &gWindowText5701F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMailComposeElementId::Element5)},
    {WindowElementType_Graphic, 16, 46, 0, 0, 1, {.graphic = &gWindowGraphic5700B8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMailComposeElementId::Element6)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5700D0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMailComposeElementId::Element7)},
    {WindowElementType_Graphic, 14, 23, 0, 0, 1, {.graphic = &gWindowGraphic5700E8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowMailComposeElementId::Element8)},
    {WindowElementType_EventArea, 207, 0, 25, 100, 1, {}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowMailComposeElementId::HandleSideArea)},
};

// 原型：窗口22；原全局：0x0054EA20。
WindowPrototype gWindowPrototypeMailCompose {0, 4, 383, 81, 211, 210, 0x80010101u, 9, gWindowElements5701F8, &WindowMailCompose_Update, &WindowRuntime_DrawDefault, &WindowMailCompose_Release}; // 22: 54EA20

// 窗口函数实现。

// 原函数：0x004A53D0。
WindowInstance* WindowMailCompose_Open()
{
    gLoginWindowRuntime.prefixD2F0B8.value26C = 1;
    return WindowRuntime_Open(WindowId_MailCompose);
}

// 原函数：0x004A53F0。
static std::int32_t WindowMailCompose_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
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
                auto* const occupancy = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
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
        const WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        if (WindowInput_ActivateElement(&gMailInputPrototype57E740, x, y, 0) == 0) {
            gMailInputPrototype57E740.input.context = &cg::ui::detail::gInputContextD66CA8;
            gMailInputPrototype57E740.input.kind = WindowInputReferenceKind::Context;
        }
        cg::ui::detail::gInputContextD66CA8.maxCharPerLine = 28;
        gMailInputPrototype57E740.maxCharPerLine = 28;
        cg::ui::detail::gInputContextD66CA8.maxLine = 10;
        gMailInputPrototype57E740.maxLine = 10;
        cg::ui::detail::gInputContextD66CA8.field14C = 12;
        gMailInputPrototype57E740.value224 = 12;
        cg::ui::detail::gInputContextD66CA8.encrypt = 0;
        gMailInputPrototype57E740.encrypt = 0;
        input::SetContext_44B330(&cg::ui::detail::gInputContextD66CA8);
        WindowInstance* const focused = currentWindow;
        gMailInputPrototype57E740.text.clear();
        auto* const handleInputElement = focused->FindElementById(WindowMailComposeElementId::HandleInput);
        if (handleInputElement) {
            gFocusedElementDataD45F94 = handleInputElement->data.input;
        }
        if (auto* const handleInputElement2 = focused->FindElementById(WindowMailComposeElementId::HandleInput); handleInputElement2) handleInputElement2->data.input->context = &cg::ui::detail::gInputContextD66CA8;
        gLoginWindowRuntime.prefixD2F0B8.value26C = 0;
    }
    gLoginWindowRuntime.choicesCC4CA0.value00 = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        const auto petIndex = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[index].index);
        if (world::gPetRuntimeRecords[petIndex].used == 0) {
            continue;
        }
        const std::int32_t available = world::HasPetMailTechnique_52E1D0(petIndex);
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
    if ((world::ReadPlayerActionFlags() & 0x80u) != 0
        || world::IsObservationSceneTransitionActive_440B50() != 0) {
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
    cg::text::StoreGbk(reinterpret_cast<char*>(gLoginWindowRuntime.mailDraftCF4BC8.bytes14.data()), gLoginWindowRuntime.mailDraftCF4BC8.bytes14.size(), input::ContextText(cg::ui::detail::gInputContextD66CA8));
    if (oldSlot != -1) {
        auto* const occupancy = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
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
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const handleInputElement = window->FindElementById(WindowMailComposeElementId::HandleInput);
        if (handleInputElement) {
            if (gFocusedElementDataD45F94 != handleInputElement->data.input) {
                const std::int32_t y = window->y;
                const std::int32_t x = window->x;
                static_cast<void>(WindowInput_ActivateElement(&gMailInputPrototype57E740, x, y, 1));
                input::SetContext_44B330(&cg::ui::detail::gInputContextD66CA8);
                WindowInstance* const focused = currentWindow;
                auto* const handleInputElement2 = focused->FindElementById(WindowMailComposeElementId::HandleInput);
                if (handleInputElement2) {
                    gFocusedElementDataD45F94 = handleInputElement2->data.input;
                }
                if (auto* const handleInputElement3 = focused->FindElementById(WindowMailComposeElementId::HandleInput); handleInputElement3) handleInputElement3->data.input->context = &cg::ui::detail::gInputContextD66CA8;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
        consumed = 1;
    }
    if (input::gCurrentInputData != &cg::ui::detail::gInputContextD66CA8) {
        render::DrawInputText_4568A0(&cg::ui::detail::gInputContextD66CA8);
    }
    return consumed;
}

// 原函数：0x004A5970。
static std::int32_t WindowMailCompose_HandleRecipients(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    if (index != 2 && index != 3) {
        return 0;
    }
    std::int32_t consumed = 0;
    if (index == 2) {
        auto* const handleRecipientsElement = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients);
        if (handleRecipientsElement) {
            auto* const graphic = handleRecipientsElement->data.graphic;
            if ((state & 1) != 0) {
                graphic->imageNo = 0x3BA0A;
                gHoverTextD68AE8 = L"使用宠物邮件。" /* 原常量参考：0x0058774C。 */;
                consumed = 1;
            } else {
                graphic->imageNo = 0x3BA08;
            }
        }
        if ((state & 2) != 0) {
            cg::text::StoreGbk(reinterpret_cast<char*>(gLoginWindowRuntime.mailDraftCF4BC8.bytes14.data()),
                gLoginWindowRuntime.mailDraftCF4BC8.bytes14.size(), input::ContextText(cg::ui::detail::gInputContextD66CA8));
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PetMailCompose, 1, 0, nullptr));
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Inventory, 1, 0, nullptr));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            if (auto* const handleRecipientsElement2 = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients); handleRecipientsElement2) handleRecipientsElement2->data.graphic->imageNo = 0x3BA09;
        }
        return consumed;
    }
    auto* const handleRecipients_4Element = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients_4);
    if (handleRecipients_4Element) {
        auto* const graphic = handleRecipients_4Element->data.graphic;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3BA1D;
            gHoverTextD68AE8 = kReplyMailHint587730;
            consumed = 1;
        } else {
            graphic->imageNo = 0x3BA1B;
        }
    }
    std::wstring timestamp;
    std::wstring packet;
    std::wstring combined;
    std::wstring storedText; // 内部正文；写文件前仍执行原300字节容量预检。
    bool closeAfterSend = false;
    const std::int32_t mode = gLoginWindowRuntime.prefixD2F0B8.value10;
    if (mode != 0) {
        if ((state & 2) != 0) {
            const std::int32_t groupMode = gLoginWindowRuntime.mailGroupCC1A30.mode;
            if (groupMode == 0 || groupMode == 1 || groupMode == 2) {
                if (cg::ui::detail::gInputContextD66CA8.empty()
                    || gLoginWindowRuntime.mailGroupCC1A30.count <= 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    combined = input::ContextText(cg::ui::detail::gInputContextD66CA8);
                    {
                        char fileBytes[300]{};
                        cg::text::StoreGbk(fileBytes, sizeof(fileBytes), combined);
                        storedText = combined;
                    }
                    packet = input::EscapeTextAndNotify(combined, 1024);
                    for (std::int32_t row = 0; row < gLoginWindowRuntime.mailGroupCC1A30.count; ++row) {
                        if (gLoginWindowRuntime.mailGroupCC1A30.selected[row] != 1) {
                            continue;
                        }
                        if (groupMode == 0) {
                            const std::int32_t contact = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                            const std::uintptr_t socket = platform::gConnectionSocket;
                            static_cast<void>(network::SendContactMail_510FA0(socket, contact, packet, 0));
                            WindowMail_FormatTimestamp(timestamp);
                            world::detail::PrependContactMailMessage_5191C0(contact, 1, timestamp, storedText);
                        } else if (groupMode == 1) {
                            const std::int32_t member = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                            const std::uintptr_t socket = platform::gConnectionSocket;
                            const std::int32_t key = cg::world::gOrderedRecords1051B14[member].value0C;
                            static_cast<void>(network::SendGuildMail_511480(socket, key, packet, 0));
                            WindowMail_FormatTimestamp(timestamp);
                            static_cast<void>(world::detail::PrependGuildMailMessage_519370(
                                key, 1, timestamp, storedText));
                        } else {
                            const std::int32_t group = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                            const std::uintptr_t socket = platform::gConnectionSocket;
                            static_cast<void>(network::SendGuildMail_511480(socket, group + 0x400, packet, 0));
                            for (std::int32_t member = 0; member < 200; ++member) {
                                const std::int32_t memberGroup = cg::world::gOrderedRecords1051B14[member].value00;
                                const std::int32_t currentGroup = gLoginWindowRuntime.mailGroupCC1A30.indices[row];
                                if (memberGroup == currentGroup) {
                                    WindowMail_FormatTimestamp(timestamp);
                                    const std::int32_t key = cg::world::gOrderedRecords1051B14[member].value0C;
                                    static_cast<void>(world::detail::PrependGuildMailMessage_519370(
                                        key, 1, timestamp, storedText));
                                }
                            }
                        }
                    }
                    static_cast<void>(cg::world::detail::SaveContactMailFile_519300());
                    static_cast<void>(cg::world::detail::SaveGuildMailFile_519550());
                    static_cast<void>(audio::PlaySe_52C450(78, 320));
                    closeAfterSend = true;
                }
            }
            consumed = 1;
        }
    } else {
        const std::int32_t category = gLoginWindowRuntime.prefixD2F0B8.value00;
        if ((category == 0 || category == 1 || category == 2) && (state & 2) != 0) {
            if (cg::ui::detail::gInputContextD66CA8.empty()) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else if (category == 2) {
                const std::int32_t contact = gLoginWindowRuntime.prefixD2F0B8.value04;
                const auto& record = world::detail::ReadMiniMailContact1075A90(contact);
                const std::int32_t key = record.key04;
                {
                    storedText = input::ContextText(cg::ui::detail::gInputContextD66CA8);
                    char fileBytes[300]{};
                    cg::text::StoreGbk(fileBytes, sizeof(fileBytes), storedText);
                }
                const std::wstring escapedName = input::EscapeText(record.name10, 256);
                const std::wstring escapedText = input::EscapeText(input::ContextText(cg::ui::detail::gInputContextD66CA8), 1024);
                combined = std::to_wstring(key) + L"|" + escapedName + L"|" + escapedText;
                packet = input::EscapeTextAndNotify(combined, 1024);
                static_cast<void>(network::SendMiniMail_511840(platform::gConnectionSocket, packet, 0));
                static_cast<void>(world::detail::FormatMailTimestamp_534520(timestamp));
                const std::int32_t currentContact = gLoginWindowRuntime.prefixD2F0B8.value04;
                world::detail::PrependMiniMailMessage_5343C0(currentContact, 1, timestamp, storedText);
                static_cast<void>(audio::PlaySe_52C450(78, 320));
                closeAfterSend = true;
            } else {
                std::int32_t target;
                if (category == 1) {
                    const std::int32_t member = gLoginWindowRuntime.prefixD2F0B8.value04;
                    target = cg::world::gOrderedRecords1051B14[member].value0C;
                    combined = input::ContextText(cg::ui::detail::gInputContextD66CA8);
                    {
                        char fileBytes[300]{};
                        cg::text::StoreGbk(fileBytes, sizeof(fileBytes), combined);
                        storedText = combined;
                    }
                } else {
                    target = gLoginWindowRuntime.prefixD2F0B8.value04;
                    combined = input::ContextText(cg::ui::detail::gInputContextD66CA8);
                    {
                        char fileBytes[300]{};
                        cg::text::StoreGbk(fileBytes, sizeof(fileBytes), combined);
                        storedText = combined;
                    }
                }
                packet = input::EscapeTextAndNotify(combined, 1024);
                const std::uintptr_t socket = platform::gConnectionSocket;
                if (category == 1) {
                    static_cast<void>(network::SendGuildMail_511480(socket, target, packet, 0));
                } else {
                    static_cast<void>(network::SendContactMail_510FA0(socket, target, packet, 0));
                }
                WindowMail_FormatTimestamp(timestamp);
                if (category == 1) {
                    static_cast<void>(world::detail::PrependGuildMailMessage_519370(
                        target, 1, timestamp, storedText));
                } else {
                    world::detail::PrependContactMailMessage_5191C0(target, 1, timestamp, storedText);
                }
                static_cast<void>(cg::world::detail::SaveContactMailFile_519300());
                static_cast<void>(cg::world::detail::SaveGuildMailFile_519550());
                static_cast<void>(audio::PlaySe_52C450(78, 320));
                closeAfterSend = true;
            }
            consumed = 1;
        }
    }
    if (closeAfterSend) {
        WindowInstance* const window = currentWindow;
        cg::ui::detail::gInputContextD66CA8.clearText();
        window->state |= 0x8000u;
    }
    if ((state & 8) != 0) {
        if (auto* const handleRecipients_4Element2 = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients_4); handleRecipients_4Element2) handleRecipients_4Element2->data.graphic->imageNo = 0x3BA1C;
    }
    return consumed;
}

// 原函数：0x004A6160。
static std::uint8_t WindowMailCompose_RefreshRecipients()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return {};
    const std::int32_t mode = gLoginWindowRuntime.prefixD2F0B8.value10;
    if (mode != 0) {
        const std::int32_t groupMode = gLoginWindowRuntime.mailGroupCC1A30.mode;
        if (groupMode == 0 || groupMode == 1 || groupMode == 2) {
            auto* const element5 = currentWindow->FindElementById(WindowMailComposeElementId::Element5);
            if (element5) {
                auto* const text = element5->data.text;
                text->text = L"群组邮件" /* 原常量参考：0x00587780。 */;
            }
            if (auto* const handleRecipientsElement = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients); handleRecipientsElement) handleRecipientsElement->visible = 0;
        }
    } else {
        const std::int32_t category = gLoginWindowRuntime.prefixD2F0B8.value00;
        std::wstring name; // 下方只在三个已赋值的category分支读取。
        switch (category) {
        case 0:
            name = world::detail::gContactsF19FB0[gLoginWindowRuntime.prefixD2F0B8.value04].name10.data();
            break;
        case 1:
            name = cg::world::gOrderedRecords1051B14[gLoginWindowRuntime.prefixD2F0B8.value04].name18;
            break;
        case 2:
            name = world::detail::ReadMiniMailContact1075A90(
                gLoginWindowRuntime.prefixD2F0B8.value04).name10.data();
            break;
        }
        if (category == 0 || category == 1 || category == 2) {
            auto* const element5 = currentWindow->FindElementById(WindowMailComposeElementId::Element5);
            if (element5) {
                auto* const text = element5->data.text;
                text->text = name;
            }
        }
        if (gLoginWindowRuntime.prefixD2F0B8.value00 == 2) {
            if (auto* const handleRecipientsElement = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients); handleRecipientsElement) handleRecipientsElement->visible = 0;
        } else {
            const std::int32_t count = gLoginWindowRuntime.choicesCC4CA0.value00;
            if (auto* const handleRecipientsElement = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients); handleRecipientsElement) handleRecipientsElement->visible = count != 0 ? 1 : 0;
        }
    }
    const WindowInstance* const window = currentWindow;
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    return WindowInput_ActivateElement(&gMailInputPrototype57E740, x, y, 2);
}

} // namespace detail

} // namespace cg::ui
