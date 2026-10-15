#pragma once

#include <cstdint>

namespace cg::world::detail {

// 原 [0x621FF8,0x62683C)：64 项记录与尾槽一次传输，游标不在文件块内。
struct ChatHistory621FF8 {
    char records[64][0x121];
    std::uint32_t tail626838; // 原全局：0x00626838。
};

// 聊天历史的唯一存储与游标，输入逻辑借用。
extern ChatHistory621FF8 gChatHistory621FF8;
extern std::int32_t gCursor62683C;

} // namespace cg::world::detail
