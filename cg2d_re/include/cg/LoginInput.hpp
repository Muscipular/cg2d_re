#pragma once

#include "cg/Input.hpp"

#include <cstddef>
#include <cstdint>

namespace cg::login {

// 原登录控件固定 8×W32 布局。
struct LoginSceneControl32 {
    std::int32_t drawX;
    std::int32_t drawY;
    std::int32_t left;
    std::int32_t top;
    std::int32_t width;
    std::int32_t height;
    std::int32_t normalImageNo;
    std::int32_t activeImageNo;
};

static_assert(offsetof(LoginSceneControl32, drawX) == 0x00);
static_assert(offsetof(LoginSceneControl32, drawY) == 0x04);
static_assert(offsetof(LoginSceneControl32, left) == 0x08);
static_assert(offsetof(LoginSceneControl32, top) == 0x0C);
static_assert(offsetof(LoginSceneControl32, width) == 0x10);
static_assert(offsetof(LoginSceneControl32, height) == 0x14);
static_assert(offsetof(LoginSceneControl32, normalImageNo) == 0x18);
static_assert(offsetof(LoginSceneControl32, activeImageNo) == 0x1C);
static_assert(sizeof(LoginSceneControl32) == 0x20);

// 两个上下文会被多个场景函数共享，只按原地址区分，不固定账号或密码语义。
extern input::InputContext gInputContext9A5960;
extern input::InputContext gInputContext9A5678;

extern std::int16_t gInputContext9A5960X; // 原 0x9A4B8C，映像零填充。
extern std::int16_t gInputContext9A5960Y; // 原 0x9A4B8A，映像零填充。
extern std::int16_t gInputContext9A5678X; // 原 0x9A4B9E，映像零填充。
extern std::int16_t gInputContext9A5678Y; // 原 0x9A4B9C，映像零填充。
extern std::int32_t gValue9A6E20;         // 原 0x9A6E20，映像零填充。

// 原 0x559FFC 的三项焦点表；x64 下保留原生指针宽度。
extern input::InputContext* gInputContextFocusTable[3];

// 原 sub_5203B0@[0x5203B0,0x5203D6)，写入 0xFF494F 的 51 字节槽。
char* WriteInputValueFF494F_5203B0(char* source);
// 原 sub_5203E0@[0x5203E0,0x520409)，读取 0xFF494F 的 51 字节槽。
char* ReadInputValueFF494F_5203E0(char* output);
// 原 sub_520410@[0x520410,0x520436)，写入 0xFF4982 的 51 字节槽。
char* WriteInputValueFF4982_520410(char* source);
// 原 sub_520440@[0x520440,0x520469)，读取 0xFF4982 的 51 字节槽。
char* ReadInputValueFF4982_520440(char* output);

// 原 sub_5036F0@[0x5036F0,0x50377E)：登录控件命中与鼠标状态位打包。
std::int32_t QueryLoginControlState_5036F0(
    const LoginSceneControl32* control);

// 原 sub_503780@[0x503780,0x5038A9)：登录控件基础图与命中覆盖图绘制。
void DrawLoginControl_503780(
    const LoginSceneControl32* control,
    std::uint8_t layer,
    std::int32_t state,
    std::uint32_t overlayImageNo,
    std::int32_t renderMode);

// 原 sub_45C7C0@[0x45C7C0,0x45C96E)。
void InitializeInputContexts_45C7C0();

} // namespace cg::login
