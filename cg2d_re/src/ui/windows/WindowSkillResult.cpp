#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowSkillResultElementId : ElementId {
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
    ButtonContinue = 11,
    ButtonClose_12 = 12,
    Element13 = 13,
    HandleSideArea = 14,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004875E0。
static cg::yobi::YobiAction* WindowSkillResult_Open(session::ClientSession& session); // 原操作计数、关联关闭及窗口13创建。

// 原函数：0x004876B0。
static WindowAnimationData* WindowSkillResult_RefreshResult(session::ClientSession& session, WindowInstance* window); // 窗口13按分类完整填充结果。

// 原函数：0x00487D60。
static std::int32_t WindowSkillResult_ButtonContinue(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 先比较FP，再继续技能请求。

// 原函数：0x00487E80。
static std::int32_t WindowSkillResult_ButtonClose(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 关闭入口窗口，音效后写图号。

// 原函数：0x00487660。
static void WindowSkillResult_Release(WindowInstance&);

// 原函数：0x00487AE0。
static std::int32_t WindowSkillResult_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x00487B80。
static std::int32_t WindowSkillResult_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic5628A8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005628A8。

static WindowGraphicData gWindowGraphic5628C0 {0x3BA3F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005628C0。

static WindowGraphicData gWindowGraphic5628D8 {0x3BA3E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005628D8。

static WindowGraphicData gWindowGraphic5628F0 {0x3BA31, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005628F0。

static WindowGraphicData gWindowGraphic562908 {0x3B565, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562908。

static WindowTextPrototype gWindowText562920 { // 原全局：0x00562920。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？？？", // 原常量参考：0x00586060。
};

static std::int32_t gWindowAnimationImage562928 = 0x1AF16; // 原全局：0x00562928。

static std::int32_t gWindowAnimationImage56292C = 0x1AF16; // 原全局：0x0056292C。

static WindowElementPrototype gWindowElements562930[14] { // 原全局：0x00562930。
    {WindowElementType_Graphic, 178, 9, 11, 11, 1, {.graphic = &gWindowGraphic5628A8}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowSkillResultElementId::ButtonClose)},
    {WindowElementType_Animation, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56292C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element2)},
    {WindowElementType_Animation, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage562928}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element3)},
    {WindowElementType_Animation, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56292C}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element4)},
    {WindowElementType_Text, 25, 28, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element5)},
    {WindowElementType_Text, 25, 42, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element6)},
    {WindowElementType_Text, 25, 56, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element7)},
    {WindowElementType_Text, 25, 70, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element8)},
    {WindowElementType_Graphic, 24, 28, 0, 0, 1, {.graphic = &gWindowGraphic5628D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element9)},
    {WindowElementType_Graphic, 24, 55, 0, 0, 1, {.graphic = &gWindowGraphic5628D8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element10)},
    {WindowElementType_Graphic, 29, 203, 49, 17, 1, {.graphic = &gWindowGraphic5628F0}, &WindowSkillResult_ButtonContinue, -1, static_cast<ElementId>(WindowSkillResultElementId::ButtonContinue)},
    {WindowElementType_Graphic, 129, 203, 49, 17, 1, {.graphic = &gWindowGraphic562908}, &WindowSkillResult_ButtonClose, -1, static_cast<ElementId>(WindowSkillResultElementId::ButtonClose_12)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5628C0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowSkillResultElementId::Element13)},
    {WindowElementType_EventArea, 201, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea, -1, static_cast<ElementId>(WindowSkillResultElementId::HandleSideArea)},
};

// 原型：窗口13；原全局：0x0054E4B0。
WindowPrototype gWindowPrototypeSkillResult {0, 4, 383, 118, 198, 232, 0x80010101u, 14, gWindowElements562930, &WindowSkillResult_Update, &WindowSkillResult_Draw, &WindowSkillResult_Release}; // 13: 54E4B0

// 窗口函数实现。

