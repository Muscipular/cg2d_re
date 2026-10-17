#pragma once

#include <lua.hpp>
#include <array>
#include <concepts>
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/text/Encoding.hpp"

namespace cg::script {

lua_State* GetLuaState();
lua_State* GetBattleLuaState();
void RegisterLuaData(lua_State* state);
void PushPlayer(lua_State* state);
void PushBattleEntry(lua_State* state, int slot);

// C++文本/分配异常在C API边界转换；离开catch后再触发Lua错误跳转。
template<lua_CFunction Function> int SafeLuaCall(lua_State* L) {
    try { return Function(L); }
    catch (const std::exception& error) { lua_pushstring(L, error.what()); }
    return lua_error(L);
}

// Lua字符串沿既有GBK脚本/协议合同；只在宽文本边界转换。
inline void Push(lua_State* L, std::string_view value) { lua_pushlstring(L, value.data(), value.size()); }
inline void Push(lua_State* L, const std::string& value) { Push(L, std::string_view(value)); }
inline void Push(lua_State* L, const char* value) { lua_pushstring(L, value); }
inline void Push(lua_State* L, std::wstring_view value) { Push(L, text::ToGbk(value)); }
inline void Push(lua_State* L, const std::wstring& value) { Push(L, std::wstring_view(value)); }
inline void Push(lua_State* L, bool value) { lua_pushboolean(L, value); }
inline void Push(lua_State* L, platform::LegacyCryptInt& value) { lua_pushinteger(L, value.value()); }
template<std::integral T> void Push(lua_State* L, T value) { lua_pushinteger(L, static_cast<lua_Integer>(value)); }
template<std::floating_point T> void Push(lua_State* L, T value) { lua_pushnumber(L, value); }
template<std::size_t N> void Push(lua_State* L, const std::array<char, N>& value) { lua_pushstring(L, value.data()); }
template<class T, std::size_t N> void Push(lua_State* L, const std::array<T, N>& value) {
    lua_createtable(L, static_cast<int>(N), 0);
    for (std::size_t i = 0; i < N; ++i) { Push(L, value[i]); lua_rawseti(L, -2, static_cast<int>(i + 1)); }
}
template<class T> void Field(lua_State* L, const char* key, T&& value) { Push(L, value); lua_setfield(L, -2, key); }

} // namespace cg::script
