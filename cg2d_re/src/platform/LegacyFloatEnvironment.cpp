#include "LegacyFloatEnvironment.hpp"
#include "LegacyFloatFormat.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

namespace cg::platform {

std::int32_t gLegacyFdivBug1085FC0 = 0;

std::uint32_t AbstractLegacyControlWord_5448A4(const std::uint32_t hardware)
{
    std::uint32_t result = 0;
    if ((hardware & 0x01u) != 0) result |= 0x10u;
    if ((hardware & 0x04u) != 0) result |= 0x08u;
    if ((hardware & 0x08u) != 0) result |= 0x04u;
    if ((hardware & 0x10u) != 0) result |= 0x02u;
    if ((hardware & 0x20u) != 0) result |= 0x01u;
    if ((hardware & 0x02u) != 0) result |= 0x80000u;
    switch (hardware & 0xC00u) {
    case 0x400u: result |= 0x100u; break;
    case 0x800u: result |= 0x200u; break;
    case 0xC00u: result |= 0x300u; break;
    }
    switch (hardware & 0x300u) {
    case 0: result |= 0x20000u; break;
    case 0x200u: result |= 0x10000u; break;
    }
    if ((hardware & 0x1000u) != 0) result |= 0x40000u;
    return result;
}

std::uint32_t HardwareLegacyControlWord_544936(const std::uint32_t abstractValue)
{
    std::uint32_t result = 0;
    if ((abstractValue & 0x10u) != 0) result |= 0x01u;
    if ((abstractValue & 0x08u) != 0) result |= 0x04u;
    if ((abstractValue & 0x04u) != 0) result |= 0x08u;
    if ((abstractValue & 0x02u) != 0) result |= 0x10u;
    if ((abstractValue & 0x01u) != 0) result |= 0x20u;
    if ((abstractValue & 0x80000u) != 0) result |= 0x02u;
    switch (abstractValue & 0x300u) {
    case 0x100u: result |= 0x400u; break;
    case 0x200u: result |= 0x800u; break;
    case 0x300u: result |= 0xC00u; break;
    }
    switch (abstractValue & 0x30000u) {
    case 0: result |= 0x300u; break;
    case 0x10000u: result |= 0x200u; break;
    }
    if ((abstractValue & 0x40000u) != 0) result |= 0x1000u;
    return result;
}

std::uint32_t ControlLegacyX87_544859(
    const std::uint32_t value, const std::uint32_t mask)
{
    const std::uint32_t before = AbstractLegacyControlWord_5448A4(
        detail::Cg2dReadLegacyX87Control());
    const std::uint32_t merged = (before & ~mask) | (value & mask);
    const std::uint32_t hardware = HardwareLegacyControlWord_544936(merged);
    detail::Cg2dWriteLegacyX87Control(static_cast<std::uint16_t>(hardware));
    return merged;
}

std::uint32_t ControlLegacyFloatingPoint_54488E(
    const std::uint32_t value, const std::uint32_t mask)
{
    return ControlLegacyX87_544859(value, mask & 0xFFF7FFFFu);
}

void SetLegacyDefaultPrecision_53FD5C()
{
    static_cast<void>(ControlLegacyFloatingPoint_54488E(0x10000u, 0x30000u));
}

std::int32_t TestLegacyFloatingPointDivision_53FD6E()
{
    return detail::Cg2dTestLegacyFdiv();
}

std::int32_t ProbeLegacyFloatingPointDivision_53FDAC()
{
    const HMODULE module = ::GetModuleHandleA("KERNEL32");
    if (module != nullptr) {
        using FeatureQuery = BOOL (WINAPI*)(DWORD);
        const auto query = reinterpret_cast<FeatureQuery>(
            ::GetProcAddress(module, "IsProcessorFeaturePresent"));
        if (query != nullptr) {
            return static_cast<std::int32_t>(query(0));
        }
    }
    return TestLegacyFloatingPointDivision_53FD6E();
}

void InitializeLegacyFloatingPoint_53B447()
{
    InstallLegacyFloatCallbacks_53B45F();
    gLegacyFdivBug1085FC0 = ProbeLegacyFloatingPointDivision_53FDAC();
    SetLegacyDefaultPrecision_53FD5C();
    detail::Cg2dClearLegacyX87Exceptions();
}

} // namespace cg::platform
