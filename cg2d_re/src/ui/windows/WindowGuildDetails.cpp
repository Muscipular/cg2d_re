#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

InputContext gInputContextD2E130 {}; // 原全局：0x00D2E130。
} // namespace cg::input

namespace cg::ui {
namespace detail {
namespace {


struct GuildRankDraft {
    std::int32_t flags;
    std::array<char, 41> text; // 原成员偏移：+0x04。
    std::array<std::byte, 3> bytes2D; // 原成员偏移：+0x2D。
};

// CF401C+48*33与CF464C是同一草稿文本；仅保留已证字段及间隔，不解释未消费字节。
struct GuildWindowDraft {
    std::array<GuildRankDraft, 32> ranks;
    std::array<std::byte, 4> bytes600; // 原成员偏移：+0x600。
    std::array<char, 41> name604; // 原成员偏移：+0x604。
    std::array<std::byte, 7> bytes62D; // 原成员偏移：+0x62D。
    std::array<char, 41> secondText634; // 原成员偏移：+0x634。
};

static_assert(offsetof(GuildRankDraft, text) == 4);

static_assert(offsetof(GuildRankDraft, bytes2D) == 0x2D);

static_assert(sizeof(GuildRankDraft) == 0x30);

static_assert(offsetof(GuildWindowDraft, name604) == 0x604);

static_assert(offsetof(GuildWindowDraft, secondText634) == 0x634);

GuildWindowDraft gGuildDraftCF4018 {}; // 原全局：0x00CF4018。

std::int32_t gGuildPageD67B78 = 0; // 原全局：0x00D67B78。

std::int32_t gGuildEditing57F03C = -1; // 原全局：0x0057F03C。

std::array<WindowInputPrototype, 9> gGuildInputs57F040 {{ // 原全局：0x0057F040。
    {{{&input::gInputContextD2E130}, WindowInputReferenceKind::Context},
        116, 31, 3, 12, 6, {}, 1, 24, 0, 0},
    {{{&input::gInputContextD2E130}, WindowInputReferenceKind::Context},
        116, 49, 3, 12, 6, {}, 1, 24, 0, 0},
    {{{&input::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 165, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&input::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 184, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&input::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 203, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&input::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 222, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&input::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 241, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&input::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 260, 3, 12, 6, {}, 1, 16, 0, 0},
    {{{&input::gInputContextD2E130}, WindowInputReferenceKind::Context},
        38, 279, 3, 12, 6, {}, 1, 16, 0, 0},
}};

constexpr wchar_t kGuildPermissionHint587AA8[] = // 原常量参考：0x00587AA8。
    L"變更此家族稱號所屬的加入權限。";

constexpr wchar_t kGuildPermissionHint587A88[] = // 原常量参考：0x00587A88。
    L"變更此家族稱號所屬的除名權限。";

constexpr wchar_t kGuildPermissionHint587A60[] = // 原常量参考：0x00587A60。
    L"變更此家族稱號所屬的家族獸餵養權限。";

constexpr wchar_t kGuildPermissionHint587A38[] = // 原常量参考：0x00587A38。
    L"變更此家族稱號所屬的家族倉庫使用權限。";

constexpr wchar_t kGuildPermissionHint587A10[] = // 原常量参考：0x00587A10。
    L"變更此家族稱號所屬的留言板刪除權限。";

const wchar_t* const kGuildPermissionHints[] {
    kGuildPermissionHint587AA8, kGuildPermissionHint587A88,
    kGuildPermissionHint587A60, kGuildPermissionHint587A38, kGuildPermissionHint587A10,
};

constexpr wchar_t kGuildNameSubmitHint587AC8[] = // 原常量参考：0x00587AC8。
    L"變更家族名稱。";

constexpr wchar_t kGuildSecondSubmitHint587AD8[] = // 原常量参考：0x00587AD8。
    L"變更家族會館名稱。";

constexpr wchar_t kGuildRankSubmitHint587AEC[] = // 原常量参考：0x00587AEC。
    L"變更已設定的家族稱號。";

constexpr wchar_t kGuildNameEditMessage587B04[] = // 原常量参考：0x00587B04。
    L"家族名稱不得更改喔!!";

constexpr wchar_t kGuildCloseHint5868EC[] = L"關閉此視窗"; // 原常量参考：0x005868EC。
} // namespace


// 原函数：0x004AEAE0。
std::int32_t WindowGuildDetails_Update(const std::int32_t value)
{
    if (value == -1) {
        gGuildPageD67B78 = 0;
        gGuildEditing57F03C = -1;
        static_cast<void>(WindowGuildDetails_RefreshDraft());
        WindowGuildDetails_RefreshReadOnly();
        std::strcpy(gGuildDraftCF4018.name604.data(), login::detail::BorrowGuildName10514BC());
        std::strcpy(gGuildDraftCF4018.secondText634.data(),
            login::detail::BorrowGuildSecondText10514E5());
        for (std::int32_t index = 0; index < 32; ++index) {
            const char* const source = login::detail::BorrowGuildRankText1051518(index);
            char* const destination = gGuildDraftCF4018.ranks[index].text.data();
            std::strcpy(destination, source);
        }
        auto* const name = static_cast<WindowTextData*>(gCurrentWindow->elements[9].data);
        name->text = cg::text::FromGbkWide(login::detail::BorrowGuildName10514BC());
        auto* const second = static_cast<WindowTextData*>(gCurrentWindow->elements[10].data);
        second->text = cg::text::FromGbkWide(login::detail::BorrowGuildSecondText10514E5());
    }
    if (WindowGuildDetails_CommitFocusedInput() == 0) {
        gGuildEditing57F03C = -1;
    }
    return 1;
}

// 原函数：0x004AEC10。
std::int32_t WindowGuildDetails_Draw(const std::int32_t value)
{
    WindowGuildDetails_DragScroll(value);
    WindowGuildDetails_RefreshReadOnly();
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x004AF180。
void WindowGuildDetails_RefreshReadOnly()
{
    if (WindowGuildDetails_ReadRankPermissions() != 0) {
        WindowGuildDetails_RefreshEditor();
        return;
    }
    gCurrentWindow->elements[64].visible = 0;
    gCurrentWindow->elements[65].visible = 0;
    gCurrentWindow->elements[72].visible = 0;
    for (std::int32_t index = 9; index < 18; ++index) {
        gCurrentWindow->elements[index].visible = 1;
        gCurrentWindow->elements[index].mouseCallback = &WindowElement_NoOp;
    }
    for (std::int32_t index = 18; index < 53; ++index) {
        gCurrentWindow->elements[index].height = 0;
        gCurrentWindow->elements[index].width = 0;
        gCurrentWindow->elements[index].mouseCallback = &WindowElement_NoOp;
    }
    gCurrentWindow->elements[72].visible = 0;
    static_cast<void>(WindowGuildDetails_RefreshRanks());
}

// 原函数：0x004AF250。
void WindowGuildDetails_RefreshEditor()
{
    gCurrentWindow->elements[64].visible = 1;
    gCurrentWindow->elements[65].visible = 1;
    gCurrentWindow->elements[72].visible = 1;
    for (std::int32_t index = 9; index < 18; ++index) {
        gCurrentWindow->elements[index].visible = 1;
    }
    for (std::int32_t index = 18; index < 53; ++index) {
        gCurrentWindow->elements[index].height = 15;
        gCurrentWindow->elements[index].width = 15;
        gCurrentWindow->elements[index].mouseCallback = &WindowGuildDetails_ToggleRankPermission;
    }
    static_cast<void>(WindowGuildDetails_RefreshRanks());
    WindowInstance* window = gCurrentWindow;
    WindowElement* elements = window->elements;
    if (gFocusedElementDataD45F94 == elements[0].data) {
        cg::text::StoreGbk(gGuildDraftCF4018.name604.data(), gGuildDraftCF4018.name604.size(), input::ContextText(*gGuildInputs57F040[0].input.context));
        window->elements[9].visible = 0;
        WindowInstance* const current = gCurrentWindow;
        const std::int32_t y = current->y;
        const std::int32_t x = current->x;
        static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[0], x, y, 1));
    } else {
        elements[9].visible = 1;
        auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[9].data);
        text->text = cg::text::FromGbkWide(gGuildDraftCF4018.name604.data());
        const bool changed = std::strcmp(gGuildDraftCF4018.name604.data(),
            login::detail::BorrowGuildName10514BC()) != 0;
        static_cast<WindowTextData*>(gCurrentWindow->elements[9].data)->bytes00[0] =
            changed ? std::byte{6} : std::byte{0};
    }
    const bool nameChanged = std::strcmp(gGuildDraftCF4018.name604.data(),
        login::detail::BorrowGuildName10514BC()) != 0;
    gCurrentWindow->elements[64].visible = nameChanged ? 1 : 0;

