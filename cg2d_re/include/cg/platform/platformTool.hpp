#pragma once
#include <string>

#include <cstdint>

namespace cg::platform {

// 原 sub_534EF0@[0x534EF0,0x535085)：枚举首个合格适配器并写入12位十六进制宽文本标识。
std::int32_t GetMacAddress_534EF0(std::wstring& output);

// A10：507D90成功查询后调用；映射taomoli，其余地址执行DNS校验及相应退出。
void ApplyServerAddressPolicy(std::wstring& serverAddress, std::uint16_t& port);

} // namespace cg::platform
