#pragma once
#include <string>
#include <string_view>

#include <cstdint>

namespace cg::platform {

// 原 sub_534EF0@[0x534EF0,0x535085)：枚举首个合格适配器并写入12位十六进制宽文本标识。
std::int32_t GetMacAddress_534EF0(std::wstring& output);

// NTM01/A10：主线程先映射地址并生成策略查询名，后台查询后在主线程校验结果。
std::wstring PrepareServerAddressPolicy(std::wstring& serverAddress, std::uint16_t& port);
[[nodiscard]] bool ApplyServerAddressPolicyResult(std::string_view address);

} // namespace cg::platform
