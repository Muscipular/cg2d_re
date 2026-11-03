#include "cg/network/CgConnection.hpp"
#include "cg/scene/login/LoginConnection.hpp"
#include "cg/scene/login/LoginRequest.hpp"

namespace cg::login {

// 原函数：0x00508350。
std::int32_t ResetServerConnectionState_508350(network::CgConnection& connection)
{
    connection.LoginRequests().connectionStep = 0;
    connection.LoginRequests().connectionErrorPresent = 0;
    return 0;
}

} // namespace cg::login
