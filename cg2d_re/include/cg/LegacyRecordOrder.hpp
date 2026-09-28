#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::login::detail {

// 原 [1051B14,1057BF4)：解析、计数及排序共用的 200 条 0x7C 字节运行记录。
struct OrderedRecord1051B14 final {
    std::int32_t value00;
    std::int32_t value04;
    std::uint16_t value08;
    std::int16_t value0A;
    std::int32_t value0C;
    std::int32_t value10;
    std::int32_t value14;
    char text18[100];
};
static_assert(offsetof(OrderedRecord1051B14, value04) == 0x04);
static_assert(offsetof(OrderedRecord1051B14, value08) == 0x08);
static_assert(offsetof(OrderedRecord1051B14, value0A) == 0x0A);
static_assert(offsetof(OrderedRecord1051B14, value0C) == 0x0C);
static_assert(offsetof(OrderedRecord1051B14, value10) == 0x10);
static_assert(offsetof(OrderedRecord1051B14, value14) == 0x14);
static_assert(offsetof(OrderedRecord1051B14, text18) == 0x18);
static_assert(sizeof(OrderedRecord1051B14) == 0x7C);

extern std::array<OrderedRecord1051B14, 200> gOrderedRecords1051B14;
extern std::uint32_t gOrderMode1057C78; // 52F910 写入，52FB60 每次比较时读取。
extern std::int32_t gOrderedRecordCount1051510; // 50EEB0 写计数，532D80 清零。

// 公会资料与同源文本表；socket未用，不复制成员/邮件/窗口owner。
void Sub_50E850(std::uintptr_t unusedSocket, const char* text); // 完整掩码字段消费，超长保留旧值。
void Sub_50EB40(std::uintptr_t unusedSocket, const char* text); // 三字段批量更新，-1只清两字段。
void Sub_50F280(std::uintptr_t unusedSocket, std::int32_t key); // 编号相等才重置、清邮件/保存并请求关窗。
std::int32_t Sub_532D80(); // 登录窄重置，不清邮件；返回窗口48关闭请求的音效结果。

// 原 52FB60：qsort 的两个 W32 索引参数；相等时仍返回 -1。
int CompareOrderedRecords_52FB60(const void* left, const void* right);
// 原 52FCC0：原位填 0..199 后用上述比较器重排共享运行表。
void BuildOrderedRecordIndices_52FCC0();
// 原 50EF50：逐条统计 +8 W16 恰为 1 的记录，返回 0..200。
std::int32_t CountRecordsValue08EqualOne_50EF50() noexcept;
// 原 50EF20：仅重置已证字段与四个文本首字节，其他字节保留。
OrderedRecord1051B14* ResetOrderedRecord_50EF20(
    OrderedRecord1051B14* record) noexcept;
// 原 50EF70：按 | 字段解析、反转义并写入指定记录，返回 0/1。
std::int32_t ParseOrderedRecord_50EF70(std::int32_t index, const char* text);
// 原 50EEB0：前两项 W32 不读取；EAX 可为门禁值、计数或满表末地址。
std::uintptr_t UpdateOrderedRecords_50EEB0(
    std::uint32_t ignored0, std::uint32_t ignored1, const char* text);
// 原 50EC50：重置并按序解析最多 200 组，重算计数后清理所选角色的失配公会邮件。
// 返回门禁 W32、零或表末 key 游标；第一参数不读取，网络派发尚待接入。
std::uintptr_t UpdateOrderedRecordBatch_50EC50(
    std::uintptr_t ignoredSocket, const char* text);
// 原 5195C0：仅查找 +8 W16 为 1 且 +0xC 与 key 相等的首条索引。
std::int32_t FindActiveOrderedRecordIndex_5195C0(std::int32_t key) noexcept;

} // namespace cg::login::detail
