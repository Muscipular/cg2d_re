#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004875E0。
static login::YobiAction* WindowSkillResult_Open(); // 原操作计数、关联关闭及窗口13创建。

// 原函数：0x004876B0。
static WindowAnimationData* WindowSkillResult_RefreshResult(WindowInstance* window); // 窗口13按分类完整填充结果。

// 原函数：0x00487D60。
static std::int32_t WindowSkillResult_ButtonContinue(std::int32_t index, std::int32_t state); // 先比较FP，再继续技能请求。

// 原函数：0x00487E80。
static std::int32_t WindowSkillResult_ButtonClose(std::int32_t index, std::int32_t state); // 关闭入口窗口，音效后写图号。

// 原函数：0x00487660。
static void WindowSkillResult_Release();

// 原函数：0x00487AE0。
static std::int32_t WindowSkillResult_Update(std::int32_t value);

// 原函数：0x00487B80。
static std::int32_t WindowSkillResult_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static std::int32_t gWindow13ResultD66E2C = 0; // 原全局：0x00D66E2C。

static std::int32_t gWindow13ExperienceCC2E5C = 0; // 原全局：0x00CC2E5C。

static std::int32_t gWindow13ValueCC3E8C = 0; // 原全局：0x00CC3E8C。

static std::int32_t gWindow13LevelUpCF468C = 0; // 原全局：0x00CF468C。

static std::int32_t gWindow13EnduranceCC421C = 0; // 原全局：0x00CC421C。

static std::int32_t gWindow13DexterityD2D644 = 0; // 原全局：0x00D2D644。

static std::int32_t gWindow13IntelligenceCC19FC = 0; // 原全局：0x00CC19FC。

static std::int32_t gWindow13ValueD2DBF0 = 0; // 原全局：0x00D2DBF0。

static std::wstring gWindow13SummaryCC18CC; // 原全局：0x00CC18CC。

static std::wstring gWindow13TitleCC18EC; // 原全局：0x00CC18EC。

