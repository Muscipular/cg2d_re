#include "preview.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "base.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/session/ClientSession.hpp"
#include "../world/PlayerState.hpp"

#include <array>
#include <cstdint>
#include <cstring>

namespace cg::yobi {





// WPL01：借初始action及现有action/图号槽保持原交错读取，见doc/cg2d_window_payload_creation_spec.md。
void ConfigureWindowElementAction(
    YobiAction* const initialAction, YobiAction*& slot,
    std::int32_t* const& imageNoSlot)
{
    initialAction->flags |= YobiActionFlag_Hidden;
    slot->callback = nullptr;
    const std::int32_t* const imageNo = imageNoSlot;
    YobiAction* const imageAction = slot;
    imageAction->imageNo = *imageNo;
    slot->displayPriority = 0xDF;
}


// WDP01：原颜色通道按选中BYTE读取；其它父全合同待，见doc/cg2d_window_element_draw_pipeline_spec.md。
// PSP01：预览显式先替换后旧字节；其余调用沿原先旧后替换，两分支取样后同式发布，无新增字段默认值。
const render::GraphicDescriptor16* ApplyWindowAnimationDrawMode(
    YobiAction* const action, const bool replacementBeforePrevious)
{
    const std::uint8_t mask = action->mixFlagsOverrideMask;
    if (mask != 0) {
        std::uint8_t previous;
        std::uint8_t replacement;
        if (replacementBeforePrevious) {
            replacement = action->mixFlagsOverrideValue;
            previous = action->drawMode.mixFlags;
        } else {
            previous = action->drawMode.mixFlags;
            replacement = action->mixFlagsOverrideValue;
        }
        action->drawMode.mixFlags = static_cast<std::uint8_t>(
            (previous & ~mask) | (replacement & mask));
    }
    const std::uint8_t flags = action->batchType;
    const auto* const colorBytes = reinterpret_cast<const unsigned char*>(&action->color2);
    if (flags != 0) {
        auto* const drawColorBytes = reinterpret_cast<unsigned char*>(&action->drawMode.color);
        if ((flags & YobiColorChannelFlag_Red) != 0) {
            drawColorBytes[2] = colorBytes[2];
        }
        if ((flags & YobiColorChannelFlag_Green) != 0) {
            drawColorBytes[1] = colorBytes[1];
        }
        if ((flags & YobiColorChannelFlag_Blue) != 0) {
            drawColorBytes[0] = colorBytes[0];
        }
        if ((flags & YobiColorChannelFlag_Alpha) != 0) {
            drawColorBytes[3] = colorBytes[3];
        }
    }
    return &action->drawMode;
}


void ConfigureWindowPetPreview(
    session::ClientSession& session, YobiAction*& slot, const std::int32_t imageNo)
{
    const std::int32_t mapped = character_select::ResolveCharacterImageForVersion(session, imageNo);
    slot->imageNo = mapped;
    slot->displayPriority = 10;
    slot->flags |= YobiActionFlag_Hidden;
    slot->callback = nullptr;
    slot->direction = YobiDirection_Down;
}


WindowSelectorPreviewRuntimeState::WindowSelectorPreviewRuntimeState() = default;
WindowSelectorPreviewRuntimeState::~WindowSelectorPreviewRuntimeState() = default;

// SIC01/NSM01：首次仍在原索引0/1入口按声明顺序构造十CryptInt，不进入普通动作链。
// 原退出0x00488420／0x00488430／0x00499920：改由实际会话owner逆序析构独占对象。
YobiAction* ConstructWindowSelectorPreview(
    session::ClientSession& session, const std::int32_t index, std::uint32_t& flags)
{
    auto& slot = session.Yobi().selectorPreviews.actions[index];
    if (slot == nullptr) {
        slot = session::MakeSessionStorage<YobiAction>(session);
        slot->ownerSession = &session;
    }
    YobiAction& action = *slot;
    flags = action.flags | YobiActionFlag_Hidden;
    return &action;
}


// IWT01：未选择宠物时仅初始化预览属性，不读取负索引图号。
void FinishWindowSelectorPreview(YobiAction* const action, const std::uint32_t flags)
{
    action->flags = flags;
    action->callback = nullptr;
    // 原4C6274/4C627F使用pet[-1]图号地址。未选宠物不读取；DrawPreview在有效选择后设置图号。
    action->displayPriority = 0xDF;
    action->direction = YobiDirection_Down;
}


// 原函数：0x0045E6E0。
std::int32_t AdvanceCharacterPreviewAnimation(YobiAction* const action)
{
    // 原55B2C0，每项依次为动作、持续次数、基图号偏移。
    static constexpr std::array<std::array<std::int32_t, 3>, 16> kPreviewAnimationSequence {{
        {{CharacterAnimation_Walk, 2, 0}},
        {{CharacterAnimation_Run, 2, 0}},
        {{CharacterAnimation_Attack, 1, -2}},
        {{CharacterAnimation_Attack, 1, -1}},
        {{CharacterAnimation_Attack, 1, 0}},
        {{CharacterAnimation_Attack, 1, 1}},
        {{CharacterAnimation_Attack, 1, 2}},
        {{CharacterAnimation_Attack, 1, 3}},
        {{CharacterAnimation_Cast, 1, 0}},
        {{CharacterAnimation_Throw, 1, 0}},
        {{CharacterAnimation_Damage, 1, 0}},
        {{CharacterAnimation_Guard, 1, 0}},
        {{CharacterAnimation_Down, 1, 0}},
        {{CharacterAnimation_Sit, 1, 0}},
        {{CharacterAnimation_Wave, 1, 0}},
        {{CharacterAnimation_Joy, 1, 0}},
    }};
    const std::int32_t phase = action->phase;
    if (phase < 0 || static_cast<std::uint32_t>(phase) >= 16) {
        action->phase = 0;
    }
    const std::int32_t counter = action->field1D0 + 1;
    action->field1D0 = counter;
    std::int32_t result = action->phase;
    if (counter >= kPreviewAnimationSequence[result][1]) {
        const std::int32_t next = result + 1;
        action->field1D0 = 0;
        action->phase = next;
        if (static_cast<std::uint32_t>(next) >= 16) {
            action->phase = 0;
        }
        const std::int32_t base = static_cast<std::int32_t>(action->readiness);
        const std::int32_t current = action->phase;
        action->imageNo = base;
        const std::int32_t offset = kPreviewAnimationSequence[current][2];
        if (offset != 0) {
            action->imageNo = base + offset;
        }
        result = kPreviewAnimationSequence[current][0];
        action->characterAction = result;
    }
    action->lastCharacterAction = -1;
    return result;
}

} // namespace cg::yobi
