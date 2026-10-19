#include "cg/core/LegacyNoOp.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/MailState.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004A4620。
static std::int32_t WindowMailRead_Update(std::int32_t value); // 读信初始化：选页后保存，再扫描未读。

// 原函数：0x004A4650。
static std::int32_t WindowMailRead_Draw(std::int32_t unused); // 刷新清当前未读，再公共绘制。

// 原函数：0x004A4660。
static std::int32_t WindowMailRead_ButtonPrevious(std::int32_t index, std::int32_t state); // 上一封，负页跳9。

// 原函数：0x004A4710。
static std::int32_t WindowMailRead_ButtonNext(std::int32_t index, std::int32_t state); // 下一封，页>=10跳0。

// 原函数：0x004A47C0。
static std::int32_t WindowMailRead_ButtonReply(std::int32_t index, std::int32_t state); // 回复上下文与旧物品占用释放。

// 原函数：0x004A48A0。
static void WindowMailRead_FindUnread(); // 最后未读位置；原唯一caller不消费混合地址/整数返回。

// 原函数：0x004A49B0。
static std::uint32_t WindowMailRead_Refresh(); // 三类邮件十行、姓名、时间和页码，返回末复制长度。

// 原函数：0x004A4FC0。
static std::int32_t WindowMailRead_HandleAvatar(std::int32_t unused, std::int32_t state); // 悬停双图，区分三类头像采样序。

// 原函数：0x004A4620。
static std::int32_t WindowMailRead_Update(std::int32_t value);

// 原函数：0x004A4650。
static std::int32_t WindowMailRead_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic56FCA8 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FCA8。

static WindowGraphicData gWindowGraphic56FCC0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FCC0。

static WindowGraphicData gWindowGraphic56FCD8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FCD8。

static WindowGraphicData gWindowGraphic56FCF0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FCF0。

static WindowGraphicData gWindowGraphic56FD08 {0x3BA0C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FD08。

static WindowGraphicData gWindowGraphic56FD20 {0x3BA06, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FD20。

static WindowGraphicData gWindowGraphic56FD38 {0x3BA07, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056FD38。

static WindowTextPrototype gWindowText56FD50 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"内容"}; // 原全局：0x0056FD50。 // 原常量参考：0x00585EC4。

static WindowTextPrototype gWindowText56FD58 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"页数"}; // 原全局：0x0056FD58。 // 原常量参考：0x00585EBC。

static WindowTextPrototype gWindowText56FD60 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名称"}; // 原全局：0x0056FD60。 // 原常量参考：0x00586230。

static WindowTextPrototype gWindowText56FD68 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"00/00 00:00"}; // 原全局：0x0056FD68。 // 原常量参考：0x00585EB0。

static WindowElementPrototype gWindowElements56FD70[21] { // 原全局：0x0056FD70。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic56FCC0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 154, 27, 49, 17, 1, {.graphic = &gWindowGraphic56FCA8}, &WindowMailRead_ButtonReply},
    {WindowElementType_Graphic, 15, 180, 18, 18, 1, {.graphic = &gWindowGraphic56FCD8}, &WindowMailRead_ButtonPrevious},
    {WindowElementType_Graphic, 77, 180, 18, 18, 1, {.graphic = &gWindowGraphic56FCF0}, &WindowMailRead_ButtonNext},
    {WindowElementType_Text, 25, 52, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 64, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 76, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 88, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 100, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 112, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 124, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 136, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 148, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 160, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 35, 183, 0, 0, 1, {.text = &gWindowText56FD58}, &WindowElement_NoOp},
    {WindowElementType_Text, 18, 30, 0, 0, 1, {.text = &gWindowText56FD60}, &WindowElement_NoOp},
    {WindowElementType_Text, 115, 180, 0, 0, 1, {.text = &gWindowText56FD68}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 45, 0, 0, 1, {.graphic = &gWindowGraphic56FD08}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 230, 210, 1, {.graphic = &gWindowGraphic56FD20}, &WindowMailRead_HandleAvatar},
    {WindowElementType_Graphic, 13, 25, 0, 0, 1, {.graphic = &gWindowGraphic56FD38}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 207, 0, 25, 100, 1, {}, &WindowElement_HandleSideArea},
};

