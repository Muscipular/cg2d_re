#include "../WindowComponents.hpp"
namespace cg::ui {
using namespace component_detail;
int WindowScrollViewData::MaxScrollY(const WindowElement& element) const { return std::max(0, contentHeight - element.height); }
void WindowScrollViewData::SetScrollY(const WindowElement& element, int value) {
    const int maximum = MaxScrollY(element);
    int next = std::clamp(value, 0, maximum);
    if (scrollStep > 1 && next < maximum) next = next / scrollStep * scrollStep;
    scrollY = next;
}
void WindowScrollViewData::SetContentHeight(const WindowElement& element, int value) { contentHeight = std::max(0, value); SetScrollY(element, scrollY); }
void WindowScrollViewData::SetScrollStep(const WindowElement& element, int value) { scrollStep = std::max(1, value); SetScrollY(element, scrollY); }
static bool ScrollClip(const WindowElement& element, int x, int y, const UIClipRect* parent, UIClipRect& view, UIClipRect& content) {
    const auto& data = *element.data.scrollView;
    UIClipRect bounds{static_cast<short>(x), static_cast<short>(y), static_cast<short>(x + element.width), static_cast<short>(y + element.height)};
    if (element.visible != 1 || element.width <= 0 || element.height <= 0 || !UIIntersectClipRect(view, parent, bounds)) return false;
    if (data.MaxScrollY(element) > 0) bounds.right -= static_cast<short>(std::clamp(data.barWidth, 1, element.width));
    UIIntersectClipRect(content, &view, bounds);
    return true;
}
void WindowElement_DrawScroll(WindowElement& element, int ox, int oy, const UIClipRect* clip) {
    auto& window = *element.owner;
    WindowElementCursor lease(window);
    const auto self = WindowRuntime_ElementRef(element);
    auto& data = *element.data.scrollView;
    const int x = ox + element.x, y = oy + element.y;
    UIClipRect view{}, content{};
    if (!ScrollClip(element, x, y, clip, view, content)) { WindowRuntime_BlurElement(element); return; }
    for (const auto& ref : WindowRuntime_Children(window, element.elementId)) {
        if (!WindowRuntime_ResolveElement(self) || element.detaching || element.visible != 1 || (window.state & WIN_STATE_CLOSE)) return;
        if (auto* child = WindowRuntime_ResolveElement(ref); child && UIClipRectValid(content))
            WindowRuntime_DrawElement(window, child->elementId, x, y - data.scrollY, &content);
        else if (child) WindowRuntime_BlurElement(*child);
    }
    const int maximum = data.MaxScrollY(element);
    if (maximum <= 0) return;
    const int bar = std::clamp(data.barWidth, 1, element.width);
    const int thumb = std::min(element.height, std::max(12, element.height * element.height / std::max(data.contentHeight, element.height)));
    const int range = std::max(1, element.height - thumb);
    RendererDrawRectClipped(x + element.width - bar, y, bar, element.height, LAYER_UI, data.trackColor, &view);
    RendererDrawRectClipped(x + element.width - bar, y + data.scrollY * range / maximum, bar, thumb, LAYER_UI, data.thumbColor, &view);
}
int WindowElement_ProcessScroll(WindowElement& element, int events, int ox, int oy, const UIClipRect* clip) {
    auto& window = *element.owner;
    WindowElementCursor lease(window);
    const auto self = WindowRuntime_ElementRef(element);
    auto& data = *element.data.scrollView;
    const int x = ox + element.x, y = oy + element.y;
    UIClipRect view{}, content{};
    if (!element.hitable || !ScrollClip(element, x, y, clip, view, content)) {
        data.draggingThumb = false;
        for (const auto& ref : WindowRuntime_Children(window, element.elementId))
            if (auto* child = WindowRuntime_ResolveElement(ref)) WindowRuntime_ResetElement(*child);
        return 0;
    }
    const int maximum = data.MaxScrollY(element);
    bool handled = false;
    if (maximum > 0 && !WindowRuntime_PointerDragBlocksMouse()) {
        const int wheel = WindowPeekMouseWheelDelta();
        if ((events & MouseFlag_Hover) && wheel != 0) {
            int step = wheel / 120;
            if (!step) step = wheel > 0 ? 1 : -1;
            data.SetScrollY(element, data.scrollY - step * data.scrollStep);
            WindowConsumeMouseWheelDelta(); handled = true;
        }
        const int bar = std::clamp(data.barWidth, 1, element.width);
        const int thumb = std::min(element.height, std::max(12, element.height * element.height / std::max(data.contentHeight, element.height)));
        const int range = std::max(1, element.height - thumb);
        const int thumbY = y + data.scrollY * range / maximum;
        const bool inTrack = MouseIsInRect(x + element.width - bar, y, bar, element.height) && UIPointInClipRect(gMouseX, gMouseY, &view);
        const bool inThumb = MouseIsInRect(x + element.width - bar, thumbY, bar, thumb) && UIPointInClipRect(gMouseX, gMouseY, &view);
        if (data.draggingThumb) {
            if (gMouseBtnDown2 & platform::LegacyMouseButton_Left) {
                data.SetScrollY(element, (gMouseY - data.dragOffsetY - y) * maximum / range); handled = true;
            } else data.draggingThumb = false;
        } else if (inTrack && (events & MouseFlag_LDownEvent)) {
            data.draggingThumb = true; data.dragOffsetY = inThumb ? gMouseY - thumbY : thumb / 2;
            data.SetScrollY(element, (gMouseY - data.dragOffsetY - y) * maximum / range); handled = true;
        }
    } else if (!(gMouseBtnDown2 & platform::LegacyMouseButton_Left)) data.draggingThumb = false;
    if (handled) { element.mouseState = WindowMouseState_Press; return 1; }
    int childrenHandled = 0;
    for (const auto& ref : WindowRuntime_Children(window, element.elementId)) {
        if (!WindowRuntime_ResolveElement(self) || !WindowRuntime_IsElementInteractive(window, element)) break;
        auto* child = WindowRuntime_ResolveElement(ref);
        if (!child) continue;
        if (!UIClipRectValid(content)) { WindowRuntime_ResetElement(*child); continue; }
        if (window.binding) window.binding->elementUpdate(*child);
        child = WindowRuntime_ResolveElement(ref);
        if (child && WindowRuntime_IsElementInteractive(window, element))
            childrenHandled |= WindowRuntime_DispatchElementMouse(window, child->elementId, x, y - data.scrollY, &content);
    }
    return childrenHandled;
}
}
