#include "cg/network/NetworkRuntime.hpp"

#include "../LoginStateDependencies.hpp"
#include "NetworkRuntimeDependencies.hpp"

#include "cg/LegacyAudio.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/LoginScene.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"

#include <winsock2.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>

namespace cg::network {
namespace {

constexpr std::size_t kWorkspaceSlotCount = 18;

using SendProtocolObserver = void (*)(
    std::uint32_t direction,
    char* buffer);

std::int32_t DefaultNetworkCallback(
    std::uintptr_t,
    const void*,
    std::uint32_t)
{
    return 0;
}

// 原网络运行期全局的唯一原生 x64 owner；状态不跨模块公开。
struct NetworkRuntimeState {
    // 原 0x593FB0、0x593FB4、0x593FB8、0x593FBC、
    // 0x593FA0、0x593FA4、0x593FA8、0x593FAC，均为 W32。
    std::uint32_t protocolWord0;
    std::uint32_t protocolWord1;
    std::uint32_t protocolWord2;
    std::uint32_t protocolWord3;
    std::uint32_t protocolWord4;
    std::uint32_t protocolWord5;
    std::uint32_t protocolWord6;
    std::uint32_t protocolWord7;

    // 原 [0x593FC4,0x593FD0) 连续协议短状态。
    std::array<std::uint8_t, 12> protocolState593FC4;
    std::uint16_t protocolModeDA3B22;
    std::time_t timeDA3B28;
    std::tm* localTimeDA3B2C;
    std::uint8_t protocolByte593850;

    NetworkCallback callback; // 原 0x58B5E8。
    std::uint32_t bufferLength;
    std::uint8_t* buffer58B5F0;
    std::uint8_t* buffer58B5F4;
    std::uint8_t* buffer58B5F8;
    std::uint8_t* buffer58B5FC;
    char** pointerTable58B614;
    std::uint8_t* tripleBuffer58B604;
    std::uint8_t* tripleBuffer58B608;
    std::uint8_t* tripleBuffer58B60C;
    std::uint8_t* tripleBuffer58B610;
    SendProtocolObserver sendObserver58B61C;
    char sendLogPathDA3B58[0x400];
    std::uint8_t integerFormatScratch[64]; // 原 [0xDA3F58,0xDA3F98)。
    void** workspaceSlots; // 原 0xDA3F98，18 个本机指针槽。
    std::uint8_t active;   // 原 0xD9F2C8，网络运行期活动门禁。
    std::uint32_t valueD9F2D0; // 原 0xD9F2D0，仅保留地址身份的 W32。
};

NetworkRuntimeState gNetworkRuntimeState {};

std::uint16_t ReadProtocolStateW16(const std::size_t offset)
{
    std::uint16_t value;
    std::memcpy(
        &value,
        gNetworkRuntimeState.protocolState593FC4.data() + offset,
        sizeof(value));
    return value;
}

void WriteProtocolStateW16(
    const std::size_t offset,
    const std::uint16_t value)
{
    std::memcpy(
        gNetworkRuntimeState.protocolState593FC4.data() + offset,
        &value,
        sizeof(value));
}

[[maybe_unused]] std::int32_t EncryptPrivatePacket_401960(
    const std::uint32_t sourceLength,
    std::uint8_t* const source,
    std::uint16_t& outputCapacityAndLength,
    std::uint8_t* const output,
    const std::uint32_t keyLength,
    const std::array<std::uint32_t, 4>& key) noexcept
{
    if (keyLength != 16U) {
        return 1;
    }

    const std::uint32_t required = (sourceLength + 9U) & ~7U;
    if (required > 0xFFFFU) {
        return 3;
    }
    if (outputCapacityAndLength < required) {
        outputCapacityAndLength = static_cast<std::uint16_t>(required);
        return 2;
    }

    const std::uint16_t encodedSourceLength =
        static_cast<std::uint16_t>(sourceLength);
    source[required - 2U] = static_cast<std::uint8_t>(encodedSourceLength);
    source[required - 1U] =
        static_cast<std::uint8_t>(encodedSourceLength >> 8U);

    std::uint32_t chainK1 = key[1];
    for (std::uint32_t offset = 0; offset < required; offset += 8U) {
        std::uint32_t left =
            static_cast<std::uint32_t>(source[offset])
            | (static_cast<std::uint32_t>(source[offset + 1U]) << 8U)
            | (static_cast<std::uint32_t>(source[offset + 2U]) << 16U)
            | (static_cast<std::uint32_t>(source[offset + 3U]) << 24U);
        std::uint32_t right =
            static_cast<std::uint32_t>(source[offset + 4U])
            | (static_cast<std::uint32_t>(source[offset + 5U]) << 8U)
            | (static_cast<std::uint32_t>(source[offset + 6U]) << 16U)
            | (static_cast<std::uint32_t>(source[offset + 7U]) << 24U);

        std::uint32_t sum = 0;
        for (std::uint32_t round = 0; round < 32U; ++round) {
            sum += 0x9E3779B9U;
            left += ((right << 4U) + key[0])
                ^ (right + sum)
                ^ ((right >> 5U) + chainK1);
            right += ((left << 4U) + key[2])
                ^ (left + sum)
                ^ ((left >> 5U) + key[3]);
        }
        chainK1 = left;

        output[offset] = static_cast<std::uint8_t>(left);
        output[offset + 1U] = static_cast<std::uint8_t>(left >> 8U);
        output[offset + 2U] = static_cast<std::uint8_t>(left >> 16U);
        output[offset + 3U] = static_cast<std::uint8_t>(left >> 24U);
        output[offset + 4U] = static_cast<std::uint8_t>(right);
        output[offset + 5U] = static_cast<std::uint8_t>(right >> 8U);
        output[offset + 6U] = static_cast<std::uint8_t>(right >> 16U);
        output[offset + 7U] = static_cast<std::uint8_t>(right >> 24U);
    }

    outputCapacityAndLength = static_cast<std::uint16_t>(required);
    return 0;
}

struct ProtocolOutputAccess {
    char* data() const noexcept
    {
        return reinterpret_cast<char*>(
            gNetworkRuntimeState.buffer58B5F0);
    }
};

} // namespace

namespace detail {

char Sub_4038A0(
    char* const text,
    const std::int32_t,
    const char* const key3,
    const std::int32_t,
    const char* const key5)
{
    // 0x4038AB..0x40392C：先各扫描一次，再以完整长度复查空密钥。
    const std::size_t textLength = std::strlen(text);
    const std::size_t initialKey3Length = std::strlen(key3);
    const std::size_t initialKey5Length = std::strlen(key5);
    const bool hasKey3 = std::strlen(key3) != 0U;
    const bool hasBothKeys = hasKey3 && std::strlen(key5) != 0U;
    const std::int32_t length3 = hasBothKeys
        ? static_cast<std::uint16_t>(initialKey3Length) : 1;
    const std::int32_t length5 = hasBothKeys
        ? static_cast<std::uint16_t>(initialKey5Length) : 1;
    const std::int32_t count = static_cast<std::uint16_t>(textLength);

    char lastDirection = 0;
    for (std::int32_t index = 0; index < count; ++index) {
        // 0x403948..0x403970：先取 key5，再取 key3；按 S8 扩展后掩码。
        const std::int32_t key5Byte = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(key5[(index + 84) % length5]));
        const std::int32_t b = (key5Byte & 0x0CAB) + index;
        const std::int32_t key3Byte = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(key3[(index + 56) % length3]));
        const std::int32_t a = (key3Byte & 0xBBCC) + index;

