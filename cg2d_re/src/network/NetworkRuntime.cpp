#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "cg/core/m_log.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/platformTool.hpp"

#include "../scene/character_select/CharacterSelectSceneDependencies.hpp"
#include "../scene/login/LoginRequestDependencies.hpp"
#include "../scene/login/LoginStateDependencies.hpp"
#include "../world/MailState.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "NetworkRuntimeDependencies.hpp"
#include "../Yobi/map_transition.hpp"
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
void HandleServerSoundPlayback_50E680(cg::network::CgConnection&, std::int32_t type, std::int32_t repeats, std::int32_t)
{
    if (type < 1000) { static_cast<void>(audio::PlaySe_52C450(type, repeats)); }
    else { static_cast<void>(audio::PlayFixedMusic_455280(type - 1000, std::bit_cast<std::uint32_t>(repeats))); }
}

} // namespace detail

namespace detail {

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
std::int32_t __cdecl HandleSpatialSoundSourceReply_50E6C0(cg::network::CgConnection&,
    const std::int32_t index,
    const std::int32_t sourceX,
    const std::int32_t sourceY)
{
    return audio::UpdateNearestSpatialSoundSource_52CDE0(index, sourceX, sourceY);
}

// 原 [0x50CF60,0x50CF6A)：直接发布完整第二参数。
std::int32_t HandleRemainingAttributePointsReply_50CF60(cg::network::CgConnection& connection,
    const std::int32_t value)
{
    connection.Owner().WorldScene().unspentAttributePoints = value;
    return value;
}

// 原 [0x50CF20,0x50CF55)：仅第二参数控制固定提示入队。
void HandleFailureNoticeReply_50CF20(cg::network::CgConnection& connection,
    const std::int32_t status,
    const std::int32_t,
    const std::int32_t,
    const std::int32_t)
{
    if (status != 0) {
        return;
    }

    world::AppendChatMessage_44B050(connection.Owner(), L"失败了！", 4, 3);
}

// 原 [0x50D3B0,0x50D3BC)：GVC01清理无消费存储，保留低W16转换、返回及协议入口。
std::uint16_t Sub_50D3B0(cg::network::CgConnection&,
    const std::int32_t value)
{
    const std::uint16_t bits = static_cast<std::uint16_t>(value);
    return bits;
}

// 原 [0x50E310,0x50E33B)：先读门禁，只有合法范围才依序发布两槽。
std::int32_t HandleMapBgmChangeReply_50E310(cg::network::CgConnection& connection,
    const std::int32_t value)
{
    const std::uint32_t gate = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (gate != 0U) {
        return std::bit_cast<std::int32_t>(gate);
    }
    if (value >= 200 && value <= 249) {
        connection.Owner().WorldScene().pendingMapBgmIndex = static_cast<std::uint32_t>(value);
        connection.Owner().WorldScene().mapBgmChangePending = 1;
    }
    return value;
}

// 原 [0x50E630,0x50E64C)：唯一 caller 不消费未定义的早退 EAX。
void HandlePaletteTransitionReply_50E630(cg::network::CgConnection& connection,
    const std::int32_t index,
    const std::int32_t steps)
{
    if (connection.Owner().WorldScene().sceneType == 10) {
        return;
    }
    static_cast<void>(render::SelectPalette(index, steps));
}

// 原 [0x50D700,0x50D73A)：非零门禁直接返回，零时按原序发布三槽。
std::int32_t HandleDuelPointResultReply_50D700(cg::network::CgConnection& connection,
    const char* const payload)
{
    const std::uint32_t gate = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (gate != 0U) {
        return std::bit_cast<std::int32_t>(gate);
    }

    connection.Owner().WorldScene().battleResult.pendingMode = 2;
    connection.Owner().WorldScene().battleResult.dpDelta =
        ParseDelimitedBase62_52D0D0(payload, '|', 1);
    const std::int32_t second =
        ParseDelimitedBase62_52D0D0(payload, '|', 2);
    connection.Owner().WorldScene().battleResult.dpTotal = second;
    return second;
}

// LDT01：外层图号先转换；有记录先生产12字段再检查mode，无记录先检查mode。
// 原函数：0x0050DD80。
void HandleMapActorTransitionReply_50DD80(cg::network::CgConnection& connection, const std::int32_t objectIndex,
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction,
    const std::int32_t withRecord, const std::int32_t mode,
    const char* const payload)
{
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
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
            actionMode = cg::yobi::MapTransitionMode_ExpandingOrbit;
            break;
        case 1:
            actionMode = cg::yobi::MapTransitionMode_TrailUsingYOrigin;
            break;
        case 2:
            actionMode = cg::yobi::MapTransitionMode_AcceleratingBlink;
            break;
        case 3:
            actionMode = cg::yobi::MapTransitionMode_Wandering;
            value02 = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(direction));
            break;
        default:
            return;
        }
        static_cast<void>(cg::yobi::CreateMapTransitionAction_448040(connection.Owner(),
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
        actionMode = cg::yobi::MapTransitionMode_ContractingOrbit;
        break;
    case 1:
        actionMode = cg::yobi::MapTransitionMode_TrailUsingXOrigin;
        break;
    case 2:
        actionMode = cg::yobi::MapTransitionMode_DeceleratingBlink;
        break;
    case 3:
        actionMode = cg::yobi::MapTransitionMode_PairedEffects;
        break;
    default:
        return;
    }
    static_cast<void>(cg::yobi::CreateMapTransitionAction_448040(connection.Owner(),
        convertedImageNo, recordX, recordY, recordDirection,
        actionMode, 0, objectIndex));
}

// 原函数：0x0050E830。
std::int32_t HandleGuildMailResetReply_50E830(cg::network::CgConnection& connection)
{
    const std::uint32_t state = connection.Owner().WorldScene().gameplayRepliesSuppressed;
    if (state != 0) {
        return std::bit_cast<std::int32_t>(state);
    }
    cg::world::detail::ClearCurrentCharacterGuildMail_5194C0();
    return cg::world::detail::SaveGuildMailFile_519550();
}

// GMR01：原gate位型早退/记录五域重置→活动计数发布→同key邮件删除，原完整S32返回保。
// 原函数：0x0050F130。
std::int32_t HandleGuildMemberRemovalReply_50F130(cg::network::CgConnection& connection, const std::int32_t key)
{
    const std::uint32_t gate = connection.Owner().WorldScene().gameplayRepliesSuppressed;
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
void HandleGuildMailReply_50F190(cg::network::CgConnection& connection, const std::int32_t key,
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
    world::AppendChatMessage_44B050(connection.Owner(), message, 4, 3);
}

// 原函数：0x00510460。
void HandleInventorySlotSwapReply_510460(cg::network::CgConnection& connection, const std::int32_t firstGroup, const std::int32_t firstIndex,
    const std::int32_t secondGroup, const std::int32_t secondIndex)
{
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0) {
        return;
    }
    world::SwapPlayerOrPetItemSlots_51B630(connection.Owner(), firstGroup, firstIndex, secondGroup, secondIndex);
}

// IER01：原78条；首字段失败只used0，成功两文本/单空格及24属性按原序写唯一记录；当前TgX明文分发保持，运行待。
// 原函数：0x00510490。
void HandleItemDetailReply_510490(cg::network::CgConnection&, const char* const text)
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

} // namespace cg::network
