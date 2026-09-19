#include "cg/LoginState.hpp"

namespace cg::login {

std::uint16_t gValue9A6E3C = 0;
std::uint16_t gValue9A6E3E = 0;

std::int32_t ResetValues_45D980()
{
    gValue9A6E3C = 0;
    gValue9A6E3E = 0;
    return 0;
}

} // namespace cg::login
