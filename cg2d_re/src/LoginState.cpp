#include "cg/LoginState.hpp"

#include "LoginStateDependencies.hpp"

#include "cg/world/SceneState.hpp"

#include <cstring>

namespace {

// 原端对应文本是旧编码字节；新端内部文本按项目约定使用 UTF-8。
constexpr char kLoginTimeoutMessage[] = "處理登入逾時。";
constexpr char kLoginFailureMessage[] = "登入處理失敗，或斷線後，數分鐘內無法登入。";

static_assert(sizeof(kLoginTimeoutMessage) == 22);
static_assert(sizeof(kLoginFailureMessage) == 64);

} // namespace

namespace cg::login {

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
        detail::Sub_45EA50();
        detail::Sub_532D80();
        ++world::gCurrentSceneState;
    }

    if (world::gCurrentSceneState == 1) {
        const std::int32_t result = detail::Sub_45EA60();
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

} // namespace cg::login
