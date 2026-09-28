#pragma once

#include <cstdint>

namespace cg::network::detail {

// 512E40尚缺的真实回复目标；仅声明已证调用侧合同，不用空体填充。
// 文本都是按参数编号复制出的借用工作区指针；原callee其他用途继续取证。
void Sub_50C7F0(std::uintptr_t socket, std::int32_t first, std::int32_t second, char* text);
void Sub_50D500(std::uintptr_t socket, char* text);
void Sub_50D740(std::uintptr_t socket, char* text);
void Sub_50D8C0(std::uintptr_t socket, char* text);
void Sub_50D9E0(std::uintptr_t socket, std::int32_t value, char* text);
void Sub_50DC70(std::uintptr_t socket, std::int32_t first, char* text, std::int32_t third);
void Sub_50E110(
    std::uintptr_t socket, std::int32_t first, std::int32_t second,
    std::int32_t third, std::int32_t fourth, char* text);
void Sub_50E340(std::uintptr_t socket, char* text);
void Sub_50E360(std::uintptr_t socket, char* text);
void Sub_50E380(std::uintptr_t socket, char* text);
void Sub_50E3A0(std::uintptr_t socket, char* text);
void Sub_50E3C0(std::uintptr_t socket, char* text);
void Sub_50E3F0(std::uintptr_t socket, char* text, std::int32_t value);
void Sub_50E680(
    std::uintptr_t socket, std::int32_t first, std::int32_t second, std::int32_t third);
void Sub_50E6E0(std::uintptr_t socket, char* text);
void Sub_50E7D0(std::uintptr_t socket, std::int32_t value);
void Sub_50E7E0(std::uintptr_t socket, std::int32_t value);
void Sub_50E7F0(
    std::uintptr_t socket, std::int32_t first, std::int32_t second,
    char* text3, char* text4, char* text5, char* text6);
void Sub_50E820(std::uintptr_t socket, std::int32_t value);
void Sub_50E850(std::uintptr_t socket, char* text);
void Sub_50EB40(std::uintptr_t socket, char* text);
void Sub_50F280(std::uintptr_t socket, std::int32_t value);
void Sub_50F2A0(std::uintptr_t socket, char* text);
void Sub_50F3A0(std::uintptr_t socket, char* text);
void Sub_50F430(std::uintptr_t socket, char* text);
void Sub_50F450(
    std::uintptr_t socket, char* text1, std::int32_t second,
    char* text3, char* text4, char* text5, std::int32_t sixth,
    std::int32_t seventh, char* text8, std::int32_t ninth,
    char* text10, std::int32_t eleventh, char* text12, char* text13);
void Sub_50F7F0(std::uintptr_t socket, std::int32_t first, char* text, std::int32_t third);
void Sub_50F8E0(std::uintptr_t socket, char* text);
void Sub_50F990(
    std::uintptr_t socket, std::int32_t first, std::int32_t second,
    std::int32_t third, std::int32_t fourth, char* text);
void Sub_50F9E0(std::uintptr_t socket, char* text);

} // namespace cg::network::detail