        // 0x40398E..0x4039DD：目标表四项，方向区只提供当轮 '+'/'-'。
        std::int32_t target = 0;
        switch ((a + b) % 4) {
        case 0:
            target = a & b & index;
            break;
        case 1:
            target = (a ^ b) | index;
            break;
        case 2:
            target = a * b + index;
            break;
        case 3:
            target = (a | b) ^ index;
            break;
        default:
            target = (a - b) & index;
            break;
        }
        std::int32_t delta = target & 7;
        if (delta == 7) {
            delta = 6;
        }
        const char direction = static_cast<char>((a & 2) + 43);

        // 0x4039ED..0x403A37：文本逐字节 S8 读取，按 U32 比较并写回低 W8。
        std::int32_t value = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(text[index]));
        if (direction == '+') {
            value += delta;
            const std::uint32_t unsignedValue =
                static_cast<std::uint32_t>(value);
            if (unsignedValue > 122U
                || (unsignedValue > 90U && unsignedValue < 97U)) {
                value -= 26;
            }
        }
        else {
            value -= delta;
            const std::uint32_t unsignedValue =
                static_cast<std::uint32_t>(value);
            if (unsignedValue < 65U
                || (unsignedValue > 90U && unsignedValue < 97U)) {
                value += 26;
            }
        }
        text[index] = std::bit_cast<char>(
            static_cast<std::uint8_t>(value));
        lastDirection = direction;
    }
    return lastDirection;
}

std::int32_t Sub_5176B0(const char* const text)
{
    const auto* cursor =
        reinterpret_cast<const unsigned char*>(text);
    const bool negative = *cursor == static_cast<unsigned char>('-');
    if (negative) {
        ++cursor;
    }

    unsigned char currentByte = *cursor;
    std::uint32_t accumulator = 0;
    while (currentByte != 0U) {
        accumulator *= 62U;

        std::uint32_t digit = 0;
        if (currentByte >= static_cast<unsigned char>('0')
            && currentByte <= static_cast<unsigned char>('9')) {
            digit = static_cast<std::uint32_t>(
                currentByte - static_cast<unsigned char>('0'));
        }
        else if (currentByte >= static_cast<unsigned char>('a')
                 && currentByte <= static_cast<unsigned char>('z')) {
            digit = static_cast<std::uint32_t>(
                currentByte - static_cast<unsigned char>('a')) + 10U;
        }
        else if (currentByte >= static_cast<unsigned char>('A')
                 && currentByte <= static_cast<unsigned char>('Z')) {
            digit = static_cast<std::uint32_t>(
                currentByte - static_cast<unsigned char>('A')) + 36U;
        }
        else {
            return 0;
        }

        accumulator += digit;
        currentByte = cursor[1];
        ++cursor;
    }

    const std::uint32_t resultBits = negative
        ? std::uint32_t {0} - accumulator
        : accumulator;
    return std::bit_cast<std::int32_t>(resultBits);
}

