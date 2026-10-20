#pragma once
#include <string_view>

#include "cg/world/GuildState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/Yobi/YobiRuntime.hpp"

#include <cstdint>

namespace cg::login::detail {

// NLD01/NTM01：失败-1不写输出；成功复制地址/U16端口，策略由异步连接适配接续。
std::int32_t GetServerAddressInfo_507D90(
    std::int32_t serverIndex,
    std::wstring& serverAddress,
    std::uint16_t* port);
// 原函数：0x0044B5C0。
std::uint32_t AppendSystemLog_44B5C0(std::wstring_view text);
// 原函数：0x00508360。
std::int32_t AdvanceServerLoginConnection_508360();

} // namespace cg::login::detail
