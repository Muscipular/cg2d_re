#pragma once

#include "cg/world/GuildState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/Yobi/YobiRuntime.hpp"

#include <cstdint>

namespace cg::login::detail {

// NLD01／原507D90：失败-1不写输出；成功复制地址／写U16端口后应用A10映射或DNS／提示策略。
std::int32_t GetServerAddressInfo_507D90(
    std::int32_t serverIndex,
    char* serverAddress,
    std::uint16_t* port);
// 原函数：0x0044B5C0。
std::uint32_t Sub_44B5C0(const char* text);
// 原函数：0x00508360。
std::int32_t Sub_508360();

} // namespace cg::login::detail
