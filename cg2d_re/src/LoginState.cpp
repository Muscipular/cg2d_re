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

#include <cstring>
#include <cstdio> // 原格式化／扫描接口：0x0053B304、0x0053BC0F；直接沿MSVC CRT。

namespace {

// 原 0x55C9F8「處理登入逾時。」正文 14 字节；字符串字面量隐式追加唯一 NUL。
constexpr char kLoginTimeoutMessage[] =
    "\xCC\x8E\xC0\xED\xB5\xC7\xC8\xEB\xD3\xE2\x95\x72\xA1\xA3";
// 原 0x55C9CC「登入處理失敗，或斷線後，數分鐘內無法登入。」正文 42 字节；字符串字面量隐式追加唯一 NUL。
constexpr char kLoginFailureMessage[] =
    "\xB5\xC7\xC8\xEB\xCC\x8E\xC0\xED\xCA\xA7\x94\xA1\xA3\xAC"
    "\xBB\xF2\x94\xE0\xBE\x80\xE1\xE1\xA3\xAC\x94\xB5\xB7\xD6"
    "\xE7\x8A\x83\xC8\x9F\x6F\xB7\xA8\xB5\xC7\xC8\xEB\xA1\xA3";
// 原 0x55BAD8「登入中」正文 6 字节；字符串字面量隐式追加唯一 NUL。
constexpr char kLoginProgressText[] = "\xB5\xC7\xC8\xEB\xD6\xD0";
// 原 asc_55C7F0 正文 12 字节；字符串字面量隐式追加 NUL。
constexpr char kServerConnectingText[] =
    "\xCB\xC5\xB7\xFE\xC6\xF7\xDF\x42\xBD\xD3\xD6\xD0";

constexpr char kServerInfoFailureMessage[] =
    "\xCB\xC5\xB7\xFE\xC6\xF7\xB5\xC4\xC3\xFB"
    "\xB7\x51\xB2\xBB\xD5\xFD\xB4\x5F\xA1\xA3";
constexpr char kSocketFailureMessage[] =
    "\xBE\x57\xC2\xB7\xCD\xA8\xD3\x8D\x85\x66"
    "\xB6\xA8\xA3\xA8\x54\x43\x50\xA3\xA9\xE5"
    "\x65\xD5\x60\xA1\xA3";
constexpr char kServerAddressFailureFormat[] =
    "\x9F\x6F\xB7\xA8\xC8\xA1\xB5\xC3\xCB\xC5"
    "\xB7\xFE\xC6\xF7\xB5\xC4\x20\x49\x50\x20"
    "\xCE\xBB\xD6\xB7\xA1\xA3\x28\x25\x73\x29";
constexpr char kConnectFailureMessage[] =
    "\x9F\x6F\xB7\xA8\xE9\x5F\xCA\xBC\xCC\x8E"
    "\xC0\xED\xC5\x63\xCB\xC5\xB7\xFE\xC6\xF7"
    "\xB5\xC4\xDF\x42\xBD\xD3\xA1\xA3";
constexpr char kConnectExceptionMessage[] =
    "\x9F\x6F\xB7\xA8\xDF\x42\xC9\xCF\xCB\xC5"
    "\xB7\xFE\xC6\xF7\xA1\xA3\x20";
constexpr char kLoginRejectedMessage[] =
    "\x9F\x6F\xB7\xA8\xB5\xC7\xC8\xEB\xA1\xA3";
constexpr char kConnectionTimeoutMessage[] =
    "\xC5\x63\xCB\xC5\xB7\xFE\xC6\xF7\xB5\xC4"
    "\xDF\x42\xBD\xD3\xD3\xE2\x95\x72\xA1\xA3";

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

// 原登录进度布局计算见doc/cg2d_login_progress_layout_sampling_spec.md。

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

    static_cast<void>(std::fprintf(file, "%s\n", text));
    return static_cast<std::uint32_t>(platform::CloseLegacyStream_53B1D0(file));
}

