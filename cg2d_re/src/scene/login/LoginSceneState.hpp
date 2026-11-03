#pragma once

#include "../../ui/UiControlRecord.hpp"
#include "cg/input/Input.hpp"

#include <array>
#include <cstdint>
#include <string>

namespace cg::yobi { struct YobiAction; }
namespace cg::login {

// 原 0x9A4E80/0x9A5958/0x9A50D4/0x9A4B90，只由登录进度更新读写。
struct LoginProgressLayout {
    std::int32_t rowCount = 0;
    std::int32_t columnCount = 0;
    std::int32_t y = 0;
    std::int32_t x = 0;
};

// 原 0x9A4E8C/0x9A60E0/0x9A60E4/0x9A4CC0，只由连接提示更新读写。
struct ServerConnectionLayout {
    std::int32_t columnCount = 0;
    std::int32_t rowCount = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
};

// 服务器列表图片号会在分类选择后改写，不能跨登录会话共用。
struct LoginSceneState {
    LoginSceneState() = default;
    LoginSceneState(const LoginSceneState&) = delete;
    LoginSceneState& operator=(const LoginSceneState&) = delete;
    LoginSceneState(LoginSceneState&&) = delete;
    LoginSceneState& operator=(LoginSceneState&&) = delete;

    input::InputContext context9A4D20 {}; // 原 0x9A4D20。
    input::InputContext context9A5960 {}; // 原 0x9A5960。
    input::InputContext context9A5678 {}; // 原 0x9A5678。
    std::int16_t context9A5960X = 0; // 原 0x9A4B8C。
    std::int16_t context9A5960Y = 0; // 原 0x9A4B8A。
    std::int16_t context9A5678X = 0; // 原 0x9A4B9E。
    std::int16_t context9A5678Y = 0; // 原 0x9A4B9C。
    std::int32_t inputFocusIndex = 0; // 原 0x9A6E20。
    std::uint32_t capability3OverlayAlpha = 0; // 原 0x9A6200。
    std::array<input::InputContext*, 3> inputFocusTable {
        &context9A5960, &context9A5678, &context9A5960,
    }; // 原 0x559FFC；借用本实例的两个上下文。
    LoginProgressLayout loginProgressLayout {};
    ServerConnectionLayout serverConnectionLayout {};
    std::uint16_t loginProgressPhase = 0; // 原 0x9A6E8C。
    yobi::YobiAction* loginProgressAction = nullptr; // 原 0x9A6E90。
    std::wstring serverConnectingText; // 原 0x9A5D6C。
    yobi::YobiAction* serverConnectionAction = nullptr; // 原 0x9A6E40。
    std::array<ui::UiControlRecord, 20> serverUi {{
        {{109, 108, 98, 34}, 30151, 30161, 30161},
        {{109, 140, 98, 34}, 30152, 30162, 30162},
        {{109, 172, 98, 34}, 30153, 30163, 30163},
        {{109, 204, 98, 34}, 30154, 30164, 30164},
        {{109, 236, 98, 34}, 30155, 30165, 30165},
        {{209, 108, 98, 34}, 30156, 30166, 30166},
        {{209, 140, 98, 34}, 30157, 30167, 30167},
        {{209, 172, 98, 34}, 30158, 30168, 30168},
        {{209, 204, 98, 34}, 30159, 30169, 30169},
        {{209, 236, 98, 34}, 30160, 30170, 30170},
        {{109, 108, 98, 34}, 30151, 30161, 30161},
        {{109, 140, 98, 34}, 30152, 30162, 30162},
        {{109, 172, 98, 34}, 30153, 30163, 30163},
        {{109, 204, 98, 34}, 30154, 30164, 30164},
        {{109, 236, 98, 34}, 30155, 30165, 30165},
        {{209, 108, 98, 34}, 30156, 30166, 30166},
        {{209, 140, 98, 34}, 30157, 30167, 30167},
        {{209, 172, 98, 34}, 30158, 30168, 30168},
        {{209, 204, 98, 34}, 30159, 30169, 30169},
        {{209, 236, 98, 34}, 30160, 30170, 30170},
    }};
};

} // namespace cg::login
