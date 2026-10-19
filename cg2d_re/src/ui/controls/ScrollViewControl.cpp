#include "../WindowComponents.hpp"

namespace cg::ui {
using namespace component_detail;
static void BlurChildTextInputs(UIControl* control) {
    if (!control) {
        return;
    }
    control->blurTextInput();
    if (control->type == UIControl::TYPE_SCROLL_VIEW) {
        auto view = static_cast<UIScrollViewControl*>(control);
        for (auto& child: view->children()) {
            BlurChildTextInputs(child);
        }
    }
}

static void BlurChildrenTextInputs(const std::vector<UIControl*>& children) {
    for (auto& child: children) {
        BlurChildTextInputs(child);
    }
}

static void ResetChildProcessState(UIControl* control) {
    if (!control) {
        return;
    }
    const uint8_t previousMouseState = control->mouseState;
    control->mouseState = UIControl::MOUSE_NONE;
    control->blurTextInput();
    UIEmitControlLeaveIfNeeded({control->winId, control->generation, control->id}, control->callbacks, previousMouseState, control->mouseState);
    if (control->type == UIControl::TYPE_SCROLL_VIEW) {
        auto view = static_cast<UIScrollViewControl*>(control);
        view->draggingThumb = false;
        for (auto* child : view->children()) {
            if (IsControlAttached({child->winId, child->generation, child->id})) ResetChildProcessState(child);
        }
    }
}

static void ResetChildrenProcessStates(const std::vector<UIControl*>& children) {
    for (auto* child : children) {
        if (IsControlAttached({child->winId, child->generation, child->id})) ResetChildProcessState(child);
    }
}

UIScrollViewControl::UIScrollViewControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, int contentHeight, int scrollY, int barWidth, int scrollStep, WindowEventBinding callbacks)
    : UIControl(winId, generation, id, TYPE_SCROLL_VIEW, name, x, y, w, h, visible, callbacks), contentHeight(contentHeight), barWidth(barWidth), scrollStep(scrollStep) {
    this->contentHeight = std::max(0, this->contentHeight);
    if (this->barWidth <= 0) {
        this->barWidth = 8;
    }
    if (this->scrollStep <= 0) {
        this->scrollStep = 1;
    }
    SetScrollY(scrollY);
}

void UIScrollViewControl::blurTextInput() { BlurChildrenTextInputs(children()); }

int UIScrollViewControl::MaxScrollY() const {
    return std::max(0, contentHeight - static_cast<int>(element().height));
}

void UIScrollViewControl::SetScrollY(int value) {
    int maxScrollY = MaxScrollY();
    int nextScrollY = std::clamp(value, 0, maxScrollY);
    if (scrollStep > 1 && nextScrollY < maxScrollY) {
        nextScrollY = (nextScrollY / scrollStep) * scrollStep;
    }
    scrollY = nextScrollY;
}

void UIScrollViewControl::SetContentHeight(int value) {
    contentHeight = std::max(0, value);
    SetScrollY(scrollY);
}

void UIScrollViewControl::SetScrollStep(int value) {
    scrollStep = std::max(1, value);
    SetScrollY(scrollY);
}

std::vector<UIControl*> UIScrollViewControl::children() const {
    return windowOwner ? WindowRuntime_Children(*windowOwner, id) : std::vector<UIControl*>{};
}
UIControl* UIScrollViewControl::FindChild(int controlId) {
    for (auto* child : children()) {
        if (child->id == controlId) return child;
        if (child->type == TYPE_SCROLL_VIEW)
            if (auto* nested = static_cast<UIScrollViewControl*>(child)->FindChild(controlId)) return nested;
    }
    return nullptr;
}

