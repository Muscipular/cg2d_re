#include "cg/network/CgConnection.hpp"
#include "LuaUI.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../world/ChatHistory.hpp"
#include "../world/WorldMapState.hpp"
#include <filesystem>
#include <ranges>
#include <unordered_map>
#include <cstring>
#include <cstdio>
#include <limits>
#include <optional>

namespace cg::script {
void startCopilot(int x, int y);
void stopCopilot();
int getAutoCopliotState();
std::vector<std::pair<int, int> > getAutoCopliotRoute();

//std::vector<std::shared_ptr<UIWindowInstanceEx>> winList;



static ui::ElementId LuaReadElementId(lua_State* L, int index) {
    if (lua_isnoneornil(L, index)) return 0;
    if (lua_type(L, index) == LUA_TSTRING && lua_objlen(L, index) == 0) return 0;
    if (lua_type(L, index) != LUA_TNUMBER) throw std::runtime_error("elementId must be a non-negative integer");
    const auto value = lua_tonumber(L, index);
    if (!std::isfinite(value) || value < 0 || value > std::numeric_limits<ui::ElementId>::max() || std::trunc(value) != value)
        throw std::runtime_error("elementId must be a non-negative integer");
    return static_cast<ui::ElementId>(value);
}
static ui::ElementId LuaElementIdField(lua_State* L, int table) {
    lua_getfield(L, table, "elementId");
    auto id = LuaReadElementId(L, -1); lua_pop(L, 1); return id;
}

static constexpr int LUA_REF_NONE = -1;
static int ResolveLuaMapX(int mapX) {
    return mapX >= 0 ? mapX : static_cast<std::int32_t>(world::gPlayerX);
}

static int ResolveLuaMapY(int mapY) {
    return mapY >= 0 ? mapY : static_cast<std::int32_t>(world::gPlayerY);
}

static int ResolveLuaMapId(int mapId) {
    return mapId >= 0 ? mapId : world::gCurrentMapId_9A86DC;
}

static int ResolveLuaFloor(int floor) {
    return floor >= 0 ? floor : world::gMapFloor;
}

struct MsgOption {
    std::wstring sender;
    int color;
    int font;
};

struct LuaEventRegistration {
    lua_State* owner = nullptr;
    int callbackRef = LUA_REF_NONE;
    bool active = true;
};

struct LuaEventHandleRef {
    LuaEventRegistration* event;
};

enum class LuaBridgeStateType {
    Unknown,
    LuaUI,
    AutoBattle,
};

enum class LuaBridgeValueType {
    String,
    Number,
};

struct LuaBridgeValue {
    LuaBridgeValueType type;
    std::string stringValue;
    lua_Number numberValue = 0;
};

struct LuaBridgeEventRegistration {
    lua_State* L = nullptr;
    LuaBridgeStateType stateType = LuaBridgeStateType::Unknown;
    std::string header;
    int callbackRef = LUA_REF_NONE;
    bool active = true;
};

struct LuaBridgeHandleRef {
    LuaBridgeEventRegistration* event;
};

struct WinMgrKeyPressOption {
    int vkMain;
    std::vector<int> vkList;
    int stateMask;
    std::shared_ptr<LuaEventRegistration> event;
};

struct WinMgrPacketRecvOption {
    std::string header;
    std::shared_ptr<LuaEventRegistration> event;
};

struct WinMgrPacketSendOption {
    std::string header;
    std::shared_ptr<LuaEventRegistration> event;
};

enum class WinMgrEventType {
    SceneStateChanged,
    ChatMessage,
    WindowFocusChanged,
    WindowOpen,
    WindowClose,
};

static std::vector<std::shared_ptr<LuaEventRegistration> > gLuaEventRegistrations;
static std::vector<std::shared_ptr<LuaBridgeEventRegistration> > gLuaBridgeEventRegistrations;
static std::vector<WinMgrKeyPressOption> gWinMgrKeyPressOptions;
static std::vector<WinMgrPacketRecvOption> gWinMgrPacketRecvOptions;
static std::vector<WinMgrPacketSendOption> gWinMgrPacketSendOptions;
static std::unordered_map<WinMgrEventType, std::vector<std::shared_ptr<LuaEventRegistration> > > gWinMgrEventOptions;
static std::unordered_map<lua_State*, LuaBridgeStateType> gLuaBridgeStateTypes;

static std::vector<std::shared_ptr<LuaEventRegistration> >& WinMgrEventOptions(WinMgrEventType type) {
    return gWinMgrEventOptions[type];
}

static int LuaCheckVkCode(lua_State* L, int index);

static void LuaReadVkList(lua_State* L, int index, std::vector<int>& vkList);

static bool WinMgrKeyPressed(const WinMgrKeyPressOption& option);

static void CliSendMsg(std::wstring_view msg, const MsgOption& option) {
    if (msg.empty()) return;
    const std::wstring value = option.sender.empty()
        ? std::wstring(msg) : option.sender + L": " + std::wstring(msg);
    // Lua消息沿原290个GBK字节上限，临时编码仅用于兼容容量。
    const auto encoded = text::ToGbk(value);
    std::size_t end = 0;
    while (end < encoded.size()) {
        const auto next = end + (static_cast<unsigned char>(encoded[end]) & 0x80 ? 2 : 1);
        if (next > 290 || next > encoded.size()) break;
        end = next;
    }
    world::AppendChatMessage_44B050(text::FromGbkWide(std::string_view(encoded).substr(0, end)).c_str(), option.color, option.font);
}

static int LuaAbsIndex(lua_State* L, int index) {
    if (index > 0 || index <= LUA_REGISTRYINDEX) {
        return index;
    }
    return lua_gettop(L) + index + 1;
}

static bool LuaHasField(lua_State* L, int index, const char* field) {
    index = LuaAbsIndex(L, index);
    lua_getfield(L, index, field);
    bool exists = !lua_isnil(L, -1);
    lua_pop(L, 1);
    return exists;
}

static int LuaGetIntField(lua_State* L, int index, const char* field, int defaultVal) {
    index = LuaAbsIndex(L, index);
    lua_getfield(L, index, field);
    int value = static_cast<int>(luaL_optinteger(L, -1, defaultVal));
    lua_pop(L, 1);
    return value;
}

static COLORREF LuaGetColorField(lua_State* L, int index, const char* field, COLORREF defaultVal) {
    index = LuaAbsIndex(L, index);
    lua_getfield(L, index, field);
    COLORREF value = defaultVal;
    if (!lua_isnil(L, -1) && lua_isnumber(L, -1)) {
        lua_Number number = lua_tonumber(L, -1);
        if (number < 0) {
            value = static_cast<COLORREF>(static_cast<int>(number));
        } else {
            value = static_cast<COLORREF>(static_cast<uint32_t>(number));
        }
    }
    lua_pop(L, 1);
    return value;
}

static void LuaPushColor(lua_State* L, COLORREF color) {
    lua_pushnumber(L, static_cast<lua_Number>(static_cast<uint32_t>(color)));
}

static bool LuaGetBoolField(lua_State* L, int index, const char* field, bool defaultVal) {
    index = LuaAbsIndex(L, index);
    lua_getfield(L, index, field);
    bool value = lua_isnil(L, -1) ? defaultVal : lua_toboolean(L, -1) != 0;
    lua_pop(L, 1);
    return value;
}

static std::string LuaGetStringField(lua_State* L, int index, const char* field, const char* defaultVal) {
    index = LuaAbsIndex(L, index);
    lua_getfield(L, index, field);
    const char* value = luaL_optstring(L, -1, defaultVal == nullptr ? "" : defaultVal);
    std::string result = value == nullptr ? "" : value;
    lua_pop(L, 1);
    return result;
}

// Lua业务字符串在属性入口解码；缺省值直接沿用内部宽文本。
static std::wstring LuaGetWideStringField(lua_State* L, int index, const char* field, std::wstring_view defaultVal = {}) {
    index = LuaAbsIndex(L, index);
    lua_getfield(L, index, field);
    std::wstring result = lua_isnil(L, -1) ? std::wstring(defaultVal) : text::FromGbkWide(luaL_checkstring(L, -1));
    lua_pop(L, 1);
    return result;
}

static bool LuaReadNumberIntField(lua_State* L, int index, const char* field, int& value) {
    index = LuaAbsIndex(L, index);
    lua_getfield(L, index, field);
    bool ok = lua_isnumber(L, -1) != 0;
    if (ok) {
        value = static_cast<int>(lua_tointeger(L, -1));
    }
    lua_pop(L, 1);
    return ok;
}

static void LuaUnref(lua_State* L, int& ref) {
    if (ref != LUA_REF_NONE) {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
        ref = LUA_REF_NONE;
    }
}

static std::shared_ptr<LuaEventRegistration> LuaRefEvent(lua_State* L, int index) {
    lua_pushvalue(L, index);
    auto event = std::make_shared<LuaEventRegistration>();
    event->owner = L;
    event->callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
    gLuaEventRegistrations.push_back(event);
    return event;
}

static bool LuaUnregisterEvent(lua_State* /*L*/, LuaEventRegistration* event) {
    if (!event || !event->active) {
        return false;
    }
    event->active = false;
    LuaUnref(event->owner, event->callbackRef);
    return true;
}

static void PushEventHandle(lua_State* L, const std::shared_ptr<LuaEventRegistration>& event) {
    auto data = static_cast<LuaEventHandleRef*>(lua_newuserdata(L, sizeof(LuaEventHandleRef)));
    data->event = event.get();
    luaL_setmetatable(L, CName(LuaEventHandleRef));
}

static LuaBridgeStateType LuaBridgeGetStateType(lua_State* L) {
    auto it = gLuaBridgeStateTypes.find(L);
    if (it == gLuaBridgeStateTypes.end()) {
        return LuaBridgeStateType::Unknown;
    }
    return it->second;
}

static void LuaBridgeSetStateType(lua_State* L, LuaBridgeStateType stateType) {
    if (!L) {
        return;
    }
    gLuaBridgeStateTypes[L] = stateType;
    for (auto& event: gLuaBridgeEventRegistrations) {
        if (event && event->L == L) {
            event->stateType = stateType;
        }
    }
}

static void LuaBridgeSetLuaUIState(lua_State* L) {
    LuaBridgeSetStateType(L, LuaBridgeStateType::LuaUI);
}

void LuaBridgeSetAutoBattleState(lua_State* L) {
    LuaBridgeSetStateType(L, LuaBridgeStateType::AutoBattle);
}

static bool LuaBridgeCanSendTo(LuaBridgeStateType from, LuaBridgeStateType to) {
    return (from == LuaBridgeStateType::LuaUI && to == LuaBridgeStateType::AutoBattle)
        || (from == LuaBridgeStateType::AutoBattle && to == LuaBridgeStateType::LuaUI);
}

static bool LuaBridgeUnregisterEvent(LuaBridgeEventRegistration* event) {
    if (!event || !event->active) {
        return false;
    }
    event->active = false;
    if (event->L) {
        LuaUnref(event->L, event->callbackRef);
    }
    // 句柄仍可查询active；保留注册对象到关闭Lua之前。
    return true;
}

static void PushBridgeHandle(lua_State* L, const std::shared_ptr<LuaBridgeEventRegistration>& event) {
    auto data = static_cast<LuaBridgeHandleRef*>(lua_newuserdata(L, sizeof(LuaBridgeHandleRef)));
    data->event = event.get();
    luaL_setmetatable(L, CName(LuaBridgeHandleRef));
}

static void LuaBridgePushValue(lua_State* L, const LuaBridgeValue& value) {
    if (value.type == LuaBridgeValueType::Number) {
        lua_pushnumber(L, value.numberValue);
        return;
    }
    lua_pushlstring(L, value.stringValue.data(), value.stringValue.size());
}

static void LuaBridgeCallEvent(const std::shared_ptr<LuaBridgeEventRegistration>& event,
    const std::string& header,
    const std::vector<LuaBridgeValue>& values) {
    if (!event || !event->active || !event->L || event->callbackRef == LUA_REF_NONE) {
        return;
    }

    lua_State* L = event->L;
    int top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, event->callbackRef);
    if (lua_isfunction(L, -1)) {
        lua_pushlstring(L, header.data(), header.size());
        for (const auto& value: values) {
            LuaBridgePushValue(L, value);
        }
        if (lua_pcall(L, static_cast<int>(values.size() + 1), 0, 0) != 0) {
            m_log_iw(L"[LUA] call lua Bridge.On error: %hs\n", lua_tostring(L, -1));
        }
    }
    lua_settop(L, top);
}

static int LuaRefFunctionField(lua_State* L, int index, const char* field) {
    index = LuaAbsIndex(L, index);
    lua_getfield(L, index, field);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return LUA_REF_NONE;
    }
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

void LuaCallbackRefs::clear(lua_State* L) {
    LuaUnref(L, onUpdate);
    LuaUnref(L, onDraw);
    LuaUnref(L, onEvent);
    LuaUnref(L, onClick);
    LuaUnref(L, onPress);
    LuaUnref(L, onHover);
    LuaUnref(L, onLeave);
    LuaUnref(L, onDrag);
    LuaUnref(L, onDrop);
    LuaUnref(L, onChange);
    LuaUnref(L, onEnter);
}

