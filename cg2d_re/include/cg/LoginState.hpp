#pragma once

#include <cstdint>

namespace cg::login {

// 原 sub_45D980@0x45D980 与其后继 sub_45D990 仅按 16 位访问这两个存储。
extern std::uint16_t gValue9A6E3C; // 原 0x9A6E3C，映像初值 0。
extern std::uint16_t gValue9A6E3E; // 原 0x9A6E3E，映像初值 0。

// 原 sub_45D980@0x45D980；依次清零两个 16 位存储并返回 0。
std::int32_t ResetValues_45D980();

} // namespace cg::login
