#pragma once
#include <string>

#include <cstdint>

namespace cg::platform {

// 原 sub_534EF0@[0x534EF0,0x535085)：枚举首个合格适配器并写入12位十六进制宽文本标识。
std::int32_t GetMacAddress_534EF0(std::wstring& output);

// SDP01：在本次调用内完成地址映射、三路DNS/DoH请求及校验，通过后才允许开始连接。
[[nodiscard]] bool PrepareServerAddressPolicy(std::wstring& serverAddress, std::uint16_t& port);

} // namespace cg::platform
