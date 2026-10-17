#include "battle_cast.hpp"
#include "base.hpp"
#include "battle_base.hpp"
#include "battle_effect.hpp"
#include "battle_display.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/render/RendererResources.hpp"

#include "animation.hpp"
#include "battle_attachment.hpp"
#include "battle_impact.hpp"
#include "battle_status.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "../world/BattleState.hpp"
#include "../world/ChatHistory.hpp"
#include "../world/WorldMapState.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BFC0、0x0053BE40、0x0053BEC0、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <cstring>
#include <optional>
#include <string>

namespace cg::yobi {

namespace detail {

namespace {

// 原57E174的57条三S32记录及全零哨兵；动作85只消费前两列。
constexpr std::array<std::array<std::int32_t, 3>, 58> kBattleNamedEffectImages57E174 {{
    {40779, 104201, 99510},
    {40780, 104202, 99511},
    {40781, 104203, 99512},
    {40782, 104204, 99513},
    {40783, 104205, 99514},
    {40784, 104206, 99515},
    {40785, 104207, 99516},
    {40786, 104208, 99517},
    {40787, 104209, 99518},
    {40788, 104210, 99519},
    {40789, 104211, 99520},
    {40790, 104212, 99521},
    {40791, 104213, 99522},
    {40792, 104214, 99523},
    {40793, 104215, 99524},
    {40794, 104216, 99525},
    {40795, 104217, 99526},
    {40796, 104218, 99527},
    {40797, 104219, 99528},
    {40798, 104220, 99529},
    {40799, 104221, 99530},
    {48111, 111058, 99772},
    {48112, 111059, 99773},
    {48113, 111060, 99774},
    {48114, 111061, 99775},
    {48115, 111062, 99776},
    {48116, 111063, 99777},
    {48117, 111064, 99778},
    {48118, 111065, 99779},
    {48119, 111066, 99780},
    {48120, 111067, 99781},
    {61027, 104201, 99510},
    {61028, 104202, 99511},
    {61029, 104203, 99512},
    {61030, 104204, 99513},
    {61031, 104205, 99514},
    {61032, 104206, 99515},
    {61033, 104207, 99516},
    {61034, 104208, 99517},
    {61035, 104209, 99518},
    {61036, 104210, 99519},
    {61037, 104211, 99520},
    {61038, 104212, 99521},
    {61039, 104213, 99522},
    {61040, 104214, 99523},
    {61041, 104215, 99524},
    {61042, 104216, 99525},
    {61043, 104217, 99526},
    {61044, 104218, 99527},
    {61045, 104219, 99528},
    {61046, 104220, 99529},
    {61047, 104221, 99530},
    {61048, 111058, 99772},
    {61049, 111059, 99773},
    {61050, 111060, 99774},
    {61051, 111061, 99775},
    {61052, 111062, 99776},
    {0, 0, 0},
}};

static_assert(sizeof(kBattleNamedEffectImages57E174) == 696);

// 原54D4C0起物理16B；末零不用于索引回退，逻辑有效项数不据此扩充。
constexpr std::array<std::int32_t, 4> kBattleStatusImages54D4C0 {103018, 103022, 103020, 0};

} // namespace

// BWT01：固定一次目标索引/point，先Y后X，两原22项S32只读表保持。
// 原函数：0x0040BD60。
void UpdateBattleAction41_40BD60(YobiAction* const action)
{
    using world::gBattleGraphicPoints553758;
    using world::gBattlePointIndices5536FC;
    auto* const payload = action->payload.battleActor;
    const auto& point = cg::world::gBattleGraphicPoints553758[cg::world::gBattlePointIndices5536FC[payload->targetPosition100C]];
    const float y = static_cast<float>(point[3]);
    const float x = static_cast<float>(point[2]);
    TurnBattleTowardPosition_4060D0(action, x, y, 1, 0.0F);
    AdvanceBattleCastState_439340(action);
}

// BWT01：固定一次全局目标，先F32 Y后X转向本对象，再推进。
// 原函数：0x0040BDB0。
void UpdateBattleAction42_40BDB0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* const target = world::gBattleAllEntries59E980[payload->targetPosition100C];
    const float y = target->offset44;
    const float x = target->offset40;
    TurnBattleTowardPosition_4060D0(action, x, y, 1, 0.0F);
    AdvanceBattleCastState_439340(action);
}

// BWT01：仅非空other才转向；共同rate100/动作0/PlayFrame(0)返回保持。
// 原函数：0x0040BE30。
std::int32_t UpdateBattleAction43_40BE30(YobiAction* const action)
{
    YobiAction* const other = action->battleOpponent10;
    if (other != nullptr) {
        TurnBattleToward_406190(action, other, 1, 0.0F);
    }
    action->framePlayingRate = 100;
    action->characterAction = 0;
    return PlayFrame_517DA0(action, 0);
}

// BWT01：由目标朝本对象转向，再推进本对象，不交换两T*。
// 原函数：0x0040BDF0。
void UpdateBattleAction44_40BDF0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* const target = world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position];
    TurnBattleToward_406190(target, action, 1, 0.0F);
    AdvanceBattleCastState_439340(action);
}

// BDE01：临时零先读齐三字段，再mask/encoded/BYTE count发布、销毁、推进。
// 原函数：0x0040B130。
void UpdateBattleAction45_40B130(YobiAction* const action)
{
    // 原40B14F/59/5F先mask后encoded，保留目标token及未复制字节。
    // 现由整数赋值统一三字段发布；一次RNG，目标token／reserved保持。
    action->readiness = std::int32_t{0};
    AdvanceBattleCastState_439340(action);
}

// BDE01：完整高位/骑乘BYTE短路/低WORD缩放，三附属清理后保不同退出分支。
// 原函数：0x0040B200。
void UpdateBattleAction46_40B200(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x10000000u) != 0) {
        payload->valueFF0 = action->phase;
        action->phase = 164;
        return;
    }
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
    action->characterAction = 10;
    action->framePlayingRate = 100;
    if (PlayFrame_517DA0(action, 1) != 1) {
        return;
    }
    if ((static_cast<std::uint16_t>(payload->flagsFDC) & 0x1000u) != 0) {
        action->scaleX = 1.0F;
        action->scaleY = 1.0F;
        payload->flagsFDC &= ~0x1000u;
    }
    YobiAction* effect = payload->effect105C;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->effect105C = nullptr;
    }
    effect = payload->effect1054;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->effect1054 = nullptr;
    }
    effect = payload->effect1068;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->effect1068 = nullptr;
    }
    if (payload->value13B0 != 0) {
        action->phase = 219;
        return;
    }
    if ((static_cast<std::uint8_t>(payload->flagsFDC) & 0xCu) != 0) {
        static_cast<void>(NewBattleCenteredEffect_41FA00(action, 110169));
        static_cast<void>(audio::PlaySe_52C450(215, action->x));
        payload->stateFE0 |= 2u;
        AdvanceBattleCastState_439340(action);
        return;
    }
    effect = payload->attachedEffect1058;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->attachedEffect1058 = nullptr;
    }
    payload->stateFE0 |= 2u;
    action->phase = 48;
}

// BDE01：完整W32 FDC置2后推进，不缩窄发布。
// 原函数：0x0040B3F0。
void UpdateBattleAction47_40B3F0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    payload->flagsFDC |= 2u;
    AdvanceBattleCastState_439340(action);
}

// BDE01：两设备路径的旧alpha/调用后fresh alpha及8/4/255门禁保持。
// 原函数：0x0040B490。
void UpdateBattleAction48_40B490(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (render::HasLegacy3DDevice_44B610() == 0) {
        const std::int32_t count = payload->valueFF0 + 1;
        payload->valueFF0 = count;
        if (action->field58 >= 8) {
            const std::uint32_t flags = action->flags;
            const std::int32_t x = action->x;
            action->field58 = 0;
            action->flags = flags | 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(218, x));
            static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 36, 0, 1));
            action->phase = 49;
        } else if (count > 4) {
            payload->valueFF0 = 0;
            const std::uint32_t flags = action->flags ^ 2u;
            const std::int32_t step = action->field58 + 1;
            action->flags = flags;
            action->field58 = step;
        }
        return;
    }
    if (payload->valueFF0 != 255) {
        payload->valueFF0 = 255;
        static_cast<void>(audio::PlaySe_52C450(218, action->x));
    }
    const std::uint8_t previousAlpha = GetYobiActionField30F(action);
    SetYobiActionField308(action, 8);
    if (previousAlpha == 0) {
        action->field58 = 0;
        payload->valueFF0 = 0;
        action->phase = 51;
        return;
    }
    static_cast<void>(NewBattleRisingEffect_420D50(action));
    const std::uint8_t alpha = GetYobiActionField30F(action);
    SetYobiActionField30F(action, alpha < 4 ? 0 : static_cast<std::uint8_t>(alpha - 4));
}

// BDE01：完整1024严格==2才清零/切51。
// 原函数：0x0040B5B0。
void UpdateBattleAction49_40B5B0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (payload->value1024 == 2) {
        payload->value1024 = 0;
        action->phase = 51;
    }
}

// BDE01：动作10后严格终帧才推进，不增rate写入。
// 原函数：0x0040B3C0。
void UpdateBattleAction50_40B3C0(YobiAction* const action)
{
    action->characterAction = 10;
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// BDE01：先清对应全局战斗槽，再标死并返回原T*。
// 原函数：0x0040BE70。
YobiAction* UpdateBattleAction51_40BE70(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    world::gBattleAllEntries59E980[payload->battlePosition18] = nullptr;
    return MarkDeath(action);
}

// BDE01：先BYTE后固定caster，13/47/51目标及启动位置逐次重取。
// 原函数：0x0040BE90。
void UpdateBattleAction52_40BE90(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    using world::GetBattlePetPosition_405B40;
    auto* const payload = action->payload.battleActor;
    const std::uint8_t flags = static_cast<std::uint8_t>(payload->flagsFDC);
    YobiAction* const caster = payload->caster1070;
    if ((flags & 4u) != 0) {
        const std::int32_t petPosition = cg::world::GetBattlePetPosition_405B40(payload->battlePosition18);
        YobiAction* const pet = cg::world::gBattleAllEntries59E980[petPosition];
        if (pet != nullptr) {
            AppendBattleCast_4392A0(caster, pet, 13);
            AppendBattleCast_4392A0(caster, cg::world::gBattleAllEntries59E980[petPosition], 47);
            AppendBattleCast_4392A0(caster, cg::world::gBattleAllEntries59E980[petPosition], 51);
            const std::int32_t currentPetPosition = cg::world::GetBattlePetPosition_405B40(payload->battlePosition18);
            MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[currentPetPosition]);
        }
    }
    AdvanceBattleCastState_439340(action);
}

// BDE01：未舍入缩放与创建后fresh零判定，清理/推进后共同播放返回。
// 原函数：0x0040BF20。
std::int32_t UpdateBattleAction53_40BF20(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    const std::uint32_t flags = action->flags | (hasDevice ? 0x100000u : 0x8000u);
    const double step = 0.02F;
    const double scaleX = action->scaleX - step;
    action->flags = flags;
    action->scaleX = static_cast<float>(scaleX);
    action->scaleY = static_cast<float>(action->scaleY - step);
    if (!(scaleX >= 0.0)) {
        action->scaleX = 0.0F;
        action->scaleY = 0.0F;
    }
    if (payload->value1024 == 0) {
        action->battleCastEffect10 = NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 22, 0, 1);
    }
    const float currentScaleX = action->scaleX;
    if (!(currentScaleX < 0.0F) && !(currentScaleX > 0.0F)) {
        YobiAction* const effect = action->battleCastEffect10;
        if (effect != nullptr) {
            static_cast<void>(MarkDeath(effect));
            action->battleCastEffect10 = nullptr;
        }
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, 1);
}

// BSA01：未舍入放大/type23/创建后精确1，完成时重查设备和fresh flags。
// 原函数：0x0040BFE0。
std::int32_t UpdateBattleAction54_40BFE0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    action->flags |= hasDevice ? 0x100000u : 0x8000u;
    Sub_406260(action);
    const double step = 0.02F;
    const double scaleX = action->scaleX + step;
    action->scaleX = static_cast<float>(scaleX);
    action->scaleY = static_cast<float>(action->scaleY + step);
    if (scaleX > 1.0) {
        action->scaleX = 1.0F;
        action->scaleY = 1.0F;
    }
    if (payload->value1024 == 0) {
        action->battleCastEffect10 = NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 23, 0, 1);
    }
    if (std::bit_cast<std::uint32_t>(action->scaleX) == 0x3F800000u) {
        YobiAction* const effect = action->battleCastEffect10;
        if (effect != nullptr) {
            static_cast<void>(MarkDeath(effect));
            action->battleCastEffect10 = nullptr;
        }
        const bool currentDevice = render::HasLegacy3DDevice_44B610() != 0;
        action->flags &= currentDevice ? ~0x100000u : ~0x8000u;
        payload->value1024 = 0;
        AdvanceBattleCastState_439340(action);
    }
    return PlayFrame_517DA0(action, 1);
}

// BSA01：播放弃返后实际扣FP，再推进，无终帧门禁。
// 原函数：0x0040C0C0。
void UpdateBattleAction55_40C0C0(YobiAction* const action)
{
    static_cast<void>(PlayFrame_517DA0(action, 1));
    ConsumeBattleFpCost_405B60(action);
    AdvanceBattleCastState_439340(action);
}

// BSA01：动作5严格终帧，按index/remain/event顺序清零后推进。
// 原函数：0x0040C160。
void UpdateBattleAction56_40C160(YobiAction* const action)
{
    action->characterAction = 5;
    if (PlayFrame_517DA0(action, 1) == 1) {
        action->frameIndex = 0;
        action->frameRemain = 0;
        action->frameEvent = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// BSA01：动作13严格终帧，仅清index/remain，保事件。
// 原函数：0x0040C1A0。
void UpdateBattleAction57_40C1A0(YobiAction* const action)
{
    action->characterAction = 13;
    if (PlayFrame_517DA0(action, 1) == 1) {
        action->frameIndex = 0;
        action->frameRemain = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// BSA01：动作14严格终帧，仅清index/remain，保事件。
// 原函数：0x0040C1E0。
void UpdateBattleAction58_40C1E0(YobiAction* const action)
{
    action->characterAction = 14;
    if (PlayFrame_517DA0(action, 1) == 1) {
        action->frameIndex = 0;
        action->frameRemain = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// BSA01：动作15严格终帧，仅清index/remain，保事件。
// 原函数：0x0040C240。
void UpdateBattleAction59_40C240(YobiAction* const action)
{
    action->characterAction = 15;
    if (PlayFrame_517DA0(action, 1) == 1) {
        action->frameIndex = 0;
        action->frameRemain = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// BSA01：动作15后直接推进，无播放/清帧。
// 原函数：0x0040C220。
void UpdateBattleAction60_40C220(YobiAction* const action)
{
    action->characterAction = 15;
    AdvanceBattleCastState_439340(action);
}

// BSA01：动作12严格终帧后推进，不清帧状态。
// 原函数：0x0040C280。
void UpdateBattleAction61_40C280(YobiAction* const action)
{
    action->characterAction = 12;
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// BSA01：动作18严格终帧后推进，不清帧状态。
// 原函数：0x0040C2B0。
void UpdateBattleAction62_40C2B0(YobiAction* const action)
{
    action->characterAction = 18;
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// BSA01：旧70/新20/fresh50，方向两次独立读，一次wrap后互斥播放返回。
// 原函数：0x0040C2E0。
std::int32_t UpdateBattleAction63_40C2E0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 0;
    const std::int32_t previous = payload->valueFF0;
    const std::int32_t count = previous + 1;
    payload->valueFF0 = count;
    if (previous >= 70) {
        AdvanceBattleCastState_439340(action);
        payload->valueFF0 = 0;
    } else {
        if (count == 20) {
            action->direction -= 2;
        }
        if (payload->valueFF0 == 50) {
            action->direction += 4;
        }
        const std::int32_t direction = action->direction;
        if (direction >= 8) {
            action->direction = direction - 8;
        }
        const std::int32_t correctedDirection = action->direction;
        if (correctedDirection < 0) {
            action->direction = correctedDirection + 8;
            return PlayFrame_517DA0(action, 0);
        }
    }
    return PlayFrame_517DA0(action, 0);
}

// BTE01：仅动作0后推进，无播放/清帧。
// 原函数：0x0040CBB0。
void UpdateBattleAction64_40CBB0(YobiAction* const action)
{
    action->characterAction = 0;
    AdvanceBattleCastState_439340(action);
}

// BTE01：旧70/新8/七fresh计数，方向独立读/单圈修正和互斥播放保持。
// 原函数：0x0040C380。
std::int32_t UpdateBattleAction65_40C380(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 0;
    const std::int32_t previous = payload->valueFF0;
    const std::int32_t count = previous + 1;
    payload->valueFF0 = count;
    if (previous >= 70) {
        AdvanceBattleCastState_439340(action);
        payload->valueFF0 = 0;
    } else {
        if (count == 8) {
            --action->direction;
        }
        if (payload->valueFF0 == 16) {
            ++action->direction;
        }
        if (payload->valueFF0 == 24) {
            ++action->direction;
        }
        if (payload->valueFF0 == 32) {
            --action->direction;
        }
        if (payload->valueFF0 == 40) {
            --action->direction;
        }
        if (payload->valueFF0 == 48) {
            ++action->direction;
        }
        if (payload->valueFF0 == 56) {
            ++action->direction;
        }
        if (payload->valueFF0 == 64) {
            --action->direction;
        }
        const std::int32_t direction = action->direction;
        if (direction >= 8) {
            action->direction = direction - 8;
        }
        const std::int32_t correctedDirection = action->direction;
        if (correctedDirection < 0) {
            action->direction = correctedDirection + 8;
            return PlayFrame_517DA0(action, 0);
        }
    }
    return PlayFrame_517DA0(action, 0);
}

// BTE01：独立1014图号，严格终帧推进后才清事件。
// 原函数：0x0040C480。
void UpdateBattleAction66_40C480(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 5;
    action->imageNo = payload->image1014;
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
        action->frameEvent = 0;
    }
}

// BTE01：入口事件先持，逐目标启动/效果推进，哨兵处仍推进一次再播放。
// 原函数：0x004106B0。
void UpdateBattleAction67_4106B0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    const std::int32_t event = action->frameEvent;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 6;
    if (event >= 10000 && event < 20000) {
        for (;;) {
            const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
            if (position == -1) {
                break;
            }
            MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[position]);
            static_cast<void>(AdvanceBattleEffect_439470(action));
        }
        static_cast<void>(AdvanceBattleEffect_439470(action));
        action->frameEvent = 0;
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// BTE01：浮动创建后fresh低BYTE60h门禁，条件播放后总推进。
// 原函数：0x0040C0E0。
void UpdateBattleAction68_40C0E0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    static_cast<void>(NewBattleFloatingIcon_41F4C0(action, payload->reactionImage1010, 0, 0));
    if ((static_cast<std::uint8_t>(payload->flagsFDC) & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 1));
    }
    AdvanceBattleCastState_439340(action);
}

// BTE01：30帧命中创建后fresh低BYTE60h，条件播放后总推进。
// 原函数：0x0040C120。
void UpdateBattleAction69_40C120(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    static_cast<void>(NewBattleHitEffect_41F5B0(action, 0, 0, 30, payload->reactionImage1010));
    if ((static_cast<std::uint8_t>(payload->flagsFDC) & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 1));
    }
    AdvanceBattleCastState_439340(action);
}

// BTE01：无播放/事件门禁，逐目标启动且哨兵额外推进，再推进源。
// 原函数：0x0040C4D0。
void UpdateBattleAction71_40C4D0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    for (;;) {
        const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
        if (position == -1) {
            break;
        }
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[position]);
        static_cast<void>(AdvanceBattleEffect_439470(action));
    }
    static_cast<void>(AdvanceBattleEffect_439470(action));
    AdvanceBattleCastState_439340(action);
}

// BTE01：caster当前载荷原00槽递增后推进源，不互换两对象。
// 原函数：0x0040CAA0。
void UpdateBattleAction72_40CAA0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    auto* const casterPayload = payload->caster1070->payload.battleActor;
    ++casterPayload->x00;
    AdvanceBattleCastState_439340(action);
}

// BCLC01：1004置1后推进，无播放/动作或速率发布。
// 原函数：0x0040CB10。
void UpdateBattleAction75_40CB10(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    payload->flash1004 = 1;
    AdvanceBattleCastState_439340(action);
}

// BCLC01：1004清零后推进，无播放/动作或速率发布。
// 原函数：0x0040CB30。
void UpdateBattleAction76_40CB30(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    payload->flash1004 = 0;
    AdvanceBattleCastState_439340(action);
}

// BCLC01：设备查询后fresh完整flags仅置对应位，再推进。
// 原函数：0x0040CB50。
void UpdateBattleAction77_40CB50(YobiAction* const action)
{
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    action->flags |= hasDevice ? 0x100000u : 0x8000u;
    AdvanceBattleCastState_439340(action);
}

// BCLC01：设备查询后fresh完整flags仅清对应位，再推进。
// 原函数：0x0040CB80。
void UpdateBattleAction78_40CB80(YobiAction* const action)
{
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    action->flags &= hasDevice ? ~0x100000u : ~0x8000u;
    AdvanceBattleCastState_439340(action);
}

// BCLC01：1064/106C/三辅助逐槽标死清理，两完整标志先读齐再FDC/FE0发布。
// 原函数：0x0040CBD0。
void UpdateBattleAction79_40CBD0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* effect = payload->centeredEffect1064;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->centeredEffect1064 = nullptr;
    }
    effect = payload->rebirthEffect106C;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->rebirthEffect106C = nullptr;
    }
    for (YobiAction*& slot : payload->rebirthAuxiliaryEffects1074) {
        effect = slot;
        if (effect != nullptr) {
            static_cast<void>(MarkDeath(effect));
            slot = nullptr;
        }
    }
    action->characterAction = 0;
    const std::uint32_t flags = payload->flagsFDC & ~2u;
    const std::uint32_t state = payload->stateFE0 & ~2u;
    payload->flagsFDC = flags;
    payload->stateFE0 = state;
    AdvanceBattleCastState_439340(action);
}

