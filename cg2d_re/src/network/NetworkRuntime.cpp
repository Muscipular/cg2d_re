#include "cg/network/NetworkRuntime.hpp"

#include "../platform/LegacyFormat.hpp"
#include "../platform/LegacyStdio.hpp"

#include "../CharacterSelectSceneDependencies.hpp"
#include "../LoginRequestDependencies.hpp"
#include "../LoginStateDependencies.hpp"
#include "../world/MailState.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "NetworkRuntimeDependencies.hpp"
#include "NetworkDispatchDependencies.hpp"

#include "cg/CharacterCreationRequest.hpp"
#include "cg/CharacterDeletionRequest.hpp"
#include "cg/CharacterListRequest.hpp"
#include "cg/Input.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/LegacyRecordOrder.hpp"
#include "cg/LoginConnection.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/LoginScene.hpp"
#include "cg/Palette.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/world/SceneState.hpp"

#include <winsock2.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <cstdio> // 原格式化／扫描接口：0x0053B304、0x0053BC0F；直接沿MSVC CRT。

namespace cg::network {
namespace {

constexpr std::size_t kWorkspaceSlotCount = 18;
constexpr std::size_t kReceiveQueueOffset = 0xC004;
constexpr std::size_t kQueueCapacity = 0x4000;
constexpr std::size_t kSendLengthOffset = kReceiveQueueOffset + kQueueCapacity;
constexpr std::size_t kQueueStorageSize = kSendLengthOffset + sizeof(std::int32_t);
constexpr std::size_t kDispatchOffset = 0x4000;
constexpr std::size_t kDispatchSize = 0x8000;
static_assert(kReceiveQueueOffset == 0xD9B2BC - 0xD8F2B8);
static_assert(kSendLengthOffset == 0xD9F2BC - 0xD8F2B8);
static_assert(kQueueStorageSize == 0x10008);
static_assert(kDispatchOffset == 0xD932B8 - 0xD8F2B8);
static_assert(kDispatchOffset + kDispatchSize + 4 == kReceiveQueueOffset);

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
    std::uint32_t protocolWord0; // 原全局：0x00593FB0。
    std::uint32_t protocolWord1; // 原全局：0x00593FB4。
    std::uint32_t protocolWord2; // 原全局：0x00593FB8。
    std::uint32_t protocolWord3; // 原全局：0x00593FBC。
    std::uint32_t protocolWord4; // 原全局：0x00593FA0。
    std::uint32_t protocolWord5; // 原全局：0x00593FA4。
    std::uint32_t protocolWord6; // 原全局：0x00593FA8。
    std::uint32_t protocolWord7; // 原全局：0x00593FAC。