bool UIScrollViewControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UIScrollViewControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (element().visible != 1 || element().width <= 0 || element().height <= 0) {
        BlurChildrenTextInputs(children());
        return true;
    }

    const int absX = ox + element().x;
    const int absY = oy + element().y;
    UIClipRect viewRect{
        static_cast<short>(absX),
        static_cast<short>(absY),
        static_cast<short>(absX + element().width),
        static_cast<short>(absY + element().height)
    };
    UIClipRect viewClip{};
    if (!UIIntersectClipRect(viewClip, clipRect, viewRect)) {
        BlurChildrenTextInputs(children());
        return true;
    }

    const int maxScroll = MaxScrollY();
    UIClipRect contentRect{
        static_cast<short>(absX),
        static_cast<short>(absY),
        static_cast<short>(absX + element().width - (maxScroll > 0 ? std::clamp(barWidth, 1, static_cast<int>(element().width)) : 0)),
        static_cast<short>(absY + element().height)
    };
    UIClipRect contentClip{};
    if (!UIIntersectClipRect(contentClip, clipRect, contentRect)) {
        BlurChildrenTextInputs(children());
        contentClip = {};
    }

    short childOx = static_cast<short>(absX);
    short childOy = static_cast<short>(absY - scrollY);
    for (auto* child : children()) {
        if (!attached || !element().visible || (windowOwner->state & WIN_STATE_CLOSE)
            || !(windowOwner->state & WIN_STATE_ACTIVE)) break;
        if (child->attached && UIClipRectValid(contentClip)) {
            RendererBreakWindowTextBatch();
            if (windowOwner->binding) windowOwner->binding->elementDraw(child->element());
            if (child->attached && attached && element().visible && !(windowOwner->state & WIN_STATE_CLOSE)
                && (windowOwner->state & WIN_STATE_ACTIVE)) child->render(childOx, childOy, &contentClip);
        }
    }

    if (!attached || !element().visible || (windowOwner->state & WIN_STATE_CLOSE)
        || !(windowOwner->state & WIN_STATE_ACTIVE) || maxScroll <= 0) {
        return true;
    }

    int drawBarWidth = std::clamp(barWidth, 1, static_cast<int>(element().width));
    int trackX = absX + element().width - drawBarWidth;
    RendererDrawRectClipped(static_cast<int16_t>(trackX), static_cast<int16_t>(absY), static_cast<int16_t>(drawBarWidth), element().height, LAYER_UI, trackColor, &viewClip);

    int thumbHeight = std::max(12, element().height * element().height / std::max(contentHeight, static_cast<int>(element().height)));
    thumbHeight = std::min<int>(thumbHeight, element().height);
    int thumbRange = std::max(1, element().height - thumbHeight);
    int thumbY = absY + scrollY * thumbRange / maxScroll;
    RendererDrawRectClipped(static_cast<int16_t>(trackX), static_cast<int16_t>(thumbY), static_cast<int16_t>(drawBarWidth), static_cast<int16_t>(thumbHeight), LAYER_UI, thumbColor, &viewClip);
    return true;
}

bool UIScrollViewControl::process(int index, int hasFocused, int captured) {
    return process(index, hasFocused, captured, nullptr, static_cast<short>(gCurrentWindow->x), static_cast<short>(gCurrentWindow->y));
}

