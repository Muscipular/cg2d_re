#pragma once

#include "cg/Input.hpp"

#include <cstdint>

namespace cg::login {

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

// 原 sub_5203E0@[0x5203E0,0x520409)，读取 0xFF494F 的 51 字节槽。
char* ReadInputValueFF494F_5203E0(char* output);
// 原 sub_520440@[0x520440,0x520469)，读取 0xFF4982 的 51 字节槽。
char* ReadInputValueFF4982_520440(char* output);

// 原 sub_45C7C0@[0x45C7C0,0x45C96E)。
void InitializeInputContexts_45C7C0();

} // namespace cg::login