// BCLC01：持X后读取独立1018音效，WAV播放后推进。
// 原函数：0x0040CC80。
void UpdateBattleAction80_40CC80(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t x = action->x;
    const std::int32_t sound = payload->value1018;
    static_cast<void>(audio::PlaySe_52C450(sound, x));
    AdvanceBattleCastState_439340(action);
}

// BCLC01：持X后读取独立101C音效，WAV播放后推进。
// 原函数：0x0040CCB0。
void UpdateBattleAction81_40CCB0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t x = action->x;
    const std::int32_t sound = payload->value101C;
    static_cast<void>(audio::PlaySe_52C450(sound, x));
    AdvanceBattleCastState_439340(action);
}

// BCLC01：持X后读取独立1020音效，WAV播放后推进。
// 原函数：0x0040CCE0。
void UpdateBattleAction82_40CCE0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t x = action->x;
    const std::int32_t sound = payload->value1020;
    static_cast<void>(audio::PlaySe_52C450(sound, x));
    AdvanceBattleCastState_439340(action);
}

// BCLC01：未舍入阈值/正常F32发布，推进后fresh payload/FDC置位，末播放返回。
// 原函数：0x0040CD10。
std::int32_t UpdateBattleAction83_40CD10(YobiAction* const action)
{
    const double step = 0.005F;
    const double scaleX = action->scaleX + step;
    action->characterAction = 9;
    action->scaleX = static_cast<float>(scaleX);
    action->scaleY = static_cast<float>(action->scaleY + step);
    if (scaleX >= 1.3F) {
        AdvanceBattleCastState_439340(action);
        auto* const currentPayload = action->payload.battleActor;
        currentPayload->flagsFDC |= 0x1000u;
    }
    return PlayFrame_517DA0(action, 0);
}

// BRAF01：未舍入缩小/推进后新payload清位，尾BYTE播放门禁仍从入口payload取。
// 原函数：0x0040CD70。
void UpdateBattleAction84_40CD70(YobiAction* const action)
{
    const double step = 0.01F;
    const double scaleX = action->scaleX - step;
    auto* const payload = action->payload.battleActor;
    action->scaleX = static_cast<float>(scaleX);
    action->scaleY = static_cast<float>(action->scaleY - step);
    if (!(scaleX > 1.0)) {
        action->scaleX = 1.0F;
        action->scaleY = 1.0F;
        AdvanceBattleCastState_439340(action);
        auto* const currentPayload = action->payload.battleActor;
        currentPayload->flagsFDC &= ~0x1000u;
    }
    if ((static_cast<std::uint8_t>(payload->flagsFDC) & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
}

// BRAF01：未舍入放大/独立方向，2002原名称-三列表选图，入口payload尾BYTE门禁。
// 原函数：0x0040CDE0。
void UpdateBattleAction85_40CDE0(YobiAction* const action)
{
    using world::gBattleNamedEffects5ABCE0;
    const double step = 0.01F;
    const double scaleX = action->scaleX + step;
    const std::int32_t previousDirection = action->direction;
    auto* const payload = action->payload.battleActor;
    const std::int32_t direction = previousDirection + 1;
    action->characterAction = 9;
    action->scaleX = static_cast<float>(scaleX);
    const double scaleY = action->scaleY + step;
    action->direction = direction;
    action->scaleY = static_cast<float>(scaleY);
    if (direction >= 8) {
        action->direction = direction - 8;
    }
    if (scaleX >= 2.0) {
        if (payload->castSkill103C == 2002) {
            std::int32_t namedIndex = 0;
            while (std::strcmp(cg::world::gBattleNamedEffects5ABCE0[namedIndex].name.data(), action->nameC4.data()) != 0) {
                ++namedIndex;
            }
            std::int32_t imageNo = 110154;
            for (std::int32_t index = 0; kBattleNamedEffectImages57E174[index][0] != 0; ++index) {
                if (kBattleNamedEffectImages57E174[index][0] == cg::world::gBattleNamedEffects5ABCE0[namedIndex].value) {
                    imageNo = kBattleNamedEffectImages57E174[index][1];
                    break;
                }
            }
            static_cast<void>(NewBattleRandomEffect_419FC0(action, imageNo, 0));
        } else {
            static_cast<void>(NewBattleRandomEffect_419FC0(action, 110154, 0));
        }
        AdvanceBattleCastState_439340(action);
    }
    if ((static_cast<std::uint8_t>(payload->flagsFDC) & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
}

// BRAF01：固定视觉target与动态HP/phase目标、两BYTE及临时index/encoded查找前持样。
// 原函数：0x0040C520。
void UpdateBattleAction86_40C520(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    do {
        YobiAction* const target = cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        auto* const targetPayload = target->payload.battleActor;
        target->hidden = 0;
        TurnBattleToward_406190(target, action, 1, 0.0F);
        Sub_405CB0(target);
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 0x20u) != 0) {
            targetPayload->flagsFDC |= 2u;
            gYobiUpdateGateMode = 1;
            targetPayload->aliveFF8 = 1;
        }
        if ((payload->effects1C[payload->effectRunningC9C].flags & 0x100000u) != 0) {
            static_cast<void>(MarkDeath(targetPayload->effect1054));
            const std::uint32_t flags = targetPayload->flagsFDC & ~0x20u;
            targetPayload->effect1054 = nullptr;
            targetPayload->flagsFDC = flags;
        }
        const std::int32_t effectIndex = payload->effectRunningC9C;
        const std::uint32_t flags = payload->effects1C[effectIndex].flags;
        if ((flags & 0x10000000u) != 0) {
            static_cast<void>(NewBattleFloatingIcon_41F4C0(
                cg::world::gBattleAllEntries59E980[payload->effects1C[effectIndex].position], 22459, 0, 0));
        } else {
            const std::int32_t x = target->x;
            const std::int32_t amount = payload->effects1C[effectIndex].position3;
            const std::int32_t y = target->y - 80;
            static_cast<void>(NewBattleNumber_41F1A0(x, y, amount, 6, 2, flags));
            if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
            }
            static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
            const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
            const std::int32_t damage = effect.position3;
            const std::int32_t position = effect.position;
            (cg::world::gBattleAllEntries59E980[position]->readiness -= static_cast<std::int32_t>(damage));
            YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                payload->effects1C[payload->effectRunningC9C].position];
            if ((compareTarget->readiness < 0)) {
                std::optional<platform::LegacyCryptInt> temporary;
                temporary.emplace(static_cast<std::int32_t>(0));
                const std::int32_t copyEffectIndex = payload->effectRunningC9C;
                const std::uint32_t encoded = temporary->encoded;
                YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[copyEffectIndex].position];
                copyTarget->readiness.encoded = encoded;
                copyTarget->readiness.mask = temporary->mask;
                copyTarget->readiness.readCount = temporary->readCount;
                temporary.reset();
            }
        }
        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position]);
        YobiAction* const currentTarget = cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        if (currentTarget->phase == 0) {
            AdvanceBattleCastState_439340(currentTarget);
        }
    } while (AdvanceBattleEffect_439470(action) != 1);
    gYobiScreenOffsetMaximumAmplitude = 6;
    gYobiScreenOffsetRising = 1;
    AdvanceBattleCastState_439340(action);
}

// BRAF01：十位置逐次阵营，父gate/image/Y/X/创建后声RNG序与270门禁保持。
// 原函数：0x0040C780。
std::int32_t UpdateBattleAction87_40C780(YobiAction* const action)
{
    using world::gBattleGraphicPoints553758;
    auto* const payload = action->payload.battleActor;
    for (std::int32_t index = 0; index < 10; ++index) {
        bool playSound = false;
        if (payload->battlePosition18 < 10) {
            if (platform::RandomRange_46FAF0(0, 2) == 0) {
                const std::int32_t imageNo = platform::RandomRange_46FAF0(-1, 0) + 110007;
                const std::int32_t randomY = platform::RandomRange_46FAF0(-60, 0);
                const std::int32_t y = randomY + cg::world::gBattleGraphicPoints553758[index + 10][3];
                const std::int32_t randomX = platform::RandomRange_46FAF0(-30, 30);
                const std::int32_t x = randomX + cg::world::gBattleGraphicPoints553758[index + 10][2];
                static_cast<void>(NewBattleScreenEffect_41A0D0(x, y, imageNo, 0));
                playSound = platform::RandomRange_46FAF0(0, 30) == 0;
            }
        } else if (platform::RandomRange_46FAF0(0, 2) == 0) {
            const std::int32_t imageNo = platform::RandomRange_46FAF0(-1, 0) + 110007;
            const std::int32_t randomY = platform::RandomRange_46FAF0(-80, 0);
            const std::int32_t y = randomY + cg::world::gBattleGraphicPoints553758[index][3];
            const std::int32_t randomX = platform::RandomRange_46FAF0(-40, 40);
            const std::int32_t x = randomX + cg::world::gBattleGraphicPoints553758[index][2];
            static_cast<void>(NewBattleScreenEffect_41A0D0(x, y, imageNo, 0));
            playSound = platform::RandomRange_46FAF0(0, 30) == 0;
        }
        if (playSound) {
            static_cast<void>(audio::PlaySe_52C450(263, 320));
        }
    }
    const std::int32_t count = payload->valueFF0 + 1;
    payload->valueFF0 = count;
    if (count >= 270) {
        payload->valueFF0 = 0;
        AdvanceBattleCastState_439340(action);
        static_cast<void>(audio::PlaySe_52C450(263, 320));
    }
    return PlayFrame_517DA0(action, 0);
}

// BRAF01：同源振幅6/启用1后推进，无播放。
// 原函数：0x0040C8D0。
void UpdateBattleAction88_40C8D0(YobiAction* const action)
{
    gYobiScreenOffsetMaximumAmplitude = 6;
    gYobiScreenOffsetRising = 1;
    AdvanceBattleCastState_439340(action);
}

// BRAF01：仅同源启用0后推进，不清振幅。
// 原函数：0x0040C8F0。
void UpdateBattleAction89_40C8F0(YobiAction* const action)
{
    gYobiScreenOffsetRising = 0;
    AdvanceBattleCastState_439340(action);
}

// BRAF01：转向后事件/目标重取，FD0设4当帧减1，原四边界及额外推进保持。
// 原函数：0x0040C910。
void UpdateBattleAction90_40C910(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    const std::int32_t event = action->frameEvent;
    auto* const payload = action->payload.battleActor;
    action->value54 = 10.0F;
    if (event == 0) {
        const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
        if (effect.position != -1) {
            payload->flash1004 = 1;
            TurnBattleToward_406190(action, cg::world::gBattleAllEntries59E980[effect.position], 0, 20.0F);
            if (action->frameEvent == 0
                && GetBattleDistance_406300(action, cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position]) < 20) {
                MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position]);
                YobiAction* const currentTarget = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                if (currentTarget->phase == 0) {
                    AdvanceBattleCastState_439340(currentTarget);
                }
                payload->valueFD0 = 4;
                if (AdvanceBattleEffect_439470(action) == 1) {
                    action->frameEvent = 1;
                }
            }
        }
    }
    const std::int32_t delay = payload->valueFD0;
    if (delay > 0) {
        payload->valueFD0 = delay - 1;
    }
    if (payload->valueFD0 == 0) {
        const std::int32_t direction = action->direction + 1;
        action->direction = direction;
        if (direction >= 8) {
            action->direction = 0;
        }
        static_cast<void>(PlayFrame_517DA0(action, 0));
        MoveBattleByAngle_405EA0(action);
        if (!(action->offset40 >= -200.0F) || action->offset40 > 840.0F
            || !(action->offset44 >= -200.0F) || action->offset44 > 680.0F) {
            static_cast<void>(AdvanceBattleEffect_439470(action));
            AdvanceBattleCastState_439340(action);
        }
    }
}

// BRAF01：完整对象flags置2、先推进后PlayFrame(0)返回。
// 原函数：0x0040CF30。
std::int32_t UpdateBattleAction91_40CF30(YobiAction* const action)
{
    action->flags |= 2u;
    AdvanceBattleCastState_439340(action);
    return PlayFrame_517DA0(action, 0);
}

// BRAF01：完整对象flags清2、先推进后PlayFrame(0)返回。
// 原函数：0x0040CF50。
std::int32_t UpdateBattleAction92_40CF50(YobiAction* const action)
{
    action->flags &= ~2u;
    AdvanceBattleCastState_439340(action);
    return PlayFrame_517DA0(action, 0);
}

// BSTL01：先Count<=1再alive清零与推进，入口payload固定。
// 原函数：0x0040CF70。
void UpdateBattleAction93_40CF70(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (CountBattleActiveEntries_406360() <= 1) {
        gYobiUpdateGateMode = 0;
    }
    payload->aliveFF8 = 0;
    AdvanceBattleCastState_439340(action);
}

// BSTL01：X发布后fresh Y/angle/目标，单圈归一化后50步进及重取转向。
// 原函数：0x0040D010。
void UpdateBattleAction94_40D010(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->offset40 = cg::world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position]->offset40;
    action->offset44 = cg::world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position]->offset44;
    const float targetAngle = cg::world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position]->angle50;
    action->angle50 = static_cast<float>(targetAngle - 180.0);
    NormalizeBattleAngle_405E60(action);
    action->value54 = 50.0F;
    MoveBattleByAngle_405EA0(action);
    TurnBattleToward_406190(action, cg::world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
    AdvanceBattleCastState_439340(action);
}

// BSTL01：旧FF0的16门禁；推进后入口载荷清零，临时index/encoded先查目标。
// 原函数：0x0040D0B0。
void UpdateBattleAction95_40D0B0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (payload->valueFF0 == 0) {
        action->characterAction = 3;
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    action->value54 = 6.0F;
    const std::int32_t previous = payload->valueFF0;
    payload->valueFF0 = previous + 1;
    if (previous < 16) {
        MoveBattleByAngle_405EA0(action);
    } else {
        AdvanceBattleCastState_439340(action);
        payload->valueFF0 = 0;
        std::optional<platform::LegacyCryptInt> temporary;
        temporary.emplace(static_cast<std::int32_t>(0));
        const std::int32_t effectIndex = payload->effectRunningC9C;
        const std::uint32_t encoded = temporary->encoded;
        YobiAction* const target = world::gBattleAllEntries59E980[
            payload->effects1C[effectIndex].position];
        target->readiness.encoded = encoded;
        target->readiness.mask = temporary->mask;
        target->readiness.readCount = temporary->readCount;
        temporary.reset();
    }
}

