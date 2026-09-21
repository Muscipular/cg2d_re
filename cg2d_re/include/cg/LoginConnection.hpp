#pragma once

#include <cstdint>

namespace cg::login {

// 原 0xDA3B50，服务器连接步骤/轮询计数，按 32 位访问。
extern std::int32_t gConnectionStep;

// 原 0x593FC0，特殊协议异常与断线展示路径共享，按 32 位访问。
extern std::uint32_t gValue593FC0;

// 原 sub_508350@0x508350：清零服务器连接共享状态。
std::int32_t ResetServerConnectionState_508350();

} // namespace cg::login
