#pragma once

#include <cstdint>

namespace cg::render {

// 原 Graphic::LoadGraphic@522570：读共享索引图，发布 DX 尺寸并领取节点链。
// 5222D0 按用户裁决仅实现恒空第二参数路径；部分建链也不改写本函数的成功返回。
std::int32_t LoadGraphic_522570(std::int32_t imageNo);

} // namespace cg::render
