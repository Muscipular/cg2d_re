#pragma once

namespace cg::render {

// RectF 表示以浮点数描述的位置与尺寸。
struct RectF {
    float x = 0.0F;
    float y = 0.0F;
    float width = 0.0F;
    float height = 0.0F;
};

// LogicalPresentationMode 控制逻辑分辨率如何映射到输出窗口。
enum class LogicalPresentationMode {
    Disabled,
    Stretch,
    Letterbox,
    Overscan,
    IntegerScale,
};

struct LogicalPresentation {
    int width = 0;
    int height = 0;
    LogicalPresentationMode mode = LogicalPresentationMode::Disabled;
};

} // namespace cg::render