    // 原 [0x593FC4,0x593FD0) 连续协议短状态。
    std::array<std::uint8_t, 12> protocolState593FC4; // 原全局：0x00593FC4。
    std::uint16_t protocolModeDA3B22; // 原全局：0x00DA3B22。
    std::int32_t time62B394; // 原全局：0x0062B394。 原 __time32_t，独立于 0xDA3B28 回复码。
    platform::LegacyTm53B8C6* localTime62B380; // 原全局：0x0062B380。 原 W32 指针槽，借用旧时间层的共享九槽对象。
    NetworkCallback callback; // 原 0x58B5E8。
    std::uint32_t bufferLength;
    std::uint8_t* buffer58B5F0; // 原全局：0x0058B5F0。
    std::uint8_t* buffer58B5F4; // 原全局：0x0058B5F4。
    std::uint8_t* buffer58B5F8; // 原全局：0x0058B5F8。
    std::uint8_t* buffer58B5FC; // 原全局：0x0058B5FC。
    char** pointerTable58B614; // 原全局：0x0058B614。
    std::uint8_t* tripleBuffer58B604; // 原全局：0x0058B604。
    std::uint8_t* tripleBuffer58B608; // 原全局：0x0058B608。
    std::uint8_t* tripleBuffer58B60C; // 原全局：0x0058B60C。
    std::uint8_t* tripleBuffer58B610; // 原全局：0x0058B610。
    SendProtocolObserver sendObserver58B61C; // 原全局：0x0058B61C。
    char sendLogPathDA3B58[0x400]; // 原全局：0x00DA3B58。
    std::string receiveLogPathDA3F9C; // 原全局：0x00DA3F9C。 原字节路径；固定容量未证，不据相邻地址猜测。
    std::uint8_t integerFormatScratch[64]; // 原 [0xDA3F58,0xDA3F98)。
    std::uint8_t** workspaceSlots; // 原 0xDA3F98，18 个同SDK域的字节缓冲指针。
    std::uint8_t active;   // 原 0xD9F2C8，网络运行期活动门禁。
    std::uint32_t valueD9F2D0; // 原 0xD9F2D0，首标记优先状态；当前明文无置1生产者。
    // 原 [0xD8F2B8,0xD9F2C0)：发送窗口、接收区与发送长度共享地址别名。
    std::array<std::uint8_t, kQueueStorageSize> queueBytes; // 原全局：0x00D8F2B8。
    std::int32_t receiveLengthD69790; // 原 0xD69790，独立于连续队列。
    std::uint8_t flagsD9F2C0; // 原全局：0x00D9F2C0。
    std::uint32_t lastTickD9F2C4; // 原全局：0x00D9F2C4。
    std::int32_t errorD9F2CC; // 原全局：0x00D9F2CC。
    std::uint32_t receiveResetD9F3C4; // 原全局：0x00D9F3C4。
    std::uint32_t frameModuloD9F3C8; // 原全局：0x00D9F3C8。
    std::uint8_t value58ADA4 = 0x80; // 原全局：0x0058ADA4。 原ed回复独立字节，不属于58ADA8的排除链文本。
    std::array<char, 0x104> addressTextDA33D8; // 原全局：0x00DA33D8。 原lSZ回复缓存，非空时按strstr判断变化。
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

struct ProtocolOutputAccess {
    char* data() const noexcept
    {
        return reinterpret_cast<char*>(
            gNetworkRuntimeState.buffer58B5F0);
    }
};

// 现有cgmsv明文合同：原命令/字段和LF原样入唯一发送队列，不包密文帧。
// 关联原入口：0x004589A0／0x004589B0／0x004589C0，内联承接窗口报告采样与清零。
std::int32_t QueuePlaintextProtocolBytes(
    const std::uintptr_t,
    const void* const data,
    const std::uint32_t size)
{
    if (login::gRequestPollingEnabled == 0) {
        return -100;
    }

    std::int32_t sendLength;
    std::uint8_t* const send = gNetworkRuntimeState.queueBytes.data();
    std::memcpy(&sendLength, send + kSendLengthOffset, sizeof(sendLength));
    const std::int32_t nextLength = sendLength + static_cast<std::int32_t>(size);
    if (nextLength > static_cast<std::int32_t>(kQueueCapacity)) {
        return -1;
    }

    if (gNetworkRuntimeState.valueD9F2D0 == 1) {
        gNetworkRuntimeState.valueD9F2D0 = 0;
    } else if (platform::gWindowReportValue > 0
        && login::ReadRequestOutcomeFlag() == 1
        && std::bit_cast<std::int32_t>(platform::gWindowReportDeadline) < 0) {
        platform::gWindowReportValue = 0;
    }

    std::memcpy(send + sendLength, data, size);
    std::memcpy(send + kSendLengthOffset, &nextLength, sizeof(nextLength));
    return 0;
}

// 对齐服务端GetOneLine_fix；半行保留，多行由NetworkLoop依序消费。
std::int32_t ReceivePlaintextProtocolLine(
    void* const destination,
    const std::int32_t capacity)
{
    if (login::gRequestPollingEnabled == 0) {
        return -100;
    }

    std::uint8_t* const receive =
        gNetworkRuntimeState.queueBytes.data() + kReceiveQueueOffset;
    const auto* const newline = static_cast<const std::uint8_t*>(std::memchr(
        receive, '\n', static_cast<std::size_t>(gNetworkRuntimeState.receiveLengthD69790)));
    if (newline == nullptr) {
        return -1;
    }
    const std::int32_t lineLength = static_cast<std::int32_t>(newline - receive) + 1;
    if (capacity <= lineLength) {
        ::OutputDebugStringA("\nBuffer Too Small\n");
        return -1;
    }

    std::memcpy(destination, receive, static_cast<std::size_t>(lineLength));
    static_cast<char*>(destination)[lineLength] = '\0';
    static_cast<void>(ConsumeReceiveQueue_508230(lineLength));
    return 0;
}

} // namespace

namespace detail {

// 原[403C50,403E4D)旧W16帧取包，仅保留历史对照；当前明文收发不调用此函数。
std::int32_t Sub_403C50(
    void* const destination,
    const std::int32_t capacity)
{
    std::array<std::uint8_t, 0x4004> plaintext;
    if (login::gRequestPollingEnabled == 0) {
        return -100;
    }

    std::uint8_t* const receive =
        gNetworkRuntimeState.queueBytes.data() + kReceiveQueueOffset;
    if (std::strcmp(
            reinterpret_cast<const char*>(receive), "JPCGEXCEPTION01") == 0) {
        std::memset(receive, 0, kQueueCapacity);
        login::gValue593FC0 = 1;
        return -1;
    }

    std::uint16_t plaintextLength;
    std::memcpy(&plaintextLength, receive, sizeof(plaintextLength));
    const std::uint32_t frameLength =
        static_cast<std::uint32_t>(plaintextLength) + 2U;
    // 403CF8/403CFE是cmp/jb；整包等待不按S32比较。
    if (static_cast<std::uint32_t>(gNetworkRuntimeState.receiveLengthD69790)
        < frameLength) {
        return -1;
    }

    // 保持原0x4004B独立输出，508230移动队列后仍以原明文生产状态。
    std::memcpy(plaintext.data(), receive + 2, plaintextLength);
    if (capacity <= static_cast<std::int32_t>(plaintextLength)) {
        ::OutputDebugStringA("\nBuffer Too Small\n");
        return -1;
    }

    auto* const output = static_cast<std::uint8_t*>(destination);
    std::memcpy(output, plaintext.data(), plaintextLength);
    if (*(output + static_cast<std::int32_t>(plaintextLength) - 1) == 13
        && gNetworkRuntimeState.valueD9F2D0 == 0) {
        *(output + static_cast<std::int32_t>(plaintextLength) - 1) = 10;
        gNetworkRuntimeState.valueD9F2D0 = 1;
    }
    output[plaintextLength] = 0;
    static_cast<void>(ConsumeReceiveQueue_508230(
        static_cast<std::int32_t>(frameLength)));

    if (plaintextLength <= 16U) {
        // 非拥有槽指针仅表达原连续W32前缀，不跨单一C++成员复制。
        const std::array<std::uint32_t*, 4> words {
            &gNetworkRuntimeState.protocolWord0,
            &gNetworkRuntimeState.protocolWord1,
            &gNetworkRuntimeState.protocolWord2,
            &gNetworkRuntimeState.protocolWord3
        };
        const std::uint32_t wholeWords = plaintextLength / 4U;
        for (std::uint32_t index = 0; index < wholeWords; ++index) {
            std::memcpy(words[index], plaintext.data() + index * 4U, 4);
        }
        const std::int32_t remainingLength =
            gNetworkRuntimeState.receiveLengthD69790;
        for (std::uint32_t index = wholeWords * 4U;
             index < plaintextLength; ++index) {
            reinterpret_cast<std::uint8_t*>(words[index / 4U])[index % 4U] =
                plaintext[index];
        }
        receive[remainingLength] = 0;
    } else {
        // 403DB5..403DD8：原大包四槽与队列尾NUL的实际写序。
        std::memcpy(
            &gNetworkRuntimeState.protocolWord2, plaintext.data() + 8, 4);
        const std::int32_t remainingLength =
            gNetworkRuntimeState.receiveLengthD69790;
        std::memcpy(
            &gNetworkRuntimeState.protocolWord0, plaintext.data(), 4);
        receive[remainingLength] = 0;
        std::memcpy(
            &gNetworkRuntimeState.protocolWord1, plaintext.data() + 4, 4);
        std::memcpy(
            &gNetworkRuntimeState.protocolWord3, plaintext.data() + 12, 4);
    }
    return 0;
}

// 原[403E50,40405A)：原首标记/状态尾/帧与发送窗口；socket未读取。
// 历史帧合同见doc/cg2d_network_plaintext_frame_spec.md；当前明文收发不调用此函数。
std::int32_t Sub_403E50(
    const std::uintptr_t,
    const void* const data,
    const std::uint32_t size)
{
    std::array<std::uint8_t, 0x4004> plaintext;
    std::array<std::uint8_t, 0x4002> frame;
    if (login::gRequestPollingEnabled == 0) {
        return -100;
    }

    std::int32_t sendLength;
    std::memcpy(
        &sendLength,
        gNetworkRuntimeState.queueBytes.data() + kSendLengthOffset,
        sizeof(sendLength));
    const std::int32_t checkedLength =
        sendLength + static_cast<std::int32_t>(size) + 1;
    if (checkedLength > static_cast<std::int32_t>(kQueueCapacity)) {
        return -1;
    }

    if (gNetworkRuntimeState.valueD9F2D0 == 1) {
        plaintext[0] = 0x13;
        gNetworkRuntimeState.valueD9F2D0 = 0;
    } else if (platform::gWindowReportValue > 0
        && login::ReadRequestOutcomeFlag() == 1
        && std::bit_cast<std::int32_t>(platform::gWindowReportDeadline) < 0) {
        plaintext[0] = 0xFC;
        platform::gWindowReportValue = 0;
    } else {
        plaintext[0] = static_cast<std::uint8_t>(
            platform::RandomRange_46FAF0(20, 250));
    }

    std::memcpy(plaintext.data() + 1, data, size);
    const std::uint16_t oldTailLength = ReadProtocolStateW16(10);
    std::memcpy(
        plaintext.data() + size + 1U,
        gNetworkRuntimeState.protocolState593FC4.data() + 4,
        oldTailLength);
    WriteProtocolStateW16(4, 0);
    gNetworkRuntimeState.protocolState593FC4[6] = 0;
    static_cast<void>(std::sprintf(
        reinterpret_cast<char*>(
            gNetworkRuntimeState.protocolState593FC4.data() + 4),
        "%s",
        reinterpret_cast<const char*>(
            gNetworkRuntimeState.protocolState593FC4.data())));

    // 403F61/403F6D：格式后重读尾长，原算法首参是完整W32表达式。
    const std::uint32_t plaintextLength =
        static_cast<std::uint32_t>(ReadProtocolStateW16(10)) + size + 1U;
    const std::uint16_t transmittedLength =
        static_cast<std::uint16_t>(plaintextLength);
    std::memcpy(frame.data(), &transmittedLength, sizeof(transmittedLength));
    const std::uint32_t bodyWordBytes = transmittedLength & ~3U;
    std::memcpy(frame.data() + 2, plaintext.data(), bodyWordBytes);
    std::memcpy(
        &sendLength,
        gNetworkRuntimeState.queueBytes.data() + kSendLengthOffset,
        sizeof(sendLength));
    std::memcpy(
        frame.data() + 2U + bodyWordBytes,
        plaintext.data() + bodyWordBytes,
        transmittedLength & 3U);
    const std::uint32_t frameLength =
        static_cast<std::uint32_t>(transmittedLength) + 2U;
    std::memcpy(
        gNetworkRuntimeState.queueBytes.data() + sendLength,
        frame.data(),
        frameLength & ~3U);
    const std::int32_t nextLength =
        sendLength + static_cast<std::int32_t>(frameLength);
    std::memcpy(
        gNetworkRuntimeState.queueBytes.data() + kSendLengthOffset,
        &nextLength,
        sizeof(nextLength));
    // 403FCE/403FD9/403FDE：DWORD前缀入队后发布长度，再写尾byte。
    const std::uint32_t frameWordBytes = frameLength & ~3U;
    std::memcpy(
        gNetworkRuntimeState.queueBytes.data() + sendLength + frameWordBytes,
        frame.data() + frameWordBytes,
        frameLength & 3U);

    // 403FE3/403FEA独立后读，状态长度不以W16帧头或旧尾快照代替。
    const std::uint32_t stateLength =
        static_cast<std::uint32_t>(ReadProtocolStateW16(10)) + size + 1U;
    if (stateLength <= 16U) {
        const std::array<std::uint32_t*, 4> words {
            &gNetworkRuntimeState.protocolWord4,
            &gNetworkRuntimeState.protocolWord5,
            &gNetworkRuntimeState.protocolWord6,
            &gNetworkRuntimeState.protocolWord7
        };
        const std::uint32_t wholeWords = stateLength / 4U;
        for (std::uint32_t index = 0; index < wholeWords; ++index) {
            std::memcpy(words[index], plaintext.data() + index * 4U, 4);
        }
        for (std::uint32_t index = wholeWords * 4U;
             index < stateLength; ++index) {
            reinterpret_cast<std::uint8_t*>(words[index / 4U])[index % 4U] =
                plaintext[index];
        }
    } else {
        std::memcpy(
            &gNetworkRuntimeState.protocolWord4, plaintext.data(), 4);
        std::memcpy(
            &gNetworkRuntimeState.protocolWord5, plaintext.data() + 4, 4);
        std::memcpy(
            &gNetworkRuntimeState.protocolWord6, plaintext.data() + 8, 4);
        std::memcpy(
            &gNetworkRuntimeState.protocolWord7, plaintext.data() + 12, 4);
    }
    const std::uint16_t currentTailLength = ReadProtocolStateW16(8);
    WriteProtocolStateW16(10, currentTailLength);
    return 0;
}

// 原函数：0x004038A0。
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

// 原函数：0x00403A70。
void Sub_403A70(
    char* const text, const std::int32_t,
    const char* const key3, const std::int32_t, const char* const key5)
{
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
    for (std::int32_t index = 0; index < count; ++index) {
        const std::int32_t key5Byte = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(key5[(index + 84) % length5]));
        const std::int32_t b = (key5Byte & 0x0CAB) + index;
        const std::int32_t key3Byte = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(key3[(index + 56) % length3]));
        const std::int32_t a = (key3Byte & 0xBBCC) + index;
        std::int32_t target;
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
        const char direction = static_cast<char>(45 - (a & 2));
        std::int32_t value = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(text[index]));
        if (direction == '+') {
            value += delta;
            const std::uint32_t unsignedValue = static_cast<std::uint32_t>(value);
            if (unsignedValue > 122U || (unsignedValue > 90U && unsignedValue < 97U)) {
                value -= 26;
            }
        } else {
            value -= delta;
            const std::uint32_t unsignedValue = static_cast<std::uint32_t>(value);
            if (unsignedValue < 65U || (unsignedValue > 90U && unsignedValue < 97U)) {
                value += 26;
            }
        }
        text[index] = std::bit_cast<char>(static_cast<std::uint8_t>(value));
    }
}

// 原函数：0x005173B0。
void SplitReceivedBuffer_5173B0(char* const buffer)
{
    const SendProtocolObserver observer = gNetworkRuntimeState.sendObserver58B61C;
    if (observer != nullptr) {
        observer(0U, buffer);
    }
    if (gNetworkRuntimeState.receiveLogPathDA3F9C.c_str()[0] != '\0') {
        platform::LegacyFile* const file = platform::OpenLegacyStream_53B2F1(
            gNetworkRuntimeState.receiveLogPathDA3F9C.c_str(), "a+");
        if (file != nullptr) {
            static_cast<void>(std::fprintf(file, "%s\n", buffer));
            static_cast<void>(platform::CloseLegacyStream_53B1D0(file));
        }
    }
    std::int32_t count = 0;
    if (buffer[0] != '\0') {
        std::int32_t index = 0;
        do {
            if (index == 0) {
                char** const table = gNetworkRuntimeState.pointerTable58B614;
                ++count;
                table[count - 1] = buffer;
            }
            if (buffer[index] == ' ') {
                gNetworkRuntimeState.pointerTable58B614[count] = buffer + index + 1;
                ++count;
            }
            ++index;
        } while (buffer[index] != '\0');
    }
    char* cursor = buffer;
    if (*cursor != '\0') {
        do {
            if (*cursor == ' ' || *cursor == '\n' || *cursor == '\r') {
                *cursor = '\0';
            }
            ++cursor;
        } while (*cursor != '\0');
    }
    gNetworkRuntimeState.pointerTable58B614[count] = nullptr;
}

// 原函数：0x005176B0。
std::int32_t Sub_5176B0(const char* const text)
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

