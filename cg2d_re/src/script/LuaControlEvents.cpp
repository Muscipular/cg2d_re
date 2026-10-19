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


struct LuaElementEvents final : ui::WindowElementEvents {
    LuaCallbackRefs pending; // 仅创建至attach期间暂存；活动ref全部在WindowElement上。
    explicit LuaElementEvents(LuaCallbackRefs value) : pending(value) {}
    ~LuaElementEvents() override { pending.clear(pending.owner); }
    void attach(ui::WindowElement& element) override {
        if (pending.onUpdate >= 0) { element.refUpdate = pending.onUpdate; pending.onUpdate = -1; }
        if (pending.onDraw >= 0) { element.refDraw = pending.onDraw; pending.onDraw = -1; }
        if (pending.onEvent >= 0) { element.refEvent = pending.onEvent; pending.onEvent = -1; }
        if (pending.onClick >= 0) { element.refClick = pending.onClick; pending.onClick = -1; }
        if (pending.onPress >= 0) { element.refPress = pending.onPress; pending.onPress = -1; }
        if (pending.onHover >= 0) { element.refHover = pending.onHover; pending.onHover = -1; }
        if (pending.onLeave >= 0) { element.refLeave = pending.onLeave; pending.onLeave = -1; }
        if (pending.onDrag >= 0) { element.refDrag = pending.onDrag; pending.onDrag = -1; }
        if (pending.onDrop >= 0) { element.refDrop = pending.onDrop; pending.onDrop = -1; }
        if (pending.onChange >= 0) { element.refChange = pending.onChange; pending.onChange = -1; }
        if (pending.onEnter >= 0) { element.refEnter = pending.onEnter; pending.onEnter = -1; }
    }
    void release(ui::WindowElement& element) override { ReleaseLuaElementRefs(pending.owner, element); }
    int mouse(ui::WindowControlRef ref, int flags) override {
        auto* control = ResolveControl({ref.winId, ref.generation, ref.controlId});
        if (!control) return 0;
        int result = CallLuaElementEvent(pending.owner, *control->windowOwner, control->element(), flags);
        auto* window = control->windowOwner;
        if (window->binding && control->attached && !(window->state & WIN_STATE_CLOSE)) {
            const int index = control->currentElement == &control->draft ? -1
                : static_cast<int>(&control->element() - window->elements);
            result |= window->binding->event(index, flags);
        }
        return result;
    }
    int text(ui::WindowControlRef ref, bool enter, std::wstring_view value) override {
        const LuaControlRef target{ref.winId, ref.generation, ref.controlId};
        auto* control = ResolveControl(target);
        if (!control) return 0;
        return CallControlTextCallback(pending.owner, enter ? control->element().refEnter : control->element().refChange,
            enter ? "UIControl.onEnter" : "UIControl.onChange", target, value);
    }
};
ui::WindowEventBinding MakeLuaElementEvents(LuaCallbackRefs refs) {
    return std::make_shared<LuaElementEvents>(refs);
}
} // namespace cg::script
