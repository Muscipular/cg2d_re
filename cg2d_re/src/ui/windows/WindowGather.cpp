#include "cg/session/ClientSession.hpp"
#include "cg/network/CgConnection.hpp"
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
static WindowAnimationData* WindowGather_Cancel(session::ClientSession& session, WindowInstance* window); // 取消窗口12操作并归空闲，保留末动画载荷返回。

// 原函数：0x00485890。
static WindowAnimationData* WindowGather_InitializeAction(session::ClientSession& session, WindowInstance* window); // 窗口12计时/操作初始化。

// 原函数：0x00486CF0。
static std::int32_t WindowGather_ButtonContinue(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // FP/背包检查与重启。

// 原函数：0x00486E50。
static std::int32_t WindowGather_ButtonClose(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 关闭或取消，hover后重取窗口。

// 原函数：0x00486F20。
static std::int32_t WindowGather_DrawResultImage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState); // 结果图及固定框，恒返0。

// 原函数：0x00485AF0。
static WindowAnimationData* WindowGather_ShowResult(session::ClientSession& session, WindowInstance* window); // 窗口12完整结果展示及双计时槽发布。

// 原函数：0x00486020。
static WindowAnimationData* WindowGather_RefreshResult(session::ClientSession& session); // 仅更新已有窗口12，不自动创建。

// 原函数：0x00485690。
static void WindowGather_Release(WindowInstance&);

// 原函数：0x00486040。
static std::int32_t WindowGather_Update(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

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
cg::yobi::YobiAction* WindowGather_Open(session::ClientSession& session, const std::int32_t skill, const std::int32_t level)
{
    auto& skillOperation = session.UiFrame().native.skillOperation;
    auto& operations = session.UiFrame().native.operations;
    auto& gather = session.UiFrame().native.gather;
    const auto skillByte = static_cast<std::uint8_t>(skill);
    gather.skillIndex = skillByte;
    gather.skillLevel = static_cast<std::uint8_t>(level);
    const std::int32_t skillId =
        session.WorldScene().playerSkills.skills[std::bit_cast<std::int8_t>(skillByte)].skillId;
    skillOperation.sendMode = 0;
    gather.extra = 0;
    switch (skillId) {
        case 225:
        case 251:
            gather.kind = 0;
            break;
        case 226:
        case 253:
            gather.kind = 1;
            break;
        case 227:
        case 250:
            gather.kind = 2;
            break;
        case 2000:
        case 2003:
            gather.kind = 3;
            break;
        default:
            break;
    }
    const std::int32_t y = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
    const std::int32_t x = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
    static_cast<void>(WindowProfile_SaveOrigin(session, x, y));
    skillOperation.extraDelay = 2000;
    WindowRuntime_IncrementOperationCount(session);
    WindowRuntime_IncrementGatherOperationCount(session);
    const auto references = static_cast<std::uint8_t>(operations.skillCount + 1);
    const auto skillReferences = static_cast<std::uint8_t>(operations.statusIconReferences + 1);
    operations.skillCount = references;
    operations.statusIconReferences = skillReferences;
    std::int32_t mode = 0;
    switch (session.WorldScene().playerSkills.skills[std::bit_cast<std::int8_t>(gather.skillIndex)].skillId) {
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
    if (mode != 0 && world::GetPlayerStatusIcon_51BEC0(session) != mode) {
        static_cast<void>(world::RequestPlayerStatusIcon_51BE50(session, mode));
    }
    for (const std::int32_t id : {11, 12, 45, 46, 13}) {
        WindowInstance* const window = WindowRuntime_States(session)[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    return WindowRuntime_CreateAnimation(session, WindowId_Gather, 1, 0, nullptr);
}

// 原函数：0x00485690。
static void WindowGather_Release(WindowInstance& releasedWindow)
{
    auto& operations = releasedWindow.ownerSession->UiFrame().native.operations;
    auto& session = *releasedWindow.ownerSession;
    auto& gather = session.UiFrame().native.gather;
    WindowRuntime_DecrementOperationCount(*releasedWindow.ownerSession);
    const std::uint8_t mode = gather.mode;
    if (mode == 1 || mode == 2) {
        WindowRuntime_DecrementGatherOperationCount(*releasedWindow.ownerSession);
    }
    const std::uint8_t previousReferences = operations.skillCount;
    WindowInstance* const window = WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_Gather].window;
    operations.skillCount = static_cast<std::uint8_t>(previousReferences - 1);
    window->state |= 0x8000u;
    // 原4856C1..4856CD：W8减后发布，分支消费同一结果。
    const auto skillReferences = static_cast<std::uint8_t>(operations.statusIconReferences - 1);
    operations.statusIconReferences = skillReferences;
    if (skillReferences == 0) {
        const std::int32_t state = world::GetPlayerStatusIcon_51BEC0(*releasedWindow.ownerSession);
        if ((state >= 5 && state <= 7) || state == 16) {
            static_cast<void>(world::RequestPlayerStatusIcon_51BE50(*releasedWindow.ownerSession, 0));
        }
    }
}

// 原函数：0x00485700。
static WindowAnimationData* WindowGather_Cancel(session::ClientSession& session, WindowInstance* const window)
{
    auto& skillOperation = session.UiFrame().native.skillOperation;
    auto& gather = session.UiFrame().native.gather;
    const std::uint8_t mode = gather.mode;
    if (mode != 0 && mode != 3 && mode != 4) {
        WindowRuntime_DecrementGatherOperationCount(session);
    }
    gather.mode = 0;
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
    gather.resultImage = -1;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 2, 1, 3, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    WindowAnimationData* const data = WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 1, 0, 5);
    skillOperation.sendMode = 0;
    gather.extra = 0;
    return data;
}

// 原函数：0x00485890。
static WindowAnimationData* WindowGather_InitializeAction(session::ClientSession& session, WindowInstance* const window)
{
    auto& skillOperation = session.UiFrame().native.skillOperation;
    auto& gather = session.UiFrame().native.gather;
    if (gather.mode != 1) {
        WindowRuntime_IncrementGatherOperationCount(session);
    }
    const std::int32_t previousValue = gather.extra;
    gather.mode = 1;
    skillOperation.sendMode = 0;
    if (previousValue > 0) {
        const std::int32_t imageNo = gather.petImage;
        skillOperation.sendMode = 1;
        skillOperation.petImage = imageNo;
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
    gather.resultImage = -1;
    gather.flag = 0;
    const std::uint64_t tick = platform::ReadTickCount64();
    std::uint32_t duration = skillOperation.extraDelay + 4500u;
    gather.tick = tick;
    const std::uint8_t rebirthMode = skillOperation.rebirthEnabled;
    gather.deadline = duration;
    if (rebirthMode != 0) {
        const std::int32_t rebirth = session.WorldScene().player->value08;
        duration = duration * kSkillRebirthRates57D7A4[rebirth] / 100u;
        gather.deadline = duration;
    }
    if (skillOperation.sendMode != 0) {
        duration = duration * kSkillCrystalRates57D7B4[gather.crystalLevel] / 100u;
    }
    const std::uint64_t deadline = duration + tick;
    skillOperation.previewTick = tick;
    gather.deadline = deadline;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 2, 1, 3, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    if (skillOperation.sendMode != 0) {
        return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 1, 0, 5);
    }
    return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 0, 10, 5);
}

// 原函数：0x00485AF0。
static WindowAnimationData* WindowGather_ShowResult(session::ClientSession& session, WindowInstance* const window)
{
    auto& skillOperation = session.UiFrame().native.skillOperation;
    auto& gather = session.UiFrame().native.gather;
    std::uint8_t mode = gather.mode;
    if (mode != 0 && mode != 3 && mode != 4) {
        WindowRuntime_DecrementGatherOperationCount(session);
        mode = gather.mode;
    }
    gather.mode = mode == 0 ? 4 : 3;
    if (auto* const element5 = window ? window->FindElementById(WindowGatherElementId::Element5) : nullptr; element5) element5->visible = 1;
    if (auto* const element6 = window ? window->FindElementById(WindowGatherElementId::Element6) : nullptr; element6) element6->visible = 1;
    if (auto* const element7 = window ? window->FindElementById(WindowGatherElementId::Element7) : nullptr; element7) element7->visible = 1;
    if (auto* const element8 = window ? window->FindElementById(WindowGatherElementId::Element8) : nullptr; element8) element8->visible = 1;
    if (auto* const element9 = window ? window->FindElementById(WindowGatherElementId::Element9) : nullptr; element9) element9->visible = 0;
    if (auto* const element10 = window ? window->FindElementById(WindowGatherElementId::Element10) : nullptr; element10) element10->visible = 1;
    if (auto* const drawResultImageElement = window ? window->FindElementById(WindowGatherElementId::DrawResultImage) : nullptr; drawResultImageElement) drawResultImageElement->visible = 1;
    if (auto* const buttonContinueElement = window ? window->FindElementById(WindowGatherElementId::ButtonContinue) : nullptr; buttonContinueElement) buttonContinueElement->visible = 0;
    if (auto* const buttonClose_13Element = window ? window->FindElementById(WindowGatherElementId::ButtonClose_13) : nullptr; buttonClose_13Element) buttonClose_13Element->visible = 1;
    if (gather.result != 0) {
        if (auto* const element52 = window ? window->FindElementById(WindowGatherElementId::Element5) : nullptr; element52) element52->data.text->text = gather.name;
        if (auto* const element62 = window ? window->FindElementById(WindowGatherElementId::Element6) : nullptr; element62) element62->data.text->text = L"　发现了。" /* 原常量参考：0x005869D4。 */;
        static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
    } else {
        if (auto* const element52 = window ? window->FindElementById(WindowGatherElementId::Element5) : nullptr; element52) element52->data.text->text = L"什么都没发现。" /* 原常量参考：0x005869C4。 */;
        if (auto* const element62 = window ? window->FindElementById(WindowGatherElementId::Element6) : nullptr; element62) element62->data.text->text = L"";
        static_cast<void>(audio::PlaySe_52C450(session, 58, 320));
        gather.resultImage = -1;
    }
    if (gather.levelUp != 0) {
        auto* const element72 = window ? window->FindElementById(WindowGatherElementId::Element7) : nullptr;
        if (element72) {
            (element72->data.text->text = kCraftLevelUp58661C);
        }
    } else {
        auto* const element72 = window ? window->FindElementById(WindowGatherElementId::Element7) : nullptr;
        if (element72) {
            auto* const data = element72->data.text;
            cg::text::FormatTo(data->text, kCraftExperience58660C, gather.experience);
        }
    }
    auto* const element82 = window ? window->FindElementById(WindowGatherElementId::Element8) : nullptr;
    if (element82) {
        std::wstring& increments = element82->data.text->text;
        increments.clear();
        if (gather.endurance != 0) {
            increments.append(std::format(kCraftEndurance586600, gather.endurance));
        }
        if (gather.dexterity != 0) {
            increments.append(std::format(kCraftDexterity5865F4, gather.dexterity));
        }
        if (gather.intelligence != 0) {
            increments.append(std::format(kCraftIntelligence5865E8, gather.intelligence));
        }
    }
    if (auto* const element3 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr; element3) element3->data.animation->value04 = 1;
    const bool succeeded = gather.result != 0;
    const std::int32_t defaultAction = succeeded ? 5 : 10;
    const std::int32_t alternateAction = succeeded ? 18 : 15;
    auto* const element32 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr;
    if (element32) {
        cg::yobi::SetYobiActionField2B0(
            element32->data.animation->action, defaultAction);
    }
    std::int32_t imageNo = session.WorldScene().player->imageNo;
    for (const auto& range : kCraftAlternateActionImages) {
        if (imageNo >= range[0] && imageNo <= range[1]) {
            auto* const element33 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr;
            if (element33) {
                cg::yobi::SetYobiActionField2B0(
                    element33->data.animation->action, alternateAction);
            }
            imageNo = session.WorldScene().player->imageNo;
        }
    }
    if (imageNo == 0x18A26) {
        auto* const element33 = window ? window->FindElementById(WindowGatherElementId::Element3) : nullptr;
        if (element33) {
            cg::yobi::SetYobiActionField2B0(
                element33->data.animation->action, defaultAction);
        }
    }
    gather.flag = 0;
    const std::uint64_t tick = platform::ReadTickCount64();
    gather.tick = tick;
    skillOperation.previewTick = tick;
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 2, 1, 3, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    if (skillOperation.sendMode != 0) {
        return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 1, gather.result != 0 ? 6 : 8, 5);
    }
    return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 0, 10, 5);
}

