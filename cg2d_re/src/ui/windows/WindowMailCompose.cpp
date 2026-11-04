#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
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
static std::int32_t WindowMailCompose_Update(WindowInstance& callbackWindow, std::int32_t value); // 发信初始化、宠物候选和收件人刷新。

// 原函数：0x004A5970。
static std::int32_t WindowMailCompose_HandleRecipients(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 宠物邮件切换、单发/群发完整六路径。

// 原函数：0x004A6160。
static std::uint8_t WindowMailCompose_RefreshRecipients(WindowInstance* callbackWindow); // 收件人/宠物按钮显隐及输入位置刷新，透传476570低W8。

// 原函数：0x004A53F0。
static std::int32_t WindowMailCompose_Update(WindowInstance& callbackWindow, std::int32_t value);

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
WindowInstance* WindowMailCompose_Open(session::ClientSession& session)
{
    auto& business = session.UiFrame().native.business;
    business.prefixD2F0B8.value26C = 1;
    return WindowRuntime_Open(session, WindowId_MailCompose);
}

// 原函数：0x004A53F0。
static std::int32_t WindowMailCompose_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& inventory = callbackWindow.ownerSession->UiFrame().native.inventory;
    if (value == -1) {
        if (business.prefixD2F0B8.value10 != 1) {
            WindowInstance* const window = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_GroupSelect].window;
            if (window != nullptr) {
                window->state |= 0x8000u;
            }
        } else {
            if (business.prefixD2F0B8.value26C != 0) {
                business.mailGroupCC1A30.initialize = 1;
            }
            static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_GroupSelect, 1, 0, nullptr));
        }
        WindowInstance* const petWindow = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_PetMailCompose].window;
        if (petWindow != nullptr) {
            petWindow->state |= 0x8000u;
            const std::int32_t oldSlot = business.prefixD2F0B8.value0C;
            if (oldSlot != -1) {
                auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
                auto count = occupancy[oldSlot];
                business.prefixD2F0B8.value0C = -1;
                --count;
                business.prefixD2F0B8.value08 = -1;
                occupancy[oldSlot] = count;
            }
        }
        const WindowRuntimePrefixD2F0B8 previous = business.mailDraftCF4BC8;
        const WindowRuntimePrefixD2F0B8 current = business.prefixD2F0B8;
        if (WindowRuntime_MatchesRuntimePrefix(current, previous) != 0) {
            inputs.mailPrototype.text = cg::text::FromGbkWide(reinterpret_cast<const char*>(business.mailDraftCF4BC8.bytes14.data()));
        }
        const WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        if (WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.mailPrototype, x, y, 0) == 0) {
            inputs.mailPrototype.input.context = &inputs.mail;
            inputs.mailPrototype.input.kind = WindowInputReferenceKind::Context;
        }
        inputs.mail.maxCharPerLine = 28;
        inputs.mailPrototype.maxCharPerLine = 28;
        inputs.mail.maxLine = 10;
        inputs.mailPrototype.maxLine = 10;
        inputs.mail.lineHeight = 12;
        inputs.mailPrototype.value224 = 12;
        inputs.mail.encrypt = 0;
        inputs.mailPrototype.encrypt = 0;
        input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.mail);
        WindowInstance* const focused = currentWindow;
        inputs.mailPrototype.text.clear();
        auto* const handleInputElement = focused->FindElementById(WindowMailComposeElementId::HandleInput);
        if (handleInputElement) {
            callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = handleInputElement->data.input;
        }
        if (auto* const handleInputElement2 = focused->FindElementById(WindowMailComposeElementId::HandleInput); handleInputElement2) handleInputElement2->data.input->context = &inputs.mail;
        business.prefixD2F0B8.value26C = 0;
    }
    business.choicesCC4CA0.value00 = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        const auto petIndex = std::bit_cast<std::int16_t>(callbackWindow.ownerSession->WorldScene().petPairs[index].index);
        if ((*callbackWindow.ownerSession->WorldScene().petRecords)[petIndex].used == 0) {
            continue;
        }
        const std::int32_t available = world::HasPetMailTechnique_52E1D0(*callbackWindow.ownerSession, petIndex);
        if (available == 1) {
            business.choicesCC4CA0.pairs18[index][1] = available;
            business.choicesCC4CA0.pairs18[index][0] = petIndex;
            const std::int32_t output = business.choicesCC4CA0.value00;
            business.choicesCC4CA0.values04[output] = petIndex;
            const std::int32_t count = business.choicesCC4CA0.value00;
            business.choicesCC4CA0.value00 = count + 1;
        } else {
            business.choicesCC4CA0.pairs18[index][1] = 0;
            business.choicesCC4CA0.pairs18[index][0] = petIndex;
        }
    }
    if (callbackWindow.ownerSession->WorldScene().sceneType == 10) {
        for (auto& pair : business.choicesCC4CA0.pairs18) {
            pair[1] = 0;
        }
        business.choicesCC4CA0.value00 = 0;
    }
    if ((world::ReadPlayerActionFlags(*callbackWindow.ownerSession) & 0x80u) != 0
        || world::IsObservationSceneTransitionActive_440B50(*callbackWindow.ownerSession) != 0) {
        for (auto& pair : business.choicesCC4CA0.pairs18) {
            pair[1] = 0;
        }
        business.choicesCC4CA0.value00 = 0;
    }
    static_cast<void>(WindowMailCompose_RefreshRecipients(&callbackWindow));
    WindowInstance* const groupWindow = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_GroupSelect].window;
    if (groupWindow != nullptr && business.prefixD2F0B8.value10 != 1) {
        groupWindow->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x004A5650。
void WindowMailCompose_Release(WindowInstance& releasedWindow)
{
    auto& inventory = releasedWindow.ownerSession->UiFrame().native.inventory;
    auto& business = releasedWindow.ownerSession->UiFrame().native.business;
    auto& inputs = releasedWindow.ownerSession->UiFrame().native.inputs;
    const std::int32_t category = business.prefixD2F0B8.value00;
    const std::int32_t target = business.prefixD2F0B8.value04;
    business.mailDraftCF4BC8.value00 = category;
    const std::int32_t mode = business.prefixD2F0B8.value10;
    business.mailDraftCF4BC8.value04 = target;
    business.mailDraftCF4BC8.value10 = mode;
    const std::int32_t pet = business.prefixD2F0B8.value08;
    business.mailDraftCF4BC8.value08 = pet;
    const std::int32_t oldSlot = business.prefixD2F0B8.value0C;
    business.mailDraftCF4BC8.value0C = oldSlot;
    cg::text::StoreGbk(reinterpret_cast<char*>(business.mailDraftCF4BC8.bytes14.data()), business.mailDraftCF4BC8.bytes14.size(), input::ContextText(inputs.mail));
    if (oldSlot != -1) {
        auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
        auto count = occupancy[oldSlot];
        business.prefixD2F0B8.value0C = -1;
        --count;
        business.prefixD2F0B8.value08 = -1;
        occupancy[oldSlot] = count;
    }
    WindowInstance* const groupWindow = WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_GroupSelect].window;
    if (groupWindow != nullptr) {
        groupWindow->state |= 0x8000u;
    }
}