static LuaCallbackRefs LuaReadCallbacks(lua_State* L, int index) {
    LuaCallbackRefs refs;
    refs.owner = L;
    refs.onUpdate = LuaRefFunctionField(L, index, "onUpdate");
    refs.onDraw = LuaRefFunctionField(L, index, "onDraw");
    refs.onEvent = LuaRefFunctionField(L, index, "onEvent");
    refs.onClick = LuaRefFunctionField(L, index, "onClick");
    refs.onPress = LuaRefFunctionField(L, index, "onPress");
    refs.onHover = LuaRefFunctionField(L, index, "onHover");
    refs.onLeave = LuaRefFunctionField(L, index, "onLeave");
    refs.onDrag = LuaRefFunctionField(L, index, "onDrag");
    refs.onDrop = LuaRefFunctionField(L, index, "onDrop");
    refs.onChange = LuaRefFunctionField(L, index, "onChange");
    refs.onEnter = LuaRefFunctionField(L, index, "onEnter");
    return refs;
}

static bool LuaReadPngRectField(lua_State* L, int index, const char* field, RECT16& rect) {
    index = LuaAbsIndex(L, index);
    lua_getfield(L, index, field);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    int x = LuaGetIntField(L, -1, "x", 0);
    int y = LuaGetIntField(L, -1, "y", 0);
    int w = LuaGetIntField(L, -1, "width", 0);
    int h = LuaGetIntField(L, -1, "height", 0);
    if (w <= 0) {
        w = LuaGetIntField(L, -1, "w", 0);
    }
    if (h <= 0) {
        h = LuaGetIntField(L, -1, "h", 0);
    }
    lua_pop(L, 1);
    if (w <= 0 || h <= 0) {
        rect = {};
        return false;
    }

    rect = {
        static_cast<short>(x),
        static_cast<short>(x + w),
        static_cast<short>(y),
        static_cast<short>(y + h)
    };
    return true;
}

static void LuaReadPngImageState(lua_State* L, int index, const char* imageField, const char* rectField, UIControl::PngImageState& state) {
    state.path = LuaGetWideStringField(L, index, imageField);
    state.hasRect = LuaReadPngRectField(L, index, rectField, state.rect);
}

static UIControl::PngImageData LuaReadPngImageData(lua_State* L, int index) {
    UIControl::PngImageData data{};
    LuaReadPngImageState(L, index, "image", "imageRect", data.image);
    LuaReadPngImageState(L, index, "imageHover", "imageHoverRect", data.imageHover);
    LuaReadPngImageState(L, index, "imagePress", "imagePressRect", data.imagePress);
    data.color = LuaGetColorField(L, index, "color", 0xffffffff);
    return data;
}

static int LuaGetCanvasPositiveIntField(lua_State* L, int index, const char* field, int defaultVal) {
    return std::max(1, LuaGetIntField(L, index, field, defaultVal));
}

static int LuaGetCanvasSegments(lua_State* L, int index) {
    return std::clamp(LuaGetIntField(L, index, "segments", 16), 1, 128);
}

static bool LuaReadCanvasShapeType(const char* type, CanvasShapeType& out) {
    if (!type) {
        return false;
    }
    if (strcmp(type, "line") == 0) {
        out = CanvasShapeType::Line;
    } else if (strcmp(type, "rect") == 0 || strcmp(type, "rectangle") == 0) {
        out = CanvasShapeType::Rect;
    } else if (strcmp(type, "rectBorder") == 0 || strcmp(type, "rectFrame") == 0 || strcmp(type, "rect_border") == 0) {
        out = CanvasShapeType::RectBorder;
    } else if (strcmp(type, "circle") == 0) {
        out = CanvasShapeType::Circle;
    } else if (strcmp(type, "circleBorder") == 0 || strcmp(type, "circleFrame") == 0 || strcmp(type, "circle_border") == 0) {
        out = CanvasShapeType::CircleBorder;
    } else if (strcmp(type, "path") == 0) {
        out = CanvasShapeType::Path;
    } else if (strcmp(type, "pathBorder") == 0 || strcmp(type, "pathFrame") == 0 || strcmp(type, "path_border") == 0) {
        out = CanvasShapeType::PathBorder;
    } else {
        return false;
    }
    return true;
}

static bool LuaReadCanvasShape(lua_State* L, int index, CanvasShape& shape) {
    index = LuaAbsIndex(L, index);
    auto type = LuaGetStringField(L, index, "type", "");
    if (!LuaReadCanvasShapeType(type.c_str(), shape.type)) {
        return false;
    }

    shape.color = LuaGetColorField(L, index, "color", 0xffffffff);
    shape.thickness = LuaGetCanvasPositiveIntField(L, index, "thickness", 1);
    shape.segments = LuaGetCanvasSegments(L, index);
    shape.x = static_cast<float>(LuaGetIntField(L, index, "x", 0));
    shape.y = static_cast<float>(LuaGetIntField(L, index, "y", 0));
    shape.w = static_cast<float>(LuaGetIntField(L, index, "width", LuaGetIntField(L, index, "w", 0)));
    shape.h = static_cast<float>(LuaGetIntField(L, index, "height", LuaGetIntField(L, index, "h", 0)));
    shape.x1 = static_cast<float>(LuaGetIntField(L, index, "x1", 0));
    shape.y1 = static_cast<float>(LuaGetIntField(L, index, "y1", 0));
    shape.x2 = static_cast<float>(LuaGetIntField(L, index, "x2", 0));
    shape.y2 = static_cast<float>(LuaGetIntField(L, index, "y2", 0));
    shape.radius = static_cast<float>(LuaGetIntField(L, index, "radius", LuaGetIntField(L, index, "r", 0)));
    shape.path = LuaGetWideStringField(L, index, "d");
    return true;
}