// 原函数：0x00508360。
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
                gRequestMessage, kServerInfoFailureMessage));
            return -2;
        }

        const SOCKET connectionSocket = ::socket(AF_INET, SOCK_STREAM, 0);
        platform::gConnectionSocket =
            static_cast<std::uintptr_t>(connectionSocket);
        if (connectionSocket == INVALID_SOCKET) {
            static_cast<void>(std::sprintf(
                gRequestMessage, kSocketFailureMessage));
            return -3;
        }

        u_long nonBlocking = 1;
        static_cast<void>(
            ::ioctlsocket(connectionSocket, FIONBIO, &nonBlocking));

        if (platform::gNoDelay != 0) {
            int noDelay = 1;
            if (::setsockopt(
                    static_cast<SOCKET>(platform::gConnectionSocket),
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
                static_cast<void>(::closesocket(
                    static_cast<SOCKET>(platform::gConnectionSocket)));
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
                static_cast<SOCKET>(platform::gConnectionSocket),
                reinterpret_cast<sockaddr*>(&address),
                0x10) == SOCKET_ERROR) {
            const int error = ::WSAGetLastError();
            if (error != 10035) {
                char logMessage[sizeof("CONNECT ERROR=-2147483648")];
                static_cast<void>(std::sprintf(
                    logMessage, "CONNECT ERROR=%d", error));
                static_cast<void>(Sub_44B5C0(logMessage));
                static_cast<void>(::closesocket(
                    static_cast<SOCKET>(platform::gConnectionSocket)));
                static_cast<void>(std::sprintf(
                    gRequestMessage, kConnectFailureMessage));
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

        if (::__WSAFDIsSet(
                static_cast<SOCKET>(platform::gConnectionSocket), &writeSet) != 0) {
            gConnectionStep = 71;
            gRequestPollingEnabled = 1;
            static_cast<void>(Sub_44B5C0("CONNECT OK"));
        }
        if (::__WSAFDIsSet(
                static_cast<SOCKET>(platform::gConnectionSocket), &exceptionSet) != 0) {
            static_cast<void>(std::sprintf(
                gRequestMessage, kConnectExceptionMessage));
            static_cast<void>(::closesocket(
                static_cast<SOCKET>(platform::gConnectionSocket)));
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
            const std::int32_t queued = network::Sub_512800(
                platform::gConnectionSocket,
                gValueDA3A1C,
                accountCopy,
                gValueDA3A5C,
                adapterId);
            // 原508748：登录发送返回后先重读阶段，再发布请求状态。
            step = gConnectionStep;
            gRequestStatus = 1;
            char diagnostic[64];
            static_cast<void>(std::sprintf(
                diagnostic, "LOGIN QUEUE RESULT=%d", queued));
            static_cast<void>(Sub_44B5C0(diagnostic));
        }
        else if (gRequestStatus == 2) {
            if (network::ReadProtocolModeDA3B22() == 0) {
                gRequestStatus = 0;
                static_cast<void>(std::sprintf(
                    gRequestMessage, kLoginRejectedMessage));
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
            gRequestMessage, kConnectionTimeoutMessage));
        return -1;
    }
    return 0;
}

} // namespace detail

std::uint16_t gValue9A6E3C = 0; // 原全局：0x009A6E3C。
std::uint16_t gValue9A6E3E = 0; // 原全局：0x009A6E3E。
std::int32_t gValue55A054 = -1; // 原全局：0x0055A054。
char gValue9A5348[256] {}; // 原全局：0x009A5348。

static_assert(sizeof(gServerConnectingText) == 256);
static_assert(sizeof(kServerConnectingText) <= sizeof(gServerConnectingText));
static_assert(sizeof(gValue9A5348) == 256);

// 原函数：0x0045D980。
std::int32_t ResetValues_45D980()
{
    gValue9A6E3C = 0;
    gValue9A6E3E = 0;
    return 0;
}

