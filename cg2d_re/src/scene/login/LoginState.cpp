#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include <winsock2.h>
#include <string>
#include "cg/text/Encoding.hpp"
#include <ws2tcpip.h>

#include "cg/scene/login/LoginState.hpp"


#include "LoginStateDependencies.hpp"
#include "../../world/MailState.hpp"

#include "cg/world/Blacklist.hpp"
#include "cg/scene/login/LoginConnection.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/scene/login/LoginScene.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/platformTool.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"

#include <cstring>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053BC0F；直接沿MSVC CRT。

namespace {

// 原 0x55BAD8「登入中」正文 6 字节；字符串字面量隐式追加唯一 NUL。
constexpr wchar_t kLoginProgressText[] = L"登入中";
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


} // namespace

namespace cg::login {

namespace {

// 原 0x9A6E8C/0x9A6E90 的全部引用都只在 sub_45EA50/sub_45EA60 内。
std::uint16_t gLoginProgressPhase = 0;
cg::yobi::YobiAction* gLoginProgressAction = nullptr;

// 原 0x9A5D6C/0x9A6E40 的全部引用都只在 sub_45D990 内。
std::wstring gServerConnectingText {};
cg::yobi::YobiAction* gServerConnectionAction = nullptr;

} // namespace

namespace detail {

// 原 0x44B5C0：日志打开失败返回零；成功返回 fclose 的 W32 位模式。
std::uint32_t Sub_44B5C0(const char* text)
{
    std::FILE* const file = std::fopen("systemlog.txt", "a");
    if (file == nullptr) {
        return 0;
    }

    static_cast<void>(std::fprintf(file, "%s\n", text));
    return static_cast<std::uint32_t>(std::fclose(file));
}

// 原函数：0x00508360。
std::int32_t Sub_508360()
{
    if (network::ReadNetworkRuntimeActive() == 0) {
        return 0;
    }

    std::int32_t step = gConnectionStep;
    if (step == 0) {
        gRequestStartTick = platform::ReadTickCount64();

        char serverAddress[128];
        std::uint16_t port;
        if (GetServerAddressInfo_507D90(
                static_cast<std::int32_t>(gSelectedServerIndex),
                serverAddress,
                &port) < 0) {
            static_cast<void>((gRequestMessage = L"伺服器的名称不正确。"));
            return -2;
        }

        const SOCKET connectionSocket = ::socket(AF_INET, SOCK_STREAM, 0);
        platform::gConnectionSocket =
            static_cast<std::uintptr_t>(connectionSocket);
        if (connectionSocket == INVALID_SOCKET) {
            static_cast<void>((gRequestMessage = L"网路通讯协定（TCP）错误。"));
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
                static_cast<void>(cg::text::FormatTo(gRequestMessage, L"无法取得伺服器的 IP 位址。({})", cg::text::FromGbkWide(serverAddress)));
                static_cast<void>(::closesocket(
                    static_cast<SOCKET>(platform::gConnectionSocket)));
                return -4;
            }
            std::memcpy(
                &address.sin_addr.s_addr,
                host->h_addr_list[0],
                sizeof(address.sin_addr.s_addr));
        }

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
                static_cast<void>((gRequestMessage = L"无法开始处理与伺服器的连接。"));
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
            static_cast<void>((gRequestMessage = L"无法连上伺服器。 "));
            static_cast<void>(::closesocket(
                static_cast<SOCKET>(platform::gConnectionSocket)));
            return -6;
        }
    }
    else if (step >= 71 && step <= 80) {
        if (step == 71) {
            network::WriteProtocolModeDA3B22(0);

            char adapterScratch[16];
            char passwordCopy[sizeof(gPassword)]; // ALM02：密码副本沿既有64字节请求变量容量。
            char adapterId[44];
            static_cast<void>(platform::GetMacAddress_534EF0(adapterScratch));
            std::strcpy(passwordCopy, gPassword);
            std::strcpy(adapterId, adapterScratch);
            std::strcpy(gPassword, passwordCopy);
            const std::int32_t queued = network::Sub_512800(
                platform::gConnectionSocket,
                cg::text::FromGbkWide(gExecutableDigest),
                cg::text::FromGbkWide(passwordCopy),
                cg::text::FromGbkWide(gAccount),
                cg::text::FromGbkWide(adapterId));
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
                static_cast<void>((gRequestMessage = L"无法登入。"));
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

    const std::uint64_t now = platform::ReadTickCount64();
    if (now - gRequestStartTick > 600000U) {
        static_cast<void>((gRequestMessage = L"与伺服器的连接逾时。"));
        return -1;
    }
    return 0;
}

} // namespace detail

std::uint16_t gValue9A6E3C = 0; // 原全局：0x009A6E3C。
std::uint16_t gValue9A6E3E = 0; // 原全局：0x009A6E3E。
std::int32_t gValue55A054 = -1; // 原全局：0x0055A054。
std::wstring gValue9A5348 {}; // 原全局：0x009A5348。


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
        gServerConnectingText = L"伺服器连接中" /* 原 asc_55C7F0 正文 12 字节；字符串字面量隐式追加 NUL。 */;

