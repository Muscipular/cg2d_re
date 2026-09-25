#include "cg/DisplaySettings.hpp"

#include "LoginMessageDependencies.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include "cg/platform/Startup.hpp"

namespace cg::login::detail {

std::int32_t Sub_4FD4C0()
{
    if (platform::gValue55463C < 512) {
        ++platform::gValue55463C;
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    return 0;
}

std::int32_t Sub_4FD510()
{
    if (platform::gValue55463C > 0) {
        --platform::gValue55463C;
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    return 0;
}

std::int32_t Sub_4FD5C0()
{
    platform::gValue554644 = static_cast<std::int32_t>(
        (static_cast<std::uint32_t>(platform::gValue554644) - 1u) & 1u);
    static_cast<void>(PlaySe(51, 320, 240));
    return 0;
}

std::int32_t Sub_4FD5F0()
{
    if (platform::gValue554640 < 5) {
        ++platform::gValue554640;
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    world::detail::gValue626BFC = 180;
    return 0;
}

std::int32_t Sub_4FD640()
{
    if (platform::gValue554640 > 1) {
        --platform::gValue554640;
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    world::detail::gValue626BFC = 180;
    return 0;
}

} // namespace cg::login::detail
