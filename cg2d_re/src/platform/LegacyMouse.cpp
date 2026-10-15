#include "cg/platform/LegacyMouse.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"
#include "../world/WorldObjectState.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "MainLoopDependencies.hpp"
#include "cg/resource/AnimationRuntime.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/GraphicRead.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/ui/WindowRuntime.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <locale>
#include <type_traits>

namespace cg::platform {

std::uint32_t gValueD68BF0 = 0; // 原全局：0x00D68BF0。
std::uint32_t gValueD68DC8 = 0; // 原全局：0x00D68DC8。

namespace {

// 原CRT553004..553010：两值直接构造后接三个防护值，保持五次RNG序。
// 原0x00401C10、0x00401C20：5939C8→5939D8；原0x00401C50退出反序，由MSVC承接。
LegacyCryptInt gStartupValue5939C8{}; // 原全局：0x005939C8。
LegacyCryptInt gStartupValue5939D8{}; // 原全局：0x005939D8。

} // namespace

// 原0x00404120／0x00404130／0x00404150：原中央位置直接构造，MSVC承接逆序析构。
LegacyCryptInt gProtectionChecksum1{}; // 原全局：0x00594070。

// 原0x00404160／0x00404170／0x00404190：原中央位置直接构造，MSVC承接逆序析构。
LegacyCryptInt gProtectionChecksum2{}; // 原全局：0x00594080。

// 原0x004041A0／0x004041B0／0x004041D0：原中央位置直接构造，MSVC承接逆序析构。
LegacyCryptInt gProtectionProcessState{}; // 原全局：0x0059E548。

} // namespace cg::platform

namespace cg::world::detail {

// 原CRT553014，原0x0043D760／0x0043D770／0x0043D7A0：原中央位置直接构造及逆序析构。
// 原0x0053C45E／0x0053C4E0／0x0053C560／0x005405F5：数组寿命及完成前缀清理由C++／MSVC承接。
// 原0x0043D7C0／0x0043D810：千项各两CryptInt按成员声明序构造、退出双逆序。
std::array<WorldObjectRecord, kWorldObjectCapacity> gWorldObjects{}; // 原全局：0x005AC4C0。

} // namespace cg::world::detail

namespace cg::platform {
namespace {

// AIS01原553018/55301C：世界表后、宿主窄宽流配置前，同一TU有序初始化。
struct SurfaceTransformCacheStartup final {
    SurfaceTransformCacheStartup()
    {
        // 原 553018 -> 55301C；各自构造后立即登记，不按第一次登记结果短路。
        static_cast<void>(cg::render::InitializeSurfaceTransformCache629300_44CC20());
        static_cast<void>(cg::render::InitializeSurfaceTransformCache62A630_44CD30());
    }
};

static_assert(std::is_trivially_destructible_v<SurfaceTransformCacheStartup>);
SurfaceTransformCacheStartup gSurfaceTransformCacheStartup;

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
std::uint8_t gLegacyPrimaryExitGuard1086760 = 0; // 原全局：0x01086760。
std::uint8_t gLegacySecondaryExitGuard1087AFC = 0; // 原全局：0x01087AFC。

// 原536960、54843B各只有RET；这两个回调确实不执行清理业务。
void EmptyLegacyExit_536960()
{
}

// 原函数：0x0054843B。
void EmptyLegacyExit_54843B()
{
}

// 原5491BE/549F4A/54A662/54A9AB：各次均真实登记并返回结果。
std::int32_t RegisterLegacyPrimaryEmptyExit()
{
    return std::atexit(EmptyLegacyExit_536960);
}

// 原函数：0x00454C00。
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

} // namespace
} // namespace cg::platform

namespace cg::world::detail {

// 原0x00456FD0／0x00456FE0／0x00457000：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gSceneTransitionValue{}; // 原全局：0x0073ECD0。

} // namespace cg::world::detail