static std::wstring gWindow13DetailsCB0FDC; // 原全局：0x00CB0FDC。 原NUL详情，只供同源结果消费者借读。

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
    {WindowElementType_Graphic, 178, 9, 11, 11, 1, {.graphic = &gWindowGraphic5628A8}, &WindowRuntime_ButtonClose},
    {WindowElementType_Animation, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56292C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage562928}, &WindowElement_NoOp},
    {WindowElementType_Animation, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56292C}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 28, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 42, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 56, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 70, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 24, 28, 0, 0, 1, {.graphic = &gWindowGraphic5628D8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 24, 55, 0, 0, 1, {.graphic = &gWindowGraphic5628D8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 29, 203, 49, 17, 1, {.graphic = &gWindowGraphic5628F0}, &WindowSkillResult_ButtonContinue},
    {WindowElementType_Graphic, 129, 203, 49, 17, 1, {.graphic = &gWindowGraphic562908}, &WindowSkillResult_ButtonClose},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5628C0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 201, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
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
static login::YobiAction* WindowSkillResult_Open()
{
    WindowRuntime_IncrementOperationCount();
    const auto references = static_cast<std::uint8_t>(gValueD67AFD + 1);
    const auto skillReferences = static_cast<std::uint8_t>(gValueD67AFE + 1);
    gValueD67AFD = references;
    WindowInstance* const previous = gWindowStates[WindowId_Craft].window;
    gValueD67AFE = skillReferences;
    if (previous != nullptr) {
        previous->state |= 0x8000u;
    }
    for (const std::int32_t id : {12, 45, 46, 13}) {
        WindowInstance* const window = gWindowStates[id].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    return WindowRuntime_CreateAnimation(WindowId_SkillResult, 1, 0, nullptr);
}

// 原函数：0x00487660。
static void WindowSkillResult_Release()
{
    WindowRuntime_DecrementOperationCount();
    const auto references = static_cast<std::uint8_t>(gValueD67AFD - 1);
    const auto skillReferences = static_cast<std::uint8_t>(gValueD67AFE - 1);
    gValueD67AFD = references;
    gValueD67AFE = skillReferences;
    if (skillReferences == 0 && world::detail::Sub_51BEC0() == 13) {
        static_cast<void>(world::detail::Sub_51BE50(0));
    }
    gWindowStates[WindowId_SkillResult].window->state |= 0x8000u;
}

// 原函数：0x004876B0。
static WindowAnimationData* WindowSkillResult_RefreshResult(WindowInstance* const window)
{
    const std::int32_t kind = gSkillTargetKindCF4A10;
    switch (kind) {
        case 0:
        case 1: {
            cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[4].data)->text, L"對{:<22}" /* 原常量参考：0x00586B00。 */, cg::text::FromGbkWide(gSkillTargetNameD2F748.data()).c_str());
            const wchar_t* text = nullptr;
            if (kind == 0) {
                text = gWindow13ResultD66E2C != 0 ? L"　變裝成功了！" /* 原常量参考：0x00586AF0。 */ : L"　變裝失敗了！" /* 原常量参考：0x00586AE0。 */;
            } else {
                text = gWindow13ResultD66E2C != 0 ? L"　變身成功了！" /* 原常量参考：0x00586AD0。 */ : L"　變身失敗了！" /* 原常量参考：0x00586AC0。 */;
            }
            static_cast<WindowTextData*>(window->elements[5].data)->text = text;
            break;
        }
        case 2:
            if (gWindow13ResultD66E2C != 0) {
                cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[4].data)->text, L"{:<16}的治療成功了" /* 原常量参考：0x00586AAC。 */, cg::text::FromGbkWide(gSkillTargetNameD2F748.data()).c_str());
                cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[5].data)->text, L"　生命值恢復了 {} 。" /* 原常量参考：0x00586A94。 */, gWindow13ValueD2DBF0);
            } else {
                cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[4].data)->text, L"對{:<22}" /* 原常量参考：0x00586B00。 */, cg::text::FromGbkWide(gSkillTargetNameD2F748.data()).c_str());
                static_cast<WindowTextData*>(window->elements[5].data)->text = gWindow13ValueD2DBF0 == 0 ? L"　不需進行治療！" /* 原常量参考：0x00586A80。 */ : L"　治療失敗了！" /* 原常量参考：0x00586A70。 */;
            }
            break;
        case 3:
            if (gWindow13ResultD66E2C != 0) {
                cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[4].data)->text, L"{:<16}的治療成功了" /* 原常量参考：0x00586AAC。 */, cg::text::FromGbkWide(gSkillTargetNameD2F748.data()).c_str());
                static_cast<WindowTextData*>(window->elements[5].data)->text = gWindow13ValueD2DBF0 == 4 ? L"傷勢全部恢復了！" /* 原常量参考：0x00586A5C。 */ : L"傷勢稍微恢復了。" /* 原常量参考：0x00586A48。 */;
            } else if (gWindow13ValueD2DBF0 == 0) {
                cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[4].data)->text, L"對{:<22}" /* 原常量参考：0x00586B00。 */, cg::text::FromGbkWide(gSkillTargetNameD2F748.data()).c_str());
                static_cast<WindowTextData*>(window->elements[5].data)->text = L"　不需進行治療！" /* 原常量参考：0x00586A80。 */;
            } else {
                cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[4].data)->text, L"{:<16}的治療失敗了" /* 原常量参考：0x00586A34。 */, cg::text::FromGbkWide(gSkillTargetNameD2F748.data()).c_str());
                const std::int32_t value = gWindow13ValueD2DBF0;
                static_cast<WindowTextData*>(window->elements[5].data)->text = value == 3 ? L"　傷勢惡化而掉魂了。" /* 原常量参考：0x00586A1C。 */
                    : value == 2 ? L"　傷勢惡化了。" /* 原常量参考：0x00586A0C。 */ : L"";
            }
            break;
        default:
            break;
    }
    static_cast<void>(audio::PlaySe_52C450(gWindow13ResultD66E2C != 0 ? 57 : 58, 320));
    if (gWindow13LevelUpCF468C != 0) {
        static_cast<WindowTextData*>(window->elements[6].data)->text = kCraftLevelUp58661C;
    } else {
        cg::text::FormatTo(static_cast<WindowTextData*>(window->elements[6].data)->text, kCraftExperience58660C, gWindow13ExperienceCC2E5C);
    }
    std::wstring& increments = static_cast<WindowTextData*>(window->elements[7].data)->text;
    increments.clear();
    if (gWindow13EnduranceCC421C != 0) {
        increments.append(std::format(kCraftEndurance586600, gWindow13EnduranceCC421C));
    }
    if (gWindow13DexterityD2D644 != 0) {
        increments.append(std::format(kCraftDexterity5865F4, gWindow13DexterityD2D644));
    }
    if (gWindow13IntelligenceCC19FC != 0) {
        increments.append(std::format(kCraftIntelligence5865E8, gWindow13IntelligenceCC19FC));
    }
    static_cast<WindowAnimationData*>(window->elements[2].data)->value04 = 1;
    login::detail::SetYobiActionField2B0(
        static_cast<WindowAnimationData*>(window->elements[2].data)->action,
        gWindow13ResultD66E2C != 0 ? 5 : 10);
    static_cast<void>(WindowCraft_SetPreviewPositions(
        window, 2, 1, 3, kCraftPreviewOffsetX57D7D0, kCraftPreviewOffsetY57D7D2));
    return WindowCraft_ConfigurePreviews_47D220(window, 1, 3, 1, gWindow13ResultD66E2C != 0 ? 6 : 8, 5);
}

