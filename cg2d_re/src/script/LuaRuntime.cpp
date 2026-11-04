#include "cg/script/LuaRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "LuaUI.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/core/m_log.hpp"
#include "cg/audio/FileBgm.hpp"
#include <luajit.h>
#include <stdexcept>

namespace cg::script {

static std::uint8_t connectionRegistryKey;

lua_State* GetLuaState(session::ClientSession& session) { return session.Lua().uiState; }
lua_State* GetBattleLuaState(session::ClientSession& session) { return session.Lua().battleState; }
bool IsLuaShuttingDown(lua_State* L) { return GetLuaConnection(L).Owner().Lua().shuttingDown; }

network::CgConnection& GetLuaConnection(lua_State* L)
{
    lua_pushlightuserdata(L, &connectionRegistryKey);
    lua_rawget(L, LUA_REGISTRYINDEX);
    auto* const connection = static_cast<network::CgConnection*>(lua_touserdata(L, -1));
    lua_pop(L, 1);
    if (connection == nullptr) {
        throw std::runtime_error("Lua state has no client session");
    }
    return *connection;
}

static lua_State* CreateState(session::ClientSession& session)
{
    lua_State* const L = luaL_newstate();
    if (!L) { m_log_iw(L"[Lua] cannot create state\n"); return nullptr; }
    lua_pushlightuserdata(L, &connectionRegistryKey);
    lua_pushlightuserdata(L, &session.Connection());
    lua_rawset(L, LUA_REGISTRYINDEX);
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

bool InitializeLua(session::ClientSession& session)
{
    auto& runtime = session.Lua();
    if (runtime.uiState && runtime.battleState) { return true; }
    runtime.shuttingDown = false;
    LUAJIT_VERSION_SYM();
    runtime.uiState = CreateState(session);
    runtime.battleState = CreateState(session);
    if (!runtime.uiState || !runtime.battleState) { ShutdownLua(session); return false; }
    yobi::InitializeBattleEffects(session);
    RegisterLuaUI(runtime.uiState);
    RegisterLuaUI(runtime.battleState);
    InitializeAutoBattle(session);
    LoadLuaUIEntry(session);
    return true;
}

void ShutdownLua(session::ClientSession& session)
{
    auto& runtime = session.Lua();
    runtime.shuttingDown = true;
    audio::ShutdownFileBgm(session);
    yobi::ShutdownBattleEffects(session);
    stopCopilot(session);
    ShutdownAutoBattle(session);
    ShutdownLuaUI(session);
    session.UiFrame().lastScene = session.UiFrame().lastSceneState = -1;
    if (runtime.battleState) { lua_close(runtime.battleState); runtime.battleState = nullptr; }
    if (runtime.uiState) { lua_close(runtime.uiState); runtime.uiState = nullptr; }
}

} // namespace cg::script
