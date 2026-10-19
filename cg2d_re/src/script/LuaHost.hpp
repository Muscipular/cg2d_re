#pragma once
#include "LuaInternal.hpp"
#include "../ui/WindowComponentHost.hpp"

namespace cg::script {
using namespace cg::ui::component_detail;
using namespace cg::ui; // 本地桥接名，实际资源及组件由UI拥有。
#define LuaFunction(A) int A(lua_State* L)
#define LuaFunctionEx(P, A) int Lua_##P##_##A(lua_State* L)
#define LuaCFunction(P, A) SafeLuaCall<Lua_##P##_##A>
#define CName(T) #T
#define LuaPush Push

} // namespace cg::script
