#pragma once

#include <cstdint>
#include <string_view>

namespace cg::login::detail {

// 原 sub_464440@[0x464440,0x46444B)：复位能力 4 删除确认模态初始化锁。
void ResetCapability4Modal_464440();

// 原 sub_464450@[0x464450,0x464831)：更新能力 4 删除确认模态。
std::int32_t UpdateCapability4Modal_464450(std::wstring_view text);

} // namespace cg::login::detail
