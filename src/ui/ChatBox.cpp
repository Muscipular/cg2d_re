#include "ChatBox.hpp"

#include "platform/Event.hpp"

#include <algorithm>

namespace cg::ui {

InputBox& ChatBox::Input() noexcept
{
    return input_;
}

const InputBox& ChatBox::GetInput() const noexcept
{
    return input_;
}

bool ChatBox::HandleEvent(const platform::Event& event)
{
    bool consumed = input_.HandleEvent(event);

    if (event.type == platform::EventType::MouseWheel && event.wheelY != 0.0F) {
        ScrollLines(static_cast<int>(event.wheelY));
        consumed = true;
    }

    if (input_.ConsumeSubmitted()) {
        const std::string text{input_.GetText()};
        if (!text.empty()) {
            AddMessage(text);
            input_.SetText({});
        }
        consumed = true;
    }

    return consumed;
}

void ChatBox::AddMessage(std::string_view text)
{
    messages_.push_back(ChatMessage{std::string{text}});
    scrollOffset_ = 0;
}

const std::vector<ChatMessage>& ChatBox::GetMessages() const noexcept
{
    return messages_;
}

std::vector<ChatMessage> ChatBox::GetVisibleMessages(std::size_t maxCount) const
{
    if (maxCount == 0 || messages_.empty()) {
        return {};
    }

    const std::size_t offset = std::min(scrollOffset_, messages_.size() - 1);
    const std::size_t end = messages_.size() - offset;
    const std::size_t count = std::min(maxCount, end);
    const std::size_t start = end - count;

    return {messages_.begin() + static_cast<std::ptrdiff_t>(start),
        messages_.begin() + static_cast<std::ptrdiff_t>(end)};
}

void ChatBox::ScrollLines(int delta) noexcept
{
    if (delta > 0) {
        scrollOffset_ += static_cast<std::size_t>(delta);
    } else if (delta < 0) {
        const std::size_t amount = static_cast<std::size_t>(-delta);
        scrollOffset_ = amount >= scrollOffset_ ? 0 : scrollOffset_ - amount;
    }

    ClampScrollOffset();
}

std::size_t ChatBox::GetScrollOffset() const noexcept
{
    return scrollOffset_;
}

void ChatBox::ClampScrollOffset() noexcept
{
    if (messages_.empty()) {
        scrollOffset_ = 0;
        return;
    }

    scrollOffset_ = std::min(scrollOffset_, messages_.size() - 1);
}

} // namespace cg::ui
