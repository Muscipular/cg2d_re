#include "cg/scene/login/LoginConnection.hpp"

namespace cg::login {

std::int32_t gConnectionStep = 0; // 原全局：0x00DA3B50。
std::uint32_t gValue593FC0 = 0; // 原全局：0x00593FC0。

// 原函数：0x00508350。
std::int32_t ResetServerConnectionState_508350()
{
    gConnectionStep = 0;
    gValue593FC0 = 0;
    return 0;
}

} // namespace cg::login