    window = gCurrentWindow;
    elements = window->elements;
    if (gFocusedElementDataD45F94 == elements[1].data) {
        elements[10].visible = 0;
        cg::text::StoreGbk(gGuildDraftCF4018.secondText634.data(), gGuildDraftCF4018.secondText634.size(),
            input::ContextText(*gGuildInputs57F040[1].input.context));
        WindowInstance* const current = gCurrentWindow;
        const std::int32_t y = current->y;
        const std::int32_t x = current->x;
        static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[1], x, y, 1));
    } else {
        elements[10].visible = 1;
        auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[10].data);
        text->text = cg::text::FromGbkWide(gGuildDraftCF4018.secondText634.data());
        const bool changed = std::strcmp(gGuildDraftCF4018.secondText634.data(),
            login::detail::BorrowGuildSecondText10514E5()) != 0;
        static_cast<WindowTextData*>(gCurrentWindow->elements[10].data)->bytes00[0] =
            changed ? std::byte{6} : std::byte{0};
    }
    const bool secondChanged = std::strcmp(gGuildDraftCF4018.secondText634.data(),
        login::detail::BorrowGuildSecondText10514E5()) != 0;
    gCurrentWindow->elements[65].visible = secondChanged ? 1 : 0;

    for (std::int32_t row = 0; row < 7; ++row) {
        window = gCurrentWindow;
        elements = window->elements;
        if (gFocusedElementDataD45F94 == elements[row + 2].data) {
            elements[row + 11].visible = 0;
            WindowGuildDetails_SaveInputDraft(row + 2);
            WindowInstance* const current = gCurrentWindow;
            // 原显示源为row，而草稿写回使用row+2，不合并这两个索引。
            static_cast<WindowTextData*>(
                current->elements[row + 11].data)->text =
                input::ContextText(*gGuildInputs57F040[row].input.context);
        }
        window = gCurrentWindow;
        const std::int32_t rank = gGuildPageD67B78 + row;
        auto* const text = static_cast<WindowTextData*>(window->elements[row + 11].data);
        const std::int32_t changed = WindowGuildDetails_HasChangedRank(rank);
        text->bytes00[0] = changed != 0 ? std::byte{6} : std::byte{0};
    }
    const std::int32_t changed = WindowGuildDetails_HasChangedRanks();
    gCurrentWindow->elements[72].visible = changed;
}

