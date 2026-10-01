#pragma once

#include <cstdint>

namespace cg::login::detail {

// 原 4FCBA0 设置表的无参回调；返回完整 EAX32=0。
std::int32_t Sub_4FD4C0();
std::int32_t Sub_4FD510();
std::int32_t Sub_4FD550();
std::int32_t Sub_4FD5A0();
std::int32_t Sub_4FD5C0();
std::int32_t Sub_4FD5F0();
std::int32_t Sub_4FD640();
std::int32_t Sub_4FE3E0();

// 原4FE4A0鼠标设置表：速度为S32，模式沿现有W32状态，均返回0。
std::int32_t Sub_4FE3F0(); // 553438速度增加，原上界100。
std::int32_t Sub_4FE440(); // 553438速度减少，原下界1。
std::int32_t Sub_4FEAC0(); // D67AE4切换并调用原生光标边界。
std::int32_t Sub_4FEFF0(); // D67AE8物品移动方式切换。
std::int32_t Sub_4FF310(); // 554B74低U8切换后写完整W16，返回0。

} // namespace cg::login::detail
