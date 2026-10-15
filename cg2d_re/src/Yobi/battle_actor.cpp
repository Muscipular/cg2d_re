#include "battle_actor.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/world/SceneState.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::login {

namespace detail {

// 原[423800,4238EA)：固定角色记录，名称及四项属性均短路匹配。
std::int32_t MatchBattleAlbum_423800(const YobiAction* const action)
{
    const std::byte* record = world::detail::GetSelectedAlbumRecords();
    for (std::int32_t index = 0; index < 960; ++index, record += 0x80) {
        if (record[0] == std::byte{0}) {
            continue;
        }
        if (std::strcmp(reinterpret_cast<const char*>(record + 0x34), action->nameC4.data()) != 0) {
            continue;
        }
        std::size_t field = 0;
        for (; field < action->fields288.size(); ++field) {
            std::uint32_t value;
            std::memcpy(&value, record + 0x20 + field * 4, sizeof(value));
            if ((value != 0) != (action->fields288[field] != 0)) {
                break;
            }
        }
        if (field == action->fields288.size()) {
            return 1;
        }
    }
    return 0;
}

// 原[4238F0,42395B)：11个直接caller均不消费原混合返回值。
void UpdateBattleAlbumFlag_4238F0(YobiAction* const action)
{
    using namespace world::detail;
    auto* const payload = action->payload.battleActor;
    const std::uint32_t state = payload->stateFE0;
    const std::int32_t position = payload->battlePosition18;
    const std::uint32_t cleared = state & ~std::uint32_t{1};
    payload->stateFE0 = cleared;
    const std::int32_t playerPosition = gBattleCurrentPosition5ABCA4;
    if (position == playerPosition) {
        payload->stateFE0 = cleared | 1u;
        return;
    }
    if (position != GetBattlePetPosition_405B40(playerPosition)) {
        if (gBattleCurrentPosition5ABCA4 < 10) {
            if (position < 10) {
                return;
            }
        } else if (position >= 10) {
            return;
        }
        if (MatchBattleAlbum_423800(action) == 0) {
            return;
        }
    }
    payload->stateFE0 = payload->stateFE0 | 1u;
}

// 原函数：0x00438290。
YobiAction* NewBattleActor_438290(const std::int32_t position)
{
    using namespace world::detail;
    const auto priority = static_cast<std::uint8_t>(static_cast<std::uint8_t>(position) * 10 + 2);
    YobiAction* const action = NewYobiAction_404AE0(priority, sizeof(BattleActorPayload), YobiPayloadKind::BattleActor);
    if (action == nullptr) {
        return nullptr;
    }
    const std::uint32_t flags = action->flags;
    action->callback = UpdateBattleActor_404DB0;
    const std::uint32_t configuredFlags = flags | 0x60054u;
    action->imageNo = 100500;
    action->flags = configuredFlags;
    action->characterAction = 0;
    action->framePlayingRate = 100;
    action->value54 = 3.0F;
    action->direction = 1;
    action->displayPriority = 9;
    const std::int32_t firstPointIndex = gBattlePointIndices5536FC[position];
    auto* const payload = action->payload.battleActor;
    action->offset40 = static_cast<float>(gBattleGraphicPoints553758[firstPointIndex][0]);
    action->offset44 = static_cast<float>(gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][1]);
    payload->x00 = gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][2];
    payload->y04 = gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][3];
    payload->x08 = gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][0];
    const std::int32_t y = gBattleGraphicPoints553758[gBattlePointIndices5536FC[position]][1];
    payload->battlePosition18 = position;
    payload->y0C = y;
    action->phase = 0;
    action->field270 = 0;
    PlayFrame_517DA0(action, 0);
    const std::int32_t screenX = static_cast<std::int32_t>(action->offset40);
    const float screenY = action->offset44;
    action->x = screenX;
    action->y = static_cast<std::int32_t>(screenY);
    return action;
}

