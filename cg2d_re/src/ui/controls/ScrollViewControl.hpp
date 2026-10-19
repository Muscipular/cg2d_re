#pragma once
#include "UIControl.hpp"

namespace cg::ui {
struct UIScrollViewControl : UIControl {
    int scrollY = 0;
    int contentHeight = 0;
    int barWidth = 8;
    int scrollStep = 1;
    COLORREF trackColor = 0x60000000;
    COLORREF thumbColor = 0xc0ffffff;
    bool draggingThumb = false;
    int dragOffsetY = 0;
    std::vector<UIControl*> children() const; // 借用窗口排序缓存，不拥有子控件。

    UIScrollViewControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, int contentHeight, int scrollY, int barWidth, int scrollStep, WindowEventBinding callbacks);

    bool render(short ox, short oy) override;
    bool render(short ox, short oy, const UIClipRect* clipRect) override;

    bool process(int index, int hasFocused, int captured) override;
    bool process(int index, int hasFocused, int captured, const UIClipRect* clipRect, short ox, short oy) override;

    UIControl* FindChild(int controlId);
    void blurTextInput() override;

    int MaxScrollY() const;
    void SetScrollY(int value);
    void SetContentHeight(int value);
    void SetScrollStep(int value);
};
} // namespace cg::ui
