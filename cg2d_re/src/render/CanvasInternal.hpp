#pragma once

#include "cg/render/Canvas.hpp"

namespace cg::render {

// R08：帧拥有者保留快照，GraphicQueueReference仅借用该对象。
struct CanvasBatchData {
    std::int32_t x;
    std::int32_t y;
    CanvasRectangle clipRectangle;
    std::vector<CanvasPreparedShape> shapes;
};

// 沿DrawAllQueued的现有层序消费；重复消费不释放批次。
void DrawCanvasBatch(const CanvasBatchData& batch);
// 首次新帧Canvas生产及renderer销毁释放帧快照。
void ReleaseCanvasBatches() noexcept;

} // namespace cg::render
