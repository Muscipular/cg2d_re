#include "cg/LoginState.hpp"

#include "LoginStateDependencies.hpp"

#include "cg/Blacklist.hpp"
#include "cg/LoginConnection.hpp"
#include "cg/LoginMessage.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/world/SceneState.hpp"

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