static std::vector<CanvasShape> LuaReadCanvasShapes(lua_State* L, int index) {
    index = LuaAbsIndex(L, index);
    std::vector<CanvasShape> shapes;
    lua_getfield(L, index, "shapes");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return shapes;
    }

    int shapesIndex = LuaAbsIndex(L, -1);
    size_t count = lua_objlen(L, shapesIndex);
    shapes.reserve(count);
    for (size_t i = 1; i <= count; ++i) {
        lua_rawgeti(L, shapesIndex, static_cast<int>(i));
        if (lua_istable(L, -1)) {
            CanvasShape shape{};
            if (LuaReadCanvasShape(L, -1, shape)) {
                shapes.push_back(std::move(shape));
            }
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return shapes;
}

static void LuaSetPngImageState(lua_State* L, int index, const char* imageField, const char* rectField, UIControl::PngImageState& state) {
    if (LuaHasField(L, index, imageField)) {
        state.path = LuaGetWideStringField(L, index, imageField, state.path);
    }
    if (LuaHasField(L, index, rectField)) {
        state.hasRect = LuaReadPngRectField(L, index, rectField, state.rect);
    }
}

static void LuaPushPngRect(lua_State* L, const UIControl::PngImageState& state) {
    if (!state.hasRect) {
        lua_pushnil(L);
        return;
    }

    lua_newtable(L);
    lua_pushinteger(L, state.rect.left);
    lua_setfield(L, -2, "x");
    lua_pushinteger(L, state.rect.top);
    lua_setfield(L, -2, "y");
    lua_pushinteger(L, state.rect.right - state.rect.left);
    lua_setfield(L, -2, "width");
    lua_pushinteger(L, state.rect.bottom - state.rect.top);
    lua_setfield(L, -2, "height");
}

static void LuaSetIntegerField(lua_State* L, const char* name, lua_Integer value) {
    lua_pushinteger(L, value);
    lua_setfield(L, -2, name);
}

static COLORREF LuaReadColorValue(lua_State* L, int index, COLORREF defaultVal) {
    if (!lua_isnumber(L, index)) {
        return defaultVal;
    }
    lua_Number number = lua_tonumber(L, index);
    if (number < 0) {
        return static_cast<COLORREF>(static_cast<int>(number));
    }
    return static_cast<COLORREF>(static_cast<uint32_t>(number));
}

static std::vector<COLORREF> LuaReadMapPointColors(lua_State* L, int index) {
    index = LuaAbsIndex(L, index);
    std::vector<COLORREF> colors;
    lua_getfield(L, index, "color");
    if (lua_istable(L, -1)) {
        int colorIndex = LuaAbsIndex(L, -1);
        size_t count = lua_objlen(L, colorIndex);
        colors.reserve(count);
        for (size_t i = 1; i <= count; ++i) {
            lua_rawgeti(L, colorIndex, static_cast<int>(i));
            if (lua_isnumber(L, -1)) {
                colors.push_back(LuaReadColorValue(L, -1, 0xffffffff));
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    if (colors.empty()) {
        colors.push_back(0xffffffff);
    }
    return colors;
}

static bool LuaReadMapPoint(lua_State* L, int index, UIMapExControl::MapExPoint& point) {
    if (!lua_istable(L, index)) {
        return false;
    }
    index = LuaAbsIndex(L, index);
    int mapX = 0;
    int mapY = 0;
    int type = 0;
    if (!LuaReadNumberIntField(L, index, "mapX", mapX) || !LuaReadNumberIntField(L, index, "mapY", mapY) || !LuaReadNumberIntField(L, index, "type", type)) {
        return false;
    }
    if (type < 0 || type > 3) {
        return false;
    }

    int size = 2;
    if (LuaHasField(L, index, "size") && !LuaReadNumberIntField(L, index, "size", size)) {
        return false;
    }
    point.mapX = mapX;
    point.mapY = mapY;
    point.type = type;
    point.size = std::clamp(size, 1, 16);
    point.colors = LuaReadMapPointColors(L, index);
    return true;
}

static void LuaPushMapPoint(lua_State* L, const UIMapExControl::MapExPoint& point) {
    lua_createtable(L, 0, 5);
    LuaSetIntegerField(L, "mapX", point.mapX);
    LuaSetIntegerField(L, "mapY", point.mapY);
    LuaSetIntegerField(L, "type", point.type);
    LuaSetIntegerField(L, "size", point.size);
    lua_pushstring(L, "color");
    lua_createtable(L, static_cast<int>(point.colors.size()), 0);
    for (size_t i = 0; i < point.colors.size(); ++i) {
        LuaPushColor(L, point.colors[i]);
        lua_rawseti(L, -2, static_cast<int>(i + 1));
    }
    lua_rawset(L, -3);
}


LuaFunctionEx(Graphic, GetGrahpicInfo) {
    int mapNo = static_cast<int>(luaL_checkinteger(L, 1));
    if (mapNo <= 0 || mapNo >= resource::gBinLoaderGraphicCapacity || !resource::gGraphicMapNoIndex || !resource::gGraphicInfo) { lua_pushnil(L); return 1; }
    int seqNo = resource::gGraphicMapNoIndex[mapNo];
    if (seqNo < 0 || seqNo >= resource::gMaxGraphicCount || resource::gGraphicInfo[seqNo].mapNo != mapNo) { lua_pushnil(L); return 1; }
    const auto& info = resource::gGraphicInfo[seqNo];
    lua_newtable(L);
    Field(L, "seqNo", info.seqNo);
    Field(L, "offset", info.offset);
    Field(L, "length", info.length);
    Field(L, "x", info.x);
    Field(L, "y", info.y);
    Field(L, "w", info.w);
    Field(L, "h", info.h);
    Field(L, "graphicFileIndex", info.fileIndex);
    Field(L, "field16", (info.reserved[0] | (info.reserved[1] << 8)));
    Field(L, "sx", info.sx);
    Field(L, "sy", info.sy);
    Field(L, "blockFlag", info.flag);
    Field(L, "mapNo", info.mapNo);
    return 1;
}

void __declspec(noinline) RegGraphic(lua_State* L) {

    lua_newtable(L);
    lua_pushcfunction(L, LuaCFunction(Graphic, GetGrahpicInfo));
    lua_setfield(L, -2, "GetGrahpicInfo");
    lua_setglobal(L, "Graphic");
    lua_settop(L, 0);

}

LuaWindow::LuaWindow(UIWindowInstance& value, lua_State* state)
    : window(value), owner(state), generation(value.generation) {}
LuaWindowBinding::~LuaWindowBinding() {
    LuaUnref(owner, window.refUpdate);
    LuaUnref(owner, window.refDraw);
    LuaUnref(owner, window.refEvent);
    // 脚本退出时原生窗口可能仍存在，解除元素回调但不销毁UI。
    for (auto& [key, element] : window.sortedMap) {
        ReleaseLuaElementRefs(owner, element);
        if (element.component) element.component->callbacks.reset();
    }
    for (auto& node : window.retiredElements) {
        auto& element = node.mapped();
        ReleaseLuaElementRefs(owner, element);
        if (element.component) element.component->callbacks.reset();
    }
}

LuaWindowRef LuaWindow::ref() const { return {window.id}; }
UIControl* LuaWindow::FindControl(int controlId) {
    return window.components ? window.components->find(controlId) : nullptr;
}
void LuaWindow::ClearChildren() { ui::WindowRuntime_ClearElements(window); }
UIControl* LuaWindow::AddControl(std::unique_ptr<UIControl> control, int parentId) {
    return ui::WindowRuntime_Components(window).add(std::move(control), parentId);
}

UIControl* LuaWindow::AddImage(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::ImageData data, LuaCallbackRefs callbacks, int parentId) {
    auto controlId = ui::WindowRuntime_AllocateElementId(window, requestedElementId);
    return AddControl(std::make_unique<UIImageControl>(window.id, generation, controlId, name, static_cast<short>(x), static_cast<short>(y), static_cast<short>(w), static_cast<short>(h), visible ? 1 : 0, data, MakeLuaElementEvents(callbacks)), parentId);
}

UIControl* LuaWindow::AddAnime(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::AnimeData data, LuaCallbackRefs callbacks, int parentId) {
    auto controlId = ui::WindowRuntime_AllocateElementId(window, requestedElementId);
    return AddControl(std::make_unique<UIAnimeControl>(window.id, generation, controlId, name, static_cast<short>(x), static_cast<short>(y), static_cast<short>(w), static_cast<short>(h), visible ? 1 : 0, data, MakeLuaElementEvents(callbacks)), parentId);
}

UIControl* LuaWindow::AddText(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::TextData data, LuaCallbackRefs callbacks, int parentId) {
    auto controlId = ui::WindowRuntime_AllocateElementId(window, requestedElementId);
    return AddControl(std::make_unique<UITextControl>(window.id, generation, controlId, name, static_cast<short>(x), static_cast<short>(y), static_cast<short>(w), static_cast<short>(h), visible ? 1 : 0, std::move(data), MakeLuaElementEvents(callbacks)), parentId);
}

UIControl* LuaWindow::AddPngImage(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::PngImageData data, LuaCallbackRefs callbacks, int parentId) {
    auto controlId = ui::WindowRuntime_AllocateElementId(window, requestedElementId);
    return AddControl(std::make_unique<UIPngImageControl>(window.id, generation, controlId, name, static_cast<short>(x), static_cast<short>(y), static_cast<short>(w), static_cast<short>(h), visible ? 1 : 0, std::move(data), MakeLuaElementEvents(callbacks)), parentId);
}

UIControl* LuaWindow::AddCanvas(std::wstring_view name, int x, int y, int w, int h, int visible, std::vector<CanvasShape> shapes, LuaCallbackRefs callbacks, int parentId) {
    auto controlId = ui::WindowRuntime_AllocateElementId(window, requestedElementId);
    return AddControl(std::make_unique<UICanvasControl>(window.id, generation, controlId, name, static_cast<short>(x), static_cast<short>(y), static_cast<short>(w), static_cast<short>(h), visible ? 1 : 0, std::move(shapes), MakeLuaElementEvents(callbacks)), parentId);
}

UIControl* LuaWindow::AddTextInput(std::wstring_view name, int x, int y, int w, int h, int visible, UIControl::TextInputData data, LuaCallbackRefs callbacks, int parentId) {
    auto controlId = ui::WindowRuntime_AllocateElementId(window, requestedElementId);
    return AddControl(std::make_unique<UITextInputControl>(window.id, generation, controlId, name, static_cast<short>(x), static_cast<short>(y), static_cast<short>(w), static_cast<short>(h), visible ? 1 : 0, std::move(data), MakeLuaElementEvents(callbacks)), parentId);
}

UIControl* LuaWindow::AddScrollView(std::wstring_view name, int x, int y, int w, int h, int visible, int contentHeight, int scrollY, int barWidth, int scrollStep, LuaCallbackRefs callbacks, int parentId) {
    auto controlId = ui::WindowRuntime_AllocateElementId(window, requestedElementId);
    return AddControl(std::make_unique<UIScrollViewControl>(window.id, generation, controlId, name, static_cast<short>(x), static_cast<short>(y), static_cast<short>(w), static_cast<short>(h), visible ? 1 : 0, contentHeight, scrollY, barWidth, scrollStep, MakeLuaElementEvents(callbacks)), parentId);
}

UIControl* LuaWindow::AddMapEx(std::wstring_view name, int x, int y, int w, int h, int visible, int mapX, int mapY, int mapId, int floor, LuaCallbackRefs callbacks, int parentId) {
    auto controlId = ui::WindowRuntime_AllocateElementId(window, requestedElementId);
    return AddControl(std::make_unique<UIMapExControl>(window.id, generation, controlId, name, static_cast<short>(x), static_cast<short>(y), static_cast<short>(w), static_cast<short>(h), visible ? 1 : 0, ResolveLuaMapX(mapX), ResolveLuaMapY(mapY), ResolveLuaMapId(mapId), ResolveLuaFloor(floor), MakeLuaElementEvents(callbacks)), parentId);
}

static std::optional<LuaWindow> ResolveWindow(const LuaWindowRef& ref) {
    auto* instance = ui::WindowRuntime_FindInstance(ref.winId);
    if (!instance || (instance->state & WIN_STATE_CLOSE)) return std::nullopt;
    return LuaWindow(*instance, nullptr);
}
ui::WindowElement* ResolveLuaElement(const LuaControlRef& ref) {
    auto* window = ui::WindowRuntime_FindInstance(ref.winId);
    return window && !(window->state & WIN_STATE_CLOSE) ? window->FindElementById(ref.controlId) : nullptr;
}
UIControl* ResolveControl(const LuaControlRef& ref) {
    auto* element = ResolveLuaElement(ref);
    return element ? element->component : nullptr;
}
bool IsLuaControlAttached(const LuaControlRef& ref) { return ResolveLuaElement(ref) != nullptr; }
static void PushWindowRef(lua_State* L, LuaWindowRef ref) {
    auto* data = static_cast<LuaWindowRef*>(lua_newuserdata(L, sizeof(LuaWindowRef)));
    *data = ref;
    luaL_setmetatable(L, CName(LuaWindowRef));
}

static void PushControlRef(lua_State* L, LuaControlRef ref) {
    auto data = static_cast<LuaControlRef*>(lua_newuserdata(L, sizeof(LuaControlRef)));
    *data = ref;
    luaL_setmetatable(L, CName(LuaControlRef));
}

static LuaWindowRef* CheckWindowRef(lua_State* L, int index) {
    return static_cast<LuaWindowRef*>(luaL_checkudata(L, index, CName(LuaWindowRef)));
}

static LuaControlRef* CheckControlRef(lua_State* L, int index) {
    return static_cast<LuaControlRef*>(luaL_checkudata(L, index, CName(LuaControlRef)));
}

static int LuaReadParentId(lua_State* L, int index, const LuaWindow* window) {
    index = LuaAbsIndex(L, index);
    int parentId = LuaGetIntField(L, index, "parentId", 0);
    lua_getfield(L, index, "parent");
    if (!lua_isnil(L, -1)) {
        auto parentRef = static_cast<LuaControlRef*>(luaL_checkudata(L, -1, CName(LuaControlRef)));
        if (!parentRef || !window || parentRef->winId != window->window.id) {
            lua_pop(L, 1);
            return -1;
        }
        parentId = parentRef->controlId;
    }
    lua_pop(L, 1);
    return parentId;
}

static LuaEventHandleRef* CheckEventHandleRef(lua_State* L, int index) {
    return static_cast<LuaEventHandleRef*>(luaL_checkudata(L, index, CName(LuaEventHandleRef)));
}

LuaFunctionEx(LuaEventHandleRef, Unregister) {
    auto ref = CheckEventHandleRef(L, 1);
    lua_pushboolean(L, LuaUnregisterEvent(L, ref ? ref->event : nullptr));
    return 1;
}

LuaFunctionEx(LuaEventHandleRef, Index) {
    auto ref = CheckEventHandleRef(L, 1);
    const char* key = lua_tostring(L, 2);
    if (!key) {
        lua_pushnil(L);
    } else if (strcmp(key, "Unregister") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaEventHandleRef, Unregister));
    } else if (strcmp(key, "active") == 0 || strcmp(key, "valid") == 0) {
        lua_pushboolean(L, ref && ref->event && ref->event->active);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

LuaFunctionEx(LuaEventHandleRef, ToString) {
    auto ref = static_cast<LuaEventHandleRef*>(lua_touserdata(L, 1));
    char buff[64] = {0};
    sprintf(buff, CName(LuaEventHandleRef) "_%p", ref ? ref->event : nullptr);
    lua_pushstring(L, buff);
    return 1;
}

static int CallWindowCallback(lua_State* L, int callbackRef, const char* name, LuaWindowRef windowRef) {
    if (callbackRef == LUA_REF_NONE) {
        return 0;
    }

    int top = lua_gettop(L);
    int result = 0;
    lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
    if (lua_isfunction(L, -1)) {
        PushWindowRef(L, windowRef);
        if (lua_pcall(L, 1, 1, 0) != 0) {
            m_log_iw(L"[LUA] call lua %hs error: %hs\n", name, lua_tostring(L, -1));
        } else if (!lua_isnil(L, -1)) {
            result = lua_toboolean(L, -1) ? 1 : 0;
        }
    }
    lua_settop(L, top);
    return result;
}

void LuaWindowBinding::update() { CallWindowCallback(owner, window.refUpdate, "LuaWindow.onUpdate", ref()); }
void LuaWindowBinding::draw() { CallWindowCallback(owner, window.refDraw, "LuaWindow.onDraw", ref()); }

static void LuaSetWindowVisible(LuaWindow* window, bool visible) {
    ui::WindowRuntime_SetVisible(window->window, visible);
}

static void LuaApplyWindowCallbacks(lua_State* L, int index, LuaWindow* window) {
    LuaApplyWindowExtras(L, index, window->window);
}

static void LuaApplyNewWindowParam(lua_State* L, int index, LuaWindow* window) {
    window->window.id = LuaGetIntField(L, index, "id", WIN_LUA_START);
    window->window.x = static_cast<short>(LuaGetIntField(L, index, "x", 0));
    window->window.y = static_cast<short>(LuaGetIntField(L, index, "y", 0));
    window->window.width = static_cast<short>(LuaGetIntField(L, index, "width", 0));
    window->window.height = static_cast<short>(LuaGetIntField(L, index, "height", 0));
    window->window.layer = static_cast<uint8_t>(LuaGetIntField(L, index, "layer", 4));
    window->window.state = LuaGetBoolField(L, index, "visible", true) ? WIN_STATE_ACTIVE : 0;
    if (auto ext = GetExtWindowById(window->window.id)) ext->dragMove = LuaGetIntField(L, index, "dragMove", 0);
    LuaApplyWindowCallbacks(L, index, window);
}

static void LuaApplyExistingWindowParam(lua_State* L, int index, LuaWindow* window) {
    if (LuaHasField(L, index, "x")) {
        window->window.x = static_cast<short>(LuaGetIntField(L, index, "x", window->window.x));
    }
    if (LuaHasField(L, index, "y")) {
        window->window.y = static_cast<short>(LuaGetIntField(L, index, "y", window->window.y));
    }
    if (LuaHasField(L, index, "width")) {
        window->window.width = static_cast<short>(LuaGetIntField(L, index, "width", window->window.width));
    }
    if (LuaHasField(L, index, "height")) {
        window->window.height = static_cast<short>(LuaGetIntField(L, index, "height", window->window.height));
    }
    if (LuaHasField(L, index, "layer"))
        window->window.layer = LuaGetIntField(L, index, "layer", window->window.layer);
    if (LuaHasField(L, index, "visible")) {
        LuaSetWindowVisible(window, LuaGetBoolField(L, index, "visible", window->window.id < WIN_MAX ? !window->window.scriptHidden : (window->window.state & WIN_STATE_ACTIVE) != 0));
    }
    if (LuaHasField(L, index, "dragMove")) {
        if (auto ext = GetExtWindowById(window->window.id)) ext->dragMove = LuaGetIntField(L, index, "dragMove", ext->dragMove);
    }
    LuaApplyWindowCallbacks(L, index, window);
}

template<typename T>
static bool LuaPushMapControlField(lua_State* L, T* map, const char* key) {
    if (strcmp(key, "mapX") == 0) {
        lua_pushinteger(L, map->mapX);
        return true;
    }
    if (strcmp(key, "mapY") == 0) {
        lua_pushinteger(L, map->mapY);
        return true;
    }
    if (strcmp(key, "mapId") == 0) {
        lua_pushinteger(L, map->mapId);
        return true;
    }
    if (strcmp(key, "floor") == 0) {
        lua_pushinteger(L, map->floor);
        return true;
    }
    if (strcmp(key, "mapWidth") == 0) {
        lua_pushinteger(L, map->mapWidth);
        return true;
    }
    if (strcmp(key, "mapHeight") == 0) {
        lua_pushinteger(L, map->mapHeight);
        return true;
    }
    if (strcmp(key, "mouseX") == 0) {
        lua_pushinteger(L, map->lastMouseX);
        return true;
    }
    if (strcmp(key, "mouseY") == 0) {
        lua_pushinteger(L, map->lastMouseY);
        return true;
    }
    if (strcmp(key, "clickX") == 0 || strcmp(key, "lastClickX") == 0) {
        lua_pushinteger(L, map->lastClickX);
        return true;
    }
    if (strcmp(key, "clickY") == 0 || strcmp(key, "lastClickY") == 0) {
        lua_pushinteger(L, map->lastClickY);
        return true;
    }
    return false;
}

static bool LuaPushMapControlField(lua_State* L, UIControl* control, const char* key) {
    if (control->type == UIControl::TYPE_MAP_EX) {
        return LuaPushMapControlField(L, static_cast<UIMapExControl*>(control), key);
    }
    return false;
}

template<typename T>
static void LuaApplyMapControlFields(lua_State* L, T* map) {
    if (LuaHasField(L, 2, "mapX")) {
        map->mapX = ResolveLuaMapX(LuaGetIntField(L, 2, "mapX", -1));
    }
    if (LuaHasField(L, 2, "mapY")) {
        map->mapY = ResolveLuaMapY(LuaGetIntField(L, 2, "mapY", -1));
    }
    if (LuaHasField(L, 2, "mapId")) {
        map->mapId = ResolveLuaMapId(LuaGetIntField(L, 2, "mapId", -1));
    }
    if (LuaHasField(L, 2, "floor")) {
        map->floor = ResolveLuaFloor(LuaGetIntField(L, 2, "floor", -1));
    }
}

static void LuaPushControlField(lua_State* L, UIControl* control, const char* key) {
    if (strcmp(key, "valid") == 0) {
        lua_pushboolean(L, control != nullptr);
        return;
    }
    if (!control) {
        lua_pushnil(L);
        return;
    }
    if (LuaPushElementProperty(L, control->element(), key)) return;
    if (LuaPushMapControlField(L, control, key)) {
        return;
    }
    if (strcmp(key, "id") == 0 || strcmp(key, "controlId") == 0) {
        lua_pushinteger(L, control->id);
    } else if (strcmp(key, "name") == 0) {
        Push(L, control->name);
    } else if (strcmp(key, "x") == 0) {
        lua_pushinteger(L, control->element().x);
    } else if (strcmp(key, "y") == 0) {
        lua_pushinteger(L, control->element().y);
    } else if (strcmp(key, "width") == 0) {
        lua_pushinteger(L, control->element().width);
    } else if (strcmp(key, "height") == 0) {
        lua_pushinteger(L, control->element().height);
    } else if (strcmp(key, "visible") == 0) {
        lua_pushboolean(L, control->element().visible == 1);
    } else if (strcmp(key, "type") == 0) {
        lua_pushinteger(L, control->type);
    } else if (strcmp(key, "mouseState") == 0) {
        lua_pushinteger(L, control->mouseState);
    } else if (strcmp(key, "hitable") == 0) {
        lua_pushboolean(L, control->hitable);
    } else if (strcmp(key, "parentId") == 0) {
        lua_pushinteger(L, control->parentId);
    } else if (control->type == UIControl::TYPE_SCROLL_VIEW) {
        auto view = static_cast<UIScrollViewControl*>(control);
        if (strcmp(key, "scrollY") == 0) {
            lua_pushinteger(L, view->scrollY);
        } else if (strcmp(key, "contentHeight") == 0) {
            lua_pushinteger(L, view->contentHeight);
        } else if (strcmp(key, "maxScrollY") == 0) {
            lua_pushinteger(L, view->MaxScrollY());
        } else if (strcmp(key, "barWidth") == 0) {
            lua_pushinteger(L, view->barWidth);
        } else if (strcmp(key, "scrollStep") == 0) {
            lua_pushinteger(L, view->scrollStep);
        } else {
            lua_pushnil(L);
        }
    } else if (control->type == UIControl::TYPE_IMAGE && strcmp(key, "image") == 0) {
        const auto* data = control->imageData();
        if (data) {
            lua_pushinteger(L, data->imageNo);
        } else {
            lua_pushnil(L);
        }
    } else if (control->type == UIControl::TYPE_ANIME) {
        const auto* data = control->animeData();
        if (!data) {
            lua_pushnil(L);
            return;
        }
        if (strcmp(key, "animeNo") == 0) {
            lua_pushinteger(L, data->animeNo);
        } else if (strcmp(key, "action") == 0) {
            lua_pushinteger(L, data->action);
        } else if (strcmp(key, "dir") == 0) {
            lua_pushinteger(L, data->dir);
        } else if (strcmp(key, "revertPlay") == 0 || strcmp(key, "revert") == 0) {
            lua_pushinteger(L, data->revertPlay);
        } else {
            lua_pushnil(L);
        }
    } else if (control->type == UIControl::TYPE_PNG_IMAGE) {
        const auto* data = control->pngImageData();
        if (!data) {
            lua_pushnil(L);
            return;
        }
        if (strcmp(key, "image") == 0) {
            Push(L, data->image.path);
        } else if (strcmp(key, "imageHover") == 0) {
            Push(L, data->imageHover.path);
        } else if (strcmp(key, "imagePress") == 0) {
            Push(L, data->imagePress.path);
        } else if (strcmp(key, "color") == 0) {
            LuaPushColor(L, data->color);
        } else if (strcmp(key, "imageRect") == 0) {
            LuaPushPngRect(L, data->image);
        } else if (strcmp(key, "imageHoverRect") == 0) {
            LuaPushPngRect(L, data->imageHover);
        } else if (strcmp(key, "imagePressRect") == 0) {
            LuaPushPngRect(L, data->imagePress);
        } else {
            lua_pushnil(L);
        }
    } else if (control->type == UIControl::TYPE_TEXT && strcmp(key, "text") == 0) {
        const auto* data = control->textData();
        Push(L, data ? std::wstring_view(data->text) : std::wstring_view{});
    } else if (control->type == UIControl::TYPE_TEXT_INPUT) {
        const auto* data = control->textInputData();
        if (!data) {
            lua_pushnil(L);
            return;
        }
        if (strcmp(key, "text") == 0) {
            Push(L, data->context.text);
        } else if (strcmp(key, "maxLength") == 0) {
            lua_pushinteger(L, data->maxLength);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
    }
}

static void LuaPushWindowField(lua_State* L, const std::optional<LuaWindow>& window, const char* key) {
    if (strcmp(key, "valid") == 0) {
        lua_pushboolean(L, window.has_value());
        return;
    }
    if (!window) {
        lua_pushnil(L);
        return;
    }
    if (LuaPushWindowProperty(L, window->window, key)) return;
    if (strcmp(key, "id") == 0 || strcmp(key, "winId") == 0) {
        lua_pushinteger(L, window->window.id);
    } else if (strcmp(key, "state") == 0) {
        lua_pushinteger(L, window->window.state);
    } else if (strcmp(key, "visible") == 0) {
        lua_pushboolean(L, window->window.id < WIN_MAX ? !window->window.scriptHidden : (window->window.state & WIN_STATE_ACTIVE) != 0);
    } else if (strcmp(key, "count") == 0) {
        lua_pushinteger(L, static_cast<int>(window->window.sortedMap.size()));
    } else if (strcmp(key, "x") == 0) {
        lua_pushinteger(L, window->window.x);
    } else if (strcmp(key, "y") == 0) {
        lua_pushinteger(L, window->window.y);
    } else if (strcmp(key, "width") == 0) {
        lua_pushinteger(L, window->window.width);
    } else if (strcmp(key, "height") == 0) {
        lua_pushinteger(L, window->window.height);
    } else if (strcmp(key, "layer") == 0) {
        lua_pushinteger(L, window->window.layer);
    } else if (strcmp(key, "dragMove") == 0) {
        const auto ext = GetExtWindowById(window->window.id);
        lua_pushinteger(L, ext ? ext->dragMove : 0);
    } else {
        lua_pushnil(L);
    }
}


LuaFunctionEx(LuaControlRef, AddPoint) {
    auto ref = CheckControlRef(L, 1);
    auto control = ResolveControl(*ref);
    if (!control || control->type != UIControl::TYPE_MAP_EX) {
        lua_pushboolean(L, false);
        return 1;
    }

    UIMapExControl::MapExPoint point{};
    if (!LuaReadMapPoint(L, 2, point)) {
        lua_pushboolean(L, false);
        return 1;
    }

    int index = static_cast<UIMapExControl*>(control)->AddPoint(std::move(point));
    lua_pushboolean(L, true);
    lua_pushinteger(L, index);
    return 2;
}

LuaFunctionEx(LuaControlRef, RemovePoints) {
    auto ref = CheckControlRef(L, 1);
    auto control = ResolveControl(*ref);
    if (!control || control->type != UIControl::TYPE_MAP_EX || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        lua_pushinteger(L, 0);
        return 1;
    }

    int mapX = static_cast<int>(lua_tointeger(L, 2));
    int mapY = static_cast<int>(lua_tointeger(L, 3));
    int removed = static_cast<UIMapExControl*>(control)->RemovePoints(mapX, mapY);
    lua_pushinteger(L, removed);
    return 1;
}

LuaFunctionEx(LuaControlRef, GetPoints) {
    auto ref = CheckControlRef(L, 1);
    auto control = ResolveControl(*ref);
    if (!control || control->type != UIControl::TYPE_MAP_EX) {
        lua_newtable(L);
        return 1;
    }

    const auto& points = static_cast<UIMapExControl*>(control)->GetPoints();
    lua_createtable(L, static_cast<int>(points.size()), 0);
    for (size_t i = 0; i < points.size(); ++i) {
        LuaPushMapPoint(L, points[i]);
        lua_rawseti(L, -2, static_cast<int>(i + 1));
    }
    return 1;
}

LuaFunctionEx(LuaControlRef, Set) {
    if (LuaHasField(L, 2, "elementId") || LuaHasField(L, 2, "controlId") || LuaHasField(L, 2, "id"))
        return luaL_error(L, "elementId is read-only");
    auto ref = CheckControlRef(L, 1);
    if (!lua_istable(L, 2)) {
        lua_pushboolean(L, false);
        return 1;
    }
    auto control = ResolveControl(*ref);
    if (!control) {
        auto* element = ResolveLuaElement(*ref);
        if (!element) { lua_pushboolean(L, false); return 1; }
        LuaApplyNativeElementTable(L, 2, *element);
        lua_pushboolean(L, true); return 1;
    }
    if (!control) {
        lua_pushboolean(L, false);
        return 1;
    }
    if (LuaHasField(L, 2, "name")) {
        control->name = LuaGetWideStringField(L, 2, "name", control->name);
    }
    if (LuaHasField(L, 2, "x")) {
        control->element().x = static_cast<short>(LuaGetIntField(L, 2, "x", control->element().x));
    }
    if (LuaHasField(L, 2, "y")) {
        control->element().y = static_cast<short>(LuaGetIntField(L, 2, "y", control->element().y));
    }
    if (LuaHasField(L, 2, "width")) {
        control->element().width = static_cast<short>(LuaGetIntField(L, 2, "width", control->element().width));
    }
    if (LuaHasField(L, 2, "height")) {
        control->element().height = static_cast<short>(LuaGetIntField(L, 2, "height", control->element().height));
    }
    if (LuaHasField(L, 2, "visible")) {
        control->element().visible = LuaGetBoolField(L, 2, "visible", control->element().visible == 1) ? 1 : 0;
        if (control->element().visible != 1) {
            control->blurTextInput();
        }
    }
    if (LuaHasField(L, 2, "hitable")) {
        control->hitable = LuaGetBoolField(L, 2, "hitable", control->hitable);
        if (!control->hitable) {
            control->blurTextInput();
        }
    }
    if (control->type == UIControl::TYPE_MAP_EX) {
        LuaApplyMapControlFields(L, static_cast<UIMapExControl*>(control));
    } else if (control->type == UIControl::TYPE_SCROLL_VIEW) {
        auto view = static_cast<UIScrollViewControl*>(control);
        if (LuaHasField(L, 2, "contentHeight")) {
            view->SetContentHeight(LuaGetIntField(L, 2, "contentHeight", view->contentHeight));
        }
        if (LuaHasField(L, 2, "scrollY")) {
            view->SetScrollY(LuaGetIntField(L, 2, "scrollY", view->scrollY));
        }
        if (LuaHasField(L, 2, "scrollStep")) {
            view->SetScrollStep(LuaGetIntField(L, 2, "scrollStep", view->scrollStep));
        }
        if (LuaHasField(L, 2, "barWidth")) {
            view->barWidth = LuaGetIntField(L, 2, "barWidth", view->barWidth);
            if (view->barWidth <= 0) {
                view->barWidth = 8;
            }
        }
    } else if (control->type == UIControl::TYPE_CANVAS) {
        if (LuaHasField(L, 2, "shapes")) {
            static_cast<UICanvasControl*>(control)->SetShapes(LuaReadCanvasShapes(L, 2));
        }
    } else if (auto imageData = control->imageData()) {
        int image = LuaGetIntField(L, 2, "image", LuaGetIntField(L, 2, "id", imageData->imageNo));
        imageData->imageNo = image;
        if (LuaHasField(L, 2, "imageHover")) {
            imageData->imageHover = LuaGetIntField(L, 2, "imageHover", imageData->imageHover);
        }
        if (LuaHasField(L, 2, "imagePress")) {
            imageData->imagePress = LuaGetIntField(L, 2, "imagePress", imageData->imagePress);
        }
        if (LuaHasField(L, 2, "color")) {
            imageData->color = LuaGetColorField(L, 2, "color", imageData->color);
        }
    } else if (auto animeData = control->animeData()) {
        LuaReadNumberIntField(L, 2, "animeNo", animeData->animeNo);
        LuaReadNumberIntField(L, 2, "action", animeData->action);
        LuaReadNumberIntField(L, 2, "dir", animeData->dir);
        if (LuaHasField(L, 2, "revertPlay") || LuaHasField(L, 2, "revert")) {
            animeData->revertPlay = LuaGetIntField(L, 2, "revertPlay", LuaGetIntField(L, 2, "revert", animeData->revertPlay));
        }
    } else if (auto pngImageData = control->pngImageData()) {
        LuaSetPngImageState(L, 2, "image", "imageRect", pngImageData->image);
        LuaSetPngImageState(L, 2, "imageHover", "imageHoverRect", pngImageData->imageHover);
        LuaSetPngImageState(L, 2, "imagePress", "imagePressRect", pngImageData->imagePress);
        if (LuaHasField(L, 2, "color")) {
            pngImageData->color = LuaGetColorField(L, 2, "color", pngImageData->color);
        }
    } else if (auto textData = control->textData()) {
        if (LuaHasField(L, 2, "text")) {
            textData->text = LuaGetWideStringField(L, 2, "text", textData->text);
        }
        if (LuaHasField(L, 2, "font") || LuaHasField(L, 2, "fontType")) {
            textData->style.font = static_cast<std::byte>(render::ResolveFontId(LuaGetIntField(L, 2, "font", LuaGetIntField(L, 2, "fontType", std::to_integer<uint8_t>(textData->style.font)))));
        }
        if (LuaHasField(L, 2, "color")) {
            textData->style.color = static_cast<std::byte>(LuaGetIntField(L, 2, "color", std::to_integer<uint8_t>(textData->style.color)) & 0xff);
        }
    } else if (auto textInputData = control->textInputData()) {
        if (LuaHasField(L, 2, "maxLength")) {
            textInputData->maxLength = ClampTextInputMaxLength(LuaGetIntField(L, 2, "maxLength", textInputData->maxLength));
            control->setTextInputText(textInputData->context.text);
        }
        if (LuaHasField(L, 2, "text")) {
            const auto value = LuaGetWideStringField(L, 2, "text", textInputData->context.text);
            control->setTextInputText(value.c_str());
        }
        if (LuaHasField(L, 2, "font") || LuaHasField(L, 2, "fontType")) {
            textInputData->font = static_cast<uint8_t>(render::ResolveFontId(LuaGetIntField(L, 2, "font", LuaGetIntField(L, 2, "fontType", textInputData->font))));
        }
        if (LuaHasField(L, 2, "color")) {
            textInputData->color = static_cast<uint8_t>(LuaGetIntField(L, 2, "color", textInputData->color) & 0xff);
        }
    }
    LuaApplyElementExtras(L, 2, *control);
    lua_pushboolean(L, true);
    return 1;
}

LuaFunctionEx(LuaControlRef, Index) {
    if (lua_gettop(L) != 2 || lua_type(L, 1) != LUA_TUSERDATA) {
        lua_pushnil(L);
        return 1;
    }
    auto ref = static_cast<LuaControlRef*>(lua_touserdata(L, 1));
    const char* key = lua_tostring(L, 2);
    if (!ref || !key) {
        lua_pushnil(L);
        return 1;
    }
    if (strcmp(key, "Set") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaControlRef, Set));
        return 1;
    }
    if (strcmp(key, "AddPoint") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaControlRef, AddPoint));
        return 1;
    }
    if (strcmp(key, "RemovePoints") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaControlRef, RemovePoints));
        return 1;
    }
    if (strcmp(key, "GetPoints") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaControlRef, GetPoints));
        return 1;
    }
    if (!ResolveControl(*ref)) return LuaElementRead(L);
    if (!std::strcmp(key, "elementId")) { Push(L, ref->controlId); return 1; }
    LuaPushControlField(L, ResolveControl(*ref), key);
    return 1;
}

