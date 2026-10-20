#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/BattleState.hpp"
#include "../../world/ChatHistory.hpp"
#include "../../world/PlayerState.hpp"
#include "../../world/WorldMapState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowGatherElementId : ElementId {
    ButtonClose = 1,
    Element2 = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    DrawResultImage = 11,
    ButtonContinue = 12,
    ButtonClose_13 = 13,
    Element14 = 14,
    HandleSideArea = 15,
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00485700。
static WindowAnimationData* WindowGather_Cancel(WindowInstance* window); // 取消窗口12操作并归空闲，保留末动画载荷返回。

// 原函数：0x00485890。
static WindowAnimationData* WindowGather_InitializeAction(WindowInstance* window); // 窗口12计时/操作初始化。

// 原函数：0x00486CF0。
static std::int32_t WindowGather_ButtonContinue(std::int32_t index, std::int32_t state); // FP/背包检查与重启。

// 原函数：0x00486E50。
static std::int32_t WindowGather_ButtonClose(std::int32_t index, std::int32_t state); // 关闭或取消，hover后重取窗口。

// 原函数：0x00486F20。
static std::int32_t WindowGather_DrawResultImage(std::int32_t index, std::int32_t unusedState); // 结果图及固定框，恒返0。

// 原函数：0x00485AF0。
static WindowAnimationData* WindowGather_ShowResult(WindowInstance* window); // 窗口12完整结果展示及双计时槽发布。

// 原函数：0x00486020。
static WindowAnimationData* WindowGather_RefreshResult(); // 仅更新已有窗口12，不自动创建。

// 原函数：0x00485690。
static void WindowGather_Release();

// 原函数：0x00486040。
static std::int32_t WindowGather_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static std::int32_t gWindow12ResultD2D820 = 0; // 原全局：0x00D2D820。

static std::int32_t gWindow12ExperienceD2DFCC = 0; // 原全局：0x00D2DFCC。

static std::int32_t gWindow12ValueD40B08 = 0; // 原全局：0x00D40B08。

static std::int32_t gWindow12LevelUpC9C0FC = 0; // 原全局：0x00C9C0FC。

static std::int32_t gWindow12EnduranceCB0358 = 0; // 原全局：0x00CB0358。

static std::int32_t gWindow12DexterityCC3EC0 = 0; // 原全局：0x00CC3EC0。

static std::int32_t gWindow12IntelligenceCC4B60 = 0; // 原全局：0x00CC4B60。

static std::int32_t gWindow12ValueCF3FF0 = 0; // 原全局：0x00CF3FF0。 回复第七字段，486F20按结果图号消费。

static std::int32_t gWindow12ValueD66E28 = 0; // 原全局：0x00D66E28。

static std::wstring gWindow12NameD2DA94; // 原全局：0x00D2DA94。

static std::wstring gWindow12SuffixCB1E1C; // 原全局：0x00CB1E1C。

static std::wstring gWindow12DetailsCC190C; // 原全局：0x00CC190C。 只承接已证NUL文本，不与前两结果摘要混用。

static std::uint8_t gWindow12ModeCC3E80 = 0; // 原全局：0x00CC3E80。

static std::uint8_t gWindow12FlagCC4D27 = 0; // 原全局：0x00CC4D27。

static std::uint64_t gWindow12TickD45D4C = 0; // 原全局：0x00D45D4C。

static std::uint8_t gWindow12SkillLevelCB0608 = 0; // 原全局：0x00CB0608。

static std::uint8_t gWindow12KindD046CC = 0; // 原全局：0x00D046CC。

static std::uint64_t gWindow12DeadlineCB0F7C = 0; // 原全局：0x00CB0F7C。

static std::int32_t gWindow12PetImageCB10CC = 0; // 原全局：0x00CB10CC。

static std::int32_t gWindow12CrystalLevelCC3E90 = 0; // 原全局：0x00CC3E90。

static WindowGraphicData gWindowGraphic562568 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562568。

