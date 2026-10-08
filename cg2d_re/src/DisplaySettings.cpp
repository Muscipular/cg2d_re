#include "cg/DisplaySettings.hpp"

#include "LoginMessageDependencies.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include "cg/Input.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyWindow.hpp"
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

std::int32_t Sub_4FD550()
{
    std::int32_t& color = input::gInputContext626AA0.field140;
    const std::int32_t next = ++color;
    if (next >= 10) {
        color = 0;
    } else if (next == 4) {
        color = 5;
    }
    static_cast<void>(PlaySe(51, 320, 240));
    return 0;
}

std::int32_t Sub_4FD5A0()
{
    if (input::IsChatContext_503E60() == 0) {
        static_cast<void>(audio::Sub_504380(6u));
    }
    static_cast<void>(audio::Sub_504180(6u));
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

std::int32_t Sub_4FE3E0()
{
    static_cast<void>(audio::Sub_504180(3u));
    return 0;
}

// 原[4FDD70,4FDDB6)：同次S32音量判断/加一，应用后才播放提示音。
std::int32_t Sub_4FDD70()
{
    const std::int32_t volume = platform::gMusicVolume;
    if (volume < 15) {
        platform::gMusicVolume = volume + 1;
        static_cast<void>(audio::Sub_52C3D0());
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    return 0;
}

// 原[4FDDC0,4FDE05)：同次S32音量判断/减一，保留音乐应用与音效顺序。
std::int32_t Sub_4FDDC0()
{
    const std::int32_t volume = platform::gMusicVolume;
    if (volume > 0) {
        platform::gMusicVolume = volume - 1;
        static_cast<void>(audio::Sub_52C3D0());
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    return 0;
}

// 原[4FE320,4FE361)：音效音量只写共享槽，由实际播放/逐帧更新消费。
std::int32_t Sub_4FE320()
{
    const std::int32_t volume = platform::gEffectVolume;
    if (volume < 15) {
        platform::gEffectVolume = volume + 1;
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    return 0;
}

// 原[4FE370,4FE3B0)：先发布新音效音量，再交既有PlaySe读取。
std::int32_t Sub_4FE370()
{
    const std::int32_t volume = platform::gEffectVolume;
    if (volume > 0) {
        platform::gEffectVolume = volume - 1;
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    return 0;
}

// 原[4FE3B0,4FE3D5)：完整S32减一后取bit0，不新增立即重设通道的调用。
std::int32_t Sub_4FE3B0()
{
    const std::int32_t stereo = platform::gUseStereo;
    platform::gUseStereo = (stereo - 1) & 1;
    static_cast<void>(PlaySe(51, 320, 240));
    return 0;
}

std::int32_t Sub_4FE3F0()
{
    const std::int32_t speed = platform::gValue553438;
    if (speed < 100) {
        platform::gValue553438 = speed + 1;
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    return 0;
}

std::int32_t Sub_4FE440()
{
    const std::int32_t speed = platform::gValue553438;
    if (speed > 1) {
        platform::gValue553438 = speed - 1;
        static_cast<void>(PlaySe(51, 320, 240));
    } else {
        static_cast<void>(PlaySe(55, 320, 240));
    }
    return 0;
}

std::int32_t Sub_4FEAC0()
{
    const std::int32_t show = platform::gValueD67AE4 == 0 ? 1 : 0;
    platform::gValueD67AE4 = static_cast<std::uint32_t>(show);
    static_cast<void>(platform::ShowLegacyCursor(show));
    static_cast<void>(PlaySe(51, 320, 240));
    return 0;
}

std::int32_t Sub_4FEFF0()
{
    platform::gValueD67AE8 = (platform::gValueD67AE8 - 1u) & 1u;
    static_cast<void>(PlaySe(51, 320, 240));
    return 0;
}

std::int32_t Sub_4FF310()
{
    const std::uint8_t mode = static_cast<std::uint8_t>(platform::gScanlineSyncMode);
    platform::gScanlineSyncMode = static_cast<std::uint16_t>((mode - 1) & 1);
    static_cast<void>(PlaySe(51, 320, 240));
    return 0;
}

std::int32_t Sub_4FF620()
{
    platform::gValueD68858 = platform::gValueD68858 == 0 ? 1u : 0u;
    static_cast<void>(PlaySe(51, 320, 240));
    return 0;
}

} // namespace cg::login::detail