// 原函数：0x004A58E0。
std::int32_t WindowMailCompose_HandleInput(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    if ((state & MouseFlag_LDownEvent) != 0) {
        WindowInstance* const window = currentWindow;
        auto* const handleInputElement = window->FindElementById(WindowMailComposeElementId::HandleInput);
        if (handleInputElement) {
            if (callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData != handleInputElement->data.input) {
                const std::int32_t y = window->y;
                const std::int32_t x = window->x;
                static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.mailPrototype, x, y, 1));
                input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.mail);
                WindowInstance* const focused = currentWindow;
                auto* const handleInputElement2 = focused->FindElementById(WindowMailComposeElementId::HandleInput);
                if (handleInputElement2) {
                    callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = handleInputElement2->data.input;
                }
                if (auto* const handleInputElement3 = focused->FindElementById(WindowMailComposeElementId::HandleInput); handleInputElement3) handleInputElement3->data.input->context = &inputs.mail;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
        consumed = 1;
    }
    if (callbackWindow.ownerSession->UiFrame().inputState.currentContext != &inputs.mail) {
        render::DrawInputText_4568A0(*callbackWindow.ownerSession, &inputs.mail);
    }
    return consumed;
}

// 原函数：0x004A5970。
static std::int32_t WindowMailCompose_HandleRecipients(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (elementId != static_cast<ElementId>(WindowMailComposeElementId::HandleRecipients) && elementId != static_cast<ElementId>(WindowMailComposeElementId::HandleRecipients_4)) {
        return 0;
    }
    std::int32_t consumed = 0;
    if (elementId == static_cast<ElementId>(WindowMailComposeElementId::HandleRecipients)) {
        auto* const handleRecipientsElement = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients);
        if (handleRecipientsElement) {
            auto* const graphic = handleRecipientsElement->data.graphic;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = 0x3BA0A;
                uiFrame.hoverText = L"使用宠物邮件。" /* 原常量参考：0x0058774C。 */;
                consumed = 1;
            } else {
                graphic->imageNo = 0x3BA08;
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            cg::text::StoreGbk(reinterpret_cast<char*>(business.mailDraftCF4BC8.bytes14.data()),
                business.mailDraftCF4BC8.bytes14.size(), input::ContextText(inputs.mail));
            static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_PetMailCompose, 1, 0, nullptr));
            static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_Inventory, 1, 0, nullptr));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            if (auto* const handleRecipientsElement2 = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients); handleRecipientsElement2) handleRecipientsElement2->data.graphic->imageNo = 0x3BA09;
        }
        return consumed;
    }
    auto* const handleRecipients_4Element = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients_4);
    if (handleRecipients_4Element) {
        auto* const graphic = handleRecipients_4Element->data.graphic;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3BA1D;
            uiFrame.hoverText = kReplyMailHint587730;
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
    const std::int32_t mode = business.prefixD2F0B8.value10;
    if (mode != 0) {
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t groupMode = business.mailGroupCC1A30.mode;
            if (groupMode == 0 || groupMode == 1 || groupMode == 2) {
                if (inputs.mail.empty()
                    || business.mailGroupCC1A30.count <= 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    combined = input::ContextText(inputs.mail);
                    {
                        char fileBytes[300]{};
                        cg::text::StoreGbk(fileBytes, sizeof(fileBytes), combined);
                        storedText = combined;
                    }
                    packet = input::EscapeTextAndNotify(combined, 1024);
                    for (std::int32_t row = 0; row < business.mailGroupCC1A30.count; ++row) {
                        if (business.mailGroupCC1A30.selected[row] != 1) {
                            continue;
                        }
                        if (groupMode == 0) {
                            const std::int32_t contact = business.mailGroupCC1A30.indices[row];
                            const auto networkSession = callbackWindow.ownerSession->Connection().CaptureSession();
                            static_cast<void>(callbackWindow.ownerSession->Connection().SendContactMail(contact, packet, 0));
                            WindowMail_FormatTimestamp(*callbackWindow.ownerSession, timestamp);
                            world::detail::PrependContactMailMessage_5191C0(*callbackWindow.ownerSession, contact, 1, timestamp, storedText);
                        } else if (groupMode == 1) {
                            const std::int32_t member = business.mailGroupCC1A30.indices[row];
                            const auto networkSession = callbackWindow.ownerSession->Connection().CaptureSession();
                            const std::int32_t key = callbackWindow.ownerSession->WorldScene().guild.orderedRecords[member].value0C;
                            static_cast<void>(callbackWindow.ownerSession->Connection().SendGuildMail(key, packet, 0));
                            WindowMail_FormatTimestamp(*callbackWindow.ownerSession, timestamp);
                            static_cast<void>(world::detail::PrependGuildMailMessage_519370(*callbackWindow.ownerSession,
                                key, 1, timestamp, storedText));
                        } else {
                            const std::int32_t group = business.mailGroupCC1A30.indices[row];
                            const auto networkSession = callbackWindow.ownerSession->Connection().CaptureSession();
                            static_cast<void>(callbackWindow.ownerSession->Connection().SendGuildMail(group + 0x400, packet, 0));
                            for (std::int32_t member = 0; member < 200; ++member) {
                                const std::int32_t memberGroup = callbackWindow.ownerSession->WorldScene().guild.orderedRecords[member].value00;
                                const std::int32_t currentGroup = business.mailGroupCC1A30.indices[row];
                                if (memberGroup == currentGroup) {
                                    WindowMail_FormatTimestamp(*callbackWindow.ownerSession, timestamp);
                                    const std::int32_t key = callbackWindow.ownerSession->WorldScene().guild.orderedRecords[member].value0C;
                                    static_cast<void>(world::detail::PrependGuildMailMessage_519370(*callbackWindow.ownerSession,
                                        key, 1, timestamp, storedText));
                                }
                            }
                        }
                    }
                    static_cast<void>(cg::world::detail::SaveContactMailFile_519300(*callbackWindow.ownerSession));
                    static_cast<void>(cg::world::detail::SaveGuildMailFile_519550(*callbackWindow.ownerSession));
                    static_cast<void>(audio::PlaySe_52C450(78, 320));
                    closeAfterSend = true;
                }
            }
            consumed = 1;
        }
    } else {
        const std::int32_t category = business.prefixD2F0B8.value00;
        if ((category == 0 || category == 1 || category == 2) && (state & MouseFlag_LDownEvent) != 0) {
            if (inputs.mail.empty()) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else if (category == 2) {
                const std::int32_t contact = business.prefixD2F0B8.value04;
                const auto& record = world::detail::ReadMiniMailContact1075A90(*callbackWindow.ownerSession, contact);
                const std::int32_t key = record.key04;
                {
                    storedText = input::ContextText(inputs.mail);
                    char fileBytes[300]{};
                    cg::text::StoreGbk(fileBytes, sizeof(fileBytes), storedText);
                }
                const std::wstring escapedName = input::EscapeText(record.name10, 256);
                const std::wstring escapedText = input::EscapeText(input::ContextText(inputs.mail), 1024);
                combined = std::to_wstring(key) + L"|" + escapedName + L"|" + escapedText;
                packet = input::EscapeTextAndNotify(combined, 1024);
                static_cast<void>(callbackWindow.ownerSession->Connection().SendMiniMail(packet, 0));
                static_cast<void>(world::detail::FormatMailTimestamp_534520(*callbackWindow.ownerSession, timestamp));
                const std::int32_t currentContact = business.prefixD2F0B8.value04;
                world::detail::PrependMiniMailMessage_5343C0(*callbackWindow.ownerSession, currentContact, 1, timestamp, storedText);
                static_cast<void>(audio::PlaySe_52C450(78, 320));
                closeAfterSend = true;
            } else {
                std::int32_t target;
                if (category == 1) {
                    const std::int32_t member = business.prefixD2F0B8.value04;
                    target = callbackWindow.ownerSession->WorldScene().guild.orderedRecords[member].value0C;
                    combined = input::ContextText(inputs.mail);
                    {
                        char fileBytes[300]{};
                        cg::text::StoreGbk(fileBytes, sizeof(fileBytes), combined);
                        storedText = combined;
                    }
                } else {
                    target = business.prefixD2F0B8.value04;
                    combined = input::ContextText(inputs.mail);
                    {
                        char fileBytes[300]{};
                        cg::text::StoreGbk(fileBytes, sizeof(fileBytes), combined);
                        storedText = combined;
                    }
                }
                packet = input::EscapeTextAndNotify(combined, 1024);
                const auto networkSession = callbackWindow.ownerSession->Connection().CaptureSession();
                if (category == 1) {
                    static_cast<void>(callbackWindow.ownerSession->Connection().SendGuildMail(target, packet, 0));
                } else {
                    static_cast<void>(callbackWindow.ownerSession->Connection().SendContactMail(target, packet, 0));
                }
                WindowMail_FormatTimestamp(*callbackWindow.ownerSession, timestamp);
                if (category == 1) {
                    static_cast<void>(world::detail::PrependGuildMailMessage_519370(*callbackWindow.ownerSession,
                        target, 1, timestamp, storedText));
                } else {
                    world::detail::PrependContactMailMessage_5191C0(*callbackWindow.ownerSession, target, 1, timestamp, storedText);
                }
                static_cast<void>(cg::world::detail::SaveContactMailFile_519300(*callbackWindow.ownerSession));
                static_cast<void>(cg::world::detail::SaveGuildMailFile_519550(*callbackWindow.ownerSession));
                static_cast<void>(audio::PlaySe_52C450(78, 320));
                closeAfterSend = true;
            }
            consumed = 1;
        }
    }
    if (closeAfterSend) {
        WindowInstance* const window = currentWindow;
        inputs.mail.clearText();
        window->state |= 0x8000u;
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (auto* const handleRecipients_4Element2 = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients_4); handleRecipients_4Element2) handleRecipients_4Element2->data.graphic->imageNo = 0x3BA1C;
    }
    return consumed;
}