// 原函数：0x00508310。
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
    const std::uintptr_t,
    const std::int32_t index,
    const std::int32_t sourceX,
    const std::int32_t sourceY)
{
    return audio::Sub_52CDE0(index, sourceX, sourceY);
}

// 原 [0x50CF60,0x50CF6A)：直接发布完整第二参数。
std::int32_t Sub_50CF60(
    const std::uintptr_t,
    const std::int32_t value)
{
    world::detail::gValueD2F788 = value;
    return value;
}

// 原 [0x50CF20,0x50CF55)：仅第二参数控制固定提示入队。
void Sub_50CF20(
    const std::uintptr_t,
    const std::int32_t status,
    const std::int32_t,
    const std::int32_t,
    const std::int32_t)
{
    if (status != 0) {
        return;
    }

    constexpr char kMessage[] =
        "\xCA\xA7\x94\xA1\xC1\xCB\xA3\xA1";
    static_assert(sizeof(kMessage) == 9);
    char buffer[256];
    std::sprintf(buffer, kMessage);
    world::detail::Sub_44B050(buffer, 4, 3);
}

// 原 [0x50D3B0,0x50D3BC)：只发布低 W16，不借用邻接槽。
std::uint16_t Sub_50D3B0(
    const std::uintptr_t,
    const std::int32_t value)
{
    const std::uint16_t bits = static_cast<std::uint16_t>(value);
    world::detail::gValueFF0D34 = bits;
    return bits;
}

// 原 [0x50E310,0x50E33B)：先读门禁，只有合法范围才依序发布两槽。
std::int32_t Sub_50E310(
    const std::uintptr_t,
    const std::int32_t value)
{
    const std::uint32_t gate = world::gValue9A6E0C;
    if (gate != 0U) {
        return std::bit_cast<std::int32_t>(gate);
    }
    if (value >= 200 && value <= 249) {
        world::gValueFF0D5C = static_cast<std::uint32_t>(value);
        world::gValueFF0D58 = 1;
    }
    return value;
}

// 原 [0x50E630,0x50E64C)：唯一 caller 不消费未定义的早退 EAX。
void Sub_50E630(
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
std::int32_t Sub_50D700(
    const std::uintptr_t,
    const char* const payload)
{
    const std::uint32_t gate = world::gValue9A6E0C;
    if (gate != 0U) {
        return std::bit_cast<std::int32_t>(gate);
    }

    world::detail::gValueFF0C10 = 2;
    world::detail::gValueFF0D04 =
        ParseDelimitedBase62_52D0D0(payload, '|', 1);
    const std::int32_t second =
        ParseDelimitedBase62_52D0D0(payload, '|', 2);
    world::detail::gValueFF0D08 = second;
    return second;
}

// LDT01：外层图号先转换；有记录先生产12字段再检查mode，无记录先检查mode。
// 原函数：0x0050DD80。
void Sub_50DD80(
    const std::uintptr_t, const std::int32_t objectIndex,
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t direction,
    const std::int32_t withRecord, const std::int32_t mode,
    const char* const payload)
{
    if (world::gValue9A6E0C != 0) {
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    const std::int32_t convertedImageNo =
        character_select::detail::Sub_51B740(imageNo);
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
        static_cast<void>(login::detail::Sub_448040(
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
    const std::int32_t recordX = platform::Atoi_53C649(field);
    ExtractDelimitedField_52CED0(payload, '|', 4, 2047, field);
    const std::int32_t recordY = platform::Atoi_53C649(field);
    ExtractDelimitedField_52CED0(payload, '|', 5, 2047, field);
    const std::int32_t recordDirection = platform::Atoi_53C649(field);
    ExtractDelimitedField_52CED0(payload, '|', 6, 2047, field);
    const std::int32_t recordImageNo =
        character_select::detail::Sub_51B740(platform::Atoi_53C649(field));
    ExtractDelimitedField_52CED0(payload, '|', 7, 2047, field);
    const std::int32_t level = platform::Atoi_53C649(field);
    const std::int32_t value40 = ParseDelimitedInteger_52CFA0(payload, '|', 8);
    ExtractDelimitedField_52CED0(payload, '|', 9, 2047, name);
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    ExtractDelimitedField_52CED0(payload, '|', 10, 2047, titles);
    input::UnescapeLegacyTextAndNotify_52D2F0(titles);
    ExtractDelimitedField_52CED0(payload, '|', 11, 2047, field);
    const std::int32_t flag1 = platform::Atoi_53C649(field);
    ExtractDelimitedField_52CED0(payload, '|', 12, 2047, field);
    const std::int32_t flag2 = platform::Atoi_53C649(field);
    if (world::detail::Sub_445A60(
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
    static_cast<void>(login::detail::Sub_448040(
        convertedImageNo, recordX, recordY, recordDirection,
        actionMode, 0, objectIndex));
}

// 原函数：0x0050E6E0。
void Sub_50E6E0(const std::uintptr_t, const char* const text)
{
    char* const previous = gNetworkRuntimeState.addressTextDA33D8.data();
    if (previous[0] != '\0') {
        if (platform::StrStr_53BE40(previous, text) != nullptr) {
            return;
        }
        constexpr char format58B0B0[] =
            "IP " "\xCE\xBB\xD6\xB7\xD3\xC9" "[%s]"
            "\xD7\x83\xB3\xC9" "[%s]" "\xA1\xA3";
        constexpr char warning58B094[] =
            "\xBE\x57\xC2\xB7\xD3\xD0\xBF\xC9\xC4\xDC\xCA\xC7"
            "\xB1\xBB\xD4\xD9\xDF\x42\xBD\xD3\xC1\xCB\xA1\xA3";
        static_assert(sizeof(format58B0B0) == 24);
        static_assert(sizeof(warning58B094) == 25);
        char message[256];
        std::sprintf(message, format58B0B0, previous, text);
        world::detail::Sub_44B050(message, 4, 3);
        world::detail::Sub_44B050(warning58B094, 4, 3);
    }
    std::strcpy(previous, text);
}

// 原函数：0x0050E780。
std::int32_t Sub_50E780(const std::uintptr_t, const std::int32_t value)
{
    const std::int32_t sceneType = world::gCurrentSceneType;
    const auto lowByte = static_cast<std::uint8_t>(value);
    gNetworkRuntimeState.value58ADA4 = lowByte;
    if (sceneType != 13) {
        return value;
    }
    const std::int32_t state = lowByte != 0 ? 200 : 100;
    world::gCurrentSceneState = state;
    return state;
}

// 原函数：0x0050E830。
std::int32_t Sub_50E830(const std::uintptr_t)
{
    const std::uint32_t state = world::gValue9A6E0C;
    if (state != 0) {
        return std::bit_cast<std::int32_t>(state);
    }
    character_select::detail::Sub_5194C0();
    return character_select::detail::Sub_519550();
}

// GMR01：原gate位型早退/记录五域重置→活动计数发布→同key邮件删除，原完整S32返回保。
// 原函数：0x0050F130。
std::int32_t Sub_50F130(const std::uintptr_t, const std::int32_t key)
{
    const std::uint32_t gate = world::gValue9A6E0C;
    if (gate != 0) {
        return std::bit_cast<std::int32_t>(gate);
    }
    const std::int32_t index = login::detail::FindActiveOrderedRecordIndex_5195C0(key);
    auto* const record = login::detail::gOrderedRecords1051B14.data() + index;
    record->value00 = -1;
    record->value08 = 0;
    record->text18[0] = '\0';
    record->text18[0x11] = '\0';
    record->value0C = -1;
    login::detail::gOrderedRecordCount1051510 =
        login::detail::CountRecordsValue08EqualOne_50EF50();
    return world::detail::Sub_5194F0(key);
}

// GMR01：原字段/反转义→持有排序index→生产→外部记录名通知→存档/SE63/消息，四参未读边界保。
// 原函数：0x0050F190。
void Sub_50F190(
    const std::uintptr_t, const std::int32_t key,
    const char* const payload, const std::int32_t)
{
    constexpr char kGuildMailFormat58B0EC[] =
        "\xC4\xFA\xCA\xD5\xB5\xBD\x81\xED\xD7\xD4\xBC\xD2\xD7\xE5"
        " %s " "\xB5\xC4\xD0\xC5\xBC\xFE\xA1\xAB";
    constexpr char kGuildPetMailFormat58B0C8[] =
        "\xC4\xFA\xCA\xD5\xB5\xBD\x81\xED\xD7\xD4\xBC\xD2\xD7\xE5"
        "\xB3\xC9\x86\x54" " %s "
        "\xB5\xC4\x8C\x99\xCE\xEF\xD0\xC5\xBC\xFE\xA1\xAB";

    char name[32];
    char message[256];
    char text[300];
    static_cast<void>(ExtractDelimitedField_52CED0(payload, '|', 1, 31, name));
    static_cast<void>(ExtractDelimitedField_52CED0(payload, '|', 2, 299, text));
    const std::int32_t kind = ParseDelimitedInteger_52CFA0(payload, '|', 3);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    const std::int32_t index = login::detail::FindActiveOrderedRecordIndex_5195C0(key);
    static_cast<void>(world::detail::Sub_519370(key, 0, name, text));
    const char* const recordName =
        (login::detail::gOrderedRecords1051B14.data() + index)->text18;
    std::sprintf(
        message, kind == -1 ? kGuildMailFormat58B0EC : kGuildPetMailFormat58B0C8,
        recordName);
    static_cast<void>(world::detail::Sub_519550());
    static_cast<void>(audio::PlaySe_52C450(63, 320));
    world::detail::Sub_44B050(message, 4, 3);
}

// 原函数：0x00510460。
void Sub_510460(
    const std::uintptr_t, const std::int32_t firstGroup, const std::int32_t firstIndex,
    const std::int32_t secondGroup, const std::int32_t secondIndex)
{
    if (world::gValue9A6E0C != 0) {
        return;
    }
    world::detail::Sub_51B630(firstGroup, firstIndex, secondGroup, secondIndex);
}

// IER01：原78条；首字段失败只used0，成功两文本/单空格及24属性按原序写唯一记录；当前TgX明文分发保持，运行待。
// 原函数：0x00510490。
void Sub_510490(const std::uintptr_t, const char* const text)
{
    auto& record = world::detail::gReplyRecordF7C950;
    if (ExtractDelimitedField_52CED0(text, ',', 1, 31, record.text04.data()) != 0) {
        record.used = 0;
        return;
    }
    record.used = 1;
    input::UnescapeLegacyTextAndNotify_52D2F0(record.text04.data());
    static_cast<void>(ExtractDelimitedField_52CED0(
        text, ',', 2, 31, record.text25.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(record.text25.data());
    record.hasText24 = static_cast<std::uint8_t>(
        std::strcmp(record.text25.data(), " ") != 0);
    record.value48 = ParseDelimitedInteger_52CFA0(text, ',', 3);
    for (std::int32_t index = 0; index < 24; ++index) {
        record.values4C[index] = ParseDelimitedInteger_52CFA0(text, ',', index + 4);
    }
}

} // namespace detail

namespace {

// 原函数：0x00508800。
void HandleResponse_508800(
    const std::uintptr_t,
    const std::int32_t value,
    const char* const text)
{
    if (login::gRequestStatus != 1) {
        static_cast<void>(login::detail::Sub_44B5C0("LOGIN REPLY IGNORED"));
        return;
    }

    login::gRequestStatus = 2;
    login::gValueDA3B24 = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(value));
    char diagnostic[64];
    static_cast<void>(std::sprintf(
        diagnostic, "LOGIN REPLY RESULT=%d", value));
    static_cast<void>(login::detail::Sub_44B5C0(diagnostic));
    if (value != 0) {
        if (std::strcmp(text, "busy") == 0) {
            login::gValueDA3B24 = 1234;
        }
        return;
    }

    gNetworkRuntimeState.protocolModeDA3B22 = 1;
    static_cast<void>(platform::ReadLegacyTime32_53BA26(
        &gNetworkRuntimeState.time62B394));
    gNetworkRuntimeState.localTime62B380 =
        platform::LocalTime_53B8C6(&gNetworkRuntimeState.time62B394);

    const char* const separator = platform::StrStr_53BE40(text, "|");
    std::memset(
        character_select::detail::gText9A4EAC,
        0,
        sizeof(character_select::detail::gText9A4EAC));
    if (separator != nullptr) {
        platform::StrNCpy_53BEC0(
            character_select::detail::gText9A4EAC,
            text,
            static_cast<std::uint32_t>(separator - text));
        platform::ParseProtectionTables_404600(separator);
        return;
    }

    std::memcpy(
        character_select::detail::gText9A4EAC,
        text,
        std::strlen(text) + 1U);
}

// 原函数：0x00517510。
std::int32_t BuildProtocolCommand_517510(
    char* const buffer,
    const char* const command)
{
    // 与服务端nrproto_CreateHeader一致；明文命令不混淆、不生成密文状态尾。
    static_cast<void>(std::sprintf(buffer, "%s ", command));
    return 0;
}

} // namespace

// 原函数：0x004037D0。
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

// 原函数：0x00516DA0。
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

    gNetworkRuntimeState.buffer58B5F0 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(bufferLength)));
    std::uint8_t* const second = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(gNetworkRuntimeState.bufferLength)));
    // 原16DF9/16E0A：先取下一请求长度，再发布本次分配结果。
    const std::uint32_t thirdLength = gNetworkRuntimeState.bufferLength;
    gNetworkRuntimeState.buffer58B5F4 = second;
    std::uint8_t* const third = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(thirdLength)));
    const std::uint32_t fourthLength = gNetworkRuntimeState.bufferLength;
    gNetworkRuntimeState.buffer58B5F8 = third;
    gNetworkRuntimeState.buffer58B5FC = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(fourthLength)));
    gNetworkRuntimeState.pointerTable58B614 = static_cast<char**>(
        std::malloc(
            static_cast<std::size_t>(gNetworkRuntimeState.bufferLength) * sizeof(char*)));
    gNetworkRuntimeState.tripleBuffer58B604 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(gNetworkRuntimeState.bufferLength * 3U)));
    gNetworkRuntimeState.tripleBuffer58B608 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(gNetworkRuntimeState.bufferLength * 3U)));
    gNetworkRuntimeState.tripleBuffer58B60C = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(gNetworkRuntimeState.bufferLength * 3U)));
    std::uint8_t* const fourthTriple = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(gNetworkRuntimeState.bufferLength * 3U)));

    // 原16E7E/16E84：末结果发布前取首清零长度和指针；零计数不执行REP写入。
    std::uint32_t clearLength = gNetworkRuntimeState.bufferLength;
    std::uint8_t* clearBuffer = gNetworkRuntimeState.buffer58B5F0;
    gNetworkRuntimeState.tripleBuffer58B610 = fourthTriple;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = gNetworkRuntimeState.bufferLength;
    clearBuffer = gNetworkRuntimeState.buffer58B5F4;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = gNetworkRuntimeState.bufferLength;
    clearBuffer = gNetworkRuntimeState.buffer58B5F8;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = gNetworkRuntimeState.bufferLength;
    clearBuffer = gNetworkRuntimeState.buffer58B5FC;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = gNetworkRuntimeState.bufferLength;
    char** const clearTable = gNetworkRuntimeState.pointerTable58B614;
    if (clearLength != 0U) {
        std::fill_n(clearTable, static_cast<std::size_t>(clearLength), nullptr);
    }
    clearLength = gNetworkRuntimeState.bufferLength * 3U;
    clearBuffer = gNetworkRuntimeState.tripleBuffer58B604;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = gNetworkRuntimeState.bufferLength * 3U;
    clearBuffer = gNetworkRuntimeState.tripleBuffer58B608;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = gNetworkRuntimeState.bufferLength * 3U;
    clearBuffer = gNetworkRuntimeState.tripleBuffer58B60C;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = gNetworkRuntimeState.bufferLength * 3U;
    clearBuffer = gNetworkRuntimeState.tripleBuffer58B610;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }

    // 原16F7E：后置检查和首个失败释放共用同次首指针。
    std::uint8_t* const firstBuffer = gNetworkRuntimeState.buffer58B5F0;
    if (firstBuffer == nullptr) {
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
    std::free(firstBuffer);
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
void Sub_517040(
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
            source != nullptr ? source : input::LegacyText593850()),
        capacity);
}

