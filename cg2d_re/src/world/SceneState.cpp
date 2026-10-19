#include "cg/core/LegacyNoOp.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "../scene/login/LoginSavedState.hpp"
#include "cg/render/Font.hpp"
#include "cg/ui/HitTest.hpp"
#include "../platform/StartupNative.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/text/Encoding.hpp"

#include "../scene/character_select/CharacterSelectState.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "../scene/login/LoginInputDependencies.hpp"
#include "../scene/login/LoginRequestDependencies.hpp"
#include "../scene/login/LoginSceneDependencies.hpp"
#include "../ui/WindowRuntimeState.hpp"
#include "cg/input/Input.hpp"
#include "cg/world/Blacklist.hpp"
#include "cg/world/GuildState.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Palette.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/GraphicAlloc.hpp"
#include "cg/resource/ResourcePaths.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/render/GraphicDxCache.hpp"
#include "cg/render/SurfaceState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/GameTime.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "../platform/MapEffectMath.hpp"

#include "cg/audio/LegacyAudio.hpp"
#include "AlbumState.hpp"
#include "BattleState.hpp"
#include "ChatHistory.hpp"
#include "MailState.hpp"
#include "PetTechniqueRecord.hpp"
#include "PlayerState.hpp"
#include "WorldMapState.hpp"
#include "WorldObjectState.hpp"
#include "WorldUpdateDependencies.hpp"

#include <ctime>
#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <direct.h>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BC41、0x0053BCCD、0x0053C680、0x0053C8B3、0x0053CFDC；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053CADC；直接沿MSVC CRT。
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053C654、0x0053BFC0、0x0053BEC0、0x0053BE40；原参数／返回合同由直接调用承接。
#include <cstring>
#include <new>
#include <optional>
#include <utility>
#include <vector>

namespace cg::world {

// 原 [C8A7B0,C8A940) 的 100 项 W32 表；C8A940 是独立消费游标。
static std::array<std::int32_t, 100> gRotationTableC8A7B0 {}; // 原全局：0x00C8A7B0。
static std::int32_t gRotationCursorC8A940 = 0; // 原全局：0x00C8A940。
static std::uint32_t gValue62B348 = 0; // 原全局：0x0062B348。
static std::int32_t gValueFF0D44 = 0; // 原全局：0x00FF0D44。 原普通场景与结果窗口分派共用的写入槽。
static std::int32_t gWorldSceneInitialized_FF0D50 = 0; // 原全局：0x00FF0D50。 原普通场景初始化标志，11分支只认恰1。
static std::int32_t gWorldSceneWaitFrames_FF0D54 = 0; // 原全局：0x00FF0D54。 原普通场景等待帧数，门槛按S32比较。
static std::int32_t gWorldBgmResumeIndex_FF0D68 = 0; // 原全局：0x00FF0D68。 原普通场景BGM恢复号，不是当前播放号owner。
static std::uint32_t gLogoutTransition9A6E94 = 0; // 原全局：0x009A6E94。 原45EBB0/45EEE0共享过渡阶段。
static std::wstring gLogoutText9A5B08; // 原全局：0x009A5B08。 动态UTF-16消息，排他尾9A5C10独立。
static cg::yobi::YobiAction* gBattleExitAction5ABCDC = nullptr; // 原全局：0x005ABCDC。 原非拥有退出面板；43CBB0清槽并保留非空交互尾。
static std::int32_t gBattleSceneIndex5ABFD8 = 0; // 原全局：0x005ABFD8。 原场景循环游标，回调之后重读，非对象地址。
// 原51D790/51E0D0共用的行数据；第480项独立于原前480项清零范围。
struct TransitionLines {
    std::vector<float> speedFF0FA8 = std::vector<float>(481, 0.0f); // 原全局：0x00FF0FA8。
    std::vector<float> xFF172C = std::vector<float>(481, 0.0f); // 原全局：0x00FF172C。
    std::vector<float> yFF1EB0 = std::vector<float>(481, 0.0f); // 原全局：0x00FF1EB0。
};
// R02：私有行存储随已发布高度增长，原地址只标识数据身份，不约束现代容器布局。
static TransitionLines gTransitionLines {};
static float gTransitionXFF48E8 = 0.0f; // 原全局：0x00FF48E8。
static float gTransitionAlternateXFF48EC = 0.0f; // 原全局：0x00FF48EC。
static float gTransitionYFF48F0 = 0.0f; // 原全局：0x00FF48F0。
static float gTransitionAlternateYFF48F4 = 0.0f; // 原全局：0x00FF48F4。
static float gTransitionVelocityFF48F8 = 0.0f; // 原全局：0x00FF48F8。
static float gTransitionActiveLinesFF48FC = 0.0f; // 原全局：0x00FF48FC。
static std::int32_t gReverseTransitionPhase58B6D4 = -1; // 原全局：0x0058B6D4。 原加载区初值FFFFFFFF。
static std::int32_t gReverseTransitionActiveLinesFF276C = 0; // 原全局：0x00FF276C。
static float gReverseTransitionVelocityFF47C4 = 0.0f; // 原全局：0x00FF47C4。
static float gReverseTransitionXFF4900 = 0.0f; // 原全局：0x00FF4900。
static float gReverseTransitionAlternateXFF4904 = 0.0f; // 原全局：0x00FF4904。
static float gReverseTransitionYFF4908 = 0.0f; // 原全局：0x00FF4908。
static float gReverseTransitionAlternateYFF490C = 0.0f; // 原全局：0x00FF490C。
static std::int32_t gQuadrantTransitionPhase58B6D8 = -1; // 原全局：0x0058B6D8。 原加载区初值FFFFFFFF。
static std::array<std::int32_t, 4> gQuadrantTransitionXFF275C {}; // 原全局：0x00FF275C。
static std::array<std::int32_t, 4> gQuadrantTransitionYFF47D0 {}; // 原全局：0x00FF47D0。
static_assert(sizeof(gQuadrantTransitionXFF275C) == 16);
static_assert(sizeof(gQuadrantTransitionYFF47D0) == 16);
// 原51EF80的64画块同源状态；阶段加载初值-1，其余为未加载区零初值。
// 状态1/2的含义随显隐/下落模式变化，源坐标与运动坐标不可合并。
static std::int32_t gTileTransitionPhase58B6DC = -1; // 原全局：0x0058B6DC。
static std::int32_t gTileTransitionCompletedFF46C0 = 0; // 原全局：0x00FF46C0。
static std::int32_t gTileTransitionStartedFF28BC = 0; // 原全局：0x00FF28BC。
static std::array<std::int32_t, 64> gTileTransitionStateFF27B8 {}; // 原全局：0x00FF27B8。
static std::array<std::int32_t, 64> gTileTransitionSpeedFF46C4 {}; // 原全局：0x00FF46C4。
static std::array<std::int32_t, 64> gTileTransitionSourceYFF47E4 {}; // 原全局：0x00FF47E4。
static std::array<std::int32_t, 64> gTileTransitionYFF0D7C {}; // 原全局：0x00FF0D7C。
static std::array<std::int32_t, 64> gTileTransitionXFF265C {}; // 原全局：0x00FF265C。
static std::array<std::int32_t, 64> gTileTransitionSourceXFF0E7C {}; // 原全局：0x00FF0E7C。

// 原51F610/51F860/51FA70共用，不随入口拆成独立比例或初始化标志。
static std::uint32_t gImageTransitionInitializedFF4910 = 0; // 原全局：0x00FF4910。
static std::int32_t gImageTransitionScaleFF0D78 = 0; // 原全局：0x00FF0D78。
static std::int32_t gHalfTransitionScaleFF0F7C = 0; // 原全局：0x00FF0F7C。 原51FC90独立比例，不属于源X数组。
struct GridTransitionOffset {
    std::int32_t x;
    std::int32_t y;
};
static_assert(sizeof(GridTransitionOffset) == 8);
static_assert(offsetof(GridTransitionOffset, y) == 4);
static std::array<GridTransitionOffset, 960> gGridTransitionOffsetsFF28C0 {}; // 原全局：0x00FF28C0。
static_assert(sizeof(gGridTransitionOffsetsFF28C0) == 0xFF46C0 - 0xFF28C0);

// 原51EF80四种初始化的共同网格写序，不在此修改阶段/计数或消费随机数。
static void InitializeTileTransitionGrid(const std::int32_t state, const bool aboveScreen)
{
    for (std::int32_t row = 0; row < 8; ++row) {
        const std::int32_t first = row * 8;
        const std::int32_t y = row * 60;
        std::fill_n(gTileTransitionStateFF27B8.begin() + first, 8, state);
        std::fill_n(gTileTransitionSpeedFF46C4.begin() + first, 8, 0);
        std::fill_n(gTileTransitionSourceYFF47E4.begin() + first, 8, y);
        std::fill_n(gTileTransitionYFF0D7C.begin() + first, 8, aboveScreen ? -60 : y);
        for (std::int32_t column = 0; column < 8; ++column) {
            const std::int32_t index = first + column;
            const std::int32_t x = column * 80;
            gTileTransitionXFF265C[index] = x;
            gTileTransitionSourceXFF0E7C[index] = x;
        }
    }
}

// 原__ftol先截断为S64，再消费低S32；4535B0另取低S16坐标。
static std::int32_t TruncateTransitionCoordinate(const double value)
{
    return static_cast<std::int32_t>(static_cast<std::int64_t>(value));
}

// R09：转场沿640x480计算，仅在复制时换算到同尺寸的Temp／Back。
// Clipped保原S16裁剪入口；Direct保原S32直接复制及资源失败边界。
enum class TransitionCopyMode {
    Clipped,
    Direct,
};

static void DrawTransitionRectangle(
    const std::int32_t x, const std::int32_t y,
    const std::array<std::int32_t, 4>& rectangle, render::TextureSurface* const source,
    const TransitionCopyMode copyMode = TransitionCopyMode::Clipped)
{
    const std::int32_t width = render::GetCpuSurfaceWidth();
    const std::int32_t height = render::GetCpuSurfaceHeight();
    const auto scaleX = [width](const std::int32_t coordinate) {
        return static_cast<std::int32_t>(static_cast<std::int64_t>(coordinate) * width / 640);
    };
    const auto scaleY = [height](const std::int32_t coordinate) {
        return static_cast<std::int32_t>(static_cast<std::int64_t>(coordinate) * height / 480);
    };
    std::array<std::int32_t, 4> actualRectangle {
        scaleX(rectangle[0]), scaleY(rectangle[1]),
        scaleX(rectangle[2]), scaleY(rectangle[3])
    };
    if (copyMode == TransitionCopyMode::Clipped) {
        static_cast<void>(render::BlitSurfaceToBack_4535B0(
            static_cast<std::int16_t>(scaleX(x)), static_cast<std::int16_t>(scaleY(y)),
            actualRectangle, source));
    } else {
        static_cast<void>(render::BlitSurfaceRectangleToBack(
            scaleX(x), scaleY(y), actualRectangle, source));
    }
}

static_assert(sizeof(gRotationTableC8A7B0) == 0x190);

// 原 [9E03D0,9E0434)：46AC10先写前缀，再按发送门禁决定是否写NUL。
static std::array<char, 100> gMoveDirectionText9E03D0 {}; // 原全局：0x009E03D0。
static_assert(sizeof(gMoveDirectionText9E03D0) == 100);

// 原 [DA43E8,DE03F4)：相册头与两组记录的唯一持久对象；见场景证据 §58。
struct AlbumStorage {
    std::array<char, 4> signature; // +00：CCA及NUL。
    std::array<std::int32_t, 2> versions; // +04：两角色的回复版本。
    std::array<AlbumRecord, 2 * 960> records; // +0C：两组连续记录。
};
static_assert(std::is_standard_layout_v<AlbumStorage>);
static_assert(std::is_trivially_copyable_v<AlbumStorage>);
static_assert(offsetof(AlbumStorage, signature) == 0);
static_assert(offsetof(AlbumStorage, versions) == 4);
static_assert(offsetof(AlbumStorage, records) == 0x0C);
static_assert(sizeof(AlbumStorage) == 0x3C00C);
static_assert(alignof(AlbumStorage) == 4);
static AlbumStorage gAlbumStorage {};

// 借用唯一相册记录；index从0开始，selected保持原S16符号扩展。
static AlbumRecord* AlbumRecordAt(const std::int32_t index, const std::int16_t selected)
{
    const std::ptrdiff_t record = index + static_cast<std::ptrdiff_t>(selected) * 960;
    return gAlbumStorage.records.data() + record;
}

// 原62B5B8；455FC0持有非拥有动画引用，4758D0经借用槽地址完成清空。
static cg::yobi::YobiAction* gWindow39Action62B5B8 = nullptr;

// 原FF0C34三条29B名称与FF0D0C三个图号；结果展示须借用同一存储。

static constexpr std::array<std::int32_t, 6> kAllowedMapFloors {
    59000,
    59001,
    59100,
    37623,
    37619,
    37631,
};

// 原 0x5541F4；IsMapDriftEffectAllowed_4493C0 按原顺序匹配的四项地图楼层表。
static constexpr std::array<std::int32_t, 4> kMapFloors5541F4 {
    59000,
    59001,
    37623,
    37619,
};

static constexpr std::array<std::int32_t, 18> kEmoteLocalActions584B6C { // 原常量参考：0x00584B6C。
    11, 0, 12, 1, 16, 10, 13, 5, 14, 9, 15, 8, 7, 6, 18, 3, 19, 17,
};
static constexpr std::array<std::int32_t, 18> kEmoteProtocolActions584BB4 { // 原常量参考：0x00584BB4。
    11, 31, 12, 30, 16, 10, 13, 5, 14, 9, 15, 8, 7, 6, 18, 3, 19, 17,
};
static std::uint64_t gEmoteRequestTickD67D20 = 0; // 原全局：0x00D67D20。

struct DirectionDelta {
    std::int16_t x;
    std::int16_t y;
};

// 原 .data [0x55D9DC,0x55D9FC)：八组可写的方向 W16 分量。
static std::array<DirectionDelta, 8> gDirectionDeltas55D9DC {{ // 原全局：0x0055D9DC。
    {0, -1}, {1, -1}, {1, 0}, {1, 1},
    {0, 1}, {-1, 1}, {-1, 0}, {-1, -1},
}};

// 原第四清零块[F1BAE0,F633EC)，既有技能/索引表改为本owner成员别名。
struct SkillRuntimeSlots {
    std::array<char, 0x1C> jobName;
    std::array<SkillRuntimeRecord, kSkillSlotCount> skills;
    std::array<SkillPairRecord, kSkillSlotCount> pairs;
    std::uint32_t valueF633E8; // 原全局：0x00F633E8。 509E30只写低U8，保留其余字节。
};

static_assert(std::is_standard_layout_v<SkillRuntimeSlots>);
static_assert(std::is_trivially_copyable_v<SkillRuntimeSlots>);
static_assert(offsetof(SkillRuntimeSlots, jobName) == 0);
static_assert(offsetof(SkillRuntimeSlots, skills) == 0x1C);
// 原偏移0x47890／0x47908、总大小0x4790C；现按扩容后的成员连续布局核对。
static_assert(offsetof(SkillRuntimeSlots, pairs) == 0x1C + sizeof(SkillRuntimeRecord) * kSkillSlotCount);
static_assert(offsetof(SkillRuntimeSlots, valueF633E8) == offsetof(SkillRuntimeSlots, pairs) + sizeof(SkillPairRecord) * kSkillSlotCount);
static_assert(sizeof(SkillRuntimeSlots) == offsetof(SkillRuntimeSlots, valueF633E8) + sizeof(std::uint32_t));
static SkillRuntimeSlots gSkillRuntimeSlots {};

static std::uint64_t gChatPreviousTickDA3B3C = 0; // 原全局：0x00DA3B3C。
static std::uint64_t gChatCurrentTickDA3B40 = 0; // 原全局：0x00DA3B40。
static std::uint8_t gChatDisabledDA3B4C = 0; // 原全局：0x00DA3B4C。
static std::int32_t gChatGateDA3B54 = 0; // 原全局：0x00DA3B54。 50CAE0写入，50C940读取同一接收门禁。
// 原 [FF02E0,FF03D0) 的五槽表；与玩家首块分属不同清零区。
struct PetMapCharaSlot {
    std::uint16_t flags;
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::uint32_t objectIndex;
    std::int32_t level08; // 原成员偏移：+0x08。 50B380玩家分支同步第0槽。
    std::int32_t value0C; // 原成员偏移：+0x0C。 50A3E0按掩码0x11写。
    std::int32_t value10; // 原成员偏移：+0x10。 50A3E0按掩码0x09写，在value0C之前消费。
    std::int32_t value14; // 原成员偏移：+0x14。 50A3E0按掩码0x21写。
    std::array<char, 17> name18; // 原+18，旧编码名称。
    std::array<std::byte, 3> bytes29; // 原成员偏移：+0x29。
    cg::yobi::YobiAction* action; // 原 +2C，非拥有。
};

static_assert(offsetof(PetMapCharaSlot, flags) == 0);
static_assert(offsetof(PetMapCharaSlot, objectIndex) == 4);
static_assert(offsetof(PetMapCharaSlot, level08) == 8);
static_assert(offsetof(PetMapCharaSlot, value0C) == 0x0C);
static_assert(offsetof(PetMapCharaSlot, value10) == 0x10);
static_assert(offsetof(PetMapCharaSlot, value14) == 0x14);
static_assert(offsetof(PetMapCharaSlot, name18) == 0x18);
static_assert(offsetof(PetMapCharaSlot, action) == 0x30);
static_assert(sizeof(PetMapCharaSlot) == 0x38);
static std::array<PetMapCharaSlot, 5> gPetMapCharaSlots {};
// 原 [9FB938,9FB942)：两次W32与末次W16清理、后续S16读取共用同一10B。
static alignas(std::uint32_t) std::array<std::uint16_t, 5> gPetFollowOrder {};
static_assert(sizeof(gPetFollowOrder) == 10);

// 原 C8A73E/C8A740/C8A744 及效果配置共享槽；第三槽的两个 W16 共用 W32。
struct MapCharaSceneSlots {
    std::uint16_t valueC8A73C; // 原全局：0x00C8A73C。
    std::uint16_t valueC8A73E; // 原全局：0x00C8A73E。
    std::uint16_t valueC8A740; // 原全局：0x00C8A740。
    std::uint16_t valueC8A742; // 原全局：0x00C8A742。
    std::uint32_t valueC8A744; // 原全局：0x00C8A744。
    std::uint16_t valueC8A748; // 原全局：0x00C8A748。
    std::uint16_t value55DE98 {0xFFFF}; // 原全局：0x0055DE98。
    std::uint16_t valueC8A74A; // 原全局：0x00C8A74A。
    std::uint16_t valueC8A74C; // 原全局：0x00C8A74C。
    std::uint16_t valueC8A74E; // 原全局：0x00C8A74E。
    std::uint16_t valueC8A750; // 原全局：0x00C8A750。
    std::uint16_t valueC8A752; // 原全局：0x00C8A752。
    std::uint16_t valueC8A754; // 原全局：0x00C8A754。
    std::uint16_t valueC8A756; // 原全局：0x00C8A756。
    std::uint32_t valueC8A758; // 原全局：0x00C8A758。
    std::uint32_t valueC8A75C; // 原全局：0x00C8A75C。
    std::int16_t valueC8A760; // 原全局：0x00C8A760。
    std::int16_t valueC8A762; // 原全局：0x00C8A762。
    cg::yobi::YobiAction* actionC8A764; // 原全局：0x00C8A764。
    std::uint32_t valueC8A768 {}; // 原全局：0x00C8A768。 原PE未初始化区装载清零；413070仅覆写命中半字。
    std::uint32_t valueC8A76C {}; // 原全局：0x00C8A76C。
    float value55DE9C {5.0F}; // 原全局：0x0055DE9C。
    std::int16_t value55DEA0 {315}; // 原全局：0x0055DEA0。
    std::int16_t value55DEA2 {-1}; // 原全局：0x0055DEA2。
};

static MapCharaSceneSlots gMapCharaSceneSlots {};

// 原 .data [55DEB0,55DF10)：前/后三项分别供 46F1D0 的 kind 6/7 绘图。
static std::array<render::GraphicDescriptor16, 6> gEffectDrawModes55DEB0 {{ // 原全局：0x0055DEB0。
    {0, -240, 320, 290, 0xFFFFFFFFu, 1, cg::render::GraphicMixFlag_None, 0},
    {-320, 64, 320, 270, 0xFFFFFFFFu, 1, cg::render::GraphicMixFlag_None, 0},
    {0, 64, 320, 370, 0xFFFFFFFFu, 1, cg::render::GraphicMixFlag_None, 0},
    {-320, -480, 320, 290, 0xFFFFFFFFu, 1, cg::render::GraphicMixFlag_None, 0},
    {0, -480, 320, 240, 0xFFFFFFFFu, 1, cg::render::GraphicMixFlag_None, 0},
    {-320, -181, 320, 240, 0xFFFFFFFFu, 1, cg::render::GraphicMixFlag_None, 0},
}};

static_assert(sizeof(gEffectDrawModes55DEB0) == 0x60);

static constexpr char kCutReadMode554920[] = "rb"; // 原常量参考：0x00554920。

static constexpr wchar_t kCutCaption5536B8[] = L"确认"; // 原常量参考：0x005536B8。
static constexpr wchar_t kCutMessage55DE20[] = // 原常量参考：0x0055DE20。
    L"要检视FAQ吗？";
static constexpr wchar_t kCutCaption55DE30[] = L"档案错误"; // 原常量参考：0x0055DE30。

// 原 468520 路径与 4687B0 创建模式；与命令行 mapfolder: 配置无关。
static constexpr char kMapPathPrefix[] = "map";
static constexpr char kMapReadMode[] = "rb";
static constexpr char kUnknownText554590[] = "???"; // 原常量参考：0x00554590。

// 原 [9DAD30,9DAD4A)；地图回复的 24 字节正文和 NUL，首字节另有世界重置写者。
static std::array<std::uint8_t, 26> gText9DAD30 {}; // 原全局：0x009DAD30。

// 原 55DAAC/55DBAC；两表各 256 项，原始内容分别为逐位反转与 0x1021 多项式表。
static constexpr std::array<std::uint8_t, 256> kMapBitReverse55DAAC { // 原常量参考：0x0055DAAC。
    0x00u, 0x80u, 0x40u, 0xC0u, 0x20u, 0xA0u, 0x60u, 0xE0u, 0x10u, 0x90u, 0x50u, 0xD0u, 0x30u, 0xB0u, 0x70u, 0xF0u,
    0x08u, 0x88u, 0x48u, 0xC8u, 0x28u, 0xA8u, 0x68u, 0xE8u, 0x18u, 0x98u, 0x58u, 0xD8u, 0x38u, 0xB8u, 0x78u, 0xF8u,
    0x04u, 0x84u, 0x44u, 0xC4u, 0x24u, 0xA4u, 0x64u, 0xE4u, 0x14u, 0x94u, 0x54u, 0xD4u, 0x34u, 0xB4u, 0x74u, 0xF4u,
    0x0Cu, 0x8Cu, 0x4Cu, 0xCCu, 0x2Cu, 0xACu, 0x6Cu, 0xECu, 0x1Cu, 0x9Cu, 0x5Cu, 0xDCu, 0x3Cu, 0xBCu, 0x7Cu, 0xFCu,
    0x02u, 0x82u, 0x42u, 0xC2u, 0x22u, 0xA2u, 0x62u, 0xE2u, 0x12u, 0x92u, 0x52u, 0xD2u, 0x32u, 0xB2u, 0x72u, 0xF2u,
    0x0Au, 0x8Au, 0x4Au, 0xCAu, 0x2Au, 0xAAu, 0x6Au, 0xEAu, 0x1Au, 0x9Au, 0x5Au, 0xDAu, 0x3Au, 0xBAu, 0x7Au, 0xFAu,
    0x06u, 0x86u, 0x46u, 0xC6u, 0x26u, 0xA6u, 0x66u, 0xE6u, 0x16u, 0x96u, 0x56u, 0xD6u, 0x36u, 0xB6u, 0x76u, 0xF6u,
    0x0Eu, 0x8Eu, 0x4Eu, 0xCEu, 0x2Eu, 0xAEu, 0x6Eu, 0xEEu, 0x1Eu, 0x9Eu, 0x5Eu, 0xDEu, 0x3Eu, 0xBEu, 0x7Eu, 0xFEu,
    0x01u, 0x81u, 0x41u, 0xC1u, 0x21u, 0xA1u, 0x61u, 0xE1u, 0x11u, 0x91u, 0x51u, 0xD1u, 0x31u, 0xB1u, 0x71u, 0xF1u,
    0x09u, 0x89u, 0x49u, 0xC9u, 0x29u, 0xA9u, 0x69u, 0xE9u, 0x19u, 0x99u, 0x59u, 0xD9u, 0x39u, 0xB9u, 0x79u, 0xF9u,
    0x05u, 0x85u, 0x45u, 0xC5u, 0x25u, 0xA5u, 0x65u, 0xE5u, 0x15u, 0x95u, 0x55u, 0xD5u, 0x35u, 0xB5u, 0x75u, 0xF5u,
    0x0Du, 0x8Du, 0x4Du, 0xCDu, 0x2Du, 0xADu, 0x6Du, 0xEDu, 0x1Du, 0x9Du, 0x5Du, 0xDDu, 0x3Du, 0xBDu, 0x7Du, 0xFDu,
    0x03u, 0x83u, 0x43u, 0xC3u, 0x23u, 0xA3u, 0x63u, 0xE3u, 0x13u, 0x93u, 0x53u, 0xD3u, 0x33u, 0xB3u, 0x73u, 0xF3u,
    0x0Bu, 0x8Bu, 0x4Bu, 0xCBu, 0x2Bu, 0xABu, 0x6Bu, 0xEBu, 0x1Bu, 0x9Bu, 0x5Bu, 0xDBu, 0x3Bu, 0xBBu, 0x7Bu, 0xFBu,
    0x07u, 0x87u, 0x47u, 0xC7u, 0x27u, 0xA7u, 0x67u, 0xE7u, 0x17u, 0x97u, 0x57u, 0xD7u, 0x37u, 0xB7u, 0x77u, 0xF7u,
    0x0Fu, 0x8Fu, 0x4Fu, 0xCFu, 0x2Fu, 0xAFu, 0x6Fu, 0xEFu, 0x1Fu, 0x9Fu, 0x5Fu, 0xDFu, 0x3Fu, 0xBFu, 0x7Fu, 0xFFu
};

static constexpr std::array<std::uint16_t, 256> kMapCrcTable55DBAC { // 原常量参考：0x0055DBAC。
    0x0000u, 0x1021u, 0x2042u, 0x3063u, 0x4084u, 0x50A5u, 0x60C6u, 0x70E7u, 0x8108u, 0x9129u, 0xA14Au, 0xB16Bu, 0xC18Cu, 0xD1ADu, 0xE1CEu, 0xF1EFu,
    0x1231u, 0x0210u, 0x3273u, 0x2252u, 0x52B5u, 0x4294u, 0x72F7u, 0x62D6u, 0x9339u, 0x8318u, 0xB37Bu, 0xA35Au, 0xD3BDu, 0xC39Cu, 0xF3FFu, 0xE3DEu,
    0x2462u, 0x3443u, 0x0420u, 0x1401u, 0x64E6u, 0x74C7u, 0x44A4u, 0x5485u, 0xA56Au, 0xB54Bu, 0x8528u, 0x9509u, 0xE5EEu, 0xF5CFu, 0xC5ACu, 0xD58Du,
    0x3653u, 0x2672u, 0x1611u, 0x0630u, 0x76D7u, 0x66F6u, 0x5695u, 0x46B4u, 0xB75Bu, 0xA77Au, 0x9719u, 0x8738u, 0xF7DFu, 0xE7FEu, 0xD79Du, 0xC7BCu,
    0x48C4u, 0x58E5u, 0x6886u, 0x78A7u, 0x0840u, 0x1861u, 0x2802u, 0x3823u, 0xC9CCu, 0xD9EDu, 0xE98Eu, 0xF9AFu, 0x8948u, 0x9969u, 0xA90Au, 0xB92Bu,
    0x5AF5u, 0x4AD4u, 0x7AB7u, 0x6A96u, 0x1A71u, 0x0A50u, 0x3A33u, 0x2A12u, 0xDBFDu, 0xCBDCu, 0xFBBFu, 0xEB9Eu, 0x9B79u, 0x8B58u, 0xBB3Bu, 0xAB1Au,
    0x6CA6u, 0x7C87u, 0x4CE4u, 0x5CC5u, 0x2C22u, 0x3C03u, 0x0C60u, 0x1C41u, 0xEDAEu, 0xFD8Fu, 0xCDECu, 0xDDCDu, 0xAD2Au, 0xBD0Bu, 0x8D68u, 0x9D49u,
    0x7E97u, 0x6EB6u, 0x5ED5u, 0x4EF4u, 0x3E13u, 0x2E32u, 0x1E51u, 0x0E70u, 0xFF9Fu, 0xEFBEu, 0xDFDDu, 0xCFFCu, 0xBF1Bu, 0xAF3Au, 0x9F59u, 0x8F78u,
    0x9188u, 0x81A9u, 0xB1CAu, 0xA1EBu, 0xD10Cu, 0xC12Du, 0xF14Eu, 0xE16Fu, 0x1080u, 0x00A1u, 0x30C2u, 0x20E3u, 0x5004u, 0x4025u, 0x7046u, 0x6067u,
    0x83B9u, 0x9398u, 0xA3FBu, 0xB3DAu, 0xC33Du, 0xD31Cu, 0xE37Fu, 0xF35Eu, 0x02B1u, 0x1290u, 0x22F3u, 0x32D2u, 0x4235u, 0x5214u, 0x6277u, 0x7256u,
    0xB5EAu, 0xA5CBu, 0x95A8u, 0x8589u, 0xF56Eu, 0xE54Fu, 0xD52Cu, 0xC50Du, 0x34E2u, 0x24C3u, 0x14A0u, 0x0481u, 0x7466u, 0x6447u, 0x5424u, 0x4405u,
    0xA7DBu, 0xB7FAu, 0x8799u, 0x97B8u, 0xE75Fu, 0xF77Eu, 0xC71Du, 0xD73Cu, 0x26D3u, 0x36F2u, 0x0691u, 0x16B0u, 0x6657u, 0x7676u, 0x4615u, 0x5634u,
    0xD94Cu, 0xC96Du, 0xF90Eu, 0xE92Fu, 0x99C8u, 0x89E9u, 0xB98Au, 0xA9ABu, 0x5844u, 0x4865u, 0x7806u, 0x6827u, 0x18C0u, 0x08E1u, 0x3882u, 0x28A3u,
    0xCB7Du, 0xDB5Cu, 0xEB3Fu, 0xFB1Eu, 0x8BF9u, 0x9BD8u, 0xABBBu, 0xBB9Au, 0x4A75u, 0x5A54u, 0x6A37u, 0x7A16u, 0x0AF1u, 0x1AD0u, 0x2AB3u, 0x3A92u,
    0xFD2Eu, 0xED0Fu, 0xDD6Cu, 0xCD4Du, 0xBDAAu, 0xAD8Bu, 0x9DE8u, 0x8DC9u, 0x7C26u, 0x6C07u, 0x5C64u, 0x4C45u, 0x3CA2u, 0x2C83u, 0x1CE0u, 0x0CC1u,
    0xEF1Fu, 0xFF3Eu, 0xCF5Du, 0xDF7Cu, 0xAF9Bu, 0xBFBAu, 0x8FD9u, 0x9FF8u, 0x6E17u, 0x7E36u, 0x4E55u, 0x5E74u, 0x2E93u, 0x3EB2u, 0x0ED1u, 0x1EF0u
};

// 原 0x55BAE0 的可写八字节区，正文六字节、NUL 后保留零填充。
static constexpr wchar_t gText55BAE0[] = L"登出中";

// 原 0x55DF10：保留可写数据身份，不以运行期三角函数重算原表。
static std::array<std::uint64_t, 360> gAngleTableBits55DF10 {
#include "SceneAngleTable55DF10.inc"
};

// 原 .data [0x55EA50,0x55EC58)：65 项 F64 位型，按表项原序保存。
static std::array<std::uint64_t, 65> gAngleTableBits55EA50 { // 原全局：0x0055EA50。
    0x0000000000000000ULL, 0x3FECA543F1C75819ULL, 0x3FFCA379B77C02B0ULL, 0x4005785F06F69446ULL,
    0x400C9C55000C953AULL, 0x4011DE5EEDCC20D5ULL, 0x40156C5D63886595ULL, 0x4018F7B84DB9C737ULL,
    0x401C800431BDE82DULL, 0x4020026BD6E8AF81ULL, 0x4021C2E5BC87DB2BULL, 0x4023813DD97F62B7ULL,
    0x40253D436B8F9B13ULL, 0x4026F6C7863BEEC4ULL, 0x4028AD9CCFAEFF5CULL, 0x402A6197C3D68406ULL,
    0x402C128E71476AF9ULL, 0x402DC059641F6449ULL, 0x402F6AD2999567DCULL, 0x403088EB24A6A876ULL,
    0x40315AA161E4F766ULL, 0x40322A7C286F8AD2ULL, 0x4032F86CA89FC6DAULL, 0x4033C46530058149ULL,
    0x40348E58F7121AB5ULL, 0x4035563C64345CFFULL, 0x40361C04C8BC9CD4ULL, 0x4036DFA8826AA8EBULL,
    0x4037A11EEAA6D267ULL, 0x4038606045BAF534ULL, 0x40391D65D3996FA8ULL, 0x4039D829CFDD2285ULL,
    0x403A90A72EAD9275ULL, 0x403B46D9F0A1BE2BULL, 0x403BFABECEDD4845ULL, 0x403CAC535C9E6688ULL,
    0x403D5B95C4220360ULL, 0x403E088509BF9C63ULL, 0x403EB3210BE9424EULL, 0x403F5B69EC2CE465ULL,
    0x404000B063E07A29ULL, 0x404052839042D8C3ULL, 0x4040A32F87AD080BULL, 0x4040F2B599AA6091ULL,
    0x40414117481B21C4ULL, 0x40418E56605EE569ULL, 0x4041DA74D9C6B053ULL, 0x40422574E65BEA0CULL,
    0x40426F58D1526D8BULL, 0x4042B8232096787DULL, 0x4042FFD6733EBBFDULL, 0x40434675A31A4BDCULL,
    0x40438C03925BB7B7ULL, 0x4043D0833C60029FULL, 0x404413F7CED91687ULL, 0x404456646F156191ULL,
    0x404497CC63F14120ULL, 0x4044D83315D701DAULL, 0x4045179BD4067CF2ULL, 0x4045560A2877EE4EULL,
    0x4045938183F91E64ULL, 0x4045D005681ECD4BULL, 0x40460B996FA82E88ULL, 0x404646411C2A0232ULL,
    0x4046800000000000ULL,
};

static_assert(sizeof(gText9DAD30) == 26);
static_assert(kMapBitReverse55DAAC[1] == 0x80u);
static_assert(kMapCrcTable55DBAC[1] == 0x1021u);
static_assert(sizeof(gAngleTableBits55DF10) == 2880);
static_assert(sizeof(gAngleTableBits55EA50) == 520);
static_assert(sizeof(gDirectionDeltas55D9DC) == 32);

std::int32_t& gCurrentSceneType = platform::gCurrentSceneTypeStorage_FF0D40; // 原全局：0x00FF0D40。
std::int32_t gPendingSceneState = 0; // 原全局：0x00FF0D60。
std::int32_t gCurrentSceneState = 0; // 原全局：0x00FF0D64。
std::int32_t gPendingSceneType = 0; // 原全局：0x00FF0D6C。

std::uint8_t gValueC82204 = 0; // 原全局：0x00C82204。
std::uint32_t gValue5ABFC4 = 0; // 原全局：0x005ABFC4。
std::uint32_t gValue9A6E0C = 0; // 原全局：0x009A6E0C。
std::uint32_t gPendingMapBgmIndex_FF0D5C = 0; // 原全局：0x00FF0D5C。
std::uint32_t gMapBgmChangePending_FF0D58 = 0; // 原全局：0x00FF0D58。
std::uint32_t gValueD67E78 = 0; // 原全局：0x00D67E78。
std::uint32_t gValueFF0D74 = 0; // 原全局：0x00FF0D74。
std::uint8_t& gMapNameFirstByte_9DAD30 = gText9DAD30[0]; // 原全局：0x009DAD30。
std::array<std::uint32_t, 7> gValueD67B08 {}; // 原全局：0x00D67B08。
std::uint32_t gSceneTransitionResetPending_58B6D0 = 1; // 原全局：0x0058B6D0。
std::uint8_t gValueD9F2CA = 0; // 原全局：0x00D9F2CA。

const char* ReadText9DAD30() noexcept
{
    return reinterpret_cast<const char*>(gText9DAD30.data());
}

// 原函数：0x0051BEF0。
void ResumePlayerAfterObservation_51BEF0() noexcept
{
    gPlayerRuntimeSlots.flags18C &= 0xFF7FFFFFu;
}

std::uint32_t gWorldProjectionSourceA = 0; // 原全局：0x009EE908。
std::uint32_t gWorldProjectionSourceB = 0; // 原全局：0x009EE90C。
std::uint32_t gWorldProjectionSourceC = 0; // 原全局：0x009EE904。
std::uint32_t gWorldProjectionSourceD = 0; // 原全局：0x009EE8F4。
std::uint32_t gWorldProjectionSnapshotA = 0; // 原全局：0x00AF10C4。
std::uint32_t gWorldProjectionSnapshotB = 0; // 原全局：0x00AF10CC。
std::uint32_t gWorldProjectionSnapshotC = 0; // 原全局：0x00AF10D0。
std::uint32_t gWorldProjectionSnapshotD = 0; // 原全局：0x00AF10D4。

volatile std::int32_t gMapFloor = 0; // 原 0x9EE8F0。
volatile std::uint32_t gWorldCurrentXBits = 0; // 原 0x9DAD4C。
volatile std::uint32_t gWorldCurrentYBits = 0; // 原 0x9DAD50。
volatile std::uint32_t gWorldProjectionReferenceXBits = 0; // 原 0xC751A8。
volatile std::uint32_t gWorldProjectionReferenceYBits = 0; // 原 0xC751A4。

static_assert(sizeof(gValueD67B08) == 7 * sizeof(std::uint32_t));

// SLC01：原51C410只判断S32索引0..4；已证八调用只消费低W8结果。
bool IsValidPetIndex_51C410(const std::int32_t index) noexcept
{
    return index >= 0 && index < 5;
}

std::int32_t ReadRippleGateOnce()
{
    return static_cast<std::int32_t>(cg::world::gRippleGate);
}

// 原函数：0x00448240。
std::int32_t IsMapFloorAllowed_448240() noexcept
{
    const std::int32_t mapFloor = gMapFloor;
    if (mapFloor >= 40000 && mapFloor <= 41999) {
        return 1;
    }
    for (const std::int32_t allowedFloor : kAllowedMapFloors) {
        if (mapFloor == allowedFloor) {
            return 1;
        }
    }
    return 0;
}

// 原 sub_4493C0@[0x4493C0, 0x4493F2)；先判断闭区间，再按原表顺序匹配。
std::int32_t IsMapDriftEffectAllowed_4493C0() noexcept
{
    const std::int32_t mapFloor = gMapFloor;
    if (mapFloor >= 40000 && mapFloor <= 40999) {
        return 1;
    }
    for (const std::int32_t allowedFloor : kMapFloors5541F4) {
        if (mapFloor == allowedFloor) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x0046CE30。
float* ProjectWorldToScreen_46CE30(
    const float worldX,
    const float worldY,
    float* const outX,
    float* const outY) noexcept
{
    return platform::Cg2dProjectWorldToScreenX87(
        worldX, worldY, outX, outY,
        &gWorldProjectionReferenceXBits, &gWorldProjectionReferenceYBits);
}

// 原函数：0x0046CE80。
float* ProjectScreenToWorld_46CE80(
    const float screenX, const float screenY,
    float* const outX, float* const outY) noexcept
{
    return platform::Cg2dProjectScreenToWorldX87(
        screenX, screenY, outX, outY,
        &gWorldProjectionReferenceXBits, &gWorldProjectionReferenceYBits);
}

// 原函数：0x0051C770。
std::int32_t SetCurrentSceneType(std::int32_t sceneType)
{
    gCurrentSceneState = 0;
    gCurrentSceneType = sceneType;
    return sceneType;
}

// 原函数：0x0051C790。
std::int32_t SetCurrentSceneTypeAndState(std::int32_t sceneType, std::int32_t sceneState)
{
    gCurrentSceneType = sceneType;
    gCurrentSceneState = sceneState;
    return sceneType;
}

// 原函数：0x0051C7B0。
std::int32_t SetPendingSceneType(std::int32_t sceneType)
{
    gPendingSceneState = 0;
    gPendingSceneType = sceneType;
    return sceneType;
}

// 原函数：0x00468FF0。
std::uint32_t CopyWorldProjectionState_468FF0()
{
    const std::uint32_t a = gWorldProjectionSourceA;
    const std::uint32_t b = gWorldProjectionSourceB;
    const std::uint32_t c = gWorldProjectionSourceC;
    gWorldProjectionSnapshotA = a;
    const std::uint32_t d = gWorldProjectionSourceD;
    gWorldProjectionSnapshotB = b;
    gWorldProjectionSnapshotC = c;
    gWorldProjectionSnapshotD = d;
    return d;
}

// 原 [46FB20,46FB45)：仅消费一次共享 RAND，生产表时不重置消费游标。
std::int32_t InitializeRotationTable_46FB20()
{
    std::int32_t index = platform::Rand16() % 100;
    for (std::int32_t value = 0; value < 100; ++value) {
        gRotationTableC8A7B0[static_cast<std::size_t>(index)] = value;
        ++index;
        if (index >= 100) {
            index = 0;
        }
    }
    return 100;
}

// 原 455C70 跳入独占尾块 [46FB50,46FB74)：先清状态，再推进游标。
std::int32_t NextRotationValue_455C70()
{
    gValue62B348 = 0;
    detail::gValue62B308 = 0;
    const std::int32_t value =
        gRotationTableC8A7B0[static_cast<std::size_t>(gRotationCursorC8A940)];
    const std::int32_t next = gRotationCursorC8A940 + 1;
    gRotationCursorC8A940 = next;
    if (next >= 100) {
        gRotationCursorC8A940 = 0;
    }
    return value;
}

// 原函数：0x00518C20。
std::uint32_t InitializeAlbum_518C20()
{
    constexpr char kAlbumSignature[] = "CCA";
    std::memset(&gAlbumStorage, 0, sizeof(gAlbumStorage));
    std::memcpy(gAlbumStorage.signature.data(), kAlbumSignature, sizeof(kAlbumSignature));
    return 4;
}

std::array<BattleResultRecord, 4> gBattleResultsFF0C14 {}; // 原全局：0x00FF0C14。
std::array<BattleSkillResultRecord, kSkillSlotCount> gBattleSkillResultsFF0C8C {}; // 原全局：0x00FF0C8C。
std::array<std::array<char, 29>, 3> gBattleResultNamesFF0C34 {}; // 原全局：0x00FF0C34。
std::array<std::int32_t, 3> gBattleResultGraphicsFF0D0C {}; // 原全局：0x00FF0D0C。

namespace detail {
static_assert(sizeof(gBattleResultsFF0C14) == 0x20);
static_assert(sizeof(gBattleSkillResultsFF0C8C) == sizeof(BattleSkillResultRecord) * kSkillSlotCount); // 原15条共0x78字节。
static_assert(sizeof(gBattleResultNamesFF0C34) == 0x57);
static_assert(sizeof(gBattleResultGraphicsFF0D0C) == 0x0C);

} // namespace detail

 // 原全局：0x0073ECD0。

std::uint32_t& gMapEffectOffsetXBits = gMapCharaSceneSlots.valueC8A758; // 原全局：0x00C8A758。
std::uint32_t& gMapEffectOffsetYBits = gMapCharaSceneSlots.valueC8A75C; // 原全局：0x00C8A75C。
std::uint32_t& gBattleSceneEffectC8A768 = gMapCharaSceneSlots.valueC8A768; // 原全局：0x00C8A768。
std::uint32_t& gBattleSceneEffectC8A76C = gMapCharaSceneSlots.valueC8A76C; // 原全局：0x00C8A76C。

namespace detail {

// 原[518780,518865)：只读4字节；错误先关闭，长度不符调用同源保存。
void LoadAlbumData_518780()
{
    auto* const encoded = static_cast<std::uint8_t*>(
        std::malloc(0x78018));
    auto* const decoded = static_cast<std::uint8_t*>(
        std::malloc(0x78018));
    char path[128];
    std::sprintf(
        path, "%s\\%s", platform::gDataPath, "album.dat");
    std::FILE* const stream =
        std::fopen(path, "rb");
    if (stream == nullptr) {
        SaveAlbumData_5186A0();
    } else {
        const std::uint32_t count =
            static_cast<std::uint32_t>(std::fread(encoded, 1, 4, stream));
        // 原5187EA/5187F0读取flags低U8并测试20H错误位。
        if (std::ferror(stream) != 0) {
            static_cast<void>(std::fclose(stream));
            std::free(encoded);
            std::free(decoded);
            return;
        }
        static_cast<void>(std::fclose(stream));
        std::int32_t decodedBytes;
        static_cast<void>(login::detail::DecodeLegacyBytes_52D460(
            encoded, static_cast<std::int32_t>(count), 0, decoded, &decodedBytes));
        if (decodedBytes == 0x3C00C) {
            std::memcpy(&gAlbumStorage, decoded, sizeof(gAlbumStorage));
        } else {
            SaveAlbumData_5186A0();
        }
    }
    std::free(encoded);
    std::free(decoded);
}

// 原[518870,518A36)：相册记录更新，分行调用后重读角色而不固定首记录地址。
void UpdateAlbumEntry_518870(
    const std::int32_t number, const std::int32_t imageNo,
    const std::int32_t mark1, const std::int32_t mark2, const std::int32_t tribe,
    const std::int32_t value0C, const std::int32_t value10, const std::int32_t value14,
    const std::int32_t value18, const std::int32_t value1C, const std::int32_t value20,
    const std::int32_t value24, const std::int32_t value28, const std::int32_t value2C,
    const std::int32_t value30, char* const text, const char* const name)
{
    if (number < 1 || number > std::bit_cast<std::int32_t>(resource::gValueD2D5FC)) {
        return;
    }
    const std::int32_t index = number - 1;
    AlbumRecord* record = AlbumRecordAt(index, login::gSelectedCharacterIndex);
    const bool added = record->used == 0;
    record->used = 1;
    record->number = number;
    record->imageNo = imageNo;
    record->mark1 = static_cast<std::uint8_t>(mark1);
    record->mark2 = static_cast<std::uint8_t>(mark2);
    record->tribe = static_cast<std::uint8_t>(tribe);
    record->ratings = {value0C, value10, value14, value18, value1C};
    record->attributes = {value20, value24, value28, value2C};
    record->value30 = value30;
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::int16_t selected = login::gSelectedCharacterIndex;
    for (std::int32_t line = 0; line < 2; ++line) {
        char* const destination = AlbumRecordAt(index, selected)->description[line].data();
        const std::int32_t result = input::ExtractLegacyLine_503AB0(
            destination, 29, text, line, 28);
        selected = login::gSelectedCharacterIndex;
        if (result == 0) {
            AlbumRecordAt(index, selected)->description[line][0] = 0;
        }
    }
    const char* const displayedName = std::strlen(name) > 16 ? "???" : name;
    record = AlbumRecordAt(index, selected);
    std::strcpy(record->name.data(), displayedName);
    record->fresh = static_cast<std::uint8_t>(added);
    SaveAlbumData_5186A0();
}

// 原[518A40,518BD5)：掩码扫描后始终发送请求列表，再重读角色写版本。
void SynchronizeAlbumEntryMask_518A40(const std::int32_t version, const char* const payload)
{
    std::vector<std::int32_t> requests;
    std::int32_t limit = std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
    std::int32_t groups = limit >> 5;
    if ((static_cast<std::uint8_t>(limit) & 0x20u) != 0) {
        ++groups;
    }
    std::int32_t index = 0;
    std::int32_t field = 1;
    std::int32_t mask;
    for (std::int32_t group = 0; group < groups && index < limit; ++group) {
        const std::int32_t ordinal = field++;
        if (network::ParseDelimitedIntegerStatus_52CFF0(payload, '|', ordinal, &mask) > 1) {
            break;
        }
        limit = std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
        std::int16_t selected = login::gSelectedCharacterIndex;
        std::uint32_t bit = 1;
        for (std::int32_t position = 0; position < 32 && index < limit; ++position) {
            AlbumRecord* const record = AlbumRecordAt(index, selected);
            if ((static_cast<std::uint32_t>(mask) & bit) != 0) {
                bool request = record->used == 0;
                if (!request) {
                    const std::int32_t previousVersion = gAlbumStorage.versions[selected];
                    request = previousVersion < version;
                }
                if (request) {
                    requests.push_back(index + 1);
                    limit = std::bit_cast<std::int32_t>(resource::gValueD2D5FC);
                    selected = login::gSelectedCharacterIndex;
                }
            } else {
                record->used = 0;
            }
            ++index;
            bit <<= 1;
        }
    }
    const std::uintptr_t socket = platform::gConnectionSocket;
    static_cast<void>(network::RequestAlbumEntries_511030(
        socket, requests.empty() ? nullptr : requests.data(),
        static_cast<std::int32_t>(requests.size())));
    const std::int16_t selected = login::gSelectedCharacterIndex;
    gAlbumStorage.versions[selected] = version;
}

} // namespace detail

MapEffectRecord* gActiveMapEffectHead = nullptr; // 原 C8A72C。

namespace detail {

static std::array<MapEffectRecord, 500> gMapEffectRecords {};
static MapEffectRecord* gFreeMapEffectHead = nullptr;   // 原 C82244。
static std::uint8_t gMapEffectStartedC8A728 = 0; // 原全局：0x00C8A728。
static std::uint64_t gMapEffectTickC8A730 = 0; // 原全局：0x00C8A730。
static std::uint8_t gMapEffectStartedC82240 = 0; // 原全局：0x00C82240。
static std::uint64_t gMapEffectTickC82250 = 0; // 原全局：0x00C82250。
static std::uint8_t gMapEffectStartedC8224C = 0; // 原全局：0x00C8224C。
static std::uint64_t gMapEffectTickC8A734 = 0; // 原全局：0x00C8A734。
static std::int32_t gProjectedX55DEA4 = -1; // 原全局：0x0055DEA4。
static std::int32_t gProjectedY55DEA8 = -1; // 原全局：0x0055DEA8。

static void ClearMapEffectAction(cg::yobi::YobiAction* const action)
{
    if (action == nullptr) {
        return;
    }
    cg::yobi::MarkDeath(action);
    MapEffectRecord* record = gActiveMapEffectHead;
    gMapCharaSceneSlots.actionC8A764 = nullptr;
    while (record != nullptr) {
        if (record->kind == 8) {
            record->kind = 6;
            return;
        }
        record = record->next;
    }
}

// 原函数：0x0046E2A0。
MapEffectRecord* InitializeMapEffectPool_46E2A0()
{
    gFreeMapEffectHead = &gMapEffectRecords[0];
    gMapEffectRecords[0].prev = nullptr;
    for (std::size_t index = 0; index + 1 < gMapEffectRecords.size(); ++index) {
        MapEffectRecord* const current = &gMapEffectRecords[index];
        MapEffectRecord* const next = &gMapEffectRecords[index + 1];
        current->next = next;
        next->prev = current;
    }
    MapEffectRecord* const last = &gMapEffectRecords.back();
    last->next = nullptr;
    gActiveMapEffectHead = nullptr;

    gMapCharaSceneSlots.valueC8A73E = 0;
    gMapCharaSceneSlots.valueC8A742 = 0;
    const std::uint16_t clearedHalf = 0;
    std::memcpy(&gMapCharaSceneSlots.valueC8A744, &clearedHalf, sizeof(clearedHalf));
    gMapCharaSceneSlots.valueC8A748 = 0;
    gMapCharaSceneSlots.value55DE98 = 0xFFFFu;
    gMapCharaSceneSlots.valueC8A74A = 0;
    gMapCharaSceneSlots.valueC8A74C = 1;
    gMapCharaSceneSlots.valueC8A74E = 0;
    gMapCharaSceneSlots.valueC8A750 = 0;
    gMapCharaSceneSlots.valueC8A752 = 0;
    gMapCharaSceneSlots.valueC8A754 = 0;
    gMapCharaSceneSlots.valueC8A756 = 0;
    gMapCharaSceneSlots.valueC8A758 = 0;
    gMapCharaSceneSlots.valueC8A75C = 0;
    return last;
}

// 原函数：0x0046E340。
MapEffectRecord* AcquireMapEffectRecord_46E340()
{
    MapEffectRecord* const record = gFreeMapEffectHead;
    if (record == nullptr) {
        return record;
    }

    MapEffectRecord* const nextFree = record->next;
    gFreeMapEffectHead = nextFree;
    if (nextFree != nullptr) {
        nextFree->prev = nullptr;
    }
    MapEffectRecord* const activeHead = gActiveMapEffectHead;
    record->prev = nullptr;
    record->next = activeHead;
    MapEffectRecord* const currentActiveHead = gActiveMapEffectHead;
    if (currentActiveHead != nullptr) {
        currentActiveHead->prev = record;
    }
    gActiveMapEffectHead = record;
    return record;
}

// 原函数：0x0046E390。
MapEffectRecord* ReleaseMapEffectRecord_46E390(MapEffectRecord* const record)
{
    if (record == nullptr) {
        return record;
    }

    MapEffectRecord* const prev = record->prev;
    if (prev != nullptr) {
        prev->next = record->next;
    }
    MapEffectRecord* const next = record->next;
    if (next != nullptr) {
        next->prev = record->prev;
    }
    if (gActiveMapEffectHead == record) {
        gActiveMapEffectHead = record->next;
    }
    MapEffectRecord* const freeHead = gFreeMapEffectHead;
    record->prev = nullptr;
    record->next = freeHead;
    gFreeMapEffectHead = record;
    record->kind = 0;
    return record;
}

// 原函数：0x0046E3E0。
std::uintptr_t __cdecl OffsetMapEffectForPlayerDirection_46E3E0(std::int32_t* const x, std::int32_t* const y)
{
    const std::int16_t direction =
        std::bit_cast<std::int16_t>(gMapCharaSceneSlots.value55DE98);
    switch (direction) {
    case 0:
        ++*x;
        ++*y;
        return reinterpret_cast<std::uintptr_t>(y);
    case 1:
        *y += 2;
        return reinterpret_cast<std::uintptr_t>(y);
    case 2:
        --*x;
        ++*y;
        return reinterpret_cast<std::uintptr_t>(y);
    case 3:
        *x -= 2;
        return reinterpret_cast<std::uintptr_t>(x);
    case 4:
        --*x;
        --*y;
        return reinterpret_cast<std::uintptr_t>(y);
    case 5:
        *y -= 2;
        return reinterpret_cast<std::uintptr_t>(y);
    case 6:
        ++*x;
        --*y;
        return reinterpret_cast<std::uintptr_t>(y);
    case 7:
        *x += 2;
        return reinterpret_cast<std::uintptr_t>(x);
    default:
        return static_cast<std::uint32_t>(static_cast<std::int32_t>(direction));
    }
}

static void ApplyMapEffectDirection(MapEffectRecord& record)
{
    // 原46E3E0单读方向；按触达轴读/算/写，见CAC09。
    const std::int16_t direction =
        std::bit_cast<std::int16_t>(gMapCharaSceneSlots.value55DE98);
    switch (direction) {
    case 0:
        record.x += 1;
        record.y += 1;
        break;
    case 1:
        record.y += 2;
        break;
    case 2:
        record.x -= 1;
        record.y += 1;
        break;
    case 3:
        record.x -= 2;
        break;
    case 4:
        record.x -= 1;
        record.y -= 1;
        break;
    case 5:
        record.y -= 2;
        break;
    case 6:
        record.x += 1;
        record.y -= 1;
        break;
    case 7:
        record.x += 2;
        break;
    default:
        break;
    }
}

static MapEffectRecord* CreateMapEffectKind1AtZero()
{
    MapEffectRecord* const record = AcquireMapEffectRecord_46E340();
    if (record == nullptr) {
        return nullptr;
    }
    record->kind = 1;
    const std::int32_t randomX = platform::Rand16();
    record->y = 0;
    record->x = randomX % 840 - 200;
    const std::int32_t randomSpeed = platform::Rand16();
    record->value0C = 9;
    record->value08 = randomSpeed % 2 + 2;
    return record;
}

static void CreateMapEffectKind1()
{
    MapEffectRecord* const record = CreateMapEffectKind1AtZero();
    if (record == nullptr) {
        return;
    }
    const std::uint64_t tick = platform::ReadTickCount64();
    ++gMapCharaSceneSlots.valueC8A742;
    gMapEffectTickC8A734 = tick;
    record->y = static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(record->value0C));
}

// 46E6C0/46E940 共用初始载荷；tick、计数和 Y 尾写由各自调用者按原序处理。
static MapEffectRecord* CreateMapEffectKind2AtZero()
{
    MapEffectRecord* const record = AcquireMapEffectRecord_46E340();
    if (record == nullptr) {
        return nullptr;
    }
    record->kind = 2;
    const std::int32_t randomX = platform::Rand16();
    record->y = 0;
    record->x = randomX % 940 - 200;
    record->variant = static_cast<std::uint16_t>(platform::Rand16() % 2);
    const std::int32_t direction = platform::Rand16() % 2;
    if (direction == 0) {
        record->value08 = platform::Rand16() % 2;
        record->value0C = platform::Rand16() % 2 + 3;
    } else if (direction == 1) {
        record->value08 = 1;
        record->value0C = platform::Rand16() % 2 + 4;
    }
    return record;
}

static void CreateMapEffectKind2()
{
    MapEffectRecord* const record = CreateMapEffectKind2AtZero();
    if (record == nullptr) {
        return;
    }
    const std::uint64_t tick = platform::ReadTickCount64();
    ++gMapCharaSceneSlots.valueC8A748;
    gMapEffectTickC82250 = tick;
    record->y = static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(record->value0C));
}

static void CreateMapEffectKind4()
{
    MapEffectRecord* const record = AcquireMapEffectRecord_46E340();
    if (record == nullptr) {
        return;
    }
    record->kind = 4;
    const std::int32_t randomX = platform::Rand16();
    record->y = 0;
    record->x = randomX % 1240 - 100;
    record->variant = static_cast<std::uint16_t>(platform::Rand16() % 4);
    record->remaining = 4;
    const std::int32_t direction = platform::Rand16() % 2;
    if (direction == 0 || direction == 1) {
        record->value08 = static_cast<std::int32_t>(0xFFFFFFFFu);
        record->value0C = direction + 2;
    }
    const std::uint64_t tick = platform::ReadTickCount64();
    ++gMapCharaSceneSlots.valueC8A750;
    gMapEffectTickC8A730 = tick;
    record->y = static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(record->value0C));
}

// 原函数：0x0046E4E0。
void UpdateMapStreakParticles_46E4E0()
{
    const std::uint8_t started = gMapEffectStartedC8224C;
    if ((started & 1u) == 0) {
        gMapEffectStartedC8224C = static_cast<std::uint8_t>(started | 1u);
        gMapEffectTickC8A734 = platform::ReadTickCount64();
    }

    const std::uint32_t selected = gCurrentSceneType == 10
        ? gMapCharaSceneSlots.valueC8A768
        : gMapCharaSceneSlots.valueC8A73E;
    const std::int16_t selectedLow =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(selected));
    if (selectedLow != 0) {
        if (selectedLow >= 3) {
            const std::int32_t attempts = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(selected * 4u - 8u));
            for (std::int32_t index = 0; index < attempts; ++index) {
                CreateMapEffectKind1();
            }
        } else {
            const std::uint64_t now = platform::ReadTickCount64();
            const std::int16_t interval = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(51u - selected * 25u));
            const std::uint64_t deadline = gMapEffectTickC8A734
                + static_cast<std::uint64_t>(static_cast<std::int64_t>(interval));
            if (deadline < now) {
                CreateMapEffectKind1();
            }
        }
    }

    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        if (record->kind == 1) {
            const std::int32_t velocityX = record->value08;
            const std::int32_t oldX = record->x;
            const std::int32_t velocityY = record->value0C;
            const std::int32_t oldY = record->y;
            const std::int32_t x = oldX + velocityX;
            const std::int32_t y = oldY + velocityY;
            record->x = x;
            record->y = y;
            ApplyMapEffectDirection(*record);
            const std::int32_t finalX = record->x;
            if (finalX >= platform::gWindowWidth
                || record->y >= platform::gWindowHeight) {
                MapEffectRecord* const next = record->next;
                ReleaseMapEffectRecord_46E390(record);
                --gMapCharaSceneSlots.valueC8A742;
                record = next;
                continue;
            }
        }
        record = record->next;
        gMapCharaSceneSlots.valueC8A73C = 1;
    }
}

// 原函数：0x0046E6C0。
void UpdateMapClusterParticles_46E6C0()
{
    const std::uint8_t started = gMapEffectStartedC82240;
    if ((started & 1u) == 0) {
        gMapEffectStartedC82240 = static_cast<std::uint8_t>(started | 1u);
        gMapEffectTickC82250 = platform::ReadTickCount64();
    }

    const std::uint16_t selected = static_cast<std::uint16_t>(
        gCurrentSceneType == 10
            ? gMapCharaSceneSlots.valueC8A768 >> 16
            : gMapCharaSceneSlots.valueC8A744);
    const std::int16_t selectedLow = std::bit_cast<std::int16_t>(selected);
    if (selectedLow != 0) {
        if (selectedLow >= 3) {
            const std::int32_t attempts = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(2u * selected - 4u));
            for (std::int32_t index = 0; index < attempts; ++index) {
                CreateMapEffectKind2();
            }
        } else {
            const std::uint64_t now = platform::ReadTickCount64();
            const std::int16_t interval = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(51u - 25u * selected));
            const std::uint64_t deadline = gMapEffectTickC82250
                + static_cast<std::uint64_t>(static_cast<std::int64_t>(interval));
            if (deadline < now) {
                CreateMapEffectKind2();
            }
        }
    }

    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        if (record->kind == 2) {
            const std::int32_t velocityX = record->value08;
            const std::int32_t oldX = record->x;
            const std::int32_t oldY = record->y;
            record->x = oldX + velocityX;
            const std::int32_t velocityY = record->value0C;
            record->y = oldY + velocityY;
            ApplyMapEffectDirection(*record);
            const std::int32_t finalX = record->x;
            if (finalX >= 740
                || record->y >= 480) {
                MapEffectRecord* const next = record->next;
                ReleaseMapEffectRecord_46E390(record);
                --gMapCharaSceneSlots.valueC8A748;
                record = next;
                continue;
            }
        }
        record = record->next;
        gMapCharaSceneSlots.valueC8A73C = 1;
    }
}

// 原函数：0x0046E940。
std::int32_t __cdecl AdvanceMapClusterParticlesCycles_46E940(std::int32_t cycles)
{
    const std::uint16_t selected = static_cast<std::uint16_t>(
        gCurrentSceneType == 10
            ? gMapCharaSceneSlots.valueC8A768 >> 16
            : gMapCharaSceneSlots.valueC8A744);
    if (cycles <= 0) {
        return cycles;
    }

    const std::int16_t selectedLow = std::bit_cast<std::int16_t>(selected);
    std::int32_t streak = 0;
    do {
        if (selectedLow != 0) {
            if (selectedLow >= 3) {
                const std::int32_t attempts = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(2u * selected - 4u));
                for (std::int32_t index = 0; index < attempts; ++index) {
                    if (CreateMapEffectKind2AtZero() != nullptr) {
                        streak = 0;
                        ++gMapCharaSceneSlots.valueC8A748;
                    }
                }
            } else {
                const std::int32_t threshold = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(2u * (2u - selected)));
                if (streak >= threshold
                    && CreateMapEffectKind2AtZero() != nullptr) {
                    streak = 0;
                    ++gMapCharaSceneSlots.valueC8A748;
                }
            }
        }

        MapEffectRecord* record = gActiveMapEffectHead;
        while (record != nullptr) {
            if (record->kind == 2) {
                const std::int32_t velocityX = record->value08;
                const std::int32_t oldX = record->x;
                const std::int32_t velocityY = record->value0C;
                const std::int32_t oldY = record->y;
                record->x = oldX + velocityX;
                record->y = oldY + velocityY;
                ApplyMapEffectDirection(*record);
                const std::int32_t finalX = record->x;
                if (finalX >= 740
                    || record->y >= 480) {
                    MapEffectRecord* const next = record->next;
                    ReleaseMapEffectRecord_46E390(record);
                    --gMapCharaSceneSlots.valueC8A748;
                    record = next;
                    continue;
                }
            }
            record = record->next;
            gMapCharaSceneSlots.valueC8A73C = 1;
        }
        ++streak;
    } while (--cycles != 0);
    return cycles;
}

// 原函数：0x0046EBB0。
std::int32_t __cdecl AdvanceMapStreakParticlesCycles_46EBB0(std::int32_t cycles)
{
    const std::uint16_t selected = static_cast<std::uint16_t>(
        gCurrentSceneType == 10
            ? gMapCharaSceneSlots.valueC8A768
            : gMapCharaSceneSlots.valueC8A73E);
    if (cycles <= 0) {
        return cycles;
    }

    const std::int16_t selectedLow = std::bit_cast<std::int16_t>(selected);
    do {
        if (selectedLow != 0) {
            if (selectedLow >= 3) {
                const std::int32_t attempts = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(4u * selected - 8u));
                for (std::int32_t index = 0; index < attempts; ++index) {
                    if (CreateMapEffectKind1AtZero() != nullptr) {
                        ++gMapCharaSceneSlots.valueC8A742;
                    }
                }
            } else {
                const std::int16_t threshold = std::bit_cast<std::int16_t>(
                    static_cast<std::uint16_t>(2u * (2u - selected)));
                if (threshold <= 0
                    && CreateMapEffectKind1AtZero() != nullptr) {
                    ++gMapCharaSceneSlots.valueC8A742;
                }
            }
        }

        MapEffectRecord* record = gActiveMapEffectHead;
        while (record != nullptr) {
            if (record->kind == 1) {
                const std::int32_t velocityX = record->value08;
                const std::int32_t oldX = record->x;
                const std::int32_t velocityY = record->value0C;
                const std::int32_t oldY = record->y;
                record->x = oldX + velocityX;
                record->y = oldY + velocityY;
                ApplyMapEffectDirection(*record);
                const std::int32_t finalX = record->x;
                if (finalX >= 640
                    || record->y >= 480) {
                    MapEffectRecord* const next = record->next;
                    ReleaseMapEffectRecord_46E390(record);
                    --gMapCharaSceneSlots.valueC8A742;
                    record = next;
                    continue;
                }
            }
            record = record->next;
            gMapCharaSceneSlots.valueC8A73C = 1;
        }
    } while (--cycles != 0);
    return cycles;
}

// 原函数：0x0046E4C0。
std::int32_t __cdecl AdvanceMapParticleCycles_46E4C0(const std::int32_t cycles)
{
    static_cast<void>(AdvanceMapStreakParticlesCycles_46EBB0(cycles));
    return AdvanceMapClusterParticlesCycles_46E940(cycles);
}

// 原函数：0x0046ED50。
void UpdateMapFlutterParticles_46ED50()
{
    const std::uint8_t started = gMapEffectStartedC8A728;
    if ((started & 1u) == 0) {
        gMapEffectStartedC8A728 = static_cast<std::uint8_t>(started | 1u);
        gMapEffectTickC8A730 = platform::ReadTickCount64();
    }

    const std::uint32_t selected = gCurrentSceneType == 10
        ? gMapCharaSceneSlots.valueC8A76C
        : gMapCharaSceneSlots.valueC8A74E;
    const std::int16_t selectedLow =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(selected));
    if (selectedLow != 0) {
        if (selectedLow >= 3) {
            const std::int32_t attempts = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(2u * selected - 4u));
            for (std::int32_t index = 0; index < attempts; ++index) {
                CreateMapEffectKind4();
            }
        } else {
            const std::uint64_t now = platform::ReadTickCount64();
            const std::int16_t interval = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(51u - 25u * selected));
            const std::uint64_t deadline = gMapEffectTickC8A730
                + static_cast<std::uint64_t>(static_cast<std::int64_t>(interval));
            if (deadline < now) {
                CreateMapEffectKind4();
            }
        }
    }

    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        if (record->kind == 4) {
            const std::int32_t randomX = platform::Rand16() % 3;
            const std::int32_t oldX = record->x;
            const std::int32_t velocityX = record->value08;
            record->x = oldX + velocityX + (randomX - 1);

            const std::int32_t randomY = platform::Rand16() % 3;
            const std::int32_t velocityY = record->value0C;
            const std::int32_t oldY = record->y;
            const std::uint32_t remaining = record->remaining;
            record->y = oldY + velocityY + (randomY - 1);
            if (std::bit_cast<std::int32_t>(remaining) <= 0) {
                record->phase = static_cast<std::uint16_t>(platform::Rand16() % 3);
                record->value28 = platform::Rand16() % 8 + 1;
                const std::uint32_t nextVelocityY =
                    static_cast<std::uint32_t>(platform::Rand16() % 8 + 1);
                record->remaining = 4;
                record->value2C = static_cast<std::int32_t>(nextVelocityY);
            } else {
                record->remaining = remaining - 1u;
            }
            ApplyMapEffectDirection(*record);
            if (record->y >= 480) {
                MapEffectRecord* const next = record->next;
                ReleaseMapEffectRecord_46E390(record);
                --gMapCharaSceneSlots.valueC8A750;
                record = next;
                continue;
            }
        }
        record = record->next;
        gMapCharaSceneSlots.valueC8A73C = 1;
    }
}

// 原函数：0x0046E4A0。
void UpdateMapEffects_46E4A0()
{
    UpdateMapStreakParticles_46E4E0();
    UpdateMapClusterParticles_46E6C0();
    // 原 46E4AA/46E4AF 两次调用的 509080 仅有 retn。
    UpdateMapFlutterParticles_46ED50();
    UpdateDirectionalMapEffects_46F1D0();
}

// 原函数：0x0046E0D0。
void __cdecl ConfigureDirectionalMapEffects_46E0D0(const std::int32_t mask, const std::int32_t value)
{
    if (mask == 0) {
        cg::yobi::YobiAction* const action = gMapCharaSceneSlots.actionC8A764;
        gMapCharaSceneSlots.valueC8A752 = 0;
        gMapCharaSceneSlots.valueC8A754 = 0;
        gMapCharaSceneSlots.value55DEA2 = -1;
        ClearMapEffectAction(action);
        return;
    }

    if ((mask & 8) != 0) {
        gMapCharaSceneSlots.valueC8A752 =
            static_cast<std::uint16_t>(value % 10);
        gMapCharaSceneSlots.value55DEA0 =
            static_cast<std::int16_t>((value % 10000) / 10);
        gMapCharaSceneSlots.value55DE9C =
            static_cast<float>(value / 10000);
    }
    if ((mask & 0x10) != 0) {
        gMapCharaSceneSlots.valueC8A754 =
            static_cast<std::uint16_t>(value % 10);
        gMapCharaSceneSlots.value55DEA0 =
            static_cast<std::int16_t>((value % 10000) / 10);
        gMapCharaSceneSlots.value55DE9C =
            static_cast<float>(value / 10000);
    }
    if ((mask & 0x20) == 0) {
        return;
    }
    ClearMapEffectAction(gMapCharaSceneSlots.actionC8A764);
    if (value == -1) {
        gMapCharaSceneSlots.value55DEA2 = -1;
        return;
    }
    gMapCharaSceneSlots.value55DEA2 = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(60 * (value / 100000)));
    gMapCharaSceneSlots.valueC8A762 =
        static_cast<std::int16_t>((value / 10000 % 10) % 8);
    gMapCharaSceneSlots.valueC8A760 =
        static_cast<std::int16_t>(value % 10000);
}

// 原函数：0x0046F160。
void ClearDirectionalMapEffects_46F160()
{
    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        const std::uint16_t kind = record->kind;
        if (kind != 6 && kind != 7 && kind != 8) {
            record = record->next;
            continue;
        }
        if (kind == 8) {
            cg::yobi::MarkDeath(gMapCharaSceneSlots.actionC8A764);
            gMapCharaSceneSlots.actionC8A764 = nullptr;
        }
        MapEffectRecord* const next = record->next;
        ReleaseMapEffectRecord_46E390(record);
        --gMapCharaSceneSlots.valueC8A756;
        record = next;
    }
}

// 原函数：0x0046F090。
MapEffectRecord* CreateDirectionalMapEffect_46F090(
    const std::int16_t kind, const std::int32_t horizontalSign,
    const std::int32_t verticalSign, const std::int32_t phase,
    const std::int32_t speed, const std::int32_t graphicIndex)
{
    MapEffectRecord* const record = AcquireMapEffectRecord_46E340();
    if (record == nullptr) {
        return record;
    }

    record->kind = static_cast<std::uint16_t>(kind);
    const std::int32_t offset = verticalSign != 0
        ? phase % (horizontalSign != 0 ? 1920 : 1040)
        : phase % 880 + 1040;

    std::int32_t x = -400;
    std::int32_t y = -400;
    record->x = x;
    record->y = y;
    if (offset >= 1040) {
        y = offset - 1440;
        record->y = y;
    } else {
        x = offset - 400;
        record->x = x;
    }
    if (horizontalSign < 0) {
        x = 240 - record->x;
        record->x = x;
    }
    if (verticalSign < 0) {
        y = 80 - record->y;
        record->y = y;
    }
    const std::int32_t currentX = record->x;
    const std::int32_t currentY = record->y;
    const std::int32_t fixedX = currentX * 65536;
    const std::int32_t fixedY = currentY * 65536;
    record->value08 = fixedX;
    record->value0C = fixedY;
    record->speed = speed;
    record->graphicIndex = graphicIndex;
    ++gMapCharaSceneSlots.valueC8A756;
    return record;
}

// 原函数：0x0046F000。
std::int32_t __cdecl ApplyMapCameraDeltaToEffectOffset_46F000(std::int32_t* const x, std::int32_t* const y)
{
    float projectedX;
    float projectedY;
    ProjectWorldToScreen_46CE30(0.0F, 0.0F, &projectedX, &projectedY);
    const std::int32_t currentX = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(static_cast<std::int64_t>(
            projectedX + 0.5)));
    const std::int32_t currentY = std::bit_cast<std::int32_t>(
        static_cast<std::uint32_t>(static_cast<std::int64_t>(
            projectedY + 0.5)));

    if (gProjectedX55DEA4 == -1) {
        gProjectedX55DEA4 = currentX;
        gProjectedY55DEA8 = currentY;
        return currentY;
    }

    const std::int32_t deltaX = currentX - gProjectedX55DEA4;
    *x += deltaX;
    const std::int32_t deltaY = currentY - gProjectedY55DEA8;
    *y += deltaY;
    gProjectedX55DEA4 = currentX;
    gProjectedY55DEA8 = currentY;
    return currentY;
}

// 原函数：0x0046F1D0。
void UpdateDirectionalMapEffects_46F1D0()
{
    if (gCurrentSceneType == 10) {
        return;
    }

    double direction[2];
    platform::detail::Cg2dMapEffectDirection(
        gMapCharaSceneSlots.value55DEA0,
        &gMapCharaSceneSlots.value55DE9C,
        &gMapCharaSceneSlots.valueC8A758,
        &gMapCharaSceneSlots.valueC8A75C, direction);
    std::int32_t cameraX = 0;
    std::int32_t cameraY = 0;
    ApplyMapCameraDeltaToEffectOffset_46F000(&cameraX, &cameraY);
    platform::detail::MapEffectMotion motion;
    platform::detail::Cg2dMapEffectMotion(
        direction, cameraX, cameraY, &motion);

    if (gMapCharaSceneSlots.value55DEA2 == 0) {
        const std::int32_t phase = gMapCharaSceneSlots.valueC8A760;
        CreateDirectionalMapEffect_46F090(8, motion.horizontalSign, motion.verticalSign,
            phase, 2000, phase % 3);
        gMapCharaSceneSlots.actionC8A764 =
            cg::yobi::NewYobiAction_404AE0(3, 0);
        if (gMapCharaSceneSlots.actionC8A764 != nullptr) {
            cg::yobi::SetYobiActionField2A8(
                gMapCharaSceneSlots.actionC8A764, 110308);
            cg::yobi::SetYobiActionField2B0(
                gMapCharaSceneSlots.actionC8A764, 3);
            cg::yobi::SetYobiActionField2B8(
                gMapCharaSceneSlots.actionC8A764,
                gMapCharaSceneSlots.valueC8A762 % 8);
            cg::yobi::ClearYobiActionCallback(
                gMapCharaSceneSlots.actionC8A764);
            cg::yobi::SetYobiActionField15(
                gMapCharaSceneSlots.actionC8A764, 26);
        }
    }
    if (gMapCharaSceneSlots.value55DEA2 >= 0) {
        --gMapCharaSceneSlots.value55DEA2;
    }

    const auto spawn = [&motion](
        const std::int16_t kind, const std::uint16_t level,
        const std::int32_t minimumSpeed) {
        if (level == 0) {
            return;
        }
        const float speed = gMapCharaSceneSlots.value55DE9C;
        if (!(speed > 0.0F || speed < 0.0F)) {
            return;
        }
        std::int32_t divisor = platform::detail::Cg2dMapEffectDivisor(
            static_cast<std::int32_t>(std::bit_cast<std::int16_t>(level)) - 1,
            &motion.magnitude);
        if (divisor == 0) {
            divisor = 1;
        }
        if (platform::Rand16() % divisor != 0) {
            return;
        }
        const std::int32_t graphicIndex = platform::Rand16() % 3;
        const std::int32_t recordSpeed =
            platform::Rand16() % 1000 + minimumSpeed;
        const std::int32_t phase = platform::Rand16();
        CreateDirectionalMapEffect_46F090(kind, motion.horizontalSign, motion.verticalSign,
            phase, recordSpeed, graphicIndex);
    };
    spawn(6, gMapCharaSceneSlots.valueC8A752, 1000);
    spawn(7, gMapCharaSceneSlots.valueC8A754, 500);

    MapEffectRecord* record = gActiveMapEffectHead;
    while (record != nullptr) {
        const std::uint16_t kind = record->kind;
        if (kind == 6 || kind == 7 || kind == 8) {
            std::int64_t delta[2];
            platform::detail::Cg2dMapEffectStep(
                record->speed,
                direction, delta);
            std::int32_t fixedX =
                record->value08
                + static_cast<std::int32_t>(delta[0]);
            record->value08 = fixedX;
            std::int32_t fixedY =
                record->value0C
                + static_cast<std::int32_t>(delta[1]);
            record->value0C = fixedY;
            fixedX += cameraX * 65536;
            record->value08 = fixedX;
            fixedY += cameraY * 65536;
            record->value0C = fixedY;
            const std::int32_t x = fixedX >> 16;
            const std::int32_t y = fixedY >> 16;
            record->x = x;
            record->y = y;
            if (x < -400 || x > 640 || y < -400 || y > 480) {
                if (record->kind == 8) {
                    cg::yobi::MarkDeath(gMapCharaSceneSlots.actionC8A764);
                    gMapCharaSceneSlots.actionC8A764 = nullptr;
                }
                MapEffectRecord* const next = record->next;
                ReleaseMapEffectRecord_46E390(record);
                --gMapCharaSceneSlots.valueC8A756;
                record = next;
                continue;
            }

            const std::uint16_t drawKind = record->kind;
            if (drawKind == 8 && gMapCharaSceneSlots.actionC8A764 != nullptr) {
                cg::yobi::SetYobiActionField30(
                    gMapCharaSceneSlots.actionC8A764, x + 150);
                const auto actionY = record->y;
                cg::yobi::SetYobiActionField34(
                    gMapCharaSceneSlots.actionC8A764, actionY + 150);
                cg::yobi::AdvanceYobiAnimationFrame_517880(
                    gMapCharaSceneSlots.actionC8A764, 100, 0, 0);
                cg::yobi::YobiAction* const action = gMapCharaSceneSlots.actionC8A764;
                const std::int32_t imageNo = cg::yobi::GetYobiActionField20(action);
                const std::int32_t drawY = cg::yobi::GetYobiActionField34(action);
                const std::int32_t drawX = cg::yobi::GetYobiActionField30(action);
                cg::render::DrawGraphic(drawX, drawY, 0, imageNo, 0, nullptr);
            }
            if (drawKind == 7) {
                const auto index = record->graphicIndex;
                render::DrawGraphic3_520970(
                    x, y, 1, 14648, 0, 1, 0,
                    gEffectDrawModes55DEB0.data() + 3 + index);
            } else if (drawKind == 6 || drawKind == 8) {
                const auto index = record->graphicIndex;
                const auto speed = record->speed;
                const auto drawX = record->x;
                const auto drawY = record->y;
                render::DrawGraphic3_520970(
                    drawX, drawY, static_cast<std::uint8_t>(speed / 200 + 15),
                    14648, 0, 1, 0, gEffectDrawModes55DEB0.data() + index);
            }
        }
        record = record->next;
        gMapCharaSceneSlots.valueC8A73C = 1;
    }
}

// 原函数：0x00440FF0。
std::int32_t DrawObservationTravelBackground_440FF0()
{
    const std::int32_t direction = cg::yobi::ReadMapObservationDirection();
    constexpr std::array<std::int32_t, 8> angles {315, 270, 225, 180, 135, 90, 45, 0};
    const auto angle = static_cast<std::int16_t>(angles[direction]);
    const render::GraphicDescriptor16 descriptor {
        -320, 340, 64, 48, 0xFFFFFFFFu, 1, cg::render::GraphicMixFlag_None, 0,
    };
    gMapCharaSceneSlots.valueC8A752 = 2;
    gMapCharaSceneSlots.valueC8A754 = 2;
    gMapCharaSceneSlots.value55DE9C = 6.0F;
    gMapCharaSceneSlots.value55DEA0 = angle;
    UpdateDirectionalMapEffects_46F1D0();

    std::int32_t result = 0;
    for (std::int32_t y = 0; y < 480; y += 48) {
        for (std::int32_t x = 0; x < 640; x += 64) {
            result = render::DrawGraphic3_520970(
                x, y, 0, 14648, 0, 1, 0, &descriptor);
        }
    }
    return result;
}

// 原六个入口共享473060发送尾；选项修改后重新读取，不按发送结果回滚。
static std::int32_t TogglePlayerOptionAndSend(const std::uint16_t mask)
{
    // 六个原入口先读低W8判位；写回仍操作同一options194低W16。
    const std::uint8_t options = *reinterpret_cast<const unsigned char*>(&gPlayerRuntimeSlots.options194);
    if ((options & mask) != 0) {
        gPlayerRuntimeSlots.options194 &= static_cast<std::uint16_t>(~mask);
    } else {
        gPlayerRuntimeSlots.options194 |= mask;
    }
    // 原473060取样后先读socket再保留低W16；原高W16被丢弃，不借新端下一字段冒充。
    const std::uint32_t sampledOptions = gPlayerRuntimeSlots.options194;
    const std::uintptr_t socket = platform::gConnectionSocket;
    const std::int32_t value = static_cast<std::uint16_t>(sampledOptions);
    return network::SendPlayerOptions_511A00(socket, value);
}

void MarkPlayerChatActive() noexcept
{
    auto* const flagBytes = reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18A);
    flagBytes[1] |= 0x20u;
}

} // namespace detail

// 技能/索引表借用唯一第四清零块；原零初值及拖放门禁不新增构造/RNG消费。
std::array<SkillRuntimeRecord, kSkillSlotCount>& gSkillsF1BAFC = gSkillRuntimeSlots.skills; // 原全局：0x00F1BAFC。
std::array<SkillPairRecord, kSkillSlotCount>& gSkillPairsF63370 = gSkillRuntimeSlots.pairs; // 原全局：0x00F63370。
std::uint16_t gValueDA3B20 = 0; // 原全局：0x00DA3B20。

namespace detail {

// 原函数：0x00504F10。
std::int32_t HasSkillRecipeForItem_504F10(const std::int32_t value, const std::int32_t mode)
{
    // 原额外F633A0读取别名为pairs[6]；按用户确认，仅搜索实际技能槽。
    for (const SkillRuntimeRecord& record : gSkillsF1BAFC) {
        const std::int32_t skillId = record.skillId;
        const bool matches = mode == 1
            ? skillId >= 601 && skillId <= 605
            : (skillId >= 200 && skillId <= 214) || skillId == 2004 || skillId == 3103;
        if (!matches) {
            continue;
        }
        for (const SkillRecipeRecord& recipe : record.recipes8E8) {
            if (recipe.value10 == value) {
                return 1;
            }
        }
    }
    return 0;
}

// 原函数：0x00519950。
void ResetSkillRecipeSlots_519950(const std::int32_t index)
{
    SkillRuntimeRecord& record = gSkillsF1BAFC[index];
    record.rebirthState4C48 = 0;
    for (SkillRecipeRecord& recipe : record.recipes8E8) {
        recipe.itemId = -1;
        for (SkillRecipeItem& item : recipe.items6C) {
            item.itemId = -1;
        }
    }
}

// 原函数：0x00518660。
std::int32_t CompareSkillDisplayEntries_518660(
    const SkillPairRecord* const left, const SkillPairRecord* const right)
{
    const bool leftUsed = left->used != 0;
    const bool rightUsed = right->used != 0;
    if (leftUsed != rightUsed) {
        return leftUsed ? -1 : 1;
    }
    if (!leftUsed) {
        return 0;
    }
    return (left->sequence > right->sequence) - (left->sequence < right->sequence);
}

// 原函数：0x00518600。
void BuildSortedSkillDisplayEntries_518600(
    const SkillRuntimeRecord* source, SkillPairRecord* const destination,
    const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        destination[index].used = source->name[0] != '\0' ? 1 : 0;
        destination[index].index = static_cast<std::uint16_t>(index);
        destination[index].sequence = source->sequence;
        ++source;
    }

    const auto sortCount = static_cast<std::uint32_t>(count);
    if (sortCount < 2) {
        return;
    }
    std::sort(destination, destination + sortCount,
        [](const SkillPairRecord& left, const SkillPairRecord& right) {
            return CompareSkillDisplayEntries_518660(&left, &right) < 0;
        });
}

 // 原全局：0x00FDA8A4。

// 两物品回复仅非空名称的字段尾相同，外层停止、清理及UI同步各自保留。
static void ApplyPlayerItemFields(
    const std::int32_t index, const std::int32_t nameField,
    const char* const payload, const char* const name,
    char* const memo, char* const description)
{
    PlayerItemRecord& record = gPlayerRuntimeSlots.items[index];
    record.used = 1;
    if (std::strlen(name) <= 28) {
        record.name = cg::text::FromGbkWide(name);
    }
    const std::int32_t color =
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 1);
    record.color = color;
    if (color < 0) {
        record.color = 0;
    }
    network::ExtractDelimitedField_52CED0(payload, '|', nameField + 2, 767, description);
    input::UnescapeLegacyTextAndNotify_52D2F0(description);
    SetPlayerItemDescription_5196B0(index, description);
    record.hasDescription = !record.description[0].empty() ? 1 : 0;
    record.imageNo = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 3);
    record.canUseInField = static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 4));
    record.battleUseFlag = static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 5));
    record.target = static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 6));
    record.level = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 7);
    record.flags = static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 8));
    record.itemId = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 9);
    record.itemType = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 10);
    record.count = network::ParseDelimitedInteger_52CFA0(payload, '|', nameField + 11);
    network::ExtractDelimitedField_52CED0(payload, '|', nameField + 12, 255, memo);
    input::UnescapeLegacyTextAndNotify_52D2F0(memo);
    if (std::strlen(memo) <= 16) {
        record.memo = cg::text::FromGbkWide(memo);
    }
}

// 原函数：0x005196B0。
void SetPlayerItemDescription_5196B0(const std::int32_t index, const char* const text)
{
    PlayerItemRecord& record = gPlayerRuntimeSlots.items[index];
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char row[96]; // 原每行最多32B；旧分行仍在GBK边界执行。
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            record.description[line] = cg::text::FromGbkWide(row);
            last = line;
        } else {
            record.description[line].clear();
        }
    }
    while (last > 0 && record.description[last - 1].empty()) {
        --last;
    }
    if (last < 0) {
        record.pages = 0;
    } else {
        const std::int32_t pages = last / 8 + 1;
        record.pages = pages;
        if (pages > 4) {
            record.pages = 4;
        }
    }
}

// 原函数：0x0051B530。
void SwapPlayerInventorySlots_51B530(const std::int32_t first, const std::int32_t second)
{
    if (first < 0 || second < 0) {
        return;
    }
    PlayerItemRecord* const left = &gPlayerRuntimeSlots.items[first];
    PlayerItemRecord* const right = &gPlayerRuntimeSlots.items[second];
    const PlayerItemRecord temporary = *left;
    *left = *right;
    *right = temporary;
    if (first < 8) {
        left->flags |= 0x80u;
    }
    if (second < 8) {
        right->flags |= 0x80u;
    }
    ui::detail::WindowItemDrag_RemapHover(first, second);
}

} // namespace detail

std::array<PetPairRecord, 5> gPetPairsDE0B08 {}; // 原全局：0x00DE0B08。

namespace detail {
std::int32_t gPetCountDE0B00 = 0; // 原全局：0x00DE0B00。

} // namespace detail
std::uint32_t gPetDetailDraftChanged_D67B28 = 0; // 原全局：0x00D67B28。

// 原F7C950同属PE虚拟零区，独立于宠物表与F7C9FC之后的状态。
ReplyRecordF7C950 gReplyRecordF7C950 {}; // 原全局：0x00F7C950。

namespace detail {

// 原函数：0x00519770。
void SetPetItemDescription_519770(
    const std::int32_t petIndex, const std::int32_t itemIndex, const char* const text)
{
    PlayerItemRecord& record = gPetRuntimeRecords[petIndex].items6E4[itemIndex];
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char row[96]; // 原每行最多32B；旧分行仍在GBK边界执行。
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            record.description[line] = cg::text::FromGbkWide(row);
            last = line;
        } else {
            record.description[line].clear();
        }
    }
    while (last > 0 && record.description[last - 1].empty()) {
        --last;
    }
    record.pages = last < 0 ? 0 : last / 8 + 1;
}

// 原函数：0x0043C4E0。
void BeginBattleCommandSelection_43C4E0()
{
    gBattleCountdownActive5ABCB8 = 1;
    const std::uint64_t tick = platform::ReadTickCount64();
    gBattleCommandDeadline5ABBF8 = tick + gBattleCommandWaitMs;
    gBattleCountdownSecond5ABB80 = 0;
    gBattleCommandState5ABC3C = 0;
    gBattleSkillActionCount5ABBF4 = 0;
    gBattlePetActionCount5ABBBC = 0;
    cg::yobi::DisableBattleActorIdleAnimation_43C040();
    cg::yobi::EnableCompletedBattleActorIdleAnimation_43BFF0();
    gBattleCommandX553F78 = platform::gWindowWidth - 147;
    gBattleCommandY553F7C = -106;
    gBattleCommandValue553F80 = 17;
    gBattleSkillUsed5ABC68 = 0;
    const std::int32_t pet = FindBattlePetIndex_43C4B0();
    if (gValue553F90 != pet) {
        gValue553F84 = -1;
    }
}

// 原函数：0x00519920。
std::int32_t RefreshPetCount_519920()
{
    std::int32_t count = 0;
    gPetCountDE0B00 = 0;
    for (const PetRuntimeRecord& record : gPetRuntimeRecords) {
        if (record.used != 0) {
            ++count;
        }
    }
    gPetCountDE0B00 = count;
    return count;
}

// 原函数：0x00518580。
void BuildPetDisplayEntries_518580(
    const PetRuntimeRecord* source, PetPairRecord* destination, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        destination->used = source->used;
        destination->index = static_cast<std::uint16_t>(index);
        destination->value = source->value6D0;
        ++source;
        ++destination;
    }
}

// 原函数：0x00492250。
void MarkPetDetailDraftChanged_492250()
{
    gPetDetailDraftChanged_D67B28 = 1;
}

// 原两表均位于PE虚拟零区；5199A0后续只重置原表编号，不能清副本。
std::array<TitleRecord, 64> gTitlesFF0508 {}; // 原全局：0x00FF0508。

} // namespace detail
std::array<TitleRecord, 64> gSortedTitlesDE03F8 {}; // 原全局：0x00DE03F8。

namespace detail {

// 原 [0x5198A0,0x5198CB)：逐项前向复制 0x1C 字节，保留每个 W32 的读写顺序。
void CopyTitleRecords_5198A0(
    std::uint8_t* destination,
    const std::uint8_t* source,
    const std::int32_t count)
{
    for (std::int32_t record = 0; record < count; ++record) {
        for (std::int32_t word = 0; word < 7; ++word) {
            std::uint32_t value;
            std::memcpy(&value, source, sizeof(value));
            std::memcpy(destination, &value, sizeof(value));
            source += sizeof(value);
            destination += sizeof(value);
        }
    }
}

// MC01-C8：原28B完整称号记录接std::qsort，同键与双无效项等价。
// 原函数：0x005198D0。
void SortTitleRecords_5198D0(TitleRecord* const records, const std::uint32_t count)
{
    if (count < 2) {
        return;
    }
    std::qsort(records, count, sizeof(TitleRecord), &CompareTitleRecordsById_5198F0);
}

// MC01-C8：非负id升序在前，负id组等价，同键返回0。
// 原函数：0x005198F0。
std::int32_t CompareTitleRecordsById_5198F0(const void* const left, const void* const right)
{
    std::int32_t rightValue;
    std::memcpy(&rightValue, right, sizeof(rightValue));
    std::int32_t leftValue;
    std::memcpy(&leftValue, left, sizeof(leftValue));
    const bool leftUsed = leftValue >= 0;
    const bool rightUsed = rightValue >= 0;
    if (leftUsed != rightUsed) {
        return leftUsed ? -1 : 1;
    }
    if (!leftUsed) {
        return 0;
    }
    return (leftValue > rightValue) - (leftValue < rightValue);
}

} // namespace detail

// 原 0x581800：World::OpenDialog 共用的独立 W32 槽，原始初值 -1。
std::int32_t gCurrentDialogType_581800 = -1;
std::int32_t gDialogRequestCC21E0 = 0; // 原全局：0x00CC21E0。
std::int32_t gDialogTargetD2DE6C = 0; // 原全局：0x00D2DE6C。

namespace detail {

// 原 455BD0 的 23 个独立 W16/W32 共享槽；仅两个 556C 槽的映像初值为 1。
std::uint16_t gValue556C14 = 1; // 原全局：0x00556C14。
std::uint16_t gValue62B34E = 0; // 原全局：0x0062B34E。
std::uint16_t gValue62B34C = 0; // 原全局：0x0062B34C。
std::uint16_t gValue62B318 = 0; // 原全局：0x0062B318。
std::uint32_t gValue62B328 = 0; // 原全局：0x0062B328。
std::uint16_t gValue62B3B0 = 0; // 原全局：0x0062B3B0。
std::uint16_t gValue556C34 = 1; // 原全局：0x00556C34。
std::uint16_t gValue62B5AA = 0; // 原全局：0x0062B5AA。
std::uint16_t gValue62B5A8 = 0; // 原全局：0x0062B5A8。

} // namespace detail
std::uint64_t gMapNoticeStartTick_62B5B0 = 0; // 原全局：0x0062B5B0。

namespace detail {
std::uint32_t gValue62B308 = 0; // 原全局：0x0062B308。
std::uint32_t gValue62B398 = 0; // 原全局：0x0062B398。
std::uint32_t gValue62B358 = 0; // 原全局：0x0062B358。
std::uint32_t gValue62B384 = 0; // 原全局：0x0062B384。
std::uint32_t gValue62B310 = 0; // 原全局：0x0062B310。
std::uint32_t gValue62B364 = 0; // 原全局：0x0062B364。
std::uint32_t gValue62B338 = 0; // 原全局：0x0062B338。
std::uint32_t gValue62B36C = 0; // 原全局：0x0062B36C。
std::uint32_t gValue62B2F8 = 0; // 原全局：0x0062B2F8。
std::uint32_t gValue62B324 = 0; // 原全局：0x0062B324。
std::uint32_t gValue62B30C = 0; // 原全局：0x0062B30C。
std::uint32_t gValue62B388 = 0; // 原全局：0x0062B388。
std::uint32_t gValue62B3A0 = 0; // 原全局：0x0062B3A0。
std::uint32_t gValue62B38C = 0; // 原全局：0x0062B38C。

// 原 46BD40 的四个独立 W32 计数槽；仅承载原位模式。
std::uint32_t gPrimaryMapGraphicCount_9FB934 = 0; // 原全局：0x009FB934。
std::uint32_t gMapObjectGraphicCount_9FB928 = 0; // 原全局：0x009FB928。
std::uint32_t gSpecialMapGraphicCount_9A86C0 = 0; // 原全局：0x009A86C0。
std::uint32_t gValue9A7F78 = 0; // 原全局：0x009A7F78。

} // namespace detail

// 原59F9F8；50D120生产，421480后续复用。
std::int32_t gBattleBackgroundImage59F9F8 = 0;
std::uint8_t gBattleAttributeEffectFlags59F9E0 = 0; // 原全局：0x0059F9E0。
std::int32_t gBattleType5ABFC8 = 0; // 原全局：0x005ABFC8。
std::uint32_t gBattleModeFlag5ABFCC = 0; // 原全局：0x005ABFCC。
std::uint32_t gValue5ABB1C = 0; // 原全局：0x005ABB1C。
std::uint32_t gValue5ABB20 = 0xFFFFFFFFu; // 原全局：0x005ABB20。
std::uint32_t gValue5ABB28 = 0; // 原全局：0x005ABB28。

namespace detail {
std::uint32_t gBattleFirstTurnPending5ABC6C = 0; // 原全局：0x005ABC6C。
std::int32_t gBattleCWriteIndex5AAA6C = 0; // 原全局：0x005AAA6C。
std::int32_t gBattleCReadIndex5AAA44 = 0; // 原全局：0x005AAA44。
std::int32_t gBattleMWriteIndex5A3A1C = 0; // 原全局：0x005A3A1C。
std::int32_t gBattleMReadIndex5AAA68 = 0; // 原全局：0x005AAA68。
std::array<char, 0x4000> gBattleCQueue5A4A3C {}; // 原全局：0x005A4A3C。
std::array<char, 0x4000> gBattleMQueue59FA18 {}; // 原全局：0x0059FA18。

} // namespace detail
std::array<char, 0x1000> gBattleCText5A3A20 {}; // 原全局：0x005A3A20。
std::int32_t gBattleCOffset5ABA80 = 0; // 原全局：0x005ABA80。
std::uint32_t gBattleFieldFlags5ABB58 = 0; // 原全局：0x005ABB58。
std::array<char, 0x1000> gBattleMText5AAA7C {}; // 原全局：0x005AAA7C。
std::int32_t gBattleMScanOffset5A8A3C = 0; // 原全局：0x005A8A3C。
std::int32_t gBattleMPreviewOffset5AAA60 = 0; // 原全局：0x005AAA60。
std::uint32_t gBattleActiveEventMask5AAA74 = 0; // 原全局：0x005AAA74。
std::array<cg::yobi::YobiAction*, 32> gBattleEventActions5ABA9C {}; // 原全局：0x005ABA9C。
const std::array<std::int32_t, 5> gBattleValueThresholds54D93C {1, 4, 7, 10, 13}; // 原全局：0x0054D93C。

namespace detail {
static_assert(sizeof(gBattleEventActions5ABA9C) == 32 * sizeof(cg::yobi::YobiAction*));

} // namespace detail
std::array<std::uint8_t, 22> gBattlePreviewParticipants5AAA48 {}; // 原全局：0x005AAA48。
std::array<std::array<std::int32_t, 2>, 22> gBattleEffectPairs5A8A40 {}; // 原全局：0x005A8A40。
std::array<BattleNamedEffectRecord, 20> gBattleNamedEffects5ABCE0 {}; // 原全局：0x005ABCE0。

namespace detail {
static_assert(sizeof(BattleNamedEffectRecord) == 36);
static_assert(offsetof(BattleNamedEffectRecord, value) == 32);
static_assert(sizeof(gBattleNamedEffects5ABCE0) == 720);

} // namespace detail
cg::yobi::YobiAction* gBattleSceneAction5ABA94 = nullptr; // 原全局：0x005ABA94。
std::array<cg::yobi::YobiAction*, 22> gBattleAllEntries59E980 {}; // 原全局：0x0059E980。
constinit const std::span<cg::yobi::YobiAction*, 20> gBattleEntries59E980 { // 原全局：0x0059E980。
    gBattleAllEntries59E980.data(), 20
};
const std::array<std::int32_t, 22> gBattlePointIndices5536FC { // 原全局：0x005536FC。
    2, 1, 3, 0, 4, 7, 6, 8, 5, 9, 12, 11, 13, 10, 14, 17, 16, 18, 15, 19, 20, 21
};
std::array<std::array<std::int32_t, 4>, 22> gBattleGraphicPoints553758 {{ // 原全局：0x00553758。
    {910, 590, 610, 290}, {845, 625, 545, 325}, {780, 660, 480, 360},
    {715, 695, 415, 395}, {650, 730, 350, 430}, {845, 540, 545, 240},
    {781, 576, 481, 276}, {718, 613, 418, 313}, {654, 649, 354, 349},
    {590, 685, 290, 385}, {-14, -194, 285, 105}, {-77, -157, 222, 142},
    {-141, -121, 158, 178}, {-205, -85, 94, 214}, {-269, -49, 30, 250},
    {45, -143, 345, 156}, {-18, -107, 281, 192}, {-81, -71, 218, 228},
    {-145, -35, 154, 264}, {-209, 0, 90, 300}, {768, 663, 418, 313},
    {-131, -121, 218, 228},
}};

std::int32_t gBattleCurrentPosition5ABCA4 = 0; // 原全局：0x005ABCA4。

namespace detail {
std::uint32_t gBattlePosition5ABCA8 = 0; // 原全局：0x005ABCA8。

} // namespace detail
std::int32_t gBattlePlayerActionState5ABC74 = 0; // 原全局：0x005ABC74。
std::array<std::int32_t, 9> gBattleSelectionModes5ABC7C {}; // 原全局：0x005ABC7C。
std::int32_t gBattleSelectedSkill5ABB5C = 0; // 原全局：0x005ABB5C。
std::int32_t gBattleSelectedTechnique5ABC78 = 0; // 原全局：0x005ABC78。
std::int32_t gBattleCommandState5ABC3C = 0; // 原全局：0x005ABC3C。
std::uint8_t gBattleRebirthDisabled5ABC40 = 0; // 原全局：0x005ABC40。
std::int32_t gBattleSkillActionCount5ABBF4 = 0; // 原全局：0x005ABBF4。
std::int32_t gBattlePetActionCount5ABBBC = 0; // 原全局：0x005ABBBC。
std::int32_t gBattleCountdownActive5ABCB8 = 0; // 原全局：0x005ABCB8。
std::uint64_t gBattleCommandDeadline5ABBF8 = 0; // 原全局：0x005ABBF8。
std::uint32_t gBattleCommandWaitMs = 300000; // G01：BtWt更新，下次命令计时生效。
bool gBattleShowEnemyHpBars = false; // G02：BtWt血条及敌方悬停数值开关。
std::uint32_t gChatMinimumIntervalMs = 200; // G06：SpDt与Sub_50C940共享；唯一定义位于cg::world::detail。
std::int32_t gBattleCountdownSecond5ABB80 = 0; // 原全局：0x005ABB80。
std::int32_t gBattleSnapshotPetIndex5ABBFC = 0; // 原全局：0x005ABBFC。
std::int32_t gBattleCommandX553F78 = 490; // 原全局：0x00553F78。
std::int32_t gBattleCommandY553F7C = -104; // 原全局：0x00553F7C。
std::int32_t gBattleCommandValue553F80 = 17; // 原全局：0x00553F80。
std::int32_t gBattlePetSkillGraphicIndex553F8C = -1; // 原全局：0x00553F8C。
std::array<std::int32_t, 9> gBattleResetValues5ABB84 {}; // 原全局：0x005ABB84。
cg::yobi::YobiAction* gBattleEntryNoticeAction5ABFB8 = nullptr; // 原全局：0x005ABFB8。
std::int32_t gBattleSelectionMode5ABC70 = 0; // 原全局：0x005ABC70。
std::int32_t gBattleSkillUsed5ABC68 = 0; // 原全局：0x005ABC68。
std::int32_t gBattlePetCommandPhase553F88 = 1; // 原全局：0x00553F88。
std::int32_t gBattleValue5ABCC0 = 0; // 原全局：0x005ABCC0。
cg::yobi::YobiAction* gBattleCommandAction5ABC00 = nullptr; // 原全局：0x005ABC00。
std::uint32_t gBattleTargetFlags5ABC44 = 0; // 原全局：0x005ABC44。
std::int32_t gBattleSelectedItem5ABC48 = 0; // 原全局：0x005ABC48。
std::int32_t gBattleItemTargetPending5ABCC4 = 0; // 原全局：0x005ABCC4。
std::int32_t gBattleBombLevelD67B2C = 0; // 原全局：0x00D67B2C。
std::uint32_t gBattlePlayerFlags5ABCAC = 0; // 原全局：0x005ABCAC。
std::uint32_t gBattleSkillFlags5ABC04 = 0; // 原全局：0x005ABC04。
std::uint32_t gBattlePetSkillFlags5ABB78 = 0; // 原全局：0x005ABB78。

namespace detail {
std::uint32_t gBattleRebirthLevel5ABB7C = 0; // 原全局：0x005ABB7C。

} // namespace detail
std::uint32_t gBattleRidePet5ABC38 = 0; // 原全局：0x005ABC38。
std::uint32_t gBattleEndBits5ABBC0 = 0; // 原全局：0x005ABBC0。
std::uint32_t gBattleTurn5ABB74 = 0; // 原全局：0x005ABB74。
std::uint32_t gBattleTurnSnapshot5ABC08 = 0; // 原全局：0x005ABC08。

namespace detail {
std::uint32_t gValue5ABCB0 = 0; // 原全局：0x005ABCB0。
static_assert(sizeof(gBattleAllEntries59E980) == 22 * sizeof(cg::yobi::YobiAction*));
static_assert(decltype(gBattleEntries59E980)::extent == 20);
static_assert(sizeof(gBattleCQueue5A4A3C) == 4 * 0x1000);
static_assert(sizeof(gBattleMQueue59FA18) == 4 * 0x1000);
static_assert(sizeof(gBattleCText5A3A20) == 0x1000);
static_assert(sizeof(gBattleMText5AAA7C) == 0x1000);
// 原5ABCD8；43CB10末尾重读图号后发布，DrawBattleScene用于重建判断。
std::int32_t gBattleBackgroundSnapshot5ABCD8 = 0;

} // namespace detail

// 原 0xD2F788/0xFF0C10：504A30 清零的独立 S32/W16 状态槽。
std::int32_t gValueD2F788 = 0; // 原全局：0x00D2F788。
std::uint16_t gPendingResultWindowMode_FF0C10 = 0; // 原全局：0x00FF0C10。

// 原 0xFF0D04/0xFF0D08：50D700 写入、展示侧按 S32 读取的独立槽。
std::int32_t gDpResultDelta_FF0D04 = 0; // 原全局：0x00FF0D04。
std::int32_t gDpResultTotal_FF0D08 = 0; // 原全局：0x00FF0D08。

// 原 0xFF0D34：50D3B0 写入、5199A0 清零的独立 W16 槽。
std::uint16_t gValueFF0D34 = 0;

namespace detail {
// 原 519A80 公共尾；协议与普通场景后续复用，初值为 PE 零填充。
std::uint16_t gValueFF0D30 = 0; // 原全局：0x00FF0D30。
std::uint16_t gValueFF0D32 = 0; // 原全局：0x00FF0D32。
std::uint16_t gValueF7CA04 = 0; // 原全局：0x00F7CA04。
std::uint16_t gValueFF03D8 = 0; // 原全局：0x00FF03D8。
std::uint16_t gValueFF0D36 = 0; // 原全局：0x00FF0D36。
std::uint64_t gValueFF0D38 = 0; // 原全局：0x00FF0D38。

// 原 0x9A6204：45ED80 重置、45ED90 读取并推进的场景 7 W32 状态。
std::uint32_t gValue9A6204 = 0;
// 原 45ED90 的四个布局 W32 与非拥有 action 槽，均只由本体直接读写。
std::int32_t gValue9A5674 = 0; // 原全局：0x009A5674。
std::int32_t gValue9A594C = 0; // 原全局：0x009A594C。
std::int32_t gValue9A61F8 = 0; // 原全局：0x009A61F8。
std::int32_t gValue9A5AC0 = 0; // 原全局：0x009A5AC0。
cg::yobi::YobiAction* gAction9A6E98 = nullptr; // 原全局：0x009A6E98。

} // namespace detail

// 原 0x5ABB70/0x5ABB6C/0x553F90/0x553F84；43C8F0 顺序重置的独立 S32 槽。
std::int32_t gValue5ABB70 = 0; // 原全局：0x005ABB70。
std::int32_t gValue5ABB6C = 0; // 原全局：0x005ABB6C。
std::int32_t gValue553F90 = -1; // 原全局：0x00553F90。
std::int32_t gValue553F84 = -1; // 原全局：0x00553F84。

namespace detail {

// 原 0xC8222E；pending 消费与世界循环尾部共用的 W16 门禁。
std::uint16_t gPlayerReachedMovementTarget_C8222E = 0;

// 原 0xC82230/0xC82234/0xC82236；供 46A9E0 初始化和后续消费者共享。
std::uint32_t gPendingPathEventActive_C82230 = 0; // 原全局：0x00C82230。
std::int16_t gPendingPathEventSteps_C82234 = 0; // 原全局：0x00C82234。
std::int16_t gPendingPathEventType_C82236 = 0; // 原全局：0x00C82236。

} // namespace detail

// 原 0xC3500C/0xC75010/0x9EE8A0/0x9ED438；468F30 的独立 W32 槽。
std::uint32_t gMapProjectionOffsetX_C3500C = 0; // 原全局：0x00C3500C。
std::uint32_t gMapProjectionOffsetY_C75010 = 0; // 原全局：0x00C75010。

namespace detail {
std::uint32_t gValue9EE8A0 = 0; // 原全局：0x009EE8A0。
std::uint32_t gValue9ED438 = 0; // 原全局：0x009ED438。
std::uint32_t gDirectionalMapTransitionHeading_9ED440 = 0; // 原全局：0x009ED440。

// 原 0xC35008/0x9DAD2C/0x9FB930/0xC821D4/0x9A86DC；4682E0 的 W32 目标。
std::uint32_t gValueC35008 = 0; // 原全局：0x00C35008。
std::uint32_t gValue9DAD2C = 0; // 原全局：0x009DAD2C。
std::uint32_t gMapWidth_9FB930 = 0; // 原全局：0x009FB930。
std::uint32_t gMapHeight_C821D4 = 0; // 原全局：0x00C821D4。

} // namespace detail
std::uint32_t gCurrentMapId_9A86DC = 0; // 原全局：0x009A86DC。

std::int16_t gMapXStart = 0; // 原全局：0x009DEF64。
std::int16_t gMapYStart = 0; // 原全局：0x009DEF60。

namespace detail {
std::int16_t gMapXEnd = 0; // 原全局：0x009DEF62。
std::int16_t gMapYEnd = 0; // 原全局：0x009DEF5E。

} // namespace detail
std::int16_t gMapSpanX = 0; // 原全局：0x009A86E0。
std::int16_t gMapSpanY = 0; // 原全局：0x009ED43C。

namespace detail {
 // 原全局：0x009EE8A8。
 // 原全局：0x009EE8C0。
 // 原全局：0x009EE8D0。
 // 原全局：0x009EE8E0。
 // 原全局：0x00C27FF8。
 // 原全局：0x00C27FE8。
 // 原全局：0x009A7F88。
 // 原全局：0x009A7F98。
 // 原全局：0x00C821E8。
 // 原全局：0x009E03C0。
 // 原全局：0x009FB918。
 // 原全局：0x00AF0FE8。
 // 原全局：0x00C821D8。
std::uint32_t gPlayerMapCoordinateSum_AF10C0 = 0; // 原全局：0x00AF10C0。
std::uint32_t gValue9EE8F8 = 0; // 原全局：0x009EE8F8。
std::uint32_t gPlayerMovementStepXBits_9EE8FC = 0; // 原全局：0x009EE8FC。
std::uint32_t gPlayerMovementStepYBits_9EE900 = 0; // 原全局：0x009EE900。

} // namespace detail
std::uint32_t gPartyCatchUpRateBits_9DAD78 = 0; // 原全局：0x009DAD78。

namespace detail {
std::uint8_t gValue9A4B94 = 0; // 原全局：0x009A4B94。

} // namespace detail
std::uint16_t gMovePathCount_C82200 = 0; // 原全局：0x00C82200。
std::uint16_t gPendingDirectionCount_C82202 = 0; // 原全局：0x00C82202。

namespace detail {
std::array<std::int16_t, 2> gMapWindowEndY9A86C4 {}; // 原全局：0x009A86C4。
std::array<std::int16_t, 2> gMapWindowStartY9A86C8 {}; // 原全局：0x009A86C8。
std::array<std::int16_t, 2> gMapWindowEndX9A86CC {}; // 原全局：0x009A86CC。
std::array<std::int16_t, 2> gMapWindowStartX9A86D0 {}; // 原全局：0x009A86D0。
std::array<std::uint16_t, 2> gPendingDirections9A86D4 {}; // 原全局：0x009A86D4。
std::array<std::uint16_t, kMovePathCapacity> gMovePathDirections {};
std::array<std::uint16_t, kMovePathCapacity> gMovePathX {};
std::array<std::uint16_t, kMovePathCapacity> gMovePathY {};
static_assert(sizeof(gMovePathDirections) == 0xC8);
static_assert(sizeof(gMovePathX) == 0xC8);
static_assert(sizeof(gMovePathY) == 0xC8);
static_assert(sizeof(gMapWindowEndY9A86C4) == 4);
static_assert(sizeof(gMapWindowStartY9A86C8) == 4);
static_assert(sizeof(gMapWindowEndX9A86CC) == 4);
static_assert(sizeof(gMapWindowStartX9A86D0) == 4);
static_assert(sizeof(gPendingDirections9A86D4) == 4);

} // namespace detail
std::uint32_t gPlayerPathRebuildPending_C821FC = 0; // 原全局：0x00C821FC。

namespace detail {
std::uint32_t gWaitingForMapRectangleData_9EE8B8 = 0; // 原全局：0x009EE8B8。
std::uint16_t gRequestedMapRectangleDirection_9EE8A4 = 0; // 原全局：0x009EE8A4。
std::int32_t gMapRectangleRequestPlayerX_9A7F7C = 0; // 原全局：0x009A7F7C。
std::int32_t gMapRectangleRequestPlayerY_9A7F80 = 0; // 原全局：0x009A7F80。
std::uint64_t gMapRectangleRequestStartTick_AF10C8 = 0; // 原全局：0x00AF10C8。
std::uint16_t gContinuousMapMovementActive_C8A79C = 0; // 原全局：0x00C8A79C。

} // namespace detail
std::uint16_t gMapTransitionRequestPending_9A86D8 = 0; // 原全局：0x009A86D8。
std::uint16_t gDirectionalMapTransitionPending_9FB92E = 0; // 原全局：0x009FB92E。

namespace detail {
std::uint16_t gValueB65A7C = 0; // 原全局：0x00B65A7C。

} // namespace detail
std::uint16_t gMapMovementInputBlocked_C82220 = 0; // 原全局：0x00C82220。

namespace detail {
std::uint64_t gLastCursorFacingRequestTick_C82238 = 0; // 原全局：0x00C82238。
std::uint64_t gLastExplicitFacingRequestTick_C8223C = 0; // 原全局：0x00C8223C。 原转向发送的独立GetTickCount位槽。
std::uint32_t gMapLeftButtonSuppressed_C8A774 = 0; // 原全局：0x00C8A774。
std::uint64_t gMapLeftHoldDurationMs_C8A778 = 0; // 原全局：0x00C8A778。

} // namespace detail
std::uint32_t gValueC8A77C = 0; // 原全局：0x00C8A77C。
std::uint32_t gValueC8A780 = 0; // 原全局：0x00C8A780。

namespace detail {
std::int32_t gPlayerMoveTargetX = 0; // 原C8A784；46F716发布。
std::int32_t gPlayerMoveTargetY = 0; // 原C8A788。
std::int32_t gMouseWorldPixelX_C8A78C = 0; // 原全局：0x00C8A78C。
std::int32_t gMouseWorldPixelY_C8A790 = 0; // 原全局：0x00C8A790。
std::uint64_t gMapLeftPressTick_C8A794 = 0; // 原全局：0x00C8A794。
std::uint32_t gMapTargetLeftClick_C8A7A0 = 0; // 原全局：0x00C8A7A0。
std::uint32_t gMapTargetLeftHeld_C8A7A4 = 0; // 原全局：0x00C8A7A4。
std::uint32_t gMapTargetInteractionClick_C8A7A8 = 0; // 原全局：0x00C8A7A8。
std::uint32_t gMapTargetRightHeld_C8A7AC = 0; // 原全局：0x00C8A7AC。
std::uint64_t gLastMapMoveTargetTick_55DDAC = UINT64_MAX; // 原全局：0x0055DDAC。
std::uint32_t gMapRectangleRequestPending_C82208 = 0; // 原全局：0x00C82208。
std::uint32_t gMapTransitionObservationPrepared_C8220C = 0; // 原全局：0x00C8220C。
std::uint32_t gMapTransitionReplyPending_C82210 = 0; // 原全局：0x00C82210。
std::uint32_t gMapTransitionAwaitingRectangle_C82214 = 0; // 原全局：0x00C82214。
std::uint32_t gMapNameReceivedDuringTransition_C82218 = 0; // 原全局：0x00C82218。
std::int32_t gPendingMapPaletteIndex_FF0D4C = 0; // 原全局：0x00FF0D4C。
std::int32_t gDayPeriodPaletteEnabled_73ED44 = 0; // 原全局：0x0073ED44。

} // namespace detail
std::int32_t gCurrentDayPeriod_73ED48 = 0; // 原全局：0x0073ED48。

namespace detail {
std::uint32_t gValueC82224 = 0; // 原全局：0x00C82224。
std::uint32_t gMapTransitionRequestSequence_C8221C = 0; // 原全局：0x00C8221C。
std::uint16_t gPendingMapTransitionSequence_9DAD54 = 0; // 原全局：0x009DAD54。
std::uint16_t gMissingMapRectangleCount_9DAD56 = 0; // 原全局：0x009DAD56。
std::uint16_t gPendingDirectionalTransitionSequence_9FB92C = 0; // 原全局：0x009FB92C。
std::uint16_t gPlayerPathFinalFacingDirection_55D9FC = 0xFFFFu; // 原全局：0x0055D9FC。
std::uint16_t gValueC821D0 = 0; // 原全局：0x00C821D0。
std::uint16_t gMapWindowReloadPending_9EE910 = 0; // 原全局：0x009EE910。

} // namespace detail
std::array<std::uint16_t, kMapBufferCells> gMapTileArray {};
std::array<std::uint16_t, kMapBufferCells> gMapObjArray {};
std::array<std::uint16_t, kMapBufferCells> gMapHitArray {};
std::array<std::uint16_t, kMapBufferCells> gMapWalkabilityArray {}; // 原全局：0x009DEF68。

namespace detail {
std::uint16_t gLastDetectedCode = 0; // 原全局：0x009DAD4A。

} // namespace detail
std::int32_t gMapCachePathKey_9E0434 = 0; // 原全局：0x009E0434。

namespace detail {

// 原 [0x468570,0x4687A3)：仅缺文件路径尝试建目录，旧头比较后可重写。
std::int32_t __cdecl EnsureMapCacheFile_468570(
    const std::int32_t a1, const std::int32_t a2,
    const std::int32_t a3, const std::int32_t a4,
    const std::int32_t a5, const std::int32_t a6,
    const std::int32_t a7)
{
    if (a3 == 0) {
        return 0;
    }

    char path[256];
    FormatMapDataPath_468520(path, a2, a1, a3);
    std::FILE* const file =
        std::fopen(path, kMapReadMode);
    if (file == nullptr) {
        ::_mkdir(kMapPathPrefix);
        char directory[256];
        std::sprintf(
            directory, "%s\\%d", kMapPathPrefix, a2);
        ::_mkdir(directory);
        if (a2 > 0) {
            std::sprintf(
                directory, "%s\\%d\\%d", kMapPathPrefix, a2, a1);
            ::_mkdir(directory);
        }
        return CreateEmptyMapCacheFile_4687B0(path, a6, a7, a4, a5) >= 0 ? 1 : -1;
    }

    ::_fseeki64(file, 0, SEEK_SET);
    char label[4];
    label[0] = static_cast<char>(std::fgetc(file));
    label[1] = static_cast<char>(std::fgetc(file));
    label[2] = static_cast<char>(std::fgetc(file));
    label[3] = '\0';
    const std::uint8_t version = static_cast<std::uint8_t>(
        std::fgetc(file));
    std::int32_t oldSixth = 0;
    std::int32_t oldSeventh = 0;
    std::int32_t oldFourth = 0;
    std::int32_t oldFifth = 0;
    static_cast<std::uint32_t>(std::fread(&oldSixth, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&oldSeventh, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&oldFourth, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&oldFifth, 4, 1, file));
    std::fclose(file);

    if (a6 != oldSixth || a7 != oldSeventh
        || a4 != oldFourth || a5 != oldFifth
        || std::strcmp(label, "MAP") != 0
        || version != 1 || a2 == 2) {
        return CreateEmptyMapCacheFile_4687B0(path, a6, a7, a4, a5) < 0 ? -1 : 0;
    }
    return 0;
}

// 原 [0x4687B0,0x4688A0)：四字段头后，三平面逐 WORD 写零。
std::int32_t __cdecl CreateEmptyMapCacheFile_4687B0(
    const char* const path,
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t height,
    const std::int32_t width)
{
    std::FILE* const file =
        std::fopen(path, "wb");
    if (file == nullptr) {
        return -1;
    }
    std::fputc('M', file);
    std::fputc('A', file);
    std::fputc('P', file);
    std::fputc(1, file);
    static_cast<std::uint32_t>(std::fwrite(&first, 4, 1, file));
    static_cast<std::uint32_t>(std::fwrite(&second, 4, 1, file));
    static_cast<std::uint32_t>(std::fwrite(&height, 4, 1, file));
    static_cast<std::uint32_t>(std::fwrite(&width, 4, 1, file));
    const std::uint32_t zero = 0;
    for (std::int32_t plane = 0; plane < 3; ++plane) {
        for (std::int32_t row = 0; row < height; ++row) {
            for (std::int32_t column = 0; column < width; ++column) {
                static_cast<std::uint32_t>(std::fwrite(&zero, 2, 1, file));
            }
        }
    }
    std::fclose(file);
    return 0;
}

// 原 [0x4688A0,0x468BF5)：先写 tile/obj，hit 逐行更新共享缓冲后再写盘。
std::int32_t __cdecl WriteMapCacheRectangle_4688A0(
    const std::int32_t mapId, const std::int32_t floor,
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    const std::uint16_t* const tile, const std::uint16_t* const obj,
    std::uint16_t* const hit)
{
    char path[256];
    FormatMapDataPath_468520(path, mapId, gMapCachePathKey_9E0434, floor);
    std::FILE* const file =
        std::fopen(path, "rb+");
    if (file == nullptr) {
        return -1;
    }

    for (int i = 0; i < 4; ++i) {
        std::fgetc(file);
    }
    std::int32_t ignoredFirst = 0;
    std::int32_t ignoredSecond = 0;
    std::int32_t fileWidth = 0;
    std::int32_t fileHeight = 0;
    static_cast<std::uint32_t>(std::fread(&ignoredFirst, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&ignoredSecond, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&fileWidth, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&fileHeight, 4, 1, file));

    const std::int32_t destinationWidth = xEnd - xStart;
    const std::int32_t sourceX = std::max(xStart, 0);
    const std::int32_t destinationX = std::max(-xStart, 0);
    const std::int32_t sourceY = std::max(yStart, 0);
    const std::int32_t destinationY = std::max(-yStart, 0);
    const std::int32_t copyWidth = std::min(xEnd, fileWidth) - sourceX;
    const std::int32_t copyHeight = std::min(yEnd, fileHeight) - sourceY;
    const std::int32_t sourceStart = sourceX + fileWidth * sourceY;
    const std::int32_t destinationStart =
        destinationX + destinationWidth * destinationY;
    const std::int32_t planeBytes = 2 * fileWidth * fileHeight;
    const std::uint32_t rowBytes =
        static_cast<std::uint32_t>(2 * copyWidth);

    if (copyHeight > 0) {
        const std::uint16_t* const planes[] {tile, obj};
        for (std::int32_t plane = 0; plane < 2; ++plane) {
            for (std::int32_t row = 0; row < copyHeight; ++row) {
                const std::int32_t fileRow =
                    20 + plane * planeBytes
                    + 2 * (sourceStart + row * fileWidth);
                ::_fseeki64(file, static_cast<std::int64_t>(static_cast<std::uint32_t>(fileRow)), SEEK_SET);
                static_cast<std::uint32_t>(std::fwrite(
                    planes[plane] + destinationStart + row * destinationWidth,
                    static_cast<std::uint32_t>(rowBytes), 1, file));
            }
        }

        for (std::int32_t row = 0; row < copyHeight; ++row) {
            std::uint16_t* const inputRow =
                hit + destinationStart + row * destinationWidth;
            const std::int32_t y = yStart + row;
            for (std::int32_t column = 0; column < copyWidth; ++column) {
                const std::uint16_t value =
                    static_cast<std::uint16_t>(inputRow[column] | 0xC000u);
                inputRow[column] = value;
                const std::int32_t x = xStart + column;
                if (gMapFloor == floor
                    && gMapXStart <= x && x < gMapXEnd
                    && gMapYStart <= y && y < gMapYEnd) {
                    static_cast<void>(SetMapCellHitValue_468C00(x, y, value));
                }
            }
            const std::int32_t fileRow =
                20 + 2 * planeBytes
                + 2 * (sourceStart + row * fileWidth);
            ::_fseeki64(file, static_cast<std::int64_t>(static_cast<std::uint32_t>(fileRow)), SEEK_SET);
            static_cast<std::uint32_t>(std::fwrite(inputRow, static_cast<std::uint32_t>(rowBytes), 1, file));
        }
    }
    std::fclose(file);
    return 0;
}

// 原 [0x469870,0x469C87)：四段定点 hit 探测，最多发布两项裁剪窗口。
std::int32_t FindMissingMapRectanglesAhead_469870(const std::int32_t direction)
{
    if (gMapSpanX < 51 || gMapSpanY < 51) {
        return 0;
    }
    if ((gPlayerX > 11)) {
        static_cast<void>(
            (gPlayerX - 11));
    }
    if ((gPlayerY > 11)) {
        static_cast<void>(
            (gPlayerY - 11));
    }
    if ((static_cast<std::int32_t>(std::bit_cast<std::int32_t>(gMapWidth_9FB930)) - gPlayerX) > 11) {
        static_cast<void>(
            (gPlayerX + 11));
    }
    if ((static_cast<std::int32_t>(std::bit_cast<std::int32_t>(gMapHeight_C821D4)) - gPlayerY) > 11) {
        static_cast<void>(
            (gPlayerY + 11));
    }

    gMissingMapRectangleCount_9DAD56 = 0;
    std::int32_t found = 0;
    if (direction == 6 || direction == 7 || direction == 5) {
        const std::int32_t x =
            (gPlayerX - 11);
        std::int32_t y =
            (gPlayerY - 11);
        const std::int32_t span = gMapSpanX;
        for (std::int32_t step = 0; step < 23; ++step, ++y) {
            if (x >= 0 && x < std::bit_cast<std::int32_t>(gMapWidth_9FB930)
                && y >= 0 && y < std::bit_cast<std::int32_t>(gMapHeight_C821D4)
                && gMapHitArray[static_cast<std::size_t>(
                    span * (step + 9) + 19)] == 0) {
                const std::size_t index = gMissingMapRectangleCount_9DAD56;
                gMapWindowEndX9A86CC[index] = static_cast<std::int16_t>(x + 1);
                gMapWindowStartX9A86D0[index] = static_cast<std::int16_t>(x - 1);
                gMapWindowStartY9A86C8[index] = static_cast<std::int16_t>(y - 1);
                gMapWindowEndY9A86C4[index] =
                    static_cast<std::int16_t>(y - step + 23);
                static_cast<void>(ClipMapRectangleToBounds_468290(
                    &gMapWindowStartX9A86D0[index],
                    &gMapWindowStartY9A86C8[index],
                    &gMapWindowEndX9A86CC[index],
                    &gMapWindowEndY9A86C4[index]));
                ++gMissingMapRectangleCount_9DAD56;
                found = 1;
                break;
            }
        }
    }

    if (direction == 0 || direction == 7 || direction == 1) {
        std::int32_t x =
            (gPlayerX - 11);
        const std::int32_t y =
            (gPlayerY - 11);
        const std::int32_t span = gMapSpanX;
        const std::int32_t xBound = std::bit_cast<std::int32_t>(gMapWidth_9FB930);
        for (std::int32_t step = 0; step < 23; ++step, ++x) {
            if (x >= 0 && x < xBound
                && y >= 0 && y < std::bit_cast<std::int32_t>(gMapHeight_C821D4)
                && gMapHitArray[static_cast<std::size_t>(
                    9 * span + 19 + step)] == 0) {
                const std::size_t index = gMissingMapRectangleCount_9DAD56;
                gMapWindowStartY9A86C8[index] = static_cast<std::int16_t>(y - 1);
                gMapWindowStartX9A86D0[index] = static_cast<std::int16_t>(x - 1);
                gMapWindowEndX9A86CC[index] =
                    static_cast<std::int16_t>(x - step + 23);
                gMapWindowEndY9A86C4[index] = static_cast<std::int16_t>(y + 1);
                static_cast<void>(ClipMapRectangleToBounds_468290(
                    &gMapWindowStartX9A86D0[index],
                    &gMapWindowStartY9A86C8[index],
                    &gMapWindowEndX9A86CC[index],
                    &gMapWindowEndY9A86C4[index]));
                ++gMissingMapRectangleCount_9DAD56;
                found = 1;
                break;
            }
        }
    }
    if (std::bit_cast<std::int16_t>(gMissingMapRectangleCount_9DAD56) >= 2) {
        return found;
    }

    if (direction == 2 || direction == 1 || direction == 3) {
        const std::int32_t x =
            (gPlayerX + 11);
        std::int32_t y =
            (gPlayerY - 11);
        const std::int32_t span = gMapSpanX;
        const std::int32_t xBound = std::bit_cast<std::int32_t>(gMapWidth_9FB930);
        for (std::int32_t step = 0; step < 23; ++step, ++y) {
            if (x >= 0 && x < xBound
                && y >= 0 && y < std::bit_cast<std::int32_t>(gMapHeight_C821D4)
                && gMapHitArray[static_cast<std::size_t>(
                    span * (step + 9) + 41)] == 0) {
                const std::size_t index = gMissingMapRectangleCount_9DAD56;
                gMapWindowStartY9A86C8[index] = static_cast<std::int16_t>(y - 1);
                gMapWindowStartX9A86D0[index] = static_cast<std::int16_t>(x);
                gMapWindowEndX9A86CC[index] = static_cast<std::int16_t>(x + 2);
                gMapWindowEndY9A86C4[index] =
                    static_cast<std::int16_t>(y - step + 23);
                static_cast<void>(ClipMapRectangleToBounds_468290(
                    &gMapWindowStartX9A86D0[index],
                    &gMapWindowStartY9A86C8[index],
                    &gMapWindowEndX9A86CC[index],
                    &gMapWindowEndY9A86C4[index]));
                ++gMissingMapRectangleCount_9DAD56;
                found = 1;
                break;
            }
        }
    }
    if (std::bit_cast<std::int16_t>(gMissingMapRectangleCount_9DAD56) >= 2) {
        return found;
    }

    if (direction == 4 || direction == 3 || direction == 5) {
        std::int32_t x =
            (gPlayerX - 11);
        const std::int32_t y =
            (gPlayerY + 11);
        const std::int32_t span = gMapSpanX;
        const std::int32_t xBound = std::bit_cast<std::int32_t>(gMapWidth_9FB930);
        for (std::int32_t step = 0; step < 23; ++step, ++x) {
            if (x >= 0 && x < xBound
                && y >= 0 && y < std::bit_cast<std::int32_t>(gMapHeight_C821D4)
                && gMapHitArray[static_cast<std::size_t>(
                    31 * span + 19 + step)] == 0) {
                const std::size_t index = gMissingMapRectangleCount_9DAD56;
                gMapWindowStartY9A86C8[index] = static_cast<std::int16_t>(y);
                gMapWindowStartX9A86D0[index] = static_cast<std::int16_t>(x - 1);
                gMapWindowEndX9A86CC[index] =
                    static_cast<std::int16_t>(x - step + 23);
                gMapWindowEndY9A86C4[index] = static_cast<std::int16_t>(y + 2);
                static_cast<void>(ClipMapRectangleToBounds_468290(
                    &gMapWindowStartX9A86D0[index],
                    &gMapWindowStartY9A86C8[index],
                    &gMapWindowEndX9A86CC[index],
                    &gMapWindowEndY9A86C4[index]));
                ++gMissingMapRectangleCount_9DAD56;
                return 1;
            }
        }
    }
    return found;
}

// 原 [0x46AF30,0x46AFDE)：探测成功后逐项发送 UUN 并发布玩家快照。
std::uint32_t RequestMissingMapRectanglesAhead_46AF30()
{
    const std::int32_t direction =
        std::bit_cast<std::int16_t>(gPendingDirections9A86D4[0]);
    if (FindMissingMapRectanglesAhead_469870(direction) == 0) {
        return 0;
    }

    std::int32_t index = 0;
    while (index < std::bit_cast<std::int16_t>(gMissingMapRectangleCount_9DAD56)) {
        const std::size_t position = static_cast<std::size_t>(index);
        const std::int32_t endY = gMapWindowEndY9A86C4[position];
        const std::int32_t endX = gMapWindowEndX9A86CC[position];
        const std::int32_t startY = gMapWindowStartY9A86C8[position];
        const std::int32_t startX = gMapWindowStartX9A86D0[position];
        const std::int32_t floor = gMapFloor;
        const std::int32_t mapId = std::bit_cast<std::int32_t>(gCurrentMapId_9A86DC);
        const std::uintptr_t socket = platform::gConnectionSocket;
        static_cast<void>(network::RequestMapRectangle_510710(
            socket, mapId, floor, startX, startY, endX, endY));
        ++index;
    }

    gWaitingForMapRectangleData_9EE8B8 = 1;
    gRequestedMapRectangleDirection_9EE8A4 = static_cast<std::uint16_t>(direction);
    gMapRectangleRequestPlayerX_9A7F7C = static_cast<std::int32_t>(gPlayerX);
    gMapRectangleRequestPlayerY_9A7F80 = static_cast<std::int32_t>(gPlayerY);
    const std::uint64_t tick = platform::ReadTickCount64();
    gMapRectangleRequestStartTick_AF10C8 = tick;
    return static_cast<std::uint32_t>(tick);
}

// 原 [0x468C40,0x468F25)：成功打开才清三平面，逐行短读保留已清零后缀。
std::int32_t __cdecl ReadMapCacheRectangle_468C40(
    const std::int32_t mapId, const std::int32_t floor,
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    std::uint16_t* const tile, std::uint16_t* const obj,
    std::uint16_t* const hit)
{
    char path[256];
    FormatMapDataPath_468520(path, mapId, gMapCachePathKey_9E0434, floor);
    std::FILE* const file =
        std::fopen(path, kMapReadMode);
    if (file == nullptr) {
        ::PostQuitMessage(0);
        return -1;
    }

    std::memset(tile, 0, kMapBufferCells * sizeof(std::uint16_t));
    std::memset(obj, 0, kMapBufferCells * sizeof(std::uint16_t));
    std::memset(hit, 0, kMapBufferCells * sizeof(std::uint16_t));
    for (int i = 0; i < 4; ++i) {
        std::fgetc(file);
    }

    std::int32_t ignoredFirst = 0;
    std::int32_t ignoredSecond = 0;
    std::int32_t fileWidth = 0;
    std::int32_t fileHeight = 0;
    static_cast<std::uint32_t>(std::fread(&ignoredFirst, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&ignoredSecond, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&fileWidth, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&fileHeight, 4, 1, file));

    const std::int32_t destinationWidth = xEnd - xStart;
    const std::int32_t sourceX = std::max(xStart, 0);
    const std::int32_t destinationX = std::max(-xStart, 0);
    const std::int32_t sourceY = std::max(yStart, 0);
    const std::int32_t destinationY = std::max(-yStart, 0);
    const std::int32_t copyWidth = std::min(xEnd, fileWidth) - sourceX;
    const std::int32_t copyHeight = std::min(yEnd, fileHeight) - sourceY;
    const std::int32_t destinationStart =
        destinationX + destinationWidth * destinationY;
    if (copyHeight > 0) {
        std::uint16_t* const planes[] {tile, obj, hit};
        const std::int32_t planeBytes = 2 * fileWidth * fileHeight;
        for (std::int32_t plane = 0; plane < 3; ++plane) {
            for (std::int32_t row = 0; row < copyHeight; ++row) {
                const std::int32_t fileRow =
                    20 + plane * planeBytes
                    + 2 * ((sourceY + row) * fileWidth + sourceX);
                ::_fseeki64(file, static_cast<std::int64_t>(static_cast<std::uint32_t>(fileRow)), SEEK_SET);
                const std::int32_t destinationRow =
                    destinationStart + row * destinationWidth;
                static_cast<std::uint32_t>(std::fread(
                    planes[plane] + destinationRow,
                    static_cast<std::uint32_t>(2 * copyWidth), 1, file));
            }
        }
    }
    std::fclose(file);
    return 0;
}

// 原 0x9DAD5C/0x9DAD68/0x9DAD58/0x9DAD60/0x9DAD64；46DA80 的 W32 目标。
std::uint32_t gMapSceneResourceMapId_9DAD5C = 0; // 原全局：0x009DAD5C。

} // namespace detail
std::uint32_t gMapResourceMode_9DAD68 = 0; // 原全局：0x009DAD68。

namespace detail {
std::int32_t gMapSceneResourceFloor_9DAD58 = 0; // 原全局：0x009DAD58。
std::int32_t gMapSceneSelectedResourceSnapshot_9DAD60 = 0; // 原全局：0x009DAD60。
std::int32_t gMapSceneSelectedResourceMirror_9DAD64 = 0; // 原全局：0x009DAD64。

// 原 [0x9FB948,0xAEFB94)；供 .cut 重置、加载及后续 46CF60 共享。
CutData9FB948 gCutData9FB948 {}; // 原全局：0x009FB948。

} // namespace detail

// 原 [0xAF10D8,0xB65A7B)；.aut 单次读取的唯一共享表。
AutRecordAF10D8 gAutDataAF10D8[159201] {}; // 原全局：0x00AF10D8。

namespace detail {
static_assert(sizeof(gAutDataAF10D8) == 477603);

// 原 [0xC35010, 0xC75010)；尾地址的 gMapProjectionOffsetY_C75010 是独立标量。
std::int32_t gValuesC35010[65536] {}; // 原全局：0x00C35010。
static_assert(sizeof(gValuesC35010) == 0x40000);

std::uint32_t gAutoMapWindowDirty_55DA88 = 1; // 原全局：0x0055DA88。

// 原 55DA90/55DA94：保留可写索引及三组 S32 尺寸，不冻结为当前 54x54。
static std::int16_t gAutoMapWindowIndex55DA90 = 1; // 原全局：0x0055DA90。
struct AutoMapWindowSize {
    std::int32_t width;
    std::int32_t height;
};
static AutoMapWindowSize gAutoMapWindowSizes55DA94[3] {{107, 107}, {54, 54}, {27, 27}}; // 原全局：0x0055DA94。

// 原 46C840 的清零字节数；与 51x51 世界地图缓存独立。
static constexpr std::size_t kAutoMapWindowCells = 11664;
static std::uint32_t gAutoMapColorWindow9CF6EC[kAutoMapWindowCells] {}; // 原全局：0x009CF6EC。
static std::uint8_t gAutoMapIndexWindow9DAD7C[kAutoMapWindowCells] {}; // 原全局：0x009DAD7C。
static_assert(sizeof(AutoMapWindowSize) == 8);
static_assert(sizeof(gAutoMapColorWindow9CF6EC) == 46656);
static_assert(sizeof(gAutoMapIndexWindow9DAD7C) == 11664);

// R09：解码后的新图号沿BLM01同表别名取色，不另建newMapRes。
static std::uint8_t GetAutoMapColorIndex(const std::int32_t mapNo)
{
    if (mapNo >= 220000 && mapNo <= 265535) {
        return resource::gAutoMapColors927F48[mapNo - 200000];
    }
    if (mapNo <= 0 || mapNo >= static_cast<std::int32_t>(resource::AutoMapColorCapacity)) {
        return 0;
    }
    return resource::gAutoMapColors927F48[mapNo];
}

// 原函数：0x0046C840。
std::int32_t LoadAutoMapWindow_46C840(
    const std::int32_t floor, const std::int32_t playerX, const std::int32_t playerY)
{
    const std::int32_t mapId = std::bit_cast<std::int32_t>(gCurrentMapId_9A86DC);
    std::memset(gAutoMapIndexWindow9DAD7C, 0, sizeof(gAutoMapIndexWindow9DAD7C));
    std::memset(gAutoMapColorWindow9CF6EC, 0, sizeof(gAutoMapColorWindow9CF6EC));
    char path[256];
    FormatMapDataPath_468520(path, mapId, gMapCachePathKey_9E0434, floor);
    std::FILE* const file =
        std::fopen(path, kMapReadMode);
    if (file == nullptr) {
        return 0;
    }

    std::array<std::uint16_t, kAutoMapWindowCells> tile {};
    std::array<std::uint16_t, kAutoMapWindowCells> object {};
    std::array<std::uint16_t, kAutoMapWindowCells> hit {};
    ::_fseeki64(file, 12, SEEK_SET);
    // 原头读取不检查返回，未写字节不套用其他地图函数的补零特例。
    std::int32_t fileWidth;
    std::int32_t fileHeight;
    static_cast<std::uint32_t>(std::fread(&fileWidth, 4, 1, file));
    static_cast<std::uint32_t>(std::fread(&fileHeight, 4, 1, file));

    const std::int32_t sizeIndex = gAutoMapWindowIndex55DA90;
    const std::int32_t windowWidth = gAutoMapWindowSizes55DA94[sizeIndex].width;
    const std::int32_t windowHeight = gAutoMapWindowSizes55DA94[sizeIndex].height;
    const std::int32_t startX = playerX - windowWidth / 2;
    const std::int32_t startY = playerY - windowHeight / 2;
    const std::int32_t endX = startX + windowWidth;
    const std::int32_t endY = startY + windowHeight;
    std::int32_t sourceX = startX;
    std::int32_t sourceY = startY;
    std::int32_t copyWidth = windowWidth;
    std::int32_t copyHeight = windowHeight;
    std::int32_t destinationX = 0;
    std::int32_t destinationY = 0;
    if (startX < 0) {
        copyWidth += startX;
        sourceX = 0;
        destinationX = -startX;
    }
    if (endX > fileWidth) {
        copyWidth += fileWidth - endX;
    }
    if (startY < 0) {
        sourceY = 0;
        copyHeight += startY;
        destinationY = -startY;
    }
    if (endY > fileHeight) {
        copyHeight += fileHeight - endY;
    }
    const std::int32_t destinationStart = destinationX + windowWidth * destinationY;
    std::uint16_t* const planes[] {tile.data(), object.data(), hit.data()};
    std::int32_t planeOffset = 20;
    for (std::int32_t plane = 0; plane < 3; ++plane) {
        std::int32_t sourceCell = sourceX + fileWidth * sourceY;
        if (copyHeight > 0) {
            std::uint16_t* target = planes[plane] + destinationStart;
            for (std::int32_t row = 0; row < copyHeight; ++row) {
                ::_fseeki64(file,
                    static_cast<std::int64_t>(static_cast<std::uint32_t>(planeOffset + 2 * sourceCell)), SEEK_SET);
                static_cast<std::uint32_t>(std::fread(target,
                    static_cast<std::uint32_t>(2 * copyWidth), 1, file));
                sourceCell += fileWidth;
                target += windowWidth;
            }
        }
        if (plane < 2) {
            planeOffset += 2 * fileWidth * fileHeight;
        }
    }
    std::fclose(file);

    if (gMapResourceMode_9DAD68 == 1) {
        const std::int32_t index = gAutoMapWindowIndex55DA90;
        const std::int32_t height = gAutoMapWindowSizes55DA94[index].height;
        if (height > 0) {
            const std::int32_t width = gAutoMapWindowSizes55DA94[index].width;
            std::int32_t autRow = startX + fileWidth * startY;
            const std::uint16_t* flags = hit.data();
            std::uint32_t* output = gAutoMapColorWindow9CF6EC;
            for (std::int32_t row = 0; row < height; ++row) {
                for (std::int32_t column = 0; column < width; ++column) {
                    if ((flags[column] & 0x4000u) != 0) {
                        const std::int32_t mapNo = ResolveMapGraphicNumber_46CF00(tile[row * width + column]);
                        const std::uint8_t colorIndex = mapNo >= 220000 && mapNo <= 265535
                            ? GetAutoMapColorIndex(mapNo) : 0;
                        const std::uint32_t paletteColor = render::GetAutoMapPaletteRgb(colorIndex);
                        if (paletteColor != 0) {
                            output[column] = paletteColor;
                        } else {
                            const AutRecordAF10D8& color = gAutDataAF10D8[autRow + column];
                            const std::uint32_t blue = color.value02;
                            const std::uint32_t green = color.value01;
                            const std::uint32_t red = color.value00;
                            output[column] = (red << 16) | (green << 8) | blue;
                        }
                    }
                }
                output += width;
                flags += width;
                autRow += fileWidth;
            }
        }
        return 1;
    }

    std::int16_t nextIndex = gAutoMapWindowIndex55DA90;
    std::int32_t currentIndex = nextIndex;
    const std::int32_t tileHeight = gAutoMapWindowSizes55DA94[currentIndex].height;
    if (tileHeight > 0) {
        const std::int32_t width = gAutoMapWindowSizes55DA94[currentIndex].width;
        std::int32_t cell = 0;
        for (std::int32_t row = 0; row < tileHeight; ++row) {
            for (std::int32_t column = 0; column < width; ++column) {
                if ((hit[cell + column] & 0x4000u) != 0) {
                    gAutoMapIndexWindow9DAD7C[cell + column] =
                        GetAutoMapColorIndex(ResolveMapGraphicNumber_46CF00(tile[cell + column]));
                }
            }
            cell += width;
        }
        nextIndex = gAutoMapWindowIndex55DA90;
    }

    std::int32_t row = 0;
    std::int32_t height = gAutoMapWindowSizes55DA94[currentIndex].height;
    while (row < height) {
        std::int32_t width = gAutoMapWindowSizes55DA94[currentIndex].width;
        std::int32_t column = 0;
        while (column < width) {
            const std::int32_t cell = row * width + column;
            if ((hit[cell] & 0x4000u) != 0) {
                const std::int32_t mapNo = ResolveMapGraphicNumber_46CF00(object[cell]);
                const std::uint8_t color = GetAutoMapColorIndex(mapNo);
                if (color != 0) {
                    std::int32_t graphicIndex;
                    resource::GetGraphicMapNoIndex(mapNo, &graphicIndex);
                    std::uint16_t flag;
                    resource::GetGraphicFlag1(std::bit_cast<std::uint32_t>(graphicIndex), &flag);
                    if (flag != 0) {
                        const std::int32_t currentWidth =
                            gAutoMapWindowSizes55DA94[gAutoMapWindowIndex55DA90].width;
                        gAutoMapIndexWindow9DAD7C[row * currentWidth + column] = color;
                    } else {
                        std::uint16_t sx;
                        std::uint16_t sy;
                        resource::GetGraphicSxSy(std::bit_cast<std::uint32_t>(graphicIndex), &sx, &sy);
                        const std::int32_t spanY = std::bit_cast<std::int16_t>(sy);
                        if (spanY > 0) {
                            const std::int32_t spanX = std::bit_cast<std::int16_t>(sx);
                            std::int32_t targetRow = row;
                            for (std::int32_t offsetY = 0; offsetY < spanY; ++offsetY) {
                                for (std::int32_t offsetX = 0; offsetX < spanX; ++offsetX) {
                                    if (targetRow >= 0) {
                                        const std::int32_t currentWidth =
                                            gAutoMapWindowSizes55DA94[gAutoMapWindowIndex55DA90].width;
                                        if (column + offsetX < currentWidth) {
                                            gAutoMapIndexWindow9DAD7C[
                                                targetRow * currentWidth + column + offsetX] = color;
                                        }
                                    }
                                }
                                --targetRow;
                            }
                        }
                    }
                }
            }
            nextIndex = gAutoMapWindowIndex55DA90;
            ++column;
            width = gAutoMapWindowSizes55DA94[nextIndex].width;
        }
        currentIndex = nextIndex;
        ++row;
        height = gAutoMapWindowSizes55DA94[currentIndex].height;
    }
    return 1;
}

// 原 [0x46DC60,0x46DC85)；整块清零完成后才逐项写 +4 哨兵。
void __cdecl ResetCutMapData_46DC60()
{
    std::memset(&gCutData9FB948, 0, sizeof(gCutData9FB948));
    for (CutRecord9FB954& record : gCutData9FB948.records) {
        record.value04 = -1;
    }
}

// 原 [0x46DB00,0x46DC55)；保留短读后继续、失败消息及完整返回值。
std::int32_t __cdecl LoadCutMapData_46DB00(std::int32_t selected)
{
    ResetCutMapData_46DC60();
    char path[256];
    std::sprintf(
        path, "bin\\PUK2\\cutdat\\%d.cut" /* 原 0x55DE50/0x554920；.cut 文件使用当前目录下的原相对路径。 */ /* 原常量参考：0x0055DE50。 */, selected);
    std::FILE* const file =
        std::fopen(path, kCutReadMode554920);
    if (file == nullptr) {
        platform::ShowWideMessage(L"cut地图档开启失败。" /* 原提示当前采用GBK窄字节；两个正文与各自标题按调用顺序配对。 */ /* 原常量参考：0x0055DE3C。 */, kCutCaption5536B8, 0x10);
        const std::int32_t response =
            platform::ShowWideMessage(kCutMessage55DE20, kCutCaption55DE30, 0x11);
        if (response != 1) {
            return response;
        }
        return ::PostMessageW(reinterpret_cast<HWND>(platform::gLegacyWindow), 0x10, 0, 0);
    }

    static_cast<std::uint32_t>(std::fread(&gCutData9FB948, 12, 1, file));
    const std::int32_t count0A = static_cast<std::int32_t>(gCutData9FB948.count0A);
    const std::int32_t count08 = static_cast<std::int32_t>(gCutData9FB948.count08);
    const std::int32_t count06 = static_cast<std::int32_t>(gCutData9FB948.count06);
    const std::int32_t count = count0A + count08 + count06;
    for (std::int32_t index = 0; index < count; ++index) {
        CutRecord9FB954& record = gCutData9FB948.records[index];
        static_cast<std::uint32_t>(std::fread(&record.value00, 4, 1, file));
        static_cast<std::uint32_t>(std::fread(&record.value04, 2, 1, file));
        static_cast<std::uint32_t>(std::fread(&record.value06, 2, 1, file));
        static_cast<std::uint32_t>(std::fread(&record.value08, 2, 1, file));
        static_cast<std::uint32_t>(std::fread(&record.value0A, 2, 1, file));
        static_cast<std::uint32_t>(std::fread(&record.value0C, 2, 1, file));
        static_cast<std::uint32_t>(std::fread(&record.value0E, 2, 1, file));
        static_cast<std::uint32_t>(std::fread(&record.value10, 2, 1, file));
    }
    return std::fclose(file);
}

// 原 [0x46DC90,0x46DD35)；失败保留表，成功单次读取并返回完整关闭结果。
std::int32_t __cdecl LoadAutoMapColorData_46DC90(std::int32_t selected)
{
    char path[256];
    std::sprintf(
        path, "bin\\PUK2\\cutdat\\%d.aut" /* 原 0x55DE7C/0x55DE68；.aut 相对路径与打开失败的第一正文。 */ /* 原常量参考：0x0055DE7C。 */, selected);
    std::FILE* const file =
        std::fopen(path, kCutReadMode554920);
    if (file == nullptr) {
        platform::ShowWideMessage(L"aut地图档开启失败。" /* 原常量参考：0x0055DE68。 */, kCutCaption5536B8, 0x10);
        const std::int32_t response =
            platform::ShowWideMessage(kCutMessage55DE20, kCutCaption55DE30, 0x11);
        if (response != 1) {
            return response;
        }
        return ::PostMessageW(reinterpret_cast<HWND>(platform::gLegacyWindow), 0x10, 0, 0);
    }

    static_cast<std::uint32_t>(std::fread(gAutDataAF10D8, 1, 477603, file));
    return std::fclose(file);
}

// 原 [0x468200,0x46828D)：四次独立读取玩家坐标，裁剪后发布两个跨度。
std::uint16_t __cdecl UpdatePlayerMapWindow_468200()
{
    gMapXStart = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            (gPlayerX + -30)));
    gMapYStart = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            (gPlayerY + -20)));
    gMapXEnd = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            (gPlayerX + 21)));
    gMapYEnd = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(
            (gPlayerY + 31)));

    ClipMapRectangleToBounds_468290(&gMapXStart, &gMapYStart, &gMapXEnd, &gMapYEnd);
    const std::uint16_t xEnd = static_cast<std::uint16_t>(gMapXEnd);
    const std::uint16_t yEnd = static_cast<std::uint16_t>(gMapYEnd);
    const std::uint16_t spanX =
        xEnd - static_cast<std::uint16_t>(gMapXStart);
    const std::uint16_t spanY =
        yEnd - static_cast<std::uint16_t>(gMapYStart);
    gMapSpanX = std::bit_cast<std::int16_t>(spanX);
    gMapSpanY = std::bit_cast<std::int16_t>(spanY);
    return spanX;
}

// 原 [0x46CEC0,0x46CED6)：Y 的读取先于 X 的带参读取。
std::int32_t __cdecl GetPlayerMapCoordinateSum_46CEC0()
{
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    return (gPlayerX + static_cast<std::int32_t>(y));
}

// 原 [0x46CEE0,0x46CEFB)：同一读序，结果写入独立的 AF10C0。
std::int32_t __cdecl StorePlayerMapCoordinateSum_46CEE0()
{
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t result =
        (gPlayerX + static_cast<std::int32_t>(y));
    gPlayerMapCoordinateSum_AF10C0 = std::bit_cast<std::uint32_t>(result);
    return result;
}

// 原 [0x468330,0x468511)：保留六次临时构造/析构及快照与 F32 写入交错。
std::uint32_t __cdecl SetPlayerMapPosition_468330(const std::int32_t x, const std::int32_t y)
{
    static_cast<void>(GetPlayerMapCoordinateSum_46CEC0());

    gPlayerX = static_cast<std::int32_t>(x);

    gPlayerY = static_cast<std::int32_t>(y);

    static_cast<void>(StorePlayerMapCoordinateSum_46CEE0());
    const double currentX =
        static_cast<std::int32_t>(gPlayerX);
    gWorldCurrentXBits = std::bit_cast<std::uint32_t>(
        static_cast<float>(currentX * 64.0));
    const double currentY =
        static_cast<std::int32_t>(gPlayerY);
    const std::uint8_t snapshotXCount = gPlayerX.readCount;
    const std::uint32_t snapshotXEncoded = gPlayerX.encoded;
    const double scaledY = currentY * 64.0;
    const std::uint32_t snapshotXMask = gPlayerX.mask;
    gPlayerSnapshotX.readCount = snapshotXCount;
    const std::uint8_t snapshotYCount = gPlayerY.readCount;
    gPlayerSnapshotX.encoded = snapshotXEncoded;
    const std::uint32_t snapshotYEncoded = gPlayerY.encoded;
    gPlayerSnapshotX.mask = snapshotXMask;
    const std::uint32_t snapshotYMask = gPlayerY.mask;
    gPlayerSnapshotY.readCount = snapshotYCount;
    gWorldCurrentYBits = std::bit_cast<std::uint32_t>(
        static_cast<float>(scaledY));
    gPlayerSnapshotY.encoded = snapshotYEncoded;
    gPlayerSnapshotY.mask = snapshotYMask;

    gPlayerMovementStepXBits_9EE8FC = 0;
    gPlayerMovementStepYBits_9EE900 = 0;
    gPartyCatchUpRateBits_9DAD78 = 0x3F800000u;

    gLastMapDrawPlayerX_9EE8D0 = std::int32_t{-1};

    gLastMapDrawPlayerY_9EE8E0 = std::int32_t{-1};

    gValueC27FF8 = std::int32_t{-1};

    gValueC27FE8 = std::int32_t{-1};

    const std::uint32_t xBits = gWorldCurrentXBits;
    const std::uint32_t finalYBits = gWorldCurrentYBits;
    gWorldProjectionReferenceXBits = xBits;
    gWorldProjectionReferenceYBits = finalYBits;
    return finalYBits;
}

// 原 [0x467F30,0x4681AB)：八次独立临时构造，末尾透传表的一过尾指针。
std::int32_t* InitializeWorldMapState_467F30()
{
    SetWorldMapMetadata_4682E0(0, 0, 0, 0, 0, 0, 0, 0);
    static_cast<void>(SetPlayerMapPosition_468330(0, 0));

    std::optional<platform::LegacyCryptInt> first;
    first.emplace(static_cast<std::int32_t>(0));
    const std::uint32_t firstMask = first->mask;
    const std::uint32_t firstEncoded = first->encoded;
    const std::uint8_t firstReadCount = first->readCount;
    gLastMapDrawPlayerX_9EE8D0.mask = firstMask;
    gLastMapDrawPlayerX_9EE8D0.encoded = firstEncoded;
    gLastMapDrawPlayerX_9EE8D0.readCount = firstReadCount;

    std::optional<platform::LegacyCryptInt> second;
    gLastMapDrawPlayerY_9EE8E0 = std::int32_t{0};
    first.reset();

    second.emplace(static_cast<std::int32_t>(0));
    const std::uint32_t thirdMask = second->mask;
    const std::uint32_t thirdEncoded = second->encoded;
    const std::uint8_t thirdReadCount = second->readCount;
    gValueC27FF8.mask = thirdMask;
    gValueC27FF8.encoded = thirdEncoded;
    gValueC27FF8.readCount = thirdReadCount;

    gValueC27FE8 = std::int32_t{0};
    second.reset();

    gValue9A4B94 = 3;
    static_cast<void>(UpdatePlayerMapWindow_468200());
    gMovePathCount_C82200 = 0;
    gPendingDirectionCount_C82202 = 0;
    gPlayerPathRebuildPending_C821FC = 0;
    gWaitingForMapRectangleData_9EE8B8 = 0;
    gContinuousMapMovementActive_C8A79C = 0;

    gValueC821E8 = std::int32_t{0};

    gValue9E03C0 = std::int32_t{0};

    gPlayerCoordinateSyncPending_C821D8 = std::int32_t{0};

    // 原末次改用独立var_1C，保留第三临时对象。
    cg::world::gRippleGate = std::int32_t{0};

    gMapTransitionRequestPending_9A86D8 = 0;
    gDirectionalMapTransitionPending_9FB92E = 0;
    gValueB65A7C = 0;
    gMapMovementInputBlocked_C82220 = 0;
    gMapLeftHoldDurationMs_C8A778 = 0;
    gMapLeftPressTick_C8A794 = 0;
    gMapTransitionObservationPrepared_C8220C = 0;
    gMapTransitionReplyPending_C82210 = 0;
    gMapTransitionAwaitingRectangle_C82214 = 0;
    gMapNameReceivedDuringTransition_C82218 = 0;
    gValueC82224 = 0;
    gPendingMapTransitionSequence_9DAD54 = 0xFFFFu;
    gPendingDirectionalTransitionSequence_9FB92C = 0xFFFFu;
    gPlayerPathFinalFacingDirection_55D9FC = 0xFFFFu;

    static_cast<void>(ResetMapProjectionState_468F30());
    gValueC821D0 = 0;
    gMapWindowReloadPending_9EE910 = 0;
    return InitializeMapGraphicNumberTable_46CF20();
}

// 原 [0x468290,0x4682D7)：四个 S16 原地裁剪，末参数指针透传。
std::int16_t* __cdecl ClipMapRectangleToBounds_468290(
    std::int16_t* xStart,
    std::int16_t* yStart,
    std::int16_t* xEnd,
    std::int16_t* yEnd)
{
    if (*xStart < 0) {
        *xStart = 0;
    }
    if (*yStart < 0) {
        *yStart = 0;
    }
    const std::uint32_t xBound = gMapWidth_9FB930;
    if (*xEnd > std::bit_cast<std::int32_t>(xBound)) {
        *xEnd = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(xBound));
    }
    const std::uint32_t yBound = gMapHeight_C821D4;
    if (*yEnd > std::bit_cast<std::int32_t>(yBound)) {
        *yEnd = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(yBound));
    }
    return yEnd;
}

// 原 [0x468C00,0x468C32)：独立重读窗口状态，写完整 WORD 并返回左边界。
std::int32_t __cdecl SetMapCellHitValue_468C00(
    const std::int32_t x, const std::int32_t y, const std::uint16_t value)
{
    const std::int32_t top = gMapYStart;
    const std::int32_t span = gMapSpanX;
    const std::int32_t left = gMapXStart;
    const std::int32_t index = x - left + span * (y - top);
    gMapHitArray[static_cast<std::size_t>(index)] = value;
    return left;
}

// MHT01：原 [0x46AE60,0x46AE94)；Y/跨度取样后求 Y 差再取 X，原低 BYTE 比较返完整 0/1。
std::int32_t __cdecl IsDirectionalMapTransitionCell_46AE60(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t top = gMapYStart;
    const std::int32_t span = gMapSpanX;
    const std::int32_t localY = y - top;
    const std::int32_t left = gMapXStart;
    const std::int32_t index = span * localY - left + x;
    return static_cast<std::uint8_t>(gMapHitArray[static_cast<std::size_t>(index)]) == 2;
}

// 原 [0x469240,0x469284)：按字节读取，两张原表查值并保留低 W16 状态。
std::int32_t __cdecl CalculateMapDataCrc_469240(
    const std::uint8_t* const bytes, const std::int32_t length)
{
    std::uint16_t state = 0;
    for (std::int32_t i = 0; i < length; ++i) {
        std::uint16_t next = kMapBitReverse55DAAC[bytes[i]];
        next ^= kMapCrcTable55DBAC[state >> 8];
        next ^= static_cast<std::uint16_t>((state & 0xFFu) << 8);
        state = next;
    }
    return state;
}

// 原 [0x469290,0x4693E9)：装载失败仍校验局部平面并可能发送 UUN。
std::int32_t __cdecl CheckMapRectangleCrcAndRequest_469290(
    const std::int32_t first, const std::int32_t floor,
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    const std::int32_t expectedTile, const std::int32_t expectedObj,
    const std::int32_t expectedHit)
{
    std::array<std::uint16_t, kMapBufferCells + 1> tile {};
    std::array<std::uint16_t, kMapBufferCells + 1> obj {};
    std::array<std::uint16_t, kMapBufferCells + 1> hit {};
    const std::int32_t width = xEnd - xStart;
    const std::int32_t height = yEnd - yStart;
    static_cast<void>(ReadMapCacheRectangle_468C40(
        std::bit_cast<std::int32_t>(gCurrentMapId_9A86DC),
        floor, xStart, yStart, xEnd, yEnd,
        tile.data(), obj.data(), hit.data()));

    if (height > 0) {
        const std::uint16_t mask = gMapResourceMode_9DAD68 == 1 ? 0x01FFu : 0x0FFFu;
        std::int32_t rowStart = 0;
        for (std::int32_t row = 0; row < height; ++row) {
            for (std::int32_t column = 0; column < width; ++column) {
                hit[static_cast<std::size_t>(rowStart + column)] &= mask;
            }
            rowStart += width;
        }
    }

    constexpr std::int32_t kCheckedBytes = 0x5B2;
    const std::int32_t tileCheck = CalculateMapDataCrc_469240(
        reinterpret_cast<const std::uint8_t*>(tile.data()), kCheckedBytes);
    const std::int32_t objCheck = CalculateMapDataCrc_469240(
        reinterpret_cast<const std::uint8_t*>(obj.data()), kCheckedBytes);
    const std::int32_t hitCheck = CalculateMapDataCrc_469240(
        reinterpret_cast<const std::uint8_t*>(hit.data()), kCheckedBytes);
    if (expectedTile == tileCheck && expectedObj == objCheck
        && expectedHit == hitCheck) {
        return 1;
    }
    static_cast<void>(network::RequestMapRectangle_510710(
        platform::gConnectionSocket,
        first, floor, xStart, yStart, xEnd, yEnd));
    return 0;
}

// 原 [0x4691D0,0x469239)：两个 -1 临时对象各自构造、复制、析构。
void __cdecl InvalidateMapDrawPosition_4691D0()
{
    // 原第一组先读齐两个 W32 和 BYTE 计数，再依原序发布。
    // 现由整数赋值统一载荷发布：一次RNG，保留目标token与reserved。
    gLastMapDrawPlayerX_9EE8D0 = std::int32_t{-1};

    // 原第二组先读齐两个 W32 和 BYTE 计数，再依原序发布。
    // 现由整数赋值统一载荷发布：一次RNG，保留目标token与reserved。
    gLastMapDrawPlayerY_9EE8E0 = std::int32_t{-1};
}

// 原函数：0x00455C60。
void ResetTimedMapNotice_455C60()
{
    gMapNoticeStartTick_62B5B0 = 0;
}

// 原函数：0x00455FC0。
void SynchronizeTimedMapNoticeWindow_455FC0()
{
    const std::uint64_t value = gMapNoticeStartTick_62B5B0;
    gValue62B398 = 0;
    if (value == 0) {
        cg::yobi::YobiAction* const action = gWindow39Action62B5B8;
        if (action != nullptr) {
            static_cast<void>(cg::yobi::MarkDeath(action));
            gWindow39Action62B5B8 = nullptr;
        }
        return;
    }
    if (ui::detail::gWindowStates[cg::ui::WindowId_TimerNotice].window == nullptr
        && gWindow39Action62B5B8 == nullptr) {
        gWindow39Action62B5B8 = ui::WindowRuntime_CreateAnimation(
            39, 0, 0, &gWindow39Action62B5B8);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
    }
}

// 原函数：0x004ED670。
std::int32_t ShouldPlayBattleResultSound_4ED670()
{
    std::int32_t result = 0;
    for (const BattleResultRecord& record : gBattleResultsFF0C14) {
        if (record.value02 == 1) {
            result = 1;
        }
    }
    for (const BattleSkillResultRecord& record : gBattleSkillResultsFF0C8C) {
        if (record.value00 == 1) {
            result = 1;
        }
    }
    return result;
}

// 原函数：0x00504A50。
void OpenPendingBattleResultWindow_504A50()
{
    const std::int16_t mode = std::bit_cast<std::int16_t>(gPendingResultWindowMode_FF0C10);
    if (mode <= 0) {
        return;
    }
    if (mode == 1) {
        static_cast<void>(ui::WindowRuntime_CreateAnimation(cg::ui::WindowId_BattleResult, 1, 0, nullptr));
        if (ShouldPlayBattleResultSound_4ED670() != 0) {
            static_cast<void>(audio::PlaySe_52C450(64, 320));
        }
        gValueFF0D44 = 0;
        gPendingResultWindowMode_FF0C10 = 0;
    } else if (mode == 2) {
        static_cast<void>(ui::WindowRuntime_CreateAnimation(cg::ui::WindowId_DpResult, 1, 0, nullptr));
        gValueFF0D44 = 0;
        gPendingResultWindowMode_FF0C10 = 0;
    } else {
        gPendingResultWindowMode_FF0C10 = 0;
    }
}

// 原[51C7D0,51D447)完整22状态；共享尾保持原同帧跳转，见场景明细normal-scene-complete。
void DrawNormalScene_51C7D0()
{
    switch (gCurrentSceneState) {
    case 0:
        gCurrentSceneState = 100;
        gValueFF0D44 = 0;
        gWorldSceneInitialized_FF0D50 = 0;
        gValue5ABFC4 = 0;
        [[fallthrough]];
    case 100:
        CreatePlayerMapPresence_519A80();
        ResetTimedMapNotice_455C60();
        RecreateMissingWorldObjectActions_446640();
        ResetCurrentDialogType_4D3170();
        ui::WindowRuntime_ProcessScene();
        cg::yobi::ResetObservationGroupList_43F220();
        static_cast<void>(UpdateUnreadMailPageHint_518DA0());
        input::SetContext_44B330(&cg::ui::detail::gInputContext626AA0);
        gCurrentSceneState = 101;
        [[fallthrough]];
    case 101:
        if (gValue9A4B94 != 0) {
            platform::gValue73ECC0 = 0;
            return;
        }
        [[fallthrough]];
    case 102:
        gWorldSceneWaitFrames_FF0D54 = 0;
        gCurrentSceneState = 104;
        return;
    case 104: {
        ApplyPendingMapPaletteReply_51D6F0();
        UpdateDayPeriodPaletteAndTimers_458F00();
        if (gWorldSceneWaitFrames_FF0D54 < 5) {
            goto waitFrame;
        }
        platform::gPendingGraphicCount = 0;
        platform::gPendingGraphicCount2 = 0;
        static_cast<void>(render::ResetGraphicCache_456D30());
        ui::detail::gValue5F9754 = 0;
        ResetMapDrawCacheCounts_46BD40();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        RenderChara_521F20();
        audio::ResetBgmPlayback_52B940();
        DrawMapSceneWithMarkers_469020();
        static_cast<void>(ui::WindowRuntime_Render());
        input::ProcessKeyInput_4596D0();
        input::BrowseChatHistory_44A1B0();
        RenderInput_4590E0();
        render::SortTextures_520790();
        static_cast<void>(render::ClearSurface_4512C0());
        render::gRenderQueueMode103CF10 = 0;
        render::DrawAllQueued_520DD0();
        static_cast<void>(render::CopyBackToTemp());
        platform::gPendingGraphicCount = 0;
        render::gRenderQueueMode103CF10 = 1;
        platform::gPendingGraphicCount2 = 0;
        static_cast<void>(render::ResetGraphicCache_456D30());
        if (gMapBgmChangePending_FF0D58 != 0) {
            static_cast<void>(audio::SelectBgmTarget_52C9C0(std::bit_cast<std::int32_t>(gPendingMapBgmIndex_FF0D5C)));
            static_cast<void>(login::detail::CaptureCharacterBgmTarget_520750(login::gSelectedCharacterIndex));
            static_cast<void>(login::detail::SaveCurrentLoginRecord_520200());
            static_cast<void>(cg::audio::LoadAndPlayBGM(static_cast<std::uint32_t>(audio::gBgmTarget1050818)));
            gWorldBgmResumeIndex_FF0D68 = audio::gBgmTarget1050818;
            gMapBgmChangePending_FF0D58 = 0;
            audio::gBgmRequest1050810 = 1;
        } else {
            static_cast<void>(cg::audio::LoadAndPlayBGM(static_cast<std::uint32_t>(audio::gBgmTarget1050818)));
        }
        const std::uint64_t tick = platform::ReadTickCount64();
        platform::gValue73ECC0 = 2;
        platform::gLastRenderDt = static_cast<double>(tick);
        gSceneTransitionResetPending_58B6D0 = 1;
        static_cast<void>(ProcessSceneTransitionEffect_51FDE0(15, 8, 0x40000000u));
        gCurrentSceneState = 103;
        ApplyPendingMapPaletteReply_51D6F0();
        return;
    }
    case 103:
        platform::gValue73ECC0 = 2;
        if (ProcessSceneTransitionEffect_51FDE0(15, 8, 0x40000000u) == 1) {
            gWorldSceneInitialized_FF0D50 = 1;
            gMapNoticeStartTick_62B5B0 = platform::ReadTickCount64();
            gCurrentSceneState = 20;
        }
        return;
    case 200:
        gSceneTransitionResetPending_58B6D0 = 1;
        static_cast<void>(CaptureSceneAndSelectEvenVariant_51D750());
        platform::gLastRenderDt = static_cast<double>(platform::ReadTickCount64());
        ui::WindowRuntime_ProcessScene();
        gCurrentSceneState = 201;
        [[fallthrough]];
    case 201: {
        const std::uint32_t gate = gValueD67E78;
        platform::gValue73ECC0 = 2;
        const std::int32_t selector = gate == 1 ? 14 : 16;
        const std::int32_t steps = gate == 1 ? 8 : 32;
        if (ProcessSceneTransitionEffect_51FDE0(selector, steps, 0x40000000u) == 1) {
            gCurrentSceneState = 202;
        }
        goto inputTail;
    }
    case 202:
        if (gMapTransitionReplyPending_C82210 != 0) {
            platform::gValue73ECC0 = 0;
            static_cast<void>(ui::WindowRuntime_Render());
            RenderInput_4590E0();
            return;
        }
        gMapTransitionReplyPending_C82210 = 0;
        [[fallthrough]];
    case 203:
        gWorldSceneWaitFrames_FF0D54 = 0;
        gCurrentSceneState = 205;
        return;
    case 205: {
        ApplyPendingMapPaletteReply_51D6F0();
        UpdateDayPeriodPaletteAndTimers_458F00();
        if (gWorldSceneWaitFrames_FF0D54 < 5) {
            goto waitFrame;
        }
        render::ClearSurfaceTransformCache_44D490();
        ApplyPendingMapPaletteReply_51D6F0();
        static_cast<void>(render::UpdatePalette_450BB0());
        platform::gPendingGraphicCount = 0;
        platform::gPendingGraphicCount2 = 0;
        static_cast<void>(render::ResetGraphicCache_456D30());
        ResetMapDrawCacheCounts_46BD40();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        RenderChara_521F20();
        InvalidateMapDrawPosition_4691D0();
        DrawMapSceneWithMarkers_469020();
        cg::core::NullSub_509080();
        if ((gMapCharaSceneSlots.valueC8A73E == 0 && gMapCharaSceneSlots.valueC8A740 != 0)
            || (static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744) == 0
                && static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744 >> 16) != 0)) {
            InitializeMapEffectPool_46E2A0();
        }
        gMapCharaSceneSlots.valueC8A75C = 0;
        gMapCharaSceneSlots.valueC8A758 = 0;
        input::ProcessKeyInput_4596D0();
        input::BrowseChatHistory_44A1B0();
        render::SortTextures_520790();
        static_cast<void>(render::ClearSurface_4512C0());
        ClearDirectionalMapEffects_46F160();
        if ((gMapCharaSceneSlots.valueC8A73E != 0 && gMapCharaSceneSlots.valueC8A740 == 0)
            || (static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744) != 0
                && static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744 >> 16) == 0)) {
            static_cast<void>(AdvanceMapParticleCycles_46E4C0(80));
        }
        render::gRenderQueueMode103CF10 = 0;
        render::DrawAllQueued_520DD0();
        static_cast<void>(render::CopyBackToTemp());
        render::gRenderQueueMode103CF10 = 1;
        platform::gPendingGraphicCount = 0;
        platform::gPendingGraphicCount2 = 0;
        static_cast<void>(render::ResetGraphicCache_456D30());
        const std::uint64_t tick = platform::ReadTickCount64();
        gSceneTransitionResetPending_58B6D0 = 1;
        gCurrentSceneState = 204;
        platform::gLastRenderDt = static_cast<double>(tick);
        ApplyPendingMapPaletteReply_51D6F0();
        [[fallthrough]];
    }
    case 204: {
        const std::uint32_t gate = gValueD67E78;
        platform::gValue73ECC0 = 2;
        const std::int32_t selector = gate == 1 ? 15 : 17;
        const std::int32_t steps = gate == 1 ? 8 : 32;
        if (ProcessSceneTransitionEffect_51FDE0(selector, steps, 0x40000000u) == 1) {
            ui::WindowRuntime_ProcessScene();
            gMapNoticeStartTick_62B5B0 = platform::ReadTickCount64();
            const std::uint32_t changeBgm = gMapBgmChangePending_FF0D58;
            gCurrentSceneState = 20;
            gValueD67E78 = 0;
            if (changeBgm != 0) {
                static_cast<void>(audio::SelectBgmTarget_52C9C0(std::bit_cast<std::int32_t>(gPendingMapBgmIndex_FF0D5C)));
                static_cast<void>(login::detail::CaptureCharacterBgmTarget_520750(login::gSelectedCharacterIndex));
                static_cast<void>(login::detail::SaveCurrentLoginRecord_520200());
                static_cast<void>(cg::audio::LoadAndPlayBGM(static_cast<std::uint32_t>(audio::gBgmTarget1050818)));
                gWorldBgmResumeIndex_FF0D68 = audio::gBgmTarget1050818;
                gMapBgmChangePending_FF0D58 = 0;
                audio::gBgmRequest1050810 = 1;
            }
            static_cast<void>(ui::WindowRuntime_Render());
        }
        goto inputTail;
    }
    case 250: {
        ui::WindowRuntime_ProcessScene();
        ClearDirectionalMapEffects_46F160();
        if ((gMapCharaSceneSlots.valueC8A73E != 0 && gMapCharaSceneSlots.valueC8A740 == 0)
            || (static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744) != 0
                && static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744 >> 16) == 0)) {
            static_cast<void>(AdvanceMapParticleCycles_46E4C0(80));
        }
        render::gRenderQueueMode103CF10 = 0;
        render::DrawAllQueued_520DD0();
        platform::gPendingGraphicCount = 0;
        render::gRenderQueueMode103CF10 = 1;
        platform::gPendingGraphicCount2 = 0;
        static_cast<void>(render::ResetGraphicCache_456D30());
        const std::uint64_t tick = platform::ReadTickCount64();
        gSceneTransitionResetPending_58B6D0 = 1;
        gCurrentSceneState = 204;
        platform::gLastRenderDt = static_cast<double>(tick);
        ApplyPendingMapPaletteReply_51D6F0();
        const std::int32_t bgm = cg::yobi::BeginObservationTravelScene_440FE0();
        audio::gBgmTarget1050818 = bgm;
        static_cast<void>(cg::audio::LoadAndPlayBGM(static_cast<std::uint32_t>(bgm)));
        gWorldBgmResumeIndex_FF0D68 = audio::gBgmTarget1050818;
        gCurrentSceneState = 251;
        [[fallthrough]];
    }
    case 251:
        platform::gValue73ECC0 = 0;
        static_cast<void>(DrawObservationTravelBackground_440FF0());
        ApplyPendingMapPaletteReply_51D6F0();
        static_cast<void>(NextRotationValue_455C70());
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        RenderChara_521F20();
        cg::core::NullSub_509080();
        input::ProcessKeyInput_4596D0();
        input::BrowseChatHistory_44A1B0();
        static_cast<void>(ui::WindowRuntime_Render());
        RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        UpdateDayPeriodPaletteAndTimers_458F00();
        return;
    case 252: {
        cg::core::NullSub_509080();
        gValueFF0D74 = 0;
        ui::WindowRuntime_ProcessScene();
        gMapNoticeStartTick_62B5B0 = platform::ReadTickCount64();
        ClearDirectionalMapEffects_46F160();
        if ((gMapCharaSceneSlots.valueC8A73E != 0 && gMapCharaSceneSlots.valueC8A740 == 0)
            || (static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744) != 0
                && static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744 >> 16) == 0)) {
            static_cast<void>(AdvanceMapParticleCycles_46E4C0(80));
        }
        ApplyPendingMapPaletteReply_51D6F0();
        ResetMapDrawCacheCounts_46BD40();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        RenderChara_521F20();
        InvalidateMapDrawPosition_4691D0();
        DrawMapSceneWithMarkers_469020();
        cg::core::NullSub_509080();
        static_cast<void>(ui::WindowRuntime_Render());
        render::gRenderQueueMode103CF10 = 0;
        render::DrawAllQueued_520DD0();
        render::gRenderQueueMode103CF10 = 1;
        const std::uint64_t tick = platform::ReadTickCount64();
        platform::gPendingGraphicCount = 0;
        platform::gPendingGraphicCount2 = 0;
        platform::gLastRenderDt = static_cast<double>(tick);
        static_cast<void>(render::ResetGraphicCache_456D30());
        ResetMapDrawCacheCounts_46BD40();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        RenderChara_521F20();
        DrawMapSceneWithMarkers_469020();
        cg::core::NullSub_509080();
        static_cast<void>(ui::WindowRuntime_Render());
        const std::uint32_t changeBgm = gMapBgmChangePending_FF0D58;
        gValueD67E78 = 0;
        if (changeBgm != 0) {
            static_cast<void>(audio::SelectBgmTarget_52C9C0(std::bit_cast<std::int32_t>(gPendingMapBgmIndex_FF0D5C)));
            static_cast<void>(login::detail::CaptureCharacterBgmTarget_520750(login::gSelectedCharacterIndex));
            static_cast<void>(login::detail::SaveCurrentLoginRecord_520200());
            static_cast<void>(cg::audio::LoadAndPlayBGM(static_cast<std::uint32_t>(audio::gBgmTarget1050818)));
            gWorldBgmResumeIndex_FF0D68 = audio::gBgmTarget1050818;
            gMapBgmChangePending_FF0D58 = 0;
            audio::gBgmRequest1050810 = 1;
        }
        gCurrentSceneState = 20;
        return;
    }
    case 1:
        gWorldSceneWaitFrames_FF0D54 = 0;
        gCurrentSceneState = 11;
        return;
    case 11: {
        ApplyPendingMapPaletteReply_51D6F0();
        UpdateDayPeriodPaletteAndTimers_458F00();
        if (gWorldSceneWaitFrames_FF0D54 < 30) {
            goto waitFrame;
        }
        gCurrentSceneState = 1;
        gValueFF0D44 = 1;
        CreatePlayerMapPresence_519A80();
        ResetTimedMapNotice_455C60();
        RecreateMissingWorldObjectActions_446640();
        ResetCurrentDialogType_4D3170();
        ui::WindowRuntime_ProcessScene();
        cg::yobi::ResetObservationGroupList_43F220();
        static_cast<void>(UpdateUnreadMailPageHint_518DA0());
        if (gWorldSceneInitialized_FF0D50 == 1) {
            platform::gPendingGraphicCount = 0;
            platform::gPendingGraphicCount2 = 0;
            static_cast<void>(render::ResetGraphicCache_456D30());
            ResetMapDrawCacheCounts_46BD40();
            static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
            RenderChara_521F20();
            audio::ResetBgmPlayback_52B940();
            static_cast<void>(UpdatePlayerMapWindow_468200());
            InvalidateMapDrawPosition_4691D0();
            DrawMapSceneWithMarkers_469020();
            if (audio::gBgmRequest1050810 == 0) {
                if (gMapBgmChangePending_FF0D58 != 0) {
                    static_cast<void>(audio::SelectBgmTarget_52C9C0(std::bit_cast<std::int32_t>(gPendingMapBgmIndex_FF0D5C)));
                    static_cast<void>(login::detail::CaptureCharacterBgmTarget_520750(login::gSelectedCharacterIndex));
                    static_cast<void>(login::detail::SaveCurrentLoginRecord_520200());
                    static_cast<void>(cg::audio::LoadAndPlayBGM(static_cast<std::uint32_t>(audio::gBgmTarget1050818)));
                    gWorldBgmResumeIndex_FF0D68 = audio::gBgmTarget1050818;
                    gMapBgmChangePending_FF0D58 = 0;
                    gPendingMapBgmIndex_FF0D5C = 0xFFFFFFFFu;
                } else {
                    const std::int32_t bgm = gWorldBgmResumeIndex_FF0D68;
                    audio::gBgmTarget1050818 = bgm;
                    static_cast<void>(cg::audio::LoadAndPlayBGM(static_cast<std::uint32_t>(bgm)));
                }
                audio::gBgmRequest1050810 = 1;
            }
            static_cast<void>(ui::WindowRuntime_Render());
            if ((gMapCharaSceneSlots.valueC8A73E == 0 && gMapCharaSceneSlots.valueC8A740 != 0)
                || (static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744) == 0
                    && static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744 >> 16) != 0)) {
                InitializeMapEffectPool_46E2A0();
            }
            gMapCharaSceneSlots.valueC8A75C = 0;
            gMapCharaSceneSlots.valueC8A758 = 0;
            input::ProcessKeyInput_4596D0();
            input::BrowseChatHistory_44A1B0();
            RenderInput_4590E0();
            render::SortTextures_520790();
            static_cast<void>(render::ClearSurface_4512C0());
            ClearDirectionalMapEffects_46F160();
            if ((gMapCharaSceneSlots.valueC8A73E != 0 && gMapCharaSceneSlots.valueC8A740 == 0)
                || (static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744) != 0
                    && static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744 >> 16) == 0)) {
                static_cast<void>(AdvanceMapParticleCycles_46E4C0(80));
            }
            ApplyPendingMapPaletteReply_51D6F0();
            static_cast<void>(render::UpdatePalette_450BB0());
            UpdateDayPeriodPaletteAndTimers_458F00();
            render::gRenderQueueMode103CF10 = 0;
            render::DrawAllQueued_520DD0();
            static_cast<void>(render::CopyBackToTemp());
            render::gRenderQueueMode103CF10 = 1;
            platform::gPendingGraphicCount = 0;
            platform::gPendingGraphicCount2 = 0;
            static_cast<void>(render::ResetGraphicCache_456D30());
            const std::uint64_t tick = platform::ReadTickCount64();
            const std::uint32_t gate = gValueD67E78;
            platform::gLastRenderDt = static_cast<double>(tick);
            const std::int32_t selector = gate == 1 ? 15 : 13;
            platform::gValue73ECC0 = 2;
            static_cast<void>(ProcessSceneTransitionEffect_51FDE0(selector, 8, 0x40000000u));
        } else {
            const std::int32_t state = gCurrentSceneState;
            gWorldSceneInitialized_FF0D50 = 1;
            gCurrentSceneState = state + 1;
        }
        input::ProcessKeyInput_4596D0();
        render::DrawInputText_4568A0(&cg::ui::detail::gInputContext626AA0);
        RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        ApplyPendingMapPaletteReply_51D6F0();
        ++gCurrentSceneState;
        return;
    }
    case 2: {
        const std::int32_t selector = gValueD67E78 == 1 ? 15 : 13;
        if (ProcessSceneTransitionEffect_51FDE0(selector, 8, 0x40000000u) == 1) {
            gCurrentSceneState = 20;
            gMapTransitionReplyPending_C82210 = 0;
            gValueD67E78 = 0;
        }
        input::ProcessKeyInput_4596D0();
        render::DrawInputText_4568A0(&cg::ui::detail::gInputContext626AA0);
        RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        ApplyPendingMapPaletteReply_51D6F0();
        return;
    }
    case 20:
        platform::gValue73ECC0 = 0;
        static_cast<void>(CaptureMapTilesForTransition_51D640());
        gCurrentSceneState = 3;
        [[fallthrough]];
    case 3: {
        platform::gValue73ECC0 = 0;
        gValueC82204 = 0;
        ApplyPendingMapPaletteReply_51D6F0();
        ResetMapActorDrawKeys_4468F0();
        static_cast<void>(UpdateMapMouseTargetAndButtons_46F680());
        OpenPendingBattleResultWindow_504A50();
        static_cast<void>(NextRotationValue_455C70());
        static_cast<void>(UpdateWorldMapInputAndMovement_469C90());
        ResetMapDrawCacheCounts_46BD40();
        SynchronizeTimedMapNoticeWindow_455FC0();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        RenderChara_521F20();
        UpdateMapEffects_46E4A0();
        DrawMapSceneWithMarkers_469020();
        cg::core::NullSub_509080();
        input::ProcessKeyInput_4596D0();
        input::BrowseChatHistory_44A1B0();
        static_cast<void>(ui::WindowRuntime_Render());
        RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        UpdateDayPeriodPaletteAndTimers_458F00();
        if (gValue5ABFC4 != 1) {
            return;
        }
        static_cast<void>(DestroyPlayerMapPresence_519D50());
        ClearWorldObjectsAndActions_4464D0();
        static_cast<void>(RestorePlayerMovementSnapshot_4681B0());
        DetachPartyMapActions_51B0F0();
        gMapNoticeStartTick_62B5B0 = 0;
        SynchronizeTimedMapNoticeWindow_455FC0();
        ResetTimedMapNotice_455C60();
        const std::uint32_t mask = gValue9FB918.mask;
        const std::uint32_t encoded = gValue9FB918.encoded;
        const std::uint8_t readCount = gValue9FB918.readCount;
        gValueC821E8.mask = mask;
        gValueC821E8.encoded = encoded;
        gValueC821E8.readCount = readCount;
        gPlayerCoordinateSyncPending_C821D8 = std::int32_t{0};
        cg::world::gRippleGate = std::int32_t{1};
        gDirectionalMapTransitionPending_9FB92E = 0;
        gValueFF03D8 = 0;
        gValueF7CA04 = 0;
        gContinuousMapMovementActive_C8A79C = 0;
        gSceneTransitionResetPending_58B6D0 = 1;
        ui::WindowRuntime_ProcessScene();
        static_cast<void>(cg::yobi::ClearObservationGroups_43F230());
        ResumePlayerAfterObservation_51BEF0();
        platform::RequestRender_457010();
        static_cast<void>(audio::PlaySe_52C450(201, 320));
        gWorldBgmResumeIndex_FF0D68 = audio::ReadBgmRecordIndex58D4F8();
        audio::ResetBgmPlayback_52B940();
        ++gCurrentSceneState;
        return;
    }
    case 4:
        static_cast<void>(CaptureSceneAndSelectEvenVariant_51D750());
        platform::gValue73ECC0 = 2;
        ui::WindowRuntime_ProcessScene();
        static_cast<void>(cg::yobi::ClearObservationGroups_43F230());
        ++gCurrentSceneState;
        goto inputTail;
    case 5:
        if (ProcessSceneTransitionEffect_51FDE0(12, 8, 0x40000000u) == 1) {
            static_cast<void>(SetCurrentSceneType(10));
        }
        goto inputTail;
    default:
        return;
    }

waitFrame:
    static_cast<void>(render::ClearSurface_4512C0());
    ++gWorldSceneWaitFrames_FF0D54;
    return;

// 原函数：0x004596D0。
inputTail:
    input::ProcessKeyInput_4596D0();
    render::DrawInputText_4568A0(&cg::ui::detail::gInputContext626AA0);
    RenderInput_4590E0();
    cg::input::DrawAndAdvanceInputCaret_44A410();
}

// 原 [0x51D750,0x51D781)：复制结果不影响后续随机调用和完整 W32 写入。
std::uint32_t CaptureSceneAndSelectEvenVariant_51D750()
{
    static_cast<void>(render::CopyBackToTemp());
    const std::uint32_t bits = static_cast<std::uint32_t>(
        platform::RandomRange_46FAF0(0, 35));
    const std::uint8_t low = static_cast<std::uint8_t>(bits) & 0xFEu;
    const std::uint32_t result = (bits & 0xFFFFFF00u) | low;
    render::gValueFF48E4 = result;
    return result;
}

// 原 [0x51D6F0,0x51D743)：只消费 -1 与非负回复，其他负值保持原状态。
// PTR01：原51D6F0完整20条；持有回复值，调色板调用后按原序发布模式/已消费状态，正确正文保持。
void __cdecl ApplyPendingMapPaletteReply_51D6F0()
{
    const std::int32_t value = gPendingMapPaletteIndex_FF0D4C;
    if (value == -1) {
        static_cast<void>(render::SelectPalette(gCurrentDayPeriod_73ED48, 0));
        gDayPeriodPaletteEnabled_73ED44 = 1;
        gPendingMapPaletteIndex_FF0D4C = -2;
    } else if (value >= 0) {
        static_cast<void>(render::SelectPalette(value, 0));
        gDayPeriodPaletteEnabled_73ED44 = 0;
        gPendingMapPaletteIndex_FF0D4C = -2;
    }
}

// GTP01：原 [0x458F40,0x458FC2)；旧 tick/remaining 先齐取，持有 ADD 结果判断，保原归零/tick 发布。
void __cdecl UpdatePlayerCountdown_458F40()
{
    const std::int32_t scene = gCurrentSceneType;
    if (scene == 9) {
        if (gPlayerRuntimeSlots.action == nullptr) {
            return;
        }
    } else if (scene != 10) {
        return;
    }
    const std::uint64_t tick = platform::ReadTickCount64();
    if (gPlayerRuntimeSlots.timerMode10 != 0) {
        const std::uint64_t previousTick = gPlayerRuntimeSlots.timerTick14;
        const std::int32_t remaining = gPlayerRuntimeSlots.timerRemaining0C;
        const std::int64_t delta =
            std::bit_cast<std::int64_t>(previousTick - tick);
        const std::int64_t updated = remaining + delta;
        gPlayerRuntimeSlots.timerRemaining0C = static_cast<std::int32_t>(updated);
        if (updated < 0) {
            gPlayerRuntimeSlots.timerRemaining0C = 0;
            gPlayerRuntimeSlots.timerMode10 = 0;
        }
    }
    gPlayerRuntimeSlots.timerTick14 = tick;
}

// GTP01：原 [0x458F00,0x458F3A)；持有时段/模式、原过渡 3600，末尾总推进倒计时。
void __cdecl UpdateDayPeriodPaletteAndTimers_458F00()
{
    const std::int32_t category = GetCate_458EC0(&gCgTime);
    if (gCurrentDayPeriod_73ED48 != category) {
        const std::int32_t mode = gDayPeriodPaletteEnabled_73ED44;
        gCurrentDayPeriod_73ED48 = category;
        if (mode == 1) {
            static_cast<void>(render::SelectPalette(category, 3600));
        }
    }
    UpdatePlayerCountdown_458F40();
}

// GTP01：原 [0x519EB0,0x519EBA)；只写玩家唯一 W32 对象号，完整 S32 原样返回。
std::int32_t __cdecl SetPlayerObjectIndex_519EB0(const std::int32_t value)
{
    gPlayerRuntimeSlots.objectIndex = std::bit_cast<std::uint32_t>(value);
    return value;
}

// 原函数：0x0043CBB0。
void DrawBattleScene_43CBB0()
{
    std::int32_t state = gCurrentSceneState;
    if (static_cast<std::uint32_t>(state) >= 2U
        && static_cast<std::uint32_t>(state) <= 6U
        && gBattleBackgroundImage59F9F8 != gBattleBackgroundSnapshot5ABCD8) {
        CaptureBattleBackgroundSnapshot_43CB10();
        state = gCurrentSceneState;
    }
    // 原退出门禁逐项读W16，保持短路，不读取整个W32后截断。
    const auto readBattleEffectWord = [](const std::uint32_t& effect, const std::size_t offset) {
        std::uint16_t value;
        std::memcpy(&value, reinterpret_cast<const std::byte*>(&effect) + offset, sizeof(value));
        return value;
    };
    const auto clearBattleEffects = [] {
        gBattleSceneEffectC8A768 = 0;
        gBattleSceneEffectC8A76C = 0;
    };
    switch (state) {
    case 0: {
        gValue5ABFC4 = 0;
        std::memset(gBattleNamedEffects5ABCE0.data(), 0, sizeof(gBattleNamedEffects5ABCE0));
        std::int32_t title = 0;
        while (title < static_cast<std::int32_t>(gSortedTitlesDE03F8.size())
            && gSortedTitlesDE03F8[title].value != 1101) {
            ++title;
        }
        const bool titleFound = title < static_cast<std::int32_t>(gSortedTitlesDE03F8.size());
        gBattleAttributeEffectFlags59F9E0 = 0;
        if (titleFound) {
            gBattleAttributeEffectFlags59F9E0 = 5;
        }
        if (gMapCharaSceneSlots.valueC8A73E != 0
            || static_cast<std::uint16_t>(gMapCharaSceneSlots.valueC8A744) != 0) {
            static_cast<void>(AdvanceMapParticleCycles_46E4C0(160));
        }
        cg::yobi::YobiClearUp_404D30();
        cg::yobi::InitializeBattleScene_4383F0();
        gBattleEntryNoticeAction5ABFB8 = nullptr;
        gBattleExitAction5ABCDC = nullptr;
        gSceneTransitionResetPending_58B6D0 = 1;
        gBattleCommandAction5ABC00 = nullptr;
        gPendingResultWindowMode_FF0C10 = 3;
        input::SetContext_44B330(&cg::ui::detail::gInputContext626AA0);
        CaptureBattleBackgroundSnapshot_43CB10();
        platform::gValue73ECC0 = 2;
        input::BrowseChatHistory_44A1B0();
        if (gMapResourceMode_9DAD68 == 1) {
            const std::int32_t image = gBattleBackgroundImage59F9F8;
            const bool specialBackground = image >= 238607 && image <= 238618;
            switch (gBattleType5ABFC8) {
            case 1:
            case 4:
                static_cast<void>(cg::audio::LoadAndPlayBGM(specialBackground ? 34 : 25));
                break;
            case 2:
            case 3:
                static_cast<void>(cg::audio::LoadAndPlayBGM(16));
                break;
            case 5:
                static_cast<void>(cg::audio::LoadAndPlayBGM(specialBackground ? 35 : 26));
                break;
            case 6:
                static_cast<void>(cg::audio::LoadAndPlayBGM(26));
                break;
            default:
                break;
            }
        } else {
            switch (gBattleType5ABFC8) {
            case 1:
            case 4:
                static_cast<void>(cg::audio::LoadAndPlayBGM(13));
                break;
            case 2:
            case 3:
                static_cast<void>(cg::audio::LoadAndPlayBGM(16));
                break;
            case 5:
                static_cast<void>(cg::audio::LoadAndPlayBGM(14));
                break;
            case 6:
                static_cast<void>(cg::audio::LoadAndPlayBGM(15));
                break;
            default:
                break;
            }
        }
        const std::int32_t floor = gMapFloor;
        if (floor >= 1457 && floor <= 1480) {
            static_cast<void>(cg::audio::LoadAndPlayBGM(16));
        }
        const std::uint64_t tick = platform::ReadTickCount64();
        platform::gValue73ECC0 = 2;
        platform::gLastRenderDt = static_cast<double>(tick);
        static_cast<void>(ProcessSceneTransitionEffect_51FDE0(13, 8, 0x40000000U));
        static_cast<void>(ui::WindowRuntime_Render());
        RenderInput_4590E0();
        cg::core::NullSub_509080();
        ++gCurrentSceneState;
        break;
    }
    case 1:
        if (ProcessSceneTransitionEffect_51FDE0(13, 8, 0x40000000U) == 1) {
            CaptureBattleBackgroundSnapshot_43CB10();
            platform::gValue73ECC0 = 3;
            const std::uint64_t tick = platform::ReadTickCount64();
            platform::gValue73ECC0 = 3;
            platform::gLastRenderDt = static_cast<double>(tick);
            input::ProcessKeyInput_4596D0();
            input::BrowseChatHistory_44A1B0();
            static_cast<void>(ui::WindowRuntime_Render());
            RenderInput_4590E0();
            cg::input::DrawAndAdvanceInputCaret_44A410();
            cg::core::NullSub_509080();
            ui::WindowRuntime_ProcessScene();
            ++gCurrentSceneState;
        } else {
            static_cast<void>(ui::WindowRuntime_Render());
            RenderInput_4590E0();
            cg::core::NullSub_509080();
        }
        input::ProcessKeyInput_4596D0();
        render::DrawInputText_4568A0(&cg::ui::detail::gInputContext626AA0);
        RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        break;
    case 2: {
        input::ProcessKeyInput_4596D0();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        static_cast<void>(cg::yobi::SortBattleActions_405C30());
        RenderChara_521F20();
        input::BrowseChatHistory_44A1B0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        static_cast<void>(ui::WindowRuntime_Render());
        RenderInput_4590E0();
        static_cast<void>(NextRotationValue_455C70());
        if ((*reinterpret_cast<const unsigned char*>(&gBattlePlayerFlags5ABCAC) & 0x40U) != 0) {
            ui::detail::gHoverTextD68AE8 = L"等待战斗开始。" /* 原常量参考：0x005541CC。 */;
        }
        const std::int32_t readIndex = gBattleCReadIndex5AAA44;
        if (readIndex != gBattleCWriteIndex5AAA6C) {
            const char* const source = gBattleCQueue5A4A3C.data() + readIndex * 0x1000;
            const std::uint32_t size = static_cast<std::uint32_t>(std::strlen(source)) + 1U;
            const std::uint32_t wordBytes = size & ~3U;
            std::memcpy(gBattleCText5A3A20.data(), source, wordBytes);
            const std::int32_t nextIndex = readIndex + 1;
            const bool consumedLastSlot = nextIndex >= 4;
            gBattleCReadIndex5AAA44 = nextIndex;
            std::memcpy(gBattleCText5A3A20.data() + wordBytes, source + wordBytes, size & 3U);
            if (consumedLastSlot) {
                gBattleCReadIndex5AAA44 = 0;
            }
            cg::yobi::ParseBattleC_438550();
            if ((*reinterpret_cast<const unsigned char*>(&gBattleFieldFlags5ABB58) & 0x20U) != 0) {
                gCurrentSceneState = 7;
                break;
            }
            cg::yobi::OrientEnteringBattleSides_43C9A0();
            const std::int32_t position = std::bit_cast<std::int32_t>(gBattlePosition5ABCA8);
            const bool changed = gBattleCurrentPosition5ABCA4 != position;
            gBattleCurrentPosition5ABCA4 = position;
            if (changed) {
                std::int32_t index = 0;
                gBattleSceneIndex5ABFD8 = 0;
                do {
                    if (gBattleAllEntries59E980[index] != nullptr) {
                        cg::yobi::UpdateBattleAlbumFlag_4238F0(gBattleAllEntries59E980[index]);
                        index = gBattleSceneIndex5ABFD8;
                    }
                    gBattleSceneIndex5ABFD8 = ++index;
                } while (index < 20);
            }
            gBattleSnapshotPetIndex5ABBFC = FindBattlePetIndex_43C4B0();
            ui::detail::WindowBattleCommand_SyncActorVitals();
            if (gBattleCurrentPosition5ABCA4 >= 20 && ui::detail::gWindowStates[cg::ui::WindowId_Spectator].window == nullptr) {
                static_cast<void>(ui::WindowRuntime_CreateAnimation(cg::ui::WindowId_Spectator, 1, 0, nullptr));
                static_cast<void>(audio::PlaySe_52C450(56, 320));
            }
            gCurrentSceneState = 3;
        }
        if ((*reinterpret_cast<const unsigned char*>(&gBattlePlayerFlags5ABCAC) & 0x40U) != 0) {
            ui::detail::gHoverTextD68AE8 = L"等待回合结束。" /* 原常量参考：0x005541BC。 */;
        }
        break;
    }
    case 3:
        ui::detail::WindowBattleStateNotice_Open();
        input::ProcessKeyInput_4596D0();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        static_cast<void>(cg::yobi::SortBattleActions_405C30());
        RenderChara_521F20();
        cg::yobi::DrawBattleActorNames_43CA80();
        input::BrowseChatHistory_44A1B0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        static_cast<void>(ui::WindowRuntime_Render());
        RenderInput_4590E0();
        static_cast<void>(NextRotationValue_455C70());
        if (cg::yobi::ReturnBattleActorsToFormation_43C910() == 1) {
            if (gBattleEntryNoticeAction5ABFB8 != nullptr) {
                static_cast<void>(cg::yobi::MarkDeath(gBattleEntryNoticeAction5ABFB8));
                const std::uint32_t flags = gBattlePlayerFlags5ABCAC & ~3U;
                gBattleEntryNoticeAction5ABFB8 = nullptr;
                gBattlePlayerFlags5ABCAC = flags;
            }
            BeginBattleCommandSelection_43C4E0();
            gCurrentSceneState = 4;
        }
        break;
    case 4:
        input::ProcessKeyInput_4596D0();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        static_cast<void>(cg::yobi::SortBattleActions_405C30());
        cg::yobi::DrawBattleActorNames_43CA80();
        input::BrowseChatHistory_44A1B0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        static_cast<void>(ui::WindowRuntime_Render());
        RenderInput_4590E0();
        static_cast<void>(NextRotationValue_455C70());
        ui::detail::WindowBattleCommand_Dispatch();
        RenderChara_521F20();
        UpdatePlayerCountdown_458F40();
        break;
    case 5: {
        input::ProcessKeyInput_4596D0();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        static_cast<void>(cg::yobi::SortBattleActions_405C30());
        RenderChara_521F20();
        cg::yobi::DrawBattleActorNames_43CA80();
        input::BrowseChatHistory_44A1B0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        static_cast<void>(ui::WindowRuntime_Render());
        RenderInput_4590E0();
        static_cast<void>(NextRotationValue_455C70());
        UpdatePlayerCountdown_458F40();
        const std::int32_t readIndex = gBattleMReadIndex5AAA68;
        if (readIndex != gBattleMWriteIndex5A3A1C) {
            const char* const source = gBattleMQueue59FA18.data() + readIndex * 0x1000;
            const std::uint32_t size = static_cast<std::uint32_t>(std::strlen(source)) + 1U;
            const std::uint32_t wordBytes = size & ~3U;
            std::memcpy(gBattleMText5AAA7C.data(), source, wordBytes);
            const std::int32_t nextIndex = readIndex + 1;
            const bool consumedLastSlot = nextIndex >= 4;
            gBattleMReadIndex5AAA68 = nextIndex;
            std::memcpy(gBattleMText5AAA7C.data() + wordBytes, source + wordBytes, size & 3U);
            if (consumedLastSlot) {
                gBattleMReadIndex5AAA68 = 0;
            }
            gBattleMScanOffset5A8A3C = 2;
            gCurrentSceneState = 6;
        } else if (cg::yobi::Sub_43CA50() >= 2) {
            ui::detail::gHoverTextD68AE8 = L"等待其他玩家的输入指令。" /* 原常量参考：0x005541A0。 */;
        }
        break;
    }
    case 6:
        input::ProcessKeyInput_4596D0();
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        static_cast<void>(cg::yobi::SortBattleActions_405C30());
        RenderChara_521F20();
        input::BrowseChatHistory_44A1B0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        static_cast<void>(ui::WindowRuntime_Render());
        RenderInput_4590E0();
        static_cast<void>(NextRotationValue_455C70());
        UpdatePlayerCountdown_458F40();
        break;
    case 7: {
        gBattleActiveEventMask5AAA74 = 0;
        gValue5ABB20 = 1;
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        static_cast<void>(cg::yobi::ConsumeBattleExitMessages_43BAA0());
        cg::yobi::ClearBattleAppendEffects_43BED0();
        gBattleSceneIndex5ABFD8 = 22;
        gBattleAllEntries59E980.fill(nullptr);
        static_cast<void>(cg::audio::BeginBgmFadeOut_52B990());
        static_cast<void>(CaptureSceneAndSelectEvenVariant_51D750());
        const std::uint16_t firstEffect = readBattleEffectWord(gBattleSceneEffectC8A768, 0);
        const bool firstEffectPresent = firstEffect != 0;
        platform::gValue73ECC0 = 2;
        if (firstEffectPresent
            || readBattleEffectWord(gBattleSceneEffectC8A768, 2) != 0
            || readBattleEffectWord(gBattleSceneEffectC8A76C, 0) != 0
            || readBattleEffectWord(gBattleSceneEffectC8A76C, 2) != 0) {
            clearBattleEffects();
            static_cast<void>(AdvanceMapParticleCycles_46E4C0(160));
        }
        const std::int32_t currentState = gCurrentSceneState;
        const std::int32_t nextState = currentState + 1;
        clearBattleEffects();
        gSceneTransitionResetPending_58B6D0 = 1;
        gCurrentSceneState = nextState;
        [[fallthrough]];
    }
    case 8: {
        input::ProcessKeyInput_4596D0();
        render::DrawInputText_4568A0(&cg::ui::detail::gInputContext626AA0);
        RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        const std::int32_t selector = gValueD67E78 == 1 ? 14 : 12;
        if (ProcessSceneTransitionEffect_51FDE0(selector, 8, 0x40000000U) != 1) {
            break;
        }
        ui::WindowRuntime_ProcessScene();
        render::ClearSurfaceTransformCache_44D490();
        static_cast<void>(SetCurrentSceneTypeAndState(9, 1));
        cg::yobi::YobiClearUp_404D30();
        DetachWorldObjectActions_446610();
        cg::world::gRippleGate = std::int32_t{0};
        static_cast<void>(network::AcknowledgeBattleExit_5108F0(platform::gConnectionSocket, 0));
        return;
    }
    case 9: {
        gValue5ABB20 = 1;
        gBattleActiveEventMask5AAA74 = 0;
        static_cast<void>(cg::yobi::YobiUpdateAll_404C50());
        static_cast<void>(cg::yobi::ConsumeBattleExitMessages_43BAA0());
        gBattleSceneIndex5ABFD8 = 22;
        gBattleAllEntries59E980.fill(nullptr);
        static_cast<void>(cg::audio::BeginBgmFadeOut_52B990());
        static_cast<void>(CaptureSceneAndSelectEvenVariant_51D750());
        const std::uint16_t firstEffect = readBattleEffectWord(gBattleSceneEffectC8A768, 0);
        const bool firstEffectPresent = firstEffect != 0;
        platform::gValue73ECC0 = 2;
        if (firstEffectPresent
            || readBattleEffectWord(gBattleSceneEffectC8A768, 2) != 0
            || readBattleEffectWord(gBattleSceneEffectC8A76C, 0) != 0
            || readBattleEffectWord(gBattleSceneEffectC8A76C, 2) != 0) {
            clearBattleEffects();
            static_cast<void>(AdvanceMapParticleCycles_46E4C0(160));
        }
        clearBattleEffects();
        cg::yobi::YobiClearUp_404D30();
        DetachWorldObjectActions_446610();
        cg::world::gRippleGate = std::int32_t{0};
        gSceneTransitionValue = std::int32_t{0};
        static_cast<void>(SetPendingSceneType(7));
        return;
    }
    default:
        break;
    }

    if (platform::gValue73ECC0 == 2) {
        return;
    }
    // G02：参考43D07B前置业务；保留Sub_4990A0内部的存活与可见门禁。
    if (gBattleShowEnemyHpBars) {
        for (std::int32_t enemy = 10; enemy < 20; ++enemy) {
            cg::yobi::YobiAction* const actor = gBattleEntries59E980[enemy];
            if (actor != nullptr && cg::yobi::ReadYobiBatchType(actor) == 0
                && (cg::yobi::GetYobiActionFlagsLowByte(actor) & 2u) == 0) {
                cg::yobi::DrawBattleActorHpBar_4990A0(enemy);
            }
        }
    }
    const std::int32_t position = gBattleCurrentPosition5ABCA4;
    std::int32_t index = position < 10 || position >= 20 ? 0 : 10;
    const std::int32_t end = position < 10 ? 10 : 20;
    gBattleSceneIndex5ABFD8 = index;
    do {
        cg::yobi::YobiAction* const actor = gBattleAllEntries59E980[index];
        if (actor != nullptr) {
            const auto& point = gBattleGraphicPoints553758[gBattlePointIndices5536FC[index]];
            bool atRest = cg::yobi::GetYobiActionField30(actor) == point[2]
                && cg::yobi::GetYobiActionField34(actor) == point[3];
            if (!atRest) {
                const std::int32_t action = cg::yobi::GetYobiActionField2B0(actor);
                atRest = action == 0 || action == 9 || action == 8 || action == 10 || action == 1;
            }
            if (atRest && cg::yobi::ReadYobiBatchType(actor) == 0 && (cg::yobi::GetYobiActionFlagsLowByte(actor) & 2U) == 0) {
                cg::yobi::DrawBattleActorHpBar_4990A0(index);
                index = gBattleSceneIndex5ABFD8;
            }
        }
        gBattleSceneIndex5ABFD8 = ++index;
    } while (index < end);
    if (platform::gValue73ECC0 == 2) {
        return;
    }
    UpdateMapEffects_46E4A0();
    if (gValue5ABCB0 == 1) {
        gCurrentSceneState = 7;
        gValue5ABCB0 = 0;
    }
    cg::yobi::YobiAction* const exitAction = gBattleExitAction5ABCDC;
    if (exitAction != nullptr && cg::yobi::ReadYobiActionHpOnce(exitAction) > 0) {
        cg::yobi::YobiAction* const action = gBattleExitAction5ABCDC;
        const std::int32_t y = cg::yobi::GetYobiActionField34(action);
        const std::int32_t x = cg::yobi::GetYobiActionField30(action);
        if (cg::ui::HitTestMouseRect_5049A0(x + 20, y + 37, x + 110, y + 63, 213) == 1) {
            ui::detail::gHoverTextD68AE8 = L"结束观战。" /* 原常量参考：0x00554194。 */;
            const auto clicked = *reinterpret_cast<const unsigned char*>(
                &platform::gLegacyMouseState.edgeButtons);
            const bool exitClicked = (clicked & 1U) != 0;
            static_cast<void>(0);
            if (exitClicked) {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
                static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"U" /* 原常量参考：0x00554190。 */));
                gCurrentSceneState = 7;
            }
        }
        cg::yobi::YobiAction* const drawAction = gBattleExitAction5ABCDC;
        const std::int32_t drawY = cg::yobi::GetYobiActionField34(drawAction);
        const std::int32_t drawX = cg::yobi::GetYobiActionField30(drawAction);
        static_cast<void>(render::DrawText2_456670(
            static_cast<std::int16_t>(drawX + 23), static_cast<std::int16_t>(drawY + 40),
            2, 2, 0, L"观战结束" /* 原常量参考：0x00554184。 */, 0));
    }
}

// 原函数：0x0051D5B0。
void* __cdecl PrepareMapTransitionObservationState_51D5B0()
{
    const std::uint16_t current = gMapCharaSceneSlots.valueC8A73E;
    std::uint16_t low;
    std::memcpy(&low, &gMapCharaSceneSlots.valueC8A744, sizeof(low));
    gMapCharaSceneSlots.valueC8A740 = current;
    const std::uint32_t gate = gValueD67E78;
    std::memcpy(
        reinterpret_cast<std::uint8_t*>(&gMapCharaSceneSlots.valueC8A744) + sizeof(low),
        &low, sizeof(low));

    if (gate != 0
        || cg::yobi::FindObservationGroupByMember_43F1F0(cg::world::gPlayerRuntimeSlots.objectIndex)
            == nullptr) {
        const std::uint32_t currentGate = gValueD67E78;
        gValueFF0D74 = 0;
        if (currentGate != 0) {
            ResumePlayerAfterObservation_51BEF0();
        }
    } else if (gValueFF0D74 == 0) {
        gCurrentSceneState = 250;
        static_cast<void>(cg::yobi::SavePlayerObservationTravelSnapshot_440F50());
        ++gValueFF0D74;
    } else {
        const std::uint32_t count = gValueFF0D74;
        gCurrentSceneState = 252;
        gValueFF0D74 = count + 1u;
    }
    return cg::yobi::ClearObservationGroups_43F230();
}

// 原 [0x469030,0x4691C9)：装载后可选重建第四平面，随后斜向扫描 tile。
std::int32_t __cdecl LoadAndDrawMapTiles_469030()
{
    const std::int32_t loadYEnd = gMapYEnd;
    const std::int32_t loadXEnd = gMapXEnd;
    const std::int32_t loadYStart = gMapYStart;
    const std::int32_t loadXStart = gMapXStart;
    const std::int32_t loadFloor = gMapFloor;
    const std::int32_t loadMapId = std::bit_cast<std::int32_t>(gCurrentMapId_9A86DC);
    const std::int32_t loadResult = ReadMapCacheRectangle_468C40(
        loadMapId, loadFloor, loadXStart, loadYStart, loadXEnd, loadYEnd,
        gMapTileArray.data(), gMapObjArray.data(), gMapHitArray.data());
    if (loadResult >= 0) {
        const std::int32_t walkYEnd = gMapYEnd;
        const std::int32_t walkXEnd = gMapXEnd;
        const std::int32_t walkYStart = gMapYStart;
        const std::int32_t walkXStart = gMapXStart;
        static_cast<void>(BuildMapWalkability_46D9C0(
            walkXStart, walkYStart, walkXEnd, walkYEnd,
            gMapTileArray.data(), gMapObjArray.data(), gMapHitArray.data(),
            gMapWalkabilityArray.data()));
    }

    const std::int32_t yEnd = gMapYEnd;
    const std::int32_t xStart = gMapXStart;
    const std::int32_t spanY = gMapSpanY;
    // 原 X 符号修正后取投影 C，Y 三倍项后取 D，再完成乘 16。
    const std::int32_t rawX = (xStart + yEnd) * 64 - 64;
    const std::int32_t adjustedX = rawX - (rawX < 0 ? -1 : 0);
    const std::int32_t projectionX =
        std::bit_cast<std::int32_t>(gWorldProjectionSourceC);
    std::int32_t baseX = (adjustedX >> 1) + projectionX;
    const std::int32_t yTriple = 3 * (yEnd - xStart - 1);
    const std::int32_t projectionY =
        std::bit_cast<std::int32_t>(gWorldProjectionSourceD);
    const std::int32_t rawY = yTriple * 16;
    std::int32_t result = rawY - (rawY < 0 ? -1 : 0);
    std::int32_t drawY = (result >> 1) + projectionY;
    std::int32_t column = 0;
    std::int32_t row = spanY - 1;

    while (row >= 0) {
        std::int32_t scanRow = row;
        std::int32_t drawX = baseX;
        const std::int32_t diagonal = column - row;
        while (scanRow >= 0) {
            const std::int32_t scanColumn = diagonal + scanRow;
            if (scanColumn < 0) {
                break;
            }
            const std::int32_t index =
                scanColumn + scanRow * gMapSpanX;
            result = ResolveMapGraphicNumber_46CF00(
                gMapTileArray[static_cast<std::size_t>(index)]);
            if (result > 999 && drawX >= -32 && drawX < platform::gWindowWidth + 32 &&
                drawY >= -24 && drawY < platform::gWindowHeight + 24) {
                result = cg::render::DrawGraphic(
                    drawX, drawY, 5, result, 0, nullptr);
            }
            --scanRow;
            drawX -= 64;
        }
        if (column < gMapSpanX - 1) {
            ++column;
            baseX += 32;
        } else {
            --row;
            baseX -= 32;
        }
        drawY -= 24;
    }
    return result;
}

// 原 [0x4693F0,0x4697C9)：tile 扫描、obj 扫描与尺寸向上覆盖共用同一输出。
std::int32_t __cdecl BuildGraphicMapWalkability_4693F0(
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk)
{
    std::memset(walk, 0, kMapBufferCells * sizeof(std::uint16_t));
    const std::int32_t width = xEnd - xStart;
    const std::int32_t height = yEnd - yStart;
    if (width < 1 || height < 1) {
        return 0;
    }

    for (std::int32_t y = 0; y < height; ++y) {
        for (std::int32_t x = 0; x < width; ++x) {
            const std::int32_t index = y * width + x;
            const std::int32_t code = ResolveMapGraphicNumber_46CF00(tile[index]);
            std::uint16_t& value = walk[index];
            if (code > 999 || (code >= 60 && code <= 79)) {
                std::int32_t graphic = 0;
                std::uint16_t flag = 0;
                resource::GetGraphicMapNoIndex(code, &graphic);
                resource::GetGraphicFlag1(static_cast<std::uint32_t>(graphic), &flag);
                if (flag == 2) {
                    value = 2;
                } else if (flag == 0 && value != 2) {
                    value = 1;
                }
            } else if (code == 4) {
                value = 2;
            } else if (code == 2 || code == 5 || code == 6 ||
                       code == 9 || code == 10 ||
                       (code == 0 &&
                        (reinterpret_cast<const std::uint8_t*>(hit)[2 * index + 1] & 0x40u) != 0)) {
                if (value != 2) {
                    value = 1;
                }
            }
        }
    }

    for (std::int32_t y = 0; y < height; ++y) {
        for (std::int32_t x = 0; x < width; ++x) {
            const std::int32_t index = y * width + x;
            const std::int32_t code = ResolveMapGraphicNumber_46CF00(obj[index]);
            std::uint16_t& value = walk[index];
            if (code > 999 || (code >= 60 && code <= 79)) {
                std::int32_t graphic = 0;
                std::uint16_t flag = 0;
                resource::GetGraphicMapNoIndex(code, &graphic);
                resource::GetGraphicFlag1(static_cast<std::uint32_t>(graphic), &flag);
                if (code > 999 && (flag == 0 || flag == 2)) {
                    std::uint16_t sx = 0;
                    std::uint16_t sy = 0;
                    resource::GetGraphicSxSy(
                        static_cast<std::uint32_t>(graphic), &sx, &sy);
                    const std::int32_t graphicWidth =
                        std::bit_cast<std::int16_t>(sx);
                    const std::int32_t graphicHeight =
                        std::bit_cast<std::int16_t>(sy);
                    for (std::int32_t row = 0; row < graphicHeight; ++row) {
                        for (std::int32_t col = 0; col < graphicWidth; ++col) {
                            if (y - row >= 0 && x + col < width) {
                                std::uint16_t& covered =
                                    walk[(y - row) * width + x + col];
                                if (flag == 2 || covered != 2) {
                                    covered = flag == 2 ? 2 : 1;
                                }
                            }
                        }
                    }
                } else if (flag == 2) {
                    value = 2;
                } else if (flag == 0 && value != 2) {
                    value = 1;
                }
            } else if (code == 4) {
                value = 2;
            } else if (code == 2 || code == 5 || code == 6 ||
                       code == 9 || code == 10) {
                if (value != 2) {
                    value = 1;
                }
            }
            if (obj[index] == 0 && tile[index] == 0 && value == 0) {
                value = 1;
            }
            if (reinterpret_cast<const std::uint8_t*>(hit)[2 * index] == 1) {
                value = 1;
            }
        }
    }
    return 2 * width;
}

// 原 [0x46D9C0,0x46DA72)：模式 1 使用 hit 高字节，其余完整委托 4693F0。
std::int32_t __cdecl BuildMapWalkability_46D9C0(
    const std::int32_t xStart, const std::int32_t yStart,
    const std::int32_t xEnd, const std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk)
{
    if (gMapResourceMode_9DAD68 != 1) {
        return BuildGraphicMapWalkability_4693F0(xStart, yStart, xEnd, yEnd, tile, obj, hit, walk);
    }
    std::memset(walk, 0, kMapBufferCells * sizeof(std::uint16_t));
    const std::int32_t width = xEnd - xStart;
    const std::int32_t remaining = yEnd - yStart;
    if (width < 1 || remaining < 1) {
        return remaining;
    }
    for (std::int32_t y = 0; y < remaining; ++y) {
        for (std::int32_t x = 0; x < width; ++x) {
            const std::int32_t index = y * width + x;
            if ((reinterpret_cast<const std::uint8_t*>(hit)[2 * index + 1] & 1u) != 0) {
                walk[index] = 1;
            }
        }
    }
    return 0;
}

// MHT01：原 [0x46AD00,0x46AD97)；先取齐 X/Y 边界，time 后 fresh 跨度，保低 BYTE/W16 发布与分类序。
std::int32_t __cdecl IsMapTransitionCellActive_46AD00(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t left = gMapXStart;
    const std::int32_t top = gMapYStart;
    const std::int32_t localX = x - left;
    const std::int32_t localY = y - top;
    const std::int32_t category = GetCate_458EC0(&gCgTime);
    const std::int32_t span = gMapSpanX;
    const std::int32_t index = span * localY + localX;
    const std::uint8_t value =
        static_cast<std::uint8_t>(gMapHitArray[static_cast<std::size_t>(index)]);
    const bool isThree = value == 3;
    gLastDetectedCode = value;
    if (isThree || value == 9 || value == 11 || value == 13 || value == 10) {
        return 1;
    }
    if (value == 6) {
        return category == 3;
    }
    if (value == 7) {
        return category == 0 || category == 1;
    }
    return value == 8 && category == 2;
}

// 原 [0x4682E0, 0x468330)；六次写入后传递四个原参数。
void __cdecl SetWorldMapMetadata_4682E0(
    std::uint32_t a1,
    std::int32_t a2,
    std::uint32_t a3,
    std::uint32_t a4,
    std::uint32_t a5,
    std::uint32_t a6,
    std::uint32_t a7,
    std::int32_t a8)
{
    gValueC35008 = a5;
    gValue9DAD2C = a6;
    gMapWidth_9FB930 = a3;
    gMapHeight_C821D4 = a4;
    gCurrentMapId_9A86DC = a1;
    gMapFloor = a2;
    InitializeMapSceneResources_46DA80(a1, a2, a8, a7);
}

// 原 [0x46DA80, 0x46DAF5)；模式 1 的加载器必须保留调用次序。
void __cdecl InitializeMapSceneResources_46DA80(
    std::uint32_t a1,
    std::int32_t a2,
    std::int32_t a3,
    std::uint32_t a4)
{
    audio::ResetSpatialSoundSources_52CD40();
    gMapSceneResourceMapId_9DAD5C = a1;
    gMapResourceMode_9DAD68 = a4;
    gMapSceneResourceFloor_9DAD58 = a2;
    if (a4 != 1) {
        return;
    }

    const std::int32_t selected = a3 == -1 ? a2 : a3;
    gMapSceneSelectedResourceSnapshot_9DAD60 = selected;
    gMapSceneSelectedResourceMirror_9DAD64 = selected;
    LoadCutMapData_46DB00(selected);
    LoadAutoMapColorData_46DC90(selected);
    cg::yobi::ClearRegisteredMapActions_46DD40();
}

// 原 [0x46A9E0, 0x46A9F4)；依次清三槽并返回完整 32 位零。
std::int32_t __cdecl ResetPendingPathEvent_46A9E0()
{
    gPendingPathEventActive_C82230 = 0;
    gPendingPathEventSteps_C82234 = 0;
    gPendingPathEventType_C82236 = 0;
    return 0;
}

// PDH01：原 [0x46AA00,0x46AB56)；X/Y 字段取样后先 Y 更新，两整数差再落 F32，保早退 Y/pending 序。
std::int32_t __cdecl DetectPendingPathEvents_46AA00()
{
    std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    if (std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202) <= 0) {
        return y;
    }

    std::int32_t step = 0;
    std::int32_t result = 0;
    std::int32_t previousX = x;
    std::int32_t previousY = y;
    for (;;) {
        const std::int16_t direction =
            std::bit_cast<std::int16_t>(
                gPendingDirections9A86D4[static_cast<std::size_t>(step)]);
        const std::int32_t deltaX =
            gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].x;
        const std::int32_t deltaY =
            gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].y;
        y += deltaY;
        x += deltaX;
        const std::int32_t differenceYInt = y - previousY;
        const std::int32_t differenceXInt = x - previousX;
        const float differenceY = static_cast<float>(differenceYInt);
        const float differenceX = static_cast<float>(differenceXInt);
        const std::int32_t heading = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(
                GetEightWayMapDirection_46CDF0(differenceX, differenceY)));

        if (IsDirectionalMapTransitionCell_46AE60(x, y) != 0) {
            result = 1;
            gPendingPathEventSteps_C82234 = static_cast<std::int16_t>(step);
            gPendingPathEventActive_C82230 = 1;
            gPendingPathEventType_C82236 = 3;
            gDirectionalMapTransitionHeading_9ED440 = std::bit_cast<std::uint32_t>(heading);
            if (heading < 0) {
                gDirectionalMapTransitionHeading_9ED440 = std::bit_cast<std::uint32_t>(heading) + 8U;
            }
            --step;
            break;
        }
        if (IsMapTransitionCellActive_46AD00(x, y) != 0) {
            result = 1;
            const std::int32_t nextStep = step + 1;
            gPendingPathEventActive_C82230 = 1;
            gPendingPathEventSteps_C82234 = static_cast<std::int16_t>(nextStep);
            gPendingPathEventType_C82236 = 1;
            break;
        }
        // 原4B0F10不读参数；直接使用UI同一零返回入口。
        if (ui::detail::WindowElement_NoOp(0, 0) != 0) {
            result = 1;
            const std::int32_t nextStep = step + 1;
            gPendingPathEventActive_C82230 = 1;
            gPendingPathEventSteps_C82234 = static_cast<std::int16_t>(nextStep);
            gPendingPathEventType_C82236 = 2;
            break;
        }
        ++step;
        previousX = x;
        previousY = y;
        if (step >= std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202)) {
            break;
        }
    }

    if (step < 2 && result != 0) {
        ++step;
        gMovePathCount_C82200 = 0;
        gPendingDirectionCount_C82202 = static_cast<std::uint16_t>(step);
    }
    return result;
}

// 原 [0x46BC90,0x46BCE4)：先减路径数量，剩余项按方向、X、Y 原序左移。
void __cdecl PopPlayerMovementPath_46BC90()
{
    if (std::bit_cast<std::int16_t>(gMovePathCount_C82200) <= 0) {
        return;
    }
    --gMovePathCount_C82200;
    const std::int32_t remaining =
        std::bit_cast<std::int16_t>(gMovePathCount_C82200);
    for (std::int32_t index = 0; index < remaining; ++index) {
        const std::size_t position = static_cast<std::size_t>(index);
        gMovePathDirections[position] = gMovePathDirections[position + 1];
        gMovePathX[position] = gMovePathX[position + 1];
        gMovePathY[position] = gMovePathY[position + 1];
    }
}

// 原 [0x46ABD0,0x46AC0A)：逐项搬路径方向，再消耗三平面首项。
void __cdecl QueueNextMovementDirections_46ABD0()
{
    for (std::int32_t iteration = 0; iteration < 2; ++iteration) {
        if (std::bit_cast<std::int16_t>(gMovePathCount_C82200) <= 0) {
            break;
        }
        const std::int16_t count = std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202);
        const std::uint16_t direction = gMovePathDirections[0];
        const std::uint16_t nextCount =
            static_cast<std::uint16_t>(gPendingDirectionCount_C82202 + 1U);
        gPendingDirections9A86D4[static_cast<std::size_t>(count)] = direction;
        gPendingDirectionCount_C82202 = nextCount;
        PopPlayerMovementPath_46BC90();
    }
}

// 原函数：0x0046AC10。
std::uint16_t SendPendingMovementDirections_46AC10()
{
    const std::uint16_t count = gPendingDirectionCount_C82202;
    if (std::bit_cast<std::int16_t>(count) <= 0) {
        return count;
    }
    std::int32_t index = 0;
    for (;;) {
        const std::int16_t direction = std::bit_cast<std::int16_t>(
            gPendingDirections9A86D4[static_cast<std::size_t>(index)]);
        const std::uint32_t encoded = EncodeMovementDirectionByte_46BD20(direction, 0);
        const std::int16_t remaining = std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202);
        gMoveDirectionText9E03D0[static_cast<std::size_t>(index)] =
            std::bit_cast<char>(static_cast<std::uint8_t>(encoded));
        ++index;
        if (index >= remaining) {
            break;
        }
    }
    if (gValueFF0D36 != 0) {
        const std::uint32_t flags = ReadPlayerFlags18A();
        if ((flags & 0x100u) == 0) {
            return static_cast<std::uint16_t>(flags);
        }
    }
    gMoveDirectionText9E03D0[static_cast<std::size_t>(index)] = '\0';
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    return static_cast<std::uint16_t>(
        SendCurrentPlayerMovementPath_50C8D0(x, y, gMoveDirectionText9E03D0.data()));
}

// 原 [0x46BCF0,0x46BD1D)：正数量减一后将剩余方向原地左移。
void __cdecl PopPendingMovementDirection_46BCF0()
{
    if (std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202) <= 0) {
        return;
    }
    --gPendingDirectionCount_C82202;
    const std::int32_t remaining =
        std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202);
    for (std::int32_t index = 0; index < remaining; ++index) {
        const std::size_t position = static_cast<std::size_t>(index);
        gPendingDirections9A86D4[position] =
            gPendingDirections9A86D4[position + 1];
    }
}

// 原 [0x46ADA0,0x46AE53)：type 1 当次读取探测值，发送后更新请求状态。
void* __cdecl RequestDetectedMapTransition_46ADA0()
{
    static_cast<void>(RestorePlayerMovementSnapshot_4681B0());

    const std::uint16_t sequenceLow =
        static_cast<std::uint16_t>(gMapTransitionRequestSequence_C8221C);
    gMapTransitionRequestPending_9A86D8 = 1;
    gMapMovementInputBlocked_C82220 = 1;
    gPendingMapTransitionSequence_9DAD54 = sequenceLow;

    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    const std::int32_t detectedCode =
        std::bit_cast<std::int16_t>(gLastDetectedCode);
    const std::uint32_t sequence = gMapTransitionRequestSequence_C8221C;
    static_cast<void>(network::SendMapTransitionRequest_510630(
        platform::gConnectionSocket,
        detectedCode,
        std::bit_cast<std::int32_t>(sequence),
        x,
        y,
        -1));

    const std::uint32_t nextSequence = gMapTransitionRequestSequence_C8221C + 1U;
    gCurrentSceneState = 200;
    gMapTransitionRequestSequence_C8221C = nextSequence;
    void* const result = PrepareMapTransitionObservationState_51D5B0();
    gMapTransitionObservationPrepared_C8220C = 1;
    gMapTransitionReplyPending_C82210 = 1;
    gMapRectangleRequestPending_C82208 = 1;
    gMapTransitionAwaitingRectangle_C82214 = 0;
    gMapNameReceivedDuringTransition_C82218 = 0;
    return result;
}

// 原 [0x46AB60,0x46ABCD)：先分发，再只清 active/countdown。
std::int32_t __cdecl DispatchPendingPathEvent_46AB60()
{
    if (gPendingPathEventActive_C82230 == 0) {
        return 0;
    }
    if (gPlayerReachedMovementTarget_C8222E != 0) {
        if (gPendingPathEventSteps_C82234 > 0) {
            --gPendingPathEventSteps_C82234;
            if (gPendingPathEventSteps_C82234 > 0) {
                return 0;
            }
        }
    } else if (gPendingPathEventSteps_C82234 > 0) {
        return 0;
    }

    switch (gPendingPathEventType_C82236) {
    case 1:
        RequestDetectedMapTransition_46ADA0();
        break;
    case 2:
        RequestPlayerCoordinateSynchronization_46AC90();
        break;
    case 3:
        RequestDirectionalMapTransition_46AEA0();
        break;
    default:
        break;
    }
    gPendingPathEventActive_C82230 = 0;
    gPendingPathEventSteps_C82234 = 0;
    return 1;
}

// 原 [0x46AC90,0x46AD00)：临时 CryptInt 赋值后发送玩家坐标。
std::int32_t __cdecl RequestPlayerCoordinateSynchronization_46AC90()
{
    static_cast<void>(RestorePlayerMovementSnapshot_4681B0());

    // 原temporary{}的reserved零初值只在临时对象内，不发布；整数赋值仍一次RNG。
    gPlayerCoordinateSyncPending_C821D8 = std::int32_t{1};

    gMapMovementInputBlocked_C82220 = 1;
    gValueB65A7C = 0;
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    return network::SendPlayerCoordinates_510810(platform::gConnectionSocket, x, y);
}

// 原 [0x46AEA0,0x46AF24)：恢复地图后发送五整数请求，重读序号再自增。
std::int32_t __cdecl RequestDirectionalMapTransition_46AEA0()
{
    static_cast<void>(RestorePlayerMovementSnapshot_4681B0());

    const std::uint32_t mapValue = gDirectionalMapTransitionHeading_9ED440;
    const std::uint16_t sequenceLow = static_cast<std::uint16_t>(gMapTransitionRequestSequence_C8221C);
    gDirectionalMapTransitionPending_9FB92E = 1;
    gMapMovementInputBlocked_C82220 = 1;
    gPendingDirectionalTransitionSequence_9FB92C = sequenceLow;

    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    const std::uint32_t sequence = gMapTransitionRequestSequence_C8221C;
    static_cast<void>(network::SendMapTransitionRequest_510630(
        platform::gConnectionSocket,
        2,
        std::bit_cast<std::int32_t>(sequence),
        x,
        y,
        std::bit_cast<std::int32_t>(mapValue)));

    const std::uint32_t nextSequence = gMapTransitionRequestSequence_C8221C + 1U;
    gValueB65A7C = 1;
    gMapTransitionRequestSequence_C8221C = nextSequence;
    return std::bit_cast<std::int32_t>(nextSequence);
}

// 原 [0x468F30, 0x468F6F)；保持十二次 W32 写入的指令顺序。
std::int32_t __cdecl ResetMapProjectionState_468F30()
{
    gMapProjectionOffsetX_C3500C = 0;
    gMapProjectionOffsetY_C75010 = 0;
    gWorldProjectionSourceA = 0;
    gWorldProjectionSourceB = 0;
    gWorldProjectionSnapshotA = 0;
    gWorldProjectionSnapshotB = 0;
    gValue9EE8A0 = 0;
    gValue9ED438 = 0;
    gWorldProjectionSourceC = 0;
    gWorldProjectionSourceD = 0;
    gWorldProjectionSnapshotC = 0;
    gWorldProjectionSnapshotD = 0;
    return 0;
}

// 原 [0x468F70,0x468FE4)；F32 局部投影、加 0.5 后截断，依次发布 X/Y 及两槽。
std::int64_t __cdecl InitializeMapProjectionOrigin_468F70()
{
    float x;
    float y;
    ProjectWorldToScreen_46CE30(0.0F, 0.0F, &x, &y);
    const std::int64_t projectedX = static_cast<std::int64_t>(
        x + 0.5);
    const double roundedY = y + 0.5;
    gWorldProjectionSourceA = static_cast<std::uint32_t>(projectedX);
    const std::int64_t projectedY = static_cast<std::int64_t>(roundedY);
    gWorldProjectionSourceC = static_cast<std::uint32_t>(projectedX);
    gWorldProjectionSnapshotA = static_cast<std::uint32_t>(projectedX);
    gWorldProjectionSourceB = static_cast<std::uint32_t>(projectedY);
    gWorldProjectionSourceD = static_cast<std::uint32_t>(projectedY);
    gWorldProjectionSnapshotB = static_cast<std::uint32_t>(projectedY);
    gMapProjectionOffsetX_C3500C = 0;
    gMapProjectionOffsetY_C75010 = 0;
    return projectedY;
}

// 原 [0x46CF20, 0x46CF57)；两段顺序填值，返回原生表一过尾指针。
std::int32_t* __cdecl InitializeMapGraphicNumberTable_46CF20()
{
    for (std::int32_t index = 0; index < 20000; ++index) {
        gValuesC35010[index] = index;
    }
    for (std::int32_t index = 20000; index < 65536; ++index) {
        gValuesC35010[index] = index + 200000;
    }
    return gValuesC35010 + 65536;
}

// PDH01：原 [0x46B580,0x46B5B3)；先算 Y/X 两整数差，再依次落 F32，透传方向 S64。
std::int64_t __cdecl GetMapDirectionBetweenPoints_46B580(
    const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth)
{
    const std::int32_t differenceYInt = fourth - second;
    const std::int32_t differenceXInt = third - first;
    const float differenceY = static_cast<float>(differenceYInt);
    const float differenceX = static_cast<float>(differenceXInt);
    return GetEightWayMapDirection_46CDF0(differenceX, differenceY);
}

// 原 [0x46B5C0,0x46B6F0)：两次阻挡探测后覆写当前路径末项，计数不增长。
std::int32_t __cdecl MergePathTailIntoDiagonalStep_46B5C0(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t firstX, const std::int32_t firstY,
    const std::int32_t secondX, const std::int32_t secondY)
{
    if ((secondX == 0 && secondY == 0) ||
        (firstX == 0 && firstY == 0)) {
        return 0;
    }
    const std::int32_t combinedX = firstX + secondX;
    if (combinedX != 1 && combinedX != -1) {
        return 0;
    }
    const std::int32_t combinedY = firstY + secondY;
    if (combinedY != 1 && combinedY != -1) {
        return 0;
    }

    std::uint16_t direction = 0;
    for (; direction < gDirectionDeltas55D9DC.size(); ++direction) {
        if (gDirectionDeltas55D9DC[direction].x == combinedX &&
            gDirectionDeltas55D9DC[direction].y == combinedY) {
            break;
        }
    }
    const std::int32_t plusX =
        gDirectionDeltas55D9DC[(direction + 1U) & 7U].x;
    const std::int32_t plusY =
        gDirectionDeltas55D9DC[(direction + 1U) & 7U].y;
    const std::int32_t minusX =
        gDirectionDeltas55D9DC[(direction - 1U) & 7U].x;
    const std::int32_t minusY =
        gDirectionDeltas55D9DC[(direction - 1U) & 7U].y;
    const std::int32_t firstBlocked =
        IsMapCellBlocked_469810(x + plusX - secondX, y + plusY - secondY);
    const std::int32_t secondBlocked =
        IsMapCellBlocked_469810(x + minusX - secondX, y + minusY - secondY);
    if (firstBlocked != 0 || secondBlocked != 0) {
        return 0;
    }

    const std::uint16_t last =
        static_cast<std::uint16_t>(gMovePathCount_C82200 - 1U);
    const std::size_t index = static_cast<std::size_t>(
        std::bit_cast<std::int16_t>(last));
    gMovePathX[index] = static_cast<std::uint16_t>(x + firstX);
    gMovePathDirections[index] = direction;
    gMovePathY[index] = static_cast<std::uint16_t>(y + firstY);
    gMovePathCount_C82200 = static_cast<std::uint16_t>(last + 1U);
    return 1;
}

// 原函数：0x0046B6F0。
std::int32_t BuildPlayerMovementPath_46B6F0()
{
    const std::int32_t targetX =
        static_cast<std::int32_t>(gMoveTargetX9A7F88);
    const std::int32_t targetY =
        static_cast<std::int32_t>(gMoveTargetY9A7F98);
    std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::array<std::int32_t, 3> offsets {0, 1, -1};
    std::int32_t accumulatedX = 0;
    std::int32_t accumulatedY = 0;
    gPlayerPathFinalFacingDirection_55D9FC = 0xFFFFu;
    if (x == targetX && y == targetY) {
        return 0;
    }
    const float initialY = static_cast<float>(targetY - y);
    const float initialX = static_cast<float>(targetX - x);
    const std::int32_t initialDirection = static_cast<std::int32_t>(
        GetEightWayMapDirection_46CDF0(initialX, initialY));
    gMovePathCount_C82200 = 0;

    while (x != targetX || y != targetY) {
        if (std::bit_cast<std::int16_t>(gMovePathCount_C82200) >= 100) {
            gMovePathCount_C82200 = 0;
            return targetY;
        }
        const float differenceY = static_cast<float>(targetY - y);
        const float differenceX = static_cast<float>(targetX - x);
        const std::int32_t desired = static_cast<std::int32_t>(
            GetEightWayMapDirection_46CDF0(differenceX, differenceY));
        if (desired != initialDirection
            && desired != ((initialDirection + 1) & 7)
            && desired != ((initialDirection - 1) & 7)) {
            break;
        }

        bool advanced = false;
        for (const std::int32_t offset : offsets) {
            std::int32_t direction = (desired + offset) & 7;
            if (direction != initialDirection
                && direction != ((initialDirection + 1) & 7)
                && direction != ((initialDirection - 1) & 7)) {
                continue;
            }
            std::int32_t deltaY =
                gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].y;
            std::int32_t deltaX =
                gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].x;
            const std::int32_t nextY = y + deltaY;
            const std::int32_t nextX = x + deltaX;
            if (IsMapCellBlocked_469810(nextX, nextY) != 0) {
                if (nextX == targetX && nextY == targetY && (direction & 1) == 0) {
                    break;
                }
                continue;
            }
            if ((direction & 1) != 0) {
                const std::int32_t plusDirection = (direction + 1) & 7;
                const std::int32_t minusDirection = (direction - 1) & 7;
                const std::int32_t plusY = gDirectionDeltas55D9DC[
                    static_cast<std::size_t>(plusDirection)].y;
                const std::int32_t plusX = gDirectionDeltas55D9DC[
                    static_cast<std::size_t>(plusDirection)].x;
                const std::int32_t minusX = gDirectionDeltas55D9DC[
                    static_cast<std::size_t>(minusDirection)].x;
                const std::int32_t minusY = gDirectionDeltas55D9DC[
                    static_cast<std::size_t>(minusDirection)].y;
                const std::int32_t plusNextX = x + plusX;
                const std::int32_t plusNextY = y + plusY;
                const std::int32_t plusBlocked = IsMapCellBlocked_469810(plusNextX, plusNextY);
                const std::int32_t minusNextX = x + minusX;
                const std::int32_t minusNextY = y + minusY;
                const std::int32_t minusBlocked = IsMapCellBlocked_469810(minusNextX, minusNextY);
                if (plusBlocked != 0) {
                    if (minusBlocked != 0) {
                        break;
                    }
                    if (MergePathTailIntoDiagonalStep_46B5C0(
                            x, y, minusX, minusY, accumulatedX, accumulatedY) != 0) {
                        accumulatedX += minusX;
                        accumulatedY += minusY;
                    } else {
                        const std::uint16_t count = gMovePathCount_C82200;
                        const std::size_t index = static_cast<std::size_t>(
                            std::bit_cast<std::int16_t>(count));
                        gMovePathDirections[index] =
                            static_cast<std::uint16_t>(minusDirection);
                        gMovePathX[index] = static_cast<std::uint16_t>(x + minusX);
                        gMovePathY[index] = static_cast<std::uint16_t>(y + minusY);
                        gMovePathCount_C82200 = static_cast<std::uint16_t>(count + 1u);
                        accumulatedX = 0;
                        accumulatedY = 0;
                    }
                    deltaX -= minusX;
                    deltaY -= minusY;
                    direction = static_cast<std::int32_t>(
                        GetMapDirectionBetweenPoints_46B580(0, 0, deltaX, deltaY));
                    x = minusNextX;
                    y = minusNextY;
                } else if (minusBlocked != 0) {
                    if (MergePathTailIntoDiagonalStep_46B5C0(
                            x, y, plusX, plusY, accumulatedX, accumulatedY) != 0) {
                        accumulatedX += plusX;
                        accumulatedY += plusY;
                    } else {
                        const std::uint16_t count = gMovePathCount_C82200;
                        const std::size_t index = static_cast<std::size_t>(
                            std::bit_cast<std::int16_t>(count));
                        // 原+1侧绕行先写X，不能与另一侧的方向优先写序合并。
                        gMovePathX[index] = static_cast<std::uint16_t>(x + plusX);
                        gMovePathDirections[index] =
                            static_cast<std::uint16_t>(plusDirection);
                        gMovePathY[index] = static_cast<std::uint16_t>(y + plusY);
                        gMovePathCount_C82200 = static_cast<std::uint16_t>(count + 1u);
                        accumulatedX = 0;
                        accumulatedY = 0;
                    }
                    deltaX -= plusX;
                    deltaY -= plusY;
                    direction = static_cast<std::int32_t>(
                        GetMapDirectionBetweenPoints_46B580(0, 0, deltaX, deltaY));
                    x = plusNextX;
                    y = plusNextY;
                }
            }
            if (MergePathTailIntoDiagonalStep_46B5C0(x, y, deltaX, deltaY, accumulatedX, accumulatedY) != 0) {
                x += deltaX;
                y += deltaY;
                accumulatedX += deltaX;
                accumulatedY += deltaY;
            } else {
                const std::uint16_t count = gMovePathCount_C82200;
                const std::size_t index = static_cast<std::size_t>(
                    std::bit_cast<std::int16_t>(count));
                x += deltaX;
                y += deltaY;
                gMovePathDirections[index] = static_cast<std::uint16_t>(direction);
                accumulatedX = deltaX;
                gMovePathX[index] = static_cast<std::uint16_t>(x);
                gMovePathY[index] = static_cast<std::uint16_t>(y);
                gMovePathCount_C82200 = static_cast<std::uint16_t>(count + 1u);
                accumulatedY = deltaY;
            }
            advanced = true;
            break;
        }
        if (!advanced) {
            break;
        }
    }

    const std::int32_t remainingX = targetX - x;
    if (remainingX != 0) {
        const std::int32_t step = remainingX > 0 ? 1 : -1;
        for (;;) {
            const std::int32_t nextX = x + step;
            if (IsMapCellBlocked_469810(nextX, y) != 0 || x == targetX) {
                break;
            }
            x = nextX;
            const std::size_t index = static_cast<std::size_t>(
                std::bit_cast<std::int16_t>(gMovePathCount_C82200));
            const std::uint16_t direction =
                static_cast<std::uint16_t>(GetMapDirectionBetweenPoints_46B580(0, 0, step, 0));
            // 轴补走先发布递增后的计数，再写三表，且到达后仍探测一次。
            ++gMovePathCount_C82200;
            gMovePathDirections[index] = direction;
            gMovePathX[index] = static_cast<std::uint16_t>(x);
            gMovePathY[index] = static_cast<std::uint16_t>(y);
        }
    }
    const std::int32_t remainingY = targetY - y;
    if (remainingY != 0) {
        const std::int32_t step = remainingY > 0 ? 1 : -1;
        for (;;) {
            const std::int32_t nextY = y + step;
            if (IsMapCellBlocked_469810(x, nextY) != 0 || y == targetY) {
                break;
            }
            y = nextY;
            const std::size_t index = static_cast<std::size_t>(
                std::bit_cast<std::int16_t>(gMovePathCount_C82200));
            const std::uint16_t direction =
                static_cast<std::uint16_t>(GetMapDirectionBetweenPoints_46B580(0, 0, 0, step));
            ++gMovePathCount_C82200;
            gMovePathDirections[index] = direction;
            gMovePathX[index] = static_cast<std::uint16_t>(x);
            gMovePathY[index] = static_cast<std::uint16_t>(y);
        }
    }
    if (std::bit_cast<std::int16_t>(gMovePathCount_C82200) > 0
        && (x != targetX || y != targetY)) {
        const float differenceY = static_cast<float>(targetY - y);
        const float differenceX = static_cast<float>(targetX - x);
        const std::int32_t direction = static_cast<std::int32_t>(
            GetEightWayMapDirection_46CDF0(differenceX, differenceY));
        gPlayerPathFinalFacingDirection_55D9FC = static_cast<std::uint16_t>(direction);
        return direction;
    }
    return targetY;
}

// 原 [0x46BD20,0x46BD32)：AL 加法不改变先装入 EAX 的 mode 高 24 位。
std::uint32_t __cdecl EncodeMovementDirectionByte_46BD20(
    const std::int32_t value, const std::int32_t mode)
{
    const std::uint8_t low = static_cast<std::uint8_t>(value);
    const std::uint8_t result =
        static_cast<std::uint8_t>(low + (mode == 0 ? 0x61 : 0x41));
    return (static_cast<std::uint32_t>(mode) & 0xFFFFFF00U) | result;
}

// 原 [0x50C8D0,0x50C8F5)：先采独立 W32，再传文本、Y、X 与当前 socket。
std::int32_t __cdecl SendCurrentPlayerMovementPath_50C8D0(
    const std::int32_t x, const std::int32_t y,
    const char* const text)
{
    const std::int32_t fourth = std::bit_cast<std::int32_t>(gValue9EE8F8);
    return network::SendPlayerMovementPath_510570(
        platform::gConnectionSocket, x, y, cg::text::FromGbkWide(text), fourth);
}

// 原 [0x4D3170,0x4D317B)：仅将 0x581800 写为 -1 后返回。
void ResetCurrentDialogType_4D3170()
{
    gCurrentDialogType_581800 = -1;
}

} // namespace detail

std::uint32_t gActorBarsEnabled5AC490 = 0; // 原全局：0x005AC490。

namespace detail {

std::int16_t ReadPetFollowIndex(const std::size_t position) noexcept
{
    return std::bit_cast<std::int16_t>(gPetFollowOrder[position]);
}

// 原函数：0x0046A990。
void RebuildPartyFollowOrder_46A990()
{
    if (gValueFF0D36 == 0) {
        return;
    }
    gPetFollowOrder.fill(0xFFFFu);
    std::size_t output = 0;
    for (std::size_t index = 0; index < gPetMapCharaSlots.size(); ++index) {
        const auto& slot = gPetMapCharaSlots[index];
        if (slot.flags != 0 && slot.action != nullptr) {
            gPetFollowOrder[output] = static_cast<std::uint16_t>(index);
            ++output;
        }
    }
}

void ResetMapFollowersFromPath(cg::yobi::YobiAction* const leader)
{
    for (std::int32_t i = 1; i < 5; ++i) {
        auto& slot = gPetMapCharaSlots[static_cast<std::size_t>(i)];
        if (slot.flags != 0) {
            const bool isPlayer = slot.objectIndex == gPlayerRuntimeSlots.objectIndex;
            cg::yobi::ResetMapFollowerFromPath(
                leader, i, slot.action, isPlayer, &gPlayerRuntimeSlots.direction);
        }
    }
}

// 原函数：0x0046AFE0。
void PropagatePartyFollowerPath_46AFE0(const std::int32_t index, std::int32_t x, std::int32_t y)
{
    cg::yobi::YobiAction* const first = gPetMapCharaSlots[0].action;
    if (first == nullptr || !cg::yobi::IsMapActorIdleAtTarget(first)) {
        return;
    }
    std::int32_t previous = index - 1;
    for (; previous >= 0; --previous) {
        const auto& slot = gPetMapCharaSlots[static_cast<std::size_t>(previous)];
        if (slot.flags != 0 && slot.action != nullptr) {
            break;
        }
    }
    if (previous >= 0 && cg::yobi::AreMapActorTargetsAdjacent(
            gPetMapCharaSlots[static_cast<std::size_t>(previous)].action,
            gPetMapCharaSlots[static_cast<std::size_t>(index)].action)) {
        return;
    }
    gPartyCatchUpRateBits_9DAD78 = 0x3F800000u;
    for (std::int32_t i = index; i < 5; ++i) {
        auto& slot = gPetMapCharaSlots[static_cast<std::size_t>(i)];
        if (slot.flags != 0) {
            cg::yobi::YobiAction* const action = slot.action;
            if (action != nullptr) {
                std::int32_t nextX;
                std::int32_t nextY;
                cg::yobi::ReadMapActorTarget(action, slot.action, &nextX, &nextY);
                cg::yobi::AppendMapActorPathPoint_441F60(action, x, y);
                x = nextX;
                y = nextY;
            }
        }
    }
}

// 原函数：0x0051B0F0。
void DetachPartyMapActions_51B0F0()
{
    for (auto& slot : gPetMapCharaSlots) {
        slot.action = nullptr;
    }
}

// 原函数：0x0051B080。
cg::yobi::YobiAction* ClearPartyMembershipState_51B080()
{
    for (auto& slot : gPetMapCharaSlots) {
        if (slot.flags != 0) {
            if (slot.objectIndex == gPlayerRuntimeSlots.objectIndex) {
                cg::yobi::YobiAction* const action = slot.action;
                if (action != nullptr) {
                    cg::yobi::ClearMapActorPathCount(action);
                }
                ClearPlayerPartyMembership_51A5F0();
            } else {
                const std::uint32_t flags = ReadPlayerFlags18A();
                if ((flags & 0x100u) != 0) {
                    cg::yobi::YobiAction* const action = slot.action;
                    if (action != nullptr) {
                        cg::yobi::ClearMapActorPathCount(action);
                    }
                }
                ClearWorldActorPartyMembership_442C70(slot.action);
            }
        }
        slot.flags = 0;
        slot.objectIndex = 0;
        slot.action = nullptr;
    }
    return ClearPlayerLeaderMarker_51A5B0();
}

// 原函数：0x0046A5A0。
void AppendFirstFollowerPathPoint_46A5A0(const std::int32_t x, const std::int32_t y)
{
    if (gValueFF0D36 == 0) {
        return;
    }
    // 原FDA88A一次W32取样，复用同一已证跨字段边界。
    const std::uint32_t flags = ReadPlayerFlags18A();
    if ((flags & 0x100u) == 0) {
        return;
    }
    const std::int16_t index = std::bit_cast<std::int16_t>(gPetFollowOrder[1]);
    if (index < 0) {
        return;
    }
    cg::yobi::YobiAction* const action =
        gPetMapCharaSlots[static_cast<std::size_t>(index)].action;
    cg::yobi::AppendMapActorPathPoint_441F60(action, x, y);
}

// 原函数：0x0046A940。
void AdvancePlayerLeaderFollowers_46A940()
{
    if (gValueFF0D36 == 0) {
        return;
    }
    const std::uint32_t flags = ReadPlayerFlags18A();
    if ((flags & 0x100u) == 0) {
        return;
    }
    for (std::size_t position = 1; position < gPetFollowOrder.size(); ++position) {
        const std::int16_t index =
            std::bit_cast<std::int16_t>(gPetFollowOrder[position]);
        if (index < 0) {
            break;
        }
        cg::yobi::YobiAction* const action =
            gPetMapCharaSlots[static_cast<std::size_t>(index)].action;
        cg::yobi::StepMapActorMovement_442730(action);
    }
}

// 原函数：0x0046F680。
std::uint8_t UpdateMapMouseTargetAndButtons_46F680()
{
    const float mouseY = static_cast<float>(
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY));
    const float mouseX = static_cast<float>(
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX));
    float projectedX;
    float projectedY;
    static_cast<void>(
        ProjectScreenToWorld_46CE80(mouseX, mouseY, &projectedX, &projectedY));
    const std::int32_t pixelX = static_cast<std::int32_t>(
        static_cast<std::int64_t>(projectedX + 0.5));
    const double biasedY = projectedY + 0.5;
    gMouseWorldPixelX_C8A78C = pixelX;
    const std::int32_t pixelY =
        static_cast<std::int32_t>(static_cast<std::int64_t>(biasedY));
    const std::int32_t adjustedX = pixelX + 32;
    gMouseWorldPixelY_C8A790 = pixelY;
    const std::int32_t targetY = (pixelY + 32) / 64;
    gPlayerMoveTargetY = targetY;
    const std::int32_t snappedY = targetY * 64;
    const std::int32_t targetX = adjustedX / 64;
    gPlayerMoveTargetX = targetX;
    const float gridY = static_cast<float>(snappedY);
    const float gridX = static_cast<float>(targetX * 64);
    ProjectWorldToScreen_46CE30(gridX, gridY, &projectedX, &projectedY);
    gMapTargetLeftClick_C8A7A0 = 0;
    gMapTargetLeftHeld_C8A7A4 = 0;
    gMapTargetInteractionClick_C8A7A8 = 0;
    gMapTargetRightHeld_C8A7AC = 0;

    if ((platform::gLegacyMouseState.mouseY < platform::gWindowHeight - 24)
        && ((*reinterpret_cast<const unsigned char*>(
                    &platform::gLegacyMouseState.valueD68DB4) < 212
                && ui::WindowItemDrag_ReadIndex() < 0)
            || gValueC8A780 != 0 || gValue62B308 != 0 || gValue62B398 != 0)
        && gContinuousMapMovementActive_C8A79C == 0) {
        render::GraphicDescriptor16 descriptor {};
        const std::uint32_t color = 0xFFFFFFFFu;
        descriptor.color = color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
        const std::int32_t drawY = static_cast<std::int32_t>(
            static_cast<std::int64_t>(projectedY + 0.5));
        const std::int32_t drawX = static_cast<std::int32_t>(
            static_cast<std::int64_t>(projectedX + 0.5));
        static_cast<void>(
            cg::render::DrawGraphic(drawX, drawY, 100, 0x3BE99, 0, &descriptor));
    }
    const bool mouseBelowLimit = (platform::gLegacyMouseState.mouseY < platform::gWindowHeight - 24);
    if (!mouseBelowLimit
        || !((*reinterpret_cast<const unsigned char*>(
                    &platform::gLegacyMouseState.valueD68DB4) < 212
                && ui::WindowItemDrag_ReadIndex() < 0)
            || gValueC8A77C != 0 || gValue62B308 != 0 || gValue62B398 != 0)) {
        gMapLeftHoldDurationMs_C8A778 = 0;
        gMapLeftPressTick_C8A794 = 0;
        return static_cast<std::uint8_t>(mouseBelowLimit);
    }

    const auto leftClick = *reinterpret_cast<const unsigned char*>(
        &platform::gLegacyMouseState.edgeButtons);
    if ((leftClick & 1u) != 0 && gMapLeftButtonSuppressed_C8A774 == 0) {
        gMapTargetLeftClick_C8A7A0 = 1;
        gMapLeftHoldDurationMs_C8A778 = 0;
        gMapLeftPressTick_C8A794 = platform::ReadTickCount64();
    }
    if ((*reinterpret_cast<const unsigned char*>(
            &platform::gLegacyMouseState.heldButtons) & 1u) != 0
        && gMapLeftButtonSuppressed_C8A774 == 0) {
        const std::uint64_t baseline = gMapLeftPressTick_C8A794;
        gMapTargetLeftHeld_C8A7A4 = 1;
        if (baseline != 0) {
            const std::uint64_t now = platform::ReadTickCount64();
            gMapLeftHoldDurationMs_C8A778 = now - gMapLeftPressTick_C8A794;
        }
    } else {
        gMapLeftHoldDurationMs_C8A778 = 0;
        gMapLeftPressTick_C8A794 = 0;
    }
    const auto rightClick = *reinterpret_cast<const unsigned char*>(
        &platform::gLegacyMouseState.edgeButtons);
    if ((rightClick & 2u) != 0 && gMapTargetInteractionClick_C8A7A8 == 0) {
        gMapTargetInteractionClick_C8A7A8 = 1;
    }
    if ((*reinterpret_cast<const unsigned char*>(
            &platform::gLegacyMouseState.heldButtons) & 2u) != 0
        && gMapTargetInteractionClick_C8A7A8 == 0) {
        gMapTargetRightHeld_C8A7AC = 1;
    }
    return 2;
}

// 原函数：0x00455A90。
std::int32_t IsMapBattleSpectatingAllowed_455A90()
{
    return 1; // G05：取消地图观战白名单，调用方其余条件保持。
}

// 原函数：0x004FA8E0。
std::uintptr_t RequestEmoteWithResult_4FA8E0(const std::int32_t index)
{
    cg::yobi::YobiAction* const initialAction = gPlayerRuntimeSlots.action;
    if (initialAction == nullptr || gCurrentSceneType == 10
        || gMapTransitionRequestPending_9A86D8 != 0 || gDirectionalMapTransitionPending_9FB92E != 0) {
        return reinterpret_cast<std::uintptr_t>(initialAction);
    }
    // 原401D00仅改AL，保留读取值高24位；只能读取一次，避免额外计数/RNG。
    const std::int32_t comparisonValue = static_cast<std::int32_t>(gPlayerCoordinateSyncPending_C821D8);
    const std::uint32_t comparisonResult = (std::bit_cast<std::uint32_t>(comparisonValue) & 0xFFFFFF00u)
        | static_cast<std::uint32_t>(comparisonValue == 0);
    if (comparisonValue != 0 || index < 0) {
        return static_cast<std::uintptr_t>(comparisonResult);
    }
    const std::uint64_t now = platform::ReadTickCount64();
    if (gEmoteRequestTickD67D20 + 500u >= now) {
        return static_cast<std::uintptr_t>(now);
    }
    cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
    bool useLimitedActions = cg::yobi::ReadMapActorAttachment44(action) != nullptr;
    if (!useLimitedActions) {
        const std::int32_t image = cg::yobi::GetYobiActionField2A8(action);
        useLimitedActions = !((image >= 0x186A0 && image <= 0x18A87)
            || (image >= 0x19A28 && image <= 0x1A130)
            || (image >= 0x1AF0E && image <= 0x1AF25)
            || (image >= 0x197D0 && image <= 0x19848)
            || (image >= 0x1D524 && image <= 0x1D553)
            || (image >= 0x1D55C && image <= 0x1D563));
    }
    std::int32_t selected = index;
    if (useLimitedActions
        && (selected == 0 || selected == 2 || selected == 4 || selected == 6
            || selected == 8 || selected == 10 || selected == 12 || selected == 14
            || selected == 16 || selected == 17)) {
        selected = 1;
    }
    const std::int32_t protocolAction = kEmoteProtocolActions584BB4[selected];
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    static_cast<void>(network::SendWorldActionRequest_511DD0(
        platform::gConnectionSocket, x, y, protocolAction));
    const std::int32_t localAction = kEmoteLocalActions584B6C[selected];
    SetPlayerAnimationState_51A060(localAction);
    const std::uint64_t completedAt = platform::ReadTickCount64();
    gEmoteRequestTickD67D20 = completedAt;
    return static_cast<std::uintptr_t>(completedAt);
}

// NFI01：邻格fIM生产，保两轴双读/F32桥/两tick和先Y后X；现发送普通空格，父主体保持。
// 原函数：0x0051B3E0。
std::uint32_t RequestAdjacentWorldInteraction_51B3E0()
{
    std::int32_t result = gCurrentDialogType_581800;
    if (result != -1) {
        return std::bit_cast<std::uint32_t>(result);
    }
    const std::int32_t signX =
        (gPlayerX - static_cast<std::int32_t>(gPlayerMoveTargetX));
    result = (gPlayerX - static_cast<std::int32_t>(gPlayerMoveTargetX));
    if (signX < 0) {
        result = -result;
    }
    if (result > 1) {
        return std::bit_cast<std::uint32_t>(result);
    }
    const std::int32_t signY =
        (gPlayerY - static_cast<std::int32_t>(gPlayerMoveTargetY));
    result = (gPlayerY - static_cast<std::int32_t>(gPlayerMoveTargetY));
    if (signY < 0) {
        result = -result;
    }
    if (result > 1) {
        return std::bit_cast<std::uint32_t>(result);
    }
    const std::int32_t targetY = gPlayerMoveTargetY;
    const std::int32_t targetX = gPlayerMoveTargetX;
    result = HasWorldObjectAt_4450D0(targetX, targetY, 7);
    if (result == 0) {
        return 0;
    }
    const std::int32_t deltaX =
        (static_cast<std::int32_t>(gPlayerMoveTargetX) - gPlayerX);
    const std::int32_t secondTargetY = gPlayerMoveTargetY;
    const float differenceX = static_cast<float>(deltaX);
    const std::int32_t deltaY =
        (static_cast<std::int32_t>(secondTargetY) - gPlayerY);
    const float differenceY = static_cast<float>(deltaY);
    const std::int32_t direction = static_cast<std::int32_t>(
        GetEightWayMapDirection_46CDF0(differenceX, differenceY));
    const std::uint64_t now = platform::ReadTickCount64();
    if (gValueFF0D38 + 500u >= now) {
        return static_cast<std::uint32_t>(now);
    }
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    static_cast<void>(network::SendPositionDirectionInteraction_510B80(platform::gConnectionSocket, x, y, direction));
    const std::uint64_t sentAt = platform::ReadTickCount64();
    gValueFF0D38 = sentAt;
    return static_cast<std::uint32_t>(sentAt);
}

// 原函数：0x0046B370。
void TurnPlayerTowardCursorTarget_46B370()
{
    if (gPlayerRuntimeSlots.action == nullptr) {
        return;
    }
    const float movementX = std::bit_cast<float>(gPlayerMovementStepXBits_9EE8FC);
    if (movementX < 0.0F || movementX > 0.0F) {
        return;
    }
    const float movementY = std::bit_cast<float>(gPlayerMovementStepYBits_9EE900);
    if (movementY < 0.0F || movementY > 0.0F
        || gValue62B5AA != 0 || gMapMovementInputBlocked_C82220 != 0) {
        return;
    }
    if ((gPlayerX == static_cast<std::int32_t>(gPlayerMoveTargetX))
        && (gPlayerY == static_cast<std::int32_t>(gPlayerMoveTargetY))) {
        return;
    }
    const std::int32_t deltaX =
        (static_cast<std::int32_t>(gPlayerMoveTargetX) - gPlayerX);
    const std::int32_t targetY = gPlayerMoveTargetY;
    const float differenceX = static_cast<float>(deltaX);
    const std::int32_t deltaY =
        (static_cast<std::int32_t>(targetY) - gPlayerY);
    const float differenceY = static_cast<float>(deltaY);
    const std::int32_t direction = static_cast<std::int32_t>(
        GetEightWayMapDirection_46CDF0(differenceX, differenceY));
    const std::uint64_t now = platform::ReadTickCount64();
    if (gLastCursorFacingRequestTick_C82238 + 500u >= now) {
        return;
    }
    static_cast<void>(SetPlayerFacingDirection_51A040(direction));
    const std::uint32_t encoded = EncodeMovementDirectionByte_46BD20(direction, 1);
    const char text[2] {
        std::bit_cast<char>(static_cast<std::uint8_t>(encoded)), '\0',
    };
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    static_cast<void>(SendCurrentPlayerMovementPath_50C8D0(x, y, text));
    gLastCursorFacingRequestTick_C82238 = platform::ReadTickCount64();
}

// 原函数：0x00469C90。
std::uint8_t UpdateWorldMapInputAndMovement_469C90()
{
    const bool pendingIsZero =
        (gPlayerCoordinateSyncPending_C821D8 == 0);
    if (pendingIsZero && gMapTransitionRequestPending_9A86D8 == 0 && gDirectionalMapTransitionPending_9FB92E == 0) {
        gMapMovementInputBlocked_C82220 = 0;
    }
    if ((ReadPlayerActionFlagsByte() & 0x80u) != 0) {
        return static_cast<std::uint8_t>(pendingIsZero);
    }
    if (gMapTargetInteractionClick_C8A7A8 != 0 && ui::WindowRuntime_ReadUiGate() == 0) {
        const std::int32_t targetY = gPlayerMoveTargetY;
        const std::int32_t targetX = gPlayerMoveTargetX;
        const std::int32_t objectIndex = FindWorldObjectAtWithActionFlags_445030(targetX, targetY, 8, 0x1000);
        if (objectIndex >= 0) {
            static_cast<void>(ui::WindowStallBrowse_RequestClose());
            static_cast<void>(ui::WindowOwnStall_RequestClose());
            static_cast<void>(network::RequestStallContents_512190(platform::gConnectionSocket, objectIndex));
        }
        TurnPlayerTowardCursorTarget_46B370();
        static_cast<void>(RequestAdjacentWorldInteraction_51B3E0());
        const std::int32_t secondY = gPlayerMoveTargetY;
        const std::int32_t secondX = gPlayerMoveTargetX;
        const std::int32_t secondIndex = FindWorldObjectAt_445160(secondX, secondY, 8);
        if (secondIndex != -1) {
            static_cast<void>(network::RequestPlayerProfile_511760(
                platform::gConnectionSocket, 3, secondIndex));
        }
    }
    if ((gValueFF0D36 == 0 || (ReadPlayerFlags18A() & 0x100u) != 0)
        && gValue62B5AA == 0 && gMapMovementInputBlocked_C82220 == 0 && ui::WindowRuntime_ReadUiGate() == 0) {
        const bool forceContinuousMove = gMapLeftHoldDurationMs_C8A778 >= 500u;
        if (forceContinuousMove) {
            gContinuousMapMovementActive_C8A79C = 1;
        }
        if (forceContinuousMove || gContinuousMapMovementActive_C8A79C == 1) {
            const auto click = *reinterpret_cast<const unsigned char*>(
                &platform::gLegacyMouseState.edgeButtons);
            if ((click & 1u) != 0) {
                gContinuousMapMovementActive_C8A79C = 0;
            } else {
                const std::uint64_t now = platform::ReadTickCount64();
                if (gLastMapMoveTargetTick_55DDAC + 250u <= now) {
                    const std::uint64_t sampledTick = platform::ReadTickCount64();
                    const std::int32_t targetX = gPlayerMoveTargetX;
                    gLastMapMoveTargetTick_55DDAC = sampledTick;
                    gMoveTargetX9A7F88 = static_cast<std::int32_t>(targetX);
                    gMoveTargetY9A7F98 = static_cast<std::int32_t>(gPlayerMoveTargetY);
                    gPlayerPathRebuildPending_C821FC = 1;
                }
                if ((static_cast<std::int32_t>(gPlayerMoveTargetX) - gPlayerX) != 0
                    || (static_cast<std::int32_t>(gPlayerMoveTargetY) - gPlayerY) != 0) {
                    const std::int32_t deltaY =
                        (static_cast<std::int32_t>(gPlayerMoveTargetY) - gPlayerY);
                    const std::int32_t targetX = gPlayerMoveTargetX;
                    const float differenceY = static_cast<float>(deltaY);
                    const std::int32_t deltaX =
                        (static_cast<std::int32_t>(targetX) - gPlayerX);
                    const float differenceX = static_cast<float>(deltaX);
                    const std::int32_t direction = static_cast<std::int32_t>(
                        GetEightWayMapDirection_46CDF0(differenceX, differenceY));
                    static_cast<void>(platform::RequestCursorState_506DE0(direction + 3, 0));
                }
            }
        } else if (gMapTargetLeftClick_C8A7A0 != 0) {
            const std::uint64_t now = platform::ReadTickCount64();
            if (gLastMapMoveTargetTick_55DDAC + 250u <= now) {
                // 单次路径先发布tick，再采X；两个临时量的载荷写序也不同。
                gLastMapMoveTargetTick_55DDAC = platform::ReadTickCount64();
                gMoveTargetX9A7F88 = static_cast<std::int32_t>(gPlayerMoveTargetX);
                gMoveTargetY9A7F98 = static_cast<std::int32_t>(gPlayerMoveTargetY);
                gPlayerPathRebuildPending_C821FC = 1;
            }
        }
    }
    RebuildPartyFollowOrder_46A990();
    if (gValueFF0D36 != 0 && (ReadPlayerFlags18A() & 0x100u) == 0) {
        return static_cast<std::uint8_t>(UpdatePartyFollowMovement_46A190());
    }
    return static_cast<std::uint8_t>(UpdatePlayerPathMovement_469FC0());
}

// 原函数：0x00469FC0。
std::uint16_t UpdatePlayerPathMovement_469FC0()
{
    const std::uint32_t stopped = gWaitingForMapRectangleData_9EE8B8;
    gPartyCatchUpRateBits_9DAD78 = 0x3F800000u;
    if (stopped != 0) {
        return static_cast<std::uint16_t>(stopped);
    }
    if ((gPlayerCoordinateSyncPending_C821D8 == 0)
        && gMapTransitionRequestPending_9A86D8 == 0 && gDirectionalMapTransitionPending_9FB92E == 0) {
        const std::int32_t pending = DispatchPendingPathEvent_46AB60();
        if (pending != 0) {
            return static_cast<std::uint16_t>(pending);
        }
        const double targetX = static_cast<std::int32_t>(gPlayerSnapshotX) * 64.0;
        const std::uint32_t worldXBits = gWorldCurrentXBits;
        const float worldX = std::bit_cast<float>(worldXBits);
        if (!(targetX < worldX || targetX > worldX)) {
            const double targetY = static_cast<std::int32_t>(gPlayerSnapshotY) * 64.0;
            const std::uint32_t worldYBits = gWorldCurrentYBits;
            const float worldY = std::bit_cast<float>(worldYBits);
            if (!(targetY < worldY || targetY > worldY)) {
                if (gPlayerPathRebuildPending_C821FC != 0 && gPendingDirectionCount_C82202 == 0) {
                    gPlayerPathRebuildPending_C821FC = 0;
                    static_cast<void>(BuildPlayerMovementPath_46B6F0());
                }
                // 原两次非零队列分支复用刚比较的W16，回调后才重新取样。
                const std::int16_t queuedCount = []() -> std::int16_t {
                    std::int16_t pathCount = std::bit_cast<std::int16_t>(gMovePathCount_C82200);
                    if (pathCount == 0) {
                        const std::int16_t pendingCount =
                            std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202);
                        if (pendingCount != 0) {
                            return pendingCount;
                        }
                        TurnPlayerAndSendDirection_46B4B0(std::bit_cast<std::int16_t>(gPlayerPathFinalFacingDirection_55D9FC));
                        pathCount = std::bit_cast<std::int16_t>(gMovePathCount_C82200);
                        gPlayerPathFinalFacingDirection_55D9FC = 0xFFFFu;
                    }
                    if (pathCount > 0) {
                        const std::int16_t pendingCount =
                            std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202);
                        if (pendingCount != 0) {
                            return pendingCount;
                        }
                        QueueNextMovementDirections_46ABD0();
                        if (std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202) > 0) {
                            static_cast<void>(DetectPendingPathEvents_46AA00());
                        }
                        static_cast<void>(ui::WindowStallBrowse_RequestClose());
                        static_cast<void>(SendPendingMovementDirections_46AC10());
                    }
                    return std::bit_cast<std::int16_t>(gPendingDirectionCount_C82202);
                }();
                if (queuedCount > 0) {
                    static_cast<void>(RequestMissingMapRectanglesAhead_46AF30());
                    const std::int16_t direction =
                        std::bit_cast<std::int16_t>(gPendingDirections9A86D4[0]);
                    PopPendingMovementDirection_46BCF0();
                    const std::int32_t speed = gPlayerRuntimeSlots.walkSpeed;
                    const std::int16_t deltaY =
                        gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].y;
                    // 原401E00忽略实参高W16；复用单次读取的加法与S16扩展。
                    const std::int32_t nextY =
                        (gPlayerY + static_cast<std::int32_t>(deltaY));
                    const std::int16_t deltaX =
                        gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].x;
                    const std::int32_t nextX =
                        (gPlayerX + static_cast<std::int32_t>(deltaX));
                    MovePlayerOneStep_46B1B0(nextX, nextY, speed);
                    const std::int32_t y =
                        static_cast<std::int32_t>(gPlayerY);
                    const std::int32_t x =
                        static_cast<std::int32_t>(gPlayerX);
                    AppendFirstFollowerPathPoint_46A5A0(x, y);
                }
            }
        }
    }
    AdvanceFollowersBehindPlayerLeader_46A5F0();
    const std::uint32_t stoppedAfterFollow = gWaitingForMapRectangleData_9EE8B8;
    gPlayerReachedMovementTarget_C8222E = 0;
    if (stoppedAfterFollow != 0) {
        return static_cast<std::uint16_t>(stoppedAfterFollow);
    }
    AdvancePlayerWorldMovement_46A700();
    AdvancePlayerLeaderFollowers_46A940();
    const std::uint16_t result = UpdatePlayerMapWindow_468200();
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    gWorldProjectionReferenceXBits = worldXBits;
    gWorldProjectionReferenceYBits = worldYBits;
    return result;
}

// 原函数：0x0046A410。
void SetPlayerMovementTarget_46A410(
    const std::int32_t x, const std::int32_t y, const std::int32_t speed)
{
    gPlayerSnapshotX = static_cast<std::int32_t>(x);
    gPlayerSnapshotY = static_cast<std::int32_t>(y);

    const std::int32_t pixelX =
        (gPlayerSnapshotX * 64);
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const float differenceX = static_cast<float>(
        static_cast<double>(pixelX) - std::bit_cast<float>(worldXBits));
    const std::int32_t pixelY =
        (gPlayerSnapshotY * 64);
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    const platform::detail::MapMoveDifference difference {
        differenceX,
        static_cast<double>(pixelY) - std::bit_cast<float>(worldYBits),
    };
    platform::detail::MapMoveDirection direction;
    platform::detail::Cg2dMapMoveDirection(&difference, &direction);
    if (gPlayerRuntimeSlots.action != nullptr) {
        const double rate = speed;
        const float percent = 0.01F;
        gPlayerMovementStepXBits_9EE8FC = std::bit_cast<std::uint32_t>(
            static_cast<float>(rate * direction.unroundedX * 4.0F * percent));
        gPlayerMovementStepYBits_9EE900 = std::bit_cast<std::uint32_t>(
            static_cast<float>(rate * direction.y * 4.0F * percent));
    } else {
        gPlayerMovementStepXBits_9EE8FC = std::bit_cast<std::uint32_t>(
            static_cast<float>(direction.unroundedX * 4.0F));
        gPlayerMovementStepYBits_9EE900 = std::bit_cast<std::uint32_t>(
            static_cast<float>(direction.y * 4.0));
    }
    if (direction.unroundedX < 0.0 || direction.unroundedX > 0.0
        || direction.y < 0.0F || direction.y > 0.0F) {
        const auto value = static_cast<std::int32_t>(
            GetEightWayMapDirection_46CDF0(direction.x, direction.y));
        static_cast<void>(SetPlayerFacingDirection_51A040(value));
        static_cast<void>(MarkPlayerMovementActive_51A640());
    }
    SynchronizePlayerMapActionPosition_519EE0();
}

// 原函数：0x0046A700。
void AdvancePlayerWorldMovement_46A700()
{
    static_cast<void>(GetPlayerMapCoordinateSum_46CEC0());
    const float movementX = std::bit_cast<float>(gPlayerMovementStepXBits_9EE8FC);
    bool idle = !(movementX < 0.0F || movementX > 0.0F);
    if (idle) {
        const float movementY = std::bit_cast<float>(gPlayerMovementStepYBits_9EE900);
        idle = !(movementY < 0.0F || movementY > 0.0F);
    }
    if (idle) {
        if (ReadPlayerMovementActive_51A680() == 1) {
            SetPlayerAnimationState_51A060(0);
            static_cast<void>(ClearPlayerMovementActive_51A660());
        }
        gMapCharaSceneSlots.value55DE98 = 0xFFFFu;
        static_cast<void>(StorePlayerMapCoordinateSum_46CEE0());
        SynchronizePlayerMapActionPosition_519EE0();
        return;
    }

    const float targetX = static_cast<float>(static_cast<std::int32_t>(gPlayerSnapshotX) * 64.0);
    const double scaledTargetY = static_cast<std::int32_t>(gPlayerSnapshotY) * 64.0;
    const std::uint32_t worldYBits = gWorldCurrentYBits;
    const float targetY = static_cast<float>(scaledTargetY);
    const float rateX = std::bit_cast<float>(gPartyCatchUpRateBits_9DAD78);
    const float rawStepX = std::bit_cast<float>(gPlayerMovementStepXBits_9EE8FC);
    const double scaledX = static_cast<double>(rateX) * rawStepX;
    const std::uint32_t worldXBits = gWorldCurrentXBits;
    const float stepX = static_cast<float>(scaledX);
    const float rateY = std::bit_cast<float>(gPartyCatchUpRateBits_9DAD78);
    const float rawStepY = std::bit_cast<float>(gPlayerMovementStepYBits_9EE900);
    const float stepY = static_cast<float>(static_cast<double>(rateY) * rawStepY);
    const double distance = GetSquaredWorldDistance_46FB80(
        std::bit_cast<float>(worldXBits), std::bit_cast<float>(worldYBits),
        targetX, targetY);
    const double squaredY = static_cast<double>(stepY) * stepY;
    const double squaredX = static_cast<double>(stepX) * stepX;
    const double squaredStep = squaredY + squaredX;
    if (!(squaredStep >= distance)) {
        const std::uint32_t currentXBits = gWorldCurrentXBits;
        gWorldCurrentXBits = std::bit_cast<std::uint32_t>(static_cast<float>(
            static_cast<double>(std::bit_cast<float>(currentXBits)) + stepX));
        const std::uint32_t currentYBits = gWorldCurrentYBits;
        gWorldCurrentYBits = std::bit_cast<std::uint32_t>(static_cast<float>(
            static_cast<double>(std::bit_cast<float>(currentYBits)) + stepY));
    } else {
        // 原先计算X格值，但先发布世界坐标/移动槽/C8222E，再截断并构造临时量。
        const double gridX = targetX * 0.015625;
        gWorldCurrentXBits = std::bit_cast<std::uint32_t>(targetX);
        gWorldCurrentYBits = std::bit_cast<std::uint32_t>(targetY);
        gPlayerMovementStepXBits_9EE8FC = 0;
        gPlayerMovementStepYBits_9EE900 = 0;
        gPlayerReachedMovementTarget_C8222E = 1;
        const auto x = static_cast<std::int32_t>(static_cast<std::int64_t>(gridX));
        gPlayerX = static_cast<std::int32_t>(x);

        const std::uint32_t currentYBits = gWorldCurrentYBits;
        const auto y = static_cast<std::int32_t>(static_cast<std::int64_t>(
            std::bit_cast<float>(currentYBits) * 0.015625));
        gPlayerY = static_cast<std::int32_t>(y);

        const std::int32_t count = gPlayerRuntimeSlots.value174;
        if (count > 0) {
            const std::int32_t remaining = count - 1;
            gPlayerRuntimeSlots.value174 = remaining;
            if (remaining <= 0) {
                cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
                if (action != nullptr) {
                    cg::yobi::BeginMapActorPhase2(action);
                    const std::int32_t snapshotY =
                        static_cast<std::int32_t>(gPlayerSnapshotY);
                    const std::int32_t snapshotX =
                        static_cast<std::int32_t>(gPlayerSnapshotX);
                    static_cast<void>(network::SendWorldActionRequest_511DD0(
                        platform::gConnectionSocket, snapshotX, snapshotY, 70));
                }
            }
        }
    }
    SetPlayerAnimationState_51A060(1);
    static_cast<void>(MarkPlayerMovementActive_51A640());
    cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
    if (action != nullptr) {
        gMapCharaSceneSlots.value55DE98 =
            cg::yobi::ReadYobiDirectionLow16(action);
    }
    static_cast<void>(StorePlayerMapCoordinateSum_46CEE0());
    SynchronizePlayerMapActionPosition_519EE0();
}

// 原函数：0x00519A80。
void CreatePlayerMapPresence_519A80()
{
    const std::int32_t direction = gPlayerRuntimeSlots.direction;
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    CreateOrUpdatePlayerMapActor_519CD0(gPlayerRuntimeSlots.imageNo, x, y, direction);
    const auto flags = static_cast<std::uint8_t>(gPlayerRuntimeSlots.flags18A);
    const std::int32_t flag40 = (flags & 0x40u) != 0;
    const std::int32_t flag80 = (flags & 0x80u) != 0;

    char escaped[256];
    char titles[256];
    cg::core::NullSub_509080();
    input::EscapeGbkText(
        gPlayerRuntimeSlots.ownTitle.data(), escaped, 256);
    std::strcpy(titles, escaped);
    std::strcat(titles, "|");
    cg::core::NullSub_509080();
    input::EscapeGbkText(
        gPlayerRuntimeSlots.text12A.data(), escaped, 256);
    std::strcat(titles, escaped);
    std::strcat(titles, "|");
    cg::core::NullSub_509080();
    input::EscapeGbkText(
        gPlayerRuntimeSlots.text14B.data(), escaped, 256);
    std::strcat(titles, escaped);
    const std::int32_t value188 =
        std::bit_cast<std::int16_t>(gPlayerRuntimeSlots.value188);
    const std::int32_t level = gPlayerRuntimeSlots.level;
    SetPlayerMapIdentity_51A090(
        gPlayerRuntimeSlots.name.data(), titles, level, value188, flag40, flag80);

    cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
    if (action != nullptr) {
        cg::yobi::CopyYobiPlayerHp(action, &gPlayerRuntimeSlots.hp);
        cg::yobi::CopyYobiPlayerMaxHp(
            gPlayerRuntimeSlots.action, &gPlayerRuntimeSlots.maxHp);
        const std::uint32_t objectIndex = gPlayerRuntimeSlots.objectIndex;
        for (auto& slot : gPetMapCharaSlots) {
            if (slot.flags != 0 && slot.objectIndex == objectIndex) {
                slot.action = gPlayerRuntimeSlots.action;
                break;
            }
        }
    }
    gValueFF0D30 = 0;
    gValueFF0D32 = 0;
    gValueF7CA04 = 0;
    gValueFF03D8 = 0;
    gValueFF0D34 = 0;
}

// 原函数：0x0051A090。
cg::yobi::YobiAction* SetPlayerMapIdentity_51A090(
    const char* const name, const char* const titles, const std::int32_t level,
    const std::int32_t value188, const std::int32_t flag40,
    const std::int32_t flag80)
{
    const auto nameLength = static_cast<std::int32_t>(std::strlen(name));
    if (nameLength <= 16) {
        // 519A80 的源就是本槽；原 rep movs 允许同源复制。
        std::memmove(gPlayerRuntimeSlots.name.data(), name, std::strlen(name) + 1);
    }

    char first[256];
    char second[256];
    char third[256];
    char fourth[256];
    network::ExtractDelimitedField_52CED0(titles, '|', 1, 255, first);
    input::UnescapeLegacyTextAndNotify_52D2F0(first);
    if (std::strlen(first) <= 16) {
        std::strcpy(gPlayerRuntimeSlots.ownTitle.data(), first);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 2, 255, second);
    input::UnescapeLegacyTextAndNotify_52D2F0(second);
    if (std::strlen(second) <= 32) {
        std::strcpy(gPlayerRuntimeSlots.text12A.data(), second);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 3, 255, third);
    input::UnescapeLegacyTextAndNotify_52D2F0(third);
    if (std::strlen(third) <= 32) {
        std::strcpy(gPlayerRuntimeSlots.text14B.data(), third);
    }
    network::ExtractDelimitedField_52CED0(titles, '|', 4, 255, fourth);
    input::UnescapeLegacyTextAndNotify_52D2F0(fourth);
    if (std::strlen(fourth) <= 16) {
        std::strcpy(gPlayerRuntimeSlots.text15BB8.data(), fourth);
    }
    gPlayerRuntimeSlots.level = level;
    gPlayerRuntimeSlots.value188 = static_cast<std::uint16_t>(value188);
    if (flag40 != 0) {
        gPlayerRuntimeSlots.flags18A |= 0x40u;
    }
    if (flag80 != 0) {
        gPlayerRuntimeSlots.flags18A |= 0x80u;
    }
    return cg::yobi::PublishPlayerActionIdentity(
        name, titles, nameLength, level, value188);
}

// 原函数：0x0051A4B0。
void CopyPlayerIdentityToMapAction_51A4B0()
{
    cg::yobi::CopyInitialPlayerActionIdentity(
        gPlayerRuntimeSlots.name.data(), gPlayerRuntimeSlots.ownTitle.data(),
        &gPlayerRuntimeSlots.level, &gPlayerRuntimeSlots.hp, &gPlayerRuntimeSlots.maxHp);
}

// 原函数：0x00519CD0。
void CreateOrUpdatePlayerMapActor_519CD0(
    const std::int32_t imageNo, const std::int32_t x, const std::int32_t y,
    const std::int32_t direction)
{
    cg::yobi::YobiAction* const oldAction = gPlayerRuntimeSlots.action;
    gPlayerRuntimeSlots.imageNo = imageNo;
    gPlayerRuntimeSlots.direction = direction;
    if (oldAction != nullptr) {
        SetPlayerMapAppearance_519E50(imageNo, direction);
        static_cast<void>(SetPlayerMapCoordinates_519EC0(x, y));
        SynchronizePlayerMapActionPosition_519EE0();
        return;
    }

    cg::yobi::YobiAction* const action =
        cg::yobi::CreateMapActorAction_441DB0(imageNo, x, y, direction);
    gPlayerRuntimeSlots.action = action;
    if (action != nullptr) {
        cg::yobi::SetYobiPlayerFlag(action);
        cg::yobi::YobiAction* const current = gPlayerRuntimeSlots.action;
        cg::yobi::SetYobiWalkSpeed(current, gPlayerRuntimeSlots.walkSpeed);
    }
}

// 原函数：0x00519E50。
void SetPlayerMapAppearance_519E50(const std::int32_t imageNo, const std::int32_t direction)
{
    cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
    gPlayerRuntimeSlots.imageNo = imageNo;
    gPlayerRuntimeSlots.direction = direction;
    if (action == nullptr) {
        return;
    }
    cg::yobi::SetYobiActionField2A8(action, imageNo);
    cg::yobi::SetYobiActionField2B8(gPlayerRuntimeSlots.action, direction);
    cg::yobi::YobiAction* const current = gPlayerRuntimeSlots.action;
    if (cg::yobi::ReadMapActorAttachment44(current) != nullptr) {
        const std::int32_t mapped =
            ResolveRidingCharacterImage_51C0D0(cg::yobi::GetYobiActionField2A8(current));
        cg::yobi::SetYobiActionField2A8(gPlayerRuntimeSlots.action, mapped);
    }
}

// 原函数：0x0051A040。
std::int32_t SetPlayerFacingDirection_51A040(const std::int32_t direction)
{
    cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
    gPlayerRuntimeSlots.direction = direction;
    if (action != nullptr) {
        cg::yobi::SetYobiActionField2B8(action, direction);
    }
    return direction;
}

// 原函数：0x0051A060。
void SetPlayerAnimationState_51A060(const std::int32_t characterAction)
{
    cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
    if (action != nullptr) {
        cg::yobi::SetMapActorAnimationState_43EFC0(
            action, characterAction, characterAction == 1 ? 0 : 1);
    }
}

// 原函数：0x0051A840。
void ApplyPlayerActionCommand_51A840(
    const std::int32_t x, const std::int32_t y, const std::int32_t direction,
    const std::int32_t characterAction, const std::int32_t option1,
    const std::int32_t option2, const std::int32_t option3, const std::int32_t option4)
{
    switch (characterAction) {
    case 0: case 2: case 3: case 4: case 6: case 17: case 18: case 19:
        SetPlayerAnimationState_51A060(characterAction);
        return;
    case 1: {
        const std::int32_t speed = option1 == -1 ? 100 : option1;
        cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
        gPlayerRuntimeSlots.walkSpeed = speed;
        cg::yobi::SetYobiWalkSpeed(action, speed);
        SetPlayerAnimationState_51A060(1);
        return;
    }
    case 5: case 7: case 8: case 9: case 10: case 11:
    case 12: case 13: case 14: case 15: case 16:
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        SetPlayerAnimationState_51A060(characterAction);
        return;
    case 30: case 31:
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        SetPlayerAnimationState_51A060(characterAction == 30 ? 1 : 0);
        return;
    case 40: case 41: case 42: case 44:
        static_cast<void>(SetPlayerMapCoordinates_519EC0(x, y));
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        static_cast<void>(RestorePlayerMovementSnapshot_4681B0());
        if (characterAction == 41) {
            if (option1 == 1) {
                MarkPlayerAsPartyLeader_51A5A0();
            } else {
                static_cast<void>(ClearPlayerLeaderMarker_51A5B0());
            }
        } else if (characterAction == 42) {
            if (option1 == 1) {
                Sub_51A600();
            } else {
                Sub_51A610();
            }
        } else if (characterAction == 44) {
            if (option1 == 1) {
                Sub_51A620();
            } else {
                Sub_51A630();
            }
        }
        return;
    case 45:
        if (option1 == 1) {
            ShowPlayerPersistentStatusIcon_51BDB0();
        } else {
            HidePlayerPersistentStatusIcon_51BDC0();
        }
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        return;
    case 46:
        if (option1 != 0) {
            static_cast<void>(SetPlayerStatusIcon_51BDD0(option1));
        } else {
            ClearPlayerStatusIcon_51BDF0();
        }
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        return;
    case 47:
        ShowPlayerTimedStatusIcon_51BE10();
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        return;
    case 48:
        if (option1 == 1) {
            ShowPlayerPetPersistentStatusIcon_51BE20();
        } else {
            HidePlayerPetPersistentStatusIcon_51BE30();
        }
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        return;
    case 49:
        ShowPlayerPetTimedStatusIcon_51BE40();
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        return;
    case 50:
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        return;
    case 51:
        if (IsMapCellBlocked_469810(x, y) == 0) {
            static_cast<void>(SetPlayerMapCoordinates_519EC0(x, y));
        }
        SynchronizePlayerMapActionPosition_519EE0();
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        SetPlayerAnimationState_51A060(0);
        static_cast<void>(RestorePlayerMovementSnapshot_4681B0());
        return;
    case 60:
        cg::yobi::ApplyPlayerAppearanceReply(
            option1, option2, option3, option4, &gPlayerRuntimeSlots.imageNo);
        return;
    case 63:
        static_cast<void>(SetPlayerStatusIcon_51BDD0(option1));
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        return;
    case 64:
        ClearPlayerStatusIcon_51BDF0();
        static_cast<void>(SetPlayerFacingDirection_51A040(direction));
        return;
    case 70:
        cg::yobi::ApplyPlayerRideReply(option1, option2, &gPlayerRuntimeSlots.value174);
        return;
    case 80:
        cg::yobi::ApplyPlayerPhase3Reply(option1);
        return;
    case 200:
        if (option1 != 0) {
            reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C)[3] |= 0x10u;
        } else {
            auto* const actionBytes = reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C) + 2;
            std::uint16_t actionFlags;
            std::memcpy(&actionFlags, actionBytes, sizeof(actionFlags));
            actionFlags &= 0xEFFFu;
            std::memcpy(actionBytes, &actionFlags, sizeof(actionFlags));
        }
        return;
    default:
        return;
    }
}

// 原函数：0x00519D50。
std::int8_t DestroyPlayerMapPresence_519D50()
{
    cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
    if (action != nullptr) {
        cg::yobi::ClearMapActorAttachments(action);
        cg::yobi::MarkDeath(gPlayerRuntimeSlots.action);
        gPlayerRuntimeSlots.action = nullptr;
    }
    ClearPlayerLeaderMarker_51A5B0();
    if (GetPlayerStatusIcon_51BEC0() == 0) {
        return 0;
    }
    return RequestPlayerStatusIcon_51BE50(0);
}

// 原函数：0x0051A5A0。
void MarkPlayerAsPartyLeader_51A5A0()
{
    auto* const flagBytes = reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18A);
    flagBytes[1] |= 1u;
    *reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C) = 1u;
}

// 原函数：0x0051A5E0。
void MarkPlayerAsPartyMember_51A5E0()
{
    auto* const flagBytes = reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18A);
    flagBytes[1] |= 2u;
}

// 原函数：0x0051A5F0。
void ClearPlayerPartyMembership_51A5F0()
{
    gPlayerRuntimeSlots.flags18A &= 0xFDFFu;
}

// 原函数：0x0051A600。
void Sub_51A600()
{
    reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18A)[1] |= 0x40u;
}

// 原函数：0x0051A610。
void Sub_51A610()
{
    gPlayerRuntimeSlots.flags18A &= 0xBFFFu;
}

// 原函数：0x0051A620。
void Sub_51A620()
{
    reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18A)[1] |= 0x80u;
}

// 原函数：0x0051A630。
void Sub_51A630()
{
    gPlayerRuntimeSlots.flags18A &= 0x7FFFu;
}

// 原函数：0x0051BDB0。
void ShowPlayerPersistentStatusIcon_51BDB0()
{
    reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C)[2] |= 1u;
}

// 原函数：0x0051BDC0。
void HidePlayerPersistentStatusIcon_51BDC0()
{
    auto* const actionBytes = reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C) + 2;
    std::uint16_t actionFlags;
    std::memcpy(&actionFlags, actionBytes, sizeof(actionFlags));
    actionFlags &= 0xFFFEu;
    std::memcpy(actionBytes, &actionFlags, sizeof(actionFlags));
}

// 原函数：0x0051BE10。
void ShowPlayerTimedStatusIcon_51BE10()
{
    reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C)[2] |= 0x24u;
}

// 原函数：0x0051BE20。
void ShowPlayerPetPersistentStatusIcon_51BE20()
{
    reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C)[2] |= 8u;
}

// 原函数：0x0051BE30。
void HidePlayerPetPersistentStatusIcon_51BE30()
{
    auto* const actionBytes = reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C) + 2;
    std::uint16_t actionFlags;
    std::memcpy(&actionFlags, actionBytes, sizeof(actionFlags));
    actionFlags &= 0xFFF7u;
    std::memcpy(actionBytes, &actionFlags, sizeof(actionFlags));
}

// 原函数：0x0051BE40。
void ShowPlayerPetTimedStatusIcon_51BE40()
{
    reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C)[2] |= 0x50u;
}

// 原函数：0x0051A5B0。
cg::yobi::YobiAction* ClearPlayerLeaderMarker_51A5B0()
{
    cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
    *reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C) = 0;
    if (action == nullptr || cg::yobi::GetYobiActionField2E4(action) != 2) {
        gPlayerRuntimeSlots.flags18A &= 0xFEFFu;
    }
    return action;
}

// 原函数：0x0051BDD0。
std::int32_t SetPlayerStatusIcon_51BDD0(const std::int32_t value)
{
    reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C)[2] |= 2u;
    gPlayerRuntimeSlots.value190 = value;
    return value;
}

// 原函数：0x0051BDF0。
void ClearPlayerStatusIcon_51BDF0()
{
    auto* const actionBytes = reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C) + 2;
    std::uint16_t actionFlags;
    std::memcpy(&actionFlags, actionBytes, sizeof(actionFlags));
    actionFlags &= 0xFFFDu;
    std::memcpy(actionBytes, &actionFlags, sizeof(actionFlags));
    gPlayerRuntimeSlots.value190 = 0;
}

} // namespace detail

// 原FDA6F8位于六块清零范围之外；后续角色创建与此处复制共用。
std::int32_t gValueFDA6F8 = 0;

namespace detail {

// 原 [5199A0,519A7E)：六块清零后恢复设置与编号，不重建CryptInt。
void InitializePlayerRuntimeState_5199A0()
{
    static_assert(sizeof(gContactsF19FB0) == 0x1B30);
    static_assert(sizeof(gPetPairsDE0B08) == 0x28);

    // 玩家／宠物的宽物品与CryptInt按类型清理，其余原字节存储保持整块清零。
    gPlayerRuntimeSlots.ClearRaw();
    for (PetRuntimeRecord& record : gPetRuntimeRecords) {
        record.ClearRaw();
    }
    gPetMapCharaSlots.fill({});
    std::memset(&gSkillRuntimeSlots, 0, sizeof(gSkillRuntimeSlots));
    std::memset(gContactsF19FB0.data(), 0, sizeof(gContactsF19FB0));
    std::memset(gPetPairsDE0B08.data(), 0, sizeof(gPetPairsDE0B08));

    const volatile std::int16_t& selected = login::gSelectedCharacterIndex;
    const std::int16_t unusedSelected = selected;
    static_cast<void>(unusedSelected);
    cg::core::NullSub_509080();
    static_cast<void>(login::detail::RestoreCharacterDisplaySettings_520550(selected));
    static_cast<void>(login::detail::RestoreCharacterBgmTarget_520770(selected));

    gValueFF0D36 = 0;
    for (PetMapCharaSlot& slot : gPetMapCharaSlots) {
        slot.flags = 0;
        slot.objectIndex = 0;
        slot.action = nullptr;
    }

    const std::int32_t selectedGraphic = character_select::state::gSelectedGraphicNo;
    const std::int32_t value = gValueFDA6F8;
    gPlayerRuntimeSlots.value17C = selectedGraphic;
    gPlayerRuntimeSlots.value104 = value;
    for (TitleRecord& title : gTitlesFF0508) {
        title.id = -1;
    }
    for (std::int32_t index = 0; index < kSkillSlotCount; ++index) {
        ResetSkillRecipeSlots_519950(index);
    }
    gPlayerRuntimeSlots.direction = -1;
}

// 原函数：0x00455AC0。
void Sub_455AC0()
{
    gValue62B2F8 = 1;
}

// 原函数：0x00455AD0。
void Sub_455AD0()
{
    gValue62B324 = 1;
}

// 原函数：0x00455AE0。
void Sub_455AE0()
{
    gValue62B30C = 1;
}

// 原函数：0x00455AF0。
void Sub_455AF0()
{
    gValue62B388 = 1;
}

// 原函数：0x00455B00。
void Sub_455B00()
{
    gValue62B3A0 = 1;
}

// 原函数：0x00455B10。
void Sub_455B10()
{
    gValue62B38C = 1;
}

// 原函数：0x00455B20。
void Sub_455B20()
{
    if (gCurrentSceneType != 10) {
        gValue62B358 = 1;
    }
}

// 原函数：0x00455B40。
void Sub_455B40()
{
    gValue62B384 = 1;
}

// 原函数：0x00455B50。
void Sub_455B50()
{
    if (gCurrentSceneType != 10) {
        gValue62B310 = 1;
    }
}

// 原函数：0x00455B70。
void Sub_455B70()
{
    if (gCurrentSceneType != 10) {
        gValue62B364 = 1;
    }
}

// 原函数：0x00455B90。
void Sub_455B90()
{
    if (gCurrentSceneType != 10) {
        gValue62B338 = 1;
    }
}

// 原函数：0x00455BB0。
void Sub_455BB0()
{
    if (gCurrentSceneType != 10) {
        gValue62B36C = 1;
    }
}

// 原 [0x455BD0,0x455C56)：按指令顺序写 23 个独立槽，返回完整 EAX32 零。
std::int32_t InitializeWorldSceneFlags_455BD0()
{
    gValue556C14 = 1;
    gValue62B34E = 0;
    gValue62B34C = 0;
    gValue62B318 = 0;
    gValue62B328 = 0;
    gValue62B3B0 = 1;
    gValue556C34 = 1;
    gValue62B5AA = 0;
    gValue62B5A8 = 0;
    gMapNoticeStartTick_62B5B0 = 0;
    gValue62B308 = 0;
    gValue62B358 = 0;
    gValue62B384 = 0;
    gValue62B310 = 0;
    gValue62B364 = 0;
    gValue62B338 = 0;
    gValue62B36C = 0;
    gValue62B2F8 = 0;
    gValue62B324 = 0;
    gValue62B30C = 0;
    gValue62B388 = 0;
    gValue62B3A0 = 0;
    gValue62B38C = 0;
    return 0;
}

// 原 [0x504A30,0x504A43)：先加载记录，弃返回，再清两槽并返回完整零。
std::int32_t InitializeGameplayNotifications_504A30()
{
    LoadChatInputHistory_44A090();
    gValueD2F788 = 0;
    gPendingResultWindowMode_FF0C10 = 0;
    return 0;
}

// 原 [0x43C8F0,0x43C90A)；按序写四个独立 S32 槽并返回完整 -1。
std::int32_t ResetBattleCommandSelection_43C8F0()
{
    gValue5ABB70 = 0;
    gValue5ABB6C = 0;
    gValue553F90 = -1;
    gValue553F84 = -1;
    return -1;
}

// 原 [0x504100,0x504113)；LWR03移除旧选择备份CAFEA0，其余两槽清理及返回0保持。
std::int32_t ResetSceneUiState_504100()
{
    gValueD2F788 = 0;
    gPendingResultWindowMode_FF0C10 = 0;
    return 0;
}

// 原函数：0x0045EEE0。
std::int32_t ProcessLogoutTransition_45EEE0()
{
    const std::uint32_t phase = gLogoutTransition9A6E94;
    if (phase == 1) {
        if (ProcessSceneTransitionEffect_51FDE0(14, 8, 0x40000000U) == 1) {
            platform::gValue73ECC0 = 0;
            render::SelectPalette(0, 0);
            return 1;
        }
    } else if (phase == 0) {
        CaptureSceneAndSelectEvenVariant_51D750();
        platform::gValue73ECC0 = 2;
        cg::audio::BeginBgmFadeOut_52B990();
        const std::uint64_t tick = platform::ReadTickCount64();
        const std::uint32_t next = gLogoutTransition9A6E94 + 1;
        gSceneTransitionResetPending_58B6D0 = 1;
        gLogoutTransition9A6E94 = next;
        platform::gLastRenderDt = static_cast<double>(tick);
        ui::WindowRuntime_ProcessScene();
        cg::yobi::ClearObservationGroups_43F230();
    }
    return 0;
}

// 原[51D790,51E0A0)：完整九模式；FST后仍参与计算的值保留PC53中间精度。
std::int32_t ProcessSceneSlideOut_51D790(
    const std::int32_t mode, const std::int32_t value, const std::uint32_t payloadBits)
{
    const std::uint32_t trigger = gSceneTransitionResetPending_58B6D0;
    std::int32_t completed = 0;
    std::array<std::int32_t, 4> rectangle {0, 0, 640, 480};
    if (trigger == 1) {
        completed = -1;
        gSceneTransitionResetPending_58B6D0 = 0;
        gTransitionXFF48E8 = 0.0f;
        gTransitionAlternateXFF48EC = 0.0f;
        gTransitionYFF48F0 = 0.0f;
        gTransitionVelocityFF48F8 = 0.0f;
    }
    const float acceleration = std::bit_cast<float>(payloadBits);
    const std::int8_t step = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
    constexpr float defaultX = 0.4F;
    constexpr float defaultY = 0.3F;
    // 原FCOMP的C3选择默认值；零及unordered均走默认。
    const bool customAcceleration = acceleration < 0.0f || acceleration > 0.0f;
    auto& speeds = gTransitionLines.speedFF0FA8;
    auto& offsetsX = gTransitionLines.xFF172C;
    auto& offsetsY = gTransitionLines.yFF1EB0;
    switch (mode) {
    case 0: {
        const double position = static_cast<double>(gTransitionYFF48F0)
            - gTransitionVelocityFF48F8;
        gTransitionYFF48F0 = static_cast<float>(position);
        gTransitionVelocityFF48F8 = static_cast<float>(
            static_cast<double>(gTransitionVelocityFF48F8)
            + (customAcceleration ? acceleration : defaultY));
        if (!(position > -480.0f)) {
            completed = 1;
        }
        break;
    }
    case 1:
    case 5: {
        const double position = static_cast<double>(gTransitionVelocityFF48F8)
            + gTransitionYFF48F0;
        gTransitionYFF48F0 = static_cast<float>(position);
        if (mode == 5) {
            gTransitionAlternateYFF48F4 = static_cast<float>(
                static_cast<double>(gTransitionAlternateYFF48F4)
                - gTransitionVelocityFF48F8);
        }
        gTransitionVelocityFF48F8 = static_cast<float>(
            static_cast<double>(gTransitionVelocityFF48F8)
            + (customAcceleration ? acceleration : defaultY));
        if (position >= 480.0f) {
            completed = 1;
        }
        break;
    }
    case 2: {
        const double position = static_cast<double>(gTransitionXFF48E8)
            - gTransitionVelocityFF48F8;
        gTransitionXFF48E8 = static_cast<float>(position);
        gTransitionVelocityFF48F8 = static_cast<float>(
            static_cast<double>(gTransitionVelocityFF48F8)
            + (customAcceleration ? acceleration : defaultX));
        if (!(position > -640.0f)) {
            completed = 1;
        }
        break;
    }
    case 3:
    case 4: {
        const double position = static_cast<double>(gTransitionVelocityFF48F8)
            + gTransitionXFF48E8;
        gTransitionXFF48E8 = static_cast<float>(position);
        if (mode == 4) {
            gTransitionAlternateXFF48EC = static_cast<float>(
                static_cast<double>(gTransitionAlternateXFF48EC)
                - gTransitionVelocityFF48F8);
        }
        gTransitionVelocityFF48F8 = static_cast<float>(
            static_cast<double>(gTransitionVelocityFF48F8)
            + (customAcceleration ? acceleration : defaultX));
        if (position >= 640.0f) {
            completed = 1;
        }
        break;
    }
    case 6: {
        double active;
        if (completed == -1) {
            std::fill_n(speeds.begin(), 480, 0.0f);
            std::fill_n(offsetsX.begin(), 480, 0.0f);
            gTransitionVelocityFF48F8 = customAcceleration ? acceleration : 1.0f;
            completed = 0;
            active = 0.0;
        } else {
            active = gTransitionActiveLinesFF48FC;
        }
        for (std::int32_t index = 0; index < active; ++index) {
            if ((index & 1) != 0) {
                const double speed = static_cast<double>(gTransitionVelocityFF48F8)
                    + speeds[index];
                speeds[index] = static_cast<float>(speed);
                offsetsX[index] = static_cast<float>(speed + offsetsX[index]);
                const std::int32_t reverse = 480 - index;
                const double reverseSpeed = static_cast<double>(gTransitionVelocityFF48F8)
                    + speeds[reverse];
                speeds[reverse] = static_cast<float>(reverseSpeed);
                offsetsX[reverse] = static_cast<float>(
                    reverseSpeed + offsetsX[reverse]);
            } else {
                const double speed = static_cast<double>(speeds[index])
                    - gTransitionVelocityFF48F8;
                speeds[index] = static_cast<float>(speed);
                offsetsX[index] = static_cast<float>(speed + offsetsX[index]);
                const std::int32_t reverse = 478 - index;
                const double reverseSpeed = static_cast<double>(speeds[reverse])
                    - gTransitionVelocityFF48F8;
                speeds[reverse] = static_cast<float>(reverseSpeed);
                offsetsX[reverse] = static_cast<float>(
                    reverseSpeed + offsetsX[reverse]);
            }
        }
        gTransitionActiveLinesFF48FC = static_cast<float>(
            active + (step != 0 ? step : 4.0));
        if (gTransitionActiveLinesFF48FC > 241.0f) {
            gTransitionActiveLinesFF48FC = 241.0f;
        }
        if (!(offsetsX[241] >= -640.0f)) {
            completed = 1;
        }
        if (offsetsX[241] > 640.0f) {
            completed = 1;
        }
        break;
    }
    case 7:
    case 8: {
        double active;
        if (completed == -1) {
            std::fill_n(speeds.begin(), 480, 0.0f);
            for (std::int32_t index = 0; index < 480; ++index) {
                offsetsY[index] = static_cast<float>(index);
            }
            gTransitionVelocityFF48F8 = customAcceleration ? acceleration : 1.0f;
            completed = 0;
            active = 0.0;
        } else {
            active = gTransitionActiveLinesFF48FC;
        }
        if (mode == 7) {
            for (std::int32_t index = 0; index < active; ++index) {
                const std::int32_t reverse = 480 - index;
                if (reverse >= 240) {
                    const double speed = static_cast<double>(speeds[index])
                        - gTransitionVelocityFF48F8;
                    speeds[index] = static_cast<float>(speed);
                    offsetsY[index] = static_cast<float>(
                        speed + offsetsY[index]);
                    if (reverse > 240) {
                        const double reverseSpeed = static_cast<double>(gTransitionVelocityFF48F8)
                            + speeds[reverse];
                        speeds[reverse] = static_cast<float>(reverseSpeed);
                        offsetsY[reverse] = static_cast<float>(
                            reverseSpeed + offsetsY[reverse]);
                    }
                }
            }
            gTransitionActiveLinesFF48FC = static_cast<float>(
                active + (step != 0 ? step : 4.0));
            if (gTransitionActiveLinesFF48FC > 241.0f) {
                gTransitionActiveLinesFF48FC = 241.0f;
            }
            if (!(offsetsY[241] >= -640.0f)) {
                completed = 1;
            }
            if (offsetsY[241] > 640.0f) {
                completed = 1;
            }
        } else {
            for (std::int32_t index = 0; index < active; ++index) {
                const float speed = static_cast<float>(
                    static_cast<double>(gTransitionVelocityFF48F8)
                    + speeds[index]);
                speeds[index] = speed;
                // 原局部F32重读，不复用上方未舍入的和。
                if (!(speed >= 100.0f)) {
                    offsetsY[index] = static_cast<float>(
                        static_cast<double>(speed) + offsetsY[index]);
                } else {
                    offsetsY[index] = static_cast<float>(
                        offsetsY[index] + 100.0);
                }
            }
            gTransitionActiveLinesFF48FC = static_cast<float>(
                active + (step != 0 ? step : 16.0));
            if (gTransitionActiveLinesFF48FC > 480.0f) {
                gTransitionActiveLinesFF48FC = 480.0f;
            }
            if (offsetsY[479] > 600.0f) {
                completed = 1;
            }
        }
        break;
    }
    default:
        break;
    }

    static_cast<void>(render::ClearSurface_4512C0());
    switch (mode) {
    case 4:
        for (std::int32_t row = 0; row < 480; ++row) {
            render::TextureSurface* const source = render::gCpuTempSurface;
            rectangle[1] = row;
            rectangle[3] = row + 1;
            rectangle[0] = 0;
            rectangle[2] = 640;
            const std::int32_t x = TruncateTransitionCoordinate(
                (row & 1) != 0 ? gTransitionXFF48E8 : gTransitionAlternateXFF48EC);
            static_cast<void>(DrawTransitionRectangle(
                static_cast<std::int16_t>(x), static_cast<std::int16_t>(row), rectangle, source));
        }
        break;
    case 5:
        for (std::int32_t row = 0; row < 480; ++row) {
            rectangle[1] = row;
            rectangle[3] = row + 1;
            rectangle[0] = 0;
            rectangle[2] = 640;
            render::TextureSurface* const source = render::gCpuTempSurface;
            const std::int32_t y = TruncateTransitionCoordinate(
                (row & 1) != 0 ? gTransitionYFF48F0 : gTransitionAlternateYFF48F4);
            static_cast<void>(DrawTransitionRectangle(
                0, static_cast<std::int16_t>(row + y), rectangle, source));
        }
        break;
    case 6:
        for (std::int32_t row = 0; row < 480; ++row) {
            render::TextureSurface* const source = render::gCpuTempSurface;
            const double position = offsetsX[row];
            rectangle[1] = row;
            rectangle[3] = row + 1;
            rectangle[0] = 0;
            rectangle[2] = 640;
            const std::int32_t x = TruncateTransitionCoordinate(position);
            static_cast<void>(DrawTransitionRectangle(
                static_cast<std::int16_t>(x), static_cast<std::int16_t>(row), rectangle, source));
        }
        break;
    case 7:
    case 8:
        for (std::int32_t row = 479; row > 0; --row) {
            render::TextureSurface* const source = render::gCpuTempSurface;
            const double position = offsetsY[row];
            rectangle[1] = row;
            rectangle[3] = row + 1;
            rectangle[0] = 0;
            rectangle[2] = 640;
            const std::int32_t y = TruncateTransitionCoordinate(position);
            static_cast<void>(DrawTransitionRectangle(
                0, static_cast<std::int16_t>(y), rectangle, source));
        }
        break;
    default: {
        render::TextureSurface* const source = render::gCpuTempSurface;
        const std::int32_t y = TruncateTransitionCoordinate(gTransitionYFF48F0);
        const std::int32_t x = TruncateTransitionCoordinate(gTransitionXFF48E8);
        static_cast<void>(DrawTransitionRectangle(
            static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), rectangle, source));
        break;
    }
    }
    if (completed != 1) {
        return 0;
    }
    gTransitionXFF48E8 = 0.0f;
    gTransitionAlternateXFF48EC = 0.0f;
    gTransitionYFF48F0 = 0.0f;
    gTransitionAlternateYFF48F4 = 0.0f;
    gTransitionVelocityFF48F8 = 0.0f;

    std::int32_t nextState;
    if ((gSceneTransitionValue == 2)) {
        nextState = 3;
    } else if ((gSceneTransitionValue == 3)) {
        nextState = 4;
    } else {
        return 1;
    }
    gSceneTransitionValue = static_cast<std::int32_t>(nextState);
    return 1;
}

// 原[51E0D0,51EBE0)：完整反向过渡；6/7/8初次预计算与恢复在同次调用完成。
std::int32_t ProcessSceneSlideIn_51E0D0(
    const std::int32_t mode, const std::int32_t value, const std::uint32_t payloadBits)
{
    const std::uint32_t trigger = gSceneTransitionResetPending_58B6D0;
    std::array<std::int32_t, 4> rectangle {0, 0, 640, 480};
    std::int32_t phase;
    if (trigger == 1) {
        phase = -1;
        gSceneTransitionResetPending_58B6D0 = 0;
        gReverseTransitionPhase58B6D4 = -1;
    } else {
        phase = gReverseTransitionPhase58B6D4;
    }
    const float acceleration = std::bit_cast<float>(payloadBits);
    const bool customAcceleration = acceleration < 0.0f || acceleration > 0.0f;
    const std::int8_t step = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
    constexpr float initialSpeedY = 17.4F;
    constexpr float initialSpeedX = 22.8F;
    constexpr float initialY = 513.2F;
    constexpr float initialNegativeY = -513.2F;
    constexpr float initialX = 661.0F;
    constexpr float initialNegativeX = -661.0F;
    constexpr float defaultX = 0.4F;
    constexpr float defaultY = 0.3F;
    auto& speeds = gTransitionLines.speedFF0FA8;
    auto& offsetsX = gTransitionLines.xFF172C;
    auto& offsetsY = gTransitionLines.yFF1EB0;
    switch (mode) {
    case 0: {
        double position;
        if (phase == -1) {
            position = initialY;
            gReverseTransitionVelocityFF47C4 = initialSpeedY;
            gReverseTransitionXFF4900 = 0.0f;
            position -= gReverseTransitionVelocityFF47C4;
            gReverseTransitionPhase58B6D4 = 0;
        } else {
            position = static_cast<double>(gReverseTransitionYFF4908)
                - gReverseTransitionVelocityFF47C4;
        }
        gReverseTransitionYFF4908 = static_cast<float>(position);
        break;
    }
    case 1:
    case 5: {
        double position;
        if (phase == -1) {
            position = initialNegativeY;
            gReverseTransitionXFF4900 = 0.0f;
            if (mode == 5) {
                gReverseTransitionAlternateYFF490C = initialY;
            }
            gReverseTransitionVelocityFF47C4 = initialSpeedY;
            gReverseTransitionPhase58B6D4 = 0;
        } else {
            position = gReverseTransitionYFF4908;
        }
        gReverseTransitionYFF4908 = static_cast<float>(
            gReverseTransitionVelocityFF47C4 + position);
        if (mode == 5) {
            gReverseTransitionAlternateYFF490C = static_cast<float>(
                static_cast<double>(gReverseTransitionAlternateYFF490C)
                - gReverseTransitionVelocityFF47C4);
        }
        break;
    }
    case 2: {
        double position;
        if (phase == -1) {
            position = initialX;
            gReverseTransitionVelocityFF47C4 = initialSpeedX;
            gReverseTransitionYFF4908 = 0.0f;
            position -= gReverseTransitionVelocityFF47C4;
            gReverseTransitionPhase58B6D4 = 0;
        } else {
            position = static_cast<double>(gReverseTransitionXFF4900)
                - gReverseTransitionVelocityFF47C4;
        }
        gReverseTransitionXFF4900 = static_cast<float>(position);
        break;
    }
    case 3:
    case 4: {
        double position;
        if (phase == -1) {
            position = initialNegativeX;
            if (mode == 4) {
                gReverseTransitionAlternateXFF4904 = initialX;
            }
            gReverseTransitionYFF4908 = 0.0f;
            gReverseTransitionVelocityFF47C4 = initialSpeedX;
            gReverseTransitionPhase58B6D4 = 0;
        } else {
            position = gReverseTransitionXFF4900;
        }
        gReverseTransitionXFF4900 = static_cast<float>(
            gReverseTransitionVelocityFF47C4 + position);
        if (mode == 4) {
            gReverseTransitionAlternateXFF4904 = static_cast<float>(
                static_cast<double>(gReverseTransitionAlternateXFF4904)
                - gReverseTransitionVelocityFF47C4);
        }
        break;
    }
    case 6: {
        std::int32_t active;
        if (phase == -1) {
            std::fill_n(speeds.begin(), 480, 0.0f);
            std::fill_n(offsetsX.begin(), 480, 0.0f);
            gReverseTransitionVelocityFF47C4 = customAcceleration ? acceleration : 1.0f;
            active = 0;
            gReverseTransitionPhase58B6D4 = 0;
            for (;;) {
                for (std::int32_t index = 0; index < active; ++index) {
                    if ((index & 1) != 0) {
                        const double speed = static_cast<double>(gReverseTransitionVelocityFF47C4)
                            + speeds[index];
                        speeds[index] = static_cast<float>(speed);
                        offsetsX[index] = static_cast<float>(
                            speed + offsetsX[index]);
                        const std::int32_t reverse = 480 - index;
                        const double reverseSpeed = static_cast<double>(gReverseTransitionVelocityFF47C4)
                            + speeds[reverse];
                        speeds[reverse] = static_cast<float>(reverseSpeed);
                        offsetsX[reverse] = static_cast<float>(
                            reverseSpeed + offsetsX[reverse]);
                    } else {
                        const double speed = static_cast<double>(speeds[index])
                            - gReverseTransitionVelocityFF47C4;
                        speeds[index] = static_cast<float>(speed);
                        offsetsX[index] = static_cast<float>(
                            speed + offsetsX[index]);
                        const std::int32_t reverse = 478 - index;
                        const double reverseSpeed = static_cast<double>(speeds[reverse])
                            - gReverseTransitionVelocityFF47C4;
                        speeds[reverse] = static_cast<float>(reverseSpeed);
                        offsetsX[reverse] = static_cast<float>(
                            reverseSpeed + offsetsX[reverse]);
                    }
                }
                active += step != 0 ? step : 4;
                if (active > 240) {
                    active = 240;
                }
                if (!(offsetsX[241] >= -640.0f)) {
                    break;
                }
                if (offsetsX[241] > 640.0f) {
                    break;
                }
            }
        } else {
            active = gReverseTransitionActiveLinesFF276C;
        }
        // 原恢复固定240对，与局部active无关；到阈值只清位移，不清速度。
        for (std::int32_t index = 0; index < 240; ++index) {
            if ((index & 1) != 0) {
                if (offsetsX[index] > 1.0f) {
                    offsetsX[index] = static_cast<float>(
                        static_cast<double>(offsetsX[index]) - speeds[index]);
                    speeds[index] = static_cast<float>(
                        static_cast<double>(speeds[index])
                        - gReverseTransitionVelocityFF47C4);
                } else {
                    offsetsX[index] = 0.0f;
                }
                const std::int32_t reverse = 480 - index;
                if (offsetsX[reverse] > 1.0f) {
                    offsetsX[reverse] = static_cast<float>(
                        static_cast<double>(offsetsX[reverse]) - speeds[reverse]);
                    speeds[reverse] = static_cast<float>(
                        static_cast<double>(speeds[reverse])
                        - gReverseTransitionVelocityFF47C4);
                } else {
                    offsetsX[reverse] = 0.0f;
                }
            } else {
                if (!(offsetsX[index] >= -1.0f)) {
                    offsetsX[index] = static_cast<float>(
                        static_cast<double>(offsetsX[index]) - speeds[index]);
                    speeds[index] = static_cast<float>(
                        static_cast<double>(gReverseTransitionVelocityFF47C4)
                        + speeds[index]);
                } else {
                    offsetsX[index] = 0.0f;
                }
                const std::int32_t reverse = 478 - index;
                if (!(offsetsX[reverse] >= -1.0f)) {
                    offsetsX[reverse] = static_cast<float>(
                        static_cast<double>(offsetsX[reverse]) - speeds[reverse]);
                    speeds[reverse] = static_cast<float>(
                        static_cast<double>(gReverseTransitionVelocityFF47C4)
                        + speeds[reverse]);
                } else {
                    offsetsX[reverse] = 0.0f;
                }
            }
        }
        active += step != 0 ? step : 1;
        gReverseTransitionActiveLinesFF276C = active;
        if (active > 240) {
            gReverseTransitionActiveLinesFF276C = 240;
        }
        const float leadingOffset = offsetsX[0];
        if (!(leadingOffset < 0.0f || leadingOffset > 0.0f)) {
            gReverseTransitionPhase58B6D4 = 1;
        }
        break;
    }
    case 7: {
        std::int32_t active;
        if (phase == -1) {
            std::fill_n(speeds.begin(), 480, 0.0f);
            for (std::int32_t index = 0; index < 480; ++index) {
                offsetsY[index] = static_cast<float>(index);
            }
            gReverseTransitionVelocityFF47C4 = customAcceleration ? acceleration : 1.0f;
            active = 0;
            gReverseTransitionPhase58B6D4 = 0;
            gReverseTransitionActiveLinesFF276C = 0;
            for (;;) {
                for (std::int32_t index = 0; index < active; ++index) {
                    const std::int32_t reverse = 480 - index;
                    if (reverse >= 240) {
                        const double speed = static_cast<double>(speeds[index])
                            - gReverseTransitionVelocityFF47C4;
                        speeds[index] = static_cast<float>(speed);
                        offsetsY[index] = static_cast<float>(
                            speed + offsetsY[index]);
                        if (reverse > 240) {
                            const double reverseSpeed = static_cast<double>(gReverseTransitionVelocityFF47C4)
                                + speeds[reverse];
                            speeds[reverse] = static_cast<float>(reverseSpeed);
                            offsetsY[reverse] = static_cast<float>(
                                reverseSpeed + offsetsY[reverse]);
                        }
                    }
                }
                if (active < 241) {
                    active += step != 0 ? step : 4;
                }
                if (!(offsetsY[241] >= -640.0f)) {
                    break;
                }
                if (offsetsY[241] > 640.0f) {
                    break;
                }
            }
            gReverseTransitionActiveLinesFF276C = active;
        } else {
            active = gReverseTransitionActiveLinesFF276C;
        }
        for (std::int32_t index = 0; index < active; ++index) {
            const std::int32_t reverse = 480 - index;
            if (reverse >= 240) {
                offsetsY[index] = static_cast<float>(
                    static_cast<double>(offsetsY[index]) - speeds[index]);
                speeds[index] = static_cast<float>(
                    static_cast<double>(gReverseTransitionVelocityFF47C4)
                    + speeds[index]);
                if (reverse > 240) {
                    offsetsY[reverse] = static_cast<float>(
                        static_cast<double>(offsetsY[reverse]) - speeds[reverse]);
                    speeds[reverse] = static_cast<float>(
                        static_cast<double>(speeds[reverse])
                        - gReverseTransitionVelocityFF47C4);
                }
            }
        }
        if (offsetsY[240] >= 240.0f) {
            active -= step != 0 ? step : 4;
            gReverseTransitionActiveLinesFF276C = active;
            if (active < 0) {
                gReverseTransitionActiveLinesFF276C = 0;
            }
        }
        if (offsetsY[0] >= 0.0f) {
            gReverseTransitionPhase58B6D4 = 1;
        }
        break;
    }
    case 8: {
        std::int32_t active;
        if (phase == -1) {
            std::fill_n(speeds.begin(), 480, 0.0f);
            for (std::int32_t index = 0; index < 480; ++index) {
                offsetsY[index] = static_cast<float>(index);
            }
            gReverseTransitionVelocityFF47C4 = customAcceleration ? acceleration : 1.0f;
            active = 0;
            gReverseTransitionPhase58B6D4 = 0;
            do {
                const std::int32_t first = 479 - active;
                for (std::int32_t index = 479; index >= first; --index) {
                    if (index >= 0) {
                        const float speed = static_cast<float>(
                            static_cast<double>(speeds[index])
                            - gReverseTransitionVelocityFF47C4);
                        speeds[index] = speed;
                        if (speed > -100.0f) {
                            offsetsY[index] = static_cast<float>(
                                static_cast<double>(speed) + offsetsY[index]);
                        } else {
                            offsetsY[index] = static_cast<float>(
                                offsetsY[index] - 100.0);
                        }
                    }
                }
                active += step != 0 ? step : 16;
                if (active > 480) {
                    active = 480;
                }
            } while (offsetsY[0] >= 0.0f);
            gReverseTransitionActiveLinesFF276C = active;
        } else {
            active = gReverseTransitionActiveLinesFF276C;
        }
        const std::int32_t first = 479 - active;
        for (std::int32_t index = 479; index >= first; --index) {
            if (index >= 0) {
                const float oldSpeed = speeds[index];
                const double position = offsetsY[index];
                if (oldSpeed > -100.0f) {
                    offsetsY[index] = static_cast<float>(
                        position - speeds[index]);
                } else {
                    offsetsY[index] = static_cast<float>(position + 100.0);
                }
                speeds[index] = static_cast<float>(
                    static_cast<double>(gReverseTransitionVelocityFF47C4)
                    + speeds[index]);
            }
        }
        if (offsetsY[0] >= 0.0f) {
            active -= step != 0 ? step : 16;
            gReverseTransitionActiveLinesFF276C = active;
            if (active < 0) {
                gReverseTransitionActiveLinesFF276C = 0;
            }
        }
        if (offsetsY[479] >= 479.0f) {
            gReverseTransitionPhase58B6D4 = 1;
        }
        break;
    }
    default:
        break;
    }
    // 原0..5共用51E33C；完成比较使用FST之前的未舍入新速度。
    if (mode >= 0 && mode <= 5) {
        const float decrement = customAcceleration ? acceleration
            : (mode >= 2 && mode <= 4 ? defaultX : defaultY);
        const double velocity = static_cast<double>(gReverseTransitionVelocityFF47C4)
            - decrement;
        gReverseTransitionVelocityFF47C4 = static_cast<float>(velocity);
        if (!(velocity > 0.0)) {
            gReverseTransitionPhase58B6D4 = 1;
        }
    }

    static_cast<void>(render::ClearSurface_4512C0());
    switch (mode) {
    case 4:
        for (std::int32_t row = 0; row < 480; ++row) {
            render::TextureSurface* const source = render::gCpuTempSurface;
            rectangle[1] = row;
            rectangle[3] = row + 1;
            rectangle[0] = 0;
            rectangle[2] = 640;
            const std::int32_t x = TruncateTransitionCoordinate(
                (row & 1) != 0 ? gReverseTransitionXFF4900 : gReverseTransitionAlternateXFF4904);
            static_cast<void>(DrawTransitionRectangle(
                static_cast<std::int16_t>(x), static_cast<std::int16_t>(row), rectangle, source));
        }
        break;
    case 5:
        for (std::int32_t row = 0; row < 480; ++row) {
            rectangle[1] = row;
            rectangle[3] = row + 1;
            rectangle[0] = 0;
            rectangle[2] = 640;
            render::TextureSurface* const source = render::gCpuTempSurface;
            const std::int32_t y = TruncateTransitionCoordinate(
                (row & 1) != 0 ? gReverseTransitionYFF4908 : gReverseTransitionAlternateYFF490C);
            static_cast<void>(DrawTransitionRectangle(
                0, static_cast<std::int16_t>(row + y), rectangle, source));
        }
        break;
    case 6:
        for (std::int32_t row = 0; row < 480; ++row) {
            render::TextureSurface* const source = render::gCpuTempSurface;
            const double position = offsetsX[row];
            rectangle[1] = row;
            rectangle[3] = row + 1;
            rectangle[0] = 0;
            rectangle[2] = 640;
            const std::int32_t x = TruncateTransitionCoordinate(position);
            static_cast<void>(DrawTransitionRectangle(
                static_cast<std::int16_t>(x), static_cast<std::int16_t>(row), rectangle, source));
        }
        break;
    case 7:
    case 8:
        for (std::int32_t row = 0; row < 480; ++row) {
            render::TextureSurface* const source = render::gCpuTempSurface;
            rectangle[1] = row;
            const double position = offsetsY[row];
            rectangle[3] = row + 1;
            rectangle[0] = 0;
            rectangle[2] = 640;
            const std::int32_t y = TruncateTransitionCoordinate(position);
            static_cast<void>(DrawTransitionRectangle(
                0, static_cast<std::int16_t>(y), rectangle, source));
        }
        break;
    default: {
        render::TextureSurface* const source = render::gCpuTempSurface;
        const std::int32_t y = TruncateTransitionCoordinate(gReverseTransitionYFF4908);
        const std::int32_t x = TruncateTransitionCoordinate(gReverseTransitionXFF4900);
        static_cast<void>(DrawTransitionRectangle(
            static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), rectangle, source));
        break;
    }
    }
    if (gReverseTransitionPhase58B6D4 != 1) {
        return 0;
    }
    std::int32_t nextState;
    if ((gSceneTransitionValue == 2)) {
        nextState = 3;
    } else if ((gSceneTransitionValue == 3)) {
        nextState = 4;
    } else {
        gReverseTransitionPhase58B6D4 = -1;
        return 1;
    }
    gSceneTransitionValue = static_cast<std::int32_t>(nextState);
    gReverseTransitionPhase58B6D4 = -1;
    return 1;
}

// 原[51EC10,51EF75)：四块画面分离/合拢，坐标发布和阈值清零沿原序。
std::int32_t ProcessSceneQuadrantTransition_51EC10(const std::int32_t mode, const std::int32_t value)
{
    std::array<std::array<std::int32_t, 4>, 4> rectangles {{
        {0, 0, 320, 240},
        {320, 0, 640, 240},
        {0, 240, 320, 480},
        {320, 240, 640, 480},
    }};
    const std::int8_t lowStep = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
    const std::int32_t step = lowStep != 0 ? lowStep : 4;
    std::int32_t phase;
    if (gSceneTransitionResetPending_58B6D0 == 1) {
        phase = -1;
        gSceneTransitionResetPending_58B6D0 = 0;
        gQuadrantTransitionPhase58B6D8 = -1;
    } else {
        phase = gQuadrantTransitionPhase58B6D8;
    }

    if (mode == 1) {
        std::int32_t x0;
        std::int32_t x1;
        std::int32_t x2;
        std::int32_t y0;
        std::int32_t y1;
        if (phase == -1) {
            x0 = -280;
            x1 = 600;
            y0 = -280;
            y1 = -280;
            x2 = -280;
            gQuadrantTransitionYFF47D0[2] = 520;
            gQuadrantTransitionXFF275C[3] = 600;
            gQuadrantTransitionYFF47D0[3] = 520;
            gQuadrantTransitionPhase58B6D8 = 0;
        } else {
            x2 = gQuadrantTransitionXFF275C[2];
            x1 = gQuadrantTransitionXFF275C[1];
            x0 = gQuadrantTransitionXFF275C[0];
            y1 = gQuadrantTransitionYFF47D0[1];
            y0 = gQuadrantTransitionYFF47D0[0];
        }
        std::int32_t y2 = gQuadrantTransitionYFF47D0[2];
        x0 += step;
        y2 -= step;
        y0 += step;
        gQuadrantTransitionYFF47D0[2] = y2;
        std::int32_t x3 = gQuadrantTransitionXFF275C[3];
        x3 -= step;
        x1 -= step;
        gQuadrantTransitionXFF275C[3] = x3;
        std::int32_t y3 = gQuadrantTransitionYFF47D0[3];
        y3 -= step;
        y1 += step;
        gQuadrantTransitionYFF47D0[3] = y3;
        x2 += step;
        gQuadrantTransitionXFF275C[0] = x0;
        gQuadrantTransitionYFF47D0[0] = y0;
        gQuadrantTransitionXFF275C[1] = x1;
        gQuadrantTransitionYFF47D0[1] = y1;
        gQuadrantTransitionXFF275C[2] = x2;
        if (x0 > 0) {
            x0 = 0;
            gQuadrantTransitionXFF275C[0] = 0;
        }
        if (y0 > 0) {
            gQuadrantTransitionYFF47D0[0] = 0;
        }
        if (x1 < 320) {
            gQuadrantTransitionXFF275C[1] = 0;
        }
        if (y1 > 0) {
            gQuadrantTransitionYFF47D0[1] = 0;
        }
        if (x2 > 0) {
            gQuadrantTransitionXFF275C[2] = 0;
        }
        if (gQuadrantTransitionYFF47D0[2] < 240) {
            gQuadrantTransitionYFF47D0[2] = 0;
        }
        if (gQuadrantTransitionXFF275C[3] < 320) {
            gQuadrantTransitionXFF275C[3] = 0;
        }
        if (gQuadrantTransitionYFF47D0[3] < 240) {
            gQuadrantTransitionYFF47D0[3] = 0;
        }
        if (x0 >= 0) {
            gQuadrantTransitionPhase58B6D8 = 1;
        }
    } else if (mode == 0) {
        std::int32_t x0;
        if (phase == -1) {
            x0 = 0;
            gQuadrantTransitionYFF47D0[0] = 0;
            gQuadrantTransitionXFF275C[1] = 320;
            gQuadrantTransitionYFF47D0[1] = 0;
            gQuadrantTransitionXFF275C[2] = 0;
            gQuadrantTransitionYFF47D0[2] = 240;
            gQuadrantTransitionXFF275C[3] = 320;
            gQuadrantTransitionYFF47D0[3] = 240;
            gQuadrantTransitionPhase58B6D8 = 0;
        } else {
            x0 = gQuadrantTransitionXFF275C[0];
        }
        std::int32_t y0 = gQuadrantTransitionYFF47D0[0];
        std::int32_t x1 = gQuadrantTransitionXFF275C[1];
        std::int32_t y1 = gQuadrantTransitionYFF47D0[1];
        y0 -= step;
        gQuadrantTransitionYFF47D0[0] = y0;
        std::int32_t x2 = gQuadrantTransitionXFF275C[2];
        x1 += step;
        y1 -= step;
        x2 -= step;
        gQuadrantTransitionXFF275C[1] = x1;
        std::int32_t y2 = gQuadrantTransitionYFF47D0[2];
        gQuadrantTransitionYFF47D0[1] = y1;
        std::int32_t x3 = gQuadrantTransitionXFF275C[3];
        gQuadrantTransitionXFF275C[2] = x2;
        std::int32_t y3 = gQuadrantTransitionYFF47D0[3];
        x0 -= step;
        y2 += step;
        x3 += step;
        y3 += step;
        gQuadrantTransitionXFF275C[0] = x0;
        gQuadrantTransitionYFF47D0[2] = y2;
        gQuadrantTransitionXFF275C[3] = x3;
        gQuadrantTransitionYFF47D0[3] = y3;
        if (x0 <= -320) {
            gQuadrantTransitionPhase58B6D8 = 1;
        }
    }

    static_cast<void>(render::ClearSurface_4512C0());
    for (std::size_t index = 0; index < 4; ++index) {
        render::TextureSurface* const source = render::gCpuTempSurface;
        const auto y = static_cast<std::int16_t>(gQuadrantTransitionYFF47D0[index]);
        const auto x = static_cast<std::int16_t>(gQuadrantTransitionXFF275C[index]);
        static_cast<void>(DrawTransitionRectangle(x, y, rectangles[index], source));
    }
    if (gQuadrantTransitionPhase58B6D8 != 1) {
        return 0;
    }
    std::int32_t nextState;
    if ((gSceneTransitionValue == 2)) {
        nextState = 3;
    } else if ((gSceneTransitionValue == 3)) {
        nextState = 4;
    } else {
        gQuadrantTransitionPhase58B6D8 = -1;
        return 1;
    }
    gSceneTransitionValue = static_cast<std::int32_t>(nextState);
    gQuadrantTransitionPhase58B6D8 = -1;
    return 1;
}

// 原[51EF80,51F5F5)：64画块随机显隐/下落；候选仅在原拒绝路径重抽。
std::int32_t ProcessSceneRandomTileTransition_51EF80(
    const std::int32_t mode, const std::int32_t value, const std::uint32_t payloadBits)
{
    std::int32_t selected = platform::RandomRange_46FAF0(0, 63);
    std::int32_t acceleration = static_cast<std::int32_t>(
        static_cast<std::int64_t>(std::bit_cast<float>(payloadBits)));
    if (acceleration < 1) {
        acceleration = 1;
    }
    const std::int8_t lowCount = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
    const std::int32_t count = lowCount != 0 ? lowCount : 1;
    std::int32_t phase;
    if (gSceneTransitionResetPending_58B6D0 == 1) {
        phase = -1;
        gSceneTransitionResetPending_58B6D0 = 0;
        gTileTransitionPhase58B6DC = -1;
    } else {
        phase = gTileTransitionPhase58B6DC;
    }
    if (phase == -1 && mode >= 0 && mode <= 3) {
        InitializeTileTransitionGrid(mode == 0 ? 1 : (mode == 2 ? 2 : 0), mode == 3);
        gTileTransitionPhase58B6DC = 0;
        gTileTransitionCompletedFF46C0 = 0;
        if (mode >= 2) {
            gTileTransitionStartedFF28BC = 0;
        }
    }
    switch (mode) {
    case 0:
        for (std::int32_t index = 0; index < count; ++index) {
            while (gTileTransitionStateFF27B8[selected] == 0) {
                selected = platform::RandomRange_46FAF0(0, 63);
            }
            const std::int32_t completed = gTileTransitionCompletedFF46C0;
            gTileTransitionStateFF27B8[selected] = 0;
            const std::int32_t next = completed + 1;
            gTileTransitionCompletedFF46C0 = next;
            if (next >= 64) {
                gTileTransitionPhase58B6DC = 1;
                break;
            }
        }
        break;
    case 1:
        for (std::int32_t index = 0; index < count; ++index) {
            while (gTileTransitionStateFF27B8[selected] == 1) {
                selected = platform::RandomRange_46FAF0(0, 63);
            }
            const std::int32_t next = gTileTransitionCompletedFF46C0 + 1;
            gTileTransitionStateFF27B8[selected] = 1;
            gTileTransitionCompletedFF46C0 = next;
            if (next >= 64) {
                gTileTransitionPhase58B6DC = 1;
                break;
            }
        }
        break;
    case 2: {
        for (std::int32_t index = 0; index < count; ++index) {
            if (gTileTransitionStartedFF28BC >= 64) {
                break;
            }
            while (gTileTransitionStateFF27B8[selected] <= 1) {
                selected = platform::RandomRange_46FAF0(0, 63);
            }
            const std::int32_t started = gTileTransitionStartedFF28BC;
            gTileTransitionStateFF27B8[selected] = 1;
            gTileTransitionStartedFF28BC = started + 1;
        }
        std::int32_t completed = gTileTransitionCompletedFF46C0;
        for (std::int32_t index = 0; index < 64; ++index) {
            if (gTileTransitionStateFF27B8[index] == 1) {
                const std::int32_t oldSpeed = gTileTransitionSpeedFF46C4[index];
                const std::int32_t oldY = gTileTransitionYFF0D7C[index];
                const std::int32_t speed = oldSpeed + acceleration;
                gTileTransitionSpeedFF46C4[index] = speed;
                const std::int32_t y = oldY + speed;
                gTileTransitionYFF0D7C[index] = y;
                if (y >= 500) {
                    gTileTransitionStateFF27B8[index] = 0;
                    ++completed;
                }
            }
        }
        gTileTransitionCompletedFF46C0 = completed;
        if (completed >= 64) {
            gTileTransitionPhase58B6DC = 1;
        }
        break;
    }
    case 3:
        for (std::int32_t index = 0; index < count; ++index) {
            if (gTileTransitionStartedFF28BC >= 64) {
                break;
            }
            while (gTileTransitionStateFF27B8[selected] >= 1) {
                selected = platform::RandomRange_46FAF0(0, 63);
            }
            const std::int32_t started = gTileTransitionStartedFF28BC;
            gTileTransitionStateFF27B8[selected] = 1;
            gTileTransitionStartedFF28BC = started + 1;
        }
        for (std::int32_t index = 0; index < 64; ++index) {
            if (gTileTransitionStateFF27B8[index] == 1) {
                const std::int32_t oldSpeed = gTileTransitionSpeedFF46C4[index];
                const std::int32_t oldY = gTileTransitionYFF0D7C[index];
                const std::int32_t speed = oldSpeed + acceleration;
                gTileTransitionSpeedFF46C4[index] = speed;
                const std::int32_t y = oldY + speed;
                const std::int32_t sourceY = gTileTransitionSourceYFF47E4[index];
                gTileTransitionYFF0D7C[index] = y;
                if (y >= sourceY) {
                    gTileTransitionYFF0D7C[index] = sourceY;
                    const std::int32_t next = gTileTransitionCompletedFF46C0 + 1;
                    gTileTransitionStateFF27B8[index] = 2;
                    gTileTransitionCompletedFF46C0 = next;
                }
            }
        }
        if (gTileTransitionCompletedFF46C0 >= 64) {
            gTileTransitionPhase58B6DC = 1;
        }
        break;
    default:
        break;
    }

    static_cast<void>(render::ClearSurface_4512C0());
    std::array<std::int32_t, 4> rectangle;
    // 原两遍有不同的源读取顺序，先画状态2再画状态1。
    for (std::int32_t index = 0; index < 64; ++index) {
        if (gTileTransitionStateFF27B8[index] == 2) {
            const std::int32_t sourceX = gTileTransitionSourceXFF0E7C[index];
            const std::int32_t sourceY = gTileTransitionSourceYFF47E4[index];
            const auto y = static_cast<std::int16_t>(gTileTransitionYFF0D7C[index]);
            rectangle[0] = sourceX;
            rectangle[1] = sourceY;
            rectangle[2] = sourceX + 80;
            render::TextureSurface* const source = render::gCpuTempSurface;
            rectangle[3] = sourceY + 60;
            const auto x = static_cast<std::int16_t>(gTileTransitionXFF265C[index]);
            static_cast<void>(DrawTransitionRectangle(x, y, rectangle, source));
        }
    }
    for (std::int32_t index = 0; index < 64; ++index) {
        if (gTileTransitionStateFF27B8[index] == 1) {
            const std::int32_t sourceY = gTileTransitionSourceYFF47E4[index];
            const std::int32_t sourceX = gTileTransitionSourceXFF0E7C[index];
            rectangle[1] = sourceY;
            rectangle[0] = sourceX;
            rectangle[3] = sourceY + 60;
            render::TextureSurface* const source = render::gCpuTempSurface;
            rectangle[2] = sourceX + 80;
            const auto y = static_cast<std::int16_t>(gTileTransitionYFF0D7C[index]);
            const auto x = static_cast<std::int16_t>(gTileTransitionXFF265C[index]);
            static_cast<void>(DrawTransitionRectangle(x, y, rectangle, source));
        }
    }
    if (gTileTransitionPhase58B6DC != 1) {
        return 0;
    }
    std::int32_t nextState;
    if ((gSceneTransitionValue == 2)) {
        nextState = 3;
    } else if ((gSceneTransitionValue == 3)) {
        nextState = 4;
    } else {
        gTileTransitionPhase58B6DC = -1;
        return 1;
    }
    gSceneTransitionValue = static_cast<std::int32_t>(nextState);
    gTileTransitionPhase58B6DC = -1;
    return 1;
}

// 原[51F610,51F85F)：960个20x16画块直接BltFast，绘制后才推进共享比例。
std::int32_t ProcessSceneGridScaleTransition_51F610(const std::int32_t value)
{
    bool initialize;
    if (gSceneTransitionResetPending_58B6D0 == 1) {
        gSceneTransitionResetPending_58B6D0 = 0;
        initialize = true;
    } else {
        initialize = gImageTransitionInitializedFF4910 == 0;
    }
    if (initialize) {
        gImageTransitionInitializedFF4910 = 1;
        std::size_t index = 0;
        for (std::int32_t y = -232; y < 248; y += 16) {
            for (std::int32_t x = -310; x < 330; x += 20) {
                gGridTransitionOffsetsFF28C0[index].x = x;
                gGridTransitionOffsetsFF28C0[index].y = y;
                ++index;
            }
        }
        gImageTransitionScaleFF0D78 = value >= 0 ? 0 : 64;
    }

    static_cast<void>(render::ClearSurface_4512C0());
    std::int32_t index = 959;
    for (std::int32_t sourceY = 464; sourceY >= 0; sourceY -= 16) {
        for (std::int32_t sourceX = 620; sourceX >= 0; sourceX -= 20) {
            const std::int32_t scale = gImageTransitionScaleFF0D78;
            std::array<std::int32_t, 4> rectangle;
            rectangle[1] = sourceY;
            rectangle[3] = sourceY + 16;
            const std::int32_t yProduct = gGridTransitionOffsetsFF28C0[index].y * scale;
            rectangle[0] = sourceX;
            rectangle[2] = sourceX + 20;
            render::TextureSurface* const source = render::gCpuTempSurface;
            const std::int32_t y = yProduct / 64 + 232;
            const std::int32_t xProduct = gGridTransitionOffsetsFF28C0[index].x * scale;
            const std::int32_t x = xProduct / 64 + 310;
            static_cast<void>(DrawTransitionRectangle(
                x, y, rectangle, source, TransitionCopyMode::Direct));
            --index;
        }
    }
    const std::int32_t next = gImageTransitionScaleFF0D78 + value;
    gImageTransitionScaleFF0D78 = next;
    if (value < 0 ? next >= 0 : next <= 64) {
        return 0;
    }

    std::int32_t nextState;
    if ((gSceneTransitionValue == 2)) {
        nextState = 3;
    } else if ((gSceneTransitionValue == 3)) {
        nextState = 4;
    } else {
        gImageTransitionInitializedFF4910 = 0;
        return 1;
    }
    gSceneTransitionValue = static_cast<std::int32_t>(nextState);
    gImageTransitionInitializedFF4910 = 0;
    return 1;
}

enum class ScanlineOrigin {
    Top,
    Bottom,
};

// 原51F860/51FA70的相同入口/推进尾；两者只改变逐行采样及目标方向。
static std::int32_t ProcessScanlineTransition(
    const std::int32_t direction, const std::int32_t value, const ScanlineOrigin origin)
{
    const std::uint32_t trigger = gSceneTransitionResetPending_58B6D0;
    std::array<std::int32_t, 4> rectangle {0, 0, 640, 0};
    bool initialize;
    if (trigger == 1) {
        gSceneTransitionResetPending_58B6D0 = 0;
        initialize = true;
    } else {
        initialize = gImageTransitionInitializedFF4910 == 0;
    }
    if (initialize) {
        gImageTransitionInitializedFF4910 = 1;
        gImageTransitionScaleFF0D78 = direction >= 0 ? 0 : 480;
    }

    static_cast<void>(render::ClearSurface_4512C0());
    std::int32_t scale = gImageTransitionScaleFF0D78;
    if (scale > 0) {
        std::int32_t row = 0;
        std::int32_t numerator = 0;
        do {
            const std::int32_t sampledRow = numerator / scale;
            rectangle[1] = origin == ScanlineOrigin::Bottom ? 480 - sampledRow : sampledRow;
            rectangle[3] = rectangle[1] + 1;
            render::TextureSurface* const source = render::gCpuTempSurface;
            const std::int32_t destinationY = origin == ScanlineOrigin::Bottom ? 480 - row : row;
            static_cast<void>(DrawTransitionRectangle(
                0, destinationY, rectangle, source, TransitionCopyMode::Direct));
            scale = gImageTransitionScaleFF0D78;
            ++row;
            numerator += 480;
        } while (row < scale);
    }
    std::int32_t step = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
    if (step == 0) {
        step = 8;
    }
    const std::int32_t next = direction < 0 ? scale - step : scale + step;
    gImageTransitionScaleFF0D78 = next;
    if (direction < 0 ? next >= 0 : next <= 480) {
        return 0;
    }

    std::int32_t nextState;
    if ((gSceneTransitionValue == 2)) {
        nextState = 3;
    } else if ((gSceneTransitionValue == 3)) {
        nextState = 4;
    } else {
        gImageTransitionInitializedFF4910 = 0;
        return 1;
    }
    std::optional<platform::LegacyCryptInt> temporary;
    temporary.emplace(static_cast<std::int32_t>(nextState));
    if (direction < 0) {
        const std::uint32_t mask = temporary->mask;
        const std::uint32_t encoded = temporary->encoded;
        const std::uint8_t readCount = temporary->readCount;
        gSceneTransitionValue.mask = mask;
        gSceneTransitionValue.encoded = encoded;
        gSceneTransitionValue.readCount = readCount;
    } else {
        const std::uint32_t encoded = temporary->encoded;
        const std::uint32_t mask = temporary->mask;
        const std::uint8_t readCount = temporary->readCount;
        gSceneTransitionValue.encoded = encoded;
        gSceneTransitionValue.mask = mask;
        gSceneTransitionValue.readCount = readCount;
    }
    temporary.reset();
    gImageTransitionInitializedFF4910 = 0;
    return 1;
}

// 原函数：0x0051F860。
std::int32_t ProcessTopScanlineTransition_51F860(const std::int32_t direction, const std::int32_t value)
{
    return ProcessScanlineTransition(direction, value, ScanlineOrigin::Top);
}

// 原函数：0x0051FA70。
std::int32_t ProcessBottomScanlineTransition_51FA70(const std::int32_t direction, const std::int32_t value)
{
    return ProcessScanlineTransition(direction, value, ScanlineOrigin::Bottom);
}

// 原[51FC90,51FDDA)：中心上下半幅采样，每次直接复制后独立读取比例。
std::int32_t ProcessSceneCenterScanlineTransition_51FC90(const std::int32_t mode, const std::int32_t value)
{
    std::int32_t step = std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
    if (step == 0) {
        step = 8;
    }
    if (gSceneTransitionResetPending_58B6D0 == 1) {
        gSceneTransitionResetPending_58B6D0 = 0;
        // 原AND只作用于CL：非零mode的FFFFFF10加240得到0，而不是256。
        gHalfTransitionScaleFF0F7C = mode == 0 ? 240 : 0;
    }

    static_cast<void>(render::ClearSurface_4512C0());
    std::int32_t scale = gHalfTransitionScaleFF0F7C;
    std::array<std::int32_t, 4> upperRectangle;
    std::array<std::int32_t, 4> lowerRectangle;
    upperRectangle[0] = 0;
    upperRectangle[2] = 640;
    lowerRectangle[0] = 0;
    lowerRectangle[2] = 640;
    if (scale > 0) {
        std::int32_t row = 0;
        std::int32_t numerator = 0;
        do {
            const std::int32_t upperSample = numerator / scale;
            upperRectangle[1] = 240 - upperSample;
            upperRectangle[3] = upperRectangle[1] + 1;
            render::TextureSurface* const upperSource = render::gCpuTempSurface;
            const std::int32_t upperY = 240 - row;
            static_cast<void>(DrawTransitionRectangle(
                0, upperY, upperRectangle, upperSource, TransitionCopyMode::Direct));

            const std::int32_t lowerSample = numerator / gHalfTransitionScaleFF0F7C;
            render::TextureSurface* const lowerSource = render::gCpuTempSurface;
            const std::int32_t lowerY = row + 240;
            lowerRectangle[1] = lowerSample + 240;
            lowerRectangle[3] = lowerRectangle[1] + 1;
            static_cast<void>(DrawTransitionRectangle(
                0, lowerY, lowerRectangle, lowerSource, TransitionCopyMode::Direct));
            scale = gHalfTransitionScaleFF0F7C;
            ++row;
            numerator += 240;
        } while (row < scale);
    }
    if (mode == 0) {
        const std::int32_t next = scale - step;
        gHalfTransitionScaleFF0F7C = next;
        return next < 0 ? 1 : 0;
    }
    const std::int32_t next = scale + step;
    gHalfTransitionScaleFF0F7C = next;
    return next > 240 ? 1 : 0;
}

// 原函数：0x0045EBB0。
std::int32_t ProcessLogoutScene_45EBB0()
{
    std::int32_t sceneState = gCurrentSceneState;
    if (sceneState == 0) {
        gValue9A6E0C = 1;
        const std::int32_t transition = ProcessLogoutTransition_45EEE0();
        if (transition == 0) {
            return transition;
        }
        cg::login::detail::SaveCurrentClientPreferences_520160();
        gLogoutTransition9A6E94 = 0;
        DestroyPlayerMapPresence_519D50();
        ResetWorldObjectTable_446470();
        cg::yobi::YobiClearUp_404D30();
        sceneState = gCurrentSceneState;
        ++sceneState;
        gCurrentSceneState = sceneState;
    }

    bool resetFailure = false;
    bool resetMaintenance = false;
    if (sceneState == 1) {
        ResetLogoutScene_45ED80();
        ++gCurrentSceneState;
    } else if (sceneState == 2) {
        const std::int32_t result = UpdateLogoutScene_45ED90();
        if (result == 1) {
            network::ShutdownNetworkRuntime_5081A0();
            cg::yobi::YobiClearUp_404D30();
            SetCurrentSceneType(2);
        } else if (result == 2) {
            gCurrentSceneState = 200;
            login::detail::gReplyCodeDA3B2E = 0;
            gLogoutText9A5B08 = LR"(相当抱歉，为了维持良好的游戏环境
目前伺服器暂时进行停机维护。
维护完毕后将重新开机，请稍做等候。)" /* 原常量参考：0x0055CA28。 */;
            resetMaintenance = true;
        } else if (result == -1) {
            network::ShutdownNetworkRuntime_5081A0();
            gCurrentSceneState = 100;
            gLogoutText9A5B08 = L"处理登出逾时。" /* 原常量参考：0x0055CA18。 */;
            resetFailure = true;
        } else if (result == -2) {
            network::ShutdownNetworkRuntime_5081A0();
            gCurrentSceneState = 100;
            gLogoutText9A5B08 = L"登出处理失败。" /* 原常量参考：0x0055CA08。 */;
            resetFailure = true;
        }
    }

    // 原维护结果直接跳200重置，超时/失败直接跳100重置，均不重复入口分派。
    if (!resetMaintenance) {
        if (resetFailure || gCurrentSceneState == 100) {
            login::ResetLoginMessage();
            ++gCurrentSceneState;
        }
        if (gCurrentSceneState == 101 && login::UpdateLoginMessage(gLogoutText9A5B08) != 0) {
            SetCurrentSceneType(2);
        }
        resetMaintenance = gCurrentSceneState == 200;
    }
    if (resetMaintenance) {
        login::ResetLoginMessage();
        ++gCurrentSceneState;
    }
    const std::int32_t finalState = gCurrentSceneState;
    if (finalState == 201) {
        const std::int32_t result = login::UpdateLoginMessage(gLogoutText9A5B08);
        if (result == 0) {
            return result;
        }
        network::ShutdownNetworkRuntime_5081A0();
        cg::yobi::YobiClearUp_404D30();
        return SetCurrentSceneType(2);
    }
    return finalState;
}

// 原 [0x45ED80,0x45ED8B)：只清场景 7 阶段槽，不定义返回值。
void ResetLogoutScene_45ED80()
{
    gValue9A6204 = 0;
}

// 原 [0x45ED90,0x45EEDA)：场景 7 请求进度与结果展示。
std::int32_t UpdateLogoutScene_45ED90()
{
    std::int32_t result = 0;
    if (gValue9A6204 == 0) {
        const std::uint32_t initialTextWidth = static_cast<std::uint32_t>(cg::render::TextMeasure(gText55BAE0, 0));
        gValue9A6204 = 1;
        const std::int32_t columns =
            static_cast<std::int32_t>((initialTextWidth >> 6) + 2U);
        gValue9A5674 = 2;
        gValue9A594C = columns;
        const std::int32_t actionWidth = columns * 64;
        const std::int32_t actionHeight = gValue9A5674 * 48;
        // 原45ED90按640×480居中；场景7无布局偏移，背景与文字共用当前画布中心。
        const std::int32_t left = (platform::gWindowWidth - actionWidth) / 2;
        const std::int32_t top = (platform::gWindowHeight - actionHeight) / 2;
        gValue9A61F8 = top;
        gValue9A5AC0 = left;
        gAction9A6E98 = cg::yobi::CreateModalAction_5044E0(
            left, top, actionWidth, actionHeight, 4, 1);
    }

    const std::uint32_t phase = gValue9A6204;
    if (phase == 1) {
        if (login::detail::gReplyCodeDA3B2E != 2) {
            static_cast<void>(login::detail::BeginLogoutRequest_508C40());
        }
        ++gValue9A6204;
    } else if (phase == 2) {
        result = login::detail::PollLogoutReply_508C90();
        if (result != 0) {
            cg::yobi::YobiAction* const action = gAction9A6E98;
            if (action != nullptr) {
                cg::yobi::MarkDeath(action);
                gAction9A6E98 = nullptr;
            }
            return result;
        }
    }

    cg::yobi::YobiAction* const action = gAction9A6E98;
    if (action != nullptr && cg::yobi::IsYobiActionReady(action, 1)) {
        const std::int32_t rows = gValue9A5674;
        const std::int32_t top = gValue9A61F8;
        const std::int32_t drawY = top + (48 * rows - 16) / 2;
        const std::int32_t textWidth = cg::render::TextMeasure(gText55BAE0, 0);
        const std::int32_t columns = gValue9A594C;
        const std::int32_t horizontalWidth = columns * 64 - textWidth;
        const std::int32_t left = gValue9A5AC0;
        const std::int32_t drawX = horizontalWidth / 2 + left;
        static_cast<void>(render::DrawText3_456560(
            static_cast<std::int16_t>(drawX),
            static_cast<std::int16_t>(drawY),
            2, 0, gText55BAE0, 0));
    }
    return result;
}

} // namespace detail



void PlayerRuntimeSlots::ClearRaw() noexcept
{
    auto* const bytes = reinterpret_cast<std::byte*>(this);
    std::memset(bytes, 0, offsetof(PlayerRuntimeSlots, hp));
    const platform::LegacyCryptInt::RawPayload zero{};
    hp.RestoreRaw(zero);
    maxHp.RestoreRaw(zero);
    fp.RestoreRaw(zero);
    maxFp.RestoreRaw(zero);
    value58.RestoreRaw(zero);
    value68.RestoreRaw(zero);
    constexpr std::size_t tail = offsetof(PlayerRuntimeSlots, values78);
    std::memset(bytes + tail, 0, offsetof(PlayerRuntimeSlots, items) - tail);
    for (auto& item : items) { item = {}; }
    action = nullptr;
    text15BB8.fill(0);
    value15BCC = 0;
    values15BD0.fill(0);
}


// 原宠物表、索引对和两独立标量均为PE虚拟零区；构造由LegacyMouse按CRT顺序接入。
void PetRuntimeRecord::ClearRaw() noexcept
{
    auto* const bytes = reinterpret_cast<std::byte*>(this);
    std::memset(bytes, 0, offsetof(PetRuntimeRecord, values0C));
    for (platform::LegacyCryptInt& value : values0C) {
        value.RestoreRaw(platform::LegacyCryptInt::RawPayload{});
    }
    constexpr std::size_t tail = offsetof(PetRuntimeRecord, lvFlag6C);
    std::memset(bytes + tail, 0, offsetof(PetRuntimeRecord, items6E4) - tail);
    for (auto& item : items6E4) { item = {}; }
    updated510C = 0;
    bytes510D.fill(std::byte{0});
}

} // namespace cg::world

namespace cg::world {

const AlbumRecord* GetSelectedAlbumRecords()
{
    const std::int16_t selected = login::gSelectedCharacterIndex;
    return AlbumRecordAt(0, selected);
}

const char* ClearAlbumNewFlagAndGetName(const std::int16_t selected, const std::int32_t index)
{
    AlbumRecord* const record = AlbumRecordAt(index, selected);
    record->fresh = 0;
    return record->name.data();
}

// 原[5186A0,518774)：完整快照交旧codec，但原目标容量仅4；短写不关闭文件。
void SaveAlbumData_5186A0()
{
    auto* const encoded = static_cast<std::uint8_t*>(
        std::malloc(0x78018));
    char path[128];
    std::sprintf(
        path, "%s\\%s", platform::gDataPath, "album.dat");
    std::FILE* const stream =
        std::fopen(path, "wb");
    if (stream == nullptr) {
        std::free(encoded);
        return;
    }
    std::array<std::uint8_t, 0x3C00C> snapshot;
    std::memcpy(snapshot.data(), &gAlbumStorage, snapshot.size());
    std::int32_t encodedBytes;
    login::detail::EncodeLegacyBytes_52D310(
        snapshot.data(), 0x3C00C, 0, encoded, &encodedBytes, 4);
    const std::uint32_t count = static_cast<std::uint32_t>(encodedBytes);
    const std::uint32_t written =
        static_cast<std::uint32_t>(std::fwrite(encoded, 1, static_cast<std::uint32_t>(count), stream));
    // 原51873D在写后重新读取编码长度，再以U32比较。
    const std::uint32_t expectedCount = static_cast<std::uint32_t>(encodedBytes);
    if (written < expectedCount) {
        std::free(encoded);
        return;
    }
    static_cast<void>(std::fclose(stream));
    std::free(encoded);
}

// 原[518BE0,518C12)：固定当前角色，清版本和整组记录后保存。
void ClearCurrentCharacterAlbum_518BE0()
{
    const std::int16_t selected = login::gSelectedCharacterIndex;
    gAlbumStorage.versions[selected] = 0;
    std::memset(AlbumRecordAt(0, selected), 0, 0x1E000);
    SaveAlbumData_5186A0();
}

// 原[50E5B0,50E5C3)：相册版本及掩码原序转传，网络首参不消费。
void HandleAlbumEntryMaskReply_50E5B0(
    const std::uintptr_t /*socket*/, const std::int32_t version, const char* const payload)
{
    detail::SynchronizeAlbumEntryMask_518A40(version, payload);
}

// 原[50E5D0,50E62E)：十五数值及两文本原序转传，不增加接收门禁。
void HandleAlbumEntryReply_50E5D0(
    const std::uintptr_t /*socket*/, const std::int32_t number, const std::int32_t imageNo,
    const std::int32_t mark1, const std::int32_t mark2, const std::int32_t tribe,
    const std::int32_t value0C, const std::int32_t value10, const std::int32_t value14,
    const std::int32_t value18, const std::int32_t value1C, const std::int32_t value20,
    const std::int32_t value24, const std::int32_t value28, const std::int32_t value2C,
    const std::int32_t value30, char* const text, const char* const name)
{
    detail::UpdateAlbumEntry_518870(number, imageNo, mark1, mark2, tribe, value0C, value10, value14,
        value18, value1C, value20, value24, value28, value2C, value30, text, name);
}

// 原函数：0x0046E480。
void DrawPendingMapEffects_46E480()
{
    if (gMapCharaSceneSlots.valueC8A73C != 0) {
        static_cast<void>(render::DrawMapEffects_453580());
        gMapCharaSceneSlots.valueC8A73C = 0;
    }
}

// 原函数：0x00476680。
void DrawChatRangeOverlay_476680()
{
    cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
    if (action == nullptr) {
        return;
    }

    std::int32_t radius = platform::gValue554640;
    std::int32_t x = cg::yobi::GetYobiActionField30(action) - radius * 64;
    std::int32_t y = cg::yobi::GetYobiActionField34(action);
    const auto countdown = std::bit_cast<std::int32_t>(gChatRangeOverlayFrames_626BFC);
    x -= 31;
    y -= 23;
    if (countdown > 0) {
        std::int32_t sideCount = radius * 2 + 1;
        if (sideCount > 0) {
            std::int32_t rowX = x;
            std::int32_t rowY = y;
            std::int32_t row = 0;
            do {
                std::int32_t column = 0;
                if (sideCount > 0) {
                    do {
                        static_cast<void>(render::QueueGraphicWithResourceOffset_521C80(x, y, 100, 3, 0, nullptr));
                        radius = platform::gValue554640;
                        ++column;
                        x += 32;
                        sideCount = radius * 2 + 1;
                        y -= 24;
                    } while (column < sideCount);
                }
                ++row;
                sideCount = radius * 2 + 1;
                x = rowX + 32;
                y = rowY + 24;
                rowX = x;
                rowY = y;
            } while (row < sideCount);
        }
    }

    // 原尾部先发布递减值，再按符号位决定是否写零。
    const std::uint32_t remaining = gChatRangeOverlayFrames_626BFC - 1u;
    gChatRangeOverlayFrames_626BFC = remaining;
    if (std::bit_cast<std::int32_t>(remaining) < 0) {
        gChatRangeOverlayFrames_626BFC = 0;
    }
}

// 原函数：0x0050A3E0。
void HandlePartyMemberReply_50A3E0(
    const std::uintptr_t /*socket*/, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    const std::uint32_t mask = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    auto& slot = gPetMapCharaSlots[static_cast<std::size_t>(index)];
    if (mask == 0) {
        if (slot.flags != 0) {
            const std::uint32_t objectIndex = slot.objectIndex;
            if (objectIndex != gPlayerRuntimeSlots.objectIndex) {
                cg::yobi::YobiAction* const action =
                    FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
                if (action != nullptr) {
                    detail::ClearWorldActorPartyMembership_442C70(action);
                }
            }
        }
        std::int32_t x = -1;
        std::int32_t y = -1;
        cg::yobi::YobiAction* const targetAction = slot.action;
        if (targetAction != nullptr) {
            cg::yobi::ReadMapActorTarget(targetAction, slot.action, &x, &y);
        }
        slot.flags = 0;
        slot.action = nullptr;
        std::int32_t count = 0;
        std::int32_t next = -1;
        for (std::int32_t i = 0; i < 5; ++i) {
            if (gPetMapCharaSlots[static_cast<std::size_t>(i)].flags != 0) {
                ++count;
                if (next == -1 && i > index) {
                    next = i;
                }
            }
        }
        if (count <= 1) {
            detail::gValueFF0D36 = 0;
            static_cast<void>(detail::ClearPartyMembershipState_51B080());
            return;
        }
        if (next >= 0 && (x >= 0 || y >= 0)) {
            detail::PropagatePartyFollowerPath_46AFE0(next, x, y);
        }
        return;
    }

    detail::gValueFF0D32 = 0;
    detail::gValueFF0D36 = 1;
    slot.flags = 1;
    std::int32_t field = 2;
    if ((mask & 3u) != 0) {
        slot.objectIndex = static_cast<std::uint32_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field));
        ++field;
    }
    if ((mask & 5u) != 0) {
        slot.level08 = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((mask & 9u) != 0) {
        slot.value10 = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((mask & 0x11u) != 0) {
        slot.value0C = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((mask & 0x21u) != 0) {
        slot.value14 = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((mask & 0x41u) != 0) {
        char name[256];
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        std::strcpy(
            slot.name18.data(), std::strlen(name) <= 16 ? name : kUnknownText554590);
    }
    const std::uint32_t objectIndex = slot.objectIndex;
    if (objectIndex != gPlayerRuntimeSlots.objectIndex) {
        cg::yobi::YobiAction* const action =
            FindObjectYobi_4462B0(std::bit_cast<std::int32_t>(objectIndex));
        if (action != nullptr) {
            slot.action = action;
            detail::MarkWorldActorAsPartyMember_442C40(action);
            if (index == 0) {
                detail::MarkWorldActorAsPartyLeader_442BD0(action);
            }
        } else {
            slot.action = nullptr;
        }
    } else {
        slot.action = gPlayerRuntimeSlots.action;
        detail::MarkPlayerAsPartyMember_51A5E0();
        if (index == 0) {
            detail::MarkPlayerAsPartyLeader_51A5A0();
        }
    }
}

// 原函数：0x0050B380。
void HandleMapObjectListReply_50B380(const std::uintptr_t /*socket*/, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (ReadRippleGateOnce() != 0) {
        return;
    }

    char row[2048];
    char field[2048];
    char name[256];
    char titles[256];
    char extraText[256];
    char label[1024];
    for (std::int32_t rowIndex = 1;; ++rowIndex) {
        const std::int32_t status = network::ExtractDelimitedField_52CED0(
            payload, '\t', rowIndex, 2047, row);
        if (std::strlen(row) == 0 && status == 1) {
            return;
        }
        network::ExtractDelimitedField_52CED0(row, '|', 11, 2047, field);
        if (std::strlen(field) != 0) {
            const std::int32_t type =
                network::ParseDelimitedInteger_52CFA0(row, '|', 1);
            network::ExtractDelimitedField_52CED0(row, '|', 2, 2047, field);
            const std::int32_t objectIndex = network::Base62Decode_52D050(field);
            network::ExtractDelimitedField_52CED0(row, '|', 3, 2047, field);
            const std::int32_t x = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 4, 2047, field);
            const std::int32_t y = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 5, 2047, field);
            const std::int32_t direction = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 6, 2047, field);
            const std::int32_t imageNo = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 7, 2047, field);
            const std::int32_t level = std::atoi(field);
            const std::int32_t value40 =
                network::ParseDelimitedInteger_52CFA0(row, '|', 8);
            network::ExtractDelimitedField_52CED0(row, '|', 9, 255, name);
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            network::ExtractDelimitedField_52CED0(row, '|', 10, 255, titles);
            input::UnescapeLegacyText_52D150(titles);
            network::ExtractDelimitedField_52CED0(row, '|', 11, 2047, field);
            const std::int32_t flag1 = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 12, 2047, field);
            const std::int32_t flag2 = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 13, 2047, field);
            static_cast<void>(std::atoi(field));
            network::ExtractDelimitedField_52CED0(row, '|', 14, 255, extraText);
            input::UnescapeLegacyTextAndNotify_52D2F0(extraText);
            network::ExtractDelimitedField_52CED0(row, '|', 15, 2047, field);
            const std::int32_t mode = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 16, 2047, field);
            const std::int32_t attachmentImage = std::atoi(field);

            cg::yobi::YobiAction* object;
            if (static_cast<std::uint32_t>(objectIndex) == gPlayerRuntimeSlots.objectIndex) {
                const std::int32_t mappedImage =
                    character_select::ResolveCharacterImageForVersion(imageNo);
                if (gPlayerRuntimeSlots.direction < 0) {
                    gPlayerRuntimeSlots.direction = direction;
                }
                if (gPlayerRuntimeSlots.action == nullptr) {
                    detail::CreateOrUpdatePlayerMapActor_519CD0(mappedImage, x, y, direction);
                    detail::CopyPlayerIdentityToMapAction_51A4B0();
                } else {
                    detail::SetPlayerMapAppearance_519E50(mappedImage, gPlayerRuntimeSlots.direction);
                }
                object = gPlayerRuntimeSlots.action;
                cg::yobi::UpdateMapActorMode(
                    gPlayerRuntimeSlots.action, mode, attachmentImage);
                if (mode == 2) {
                    if ((gPlayerRuntimeSlots.flags18A & 0x200u) == 0) {
                        gPlayerRuntimeSlots.flags18A |= 0x100u;
                    }
                } else if (static_cast<std::uint8_t>(gPlayerRuntimeSlots.flags18C) == 0) {
                    gPlayerRuntimeSlots.flags18A &= 0xFEFFu;
                }
                detail::gValue9A4B94 &= 0xFDu;
                static_cast<void>(detail::UpdatePlayerMapWindow_468200());
                detail::SetPlayerMapIdentity_51A090(name, titles, level, value40, flag1, flag2);
                if ((gPlayerRuntimeSlots.flags18A & 0x100u) != 0
                    && gPetMapCharaSlots[0].flags != 0) {
                    gPetMapCharaSlots[0].level08 = gPlayerRuntimeSlots.level;
                    std::strcpy(
                        gPetMapCharaSlots[0].name18.data(), gPlayerRuntimeSlots.name.data());
                }
                for (std::size_t i = 0; i < gPetMapCharaSlots.size(); ++i) {
                    auto& slot = gPetMapCharaSlots[i];
                    if (slot.flags != 0
                        && slot.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
                        slot.action = gPlayerRuntimeSlots.action;
                        detail::MarkPlayerAsPartyMember_51A5E0();
                        if (i == 0) {
                            detail::MarkPlayerAsPartyLeader_51A5A0();
                        }
                        break;
                    }
                }
            } else {
                const std::int32_t mappedImage =
                    character_select::ResolveCharacterImageForVersion(imageNo);
                detail::UpsertWorldActor_445350(
                    objectIndex, mappedImage, x, y, direction, name, titles,
                    extraText, level, value40, flag1, flag2, type);
                object = FindObjectYobi_4462B0(objectIndex);
                if (object != nullptr) {
                    cg::yobi::UpdateMapActorMode(object, mode, attachmentImage);
                    if (mode == 2) {
                        detail::EnableWorldActorModeMarker_449C90(object);
                    } else {
                        detail::ClearWorldActorModeMarkerUnlessLeader_449CC0(object);
                    }
                    for (std::size_t i = 0; i < gPetMapCharaSlots.size(); ++i) {
                        auto& slot = gPetMapCharaSlots[i];
                        if (slot.flags != 0
                            && slot.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
                            slot.action = object;
                            detail::MarkWorldActorAsPartyMember_442C40(object);
                            if (i == 0) {
                                detail::MarkWorldActorAsPartyLeader_442BD0(object);
                            }
                            break;
                        }
                    }
                }
            }
            cg::yobi::ApplyMapActorReplyAttachments(object, row);
            continue;
        }

        network::ExtractDelimitedField_52CED0(row, '|', 6, 2047, field);
        if (std::strlen(field) != 0) {
            network::ExtractDelimitedField_52CED0(row, '|', 1, 2047, field);
            const std::int32_t objectIndex = network::Base62Decode_52D050(field);
            network::ExtractDelimitedField_52CED0(row, '|', 2, 2047, field);
            const std::int32_t x = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 3, 2047, field);
            const std::int32_t y = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 4, 2047, field);
            const std::int32_t imageNo = std::atoi(field);
            const std::int32_t value48 =
                network::ParseDelimitedInteger_52CFA0(row, '|', 5);
            network::ExtractDelimitedField_52CED0(row, '|', 6, 1023, label);
            input::UnescapeLegacyTextAndNotify_52D2F0(label);
            const std::int32_t value50 =
                network::ParseDelimitedInteger_52CFA0(row, '|', 7);
            detail::UpsertGroundItemObject_445D20(objectIndex, imageNo, x, y, 0, value48, label, value50);
            continue;
        }

        network::ExtractDelimitedField_52CED0(row, '|', 4, 2047, field);
        if (std::strlen(field) != 0) {
            network::ExtractDelimitedField_52CED0(row, '|', 1, 2047, field);
            const std::int32_t objectIndex = network::Base62Decode_52D050(field);
            network::ExtractDelimitedField_52CED0(row, '|', 2, 2047, field);
            const std::int32_t x = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 3, 2047, field);
            const std::int32_t y = std::atoi(field);
            network::ExtractDelimitedField_52CED0(row, '|', 4, 2047, field);
            const std::int32_t count = std::atoi(field);
            const std::wstring moneyLabel = std::format(L"{} 金币", count);
            std::int32_t imageNo;
            if (count < 50) {
                imageNo = 0x6B08;
            } else if (count < 100) {
                imageNo = 0x6B09;
            } else if (count < 1000) {
                imageNo = 0x6B0A;
            } else if (count < 5000) {
                imageNo = 0x6B0B;
            } else if (count < 10000) {
                imageNo = 0x6B0C;
            } else if (count < 50000) {
                imageNo = 0x6B0D;
            } else if (count < 100000) {
                imageNo = 0x6B0E;
            } else if (count < 500000) {
                imageNo = 0x6B0F;
            } else if (count < 1000000) {
                imageNo = 0x6B10;
            } else {
                imageNo = 0x6B11;
            }
            detail::UpsertGroundGoldObject_445F40(objectIndex, imageNo, x, y, 0, count, moneyLabel);
        }
    }
}

// 原函数：0x0050BCD0。
void HandleWorldActorActionsReply_50BCD0(const std::uintptr_t /*socket*/, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (ReadRippleGateOnce() != 0) {
        return;
    }
    char row[2048];
    char field[2048];
    // 原0x1000字节局部区，比较门槛仍为0x1000项；不改容量或模拟邻栈。
    std::array<std::int32_t, 1024> missing;
    std::int32_t missingCount = 0;
    std::int32_t* missingCursor = missing.data();
    for (std::int32_t rowIndex = 1;; ++rowIndex) {
        network::ExtractDelimitedField_52CED0(payload, ',', rowIndex, 2047, row);
        if (std::strlen(row) == 0) {
            return;
        }
        network::ExtractDelimitedField_52CED0(row, '|', 1, 2047, field);
        const std::int32_t objectIndex = network::Base62Decode_52D050(field);
        const std::int32_t x = network::ParseDelimitedInteger_52CFA0(row, '|', 2);
        const std::int32_t y = network::ParseDelimitedInteger_52CFA0(row, '|', 3);
        const std::int32_t characterAction = network::ParseDelimitedInteger_52CFA0(row, '|', 4);
        const std::int32_t direction = network::ParseDelimitedInteger_52CFA0(row, '|', 5);
        if (cg::yobi::HandleMapObservationActionReply_440920(objectIndex, x, y, direction, characterAction, row) != 0) {
            continue;
        }
        const std::int32_t option1 = network::ParseDelimitedInteger_52CFA0(row, '|', 6);
        const std::int32_t option2 = network::ParseDelimitedInteger_52CFA0(row, '|', 7);
        const std::int32_t option3 = network::ParseDelimitedInteger_52CFA0(row, '|', 8);
        const std::int32_t option4 = network::ParseDelimitedInteger_52CFA0(row, '|', 9);
        if (gPlayerRuntimeSlots.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
            cg::yobi::YobiAction* const player = gPlayerRuntimeSlots.action;
            if (player != nullptr && cg::yobi::GetYobiActionField2A8(player) != 0) {
                detail::ApplyPlayerActionCommand_51A840(x, y, direction, characterAction, option1, option2, option3, option4);
            }
            continue;
        }
        cg::yobi::YobiAction* const action = FindObjectYobi_4462B0(objectIndex);
        if (action != nullptr) {
            detail::ApplyWorldActorActionCommand_444410(action, x, y, direction, characterAction, option1, option2, option3, option4);
            continue;
        }
        std::int32_t index = 0;
        for (; index < missingCount; ++index) {
            if (missing[index] == objectIndex) {
                break;
            }
        }
        if (index == missingCount && static_cast<std::uint32_t>(missingCount) < 0x1000u) {
            ++missingCount;
            *missingCursor = objectIndex;
            ++missingCursor;
        }
    }
}

// 原函数：0x00472F40。
std::int32_t ToggleDuelPermission_472F40()
{
    return detail::TogglePlayerOptionAndSend(0x04);
}

std::uint8_t ReadPlayerOptions194LowByte() noexcept
{
    return *reinterpret_cast<const unsigned char*>(&gPlayerRuntimeSlots.options194);
}

// 原函数：0x00472F70。
std::int32_t TogglePartyChat_472F70()
{
    return detail::TogglePlayerOptionAndSend(0x08);
}

// 原函数：0x00472FA0。
std::int32_t TogglePartyJoinPermission_472FA0()
{
    return detail::TogglePlayerOptionAndSend(0x01);
}

// 原函数：0x00472FD0。
std::int32_t ToggleContactCardExchangePermission_472FD0()
{
    return detail::TogglePlayerOptionAndSend(0x10);
}

// 原函数：0x00473000。
std::int32_t ToggleTradePermission_473000()
{
    return detail::TogglePlayerOptionAndSend(0x20);
}

// 原函数：0x00473030。
std::int32_t ToggleGuildInvitePermission_473030()
{
    return detail::TogglePlayerOptionAndSend(0x40);
}

// 原函数：0x0050BFA0。
std::uint32_t HandlePlayerOptionsReply_50BFA0(
    const std::uintptr_t /*socket*/, const std::int32_t value)
{
    const std::uint32_t gate = gValue9A6E0C;
    if (gate != 0) {
        return gate;
    }
    const std::uint32_t options = static_cast<std::uint32_t>(value) & 0xFFFDu;
    gPlayerRuntimeSlots.options194 = static_cast<std::uint16_t>(options);
    return options;
}

// 原函数：0x0050BFC0。
std::uint16_t HandlePlayerBattleRowReply_50BFC0(
    const std::uintptr_t /*socket*/, const std::int32_t value)
{
    const auto low = static_cast<std::uint16_t>(value);
    gPlayerRuntimeSlots.value1A0 = low;
    return low;
}

// 原函数：0x0050CF70。
std::int32_t HandlePetAttributePointsReply_50CF70(
    const std::uintptr_t /*socket*/, const std::int32_t index,
    const std::int32_t value)
{
    gPetRuntimeRecords[index].lvFlag6C = value;
    return index;
}

// PTP01：原 [0x50C7A0,0x50C7E4)；首 ×5 后发布 mode，剩余缩放/单 tick 与完整 gate/tick 返回保。
std::uint32_t HandlePlayerTimerReply_50C7A0(
    const std::uintptr_t /*socket*/, const std::int32_t seconds,
    const std::int32_t mode)
{
    const std::uint32_t gate = gValue9A6E0C;
    if (gate != 0) {
        return gate;
    }
    const std::int32_t duration = seconds < 0 ? 0 : seconds;
    const std::int32_t firstScale = duration * 5;
    gPlayerRuntimeSlots.timerMode10 = mode;
    gPlayerRuntimeSlots.timerRemaining0C = firstScale * 5 * 5 * 8;
    const std::uint64_t tick = platform::ReadTickCount64();
    gPlayerRuntimeSlots.timerTick14 = tick;
    input::gChatLogSuppressed_626C14 = 0;
    return static_cast<std::uint32_t>(tick);
}

// 原函数：0x0050CE10。
std::int32_t HandleSystemChatReply_50CE10(const std::uintptr_t /*socket*/, const char* const text)
{
    char source[2024];
    char command[512];
    std::strcpy(source, text);
    input::UnescapeLegacyTextAndNotify_52D2F0(source);
    if (std::strncmp(source, "@FIND_LV1", static_cast<std::uint32_t>(9)) == 0) {
        std::int32_t sound = 0;
        std::strncpy(command, source, static_cast<std::uint32_t>(sizeof(command)));
        std::sscanf(command, "@FIND_LV1@SE:%d", &sound);
        const char* const message = const_cast<char*>(std::strstr(command, "@MSG:"));
        std::strncpy(source, message + 5, static_cast<std::uint32_t>(sizeof(source)));
        static_cast<void>(audio::PlaySe_52C450(sound, 320));
    }
    AppendChatMessage_44B050(cg::text::FromGbkWide(source).c_str(), 4, 3);
    const std::int32_t soundEnabled = platform::gValue554644;
    if (soundEnabled != 0) {
        return audio::PlaySe_52C450(74, 320);
    }
    return soundEnabled;
}

// 原函数：0x0050E1A0。
void HandleDirectionalMapEffectsReply_50E1A0(
    const std::uintptr_t /*socket*/, const std::int32_t mask,
    const std::int32_t value, const char* const /*unusedText*/)
{
    if (mask == 0) {
        const std::uint16_t zero = 0;
        gMapCharaSceneSlots.valueC8A73E = 0;
        std::memcpy(&gMapCharaSceneSlots.valueC8A744, &zero, sizeof(zero));
        gMapCharaSceneSlots.valueC8A74E = 0;
    } else {
        const auto low = static_cast<std::uint16_t>(value);
        if ((mask & 1) != 0) {
            gMapCharaSceneSlots.valueC8A73E = low;
        }
        if ((mask & 2) != 0) {
            std::memcpy(&gMapCharaSceneSlots.valueC8A744, &low, sizeof(low));
        }
        if ((mask & 4) != 0) {
            gMapCharaSceneSlots.valueC8A74E = low;
        }
    }
    detail::ConfigureDirectionalMapEffects_46E0D0(mask, value);
}

// 原函数：0x0050E650。
void HandleMapSceneEffectReply_50E650(
    const std::uintptr_t /*socket*/, const std::int32_t mode,
    const std::int32_t duration, const std::int32_t floor,
    const std::int32_t mapX, const std::int32_t mapY)
{
    cg::yobi::ConfigureMapSceneEffect_449700(mode, duration, floor, mapX, mapY);
}

// 原函数：0x0050E210。
std::int32_t HandleNotificationSoundReply_50E210(
    const std::uintptr_t /*socket*/, const std::int32_t value)
{
    const std::int32_t gate = std::bit_cast<std::int32_t>(gValue9A6E0C);
    if (gate != 0) {
        return gate;
    }
    std::int32_t sound;
    switch (value) {
    case 0: sound = 0x47; break;
    case 1: sound = 0x48; break;
    case 2:
    case 4: sound = 0x44; break;
    case 3: sound = 0x45; break;
    case 5: sound = 0x41; break;
    case 6: sound = 0x42; break;
    case 7: sound = 0x43; break;
    case 8: sound = 0x49; break;
    default: return value;
    }
    return audio::PlaySe_52C450(sound, 320);
}

// CTS01：原两点选项低BYTE现场读取，计时/转义/六字段发送与原返回序保持。
// 原函数：0x0050C940。
std::int32_t SendPlayerChatInput_50C940(
    const input::InputContext* const context, const std::int32_t color)
{
    const auto chatText = text::ToGbk(input::ContextText(*context));
    if (script::LuaProcessWinMgrChatMessage(chatText.c_str()) == 1) {
        return 0;
    }
    const std::int32_t sendColor = color == 4 ? 5 : color;
    const std::int32_t gate = gChatGateDA3B54;
    const std::uint8_t disabled = gChatDisabledDA3B4C;
    const bool restricted = disabled != 0
        || (gate == 0 && detail::gValueFF0D36 != 0
            && (ReadPlayerOptions194LowByte() & 8u) != 0);
    if (restricted) {
        const std::uint64_t tick = platform::ReadTickCount64();
        if (tick - gChatCurrentTickDA3B40 > 180000u) {
            gChatDisabledDA3B4C = 0;
        } else if (gChatGateDA3B54 != 0 || detail::gValueFF0D36 == 0
                   || (ReadPlayerOptions194LowByte() & 8u) == 0) {
            AppendChatMessage_44B050(L"目前禁言中" /* 原常量参考：0x0058AFEC。 */, 4, 3);
            const std::int32_t sound = platform::gValue554644;
            if (sound != 0) {
                return audio::PlaySe_52C450(74, 320);
            }
            return sound;
        }
    } else {
        gChatPreviousTickDA3B3C = gChatCurrentTickDA3B40;
        const std::uint64_t tick = platform::ReadTickCount64();
        const std::uint64_t previous = gChatPreviousTickDA3B3C;
        gChatCurrentTickDA3B40 = tick;
        if (tick - previous < gChatMinimumIntervalMs) {
            AppendChatMessage_44B050(L"重复发言，禁言三分钟" /* 原常量参考：0x0058B000。 */, 4, 3);
            std::int32_t result = platform::gValue554644;
            if (result != 0) {
                result = audio::PlaySe_52C450(74, 320);
            }
            gChatDisabledDA3B4C = 1;
            return result;
        }
    }
    // 原1024字节发送字段：P|占2字节，正文转义后按GBK检查，宽正文不限长。
    const std::wstring packet = L"P|" + input::EscapeInputText_52D2D0(context, 1022);
    const std::int32_t mode = platform::gValue73ECBC;
    const std::int32_t range = platform::gValue554640;
    const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
    const std::uintptr_t socket = platform::gConnectionSocket;
    return network::SendChatMessage_511900(socket, x, y, packet, sendColor, range, mode);
}

// 原函数：0x0050CAE0。
void HandlePlayerChatReply_50CAE0(
    const std::uintptr_t /*socket*/, const std::int32_t objectIndex,
    const char* const payload, const std::int32_t color, const std::int32_t font)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char kind[2];
    network::ExtractDelimitedField_52CED0(payload, '|', 1, 1, kind);
    if (kind[0] != 'P') {
        return;
    }
    char source[2024];
    network::ExtractDelimitedField_52CED0(payload, '|', 2, 2023, source);
    input::UnescapeLegacyTextAndNotify_52D2F0(source);
    const auto length = static_cast<std::int32_t>(std::strlen(source));
    char snapshot[2048];
    if (length > 0) {
        std::memcpy(snapshot, source, static_cast<std::size_t>(length));
    }
    std::int32_t drawColor = color;
    std::int32_t drawFont = font;
    if (input::gChatLogSuppressed_626C14 == 0) {
        char name[64];
        char stripped[32];
        std::memset(name, 0, sizeof(name));
        const std::size_t initialLength = std::strlen(source);
        gChatGateDA3B54 = 0;
        if (initialLength != 0) {
            std::uint32_t cursor = 0;
            for (;;) {
                if (source[cursor] == ':' && source[cursor + 1] == ' ') {
                    std::strncpy(name, source, static_cast<std::uint32_t>(cursor));
                    name[cursor] = '\0';
                    break;
                }
                ++cursor;
                if (cursor >= std::strlen(source)) {
                    break;
                }
            }
        }
        const auto stripPrefix = [&](const std::uint32_t prefixLength) {
            std::uint32_t cursor = 0;
            if (std::strlen(name) != prefixLength) {
                do {
                    stripped[cursor] = name[cursor + prefixLength];
                    ++cursor;
                } while (cursor < std::strlen(name) - prefixLength);
            }
            stripped[cursor] = '\0';
            std::memset(name, 0, sizeof(name));
            std::strcpy(name, stripped);
        };
        if (std::strncmp(name, "[MIC]", static_cast<std::uint32_t>(std::strlen("[MIC]"))) == 0) {
            gChatGateDA3B54 = 1;
            stripPrefix(5);
        } else if (std::strncmp(name, "[GP]", static_cast<std::uint32_t>(std::strlen("[GP]"))) == 0) {
            stripPrefix(4);
        }
        if (cg::world::blacklist::ContainsBlacklistRecord_4013D0(cg::text::FromGbkWide(name)) != 0) {
            return;
        }
        if (color < 0) {
            drawColor = 0;
            drawFont = 1;
        }
        const char* const sharedText = "";
        bool sameAsSharedText = false;
        std::size_t comparisonIndex = 0;
        for (;;) {
            const auto sourceByte = static_cast<unsigned char>(source[comparisonIndex]);
            const auto sharedByte =
                static_cast<unsigned char>(sharedText[comparisonIndex]);
            if (sourceByte != sharedByte) {
                break;
            }
            if (sourceByte == 0) {
                sameAsSharedText = true;
                break;
            }
            ++comparisonIndex;
        }
        if (!sameAsSharedText) {
            AppendChatMessage_44B050(cg::text::FromGbkWide(source).c_str(), drawColor, drawFont);
        }
    }
    if (platform::gValue554644 != 0) {
        static_cast<void>(audio::PlaySe_52C450(74, 320));
    }
    if (objectIndex < 0) {
        return;
    }
    if (gPlayerRuntimeSlots.action != nullptr
        && gPlayerRuntimeSlots.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
        detail::SetPlayerChatBubble_51A6A0(1000, snapshot, drawColor, drawFont, length);
    } else {
        cg::yobi::YobiAction* const action = FindObjectYobi_4462B0(objectIndex);
        if (action != nullptr) {
            detail::SetWorldActorChatBubble_442DC0(action, 1000, snapshot, drawColor, drawFont, length);
        }
    }
}

// 原函数：0x0050D000。
void HandlePartyMembershipReply_50D000(
    const std::uintptr_t /*socket*/, const std::int32_t first, const std::int32_t second)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (first == 1) {
        if (second == 1) {
            detail::MarkPlayerAsPartyMember_51A5E0();
        }
    } else if (first == 0 && second == 1) {
        detail::ClearPlayerPartyMembership_51A5F0();
        detail::gValueFF0D36 = 0;
        static_cast<void>(detail::ClearPartyMembershipState_51B080());
        const std::int32_t direction = std::bit_cast<std::int8_t>(
            static_cast<std::uint8_t>(gPlayerRuntimeSlots.direction));
        const std::int32_t y = static_cast<std::int32_t>(gPlayerSnapshotY);
        const std::int32_t x = static_cast<std::int32_t>(gPlayerSnapshotX);
        static_cast<void>(
            network::SendPlayerPositionAndDirection_512720(platform::gConnectionSocket, x, y, direction));
    }
    detail::gValueFF0D32 = 0;
}

// MPR01：原509650完整444条；掩码推进及六CryptInt/RNG、先三临时值后发布、金额双尾门槛/名字长度和S32返回保。
std::int32_t HandlePlayerPrimaryStatsReply_509650(
    const std::uintptr_t /*socket*/, const char* const payload)
{
    const std::int32_t gate = std::bit_cast<std::int32_t>(gValue9A6E0C);
    if (gate != 0) {
        return gate;
    }
    const auto flags = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    std::int32_t field = 2;
    const auto parseCrypt = [&](platform::LegacyCryptInt* const target,
                                const std::uint32_t mask) {
        if ((flags & mask) != 0) {
            const std::int32_t parsed =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            *target = static_cast<std::int32_t>(parsed);
            ++field;
        }
    };
    parseCrypt(&gPlayerRuntimeSlots.hp, 0x3u);
    parseCrypt(&gPlayerRuntimeSlots.maxHp, 0x5u);
    parseCrypt(&gPlayerRuntimeSlots.fp, 0x9u);
    parseCrypt(&gPlayerRuntimeSlots.maxFp, 0x11u);

    std::uint32_t bit = 0x20u;
    for (std::int32_t index = 0; index < 8; ++index) {
        if ((flags & (bit | 1u)) != 0) {
            gPlayerRuntimeSlots.values78[index] =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
    parseCrypt(&gPlayerRuntimeSlots.value58, 0x2001u);
    parseCrypt(&gPlayerRuntimeSlots.value68, 0x4001u);

    const std::array<std::int32_t*, 12> targets {{
        &gPlayerRuntimeSlots.level,
        &gPlayerRuntimeSlots.values78[8], &gPlayerRuntimeSlots.values78[9],
        &gPlayerRuntimeSlots.values78[10], &gPlayerRuntimeSlots.values78[11],
        &gPlayerRuntimeSlots.values78[12], &gPlayerRuntimeSlots.values78[13],
        &gPlayerRuntimeSlots.valueB8,
        &gPlayerRuntimeSlots.valuesE4[0], &gPlayerRuntimeSlots.valuesE4[1],
        &gPlayerRuntimeSlots.valuesE4[2], &gPlayerRuntimeSlots.valuesE4[3],
    }};
    bit = 0x8000u;
    for (std::int32_t* const target : targets) {
        if ((flags & (bit | 1u)) != 0) {
            *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
    const std::uint32_t tailGate = flags & 0x8000001u;
    if (tailGate != 0) {
        gPlayerRuntimeSlots.valueFC =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((flags & 0x10000001u) != 0) {
        gPlayerRuntimeSlots.titleIndex100 =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        ++field;
    }
    if ((flags & 0x20000001u) != 0) {
        char name[256];
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) <= 16) {
            std::strcpy(gPlayerRuntimeSlots.name.data(), name);
        }
        ++field;
    }
    if ((flags & 0x40000001u) != 0) {
        char title[256];
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, title);
        input::UnescapeLegacyTextAndNotify_52D2F0(title);
        if (std::strlen(title) <= 16) {
            std::strcpy(gPlayerRuntimeSlots.ownTitle.data(), title);
        }
        ++field;
    }
    if (tailGate != 0) {
        const std::int32_t result =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        gPlayerRuntimeSlots.value15BCC = result;
        return result;
    }
    return 0;
}

// MPR01：原509BA0完整232条；附加属性按掩码顺序、W16 tribe/三S32金额上限及原步速修正/同action发布保。
void HandlePlayerSecondaryStatsReply_509BA0(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    const auto flags = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    std::int32_t field = 2;
    const std::array<std::int32_t*, 12> firstTargets {{
        &gPlayerRuntimeSlots.valuesD4[0], &gPlayerRuntimeSlots.valuesD4[1],
        &gPlayerRuntimeSlots.valuesD4[2], &gPlayerRuntimeSlots.valuesD4[3],
        &gPlayerRuntimeSlots.valuesBC[0], &gPlayerRuntimeSlots.valuesBC[1],
        &gPlayerRuntimeSlots.valuesBC[2], &gPlayerRuntimeSlots.valuesBC[3],
        &gPlayerRuntimeSlots.valuesBC[4], &gPlayerRuntimeSlots.valuesBC[5],
        &gPlayerRuntimeSlots.value104, &gPlayerRuntimeSlots.valueF4,
    }};
    std::uint32_t bit = 2;
    for (std::int32_t* const target : firstTargets) {
        if ((flags & (bit | 1u)) != 0) {
            *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
    if ((flags & 0x2001u) != 0) {
        gPlayerRuntimeSlots.value00 = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field));
        ++field;
    }
    const std::array<std::int32_t*, 4> middleTargets {{
        &gPlayerRuntimeSlots.valueF8, &gPlayerRuntimeSlots.value08,
        &gPlayerRuntimeSlots.value17C, &gPlayerRuntimeSlots.value16C,
    }};
    bit = 0x4000u;
    for (std::int32_t* const target : middleTargets) {
        if ((flags & (bit | 1u)) != 0) {
            *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
    if ((flags & 0x40001u) != 0) {
        std::int32_t speed = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        gPlayerRuntimeSlots.walkSpeed = speed;
        if (speed <= 0) {
            speed = 100;
            gPlayerRuntimeSlots.walkSpeed = speed;
        }
        cg::yobi::YobiAction* const action = gPlayerRuntimeSlots.action;
        if (action != nullptr) {
            cg::yobi::SetYobiWalkSpeed(action, speed);
        }
        ++field;
    }
    const std::array<std::int32_t*, 5> lastTargets {{
        &gPlayerRuntimeSlots.valuesB0[0], &gPlayerRuntimeSlots.valuesB0[1],
        &gPlayerRuntimeSlots.values15BD0[0], &gPlayerRuntimeSlots.values15BD0[1],
        &gPlayerRuntimeSlots.values15BD0[2],
    }};
    bit = 0x80000u;
    for (std::int32_t* const target : lastTargets) {
        if ((flags & (bit | 1u)) != 0) {
            *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            ++field;
        }
        bit <<= 1;
    }
}

// 原函数：0x00509E30。
void HandlePlayerProfessionReply_509E30(const std::uintptr_t, const std::int32_t value, char* const text)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    gSkillRuntimeSlots.valueF633E8 =
        (gSkillRuntimeSlots.valueF633E8 & 0xFFFFFF00u) | static_cast<std::uint8_t>(value);
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    std::strcpy(
        gSkillRuntimeSlots.jobName.data(), std::strlen(text) < 24 ? text : kUnknownText554590);
}

std::uint16_t ReadSkillPairUsed(const std::int8_t pairIndex)
{
    return pairIndex >= 0 && pairIndex < kSkillSlotCount ? gSkillPairsF63370[pairIndex].used : 0;
}

// 原函数：0x00505040。
std::int32_t IsItemEligibleForDecoration_505040(const std::int32_t slot)
{
    const PlayerItemRecord* const item = &gPlayerRuntimeSlots.items[slot];
    if (item->hasDescription == 0) {
        return 0;
    }
    const std::int32_t type = item->itemType;
    if (type == 38 || type == 25 || type == 39) {
        return 1;
    }
    if ((item->flags & 0x800u) == 0) {
        return 0;
    }
    return detail::HasSkillRecipeForItem_504F10(item->itemId, 0) != 0 ? 1 : 0;
}

// 原函数：0x005050B0。
std::int32_t IsItemEligibleForPetDecoration_5050B0(const std::int32_t slot)
{
    const PlayerItemRecord* const item = &gPlayerRuntimeSlots.items[slot];
    if (item->hasDescription == 0) {
        return 0;
    }
    if (item->itemType == 80) {
        return 1;
    }
    if ((item->flags & 0x800u) == 0) {
        return 0;
    }
    return detail::HasSkillRecipeForItem_504F10(item->itemId, 1) != 0 ? 1 : 0;
}

// 原函数：0x00505110。
std::int32_t IsItemEligibleForRefining_505110(const std::int32_t slot)
{
    const PlayerItemRecord* const item = &gPlayerRuntimeSlots.items[slot];
    if (item->hasDescription == 0) {
        return 0;
    }
    const std::int32_t type = item->itemType;
    if (!((type >= 0 && type < 23) || (type >= 56 && type < 66))) {
        return 0;
    }
    return (item->flags & 0x1000u) != 0 ? 1 : 0;
}

// 原函数：0x00509EB0。
void HandlePlayerSkillsReply_509EB0(const std::uintptr_t /*socket*/, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char name[256];
    std::int32_t field = 2;
    for (;;) {
        const std::int32_t index =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field - 1);
        const std::int32_t skillId =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::int32_t level =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 2);
        const std::int32_t experience =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 3);
        const std::int32_t maxLevel =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
        const std::int32_t nextExperience =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 5);
        const std::int32_t requiredSlots =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 6);
        const std::int32_t canRebirth =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 7);
        const std::int32_t fpReduce =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 8);
        const std::int32_t sequence =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 9);
        if (index < 0) {
            break;
        }

        if (index >= kSkillSlotCount) {
            field += 11;
            continue;
        }
        SkillRuntimeRecord& record = gSkillsF1BAFC[index];
        if (std::strlen(name) == 0 && level < 0 && experience < 0) {
            record.name[0] = '\0';
            record.level = 0;
            record.experience = 0;
            std::memset(record.techniques3C.data(), 0, sizeof(record.techniques3C));
            detail::ResetSkillRecipeSlots_519950(index);
        } else {
            std::strcpy(
                record.name.data(), std::strlen(name) <= 24 ? name : kUnknownText554590);
            record.level = level;
            record.experience = experience;
            record.maxLevel = maxLevel;
            record.nextExperience = nextExperience;
            record.requiredSlots19[0] = static_cast<std::uint8_t>(requiredSlots);
            record.canRebirth = canRebirth;
            record.skillId = skillId;
            record.fpReduce = fpReduce;
            record.sequence = sequence;
            if (canRebirth == 1) {
                const std::uint8_t flag = ui::WindowRuntime_ReadObservationFlag();
                const std::uintptr_t socket = platform::gConnectionSocket;
                const std::int32_t value = index + (flag != 0 ? 100 : 0);
                static_cast<void>(network::SendSkillRebirthSelection_510F50(socket, value));
            }
        }
        field += 11;
    }
    detail::BuildSortedSkillDisplayEntries_518600(gSkillsF1BAFC.data(), gSkillPairsF63370.data(), kSkillSlotCount);
    gValueDA3B20 = 0;
}

// 原函数：0x0050A120。
void HandlePlayerSkillTechniquesReply_50A120(
    const std::uintptr_t, const std::int32_t skillIndex, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (skillIndex < 0 || skillIndex >= kSkillSlotCount) {
        return;
    }
    auto& techniques = gSkillsF1BAFC[skillIndex].techniques3C;
    std::memset(techniques.data(), 0, sizeof(techniques));
    SkillTechniqueRecord* target = techniques.data();
    char name[256];
    char description[256];
    std::int32_t field = 2;
    for (; target != techniques.data() + techniques.size();) {
        if (network::ExtractDelimitedField_52CED0(
                payload, '|', field - 1, 255, name) == 1) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        network::ExtractDelimitedField_52CED0(payload, '|', field, 255, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        const std::int32_t value7C =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 1);
        const std::int32_t value80 =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 2);
        const auto packed = static_cast<std::uint32_t>(
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 3));
        const std::int32_t value8C =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
        if (std::strlen(name) == 0) {
            continue;
        }
        target->used88 = 1;
        std::strcpy(target->name.data(), std::strlen(name) <= 24 ? name : kUnknownText554590);
        std::strcpy(
            target->description.data(),
            std::strlen(description) <= 96 ? description : kUnknownText554590);
        target->value7C = value7C;
        target->value80 = value80;
        target->value90 = static_cast<std::int32_t>((packed >> 12) & 0xFu);
        target->value84 = static_cast<std::int32_t>(packed & 0xFFFu);
        target->value8C = value8C;
        field += 6;
        ++target;
    }
}

// 原函数：0x0050C580。
void HandleSkillRecipesReply_50C580(
    const std::uintptr_t, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0 || index < 0 || index >= kSkillSlotCount) {
        return;
    }
    SkillRuntimeRecord& record = gSkillsF1BAFC[index];
    record.rebirthState4C48 = std::bit_cast<std::int8_t>(ui::WindowRuntime_ReadObservationFlag());
    char name[256];
    char description[256];
    std::int32_t field = 1;
    for (SkillRecipeRecord& recipe : record.recipes8E8) {
        recipe.itemId = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        recipe.level = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        recipe.value10 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        network::ExtractDelimitedField_52CED0(payload, '|', field++, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) <= 28) {
            std::strcpy(recipe.name.data(), name);
        }
        network::ExtractDelimitedField_52CED0(payload, '|', field++, 255, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        if (std::strlen(description) <= 56) {
            std::strcpy(recipe.description.data(), description);
        }
        recipe.value04 = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        recipe.value0C = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        for (SkillRecipeItem& item : recipe.items6C) {
            item.itemId = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
            network::ExtractDelimitedField_52CED0(payload, '|', field++, 255, name);
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            if (std::strlen(name) <= 28) {
                std::strcpy(item.name.data(), name);
            }
            item.count = network::ParseDelimitedInteger_52CFA0(payload, '|', field++);
        }
    }
}

// 原函数：0x0050BF20。
void HandleWorldObjectRemovalReply_50BF20(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    std::int32_t field = 1;
    std::int32_t objectIndex =
        network::ParseDelimitedBase62_52D0D0(payload, ',', field);
    while (objectIndex != -1) {
        RemoveWorldObject_4462E0(objectIndex);
        for (auto& slot : gPetMapCharaSlots) {
            if (slot.flags != 0
                && slot.objectIndex == static_cast<std::uint32_t>(objectIndex)) {
                slot.action = nullptr;
                break;
            }
        }
        ++field;
        objectIndex = network::ParseDelimitedBase62_52D0D0(payload, ',', field);
    }
}

// 原函数：0x0050BFD0。
void HandleFullPlayerInventoryReply_50BFD0(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char name[256];
    char memo[256];
    char description[768];
    for (std::int32_t index = 0; index < 28; ++index) {
        static_cast<void>(ui::detail::WindowItem_CancelMatchingItemDrag(index));
        const std::int32_t nameField = 2 + index * 13;
        network::ExtractDelimitedField_52CED0(payload, '|', nameField, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) == 0) {
            gPlayerRuntimeSlots.items[index].used = 0;
            gPlayerRuntimeSlots.items[index].name.clear();
        } else {
            detail::ApplyPlayerItemFields(index, nameField, payload, name, memo, description);
        }
        if (index >= 8) {
            ui::detail::WindowTrade_CopyOwnItem(1, index - 8);
        }
    }
}

// 原函数：0x0050C260。
void HandlePlayerInventoryUpdatesReply_50C260(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char name[256];
    char memo[256];
    char description[768];
    std::int32_t field = 1;
    std::int32_t index = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
    static_cast<void>(ui::detail::WindowItem_CancelMatchingItemDrag(index));
    std::int32_t status =
        network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, name);
    while (status != 1) {
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) == 0) {
            gPlayerRuntimeSlots.items[index].used = 0;
        } else {
            detail::ApplyPlayerItemFields(index, field + 1, payload, name, memo, description);
        }
        if (index >= 8) {
            ui::detail::WindowTrade_CopyOwnItem(1, index - 8);
            ui::detail::WindowTrade_CopyDisplayItems(1, index - 8);
        }
        field += 14;
        index = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        static_cast<void>(ui::detail::WindowItem_CancelMatchingItemDrag(index));
        status = network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, name);
    }
}

// 原函数：0x0051B5F0。
std::int32_t CountInventoryItemQuantity_51B5F0(const std::int32_t itemId)
{
    std::int32_t count = 0;
    if (itemId <= 0) {
        return count;
    }
    for (const PlayerItemRecord* item = gPlayerRuntimeSlots.items.data() + 8;
         item != gPlayerRuntimeSlots.items.data() + gPlayerRuntimeSlots.items.size(); ++item) {
        if (item != nullptr && item->used != 0 && item->itemId == itemId) {
            count += item->count;
        }
    }
    return count;
}

// 原函数：0x0050C900。
void HandleInventorySlotSwapReply_50C900(
    const std::uintptr_t, const std::int32_t first, const std::int32_t second)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    detail::SwapPlayerInventorySlots_51B530(first, second);
    static_cast<void>(ui::detail::WindowItem_CancelMatchingItemDrag(first));
    static_cast<void>(ui::detail::WindowItem_CancelMatchingItemDrag(second));
    ui::detail::WindowTrade_SwapRemoteItems(1, first, second);
    ui::detail::WindowTrade_SwapDisplayItems(1, first, second);
}

// 原函数：0x0051B630。
void SwapPlayerOrPetItemSlots_51B630(
    const std::int32_t firstGroup, const std::int32_t firstIndex,
    const std::int32_t secondGroup, const std::int32_t secondIndex)
{
    if (firstIndex < 0 || secondIndex < 0) {
        return;
    }
    PlayerItemRecord* firstBase = nullptr;
    PlayerItemRecord* secondBase = nullptr;
    if (firstGroup == 0) {
        if (firstIndex >= 28) {
            return;
        }
        firstBase = gPlayerRuntimeSlots.items.data();
    } else if (firstGroup >= 1 && firstGroup <= 5) {
        if (firstIndex >= 6) {
            return;
        }
        firstBase = gPetRuntimeRecords[firstGroup - 1].items6E4.data();
    }
    if (secondGroup == 0) {
        if (secondIndex >= 28) {
            return;
        }
        secondBase = gPlayerRuntimeSlots.items.data();
    } else if (secondGroup >= 1 && secondGroup <= 5) {
        if (secondIndex >= 6) {
            return;
        }
        secondBase = gPetRuntimeRecords[secondGroup - 1].items6E4.data();
    }
    if (firstBase == nullptr || secondBase == nullptr) {
        return;
    }
    PlayerItemRecord* const first = firstBase + firstIndex;
    PlayerItemRecord* const second = secondBase + secondIndex;
    const PlayerItemRecord temporary = *first;
    *first = *second;
    *second = temporary;
}

// 原函数：0x0052E180。
std::int32_t HasPetTechnique_52E180(const std::int32_t index, const std::int32_t techniqueId)
{
    if (index < 0 || index >= 5) {
        return 0;
    }
    const PetRuntimeRecord& pet = gPetRuntimeRecords[index];
    if (pet.used == 0) {
        return 0;
    }
    for (const detail::PetTechniqueRecord& technique : pet.techniquesD8) {
        if (technique.used != 0 && technique.techniqueId == techniqueId) {
            return 1;
        }
    }
    return 0;
}

// 原函数：0x0052E1D0。
std::int32_t HasPetMailTechnique_52E1D0(const std::int32_t index)
{
    if (HasPetTechnique_52E180(index, 0x22C4) != 0) {
        return 1;
    }
    if (HasPetTechnique_52E180(index, 0x22C5) != 0) {
        return 1;
    }
    if (HasPetTechnique_52E180(index, 0x22C6) != 0) {
        return 1;
    }
    return HasPetTechnique_52E180(index, 0x22C7) != 0 ? 1 : 0;
}

// 原函数：0x00510150。
void HandlePetInventoryUpdatesReply_510150(
    const std::uintptr_t, const std::int32_t petIndex, const char* const payload)
{
    if (gValue9A6E0C != 0 || petIndex < 0 || petIndex >= 5) {
        return;
    }
    char name[256];
    char memo[256];
    char description[768];
    std::int32_t field = 1;
    std::int32_t index = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
    while (index < 6) {
        if (network::ExtractDelimitedField_52CED0(
                payload, '|', field + 1, 255, name) == 1) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        PlayerItemRecord& record = gPetRuntimeRecords[petIndex].items6E4[index];
        if (std::strlen(name) == 0) {
            record.used = 0;
            record.name.clear();
        } else {
            record.used = 1;
            if (std::strlen(name) <= 28) {
                record.name = cg::text::FromGbkWide(name);
            }
            const std::int32_t color =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 2);
            record.color = color;
            if (color < 0) {
                record.color = 0;
            }
            network::ExtractDelimitedField_52CED0(
                payload, '|', field + 3, 767, description);
            input::UnescapeLegacyTextAndNotify_52D2F0(description);
            detail::SetPetItemDescription_519770(petIndex, index, description);
            record.hasDescription = !record.description[0].empty() ? 1 : 0;
            record.imageNo = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
            record.canUseInField = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 5));
            record.battleUseFlag = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 6));
            record.target = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 7));
            // 原510362再次传field+4，不能按玩家物品协议改成field+8或复用图号。
            record.level = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
            record.flags = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field + 9));
            record.itemId = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 10);
            record.itemType = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 11);
            record.count = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 12);
            network::ExtractDelimitedField_52CED0(payload, '|', field + 13, 255, memo);
            input::UnescapeLegacyTextAndNotify_52D2F0(memo);
            if (std::strlen(memo) <= 16) {
                record.memo = cg::text::FromGbkWide(memo);
            }
        }
        field += 14;
        index = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
    }
}

// 原函数：0x0050A6B0。
void HandlePetPrimaryStatsReply_50A6B0(
    const std::uintptr_t /*socket*/, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0 || index < 0 || index >= 5) {
        return;
    }
    const auto flags = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    PetRuntimeRecord& record = gPetRuntimeRecords[index];
    std::int32_t field = 2;
    if (flags == 0) {
        record.ClearRaw();
    } else {
        record.used = 1;
        if ((flags & 0x3u) != 0) {
            const std::int32_t imageNo =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field);
            record.imageNo6A0 = character_select::ResolveCharacterImageForVersion(imageNo);
            ++field;
        }
        if ((flags & 0x5u) != 0) {
            record.value04 = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field));
            ++field;
        }
        std::uint32_t bit = 0x8u;
        for (platform::LegacyCryptInt& value : record.values0C) {
            if ((flags & (bit | 1u)) != 0) {
                const std::int32_t parsed =
                    network::ParseDelimitedInteger_52CFA0(payload, '|', field);
                value = static_cast<std::int32_t>(parsed);
                ++field;
            }
            bit <<= 1;
        }

        // 掩码201..100001按原顺序映射，不将两条协议的数值列混用。
        const std::array<std::int32_t*, 12> targets {{
            &record.value08,
            &record.values70[5], &record.values70[6], &record.values70[7],
            &record.values70[8], &record.values70[9], &record.values70[10],
            &record.values70[21], &record.values70[22],
            &record.values70[23], &record.values70[24],
            &record.value6A4,
        }};
        for (std::int32_t* const target : targets) {
            if ((flags & (bit | 1u)) != 0) {
                *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
                ++field;
            }
            bit <<= 1;
        }
        if ((flags & 0x200001u) != 0) {
            record.value02 = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field));
            ++field;
        }
        if ((flags & 0x400001u) != 0) {
            char name[256];
            network::ExtractDelimitedField_52CED0(payload, '|', field, 255, name);
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            if (std::strlen(name) <= 16) {
                std::strcpy(record.name6AC.data(), name);
            }
            ++field;
        }
        if ((flags & 0x800001u) != 0) {
            char name[256];
            network::ExtractDelimitedField_52CED0(payload, '|', field, 255, name);
            input::UnescapeLegacyTextAndNotify_52D2F0(name);
            if (std::strlen(name) <= 16) {
                std::strcpy(record.name6BD.data(), name);
            }
            ++field;
        }
        if ((flags & 0x1000001u) != 0) {
            const auto state = static_cast<std::uint16_t>(
                network::ParseDelimitedInteger_52CFA0(payload, '|', field));
            if ((state & 0x10u) != 0) {
                record.flags6AA = static_cast<std::uint16_t>(state & ~0x10u);
                record.follow6D4 = 1;
            } else {
                record.flags6AA = state;
                record.follow6D4 = 0;
            }
            ++field;
        }
        if ((flags & 0x2000001u) != 0) {
            record.values70[25] =
                network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        }
    }
    detail::RefreshPetCount_519920();
    detail::BuildPetDisplayEntries_518580(gPetRuntimeRecords.data(), gPetPairsDE0B08.data(), 5);
    detail::MarkPetDetailDraftChanged_492250();
    record.updated510C = 1;
}

// 原函数：0x0050AC20。
void HandlePetSecondaryStatsReply_50AC20(
    const std::uintptr_t /*socket*/, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0 || index < 0 || index >= 5) {
        return;
    }
    const auto flags = static_cast<std::uint32_t>(
        network::ParseDelimitedBase62_52D0D0(payload, '|', 1));
    PetRuntimeRecord& record = gPetRuntimeRecords[index];
    if (flags == 0) {
        record.used = 0;
        record.name6AC[0] = '\0';
        record.name6BD[0] = '\0';
    } else {
        record.used = 1;
        const std::array<std::int32_t*, 19> targets {{
            &record.values70[0], &record.values70[1], &record.values70[2],
            &record.values70[3], &record.values70[4],
            &record.values70[17], &record.values70[18],
            &record.values70[19], &record.values70[20],
            &record.values70[11], &record.values70[12], &record.values70[13],
            &record.values70[14], &record.values70[15], &record.values70[16],
            &record.value6D0,
            &record.values6D8[1], &record.values6D8[2], &record.values6D8[0],
        }};
        std::uint32_t bit = 2;
        std::int32_t field = 2;
        for (std::int32_t* const target : targets) {
            if ((flags & (bit | 1u)) != 0) {
                *target = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
                ++field;
            }
            bit <<= 1;
        }
    }
    detail::RefreshPetCount_519920();
    detail::BuildPetDisplayEntries_518580(gPetRuntimeRecords.data(), gPetPairsDE0B08.data(), 5);
    detail::MarkPetDetailDraftChanged_492250();
}

// 原函数：0x0050B0D0。
void HandlePetTechniquesReply_50B0D0(
    const std::uintptr_t, const std::int32_t index, const char* const payload)
{
    if (gValue9A6E0C != 0 || index < 0 || index >= 5) {
        return;
    }
    PetRuntimeRecord& record = gPetRuntimeRecords[index];
    if (record.used == 0) {
        return;
    }
    std::memset(record.techniquesD8.data(), 0, sizeof(record.techniquesD8));
    char name[256];
    char description[256];
    std::int32_t field = 2;
    for (;;) {
        const std::int32_t techniqueIndex =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field - 1);
        if (techniqueIndex < 0) {
            break;
        }
        const std::int32_t techniqueId =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 1, 255, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        network::ExtractDelimitedField_52CED0(payload, '|', field + 2, 255, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        const std::int32_t fp =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 3);
        const std::int32_t techniqueField =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 4);
        const std::int32_t target =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 5);
        const std::int32_t sequence =
            network::ParseDelimitedInteger_52CFA0(payload, '|', field + 6);
        if (techniqueIndex >= kPetTechniqueSlotCount) {
            field += 8;
            continue;
        }
        detail::PetTechniqueRecord& technique = record.techniquesD8[techniqueIndex];
        if (std::strlen(name) == 0) {
            technique.name[0] = '\0';
            technique.description[0] = '\0';
            technique.fp = 0;
            technique.used = 0;
            // 原50B230不推进字段；不把空名称改成跳过本条或结束解析。
            continue;
        }
        technique.used = 1;
        technique.techniqueId = techniqueId;
        std::strcpy(technique.name.data(), std::strlen(name) <= 24 ? name : "???");
        std::strcpy(
            technique.description.data(),
            std::strlen(description) <= 96 ? description : "???");
        field += 8;
        technique.fp = fp;
        technique.field = static_cast<std::uint16_t>(techniqueField);
        technique.target = static_cast<std::uint16_t>(target);
        technique.sequence = sequence;
    }
    BuildPetTechniqueDisplayEntries_5185C0(record.techniquesD8.data(), record.techniquePairs650.data(), kPetTechniqueSlotCount);
}

// 原函数：0x005185C0。
void BuildPetTechniqueDisplayEntries_5185C0(
    const detail::PetTechniqueRecord* const source, SkillPairRecord* const destination,
    const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        destination[index].used = source[index].used;
        destination[index].index = static_cast<std::uint16_t>(index);
        destination[index].sequence = source[index].sequence;
    }
}

// 原函数：0x0043C4B0。
std::int32_t FindBattlePetIndex_43C4B0()
{
    for (std::int32_t index = 0; index < 5; ++index) {
        const PetRuntimeRecord& pet = gPetRuntimeRecords[index];
        if (pet.used == 1 && pet.flags6AA == 2) {
            return index;
        }
    }
    return -1;
}

// 原函数：0x0050A300。
void HandlePlayerTitlesReply_50A300(const std::uintptr_t /*socket*/, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    char text[256];
    std::int32_t field = 2;
    for (TitleRecord& record : detail::gTitlesFF0508) {
        network::ExtractDelimitedField_52CED0(payload, '|', field - 1, 255, text);
        input::UnescapeLegacyTextAndNotify_52D2F0(text);
        const std::int32_t id = network::ParseDelimitedInteger_52CFA0(payload, '|', field);
        if (id >= 0) {
            record.id = id;
            std::strcpy(record.text.data(), text);
            record.value = network::ParseDelimitedInteger_52CFA0(payload, '|', field + 1);
        } else {
            record.id = -1;
            record.text[0] = '\0';
            record.value = -1;
        }
        field += 3;
    }
    detail::CopyTitleRecords_5198A0(
        reinterpret_cast<std::uint8_t*>(gSortedTitlesDE03F8.data()),
        reinterpret_cast<const std::uint8_t*>(detail::gTitlesFF0508.data()), 64);
    detail::SortTitleRecords_5198D0(gSortedTitlesDE03F8.data(), 64);
}

// 原函数：0x00519840。
std::int32_t FindTitleIndexById(const std::int32_t id)
{
    if (id < 0) {
        return -1;
    }
    for (std::int32_t index = 0; index < 64; ++index) {
        if (detail::gTitlesFF0508[index].id == id) {
            return index;
        }
    }
    return -1;
}

// R03：唯一阵位表按尺寸增量更新，重复初始化不累加中心偏移。
void CenterBattleLayoutToWindow()
{
    if (platform::gWindowHeight > 480) {
        const std::size_t extent = static_cast<std::size_t>(platform::gWindowHeight) + 1;
        if (extent > gTransitionLines.speedFF0FA8.size()) {
            gTransitionLines.speedFF0FA8.resize(extent);
            gTransitionLines.xFF172C.resize(extent);
            gTransitionLines.yFF1EB0.resize(extent);
        }
    }
    static std::int32_t appliedOffsetX = 0;
    static std::int32_t appliedOffsetY = 0;
    const std::int32_t offsetX = platform::gWindowWidth > 0 ? (platform::gWindowWidth - 640) / 2 : 0;
    const std::int32_t offsetY = platform::gWindowHeight > 0 ? (platform::gWindowHeight - 480) / 2 : 0;
    const std::int32_t deltaX = offsetX - appliedOffsetX;
    const std::int32_t deltaY = offsetY - appliedOffsetY;
    if (deltaX == 0 && deltaY == 0) {
        return;
    }
    for (auto& point : gBattleGraphicPoints553758) {
        point[0] += deltaX;
        point[1] += deltaY;
        point[2] += deltaX;
        point[3] += deltaY;
    }
    appliedOffsetX = offsetX;
    appliedOffsetY = offsetY;
}

// 原 [0x468520,0x46856A)；零 mapId 忽略第三参，两路均透传 sprintf 返回。
std::int32_t __cdecl FormatMapDataPath_468520(
    char* const buffer,
    const std::int32_t mapId,
    const std::int32_t value9E0434,
    const std::int32_t floor)
{
    if (mapId == 0) {
        return std::sprintf(
            buffer, "%s\\%d\\%d.dat", kMapPathPrefix, 0, floor);
    }
    return std::sprintf(
        buffer, "%s\\%d\\%d\\%d.dat", kMapPathPrefix, mapId, value9E0434, floor);
}

// 原函数：0x0046C760。
std::int32_t DrawCurrentAutoMap_46C760(const std::int32_t x, const std::int32_t y)
{
    if (gMapResourceMode_9DAD68 == 1) {
        detail::gAutoMapWindowDirty_55DA88 = 1;
        const std::int32_t playerY = static_cast<std::int32_t>(gPlayerY);
        const std::int32_t playerX = static_cast<std::int32_t>(gPlayerX);
        static_cast<void>(detail::LoadAutoMapWindow_46C840(gMapFloor, playerX, playerY));
        const std::int32_t index = detail::gAutoMapWindowIndex55DA90;
        detail::gAutoMapWindowDirty_55DA88 = 0;
        const std::int32_t height = detail::gAutoMapWindowSizes55DA94[index].height;
        const std::int32_t width = detail::gAutoMapWindowSizes55DA94[index].width;
        return render::DrawColorAutoMap_44FEB0(x, y, detail::gAutoMapColorWindow9CF6EC, width, height);
    }
    if (detail::gAutoMapWindowDirty_55DA88 != 0) {
        const std::int32_t playerY = static_cast<std::int32_t>(gPlayerY);
        const std::int32_t playerX = static_cast<std::int32_t>(gPlayerX);
        static_cast<void>(detail::LoadAutoMapWindow_46C840(gMapFloor, playerX, playerY));
        detail::gAutoMapWindowDirty_55DA88 = 0;
    }
    const std::int32_t index = detail::gAutoMapWindowIndex55DA90;
    const std::int32_t height = detail::gAutoMapWindowSizes55DA94[index].height;
    const std::int32_t width = detail::gAutoMapWindowSizes55DA94[index].width;
    return render::DrawIndexedAutoMap_44F5C0(x, y, detail::gAutoMapIndexWindow9DAD7C, width, height);
}

// 原 [0x4681B0,0x4681F6)：先读 Y/X 快照并恢复窗口，随后顺序清六槽。
std::int32_t __cdecl RestorePlayerMovementSnapshot_4681B0()
{
    const std::int32_t y =
        static_cast<std::int32_t>(gPlayerSnapshotY);
    const std::int32_t x =
        static_cast<std::int32_t>(gPlayerSnapshotX);
    static_cast<void>(detail::SetPlayerMapPosition_468330(x, y));
    static_cast<void>(detail::UpdatePlayerMapWindow_468200());
    gMovePathCount_C82200 = 0;
    gPendingDirectionCount_C82202 = 0;
    gPlayerPathRebuildPending_C821FC = 0;
    detail::gMapLeftHoldDurationMs_C8A778 = 0;
    detail::gMapLeftPressTick_C8A794 = 0;
    detail::gValueC82224 = 0;
    return 0;
}

// 原 [0x469810,0x469864)：边界外返回 1，边界内比较第四平面的完整 WORD。
std::int32_t __cdecl IsMapCellBlocked_469810(const std::int32_t x, const std::int32_t y)
{
    const std::int32_t localX = x - gMapXStart;
    const std::int32_t localY = y - gMapYStart;
    if (localX < 0) {
        return 1;
    }
    const std::int32_t width = gMapSpanX;
    if (localX >= width || localY < 0) {
        return 1;
    }
    const std::int32_t height = gMapSpanY;
    if (localY >= height) {
        return 1;
    }
    const std::int32_t index = localX + width * localY;
    return gMapWalkabilityArray[static_cast<std::size_t>(index)] == 1;
}

// 原函数：0x005093C0。
void HandlePlayerPositionReply_5093C0(
    const std::uintptr_t, const std::int32_t x, const std::int32_t y,
    const std::int32_t direction)
{
    static_cast<void>(detail::UpdatePlayerMapWindow_468200());
    static_cast<void>(SetPlayerMapCoordinates_519EC0(x, y));
    SynchronizePlayerMapActionPosition_519EE0();
    static_cast<void>(detail::SetPlayerFacingDirection_51A040(direction));
}

// 原函数：0x0050D080。
void Sub_50D080(
    const std::uintptr_t, const std::int32_t first, const std::int32_t second)
{
    gValue9FB918 = static_cast<std::int32_t>(first);

    gValueAF0FE8 = static_cast<std::int32_t>(second);

    const std::uint32_t encoded = gValue9FB918.encoded;
    const std::uint32_t mask = gValue9FB918.mask;
    const std::uint8_t readCount = gValue9FB918.readCount;
    gValueC821E8.encoded = encoded;
    gValueC821E8.mask = mask;
    gValueC821E8.readCount = readCount;
}

// 原函数：0x0050D120。
void HandleBattleEntryReply_50D120(
    const std::uintptr_t, const std::int32_t type, const std::int32_t imageNo)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (gValueD67E78 != 0) {
        static_cast<void>(detail::DestroyPlayerMapPresence_519D50());
        detail::ClearWorldObjectsAndActions_4464D0();
        static_cast<void>(RestorePlayerMovementSnapshot_4681B0());
        detail::DetachPartyMapActions_51B0F0();
        gMapNoticeStartTick_62B5B0 = 0;
        detail::SynchronizeTimedMapNoticeWindow_455FC0();
        detail::ResetTimedMapNotice_455C60();

        const std::uint32_t mask = gValue9FB918.mask;
        const std::uint32_t encoded = gValue9FB918.encoded;
        const std::uint8_t readCount = gValue9FB918.readCount;
        gValueC821E8.mask = mask;
        gValueC821E8.encoded = encoded;
        gValueC821E8.readCount = readCount;

        gPlayerCoordinateSyncPending_C821D8 = std::int32_t{0};

        cg::world::gRippleGate = std::int32_t{1};

        gDirectionalMapTransitionPending_9FB92E = 0;
        detail::gValueFF03D8 = 0;
        detail::gValueF7CA04 = 0;
        static_cast<void>(SetCurrentSceneTypeAndState(9, 1));
        cg::yobi::YobiClearUp_404D30();
        detail::DetachWorldObjectActions_446610();

        cg::world::gRippleGate = std::int32_t{0};
        static_cast<void>(network::AcknowledgeBattleExit_5108F0(platform::gConnectionSocket, 0));
        return;
    }
    if (type > 0) {
        const bool modeTwo = type == 2;
        gValue5ABFC4 = 1;
        gBattleType5ABFC8 = type;
        if (modeTwo) {
            gBattleModeFlag5ABFCC = 1;
            detail::gValueC821D0 = 0;
            detail::gValueB65A7C = 1;
        } else {
            const bool modeThree = type == 3;
            gBattleModeFlag5ABFCC = 0;
            if (modeThree) {
                detail::gValueC821D0 = 1;
                detail::gValueB65A7C = 0;
            } else {
                detail::gValueC821D0 = 0;
                detail::gValueB65A7C = type == 2 || type == 4 || type == 5 || type == 6 ? 1 : 0;
            }
        }
        gValue5ABB1C = 0;
        gBattleBackgroundImage59F9F8 = imageNo;
        gValue5ABB28 = 0;
        detail::gBattleCWriteIndex5AAA6C = 0;
        detail::gBattleCReadIndex5AAA44 = 0;
        detail::gBattleMWriteIndex5A3A1C = 0;
        detail::gBattleMReadIndex5AAA68 = 0;
        gBattleCText5A3A20[0] = '\0';
        gBattleMText5AAA7C[0] = '\0';
        for (std::size_t offset = 0; offset < 0x4000; offset += 0x1000) {
            detail::gBattleCQueue5A4A3C[offset] = '\0';
            detail::gBattleMQueue59FA18[offset] = '\0';
        }
        detail::gBattleFirstTurnPending5ABC6C = 1;
        return;
    }
    gPlayerCoordinateSyncPending_C821D8 = std::int32_t{0};
    detail::gValueFF03D8 = 0;
    detail::gValueF7CA04 = 0;
}

// 原函数：0x0050D3C0。
void HandleBattleStreamReply_50D3C0(const std::uintptr_t, const char* const text)
{
    const char kind = text[0];
    if (kind == 'C') {
        const std::int32_t index = detail::gBattleCWriteIndex5AAA6C;
        char* const destination = detail::gBattleCQueue5A4A3C.data()
            + static_cast<std::ptrdiff_t>(index) * 0x1000;
        const std::size_t byteCount = std::strlen(text) + 1;
        const std::size_t wordBytes = byteCount & ~std::size_t{3};
        std::memcpy(destination, text, wordBytes);
        const std::int32_t next = index + 1;
        std::memcpy(destination + wordBytes, text + wordBytes, byteCount - wordBytes);
        const bool wrap = next >= 4;
        detail::gBattleCWriteIndex5AAA6C = next;
        if (wrap) {
            detail::gBattleCWriteIndex5AAA6C = 0;
        }
        return;
    }
    if (kind == 'P') {
        static_cast<void>(std::sscanf(text + 2, "%X|%X|%X|%X|%X|%X",
            &detail::gBattlePosition5ABCA8, &gBattlePlayerFlags5ABCAC,
            &gBattleSkillFlags5ABC04, &gBattlePetSkillFlags5ABB78,
            &detail::gBattleRebirthLevel5ABB7C, &gBattleRidePet5ABC38));
        return;
    }
    if (kind == 'A') {
        static_cast<void>(std::sscanf(text + 2, "%X|%X",
            &gBattleEndBits5ABBC0, &gBattleTurn5ABB74));
        if (detail::gBattleFirstTurnPending5ABC6C == 1) {
            const std::uint32_t turn = gBattleTurn5ABB74;
            detail::gBattleFirstTurnPending5ABC6C = 0;
            gBattleTurnSnapshot5ABC08 = turn;
        }
        cg::yobi::EnableCompletedBattleActorIdleAnimation_43BFF0();
        return;
    }
    if (text[1] == 'U') {
        detail::gValue5ABCB0 = 1;
        return;
    }
    if (kind == 'M') {
        const std::int32_t index = detail::gBattleMWriteIndex5A3A1C;
        char* const destination = detail::gBattleMQueue59FA18.data()
            + static_cast<std::ptrdiff_t>(index) * 0x1000;
        const std::size_t byteCount = std::strlen(text) + 1;
        const std::size_t wordBytes = byteCount & ~std::size_t{3};
        std::memcpy(destination, text, wordBytes);
        const std::int32_t next = index + 1;
        std::memcpy(destination + wordBytes, text + wordBytes, byteCount - wordBytes);
        const bool wrap = next >= 4;
        detail::gBattleMWriteIndex5A3A1C = next;
        if (wrap) {
            detail::gBattleMWriteIndex5A3A1C = 0;
        }
    }
}

// 原函数：0x0050D500。
void HandleBattleResultsReply_50D500(const std::uintptr_t, const char* const payload)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    gPendingResultWindowMode_FF0C10 = 1;
    char row[2048];
    char entry[2048];
    char name[2048];
    std::int32_t field = 1;
    for (BattleResultRecord& record : gBattleResultsFF0C14) {
        network::ExtractDelimitedField_52CED0(payload, ',', field, 2047, row);
        ++field;
        record.value00 = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(row, '|', 1));
        record.value02 = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(row, '|', 2));
        record.value04 = network::ParseDelimitedBase62_52D0D0(row, '|', 3);
    }

    network::ExtractDelimitedField_52CED0(payload, ',', field, 2047, row);
    ++field;
    for (std::int32_t index = 0; index < 3; ++index) {
        network::ExtractDelimitedField_52CED0(row, '|', index + 1, 2047, entry);
        input::UnescapeLegacyText_52D150(entry);
        gBattleResultGraphicsFF0D0C[index] =
            network::ParseDelimitedInteger_52CFA0(entry, '|', 1);
        network::ExtractDelimitedField_52CED0(entry, '|', 2, 2047, name);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        if (std::strlen(name) <= 28) {
            std::strcpy(gBattleResultNamesFF0C34[index].data(), name);
        } else {
            gBattleResultNamesFF0C34[index][0] = '\0';
        }
    }

    for (BattleSkillResultRecord& record : gBattleSkillResultsFF0C8C) {
        network::ExtractDelimitedField_52CED0(payload, ',', field, 2047, row);
        ++field;
        const auto bits = static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(row, '|', 1));
        const std::int16_t parsedValue = std::bit_cast<std::int16_t>(bits);
        record.value00 = parsedValue;
        if (parsedValue < 0) {
            record.value00 = 0;
        }
        const std::int32_t parsedBase62 = network::ParseDelimitedBase62_52D0D0(row, '|', 2);
        record.value04 = parsedBase62;
        if (parsedBase62 < 0) {
            record.value04 = 0;
        }
    }
}

// 原 [0x50CF90,0x50CFF7)：回复序号按 S16 比较，成功时清地图请求状态。
void __cdecl HandleMapTransitionReply_50CF90(
    const std::uintptr_t, const std::int32_t sequence,
    const std::int32_t status)
{
    if (gValue9A6E0C != 0) {
        return;
    }
    if (std::bit_cast<std::int16_t>(detail::gPendingMapTransitionSequence_9DAD54) == sequence) {
        gMapTransitionRequestPending_9A86D8 = 0;
        if (status == 0) {
            detail::InvalidateMapDrawPosition_4691D0();
            detail::gMapRectangleRequestPending_C82208 = 0;
            detail::gMapTransitionReplyPending_C82210 = 0;
        } else if (detail::gMapTransitionAwaitingRectangle_C82214 == 0) {
            detail::gMapTransitionReplyPending_C82210 = 0;
        }
    } else if (std::bit_cast<std::int16_t>(detail::gPendingDirectionalTransitionSequence_9FB92C) == sequence &&
               status == 0) {
        gDirectionalMapTransitionPending_9FB92E = 0;
    }
}

// 原 [0x509090,0x5090FC)：校验成功才清地图请求及一次性状态位。
void __cdecl HandleMapRectangleCrcReply_509090(
    const std::uintptr_t, const std::int32_t first,
    const std::int32_t floor, const std::int32_t xStart,
    const std::int32_t yStart, const std::int32_t xEnd,
    const std::int32_t yEnd, const std::int32_t expectedTile,
    const std::int32_t expectedObj, const std::int32_t expectedHit)
{
    if (gValue9A6E0C != 0 ||
        detail::CheckMapRectangleCrcAndRequest_469290(
            first, floor, xStart, yStart, xEnd, yEnd,
            expectedTile, expectedObj, expectedHit) == 0) {
        return;
    }
    if (gMapFloor == floor) {
        detail::gMapRectangleRequestPending_C82208 = 0;
    }
    if ((detail::gValue9A4B94 & 1u) != 0) {
        detail::InvalidateMapDrawPosition_4691D0();
        detail::gValue9A4B94 &= 0xFEu;
    }
}

// 原 [0x509100,0x5093B6)：同一局部帧中的 WORD 写入可覆盖后续原文。
std::int32_t __cdecl HandleMapRectangleDataReply_509100(
    const std::uintptr_t, const std::int32_t mapId,
    const std::int32_t floor, const std::int32_t xStart,
    const std::int32_t yStart, const std::int32_t xEnd,
    const std::int32_t yEnd, const char* const payload)
{
    // 原入口只采一次 W32 门禁，非零时透传该样本。
    const std::uint32_t gate = gValue9A6E0C;
    if (gate != 0) {
        return std::bit_cast<std::int32_t>(gate);
    }

    constexpr std::size_t frameBytes = 0x10594;
    constexpr std::size_t tileOffset = 0x64;
    constexpr std::size_t tileTextOffset = 0x1064;
    constexpr std::size_t objOffset = 0x5774;
    constexpr std::size_t objTextOffset = 0x6774;
    constexpr std::size_t hitOffset = 0xAE84;
    constexpr std::size_t hitTextOffset = 0xBE84;
    static_assert(std::endian::native == std::endian::little);
    // 原连续帧七段共用存储；仅保留既有整帧零初始化适配。
    std::array<std::uint16_t, frameBytes / 2> frame {};
    char* const bytes = reinterpret_cast<char*>(frame.data());
    std::uint16_t* const tile = frame.data() + tileOffset / 2;
    std::uint16_t* const obj = frame.data() + objOffset / 2;
    std::uint16_t* const hit = frame.data() + hitOffset / 2;
    char* const tileText = bytes + tileTextOffset;
    char* const objText = bytes + objTextOffset;
    char* const hitText = bytes + hitTextOffset;
    char* const field = bytes;

    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 1, 0x470F, tileText));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 2, 0x470F, objText));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 3, 0x470F, hitText));

    const std::int32_t firstStatus = network::ExtractDelimitedField_52CED0(
        tileText, ',', 1, 99, field);
    tile[0] = static_cast<std::uint16_t>(network::Base62Decode_52D050(field));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        objText, ',', 1, 99, field));
    obj[0] = static_cast<std::uint16_t>(network::Base62Decode_52D050(field));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        hitText, ',', 1, 99, field));
    hit[0] = static_cast<std::uint16_t>(network::Base62Decode_52D050(field));

    if (firstStatus != 1) {
        std::size_t item = 1;
        std::int32_t ordinal = 2;
        std::int32_t status;
        do {
            status = network::ExtractDelimitedField_52CED0(
                tileText, ',', ordinal, 99, field);
            tile[item] = static_cast<std::uint16_t>(
                network::Base62Decode_52D050(field));
            static_cast<void>(network::ExtractDelimitedField_52CED0(
                objText, ',', ordinal, 99, field));
            obj[item] = static_cast<std::uint16_t>(
                network::Base62Decode_52D050(field));
            static_cast<void>(network::ExtractDelimitedField_52CED0(
                hitText, ',', ordinal, 99, field));
            hit[item] = static_cast<std::uint16_t>(
                network::Base62Decode_52D050(field));
            ++item;
            ++ordinal;
        } while (status != 1);
    }

    static_cast<void>(detail::WriteMapCacheRectangle_4688A0(
        mapId, floor, xStart, yStart, xEnd, yEnd, tile, obj, hit));
    if ((detail::gWaitingForMapRectangleData_9EE8B8 != 0 || detail::gMapRectangleRequestPending_C82208 != 0)
        && gMapFloor == floor) {
        detail::InvalidateMapDrawPosition_4691D0();
        detail::gMapRectangleRequestPending_C82208 = 0;
    }
    // 原先取 BYTE 标记和当前地图号，再发布清位后的同一样本。
    const std::uint8_t replyFlags = detail::gValue9A4B94;
    const std::int32_t currentMapId = std::bit_cast<std::int32_t>(gCurrentMapId_9A86DC);
    detail::gValue9A4B94 = static_cast<std::uint8_t>(replyFlags & 0xFEu);

    if (currentMapId == mapId
        && gMapFloor == floor
        && (gPlayerX - 10) <= xStart
        && xEnd <= (gPlayerX + 10)
        && (gPlayerY - 10) <= yStart
        && yEnd <= (gPlayerY + 10)) {
        detail::gMapWindowReloadPending_9EE910 = 1;
    }

    const std::uint32_t state = detail::gMapTransitionAwaitingRectangle_C82214;
    if (state != 1) {
        return std::bit_cast<std::int32_t>(state);
    }
    const std::int32_t currentFloor = gMapFloor;
    if (currentFloor != floor) {
        return currentFloor;
    }
    detail::gMapTransitionReplyPending_C82210 = 0;
    detail::gMapTransitionAwaitingRectangle_C82214 = 0;
    detail::gMapNameReceivedDuringTransition_C82218 = 0;
    return 0;
}

// 原 [0x5093F0,0x509548)：保留地图保存、场景清理和加密对象赋值顺序。
std::int32_t __cdecl HandleMapChangeReply_5093F0(
    const std::uintptr_t, const std::int32_t a2, const std::int32_t a3,
    const std::int32_t a4, const std::int32_t a5, const std::int32_t a6,
    const std::int32_t a7, const std::int32_t a8, const std::int32_t a9,
    const std::int32_t a10, const std::int32_t a11, const std::int32_t a12,
    const std::int32_t a13)
{
    const std::uint8_t priorFlag = detail::gValue9A4B94;
    detail::gMapRectangleRequestPending_C82208 = 1;
    if (priorFlag == 0 && gCurrentSceneType == 9) {
        if (detail::gMapTransitionObservationPrepared_C8220C == 0) {
            gCurrentSceneState = 200;
            static_cast<void>(detail::PrepareMapTransitionObservationState_51D5B0());
        }
        static_cast<void>(detail::DestroyPlayerMapPresence_519D50());
        detail::gMapTransitionObservationPrepared_C8220C = 0;
    }

    static_cast<void>(detail::EnsureMapCacheFile_468570(
        gMapCachePathKey_9E0434, a2, a3, a4, a5, a8, a9));
    detail::SetWorldMapMetadata_4682E0(
        static_cast<std::uint32_t>(a2), a3,
        static_cast<std::uint32_t>(a4), static_cast<std::uint32_t>(a5),
        static_cast<std::uint32_t>(a8), static_cast<std::uint32_t>(a9),
        static_cast<std::uint32_t>(a10), a12);
    static_cast<void>(detail::SetPlayerMapPosition_468330(a6, a7));
    detail::ClearWorldObjectsAndActions_4464D0();

    const std::uint32_t mask = gValue9FB918.mask;
    const std::uint32_t encoded = gValue9FB918.encoded;
    const std::uint8_t readCount = gValue9FB918.readCount;
    gValueC821E8.mask = mask;
    detail::gWaitingForMapRectangleData_9EE8B8 = 0;
    gValueC821E8.encoded = encoded;
    gValueC821E8.readCount = readCount;

    gValue9E03C0 = std::int32_t{0};
    static_cast<void>(RestorePlayerMovementSnapshot_4681B0());

    if (a11 > 0) {
        gPendingMapBgmIndex_FF0D5C = static_cast<std::uint32_t>(a11);
        gMapBgmChangePending_FF0D58 = 1;
    }
    detail::gValue9EE8F8 = static_cast<std::uint32_t>(a13);
    return a11 > 0 ? a13 : a11;
}

// 原 [0x509550,0x509642)：原位反转义、两段提取与地图回复状态发布。
// PTR01：原509550完整82条；地图名整词先发/回复状态/尾字节，atoi持值判负，原提取与返回保持。
std::int32_t __cdecl HandleMapNameReply_509550(
    const std::uintptr_t, char* const payload)
{
    std::array<char, 256> text;
    std::array<char, 32> number;
    input::UnescapeLegacyText_52D150(payload);
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 1, 255, text.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(text.data());

    const char* const selected =
        std::strlen(text.data()) > 24 ? kUnknownText554590 : text.data();
    const std::size_t selectedBytes = std::strlen(selected) + 1;
    const std::size_t wordBytes = selectedBytes & ~std::size_t{3};
    std::memcpy(gText9DAD30.data(), selected, wordBytes);
    detail::gPendingMapPaletteIndex_FF0D4C = -2;
    std::memcpy(gText9DAD30.data() + wordBytes, selected + wordBytes, selectedBytes & 3u);

    static_cast<void>(network::ExtractDelimitedField_52CED0(
        payload, '|', 2, 31, number.data()));
    if (number[0] == '\0') {
        detail::gPendingMapPaletteIndex_FF0D4C = -1;
    } else {
        const std::int32_t value = std::atoi(number.data());
        detail::gPendingMapPaletteIndex_FF0D4C = value;
        if (value < 0) {
            detail::gPendingMapPaletteIndex_FF0D4C = -1;
        }
    }
    if (detail::gMapTransitionReplyPending_C82210 == 1) {
        detail::gMapNameReceivedDuringTransition_C82218 = 1;
    }
    return 1;
}

// 原 [51D640,51D6E3)：恢复 C82204 后拷贝整面，queueMode 固定写回 1。
std::uint32_t CaptureMapTilesForTransition_51D640()
{
    static_cast<void>(detail::InitializeMapProjectionOrigin_468F70());
    platform::gPendingGraphicCount = 0;
    platform::gPendingGraphicCount2 = 0;
    static_cast<void>(render::ResetGraphicCache_456D30());
    static_cast<void>(detail::LoadAndDrawMapTiles_469030());
    render::SortTextures_520790();
    static_cast<void>(render::ClearSurface_4512C0());
    const std::uint8_t previousC82204 = gValueC82204;
    gValueC82204 = 0;
    render::gRenderQueueMode103CF10 = 0;
    render::DrawAllQueued_520DD0();
    gValueC82204 = previousC82204;
    static_cast<void>(render::CopyBackToTemp());
    render::gRenderQueueMode103CF10 = 1;
    platform::gPendingGraphicCount = 0;
    platform::gPendingGraphicCount2 = 0;
    static_cast<void>(render::ResetGraphicCache_456D30());
    const std::uint64_t tick = platform::ReadTickCount64();
    platform::gLastRenderDt = static_cast<double>(tick);
    return static_cast<std::uint32_t>(tick);
}

// GTP01：原 [0x50C870,0x50C8C8)；保两次 time/旧 adjustment，持有时段交调色板后再发布两字段。
std::int32_t __cdecl HandlePlayerWorldTimeReply_50C870(
    const std::uintptr_t, const std::int32_t playerValue,
    const std::int32_t timeOffset, const std::int32_t pathValue,
    const std::int32_t adjustment)
{
    static_cast<void>(detail::SetPlayerObjectIndex_519EB0(playerValue));
    gServerTimeAtSyncSeconds_73ED4C = std::bit_cast<std::uint32_t>(timeOffset);
    gLocalTimeAtSyncSeconds_73ED68 = static_cast<std::uint64_t>(
        ::_time64(nullptr));
    static_cast<void>(UpdateGameTime(&gCgTime));
    const std::int32_t category = GetCate_458EC0(&gCgTime);
    gCurrentDayPeriod_73ED48 = category;
    static_cast<void>(render::SelectPalette(category, 0));
    gMapCachePathKey_9E0434 = pathValue;
    gGameTimeAdjustmentSeconds_73ED40 = std::bit_cast<std::uint32_t>(adjustment);
    return adjustment;
}

// 原 [0x46CF00, 0x46CF1A)；上界外返回入口位模式，上界内读唯一共享表。
std::int32_t __cdecl ResolveMapGraphicNumber_46CF00(const std::int32_t value)
{
    if (static_cast<std::uint32_t>(value) >= 65536u) {
        return value;
    }
    if (value < 0) {
        return 0;
    }
    return detail::gValuesC35010[static_cast<std::size_t>(value)];
}

// WDG01：原 [0x46F8F0,0x46F92B)；上/下界比较后各重新取 F32，再加减、写回并重来。
void __cdecl NormalizeAngleDegrees_46F8F0(float* const value)
{
    constexpr float kFullTurn = 360.0F;
    for (;;) {
        const float upperValue = *value;
        if (upperValue >= 360.0) {
            const float updateValue = *value;
            *value = updateValue - kFullTurn;
            continue;
        }

        const float lowerValue = *value;
        if (lowerValue >= 0.0) {
            return;
        }
        const float updateValue = *value;
        *value = updateValue + kFullTurn;
    }
}

// 原 [0x46F930,0x46F94E)：F32 归一化、向零截断索引，原表 F64 位型返回。
double __cdecl LookupSineDegrees_46F930(float angle)
{
    NormalizeAngleDegrees_46F8F0(&angle);
    const std::int32_t index = static_cast<std::int32_t>(angle);
    return std::bit_cast<double>(
        gAngleTableBits55DF10[static_cast<std::size_t>(index)]);
}

// 原 [0x46F950,0x46F967)：先将 +90 的结果写为 F32，再透传表读取结果。
double __cdecl LookupCosineDegrees_46F950(const float angle)
{
    const float shifted = angle + 90.0F;
    return LookupSineDegrees_46F930(shifted);
}

// 原 [0x46F970,0x46FACF)：原表插值及八路象限修正，F32 写槽独立保留。
double __cdecl GetVectorAngleDegrees_46F970(const float first, const float second)
{
    if (first == 0.0F && second == 0.0F) {
        return 0.0;
    }

    const bool firstNegative = first < 0.0F;
    const bool secondNegative = second < 0.0F;
    const double firstAbs = firstNegative ? -first : first;
    const double secondAbs = secondNegative ? -second : second;
    const bool swapAxes = firstAbs < secondAbs;
    const std::int32_t selector =
        (firstNegative ? 1 : 0) + (secondNegative ? 2 : 0) + (swapAxes ? 4 : 0);
    const double scaled =
        (swapAxes ? firstAbs / secondAbs : secondAbs / firstAbs) * 64.0;
    const std::int32_t index = static_cast<std::int32_t>(scaled);
    const std::int32_t nextIndex = index + 1 > 64 ? 64 : index + 1;
    const float fraction = static_cast<float>(scaled - index);
    const double leftSample = std::bit_cast<double>(
        gAngleTableBits55EA50[static_cast<std::size_t>(index)]);
    const double firstWeight = 1.0 - fraction;
    const double left = leftSample * firstWeight;
    const double rightSample = std::bit_cast<double>(
        gAngleTableBits55EA50[static_cast<std::size_t>(nextIndex)]);
    const double right = rightSample * fraction;
    const float angle = static_cast<float>(left + right);

    switch (selector) {
    case 0: return angle;
    case 1: return 180.0 - angle;
    case 2: return 360.0 - angle;
    case 3: return angle + 180.0;
    case 4: return 90.0 - angle;
    case 5: return angle + 90.0;
    case 6: return angle + 270.0;
    case 7: return 270.0 - angle;
    default: return 0.0;
    }
}

// 原函数：0x0046FB80。
double GetSquaredWorldDistance_46FB80(
    const float x, const float y, const float targetX, const float targetY)
{
    const double differenceX = static_cast<double>(targetX) - x;
    const double differenceY = static_cast<double>(targetY) - y;
    const double squaredY = differenceY * differenceY;
    const double squaredX = differenceX * differenceX;
    return squaredY + squaredX;
}

// WDG01：原 [0x46CDF0,0x46CE25)；查表加原偏移落 F32、原地归一后重取，乘原常量转 S64。
std::int64_t __cdecl GetEightWayMapDirection_46CDF0(const float first, const float second)
{
    const double direction = GetVectorAngleDegrees_46F970(first, second);
    const float offset = 112.5F;
    float angle = static_cast<float>(direction + offset);
    NormalizeAngleDegrees_46F8F0(&angle);
    constexpr float multiplier = 1.0F / 45.0F;
    return static_cast<std::int64_t>(
        static_cast<double>(angle) * multiplier);
}

// 原[440B50,440B6C)：只采一次场景值，以U32比较250..252。
std::int32_t IsObservationSceneTransitionActive_440B50()
{
    const auto scene = static_cast<std::uint32_t>(gCurrentSceneState);
    return scene >= 250u && scene <= 252u ? 1 : 0;
}

// 原 [0x4410E0,0x4410E6)：无状态固定返回叶。
std::int32_t GetActorStatusIconCount_4410E0()
{
    return 0x11;
}

std::uint32_t ReadPlayerFlags18A() noexcept
{
    // 原FDA88A一次跨字段W32读取，flags18A与flags18C低W16相邻。
    const auto* const bytes = reinterpret_cast<const std::uint8_t*>(&gPlayerRuntimeSlots);
    std::uint32_t flags;
    std::memcpy(&flags, bytes + offsetof(PlayerRuntimeSlots, flags18A), sizeof(flags));
    return flags;
}

// 原函数：0x00405B40。
std::int32_t GetBattlePetPosition_405B40(const std::int32_t position)
{
    if ((position >= 0 && position < 5) || (position >= 10 && position < 15)) {
        return position + 5;
    }
    return position - 5;
}

// BEI01：原九条，唯一玩家item区8..27逐used W16统计；现正确正文保持，十九调用点的完整外父/owner与运行待。
// 原函数：0x00518C60。
std::int32_t CountFreeInventorySlots_518C60()
{
    std::int32_t count = 0;
    for (std::int32_t slot = 8; slot < 28; ++slot) {
        if (gPlayerRuntimeSlots.items[slot].used == 0) {
            ++count;
        }
    }
    return count;
}

const char* GetPlayerJobName() noexcept
{
    return gSkillRuntimeSlots.jobName.data();
}

void ConsumeBattlePlayerTimer(const std::int32_t seconds)
{
    const std::int32_t remaining = gPlayerRuntimeSlots.timerRemaining0C - 1000 * seconds;
    const bool negative = remaining < 0;
    gPlayerRuntimeSlots.timerRemaining0C = remaining;
    if (negative) {
        gPlayerRuntimeSlots.timerRemaining0C = 0;
    }
}

// SHP01：原玩家正加点优先，否则五S16配对首个宠物非零加点位置；返回5/6原哨兵。
// 原函数：0x00504B10。
std::int32_t FindAttributeAllocationHintTarget_504B10()
{
    if (gValueD2F788 > 0) {
        return 5;
    }
    for (std::int32_t position = 0; position < 5; ++position) {
        const std::int16_t index = std::bit_cast<std::int16_t>(gPetPairsDE0B08[position].index);
        if (gPetRuntimeRecords[index].lvFlag6C != 0) {
            return position;
        }
    }
    return 6;
}

// G04：五S16配对顺序，used且伤势大于0才返回原宠物编号，未命中-1。
// 原函数：0x00504B60。
std::int32_t FindFirstInjuredPet_504B60()
{
    for (std::int32_t position = 0; position < 5; ++position) {
        if (gPetPairsDE0B08[position].used == 0) {
            continue;
        }
        const std::int16_t index = std::bit_cast<std::int16_t>(gPetPairsDE0B08[position].index);
        if (gPetRuntimeRecords[index].values70[25] > 0) {
            return index;
        }
    }
    return -1;
}

std::int16_t ReadPlayerTribe() noexcept
{
    return std::bit_cast<std::int16_t>(gPlayerRuntimeSlots.value00);
}

std::uint8_t ReadPlayerActionFlagsByte() noexcept
{
    return reinterpret_cast<const unsigned char*>(&gPlayerRuntimeSlots.flags18C)[2];
}

std::uint16_t ReadPlayerActionFlags() noexcept
{
    return static_cast<std::uint16_t>(gPlayerRuntimeSlots.flags18C >> 16);
}

void ClearPlayerActionFlags(const std::uint16_t mask) noexcept
{
    gPlayerRuntimeSlots.flags18C &= ~(static_cast<std::uint32_t>(mask) << 16);
}

void SetPlayerActionFlags(const std::uint16_t mask) noexcept
{
    gPlayerRuntimeSlots.flags18C |= static_cast<std::uint32_t>(mask) << 16;
}

void CopyPlayerBarValues(
    platform::LegacyCryptInt* const hp, platform::LegacyCryptInt* const maxHp,
    platform::LegacyCryptInt* const fp, platform::LegacyCryptInt* const maxFp)
{
    *hp = gPlayerRuntimeSlots.hp;
    *maxHp = gPlayerRuntimeSlots.maxHp;
    *fp = gPlayerRuntimeSlots.fp;
    *maxFp = gPlayerRuntimeSlots.maxFp;
}

const char* GetTitleText(const std::int32_t index) noexcept
{
    // 未装备或无效索引返回空文本；修正原FF0510+index*1Ch越界读。
    if (index < 0 || index >= static_cast<std::int32_t>(detail::gTitlesFF0508.size())) {
        return "";
    }
    return detail::gTitlesFF0508[index].text.data();
}

const char* GetPlayerDisplayText(const std::int16_t mode) noexcept
{
    switch (mode) {
    case 1: return gPlayerRuntimeSlots.name.data();
    case 2: return gPlayerRuntimeSlots.ownTitle.data();
    case 3: return GetTitleText(gPlayerRuntimeSlots.titleIndex100);
    case 4: return gPlayerRuntimeSlots.text12A.data();
    case 5: return gPlayerRuntimeSlots.text14B.data();
    case 6: return gPlayerRuntimeSlots.text15BB8.data();
    default: return nullptr;
    }
}

std::int16_t ReadDirectionDeltaX(const std::int16_t direction) noexcept
{
    return gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].x;
}

std::int16_t ReadDirectionDeltaY(const std::int16_t direction) noexcept
{
    return gDirectionDeltas55D9DC[static_cast<std::size_t>(direction)].y;
}

cg::yobi::YobiAction* ReadPetMapAction(const std::int16_t index) noexcept
{
    return gPetMapCharaSlots[static_cast<std::size_t>(index)].action;
}

std::uint32_t ReadPetMapObjectIndex(const std::int16_t index) noexcept
{
    return gPetMapCharaSlots[static_cast<std::size_t>(index)].objectIndex;
}

std::uint16_t ReadPetMapFlags(const std::int16_t index) noexcept
{
    return gPetMapCharaSlots[static_cast<std::size_t>(index)].flags;
}

// 六请求的门禁、声音和计时差异见doc/ui_windows/window_4.md。
// 原函数：0x00470050。
void RequestDuelWithFacingPlayer_470050()
{
    const std::int32_t direction = gPlayerRuntimeSlots.direction;
    if (direction < 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::int32_t deltaX = gDirectionDeltas55D9DC[direction].x;
    const std::int32_t deltaY = gDirectionDeltas55D9DC[direction].y;
    const std::int32_t targetY =
        (gPlayerY + static_cast<std::int32_t>(deltaY));
    const std::int32_t targetX =
        (gPlayerX + static_cast<std::int32_t>(deltaX));
    const std::int32_t target = detail::HasWorldObjectAt_4450D0(targetX, targetY, 8);
    const std::uint16_t follow = detail::gValueFF0D36;
    if ((follow != 0 && (follow != 1 || (ReadPlayerFlags18A() & 0x100u) == 0))
        || target != 1 || gMapTransitionRequestPending_9A86D8 != 0 || gDirectionalMapTransitionPending_9FB92E != 0
        || !(gPlayerCoordinateSyncPending_C821D8 == 0)) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::uint64_t now = platform::ReadTickCount64();
    if (ui::detail::gWindowActionTick62B5AC + 500u < now) {
        const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
        static_cast<void>(network::SendDuelRequest_510880(platform::gConnectionSocket, x, y));
        ui::detail::gWindowActionTick62B5AC = platform::ReadTickCount64();
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
}

// 原函数：0x00470150。
void RequestSpectateFacingBattle_470150()
{
    const std::int32_t direction = gPlayerRuntimeSlots.direction;
    if (direction < 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::int32_t deltaX = gDirectionDeltas55D9DC[direction].x;
    const std::int32_t deltaY = gDirectionDeltas55D9DC[direction].y;
    std::int32_t target = 0;
    if (detail::IsMapBattleSpectatingAllowed_455A90() == 1) {
        const std::int32_t targetY =
            (gPlayerY + static_cast<std::int32_t>(deltaY));
        const std::int32_t targetX =
            (gPlayerX + static_cast<std::int32_t>(deltaX));
        target = detail::HasWorldObjectAtWithFlags_4451F0(targetX, targetY, 8, 0xC400, 0);
    }
    const std::uint16_t follow = detail::gValueFF0D36;
    if ((follow != 0 && (follow != 1 || (ReadPlayerFlags18A() & 0x100u) == 0))
        || target != 1 || gMapTransitionRequestPending_9A86D8 != 0 || gDirectionalMapTransitionPending_9FB92E != 0
        || !(gPlayerCoordinateSyncPending_C821D8 == 0)) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::uint64_t now = platform::ReadTickCount64();
    if (ui::detail::gWindowActionTick62B5AC + 500u < now) {
        const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
        static_cast<void>(network::SendSpectateRequest_510940(platform::gConnectionSocket, x, y));
        ui::detail::gWindowActionTick62B5AC = platform::ReadTickCount64();
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
}

// 原函数：0x00470270。
void RequestPartyJoinOrLeave_470270()
{
    const bool joinTeam = detail::gValueFF0D36 == 0;
    if (joinTeam) {
        const std::int32_t direction = gPlayerRuntimeSlots.direction;
        if (direction < 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            return;
        }
        const std::int32_t deltaX = gDirectionDeltas55D9DC[direction].x;
        const std::int32_t deltaY = gDirectionDeltas55D9DC[direction].y;
        const std::int32_t targetY =
            (gPlayerY + static_cast<std::int32_t>(deltaY));
        const std::int32_t targetX =
            (gPlayerX + static_cast<std::int32_t>(deltaX));
        if (detail::HasWorldObjectAtWithoutFlags_4452A0(targetX, targetY, 0x28, 0x400, 0) != 1) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            return;
        }
    }
    if (gMapTransitionRequestPending_9A86D8 != 0 || gDirectionalMapTransitionPending_9FB92E != 0
        || !(gPlayerCoordinateSyncPending_C821D8 == 0)) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::uint64_t now = platform::ReadTickCount64();
    if (ui::detail::gWindowActionTick62B5AC + 500u < now) {
        if (joinTeam && (ReadPlayerFlags18A() & 0x100u) != 0) {
            return;
        }
        const std::int32_t mode = joinTeam ? 1 : 0;
        const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
        static_cast<void>(network::SendTeamJoinLeaveRequest_511A50(platform::gConnectionSocket, x, y, mode));
        ui::detail::gWindowActionTick62B5AC = platform::ReadTickCount64();
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
}

// 原函数：0x004703C0。
void RequestFacingPlayerContactCard_4703C0()
{
    const std::int32_t direction = gPlayerRuntimeSlots.direction;
    if (direction < 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::int32_t deltaX = gDirectionDeltas55D9DC[direction].x;
    const std::int32_t deltaY = gDirectionDeltas55D9DC[direction].y;
    const std::int32_t targetY =
        (gPlayerY + static_cast<std::int32_t>(deltaY));
    const std::int32_t targetX =
        (gPlayerX + static_cast<std::int32_t>(deltaX));
    if (detail::HasWorldObjectAt_4450D0(targetX, targetY, 8) != 1
        || gMapTransitionRequestPending_9A86D8 != 0 || gDirectionalMapTransitionPending_9FB92E != 0
        || !(gPlayerCoordinateSyncPending_C821D8 == 0)) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::uint64_t now = platform::ReadTickCount64();
    if (ui::detail::gWindowActionTick62B5AC + 500u < now) {
        const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
        static_cast<void>(network::SendContactCardRequest_5111E0(platform::gConnectionSocket, x, y));
        ui::detail::gWindowActionTick62B5AC = platform::ReadTickCount64();
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
}

// 原函数：0x004704A0。
void RequestNearbyPlayerTrade_4704A0()
{
    const std::int32_t direction = gPlayerRuntimeSlots.direction;
    if (direction < 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::int32_t deltaX = gDirectionDeltas55D9DC[direction].x;
    const std::int32_t deltaY = gDirectionDeltas55D9DC[direction].y;
    const std::int32_t firstY =
        (gPlayerY + static_cast<std::int32_t>(deltaY));
    const std::int32_t firstX =
        (gPlayerX + static_cast<std::int32_t>(deltaX));
    const std::int32_t first = detail::HasWorldObjectAt_4450D0(firstX, firstY, 8);
    const std::int32_t secondY =
        (gPlayerY + static_cast<std::int32_t>(deltaY * 2));
    const std::int32_t secondX =
        (gPlayerX + static_cast<std::int32_t>(deltaX * 2));
    const std::int32_t second = detail::HasWorldObjectAt_4450D0(secondX, secondY, 8);
    if (detail::gValueFF0D36 != 1 && first != 1 && second != 1) {
        AppendChatMessage_44B050(L"没有可交易的对象！" /* 原常量参考：0x00556CC4。 */, 4, 0);
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    if (gMapTransitionRequestPending_9A86D8 == 0 && gDirectionalMapTransitionPending_9FB92E == 0
        && (gPlayerCoordinateSyncPending_C821D8 == 0)) {
        const std::uint64_t now = platform::ReadTickCount64();
        if (ui::detail::gWindowActionTick62B5AC + 500u < now) {
            static_cast<void>(network::SendTradeRequest_511FB0(platform::gConnectionSocket));
            ui::detail::gWindowActionTick62B5AC = platform::ReadTickCount64();
        }
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
}

// 原函数：0x00470670。
void RequestFacingPlayerGuildInvitation_470670()
{
    const std::int32_t direction = gPlayerRuntimeSlots.direction;
    if (direction < 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::int32_t deltaX = gDirectionDeltas55D9DC[direction].x;
    const std::int32_t deltaY = gDirectionDeltas55D9DC[direction].y;
    const std::int32_t targetY =
        (gPlayerY + static_cast<std::int32_t>(deltaY));
    const std::int32_t targetX =
        (gPlayerX + static_cast<std::int32_t>(deltaX));
    if (detail::HasWorldObjectAt_4450D0(targetX, targetY, 8) != 1
        || gMapTransitionRequestPending_9A86D8 != 0 || gDirectionalMapTransitionPending_9FB92E != 0
        || !(gPlayerCoordinateSyncPending_C821D8 == 0)) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::int32_t rank = cg::world::ReadGuildTextIndex10514B0();
    if ((cg::world::ReadGuildRankFlagsByte1051514(rank) & 1u) == 0) {
        AppendChatMessage_44B050(LR"(您没有家族唷，所以不能邀请对方捏...^^")" /* 原常量参考：0x005862AC。 */, 4, 0);
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        return;
    }
    const std::uint64_t now = platform::ReadTickCount64();
    if (ui::detail::gWindowActionTick62B5AC + 500u < now) {
        const std::int32_t y = static_cast<std::int32_t>(gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(gPlayerX);
        static_cast<void>(network::SendGuildInvitation_5113C0(platform::gConnectionSocket, x, y));
        ui::detail::gWindowActionTick62B5AC = platform::ReadTickCount64();
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
}

// 原函数：0x004FA8E0。
void RequestPlayerEmote_4FA8E0(const std::int32_t index)
{
    static_cast<void>(detail::RequestEmoteWithResult_4FA8E0(index));
}

// 原函数：0x00519EC0。
std::uint32_t SetPlayerMapCoordinates_519EC0(const std::int32_t x, const std::int32_t y)
{
    return detail::SetPlayerMapPosition_468330(x, y);
}

// 原函数：0x0051C0D0。
std::int32_t ResolveRidingCharacterImage_51C0D0(const std::int32_t imageNo)
{
    // 保留原区段顺序和两种除法，不扩展为通用图号规则。
    struct ImageRange {
        std::int32_t first;
        std::int32_t last;
        std::int32_t base;
    };
    constexpr ImageRange grouped[] {
        {100000, 100173, 114000}, {100250, 100423, 114028},
        {105000, 105173, 114056}, {105250, 105423, 114084},
        {106000, 106173, 114112}, {106250, 106423, 114140},
        {105500, 105548, 112000},
    };
    for (const auto& range : grouped) {
        if (imageNo >= range.first && imageNo <= range.last) {
            const std::int32_t offset = imageNo - range.first;
            return offset % 25 / 6 + 4 * (offset / 25) + range.base;
        }
    }
    constexpr ImageRange linear[] {
        {104400, 104429, 112010}, {104430, 104459, 112015},
        {104460, 104489, 112020}, {104490, 104519, 112025},
        {120100, 120123, 120148}, {120124, 120147, 120152},
    };
    for (const auto& range : linear) {
        if (imageNo >= range.first && imageNo <= range.last) {
            return (imageNo - range.first) / 6 + range.base;
        }
    }
    if (imageNo >= 120156 && imageNo <= 120159) {
        return imageNo - 8;
    }
    if (imageNo >= 120160 && imageNo <= 120163) {
        return imageNo - 8;
    }
    if (imageNo == 100900) {
        return 114169;
    }
    if (imageNo == 100901) {
        return 114168;
    }
    return imageNo;
}

// 原函数：0x0051BEE0。
void SuspendPlayerForObservation_51BEE0()
{
    reinterpret_cast<unsigned char*>(&gPlayerRuntimeSlots.flags18C)[2] |= 0x80u;
}

// 原函数：0x0051BEC0。
std::int32_t GetPlayerStatusIcon_51BEC0()
{
    const std::uint8_t flags =
        reinterpret_cast<const std::uint8_t*>(&gPlayerRuntimeSlots.flags18C)[2];
    const std::int32_t value = gPlayerRuntimeSlots.value190;
    return (flags & 2u) != 0 ? value : 0;
}

// 原函数：0x0051BE50。
std::int8_t RequestPlayerStatusIcon_51BE50(const std::int32_t requested)
{
    const std::uint8_t flags =
        reinterpret_cast<const std::uint8_t*>(&gPlayerRuntimeSlots.flags18C)[2];
    if ((flags & 2u) != 0 && gPlayerRuntimeSlots.value190 == requested) {
        return std::bit_cast<std::int8_t>(flags);
    }

    std::int32_t value = requested;
    if (value < 0 || value >= GetActorStatusIconCount_4410E0()) {
        value = 0;
    }
    const std::int32_t y = static_cast<std::int32_t>(gPlayerSnapshotY);
    const std::int32_t x = static_cast<std::int32_t>(gPlayerSnapshotX);
    const std::int32_t result =
        network::SendPlayerMotionRequest_511E60(platform::gConnectionSocket, x, y, value);
    // 原51BE97/51BEA5：先W8置位，动作0时另作同地址W16清位。
    auto* const actionBytes = reinterpret_cast<std::uint8_t*>(&gPlayerRuntimeSlots.flags18C) + 2;
    actionBytes[0] |= 2u;
    if (value == 0) {
        std::uint16_t actionFlags;
        std::memcpy(&actionFlags, actionBytes, sizeof(actionFlags));
        actionFlags &= 0xFFFDu;
        std::memcpy(actionBytes, &actionFlags, sizeof(actionFlags));
    }
    gPlayerRuntimeSlots.value190 = value;
    return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(result));
}

// 原 [0x46BD40,0x46BD57)：按指令顺序清四个独立 W32 槽，返回完整 EAX32 零。
std::int32_t ResetMapDrawCacheCounts_46BD40()
{
    detail::gPrimaryMapGraphicCount_9FB934 = 0;
    detail::gMapObjectGraphicCount_9FB928 = 0;
    detail::gSpecialMapGraphicCount_9A86C0 = 0;
    detail::gValue9A7F78 = 0;
    return 0;
}

} // namespace cg::world
