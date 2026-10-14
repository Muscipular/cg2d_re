#include "cg/platform/LegacyMouse.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../LegacyPanelRuntime.hpp"
#include "MainLoopDependencies.hpp"
#include "cg/AnimationRuntime.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/GraphicRead.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/platform/LegacyMouseTracker.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/world/WorldObjects.hpp"
#include "cg/ui/WindowRuntime.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <locale>
#include <optional>
#include <type_traits>

namespace cg::platform {

// 空存储首次用于中央拥有者构造时登记；退出先由拥有者reset，后销毁空容器。
std::optional<LegacyMouseState>& BorrowLegacyMouseStateStorage()
{
    static std::optional<LegacyMouseState> storage;
    return storage;
} // 原全局：0x00D68CF8。
std::uint32_t gValueD68BF0 = 0; // 原全局：0x00D68BF0。
std::uint32_t gValueD68DC8 = 0; // 原全局：0x00D68DC8。

namespace {

// 原CRT553004..553010：两值直接构造后接三个保护拥有者，保持五次RNG序。
// 原0x00401C10、0x00401C20：5939C8→5939D8；原0x00401C50退出反序，由MSVC承接。
LegacyCryptInt gStartupValue5939C8{}; // 原全局：0x005939C8。
LegacyCryptInt gStartupValue5939D8{}; // 原全局：0x005939D8。

// 原函数：0x00404120；同一中央位置构造，MSVC按完成逆序析构。
const cg::platform::ProtectionChecksum1Startup gProtectionChecksum1Startup;

// 原函数：0x00404160；同一中央位置构造，MSVC按完成逆序析构。
const cg::platform::ProtectionChecksum2Startup gProtectionChecksum2Startup;

// 原函数：0x004041A0；同一中央位置构造，MSVC按完成逆序析构。
const cg::platform::ProtectionProcessStateStartup gProtectionProcessStateStartup;

struct WorldObjectsStaticInitializer final {
    WorldObjectsStaticInitializer()
    {
        // 原0x0043D760：原生拥有者接管构造及退出登记。
        cg::world::ConstructWorldObjects();
    }