// 原函数：0x004AF650。
std::int32_t WindowGuildDetails_RefreshRanks()
{
    const std::int32_t editable = WindowGuildDetails_ReadRankPermissions();
    gCurrentWindow->elements[53].visible = 0;
    auto* const name = static_cast<WindowTextData*>(gCurrentWindow->elements[9].data);
    name->text = cg::text::FromGbkWide(editable != 0 ? gGuildDraftCF4018.name604.data()
        : login::detail::BorrowGuildName10514BC());
    auto* const second = static_cast<WindowTextData*>(gCurrentWindow->elements[10].data);
    second->text = cg::text::FromGbkWide(editable != 0 ? gGuildDraftCF4018.secondText634.data()
        : login::detail::BorrowGuildSecondText10514E5());
    for (std::int32_t index = 0; index < 3; ++index) {
        WindowInstance* const window = gCurrentWindow;
        const char* const source = login::detail::BorrowGuildAuxText1057BF4(index);
        static_cast<WindowTextData*>(window->elements[index + 54].data)->text = cg::text::FromGbkWide(source);
    }
    std::int32_t y = 164;
    for (std::int32_t row = 0; row < 7; ++row) {
        const std::int32_t rank = gGuildPageD67B78 + row;
        const char* const source = editable != 0 ? gGuildDraftCF4018.ranks[rank].text.data()
            : login::detail::BorrowGuildRankText1051518(rank);
        auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[row + 11].data);
        text->text = cg::text::FromGbkWide(source);
        for (std::int32_t column = 0; column < 5; ++column) {
            const std::int32_t flags = editable != 0 ? gGuildDraftCF4018.ranks[rank].flags
                : login::detail::ReadGuildRankFlags1051514(rank);
            auto* const graphic = static_cast<WindowGraphicData*>(
                gCurrentWindow->elements[18 + row * 5 + column].data);
            graphic->imageNo = (flags & (1 << column)) != 0 ? 0x3B9F6 : 0x3B9FD;
        }
        const std::uint8_t flags = editable != 0
            ? *reinterpret_cast<const unsigned char*>(&gGuildDraftCF4018.ranks[rank].flags)
            : login::detail::ReadGuildRankFlagsByte1051514(rank);
        if ((flags & 0x80) != 0) {
            gCurrentWindow->elements[53].visible = 1;
            gCurrentWindow->elements[53].x = 23;
            gCurrentWindow->elements[53].y = y;
        }
        y += 20;
    }
    return y;
}

