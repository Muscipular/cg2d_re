#include "../LuaUI.hpp"

namespace cg::script {
static void BlurChildTextInputs(UIControl* control) {
    if (!control) {
        return;
    }
    control->blurTextInput();
    if (control->type == UIControl::TYPE_SCROLL_VIEW) {
        auto view = static_cast<UIScrollViewControl*>(control);
        for (auto& child: view->children) {
            BlurChildTextInputs(child.get());
        }
    }
}

static void BlurChildrenTextInputs(const std::vector<std::unique_ptr<UIControl>>& children) {
    for (auto& child: children) {
        BlurChildTextInputs(child.get());
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
        for (auto* child : SnapshotControls(view->children)) {
            if (IsLuaControlAttached({child->winId, child->generation, child->id})) ResetChildProcessState(child);
        }
    }
}

static void ResetChildrenProcessStates(const std::vector<std::unique_ptr<UIControl>>& children) {
    for (auto* child : SnapshotControls(children)) {
        if (IsLuaControlAttached({child->winId, child->generation, child->id})) ResetChildProcessState(child);
    }
}

UIScrollViewControl::UIScrollViewControl(int winId, uint32_t generation, int id, std::wstring_view name, short x, short y, short w, short h, uint8_t visible, int contentHeight, int scrollY, int barWidth, int scrollStep, LuaCallbackRefs callbacks)
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

void UIScrollViewControl::blurTextInput() { BlurChildrenTextInputs(children); }

int UIScrollViewControl::MaxScrollY() const {
    return std::max(0, contentHeight - static_cast<int>(h));
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

UIControl* UIScrollViewControl::AddChild(std::unique_ptr<UIControl> control) {
    if (!control) {
        return nullptr;
    }
    control->parentId = id;
    children.emplace_back(std::move(control));
    return children.back().get();
}

UIControl* UIScrollViewControl::FindChild(int controlId) {
    if (controlId <= 0) {
        return nullptr;
    }
    for (auto& child: children) {
        if (!child) {
            continue;
        }
        if (child->id == controlId) {
            return child.get();
        }
        if (child->type == UIControl::TYPE_SCROLL_VIEW) {
            auto nested = static_cast<UIScrollViewControl*>(child.get())->FindChild(controlId);
            if (nested) {
                return nested;
            }
        }
    }
    return nullptr;
}

bool UIScrollViewControl::render(short ox, short oy) {
    return render(ox, oy, nullptr);
}

bool UIScrollViewControl::render(short ox, short oy, const UIClipRect* clipRect) {
    if (visible != 1 || w <= 0 || h <= 0) {
        BlurChildrenTextInputs(children);
        return true;
    }

    const int absX = ox + x;
    const int absY = oy + y;
    UIClipRect viewRect{
        static_cast<short>(absX),
        static_cast<short>(absY),
        static_cast<short>(absX + w),
        static_cast<short>(absY + h)
    };
    UIClipRect viewClip{};
    if (!UIIntersectClipRect(viewClip, clipRect, viewRect)) {
        BlurChildrenTextInputs(children);
        return true;
    }

    const int maxScroll = MaxScrollY();
    UIClipRect contentRect{
        static_cast<short>(absX),
        static_cast<short>(absY),
        static_cast<short>(absX + w - (maxScroll > 0 ? std::clamp(barWidth, 1, static_cast<int>(w)) : 0)),
        static_cast<short>(absY + h)
    };
    UIClipRect contentClip{};
    if (!UIIntersectClipRect(contentClip, clipRect, contentRect)) {
        BlurChildrenTextInputs(children);
        contentClip = {};
    }

    short childOx = static_cast<short>(absX);
    short childOy = static_cast<short>(absY - scrollY);
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        if (*it && UIClipRectValid(contentClip)) {
            RendererBreakWindowTextBatch();
            (*it)->render(childOx, childOy, &contentClip);
        }
    }

    if (maxScroll <= 0) {
        return true;
    }

    int drawBarWidth = std::clamp(barWidth, 1, static_cast<int>(w));
    int trackX = absX + w - drawBarWidth;
    RendererDrawRectClipped(static_cast<int16_t>(trackX), static_cast<int16_t>(absY), static_cast<int16_t>(drawBarWidth), h, LAYER_UI, trackColor, &viewClip);

    int thumbHeight = std::max(12, h * h / std::max(contentHeight, static_cast<int>(h)));
    thumbHeight = std::min<int>(thumbHeight, h);
    int thumbRange = std::max(1, h - thumbHeight);
    int thumbY = absY + scrollY * thumbRange / maxScroll;
    RendererDrawRectClipped(static_cast<int16_t>(trackX), static_cast<int16_t>(thumbY), static_cast<int16_t>(drawBarWidth), static_cast<int16_t>(thumbHeight), LAYER_UI, thumbColor, &viewClip);
    return true;
}

bool UIScrollViewControl::process(int index, int hasFocused, int captured) {
    return process(index, hasFocused, captured, nullptr, static_cast<short>(gCurrentWindow->x), static_cast<short>(gCurrentWindow->y));
}

bool UIScrollViewControl::process(int index, int hasFocused, int captured, const UIClipRect* clipRect, short ox, short oy) {
    const uint8_t previousMouseState = mouseState;
    if (visible != 1 || !hitable || w <= 0 || h <= 0) {
        mouseState = MOUSE_NONE;
        draggingThumb = false;
        ResetChildrenProcessStates(children);
        UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
        return false;
    }

    const int absX = ox + x;
    const int absY = oy + y;
    UIClipRect viewRect{
        static_cast<short>(absX),
        static_cast<short>(absY),
        static_cast<short>(absX + w),
        static_cast<short>(absY + h)
    };
    UIClipRect viewClip{};
    if (!UIIntersectClipRect(viewClip, clipRect, viewRect)) {
        mouseState = MOUSE_NONE;
        draggingThumb = false;
        ResetChildrenProcessStates(children);
        UIEmitControlLeaveIfNeeded({winId, generation, id}, callbacks, previousMouseState, mouseState);
        return false;
    }

    const int maxScroll = MaxScrollY();
    bool handled = false;

    if (maxScroll > 0 && hasFocused && !captured) {
        int wheelDelta = LuaPeekMouseWheelDelta();
        if (wheelDelta != 0 && MouseIsInRect(absX, absY, w, h) && UIPointInClipRect(gMouseX, gMouseY, &viewClip)) {
            int wheelStep = wheelDelta / 120;
            if (wheelStep == 0) {
                wheelStep = wheelDelta > 0 ? 1 : -1;
            }
            SetScrollY(scrollY - wheelStep * scrollStep);
            LuaConsumeMouseWheelDelta();
            handled = true;
        }

        int drawBarWidth = std::clamp(barWidth, 1, static_cast<int>(w));
        int trackX = absX + w - drawBarWidth;
        int thumbHeight = std::max(12, h * h / std::max(contentHeight, static_cast<int>(h)));
        thumbHeight = std::min<int>(thumbHeight, h);
        int thumbRange = std::max(1, h - thumbHeight);
        int thumbY = absY + scrollY * thumbRange / maxScroll;
        bool inTrack = MouseIsInRect(trackX, absY, drawBarWidth, h) && UIPointInClipRect(gMouseX, gMouseY, &viewClip);
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
        static_cast<short>(absX + w - (maxScroll > 0 ? std::clamp(barWidth, 1, static_cast<int>(w)) : 0)),
        static_cast<short>(absY + h)
    };
    UIClipRect contentClip{};
    bool hasContentClip = UIIntersectClipRect(contentClip, clipRect, contentRect);
    if (!hasContentClip) {
        ResetChildrenProcessStates(children);
    }

    short childOx = static_cast<short>(absX);
    short childOy = static_cast<short>(absY - scrollY);
    bool childCaptured = false;
    if (hasContentClip) {
        const auto snapshot = SnapshotControls(children);
        for (int i = static_cast<int>(snapshot.size()) - 1; i >= 0; --i) {
            auto* child = snapshot[i];
            if (IsLuaControlAttached({child->winId, child->generation, child->id})
                && child->process(i, hasFocused, captured || childCaptured, &contentClip, childOx, childOy)) {
                childCaptured = true;
            }
        }
    }
    if (childCaptured) {
        return true;
    }

    return UIControl::process(index, hasFocused, captured, &viewClip, ox, oy);
}
} // namespace cg::script
