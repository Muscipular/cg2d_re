#pragma once
#include <string>

#include <cstdint>

namespace cg::platform {

// 原 sub_534EF0@[0x534EF0,0x535085)：枚举首个合格适配器并写 12 字节标识及NUL，output至少13字节。
std::int32_t GetMacAddress_534EF0(char* output);

// A10：507D90成功查询后调用；映射taomoli，其余地址执行DNS校验及相应退出。
void ApplyServerAddressPolicy(std::wstring& serverAddress, std::uint16_t& port);

} // namespace cg::platform