// BSTL01：当前动态目标hidden=0后仅推进source。
// 原函数：0x0040D170。
void UpdateBattleAction96_40D170(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position]->hidden = 0;
    AdvanceBattleCastState_439340(action);
}

// BSTL01：当前动态目标hidden=1后仅推进source。
// 原函数：0x0040D1A0。
void UpdateBattleAction97_40D1A0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position]->hidden = 1;
    AdvanceBattleCastState_439340(action);
}

// BSTL01：入口事件与终帧独立选图，附属门禁后两次skill及共享铺格生产。
// 原函数：0x0040D1D0。
void UpdateBattleAction98_40D1D0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    const std::int32_t event = action->frameEvent;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 6;
    if (event >= 10000) {
        const std::int32_t imageNo = payload->castSkillLevel1044 < 12 ? 110074 : 111056;
        static_cast<void>(NewBattleRandomEffect_419FC0(
            cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], imageNo, 0));
        action->frameEvent = 0;
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        const std::int32_t imageNo = payload->castSkillLevel1044 < 12 ? 110074 : 111056;
        static_cast<void>(NewBattleRandomEffect_419FC0(
            cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], imageNo, 0));
        AdvanceBattleCastState_439340(action);
    }
    if (action->battleCastEffect10 != nullptr) {
        const std::int32_t skill = payload->castSkill103C;
        std::int32_t type = -1;
        switch (skill) {
        case 19: case 23: case 27: case 2068: case 2069: case 2070:
            type = 0;
            break;
        case 20: case 24: case 28: case 2071: case 2072: case 2073:
            type = 1;
            break;
        case 21: case 25: case 29: case 2074: case 2075: case 2076:
            type = 2;
            break;
        case 22: case 26: case 30: case 2077: case 2078: case 2079:
            type = 3;
            break;
        case 31: case 3037:
            type = 4;
            break;
        default:
            break;
        }
        if (type >= 0) {
            static_cast<void>(DrawBattleTiledEffect_41E890(type));
        }
        const std::int32_t currentSkill = payload->castSkill103C;
        if (currentSkill == 86 || currentSkill == 2055 || currentSkill == 2056 || currentSkill == 2057) {
            static_cast<void>(DrawBattleTiledEffect_41E890(5));
        }
    }
}

// BCPY01：rate25/推进后清帧索引和剩余帧，返回0而不清事件/播放。
// 原函数：0x0040D390。
std::int32_t UpdateBattleAction99_40D390(YobiAction* const action)
{
    action->framePlayingRate = 25;
    AdvanceBattleCastState_439340(action);
    action->frameIndex = 0;
    action->frameRemain = 0;
    return 0;
}

// BCPY01：position-frameIndex-目标取样，S8序列/fresh共享游标与保存BYTE分档。
// 原函数：0x0040D3C0。
void UpdateBattleAction100_40D3C0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    const std::int32_t frameIndex = action->frameIndex;
    YobiAction* const target = cg::world::gBattleAllEntries59E980[position];
    auto* const targetPayload = target->payload.battleActor;
    if (frameIndex == 0 && action->frameRemain == 0) {
        action->characterAction = kBattleActionSequence5538F4[
            payload->castSkillLevel1044 * 20 + gBattleActionSequenceIndex59F9DC];
        ++gBattleActionSequenceIndex59F9DC;
    }
    if (PlayFrame_517DA0(action, 1) != 1) {
        return;
    }
    targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
        payload->effects1C[payload->effectRunningC9C].flags);
    const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
    if ((static_cast<std::uint8_t>(flags >> 8) & 0x20u) != 0) {
        static_cast<void>(NewBattleEffect_41CD00(target, 0, {.offsetY = 0}, 7, 0, 1));
    } else if ((static_cast<std::uint8_t>(flags >> 8) & 0x80u) != 0) {
        static_cast<void>(NewBattleEffect_41CD00(target, 0, {.offsetY = 0}, 9, 0, 1));
    } else if ((static_cast<std::uint8_t>(flags >> 8) & 0x40u) != 0) {
        static_cast<void>(NewBattleEffect_41CD00(target, 0, {.offsetY = 0}, 8, 0, 1));
    } else {
        target->phase = 26;
        const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
        const std::uint32_t currentFlags = effect.flags;
        std::int32_t imageNo;
        if ((static_cast<std::uint8_t>(currentFlags) & 4u) != 0) {
            imageNo = 110000;
        } else if ((static_cast<std::uint8_t>(currentFlags >> 8) & 4u) != 0) {
            imageNo = FindBattleAppendEffectTechnique_43BEA0(8, effect.position) < 12 ? 110098 : 111048;
        } else {
            imageNo = 110133;
        }
        static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
        NewBattleRebirthEffects_4208F0(action, target);
    }
    const std::int32_t x = action->x;
    const std::int32_t sound = platform::RandomRange_46FAF0(131, 132);
    static_cast<void>(audio::PlaySe_52C450(sound, x));
    action->frameIndex = 0;
    action->frameRemain = 0;
    action->frameEvent = 0;
    if (kBattleActionSequence5538F4[payload->castSkillLevel1044 * 20 + gBattleActionSequenceIndex59F9DC] < 0) {
        gBattleActionSequenceIndex59F9DC = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// BCPY01：三次独立位置判断/中心汇聚，抵达先推进再角色动作9/清FF0。
// 原函数：0x0040D580。
std::int32_t UpdateBattleAction101_40D580(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 3;
    action->value54 = 8.0F;
    action->framePlayingRate = 100;
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    TurnBattleTowardPosition_4060D0(action, position < 10 ? 356.0F : 278.0F,
        position < 10 ? 276.0F : 278.0F, 1, 0.0F);
    bool arrived = false;
    float facingX;
    float facingY;
    if (payload->effects1C[payload->effectRunningC9C].position >= 10
        && GetBattleDistanceToPosition_4062A0(action, 278.0F, 278.0F) <= 8) {
        action->offset40 = 278.0F;
        action->offset44 = 278.0F;
        facingX = 98.0F;
        facingY = 128.0F;
        arrived = true;
    } else if (payload->effects1C[payload->effectRunningC9C].position <= 9
        && GetBattleDistanceToPosition_4062A0(action, 356.0F, 276.0F) <= 8) {
        action->offset40 = 356.0F;
        action->offset44 = 276.0F;
        facingX = 542.0F;
        facingY = 407.0F;
        arrived = true;
    }
    if (arrived) {
        TurnBattleTowardPosition_4060D0(action, facingX, facingY, 1, 0.0F);
        AdvanceBattleCastState_439340(action);
        action->characterAction = 9;
        payload->valueFF0 = 0;
        return PlayFrame_517DA0(action, 0);
    }
    MoveBattleByAngle_405EA0(action);
    return PlayFrame_517DA0(action, 0);
}

// BCPY01：旧8门禁/固定视觉与动态HP，两样本BYTE及临时零复制原读序。
// 原函数：0x0040D6C0。
void UpdateBattleAction102_40D6C0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 5;
    action->value54 = 32.0F;
    action->framePlayingRate = 25;
    const std::int32_t previous = payload->valueFF0;
    payload->valueFF0 = previous + 1;
    if (previous < 8) {
        MoveBattleByAngle_405EA0(action);
        static_cast<void>(PlayFrame_517DA0(action, 1));
        return;
    }
    do {
        const BattleEntryEffectRecord& initial = payload->effects1C[payload->effectRunningC9C];
        const std::int32_t position = initial.position;
        const std::uint32_t initialFlags = initial.flags;
        YobiAction* const target = cg::world::gBattleAllEntries59E980[position];
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t imageNo;
        if ((static_cast<std::uint8_t>(initialFlags) & 4u) != 0) {
            imageNo = 110000;
        } else if ((static_cast<std::uint8_t>(initialFlags >> 8) & 4u) != 0) {
            imageNo = FindBattleAppendEffectTechnique_43BEA0(8, position) < 12 ? 110098 : 111048;
        } else {
            imageNo = 110133;
        }
        const std::int32_t sourcePosition = payload->battlePosition18;
        TurnBattleTowardPosition_4060D0(target, sourcePosition > 9 ? 356.0F : 278.0F,
            sourcePosition > 9 ? 276.0F : 278.0F, 1, 0.0F);
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        const std::int32_t hitIndex = payload->effectRunningC9C;
        const std::uint32_t hitFlags = payload->effects1C[hitIndex].flags;
        if ((static_cast<std::uint8_t>(hitFlags) & 8u) != 0) {
            target->phase = 31;
        } else if ((static_cast<std::uint8_t>(hitFlags >> 8) & 1u) != 0) {
            targetPayload->flash1004 = 1;
            static_cast<void>(audio::PlaySe_52C450(210, action->x));
            target->phase = 31;
        } else {
            if ((static_cast<std::uint8_t>(hitFlags >> 8) & 0xE0u) == 0 && payload->effects1C[hitIndex].position3 != 0) {
                if ((static_cast<std::uint8_t>(hitFlags) & 2u) != 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                }
                if (imageNo >= 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                }
                NewBattleRebirthEffects_4208F0(action, target);
                static_cast<void>(audio::PlaySe_52C450(134, action->x));
                const BattleEntryEffectRecord& damage = payload->effects1C[payload->effectRunningC9C];
                const std::int32_t amount = damage.position3;
                (cg::world::gBattleAllEntries59E980[damage.position]->readiness -= static_cast<std::int32_t>(amount));
                YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                if ((compareTarget->readiness < 0)) {
                    std::optional<platform::LegacyCryptInt> temporary;
                    temporary.emplace(static_cast<std::int32_t>(0));
                    const std::int32_t copyEffectIndex = payload->effectRunningC9C;
                    const std::uint32_t encoded = temporary->encoded;
                    YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[copyEffectIndex].position];
                    copyTarget->readiness.encoded = encoded;
                    copyTarget->readiness.mask = temporary->mask;
                    copyTarget->readiness.readCount = temporary->readCount;
                    temporary.reset();
                }
                if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 0x20u) != 0) {
                    targetPayload->flagsFDC |= 2u;
                    gYobiUpdateGateMode = 1;
                    targetPayload->aliveFF8 = 1;
                }
            }
            const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
            bool showAttributes = false;
            if ((static_cast<std::uint8_t>(flags >> 8) & 0x80u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else if ((static_cast<std::uint8_t>(flags >> 8) & 0x40u) != 0) {
                AdvanceBattleCastState_439340(target);
                const std::int32_t healIndex = payload->effectRunningC9C;
                if ((payload->effects1C[healIndex].flags & 0x10000000u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[healIndex].position], 22459, 0, 0));
                } else {
                    const std::int32_t amount = payload->effects1C[healIndex].position3;
                    (cg::world::gBattleAllEntries59E980[payload->effects1C[healIndex].position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const BattleEntryEffectRecord& current = payload->effects1C[payload->effectRunningC9C];
                    const std::uint32_t displayFlags = current.flags;
                    const std::int32_t displayed = current.position3;
                    const std::int32_t y = target->y - 80;
                    const std::int32_t x = target->x;
                    static_cast<void>(NewBattleNumber_41F1A0(x, y, displayed, 5, 2, displayFlags));
                    showAttributes = true;
                }
            } else if ((static_cast<std::uint8_t>(flags >> 8) & 0x20u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else {
                if ((flags & 0x100000u) != 0) {
                    static_cast<void>(MarkDeath(targetPayload->effect1054));
                    const std::uint32_t state = targetPayload->flagsFDC & ~0x20u;
                    targetPayload->effect1054 = nullptr;
                    targetPayload->flagsFDC = state;
                }
                AdvanceBattleCastState_439340(target);
                const std::int32_t displayIndex = payload->effectRunningC9C;
                const std::int32_t amount = payload->effects1C[displayIndex].position3;
                if (amount == 0 && (static_cast<std::uint8_t>(payload->effects1C[displayIndex].flags >> 24) & 0x10u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[displayIndex].position], 22459, 0, 0));
                } else {
                    const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                    const std::int32_t y = target->y - 80;
                    const std::int32_t x = target->x;
                    static_cast<void>(NewBattleNumber_41F1A0(x, y, amount, 6, 2, displayFlags));
                    showAttributes = true;
                }
            }
            if (showAttributes) {
                if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                }
                static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
            }
        }
        const std::int32_t tailIndex = payload->effectRunningC9C;
        if ((static_cast<std::uint8_t>(payload->effects1C[tailIndex].flags) & 8u) == 0
            && payload->effects1C[tailIndex].position3 != 0) {
            payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
        }
    } while (AdvanceBattleEffect_439470(action) != 1);
    action->value54 = 8.0F;
    action->framePlayingRate = 100;
    payload->valueFF0 = 0;
    AdvanceBattleCastState_439340(action);
}

// BIE01：先读全局玩家位，成功分支重取；按原CryptInt前两W32显示并保额外效果推进。
// 原函数：0x0040DC90。
void UpdateBattleAction104_40DC90(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    using world::gBattleCurrentPosition5ABCA4;
    const std::int32_t currentPosition = cg::world::gBattleCurrentPosition5ABCA4;
    auto* const payload = action->payload.battleActor;
    if (payload->battlePosition18 == currentPosition) {
        std::wstring message;
        for (;;) {
            const std::int32_t index = payload->effectRunningC9C;
            const std::int32_t position = payload->effects1C[index].position;
            if (position == -1) {
                break;
            }
            YobiAction* const target = cg::world::gBattleAllEntries59E980[position];
            if ((payload->effects1C[index].flags & 0x80000u) != 0) {
                const std::int32_t latestPosition = cg::world::gBattleCurrentPosition5ABCA4;
                if (payload->battlePosition18 == latestPosition) {
                    // 原x86按值压入两份CryptInt；两个%d实际消费当前结构前两字，不调用数值解码。
                    std::array<std::uint32_t, 8> arguments;
                    std::memcpy(arguments.data() + 4, &target->maxHp, sizeof(platform::LegacyCryptInt));
                    const char* const name = target->nameC4.data();
                    std::memcpy(arguments.data(), &target->readiness, sizeof(platform::LegacyCryptInt));
                    cg::text::FormatTo(message, L"{} 的生命为 {} / {}" /* 原常量参考：0x005539E8。 */, cg::text::FromGbkWide(name).c_str(), std::bit_cast<std::int32_t>(arguments[0]), std::bit_cast<std::int32_t>(arguments[1]));
                    world::Sub_44B050(message, 4, 3);
                    std::memcpy(arguments.data() + 4, &target->barMaximum1C0, sizeof(platform::LegacyCryptInt));
                    std::memcpy(arguments.data(), &target->barValue1B0, sizeof(platform::LegacyCryptInt));
                    cg::text::FormatTo(message, L"{} 的魔力为 {} / {}" /* 原常量参考：0x005539D4。 */, cg::text::FromGbkWide(name).c_str(), std::bit_cast<std::int32_t>(arguments[0]), std::bit_cast<std::int32_t>(arguments[1]));
                    world::Sub_44B050(message, 4, 3);
                }
            } else {
                cg::text::FormatTo(message, L"无法调查出 {} 的情报" /* 原常量参考：0x005539BC。 */, cg::text::FromGbkWide(target->nameC4.data()).c_str());
                world::Sub_44B050(message, 4, 3);
            }
            static_cast<void>(AdvanceBattleEffect_439470(action));
        }
    } else {
        while (payload->effects1C[payload->effectRunningC9C].position != -1) {
            static_cast<void>(AdvanceBattleEffect_439470(action));
        }
    }
    static_cast<void>(AdvanceBattleEffect_439470(action));
    AdvanceBattleCastState_439340(action);
}

// BIE01：游标先源位置，两次门禁独立；旧计数>=120后推进源/入口目标，末播放返回。
// 原函数：0x0040DE40。
std::int32_t UpdateBattleAction105_40DE40(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* const target = world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position];
    action->characterAction = 16;
    action->framePlayingRate = 50;
    const std::int32_t currentIndex = payload->effectRunningC9C;
    const std::int32_t currentPosition = payload->battlePosition18;
    if (currentPosition != payload->effects1C[currentIndex].position) {
        const std::int32_t count = payload->valueFF0;
        if (count <= 30) {
            action->offset40 = target->offset40;
            action->offset44 = static_cast<float>(target->offset44 - 70.0);
        } else if (count <= 60) {
            action->offset44 = target->offset44;
            action->offset40 = static_cast<float>(target->offset40 + 70.0);
        } else if (count <= 90) {
            action->offset40 = target->offset40;
            action->offset44 = static_cast<float>(target->offset44 + 70.0);
        } else {
            action->offset44 = target->offset44;
            action->offset40 = static_cast<float>(target->offset40 - 70.0);
        }
        TurnBattleToward_406190(action, target, 1, 0.0F);
    }
    const std::int32_t previous = payload->valueFF0;
    payload->valueFF0 = previous + 1;
    if (previous >= 120) {
        payload->valueFF0 = 0;
        action->framePlayingRate = 100;
        AdvanceBattleCastState_439340(action);
        const std::int32_t tailIndex = payload->effectRunningC9C;
        const std::int32_t tailPosition = payload->battlePosition18;
        if (tailPosition != payload->effects1C[tailIndex].position) {
            AdvanceBattleCastState_439340(target);
        }
    }
    return PlayFrame_517DA0(action, 0);
}

// BIE01：完整动作106仅角色动作13→Advance，无播放/速率/帧清零。
// 原函数：0x0040DF50。
void UpdateBattleAction106_40DF50(YobiAction* const action)
{
    action->characterAction = 13;
    AdvanceBattleCastState_439340(action);
}

// BIE01：完整动作107调用MarkDeath并返回真实T*；物理释放沿scheduler。
// 原函数：0x0040DF70。
YobiAction* UpdateBattleAction107_40DF70(YobiAction* const action)
{
    return MarkDeath(action);
}

