#pragma once

namespace cg::session { class ClientSession; }

namespace cg::platform {

// 原 sub_457A40，包含共享尾块 [0x465A60, 0x465A7D)。
int ShutdownClient_457A40(session::ClientSession& session);

} // namespace cg::platform
