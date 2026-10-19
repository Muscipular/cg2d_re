#pragma once

#include "base.hpp"
#include "cg/resource/AnimationRuntime.hpp"

// 本组内部共享声明；跨模块接口见 cg/Yobi/YobiRuntime.hpp。

namespace cg::yobi::detail {
// 原518300：先确定kind的F32系数再查询，按低BYTE轴向零转换/窄化；完整合同沿YGOS01。
std::int32_t GetScaledYobiGraphicOriginOffset_518300(YobiAction* action, std::uint8_t axis, std::int32_t kind);

// 原 Yobi::PlayFrame@[517DA0,5182A1)：使用对象播放速率；静图仅小于 100000；两入口共享帧消费沿YFPS01 SPEC。
// 返回1含动画上界/索引或动作缺失及停播终点；装载失败为-1，其余为0；资源实际播放另验。
std::int32_t PlayFrame_517DA0(
    YobiAction* action, std::int32_t stopAtEnd, const YobiAction* syncSource = nullptr);

// AF406：读取主挂件已发布的动作属性；战斗人物Coordinate不翻转，地图及骑乘按横向翻转。
std::uint16_t GetYobiAnimeActionFlags(const YobiAction* action) noexcept;
void GetYobiAttachmentCoordinate(
    const YobiAction* action, std::int32_t& x, std::int32_t& y, bool applyFlip = true) noexcept;

} // namespace cg::yobi::detail