// 原函数：0x00486020。
static WindowAnimationData* WindowGather_RefreshResult(session::ClientSession& session)
{
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_Gather].window;
    return window != nullptr ? WindowGather_ShowResult(session, window) : nullptr;
}

// 原函数：0x004ED6B0。
WindowAnimationData* WindowGather_ReceiveResult(session::ClientSession& session, const std::int32_t result, const char* const text)
{
    auto& gather = session.UiFrame().native.gather;
    gather.result = result;
    gather.details.clear();
    gather.extra = 0;
    char buffer[768];
    if (result == 0) {
        gather.experience = 0;
        // 原四个dword文字复制后清两槽，再复制末尾LF/NUL；string只保存逻辑文本。
        gather.details += LR"(没看到任何东西。)" /* 原常量参考：0x005897DC。 */;
        gather.levelUp = 0;
        gather.endurance = 0;
        // 原两段拼接并为完整UTF-16正文追加。
        gather.dexterity = 0;
        gather.intelligence = 0;
        return WindowGather_RefreshResult(session);
    }
    gather.experience = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    gather.value = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    gather.levelUp = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    gather.endurance = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    gather.dexterity = network::ParseDelimitedInteger_52CFA0(text, '|', 5);
    gather.intelligence = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    gather.resultImage = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 8, 255, buffer));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
    gather.name = std::strlen(buffer) <= 28 ? cg::text::FromGbkWide(buffer) : kWindow12Unknown554590;
    gather.extra = network::ParseDelimitedInteger_52CFA0(text, '|', 9);
    const std::int32_t experience = gather.experience;
    gather.suffix = L"得到了！" /* 原常量参考：0x00589830。 */;
    std::int32_t lines = 0;
    const auto appendValue = [&](const std::wstring_view format, const std::int32_t value) {
        // 四种数值行按原GBK最多22字节，始终低于28列换行门槛。
        gather.details += std::vformat(format, std::make_wformat_args(value)) + L'\n';
        ++lines;
    };
    if (experience > 0) {
        appendValue(kCraftExperience589820, experience);
    }
    if (gather.levelUp != 0 && lines < 8) {
        gather.details += kCraftLevelUp589814;
        ++lines;
        static_cast<void>(audio::PlaySe_52C450(session, 64, 320));
    }
    if (gather.endurance != 0 && lines < 8) {
        appendValue(kCraftEndurance589808, gather.endurance);
    }
    if (gather.dexterity != 0 && lines < 8) {
        appendValue(kCraftDexterity5897FC, gather.dexterity);
    }
    if (gather.intelligence != 0 && lines < 8) {
        appendValue(kCraftIntelligence5897F0, gather.intelligence);
    }
    return WindowGather_RefreshResult(session);
}

