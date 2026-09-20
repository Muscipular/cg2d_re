#include "cg/platform/LegacyMouse.hpp"

#include "../LoginMessageDependencies.hpp"
#include "cg/platform/Startup.hpp"

#include <cstring>

namespace cg::platform {

LegacyMouseState gLegacyMouseState {};
std::uint32_t gValueD68BF0 = 0;
std::uint32_t gValueD68DC8 = 0;

namespace {

struct LegacyMouseStaticInitializer {
    LegacyMouseStaticInitializer()
    {
        legacy_crypt_int::ConstructZero_401C70(&gLegacyMouseState.mouseX);
        legacy_crypt_int::ConstructZero_401C70(&gLegacyMouseState.mouseY);
        for (LegacyCryptInt& coordinate : gLegacyMouseState.buttonCoordinates) {
            legacy_crypt_int::ConstructZero_401C70(&coordinate);
        }
    }
};

const LegacyMouseStaticInitializer gLegacyMouseStaticInitializer;

} // namespace

std::int32_t ResetLegacyMouseState_506870()
{
    std::memset(&gLegacyMouseState, 0, sizeof(gLegacyMouseState));
    gValueD68DC4 = 0;
    gLegacyMouseState.valueD68DB8 = -1;
    return 0;
}

void ResetLegacyMousePosition_506890(const std::int32_t x, const std::int32_t y)
{
    legacy_crypt_int::AssignFromInt(&gLegacyMouseState.mouseX, x);
    legacy_crypt_int::AssignFromInt(&gLegacyMouseState.mouseY, y);
}

} // namespace cg::platform

namespace cg::login::detail {

std::uint32_t& gValueD68D98 = platform::gLegacyMouseState.heldButtons;
std::uint32_t& gValueD68D9C = platform::gLegacyMouseState.edgeButtons;
std::uint32_t& gValueD68DA0 = platform::gLegacyMouseState.pulseButtons;

std::int32_t ReadLegacyMouseX()
{
    return platform::legacy_crypt_int::Read_401EC0(
        &platform::gLegacyMouseState.mouseX);
}

std::int32_t ReadLegacyMouseY()
{
    return platform::legacy_crypt_int::Read_401EC0(
        &platform::gLegacyMouseState.mouseY);
}

} // namespace cg::login::detail
