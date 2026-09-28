#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::world::detail {

// 原F19FB0的74H联系人记录，三路回复生产，普通邮件与窗口共用。
struct ContactRuntimeRecord {
    std::uint16_t used;
    std::int16_t value02; // 排序按S16比较，回复保留低W16位型。
    std::int32_t value04;
    std::int32_t value08;
    std::int32_t value0C;
    std::array<char, 17> name10;
    std::array<char, 17> text21;
    std::array<char, 33> text32;
    std::array<char, 33> text53;
};

static_assert(std::is_standard_layout_v<ContactRuntimeRecord>);
static_assert(std::is_trivially_copyable_v<ContactRuntimeRecord>);
static_assert(offsetof(ContactRuntimeRecord, used) == 0);
static_assert(offsetof(ContactRuntimeRecord, value02) == 2);
static_assert(offsetof(ContactRuntimeRecord, value04) == 4);
static_assert(offsetof(ContactRuntimeRecord, value08) == 8);
static_assert(offsetof(ContactRuntimeRecord, value0C) == 0x0C);
static_assert(offsetof(ContactRuntimeRecord, name10) == 0x10);
static_assert(offsetof(ContactRuntimeRecord, text21) == 0x21);
static_assert(offsetof(ContactRuntimeRecord, text32) == 0x32);
static_assert(offsetof(ContactRuntimeRecord, text53) == 0x53);
static_assert(sizeof(ContactRuntimeRecord) == 0x74);

extern std::array<ContactRuntimeRecord, 60> gContactsF19FB0;
extern std::array<std::int32_t, 60> gContactIndicesFDA608; // 数值索引，不是地址。
void Sub_4EC1A0(); // 原序生成/排序60个索引，比较器相等仍为-1。
void Sub_50D740(std::uintptr_t unusedSocket, const char* payload); // 基础批量回复，无排序尾。
void Sub_50D8C0(std::uintptr_t unusedSocket, const char* payload); // 长文本批量回复，提前结束也排序。
void Sub_50D9E0(
    std::uintptr_t unusedSocket, std::int32_t index, const char* payload); // 单槽回复，可清同源普通邮件。

// 原 518C80：依次清两段邮件持久区及 CB1384，返回 0。
std::int32_t Sub_518C80();
// 原 [518CB0,518D94)：依次读两文件，返回 fclose 或缺失文件 helper 的整数结果。
std::int32_t Sub_518CB0();
// 原 [519300,51936C) 与 [519550,5195BC)：各自重取保存路径，写唯一共享缓冲。
std::int32_t Sub_519300();
std::int32_t Sub_519550();

// 原 519290/5194C0：按同一 S16 所选角色清第一/第二邮件组，不保存文件，返回 0。
std::int32_t Sub_519290();
std::int32_t Sub_5194C0();
// 原 5192C0：清所选角色的一个普通邮件编号，不检查编号范围，返回 0。
std::int32_t Sub_5192C0(std::int32_t number);
// 原 5195F0：扫描全部 200 项，返回 active==1 且 key 相等的最后索引，未命中为 -1。
std::int32_t Sub_5195F0(std::int32_t key);
// 原 5194F0：查询后重新读取角色号，清该项；未命中返回 -1，命中返回 0。
std::int32_t Sub_5194F0(std::int32_t key);
// 原 519370：混合返回 W32 FFFFFFFF、含 NUL 长度或满组末地址+4；不截断地址域。
std::uintptr_t Sub_519370(
    std::int32_t key, std::int32_t kind, const char* name, const char* text);

// 原 50EC50 的 50EE4A–50EEA1 公共尾：全 key 比较，不检查活动位、不保存文件。
// selected 由父入口在计数后、计数发布前读取；返回最后邮件决定的零或表末游标。
std::uintptr_t PruneGuildMailAgainstOrderedRecords_50EE4A(std::int16_t selected);

} // namespace cg::world::detail
