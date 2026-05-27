#pragma once

#include "render/RenderTypes.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace cg::platform {

struct Event;

} // namespace cg::platform

namespace cg::ui {

// InputBox 保存单行文本输入状态，不负责绘制。
class InputBox final {
public:
    void SetBounds(render::RectF bounds) noexcept;
    [[nodiscard]] const render::RectF& GetBounds() const noexcept;

    void SetFocused(bool focused) noexcept;
    [[nodiscard]] bool IsFocused() const noexcept;

    void SetText(std::string_view text);
    [[nodiscard]] std::string_view GetText() const noexcept;

    void SetCursorByteOffset(std::size_t offset) noexcept;
    [[nodiscard]] std::size_t GetCursorByteOffset() const noexcept;

    [[nodiscard]] bool WasSubmitted() const noexcept;
    [[nodiscard]] bool ConsumeSubmitted() noexcept;

    [[nodiscard]] bool HandleEvent(const platform::Event& event);

private:
    render::RectF bounds_ = {};
    std::string text_;
    std::size_t cursorByteOffset_ = 0;
    bool focused_ = false;
    bool submitted_ = false;
};

} // namespace cg::ui
