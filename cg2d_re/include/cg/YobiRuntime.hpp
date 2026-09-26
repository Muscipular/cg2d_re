#pragma once

#include <cstdint>

namespace cg::login {

// 原 x86 YobiAction 的原生 x64 承载；具体布局只由 Yobi runtime 持有。
// runtime/list 是唯一 owner，场景中的 YobiAction* 均为非拥有观察者。
struct YobiAction;

namespace detail {

// 原 49BE00：复用既有九宫格入队入口，不复制图形记录或绘制算法。
void DrawNineGrid_49BE00(
    std::int32_t baseImageNo,
    std::int16_t x,
    std::int16_t y,
    std::int16_t width,
    std::int16_t height,
    std::uint8_t layer,
    std::int32_t borderColor,
    std::int32_t centerColor,
    std::int16_t tileWidth,
    std::int16_t tileHeight);

// 原 dword_59E570 与 dword_59E574；后者由 UpdateAll 在 [0, 3] 间循环。
extern std::int32_t gYobiUpdateGateMode;
extern std::int32_t gYobiUpdatePhaseCounter;

// 原 0x59F9F4/0x59F9FC/0x59FA00/0x59FA04/0x553C58；
// volatile 保留 sub_423640 的独立 W32 读取及越界值、钳位值两次写入。
extern volatile std::uint32_t gYobiScreenOffsetRising;
extern volatile std::uint32_t gYobiScreenOffsetX;
extern volatile std::uint32_t gYobiScreenOffsetY;
extern volatile std::uint32_t gYobiScreenOffsetAmplitude;
extern volatile std::uint32_t gYobiScreenOffsetMaximumAmplitude;

// 原 sub_404A40@[0x404A40,0x404AD1)；初始化首尾哨兵并返回尾哨兵。
// 与原端相同，调用前提是尚未初始化，且不注册进程退出析构。
YobiAction* Sub_404A40();

// 原 YobiAction::new@[0x404AE0,0x404C42)。priority 的有效范围为 [0, 0xFE]。
YobiAction* NewYobiAction_404AE0(
    std::uint8_t priority,
    std::uint32_t payloadSize);

// 三种地图角色生产者共用的 x64 payload 分配，不改变通用 404AE0 的尺寸合同。
YobiAction* NewMapCharaAction();

// 原 5F9750 独立观察链；引用非拥有，清头不释放 action。
YobiAction* Sub_43F180(YobiAction* action);
void* Sub_43F1B0(YobiAction* action);
YobiAction* Sub_43F1F0(std::uint32_t value);
void Sub_43F220();

// 原 YobiGear::new@[0x4410F0,0x441128)：分配零载荷 action 并设置五个字段。
YobiAction* NewYobiGear_4410F0(std::int32_t imageNo);

// 原 Yobi::UpdateAll@[0x404C50,0x404CF7)。
std::int32_t YobiUpdateAll_404C50();

// 原 Yobi::MarkDeath@[0x404D00,0x404D10)，只置死亡标志并返回原指针。
YobiAction* MarkDeath(YobiAction* action);

// 原 YobiClearUp@[0x404D30,0x404D5B) 与 sub_404D60@[0x404D60,0x404D85)。
void YobiClearUp_404D30();
void Sub_404D60();

// 原 sub_448370@[0x448370,0x44839C)；查找首个 callback 匹配且 deathFlag 为零的普通节点。
// 调用前提为已初始化的有效链；允许空 callback，返回非拥有指针，未命中返回空。
YobiAction* Sub_448370(void (*callback)(YobiAction*));

// 原 49C5C0/49C5E0：dialog 尺寸的有符号整数分量计算。
std::int32_t Sub_49C5C0(std::int16_t width);
std::int32_t Sub_49C5E0(
    std::int16_t height,
    std::int32_t marginFlag,
    std::int32_t divisor);

// 原 modal creator sub_5044E0@[0x5044E0,0x5046C6)。
YobiAction* Sub_5044E0(
    std::int32_t x,
    std::int32_t y,
    std::int32_t width,
    std::int32_t height,
    std::int32_t winId,
    std::int32_t param6);

// action+0x170 的 LegacyCryptInt 窄访问；每次比较只解码一次。
bool IsYobiActionReady(YobiAction* action, std::int32_t threshold);
bool IsYobiActionStateEqual(YobiAction* action, std::int32_t expected);
bool IsYobiActionStateLessOrEqual(
    YobiAction* action,
    std::int32_t maximum);

// 当前角色预览/槽位调用所需的语义字段访问，不暴露原 x86 偏移。
std::int32_t GetYobiActionField2A8(YobiAction* action);
void SetYobiActionField2A8(YobiAction* action, std::int32_t value);
void SetYobiActionField2B0(YobiAction* action, std::int32_t value);
void SetYobiActionField15(YobiAction* action, std::uint8_t value);
std::int32_t GetYobiActionField30(YobiAction* action);
void SetYobiActionField30(YobiAction* action, std::int32_t value);
std::int32_t GetYobiActionField34(YobiAction* action);
void SetYobiActionField34(YobiAction* action, std::int32_t value);
void SetYobiActionField2B8(YobiAction* action, std::int32_t value);
void SetYobiActionField308(YobiAction* action, std::uint8_t value);
void SetYobiActionField30C(YobiAction* action, std::int32_t value);
std::uint8_t GetYobiActionField30F(YobiAction* action);
void SetYobiActionField30F(YobiAction* action, std::uint8_t value);

// 原 Yobi::FrameMove@[0x517880,0x517D9D)；当前仅保留唯一真实依赖声明。
std::int32_t Sub_517880(
    YobiAction* action,
    std::int32_t value1,
    std::int32_t value2,
    std::uint32_t value3);

} // namespace detail
} // namespace cg::login
