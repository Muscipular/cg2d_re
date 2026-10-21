#include "battle_skill_effect.hpp"
#include "battle_message.hpp"
#include "../script/LuaInternal.hpp"
#include "cg/core/m_log.hpp"

#include "animation.hpp"
#include "battle_actor.hpp"
#include "battle_base.hpp"
#include "battle_effect.hpp"
#include "battle_scene.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"

#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

// E10：迁自 EffectHooks/luaSkillEffect.cpp，脚本合同见 EFM01；所有对象均借用原生 Yobi 链。
namespace cg::yobi::detail {

static constexpr int kBattleEntryCount = 20;
static constexpr int kMaxQueueItems = 199;
static constexpr int kMaxPacketFields = 0x1000 / 2;
static constexpr int kMaxActionState = 256;
static constexpr int kEndActionState = 236;
static constexpr int kPacketBufferSize = 0x1000;
static constexpr int kLuaSkillMetaFlag = 1;
static constexpr size_t kMaxOperationsPerBatch = 80;
static constexpr size_t kMaxSortedYobiActions = 247;
static constexpr int kSortedYobiFlag = 0x20000;
static constexpr uint32_t kSpecialBattleEntryMask = 0x00300000;
static constexpr const char* kPacketReaderMetaTable = "BattleSkill.PacketReader";
static constexpr const char* kLuaSkillScript = "lua/battleSkillEffect.lua";
static constexpr const char* kBattlePacketHeaders[] = {
    "SKL|", "ESC|", "CLR|", "SUP|", "POS|", "MON|", "EQU|", "ITM|", "CMB|",
    "PSN|", "ANR|", "PUE|", "ACT|", "CFP|", "CHT|", "PNO|", "PLV|", "CBC|",
    "PDN|", "REB|", "BST|", "GMN|", "RID|", "DSK|", "END|",
};

// Skill.register的双ID键、报文字段数及本技能Lua状态持有的解析引用。
struct SkillRegistration {
    int skillId;
    int techId;
    int intCount;
    int parseRef;
};

// 当前SKL的只读字段索引；epoch使上次回调保存的PacketReader失效。
struct PacketView {
    bool active = false;
    uint32_t epoch = 0;
    int startOffset = 0;
    int endOffset = 0;
    int failureOffset = 0;
    int intCount = 0;
    std::vector<int> offsets;
};

struct PacketReaderUserData {
    uint32_t epoch;
};

// 动作三个阶段的Lua注册表引用，提交后由RuntimeCastBinding持有。
struct CallbackRefs {
    int onEnter{LUA_NOREF};
    int onEffect{LUA_NOREF};
    int onAdvance{LUA_NOREF};
};

struct PendingCast {
    int actionId = 0;
    CallbackRefs callbacks;
};

struct PendingEntry {
    int position = 0;
    size_t castBegin = 0;
    size_t castCount = 0;
    size_t effectBegin = 0;
    size_t effectCount = 0;
};

struct PendingChild {
    int sourcePos = 0;
    int targetPos = 0;
    int spawnMode = 0;
    int waitMode = 0;
    size_t castBegin = 0;
    size_t castCount = 0;
    size_t effectBegin = 0;
    size_t effectCount = 0;
};

struct PendingVisual {
    int sourcePos = 0;
    int targetPos = 0;
    int waitMode = 0;
    int actionState = 0;
    int imageNo = 0;
    int charAction = 0;
    int dir = 0;
    float scaleX{1.0f};
    float scaleY{1.0f};
    bool hasX = false;
    bool hasY = false;
    float x = 0.0f;
    float y = 0.0f;
    int updateRef{LUA_NOREF};
};

struct PendingVisualFrame {
    bool hasImageNo = false;
    int imageNo = 0;
    bool hasCharAction = false;
    int charAction = 0;
    bool hasDir = false;
    int dir = 0;
    bool hasScaleX = false;
    float scaleX = 0.0f;
    bool hasScaleY = false;
    float scaleY = 0.0f;
    bool hasX = false;
    float x = 0.0f;
    bool hasY = false;
    float y = 0.0f;
    bool markDeath = false;
};

struct PendingOperation {
    int opId = 0;
    int position = 0;
    int value = 0;
    size_t hitIndex = 0;
};

struct PendingHitTargetCast {
    int position = 0;
    int actionId = 0;
};

struct PendingHit {
    BattleEntryEffectRecord effect{};
    size_t ownerCastIndex = 0;
    size_t targetCastBegin = 0;
    size_t targetCastCount = 0;
};

// 一次脚本回调的待提交结果；ownedRefs在失败时释放，成功时转交运行表。
struct PendingBatch {
    bool hasNextActionState = false;
    int nextActionState = 0;
    int onCompleteRef{LUA_NOREF};
    size_t appendCastBegin = 0;
    size_t appendCastCount = 0;
    size_t appendEffectBegin = 0;
    size_t appendEffectCount = 0;
    std::vector<PendingEntry> entries;
    std::vector<PendingChild> children;
    std::vector<PendingVisual> visuals;
    std::vector<PendingCast> casts;
    std::vector<BattleEntryEffectRecord> effects;
    std::vector<PendingOperation> operations;
    std::vector<PendingHit> hits;
    std::vector<PendingHitTargetCast> hitTargetCasts;
    std::vector<int> ownedRefs;
    PendingVisualFrame visualFrame;
};

// 运行表只观察Yobi对象；完成、逻辑死亡、物理释放和子树等待分别记录。
struct RuntimeNode {
    uint32_t id = 0;
    uint32_t parentId = 0;
    YobiAction* action = nullptr;
    int waitMode = 0;
    int observedActionState{std::numeric_limits<int>::min()};
    int observedCastOrdinal{std::numeric_limits<int>::min()};
    int observedEffectRunning = 0;
    uint32_t transitionGeneration = 0;
    uint32_t enteredTransitionGeneration = 0;
    int pendingNextActionState = 0;
    bool temporary = false;
    bool pendingBind = false;
    bool actionDead = false;
    bool deathMarked = false;
    bool actionComplete = false;
    bool nativeFinishPending = false;
    bool nativeCleanup = false;
    bool pendingAdvance = false;
    bool pendingHasNextActionState = false;
    bool pendingWasVirtual = false;
    bool activationPending = false;
    bool visual = false;
    bool visualFinishPending = false;
    int visualUpdateRef{LUA_NOREF};
    uint32_t visualTick = 0;
    int visualSourcePos = 0;
    int visualTargetPos = 0;
    float visualStartX = 0.0f;
    float visualStartY = 0.0f;
    uint32_t queueOwnerNodeId = 0;
};

// 将实际动作队列序号关联到阶段回调及受击编号，不另建动作执行队列。
struct RuntimeCastBinding {
    uint32_t nodeId = 0;
    int castOrdinal = 0;
    int actionId = 0;
    int hitId = 0;
    int hitEffectIndex{-1};
    CallbackRefs callbacks;
};

// 单个被接管SKL的场景、根节点和回调所有权；结束后继续原报文游标。
struct LuaSkillRuntime {
    bool active = false;
    bool aborting = false;
    bool inCallback = false;
    bool completionCalled = false;
    uint32_t generation = 0;
    uint32_t nextNodeId{1};
    int nextHitId{1};
    int rawSkillId = 0;
    int rawTechId = 0;
    int skillLevel = 0;
    int skillLevel2 = 0;
    int normalizedSkillLevel = 0;
    int normalizedSkillLevel2 = 0;
    int casterPos = 0;
    int packetStart = 0;
    int packetEnd = 0;
    int onCompleteRef{LUA_NOREF};
    YobiAction* scene = nullptr;
    std::vector<uint32_t> rootNodeIds;
    std::vector<RuntimeNode> nodes;
    std::vector<RuntimeCastBinding> castBindings;
};

// 原生分配已成功但尚未提交的对象；批次失败时仅标死，由Yobi链释放。
struct StagedChild {
    YobiAction* action = nullptr;
    uint32_t nodeId = 0;
    size_t pendingIndex = 0;
};

struct StagedVisual {
    YobiAction* action = nullptr;
    uint32_t nodeId = 0;
    size_t pendingIndex = 0;
    float startX = 0.0f;
    float startY = 0.0f;
};

// 减益批次预创建时保存／恢复真实角色字段，成功后按操作顺序发布。
struct DebuffState {
    int flags = 0;
    int debuffState = 0;
    int debuffType = 0;
    YobiAction* buffYobi = nullptr;
};

struct StagedDebuff {
    size_t operationIndex = 0;
    YobiAction* target = nullptr;
    YobiAction* effect = nullptr;
    DebuffState desiredState;
};

// 本线22槽的非拥有观察指针；脚本普通角色仍只允许0–19。
static YobiAction** const battleEntries = world::gBattleAllEntries59E980.data();
static lua_State* gSkillEffectL = nullptr;
static std::vector<SkillRegistration> gSkillRegistrations;
static PacketView gPacketView;
static PendingBatch gPendingBatch;
static LuaSkillRuntime gLuaSkillRuntime;
static std::vector<StagedChild> gStagedChildren;
static std::vector<StagedVisual> gStagedVisuals;
static std::vector<StagedDebuff> gStagedDebuffs;
static std::vector<YobiAction*> gDeathMarkedActions;
static uint32_t gRuntimeGeneration = 0;

static void LuaVisualUpdate(YobiAction* action);

static char* PacketBuffer() {
    return world::gBattleMText5AAA7C.data();
}

static void NormalizeSkillLevels(int techId, int skillLevel, int skillLevel2,
                                 int& normalizedSkillLevel, int& normalizedSkillLevel2) {
    if (techId >= 0x320C8) {
        normalizedSkillLevel = skillLevel % 30;
        if (normalizedSkillLevel < 1) {
            normalizedSkillLevel = 1;
        } else if (normalizedSkillLevel > 15) {
            normalizedSkillLevel = 15;
        }
        if (skillLevel2 < 1) {
            skillLevel2 = 1;
        } else if (skillLevel2 > 15) {
            skillLevel2 = 15;
        }
        normalizedSkillLevel2 = skillLevel2 - 1;
        return;
    }

    int level = techId % 100;
    if (level >= 50) {
        level %= 50;
    } else if (level >= 30) {
        level %= 30;
    }
    if (level < 0) {
        level = 0;
    } else if (level > 14) {
        level = 14;
    }
    normalizedSkillLevel = level + 1;
    normalizedSkillLevel2 = level;
}

static void SetCustomSkillGlobals(int casterPos, int skillId, int techId,
                                  int normalizedSkillLevel, int normalizedSkillLevel2) {
    gBattleMCasterPosition5AAA64 = casterPos;
    gBattleMSkill5AAA78 = skillId;
    gBattleMTechnique5A4A38 = techId;
    gBattleMSkillLevel5A4A2C = normalizedSkillLevel;
    gBattleMSkillLevelIndex5A3A18 = normalizedSkillLevel2;
}

static int LuaAbsIndex(lua_State* L, int index) {
    if (index > 0 || index <= LUA_REGISTRYINDEX) {
        return index;
    }
    return lua_gettop(L) + index + 1;
}

static void LuaRawGetField(lua_State* L, int index, const char* field) {
    index = LuaAbsIndex(L, index);
    lua_pushstring(L, field);
    lua_rawget(L, index);
}

static bool LuaReadStrictInt(lua_State* L, int index, int& value) {
    if (lua_type(L, index) != LUA_TNUMBER) {
        return false;
    }
    lua_Number number = lua_tonumber(L, index);
    if (!(number >= static_cast<lua_Number>(std::numeric_limits<int>::min()) &&
          number <= static_cast<lua_Number>(std::numeric_limits<int>::max())) ||
        std::floor(number) != number) {
        return false;
    }
    value = static_cast<int>(number);
    return true;
}

static int LuaRequireIntField(lua_State* L, int index, const char* field, int minimum, int maximum) {
    LuaRawGetField(L, index, field);
    int value = 0;
    if (!LuaReadStrictInt(L, -1, value) || value < minimum || value > maximum) {
        luaL_error(L, "%s must be an integer in [%d, %d]", field, minimum, maximum);
    }
    lua_pop(L, 1);
    return value;
}

static int LuaOptionalIntField(lua_State* L, int index, const char* field, int defaultValue,
                               int minimum, int maximum) {
    LuaRawGetField(L, index, field);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return defaultValue;
    }
    int value = 0;
    if (!LuaReadStrictInt(L, -1, value) || value < minimum || value > maximum) {
        luaL_error(L, "%s must be an integer in [%d, %d]", field, minimum, maximum);
    }
    lua_pop(L, 1);
    return value;
}

static bool LuaReadOptionalIntField(lua_State* L, int index, const char* field, int minimum,
                                    int maximum, int& value) {
    LuaRawGetField(L, index, field);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return false;
    }
    if (!LuaReadStrictInt(L, -1, value) || value < minimum || value > maximum) {
        luaL_error(L, "%s must be an integer in [%d, %d]", field, minimum, maximum);
    }
    lua_pop(L, 1);
    return true;
}

static bool LuaOptionalFloatField(lua_State* L, int index, const char* field, float& value) {
    LuaRawGetField(L, index, field);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return false;
    }
    if (lua_type(L, -1) != LUA_TNUMBER) {
        luaL_error(L, "%s must be a finite number", field);
    }
    lua_Number number = lua_tonumber(L, -1);
    if (!std::isfinite(number) ||
        number < static_cast<lua_Number>(std::numeric_limits<float>::lowest()) ||
        number > static_cast<lua_Number>(std::numeric_limits<float>::max())) {
        luaL_error(L, "%s must be a finite float", field);
    }
    value = static_cast<float>(number);
    lua_pop(L, 1);
    return true;
}

static bool LuaOptionalBoolField(lua_State* L, int index, const char* field,
                                 bool defaultValue) {
    LuaRawGetField(L, index, field);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return defaultValue;
    }
    if (lua_type(L, -1) != LUA_TBOOLEAN) {
        luaL_error(L, "%s must be a boolean", field);
    }
    bool value = lua_toboolean(L, -1) != 0;
    lua_pop(L, 1);
    return value;
}

