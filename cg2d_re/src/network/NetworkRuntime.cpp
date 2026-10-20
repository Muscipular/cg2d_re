#include "cg/core/LegacyNoOp.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "cg/core/m_log.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "CgConnectionLegacy.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/platformTool.hpp"

#include "../scene/character_select/CharacterSelectSceneDependencies.hpp"
#include "../scene/login/LoginRequestDependencies.hpp"
#include "../scene/login/LoginStateDependencies.hpp"
#include "../world/MailState.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "NetworkRuntimeDependencies.hpp"
#include "NetworkDispatchDependencies.hpp"

#include "cg/scene/login/CharacterCreationRequest.hpp"
#include "cg/scene/login/CharacterDeletionRequest.hpp"
#include "cg/scene/login/CharacterListRequest.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/world/GuildState.hpp"
#include "cg/scene/login/LoginConnection.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/scene/login/LoginScene.hpp"
#include "cg/render/Palette.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/world/SceneState.hpp"
#include "../world/AlbumState.hpp"
#include "../world/BattleState.hpp"
#include "../world/ChatHistory.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldObjectState.hpp"
#include "../world/WorldUpdateDependencies.hpp"


#include <ctime>
#include <Windows.h> // 原调试输出与系统错误常量；网络SDK操作已移至平台后端。

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BEC0、0x0053BE40；原参数／返回合同由直接调用承接。
#include <cstring>
#include <format>
#include <list>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053BC0F；直接沿MSVC CRT。

#include "cg/core/vm.h"

// CTM01：本地CRT时间槽显式__time64_t；协议与存档字段仍按各自原格式承接。
static_assert(sizeof(__time64_t) == sizeof(std::int64_t));
static_assert(alignof(__time64_t) == alignof(std::int64_t));

