#pragma once


namespace cg::ui {
struct UICanvasControl : UIControl {
    std::vector<CanvasShape> shapes;

    UICanvasControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, std::vector<CanvasShape> shapes, WindowEventBinding callbacks);

    bool render(short ox, short oy) override;
    bool render(short ox, short oy, const UIClipRect* clipRect) override;

    void SetShapes(std::vector<CanvasShape> newShapes);
};
} // namespace cg::ui