LuaFunctionEx(LuaControlRef, ToString) {
    auto ref = static_cast<LuaControlRef*>(lua_touserdata(L, 1));
    if (!ref) {
        lua_pushstring(L, CName(LuaControlRef) "_invalid");
        return 1;
    }
    char buff[64] = {0};
    sprintf(buff, CName(LuaControlRef) "_%d_%u", ref->winId, ref->controlId);
    lua_pushstring(L, buff);
    return 1;
}

LuaFunctionEx(LuaWindowRef, Set) {
    auto ref = CheckWindowRef(L, 1);
    if (!lua_istable(L, 2)) {
        lua_pushboolean(L, false);
        return 1;
    }
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushboolean(L, false);
        return 1;
    }
    LuaApplyExistingWindowParam(L, 2, &*window);
    lua_pushboolean(L, true);
    return 1;
}

LuaFunctionEx(LuaWindowRef, Close) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushboolean(L, false);
        return 1;
    }
    CgWindowClose(window->window.id);
    lua_pushboolean(L, true);
    return 1;
}

LuaFunctionEx(LuaWindowRef, CheckKeyState) {
    auto ref = CheckWindowRef(L, 1);
    if (!ResolveWindow(*ref)) {
        lua_pushboolean(L, false);
        return 1;
    }

    WinMgrKeyPressOption option{LuaCheckVkCode(L, 2), {}, 0, nullptr};
    int stateArg = 4;
    if (lua_gettop(L) == 3) {
        stateArg = 3;
    } else {
        LuaReadVkList(L, 3, option.vkList);
    }

    option.stateMask = static_cast<int>(luaL_checkinteger(L, stateArg));
    if (option.stateMask <= 0 || option.stateMask > 0xff) {
        return luaL_argerror(L, stateArg, "StateMask must be in range 1..255");
    }
    lua_pushboolean(L, WinMgrKeyPressed(option));
    return 1;
}