// 原型：窗口40；原全局：0x0054E9F0。
WindowPrototype gWindowPrototypeMailRead {0, 4, 383, 71, 211, 206, 0x80010101u, 21, gWindowElements56FD70, &WindowMailRead_Update, &WindowMailRead_Draw, &WindowRuntime_ReleaseDefault}; // 40: 54E9F0

// 窗口函数实现。

// 原函数：0x004A4620。
static std::int32_t WindowMailRead_Update(const std::int32_t value)
{
    if (value == -1) {
        WindowMailRead_FindUnread();
        static_cast<void>(cg::world::detail::SaveContactMailFile_519300());
        static_cast<void>(cg::world::detail::SaveGuildMailFile_519550());
        static_cast<void>(world::detail::UpdateUnreadMailPageHint_518DA0());
        cg::core::NullSub_509080();
    }
    return 1;
}

// 原函数：0x004A4650。
static std::int32_t WindowMailRead_Draw(const std::int32_t)
{
    static_cast<void>(WindowMailRead_Refresh());
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004A4660。
static std::int32_t WindowMailRead_ButtonPrevious(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    graphic->imageNo = 0x3B556;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"上一封邮件" /* 原常量参考：0x00587718。 */;
        graphic->imageNo = 0x3B558;
    }
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B557;
        const std::int32_t page = world::detail::gViewedMailMessageIndex_D046D4 - 1;
        world::detail::gViewedMailMessageIndex_D046D4 = page;
        if (page < 0) {
            world::detail::gViewedMailMessageIndex_D046D4 = 9;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        cg::core::NullSub_509080();
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B557;
    }
    return consumed;
}

// 原函数：0x004A4710。
static std::int32_t WindowMailRead_ButtonNext(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    graphic->imageNo = 0x3B559;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"下一封邮件" /* 原常量参考：0x00587724。 */;
        graphic->imageNo = 0x3B55B;
    }
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B55A;
        const std::int32_t page = world::detail::gViewedMailMessageIndex_D046D4 + 1;
        world::detail::gViewedMailMessageIndex_D046D4 = page;
        if (page >= 10) {
            world::detail::gViewedMailMessageIndex_D046D4 = 0;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        cg::core::NullSub_509080();
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B55A;
    }
    return consumed;
}

// 原函数：0x004A47C0。
static std::int32_t WindowMailRead_ButtonReply(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kReplyMailHint587730;
        graphic->imageNo = 0x3BA1D;
    } else {
        graphic->imageNo = 0x3BA1B;
    }
    if ((state & 2) != 0) {
        const std::int32_t contact = world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t oldSlot = gLoginWindowRuntime.prefixD2F0B8.value0C;
        const std::int32_t category = gLoginWindowRuntime.mailCategoryD67B64;
        gLoginWindowRuntime.prefixD2F0B8.value04 = contact;
        gLoginWindowRuntime.prefixD2F0B8.value10 = 0;
        gLoginWindowRuntime.prefixD2F0B8.value00 = category;
        if (oldSlot != -1) {
            auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
            auto value = occupancy[oldSlot];
            gLoginWindowRuntime.prefixD2F0B8.value0C = -1;
            --value;
            gLoginWindowRuntime.prefixD2F0B8.value08 = -1;
            occupancy[oldSlot] = value;
        }
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_MailCompose, 1, 0, nullptr));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3BA1C;
    }
    return consumed;
}

