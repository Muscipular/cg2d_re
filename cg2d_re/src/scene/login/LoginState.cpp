#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include <string>
#include <format>
#include "cg/text/Encoding.hpp"

#include "cg/scene/login/LoginState.hpp"

#include "LoginStateDependencies.hpp"
#include "LoginSceneState.hpp"
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
#include "cg/platform/Startup.hpp"
#include "cg/world/SceneState.hpp"

#include <cstring>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053BC0F；直接沿MSVC CRT。

namespace cg::login {

// 原 0x55BAD8「登入中」正文 6 字节；字符串字面量隐式追加唯一 NUL。
static constexpr wchar_t kLoginProgressText[] = L"登入中";
// 原登录进度布局计算见doc/cg2d_login_progress_layout_sampling_spec.md。

namespace detail {

// 原 0x44B5C0：日志打开失败返回零；成功返回 fclose 的 W32 位模式。
std::uint32_t AppendSystemLog_44B5C0(std::wstring_view text)
{
    std::FILE* const file = _wfopen(L"systemlog.txt", L"a");
    if (file == nullptr) {
        return 0;
    }

    static_cast<void>(std::fprintf(file, "%s\n", cg::text::ToGbk(text).c_str()));
    return static_cast<std::uint32_t>(std::fclose(file));
}

// 原函数：0x00508360。
std::int32_t AdvanceServerLoginConnection_508360(network::CgConnection& connection)
{
    if (connection.Active() == 0) {
        return 0;
    }

    auto& request = connection.LoginRequests();
    std::int32_t step = request.connectionStep;
    if (step == 0) {
        request.startTick = platform::ReadTickCount64();

        std::wstring serverAddress;
        std::uint16_t port;
        if (GetServerAddressInfo_507D90(
                static_cast<std::int32_t>(request.selectedServerIndex),
                serverAddress,
                &port) < 0) {
            static_cast<void>((request.message = L"伺服器的名称不正确。"));
            return -2;
        }

        connection.DisconnectState() = 0;
        if (connection.BeginConnect(serverAddress, port, platform::gNoDelay != 0) == 0) {
            static_cast<void>((request.message = L"网路通讯协定（TCP）错误。"));
            return -3;
        }
        request.connectionStep = 1;
    }
    else if (step >= 1 && step <= 70) {
        ++step;
        request.connectionStep = step;
        if (step == 70) {
            request.connectionStep = 69;
        }

        const auto connectionState = connection.PollConnection();
        if (connectionState.state == platform::ConnectionState::Connected) {
            request.connectionStep = 71;
            connection.SetIoEnabled(true);
            static_cast<void>(AppendSystemLog_44B5C0(L"CONNECT OK"));
        }
        else if (connectionState.state == platform::ConnectionState::Failed
            || connectionState.state == platform::ConnectionState::Closed) {
            const auto diagnostic = std::format(L"CONNECT ERROR={} PHASE={} ENDPOINT={} ATTEMPT={}",
                connectionState.error, static_cast<int>(connectionState.phase), connectionState.endpoint, connectionState.attempt);
            static_cast<void>(AppendSystemLog_44B5C0(diagnostic));
            connection.Close();
            if (connectionState.phase == platform::ConnectionPhase::Resolve) {
                static_cast<void>(cg::text::FormatTo(request.message,
                    L"无法取得伺服器的 IP 位址。({})", connectionState.host));
                return -4;
            }
            if (connectionState.phase == platform::ConnectionPhase::Setup) {
                request.message = L"网路通讯协定（TCP）错误。";
                return -3;
            }
            if (connectionState.phase == platform::ConnectionPhase::SocketOptions) {
                request.message = L"网路通讯协定（TCP）设置错误。";
                return -100;
            }
            if (connectionState.phase == platform::ConnectionPhase::ConnectStart) {
                request.message = L"无法开始处理与伺服器的连接。";
                return -5;
            }
            request.message = L"无法连上伺服器。 ";
            return -6;
        }
    }
    else if (step >= 71 && step <= 80) {
        if (step == 71) {
            connection.ResetLoginReply();

            std::wstring adapterScratch;
            std::wstring passwordCopy;
            std::wstring adapterId;
            static_cast<void>(platform::GetMacAddress_534EF0(adapterScratch));
            passwordCopy = request.password;
            adapterId = adapterScratch;
            request.password = passwordCopy;
            const auto networkSession = connection.CaptureSession();
            // 登录业务先发XCNY并替换旧待发内容，成功后才追加账号登录请求。
            constexpr char loginPrelude[] = "XCNY 3 \n";
            auto queued = connection.SendPacket(loginPrelude,
                static_cast<std::uint32_t>(sizeof(loginPrelude) - 1), true);
            if (queued == network::SendResult::Queued) {
                queued = connection.SendAccountLoginRequest(gExecutableDigest,
                    passwordCopy,
                    request.account,
                    adapterId);
            }
            // 原508748：登录发送返回后先重读阶段，再发布请求状态。
            step = request.connectionStep;
            request.status = 1;
            const auto diagnostic = std::format(L"LOGIN QUEUE RESULT={}", static_cast<std::int32_t>(queued));
            static_cast<void>(AppendSystemLog_44B5C0(diagnostic));
        }
        else if (request.status == 2) {
            if (connection.LoginAccepted() == 0) {
                request.status = 0;
                static_cast<void>((request.message = L"无法登入。"));
                connection.Close();
                return -7;
            }
            step = 81;
        }

        ++step;
        request.connectionStep = step;
        if (step == 81) {
            request.connectionStep = 80;
        }
    }
    else if (step >= 81 && step <= 98) {
        ++step;
        request.connectionStep = step;
    }
    else if (step == 99) {
        request.status = 0;
        return 1;
    }

    const std::uint64_t now = platform::ReadTickCount64();
    if (now - request.startTick > 600000U) {
        connection.Close();
        static_cast<void>((request.message = L"与伺服器的连接逾时。"));
        return -1;
    }
    return 0;
}

} // namespace detail

// 原函数：0x0045D980。
std::int32_t ResetValues_45D980(network::CgConnection& connection)
{
    auto& request = connection.LoginRequests();
    request.serverConnectionPhase = 0;
    return 0;
}

// 原函数：0x0045D990。
std::int32_t UpdateServerConnection(network::CgConnection& connection)
{
    auto& request = connection.LoginRequests();
    auto& sceneUi = connection.LoginSceneUi();
    std::uint16_t phase = request.serverConnectionPhase;
    std::int32_t result = 0;

    if (phase == 0) {
        sceneUi.serverConnectingText = L"伺服器连接中" /* 原 asc_55C7F0 正文 12 字节；字符串字面量隐式追加 NUL。 */;

        std::int32_t columnCount =
            (cg::render::TextMeasure(sceneUi.serverConnectingText, 0) + 111) / 64;
        sceneUi.serverConnectionLayout.columnCount = columnCount;
        if (columnCount < 2) {
            columnCount = 2;
            sceneUi.serverConnectionLayout.columnCount = columnCount;
        }

        const std::int32_t x = ((10 - columnCount) * 64) / 2;
        const std::int32_t width = columnCount * 64;
        sceneUi.serverConnectionLayout.rowCount = 2;
        sceneUi.serverConnectionLayout.x = x;
        sceneUi.serverConnectionLayout.y = 192;
        sceneUi.serverConnectionAction = cg::yobi::CreateModalAction_5044E0(
            connection.Owner(), x,
            192,
            width,
            96,
            4,
            1);
        request.serverConnectionPhase = 2;
        phase = 2;
    }

    if (phase == 1) {
        request.serverConnectionPhase = 2;
        phase = 2;
    }

    if (phase == 2) {
        connection.Shutdown();
        connection.DisconnectState() = 0;
        cg::world::blacklist::PersistAndReset_401000(connection.Owner(), request.selectedLoginServerGroup);
        if (connection.Initialize()) {
            connection.DisconnectState() = 0;
            request.serverConnectionPhase = 3;
            ResetServerConnectionState_508350(connection);
            request.serverConnectionPhase = 4;
            phase = 4;
        } else {
            result = -1;
            phase = request.serverConnectionPhase;
        }
    } else {
        phase = request.serverConnectionPhase;
    }

    if (phase == 3) {
        ResetServerConnectionState_508350(connection);
        request.serverConnectionPhase = 4;
        phase = 4;
    }

    if (phase == 4) {
        const std::int32_t updateResult = detail::AdvanceServerLoginConnection_508360(connection);
        if (updateResult == 1) {
            result = 1;
        } else if (updateResult < 0) {
            result = -2;
        }
    }

    if (result != 0) {
        if (sceneUi.serverConnectionAction != nullptr) {
            cg::yobi::MarkDeath(sceneUi.serverConnectionAction);
            sceneUi.serverConnectionAction = nullptr;
        }
        cg::world::blacklist::LoadBlacklistFile(connection.Owner(), request.selectedLoginServerGroup);
    }

    cg::yobi::YobiAction* const action = sceneUi.serverConnectionAction;
    if (action != nullptr && cg::yobi::IsYobiActionReady(action, 1)) {
        const std::int32_t textWidth = cg::render::TextMeasure(sceneUi.serverConnectingText, 0);
        const std::int32_t rowCount = sceneUi.serverConnectionLayout.rowCount;
        const std::int32_t verticalHeight = rowCount * 48 - 16;
        const std::int32_t y = sceneUi.serverConnectionLayout.y;
        const std::int32_t textY = verticalHeight / 2 + y;
        const std::int32_t columnCount = sceneUi.serverConnectionLayout.columnCount;
        const std::int32_t horizontalWidth = columnCount * 64 - textWidth;
        const std::int32_t x = sceneUi.serverConnectionLayout.x;
        const std::int32_t textX = horizontalWidth / 2 + x;
        cg::render::DrawText_456780(
            static_cast<std::int16_t>(textX),
            static_cast<std::int16_t>(textY),
            2,
            0,
            0,
            sceneUi.serverConnectingText,
            0,
            0);
    }
    return result;
}

// 原函数：0x0045E960。
std::int32_t ProcessState_45E960(network::CgConnection& connection)
{
    auto& request = connection.LoginRequests();
    if (connection.Owner().WorldScene().sceneState == 0) {
        ResetLoginProgress(connection);
        cg::world::ResetGuildStateAndCloseDetails_532D80(connection.Owner());
        ++connection.Owner().WorldScene().sceneState;
    }

    if (connection.Owner().WorldScene().sceneState == 1) {
        const std::int32_t result = UpdateLoginProgress(connection);
        if (result == 1) {
            world::detail::ResendSavedPlayerProfile_534290(connection.Owner());
            world::SetCurrentSceneType(connection.Owner(), 9);
        } else if (result == -1) {
            connection.Owner().WorldScene().sceneState = 100;
            request.processingErrorMessage = L"处理登入逾时。" /* 原 0x55C9F8「處理登入逾時。」正文 14 字节；字符串字面量隐式追加唯一 NUL。 */;
        } else if (result == -2) {
            connection.Owner().WorldScene().sceneState = 100;
            request.processingErrorMessage = L"登入处理失败，或断线后，数分钟内无法登入。" /* 原 0x55C9CC「登入處理失敗，或斷線後，數分鐘內無法登入。」正文 42 字节；字符串字面量隐式追加唯一 NUL。 */;
        }
    }

    if (connection.Owner().WorldScene().sceneState == 100) {
        ResetLoginMessage(connection);
        ++connection.Owner().WorldScene().sceneState;
    }

    const std::int32_t sceneState = connection.Owner().WorldScene().sceneState;
    if (sceneState != 101) {
        return sceneState;
    }

    const std::int32_t result = UpdateLoginMessage(connection, request.processingErrorMessage);
    if (result == 0) {
        return 0;
    }
    return world::SetCurrentSceneType(connection.Owner(), 2);
}

// 原函数：0x0045EA50。
void ResetLoginProgress(network::CgConnection& connection)
{
    auto& sceneUi = connection.LoginSceneUi();
    sceneUi.loginProgressPhase = 0;
}

// 原函数：0x0045EA60。
std::int32_t UpdateLoginProgress(network::CgConnection& connection)
{
    auto& sceneUi = connection.LoginSceneUi();
    std::uint16_t phase = sceneUi.loginProgressPhase;
    std::int32_t result = 0;

    if (phase == 0) {
        const std::uint32_t textWidth =
            static_cast<std::uint32_t>(cg::render::TextMeasure(kLoginProgressText, 0));
        sceneUi.loginProgressPhase = 1;
        const std::int32_t columnCount =
            static_cast<std::int32_t>((textWidth >> 6U) + 2U);
        sceneUi.loginProgressLayout.rowCount = 2;
        sceneUi.loginProgressLayout.columnCount = columnCount;
        const std::int32_t actionWidth = columnCount * 64;
        const std::int32_t actionHeight = sceneUi.loginProgressLayout.rowCount * 48;
        // 原45EA60按640×480居中；本场景无LegacyUiLayoutScope，改用当前画布尺寸。
        const std::int32_t actionX = (platform::gWindowWidth - actionWidth) / 2;
        const std::int32_t actionY = (platform::gWindowHeight - actionHeight) / 2;
        sceneUi.loginProgressLayout.x = actionX;
        sceneUi.loginProgressLayout.y = actionY;
        sceneUi.loginProgressAction = cg::yobi::CreateModalAction_5044E0(
            connection.Owner(), actionX,
            actionY,
            actionWidth,
            actionHeight,
            4,
            1);
        phase = sceneUi.loginProgressPhase;
    }

    if (phase == 1) {
        StartLoginRequest(connection);
        ++sceneUi.loginProgressPhase;
    } else if (phase == 2) {
        result = PollLoginRequest(connection);
        if (result != 0) {
            cg::yobi::YobiAction* const action = sceneUi.loginProgressAction;
            if (action != nullptr) {
                cg::yobi::MarkDeath(action);
                sceneUi.loginProgressAction = nullptr;
            }
            return result;
        }
    }

    cg::yobi::YobiAction* const action = sceneUi.loginProgressAction;
    if (action != nullptr && cg::yobi::IsYobiActionReady(action, 1)) {
        const std::int32_t rowCount = sceneUi.loginProgressLayout.rowCount;
        const std::int32_t y = sceneUi.loginProgressLayout.y;
        const std::int32_t textY = ((rowCount * 48 - 16) / 2) + y;
        const std::int32_t textWidth = cg::render::TextMeasure(kLoginProgressText, 0);
        const std::int32_t columnCount = sceneUi.loginProgressLayout.columnCount;
        const std::int32_t horizontalWidth = columnCount * 64 - textWidth;
        const std::int32_t x = sceneUi.loginProgressLayout.x;
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