static WindowGraphicData gWindowGraphic562580 {0x3BA2E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562580。

static WindowGraphicData gWindowGraphic562598 {0x3BA30, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562598。

static WindowGraphicData gWindowGraphic5625C8 {0x3B56B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005625C8。

static WindowGraphicData gWindowGraphic5625E0 {0x3B565, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005625E0。

static WindowGraphicData gWindowGraphic5625F8 {0x3BA34, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005625F8。

static WindowTextPrototype gWindowText562610 { // 原全局：0x00562610。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？？？", // 原常量参考：0x00586060。
};

static std::int32_t gWindowAnimationImage562618 = 0x1AF16; // 原全局：0x00562618。

static std::int32_t gWindowAnimationImage56261C = 0x1AF16; // 原全局：0x0056261C。

static WindowElementPrototype gWindowElements562620[15] { // 原全局：0x00562620。
    {WindowElementType_Graphic, 178, 9, 11, 11, 1, {.graphic = &gWindowGraphic562568}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowGatherElementId::ButtonClose)},
    {WindowElementType_Animation, 107, 204, 0, 0, 0, {.animationImageNo = &gWindowAnimationImage56261C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element2)},
    {WindowElementType_Animation, 107, 204, 0, 0, 0, {.animationImageNo = &gWindowAnimationImage562618}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element3)},
    {WindowElementType_Animation, 107, 204, 0, 0, 0, {.animationImageNo = &gWindowAnimationImage56261C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element4)},
    {WindowElementType_Text, 16, 27, 0, 0, 1, {.text = &gWindowText562610}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element5)},
    {WindowElementType_Text, 16, 41, 0, 0, 1, {.text = &gWindowText562610}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element6)},
    {WindowElementType_Text, 16, 55, 0, 0, 1, {.text = &gWindowText562610}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element7)},
    {WindowElementType_Text, 16, 69, 0, 0, 1, {.text = &gWindowText562610}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element8)},
    {WindowElementType_Graphic, 16, 43, 0, 0, 1, {.graphic = &gWindowGraphic562598}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element9)},
    {WindowElementType_Graphic, 15, 40, 0, 0, 1, {.graphic = &gWindowGraphic5625F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element10)},
    {WindowElementType_EventArea, 143, 29, 0, 0, 1, {.unusedValue = 0}, &WindowGather_DrawResultImage, -1, static_cast<ElementId>(WindowGatherElementId::DrawResultImage)},
    {WindowElementType_Graphic, 29, 203, 49, 17, 1, {.graphic = &gWindowGraphic5625C8}, &WindowGather_ButtonContinue, -1, static_cast<ElementId>(WindowGatherElementId::ButtonContinue)},
    {WindowElementType_Graphic, 129, 203, 49, 17, 1, {.graphic = &gWindowGraphic5625E0}, &WindowGather_ButtonClose, -1, static_cast<ElementId>(WindowGatherElementId::ButtonClose_13)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic562580}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowGatherElementId::Element14)},
    {WindowElementType_EventArea, 201, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowGatherElementId::HandleSideArea)},
};

// 原型：窗口12；原全局：0x0054E480。
WindowPrototype gWindowPrototypeGather {0, 4, 383, 118, 198, 232, 0x80010101u, 15, gWindowElements562620, &WindowGather_Update, &WindowRuntime_DrawDefault, &WindowGather_Release}; // 12: 54E480

// 窗口函数实现。

