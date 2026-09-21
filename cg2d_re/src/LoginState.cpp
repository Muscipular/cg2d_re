#include "cg/LoginState.hpp"

#include "LoginStateDependencies.hpp"

#include "cg/Blacklist.hpp"
#include "cg/LoginConnection.hpp"
#include "cg/LoginMessage.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/world/SceneState.hpp"

#include <cstring>

namespace {

// 原端对应文本是旧编码字节；新端内部文本按项目约定使用 UTF-8。
constexpr char kLoginTimeoutMessage[] = "處理登入逾時。";
constexpr char kLoginFailureMessage[] = "登入處理失敗，或斷線後，數分鐘內無法登入。";
constexpr char kLoginProgressText[] = "登入中";
constexpr char kServerConnectingText[] = "伺服器連接中";

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

static_assert(sizeof(kLoginTimeoutMessage) == 22);
static_assert(sizeof(kLoginFailureMessage) == 64);
static_assert(sizeof(kServerConnectingText) == 19);

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
        if (gValue9A6E3E == 0) {
            if (detail::Sub_50FC30() < 0) {
                result = -4;
            } else if (detail::Sub_50FE30() == 1) {
                detail::Sub_50FF10();
                gValue9A6E3E = 1;
            } else {
                result = -3;
            }
        }

        if (gValue9A6E3E == 1) {
            const std::int32_t updateResult = detail::Sub_50FF70();
            if (updateResult == 1) {
                gValue9A6E3E = 2;
                detail::Sub_510120();
                gValue9A6E3C = 2;
            } else if (updateResult == -1) {
                result = -3;
            }
        } else if (gValue9A6E3E == 2) {
            detail::Sub_510120();
            gValue9A6E3C = 2;
        }
        phase = gValue9A6E3C;
    }

    if (phase == 2) {
        detail::Sub_5081A0();
        cg::blacklist::PersistAndReset_401000(gValue55A054);
        if (detail::Sub_508140() != 0) {
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
            detail::Sub_534290();
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
