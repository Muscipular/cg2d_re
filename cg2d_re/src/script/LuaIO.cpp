#include "LuaUI.hpp"
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <cmath>
#include <limits>
#include <cerrno>
#include <cstdlib>

namespace cg::script {
namespace lua_io_detail {
constexpr int kMaxYamlDepth = 64;

int LuaAbsIndex(lua_State* L, int index) {
    if (index > 0 || index <= LUA_REGISTRYINDEX) {
        return index;
    }
    return lua_gettop(L) + index + 1;
}

bool IsValidLuaFileName(const char* name, size_t len) {
    if (!name || len == 0) {
        return false;
    }
    if ((len == 1 && name[0] == '.') || (len == 2 && name[0] == '.' && name[1] == '.')) {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = static_cast<unsigned char>(name[i]);
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-') {
            continue;
        }
        return false;
    }
    return true;
}

bool BuildLuaFilePath(lua_State* L, int index, std::filesystem::path& path) {
    size_t len = 0;
    const char* name = luaL_checklstring(L, index, &len);
    if (!IsValidLuaFileName(name, len)) {
        return false;
    }
    std::error_code ec;
    auto cwd = std::filesystem::current_path(ec);
    if (ec) {
        return false;
    }
    path = cwd / "data" / "luaUI" / std::string(name, len);
    return true;
}

bool EnsureLuaDataDir(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    return !ec;
}

void PushReadFail(lua_State* L) {
    lua_pushboolean(L, 0);
    lua_pushnil(L);
}

bool ReadFileData(const std::filesystem::path& path, std::string& data) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }
    data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return !file.bad();
}

bool WriteFileData(const std::filesystem::path& path, const char* data, size_t len) {
    if (!EnsureLuaDataDir(path)) {
        return false;
    }
    if (len > static_cast<size_t>(std::numeric_limits<std::streamsize>::max())) {
        return false;
    }
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        return false;
    }
    file.write(data, static_cast<std::streamsize>(len));
    file.close();
    return !file.fail();
}

bool IsTrueText(const std::string& text) {
    return text == "true" || text == "True" || text == "TRUE";
}

bool IsFalseText(const std::string& text) {
    return text == "false" || text == "False" || text == "FALSE";
}

bool TryParseNumber(const std::string& text, double& value) {
    if (text.empty()) {
        return false;
    }
    errno = 0;
    char* end = nullptr;
    value = std::strtod(text.c_str(), &end);
    return end && end != text.c_str() && *end == '\0' && errno != ERANGE && std::isfinite(value);
}

bool PushYamlNode(lua_State* L, const YAML::Node& node, int depth) {
    if (depth > kMaxYamlDepth) {
        return false;
    }
    if (!node || node.IsNull()) {
        lua_pushnil(L);
        return true;
    }
    if (node.IsScalar()) {
        std::string value = node.Scalar();
        if (IsTrueText(value)) {
            lua_pushboolean(L, 1);
            return true;
        }
        if (IsFalseText(value)) {
            lua_pushboolean(L, 0);
            return true;
        }
        double number = 0;
        if (TryParseNumber(value, number)) {
            lua_pushnumber(L, static_cast<lua_Number>(number));
            return true;
        }
        lua_pushlstring(L, value.data(), value.size());
        return true;
    }
    if (node.IsSequence()) {
        lua_createtable(L, static_cast<int>(node.size()), 0);
        for (size_t i = 0; i < node.size(); ++i) {
            if (!PushYamlNode(L, node[i], depth + 1)) {
                lua_pop(L, 1);
                return false;
            }
            lua_rawseti(L, -2, static_cast<int>(i + 1));
        }
        return true;
    }
    if (node.IsMap()) {
        lua_createtable(L, 0, static_cast<int>(node.size()));
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string key = it->first.as<std::string>();
            lua_pushlstring(L, key.data(), key.size());
            if (!PushYamlNode(L, it->second, depth + 1)) {
                lua_pop(L, 2);
                return false;
            }
            lua_settable(L, -3);
        }
        return true;
    }
    return false;
}

bool IsIntegerNumber(double value) {
    return std::isfinite(value) && std::floor(value) == value && value >= static_cast<double>(std::numeric_limits<int>::min()) && value <= static_cast<double>(std::numeric_limits<int>::max());
}

bool ReadIntegerKey(lua_State* L, int index, int& key) {
    if (lua_type(L, index) != LUA_TNUMBER) {
        return false;
    }
    double value = lua_tonumber(L, index);
    if (!IsIntegerNumber(value)) {
        return false;
    }
    key = static_cast<int>(value);
    return true;
}

struct LuaTableShape {
    bool canSequence = true;
    int count = 0;
    int maxIndex = 0;
};

bool ScanLuaTable(lua_State* L, int index, LuaTableShape& shape) {
    lua_pushnil(L);
    while (lua_next(L, index) != 0) {
        ++shape.count;
        int key = 0;
        if (ReadIntegerKey(L, -2, key)) {
            if (key > 0) {
                shape.maxIndex = std::max(shape.maxIndex, key);
            } else {
                shape.canSequence = false;
            }
        } else if (lua_type(L, -2) == LUA_TSTRING) {
            shape.canSequence = false;
        } else {
            lua_pop(L, 2);
            return false;
        }
        lua_pop(L, 1);
    }
    return true;
}

bool HasTableInStack(const std::vector<const void*>& stack, const void* ptr) {
    for (auto item: stack) {
        if (item == ptr) {
            return true;
        }
    }
    return false;
}