bool UIScrollViewControl::process(int index, int hasFocused, int captured, const UIClipRect* clipRect, short ox, short oy) {
    const uint8_t previousMouseState = mouseState;
    if (element().visible != 1 || !hitable || element().width <= 0 || element().height <= 0) {
        mouseState = MOUSE_NONE;
        draggingThumb = false;
        ResetChildrenProcessStates(children());
        UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
        return false;
    }

    const int absX = ox + element().x;
    const int absY = oy + element().y;
    UIClipRect viewRect{
        static_cast<short>(absX),
        static_cast<short>(absY),
        static_cast<short>(absX + element().width),
        static_cast<short>(absY + element().height)
    };
    UIClipRect viewClip{};
    if (!UIIntersectClipRect(viewClip, clipRect, viewRect)) {
        mouseState = MOUSE_NONE;
        draggingThumb = false;
        ResetChildrenProcessStates(children());
        UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
        return false;
    }

    const int maxScroll = MaxScrollY();
    bool handled = false;

    if (maxScroll > 0 && hasFocused && !captured && !WindowRuntime_PointerDragBlocksMouse()) {
        int wheelDelta = WindowPeekMouseWheelDelta();
        if (wheelDelta != 0 && MouseIsInRect(absX, absY, element().width, element().height) && UIPointInClipRect(gMouseX, gMouseY, &viewClip)) {
            int wheelStep = wheelDelta / 120;
            if (wheelStep == 0) {
                wheelStep = wheelDelta > 0 ? 1 : -1;
            }
            SetScrollY(scrollY - wheelStep * scrollStep);
            WindowConsumeMouseWheelDelta();
            handled = true;
        }

        int drawBarWidth = std::clamp(barWidth, 1, static_cast<int>(element().width));
        int trackX = absX + element().width - drawBarWidth;
        int thumbHeight = std::max(12, element().height * element().height / std::max(contentHeight, static_cast<int>(element().height)));
        thumbHeight = std::min<int>(thumbHeight, element().height);
        int thumbRange = std::max(1, element().height - thumbHeight);
        int thumbY = absY + scrollY * thumbRange / maxScroll;
        bool inTrack = MouseIsInRect(trackX, absY, drawBarWidth, element().height) && UIPointInClipRect(gMouseX, gMouseY, &viewClip);
        bool inThumb = MouseIsInRect(trackX, thumbY, drawBarWidth, thumbHeight) && UIPointInClipRect(gMouseX, gMouseY, &viewClip);

        if (draggingThumb) {
            if (gMouseBtnDown2 & 0x01) {
                int nextThumbY = gMouseY - dragOffsetY - absY;
                SetScrollY(nextThumbY * maxScroll / thumbRange);
                handled = true;
            } else {
                draggingThumb = false;
            }
        } else if (inTrack && (gMouseClickState & 0x01)) {
            draggingThumb = true;
            dragOffsetY = inThumb ? gMouseY - thumbY : thumbHeight / 2;
            int nextThumbY = gMouseY - dragOffsetY - absY;
            SetScrollY(nextThumbY * maxScroll / thumbRange);
            handled = true;
        }
    } else if (!(gMouseBtnDown2 & 0x01)) {
        draggingThumb = false;
    }

    if (handled) {
        mouseState = MOUSE_PRESS;
        gMouseEventState = MES_CAPTURED;
        gWindowHasFocus = 1;
        return true;
    }

    UIClipRect contentRect{
        static_cast<short>(absX),
        static_cast<short>(absY),
        static_cast<short>(absX + element().width - (maxScroll > 0 ? std::clamp(barWidth, 1, static_cast<int>(element().width)) : 0)),
        static_cast<short>(absY + element().height)
    };
    UIClipRect contentClip{};
    bool hasContentClip = UIIntersectClipRect(contentClip, clipRect, contentRect);
    if (!hasContentClip) {
        ResetChildrenProcessStates(children());
    }

    short childOx = static_cast<short>(absX);
    short childOy = static_cast<short>(absY - scrollY);
    bool childCaptured = false;
    if (hasContentClip) {
        const auto snapshot = children();
        for (int i = 0; i < static_cast<int>(snapshot.size()); ++i) {
            auto* child = snapshot[i];
            if (!attached || !element().visible || (windowOwner->state & WIN_STATE_CLOSE)
                || !(windowOwner->state & WIN_STATE_ACTIVE)) break;
            if (child->attached && windowOwner->binding) windowOwner->binding->elementUpdate(child->element());
            if (IsControlAttached({child->winId, child->generation, child->id})
                && attached && element().visible && (windowOwner->state & WIN_STATE_ACTIVE)
                && child->process(static_cast<int>(&child->element() - windowOwner->elements), hasFocused, captured || childCaptured, &contentClip, childOx, childOy)) {
                childCaptured = true;
            }
        }
    }
    if (childCaptured) {
        return true;
    }

    return UIControl::process(index, hasFocused, captured, &viewClip, ox, oy);
}
} // namespace cg::ui
