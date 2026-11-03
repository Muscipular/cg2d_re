#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "LuaUI.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "../world/BattleState.hpp"
#include "../world/PlayerState.hpp"
#include <bit>
#include <charconv>
#include <ctime>
#include <process.h>
#include <random>
#include <cstdio>

namespace cg::script {
namespace autobattle_detail {
static constexpr std::uint32_t BP_FLAG_WEAPON_DIRECT = 0x80;

using B_TARGET_FLAGS = std::uint16_t;
enum : std::uint16_t {
    B_TARGET_WHO = 0x1,
    B_TARGET_IN_DEAD = 0x2,
    B_TARGET_IN_PET = 0x4,
    B_TARGET_IN_ME = 0x8,
    B_TARGET_IN_FRIEND = 0x10,
    B_TARGET_IN_ENEMY = 0x20,
    B_TARGET_SORO = 0x40,
    B_TARGET_CIRCUMFERENCE = 0x80,
    B_TARGET_SIDE = 0x100,
    B_TARGET_ALL = 0x200,
    B_TARGET_CHECK_WEAPON = 0x400,
    B_TARGET_NOT_ME = 0x800,
};

enum {
    SIDE_0 = 40,
    SIDE_1 = 41,
    SIDE_ALL = 42,
};

static bool CheckTarget(int a, int d, B_TARGET_FLAGS flags) {
    if (a < 0 || d < 0 || a >= 20 || d >= 20)
        return false;
    if (!cg::world::gBattleEntries59E980[d]) {
        return false;
    }
    int aSide = a / 10;
    int aLine = a % 10 / 5;
    int dSide = d / 10;
    int dLine = d % 10 / 5;
    if (static_cast<std::int32_t>(cg::world::gBattleEntries59E980[d]->readiness) <= 0) {
        if (!(flags & B_TARGET_IN_DEAD)) {
            return false;
        }
    }
    if ((flags & B_TARGET_NOT_ME) && a == d) {
        return false;
    }
    bool isMe = (flags & B_TARGET_IN_ME) && a == d;
    bool isFriend = (flags & B_TARGET_IN_FRIEND) && (a / 10) == (d / 10);
    bool isEnemy = (flags & B_TARGET_IN_ENEMY) && (a / 10) != (d / 10);
    bool isPet = (flags & B_TARGET_IN_PET) && ((a + 5) % 10 + a / 10 * 10) == d;
    bool isAll = flags & B_TARGET_ALL;
    if (!isMe && !isFriend && !isEnemy && !isPet && !isAll) {
        return false;
    }
    if (flags & B_TARGET_CHECK_WEAPON) {
        if (aSide != dSide && aLine == dLine && aLine == 0) {
            int aLinePos = a % 10 % 5;
            int aP = aSide * 10 + aLinePos + (aLine + 1) % 2 * 5;
            bool aAlive = cg::world::gBattleEntries59E980[aP] && cg::world::gBattleEntries59E980[aP]->readiness > 0;
            int dLinePos = d % 10 % 5;
            int dP = dSide * 10 + dLinePos + (dLine + 1) % 2 * 5;
            bool dAlive = cg::world::gBattleEntries59E980[dP] && cg::world::gBattleEntries59E980[dP]->readiness > 0;
            if (aAlive && dAlive) {
                if (a == cg::world::gBattleCurrentPosition5ABCA4) {
                    if (cg::world::gBattlePlayerFlags5ABCAC & BP_FLAG_WEAPON_DIRECT) {
                        return false;
                    }
                } else {
                    // 宠物强制近战
                    return false;
                }
            }
        }
    }
    return true;
}

static constexpr struct {
    short x, y;
    short w, h;
    const wchar_t* msg;
    char group;
} checkboxList[]{
        {67,          31,          50,  14, L"重复上一回合动作",     1},
        {67 + 52,     31,          50,  14, L"LUA控制行为",          1},
        //
        {67,          31 + 19,     50,  14, L"使用血瓶",             0},
        {67 + 52,     31 + 19,     50,  14, L"使用补血技能",         0},
        //
        {67,          31 + 19 * 2, 50,  14, L"遇到1级怪物时暂停",    0},
        {67 + 52,     31 + 19 * 2, 50,  14, L"遇到BOSS时暂停",       0},
        {67 + 52 * 2, 31 + 19 * 2, 50,  14, L"背包当前页装满时暂停", 0},
        //
        {67,          31 + 19 * 3, 50,  14, L"队友掉线/离队时暂停",  0},
        {67 + 52,     31 + 19 * 3, 100, 14, L"队友死亡时暂停",       0},
        //
        {67,          31 + 19 * 4, 50,  14, L"自动选择最优目标",     3},
        {67 + 52,     31 + 19 * 4, 100, 14, L"选择上次选中的目标",   3},
        //
        {67,          31 + 19 * 5, 50,  14, L"自动选取技能等级",     2},
        {67 + 52,     31 + 19 * 5, 50,  14, L"最后使用的技能等级",   2},

};
static_assert(std::size(checkboxList) == CHK_MAX);
enum {
    SIMPLE_MODE_OFF = 0,
    SIMPLE_MODE_FULL = 1,
    SIMPLE_MODE_HIDE = 2,
};

static void MakeAutoBattle(session::ClientSession& session);

static int takeRandomTarget(session::ClientSession& session, int attacker, B_TARGET_FLAGS flags);

static int getSkillId(int index) {
    return index >= 0 && index < cg::world::kSkillSlotCount ? cg::world::gSkillsF1BAFC[index].skillId : -1;
}

static int getSkillIndex(int id) {
    for (int j = 0; j < cg::world::kSkillSlotCount; ++j) {
        if (cg::world::gSkillPairsF63370[j].used && cg::world::gSkillPairsF63370[j].index < cg::world::kSkillSlotCount && cg::world::gSkillsF1BAFC[cg::world::gSkillPairsF63370[j].index].skillId == id) {
            return cg::world::gSkillPairsF63370[j].index;
        }
    }
    return -1;
}

static int getCurrentPet() {
    for (int i = 0; i < 5; ++i) {
        if (cg::world::gPetRuntimeRecords[i].flags6AA & 2) {
            return i;
        }
    }
    return -1;
}

static int getPetTechIndex(int techId) {
    int pet = getCurrentPet();
    if (pet < 0) {
        return -1;
    }
    for (int ix = 0; ix < cg::world::kPetTechniqueSlotCount; ++ix) {
        auto& data = cg::world::gPetRuntimeRecords[pet].techniquePairs650[ix];
        if (data.used && data.index < cg::world::kPetTechniqueSlotCount && cg::world::gPetRuntimeRecords[pet].techniquesD8[data.index].techniqueId == techId) {
            return data.index;
        }
    }
    return -1;
}

static int getPetTechId(int techIndex) {
    int pet = getCurrentPet();
    if (pet < 0) {
        return -1;
    }
    if (techIndex >= 0 && techIndex < cg::world::kPetTechniqueSlotCount && cg::world::gPetRuntimeRecords[pet].techniquesD8[techIndex].used)
        return cg::world::gPetRuntimeRecords[pet].techniquesD8[techIndex].techniqueId;
    return -1;
}

static int getItemId(int itemIndex) {
    if (itemIndex >= 0 && itemIndex < 28 && cg::world::gPlayerRuntimeSlots.items[itemIndex].used) {
        return cg::world::gPlayerRuntimeSlots.items[itemIndex].itemId;
    }
    return -1;
}

static int getItemIndex(int itemId) {
    for (int i = 8; i < 28; ++i) {
        if (cg::world::gPlayerRuntimeSlots.items[i].used && cg::world::gPlayerRuntimeSlots.items[i].itemId == itemId) {
            return i;
        }
    }
    return -1;
}

static int checkAndGetTarget(int attacker, int defer, B_TARGET_FLAGS flg) {
    if (attacker < 0 || attacker >= 20) return -1;
    if (defer >= SIDE_ALL) {
        defer = (1 - attacker / 10) * 10;
        if (flg & B_TARGET_IN_ENEMY) {
            for (int j = 0; j < 10; ++j) {
                if (cg::world::gBattleEntries59E980[defer + j] && ((flg & B_TARGET_IN_DEAD) || cg::world::gBattleEntries59E980[defer + j]->readiness > 0)) {
                    defer += j;
                    break;
                }
            }
        } else {
            defer = attacker;
        }
    } else if (defer >= SIDE_0) {
        defer = 10 * (defer - SIDE_0);
        for (int j = 0; j < 10; ++j) {
            if (cg::world::gBattleEntries59E980[defer + j] && ((flg & B_TARGET_IN_DEAD) || cg::world::gBattleEntries59E980[defer + j]->readiness > 0)) {
                defer = defer + j;
                break;
            }
        }
    } else if (defer >= 20) {
        defer = (defer - 20);
    }
    if (!CheckTarget(attacker, defer, flg)) {
        return -1;
    }
    if (flg & B_TARGET_ALL) {
        return SIDE_ALL;
    }
    if (flg & B_TARGET_SIDE) {
        return defer / 10 + SIDE_0;
    }
    if (flg & B_TARGET_CIRCUMFERENCE) {
        return defer + 20;
    }
    return defer;
}

static int SendBattleAction(lua_State* L)
{
    auto& connection = GetLuaConnection(L);
    auto& runtime = connection.Owner().Lua();
    if (runtime.autoBattleActionCount >= 2) {
        LuaPush(L, false);
        return 1;
    }
    runtime.autoBattleActionCount++;
    const auto param1 = static_cast<B_ACTION>(luaL_optinteger(L, 1, B_ACTION_ATTACK));
    int param2 = static_cast<int>(luaL_optinteger(L, 2, -1));
    int param3 = static_cast<int>(luaL_optinteger(L, 3, -1));
    int param4 = static_cast<int>(luaL_optinteger(L, 4, -1));
    m_log_iw(L"send command %d %d %d %d\n", param1, param2, param3, param4);
    char buff[256] = {};
    int n = 0;
    auto sendNone = [&]() {
        n = sprintf_s(buff, "Rg N\n");
        static_cast<void>(connection.SendPacket(buff, n));
    };
    switch (param1) {
        case B_ACTION_SKILL: {
            auto skIndex = getSkillIndex(param2);
            if (skIndex < 0) {
                sendNone();
                break;
            }
            if (param3 < 0 || param3 >= cg::world::kTechniqueSlotCount) {
                sendNone();
                break;
            }
            const auto& data = cg::world::gSkillsF1BAFC[skIndex].techniques3C[param3];
            const auto targetFlags = static_cast<B_TARGET_FLAGS>(data.value84);
            if (!data.used88 || !data.value8C || !(data.value80 == 0 || data.value80 == 1)) {
                sendNone();
                break;
            }
            if (param4 < 0) {
                param4 = takeRandomTarget(connection.Owner(), cg::world::gBattleCurrentPosition5ABCA4, targetFlags);
            }
            auto target = checkAndGetTarget(cg::world::gBattleCurrentPosition5ABCA4, param4, targetFlags);
            if (target < 0 && (targetFlags & B_TARGET_WHO)) {
                sendNone();
                break;
            }
            n = sprintf_s(buff, "Rg S|%X|%X|%X\n", skIndex, param3, target);
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        }
        case B_ACTION_ITEM: {
            auto itemIndex = getItemIndex(param2);
            if (itemIndex < 0) {
                sendNone();
                break;
            }
            auto target = checkAndGetTarget(cg::world::gBattleCurrentPosition5ABCA4, param3, cg::world::gPlayerRuntimeSlots.items[itemIndex].target);
            if (target < 0) {
                sendNone();
                break;
            }
            n = sprintf_s(buff, "Rg I|%X|%X\n", itemIndex, target);
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        }
        case B_ACTION_ATTACK: {
            int attacker = cg::world::gBattleCurrentPosition5ABCA4;
            auto flg = (B_TARGET_FLAGS) (B_TARGET_IN_ENEMY | B_TARGET_SORO | B_TARGET_CHECK_WEAPON);
            if (param2 < 0) {
                m_log_iw(L"B_ACTION_ATTACK takeRandomTarget %d target %d\n", attacker, param2);
                param2 = takeRandomTarget(connection.Owner(), attacker, flg);
            }
            m_log_iw(L"B_ACTION_ATTACK attacker %d target %d\n", attacker, param2);
            auto target = checkAndGetTarget(attacker, param2, flg);
            m_log_iw(L"B_ACTION_ATTACK attacker %d target %d\n", attacker, param2);
            if (target < 0) {
                sendNone();
                break;
            }
            n = sprintf_s(buff, "Rg H|%X\n", target);
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        }
        case B_ACTION_DEF:
            n = sprintf_s(buff, "Rg G\n");
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        case B_ACTION_ESCAPE:
            n = sprintf_s(buff, "Rg E\n");
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        case B_ACTION_MOVE:
            n = sprintf_s(buff, "Rg P\n");
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        case B_ACTION_PET:
            n = sprintf_s(buff, "Rg M|%X\n", param2);
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        case B_ACTION_NONE:
            sendNone();
            break;
        case B_ACTION_EQUIP:
            n = sprintf_s(buff, "Rg Q|%X\n", param2 < 0 ? 255 : param2);
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        case B_ACTION_REBIRTH:
            n = sprintf_s(buff, "Rg R|%X\n", param2 ? 1 : 0);
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        case B_ACTION_PET_SKILL: {
            auto techIndex = getPetTechIndex(param2);
            auto petPos = (cg::world::gBattleCurrentPosition5ABCA4 % 10 + 5) % 10 + cg::world::gBattleCurrentPosition5ABCA4 / 10 * 10;
            int petIndex = getCurrentPet();
            m_log_iw(L"B_ACTION_ATTACK techIndex %d petIndex %d\n", techIndex, petIndex);
            m_log_iw(L"B_ACTION_ATTACK attacker %d target %d\n", petPos, param3);
            if (petIndex < 0) {
                sendNone();
                break;
            }
            if (techIndex < 0) {
                n = sprintf_s(buff, "Rg W|FF|FF\n");
                static_cast<void>(connection.SendPacket(buff, n));
                break;
            }
            m_log_iw(L"B_ACTION_ATTACK attacker %d target %d\n", petPos, param3);
            B_TARGET_FLAGS targetFlags = cg::world::gPetRuntimeRecords[petIndex].techniquesD8[techIndex].target;
            m_log_iw(L"B_ACTION_ATTACK tech %d %d %ls\n", cg::world::gPetRuntimeRecords[petIndex].techniquesD8[techIndex].techniqueId, targetFlags, cg::world::gPetRuntimeRecords[petIndex].techniquesD8[techIndex].name.data());
            auto target = checkAndGetTarget(petPos, param3, targetFlags);
            m_log_iw(L"B_ACTION_ATTACK checkAndGetTarget %d target %d\n", petPos, target);
            if (target < 0 && (targetFlags & B_TARGET_WHO)) {
                n = sprintf_s(buff, "Rg W|FF|FF\n");
                static_cast<void>(connection.SendPacket(buff, n));
                break;
            }
            n = sprintf_s(buff, "Rg W|%X|%X\n", techIndex, target);
            static_cast<void>(connection.SendPacket(buff, n));
            break;
        }
    }
    lua_pushboolean(L, true);
    return 1;
}

static void PushAction(lua_State* L, const Action& action) {
    lua_newtable(L);
    Field(L, "type", static_cast<int>(action.type));
    Field(L, "skillId", action.skillId); Field(L, "techId", action.techId);
    Field(L, "target", action.target); Field(L, "itemId", action.itemId);
}

static void PushBattleInfo(session::ClientSession& session, lua_State* L) {
    auto& runtime = session.Lua();
    const auto& values = runtime.autoBattlePanel.checkboxValues;
    runtime.autoBattleActionCount = 0;
    lua_newtable(L);
    lua_pushinteger(L, session.WorldScene().battle.turn);
    lua_setfield(L, -2, "battleTurn");
    lua_pushinteger(L, session.WorldScene().battle.battleType);
    lua_setfield(L, -2, "battleType");
    lua_pushinteger(L, session.WorldScene().battle.fieldFlags);
    lua_setfield(L, -2, "battleField");
    int n = 0;
    for (int j = 0; j < 10; ++j) {
        if (cg::world::gBattleEntries59E980[j]) {
            n++;
        }
    }
    lua_pushinteger(L, n);
    lua_setfield(L, -2, "partyCount");
    if (values[CHK_TYPE_SIMPLE]) {
        lua_createtable(L, 2, 0);
        for (int i = 0; i < 2; ++i) { PushAction(L, runtime.autoBattleContext.action[i]); lua_rawseti(L, -2, i + 1); }
        lua_setfield(L, -2, "lastAction");
    }
    lua_pushboolean(L, values[CHK_TYPE_SIMPLE]);
    lua_setfield(L, -2, "CHK_TYPE_SIMPLE");
    lua_pushboolean(L, values[CHK_TYPE_LUA]);
    lua_setfield(L, -2, "CHK_TYPE_LUA");
    lua_pushboolean(L, values[CHK_REC_ITEM]);
    lua_setfield(L, -2, "CHK_REC_ITEM");
    lua_pushboolean(L, values[CHK_REC_SKILL]);
    lua_setfield(L, -2, "CHK_REC_SKILL");
    lua_pushboolean(L, values[CHK_STOP_LV1]);
    lua_setfield(L, -2, "CHK_STOP_LV1");
    lua_pushboolean(L, values[CHK_STOP_BOSS]);
    lua_setfield(L, -2, "CHK_STOP_BOSS");
    lua_pushboolean(L, values[CHK_STOP_BAG_FULL]);
    lua_setfield(L, -2, "CHK_STOP_BAG_FULL");
    lua_pushboolean(L, values[CHK_STOP_LOST_PARTY]);
    lua_setfield(L, -2, "CHK_STOP_LOST_PARTY");
    lua_pushboolean(L, values[CHK_STOP_DEATH]);
    lua_setfield(L, -2, "CHK_STOP_DEATH");
    lua_pushboolean(L, values[CHK_TARGET_AUTO]);
    lua_setfield(L, -2, "CHK_TARGET_AUTO");
    lua_pushboolean(L, values[CHK_TARGET_LAST]);
    lua_setfield(L, -2, "CHK_TARGET_LAST");
    lua_pushboolean(L, values[CHK_SKILL_LV_AUTO]);
    lua_setfield(L, -2, "CHK_SKILL_LV_AUTO");
    lua_pushboolean(L, values[CHK_SKILL_LV_LAST]);
    lua_setfield(L, -2, "CHK_SKILL_LV_LAST");
    lua_createtable(L, 20, 0);
    for (int i = 0; i < 20; ++i) { PushBattleEntry(L, i); lua_rawseti(L, -2, i + 1); }
    lua_setfield(L, -2, "entryList");
    lua_pushcfunction(L, SafeLuaCall<SendBattleAction>);
    lua_setfield(L, -2, "SendAction");
}

static bool EmitAutoBattle(session::ClientSession& session) {
    auto* L = GetBattleLuaState(session);
    if (!L) return false;
    const int top = lua_gettop(L);
    lua_getglobal(L, "AutoBattleEvent");
    bool accepted = false;
    if (lua_isfunction(L, -1)) {
        PushBattleInfo(session, L); PushPlayer(L);
        if (lua_pcall(L, 2, 1, 0) == 0) accepted = lua_isnumber(L, -1) && lua_tointeger(L, -1) != 0;
        else m_log_iw(L"[Lua] AutoBattleEvent failed: %hs\n", lua_tostring(L, -1));
    }
    lua_settop(L, top);
    return accepted;
}

static void DoAutoBattle(session::ClientSession& session) {
    if (world::gCurrentSceneType == SCENE_BATTLE && world::gCurrentSceneState == 4) {
        if (session.Lua().autoBattleActionSetup && EmitAutoBattle(session)) ui::detail::WindowBattleCommand_FinishAutoAction(session);
        else session.Lua().autoBattlePanel.enabled = false;
    }
}
static int takeRandomTarget(session::ClientSession& session, int attacker, B_TARGET_FLAGS flags) {
    std::vector<int> list;
    int min = 10, max = 19;
    if (flags & B_TARGET_IN_FRIEND && !(flags & B_TARGET_IN_ENEMY)) {
        min = 0;
        max = 9;
    }
    for (int i = min; i <= max; ++i) {
        if (cg::world::gBattleEntries59E980[i] && cg::world::gBattleEntries59E980[i]->readiness > 0) {
            list.push_back(i);
        }
    }
    if (list.empty()) {
        return -1;
    }
    if (list.size() > 1) {
        auto& rnd = session.Lua().autoBattleTargetRandom;
        if (!rnd) rnd.emplace(std::random_device().operator()() ^ _getpid());
        std::shuffle(list.begin(), list.end(), *rnd);
        std::shuffle(list.begin(), list.end(), *rnd);
    }
    for (auto id: list) {
        m_log_iw(L"enemy %d %ls\n", id, cg::world::gBattleEntries59E980[id]->nameC4.c_str());
    }
    for (auto ix: list) {
        int t = checkAndGetTarget(attacker, ix, flags);
        m_log_iw(L"enemy %d checkAndGetTarget %d(%x) => %d\n", ix, attacker, flags, t);
        if (t >= 0) {
            return t;
        }
    }
    return -1;
}

// 面板沿原坐标、图号、互斥选项和鼠标规则；载荷由C++容器持有。
struct AutoBattleWindow final : UIWindowInstanceEx {
    std::array<ui::WindowGraphicData, CHK_MAX + 3> graphics{};
    AutoBattleWindow(session::ClientSession& session, int id, int x, int width, int height, int count) {
        window.id = id; window.x = x; window.y = 100;
        window.width = width; window.height = height;
        window.state = WIN_STATE_ACTIVE; window.layer = 4;
        window.ownerSession = &session;
        for (int slot = 0; slot < count; ++slot) {
            auto& element = ui::WindowRuntime_AddElement(window, ui::NativeElementKey(slot));
            element.seqNo = slot;
            element.ownsPayload = false;
        }
    }
    ~AutoBattleWindow() override {
        ui::WindowRuntime_DestroyElements(window);
        ui::WindowRuntime_ReleaseBindings(window);
    }
    void AddGraphic(int ix, int x, int y, int w, int h, int image, ui::WindowElementCallback callback) {
        graphics[ix] = {image, 0, 0, 0, 0, 0xFFFFFFFFu};
        auto* element = window.FindElementById(ui::NativeElementKey(ix));
        if (!element) return;
        element->type = ui::WindowElementType_Graphic;
        element->x = x; element->y = y; element->width = w; element->height = h;
        element->visible = 1; element->data.graphic = &graphics[ix]; element->mouseCallback = callback;
    }
    int update(int stage, int) override {
        auto& panel = window.ownerSession->Lua().autoBattlePanel;
        if (window.id == WIN_AUTOBATTLE && panel.clicked && (gMouseBtnDown2 & CG_MBTN_LEFTDOWN)) {
            window.x += gMouseX - panel.lastX; window.y += gMouseY - panel.lastY;
            panel.lastX = static_cast<short>(static_cast<std::int32_t>(gMouseX)); panel.lastY = static_cast<short>(static_cast<std::int32_t>(gMouseY));
        }
        ui::detail::WindowRuntime_UpdateExtendedElements();
        return stage;
    }
    int render(int) override { ui::WindowRuntime_DrawCurrent(window); return 1; }
};

static ui::WindowGraphicData* CurrentGraphic(ui::ElementId elementId) {
    auto* const window = gCurrentWindow;
    if (!window) return nullptr;
    auto* const element = window->FindElementById(elementId);
    return element ? element->data.graphic : nullptr;
}

static void MakeAutoBattleSimple(session::ClientSession& session) {
    auto& state = session.Lua().autoBattlePanel;
    if (state.simpleMode == SIMPLE_MODE_OFF) state.simpleMode = SIMPLE_MODE_FULL;
    auto panel = std::make_shared<AutoBattleWindow>(session, WIN_AUTOBATTLE_SIMPLE,
        state.simpleMode == SIMPLE_MODE_FULL ? -2 : -70, 77, 20, 2);
    panel->window.category = 0;
    auto* toggle = panel->window.FindElementById(ui::NativeElementKey(0));
    toggle->type = ui::WindowElementType_EventArea; toggle->x = 65; toggle->width = 10;
    toggle->height = 20; toggle->visible = 1;
    toggle->mouseCallback = +[](ui::ElementId, int mouse) {
            if (mouse & 2) {
                auto& state = gCurrentWindow->ownerSession->Lua().autoBattlePanel;
                state.simpleMode = state.simpleMode == SIMPLE_MODE_HIDE ? SIMPLE_MODE_FULL : SIMPLE_MODE_HIDE;
                gCurrentWindow->x = state.simpleMode == SIMPLE_MODE_FULL ? -2 : -70;
            }
            return (mouse & 2) ? 1 : 0;
        };
    panel->AddGraphic(1, 0, 0, 77, 20, 641503, +[](ui::ElementId, int mouse) {
        if (mouse & 2) {
            auto& state = gCurrentWindow->ownerSession->Lua().autoBattlePanel;
            state.simpleMode = SIMPLE_MODE_OFF;
            gCurrentWindow->state |= WIN_STATE_CLOSE;
            MakeAutoBattle(*gCurrentWindow->ownerSession);
        }
        return (mouse & 2) ? 1 : 0;
    });
    std::shared_ptr<UIWindowInstanceEx> base = panel;
    WindowExOpen(base);
}

static void MakeAutoBattle(session::ClientSession& session) {
    auto panel = std::make_shared<AutoBattleWindow>(session, WIN_AUTOBATTLE, 50, 240, 185, CHK_MAX + 3);
    panel->window.category = 1;
    for (int ix = 0; ix < CHK_MAX; ++ix) {
        const auto& check = checkboxList[ix];
        panel->AddGraphic(ix, check.x, check.y, check.w, check.h, 641502, +[](ui::ElementId elementId, int mouse) {
            const int index = static_cast<int>(elementId) - 1; // 复选项数组从0开始。
            const auto& check = checkboxList[index];
            auto& values = gCurrentWindow->ownerSession->Lua().autoBattlePanel.checkboxValues;
            if (mouse & 1) ui::detail::gHoverTextD68AE8 = check.msg;
            if (mouse & 2) {
                if (check.group) for (int other = 0; other < CHK_MAX; ++other)
                    if (checkboxList[other].group == check.group) values[other] = 0;
                values[index] = values[index] ? 0 : 1;
            }
            if (auto* graphic = CurrentGraphic(elementId)) graphic->imageNo = values[index] ? 641501 : 641502;
            return (mouse & 2) ? 1 : 0;
        });
    }
    panel->AddGraphic(CHK_MAX, 200, 6, 14, 14, 243000, +[](ui::ElementId elementId, int mouse) {
        if (mouse & 1) ui::detail::gHoverTextD68AE8 = L"关闭窗口";
        if (mouse & 2) {
            MakeAutoBattleSimple(*gCurrentWindow->ownerSession);
            gCurrentWindow->state |= WIN_STATE_CLOSE;
            return 1;
        }
        if (auto* graphic = CurrentGraphic(elementId)) graphic->imageNo = (mouse & 1) ? 243001 : 243000;
        return 0;
    });
    panel->graphics[CHK_MAX].offsetX = -2;
    panel->graphics[CHK_MAX].offsetY = -1;
    panel->graphics[CHK_MAX].width = panel->graphics[CHK_MAX].height = 12;
    panel->AddGraphic(CHK_MAX + 1, 80, 150, 58, 17, 640186, +[](ui::ElementId elementId, int mouse) {
        auto& enabled = gCurrentWindow->ownerSession->Lua().autoBattlePanel.enabled;
        if (mouse & 1) ui::detail::gHoverTextD68AE8 = enabled ? L"停止自动战斗" : L"开始自动战斗";
        if (mouse & 2) {
            const int count = cg::world::gBattleSkillActionCount5ABBF4 + cg::world::gBattlePetActionCount5ABBBC;
            if ((world::gCurrentSceneState == 4 && count == 0) || enabled || world::gCurrentSceneState != 4)
                enabled = !enabled;
        }
        if (auto* graphic = CurrentGraphic(elementId)) graphic->imageNo = enabled ? 640188 : ((mouse & 1) ? 640187 : 640186);
        return (mouse & 2) ? 1 : 0;
    });
    panel->AddGraphic(CHK_MAX + 2, 0, 0, 240, 185, 641500, +[](ui::ElementId, int mouse) {
        auto& state = gCurrentWindow->ownerSession->Lua().autoBattlePanel;
        if (mouse & 2) { state.clicked = true; state.lastX = static_cast<short>(static_cast<std::int32_t>(gMouseX)); state.lastY = static_cast<short>(static_cast<std::int32_t>(gMouseY)); }
        else if (state.clicked && !(mouse & 8)) state.clicked = false;
        return (mouse & 2) ? 1 : 0;
    });
    std::shared_ptr<UIWindowInstanceEx> base = panel;
    WindowExOpen(base);
}
// 兼容原Lua全局名；日志仅进入本线m_log。
static int LuaCheckTarget(lua_State* L) {
    const int attacker = static_cast<int>(lua_tointeger(L, 1));
    const int defender = static_cast<int>(lua_tointeger(L, 2));
    int flags = static_cast<int>(lua_tointeger(L, 3));
    if (flags <= 0) flags = B_TARGET_CHECK_WEAPON | B_TARGET_IN_FRIEND | B_TARGET_IN_ENEMY | B_TARGET_IN_PET | B_TARGET_IN_ME | B_TARGET_WHO;
    Push(L, CheckTarget(attacker, defender, static_cast<B_TARGET_FLAGS>(flags))); return 1;
}
static int LuaBattleTime(lua_State* L) { Push(L, _time64(nullptr)); return 1; }
static int LuaBattleLog(lua_State* L) { m_log_iw(L"[AutoBattle] %hs\n", luaL_checkstring(L, 1)); return 0; }
static int LuaRandomTarget(lua_State* L) {
    Push(L, takeRandomTarget(GetLuaConnection(L).Owner(),
        static_cast<int>(lua_tointeger(L, 1)), static_cast<B_TARGET_FLAGS>(lua_tointeger(L, 2)))); return 1;
}
static int LuaCheckedTarget(lua_State* L) {
    Push(L, checkAndGetTarget(static_cast<int>(lua_tointeger(L, 1)), static_cast<int>(lua_tointeger(L, 2)), static_cast<B_TARGET_FLAGS>(lua_tointeger(L, 3)))); return 1;
}

} // namespace autobattle_detail


void InitializeAutoBattle(session::ClientSession& session) {
    auto* L = GetBattleLuaState(session);
    const int top = lua_gettop(L);
    lua_pushcfunction(L, SafeLuaCall<autobattle_detail::LuaCheckTarget>); lua_setglobal(L, "CheckTarget");
    lua_pushcfunction(L, SafeLuaCall<autobattle_detail::LuaBattleTime>); lua_setglobal(L, "GetTime");
    lua_pushcfunction(L, SafeLuaCall<autobattle_detail::LuaBattleLog>); lua_setglobal(L, "LogToFile");
    lua_pushcfunction(L, SafeLuaCall<autobattle_detail::LuaRandomTarget>); lua_setglobal(L, "TakeRandomTarget");
    lua_pushcfunction(L, SafeLuaCall<autobattle_detail::LuaCheckedTarget>); lua_setglobal(L, "CheckAndGetTarget");
    lua_getglobal(L, "math"); lua_getfield(L, -1, "randomseed");
    Push(L, (platform::ReadTickCount64() * 5137) ^ static_cast<std::uint64_t>(_getpid()));
    if (lua_pcall(L, 1, 0, 0) != 0) m_log_iw(L"[Lua] randomseed failed: %hs\n", lua_tostring(L, -1));
    lua_settop(L, top);
    if (luaL_dofile(L, "lua/autobattle_base.lua") != 0)
        m_log_iw(L"[Lua] autobattle_base failed: %hs\n", lua_tostring(L, -1));
    lua_settop(L, top);
}

void ShutdownAutoBattle(session::ClientSession& session) {
    auto& runtime = session.Lua();
    runtime.autoBattleRequireAction = false;
    runtime.autoBattleActionSetup = false;
    runtime.autoBattleActionCount = 0;
    runtime.autoBattleContext = {};
    runtime.autoBattlePanel = {};
}

// 实际Rg发送前记录手动动作；不占用N01的首匹配注册槽。
void RecordAutoBattleAction(cg::network::CgConnection& networkConnection, std::string_view packet) {
    if (!GetBattleLuaState(networkConnection.Owner()) || networkConnection.Owner().Lua().autoBattlePanel.enabled || !packet.starts_with("Rg ")) return;
    auto& runtime = networkConnection.Owner().Lua();
    switch (networkConnection.Owner().WorldScene().battle.battleType) { case 0: case 2: case 3: case 7: return; default: break; }
    const int count = cg::world::gBattleSkillActionCount5ABBF4 + cg::world::gBattlePetActionCount5ABBBC;
    if (count < 0 || count >= 2) {
        m_log_iw(L"[AutoBattle] action slot out of range: %d\n", count);
        return;
    }
    const auto body = packet.substr(3);
    if (body.empty()) return;
    std::array<int, 3> values{-1, -1, -1};
    std::size_t offset = 2;
    for (auto& value : values) {
        if (offset >= body.size()) break;
        unsigned number = 0;
        const auto result = std::from_chars(body.data() + offset, body.data() + body.size(), number, 16);
        if (result.ec != std::errc{}) break;
        value = std::bit_cast<int>(number);
        offset = static_cast<std::size_t>(result.ptr - body.data());
        if (offset >= body.size() || body[offset] != '|') break;
        ++offset;
    }
    autobattle_detail::Action action{};
    action.target = -1;
    switch (body[0]) {
    case 'S': action.type = autobattle_detail::B_ACTION_SKILL; action.skillId = autobattle_detail::getSkillId(values[0]); action.techId = values[1]; action.target = values[2]; break;
    case 'W': action.type = autobattle_detail::B_ACTION_PET_SKILL; action.techId = autobattle_detail::getPetTechId(values[0]); action.target = values[1]; break;
    case 'H': action.type = autobattle_detail::B_ACTION_ATTACK; action.target = values[0]; break;
    case 'Q': case 'M': case 'R': action.type = autobattle_detail::B_ACTION_ATTACK; break;
    case 'I': action.type = autobattle_detail::B_ACTION_ITEM; action.itemId = autobattle_detail::getItemId(values[0]); action.target = values[1]; break;
    case 'P': action.type = autobattle_detail::B_ACTION_MOVE; break;
    case 'N': case 'U': action.type = autobattle_detail::B_ACTION_NONE; break;
    case 'G': action.type = autobattle_detail::B_ACTION_DEF; break;
    case 'E': action.type = autobattle_detail::B_ACTION_ESCAPE; break;
    default: return;
    }
    runtime.autoBattleContext.actionNew[count] = action;
    runtime.autoBattleActionCount = count + 1;
    if (runtime.autoBattleActionCount == 2) runtime.autoBattleRequireAction = true;
}

void AutoBattleProc(session::ClientSession& session) {
    auto& runtime = session.Lua();
    auto winAutoBattle = GetExtWindowById(session, WIN_AUTOBATTLE);
    auto winAutoBattleSimple = GetExtWindowById(session, WIN_AUTOBATTLE_SIMPLE);
    bool isOpen = winAutoBattle || winAutoBattleSimple;
    if (world::gCurrentSceneType == SCENE_BATTLE) {
        if (world::gCurrentSceneState == 1) {
            runtime.autoBattlePanel.simpleMode = autobattle_detail::SIMPLE_MODE_FULL;
        }
        if (runtime.autoBattleRequireAction) {
            runtime.autoBattleContext.action[0] = runtime.autoBattleContext.actionNew[0];
            runtime.autoBattleContext.action[1] = runtime.autoBattleContext.actionNew[1];
            runtime.autoBattleActionSetup = true;
            runtime.autoBattleRequireAction = false;
        }
        if (!isOpen && world::gCurrentSceneState >= 3 && world::gCurrentSceneState < 7) {
            if (runtime.autoBattlePanel.simpleMode) {
                autobattle_detail::MakeAutoBattleSimple(session);
            } else {
                autobattle_detail::MakeAutoBattle(session);
            }
        } else if (world::gCurrentSceneState >= 7) {
            CgWindowClose(session, WIN_AUTOBATTLE);
            CgWindowClose(session, WIN_AUTOBATTLE_SIMPLE);
        }
        if (runtime.autoBattlePanel.enabled) {
            autobattle_detail::DoAutoBattle(session);
        }
    }
    if ((world::gPendingSceneType >= 0 && world::gPendingSceneType != SCENE_BATTLE) || world::gCurrentSceneType != SCENE_BATTLE) {
        if (winAutoBattle != nullptr) {
            CgWindowClose(session, WIN_AUTOBATTLE);
        }
        if (winAutoBattleSimple != nullptr) {
            CgWindowClose(session, WIN_AUTOBATTLE_SIMPLE);
        }
    }
}



} // namespace cg::script