std::uint8_t* Sub_517730(
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

// 原 sub_5082D0/508310：先记录 Winsock 错误，再过滤 would-block。
std::int32_t Sub_5082D0()
{
    const std::int32_t error = ::WSAGetLastError();
    char message[256];
    std::sprintf(message, "RECV ERROR=%d", error);
    static_cast<void>(login::detail::Sub_44B5C0(message));
    return error == WSAEWOULDBLOCK ? 0 : error;
}

std::int32_t Sub_508310()
{
    const std::int32_t error = ::WSAGetLastError();
    char message[256];
    std::sprintf(message, "SNED ERROR=%d", error);
    static_cast<void>(login::detail::Sub_44B5C0(message));
    return error == WSAEWOULDBLOCK ? 0 : error;
}

// 原 [0x50E6C0,0x50E6D8)：只转发后三个 W32 参数，透传空间音频生产者返回。
std::int32_t __cdecl Sub_50E6C0(
    const std::int32_t,
    const std::int32_t index,
    const std::int32_t sourceX,
    const std::int32_t sourceY)
{
    return audio::Sub_52CDE0(index, sourceX, sourceY);
}

} // namespace detail

namespace {

[[maybe_unused]] void HandleResponse_508800(
    const std::uintptr_t,
    const std::int32_t value,
    const char* const text)
{
    if (login::gRequestStatus != 1) {
        return;
    }

    login::gRequestStatus = 2;
    login::gValueDA3B24 = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(value));
    if (value != 0) {
        if (std::strcmp(text, "busy") == 0) {
            login::gValueDA3B24 = 1234;
        }
        return;
    }

    gNetworkRuntimeState.protocolModeDA3B22 = 1;
    gNetworkRuntimeState.timeDA3B28 = std::time(nullptr);
    gNetworkRuntimeState.localTimeDA3B2C = std::localtime(
        &gNetworkRuntimeState.timeDA3B28);

    const char* const separator = std::strstr(text, "|");
    std::memset(
        character_select::detail::gText9A4EAC,
        0,
        sizeof(character_select::detail::gText9A4EAC));
    if (separator != nullptr) {
        std::memcpy(
            character_select::detail::gText9A4EAC,
            text,
            static_cast<std::size_t>(separator - text));
        platform::ParseProtectionTables_404600(separator);
        return;
    }

    std::memcpy(
        character_select::detail::gText9A4EAC,
        text,
        std::strlen(text) + 1U);
}

std::int32_t BuildProtocolCommand_517510(
    char* const buffer,
    const char* const command)
{
    std::array<char, 8> temp;
    temp[0] = static_cast<char>(
        gNetworkRuntimeState.protocolByte593850);
    const std::size_t commandLength = std::strlen(command);

    const std::uint32_t clearedBytes = 0;
    std::memcpy(temp.data() + 1, &clearedBytes, sizeof(clearedBytes));

    const std::size_t dwordLength = commandLength & ~std::size_t {3};
    std::size_t copyOffset = 0;
    while (copyOffset < dwordLength) {
        std::uint32_t commandDword;
        std::memcpy(
            &commandDword,
            command + copyOffset,
            sizeof(commandDword));
        std::memcpy(
            temp.data() + copyOffset,
            &commandDword,
            sizeof(commandDword));
        copyOffset += sizeof(commandDword);
    }

    const std::uint16_t firstMode =
        gNetworkRuntimeState.protocolModeDA3B22;
    while (copyOffset < commandLength) {
        temp[copyOffset] = command[copyOffset];
        ++copyOffset;
    }

    if (firstMode == 0) {
        static_cast<void>(detail::Sub_4038A0(
            temp.data(), 25, "uCYPWdjs", 120, "MsItMVhgbWP"));
    } else {
        static_cast<void>(detail::Sub_4038A0(
            temp.data(),
            31,
            login::gValueDA3A5C,
            121,
            login::gValueDA38DC));
    }

    static_cast<void>(std::sprintf(buffer, "%s ", temp.data()));

    std::array<char, 8> checksumSlot;
    checksumSlot[0] = static_cast<char>(
        gNetworkRuntimeState.protocolByte593850);
    const std::int32_t passwordLength = static_cast<std::int32_t>(
        std::strlen(login::gValueDA3A5C));

    WriteProtocolStateW16(0, 0);
    checksumSlot[1] = 0;
    checksumSlot[2] = 0;
    gNetworkRuntimeState.protocolState593FC4[2] = 0;

    if (temp[0] != '\0') {
        std::size_t index = 0;
        do {
            std::uint16_t sum = ReadProtocolStateW16(0);
            sum = static_cast<std::uint16_t>(
                sum
                + (static_cast<std::uint8_t>(temp[index]) & 0x7FU));
            if (sum > 255U) {
                sum = static_cast<std::uint16_t>(sum - 255U);
            }
            WriteProtocolStateW16(0, sum);
            ++index;
        } while (temp[index] != '\0');
    }

    std::uint16_t checksum = ReadProtocolStateW16(0);
    const std::uint16_t secondMode =
        gNetworkRuntimeState.protocolModeDA3B22;
    if (secondMode == 0) {
        checksum = static_cast<std::uint16_t>(checksum ^ 59U);
    } else {
        const std::int32_t divisor = passwordLength == 0
            ? 1 : passwordLength;
        const std::int32_t passwordIndex = 82 % divisor;
        const std::int8_t passwordByte = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(
                login::gValueDA3A5C[passwordIndex]));
        const std::int16_t widenedPasswordByte =
            static_cast<std::int16_t>(passwordByte);
        const std::uint32_t passwordValue = static_cast<std::uint32_t>(
            std::bit_cast<std::uint16_t>(widenedPasswordByte));
        checksum = static_cast<std::uint16_t>(
            static_cast<std::uint32_t>(checksum) ^ passwordValue);
    }
    WriteProtocolStateW16(0, checksum);

    static_cast<void>(::_itoa(
        static_cast<int>(ReadProtocolStateW16(0)),
        checksumSlot.data(),
        16));
    const std::size_t checksumLength = std::strlen(checksumSlot.data());
    std::memcpy(
        gNetworkRuntimeState.protocolState593FC4.data(),
        checksumSlot.data(),
        checksumLength);
    const std::size_t storedLength = std::strlen(
        reinterpret_cast<const char*>(
            gNetworkRuntimeState.protocolState593FC4.data()));
    WriteProtocolStateW16(8, static_cast<std::uint16_t>(storedLength));
    return 0;
}

} // namespace