// 原函数：0x004AFA00。
void WindowGuildDetails_SaveInputDraft(const std::int32_t index)
{
    const std::int32_t editing = gGuildEditing57F03C;
    if (editing == -1) {
        return;
    }
    char* const destination = reinterpret_cast<char*>(&gGuildDraftCF4018)
        + 4 + static_cast<std::ptrdiff_t>(editing) * sizeof(GuildRankDraft);
    cg::text::StoreGbk(destination, gGuildDraftCF4018.name604.size(), input::ContextText(*gGuildInputs57F040[index].input.context));
}

// 原函数：0x004AFED0。
std::int32_t WindowGuildDetails_CommitFocusedInput()
{
    WindowInstance* const window = gCurrentWindow;
    void* const focused = gFocusedElementDataD45F94;
    WindowElement* const elements = window->elements;
    for (std::int32_t index = 0; index < 9; ++index) {
        if (elements[index].data == focused) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x004AFAE0。
void WindowGuildDetails_DragScroll(const std::int32_t)
{
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const mouse = static_cast<WindowMouseData*>(elements[69].data);
    if ((mouse->flags & 1) != 0) {
        gGuildPageD67B78 = WindowScroll_ReadPosition(&elements[69], 25);
    }
}

// 原函数：0x004AFA60。
void WindowGuildDetails_ScrollRow()
{
    const std::int32_t page = gGuildPageD67B78;
    std::int32_t sound = 51;
    if (page < 0) {
        gGuildPageD67B78 = 0;
        sound = 55;
    } else if (page > 25) {
        gGuildPageD67B78 = 25;
        sound = 55;
    }
    static_cast<void>(audio::PlaySe_52C450(sound, 320));
    WindowInstance* const window = gCurrentWindow;
    const std::int32_t position = gGuildPageD67B78;
    static_cast<void>(WindowScroll_SetPosition(&window->elements[69], 25, position));
}

// 原函数：0x004AEC30。
std::int32_t WindowGuildDetails_ToggleRankPermission(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t row = (index - 18) / 5;
    const std::int32_t column = (index - 18) % 5;
    if (row == 0 && gGuildPageD67B78 == 0) {
        return 0;
    }
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kGuildPermissionHints[column];
    }
    if ((state & 2) != 0) {
        const std::int32_t rank = gGuildPageD67B78 + row;
        if (gGuildDraftCF4018.ranks[rank].text[0] == '\0') {
            return 0;
        }
        gGuildDraftCF4018.ranks[rank].flags ^= 1 << column;
        WindowGuildDetails_RefreshEditor();
        return 1;
    }
    return 0;
}

// 原函数：0x004AED40。
std::int32_t WindowGuildDetails_HandleActionButton(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B56E;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B570;
        switch (index) {
        case 64: gHoverTextD68AE8 = kGuildNameSubmitHint587AC8; break;
        case 65: gHoverTextD68AE8 = kGuildSecondSubmitHint587AD8; break;
        case 72: gHoverTextD68AE8 = kGuildRankSubmitHint587AEC; break;
        }
    }
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B56F;
        switch (index) {
        case 64: static_cast<void>(WindowGuildDetails_SubmitName()); break;
        case 65: static_cast<void>(WindowGuildDetails_SubmitDescription()); break;
        case 72: static_cast<void>(WindowGuildDetails_SubmitRanks()); break;
        }
        graphic->imageNo = 0x3B56F;
        return 1;
    }
    return 0;
}