// BIE01：入口事件持样/逐效果do，13 BYTE与临时HP复制读序；免疫只按完整flags。
// 原函数：0x0040DF80。
void UpdateBattleAction108_40DF80(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    const std::int32_t event = action->frameEvent;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 5;
    action->framePlayingRate = 200;
    if (event >= 10000 && event < 20000) {
        do {
            const std::int32_t initialIndex = payload->effectRunningC9C;
            const BattleEntryEffectRecord& initial = payload->effects1C[initialIndex];
            const std::int32_t position = initial.position;
            YobiAction* const target = cg::world::gBattleAllEntries59E980[position];
            auto* const targetPayload = target->payload.battleActor;
            const std::uint32_t initialFlags = initial.flags;
            std::int32_t imageNo;
            if ((static_cast<std::uint8_t>(initialFlags) & 4u) != 0) {
                imageNo = 110000;
            } else if ((static_cast<std::uint8_t>(initialFlags >> 8) & 4u) != 0) {
                imageNo = FindBattleAppendEffectTechnique_43BEA0(8, position) < 12 ? 110098 : 111048;
            } else if ((static_cast<std::uint8_t>(initialFlags >> 8) & 2u) != 0 || initialIndex == 0) {
                imageNo = 110135;
            } else {
                imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
            }
            TurnBattleToward_406190(target, action, 1, 0.0F);
            if (payload->effectRunningC9C == 0) {
                static_cast<void>(NewBattleRandomEffect_419FC0(target, 110014, 0));
            }
            targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
                payload->effects1C[payload->effectRunningC9C].flags);
            ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
            const std::int32_t hitIndex = payload->effectRunningC9C;
            const std::uint32_t hitFlags = payload->effects1C[hitIndex].flags;
            if ((static_cast<std::uint8_t>(hitFlags) & 8u) != 0) {
                target->phase = 31;
            } else if ((static_cast<std::uint8_t>(hitFlags >> 8) & 1u) != 0) {
                targetPayload->flash1004 = 1;
                static_cast<void>(audio::PlaySe_52C450(210, action->x));
                target->phase = 31;
            } else {
                if ((static_cast<std::uint8_t>(hitFlags >> 8) & 0xE0u) == 0 && payload->effects1C[hitIndex].position3 != 0) {
                    if ((static_cast<std::uint8_t>(hitFlags) & 2u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                    }
                    if (imageNo >= 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                    }
                    NewBattleRebirthEffects_4208F0(action, target);
                    static_cast<void>(audio::PlaySe_52C450(134, action->x));
                    const BattleEntryEffectRecord& damage = payload->effects1C[payload->effectRunningC9C];
                    const std::int32_t amount = damage.position3;
                    (cg::world::gBattleAllEntries59E980[damage.position]->readiness -= static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness < 0)) {
                        std::optional<platform::LegacyCryptInt> temporary;
                        temporary.emplace(static_cast<std::int32_t>(0));
                        const std::int32_t copyIndex = payload->effectRunningC9C;
                        const std::uint32_t encoded = temporary->encoded;
                        YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[copyIndex].position];
                        copyTarget->readiness.encoded = encoded;
                        copyTarget->readiness.mask = temporary->mask;
                        copyTarget->readiness.readCount = temporary->readCount;
                        temporary.reset();
                    }
                    if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 0x20u) != 0) {
                        targetPayload->flagsFDC |= 2u;
                        gYobiUpdateGateMode = 1;
                        targetPayload->aliveFF8 = 1;
                    }
                }
                const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
                bool showAttributes = false;
                if ((static_cast<std::uint8_t>(flags >> 8) & 0x80u) != 0) {
                    AdvanceBattleCastState_439340(target);
                } else if ((static_cast<std::uint8_t>(flags >> 8) & 0x40u) != 0) {
                    AdvanceBattleCastState_439340(target);
                    const std::int32_t healIndex = payload->effectRunningC9C;
                    if ((payload->effects1C[healIndex].flags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            cg::world::gBattleAllEntries59E980[payload->effects1C[healIndex].position], 22459, 0, 0));
                    } else {
                        const std::int32_t amount = payload->effects1C[healIndex].position3;
                        (cg::world::gBattleAllEntries59E980[payload->effects1C[healIndex].position]->readiness += static_cast<std::int32_t>(amount));
                        YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        if ((compareTarget->readiness > compareTarget->maxHp)) {
                            YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                                payload->effects1C[payload->effectRunningC9C].position];
                            copyTarget->readiness = copyTarget->maxHp;
                        }
                        const BattleEntryEffectRecord& current = payload->effects1C[payload->effectRunningC9C];
                        const std::uint32_t displayFlags = current.flags;
                        const std::int32_t displayed = current.position3;
                        const std::int32_t y = target->y - 80;
                        const std::int32_t x = target->x;
                        static_cast<void>(NewBattleNumber_41F1A0(x, y, displayed, 5, 2, displayFlags));
                        showAttributes = true;
                    }
                } else if ((static_cast<std::uint8_t>(flags >> 8) & 0x20u) != 0) {
                    AdvanceBattleCastState_439340(target);
                } else {
                    if ((flags & 0x100000u) != 0) {
                        static_cast<void>(MarkDeath(targetPayload->effect1054));
                        const std::uint32_t state = targetPayload->flagsFDC & ~0x20u;
                        targetPayload->effect1054 = nullptr;
                        targetPayload->flagsFDC = state;
                    }
                    AdvanceBattleCastState_439340(target);
                    const std::int32_t displayIndex = payload->effectRunningC9C;
                    const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                    if ((displayFlags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            cg::world::gBattleAllEntries59E980[payload->effects1C[displayIndex].position], 22459, 0, 0));
                    } else {
                        const std::int32_t y = target->y - 80;
                        const std::int32_t amount = payload->effects1C[displayIndex].position3;
                        const std::int32_t x = target->x;
                        static_cast<void>(NewBattleNumber_41F1A0(x, y, amount, 6, 2, displayFlags));
                        showAttributes = true;
                    }
                }
                if (showAttributes) {
                    if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 2u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
                }
            }
            const std::int32_t tailIndex = payload->effectRunningC9C;
            if ((static_cast<std::uint8_t>(payload->effects1C[tailIndex].flags) & 8u) == 0
                && payload->effects1C[tailIndex].position3 != 0) {
                payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
            }
        } while (AdvanceBattleEffect_439470(action) != 1);
        action->value54 = 8.0F;
        action->framePlayingRate = 100;
        payload->valueFF0 = 0;
        action->frameEvent = 0;
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// BKE01：旧FF0/1038蓄力及两侧HP零逐项复制；五BYTE与真实事件循环/旧FD0收尾。
// 原函数：0x0040E560。
void UpdateBattleAction110_40E560(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 3;
    action->value54 = 16.0F;
    action->framePlayingRate = 290;
    bool suppressPlayback = false;
    TurnBattleToward_406190(action, cg::world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
    const std::int32_t previous = payload->valueFF0;
    const std::int32_t count = previous + 1;
    payload->valueFF0 = count;
    if (previous >= 60 && payload->value1038 == 0) {
        action->framePlayingRate = 50;
        if (GetBattleDistance_406300(action, cg::world::gBattleAllEntries59E980[
                payload->effects1C[payload->effectRunningC9C].position]) >= 70) {
            MoveBattleByAngle_405EA0(action);
        } else {
            if (payload->battlePosition18 >= 0) {
                do {
                    YobiAction* const target = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    auto* const targetPayload = target->payload.battleActor;
                    TurnBattleToward_406190(target, action, 1, 0.0F);
                    targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
                        payload->effects1C[payload->effectRunningC9C].flags);
                    ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
                    const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
                    if ((static_cast<std::uint8_t>(flags) & 8u) != 0) {
                        target->phase = 31;
                    } else if ((flags & 0x80000u) != 0) {
                        if ((static_cast<std::uint8_t>(flags >> 8) & 0xE0u) == 0) {
                            const std::int32_t imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                            YobiAction* const effect = NewBattleRandomEffect_419FC0(target, imageNo, 0);
                            NewBattleRebirthEffects_4208F0(action, target);
                            effect->scaleX = 2.0F;
                            effect->scaleY = 2.0F;
                            static_cast<void>(audio::PlaySe_52C450(132, action->x));
                            std::optional<platform::LegacyCryptInt> temporary;
                            temporary.emplace(static_cast<std::int32_t>(0));
                            const std::int32_t copyIndex = payload->effectRunningC9C;
                            const std::uint32_t encoded = temporary->encoded;
                            YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                                payload->effects1C[copyIndex].position];
                            copyTarget->readiness.encoded = encoded;
                            copyTarget->readiness.mask = temporary->mask;
                            copyTarget->readiness.readCount = temporary->readCount;
                            temporary.reset();
                            if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 0x20u) != 0) {
                                targetPayload->flagsFDC |= 2u;
                                gYobiUpdateGateMode = 1;
                                targetPayload->aliveFF8 = 1;
                            }
                        }
                        if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags >> 8) & 0x20u) != 0) {
                            const std::int32_t imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                            YobiAction* const effect = NewBattleRandomEffect_419FC0(action, imageNo, 0);
                            NewBattleRebirthEffects_4208F0(action, target);
                            effect->scaleX = 2.0F;
                            effect->scaleY = 2.0F;
                            static_cast<void>(audio::PlaySe_52C450(132, action->x));
                            std::optional<platform::LegacyCryptInt> temporary;
                            temporary.emplace(static_cast<std::int32_t>(0));
                            const std::int32_t sourcePosition = payload->battlePosition18;
                            const std::uint32_t encoded = temporary->encoded;
                            YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[sourcePosition];
                            copyTarget->readiness.encoded = encoded;
                            copyTarget->readiness.mask = temporary->mask;
                            copyTarget->readiness.readCount = temporary->readCount;
                            temporary.reset();
                            if ((static_cast<std::uint8_t>(payload->effects1C[payload->effectRunningC9C].flags) & 0x20u) != 0) {
                                payload->flagsFDC |= 2u;
                                gYobiUpdateGateMode = 1;
                                payload->aliveFF8 = 1;
                            }
                            AdvanceBattleCastState_439340(action);
                        }
                    }
                    AdvanceBattleCastState_439340(target);
                    payload->valueFD0 = 14 * static_cast<std::int32_t>(
                        ((payload->effects1C[payload->effectRunningC9C].flags >> 19) & 1u) + 1u);
                } while (AdvanceBattleEffect_439470(action) != 1);
            }
            MoveBattleByAngle_405EA0(action);
            payload->valueFF0 = 0;
            payload->value1038 = 1;
            payload->flash1004 = 0;
            action->characterAction = 5;
            action->frameEvent = 0;
            for (;;) {
                const std::int32_t event = action->frameEvent;
                if (event >= 10000 && event < 20000) {
                    break;
                }
                static_cast<void>(PlayFrame_517DA0(action, 0));
            }
            action->frameEvent = 0;
        }
    } else {
        const std::int32_t state = payload->value1038;
        if (state == 0) {
            if (count < 60) {
                action->framePlayingRate -= 4 * count;
            }
        } else {
            const std::int32_t remaining = payload->valueFD0;
            payload->valueFD0 = remaining - 1;
            if (remaining != 0) {
                suppressPlayback = true;
                action->value54 = 2.0F;
                action->framePlayingRate = 100;
                MoveBattleByAngle_405EA0(action);
            }
        }
    }
    if (payload->value1038 != 0 && payload->valueFD0 == 0) {
        action->value54 = 8.0F;
        action->framePlayingRate = 100;
        action->characterAction = 3;
        payload->valueFF0 = 0;
        payload->value1038 = 0;
        payload->valueFD0 = 0;
        AdvanceBattleCastState_439340(action);
    } else if (!suppressPlayback) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
}

// BKE01：两次fresh BYTE FDC/40次散射，原角度与四边界后仍Move/Play S32。
// 原函数：0x0040E970。
std::int32_t UpdateBattleAction111_40E970(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    action->framePlayingRate = 100;
    const std::uint32_t flags = payload->flagsFDC;
    const std::int32_t count = payload->valueFCC + 1;
    payload->valueFCC = count;
    const std::int32_t duration = 14 * static_cast<std::int32_t>(((flags >> 1) & 1u) + 1u);
    if (count < duration) {
        action->value54 = -2.0F;
        if ((static_cast<std::uint8_t>(payload->flagsFDC) & 2u) != 0) {
            gYobiUpdateGateMode = 1;
            payload->aliveFF8 = 1;
        }
    } else {
        if (count == duration) {
            static_cast<void>(audio::PlaySe_52C450(207, action->x));
            if ((static_cast<std::uint8_t>(payload->flagsFDC) & 2u) != 0) {
                if (CountBattleActiveEntries_406360() <= 1) {
                    gYobiUpdateGateMode = 0;
                }
                const std::uint32_t state = payload->stateFE0;
                payload->aliveFF8 = 0;
                payload->stateFE0 = state | 2u;
            }
            for (std::int32_t remaining = 40; remaining != 0; --remaining) {
                static_cast<void>(NewBattleScatterEffect_41E510(action, 110004));
            }
            payload->valueFF0 = 0;
            payload->valueFD0 = 0;
            const double angle = action->angle50 + 180.0;
            action->value54 = 32.0F;
            action->angle50 = static_cast<float>(angle);
            if (angle >= 360.0) {
                action->angle50 = static_cast<float>(angle - 360.0);
            }
        }
        if (!(action->offset40 >= -200.0F) || action->offset40 > 840.0F
            || !(action->offset44 >= -200.0F) || action->offset44 > 680.0F) {
            AdvanceBattleCastState_439340(action);
            payload->valueFD0 = 0;
            gYobiScreenOffsetMaximumAmplitude = 6;
            gYobiScreenOffsetRising = 1;
        }
    }
    MoveBattleByAngle_405EA0(action);
    return PlayFrame_517DA0(action, 0);
}

// BKE01：worldY先载荷；两轴各自先借用父再数值，旧294/FF0及末fresh父标死保持。
// 原函数：0x0040EFE0。
void UpdateBattleAction113_40EFE0(YobiAction* const action)
{
    const float baseY = action->worldY;
    auto* const payload = action->payload.battleActor;
    action->offset44 = baseY;
    MoveBattleByAngle_405EA0(action);
    YobiAction* const horizontalOpponent = action->battleOpponent10;
    const float x = action->offset40;
    horizontalOpponent->offset40 = x;
    YobiAction* const verticalOpponent = action->battleOpponent10;
    const std::int32_t graphicOffsetY = action->graphicOffsetY;
    const float y = action->offset44;
    verticalOpponent->offset44 = static_cast<float>(
        static_cast<double>(y) - graphicOffsetY);
    action->worldY = action->offset44;
    const std::int32_t duration = action->extraState290.castDuration294;
    const std::int32_t delta = payload->valueFF0 - duration / 2;
    const std::int32_t square = delta * delta;
    action->offset44 = static_cast<float>(static_cast<double>(square) * action->worldX
        - action->field58 + action->offset44);
    const std::int32_t previous = payload->valueFF0;
    payload->valueFF0 = previous + 1;
    if (previous >= duration) {
        action->offset40 = static_cast<float>(payload->moveTargetXFE8);
        action->offset44 = static_cast<float>(payload->moveTargetYFEC);
        AdvanceBattleCastState_439340(action);
        static_cast<void>(MarkDeath(action->battleOpponent10));
        payload->valueFF0 = 0;
    }
}

