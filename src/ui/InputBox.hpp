#pragma once

#include "render/RenderTypes.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace cg::platform {

struct Event;

} // namespace cg::platform

namespace cg::ui {

enum class CandidateDisplayMode {
    System,
    Custom,
};

// InputBox 保存单行文本输入状态，不负责绘制。
class InputBox final {
public:
    void SetBounds(render::RectF bounds) noexcept;
    [[nodiscard]] const render::RectF& GetBounds() const noexcept;

    void SetFocused(bool focused) noexcept;
    [[nodiscard]] bool IsFocused() const noexcept;

    void SetText(std::string_view text);
    [[nodiscard]] std::string_view GetText() const noexcept;
    [[nodiscard]] std::string GetDisplayText() const;

    void SetCursorByteOffset(std::size_t offset) noexcept;
    [[nodiscard]] std::size_t GetCursorByteOffset() const noexcept;

    [[nodiscard]] std::string_view GetCompositionText() const noexcept;
    [[nodiscard]] std::size_t GetCompositionCursor() const noexcept;
    [[nodiscard]] std::size_t GetCompositionSelectionLength() const noexcept;
    [[nodiscard]] bool HasComposition() const noexcept;

    void SetCandidateDisplayMode(CandidateDisplayMode mode) noexcept;
    [[nodiscard]] CandidateDisplayMode GetCandidateDisplayMode() const noexcept;
    [[nodiscard]] const std::vector<std::string>& GetCandidates() const noexcept;
    [[nodiscard]] int GetSelectedCandidate() const noexcept;
    [[nodiscard]] bool AreCandidatesHorizontal() const noexcept;
    [[nodiscard]] bool HasCandidates() const noexcept;

    [[nodiscard]] bool WasSubmitted() const noexcept;
    [[nodiscard]] bool ConsumeSubmitted() noexcept;

    [[nodiscard]] bool HandleEvent(const platform::Event& event);

private:
    void ClearComposition() noexcept;
    void ClearCandidates() noexcept;

    render::RectF bounds_ = {};
    std::string text_;
    std::string compositionText_;
    std::vector<std::string> candidates_;
    std::size_t cursorByteOffset_ = 0;
    std::size_t compositionCursor_ = 0;
    std::size_t compositionSelectionLength_ = 0;
    int selectedCandidate_ = -1;
    bool candidatesHorizontal_ = false;
    CandidateDisplayMode candidateDisplayMode_ = CandidateDisplayMode::System;
    bool focused_ = false;
    bool submitted_ = false;
};

} // namespace cg::ui
