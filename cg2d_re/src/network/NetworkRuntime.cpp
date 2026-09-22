#include "cg/network/NetworkRuntime.hpp"

#include "NetworkRuntimeDependencies.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"

#include <winsock2.h>

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>

namespace cg::network {
namespace {

constexpr std::size_t kWorkspaceSlotCount = 18;

using SendProtocolObserver = void (*)(
    std::uint32_t direction,
    char* buffer);

std::int32_t DefaultNetworkCallback(
    std::uint32_t,
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

    // 原 0x593FCC、0x593FCE、0x593FC4、0x593FC8，均为 W16。
    std::uint16_t protocolHalfWord0;
    std::uint16_t protocolHalfWord1;
    std::uint16_t protocolHalfWord2;
    std::uint16_t protocolHalfWord3;

    // 原 0x593FC6、0x593FCA，均为 W8。
    std::uint8_t protocolByte0;
    std::uint8_t protocolByte1;

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

} // namespace

namespace detail {

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

} // namespace detail

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

    gNetworkRuntimeState.protocolHalfWord0 = 0;
    gNetworkRuntimeState.protocolHalfWord1 = 0;
    gNetworkRuntimeState.protocolHalfWord2 = 0;
    gNetworkRuntimeState.protocolHalfWord3 = 0;
    gNetworkRuntimeState.protocolByte0 = 0;
    gNetworkRuntimeState.protocolByte1 = 0;
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
    const std::uint32_t socketValue,
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

    return gNetworkRuntimeState.callback(socketValue, buffer, length);
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
