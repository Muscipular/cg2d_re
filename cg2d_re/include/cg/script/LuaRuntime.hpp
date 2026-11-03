#pragma once

namespace cg::network { class CgConnection; }
namespace cg::session { class ClientSession; }
#include <string_view>

#include <cstdint>
#include <string_view>

namespace cg::script {

// LUM01：仅主线程调用；在资源/UI准备后初始化，在资源销毁前关闭。
bool InitializeLua(session::ClientSession& session);
void ShutdownLua(session::ClientSession& session);
void UpdateLuaScene(session::ClientSession& session);
void AddLuaMouseWheel(int delta);
int LuaProcessWinMgrChatMessage(std::wstring_view text);
void LuaReceivePacket(cg::network::CgConnection& networkConnection, const char* header, char** arguments);
void LuaSendPacket(cg::network::CgConnection& networkConnection, std::string_view packet);
void RecordAutoBattleAction(cg::network::CgConnection& networkConnection, std::string_view packet);

} // namespace cg::script