// 原函数：0x00404DB0。
void UpdateBattleActor_404DB0(YobiAction* const action)
{
    const std::int32_t scene = world::gCurrentSceneState;
    const bool isSceneSeven = scene == 7;
    auto* const payload = action->payload.battleActor;
    if (isSceneSeven) {
        return;
    }
    switch (action->phase) {
    case 0:
        static_cast<void>(UpdateBattleAction0_406650(action));
        break;
    case 1:
        static_cast<void>(UpdateBattleAction1_406720(action));
        break;
    case 2:
        static_cast<void>(UpdateBattleAction2_406760(action));
        break;
    case 3:
        static_cast<void>(UpdateBattleAction3_406C30(action));
        break;
    case 4:
        static_cast<void>(UpdateBattleAction4_406D70(action));
        break;
    case 5:
        static_cast<void>(UpdateBattleAction5_40CFA0(action));
        break;
    case 6:
        static_cast<void>(UpdateBattleAction6_406CD0(action));
        break;
    case 7:
        static_cast<void>(UpdateBattleAction7_409410(action));
        break;
    case 8:
        static_cast<void>(UpdateBattleAction8_4094A0(action));
        break;
    case 9:
        static_cast<void>(UpdateBattleAction9_409510(action));
        break;
    case 10:
        static_cast<void>(UpdateBattleAction10_409610(action));
        break;
    case 11:
        static_cast<void>(UpdateBattleAction11_4067F0(action));
        break;
    case 12:
        static_cast<void>(UpdateBattleAction12_406930(action));
        break;
    case 13:
        static_cast<void>(UpdateBattleAction13_406A80(action));
        break;
    case 14:
        static_cast<void>(UpdateBattleAction14_409680(action));
        break;
    case 15:
        static_cast<void>(UpdateBattleAction15_406E10(action));
        break;
    case 16:
        static_cast<void>(UpdateBattleAction16_4072E0(action));
        break;
    case 17:
        static_cast<void>(UpdateBattleAction17_4093B0(action));
        break;
    case 18:
        static_cast<void>(UpdateBattleAction18_408170(action));
        break;
    case 19:
        static_cast<void>(UpdateBattleAction19_4082F0(action));
        break;
    case 20:
        static_cast<void>(UpdateBattleAction20_408410(action));
        break;
    case 21:
        static_cast<void>(UpdateBattleAction21_408750(action));
        break;
    case 22:
        static_cast<void>(UpdateBattleAction22_4087C0(action));
        break;
    case 23:
        static_cast<void>(UpdateBattleAction23_4090F0(action));
        break;
    case 24:
        static_cast<void>(UpdateBattleAction24_409380(action));
        break;
    case 25:
        static_cast<void>(UpdateBattleAction25_406BA0(action));
        break;
    case 26:
        static_cast<void>(UpdateBattleAction26_40A9E0(action));
        break;
    case 27:
        static_cast<void>(UpdateBattleAction27_40AA70(action));
        break;
    case 28:
        static_cast<void>(UpdateBattleAction28_40ACF0(action));
        break;
    case 29:
        static_cast<void>(UpdateBattleAction29_40AE50(action));
        break;
    case 30:
        static_cast<void>(UpdateBattleAction30_40AF30(action));
        break;
    case 31:
        static_cast<void>(UpdateBattleAction31_40B020(action));
        break;
    case 32:
        static_cast<void>(UpdateBattleAction32_40B410(action));
        break;
    case 33:
        static_cast<void>(UpdateBattleAction33_40B450(action));
        break;
    case 34:
        static_cast<void>(UpdateBattleAction34_40B5E0(action));
        break;
    case 35:
        static_cast<void>(UpdateBattleAction35_40B930(action));
        break;
    case 36:
        static_cast<void>(UpdateBattleAction36_40BAD0(action));
        break;
    case 37:
        static_cast<void>(UpdateBattleAction37_40BB20(action));
        break;
    case 38:
        static_cast<void>(UpdateBattleAction38_40BB70(action));
        break;
    case 39:
        static_cast<void>(UpdateBattleAction39_40BBC0(action));
        break;
    case 40:
        static_cast<void>(UpdateBattleAction40_40BD40(action));
        break;
    case 41:
        static_cast<void>(UpdateBattleAction41_40BD60(action));
        break;
    case 42:
        static_cast<void>(UpdateBattleAction42_40BDB0(action));
        break;
    case 43:
        static_cast<void>(UpdateBattleAction43_40BE30(action));
        break;
    case 44:
        static_cast<void>(UpdateBattleAction44_40BDF0(action));
        break;
    case 45:
        static_cast<void>(UpdateBattleAction45_40B130(action));
        break;
    case 46:
        static_cast<void>(UpdateBattleAction46_40B200(action));
        break;
    case 47:
        static_cast<void>(UpdateBattleAction47_40B3F0(action));
        break;
    case 48:
        static_cast<void>(UpdateBattleAction48_40B490(action));
        break;
    case 49:
        static_cast<void>(UpdateBattleAction49_40B5B0(action));
        break;
    case 50:
        static_cast<void>(UpdateBattleAction50_40B3C0(action));
        break;
    case 51:
        static_cast<void>(UpdateBattleAction51_40BE70(action));
        break;
    case 52:
        static_cast<void>(UpdateBattleAction52_40BE90(action));
        break;
    case 53:
        static_cast<void>(UpdateBattleAction53_40BF20(action));
        break;
    case 54:
        static_cast<void>(UpdateBattleAction54_40BFE0(action));
        break;
    case 55:
        static_cast<void>(UpdateBattleAction55_40C0C0(action));
        break;
    case 56:
        static_cast<void>(UpdateBattleAction56_40C160(action));
        break;
    case 57:
        static_cast<void>(UpdateBattleAction57_40C1A0(action));
        break;
    case 58:
        static_cast<void>(UpdateBattleAction58_40C1E0(action));
        break;
    case 59:
        static_cast<void>(UpdateBattleAction59_40C240(action));
        break;
    case 60:
        static_cast<void>(UpdateBattleAction60_40C220(action));
        break;
    case 61:
        static_cast<void>(UpdateBattleAction61_40C280(action));
        break;
    case 62:
        static_cast<void>(UpdateBattleAction62_40C2B0(action));
        break;
    case 63:
        static_cast<void>(UpdateBattleAction63_40C2E0(action));
        break;
    case 64:
        static_cast<void>(UpdateBattleAction64_40CBB0(action));
        break;
    case 65:
        static_cast<void>(UpdateBattleAction65_40C380(action));
        break;
    case 66:
        static_cast<void>(UpdateBattleAction66_40C480(action));
        break;
    case 67:
        static_cast<void>(UpdateBattleAction67_4106B0(action));
        break;
    case 68:
        static_cast<void>(UpdateBattleAction68_40C0E0(action));
        break;
    case 69:
        static_cast<void>(UpdateBattleAction69_40C120(action));
        break;
    case 70:
        static_cast<void>(UpdateBattleAction70_4070E0(action));
        break;
    case 71:
        static_cast<void>(UpdateBattleAction71_40C4D0(action));
        break;
    case 72:
        static_cast<void>(UpdateBattleAction72_40CAA0(action));
        break;
    case 73:
        static_cast<void>(AdvanceBattleLinkedCast_40CAC0(action));
        break;
    case 74:
        static_cast<void>(AdvanceBattleEffectAndCast_40CAF0(action));
        break;
    case 75:
        static_cast<void>(UpdateBattleAction75_40CB10(action));
        break;
    case 76:
        static_cast<void>(UpdateBattleAction76_40CB30(action));
        break;
    case 77:
        static_cast<void>(UpdateBattleAction77_40CB50(action));
        break;
    case 78:
        static_cast<void>(UpdateBattleAction78_40CB80(action));
        break;
    case 79:
        static_cast<void>(UpdateBattleAction79_40CBD0(action));
        break;
    case 80:
        static_cast<void>(UpdateBattleAction80_40CC80(action));
        break;
    case 81:
        static_cast<void>(UpdateBattleAction81_40CCB0(action));
        break;
    case 82:
        static_cast<void>(UpdateBattleAction82_40CCE0(action));
        break;
    case 83:
        static_cast<void>(UpdateBattleAction83_40CD10(action));
        break;
    case 84:
        static_cast<void>(UpdateBattleAction84_40CD70(action));
        break;
    case 85:
        static_cast<void>(UpdateBattleAction85_40CDE0(action));
        break;
    case 86:
        static_cast<void>(UpdateBattleAction86_40C520(action));
        break;
    case 87:
        static_cast<void>(UpdateBattleAction87_40C780(action));
        break;
    case 88:
        static_cast<void>(UpdateBattleAction88_40C8D0(action));
        break;
    case 89:
        static_cast<void>(UpdateBattleAction89_40C8F0(action));
        break;
    case 90:
        static_cast<void>(UpdateBattleAction90_40C910(action));
        break;
    case 91:
        static_cast<void>(UpdateBattleAction91_40CF30(action));
        break;
    case 92:
        static_cast<void>(UpdateBattleAction92_40CF50(action));
        break;
    case 93:
        static_cast<void>(UpdateBattleAction93_40CF70(action));
        break;
    case 94:
        static_cast<void>(UpdateBattleAction94_40D010(action));
        break;
    case 95:
        static_cast<void>(UpdateBattleAction95_40D0B0(action));
        break;
    case 96:
        static_cast<void>(UpdateBattleAction96_40D170(action));
        break;
    case 97:
        static_cast<void>(UpdateBattleAction97_40D1A0(action));
        break;
    case 98:
        static_cast<void>(UpdateBattleAction98_40D1D0(action));
        break;
    case 99:
        static_cast<void>(UpdateBattleAction99_40D390(action));
        break;
    case 100:
        static_cast<void>(UpdateBattleAction100_40D3C0(action));
        break;
    case 101:
        static_cast<void>(UpdateBattleAction101_40D580(action));
        break;
    case 102:
        static_cast<void>(UpdateBattleAction102_40D6C0(action));
        break;
    case 103:
        static_cast<void>(UpdateBattleAction103_40DBE0(action));
        break;
    case 104:
        static_cast<void>(UpdateBattleAction104_40DC90(action));
        break;
    case 105:
        static_cast<void>(UpdateBattleAction105_40DE40(action));
        break;
    case 106:
        static_cast<void>(UpdateBattleAction106_40DF50(action));
        break;
    case 107:
        static_cast<void>(UpdateBattleAction107_40DF70(action));
        break;
    case 108:
        static_cast<void>(UpdateBattleAction108_40DF80(action));
        break;
    case 109:
        static_cast<void>(UpdateBattleAction109_40E4B0(action));
        break;
    case 110:
        static_cast<void>(UpdateBattleAction110_40E560(action));
        break;
    case 111:
        static_cast<void>(UpdateBattleAction111_40E970(action));
        break;
    case 112:
        static_cast<void>(UpdateBattleAction112_40EB00(action));
        break;
    case 113:
        static_cast<void>(UpdateBattleAction113_40EFE0(action));
        break;
    case 114:
        static_cast<void>(UpdateBattleAction114_40F0A0(action));
        break;
    case 115:
        static_cast<void>(UpdateBattleAction115_40FB80(action));
        break;
    case 116:
    case 117:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
    case 130:
    case 131:
    case 132:
    case 133:
    case 134:
    case 135:
    case 136:
    case 137:
    case 146:
    case 149:
    case 150:
    case 151:
    case 152:
    case 153:
    case 157:
    case 158:
    case 161:
    case 217:
    case 250:
    case 252:
        static_cast<void>(UpdateBattleFactoryAction_40FC10(action));
        break;
    case 127:
    case 128:
    case 129:
        static_cast<void>(UpdateBattleActions127To129_40FF90(action));
        break;
    case 138:
        static_cast<void>(UpdateBattleAction138_40FF40(action));
        break;
    case 139:
        static_cast<void>(UpdateBattleAction139_410000(action));
        break;
    case 140:
        static_cast<void>(UpdateBattleAction140_410730(action));
        break;
    case 141:
        static_cast<void>(UpdateBattleAction141_410E90(action));
        break;
    case 142:
        static_cast<void>(UpdateBattleAction142_410F70(action));
        break;
    case 143:
        static_cast<void>(UpdateBattleAction143_410FC0(action));
        break;
    case 144:
        static_cast<void>(UpdateBattleAction144_411240(action));
        break;
    case 145:
        static_cast<void>(UpdateBattleAction145_411290(action));
        break;
    case 147:
        static_cast<void>(UpdateBattleAction147_411320(action));
        break;
    case 148:
        static_cast<void>(UpdateBattleAction148_40FEF0(action));
        break;
    case 154:
        static_cast<void>(UpdateBattleAction154_40F790(action));
        break;
    case 155:
        static_cast<void>(UpdateBattleAction155_40F800(action));
        break;
    case 156:
        static_cast<void>(UpdateBattleAction156_40FB30(action));
        break;
    case 159:
        static_cast<void>(UpdateBattleAction159_4107C0(action));
        break;
    case 160:
        static_cast<void>(UpdateBattleAction160_410B00(action));
        break;
    case 162:
        static_cast<void>(UpdateBattleAction162_410E30(action));
        break;
    case 163:
        static_cast<void>(UpdateBattleAction163_4113A0(action));
        break;
    case 164:
        static_cast<void>(UpdateBattleAction164_411A50(action));
        break;
    case 165:
        static_cast<void>(UpdateBattleAction165_411D30(action));
        break;
    case 166:
        static_cast<void>(UpdateBattleAction166_411CB0(action));
        break;
    case 167:
        static_cast<void>(UpdateBattleAction167_411FA0(action));
        break;
    case 168:
        static_cast<void>(UpdateBattleAction168_411FC0(action));
        break;
    case 169:
        static_cast<void>(UpdateBattleAction169_411FE0(action));
        break;
    case 170:
        static_cast<void>(UpdateBattleAction170_412000(action));
        break;
    case 171:
        static_cast<void>(UpdateBattleAction171_412020(action));
        break;
    case 172:
    case 173:
        static_cast<void>(UpdateBattleActions172_173_4120F0(action));
        break;
    case 174:
        static_cast<void>(UpdateBattleAction174_4121D0(action));
        break;
    case 175:
        static_cast<void>(UpdateBattleAction175_412210(action));
        break;
    case 176:
        static_cast<void>(UpdateBattleAction176_412260(action));
        break;
    case 177:
        static_cast<void>(UpdateBattleAction177_412290(action));
        break;
    case 178:
        static_cast<void>(UpdateBattleAction178_4122C0(action));
        break;
    case 179:
        static_cast<void>(UpdateBattleAction179_4122E0(action));
        break;
    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
    case 186:
    case 187:
    case 188:
    case 189:
    case 190:
    case 191:
    case 192:
    case 193:
    case 194:
    case 195:
    case 196:
    case 197:
    case 198:
    case 199:
    case 200:
    case 201:
    case 202:
    case 203:
        static_cast<void>(UpdateBattleActions180_203_412310(action));
        break;
    case 204:
        static_cast<void>(UpdateBattleAction204_4124E0(action));
        break;
    case 205:
        static_cast<void>(UpdateBattleAction205_4127B0(action));
        break;
    case 206:
        static_cast<void>(UpdateBattleAction206_4127F0(action));
        break;
    case 207:
        static_cast<void>(UpdateBattleAction207_412830(action));
        break;
    case 208:
        static_cast<void>(SetBattleAction208Target_412870(action));
        break;
    case 209:
        static_cast<void>(UpdateBattleAction209_412900(action));
        break;
    case 210:
        static_cast<void>(UpdateBattleAction210_412F70(action));
        break;
    case 211:
        static_cast<void>(ApplyBattleSceneEffects_413070(action));
        break;
    case 212:
        static_cast<void>(UpdateBattleAction212_413110(action));
        break;
    case 213:
        static_cast<void>(UpdateBattleAction213_4131B0(action));
        break;
    case 214:
        static_cast<void>(UpdateBattleAction214_4132B0(action));
        break;
    case 215:
        static_cast<void>(UpdateBattleAction215_4133D0(action));
        break;
    case 216:
        static_cast<void>(UpdateBattleAction216_413400(action));
        break;
    case 218:
        static_cast<void>(UpdateBattleAction218_413550(action));
        break;
    case 219:
        static_cast<void>(UpdateBattleAction219_413750(action));
        break;
    case 220:
        static_cast<void>(UpdateBattleAction220_413980(action));
        break;
    case 221:
        static_cast<void>(UpdateBattleAction221_413B40(action));
        break;
    case 222:
        static_cast<void>(UpdateBattleAction222_413E90(action));
        break;
    case 223:
        static_cast<void>(UpdateBattleAction223_413FE0(action));
        break;
    case 224:
        static_cast<void>(UpdateBattleAction224_414050(action));
        break;
    case 225:
        static_cast<void>(UpdateBattleAction225_4150F0(action));
        break;
    case 226:
        static_cast<void>(UpdateBattleAction226_4154C0(action));
        break;
    case 227:
        static_cast<void>(UpdateBattleAction227_415940(action));
        break;
    case 228:
        static_cast<void>(UpdateBattleAction228_416000(action));
        break;
    case 229:
        static_cast<void>(UpdateBattleAction229_416020(action));
        break;
    case 230:
        static_cast<void>(UpdateBattleAction230_4160A0(action));
        break;
    case 231:
        static_cast<void>(UpdateBattleAction231_416150(action));
        break;
    case 232:
        static_cast<void>(UpdateBattleAction232_416220(action));
        break;
    case 233:
        static_cast<void>(UpdateBattleAction233_416860(action));
        break;
    case 234:
        static_cast<void>(UpdateBattleAction234_416AB0(action));
        break;
    case 235:
        static_cast<void>(ApplyBattleCastEffects_416DB0(action));
        break;
    case 236:
        static_cast<void>(UpdateBattleAction236_406630(action));
        break;
    case 237:
        static_cast<void>(UpdateBattleAction237_416DD0(action));
        break;
    case 238:
    case 239:
        static_cast<void>(UpdateBattleActions238_239_417410(action));
        break;
    case 240:
        static_cast<void>(UpdateBattleAction240_417AC0(action));
        break;
    case 241:
        static_cast<void>(UpdateBattleAction241_417B80(action));
        break;
    case 242:
        static_cast<void>(UpdateBattleAction242_417CC0(action));
        break;
    case 243:
        static_cast<void>(UpdateBattleAction243_417D80(action));
        break;
    case 244:
        static_cast<void>(UpdateBattleAction244_418550(action));
        break;
    case 245:
        static_cast<void>(UpdateBattleAction245_418580(action));
        break;
    case 246:
        static_cast<void>(UpdateBattleAction246_4185C0(action));
        break;
    case 247:
        static_cast<void>(UpdateBattleAction247_4185F0(action));
        break;
    case 248:
        static_cast<void>(UpdateBattleAction248_418620(action));
        break;
    case 249:
        static_cast<void>(UpdateBattleAction249_4071C0(action));
        break;
    case 251:
        static_cast<void>(UpdateBattleAction251_40B180(action));
        break;
    case 253:
        static_cast<void>(UpdateBattleAction253_40FE50(action));
        break;
    case 254:
        static_cast<void>(UpdateBattleAction254_40FEB0(action));
        break;
    case 255:
        static_cast<void>(UpdateBattleAction255_4186B0(action));
        break;
    case 256:
        static_cast<void>(UpdateBattleAction256_418B50(action));
        break;
    default:
        break;
    }
    // 公共尾沿用入口载荷；动作可能改写对象表，不在此重取载荷。
    if (payload->flash1004 == 1) {
        const std::int32_t count = payload->flashCounter1000 + 1;
        payload->flashCounter1000 = count;
        if (count >= 2) {
            payload->flashCounter1000 = 0;
        }
        if (payload->flashCounter1000 == 1) {
            static_cast<void>(NewBattleAfterimage_41E5F0(action));
        }
    }
    if ((action->flags & 0x800000u) == 0) {
        const std::int32_t x = static_cast<std::int32_t>(action->offset40);
        const std::int32_t offsetX = static_cast<std::int32_t>(gYobiScreenOffsetX);
        const float y = action->offset44;
        action->x = x + offsetX;
        const std::int32_t yCoordinate = static_cast<std::int32_t>(y);
        const std::int32_t offsetY = static_cast<std::int32_t>(gYobiScreenOffsetY);
        action->y = yCoordinate + offsetY;
    }
    YobiAction* const rider = payload->rider13C8;
    if (rider != nullptr) {
        SyncBattleRider_421FB0(rider);
    }
    YobiAction* const gear = payload->gear13B4;
    if (gear != nullptr) {
        SyncBattleGear_4225D0(gear);
    }
    const bool hasRider = payload->rider13C8 != nullptr;
    YobiAction* const firstAuxiliaryGear = payload->auxiliaryGear13B8[0];
    if (hasRider) {
        if (firstAuxiliaryGear != nullptr) {
            static_cast<void>(MarkDeath(firstAuxiliaryGear));
            payload->auxiliaryGear13B8[0] = nullptr;
        }
        YobiAction* const secondAuxiliaryGear = payload->auxiliaryGear13B8[1];
        if (secondAuxiliaryGear != nullptr) {
            static_cast<void>(MarkDeath(secondAuxiliaryGear));
            payload->auxiliaryGear13B8[1] = nullptr;
        }
    } else {
        if (firstAuxiliaryGear != nullptr) {
            SyncBattleAuxGear_422780(firstAuxiliaryGear, 0);
        }
        YobiAction* const secondAuxiliaryGear = payload->auxiliaryGear13B8[1];
        if (secondAuxiliaryGear != nullptr) {
            SyncBattleAuxGear_422780(secondAuxiliaryGear, 1);
        }
    }
}