namespace cg::network {
namespace detail {

// 原50E680只取socket后的type/repeats；第三已解析字段不被原本体读取。
void HandleServerSoundPlayback_50E680(std::uintptr_t, std::int32_t type, std::int32_t repeats, std::int32_t)
{
    if (type < 1000) { static_cast<void>(audio::PlaySe_52C450(type, repeats)); }
    else { static_cast<void>(audio::PlayFixedMusic_455280(type - 1000, std::bit_cast<std::uint32_t>(repeats))); }
}

} // namespace detail

// CTO01：进程内唯一连接对象，旧接口只适配业务状态和身份镜像。
static CgConnection gGameConnection;
static_assert(sizeof(std::uintptr_t) == sizeof(platform::ConnectionId));
CgConnection& GetGameConnection() noexcept { return gGameConnection; }

std::uintptr_t BeginServerConnection(std::wstring host, std::uint16_t port, bool noDelay)
{
    login::gRequestPollingEnabled = 0;
    world::gNetworkDisconnectState_D9F2CA = 0;
    const auto id = GetGameConnection().BeginConnect(std::move(host), port, noDelay);
    platform::gConnectionSocket = static_cast<std::uintptr_t>(id);
    return platform::gConnectionSocket;
}

platform::ConnectionSnapshot PollServerConnection()
{
    const auto snapshot = GetGameConnection().PollConnection();
    platform::gConnectionSocket = static_cast<std::uintptr_t>(GetGameConnection().Id());
    return snapshot;
}

void CloseServerConnection()
{
    GetGameConnection().Close();
    platform::gConnectionSocket = 0;
    login::gRequestPollingEnabled = 0;
}

void StopNetworkIo()
{
    GetGameConnection().StopIo();
    platform::gConnectionSocket = 0;
    login::gRequestPollingEnabled = 0;
}

struct ProtocolOutputAccess {
    char* data() const noexcept
    { return detail::ConnectionLegacyAccess::OutputBuffer(GetGameConnection()); }
    std::uint32_t capacity() const noexcept
    { return detail::ConnectionLegacyAccess::BufferCapacity(GetGameConnection()); }
};

// Lua完整明文入口，实际回调与入队由当前连接执行。
std::int32_t SendLuaPacket(const char* data, const std::uint32_t length)
{
    auto& connection = GetGameConnection();
    connection.SetIoEnabled(login::gRequestPollingEnabled != 0);
    return connection.SendPacket(connection.Id(), data, length);
}

void RegisterProtocolReceive(const char* header, ProtocolReceiveHandler handler)
{ GetGameConnection().RegisterReceive(header, std::move(handler)); }

void RegisterProtocolSend(const char* header, ProtocolSendHandler handler)
{ GetGameConnection().RegisterSend(header, std::move(handler)); }

namespace detail {

// 原函数：0x005173B0。
void SplitReceivedBuffer_5173B0(char* const buffer)
{
    return detail::ConnectionLegacyAccess::SplitReceivedBuffer_5173B0(GetGameConnection(), buffer);
}

// 原函数：0x005176B0。
std::int32_t DecodeProtocolBase62_5176B0(const char* const text)
{
    const auto* cursor =
        reinterpret_cast<const unsigned char*>(text);
    std::int32_t accumulator = 0;
    std::int32_t sign;
    if (*cursor == static_cast<unsigned char>('-')) {
        sign = -1;
        ++cursor;
    }
    else {
        sign = 1;
    }

    unsigned char currentByte = *cursor;
    while (currentByte != 0U) {
        accumulator *= 62;

        std::int32_t digit;
        if (currentByte >= static_cast<unsigned char>('0')
            && currentByte <= static_cast<unsigned char>('9')) {
            digit = currentByte - static_cast<unsigned char>('0');
        }
        else if (currentByte >= static_cast<unsigned char>('a')
                 && currentByte <= static_cast<unsigned char>('z')) {
            digit = currentByte - static_cast<unsigned char>('a') + 10;
        }
        else if (currentByte >= static_cast<unsigned char>('A')
                 && currentByte <= static_cast<unsigned char>('Z')) {
            digit = currentByte - static_cast<unsigned char>('A') + 36;
        }
        else {
            return 0;
        }

        accumulator += digit;
        currentByte = cursor[1];
        ++cursor;
    }

    return sign * accumulator;
}

// 原函数：0x00517730。
std::uint8_t* EncodeProtocolBase62_517730(
    const std::int32_t value,
    std::uint8_t* const destination,
    const std::int32_t capacity)
{
    // 原 0x58B644 表复制 63 字节（含 NUL），局部第 64 字节不清零。
    std::uint8_t alphabet[64];
    std::memcpy(
        alphabet,
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
        63);
    std::int32_t remainders[64];

    const bool negative = value < 0;
    std::int32_t magnitude = value;
    if (negative) {
        magnitude = -magnitude;
    }

    // 单字符路径先读表，再写输出，不读取 capacity。
    if (magnitude < 62) {
        const std::uint8_t digit = alphabet[magnitude];
        if (negative) {
            destination[0] = '-';
            destination[1] = digit;
            destination[2] = 0;
        }
        else {
            destination[0] = digit;
            destination[1] = 0;
        }
        return destination;
    }

    std::int32_t remainderIndex = 0;
    do {
        remainders[remainderIndex] = magnitude % 62;
        magnitude /= 62;
        ++remainderIndex;
    } while (magnitude >= 62);
    --remainderIndex;

    std::int32_t destinationIndex;
    if (negative) {
        destination[1] = alphabet[magnitude];
        destination[0] = '-';
        destinationIndex = 2;
    }
    else {
        destination[0] = alphabet[magnitude];
        destinationIndex = 1;
    }

    // 前缀已写入；容量不足时保留已有字节，不补 NUL。
    while (remainderIndex >= 0) {
        if (destinationIndex > capacity - 2) {
            return nullptr;
        }
        destination[destinationIndex] = alphabet[remainders[remainderIndex]];
        ++destinationIndex;
        --remainderIndex;
    }
    destination[destinationIndex] = 0;
    return destination;
}

// 原 [0x50E6C0,0x50E6D8)：只转发后三个 W32 参数，透传空间音频生产者返回。
std::int32_t __cdecl HandleSpatialSoundSourceReply_50E6C0(
    const std::uintptr_t,
    const std::int32_t index,
    const std::int32_t sourceX,
    const std::int32_t sourceY)
{
    return audio::UpdateNearestSpatialSoundSource_52CDE0(index, sourceX, sourceY);
}

// 原 [0x50CF60,0x50CF6A)：直接发布完整第二参数。
std::int32_t HandleRemainingAttributePointsReply_50CF60(
    const std::uintptr_t,
    const std::int32_t value)
{
    world::gPlayerUnspentAttributePoints_D2F788 = value;
    return value;
}

// 原 [0x50CF20,0x50CF55)：仅第二参数控制固定提示入队。
void HandleFailureNoticeReply_50CF20(
    const std::uintptr_t,
    const std::int32_t status,
    const std::int32_t,
    const std::int32_t,
    const std::int32_t)
{
    if (status != 0) {
        return;
    }

    world::AppendChatMessage_44B050(L"失败了！", 4, 3);
}

// 原 [0x50D3B0,0x50D3BC)：GVC01清理无消费存储，保留低W16转换、返回及协议入口。
std::uint16_t Sub_50D3B0(
    const std::uintptr_t,
    const std::int32_t value)
{
    const std::uint16_t bits = static_cast<std::uint16_t>(value);
    return bits;
}

// 原 [0x50E310,0x50E33B)：先读门禁，只有合法范围才依序发布两槽。
std::int32_t HandleMapBgmChangeReply_50E310(
    const std::uintptr_t,
    const std::int32_t value)
{
    const std::uint32_t gate = world::gGameplayRepliesSuppressed_9A6E0C;
    if (gate != 0U) {
        return std::bit_cast<std::int32_t>(gate);
    }
    if (value >= 200 && value <= 249) {
        world::gPendingMapBgmIndex_FF0D5C = static_cast<std::uint32_t>(value);
        world::gMapBgmChangePending_FF0D58 = 1;
    }
    return value;
}

// 原 [0x50E630,0x50E64C)：唯一 caller 不消费未定义的早退 EAX。
void HandlePaletteTransitionReply_50E630(
    const std::uintptr_t,
    const std::int32_t index,
    const std::int32_t steps)
{
    if (world::gCurrentSceneType == 10) {
        return;
    }
    static_cast<void>(render::SelectPalette(index, steps));
}

// 原 [0x50D700,0x50D73A)：非零门禁直接返回，零时按原序发布三槽。
std::int32_t HandleDuelPointResultReply_50D700(
    const std::uintptr_t,
    const char* const payload)
{
    const std::uint32_t gate = world::gGameplayRepliesSuppressed_9A6E0C;
    if (gate != 0U) {
        return std::bit_cast<std::int32_t>(gate);
    }

    world::gPendingResultWindowMode_FF0C10 = 2;
    world::gDpResultDelta_FF0D04 =
        ParseDelimitedBase62_52D0D0(payload, '|', 1);
    const std::int32_t second =
        ParseDelimitedBase62_52D0D0(payload, '|', 2);
    world::gDpResultTotal_FF0D08 = second;
    return second;
}

// LDT01：外层图号先转换；有记录先生产12字段再检查mode，无记录先检查mode。
// 原函数：0x0050DD80。
void HandleMapActorTransitionReply_50DD80(
    const std::uintptr_t, const std::int32_t objectIndex,
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction,
    const std::int32_t withRecord, const std::int32_t mode,
    const char* const payload)
{
    if (world::gGameplayRepliesSuppressed_9A6E0C != 0) {
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    const std::int32_t convertedImageNo =
        character_select::ResolveCharacterImageForVersion(imageNo);
    if (withRecord == 0) {
        std::int16_t actionMode;
        std::int16_t value02 = 0;
        switch (mode) {
        case 0:
            actionMode = 2;
            break;
        case 1:
            actionMode = 8;
            break;
        case 2:
            actionMode = 4;
            break;
        case 3:
            actionMode = 0;
            value02 = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(direction));
            break;
        default:
            return;
        }
        static_cast<void>(cg::yobi::CreateMapTransitionAction_448040(
            convertedImageNo, mapX, mapY, direction, actionMode, value02, -1));
        return;
    }

    char field[2048];
    char name[2048];
    char titles[2048];
    const std::int32_t type = ParseDelimitedInteger_52CFA0(payload, '|', 1);
    ExtractDelimitedField_52CED0(payload, '|', 2, 2047, field);
    const std::int32_t recordIndex = Base62Decode_52D050(field);
    ExtractDelimitedField_52CED0(payload, '|', 3, 2047, field);
    const std::int32_t recordX = std::atoi(field);
    ExtractDelimitedField_52CED0(payload, '|', 4, 2047, field);
    const std::int32_t recordY = std::atoi(field);
    ExtractDelimitedField_52CED0(payload, '|', 5, 2047, field);
    const std::int32_t recordDirection = std::atoi(field);
    ExtractDelimitedField_52CED0(payload, '|', 6, 2047, field);
    const std::int32_t recordImageNo =
        character_select::ResolveCharacterImageForVersion(std::atoi(field));
    ExtractDelimitedField_52CED0(payload, '|', 7, 2047, field);
    const std::int32_t level = std::atoi(field);
    const std::int32_t value40 = ParseDelimitedInteger_52CFA0(payload, '|', 8);
    ExtractDelimitedField_52CED0(payload, '|', 9, 2047, name);
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    ExtractDelimitedField_52CED0(payload, '|', 10, 2047, titles);
    input::UnescapeLegacyTextAndNotify_52D2F0(titles);
    ExtractDelimitedField_52CED0(payload, '|', 11, 2047, field);
    const std::int32_t flag1 = std::atoi(field);
    ExtractDelimitedField_52CED0(payload, '|', 12, 2047, field);
    const std::int32_t flag2 = std::atoi(field);
    if (world::RegisterWorldObjectWithoutAction_445A60(
            recordIndex, recordImageNo, recordX, recordY, recordDirection,
            name, titles, level, static_cast<std::uint16_t>(value40),
            flag1, flag2, type) == 0) {
        return;
    }

    // 原带记录路径到此才检查mode；无效mode也保留已生产记录。
    std::int16_t actionMode;
    switch (mode) {
    case 0:
        actionMode = 3;
        break;
    case 1:
        actionMode = 9;
        break;
    case 2:
        actionMode = 5;
        break;
    case 3:
        actionMode = 1;
        break;
    default:
        return;
    }
    static_cast<void>(cg::yobi::CreateMapTransitionAction_448040(
        convertedImageNo, recordX, recordY, recordDirection,
        actionMode, 0, objectIndex));
}

// 原函数：0x0050E6E0。
void HandleServerAddressChangeReply_50E6E0(const std::uintptr_t, const char* const text)
{
    return detail::ConnectionLegacyAccess::HandleServerAddressChangeReply_50E6E0(GetGameConnection(), 0, text);
}

// 原函数：0x0050E780。
std::int32_t HandleVersionUpgradeReply_50E780(const std::uintptr_t, const std::int32_t value)
{
    return detail::ConnectionLegacyAccess::HandleVersionUpgradeReply_50E780(GetGameConnection(), 0, value);
}

// 原函数：0x0050E830。
std::int32_t HandleGuildMailResetReply_50E830(const std::uintptr_t)
{
    const std::uint32_t state = world::gGameplayRepliesSuppressed_9A6E0C;
    if (state != 0) {
        return std::bit_cast<std::int32_t>(state);
    }
    cg::world::detail::ClearCurrentCharacterGuildMail_5194C0();
    return cg::world::detail::SaveGuildMailFile_519550();
}

// GMR01：原gate位型早退/记录五域重置→活动计数发布→同key邮件删除，原完整S32返回保。
// 原函数：0x0050F130。
std::int32_t HandleGuildMemberRemovalReply_50F130(const std::uintptr_t, const std::int32_t key)
{
    const std::uint32_t gate = world::gGameplayRepliesSuppressed_9A6E0C;
    if (gate != 0) {
        return std::bit_cast<std::int32_t>(gate);
    }
    const std::int32_t index = cg::world::FindActiveOrderedRecordIndex_5195C0(key);
    if (index >= 0 && static_cast<std::size_t>(index) < cg::world::gOrderedRecords1051B14.size()) {
        auto& record = cg::world::gOrderedRecords1051B14[index];
        record.value00 = -1;
        record.value08 = 0;
        record.name18.clear();
        record.title29.clear();
        record.value0C = -1;
    }
    cg::world::gOrderedRecordCount1051510 =
        cg::world::CountRecordsValue08EqualOne_50EF50();
    return world::detail::DeleteGuildMailEntry_5194F0(key);
}

// GMR01：原字段/反转义→持有排序index→生产→外部记录名通知→存档/SE63/消息，四参未读边界保。
// 原函数：0x0050F190。
void HandleGuildMailReply_50F190(
    const std::uintptr_t, const std::int32_t key,
    const char* const payload, const std::int32_t)
{
    char name[32];
    std::wstring message;
    char text[300];
    static_cast<void>(ExtractDelimitedField_52CED0(payload, '|', 1, 31, name));
    static_cast<void>(ExtractDelimitedField_52CED0(payload, '|', 2, 299, text));
    const std::int32_t kind = ParseDelimitedInteger_52CFA0(payload, '|', 3);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    const std::int32_t index = cg::world::FindActiveOrderedRecordIndex_5195C0(key);
    static_cast<void>(world::detail::PrependGuildMailMessage_519370(
        key, 0, cg::text::FromGbkWide(name), cg::text::FromGbkWide(text)));
    const wchar_t* const recordName = index >= 0 && static_cast<std::size_t>(index) < cg::world::gOrderedRecords1051B14.size()
        ? cg::world::gOrderedRecords1051B14[index].name18.c_str() : L"";
    cg::text::FormatTo(message, kind == -1 ? L"您收到来自家族 {} 的信件～" /* 原常量参考：0x0058B0EC。 */ : L"您收到来自家族成员 {} 的宠物信件～" /* 原常量参考：0x0058B0C8。 */, recordName);
    static_cast<void>(cg::world::detail::SaveGuildMailFile_519550());
    static_cast<void>(audio::PlaySe_52C450(63, 320));
    world::AppendChatMessage_44B050(message, 4, 3);
}

// 原函数：0x00510460。
void HandleInventorySlotSwapReply_510460(
    const std::uintptr_t, const std::int32_t firstGroup, const std::int32_t firstIndex,
    const std::int32_t secondGroup, const std::int32_t secondIndex)
{
    if (world::gGameplayRepliesSuppressed_9A6E0C != 0) {
        return;
    }
    world::SwapPlayerOrPetItemSlots_51B630(firstGroup, firstIndex, secondGroup, secondIndex);
}

// IER01：原78条；首字段失败只used0，成功两文本/单空格及24属性按原序写唯一记录；当前TgX明文分发保持，运行待。
// 原函数：0x00510490。
void HandleItemDetailReply_510490(const std::uintptr_t, const char* const text)
{
    auto& record = world::gReplyRecordF7C950;
    char title[32]{};
    const auto titleResult = ExtractDelimitedField_52CED0(text, ',', 1, 31, title);
    record.text04 = cg::text::FromGbkWide(title);
    if (titleResult != 0) {
        record.used = 0;
        return;
    }
    record.used = 1;
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    record.text04 = cg::text::FromGbkWide(title);
    char description[32]{};
    static_cast<void>(ExtractDelimitedField_52CED0(
        text, ',', 2, 31, description));
    input::UnescapeLegacyTextAndNotify_52D2F0(description);
    record.text25 = cg::text::FromGbkWide(description);
    record.hasText24 = static_cast<std::uint8_t>(
        record.text25 != L" ");
    record.value48 = ParseDelimitedInteger_52CFA0(text, ',', 3);
    for (std::int32_t index = 0; index < 24; ++index) {
        record.values4C[index] = ParseDelimitedInteger_52CFA0(text, ',', index + 4);
    }
}

} // namespace detail

// 原函数：0x00517510。
static std::int32_t BuildProtocolCommand_517510(
    char* const buffer,
    const char* const command)
{
    // 与服务端nrproto_CreateHeader一致；明文命令不混淆、不生成密文状态尾。
    static_cast<void>(std::sprintf(buffer, "%s ", command));
    return 0;
}

// 原函数：0x00516DA0。
std::int32_t AllocateProtocolBuffers_516DA0(
    const std::uint32_t bufferLength)
{
    return detail::ConnectionLegacyAccess::AllocateProtocolBuffers_516DA0(GetGameConnection(), bufferLength);
}

// 原函数：0x00517080。
void CopyProtocolBytes_517080(
    std::uint8_t* const destination,
    const std::uint8_t* const source,
    const std::int32_t capacity)
{
    const std::int32_t limit = capacity - 1;

    std::int32_t index = 0;
    while (index < limit) {
        destination[index] = source[index];
        if (source[index] == 0U) {
            break;
        }
        ++index;
    }
    destination[index] = 0;
}

// 原函数：0x00517040。
void ReadProtocolCommandHeader_517040(
    std::uint32_t* const output,
    std::uint8_t* const destination,
    const std::int32_t capacity,
    char* const* const protocolArgs)
{
    *output = 0;
    const char* const source = *protocolArgs;
    CopyProtocolBytes_517080(
        destination,
        reinterpret_cast<const std::uint8_t*>(
            source != nullptr ? source : ""),
        capacity);
}

// 原函数：0x00517670。
std::uint8_t* FormatProtocolInteger_517670(const std::int32_t value)
{
    return detail::ConnectionLegacyAccess::FormatProtocolInteger_517670(GetGameConnection(), value);
}

// 原函数：0x00517690。
std::uint8_t* CopyProtocolBytesAndReturn_517690(
    std::uint8_t* const destination,
    const std::int32_t capacity,
    const std::uint8_t* const source)
{
    CopyProtocolBytes_517080(destination, source, capacity);
    return destination;
}

// 原函数：0x005170D0。
std::int32_t AppendProtocolText_5170D0(
    char* const destination,
    const char* const source,
    const std::int32_t capacity)
{
    const std::int32_t limit = capacity - 1;
    if (limit <= 0) {
        return capacity;
    }

    std::int32_t destinationIndex = 0;
    while (destinationIndex < limit
           && destination[destinationIndex] != '\0') {
        ++destinationIndex;
    }
    if (destinationIndex == limit) {
        return capacity;
    }

    if (std::strlen(destination) + std::strlen(source) >= static_cast<std::size_t>(capacity)) {
        throw cg::text::GbkStorageError(ERROR_INSUFFICIENT_BUFFER, std::system_category(), "组包超过原GBK协议容量");
    }

    std::int32_t sourceIndex = 0;
    while (destinationIndex < limit) {
        // 原517103/517105/517108：先写目标，再重读源字节判断NUL。
        destination[destinationIndex] = source[sourceIndex];
        if (source[sourceIndex] == '\0') {
            break;
        }
        ++destinationIndex;
        ++sourceIndex;
    }
    destination[destinationIndex] = '\0';
    return destinationIndex;
}

// 原函数：0x00517120。
std::uint8_t* FormatProtocolIntegerToken_517120(const std::int32_t value)
{
    return detail::ConnectionLegacyAccess::FormatProtocolIntegerToken_517120(GetGameConnection(), value);
}

// 原函数：0x00517160。
std::uint8_t* FormatProtocolTextToken_517160(const std::wstring_view text)
{
    return detail::ConnectionLegacyAccess::FormatProtocolTextToken_517160(GetGameConnection(), text);
}

// 原函数：0x00517190。
std::int32_t ParseProtocolInteger_517190(const char* const text)
{
    if (text == nullptr) {
        return 0;
    }
    return detail::DecodeProtocolBase62_5176B0(text);
}

// 原 [0x52CEA0,0x52CECB)：分隔符仅在低位字节上匹配。
const char* FindNextDelimitedField_52CEA0(
    const char* source, const char delimiter)
{
    const auto delimiterByte = static_cast<unsigned char>(delimiter);
    const auto* cursor = reinterpret_cast<const unsigned char*>(source);
    for (;;) {
        const unsigned char current = *cursor;
        if (current == 0U) {
            return nullptr;
        }
        if (current >= 0x80U) {
            ++cursor;
            if (*cursor == 0U) {
                return nullptr;
            }
            ++cursor;
        }
        else if (current == delimiterByte) {
            return reinterpret_cast<const char*>(cursor + 1);
        }
        else {
            ++cursor;
        }
    }
}

// 原 [0x52CF30,0x52CF98)：高位首字节及后继字节按原容量门禁复制。
std::int32_t CopyDelimitedField_52CF30(
    const char* const source,
    const char delimiter,
    const std::int32_t capacity,
    char* const destination)
{
    const auto delimiterByte = static_cast<unsigned char>(delimiter);
    const auto* bytes = reinterpret_cast<const unsigned char*>(source);
    std::int32_t index = 0;
    while (index < capacity) {
        const unsigned char current = bytes[index];
        if (current < 0x80U) {
            if (current == delimiterByte) {
                destination[index] = '\0';
                return 0;
            }
            destination[index] = static_cast<char>(current);
            if (current == 0U) {
                return 1;
            }
            ++index;
        }
        else {
            destination[index] = static_cast<char>(current);
            ++index;
            if (index >= capacity) {
                break;
            }
            const unsigned char second = bytes[index];
            destination[index] = static_cast<char>(second);
            if (second == 0U) {
                return 1;
            }
            ++index;
        }
    }
    destination[index] = '\0';
    return 1;
}

// 原 [0x52CED0,0x52CF27)：ordinal<=1 取首段，缺段只清首字节。
std::int32_t ExtractDelimitedField_52CED0(
    const char* source,
    const char delimiter,
    const std::int32_t ordinal,
    const std::int32_t capacity,
    char* const destination)
{
    for (std::int32_t index = 1; index < ordinal && source != nullptr;
         ++index) {
        source = FindNextDelimitedField_52CEA0(source, delimiter);
    }
    if (source == nullptr) {
        destination[0] = '\0';
        return 1;
    }
    return CopyDelimitedField_52CF30(
        source, delimiter, capacity, destination);
}

// 原 [0x52CFA0,0x52CFED)：忽略字段复制返回，空字段返回 -1。
std::int32_t ParseDelimitedInteger_52CFA0(
    const char* const source,
    const char delimiter,
    const std::int32_t ordinal)
{
    std::array<char, 128> field;
    static_cast<void>(ExtractDelimitedField_52CED0(
        source, delimiter, ordinal, 127, field.data()));
    if (field[0] == '\0') {
        return -1;
    }
    return std::atoi(field.data());
}

// 原 [0x52CFF0,0x52D04F)：保留字段提取结果；空字段不写输出。
std::int32_t ParseDelimitedIntegerStatus_52CFF0(
    const char* const source,
    const char delimiter,
    const std::int32_t ordinal,
    std::int32_t* const output)
{
    std::array<char, 128> field;
    const std::int32_t status = ExtractDelimitedField_52CED0(
        source, delimiter, ordinal, 127, field.data());
    if (field[0] == '\0') {
        return 2;
    }
    *output = std::atoi(field.data());
    return status;
}

// 原 [0x52D050,0x52D0C2)：每轮先乘 62；'-' 只改变最终符号。
std::int32_t Base62Decode_52D050(const char* const text)
{
    const auto* cursor = reinterpret_cast<const unsigned char*>(text);
    std::int32_t value = 0;
    unsigned char current = *cursor;
    std::int32_t sign = 1;
    while (current != 0U) {
        value *= 62;
        if (current >= static_cast<unsigned char>('0')
            && current <= static_cast<unsigned char>('9')) {
            value += current - static_cast<unsigned char>('0');
        }
        else if (current >= static_cast<unsigned char>('a')
                 && current <= static_cast<unsigned char>('z')) {
            value += current - static_cast<unsigned char>('a') + 10;
        }
        else if (current >= static_cast<unsigned char>('A')
                 && current <= static_cast<unsigned char>('Z')) {
            value += current - static_cast<unsigned char>('A') + 36;
        }
        else if (current == static_cast<unsigned char>('-')) {
            sign = -1;
        }
        else {
            return 0;
        }
        // 原先读取下一字节，再推进实际游标；每个字节只取样一次。
        current = cursor[1];
        ++cursor;
    }
    return sign * value;
}

// 原 [0x52D0D0,0x52D11D)：空字段返回 -1，非空透传独立 base62 解码器。
std::int32_t ParseDelimitedBase62_52D0D0(
    const char* const source,
    const char delimiter,
    const std::int32_t ordinal)
{
    std::array<char, 128> field;
    static_cast<void>(ExtractDelimitedField_52CED0(
        source, delimiter, ordinal, 127, field.data()));
    if (field[0] == '\0') {
        return -1;
    }
    return Base62Decode_52D050(field.data());
}

// 原函数：0x005171F0。
std::uint8_t* EscapeProtocolText_5171F0(const std::wstring_view text)
{
    return detail::ConnectionLegacyAccess::EscapeProtocolText_5171F0(GetGameConnection(), text);
}

// 原函数：0x005172D0。
std::uint8_t* UnescapeProtocolText_5172D0(const char* const text)
{
    return detail::ConnectionLegacyAccess::UnescapeProtocolText_5172D0(GetGameConnection(), text);
}

// 原函数：0x005171B0。
std::uint8_t* DecodeOptionalProtocolText_5171B0(const char* const text)
{
    return detail::ConnectionLegacyAccess::DecodeOptionalProtocolText_5171B0(GetGameConnection(), text);
}

// 原函数：0x00517480。
std::int32_t SendProtocolBuffer_517480(
    const std::uintptr_t socket,
    char* const buffer)
{
    GetGameConnection().SetIoEnabled(login::gRequestPollingEnabled != 0);
    return detail::ConnectionLegacyAccess::SendProtocolBuffer_517480(GetGameConnection(), socket, buffer);
}

// 原函数：0x00512C10。
std::int32_t SendConnectionProbe_512C10(
    const std::uintptr_t socket,
    const std::wstring_view text)
{
    GetGameConnection().SetIoEnabled(login::gRequestPollingEnabled != 0);
    return detail::ConnectionLegacyAccess::SendConnectionProbe_512C10(GetGameConnection(), socket, text);
}

// 原 [0x512B90,0x512BCC)：列表请求的三次共享缓冲读取保持独立。
std::int32_t SendCharacterListRequest_512B90(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Gp"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        "",
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512BD0,0x512C0C)：登出请求的三次共享缓冲读取保持独立。
std::int32_t SendLogoutRequest_512BD0(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "WA"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        "",
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512C60,0x512C9C)：保活命令后追加 593850 当前文本并发送。
std::int32_t SendKeepAliveRequest_512C60(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "DE"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        "",
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511CD0,0x511D0C)：as 请求，追加共享原字节文本。
std::int32_t RequestItemTargetAttributes_511CD0(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "as"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        "",
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511FB0,0x511FEC)：ixH 请求，追加共享原字节文本。
std::int32_t SendTradeRequest_511FB0(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "ixH"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        "",
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512230,0x51226C)：suI 请求，追加共享原字节文本。
std::int32_t WithdrawTradeConfirmation_512230(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "suI"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        "",
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512570,0x5125AC)：ih 请求，追加共享原字节文本。
std::int32_t ToggleBattleRowRequest_512570(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "ih"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        "",
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512CF0,0x512D2C)：lO 请求，追加共享原字节文本。
std::int32_t RequestLogoutToTown_512CF0(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "lO"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        "",
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512DF0,0x512E38)：容量仅在命令构造之后、整数格式化之前采样。
std::int32_t SendLoginTimestamp_512DF0(
    const std::uintptr_t socket,
    const std::uint32_t tickBits)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "CCL"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(
        std::bit_cast<std::int32_t>(tickBits));
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512D30,0x512D9B)：两项分别采样容量，格式化后重读输出缓冲。
std::int32_t SendVersionUpgradeRequest_512D30(
    const std::uintptr_t socket,
    const std::int32_t version,
    const std::wstring_view payload)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Xoee"));

    const std::int32_t integerCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const integerToken = FormatProtocolIntegerToken_517120(version);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(integerToken),
        integerCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(payload);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512DA0,0x512DE8)：命令构造后采一次容量，再格式化和追加整数。