namespace cg::platform {
namespace {

// AIS01原553058/55305C：场景值后、动画共享值前；各项仍即时登记。
struct GraphicInfoContainersStartup final {
    GraphicInfoContainersStartup()
    {
        // 原553058 -> 55305C：第一登记失败不跳过第二构造/登记。
        static_cast<void>(cg::resource::InitializeGraphicMapNumbers_4597E0());
        static_cast<void>(cg::resource::InitializeGraphicFirstRecords_459850());
    }
};

static_assert(std::is_trivially_destructible_v<GraphicInfoContainersStartup>);
GraphicInfoContainersStartup gGraphicInfoContainersStartup;

// 原CRT553060；前两Graphic容器已由AIS01原序调用，不重复构造。
struct AnimationSharedValueStaticInitializer {
    AnimationSharedValueStaticInitializer()
    {
        static_cast<void>(cg::resource::animation::Sub_45B370());
    }
};

const AnimationSharedValueStaticInitializer gAnimationSharedValueStaticInitializer;

} // namespace
} // namespace cg::platform

namespace cg::world::detail {

// 原0x00467B90／0x00467BA0／0x00467BD0：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gPlayerX{}; // 原全局：0x009EE8A8。
platform::LegacyCryptInt gPlayerY{}; // 原全局：0x009EE8C0。

// 原0x00467BF0／0x00467C00／0x00467C20：中央原位置直接构造，MSVC承接退出析构。
platform::LegacyCryptInt gPlayerSnapshotX{}; // 原全局：0x00C751C0。
// 原0x00467C30／0x00467C40／0x00467C60：紧接X构造，退出先Y后X；各构造一次RNG。
platform::LegacyCryptInt gPlayerSnapshotY{}; // 原全局：0x00C751B0。

// 原0x00467C70／0x00467C80／0x00467CA0：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gValue9EE8D0{}; // 原全局：0x009EE8D0。

// 原0x00467CB0／0x00467CC0／0x00467CE0：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gValue9EE8E0{}; // 原全局：0x009EE8E0。

// 原0x00467CF0／0x00467D00／0x00467D30：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gValueC27FF8{}; // 原全局：0x00C27FF8。
platform::LegacyCryptInt gValueC27FE8{}; // 原全局：0x00C27FE8。

// 原0x00467D50／0x00467D60／0x00467D90：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gMoveTargetX9A7F88{}; // 原全局：0x009A7F88。
platform::LegacyCryptInt gMoveTargetY9A7F98{}; // 原全局：0x009A7F98。

// 原0x00467DB0／0x00467DC0／0x00467DE0：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gValueC821E8{}; // 原全局：0x00C821E8。

// 原0x00467DF0／0x00467E00／0x00467E20：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gValue9E03C0{}; // 原全局：0x009E03C0。

// 原0x00467E30／0x00467E40／0x00467E60：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gValue9FB918{}; // 原全局：0x009FB918。

// 原0x00467E70／0x00467E80／0x00467EA0：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gValueAF0FE8{}; // 原全局：0x00AF0FE8。

// 原0x00467EB0／0x00467EC0／0x00467EE0：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gValueC821D8{}; // 原全局：0x00C821D8。

// 原0x00467EF0／0x00467F00／0x00467F20：原中央位置直接构造，MSVC承接逆序析构。
platform::LegacyCryptInt gRippleGate{}; // 原全局：0x009A7FA8。

} // namespace cg::world::detail

namespace cg::platform {
namespace {

// 原CRT 553098紧接553094 Ripple；不增加RNG调用或析构步骤。
struct WindowNumberSpacingStaticInitializer {
    WindowNumberSpacingStaticInitializer()
    {
        cg::ui::WindowNumber_InitializeLegacySpacing();
    }
};

const WindowNumberSpacingStaticInitializer gWindowNumberSpacingStaticInitializer;

// 原55309C/5530A0相邻，两对象各十CryptInt；各自构造／析构。
// 原函数：0x004883F0；同一中央位置构造，MSVC按完成逆序析构。
const cg::ui::FirstItemSelectorStartup gFirstItemSelectorStartup;

// 原函数：0x004998F0；同一中央位置构造，MSVC按完成逆序析构。
const cg::ui::SecondItemSelectorStartup gSecondItemSelectorStartup;

// 原CRT 5530A4；浏览宠物拥有者接管构析，不与玩家宠物共享。
// 原函数：0x004CD240；同一中央位置构造，MSVC按完成逆序析构。
const cg::ui::StallBrowsePetValuesStartup gStallBrowsePetValuesStartup;

} // namespace

// 原CRT5530F8，原0x00506340／0x00506350／0x00506370：同一中央位置直接定义。
// 原0x00506460／0x00506530／0x00506380：X→Y→八坐标构造，退出成员逆序。
LegacyMouseState gLegacyMouseState{}; // 原全局：0x00D68CF8。

} // namespace cg::platform

