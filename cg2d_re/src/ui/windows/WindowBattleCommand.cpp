#include "cg/network/CgConnection.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/session/ClientSession.hpp"
#include "../../world/BattleState.hpp"
#include "../../world/PlayerState.hpp"

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowBattleCommandElementId : ElementId {
    ButtonAttack = 1,
    ButtonDefend = 2,
    ButtonSkill = 3,
    ButtonItem = 4,
    ButtonPet = 5,
    ButtonFormation = 6,
    ButtonEscape = 7,
    ButtonRebirth = 8,
    ButtonPetSkills = 9,
    ButtonEscape_10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
};

namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004966B0。
static void WindowBattleCommand_Open(session::ClientSession& session, std::int32_t mode, std::int16_t x, std::int16_t y); // 窗口26五模式创建/配置及入场偏移。

// 原函数：0x00496B90。
static std::int32_t WindowBattleCommand_Update(WindowInstance& callbackWindow, std::int32_t value); // 窗口26初始化、入退场与按钮显隐。

// 原函数：0x00496D50。
static std::int32_t WindowBattleCommand_Draw(WindowInstance& callbackWindow, std::int32_t unused); // 先绘制再更新技术名，按命中索引追加目标说明。

// 原函数：0x00496AE0。
static void WindowBattleCommand_Close(session::ClientSession& session); // 窗口26存在才收尾、关闭10/27/28及清三展开引用，原返回无人消费。

// 原函数：0x00496F10。
static void WindowBattleCommand_ResetSelection(session::ClientSession& session); // 先清九个选择槽，再关闭三子窗及清同源展开引用。

// 原函数：0x004972B0。
static std::int32_t WindowBattleCommand_ButtonAttack(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 攻击选择，原目标标志475H。

// 原函数：0x004973C0。
static std::int32_t WindowBattleCommand_ButtonDefend(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 防御G命令及玩家动作完成。

// 原函数：0x004974C0。
static void WindowBattleCommand_SelectSkill(session::ClientSession& session, std::int32_t skillIndex, std::int32_t technique); // CC4230选择回送所属会话。

// 原函数：0x004975A0。
static std::int32_t WindowBattleCommand_ButtonSkill(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 技能按钮及选目标返回技术窗。

// 原函数：0x00497770。
static std::int32_t WindowBattleCommand_ButtonItem(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 物品选择/返回27，保留CC4D40独立门禁。

// 原函数：0x00497900。
static std::int32_t WindowBattleCommand_ButtonPet(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 宠物选择及28窗口关闭。

// 原函数：0x00497A60。
static std::int32_t WindowBattleCommand_ButtonFormation(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // P命令，使用九槽中的槽6。

// 原函数：0x00497B80。
static std::int32_t WindowBattleCommand_ButtonEscape(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // E命令，使用槽7，无已用技能门禁。

// 原函数：0x00497C80。
static std::int32_t WindowBattleCommand_ButtonRebirth(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // TUC01：原载荷判空/hover持有，刷新后重取R命令状态。

// 原函数：0x00497E30。
static void WindowBattleCommand_SelectPetSkill(session::ClientSession& session, std::int32_t technique); // 宠物命令，保留重复查询与负参数略过。

// 原函数：0x00497FF0。
static void WindowBattleCommand_SelectRidingSkill(session::ClientSession& session, std::int32_t technique); // 骑乘命令，清选择模式并保留骑乘快照写序。

// 原函数：0x00498160。
static std::int32_t WindowBattleCommand_ButtonPetSkills(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 宠物/骑乘技能窗开关。

// 原函数：0x004982C0。
static void WindowBattleCommand_DrawTimer(session::ClientSession& session); // 完整倒计时/超时发送，本帧超时后仍绘制。

// 原函数：0x0043C3E0。
static void WindowBattleCommand_DrawTargetHint(session::ClientSession& session); // 553F80门禁及首个可选对象的目标提示，不要求鼠标命中。

// 原函数：0x0043C490。
static void WindowBattleCommand_ResetTargets(std::int32_t* values, std::int32_t count); // 正数count逐槽写-2，原caller弃返回。

// 原函数：0x00496B90。
static std::int32_t WindowBattleCommand_Update(WindowInstance& callbackWindow, std::int32_t value);

// 原函数：0x00496D50。
static std::int32_t WindowBattleCommand_Draw(WindowInstance& callbackWindow, std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic56B5E0 {0x3B9C0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B5E0。

static WindowGraphicData gWindowGraphic56B5F8 {0x3B9C5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B5F8。

static WindowGraphicData gWindowGraphic56B610 {0x3B9D1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B610。

static WindowGraphicData gWindowGraphic56B628 {0x3B9C8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B628。

static WindowGraphicData gWindowGraphic56B640 {0x3B9D4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B640。

static WindowGraphicData gWindowGraphic56B658 {0x3B9CB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B658。

static WindowGraphicData gWindowGraphic56B670 {0x3B9D7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B670。

static WindowGraphicData gWindowGraphic56B688 {0x3B9DA, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B688。

static WindowGraphicData gWindowGraphic56B6A0 {0x3B9CE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B6A0。

static WindowTextPrototype gWindowText56B6F8 { // 原全局：0x0056B6F8。
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, L"名称", // 原常量参考：0x00586230。
};

static WindowTextPrototype gWindowText56B700 { // 原全局：0x0056B700。
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, L"魔力", // 原常量参考：0x005861C0。
};

static WindowTextPrototype gWindowText56B708 { // 原全局：0x0056B708。
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, L"技能", // 原常量参考：0x00586094。
};

static WindowElementPrototype gWindowElements56B710[14] { // 原全局：0x0056B710。
    {WindowElementType_Graphic, 13, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B5F8}, &WindowBattleCommand_ButtonAttack, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonAttack)},
    {WindowElementType_Graphic, 13, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B610}, &WindowBattleCommand_ButtonDefend, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonDefend)},
    {WindowElementType_Graphic, 83, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B628}, &WindowBattleCommand_ButtonSkill, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonSkill)},
    {WindowElementType_Graphic, 83, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B640}, &WindowBattleCommand_ButtonItem, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonItem)},
    {WindowElementType_Graphic, 153, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B658}, &WindowBattleCommand_ButtonPet, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonPet)},
    {WindowElementType_Graphic, 153, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B670}, &WindowBattleCommand_ButtonFormation, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonFormation)},
    {WindowElementType_Graphic, 223, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B688}, &WindowBattleCommand_ButtonEscape, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonEscape)},
    {WindowElementType_Graphic, 223, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B6A0}, &WindowBattleCommand_ButtonRebirth, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonRebirth)},
    {WindowElementType_Graphic, 216, 46, 64, 20, 1, {.graphic = &gWindowGraphic56B628}, &WindowBattleCommand_ButtonPetSkills, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonPetSkills)},
    {WindowElementType_Graphic, 216, 46, 64, 20, 1, {.graphic = &gWindowGraphic56B688}, &WindowBattleCommand_ButtonEscape, -1, static_cast<ElementId>(WindowBattleCommandElementId::ButtonEscape_10)},
    {WindowElementType_Text, 20, 24, 0, 0, 1, {.text = &gWindowText56B6F8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBattleCommandElementId::Element11)},
    {WindowElementType_Text, 215, 24, 0, 0, 1, {.text = &gWindowText56B700}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBattleCommandElementId::Element12)},
    {WindowElementType_Text, 20, 48, 0, 0, 1, {.text = &gWindowText56B708}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBattleCommandElementId::Element13)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56B5E0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBattleCommandElementId::Element14)},
};

// 原型：窗口26；原全局：0x0054E720。
WindowPrototype gWindowPrototypeBattleCommand {2, 5, 341, 0, 299, 69, 0x80010101u, 14, gWindowElements56B710, &WindowBattleCommand_Update, &WindowBattleCommand_Draw, &WindowRuntime_ReleaseDefault}; // 26: 54E720

// 窗口函数实现。