std::int32_t SendCraftDuration_512DA0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "IPy"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x510570,0x510621)：四项各自采容量、格式化后重读共享输出缓冲。
std::int32_t SendPlayerMovementPath_510570(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::wstring_view text,
    const std::int32_t fourth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "zA"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512B00,0x512B8E)：每项均先读容量，格式化后重新读取输出缓冲。
std::int32_t SendCharacterLoginRequest_512B00(
    const std::uintptr_t socket,
    const std::int32_t field4C,
    const std::int32_t field50,
    const std::int32_t field18)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "emRV"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(field4C);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(field50);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(field18);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512AB0,0x512AF8)：一次容量采样，输出缓冲在各调用点重新读取。
std::int32_t SendCharacterDeletionRequest_512AB0(
    const std::uintptr_t socket,
    const std::int32_t registNumber)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Godf"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(registNumber);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// NLD01／原512800：摘要、口令、账号、适配器四轮独立采容量，转义后重读输出缓冲。
std::int32_t SendAccountLoginRequest_512800(
    const std::uintptr_t socket,
    const std::wstring_view executableDigest,
    const std::wstring_view password,
    const std::wstring_view account,
    const std::wstring_view adapterId)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "JFVf"));

    const std::int32_t digestCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const digestToken =
        FormatProtocolTextToken_517160(executableDigest);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(digestToken),
        digestCapacity));

    const std::int32_t passwordCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const passwordToken = FormatProtocolTextToken_517160(password);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(passwordToken),
        passwordCapacity));

    const std::int32_t accountCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const accountToken =
        FormatProtocolTextToken_517160(account);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(accountToken),
        accountCapacity));

    const std::int32_t adapterCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const adapterToken =
        FormatProtocolTextToken_517160(adapterId);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(adapterToken),
        adapterCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5128C0,0x512AAC)：每项独立读取容量，格式化后重读输出缓冲。
