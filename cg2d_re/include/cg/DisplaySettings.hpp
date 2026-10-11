#pragma once

#include <cstdint>

namespace cg::login::detail {

// 原 4FCBA0 设置表的无参回调；返回完整 EAX32=0。
// 原函数：0x004FD4C0。
std::int32_t Sub_4FD4C0();
// 原函数：0x004FD510。
std::int32_t Sub_4FD510();
// 原函数：0x004FD550。
std::int32_t Sub_4FD550();
// 原函数：0x004FD5A0。
std::int32_t Sub_4FD5A0();
// 原函数：0x004FD5C0。
std::int32_t Sub_4FD5C0();
// 原函数：0x004FD5F0。
std::int32_t Sub_4FD5F0();
// 原函数：0x004FD640。
std::int32_t Sub_4FD640();
// 原函数：0x004FE3E0。
std::int32_t Sub_4FE3E0();

// 原旧7/旧8音频设置回调；复用platform音量/立体声状态与实际SDL播放链。
// 原函数：0x004FDD70。
std::int32_t Sub_4FDD70(); // 音乐音量增加，写58D3CC后调用52C3D0，再播放提示音。
// 原函数：0x004FDDC0。
std::int32_t Sub_4FDDC0(); // 音乐音量减少，原下界0；边界只播放55号音效。
// 原函数：0x004FE320。
std::int32_t Sub_4FE320(); // 音效音量增加，写58D3C8；不调用音乐音量应用。
// 原函数：0x004FE370。
std::int32_t Sub_4FE370(); // 音效音量减少，原下界0；成功播放51号音效。
// 原函数：0x004FE3B0。
std::int32_t Sub_4FE3B0(); // 58D3C4完整S32立体声状态切换，返回0。

// 原4FE4A0鼠标设置表：速度为S32，模式沿现有W32状态，均返回0。
// 原函数：0x004FE3F0。
std::int32_t Sub_4FE3F0(); // 553438速度增加，原上界100。
// 原函数：0x004FE440。
std::int32_t Sub_4FE440(); // 553438速度减少，原下界1。
// 原函数：0x004FEAC0。
std::int32_t Sub_4FEAC0(); // D67AE4切换并调用原生光标边界。
// 原函数：0x004FEFF0。
std::int32_t Sub_4FEFF0(); // D67AE8物品移动方式切换。
// 原函数：0x004FF310。
std::int32_t Sub_4FF310(); // 554B74低U8切换后写完整W16，返回0。
// 原函数：0x004FF620。
std::int32_t Sub_4FF620(); // D68858完整W32零/非零切换，返回0。

} // namespace cg::login::detail