// 原函数：0x004966B0。
static void WindowBattleCommand_Open(
    session::ClientSession& session, const std::int32_t mode, std::int16_t x, std::int16_t y)
{
    AdaptWindowPosition(x, y, 299, 69);
    WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C =
        (WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C & ~0xFFu) | static_cast<std::uint8_t>(mode);
    WindowInstance* const window = WindowRuntime_States(session)[WindowId_BattleCommand].window;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    } else {
        WindowRuntime_States(session)[WindowId_BattleCommand].window = WindowRuntime_Open(session, WindowId_BattleCommand);
    }
    auto* elementsWindow = WindowRuntime_States(session)[WindowId_BattleCommand].window;
    const auto currentMode = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C));
    auto* const element14 = elementsWindow ? elementsWindow->FindElementById(WindowBattleCommandElementId::Element14) : nullptr;
    auto* const graphic = element14 ? element14->data.graphic : nullptr;
    switch (currentMode) {
    case 0:
    case 1:
    case 2:
        if (graphic) graphic->imageNo = 0x3B9C0 + currentMode;
        for (std::int32_t index = 0; index < 8; ++index) {
            if (auto* const indexedElement = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 1;
        }
        if (auto* const buttonPetSkillsElement = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::ButtonPetSkills) : nullptr; buttonPetSkillsElement) buttonPetSkillsElement->visible = 0;
        if (auto* const buttonEscape_10Element = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::ButtonEscape_10) : nullptr; buttonEscape_10Element) buttonEscape_10Element->visible = 0;
        if (auto* const element11 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element11) : nullptr; element11) element11->visible = 0;
        if (auto* const element12 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element12) : nullptr; element12) element12->visible = 0;
        if (auto* const element13 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element13) : nullptr; element13) element13->visible = 0;
        break;
    case 3: {
        if (graphic) graphic->imageNo = 0x3B9C3;
        for (std::int32_t index = 0; index < 8; ++index) {
            if (auto* const indexedElement = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
        }
        if (auto* const buttonPetSkillsElement2 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::ButtonPetSkills) : nullptr; buttonPetSkillsElement2) buttonPetSkillsElement2->visible = 1;
        if (auto* const buttonPetSkillsElement3 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::ButtonPetSkills) : nullptr; buttonPetSkillsElement3) buttonPetSkillsElement3->x = 216;
        if (auto* const buttonEscape_10Element2 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::ButtonEscape_10) : nullptr; buttonEscape_10Element2) buttonEscape_10Element2->visible = 0;
        if (auto* const element112 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element11) : nullptr; element112) element112->visible = 1;
        if (auto* const element122 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element12) : nullptr; element122) element122->visible = 1;
        if (auto* const element132 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element13) : nullptr; element132) element132->visible = 1;
        const std::int32_t namePosition = world::GetBattlePetPosition_405B40(
            session.WorldScene().battle.currentPosition);
        const cg::yobi::YobiAction* const action = session.WorldScene().battle.entries[namePosition];
        const wchar_t* name = cg::yobi::GetYobiTitle(action);
        if (name[0] == '\0') {
            name = cg::yobi::GetYobiName(action);
        }
        if (auto* const element113 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element11) : nullptr; element113) element113->data.text->text = name;
        const std::int32_t maximumPosition = world::GetBattlePetPosition_405B40(
            session.WorldScene().battle.currentPosition);
        const std::int32_t maximum = cg::yobi::ReadYobiActionMaxFpOnce(
            session.WorldScene().battle.entries[maximumPosition]);
        const std::int32_t currentPosition = world::GetBattlePetPosition_405B40(
            session.WorldScene().battle.currentPosition);
        const std::int32_t current = cg::yobi::ReadYobiActionFpOnce(
            session.WorldScene().battle.entries[currentPosition]);
        auto* const element123 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element12) : nullptr;
        if (element123) {
            cg::text::FormatTo(element123->data.text->text, L"{:4}/{:4}", current, maximum);
        }
        break;
    }
    case 4: {
        if (graphic) graphic->imageNo = 0x3B9C3;
        for (std::int32_t index = 0; index < 8; ++index) {
            if (auto* const indexedElement = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(NativeElementKey(index)) : nullptr; indexedElement) indexedElement->visible = 0;
        }
        if (auto* const buttonPetSkillsElement2 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::ButtonPetSkills) : nullptr; buttonPetSkillsElement2) buttonPetSkillsElement2->visible = 1;
        if (auto* const buttonPetSkillsElement3 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::ButtonPetSkills) : nullptr; buttonPetSkillsElement3) buttonPetSkillsElement3->x = 146;
        if (auto* const buttonEscape_10Element2 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::ButtonEscape_10) : nullptr; buttonEscape_10Element2) buttonEscape_10Element2->visible = 1;
        if (auto* const element112 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element11) : nullptr; element112) element112->visible = 1;
        if (auto* const element122 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element12) : nullptr; element122) element122->visible = 1;
        if (auto* const element132 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element13) : nullptr; element132) element132->visible = 1;
        const std::int32_t namePosition = session.WorldScene().battle.currentPosition;
        const wchar_t* const name = cg::yobi::GetYobiName(
            session.WorldScene().battle.entries[namePosition]);
        if (auto* const element113 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element11) : nullptr; element113) element113->data.text->text = name;
        const std::int32_t maximumPosition = session.WorldScene().battle.currentPosition;
        const std::int32_t maximum = cg::yobi::ReadYobiActionMaxFpOnce(
            session.WorldScene().battle.entries[maximumPosition]);
        const std::int32_t currentPosition = session.WorldScene().battle.currentPosition;
        const std::int32_t current = cg::yobi::ReadYobiActionFpOnce(
            session.WorldScene().battle.entries[currentPosition]);
        auto* const element123 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element12) : nullptr;
        if (element123) {
            cg::text::FormatTo(element123->data.text->text, L"{:4}/{:4}", current, maximum);
        }
        break;
    }
    default:
        break;
    }
    WindowInstance* const positionedWindow = WindowRuntime_States(session)[WindowId_BattleCommand].window;
    WindowRuntime_States(session)[WindowId_BattleCommand].x = x;
    WindowRuntime_States(session)[WindowId_BattleCommand].y = y;
    WindowRuntime_States(session)[WindowId_BattleCommand].value10 = std::bit_cast<std::uint32_t>(std::int32_t {-153});
    WindowRuntime_States(session)[WindowId_BattleCommand].value14 = 17;
    positionedWindow->x = x;
    const std::int32_t baseY = WindowRuntime_States(session)[WindowId_BattleCommand].y;
    const auto offset = std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_BattleCommand].value10);
    WindowRuntime_States(session)[WindowId_BattleCommand].window->y = baseY + offset;
    WindowRuntime_States(session)[WindowId_BattleCommand].userState.value = 1;
}

// 原函数：0x00496B90。
static std::int32_t WindowBattleCommand_Update(WindowInstance& callbackWindow, const std::int32_t value)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& inventory = callbackWindow.ownerSession->UiFrame().native.inventory;
    auto& session = *currentWindow->ownerSession;
    if (value == -1) {
        WindowRuntime_States(session)[WindowId_BattleCommand].value10 = 0;
        WindowRuntime_States(session)[WindowId_BattleCommand].value14 = 0;
        WindowRuntime_States(session)[WindowId_BattleCommand].userState.value = 0;
        session.UiFrame().native.battleCommand.skillTargetPending = 0;
        inventory.battleItemTargetFlag = 0;
        return 1;
    }
    const std::uint32_t phase = WindowRuntime_States(session)[WindowId_BattleCommand].userState.value;
    if (phase == 1) {
        const auto oldOffset = std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_BattleCommand].value10);
        const auto velocity = std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_BattleCommand].value14);
        const std::int32_t offset = oldOffset + velocity;
        WindowRuntime_States(session)[WindowId_BattleCommand].value14 = std::bit_cast<std::uint32_t>(velocity - 1);
        const std::int32_t baseY = WindowRuntime_States(session)[WindowId_BattleCommand].y;
        WindowRuntime_States(session)[WindowId_BattleCommand].value10 = std::bit_cast<std::uint32_t>(offset);
        currentWindow->y = baseY + offset;
        if (std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_BattleCommand].value14) <= 0) {
            WindowRuntime_States(session)[WindowId_BattleCommand].value14 = 0;
            WindowRuntime_States(session)[WindowId_BattleCommand].userState.value = 0;
        }
    } else if (phase == 2) {
        const auto velocity = std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_BattleCommand].value14);
        const auto oldOffset = std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_BattleCommand].value10);
        const std::int32_t nextVelocity = velocity + 1;
        WindowRuntime_States(session)[WindowId_BattleCommand].value14 = std::bit_cast<std::uint32_t>(nextVelocity);
        const std::int32_t offset = oldOffset - nextVelocity;
        const std::int32_t baseY = WindowRuntime_States(session)[WindowId_BattleCommand].y;
        WindowRuntime_States(session)[WindowId_BattleCommand].value10 = std::bit_cast<std::uint32_t>(offset);
        currentWindow->y = baseY + offset;
        if (std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_BattleCommand].value14) >= 17) {
            WindowRuntime_States(session)[WindowId_BattleCommand].value14 = 17;
            WindowRuntime_States(session)[WindowId_BattleCommand].window->state |= 0x8000u;
            WindowRuntime_States(session)[WindowId_BattleCommand].userState.value = 3;
        }
        WindowInstance* window = WindowRuntime_States(session)[WindowId_Skill].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
        window = WindowRuntime_States(session)[WindowId_BattleItems].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
        window = WindowRuntime_States(session)[WindowId_BattlePets].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    auto mode = static_cast<std::uint8_t>(WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C);
    if (mode != 3 && mode != 4) {
        if (auto* const buttonRebirthElement = currentWindow->FindElementById(WindowBattleCommandElementId::ButtonRebirth); buttonRebirthElement) buttonRebirthElement->visible = 1;
        mode = static_cast<std::uint8_t>(WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C);
    }
    if (mode == 2) {
        if (auto* const buttonSkillElement = currentWindow->FindElementById(WindowBattleCommandElementId::ButtonSkill); buttonSkillElement) buttonSkillElement->visible = 1;
        if (auto* const buttonItemElement = currentWindow->FindElementById(WindowBattleCommandElementId::ButtonItem); buttonItemElement) buttonItemElement->visible = 1;
        if (auto* const buttonPetElement = currentWindow->FindElementById(WindowBattleCommandElementId::ButtonPet); buttonPetElement) buttonPetElement->visible = 1;
        if (auto* const buttonFormationElement = currentWindow->FindElementById(WindowBattleCommandElementId::ButtonFormation); buttonFormationElement) buttonFormationElement->visible = 1;
        if (callbackWindow.ownerSession->WorldScene().battle.skillUsed != 0) {
            if (auto* const buttonSkillElement2 = currentWindow->FindElementById(WindowBattleCommandElementId::ButtonSkill); buttonSkillElement2) buttonSkillElement2->visible = 0;
            if (auto* const buttonItemElement2 = currentWindow->FindElementById(WindowBattleCommandElementId::ButtonItem); buttonItemElement2) buttonItemElement2->visible = 0;
            if (auto* const buttonPetElement2 = currentWindow->FindElementById(WindowBattleCommandElementId::ButtonPet); buttonPetElement2) buttonPetElement2->visible = 0;
            if (auto* const buttonFormationElement2 = currentWindow->FindElementById(WindowBattleCommandElementId::ButtonFormation); buttonFormationElement2) buttonFormationElement2->visible = 0;
        }
    }
    return 1;
}

