#include "InputBox.hpp"

#include "platform/Event.hpp"

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

#include <algorithm>
#include <cstddef>
#include <string_view>

namespace cg::ui {
namespace {

[[nodiscard]] bool IsUtf8Continuation(unsigned char value) noexcept
{
    return (value & 0xC0U) == 0x80U;
}

[[nodiscard]] std::size_t ClampToUtf8Boundary(std::string_view text, std::size_t offset) noexcept
{
    offset = std::min(offset, text.size());
    while (offset > 0 && offset < text.size() && IsUtf8Continuation(static_cast<unsigned char>(text[offset]))) {
        --offset;
    }

    return offset;
}

[[nodiscard]] std::size_t NextUtf8Offset(std::string_view text, std::size_t offset) noexcept
{
    offset = ClampToUtf8Boundary(text, offset);
    if (offset >= text.size()) {
        return text.size();
    }

    const auto first = static_cast<unsigned char>(text[offset]);
    const std::size_t remaining = text.size() - offset;
    if ((first & 0x80U) == 0U) {
        return offset + 1;
    }
    if ((first & 0xE0U) == 0xC0U && remaining >= 2 &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 1]))) {
        return offset + 2;
    }
    if ((first & 0xF0U) == 0xE0U && remaining >= 3 &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 1])) &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 2]))) {
        return offset + 3;
    }
    if ((first & 0xF8U) == 0xF0U && remaining >= 4 &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 1])) &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 2])) &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 3]))) {
        return offset + 4;
    }

    return offset + 1;
}

[[nodiscard]] std::size_t PreviousUtf8Offset(std::string_view text, std::size_t offset) noexcept
{
    offset = ClampToUtf8Boundary(text, offset);
    if (offset == 0) {
        return 0;
    }

    std::size_t previous = offset - 1;
    while (previous > 0 && IsUtf8Continuation(static_cast<unsigned char>(text[previous]))) {
        --previous;
    }

    return previous;
}

[[nodiscard]] std::size_t CountUtf8Codepoints(std::string_view text) noexcept
{
    std::size_t count = 0;
    std::size_t offset = 0;
    while (offset < text.size()) {
        const std::size_t next = NextUtf8Offset(text, offset);
        offset = next > offset ? next : offset + 1;
        ++count;
    }

    return count;
}

[[nodiscard]] std::size_t ToNonNegativeSize(int value) noexcept
{
    return value > 0 ? static_cast<std::size_t>(value) : 0;
}

[[nodiscard]] bool Contains(render::RectF bounds, float x, float y) noexcept
{
    return x >= bounds.x &&
        y >= bounds.y &&
        x < bounds.x + bounds.width &&
        y < bounds.y + bounds.height;
}

} // namespace

void InputBox::SetBounds(render::RectF bounds) noexcept
{
    bounds_ = bounds;
}

const render::RectF& InputBox::GetBounds() const noexcept
{
    return bounds_;
}

void InputBox::SetFocused(bool focused) noexcept
{
    focused_ = focused;
    if (!focused_) {
        ClearComposition();
    }
}

bool InputBox::IsFocused() const noexcept
{
    return focused_;
}

void InputBox::SetText(std::string_view text)
{
    text_.assign(text);
    cursorByteOffset_ = text_.size();
    ClearComposition();
    submitted_ = false;
}

std::string_view InputBox::GetText() const noexcept
{
    return text_;
}

std::string InputBox::GetDisplayText() const
{
    std::string displayText = text_;
    const std::size_t insertOffset = ClampToUtf8Boundary(displayText, cursorByteOffset_);
    displayText.insert(insertOffset, compositionText_);
    return displayText;
}

void InputBox::SetCursorByteOffset(std::size_t offset) noexcept
{
    cursorByteOffset_ = ClampToUtf8Boundary(text_, offset);
}

std::size_t InputBox::GetCursorByteOffset() const noexcept
{
    return cursorByteOffset_;
}

std::string_view InputBox::GetCompositionText() const noexcept
{
    return compositionText_;
}

std::size_t InputBox::GetCompositionCursor() const noexcept
{
    return compositionCursor_;
}

std::size_t InputBox::GetCompositionSelectionLength() const noexcept
{
    return compositionSelectionLength_;
}