// 原函数：0x0045D990。
std::int32_t UpdateServerConnection()
{
    std::uint16_t phase = gValue9A6E3C;
    std::int32_t result = 0;

    if (phase == 0) {
        static_cast<void>(std::sprintf(
            gServerConnectingText, kServerConnectingText));

        std::int32_t columnCount =
            (detail::TextMeasure(gServerConnectingText, 0) + 111) / 64;
        gServerConnectionLayout.columnCount = columnCount;
        if (columnCount < 2) {
            columnCount = 2;
            gServerConnectionLayout.columnCount = columnCount;
        }

        const std::int32_t x = ((10 - columnCount) * 64) / 2;
        const std::int32_t width = columnCount * 64;
        gServerConnectionLayout.rowCount = 2;
        gServerConnectionLayout.x = x;
        gServerConnectionLayout.y = 192;
        gServerConnectionAction = detail::Sub_5044E0(
            x,
            192,
            width,
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
            phase = gValue9A6E3C;
        }
    } else {
        phase = gValue9A6E3C;
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
        const std::int32_t rowCount = gServerConnectionLayout.rowCount;
        const std::int32_t verticalHeight = rowCount * 48 - 16;
        const std::int32_t y = gServerConnectionLayout.y;
        const std::int32_t textY = verticalHeight / 2 + y;
        const std::int32_t columnCount = gServerConnectionLayout.columnCount;
        const std::int32_t horizontalWidth = columnCount * 64 - textWidth;
        const std::int32_t x = gServerConnectionLayout.x;
        const std::int32_t textX = horizontalWidth / 2 + x;
        detail::DrawText(
            static_cast<std::int16_t>(textX),
            static_cast<std::int16_t>(textY),
            2,
            0,
            0,
            gServerConnectingText,
            0,
            0);
    }
    return result;
}

// 原函数：0x0045E960。
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

// 原函数：0x0045EA50。
void ResetLoginProgress()
{
    gLoginProgressPhase = 0;
}

// 原函数：0x0045EA60。
std::int32_t UpdateLoginProgress()
{
    std::uint16_t phase = gLoginProgressPhase;
    std::int32_t result = 0;

    if (phase == 0) {
        const std::uint32_t textLength =
            static_cast<std::uint32_t>(std::strlen(kLoginProgressText));
        gLoginProgressPhase = 1;
        const std::int32_t columnCount =
            static_cast<std::int32_t>(((textLength * 9U) >> 6U) + 2U);
        gLoginProgressLayout.rowCount = 2;
        gLoginProgressLayout.columnCount = columnCount;
        const std::int32_t actionX = ((10 - columnCount) * 64) / 2;
        gLoginProgressLayout.y = 192;
        const std::int32_t actionWidth = columnCount * 64;
        gLoginProgressLayout.x = actionX;
        gLoginProgressAction = detail::Sub_5044E0(
            actionX,
            192,
            actionWidth,
            96,
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
            YobiAction* const action = gLoginProgressAction;
            if (action != nullptr) {
                detail::MarkDeath(action);
                gLoginProgressAction = nullptr;
            }
            return result;
        }
    }

    YobiAction* const action = gLoginProgressAction;
    if (action != nullptr && detail::IsYobiActionReady(action, 1)) {
        const std::int32_t rowCount = gLoginProgressLayout.rowCount;
        const std::int32_t y = gLoginProgressLayout.y;
        const std::int32_t textY = ((rowCount * 48 - 16) / 2) + y;
        const std::int32_t textLength =
            static_cast<std::int32_t>(std::strlen(kLoginProgressText));
        const std::int32_t columnCount = gLoginProgressLayout.columnCount;
        const std::int32_t horizontalWidth = (columnCount * 8 - textLength) * 8;
        const std::int32_t x = gLoginProgressLayout.x;
        detail::DrawText3(
            static_cast<std::int16_t>(horizontalWidth / 2 + x),
            static_cast<std::int16_t>(textY),
            2,
            0,
            kLoginProgressText,
            0);
    }
    return result;
}

} // namespace cg::login