// 原函数：0x00496D50。
static std::int32_t WindowBattleCommand_Draw(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    WindowRuntime_DrawCurrent(callbackWindow);
    const auto mode = static_cast<std::uint8_t>(WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C);
    if (mode == 3 || mode == 4) {
        std::wstring_view name;
        if (callbackWindow.ownerSession->WorldScene().battle.selectedPetTechniqueIndex < 0) {
            name = {};
        } else {
            const std::int32_t pet = world::FindBattlePetIndex_43C4B0(*callbackWindow.ownerSession);
            const std::int32_t technique = callbackWindow.ownerSession->WorldScene().battle.selectedPetTechniqueIndex;
            if (pet >= 0 && pet < static_cast<std::int32_t>(callbackWindow.ownerSession->WorldScene().petRecords->size())
                && technique < world::kPetTechniqueSlotCount)
                name = (*callbackWindow.ownerSession->WorldScene().petRecords)[pet].techniquesD8[technique].name;
        }
        if (auto* const element13 = WindowRuntime_States(session)[WindowId_BattleCommand].window ? WindowRuntime_States(session)[WindowId_BattleCommand].window->FindElementById(WindowBattleCommandElementId::Element13) : nullptr; element13) element13->data.text->text = name;
    }
    if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value == 0) {
        std::int32_t first = 0;
        for (; first < 20; ++first) {
            const cg::yobi::YobiAction* const action = callbackWindow.ownerSession->WorldScene().battle.entries[first];
            if (action != nullptr && (cg::yobi::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
                break;
            }
        }
        if (first < 20) {
            const std::int32_t hitIndex = callbackWindow.ownerSession->UiFrame().hoveredGraphicPendingIndex;
            std::int8_t selected = -1;
            for (std::int32_t index = 0; index < 20; ++index) {
                const cg::yobi::YobiAction* const action = callbackWindow.ownerSession->WorldScene().battle.entries[index];
                if (action != nullptr && cg::yobi::GetYobiActionCacheIndex(action) == hitIndex
                    && (cg::yobi::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
                    selected = static_cast<std::int8_t>(index);
                    break;
                }
            }
            WindowInstance* const textWindow = currentWindow;
            const auto windowX = static_cast<std::int16_t>(textWindow->x);
            const auto windowY = static_cast<std::int16_t>(textWindow->y);
            const auto textX = static_cast<std::int16_t>(windowX + 17);
            const auto textY = static_cast<std::int16_t>(windowY + 89);
            std::wstring name = L"请选择对象";
            if (selected >= 0) {
                const cg::yobi::YobiAction* const action = callbackWindow.ownerSession->WorldScene().battle.entries[selected];
                const wchar_t* title = cg::yobi::GetYobiTitle(action);
                if (title[0] == '\0') {
                    title = cg::yobi::GetYobiName(action);
                }
                name = title;
            }
            static_cast<void>(render::DrawText_456780(textX, textY, 3, 0, 0, name, 0, 0));
            WindowInstance* const backgroundWindow = currentWindow;
            const auto backgroundWindowY = static_cast<std::int16_t>(backgroundWindow->y);
            const auto backgroundWindowX = static_cast<std::int16_t>(backgroundWindow->x);
            const auto backgroundY = static_cast<std::int16_t>(backgroundWindowY + 69);
            const auto backgroundX = static_cast<std::int16_t>(backgroundWindowX + 2);
            cg::yobi::DrawNineGrid_49BE00(0x3BE84, backgroundX, backgroundY, 296, 61, 223,
                -1, std::bit_cast<std::int32_t>(0x80FFFFFFu), 74, 54);
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// 原函数：0x00496AE0。
static void WindowBattleCommand_Close(session::ClientSession& session)
{
    if (WindowRuntime_States(session)[WindowId_BattleCommand].window == nullptr) {
        return;
    }
    if (std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_BattleCommand].value10) > 0) {
        WindowRuntime_States(session)[WindowId_BattleCommand].value10 = 0;
    }
    if (std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_BattleCommand].value14) < 0) {
        WindowRuntime_States(session)[WindowId_BattleCommand].value14 = 0;
    }
    WindowInstance* window = WindowRuntime_States(session)[WindowId_Skill].window;
    WindowRuntime_States(session)[WindowId_BattleCommand].userState.value = 2;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    window = WindowRuntime_States(session)[WindowId_BattleItems].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    window = WindowRuntime_States(session)[WindowId_BattlePets].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    cg::yobi::YobiAction* const skillAction = session.UiFrame().native.battleCommand.skillOpening;
    if (skillAction != nullptr) {
        cg::yobi::MarkDeath(skillAction);
        session.UiFrame().native.battleCommand.skillOpening = nullptr;
    }
    cg::yobi::YobiAction* const itemAction = session.UiFrame().native.battleCommand.itemOpening;
    if (itemAction != nullptr) {
        cg::yobi::MarkDeath(itemAction);
        session.UiFrame().native.battleCommand.itemOpening = nullptr;
    }
    cg::yobi::YobiAction* const petAction = session.UiFrame().native.battleCommand.petOpening;
    if (petAction != nullptr) {
        cg::yobi::MarkDeath(petAction);
        session.UiFrame().native.battleCommand.petOpening = nullptr;
    }
}

// 原函数：0x00496F10。
static void WindowBattleCommand_ResetSelection(session::ClientSession& session)
{
    session.WorldScene().battle.selectionModes.fill(0);
    WindowInstance* window = WindowRuntime_States(session)[WindowId_Skill].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    window = WindowRuntime_States(session)[WindowId_BattleItems].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    window = WindowRuntime_States(session)[WindowId_BattlePets].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    cg::yobi::YobiAction* const skillWindow = session.UiFrame().native.battleCommand.skillOpening;
    if (skillWindow != nullptr) {
        cg::yobi::MarkDeath(skillWindow);
        session.UiFrame().native.battleCommand.skillOpening = nullptr;
    }
    cg::yobi::YobiAction* const itemWindow = session.UiFrame().native.battleCommand.itemOpening;
    if (itemWindow != nullptr) {
        cg::yobi::MarkDeath(itemWindow);
        session.UiFrame().native.battleCommand.itemOpening = nullptr;
    }
    cg::yobi::YobiAction* const petWindow = session.UiFrame().native.battleCommand.petOpening;
    if (petWindow != nullptr) {
        cg::yobi::MarkDeath(petWindow);
        session.UiFrame().native.battleCommand.petOpening = nullptr;
    }
}

// L05：Lua动作已发送后，沿既有战斗窗口和选择状态收尾顺序提交该回合。
void WindowBattleCommand_FinishAutoAction(session::ClientSession& session)
{
    WindowRuntime_States(session)[WindowId_BattleCommand].userState.value = 0;
    session.WorldScene().battle.countdownActive = 0;
    cg::yobi::FlushBattleEntries_43C060(session);
    WindowBattleCommand_Close(session);
    session.WorldScene().sceneState = 5;
    if (session.WorldScene().battle.commandAction) cg::yobi::MarkDeath(session.WorldScene().battle.commandAction);
    session.WorldScene().battle.commandAction = nullptr;
    session.WorldScene().battle.petActionCount = 2;
    session.WorldScene().battle.skillActionCount = 2;
    session.WorldScene().battle.commandState = 5;
    session.WorldScene().battle.snapshotPetIndex = -1;
    WindowBattleCommand_ResetSelection(session);
    if (auto* window = WindowRuntime_States(session)[WindowId_BattleCommand].window) window->state |= 0x8000u;
}

// 原函数：0x004972B0。
static std::int32_t WindowBattleCommand_ButtonAttack(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0) {
            graphic->imageNo = 0x3B9C4;
            if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[0] != 0) {
                graphic->imageNo = 0x3B9C5;
            }
            return 0;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"普通物理攻击。";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            WindowBattleCommand_ResetSelection(session);
            callbackWindow.ownerSession->WorldScene().battle.selectionModes[0] = 1;
            callbackWindow.ownerSession->WorldScene().battle.selectionMode = 0;
            cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
            static_cast<void>(cg::yobi::CheckBattleCommand_43C560(*callbackWindow.ownerSession,
                callbackWindow.ownerSession->WorldScene().battle.currentPosition, 0x475u));
            cg::yobi::MarkDeath(callbackWindow.ownerSession->WorldScene().battle.commandAction);
            callbackWindow.ownerSession->WorldScene().battle.commandAction = nullptr;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
            session.UiFrame().native.battleCommand.targetSelectionReady = 0;
            consumed = 1;
        }
        graphic->imageNo = 0x3B9C4;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B9C6;
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[0] != 0) {
            graphic->imageNo = 0x3B9C5;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x004973C0。
static std::int32_t WindowBattleCommand_ButtonDefend(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0) {
            graphic->imageNo = 0x3B9D0;
            if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[1] != 0) {
                graphic->imageNo = 0x3B9D1;
            }
            return 0;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"普通物理防御。";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            WindowBattleCommand_ResetSelection(session);
            callbackWindow.ownerSession->WorldScene().battle.selectionModes[1] = 1;
            callbackWindow.ownerSession->WorldScene().battle.selectionMode = 0;
            cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
            static_cast<void>(session.Connection().SendBattlePacket(L"G"));
            cg::yobi::MarkDeath(callbackWindow.ownerSession->WorldScene().battle.commandAction);
            callbackWindow.ownerSession->WorldScene().battle.commandAction = nullptr;
            callbackWindow.ownerSession->WorldScene().battle.playerActionState = 1;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3B9D0;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B9D2;
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[1] != 0) {
            graphic->imageNo = 0x3B9D1;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00497770。
static std::int32_t WindowBattleCommand_ButtonItem(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& inventory = callbackWindow.ownerSession->UiFrame().native.inventory;
    auto& session = *currentWindow->ownerSession;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if (callbackWindow.ownerSession->WorldScene().battle.skillUsed != 0) {
            graphic->imageNo = 0x3B9D4;
            return 0;
        }
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0) {
            graphic->imageNo = 0x3B9D3;
            if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[3] != 0) {
                graphic->imageNo = 0x3B9D4;
            }
            return 0;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"使用物品。";
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[3] == 0) {
            if ((state & MouseFlag_LDownEvent) != 0) {
                WindowBattleCommand_ResetSelection(session);
                callbackWindow.ownerSession->WorldScene().battle.selectionMode = 3;
                callbackWindow.ownerSession->WorldScene().battle.selectionModes[3] = 1;
                cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
                cg::yobi::MarkDeath(callbackWindow.ownerSession->WorldScene().battle.commandAction);
                callbackWindow.ownerSession->WorldScene().battle.commandAction = nullptr;
                callbackWindow.ownerSession->WorldScene().battle.itemTargetPending = 0;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
                session.UiFrame().native.battleCommand.targetSelectionReady = 0;
                session.UiFrame().native.battleCommand.itemOpening = WindowRuntime_CreateAnimation(session, WindowId_BattleItems, 1, 0, nullptr);
                consumed = 1;
            }
        } else if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(*callbackWindow.ownerSession)) & platform::LegacyMouseEdge_RDown) != 0 && inventory.battleItemTargetFlag != 0) {
            session.UiFrame().native.battleCommand.itemOpening = WindowRuntime_CreateAnimation(session, WindowId_BattleItems, 1, 0, nullptr);
            inventory.battleItemTargetFlag = 0;
            cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
        }
        graphic->imageNo = 0x3B9D3;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B9D5;
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[3] != 0) {
            graphic->imageNo = 0x3B9D4;
        }
        if (WindowRuntime_States(session)[WindowId_BattleItems].window != nullptr) {
            session.UiFrame().native.battleCommand.itemOpening = nullptr;
        } else if (session.UiFrame().native.battleCommand.itemOpening == nullptr && inventory.battleItemTargetFlag == 0) {
            callbackWindow.ownerSession->WorldScene().battle.selectionModes[3] = 0;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00497900。
static std::int32_t WindowBattleCommand_ButtonPet(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if (callbackWindow.ownerSession->WorldScene().battle.skillUsed != 0) {
            graphic->imageNo = 0x3B9CB;
            return 0;
        }
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0) {
            graphic->imageNo = 0x3B9CA;
            if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[4] != 0) {
                graphic->imageNo = 0x3B9CB;
            }
            return 0;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"召唤宠物或封印宠物。";
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[4] == 0) {
            if ((state & MouseFlag_LDownEvent) != 0) {
                WindowBattleCommand_ResetSelection(session);
                callbackWindow.ownerSession->WorldScene().battle.selectionMode = 4;
                callbackWindow.ownerSession->WorldScene().battle.selectionModes[4] = 1;
                cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
                cg::yobi::MarkDeath(callbackWindow.ownerSession->WorldScene().battle.commandAction);
                callbackWindow.ownerSession->WorldScene().battle.commandAction = nullptr;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
                session.UiFrame().native.battleCommand.petOpening = WindowRuntime_CreateAnimation(session, WindowId_BattlePets, 1, 0, nullptr);
                consumed = 1;
            }
        } else if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(*callbackWindow.ownerSession)) & platform::LegacyMouseEdge_RDown) != 0) {
            WindowInstance* const window = WindowRuntime_States(session)[WindowId_BattlePets].window;
            if (window != nullptr) {
                window->state |= 0x8000u;
            }
        }
        graphic->imageNo = 0x3B9CA;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B9CC;
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[4] != 0) {
            graphic->imageNo = 0x3B9CB;
        }
        if (WindowRuntime_States(session)[WindowId_BattlePets].window != nullptr) {
            session.UiFrame().native.battleCommand.petOpening = nullptr;
        } else if (session.UiFrame().native.battleCommand.petOpening == nullptr) {
            callbackWindow.ownerSession->WorldScene().battle.selectionModes[4] = 0;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00497A60。
static std::int32_t WindowBattleCommand_ButtonFormation(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if (callbackWindow.ownerSession->WorldScene().battle.skillUsed != 0) {
            graphic->imageNo = 0x3B9D7;
            return 0;
        }
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0) {
            graphic->imageNo = 0x3B9D6;
            if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[6] != 0) {
                graphic->imageNo = 0x3B9D7;
            }
            return 0;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"变更队形。";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            WindowBattleCommand_ResetSelection(session);
            callbackWindow.ownerSession->WorldScene().battle.selectionMode = 6;
            callbackWindow.ownerSession->WorldScene().battle.selectionModes[6] = 1;
            cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
            static_cast<void>(session.Connection().SendBattlePacket(L"P"));
            cg::yobi::MarkDeath(callbackWindow.ownerSession->WorldScene().battle.commandAction);
            callbackWindow.ownerSession->WorldScene().battle.commandAction = nullptr;
            callbackWindow.ownerSession->WorldScene().battle.playerActionState = 1;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3B9D6;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B9D8;
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[6] != 0) {
            graphic->imageNo = 0x3B9D7;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00497B80。
static std::int32_t WindowBattleCommand_ButtonEscape(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0) {
            graphic->imageNo = 0x3B9D9;
            if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[7] != 0) {
                graphic->imageNo = 0x3B9DA;
            }
            return 0;
        }
        std::int32_t consumed = 0;
        if ((state & MouseFlag_Hover) != 0) {
            uiFrame.hoverText = L"逃离此场战斗。";
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            WindowBattleCommand_ResetSelection(session);
            callbackWindow.ownerSession->WorldScene().battle.selectionModes[7] = 1;
            callbackWindow.ownerSession->WorldScene().battle.selectionMode = 0;
            cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
            static_cast<void>(session.Connection().SendBattlePacket(L"E"));
            cg::yobi::MarkDeath(callbackWindow.ownerSession->WorldScene().battle.commandAction);
            callbackWindow.ownerSession->WorldScene().battle.commandAction = nullptr;
            callbackWindow.ownerSession->WorldScene().battle.playerActionState = 1;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3B9D9;
        if ((state & MouseFlag_Hover) != 0) {
            graphic->imageNo = 0x3B9DB;
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[7] != 0) {
            graphic->imageNo = 0x3B9DA;
        }
        return consumed;
    }
    return 0;
}

