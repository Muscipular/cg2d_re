#pragma once

#include "InputBox.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace cg::platform {

struct Event;

} // namespace cg::platform

namespace cg::ui {

struct ChatMessage {
    std::string text;
};

// ChatBox 保存聊天历史、滚动和输入栏状态，不负责绘制。
class ChatBox final {
public:
    [[nodiscard]] InputBox& Input() noexcept;
    [[nodiscard]] const InputBox& GetInput() const noexcept;

    [[nodiscard]] bool HandleEvent(const platform::Event& event);

    void AddMessage(std::string_view text);
    [[nodiscard]] const std::vector<ChatMessage>& GetMessages() const noexcept;
    [[nodiscard]] std::vector<ChatMessage> GetVisibleMessages(std::size_t maxCount) const;

    void ScrollLines(int delta) noexcept;
    [[nodiscard]] std::size_t GetScrollOffset() const noexcept;

private:
    void ClampScrollOffset() noexcept;

    InputBox input_;
    std::vector<ChatMessage> messages_;
    std::size_t scrollOffset_ = 0;
};

} // namespace cg::ui
