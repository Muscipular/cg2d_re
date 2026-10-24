#pragma once
#include <string>
#include <string_view>

#include <cstdint>

namespace cg::platform {

// 原 sub_534EF0@[0x534EF0,0x535085)：枚举首个合格适配器并写入12位十六进制宽文本标识。
std::int32_t GetMacAddress_534EF0(std::wstring& output);

// SDP01：主线程映射地址并生成策略查询名；后台三路查询共用同一校验规则。
std::wstring PrepareServerAddressPolicy(std::wstring& serverAddress, std::uint16_t& port);
[[nodiscard]] bool IsServerAddressPolicyAccepted(std::string_view address);

} // namespace cg::platform
