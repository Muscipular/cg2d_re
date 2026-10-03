#include <winsock2.h>
#include <ws2tcpip.h>

#include "cg/LoginState.hpp"

#include "platform/LegacyFormat.hpp"
#include "platform/LegacyStdio.hpp"

#include "LoginStateDependencies.hpp"
#include "world/MailState.hpp"

#include "cg/Blacklist.hpp"
#include "cg/LoginConnection.hpp"
#include "cg/LoginMessage.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/LoginScene.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyNetworkAdapter.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"

#include <cstdio>
#include <cstring>

namespace {

// 原 0x55C9F8「處理登入逾時。」正文 14 字节；字符串字面量隐式追加唯一 NUL。
constexpr char kLoginTimeoutMessage[] =
    "\xB3\x42\xB2\x7A\xB5\x6E\xA4\x4A\xB9\x4F\xAE\xC9\xA1\x43";
// 原 0x55C9CC「登入處理失敗，或斷線後，數分鐘內無法登入。」正文 42 字节；字符串字面量隐式追加唯一 NUL。
constexpr char kLoginFailureMessage[] =
    "\xB5\x6E\xA4\x4A\xB3\x42\xB2\x7A\xA5\xA2\xB1\xD1\xA1\x41"
    "\xA9\xCE\xC2\x5F\xBD\x75\xAB\xE1\xA1\x41\xBC\xC6\xA4\xC0"
    "\xC4\xC1\xA4\xBA\xB5\x4C\xAA\x6B\xB5\x6E\xA4\x4A\xA1\x43";
// 原 0x55BAD8「登入中」正文 6 字节；字符串字面量隐式追加唯一 NUL。
constexpr char kLoginProgressText[] = "\xB5\x6E\xA4\x4A\xA4\xA4";
// 原 asc_55C7F0 正文 12 字节；字符串字面量隐式追加 NUL。
constexpr char kServerConnectingText[] =
    "\xA6\xF8\xAA\x41\xBE\xB9\xB3\x73\xB1\xB5\xA4\xA4";

constexpr char kServerInfoFailureMessage[] =
    "\xA6\xF8\xAA\x41\xBE\xB9\xAA\xBA\xA6\x57"
    "\xBA\xD9\xA4\xA3\xA5\xBF\xBD\x54\xA1\x43";
constexpr char kSocketFailureMessage[] =
    "\xBA\xF4\xB8\xF4\xB3\x71\xB0\x54\xA8\xF3"
    "\xA9\x77\xA1\x5D\x54\x43\x50\xA1\x5E\xBF"
    "\xF9\xBB\x7E\xA1\x43";
constexpr char kServerAddressFailureFormat[] =
    "\xB5\x4C\xAA\x6B\xA8\xFA\xB1\x6F\xA6\xF8"
    "\xAA\x41\xBE\xB9\xAA\xBA\x20\x49\x50\x20"
    "\xA6\xEC\xA7\x7D\xA1\x43\x28\x25\x73\x29";
constexpr char kConnectFailureMessage[] =
    "\xB5\x4C\xAA\x6B\xB6\x7D\xA9\x6C\xB3\x42"
    "\xB2\x7A\xBB\x50\xA6\xF8\xAA\x41\xBE\xB9"
    "\xAA\xBA\xB3\x73\xB1\xB5\xA1\x43";
constexpr char kConnectExceptionMessage[] =
    "\xB5\x4C\xAA\x6B\xB3\x73\xA4\x57\xA6\xF8"
    "\xAA\x41\xBE\xB9\xA1\x43\x20";
constexpr char kLoginRejectedMessage[] =
    "\xB5\x4C\xAA\x6B\xB5\x6E\xA4\x4A\xA1\x43";
constexpr char kConnectionTimeoutMessage[] =
    "\xBB\x50\xA6\xF8\xAA\x41\xBE\xB9\xAA\xBA"
    "\xB3\x73\xB1\xB5\xB9\x4F\xAE\xC9\xA1\x43";

// 原 0x9A4E80、0x9A5958、0x9A50D4、0x9A4B90 只由 sub_45EA60 读写。
struct LoginProgressLayout {
    std::int32_t rowCount;
    std::int32_t columnCount;
    std::int32_t y;
    std::int32_t x;
};

LoginProgressLayout gLoginProgressLayout {};

// 原 0x9A4E8C、0x9A60E0、0x9A60E4、0x9A4CC0 只由 sub_45D990 读写。
struct ServerConnectionLayout {
    std::int32_t columnCount;
    std::int32_t rowCount;
    std::int32_t x;
    std::int32_t y;
};

ServerConnectionLayout gServerConnectionLayout {};

constexpr std::int32_t kProgressActionX = 256;
constexpr std::int32_t kProgressActionY = 192;
constexpr std::int32_t kProgressActionWidth = 128;
constexpr std::int32_t kProgressActionHeight = 96;
constexpr std::int16_t kProgressTextX = 296;
constexpr std::int16_t kProgressTextY = 232;

static_assert(sizeof(kLoginTimeoutMessage) == 15);
static_assert(sizeof(kLoginFailureMessage) == 43);
static_assert(sizeof(kLoginProgressText) == 7);
static_assert(sizeof(kServerConnectingText) == 13);
static_assert(sizeof(kServerInfoFailureMessage) == 21);
static_assert(sizeof(kSocketFailureMessage) == 26);
static_assert(sizeof(kServerAddressFailureFormat) == 31);
static_assert(sizeof(kConnectFailureMessage) == 29);
static_assert(sizeof(kConnectExceptionMessage) == 18);
static_assert(sizeof(kLoginRejectedMessage) == 11);
static_assert(sizeof(kConnectionTimeoutMessage) == 21);

} // namespace