std::int32_t ResetProtocolState_4037D0()
{
    if (gNetworkRuntimeState.protocolWord0 != 0x1B768BA7U) {
        gNetworkRuntimeState.protocolWord0 = 0x1B768BA7U;
    }
    if (gNetworkRuntimeState.protocolWord1 != 0x25957EF9U) {
        gNetworkRuntimeState.protocolWord1 = 0x25957EF9U;
    }
    if (gNetworkRuntimeState.protocolWord2 != 0x0FB1BFB8U) {
        gNetworkRuntimeState.protocolWord2 = 0x0FB1BFB8U;
    }
    if (gNetworkRuntimeState.protocolWord3 != 0x3039EA46U) {
        gNetworkRuntimeState.protocolWord3 = 0x3039EA46U;
    }
    if (gNetworkRuntimeState.protocolWord4 != 0x135D42BAU) {
        gNetworkRuntimeState.protocolWord4 = 0x135D42BAU;
    }
    if (gNetworkRuntimeState.protocolWord5 != 0x3947BF98U) {
        gNetworkRuntimeState.protocolWord5 = 0x3947BF98U;
    }
    if (gNetworkRuntimeState.protocolWord6 != 0x0F1A0485U) {
        gNetworkRuntimeState.protocolWord6 = 0x0F1A0485U;
    }
    if (gNetworkRuntimeState.protocolWord7 != 0x259F4C5EU) {
        gNetworkRuntimeState.protocolWord7 = 0x259F4C5EU;
    }

    WriteProtocolStateW16(8, 0);
    WriteProtocolStateW16(10, 0);
    WriteProtocolStateW16(0, 0);
    WriteProtocolStateW16(4, 0);
    gNetworkRuntimeState.protocolState593FC4[2] = 0;
    gNetworkRuntimeState.protocolState593FC4[6] = 0;
    return 0;
}