// 原函数：0x004854A0。
cg::yobi::YobiAction* WindowGather_Open(const std::int32_t skill, const std::int32_t level)
{
    const auto skillByte = static_cast<std::uint8_t>(skill);
    gWindow12SkillIndexCA0470 = skillByte;
    gWindow12SkillLevelCB0608 = static_cast<std::uint8_t>(level);
    const std::int32_t skillId =
        world::gSkillsF1BAFC[std::bit_cast<std::int8_t>(skillByte)].skillId;
    gSkillSendModeC8A970 = 0;
    gWindow12ValueD66E28 = 0;
    switch (skillId) {
        case 225:
        case 251:
            gWindow12KindD046CC = 0;
            break;
        case 226:
        case 253:
            gWindow12KindD046CC = 1;
            break;
        case 227:
        case 250:
            gWindow12KindD046CC = 2;
            break;
        case 2000:
        case 2003:
            gWindow12KindD046CC = 3;
            break;
        default:
            break;
    }
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(WindowProfile_SaveOrigin(x, y));
    gSkillExtraDelayCF46C4 = 2000;
    WindowRuntime_IncrementOperationCount();
    WindowRuntime_IncrementGatherOperationCount();
    const auto references = static_cast<std::uint8_t>(gSkillOperationCount_D67AFD + 1);
    const auto skillReferences = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE + 1);
    gSkillOperationCount_D67AFD = references;
    gSkillStatusIconReferences_D67AFE = skillReferences;
    std::int32_t mode = 0;
    switch (world::gSkillsF1BAFC[std::bit_cast<std::int8_t>(gWindow12SkillIndexCA0470)].skillId) {
        case 225:
        case 251:
            mode = 5;
            break;
        case 226:
        case 253:
            mode = 7;
            break;
        case 227:
        case 250:
            mode = 6;
            break;
        case 2000:
        case 2003:
            mode = 16;
            break;
        default:
            break;
    }
    if (mode != 0 && world::GetPlayerStatusIcon_51BEC0() != mode) {
        static_cast<void>(world::RequestPlayerStatusIcon_51BE50(mode));
    }
    for (const std::int32_t id : {11, 12, 45, 46, 13}) {
        WindowInstance* const window = gWindowStates[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    return WindowRuntime_CreateAnimation(WindowId_Gather, 1, 0, nullptr);
}

// 原函数：0x00485690。
static void WindowGather_Release()
{
    WindowRuntime_DecrementOperationCount();
    const std::uint8_t mode = gWindow12ModeCC3E80;
    if (mode == 1 || mode == 2) {
        WindowRuntime_DecrementGatherOperationCount();
    }
    const std::uint8_t previousReferences = gSkillOperationCount_D67AFD;
    WindowInstance* const window = gWindowStates[WindowId_Gather].window;
    gSkillOperationCount_D67AFD = static_cast<std::uint8_t>(previousReferences - 1);
    window->state |= 0x8000u;
    // 原4856C1..4856CD：W8减后发布，分支消费同一结果。
    const auto skillReferences = static_cast<std::uint8_t>(gSkillStatusIconReferences_D67AFE - 1);
    gSkillStatusIconReferences_D67AFE = skillReferences;
    if (skillReferences == 0) {
        const std::int32_t state = world::GetPlayerStatusIcon_51BEC0();
        if ((state >= 5 && state <= 7) || state == 16) {
            static_cast<void>(world::RequestPlayerStatusIcon_51BE50(0));
        }
    }
}

// 原函数：0x00485700。
static WindowAnimationData* WindowGather_Cancel(WindowInstance* const window)
{
    const std::uint8_t mode = gWindow12ModeCC3E80;
    if (mode != 0 && mode != 3 && mode != 4) {
        WindowRuntime_DecrementGatherOperationCount();
    }
    gWindow12ModeCC3E80 = 0;
    for (std::int32_t index = 4; index <= 7; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->data.text->text = L"";
    }
    for (std::int32_t index = 4; index <= 10; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowGatherElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 1;
    if (auto* const buttonClose_13Element = window ? window->FindElementById(WindowGatherElementId::ButtonClose_13) : nullptr; buttonClose_13Element) buttonClose_13Element->visible = 1;
    auto* const element3 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr;
    if (element3) {
        cg::yobi::SetYobiActionField2B0(
            element3->data.animation->action, 0);
    }
    if (auto* const element32 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr; element32) element32->data.animation->value04 = 0;
    gWindow12ValueCF3FF0 = -1;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 2, 1, 3, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* const data = WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 1, 0, 5);
    gSkillSendModeC8A970 = 0;
    gWindow12ValueD66E28 = 0;
    return data;
}

// 原函数：0x00485890。
static WindowAnimationData* WindowGather_InitializeAction(WindowInstance* const window)
{
    if (gWindow12ModeCC3E80 != 1) {
        WindowRuntime_IncrementGatherOperationCount();
    }
    const std::int32_t previousValue = gWindow12ValueD66E28;
    gWindow12ModeCC3E80 = 1;
    gSkillSendModeC8A970 = 0;
    if (previousValue > 0) {
        const std::int32_t imageNo = gWindow12PetImageCB10CC;
        gSkillSendModeC8A970 = 1;
        gSkillPetImageCB0A18 = imageNo;
    }
    for (std::int32_t index = 4; index <= 7; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->data.text->text = L"";
    }
    for (std::int32_t index = 4; index <= 7; ++index) {
        if (auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
    }
    if (auto* const element9 = window ? window->FindElementById(WindowGatherElementId::Element9) : nullptr; element9) element9->visible = 1;
    if (auto* const element10 = window ? window->FindElementById(WindowGatherElementId::Element10) : nullptr; element10) element10->visible = 0;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowGatherElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 0;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowGatherElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonClose_13Element = window ? window->FindElementById(WindowGatherElementId::ButtonClose_13) : nullptr; buttonClose_13Element) buttonClose_13Element->visible = 1;
    auto* const element3 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr;
    if (element3) {
        cg::yobi::SetYobiActionField2B0(
            element3->data.animation->action, 0);
    }
    if (auto* const element32 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr; element32) element32->data.animation->value04 = 0;
    gWindow12ValueCF3FF0 = -1;
    gWindow12FlagCC4D27 = 0;
    const std::uint64_t tick = platform::ReadTickCount64();
    std::uint32_t duration = gSkillExtraDelayCF46C4 + 4500u;
    gWindow12TickD45D4C = tick;
    const std::uint8_t rebirthMode = gSkillRebirthModeEnabled_5F9754;
    gWindow12DeadlineCB0F7C = duration;
    if (rebirthMode != 0) {
        const std::int32_t rebirth = world::gPlayerRuntimeSlots.value08;
        duration = duration * kSkillRebirthRates57D7A4[rebirth] / 100u;
        gWindow12DeadlineCB0F7C = duration;
    }
    if (gSkillSendModeC8A970 != 0) {
        duration = duration * kSkillCrystalRates57D7B4[gWindow12CrystalLevelCC3E90] / 100u;
    }
    const std::uint64_t deadline = duration + tick;
    gSkillPreviewTickCF46C8 = tick;
    gWindow12DeadlineCB0F7C = deadline;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 2, 1, 3, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    if (gSkillSendModeC8A970 != 0) {
        return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 1, 0, 5);
    }
    return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 0, 10, 5);
}