std::int32_t SendCharacterCreationRequest_5128C0(
    const std::uintptr_t socket,
    const std::int32_t field1,
    const std::wstring_view field2,
    const std::int32_t field3,
    const std::int32_t field4,
    const std::int32_t field5,
    const std::int32_t field6,
    const std::int32_t field7,
    const std::int32_t field8,
    const std::int32_t field9,
    const std::int32_t field10,
    const std::int32_t field11,
    const std::int32_t field12,
    const std::int32_t field13)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "aSEl"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(field1);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(field2);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::array<std::int32_t, 11> remainingIntegerFields {
        field3, field4, field5, field6, field7, field8,
        field9, field10, field11, field12, field13
    };
    for (const std::int32_t field : remainingIntegerFields) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            ProtocolOutputAccess{}.capacity());
        std::uint8_t* const token = FormatProtocolIntegerToken_517120(field);
        static_cast<void>(AppendProtocolText_5170D0(
            ProtocolOutputAccess{}.data(),
            reinterpret_cast<const char*>(token),
            capacity));
    }

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x510630,0x510704)：五轮各自采容量、格式化后重读输出缓冲。
std::int32_t SendMapTransitionRequest_510630(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "pg"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x510710,0x510807)：六轮各自采容量、格式化后重读输出缓冲。
std::int32_t RequestMapRectangle_510710(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth,
    const std::int32_t sixth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "UUN"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    const std::int32_t sixthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const sixthToken = FormatProtocolIntegerToken_517120(sixth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(sixthToken),
        sixthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原函数：0x00510AC0。
std::int32_t SendItemUseRequest_510AC0(
    const std::uintptr_t socket, const std::int32_t x, const std::int32_t y,
    const std::int32_t item, const std::int32_t target)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "iVfo"));
    const std::array<std::int32_t, 4> fields {x, y, item, target};
    for (const std::int32_t field : fields) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            ProtocolOutputAccess{}.capacity());
        std::uint8_t* const token = FormatProtocolIntegerToken_517120(field);
        static_cast<void>(AppendProtocolText_5170D0(
            ProtocolOutputAccess{}.data(),
            reinterpret_cast<const char*>(token), capacity));
    }
    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// NFI01：fIM三Base62整数按现明文普通空格发送；保原time/seed/rand与共享缓冲时序。