// TUC01：原战斗转生门槛/载荷判空保持；持有 hover，刷新后重取命令状态，发送弃返。
// 原函数：0x00497C80。
static std::int32_t WindowBattleCommand_ButtonRebirth(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto* const elementPtr = currentWindow->FindElementById(elementId);
    if (elementPtr) {
        WindowElement* const element = elementPtr;
        auto* const graphic = element->data.graphic;
        if (callbackWindow.ownerSession->WorldScene().battle.rebirthDisabled != 0
            || callbackWindow.ownerSession->WorldScene().player->value08 == 0
            || callbackWindow.ownerSession->WorldScene().player->timerRemaining0C <= 0) {
            element->visible = 0;
            graphic->imageNo = 0x3B9CE;
            return 0;
        }
        element->visible = 1;
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0) {
            graphic->imageNo = 0x3B9CD;
            if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[8] != 0) {
                graphic->imageNo = 0x3B9CE;
            }
            return 0;
        }
        const cg::yobi::YobiAction* const action =
            callbackWindow.ownerSession->WorldScene().battle.entries[callbackWindow.ownerSession->WorldScene().battle.currentPosition];
        if (action == nullptr) {
            return 0;
        }
        cg::yobi::YobiAction* const* const rebirth = cg::yobi::FindBattleRebirthSlot(action);
        if (rebirth == nullptr) {
            return 0;
        }
        const bool hover = (state & MouseFlag_Hover) != 0;
        std::int32_t consumed = 0;
        if (hover) {
            uiFrame.hoverText = *rebirth != nullptr ? kSkillDisableRebirth5864B4 : kSkillEnableRebirth5864A0;
        }
        if ((state & MouseFlag_LDownEvent) != 0) {
            WindowBattleCommand_ResetSelection(session);
            callbackWindow.ownerSession->WorldScene().battle.selectionModes[8] = 1;
            callbackWindow.ownerSession->WorldScene().battle.selectionMode = 8;
            cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
            const cg::yobi::YobiAction* const current =
                callbackWindow.ownerSession->WorldScene().battle.entries[callbackWindow.ownerSession->WorldScene().battle.currentPosition];
            const char* const command =
                *cg::yobi::FindBattleRebirthSlot(current) != nullptr ? "R|0" : "R|1";
            static_cast<void>(session.Connection().SendBattlePacket(cg::text::FromGbkWide(command)));
            cg::yobi::MarkDeath(callbackWindow.ownerSession->WorldScene().battle.commandAction);
            callbackWindow.ownerSession->WorldScene().battle.commandAction = nullptr;
            callbackWindow.ownerSession->WorldScene().battle.playerActionState = 1;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
            consumed = 1;
        }
        graphic->imageNo = 0x3B9CD;
        if (hover) {
            graphic->imageNo = 0x3B9CF;
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[8] != 0) {
            graphic->imageNo = 0x3B9CE;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00496FB0。
std::int32_t WindowBattleCommand_CanSelectSkill(session::ClientSession& session)
{
    if (WindowRuntime_States(session)[WindowId_BattleCommand].window == nullptr
        || WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0
        || session.WorldScene().battle.currentPosition >= 20) {
        return 0;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || session.WorldScene().battle.skillUsed != 0) {
        return 0;
    }
    return session.WorldScene().battle.selectionModes[2] == 0 ? -1 : 0;
}

// 原函数：0x004970B0。
std::int32_t WindowBattleCommand_CanSelectItem(session::ClientSession& session)
{
    if (WindowRuntime_States(session)[WindowId_BattleCommand].window == nullptr
        || WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0
        || session.WorldScene().battle.currentPosition >= 20) {
        return 0;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || session.WorldScene().battle.skillUsed != 0) {
        return 0;
    }
    return session.WorldScene().battle.selectionModes[3] == 0 ? -1 : 0;
}

// 原函数：0x00497100。
void WindowBattleCommand_OpenItems(session::ClientSession& session)
{
    if (WindowRuntime_States(session)[WindowId_BattleCommand].window == nullptr
        || WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0
        || session.WorldScene().battle.currentPosition >= 20) {
        return;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        *reinterpret_cast<const std::uint8_t*>(&WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || session.WorldScene().battle.skillUsed != 0
        || session.WorldScene().battle.selectionModes[3] != 0) {
        return;
    }
    WindowBattleCommand_ResetSelection(session);
    session.WorldScene().battle.selectionModes[3] = 1;
    session.WorldScene().battle.selectionMode = 3;
    cg::yobi::FlushBattleEntries_43C060(session);
    cg::yobi::MarkDeath(session.WorldScene().battle.commandAction);
    session.WorldScene().battle.commandAction = nullptr;
    session.WorldScene().battle.itemTargetPending = 0;
    static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
    session.UiFrame().native.battleCommand.targetSelectionReady = 0;
    session.UiFrame().native.battleCommand.itemOpening = WindowRuntime_CreateAnimation(session, WindowId_BattleItems, 1, 0, nullptr);
}

// 原函数：0x004971C0。
std::int32_t WindowBattleCommand_CanSelectPet(session::ClientSession& session)
{
    if (WindowRuntime_States(session)[WindowId_BattleCommand].window == nullptr
        || WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0
        || session.WorldScene().battle.currentPosition >= 20) {
        return 0;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || session.WorldScene().battle.skillUsed != 0) {
        return 0;
    }
    return session.WorldScene().battle.selectionModes[4] == 0 ? -1 : 0;
}

// 原函数：0x00497210。
void WindowBattleCommand_OpenPets(session::ClientSession& session)
{
    if (WindowRuntime_States(session)[WindowId_BattleCommand].window == nullptr
        || WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0
        || session.WorldScene().battle.currentPosition >= 20) {
        return;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        *reinterpret_cast<const std::uint8_t*>(&WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || session.WorldScene().battle.skillUsed != 0
        || session.WorldScene().battle.selectionModes[4] != 0) {
        return;
    }
    WindowBattleCommand_ResetSelection(session);
    session.WorldScene().battle.selectionModes[4] = 1;
    session.WorldScene().battle.selectionMode = 4;
    cg::yobi::FlushBattleEntries_43C060(session);
    cg::yobi::MarkDeath(session.WorldScene().battle.commandAction);
    session.WorldScene().battle.commandAction = nullptr;
    static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
    session.UiFrame().native.battleCommand.petOpening = WindowRuntime_CreateAnimation(session, WindowId_BattlePets, 1, 0, nullptr);
}

// 原函数：0x00497000。
void WindowBattleCommand_OpenSkills(session::ClientSession& session)
{
    if (WindowRuntime_States(session)[WindowId_BattleCommand].window == nullptr
        || WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0
        || session.WorldScene().battle.currentPosition >= 20) {
        return;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        *reinterpret_cast<const std::uint8_t*>(&WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || session.WorldScene().battle.skillUsed != 0
        || session.WorldScene().battle.selectionModes[2] != 0) {
        return;
    }
    WindowBattleCommand_ResetSelection(session);
    session.WorldScene().battle.selectionModes[2] = 1;
    session.WorldScene().battle.selectionMode = 2;
    cg::yobi::FlushBattleEntries_43C060(session);
    cg::yobi::MarkDeath(session.WorldScene().battle.commandAction);
    session.WorldScene().battle.commandAction = nullptr;
    session.WorldScene().battle.value5ABCC0 = 0;
    static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
    session.UiFrame().native.battleCommand.targetSelectionReady = 0;
    session.UiFrame().native.battleCommand.skillOpening = WindowSkill_OpenBattleSkills(session, &WindowBattleCommand_SelectSkill);
}

// 原函数：0x004974C0。
static void WindowBattleCommand_SelectSkill(
    session::ClientSession& session, const std::int32_t skillIndex, const std::int32_t technique)
{
    session.WorldScene().battle.selectedSkill = skillIndex;
    session.WorldScene().battle.selectedTechnique = technique;
    const auto target = std::bit_cast<std::uint32_t>(
        session.WorldScene().playerSkills.skills[skillIndex].techniques3C[technique].value84);
    const std::int32_t position = session.WorldScene().battle.currentPosition;
    if (cg::yobi::CheckBattleCommand_43C560(session, position, target) != 0) {
        session.UiFrame().native.battleCommand.skillTargetPending = 1;
        return;
    }
    const std::int32_t currentPosition = session.WorldScene().battle.currentPosition;
    const std::int32_t currentTechnique = session.WorldScene().battle.selectedTechnique;
    const std::int32_t currentSkill = session.WorldScene().battle.selectedSkill;
    std::array<char, 768> buffer;
    std::sprintf(buffer.data(), "S|%X|%X|%X",
        static_cast<std::uint32_t>(currentSkill),
        static_cast<std::uint32_t>(currentTechnique),
        static_cast<std::uint32_t>(currentPosition));
    static_cast<void>(session.Connection().SendBattlePacket(cg::text::FromGbkWide(buffer.data())));
    cg::yobi::FlushBattleEntries_43C060(session);
    static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
    const std::int32_t count = session.WorldScene().battle.skillActionCount;
    session.WorldScene().battle.commandState = 2;
    session.WorldScene().battle.skillActionCount = count + 1;
    cg::yobi::FlushBattleEntries_43C060(session);
    static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
    WindowBattleCommand_Close(session);
}

// 原函数：0x004975A0。
static std::int32_t WindowBattleCommand_ButtonSkill(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if (callbackWindow.ownerSession->WorldScene().battle.skillUsed != 0) {
            graphic->imageNo = 0x3B9C8;
            return 0;
        }
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0) {
            graphic->imageNo = 0x3B9C7;
            if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[2] != 0) {
                graphic->imageNo = 0x3B9C8;
            }
            return 0;
        }
        std::int32_t consumed = 0;
        const std::int32_t hover = state & MouseFlag_Hover;
        if (hover != 0) {
            uiFrame.hoverText = L"使用技能。";
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[2] == 0) {
            if ((state & MouseFlag_LDownEvent) != 0) {
                WindowBattleCommand_ResetSelection(session);
                callbackWindow.ownerSession->WorldScene().battle.selectionMode = 2;
                callbackWindow.ownerSession->WorldScene().battle.selectionModes[2] = 1;
                cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
                cg::yobi::MarkDeath(callbackWindow.ownerSession->WorldScene().battle.commandAction);
                callbackWindow.ownerSession->WorldScene().battle.commandAction = nullptr;
                callbackWindow.ownerSession->WorldScene().battle.value5ABCC0 = 0;
                static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 51, 320));
                session.UiFrame().native.battleCommand.targetSelectionReady = 0;
                session.UiFrame().native.battleCommand.skillOpening = WindowSkill_OpenBattleSkills(session, &WindowBattleCommand_SelectSkill);
                consumed = 1;
            }
        } else if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(*callbackWindow.ownerSession)) & platform::LegacyMouseEdge_RDown) != 0) {
            if (session.UiFrame().native.battleCommand.skillTargetPending != 0) {
                session.UiFrame().native.battleCommand.skillOpening = WindowSkill_OpenBattleTechniques(
                    session, callbackWindow.ownerSession->WorldScene().battle.selectedSkill, &WindowBattleCommand_SelectSkill);
                session.UiFrame().native.battleCommand.skillTargetPending = 0;
                cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
            } else if (WindowRuntime_States(session)[WindowId_Skill].window != nullptr) {
                if (WindowSkill_Back(session) != 0) {
                    callbackWindow.ownerSession->WorldScene().battle.selectionModes[2] = 0;
                }
            } else {
                session.UiFrame().native.battleCommand.skillOpening = WindowSkill_OpenBattleSkills(session, &WindowBattleCommand_SelectSkill);
            }
        }
        graphic->imageNo = 0x3B9C7;
        if (hover != 0) {
            graphic->imageNo = 0x3B9C9;
        }
        if (callbackWindow.ownerSession->WorldScene().battle.selectionModes[2] != 0) {
            graphic->imageNo = 0x3B9C8;
        }
        if (WindowRuntime_States(session)[WindowId_Skill].window != nullptr) {
            session.UiFrame().native.battleCommand.skillOpening = nullptr;
        } else if (session.UiFrame().native.battleCommand.skillOpening == nullptr && session.UiFrame().native.battleCommand.skillTargetPending == 0) {
            callbackWindow.ownerSession->WorldScene().battle.selectionModes[2] = 0;
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x00497E30。
static void WindowBattleCommand_SelectPetSkill(session::ClientSession& session, const std::int32_t technique)
{
    if (technique < 0) {
        session.WorldScene().battle.petCommandPhase = 0;
        static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
        if (session.WorldScene().battle.currentPosition < 20) {
            static_cast<void>(session.Connection().SendBattlePacket(L"W|FF"));
        }
        cg::yobi::FlushBattleEntries_43C060(session);
        static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
        const std::int32_t count = session.WorldScene().battle.petActionCount;
        session.WorldScene().battle.commandState = 5;
        session.WorldScene().battle.petActionCount = count + 1;
        session.WorldScene().battle.lastCommandPetIndex = world::FindBattlePetIndex_43C4B0(session);
        WindowBattleCommand_Close(session);
        return;
    }
    session.WorldScene().battle.selectedPetTechniqueIndex = technique;
    const std::int32_t targetPet = world::FindBattlePetIndex_43C4B0(session);
    const std::int32_t targetTechnique = session.WorldScene().battle.selectedPetTechniqueIndex;
    const auto target = static_cast<std::uint32_t>(static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>((*session.WorldScene().petRecords)[targetPet]
            .techniquesD8[targetTechnique].target)));
    const std::int32_t position = world::GetBattlePetPosition_405B40(
        session.WorldScene().battle.currentPosition);
    if (cg::yobi::CheckBattleCommand_43C560(session, position, target) == 0) {
        const std::int32_t idPet = world::FindBattlePetIndex_43C4B0(session);
        const std::int32_t idTechnique = session.WorldScene().battle.selectedPetTechniqueIndex;
        const std::int32_t targetIndex =
            (*session.WorldScene().petRecords)[idPet].techniquesD8[idTechnique].techniqueId >= 200600
            ? world::GetBattlePetPosition_405B40(session.WorldScene().battle.currentPosition)
            : world::FindBattlePetIndex_43C4B0(session);
        const std::int32_t currentTechnique = session.WorldScene().battle.selectedPetTechniqueIndex;
        std::array<char, 768> buffer;
        std::sprintf(buffer.data(), "W|%X|%X",
            static_cast<std::uint32_t>(currentTechnique), static_cast<std::uint32_t>(targetIndex));
        static_cast<void>(session.Connection().SendBattlePacket(cg::text::FromGbkWide(buffer.data())));
        cg::yobi::FlushBattleEntries_43C060(session);
        static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
        const std::int32_t count = session.WorldScene().battle.petActionCount;
        session.WorldScene().battle.commandState = 5;
        session.WorldScene().battle.petActionCount = count + 1;
        session.WorldScene().battle.lastCommandPetIndex = world::FindBattlePetIndex_43C4B0(session);
        WindowBattleCommand_Close(session);
    }
    static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
    session.WorldScene().battle.petCommandPhase = 2;
}

