#include "../WindowRuntimeInternal.hpp"

namespace cg::input {

InputContext gInputContextD66CA8 {}; // 原全局：0x00D66CA8。

InputContext gInputContextCB03F0 {}; // 原全局：0x00CB03F0。
} // namespace cg::input

namespace cg::ui {
namespace detail {

// 原函数：0x004A5390。
std::int32_t WindowMail_ResetComposeState()
{
    WindowChoiceSlotsCC4CA0& choices = gLoginWindowRuntime.choicesCC4CA0;
    choices.value00 = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        choices.values04[index] = 0;
        choices.pairs18[index][1] = 0;
        choices.pairs18[index][0] = 0;
    }
    gLoginWindowRuntime.prefixD2F0B8.value26C = 0;
    gLoginWindowRuntime.prefixD2F0B8.value08 = -1;
    gLoginWindowRuntime.prefixD2F0B8.value0C = -1;
    return -1;
}

void WindowMail_InitializeDeleteConfirmation(const WindowElementCallback callback)
{
    gWindow47Runtime.visibleMaskCC4240 = 3;
    gWindow47Runtime.widthUnitsCC4244 = 3;
    gWindow47Runtime.valueCC424C = 0;
    gWindow47Runtime.valueCC4248 = 0;
    gWindow47Runtime.initializeCC4250 = nullptr;
    gWindow47Runtime.actionsCC4254[5] = nullptr;
    gWindow47Runtime.actionsCC4254[4] = nullptr;
    gWindow47Runtime.actionsCC4254[3] = nullptr;
    gWindow47Runtime.actionsCC4254[2] = nullptr;
    gWindow47Runtime.actionsCC4254[0] = callback;
    gWindow47Runtime.actionsCC4254[1] = &WindowMail_CancelDelete;
}
namespace {

struct MailStatusTextTable {
    const wchar_t* prefixes[25];
    const wchar_t* suffixes[20];

