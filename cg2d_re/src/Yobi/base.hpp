#pragma once

#include "cg/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../ui/WindowRuntimeState.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::login {

using YobiCallback = void (*)(YobiAction* action);

// 原 x86 +offset 只作为字段来源证据；原生 x64 对象不复制 0x330 指针布局。
struct YobiAction {
    // 预览沿声明顺序默认构造十成员；普通动态节点保留原calloc零载荷。
    YobiAction() = default;
    ~YobiAction() = default;
    struct RawFields {}; // 既有零填充分配路径，不新增RNG。
    explicit YobiAction(RawFields);
    YobiAction* prev; // 原成员偏移：+0x00。
    YobiAction* next; // 原成员偏移：+0x04。
    YobiCallback callback; // 原成员偏移：+0x08。
    void* payload; // 原成员偏移：+0x0C。
    union {
        YobiAction** completionSlot10; // 原+10，444260/4441A0的非拥有完成回写槽。
        YobiAction* observationParent10; // 同一原+10，43F290/43FE00的非拥有观察父节点。
        YobiAction* battleAttachmentParent10; // 同一原+10，附件/特效/粒子发射器的非拥有父引用。
        YobiAction* battleOpponent10; // 同一原+10，416DD0/412900的非拥有对象引用；412900另快照原值。
        YobiAction* battleCastEffect10; // 同一原+10，413400保存创建返回的非拥有施法特效。
    };
    std::uint8_t priority; // 原成员偏移：+0x14。
    std::uint8_t displayPriority; // 原成员偏移：+0x15。
    std::int32_t cacheIndex; // 原成员偏移：+0x18。
    std::int32_t deathFlag; // 原成员偏移：+0x1C。
    std::int32_t graphicIndex; // 原成员偏移：+0x20。
    std::uint32_t flags; // 原成员偏移：+0x24。
    std::int32_t counter28; // 原+28，440440退出轨迹的独立计数。
    union {
        std::int32_t phase; // 原+2C，普通对象的S32阶段。
        ui::WindowValueCallback numberWindowCallback2C; // 原成员偏移：+0x2C。 48AC60数量族的非拥有函数指针。
    };
    std::int32_t x; // 原成员偏移：+0x30。
    std::int32_t y; // 原成员偏移：+0x34。
    std::int32_t deltaX; // 原+38；窗口横向步长，41F570/41FB20复用为计数。
    std::int32_t deltaY; // 原+3C；窗口/排序字段，41F570复用为寿命上限，不与y合并。
    float offset40; // 原+40；448A50/448BF0/449A00的投影后横向偏移。
    float offset44; // 原+44；对应纵向偏移，不与屏幕y合并。
    float effectAcceleration48; // 原+48，420D50/41A310写0.2，41A7D0写0.1，41A200从4C扣除；4087C0/4090F0复用为回旋转向步长。
    std::uint32_t value4CBits; // 原+4C；41F4C0/41F5B0写C1000000，41F450/41A200按F32消费。
    float angle50; // 原+50；本对象族创建清零，449160另作角度。
    float value54; // 原+54；三路对象速度及其他回调的幅值，共用F32。
    std::int32_t field58; // 原 +58，与 +2B8 的 direction 独立。
    float scaleX; // 原成员偏移：+0x5C。
    float scaleY; // 原成员偏移：+0x60。
    std::array<char, 64> displayName; // 原 +64..+A3；41EB10 按 64B 复制，保留原编码。
    std::int32_t nameX;              // 原 +A4；DrawText2消费低S16，41A7D0作S32生成计数。
    std::int32_t nameY;              // 原+A8；下落族作终点，41B440帧事件族作S32消退计数。
    float nameOffsetAC; // 原+AC，伤害数字创建/更新投影到nameX。
    float nameOffsetB0; // 原+B0，伤害数字创建/更新投影到nameY。
    float battleTurnStepB4; // 原+B4；41C3B0/4219D0转向步长，40F800复用为F32目标缩放。
    float numberVelocityB8; // 原+B8，41F1A0写-8，41F110更新独立数字速度。
    std::int8_t nameColor;           // 原 +BC。
    std::int8_t nameFontType;        // 原 +BD。
    std::int8_t nameLayer;           // 原 +BE；DrawText2 的 renderMode。
    std::int32_t nameValueC0;        // 原 +C0；DrawText2 仅消费低 S8。
    std::array<char, 29> nameC4; // 原 +C4..+E0；445D20允许28B及NUL，不是+64绘制文本。
    std::array<char, 33> titleE1; // 原+E1..101；41EB10复制33B，51A090仍最多写16B及NUL。
    std::array<char, 33> text102; // 原成员偏移：+0x102。
    std::array<char, 33> text123; // 原成员偏移：+0x123。
    std::array<char, 17> text144; // 原+144；445350复制长度<=16的独立原字节文本。
    std::array<std::byte, 25> bytes155; // 原+155..16D，41EB10复制原字节，业务用途未定。
    platform::LegacyCryptInt readiness; // 原 +170；地图实体将同一槽用于 HP。
    platform::LegacyCryptInt maxHp; // 原 +180；不与 +170 合并。
    platform::LegacyCryptInt value190; // 原+190/+1A0，选择对象静态构造各自初始化。
    platform::LegacyCryptInt value1A0; // 原成员偏移：+0x1A0。
    platform::LegacyCryptInt barValue1B0; // 原+1B0；442F70同步玩家/宠物FP，43DB60绘第二条。
    platform::LegacyCryptInt barMaximum1C0; // 原+1C0；同步最大FP，读取仍消费计数/RNG。
    union {
        std::int32_t field1D0; // 原+1D0数值路径，宽度保持S32。
        YobiAction** windowCompletionSlot1D0; // 原成员偏移：+0x1D0。 475950/4758D0非拥有完成槽。
    };
    std::int32_t value1D4; // 原+1D4；观察父节点主队列索引/子节点轨迹计数，共用S32。
    std::int32_t field1D8; // 原 +1D8；51A090 写入完整第四参数。
    std::int32_t counter1DC; // 原+1DC，442F70图标/420300序列特效计数。
    platform::LegacyCryptInt mapX1E0; // 原+1E0/+1F0当前位置；420100族复用为序号/计时。
    platform::LegacyCryptInt mapY1F0; // 原成员偏移：+0x1F0。 46DD90仍按X→Y读取坐标并乘64。
    platform::LegacyCryptInt mapX200; // 原 +200/+210 移动目标；46DE50先赋这组，不合并载荷。
    platform::LegacyCryptInt mapY210; // 原成员偏移：+0x210。
    std::array<std::int32_t, 10> pathX220; // 原 +220..+247，路径X。
    std::array<std::int32_t, 10> pathY248; // 原 +248..+26F，路径Y。
    std::uint16_t field270; // 原 +270；路径按S16消费，地图创建只清本W16。
    std::uint16_t field272; // 原 +272；移动动作标志，不与路径计数合并。
    float worldX; // 原 +274/+278，地图投影输入，与 CryptInt 两组坐标独立。
    float worldY; // 原成员偏移：+0x278。
    std::uint32_t field27C; // 原成员偏移：+0x27C。
    std::uint32_t field280; // 原成员偏移：+0x280。
    std::int32_t walkSpeed; // 原成员偏移：+0x284。
    std::array<std::uint16_t, 4> fields288; // 原+288/+28A/+28C/+28E；423800相册匹配按序消费。
    std::array<std::byte, 20> bytes290; // 原+290..2A3的五W32复制范围；不作为本机地址存储。
    std::int32_t framePlayingRate; // 原成员偏移：+0x2A4。
    std::int32_t imageNo; // 原+2A8；420100族以此槽分派阶段，图形序号另存graphicIndex。
    std::int32_t characterAction; // 原成员偏移：+0x2B0。
    std::int32_t direction; // 原成员偏移：+0x2B8。
    std::int32_t lastImageNo;       // 原 +2AC：上次播放图号。
    std::int32_t lastCharacterAction; // 原 +2B4：上次动作。
    std::int32_t lastDirection;     // 原 +2BC：上次方向。
    std::uint32_t frameIndex;       // 原 +2C0：以 U32 与帧数比较。
    std::int32_t frameRemain;       // 原 +2C4：非零则递减，本轮不取新帧。
    std::int32_t graphicOffsetX;    // 原 +2C8：图形偏移叠加帧坐标。
    std::int32_t graphicOffsetY;    // 原 +2CC。
    std::int32_t frameEvent;        // 原+2D0：播放flag>=10000写入；完成族清零，帧事件族消费，41BB00作命中标记。
    std::int32_t hidden;            // 原 +2D4：仅严格等于 1 时跳过播放。
    std::uint32_t field2D8;         // 原+2D8；41EB10复制，410E30/4124E0另清零，独立位型。
    std::uint32_t completedLoops;   // 原 +2DC。
    std::int32_t playRevert;        // 原 +2E0：保留 caller 完整 W32。
    std::int32_t field2E4;          // 原 +2E4；51A5B0 比较完整 W32 是否为2。
    std::int32_t imageNo2E8;        // 原+2E8，442F70第一附件图号。
    std::uint8_t refresh2EC;        // 原+2EC，创建第一附件时清零。
    std::int32_t frameX;            // 原 +2F0：帧坐标叠加坐标文件偏移。
    std::int32_t frameY;            // 原 +2F4。
    render::GraphicDescriptor16 drawMode; // 原成员偏移：+0x2F8。
    std::uint8_t batchType; // 原成员偏移：+0x308。
    std::int32_t color2; // 原成员偏移：+0x30C。
    std::uint8_t rebirthValue1; // 原成员偏移：+0x309。
    std::uint8_t rebirthValue2; // 原成员偏移：+0x310。
    std::int32_t mapOffsetX; // 原 +314/+318；46CF60 写 .cut 偏移，46DD90 在 FrameMove 后读取。
    std::int32_t mapOffsetY; // 原成员偏移：+0x318。
    std::array<char, 17> text31C; // 原 +31C；51A090 第四文本的独立槽。
};

static_assert(std::is_standard_layout_v<YobiAction>);

static_assert(!std::is_trivially_destructible_v<YobiAction>);

namespace detail {

inline constexpr std::uint8_t kSpriteBatch = 0;

// 共享原状态，只在本组实现中定义；原全局：0x0059E56C。
extern YobiAction* gYobiActionStart;

// 共享原状态，只在本组实现中定义；原全局：0x0059E568。
extern YobiAction* gYobiActionEnd;

// 按原S8位型读取低字节，供窗口绘制参数转换。
std::int32_t LowSignedByte(const std::int16_t value);

} // namespace detail

} // namespace cg::login
