#include "cg/script/LuaRuntime.hpp"
#include "LuaUI.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/core/m_log.hpp"
#include "cg/audio/FileBgm.hpp"
#include <luajit.h>

namespace cg::script {

static lua_State* uiState = nullptr;
static lua_State* battleState = nullptr;
static bool shuttingDown = false;

lua_State* GetLuaState() { return uiState; }
lua_State* GetBattleLuaState() { return battleState; }
bool IsLuaShuttingDown() { return shuttingDown; }

static lua_State* CreateState()
{
    lua_State* const L = luaL_newstate();
    if (!L) { m_log_iw(L"[Lua] cannot create state\n"); return nullptr; }
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
    shuttingDown = false;
    LUAJIT_VERSION_SYM();
    uiState = CreateState();
    battleState = CreateState();
    if (!uiState || !battleState) { ShutdownLua(); return false; }
    yobi::InitializeBattleEffects();
    RegisterLuaUI(uiState);
    RegisterLuaUI(battleState);
    InitializeAutoBattle();
    LoadLuaUIEntry();
    return true;
}

void ShutdownLua()
{
    shuttingDown = true;
    audio::ShutdownFileBgm();
    yobi::ShutdownBattleEffects();
    stopCopilot();
    ShutdownAutoBattle();
    ShutdownLuaUI();
    if (battleState) { lua_close(battleState); battleState = nullptr; }
    if (uiState) { lua_close(uiState); uiState = nullptr; }
}

} // namespace cg::script
