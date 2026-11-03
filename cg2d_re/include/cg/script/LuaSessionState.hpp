#pragma once

#include <array>
#include <memory>
#include <optional>
#include <random>

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

enum AutoBattleCheckbox {
    CHK_TYPE_SIMPLE,
    CHK_TYPE_LUA,
    CHK_REC_ITEM,
    CHK_REC_SKILL,
    CHK_STOP_LV1,
    CHK_STOP_BOSS,
    CHK_STOP_BAG_FULL,
    CHK_STOP_LOST_PARTY,
    CHK_STOP_DEATH,
    CHK_TARGET_AUTO,
    CHK_TARGET_LAST,
    CHK_SKILL_LV_AUTO,
    CHK_SKILL_LV_LAST,
    CHK_MAX,
};

// 自动战斗面板的选项、启停与鼠标拖动状态，由所属会话的窗口回调读写。
struct AutoBattlePanelState {
    std::array<char, CHK_MAX> checkboxValues {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1};
    bool clicked = false;
    short lastX = 0;
    short lastY = 0;
    int simpleMode = 1;
    bool enabled = false;
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
    autobattle_detail::AutoBattlePanelState autoBattlePanel {};
    std::optional<std::minstd_rand> autoBattleTargetRandom; // 首次需洗牌时播种；Lua 重启不重置序列。
};

} // namespace cg::script
