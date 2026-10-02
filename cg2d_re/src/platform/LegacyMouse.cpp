#include "cg/platform/LegacyMouse.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../LegacyPanelRuntime.hpp"
#include "MainLoopDependencies.hpp"
#include "cg/AnimationRuntime.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/world/WorldObjects.hpp"
#include "cg/ui/WindowRuntime.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace cg::platform {

LegacyMouseState gLegacyMouseState {};
std::uint32_t gValueD68BF0 = 0;
std::uint32_t gValueD68DC8 = 0;

namespace {

// 原CRT 553004..553010；每项完成后独立登记退出，早于553014对象表。
struct StartupCryptValuesStaticInitializer {
    StartupCryptValuesStaticInitializer()
    {
        legacy_crypt_int::ConstructStartupValues_401C20();
    }

    ~StartupCryptValuesStaticInitializer()
    {
        legacy_crypt_int::DestroyStartupValues_401C50();
    }
};

const StartupCryptValuesStaticInitializer gStartupCryptValuesStaticInitializer;

struct ProtectionChecksum1StaticInitializer {
    ProtectionChecksum1StaticInitializer()
    {
        ConstructProtectionChecksum1_404130();
    }

    ~ProtectionChecksum1StaticInitializer()
    {
        DestroyProtectionChecksum1_404150();
    }
};

const ProtectionChecksum1StaticInitializer gProtectionChecksum1StaticInitializer;

struct ProtectionChecksum2StaticInitializer {
    ProtectionChecksum2StaticInitializer()
    {
        ConstructProtectionChecksum2_404170();
    }

    ~ProtectionChecksum2StaticInitializer()
    {
        DestroyProtectionChecksum2_404190();
    }
};

const ProtectionChecksum2StaticInitializer gProtectionChecksum2StaticInitializer;

struct ProtectionProcessStateStaticInitializer {
    ProtectionProcessStateStaticInitializer()
    {
        ConstructProtectionProcessState_4041B0();
    }

    ~ProtectionProcessStateStaticInitializer()
    {
        DestroyProtectionProcessState_4041D0();
    }
};

const ProtectionProcessStateStaticInitializer gProtectionProcessStateStaticInitializer;

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

// 原5495EE不创建sentry；直接同步缓冲，避免现代flush额外刷新tie。
template <typename Character>
void FlushLegacyStandardStream(std::basic_ostream<Character>& stream)
{
    if (!stream.fail() && stream.rdbuf()->pubsync() == -1) {
        stream.setstate(std::ios_base::badbit);
    }
}

// 原CRT 553020/553024；标准流由本机CRT持有，只承接应用侧配置与退出顺序。
struct NarrowStandardStreamsStaticInitializer {
    NarrowStandardStreamsStaticInitializer()
    {
        std::cin.tie(&std::cout);
        std::cerr.tie(&std::cout);
        std::cerr.setf(std::ios_base::unitbuf);
        // 原10864C8/1086310共用10863D8；只借用错误流缓冲，各流状态仍独立。
        std::clog.rdbuf(std::cerr.rdbuf());
        std::clog.tie(&std::cout);
    }

    ~NarrowStandardStreamsStaticInitializer()
    {
        FlushLegacyStandardStream(std::cout);
        FlushLegacyStandardStream(std::cerr);
        FlushLegacyStandardStream(std::clog);
    }
};

const NarrowStandardStreamsStaticInitializer gNarrowStandardStreamsStaticInitializer;

struct WideStandardStreamsStaticInitializer {
    WideStandardStreamsStaticInitializer()
    {
        std::wcin.tie(&std::wcout);
        std::wcerr.tie(&std::wcout);
        std::wcerr.setf(std::ios_base::unitbuf);
        // 原1086500/1086540共用10866A0；所有权保留在本机标准库。
        std::wclog.rdbuf(std::wcerr.rdbuf());
        std::wclog.tie(&std::wcout);
    }

    ~WideStandardStreamsStaticInitializer()
    {
        FlushLegacyStandardStream(std::wcout);
        FlushLegacyStandardStream(std::wcerr);
        FlushLegacyStandardStream(std::wclog);
    }
};

const WideStandardStreamsStaticInitializer gWideStandardStreamsStaticInitializer;

// 原五个CRT入口共用的W8标记；保留其他位，不为各包装另建状态。
std::uint8_t gLegacyPrimaryExitGuard1086760 = 0;
std::uint8_t gLegacySecondaryExitGuard1087AFC = 0;

// 原536960、54843B各只有RET；这两个回调确实不执行清理业务。
void EmptyLegacyExit_536960()
{
}

void EmptyLegacyExit_54843B()
{
}

// 原5491BE/549F4A/54A662/54A9AB：各次均真实登记并返回结果。
std::int32_t RegisterLegacyPrimaryEmptyExit()
{
    return std::atexit(EmptyLegacyExit_536960);
}

std::int32_t InitializeLegacyEmptyExit_454C00()
{
    if ((gLegacyPrimaryExitGuard1086760 & 1u) == 0) {
        gLegacyPrimaryExitGuard1086760 |= 1u;
    }
    return RegisterLegacyPrimaryEmptyExit();
}

// 原549194/549F20/54A638/54A981：第一次登记失败也继续第二次，不回滚标记。
std::int32_t RegisterLegacyEmptyExitPair()
{
    if ((gLegacyPrimaryExitGuard1086760 & 1u) == 0) {
        gLegacyPrimaryExitGuard1086760 |= 1u;
    }
    static_cast<void>(RegisterLegacyPrimaryEmptyExit());
    if ((gLegacySecondaryExitGuard1087AFC & 1u) == 0) {
        gLegacySecondaryExitGuard1087AFC |= 1u;
    }
    return std::atexit(EmptyLegacyExit_54843B);
}

// 原CRT553028；没有对象析构，退出登记仅来自原入口中的atexit。
struct LegacyPrimaryEmptyExitStaticInitializer {
    LegacyPrimaryEmptyExitStaticInitializer()
    {
        static_cast<void>(InitializeLegacyEmptyExit_454C00());
    }
};

const LegacyPrimaryEmptyExitStaticInitializer gLegacyPrimaryEmptyExitStaticInitializer;

// 原CRT 55302C..553050；无析构登记，不能把十五组记录改为统一写序。
struct LegacyUi455490StaticInitializer {
    LegacyUi455490StaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_455490());
        static_cast<void>(cg::login::detail::Sub_4554C0());
        static_cast<void>(cg::login::detail::Sub_455540());
        static_cast<void>(cg::login::detail::Sub_4555C0());
        static_cast<void>(cg::login::detail::Sub_455640());
        static_cast<void>(cg::login::detail::Sub_4556C0());
        static_cast<void>(cg::login::detail::Sub_455740());
        static_cast<void>(cg::login::detail::Sub_4557C0());
        static_cast<void>(cg::login::detail::Sub_455840());
        static_cast<void>(cg::login::detail::Sub_455930());
    }
};