// 原函数：0x00510B80。
std::int32_t SendPositionDirectionInteraction_510B80(
    const std::uintptr_t socket, const std::int32_t x,
    const std::int32_t y, const std::int32_t direction)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "fIM"));
    const std::array<std::int32_t, 3> fields {x, y, direction};
    for (const std::int32_t field : fields) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            ProtocolOutputAccess{}.capacity());
        std::uint8_t* const token = FormatProtocolIntegerToken_517120(field);
        static_cast<void>(AppendProtocolText_5170D0(
            ProtocolOutputAccess{}.data(),
            reinterpret_cast<const char*>(token), capacity));
    }

    char* buffer = ProtocolOutputAccess{}.data();
    std::int32_t selected = 1;
    std::int32_t spaces = 0;
    std::uint32_t index = 0;
    if (std::strlen(buffer) != 0) {
        do {
            if (buffer[index] == ' ') {
                ++spaces;
            }
            if (spaces >= 3) {
                break;
            }
            ++index;
        } while (index < std::strlen(buffer));
        if (spaces > 0) {
            const __time64_t seconds = ::_time64(nullptr);
            static_cast<void>(
                platform::SeedRandom_53B4BF(static_cast<std::uint32_t>(seconds)));
            selected = platform::Rand16() % spaces;
            if (selected == 0) {
                selected = 1;
            }
            buffer = ProtocolOutputAccess{}.data();
        }
    }
    std::int32_t ordinal = 0;
    for (index = 0; index < std::strlen(buffer); ++index) {
        if (buffer[index] == ' ') {
            if (ordinal == selected) {
                // 服务端不以0x17终止整数token，保普通空格；原选择/RNG消费及fresh借用保持。
                buffer = ProtocolOutputAccess{}.data();
                break;
            }
            ++ordinal;
        }
    }
    return SendProtocolBuffer_517480(socket, buffer);
}