// 原函数：0x00517670。
std::uint8_t* FormatProtocolInteger_517670(const std::int32_t value)
{
    static_cast<void>(
        detail::Sub_517730(value, gNetworkRuntimeState.integerFormatScratch, 64));
    return gNetworkRuntimeState.integerFormatScratch;
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

// 原函数：0x00517160。
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

// 原函数：0x00517190。
std::int32_t ParseProtocolInteger_517190(const char* const text)
{
    if (text == nullptr) {
        return 0;
    }
    return detail::Sub_5176B0(text);
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
    return platform::Atoi_53C649(field.data());
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
    *output = platform::Atoi_53C649(field.data());
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

// 原函数：0x005172D0。
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

// 原函数：0x005171B0。
std::uint8_t* Sub_5171B0(const char* const text)
{
    if (text != nullptr) {
        return UnescapeProtocolText_5172D0(text);
    }
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const destination = gNetworkRuntimeState.buffer58B5F8;
    CopyProtocolBytes_517080(
        destination,
        reinterpret_cast<const std::uint8_t*>(input::LegacyText593850()),
        capacity);
    return gNetworkRuntimeState.buffer58B5F8;
}

// 原函数：0x00517480。
std::int32_t SendProtocolBuffer_517480(
    const std::uintptr_t socket,
    char* const buffer)
{
    if (gNetworkRuntimeState.sendLogPathDA3B58[0] != '\0') {
        platform::LegacyFile* const file = platform::OpenLegacyStream_53B2F1(
            gNetworkRuntimeState.sendLogPathDA3B58,
            "a+");
        if (file != nullptr) {
            static_cast<void>(std::fprintf(file, "->%s\n", buffer));
            static_cast<void>(platform::CloseLegacyStream_53B1D0(file));
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

// 原函数：0x00512CA0。
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

// 原函数：0x00512C10。
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

// 原 [0x512B90,0x512BCC)：列表请求的三次共享缓冲读取保持独立。
std::int32_t Sub_512B90(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Gp"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        input::LegacyText593850(),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512BD0,0x512C0C)：登出请求的三次共享缓冲读取保持独立。
std::int32_t Sub_512BD0(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "WA"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        input::LegacyText593850(),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512C60,0x512C9C)：保活命令后追加 593850 当前文本并发送。
std::int32_t Sub_512C60(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "DE"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        input::LegacyText593850(),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511CD0,0x511D0C)：as 请求，追加共享原字节文本。
std::int32_t Sub_511CD0(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "as"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        input::LegacyText593850(),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511FB0,0x511FEC)：ixH 请求，追加共享原字节文本。
std::int32_t Sub_511FB0(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "ixH"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        input::LegacyText593850(),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512230,0x51226C)：suI 请求，追加共享原字节文本。
std::int32_t Sub_512230(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "suI"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        input::LegacyText593850(),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512570,0x5125AC)：ih 请求，追加共享原字节文本。
std::int32_t Sub_512570(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "ih"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        input::LegacyText593850(),
        capacity));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512CF0,0x512D2C)：lO 请求，追加共享原字节文本。
std::int32_t Sub_512CF0(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "lO"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        input::LegacyText593850(),
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

// 原 [0x512D30,0x512D9B)：两项分别采样容量，格式化后重读输出缓冲。
std::int32_t Sub_512D30(
    const std::uintptr_t socket,
    const std::int32_t version,
    const char* const payload)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Xoee"));

    const std::int32_t integerCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const integerToken = FormatProtocolIntegerToken_517120(version);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(integerToken),
        integerCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(payload);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512DA0,0x512DE8)：命令构造后采一次容量，再格式化和追加整数。
std::int32_t Sub_512DA0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "IPy"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x510570,0x510621)：四项各自采容量、格式化后重读共享输出缓冲。
std::int32_t Sub_510570(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const char* const text,
    const std::int32_t fourth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "zA"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

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

// NLD01／原512800：摘要、账号、口令、适配器四轮独立采容量，转义后重读输出缓冲。
std::int32_t Sub_512800(
    const std::uintptr_t socket,
    const char* const executableDigest,
    const char* const account,
    const char* const password,
    const char* const adapterId)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "JFVf"));

    const std::int32_t digestCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const digestToken =
        FormatProtocolTextToken_517160(executableDigest);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(digestToken),
        digestCapacity));

    const std::int32_t accountCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const accountToken = FormatProtocolTextToken_517160(account);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(accountToken),
        accountCapacity));

    const std::int32_t passwordCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const passwordToken =
        FormatProtocolTextToken_517160(password);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(passwordToken),
        passwordCapacity));

    const std::int32_t adapterCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const adapterToken =
        FormatProtocolTextToken_517160(adapterId);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(adapterToken),
        adapterCapacity));

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

