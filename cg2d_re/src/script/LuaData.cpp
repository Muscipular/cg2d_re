#include "LuaInternal.hpp"
#include "../Yobi/battle_base.hpp"
#include "cg/world/SceneState.hpp"
#include "../world/BattleState.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"

namespace cg::script {

// L01：固定业务槽引用，每次取字段重读owner；不保留已释放战斗对象地址。
enum class DataKind { Player, Item, Pet, Skill, Technique, PetTechnique, Battle };
struct DataRef { DataKind kind; int index; int subIndex; };
static const char* const dataNames[] = {"PlayerDataVal", "InvItemData", "PetData", "SkillData", "TechData", "PetTechData", "BattleEntry"};
static void PushData(lua_State* L, DataKind kind, int index = 0, int subIndex = 0)
{
    auto* ref = static_cast<DataRef*>(lua_newuserdata(L, sizeof(DataRef)));
    *ref = {kind, index, subIndex};
    luaL_setmetatable(L, dataNames[static_cast<int>(kind)]);
}
void PushPlayer(lua_State* L) { PushData(L, DataKind::Player); }
void PushBattleEntry(lua_State* L, int slot)
{
    if (slot < 0 || slot >= 20 || !cg::world::gBattleEntries59E980[slot]) { lua_pushnil(L); return; }
    PushData(L, DataKind::Battle, slot);
}
static int ReadItem(lua_State* L, int index, std::string_view key)
{
    const auto& p = cg::world::gPlayerRuntimeSlots.items[index];
    if (key == "use") { Push(L, (p.used != 0)); return 1; }
    if (key == "name") { Push(L, p.name); return 1; }
    if (key == "memo") { Push(L, p.memo); return 1; }
    if (key == "desc") { Push(L, p.description); return 1; }
    if (key == "descLine") { Push(L, p.pages); return 1; }
    if (key == "lv") { Push(L, p.level); return 1; }
    if (key == "imageNo") { Push(L, p.imageNo); return 1; }
    if (key == "itemId") { Push(L, p.itemId); return 1; }
    if (key == "count") { Push(L, p.count); return 1; }
    if (key == "target") { Push(L, p.target); return 1; }
    if (key == "color") { Push(L, p.color); return 1; }
    if (key == "itemType") { Push(L, p.itemType); return 1; }
    if (key == "flag") { Push(L, p.flags); return 1; }
    if (key == "battleUseFlag") { Push(L, p.battleUseFlag); return 1; }
    if (key == "canUseInField") { Push(L, p.canUseInField); return 1; }
    if (key == "hasDesc") { Push(L, (p.hasDescription != 0)); return 1; }
    lua_pushnil(L); return 1;
}
static int ReadPlayer(lua_State* L, std::string_view key, bool allItems)
{
    auto& p = cg::world::gPlayerRuntimeSlots;
    if (key == "tribeType") { Push(L, p.value00); return 1; }
    if (key == "lv") { Push(L, p.level); return 1; }
    if (key == "rebirth") { Push(L, p.value08); return 1; }
    if (key == "feverTime") { Push(L, p.timerRemaining0C); return 1; }
    if (key == "enableFever") { Push(L, p.timerMode10); return 1; }
    if (key == "feverLastTick") { Push(L, p.timerTick14); return 1; }
    if (key == "hp") { Push(L, p.hp); return 1; }
    if (key == "maxHp") { Push(L, p.maxHp); return 1; }
    if (key == "fp") { Push(L, p.fp); return 1; }
    if (key == "maxFp") { Push(L, p.maxFp); return 1; }
    if (key == "exp") { Push(L, p.value58); return 1; }
    if (key == "nextExp") { Push(L, p.value68); return 1; }
    if (key == "modAttack") { Push(L, p.valuesB0[0]); return 1; }
    if (key == "modDefence") { Push(L, p.valuesB0[1]); return 1; }
    if (key == "fame") { Push(L, p.valueB8); return 1; }
    if (key == "attrs") { Push(L, p.valuesE4); return 1; }
    if (key == "injury") { Push(L, p.valueF4); return 1; }
    if (key == "penalty") { Push(L, p.valueF8); return 1; }
    if (key == "gold") { Push(L, p.valueFC); return 1; }
    if (key == "title") { Push(L, p.titleIndex100); return 1; }
    if (key == "duelPoint") { Push(L, p.value104); return 1; }
    if (key == "name") { Push(L, p.name); return 1; }
    if (key == "ownTitle") { Push(L, p.ownTitle); return 1; }
    if (key == "customTitle") { Push(L, p.text12A); return 1; }
    if (key == "familyTitle") { Push(L, p.text14B); return 1; }
    if (key == "totalSkillSlot") { Push(L, p.value16C); return 1; }
    if (key == "walkSpeed") { Push(L, p.walkSpeed); return 1; }
    if (key == "rideStepCount") { Push(L, p.value174); return 1; }
    if (key == "imageNo") { Push(L, p.imageNo); return 1; }
    if (key == "faceImage") { Push(L, p.value17C); return 1; }
    if (key == "ojbIndex") { Push(L, p.objectIndex); return 1; }
    if (key == "dir") { Push(L, p.direction); return 1; }
    if (key == "mapActionFlag") { Push(L, p.flags18A); return 1; }
    if (key == "partyMode") { Push(L, (p.flags18C & 255u)); return 1; }
    if (key == "charActionState") { Push(L, ((p.flags18C >> 16) & 255u)); return 1; }
    if (key == "actionEffectNo") { Push(L, p.value190); return 1; }
    if (key == "switchFlags") { Push(L, p.options194); return 1; }
    if (key == "battlePosition") { Push(L, p.value1A0); return 1; }
    if (key == "familyTitle2") { Push(L, p.text15BB8); return 1; }
    if (key == "crystal") { Push(L, p.value15BCC); return 1; }
    if (key == "goldLimit") { Push(L, p.values15BD0[0]); return 1; }
    if (key == "poolGoldLimit") { Push(L, p.values15BD0[1]); return 1; }
    if (key == "poolGoldLimit2") { Push(L, p.values15BD0[2]); return 1; }
    if (key == "isInjury") { Push(L, ((p.flags18C & 0x10000u) != 0)); return 1; }
    if (key == "position") { Push(L, cg::world::gBattleCurrentPosition5ABCA4); return 1; }
    if (key == "surprisal") { Push(L, (cg::world::gBattlePlayerFlags5ABCAC & 3u)); return 1; }
    if (key == "mapX") { Push(L, cg::world::gPlayerX); return 1; }
    if (key == "mapY") { Push(L, cg::world::gPlayerY); return 1; }
    if (key == "mapId") { Push(L, cg::world::gValue9A86DC); return 1; }
    if (key == "floor") { Push(L, static_cast<int>(world::gMapFloor)); return 1; }
    if (key == "vit") { Push(L, p.values78[0]); return 1; }
    if (key == "str") { Push(L, p.values78[1]); return 1; }
    if (key == "tough") { Push(L, p.values78[2]); return 1; }
    if (key == "quick") { Push(L, p.values78[3]); return 1; }
    if (key == "magic") { Push(L, p.values78[4]); return 1; }
    if (key == "stamina") { Push(L, p.values78[5]); return 1; }
    if (key == "dex") { Push(L, p.values78[6]); return 1; }
    if (key == "intelligence") { Push(L, p.values78[7]); return 1; }
    if (key == "atk") { Push(L, p.values78[8]); return 1; }
    if (key == "def") { Push(L, p.values78[9]); return 1; }
    if (key == "agi") { Push(L, p.values78[10]); return 1; }
    if (key == "magicPower") { Push(L, p.values78[11]); return 1; }
    if (key == "recovery") { Push(L, p.values78[12]); return 1; }
    if (key == "charm") { Push(L, p.values78[13]); return 1; }
    if (key == "posion") { Push(L, p.valuesBC[0]); return 1; }
    if (key == "sleep") { Push(L, p.valuesBC[1]); return 1; }
    if (key == "stone") { Push(L, p.valuesBC[2]); return 1; }
    if (key == "drunk") { Push(L, p.valuesBC[3]); return 1; }
    if (key == "confusion") { Push(L, p.valuesBC[4]); return 1; }
    if (key == "amnesia") { Push(L, p.valuesBC[5]); return 1; }
    if (key == "critical") { Push(L, p.valuesD4[0]); return 1; }
    if (key == "counter") { Push(L, p.valuesD4[1]); return 1; }
    if (key == "hitrate") { Push(L, p.valuesD4[2]); return 1; }
    if (key == "avoid") { Push(L, p.valuesD4[3]); return 1; }
    if (key == "disableAct1") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 4u) != 0)); return 1; }
    if (key == "disableAct2") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 16u) != 0)); return 1; }
    if (key == "disablePetAct1") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 8u) != 0)); return 1; }
    if (key == "disablePetAct2") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 131072u) != 0)); return 1; }
    if (key == "hasPet") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 32u) != 0)); return 1; }
    if (key == "weaponDirect") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 128u) != 0)); return 1; }
    if (key == "weaponBow") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 256u) != 0)); return 1; }
    if (key == "weaponBoomerang") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 512u) != 0)); return 1; }
    if (key == "weaponKnife") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 1024u) != 0)); return 1; }
    if (key == "petRide") { Push(L, ((cg::world::gBattlePlayerFlags5ABCAC & 65536u) != 0)); return 1; }
    if (key == "skillList") {
        lua_createtable(L, 15, 0);
        for (int i = 0; i < 15; ++i) {
            const auto& pair = cg::world::gSkillPairsF63370[i];
            if (pair.used) PushData(L, DataKind::Skill, pair.index); else lua_pushnil(L);
            lua_rawseti(L, -2, i + 1);
        }
        return 1;
    }
    if (key == "petList") {
        lua_createtable(L, 5, 0);
        for (int i = 0; i < 5; ++i) {
            if (cg::world::gPetRuntimeRecords[i].used) PushData(L, DataKind::Pet, i); else lua_pushnil(L);
            lua_rawseti(L, -2, i + 1);
        }
        return 1;
    }
    if (key == "itemList" || key == "items" || key == "Items") {
        lua_createtable(L, 28, 0);
        for (int i = 0; i < 28; ++i) {
            if (allItems) {
                // UI接口返回含空槽的值快照，战斗接口返回实时userdata。
                lua_newtable(L);
                for (const char* field : {"use","name","memo","desc","descLine","color","imageNo","lv","itemId","count","itemType","canUseInField","battleUseFlag","target","flag","hasDesc"}) {
                    ReadItem(L, i, field); lua_setfield(L, -2, field);
                }
            } else if (p.items[i].used) PushData(L, DataKind::Item, i); else lua_pushnil(L);
            lua_rawseti(L, -2, i + 1);
        }
        return 1;
    }
    lua_pushnil(L); return 1;
}
static int ReadPet(lua_State* L, int index, std::string_view key)
{
    auto& p = cg::world::gPetRuntimeRecords[index];
    if (key == "use") { Push(L, (p.used != 0)); return 1; }
    if (key == "lv") { Push(L, p.value08); return 1; }
    if (key == "name") { Push(L, p.name6AC); return 1; }
    if (key == "imageNo") { Push(L, p.imageNo6A0); return 1; }
    if (key == "userPetName") { Push(L, p.name6BD); return 1; }
    if (key == "injury") { Push(L, p.values70[25]); return 1; }
    if (key == "exp") { Push(L, p.values0C[4]); return 1; }
    if (key == "nextExp") { Push(L, p.values0C[5]); return 1; }
    if (key == "petBattleState") { Push(L, (p.flags6AA & 15)); return 1; }
    if (key == "hp") { Push(L, p.values0C[0]); return 1; }
    if (key == "maxHp") { Push(L, p.values0C[1]); return 1; }
    if (key == "fp") { Push(L, p.values0C[2]); return 1; }
    if (key == "maxFp") { Push(L, p.values0C[3]); return 1; }
    if (key == "loyalty") { Push(L, p.values70[10]); return 1; }
    if (key == "seqNo") { Push(L, p.value6D0); return 1; }
    if (key == "tribe") { Push(L, p.value04); return 1; }
    if (key == "techList") {
        lua_createtable(L, 10, 0);
        for (int i = 0; i < 10; ++i) {
            const auto& pair = p.techniquePairs650[i];
            if (pair.used) PushData(L, DataKind::PetTechnique, index, pair.index); else lua_pushnil(L);
            lua_rawseti(L, -2, i + 1);
        }
        return 1;
    }
    lua_pushnil(L); return 1;
}
static int ReadSkill(lua_State* L, int index, std::string_view key)
{
    const auto& p = cg::world::gSkillsF1BAFC[index];
    if (key == "lv") { Push(L, p.level); return 1; }
    if (key == "name") { Push(L, p.name); return 1; }
    if (key == "exp") { Push(L, p.experience); return 1; }
    if (key == "nextExp") { Push(L, p.nextExperience); return 1; }
    if (key == "seqNo") { Push(L, p.sequence); return 1; }
    if (key == "maxLv") { Push(L, p.maxLevel); return 1; }
    if (key == "skillId") { Push(L, p.skillId); return 1; }
    if (key == "fpReduce") { Push(L, p.fpReduce); return 1; }
    if (key == "reqSlot") { Push(L, static_cast<std::int8_t>(p.requiredSlots19[0])); return 1; }
    if (key == "index") { Push(L, index); return 1; }
    if (key == "canUse") {
        bool used = false;
        for (const auto& pair : cg::world::gSkillPairsF63370) if (pair.used && pair.index == index) used = true;
        Push(L, used && (cg::world::gBattleSkillFlags5ABC04 & (1u << index)) != 0); return 1;
    }
    if (key == "techList") {
        lua_createtable(L, 15, 0);
        for (int i = 0; i < 15; ++i) {
            if (p.techniques3C[i].used88) PushData(L, DataKind::Technique, index, i); else lua_pushnil(L);
            lua_rawseti(L, -2, i + 1);
        }
        return 1;
    }
    lua_pushnil(L); return 1;
}
static int ReadTechnique(lua_State* L, int index, int sub, std::string_view key)
{
    const auto& p = cg::world::gSkillsF1BAFC[index].techniques3C[sub];
    if (key == "reqLv") { Push(L, p.value90); return 1; }
    if (key == "field") { Push(L, p.value80); return 1; }
    if (key == "name") { Push(L, p.name); return 1; }
    if (key == "use") { Push(L, (p.used88 != 0)); return 1; }
    if (key == "target") { Push(L, (p.value84 & 0xFFFF)); return 1; }
    if (key == "desc") { Push(L, p.description); return 1; }
    if (key == "canUse") { Push(L, (p.value8C != 0)); return 1; }
    if (key == "fp") { Push(L, p.value7C); return 1; }
    lua_pushnil(L); return 1;
}
static int ReadPetTechnique(lua_State* L, int index, int sub, std::string_view key)
{
    const auto& pet = cg::world::gPetRuntimeRecords[index];
    const auto& p = pet.techniquesD8[sub];
    if (key == "use") { Push(L, (p.used != 0)); return 1; }
    if (key == "name") { Push(L, p.name); return 1; }
    if (key == "desc") { Push(L, p.description); return 1; }
    if (key == "fp") { Push(L, p.fp); return 1; }
    if (key == "techId") { Push(L, p.techniqueId); return 1; }
    if (key == "seqNo") { Push(L, p.sequence); return 1; }
    if (key == "field") { Push(L, p.field); return 1; }
    if (key == "target") { Push(L, p.target); return 1; }
    if (key == "canUse") { Push(L, (pet.used && (cg::world::gBattlePetSkillFlags5ABB78 & (1u << sub)) != 0)); return 1; }
    if (key == "index") { Push(L, (pet.used ? sub : -1)); return 1; }
    lua_pushnil(L); return 1;
}
static int ReadBattle(lua_State* L, int index, std::string_view key)
{
    auto* p = cg::world::gBattleEntries59E980[index];
    if (!p) { lua_pushnil(L); return 1; }
    if (key == "battlePos") { Push(L, p->value1D4); return 1; }
    if (key == "lv") { Push(L, p->field1D0); return 1; }
    if (key == "imageNo") { Push(L, p->imageNo); return 1; }
    if (key == "name") { Push(L, p->nameC4); return 1; }
    if (key == "hp") { Push(L, p->readiness); return 1; }
    if (key == "fp") { Push(L, p->barValue1B0); return 1; }
    if (key == "maxHp") { Push(L, p->maxHp); return 1; }
    if (key == "maxFp") { Push(L, p->barMaximum1C0); return 1; }
    const auto* payload = p->payload.battleActor;
    if (!payload) { lua_pushnil(L); return 1; }
    if (key == "isPlayer") { Push(L, ((payload->flagsFDC & 4u) != 0)); return 1; }
    if (key == "isPet") { Push(L, ((payload->flagsFDC & 8u) != 0)); return 1; }
    if (key == "isDeath") { Push(L, ((payload->flagsFDC & 2u) != 0)); return 1; }
    if (key == "isTwoAction") { Push(L, ((payload->flagsFDC & 1024u) != 0)); return 1; }
    if (key == "isPoison") { Push(L, ((payload->flagsFDC & 16u) != 0)); return 1; }
    if (key == "isSleep") { Push(L, ((payload->flagsFDC & 32u) != 0)); return 1; }
    if (key == "isStone") { Push(L, ((payload->flagsFDC & 64u) != 0)); return 1; }
    if (key == "isDrug") { Push(L, ((payload->flagsFDC & 128u) != 0)); return 1; }
    if (key == "isConfusion") { Push(L, ((payload->flagsFDC & 256u) != 0)); return 1; }
    if (key == "isForget") { Push(L, ((payload->flagsFDC & 512u) != 0)); return 1; }
    lua_pushnil(L); return 1;
}
static int DataIndex(lua_State* L)
{
    const auto kind = static_cast<DataKind>(lua_tointeger(L, lua_upvalueindex(1)));
    const auto* ref = static_cast<DataRef*>(luaL_checkudata(L, 1, dataNames[static_cast<int>(kind)]));
    const char* name = lua_tostring(L, 2);
    if (!name) { lua_pushnil(L); return 1; }
    switch (kind) {
    case DataKind::Player: return ReadPlayer(L, name, false);
    case DataKind::Item: return ReadItem(L, ref->index, name);
    case DataKind::Pet: return ReadPet(L, ref->index, name);
    case DataKind::Skill: return ReadSkill(L, ref->index, name);
    case DataKind::Technique: return ReadTechnique(L, ref->index, ref->subIndex, name);
    case DataKind::PetTechnique: return ReadPetTechnique(L, ref->index, ref->subIndex, name);
    case DataKind::Battle: return ReadBattle(L, ref->index, name);
    }
    lua_pushnil(L); return 1;
}
static int PlayerIndex(lua_State* L)
{
    const char* key = lua_tostring(L, 2);
    if (!key) { lua_pushnil(L); return 1; }
    return ReadPlayer(L, key, true);
}
static int DataToString(lua_State* L)
{
    const int kind = static_cast<int>(lua_tointeger(L, lua_upvalueindex(1)));
    luaL_checkudata(L, 1, dataNames[kind]);
    lua_pushfstring(L, "%s_%p", dataNames[kind], lua_touserdata(L, 1));
    return 1;
}
void RegisterLuaData(lua_State* L)
{
    for (int i = 0; i < 7; ++i) {
        luaL_newmetatable(L, dataNames[i]);
        lua_pushinteger(L, i); lua_pushcclosure(L, SafeLuaCall<DataIndex>, 1); lua_setfield(L, -2, "__index");
        lua_pushinteger(L, i); lua_pushcclosure(L, DataToString, 1); lua_setfield(L, -2, "__tostring");
        lua_pop(L, 1);
    }
    lua_newtable(L); lua_newtable(L);
    lua_pushcfunction(L, SafeLuaCall<PlayerIndex>); lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2); lua_setglobal(L, "Player");
}
} // namespace cg::script
