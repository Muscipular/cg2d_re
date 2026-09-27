#pragma once

#include <cstdint>

namespace cg::render {

// 原 Graphic::LoadGraphic@522570：读共享索引图，发布 DX 尺寸并领取节点链。
// 节点构建依赖原 5222D0，当前仅声明该下游，不伪造构建结果。
std::int32_t LoadGraphic_522570(std::int32_t imageNo);

} // namespace cg::render
