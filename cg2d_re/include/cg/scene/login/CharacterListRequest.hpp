#pragma once

namespace cg::network { class CgConnection; }

#include <cstdint>

namespace cg::login {

// 原 0xDA3B26，角色列表异步响应写入的 W16 回复码，映像初值为 0。
extern std::uint16_t gCharacterListReplyCode;

// 原 sub_508910@[0x508910,0x508950)；清空两条记录并发起角色列表请求。
std::uint32_t StartCharacterListRequest();

// 原 sub_508950@[0x508950,0x5089B0)；轮询回复或 600000 ms 超时。
std::int32_t PollCharacterListRequest();

// 原 sub_5089B0@[0x5089B0,0x508AE9)；发布角色列表响应并固定处理两组记录。
std::int32_t HandleCharacterListResponse_5089B0(cg::network::CgConnection& networkConnection,
    std::int32_t result,
    const char* data);

} // namespace cg::login
