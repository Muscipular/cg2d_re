#pragma once

#include <cstdint>

namespace cg::network {
class CgConnection;
}

namespace cg::login {

// 原 sub_508350@0x508350：清零服务器连接共享状态。
std::int32_t ResetServerConnectionState_508350(network::CgConnection& connection);

} // namespace cg::login