// 原函数：0x00487AE0。
static std::int32_t WindowSkillResult_Update(const std::int32_t value)
{
    if (value == -1) {
        static_cast<void>(WindowSkillResult_RefreshResult(gCurrentWindow));
    }
    const std::int32_t imageNo =
        WindowSkillResult_ReadPreviewImage(character_select::detail::Sub_51B740(world::detail::gPlayerRuntimeSlots.imageNo), 0);
    login::detail::SetYobiActionField2A8(
        static_cast<WindowAnimationData*>(gCurrentWindow->elements[2].data)->action, imageNo);
    login::detail::SetYobiActionField2B8(
        static_cast<WindowAnimationData*>(gCurrentWindow->elements[2].data)->action, 5);
    if (gSkillSendModeC8A970 != 0) {
        static_cast<void>(WindowCraft_SetPreviewImages(gCurrentWindow, 1, 3, gSkillPetImageCB0A18));
    } else {
        static_cast<void>(WindowCraft_SetPreviewImages(gCurrentWindow, 1, 3, 0));
    }
    return 1;
}

// 原函数：0x00487B80。
static std::int32_t WindowSkillResult_Draw(const std::int32_t)
{
    login::YobiAction* const action =
        static_cast<WindowAnimationData*>(gCurrentWindow->elements[2].data)->action;
    const std::int32_t actionId = login::detail::GetYobiActionField2B0(action);
    const std::int32_t imageNo = login::detail::GetYobiActionField2A8(action);
    if (actionId == 18 || actionId == 5 || actionId == 15 || actionId == 10) {
        const bool succeeded = actionId == 18 || actionId == 5;
        const std::int32_t defaultAction = succeeded ? 5 : 10;
        login::detail::SetYobiActionField2B0(action, defaultAction);
        for (const auto& range : kCraftAlternateActionImages) {
            if (imageNo >= range[0] && imageNo <= range[1]) {
                login::detail::SetYobiActionField2B0(action, succeeded ? 18 : 15);
            }
        }
        if (imageNo == 0x18A26) {
            login::detail::SetYobiActionField2B0(action, defaultAction);
        }
    }
    if (imageNo >= 0x1AF0E && imageNo <= 0x1AF25) {
        login::detail::SetYobiActionField309(action, 4);
        login::detail::SetYobiActionField310(action, 4);
    } else {
        login::detail::SetYobiActionField309(action, 0);
    }
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x00487D60。
static std::int32_t WindowSkillResult_ButtonContinue(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    const std::int32_t level = gUsingSkillLevelCA0AC4;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    const std::int32_t skill = gUsingSkillIndexD2F32C;
    const std::int32_t cost = world::detail::gSkillsF1BAFC[skill].techniques3C[level].value7C
        * world::detail::gSkillsF1BAFC[skill].fpReduce / 100;
    if (world::detail::IsPlayerFpLessThan(cost)) {
        graphic->imageNo = 0x3BA32;
        if ((state & 2) != 0) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        return 0;
    }
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"繼續執行。" /* 原常量参考：0x005868E0。 */;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        const std::int32_t currentSkill = gUsingSkillIndexD2F32C;
        static_cast<void>(network::Sub_511C30(platform::gConnectionSocket, currentSkill));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3BA31;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3BA33;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3BA32;
    }
    return consumed;
}

// 原函数：0x00487E80。
static std::int32_t WindowSkillResult_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = kWindow13Text5868EC;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
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