// 原函数：0x00497FF0。
static void WindowBattleCommand_SelectRidingSkill(session::ClientSession& session, const std::int32_t technique)
{
    if (technique < 0) {
        session.WorldScene().battle.petCommandPhase = 0;
        session.WorldScene().battle.selectionMode = 0;
        static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
        if (session.WorldScene().battle.currentPosition < 20) {
            static_cast<void>(session.Connection().SendBattlePacket(L"W|FF"));
        }
        cg::yobi::FlushBattleEntries_43C060(session);
        static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
        const std::int32_t count = session.WorldScene().battle.petActionCount;
        const auto pet = std::bit_cast<std::int32_t>(session.WorldScene().battle.ridePet);
        session.WorldScene().battle.petActionCount = count + 1;
        session.WorldScene().battle.commandState = 5;
        session.WorldScene().battle.lastCommandPetIndex = pet;
        WindowBattleCommand_Close(session);
        return;
    }
    session.WorldScene().battle.selectionMode = 0;
    const auto targetPet = std::bit_cast<std::int32_t>(session.WorldScene().battle.ridePet);
    session.WorldScene().battle.selectedPetTechniqueIndex = technique;
    const auto target = static_cast<std::uint32_t>(static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>((*session.WorldScene().petRecords)[targetPet]
            .techniquesD8[technique].target)));
    const std::int32_t position = session.WorldScene().battle.currentPosition;
    if (cg::yobi::CheckBattleCommand_43C560(session, position, target) == 0) {
        const std::uint32_t pet = session.WorldScene().battle.ridePet;
        const std::int32_t currentTechnique = session.WorldScene().battle.selectedPetTechniqueIndex;
        std::array<char, 768> buffer;
        std::sprintf(buffer.data(), "W|%X|%X",
            static_cast<std::uint32_t>(currentTechnique), pet);
        static_cast<void>(session.Connection().SendBattlePacket(cg::text::FromGbkWide(buffer.data())));
        cg::yobi::FlushBattleEntries_43C060(session);
        static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
        const std::int32_t count = session.WorldScene().battle.petActionCount;
        const auto currentPet = std::bit_cast<std::int32_t>(session.WorldScene().battle.ridePet);
        session.WorldScene().battle.petActionCount = count + 1;
        session.WorldScene().battle.commandState = 5;
        session.WorldScene().battle.lastCommandPetIndex = currentPet;
        WindowBattleCommand_Close(session);
    }
    static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
    session.WorldScene().battle.petCommandPhase = 2;
}

