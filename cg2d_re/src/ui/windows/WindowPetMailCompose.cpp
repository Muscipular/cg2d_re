#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004A56F0。
static std::int32_t WindowPetMailCompose_Update(std::int32_t value); // 宠物邮件两遍候选与刷新，标关闭后仍继续。

// 原函数：0x004A6300。
static std::int32_t WindowPetMailCompose_HandleGiftItem(std::int32_t index, std::int32_t state); // 物品占用迁移、取出拖动及悬停框。

// 原函数：0x004A6450。
static std::int32_t WindowPetMailCompose_HandleSendButtons(std::int32_t index, std::int32_t state); // 四按钮，两类发送保留不同失败声音。

// 原函数：0x004A6AC0。
static std::uint8_t WindowPetMailCompose_RefreshPet(); // 宠物名称/颜色和携带物品图、数量刷新。

// 原函数：0x004A6D30。
static std::int32_t WindowPetMailCompose_HasGiftPet(std::int32_t index); // 同源flags6AA完整W16恰3。

// 原函数：0x004A6D50。
static std::int32_t WindowPetMailCompose_HasGiftItem(std::int32_t index); // 先47D4A0，非零才读取flags6AA恰2。

// 原函数：0x004A56F0。
static std::int32_t WindowPetMailCompose_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic570118 {0x3BA0D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570118。

static WindowGraphicData gWindowGraphic570130 {0x3BA0E, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00570130。

static WindowGraphicData gWindowGraphic570178 {0x3BE54, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570178。

static WindowGraphicData gWindowGraphic5701A8 {0x3B5C3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005701A8。

static WindowGraphicData gWindowGraphic5701C0 {0x3BE5D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005701C0。

static WindowGraphicData gWindowGraphic5701D8 {0x3BE5A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005701D8。

static WindowElementPrototype gWindowElements570318[13] { // 原全局：0x00570318。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic570190}, &WindowRuntime_ButtonClose},
    {WindowElementType_Input, 33, 84, 170, 120, 1, {}, &WindowMailCompose_HandleInput},
    {WindowElementType_Graphic, 15, 29, 56, 16, 1, {.graphic = &gWindowGraphic570178}, &WindowPetMailCompose_HandleSendButtons},
    {WindowElementType_Graphic, 154, 29, 50, 18, 1, {.graphic = &gWindowGraphic570160}, &WindowPetMailCompose_HandleSendButtons},
    {WindowElementType_Graphic, 151, 199, 0, 0, 1, {.graphic = &gWindowGraphic5701A8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 151, 199, 50, 50, 1, {.graphic = &gWindowGraphic5701A8}, &WindowPetMailCompose_HandleGiftItem},
    {WindowElementType_Text, 21, 49, 0, 0, 1, {.text = &gWindowText5701F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 16, 199, 0, 0, 1, {.text = &gWindowText5701F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 224, 13, 11, 1, {.graphic = &gWindowGraphic5701C0}, &WindowPetMailCompose_HandleSendButtons},
    {WindowElementType_Graphic, 66, 224, 13, 11, 1, {.graphic = &gWindowGraphic5701D8}, &WindowPetMailCompose_HandleSendButtons},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic570118}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 13, 26, 0, 0, 1, {.graphic = &gWindowGraphic570130}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 207, 0, 25, 100, 1, {}, &WindowElement_HandleSideArea},
};

// 原型：窗口23；原全局：0x0054EA50。
WindowPrototype gWindowPrototypePetMailCompose {0, 4, 383, 81, 211, 259, 0x80010101u, 13, gWindowElements570318, &WindowPetMailCompose_Update, &WindowRuntime_DrawDefault, &WindowMailCompose_Release}; // 23: 54EA50

// 窗口函数实现。

// 原函数：0x004A56F0。
static std::int32_t WindowPetMailCompose_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowInstance* const normalWindow = gWindowStates[WindowId_MailCompose].window;
        if (normalWindow != nullptr) {
            normalWindow->state |= 0x8000u;
        }
        gLoginWindowRuntime.choicesCC4CA0.value00 = 0;
        for (std::int32_t index = 0; index < 5; ++index) {
            const auto pet = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[index].index);
            if (world::gPetRuntimeRecords[pet].used != 0
                && world::HasPetMailTechnique_52E1D0(pet) == 1) {
                ++gLoginWindowRuntime.choicesCC4CA0.value00;
            }
        }
        if (gLoginWindowRuntime.choicesCC4CA0.value00 > 0) {
            gLoginWindowRuntime.prefixD2F0B8.value08 = 0;
        }
        const WindowRuntimePrefixD2F0B8 previous = gLoginWindowRuntime.mailDraftCF4BC8;
        const WindowRuntimePrefixD2F0B8 current = gLoginWindowRuntime.prefixD2F0B8;
        if (WindowRuntime_MatchesRuntimePrefix(current, previous) != 0) {
            gMailInputPrototype57E740.text = cg::text::FromGbkWide(reinterpret_cast<const char*>(gLoginWindowRuntime.mailDraftCF4BC8.bytes14.data()));
        }
        const WindowInstance* const window = gCurrentWindow;
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gMailInputPrototype57E740, x, y, 0));
        input::SetContext_44B330(&cg::ui::detail::gInputContextD66CA8);
        WindowInstance* const focused = gCurrentWindow;
        gMailInputPrototype57E740.text.clear();
        gFocusedElementDataD45F94 = focused->elements[1].data;
        static_cast<WindowInputData*>(focused->elements[1].data)->context = &cg::ui::detail::gInputContextD66CA8;
    }
    gLoginWindowRuntime.choicesCC4CA0.value00 = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        const auto pet = std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[index].index);
        if (world::gPetRuntimeRecords[pet].used == 0) {
            continue;
        }
        const std::int32_t available = world::HasPetMailTechnique_52E1D0(pet);
        if (available == 1) {
            gLoginWindowRuntime.choicesCC4CA0.pairs18[index][1] = available;
            gLoginWindowRuntime.choicesCC4CA0.pairs18[index][0] = pet;
            const std::int32_t output = gLoginWindowRuntime.choicesCC4CA0.value00;
            gLoginWindowRuntime.choicesCC4CA0.values04[output] = pet;
            const std::int32_t count = gLoginWindowRuntime.choicesCC4CA0.value00;
            gLoginWindowRuntime.choicesCC4CA0.value00 = count + 1;
        } else {
            gLoginWindowRuntime.choicesCC4CA0.pairs18[index][1] = 0;
            gLoginWindowRuntime.choicesCC4CA0.pairs18[index][0] = pet;
        }
    }
    if (gLoginWindowRuntime.choicesCC4CA0.value00 == 0) {
        gCurrentWindow->state |= 0x8000u;
    }
    const std::int32_t position = gLoginWindowRuntime.prefixD2F0B8.value08;
    const std::int32_t pet = gLoginWindowRuntime.choicesCC4CA0.values04[position];
    if (world::gPetRuntimeRecords[pet].used == 0) {
        gCurrentWindow->state |= 0x8000u;
    }
    static_cast<void>(WindowPetMailCompose_RefreshPet());
    WindowInstance* const groupWindow = gWindowStates[WindowId_GroupSelect].window;
    if (groupWindow != nullptr && gLoginWindowRuntime.prefixD2F0B8.value10 != 1) {
        groupWindow->state |= 0x8000u;
    }
    return 1;
}