// BIDLE01：完整FDC持样/CL消费、归零后fresh BYTE与上一动作门禁见doc/cg2d_battle_idle_dispatch_spec.md。
// 原函数：0x00406650。
void UpdateBattleAction0_406650(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t index = payload->castPlayIndexFC4;
    if (index <= payload->castListIndexFC8 && payload->castListCA4[index] == 166) {
        AdvanceBattleCastState_439340(action);
    }
    action->framePlayingRate = 100;
    const std::uint32_t flags = payload->flagsFDC;
    const std::uint8_t cachedFlags = static_cast<std::uint8_t>(flags);
    if ((cachedFlags & 2u) != 0) {
        action->characterAction = 10;
        static_cast<void>(PlayFrame_517DA0(action, 1));
        return;
    }
    const std::int32_t skill = payload->castSkill103C;
    if (skill == 72 || skill == 113) {
        const std::int32_t previous = payload->castPlayIndexFC4 - 1;
        if (previous >= 0) {
            const std::int32_t previousAction = payload->castListCA4[previous];
            if (previousAction == 57 || previousAction == 59) {
                return;
            }
        }
    }
    if (action->characterAction != 0) {
        action->characterAction = 0;
        const std::uint8_t currentFlags = static_cast<std::uint8_t>(payload->flagsFDC);
        if ((currentFlags & 0x60u) == 0) {
            static_cast<void>(PlayFrame_517DA0(action, 0));
        }
    } else if ((static_cast<std::uint8_t>(action->flags) & 0x40u) != 0
        && (cachedFlags & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
}

// BIDLE01：保存动作/固定payload后归站立，再读FDC低BYTE门禁，见待机SPEC。
// 原函数：0x00406720。
void UpdateBattleAction1_406720(YobiAction* const action)
{
    const std::int32_t characterAction = action->characterAction;
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    action->framePlayingRate = 100;
    if (characterAction != 0) {
        action->characterAction = 0;
    }
    const std::uint8_t flags = static_cast<std::uint8_t>(payload->flagsFDC);
    if ((flags & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
}

// BRCP01：回00/04，先X发布后Y取样/phase0；FDC置1后才播放，不推进队列。
// 原函数：0x00406760。
std::int32_t UpdateBattleAction2_406760(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 1;
    action->value54 = 4.0F;
    action->framePlayingRate = 100;
    const float turnY = static_cast<float>(payload->y04);
    const float turnX = static_cast<float>(payload->x00);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, 1, 0.0F);
    MoveBattleByAngle_405EA0(action);
    const float distanceY = static_cast<float>(payload->y04);
    const float distanceX = static_cast<float>(payload->x00);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->offset40 = static_cast<float>(payload->x00);
        const double y = static_cast<double>(payload->y04);
        action->phase = 0;
        action->offset44 = static_cast<float>(y);
        payload->flagsFDC |= 1u;
    }
    return PlayFrame_517DA0(action, 0);
}

// BRCP01：转向后重取C9C/position/目标；距离<70推进，否则移动，再播放。
// 原函数：0x00406C30。
std::int32_t UpdateBattleAction3_406C30(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 1;
    action->value54 = 8.0F;
    action->framePlayingRate = 100;
    TurnBattleToward_406190(action,
        gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
    if (GetBattleDistance_406300(action,
            gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 70) {
        AdvanceBattleCastState_439340(action);
    } else {
        MoveBattleByAngle_405EA0(action);
    }
    return PlayFrame_517DA0(action, 0);
}

// BRCP01：转向后重取效果目标；距离<8推进，否则移动，再播放。
// 原函数：0x00406D70。
std::int32_t UpdateBattleAction4_406D70(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 3;
    action->value54 = 8.0F;
    action->framePlayingRate = 100;
    TurnBattleToward_406190(action,
        gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
    if (GetBattleDistance_406300(action,
            gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 8) {
        AdvanceBattleCastState_439340(action);
    } else {
        MoveBattleByAngle_405EA0(action);
    }
    return PlayFrame_517DA0(action, 0);
}

// BPAW01：FF0先发布增量再按旧值<16分流，推进后清零，再播放。
// 原函数：0x0040CFA0。
std::int32_t UpdateBattleAction5_40CFA0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 3;
    action->value54 = 8.0F;
    action->framePlayingRate = 100;
    const std::int32_t count = payload->valueFF0;
    payload->valueFF0 = count + 1;
    if (count < 16) {
        MoveBattleByAngle_405EA0(action);
    } else {
        AdvanceBattleCastState_439340(action);
        payload->valueFF0 = 0;
    }
    return PlayFrame_517DA0(action, 0);
}

// BRCP01：两组FE8/FEC先Y后X取样；抵达先X发布再取Y，随后推进/播放。
// 原函数：0x00406CD0。
std::int32_t UpdateBattleAction6_406CD0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 3;
    action->value54 = 8.0F;
    action->framePlayingRate = 100;
    const float turnY = static_cast<float>(payload->moveTargetYFEC);
    const float turnX = static_cast<float>(payload->moveTargetXFE8);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, 1, 0.0F);
    MoveBattleByAngle_405EA0(action);
    const float distanceY = static_cast<float>(payload->moveTargetYFEC);
    const float distanceX = static_cast<float>(payload->moveTargetXFE8);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->offset40 = static_cast<float>(payload->moveTargetXFE8);
        action->offset44 = static_cast<float>(payload->moveTargetYFEC);
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, 0);
}

// BPAW01：持一次position选转向；严格终帧后fresh低BYTE调速，再推进。
// 原函数：0x00409410。
void UpdateBattleAction7_409410(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 2;
    action->framePlayingRate = 100;
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    if (position == -1) {
        const float y = static_cast<float>(payload->y04);
        const float x = static_cast<float>(payload->x00);
        TurnBattleTowardPosition_4060D0(action, x, y, 1, 0.0F);
    } else {
        TurnBattleToward_406190(action, world::detail::gBattleAllEntries59E980[position], 1, 0.0F);
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        if ((static_cast<std::uint8_t>(payload->flagsFDC) & 2u) != 0) {
            action->framePlayingRate = 200;
        }
        AdvanceBattleCastState_439340(action);
    }
}

// BPAW01：FEC/FE8先Y后X转向；严格终帧后fresh低BYTE调速/推进。
// 原函数：0x004094A0。
void UpdateBattleAction8_4094A0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 2;
    action->framePlayingRate = 100;
    const float y = static_cast<float>(payload->moveTargetYFEC);
    const float x = static_cast<float>(payload->moveTargetXFE8);
    TurnBattleTowardPosition_4060D0(action, x, y, 1, 0.0F);
    if (PlayFrame_517DA0(action, 1) == 1) {
        if ((static_cast<std::uint8_t>(payload->flagsFDC) & 2u) != 0) {
            action->framePlayingRate = 200;
        }
        AdvanceBattleCastState_439340(action);
    }
}

// BPAW01：FC4-2为14走回位距离5，否则两次fresh目标距离70；另一支才移动。
// 原函数：0x00409510。
std::int32_t UpdateBattleAction9_409510(YobiAction* const action)
{
    using world::detail::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 3;
    action->value54 = 8.0F;
    action->framePlayingRate = 100;
    const std::int32_t previous = payload->castPlayIndexFC4 - 2;
    if (previous >= 0 && payload->castListCA4[previous] == 14) {
        const float turnY = static_cast<float>(payload->y04);
        const float turnX = static_cast<float>(payload->x00);
        TurnBattleTowardPosition_4060D0(action, turnX, turnY, 1, 0.0F);
        const float distanceY = static_cast<float>(payload->y04);
        const float distanceX = static_cast<float>(payload->x00);
        if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
            AdvanceBattleCastState_439340(action);
            return PlayFrame_517DA0(action, 0);
        }
    } else {
        TurnBattleToward_406190(action,
            gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
        if (GetBattleDistance_406300(action,
                gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 70) {
            AdvanceBattleCastState_439340(action);
            return PlayFrame_517DA0(action, 0);
        }
    }
    MoveBattleByAngle_405EA0(action);
    return PlayFrame_517DA0(action, 0);
}

// BPAW01：严格终帧才取上一项11/低BYTE门禁，按本位重查全局方向叶后推进。
// 原函数：0x00409610。
void UpdateBattleAction10_409610(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 4;
    action->framePlayingRate = 100;
    if (PlayFrame_517DA0(action, 1) == 1) {
        const std::int32_t previous = payload->castPlayIndexFC4 - 1;
        if (previous >= 0 && payload->castListCA4[previous] == 11 && (static_cast<std::uint8_t>(payload->flagsFDC) & 2u) == 0) {
            Sub_406260(world::detail::gBattleAllEntries59E980[payload->battlePosition18]);
        }
        AdvanceBattleCastState_439340(action);
    }
}

// BRCP01：骑乘两低BYTE死亡短路/持W32门禁；推进后phase及末播放动作重取。
// 原函数：0x004067F0。
void UpdateBattleAction11_4067F0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (payload->rider13C8 != nullptr && payload->effect1060 != nullptr) {
        const auto* const carrier = static_cast<const BattleRideCarrierPayload*>(payload->effect1060->payload.battleRideCarrier);
        YobiAction* const first = carrier->first00;
        bool dead = false;
        if (first != nullptr) {
            const auto* const firstPayload = static_cast<const BattleActorPayload*>(first->payload.battleActor);
            dead = firstPayload != nullptr && (static_cast<std::uint8_t>(firstPayload->flagsFDC) & 2u) != 0;
        }
        if (!dead) {
            YobiAction* const second = carrier->second04;
            if (second != nullptr) {
                const auto* const secondPayload = static_cast<const BattleActorPayload*>(second->payload.battleActor);
                dead = secondPayload != nullptr && (static_cast<std::uint8_t>(secondPayload->flagsFDC) & 2u) != 0;
            }
        }
        if (dead) {
            AdvanceBattleCastState_439340(action);
            if (action->phase == 46) {
                AdvanceBattleCastState_439340(action);
            }
            return;
        }
    }
    const std::uint32_t flags = payload->flagsFDC;
    const std::uint8_t entryFlags = static_cast<std::uint8_t>(flags);
    if ((entryFlags & 2u) != 0 && (entryFlags & 0xCu) == 0) {
        if (action->characterAction != 10) {
            action->phase = 46;
            return;
        }
    } else if (action->characterAction != 10) {
        action->characterAction = 1;
    }
    const std::int32_t previous = payload->castPlayIndexFC4 - 1;
    if (previous >= 0 && payload->castListCA4[previous] == 7) {
        action->characterAction = 3;
    }
    const std::int32_t updateDirection = action->characterAction != 10 ? 1 : 0;
    const float turnY = static_cast<float>(payload->y04);
    const float turnX = static_cast<float>(payload->x00);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, updateDirection, 0.0F);
    MoveBattleByAngle_405EA0(action);
    const float distanceY = static_cast<float>(payload->y04);
    const float distanceX = static_cast<float>(payload->x00);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->offset40 = static_cast<float>(payload->x00);
        action->offset44 = static_cast<float>(payload->y04);
        AdvanceBattleCastState_439340(action);
    }
    static_cast<void>(PlayFrame_517DA0(action, action->characterAction == 10 ? 1 : 0));
}

