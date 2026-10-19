#pragma once
#include "LuaHost.hpp"
namespace cg::script {
struct LuaWindowRef {
    int winId = -1;
    uint32_t generation = 0;
};
struct LuaNativeWindowRef {
    int winId = -1;
    uint32_t generation = 0;
};

struct LuaControlRef {
    int winId = -1;
    uint32_t generation = 0;
    int controlId = 0;
};

struct LuaCallbackRefs {
    lua_State* owner = nullptr; // registry引用所属状态。
    int onUpdate = -1;
    int onDraw = -1;
    int onEvent = -1;
    int onClick = -1;
    int onPress = -1;
    int onHover = -1;
    int onLeave = -1;
    int onDrag = -1;
    int onDrop = -1;
    int onChange = -1;
    int onEnter = -1;

    void clear(lua_State* L);
};

}