// 原 [0x510810,0x51087B)：两轮各自采容量、格式化后重读输出缓冲。
std::int32_t SendPlayerCoordinates_510810(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "KeOX"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511030,0x5110AE)：先追加数量，再逐项读取 W32 并分别采容量。
std::int32_t RequestAlbumEntries_511030(
    const std::uintptr_t socket,
    const std::int32_t* values,
    const std::int32_t count)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "kza"));

    const std::int32_t countCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const countToken = FormatProtocolIntegerToken_517120(count);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(countToken),
        countCapacity));

    for (std::int32_t index = 0; index < count; ++index) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            ProtocolOutputAccess{}.capacity());
        const std::int32_t value = values[index];
        std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
        static_cast<void>(AppendProtocolText_5170D0(
            ProtocolOutputAccess{}.data(),
            reinterpret_cast<const char*>(token),
            capacity));
    }

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511190,0x5111D8)：FPkR单整数发送；容量先采，输出缓冲独立重取。
std::int32_t RequestContactDeletion_511190(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "FPkR"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原函数：0x00511250。
std::int32_t SendGuildTextUpdate_511250(
    const std::uintptr_t socket, const std::int32_t kind, const std::wstring_view text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Ufm"));
    const std::int32_t kindCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const kindToken = FormatProtocolIntegerToken_517120(kind);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(kindToken), kindCapacity));
    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken), textCapacity));
    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原 [0x5112C0,0x51134E)：两整数、文本各自采容量并重读输出缓冲。
std::int32_t SendGuildRankDefinition_5112C0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::wstring_view text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "GY"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511350,0x5113BB)：两整数分别采容量并在格式化后重读输出缓冲。
std::int32_t SendGuildMemberRankChange_511350(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "uHb"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x510880,0x5108EB)：对战请求，发送坐标不是目标格坐标。
std::int32_t SendDuelRequest_510880(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "ik"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x510940,0x5109AB)：观战命令原字节为大小写敏感的IMv。
std::int32_t SendSpectateRequest_510940(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "IMv"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5111E0,0x51124B)：名片请求，两字段分别采容量并重读共享缓冲。
std::int32_t SendContactCardRequest_5111E0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "vcAz"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5113C0,0x51142B)：两整数各自采容量并在格式化后重读输出缓冲。
std::int32_t SendGuildInvitation_5113C0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "npzi"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511430,0x511478)：单次采容量、格式化后重读输出缓冲发送。
std::int32_t RequestGuildMemberDeletion_511430(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "dbQC"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原[510FA0,51102E)：三次采容量与格式化后的输出地址重取独立。
std::int32_t SendContactMail_510FA0(
    const std::uintptr_t socket, const std::int32_t first,
    const std::wstring_view text, const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "CV"));
    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken), firstCapacity));
    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken), textCapacity));
    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken), thirdCapacity));
    return SendProtocolBuffer_517480(socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511480,0x51150E)：整数、文本、整数分别采容量并重读输出缓冲。
std::int32_t SendGuildMail_511480(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::wstring_view text,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "uId"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原[5110B0,511184)：五字段各自采容量，格式化后重取输出地址。
std::int32_t SendContactPetMail_5110B0(
    const std::uintptr_t socket, const std::int32_t first, const std::int32_t second,
    const std::int32_t third, const std::wstring_view text, const std::int32_t fifth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "rMsv"));
    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken), firstCapacity));
    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken), secondCapacity));
    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken), thirdCapacity));
    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken), textCapacity));
    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fifthToken), fifthCapacity));
    return SendProtocolBuffer_517480(socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511510,0x5115E4)：三整数、文本、末整数分别采容量并重读输出缓冲。
std::int32_t SendGuildPetMail_511510(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::wstring_view text,
    const std::int32_t fifth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "PfqV"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5115F0,0x511638)：单次采容量、整数格式化后重读缓冲发送。
std::int32_t RequestProfileCategoryUpdates_5115F0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "HoW"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511640,0x51175A)：七项依次采容量、格式化并重读输出缓冲。
std::int32_t SendPlayerProfile_511640(
    const std::uintptr_t socket,
    const std::int32_t field1,
    const std::wstring_view text2,
    const std::int32_t field3,
    const std::wstring_view text4,
    const std::int32_t field5,
    const std::wstring_view text6,
    const std::wstring_view text7)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Ot"));

    const std::int32_t capacity1 = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token1 = FormatProtocolIntegerToken_517120(field1);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token1),
        capacity1));

    const std::int32_t capacity2 = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token2 = FormatProtocolTextToken_517160(text2);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token2),
        capacity2));

    const std::int32_t capacity3 = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token3 = FormatProtocolIntegerToken_517120(field3);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token3),
        capacity3));

    const std::int32_t capacity4 = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token4 = FormatProtocolTextToken_517160(text4);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token4),
        capacity4));

    const std::int32_t capacity5 = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token5 = FormatProtocolIntegerToken_517120(field5);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token5),
        capacity5));

    const std::int32_t capacity6 = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token6 = FormatProtocolTextToken_517160(text6);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token6),
        capacity6));

    const std::int32_t capacity7 = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token7 = FormatProtocolTextToken_517160(text7);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token7),
        capacity7));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511760,0x5117CB)：两项各自采容量、格式化后重读输出缓冲。
std::int32_t RequestPlayerProfile_511760(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "TLKH"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5117D0,0x51183B)：整数、文本各自采容量并在格式化后重读输出缓冲。
std::int32_t RequestMiniMailContactDeletion_5117D0(
    const std::uintptr_t socket,
    const std::int32_t value,
    const std::wstring_view text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "wD"));

    const std::int32_t valueCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const valueToken = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(valueToken),
        valueCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511840,0x5118AB)：文本、整数各自采容量并在格式化后重读输出缓冲。
std::int32_t SendMiniMail_511840(
    const std::uintptr_t socket,
    const std::wstring_view text,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "GH"));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t valueCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const valueToken = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(valueToken),
        valueCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5118B0,0x5118F8)：单次容量采样、整数格式化后重读缓冲发送。