std::int32_t AllocateProtocolBuffers_516DA0(
    const std::uint32_t bufferLength)
{
    gNetworkRuntimeState.bufferLength = bufferLength;

    // 原函数不释放旧值；重复调用会先丢失旧指针并保留原泄漏行为。
    gNetworkRuntimeState.buffer58B5F0 = nullptr;
    gNetworkRuntimeState.buffer58B5F4 = nullptr;
    gNetworkRuntimeState.buffer58B5F8 = nullptr;
    gNetworkRuntimeState.buffer58B5FC = nullptr;
    gNetworkRuntimeState.pointerTable58B614 = nullptr;
    gNetworkRuntimeState.tripleBuffer58B604 = nullptr;
    gNetworkRuntimeState.tripleBuffer58B608 = nullptr;
    gNetworkRuntimeState.tripleBuffer58B60C = nullptr;
    gNetworkRuntimeState.tripleBuffer58B610 = nullptr;

    const std::uint32_t tripleLength = bufferLength * 3U;
    gNetworkRuntimeState.buffer58B5F0 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(bufferLength)));
    gNetworkRuntimeState.buffer58B5F4 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(bufferLength)));
    gNetworkRuntimeState.buffer58B5F8 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(bufferLength)));
    gNetworkRuntimeState.buffer58B5FC = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(bufferLength)));
    gNetworkRuntimeState.pointerTable58B614 = static_cast<char**>(
        std::malloc(
            static_cast<std::size_t>(bufferLength) * sizeof(char*)));
    gNetworkRuntimeState.tripleBuffer58B604 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(tripleLength)));
    gNetworkRuntimeState.tripleBuffer58B608 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(tripleLength)));
    gNetworkRuntimeState.tripleBuffer58B60C = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(tripleLength)));
    gNetworkRuntimeState.tripleBuffer58B610 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(tripleLength)));

    if (bufferLength != 0U) {
        if (gNetworkRuntimeState.buffer58B5F0 == nullptr) {
            std::terminate();
        }
        std::memset(
            gNetworkRuntimeState.buffer58B5F0,
            0,
            static_cast<std::size_t>(bufferLength));
    }
    if (bufferLength != 0U) {
        if (gNetworkRuntimeState.buffer58B5F4 == nullptr) {
            std::terminate();
        }
        std::memset(
            gNetworkRuntimeState.buffer58B5F4,
            0,
            static_cast<std::size_t>(bufferLength));
    }
    if (bufferLength != 0U) {
        if (gNetworkRuntimeState.buffer58B5F8 == nullptr) {
            std::terminate();
        }
        std::memset(
            gNetworkRuntimeState.buffer58B5F8,
            0,
            static_cast<std::size_t>(bufferLength));
    }
    if (bufferLength != 0U) {
        if (gNetworkRuntimeState.buffer58B5FC == nullptr) {
            std::terminate();
        }
        std::memset(
            gNetworkRuntimeState.buffer58B5FC,
            0,
            static_cast<std::size_t>(bufferLength));
    }
    if (bufferLength != 0U) {
        if (gNetworkRuntimeState.pointerTable58B614 == nullptr) {
            std::terminate();
        }
        std::fill_n(
            gNetworkRuntimeState.pointerTable58B614,
            static_cast<std::size_t>(bufferLength),
            nullptr);
    }
    if (tripleLength != 0U) {
        if (gNetworkRuntimeState.tripleBuffer58B604 == nullptr) {
            std::terminate();
        }
        std::memset(
            gNetworkRuntimeState.tripleBuffer58B604,
            0,
            static_cast<std::size_t>(tripleLength));
    }
    if (tripleLength != 0U) {
        if (gNetworkRuntimeState.tripleBuffer58B608 == nullptr) {
            std::terminate();
        }
        std::memset(
            gNetworkRuntimeState.tripleBuffer58B608,
            0,
            static_cast<std::size_t>(tripleLength));
    }
    if (tripleLength != 0U) {
        if (gNetworkRuntimeState.tripleBuffer58B60C == nullptr) {
            std::terminate();
        }
        std::memset(
            gNetworkRuntimeState.tripleBuffer58B60C,
            0,
            static_cast<std::size_t>(tripleLength));
    }
    if (tripleLength != 0U) {
        if (gNetworkRuntimeState.tripleBuffer58B610 == nullptr) {
            std::terminate();
        }
        std::memset(
            gNetworkRuntimeState.tripleBuffer58B610,
            0,
            static_cast<std::size_t>(tripleLength));
    }

    if (gNetworkRuntimeState.buffer58B5F0 == nullptr) {
        goto allocationFailure;
    }
    if (gNetworkRuntimeState.buffer58B5F4 == nullptr) {
        goto allocationFailure;
    }
    if (gNetworkRuntimeState.buffer58B5F8 == nullptr) {
        goto allocationFailure;
    }
    if (gNetworkRuntimeState.buffer58B5FC == nullptr) {
        goto allocationFailure;
    }
    if (gNetworkRuntimeState.pointerTable58B614 == nullptr) {
        goto allocationFailure;
    }
    if (gNetworkRuntimeState.tripleBuffer58B604 == nullptr) {
        goto allocationFailure;
    }
    if (gNetworkRuntimeState.tripleBuffer58B608 == nullptr) {
        goto allocationFailure;
    }
    if (gNetworkRuntimeState.tripleBuffer58B60C == nullptr) {
        goto allocationFailure;
    }
    if (gNetworkRuntimeState.tripleBuffer58B610 == nullptr) {
        goto allocationFailure;
    }
    return 0;

allocationFailure:
    std::free(gNetworkRuntimeState.buffer58B5F0);
    std::free(gNetworkRuntimeState.buffer58B5FC);
    std::free(gNetworkRuntimeState.buffer58B5F8);
    std::free(gNetworkRuntimeState.buffer58B5F4);
    std::free(gNetworkRuntimeState.pointerTable58B614);
    std::free(gNetworkRuntimeState.tripleBuffer58B604);
    std::free(gNetworkRuntimeState.tripleBuffer58B608);
    std::free(gNetworkRuntimeState.tripleBuffer58B60C);
    std::free(gNetworkRuntimeState.tripleBuffer58B610);
    return -1;
}