LuaFunctionEx(LuaWindowRef, ShowTips) {
    auto ref = CheckWindowRef(L, 1);
    if (!ResolveWindow(*ref)) {
        lua_pushboolean(L, false);
        return 1;
    }
    const char* tips = luaL_optstring(L, 2, "");
    ui::detail::gHoverTextD68AE8 = text::FromGbkWide(tips);
    lua_pushboolean(L, true);
    return 1;
}

LuaFunctionEx(LuaWindowRef, DrawRect) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushboolean(L, false);
        return 1;
    }
    if (window->window.scriptHidden || (window->window.id >= WIN_MAX && !(window->window.state & WIN_STATE_ACTIVE))) {
        lua_pushboolean(L, false);
        return 1;
    }
    if (!lua_istable(L, 2)) {
        return luaL_argerror(L, 2, "param is not a table");
    }

    int x = LuaGetIntField(L, 2, "x", 0);
    int y = LuaGetIntField(L, 2, "y", 0);
    int w = LuaGetIntField(L, 2, "width", LuaGetIntField(L, 2, "w", 0));
    int h = LuaGetIntField(L, 2, "height", LuaGetIntField(L, 2, "h", 0));
    COLORREF color = LuaGetColorField(L, 2, "color", 0);
    if (w <= 0 || h <= 0) {
        lua_pushboolean(L, false);
        return 1;
    }
    // m_log_iw(L"draw rect: %d %d %d %d layer: %d color %x\n", window->window.x + x, window->window.y + y, w, h, window->window.layer, color);
    int index = RendererDrawRect(static_cast<std::int16_t>(window->window.x + x),
        static_cast<std::int16_t>(window->window.y + y),
        static_cast<std::int16_t>(w), static_cast<std::int16_t>(h), LAYER_UI, color);
    lua_pushboolean(L, true);
    lua_pushinteger(L, index);
    return 2;
}

// 统一按ID查找，不创建Lua绑定或元素；失效窗口和缺失元素均返回nil。
LuaFunctionEx(LuaWindowRef, FindElementById) {
    auto window = ResolveWindow(*CheckWindowRef(L, 1));
    const auto id = LuaReadElementId(L, 2);
    if (window) {
        if (auto* element = window->window.FindElementById(id)) {
            PushLuaElement(L, window->window, *element);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

LuaFunctionEx(LuaWindowRef, RemoveElement) {
    auto window = ResolveWindow(*CheckWindowRef(L, 1));
    const auto id = LuaReadElementId(L, 2);
    lua_pushboolean(L, window && ui::WindowRuntime_RemoveElement(window->window, id));
    return 1;
}

LuaFunctionEx(LuaWindowRef, ClearChildren) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushboolean(L, false);
        return 1;
    }
    window->ClearChildren();
    lua_pushboolean(L, true);
    return 1;
}

LuaFunctionEx(LuaWindowRef, AddImage) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_istable(L, 2)) {
        return luaL_argerror(L, 2, "param is not a table");
    }

    auto name = LuaGetWideStringField(L, 2, "name");
    int parentId = LuaReadParentId(L, 2, &*window);
    int image = LuaGetIntField(L, 2, "image", LuaGetIntField(L, 2, "id", -1));
    int imageHover = LuaGetIntField(L, 2, "imageHover", image);
    int imagePress = LuaGetIntField(L, 2, "imagePress", image);
    UIControl::ImageData data{image, imageHover, imagePress, LuaGetColorField(L, 2, "color", 0xffffffff)};
    window->requestedElementId = LuaElementIdField(L, 2);
    auto control = window->AddImage(name.c_str(), LuaGetIntField(L, 2, "x", 0), LuaGetIntField(L, 2, "y", 0), LuaGetIntField(L, 2, "width", 0), LuaGetIntField(L, 2, "height", 0), LuaGetBoolField(L, 2, "visible", true) ? 1 : 0, data, LuaReadCallbacks(L, 2), parentId);
    if (!control) {
        lua_pushnil(L);
        return 1;
    }
    control->hitable = LuaGetBoolField(L, 2, "hitable", true);
    LuaApplyElementExtras(L, 2, *control);
    PushControlRef(L, {window->window.id, static_cast<std::uint32_t>(control->id)});
    return 1;
}

LuaFunctionEx(LuaWindowRef, AddPngImage) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_istable(L, 2)) {
        return luaL_argerror(L, 2, "param is not a table");
    }

    auto name = LuaGetWideStringField(L, 2, "name");
    int parentId = LuaReadParentId(L, 2, &*window);
    auto data = LuaReadPngImageData(L, 2);
    window->requestedElementId = LuaElementIdField(L, 2);
    auto control = window->AddPngImage(name.c_str(), LuaGetIntField(L, 2, "x", 0), LuaGetIntField(L, 2, "y", 0), LuaGetIntField(L, 2, "width", 0), LuaGetIntField(L, 2, "height", 0), LuaGetBoolField(L, 2, "visible", true) ? 1 : 0, std::move(data), LuaReadCallbacks(L, 2), parentId);
    if (!control) {
        lua_pushnil(L);
        return 1;
    }
    control->hitable = LuaGetBoolField(L, 2, "hitable", true);
    LuaApplyElementExtras(L, 2, *control);
    PushControlRef(L, {window->window.id, static_cast<std::uint32_t>(control->id)});
    return 1;
}

LuaFunctionEx(LuaWindowRef, AddCanvas) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_istable(L, 2)) {
        return luaL_argerror(L, 2, "param is not a table");
    }

    auto name = LuaGetWideStringField(L, 2, "name");
    int parentId = LuaReadParentId(L, 2, &*window);
    window->requestedElementId = LuaElementIdField(L, 2);
    auto control = window->AddCanvas(name.c_str(), LuaGetIntField(L, 2, "x", 0), LuaGetIntField(L, 2, "y", 0), LuaGetIntField(L, 2, "width", LuaGetIntField(L, 2, "w", 0)), LuaGetIntField(L, 2, "height", LuaGetIntField(L, 2, "h", 0)), LuaGetBoolField(L, 2, "visible", true) ? 1 : 0, LuaReadCanvasShapes(L, 2), LuaReadCallbacks(L, 2), parentId);
    if (!control) {
        lua_pushnil(L);
        return 1;
    }
    control->hitable = LuaGetBoolField(L, 2, "hitable", false);
    LuaApplyElementExtras(L, 2, *control);
    PushControlRef(L, {window->window.id, static_cast<std::uint32_t>(control->id)});
    return 1;
}

LuaFunctionEx(LuaWindowRef, AddAnime) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_istable(L, 2)) {
        return luaL_argerror(L, 2, "param is not a table");
    }

    auto name = LuaGetWideStringField(L, 2, "name");
    int parentId = LuaReadParentId(L, 2, &*window);
    UIControl::AnimeData data{
        -1,
        0,
        0,
        LuaGetIntField(L, 2, "revertPlay", LuaGetIntField(L, 2, "revert", 0))
    };
    LuaReadNumberIntField(L, 2, "animeNo", data.animeNo);
    LuaReadNumberIntField(L, 2, "action", data.action);
    LuaReadNumberIntField(L, 2, "dir", data.dir);
    window->requestedElementId = LuaElementIdField(L, 2);
    auto control = window->AddAnime(name.c_str(), LuaGetIntField(L, 2, "x", 0), LuaGetIntField(L, 2, "y", 0), LuaGetIntField(L, 2, "width", 0), LuaGetIntField(L, 2, "height", 0), LuaGetBoolField(L, 2, "visible", true) ? 1 : 0, data, LuaReadCallbacks(L, 2), parentId);
    if (!control) {
        lua_pushnil(L);
        return 1;
    }
    control->hitable = LuaGetBoolField(L, 2, "hitable", true);
    LuaApplyElementExtras(L, 2, *control);
    PushControlRef(L, {window->window.id, static_cast<std::uint32_t>(control->id)});
    return 1;
}

LuaFunctionEx(LuaWindowRef, AddText) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_istable(L, 2)) {
        return luaL_argerror(L, 2, "param is not a table");
    }

    auto name = LuaGetWideStringField(L, 2, "name");
    int parentId = LuaReadParentId(L, 2, &*window);
    UIControl::TextData data{
        static_cast<uint8_t>(LuaGetIntField(L, 2, "color", 0) & 0xff),
        static_cast<uint8_t>(render::ResolveFontId(LuaGetIntField(L, 2, "font", LuaGetIntField(L, 2, "fontType", 0)))),
        LuaGetWideStringField(L, 2, "text")
    };
    window->requestedElementId = LuaElementIdField(L, 2);
    auto control = window->AddText(name.c_str(), LuaGetIntField(L, 2, "x", 0), LuaGetIntField(L, 2, "y", 0), LuaGetIntField(L, 2, "width", 0), LuaGetIntField(L, 2, "height", 0), LuaGetBoolField(L, 2, "visible", true) ? 1 : 0, std::move(data), LuaReadCallbacks(L, 2), parentId);
    if (!control) {
        lua_pushnil(L);
        return 1;
    }
    control->hitable = LuaGetBoolField(L, 2, "hitable", false);
    LuaApplyElementExtras(L, 2, *control);
    PushControlRef(L, {window->window.id, static_cast<std::uint32_t>(control->id)});
    return 1;
}

