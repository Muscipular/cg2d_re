#include "LuaUI.hpp"
#include <stdexcept>

namespace cg::script {

struct LuaElementsRef { LuaWindowRef window; };

static ui::WindowInstance* ResolveInstance(LuaWindowRef ref) {
    auto* window = ui::WindowRuntime_FindInstance(ref.winId);
    return window && !(window->state & WIN_STATE_CLOSE) ? window : nullptr;
}
static int AbsIndex(lua_State* L, int index) { return index > 0 || index <= LUA_REGISTRYINDEX ? index : lua_gettop(L) + index + 1; }
static void Unref(lua_State* L, int& ref) {
    if (L && ref >= 0) luaL_unref(L, LUA_REGISTRYINDEX, ref);
    ref = -1;
}
static constexpr std::pair<const char*, int ui::WindowElement::*> elementRefs[] = {
    {"onUpdate", &ui::WindowElement::refUpdate}, {"onDraw", &ui::WindowElement::refDraw},
    {"onEvent", &ui::WindowElement::refEvent}, {"onClick", &ui::WindowElement::refClick},
    {"onPress", &ui::WindowElement::refPress}, {"onHover", &ui::WindowElement::refHover},
    {"onLeave", &ui::WindowElement::refLeave}, {"onDrag", &ui::WindowElement::refDrag},
    {"onDrop", &ui::WindowElement::refDrop}, {"onChange", &ui::WindowElement::refChange},
    {"onEnter", &ui::WindowElement::refEnter}
};
void ReleaseLuaElementRefs(lua_State* L, ui::WindowElement& element) {
    for (const auto& [name, member] : elementRefs) Unref(L, element.*member);
}
static int* ElementRef(ui::WindowElement& element, const char* key) {
    for (const auto& [name, member] : elementRefs) if (std::strcmp(key, name) == 0) return &(element.*member);
    return nullptr;
}
static int* WindowRef(ui::WindowInstance& window, const char* key) {
    if (!std::strcmp(key, "onUpdate") || !std::strcmp(key, "update")) return &window.refUpdate;
    if (!std::strcmp(key, "onDraw") || !std::strcmp(key, "draw")) return &window.refDraw;
    if (!std::strcmp(key, "onEvent")) return &window.refEvent;
    return nullptr;
}
LuaWindowBinding* EnsureLuaWindowBinding(lua_State* L, ui::WindowInstance& window) {
    ui::WindowRuntime_EnsureIdentity(window);
    if (!window.binding) {
        window.binding = new LuaWindowBinding(window, L);
    }
    auto* binding = static_cast<LuaWindowBinding*>(window.binding);
    if (binding->owner != L) throw std::runtime_error("window belongs to another Lua state");
    return binding;
}
static void SetRef(lua_State* L, int value, int& ref) {
    const int type = lua_type(L, value);
    if (type != LUA_TFUNCTION && type != LUA_TNIL) throw std::runtime_error("callback must be a function or nil");
    lua_pushvalue(L, value);
    const int next = luaL_ref(L, LUA_REGISTRYINDEX);
    Unref(L, ref);
    ref = next;
}
static void PushRef(lua_State* L, int ref) { if (ref < 0) lua_pushnil(L); else lua_rawgeti(L, LUA_REGISTRYINDEX, ref); }
static std::int32_t ReadInteger(lua_State* L, int index) {
    if (lua_type(L, index) != LUA_TNUMBER) throw std::runtime_error("integer expected");
    const auto value = lua_tonumber(L, index);
    if (!std::isfinite(value) || std::trunc(value) != value
        || value < std::numeric_limits<std::int32_t>::min() || value > std::numeric_limits<std::int32_t>::max())
        throw std::runtime_error("integer must be in int32 range");
    return static_cast<std::int32_t>(value);
}
ui::WindowTextAlign LuaReadTextAlign(lua_State* L, int value) {
    const auto align = ReadInteger(L, value);
    if (align < 0 || align > 2) throw std::runtime_error("align must be 0 (left), 1 (center), or 2 (right)");
    return static_cast<ui::WindowTextAlign>(align);
}
static bool PushProperty(lua_State* L, const ui::WindowPropertyMap* properties, const char* key) {
    if (!properties) return false;
    const auto it = properties->find(key);
    if (it == properties->end()) return false;
    std::visit([&](const auto& value) { Push(L, value); }, it->second);
    return true;
}
static void SetProperty(lua_State* L, ui::WindowPropertyMap*& properties, const char* key, int index) {
    if (!std::strncmp(key, "ref", 3) || !std::strcmp(key, "data") || !std::strcmp(key, "state")
        || !std::strcmp(key, "type") || !std::strcmp(key, "id") || !std::strcmp(key, "winId")
        || !std::strcmp(key, "valid") || !std::strcmp(key, "count") || !std::strcmp(key, "elements")
        || !std::strcmp(key, "parentId") || !std::strcmp(key, "controlId") || !std::strcmp(key, "elementId") || !std::strcmp(key, "mouseState")
        || !std::strcmp(key, "mapWidth") || !std::strcmp(key, "mapHeight") || !std::strcmp(key, "maxScrollY")
        || !std::strcmp(key, "mouseX") || !std::strcmp(key, "mouseY") || !std::strcmp(key, "clickX")
        || !std::strcmp(key, "clickY") || !std::strcmp(key, "lastClickX") || !std::strcmp(key, "lastClickY"))
        throw std::runtime_error("property is read-only");
    std::variant<std::string, std::int32_t> value;
    if (lua_type(L, index) == LUA_TSTRING) {
        std::size_t size = 0; const auto* bytes = lua_tolstring(L, index, &size);
        value = std::string(bytes, size);
    } else value = ReadInteger(L, index); // 不接受nil、布尔及复杂对象。
    if (!properties) properties = new ui::WindowPropertyMap;
    properties->insert_or_assign(key, std::move(value));
}

void PushLuaElement(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element) {
    auto* ref = static_cast<LuaControlRef*>(lua_newuserdata(L, sizeof(LuaControlRef)));
    *ref = {window.id, element.elementId};
    luaL_setmetatable(L, "LuaControlRef");
}
static void PushElements(lua_State* L, ui::WindowInstance& window) {
    auto* ref = static_cast<LuaElementsRef*>(lua_newuserdata(L, sizeof(LuaElementsRef)));
    ref->window = {window.id};
    luaL_setmetatable(L, "LuaElementsRef");
}
bool LuaPushElementProperty(lua_State* L, ui::WindowElement& element, const char* key) {
    if (!std::strcmp(key, "type")) { Push(L, static_cast<int>(element.type)); return true; }
    if (!std::strcmp(key, "elementId")) { Push(L, element.elementId); return true; }
    if (auto* ref = ElementRef(element, key)) { if (*ref >= 0 && element.owner) EnsureLuaWindowBinding(L, *element.owner); PushRef(L, *ref); return true; }
    if (!std::strcmp(key, "seqNo")) { Push(L, element.seqNo); return true; }
    if (!std::strcmp(key, "x")) { Push(L, element.x); return true; }
    if (!std::strcmp(key, "y")) { Push(L, element.y); return true; }
    if (!std::strcmp(key, "width")) { Push(L, element.width); return true; }
    if (!std::strcmp(key, "height")) { Push(L, element.height); return true; }
    if (!std::strcmp(key, "visible")) { Push(L, element.visible == 1); return true; }
    if (element.type == ui::WindowElementType_Text && element.data.text) {
        const auto& data = *element.data.text;
        if (!std::strcmp(key, "text")) { Push(L, data.text); return true; }
        if (!std::strcmp(key, "align")) { Push(L, static_cast<int>(data.align)); return true; }
        if (!std::strcmp(key, "font") || !std::strcmp(key, "fontType")) { Push(L, std::to_integer<uint8_t>(data.style.font)); return true; }
        if (!std::strcmp(key, "color")) { Push(L, std::to_integer<uint8_t>(data.style.color)); return true; }
    }
    if (element.type == ui::WindowElementType_Graphic && element.data.graphic) {
        if (!std::strcmp(key, "image")) { Push(L, element.data.graphic->imageNo); return true; }
        if (!std::strcmp(key, "color")) { Push(L, element.data.graphic->color); return true; }
        if (!std::strcmp(key, "imageHover")) { Push(L, element.data.graphic->imageHover); return true; }
        if (!std::strcmp(key, "imagePress")) { Push(L, element.data.graphic->imagePress); return true; }
    }
    if (element.type == ui::WindowElementType_Input && element.data.input && element.data.input->context) {
        const auto& context = *element.data.input->context;
        if (!std::strcmp(key, "text")) { Push(L, context.text); return true; }
        if (!std::strcmp(key, "font") || !std::strcmp(key, "fontType")) {
            Push(L, element.data.input->ownedInput ? element.data.input->ownedInput->font : context.field13C); return true;
        }
        if (!std::strcmp(key, "color")) {
            Push(L, element.data.input->ownedInput ? element.data.input->ownedInput->color : context.field140); return true;
        }
    }
    return PushProperty(L, element.properties, key);
}
bool LuaPushWindowProperty(lua_State* L, ui::WindowInstance& window, const char* key) {
    if (!std::strcmp(key, "category")) { Push(L, static_cast<int>(window.category)); return true; }
    if (auto* ref = WindowRef(window, key)) { if (*ref >= 0) EnsureLuaWindowBinding(L, window); PushRef(L, *ref); return true; }
    if (!std::strcmp(key, "elements")) { PushElements(L, window); return true; }
    return PushProperty(L, window.properties, key);
}

static bool SetElementCore(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element, const char* key, int value) {
    if (auto* ref = ElementRef(element, key)) {
        if (*ref >= 0 || !lua_isnil(L, value)) EnsureLuaWindowBinding(L, window); SetRef(L, value, *ref); return true;
    }
    if (element.type == ui::WindowElementType_Text && !std::strcmp(key, "align")) {
        auto& data = *element.data.text;
        if (data.layout != ui::WindowTextLayout::Box)
            throw std::runtime_error("align is only supported by text box layout");
        data.align = LuaReadTextAlign(L, value);
        return true;
    }
    if (!std::strcmp(key, "seqNo")) {
        const auto previous = element.seqNo;
        element.seqNo = ReadInteger(L, value);
        try { window.sortedMap.notifyUpdate(); }
        catch (...) { element.seqNo = previous; throw; }
        return true;
    }
    return false;
}
static bool Listed(const char* key, std::initializer_list<const char*> names) {
    for (auto* name : names) if (!std::strcmp(key, name)) return true;
    return false;
}
void LuaApplyElementExtras(lua_State* L, int table, ui::WindowElement& control) {
    table = AbsIndex(L, table);
    auto& window = *control.owner;
    auto& element = control;
    lua_pushnil(L);
    while (lua_next(L, table)) {
        if (lua_type(L, -2) != LUA_TSTRING) throw std::runtime_error("property name must be a string");
        const auto* key = lua_tostring(L, -2);
        if (!SetElementCore(L, window, element, key, -1)
            && !Listed(key, {"elementId","name","x","y","width","height","visible","hitable","parent","parentId","seqNo"})
            && !(control.type == ui::WindowElementType_Graphic && Listed(key, {"image","id","imageHover","imagePress","color"}))
            && !(control.type == ui::WindowElementType_Text && Listed(key, {"text","font","fontType","color"}))
            && !(control.type == ui::WindowElementType_Animation && control.data.animation && control.data.animation->options && Listed(key, {"animeNo","action","dir","revertPlay","revert"}))
            && !(control.type == ui::WindowElementType_Input && Listed(key, {"text","font","fontType","color"}))
            && !(control.type == ui::WindowElementType_Input && control.data.input && control.data.input->ownedInput && !std::strcmp(key, "maxLength"))
            && !(control.type == ui::WindowElementType_PngImage && Listed(key, {"image","imageHover","imagePress","imageRect","imageHoverRect","imagePressRect","color"}))
            && !(control.type == ui::WindowElementType_Canvas && Listed(key, {"shapes"}))
            && !(control.type == ui::WindowElementType_ScrollView && Listed(key, {"contentHeight","scrollY","barWidth","scrollStep"}))
            && !(control.type == ui::WindowElementType_MapEx && Listed(key, {"mapX","mapY","mapId","floor","w","h"})))
            SetProperty(L, element.properties, key, -1);
        lua_pop(L, 1);
    }
}
void LuaApplyWindowExtras(lua_State* L, int table, ui::WindowInstance& window) {
    table = AbsIndex(L, table);
    lua_pushnil(L);
    while (lua_next(L, table)) {
        if (lua_type(L, -2) != LUA_TSTRING) throw std::runtime_error("property name must be a string");
        const auto* key = lua_tostring(L, -2);
        if (auto* ref = WindowRef(window, key)) {
            if (*ref >= 0 || !lua_isnil(L, -1)) EnsureLuaWindowBinding(L, window); SetRef(L, -1, *ref);
        } else if (!std::strcmp(key, "category")) {
            window.category = static_cast<std::int8_t>(ReadInteger(L, -1));
        } else if (!Listed(key, {"id","x","y","width","height","layer","visible","dragMove"}))
            SetProperty(L, window.properties, key, -1);
        lua_pop(L, 1);
    }
}

static int ElementsIndex(lua_State* L) {
    const auto* ref = static_cast<LuaElementsRef*>(luaL_checkudata(L, 1, "LuaElementsRef"));
    auto* window = ResolveInstance(ref->window);
    const auto number = luaL_checknumber(L, 2);
    if (number >= 0 && number <= std::numeric_limits<ui::ElementId>::max() && std::trunc(number) == number && window) {
        if (auto* element = window->FindElementById(static_cast<ui::ElementId>(number))) { PushLuaElement(L, *window, *element); return 1; }
    }
    lua_pushnil(L); return 1;
}
static int ElementsLength(lua_State* L) {
    const auto* ref = static_cast<LuaElementsRef*>(luaL_checkudata(L, 1, "LuaElementsRef"));
    const auto* window = ResolveInstance(ref->window);
    Push(L, window ? static_cast<int>(window->sortedMap.size()) : 0); return 1;
}
int LuaElementRead(lua_State* L) {
    const auto* ref = static_cast<LuaControlRef*>(luaL_checkudata(L, 1, "LuaControlRef"));
    auto* element = ResolveLuaElement(*ref);
    const auto* key = luaL_checkstring(L, 2);
    if (!std::strcmp(key, "valid")) { Push(L, element != nullptr); return 1; }
    if (element) {
        if (!std::strcmp(key, "type")) { Push(L, static_cast<int>(element->type)); return 1; }
        if (Listed(key, {"id", "controlId", "elementId"})) { Push(L, ref->controlId); return 1; }
        if (LuaPushElementProperty(L, *element, key)) return 1;
    }
    lua_pushnil(L); return 1;
}
int CallLuaElementEvent(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element, int flags) {
    const auto call = [&](int callback) {
        if (!flags || element.removed || element.detaching || (window.state & WIN_STATE_CLOSE)) return 0;
        // 同一批onEvent/onPress/onDrag之间允许隐藏、禁用或清空；每次调用重新检查。
        // Leave保留隐藏时的退出通知，不要求控件继续可交互。
        if (flags != MouseFlag_Leave && !ui::WindowRuntime_IsElementInteractive(window, element)) return 0;
        // 与原生分发一致：没有监听函数不额外消费命中。
        if (callback < 0) return 0;
        const int top = lua_gettop(L);
        lua_rawgeti(L, LUA_REGISTRYINDEX, callback);
        PushLuaElement(L, window, element); lua_pushinteger(L, flags);
        int result = 0;
        if (lua_pcall(L, 2, 1, 0)) m_log_iw(L"[LUA] element event: %hs", lua_tostring(L, -1));
        else result = lua_toboolean(L, -1) ? 1 : 0;
        lua_settop(L, top); return result;
    };
    if (flags == MouseFlag_Leave) return call(element.refLeave);
    int result = call(element.refEvent);
    if (flags & (MouseFlag_Hover | MouseFlag_DragFinish)) result |= call(element.refHover);
    if (flags & MouseFlag_LDownEvent) result |= call(element.refClick);
    if (flags & (MouseFlag_Dragging | MouseFlag_LDownEvent | MouseFlag_LHoldPhysical | MouseFlag_LRepeatDown)) result |= call(element.refPress);
    if (flags & MouseFlag_Leave) result |= call(element.refLeave);
    if (flags & MouseFlag_Dragging) result |= call(element.refDrag);
    if (flags & MouseFlag_DragFinish) result |= call(element.refDrop);
    return result;
}
static void CallElementFrame(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element, int callback) {
    if (callback < 0 || (window.state & WIN_STATE_CLOSE)) return;
    const int top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, callback); PushLuaElement(L, window, element);
    if (lua_pcall(L, 1, 0, 0)) m_log_iw(L"[LUA] element frame: %hs", lua_tostring(L, -1));
    lua_settop(L, top);
}
void LuaWindowBinding::elementUpdate(ui::WindowElement& element) { CallElementFrame(owner, window, element, element.refUpdate); }
void LuaWindowBinding::elementDraw(ui::WindowElement& element) { CallElementFrame(owner, window, element, element.refDraw); }
int LuaWindowBinding::elementEvent(ui::WindowElement& element, int, int flags) { return CallLuaElementEvent(owner, window, element, flags); }
int LuaWindowBinding::event(int index, int flags) {
    if (!flags || window.refEvent < 0 || (window.state & WIN_STATE_CLOSE)) return 0;
    const int top = lua_gettop(owner);
    lua_rawgeti(owner, LUA_REGISTRYINDEX, window.refEvent);
    auto* ref = static_cast<LuaWindowRef*>(lua_newuserdata(owner, sizeof(LuaWindowRef)));
    *ref = this->ref(); luaL_setmetatable(owner, "LuaWindowRef");
    lua_pushinteger(owner, static_cast<ui::ElementId>(index)); lua_pushinteger(owner, flags);
    int result = 0;
    if (lua_pcall(owner, 3, 1, 0)) m_log_iw(L"[LUA] window event: %hs", lua_tostring(owner, -1));
    else result = lua_toboolean(owner, -1) ? 1 : 0;
    lua_settop(owner, top); return result;
}
void RegisterLuaUIProxy(lua_State* L) {
    luaL_newmetatable(L, "LuaElementsRef");
    lua_pushcfunction(L, SafeLuaCall<ElementsIndex>); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, SafeLuaCall<ElementsLength>); lua_setfield(L, -2, "__len"); lua_pop(L, 1);

}
} // namespace cg::script
