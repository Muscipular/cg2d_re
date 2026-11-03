#pragma once
#include "LuaHost.hpp"

namespace cg::session { class ClientSession; }

namespace cg::script {
struct LuaWindowRef {
    int winId = -1;
    session::ClientSession* ownerSession = nullptr; // 所属 Lua VM 的会话，ID 只在此会话内解析。
};


struct LuaControlRef {
    int winId = -1;
    std::uint32_t controlId = 0; // elementId，兼容字段名。
    session::ClientSession* ownerSession = nullptr;
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
