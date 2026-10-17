#pragma once

#include <cstdint>
#include <string_view>

namespace cg::script {

// LUM01：仅主线程调用；在资源/UI准备后初始化，在资源销毁前关闭。
bool InitializeLua();
void ShutdownLua();
void UpdateLuaScene();
void AddLuaMouseWheel(int delta);
int LuaProcessWinMgrChatMessage(const char* text);
void LuaReceivePacket(const char* header, char** arguments);
void LuaSendPacket(std::string_view packet);
void RecordAutoBattleAction(std::string_view packet);

} // namespace cg::script