// B12P01：原入口低BYTE、周期特效/计数与特殊直接推进见doc/cg2d_battle_coordinate_action_producer_spec.md。
// 原函数：0x00406930。
void UpdateBattleAction12_406930(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::uint8_t entryFlags = static_cast<std::uint8_t>(payload->flagsFDC);
    if ((entryFlags & 2u) != 0) {
        action->characterAction = 10;
        static_cast<void>(PlayFrame_517DA0(action, 1));
        const float y = static_cast<float>(payload->y0C);
        const float x = static_cast<float>(payload->x08);
        TurnBattleTowardPosition_4060D0(action, x, y, 0, 0.0F);
        action->value54 = 6.0F;
        AdvanceBattleCastState_439340(action);
        return;
    }
    if (payload->valueFF0 == 0) {
        const std::int32_t randomY = platform::RandomRange_46FAF0(-10, 10);
        const float y = static_cast<float>(static_cast<double>(randomY) + action->offset44);
        const std::int32_t randomX = platform::RandomRange_46FAF0(-10, 10);
        const float x = static_cast<float>(static_cast<double>(randomX) + action->offset40);
        static_cast<void>(NewBattleCoordinateEffect_41E720(x, y, 110006));
    }
    const std::int32_t count = payload->valueFF0;
    payload->valueFF0 = count + 1;
    if (count >= 2) {
        payload->valueFF0 = 0;
    }
    action->characterAction = 1;
    static_cast<void>(PlayFrame_517DA0(action, 0));
    const float y = static_cast<float>(payload->y0C);
    const float x = static_cast<float>(payload->x08);
    TurnBattleTowardPosition_4060D0(action, x, y, 1, 0.0F);
    const std::int32_t frames = payload->valueFCC + 1;
    payload->valueFCC = frames;
    action->framePlayingRate = frames > 70 ? 30 : 100 - frames;
    if (payload->valueFCC >= 120) {
        payload->valueFCC = 0;
        payload->valueFF0 = 0;
        action->value54 = 6.0F;
        AdvanceBattleCastState_439340(action);
    }
}

