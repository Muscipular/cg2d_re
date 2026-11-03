#include "cg/network/CgConnection.hpp"
#include "cg/scene/login/LoginConnection.hpp"
#include "cg/scene/login/LoginRequest.hpp"

namespace cg::login {

// 原函数：0x00508350。
std::int32_t ResetServerConnectionState_508350()
{
    cg::network::GetGameConnection().LoginRequests().connectionStep = 0;
    cg::network::GetGameConnection().LoginRequests().connectionErrorPresent = 0;
    return 0;
}

} // namespace cg::login
