#include "LuaInternal.hpp"
#include "cg/audio/FileBgm.hpp"

#include <new>

namespace cg::script {

static constexpr char kBgmHandleMetatable[] = "cg.BGMHandle";

// userdata仅保存稳定身份；track/音频缓存由audio服务拥有。
struct LuaBgmHandle { audio::FileBgmHandle id = 0; };

static int BgmStop(lua_State* L)
{
    const auto* handle = static_cast<LuaBgmHandle*>(luaL_checkudata(L, 1, kBgmHandleMetatable));
    audio::StopFileBgm(handle->id);
    lua_pushboolean(L, true);
    return 1;
}

static int BgmGetState(lua_State* L)
{
    const auto* handle = static_cast<LuaBgmHandle*>(luaL_checkudata(L, 1, kBgmHandleMetatable));
    const char* state = "stopped";
    switch (audio::GetFileBgmState(handle->id)) {
    case audio::FileBgmState::Playing: state = "playing"; break;
    case audio::FileBgmState::Stopped: state = "stopped"; break;
    case audio::FileBgmState::Finished: state = "finished"; break;
    case audio::FileBgmState::Replaced: state = "replaced"; break;
    }
    lua_pushstring(L, state);
    return 1;
}

// lua_close期间仍可执行；清理幂等，不走SafeLuaCall的关闭门禁。
static int BgmGc(lua_State* L)
{
    auto* handle = static_cast<LuaBgmHandle*>(luaL_checkudata(L, 1, kBgmHandleMetatable));
    audio::ForgetFileBgm(handle->id);
    handle->id = 0;
    return 0;
}

static int PlayBgmFile(lua_State* L)
{
    // 所有Lua参数错误先于C++资源创建，避免longjmp绕过析构。
    luaL_checktype(L, 1, LUA_TSTRING);
    std::size_t length = 0;
    const char* path = lua_tolstring(L, 1, &length);
    luaL_checktype(L, 2, LUA_TTABLE);
    lua_getfield(L, 2, "mode");
    if (lua_type(L, -1) != LUA_TSTRING) return luaL_argerror(L, 2, "mode must be a string");
    std::size_t modeLength = 0;
    const char* modeText = lua_tolstring(L, -1, &modeLength);
    audio::FileBgmMode mode;
    if (std::string_view(modeText, modeLength) == "replace") mode = audio::FileBgmMode::Replace;
    else if (std::string_view(modeText, modeLength) == "takeover") mode = audio::FileBgmMode::Takeover;
    else if (std::string_view(modeText, modeLength) == "independent") mode = audio::FileBgmMode::Independent;
    else return luaL_argerror(L, 2, "mode must be replace, takeover or independent");
    lua_pop(L, 1);
    lua_getfield(L, 2, "loop");
    if (!lua_isboolean(L, -1)) return luaL_argerror(L, 2, "loop is required and must be a boolean");
    const bool loop = lua_toboolean(L, -1) != 0;
    lua_pop(L, 1);

    // 先创建带GC的空身份，再启动；之后的Lua内存错误也不会遗留无主播放。
    auto* handle = new (lua_newuserdata(L, sizeof(LuaBgmHandle))) LuaBgmHandle;
    luaL_getmetatable(L, kBgmHandleMetatable);
    lua_setmetatable(L, -2);
    audio::FileBgmResult result;
    try {
        result = audio::PlayFileBgm(text::FromGbkWide(std::string_view(path, length)), mode, loop,
            L == GetLuaState(GetLuaConnection(L).Owner()) ? audio::FileBgmOwner::Ui : audio::FileBgmOwner::Battle);
    } catch (const std::exception&) {
        result.error = "Cannot prepare BGM file (path conversion or allocation failed)";
    }
    handle->id = result.handle;
    if (!result.handle) {
        lua_pop(L, 1);
        lua_pushnil(L);
        Push(L, text::FromUtf8Wide(result.error));
        return 2;
    }
    lua_pushnil(L);
    return 2;
}

void RegLuaAudio(lua_State* L)
{
    luaL_newmetatable(L, kBgmHandleMetatable);
    lua_pushcfunction(L, BgmGc);
    lua_setfield(L, -2, "__gc");
    lua_pushliteral(L, "BGMHandle");
    lua_setfield(L, -2, "__metatable");
    lua_newtable(L);
    lua_pushcfunction(L, SafeLuaCall<BgmStop>);
    lua_setfield(L, -2, "Stop");
    lua_pushcfunction(L, SafeLuaCall<BgmGetState>);
    lua_setfield(L, -2, "GetState");
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
    lua_getglobal(L, "WinMgr");
    lua_pushcfunction(L, SafeLuaCall<PlayBgmFile>);
    lua_setfield(L, -2, "PlayBGMFile");
    lua_pop(L, 1);
}

} // namespace cg::script