namespace cg::login {

namespace {

// 原 0x9A6E8C/0x9A6E90 的全部引用都只在 sub_45EA50/sub_45EA60 内。
std::uint16_t gLoginProgressPhase = 0;
YobiAction* gLoginProgressAction = nullptr;

// 原 0x9A5D6C/0x9A6E40 的全部引用都只在 sub_45D990 内。
char gServerConnectingText[256] {};
YobiAction* gServerConnectionAction = nullptr;

} // namespace

namespace detail {

// 原 0x44B5C0：日志打开失败返回零；成功返回 fclose 的 W32 位模式。
std::uint32_t Sub_44B5C0(const char* text)
{
    platform::LegacyFile* const file = platform::OpenLegacyStream_53B2F1("systemlog.txt", "a");
    if (file == nullptr) {
        return 0;
    }

    static_cast<void>(platform::FormatLegacyStream_53BC0F(file, "%s\n", text));
    return static_cast<std::uint32_t>(platform::CloseLegacyStream_53B1D0(file));
}

std::int32_t Sub_508360()
{
    if (network::ReadNetworkRuntimeActive() == 0) {
        return 0;
    }

    std::int32_t step = gConnectionStep;
    if (step == 0) {
        gRequestStartTick = platform::ReadTickCount32();

        char serverAddress[128];
        std::uint16_t port;
        if (GetServerAddressInfo_507D90(
                static_cast<std::int32_t>(gValue58AD94),
                serverAddress,
                &port) < 0) {
            static_cast<void>(std::sprintf(
                gRequestMessage, "%s", kServerInfoFailureMessage));
            return -2;
        }

        const SOCKET connectionSocket = ::socket(AF_INET, SOCK_STREAM, 0);
        platform::gConnectionSocket =
            static_cast<std::uintptr_t>(connectionSocket);
        if (connectionSocket == INVALID_SOCKET) {
            static_cast<void>(std::sprintf(
                gRequestMessage, "%s", kSocketFailureMessage));
            return -3;
        }

        u_long nonBlocking = 1;
        static_cast<void>(
            ::ioctlsocket(connectionSocket, FIONBIO, &nonBlocking));

        if (platform::gNoDelay != 0) {
            int noDelay = 1;
            if (::setsockopt(
                    connectionSocket,
                    IPPROTO_TCP,
                    TCP_NODELAY,
                    reinterpret_cast<const char*>(&noDelay),
                    4) != 0) {
                return -100;
            }
        }

        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = ::htons(port);
        address.sin_addr.s_addr = ::inet_addr(serverAddress);
        if (address.sin_addr.s_addr == INADDR_NONE) {
            hostent* const host = ::gethostbyname(serverAddress);
            if (host == nullptr) {
                static_cast<void>(std::sprintf(
                    gRequestMessage,
                    kServerAddressFailureFormat,
                    serverAddress));
                static_cast<void>(::closesocket(connectionSocket));
                return -4;
            }
            std::memcpy(
                &address.sin_addr.s_addr,
                host->h_addr_list[0],
                sizeof(address.sin_addr.s_addr));
        }

        static_cast<void>(network::ResetProtocolState_4037D0());
        network::WriteValueD9F2D0(0);

        if (::connect(
                connectionSocket,
                reinterpret_cast<sockaddr*>(&address),
                0x10) == SOCKET_ERROR) {
            const int error = ::WSAGetLastError();
            if (error != 10035) {
                char logMessage[sizeof("CONNECT ERROR=-2147483648")];
                static_cast<void>(std::sprintf(
                    logMessage, "CONNECT ERROR=%d", error));
                static_cast<void>(Sub_44B5C0(logMessage));
                static_cast<void>(::closesocket(connectionSocket));
                static_cast<void>(std::sprintf(
                    gRequestMessage, "%s", kConnectFailureMessage));
                return -5;
            }
        }
        gConnectionStep = 1;
    }
    else if (step >= 1 && step <= 70) {
        ++step;
        gConnectionStep = step;
        if (step == 70) {
            gConnectionStep = 69;
        }

        const SOCKET connectionSocket =
            static_cast<SOCKET>(platform::gConnectionSocket);
        fd_set readSet;
        readSet.fd_count = 1;
        readSet.fd_array[0] = connectionSocket;
        fd_set writeSet;
        writeSet.fd_count = 1;
        writeSet.fd_array[0] = connectionSocket;
        fd_set exceptionSet;
        exceptionSet.fd_count = 1;
        exceptionSet.fd_array[0] = connectionSocket;
        timeval timeout {0, 0};
        static_cast<void>(
            ::select(2, &readSet, &writeSet, &exceptionSet, &timeout));

        if (::__WSAFDIsSet(connectionSocket, &writeSet) != 0) {
            gConnectionStep = 71;
            gRequestPollingEnabled = 1;
            static_cast<void>(Sub_44B5C0("CONNECT OK"));
        }
        if (::__WSAFDIsSet(connectionSocket, &exceptionSet) != 0) {
            static_cast<void>(std::sprintf(
                gRequestMessage, "%s", kConnectExceptionMessage));
            static_cast<void>(::closesocket(connectionSocket));
            return -6;
        }
    }
    else if (step >= 71 && step <= 80) {
        if (step == 71) {
            network::WriteProtocolModeDA3B22(0);

            char adapterScratch[16];
            char accountCopy[44];
            char adapterId[44];
            static_cast<void>(platform::Sub_534EF0(adapterScratch));
            std::strcpy(accountCopy, gValueDA38DC);
            std::strcpy(adapterId, adapterScratch);
            std::strcpy(gValueDA38DC, accountCopy);
            static_cast<void>(network::Sub_512800(
                platform::gConnectionSocket,
                gValueDA3A1C,
                accountCopy,
                gValueDA3A5C,
                adapterId));
            gRequestStatus = 1;
        }
        else if (gRequestStatus == 2) {
            if (network::ReadProtocolModeDA3B22() == 0) {
                gRequestStatus = 0;
                static_cast<void>(std::sprintf(
                    gRequestMessage, "%s", kLoginRejectedMessage));
                static_cast<void>(::closesocket(
                    static_cast<SOCKET>(platform::gConnectionSocket)));
                return -7;
            }
            step = 81;
        }

        ++step;
        gConnectionStep = step;
        if (step == 81) {
            gConnectionStep = 80;
        }
    }
    else if (step >= 81 && step <= 98) {
        ++step;
        gConnectionStep = step;
    }
    else if (step == 99) {
        gRequestStatus = 0;
        return 1;
    }

    const std::uint32_t now = platform::ReadTickCount32();
    if (now - gRequestStartTick > 600000U) {
        static_cast<void>(std::sprintf(
            gRequestMessage, "%s", kConnectionTimeoutMessage));
        return -1;
    }
    return 0;
}

} // namespace detail

std::uint16_t gValue9A6E3C = 0;
std::uint16_t gValue9A6E3E = 0;
std::int32_t gValue55A054 = -1;
char gValue9A5348[256] {};

static_assert(sizeof(gServerConnectingText) == 256);
static_assert(sizeof(kServerConnectingText) <= sizeof(gServerConnectingText));
static_assert(sizeof(gValue9A5348) == 256);

std::int32_t ResetValues_45D980()
{
    gValue9A6E3C = 0;
    gValue9A6E3E = 0;
    return 0;
}

std::int32_t UpdateServerConnection()
{
    std::uint16_t phase = gValue9A6E3C;
    std::int32_t result = 0;

    if (phase == 0) {
        std::memcpy(
            gServerConnectingText,
            kServerConnectingText,
            sizeof(kServerConnectingText));

        gServerConnectionLayout.columnCount =
            (detail::TextMeasure(gServerConnectingText, 0) + 111) / 64;
        if (gServerConnectionLayout.columnCount < 2) {
            gServerConnectionLayout.columnCount = 2;
        }

        const std::int32_t columnCount = gServerConnectionLayout.columnCount;
        const std::int32_t x = ((10 - columnCount) * 64) / 2;
        gServerConnectionLayout.rowCount = 2;
        gServerConnectionLayout.x = x;
        gServerConnectionLayout.y = 192;
        gServerConnectionAction = detail::Sub_5044E0(
            gServerConnectionLayout.x,
            gServerConnectionLayout.y,
            columnCount * 64,
            96,
            4,
            1);
        gValue9A6E3C = 2;
        phase = 2;
    }

    if (phase == 1) {
        gValue9A6E3C = 2;
        phase = 2;
    }

    if (phase == 2) {
        cg::network::ShutdownNetworkRuntime_5081A0();
        cg::blacklist::PersistAndReset_401000(gValue55A054);
        if (cg::network::InitializeNetworkRuntime_508140() != 0) {
            gValue9A6E3C = 3;
            ResetServerConnectionState_508350();
            gValue9A6E3C = 4;
            phase = 4;
        } else {
            result = -1;
        }
    }

    if (phase == 3) {
        ResetServerConnectionState_508350();
        gValue9A6E3C = 4;
        phase = 4;
    }

    if (phase == 4) {
        const std::int32_t updateResult = detail::Sub_508360();
        if (updateResult == 1) {
            result = 1;
        } else if (updateResult < 0) {
            result = -2;
        }
    }

    if (result != 0) {
        if (gServerConnectionAction != nullptr) {
            detail::MarkDeath(gServerConnectionAction);
            gServerConnectionAction = nullptr;
        }
        cg::blacklist::LoadBlacklistFile(gValue55A054);
    }

    YobiAction* const action = gServerConnectionAction;
    if (action != nullptr && detail::IsYobiActionReady(action, 1)) {
        const std::int32_t textWidth = detail::TextMeasure(gServerConnectingText, 0);
        const std::int32_t y = gServerConnectionLayout.y
            + (gServerConnectionLayout.rowCount * 48 - 16) / 2;
        const std::int32_t x = gServerConnectionLayout.x
            + (gServerConnectionLayout.columnCount * 64 - textWidth) / 2;
        detail::DrawText(
            static_cast<std::int16_t>(x),
            static_cast<std::int16_t>(y),
            2,
            0,
            0,
            gServerConnectingText,
            0,
            0);
    }
    return result;
}

std::int32_t ProcessState_45E960()
{
    if (world::gCurrentSceneState == 0) {
        ResetLoginProgress();
        detail::Sub_532D80();
        ++world::gCurrentSceneState;
    }

    if (world::gCurrentSceneState == 1) {
        const std::int32_t result = UpdateLoginProgress();
        if (result == 1) {
            world::detail::Sub_534290();
            world::SetCurrentSceneType(9);
        } else if (result == -1) {
            world::gCurrentSceneState = 100;
            std::memcpy(gValue9A5348, kLoginTimeoutMessage, sizeof(kLoginTimeoutMessage));
        } else if (result == -2) {
            world::gCurrentSceneState = 100;
            std::memcpy(gValue9A5348, kLoginFailureMessage, sizeof(kLoginFailureMessage));
        }
    }

    if (world::gCurrentSceneState == 100) {
        ResetLoginMessage();
        ++world::gCurrentSceneState;
    }

    const std::int32_t sceneState = world::gCurrentSceneState;
    if (sceneState != 101) {
        return sceneState;
    }

    const std::int32_t result = UpdateLoginMessage(gValue9A5348);
    if (result == 0) {
        return 0;
    }
    return world::SetCurrentSceneType(2);
}

void ResetLoginProgress()
{
    gLoginProgressPhase = 0;
}

std::int32_t UpdateLoginProgress()
{
    std::uint16_t phase = gLoginProgressPhase;
    std::int32_t result = 0;

    if (phase == 0) {
        gLoginProgressPhase = 1;
        gLoginProgressLayout.rowCount = 2;
        gLoginProgressLayout.columnCount = 2;
        gLoginProgressLayout.y = kProgressActionY;
        gLoginProgressLayout.x = kProgressActionX;
        gLoginProgressAction = detail::Sub_5044E0(
            kProgressActionX,
            kProgressActionY,
            kProgressActionWidth,
            kProgressActionHeight,
            4,
            1);
        phase = gLoginProgressPhase;
    }

    if (phase == 1) {
        StartLoginRequest();
        ++gLoginProgressPhase;
    } else if (phase == 2) {
        result = PollLoginRequest();
        if (result != 0) {
            if (gLoginProgressAction != nullptr) {
                detail::MarkDeath(gLoginProgressAction);
                gLoginProgressAction = nullptr;
            }
            return result;
        }
    }

    YobiAction* const action = gLoginProgressAction;
    if (action != nullptr && detail::IsYobiActionReady(action, 1)) {
        detail::DrawText3(kProgressTextX, kProgressTextY, 2, 0, kLoginProgressText, 0);
    }
    return result;
}

} // namespace cg::login