static size_t LuaRequireDenseArray(lua_State* L, int index, const char* field, size_t maximum) {
    index = LuaAbsIndex(L, index);
    if (!lua_istable(L, index)) {
        luaL_error(L, "%s must be an array", field);
    }
    size_t length = lua_objlen(L, index);
    if (length > maximum) {
        luaL_error(L, "%s exceeds capacity %u", field, static_cast<unsigned>(maximum));
    }
    for (size_t i = 1; i <= length; ++i) {
        lua_rawgeti(L, index, static_cast<int>(i));
        if (lua_isnil(L, -1)) {
            luaL_error(L, "%s must be dense", field);
        }
        lua_pop(L, 1);
    }
    size_t keyCount = 0;
    lua_pushnil(L);
    while (lua_next(L, index) != 0) {
        int key = 0;
        if (!LuaReadStrictInt(L, -2, key) || key <= 0 || static_cast<size_t>(key) > length) {
            luaL_error(L, "%s contains a non-array key", field);
        }
        ++keyCount;
        lua_pop(L, 1);
    }
    if (keyCount != length) {
        luaL_error(L, "%s must be dense", field);
    }
    return length;
}

static void LuaUnref(int& ref) {
    if (gSkillEffectL && ref != LUA_NOREF && ref != LUA_REFNIL) {
        luaL_unref(gSkillEffectL, LUA_REGISTRYINDEX, ref);
    }
    ref = LUA_NOREF;
}

static void ClearCallbackRefs(CallbackRefs& refs) {
    LuaUnref(refs.onEnter);
    LuaUnref(refs.onEffect);
    LuaUnref(refs.onAdvance);
}

static void ResetPendingBatch(bool releaseRefs) {
    if (releaseRefs) {
        for (int ref : gPendingBatch.ownedRefs) {
            if (gSkillEffectL && ref != LUA_NOREF && ref != LUA_REFNIL) {
                luaL_unref(gSkillEffectL, LUA_REGISTRYINDEX, ref);
            }
        }
    }
    gPendingBatch.hasNextActionState = false;
    gPendingBatch.nextActionState = 0;
    gPendingBatch.onCompleteRef = LUA_NOREF;
    gPendingBatch.appendCastBegin = 0;
    gPendingBatch.appendCastCount = 0;
    gPendingBatch.appendEffectBegin = 0;
    gPendingBatch.appendEffectCount = 0;
    gPendingBatch.entries.clear();
    gPendingBatch.children.clear();
    gPendingBatch.visuals.clear();
    gPendingBatch.casts.clear();
    gPendingBatch.effects.clear();
    gPendingBatch.operations.clear();
    gPendingBatch.hits.clear();
    gPendingBatch.hitTargetCasts.clear();
    gPendingBatch.ownedRefs.clear();
    gPendingBatch.visualFrame = {};
}

static bool DecodeHexField(int offset, int& nextOffset, int& value) {
    if (offset < 0 || offset >= kPacketBufferSize) {
        return false;
    }
    uint32_t result = 0;
    bool hasDigit = false;
    int digitCount = 0;
    int cursor = offset;
    while (cursor < kPacketBufferSize) {
        unsigned char ch = static_cast<unsigned char>(PacketBuffer()[cursor]);
        if (ch == '|') {
            if (!hasDigit) {
                return false;
            }
            nextOffset = cursor + 1;
            value = static_cast<int>(result);
            return true;
        }
        uint32_t digit = 0;
        if (ch >= '0' && ch <= '9') {
            digit = ch - '0';
        } else if (ch >= 'A' && ch <= 'F') {
            digit = ch - 'A' + 10;
        } else {
            return false;
        }
        if (++digitCount > 8) {
            return false;
        }
        hasDigit = true;
        result = result * 16 + digit;
        ++cursor;
    }
    return false;
}

static bool ReadSkillKey(int startOffset, int& techId, int& skillId) {
    int offset = startOffset;
    int ignored = 0;
    if (!DecodeHexField(offset, offset, techId)) {
        return false;
    }
    if (!DecodeHexField(offset, offset, ignored)) {
        return false;
    }
    return DecodeHexField(offset, offset, skillId);
}

static SkillRegistration* FindRegistration(int skillId, int techId) {
    for (SkillRegistration& registration : gSkillRegistrations) {
        if (registration.skillId == skillId && registration.techId == techId) {
            return &registration;
        }
    }
    return nullptr;
}

static bool BuildPacketView(int startOffset, int intCount) {
    gPacketView.active = false;
    gPacketView.offsets.clear();
    gPacketView.startOffset = startOffset;
    gPacketView.endOffset = startOffset;
    gPacketView.failureOffset = startOffset;
    gPacketView.intCount = intCount;
    gPacketView.offsets.reserve(static_cast<size_t>(intCount) + 1);
    gPacketView.offsets.push_back(startOffset);
    int offset = startOffset;
    for (int i = 0; i < intCount; ++i) {
        int value = 0;
        if (!DecodeHexField(offset, offset, value)) {
            gPacketView.failureOffset = offset;
            return false;
        }
        gPacketView.offsets.push_back(offset);
    }
    ++gPacketView.epoch;
    if (gPacketView.epoch == 0) {
        ++gPacketView.epoch;
    }
    gPacketView.startOffset = startOffset;
    gPacketView.endOffset = offset;
    gPacketView.failureOffset = offset;
    gPacketView.intCount = intCount;
    gPacketView.active = true;
    return true;
}

static bool IsBattlePacketHeader(int offset) {
    if (offset < 0 || offset + 4 > kPacketBufferSize) {
        return false;
    }
    char* buffer = PacketBuffer();
    for (const char* header : kBattlePacketHeaders) {
        if (buffer[offset] == header[0] && buffer[offset + 1] == header[1] &&
            buffer[offset + 2] == header[2] && buffer[offset + 3] == header[3]) {
            return true;
        }
    }
    return false;
}

static int FindNextBattlePacketHeader(int startOffset) {
    constexpr size_t kSkillKeyFieldCount = 3;
    for (size_t i = kSkillKeyFieldCount; i < gPacketView.offsets.size(); ++i) {
        int offset = gPacketView.offsets[i];
        if (IsBattlePacketHeader(offset)) {
            return offset;
        }
    }
    if (startOffset < 0) {
        startOffset = 0;
    }
    char* buffer = PacketBuffer();
    for (int offset = startOffset; offset + 4 <= kPacketBufferSize; ++offset) {
        if (buffer[offset] == '\0') {
            break;
        }
        if (offset != startOffset && buffer[offset - 1] != '|') {
            continue;
        }
        if (IsBattlePacketHeader(offset)) {
            return offset;
        }
    }
    return -1;
}

static void TerminateInvalidSkillPacket(YobiAction* scene) {
    BattleScenePayload& sceneData = *scene->payload.battleScene;
    int nextHeaderOffset = FindNextBattlePacketHeader(gPacketView.failureOffset);
    if (nextHeaderOffset >= 0) {
        m_log_iw(L"[LuaSkill] resync invalid SKL packet: failureOffset=%d nextOffset=%d\n",
              gPacketView.failureOffset, nextHeaderOffset);
        sceneData.packetOffset64 = nextHeaderOffset;
    } else {
        m_log_iw(L"[LuaSkill] discard invalid BattleScene packet group: failureOffset=%d\n",
              gPacketView.failureOffset);
        sceneData.remainingMessages5C = 1;
    }
    gPacketView.active = false;
    FinishBattleEvent_43B6C0(scene);
}

static bool ReadPacketInt(int pos, int& value) {
    if (!gPacketView.active || pos <= 0 || pos > gPacketView.intCount) {
        return false;
    }
    int nextOffset = 0;
    return DecodeHexField(gPacketView.offsets[static_cast<size_t>(pos - 1)], nextOffset, value) &&
           nextOffset == gPacketView.offsets[static_cast<size_t>(pos)];
}

static bool IsActionDeathMarked(YobiAction* action) {
    for (YobiAction* marked : gDeathMarkedActions) {
        if (marked == action) {
            return true;
        }
    }
    return false;
}

static YobiAction* MarkActionDeathOnce(YobiAction* action) {
    if (!action || IsActionDeathMarked(action)) {
        return action;
    }
    gDeathMarkedActions.push_back(action);
    action->deathFlag = 1; // 原404D00本体；运行节点已在调用方完成逻辑标死。
    return action;
}

static void ForgetDeathMarkedAction(YobiAction* action) {
    for (size_t i = 0; i < gDeathMarkedActions.size(); ++i) {
        if (gDeathMarkedActions[i] == action) {
            gDeathMarkedActions.erase(gDeathMarkedActions.begin() + i);
            return;
        }
    }
}

static int LuaPacketReaderSize(lua_State* L) {
    auto* reader = static_cast<PacketReaderUserData*>(luaL_checkudata(L, 1, kPacketReaderMetaTable));
    if (!reader || !gPacketView.active || reader->epoch != gPacketView.epoch) {
        return luaL_error(L, "PacketReader is no longer valid");
    }
    lua_pushinteger(L, gPacketView.intCount);
    return 1;
}

static int LuaPacketReaderGetInt(lua_State* L) {
    auto* reader = static_cast<PacketReaderUserData*>(luaL_checkudata(L, 1, kPacketReaderMetaTable));
    if (!reader || !gPacketView.active || reader->epoch != gPacketView.epoch) {
        return luaL_error(L, "PacketReader is no longer valid");
    }
    int pos = 0;
    if (!LuaReadStrictInt(L, 2, pos) || pos <= 0 || pos > gPacketView.intCount) {
        return luaL_error(L, "PacketReader index is out of range");
    }
    int value = 0;
    if (!ReadPacketInt(pos, value)) {
        return luaL_error(L, "PacketReader field is not a valid integer");
    }
    lua_pushinteger(L, value);
    return 1;
}

static int LuaSkillRegister(lua_State* L) {
    if (lua_gettop(L) != 1 || !lua_istable(L, 1)) {
        return luaL_error(L, "Skill.register expects one table");
    }
    int skillId = LuaRequireIntField(L, 1, "skillId", std::numeric_limits<int>::min(),
                                     std::numeric_limits<int>::max());
    int techId = LuaRequireIntField(L, 1, "techId", std::numeric_limits<int>::min(),
                                    std::numeric_limits<int>::max());
    int intCount = LuaRequireIntField(L, 1, "intCount", 5, kMaxPacketFields);
    if (FindRegistration(skillId, techId)) {
        return luaL_error(L, "duplicate skillId/techId registration");
    }
    LuaRawGetField(L, 1, "parse");
    if (!lua_isfunction(L, -1)) {
        return luaL_error(L, "parse must be a function");
    }
    int parseRef = luaL_ref(L, LUA_REGISTRYINDEX);
    gSkillRegistrations.push_back({skillId, techId, intCount, parseRef});
    return 0;
}

static int RefOptionalCallback(lua_State* L, int index, const char* field) {
    LuaRawGetField(L, index, field);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return LUA_NOREF;
    }
    if (!lua_isfunction(L, -1)) {
        luaL_error(L, "%s must be a function", field);
    }
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    gPendingBatch.ownedRefs.push_back(ref);
    return ref;
}

static int RefRequiredCallback(lua_State* L, int index, const char* field) {
    LuaRawGetField(L, index, field);
    if (!lua_isfunction(L, -1)) {
        luaL_error(L, "%s must be a function", field);
    }
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    gPendingBatch.ownedRefs.push_back(ref);
    return ref;
}

static void ReadCallbackRefs(lua_State* L, int index, CallbackRefs& callbacks) {
    callbacks.onEnter = RefOptionalCallback(L, index, "onEnter");
    callbacks.onEffect = RefOptionalCallback(L, index, "onEffect");
    callbacks.onAdvance = RefOptionalCallback(L, index, "onAdvance");
}

static void ReadCast(lua_State* L, int index) {
    PendingCast cast{};
    if (lua_type(L, index) == LUA_TNUMBER) {
        if (!LuaReadStrictInt(L, index, cast.actionId) || cast.actionId < 0 ||
            cast.actionId > kMaxActionState) {
            luaL_error(L, "cast actionId must be an integer in [0, 256]");
        }
    } else if (lua_istable(L, index)) {
        cast.actionId = LuaRequireIntField(L, index, "actionId", 0, kMaxActionState);
        ReadCallbackRefs(L, index, cast.callbacks);
    } else {
        luaL_error(L, "cast item must be an actionId or table");
    }
    gPendingBatch.casts.push_back(cast);
}