// 原函数：0x00498160。
static std::int32_t WindowBattleCommand_ButtonPetSkills(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const currentWindow = &callbackWindow;
    if (!currentWindow) return 0;
    auto& session = *currentWindow->ownerSession;
    auto* const graphicElement = currentWindow->FindElementById(elementId);
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value != 0) {
            return 0;
        }
        std::int32_t consumed = 0;
        const std::int32_t entryState = state;
        WindowInstance* const window = WindowRuntime_States(session)[WindowId_Skill].window;
        const std::int32_t hover = entryState & MouseFlag_Hover;
        if (hover != 0 && window != nullptr) {
            uiFrame.hoverText = L"关闭技能视窗。";
        }
        cg::yobi::YobiAction* const pendingAction = session.UiFrame().native.battleCommand.petSkillOpening;
        if (hover != 0 && window == nullptr && pendingAction == nullptr) {
            uiFrame.hoverText = L"开启技能视窗。";
        }
        if (pendingAction == nullptr
            && ((entryState & MouseFlag_LDownEvent) != 0
                || (static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(*callbackWindow.ownerSession)) & platform::LegacyMouseEdge_RDown) != 0)) {
            if (window != nullptr) {
                window->state |= 0x8000u;
            } else {
                cg::yobi::FlushBattleEntries_43C060(*callbackWindow.ownerSession);
                const std::uint8_t actionState =
                    reinterpret_cast<const std::uint8_t*>(&WindowRuntime_States(session)[WindowId_BattleCommand].actionFlags1C)[0];
                callbackWindow.ownerSession->WorldScene().battle.petCommandPhase = 1;
                const bool riding = actionState == 4;
                callbackWindow.ownerSession->WorldScene().battle.selectedPetTechniqueIndex = -1;
                session.UiFrame().native.battleCommand.petSkillOpening = riding
                    ? WindowSkill_OpenRidingSkills(session, &WindowBattleCommand_SelectRidingSkill) : WindowSkill_OpenPetSkills(session, &WindowBattleCommand_SelectPetSkill);
            }
            consumed = 1;
        }
        graphic->imageNo = 0x3B9C7;
        if (hover != 0) {
            graphic->imageNo = 0x3B9C9;
        }
        if (WindowRuntime_States(session)[WindowId_Skill].window != nullptr || session.UiFrame().native.battleCommand.petSkillOpening != nullptr) {
            graphic->imageNo = 0x3B9C8;
            if (WindowRuntime_States(session)[WindowId_Skill].window != nullptr) {
                session.UiFrame().native.battleCommand.petSkillOpening = nullptr;
            }
        }
        return consumed;
    }
    return 0;
}

// 原函数：0x0043C3E0。
static void WindowBattleCommand_DrawTargetHint(session::ClientSession& session)
{
    auto& uiFrame = session.UiFrame();
    if (session.WorldScene().battle.commandValue != 0) {
        return;
    }
    bool selected = false;
    for (const cg::yobi::YobiAction* const action : session.WorldScene().battle.entries) {
        if (action != nullptr && (cg::yobi::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
            selected = true;
            break;
        }
    }
    if (!selected) {
        return;
    }
    const std::int32_t y = session.WorldScene().battle.commandState == 4 ? 88 : 91;
    static_cast<void>(cg::render::DrawGraphic(platform::gWindowWidth - 138, y + 20, 212, 0x58B1, 0, nullptr));
    static_cast<void>(render::DrawText2_456670(
        static_cast<std::int16_t>(platform::gWindowWidth - 257), static_cast<std::int16_t>(y + 12), 2, 0, 0,
        L"请选择目标", 0));
    uiFrame.hoverText = L"按滑鼠右键可取消。";
}

// 原函数：0x0043C490。
static void WindowBattleCommand_ResetTargets(std::int32_t* const values, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        values[index] = -2;
    }
}

