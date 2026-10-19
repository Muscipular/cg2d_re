#include "LuaUI.hpp"
#include <stdexcept>

namespace cg::script {

struct LuaElementsRef { LuaWindowRef window; };
struct LuaNativeElementRef { LuaWindowRef window; int index; };

static ui::WindowInstance* ResolveInstance(LuaWindowRef ref) {
    auto* window = ui::WindowRuntime_FindInstance(ref.winId);
    return window && window->generation == ref.generation && !(window->state & WIN_STATE_CLOSE) ? window : nullptr;
}
static ui::WindowElement* ResolveNativeElement(const LuaNativeElementRef& ref) {
    auto* window = ResolveInstance(ref.window);
    return window && !window->components && ref.index >= 0 && ref.index < window->elementCount
        ? &window->elements[ref.index] : nullptr;
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
LuaWindow* EnsureLuaWindowBinding(lua_State* L, ui::WindowInstance& window) {
    ui::WindowRuntime_EnsureIdentity(window);
    if (!window.binding) {
        // 旧窗口按calloc创建，尚未绑定的registry槽在首次绑定时初始化。
        window.refUpdate = window.refDraw = window.refEvent = -1;
        for (int i = 0; i < window.elementCount; ++i)
            for (const auto& [name, member] : elementRefs) window.elements[i].*member = -1;
        window.binding = new LuaWindow(window, L);
    }
    auto* binding = static_cast<LuaWindow*>(window.binding);
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
        || !std::strcmp(key, "parentId") || !std::strcmp(key, "controlId") || !std::strcmp(key, "mouseState")
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
    if (element.component) {
        auto* ref = static_cast<LuaControlRef*>(lua_newuserdata(L, sizeof(LuaControlRef)));
        *ref = {window.id, window.generation, element.component->id};
        luaL_setmetatable(L, "LuaControlRef");
    } else {
        auto* ref = static_cast<LuaNativeElementRef*>(lua_newuserdata(L, sizeof(LuaNativeElementRef)));
        *ref = {{window.id, window.generation}, static_cast<int>(&element - window.elements)};
        luaL_setmetatable(L, "LuaNativeElementRef");
    }
}
static void PushElements(lua_State* L, ui::WindowInstance& window) {
    auto* ref = static_cast<LuaElementsRef*>(lua_newuserdata(L, sizeof(LuaElementsRef)));
    ref->window = {window.id, window.generation};
    luaL_setmetatable(L, "LuaElementsRef");
}
bool LuaPushElementProperty(lua_State* L, ui::WindowElement& element, const char* key) {
    if (auto* ref = ElementRef(element, key)) { PushRef(L, *ref); return true; }
    if (!std::strcmp(key, "seqNo")) { Push(L, element.seqNo); return true; }
    if (!std::strcmp(key, "x")) { Push(L, element.x); return true; }
    if (!std::strcmp(key, "y")) { Push(L, element.y); return true; }
    if (!std::strcmp(key, "width")) { Push(L, element.width); return true; }
    if (!std::strcmp(key, "height")) { Push(L, element.height); return true; }
    if (!std::strcmp(key, "visible")) { Push(L, element.visible == 1); return true; }
    if (element.type == ui::WindowElementType_Text && element.data.text) {
        const auto& data = *element.data.text;
        if (!std::strcmp(key, "text")) { Push(L, data.text); return true; }
        if (!std::strcmp(key, "font") || !std::strcmp(key, "fontType")) { Push(L, std::to_integer<uint8_t>(data.style.font)); return true; }
        if (!std::strcmp(key, "color")) { Push(L, std::to_integer<uint8_t>(data.style.color)); return true; }
    }
    if (element.type == ui::WindowElementType_Graphic && element.data.graphic) {
        if (!std::strcmp(key, "image")) { Push(L, element.data.graphic->imageNo); return true; }
        if (!std::strcmp(key, "color")) { Push(L, element.data.graphic->color); return true; }
        if (element.component) {
            if (!std::strcmp(key, "imageHover")) { Push(L, element.component->imageData()->imageHover); return true; }
            if (!std::strcmp(key, "imagePress")) { Push(L, element.component->imageData()->imagePress); return true; }
        }
    }
    if (element.type == ui::WindowElementType_Input && element.data.input && element.data.input->context) {
        const auto& context = *element.data.input->context;
        if (!std::strcmp(key, "text")) { Push(L, context.text); return true; }
        if (!std::strcmp(key, "font") || !std::strcmp(key, "fontType")) {
            Push(L, element.component ? element.component->textInputData()->font : context.field13C); return true;
        }
        if (!std::strcmp(key, "color")) {
            Push(L, element.component ? element.component->textInputData()->color : context.field140); return true;
        }
    }
    return PushProperty(L, element.properties, key);
}
bool LuaPushWindowProperty(lua_State* L, ui::WindowInstance& window, const char* key) {
    if (auto* ref = WindowRef(window, key)) { PushRef(L, *ref); return true; }
    if (!std::strcmp(key, "elements")) { PushElements(L, window); return true; }
    return PushProperty(L, window.properties, key);
}

static bool SetElementCore(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element, const char* key, int value) {
    if (auto* ref = ElementRef(element, key)) {
        EnsureLuaWindowBinding(L, window); SetRef(L, value, *ref); return true;
    }
    if (!std::strcmp(key, "seqNo")) { element.seqNo = ReadInteger(L, value); return true; }
    return false;
}
static bool Listed(const char* key, std::initializer_list<const char*> names) {
    for (auto* name : names) if (!std::strcmp(key, name)) return true;
    return false;
}
void LuaApplyElementExtras(lua_State* L, int table, UIControl& control) {
    table = AbsIndex(L, table);
    auto& window = *control.windowOwner;
    auto& element = control.element();
    lua_pushnil(L);
    while (lua_next(L, table)) {
        if (lua_type(L, -2) != LUA_TSTRING) throw std::runtime_error("property name must be a string");
        const auto* key = lua_tostring(L, -2);
        if (!SetElementCore(L, window, element, key, -1)
            && !Listed(key, {"name","x","y","width","height","visible","hitable","parent","parentId","seqNo"})
            && !(control.type == UIControl::TYPE_IMAGE && Listed(key, {"image","id","imageHover","imagePress","color"}))
            && !(control.type == UIControl::TYPE_TEXT && Listed(key, {"text","font","fontType","color"}))
            && !(control.type == UIControl::TYPE_ANIME && Listed(key, {"animeNo","action","dir","revertPlay","revert"}))
            && !(control.type == UIControl::TYPE_TEXT_INPUT && Listed(key, {"text","font","fontType","color","maxLength"}))
            && !(control.type == UIControl::TYPE_PNG_IMAGE && Listed(key, {"image","imageHover","imagePress","imageRect","imageHoverRect","imagePressRect","color"}))
            && !(control.type == UIControl::TYPE_CANVAS && Listed(key, {"shapes"}))
            && !(control.type == UIControl::TYPE_SCROLL_VIEW && Listed(key, {"contentHeight","scrollY","barWidth","scrollStep"}))
            && !(control.type == UIControl::TYPE_MAP_EX && Listed(key, {"mapX","mapY","mapId","floor","w","h"})))
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
            EnsureLuaWindowBinding(L, window); SetRef(L, -1, *ref);
        } else if (!Listed(key, {"id","x","y","width","height","layer","visible","dragMove"}))
            SetProperty(L, window.properties, key, -1);
        lua_pop(L, 1);
    }
}

static int ElementsIndex(lua_State* L) {
    const auto* ref = static_cast<LuaElementsRef*>(luaL_checkudata(L, 1, "LuaElementsRef"));
    auto* window = ResolveInstance(ref->window);
    const int number = ReadInteger(L, 2);
    int index = number > 0 ? number - 1 : -1;
    if (window && index >= 0) {
        if (!window->components) {
            if (index < window->elementCount) { PushLuaElement(L, *window, window->elements[index]); return 1; }
        } else {
            for (const auto* list : {&window->components->components, &window->components->pending})
                for (const auto& item : *list) if (item->attached && index-- == 0) {
                    PushLuaElement(L, *window, item->element()); return 1;
                }
        }
    }
    lua_pushnil(L); return 1;
}
static int ElementsLength(lua_State* L) {
    const auto* ref = static_cast<LuaElementsRef*>(luaL_checkudata(L, 1, "LuaElementsRef"));
    const auto* window = ResolveInstance(ref->window);
    int count = window ? window->elementCount : 0;
    if (window && window->components) {
        count = 0;
        for (const auto* list : {&window->components->components, &window->components->pending})
            for (const auto& item : *list) if (item->attached) ++count;
    }
    lua_pushinteger(L, count); return 1;
}
static int NativeElementIndex(lua_State* L) {
    const auto* ref = static_cast<LuaNativeElementRef*>(luaL_checkudata(L, 1, "LuaNativeElementRef"));
    auto* element = ResolveNativeElement(*ref);
    const auto* key = luaL_checkstring(L, 2);
    if (!std::strcmp(key, "valid")) { Push(L, element != nullptr); return 1; }
    if (element) {
        if (!std::strcmp(key, "type")) { Push(L, static_cast<int>(element->type)); return 1; }
        if (!std::strcmp(key, "id")) { Push(L, ref->index + 1); return 1; }
        if (LuaPushElementProperty(L, *element, key)) return 1;
    }
    lua_pushnil(L); return 1;
}
static int NativeElementNewIndex(lua_State* L) {
    const auto* ref = static_cast<LuaNativeElementRef*>(luaL_checkudata(L, 1, "LuaNativeElementRef"));
    auto* window = ResolveInstance(ref->window);
    auto* element = ResolveNativeElement(*ref);
    if (!element) throw std::runtime_error("element is no longer valid");
    EnsureLuaWindowBinding(L, *window);
    const auto* key = luaL_checkstring(L, 2);
    if (SetElementCore(L, *window, *element, key, 3)) return 0;
    if (!std::strcmp(key, "x")) element->x = ReadInteger(L, 3);
    else if (!std::strcmp(key, "y")) element->y = ReadInteger(L, 3);
    else if (!std::strcmp(key, "width")) element->width = ReadInteger(L, 3);
    else if (!std::strcmp(key, "height")) element->height = ReadInteger(L, 3);
    else if (!std::strcmp(key, "visible")) {
        element->visible = lua_toboolean(L, 3) ? 1 : 0;
        if (!element->visible && element->type == ui::WindowElementType_Input && element->data.input
            && gCurrentInputData == element->data.input->context) ui::detail::WindowChat_RestoreInput();
    } else if (element->type == ui::WindowElementType_Text && element->data.text && Listed(key, {"text","font","fontType","color"})) {
        auto& data = *element->data.text;
        if (!std::strcmp(key, "text")) {
            std::size_t size = 0; const char* bytes = luaL_checklstring(L, 3, &size);
            data.text = text::FromGbkWide(std::string_view(bytes, size));
        } else if (!std::strcmp(key, "color")) data.style.color = static_cast<std::byte>(ReadInteger(L, 3) & 255);
        else data.style.font = static_cast<std::byte>(render::ResolveFontId(ReadInteger(L, 3)));
    } else if (element->type == ui::WindowElementType_Graphic && element->data.graphic && Listed(key, {"image","color"})) {
        if (!std::strcmp(key, "image")) element->data.graphic->imageNo = ReadInteger(L, 3);
        else element->data.graphic->color = static_cast<std::uint32_t>(luaL_checknumber(L, 3));
    } else if (element->type == ui::WindowElementType_Input && element->data.input && element->data.input->context && Listed(key, {"text","font","fontType","color"})) {
        auto& context = *element->data.input->context;
        if (!std::strcmp(key, "text")) {
            std::size_t size = 0; const char* bytes = luaL_checklstring(L, 3, &size);
            context.setText(text::FromGbkWide(std::string_view(bytes, size)));
        } else if (!std::strcmp(key, "color")) context.field140 = ReadInteger(L, 3) & 255;
        else context.field13C = render::ResolveFontId(ReadInteger(L, 3));
    } else SetProperty(L, element->properties, key, 3);
    return 0;
}
static int NativeWindowNewIndex(lua_State* L) {
    const auto* ref = static_cast<LuaNativeWindowRef*>(luaL_checkudata(L, 1, "LuaNativeWindowRef"));
    auto* window = ResolveInstance({ref->winId, ref->generation});
    if (!window) throw std::runtime_error("window is no longer valid");
    EnsureLuaWindowBinding(L, *window);
    const auto* key = luaL_checkstring(L, 2);
    if (auto* callback = WindowRef(*window, key)) SetRef(L, 3, *callback);
    else if (!std::strcmp(key, "x")) window->x = ReadInteger(L, 3);
    else if (!std::strcmp(key, "y")) window->y = ReadInteger(L, 3);
    else if (!std::strcmp(key, "width")) window->width = ReadInteger(L, 3);
    else if (!std::strcmp(key, "height")) window->height = ReadInteger(L, 3);
    else if (!std::strcmp(key, "layer")) window->layer = ReadInteger(L, 3);
    else if (!std::strcmp(key, "visible")) ui::WindowRuntime_SetVisible(*window, lua_toboolean(L, 3) != 0);
    else SetProperty(L, window->properties, key, 3);
    return 0;
}

int CallLuaElementEvent(lua_State* L, ui::WindowInstance& window, ui::WindowElement& element, int flags) {
    const auto call = [&](int callback) {
        if (!flags || (window.state & WIN_STATE_CLOSE) || (element.component && !element.component->attached)) return 0;
        // 同一批onEvent/onPress/onDrag之间允许隐藏、禁用或清空；每次调用重新检查。
        // Leave保留隐藏时的退出通知，不要求控件继续可交互。
        if (flags != MouseFlag_Leave && !ui::WindowRuntime_IsElementInteractive(window, element)) return 0;
        // 旧Lua CallControlCallback在未绑定函数时返回1，hitable控件仍消费命中。
        // 原生旧元素没有这一默认值，脚本监听缺席时不能抢走原业务事件。
        if (callback < 0) return element.component ? 1 : 0;
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
    if (callback < 0 || (window.state & WIN_STATE_CLOSE) || (element.component && !element.component->attached)) return;
    const int top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, callback); PushLuaElement(L, window, element);
    if (lua_pcall(L, 1, 0, 0)) m_log_iw(L"[LUA] element frame: %hs", lua_tostring(L, -1));
    lua_settop(L, top);
}
void LuaWindow::elementUpdate(ui::WindowElement& element) { CallElementFrame(owner, window, element, element.refUpdate); }
void LuaWindow::elementDraw(ui::WindowElement& element) { CallElementFrame(owner, window, element, element.refDraw); }
int LuaWindow::elementEvent(ui::WindowElement& element, int, int flags) { return CallLuaElementEvent(owner, window, element, flags); }
int LuaWindow::event(int index, int flags) {
    if (!flags || window.refEvent < 0 || (window.state & WIN_STATE_CLOSE)) return 0;
    const int top = lua_gettop(owner);
    lua_rawgeti(owner, LUA_REGISTRYINDEX, window.refEvent);
    const bool native = window.id < WIN_MAX;
    if (native) {
        auto* ref = static_cast<LuaNativeWindowRef*>(lua_newuserdata(owner, sizeof(LuaNativeWindowRef)));
        *ref = {window.id, window.generation}; luaL_setmetatable(owner, "LuaNativeWindowRef");
    } else {
        auto* ref = static_cast<LuaWindowRef*>(lua_newuserdata(owner, sizeof(LuaWindowRef)));
        *ref = this->ref(); luaL_setmetatable(owner, "LuaWindowRef");
    }
    lua_pushinteger(owner, index + 1); lua_pushinteger(owner, flags);
    int result = 0;
    if (lua_pcall(owner, 3, 1, 0)) m_log_iw(L"[LUA] window event: %hs", lua_tostring(owner, -1));
    else result = lua_toboolean(owner, -1) ? 1 : 0;
    lua_settop(owner, top); return result;
}
void RegisterLuaUIProxy(lua_State* L) {
    luaL_newmetatable(L, "LuaElementsRef");
    lua_pushcfunction(L, SafeLuaCall<ElementsIndex>); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, SafeLuaCall<ElementsLength>); lua_setfield(L, -2, "__len"); lua_pop(L, 1);
    luaL_newmetatable(L, "LuaNativeElementRef");
    lua_pushcfunction(L, SafeLuaCall<NativeElementIndex>); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, SafeLuaCall<NativeElementNewIndex>); lua_setfield(L, -2, "__newindex"); lua_pop(L, 1);
    luaL_getmetatable(L, "LuaNativeWindowRef");
    lua_pushcfunction(L, SafeLuaCall<NativeWindowNewIndex>); lua_setfield(L, -2, "__newindex"); lua_pop(L, 1);
}
} // namespace cg::script
