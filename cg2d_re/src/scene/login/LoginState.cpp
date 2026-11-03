#include "cg/network/CgConnection.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include <string>
#include <format>
#include "cg/text/Encoding.hpp"

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
#include "cg/platform/Startup.hpp"
#include "cg/world/SceneState.hpp"

#include <cstring>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053BC0F；直接沿MSVC CRT。

namespace cg::login {

// 原 0x55BAD8「登入中」正文 6 字节；字符串字面量隐式追加唯一 NUL。
static constexpr wchar_t kLoginProgressText[] = L"登入中";
// 原 0x9A4E80、0x9A5958、0x9A50D4、0x9A4B90 只由 sub_45EA60 读写。
struct LoginProgressLayout {
    std::int32_t rowCount;
    std::int32_t columnCount;
    std::int32_t y;
    std::int32_t x;
};

static LoginProgressLayout gLoginProgressLayout {};

// 原 0x9A4E8C、0x9A60E0、0x9A60E4、0x9A4CC0 只由 sub_45D990 读写。
struct ServerConnectionLayout {
    std::int32_t columnCount;
    std::int32_t rowCount;
    std::int32_t x;
    std::int32_t y;
};

static ServerConnectionLayout gServerConnectionLayout {};

// 原登录进度布局计算见doc/cg2d_login_progress_layout_sampling_spec.md。

// 原 0x9A6E8C/0x9A6E90 的全部引用都只在 sub_45EA50/sub_45EA60 内。
static std::uint16_t gLoginProgressPhase = 0;
static cg::yobi::YobiAction* gLoginProgressAction = nullptr;

// 原 0x9A5D6C/0x9A6E40 的全部引用都只在 sub_45D990 内。
static std::wstring gServerConnectingText {};
static cg::yobi::YobiAction* gServerConnectionAction = nullptr;

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
std::int32_t AdvanceServerLoginConnection_508360()
{
    if (cg::network::GetGameConnection().Active() == 0) {
        return 0;
    }

    auto& request = cg::network::GetGameConnection().LoginRequests();
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

        cg::network::GetGameConnection().DisconnectState() = 0;
        if (cg::network::GetGameConnection().BeginConnect(serverAddress, port, platform::gNoDelay != 0) == 0) {
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

        const auto connection = cg::network::GetGameConnection().PollConnection();
        if (connection.state == platform::ConnectionState::Connected) {
            request.connectionStep = 71;
            cg::network::GetGameConnection().SetIoEnabled(true);
            static_cast<void>(AppendSystemLog_44B5C0(L"CONNECT OK"));
        }
        else if (connection.state == platform::ConnectionState::Failed
            || connection.state == platform::ConnectionState::Closed) {
            const auto diagnostic = std::format(L"CONNECT ERROR={} PHASE={} ENDPOINT={} ATTEMPT={}",
                connection.error, static_cast<int>(connection.phase), connection.endpoint, connection.attempt);
            static_cast<void>(AppendSystemLog_44B5C0(diagnostic));
            cg::network::GetGameConnection().Close();
            if (connection.phase == platform::ConnectionPhase::Resolve) {
                static_cast<void>(cg::text::FormatTo(request.message,
                    L"无法取得伺服器的 IP 位址。({})", connection.host));
                return -4;
            }
            if (connection.phase == platform::ConnectionPhase::Setup) {
                request.message = L"网路通讯协定（TCP）错误。";
                return -3;
            }
            if (connection.phase == platform::ConnectionPhase::SocketOptions) {
                request.message = L"网路通讯协定（TCP）设置错误。";
                return -100;
            }
            if (connection.phase == platform::ConnectionPhase::ConnectStart) {
                request.message = L"无法开始处理与伺服器的连接。";
                return -5;
            }
            request.message = L"无法连上伺服器。 ";
            return -6;
        }
    }
    else if (step >= 71 && step <= 80) {
        if (step == 71) {
            cg::network::GetGameConnection().ResetLoginReply();

            std::wstring adapterScratch;
            std::wstring passwordCopy;
            std::wstring adapterId;
            static_cast<void>(platform::GetMacAddress_534EF0(adapterScratch));
            passwordCopy = request.password;
            adapterId = adapterScratch;
            request.password = passwordCopy;
            auto& connection = network::GetGameConnection();
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
            if (cg::network::GetGameConnection().LoginAccepted() == 0) {
                request.status = 0;
                static_cast<void>((request.message = L"无法登入。"));
                cg::network::GetGameConnection().Close();
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
        cg::network::GetGameConnection().Close();
        static_cast<void>((request.message = L"与伺服器的连接逾时。"));
        return -1;
    }
    return 0;
}

} // namespace detail

// 原函数：0x0045D980。
std::int32_t ResetValues_45D980()
{
    auto& request = cg::network::GetGameConnection().LoginRequests();
    request.serverConnectionPhase = 0;
    return 0;
}

// 原函数：0x0045D990。
std::int32_t UpdateServerConnection()
{
    auto& request = cg::network::GetGameConnection().LoginRequests();
    std::uint16_t phase = request.serverConnectionPhase;
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
        gServerConnectionAction = cg::yobi::CreateModalAction_5044E0(
            x,
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
        cg::network::GetGameConnection().Shutdown();
        cg::network::GetGameConnection().DisconnectState() = 0;
        cg::world::blacklist::PersistAndReset_401000(request.selectedLoginServerGroup);
        if (cg::network::GetGameConnection().Initialize()) {
            cg::network::GetGameConnection().DisconnectState() = 0;
            request.serverConnectionPhase = 3;
            ResetServerConnectionState_508350();
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
        ResetServerConnectionState_508350();
        request.serverConnectionPhase = 4;
        phase = 4;
    }

    if (phase == 4) {
        const std::int32_t updateResult = detail::AdvanceServerLoginConnection_508360();
        if (updateResult == 1) {
            result = 1;
        } else if (updateResult < 0) {
            result = -2;
        }
    }

    if (result != 0) {
        if (gServerConnectionAction != nullptr) {
            cg::yobi::MarkDeath(gServerConnectionAction);
            gServerConnectionAction = nullptr;
        }
        cg::world::blacklist::LoadBlacklistFile(request.selectedLoginServerGroup);
    }

    cg::yobi::YobiAction* const action = gServerConnectionAction;
    if (action != nullptr && cg::yobi::IsYobiActionReady(action, 1)) {
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
    auto& request = cg::network::GetGameConnection().LoginRequests();
    if (world::gCurrentSceneState == 0) {
        ResetLoginProgress();
        cg::world::ResetGuildStateAndCloseDetails_532D80();
        ++world::gCurrentSceneState;
    }

    if (world::gCurrentSceneState == 1) {
        const std::int32_t result = UpdateLoginProgress();
        if (result == 1) {
            world::detail::ResendSavedPlayerProfile_534290();
            world::SetCurrentSceneType(9);
        } else if (result == -1) {
            world::gCurrentSceneState = 100;
            request.processingErrorMessage = L"处理登入逾时。" /* 原 0x55C9F8「處理登入逾時。」正文 14 字节；字符串字面量隐式追加唯一 NUL。 */;
        } else if (result == -2) {
            world::gCurrentSceneState = 100;
            request.processingErrorMessage = L"登入处理失败，或断线后，数分钟内无法登入。" /* 原 0x55C9CC「登入處理失敗，或斷線後，數分鐘內無法登入。」正文 42 字节；字符串字面量隐式追加唯一 NUL。 */;
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

    const std::int32_t result = UpdateLoginMessage(request.processingErrorMessage);
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
        const std::int32_t actionWidth = columnCount * 64;
        const std::int32_t actionHeight = gLoginProgressLayout.rowCount * 48;
        // 原45EA60按640×480居中；本场景无LegacyUiLayoutScope，改用当前画布尺寸。
        const std::int32_t actionX = (platform::gWindowWidth - actionWidth) / 2;
        const std::int32_t actionY = (platform::gWindowHeight - actionHeight) / 2;
        gLoginProgressLayout.x = actionX;
        gLoginProgressLayout.y = actionY;
        gLoginProgressAction = cg::yobi::CreateModalAction_5044E0(
            actionX,
            actionY,
            actionWidth,
            actionHeight,
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
                cg::yobi::MarkDeath(action);
                gLoginProgressAction = nullptr;
            }
            return result;
        }
    }

    cg::yobi::YobiAction* const action = gLoginProgressAction;
    if (action != nullptr && cg::yobi::IsYobiActionReady(action, 1)) {
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