// BCVI01：位置缓存按已证PE静态零初值；临时HP复制index/encoded先目标/逐项发布。
// 原函数：0x0040F0A0。
void UpdateBattleAction114_40F0A0(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    using world::gBattleEffectPairs5A8A40;
    using world::gBattleNamedEffects5ABCE0;
    auto* const payload = action->payload.battleActor;
    do {
        const std::int32_t cacheIndex = payload->effectRunningC9C;
        BattleEntryEffectRecord& cached = payload->effects1C[cacheIndex];
        if (cached.position > 0) {
            gBattleEffectPositions59E9D8[cacheIndex] = cached.position;
        } else {
            cached.position = gBattleEffectPositions59E9D8[cacheIndex];
        }
        const BattleEntryEffectRecord& initial = payload->effects1C[payload->effectRunningC9C];
        const std::int32_t position = initial.position;
        const std::uint32_t initialFlags = initial.flags;
        YobiAction* const target = cg::world::gBattleAllEntries59E980[position];
        auto* const targetPayload = target->payload.battleActor;
        std::int32_t imageNo;
        if ((initialFlags & 4u) != 0) {
            imageNo = 110000;
        } else if ((initialFlags & 0x400u) != 0) {
            imageNo = 110098;
        } else {
            imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
        }
        TurnBattleToward_406190(target, payload->castSkillLevel1044 >= 90
            ? action : cg::world::gBattleAllEntries59E980[payload->battlePosition18], 1, 0.0F);
        targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
            payload->effects1C[payload->effectRunningC9C].flags);
        ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
        if (payload->effectRunningC9C == 0 && payload->castSkillLevel1044 < 90) {
            std::int32_t namedImage = 110158;
            std::int32_t namedIndex = 0;
            while (std::strcmp(cg::world::gBattleNamedEffects5ABCE0[namedIndex].name.data(), action->nameC4.data()) != 0) {
                ++namedIndex;
                if (namedIndex >= 20) {
                    break;
                }
            }
            if (namedIndex < 20) {
                for (std::int32_t index = 0; kBattleNamedEffectImages57E174[index][0] != 0; ++index) {
                    if (kBattleNamedEffectImages57E174[index][0] == cg::world::gBattleNamedEffects5ABCE0[namedIndex].value) {
                        namedImage = kBattleNamedEffectImages57E174[index][1];
                        break;
                    }
                }
            }
            YobiAction* const effect = NewBattleRandomEffect_419FC0(action, namedImage, 0);
            const double step = 0.1F;
            effect->scaleX = static_cast<float>(
                static_cast<double>(payload->castSkillLevel1044 / 10) + effect->scaleX + step);
            effect->scaleY = static_cast<float>(
                static_cast<double>(payload->castSkillLevel1044 / 10) + effect->scaleY + step);
        }
        const BattleEntryEffectRecord& paired = payload->effects1C[payload->effectRunningC9C];
        const std::int32_t pairPosition = paired.position;
        if (cg::world::gBattleEffectPairs5A8A40[pairPosition][1] != 0) {
            const std::int32_t type = cg::world::gBattleEffectPairs5A8A40[pairPosition][0];
            if (type >= 1 && type <= 6) {
                static_cast<void>(NewBattleDebuffEffect_41F660(cg::world::gBattleAllEntries59E980[pairPosition], type + 110074));
            }
        } else {
            payload->flagsFDC &= 0xFFFFFC0Fu;
            AppendBattleCast_4392A0(action, cg::world::gBattleAllEntries59E980[paired.position], 142);
        }
        const std::int32_t hitIndex = payload->effectRunningC9C;
        const std::uint32_t hitFlags = payload->effects1C[hitIndex].flags;
        if ((hitFlags & 8u) != 0) {
            target->phase = 31;
        } else if ((hitFlags & 0x100u) != 0) {
            targetPayload->flash1004 = 1;
            static_cast<void>(audio::PlaySe_52C450(210, action->x));
            target->phase = 31;
        } else {
            if ((hitFlags & 0xE000u) == 0 && payload->effects1C[hitIndex].position3 != 0) {
                if ((hitFlags & 2u) != 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                }
                if (imageNo >= 0) {
                    static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                }
                NewBattleRebirthEffects_4208F0(cg::world::gBattleAllEntries59E980[payload->battlePosition18], target);
                static_cast<void>(audio::PlaySe_52C450(267, action->x));
                const BattleEntryEffectRecord& damage = payload->effects1C[payload->effectRunningC9C];
                const std::int32_t amount = damage.position3;
                (cg::world::gBattleAllEntries59E980[damage.position]->readiness -= static_cast<std::int32_t>(amount));
                YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                    payload->effects1C[payload->effectRunningC9C].position];
                if ((compareTarget->readiness < 0)) {
                    std::optional<platform::LegacyCryptInt> temporary;
                    temporary.emplace(static_cast<std::int32_t>(0));
                    const std::int32_t copyEffectIndex = payload->effectRunningC9C;
                    const std::uint32_t encoded = temporary->encoded;
                    YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[copyEffectIndex].position];
                    copyTarget->readiness.encoded = encoded;
                    copyTarget->readiness.mask = temporary->mask;
                    copyTarget->readiness.readCount = temporary->readCount;
                    temporary.reset();
                }
                const std::uint8_t effectFlags = *reinterpret_cast<const std::uint8_t*>(
                    &payload->effects1C[payload->effectRunningC9C].flags);
                if ((effectFlags & 0x20u) != 0) {
                    targetPayload->flagsFDC |= 2u;
                    gYobiUpdateGateMode = 1;
                    targetPayload->aliveFF8 = 1;
                }
            }
            const std::uint32_t flags = payload->effects1C[payload->effectRunningC9C].flags;
            bool showAttributes = false;
            if ((flags & 0x8000u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else if ((flags & 0x4000u) != 0) {
                AdvanceBattleCastState_439340(target);
                const std::int32_t healIndex = payload->effectRunningC9C;
                if ((payload->effects1C[healIndex].flags & 0x10000000u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[healIndex].position], 22459, 0, 0));
                } else {
                    const std::int32_t amount = payload->effects1C[healIndex].position3;
                    (cg::world::gBattleAllEntries59E980[payload->effects1C[healIndex].position]->readiness += static_cast<std::int32_t>(amount));
                    YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position];
                    if ((compareTarget->readiness > compareTarget->maxHp)) {
                        YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position];
                        copyTarget->readiness = copyTarget->maxHp;
                    }
                    const BattleEntryEffectRecord& current = payload->effects1C[payload->effectRunningC9C];
                    const std::uint32_t displayFlags = current.flags;
                    const std::int32_t y = target->y - 80;
                    const std::int32_t displayed = current.position3;
                    const std::int32_t x = target->x;
                    static_cast<void>(NewBattleNumber_41F1A0(x, y, displayed, 5, 2, displayFlags));
                    showAttributes = true;
                }
            } else if ((flags & 0x2000u) != 0) {
                AdvanceBattleCastState_439340(target);
            } else {
                if ((flags & 0x100000u) != 0) {
                    static_cast<void>(MarkDeath(targetPayload->effect1054));
                    const std::uint32_t state = targetPayload->flagsFDC & ~0x20u;
                    targetPayload->effect1054 = nullptr;
                    targetPayload->flagsFDC = state;
                }
                AdvanceBattleCastState_439340(target);
                const std::int32_t displayIndex = payload->effectRunningC9C;
                const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                if ((displayFlags & 0x10000000u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(
                        cg::world::gBattleAllEntries59E980[payload->effects1C[displayIndex].position], 22459, 0, 0));
                } else {
                    const std::int32_t y = target->y - 80;
                    const std::int32_t amount = payload->effects1C[displayIndex].position3;
                    const std::int32_t x = target->x;
                    static_cast<void>(NewBattleNumber_41F1A0(x, y, amount, 6, 2, displayFlags));
                    showAttributes = true;
                }
            }
            if (showAttributes) {
                const std::uint8_t attributeFlags = *reinterpret_cast<const std::uint8_t*>(
                    &payload->effects1C[payload->effectRunningC9C].flags);
                if ((attributeFlags & 2u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                }
                static_cast<void>(NewBattleAttributeEffects_4209C0(
                    cg::world::gBattleAllEntries59E980[payload->battlePosition18], target));
            }
        }
        const std::int32_t tailIndex = payload->effectRunningC9C;
        const std::uint8_t tailFlags =
            *reinterpret_cast<const std::uint8_t*>(&payload->effects1C[tailIndex].flags);
        if ((tailFlags & 8u) == 0
            && payload->effects1C[tailIndex].position3 != 0) {
            payload->valueFD0 = 14 * static_cast<std::int32_t>(((targetPayload->flagsFDC >> 1) & 1u) + 1u);
        }
    } while (AdvanceBattleEffect_439470(action) != 1);
    AdvanceBattleCastState_439340(action);
}

// BFW01：固定载荷/完整flags的CH门禁，工厂后重取目标隐藏；全路径phase162。
// 原函数：0x0040FB80。
void UpdateBattleAction115_40FB80(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = 5;
    static_cast<void>(PlayFrame_517DA0(action, 1));
    if (payload->value1024 == 0) {
        const std::uint32_t type = (payload->effects1C[payload->effectRunningC9C].flags & 0x1000u) != 0 ? 3u : 0u;
        static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = -32}, type, 0, 1));
        world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position]->hidden = 1;
        action->hidden = 1;
    }
    action->phase = 162;
}

// 原函数：0x0040FC10。
void UpdateBattleFactoryAction_40FC10(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    if (payload->value1024 == 0) {
        std::int32_t type = -1;
        switch (action->phase) {
        case 130: type = 13; break;
        case 116: type = 1; break;
        case 117: type = 2; break;
        case 133: type = 25; break;
        case 149: type = 16; break;
        case 150: type = 18; break;
        case 151: type = 21; break;
        case 152: type = 22; break;
        case 153: type = 23; break;
        case 119: type = 4; break;
        case 120: type = 5; break;
        case 121: type = 7; break;
        case 122: type = 8; break;
        case 123: type = 9; break;
        case 124: type = 10; break;
        case 125: type = 11; break;
        case 126: type = 12; break;
        case 134: type = 26; break;
        case 135: type = 27; break;
        case 136: type = 28; break;
        case 137: type = 29; break;
        case 131: type = 14; break;
        case 132: type = 15; break;
        case 146: type = 6; break;
        case 157: type = 37; break;
        case 158: type = 38; break;
        case 161: type = 39; break;
        case 217: type = 40; break;
        case 250: type = 41; break;
        case 252: type = 42; break;
        default: break;
        }
        if (type >= 0) {
            static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0},
                static_cast<std::uint32_t>(type), 0, 1));
        }
    }
    action->phase = 162;
}

// BFW01：完整FDC持样/条件播放，后取phase选7/8/9末参0，全路径推进。
// 原函数：0x0040FF90。
void UpdateBattleActions127To129_40FF90(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    if (payload->value1024 == 0) {
        const std::int32_t phase = action->phase;
        if (phase >= 127 && phase <= 129) {
            static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0},
                static_cast<std::uint32_t>(phase - 120), 0, 0));
        }
    }
    AdvanceBattleCastState_439340(action);
}

// BFW01：完整FDC持样/条件播放，后取1024创建type20，全路径phase162。
// 原函数：0x0040FF40。
void UpdateBattleAction138_40FF40(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    if (payload->value1024 == 0) {
        static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 20, 0, 1));
    }
    action->phase = 162;
}

// BFW01：事件/完成两次独立特效指针持样，98单次属性；405不同取样/四临时复制原序。
// 原函数：0x00410000。
void UpdateBattleAction139_410000(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 6;
    if (payload->value1024 == 0) {
        action->battleCastEffect10 = NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 34, 0, 1);
    }
    const std::int32_t event = action->frameEvent;
    if (event >= 10000 && event < 20000) {
        YobiAction* const eventEffect = action->battleCastEffect10;
        if (eventEffect != nullptr && payload->castSkill103C != 402) {
            static_cast<void>(MarkDeath(eventEffect));
            action->battleCastEffect10 = nullptr;
            const std::int32_t skill = payload->castSkill103C;
            bool moveSpecialTarget = false;
            if (skill == 98) {
                const std::int32_t attribute = payload->value1048;
                moveSpecialTarget = attribute == 12 || attribute == 13;
            }
            if (moveSpecialTarget) {
                MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[payload->targetPosition100C]);
            } else if (skill == 97) {
                gYobiScreenOffsetMaximumAmplitude = 6;
                gYobiScreenOffsetRising = 1;
                static_cast<void>(audio::PlaySe_52C450(5, 320));
                static_cast<void>(audio::PlaySe_52C450(262, 320));
            } else {
                if ((skill >= 50 && skill <= 53) || skill == 88) {
                    if (payload->effects1C[payload->effectRunningC9C].position != -1) {
                        static_cast<void>(audio::PlaySe_52C450(275, action->x));
                        std::int32_t imageNo = 0;
                        switch (payload->castSkill103C) {
                        case 50: imageNo = 110092; break;
                        case 51: imageNo = 110089; break;
                        case 52: imageNo = 110090; break;
                        case 53: imageNo = 110091; break;
                        case 88: imageNo = 110155; break;
                        default: break;
                        }
                        if (imageNo != 0) {
                            static_cast<void>(NewBattleFieldEffect_41FFE0(imageNo));
                        }
                        static_cast<void>(AdvanceBattleEffect_439470(action));
                    }
                } else if (skill == 405) {
                    static_cast<void>(audio::PlaySe_52C450(275, action->x));
                    static_cast<void>(NewBattleBackgroundFade_421570(
                        payload->effects1C[payload->effectRunningC9C].position4, 1, 1));
                    static_cast<void>(AdvanceBattleEffect_439470(action));
                    for (;;) {
                        const auto& levelEffect = payload->effects1C[payload->effectRunningC9C];
                        const std::int32_t levelPosition = levelEffect.position;
                        if (levelPosition == -1) {
                            break;
                        }
                        YobiAction* const levelTarget = cg::world::gBattleAllEntries59E980[levelPosition];
                        levelTarget->field1D0 = levelEffect.position3;
                        static_cast<void>(AdvanceBattleEffect_439470(action));
                        const auto& imageEffect = payload->effects1C[payload->effectRunningC9C];
                        const std::int32_t imagePosition = imageEffect.position;
                        const std::int32_t imageNo = imageEffect.position3;
                        cg::world::gBattleAllEntries59E980[imagePosition]->imageNo = imageNo;
                        const auto& riderEffect = payload->effects1C[payload->effectRunningC9C];
                        YobiAction* const riderTarget = cg::world::gBattleAllEntries59E980[riderEffect.position];
                        if (riderTarget->payload.battleActor->rider13C8 != nullptr) {
                            const std::int32_t riderImage = riderEffect.position4;
                            if (riderImage != 0) {
                                static_cast<void>(SetBattleRider_4222B0(
                                    riderTarget, riderTarget->imageNo, riderImage));
                            }
                        }
                        static_cast<void>(AdvanceBattleEffect_439470(action));
                        const auto& gearEffect = payload->effects1C[payload->effectRunningC9C];
                        const std::int32_t gearType = gearEffect.position3;
                        const std::int32_t gearPosition = gearEffect.position;
                        SetBattleGear_422BC0(cg::world::gBattleAllEntries59E980[gearPosition], gearType);
                        static_cast<void>(AdvanceBattleEffect_439470(action));
                        // 构造会消费共享随机数，每次之后按当前效果重新查对象，再复制并析构。
                        std::optional<platform::LegacyCryptInt> temporary;
                        temporary.emplace(static_cast<std::int32_t>(
                            payload->effects1C[payload->effectRunningC9C].position3));
                        const std::int32_t hpPosition =
                            payload->effects1C[payload->effectRunningC9C].position;
                        const std::uint32_t hpEncoded = temporary->encoded;
                        YobiAction* target = cg::world::gBattleAllEntries59E980[hpPosition];
                        target->readiness.encoded = hpEncoded;
                        target->readiness.mask = temporary->mask;
                        target->readiness.readCount = temporary->readCount;
                        temporary.reset();
                        temporary.emplace(static_cast<std::int32_t>(
                            payload->effects1C[payload->effectRunningC9C].position4));
                        const std::int32_t maxHpPosition =
                            payload->effects1C[payload->effectRunningC9C].position;
                        const std::uint32_t maxHpEncoded = temporary->encoded;
                        target = cg::world::gBattleAllEntries59E980[maxHpPosition];
                        target->maxHp.encoded = maxHpEncoded;
                        target->maxHp.mask = temporary->mask;
                        target->maxHp.readCount = temporary->readCount;
                        temporary.reset();
                        static_cast<void>(AdvanceBattleEffect_439470(action));
                        temporary.emplace(static_cast<std::int32_t>(
                            payload->effects1C[payload->effectRunningC9C].position3));
                        const std::int32_t fpPosition =
                            payload->effects1C[payload->effectRunningC9C].position;
                        const std::uint32_t fpEncoded = temporary->encoded;
                        target = cg::world::gBattleAllEntries59E980[fpPosition];
                        target->barValue1B0.encoded = fpEncoded;
                        target->barValue1B0.mask = temporary->mask;
                        target->barValue1B0.readCount = temporary->readCount;
                        temporary.reset();
                        temporary.emplace(static_cast<std::int32_t>(
                            payload->effects1C[payload->effectRunningC9C].position4));
                        const std::int32_t maxFpPosition =
                            payload->effects1C[payload->effectRunningC9C].position;
                        const std::uint32_t maxFpEncoded = temporary->encoded;
                        target = cg::world::gBattleAllEntries59E980[maxFpPosition];
                        target->barMaximum1C0.encoded = maxFpEncoded;
                        target->barMaximum1C0.mask = temporary->mask;
                        target->barMaximum1C0.readCount = temporary->readCount;
                        temporary.reset();
                        static_cast<void>(AdvanceBattleEffect_439470(action));
                    }
                } else if ((skill >= 19 && skill <= 30) || (skill >= 2068 && skill <= 2079)
                    || (skill >= 2055 && skill <= 2057) || skill == 31 || skill == 3037) {
                    std::int32_t type = -1;
                    if (skill == 2070) {
                        type = 44;
                    } else if (skill == 2076) {
                        type = 45;
                    } else if (skill == 2079) {
                        type = 46;
                    }
                    if (type >= 0) {
                        static_cast<void>(NewBattleEffect_41CD00(cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position],
                            0, {.offsetY = 0}, type, 0, 1));
                    }
                    for (;;) {
                        const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
                        if (position == -1) {
                            break;
                        }
                        static_cast<void>(SeedBattleRandom_4065A0(position + payload->battlePosition18));
                        static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 35, 0, 0));
                        static_cast<void>(AdvanceBattleEffect_439470(action));
                    }
                } else {
                    for (;;) {
                        const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
                        if (position == -1) {
                            break;
                        }
                        static_cast<void>(SeedBattleRandom_4065A0(position + payload->battlePosition18));
                        MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[
                            payload->effects1C[payload->effectRunningC9C].position]);
                        static_cast<void>(AdvanceBattleEffect_439470(action));
                    }
                }
                static_cast<void>(AdvanceBattleEffect_439470(action));
            }
        }
        action->frameEvent = 0;
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        YobiAction* const completedEffect = action->battleCastEffect10;
        if (completedEffect != nullptr) {
            static_cast<void>(MarkDeath(completedEffect));
            action->battleCastEffect10 = nullptr;
            do {
                if (payload->castSkill103C == 402) {
                    MoveBattleCastState_439380(cg::world::gBattleAllEntries59E980[
                        payload->effects1C[payload->effectRunningC9C].position]);
                }
            } while (AdvanceBattleEffect_439470(action) != 1);
        }
        payload->value1024 = 0;
        AdvanceBattleCastState_439340(action);
    }
    if (action->battleCastEffect10 != nullptr) {
        std::int32_t type = -1;
        switch (payload->castSkill103C) {
        case 19: case 23: case 27: case 2068: case 2069: case 2070:
            type = 0;
            break;
        case 20: case 24: case 28: case 2071: case 2072: case 2073:
            type = 1;
            break;
        case 21: case 25: case 29: case 2074: case 2075: case 2076:
            type = 2;
            break;
        case 22: case 26: case 30: case 2077: case 2078: case 2079:
            type = 3;
            break;
        case 31: case 3037:
            type = 4;
            break;
        default:
            break;
        }
        if (type >= 0) {
            static_cast<void>(DrawBattleTiledEffect_41E890(type));
        }
        const std::int32_t currentSkill = payload->castSkill103C;
        if (currentSkill == 86 || currentSkill == 2055 || currentSkill == 2056 || currentSkill == 2057) {
            static_cast<void>(DrawBattleTiledEffect_41E890(5));
        }
    }
}