// 原函数：0x00487F30。
std::int32_t WindowSkillResult_ReadPreviewImage(const std::int32_t imageNo, const std::int32_t offset)
{
    if (imageNo >= 0x18A24 && imageNo <= 0x18A26) {
        return imageNo;
    }
    for (std::int32_t index = 0; index < 4; ++index) {
        const auto& range = kCraftAlternateActionImages[index];
        if (imageNo >= range[0] && imageNo <= range[1]) {
            const std::int32_t relative = imageNo - range[0];
            const std::int32_t groupSize = index == 2 ? 30 : 25;
            const std::int32_t group = relative / groupSize * groupSize;
            return range[0] + 2 + group + (relative - group) / 6 * 6 + offset;
        }
    }
    if (imageNo >= 0x1D55C && imageNo <= 0x1D563) {
        return imageNo;
    }
    if (imageNo >= 0x1AF0E && imageNo <= 0x1AF25) {
        return 0x1AF10 + (imageNo - 0x1AF0E) / 6 * 6 + offset;
    }
    return imageNo;
}

// 原函数：0x004875E0。
static cg::yobi::YobiAction* WindowSkillResult_Open(session::ClientSession& session)
{
    auto& operations = session.UiFrame().native.operations;
    WindowRuntime_IncrementOperationCount(session);
    const auto references = static_cast<std::uint8_t>(operations.skillCount + 1);
    const auto skillReferences = static_cast<std::uint8_t>(operations.statusIconReferences + 1);
    operations.skillCount = references;
    WindowInstance* const previous = WindowRuntime_States(session)[WindowId_Craft].window;
    operations.statusIconReferences = skillReferences;
    if (previous != nullptr) {
        previous->state |= 0x8000u;
    }
    for (const std::int32_t id : {12, 45, 46, 13}) {
        WindowInstance* const window = WindowRuntime_States(session)[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    return WindowRuntime_CreateAnimation(session, WindowId_SkillResult, 1, 0, nullptr);
}

// 原函数：0x00487660。
static void WindowSkillResult_Release(WindowInstance& releasedWindow)
{
    auto& operations = releasedWindow.ownerSession->UiFrame().native.operations;
    WindowRuntime_DecrementOperationCount(*releasedWindow.ownerSession);
    const auto references = static_cast<std::uint8_t>(operations.skillCount - 1);
    const auto skillReferences = static_cast<std::uint8_t>(operations.statusIconReferences - 1);
    operations.skillCount = references;
    operations.statusIconReferences = skillReferences;
    if (skillReferences == 0 && world::GetPlayerStatusIcon_51BEC0(*releasedWindow.ownerSession) == 13) {
        static_cast<void>(world::RequestPlayerStatusIcon_51BE50(*releasedWindow.ownerSession, 0));
    }
    WindowRuntime_States(*releasedWindow.ownerSession)[WindowId_SkillResult].window->state |= 0x8000u;
}

// 原函数：0x004876B0。
static WindowAnimationData* WindowSkillResult_RefreshResult(session::ClientSession& session, WindowInstance* const window)
{
    auto& request = session.UiFrame().native.skillRequest;
    auto& resultState = session.UiFrame().native.skillResult;
    const std::int32_t kind = request.targetKind;
    switch (kind) {
        case 0:
        case 1: {
            auto* const element5 = window ? window->FindElementById(WindowSkillResultElementId::Element5) : nullptr;
            if (element5) {
                cg::text::FormatTo(element5->data.text->text, L"对{:<22}" /* 原常量参考：0x00586B00。 */, request.targetName);
            }
            const wchar_t* text = nullptr;
            if (kind == 0) {
                text = resultState.result != 0 ? L"　变装成功了！" /* 原常量参考：0x00586AF0。 */ : L"　变装失败了！" /* 原常量参考：0x00586AE0。 */;
            } else {
                text = resultState.result != 0 ? L"　变身成功了！" /* 原常量参考：0x00586AD0。 */ : L"　变身失败了！" /* 原常量参考：0x00586AC0。 */;
            }
            if (auto* const element6 = window ? window->FindElementById(WindowSkillResultElementId::Element6) : nullptr; element6) element6->data.text->text = text;
            break;
        }
        case 2:
            if (resultState.result != 0) {
                auto* const element5 = window ? window->FindElementById(WindowSkillResultElementId::Element5) : nullptr;
                if (element5) {
                    cg::text::FormatTo(element5->data.text->text, L"{:<16}的治疗成功了" /* 原常量参考：0x00586AAC。 */, request.targetName);
                }
                auto* const element6 = window ? window->FindElementById(WindowSkillResultElementId::Element6) : nullptr;
                if (element6) {
                    cg::text::FormatTo(element6->data.text->text, L"　生命值恢复了 {} 。" /* 原常量参考：0x00586A94。 */, resultState.extra);
                }
            } else {
                auto* const element5 = window ? window->FindElementById(WindowSkillResultElementId::Element5) : nullptr;
                if (element5) {
                    cg::text::FormatTo(element5->data.text->text, L"对{:<22}" /* 原常量参考：0x00586B00。 */, request.targetName);
                }
                if (auto* const element6 = window ? window->FindElementById(WindowSkillResultElementId::Element6) : nullptr; element6) element6->data.text->text = resultState.extra == 0 ? L"　不需进行治疗！" /* 原常量参考：0x00586A80。 */ : L"　治疗失败了！" /* 原常量参考：0x00586A70。 */;
            }
            break;
        case 3:
            if (resultState.result != 0) {
                auto* const element5 = window ? window->FindElementById(WindowSkillResultElementId::Element5) : nullptr;
                if (element5) {
                    cg::text::FormatTo(element5->data.text->text, L"{:<16}的治疗成功了" /* 原常量参考：0x00586AAC。 */, request.targetName);
                }
                if (auto* const element6 = window ? window->FindElementById(WindowSkillResultElementId::Element6) : nullptr; element6) element6->data.text->text = resultState.extra == 4 ? L"伤势全部恢复了！" /* 原常量参考：0x00586A5C。 */ : L"伤势稍微恢复了。" /* 原常量参考：0x00586A48。 */;
            } else if (resultState.extra == 0) {
                auto* const element5 = window ? window->FindElementById(WindowSkillResultElementId::Element5) : nullptr;
                if (element5) {
                    cg::text::FormatTo(element5->data.text->text, L"对{:<22}" /* 原常量参考：0x00586B00。 */, request.targetName);
                }
                if (auto* const element6 = window ? window->FindElementById(WindowSkillResultElementId::Element6) : nullptr; element6) element6->data.text->text = L"　不需进行治疗！" /* 原常量参考：0x00586A80。 */;
            } else {
                auto* const element5 = window ? window->FindElementById(WindowSkillResultElementId::Element5) : nullptr;
                if (element5) {
                    cg::text::FormatTo(element5->data.text->text, L"{:<16}的治疗失败了" /* 原常量参考：0x00586A34。 */, request.targetName);
                }
                const std::int32_t value = resultState.extra;
                auto* const element6 = window ? window->FindElementById(WindowSkillResultElementId::Element6) : nullptr;
                if (element6) {
                    element6->data.text->text = value == 3 ? L"　伤势恶化而掉魂了。" /* 原常量参考：0x00586A1C。 */
                        : value == 2 ? L"　伤势恶化了。" /* 原常量参考：0x00586A0C。 */ : L"";
                }
            }
            break;
        default:
            break;
    }
    static_cast<void>(audio::PlaySe_52C450(resultState.result != 0 ? 57 : 58, 320));
    if (resultState.levelUp != 0) {
        if (auto* const element7 = window ? window->FindElementById(WindowSkillResultElementId::Element7) : nullptr; element7) element7->data.text->text = kCraftLevelUp58661C;
    } else {
        auto* const element7 = window ? window->FindElementById(WindowSkillResultElementId::Element7) : nullptr;
        if (element7) {
            cg::text::FormatTo(element7->data.text->text, kCraftExperience58660C, resultState.experience);
        }
    }
    auto* const element8 = window ? window->FindElementById(WindowSkillResultElementId::Element8) : nullptr;
    if (element8) {
        std::wstring& increments = element8->data.text->text;
        increments.clear();
        if (resultState.endurance != 0) {
            increments.append(std::format(kCraftEndurance586600, resultState.endurance));
        }
        if (resultState.dexterity != 0) {
            increments.append(std::format(kCraftDexterity5865F4, resultState.dexterity));
        }
        if (resultState.intelligence != 0) {
            increments.append(std::format(kCraftIntelligence5865E8, resultState.intelligence));
        }
    }
    if (auto* const element3 = window ? window->FindElementById(WindowSkillResultElementId::Element3) : nullptr; element3) element3->data.animation->value04 = 1;
    auto* const element32 = window ? window->FindElementById(WindowSkillResultElementId::Element3) : nullptr;
    if (element32) {
        cg::yobi::SetYobiActionField2B0(
            element32->data.animation->action,
            resultState.result != 0 ? 5 : 10);
    }
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 2, 1, 3, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 1, resultState.result != 0 ? 6 : 8, 5);
}

// 原函数：0x00487AE0。
static std::int32_t WindowSkillResult_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    auto& skillOperation = callbackWindow.ownerSession->UiFrame().native.skillOperation;
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    if (value == -1) {
        static_cast<void>(WindowSkillResult_RefreshResult(session, currentWindow));
    }
    const std::int32_t imageNo =
        WindowSkillResult_ReadPreviewImage(character_select::ResolveCharacterImageForVersion(callbackWindow.ownerSession->WorldScene().player->imageNo), 0);
    auto* const element3 = currentWindow->FindElementById(WindowSkillResultElementId::Element3);
    if (element3) {
        cg::yobi::SetYobiActionField2A8(
            element3->data.animation->action, imageNo);
    }
    auto* const element32 = currentWindow->FindElementById(WindowSkillResultElementId::Element3);
    if (element32) {
        cg::yobi::SetYobiActionField2B8(
            element32->data.animation->action, cg::yobi::YobiDirection_Down);
    }
    if (skillOperation.sendMode != 0) {
        static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 1, 3, skillOperation.petImage));
    } else {
        static_cast<void>(WindowCraft_SetPreviewImages(currentWindow, 1, 3, 0));
    }
    return 1;
}

