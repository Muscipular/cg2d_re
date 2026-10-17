#include "battle_skill_effect.hpp"
#include "battle_message.hpp"
#include "cg/core/m_log.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "../world/BattleState.hpp"

#include <array>
#include <optional>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace cg::yobi::detail {

// E01：只在解析新的 SKL 时替换，主角色进入动作队列时绑定。
static BattleSkillMeta skillContext{};
static bool skillContextActive = false;

struct SkillEffectImage {
    std::int32_t skillId; // 原配置保留字段；原生特效实际按 techId 首匹配。
    std::int32_t techId;
    std::int32_t imageNo;
    std::optional<std::int32_t> castingAnime;
};

static std::array<std::vector<SkillEffectImage>, static_cast<std::size_t>(SkillEffectKind::Count)> effectImages;

BattleSkillMeta* GetBattleSkillMeta(YobiAction* const action)
{
    return action && action->payloadKind == YobiPayloadKind::BattleActor
        && action->payload.battleActor ? &action->payload.battleActor->skillMeta : nullptr;
}

void CaptureBattleSkillMeta(const std::int32_t techId, const std::int32_t skillId)
{
    skillContext = {techId, skillId, 0, 0, 0, 0};
    skillContextActive = true;
}

void BindBattleSkillMeta(YobiAction* const action)
{
    if (!skillContextActive) return;
    auto* const meta = GetBattleSkillMeta(action);
    if (!meta) return;
    for (std::size_t slot = 0; slot < 20; ++slot) {
        if (world::gBattleAllEntries59E980[slot] != action) continue;
        BattleSkillMeta bound = skillContext;
        if (LuaSkillEffectBindMoveState(action, bound)) {
            *meta = bound;
        } else if (action->payload.battleActor->castSkill103C == gBattleMSkill5AAA78) {
            *meta = skillContext;
        }
        return;
    }
}

void InitializeBattleEffects()
{
    skillContext = {};
    skillContextActive = false;
    for (auto& entries : effectImages) entries.clear();
    try {
        const YAML::Node root = YAML::LoadFile("effectHook.yml");
        constexpr std::array<const char*, 10> names{
            "spiracleShot", "blastWave", "magicEffect", "healEffect", "recoveryEffect",
            "statusRecoverEffect", "reviveEffect", "bow", "knife", "boomerang",
        };
        for (std::size_t index = 0; index < names.size(); ++index) {
            const YAML::Node parent = index < 7 ? root : root["rangeHit"];
            if (!parent) continue;
            const YAML::Node section = parent[names[index]];
            if (!section) continue;
            const YAML::Node effects = section["effects"];
            if (!effects || !effects.IsSequence()) continue;
            for (const auto& effect : effects) {
                const bool graphic = index == 7 || index == 8;
                const auto skillId = effect["skillId"].as<std::int32_t>();
                const auto techId = effect["techId"].as<std::int32_t>();
                const auto imageNo = effect[graphic ? "graphicNo" : "animeNo"].as<std::int32_t>();
                std::optional<std::int32_t> casting;
                if (index >= 3 && index <= 6 && effect["castingAnime"]) {
                    casting = effect["castingAnime"].as<std::int32_t>();
                }
                effectImages[index].push_back({skillId, techId, imageNo, casting});
            }
        }
    } catch (const YAML::Exception& error) {
        static_cast<void>(error); // m_log 关闭时仍保留同一异常处理路径。
        m_log_i("effectHook.yml load failed: %s\n", error.what());
    }
    InitializeLuaSkillEffects();
}

void ShutdownBattleEffects()
{
    ShutdownLuaSkillEffects();
    skillContext = {};
    skillContextActive = false;
    for (auto& entries : effectImages) entries.clear();
}

std::int32_t ResolveSkillEffectImage(
    YobiAction* const source, const SkillEffectKind kind, const std::int32_t originalImage)
{
    const auto* const meta = GetBattleSkillMeta(source);
    if (!meta) return originalImage;
    const std::int32_t tech = meta->techId;
    switch (kind) {
    case SkillEffectKind::Magic: if (tech < 1900 || tech > 3199) return originalImage; break;
    case SkillEffectKind::Heal: if (tech < 6100 || tech > 6399) return originalImage; break;
    case SkillEffectKind::Recovery: if (tech < 6400 || tech > 6699) return originalImage; break;
    case SkillEffectKind::StatusRecover: if (tech < 6700 || tech > 6799) return originalImage; break;
    case SkillEffectKind::Revive: if (tech < 6800 || tech > 6899) return originalImage; break;
    default: break;
    }
    m_log_i("[SkillEffect] kind=%d image=%d skillId=%d techId=%d\n",
        static_cast<int>(kind), originalImage, meta->skillId, tech);
    for (const auto& effect : effectImages[static_cast<std::size_t>(kind)]) {
        if (effect.techId == tech) return effect.imageNo;
    }
    return originalImage;
}

std::int32_t ResolveSkillCastingImage(YobiAction* const source, const std::int32_t originalImage)
{
    const auto* const meta = GetBattleSkillMeta(source);
    if (!meta || meta->techId < 6100 || meta->techId > 6899) return originalImage;
    const auto kind = meta->techId <= 6399 ? SkillEffectKind::Heal
        : meta->techId <= 6699 ? SkillEffectKind::Recovery
        : meta->techId <= 6799 ? SkillEffectKind::StatusRecover : SkillEffectKind::Revive;
    m_log_i("[SkillEffect] casting image=%d skillId=%d techId=%d\n",
        originalImage, meta->skillId, meta->techId);
    for (const auto& effect : effectImages[static_cast<std::size_t>(kind)]) {
        if (effect.techId == meta->techId) return effect.castingAnime.value_or(originalImage);
    }
    return originalImage;
}

} // namespace cg::yobi::detail