// BFL01：入口技能持样，特效末参0或先推进61–63目标，最后推进源。
// 原函数：0x00410730。
void UpdateBattleAction140_410730(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t skill = payload->castSkill103C;
    if ((skill >= 19 && skill <= 30) || (skill >= 2068 && skill <= 2079)
        || (skill >= 2055 && skill <= 2057) || skill == 31 || skill == 3037) {
        static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 35, 0, 0));
    } else if (skill >= 61 && skill <= 63) {
        AdvanceBattleCastState_439340(world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position]);
    }
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00410E90。
void UpdateBattleAction141_410E90(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    bool create = true;
    std::int32_t imageNo;
    if (payload->value13D8 == 3) {
        const std::int32_t type = payload->value13DC;
        if (type == 19 || type == 20) {
            imageNo = type + 111173;
        } else {
            create = false;
        }
    } else {
        const std::int32_t skill = payload->castSkill103C;
        if (skill == 98) {
            imageNo = payload->value1048 + 110075;
        } else if (skill == 2002) {
            imageNo = 110075;
        } else {
            const std::int32_t type = payload->value13DC;
            if (type >= 1 && type <= 6) {
                imageNo = type + 110074;
            } else {
                create = false;
            }
        }
    }
    if (create) {
        static_cast<void>(NewBattleDebuffEffect_41F660(action, imageNo));
    }
    const std::uint8_t flags =
        *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
    if ((flags & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    AdvanceBattleCastState_439340(action);
}

// BFL01：无空门禁标死1054，fresh完整FDC取样后先清槽再发布掩码值，播放/推进。
// 原函数：0x00410F70。
void UpdateBattleAction142_410F70(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    static_cast<void>(MarkDeath(payload->effect1054));
    const std::uint32_t flags = payload->flagsFDC & 0xFFFFFC0Fu;
    payload->effect1054 = nullptr;
    payload->flagsFDC = flags;
    static_cast<void>(PlayFrame_517DA0(action, 0));
    AdvanceBattleCastState_439340(action);
    // G03：原状态收尾完成后重取载荷，发布当前减益类型并清理变化标记。
    auto* const currentPayload = action->payload.battleActor;
    currentPayload->value13DC = currentPayload->debuffType13D0;
    currentPayload->value13D8 = 0;
}

// 原函数：0x00410FC0。
void UpdateBattleAction143_410FC0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    const std::int32_t use = payload->value13D8;
    bool create = true;
    std::int32_t imageNo;
    if (use == 1) {
        const std::int32_t type = payload->value13DC;
        if (type >= 1 && type <= 6) {
            imageNo = type + 103017;
        } else if (type == 7) {
            imageNo = 103033;
        } else {
            create = false;
        }
    } else if (use == 3) {
        switch (payload->value13DC) {
        case 0: imageNo = 111100; break;
        case 4: case 30: case 31: case 32: imageNo = 111099; break;
        case 5: case 16: case 17: case 18: imageNo = 111098; break;
        case 7: imageNo = kBattleStatusImages54D4C0[payload->value13E0]; break;
        case 9: imageNo = 111152; break;
        case 11: imageNo = 98254; break;
        case 12: imageNo = 98252; break;
        case 13: imageNo = 98257; break;
        case 14: imageNo = 98255; break;
        case 15: imageNo = 98258; break;
        case 33: imageNo = 98273; break;
        case 34: imageNo = 98261; break;
        case 35: case 45: imageNo = 98259; break;
        case 36: imageNo = 98260; break;
        case 37: imageNo = 98264; break;
        case 38: imageNo = 98266; break;
        case 39: imageNo = 98272; break;
        case 40: imageNo = 98263; break;
        case 41: imageNo = 98267; break;
        case 42: imageNo = 98269; break;
        case 43: imageNo = 98270; break;
        case 44: imageNo = 98271; break;
        case 46: imageNo = 98265; break;
        case 47: imageNo = 98262; break;
        default: create = false; break;
        }
    } else {
        create = false;
    }
    if (create) {
        static_cast<void>(NewBattleStatusEffect_41FE00(action, imageNo, 1));
    }
    const std::uint8_t flags =
        *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
    if ((flags & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    AdvanceBattleCastState_439340(action);
}

// BFL01：无空门禁标死1068，fresh完整FDC取样后先清槽再发布掩码值，播放/推进。
// 原函数：0x00411240。
void UpdateBattleAction144_411240(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    static_cast<void>(MarkDeath(payload->effect1068));
    const std::uint32_t flags = payload->flagsFDC & 0xFFF03FFFu;
    payload->effect1068 = nullptr;
    payload->flagsFDC = flags;
    static_cast<void>(PlayFrame_517DA0(action, 0));
    AdvanceBattleCastState_439340(action);
}

// BFL01：一次技能选择四图，创建返回弃置；其它技能亦共同推进，无播放。
// 原函数：0x00411290。
void UpdateBattleAction145_411290(YobiAction* const action)
{
    const std::int32_t skill = action->payload.battleActor->castSkill103C;
    std::int32_t imageNo = -1;
    switch (skill) {
    case 50: imageNo = 110092; break;
    case 51: imageNo = 110089; break;
    case 52: imageNo = 110090; break;
    case 53: imageNo = 110091; break;
    default: break;
    }
    if (imageNo >= 0) {
        static_cast<void>(NewBattleFieldEffect_41FFE0(imageNo));
    }
    AdvanceBattleCastState_439340(action);
}

// BAS01：完整FDC的AH800开关；创建失败仍置位，标死后fresh取旗位、先清105C再发布。
// 原函数：0x00411320。
void UpdateBattleAction147_411320(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x800u) != 0) {
        static_cast<void>(MarkDeath(payload->effect105C));
        const std::uint32_t flags = payload->flagsFDC & ~0x800u;
        payload->effect105C = nullptr;
        payload->flagsFDC = flags;
    } else {
        static_cast<void>(NewBattleReverseEffect_41F860(action, 110152));
        payload->flagsFDC |= 0x800u;
    }
    AdvanceBattleCastState_439340(action);
}

// BAS01：完整FDC持样/AL60播放门禁，后取1024创建type33，全路径phase162。
// 原函数：0x0040FEF0。
void UpdateBattleAction148_40FEF0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    if (payload->value1024 == 0) {
        static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 33, 0, 1));
    }
    action->phase = 162;
}

// 原函数：0x0040F790。
void UpdateBattleAction154_40F790(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    if (payload->effect1060 == nullptr) {
        payload->effect1060 = NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 24, 0, 0);
        static_cast<void>(audio::PlaySe_52C450(212, action->x));
    }
    if (payload->effect1060->phase == 1) {
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x0040F800。
void UpdateBattleAction155_40F800(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x60u) == 0) {
        while (PlayFrame_517DA0(action, 1) == 0) {
        }
    }
    const double step = 0.01F;
    const double tenth = 0.1F;
    if (payload->value1024 == 0) {
        YobiAction* const effect = NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 22, 0, 1);
        const std::int32_t soundX = action->x;
        action->battleCastEffect10 = effect;
        static_cast<void>(audio::PlaySe_52C450(213, soundX));
        static_cast<void>(SeedBattleRandom_4065A0(payload->battlePosition18
            + payload->effects1C[payload->effectRunningC9C].position));
        action->battleTurnStepB4 = static_cast<float>(
            platform::RandomRange_46FAF0(1, 9) * tenth);
        static_cast<void>(SeedBattleRandom_4065A0(payload->battlePosition18
            + payload->effects1C[payload->effectRunningC9C].position));
        payload->attackModeFF4 = platform::RandomRange_46FAF0(5, 9);
        action->characterAction = 8;
        const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
        const std::uint32_t flags = action->flags;
        action->flags = flags | (hasDevice ? 0x100000u : 0x8000u);
        payload->valueFF0 = 0;
    }
    if (action->battleTurnStepB4 < action->scaleX) {
        const double scaleX = action->scaleX - step;
        action->scaleX = static_cast<float>(scaleX);
        action->scaleY = static_cast<float>(action->scaleY - step);
        if (scaleX <= action->battleTurnStepB4) {
            action->scaleX = action->battleTurnStepB4;
            action->scaleY = action->battleTurnStepB4;
            const std::int32_t count = payload->valueFF0 + 1;
            const std::int32_t total = payload->attackModeFF4;
            payload->valueFF0 = count;
            if (count < total) {
                if (count == total - 1) {
                    if (payload->value1030 == 1) {
                        action->battleTurnStepB4 = 0.0F;
                    } else {
                        action->battleTurnStepB4 =
                            (action->payload.battleActor->flagsFDC & 0x1000u) != 0
                            ? 1.3F : 1.0F;
                        static_cast<void>(audio::PlaySe_52C450(213, action->x));
                    }
                } else {
                    const std::int32_t minimum = static_cast<std::int32_t>(
                        action->battleTurnStepB4 * 10.0);
                    const std::int32_t target = platform::RandomRange_46FAF0(minimum, 9);
                    const std::int32_t soundX = action->x;
                    action->battleTurnStepB4 = static_cast<float>(target * tenth);
                    static_cast<void>(audio::PlaySe_52C450(213, soundX));
                }
            }
        }
    } else {
        const double scaleX = action->scaleX + step;
        action->scaleX = static_cast<float>(scaleX);
        action->scaleY = static_cast<float>(action->scaleY + step);
        if (scaleX >= action->battleTurnStepB4) {
            action->scaleX = action->battleTurnStepB4;
            action->scaleY = action->battleTurnStepB4;
            const std::int32_t count = payload->valueFF0 + 1;
            const std::int32_t total = payload->attackModeFF4;
            payload->valueFF0 = count;
            if (count < total) {
                if (count == total - 1) {
                    if (payload->value1030 == 1) {
                        action->battleTurnStepB4 = 0.0F;
                        static_cast<void>(audio::PlaySe_52C450(213, action->x));
                    } else {
                        action->battleTurnStepB4 =
                            (action->payload.battleActor->flagsFDC & 0x1000u) != 0
                            ? 1.3F : 1.0F;
                    }
                } else {
                    const std::int32_t maximum = static_cast<std::int32_t>(
                        action->battleTurnStepB4 * 10.0);
                    const std::int32_t target = platform::RandomRange_46FAF0(1, maximum);
                    const std::int32_t soundX = action->x;
                    action->battleTurnStepB4 = static_cast<float>(target * tenth);
                    static_cast<void>(audio::PlaySe_52C450(213, soundX));
                }
            }
        }
    }
    const std::int32_t total = payload->attackModeFF4;
    if (payload->valueFF0 >= total + 1) {
        YobiAction* const castEffect = action->battleCastEffect10;
        if (castEffect != nullptr) {
            static_cast<void>(MarkDeath(castEffect));
            action->battleCastEffect10 = nullptr;
            payload->value1024 = 0;
        }
        payload->effect1060->phase = 2;
        payload->valueFF0 = 0;
        const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
        const std::uint32_t flags = action->flags & ~(hasDevice ? 0x100000u : 0x8000u);
        const std::int32_t soundX = action->x;
        action->flags = flags;
        static_cast<void>(audio::PlaySe_52C450(212, soundX));
        action->characterAction = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x0040FB30。
void UpdateBattleAction156_40FB30(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    YobiAction* const effect = payload->effect1060;
    if (effect->phase == 3) {
        static_cast<void>(MarkDeath(effect));
        payload->effect1060 = nullptr;
        AdvanceBattleCastState_439340(action);
    }
}

// BCPY01：每效果独立复制载荷/七动作队列，四效果值按4-flags-3-position读齐。
// 原函数：0x0040DBE0。
void UpdateBattleAction103_40DBE0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* const caster = payload->caster1070;
    do {
        YobiAction* const copy = NewBattleCastCopy_4205F0(action);
        AppendBattleCast_4392A0(caster, copy, 25);
        AppendBattleCast_4392A0(caster, copy, 77);
        AppendBattleCast_4392A0(caster, copy, 91);
        AppendBattleCast_4392A0(caster, copy, 9);
        AppendBattleCast_4392A0(caster, copy, 92);
        AppendBattleCast_4392A0(caster, copy, 14);
        AppendBattleCast_4392A0(caster, copy, 107);
        const BattleEntryEffectRecord& effect = payload->effects1C[payload->effectRunningC9C];
        const std::int32_t value4 = effect.position4;
        const std::uint32_t flags = effect.flags;
        const std::int32_t value3 = effect.position3;
        const std::int32_t position = effect.position;
        AppendBattleEffect_439400(copy, {position, flags, value3, value4});
        AdvanceBattleCastState_439340(copy);
    } while (AdvanceBattleEffect_439470(action) != 1);
    AdvanceBattleCastState_439340(action);
}

// BKE01：独立复制体25/110/107及四值4-flags-3-position读取，设备后完整flags与双推进保持。
// 原函数：0x0040E4B0。
void UpdateBattleAction109_40E4B0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* const caster = payload->caster1070;
    YobiAction* const copy = NewBattleCastCopy_4205F0(action);
    auto* const copyPayload = copy->payload.battleActor;
    copy->imageNo = 101031;
    const bool hasDevice = render::HasLegacy3DDevice_44B610() != 0;
    copy->flags |= hasDevice ? 0x100000u : 0x8000u;
    copyPayload->flash1004 = 1;
    copyPayload->battlePosition18 = -1;
    AppendBattleCast_4392A0(caster, copy, 25);
    AppendBattleCast_4392A0(caster, copy, 110);
    AppendBattleCast_4392A0(caster, copy, 107);
    const std::int32_t effectIndex = payload->effectRunningC9C;
    const BattleEntryEffectRecord& effect = payload->effects1C[effectIndex];
    const std::int32_t position4 = effect.position4;
    const std::uint32_t flags = effect.flags;
    const std::int32_t position3 = effect.position3;
    const std::int32_t position = effect.position;
    AppendBattleEffect_439400(copy, BattleEntryEffectRecord{position, flags, position3, position4});
    AdvanceBattleCastState_439340(copy);
    AdvanceBattleCastState_439340(action);
}