// 原函数：0x00498520。
void WindowBattleCommand_Dispatch(session::ClientSession& session)
{
    auto& uiFrame = session.UiFrame();
    std::wstring buffer;
    const std::int32_t hitIndex = session.UiFrame().hoveredGraphicPendingIndex;
    for (const cg::yobi::YobiAction* const action : session.WorldScene().battle.entries) {
        if (action != nullptr && cg::yobi::GetYobiActionCacheIndex(action) == hitIndex
            && (cg::yobi::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
            static_cast<void>(platform::RequestCursorState_506DE0(session, 2, 0));
            break;
        }
    }
    WindowBattleCommand_DrawTargetHint(session);
    switch (session.WorldScene().battle.commandState) {
    case 0: {
        std::int32_t position = session.WorldScene().battle.currentPosition;
        if (position < 20) {
            cg::yobi::YobiAction* const player = session.WorldScene().battle.entries[position];
            if (*cg::yobi::FindBattleRiderSlot(player) != nullptr) {
                if ((reinterpret_cast<const std::uint8_t*>(&session.WorldScene().battle.playerFlags)[0] & 8u) != 0) {
                    static_cast<void>(session.Connection().SendBattlePacket(L"N"));
                    const std::int32_t petCount = session.WorldScene().battle.petActionCount;
                    const std::uint32_t flags = session.WorldScene().battle.playerFlags;
                    const bool skipPlayer = (flags & 0x20000u) != 0;
                    session.WorldScene().battle.petActionCount = petCount + 1;
                    if (skipPlayer) {
                        static_cast<void>(session.Connection().SendBattlePacket(L"N"));
                        const std::int32_t count = session.WorldScene().battle.skillActionCount;
                        session.WorldScene().sceneState = 5;
                        session.WorldScene().battle.skillActionCount = count + 1;
                    } else {
                        session.WorldScene().battle.playerFlags &= ~8u;
                        session.WorldScene().battle.rebirthDisabled = 0;
                    }
                    WindowBattleCommand_Close(session);
                    session.WorldScene().battle.playerActionState = 0;
                    break;
                }
                const std::int32_t technique = session.WorldScene().battle.selectedPetTechniqueIndex;
                bool open = technique == -1 || session.WorldScene().battle.selectionMode != 0;
                if (!open) {
                    const std::uint32_t flags = session.WorldScene().battle.petSkillFlags;
                    open = (flags & (1u << (static_cast<std::uint32_t>(technique) & 31u))) == 0;
                }
                if (!open) {
                    const auto pet = std::bit_cast<std::int32_t>(session.WorldScene().battle.ridePet);
                    const std::int32_t cost = (*session.WorldScene().petRecords)[pet].techniquesD8[technique].fp;
                    open = cg::yobi::IsCostGreaterThanYobiActionFp(cost, player);
                }
                if (!open) {
                    const auto pet = std::bit_cast<std::int32_t>(session.WorldScene().battle.ridePet);
                    const std::int32_t currentTechnique = session.WorldScene().battle.selectedPetTechniqueIndex;
                    const auto target =
                        std::bit_cast<std::int16_t>((*session.WorldScene().petRecords)[pet].techniquesD8[currentTechnique].target);
                    const std::int32_t currentPosition = session.WorldScene().battle.currentPosition;
                    open = cg::yobi::CheckBattleCommand_43C560(session,
                        currentPosition, static_cast<std::uint32_t>(target)) == 0;
                }
                if (open) {
                    session.WorldScene().battle.petCommandPhase = 1;
                    session.WorldScene().battle.selectedPetTechniqueIndex = -1;
                    static_cast<void>(WindowSkill_OpenRidingSkills(session, &WindowBattleCommand_SelectRidingSkill));
                }
                session.WorldScene().battle.petSkillGraphicIndex = -2;
                WindowBattleCommand_ResetSelection(session);
                const std::int32_t mode = session.WorldScene().battle.selectionMode;
                if (mode > 0) {
                    session.WorldScene().battle.selectionModes[mode] = 1;
                    cg::yobi::FlushBattleEntries_43C060(session);
                } else if (mode == 0) {
                    session.WorldScene().battle.selectionModes[0] = 1;
                }
                session.WorldScene().battle.commandState = 4;
                WindowBattleCommand_Open(session, 4, 341, 0);
                static_cast<void>(audio::PlaySe_52C450(session, 56, 320));
                session.WorldScene().battle.playerActionState = 0;
                break;
            }
        }
        std::uint32_t flags = session.WorldScene().battle.playerFlags;
        if ((flags & 4u) != 0) {
            if (position < 20) {
                static_cast<void>(session.Connection().SendBattlePacket(L"N"));
                position = session.WorldScene().battle.currentPosition;
                flags = session.WorldScene().battle.playerFlags;
            }
            const std::int32_t count = session.WorldScene().battle.skillActionCount;
            session.WorldScene().battle.skillActionCount = count + 1;
            if ((flags & 0x20u) != 0) {
                session.WorldScene().battle.commandState = 3;
            } else if ((flags & 0x10u) != 0) {
                if (position < 20) {
                    static_cast<void>(session.Connection().SendBattlePacket(L"N"));
                }
                const std::int32_t nextCount = session.WorldScene().battle.skillActionCount;
                session.WorldScene().sceneState = 5;
                session.WorldScene().battle.skillActionCount = nextCount + 1;
            } else {
                session.WorldScene().battle.playerFlags = flags & ~4u;
                session.WorldScene().battle.rebirthDisabled = 0;
            }
            WindowBattleCommand_Close(session);
            session.WorldScene().battle.playerActionState = 0;
            break;
        }
        if ((flags & 0x10u) != 0) {
            const std::int32_t count = session.WorldScene().battle.skillActionCount;
            session.WorldScene().battle.rebirthDisabled = 0;
            session.WorldScene().battle.selectionMode = count == 0 ? session.WorldScene().battle.firstActionSelectionMode : session.WorldScene().battle.secondActionSelectionMode;
        } else if (session.WorldScene().battle.skillActionCount == 0) {
            const std::int32_t mode = session.WorldScene().battle.firstActionSelectionMode;
            session.WorldScene().battle.rebirthDisabled = 0;
            session.WorldScene().battle.selectionMode = mode;
        } else {
            session.WorldScene().battle.selectionMode = session.WorldScene().battle.secondActionSelectionMode;
        }
        WindowBattleCommand_ResetTargets(session.WorldScene().battle.resetValues.data(), 9);
        WindowBattleCommand_ResetSelection(session);
        std::int32_t mode = session.WorldScene().battle.selectionMode;
        if (mode >= 0) {
            session.WorldScene().battle.selectionModes[mode] = 1;
            cg::yobi::FlushBattleEntries_43C060(session);
            mode = session.WorldScene().battle.selectionMode;
        }
        switch (mode) {
        case 0:
            static_cast<void>(cg::yobi::CheckBattleCommand_43C560(session,
                session.WorldScene().battle.currentPosition, 0x475u));
            break;
        case 2:
            static_cast<void>(WindowSkill_OpenBattleSkills(session, &WindowBattleCommand_SelectSkill));
            break;
        case 3:
            static_cast<void>(WindowRuntime_CreateAnimation(session, WindowId_BattleItems, 1, 0, nullptr));
            break;
        case 4:
            static_cast<void>(WindowRuntime_CreateAnimation(session, WindowId_BattlePets, 1, 0, nullptr));
            break;
        default:
            break;
        }
        const auto modeFlags = reinterpret_cast<const std::uint8_t*>(&session.WorldScene().battle.playerFlags)[0];
        session.WorldScene().battle.commandState = 1;
        std::int32_t commandType = 0;
        if ((modeFlags & 0x10u) == 0) {
            commandType = session.WorldScene().battle.skillActionCount != 0 ? 2 : 1;
        }
        WindowBattleCommand_Open(session, commandType, 341, 0);
        static_cast<void>(audio::PlaySe_52C450(session, 56, 320));
        session.WorldScene().battle.playerActionState = 0;
        break;
    }
    case 1: {
        if (session.WorldScene().battle.selectionModes[0] == 1 && (static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session)) & platform::LegacyMouseEdge_RDown) != 0) {
            session.WorldScene().battle.selectionModes[0] = 0;
            cg::yobi::FlushBattleEntries_43C060(session);
            static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
        }
        if (session.UiFrame().native.battleCommand.targetSelectionReady != 0) {
            const std::int32_t target = cg::yobi::GetSelectedBattleEntry_43C120(session);
            if (target != -1 && (static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session)) & platform::LegacyMouseEdge_LDown) != 0) {
                bool send = true;
                switch (session.WorldScene().battle.selectionMode) {
                case 0:
                    cg::text::FormatTo(buffer, L"H|{:X}", static_cast<std::uint32_t>(target));
                    break;
                case 2: {
                    const std::int32_t technique = session.WorldScene().battle.selectedTechnique;
                    const std::int32_t skill = session.WorldScene().battle.selectedSkill;
                    cg::text::FormatTo(buffer, L"S|{:X}|{:X}|{:X}", static_cast<std::uint32_t>(skill), static_cast<std::uint32_t>(technique), static_cast<std::uint32_t>(target));
                    break;
                }
                case 3: {
                    const std::int32_t item = session.WorldScene().battle.selectedItem;
                    cg::text::FormatTo(buffer, L"I|{:X}|{:X}", static_cast<std::uint32_t>(item), static_cast<std::uint32_t>(target));
                    break;
                }
                default:
                    send = false;
                    break;
                }
                if (send) {
                    static_cast<void>(session.Connection().SendBattlePacket(buffer));
                    cg::yobi::FlushBattleEntries_43C060(session);
                    session.WorldScene().battle.playerActionState = 1;
                    static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
                }
            }
        }
        if (session.WorldScene().battle.playerActionState == 1) {
            const std::int32_t count = session.WorldScene().battle.skillActionCount;
            session.WorldScene().battle.commandState = 2;
            session.WorldScene().battle.skillActionCount = count + 1;
            cg::yobi::FlushBattleEntries_43C060(session);
            WindowBattleCommand_Close(session);
        }
        if (session.UiFrame().native.battleCommand.targetSelectionReady == 0) {
            session.UiFrame().native.battleCommand.targetSelectionReady = 1;
        }
        break;
    }
    case 2:
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value == 3) {
            const std::int32_t count = session.WorldScene().battle.skillActionCount;
            session.WorldScene().battle.playerActionState = 0;
            if (count == 1) {
                const bool singleAction = (reinterpret_cast<const std::uint8_t*>(&session.WorldScene().battle.playerFlags)[0] & 0x10u) != 0;
                std::int32_t mode = session.WorldScene().battle.selectionMode;
                if (!singleAction) {
                    session.WorldScene().battle.commandState = 0;
                    session.WorldScene().battle.firstActionSelectionMode = mode;
                    if (mode == 6) {
                        mode = 0;
                        session.WorldScene().battle.skillUsed = 1;
                        session.WorldScene().battle.firstActionSelectionMode = 0;
                        session.WorldScene().battle.secondActionSelectionMode = 0;
                    } else if (mode >= 2 && mode <= 6) {
                        session.WorldScene().battle.skillUsed = 1;
                        session.WorldScene().battle.secondActionSelectionMode = 0;
                    }
                } else {
                    session.WorldScene().battle.firstActionSelectionMode = mode;
                    if (mode == 6) {
                        mode = 0;
                        session.WorldScene().battle.firstActionSelectionMode = 0;
                    }
                    session.WorldScene().battle.commandState = 3;
                }
                if (mode == 8) {
                    session.WorldScene().battle.rebirthDisabled = 1;
                }
            } else {
                const std::int32_t mode = session.WorldScene().battle.selectionMode;
                session.WorldScene().battle.secondActionSelectionMode = mode;
                if (mode == 6) {
                    session.WorldScene().battle.secondActionSelectionMode = 0;
                }
                session.WorldScene().sceneState = 5;
            }
        }
        break;
    case 3: {
        const auto flags = reinterpret_cast<const std::uint8_t*>(&session.WorldScene().battle.playerFlags)[0];
        if ((flags & 0x20u) == 0) {
            session.WorldScene().sceneState = 5;
            break;
        }
        if ((flags & 8u) != 0) {
            if (session.WorldScene().battle.currentPosition < 20) {
                static_cast<void>(session.Connection().SendBattlePacket(L"W|FF"));
            }
            ++session.WorldScene().battle.petActionCount;
            session.WorldScene().sceneState = 5;
            break;
        }
        const std::int32_t technique = session.WorldScene().battle.selectedPetTechniqueIndex;
        bool open = technique == -1
            || (session.WorldScene().battle.petSkillFlags & (1u << (static_cast<std::uint32_t>(technique) & 31u))) == 0;
        if (!open) {
            const std::int32_t position = cg::world::GetBattlePetPosition_405B40(session.WorldScene().battle.currentPosition);
            cg::yobi::YobiAction* const action = session.WorldScene().battle.entries[position];
            const std::int32_t pet = cg::world::FindBattlePetIndex_43C4B0(session);
            const std::int32_t currentTechnique = session.WorldScene().battle.selectedPetTechniqueIndex;
            const std::int32_t cost = (*session.WorldScene().petRecords)[pet].techniquesD8[currentTechnique].fp;
            open = cg::yobi::IsCostGreaterThanYobiActionFp(cost, action);
        }
        if (!open) {
            const std::int32_t pet = cg::world::FindBattlePetIndex_43C4B0(session);
            const std::int32_t currentTechnique = session.WorldScene().battle.selectedPetTechniqueIndex;
            const auto target =
                std::bit_cast<std::int16_t>((*session.WorldScene().petRecords)[pet].techniquesD8[currentTechnique].target);
            const std::int32_t position = cg::world::GetBattlePetPosition_405B40(session.WorldScene().battle.currentPosition);
            open = cg::yobi::CheckBattleCommand_43C560(session, position, static_cast<std::uint32_t>(target)) == 0;
        }
        if (open) {
            session.WorldScene().battle.petCommandPhase = 1;
            session.WorldScene().battle.selectedPetTechniqueIndex = -1;
            static_cast<void>(WindowSkill_OpenPetSkills(session, &WindowBattleCommand_SelectPetSkill));
        }
        session.WorldScene().battle.petSkillGraphicIndex = -2;
        session.WorldScene().battle.commandState = 4;
        WindowBattleCommand_Open(session, 3, 341, 0);
        static_cast<void>(audio::PlaySe_52C450(session, 56, 320));
        break;
    }
    case 4: {
        if (session.WorldScene().battle.petCommandPhase == 3) {
            const std::int32_t target = cg::yobi::GetSelectedBattleEntry_43C120(session);
            if (target != -1 && (static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session)) & platform::LegacyMouseEdge_LDown) != 0) {
                const std::int32_t technique = session.WorldScene().battle.selectedPetTechniqueIndex;
                cg::text::FormatTo(buffer, L"W|{:X}|{:X}", static_cast<std::uint32_t>(technique), static_cast<std::uint32_t>(target));
                static_cast<void>(session.Connection().SendBattlePacket(buffer));
                cg::yobi::FlushBattleEntries_43C060(session);
                session.WorldScene().battle.playerActionState = 1;
                static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
            }
        }
        std::int32_t phase = session.WorldScene().battle.petCommandPhase;
        if (phase == 2) {
            phase = 3;
            session.WorldScene().battle.petCommandPhase = 3;
        }
        bool draw = false;
        std::int32_t imageNo = 0;
        switch (phase) {
        case 0: {
            if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session)) & platform::LegacyMouseEdge_RDown) != 0) {
                const auto pet = std::bit_cast<std::int32_t>(session.WorldScene().battle.ridePet);
                session.WorldScene().battle.petCommandPhase = 1;
                if (pet < 0) {
                    static_cast<void>(WindowSkill_OpenPetSkills(session, &WindowBattleCommand_SelectPetSkill));
                } else {
                    static_cast<void>(WindowSkill_OpenRidingSkills(session, &WindowBattleCommand_SelectRidingSkill));
                }
            }
            const std::int32_t graphicIndex = session.WorldScene().battle.petSkillGraphicIndex;
            const std::int32_t hover = session.UiFrame().hoveredGraphicPendingIndex;
            if (graphicIndex == hover) {
                const auto clicks = static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session));
                uiFrame.hoverText = L"打开技能视窗。";
                if ((clicks & platform::LegacyMouseEdge_LDown) != 0) {
                    session.WorldScene().battle.petCommandPhase = 1;
                }
            }
            imageNo = 0x589B;
            draw = true;
            break;
        }
        case 1: {
            const std::int32_t graphicIndex = session.WorldScene().battle.petSkillGraphicIndex;
            const std::int32_t hover = session.UiFrame().hoveredGraphicPendingIndex;
            if (graphicIndex == hover) {
                // 原498DF9复制12B，498DFD取点击位，498E07再复制末3B。
                uiFrame.hoverText = L"关闭技能视窗。";
                const auto clicks = static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session));
                static_cast<void>(0);
                if ((clicks & platform::LegacyMouseEdge_LDown) != 0) {
                    WindowInstance* const window = WindowRuntime_States(session)[WindowId_Skill].window;
                    if (window != nullptr) {
                        window->state |= 0x8000u;
                    }
                    session.WorldScene().battle.petCommandPhase = 0;
                    static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
                }
            }
            imageNo = 0x589C;
            draw = true;
            break;
        }
        case 2:
        case 3: {
            if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session)) & platform::LegacyMouseEdge_RDown) != 0) {
                cg::yobi::FlushBattleEntries_43C060(session);
                const auto pet = std::bit_cast<std::int32_t>(session.WorldScene().battle.ridePet);
                session.WorldScene().battle.petCommandPhase = 1;
                session.WorldScene().battle.selectedPetTechniqueIndex = -1;
                if (pet < 0) {
                    static_cast<void>(WindowSkill_OpenPetSkills(session, &WindowBattleCommand_SelectPetSkill));
                } else {
                    static_cast<void>(WindowSkill_OpenRidingSkills(session, &WindowBattleCommand_SelectRidingSkill));
                }
            }
            const std::int32_t graphicIndex = session.WorldScene().battle.petSkillGraphicIndex;
            const std::int32_t hover = session.UiFrame().hoveredGraphicPendingIndex;
            bool clickedReturn = false;
            if (graphicIndex == hover) {
                const auto clicks = static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session));
                uiFrame.hoverText = L"返回技能视窗。";
                if ((clicks & platform::LegacyMouseEdge_LDown) != 0) {
                    cg::yobi::FlushBattleEntries_43C060(session);
                    session.WorldScene().battle.petCommandPhase = 1;
                    session.WorldScene().battle.selectedPetTechniqueIndex = -1;
                    clickedReturn = true;
                }
            }
            if (!clickedReturn && session.WorldScene().battle.selectedPetTechniqueIndex != -1) {
                const std::int32_t namePet = cg::world::FindBattlePetIndex_43C4B0(session);
                const std::int32_t nameTechnique = session.WorldScene().battle.selectedPetTechniqueIndex;
                buffer = std::format(L"{:<16}", (*session.WorldScene().petRecords)[namePet].techniquesD8[nameTechnique].name);
                const std::int32_t nameY = session.WorldScene().battle.commandY;
                const std::int32_t nameX = session.WorldScene().battle.commandX;
                static_cast<void>(render::DrawText2_456670(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(nameX - 114)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(nameY - 15)),
                    2, 0, 0, buffer, 0));
                const std::int32_t costPet = cg::world::FindBattlePetIndex_43C4B0(session);
                const std::int32_t costTechnique = session.WorldScene().battle.selectedPetTechniqueIndex;
                cg::text::FormatTo(buffer, L"{:4}", (*session.WorldScene().petRecords)[costPet].techniquesD8[costTechnique].fp);
                const std::int32_t costY = session.WorldScene().battle.commandY;
                const std::int32_t costX = session.WorldScene().battle.commandX;
                static_cast<void>(render::DrawText2_456670(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(costX + 90)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(costY - 12)),
                    2, 1, 0, buffer, 0));
            }
            imageNo = 0x5899;
            draw = true;
            break;
        }
        default:
            break;
        }
        if (draw) {
            const std::int32_t y = session.WorldScene().battle.commandY;
            const std::int32_t x = session.WorldScene().battle.commandX;
            session.WorldScene().battle.petSkillGraphicIndex = cg::render::DrawGraphic(x, y, 212, imageNo, 2, nullptr);
        }
        if (session.WorldScene().battle.playerActionState == 1) {
            const std::int32_t count = session.WorldScene().battle.petActionCount;
            session.WorldScene().battle.commandState = 5;
            session.WorldScene().battle.petActionCount = count + 1;
            session.WorldScene().battle.lastCommandPetIndex = cg::world::FindBattlePetIndex_43C4B0(session);
            WindowBattleCommand_Close(session);
        }
        break;
    }
    case 5:
        if (WindowRuntime_States(session)[WindowId_BattleCommand].userState.value == 3) {
            cg::yobi::YobiAction* const player = session.WorldScene().battle.entries[session.WorldScene().battle.currentPosition];
            if (*cg::yobi::FindBattleRiderSlot(player) != nullptr && session.WorldScene().battle.petActionCount < 2) {
                session.WorldScene().battle.playerActionState = 0;
                session.WorldScene().battle.commandState = 0;
            } else {
                session.WorldScene().sceneState = 5;
            }
        }
        break;
    default:
        break;
    }
    const auto flags = reinterpret_cast<const std::uint8_t*>(&session.WorldScene().battle.playerFlags)[0];
    if ((flags & 0x1Cu) != 0x1Cu) {
        WindowBattleCommand_DrawTimer(session);
    }
}