LuaFunctionEx(LuaWindowRef, AddTextInput) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_istable(L, 2)) {
        return luaL_argerror(L, 2, "param is not a table");
    }

    auto name = LuaGetWideStringField(L, 2, "name");
    int parentId = LuaReadParentId(L, 2, &*window);
    UIControl::TextInputData data{
        static_cast<uint8_t>(LuaGetIntField(L, 2, "color", 0) & 0xff),
        static_cast<uint8_t>(render::ResolveFontId(LuaGetIntField(L, 2, "font", LuaGetIntField(L, 2, "fontType", 0)))),
        ClampTextInputMaxLength(LuaGetIntField(L, 2, "maxLength", TEXT_INPUT_MAX_LENGTH)),
        LuaGetWideStringField(L, 2, "text")
    };
    window->requestedElementId = LuaElementIdField(L, 2);
    auto control = window->AddTextInput(name.c_str(), LuaGetIntField(L, 2, "x", 0), LuaGetIntField(L, 2, "y", 0), LuaGetIntField(L, 2, "width", 0), LuaGetIntField(L, 2, "height", 0), LuaGetBoolField(L, 2, "visible", true) ? 1 : 0, std::move(data), LuaReadCallbacks(L, 2), parentId);
    if (!control) {
        lua_pushnil(L);
        return 1;
    }
    control->hitable = LuaGetBoolField(L, 2, "hitable", true);
    LuaApplyElementExtras(L, 2, *control);
    PushControlRef(L, {window->window.id, static_cast<std::uint32_t>(control->id)});
    return 1;
}

LuaFunctionEx(LuaWindowRef, AddScrollView) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_istable(L, 2)) {
        return luaL_argerror(L, 2, "param is not a table");
    }

    auto name = LuaGetWideStringField(L, 2, "name");
    int parentId = LuaReadParentId(L, 2, &*window);
    window->requestedElementId = LuaElementIdField(L, 2);
    auto control = window->AddScrollView(
        name.c_str(),
        LuaGetIntField(L, 2, "x", 0),
        LuaGetIntField(L, 2, "y", 0),
        LuaGetIntField(L, 2, "width", 0),
        LuaGetIntField(L, 2, "height", 0),
        LuaGetBoolField(L, 2, "visible", true) ? 1 : 0,
        LuaGetIntField(L, 2, "contentHeight", LuaGetIntField(L, 2, "height", 0)),
        LuaGetIntField(L, 2, "scrollY", 0),
        LuaGetIntField(L, 2, "barWidth", 8),
        LuaGetIntField(L, 2, "scrollStep", 1),
        LuaReadCallbacks(L, 2),
        parentId);
    if (!control) {
        lua_pushnil(L);
        return 1;
    }
    control->hitable = LuaGetBoolField(L, 2, "hitable", true);
    LuaApplyElementExtras(L, 2, *control);
    PushControlRef(L, {window->window.id, static_cast<std::uint32_t>(control->id)});
    return 1;
}

LuaFunctionEx(LuaWindowRef, AddMapEx) {
    auto ref = CheckWindowRef(L, 1);
    auto window = ResolveWindow(*ref);
    if (!window) {
        lua_pushnil(L);
        return 1;
    }
    if (!lua_istable(L, 2)) {
        return luaL_argerror(L, 2, "param is not a table");
    }

    auto name = LuaGetWideStringField(L, 2, "name");
    int parentId = LuaReadParentId(L, 2, &*window);
    window->requestedElementId = LuaElementIdField(L, 2);
    auto control = window->AddMapEx(
        name.c_str(),
        LuaGetIntField(L, 2, "x", 0),
        LuaGetIntField(L, 2, "y", 0),
        LuaGetIntField(L, 2, "width", LuaGetIntField(L, 2, "w", 0)),
        LuaGetIntField(L, 2, "height", LuaGetIntField(L, 2, "h", 0)),
        LuaGetBoolField(L, 2, "visible", true) ? 1 : 0,
        LuaGetIntField(L, 2, "mapX", -1),
        LuaGetIntField(L, 2, "mapY", -1),
        LuaGetIntField(L, 2, "mapId", -1),
        LuaGetIntField(L, 2, "floor", -1),
        LuaReadCallbacks(L, 2),
        parentId);
    if (!control) {
        lua_pushnil(L);
        return 1;
    }
    control->hitable = LuaGetBoolField(L, 2, "hitable", true);
    LuaApplyElementExtras(L, 2, *control);
    PushControlRef(L, {window->window.id, static_cast<std::uint32_t>(control->id)});
    return 1;
}

LuaFunctionEx(LuaWindowRef, Index) {
    if (lua_gettop(L) != 2 || lua_type(L, 1) != LUA_TUSERDATA) {
        lua_pushnil(L);
        return 1;
    }
    auto ref = static_cast<LuaWindowRef*>(lua_touserdata(L, 1));
    const char* key = lua_tostring(L, 2);
    if (!ref || !key) {
        lua_pushnil(L);
        return 1;
    }
    if (!std::strcmp(key, "id") || !std::strcmp(key, "winId")) { Push(L, ref->winId); return 1; }
    if (strcmp(key, "FindElementById") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, FindElementById));
    } else if (strcmp(key, "RemoveElement") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, RemoveElement));
    } else if (strcmp(key, "AddImage") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, AddImage));
    } else if (strcmp(key, "AddAnime") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, AddAnime));
    } else if (strcmp(key, "AddPngImage") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, AddPngImage));
    } else if (strcmp(key, "AddCanvas") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, AddCanvas));
    } else if (strcmp(key, "AddText") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, AddText));
    } else if (strcmp(key, "AddTextInput") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, AddTextInput));
    } else if (strcmp(key, "AddScrollView") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, AddScrollView));
    } else if (strcmp(key, "AddMap") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, AddMapEx));
    } else if (strcmp(key, "Set") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, Set));
    } else if (strcmp(key, "CheckKeyState") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, CheckKeyState));
    } else if (strcmp(key, "Close") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, Close));
    } else if (strcmp(key, "ClearChildren") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, ClearChildren));
    } else if (strcmp(key, "ShowTips") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, ShowTips));
    } else if (strcmp(key, "DrawRect") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaWindowRef, DrawRect));
    } else {
        LuaPushWindowField(L, ResolveWindow(*ref), key);
    }
    return 1;
}

LuaFunctionEx(LuaWindowRef, ToString) {
    auto ref = static_cast<LuaWindowRef*>(lua_touserdata(L, 1));
    if (!ref) {
        lua_pushstring(L, CName(LuaWindowRef) "_invalid");
        return 1;
    }
    char buff[64] = {0};
    sprintf(buff, CName(LuaWindowRef) "_%d", ref->winId);
    lua_pushstring(L, buff);
    return 1;
}

LuaFunctionEx(LuaWindowRef, NewIndex) {
    auto window = ResolveWindow(*CheckWindowRef(L, 1));
    if (!window) return luaL_error(L, "window is no longer valid");
    if (lua_type(L, 2) != LUA_TSTRING) return luaL_error(L, "property name must be a string");
    const auto* key = lua_tostring(L, 2);
    if (!std::strcmp(key, "id") || !std::strcmp(key, "winId")) return luaL_error(L, "property is read-only");
    if (lua_isnil(L, 3)) {
        int* ref = nullptr;
        if (!std::strcmp(key, "onUpdate") || !std::strcmp(key, "update")) ref = &window->window.refUpdate;
        else if (!std::strcmp(key, "onDraw") || !std::strcmp(key, "draw")) ref = &window->window.refDraw;
        else if (!std::strcmp(key, "onEvent")) ref = &window->window.refEvent;
        if (!ref) return luaL_error(L, "nil is only allowed for callbacks");
        if (*ref >= 0) EnsureLuaWindowBinding(L, window->window);
        LuaUnref(L, *ref); return 0;
    }
    lua_newtable(L); lua_pushvalue(L, 2); lua_pushvalue(L, 3); lua_rawset(L, -3);
    lua_replace(L, 2); lua_settop(L, 2);
    return Lua_LuaWindowRef_Set(L);
}
LuaFunctionEx(LuaControlRef, NewIndex) {
    if (!ResolveControl(*CheckControlRef(L, 1))) return LuaElementWrite(L);
    if (lua_type(L, 2) != LUA_TSTRING) return luaL_error(L, "property name must be a string");
    lua_newtable(L); lua_pushvalue(L, 2); lua_pushvalue(L, 3); lua_rawset(L, -3);
    // nil callback assignment is handled separately because tables cannot retain a nil key.
    if (lua_isnil(L, 3)) {
        auto* control = ResolveControl(*CheckControlRef(L, 1));
        if (!control) return luaL_error(L, "element is no longer valid");
        const auto* key = lua_tostring(L, 2);
        int* ref = nullptr;
        if (!std::strcmp(key, "onUpdate")) ref = &control->element().refUpdate;
        else if (!std::strcmp(key, "onDraw")) ref = &control->element().refDraw;
        else if (!std::strcmp(key, "onEvent")) ref = &control->element().refEvent;
        else if (!std::strcmp(key, "onClick")) ref = &control->element().refClick;
        else if (!std::strcmp(key, "onPress")) ref = &control->element().refPress;
        else if (!std::strcmp(key, "onHover")) ref = &control->element().refHover;
        else if (!std::strcmp(key, "onLeave")) ref = &control->element().refLeave;
        else if (!std::strcmp(key, "onDrag")) ref = &control->element().refDrag;
        else if (!std::strcmp(key, "onDrop")) ref = &control->element().refDrop;
        else if (!std::strcmp(key, "onChange")) ref = &control->element().refChange;
        else if (!std::strcmp(key, "onEnter")) ref = &control->element().refEnter;
        if (!ref) return luaL_error(L, "nil is only allowed for callbacks");
        if (*ref >= 0) EnsureLuaWindowBinding(L, *control->windowOwner);
        LuaUnref(L, *ref); return 0;
    }
    lua_replace(L, 2); lua_settop(L, 2);
    return Lua_LuaControlRef_Set(L);
}

void __declspec(noinline) RegCControl(lua_State* L) {

    luaL_newmetatable(L, CName(LuaControlRef));
    lua_pushcfunction(L, LuaCFunction(LuaControlRef, NewIndex));
    lua_setfield(L, -2, "__newindex");
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, LuaCFunction(LuaControlRef, Index));
    lua_rawset(L, -3);
    lua_pushstring(L, "__tostring");
    lua_pushcfunction(L, LuaCFunction(LuaControlRef, ToString));
    lua_rawset(L, -3);
    lua_settop(L, 0);

}

void __declspec(noinline) RegCWindow(lua_State* L) {

    luaL_newmetatable(L, CName(LuaWindowRef));
    lua_pushcfunction(L, LuaCFunction(LuaWindowRef, NewIndex));
    lua_setfield(L, -2, "__newindex");
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, LuaCFunction(LuaWindowRef, Index));
    lua_rawset(L, -3);
    lua_pushstring(L, "__tostring");
    lua_pushcfunction(L, LuaCFunction(LuaWindowRef, ToString));
    lua_rawset(L, -3);
    lua_settop(L, 0);

}

void __declspec(noinline) RegCEventHandle(lua_State* L) {

    luaL_newmetatable(L, CName(LuaEventHandleRef));
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, LuaCFunction(LuaEventHandleRef, Index));
    lua_rawset(L, -3);
    lua_pushstring(L, "__tostring");
    lua_pushcfunction(L, LuaCFunction(LuaEventHandleRef, ToString));
    lua_rawset(L, -3);
    lua_settop(L, 0);

}

static LuaBridgeHandleRef* CheckBridgeHandleRef(lua_State* L, int index) {
    return static_cast<LuaBridgeHandleRef*>(luaL_checkudata(L, index, CName(LuaBridgeHandleRef)));
}

LuaFunctionEx(LuaBridgeHandleRef, Unregister) {
    auto ref = CheckBridgeHandleRef(L, 1);
    lua_pushboolean(L, LuaBridgeUnregisterEvent(ref ? ref->event : nullptr));
    return 1;
}