// 原 [0x510630,0x510704)：五轮各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_510630(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "pg"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x510710,0x510807)：六轮各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_510710(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth,
    const std::int32_t sixth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "UUN"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    const std::int32_t sixthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const sixthToken = FormatProtocolIntegerToken_517120(sixth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(sixthToken),
        sixthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x00510AC0。
std::int32_t Sub_510AC0(
    const std::uintptr_t socket, const std::int32_t x, const std::int32_t y,
    const std::int32_t item, const std::int32_t target)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "iVfo"));
    const std::array<std::int32_t, 4> fields {x, y, item, target};
    for (const std::int32_t field : fields) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            gNetworkRuntimeState.bufferLength);
        std::uint8_t* const token = FormatProtocolIntegerToken_517120(field);
        static_cast<void>(AppendProtocolText_5170D0(
            reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
            reinterpret_cast<const char*>(token), capacity));
    }
    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// NFI01：fIM三Base62整数按现明文普通空格发送；保原time/seed/rand与共享缓冲时序。
// 原函数：0x00510B80。
std::int32_t Sub_510B80(
    const std::uintptr_t socket, const std::int32_t x,
    const std::int32_t y, const std::int32_t direction)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "fIM"));
    const std::array<std::int32_t, 3> fields {x, y, direction};
    for (const std::int32_t field : fields) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            gNetworkRuntimeState.bufferLength);
        std::uint8_t* const token = FormatProtocolIntegerToken_517120(field);
        static_cast<void>(AppendProtocolText_5170D0(
            reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
            reinterpret_cast<const char*>(token), capacity));
    }

    char* buffer = reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0);
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
            const std::int32_t seconds = platform::ReadLegacyTime32_53BA26(nullptr);
            static_cast<void>(
                platform::SeedRandom_53B4BF(std::bit_cast<std::uint32_t>(seconds)));
            selected = platform::Rand16() % spaces;
            if (selected == 0) {
                selected = 1;
            }
            buffer = reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0);
        }
    }
    std::int32_t ordinal = 0;
    for (index = 0; index < std::strlen(buffer); ++index) {
        if (buffer[index] == ' ') {
            if (ordinal == selected) {
                // 服务端不以0x17终止整数token，保普通空格；原选择/RNG消费及fresh借用保持。
                buffer = reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0);
                break;
            }
            ++ordinal;
        }
    }
    return SendProtocolBuffer_517480(socket, buffer);
}

// 原 [0x510810,0x51087B)：两轮各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_510810(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "KeOX"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511030,0x5110AE)：先追加数量，再逐项读取 W32 并分别采容量。
std::int32_t Sub_511030(
    const std::uintptr_t socket,
    const std::int32_t* values,
    const std::int32_t count)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "kza"));

    const std::int32_t countCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const countToken = FormatProtocolIntegerToken_517120(count);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(countToken),
        countCapacity));

    for (std::int32_t index = 0; index < count; ++index) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            gNetworkRuntimeState.bufferLength);
        const std::int32_t value = values[index];
        std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
        static_cast<void>(AppendProtocolText_5170D0(
            reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
            reinterpret_cast<const char*>(token),
            capacity));
    }

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511190,0x5111D8)：FPkR单整数发送；容量先采，输出缓冲独立重取。
std::int32_t Sub_511190(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "FPkR"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x00511250。
std::int32_t Sub_511250(
    const std::uintptr_t socket, const std::int32_t kind, const char* const text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Ufm"));
    const std::int32_t kindCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const kindToken = FormatProtocolIntegerToken_517120(kind);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(kindToken), kindCapacity));
    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken), textCapacity));
    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5112C0,0x51134E)：两整数、文本各自采容量并重读输出缓冲。
std::int32_t Sub_5112C0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const char* const text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "GY"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511350,0x5113BB)：两整数分别采容量并在格式化后重读输出缓冲。
std::int32_t Sub_511350(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "uHb"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x510880,0x5108EB)：对战请求，发送坐标不是目标格坐标。
std::int32_t Sub_510880(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "ik"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x510940,0x5109AB)：观战命令原字节为大小写敏感的IMv。
std::int32_t Sub_510940(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "IMv"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5111E0,0x51124B)：名片请求，两字段分别采容量并重读共享缓冲。
std::int32_t Sub_5111E0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "vcAz"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5113C0,0x51142B)：两整数各自采容量并在格式化后重读输出缓冲。
std::int32_t Sub_5113C0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "npzi"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511430,0x511478)：单次采容量、格式化后重读输出缓冲发送。
std::int32_t Sub_511430(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "dbQC"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原[510FA0,51102E)：三次采容量与格式化后的输出地址重取独立。
std::int32_t Sub_510FA0(
    const std::uintptr_t socket, const std::int32_t first,
    const char* const text, const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "CV"));
    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken), firstCapacity));
    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken), textCapacity));
    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken), thirdCapacity));
    return SendProtocolBuffer_517480(socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511480,0x51150E)：整数、文本、整数分别采容量并重读输出缓冲。
std::int32_t Sub_511480(
    const std::uintptr_t socket,
    const std::int32_t first,
    const char* const text,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "uId"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原[5110B0,511184)：五字段各自采容量，格式化后重取输出地址。
std::int32_t Sub_5110B0(
    const std::uintptr_t socket, const std::int32_t first, const std::int32_t second,
    const std::int32_t third, const char* const text, const std::int32_t fifth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "rMsv"));
    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken), firstCapacity));
    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken), secondCapacity));
    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken), thirdCapacity));
    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken), textCapacity));
    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fifthToken), fifthCapacity));
    return SendProtocolBuffer_517480(socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511510,0x5115E4)：三整数、文本、末整数分别采容量并重读输出缓冲。
std::int32_t Sub_511510(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const char* const text,
    const std::int32_t fifth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "PfqV"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5115F0,0x511638)：单次采容量、整数格式化后重读缓冲发送。
std::int32_t Sub_5115F0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "HoW"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511640,0x51175A)：七项依次采容量、格式化并重读输出缓冲。
std::int32_t Sub_511640(
    const std::uintptr_t socket,
    const std::int32_t field1,
    const char* const text2,
    const std::int32_t field3,
    const char* const text4,
    const std::int32_t field5,
    const char* const text6,
    const char* const text7)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Ot"));

    const std::int32_t capacity1 = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token1 = FormatProtocolIntegerToken_517120(field1);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token1),
        capacity1));

    const std::int32_t capacity2 = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token2 = FormatProtocolTextToken_517160(text2);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token2),
        capacity2));

    const std::int32_t capacity3 = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token3 = FormatProtocolIntegerToken_517120(field3);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token3),
        capacity3));

    const std::int32_t capacity4 = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token4 = FormatProtocolTextToken_517160(text4);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token4),
        capacity4));

    const std::int32_t capacity5 = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token5 = FormatProtocolIntegerToken_517120(field5);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token5),
        capacity5));

    const std::int32_t capacity6 = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token6 = FormatProtocolTextToken_517160(text6);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token6),
        capacity6));

    const std::int32_t capacity7 = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token7 = FormatProtocolTextToken_517160(text7);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token7),
        capacity7));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511760,0x5117CB)：两项各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_511760(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "TLKH"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5117D0,0x51183B)：整数、文本各自采容量并在格式化后重读输出缓冲。
std::int32_t Sub_5117D0(
    const std::uintptr_t socket,
    const std::int32_t value,
    const char* const text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "wD"));

    const std::int32_t valueCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const valueToken = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(valueToken),
        valueCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511840,0x5118AB)：文本、整数各自采容量并在格式化后重读输出缓冲。
std::int32_t Sub_511840(
    const std::uintptr_t socket,
    const char* const text,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "GH"));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t valueCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const valueToken = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(valueToken),
        valueCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5118B0,0x5118F8)：单次容量采样、整数格式化后重读缓冲发送。
std::int32_t Sub_5118B0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "lz"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511900,0x5119F7)：六项各自先读容量，格式化后重读输出缓冲。
// CTS01：原六字段逐项容量→token→新buffer→append，末发送S32透传。
std::int32_t Sub_511900(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const char* const text,
    const std::int32_t fourth,
    const std::int32_t fifth,
    const std::int32_t sixth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "uSr"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    const std::int32_t sixthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const sixthToken = FormatProtocolIntegerToken_517120(sixth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(sixthToken),
        sixthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511A00,0x511A48)：构造命令后只采一次容量，追加并发送。
std::int32_t Sub_511A00(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "dcMv"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511A50,0x511ADE)：三项各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_511A50(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "zn"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511AE0,0x511BB4)：五项各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_511AE0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "LKQy"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511BC0,0x511C2B)：两项各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_511BC0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Ukuy"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511C30,0x511C78)：构造命令后只采一次容量，追加并发送。
std::int32_t Sub_511C30(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "iET"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511C80,0x511CC8)：一次容量采样，整数 token 追加后发送。
std::int32_t Sub_511C80(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "McVa"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511D10,0x511D58)：一次容量采样，整数 token 追加后发送。
std::int32_t Sub_511D10(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "mjCv"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511D60,0x511DCB)：两项各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_511D60(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "BG"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511DD0,0x511E5E)：三项各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_511DD0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "MIS"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511E60,0x511EEE)：三项各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_511E60(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "klF"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511EF0,0x511FA1)：三个整数及文本各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_511EF0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const char* const text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "sM"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x511FF0,0x512038)：容量在格式化前采样，追加和发送各自重读缓冲。
std::int32_t Sub_511FF0(
    const std::uintptr_t socket, const std::int32_t index)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "QeuC"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(index);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512040,0x5120CE)：两段文本和整数各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_512040(
    const std::uintptr_t socket,
    const char* const first,
    const char* const second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "aiAb"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolTextToken_517160(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolTextToken_517160(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5120D0,0x51215E)：三段文本各自采容量、格式化后重读输出缓冲。
std::int32_t Sub_5120D0(
    const std::uintptr_t socket,
    const char* const first,
    const char* const second,
    const char* const third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "ylGN"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolTextToken_517160(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolTextToken_517160(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolTextToken_517160(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512160,0x512185)：建命令后重读缓冲并完整透传发送返回。
std::int32_t Sub_512160(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "JMh"));
    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512190,0x5121D8)：一次容量采样，整数 token 追加后发送。
std::int32_t Sub_512190(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "ElVN"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5121E0,0x512228)：容量在格式化前采样，追加及发送时重读缓冲。
std::int32_t Sub_5121E0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "wAo"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x005127B0。
std::int32_t Sub_5127B0(const std::uintptr_t socket, const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "rh"));
    const std::int32_t capacity =
        std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512270,0x5122B8)：格式化前采容量，追加与发送各自重读缓冲。
std::int32_t Sub_512270(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "OU"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5122C0,0x512308)：格式化前采容量，追加与发送各自重读缓冲。
std::int32_t Sub_5122C0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "zGp"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512310,0x512358)：格式化前采容量，追加与发送各自重读缓冲。
std::int32_t Sub_512310(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "tj"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512360,0x5123A8)：转义前采容量，追加与发送各自重读缓冲。
std::int32_t Sub_512360(
    const std::uintptr_t socket,
    const char* const text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "ACwB"));

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

