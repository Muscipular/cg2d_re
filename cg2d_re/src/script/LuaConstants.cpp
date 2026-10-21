#include "LuaUI.hpp"
#include <cstring>

namespace cg::script {
namespace lua_const_detail {
struct LuaVkConst {
    int code;
    const char* name;
};

constexpr LuaVkConst kNamedVkConsts[] = {
    {0x01, "LBUTTON"}, {0x02, "RBUTTON"}, {0x03, "CANCEL"}, {0x04, "MBUTTON"},
    {0x05, "XBUTTON1"}, {0x06, "XBUTTON2"}, {0x08, "BACK"}, {0x09, "TAB"},
    {0x0D, "ENTER"}, {0x10, "SHIFT"}, {0x11, "CTRL"}, {0x12, "ALT"},
    {0x13, "PAUSE"}, {0x14, "CAPSLOCK"}, {0x1B, "ESC"}, {0x20, "SPACE"},
    {0x21, "PGUP"}, {0x22, "PGDN"}, {0x23, "END"}, {0x24, "HOME"},
    {0x25, "LEFT"}, {0x26, "UP"}, {0x27, "RIGHT"}, {0x28, "DOWN"},
    {0x2D, "INS"}, {0x2E, "DEL"}, {0x5B, "LWIN"}, {0x5C, "RWIN"},
    {0x5D, "APPS"}, {0x60, "NUMPAD0"}, {0x61, "NUMPAD1"}, {0x62, "NUMPAD2"},
    {0x63, "NUMPAD3"}, {0x64, "NUMPAD4"}, {0x65, "NUMPAD5"}, {0x66, "NUMPAD6"},
    {0x67, "NUMPAD7"}, {0x68, "NUMPAD8"}, {0x69, "NUMPAD9"}, {0x6A, "MULTIPLY"},
    {0x6B, "ADD"}, {0x6C, "SEPARATOR"}, {0x6D, "SUBTRACT"}, {0x6E, "DECIMAL"},
    {0x6F, "DIVIDE"}, {0x90, "NUMLOCK"}, {0x91, "SCROLLLOCK"}, {0xA0, "LSHIFT"},
    {0xA1, "RSHIFT"}, {0xA2, "LCTRL"}, {0xA3, "RCTRL"}, {0xA4, "LALT"},
    {0xA5, "RALT"}, {0xA6, "BROWSER_BACK"}, {0xA7, "BROWSER_FORWARD"},
    {0xA8, "BROWSER_REFRESH"}, {0xA9, "BROWSER_STOP"}, {0xAA, "BROWSER_SEARCH"},
    {0xAB, "BROWSER_FAVORITES"}, {0xAC, "BROWSER_HOME"}, {0xAD, "VOLUME_MUTE"},
    {0xAE, "VOLUME_DOWN"}, {0xAF, "VOLUME_UP"}, {0xB0, "MEDIA_NEXT"},
    {0xB1, "MEDIA_PREV"}, {0xB2, "MEDIA_STOP"}, {0xB3, "MEDIA_PLAY"},
    {0xB4, "LAUNCH_MAIL"}, {0xB5, "LAUNCH_MEDIA"}, {0xB6, "LAUNCH_APP1"},
    {0xB7, "LAUNCH_APP2"}, {0xBA, "OEM_1"}, {0xBB, "OEM_PLUS"},
    {0xBC, "OEM_COMMA"}, {0xBD, "OEM_MINUS"}, {0xBE, "OEM_PERIOD"},
    {0xBF, "OEM_2"}, {0xC0, "OEM_3"}, {0xDB, "OEM_4"}, {0xDC, "OEM_5"},
    {0xDD, "OEM_6"}, {0xDE, "OEM_7"}, {0xDF, "OEM_8"}, {0xE2, "OEM_102"},
};

static void AddVkConst(lua_State* L, int code, const char* name) {
    lua_pushinteger(L, code);
    lua_pushstring(L, name);
    lua_settable(L, -3);
    lua_pushinteger(L, code);
    lua_setfield(L, -2, name);
}

static void RegVkConst(lua_State* L) {
    lua_getfield(L, -1, "VK");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
    }

