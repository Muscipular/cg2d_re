#include "cg/platform/LegacyMouse.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../LegacyPanelRuntime.hpp"
#include "MainLoopDependencies.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/world/WorldObjects.hpp"
#include "cg/ui/WindowRuntime.hpp"

#include <cstring>

namespace cg::platform {

LegacyMouseState gLegacyMouseState {};
std::uint32_t gValueD68BF0 = 0;
std::uint32_t gValueD68DC8 = 0;

namespace {

struct WorldObjectsStaticInitializer {
    WorldObjectsStaticInitializer()
    {
        cg::world::ConstructWorldObjects();
    }

    ~WorldObjectsStaticInitializer()
    {
        cg::world::DestroyWorldObjects();
    }
};

const WorldObjectsStaticInitializer gWorldObjectsStaticInitializer;

// 原CRT 553054介于World表553014和玩家坐标553064之间；未落位的其他项不填空体。
struct SceneTransitionValueStaticInitializer {
    SceneTransitionValueStaticInitializer()
    {
        cg::world::ConstructSceneTransitionValue();
    }

    ~SceneTransitionValueStaticInitializer()
    {
        cg::world::DestroySceneTransitionValue();
    }
};

const SceneTransitionValueStaticInitializer gSceneTransitionValueStaticInitializer;

struct PlayerCoordinatesStaticInitializer {
    PlayerCoordinatesStaticInitializer()
    {
        cg::world::ConstructPlayerCoordinates();
    }

    ~PlayerCoordinatesStaticInitializer()
    {
        cg::world::DestroyPlayerCoordinates();
    }
};

const PlayerCoordinatesStaticInitializer gPlayerCoordinatesStaticInitializer;

struct WorldCoordinateStatesStaticInitializer {
    WorldCoordinateStatesStaticInitializer()
    {
        cg::world::ConstructWorldCoordinateStates();
    }

    ~WorldCoordinateStatesStaticInitializer()
    {
        cg::world::DestroyWorldCoordinateStates();
    }
};

const WorldCoordinateStatesStaticInitializer gWorldCoordinateStatesStaticInitializer;

struct WorldMoveStatesStaticInitializer {
    WorldMoveStatesStaticInitializer()
    {
        cg::world::ConstructWorldMoveStates();
    }

    ~WorldMoveStatesStaticInitializer()
    {
        cg::world::DestroyWorldMoveStates();
    }
};

const WorldMoveStatesStaticInitializer gWorldMoveStatesStaticInitializer;

struct RippleGateStaticInitializer {
    RippleGateStaticInitializer()
    {
        cg::world::ConstructRippleGate();
    }

    ~RippleGateStaticInitializer()
    {
        cg::world::DestroyRippleGate();
    }
};

const RippleGateStaticInitializer gRippleGateStaticInitializer;

// 原CRT 553098紧接553094 Ripple；不增加RNG调用或析构步骤。
struct WindowNumberSpacingStaticInitializer {
    WindowNumberSpacingStaticInitializer()
    {
        cg::ui::InitializeNumberSpacing_477170();
    }
};

const WindowNumberSpacingStaticInitializer gWindowNumberSpacingStaticInitializer;

// 原55309C/5530A0相邻，两对象各十CryptInt；退出保持相反顺序。
struct ItemSelectorsStaticInitializer {
    ItemSelectorsStaticInitializer()
    {
        cg::ui::ConstructItemSelectors();
    }

    ~ItemSelectorsStaticInitializer()
    {
        cg::ui::DestroyItemSelectors();
    }
};

const ItemSelectorsStaticInitializer gItemSelectorsStaticInitializer;

// 原CRT 5530A4；成功构造后才登记析构，浏览区不与玩家宠物共享。
struct StallBrowsePetsStaticInitializer {
    StallBrowsePetsStaticInitializer()
    {
        cg::ui::ConstructStallBrowsePetValues();
    }

    ~StallBrowsePetsStaticInitializer()
    {
        cg::ui::DestroyStallBrowsePetValues();
    }
};

const StallBrowsePetsStaticInitializer gStallBrowsePetsStaticInitializer;

// 原CRT 5530A8；文字记录持有真实回调，不引入额外析构或RNG消费。
struct LegacySettingsMenuStaticInitializer {
    LegacySettingsMenuStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_4FC420());
    }
};

const LegacySettingsMenuStaticInitializer gLegacySettingsMenuStaticInitializer;

// 原CRT 5530AC；复用既有显示设置回调，不增加初始化之外的操作。
struct LegacyChatSettingsStaticInitializer {
    LegacyChatSettingsStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_4FCB90());
    }
};

const LegacyChatSettingsStaticInitializer gLegacyChatSettingsStaticInitializer;

// 原CRT 5530B8；中间音乐/音效页仍暂缓，不注册空构造。
struct LegacyMouseSettingsStaticInitializer {
    LegacyMouseSettingsStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_4FE490());
    }
};

const LegacyMouseSettingsStaticInitializer gLegacyMouseSettingsStaticInitializer;

// 原CRT 5530C8；只补已证地图构造，不为其他缺项填空体。
struct LegacyMapPanelStaticInitializer {
    LegacyMapPanelStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_502930());
    }
};

const LegacyMapPanelStaticInitializer gLegacyMapPanelStaticInitializer;

struct LegacyMouseStaticInitializer {
    LegacyMouseStaticInitializer()
    {
        legacy_crypt_int::ConstructZero_401C70(&gLegacyMouseState.mouseX);
        legacy_crypt_int::ConstructZero_401C70(&gLegacyMouseState.mouseY);
        for (LegacyCryptInt& coordinate : gLegacyMouseState.buttonCoordinates) {
            legacy_crypt_int::ConstructZero_401C70(&coordinate);
        }
    }

    ~LegacyMouseStaticInitializer()
    {
        for (std::size_t index = 8; index != 0; --index) {
            legacy_crypt_int::Destroy_401CF0(
                &gLegacyMouseState.buttonCoordinates[index - 1]);
        }
        legacy_crypt_int::Destroy_401CF0(&gLegacyMouseState.mouseY);
        legacy_crypt_int::Destroy_401CF0(&gLegacyMouseState.mouseX);
    }
};

const LegacyMouseStaticInitializer gLegacyMouseStaticInitializer;

// 原CRT表5530FC/553100紧接鼠标5530F8，退出按声明逆序。
struct PlayerRuntimeValuesStaticInitializer {
    PlayerRuntimeValuesStaticInitializer()
    {
        cg::world::ConstructPlayerRuntimeValues();
    }

    ~PlayerRuntimeValuesStaticInitializer()
    {
        cg::world::DestroyPlayerRuntimeValues();
    }
};

const PlayerRuntimeValuesStaticInitializer gPlayerRuntimeValuesStaticInitializer;

struct PetRuntimeValuesStaticInitializer {
    PetRuntimeValuesStaticInitializer()
    {
        cg::world::ConstructPetRuntimeValues();
    }

    ~PetRuntimeValuesStaticInitializer()
    {
        cg::world::DestroyPetRuntimeValues();
    }
};

const PetRuntimeValuesStaticInitializer gPetRuntimeValuesStaticInitializer;

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

std::int32_t AddLegacyMouseWheelDelta_506DB0(const std::int32_t delta) noexcept
{
    gLegacyMouseState.tailReset += static_cast<std::uint32_t>(delta);
    return delta;
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
