#pragma once

#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../ui/WindowRuntimeState.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

namespace cg::yobi {

// Yobi对象cacheIndex的未入队哨兵；其他索引及容量不属于此域。
enum YobiGraphicCacheIndex : std::int32_t {
    YobiGraphicCacheIndex_NotQueued = -2,
};


using YobiCallback = void (*)(YobiAction* action);

namespace detail {

// 原41EAB0：克隆战斗对象及原生载荷，保留新节点链表位置，内部T*仍为借用引用。
YobiAction* CloneBattleAction_41EAB0(YobiAction* source);


struct BattleActorPayload;
struct BattleScenePayload;
struct BattleRideCarrierPayload;
struct BattleRingEmitterPayload;
struct BattleFollowOffsetPayload;
struct BattleTransitionEffectPayload;
struct BattleTextPayload;
struct MapActorPayload;
struct MapCharaPayload;
struct MapTransitionPayload;
struct WindowOpeningPayload;
struct YobiUIData;

} // namespace detail

// 独立分配的载荷指针；只访问YobiAction::payloadKind对应的有效成员。
union YobiPayload {
    void* raw = nullptr;
    cg::yobi::detail::BattleActorPayload* battleActor;
    cg::yobi::detail::BattleScenePayload* battleScene;
    cg::yobi::detail::BattleRideCarrierPayload* battleRideCarrier;
    cg::yobi::detail::BattleRingEmitterPayload* battleRingEmitter;
    cg::yobi::detail::BattleFollowOffsetPayload* battleFollowOffset;
    cg::yobi::detail::BattleTransitionEffectPayload* battleTransitionEffect;
    cg::yobi::detail::BattleTextPayload* battleText;
    cg::yobi::detail::MapActorPayload* mapActor;
    cg::yobi::detail::MapCharaPayload* mapChara;
    cg::yobi::detail::MapTransitionPayload* mapTransition;
    cg::yobi::detail::WindowOpeningPayload* windowOpening;
    cg::yobi::detail::YobiUIData* windowUi;
};

static_assert(sizeof(YobiPayload) == sizeof(void*));
static_assert(alignof(YobiPayload) == alignof(void*));
static_assert(std::is_trivially_copyable_v<YobiPayload>);
static_assert(sizeof(YobiPayloadKind) == 1);

// 原+290..2A3；只有+294已证为战斗动作113的S32时长，其余保留原字节。
struct YobiExtraState290 {
    std::array<std::byte, 4> bytes290;
    std::int32_t castDuration294;
    std::array<std::byte, 12> bytes298;
};

static_assert(sizeof(YobiExtraState290) == 20);
static_assert(alignof(YobiExtraState290) == alignof(std::int32_t));
static_assert(offsetof(YobiExtraState290, castDuration294) == 4);
static_assert(offsetof(YobiExtraState290, bytes298) == 8);
static_assert(std::is_trivially_copyable_v<YobiExtraState290>);

// 原 x86 +offset 只作为字段来源证据；原生 x64 对象不复制 0x330 指针布局。
struct YobiAction {
    // 预览沿声明顺序默认构造十成员；普通动态节点显式零初始化普通字段，不额外消费RNG。
    YobiAction() = default;
    ~YobiAction() = default;
    struct RawFields {}; // 普通动态节点构造方式，不新增RNG。
    explicit YobiAction(RawFields);
    YobiAction* prev = nullptr; // 原成员偏移：+0x00。
    YobiAction* next = nullptr; // 原成员偏移：+0x04。
    YobiCallback callback = nullptr; // 原成员偏移：+0x08。
    YobiPayload payload{}; // 原成员偏移：+0x0C。 本机使用带类别的指针联合体。
    // YUF01：+10按创建入口／callback选择下列非拥有引用；T*与完成槽T**不能交叉解引用。
    union {
        YobiAction** completionSlot10 = nullptr; // 原+10，444260/4441A0的非拥有完成回写槽。
        YobiAction* observationParent10; // 同一原+10，43F290/43FE00的非拥有观察父节点。
        YobiAction* battleAttachmentParent10; // 同一原+10，附件/特效/粒子发射器的非拥有父引用。
        YobiAction* battleOpponent10; // 同一原+10，416DD0/412900的非拥有对象引用；412900另快照原值。
        YobiAction* battleCastEffect10; // 同一原+10，413400保存创建返回的非拥有施法特效。
    };
    std::uint8_t priority = 0; // 原成员偏移：+0x14。
    std::uint8_t displayPriority = 0; // 原成员偏移：+0x15。
    YobiPayloadKind payloadKind = YobiPayloadKind::None; // 本机载荷类别，使用原生对齐空隙，不复用业务字段。
    std::int32_t cacheIndex = 0; // 原成员偏移：+0x18。
    std::int32_t deathFlag = 0; // 原成员偏移：+0x1C。
    std::int32_t graphicIndex = 0; // 原成员偏移：+0x20。
    std::uint32_t flags = 0; // 原成员偏移：+0x24。
    std::int32_t counter28 = 0; // 原+28，440440退出轨迹的独立计数。
    // 普通phase与数量窗口回调互斥；同一payloadKind不能代替callback家族判定。
    union {
        std::int32_t phase; // 原+2C，普通对象的S32阶段。
        ui::WindowValueCallback numberWindowCallback2C = nullptr; // 原成员偏移：+0x2C。 48AC60数量族的非拥有函数指针。
    };
    std::int32_t x = 0; // 原成员偏移：+0x30。
    std::int32_t y = 0; // 原成员偏移：+0x34。
    std::int32_t deltaX = 0; // 原+38；窗口横向步长，41F570/41FB20复用为计数。
    std::int32_t deltaY = 0; // 原+3C；窗口/排序字段，41F570复用为寿命上限，不与y合并。
    float offset40 = 0.0f; // 原+40；448A50/448BF0/449A00的投影后横向偏移。
    float offset44 = 0.0f; // 原+44；对应纵向偏移，不与屏幕y合并。
    float effectAcceleration48 = 0.0f; // 原+48，420D50/41A310写0.2，41A7D0写0.1，41A200从4C扣除；4087C0/4090F0复用为回旋转向步长。
    std::uint32_t verticalVelocityBits4C = 0; // 原+4C；41F4C0/41F5B0写C1000000，41F450/41A200按F32消费。
    float angle50 = 0.0f; // 原+50；本对象族创建清零，449160另作角度。
    float motionMagnitude54 = 0.0f; // 原+54；三路对象速度及其他回调的幅值，共用F32。
    std::int32_t field58 = 0; // 原+58；共用保存方向、骑乘计时、命中延迟及发射完成计数，不能统一为direction。
    float scaleX = 0.0f; // 原成员偏移：+0x5C。
    float scaleY = 0.0f; // 原成员偏移：+0x60。
    std::wstring displayName; // 原 +64..+A3；绘制文本，内部使用宽字符串。
    std::int32_t nameX = 0;              // 原 +A4；DrawText2消费低S16，41A7D0作S32生成计数。
    std::int32_t nameY = 0;              // 原+A8；下落族作终点，41B440帧事件族作S32消退计数。
    float nameOffsetAC = 0.0f; // 原+AC，伤害数字创建/更新投影到nameX。
    float nameOffsetB0 = 0.0f; // 原+B0，伤害数字创建/更新投影到nameY。
    float battleTurnStepB4 = 0.0f; // 原+B4；41C3B0/4219D0转向步长，40F800复用为F32目标缩放。
    float numberVelocityB8 = 0.0f; // 原+B8，41F1A0写-8，41F110更新独立数字速度。
    std::int8_t nameColor = 0;           // 原 +BC。
    std::int8_t nameFontType = 0;        // 原 +BD。
    std::int8_t nameLayer = 0;           // 原 +BE；DrawText2 的 renderMode。
    std::int32_t nameValueC0 = 0;        // 原 +C0；DrawText2 仅消费低 S8。
    std::wstring nameC4; // 原 +C4..+E0；445D20允许28B及NUL，不是+64绘制文本。
    std::wstring titleE1; // 原+E1..101；克隆按值复制；51A090解析入口保留16个GBK字节限制。
    std::wstring text102; // 原成员偏移：+0x102。
    std::wstring text123; // 原成员偏移：+0x123。
    std::wstring text144; // 原+144；445350解析长度<=16个GBK字节的独立文本。
    std::array<std::byte, 25> bytes155{}; // 原+155..16D，41EB10复制原字节，业务用途未定。
    platform::LegacyCryptInt readiness; // 原 +170；地图实体将同一槽用于 HP。
    platform::LegacyCryptInt maxHp; // 原 +180；不与 +170 合并。
    platform::LegacyCryptInt value190; // 原+190；当前仅见构造／复制，未找到业务消费，保留原名；与1A0各自初始化。
    platform::LegacyCryptInt value1A0; // 原+1A0；当前仅见构造／复制，缺少用途依据，保留原名。
    platform::LegacyCryptInt barValue1B0; // 原+1B0；442F70同步玩家/宠物FP，43DB60绘第二条。
    platform::LegacyCryptInt barMaximum1C0; // 原+1C0；同步最大FP，读取仍消费计数/RNG。
    union {
        std::int32_t field1D0; // 原+1D0；共用等级、数量上限、观察序号、计数、转向符号／延迟，保持S32。
        YobiAction** windowCompletionSlot1D0 = nullptr; // 原成员偏移：+0x1D0。 475950/4758D0非拥有完成槽。
    };
    std::int32_t value1D4 = 0; // 原+1D4；共用附件位置、观察队列索引／轨迹计数和效果反向标志，不能统一为单一用途，保留原名。
    std::int32_t hoverTextColor1D8 = 0; // 原+1D8；507140发布悬停文字色，4731E0消费低BYTE，字段保留完整S32。
    std::int32_t counter1DC = 0; // 原+1DC；图标／序列计数、观察动画覆盖及部分恢复数值，共用槽而非统一计时器。
    platform::LegacyCryptInt mapX1E0; // 原+1E0/+1F0当前位置；420100族复用为序号/计时。
    platform::LegacyCryptInt mapY1F0; // 原成员偏移：+0x1F0。 46DD90仍按X→Y读取坐标并乘64。
    platform::LegacyCryptInt mapX200; // 原 +200/+210 移动目标；46DE50先赋这组，不合并载荷。
    platform::LegacyCryptInt mapY210; // 原成员偏移：+0x210。
    std::array<std::int32_t, 10> pathX220{}; // 原 +220..+247，路径X。
    std::array<std::int32_t, 10> pathY248{}; // 原 +248..+26F，路径Y。
    std::uint16_t pathCountOrDrawOrder_270 = 0; // 原 +270；路径按S16消费，地图创建只清本W16。
    std::uint16_t movementActive_272 = 0; // 原 +272；移动动作标志，不与路径计数合并。
    float worldX = 0.0f; // 原 +274/+278，地图投影输入，与 CryptInt 两组坐标独立。
    float worldY = 0.0f; // 原成员偏移：+0x278。
    std::uint32_t movementStepXBits_27C = 0; // 原成员偏移：+0x27C。
    std::uint32_t movementStepYBits_280 = 0; // 原成员偏移：+0x280。
    std::int32_t walkSpeed = 0; // 原成员偏移：+0x284。
    std::array<std::uint16_t, 4> fields288{}; // 原+288/+28A/+28C/+28E；423800相册匹配按序消费。
    YobiExtraState290 extraState290{}; // 原+290..2A3；不作为本机地址存储。
    std::int32_t framePlayingRate = 0; // 原成员偏移：+0x2A4。
    std::int32_t imageNo = 0; // 原+2A8；420100族以此槽分派阶段，图形序号另存graphicIndex。
    std::int32_t characterAction = CharacterAnimation_Stand; // 原+2B0；动画路径见CharacterAnimationId，场景特效也借作独立阶段。
    std::int32_t direction = 0; // 原成员偏移：+0x2B8。
    std::int32_t lastImageNo = 0;       // 原 +2AC：上次播放图号。
    std::int32_t lastCharacterAction = CharacterAnimation_Stand; // 原+2B4：上次动画；-1使播放缓存失效，不是动画编号。
    std::int32_t lastDirection = 0;     // 原 +2BC：上次方向。
    std::uint32_t frameIndex = 0;       // 原 +2C0：以 U32 与帧数比较。
    std::int32_t frameRemain = 0;       // 原 +2C4：非零则递减，本轮不取新帧。
    std::int32_t graphicOffsetX = 0;    // 原 +2C8：图形偏移叠加帧坐标。
    std::int32_t graphicOffsetY = 0;    // 原 +2CC。
    std::int32_t frameEvent = 0;        // 原+2D0：播放flag>=10000写入；完成族清零，帧事件族消费，41BB00作命中标记。
    std::int32_t hidden = 0;            // 原 +2D4：仅严格等于 1 时跳过播放。
    std::uint32_t field2D8 = 0; // 原+2D8；克隆／骑乘复制，410E30/4124E0清零；未证业务读取，不猜动画位。
    std::uint32_t completedLoops = 0;   // 原 +2DC。
    std::int32_t playRevert = AnimationPlayback_Loop; // 原+2E0：见AnimationPlaybackMode，保留caller完整W32。
    std::int32_t overheadMarkerMode2E4 = OverheadMarkerMode_DefaultImage; // 原+2E4；0用默认头顶标记，1／2用+2E8，模式2影响队长标记清理。
    std::int32_t overheadMarkerImage2E8 = 0; // 原+2E8；overheadMarkerMode2E4为1／2时采用的头顶标记图号。
    std::uint8_t overheadMarkerRefresh2EC = 0; // 原+2EC；图号变化置1，442F70在原条件下重建头顶标记并清零。
    std::int32_t frameX = 0;            // 原 +2F0：帧坐标叠加坐标文件偏移。
    std::int32_t frameY = 0;            // 原 +2F4。
    render::GraphicDescriptor16 drawMode{}; // 原成员偏移：+0x2F8。
    std::uint8_t batchType = 0; // 原成员偏移：+0x308。
    std::int32_t color2 = 0; // 原成员偏移：+0x30C。
    std::uint8_t rebirthValue1 = 0; // 原成员偏移：+0x309。
    std::uint8_t rebirthValue2 = 0; // 原成员偏移：+0x310。
    std::int32_t mapOffsetX = 0; // 原 +314/+318；46CF60 写 .cut 偏移，46DD90 在 FrameMove 后读取。
    std::int32_t mapOffsetY = 0; // 原成员偏移：+0x318。
    std::wstring text31C; // 原 +31C；51A090 第四文本的独立槽。

private:
    // 原41EB10：仅克隆入口可用，按原顺序赋值字段，不复制独立载荷内容。
    YobiAction& operator=(const YobiAction& source);
    friend YobiAction* cg::yobi::detail::CloneBattleAction_41EAB0(YobiAction* source);
};

static_assert(!std::is_trivially_destructible_v<YobiAction>);

namespace detail {

inline constexpr std::uint8_t kSpriteBatch = 0;

// 共享原状态，只在本组实现中定义；原全局：0x0059E56C。
extern YobiAction* gYobiActionStart;

// 共享原状态，只在本组实现中定义；原全局：0x0059E568。
extern YobiAction* gYobiActionEnd;

} // namespace detail

} // namespace cg::yobi