    for (const auto& item: kNamedVkConsts) {
        AddVkConst(L, item.code, item.name);
    }

    for (int i = 0; i <= 9; ++i) {
        char name[2] = {static_cast<char>('0' + i), '\0'};
        AddVkConst(L, 0x30 + i, name);
    }

    for (int i = 'A'; i <= 'Z'; ++i) {
        char name[2] = {static_cast<char>(i), '\0'};
        AddVkConst(L, i, name);
    }

    for (int i = 1; i <= 24; ++i) {
        char name[4] = {};
        sprintf_s(name, sizeof(name), "F%d", i);
        AddVkConst(L, 0x70 + (i - 1), name);
    }

    lua_pushvalue(L, -1);
    lua_setglobal(L, "VK");
    lua_setfield(L, -2, "VK");
}

static int LuaScreenIndex(lua_State* L) {
    const char* key = lua_tostring(L, 2);
    if (!key) {
        lua_pushnil(L);
        return 1;
    }
    if (strcmp(key, "Width") == 0) {
        lua_pushinteger(L, render::GetRendererLogicalWidth());
        return 1;
    }
    if (strcmp(key, "Height") == 0) {
        lua_pushinteger(L, render::GetRendererLogicalHeight());
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static void RegScreenConst(lua_State* L) {
    lua_newtable(L);
    lua_newtable(L);
    lua_pushcfunction(L, SafeLuaCall<LuaScreenIndex>);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, "Screen");
}

static int LuaMouseIndex(lua_State* L) {
    const char* key = lua_tostring(L, 2);
    if (!key) {
        lua_pushnil(L);
        return 1;
    }
    if (strcmp(key, "x") == 0) {
        lua_pushinteger(L, gMouseX);
        return 1;
    }
    if (strcmp(key, "y") == 0) {
        lua_pushinteger(L, gMouseY);
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static void RegMouseConst(lua_State* L) {
    lua_newtable(L);
    lua_newtable(L);
    lua_pushcfunction(L, SafeLuaCall<LuaMouseIndex>);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    lua_setfield(L, -2, "Mouse");
}

static int LuaConstIndex(lua_State* L) {
    const char* key = lua_tostring(L, 2);
    if (!key) {
        lua_pushnil(L);
        return 1;
    }
    if (strcmp(key, "GameTick") == 0) {
        lua_pushnumber(L, static_cast<lua_Number>(platform::ReadTickCount64()));
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

static int LuaConstNewIndex(lua_State* L) {
    const char* key = lua_tostring(L, 2);
    if (key && strcmp(key, "GameTick") == 0) {
        return luaL_error(L, "CONST.GameTick is readonly");
    }
    lua_rawset(L, 1);
    return 0;
}

static void RegConstMeta(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, SafeLuaCall<LuaConstIndex>);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, SafeLuaCall<LuaConstNewIndex>);
    lua_setfield(L, -2, "__newindex");
    lua_setmetatable(L, -2);
}
} // namespace lua_const_detail

void RegUIConst(lua_State* L) {

    lua_getglobal(L, "CONST");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
    }

    lua_newtable(L);

    lua_newtable(L);
    lua_pushinteger(L, ui::WindowElementType_EventArea);
    lua_setfield(L, -2, "NONE");
    lua_pushinteger(L, ui::WindowElementType_Graphic);
    lua_setfield(L, -2, "IMAGE");
    lua_pushinteger(L, ui::WindowElementType_Text);
    lua_setfield(L, -2, "TEXT");
    lua_pushinteger(L, ui::WindowElementType_PngImage);
    lua_setfield(L, -2, "PNG_IMAGE");
    lua_pushinteger(L, ui::WindowElementType_Input);
    lua_setfield(L, -2, "TEXT_INPUT");
    lua_pushinteger(L, ui::WindowElementType_Animation);
    lua_setfield(L, -2, "ANIME");
    lua_pushinteger(L, ui::WindowElementType_ScrollView);
    lua_setfield(L, -2, "SCROLL_VIEW");
    lua_pushinteger(L, ui::WindowElementType_MapEx);
    lua_setfield(L, -2, "MAP");
    lua_pushinteger(L, ui::WindowElementType_Canvas);
    lua_setfield(L, -2, "CANVAS");
    lua_setfield(L, -2, "TYPE");

    lua_newtable(L);
    lua_pushinteger(L, ui::WindowMouseState_None);
    lua_setfield(L, -2, "NONE");
    lua_pushinteger(L, ui::WindowMouseState_Hover);
    lua_setfield(L, -2, "HOVER");
    lua_pushinteger(L, ui::WindowMouseState_Press);
    lua_setfield(L, -2, "PRESS");
    lua_setfield(L, -2, "MOUSE_STATE");

    lua_setfield(L, -2, "UIControl");

    lua_newtable(L);
    lua_pushinteger(L, MouseFlag_None);
    lua_setfield(L, -2, "NONE");
    lua_pushinteger(L, MouseFlag_Hover);
    lua_setfield(L, -2, "HOVER");
    lua_pushinteger(L, MouseFlag_LDownEvent);
    lua_setfield(L, -2, "L_DOWN_EVENT");
    lua_pushinteger(L, MouseFlag_RDownEvent);
    lua_setfield(L, -2, "R_DOWN_EVENT");
    lua_pushinteger(L, MouseFlag_LHoldPhysical);
    lua_setfield(L, -2, "L_HOLD_PHYSICAL");
    lua_pushinteger(L, MouseFlag_RHoldPhysical);
    lua_setfield(L, -2, "R_HOLD_PHYSICAL");
    lua_pushinteger(L, MouseFlag_Dragging);
    lua_setfield(L, -2, "DRAGGING");
    lua_pushinteger(L, MouseFlag_DragFinish);
    lua_setfield(L, -2, "DRAG_FINISH");
    lua_pushinteger(L, MouseFlag_Leave);
    lua_setfield(L, -2, "LEAVE");
    lua_pushinteger(L, MouseFlag_LDblClickEvent);
    lua_setfield(L, -2, "L_DBL_CLICK_EVENT");
    lua_pushinteger(L, MouseFlag_RDblClickEvent);
    lua_setfield(L, -2, "R_DBL_CLICK_EVENT");
    lua_pushinteger(L, MouseFlag_LRepeatDown);
    lua_setfield(L, -2, "L_REPEAT_DOWN");
    lua_pushinteger(L, MouseFlag_RRepeatDown);
    lua_setfield(L, -2, "R_REPEAT_DOWN");
    lua_setfield(L, -2, "MouseFlags");

    lua_newtable(L);
    lua_pushinteger(L, KEY_UP_EDGE);
    lua_setfield(L, -2, "UP_EDGE");
    lua_pushinteger(L, KEY_DOWN);
    lua_setfield(L, -2, "DOWN");
    lua_pushinteger(L, KEY_DOWN_EDGE);
    lua_setfield(L, -2, "DOWN_EDGE");
    lua_pushinteger(L, KEY_DOWN_LATCH);
    lua_setfield(L, -2, "DOWN_LATCH");
    lua_pushinteger(L, KEY_DOWN_MESSAGE);
    lua_setfield(L, -2, "DOWN_MESSAGE");
    lua_setfield(L, -2, "KeyStateFlag");

    lua_const_detail::RegScreenConst(L);
    lua_const_detail::RegMouseConst(L);
    lua_const_detail::RegVkConst(L);
    lua_const_detail::RegConstMeta(L);
    lua_setglobal(L, "CONST");

}
} // namespace cg::script