// 原函数：0x004A48A0。
static void WindowMailRead_FindUnread()
{
    const std::int32_t category = gLoginWindowRuntime.mailCategoryD67B64;
    cg::world::detail::gViewedMailMessageIndex_D046D4 = 0;
    switch (category) {
    case 0: {
        const std::int16_t selected = login::gSelectedCharacterIndex;
        const std::int32_t contact = cg::world::detail::gViewedMailContactId_CF4FC0;
        for (std::int32_t page = 9; page >= 0; --page) {
            if (*cg::world::GetMailMessageF7CA08(selected, contact, page).unread00 != std::byte{0}) {
                cg::world::detail::gViewedMailMessageIndex_D046D4 = page;
                break;
            }
        }
        break;
    }
    case 1: {
        const std::int32_t member = cg::world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t memberKey = cg::world::gOrderedRecords1051B14[member].value0C;
        static_cast<void>(cg::world::FindActiveOrderedRecordIndex_5195C0(memberKey));
        const std::int32_t mailMember = cg::world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t mailKey = cg::world::gOrderedRecords1051B14[mailMember].value0C;
        const std::int32_t slot = cg::world::detail::FindLastActiveGuildMailEntry_5195F0(mailKey);
        if (slot == -1) {
            cg::world::detail::gViewedMailMessageIndex_D046D4 = 0;
            return;
        }
        const std::int16_t selected = login::gSelectedCharacterIndex;
        for (std::int32_t page = 9; page >= 0; --page) {
            if (*cg::world::GetGuildMailMessageDE0B30(selected, slot, page).unread00 != std::byte{0}) {
                cg::world::detail::gViewedMailMessageIndex_D046D4 = page;
                break;
            }
        }
        break;
    }
    case 2: {
        const std::int32_t contact = cg::world::detail::gViewedMailContactId_CF4FC0;
        for (std::int32_t page = 9; page >= 0; --page) {
            if (*cg::world::GetMiniMailMessage10763A0(contact, page).unread00 != std::byte{0}) {
                cg::world::detail::gViewedMailMessageIndex_D046D4 = page;
                break;
            }
        }
        break;
    }
    }
}

// 原函数：0x004A49B0。
static std::uint32_t WindowMailRead_Refresh()
{
    const std::int32_t category = gLoginWindowRuntime.mailCategoryCC4D50;
    if (category != 0 && category != 1 && category != 2) {
        return static_cast<std::uint32_t>(category) - 2u;
    }
    std::wstring buffer;
    std::int32_t memberIndex = 0;
    std::int32_t mailSlot = 0;
    if (category == 1) {
        const std::int32_t member = cg::world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t memberKey = cg::world::gOrderedRecords1051B14[member].value0C;
        memberIndex = cg::world::FindActiveOrderedRecordIndex_5195C0(memberKey);
        const std::int32_t mailMember = cg::world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t mailKey = cg::world::gOrderedRecords1051B14[mailMember].value0C;
        mailSlot = cg::world::detail::FindLastActiveGuildMailEntry_5195F0(mailKey);
    }
    // 每次调用都重新采样；颜色与正文不能共用一次消息快照。
    const auto readCurrentMessage = [category, mailSlot]() -> cg::world::MailMessageView {
        if (category == 0) {
            const std::int16_t selected = login::gSelectedCharacterIndex;
            const std::int32_t contact = cg::world::detail::gViewedMailContactId_CF4FC0;
            const std::int32_t page = cg::world::detail::gViewedMailMessageIndex_D046D4;
            return cg::world::GetMailMessageF7CA08(selected, contact, page);
        }
        if (category == 1) {
            const std::int16_t selected = login::gSelectedCharacterIndex;
            const std::int32_t page = cg::world::detail::gViewedMailMessageIndex_D046D4;
            return cg::world::GetGuildMailMessageDE0B30(selected, mailSlot, page);
        }
        const std::int32_t contact = cg::world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t page = cg::world::detail::gViewedMailMessageIndex_D046D4;
        return cg::world::GetMiniMailMessage10763A0(contact, page);
    };
    if (category == 1 && mailSlot == -1) {
        auto* const time = static_cast<WindowTextData*>(gCurrentWindow->elements[16].data);
        time->text = L"";
        for (std::int32_t row = 0; row < 10; ++row) {
            auto* const line = static_cast<WindowTextData*>(gCurrentWindow->elements[row + 4].data);
            line->text = L"";
        }
    } else {
        const cg::world::MailMessageView message = readCurrentMessage();
        *message.unread00 = std::byte{0};
        cg::text::FormatTo(buffer, L"{}", cg::text::FromGbkWide(message.text02).c_str());
        const std::size_t slash = buffer.find(L'/');
        auto* const time = static_cast<WindowTextData*>(gCurrentWindow->elements[16].data);
        time->text = slash != std::wstring::npos ? buffer.substr(slash + 1) : L"";
        for (std::int32_t row = 0; row < 10; ++row) {
            const std::byte kind = *readCurrentMessage().kind01;
            auto* const color = static_cast<WindowTextData*>(gCurrentWindow->elements[row + 4].data);
            color->style.color = kind == std::byte{0} ? std::byte{4} : std::byte{0};
            const char* const text = readCurrentMessage().text14;
            static_cast<void>(input::ExtractGbkTextLine(buffer, text, row, 28));
            auto* const line = static_cast<WindowTextData*>(gCurrentWindow->elements[row + 4].data);
            line->text = buffer;
        }
    }
    const char* name;
    if (category == 1) {
        name = cg::world::gOrderedRecords1051B14[memberIndex].text18;
    } else {
        const std::int32_t contact = cg::world::detail::gViewedMailContactId_CF4FC0;
        name = category == 0 ? cg::world::detail::gContactsF19FB0[contact].name10.data()
            : cg::world::detail::ReadMiniMailContact1075A90(contact).name10.data();
    }
    cg::text::FormatTo(buffer, L"{}", cg::text::FromGbkWide(name).c_str());
    auto* const nameData = static_cast<WindowTextData*>(gCurrentWindow->elements[15].data);
    nameData->text = buffer;
    const std::int32_t page = cg::world::detail::gViewedMailMessageIndex_D046D4 + 1;
    cg::text::FormatTo(buffer, L"{:2}/{:2}", page, 10);
    auto* const pageData = static_cast<WindowTextData*>(gCurrentWindow->elements[14].data);
    const std::uint32_t length = static_cast<std::uint32_t>(buffer.size() + 1);
    pageData->text = buffer;
    return length;
}