void CopyProtocolBytes_517080(
    std::uint8_t* const destination,
    const std::uint8_t* const source,
    const std::int32_t capacity)
{
    const std::uint32_t capacityBits =
        std::bit_cast<std::uint32_t>(capacity);
    const std::int32_t limit = std::bit_cast<std::int32_t>(
        capacityBits - 1U);

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

std::uint8_t* FormatProtocolInteger_517670(const std::int32_t value)
{
    static_cast<void>(
        detail::Sub_517730(value, gNetworkRuntimeState.integerFormatScratch, 64));
    return gNetworkRuntimeState.integerFormatScratch;
}

std::uint8_t* CopyProtocolBytesAndReturn_517690(
    std::uint8_t* const destination,
    const std::int32_t capacity,
    const std::uint8_t* const source)
{
    CopyProtocolBytes_517080(destination, source, capacity);
    return destination;
}

std::int32_t AppendProtocolText_5170D0(
    char* const destination,
    const char* const source,
    const std::int32_t capacity)
{
    if (capacity <= 1) {
        return capacity;
    }

    const std::int32_t limit = capacity - 1;
    std::int32_t destinationIndex = 0;
    while (destinationIndex < limit
           && destination[destinationIndex] != '\0') {
        ++destinationIndex;
    }
    if (destinationIndex == limit) {
        return capacity;
    }

    std::int32_t sourceIndex = 0;
    while (destinationIndex < limit && source[sourceIndex] != '\0') {
        destination[destinationIndex] = source[sourceIndex];
        ++destinationIndex;
        ++sourceIndex;
    }
    destination[destinationIndex] = '\0';
    return destinationIndex;
}

std::uint8_t* FormatProtocolIntegerToken_517120(const std::int32_t value)
{
    static constexpr std::uint8_t kSpace[] {' ', '\0'};

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const formatted = FormatProtocolInteger_517670(value);
    std::uint8_t* const firstDestination =
        gNetworkRuntimeState.buffer58B5FC;
    CopyProtocolBytes_517080(firstDestination, formatted, firstCapacity);

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondDestination =
        gNetworkRuntimeState.buffer58B5FC;
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(secondDestination),
        reinterpret_cast<const char*>(kSpace),
        secondCapacity));

    return gNetworkRuntimeState.buffer58B5FC;
}

std::uint8_t* FormatProtocolTextToken_517160(const char* const text)
{
    static constexpr char kSpace[] {' ', '\0'};

    std::uint8_t* const escaped = EscapeProtocolText_5171F0(text);
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(escaped), kSpace, capacity));
    return escaped;
}

std::int32_t ParseProtocolInteger_517190(const char* const text)
{
    if (text == nullptr) {
        return 0;
    }
    return detail::Sub_5176B0(text);
}

std::uint8_t* EscapeProtocolText_5171F0(const char* const text)
{
    gNetworkRuntimeState.buffer58B5F8[0] = 0;
    if (*text == '\0') {
        gNetworkRuntimeState.buffer58B5F8[0] = 0;
        return gNetworkRuntimeState.buffer58B5F8;
    }

    const auto* cursor = reinterpret_cast<const unsigned char*>(text);
    std::uint32_t destinationIndex = 0;
    for (;;) {
        const unsigned char currentByte = *cursor;
        if (currentByte >= 0x80U) {
            std::uint8_t* const firstDestination =
                gNetworkRuntimeState.buffer58B5F8;
            ++destinationIndex;
            ++cursor;
            firstDestination[destinationIndex - 1U] = currentByte;
            const unsigned char secondByte = *cursor;
            gNetworkRuntimeState.buffer58B5F8[destinationIndex] = secondByte;
        }
        else if (currentByte == static_cast<unsigned char>('\\')) {
            std::uint8_t* const firstDestination =
                gNetworkRuntimeState.buffer58B5F8;
            ++destinationIndex;
            firstDestination[destinationIndex - 1U] = '\\';
            gNetworkRuntimeState.buffer58B5F8[destinationIndex] = '\\';
        }
        else if (currentByte == static_cast<unsigned char>(' ')) {
            std::uint8_t* const firstDestination =
                gNetworkRuntimeState.buffer58B5F8;
            ++destinationIndex;
            firstDestination[destinationIndex - 1U] = '\\';
            gNetworkRuntimeState.buffer58B5F8[destinationIndex] = 'S';
        }
        else if (currentByte == static_cast<unsigned char>('\n')) {
            std::uint8_t* const firstDestination =
                gNetworkRuntimeState.buffer58B5F8;
            ++destinationIndex;
            firstDestination[destinationIndex - 1U] = '\\';
            gNetworkRuntimeState.buffer58B5F8[destinationIndex] = 'n';
        }
        else if (currentByte == static_cast<unsigned char>('\r')) {
            std::uint8_t* const firstDestination =
                gNetworkRuntimeState.buffer58B5F8;
            ++destinationIndex;
            firstDestination[destinationIndex - 1U] = '\\';
            gNetworkRuntimeState.buffer58B5F8[destinationIndex] = 'r';
        }
        else {
            gNetworkRuntimeState.buffer58B5F8[destinationIndex] = currentByte;
        }

        const unsigned char nextByte = cursor[1];
        ++destinationIndex;
        ++cursor;
        if (nextByte == 0U) {
            std::uint8_t* const finalDestination =
                gNetworkRuntimeState.buffer58B5F8;
            finalDestination[destinationIndex] = 0;
            return gNetworkRuntimeState.buffer58B5F8;
        }
    }
}