    ~WorldObjectsStaticInitializer()
    {
        cg::world::DestroyWorldObjects();
    }
};

const WorldObjectsStaticInitializer gWorldObjectsStaticInitializer;

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
// 原函数：0x00456FD0；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::SceneTransitionValueStartup gSceneTransitionValueStartup;

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

// 原函数：0x00467B90；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::PlayerCoordinatesStartup gPlayerCoordinatesStartup;

// 原函数：0x00467BF0；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::PlayerSnapshotXStartup gPlayerSnapshotXStartup;

// 原函数：0x00467C30；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::PlayerSnapshotYStartup gPlayerSnapshotYStartup;

// 原函数：0x00467C70；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::WorldCoordinateState9EE8D0Startup gWorldCoordinateState9EE8D0Startup;

// 原函数：0x00467CB0；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::WorldCoordinateState9EE8E0Startup gWorldCoordinateState9EE8E0Startup;

// 原函数：0x00467CF0；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::WorldCoordinatePairStartup gWorldCoordinatePairStartup;

// 原函数：0x00467D50；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::WorldMoveTargetsStartup gWorldMoveTargetsStartup;

// 原函数：0x00467DB0；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::WorldMoveStateC821E8Startup gWorldMoveStateC821E8Startup;

// 原函数：0x00467DF0；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::WorldMoveState9E03C0Startup gWorldMoveState9E03C0Startup;

// 原函数：0x00467E30；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::WorldMoveState9FB918Startup gWorldMoveState9FB918Startup;

// 原函数：0x00467E70；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::WorldMoveStateAF0FE8Startup gWorldMoveStateAF0FE8Startup;

// 原函数：0x00467EB0；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::WorldMoveStateC821D8Startup gWorldMoveStateC821D8Startup;

// 原函数：0x00467EF0；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::RippleGateStartup gRippleGateStartup;

// 原CRT 553098紧接553094 Ripple；不增加RNG调用或析构步骤。
struct WindowNumberSpacingStaticInitializer {
    WindowNumberSpacingStaticInitializer()
    {
        cg::ui::InitializeLegacyNumberSpacing_477160();
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

// 原相邻CRT 5530B0/5530B4；真实音乐/音效控件构造位于聊天与鼠标设置之间。
struct LegacyMusicSettingsStaticInitializer final {
    LegacyMusicSettingsStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_4FD9F0());
    }
};
static_assert(std::is_trivially_destructible_v<LegacyMusicSettingsStaticInitializer>);
const LegacyMusicSettingsStaticInitializer gLegacyMusicSettingsStaticInitializer;

struct LegacySoundSettingsStaticInitializer final {
    LegacySoundSettingsStaticInitializer()
    {
        static_cast<void>(cg::login::detail::Sub_4FDE10());
    }
};
static_assert(std::is_trivially_destructible_v<LegacySoundSettingsStaticInitializer>);
const LegacySoundSettingsStaticInitializer gLegacySoundSettingsStaticInitializer;

// 原CRT 5530B8；沿原序接在两音频设置构造之后。
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

// 原函数：0x00506340；同一中央位置构造，MSVC按完成逆序析构。
const cg::platform::MouseRuntimeStartup gMouseRuntimeStartup;

// 原CRT表5530FC/553100紧接鼠标5530F8，退出按声明逆序。
// 原函数：0x005183D0；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::PlayerRuntimeValuesStartup gPlayerRuntimeValuesStartup;

// 原函数：0x00518520；同一中央位置构造，MSVC按完成逆序析构。
const cg::world::PetRuntimeValuesStartup gPetRuntimeValuesStartup;

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

namespace {

// 原506350固定D68CF8→506460；仅十个CryptInt按原序构造，不动鼠标其他字段。
LegacyCryptInt* ConstructLegacyMouseRuntime_506350()
{
    // 原0x00506460、0x00506530：十成员正序构造及异常时的已完成前缀清理由C++承接。
    return &BorrowLegacyMouseStateStorage().emplace().mouseX;
}

// 原506370固定D68CF8→506380；整体退出按八坐标/Y/X逆序，只原析构动作。
void ReleaseLegacyMouseRuntimeAtExit_506370()
{
    // 原0x00506380：八坐标、Y、X由成员析构自动逆序清理。
    BorrowLegacyMouseStateStorage().reset();
}

} // namespace

// 原函数：0x00506340；LCI01启动拥有者替代CRT构造／退出登记。
MouseRuntimeStartup::MouseRuntimeStartup()
{
    ConstructLegacyMouseRuntime_506350();
}

// 对应同一启动组的原逆序清理；登记与异常规则由MSVC承接。
MouseRuntimeStartup::~MouseRuntimeStartup()
{
    ReleaseLegacyMouseRuntimeAtExit_506370();
}

// 原函数：0x00506870。
std::int32_t ResetLegacyMouseState_506870()
{
    auto& state = *BorrowLegacyMouseStateStorage();
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
    BorrowLegacyMouseStateStorage()->mouseX = static_cast<std::int32_t>(x);

    BorrowLegacyMouseStateStorage()->mouseY = static_cast<std::int32_t>(y);
}

// 原函数：0x00506900。
std::int32_t UpdateLegacyMouseLeftDown_506900(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t edgeButtons = BorrowLegacyMouseStateStorage()->edgeButtons;
    const std::uint32_t heldButtons = BorrowLegacyMouseStateStorage()->heldButtons;
    const std::uint32_t pulseButtons = BorrowLegacyMouseStateStorage()->pulseButtons;
    BorrowLegacyMouseStateStorage()->edgeButtons = edgeButtons | 1u;
    BorrowLegacyMouseStateStorage()->heldButtons = heldButtons | 1u;
    BorrowLegacyMouseStateStorage()->pulseButtons = pulseButtons | 1u;

    BorrowLegacyMouseStateStorage()->buttonCoordinates[0] = static_cast<std::int32_t>(x);

    BorrowLegacyMouseStateStorage()->buttonCoordinates[1] = static_cast<std::int32_t>(y);

    const std::uint32_t tick = ReadTickCount32();
    BorrowLegacyMouseStateStorage()->leftDownTick = tick;
    BorrowLegacyMouseStateStorage()->leftElapsed = 0;

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
    const std::uint32_t heldButtons = BorrowLegacyMouseStateStorage()->heldButtons;
    const std::uint32_t edgeButtons = BorrowLegacyMouseStateStorage()->edgeButtons;
    BorrowLegacyMouseStateStorage()->heldButtons = heldButtons & ~1u;
    const std::uint32_t pulseButtons = BorrowLegacyMouseStateStorage()->pulseButtons;
    BorrowLegacyMouseStateStorage()->edgeButtons = edgeButtons | 4u;
    BorrowLegacyMouseStateStorage()->pulseButtons = pulseButtons & ~1u;

    BorrowLegacyMouseStateStorage()->buttonCoordinates[2] = static_cast<std::int32_t>(x);

    BorrowLegacyMouseStateStorage()->buttonCoordinates[3] = static_cast<std::int32_t>(y);

    BorrowLegacyMouseStateStorage()->leftDownTick = 0;
    BorrowLegacyMouseStateStorage()->leftElapsed = 0;
    return 0;
}

// 原函数：0x00506A90。
std::uint32_t UpdateLegacyMouseLeftDoubleClick_506A90(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = BorrowLegacyMouseStateStorage()->heldButtons;
    const std::uint32_t edgeButtons = BorrowLegacyMouseStateStorage()->edgeButtons;
    BorrowLegacyMouseStateStorage()->heldButtons = heldButtons | 1u;
    const std::uint32_t pulseButtons = BorrowLegacyMouseStateStorage()->pulseButtons;
    BorrowLegacyMouseStateStorage()->edgeButtons = edgeButtons | 0x11u;
    BorrowLegacyMouseStateStorage()->pulseButtons = pulseButtons | 1u;

    BorrowLegacyMouseStateStorage()->buttonCoordinates[0] = static_cast<std::int32_t>(x);

    BorrowLegacyMouseStateStorage()->buttonCoordinates[1] = static_cast<std::int32_t>(y);

    const std::uint32_t tick = ReadTickCount32();
    BorrowLegacyMouseStateStorage()->leftDownTick = tick;
    BorrowLegacyMouseStateStorage()->leftElapsed = 0;
    return tick;
}

// 原函数：0x00506B50。
std::int32_t UpdateLegacyMouseRightDown_506B50(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t pulseButtons = BorrowLegacyMouseStateStorage()->pulseButtons;
    const std::uint32_t edgeButtons = BorrowLegacyMouseStateStorage()->edgeButtons;
    const std::uint32_t heldButtons = BorrowLegacyMouseStateStorage()->heldButtons;
    BorrowLegacyMouseStateStorage()->heldButtons = heldButtons | 2u;
    BorrowLegacyMouseStateStorage()->pulseButtons = pulseButtons | 2u;
    BorrowLegacyMouseStateStorage()->edgeButtons = edgeButtons | 2u;

    BorrowLegacyMouseStateStorage()->buttonCoordinates[4] = static_cast<std::int32_t>(x);

    BorrowLegacyMouseStateStorage()->buttonCoordinates[5] = static_cast<std::int32_t>(y);

    const std::uint32_t tick = ReadTickCount32();
    BorrowLegacyMouseStateStorage()->rightDownTick = tick;
    BorrowLegacyMouseStateStorage()->rightElapsed = 0;

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
    const std::uint32_t heldButtons = BorrowLegacyMouseStateStorage()->heldButtons;
    const std::uint32_t edgeButtons = BorrowLegacyMouseStateStorage()->edgeButtons;
    BorrowLegacyMouseStateStorage()->heldButtons = heldButtons & ~2u;
    const std::uint32_t pulseButtons = BorrowLegacyMouseStateStorage()->pulseButtons;
    BorrowLegacyMouseStateStorage()->edgeButtons = edgeButtons | 8u;
    BorrowLegacyMouseStateStorage()->pulseButtons = pulseButtons & ~2u;

    BorrowLegacyMouseStateStorage()->buttonCoordinates[6] = static_cast<std::int32_t>(x);

    BorrowLegacyMouseStateStorage()->buttonCoordinates[7] = static_cast<std::int32_t>(y);

    BorrowLegacyMouseStateStorage()->rightDownTick = 0;
    BorrowLegacyMouseStateStorage()->rightElapsed = 0;
    return 0;
}

// 原函数：0x00506CF0。
std::uint32_t UpdateLegacyMouseRightDoubleClick_506CF0(
    const std::int32_t x,
    const std::int32_t y)
{
    const std::uint32_t heldButtons = BorrowLegacyMouseStateStorage()->heldButtons;
    const std::uint32_t edgeButtons = BorrowLegacyMouseStateStorage()->edgeButtons;
    BorrowLegacyMouseStateStorage()->heldButtons = heldButtons | 2u;
    const std::uint32_t pulseButtons = BorrowLegacyMouseStateStorage()->pulseButtons;
    BorrowLegacyMouseStateStorage()->edgeButtons = edgeButtons | 0x22u;
    BorrowLegacyMouseStateStorage()->pulseButtons = pulseButtons | 2u;

    BorrowLegacyMouseStateStorage()->buttonCoordinates[4] = static_cast<std::int32_t>(x);

    BorrowLegacyMouseStateStorage()->buttonCoordinates[5] = static_cast<std::int32_t>(y);

    const std::uint32_t tick = ReadTickCount32();
    BorrowLegacyMouseStateStorage()->rightDownTick = tick;
    BorrowLegacyMouseStateStorage()->rightElapsed = 0;
    return tick;
}

// 原函数：0x00506DB0。
std::int32_t AddLegacyMouseWheelDelta_506DB0(const std::int32_t delta) noexcept
{
    BorrowLegacyMouseStateStorage()->tailReset += static_cast<std::uint32_t>(delta);
    return delta;
}

} // namespace cg::platform

namespace cg::login::detail {

std::uint32_t& gValueD68D98 = platform::BorrowLegacyMouseStateStorage()->heldButtons; // 原全局：0x00D68D98。
std::uint32_t& gValueD68D9C = platform::BorrowLegacyMouseStateStorage()->edgeButtons; // 原全局：0x00D68D9C。
std::uint32_t& gValueD68DA0 = platform::BorrowLegacyMouseStateStorage()->pulseButtons; // 原全局：0x00D68DA0。

std::int32_t ReadLegacyMouseX()
{
    return platform::BorrowLegacyMouseStateStorage()->mouseX.Read_401EC0();
}

std::int32_t ReadLegacyMouseY()
{
    return platform::BorrowLegacyMouseStateStorage()->mouseY.Read_401EC0();
}

} // namespace cg::login::detail
