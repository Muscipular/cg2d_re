#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace cg::render {

// R08：参考Canvas图元类型；位置相对于DrawCanvasClipped传入的画布原点。
enum class CanvasShapeType : std::uint8_t {
    Line,
    Rect,
    RectBorder,
    Circle,
    CircleBorder,
    Path,
    PathBorder,
};

struct CanvasPoint {
    float x = 0.0f;
    float y = 0.0f;
};

// 已准备的绘制数据；入队时复制，调用者后续修改不影响该批次。
struct CanvasPreparedShape {
    CanvasShapeType type = CanvasShapeType::Line;
    std::uint32_t color = 0xFFFFFFFFu; // 0xAARRGGBB。
    std::int32_t thickness = 1; // 线与描边宽度。
    std::int32_t segments = 16; // 圆／曲线分段数。
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    float x1 = 0.0f; // Line起点。
    float y1 = 0.0f;
    float x2 = 0.0f; // Line终点。
    float y2 = 0.0f;
    float radius = 0.0f;
    bool closed = false; // PathBorder是否闭合。
    std::vector<CanvasPoint> points; // 路径轮廓，供描边使用。
    std::vector<CanvasPoint> triangles; // Path三角形列表，每三个点一组。
};

struct CanvasShape : CanvasPreparedShape {
    std::wstring path; // M/L/H/V/Q/C/Z及小写相对命令，按参考规则解析。
};

// 路径修改后调用一次：重建points／triangles／closed，解析失败清空这些准备数据。
// 程序直接提供points／triangles时不调用此接口，可直接交给DrawCanvasClipped。
void PrepareCanvasShape(CanvasShape& shape);

// 参考UIClipRect的S16四边，顺序left/top/right/bottom；不暴露SDK类型。
using CanvasRectangle = std::array<std::int16_t, 4>;

// 窗口坐标入队；与画布边界求交，空交集或原容量限制失败返回-1，队列失败透传。
std::int32_t DrawCanvasClipped(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::uint8_t layer, std::span<const CanvasShape> shapes,
    const CanvasRectangle* clipRectangle = nullptr);

} // namespace cg::render