// 原函数：0x004A4FC0。
static std::int32_t WindowMailRead_HandleAvatar(const std::int32_t, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    if ((state & 1) == 0) {
        return 0;
    }
    const std::int32_t category = gLoginWindowRuntime.mailCategoryCC4D50;
    cg::world::MailMessageView message;
    std::int32_t identity;
    switch (category) {
    case 0: {
        const std::int16_t selected = login::gSelectedCharacterIndex;
        identity = cg::world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t page = cg::world::detail::gViewedMailMessageIndex_D046D4;
        message = cg::world::GetMailMessageF7CA08(selected, identity, page);
        break;
    }
    case 1: {
        const std::int32_t member = cg::world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t memberKey = cg::world::gOrderedRecords1051B14[member].value0C;
        identity = cg::world::FindActiveOrderedRecordIndex_5195C0(memberKey);
        const std::int32_t mailMember = cg::world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t mailKey = cg::world::gOrderedRecords1051B14[mailMember].value0C;
        const std::int32_t slot = cg::world::detail::FindLastActiveGuildMailEntry_5195F0(mailKey);
        const std::int16_t selected = login::gSelectedCharacterIndex;
        const std::int32_t page = cg::world::detail::gViewedMailMessageIndex_D046D4;
        message = cg::world::GetGuildMailMessageDE0B30(selected, slot, page);
        break;
    }
    case 2: {
        identity = cg::world::detail::gViewedMailContactId_CF4FC0;
        const std::int32_t page = cg::world::detail::gViewedMailMessageIndex_D046D4;
        message = cg::world::GetMiniMailMessage10763A0(identity, page);
        break;
    }
    default:
        return 1;
    }
    if (message.text14[0] == '\0' && message.text02[0] == '\0') {
        return 0;
    }
    const std::byte kind = *message.kind01;
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const WindowInstance* const firstWindow = gCurrentWindow;
    const std::int32_t firstY = firstWindow->y;
    const std::int32_t firstX = firstWindow->x;
    static_cast<void>(cg::render::DrawGraphic(firstX + 202, firstY + 216, 223, 0x3BD32, 0, &descriptor));
    std::int32_t image;
    const WindowInstance* window;
    if (kind == std::byte{0}) {
        std::int32_t source;
        if (category == 0) {
            source = cg::world::detail::gContactsF19FB0[identity].value0C;
        } else if (category == 1) {
            source = cg::world::gOrderedRecords1051B14[identity].value14;
        } else {
            source = cg::world::detail::ReadMiniMailContact1075A90(identity).value0C;
        }
        image = character_select::detail::ResolveCompatibleCharacterGraphic_51BC50(source);
        window = gCurrentWindow;
    } else if (category == 2) {
        window = gCurrentWindow;
        image = cg::world::gPlayerRuntimeSlots.value17C;
    } else {
        image = cg::world::gPlayerRuntimeSlots.value17C;
        window = gCurrentWindow;
    }
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    static_cast<void>(cg::render::DrawGraphic(x + 202, y + 216, 223, image, 0, &descriptor));
    return 1;
}

} // namespace detail

} // namespace cg::ui
