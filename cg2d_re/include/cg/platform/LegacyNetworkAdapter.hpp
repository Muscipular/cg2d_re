#pragma once

#include <cstdint>

namespace cg::platform {

// 原 sub_534EF0@[0x534EF0,0x535085)：枚举首个合格适配器并写 12 字节标识。
std::int32_t GetMacAddress_534EF0(char* output);

} // namespace cg::platform