    const wchar_t* Suffix(const std::int32_t status) const
    {
        return suffixes[status - 1]; // 原0x638 + status*8：有效在线状态从1开始。
    }
};

constexpr MailStatusTextTable kMailContactStatusText56E9BC { // 原常量参考：0x0056E9BC。
    {
        L"卡蓮",
        L"金牛座",
        L"雙子座",
        L"雙子",
        L"獅子座",
        L"處女座",
        L"天秤座",
        L"歌姬",
        L"櫻之舞",
        L"摩羯座",
        L"水瓶座",
        L"歌姬",
        L"香港",
        L"測試",
        L"測試",
        L"卡蓮",
        L"金牛座",
        L"露比",
        L"香港",
        L"獅子座",
        L"處女座",
        L"露比",
        L"心美",
        L"香港飛馬座",
        L"香港獵戶座",
    },
    {
        L"I", L"II", L"III", L"IV", L"V", L"VI", L"VII", L"VIII", L"IX", L"X",
        L"11", L"12", L"13", L"14", L"15", L"16", L"17", L"18", L"19", L"20",
    },
};

constexpr MailStatusTextTable kMailGuildStatusText56F0AC { // 原常量参考：0x0056F0AC。
    {
        L"卡蓮",
        L"金牛座",
        L"雙子座",
        L"香港",
        L"獅子座",
        L"處女座",
        L"天秤座",
        L"歌姬",
        L"櫻之舞",
        L"摩羯座",
        L"水瓶座",
        L"雙魚座",
        L"香港",
        L"測試",
        L"測試",
        L"卡蓮",
        L"金牛座",
        L"露比",
        L"香港",
        L"獅子座",
        L"處女座",
        L"露比",
        L"心美",
        L"香港飛馬座",
        L"香港獵戶座",
    },
    {
        L"I", L"II", L"III", L"IV", L"V", L"VI", L"VII", L"VIII", L"IX", L"X",
        L"11", L"12", L"13", L"14", L"15", L"16", L"17", L"18", L"19", L"20",
    },
};

constexpr std::array<std::uint8_t, 16> kMailStatusColors56F09C { // 原常量参考：0x0056F09C。
    7, 5, 1, 3, 4, 2, 6, 5, 1, 3, 4, 2, 6, 0, 0, 0,
};

constexpr wchar_t kMailOffline58743C[] = L"離線"; // 原常量参考：0x0058743C。

constexpr wchar_t kMailStatusFormat5598F0[] = L"{}{}"; // 原常量参考：0x005598F0。

constexpr wchar_t kMailReverseFormat587434[] = L"逆-{}"; // 原常量参考：0x00587434。
} // namespace


MailContactView WindowMail_BorrowContactView(const world::detail::ContactRuntimeRecord& record)
{
    return {
        &record.used, reinterpret_cast<const std::byte*>(&record.value02),
        &record.value08, &record.value0C,
        record.name10.data(), record.text21.data(), record.text32.data(),
    };
}

MailContactView WindowMail_BorrowContactView(const login::detail::OrderedRecord1051B14& record)
{
    return {
        &record.value08, reinterpret_cast<const std::byte*>(&record.value0A),
        &record.value10, &record.value14,
        record.text18, record.text18 + 17, record.text18 + 34,
    };
}

MailContactView WindowMail_BorrowContactView(const world::detail::MiniMailContactRecord& record)
{
    return {
        &record.used, reinterpret_cast<const std::byte*>(&record.sequence02),
        &record.value08, &record.value0C,
        record.name10.data(), record.text21.data(), record.text32.data(),
    };
}

void WindowMail_WriteGuildStatus(WindowTextData* const data, const std::int32_t status)
{
    if (status == 0) {
        data->text = kMailOffline58743C;
        const std::int32_t currentStatus = login::detail::ReadGuildStatus10514AC();
        data->bytes00[0] = static_cast<std::byte>(
            gLoginWindowRuntime.mailGuildColorsD6784C[currentStatus]);
    } else {
        std::int32_t server = login::gValue55A054;
        if (server >= 25) {
            server -= 25;
        }
        cg::text::FormatTo(data->text, kMailStatusFormat5598F0, kMailGuildStatusText56F0AC.prefixes[server], kMailGuildStatusText56F0AC.Suffix(status));
        data->bytes00[0] = static_cast<std::byte>(gLoginWindowRuntime.mailGuildColorsD6784C[1]);
    }
}

void WindowMail_WriteContactStatus(
    const MailContactView& record, WindowTextData* const data, const std::int16_t status)
{
    if (status == -1) {
        return;
    }
    if (status == 0) {
        data->text = kMailOffline58743C;
        const std::int16_t currentStatus = record.ReadStatus();
        data->bytes00[0] = static_cast<std::byte>(kMailStatusColors56F09C[currentStatus]);
    } else {
        std::int32_t server = login::gValue55A054;
        if (server >= 25) {
            server -= 25;
        }
        const wchar_t* const prefix = kMailContactStatusText56E9BC.prefixes[server];
        cg::text::FormatTo(data->text, kMailStatusFormat5598F0, prefix, kMailContactStatusText56E9BC.Suffix(status));
        if (record.ReadStatus() == 10) {
            cg::text::FormatTo(data->text, kMailReverseFormat587434, prefix);
        }
        data->bytes00[0] = static_cast<std::byte>(kMailStatusColors56F09C[1]);
    }
}

void WindowMail_SetUnreadGraphic(const std::int32_t index)
{
    WindowInstance* const window = gCurrentWindow;
    WindowElement* const elements = window->elements;
    const std::int32_t blink = gLoginWindowRuntime.mailBlinkFlagD67B60;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    graphic->imageNo = blink != 0 ? 0x3B549 : 0x3B547;
}

std::int32_t& WindowMail_BorrowDeleteIndex()
{
    return gLoginWindowRuntime.mailDeleteIndexD2DA8C;
}

// 原函数：0x004A3600。
std::int32_t WindowMail_DeleteContact(const std::int32_t, const std::int32_t)
{
    const std::int32_t index = gLoginWindowRuntime.mailDeleteIndexD2DA8C;
    static_cast<void>(network::Sub_511190(platform::gConnectionSocket, index));
    const std::int32_t currentIndex = gLoginWindowRuntime.mailDeleteIndexD2DA8C;
    static_cast<void>(world::detail::Sub_5192C0(currentIndex));
    static_cast<void>(world::detail::Sub_519300());
    static_cast<void>(world::detail::Sub_519550());
    if (gWindowStates[WindowId_Contacts].window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Contacts, 1, 0, nullptr));
    }
    static_cast<void>(audio::PlaySe_52C450(78, 320));
    return 1;
}