std::uint8_t* UnescapeProtocolText_5172D0(const char* const text)
{
    gNetworkRuntimeState.buffer58B5F8[0] = 0;
    if (*text == '\0') {
        gNetworkRuntimeState.buffer58B5F8[0] = 0;
        return gNetworkRuntimeState.buffer58B5F8;
    }

    const auto* cursor = reinterpret_cast<const unsigned char*>(text);
    std::uint32_t destinationIndex = 0;
    for (;;) {
        const unsigned char currentByte = *cursor;
        if (currentByte >= 0x80U) {
            std::uint8_t* const firstDestination =
                gNetworkRuntimeState.buffer58B5F8;
            ++destinationIndex;
            ++cursor;
            firstDestination[destinationIndex - 1U] = currentByte;
            const unsigned char secondByte = *cursor;
            gNetworkRuntimeState.buffer58B5F8[destinationIndex] = secondByte;
            ++destinationIndex;
        }
        else if (currentByte == static_cast<unsigned char>('\\')) {
            const unsigned char escapedByte = cursor[1];
            if (escapedByte == 0U) {
                gNetworkRuntimeState.buffer58B5F8[destinationIndex] = '\\';
                ++destinationIndex;
            }
            else if (escapedByte == static_cast<unsigned char>('S')) {
                std::uint8_t* const destination =
                    gNetworkRuntimeState.buffer58B5F8;
                ++destinationIndex;
                ++cursor;
                destination[destinationIndex - 1U] = ' ';
            }
            else if (escapedByte == static_cast<unsigned char>('n')) {
                std::uint8_t* const destination =
                    gNetworkRuntimeState.buffer58B5F8;
                ++destinationIndex;
                ++cursor;
                destination[destinationIndex - 1U] = '\n';
            }
            else if (escapedByte == static_cast<unsigned char>('r')) {
                std::uint8_t* const destination =
                    gNetworkRuntimeState.buffer58B5F8;
                ++destinationIndex;
                ++cursor;
                destination[destinationIndex - 1U] = '\r';
            }
            else {
                gNetworkRuntimeState.buffer58B5F8[destinationIndex] = '\\';
                ++destinationIndex;
                ++cursor;
            }
        }
        else {
            gNetworkRuntimeState.buffer58B5F8[destinationIndex] = currentByte;
            ++destinationIndex;
        }

        const unsigned char nextByte = cursor[1];
        ++cursor;
        if (nextByte == 0U) {
            gNetworkRuntimeState.buffer58B5F8[destinationIndex] = 0;
            return gNetworkRuntimeState.buffer58B5F8;
        }
    }
}

std::int32_t SendProtocolBuffer_517480(
    const std::uintptr_t socket,
    char* const buffer)
{
    if (gNetworkRuntimeState.sendLogPathDA3B58[0] != '\0') {
        std::FILE* const file = std::fopen(
            gNetworkRuntimeState.sendLogPathDA3B58,
            "a+");
        if (file != nullptr) {
            static_cast<void>(std::fprintf(file, "->%s\n", buffer));
            static_cast<void>(std::fclose(file));
        }
    }

    const SendProtocolObserver observer =
        gNetworkRuntimeState.sendObserver58B61C;
    if (observer != nullptr) {
        observer(1U, buffer);
    }

    std::uint32_t length =
        static_cast<std::uint32_t>(std::strlen(buffer));
    const std::uint32_t bufferLength =
        gNetworkRuntimeState.bufferLength;
    const std::uint32_t threshold = bufferLength * 3U;
    if (length < threshold) {
        buffer[length] = '\n';
        buffer[length + 1U] = '\0';
        ++length;
    }

    return gNetworkRuntimeState.callback(socket, buffer, length);
}

std::int32_t Report_512CA0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    const ProtocolOutputAccess output;
    static_cast<void>(BuildProtocolCommand_517510(
        output.data(), "PGXE"));
    const std::uint32_t bufferLength =
        gNetworkRuntimeState.bufferLength;
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        output.data(),
        reinterpret_cast<const char*>(token),
        std::bit_cast<std::int32_t>(bufferLength)));
    return SendProtocolBuffer_517480(socket, output.data());
}

std::int32_t Sub_512C10(
    const std::uintptr_t socket,
    const char* const text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Or"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512DF0,0x512E38)：容量仅在命令构造之后、整数格式化之前采样。
std::int32_t Sub_512DF0(
    const std::uintptr_t socket,
    const std::uint32_t tickBits)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "CCL"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(
        std::bit_cast<std::int32_t>(tickBits));
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512B00,0x512B8E)：每项均先读容量，格式化后重新读取输出缓冲。
std::int32_t Sub_512B00(
    const std::uintptr_t socket,
    const std::int32_t field4C,
    const std::int32_t field50,
    const std::int32_t field18)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "emRV"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(field4C);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(field50);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(field18);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512AB0,0x512AF8)：一次容量采样，输出缓冲在各调用点重新读取。
