#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::login::detail {

// 原 [1051B14,1057BF4)：供 52FB60 排序的 200 条 0x7C 字节运行记录。
struct OrderedRecord1051B14 final {
    std::int32_t value00;
    std::int32_t value04;
    std::uint16_t value08;
    std::int16_t value0A;
    std::int32_t value0C;
    std::array<std::byte, 8> opaque10;
    char text18[100];
};
static_assert(offsetof(OrderedRecord1051B14, value04) == 0x04);
static_assert(offsetof(OrderedRecord1051B14, value08) == 0x08);
static_assert(offsetof(OrderedRecord1051B14, value0A) == 0x0A);
static_assert(offsetof(OrderedRecord1051B14, value0C) == 0x0C);
static_assert(offsetof(OrderedRecord1051B14, text18) == 0x18);
static_assert(sizeof(OrderedRecord1051B14) == 0x7C);

extern std::array<OrderedRecord1051B14, 200> gOrderedRecords1051B14;
extern std::uint32_t gOrderMode1057C78; // 52F910 写入，52FB60 每次比较时读取。

// 原 52FB60：qsort 的两个 W32 索引参数；相等时仍返回 -1。
int CompareOrderedRecords_52FB60(const void* left, const void* right);
// 原 52FCC0：原位填 0..199 后用上述比较器重排共享运行表。
void BuildOrderedRecordIndices_52FCC0();

} // namespace cg::login::detail