std::int32_t SetProfileVisibilityRequest_5118B0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "lz"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511900,0x5119F7)：六项各自先读容量，格式化后重读输出缓冲。
// CTS01：原六字段逐项容量→token→新buffer→append，末发送S32透传。
std::int32_t SendChatMessage_511900(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::wstring_view text,
    const std::int32_t fourth,
    const std::int32_t fifth,
    const std::int32_t sixth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "uSr"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    const std::int32_t sixthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const sixthToken = FormatProtocolIntegerToken_517120(sixth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(sixthToken),
        sixthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511A00,0x511A48)：构造命令后只采一次容量，追加并发送。
std::int32_t SendPlayerOptions_511A00(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "dcMv"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511A50,0x511ADE)：三项各自采容量、格式化后重读输出缓冲。
std::int32_t SendTeamJoinLeaveRequest_511A50(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "zn"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511AE0,0x511BB4)：五项各自采容量、格式化后重读输出缓冲。
std::int32_t SendPetStates_511AE0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "LKQy"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511BC0,0x511C2B)：两项各自采容量、格式化后重读输出缓冲。
std::int32_t MovePetSlotRequest_511BC0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Ukuy"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511C30,0x511C78)：构造命令后只采一次容量，追加并发送。
std::int32_t RequestSkillTargetAttributes_511C30(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "iET"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511C80,0x511CC8)：一次容量采样，整数 token 追加后发送。
std::int32_t SelectSkillTextTarget_511C80(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "McVa"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511D10,0x511D58)：一次容量采样，整数 token 追加后发送。
std::int32_t SelectItemTextTarget_511D10(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "mjCv"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511D60,0x511DCB)：两项各自采容量、格式化后重读输出缓冲。
std::int32_t SendItemTargetPair_511D60(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "BG"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511DD0,0x511E5E)：三项各自采容量、格式化后重读输出缓冲。
std::int32_t SendWorldActionRequest_511DD0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "MIS"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511E60,0x511EEE)：三项各自采容量、格式化后重读输出缓冲。
std::int32_t SendPlayerMotionRequest_511E60(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "klF"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511EF0,0x511FA1)：三个整数及文本各自采容量、格式化后重读输出缓冲。
std::int32_t SendSkillUseRequest_511EF0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::wstring_view text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "sM"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x511FF0,0x512038)：容量在格式化前采样，追加和发送各自重读缓冲。
std::int32_t SelectTradePartnerRequest_511FF0(
    const std::uintptr_t socket, const std::int32_t index)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "QeuC"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(index);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原 [0x512040,0x5120CE)：两段文本和整数各自采容量、格式化后重读输出缓冲。
std::int32_t SendTradeOffer_512040(
    const std::uintptr_t socket,
    const std::wstring_view first,
    const std::wstring_view second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "aiAb"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolTextToken_517160(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolTextToken_517160(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5120D0,0x51215E)：三段文本各自采容量、格式化后重读输出缓冲。
std::int32_t PublishStallRequest_5120D0(
    const std::uintptr_t socket,
    const std::wstring_view first,
    const std::wstring_view second,
    const std::wstring_view third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "ylGN"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolTextToken_517160(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolTextToken_517160(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolTextToken_517160(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512160,0x512185)：建命令后重读缓冲并完整透传发送返回。
std::int32_t CloseStallRequest_512160(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "JMh"));
    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512190,0x5121D8)：一次容量采样，整数 token 追加后发送。
std::int32_t RequestStallContents_512190(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "ElVN"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5121E0,0x512228)：容量在格式化前采样，追加及发送时重读缓冲。
std::int32_t PurchaseStallOffer_5121E0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "wAo"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原函数：0x005127B0。
std::int32_t SelectBankPageRequest_5127B0(const std::uintptr_t socket, const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "rh"));
    const std::int32_t capacity =
        std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原 [0x512270,0x5122B8)：格式化前采容量，追加与发送各自重读缓冲。
std::int32_t SendTradeDecision_512270(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "OU"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5122C0,0x512308)：格式化前采容量，追加与发送各自重读缓冲。
std::int32_t SelectPlayerTitleRequest_5122C0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "zGp"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512310,0x512358)：格式化前采容量，追加与发送各自重读缓冲。
std::int32_t DeletePlayerTitleRequest_512310(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "tj"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512360,0x5123A8)：转义前采容量，追加与发送各自重读缓冲。
std::int32_t SetCustomPlayerTitleRequest_512360(
    const std::uintptr_t socket,
    const std::wstring_view text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "ACwB"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5123B0,0x5123F8)：格式化前采容量，追加与发送各自重读缓冲。
std::int32_t IncreasePlayerAttributeRequest_5123B0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "IHw"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512400,0x51246B)：两轮分别采容量、格式化后重读共享缓冲。
std::int32_t IncreasePetAttributeRequest_512400(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "kjSK"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512470,0x5124DB)：两轮分别采容量、格式化后重读共享缓冲。
std::int32_t SwapPlayerSkillOrderRequest_512470(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Noeb"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x510E30,0x510EBE)：放地请求，容量读取先于格式化，缓冲读取在其后。
std::int32_t DropPetRequest_510E30(
    const std::uintptr_t socket,
    const std::int32_t x,
    const std::int32_t y,
    const std::int32_t pet)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "FvWm"));
    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const xToken = FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(xToken), xCapacity));
    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const yToken = FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(yToken), yCapacity));
    const std::int32_t petCapacity = std::bit_cast<std::int32_t>(ProtocolOutputAccess{}.capacity());
    std::uint8_t* const petToken = FormatProtocolIntegerToken_517120(pet);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(petToken), petCapacity));
    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原 [0x5124E0,0x51256E)：三轮分别采容量、格式化后重读共享缓冲。
std::int32_t SwapPetSkillOrderRequest_5124E0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "HJQp"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x5125B0,0x51261B)：两轮各自采容量、格式化后重读共享输出缓冲。
std::int32_t RenamePetRequest_5125B0(
    const std::uintptr_t socket,
    const std::int32_t value,
    const std::wstring_view text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "ycX"));

    const std::int32_t valueCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const valueToken = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(valueToken),
        valueCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// 原 [0x512620,0x512717)：六轮分别采容量、格式化后重读共享输出缓冲。
// SDM01：原512620完整77条；xD五S32及文本各先容量再token/fresh输出，当前原字段/LF明文及S32返回保。
std::int32_t SendDialogResponse_512620(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth,
    const std::wstring_view text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "xD"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

// IMT01：原55条，四S32字段容量先格式/输出重读及S32发送返回，当前原字段/LF明文，正确正文保。
// 原函数：0x00510CE0。
std::int32_t DropItemRequest_510CE0(
    const std::uintptr_t socket, const std::int32_t x, const std::int32_t y,
    const std::int32_t group, const std::int32_t index)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "QpfE"));

    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const xToken = FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(xToken), xCapacity));

    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const yToken = FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(yToken), yCapacity));

    const std::int32_t groupCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const groupToken = FormatProtocolIntegerToken_517120(group);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(groupToken), groupCapacity));

    const std::int32_t indexCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const indexToken = FormatProtocolIntegerToken_517120(index);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(indexToken), indexCapacity));

    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原函数：0x00510A00。
