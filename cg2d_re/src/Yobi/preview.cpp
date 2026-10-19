#include "preview.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "base.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
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
    initialAction->flags |= 2u;
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
    const std::uint8_t mask = action->rebirthValue1;
    if (mask != 0) {
        std::uint8_t previous;
        std::uint8_t replacement;
        if (replacementBeforePrevious) {
            replacement = action->rebirthValue2;
            previous = action->drawMode.mixFlags;
        } else {
            previous = action->drawMode.mixFlags;
            replacement = action->rebirthValue2;
        }
        action->drawMode.mixFlags = static_cast<std::uint8_t>(
            (previous & ~mask) | (replacement & mask));
    }
    const std::uint8_t flags = action->batchType;
    const auto* const colorBytes = reinterpret_cast<const unsigned char*>(&action->color2);
    if (flags != 0) {
        auto* const drawColorBytes = reinterpret_cast<unsigned char*>(&action->drawMode.color);
        if ((flags & 1u) != 0) {
            drawColorBytes[2] = colorBytes[2];
        }
        if ((flags & 2u) != 0) {
            drawColorBytes[1] = colorBytes[1];
        }
        if ((flags & 4u) != 0) {
            drawColorBytes[0] = colorBytes[0];
        }
        if ((flags & 8u) != 0) {
            drawColorBytes[3] = colorBytes[3];
        }
    }
    return &action->drawMode;
}


void ConfigureWindowPetPreview(YobiAction*& slot, const std::int32_t imageNo)
{
    const std::int32_t mapped = character_select::ResolveCharacterImageForVersion(imageNo);
    slot->imageNo = mapped;
    slot->displayPriority = 10;
    slot->flags |= 2u;
    slot->callback = nullptr;
    slot->direction = 5;
}


// SIC01：只借两个既有静态预览的同一地址，原owner索引0/1由具名构造/退出入口固定。
// SIC01：同静态预览十CryptInt按原170..210次序构造/RNG，尾取flags|2快照，不构造通用Yobi对象。
// 原退出0x00488420／0x00488430／0x00499920：固定预览由各实际静态对象自动析构。
YobiAction* ConstructWindowSelectorPreview(const std::int32_t index, std::uint32_t& flags)
{
    // 两个lambda各拥有一个静态预览，分别在原索引0／1调用位置构造和登记退出。
    static constexpr std::array<YobiAction* (*)(), 2> initialize {
        +[]() -> YobiAction* { static YobiAction preview{}; return &preview; },
        +[]() -> YobiAction* { static YobiAction preview{}; return &preview; },
    };
    YobiAction& action = *initialize[index]();
    flags = action.flags | 2u;
    return &action;
}


// IWT01：未选择宠物时仅初始化预览属性，不读取负索引图号。
void FinishWindowSelectorPreview(YobiAction* const action, const std::uint32_t flags)
{
    action->flags = flags;
    action->callback = nullptr;
    // 原4C6274/4C627F使用pet[-1]图号地址。未选宠物不读取；DrawPreview在有效选择后设置图号。
    action->displayPriority = 0xDF;
    action->direction = 5;
}


// 原函数：0x0045E6E0。
std::int32_t AdvanceCharacterPreviewAnimation(YobiAction* const action)
{
    // 原55B2C0，每项依次为动作、持续次数、基图号偏移。
    static constexpr std::array<std::array<std::int32_t, 3>, 16> kPreviewAnimationSequence {{
        {{1, 2, 0}}, {{3, 2, 0}}, {{5, 1, -2}}, {{5, 1, -1}},
        {{5, 1, 0}}, {{5, 1, 1}}, {{5, 1, 2}}, {{5, 1, 3}},
        {{6, 1, 0}}, {{7, 1, 0}}, {{8, 1, 0}}, {{9, 1, 0}},
        {{10, 1, 0}}, {{11, 1, 0}}, {{12, 1, 0}}, {{13, 1, 0}},
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