// 原函数：0x00485AF0。
static WindowAnimationData* WindowGather_ShowResult(WindowInstance* const window)
{
    std::uint8_t mode = gWindow12ModeCC3E80;
    if (mode != 0 && mode != 3 && mode != 4) {
        WindowRuntime_DecrementGatherOperationCount();
        mode = gWindow12ModeCC3E80;
    }
    gWindow12ModeCC3E80 = mode == 0 ? 4 : 3;
    if (auto* const element5 = window ? window->FindElementById(WindowGatherElementId::Element5) : nullptr; element5) element5->visible = 1;
    if (auto* const element6 = window ? window->FindElementById(WindowGatherElementId::Element6) : nullptr; element6) element6->visible = 1;
    if (auto* const element7 = window ? window->FindElementById(WindowGatherElementId::Element7) : nullptr; element7) element7->visible = 1;
    if (auto* const element8 = window ? window->FindElementById(WindowGatherElementId::Element8) : nullptr; element8) element8->visible = 1;
    if (auto* const element9 = window ? window->FindElementById(WindowGatherElementId::Element9) : nullptr; element9) element9->visible = 0;
    if (auto* const element10 = window ? window->FindElementById(WindowGatherElementId::Element10) : nullptr; element10) element10->visible = 1;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowGatherElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 1;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowGatherElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonClose_13Element = window ? window->FindElementById(WindowGatherElementId::ButtonClose_13) : nullptr; buttonClose_13Element) buttonClose_13Element->visible = 1;
    if (gWindow12ResultD2D820 != 0) {
        if (auto* const element52 = window ? window->FindElementById(WindowGatherElementId::Element5) : nullptr; element52) element52->data.text->text = gWindow12NameD2DA94;
        if (auto* const element62 = window ? window->FindElementById(WindowGatherElementId::Element6) : nullptr; element62) element62->data.text->text = L"　发现了。" /* 原常量参考：0x005869D4。 */;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else {
        if (auto* const element52 = window ? window->FindElementById(WindowGatherElementId::Element5) : nullptr; element52) element52->data.text->text = L"什么都没发现。" /* 原常量参考：0x005869C4。 */;
        if (auto* const element62 = window ? window->FindElementById(WindowGatherElementId::Element6) : nullptr; element62) element62->data.text->text = L"";
        static_cast<void>(audio::PlaySe_52C450(58, 320));
        gWindow12ValueCF3FF0 = -1;
    }
    if (gWindow12LevelUpC9C0FC != 0) {
        auto* const element72 = window ? window->FindElementById(WindowGatherElementId::Element7) : nullptr;
        if (element72) {
            (element72->data.text->text = kCraftLevelUp58661C);
        }
    } else {
        auto* const element72 = window ? window->FindElementById(WindowGatherElementId::Element7) : nullptr;
        if (element72) {
            auto* const data = element72->data.text;
            cg::text::FormatTo(data->text, kCraftExperience58660C, gWindow12ExperienceD2DFCC);
        }
    }
    auto* const element82 = window ? window->FindElementById(WindowGatherElementId::Element8) : nullptr;
    if (element82) {
        std::wstring& increments = element82->data.text->text;
        increments.clear();
        if (gWindow12EnduranceCB0358 != 0) {
            increments.append(std::format(kCraftEndurance586600, gWindow12EnduranceCB0358));
        }
        if (gWindow12DexterityCC3EC0 != 0) {
            increments.append(std::format(kCraftDexterity5865F4, gWindow12DexterityCC3EC0));
        }
        if (gWindow12IntelligenceCC4B60 != 0) {
            increments.append(std::format(kCraftIntelligence5865E8, gWindow12IntelligenceCC4B60));
        }
    }
    if (auto* const element3 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr; element3) element3->data.animation->value04 = 1;
    const bool succeeded = gWindow12ResultD2D820 != 0;
    const std::int32_t defaultAction = succeeded ? 5 : 10;
    const std::int32_t alternateAction = succeeded ? 18 : 15;
    auto* const element32 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr;
    if (element32) {
        cg::yobi::SetYobiActionField2B0(
            element32->data.animation->action, defaultAction);
    }
    std::int32_t imageNo = world::gPlayerRuntimeSlots.imageNo;
    for (const auto& range : kCraftAlternateActionImages) {
        if (imageNo >= range[0] && imageNo <= range[1]) {
            auto* const element33 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr;
            if (element33) {
                cg::yobi::SetYobiActionField2B0(
                    element33->data.animation->action, alternateAction);
            }
            imageNo = world::gPlayerRuntimeSlots.imageNo;
        }
    }
    if (imageNo == 0x18A26) {
        auto* const element33 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr;
        if (element33) {
            cg::yobi::SetYobiActionField2B0(
                element33->data.animation->action, defaultAction);
        }
    }
    gWindow12FlagCC4D27 = 0;
    const std::uint64_t tick = platform::ReadTickCount64();
    gWindow12TickD45D4C = tick;
    gSkillPreviewTickCF46C8 = tick;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 2, 1, 3, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    if (gSkillSendModeC8A970 != 0) {
        return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 1, gWindow12ResultD2D820 != 0 ? 6 : 8, 5);
    }
    return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 0, 10, 5);
}

