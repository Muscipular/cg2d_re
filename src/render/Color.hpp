#pragma once

namespace cg::render {

// ColorF 表示归一化 RGBA 颜色，取值通常为 0.0F 到 1.0F。
struct ColorF {
    float r = 0.0F;
    float g = 0.0F;
    float b = 0.0F;
    float a = 1.0F;
};

// ClearColor 表示帧开始时用于清屏的颜色。
using ClearColor = ColorF;

} // namespace cg::render