// 原函数：0x004A3660。
std::int32_t WindowMail_DeleteGuildMember(const std::int32_t, const std::int32_t)
{
    const std::int32_t index = gLoginWindowRuntime.mailDeleteIndexD2DA8C;
    const std::uintptr_t socket = platform::gConnectionSocket;
    const std::int32_t key = login::detail::gOrderedRecords1051B14[index].value0C;
    static_cast<void>(network::Sub_511430(socket, key));
    static_cast<void>(world::detail::Sub_519300());
    static_cast<void>(world::detail::Sub_519550());
    static_cast<void>(audio::PlaySe_52C450(78, 320));
    return 1;
}

// 原函数：0x004A36B0。
std::int32_t WindowMail_DeleteMiniMailContact(const std::int32_t, const std::int32_t)
{
    const std::int32_t index = gLoginWindowRuntime.mailDeleteIndexD2DA8C;
    const auto& record = world::detail::ReadMiniMailContact1075A90(index);
    const std::int32_t key = record.key04;
    const std::wstring escapedText = input::EscapeTextAndNotify(cg::text::FromGbkWide(record.name10.data()), 255);
    const std::uintptr_t socket = platform::gConnectionSocket;
    static_cast<void>(network::Sub_5117D0(socket, key, escapedText));
    const std::int32_t currentIndex = gLoginWindowRuntime.mailDeleteIndexD2DA8C;
    world::detail::Sub_534480(currentIndex);
    static_cast<void>(audio::PlaySe_52C450(78, 320));
    return 1;
}

// 原函数：0x004A3760。
std::int32_t WindowMail_CancelDelete(const std::int32_t, const std::int32_t)
{
    if (gWindowStates[WindowId_Contacts].window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(WindowId_Contacts, 1, 0, nullptr));
    }
    return 1;
}

// MC01-C8：两邮件索引域共用std::sort；三向COMP<0表示在前，同键等价。
void WindowMail_SortIndices(
    std::int32_t* const first, const std::uint32_t count,
    std::int32_t (*compare)(const std::int32_t*, const std::int32_t*))
{
    std::sort(first, first + count,
        [compare](const std::int32_t left, const std::int32_t right) {
            return compare(&left, &right) < 0;
        });
}

// 4A5970各成功发送路径分别取时，不跨收件人复用时间或Input正文。
void WindowMail_FormatTimestamp(char* const destination)
{
    std::int32_t time;
    platform::ReadLegacyTime32_53BA26(&time);
    const platform::LegacyTm53B8C6* const calendar = platform::LocalTime_53B8C6(&time);
    const auto* const bytes = reinterpret_cast<const std::byte*>(calendar);
    std::int32_t minute;
    std::memcpy(&minute, bytes + 4, sizeof(minute));
    std::int32_t hour;
    std::memcpy(&hour, bytes + 8, sizeof(hour));
    std::int32_t day;
    std::memcpy(&day, bytes + 12, sizeof(day));
    std::int32_t month;
    std::memcpy(&month, bytes + 16, sizeof(month));
    std::int32_t year;
    std::memcpy(&year, bytes + 20, sizeof(year));
    std::sprintf(destination, "%4d/%2d/%2d %02d:%02d",
        year + 1900, month + 1, day, hour, minute);
}

// 原函数：0x004A32B0。
std::int32_t WindowMail_AdvanceUnreadBlink()
{
    const std::int32_t phase = (gLoginWindowRuntime.mailBlinkPhaseD67B5C + 1) % 16;
    gLoginWindowRuntime.mailBlinkPhaseD67B5C = phase;
    std::int32_t flag = gLoginWindowRuntime.mailBlinkFlagD67B60;
    if (phase == 0) {
        flag ^= 1;
        gLoginWindowRuntime.mailBlinkFlagD67B60 = flag;
    }
    return flag;
}

// 原函数：0x004A0940。
std::int32_t WindowMail_ResetSelectionState()
{
    gLoginWindowRuntime.valuesD67B40[0] = 0;
    gLoginWindowRuntime.valuesD67B40[3] = 0;
    gLoginWindowRuntime.valuesD67B40[1] = 0;
    gLoginWindowRuntime.valuesD67B40[4] = 0;
    gLoginWindowRuntime.valuesD67B40[6] = 0;
    gLoginWindowRuntime.valuesD67B40[2] = 0;
    gLoginWindowRuntime.valuesD67B40[5] = 0;
    return 0;
}

} // namespace detail

} // namespace cg::ui