// 原函数：0x00487B80。
static std::int32_t WindowSkillResult_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto* const element3 = currentWindow->FindElementById(WindowSkillResultElementId::Element3);
    if (element3) {
        cg::yobi::YobiAction* const action =
            element3->data.animation->action;
        const std::int32_t actionId = cg::yobi::GetYobiActionField2B0(action);
        const std::int32_t imageNo = cg::yobi::GetYobiActionField2A8(action);
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
        if (imageNo >= 0x1AF0E && imageNo <= 0x1AF25) {
            cg::yobi::SetYobiActionMixFlagsOverrideMask(action, render::GraphicMixFlag_OriginalPalette);
            cg::yobi::SetYobiActionMixFlagsOverrideValue(action, render::GraphicMixFlag_OriginalPalette);
        } else {
            cg::yobi::SetYobiActionMixFlagsOverrideMask(action, render::GraphicMixFlag_None);
        }
    }
    WindowRuntime_DrawCurrent(callbackWindow);
    return 1;
}

// 原函数：0x00487D60。
static std::int32_t WindowSkillResult_ButtonContinue(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto& request = session.UiFrame().native.skillRequest;
    auto* elementsWindow = currentWindow;
    const std::int32_t level = request.level;
    auto* const graphicElement = elementsWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const std::int32_t skill = request.index;
        const std::int32_t cost = callbackWindow.ownerSession->WorldScene().playerSkills.skills[skill].techniques3C[level].value7C
            * callbackWindow.ownerSession->WorldScene().playerSkills.skills[skill].fpReduce / 100;
        if ((callbackWindow.ownerSession->WorldScene().player->fp < cost)) {
            graphic->imageNo = 0x3BA32;
            if ((state & MouseFlag_LDownEvent) != 0) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            return 0;
        }
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"继续执行。" /* 原常量参考：0x005868E0。 */;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            const std::int32_t currentSkill = request.index;
            static_cast<void>(session.Connection().RequestSkillTargetAttributes(currentSkill));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3BA31;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3BA33;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3BA32;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00487E80。
static std::int32_t WindowSkillResult_ButtonClose(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    WindowInstance* const window = currentWindow;
    auto* const graphicElement = window->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = kWindow13Text5868EC;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
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

// 原函数：0x004EEA40。
cg::yobi::YobiAction* WindowSkillResult_ReceiveResult(session::ClientSession& session, const std::int32_t result, const char* const text)
{
    auto& request = session.UiFrame().native.skillRequest;
    auto& resultState = session.UiFrame().native.skillResult;
    resultState.result = result;
    resultState.details.clear();
    resultState.experience = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    resultState.value = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    resultState.levelUp = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    resultState.endurance = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    resultState.dexterity = network::ParseDelimitedInteger_52CFA0(text, '|', 5);
    resultState.intelligence = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    const std::int32_t value = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    const std::int32_t kind = request.targetKind;
    resultState.extra = value;
    std::int32_t lines = 0;
    switch (kind) {
        case 0:
            resultState.title = L"使用『变装』技能" /* 原常量参考：0x00589A80。 */;
            resultState.summary = result != 0 ? L"使用『变装』技能，成功了！" /* 原常量参考：0x00589A64。 */ : L"使用『变装』技能，失败了！" /* 原常量参考：0x00589A48。 */;
            lines = 1;
            break;
        case 1:
            resultState.title = L"使用『变身』技能" /* 原常量参考：0x00589A34。 */;
            resultState.summary = result != 0 ? L"使用『变身』技能，成功了！" /* 原常量参考：0x00589A18。 */ : L"使用『变身』技能，失败了！" /* 原常量参考：0x005899FC。 */;
            lines = 1;
            break;
        case 2:
            resultState.title = L"使用『急救』技能" /* 原常量参考：0x005899E8。 */;
            if (result != 0) {
                resultState.summary = L"使用『急救』技能，成功了！" /* 原常量参考：0x005899B8。 */;
                resultState.details += std::format(L"回复了{}点生命力。" /* 原常量参考：0x005899D4。 */, value);
                lines = 1;
            } else {
                resultState.summary = value == 0 ? L"无法实施『急救』技能！" /* 原常量参考：0x005899A0。 */ : L"使用『急救』技能，失败了！" /* 原常量参考：0x00589984。 */;
            }
            break;
        case 3:
            if (result != 0) {
                resultState.title = L"使用『治疗』技能" /* 原常量参考：0x00589970。 */;
                resultState.summary = L"使用『治疗』技能，成功了！" /* 原常量参考：0x00589954。 */;
                resultState.details += value == 4 ? L"回复了健康状态。" /* 原常量参考：0x00589940。 */ : L"回复了少许健康状态。" /* 原常量参考：0x00589928。 */;
                lines = 1;
            } else {
                resultState.title = L"使用『治疗』技能，" /* 原常量参考：0x00589914。 */;
                if (value == 0) {
                    resultState.summary = L"使用『治疗』技能，无法实施！" /* 原常量参考：0x005898F4。 */;
                } else {
                    resultState.summary = L"使用『治疗』技能，失败了！" /* 原常量参考：0x005898D8。 */;
                    if (value == 3 || value == 2) {
                        resultState.details += value == 3 ? L"伤势过度恶化，灵魂掉了一格。" /* 原常量参考：0x005898B8。 */ : L"伤势恶化了。" /* 原常量参考：0x005898A8。 */;
                    }
                    lines = 1;
                }
            }
            break;
        default:
            break;
    }
    const auto appendValue = [&](const std::wstring_view format, const std::int32_t number) {
        // 四种数值行按原GBK最多22字节，始终低于28列换行门槛。
        resultState.details += std::vformat(format, std::make_wformat_args(number)) + L'\n';
        ++lines;
    };
    if (resultState.experience > 0 && lines < 8) {
        appendValue(kCraftExperience589820, resultState.experience);
    }
    if (resultState.levelUp != 0 && lines < 8) {
        resultState.details += kCraftLevelUp589814;
        ++lines;
        static_cast<void>(audio::PlaySe_52C450(64, 320));
    }
    if (resultState.endurance != 0 && lines < 8) {
        appendValue(kCraftEndurance589808, resultState.endurance);
    }
    if (resultState.dexterity != 0 && lines < 8) {
        appendValue(kCraftDexterity5897FC, resultState.dexterity);
    }
    if (resultState.intelligence != 0 && lines < 8) {
        appendValue(kCraftIntelligence5897F0, resultState.intelligence);
    }
    return WindowSkillResult_Open(session);
}

} // namespace detail

} // namespace cg::ui