// BKE01：双复制体/第三列表选图、一次事件/三Y-X及运动读序、两Append不同四值顺序。
// 原函数：0x0040EB00。
void UpdateBattleAction112_40EB00(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    using world::gBattleNamedEffects5ABCE0;
    auto* const payload = action->payload.battleActor;
    YobiAction* const caster = payload->caster1070;
    if (PlayFrame_517DA0(action, 1) == 1) {
        AdvanceBattleCastState_439340(action);
    }
    const std::int32_t image = action->imageNo;
    action->characterAction = (image >= 100000 && image <= 100999)
            || (image >= 105000 && image <= 106800)
            || (image >= 104400 && image <= 104520)
            || (image >= 120100 && image <= 120147) ? 7 : 5;
    if (payload->castSkillLevel1044 < 90) {
        TurnBattleToward_406190(action,
            cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
    }
    const std::int32_t event = action->frameEvent;
    if (event >= 20000 || event < 10000) {
        return;
    }
    YobiAction* const first = NewBattleCastCopy_4205F0(action);
    first->imageNo = 98003;
    first->characterAction = 0;
    first->direction = 0;
    static_cast<void>(PlayFrame_517DA0(first, 0));
    YobiAction* const copy = NewBattleCastCopy_4205F0(action);
    auto* const copyPayload = copy->payload.battleActor;
    copy->imageNo = 26220;
    std::int32_t namedIndex = 0;
    while (std::strcmp(cg::world::gBattleNamedEffects5ABCE0[namedIndex].name.data(), action->nameC4.data()) != 0) {
        ++namedIndex;
    }
    for (std::int32_t index = 0; kBattleNamedEffectImages57E174[index][0] != 0; ++index) {
        if (kBattleNamedEffectImages57E174[index][0] == cg::world::gBattleNamedEffects5ABCE0[namedIndex].value) {
            copy->imageNo = kBattleNamedEffectImages57E174[index][2];
            break;
        }
    }
    std::strcpy(copy->nameC4.data(), action->nameC4.data());
    copy->characterAction = 0;
    copy->direction = 0;
    static_cast<void>(PlayFrame_517DA0(copy, 0));
    copy->battleAttachmentParent10 = first;
    copyPayload->castSkillLevel1044 = payload->castSkillLevel1044;
    if (payload->castSkillLevel1044 == 90) {
        if (payload->effects1C[payload->effectRunningC9C].position > 9) {
            copyPayload->moveTargetXFE8 = 188;
            copyPayload->moveTargetYFEC = 203;
        } else {
            copyPayload->moveTargetXFE8 = 449;
            copyPayload->moveTargetYFEC = 336;
        }
        const float targetY = static_cast<float>(copyPayload->moveTargetYFEC);
        const float targetX = static_cast<float>(copyPayload->moveTargetXFE8);
        TurnBattleTowardPosition_4060D0(action, targetX, targetY, 1, 0.0F);
    } else {
        copyPayload->moveTargetXFE8 = static_cast<std::int32_t>(
            cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]->offset40);
        copyPayload->moveTargetYFEC = static_cast<std::int32_t>(
            cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]->offset44);
    }
    copy->offset44 = static_cast<float>(static_cast<double>(action->graphicOffsetY / 2) + copy->offset44);
    const std::int32_t copyX = static_cast<std::int32_t>(copy->offset40);
    const float copyY = copy->offset44;
    copy->x = copyX;
    copy->y = static_cast<std::int32_t>(copyY);
    const float copyTargetY = static_cast<float>(copyPayload->moveTargetYFEC);
    const float copyTargetX = static_cast<float>(copyPayload->moveTargetXFE8);
    TurnBattleTowardPosition_4060D0(copy, copyTargetX, copyTargetY, 1, 0.0F);
    const float firstTargetY = static_cast<float>(copyPayload->moveTargetYFEC);
    const float firstTargetX = static_cast<float>(copyPayload->moveTargetXFE8);
    TurnBattleTowardPosition_4060D0(first, firstTargetX, firstTargetY, 1, 0.0F);
    const std::int32_t screenX = copy->x;
    const std::int32_t duration = 60;
    copy->extraState290.castDuration294 = duration;
    const std::int32_t horizontalDistance = std::abs(copyPayload->moveTargetXFE8 - screenX);
    const float cosineAngle = copy->angle50;
    constexpr float kCastStepScale = 1.0F / 60.0F; // 60帧水平位移的每帧比例。
    const float xStep = static_cast<float>(static_cast<double>(horizontalDistance)
        * kCastStepScale);
    const double cosine = world::Sub_46F950(cosineAngle);
    const std::int32_t screenY = copy->y;
    copy->value54 = static_cast<float>(std::abs(cosine) * xStep);
    const std::int32_t verticalDistance = std::abs(copyPayload->moveTargetYFEC - screenY);
    const float sineAngle = copy->angle50;
    std::int32_t currentDuration = copy->extraState290.castDuration294;
    const float yStep = static_cast<float>(static_cast<double>(verticalDistance) / currentDuration);
    const double sine = world::Sub_46F930(sineAngle);
    currentDuration = copy->extraState290.castDuration294;
    const float savedY = copy->offset44;
    copy->field58 = 100;
    const double speed = std::abs(sine) * yStep + copy->value54;
    copy->worldY = savedY;
    copy->value54 = static_cast<float>(speed);
    const std::int32_t halfDuration = currentDuration / 2;
    copy->worldX = static_cast<float>(100.0 / (halfDuration * halfDuration));
    AppendBattleCast_4392A0(caster, copy, 113);
    if (payload->castSkillLevel1044 == 90) {
        AppendBattleCast_4392A0(caster, copy, 85);
        AppendBattleCast_4392A0(caster, copy, 36);
    }
    AppendBattleCast_4392A0(caster, copy, 114);
    AppendBattleCast_4392A0(caster, copy, 107);
    if (payload->castSkillLevel1044 < 90) {
        const std::int32_t effectIndex = payload->effectRunningC9C;
        const BattleEntryEffectRecord& effect = payload->effects1C[effectIndex];
        const std::uint32_t flags = effect.flags;
        const std::int32_t position4 = effect.position4;
        const std::int32_t position3 = effect.position3;
        const std::int32_t position = effect.position;
        AppendBattleEffect_439400(copy, BattleEntryEffectRecord{position, flags, position3, position4});
    } else {
        do {
            const std::int32_t effectIndex = payload->effectRunningC9C;
            const BattleEntryEffectRecord& effect = payload->effects1C[effectIndex];
            const std::int32_t position4 = effect.position4;
            const std::uint32_t flags = effect.flags;
            const std::int32_t position3 = effect.position3;
            const std::int32_t position = effect.position;
            AppendBattleEffect_439400(copy, BattleEntryEffectRecord{position, flags, position3, position4});
        } while (AdvanceBattleEffect_439470(action) != 1);
    }
    copyPayload->effectRunningC9C = 0;
    AdvanceBattleCastState_439340(copy);
    action->frameEvent = 0;
    if (payload->effects1C[payload->effectRunningC9C + 1].position >= 0 && payload->castSkillLevel1044 < 90) {
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x00412870。
void SetBattleAction208Target_412870(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* const target = world::gBattleAllEntries59E980[
        payload->effects1C[payload->effectRunningC9C].position];
    const double boundary = target->offset40 * -0.75 + 480.0;
    const bool below = !(target->offset44 >= boundary);
    const std::int32_t x = static_cast<std::int32_t>(target->offset40);
    if (below) {
        payload->moveTargetXFE8 = x + 44;
        payload->moveTargetYFEC = static_cast<std::int32_t>(target->offset44) + 40;
    } else {
        payload->moveTargetXFE8 = x - 44;
        payload->moveTargetYFEC = static_cast<std::int32_t>(target->offset44) - 30;
    }
    action->phase = 6;
}

// 原函数：0x00412900。
std::int32_t UpdateBattleAction209_412900(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 8;
    if ((payload->effectFlagsFE4 & 0xC04) != 0) {
        action->characterAction = 9;
    }
    action->framePlayingRate = 50;
    const std::int32_t count = payload->valueFCC + 1;
    payload->valueFCC = count;
    const std::uint32_t initialFlags = payload->flagsFDC;
    const std::int32_t initialBit = static_cast<std::int32_t>((initialFlags >> 1) & 1u);
    if (count < 14 * (initialBit + 1)) {
        const double angle = action->angle50 + 180.0;
        action->value54 = 5.0F;
        action->angle50 = static_cast<float>(angle);
        if (angle >= 360.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        }
        const std::uint8_t movingFlags =
            *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
        if ((movingFlags & 2u) != 0) {
            gYobiUpdateGateMode = 1;
            payload->aliveFF8 = 1;
        }
        MoveBattleByAngle_405EA0(action);
        return PlayFrame_517DA0(action, 1);
    }
    if (count < 14 * initialBit + 34) {
        const std::uint8_t directionFlags =
            *reinterpret_cast<const std::uint8_t*>(&payload->effectFlagsFE4);
        if ((directionFlags & 2u) != 0) {
            const std::int32_t direction = action->direction + 1;
            action->direction = direction;
            if (direction >= 8) {
                action->direction = direction - 8;
            }
        }
        const std::uint32_t movementThresholdFlags = payload->flagsFDC;
        const std::int32_t movementThresholdCount = payload->valueFCC;
        if (movementThresholdCount == 14 * (static_cast<std::int32_t>((movementThresholdFlags >> 1) & 1u) + 1)) {
            action->value54 = 5.0F;
            const std::uint8_t thresholdFlags =
                *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
            if ((thresholdFlags & 2u) != 0) {
                if (CountBattleActiveEntries_406360() <= 1) {
                    gYobiUpdateGateMode = 0;
                }
                payload->aliveFF8 = 0;
            }
            payload->valueFF0 = 0;
        }
        if (payload->valueFF0 == 0) {
            static_cast<void>(NewBattlePositionEffect_41E680(action, 110006));
        }
        const std::int32_t previous = payload->valueFF0;
        payload->valueFF0 = previous + 1;
        if (previous >= 2) {
            payload->valueFF0 = 0;
        }
        const double speed = static_cast<double>(action->value54) - 0.3F;
        action->value54 = static_cast<float>(speed);
        if (!(speed >= 0.0)) {
            action->value54 = 0.0F;
        }
        MoveBattleByAngle_405EA0(action);
        const std::uint32_t impactThresholdFlags = payload->flagsFDC;
        const std::int32_t impactThresholdCount = payload->valueFCC;
        if (impactThresholdCount == 14 * static_cast<std::int32_t>((impactThresholdFlags >> 1) & 1u) + 24) {
            YobiAction* const sourceEntry = cg::world::gBattleAllEntries59E980[payload->battlePosition18];
            // 原指令在覆盖目标+10前保存此指针，源与目标别名时也不重取。
            YobiAction* const attacker = sourceEntry->battleOpponent10;
            YobiAction* const target = cg::world::gBattleAllEntries59E980[
                payload->effects1C[payload->effectRunningC9C].position];
            auto* const targetPayload = target->payload.battleActor;
            target->battleOpponent10 = sourceEntry;
            targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
                payload->effects1C[payload->effectRunningC9C].flags);
            targetPayload->valueFCC = 0;
            targetPayload->value1024 = 0;
            ApplyBattleEffectFlags_406390(payload->battlePosition18, action);
            const std::int32_t effectIndex = payload->effectRunningC9C;
            const std::uint32_t flags = payload->effects1C[effectIndex].flags;
            if ((flags & 8u) != 0) {
                target->phase = 31;
            } else if ((flags & 0x100u) != 0) {
                if (targetPayload->flash1004 == 0) {
                    static_cast<void>(audio::PlaySe_52C450(210, action->x));
                    targetPayload->flash1004 = 1;
                    target->phase = 31;
                }
            } else {
                if ((flags & 0xC000u) == 0 && payload->effects1C[effectIndex].position3 != 0) {
                    if ((flags & 0x2000u) == 0 && (flags & 0x100000u) != 0) {
                        static_cast<void>(MarkDeath(targetPayload->effect1054));
                        const std::uint32_t targetFlags = targetPayload->flagsFDC;
                        targetPayload->effect1054 = nullptr;
                        targetPayload->flagsFDC = targetFlags & ~0x20u;
                    }
                    const std::uint8_t randomEffectFlags = *reinterpret_cast<const std::uint8_t*>(
                        &payload->effects1C[payload->effectRunningC9C].flags);
                    if ((randomEffectFlags & 2u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                    }
                    const std::int32_t imageEffectIndex = payload->effectRunningC9C;
                    const std::uint32_t imageFlags = payload->effects1C[imageEffectIndex].flags;
                    if ((imageFlags & 4u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110000, 0));
                    } else if ((imageFlags & 0x400u) != 0) {
                        const std::int32_t technique = FindBattleAppendEffectTechnique_43BEA0(
                            8, payload->effects1C[imageEffectIndex].position);
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, technique < 12 ? 110098 : 111048, 0));
                    } else {
                        const auto* const attackerPayload = static_cast<const BattleActorPayload*>(attacker->payload.battleActor);
                        std::int32_t imageNo;
                        std::int32_t direction = 0;
                        if (attackerPayload->castSkill103C == 260) {
                            const std::int32_t level = attackerPayload->castSkillLevel1044;
                            if (level >= 12) {
                                imageNo = 111054;
                                direction = 1;
                            } else if (level >= 9) {
                                imageNo = 110473;
                                direction = 1;
                            } else {
                                imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                            }
                        } else {
                            imageNo = platform::RandomRange_46FAF0(0, 2) + 110001;
                        }
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, direction));
                    }
                    NewBattleRebirthEffects_4208F0(attacker, target);
                    const std::uint8_t rebirthFlags = *reinterpret_cast<const std::uint8_t*>(
                        &payload->effects1C[payload->effectRunningC9C].flags);
                    if ((rebirthFlags & 0x20u) != 0) {
                        targetPayload->flagsFDC |= 2u;
                        gYobiUpdateGateMode = 1;
                        targetPayload->aliveFF8 = 1;
                    }
                }
                const std::int32_t damageIndex = payload->effectRunningC9C;
                const std::uint32_t damageFlags = payload->effects1C[damageIndex].flags;
                if ((damageFlags & 0xE000u) == 0) {
                    if ((damageFlags & 0x10000000u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(
                            cg::world::gBattleAllEntries59E980[payload->effects1C[damageIndex].position], 22459, 0, 0));
                    } else {
                        const BattleEntryEffectRecord& effect = payload->effects1C[damageIndex];
                        const std::int32_t amount = effect.position3;
                        if (amount == 0) {
                            static_cast<void>(NewBattleFloatingIcon_41F4C0(
                                cg::world::gBattleAllEntries59E980[effect.position], 22331, 0, 0));
                        } else {
                            const std::int32_t skill = payload->castSkill103C;
                            if (skill != 84 && skill != 85 && skill != 87) {
                                (cg::world::gBattleAllEntries59E980[effect.position]->readiness -= static_cast<std::int32_t>(amount));
                                YobiAction* const compareTarget = cg::world::gBattleAllEntries59E980[
                                    payload->effects1C[payload->effectRunningC9C].position];
                                if ((compareTarget->readiness < 0)) {
                                    std::optional<platform::LegacyCryptInt> temporary;
                                    temporary.emplace(static_cast<std::int32_t>(0));
                                    const std::int32_t copyIndex = payload->effectRunningC9C;
                                    const std::uint32_t encoded = temporary->encoded;
                                    const std::int32_t copyPosition = payload->effects1C[copyIndex].position;
                                    YobiAction* const copyTarget = cg::world::gBattleAllEntries59E980[copyPosition];
                                    copyTarget->readiness.encoded = encoded;
                                    copyTarget->readiness.mask = temporary->mask;
                                    copyTarget->readiness.readCount = temporary->readCount;
                                    temporary.reset();
                                }
                                const std::int32_t displayIndex = payload->effectRunningC9C;
                                const std::uint32_t displayFlags = payload->effects1C[displayIndex].flags;
                                const std::int32_t y = target->y;
                                const std::int32_t displayed = payload->effects1C[displayIndex].position3;
                                static_cast<void>(NewBattleNumber_41F1A0(target->x, y - 80, displayed, 6, 2, displayFlags));
                                const std::uint8_t attributeIconFlags = *reinterpret_cast<const std::uint8_t*>(
                                    &payload->effects1C[payload->effectRunningC9C].flags);
                                if ((attributeIconFlags & 2u) != 0) {
                                    static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                                }
                                static_cast<void>(NewBattleAttributeEffects_4209C0(attacker, target));
                            }
                        }
                    }
                }
                AdvanceBattleCastState_439340(target);
            }
            TurnBattleToward_406190(target, action, 1, 0.0F);
            static_cast<void>(AdvanceBattleEffect_439470(action));
        }
    } else if (count >= 14 * initialBit + 54) {
        payload->valueFCC = 0;
        if ((initialFlags & 2u) != 0) {
            if ((payload->effectFlagsFE4 & 0x3010) != 0) {
                action->value54 = 6.0F;
                action->framePlayingRate = 100;
            } else {
                action->value54 = 1.0F;
                action->framePlayingRate = 250;
            }
            AdvanceBattleCastState_439340(action);
        } else {
            action->value54 = 6.0F;
            action->framePlayingRate = 100;
            AdvanceBattleCastState_439340(action);
            if (action->phase == 11) {
                action->value54 = 4.0F;
                action->framePlayingRate = 100;
            }
        }
    }
    return PlayFrame_517DA0(action, 1);
}

// 原函数：0x00412F70。
YobiAction* UpdateBattleAction210_412F70(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    action->characterAction = 3;
    action->value54 = 16.0F;
    action->framePlayingRate = 50;
    TurnBattleToward_406190(action,
        cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 1, 0.0F);
    if (GetBattleDistance_406300(action,
        cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 70) {
        AdvanceBattleCastState_439340(action);
        action->value54 = 8.0F;
        action->framePlayingRate = 100;
    } else {
        MoveBattleByAngle_405EA0(action);
    }
    static_cast<void>(PlayFrame_517DA0(action, 0));
    const std::int32_t imageNo = 110004 + (platform::Rand16() & 3);
    return NewBattlePositionEffect_41E680(action, imageNo);
}

// 原函数：0x00413070。
void ApplyBattleSceneEffects_413070(YobiAction* const action)
{
    using world::gBattleSceneEffectC8A768;
    using world::gBattleSceneEffectC8A76C;
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    const std::int32_t firstIndex = payload->effectRunningC9C;
    const std::uint8_t firstFlags =
        *reinterpret_cast<const std::uint8_t*>(&payload->effects1C[firstIndex].flags);
    if ((firstFlags & 1u) != 0) {
        std::uint16_t value;
        std::memcpy(&value, &payload->effects1C[firstIndex].position3, sizeof(value));
        std::memcpy(&cg::world::gBattleSceneEffectC8A768, &value, sizeof(value));
    }
    const std::int32_t secondIndex = payload->effectRunningC9C;
    const std::uint8_t secondFlags =
        *reinterpret_cast<const std::uint8_t*>(&payload->effects1C[secondIndex].flags);
    if ((secondFlags & 2u) != 0) {
        std::uint16_t value;
        std::memcpy(&value, &payload->effects1C[secondIndex].position3, sizeof(value));
        std::memcpy(reinterpret_cast<std::byte*>(&cg::world::gBattleSceneEffectC8A768) + 2, &value, sizeof(value));
    }
    const std::int32_t thirdIndex = payload->effectRunningC9C;
    const std::uint8_t thirdFlags =
        *reinterpret_cast<const std::uint8_t*>(&payload->effects1C[thirdIndex].flags);
    if ((thirdFlags & 4u) != 0) {
        std::uint16_t value;
        std::memcpy(&value, &payload->effects1C[thirdIndex].position3, sizeof(value));
        std::memcpy(&cg::world::gBattleSceneEffectC8A76C, &value, sizeof(value));
    }
    const std::int32_t fourthIndex = payload->effectRunningC9C;
    const std::uint8_t fourthFlags =
        *reinterpret_cast<const std::uint8_t*>(&payload->effects1C[fourthIndex].flags);
    if ((fourthFlags & 8u) != 0) {
        std::uint16_t value;
        std::memcpy(&value, &payload->effects1C[fourthIndex].position3, sizeof(value));
        std::memcpy(reinterpret_cast<std::byte*>(&cg::world::gBattleSceneEffectC8A76C) + 2, &value, sizeof(value));
    }
    static_cast<void>(AdvanceBattleEffect_439470(action));
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00413110。
void UpdateBattleAction212_413110(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    YobiAction* const effect = NewBattleRandomEffect_419FC0(
        action, payload->effects1C[payload->effectRunningC9C].position, 0);
    const std::int32_t xOffset = payload->effects1C[payload->effectRunningC9C].position3;
    effect->x += xOffset;
    const std::int32_t yOffset = payload->effects1C[payload->effectRunningC9C].position4;
    effect->y += yOffset;
    const std::uint8_t playFlags =
        *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
    if ((playFlags & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    const std::int32_t count = payload->valueFF0 + 1;
    payload->valueFF0 = count;
    const std::int32_t threshold = std::bit_cast<std::int32_t>(
        payload->effects1C[payload->effectRunningC9C].flags);
    if (count >= threshold) {
        payload->valueFF0 = 0;
        static_cast<void>(AdvanceBattleEffect_439470(action));
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x004131B0。
std::int32_t UpdateBattleAction213_4131B0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if ((payload->flagsFDC & 0x10000000u) != 0) {
        const std::int32_t phase = action->phase;
        payload->valueFF0 = phase;
        action->phase = 164;
        return phase;
    }
    action->characterAction = 10;
    action->framePlayingRate = 100;
    const std::int32_t result = PlayFrame_517DA0(action, 1);
    if (result != 1) {
        return result;
    }
    if ((payload->flagsFDC & 0x1000u) != 0) {
        action->scaleX = 1.0F;
        action->scaleY = 1.0F;
        payload->flagsFDC &= ~0x1000u;
    }
    YobiAction* effect = payload->effect105C;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->effect105C = nullptr;
    }
    effect = payload->effect1054;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->effect1054 = nullptr;
    }
    effect = payload->effect1068;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->effect1068 = nullptr;
    }
    effect = payload->attachedEffect1058;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        payload->attachedEffect1058 = nullptr;
    }
    const std::int32_t next = payload->value13B0;
    action->phase = next != 0 ? 220 : 214;
    return next;
}

// 原函数：0x004132B0。
void UpdateBattleAction214_4132B0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (render::HasLegacy3DDevice_44B610() == 0) {
        const std::int32_t count = payload->valueFF0 + 1;
        payload->valueFF0 = count;
        if (action->field58 >= 8) {
            const std::uint32_t flags = action->flags;
            const std::int32_t x = action->x;
            action->field58 = 0;
            action->flags = flags | 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(218, x));
            static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 36, 0, 1));
            action->phase = 215;
        } else if (count > 4) {
            payload->valueFF0 = 0;
            const std::uint32_t flags = action->flags ^ 2u;
            const std::int32_t step = action->field58 + 1;
            action->flags = flags;
            action->field58 = step;
        }
        return;
    }
    if (payload->valueFF0 != 255) {
        payload->valueFF0 = 255;
        static_cast<void>(audio::PlaySe_52C450(218, action->x));
    }
    const std::uint8_t previousAlpha = GetYobiActionField30F(action);
    SetYobiActionField308(action, 8);
    if (previousAlpha == 0) {
        action->field58 = 0;
        payload->valueFF0 = 0;
        AdvanceBattleCastState_439340(action);
        return;
    }
    static_cast<void>(NewBattleRisingEffect_420D50(action));
    const std::uint8_t alpha = GetYobiActionField30F(action);
    SetYobiActionField30F(action, alpha < 4 ? 0 : static_cast<std::uint8_t>(alpha - 4));
}

