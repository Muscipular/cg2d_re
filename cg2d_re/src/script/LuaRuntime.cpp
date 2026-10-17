#include "cg/script/LuaRuntime.hpp"
#include "LuaInternal.hpp"
#include "cg/core/m_log.hpp"
#include <luajit.h>

namespace cg::script {

static lua_State* uiState = nullptr;
static lua_State* battleState = nullptr;

lua_State* GetLuaState() { return uiState; }
lua_State* GetBattleLuaState() { return battleState; }

static lua_State* CreateState()
{
    lua_State* const L = luaL_newstate();
    if (!L) { m_log_i("[Lua] cannot create state\n"); return nullptr; }
    // 参考仅开放这五组库；不额外开放os/io/ffi或修改脚本权限。
    luaopen_base(L);
    luaopen_bit(L);
    luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    lua_settop(L, 0);
    RegisterLuaData(L);
    return L;
}

bool InitializeLua()
{
    if (uiState && battleState) { return true; }
    LUAJIT_VERSION_SYM();
    uiState = CreateState();
    battleState = CreateState();
    if (!uiState || !battleState) { ShutdownLua(); return false; }
    return true;
}

void ShutdownLua()
{
    if (battleState) { lua_close(battleState); battleState = nullptr; }
    if (uiState) { lua_close(uiState); uiState = nullptr; }
}

} // namespace cg::script