        std::int32_t columnCount =
            (cg::render::TextMeasure(gServerConnectingText, 0) + 111) / 64;
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
        gServerConnectionAction = cg::yobi::detail::CreateModalAction_5044E0(
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
        cg::world::blacklist::PersistAndReset_401000(gValue55A054);
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
            cg::yobi::detail::MarkDeath(gServerConnectionAction);
            gServerConnectionAction = nullptr;
        }
        cg::world::blacklist::LoadBlacklistFile(gValue55A054);
    }

    cg::yobi::YobiAction* const action = gServerConnectionAction;
    if (action != nullptr && cg::yobi::detail::IsYobiActionReady(action, 1)) {
        const std::int32_t textWidth = cg::render::TextMeasure(gServerConnectingText, 0);
        const std::int32_t rowCount = gServerConnectionLayout.rowCount;
        const std::int32_t verticalHeight = rowCount * 48 - 16;
        const std::int32_t y = gServerConnectionLayout.y;
        const std::int32_t textY = verticalHeight / 2 + y;
        const std::int32_t columnCount = gServerConnectionLayout.columnCount;
        const std::int32_t horizontalWidth = columnCount * 64 - textWidth;
        const std::int32_t x = gServerConnectionLayout.x;
        const std::int32_t textX = horizontalWidth / 2 + x;
        cg::render::DrawText_456780(
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
        cg::world::detail::Sub_532D80();
        ++world::gCurrentSceneState;
    }

    if (world::gCurrentSceneState == 1) {
        const std::int32_t result = UpdateLoginProgress();
        if (result == 1) {
            world::detail::Sub_534290();
            world::SetCurrentSceneType(9);
        } else if (result == -1) {
            world::gCurrentSceneState = 100;
            gValue9A5348 = L"处理登入逾时。" /* 原 0x55C9F8「處理登入逾時。」正文 14 字节；字符串字面量隐式追加唯一 NUL。 */;
        } else if (result == -2) {
            world::gCurrentSceneState = 100;
            gValue9A5348 = L"登入处理失败，或断线后，数分钟内无法登入。" /* 原 0x55C9CC「登入處理失敗，或斷線後，數分鐘內無法登入。」正文 42 字节；字符串字面量隐式追加唯一 NUL。 */;
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
        const std::uint32_t textWidth =
            static_cast<std::uint32_t>(cg::render::TextMeasure(kLoginProgressText, 0));
        gLoginProgressPhase = 1;
        const std::int32_t columnCount =
            static_cast<std::int32_t>((textWidth >> 6U) + 2U);
        gLoginProgressLayout.rowCount = 2;
        gLoginProgressLayout.columnCount = columnCount;
        const std::int32_t actionX = ((10 - columnCount) * 64) / 2;
        gLoginProgressLayout.y = 192;
        const std::int32_t actionWidth = columnCount * 64;
        gLoginProgressLayout.x = actionX;
        gLoginProgressAction = cg::yobi::detail::CreateModalAction_5044E0(
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
            cg::yobi::YobiAction* const action = gLoginProgressAction;
            if (action != nullptr) {
                cg::yobi::detail::MarkDeath(action);
                gLoginProgressAction = nullptr;
            }
            return result;
        }
    }

    cg::yobi::YobiAction* const action = gLoginProgressAction;
    if (action != nullptr && cg::yobi::detail::IsYobiActionReady(action, 1)) {
        const std::int32_t rowCount = gLoginProgressLayout.rowCount;
        const std::int32_t y = gLoginProgressLayout.y;
        const std::int32_t textY = ((rowCount * 48 - 16) / 2) + y;
        const std::int32_t textWidth = cg::render::TextMeasure(kLoginProgressText, 0);
        const std::int32_t columnCount = gLoginProgressLayout.columnCount;
        const std::int32_t horizontalWidth = columnCount * 64 - textWidth;
        const std::int32_t x = gLoginProgressLayout.x;
        cg::render::DrawText3_456560(
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
