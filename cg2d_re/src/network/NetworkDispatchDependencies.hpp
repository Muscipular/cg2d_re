#pragma once

#include <cstdint>

namespace cg::network::detail {

// 512E40尚缺的真实回复目标；仅声明已证调用侧合同，不用空体填充。
// 文本都是按参数编号复制出的借用工作区指针；原callee其他用途继续取证。
void Sub_50C7F0(std::uintptr_t socket, std::int32_t first, std::int32_t second, char* text);
void Sub_50E340(std::uintptr_t socket, char* text);
void Sub_50E360(std::uintptr_t socket, char* text);
void Sub_50E380(std::uintptr_t socket, char* text);
void Sub_50E3A0(std::uintptr_t socket, char* text);
void Sub_50E680(
    std::uintptr_t socket, std::int32_t first, std::int32_t second, std::int32_t third);
void Sub_50E7F0(
    std::uintptr_t socket, std::int32_t first, std::int32_t second,
    char* text3, char* text4, char* text5, char* text6);

} // namespace cg::network::detail
