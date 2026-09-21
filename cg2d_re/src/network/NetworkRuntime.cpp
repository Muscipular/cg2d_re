#include "cg/network/NetworkRuntime.hpp"

#include "NetworkRuntimeDependencies.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"

#include <winsock2.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>

namespace cg::network {
namespace {

constexpr std::size_t kWorkspaceSlotCount = 18;

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
    void** workspaceSlots; // 原 0xDA3F98，18 个本机指针槽。
    std::uint8_t active;   // 原 0xD9F2C8，网络运行期活动门禁。
    std::uint32_t valueD9F2D0; // 原 0xD9F2D0，仅保留地址身份的 W32。
};

NetworkRuntimeState gNetworkRuntimeState {};

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
