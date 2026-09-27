#pragma once

#include <cstdint>

namespace cg::login {

// 原连续存储 [0x9A620C,0x9A6E0C)，第 i 行对应 0x9A620C + i * 0x100。
// LoginScene::Process 与消息更新函数共享这一份 12 * 256 字节存储。
extern char gValue9A620C[12][256];

// 原 sub_45CE00@0x45CE00；只清消息内部的 16 位状态。
void ResetLoginMessage();

// 原 sub_45CE10@0x45CE10。
// 前提：text 非空、以 NUL 结尾，原编码字节长度不超过 255；本接口不转码。
std::int32_t UpdateLoginMessage(const char* text);

} // namespace cg::login