// 原函数：0x00486020。
static WindowAnimationData* WindowGather_RefreshResult()
{
    WindowInstance* const window = gWindowStates[WindowId_Gather].window;
    return window != nullptr ? WindowGather_ShowResult(window) : nullptr;
}

// 原函数：0x004ED6B0。
WindowAnimationData* WindowGather_ReceiveResult(const std::int32_t result, const char* const text)
{
    gWindow12ResultD2D820 = result;
    gWindow12DetailsCC190C.clear();
    gWindow12ValueD66E28 = 0;
    char buffer[768];
    if (result == 0) {
        gWindow12ExperienceD2DFCC = 0;
        // 原四个dword文字复制后清两槽，再复制末尾LF/NUL；string只保存逻辑文本。
        gWindow12DetailsCC190C += LR"(没看到任何东西。)" /* 原常量参考：0x005897DC。 */;
        gWindow12LevelUpC9C0FC = 0;
        gWindow12EnduranceCB0358 = 0;
        // 原两段拼接并为完整UTF-16正文追加。
        gWindow12DexterityCC3EC0 = 0;
        gWindow12IntelligenceCC4B60 = 0;
        return WindowGather_RefreshResult();
    }
    gWindow12ExperienceD2DFCC = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    gWindow12ValueD40B08 = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    gWindow12LevelUpC9C0FC = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    gWindow12EnduranceCB0358 = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    gWindow12DexterityCC3EC0 = network::ParseDelimitedInteger_52CFA0(text, '|', 5);
    gWindow12IntelligenceCC4B60 = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    gWindow12ValueCF3FF0 = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 8, 255, buffer));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
    gWindow12NameD2DA94 = std::strlen(buffer) <= 28 ? cg::text::FromGbkWide(buffer) : kWindow12Unknown554590;
    gWindow12ValueD66E28 = network::ParseDelimitedInteger_52CFA0(text, '|', 9);
    const std::int32_t experience = gWindow12ExperienceD2DFCC;
    gWindow12SuffixCB1E1C = L"得到了！" /* 原常量参考：0x00589830。 */;
    std::int32_t lines = 0;
    const auto appendValue = [&](const std::wstring_view format, const std::int32_t value) {
        // 四种数值行按原GBK最多22字节，始终低于28列换行门槛。
        gWindow12DetailsCC190C += std::vformat(format, std::make_wformat_args(value)) + L'\n';
        ++lines;
    };
    if (experience > 0) {
        appendValue(kCraftExperience589820, experience);
    }
    if (gWindow12LevelUpC9C0FC != 0 && lines < 8) {
        gWindow12DetailsCC190C += kCraftLevelUp589814;
        ++lines;
        static_cast<void>(audio::PlaySe_52C450(64, 320));
    }
    if (gWindow12EnduranceCB0358 != 0 && lines < 8) {
        appendValue(kCraftEndurance589808, gWindow12EnduranceCB0358);
    }
    if (gWindow12DexterityCC3EC0 != 0 && lines < 8) {
        appendValue(kCraftDexterity5897FC, gWindow12DexterityCC3EC0);
    }
    if (gWindow12IntelligenceCC4B60 != 0 && lines < 8) {
        appendValue(kCraftIntelligence5897F0, gWindow12IntelligenceCC4B60);
    }
    return WindowGather_RefreshResult();
}