// 原函数：0x004982C0。
static void WindowBattleCommand_DrawTimer(session::ClientSession& session)
{
    const std::uint64_t tick = platform::ReadTickCount64();
    std::int64_t remaining = std::bit_cast<std::int64_t>(session.WorldScene().battle.commandDeadline - tick);
    if (session.WorldScene().battle.countdownActive == 0) {
        return;
    }
    if (remaining <= 0) {
        session.WorldScene().battle.countdownActive = 0;
        remaining = 0;
        cg::yobi::FlushBattleEntries_43C060(session);
        WindowBattleCommand_Close(session);
        cg::yobi::YobiAction* const player = session.WorldScene().battle.entries[session.WorldScene().battle.currentPosition];
        bool finish = false;
        if (*cg::yobi::FindBattleRiderSlot(player) != nullptr) {
            static_cast<void>(session.Connection().SendBattlePacket(L"W|FF"));
            finish = true;
        } else {
            std::uint8_t flags = reinterpret_cast<const std::uint8_t*>(&session.WorldScene().battle.playerFlags)[0];
            const std::int32_t skillCount = session.WorldScene().battle.skillActionCount;
            if ((flags & 0x20u) != 0) {
                if (skillCount == 0 && (flags & 4u) == 0) {
                    static_cast<void>(session.Connection().SendBattlePacket(L"N"));
                    finish = true;
                }
                if (session.WorldScene().battle.petActionCount == 0) {
                    static_cast<void>(session.Connection().SendBattlePacket(L"W|FF"));
                    finish = true;
                }
            } else if (skillCount != 0 || (flags & 4u) == 0) {
                if (skillCount == 0) {
                    static_cast<void>(session.Connection().SendBattlePacket(L"N"));
                    static_cast<void>(session.Connection().SendBattlePacket(L"N"));
                    flags = reinterpret_cast<const std::uint8_t*>(&session.WorldScene().battle.playerFlags)[0];
                    finish = true;
                }
                if (session.WorldScene().battle.skillActionCount == 1 && (flags & 4u) == 0) {
                    static_cast<void>(session.Connection().SendBattlePacket(L"N"));
                    finish = true;
                }
            }
        }
        if (finish) {
            static_cast<void>(audio::PlaySe_52C450(session, 55, 320));
            cg::yobi::YobiAction* const action = session.WorldScene().battle.commandAction;
            session.WorldScene().sceneState = 5;
            cg::yobi::MarkDeath(action);
            session.WorldScene().battle.commandAction = nullptr;
        }
    }
    std::int32_t imageBase = 0x5690;
    if (remaining < 10000) {
        imageBase = 0x569A;
        const std::int32_t seconds = static_cast<std::int32_t>(remaining / 1000);
        if (seconds != session.WorldScene().battle.countdownSecond) {
            static_cast<void>(audio::PlaySe_52C450(session, 202, 320));
            session.WorldScene().battle.countdownSecond = seconds;
        }
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{}", remaining / 1000);
    // G01：完整绘制秒数，沿参考 calcOffset 按实际位数居中。
    const std::int32_t digitCount = static_cast<std::int32_t>(buffer.size());
    const std::int32_t firstDigitX = (platform::gWindowWidth - digitCount * 32 + 32) / 2;
    for (std::int32_t index = 0; index < digitCount; ++index) {
        const std::int32_t digit =
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(buffer[index]));
        static_cast<void>(cg::render::DrawGraphic(
            firstDigitX + index * 32, platform::gWindowHeight / 2, 209, imageBase + digit - '0', 0, nullptr));
    }
    if (session.WorldScene().battle.modeFlag == 1) {
        const auto turn = std::bit_cast<std::int32_t>(session.WorldScene().battle.turnSnapshot + 1u);
        cg::text::FormatTo(buffer, L"第 {} 回合", turn);
        const std::int32_t screenWidth = platform::gWindowWidth > 0 ? platform::gWindowWidth : 640;
        std::int32_t textWidth = render::TextMeasure(buffer, 0);
        if (textWidth <= 0) {
            textWidth = render::GetFontMetrics(0).sWidth
                * static_cast<std::int32_t>(text::ToGbk(buffer).size());
        }
        static_cast<void>(render::DrawText3_456560(static_cast<std::int16_t>((screenWidth - textWidth) / 2),
            static_cast<std::int16_t>(platform::gWindowHeight / 2 - 60), 0, 4, buffer, 0));
    }
}

// 原函数：0x00496570。
void WindowBattleCommand_SnapshotPetVitals(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    if (session.WorldScene().battle.currentPosition >= 20) {
        return;
    }
    for (std::size_t index = 0; index < inventory.battlePetVitals.size(); ++index) {
        auto& pet = (*session.WorldScene().petRecords)[index];
        inventory.battlePetVitals[index].hp = static_cast<std::int32_t>(pet.values0C[0]);
        inventory.battlePetVitals[index].maxHp = static_cast<std::int32_t>(pet.values0C[1]);
        inventory.battlePetVitals[index].fp = static_cast<std::int32_t>(pet.values0C[2]);
        inventory.battlePetVitals[index].maxFp = static_cast<std::int32_t>(pet.values0C[3]);
    }
}

// 原函数：0x004965D0。
void WindowBattleCommand_SyncActorVitals(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    const std::int32_t position = session.WorldScene().battle.currentPosition;
    if (position >= 20 || session.WorldScene().battle.snapshotPetIndex < 0) {
        return;
    }
    const std::int32_t petPosition = cg::world::GetBattlePetPosition_405B40(position);
    cg::yobi::YobiAction* const player = session.WorldScene().battle.entries[session.WorldScene().battle.currentPosition];
    if (player == nullptr) {
        return;
    }
    cg::yobi::YobiAction* const* const rider = cg::yobi::FindBattleRiderSlot(player);
    if (rider == nullptr || *rider != nullptr) {
        return;
    }
    const std::int32_t hp = cg::yobi::ReadYobiActionHpOnce(session.WorldScene().battle.entries[petPosition]);
    inventory.battlePetVitals[session.WorldScene().battle.snapshotPetIndex].hp = hp;
    const std::int32_t maxHp = cg::yobi::ReadYobiActionMaxHpOnce(session.WorldScene().battle.entries[petPosition]);
    const std::int32_t maxHpIndex = session.WorldScene().battle.snapshotPetIndex;
    cg::yobi::YobiAction* const fpAction = session.WorldScene().battle.entries[petPosition];
    inventory.battlePetVitals[maxHpIndex].maxHp = maxHp;
    const std::int32_t fp = cg::yobi::ReadYobiActionFpOnce(fpAction);
    inventory.battlePetVitals[session.WorldScene().battle.snapshotPetIndex].fp = fp;
    const std::int32_t maxFp = cg::yobi::ReadYobiActionMaxFpOnce(session.WorldScene().battle.entries[petPosition]);
    inventory.battlePetVitals[session.WorldScene().battle.snapshotPetIndex].maxFp = maxFp;
}

} // namespace detail

} // namespace cg::ui