bool InputBox::HasComposition() const noexcept
{
    return !compositionText_.empty();
}

void InputBox::SetCandidateDisplayMode(CandidateDisplayMode mode) noexcept
{
    candidateDisplayMode_ = mode;
}

CandidateDisplayMode InputBox::GetCandidateDisplayMode() const noexcept
{
    return candidateDisplayMode_;
}

const std::vector<std::string>& InputBox::GetCandidates() const noexcept
{
    return candidates_;
}

int InputBox::GetSelectedCandidate() const noexcept
{
    return selectedCandidate_;
}

bool InputBox::AreCandidatesHorizontal() const noexcept
{
    return candidatesHorizontal_;
}

bool InputBox::HasCandidates() const noexcept
{
    return !candidates_.empty();
}

bool InputBox::WasSubmitted() const noexcept
{
    return submitted_;
}

bool InputBox::ConsumeSubmitted() noexcept
{
    const bool submitted = submitted_;
    submitted_ = false;
    return submitted;
}

void InputBox::ClearComposition() noexcept
{
    compositionText_.clear();
    compositionCursor_ = 0;
    compositionSelectionLength_ = 0;
    ClearCandidates();
}

void InputBox::ClearCandidates() noexcept
{
    candidates_.clear();
    selectedCandidate_ = -1;
    candidatesHorizontal_ = false;
}

bool InputBox::HandleEvent(const platform::Event& event)
{
    if (event.type == platform::EventType::MouseButtonDown && event.button == SDL_BUTTON_LEFT) {
        const bool wasFocused = focused_;
        focused_ = Contains(bounds_, event.mouseX, event.mouseY);
        if (focused_) {
            cursorByteOffset_ = text_.size();
        } else {
            ClearComposition();
        }

        return focused_ || wasFocused;
    }

    if (!focused_) {
        return false;
    }

    if (event.type == platform::EventType::TextInput) {
        if (event.text.empty()) {
            return false;
        }

        cursorByteOffset_ = ClampToUtf8Boundary(text_, cursorByteOffset_);
        text_.insert(cursorByteOffset_, event.text);
        cursorByteOffset_ += event.text.size();
        ClearComposition();
        submitted_ = false;
        return true;
    }

    if (event.type == platform::EventType::TextEditing) {
        compositionText_ = event.text;
        const std::size_t compositionLength = CountUtf8Codepoints(compositionText_);
        compositionCursor_ = std::min(ToNonNegativeSize(event.cursor), compositionLength);
        compositionSelectionLength_ =
            std::min(ToNonNegativeSize(event.selectionLength), compositionLength - compositionCursor_);
        if (compositionText_.empty()) {
            ClearCandidates();
        }
        submitted_ = false;
        return true;
    }

    if (event.type == platform::EventType::TextEditingCandidates) {
        candidates_ = event.candidates;
        selectedCandidate_ = event.selectedCandidate;
        candidatesHorizontal_ = event.candidatesHorizontal;
        submitted_ = false;
        return true;
    }

    if (event.type != platform::EventType::KeyDown) {
        return false;
    }

    switch (event.key) {
    case SDLK_BACKSPACE: {
        ClearComposition();
        const std::size_t previous = PreviousUtf8Offset(text_, cursorByteOffset_);
        if (previous != cursorByteOffset_) {
            text_.erase(previous, cursorByteOffset_ - previous);
            cursorByteOffset_ = previous;
        }
        submitted_ = false;
        return true;
    }
    case SDLK_DELETE: {
        ClearComposition();
        cursorByteOffset_ = ClampToUtf8Boundary(text_, cursorByteOffset_);
        const std::size_t next = NextUtf8Offset(text_, cursorByteOffset_);
        if (next != cursorByteOffset_) {
            text_.erase(cursorByteOffset_, next - cursorByteOffset_);
        }
        submitted_ = false;
        return true;
    }
    case SDLK_LEFT:
        ClearComposition();
        cursorByteOffset_ = PreviousUtf8Offset(text_, cursorByteOffset_);
        return true;
    case SDLK_RIGHT:
        ClearComposition();
        cursorByteOffset_ = NextUtf8Offset(text_, cursorByteOffset_);
        return true;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        submitted_ = true;
        return true;
    default:
        return false;
    }
}

} // namespace cg::ui
