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
#include <type_traits>

namespace cg::platform {

LegacyMouseState gLegacyMouseState {};
std::uint32_t gValueD68BF0 = 0;
std::uint32_t gValueD68DC8 = 0;

namespace {

// 原CRT553004..553010：每项完成构造后即时登记退出，第一结果不短路后项。
// 只有本组模块位置适配；接全应用原序时须同小步移除此调用，避免双构造/登记。
struct StartupProtectionValuesStartup final {
    StartupProtectionValuesStartup()
    {
        static_cast<void>(legacy_crypt_int::InitializeLegacyStartupValues_401C10());
        static_cast<void>(InitializeLegacyProtectionChecksum1_404120());
        static_cast<void>(InitializeLegacyProtectionChecksum2_404160());
        static_cast<void>(InitializeLegacyProtectionProcessState_4041A0());
    }
};
static_assert(std::is_trivially_destructible_v<StartupProtectionValuesStartup>);

const StartupProtectionValuesStartup gStartupProtectionValuesStartup;

struct WorldObjectsStaticInitializer final {
    WorldObjectsStaticInitializer()
    {
        static_cast<void>(cg::world::InitializeLegacyWorldObjects_43D760());
    }
};
static_assert(std::is_trivially_destructible_v<WorldObjectsStaticInitializer>);

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
std::uint8_t gLegacyPrimaryExitGuard1086760 = 0;
std::uint8_t gLegacySecondaryExitGuard1087AFC = 0;

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
struct SceneTransitionValueStaticInitializer final {
    SceneTransitionValueStaticInitializer()
    {
        static_cast<void>(cg::world::InitializeLegacySceneTransitionValue_456FD0());
    }
};
static_assert(std::is_trivially_destructible_v<SceneTransitionValueStaticInitializer>);

const SceneTransitionValueStaticInitializer gSceneTransitionValueStaticInitializer;

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
        cg::ui::InitializeLegacyNumberSpacing_477160();
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
struct StallBrowsePetsStaticInitializer final {
    StallBrowsePetsStaticInitializer()
    {
        static_cast<void>(cg::ui::InitializeLegacyStallBrowsePetValues_4CD240());
    }
};
static_assert(std::is_trivially_destructible_v<StallBrowsePetsStaticInitializer>);

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

struct LegacyMouseStaticInitializer final {
    LegacyMouseStaticInitializer()
    {
        static_cast<void>(cg::platform::InitializeLegacyMouseRuntime_506340());
    }
};
static_assert(std::is_trivially_destructible_v<LegacyMouseStaticInitializer>);

const LegacyMouseStaticInitializer gLegacyMouseStaticInitializer;

// 原CRT表5530FC/553100紧接鼠标5530F8，退出按声明逆序。
struct PlayerRuntimeValuesStaticInitializer final {
    PlayerRuntimeValuesStaticInitializer()
    {
        static_cast<void>(cg::world::InitializeLegacyPlayerRuntime_5183D0());
    }
};
static_assert(std::is_trivially_destructible_v<PlayerRuntimeValuesStaticInitializer>);

const PlayerRuntimeValuesStaticInitializer gPlayerRuntimeValuesStaticInitializer;

struct PetRuntimeValuesStaticInitializer final {
    PetRuntimeValuesStaticInitializer()
    {
        static_cast<void>(cg::world::InitializeLegacyPetRuntime_518520());
    }
};
static_assert(std::is_trivially_destructible_v<PetRuntimeValuesStaticInitializer>);

const PetRuntimeValuesStaticInitializer gPetRuntimeValuesStaticInitializer;

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

const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553130;

// SLN01原553134：独立完整SDK Init对象持有窄流生命周期。
// 自动析构由SDK承接；不另登记旧5491F6或改Legacy计数/登记表。
std::ios_base::Init gSdkNarrowLateInit553134;

const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553158;

// SCL01原55315C：晚段经典对象职责由宿主完整locale持有并清理。
// 不另登记旧54A3FA，也不将SDK对象写入Legacy三槽。
struct SdkClassicLocaleStaticInitializer final {
    SdkClassicLocaleStaticInitializer()
    {
        static_cast<void>(std::locale::classic());
    }
};
static_assert(std::is_trivially_destructible_v<SdkClassicLocaleStaticInitializer>);

const SdkClassicLocaleStaticInitializer gSdkClassicLocale55315C;

const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553160;
const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553164;

} // namespace

namespace {

// 原506350固定D68CF8→506460；仅十个CryptInt按原序构造，不动鼠标其他字段。
LegacyCryptInt* ConstructLegacyMouseRuntime_506350()
{
    legacy_crypt_int::ConstructZero_401C70(&gLegacyMouseState.mouseX);
    legacy_crypt_int::ConstructZero_401C70(&gLegacyMouseState.mouseY);
    for (LegacyCryptInt& coordinate : gLegacyMouseState.buttonCoordinates) {
        legacy_crypt_int::ConstructZero_401C70(&coordinate);
    }
    return &gLegacyMouseState.mouseX;
}

// 原506370固定D68CF8→506380；整体退出按八坐标/Y/X逆序，只原析构动作。
void ReleaseLegacyMouseRuntimeAtExit_506370()
{
    for (std::size_t index = 8; index != 0; --index) {
        legacy_crypt_int::Destroy_401CF0(&gLegacyMouseState.buttonCoordinates[index - 1]);
    }
    legacy_crypt_int::Destroy_401CF0(&gLegacyMouseState.mouseY);
    legacy_crypt_int::Destroy_401CF0(&gLegacyMouseState.mouseX);
}

} // namespace

// 原CRT 5530F8：构造后原整体退出登记，完整S32透传。
// 原函数：0x00506340。
std::int32_t InitializeLegacyMouseRuntime_506340()
{
    ConstructLegacyMouseRuntime_506350();
    return std::atexit(ReleaseLegacyMouseRuntimeAtExit_506370);
}

// 原函数：0x00506870。
std::int32_t ResetLegacyMouseState_506870()
{
    std::memset(&gLegacyMouseState, 0, sizeof(gLegacyMouseState));
    gValueD68DC4 = 0;
    gLegacyMouseState.valueD68DB8 = -1;
    return 0;
}

// 原函数：0x00506890。
void ResetLegacyMousePosition_506890(const std::int32_t x, const std::int32_t y)
{
    LegacyCryptInt temporary;
    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
    const std::uint32_t xEncoded = temporary.encoded;
    const std::uint32_t xMask = temporary.mask;
    const std::uint8_t xReadCount = temporary.readCount;
    gLegacyMouseState.mouseX.encoded = xEncoded;
    gLegacyMouseState.mouseX.mask = xMask;
    gLegacyMouseState.mouseX.readCount = xReadCount;
    legacy_crypt_int::Destroy_401CF0(&temporary);

    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
    const std::uint8_t yReadCount = temporary.readCount;
    const std::uint32_t yEncoded = temporary.encoded;
    const std::uint32_t yMask = temporary.mask;
    gLegacyMouseState.mouseY.readCount = yReadCount;
    gLegacyMouseState.mouseY.encoded = yEncoded;
    gLegacyMouseState.mouseY.mask = yMask;
    legacy_crypt_int::Destroy_401CF0(&temporary);
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

    LegacyCryptInt temporary;
    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
    const std::uint32_t xMask = temporary.mask;
    const std::uint32_t xEncoded = temporary.encoded;
    const std::uint8_t xReadCount = temporary.readCount;
    gLegacyMouseState.buttonCoordinates[0].mask = xMask;
    gLegacyMouseState.buttonCoordinates[0].encoded = xEncoded;
    gLegacyMouseState.buttonCoordinates[0].readCount = xReadCount;
    legacy_crypt_int::Destroy_401CF0(&temporary);

    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
    const std::uint32_t yMask = temporary.mask;
    const std::uint32_t yEncoded = temporary.encoded;
    const std::uint8_t yReadCount = temporary.readCount;
    gLegacyMouseState.buttonCoordinates[1].mask = yMask;
    gLegacyMouseState.buttonCoordinates[1].encoded = yEncoded;
    gLegacyMouseState.buttonCoordinates[1].readCount = yReadCount;
    legacy_crypt_int::Destroy_401CF0(&temporary);

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

    LegacyCryptInt temporary;
    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
    const std::uint32_t xEncoded = temporary.encoded;
    const std::uint32_t xMask = temporary.mask;
    const std::uint8_t xReadCount = temporary.readCount;
    gLegacyMouseState.buttonCoordinates[2].encoded = xEncoded;
    gLegacyMouseState.buttonCoordinates[2].mask = xMask;
    gLegacyMouseState.buttonCoordinates[2].readCount = xReadCount;
    legacy_crypt_int::Destroy_401CF0(&temporary);

    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
    const std::uint8_t yReadCount = temporary.readCount;
    const std::uint32_t yEncoded = temporary.encoded;
    const std::uint32_t yMask = temporary.mask;
    gLegacyMouseState.buttonCoordinates[3].readCount = yReadCount;
    gLegacyMouseState.buttonCoordinates[3].encoded = yEncoded;
    gLegacyMouseState.buttonCoordinates[3].mask = yMask;
    legacy_crypt_int::Destroy_401CF0(&temporary);

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

    LegacyCryptInt temporary;
    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
    const std::uint32_t xEncoded = temporary.encoded;
    const std::uint32_t xMask = temporary.mask;
    const std::uint8_t xReadCount = temporary.readCount;
    gLegacyMouseState.buttonCoordinates[0].encoded = xEncoded;
    gLegacyMouseState.buttonCoordinates[0].mask = xMask;
    gLegacyMouseState.buttonCoordinates[0].readCount = xReadCount;
    legacy_crypt_int::Destroy_401CF0(&temporary);

    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
    const std::uint8_t yReadCount = temporary.readCount;
    const std::uint32_t yEncoded = temporary.encoded;
    const std::uint32_t yMask = temporary.mask;
    gLegacyMouseState.buttonCoordinates[1].readCount = yReadCount;
    gLegacyMouseState.buttonCoordinates[1].encoded = yEncoded;
    gLegacyMouseState.buttonCoordinates[1].mask = yMask;
    legacy_crypt_int::Destroy_401CF0(&temporary);

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

    LegacyCryptInt temporary;
    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
    const std::uint32_t xMask = temporary.mask;
    const std::uint32_t xEncoded = temporary.encoded;
    const std::uint8_t xReadCount = temporary.readCount;
    gLegacyMouseState.buttonCoordinates[4].mask = xMask;
    gLegacyMouseState.buttonCoordinates[4].encoded = xEncoded;
    gLegacyMouseState.buttonCoordinates[4].readCount = xReadCount;
    legacy_crypt_int::Destroy_401CF0(&temporary);

    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
    const std::uint32_t yMask = temporary.mask;
    const std::uint32_t yEncoded = temporary.encoded;
    const std::uint8_t yReadCount = temporary.readCount;
    gLegacyMouseState.buttonCoordinates[5].mask = yMask;
    gLegacyMouseState.buttonCoordinates[5].encoded = yEncoded;
    gLegacyMouseState.buttonCoordinates[5].readCount = yReadCount;
    legacy_crypt_int::Destroy_401CF0(&temporary);

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

    LegacyCryptInt temporary;
    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
    const std::uint32_t xEncoded = temporary.encoded;
    const std::uint32_t xMask = temporary.mask;
    const std::uint8_t xReadCount = temporary.readCount;
    gLegacyMouseState.buttonCoordinates[6].encoded = xEncoded;
    gLegacyMouseState.buttonCoordinates[6].mask = xMask;
    gLegacyMouseState.buttonCoordinates[6].readCount = xReadCount;
    legacy_crypt_int::Destroy_401CF0(&temporary);

    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
    const std::uint8_t yReadCount = temporary.readCount;
    const std::uint32_t yEncoded = temporary.encoded;
    const std::uint32_t yMask = temporary.mask;
    gLegacyMouseState.buttonCoordinates[7].readCount = yReadCount;
    gLegacyMouseState.buttonCoordinates[7].encoded = yEncoded;
    gLegacyMouseState.buttonCoordinates[7].mask = yMask;
    legacy_crypt_int::Destroy_401CF0(&temporary);

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

    LegacyCryptInt temporary;
    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, x);
    const std::uint32_t xEncoded = temporary.encoded;
    const std::uint32_t xMask = temporary.mask;
    const std::uint8_t xReadCount = temporary.readCount;
    gLegacyMouseState.buttonCoordinates[4].encoded = xEncoded;
    gLegacyMouseState.buttonCoordinates[4].mask = xMask;
    gLegacyMouseState.buttonCoordinates[4].readCount = xReadCount;
    legacy_crypt_int::Destroy_401CF0(&temporary);

    legacy_crypt_int::ConstructFromInt_401CC0(&temporary, y);
    const std::uint8_t yReadCount = temporary.readCount;
    const std::uint32_t yEncoded = temporary.encoded;
    const std::uint32_t yMask = temporary.mask;
    gLegacyMouseState.buttonCoordinates[5].readCount = yReadCount;
    gLegacyMouseState.buttonCoordinates[5].encoded = yEncoded;
    gLegacyMouseState.buttonCoordinates[5].mask = yMask;
    legacy_crypt_int::Destroy_401CF0(&temporary);

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