// 原函数：0x004A6160。
static std::uint8_t WindowMailCompose_RefreshRecipients(WindowInstance* callbackWindow)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    auto& business = callbackWindow->ownerSession->UiFrame().native.business;
    auto& inputs = currentWindow->ownerSession->UiFrame().native.inputs;
    const std::int32_t mode = business.prefixD2F0B8.value10;
    if (mode != 0) {
        const std::int32_t groupMode = business.mailGroupCC1A30.mode;
        if (groupMode == 0 || groupMode == 1 || groupMode == 2) {
            auto* const element5 = currentWindow->FindElementById(WindowMailComposeElementId::Element5);
            if (element5) {
                auto* const text = element5->data.text;
                text->text = L"群组邮件" /* 原常量参考：0x00587780。 */;
            }
            if (auto* const handleRecipientsElement = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients); handleRecipientsElement) handleRecipientsElement->visible = 0;
        }
    } else {
        const std::int32_t category = business.prefixD2F0B8.value00;
        std::wstring name; // 下方只在三个已赋值的category分支读取。
        switch (category) {
        case 0:
            name = callbackWindow->ownerSession->WorldScene().mail.data->contacts[business.prefixD2F0B8.value04].name10.data();
            break;
        case 1:
            name = callbackWindow->ownerSession->WorldScene().guild.orderedRecords[business.prefixD2F0B8.value04].name18;
            break;
        case 2:
            name = world::detail::ReadMiniMailContact1075A90(*callbackWindow->ownerSession,
                business.prefixD2F0B8.value04).name10.data();
            break;
        }
        if (category == 0 || category == 1 || category == 2) {
            auto* const element5 = currentWindow->FindElementById(WindowMailComposeElementId::Element5);
            if (element5) {
                auto* const text = element5->data.text;
                text->text = name;
            }
        }
        if (business.prefixD2F0B8.value00 == 2) {
            if (auto* const handleRecipientsElement = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients); handleRecipientsElement) handleRecipientsElement->visible = 0;
        } else {
            const std::int32_t count = business.choicesCC4CA0.value00;
            if (auto* const handleRecipientsElement = currentWindow->FindElementById(WindowMailComposeElementId::HandleRecipients); handleRecipientsElement) handleRecipientsElement->visible = count != 0 ? 1 : 0;
        }
    }
    const WindowInstance* const window = currentWindow;
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    return WindowInput_ActivateElement(*currentWindow->ownerSession, &inputs.mailPrototype, x, y, 2);
}

} // namespace detail

} // namespace cg::ui
