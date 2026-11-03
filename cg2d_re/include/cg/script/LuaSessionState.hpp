#pragma once

#include <memory>

struct lua_State;

namespace cg::script {

struct LuaPacketBridgeState;

// 两套 Lua VM 及其关闭门禁属于同一账号会话，关闭前由脚本模块按原顺序释放。
struct LuaSessionState {
    lua_State* uiState = nullptr;
    lua_State* battleState = nullptr;
    bool shuttingDown = false;
    std::shared_ptr<LuaPacketBridgeState> packetBridge;
};

} // namespace cg::script