// 原函数：0x004133D0。
void UpdateBattleAction215_4133D0(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (payload->value1024 == 2) {
        payload->value1024 = 0;
        AdvanceBattleCastState_439340(action);
    }
}

// 原函数：0x00413400。
void UpdateBattleAction216_413400(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    action->characterAction = 6;
    if (payload->value1024 == 0) {
        action->battleCastEffect10 = NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 34, 0, 1);
        payload->valueFF0 = 0;
    }
    const std::int32_t event = action->frameEvent;
    if (event >= 10000 && event < 20000) {
        YobiAction* const effect = action->battleCastEffect10;
        if (effect != nullptr) {
            static_cast<void>(MarkDeath(effect));
            action->battleCastEffect10 = nullptr;
            for (;;) {
                const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
                if (position == 255) {
                    break;
                }
                static_cast<void>(SeedBattleRandom_4065A0(position + payload->battlePosition18));
                if (payload->castSkill103C == 400) {
                    static_cast<void>(NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 35, 0, 0));
                }
                const std::int32_t currentIndex = payload->effectRunningC9C;
                const std::int32_t ownPosition = payload->battlePosition18;
                const std::int32_t currentPosition = payload->effects1C[currentIndex].position;
                if (ownPosition != currentPosition) {
                    AdvanceBattleCastState_439340(cg::world::gBattleAllEntries59E980[currentPosition]);
                }
                static_cast<void>(AdvanceBattleEffect_439470(action));
            }
            static_cast<void>(AdvanceBattleEffect_439470(action));
        }
        action->frameEvent = 0;
    }
    if (PlayFrame_517DA0(action, 1) == 1) {
        YobiAction* const effect = action->battleCastEffect10;
        if (effect != nullptr) {
            static_cast<void>(MarkDeath(effect));
            action->battleCastEffect10 = nullptr;
        }
        payload->value1024 = 0;
        AdvanceBattleCastState_439340(action);
    }
    if (action->battleCastEffect10 != nullptr) {
        const std::int32_t skill = payload->castSkill103C;
        if (skill == 400) {
            static_cast<void>(DrawBattleTiledEffect_41E890(3));
        } else if (skill == 401) {
            static_cast<void>(DrawBattleTiledEffect_41E890(1));
        }
    }
}

// 原函数：0x00413550。
void UpdateBattleAction218_413550(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    action->characterAction = payload->effects1C[payload->effectRunningC9C].position == -1 ? 0 : 6;
    if (payload->effect1060 == nullptr) {
        if (payload->effects1C[payload->effectRunningC9C].position != -1
            && PlayFrame_517DA0(action, 1) != 1) {
            return;
        }
        YobiAction* const copy = CloneBattleAction_41EAB0(action);
        payload->effect1060 = copy;
        copy->callback = nullptr;
        const std::int32_t index = payload->effectRunningC9C;
        YobiAction* const imageCopy = payload->effect1060;
        const std::int32_t imageNo = payload->effects1C[index].position4;
        imageCopy->imageNo = imageNo;
        payload->effect1060->characterAction = 0;
        SetYobiActionField30F(payload->effect1060, 0);
        static_cast<void>(PlayFrame_517DA0(payload->effect1060, 0));
        return;
    }
    static_cast<void>(NewBattleRandomEffect_419FC0(action, 110471, 0));
    if (render::HasLegacy3DDevice_44B610() == 0) {
        action->flags |= 0x8000u;
        payload->effect1060->flags |= 0x10000u;
    }
    SetYobiActionField308(action, 8);
    SetYobiActionField308(payload->effect1060, 8);
    YobiAction* const copy = payload->effect1060;
    const std::int32_t nextAlpha = GetYobiActionField30F(copy) + 7;
    if (nextAlpha < 255) {
        const std::int32_t alpha = GetYobiActionField30F(action) - 7;
        SetYobiActionField30F(action, alpha <= 0 ? 0 : static_cast<std::uint8_t>(alpha));
        YobiAction* const currentCopy = payload->effect1060;
        const std::uint8_t copyAlpha = GetYobiActionField30F(currentCopy);
        SetYobiActionField30F(currentCopy, static_cast<std::uint8_t>(copyAlpha + 7));
        return;
    }
    static_cast<void>(MarkDeath(copy));
    const std::int32_t index = payload->effectRunningC9C;
    payload->effect1060 = nullptr;
    const BattleEntryEffectRecord& effect = payload->effects1C[index];
    if (effect.position != -1) {
        const std::uint32_t flags = payload->flagsFDC;
        payload->value13B0 = 1;
        if ((flags & 0x10000000u) == 0) {
            payload->baseImage13AC = action->imageNo;
            action->imageNo = effect.position4;
        }
    } else {
        const std::uint32_t flags = payload->flagsFDC;
        payload->value13B0 = 0;
        if ((flags & 0x10000000u) == 0) {
            action->imageNo = effect.position4;
        }
    }
    action->characterAction = 0;
    SetYobiActionField308(action, 0);
    SetYobiActionField30F(action, 255);
    reinterpret_cast<unsigned char*>(&action->drawMode.color)[3] = 255;
    static_cast<void>(PlayFrame_517DA0(action, 0));
    if (render::HasLegacy3DDevice_44B610() == 0) {
        action->flags &= ~0x8000u;
    }
    action->frameEvent = 0;
    AdvanceBattleCastState_439340(action);
}

namespace {

// 413750/413980相同的恢复阶段；true仅表示本帧完成，两个动作的完成尾仍各自执行。
bool RestoreBattleBaseImage(YobiAction* const action, BattleActorPayload* const payload)
{
    if (payload->effect1060 == nullptr) {
        YobiAction* const copy = CloneBattleAction_41EAB0(action);
        payload->effect1060 = copy;
        copy->callback = nullptr;
        YobiAction* const imageCopy = payload->effect1060;
        const std::int32_t baseImage = payload->baseImage13AC;
        imageCopy->imageNo = baseImage;
        payload->effect1060->characterAction = 10;
        SetYobiActionField30F(payload->effect1060, 0);
        YobiAction* const currentCopy = payload->effect1060;
        // 原同步推进到终帧；循环只重取1060，不重查图号或限制迭代次数。
        if (currentCopy->imageNo > 999 && PlayFrame_517DA0(currentCopy, 1) != 1) {
            while (PlayFrame_517DA0(payload->effect1060, 1) != 1) {
            }
        }
        if (payload->effect1060 == nullptr) {
            return false;
        }
    }
    static_cast<void>(NewBattleRandomEffect_419FC0(action, 110471, 0));
    if (render::HasLegacy3DDevice_44B610() == 0) {
        action->flags |= 0x8000u;
        payload->effect1060->flags |= 0x10000u;
    }
    SetYobiActionField308(action, 8);
    SetYobiActionField308(payload->effect1060, 8);
    YobiAction* const copy = payload->effect1060;
    const std::int32_t nextAlpha = GetYobiActionField30F(copy) + 7;
    if (nextAlpha < 255) {
        const std::int32_t alpha = GetYobiActionField30F(action) - 7;
        SetYobiActionField30F(action, alpha <= 0 ? 0 : static_cast<std::uint8_t>(alpha));
        YobiAction* const currentCopy = payload->effect1060;
        const std::uint8_t copyAlpha = GetYobiActionField30F(currentCopy);
        SetYobiActionField30F(currentCopy, static_cast<std::uint8_t>(copyAlpha + 7));
        return false;
    }
    static_cast<void>(MarkDeath(copy));
    const std::int32_t imageNo = payload->baseImage13AC;
    payload->effect1060 = nullptr;
    action->imageNo = imageNo;
    action->characterAction = 10;
    SetYobiActionField308(action, 0);
    SetYobiActionField30F(action, 255);
    reinterpret_cast<unsigned char*>(&action->drawMode.color)[3] = 255;
    if (imageNo > 999 && PlayFrame_517DA0(action, 1) != 1) {
        while (PlayFrame_517DA0(action, 1) != 1) {
        }
    }
    payload->value13B0 = 0;
    action->frameEvent = 0;
    if (render::HasLegacy3DDevice_44B610() == 0) {
        action->flags &= ~0x8000u;
    }
    return true;
}

} // namespace

// 原函数：0x00413750。
void UpdateBattleAction219_413750(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (!RestoreBattleBaseImage(action, payload)) {
        return;
    }
    const std::uint8_t flags =
        *reinterpret_cast<const std::uint8_t*>(&payload->flagsFDC);
    if ((flags & 0x0Cu) == 0) {
        YobiAction* const effect = payload->attachedEffect1058;
        if (effect != nullptr) {
            static_cast<void>(MarkDeath(effect));
            payload->attachedEffect1058 = nullptr;
        }
        payload->stateFE0 |= 2u;
        action->phase = 48;
        return;
    }
    static_cast<void>(NewBattleCenteredEffect_41FA00(action, 110169));
    static_cast<void>(audio::PlaySe_52C450(215, action->x));
    payload->stateFE0 |= 2u;
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x00413980。
void UpdateBattleAction220_413980(YobiAction* const action)
{
    auto* const payload = action->payload.battleActor;
    if (RestoreBattleBaseImage(action, payload)) {
        action->phase = 214;
    }
}

// 原函数：0x00413B40。
void UpdateBattleAction221_413B40(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    std::int32_t imageNo = -1;
    const std::int32_t attackerPosition = payload->effects1C[payload->effectRunningC9C].position;
    static_cast<void>(AdvanceBattleEffect_439470(action));
    const std::int32_t initialIndex = payload->effectRunningC9C;
    const std::int32_t targetPosition = payload->effects1C[initialIndex].position;
    const std::uint32_t initialFlags = payload->effects1C[initialIndex].flags;
    YobiAction* const target = cg::world::gBattleAllEntries59E980[targetPosition];
    auto* const targetPayload = target->payload.battleActor;
    if ((initialFlags & 4u) != 0) {
        imageNo = 110000;
    } else if ((initialFlags & 0x400u) != 0) {
        imageNo = FindBattleAppendEffectTechnique_43BEA0(8, targetPosition) < 12 ? 110098 : 111048;
        static_cast<void>(NewBattleRandomEffect_419FC0(action, 110014, 0));
    } else if (payload->castSkillLevel1044 >= 9) {
        imageNo = 110014;
    }
    target->battleOpponent10 = cg::world::gBattleAllEntries59E980[attackerPosition];
    targetPayload->effectFlagsFE4 = std::bit_cast<std::int32_t>(
        payload->effects1C[payload->effectRunningC9C].flags);
    ApplyBattleEffectFlags_406390(attackerPosition, action);
    const std::int32_t index = payload->effectRunningC9C;
    const std::uint32_t flags = payload->effects1C[index].flags;
    if ((flags & 0x8000u) == 0) {
        if ((flags & 0x4000u) != 0) {
            if ((flags & 0x10000000u) != 0) {
                static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22459, 0, 0));
            } else {
                (target->readiness += static_cast<std::int32_t>(payload->effects1C[index].position3));
                if ((target->readiness > target->maxHp)) {
                    target->readiness = target->maxHp;
                }
                const std::int32_t numberIndex = payload->effectRunningC9C;
                const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                const std::int32_t y = target->y;
                const std::int32_t amount = payload->effects1C[numberIndex].position3;
                const std::int32_t x = target->x;
                static_cast<void>(NewBattleNumber_41F1A0(x, y - 80, amount, 5, 2, numberFlags));
                const std::uint8_t recoveryIconFlags = *reinterpret_cast<const std::uint8_t*>(
                    &payload->effects1C[payload->effectRunningC9C].flags);
                if ((recoveryIconFlags & 2u) != 0) {
                    static_cast<void>(NewBattleFloatingIcon_41F4C0(target, 22454, 0, 0));
                }
                static_cast<void>(NewBattleAttributeEffects_4209C0(action, target));
            }
        } else if ((flags & 0x2000u) == 0) {
            if ((flags & 0x10000000u) != 0) {
                static_cast<void>(NewBattleFloatingIcon_41F4C0(
                    cg::world::gBattleAllEntries59E980[payload->effects1C[index].position], 22459, 0, 0));
            } else {
                (target->readiness -= static_cast<std::int32_t>(payload->effects1C[index].position3));
                if ((target->readiness < 0)) {
                    // 原413D6A/74/7A清源对象HP而非目标，且先mask后encoded，不能改用同类operator=。
                    // 现由整数赋值统一三字段发布；一次RNG，目标token／reserved保持。
                    action->readiness = std::int32_t{0};
                }
                const std::int32_t hitIndex = payload->effectRunningC9C;
                if (payload->effects1C[hitIndex].position3 != 0) {
                    const std::uint8_t hitFlags =
                        *reinterpret_cast<const std::uint8_t*>(&payload->effects1C[hitIndex].flags);
                    if ((hitFlags & 2u) != 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, 110008, 0));
                    }
                    if (imageNo >= 0) {
                        static_cast<void>(NewBattleRandomEffect_419FC0(target, imageNo, 0));
                    }
                    NewBattleRebirthEffects_4208F0(cg::world::gBattleAllEntries59E980[attackerPosition], target);
                    const std::int32_t numberIndex = payload->effectRunningC9C;
                    const std::uint32_t numberFlags = payload->effects1C[numberIndex].flags;
                    const std::int32_t y = target->y;
                    const std::int32_t amount = payload->effects1C[numberIndex].position3;
                    const std::int32_t x = target->x;
                    static_cast<void>(NewBattleNumber_41F1A0(x, y - 80, amount, 6, 2, numberFlags));
                    const std::uint8_t damageIconFlags = *reinterpret_cast<const std::uint8_t*>(
                        &payload->effects1C[payload->effectRunningC9C].flags);
                    if ((damageIconFlags & 2u) != 0) {
                        static_cast<void>(NewBattleFloatingIcon_41F4C0(action, 22454, 0, 0));
                    }
                    static_cast<void>(NewBattleAttributeEffects_4209C0(
                        cg::world::gBattleAllEntries59E980[attackerPosition], target));
                }
            }
            const std::uint8_t activateFlags = *reinterpret_cast<const std::uint8_t*>(
                &payload->effects1C[payload->effectRunningC9C].flags);
            if ((activateFlags & 0x20u) != 0) {
                payload->flagsFDC |= 2u;
                gYobiUpdateGateMode = 1;
                payload->aliveFF8 = 1;
            }
        }
    }
    AdvanceBattleCastState_439340(action);
    static_cast<void>(AdvanceBattleEffect_439470(action));
}

// 原函数：0x00413E90。
std::int32_t UpdateBattleAction222_413E90(YobiAction* const action)
{
    using world::gBattleAllEntries59E980;
    auto* const payload = action->payload.battleActor;
    TurnBattleToward_406190(action,
        cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position], 0, 10.0F);
    if (GetBattleDistance_406300(action,
        cg::world::gBattleAllEntries59E980[payload->effects1C[payload->effectRunningC9C].position]) < 20) {
        const std::int32_t skill = payload->castSkill103C;
        if (skill != 266 && skill != 2022) {
            static_cast<void>(MarkDeath(action));
            static_cast<void>(audio::PlaySe_52C450(216, action->x));
            MoveBattleByAngle_405EA0(action);
            return PlayFrame_517DA0(action, 0);
        }
        YobiAction* const target = cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position];
        target->payload.battleActor->valueFF0 = 0;
        AdvanceBattleCastState_439340(cg::world::gBattleAllEntries59E980[
            payload->effects1C[payload->effectRunningC9C].position]);
        static_cast<void>(AdvanceBattleEffect_439470(action));
        YobiAction* const opponent = action->battleOpponent10;
        action->phase = 22;
        TurnBattleToward_406190(action, opponent, 0, 0.0F);
        const double angle = action->angle50 + 180.0;
        action->angle50 = static_cast<float>(angle);
        if (angle > 180.0) {
            action->angle50 = static_cast<float>(angle - 360.0);
        } else if (!(angle > -180.0)) {
            action->angle50 = static_cast<float>(angle + 360.0);
        }
        action->value54 = static_cast<float>(action->value54 * 0.5);
    }
    MoveBattleByAngle_405EA0(action);
    return PlayFrame_517DA0(action, 0);
}

// 原函数：0x00413FE0。
void UpdateBattleAction223_413FE0(YobiAction* const action)
{
    const std::int32_t playerPosition = cg::world::gBattleCurrentPosition5ABCA4;
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    static_cast<void>(cg::world::GetBattlePetPosition_405B40(playerPosition));
    const std::int32_t position = payload->effects1C[payload->effectRunningC9C].position;
    if (position >= 0) {
        cg::world::gBattleSnapshotPetIndex5ABBFC = position;
        static_cast<void>(AdvanceBattleEffect_439470(action));
        AdvanceBattleCastState_439340(action);
        return;
    }
    if (position == -2 && cg::world::gBattleSnapshotPetIndex5ABBFC >= 0) {
        ui::detail::WindowBattleCommand_SyncActorVitals();
        cg::world::gBattleSnapshotPetIndex5ABBFC = -1;
    }
    static_cast<void>(AdvanceBattleEffect_439470(action));
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x0040B180。
void UpdateBattleAction251_40B180(YobiAction* const action)
{
    action->readiness = std::int32_t{1};
    action->barValue1B0 = std::int32_t{1};
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x0040FE50。
void UpdateBattleAction253_40FE50(YobiAction* const action)
{
    const auto* const payload = static_cast<const BattleActorPayload*>(action->payload.battleActor);
    if ((payload->flagsFDC & 0x60u) == 0) {
        static_cast<void>(PlayFrame_517DA0(action, 0));
    }
    if (action->phase == 253 && payload->value1024 == 0) {
        action->battleCastEffect10 = NewBattleEffect_41CD00(action, 0, {.offsetY = 0}, 43, 0, 1);
    }
    AdvanceBattleCastState_439340(action);
}

// 原函数：0x0040FEB0。
void UpdateBattleAction254_40FEB0(YobiAction* const action)
{
    YobiAction* const effect = action->battleCastEffect10;
    auto* const payload = action->payload.battleActor;
    if (effect != nullptr) {
        static_cast<void>(MarkDeath(effect));
        action->battleCastEffect10 = nullptr;
        payload->value1024 = 0;
    }
    AdvanceBattleCastState_439340(action);
}

} // namespace detail

} // namespace cg::yobi
