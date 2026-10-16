#include "cg/render/Canvas.hpp"

#include "CanvasInternal.hpp"
#include "LegacyTextureDrawing.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <utility>

namespace cg::render {

// R08：保留参考Canvas批次容量和图元分段规则。
static constexpr std::size_t CANVAS_MAX_SHAPES = 1024;
static constexpr std::size_t CANVAS_MAX_POINTS = 32768;
static constexpr std::size_t CANVAS_MAX_BATCH_VERTICES = 65535;
static constexpr std::size_t CANVAS_MAX_DRAW_VERTICES = 6144;
static constexpr float CANVAS_PI = 3.14159265358979323846f;

// 每个队列槽拥有独立快照；清理只释放形状，重建后旧队列借用仍指向稳定槽。
static std::array<CanvasBatchData, MAX_QUEUE_SIZE> gCanvasBatches;
static std::uint32_t gCanvasBatchFrame = 0;

static constexpr float CANVAS_EPSILON = 0.0001f;

// 参考曲线采样段数，圆的最少三段在顶点构造处处理。
static std::int32_t ClampCanvasSegments(std::int32_t value)
{
    return std::clamp(value, 1, 128);
}

// 路径去重与闭合共用参考浮点阈值。
static bool SamePoint(const CanvasPoint& a, const CanvasPoint& b)
{
    return std::fabs(a.x - b.x) < CANVAS_EPSILON && std::fabs(a.y - b.y) < CANVAS_EPSILON;
}

// 三点叉积，供耳切凸角与点包含判断使用。
static float Cross(const CanvasPoint& a, const CanvasPoint& b, const CanvasPoint& c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// 有向多边形面积确定路径绕序。
static float PolygonArea(const std::vector<CanvasPoint>& points)
{
    float area = 0.0f;
    const std::size_t count = points.size();
    for (std::size_t i = 0; i < count; ++i) {
        const auto& a = points[i];
        const auto& b = points[(i + 1) % count];
        area += a.x * b.y - b.x * a.y;
    }
    return area * 0.5f;
}

// 耳切时检查其他轮廓点是否落入候选三角形。
static bool PointInTriangle(const CanvasPoint& p, const CanvasPoint& a, const CanvasPoint& b, const CanvasPoint& c)
{
    float c1 = Cross(a, b, p);
    float c2 = Cross(b, c, p);
    float c3 = Cross(c, a, p);
    bool hasNegative = c1 < -CANVAS_EPSILON || c2 < -CANVAS_EPSILON || c3 < -CANVAS_EPSILON;
    bool hasPositive = c1 > CANVAS_EPSILON || c2 > CANVAS_EPSILON || c3 > CANVAS_EPSILON;
    return !(hasNegative && hasPositive);
}

// 去相邻重合点后按参考耳切顺序准备实心路径；无可切耳时清三角形。
static void BuildPathTriangles(CanvasShape& shape)
{
    shape.triangles.clear();
    std::vector<CanvasPoint> points;
    for (const auto& point : shape.points) {
        if (points.empty() || !SamePoint(points.back(), point)) {
            points.push_back(point);
        }
    }
    if (points.size() > 1 && SamePoint(points.front(), points.back())) {
        points.pop_back();
    }
    if (points.size() < 3) {
        return;
    }

    float area = PolygonArea(points);
    if (std::fabs(area) < CANVAS_EPSILON) {
        return;
    }
    bool ccw = area > 0.0f;

    std::vector<std::size_t> indices(points.size());
    for (std::size_t i = 0; i < points.size(); ++i) {
        indices[i] = i;
    }

    std::size_t guard = 0;
    while (indices.size() > 3 && guard < points.size() * points.size()) {
        bool found = false;
        for (std::size_t i = 0; i < indices.size(); ++i) {
            std::size_t prevIndex = indices[(i + indices.size() - 1) % indices.size()];
            std::size_t currIndex = indices[i];
            std::size_t nextIndex = indices[(i + 1) % indices.size()];
            const auto& a = points[prevIndex];
            const auto& b = points[currIndex];
            const auto& c = points[nextIndex];
            float cross = Cross(a, b, c);
            if ((ccw && cross <= CANVAS_EPSILON) || (!ccw && cross >= -CANVAS_EPSILON)) {
                continue;
            }

            bool contains = false;
            for (std::size_t testIndex : indices) {
                if (testIndex == prevIndex || testIndex == currIndex || testIndex == nextIndex) {
                    continue;
                }
                if (PointInTriangle(points[testIndex], a, b, c)) {
                    contains = true;
                    break;
                }
            }
            if (contains) {
                continue;
            }

            if (ccw) {
                shape.triangles.push_back(a);
                shape.triangles.push_back(b);
                shape.triangles.push_back(c);
            } else {
                shape.triangles.push_back(a);
                shape.triangles.push_back(c);
                shape.triangles.push_back(b);
            }
            indices.erase(indices.begin() + static_cast<std::ptrdiff_t>(i));
            found = true;
            break;
        }
        if (!found) {
            shape.triangles.clear();
            return;
        }
        ++guard;
    }

    if (indices.size() == 3) {
        const auto& a = points[indices[0]];
        const auto& b = points[indices[1]];
        const auto& c = points[indices[2]];
        if (ccw) {
            shape.triangles.push_back(a);
            shape.triangles.push_back(b);
            shape.triangles.push_back(c);
        } else {
            shape.triangles.push_back(a);
            shape.triangles.push_back(c);
            shape.triangles.push_back(b);
        }
    }
}

// 参考路径解析器，直接写入调用者准备阶段的轮廓与闭合状态。
class CanvasPathParser {
public:
    CanvasPathParser(const std::string& path, std::int32_t segments, std::vector<CanvasPoint>& points, bool& closed)
        : current(path.c_str()), segments(ClampCanvasSegments(segments)), points(points), closed(closed)
        {
    }

    bool Parse()
    {
        CanvasPoint currentPoint{};
        CanvasPoint startPoint{};
        char command = 0;
        bool hasStart = false;
        while (true) {
            SkipSeparators();
            if (*current == '\0') {
                return true;
            }
            if (std::isalpha(static_cast<unsigned char>(*current))) {
                command = *current++;
            } else if (command == 0)
            {
                return false;
            }

            bool relative = std::islower(static_cast<unsigned char>(command)) != 0;
            switch (std::toupper(static_cast<unsigned char>(command))) {
                case 'M':
                    if (!ParseMove(relative, currentPoint, startPoint, hasStart, command)) {
                        return false;
                    }
                    break;
                case 'L':
                    if (!ParseLine(relative, currentPoint)) {
                        return false;
                    }
                    break;
                case 'H':
                    if (!ParseHorizontal(relative, currentPoint)) {
                        return false;
                    }
                    break;
                case 'V':
                    if (!ParseVertical(relative, currentPoint)) {
                        return false;
                    }
                    break;
                case 'Q':
                    if (!ParseQuad(relative, currentPoint)) {
                        return false;
                    }
                    break;
                case 'C':
                    if (!ParseCubic(relative, currentPoint)) {
                        return false;
                    }
                    break;
                case 'Z':
                    if (hasStart) {
                        if (points.empty() || !SamePoint(points.back(), startPoint)) {
                            points.push_back(startPoint);
                        }
                        currentPoint = startPoint;
                        closed = true;
                    }
                    command = 0;
                    break;
                default:
                    return false;
            }
        }
    }

private:
    const char* current;
    std::int32_t segments;
    std::vector<CanvasPoint>& points;
    bool& closed;

    void SkipSeparators()
    {
        while (*current == ' ' || *current == '\t' || *current == '\r' || *current == '\n' || *current == ',') {
            ++current;
        }
    }

    bool HasNumber()
    {
        SkipSeparators();
        return *current == '-' || *current == '+' || *current == '.' || std::isdigit(static_cast<unsigned char>(*current));
    }

    bool ParseNumber(float& value)
    {
        SkipSeparators();
        char* end = nullptr;
        value = std::strtof(current, &end);
        if (end == current) {
            return false;
        }
        current = end;
        return true;
    }

    bool ParsePoint(bool relative, CanvasPoint base, CanvasPoint& point)
    {
        if (!ParseNumber(point.x) || !ParseNumber(point.y)) {
            return false;
        }
        if (relative) {
            point.x += base.x;
            point.y += base.y;
        }
        return true;
    }

    bool ParseMove(bool relative, CanvasPoint& currentPoint, CanvasPoint& startPoint, bool& hasStart, char& command)
    {
        CanvasPoint point{};
        if (!ParsePoint(relative, currentPoint, point)) {
            return false;
        }
        currentPoint = point;
        startPoint = point;
        hasStart = true;
        points.push_back(point);
        command = relative ? 'l' : 'L';
        while (HasNumber()) {
            if (!ParsePoint(relative, currentPoint, point)) {
                return false;
            }
            currentPoint = point;
            points.push_back(point);
        }
        return true;
    }

    bool ParseLine(bool relative, CanvasPoint& currentPoint)
    {
        CanvasPoint point{};
        if (!HasNumber()) {
            return false;
        }
        while (HasNumber()) {
            if (!ParsePoint(relative, currentPoint, point)) {
                return false;
            }
            currentPoint = point;
            points.push_back(point);
        }
        return true;
    }

    bool ParseHorizontal(bool relative, CanvasPoint& currentPoint)
    {
        float x = 0.0f;
        if (!HasNumber()) {
            return false;
        }
        while (HasNumber()) {
            if (!ParseNumber(x)) {
                return false;
            }
            currentPoint.x = relative ? currentPoint.x + x : x;
            points.push_back(currentPoint);
        }
        return true;
    }

    bool ParseVertical(bool relative, CanvasPoint& currentPoint)
    {
        float y = 0.0f;
        if (!HasNumber()) {
            return false;
        }
        while (HasNumber()) {
            if (!ParseNumber(y)) {
                return false;
            }
            currentPoint.y = relative ? currentPoint.y + y : y;
            points.push_back(currentPoint);
        }
        return true;
    }

    bool ParseQuad(bool relative, CanvasPoint& currentPoint)
    {
        if (!HasNumber()) {
            return false;
        }
        while (HasNumber()) {
            CanvasPoint c{};
            CanvasPoint end{};
            CanvasPoint start = currentPoint;
            if (!ParsePoint(relative, currentPoint, c) || !ParsePoint(relative, currentPoint, end)) {
                return false;
            }
            for (std::int32_t i = 1; i <= segments; ++i) {
                float t = static_cast<float>(i) / static_cast<float>(segments);
                float inv = 1.0f - t;
                points.push_back({
                    inv * inv * start.x + 2.0f * inv * t * c.x + t * t * end.x,
                    inv * inv * start.y + 2.0f * inv * t * c.y + t * t * end.y
                });
            }
            currentPoint = end;
        }
        return true;
    }

    bool ParseCubic(bool relative, CanvasPoint& currentPoint)
    {
        if (!HasNumber()) {
            return false;
        }
        while (HasNumber()) {
            CanvasPoint c1{};
            CanvasPoint c2{};
            CanvasPoint end{};
            CanvasPoint start = currentPoint;
            if (!ParsePoint(relative, currentPoint, c1) || !ParsePoint(relative, currentPoint, c2) || !ParsePoint(relative, currentPoint, end)) {
                return false;
            }
            for (std::int32_t i = 1; i <= segments; ++i) {
                float t = static_cast<float>(i) / static_cast<float>(segments);
                float inv = 1.0f - t;
                points.push_back({
                    inv * inv * inv * start.x + 3.0f * inv * inv * t * c1.x + 3.0f * inv * t * t * c2.x + t * t * t * end.x,
                    inv * inv * inv * start.y + 3.0f * inv * inv * t * c1.y + 3.0f * inv * t * t * c2.y + t * t * t * end.y
                });
            }
            currentPoint = end;
        }
        return true;
    }
};

void PrepareCanvasShape(CanvasShape& shape)
{
    shape.segments = ClampCanvasSegments(shape.segments);
    shape.thickness = std::max(1, shape.thickness);
    shape.points.clear();
    shape.triangles.clear();
    shape.closed = false;
    if (shape.type != CanvasShapeType::Path && shape.type != CanvasShapeType::PathBorder) {
        return;
    }

    CanvasPathParser parser(shape.path, shape.segments, shape.points, shape.closed);
    if (!parser.Parse()) {
        shape.points.clear();
        shape.triangles.clear();
        shape.closed = false;
        return;
    }
    if (shape.type == CanvasShapeType::Path) {
        BuildPathTriangles(shape);
    }
}

// 转成纯色后端顶点；保留F32位置与完整ARGB。
static ColorVertex MakeCanvasVertex(float x, float y, std::uint32_t color)
{
    return {x, y, color};
}

// 参考图元只在Alpha非零时产生顶点。
static bool ColorVisible(std::uint32_t color)
{
    return (color & 0xFF000000u) != 0;
}

// 保持参考顶点序，一个四边形拆成共享对角线的两个三角形。
static void AppendQuadVertices(std::vector<ColorVertex>& vertices, ColorVertex a, ColorVertex b, ColorVertex c, ColorVertex d)
{
    vertices.push_back(a);
    vertices.push_back(b);
    vertices.push_back(c);
    vertices.push_back(a);
    vertices.push_back(c);
    vertices.push_back(d);
}

// 实心矩形，非正尺寸按参考不产生顶点。
static void AppendQuad(std::vector<ColorVertex>& vertices, float x, float y, float w, float h, std::uint32_t color)
{
    if (w <= 0.0f || h <= 0.0f || !ColorVisible(color)) {
        return;
    }
    AppendQuadVertices(vertices,
        MakeCanvasVertex(x, y, color),
        MakeCanvasVertex(x + w, y, color),
        MakeCanvasVertex(x + w, y + h, color),
        MakeCanvasVertex(x, y + h, color));
}

// 线段按法向量展开为四边形，零长度线段绘成线宽大小的方块。
static void AppendLine(std::vector<ColorVertex>& vertices, float x1, float y1, float x2, float y2, std::int32_t thickness, std::uint32_t color)
{
    if (!ColorVisible(color)) {
        return;
    }
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx * dx + dy * dy);
    float half = std::max(1.0f, static_cast<float>(thickness)) * 0.5f;
    if (len < 0.001f) {
        AppendQuad(vertices, x1 - half, y1 - half, half * 2.0f, half * 2.0f, color);
        return;
    }
    float nx = -dy / len * half;
    float ny = dx / len * half;
    AppendQuadVertices(vertices,
        MakeCanvasVertex(x1 + nx, y1 + ny, color),
        MakeCanvasVertex(x2 + nx, y2 + ny, color),
        MakeCanvasVertex(x2 - nx, y2 - ny, color),
        MakeCanvasVertex(x1 - nx, y1 - ny, color));
}

// 四条矩形边顺序叠加，角部重叠的Alpha保持。
static void AppendRectBorder(std::vector<ColorVertex>& vertices, float x, float y, float w, float h, std::int32_t thickness, std::uint32_t color)
{
    float t = std::max(1.0f, static_cast<float>(thickness));
    AppendQuad(vertices, x, y, w, t, color);
    AppendQuad(vertices, x, y + h - t, w, t, color);
    AppendQuad(vertices, x, y, t, h, color);
    AppendQuad(vertices, x + w - t, y, t, h, color);
}

// 圆心扇形展开实心圆。
static void AppendCircle(std::vector<ColorVertex>& vertices, float x, float y, float radius, std::int32_t segments, std::uint32_t color)
{
    if (radius <= 0.0f || !ColorVisible(color)) {
        return;
    }
    std::int32_t count = std::clamp(segments, 3, 128);
    ColorVertex center = MakeCanvasVertex(x, y, color);
    for (std::int32_t i = 0; i < count; ++i) {
        float angleA = CANVAS_PI * 2.0f * static_cast<float>(i) / static_cast<float>(count);
        float angleB = CANVAS_PI * 2.0f * static_cast<float>(i + 1) / static_cast<float>(count);
        vertices.push_back(center);
        vertices.push_back(MakeCanvasVertex(x + std::cos(angleA) * radius, y + std::sin(angleA) * radius, color));
        vertices.push_back(MakeCanvasVertex(x + std::cos(angleB) * radius, y + std::sin(angleB) * radius, color));
    }
}

// 圆周分段沿用线段描边。
static void AppendCircleBorder(std::vector<ColorVertex>& vertices, float x, float y, float radius, std::int32_t thickness, std::int32_t segments, std::uint32_t color)
{
    if (radius <= 0.0f || !ColorVisible(color)) {
        return;
    }
    std::int32_t count = std::clamp(segments, 3, 128);
    CanvasPoint last{x + radius, y};
    for (std::int32_t i = 1; i <= count; ++i) {
        float angle = CANVAS_PI * 2.0f * static_cast<float>(i) / static_cast<float>(count);
        CanvasPoint next{x + std::cos(angle) * radius, y + std::sin(angle) * radius};
        AppendLine(vertices, last.x, last.y, next.x, next.y, thickness, color);
        last = next;
    }
}

// 使用准备阶段生成的三角形，叠加画布原点。
static void AppendPath(std::vector<ColorVertex>& vertices, const CanvasPreparedShape& shape, float ox, float oy)
{
    if (shape.triangles.empty() || !ColorVisible(shape.color)) {
        return;
    }
    for (const auto& point : shape.triangles) {
        vertices.push_back(MakeCanvasVertex(ox + point.x, oy + point.y, shape.color));
    }
}

// 轮廓折线与可选闭合边使用同一线宽。
static void AppendPathBorder(std::vector<ColorVertex>& vertices, const CanvasPreparedShape& shape, float ox, float oy)
{
    if (shape.points.size() < 2 || !ColorVisible(shape.color)) {
        return;
    }
    for (std::size_t i = 1; i < shape.points.size(); ++i) {
        AppendLine(vertices, ox + shape.points[i - 1].x, oy + shape.points[i - 1].y, ox + shape.points[i].x, oy + shape.points[i].y, shape.thickness, shape.color);
    }
    if (shape.closed && !shape.points.empty() && !SamePoint(shape.points.front(), shape.points.back())) {
        const auto& first = shape.points.front();
        const auto& last = shape.points.back();
        AppendLine(vertices, ox + last.x, oy + last.y, ox + first.x, oy + first.y, shape.thickness, shape.color);
    }
}

// 七种图元按原形状顺序生成三角形。
static void AppendCanvasShape(std::vector<ColorVertex>& vertices, const CanvasPreparedShape& shape, float ox, float oy)
{
    switch (shape.type) {
        case CanvasShapeType::Line:
            AppendLine(vertices, ox + shape.x1, oy + shape.y1, ox + shape.x2, oy + shape.y2, shape.thickness, shape.color);
            break;
        case CanvasShapeType::Rect:
            AppendQuad(vertices, ox + shape.x, oy + shape.y, shape.w, shape.h, shape.color);
            break;
        case CanvasShapeType::RectBorder:
            AppendRectBorder(vertices, ox + shape.x, oy + shape.y, shape.w, shape.h, shape.thickness, shape.color);
            break;
        case CanvasShapeType::Circle:
            AppendCircle(vertices, ox + shape.x, oy + shape.y, shape.radius, shape.segments, shape.color);
            break;
        case CanvasShapeType::CircleBorder:
            AppendCircleBorder(vertices, ox + shape.x, oy + shape.y, shape.radius, shape.thickness, shape.segments, shape.color);
            break;
        case CanvasShapeType::Path:
            AppendPath(vertices, shape, ox, oy);
            break;
        case CanvasShapeType::PathBorder:
            AppendPathBorder(vertices, shape, ox, oy);
            break;
    }
}

// 与描边实际闭合条件一致，计入末尾到首点的边。
static std::size_t EstimatePathBorderVertices(const CanvasShape& shape)
{
    if (shape.points.size() < 2) {
        return 0;
    }
    std::size_t lineCount = shape.points.size() - 1;
    if (shape.closed && !shape.points.empty() && !SamePoint(shape.points.front(), shape.points.back())) {
        ++lineCount;
    }
    return lineCount * 6;
}

// 顶点预算与七类图元实际展开数量对应。
static std::size_t EstimateCanvasShapeVertices(const CanvasShape& shape)
{
    switch (shape.type) {
        case CanvasShapeType::Line:
            return 6;
        case CanvasShapeType::Rect:
            return 6;
        case CanvasShapeType::RectBorder:
            return 24;
        case CanvasShapeType::Circle:
            return static_cast<std::size_t>(std::clamp(shape.segments, 3, 128)) * 3;
        case CanvasShapeType::CircleBorder:
            return static_cast<std::size_t>(std::clamp(shape.segments, 3, 128)) * 6;
        case CanvasShapeType::Path:
            return shape.triangles.size();
        case CanvasShapeType::PathBorder:
            return EstimatePathBorderVertices(shape);
    }
    return 0;
}

// 累加参考点数／顶点上限，包含解析后的路径轮廓和三角形。
static bool CountCanvasShapeLimits(const CanvasShape& shape, std::size_t& totalPoints, std::size_t& totalVertices)
{
    totalPoints += shape.points.size();
    totalPoints += shape.triangles.size();
    if (shape.type == CanvasShapeType::Circle || shape.type == CanvasShapeType::CircleBorder) {
        totalPoints += static_cast<std::size_t>(std::clamp(shape.segments, 3, 128));
    }
    totalVertices += EstimateCanvasShapeVertices(shape);
    return totalPoints <= CANVAS_MAX_POINTS && totalVertices <= CANVAS_MAX_BATCH_VERTICES;
}

std::int32_t DrawCanvasClipped(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::uint8_t layer, const std::span<const CanvasShape> shapes,
    const CanvasRectangle* const clipRectangle)
{
    if (width <= 0 || height <= 0 || shapes.empty() || shapes.size() > CANVAS_MAX_SHAPES) {
        return -1;
    }
    CanvasRectangle drawClip {
        static_cast<std::int16_t>(x), static_cast<std::int16_t>(y),
        static_cast<std::int16_t>(x + width), static_cast<std::int16_t>(y + height)
    };
    if (clipRectangle != nullptr) {
        drawClip[0] = std::max(drawClip[0], (*clipRectangle)[0]);
        drawClip[1] = std::max(drawClip[1], (*clipRectangle)[1]);
        drawClip[2] = std::min(drawClip[2], (*clipRectangle)[2]);
        drawClip[3] = std::min(drawClip[3], (*clipRectangle)[3]);
    }
    if (drawClip[2] <= drawClip[0] || drawClip[3] <= drawClip[1]) {
        return -1;
    }
    std::size_t totalPoints = 0;
    std::size_t totalVertices = 0;
    for (const auto& shape : shapes) {
        if (!CountCanvasShapeLimits(shape, totalPoints, totalVertices)) {
            return -1;
        }
    }
    if (gCanvasBatchFrame != platform::gFrameCount) {
        ReleaseCanvasBatches();
        gCanvasBatchFrame = platform::gFrameCount;
    }
    const std::int32_t index = DrawGraphic(0, 0, layer, -2, 0, nullptr);
    if (index < 0) {
        return index;
    }
    CanvasBatchData& batch = gCanvasBatches[index];
    batch.x = x;
    batch.y = y;
    batch.clipRectangle = drawClip;
    batch.shapes.clear();
    batch.shapes.reserve(shapes.size());
    for (const auto& shape : shapes) {
        batch.shapes.push_back(static_cast<const CanvasPreparedShape&>(shape));
    }
    auto& record = gLegacyPendingStorage.records[index];
    record.reference.canvas = &batch;
    record.batchType = GraphicBatch_Canvas;
    return index;
}

void DrawCanvasBatch(const CanvasBatchData& batch)
{
    std::vector<ColorVertex> vertices;
    vertices.reserve(std::min(CANVAS_MAX_BATCH_VERTICES, batch.shapes.size() * 24));
    for (const auto& shape : batch.shapes) {
        AppendCanvasShape(vertices, shape, static_cast<float>(batch.x), static_cast<float>(batch.y));
        if (vertices.size() > CANVAS_MAX_BATCH_VERTICES) {
            vertices.clear();
            break;
        }
    }
    const TextureRectangle clip {
        batch.clipRectangle[0], batch.clipRectangle[1],
        batch.clipRectangle[2], batch.clipRectangle[3]
    };
    std::size_t offset = 0;
    while (offset + 2 < vertices.size()) {
        std::size_t count = std::min(CANVAS_MAX_DRAW_VERTICES, vertices.size() - offset);
        count -= count % 3;
        static_cast<void>(RenderColorTriangles(
            std::span<const ColorVertex>(vertices).subspan(offset, count), clip));
        offset += count;
    }
}

void ReleaseCanvasBatches() noexcept
{
    for (auto& batch : gCanvasBatches) {
        std::vector<CanvasPreparedShape>().swap(batch.shapes);
    }
}

} // namespace cg::render
