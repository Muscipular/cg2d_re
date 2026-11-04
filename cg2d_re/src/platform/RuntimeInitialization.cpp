// 此文件按原CRT表顺序集中构造跨模块状态；多个 namespace 为保序例外。
// 不按模块分拆或重排，RNG消费和逆序析构依赖同一翻译单元。
#include "cg/platform/LegacyMouse.hpp"

#include "../scene/login/LoginMessageDependencies.hpp"
#include "../world/WorldObjectState.hpp"
#include "MainLoopDependencies.hpp"
#include "cg/resource/AnimationRuntime.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/GraphicRead.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/session/ClientHost.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <locale>
#include <type_traits>

namespace cg::platform {

// 原CRT553004..553010：保留以下两值的构造顺序；其后三个防护值按PRM01移除。
// 原0x00401C10、0x00401C20：5939C8→5939D8；原0x00401C50退出反序，由MSVC承接。
static LegacyCryptInt gStartupValue5939C8{}; // 原全局：0x005939C8。
static LegacyCryptInt gStartupValue5939D8{}; // 原全局：0x005939D8。

} // namespace cg::platform

namespace cg::world::detail {

// 原CRT553014，原0x0043D760／0x0043D770／0x0043D7A0：中央原位置初始化，实际寿命归会话。
// 原0x0053C45E／0x0053C4E0／0x0053C560／0x005405F5：数组寿命及完成前缀清理由C++／MSVC承接。
// 原0x0043D7C0／0x0043D810：千项各两CryptInt按成员声明序构造、退出双逆序。
struct WorldObjectRuntimeStartup final {
    WorldObjectRuntimeStartup()
    {
        InitializeWorldObjectStorage(cg::session::GetClientHost().FirstSession());
    }
};
static_assert(std::is_trivially_destructible_v<WorldObjectRuntimeStartup>);
static const WorldObjectRuntimeStartup gWorldObjectRuntimeStartup;

} // namespace cg::world::detail

namespace cg::platform {

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
static SurfaceTransformCacheStartup gSurfaceTransformCacheStartup;

// 原5495EE不创建sentry；直接同步缓冲，避免现代flush额外刷新tie。
template <typename Character>
static void FlushLegacyStandardStream(std::basic_ostream<Character>& stream)
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

static const NarrowStandardStreamsStaticInitializer gNarrowStandardStreamsStaticInitializer;

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

static const WideStandardStreamsStaticInitializer gWideStandardStreamsStaticInitializer;

// 原五个CRT入口共用的W8标记；保留其他位，不为各包装另建状态。
static std::uint8_t gLegacyPrimaryExitGuard1086760 = 0; // 原全局：0x01086760。
static std::uint8_t gLegacySecondaryExitGuard1087AFC = 0; // 原全局：0x01087AFC。

// 原536960、54843B各只有RET；这两个回调确实不执行清理业务。
static void EmptyLegacyExit_536960()
{
}

// 原函数：0x0054843B。
static void EmptyLegacyExit_54843B()
{
}

// 原5491BE/549F4A/54A662/54A9AB：各次均真实登记并返回结果。
static std::int32_t RegisterLegacyPrimaryEmptyExit()
{
    return std::atexit(EmptyLegacyExit_536960);
}

// 原函数：0x00454C00。
static std::int32_t InitializeLegacyEmptyExit_454C00()
{
    if ((gLegacyPrimaryExitGuard1086760 & 1u) == 0) {
        gLegacyPrimaryExitGuard1086760 |= 1u;
    }
    return RegisterLegacyPrimaryEmptyExit();
}

// 原549194/549F20/54A638/54A981：第一次登记失败也继续第二次，不回滚标记。
static std::int32_t RegisterLegacyEmptyExitPair()
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

static const LegacyPrimaryEmptyExitStaticInitializer gLegacyPrimaryEmptyExitStaticInitializer;

} // namespace cg::platform

namespace cg::world {

// 原456FD0至467EF0：原位置按声明顺序构造十七CryptInt，实际寿命归会话。
struct WorldMapCryptRuntimeStartup final {
    WorldMapCryptRuntimeStartup()
    {
        InitializeWorldMapCryptStorage(cg::session::GetClientHost().FirstSession());
    }
};
static_assert(std::is_trivially_destructible_v<WorldMapCryptRuntimeStartup>);
static const WorldMapCryptRuntimeStartup gWorldMapCryptRuntimeStartup;

} // namespace cg::world

