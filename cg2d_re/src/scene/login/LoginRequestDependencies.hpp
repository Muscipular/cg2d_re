#pragma once

namespace cg::network { class CgConnection; }

#include <cstdint>

namespace cg::login::detail {

// 原508C40、508C90：登出请求启动及回复/超时轮询。
std::uint32_t BeginLogoutRequest_508C40();
std::int32_t PollLogoutReply_508C90();


// 原 0xDA3B2E；与登录回复码 0xDA3B2C 分离，508CF0复用。
extern std::uint16_t gReplyCodeDA3B2E; // 原全局：0x00DA3B2E。

// 原508CF0仅读result；S32比较/成功/末次调用返回保留，网络分派弃其返回。
std::int32_t HandleLogoutReply_508CF0(cg::network::CgConnection& networkConnection, const char* result, const char* unusedText);

} // namespace cg::login::detail
