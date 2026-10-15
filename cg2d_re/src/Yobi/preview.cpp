#include "preview.hpp"
#include "base.hpp"
#include "../scene/character_select/CharacterSelectSceneDependencies.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <array>
#include <cstdint>
#include <cstring>

namespace cg::login {

namespace detail {

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
            previous = action->drawMode.bytes[13];
        } else {
            previous = action->drawMode.bytes[13];
            replacement = action->rebirthValue2;
        }
        action->drawMode.bytes[13] = static_cast<std::uint8_t>(
            (previous & ~mask) | (replacement & mask));
    }
    const std::uint8_t flags = action->batchType;
    const auto* const colorBytes = reinterpret_cast<const unsigned char*>(&action->color2);
    if (flags != 0) {
        if ((flags & 1u) != 0) {
            action->drawMode.bytes[10] = colorBytes[2];
        }
        if ((flags & 2u) != 0) {
            action->drawMode.bytes[9] = colorBytes[1];
        }
        if ((flags & 4u) != 0) {
            action->drawMode.bytes[8] = colorBytes[0];
        }
        if ((flags & 8u) != 0) {
            action->drawMode.bytes[11] = colorBytes[3];
        }
    }
    return &action->drawMode;
}

void ConfigureWindowPetPreview(YobiAction*& slot, const std::int32_t imageNo)
{
    const std::int32_t mapped = character_select::detail::Sub_51B740(imageNo);
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

// SIC01：同预览flags/callback/固定技能源图号/layer/direction原序发布，不补影子值或读负宠物槽。
void FinishWindowSelectorPreview(YobiAction* const action, const std::uint32_t flags)
{
    action->flags = flags;
    action->callback = nullptr;
    // 原固定来源F5E988归技能块，不建立负宠物槽或独立影子值。
    static_assert(0x3C + 3 * 0x94 + 0x19 + 83 == 0x264);
    std::int32_t imageNo;
    std::memcpy(&imageNo,
        world::detail::gSkillsF1BAFC[14].techniques3C[3].description.data() + 83,
        sizeof(imageNo));
    action->displayPriority = 0xDF;
    action->imageNo = imageNo;
    action->direction = 5;
}

// 原函数：0x0045E6E0。
std::int32_t Sub_45E6E0(YobiAction* const action)
{
    // 原55B2C0，每项依次为动作、持续次数、基图号偏移。
    static constexpr std::array<std::array<std::int32_t, 3>, 16> sequence {{
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
    if (counter >= sequence[result][1]) {
        const std::int32_t next = result + 1;
        action->field1D0 = 0;
        action->phase = next;
        if (static_cast<std::uint32_t>(next) >= 16) {
            action->phase = 0;
        }
        const std::int32_t base = action->readiness.value();
        const std::int32_t current = action->phase;
        action->imageNo = base;
        const std::int32_t offset = sequence[current][2];
        if (offset != 0) {
            action->imageNo = base + offset;
        }
        result = sequence[current][0];
        action->characterAction = result;
    }
    action->lastCharacterAction = -1;
    return result;
}

} // namespace detail

} // namespace cg::login