static void ReadCastArrayField(lua_State* L, int index, const char* field, bool required,
                               size_t& begin, size_t& count) {
    index = LuaAbsIndex(L, index);
    begin = gPendingBatch.casts.size();
    count = 0;
    LuaRawGetField(L, index, field);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        if (required) {
            luaL_error(L, "%s is required", field);
        }
        return;
    }
    int arrayIndex = LuaAbsIndex(L, -1);
    size_t length = LuaRequireDenseArray(L, arrayIndex, field, kMaxQueueItems);
    if (required && length == 0) {
        luaL_error(L, "%s cannot be empty", field);
    }
    for (size_t i = 1; i <= length; ++i) {
        lua_rawgeti(L, arrayIndex, static_cast<int>(i));
        ReadCast(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    count = length;
}

static void ReadEffect(lua_State* L, int index) {
    if (!lua_istable(L, index)) {
        luaL_error(L, "effect item must be a table");
    }
    BattleEntryEffectRecord effect{};
    effect.position = LuaRequireIntField(L, index, "pos", 0, kBattleEntryCount - 1);
    effect.flags = LuaRequireIntField(L, index, "flg", std::numeric_limits<int>::min(),
                                    std::numeric_limits<int>::max());
    effect.position3 = LuaRequireIntField(L, index, "pos3", std::numeric_limits<int>::min(),
                                     std::numeric_limits<int>::max());
    effect.position4 = LuaOptionalIntField(L, index, "pos4", 0, std::numeric_limits<int>::min(),
                                      std::numeric_limits<int>::max());
    gPendingBatch.effects.push_back(effect);
}

static void ReadEffectArrayField(lua_State* L, int index, const char* field, size_t& begin,
                                 size_t& count) {
    index = LuaAbsIndex(L, index);
    begin = gPendingBatch.effects.size();
    count = 0;
    LuaRawGetField(L, index, field);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    int arrayIndex = LuaAbsIndex(L, -1);
    size_t length = LuaRequireDenseArray(L, arrayIndex, field, kMaxQueueItems);
    for (size_t i = 1; i <= length; ++i) {
        lua_rawgeti(L, arrayIndex, static_cast<int>(i));
        ReadEffect(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    count = length;
}

static void ReadEntry(lua_State* L, int index) {
    if (!lua_istable(L, index)) {
        luaL_error(L, "entry item must be a table");
    }
    PendingEntry entry{};
    entry.position = LuaRequireIntField(L, index, "pos", 0, kBattleEntryCount - 1);
    ReadCastArrayField(L, index, "castList", true, entry.castBegin, entry.castCount);
    ReadEffectArrayField(L, index, "effectList", entry.effectBegin, entry.effectCount);
    gPendingBatch.entries.push_back(entry);
}

static void ReadEntries(lua_State* L, int index) {
    index = LuaAbsIndex(L, index);
    LuaRawGetField(L, index, "entries");
    if (lua_isnil(L, -1)) {
        luaL_error(L, "entries is required");
    }
    int arrayIndex = LuaAbsIndex(L, -1);
    size_t length = LuaRequireDenseArray(L, arrayIndex, "entries", kBattleEntryCount);
    if (length == 0) {
        luaL_error(L, "entries cannot be empty");
    }
    for (size_t i = 1; i <= length; ++i) {
        lua_rawgeti(L, arrayIndex, static_cast<int>(i));
        ReadEntry(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

static void ReadChildren(lua_State* L, int index) {
    index = LuaAbsIndex(L, index);
    LuaRawGetField(L, index, "children");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    int arrayIndex = LuaAbsIndex(L, -1);
    size_t length = LuaRequireDenseArray(L, arrayIndex, "children", kMaxSortedYobiActions);
    for (size_t i = 1; i <= length; ++i) {
        lua_rawgeti(L, arrayIndex, static_cast<int>(i));
        if (!lua_istable(L, -1)) {
            luaL_error(L, "child item must be a table");
        }
        PendingChild child{};
        child.sourcePos = LuaRequireIntField(L, -1, "sourcePos", 0, kBattleEntryCount - 1);
        child.targetPos = LuaRequireIntField(L, -1, "targetPos", 0, kBattleEntryCount - 1);
        child.spawnMode = LuaRequireIntField(L, -1, "spawnMode", 0, 1);
        child.waitMode = LuaOptionalIntField(L, -1, "waitMode", 0, 0, 2);
        ReadCastArrayField(L, -1, "castList", true, child.castBegin, child.castCount);
        ReadEffectArrayField(L, -1, "effectList", child.effectBegin, child.effectCount);
        gPendingBatch.children.push_back(child);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

static void ReadVisuals(lua_State* L, int index) {
    index = LuaAbsIndex(L, index);
    LuaRawGetField(L, index, "visuals");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    int arrayIndex = LuaAbsIndex(L, -1);
    size_t length = LuaRequireDenseArray(L, arrayIndex, "visuals", kMaxSortedYobiActions);
    for (size_t i = 1; i <= length; ++i) {
        lua_rawgeti(L, arrayIndex, static_cast<int>(i));
        if (!lua_istable(L, -1)) {
            luaL_error(L, "visual item must be a table");
        }
        PendingVisual visual{};
        visual.sourcePos =
            LuaRequireIntField(L, -1, "sourcePos", 0, kBattleEntryCount - 1);
        visual.targetPos =
            LuaRequireIntField(L, -1, "targetPos", 0, kBattleEntryCount - 1);
        visual.waitMode = LuaOptionalIntField(L, -1, "waitMode", 0, 0, 2);
        visual.actionState = LuaOptionalIntField(L, -1, "actionState", 0, 0,
                                                 kMaxActionState);
        visual.imageNo = LuaRequireIntField(L, -1, "imageNo", 0,
                                            std::numeric_limits<int>::max());
        visual.charAction = LuaOptionalIntField(L, -1, "charAction", 0, 0,
                                                kMaxActionState);
        visual.dir = LuaOptionalIntField(L, -1, "dir", 0, 0, 7);
        LuaOptionalFloatField(L, -1, "scaleX", visual.scaleX);
        LuaOptionalFloatField(L, -1, "scaleY", visual.scaleY);
        visual.hasX = LuaOptionalFloatField(L, -1, "x", visual.x);
        visual.hasY = LuaOptionalFloatField(L, -1, "y", visual.y);
        visual.updateRef = RefRequiredCallback(L, -1, "update");
        gPendingBatch.visuals.push_back(visual);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

static size_t ReadHit(lua_State* L, int index) {
    index = LuaAbsIndex(L, index);
    PendingHit hit{};

    LuaRawGetField(L, index, "effect");
    if (!lua_istable(L, -1)) {
        luaL_error(L, "operation effect must be a table");
    }
    hit.effect.position = LuaRequireIntField(L, -1, "pos", 0, kBattleEntryCount - 1);
    hit.effect.flags = LuaRequireIntField(L, -1, "flg", std::numeric_limits<int>::min(),
                                        std::numeric_limits<int>::max());
    hit.effect.position3 = LuaRequireIntField(L, -1, "value3", std::numeric_limits<int>::min(),
                                         std::numeric_limits<int>::max());
    hit.effect.position4 = LuaRequireIntField(L, -1, "value4", std::numeric_limits<int>::min(),
                                         std::numeric_limits<int>::max());
    lua_pop(L, 1);

    LuaRawGetField(L, index, "ownerCast");
    if (!lua_istable(L, -1)) {
        luaL_error(L, "operation ownerCast must be a table");
    }
    hit.ownerCastIndex = gPendingBatch.casts.size();
    ReadCast(L, -1);
    lua_pop(L, 1);

    hit.targetCastBegin = gPendingBatch.hitTargetCasts.size();
    LuaRawGetField(L, index, "targetCasts");
    if (!lua_istable(L, -1)) {
        luaL_error(L, "operation targetCasts must be an array");
    }
    int targetCastsIndex = LuaAbsIndex(L, -1);
    size_t targetCastCount =
        LuaRequireDenseArray(L, targetCastsIndex, "targetCasts", kMaxQueueItems);
    for (size_t i = 1; i <= targetCastCount; ++i) {
        lua_rawgeti(L, targetCastsIndex, static_cast<int>(i));
        if (!lua_istable(L, -1)) {
            luaL_error(L, "targetCast item must be a table");
        }
        PendingHitTargetCast targetCast{};
        targetCast.position = LuaRequireIntField(L, -1, "pos", 0, kBattleEntryCount - 1);
        targetCast.actionId =
            LuaRequireIntField(L, -1, "actionId", 0, kMaxActionState);
        gPendingBatch.hitTargetCasts.push_back(targetCast);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    hit.targetCastCount = targetCastCount;

    size_t hitIndex = gPendingBatch.hits.size();
    gPendingBatch.hits.push_back(hit);
    return hitIndex;
}

static void ReadOperations(lua_State* L, int index) {
    index = LuaAbsIndex(L, index);
    LuaRawGetField(L, index, "operations");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    int arrayIndex = LuaAbsIndex(L, -1);
    size_t length =
        LuaRequireDenseArray(L, arrayIndex, "operations", kMaxOperationsPerBatch);
    for (size_t i = 1; i <= length; ++i) {
        lua_rawgeti(L, arrayIndex, static_cast<int>(i));
        if (!lua_istable(L, -1)) {
            luaL_error(L, "operation item must be a table");
        }
        PendingOperation operation{};
        operation.opId = LuaRequireIntField(L, -1, "opId", 1, 4);
        if (operation.opId == 4) {
            operation.hitIndex = ReadHit(L, -1);
        } else {
            operation.position = LuaRequireIntField(L, -1, "pos", 0, kBattleEntryCount - 1);
            operation.value = LuaOptionalIntField(L, -1, "value", 0,
                                                   std::numeric_limits<int>::min(),
                                                   std::numeric_limits<int>::max());
            if (operation.opId == 1 || operation.opId == 2) {
                LuaRawGetField(L, -1, "value");
                bool hasValue = !lua_isnil(L, -1);
                lua_pop(L, 1);
                if (!hasValue) {
                    luaL_error(L, "operation value is required");
                }
            }
        }
        gPendingBatch.operations.push_back(operation);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

static void RejectRootCallbackField(lua_State* L, int index, const char* field) {
    index = LuaAbsIndex(L, index);
    LuaRawGetField(L, index, field);
    bool present = !lua_isnil(L, -1);
    lua_pop(L, 1);
    if (present) {
        luaL_error(L, "%s is only valid in callback results", field);
    }
}

static void DecodeRootPlan(lua_State* L, int index) {
    if (!lua_istable(L, index)) {
        luaL_error(L, "parse must return a table");
    }
    RejectRootCallbackField(L, index, "nextActionState");
    RejectRootCallbackField(L, index, "appendCast");
    RejectRootCallbackField(L, index, "appendEffect");
    RejectRootCallbackField(L, index, "children");
    RejectRootCallbackField(L, index, "visuals");
    RejectRootCallbackField(L, index, "operations");
    ReadEntries(L, index);
    gPendingBatch.onCompleteRef = RefOptionalCallback(L, index, "onComplete");
}

static void DecodeCallbackResult(lua_State* L, int index, bool allowNextActionState) {
    if (lua_isnil(L, index)) {
        return;
    }
    if (!lua_istable(L, index)) {
        luaL_error(L, "callback must return nil or a table");
    }
    LuaRawGetField(L, index, "nextActionState");
    if (!lua_isnil(L, -1)) {
        int nextActionState = 0;
        if (!allowNextActionState) {
            luaL_error(L, "nextActionState is only valid in onAdvance");
        }
        if (!LuaReadStrictInt(L, -1, nextActionState) || nextActionState > kMaxActionState) {
            luaL_error(L, "nextActionState must be an integer <= 256");
        }
        gPendingBatch.hasNextActionState = true;
        gPendingBatch.nextActionState = nextActionState;
    }
    lua_pop(L, 1);
    ReadCastArrayField(L, index, "appendCast", false, gPendingBatch.appendCastBegin,
                       gPendingBatch.appendCastCount);
    ReadEffectArrayField(L, index, "appendEffect", gPendingBatch.appendEffectBegin,
                          gPendingBatch.appendEffectCount);
    ReadChildren(L, index);
    ReadVisuals(L, index);
    ReadOperations(L, index);
}

static void DecodeVisualFrame(lua_State* L, int index) {
    PendingVisualFrame& frame = gPendingBatch.visualFrame;
    frame.hasImageNo = LuaReadOptionalIntField(
        L, index, "imageNo", 0, std::numeric_limits<int>::max(), frame.imageNo);
    frame.hasCharAction = LuaReadOptionalIntField(
        L, index, "charAction", 0, kMaxActionState, frame.charAction);
    frame.hasDir = LuaReadOptionalIntField(L, index, "dir", 0, 7, frame.dir);
    frame.hasScaleX = LuaOptionalFloatField(L, index, "scaleX", frame.scaleX);
    frame.hasScaleY = LuaOptionalFloatField(L, index, "scaleY", frame.scaleY);
    frame.hasX = LuaOptionalFloatField(L, index, "x", frame.x);
    frame.hasY = LuaOptionalFloatField(L, index, "y", frame.y);
    frame.markDeath = LuaOptionalBoolField(L, index, "markDeath", false);
}

static void LuaSetIntField(lua_State* L, const char* field, int value) {
    lua_pushinteger(L, value);
    lua_setfield(L, -2, field);
}

static void LuaSetNumberField(lua_State* L, const char* field, lua_Number value) {
    lua_pushnumber(L, value);
    lua_setfield(L, -2, field);
}

static void PushPacketReader(lua_State* L) {
    auto* reader = static_cast<PacketReaderUserData*>(lua_newuserdata(L, sizeof(PacketReaderUserData)));
    reader->epoch = gPacketView.epoch;
    luaL_getmetatable(L, kPacketReaderMetaTable);
    lua_setmetatable(L, -2);
}

static void PushParseContext(lua_State* L, int skillId, int techId, int skillLevel,
                             int skillLevel2, int casterPos) {
    lua_newtable(L);
    LuaSetIntField(L, "skillId", skillId);
    LuaSetIntField(L, "techId", techId);
    LuaSetIntField(L, "skillLevel", skillLevel);
    LuaSetIntField(L, "skillLevel2", skillLevel2);
    LuaSetIntField(L, "casterPos", casterPos);
    LuaSetIntField(L, "hitId", 0);
}

static int LuaParseTrampoline(lua_State* L) {
    int parseRef = 0;
    if (!LuaReadStrictInt(L, 1, parseRef)) {
        return luaL_error(L, "invalid parse reference");
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, parseRef);
    if (!lua_isfunction(L, -1)) {
        return luaL_error(L, "parse reference is not a function");
    }
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_call(L, 2, 1);
    DecodeRootPlan(L, -1);
    return 0;
}

static int LuaCallbackTrampoline(lua_State* L) {
    int callbackRef = 0;
    if (!LuaReadStrictInt(L, 1, callbackRef)) {
        return luaL_error(L, "invalid callback reference");
    }
    bool allowNextActionState = lua_toboolean(L, 3) != 0;
    lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
    if (!lua_isfunction(L, -1)) {
        return luaL_error(L, "callback reference is not a function");
    }
    lua_pushvalue(L, 2);
    lua_call(L, 1, 1);
    DecodeCallbackResult(L, -1, allowNextActionState);
    return 0;
}

static int LuaVisualUpdateTrampoline(lua_State* L) {
    int callbackRef = 0;
    if (!LuaReadStrictInt(L, 1, callbackRef)) {
        return luaL_error(L, "invalid visual update reference");
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, callbackRef);
    if (!lua_isfunction(L, -1)) {
        return luaL_error(L, "visual update reference is not a function");
    }
    lua_pushvalue(L, 2);
    lua_call(L, 1, 1);
    if (lua_isnil(L, -1)) {
        return 0;
    }
    if (lua_type(L, -1) == LUA_TNUMBER) {
        int nextActionState = 0;
        if (!LuaReadStrictInt(L, -1, nextActionState) || nextActionState > kMaxActionState) {
            return luaL_error(L, "visual update state must be an integer <= 256");
        }
        gPendingBatch.hasNextActionState = true;
        gPendingBatch.nextActionState = nextActionState;
        return 0;
    }
    if (!lua_istable(L, -1)) {
        return luaL_error(L, "visual update must return nil, an integer, or a table");
    }
    DecodeCallbackResult(L, -1, true);
    DecodeVisualFrame(L, -1);
    return 0;
}

static bool RunParseCallback(const SkillRegistration& registration, int skillId, int techId,
                             int skillLevel, int skillLevel2, int casterPos) {
    ResetPendingBatch(true);
    int top = lua_gettop(gSkillEffectL);
    lua_pushcfunction(gSkillEffectL, script::SafeLuaCall<LuaParseTrampoline>);
    lua_pushinteger(gSkillEffectL, registration.parseRef);
    PushPacketReader(gSkillEffectL);
    PushParseContext(gSkillEffectL, skillId, techId, skillLevel, skillLevel2, casterPos);
    int status = lua_pcall(gSkillEffectL, 3, 0, 0);
    gPacketView.active = false;
    if (status != 0) {
        [[maybe_unused]] const char* error = lua_tostring(gSkillEffectL, -1);
        m_log_iw(L"[LuaSkill] parse failed: skillId=%d techId=%d error=%hs\n", skillId, techId,
              error ? error : "unknown");
        lua_settop(gSkillEffectL, top);
        ResetPendingBatch(true);
        return false;
    }
    lua_settop(gSkillEffectL, top);
    return true;
}

static RuntimeNode* FindNodeById(uint32_t nodeId) {
    for (RuntimeNode& node : gLuaSkillRuntime.nodes) {
        if (node.id == nodeId) {
            return &node;
        }
    }
    return nullptr;
}

static RuntimeNode* FindNodeByAction(YobiAction* action) {
    if (!gLuaSkillRuntime.active || !action) {
        return nullptr;
    }
    for (RuntimeNode& node : gLuaSkillRuntime.nodes) {
        if (!node.actionDead && node.action == action) {
            return &node;
        }
    }
    return nullptr;
}

static RuntimeNode* FindAnyNodeByAction(YobiAction* action) {
    if (!gLuaSkillRuntime.active || !action) {
        return nullptr;
    }
    for (RuntimeNode& node : gLuaSkillRuntime.nodes) {
        if (node.action == action) {
            return &node;
        }
    }
    return nullptr;
}

static uint32_t ResolveQueueOwnerNodeId(uint32_t nodeId) {
    RuntimeNode* node = FindNodeById(nodeId);
    if (!node) {
        return 0;
    }
    return node->visual ? node->queueOwnerNodeId : node->id;
}

static RuntimeCastBinding* FindCastBinding(uint32_t nodeId, int castOrdinal) {
    for (RuntimeCastBinding& binding : gLuaSkillRuntime.castBindings) {
        if (binding.nodeId == nodeId && binding.castOrdinal == castOrdinal) {
            return &binding;
        }
    }
    return nullptr;
}

static bool IsSubtreeDead(uint32_t nodeId) {
    RuntimeNode* node = FindNodeById(nodeId);
    if (!node || !node->actionDead) {
        return false;
    }
    for (const RuntimeNode& child : gLuaSkillRuntime.nodes) {
        if (child.parentId == nodeId && !IsSubtreeDead(child.id)) {
            return false;
        }
    }
    return true;
}

static bool IsNodeBlocked(uint32_t nodeId) {
    for (const RuntimeNode& child : gLuaSkillRuntime.nodes) {
        if (child.parentId != nodeId || child.waitMode == 0) {
            continue;
        }
        if (child.waitMode == 1 && !child.actionDead) {
            return true;
        }
        if (child.waitMode == 2 && !IsSubtreeDead(child.id)) {
            return true;
        }
    }
    return false;
}

static bool AllTemporaryNodesDead() {
    for (const RuntimeNode& node : gLuaSkillRuntime.nodes) {
        if (node.temporary && !node.actionDead) {
            return false;
        }
    }
    return true;
}

static bool AllRootNodesComplete() {
    for (uint32_t nodeId : gLuaSkillRuntime.rootNodeIds) {
        RuntimeNode* node = FindNodeById(nodeId);
        if (!node || !node->actionComplete) {
            return false;
        }
    }
    return true;
}

static void PushCallbackContext(lua_State* L, uint32_t nodeId, int hitId) {
    RuntimeNode* node = FindNodeById(nodeId);
    lua_newtable(L);
    LuaSetIntField(L, "skillId", gLuaSkillRuntime.rawSkillId);
    LuaSetIntField(L, "techId", gLuaSkillRuntime.rawTechId);
    LuaSetIntField(L, "skillLevel", gLuaSkillRuntime.skillLevel);
    LuaSetIntField(L, "skillLevel2", gLuaSkillRuntime.skillLevel2);
    LuaSetIntField(L, "casterPos", gLuaSkillRuntime.casterPos);
    LuaSetIntField(L, "nodeId", static_cast<int>(nodeId));
    LuaSetIntField(L, "hitId", hitId);
    LuaSetIntField(L, "parentNodeId", node ? static_cast<int>(node->parentId) : 0);
    if (node && node->action && node->action->payload.battleActor) {
        LuaSetIntField(L, "pos", node->action->payload.battleActor->battlePosition18);
        LuaSetIntField(L, "actionState", node->action->phase);
        LuaSetIntField(L, "effectRunning", node->action->payload.battleActor->effectRunningC9C);
    }
}

static bool PushVisualUpdateContext(lua_State* L, const RuntimeNode& node) {
    if (!node.action || !node.action->payload.battleActor || node.visualTargetPos < 0 ||
        node.visualTargetPos >= kBattleEntryCount || !battleEntries ||
        !battleEntries[node.visualTargetPos] || !battleEntries[node.visualTargetPos]->payload.battleActor) {
        return false;
    }
    YobiAction* action = node.action;
    YobiAction* target = battleEntries[node.visualTargetPos];
    if (!std::isfinite(target->offset40) || !std::isfinite(target->offset44)) {
        return false;
    }
    PushCallbackContext(L, node.id, 0);
    LuaSetNumberField(L, "tick", static_cast<lua_Number>(node.visualTick));
    LuaSetIntField(L, "sourcePos", node.visualSourcePos);
    LuaSetIntField(L, "targetPos", node.visualTargetPos);
    LuaSetIntField(L, "ownerNodeId", static_cast<int>(node.queueOwnerNodeId));
    LuaSetIntField(L, "imageNo", action->imageNo);
    LuaSetIntField(L, "charAction", action->characterAction);
    LuaSetIntField(L, "dir", action->direction);
    LuaSetNumberField(L, "scaleX", action->scaleX);
    LuaSetNumberField(L, "scaleY", action->scaleY);
    LuaSetNumberField(L, "x", action->offset40);
    LuaSetNumberField(L, "y", action->offset44);
    LuaSetNumberField(L, "startX", node.visualStartX);
    LuaSetNumberField(L, "startY", node.visualStartY);
    LuaSetNumberField(L, "targetX", target->offset40);
    LuaSetNumberField(L, "targetY", target->offset44);
    return true;
}

static bool RunLuaCallback(int callbackRef, uint32_t nodeId, bool allowNextActionState,
                           [[maybe_unused]] const char* callbackName, int hitId = 0) {
    if (callbackRef == LUA_NOREF || callbackRef == LUA_REFNIL) {
        ResetPendingBatch(true);
        return true;
    }
    ResetPendingBatch(true);
    int top = lua_gettop(gSkillEffectL);
    lua_pushcfunction(gSkillEffectL, script::SafeLuaCall<LuaCallbackTrampoline>);
    lua_pushinteger(gSkillEffectL, callbackRef);
    PushCallbackContext(gSkillEffectL, nodeId, hitId);
    lua_pushboolean(gSkillEffectL, allowNextActionState ? 1 : 0);
    gLuaSkillRuntime.inCallback = true;
    int status = lua_pcall(gSkillEffectL, 3, 0, 0);
    gLuaSkillRuntime.inCallback = false;
    if (status != 0) {
        [[maybe_unused]] const char* error = lua_tostring(gSkillEffectL, -1);
        m_log_iw(L"[LuaSkill] %hs failed: skillId=%d techId=%d nodeId=%u error=%hs\n",
              callbackName, gLuaSkillRuntime.rawSkillId, gLuaSkillRuntime.rawTechId, nodeId,
              error ? error : "unknown");
        lua_settop(gSkillEffectL, top);
        ResetPendingBatch(true);
        return false;
    }
    lua_settop(gSkillEffectL, top);
    return true;
}

static bool RunVisualUpdateCallback(uint32_t nodeId) {
    RuntimeNode* node = FindNodeById(nodeId);
    if (!node || !node->visual || node->visualUpdateRef == LUA_NOREF ||
        node->visualUpdateRef == LUA_REFNIL) {
        ResetPendingBatch(true);
        return false;
    }
    ResetPendingBatch(true);
    int top = lua_gettop(gSkillEffectL);
    lua_pushcfunction(gSkillEffectL, script::SafeLuaCall<LuaVisualUpdateTrampoline>);
    lua_pushinteger(gSkillEffectL, node->visualUpdateRef);
    if (!PushVisualUpdateContext(gSkillEffectL, *node)) {
        lua_settop(gSkillEffectL, top);
        ResetPendingBatch(true);
        return false;
    }
    gLuaSkillRuntime.inCallback = true;
    int status = lua_pcall(gSkillEffectL, 2, 0, 0);
    gLuaSkillRuntime.inCallback = false;
    if (status != 0) {
        [[maybe_unused]] const char* error = lua_tostring(gSkillEffectL, -1);
        m_log_iw(L"[LuaSkill] visual update failed: skillId=%d techId=%d nodeId=%u error=%hs\n",
              gLuaSkillRuntime.rawSkillId, gLuaSkillRuntime.rawTechId, nodeId,
              error ? error : "unknown");
        lua_settop(gSkillEffectL, top);
        ResetPendingBatch(true);
        return false;
    }
    lua_settop(gSkillEffectL, top);
    return true;
}

static void ClearRuntimeCallbackRefs() {
    LuaUnref(gLuaSkillRuntime.onCompleteRef);
    for (RuntimeCastBinding& binding : gLuaSkillRuntime.castBindings) {
        ClearCallbackRefs(binding.callbacks);
    }
    for (RuntimeNode& node : gLuaSkillRuntime.nodes) {
        LuaUnref(node.visualUpdateRef);
    }
}

static void ResetRuntime() {
    ClearRuntimeCallbackRefs();
    gLuaSkillRuntime.active = false;
    gLuaSkillRuntime.aborting = false;
    gLuaSkillRuntime.inCallback = false;
    gLuaSkillRuntime.completionCalled = false;
    gLuaSkillRuntime.rawSkillId = 0;
    gLuaSkillRuntime.rawTechId = 0;
    gLuaSkillRuntime.skillLevel = 0;
    gLuaSkillRuntime.skillLevel2 = 0;
    gLuaSkillRuntime.normalizedSkillLevel = 0;
    gLuaSkillRuntime.normalizedSkillLevel2 = 0;
    gLuaSkillRuntime.casterPos = 0;
    gLuaSkillRuntime.packetStart = 0;
    gLuaSkillRuntime.packetEnd = 0;
    gLuaSkillRuntime.onCompleteRef = LUA_NOREF;
    gLuaSkillRuntime.scene = nullptr;
    gLuaSkillRuntime.rootNodeIds.clear();
    gLuaSkillRuntime.nodes.clear();
    gLuaSkillRuntime.castBindings.clear();
    gLuaSkillRuntime.nextNodeId = 1;
    gLuaSkillRuntime.nextHitId = 1;
}

static uint32_t AddRuntimeNode(YobiAction* action, uint32_t parentId, int waitMode,
                               bool temporary, bool pendingBind, bool visual = false,
                               uint32_t queueOwnerNodeId = 0) {
    RuntimeNode node{};
    node.id = gLuaSkillRuntime.nextNodeId++;
    node.parentId = parentId;
    node.action = action;
    node.waitMode = waitMode;
    node.temporary = temporary;
    node.pendingBind = pendingBind;
    node.visual = visual;
    node.queueOwnerNodeId = visual ? queueOwnerNodeId : node.id;
    node.observedEffectRunning = action && action->payload.battleActor
                                     ? action->payload.battleActor->effectRunningC9C
                                     : 0;
    gLuaSkillRuntime.nodes.push_back(node);
    return node.id;
}

static void ClearPendingAdvance(RuntimeNode& node) {
    node.pendingAdvance = false;
    node.pendingHasNextActionState = false;
    node.pendingNextActionState = 0;
    node.pendingWasVirtual = false;
}

static void RecordNodeTransition(RuntimeNode& node) {
    if (++node.transitionGeneration == 0) {
        node.transitionGeneration = 1;
        node.enteredTransitionGeneration = 0;
    }
    node.observedActionState = node.action->phase;
    node.observedCastOrdinal = node.action->payload.battleActor->castPlayIndexFC4;
}

static void ObserveNodeTransition(RuntimeNode& node) {
    int castOrdinal = node.action->payload.battleActor->castPlayIndexFC4;
    if (node.observedActionState != node.action->phase ||
        node.observedCastOrdinal != castOrdinal) {
        RecordNodeTransition(node);
    }
}

static void InitializeActionQueues(YobiAction* action) {
    BattleActorPayload& data = *action->payload.battleActor;
    data.valueFCC = 0;
    data.valueFD0 = 0;
    data.valueFD4 = 0;
    data.busyFD8 = 0;
    data.effectFlagsFE4 = 0;
    data.valueFF0 = 0;
    data.aliveFF8 = 0;
    data.value1024 = 0;
    data.value1038 = 0;
    for (BattleEntryEffectRecord& effect : data.effects1C) {
        effect = {-1, 0, 0, 0};
    }
    data.effectRunningC9C = 0;
    data.effectCountCA0 = 0;
    for (int& cast : data.castListCA4) {
        cast = -1;
    }
    data.castPlayIndexFC4 = 0;
    data.castListIndexFC8 = -1;
    data.queuedTargets1084[0] = -1;
    data.queuedTargets1084[1] = -1;
    data.value13A4 = 0;
    data.queuedTargetCount13A8 = 0;
    data.castSkill103C = gLuaSkillRuntime.rawSkillId;
    data.castLevel1040 = gLuaSkillRuntime.normalizedSkillLevel;
    data.castSkillLevel1044 = gLuaSkillRuntime.normalizedSkillLevel2;
}

static void AppendPendingCast(uint32_t nodeId, YobiAction* caster, YobiAction* target,
                              size_t index, int hitId = 0, int hitEffectIndex = -1) {
    PendingCast& pending = gPendingBatch.casts[index];
    int ordinal = target->payload.battleActor->castListIndexFC8 + 1;
    AppendBattleCast_4392A0(caster, target, pending.actionId);
    gLuaSkillRuntime.castBindings.push_back(
        {nodeId, ordinal, pending.actionId, hitId, hitEffectIndex, pending.callbacks});
}

static void AppendPendingCasts(uint32_t nodeId, YobiAction* caster, YobiAction* target,
                               size_t begin, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        AppendPendingCast(nodeId, caster, target, begin + i);
    }
}

static void AppendPendingEffects(YobiAction* target, size_t begin, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        AppendBattleEffect_439400(target, gPendingBatch.effects[begin + i]);
    }
}

static bool ValidatePendingEffects(size_t begin, size_t count) {
    if (begin > gPendingBatch.effects.size() ||
        count > gPendingBatch.effects.size() - begin) {
        return false;
    }
    for (size_t i = 0; i < count; ++i) {
        int pos = gPendingBatch.effects[begin + i].position;
        if (!battleEntries[pos] || !battleEntries[pos]->payload.battleActor) {
            return false;
        }
    }
    return true;
}

static bool ValidateRootPlan() {
    if (!battleEntries) {
        return false;
    }
    for (size_t i = 0; i < gPendingBatch.entries.size(); ++i) {
        const PendingEntry& entry = gPendingBatch.entries[i];
        if (!battleEntries[entry.position] || !battleEntries[entry.position]->payload.battleActor ||
            entry.castCount == 0 || entry.castCount > kMaxQueueItems ||
            entry.effectCount > kMaxQueueItems ||
            !ValidatePendingEffects(entry.effectBegin, entry.effectCount)) {
            return false;
        }
        for (size_t j = 0; j < i; ++j) {
            if (gPendingBatch.entries[j].position == entry.position) {
                return false;
            }
        }
    }
    return true;
}

static bool CommitRootPlan(YobiAction* scene, int skillId, int techId, int skillLevel,
                           int skillLevel2, int normalizedSkillLevel,
                           int normalizedSkillLevel2, int casterPos) {
    if (!ValidateRootPlan() || casterPos < 0 || casterPos >= kBattleEntryCount ||
        !battleEntries[casterPos] || !battleEntries[casterPos]->payload.battleActor) {
        m_log_iw(L"[LuaSkill] root plan validation failed: skillId=%d techId=%d casterPos=%d\n",
              skillId, techId, casterPos);
        return false;
    }
    ResetRuntime();
    ++gRuntimeGeneration;
    if (gRuntimeGeneration == 0) {
        ++gRuntimeGeneration;
    }
    gLuaSkillRuntime.active = true;
    gLuaSkillRuntime.generation = gRuntimeGeneration;
    gLuaSkillRuntime.rawSkillId = skillId;
    gLuaSkillRuntime.rawTechId = techId;
    gLuaSkillRuntime.skillLevel = skillLevel;
    gLuaSkillRuntime.skillLevel2 = skillLevel2;
    gLuaSkillRuntime.normalizedSkillLevel = normalizedSkillLevel;
    gLuaSkillRuntime.normalizedSkillLevel2 = normalizedSkillLevel2;
    gLuaSkillRuntime.casterPos = casterPos;
    gLuaSkillRuntime.packetStart = gPacketView.startOffset;
    gLuaSkillRuntime.packetEnd = gPacketView.endOffset;
    gLuaSkillRuntime.scene = scene;
    gLuaSkillRuntime.onCompleteRef = gPendingBatch.onCompleteRef;
    uint32_t rootEntryBitFlag = 0;
    for (const PendingEntry& entry : gPendingBatch.entries) {
        rootEntryBitFlag |= uint32_t{1} << entry.position;
    }
    uint32_t originalEntryBitFlag = static_cast<uint32_t>(scene->payload.battleScene->entryMaskA8);
    scene->payload.battleScene->entryMaskA8 =
        static_cast<int>((originalEntryBitFlag & kSpecialBattleEntryMask) | rootEntryBitFlag);

    for (const PendingEntry& entry : gPendingBatch.entries) {
        YobiAction* action = battleEntries[entry.position];
        action->value54 = 1.0f;
        action->hidden = 0;
        InitializeActionQueues(action);
        uint32_t nodeId = AddRuntimeNode(action, 0, 0, false, true);
        gLuaSkillRuntime.rootNodeIds.push_back(nodeId);
        AppendPendingCasts(nodeId, battleEntries[casterPos], action, entry.castBegin,
                           entry.castCount);
        AppendPendingEffects(action, entry.effectBegin, entry.effectCount);
    }

    gPendingBatch.ownedRefs.clear();
    for (uint32_t nodeId : gLuaSkillRuntime.rootNodeIds) {
        RuntimeNode* node = FindNodeById(nodeId);
        MoveBattleCastState_439380(node->action);
    }
    for (uint32_t nodeId : gLuaSkillRuntime.rootNodeIds) {
        RuntimeNode* node = FindNodeById(nodeId);
        if (!node || node->pendingBind) {
            m_log_iw(L"[LuaSkill] root MoveState bind failed: skillId=%d techId=%d nodeId=%u\n",
                  skillId, techId, nodeId);
            return false;
        }
    }
    gPendingBatch.ownedRefs.clear();
    ResetPendingBatch(false);
    return true;
}

struct QueueGrowth {
    YobiAction* action = nullptr;
    size_t castCount = 0;
    size_t effectCount = 0;
};

static bool AddQueueGrowth(std::vector<QueueGrowth>& growths, YobiAction* action,
                           size_t castCount, size_t effectCount) {
    if (!action || !action->payload.battleActor || castCount > kMaxQueueItems ||
        effectCount > kMaxQueueItems) {
        return false;
    }
    for (QueueGrowth& growth : growths) {
        if (growth.action != action) {
            continue;
        }
        if (castCount > kMaxQueueItems - growth.castCount ||
            effectCount > kMaxQueueItems - growth.effectCount) {
            return false;
        }
        growth.castCount += castCount;
        growth.effectCount += effectCount;
        return true;
    }
    growths.push_back({action, castCount, effectCount});
    return true;
}

static bool ValidateQueueGrowth(const QueueGrowth& growth) {
    const BattleActorPayload& data = *growth.action->payload.battleActor;
    if (data.castListIndexFC8 < -1 || data.castListIndexFC8 >= kMaxQueueItems ||
        data.effectCountCA0 < 0 || data.effectCountCA0 > kMaxQueueItems) {
        return false;
    }
    size_t castCount = static_cast<size_t>(data.castListIndexFC8 + 1);
    size_t effectCount = static_cast<size_t>(data.effectCountCA0);
    return growth.castCount <= kMaxQueueItems - castCount &&
           growth.effectCount <= kMaxQueueItems - effectCount;
}

static bool ValidateOperationBatch(uint32_t nodeId) {
    struct SimulatedDebuffState {
        YobiAction* target;
        bool active;
    };
    std::vector<SimulatedDebuffState> debuffStates;
    size_t hitCount = 0;
    for (size_t i = 0; i < gPendingBatch.operations.size(); ++i) {
        const PendingOperation& operation = gPendingBatch.operations[i];
        if (operation.opId == 4) {
            if (nodeId == 0 || operation.hitIndex != hitCount ||
                operation.hitIndex >= gPendingBatch.hits.size()) {
                return false;
            }
            const PendingHit& hit = gPendingBatch.hits[operation.hitIndex];
            if (hit.ownerCastIndex >= gPendingBatch.casts.size() ||
                hit.targetCastBegin > gPendingBatch.hitTargetCasts.size() ||
                hit.targetCastCount >
                    gPendingBatch.hitTargetCasts.size() - hit.targetCastBegin ||
                hit.effect.position < 0 || hit.effect.position >= kBattleEntryCount ||
                !battleEntries[hit.effect.position] || !battleEntries[hit.effect.position]->payload.battleActor) {
                return false;
            }
            for (size_t j = 0; j < hit.targetCastCount; ++j) {
                const PendingHitTargetCast& targetCast =
                    gPendingBatch.hitTargetCasts[hit.targetCastBegin + j];
                if (targetCast.position < 0 || targetCast.position >= kBattleEntryCount ||
                    !battleEntries[targetCast.position] || !battleEntries[targetCast.position]->payload.battleActor) {
                    return false;
                }
            }
            ++hitCount;
            continue;
        }
        YobiAction* target = battleEntries[operation.position];
        if (!target || !target->payload.battleActor) {
            return false;
        }
        if (operation.opId == 1) {
            int maxHp = target->maxHp;
            if (operation.value < 0 || operation.value > maxHp) {
                return false;
            }
        } else if (operation.opId == 2) {
            if (operation.value < 110075 || operation.value > 110080) {
                return false;
            }
        } else if (operation.opId != 3) {
            return false;
        }
        if (operation.opId == 2 || operation.opId == 3) {
            SimulatedDebuffState* state = nullptr;
            for (SimulatedDebuffState& candidate : debuffStates) {
                if (candidate.target == target) {
                    state = &candidate;
                    break;
                }
            }
            if (!state) {
                debuffStates.push_back({target, target->payload.battleActor->effect1054 != nullptr});
                state = &debuffStates.back();
            }
            if (operation.opId == 2) {
                if (state->active) {
                    return false;
                }
                state->active = true;
            } else {
                state->active = false;
            }
        }
    }
    return hitCount == gPendingBatch.hits.size();
}

static bool HasSortedYobiCapacity(size_t additionalCount) {
    YobiAction* const start = gYobiActionStart;
    YobiAction* const end = gYobiActionEnd;
    if (!start || !end) {
        return false;
    }

    size_t count = 0;
    YobiAction* action = start->next;
    for (; action && action != end; action = action->next) {
        if ((action->flags & kSortedYobiFlag) != 0 && ++count > kMaxSortedYobiActions) {
            return false;
        }
    }
    return action == end && additionalCount <= kMaxSortedYobiActions - count;
}

static bool ValidateCallbackBatch(uint32_t nodeId) {
    uint32_t queueOwnerNodeId = nodeId == 0 ? 0 : ResolveQueueOwnerNodeId(nodeId);
    if (!battleEntries || !ValidateOperationBatch(queueOwnerNodeId)) {
        return false;
    }
    size_t additionalSortedYobiCount =
        gPendingBatch.children.size() + gPendingBatch.visuals.size();
    for (const PendingOperation& operation : gPendingBatch.operations) {
        if (operation.opId == 2) {
            ++additionalSortedYobiCount;
        }
    }
    if (!HasSortedYobiCapacity(additionalSortedYobiCount)) {
        return false;
    }
    if (nodeId == 0) {
        return gPendingBatch.appendCastCount == 0 && gPendingBatch.appendEffectCount == 0 &&
               gPendingBatch.children.empty() && gPendingBatch.visuals.empty() &&
               gPendingBatch.hits.empty() && gPendingBatch.hitTargetCasts.empty();
    }
    RuntimeNode* callbackNode = FindNodeById(nodeId);
    RuntimeNode* queueOwnerNode = FindNodeById(queueOwnerNodeId);
    if (!callbackNode || callbackNode->actionDead || !callbackNode->action ||
        !callbackNode->action->payload.battleActor || !queueOwnerNode || queueOwnerNode->visual ||
        queueOwnerNode->actionDead || !queueOwnerNode->action ||
        !queueOwnerNode->action->payload.battleActor) {
        return false;
    }
    BattleActorPayload& data = *queueOwnerNode->action->payload.battleActor;
    YobiAction* caster = data.caster1070 ? data.caster1070 : battleEntries[gLuaSkillRuntime.casterPos];
    if (!caster || !caster->payload.battleActor ||
        !ValidatePendingEffects(gPendingBatch.appendEffectBegin,
                                gPendingBatch.appendEffectCount) ||
        gPendingBatch.hits.size() >
            static_cast<size_t>(std::numeric_limits<int>::max() -
                                gLuaSkillRuntime.nextHitId)) {
        return false;
    }
    std::vector<QueueGrowth> growths;
    if (!AddQueueGrowth(growths, queueOwnerNode->action,
                        gPendingBatch.appendCastCount + gPendingBatch.hits.size(),
                        gPendingBatch.appendEffectCount + gPendingBatch.hits.size())) {
        return false;
    }
    for (const PendingHit& hit : gPendingBatch.hits) {
        for (size_t i = 0; i < hit.targetCastCount; ++i) {
            const PendingHitTargetCast& targetCast =
                gPendingBatch.hitTargetCasts[hit.targetCastBegin + i];
            if (!AddQueueGrowth(growths, battleEntries[targetCast.position], 1, 0)) {
                return false;
            }
        }
    }
    for (const QueueGrowth& growth : growths) {
        if (!ValidateQueueGrowth(growth)) {
            return false;
        }
    }
    for (const PendingChild& child : gPendingBatch.children) {
        if (!battleEntries[child.sourcePos] || !battleEntries[child.sourcePos]->payload.battleActor ||
            !battleEntries[child.targetPos] || !battleEntries[child.targetPos]->payload.battleActor ||
            child.castCount == 0 || child.castCount > kMaxQueueItems ||
            child.effectCount > kMaxQueueItems ||
            !ValidatePendingEffects(child.effectBegin, child.effectCount)) {
            return false;
        }
    }
    for (const PendingVisual& visual : gPendingBatch.visuals) {
        if (!battleEntries[visual.sourcePos] || !battleEntries[visual.sourcePos]->payload.battleActor ||
            !battleEntries[visual.targetPos] || !battleEntries[visual.targetPos]->payload.battleActor ||
            visual.updateRef == LUA_NOREF || visual.updateRef == LUA_REFNIL) {
            return false;
        }
    }
    return true;
}

static bool SyncVisualPosition(YobiAction* action) {
    if (!action || !std::isfinite(action->offset40) || !std::isfinite(action->offset44)) {
        return false;
    }
    double gX = std::trunc(static_cast<double>(action->offset40)) +
                std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(gYobiScreenOffsetX));
    double gY = std::trunc(static_cast<double>(action->offset44)) +
                std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(gYobiScreenOffsetY));
    if (gX < static_cast<double>(std::numeric_limits<int>::min()) ||
        gX > static_cast<double>(std::numeric_limits<int>::max()) ||
        gY < static_cast<double>(std::numeric_limits<int>::min()) ||
        gY > static_cast<double>(std::numeric_limits<int>::max())) {
        return false;
    }
    action->x = static_cast<int>(gX);
    action->y = static_cast<int>(gY);
    return true;
}

static void RollbackStagedChildren() {
    for (StagedChild& child : gStagedChildren) {
        if (child.action) {
            MarkActionDeathOnce(child.action);
        }
    }
    gStagedChildren.clear();
}

static bool StageChildren() {
    gStagedChildren.clear();
    for (size_t i = 0; i < gPendingBatch.children.size(); ++i) {
        const PendingChild& pending = gPendingBatch.children[i];
        YobiAction* child = NewBattleActor_438290(pending.targetPos);
        if (!child || !child->payload.battleActor) {
            if (child) {
                MarkActionDeathOnce(child);
            }
            RollbackStagedChildren();
            return false;
        }
        YobiAction* source = battleEntries[pending.sourcePos];
        child->imageNo = source->imageNo;
        child->characterAction = source->characterAction;
        child->direction = source->direction;
        child->scaleX = source->scaleX;
        child->scaleY = source->scaleY;
        if (pending.spawnMode == 1) {
            child->offset40 = source->offset40;
            child->offset44 = source->offset44;
            child->x = source->x;
            child->y = source->y;
        }
        child->value1D4 = pending.targetPos;
        InitializeActionQueues(child);
        PlayFrame_517DA0(child, 0);
        gStagedChildren.push_back({child, 0, i});
    }
    return true;
}

static void RollbackStagedVisuals() {
    for (StagedVisual& visual : gStagedVisuals) {
        if (visual.action) {
            MarkActionDeathOnce(visual.action);
        }
    }
    gStagedVisuals.clear();
}

static bool StageVisuals() {
    gStagedVisuals.clear();
    for (size_t i = 0; i < gPendingBatch.visuals.size(); ++i) {
        const PendingVisual& pending = gPendingBatch.visuals[i];
        YobiAction* visual = NewBattleActor_438290(pending.targetPos);
        if (!visual || !visual->payload.battleActor) {
            if (visual) {
                MarkActionDeathOnce(visual);
            }
            RollbackStagedVisuals();
            return false;
        }
        YobiAction* source = battleEntries[pending.sourcePos];
        float startX = source->offset40;
        float startY = source->offset44;
        visual->phase = pending.actionState;
        visual->imageNo = pending.imageNo;
        visual->characterAction = pending.charAction;
        visual->direction = pending.dir;
        visual->scaleX = pending.scaleX;
        visual->scaleY = pending.scaleY;
        visual->offset40 = pending.hasX ? pending.x : startX;
        visual->offset44 = pending.hasY ? pending.y : startY;
        visual->value1D4 = pending.targetPos;
        InitializeActionQueues(visual);
        visual->callback = LuaVisualUpdate;
        PlayFrame_517DA0(visual, 0);
        if (!SyncVisualPosition(visual)) {
            MarkActionDeathOnce(visual);
            RollbackStagedVisuals();
            return false;
        }
        gStagedVisuals.push_back({visual, 0, i, startX, startY});
    }
    return true;
}

static void RollbackStagedDebuffs() {
    for (StagedDebuff& staged : gStagedDebuffs) {
        if (staged.effect) {
            MarkActionDeathOnce(staged.effect);
        }
    }
    gStagedDebuffs.clear();
}

static DebuffState ReadDebuffState(YobiAction* target) {
    BattleActorPayload& data = *target->payload.battleActor;
    return {static_cast<int>(data.flagsFDC), data.debuffState13CC, data.debuffType13D0,
            data.effect1054};
}

static void WriteDebuffState(YobiAction* target, const DebuffState& state) {
    BattleActorPayload& data = *target->payload.battleActor;
    data.flagsFDC = static_cast<std::uint32_t>(state.flags);
    data.debuffState13CC = state.debuffState;
    data.debuffType13D0 = state.debuffType;
    data.effect1054 = state.buffYobi;
}

static bool StageDebuffOperations() {
    struct DebuffShadow {
        YobiAction* target;
        DebuffState state;
    };
    std::vector<DebuffShadow> shadows;
    gStagedDebuffs.clear();
    for (size_t i = 0; i < gPendingBatch.operations.size(); ++i) {
        const PendingOperation& operation = gPendingBatch.operations[i];
        if (operation.opId != 2 && operation.opId != 3) {
            continue;
        }
        YobiAction* target = battleEntries[operation.position];
        DebuffShadow* shadow = nullptr;
        for (DebuffShadow& candidate : shadows) {
            if (candidate.target == target) {
                shadow = &candidate;
                break;
            }
        }
        if (!shadow) {
            shadows.push_back({target, ReadDebuffState(target)});
            shadow = &shadows.back();
        }
        if (operation.opId == 3) {
            shadow->state.buffYobi = nullptr;
            shadow->state.flags &= 0xFFFFFC0F;
            continue;
        }
        if (shadow->state.buffYobi) {
            RollbackStagedDebuffs();
            return false;
        }

        DebuffState actualState = ReadDebuffState(target);
        WriteDebuffState(target, shadow->state);
        YobiAction* effect = NewBattleDebuffEffect_41F660(target, operation.value);
        DebuffState desiredState = ReadDebuffState(target);
        WriteDebuffState(target, actualState);
        if (!effect || desiredState.buffYobi != effect) {
            if (effect) {
                MarkActionDeathOnce(effect);
            }
            RollbackStagedDebuffs();
            return false;
        }
        gStagedDebuffs.push_back({i, target, effect, desiredState});
        shadow->state = desiredState;
    }
    return true;
}

static void AppendReactionCast(YobiAction* caster, const PendingHitTargetCast& pending) {
    YobiAction* target = battleEntries[pending.position];
    BattleActorPayload& data = *target->payload.battleActor;
    RuntimeNode* node = FindNodeByAction(target);
    bool reactivate = data.castPlayIndexFC4 == data.castListIndexFC8 + 1 &&
                      (target->phase == kEndActionState ||
                       (node && node->actionComplete));
    AppendBattleCast_4392A0(caster, target, pending.actionId);
    if (!reactivate || data.castPlayIndexFC4 > data.castListIndexFC8) {
        return;
    }
    target->phase = data.castListCA4[data.castPlayIndexFC4];
    if (node) {
        ClearPendingAdvance(*node);
        node->actionComplete = false;
        node->nativeFinishPending = false;
        node->nativeCleanup = false;
        RecordNodeTransition(*node);
    }
}

static void CommitOperations(uint32_t nodeId) {
    RuntimeNode* ownerNode = nodeId == 0 ? nullptr : FindNodeById(nodeId);
    YobiAction* owner = ownerNode ? ownerNode->action : nullptr;
    YobiAction* caster = owner && owner->payload.battleActor ? owner->payload.battleActor->caster1070 : nullptr;
    if (!caster && battleEntries) {
        caster = battleEntries[gLuaSkillRuntime.casterPos];
    }
    size_t stagedDebuffIndex = 0;
    for (size_t i = 0; i < gPendingBatch.operations.size(); ++i) {
        const PendingOperation& operation = gPendingBatch.operations[i];
        if (operation.opId == 4) {
            const PendingHit& hit = gPendingBatch.hits[operation.hitIndex];
            int hitId = gLuaSkillRuntime.nextHitId++;
            int effectIndex = owner->payload.battleActor->effectCountCA0;
            AppendPendingCast(nodeId, caster, owner, hit.ownerCastIndex, hitId, effectIndex);
            AppendBattleEffect_439400(owner, hit.effect);
            for (size_t targetIndex = 0; targetIndex < hit.targetCastCount; ++targetIndex) {
                AppendReactionCast(
                    caster, gPendingBatch.hitTargetCasts[hit.targetCastBegin + targetIndex]);
            }
            continue;
        }
        YobiAction* target = battleEntries[operation.position];
        if (operation.opId == 1) {
            target->readiness = operation.value;
        } else if (operation.opId == 2) {
            const StagedDebuff& staged = gStagedDebuffs[stagedDebuffIndex++];
            WriteDebuffState(staged.target, staged.desiredState);
        } else if (operation.opId == 3) {
            BattleActorPayload& data = *target->payload.battleActor;
            if (data.effect1054) {
                MarkActionDeathOnce(data.effect1054);
                data.effect1054 = nullptr;
            }
            data.flagsFDC = static_cast<std::uint32_t>(static_cast<int>(data.flagsFDC) & 0xFFFFFC0F);
        }
    }
}

static void CommitStagedChildren(uint32_t parentNodeId) {
    RuntimeNode* parent = FindNodeById(parentNodeId);
    YobiAction* parentAction = parent->action;
    RuntimeNode* queueOwner = FindNodeById(ResolveQueueOwnerNodeId(parentNodeId));
    YobiAction* caster = queueOwner->action->payload.battleActor->caster1070;
    if (!caster) {
        caster = battleEntries[gLuaSkillRuntime.casterPos];
    }
    BattleSkillMeta* parentMeta = GetBattleSkillMeta(parentAction);
    for (StagedChild& staged : gStagedChildren) {
        const PendingChild& pending = gPendingBatch.children[staged.pendingIndex];
        uint32_t nodeId = AddRuntimeNode(staged.action, parentNodeId, pending.waitMode, true, false);
        staged.nodeId = nodeId;
        RuntimeNode* childNode = FindNodeById(nodeId);
        childNode->activationPending = true;
        if (BattleSkillMeta* childMeta = GetBattleSkillMeta(staged.action)) {
            *childMeta = parentMeta ? *parentMeta : BattleSkillMeta{};
            childMeta->techId = gLuaSkillRuntime.rawTechId;
            childMeta->skillId = gLuaSkillRuntime.rawSkillId;
            childMeta->runtimeGeneration = static_cast<int>(gLuaSkillRuntime.generation);
            childMeta->runtimeNodeId = static_cast<int>(nodeId);
            childMeta->flags |= kLuaSkillMetaFlag;
        }
        AppendPendingCasts(nodeId, caster, staged.action, pending.castBegin, pending.castCount);
        AppendPendingEffects(staged.action, pending.effectBegin, pending.effectCount);
        MoveBattleCastState_439380(staged.action);
    }
    gStagedChildren.clear();
}

static void CommitStagedVisuals(uint32_t parentNodeId) {
    RuntimeNode* parent = FindNodeById(parentNodeId);
    YobiAction* parentAction = parent->action;
    BattleSkillMeta* parentMeta = GetBattleSkillMeta(parentAction);
    uint32_t queueOwnerNodeId = ResolveQueueOwnerNodeId(parentNodeId);
    for (StagedVisual& staged : gStagedVisuals) {
        const PendingVisual& pending = gPendingBatch.visuals[staged.pendingIndex];
        uint32_t nodeId = AddRuntimeNode(staged.action, parentNodeId, pending.waitMode, true,
                                         false, true, queueOwnerNodeId);
        staged.nodeId = nodeId;
        RuntimeNode* visualNode = FindNodeById(nodeId);
        visualNode->activationPending = true;
        visualNode->visualUpdateRef = pending.updateRef;
        visualNode->visualSourcePos = pending.sourcePos;
        visualNode->visualTargetPos = pending.targetPos;
        visualNode->visualStartX = staged.startX;
        visualNode->visualStartY = staged.startY;
        if (BattleSkillMeta* visualMeta = GetBattleSkillMeta(staged.action)) {
            *visualMeta = parentMeta ? *parentMeta : BattleSkillMeta{};
            visualMeta->techId = gLuaSkillRuntime.rawTechId;
            visualMeta->skillId = gLuaSkillRuntime.rawSkillId;
            visualMeta->runtimeGeneration = static_cast<int>(gLuaSkillRuntime.generation);
            visualMeta->runtimeNodeId = static_cast<int>(nodeId);
            visualMeta->flags |= kLuaSkillMetaFlag;
        }
    }
    gStagedVisuals.clear();
}

static bool CommitCallbackBatch(uint32_t nodeId) {
    if (!ValidateCallbackBatch(nodeId)) {
        m_log_iw(L"[LuaSkill] callback batch validation failed: skillId=%d techId=%d nodeId=%u\n",
              gLuaSkillRuntime.rawSkillId, gLuaSkillRuntime.rawTechId, nodeId);
        ResetPendingBatch(true);
        return false;
    }
    if (!StageChildren()) {
        m_log_iw(L"[LuaSkill] child staging failed: skillId=%d techId=%d nodeId=%u\n",
              gLuaSkillRuntime.rawSkillId, gLuaSkillRuntime.rawTechId, nodeId);
        ResetPendingBatch(true);
        return false;
    }
    if (!StageVisuals()) {
        m_log_iw(L"[LuaSkill] visual staging failed: skillId=%d techId=%d nodeId=%u\n",
              gLuaSkillRuntime.rawSkillId, gLuaSkillRuntime.rawTechId, nodeId);
        RollbackStagedChildren();
        ResetPendingBatch(true);
        return false;
    }
    if (!StageDebuffOperations()) {
        m_log_iw(L"[LuaSkill] debuff staging failed: skillId=%d techId=%d nodeId=%u\n",
              gLuaSkillRuntime.rawSkillId, gLuaSkillRuntime.rawTechId, nodeId);
        RollbackStagedChildren();
        RollbackStagedVisuals();
        ResetPendingBatch(true);
        return false;
    }
    if (nodeId != 0) {
        uint32_t queueOwnerNodeId = ResolveQueueOwnerNodeId(nodeId);
        RuntimeNode* queueOwnerNode = FindNodeById(queueOwnerNodeId);
        YobiAction* queueOwnerAction = queueOwnerNode->action;
        YobiAction* caster = queueOwnerAction->payload.battleActor->caster1070;
        if (!caster) {
            caster = battleEntries[gLuaSkillRuntime.casterPos];
        }
        AppendPendingCasts(queueOwnerNodeId, caster, queueOwnerAction,
                           gPendingBatch.appendCastBegin,
                           gPendingBatch.appendCastCount);
        AppendPendingEffects(queueOwnerAction, gPendingBatch.appendEffectBegin,
                              gPendingBatch.appendEffectCount);
        CommitStagedChildren(nodeId);
        CommitStagedVisuals(nodeId);
    }
    CommitOperations(nodeId == 0 ? 0 : ResolveQueueOwnerNodeId(nodeId));
    gStagedDebuffs.clear();
    gPendingBatch.ownedRefs.clear();
    ResetPendingBatch(false);
    return true;
}

struct EffectCallbackDispatch {
    int callbackRef = 0;
    int hitId = 0;
};

static bool DispatchPendingEffect(uint32_t nodeId, int castOrdinal) {
    RuntimeNode* node = FindNodeById(nodeId);
    if (!node || node->actionDead || !node->action || !node->action->payload.battleActor) {
        return false;
    }
    int effectRunning = node->action->payload.battleActor->effectRunningC9C;
    if (effectRunning == node->observedEffectRunning) {
        return true;
    }
    int oldEffectRunning = node->observedEffectRunning;
    node->observedEffectRunning = effectRunning;
    std::vector<EffectCallbackDispatch> dispatches;
    if (effectRunning > oldEffectRunning) {
        for (const RuntimeCastBinding& hitBinding : gLuaSkillRuntime.castBindings) {
            if (hitBinding.nodeId == nodeId && hitBinding.hitId > 0 &&
                hitBinding.hitEffectIndex >= oldEffectRunning &&
                hitBinding.hitEffectIndex < effectRunning &&
                hitBinding.callbacks.onEffect != LUA_NOREF &&
                hitBinding.callbacks.onEffect != LUA_REFNIL) {
                dispatches.push_back({hitBinding.callbacks.onEffect, hitBinding.hitId});
            }
        }
    }
    RuntimeCastBinding* binding = FindCastBinding(nodeId, castOrdinal);
    if (binding && binding->hitId == 0 && binding->callbacks.onEffect != LUA_NOREF &&
        binding->callbacks.onEffect != LUA_REFNIL) {
        dispatches.push_back({binding->callbacks.onEffect, 0});
    }
    for (const EffectCallbackDispatch& dispatch : dispatches) {
        if (!RunLuaCallback(dispatch.callbackRef, nodeId, false, "onEffect",
                            dispatch.hitId) ||
            !CommitCallbackBatch(nodeId)) {
            return false;
        }
    }
    return true;
}

static bool DispatchPendingEnter(uint32_t nodeId) {
    RuntimeNode* node = FindNodeById(nodeId);
    if (!node || node->actionDead || !node->action || !node->action->payload.battleActor) {
        return false;
    }
    ObserveNodeTransition(*node);
    if (node->enteredTransitionGeneration == node->transitionGeneration) {
        return true;
    }
    node->enteredTransitionGeneration = node->transitionGeneration;
    int castOrdinal = node->observedCastOrdinal;
    RuntimeCastBinding* binding = FindCastBinding(nodeId, castOrdinal);
    int callbackRef = binding ? binding->callbacks.onEnter : LUA_NOREF;
    int hitId = binding ? binding->hitId : 0;
    return RunLuaCallback(callbackRef, nodeId, false, "onEnter", hitId) &&
           CommitCallbackBatch(nodeId);
}

static void MarkNodeLogicalDeath(uint32_t nodeId) {
    RuntimeNode* node = FindNodeById(nodeId);
    if (!node || node->actionDead) {
        return;
    }
    node->actionDead = true;
    node->deathMarked = true;
    node->actionComplete = true;
    node->nativeFinishPending = false;
    node->nativeCleanup = false;
    node->visualFinishPending = false;
    LuaUnref(node->visualUpdateRef);
    ClearPendingAdvance(*node);
}

static void FinishRuntimeNode(uint32_t nodeId) {
    RuntimeNode* node = FindNodeById(nodeId);
    if (!node || node->actionDead || !node->action) {
        return;
    }
    if (!node->temporary) {
        ClearPendingAdvance(*node);
        if (node->nativeCleanup) {
            return;
        }
        if (node->actionComplete || node->nativeFinishPending) {
            return;
        }
        node->action->phase = kEndActionState;
        node->nativeFinishPending = true;
        return;
    }
    YobiAction* action = node->action;
    MarkNodeLogicalDeath(nodeId);
    MarkActionDeathOnce(action);
}

static void AbortRuntime([[maybe_unused]] uint32_t nodeId, [[maybe_unused]] const char* stage) {
    if (!gLuaSkillRuntime.active || gLuaSkillRuntime.aborting) {
        return;
    }
    m_log_iw(L"[LuaSkill] abort: skillId=%d techId=%d nodeId=%u stage=%hs\n",
          gLuaSkillRuntime.rawSkillId, gLuaSkillRuntime.rawTechId, nodeId,
          stage ? stage : "unknown");
    gLuaSkillRuntime.aborting = true;
    ResetPendingBatch(true);
    RollbackStagedChildren();
    RollbackStagedVisuals();
    RollbackStagedDebuffs();
    ClearRuntimeCallbackRefs();
    for (uint32_t rootId : gLuaSkillRuntime.rootNodeIds) {
        FinishRuntimeNode(rootId);
    }
    for (size_t i = 0; i < gLuaSkillRuntime.nodes.size(); ++i) {
        if (gLuaSkillRuntime.nodes[i].temporary && !gLuaSkillRuntime.nodes[i].actionDead) {
            FinishRuntimeNode(gLuaSkillRuntime.nodes[i].id);
        }
    }
}

static void ApplyAdvance(uint32_t nodeId, bool hasNextActionState, int nextActionState,
                         bool wasVirtual) {
    RuntimeNode* node = FindNodeById(nodeId);
    if (!node || node->actionDead || !node->action || !node->action->payload.battleActor) {
        return;
    }
    if (hasNextActionState && nextActionState < 0) {
        FinishRuntimeNode(nodeId);
        return;
    }
    YobiAction* action = node->action;
    BattleActorPayload& data = *action->payload.battleActor;
    if (wasVirtual) {
        if (data.castPlayIndexFC4 <= data.castListIndexFC8) {
            action->phase = data.castListCA4[data.castPlayIndexFC4];
            if (hasNextActionState) {
                action->phase = nextActionState;
            }
            if (action->phase == kEndActionState) {
                FinishRuntimeNode(nodeId);
            } else {
                RecordNodeTransition(*node);
            }
        } else if (hasNextActionState) {
            action->phase = nextActionState;
            if (action->phase == kEndActionState) {
                FinishRuntimeNode(nodeId);
            } else {
                RecordNodeTransition(*node);
            }
        } else {
            FinishRuntimeNode(nodeId);
        }
        return;
    }
    if (data.castPlayIndexFC4 > data.castListIndexFC8) {
        if (hasNextActionState) {
            action->phase = nextActionState;
            RecordNodeTransition(*node);
        } else {
            FinishRuntimeNode(nodeId);
        }
        return;
    }
    AdvanceNativeBattleCastState(action);
    if (hasNextActionState) {
        action->phase = nextActionState;
    }
    if (action->phase == kEndActionState) {
        FinishRuntimeNode(nodeId);
        return;
    }
    RecordNodeTransition(*node);
}

static void AdvanceSkillState(YobiAction* action) {
    RuntimeNode* found = FindNodeByAction(action);
    if (!found) {
        AdvanceNativeBattleCastState(action);
        return;
    }
    uint32_t nodeId = found->id;
    RuntimeNode* node = FindNodeById(nodeId);
    if (node->nativeCleanup) {
        AdvanceNativeBattleCastState(action);
        return;
    }
    if (gLuaSkillRuntime.aborting) {
        FinishRuntimeNode(nodeId);
        return;
    }
    if (node->nativeFinishPending || node->actionComplete) {
        return;
    }
    if (node->pendingAdvance) {
        if (IsNodeBlocked(nodeId)) {
            return;
        }
        bool hasNextActionState = node->pendingHasNextActionState;
        int nextActionState = node->pendingNextActionState;
        bool wasVirtual = node->pendingWasVirtual;
        node->pendingAdvance = false;
        node->pendingHasNextActionState = false;
        node->pendingNextActionState = 0;
        node->pendingWasVirtual = false;
        ApplyAdvance(nodeId, hasNextActionState, nextActionState, wasVirtual);
        return;
    }
    BattleActorPayload& data = *action->payload.battleActor;
    bool wasVirtual = data.castPlayIndexFC4 > data.castListIndexFC8;
    int castOrdinal = action->payload.battleActor->castPlayIndexFC4;
    if (!DispatchPendingEffect(nodeId, castOrdinal)) {
        AbortRuntime(nodeId, "onEffect");
        return;
    }
    RuntimeCastBinding* binding = FindCastBinding(nodeId, castOrdinal);
    int callbackRef = binding ? binding->callbacks.onAdvance : LUA_NOREF;
    int hitId = binding ? binding->hitId : 0;
    if (!RunLuaCallback(callbackRef, nodeId, true, "onAdvance", hitId)) {
        AbortRuntime(nodeId, "onAdvance");
        return;
    }
    bool hasNextActionState = gPendingBatch.hasNextActionState;
    int nextActionState = gPendingBatch.nextActionState;
    if (!CommitCallbackBatch(nodeId)) {
        AbortRuntime(nodeId, "onAdvance commit");
        return;
    }
    node = FindNodeById(nodeId);
    if (IsNodeBlocked(nodeId)) {
        node->pendingAdvance = true;
        node->pendingHasNextActionState = hasNextActionState;
        node->pendingNextActionState = nextActionState;
        node->pendingWasVirtual = wasVirtual;
        return;
    }
    ApplyAdvance(nodeId, hasNextActionState, nextActionState, wasVirtual);
}

static void LuaVisualUpdate(YobiAction* action) {
    RuntimeNode* found = FindNodeByAction(action);
    if (!found || !found->visual) {
        return;
    }
    uint32_t nodeId = found->id;
    RuntimeNode* node = FindNodeById(nodeId);
    if (node->activationPending) {
        node->activationPending = false;
        return;
    }
    if (gLuaSkillRuntime.aborting) {
        FinishRuntimeNode(nodeId);
        return;
    }
    if (node->visualFinishPending) {
        PlayFrame_517DA0(action, 0);
        if (!SyncVisualPosition(action)) {
            AbortRuntime(nodeId, "visual finish position");
            return;
        }
        if (!IsNodeBlocked(nodeId)) {
            FinishRuntimeNode(nodeId);
        }
        return;
    }
    if (!RunVisualUpdateCallback(nodeId)) {
        AbortRuntime(nodeId, "visual update");
        return;
    }
    PendingVisualFrame frame = gPendingBatch.visualFrame;
    bool hasNextActionState = gPendingBatch.hasNextActionState;
    int nextActionState = gPendingBatch.nextActionState;
    if (!CommitCallbackBatch(nodeId)) {
        AbortRuntime(nodeId, "visual update commit");
        return;
    }
    node = FindNodeById(nodeId);
    if (!node || node->actionDead || !node->action) {
        return;
    }
    action = node->action;
    if (frame.hasImageNo) {
        action->imageNo = frame.imageNo;
    }
    if (frame.hasCharAction) {
        action->characterAction = frame.charAction;
    }
    if (frame.hasDir) {
        action->direction = frame.dir;
    }
    if (frame.hasScaleX) {
        action->scaleX = frame.scaleX;
    }
    if (frame.hasScaleY) {
        action->scaleY = frame.scaleY;
    }
    if (frame.hasX) {
        action->offset40 = frame.x;
    }
    if (frame.hasY) {
        action->offset44 = frame.y;
    }
    bool finishRequested = frame.markDeath;
    if (hasNextActionState) {
        if (nextActionState < 0) {
            finishRequested = true;
        } else {
            action->phase = nextActionState;
        }
    }
    PlayFrame_517DA0(action, 0);
    if (!SyncVisualPosition(action)) {
        AbortRuntime(nodeId, "visual position");
        return;
    }
    if (node->visualTick == std::numeric_limits<uint32_t>::max()) {
        AbortRuntime(nodeId, "visual tick overflow");
        return;
    }
    ++node->visualTick;
    if (!finishRequested) {
        return;
    }
    node->visualFinishPending = true;
    if (!IsNodeBlocked(nodeId)) {
        FinishRuntimeNode(nodeId);
    }
}

static void UpdateSkillEffect(YobiAction* action) {
    RuntimeNode* found = FindNodeByAction(action);
    if (!found) {
        UpdateNativeBattleActor(action);
        return;
    }
    uint32_t nodeId = found->id;
    RuntimeNode* node = FindNodeById(nodeId);
    if (node->activationPending) {
        node->activationPending = false;
        return;
    }
    if (node->nativeCleanup) {
        UpdateNativeBattleActor(action);
        node = FindNodeById(nodeId);
        if (node && !node->actionDead && node->action && node->action->payload.battleActor &&
            node->action->phase == 0 &&
            node->action->payload.battleActor->busyFD8 == 0) {
            node->nativeCleanup = false;
            node->nativeFinishPending = false;
            node->actionComplete = true;
            ClearPendingAdvance(*node);
        }
        return;
    }
    if (!node->temporary && node->actionComplete) {
        if (action->phase == 0 || action->phase == kEndActionState) {
            return;
        }
        ClearPendingAdvance(*node);
        node->actionComplete = false;
        node->observedActionState = std::numeric_limits<int>::min();
        node->observedCastOrdinal = std::numeric_limits<int>::min();
    }
    if (gLuaSkillRuntime.aborting) {
        FinishRuntimeNode(nodeId);
        node = FindNodeById(nodeId);
        if (!node || node->temporary) {
            return;
        }
    }
    if (!node->temporary &&
        (node->nativeFinishPending || action->phase == kEndActionState)) {
        action->phase = kEndActionState;
        UpdateNativeBattleActor(action);
        node = FindNodeById(nodeId);
        if (node && !node->actionDead) {
            node->nativeFinishPending = false;
            node->actionComplete = true;
        }
        return;
    }
    if (action->phase == kEndActionState) {
        FinishRuntimeNode(nodeId);
        return;
    }
    if (node->pendingAdvance) {
        if (IsNodeBlocked(nodeId)) {
            return;
        }
        bool hasNextActionState = node->pendingHasNextActionState;
        int nextActionState = node->pendingNextActionState;
        bool wasVirtual = node->pendingWasVirtual;
        node->pendingAdvance = false;
        node->pendingHasNextActionState = false;
        node->pendingNextActionState = 0;
        node->pendingWasVirtual = false;
        ApplyAdvance(nodeId, hasNextActionState, nextActionState, wasVirtual);
        node = FindNodeById(nodeId);
        if (!node || node->actionDead || !node->action ||
            node->action->phase == kEndActionState) {
            FinishRuntimeNode(nodeId);
            return;
        }
    }
    if (!DispatchPendingEnter(nodeId)) {
        AbortRuntime(nodeId, "onEnter");
        return;
    }
    node = FindNodeById(nodeId);
    if (!node || node->actionDead || !node->action) {
        return;
    }
    int oldCastOrdinal = action->payload.battleActor->castPlayIndexFC4;
    UpdateNativeBattleActor(action);

    node = FindNodeById(nodeId);
    if (!node || node->actionDead || !node->action) {
        return;
    }
    if (!DispatchPendingEffect(nodeId, oldCastOrdinal)) {
        AbortRuntime(nodeId, "onEffect");
        return;
    }
    if (gLuaSkillRuntime.aborting || action->phase == kEndActionState) {
        FinishRuntimeNode(nodeId);
        return;
    }
    if (!DispatchPendingEnter(nodeId)) {
        AbortRuntime(nodeId, "onEnter after EffectUpdate");
        return;
    }
}

static void CleanupRuntimeForScene(YobiAction* scene) {
    if (!gLuaSkillRuntime.active || gLuaSkillRuntime.scene != scene) {
        return;
    }
    if (!gLuaSkillRuntime.aborting) {
        AbortRuntime(0, "scene teardown");
    }
    ResetPendingBatch(true);
    RollbackStagedChildren();
    RollbackStagedVisuals();
    RollbackStagedDebuffs();
    ResetRuntime();
}

static bool HandleSkillDeath(YobiAction* action) {
    CleanupRuntimeForScene(action);
    if (IsActionDeathMarked(action)) return true;
    RuntimeNode* node = FindAnyNodeByAction(action);
    if (!node) return false;
    if (node->deathMarked) return true;
    const uint32_t nodeId = node->id;
    if (!node->temporary) {
        FinishRuntimeNode(nodeId);
        return true;
    }
    MarkNodeLogicalDeath(nodeId);
    MarkActionDeathOnce(action);
    return true;
}

static bool HasRuntimeChild(uint32_t nodeId) {
    for (const RuntimeNode& node : gLuaSkillRuntime.nodes) {
        if (node.parentId == nodeId) {
            return true;
        }
    }
    return false;
}

static void ClearNodeCastBindings(uint32_t nodeId) {
    for (size_t i = 0; i < gLuaSkillRuntime.castBindings.size();) {
        RuntimeCastBinding& binding = gLuaSkillRuntime.castBindings[i];
        if (binding.nodeId != nodeId) {
            ++i;
            continue;
        }
        ClearCallbackRefs(binding.callbacks);
        gLuaSkillRuntime.castBindings.erase(gLuaSkillRuntime.castBindings.begin() + i);
    }
}

static void RetireDeadRuntimeNodes() {
    bool retired = false;
    do {
        retired = false;
        for (size_t i = 0; i < gLuaSkillRuntime.nodes.size(); ++i) {
            const RuntimeNode& node = gLuaSkillRuntime.nodes[i];
            if (!node.temporary || !node.actionDead || node.action || HasRuntimeChild(node.id)) {
                continue;
            }
            LuaUnref(gLuaSkillRuntime.nodes[i].visualUpdateRef);
            ClearNodeCastBindings(node.id);
            gLuaSkillRuntime.nodes.erase(gLuaSkillRuntime.nodes.begin() + i);
            retired = true;
            break;
        }
    } while (retired);
}

static void FreeSkillAction(YobiAction* action, YobiCallback releaseStorage) {
    CleanupRuntimeForScene(action);
    RuntimeNode* node = FindAnyNodeByAction(action);
    if (node) {
        MarkNodeLogicalDeath(node->id);
        node->action = nullptr;
    }
    ForgetDeathMarkedAction(action);
    releaseStorage(action);
    RetireDeadRuntimeNodes();
}

static void StartAbortOnlyRuntime(YobiAction* scene, int skillId, int techId, int skillLevel,
                                  int skillLevel2, int casterPos) {
    ResetRuntime();
    ++gRuntimeGeneration;
    if (gRuntimeGeneration == 0) {
        ++gRuntimeGeneration;
    }
    gLuaSkillRuntime.active = true;
    gLuaSkillRuntime.aborting = true;
    gLuaSkillRuntime.generation = gRuntimeGeneration;
    gLuaSkillRuntime.rawSkillId = skillId;
    gLuaSkillRuntime.rawTechId = techId;
    gLuaSkillRuntime.skillLevel = skillLevel;
    gLuaSkillRuntime.skillLevel2 = skillLevel2;
    gLuaSkillRuntime.casterPos = casterPos;
    gLuaSkillRuntime.packetStart = gPacketView.startOffset;
    gLuaSkillRuntime.packetEnd = gPacketView.endOffset;
    gLuaSkillRuntime.scene = scene;
}

static bool UpdateRuntimePacketState(YobiAction* scene) {
    if (!gLuaSkillRuntime.active || gLuaSkillRuntime.scene != scene) {
        return false;
    }
    if (!AllRootNodesComplete() || !AllTemporaryNodesDead()) {
        return false;
    }
    if (!CheckBattleTurnReady_4394A0(scene, 0)) {
        return false;
    }
    if (!gLuaSkillRuntime.aborting && !gLuaSkillRuntime.completionCalled) {
        gLuaSkillRuntime.completionCalled = true;
        int callbackRef = gLuaSkillRuntime.onCompleteRef;
        if (!RunLuaCallback(callbackRef, 0, false, "onComplete") ||
            !CommitCallbackBatch(0)) {
            AbortRuntime(0, "onComplete");
            return false;
        }
    }
    if (!AllRootNodesComplete() || !AllTemporaryNodesDead()) {
        return false;
    }
    YobiAction* activeScene = gLuaSkillRuntime.scene;
    ResetRuntime();
    FinishBattleEvent_43B6C0(activeScene);
    return true;
}

static LuaSkillPacketResult HandleLuaSkillPacket(YobiAction* scene) {
    if (!scene || !scene->payload.battleScene || !gSkillEffectL) {
        return LuaSkillPacketResult::Native;
    }
    BattleScenePayload& sceneData = *scene->payload.battleScene;
    if (sceneData.packetSubState58 == 1 && gLuaSkillRuntime.active &&
        gLuaSkillRuntime.scene == scene) {
        return UpdateRuntimePacketState(scene) ? LuaSkillPacketResult::Finished
                                               : LuaSkillPacketResult::Handled;
    }
    if (sceneData.packetSubState58 != 0 || gLuaSkillRuntime.active) {
        return LuaSkillPacketResult::Native;
    }

    int techId = 0;
    int skillId = 0;
    int startOffset = sceneData.packetOffset64;
    if (!ReadSkillKey(startOffset, techId, skillId)) {
        return LuaSkillPacketResult::Native;
    }
    SkillRegistration* registration = FindRegistration(skillId, techId);
    if (!registration) {
        return LuaSkillPacketResult::Native;
    }
    if (!BuildPacketView(startOffset, registration->intCount)) {
        m_log_iw(L"[LuaSkill] terminate invalid SKL packet: skillId=%d techId=%d intCount=%d\n",
              skillId, techId, registration->intCount);
        ResetPendingBatch(true);
        TerminateInvalidSkillPacket(scene);
        return LuaSkillPacketResult::Finished;
    }
    int casterPos = 0;
    int verifiedSkillId = 0;
    int verifiedTechId = 0;
    int skillLevel = 0;
    int skillLevel2 = 0;
    if (!ReadPacketInt(1, verifiedTechId) || !ReadPacketInt(2, casterPos) ||
        !ReadPacketInt(3, verifiedSkillId) || !ReadPacketInt(4, skillLevel) ||
        !ReadPacketInt(5, skillLevel2) ||
        verifiedTechId != techId || verifiedSkillId != skillId || casterPos < 0 ||
        casterPos >= kBattleEntryCount || !battleEntries || !battleEntries[casterPos] ||
        !battleEntries[casterPos]->payload.battleActor) {
        m_log_iw(L"[LuaSkill] invalid SKL common fields: skillId=%d techId=%d\n", skillId,
              techId);
        gPacketView.active = false;
        ResetPendingBatch(true);
        sceneData.packetOffset64 = gPacketView.endOffset;
        FinishBattleEvent_43B6C0(scene);
        return LuaSkillPacketResult::Finished;
    }

    int normalizedSkillLevel = 0;
    int normalizedSkillLevel2 = 0;
    NormalizeSkillLevels(techId, skillLevel, skillLevel2, normalizedSkillLevel,
                         normalizedSkillLevel2);
    SetCustomSkillGlobals(casterPos, skillId, techId, normalizedSkillLevel,
                          normalizedSkillLevel2);
    sceneData.casterPosition6C = casterPos;
    sceneData.skill7C = skillId;
    sceneData.technique80 = techId;
    CaptureBattleSkillMeta(techId, skillId);
    bool parsed =
        RunParseCallback(*registration, skillId, techId, skillLevel, skillLevel2, casterPos);
    if (!parsed) {
        StartAbortOnlyRuntime(scene, skillId, techId, skillLevel, skillLevel2, casterPos);
    } else if (!CommitRootPlan(scene, skillId, techId, skillLevel, skillLevel2,
                               normalizedSkillLevel, normalizedSkillLevel2, casterPos)) {
        if (!gLuaSkillRuntime.active) {
            ResetPendingBatch(true);
            StartAbortOnlyRuntime(scene, skillId, techId, skillLevel, skillLevel2, casterPos);
        } else {
            AbortRuntime(0, "parse commit");
        }
    }
    sceneData.packetOffset64 = gPacketView.endOffset;
    sceneData.packetSubState58 = 1;
    return LuaSkillPacketResult::Handled;
}

static bool InitializeLuaSkillState() {
    if (gSkillEffectL) {
        return true;
    }
    gSkillEffectL = luaL_newstate();
    if (!gSkillEffectL) {
        m_log_iw(L"[LuaSkill] luaL_newstate failed\n");
        return false;
    }
    luaopen_base(gSkillEffectL);
    lua_settop(gSkillEffectL, 0);
    luaopen_bit(gSkillEffectL);
    lua_settop(gSkillEffectL, 0);
    luaopen_table(gSkillEffectL);
    lua_settop(gSkillEffectL, 0);
    luaopen_string(gSkillEffectL);
    lua_settop(gSkillEffectL, 0);
    luaopen_math(gSkillEffectL);
    lua_settop(gSkillEffectL, 0);
    lua_pushnil(gSkillEffectL);
    lua_setglobal(gSkillEffectL, "coroutine");

    luaL_newmetatable(gSkillEffectL, kPacketReaderMetaTable);
    lua_pushvalue(gSkillEffectL, -1);
    lua_setfield(gSkillEffectL, -2, "__index");
    lua_pushcfunction(gSkillEffectL, script::SafeLuaCall<LuaPacketReaderSize>);
    lua_setfield(gSkillEffectL, -2, "size");
    lua_pushcfunction(gSkillEffectL, script::SafeLuaCall<LuaPacketReaderGetInt>);
    lua_setfield(gSkillEffectL, -2, "getInt");
    lua_pop(gSkillEffectL, 1);

    lua_newtable(gSkillEffectL);
    lua_pushcfunction(gSkillEffectL, script::SafeLuaCall<LuaSkillRegister>);
    lua_setfield(gSkillEffectL, -2, "register");
    lua_setglobal(gSkillEffectL, "Skill");

    if (luaL_loadfile(gSkillEffectL, kLuaSkillScript) != 0 ||
        lua_pcall(gSkillEffectL, 0, 0, 0) != 0) {
        [[maybe_unused]] const char* error = lua_tostring(gSkillEffectL, -1);
        m_log_iw(L"[LuaSkill] load failed: file=%hs error=%hs\n", kLuaSkillScript,
              error ? error : "unknown");
        gSkillRegistrations.clear();
        lua_close(gSkillEffectL);
        gSkillEffectL = nullptr;
        return false;
    }
    lua_settop(gSkillEffectL, 0);
    return true;
}

bool LuaSkillEffectBindMoveState(YobiAction* action, BattleSkillMeta& meta) {
    if (!gLuaSkillRuntime.active || !action) {
        return false;
    }
    RuntimeNode* node = FindNodeByAction(action);
    if (!node) {
        return false;
    }
    if (node->actionComplete || node->nativeFinishPending) {
        ClearPendingAdvance(*node);
        node->actionComplete = false;
        node->nativeFinishPending = false;
        node->nativeCleanup = true;
        node->observedActionState = std::numeric_limits<int>::min();
        node->observedCastOrdinal = std::numeric_limits<int>::min();
    }
    meta.techId = gLuaSkillRuntime.rawTechId;
    meta.skillId = gLuaSkillRuntime.rawSkillId;
    meta.runtimeGeneration = static_cast<int>(gLuaSkillRuntime.generation);
    meta.runtimeNodeId = static_cast<int>(node->id);
    meta.flags |= kLuaSkillMetaFlag;
    if (node->pendingBind) {
        node->pendingBind = false;
    }
    return true;
}

void InitializeLuaSkillEffects()
{
    static_cast<void>(InitializeLuaSkillState());
}

void ShutdownLuaSkillEffects()
{
    if (gLuaSkillRuntime.active) CleanupRuntimeForScene(gLuaSkillRuntime.scene);
    ResetPendingBatch(true);
    RollbackStagedChildren();
    RollbackStagedVisuals();
    RollbackStagedDebuffs();
    ResetRuntime();
    for (auto& registration : gSkillRegistrations) LuaUnref(registration.parseRef);
    gSkillRegistrations.clear();
    gPacketView.active = false;
    gPacketView.offsets.clear();
    gDeathMarkedActions.clear();
    if (gSkillEffectL) {
        lua_close(gSkillEffectL);
        gSkillEffectL = nullptr;
    }
}

LuaSkillPacketResult ProcessLuaSkillPacket(YobiAction* scene) { return HandleLuaSkillPacket(scene); }
void UpdateLuaSkillEffect(YobiAction* action) { UpdateSkillEffect(action); }
void AdvanceLuaSkillEffect(YobiAction* action) { AdvanceSkillState(action); }
bool HandleLuaSkillEffectDeath(YobiAction* action) { return HandleSkillDeath(action); }
void FreeLuaSkillEffectAction(YobiAction* action, YobiCallback releaseStorage)
{
    FreeSkillAction(action, releaseStorage);
}

} // namespace cg::yobi::detail
