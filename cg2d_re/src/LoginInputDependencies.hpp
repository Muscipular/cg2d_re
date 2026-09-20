#pragma once

#include <cstdint>

namespace cg::login::detail {

// 原 sub_533680@0x533680：按选中槽读取第一项登录输入。
char* Sub_533680(char* output, std::int32_t selectedSlot);
// 原 sub_5335F0@0x5335F0：按选中槽读取第二项登录输入。
char* Sub_5335F0(char* output, std::int32_t selectedSlot);

// 原 dword_105C394@0x105C394：当前选中的持久槽索引。
extern std::int32_t gSelectedSavedSlot105C394;

} // namespace cg::login::detail
