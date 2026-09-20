#include "cg/platform/LegacyMouse.hpp"

#include "../LoginMessageDependencies.hpp"
#include "MainLoopDependencies.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/MainLoop.hpp"
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

std::int32_t UpdateLegacyMouseLeftDown_506900(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 1u;
    gLegacyMouseState.pulseButtons = pulseButtons | 1u;
    gLegacyMouseState.heldButtons = heldButtons | 1u;

    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[0], x);
    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[1], y);

    const std::uint32_t tick = ReadTickCount32();
    gLegacyMouseState.leftDownTick = tick;
    gLegacyMouseState.leftElapsed = 0;

    const std::int32_t result = TrackLegacyMouseDown_5067F0(x, y, 0);
    if (result < gLegacyMouseTrackerThreshold) {
        return result;
    }
    static_cast<void>(Report_512CA0(gConnectionSocket, 1));
    return ResetLegacyMouseTracker_506580();
}

std::int32_t UpdateLegacyMouseLeftUp_5069E0(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.pulseButtons = pulseButtons & ~1u;
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 4u;
    gLegacyMouseState.heldButtons = heldButtons & ~1u;

    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[2], x);
    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[3], y);

    gLegacyMouseState.leftDownTick = 0;
    gLegacyMouseState.leftElapsed = 0;
    return 0;
}

std::uint32_t UpdateLegacyMouseLeftDoubleClick_506A90(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.pulseButtons = pulseButtons | 1u;
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 0x11u;
    gLegacyMouseState.heldButtons = heldButtons | 1u;

    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[0], x);
    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[1], y);

    const std::uint32_t tick = ReadTickCount32();
    gLegacyMouseState.leftDownTick = tick;
    gLegacyMouseState.leftElapsed = 0;
    return tick;
}

std::int32_t UpdateLegacyMouseRightDown_506B50(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.pulseButtons = pulseButtons | 2u;
    gLegacyMouseState.heldButtons = heldButtons | 2u;
    gLegacyMouseState.edgeButtons = edgeButtons | 2u;

    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[4], x);
    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[5], y);

    const std::uint32_t tick = ReadTickCount32();
    gLegacyMouseState.rightDownTick = tick;
    gLegacyMouseState.rightElapsed = 0;

    const std::int32_t result = TrackLegacyMouseDown_5067F0(x, y, 1);
    if (result < gLegacyMouseTrackerThreshold) {
        return result;
    }
    static_cast<void>(Report_512CA0(gConnectionSocket, 1));
    return ResetLegacyMouseTracker_506580();
}

std::int32_t UpdateLegacyMouseRightUp_506C40(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.pulseButtons = pulseButtons & ~2u;
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 8u;
    gLegacyMouseState.heldButtons = heldButtons & ~2u;

    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[6], x);
    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[7], y);

    gLegacyMouseState.rightDownTick = 0;
    gLegacyMouseState.rightElapsed = 0;
    return 0;
}

std::uint32_t UpdateLegacyMouseRightDoubleClick_506CF0(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.pulseButtons = pulseButtons | 2u;
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 0x22u;
    gLegacyMouseState.heldButtons = heldButtons | 2u;

    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[4], x);
    legacy_crypt_int::AssignFromInt(
        &gLegacyMouseState.buttonCoordinates[5], y);

    const std::uint32_t tick = ReadTickCount32();
    gLegacyMouseState.rightDownTick = tick;
    gLegacyMouseState.rightElapsed = 0;
    return tick;
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