std::int32_t Sub_512AB0(
    const std::uintptr_t socket,
    const std::int32_t registNumber)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Godf"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(registNumber);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5128C0,0x512AAC)：每项独立读取容量，格式化后重读输出缓冲。
std::int32_t Sub_5128C0(
    const std::uintptr_t socket,
    const std::int32_t field1,
    const char* const field2,
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
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "aSEl"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(field1);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(field2);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::array<std::int32_t, 11> remainingIntegerFields {
        field3, field4, field5, field6, field7, field8,
        field9, field10, field11, field12, field13
    };
    for (const std::int32_t field : remainingIntegerFields) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            gNetworkRuntimeState.bufferLength);
        std::uint8_t* const token = FormatProtocolIntegerToken_517120(field);
        static_cast<void>(AppendProtocolText_5170D0(
            reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
            reinterpret_cast<const char*>(token),
            capacity));
    }

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

std::int32_t AllocateNetworkWorkspace_516B60(
    const NetworkCallback callback,
    const std::uint32_t bufferLength,
    const std::uintptr_t)
{
    gNetworkRuntimeState.callback = &DefaultNetworkCallback;
    if (callback != nullptr) {
        gNetworkRuntimeState.callback = callback;
    }

    static_cast<void>(AllocateProtocolBuffers_516DA0(bufferLength));

    gNetworkRuntimeState.workspaceSlots = static_cast<void**>(
        std::malloc(kWorkspaceSlotCount * sizeof(void*)));
    if (gNetworkRuntimeState.workspaceSlots == nullptr) {
        return -1;
    }

    std::fill_n(
        gNetworkRuntimeState.workspaceSlots,
        kWorkspaceSlotCount,
        nullptr);
    for (std::size_t index = 0; index < kWorkspaceSlotCount; ++index) {
        gNetworkRuntimeState.workspaceSlots[index] =
            std::malloc(static_cast<std::size_t>(bufferLength));
        if (gNetworkRuntimeState.workspaceSlots[index] == nullptr) {
            std::free(gNetworkRuntimeState.workspaceSlots[0]);
            return -1;
        }
    }
    return 0;
}

std::int32_t InitializeNetworkRuntime_508140()
{
    WSADATA data;
    if (::WSAStartup(MAKEWORD(1, 1), &data) != 0) {
        return 0;
    }

    static_cast<void>(AllocateNetworkWorkspace_516B60(
        &detail::Sub_403E50,
        0x10000U,
        platform::gConnectionSocket));
    gNetworkRuntimeState.active = 1;
    world::gValueD9F2CA = 0;
    return 1;
}

std::uint8_t ReadNetworkRuntimeActive() noexcept
{
    return gNetworkRuntimeState.active;
}

std::uint8_t ReadProtocolByte593850() noexcept
{
    return gNetworkRuntimeState.protocolByte593850;
}

void WriteValueD9F2D0(const std::uint32_t value) noexcept
{
    gNetworkRuntimeState.valueD9F2D0 = value;
}

std::uint16_t ReadProtocolModeDA3B22() noexcept
{
    return gNetworkRuntimeState.protocolModeDA3B22;
}

void WriteProtocolModeDA3B22(const std::uint16_t value) noexcept
{
    gNetworkRuntimeState.protocolModeDA3B22 = value;
}

void ShutdownNetworkRuntime_5081A0()
{
    if (gNetworkRuntimeState.active == 0) {
        return;
    }

    gNetworkRuntimeState.valueD9F2D0 = 0;
    gNetworkRuntimeState.active = 0;
    login::gRequestPollingEnabled = 0;
    world::gValueD9F2CA = 0;
    static_cast<void>(::closesocket(
        static_cast<SOCKET>(platform::gConnectionSocket)));
    static_cast<void>(::WSACleanup());
    static_cast<void>(ResetProtocolState_4037D0());

    std::free(gNetworkRuntimeState.buffer58B5F0);
    std::free(gNetworkRuntimeState.buffer58B5F4);
    std::free(gNetworkRuntimeState.buffer58B5F8);
    std::free(gNetworkRuntimeState.buffer58B5FC);
    std::free(gNetworkRuntimeState.pointerTable58B614);
    std::free(gNetworkRuntimeState.tripleBuffer58B604);
    std::free(gNetworkRuntimeState.tripleBuffer58B608);
    std::free(gNetworkRuntimeState.tripleBuffer58B60C);
    std::free(gNetworkRuntimeState.tripleBuffer58B610);
    for (std::size_t index = 0; index < kWorkspaceSlotCount; ++index) {
        std::free(gNetworkRuntimeState.workspaceSlots[index]);
    }
    std::free(gNetworkRuntimeState.workspaceSlots);
}

} // namespace cg::network