// 原函数：0x00486040。
static std::int32_t WindowGather_Update(const std::int32_t value)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        gWindowActionD2E928 = nullptr;
        gWindow12ModeCC3E80 = 0;
        WindowRuntime_DecrementGatherOperationCount();
        static_cast<void>(WindowGather_InitializeAction(currentWindow));
        return 1;
    }
    const std::int32_t mapped = character_select::ResolveCharacterImageForVersion(world::gPlayerRuntimeSlots.imageNo);
    const std::int32_t kind = std::bit_cast<std::int8_t>(gWindow12KindD046CC);
    const std::int32_t offset = kind == 0 ? -2 : kind == 1 ? -1 : kind == 3 ? 1 : 0;
    const std::int32_t previewImage = WindowSkillResult_ReadPreviewImage(mapped, offset);
    auto* const element3 = currentWindow->FindElementById(WindowGatherElementId::Element3);
    if (element3) {
        cg::yobi::SetYobiActionField2A8(
            element3->data.animation->action, previewImage);
    }
    auto* const element32 = currentWindow->FindElementById(WindowGatherElementId::Element3);
    if (element32) {
        cg::yobi::SetYobiActionField2B8(
            element32->data.animation->action, 5);
    }
    std::uint8_t mode;
    if (gSkillSendModeC8A970 != 0) {
        const std::int32_t imageNo = WindowSkill_CanUseSpecialSkill() != 0 ? gSkillPetImageCB0A18 : 0;
        static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 1, 3, imageNo));
        mode = gWindow12ModeCC3E80;
        if (mode == 1) {
            static_cast<void>(WindowCraft_AnimatePreviewPositions(currentWindow, 2, 1, 3, gWindow12DeadlineCB0F7C));
            mode = gWindow12ModeCC3E80;
        }
    } else {
        static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 1, 3, 0));
        mode = gWindow12ModeCC3E80;
    }
    if (mode == 1) {
        // 四条阶段路径共用原6/16选择；每次命中仍重取当前窗口和玩家图号。
        const auto setWorkingAction = [currentWindow](cg::yobi::YobiAction* const initialAction) {
            cg::yobi::SetYobiActionField2B0(initialAction, 6);
            std::int32_t imageNo = world::gPlayerRuntimeSlots.imageNo;
            for (const auto& range : kCraftAlternateActionImages) {
                if (imageNo >= range[0] && imageNo <= range[1]) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::SetYobiActionField2B0(
                            element33->data.animation->action, 16);
                    }
                    imageNo = world::gPlayerRuntimeSlots.imageNo;
                }
            }
            if (imageNo == 0x18A26) {
                auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                if (element33) {
                    cg::yobi::SetYobiActionField2B0(
                        element33->data.animation->action, 6);
                }
            }
        };
        switch (std::bit_cast<std::int8_t>(gWindow12KindD046CC)) {
            case 0:
            case 1:
                if (gWindow12FlagCC4D27 == 0) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::SetYobiActionField2B0(
                            element33->data.animation->action, 1);
                    }
                    if (auto* const element34 = currentWindow->FindElementById(WindowGatherElementId::Element3); element34) element34->data.animation->value04 = 0;
                    ++gWindow12FlagCC4D27;
                } else if (gWindow12FlagCC4D27 == 1) {
                    const std::uint64_t tick = platform::ReadTickCount64();
                    if (gWindow12TickD45D4C + 2000u < tick) {
                        auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                        if (element33) {
                            cg::yobi::SetYobiActionField2B0(
                                element33->data.animation->action, 5);
                        }
                        ++gWindow12FlagCC4D27;
                    }
                } else if (gWindow12FlagCC4D27 == 2) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::YobiAction* const action =
                            element33->data.animation->action;
                        if (cg::yobi::GetYobiActionField2DC(action) >= 2) {
                            setWorkingAction(action);
                            ++gWindow12FlagCC4D27;
                        }
                    }
                }
                break;
            case 2:
                if (gWindow12FlagCC4D27 == 0) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::SetYobiActionField2B0(
                            element33->data.animation->action, 5);
                    }
                    if (auto* const element34 = currentWindow->FindElementById(WindowGatherElementId::Element3); element34) element34->data.animation->value04 = 0;
                    ++gWindow12FlagCC4D27;
                } else if (gWindow12FlagCC4D27 == 1) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::YobiAction* const action =
                            element33->data.animation->action;
                        if (cg::yobi::GetYobiActionField2DC(action) >= 2) {
                            setWorkingAction(action);
                            ++gWindow12FlagCC4D27;
                        }
                    }
                }
                [[fallthrough]]; // 原486562递增后落入486568，再检查同一阶段。
            case 3:
                if (gWindow12FlagCC4D27 == 0) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::SetYobiActionField2B0(
                            element33->data.animation->action, 5);
                    }
                    if (auto* const element34 = currentWindow->FindElementById(WindowGatherElementId::Element3); element34) element34->data.animation->value04 = 0;
                    ++gWindow12FlagCC4D27;
                } else if (gWindow12FlagCC4D27 == 1) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::YobiAction* const action =
                            element33->data.animation->action;
                        if (cg::yobi::GetYobiActionField2DC(action) >= 2) {
                            setWorkingAction(action);
                            ++gWindow12FlagCC4D27;
                        }
                    }
                }
                break;
            default:
                break;
        }
        const std::uint64_t tick = platform::ReadTickCount64();
        if (gWindow12DeadlineCB0F7C < tick) {
            const std::uint8_t sendMode = gSkillRebirthModeEnabled_5F9754;
            gSkillExtraDelayCF46C4 = 0;
            std::int32_t skill;
            std::int32_t level;
            if (sendMode != 0) {
                level = std::bit_cast<std::int8_t>(gWindow12SkillLevelCB0608);
                skill = std::bit_cast<std::int8_t>(gWindow12SkillIndexCA0470) + 100;
            } else {
                level = std::bit_cast<std::int8_t>(gWindow12SkillLevelCB0608);
                static_cast<void>(gSkillSendModeC8A970); // 原cmp未被后续分支消费，没有+200。
                skill = std::bit_cast<std::int8_t>(gWindow12SkillIndexCA0470);
            }
            static_cast<void>(network::SendSkillUseRequest_511EF0(
                platform::gConnectionSocket, skill, level, -1, L""));
            gWindow12PetImageCB10CC = WindowSkill_ReadBattlePetImage();
            gWindow12CrystalLevelCC3E90 = WindowSkill_ReadCrystalLevel();
            gWindow12ModeCC3E80 = 2;
        } else if (WindowProfile_CheckDistance(1, 1) != 0) {
            static_cast<void>(WindowGather_Cancel(currentWindow));
        }
    } else if (mode == 3) {
        const std::uint64_t tick = platform::ReadTickCount64();
        if (gWindow12TickD45D4C + 2000u < tick) {
            const std::int32_t skill = std::bit_cast<std::int8_t>(gWindow12SkillIndexCA0470);
            const std::int32_t level = std::bit_cast<std::int8_t>(gWindow12SkillLevelCB0608);
            const std::int32_t cost = world::gSkillsF1BAFC[skill].techniques3C[level].value7C
                * world::gSkillsF1BAFC[skill].fpReduce / 100;
            if ((world::gPlayerRuntimeSlots.fp < cost)) {
                world::AppendChatMessage_44B050(kWindow12NoFp5864F0, 4, 3);
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                static_cast<void>(audio::PlaySe_52C450(58, 320));
                static_cast<void>(WindowGather_Cancel(currentWindow));
            } else if (world::CountFreeInventorySlots_518C60() <= 0) {
                world::AppendChatMessage_44B050(L"物品栏没有空位。" /* 原常量参考：0x005869E0。 */, 4, 3);
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                static_cast<void>(audio::PlaySe_52C450(58, 320));
                static_cast<void>(WindowGather_Cancel(currentWindow));
            } else {
                static_cast<void>(WindowGather_InitializeAction(currentWindow));
            }
        }
    } else if (mode == 4) {
        const std::uint64_t tick = platform::ReadTickCount64();
        if (gWindow12TickD45D4C + 2000u < tick) {
            static_cast<void>(WindowGather_Cancel(currentWindow));
        }
    }
    auto* const element2 = currentWindow->FindElementById(WindowGatherElementId::Element2);
    if (element2) {
        auto* const animation = element2->data.animation;
        const std::uint32_t stop = animation->value04;
        static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(animation->action, 100, stop != 0, 0));
    }
    auto* const element4 = currentWindow->FindElementById(WindowGatherElementId::Element4);
    if (element4) {
        auto* const animation = element4->data.animation;
        const std::uint32_t stop = animation->value04;
        static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(animation->action, 100, stop != 0, 0));
    }
    auto* elementsWindow = currentWindow;
    auto* const element22 = elementsWindow->FindElementById(WindowGatherElementId::Element2);
    if (element22) {
        if (element22->visible != 0) {
            auto* const element23 = elementsWindow->FindElementById(WindowGatherElementId::Element2);
            if (element23) {
                cg::yobi::YobiAction* const action = element23->data.animation->action;
                const render::GraphicDescriptor16* const descriptor = cg::yobi::ApplyWindowAnimationDrawMode(action);
                WindowInstance* const window = currentWindow;
                const std::int32_t graphic = cg::yobi::GetYobiActionField20(action);
                elementsWindow = window;
                auto* const element24 = elementsWindow->FindElementById(WindowGatherElementId::Element2);
                if (element24) {
                    const std::int32_t y = element24->y + window->y;
                    auto* const element25 = elementsWindow->FindElementById(WindowGatherElementId::Element2);
                    if (element25) {
                        const std::int32_t x = element25->x + window->x;
                        static_cast<void>(render::DrawGraphic2_521DD0(x, y, 0xDF, graphic, 0, 0, descriptor));
                    }
                }
            }
        }
    }
    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
    if (element33) {
        cg::yobi::YobiAction* const action =
            element33->data.animation->action;
        const std::uint8_t currentMode = gWindow12ModeCC3E80;
        const std::int32_t imageNo = cg::yobi::GetYobiActionField2A8(action);
        if (currentMode == 4) {
            const std::int32_t actionId = cg::yobi::GetYobiActionField2B0(action);
            if (actionId == 18 || actionId == 5 || actionId == 15 || actionId == 10) {
                const bool succeeded = actionId == 18 || actionId == 5;
                const std::int32_t defaultAction = succeeded ? 5 : 10;
                cg::yobi::SetYobiActionField2B0(action, defaultAction);
                for (const auto& range : kCraftAlternateActionImages) {
                    if (imageNo >= range[0] && imageNo <= range[1]) {
                        cg::yobi::SetYobiActionField2B0(action, succeeded ? 18 : 15);
                    }
                }
                if (imageNo == 0x18A26) {
                    cg::yobi::SetYobiActionField2B0(action, defaultAction);
                }
            }
        }
        if (imageNo >= 0x1AF0E && imageNo <= 0x1AF25) {
            cg::yobi::SetYobiActionField309(action, 4);
            cg::yobi::SetYobiActionField310(action, 4);
        } else {
            cg::yobi::SetYobiActionField309(action, 0);
        }
        const std::uint32_t stop = element33->data.animation->value04;
        static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(action, 100, stop != 0, 0));
        const std::int32_t graphic = cg::yobi::GetYobiActionField20(action);
        if (graphic != 0) {
            const render::GraphicDescriptor16* const descriptor = cg::yobi::ApplyWindowAnimationDrawMode(action);
            WindowInstance* const window = currentWindow;
            elementsWindow = window;
            auto* const element35 = elementsWindow->FindElementById(WindowGatherElementId::Element3);
            if (element35) {
                const std::int32_t y = element35->y + window->y;
                auto* const element36 = elementsWindow->FindElementById(WindowGatherElementId::Element3);
                if (element36) {
                    const std::int32_t x = element36->x + window->x;
                    static_cast<void>(render::DrawGraphic2_521DD0(x, y, 0xDF, graphic, 0, 0, descriptor));
                }
            }
        }
    }
    elementsWindow = currentWindow;
    auto* const element23 = elementsWindow->FindElementById(WindowGatherElementId::Element2);
    if (element23) {
        if (element23->visible == 0) {
            auto* const element42 = elementsWindow->FindElementById(WindowGatherElementId::Element4);
            if (element42) {
                cg::yobi::YobiAction* const back = element42->data.animation->action;
                const render::GraphicDescriptor16* const descriptor = cg::yobi::ApplyWindowAnimationDrawMode(back);
                WindowInstance* const window = currentWindow;
                const std::int32_t backGraphic = cg::yobi::GetYobiActionField20(back);
                elementsWindow = window;
                auto* const element43 = elementsWindow->FindElementById(WindowGatherElementId::Element4);
                if (element43) {
                    const std::int32_t y = element43->y + window->y;
                    auto* const element44 = elementsWindow->FindElementById(WindowGatherElementId::Element4);
                    if (element44) {
                        const std::int32_t x = element44->x + window->x;
                        static_cast<void>(render::DrawGraphic2_521DD0(x, y, 0xDF, backGraphic, 0, 0, descriptor));
                    }
                }
            }
        }
    }
    if (auto* const element24 = currentWindow->FindElementById(WindowGatherElementId::Element2); element24) element24->visible = 0;
    if (auto* const element42 = currentWindow->FindElementById(WindowGatherElementId::Element4); element42) element42->visible = 0;
    return 1;
}

