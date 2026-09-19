#include "cg/LoginState.hpp"

#include "LoginStateDependencies.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/world/SceneState.hpp"

#include <cstring>

namespace {

// 原端对应文本是旧编码字节；新端内部文本按项目约定使用 UTF-8。
constexpr char kLoginTimeoutMessage[] = "處理登入逾時。";
constexpr char kLoginFailureMessage[] = "登入處理失敗，或斷線後，數分鐘內無法登入。";
constexpr char kLoginProgressText[] = "登入中";

// 原 0x9A4E80、0x9A5958、0x9A50D4、0x9A4B90 只由 sub_45EA60 读写。
struct LoginProgressLayout {
    std::int32_t rowCount;
    std::int32_t columnCount;
    std::int32_t y;
    std::int32_t x;
};

LoginProgressLayout gLoginProgressLayout {};

constexpr std::int32_t kProgressActionX = 256;
constexpr std::int32_t kProgressActionY = 192;
constexpr std::int32_t kProgressActionWidth = 128;
constexpr std::int32_t kProgressActionHeight = 96;
constexpr std::int16_t kProgressTextX = 296;
constexpr std::int16_t kProgressTextY = 232;

static_assert(sizeof(kLoginTimeoutMessage) == 22);
static_assert(sizeof(kLoginFailureMessage) == 64);

} // namespace

namespace cg::login {

namespace {

// 原 0x9A6E8C/0x9A6E90 的全部引用都只在 sub_45EA50/sub_45EA60 内。
std::uint16_t gLoginProgressPhase = 0;
YobiAction* gLoginProgressAction = nullptr;

} // namespace

std::uint16_t gValue9A6E3C = 0;
std::uint16_t gValue9A6E3E = 0;
char gValue9A5348[256] {};

static_assert(sizeof(gValue9A5348) == 256);

std::int32_t ResetValues_45D980()
{
    gValue9A6E3C = 0;
    gValue9A6E3E = 0;
    return 0;
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
        detail::Sub_45CE00();
        ++world::gCurrentSceneState;
    }

    const std::int32_t sceneState = world::gCurrentSceneState;
    if (sceneState != 101) {
        return sceneState;
    }

    const std::int32_t result = detail::Sub_45CE10(gValue9A5348);
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
