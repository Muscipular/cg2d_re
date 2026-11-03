#pragma once

#include "../../ui/UiControlRecord.hpp"

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
