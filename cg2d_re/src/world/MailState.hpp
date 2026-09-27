#pragma once

#include <cstdint>

namespace cg::world::detail {

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

} // namespace cg::world::detail
