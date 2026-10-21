#include "LuaUI.hpp"

namespace cg::script {
static constexpr int LUA_REF_NONE = -1;
static void PushControlRef(lua_State* L, LuaControlRef ref) {
    auto data = static_cast<LuaControlRef*>(lua_newuserdata(L, sizeof(LuaControlRef)));
    *data = ref;
    luaL_setmetatable(L, CName(LuaControlRef));
}

static int CallControlTextCallback(lua_State* L, int callbackRef, const char* name, LuaControlRef controlRef, std::wstring_view value) {
    if (!IsLuaControlAttached(controlRef)) return 0;
    if (callbackRef == LUA_REF_NONE) {
        return 0;
    }
    int top = lua_gettop(L);
    int result = 0;
    lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
    if (lua_isfunction(L, -1)) {
        PushControlRef(L, controlRef);
        Push(L, value);
        if (lua_pcall(L, 2, 1, 0) != 0) {
            m_log_iw(L"[LUA] call lua %hs error: %hs\n", name, lua_tostring(L, -1));
        } else if (!lua_isnil(L, -1)) {
            result = lua_toboolean(L, -1) ? 1 : 0;
        }
    }
    lua_settop(L, top);
    return result;
}


void AttachLuaElementRefs(LuaCallbackRefs& refs, ui::WindowElement& element) {
    element.refUpdate = refs.onUpdate; refs.onUpdate = -1;
    element.refDraw = refs.onDraw; refs.onDraw = -1;
    element.refEvent = refs.onEvent; refs.onEvent = -1;
    element.refClick = refs.onClick; refs.onClick = -1;
    element.refPress = refs.onPress; refs.onPress = -1;
    element.refHover = refs.onHover; refs.onHover = -1;
    element.refLeave = refs.onLeave; refs.onLeave = -1;
    element.refDrag = refs.onDrag; refs.onDrag = -1;
    element.refDrop = refs.onDrop; refs.onDrop = -1;
    element.refChange = refs.onChange; refs.onChange = -1;
    element.refEnter = refs.onEnter; refs.onEnter = -1;
}
int LuaWindowBinding::elementText(ui::WindowElement& element, bool enter, std::wstring_view value) {
    if (element.removed || element.detaching || (window.state & WIN_STATE_CLOSE)) return 0;
    return CallControlTextCallback(owner, enter ? element.refEnter : element.refChange,
        enter ? "WindowElement.onEnter" : "WindowElement.onChange", {window.id, element.elementId}, value);
}
} // namespace cg::script
