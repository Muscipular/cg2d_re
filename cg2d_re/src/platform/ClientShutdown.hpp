#pragma once

namespace cg::session { class ClientSession; }

namespace cg::platform {

// NSM01：所属会话先停止/保存/释放；初始化未完成时不写存档。
void ShutdownSessionRuntime(session::ClientSession& session, bool save);
// 原 sub_457A40，逐项关闭后才释放共享资源，保留原系统颜色恢复尾。
int ShutdownClient_457A40();

} // namespace cg::platform
