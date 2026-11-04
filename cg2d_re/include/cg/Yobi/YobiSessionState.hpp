#pragma once

#include "cg/session/SessionStorage.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace cg::yobi {

struct YobiAction;
namespace detail { struct BattleAppendEffectNode; }
namespace detail { struct BattleSkillEffectsData; }

inline constexpr std::size_t kMapRegistryRows = 399;
inline constexpr std::size_t kMapRegistryColumns = 399;
inline constexpr std::size_t kMapRegistryEntries = kMapRegistryRows * kMapRegistryColumns;

// 两个固定选择器预览独立于普通动作链；完整类型边界在preview.cpp构造/析构。
struct WindowSelectorPreviewRuntimeState {
    WindowSelectorPreviewRuntimeState();
    ~WindowSelectorPreviewRuntimeState();
    WindowSelectorPreviewRuntimeState(const WindowSelectorPreviewRuntimeState&) = delete;
    WindowSelectorPreviewRuntimeState& operator=(const WindowSelectorPreviewRuntimeState&) = delete;
    WindowSelectorPreviewRuntimeState(WindowSelectorPreviewRuntimeState&&) = delete;
    WindowSelectorPreviewRuntimeState& operator=(WindowSelectorPreviewRuntimeState&&) = delete;
    std::array<session::SessionStoragePtr<YobiAction>, 2> actions {};
};

// 原生技能特效的元数据及配置容器由会话独占，完整类型留在实现边界。
struct BattleEffectsRuntimeState {
    BattleEffectsRuntimeState();
    ~BattleEffectsRuntimeState();
    BattleEffectsRuntimeState(const BattleEffectsRuntimeState&) = delete;
    BattleEffectsRuntimeState& operator=(const BattleEffectsRuntimeState&) = delete;
    BattleEffectsRuntimeState(BattleEffectsRuntimeState&&) = delete;
    BattleEffectsRuntimeState& operator=(BattleEffectsRuntimeState&&) = delete;
    std::unique_ptr<detail::BattleSkillEffectsData> data; // 完整配置/元数据由battle_skill_effect.cpp持有。
};

// 每个会话持有其动作链；节点及载荷由 Yobi 生命周期函数分配和释放。
struct YobiSessionState {
    WindowSelectorPreviewRuntimeState selectorPreviews {};
    BattleEffectsRuntimeState battleEffects {};
    YobiAction* start = nullptr;
    YobiAction* end = nullptr;
    std::int32_t gateMode = 0; // 原 0x59E570，初值为 FullRate。
    std::int32_t phaseCounter = 0; // 原 0x59E574，UpdateAll 在 [0, 3] 间轮转。
    volatile std::uint32_t screenOffsetRising = 0; // 原 0x59F9F4。
    volatile std::int32_t screenOffsetX = 0; // 原 0x59F9FC。
    volatile std::int32_t screenOffsetY = 0; // 原 0x59FA00。
    volatile std::uint32_t screenOffsetAmplitude = 0; // 原 0x59FA04。
    volatile std::uint32_t screenOffsetMaximumAmplitude = 4; // 原 0x553C58。
    std::int32_t mapEffectDurationSeconds = 0; // 原 0x5AC4B8。
    std::uint64_t mapEffectStartTick = 0; // 原 0x5F973C。
    std::uint8_t mapEffectSpawnCounter = 0; // 原 0x5F9755。
    std::int32_t drawBattleActionCount = 0; // 原 0x59E578。
    std::array<YobiAction*, 257> drawBattleActions {}; // 原 0x59E57C，非拥有，排序范围外尾部不清。
    std::int32_t battleActionSequenceIndex = 0; // 原 0x59F9DC，战斗动作100的序列游标。
    detail::BattleAppendEffectNode* battleAppendEffects = nullptr; // 原 0x5ABA8C，清理函数释放链。
    std::array<std::uint8_t, kMapRegistryEntries> mapRegistryFlags {}; // 原 0xB65A80。
    std::array<YobiAction*, kMapRegistryEntries> mapRegistryActions {}; // 原 0xB8C864，非拥有。
    YobiAction* mapCharaHead = nullptr; // 原 0x5F9750，独立观察组链表头，非拥有。
    std::array<std::int32_t, 5> mapObservationValues {}; // 原 0x5F9720。
    std::array<std::int32_t, 5> mapObservationImages {}; // 原 0x5AC4A0。
    std::int32_t mapObservationImage = 0; // 原 0x5F9734。
    std::int32_t mapObservationDirection = 0; // 原 0x5F9740。
    std::int32_t mapObservationDuration = 0; // 原 0x5F9744。
};

} // namespace cg::yobi