// 原 [0x5123B0,0x5123F8)：格式化前采容量，追加与发送各自重读缓冲。
std::int32_t Sub_5123B0(
    const std::uintptr_t socket,
    const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "IHw"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512400,0x51246B)：两轮分别采容量、格式化后重读共享缓冲。
std::int32_t Sub_512400(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "kjSK"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512470,0x5124DB)：两轮分别采容量、格式化后重读共享缓冲。
std::int32_t Sub_512470(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Noeb"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x510E30,0x510EBE)：放地请求，容量读取先于格式化，缓冲读取在其后。
std::int32_t Sub_510E30(
    const std::uintptr_t socket,
    const std::int32_t x,
    const std::int32_t y,
    const std::int32_t pet)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "FvWm"));
    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const xToken = FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(xToken), xCapacity));
    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const yToken = FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(yToken), yCapacity));
    const std::int32_t petCapacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
    std::uint8_t* const petToken = FormatProtocolIntegerToken_517120(pet);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(petToken), petCapacity));
    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5124E0,0x51256E)：三轮分别采容量、格式化后重读共享缓冲。
std::int32_t Sub_5124E0(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "HJQp"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x5125B0,0x51261B)：两轮各自采容量、格式化后重读共享输出缓冲。
std::int32_t Sub_5125B0(
    const std::uintptr_t socket,
    const std::int32_t value,
    const char* const text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "ycX"));

    const std::int32_t valueCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const valueToken = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(valueToken),
        valueCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512620,0x512717)：六轮分别采容量、格式化后重读共享输出缓冲。
// SDM01：原512620完整77条；xD五S32及文本各先容量再token/fresh输出，当前原字段/LF明文及S32返回保。
std::int32_t Sub_512620(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth,
    const char* const text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "xD"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fourthToken = FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const fifthToken = FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const textToken = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// IMT01：原55条，四S32字段容量先格式/输出重读及S32发送返回，当前原字段/LF明文，正确正文保。
// 原函数：0x00510CE0。
std::int32_t Sub_510CE0(
    const std::uintptr_t socket, const std::int32_t x, const std::int32_t y,
    const std::int32_t group, const std::int32_t index)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "QpfE"));

    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const xToken = FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(xToken), xCapacity));

    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const yToken = FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(yToken), yCapacity));

    const std::int32_t groupCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const groupToken = FormatProtocolIntegerToken_517120(group);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(groupToken), groupCapacity));

    const std::int32_t indexCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const indexToken = FormatProtocolIntegerToken_517120(index);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(indexToken), indexCapacity));

    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x00510A00。
std::int32_t Sub_510A00(
    const std::uintptr_t socket, const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t group)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Ak"));

    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const xToken = FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(xToken), xCapacity));

    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const yToken = FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(yToken), yCapacity));

    const std::int32_t indexCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const indexToken = FormatProtocolIntegerToken_517120(index);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(indexToken), indexCapacity));

    const std::int32_t groupCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const groupToken = FormatProtocolIntegerToken_517120(group);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(groupToken), groupCapacity));

    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x00516C90。
std::int32_t Sub_516C90(
    const std::uintptr_t socket, const std::int32_t sourceGroup, const std::int32_t source,
    const std::int32_t targetGroup, const std::int32_t target, const std::int32_t amount)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Cpa"));

    const std::int32_t sourceGroupCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const sourceGroupToken = FormatProtocolIntegerToken_517120(sourceGroup);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(sourceGroupToken), sourceGroupCapacity));

    const std::int32_t sourceCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const sourceToken = FormatProtocolIntegerToken_517120(source);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(sourceToken), sourceCapacity));

    const std::int32_t targetGroupCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const targetGroupToken = FormatProtocolIntegerToken_517120(targetGroup);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(targetGroupToken), targetGroupCapacity));

    const std::int32_t targetCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const targetToken = FormatProtocolIntegerToken_517120(target);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(targetToken), targetCapacity));

    const std::int32_t amountCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const amountToken = FormatProtocolIntegerToken_517120(amount);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(amountToken), amountCapacity));

    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x00516D70。
std::int32_t SendIuEPacket_516D70(const std::uintptr_t socket)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "iuE"));
    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x005109B0。
std::int32_t SendBattlePacket_5109B0(
    const std::uintptr_t socket, const char* const text)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "Rg"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// MDR01：原510DA0完整44条；woz三个S32，各容量先token/fresh输出，末真实S32发送结果，原字段/LF明文。
std::int32_t Sub_510DA0(
    const std::uintptr_t socket, const std::int32_t x,
    const std::int32_t y, const std::int32_t amount)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "woz"));

    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const xToken = FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(xToken), xCapacity));

    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const yToken = FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(yToken), yCapacity));

    const std::int32_t amountCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const amountToken = FormatProtocolIntegerToken_517120(amount);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(amountToken), amountCapacity));

    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x00510EC0。
std::int32_t Sub_510EC0(
    const std::uintptr_t socket, const std::int32_t source,
    const std::int32_t target, const std::int32_t amount)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "yi"));

    const std::int32_t sourceCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const sourceToken = FormatProtocolIntegerToken_517120(source);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(sourceToken), sourceCapacity));

    const std::int32_t targetCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const targetToken = FormatProtocolIntegerToken_517120(target);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(targetToken), targetCapacity));

    const std::int32_t amountCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const amountToken = FormatProtocolIntegerToken_517120(amount);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(amountToken), amountCapacity));

    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x00510F50。
std::int32_t Sub_510F50(
    const std::uintptr_t socket, const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "fS"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token),
        capacity));

    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原函数：0x005108F0。
