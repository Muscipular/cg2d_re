#pragma once

namespace cg::network { class CgConnection; }

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace cg::world {

// 原 [1051B14,1057BF4)：解析、计数及排序共用的 200 条 0x7C 字节运行记录。
struct OrderedRecord1051B14 final {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::uint16_t value08; // 原成员偏移：+0x08。
    std::int16_t value0A; // 原成员偏移：+0x0A。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。
    std::int32_t value14; // 原成员偏移：+0x14。
    std::wstring name18; // 原+18：成员姓名，最多16个GBK字节。
    std::wstring title29; // 原+29：成员称号，最多16个GBK字节。
    std::wstring text3A; // 原+3A：第一附文。
    std::wstring text5B; // 原+5B：第二附文。
};
static_assert(offsetof(OrderedRecord1051B14, value04) == 0x04);
static_assert(offsetof(OrderedRecord1051B14, value08) == 0x08);
static_assert(offsetof(OrderedRecord1051B14, value0A) == 0x0A);
static_assert(offsetof(OrderedRecord1051B14, value0C) == 0x0C);
static_assert(offsetof(OrderedRecord1051B14, value10) == 0x10);
static_assert(offsetof(OrderedRecord1051B14, value14) == 0x14);

extern std::array<OrderedRecord1051B14, 200> gOrderedRecords1051B14; // 原全局：0x01051B14。
extern std::int32_t gOrderedRecordCount1051510; // 原全局：0x01051510。 50EEB0 写计数，532D80 清零。

// 公会资料与同源文本表；socket未用，不复制成员/邮件/窗口owner。
// 原函数：0x0050E850。
void HandleGuildInfoReply_50E850(cg::network::CgConnection& networkConnection, const char* text); // 完整掩码字段消费，超长保留旧值。
// 原函数：0x0050EB40。
void HandleGuildRankDefinitionsReply_50EB40(cg::network::CgConnection& networkConnection, const char* text); // 三字段批量更新，-1只清两字段。
// 原函数：0x0050F280。
void HandleGuildRemovalReply_50F280(cg::network::CgConnection& networkConnection, std::int32_t key); // 编号相等才重置、清邮件/保存并请求关窗。
// 原函数：0x00532D80。
std::int32_t ResetGuildStateAndCloseDetails_532D80(); // 登录窄重置，不清邮件；返回窗口48关闭请求的音效结果。
std::int32_t ReadGuildValue10514B4() noexcept; // 窗口51权限读取，共用回复写入的value0C。
std::int32_t ReadGuildTextIndex10514B0() noexcept; // 窗口7分支与索引各自读取value08。
std::int32_t ReadGuildRankFlags1051514(std::int32_t index) noexcept; // 窗口21权限，借同源职级记录value00。
std::uint8_t ReadGuildRankFlagsByte1051514(std::int32_t index) noexcept; // 窗口21三行按钮只读权限首字节。
std::int32_t ReadGuildStatus10514AC() noexcept; // 窗口21公会状态，借header.value04。
std::int32_t ReadGuildOwnKey10514B8() noexcept; // 窗口21成员本人判断，借header.value10。
const wchar_t* GetGuildName() noexcept; // 同源texts14[0]，宽文本借用。
const wchar_t* GetGuildSecondText() noexcept; // 窗口48借用同一公会头的第二文本。
const wchar_t* GetGuildAuxText(std::int32_t index) noexcept; // 窗口48三条附加文本，步长29H。
const wchar_t* GetGuildRankText(std::int32_t index) noexcept; // 职级文本，无效索引返回空文本。

// GMR01：原50EF50，200条/+8 W16恰1/S32 count0..200，正文正确保持。
std::int32_t CountRecordsValue08EqualOne_50EF50() noexcept;
// 原 50EEB0：首参为未读 socket，次参为未读 W32；EAX 可为门禁值、计数或满表末地址。
std::uintptr_t UpdateOrderedRecords_50EEB0(cg::network::CgConnection& networkConnection, std::uint32_t ignored1, const char* text);
// 原 50EC50：重置并按序解析最多 200 组，重算计数后清理所选角色的失配公会邮件。
// 返回门禁W32、零或表末key游标；第一参数不读取，yuD派发已接，完整业务/运行未验。
std::uintptr_t UpdateOrderedRecordBatch_50EC50(cg::network::CgConnection& networkConnection, const char* text);
// GML01：原5195C0，200项/+8 W16==1/+C key首匹配，S32计数/返回，无则-1。
std::int32_t FindActiveOrderedRecordIndex_5195C0(std::int32_t key) noexcept;

} // namespace cg::world
