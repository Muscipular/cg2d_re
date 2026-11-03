#pragma once

struct lua_State;

namespace cg::script {

// 两套 Lua VM 及其关闭门禁属于同一账号会话，关闭前由脚本模块按原顺序释放。
struct LuaSessionState {
    lua_State* uiState = nullptr;
    lua_State* battleState = nullptr;
    bool shuttingDown = false;
};

} // namespace cg::script