bool LuaValueToYaml(lua_State* L, int index, YAML::Node& node, std::vector<const void*>& stack, int depth) {
    if (depth > kMaxYamlDepth) {
        return false;
    }
    index = LuaAbsIndex(L, index);
    int type = lua_type(L, index);
    switch (type) {
        case LUA_TNIL:
            node = YAML::Node();
            return true;
        case LUA_TBOOLEAN:
            node = lua_toboolean(L, index) != 0;
            return true;
        case LUA_TNUMBER: {
            double value = lua_tonumber(L, index);
            if (!std::isfinite(value)) {
                return false;
            }
            node = value;
            return true;
        }
        case LUA_TSTRING: {
            size_t len = 0;
            const char* value = lua_tolstring(L, index, &len);
            node = std::string(value ? value : "", len);
            return true;
        }
        case LUA_TTABLE:
            break;
        default:
            return false;
    }

    const void* ptr = lua_topointer(L, index);
    if (!ptr || HasTableInStack(stack, ptr)) {
        return false;
    }
    stack.push_back(ptr);

    LuaTableShape shape;
    if (!ScanLuaTable(L, index, shape)) {
        stack.pop_back();
        return false;
    }

    bool isSequence = shape.count > 0 && shape.canSequence && shape.count == shape.maxIndex;
    if (isSequence) {
        node = YAML::Node(YAML::NodeType::Sequence);
        for (int i = 1; i <= shape.maxIndex; ++i) {
            lua_rawgeti(L, index, i);
            YAML::Node child;
            bool ok = LuaValueToYaml(L, -1, child, stack, depth + 1);
            lua_pop(L, 1);
            if (!ok) {
                stack.pop_back();
                return false;
            }
            node.push_back(child);
        }
    } else {
        node = YAML::Node(YAML::NodeType::Map);
        lua_pushnil(L);
        while (lua_next(L, index) != 0) {
            YAML::Node child;
            bool ok = LuaValueToYaml(L, -1, child, stack, depth + 1);
            if (!ok) {
                lua_pop(L, 2);
                stack.pop_back();
                return false;
            }

            int key = 0;
            if (ReadIntegerKey(L, -2, key)) {
                node.force_insert(key, child);
            } else if (lua_type(L, -2) == LUA_TSTRING) {
                size_t len = 0;
                const char* keyText = lua_tolstring(L, -2, &len);
                node.force_insert(std::string(keyText ? keyText : "", len), child);
            } else {
                lua_pop(L, 2);
                stack.pop_back();
                return false;
            }
            lua_pop(L, 1);
        }
    }

    stack.pop_back();
    return true;
}

int __cdecl Lua_File_read(lua_State* L) {
    std::filesystem::path path;
    if (!BuildLuaFilePath(L, 1, path)) {
        PushReadFail(L);
        return 2;
    }

    std::string data;
    if (!ReadFileData(path, data)) {
        PushReadFail(L);
        return 2;
    }

    lua_pushboolean(L, 1);
    lua_pushlstring(L, data.data(), data.size());
    return 2;
}

int __cdecl Lua_File_write(lua_State* L) {
    std::filesystem::path path;
    if (!BuildLuaFilePath(L, 1, path)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);
    lua_pushboolean(L, WriteFileData(path, data, len));
    return 1;
}

int __cdecl Lua_File_readYml(lua_State* L) {
    std::filesystem::path path;
    if (!BuildLuaFilePath(L, 1, path)) {
        PushReadFail(L);
        return 2;
    }

    std::string data;
    if (!ReadFileData(path, data)) {
        PushReadFail(L);
        return 2;
    }

    try {
        if (data.empty()) {
            lua_pushboolean(L, 1);
            lua_newtable(L);
            return 2;
        }

        YAML::Node node = YAML::Load(data);
        if (!node || node.IsNull()) {
            lua_pushboolean(L, 1);
            lua_newtable(L);
            return 2;
        }
        if (!node.IsMap() && !node.IsSequence()) {
            PushReadFail(L);
            return 2;
        }

        lua_pushboolean(L, 1);
        if (!PushYamlNode(L, node, 0)) {
            lua_pop(L, 1);
            PushReadFail(L);
            return 2;
        }
        return 2;
    } catch (...) {
        PushReadFail(L);
        return 2;
    }
}

int __cdecl Lua_File_writeYml(lua_State* L) {
    std::filesystem::path path;
    if (!BuildLuaFilePath(L, 1, path)) {
        lua_pushboolean(L, 0);
        return 1;
    }
    luaL_checktype(L, 2, LUA_TTABLE);

    try {
        YAML::Node node;
        std::vector<const void*> stack;
        if (!LuaValueToYaml(L, 2, node, stack, 0)) {
            lua_pushboolean(L, 0);
            return 1;
        }
        std::string data = YAML::Dump(node);
        lua_pushboolean(L, WriteFileData(path, data.data(), data.size()));
        return 1;
    } catch (...) {
        lua_pushboolean(L, 0);
        return 1;
    }
}
} // namespace lua_io_detail

void RegLuaIO(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, SafeLuaCall<lua_io_detail::Lua_File_read>);
    lua_setfield(L, -2, "read");
    lua_pushcfunction(L, SafeLuaCall<lua_io_detail::Lua_File_write>);
    lua_setfield(L, -2, "write");
    lua_pushcfunction(L, SafeLuaCall<lua_io_detail::Lua_File_readYml>);
    lua_setfield(L, -2, "readYml");
    lua_pushcfunction(L, SafeLuaCall<lua_io_detail::Lua_File_writeYml>);
    lua_setfield(L, -2, "writeYml");
    lua_setglobal(L, "File");
}
} // namespace cg::script
