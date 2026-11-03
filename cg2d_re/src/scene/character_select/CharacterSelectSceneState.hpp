#pragma once

#include <cstdint>
#include <string>

namespace cg::yobi { struct YobiAction; }
namespace cg::character_select {

// 角色列表请求进度框的阶段、布局及 Yobi 动作借用。
struct CharacterListProgressState {
    std::uint16_t stage = 0; // 原 0x9A6E5C。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E60。
    std::wstring text;
    std::int32_t columns = 0;
    std::int32_t rows = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
};

// 角色删除请求进度框的阶段、布局及 Yobi 动作借用。
struct CharacterDeletionProgressState {
    std::uint16_t stage = 0; // 原 0x9A6E54。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E58。
    std::int32_t x = 0; // 原 0x9A4EA4。
    std::int32_t y = 0; // 原 0x9A5FDC。
    std::int32_t columns = 0; // 原 0x9A50DC。
    std::int32_t rows = 0; // 原 0x9A5950。
};

// 角色认证失败确认框的阶段、布局及 Yobi 动作借用。
struct AuthenticationFailureState {
    std::uint16_t stage = 0; // 原 0x9A6E34。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E38。
    std::int32_t sentinel = -2; // 原 0x55A050。
    std::int32_t width = 0; // 原 0x9A50CC。
    std::int32_t height = 0; // 原 0x9A5E6C。
    std::int32_t x = 0; // 原 0x9A5654。
    std::int32_t y = 0; // 原 0x9A57DC。
};

// 角色选择场景的可变运行期状态；动作本体尚待随场景隔离。
struct CharacterSelectSceneState {
    std::uint8_t pukVersion = 0; // 原 0x9A6E1C；网络回复生产，选角和窗口消费。
    CharacterListProgressState listProgress {};
    CharacterDeletionProgressState deletionProgress {};
    AuthenticationFailureState authenticationFailure {};
};

} // namespace cg::character_select
