#pragma once

#include <memory>

struct lua_State;

namespace cg::script {

struct LuaPacketBridgeState;

namespace autobattle_detail {

enum B_ACTION {
    B_ACTION_SKILL,
    B_ACTION_ITEM,
    B_ACTION_ATTACK,
    B_ACTION_DEF,
    B_ACTION_ESCAPE,
    B_ACTION_MOVE,
    B_ACTION_PET,
    B_ACTION_NONE,
    B_ACTION_EQUIP,
    B_ACTION_REBIRTH,
    B_ACTION_PET_SKILL,
};

struct Action {
    B_ACTION type;
    int skillId;
    int target;
    int techId;
    int itemId;
};

struct AutoBattleContext {
    Action actionSet[2];
    Action action[2];
    Action actionNew[2];
};

} // namespace autobattle_detail

// 两套 Lua VM 及其关闭门禁属于同一账号会话，关闭前由脚本模块按原顺序释放。
struct LuaSessionState {
    lua_State* uiState = nullptr;
    lua_State* battleState = nullptr;
    bool shuttingDown = false;
    std::shared_ptr<LuaPacketBridgeState> packetBridge;
    autobattle_detail::AutoBattleContext autoBattleContext {}; // 两条旧动作及本轮新动作。
    int autoBattleActionCount = 0; // 本轮已发送的动作数。
    bool autoBattleRequireAction = false; // 收齐两条后等待采纳。
    bool autoBattleActionSetup = false; // 上轮动作可用于自动发送。
};

} // namespace cg::script