namespace cg::world::detail {

// 原CRT5530FC，原0x005183D0／0x005183E0／0x00518400：紧接鼠标，原生逆序析构。
// 原0x005184A0／0x00518410：六CryptInt原声明序构造，普通字段保留零初值。
PlayerRuntimeSlots gPlayerRuntimeSlots{}; // 原全局：0x00FDA700。

// 原CRT553100，原0x00518520／0x00518530／0x00518560：紧接玩家，独立于摊位浏览宠物。
// 原0x004CD360／0x004CD2D0：五元素各六成员构造；退出元素及成员逆序。
std::array<PetRuntimeRecord, 5> gPetRuntimeRecords{}; // 原全局：0x00F633F8。

} // namespace cg::world::detail

namespace cg::platform {
namespace {

// AIS01原553104/553108：主/宠物后，旧键盘排除项前，不混入待接标准流。
struct IndependentPaletteContainersStartup final {
    IndependentPaletteContainersStartup()
    {
        // 原553104 -> 553108；每项构造后立即登记，不按首次结果短路。
        static_cast<void>(cg::resource::InitializeIndependentPaletteQueue_522620());
        static_cast<void>(cg::resource::InitializePaletteDbfState_522690());
    }
};

static_assert(std::is_trivially_destructible_v<IndependentPaletteContainersStartup>);
IndependentPaletteContainersStartup gIndependentPaletteContainersStartup;

// 原后段CRT553130/553158/553160/553164；各自保留两次登记，按原相对顺序执行。
struct LegacyPairedEmptyExitStaticInitializer {
    LegacyPairedEmptyExitStaticInitializer()
    {
        static_cast<void>(RegisterLegacyEmptyExitPair());
    }
};

const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553130; // 原全局：0x00553130。

// SLN01原553134：独立完整SDK Init对象持有窄流生命周期。
// 自动析构由SDK承接；不另登记旧5491F6或改Legacy计数/登记表。
std::ios_base::Init gSdkNarrowLateInit553134;

const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553158; // 原全局：0x00553158。

// SCL01原55315C：晚段经典对象职责由宿主完整locale持有并清理。
// 不另登记旧54A3FA，也不将SDK对象写入Legacy三槽。
struct SdkClassicLocaleStaticInitializer final {
    SdkClassicLocaleStaticInitializer()
    {
        static_cast<void>(std::locale::classic());
    }
};
static_assert(std::is_trivially_destructible_v<SdkClassicLocaleStaticInitializer>);

const SdkClassicLocaleStaticInitializer gSdkClassicLocale55315C; // 原全局：0x0055315C。

const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553160; // 原全局：0x00553160。
const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553164; // 原全局：0x00553164。

} // namespace

// 原函数：0x00506870。
std::int32_t ResetLegacyMouseState_506870()
{
    auto& state = gLegacyMouseState;
    const LegacyCryptInt::RawPayload zero{};
    state.mouseX.RestoreRaw(zero);
    state.mouseY.RestoreRaw(zero);
    for (LegacyCryptInt& value : state.buttonCoordinates) {
        value.RestoreRaw(zero);
    }
    // 原整块零字节：普通字段直接清零，不重构十成员或消费RNG。
    constexpr std::size_t tail = offsetof(LegacyMouseState, heldButtons);
    std::memset(reinterpret_cast<std::byte*>(&state) + tail, 0, sizeof(state) - tail);
    gValueD68DC4 = 0;
    state.valueD68DB8 = -1;
    return 0;
}

// 原函数：0x00506890。
void ResetLegacyMousePosition_506890(const std::int32_t x, const std::int32_t y)
{
    gLegacyMouseState.mouseX = x;

    gLegacyMouseState.mouseY = y;
}

// 原函数：0x00506900。
std::int32_t UpdateLegacyMouseLeftDown_506900(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 1u;
    gLegacyMouseState.heldButtons = heldButtons | 1u;
    gLegacyMouseState.pulseButtons = pulseButtons | 1u;

    gLegacyMouseState.buttonCoordinates[0] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[1] = static_cast<std::int32_t>(y);

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

// 原函数：0x005069E0。
std::int32_t UpdateLegacyMouseLeftUp_5069E0(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.heldButtons = heldButtons & ~1u;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 4u;
    gLegacyMouseState.pulseButtons = pulseButtons & ~1u;

    gLegacyMouseState.buttonCoordinates[2] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[3] = static_cast<std::int32_t>(y);

    gLegacyMouseState.leftDownTick = 0;
    gLegacyMouseState.leftElapsed = 0;
    return 0;
}

// 原函数：0x00506A90。
std::uint32_t UpdateLegacyMouseLeftDoubleClick_506A90(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.heldButtons = heldButtons | 1u;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 0x11u;
    gLegacyMouseState.pulseButtons = pulseButtons | 1u;

    gLegacyMouseState.buttonCoordinates[0] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[1] = static_cast<std::int32_t>(y);

    const std::uint32_t tick = ReadTickCount32();
    gLegacyMouseState.leftDownTick = tick;
    gLegacyMouseState.leftElapsed = 0;
    return tick;
}

// 原函数：0x00506B50。
std::int32_t UpdateLegacyMouseRightDown_506B50(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    gLegacyMouseState.heldButtons = heldButtons | 2u;
    gLegacyMouseState.pulseButtons = pulseButtons | 2u;
    gLegacyMouseState.edgeButtons = edgeButtons | 2u;

    gLegacyMouseState.buttonCoordinates[4] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[5] = static_cast<std::int32_t>(y);

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

// 原函数：0x00506C40。
std::int32_t UpdateLegacyMouseRightUp_506C40(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.heldButtons = heldButtons & ~2u;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 8u;
    gLegacyMouseState.pulseButtons = pulseButtons & ~2u;

    gLegacyMouseState.buttonCoordinates[6] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[7] = static_cast<std::int32_t>(y);

    gLegacyMouseState.rightDownTick = 0;
    gLegacyMouseState.rightElapsed = 0;
    return 0;
}

// 原函数：0x00506CF0。
std::uint32_t UpdateLegacyMouseRightDoubleClick_506CF0(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = gLegacyMouseState.heldButtons;
    const std::uint32_t edgeButtons = gLegacyMouseState.edgeButtons;
    gLegacyMouseState.heldButtons = heldButtons | 2u;
    const std::uint32_t pulseButtons = gLegacyMouseState.pulseButtons;
    gLegacyMouseState.edgeButtons = edgeButtons | 0x22u;
    gLegacyMouseState.pulseButtons = pulseButtons | 2u;

    gLegacyMouseState.buttonCoordinates[4] = static_cast<std::int32_t>(x);

    gLegacyMouseState.buttonCoordinates[5] = static_cast<std::int32_t>(y);

    const std::uint32_t tick = ReadTickCount32();
    gLegacyMouseState.rightDownTick = tick;
    gLegacyMouseState.rightElapsed = 0;
    return tick;
}

// 原函数：0x00506DB0。
std::int32_t AddLegacyMouseWheelDelta_506DB0(const std::int32_t delta) noexcept
{
    gLegacyMouseState.tailReset += static_cast<std::uint32_t>(delta);
    return delta;
}

} // namespace cg::platform

namespace cg::login::detail {

std::uint32_t& gValueD68D98 = platform::gLegacyMouseState.heldButtons; // 原全局：0x00D68D98。
std::uint32_t& gValueD68D9C = platform::gLegacyMouseState.edgeButtons; // 原全局：0x00D68D9C。
std::uint32_t& gValueD68DA0 = platform::gLegacyMouseState.pulseButtons; // 原全局：0x00D68DA0。

std::int32_t ReadLegacyMouseX()
{
    return platform::gLegacyMouseState.mouseX.value();
}

std::int32_t ReadLegacyMouseY()
{
    return platform::gLegacyMouseState.mouseY.value();
}

} // namespace cg::login::detail