std::int32_t BeginItemUseRequest_510A00(
    const std::uintptr_t socket, const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t group)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Ak"));

    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const xToken = FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(xToken), xCapacity));

    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const yToken = FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(yToken), yCapacity));

    const std::int32_t indexCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const indexToken = FormatProtocolIntegerToken_517120(index);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(indexToken), indexCapacity));

    const std::int32_t groupCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const groupToken = FormatProtocolIntegerToken_517120(group);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(groupToken), groupCapacity));

    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原函数：0x00516C90。
std::int32_t MoveItemBetweenOwnersRequest_516C90(
    const std::uintptr_t socket, const std::int32_t sourceGroup, const std::int32_t source,
    const std::int32_t targetGroup, const std::int32_t target, const std::int32_t amount)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Cpa"));

    const std::int32_t sourceGroupCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const sourceGroupToken = FormatProtocolIntegerToken_517120(sourceGroup);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(sourceGroupToken), sourceGroupCapacity));

    const std::int32_t sourceCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const sourceToken = FormatProtocolIntegerToken_517120(source);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(sourceToken), sourceCapacity));

    const std::int32_t targetGroupCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const targetGroupToken = FormatProtocolIntegerToken_517120(targetGroup);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(targetGroupToken), targetGroupCapacity));

    const std::int32_t targetCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const targetToken = FormatProtocolIntegerToken_517120(target);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(targetToken), targetCapacity));

    const std::int32_t amountCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const amountToken = FormatProtocolIntegerToken_517120(amount);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(amountToken), amountCapacity));

    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原函数：0x00516D70。
std::int32_t SendIuEPacket_516D70(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "iuE"));
    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原函数：0x005109B0。
std::int32_t SendBattlePacket_5109B0(
    const std::uintptr_t socket, const std::wstring_view text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "Rg"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// MDR01：原510DA0完整44条；woz三个S32，各容量先token/fresh输出，末真实S32发送结果，原字段/LF明文。
std::int32_t DropGoldRequest_510DA0(
    const std::uintptr_t socket, const std::int32_t x,
    const std::int32_t y, const std::int32_t amount)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "woz"));

    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const xToken = FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(xToken), xCapacity));

    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const yToken = FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(yToken), yCapacity));

    const std::int32_t amountCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const amountToken = FormatProtocolIntegerToken_517120(amount);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(amountToken), amountCapacity));

    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原函数：0x00510EC0。
std::int32_t MoveItemStackRequest_510EC0(
    const std::uintptr_t socket, const std::int32_t source,
    const std::int32_t target, const std::int32_t amount)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "yi"));

    const std::int32_t sourceCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const sourceToken = FormatProtocolIntegerToken_517120(source);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(sourceToken), sourceCapacity));

    const std::int32_t targetCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const targetToken = FormatProtocolIntegerToken_517120(target);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(targetToken), targetCapacity));

    const std::int32_t amountCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const amountToken = FormatProtocolIntegerToken_517120(amount);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(amountToken), amountCapacity));

    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原函数：0x00510F50。
std::int32_t SendSkillRebirthSelection_510F50(
    const std::uintptr_t socket, const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "fS"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原函数：0x005108F0。
std::int32_t AcknowledgeBattleExit_5108F0(
    const std::uintptr_t socket, const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "eL"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, ProtocolOutputAccess{}.data());
}

// 原 [0x512720,0x5127AE)：三项各自采容量、格式化后重读共享输出缓冲。
std::int32_t SendPlayerPositionAndDirection_512720(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        ProtocolOutputAccess{}.data(), "cZt"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        ProtocolOutputAccess{}.capacity());
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        ProtocolOutputAccess{}.data(),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        ProtocolOutputAccess{}.data());
}

namespace detail {

// 原函数：0x00512E40。
std::int32_t NetworkDispatch_512E40(
    const std::uintptr_t socket, const void* const received)
{
    GetGameConnection().SetIoEnabled(login::gRequestPollingEnabled != 0);
    return detail::ConnectionLegacyAccess::NetworkDispatch_512E40(GetGameConnection(), socket, received);
}

} // namespace detail

// 原函数：0x00516B60。
std::int32_t AllocateNetworkWorkspace_516B60(
    const NetworkCallback callback,
    const std::uint32_t bufferLength,
    const std::uintptr_t)
{
    return detail::ConnectionLegacyAccess::AllocateNetworkWorkspace_516B60(GetGameConnection(), callback, bufferLength, 0);
}

// 原函数：0x00508140。
std::int32_t InitializeNetworkRuntime_508140()
{
    const bool initialized = GetGameConnection().Initialize();
    platform::gConnectionSocket = static_cast<std::uintptr_t>(GetGameConnection().Id());
    login::gRequestPollingEnabled = 0;
    if (initialized) world::gNetworkDisconnectState_D9F2CA = 0;
    return initialized ? 1 : 0;
}

// 原函数：0x00507DF0。
std::uint8_t NetworkLoop_507DF0()
{
    auto& connection = GetGameConnection();
    if (!connection.Active()) return 0;
    const auto state = world::gNetworkDisconnectState_D9F2CA;
    if (state == 1) {
        const auto scene = world::SetCurrentSceneType(11);
        world::gNetworkDisconnectState_D9F2CA = 2;
        return static_cast<std::uint8_t>(scene);
    }
    if (state != 0) return state;
    connection.SetIoEnabled(login::gRequestPollingEnabled != 0);
    const auto result = connection.Pump();
    platform::gConnectionSocket = static_cast<std::uintptr_t>(connection.Id());
    if (result.terminated) {
        login::gRequestPollingEnabled = 0;
        world::gNetworkDisconnectState_D9F2CA = 1;
        const auto diagnostic = std::format(L"NETWORK CLOSED PHASE={} ERROR={} ENDPOINT={}",
            static_cast<int>(result.terminal.phase), result.terminal.error, result.terminal.endpoint);
        static_cast<void>(login::detail::AppendSystemLog_44B5C0(diagnostic));
    }
    return result.value;
}

// 原函数：0x005081E0。
std::int32_t AppendReceiveQueue_5081E0(
    const void* const source,
    const std::uint32_t size)
{
    GetGameConnection().SetIoEnabled(login::gRequestPollingEnabled != 0);
    return detail::ConnectionLegacyAccess::AppendReceiveQueue_5081E0(GetGameConnection(), source, size);
}

// 原函数：0x00508230。
std::int32_t ConsumeReceiveQueue_508230(const std::int32_t size)
{
    GetGameConnection().SetIoEnabled(login::gRequestPollingEnabled != 0);
    return detail::ConnectionLegacyAccess::ConsumeReceiveQueue_508230(GetGameConnection(), size);
}

std::uint8_t ReadNetworkRuntimeActive() noexcept
{ return GetGameConnection().Active() ? 1 : 0; }

std::int32_t ReadNetworkErrorD9F2CC() noexcept
{ return GetGameConnection().Error(); }

std::uint16_t ReadProtocolModeDA3B22() noexcept
{
    return detail::ConnectionLegacyAccess::ReadProtocolModeDA3B22(GetGameConnection());
}

void WriteProtocolModeDA3B22(const std::uint16_t value) noexcept
{
    return detail::ConnectionLegacyAccess::WriteProtocolModeDA3B22(GetGameConnection(), value);
}

// 原函数：0x005081A0。
void ShutdownNetworkRuntime_5081A0()
{
    GetGameConnection().Shutdown();
    platform::gConnectionSocket = 0;
    login::gRequestPollingEnabled = 0;
    world::gNetworkDisconnectState_D9F2CA = 0;
}

} // namespace cg::network
