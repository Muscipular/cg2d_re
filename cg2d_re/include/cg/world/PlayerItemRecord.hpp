#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace cg::world {

// 原FDA8A4起的28条玩家物品；NIO01在网络字段解析后持有UTF-16，注释偏移仅作原端参考。
struct PlayerItemRecord {
    std::uint16_t used; // 原成员偏移：+0x00。
    std::wstring name; // 原成员偏移：+0x02；输入GBK上限28B。
    std::wstring memo; // 原成员偏移：+0x1F；输入GBK上限16B。
    std::array<std::wstring, 32> description; // 原成员偏移：+0x30；32行按原GBK列规则解析。
    std::int32_t pages; // 原成员偏移：+0xC30。
    std::int32_t color; // 原成员偏移：+0xC34。
    std::int32_t imageNo; // 原成员偏移：+0xC38。
    std::int32_t level; // 原成员偏移：+0xC3C。
    std::int32_t itemId; // 原成员偏移：+0xC40。
    std::int32_t count; // 原成员偏移：+0xC44。
    std::int32_t itemType; // 原成员偏移：+0xC48。
    std::uint16_t canUseInField; // 原成员偏移：+0xC4C。
    std::uint16_t battleUseFlag; // 原成员偏移：+0xC4E。
    std::uint16_t target; // 原成员偏移：+0xC50。
    std::uint16_t flags; // 原成员偏移：+0xC52。
    std::uint16_t hasDescription; // 原成员偏移：+0xC54。
    std::array<std::byte, 6> bytesC56; // 原成员偏移：+0xC56。
};

} // namespace cg::world