// 原函数：0x004AEE00。
std::int32_t WindowGuildDetails_HandleInput(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t editable = WindowGuildDetails_ReadRankPermissions();
    if (editable == 0) {
        return editable;
    }
    WindowInstance* const window = gCurrentWindow;
    WindowElement* const elements = window->elements;
    if (gFocusedElementDataD45F94 == elements[index].data) {
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[index], x, y, 2));
        return 0;
    }
    elements[index + 9].visible = 1;
    if ((state & 2) == 0) {
        return 0;
    }
    switch (index) {
    case 0:
        world::detail::Sub_44B050(kGuildNameEditMessage587B04, 4, 3);
        break;
    case 1: {
        gGuildInputs57F040[1].text = cg::text::FromGbkWide(gGuildDraftCF4018.secondText634.data());
        WindowInstance* const current = gCurrentWindow;
        const std::int32_t y = current->y;
        const std::int32_t x = current->x;
        static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[1], x, y, 0));
        input::SetContext_44B330(&input::gInputContextD2E130);
        WindowInstance* const focused = gCurrentWindow;
        gFocusedElementDataD45F94 = focused->elements[index].data;
        static_cast<WindowInputData*>(focused->elements[index].data)->context =
            &input::gInputContextD2E130;
        gGuildEditing57F03C = 33;
        break;
    }
    case 2: case 3: case 4: case 5: case 6: case 7: case 8: {
        const std::wstring& source = static_cast<WindowTextData*>(gCurrentWindow->elements[index + 9].data)->text;
        gGuildInputs57F040[index].text = source;
        WindowInstance* const current = gCurrentWindow;
        const std::int32_t y = current->y;
        const std::int32_t x = current->x;
        static_cast<void>(WindowInput_ActivateElement(&gGuildInputs57F040[index], x, y, 0));
        input::SetContext_44B330(&input::gInputContextD2E130);
        WindowInstance* const focused = gCurrentWindow;
        gFocusedElementDataD45F94 = focused->elements[index].data;
        static_cast<WindowInputData*>(focused->elements[index].data)->context =
            &input::gInputContextD2E130;
        gGuildEditing57F03C = gGuildPageD67B78 + index - 2;
        break;
    }
    }
    return 1;
}

