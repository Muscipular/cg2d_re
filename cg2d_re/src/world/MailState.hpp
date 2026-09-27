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

} // namespace cg::world::detail