// 原函数：0x004A6300。
static std::int32_t WindowPetMailCompose_HandleGiftItem(const std::int32_t index, const std::int32_t state)
{
    std::uint32_t events = static_cast<std::uint32_t>(state);
    std::int32_t consumed = 0;
    if ((events & 0x60u) != 0 && WindowDrag_ReadKind() != 2) {
        events &= ~0x60u;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t source = WindowDrag_ReadSourceIndex();
        if (source >= 8 && (static_cast<std::uint8_t>(world::gPlayerRuntimeSlots.items[source].flags) & 1u) != 0) {
            auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
            const std::int32_t previous = gLoginWindowRuntime.prefixD2F0B8.value0C;
            if (previous != -1) {
                --occupancy[previous];
            }
            auto count = occupancy[source];
            gLoginWindowRuntime.prefixD2F0B8.value0C = source;
            ++count;
            events &= ~0x40u;
            occupancy[source] = count;
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        WindowDrag_Reset();
    }
    if ((events & 2u) != 0) {
        const std::int32_t selected = gLoginWindowRuntime.prefixD2F0B8.value0C;
        if (selected >= 0) {
            static_cast<void>(WindowItemDrag_Start(selected, 1));
            const std::int32_t current = gLoginWindowRuntime.prefixD2F0B8.value0C;
            auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
            auto count = occupancy[current];
            gLoginWindowRuntime.prefixD2F0B8.value0C = -1;
            --count;
            occupancy[current] = count;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (WindowDrag_ReadKind() == 2) {
        static_cast<void>(WindowDrag_ReadSourceIndex());
    }
    if ((events & 0x21u) != 0) {
        const WindowInstance* const window = gCurrentWindow;
        const std::uint32_t image = cg::render::gValueD69768;
        const std::int32_t windowY = window->y;
        const WindowElement* const elements = window->elements;
        const std::int32_t windowX = window->x;
        const std::int32_t elementX = elements[index].x;
        const std::int32_t elementY = elements[index].y;
        const std::int32_t height = elements[index].height;
        const std::int32_t width = elements[index].width;
        cg::render::DrawGraphicHover(
            static_cast<std::uint16_t>(windowX + elementX),
            static_cast<std::uint16_t>(windowY + elementY),
            static_cast<std::uint16_t>(width + windowX + elementX),
            static_cast<std::uint16_t>(height + windowY + elementY), 223, image, 0);
    }
    return consumed;
}

// 原函数：0x004A6D30。
static std::int32_t WindowPetMailCompose_HasGiftPet(const std::int32_t index)
{
    return world::gPetRuntimeRecords[index].flags6AA == 3 ? 1 : 0;
}

// 原函数：0x004A6D50。
static std::int32_t WindowPetMailCompose_HasGiftItem(const std::int32_t index)
{
    if (WindowCraft_ReadItemSelection() == 0) {
        return 0;
    }
    return world::gPetRuntimeRecords[index].flags6AA == 2 ? 1 : 0;
}

// 原函数：0x004A6450。
static std::int32_t WindowPetMailCompose_HandleSendButtons(const std::int32_t index, const std::int32_t state)
{
    std::int32_t consumed = 0;
    switch (index) {
    case 2:
    case 8:
    case 9: {
        const std::int32_t idle = index == 2 ? 0x3BE54 : index == 8 ? 0x3BE5D : 0x3BE5A;
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        if ((state & 1) != 0) {
            graphic->imageNo = idle + 2;
            gHoverTextD68AE8 = index == 2 ? L"寄送一般邮件。" /* 原常量参考：0x005877A4。 */ : L"换另一只宠物寄送邮件。" /* 原常量参考：0x0058778C。 */;
            consumed = 1;
        } else {
            graphic->imageNo = idle;
        }
        if ((state & 2) != 0) {
            if (index == 2) {
                gCurrentWindow->state |= 0x8000u;
                static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailCompose, 1, 0, nullptr));
            } else if (index == 8) {
                const std::int32_t position = gLoginWindowRuntime.prefixD2F0B8.value08 - 1;
                gLoginWindowRuntime.prefixD2F0B8.value08 = position;
                if (position < 0) {
                    gLoginWindowRuntime.prefixD2F0B8.value08 = gLoginWindowRuntime.choicesCC4CA0.value00 - 1;
                }
            } else {
                const std::int32_t previous = gLoginWindowRuntime.prefixD2F0B8.value08;
                const std::int32_t count = gLoginWindowRuntime.choicesCC4CA0.value00;
                const std::int32_t position = previous + 1;
                gLoginWindowRuntime.prefixD2F0B8.value08 = position;
                if (position > count - 1) {
                    gLoginWindowRuntime.prefixD2F0B8.value08 = 0;
                }
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        if ((state & 8) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = idle + 1;
        }
        return consumed;
    }
    case 3:
        break;
    default:
        return 0;
    }
    const std::int32_t category = gLoginWindowRuntime.prefixD2F0B8.value00;
    if (category == 0 || category == 1) {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3BA1D;
            gHoverTextD68AE8 = kReplyMailHint587730;
            consumed = 1;
        } else {
            graphic->imageNo = 0x3BA1B;
        }
        if ((state & 2) != 0) {
            bool allowed = !cg::ui::detail::gInputContextD66CA8.empty();
            if (allowed) {
                const std::int32_t position = gLoginWindowRuntime.prefixD2F0B8.value08;
                const std::int32_t pet = gLoginWindowRuntime.choicesCC4CA0.values04[position];
                allowed = WindowPetMailCompose_HasGiftPet(pet) == 0;
            }
            if (allowed) {
                const std::int32_t position = gLoginWindowRuntime.prefixD2F0B8.value08;
                const std::int32_t pet = gLoginWindowRuntime.choicesCC4CA0.values04[position];
                allowed = WindowPetMailCompose_HasGiftItem(pet) == 0;
            }
            if (allowed) {
                std::wstring text;
                std::array<char, 300> storedText; // 原邮件0x140字节记录的+0x14正文，含NUL。
                char timestamp[64];
                std::int32_t target;
                if (category == 1) {
                    const std::int32_t member = gLoginWindowRuntime.prefixD2F0B8.value04;
                    target = cg::world::gOrderedRecords1051B14[member].value0C;
                    text = input::ContextText(cg::ui::detail::gInputContextD66CA8);
                } else {
                    target = gLoginWindowRuntime.prefixD2F0B8.value04;
                    text = input::ContextText(cg::ui::detail::gInputContextD66CA8);
                }
                cg::text::StoreGbk(storedText.data(), storedText.size(), text);
                const std::wstring packet = input::EscapeTextAndNotify(text, 600);
                const std::int32_t position = gLoginWindowRuntime.prefixD2F0B8.value08;
                const std::int32_t item = gLoginWindowRuntime.prefixD2F0B8.value0C;
                const std::int32_t pet = gLoginWindowRuntime.choicesCC4CA0.values04[position];
                const std::uintptr_t socket = platform::gConnectionSocket;
                if (category == 1) {
                    static_cast<void>(network::SendGuildPetMail_511510(socket, target, pet, item, packet, 0));
                } else {
                    static_cast<void>(network::SendContactPetMail_5110B0(socket, target, pet, item, packet, 0));
                }
                WindowMail_FormatTimestamp(timestamp);
                if (category == 1) {
                    static_cast<void>(world::detail::PrependGuildMailMessage_519370(
                        target, 1, timestamp, storedText.data()));
                } else {
                    world::detail::PrependContactMailMessage_5191C0(target, 1, timestamp, storedText.data());
                }
                static_cast<void>(cg::world::detail::SaveContactMailFile_519300());
                static_cast<void>(cg::world::detail::SaveGuildMailFile_519550());
                static_cast<void>(audio::PlaySe_52C450(78, 320));
                WindowInstance* const window = gCurrentWindow;
                cg::ui::detail::gInputContextD66CA8.clearText();
                window->state |= 0x8000u;
            } else if (category == 0) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            consumed = 1;
        }
    }
    if ((state & 8) != 0) {
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 0x3BA1C;
    }
    return consumed;
}

// 原函数：0x004A6AC0。
static std::uint8_t WindowPetMailCompose_RefreshPet()
{
    const std::int32_t category = gLoginWindowRuntime.prefixD2F0B8.value00;
    if (category == 0 || category == 1) {
        const std::int32_t target = gLoginWindowRuntime.prefixD2F0B8.value04;
        auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[6].data);
        const char* const name = category == 0 ? world::detail::gContactsF19FB0[target].name10.data()
            : cg::world::gOrderedRecords1051B14[target].text18;
        text->text = cg::text::FromGbkWide(name);
    }
    const std::int32_t namePosition = gLoginWindowRuntime.prefixD2F0B8.value08;
    const std::int32_t namePet = gLoginWindowRuntime.choicesCC4CA0.values04[namePosition];
    const auto& pet = world::gPetRuntimeRecords[namePet];
    const char* const name = pet.name6BD[0] != '\0' ? pet.name6BD.data() : pet.name6AC.data();
    auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[7].data);
    nameData->text = cg::text::FromGbkWide(name);
    const std::int32_t firstPosition = gLoginWindowRuntime.prefixD2F0B8.value08;
    const std::int32_t firstPet = gLoginWindowRuntime.choicesCC4CA0.values04[firstPosition];
    bool unavailable = WindowPetMailCompose_HasGiftPet(firstPet) != 0;
    if (!unavailable) {
        const std::int32_t secondPosition = gLoginWindowRuntime.prefixD2F0B8.value08;
        const std::int32_t secondPet = gLoginWindowRuntime.choicesCC4CA0.values04[secondPosition];
        unavailable = WindowPetMailCompose_HasGiftItem(secondPet) != 0;
    }
    auto* const colorData = static_cast<WindowTextData*>(gCurrentWindow->elements[7].data);
    colorData->style.color = unavailable ? std::byte{7} : std::byte{0};
    const bool hasItem = gLoginWindowRuntime.prefixD2F0B8.value0C != -1;
    gCurrentWindow->elements[4].visible = 0;
    if (hasItem) {
        const std::int32_t imageSlot = gLoginWindowRuntime.prefixD2F0B8.value0C;
        const std::int32_t image = world::gPlayerRuntimeSlots.items[imageSlot].imageNo;
        const WindowInstance* const graphicWindow = gCurrentWindow;
        const std::int32_t y = graphicWindow->y;
        const std::int32_t x = graphicWindow->x;
        static_cast<void>(cg::render::DrawGraphic(x + 176, y + 224, 223, image, 0, nullptr));
        const std::int32_t countSlot = gLoginWindowRuntime.prefixD2F0B8.value0C;
        const std::int32_t count = world::gPlayerRuntimeSlots.items[countSlot].count;
        if (count > 0) {
            std::wstring buffer;
            cg::text::FormatTo(buffer, L"{:3}", count);
            const WindowInstance* const textWindow = gCurrentWindow;
            const auto textY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(textWindow->y + 232));
            const auto textX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(textWindow->x + 174));
            static_cast<void>(render::DrawText_456780(textX, textY, 3, 1, 1, buffer, 0, 0));
        }
    }
    const WindowInstance* const window = gCurrentWindow;
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    return WindowInput_ActivateElement(&gMailInputPrototype57E740, x, y, 2);
}

} // namespace detail

} // namespace cg::ui