std::int32_t Sub_5108F0(
    const std::uintptr_t socket, const std::int32_t value)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "eL"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const token = FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(token), capacity));
    return SendProtocolBuffer_517480(
        socket, reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

// 原 [0x512720,0x5127AE)：三项各自采容量、格式化后重读共享输出缓冲。
std::int32_t Sub_512720(
    const std::uintptr_t socket,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0), "cZt"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const firstToken = FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const secondToken = FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    std::uint8_t* const thirdToken = FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return SendProtocolBuffer_517480(
        socket,
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
}

namespace {

// 512E40每项独立读取参数表；文本先反转义再复制到同编号工作区。
template <typename Value>
Value DecodeDispatchArgument(
    const std::size_t index, const bool workspaceBeforeCapacity)
{
    static_assert(std::is_same_v<Value, std::int32_t> || std::is_same_v<Value, char*>);
    if constexpr (std::is_same_v<Value, std::int32_t>) {
        return ParseProtocolInteger_517190(gNetworkRuntimeState.pointerTable58B614[index]);
    } else {
        std::uint8_t* const decoded = Sub_5171B0(gNetworkRuntimeState.pointerTable58B614[index]);
        std::int32_t capacity;
        std::uint8_t** workspace;
        // 原85字段分两种基址/容量读取顺序；槽解引用均在两次读取后。
        if (workspaceBeforeCapacity) {
            workspace = gNetworkRuntimeState.workspaceSlots;
            capacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
        } else {
            capacity = std::bit_cast<std::int32_t>(gNetworkRuntimeState.bufferLength);
            workspace = gNetworkRuntimeState.workspaceSlots;
        }
        std::uint8_t* const destination = workspace[index];
        return reinterpret_cast<char*>(
            CopyProtocolBytesAndReturn_517690(destination, capacity, decoded));
    }
}

template <typename... Values, std::size_t... Indices>
std::tuple<Values...> DecodeDispatchArguments(
    std::index_sequence<Indices...>, const std::uint32_t workspaceBeforeCapacityMask)
{
    // bit N对应原第N参数读序；见doc/cg2d_dispatch_account_field_read_spec.md。
    // 花括号初始化保证先完成第N项，再读取第N+1项及其共享临时区。
    return std::tuple<Values...> {
        DecodeDispatchArgument<Values>(
            Indices + 1,
            (workspaceBeforeCapacityMask & (1U << (Indices + 1))) != 0U)...
    };
}

template <auto Callback, typename... Values>
bool TryDispatchReply(
    const char* const head, const char* const command, const std::uintptr_t socket,
    const std::uint32_t workspaceBeforeCapacityMask = 0U)
{
    // 原108块均先取头U8再取命令U8；每对比较后以同头样本判NUL。
    auto* receivedBytes = reinterpret_cast<const unsigned char*>(head);
    auto* commandBytes = reinterpret_cast<const unsigned char*>(command);
    for (;;) {
        const unsigned char receivedFirst = receivedBytes[0];
        const unsigned char commandFirst = commandBytes[0];
        if (receivedFirst != commandFirst) {
            return false;
        }
        if (receivedFirst == 0) {
            break;
        }
        const unsigned char receivedSecond = receivedBytes[1];
        const unsigned char commandSecond = commandBytes[1];
        if (receivedSecond != commandSecond) {
            return false;
        }
        receivedBytes += 2;
        commandBytes += 2;
        if (receivedSecond == 0) {
            break;
        }
    }
    const auto values = DecodeDispatchArguments<Values...>(
        std::index_sequence_for<Values...> {}, workspaceBeforeCapacityMask);
    std::apply([socket](const auto... arguments) {
        static_cast<void>(Callback(socket, arguments...));
    }, values);
    return true;
}

// 原509080不读取参数；六个调用形态仍先完成各自的全部解码。
template <typename... Values>
// 原函数：0x00509080。
void IgnoreReplyArguments_509080(const std::uintptr_t, Values...)
{
    cg::NullSub_509080();
}

} // namespace

namespace detail {

// 原函数：0x00512E40。
std::int32_t NetworkDispatch_512E40(
    const std::uintptr_t socket, const void* const received)
{
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        gNetworkRuntimeState.bufferLength);
    CopyProtocolBytes_517080(
        gNetworkRuntimeState.buffer58B5F0,
        static_cast<const std::uint8_t*>(received), capacity);
    SplitReceivedBuffer_5173B0(
        reinterpret_cast<char*>(gNetworkRuntimeState.buffer58B5F0));
    std::uint32_t headerValue;
    char head[1024];
    Sub_517040(&headerValue, reinterpret_cast<std::uint8_t*>(head), sizeof(head),
        gNetworkRuntimeState.pointerTable58B614);
    // 服务端明文头直接是原命令（如whV），不再执行原密文命令逆变换。

    // I为完整S32解析，T为同编号工作区char*；顺序与原108条比较路径一致。
    using I = std::int32_t;
    using T = char*;
    if (TryDispatchReply<world::detail::Sub_5093C0, I, I, I>(head, "QBt", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_509090, I, I, I, I, I, I, I, I, I>(head, "jmb", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_509100, I, I, I, I, I, I, T>(head, "aBS", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50CF90, I, I>(head, "EZ", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50D080, I, I>(head, "OaHi", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50D120, I, I>(head, "dAgg", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50D500, T>(head, "Gn", socket)) return 0;
    if (TryDispatchReply<Sub_50D700, T>(head, "hB", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50D3C0, T>(head, "LYIm", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50BFD0, T>(head, "ELoy", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50C260, T>(head, "rU", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50C870, I, I, I, I>(head, "PudO", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50C900, I, I>(head, "vWpn", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50C580, I, T>(head, "xmgQ", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50C7A0, I, I>(head, "MTg", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50DC70, I, T, I>(head, "Mhbt", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50E5B0, I, T>(head, "Ilz", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50E5D0, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, T, T>(head, "dM", socket, 0x30000U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<I, T>, I, T>(head, "Dn", socket, 0x4U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<I>, I>(head, "IYo", socket)) return 0;
    if (TryDispatchReply<Sub_50DD80, I, I, I, I, I, I, I, T>(head, "lDuJ", socket, 0x100U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50D740, T>(head, "gnxd", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50D8C0, T>(head, "PhdQ", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50D9E0, I, T>(head, "vI", socket, 0x4U)) return 0;
    if (TryDispatchReply<Sub_50E830>(head, "GC", socket)) return 0;
    if (TryDispatchReply<login::detail::Sub_50E850, T>(head, "aDn", socket)) return 0;
    if (TryDispatchReply<login::detail::Sub_50EB40, T>(head, "znv", socket)) return 0;
    if (TryDispatchReply<login::detail::UpdateOrderedRecordBatch_50EC50, T>(head, "yuD", socket)) return 0;
    if (TryDispatchReply<login::detail::UpdateOrderedRecords_50EEB0, I, T>(head, "Kka", socket, 0x4U)) return 0;
    if (TryDispatchReply<Sub_50F130, I>(head, "oRut", socket)) return 0;
    if (TryDispatchReply<Sub_50F190, I, T, I>(head, "Rll", socket, 0x4U)) return 0;
    if (TryDispatchReply<login::detail::Sub_50F280, I>(head, "frQp", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50F3A0, T>(head, "HNP", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50F2A0, T>(head, "eZT", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50F8E0, T>(head, "IUn", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50F450, T, I, T, T, T, I, I, T, I, T, I, T, T>(head, "DpSY", socket, 0x1128U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50F430, T>(head, "cNB", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50F7F0, I, T, I>(head, "ow", socket, 0x4U)) return 0;
    if (TryDispatchReply<ui::Sub_50F990, I, I, I, I, T>(head, "DyAG", socket, 0x20U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50F9E0, T>(head, "KyN", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50CAE0, I, T, I, I>(head, "zdr", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50CE10, T>(head, "Zd", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_509650, T>(head, "aPc", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_509BA0, T>(head, "UU", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50A6B0, I, T>(head, "IsSy", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50AC20, I, T>(head, "AU", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50AEF0, T>(head, "TNT", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50B380, T>(head, "nsY", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50A3E0, I, T>(head, "JJQj", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50A300, T>(head, "Sc", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50BCD0, T>(head, "NI", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50BF20, T>(head, "uSB", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_509E30, I, T>(head, "jv", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::detail::Sub_509EB0, T>(head, "cNE", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50A120, I, T>(head, "Rspn", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50B0D0, I, T>(head, "OBee", socket, 0x4U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<T>, T>(head, "Yo", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50BFA0, I>(head, "SkBD", socket)) return 0;
    if (TryDispatchReply<Sub_50D3B0, I>(head, "sZWu", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50D000, I, I>(head, "bNY", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E340, T>(head, "ysVc", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E360, T>(head, "ND", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E380, T>(head, "LY", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E3A0, T>(head, "nFV", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50C7F0, I, I, T>(head, "JgO", socket, 0x8U)) return 0;
    if (TryDispatchReply<ui::Sub_50E3C0, T>(head, "Cxo", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E3F0, T, I>(head, "ms", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E440>(head, "NdM", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E4A0, T>(head, "nUbJ", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E470, I>(head, "hJq", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E4D0, I, T>(head, "RwK", socket, 0x4U)) return 0;
    if (TryDispatchReply<ui::Sub_50E500, I, T>(head, "Yq", socket, 0x4U)) return 0;
    if (TryDispatchReply<ui::Sub_50E530>(head, "xEg", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E570, I>(head, "We", socket)) return 0;
    if (TryDispatchReply<Sub_50CF20, I, I, I, I>(head, "BHe", socket)) return 0;
    if (TryDispatchReply<Sub_50CF60, I>(head, "kg", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50CF70, I, I>(head, "sA", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50BFC0, I>(head, "Ov", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E110, I, I, I, I, T>(head, "BZ", socket, 0x20U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50E1A0, I, I, T>(head, "zzd", socket, 0x8U)) return 0;
    if (TryDispatchReply<world::detail::Sub_50E210, I>(head, "TFCu", socket)) return 0;
    if (TryDispatchReply<Sub_50E310, I>(head, "wiB", socket)) return 0;
    if (TryDispatchReply<Sub_50E630, I, I>(head, "bJg", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_50E650, I, I, I, I, I>(head, "PVfz", socket)) return 0;
    if (TryDispatchReply<Sub_50E680, I, I, I>(head, "murD", socket)) return 0;
    if (TryDispatchReply<Sub_50E6C0, I, I, I>(head, "ftaN", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_509550, T>(head, "TeD", socket)) return 0;
    if (TryDispatchReply<world::detail::Sub_5093F0, I, I, I, I, I, I, I, I, I, I, I, I>(head, "GmNM", socket)) return 0;
    if (TryDispatchReply<HandleResponse_508800, I, T>(head, "whV", socket, 0x4U)) return 0;
    if (TryDispatchReply<login::HandleCharacterCreationResponse_508EE0, T, I, T>(head, "DZ", socket, 0x8U)) return 0;
    if (TryDispatchReply<login::HandleCharacterDeletionResponse_509020, T, T>(head, "oy", socket, 0x4U)) return 0;
    if (TryDispatchReply<login::HandleLoginResponse_508BC0, T, T>(head, "hPUd", socket, 0x4U)) return 0;
    if (TryDispatchReply<login::HandleCharacterListResponse_5089B0, I, T>(head, "Daj", socket, 0x4U)) return 0;
    if (TryDispatchReply<login::detail::Sub_508CF0, T, T>(head, "HMNy", socket, 0x4U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<T>, T>(head, "cJzI", socket)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<I, I>, I, I>(head, "Bi", socket)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<T>, T>(head, "LOh", socket)) return 0;
    if (TryDispatchReply<Sub_50E6E0, T>(head, "lSZ", socket)) return 0;
    if (TryDispatchReply<character_select::detail::Sub_50E770, I>(head, "PfM", socket)) return 0;
    if (TryDispatchReply<Sub_50E780, I>(head, "ed", socket)) return 0;
    if (TryDispatchReply<platform::ConfigureLegacyMouseTracker_50E7B0, I, I>(head, "tq", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E7D0, I>(head, "Tmrx", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E7E0, I>(head, "KXhH", socket)) return 0;
    if (TryDispatchReply<ui::Sub_50E7F0, I, I, T, T, T, T>(head, "ft", socket, 0x68U)) return 0;
    if (TryDispatchReply<ui::Sub_50E820, I>(head, "Yx", socket)) return 0;
    static_cast<void>(TryDispatchReply<world::detail::Sub_510150, I, T>(head, "iKe", socket, 0x4U));
    static_cast<void>(TryDispatchReply<Sub_510460, I, I, I, I>(head, "kiG", socket));
    static_cast<void>(TryDispatchReply<Sub_510490, T>(head, "TgX", socket));
    return -1;
}

} // namespace detail

// 原函数：0x00516B60。
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

    gNetworkRuntimeState.workspaceSlots = static_cast<std::uint8_t**>(
        std::malloc(kWorkspaceSlotCount * sizeof(std::uint8_t*)));
    if (gNetworkRuntimeState.workspaceSlots == nullptr) {
        return -1;
    }

    std::fill_n(
        gNetworkRuntimeState.workspaceSlots,
        kWorkspaceSlotCount,
        nullptr);
    for (std::size_t index = 0; index < kWorkspaceSlotCount; ++index) {
        gNetworkRuntimeState.workspaceSlots[index] = static_cast<std::uint8_t*>(
            std::malloc(static_cast<std::size_t>(bufferLength)));
        if (gNetworkRuntimeState.workspaceSlots[index] == nullptr) {
            std::free(gNetworkRuntimeState.workspaceSlots[0]);
            return -1;
        }
    }
    return 0;
}

// 原函数：0x00508140。
std::int32_t InitializeNetworkRuntime_508140()
{
    WSADATA data;
    if (::WSAStartup(MAKEWORD(1, 1), &data) != 0) {
        return 0;
    }

    static_cast<void>(AllocateNetworkWorkspace_516B60(
        &QueuePlaintextProtocolBytes,
        0x10000U,
        platform::gConnectionSocket));
    // 新会话复用同一队列owner，仅重置有效数量与轮询状态，避免遗留半包。
    constexpr std::int32_t emptyLength = 0;
    std::memcpy(gNetworkRuntimeState.queueBytes.data() + kSendLengthOffset,
        &emptyLength, sizeof(emptyLength));
    gNetworkRuntimeState.receiveLengthD69790 = 0;
    gNetworkRuntimeState.frameModuloD9F3C8 = 0;
    gNetworkRuntimeState.flagsD9F2C0 = 0;
    gNetworkRuntimeState.errorD9F2CC = 0;
    gNetworkRuntimeState.active = 1;
    world::gValueD9F2CA = 0;
    return 1;
}

// 原函数：0x00507DF0。
std::uint8_t NetworkLoop_507DF0()
{
    const std::uint8_t flags = gNetworkRuntimeState.flagsD9F2C0;
    if ((flags & 1U) == 0) {
        gNetworkRuntimeState.flagsD9F2C0 = static_cast<std::uint8_t>(flags | 1U);
        gNetworkRuntimeState.lastTickD9F2C4 = ::GetTickCount();
    }
    if (gNetworkRuntimeState.active == 0) {
        return 0;
    }

    const std::uint8_t connectionState = world::gValueD9F2CA;
    if (connectionState == 1) {
        const std::int32_t sceneType = world::SetCurrentSceneType(11);
        world::gValueD9F2CA = 2;
        return static_cast<std::uint8_t>(sceneType);
    }
    if (connectionState != 0) {
        return connectionState;
    }
    if (login::gRequestPollingEnabled == 0) {
        return 0;
    }

    const std::uint32_t frame =
        (gNetworkRuntimeState.frameModuloD9F3C8 + 1U) & 7U;
    gNetworkRuntimeState.frameModuloD9F3C8 = frame;
    if (frame != 0) {
        return static_cast<std::uint8_t>(frame);
    }

    fd_set readSet;
    fd_set writeSet;
    const SOCKET socket = static_cast<SOCKET>(platform::gConnectionSocket);
    readSet.fd_count = 1;
    writeSet.fd_count = 1;
    readSet.fd_array[0] = socket;
    writeSet.fd_array[0] = socket;
    timeval timeout {0, 0};
    static_cast<void>(::select(2, &readSet, &writeSet, nullptr, &timeout));

    if (::FD_ISSET(
            static_cast<SOCKET>(platform::gConnectionSocket), &readSet)) {
        char received[0x1000];
        std::memset(received, 0, sizeof(received));
        const std::int32_t receivedLength = ::recv(
            static_cast<SOCKET>(platform::gConnectionSocket),
            received,
            0xFFF,
            0);
        if (receivedLength > 0) {
            gNetworkRuntimeState.receiveResetD9F3C4 = 0;
            const std::int32_t queued = AppendReceiveQueue_5081E0(
                received, static_cast<std::uint32_t>(receivedLength));
            if (login::gConnectionStep >= 71 && login::gConnectionStep <= 80) {
                char diagnostic[64];
                static_cast<void>(std::sprintf(
                    diagnostic, "LOGIN RECEIVED BYTES=%d QUEUE=%d", receivedLength, queued));
                static_cast<void>(login::detail::Sub_44B5C0(diagnostic));
            }
        }
        else if (receivedLength == 0) {
            // 对端正常关闭也必须进入既有断线路径，不能继续等待登录回复。
            static_cast<void>(::closesocket(
                static_cast<SOCKET>(platform::gConnectionSocket)));
            world::gValueD9F2CA = 1;
            gNetworkRuntimeState.errorD9F2CC = 0;
            static_cast<void>(login::detail::Sub_44B5C0("NETWORK PEER CLOSED"));
        }
        else if (receivedLength == -1) {
            const std::int32_t error = detail::Sub_5082D0();
            if (error > 0) {
                static_cast<void>(::closesocket(
                    static_cast<SOCKET>(platform::gConnectionSocket)));
                world::gValueD9F2CA = 1;
                gNetworkRuntimeState.errorD9F2CC = error;
            }
        }
    }

    std::uint8_t* const dispatch =
        gNetworkRuntimeState.queueBytes.data() + kDispatchOffset;
    const volatile std::uint32_t* const length =
        &gNetworkRuntimeState.bufferLength;
    while (gNetworkRuntimeState.receiveLengthD69790 > 0) {
        if (ReceivePlaintextProtocolLine(dispatch, static_cast<std::int32_t>(kDispatchSize)) != 0) {
            break;
        }
        const bool diagnoseLogin = login::gConnectionStep >= 71
            && login::gConnectionStep <= 80;
        const std::int32_t dispatchResult = detail::NetworkDispatch_512E40(
            platform::gConnectionSocket, dispatch);
        std::memset(dispatch, 0, kDispatchSize);
        std::memset(gNetworkRuntimeState.buffer58B5F0, 0, *length);
        std::memset(gNetworkRuntimeState.buffer58B5F4, 0, *length);
        std::memset(gNetworkRuntimeState.buffer58B5F8, 0, *length);
        std::memset(gNetworkRuntimeState.buffer58B5FC, 0, *length);
        std::fill_n(gNetworkRuntimeState.pointerTable58B614, *length, nullptr);
        std::memset(gNetworkRuntimeState.tripleBuffer58B604, 0, 3U * *length);
        std::memset(gNetworkRuntimeState.tripleBuffer58B608, 0, 3U * *length);
        std::memset(gNetworkRuntimeState.tripleBuffer58B60C, 0, 3U * *length);
        std::memset(gNetworkRuntimeState.tripleBuffer58B610, 0, 3U * *length);
        std::fill_n(gNetworkRuntimeState.pointerTable58B614, *length, nullptr);
        if (diagnoseLogin) {
            char diagnostic[64];
            static_cast<void>(std::sprintf(
                diagnostic, "LOGIN DISPATCH RESULT=%d", dispatchResult));
            static_cast<void>(login::detail::Sub_44B5C0(diagnostic));
        }
    }

    std::int32_t sendLength;
    std::memcpy(
        &sendLength,
        gNetworkRuntimeState.queueBytes.data() + kSendLengthOffset,
        sizeof(sendLength));
    if (sendLength > 0 && world::gValueD9F2CA == 0) {
        const std::int32_t sent = ::send(
            static_cast<SOCKET>(platform::gConnectionSocket),
            reinterpret_cast<const char*>(gNetworkRuntimeState.queueBytes.data()),
            sendLength,
            0);
        if (sent > 0) {
            gNetworkRuntimeState.lastTickD9F2C4 = ::GetTickCount();
        }
        if (sent == -1) {
            const std::int32_t error = detail::Sub_508310();
            if (error > 0) {
                static_cast<void>(::closesocket(
                    static_cast<SOCKET>(platform::gConnectionSocket)));
                world::gValueD9F2CA = 1;
                gNetworkRuntimeState.errorD9F2CC = error;
            }
        }
        else {
            static_cast<void>(ConsumeSendQueue_508280(sent));
            if (sent > 0 && login::gConnectionStep >= 71
                && login::gConnectionStep <= 80) {
                char diagnostic[64];
                static_cast<void>(std::sprintf(
                    diagnostic, "LOGIN SENT BYTES=%d", sent));
                static_cast<void>(login::detail::Sub_44B5C0(diagnostic));
            }
        }
    }

    const std::uint32_t now = ::GetTickCount();
    if (gNetworkRuntimeState.lastTickD9F2C4 + 30000U < now
        && gNetworkRuntimeState.active == 1) {
        return static_cast<std::uint8_t>(
            Sub_512C10(platform::gConnectionSocket, "nr"));
    }
    return static_cast<std::uint8_t>(now);
}

// 原函数：0x005081E0。
std::int32_t AppendReceiveQueue_5081E0(
    const void* const source,
    const std::uint32_t size)
{
    if (login::gRequestPollingEnabled == 0) {
        return -100;
    }

    const std::int32_t previous = gNetworkRuntimeState.receiveLengthD69790;
    const std::int32_t next =
        previous + static_cast<std::int32_t>(size);
    if (next > static_cast<std::int32_t>(kQueueCapacity)) {
        return -1;
    }

    std::memcpy(
        gNetworkRuntimeState.queueBytes.data() + kReceiveQueueOffset + previous,
        source,
        size);
    gNetworkRuntimeState.receiveLengthD69790 = next;
    return 0;
}

// 原函数：0x00508230。
std::int32_t ConsumeReceiveQueue_508230(const std::int32_t size)
{
    if (login::gRequestPollingEnabled == 0) {
        return -100;
    }
    if (size >= static_cast<std::int32_t>(kQueueCapacity)) {
        return -1;
    }

    const std::int32_t remaining =
        gNetworkRuntimeState.receiveLengthD69790 - size;
    gNetworkRuntimeState.receiveLengthD69790 = remaining;
    std::uint8_t* const receive =
        gNetworkRuntimeState.queueBytes.data() + kReceiveQueueOffset;
    std::memmove(receive, receive + size, static_cast<std::uint32_t>(remaining));
    return 0;
}

// 原函数：0x00508280。
std::int32_t ConsumeSendQueue_508280(const std::int32_t size)
{
    if (login::gRequestPollingEnabled == 0) {
        return -100;
    }
    if (size >= static_cast<std::int32_t>(kQueueCapacity)) {
        return -1;
    }

    std::uint8_t* const send = gNetworkRuntimeState.queueBytes.data();
    std::int32_t previous;
    std::memcpy(&previous, send + kSendLengthOffset, sizeof(previous));
    const std::int32_t remaining = previous - size;
    std::memcpy(send + kSendLengthOffset, &remaining, sizeof(remaining));
    std::memmove(send, send + size, static_cast<std::uint32_t>(remaining));
    return 0;
}

std::uint8_t ReadNetworkRuntimeActive() noexcept
{
    return gNetworkRuntimeState.active;
}

std::int32_t ReadNetworkErrorD9F2CC() noexcept
{
    return gNetworkRuntimeState.errorD9F2CC;
}

std::uint8_t ReadProtocolByte593850() noexcept
{
    return static_cast<std::uint8_t>(input::LegacyText593850()[0]);
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

// 原函数：0x005081A0。
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
