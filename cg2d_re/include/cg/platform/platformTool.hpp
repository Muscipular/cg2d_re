#pragma once
#include <string>

#include <cstdint>

namespace cg::platform {

// 原 sub_534EF0@[0x534EF0,0x535085)：枚举首个合格适配器并写入12位十六进制宽文本标识。
std::int32_t GetMacAddress_534EF0(std::wstring& output);

// SDP01：映射地址并发起三路后台校验后立即返回；任一路通过即可，全部失败退出进程。
void PrepareServerAddressPolicy(std::wstring& serverAddress, std::uint16_t& port);

} // namespace cg::platform
