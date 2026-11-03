#include "cg/network/CgConnection.hpp"
#include "cg/scene/login/LoginConnection.hpp"
#include "cg/scene/login/LoginRequest.hpp"

namespace cg::login {

std::uint32_t gServerConnectionErrorPresent_593FC0 = 0; // 原全局：0x00593FC0。

// 原函数：0x00508350。
std::int32_t ResetServerConnectionState_508350()
{
    cg::network::GetGameConnection().LoginRequests().connectionStep = 0;
    gServerConnectionErrorPresent_593FC0 = 0;
    return 0;
}

} // namespace cg::login