// 原函数：0x004AF000。
std::int32_t WindowGuildDetails_HandleScrollButtons(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const std::int32_t offset = index == 66 ? 0 : 3;
    graphic->imageNo = 0x3B592 + offset;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B594 + offset;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B593 + offset;
    }
    if ((state & 0x400) != 0) {
        gGuildPageD67B78 += index == 66 ? -1 : 1;
        WindowGuildDetails_ScrollRow();
        static_cast<void>(WindowChat_RestoreInput());
        return 1;
    }
    return 0;
}

// WSP01：滚动结果恰1才调用既有焦点页依赖，保留原S32结果。
// 原函数：0x004AF090。
std::int32_t WindowGuildDetails_HandleScrollFocus(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t result = WindowElement_HandleVerticalScroll(index, state);
    if (result == 1) {
        static_cast<void>(WindowChat_RestoreInput());
    }
    return result;
}

// 原函数：0x004AF0C0。
std::int32_t WindowGuildDetails_Scroll(const std::int32_t, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::BorrowLegacyMouseStateStorage()->tailReset);
        if (wheel != 0) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t page = gGuildPageD67B78;
            gGuildPageD67B78 = WindowScroll_ApplyWheel(&window->elements[69], page, 25, wheel);
            static_cast<void>(WindowChat_RestoreInput());
        }
    }
    return 0;
}

// 原函数：0x004AF100。
std::int32_t WindowGuildDetails_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 0x3B538;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B53A;
        gHoverTextD68AE8 = kGuildCloseHint5868EC;
    }
    if ((state & 2) != 0) {
        graphic->imageNo = 0x3B539;
        gWindowStates[WindowId_GuildDetails].window->state = 0x8000;
        return 1;
    }
    return 0;
}

// 原函数：0x004AF960。
std::uint32_t WindowGuildDetails_RefreshDraft()
{
    if (WindowGuildDetails_ReadRankPermissions() == 0) {
        return 0;
    }
    for (std::int32_t index = 0; index < 32; ++index) {
        const std::int32_t flags = login::detail::ReadGuildRankFlags1051514(index);
        const char* const text = login::detail::BorrowGuildRankText1051518(index);
        gGuildDraftCF4018.ranks[index].flags = flags;
        std::strcpy(gGuildDraftCF4018.ranks[index].text.data(), text);
    }
    std::strcpy(gGuildDraftCF4018.name604.data(), login::detail::BorrowGuildName10514BC());
    const char* const text = login::detail::BorrowGuildSecondText10514E5();
    const auto length = static_cast<std::uint32_t>(std::strlen(text) + 1);
    std::memcpy(gGuildDraftCF4018.secondText634.data(), text, length);
    return length;
}

// 原函数：0x004AFE20。
std::int32_t WindowGuildDetails_HasChangedRank(const std::int32_t index)
{
    std::int32_t changes = std::strcmp(
        gGuildDraftCF4018.ranks[index].text.data(),
        login::detail::BorrowGuildRankText1051518(index)) != 0 ? 1 : 0;
    const std::int32_t flags = login::detail::ReadGuildRankFlags1051514(index);
    if ((flags & 0x80) == 0 && gGuildDraftCF4018.ranks[index].flags != flags) {
        ++changes;
    }
    return changes > 0 ? 1 : 0;
}

// 原函数：0x004AFEA0。
std::int32_t WindowGuildDetails_HasChangedRanks()
{
    std::int32_t changes = 0;
    for (std::int32_t index = 0; index < 32; ++index) {
        if (WindowGuildDetails_HasChangedRank(index) != 0) {
            ++changes;
        }
    }
    return changes > 0 ? 1 : 0;
}

// 原函数：0x004AFB10。
std::int32_t WindowGuildDetails_SubmitName()
{
    std::int32_t result = std::strcmp(
        gGuildDraftCF4018.name604.data(), login::detail::BorrowGuildName10514BC());
    if (result == 0) {
        return result;
    }
    result = input::Sub_476740(cg::text::FromGbkWide(gGuildDraftCF4018.name604.data()));
    if (result == 0) {
        return result;
    }
    result = input::Sub_45E770(cg::text::FromGbkWide(gGuildDraftCF4018.name604.data()));
    if (result == 1) {
        return result;
    }
    const std::wstring escapedText = input::EscapeTextAndNotify(cg::text::FromGbkWide(gGuildDraftCF4018.name604.data()), 256);
    static_cast<void>(network::Sub_511250(
        platform::gConnectionSocket, 2, escapedText));
    static_cast<void>(audio::PlaySe_52C450(79, 320));
    return WindowChat_RestoreInput();
}