LuaFunctionEx(LuaBridgeHandleRef, Index) {
    auto ref = CheckBridgeHandleRef(L, 1);
    const char* key = lua_tostring(L, 2);
    if (!key) {
        lua_pushnil(L);
    } else if (strcmp(key, "Unregister") == 0) {
        lua_pushcfunction(L, LuaCFunction(LuaBridgeHandleRef, Unregister));
    } else if (strcmp(key, "active") == 0 || strcmp(key, "valid") == 0) {
        lua_pushboolean(L, ref && ref->event && ref->event->active);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

LuaFunctionEx(LuaBridgeHandleRef, ToString) {
    auto ref = static_cast<LuaBridgeHandleRef*>(lua_touserdata(L, 1));
    char buff[64] = {0};
    sprintf(buff, CName(LuaBridgeHandleRef) "_%p", ref ? ref->event : nullptr);
    lua_pushstring(L, buff);
    return 1;
}

void __declspec(noinline) RegCBridgeHandle(lua_State* L) {

    luaL_newmetatable(L, CName(LuaBridgeHandleRef));
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, LuaCFunction(LuaBridgeHandleRef, Index));
    lua_rawset(L, -3);
    lua_pushstring(L, "__tostring");
    lua_pushcfunction(L, LuaCFunction(LuaBridgeHandleRef, ToString));
    lua_rawset(L, -3);
    lua_settop(L, 0);

}

LuaFunction(Bridge_On) {
    std::string header = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    LuaBridgeStateType stateType = LuaBridgeGetStateType(L);
    if (stateType == LuaBridgeStateType::Unknown) {
        return luaL_error(L, "Bridge only supports luaUI and autoBattle");
    }

    lua_pushvalue(L, 2);
    auto event = std::make_shared<LuaBridgeEventRegistration>();
    event->L = L;
    event->stateType = stateType;
    event->header = header;
    event->callbackRef = luaL_ref(L, LUA_REGISTRYINDEX);
    gLuaBridgeEventRegistrations.push_back(event);
    PushBridgeHandle(L, event);
    return 1;
}

LuaFunction(Bridge_Send) {
    std::string header = luaL_checkstring(L, 1);
    LuaBridgeStateType stateType = LuaBridgeGetStateType(L);
    if (stateType == LuaBridgeStateType::Unknown) {
        return luaL_error(L, "Bridge only supports luaUI and autoBattle");
    }

    std::vector<LuaBridgeValue> values;
    int top = lua_gettop(L);
    for (int i = 2; i <= top; ++i) {
        int type = lua_type(L, i);
        if (type == LUA_TNUMBER) {
            values.push_back({LuaBridgeValueType::Number, {}, lua_tonumber(L, i)});
        } else if (type == LUA_TSTRING) {
            size_t len = 0;
            const char* data = lua_tolstring(L, i, &len);
            values.push_back({LuaBridgeValueType::String, std::string(data ? data : "", len), 0});
        } else {
            return luaL_argerror(L, i, "data must be string or number");
        }
    }

    std::vector<std::shared_ptr<LuaBridgeEventRegistration> > events;
    for (const auto& event: gLuaBridgeEventRegistrations) {
        if (!event || !event->active || event->callbackRef == LUA_REF_NONE || event->header != header) {
            continue;
        }
        if (LuaBridgeCanSendTo(stateType, event->stateType)) {
            events.push_back(event);
        }
    }
    for (const auto& event: events) {
        LuaBridgeCallEvent(event, header, values);
    }
    return 0;
}

void RegBridge(lua_State* L) {

    lua_newtable(L);
    lua_pushcfunction(L, SafeLuaCall<Bridge_On>);
    lua_setfield(L, -2, "On");
    lua_pushcfunction(L, SafeLuaCall<Bridge_Send>);
    lua_setfield(L, -2, "Send");
    lua_setglobal(L, "Bridge");

}

LuaFunction(WinMgr_newWindow) {
    if (!lua_istable(L, 1)) {
        return luaL_argerror(L, 1, "param is not a table");
    }
    int id = LuaGetIntField(L, 1, "id", -1);
    if (id <= WIN_NORMAL_END) {
        return luaL_argerror(L, 1, "id must be greater than WIN_NORMAL_END");
    }
    if (!LuaHasField(L, 1, "x") || !LuaHasField(L, 1, "y") || !LuaHasField(L, 1, "width") || !LuaHasField(L, 1, "height")) {
        return luaL_argerror(L, 1, "x, y, width and height are required");
    }

    auto existing = GetExtWindowById(id);
    if (existing) {
        auto view = LuaWindow(existing->window, L);
        auto* window = &view;
        LuaApplyExistingWindowParam(L, 1, window);
        window->window.state |= WIN_STATE_FOCUS;
        lua_pushinteger(L, 1);
        PushWindowRef(L, window->ref());
        return 2;
    }

    std::shared_ptr<UIWindowInstanceEx> windowBase = std::make_shared<ui::WindowExtension>();
    auto view = LuaWindow(windowBase->window, L);
    auto* window = &view;
    LuaApplyNewWindowParam(L, 1, window);
    WindowExOpen(windowBase);
    windowBase->dragMove = LuaGetIntField(L, 1, "dragMove", 0);
    lua_pushinteger(L, 0);
    PushWindowRef(L, window->ref());
    return 2;
}

LuaFunction(WinMgr_findWindow) {
    const int id = static_cast<int>(luaL_checkinteger(L, 1));
    auto* window = ui::WindowRuntime_FindInstance(id);
    if (!window || (window->state & WIN_STATE_CLOSE)) { lua_pushnil(L); return 1; }
    PushWindowRef(L, {id});
    return 1;
}

LuaFunction(WinMgr_close) {
    int id = static_cast<int>(lua_tointeger(L, 1));
    CgWindowClose(id);
    return 0;
}

LuaFunction(WinMgr_autoCopilot) {
    int x = static_cast<int>(luaL_checkinteger(L, 1));
    int y = static_cast<int>(luaL_checkinteger(L, 2));
    startCopilot(x, y);
    return 0;
}

LuaFunction(WinMgr_stopCopilot) {
    static_cast<void>(L);
    stopCopilot();
    return 0;
}

LuaFunction(WinMgr_getAutoCopliotState) {
    lua_pushinteger(L, getAutoCopliotState());
    return 1;
}

LuaFunction(WinMgr_getAutoCopliotRoute) {
    auto route = getAutoCopliotRoute();
    lua_newtable(L);
    for (size_t i = 0; i < route.size(); ++i) {
        lua_newtable(L);
        lua_pushinteger(L, route[i].first);
        lua_setfield(L, -2, "x");
        lua_pushinteger(L, route[i].second);
        lua_setfield(L, -2, "y");
        lua_rawseti(L, -2, static_cast<int>(i + 1));
    }
    return 1;
}

LuaFunction(WinMgr_focus) {
    int id = static_cast<int>(lua_tointeger(L, 1));
    if (id >= 0 && id < WIN_MAX) {
        if (gWindowState[id].window) {
            gWindowState[id].window->state |= WIN_STATE_FOCUS;
            lua_pushinteger(L, 1);
            return 1;
        }
    } else {
        auto ptr = GetExtWindowById(id);
        if (ptr) {
            ptr->window.state |= WIN_STATE_FOCUS;
            lua_pushinteger(L, 1);
            return 1;
        }
    }
    lua_pushinteger(L, 0);
    return 1;
}


static void LuaCallPacketRecvEvent(const std::shared_ptr<LuaEventRegistration>& event, const char* packetHeader, char** params) {
    if (!event || !event->active || event->callbackRef == LUA_REF_NONE) {
        return;
    }
    lua_State* L = event->owner;
    int top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, event->callbackRef);
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, packetHeader);
        lua_newtable(L);
        if (params) {
            for (int i = 1; params[i]; ++i) {
                lua_pushstring(L, params[i]);
                lua_rawseti(L, -2, i);
            }
        }
        if (lua_pcall(L, 2, 0, 0) != 0) {
            m_log_iw(L"[LUA] call lua WinMgr.OnPacketRecv error: %hs\n", lua_tostring(L, -1));
        }
    }
    lua_settop(L, top);
}

static void LuaCallPacketSendEvent(const std::shared_ptr<LuaEventRegistration>& event, const char* packetHeader, const char* data, int len) {
    if (!event || !event->active || event->callbackRef == LUA_REF_NONE) {
        return;
    }
    lua_State* L = event->owner;
    int top = lua_gettop(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, event->callbackRef);
    if (lua_isfunction(L, -1)) {
        int dataLen = len > 0 ? len : 0;
        lua_pushstring(L, packetHeader);
        lua_pushlstring(L, data ? data : "", dataLen);
        lua_pushinteger(L, len);
        if (lua_pcall(L, 3, 0, 0) != 0) {
            m_log_iw(L"[LUA] call lua WinMgr.OnPacketSend error: %hs\n", lua_tostring(L, -1));
        }
    }
    lua_settop(L, top);
}

void LuaProcessWinMgrSceneStateChanged(int sceneType, int sceneState) {
    auto& options = WinMgrEventOptions(WinMgrEventType::SceneStateChanged);
    if (options.empty()) {
        return;
    }

    size_t count = options.size();
    for (size_t i = 0; i < count; ++i) {
        auto event = options[i];
        if (!event || !event->active || event->callbackRef == LUA_REF_NONE) {
            continue;
        }

        lua_State* L = event->owner;
        int top = lua_gettop(L);
        lua_rawgeti(L, LUA_REGISTRYINDEX, event->callbackRef);
        if (lua_isfunction(L, -1)) {
            lua_pushinteger(L, sceneType);
            lua_pushinteger(L, sceneState);
            if (lua_pcall(L, 2, 0, 0) != 0) {
                m_log_iw(L"[LUA] call lua WinMgr.OnSceneStateChanged error: %hs\n", lua_tostring(L, -1));
            }
        }
        lua_settop(L, top);
    }
}

void LuaProcessWinMgrWindowFocusChanged(int focusWinId, int blurWinId) {
    auto& options = WinMgrEventOptions(WinMgrEventType::WindowFocusChanged);
    if (options.empty()) {
        return;
    }

    size_t count = options.size();
    for (size_t i = 0; i < count; ++i) {
        auto event = options[i];
        if (!event || !event->active || event->callbackRef == LUA_REF_NONE) {
            continue;
        }

        lua_State* L = event->owner;
        int top = lua_gettop(L);
        lua_rawgeti(L, LUA_REGISTRYINDEX, event->callbackRef);
        if (lua_isfunction(L, -1)) {
            lua_pushinteger(L, focusWinId);
            lua_pushinteger(L, blurWinId);
            if (lua_pcall(L, 2, 0, 0) != 0) {
                m_log_iw(L"[LUA] call lua WinMgr.OnWindowFocusChanged error: %hs\n", lua_tostring(L, -1));
            }
        }
        lua_settop(L, top);
    }
}

void LuaProcessWinMgrWindowOpen(int winId) {
    auto* window = ui::WindowRuntime_FindInstance(winId);
    if (!window || (window->state & WIN_STATE_CLOSE)) return;
    const auto generation = window->generation;
    // 快照允许回调新增/注销订阅；新订阅从下一次打开事件开始接收。
    const auto events = WinMgrEventOptions(WinMgrEventType::WindowOpen);
    for (const auto& event : events) {
        window = ui::WindowRuntime_FindInstance(winId);
        if (!window || window->generation != generation || (window->state & WIN_STATE_CLOSE)) break;
        if (!event || !event->active || event->callbackRef == LUA_REF_NONE) continue;
        lua_State* L = event->owner;
        const int top = lua_gettop(L);
        lua_rawgeti(L, LUA_REGISTRYINDEX, event->callbackRef);
        if (lua_isfunction(L, -1)) {
            lua_pushinteger(L, winId);
            if (lua_pcall(L, 1, 0, 0) != 0) {
                m_log_iw(L"[LUA] call lua WinMgr.OnWindowOpen error: %hs\n", lua_tostring(L, -1));
            }
        }
        lua_settop(L, top);
    }
}

void LuaProcessWinMgrWindowClose(int winId) {
    auto& options = WinMgrEventOptions(WinMgrEventType::WindowClose);
    if (options.empty()) {
        return;
    }

    size_t count = options.size();
    for (size_t i = 0; i < count; ++i) {
        auto event = options[i];
        if (!event || !event->active || event->callbackRef == LUA_REF_NONE) {
            continue;
        }

        lua_State* L = event->owner;
        int top = lua_gettop(L);
        lua_rawgeti(L, LUA_REGISTRYINDEX, event->callbackRef);
        if (lua_isfunction(L, -1)) {
            lua_pushinteger(L, winId);
            if (lua_pcall(L, 1, 0, 0) != 0) {
                m_log_iw(L"[LUA] call lua WinMgr.OnWindowClose error: %hs\n", lua_tostring(L, -1));
            }
        }
        lua_settop(L, top);
    }
}

int LuaProcessWinMgrChatMessage(std::wstring_view message) {
    auto& options = WinMgrEventOptions(WinMgrEventType::ChatMessage);
    if (options.empty()) {
        return 0;
    }

    size_t count = options.size();
    for (size_t i = 0; i < count; ++i) {
        auto event = options[i];
        if (!event || !event->active || event->callbackRef == LUA_REF_NONE) {
            continue;
        }

        lua_State* L = event->owner;
        int top = lua_gettop(L);
        int handled = 0;
        lua_rawgeti(L, LUA_REGISTRYINDEX, event->callbackRef);
        if (lua_isfunction(L, -1)) {
            lua_pushstring(L, text::ToGbk(message).c_str());
            if (lua_pcall(L, 1, 1, 0) != 0) {
                m_log_iw(L"[LUA] call lua WinMgr.OnChatMessage error: %hs\n", lua_tostring(L, -1));
            } else if (!lua_isnil(L, -1)) {
                handled = static_cast<int>(lua_tointeger(L, -1));
            }
        }
        lua_settop(L, top);
        if (handled == 1) {
            return 1;
        }
    }
    return 0;
}

LuaFunction(WinMgr_onSceneStateChanged) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    auto event = LuaRefEvent(L, 1);
    WinMgrEventOptions(WinMgrEventType::SceneStateChanged).push_back(event);
    PushEventHandle(L, event);
    return 1;
}

LuaFunction(WinMgr_onWindowFocusChanged) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    auto event = LuaRefEvent(L, 1);
    WinMgrEventOptions(WinMgrEventType::WindowFocusChanged).push_back(event);
    PushEventHandle(L, event);
    return 1;
}

LuaFunction(WinMgr_onWindowOpen) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    auto event = LuaRefEvent(L, 1);
    WinMgrEventOptions(WinMgrEventType::WindowOpen).push_back(event);
    PushEventHandle(L, event);
    return 1;
}

LuaFunction(WinMgr_onWindowClose) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    auto event = LuaRefEvent(L, 1);
    WinMgrEventOptions(WinMgrEventType::WindowClose).push_back(event);
    PushEventHandle(L, event);
    return 1;
}

LuaFunction(WinMgr_onChatMessage) {
    luaL_checktype(L, 1, LUA_TFUNCTION);
    auto event = LuaRefEvent(L, 1);
    WinMgrEventOptions(WinMgrEventType::ChatMessage).push_back(event);
    PushEventHandle(L, event);
    return 1;
}

LuaFunction(WinMgr_onPacketRecv) {
    std::string header = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    auto event = LuaRefEvent(L, 2);
    gWinMgrPacketRecvOptions.push_back({header, event});
    PushEventHandle(L, event);
    return 1;
}