// B12P01：原入口低BYTE、播放后移动及到位X发布再读Y，见坐标动作SPEC。
// 原函数：0x00406A80。
void UpdateBattleAction13_406A80(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    std::int32_t updateDirection;
    const std::uint8_t entryFlags = static_cast<std::uint8_t>(payload->flagsFDC);
    if ((entryFlags & 2u) != 0) {
        action->characterAction = 10;
        static_cast<void>(PlayFrame_517DA0(action, 1));
        updateDirection = 0;
    } else {
        if (payload->valueFF0 == 0) {
            const std::int32_t randomY = platform::RandomRange_46FAF0(-10, 10);
            const float y = static_cast<float>(static_cast<double>(randomY) + action->offset44);
            const std::int32_t randomX = platform::RandomRange_46FAF0(-10, 10);
            const float x = static_cast<float>(static_cast<double>(randomX) + action->offset40);
            static_cast<void>(NewBattleCoordinateEffect_41E720(x, y, 110006));
        }
        const std::int32_t count = payload->valueFF0;
        payload->valueFF0 = count + 1;
        if (count >= 2) {
            payload->valueFF0 = 0;
        }
        action->characterAction = 1;
        static_cast<void>(PlayFrame_517DA0(action, 0));
        updateDirection = 1;
    }
    const float turnY = static_cast<float>(payload->y0C);
    const float turnX = static_cast<float>(payload->x08);
    TurnBattleTowardPosition_4060D0(action, turnX, turnY, updateDirection, 0.0F);
    action->value54 = 6.0F;
    action->framePlayingRate = 20;
    MoveBattleByAngle_405EA0(action);
    const float distanceY = static_cast<float>(payload->y0C);
    const float distanceX = static_cast<float>(payload->x08);
    if (GetBattleDistanceToPosition_4062A0(action, distanceX, distanceY) < 5) {
        action->offset40 = static_cast<float>(payload->x08);
        action->offset44 = static_cast<float>(payload->y0C);
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x00406630。
void UpdateBattleAction236_406630(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->phase = 0;
    payload->busyFD8 = 0;
    UpdateBattleAction0_406650(action);
}

} // namespace detail

} // namespace cg::login