// 原函数：0x004EEA40。
login::YobiAction* WindowSkillResult_ReceiveResult(const std::int32_t result, const char* const text)
{
    gWindow13ResultD66E2C = result;
    gWindow13DetailsCB0FDC.clear();
    gWindow13ExperienceCC2E5C = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    gWindow13ValueCC3E8C = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    gWindow13LevelUpCF468C = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    gWindow13EnduranceCC421C = network::ParseDelimitedInteger_52CFA0(text, '|', 4);
    gWindow13DexterityD2D644 = network::ParseDelimitedInteger_52CFA0(text, '|', 5);
    gWindow13IntelligenceCC19FC = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    const std::int32_t value = network::ParseDelimitedInteger_52CFA0(text, '|', 7);
    const std::int32_t kind = gSkillTargetKindCF4A10;
    gWindow13ValueD2DBF0 = value;
    std::int32_t lines = 0;
    switch (kind) {
        case 0:
            gWindow13TitleCC18EC = L"使用『變裝』技能" /* 原常量参考：0x00589A80。 */;
            gWindow13SummaryCC18CC = result != 0 ? L"使用『變裝』技能，成功了！" /* 原常量参考：0x00589A64。 */ : L"使用『變裝』技能，失敗了！" /* 原常量参考：0x00589A48。 */;
            lines = 1;
            break;
        case 1:
            gWindow13TitleCC18EC = L"使用『變身』技能" /* 原常量参考：0x00589A34。 */;
            gWindow13SummaryCC18CC = result != 0 ? L"使用『變身』技能，成功了！" /* 原常量参考：0x00589A18。 */ : L"使用『變身』技能，失敗了！" /* 原常量参考：0x005899FC。 */;
            lines = 1;
            break;
        case 2:
            gWindow13TitleCC18EC = L"使用『急救』技能" /* 原常量参考：0x005899E8。 */;
            if (result != 0) {
                gWindow13SummaryCC18CC = L"使用『急救』技能，成功了！" /* 原常量参考：0x005899B8。 */;
                gWindow13DetailsCB0FDC += std::format(L"回復了{}點生命力。" /* 原常量参考：0x005899D4。 */, value);
                lines = 1;
            } else {
                gWindow13SummaryCC18CC = value == 0 ? L"無法實施『急救』技能！" /* 原常量参考：0x005899A0。 */ : L"使用『急救』技能，失敗了！" /* 原常量参考：0x00589984。 */;
            }
            break;
        case 3:
            if (result != 0) {
                gWindow13TitleCC18EC = L"使用『治療』技能" /* 原常量参考：0x00589970。 */;
                gWindow13SummaryCC18CC = L"使用『治療』技能，成功了！" /* 原常量参考：0x00589954。 */;
                gWindow13DetailsCB0FDC += value == 4 ? L"回復了健康狀態。" /* 原常量参考：0x00589940。 */ : L"回復了少許健康狀態。" /* 原常量参考：0x00589928。 */;
                lines = 1;
            } else {
                gWindow13TitleCC18EC = L"使用『治療』技能，" /* 原常量参考：0x00589914。 */;
                if (value == 0) {
                    gWindow13SummaryCC18CC = L"使用『治療』技能，無法實施！" /* 原常量参考：0x005898F4。 */;
                } else {
                    gWindow13SummaryCC18CC = L"使用『治療』技能，失敗了！" /* 原常量参考：0x005898D8。 */;
                    if (value == 3 || value == 2) {
                        gWindow13DetailsCB0FDC += value == 3 ? L"傷勢過度惡化，靈魂掉了一格。" /* 原常量参考：0x005898B8。 */ : L"傷勢惡化了。" /* 原常量参考：0x005898A8。 */;
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
        gWindow13DetailsCB0FDC += std::vformat(format, std::make_wformat_args(number)) + L'\n';
        ++lines;
    };
    if (gWindow13ExperienceCC2E5C > 0 && lines < 8) {
        appendValue(kCraftExperience589820, gWindow13ExperienceCC2E5C);
    }
    if (gWindow13LevelUpCF468C != 0 && lines < 8) {
        gWindow13DetailsCB0FDC += kCraftLevelUp589814;
        ++lines;
        static_cast<void>(audio::PlaySe_52C450(64, 320));
    }
    if (gWindow13EnduranceCC421C != 0 && lines < 8) {
        appendValue(kCraftEndurance589808, gWindow13EnduranceCC421C);
    }
    if (gWindow13DexterityD2D644 != 0 && lines < 8) {
        appendValue(kCraftDexterity5897FC, gWindow13DexterityD2D644);
    }
    if (gWindow13IntelligenceCC19FC != 0 && lines < 8) {
        appendValue(kCraftIntelligence5897F0, gWindow13IntelligenceCC19FC);
    }
    return WindowSkillResult_Open();
}

} // namespace detail

} // namespace cg::ui