// 原函数：0x004AFC00。
std::int32_t WindowGuildDetails_SubmitDescription()
{
    std::int32_t result = std::strcmp(
        gGuildDraftCF4018.secondText634.data(), login::detail::BorrowGuildSecondText10514E5());
    if (result == 0) {
        return result;
    }
    result = input::Sub_476740(cg::text::FromGbkWide(gGuildDraftCF4018.secondText634.data()));
    if (result == 0) {
        return result;
    }
    result = input::Sub_45E770(cg::text::FromGbkWide(gGuildDraftCF4018.secondText634.data()));
    if (result == 1) {
        return result;
    }
    const std::wstring escapedText = input::EscapeTextAndNotify(cg::text::FromGbkWide(gGuildDraftCF4018.secondText634.data()), 256);
    static_cast<void>(network::Sub_511250(
        platform::gConnectionSocket, 3, escapedText));
    static_cast<void>(audio::PlaySe_52C450(79, 320));
    return WindowChat_RestoreInput();
}

// 原函数：0x004AFCF0。
std::int32_t WindowGuildDetails_SubmitRanks()
{
    for (std::int32_t index = 0; index < 32; ++index) {
        char* const text = gGuildDraftCF4018.ranks[index].text.data();
        if (WindowGuildDetails_HasChangedRank(index) != 0) {
            const std::int32_t valid = input::Sub_476740(cg::text::FromGbkWide(text));
            if (valid == 0) {
                return valid;
            }
            const std::int32_t rejected = input::Sub_45E770(cg::text::FromGbkWide(text));
            if (rejected == 1) {
                return rejected;
            }
        }
    }
    for (std::int32_t index = 0; index < 32; ++index) {
        const char* const draft = gGuildDraftCF4018.ranks[index].text.data();
        if (std::strcmp(login::detail::BorrowGuildRankText1051518(index), draft) != 0
            || login::detail::ReadGuildRankFlags1051514(index) != gGuildDraftCF4018.ranks[index].flags) {
            const std::wstring escapedText = input::EscapeTextAndNotify(cg::text::FromGbkWide(draft), 256);
            const std::int32_t flags = gGuildDraftCF4018.ranks[index].flags;
            const std::uintptr_t socket = platform::gConnectionSocket;
            static_cast<void>(network::Sub_5112C0(socket, index, flags, escapedText));
            static_cast<void>(audio::PlaySe_52C450(79, 320));
        }
    }
    return WindowChat_RestoreInput();
}

// 原函数：0x004A3950。
std::int32_t WindowGuildDetails_ReadRankPermissions()
{
    return WindowGuildDetails_CanEdit(0x80);
}

// 原函数：0x004A3960。
std::int32_t WindowGuildDetails_CanEdit(const std::int32_t mask)
{
    const std::int32_t index = login::detail::ReadGuildTextIndex10514B0();
    if (index == -1) {
        return 0;
    }
    const std::int32_t flags = login::detail::ReadGuildRankFlags1051514(index);
    return (flags & mask) != 0 ? 1 : 0;
}

// 原函数：0x004A3990。
std::int32_t WindowGuildDetails_CanEditRank(const std::int32_t index, const std::int32_t mask)
{
    if (login::detail::ReadGuildTextIndex10514B0() == -1) {
        return 0;
    }
    const std::int32_t flags = login::detail::ReadGuildRankFlags1051514(index);
    return (flags & mask) != 0 ? 1 : 0;
}
} // namespace detail


