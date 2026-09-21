#include "cg/LoginConnection.hpp"

namespace cg::login {

std::int32_t gConnectionStep = 0;
std::uint32_t gValue593FC0 = 0;

std::int32_t ResetServerConnectionState_508350()
{
    gConnectionStep = 0;
    gValue593FC0 = 0;
    return 0;
}

} // namespace cg::login