namespace cg::platform {

// 原CRT 553098紧接553094 Ripple；不增加RNG调用或析构步骤。
struct WindowNumberSpacingStaticInitializer {
    WindowNumberSpacingStaticInitializer()
    {
        cg::ui::WindowNumber_InitializeLegacySpacing();
    }
};

static const WindowNumberSpacingStaticInitializer gWindowNumberSpacingStaticInitializer;

// 原55309C/5530A0相邻，两对象各十CryptInt；各自构造／析构。
// 原函数：0x004883F0；同一中央位置构造，MSVC按完成逆序析构。
static const cg::ui::FirstItemSelectorStartup gFirstItemSelectorStartup {
    cg::session::GetClientHost().FirstSession()};

// 原函数：0x004998F0；同一中央位置构造，MSVC按完成逆序析构。
static const cg::ui::SecondItemSelectorStartup gSecondItemSelectorStartup {
    cg::session::GetClientHost().FirstSession()};

// 原CRT 5530A4；浏览宠物拥有者接管构析，不与玩家宠物共享。
// 原函数：0x004CD240；同一中央位置构造，MSVC按完成逆序析构。
static const cg::ui::StallBrowsePetValuesStartup gStallBrowsePetValuesStartup {
    cg::session::GetClientHost().FirstSession()};

// 原CRT5530F8，原0x00506340／0x00506350／0x00506370：同一中央位置初始化实际会话记录。
// 原0x00506460／0x00506530／0x00506380：X→Y→八坐标构造，退出成员逆序。
struct LegacyMouseRuntimeStartup final {
    LegacyMouseRuntimeStartup()
    {
        InitializeLegacyMouseStorage(cg::session::GetClientHost().FirstSession());
    }
};
static_assert(std::is_trivially_destructible_v<LegacyMouseRuntimeStartup>);
static const LegacyMouseRuntimeStartup gLegacyMouseRuntimeStartup;

} // namespace cg::platform

namespace cg::world {

// 原CRT5530FC，原0x005183D0／0x005183E0／0x00518400：紧接鼠标，玩家实际寿命归会话。
// 原0x005184A0／0x00518410：六CryptInt原声明序构造，普通字段保留零初值。
struct PlayerRuntimeStartup final {
    PlayerRuntimeStartup()
    {
        InitializePlayerRuntimeStorage(cg::session::GetClientHost().FirstSession());
    }
};
static_assert(std::is_trivially_destructible_v<PlayerRuntimeStartup>);
static const PlayerRuntimeStartup gPlayerRuntimeStartup;

// 原CRT553100，原0x00518520／0x00518530／0x00518560：紧接玩家，独立于摊位浏览宠物。
// 原0x004CD360／0x004CD2D0：五元素各六成员构造；实际寿命归所属会话。
struct PetRuntimeStartup final {
    PetRuntimeStartup()
    {
        InitializePetRuntimeStorage(cg::session::GetClientHost().FirstSession());
    }
};
static_assert(std::is_trivially_destructible_v<PetRuntimeStartup>);
static const PetRuntimeStartup gPetRuntimeStartup;

} // namespace cg::world

namespace cg::platform {

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
static IndependentPaletteContainersStartup gIndependentPaletteContainersStartup;

// 原后段CRT553130/553158/553160/553164；各自保留两次登记，按原相对顺序执行。
struct LegacyPairedEmptyExitStaticInitializer {
    LegacyPairedEmptyExitStaticInitializer()
    {
        static_cast<void>(RegisterLegacyEmptyExitPair());
    }
};

static const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553130; // 原全局：0x00553130。

// SLN01原553134：独立完整SDK Init对象持有窄流生命周期。
// 自动析构由SDK承接；不另登记旧5491F6或改Legacy计数/登记表。
static std::ios_base::Init gSdkNarrowLateInit553134;

static const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553158; // 原全局：0x00553158。

// SCL01原55315C：晚段经典对象职责由宿主完整locale持有并清理。
// 不另登记旧54A3FA，也不将SDK对象写入Legacy三槽。
struct SdkClassicLocaleStaticInitializer final {
    SdkClassicLocaleStaticInitializer()
    {
        static_cast<void>(std::locale::classic());
    }
};
static_assert(std::is_trivially_destructible_v<SdkClassicLocaleStaticInitializer>);

static const SdkClassicLocaleStaticInitializer gSdkClassicLocale55315C; // 原全局：0x0055315C。

static const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553160; // 原全局：0x00553160。
static const LegacyPairedEmptyExitStaticInitializer gLegacyPairedEmptyExit553164; // 原全局：0x00553164。

} // namespace cg::platform