// 原函数：0x004AFF00。
std::int32_t WindowGuildDetails_RequestClose()
{
    WindowInstance* const window = detail::gWindowStates[WindowId_GuildDetails].window;
    if (window == nullptr) {
        return 0;
    }
    window->state |= 0x8000u;
    return audio::PlaySe_52C450(57, 320);
}

} // namespace cg::ui

namespace cg::ui::detail {

namespace {

WindowGraphicData gWindowGraphic571DF0 {0x3B9FD, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571DF0。

WindowGraphicData gWindowGraphic571E08 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E08。

WindowGraphicData gWindowGraphic571E20 {0x3B56E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E20。

WindowGraphicData gWindowGraphic571E38 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E38。

WindowGraphicData gWindowGraphic571E50 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E50。

WindowGraphicData gWindowGraphic571E68 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E68。

WindowGraphicData gWindowGraphic571EB0 {0x3B9F4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571EB0。

WindowGraphicData gWindowGraphic571EC8 {0x3B9F5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571EC8。

WindowGraphicData gWindowGraphic571EE0 {0x3B9F2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571EE0。

WindowGraphicData gWindowGraphic571EF8 {0x3B9F3, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00571EF8。

WindowTextPrototype gWindowText571F10 { // 原全局：0x00571F10。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"guild", // 原常量参考：0x00585F04。
};

std::uint32_t gWindowMouseFlagsD678A8 = 0; // 原全局：0x00D678A8。

} // namespace

// 原型：窗口48；原全局：0x0054EBA0。
WindowPrototype gWindowPrototypeGuildDetails {0, 4, 100, 60, 317, 325, 0x80010101u, 76, gWindowElements571F18, &WindowGuildDetails_Update, &WindowGuildDetails_Draw, &WindowRuntime_ReleaseDefault}; // 48: 54EBA0

WindowElementPrototype gWindowElements571F18[76] { // 原全局：0x00571F18。
    {WindowElementType_Input, 116, 31, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 116, 49, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 163, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 182, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 201, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 220, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 239, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 258, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 277, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Text, 116, 31, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 116, 49, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 165, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 184, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 203, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 222, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 241, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 260, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 279, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 18, 161, 0, 0, 1, {.graphic = &gWindowGraphic571EC8}, &WindowElement_NoOp},
    {WindowElementType_Text, 116, 70, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 116, 90, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 116, 110, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 161, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 180, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 199, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 218, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 237, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 256, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 275, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 273, 28, 32, 17, 0, {.graphic = &gWindowGraphic571E20}, &WindowGuildDetails_HandleActionButton},
    {WindowElementType_Graphic, 273, 46, 32, 17, 0, {.graphic = &gWindowGraphic571E20}, &WindowGuildDetails_HandleActionButton},
    {WindowElementType_Graphic, 299, 163, 11, 11, 1, {.graphic = &gWindowGraphic571E38}, &WindowGuildDetails_HandleScrollButtons},
    {WindowElementType_Graphic, 299, 280, 11, 11, 1, {.graphic = &gWindowGraphic571E68}, &WindowGuildDetails_HandleScrollButtons},
    {WindowElementType_Graphic, 299, 173, 0, 0, 1, {.graphic = &gWindowGraphic571E50}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 298, 173, 10, 94, 1, {.mouseFlags = &gWindowMouseFlagsD678A8}, &WindowGuildDetails_HandleScrollFocus},
    {WindowElementType_EventArea, 0, 0, 317, 325, 1, {.unusedValue = 0}, &WindowGuildDetails_Scroll},
    {WindowElementType_Graphic, 299, 9, 11, 11, 1, {.graphic = &gWindowGraphic571E08}, &WindowGuildDetails_ButtonClose},
    {WindowElementType_Graphic, 265, 296, 57, 17, 0, {.graphic = &gWindowGraphic571E20}, &WindowGuildDetails_HandleActionButton},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic571EE0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic571EF8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 317, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

} // namespace cg::ui::detail