// 原函数：0x00486040。
static std::int32_t WindowGather_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& skillOperation = callbackWindow.ownerSession->UiFrame().native.skillOperation;
    auto& operations = callbackWindow.ownerSession->UiFrame().native.operations;
    auto& session = *callbackWindow.ownerSession;
    auto& gather = session.UiFrame().native.gather;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    if (value == -1) {
        operations.gatherOpening = nullptr;
        gather.mode = 0;
        WindowRuntime_DecrementGatherOperationCount(*callbackWindow.ownerSession);
        static_cast<void>(WindowGather_InitializeAction(session, currentWindow));
        return 1;
    }
    const std::int32_t mapped = character_select::ResolveCharacterImageForVersion(*callbackWindow.ownerSession, callbackWindow.ownerSession->WorldScene().player->imageNo);
    const std::int32_t kind = std::bit_cast<std::int8_t>(gather.kind);
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
            element32->data.animation->action, cg::yobi::YobiDirection_Down);
    }
    std::uint8_t mode;
    if (skillOperation.sendMode != 0) {
        const std::int32_t imageNo = WindowSkill_CanUseSpecialSkill(*callbackWindow.ownerSession) != 0 ? skillOperation.petImage : 0;
        static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 1, 3, imageNo));
        mode = gather.mode;
        if (mode == 1) {
            static_cast<void>(WindowCraft_AnimatePreviewPositions(currentWindow, 2, 1, 3, gather.deadline));
            mode = gather.mode;
        }
    } else {
        static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 1, 3, 0));
        mode = gather.mode;
    }
    if (mode == 1) {
        // 四条阶段路径共用原6/16选择；每次命中仍重取当前窗口和玩家图号。
        const auto setWorkingAction = [currentWindow](cg::yobi::YobiAction* const initialAction) {
            cg::yobi::SetYobiActionField2B0(initialAction, 6);
            std::int32_t imageNo = currentWindow->ownerSession->WorldScene().player->imageNo;
            for (const auto& range : kCraftAlternateActionImages) {
                if (imageNo >= range[0] && imageNo <= range[1]) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::SetYobiActionField2B0(
                            element33->data.animation->action, 16);
                    }
                    imageNo = currentWindow->ownerSession->WorldScene().player->imageNo;
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
        switch (std::bit_cast<std::int8_t>(gather.kind)) {
            case 0:
            case 1:
                if (gather.flag == 0) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::SetYobiActionField2B0(
                            element33->data.animation->action, 1);
                    }
                    if (auto* const element34 = currentWindow->FindElementById(WindowGatherElementId::Element3); element34) element34->data.animation->value04 = 0;
                    ++gather.flag;
                } else if (gather.flag == 1) {
                    const std::uint64_t tick = platform::ReadTickCount64();
                    if (gather.tick + 2000u < tick) {
                        auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                        if (element33) {
                            cg::yobi::SetYobiActionField2B0(
                                element33->data.animation->action, 5);
                        }
                        ++gather.flag;
                    }
                } else if (gather.flag == 2) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::YobiAction* const action =
                            element33->data.animation->action;
                        if (cg::yobi::GetYobiActionField2DC(action) >= 2) {
                            setWorkingAction(action);
                            ++gather.flag;
                        }
                    }
                }
                break;
            case 2:
                if (gather.flag == 0) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::SetYobiActionField2B0(
                            element33->data.animation->action, 5);
                    }
                    if (auto* const element34 = currentWindow->FindElementById(WindowGatherElementId::Element3); element34) element34->data.animation->value04 = 0;
                    ++gather.flag;
                } else if (gather.flag == 1) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::YobiAction* const action =
                            element33->data.animation->action;
                        if (cg::yobi::GetYobiActionField2DC(action) >= 2) {
                            setWorkingAction(action);
                            ++gather.flag;
                        }
                    }
                }
                [[fallthrough]]; // 原486562递增后落入486568，再检查同一阶段。
            case 3:
                if (gather.flag == 0) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::SetYobiActionField2B0(
                            element33->data.animation->action, 5);
                    }
                    if (auto* const element34 = currentWindow->FindElementById(WindowGatherElementId::Element3); element34) element34->data.animation->value04 = 0;
                    ++gather.flag;
                } else if (gather.flag == 1) {
                    auto* const element33 = currentWindow->FindElementById(WindowGatherElementId::Element3);
                    if (element33) {
                        cg::yobi::YobiAction* const action =
                            element33->data.animation->action;
                        if (cg::yobi::GetYobiActionField2DC(action) >= 2) {
                            setWorkingAction(action);
                            ++gather.flag;
                        }
                    }
                }
                break;
            default:
                break;
        }
        const std::uint64_t tick = platform::ReadTickCount64();
        if (gather.deadline < tick) {
            const std::uint8_t sendMode = skillOperation.rebirthEnabled;
            skillOperation.extraDelay = 0;
            std::int32_t skill;
            std::int32_t level;
            if (sendMode != 0) {
                level = std::bit_cast<std::int8_t>(gather.skillLevel);
                skill = std::bit_cast<std::int8_t>(gather.skillIndex) + 100;
            } else {
                level = std::bit_cast<std::int8_t>(gather.skillLevel);
                static_cast<void>(skillOperation.sendMode); // 原cmp未被后续分支消费，没有+200。
                skill = std::bit_cast<std::int8_t>(gather.skillIndex);
            }
            static_cast<void>(callbackWindow.ownerSession->Connection().SendSkillUseRequest(skill, level, -1, L""));
            gather.petImage = WindowSkill_ReadBattlePetImage(*callbackWindow.ownerSession);
            gather.crystalLevel = WindowSkill_ReadCrystalLevel(*callbackWindow.ownerSession);
            gather.mode = 2;
        } else if (WindowProfile_CheckDistance(*callbackWindow.ownerSession, 1, 1) != 0) {
            static_cast<void>(WindowGather_Cancel(session, currentWindow));
        }
    } else if (mode == 3) {
        const std::uint64_t tick = platform::ReadTickCount64();
        if (gather.tick + 2000u < tick) {
            const std::int32_t skill = std::bit_cast<std::int8_t>(gather.skillIndex);
            const std::int32_t level = std::bit_cast<std::int8_t>(gather.skillLevel);
            const std::int32_t cost = callbackWindow.ownerSession->WorldScene().playerSkills.skills[skill].techniques3C[level].value7C
                * callbackWindow.ownerSession->WorldScene().playerSkills.skills[skill].fpReduce / 100;
            if ((callbackWindow.ownerSession->WorldScene().player->fp < cost)) {
                world::AppendChatMessage_44B050(*callbackWindow.ownerSession, kWindow12NoFp5864F0, 4, 3);
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 58, 320));
                static_cast<void>(WindowGather_Cancel(session, currentWindow));
            } else if (world::CountFreeInventorySlots_518C60(*callbackWindow.ownerSession) <= 0) {
                world::AppendChatMessage_44B050(*callbackWindow.ownerSession, L"物品栏没有空位。" /* 原常量参考：0x005869E0。 */, 4, 3);
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 55, 320));
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 58, 320));
                static_cast<void>(WindowGather_Cancel(session, currentWindow));
            } else {
                static_cast<void>(WindowGather_InitializeAction(session, currentWindow));
            }
        }
    } else if (mode == 4) {
        const std::uint64_t tick = platform::ReadTickCount64();
        if (gather.tick + 2000u < tick) {
            static_cast<void>(WindowGather_Cancel(session, currentWindow));
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
        const std::uint8_t currentMode = gather.mode;
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
            cg::yobi::SetYobiActionMixFlagsOverrideMask(action, render::GraphicMixFlag_OriginalPalette);
            cg::yobi::SetYobiActionMixFlagsOverrideValue(action, render::GraphicMixFlag_OriginalPalette);
        } else {
            cg::yobi::SetYobiActionMixFlagsOverrideMask(action, render::GraphicMixFlag_None);
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
static std::int32_t WindowGather_ButtonContinue(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& session = *callbackWindow.ownerSession;
    auto& gather = session.UiFrame().native.gather;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* elementsWindow = currentWindow;
    const std::int32_t level = std::bit_cast<std::int8_t>(gather.skillLevel);
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t skill = std::bit_cast<std::int8_t>(gather.skillIndex);
        const std::int32_t cost = callbackWindow.ownerSession->WorldScene().playerSkills.skills[skill].techniques3C[level].value7C
            * callbackWindow.ownerSession->WorldScene().playerSkills.skills[skill].fpReduce / 100;
        const bool enoughFp = !(callbackWindow.ownerSession->WorldScene().player->fp < cost);
        if (!enoughFp && (state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = kWindow12NoFp5864F0;
        }
        if (world::CountFreeInventorySlots_518C60(*callbackWindow.ownerSession) <= 0 || !enoughFp) {
            graphic->imageNo = 0x3B56C;
            return 0;
        }
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"重新再开。" /* 原常量参考：0x005869F4。 */;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t y = static_cast<std::int32_t>(callbackWindow.ownerSession->WorldScene().map.crypt->playerY);
            const std::int32_t x = static_cast<std::int32_t>(callbackWindow.ownerSession->WorldScene().map.crypt->playerX);
            static_cast<void>(WindowProfile_SaveOrigin(*callbackWindow.ownerSession, x, y));
            static_cast<void>(WindowGather_InitializeAction(session, currentWindow));
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3B56B;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B56D;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B56C;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00486E50。
static std::int32_t WindowGather_ButtonClose(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& session = *callbackWindow.ownerSession;
    auto& gather = session.UiFrame().native.gather;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = gather.mode == 0 ? kWindow13Text5868EC : L"暂时中断。" /* 原常量参考：0x00586A00。 */;
            window = currentWindow;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            if (gather.mode == 0) {
                window->state |= 0x8000u;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
            } else {
                static_cast<void>(WindowGather_Cancel(session, window));
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
            }
            consumed = 1;
        }
        graphic->imageNo = 0x3B565;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B567;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B566;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00486F20。
static std::int32_t WindowGather_DrawResultImage(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t)
{
    auto& session = *callbackWindow.ownerSession;
    auto& gather = session.UiFrame().native.gather;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t resultImage = gather.resultImage;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (resultImage >= 0) {
        WindowInstance* const window = currentWindow;
        auto* const elementPtr = window->FindElementById(elementId);
        if (elementPtr) {
            const WindowElement* const element = elementPtr;
            const std::int32_t y = element->y + window->y + 24;
            const std::int32_t x = element->x + window->x + 24;
            static_cast<void>(cg::render::DrawGraphic(x, y, 0xDF, resultImage, 0, &descriptor));
        }
    }
    WindowInstance* const window = currentWindow;
    auto* const elementPtr = window->FindElementById(elementId);
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