const LegacyUi455490StaticInitializer gLegacyUi455490StaticInitializer;

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

// 原CRT 553060；中间Graphic容器由既有std容器承接，不重复构造。
struct AnimationSharedValueStaticInitializer {
    AnimationSharedValueStaticInitializer()
    {
        static_cast<void>(cg::resource::animation::Sub_45B370());
    }
};

const AnimationSharedValueStaticInitializer gAnimationSharedValueStaticInitializer;

struct PlayerCoordinatesStaticInitializer {
    PlayerCoordinatesStaticInitializer()
    {
        cg::world::InitializePlayerCoordinates();
    }
};

const PlayerCoordinatesStaticInitializer gPlayerCoordinatesStaticInitializer;

struct WorldCoordinateStatesStaticInitializer {
    WorldCoordinateStatesStaticInitializer()
    {
        cg::world::InitializeWorldCoordinateStates();
    }
};

const WorldCoordinateStatesStaticInitializer gWorldCoordinateStatesStaticInitializer;

struct WorldMoveStatesStaticInitializer {
    WorldMoveStatesStaticInitializer()
    {
        cg::world::InitializeWorldMoveStates();
    }
};

const WorldMoveStatesStaticInitializer gWorldMoveStatesStaticInitializer;

struct RippleGateStaticInitializer {
    RippleGateStaticInitializer()
    {
        cg::world::InitializeRippleGate();
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

// 原55309C/5530A0相邻，两对象各十CryptInt；每项构造后独立登记退出。
struct ItemSelectorsStaticInitializer {
    ItemSelectorsStaticInitializer()
    {
        cg::ui::InitializeItemSelectors();
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

// 原相邻CRT 5530BC/5530C0；两构造均无析构或RNG消费。
struct LegacyHotkeyAndScanlineStaticInitializer {
    LegacyHotkeyAndScanlineStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_4FEB00());
        static_cast<void>(cg::login::detail::Sub_4FF020());
    }
};

const LegacyHotkeyAndScanlineStaticInitializer gLegacyHotkeyAndScanlineStaticInitializer;

// 原CRT 5530C4，紧邻已接的5530C0与5530C8。
struct LegacyButtonSettingsStaticInitializer {
    LegacyButtonSettingsStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_4FF340());
    }
};

const LegacyButtonSettingsStaticInitializer gLegacyButtonSettingsStaticInitializer;

// 原CRT 5530C8；只补已证地图构造，不为其他缺项填空体。
struct LegacyMapPanelStaticInitializer {
    LegacyMapPanelStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_502930());
    }
};

const LegacyMapPanelStaticInitializer gLegacyMapPanelStaticInitializer;

// 原CRT 5530CC..5530F4；逐项调用，不能跨条目提前缓存尺寸或位置。
struct LegacyBottomBarStaticInitializer {
    LegacyBottomBarStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_502C30());
        static_cast<void>(cg::login::detail::Sub_502C60());
        static_cast<void>(cg::login::detail::Sub_502C80());
        static_cast<void>(cg::login::detail::Sub_502CA0());
        static_cast<void>(cg::login::detail::Sub_502D20());
        static_cast<void>(cg::login::detail::Sub_502DA0());
        static_cast<void>(cg::login::detail::Sub_502E20());
        static_cast<void>(cg::login::detail::Sub_502EA0());
        static_cast<void>(cg::login::detail::Sub_502F20());
        static_cast<void>(cg::login::detail::Sub_502FA0());
        static_cast<void>(cg::login::detail::Sub_503020());
    }
};

const LegacyBottomBarStaticInitializer gLegacyBottomBarStaticInitializer;

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

// 原后段CRT553130/553158/553160/553164；各自保留两次登记，按原相对顺序执行。
struct LegacyPairedEmptyExitStaticInitializer {
    LegacyPairedEmptyExitStaticInitializer()
    {
        static_cast<void>(RegisterLegacyEmptyExitPair());
    }
};

const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553130;
const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553158;
const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553160;
const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553164;

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