LuaFunction(WinMgr_onPacketSend) {
    std::string header = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    auto event = LuaRefEvent(L, 2);
    gWinMgrPacketSendOptions.push_back({header, event});
    PushEventHandle(L, event);
    return 1;
}

static int LuaCheckVkCode(lua_State* L, int index) {
    int vk = static_cast<int>(luaL_checkinteger(L, index));
    if (vk < 0 || vk > 255) {
        return luaL_argerror(L, index, "VK code must be in range 0..255");
    }
    return vk;
}

static void LuaReadVkList(lua_State* L, int index, std::vector<int>& vkList) {
    index = LuaAbsIndex(L, index);
    if (lua_isnil(L, index)) {
        return;
    }
    if (!lua_istable(L, index)) {
        vkList.push_back(LuaCheckVkCode(L, index));
        return;
    }

    size_t count = lua_objlen(L, index);
    for (size_t i = 1; i <= count; ++i) {
        lua_rawgeti(L, index, static_cast<int>(i));
        vkList.push_back(LuaCheckVkCode(L, -1));
        lua_pop(L, 1);
    }
}

static bool WinMgrKeyPressed(const WinMgrKeyPressOption& option) {
    int state = static_cast<int>(platform::gKeyStates[option.vkMain]);
    if ((state & option.stateMask) == 0) {
        return false;
    }
    for (int vk: option.vkList) {
        if ((static_cast<int>(platform::gKeyStates[vk]) & KEY_DOWN) == 0) {
            return false;
        }
    }
    return true;
}

void LuaProcessWinMgrKeyPress() {
    if (gWinMgrKeyPressOptions.empty()) {
        return;
    }

    size_t count = gWinMgrKeyPressOptions.size();
    for (size_t i = 0; i < count; ++i) {
        const auto option = gWinMgrKeyPressOptions[i];
        auto event = option.event;
        if (!event || !event->active || event->callbackRef == LUA_REF_NONE || !WinMgrKeyPressed(option)) {
            continue;
        }

        lua_State* L = event->owner;
        int top = lua_gettop(L);
        lua_rawgeti(L, LUA_REGISTRYINDEX, event->callbackRef);
        if (lua_isfunction(L, -1) && lua_pcall(L, 0, 0, 0) != 0) {
            m_log_iw(L"[LUA] call lua WinMgr.OnKeyPress error: %hs\n", lua_tostring(L, -1));
        }
        lua_settop(L, top);
    }
}

LuaFunction(WinMgr_onKeyPress) {
    WinMgrKeyPressOption option{LuaCheckVkCode(L, 1), {}, 0, nullptr};
    int stateArg = 3;
    int callbackArg = 4;
    if (lua_isfunction(L, 3)) {
        stateArg = 2;
        callbackArg = 3;
    } else {
        LuaReadVkList(L, 2, option.vkList);
    }

    option.stateMask = static_cast<int>(luaL_checkinteger(L, stateArg));
    if (option.stateMask <= 0 || option.stateMask > 0xff) {
        return luaL_argerror(L, stateArg, "StateMask must be in range 1..255");
    }
    luaL_checktype(L, callbackArg, LUA_TFUNCTION);
    option.event = LuaRefEvent(L, callbackArg);
    gWinMgrKeyPressOptions.push_back(option);
    PushEventHandle(L, option.event);
    return 1;
}

LuaFunction(WinMgr_sendPacket) {
    int top = lua_gettop(L);
    if (top == 1) {
        size_t len = 0;
        const char* data = luaL_checklstring(L, 1, &len);
        if (len > 0 && data[len - 1] == '\n') {
            const auto result = cg::network::GetGameConnection().SendPacket( data, static_cast<int>(len));
            lua_pushinteger(L, static_cast<lua_Integer>(result));
            return 1;
        }
        std::string packet(data, len);
        packet.push_back('\n');
        const auto result = cg::network::GetGameConnection().SendPacket( packet.c_str(), static_cast<int>(packet.size()));
        lua_pushinteger(L, static_cast<lua_Integer>(result));
        return 1;
    }

    const char* head = luaL_checkstring(L, 1);
    std::string packet = head;
    for (int i = 2; i <= top; ++i) {
        packet.push_back(' ');
        if (lua_type(L, i) == LUA_TNUMBER) {
            int value = static_cast<int>(lua_tointeger(L, i));
            packet += cg::network::GetGameConnection().EncodeInteger(value).c_str();
        } else if (lua_isstring(L, i)) {
            packet += cg::network::GetGameConnection().EscapeText(text::FromGbkWide(lua_tostring(L, i))).c_str();
        } else {
            return luaL_argerror(L, i, "data must be string or integer");
        }
    }
    packet.push_back('\n');

    const auto result = cg::network::GetGameConnection().SendPacket( packet.c_str(), static_cast<int>(packet.size()));
    lua_pushinteger(L, static_cast<lua_Integer>(result));
    return 1;
}

LuaFunction(WinMgr_playSe) {
    int seNo = static_cast<int>(luaL_checkinteger(L, 1));
    if (seNo < 0 || seNo > 500) {
        return luaL_argerror(L, 1, "seNo must be in range 0..500");
    }
    int panX = static_cast<int>(luaL_optinteger(L, 2, 320));
    int result = audio::PlaySe_52C450(seNo, panX);
    lua_pushinteger(L, result);
    return 1;
}

LuaFunction(WinMgr_loadAndPlayBGM) {
    int bgmNo = static_cast<int>(luaL_checkinteger(L, 1));
    if (bgmNo < 0 || bgmNo >= 512) {
        return luaL_argerror(L, 1, "bgmNo must be in range 0..511");
    }
    const int result = audio::LoadMappedBgm(bgmNo);
    lua_pushinteger(L, result);
    return 1;
}

LuaFunction(WinMgr_playFixedBGM) {
    int type = static_cast<int>(luaL_checkinteger(L, 1));
    if (type < 0 || type > 1) {
        return luaL_argerror(L, 1, "type must be in range 0..1");
    }
    DWORD repeats = static_cast<DWORD>(luaL_checkinteger(L, 2));
    int result = audio::PlayFixedMusic_455280(type, repeats);
    lua_pushinteger(L, result);
    return 1;
}

LuaFunction(WinMgr_cliSendMsg) {
    const auto msg = text::FromGbkWide(luaL_checkstring(L, 1));
    MsgOption option{
        text::FromGbkWide(luaL_optstring(L, 4, "")),
        static_cast<int>(luaL_optinteger(L, 2, 0)),
        static_cast<int>(luaL_optinteger(L, 3, 0))
    };
    CliSendMsg(msg, option);
    return 0;
}

LuaFunction(WinMgr_nrEncode62) {
    int value = static_cast<int>(luaL_checkinteger(L, 1));
    lua_pushstring(L, cg::network::GetGameConnection().EncodeInteger(value).c_str());
    return 1;
}

LuaFunction(WinMgr_nrDecode62) {
    const char* value = luaL_checkstring(L, 1);
    lua_pushinteger(L, network::Base62Decode_52D050(value));
    return 1;
}

LuaFunction(WinMgr_nrEncode16) {
    int value = static_cast<int>(luaL_checkinteger(L, 1));
    char buff[32] = {};
    sprintf_s(buff, "%X", value);
    lua_pushstring(L, buff);
    return 1;
}

LuaFunction(WinMgr_nrDecode16) {
    const char* value = luaL_checkstring(L, 1);
    lua_pushinteger(L, static_cast<int>(strtol(value, nullptr, 16)));
    return 1;
}

LuaFunction(WinMgr_nrEncodeString) {
    const char* value = luaL_checkstring(L, 1);
    lua_pushstring(L, cg::network::GetGameConnection().EscapeText(text::FromGbkWide(value)).c_str());
    return 1;
}

LuaFunction(WinMgr_nrDecodeString) {
    const char* value = luaL_checkstring(L, 1);
    lua_pushstring(L, cg::network::GetGameConnection().UnescapeText(value).c_str());
    return 1;
}

void RegWinMgr(lua_State* L) {

    lua_newtable(L);
    lua_pushcfunction(L, SafeLuaCall<WinMgr_newWindow>);
    lua_setfield(L, -2, "NewWindow");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_findWindow>);
    lua_setfield(L, -2, "FindWindow");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_focus>);
    lua_setfield(L, -2, "Focus");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_close>);
    lua_setfield(L, -2, "Close");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_autoCopilot>);
    lua_setfield(L, -2, "AutoCopilot");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_stopCopilot>);
    lua_setfield(L, -2, "StopCopilot");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_getAutoCopliotState>);
    lua_setfield(L, -2, "GetAutoCopliotState");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_getAutoCopliotRoute>);
    lua_setfield(L, -2, "GetAutoCopliotRoute");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_onWindowFocusChanged>);
    lua_setfield(L, -2, "OnWindowFocusChanged");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_onWindowOpen>);
    lua_setfield(L, -2, "OnWindowOpen");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_onWindowClose>);
    lua_setfield(L, -2, "OnWindowClose");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_onSceneStateChanged>);
    lua_setfield(L, -2, "OnSceneStateChanged");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_onChatMessage>);
    lua_setfield(L, -2, "OnChatMessage");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_onPacketRecv>);
    lua_setfield(L, -2, "OnPacketRecv");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_onPacketSend>);
    lua_setfield(L, -2, "OnPacketSend");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_onKeyPress>);
    lua_setfield(L, -2, "OnKeyPress");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_sendPacket>);
    lua_setfield(L, -2, "SendPacket");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_playSe>);
    lua_setfield(L, -2, "PlaySe");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_loadAndPlayBGM>);
    lua_setfield(L, -2, "LoadAndPlayBGM");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_playFixedBGM>);
    lua_setfield(L, -2, "PlayFixedBGM");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_cliSendMsg>);
    lua_setfield(L, -2, "CliSendMsg");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_nrEncode62>);
    lua_setfield(L, -2, "NrEncode62");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_nrDecode62>);
    lua_setfield(L, -2, "NrDecode62");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_nrEncode16>);
    lua_setfield(L, -2, "NrEncode16");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_nrDecode16>);
    lua_setfield(L, -2, "NrDecode16");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_nrEncodeString>);
    lua_setfield(L, -2, "NrEncodeString");
    lua_pushcfunction(L, SafeLuaCall<WinMgr_nrDecodeString>);
    lua_setfield(L, -2, "NrDecodeString");
    lua_setglobal(L, "WinMgr");

}

void LoadLuaUIEntry()
{
    auto* L = GetLuaState();
    if (!std::filesystem::exists(L"luaUI/init.lua")) { m_log_iw(L"[Lua] luaUI/init.lua not found\n"); return; }
    const int top = lua_gettop(L);
    if (luaL_dofile(L, "luaUI/init.lua") != 0) m_log_iw(L"[Lua] UI init failed: %hs\n", lua_tostring(L, -1));
    lua_settop(L, top);
}
void RegisterLuaUI(lua_State* L)
{
    RegUIConst(L);
    RegCControl(L);
    RegCWindow(L);
    RegisterLuaUIProxy(L);
    RegCEventHandle(L);
    RegCBridgeHandle(L);
    RegBridge(L);
    RegWinMgr(L);
    RegLuaAudio(L);
    RegGraphic(L);
    RegLuaIO(L);
    if (L == GetLuaState()) LuaBridgeSetLuaUIState(L); else LuaBridgeSetAutoBattleState(L);
}
void LuaReceivePacket(cg::network::CgConnection& networkConnection, const char* header, char** arguments)
{
    const auto connection = networkConnection.Id();
    // 先快照注册集合和参数，允许回调注册/注销或重入网络发送。
    const auto options = gWinMgrPacketRecvOptions;
    std::vector<std::string> values;
    for (int i = 0; arguments && arguments[i]; ++i) values.emplace_back(arguments[i]);
    std::vector<char*> pointers;
    for (auto& value : values) pointers.push_back(value.data());
    pointers.push_back(nullptr);
    const std::string packetHeader(header);
    for (const auto& option : options) {
        if (connection != networkConnection.Id() || !networkConnection.Active()) break;
        if (option.header == packetHeader) LuaCallPacketRecvEvent(option.event, packetHeader.c_str(), pointers.data());
    }
}
void LuaSendPacket(cg::network::CgConnection& networkConnection, std::string_view packet)
{
    const auto connection = networkConnection.Id();
    const auto options = gWinMgrPacketSendOptions;
    std::string matched;
    bool found = false;
    for (const auto& option : options) if (packet.starts_with(option.header)) { matched = option.header; found = true; break; }
    if (!found) return;
    const std::string bytes(packet.substr(matched.size()));
    for (const auto& option : options) {
        if (connection != networkConnection.Id() || !networkConnection.Active()) break;
        if (option.header == matched) LuaCallPacketSendEvent(option.event, matched.c_str(), bytes.data(), static_cast<int>(bytes.size()));
    }
}
void AddLuaMouseWheel(int delta) { ui::WindowAddMouseWheelDelta(delta); }
void ShutdownLuaUIEvents()
{
    for (const auto& event : gLuaEventRegistrations) { event->active = false; LuaUnref(event->owner, event->callbackRef); }
    for (const auto& event : gLuaBridgeEventRegistrations) { event->active = false; LuaUnref(event->L, event->callbackRef); }
    gWinMgrKeyPressOptions.clear();
    gWinMgrPacketRecvOptions.clear();
    gWinMgrPacketSendOptions.clear();
    gWinMgrEventOptions.clear();
    gLuaEventRegistrations.clear();
    gLuaBridgeEventRegistrations.clear();
    gLuaBridgeStateTypes.clear();
    ui::WindowConsumeMouseWheelDelta();
}
} // namespace cg::script
