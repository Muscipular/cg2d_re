#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace cg::login {

// 原连续存储 [0x9A620C,0x9A6E0C)，第 i 行对应 0x9A620C + i * 0x100。
// UTL02：LoginScene与消息更新函数共享12行动态UTF-16正文，原行地址保留为历史映射。
extern std::array<std::wstring, 12> gValue9A620C; // 原全局：0x009A620C。

// 原 sub_45CE00@0x45CE00；只清消息内部的 16 位状态。
void ResetLoginMessage();

// 原 sub_45CE10@0x45CE10。
// UTL02：原生入口接收UTF-16；旧窄入口仅转换局部UTF-8视图。
std::int32_t UpdateLoginMessage(std::wstring_view text);
std::int32_t UpdateLoginMessage(const char* text);

} // namespace cg::login
