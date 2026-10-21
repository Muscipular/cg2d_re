#pragma once
#include "UIControl.hpp"
namespace cg::ui {
struct WindowScrollViewData {
    int scrollY = 0;
    int contentHeight = 0;
    int barWidth = 8;
    int scrollStep = 1;
    COLORREF trackColor = 0x60000000;
    COLORREF thumbColor = 0xc0ffffff;
    bool draggingThumb = false;
    int dragOffsetY = 0;
    int MaxScrollY(const WindowElement& element) const;
    void SetScrollY(const WindowElement& element, int value);
    void SetContentHeight(const WindowElement& element, int value);
    void SetScrollStep(const WindowElement& element, int value);
};
void WindowElement_DrawScroll(WindowElement& element, int ox, int oy, const UIClipRect* clip);
int WindowElement_ProcessScroll(WindowElement& element, int events, int ox, int oy, const UIClipRect* clip);
}