// 原函数：0x00486CF0。
static std::int32_t WindowGather_ButtonContinue(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    auto* elementsWindow = currentWindow;
    const std::int32_t level = std::bit_cast<std::int8_t>(gWindow12SkillLevelCB0608);
    auto* const graphicElement = elementsWindow->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t skill = std::bit_cast<std::int8_t>(gWindow12SkillIndexCA0470);
        const std::int32_t cost = world::gSkillsF1BAFC[skill].techniques3C[level].value7C
            * world::gSkillsF1BAFC[skill].fpReduce / 100;
        const bool enoughFp = !(world::gPlayerRuntimeSlots.fp < cost);
        if (!enoughFp && (state & 1) != 0) {
            gHoverTextD68AE8 = kWindow12NoFp5864F0;
        }
        if (world::CountFreeInventorySlots_518C60() <= 0 || !enoughFp) {
            graphic->imageNo = 0x3B56C;
            return 0;
        }
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = L"重新再开。" /* 原常量参考：0x005869F4。 */;
        }
        std::int32_t consumed = 0;
        if ((state & 2) != 0) {
            const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(WindowProfile_SaveOrigin(x, y));
            static_cast<void>(WindowGather_InitializeAction(currentWindow));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3B56B;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B56D;
        }
        if ((state & 8) != 0) {
            graphic->imageNo = 0x3B56C;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00486E50。
static std::int32_t WindowGather_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    WindowInstance* window = currentWindow;
    auto* const graphicElement = window->FindElementById(NativeElementKey(index));
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & 1) != 0) {
            gHoverTextD68AE8 = gWindow12ModeCC3E80 == 0 ? kWindow13Text5868EC : L"暂时中断。" /* 原常量参考：0x00586A00。 */;
            window = currentWindow;
        }
        std::int32_t consumed = 0;
        if ((state & 2) != 0) {
            if (gWindow12ModeCC3E80 == 0) {
                window->state |= 0x8000u;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
            } else {
                static_cast<void>(WindowGather_Cancel(window));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        graphic->imageNo = 0x3B565;
        if ((state & 1) != 0) {
            graphic->imageNo = 0x3B567;
        }
        if ((state & 8) != 0) {
            graphic->imageNo = 0x3B566;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00486F20。
static std::int32_t WindowGather_DrawResultImage(const std::int32_t index, const std::int32_t)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    const std::int32_t resultImage = gWindow12ValueCF3FF0;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (resultImage >= 0) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(NativeElementKey(index));
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t y = element->y + window->y + 24;
            const std::int32_t x = element->x + window->x + 24;
            static_cast<void>(cg::render::DrawGraphic(x, y, 0xDF, resultImage, 0, &descriptor));
        }
    }
    WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(NativeElementKey(index));
    if (elementPtr) {
        const WindowElement* const element = elementPtr;
        const std::int32_t y = element->y + window->y + 24;
        const std::int32_t x = element->x + window->x + 24;
        static_cast<void>(cg::render::DrawGraphic(x, y, 0xDF, 0x3B5C3, 0, &descriptor));
    }
    return 0;
}

} // namespace detail

} // namespace cg::ui
