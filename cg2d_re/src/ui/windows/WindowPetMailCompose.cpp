#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowPetMailComposeElementId : ElementId {
    ButtonClose = 1,
    HandleInput = 2,
    HandleSendButtons = 3,
    HandleSendButtons_4 = 4,
    Element5 = 5,
    HandleGiftItem = 6,
    Element7 = 7,
    Element8 = 8,
    HandleSendButtons_9 = 9,
    HandleSendButtons_10 = 10,
    Element11 = 11,
    Element12 = 12,
    HandleSideArea = 13,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004A56F0。
static std::int32_t WindowPetMailCompose_Update(WindowInstance& callbackWindow, std::int32_t value); // 宠物邮件两遍候选与刷新，标关闭后仍继续。

// 原函数：0x004A6300。
static std::int32_t WindowPetMailCompose_HandleGiftItem(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 物品占用迁移、取出拖动及悬停框。

// 原函数：0x004A6450。
static std::int32_t WindowPetMailCompose_HandleSendButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 四按钮，两类发送保留不同失败声音。

// 原函数：0x004A6AC0。
static std::uint8_t WindowPetMailCompose_RefreshPet(WindowInstance& window); // 宠物名称/颜色和携带物品图、数量刷新。

// 原函数：0x004A6D30。
static std::int32_t WindowPetMailCompose_HasGiftPet(session::ClientSession& session, std::int32_t index); // 同源flags6AA完整W16恰3。

// 原函数：0x004A6D50。
static std::int32_t WindowPetMailCompose_HasGiftItem(session::ClientSession& session, std::int32_t index); // 先47D4A0，非零才读取flags6AA恰2。

// 原函数：0x004A56F0。
static std::int32_t WindowPetMailCompose_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic570118 {0x3BA0D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570118。

static WindowGraphicData gWindowGraphic570130 {0x3BA0E, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00570130。

static WindowGraphicData gWindowGraphic570178 {0x3BE54, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570178。

static WindowGraphicData gWindowGraphic5701A8 {0x3B5C3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005701A8。

static WindowGraphicData gWindowGraphic5701C0 {0x3BE5D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005701C0。

static WindowGraphicData gWindowGraphic5701D8 {0x3BE5A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005701D8。

static WindowElementPrototype gWindowElements570318[13] { // 原全局：0x00570318。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic570190}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowPetMailComposeElementId::ButtonClose)},
    {WindowElementType_Input, 33, 84, 170, 120, 1, {}, &WindowMailCompose_HandleInput, -1, static_cast<ElementId>(WindowPetMailComposeElementId::HandleInput)},
    {WindowElementType_Graphic, 15, 29, 56, 16, 1, {.graphic = &gWindowGraphic570178}, &WindowPetMailCompose_HandleSendButtons, -1, static_cast<ElementId>(WindowPetMailComposeElementId::HandleSendButtons)},
    {WindowElementType_Graphic, 154, 29, 50, 18, 1, {.graphic = &gWindowGraphic570160}, &WindowPetMailCompose_HandleSendButtons, -1, static_cast<ElementId>(WindowPetMailComposeElementId::HandleSendButtons_4)},
    {WindowElementType_Graphic, 151, 199, 0, 0, 1, {.graphic = &gWindowGraphic5701A8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetMailComposeElementId::Element5)},
    {WindowElementType_Graphic, 151, 199, 50, 50, 1, {.graphic = &gWindowGraphic5701A8}, &WindowPetMailCompose_HandleGiftItem, -1, static_cast<ElementId>(WindowPetMailComposeElementId::HandleGiftItem)},
    {WindowElementType_Text, 21, 49, 0, 0, 1, {.text = &gWindowText5701F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetMailComposeElementId::Element7)},
    {WindowElementType_Text, 16, 199, 0, 0, 1, {.text = &gWindowText5701F0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetMailComposeElementId::Element8)},
    {WindowElementType_Graphic, 16, 224, 13, 11, 1, {.graphic = &gWindowGraphic5701C0}, &WindowPetMailCompose_HandleSendButtons, -1, static_cast<ElementId>(WindowPetMailComposeElementId::HandleSendButtons_9)},
    {WindowElementType_Graphic, 66, 224, 13, 11, 1, {.graphic = &gWindowGraphic5701D8}, &WindowPetMailCompose_HandleSendButtons, -1, static_cast<ElementId>(WindowPetMailComposeElementId::HandleSendButtons_10)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic570118}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetMailComposeElementId::Element11)},
    {WindowElementType_Graphic, 13, 26, 0, 0, 1, {.graphic = &gWindowGraphic570130}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowPetMailComposeElementId::Element12)},
    {WindowElementType_EventArea, 207, 0, 25, 100, 1, {}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowPetMailComposeElementId::HandleSideArea)},
};

// 原型：窗口23；原全局：0x0054EA50。
WindowPrototype gWindowPrototypePetMailCompose {0, 4, 383, 81, 211, 259, 0x80010101u, 13, gWindowElements570318, &WindowPetMailCompose_Update, &WindowRuntime_DrawDefault, &WindowMailCompose_Release}; // 23: 54EA50

// 窗口函数实现。

// 原函数：0x004A56F0。
static std::int32_t WindowPetMailCompose_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        WindowInstance* const normalWindow = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_MailCompose].window;
        if (normalWindow != nullptr) {
            normalWindow->state |= 0x8000u;
        }
        business.choicesCC4CA0.value00 = 0;
        for (std::int32_t index = 0; index < 5; ++index) {
            const auto pet = std::bit_cast<std::int16_t>(callbackWindow.ownerSession->WorldScene().petPairs[index].index);
            if ((*callbackWindow.ownerSession->WorldScene().petRecords)[pet].used != 0
                && world::HasPetMailTechnique_52E1D0(*callbackWindow.ownerSession, pet) == 1) {
                ++business.choicesCC4CA0.value00;
            }
        }
        if (business.choicesCC4CA0.value00 > 0) {
            business.prefixD2F0B8.value08 = 0;
        }
        const WindowRuntimePrefixD2F0B8 previous = business.mailDraftCF4BC8;
        const WindowRuntimePrefixD2F0B8 current = business.prefixD2F0B8;
        if (WindowRuntime_MatchesRuntimePrefix(current, previous) != 0) {
            inputs.mailPrototype.text = cg::text::FromGbkWide(reinterpret_cast<const char*>(business.mailDraftCF4BC8.bytes14.data()));
        }
        const WindowInstance* const window = currentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.mailPrototype, x, y, 0));
        input::SetContext_44B330(*callbackWindow.ownerSession, &inputs.mail);
        WindowInstance* const focused = currentWindow;
        inputs.mailPrototype.text.clear();
        auto* const handleInputElement = focused->FindElementById(WindowPetMailComposeElementId::HandleInput);
        if (handleInputElement) {
            callbackWindow.ownerSession->UiFrame().native.pointer.focusedElementData = handleInputElement->data.input;
        }
        if (auto* const handleInputElement2 = focused->FindElementById(WindowPetMailComposeElementId::HandleInput); handleInputElement2) handleInputElement2->data.input->context = &inputs.mail;
    }
    business.choicesCC4CA0.value00 = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        const auto pet = std::bit_cast<std::int16_t>(callbackWindow.ownerSession->WorldScene().petPairs[index].index);
        if ((*callbackWindow.ownerSession->WorldScene().petRecords)[pet].used == 0) {
            continue;
        }
        const std::int32_t available = world::HasPetMailTechnique_52E1D0(*callbackWindow.ownerSession, pet);
        if (available == 1) {
            business.choicesCC4CA0.pairs18[index][1] = available;
            business.choicesCC4CA0.pairs18[index][0] = pet;
            const std::int32_t output = business.choicesCC4CA0.value00;
            business.choicesCC4CA0.values04[output] = pet;
            const std::int32_t count = business.choicesCC4CA0.value00;
            business.choicesCC4CA0.value00 = count + 1;
        } else {
            business.choicesCC4CA0.pairs18[index][1] = 0;
            business.choicesCC4CA0.pairs18[index][0] = pet;
        }
    }
    if (business.choicesCC4CA0.value00 == 0) {
        currentWindow->state |= 0x8000u;
    }
    const std::int32_t position = business.prefixD2F0B8.value08;
    const std::int32_t pet = business.choicesCC4CA0.values04[position];
    if ((*callbackWindow.ownerSession->WorldScene().petRecords)[pet].used == 0) {
        currentWindow->state |= 0x8000u;
    }
    static_cast<void>(WindowPetMailCompose_RefreshPet(callbackWindow));
    WindowInstance* const groupWindow = WindowRuntime_States(*callbackWindow.ownerSession)[WindowId_GroupSelect].window;
    if (groupWindow != nullptr && business.prefixD2F0B8.value10 != 1) {
        groupWindow->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x004A6300。
static std::int32_t WindowPetMailCompose_HandleGiftItem(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& inventory = callbackWindow.ownerSession->UiFrame().native.inventory;
    std::uint32_t events = static_cast<std::uint32_t>(state);
    std::int32_t consumed = 0;
    if ((events & (MouseFlag_Dragging | MouseFlag_DragFinish)) != 0 && WindowDrag_ReadKind(*callbackWindow.ownerSession) != 2) {
        events &= ~(MouseFlag_Dragging | MouseFlag_DragFinish);
        consumed = 1;
    }
    if ((events & MouseFlag_DragFinish) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex(*callbackWindow.ownerSession);
        if (source >= 8 && (static_cast<std::uint8_t>(callbackWindow.ownerSession->WorldScene().playerItems[source].flags) & 1u) != 0) {
            auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
            const std::int32_t previous = business.prefixD2F0B8.value0C;
            if (previous != -1) {
                --occupancy[previous];
            }
            auto count = occupancy[source];
            business.prefixD2F0B8.value0C = source;
            ++count;
            events &= ~MouseFlag_DragFinish;
            occupancy[source] = count;
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        WindowDrag_Reset(*callbackWindow.ownerSession);
    }
    if ((events & MouseFlag_LDownEvent) != 0) {
        const std::int32_t selected = business.prefixD2F0B8.value0C;
        if (selected >= 0) {
            static_cast<void>(WindowItemDrag_Start(*callbackWindow.ownerSession, selected, 1));
            const std::int32_t current = business.prefixD2F0B8.value0C;
            auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
            auto count = occupancy[current];
            business.prefixD2F0B8.value0C = -1;
            --count;
            occupancy[current] = count;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (WindowDrag_ReadKind(*callbackWindow.ownerSession) == 2) {
        static_cast<void>(WindowDrag_ReadSourceIndex(*callbackWindow.ownerSession));
    }
    if ((events & (MouseFlag_Hover | MouseFlag_Dragging)) != 0) {
        const WindowInstance* const window = currentWindow;
        const std::uint32_t image = cg::render::gHoverFramePaletteIndex_D69768;
        const std::int32_t windowY = window->y;
        auto* elementsWindow = window;
        const std::int32_t windowX = window->x;
        auto* const elementXElement = elementsWindow->FindElementById(elementId);
        if (elementXElement) {
            const std::int32_t elementX = elementXElement->x;
            auto* const elementYElement = elementsWindow->FindElementById(elementId);
            if (elementYElement) {
                const std::int32_t elementY = elementYElement->y;
                auto* const heightElement = elementsWindow->FindElementById(elementId);
                if (heightElement) {
                    const std::int32_t height = heightElement->height;
                    auto* const widthElement = elementsWindow->FindElementById(elementId);
                    if (widthElement) {
                        const std::int32_t width = widthElement->width;
                        cg::render::DrawGraphicHover(
                            static_cast<std::uint16_t>(windowX + elementX),
                            static_cast<std::uint16_t>(windowY + elementY),
                            static_cast<std::uint16_t>(width + windowX + elementX),
                            static_cast<std::uint16_t>(height + windowY + elementY), 223, image, 0);
                    }
                }
            }
        }
    }
    return consumed;
}

// 原函数：0x004A6D30。
static std::int32_t WindowPetMailCompose_HasGiftPet(session::ClientSession& session, const std::int32_t index)
{
    return (*session.WorldScene().petRecords)[index].flags6AA == 3 ? 1 : 0;
}

// 原函数：0x004A6D50。
static std::int32_t WindowPetMailCompose_HasGiftItem(session::ClientSession& session, const std::int32_t index)
{
    if (WindowCraft_ReadItemSelection(session) == 0) {
        return 0;
    }
    return (*session.WorldScene().petRecords)[index].flags6AA == 2 ? 1 : 0;
}

// 原函数：0x004A6450。
static std::int32_t WindowPetMailCompose_HandleSendButtons(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    switch (static_cast<WindowPetMailComposeElementId>(elementId)) {
    case WindowPetMailComposeElementId::HandleSendButtons:
    case WindowPetMailComposeElementId::HandleSendButtons_9:
    case WindowPetMailComposeElementId::HandleSendButtons_10: {
        const std::int32_t idle = elementId == static_cast<ElementId>(WindowPetMailComposeElementId::HandleSendButtons) ? 0x3BE54 : elementId == static_cast<ElementId>(WindowPetMailComposeElementId::HandleSendButtons_9) ? 0x3BE5D : 0x3BE5A;
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = idle + 2;
                uiFrame.hoverText = elementId == static_cast<ElementId>(WindowPetMailComposeElementId::HandleSendButtons) ? L"寄送一般邮件。" /* 原常量参考：0x005877A4。 */ : L"换另一只宠物寄送邮件。" /* 原常量参考：0x0058778C。 */;
                consumed = 1;
            } else {
                graphic->imageNo = idle;
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (elementId == static_cast<ElementId>(WindowPetMailComposeElementId::HandleSendButtons)) {
                currentWindow->state |= 0x8000u;
                static_cast<void>(WindowRuntime_CreateAnimation(*callbackWindow.ownerSession, WindowId_MailCompose, 1, 0, nullptr));
            } else if (elementId == static_cast<ElementId>(WindowPetMailComposeElementId::HandleSendButtons_9)) {
                const std::int32_t position = business.prefixD2F0B8.value08 - 1;
                business.prefixD2F0B8.value08 = position;
                if (position < 0) {
                    business.prefixD2F0B8.value08 = business.choicesCC4CA0.value00 - 1;
                }
            } else {
                const std::int32_t previous = business.prefixD2F0B8.value08;
                const std::int32_t count = business.choicesCC4CA0.value00;
                const std::int32_t position = previous + 1;
                business.prefixD2F0B8.value08 = position;
                if (position > count - 1) {
                    business.prefixD2F0B8.value08 = 0;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = idle + 1;
        }
        return consumed;
    }
    case WindowPetMailComposeElementId::HandleSendButtons_4:
        break;
    default:
        return 0;
    }
    const std::int32_t category = business.prefixD2F0B8.value00;
    if (category == 0 || category == 1) {
        auto* const graphicElement = currentWindow->FindElementById(elementId);
        if (graphicElement) {
            auto* const graphic = graphicElement->data.graphic;
            if ((state & MouseFlag_Hover) != 0) {
                graphic->imageNo = 0x3BA1D;
                uiFrame.hoverText = kReplyMailHint587730;
                consumed = 1;
            } else {
                graphic->imageNo = 0x3BA1B;
            }
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            bool allowed = !inputs.mail.empty();
            if (allowed) {
                const std::int32_t position = business.prefixD2F0B8.value08;
                const std::int32_t pet = business.choicesCC4CA0.values04[position];
                allowed = WindowPetMailCompose_HasGiftPet(*callbackWindow.ownerSession, pet) == 0;
            }
            if (allowed) {
                const std::int32_t position = business.prefixD2F0B8.value08;
                const std::int32_t pet = business.choicesCC4CA0.values04[position];
                allowed = WindowPetMailCompose_HasGiftItem(*callbackWindow.ownerSession, pet) == 0;
            }
            if (allowed) {
                std::wstring text;
                std::wstring storedText; // 内部正文；写文件前仍执行原300字节容量预检。
                std::wstring timestamp;
                std::int32_t target;
                if (category == 1) {
                    const std::int32_t member = business.prefixD2F0B8.value04;
                    target = cg::world::gOrderedRecords1051B14[member].value0C;
                    text = input::ContextText(inputs.mail);
                } else {
                    target = business.prefixD2F0B8.value04;
                    text = input::ContextText(inputs.mail);
                }
                {
                    char fileBytes[300]{};
                    cg::text::StoreGbk(fileBytes, sizeof(fileBytes), text);
                    storedText = text;
                }
                const std::wstring packet = input::EscapeTextAndNotify(text, 600);
                const std::int32_t position = business.prefixD2F0B8.value08;
                const std::int32_t item = business.prefixD2F0B8.value0C;
                const std::int32_t pet = business.choicesCC4CA0.values04[position];
                const auto networkSession = callbackWindow.ownerSession->Connection().CaptureSession();
                if (category == 1) {
                    static_cast<void>(callbackWindow.ownerSession->Connection().SendGuildPetMail(target, pet, item, packet, 0));
                } else {
                    static_cast<void>(callbackWindow.ownerSession->Connection().SendContactPetMail(target, pet, item, packet, 0));
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
                WindowInstance* const window = currentWindow;
                inputs.mail.clearText();
                window->state |= 0x8000u;
            } else if (category == 0) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            consumed = 1;
        }
    }
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = 0x3BA1C;
    }
    return consumed;
}

// 原函数：0x004A6AC0。
static std::uint8_t WindowPetMailCompose_RefreshPet(WindowInstance& callbackWindow)
{
    auto& business = callbackWindow.ownerSession->UiFrame().native.business;
    auto& inputs = callbackWindow.ownerSession->UiFrame().native.inputs;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t category = business.prefixD2F0B8.value00;
    if (category == 0 || category == 1) {
        const std::int32_t target = business.prefixD2F0B8.value04;
        auto* const element7 = currentWindow->FindElementById(WindowPetMailComposeElementId::Element7);
        if (element7) {
            auto* const text = element7->data.text;
            text->text = category == 0 ? callbackWindow.ownerSession->WorldScene().mail.data->contacts[target].name10
                : cg::world::gOrderedRecords1051B14[target].name18;
        }
    }
    const std::int32_t namePosition = business.prefixD2F0B8.value08;
    const std::int32_t namePet = business.choicesCC4CA0.values04[namePosition];
    const auto& pet = (*callbackWindow.ownerSession->WorldScene().petRecords)[namePet];
    const wchar_t* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
    auto* const element8 = currentWindow->FindElementById(WindowPetMailComposeElementId::Element8);
    if (element8) {
        auto* const nameData = element8->data.text;
        nameData->text = name;
    }
    const std::int32_t firstPosition = business.prefixD2F0B8.value08;
    const std::int32_t firstPet = business.choicesCC4CA0.values04[firstPosition];
    bool unavailable = WindowPetMailCompose_HasGiftPet(*callbackWindow.ownerSession, firstPet) != 0;
    if (!unavailable) {
        const std::int32_t secondPosition = business.prefixD2F0B8.value08;
        const std::int32_t secondPet = business.choicesCC4CA0.values04[secondPosition];
        unavailable = WindowPetMailCompose_HasGiftItem(*callbackWindow.ownerSession, secondPet) != 0;
    }
    auto* const element82 = currentWindow->FindElementById(WindowPetMailComposeElementId::Element8);
    if (element82) {
        auto* const colorData = element82->data.text;
        colorData->style.color = unavailable ? std::byte{7} : std::byte{0};
    }
    const bool hasItem = business.prefixD2F0B8.value0C != -1;
    if (auto* const element5 = currentWindow->FindElementById(WindowPetMailComposeElementId::Element5); element5) element5->visible = 0;
    if (hasItem) {
        const std::int32_t imageSlot = business.prefixD2F0B8.value0C;
        const std::int32_t image = callbackWindow.ownerSession->WorldScene().playerItems[imageSlot].imageNo;
        const WindowInstance* const graphicWindow = currentWindow;
        const std::int32_t y = graphicWindow->y;
        const std::int32_t x = graphicWindow->x;
        static_cast<void>(cg::render::DrawGraphic(x + 176, y + 224, 223, image, 0, nullptr));
        const std::int32_t countSlot = business.prefixD2F0B8.value0C;
        const std::int32_t count = callbackWindow.ownerSession->WorldScene().playerItems[countSlot].count;
        if (count > 0) {
            std::wstring buffer;
            cg::text::FormatTo(buffer, L"{:3}", count);
            const WindowInstance* const textWindow = currentWindow;
            const auto textY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(textWindow->y + 232));
            const auto textX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(textWindow->x + 174));
            static_cast<void>(render::DrawText_456780(textX, textY, 3, 1, 1, buffer, 0, 0));
        }
    }
    const WindowInstance* const window = currentWindow;
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    return WindowInput_ActivateElement(*callbackWindow.ownerSession, &inputs.mailPrototype, x, y, 2);
}

} // namespace detail

} // namespace cg::ui
