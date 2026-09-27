#pragma once

#include <cstdint>

namespace cg::render {

// 原 Graphic::LoadGraphic@522570：读共享索引图，发布 DX 尺寸并领取节点链。
// 5222D0 按用户裁决仅实现恒空第二参数路径；部分建链也不改写本函数的成功返回。
std::int32_t LoadGraphic_522570(std::int32_t imageNo);

// 原播放/绘制消费者按阶段读取 DX 宽、高的低 S16；不检查图号，不触发加载。
std::int16_t ReadGraphicDxWidth(std::int32_t imageNo) noexcept;
std::int16_t ReadGraphicDxHeight(std::int32_t imageNo) noexcept;

// 原 44D490：仅清当前场景对应的变换树，按完整地址键次序释放面、清槽并摘项。
// 原两个 caller 均不消费残留 EAX，不向上层暴露树哨兵地址。
void ClearSurfaceTransformCache_44D490();

} // namespace cg::render
