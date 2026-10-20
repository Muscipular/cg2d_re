#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
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
static void WindowBattleCommand_Open(std::int32_t mode, std::int16_t x, std::int16_t y); // 窗口26五模式创建/配置及入场偏移。

// 原函数：0x00496B90。
static std::int32_t WindowBattleCommand_Update(std::int32_t value); // 窗口26初始化、入退场与按钮显隐。

// 原函数：0x00496D50。
static std::int32_t WindowBattleCommand_Draw(std::int32_t unused); // 先绘制再更新技术名，按命中索引追加目标说明。

// 原函数：0x00496AE0。
static void WindowBattleCommand_Close(); // 窗口26存在才收尾、关闭10/27/28及清三展开引用，原返回无人消费。

// 原函数：0x00496F10。
static void WindowBattleCommand_ResetSelection(); // 先清九个选择槽，再关闭三子窗及清同源展开引用。

// 原函数：0x004972B0。
static std::int32_t WindowBattleCommand_ButtonAttack(std::int32_t index, std::int32_t state); // 攻击选择，原目标标志475H。

// 原函数：0x004973C0。
static std::int32_t WindowBattleCommand_ButtonDefend(std::int32_t index, std::int32_t state); // 防御G命令及玩家动作完成。

// 原函数：0x004974C0。
static void WindowBattleCommand_SelectSkill(std::int32_t skillIndex, std::int32_t technique); // CC4230真实双参数命令回调。

// 原函数：0x004975A0。
static std::int32_t WindowBattleCommand_ButtonSkill(std::int32_t index, std::int32_t state); // 技能按钮及选目标返回技术窗。

// 原函数：0x00497770。
static std::int32_t WindowBattleCommand_ButtonItem(std::int32_t index, std::int32_t state); // 物品选择/返回27，保留CC4D40独立门禁。

// 原函数：0x00497900。
static std::int32_t WindowBattleCommand_ButtonPet(std::int32_t index, std::int32_t state); // 宠物选择及28窗口关闭。

// 原函数：0x00497A60。
static std::int32_t WindowBattleCommand_ButtonFormation(std::int32_t index, std::int32_t state); // P命令，使用九槽中的槽6。

// 原函数：0x00497B80。
static std::int32_t WindowBattleCommand_ButtonEscape(std::int32_t index, std::int32_t state); // E命令，使用槽7，无已用技能门禁。

// 原函数：0x00497C80。
static std::int32_t WindowBattleCommand_ButtonRebirth(std::int32_t index, std::int32_t state); // TUC01：原载荷判空/hover持有，刷新后重取R命令状态。

// 原函数：0x00497E30。
static void WindowBattleCommand_SelectPetSkill(std::int32_t technique); // 宠物命令，保留重复查询与负参数略过。

// 原函数：0x00497FF0。
static void WindowBattleCommand_SelectRidingSkill(std::int32_t technique); // 骑乘命令，清选择模式并保留骑乘快照写序。

// 原函数：0x00498160。
static std::int32_t WindowBattleCommand_ButtonPetSkills(std::int32_t index, std::int32_t state); // 宠物/骑乘技能窗开关。

// 原函数：0x004982C0。
static void WindowBattleCommand_DrawTimer(); // 完整倒计时/超时发送，本帧超时后仍绘制。

// 原函数：0x0043C3E0。
static void WindowBattleCommand_DrawTargetHint(); // 553F80门禁及首个可选对象的目标提示，不要求鼠标命中。

// 原函数：0x0043C490。
static void WindowBattleCommand_ResetTargets(std::int32_t* values, std::int32_t count); // 正数count逐槽写-2，原caller弃返回。

// 原函数：0x00496B90。
static std::int32_t WindowBattleCommand_Update(std::int32_t value);

// 原函数：0x00496D50。
static std::int32_t WindowBattleCommand_Draw(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

// 原战斗子窗展开引用；CC423C独立于关闭入口处理的前三槽，不合并生命周期。
static cg::yobi::YobiAction* gBattleSkillWindowActionCC1A10 = nullptr; // 原全局：0x00CC1A10。

static cg::yobi::YobiAction* gBattleItemWindowActionD2F790 = nullptr; // 原全局：0x00D2F790。

static cg::yobi::YobiAction* gBattlePetWindowActionD2D60C = nullptr; // 原全局：0x00D2D60C。

static cg::yobi::YobiAction* gBattlePetSkillWindowActionCC423C = nullptr; // 原全局：0x00CC423C。

static std::uint8_t gBattleSkillTargetPendingD303F8 = 0; // 原全局：0x00D303F8。

static std::uint8_t gBattleValueCB054C = 0; // 原全局：0x00CB054C。

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
    const std::int32_t mode, std::int16_t x, std::int16_t y)
{
    AdaptWindowPosition(x, y, 299, 69);
    gWindowStates[WindowId_BattleCommand].actionFlags1C =
        (gWindowStates[WindowId_BattleCommand].actionFlags1C & ~0xFFu) | static_cast<std::uint8_t>(mode);
    WindowInstance* const window = gWindowStates[WindowId_BattleCommand].window;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    } else {
        gWindowStates[WindowId_BattleCommand].window = WindowRuntime_Open(WindowId_BattleCommand);
    }
    auto* elementsWindow = gWindowStates[WindowId_BattleCommand].window;
    const auto currentMode = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (!((elementsWindow && (*elementsWindow)[WindowBattleCommandElementId::Element14]))) { return; }
    auto* const graphic = (*elementsWindow)[WindowBattleCommandElementId::Element14]->data.graphic;
    switch (currentMode) {
    case 0:
    case 1:
    case 2:
        graphic->imageNo = 0x3B9C0 + currentMode;
        for (std::int32_t index = 0; index < 8; ++index) {
            if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[NativeElementKey(index)])) (*gWindowStates[WindowId_BattleCommand].window)[NativeElementKey(index)]->visible = 1;
        }
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills]->visible = 0;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonEscape_10])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonEscape_10]->visible = 0;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11]->visible = 0;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12]->visible = 0;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element13])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element13]->visible = 0;
        break;
    case 3: {
        graphic->imageNo = 0x3B9C3;
        for (std::int32_t index = 0; index < 8; ++index) {
            if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[NativeElementKey(index)])) (*gWindowStates[WindowId_BattleCommand].window)[NativeElementKey(index)]->visible = 0;
        }
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills]->visible = 1;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills]->x = 216;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonEscape_10])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonEscape_10]->visible = 0;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11]->visible = 1;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12]->visible = 1;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element13])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element13]->visible = 1;
        const std::int32_t namePosition = world::GetBattlePetPosition_405B40(
            world::gBattleCurrentPosition5ABCA4);
        const cg::yobi::YobiAction* const action = world::gBattleEntries59E980[namePosition];
        const wchar_t* name = cg::yobi::GetYobiTitle(action);
        if (name[0] == '\0') {
            name = cg::yobi::GetYobiName(action);
        }
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11]->data.text->text = name;
        const std::int32_t maximumPosition = world::GetBattlePetPosition_405B40(
            world::gBattleCurrentPosition5ABCA4);
        const std::int32_t maximum = cg::yobi::ReadYobiActionMaxFpOnce(
            world::gBattleEntries59E980[maximumPosition]);
        const std::int32_t currentPosition = world::GetBattlePetPosition_405B40(
            world::gBattleCurrentPosition5ABCA4);
        const std::int32_t current = cg::yobi::ReadYobiActionFpOnce(
            world::gBattleEntries59E980[currentPosition]);
        if (!((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12]))) { return; }
        cg::text::FormatTo((*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12]->data.text->text, L"{:4}/{:4}", current, maximum);
        break;
    }
    case 4: {
        graphic->imageNo = 0x3B9C3;
        for (std::int32_t index = 0; index < 8; ++index) {
            if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[NativeElementKey(index)])) (*gWindowStates[WindowId_BattleCommand].window)[NativeElementKey(index)]->visible = 0;
        }
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills]->visible = 1;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonPetSkills]->x = 146;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonEscape_10])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::ButtonEscape_10]->visible = 1;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11]->visible = 1;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12]->visible = 1;
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element13])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element13]->visible = 1;
        const std::int32_t namePosition = world::gBattleCurrentPosition5ABCA4;
        const wchar_t* const name = cg::yobi::GetYobiName(
            world::gBattleEntries59E980[namePosition]);
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element11]->data.text->text = name;
        const std::int32_t maximumPosition = world::gBattleCurrentPosition5ABCA4;
        const std::int32_t maximum = cg::yobi::ReadYobiActionMaxFpOnce(
            world::gBattleEntries59E980[maximumPosition]);
        const std::int32_t currentPosition = world::gBattleCurrentPosition5ABCA4;
        const std::int32_t current = cg::yobi::ReadYobiActionFpOnce(
            world::gBattleEntries59E980[currentPosition]);
        if (!((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12]))) { return; }
        cg::text::FormatTo((*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element12]->data.text->text, L"{:4}/{:4}", current, maximum);
        break;
    }
    default:
        break;
    }
    WindowInstance* const positionedWindow = gWindowStates[WindowId_BattleCommand].window;
    gWindowStates[WindowId_BattleCommand].x = x;
    gWindowStates[WindowId_BattleCommand].y = y;
    gWindowStates[WindowId_BattleCommand].value10 = std::bit_cast<std::uint32_t>(std::int32_t {-153});
    gWindowStates[WindowId_BattleCommand].value14 = 17;
    positionedWindow->x = x;
    const std::int32_t baseY = gWindowStates[WindowId_BattleCommand].y;
    const auto offset = std::bit_cast<std::int32_t>(gWindowStates[WindowId_BattleCommand].value10);
    gWindowStates[WindowId_BattleCommand].window->y = baseY + offset;
    gWindowStates[WindowId_BattleCommand].userState.value = 1;
}

// 原函数：0x00496B90。
static std::int32_t WindowBattleCommand_Update(const std::int32_t value)
{
    if (value == -1) {
        gWindowStates[WindowId_BattleCommand].value10 = 0;
        gWindowStates[WindowId_BattleCommand].value14 = 0;
        gWindowStates[WindowId_BattleCommand].userState.value = 0;
        gBattleSkillTargetPendingD303F8 = 0;
        gBattleItemTargetFlagCC4D40 = 0;
        return 1;
    }
    const std::uint32_t phase = gWindowStates[WindowId_BattleCommand].userState.value;
    if (phase == 1) {
        const auto oldOffset = std::bit_cast<std::int32_t>(gWindowStates[WindowId_BattleCommand].value10);
        const auto velocity = std::bit_cast<std::int32_t>(gWindowStates[WindowId_BattleCommand].value14);
        const std::int32_t offset = oldOffset + velocity;
        gWindowStates[WindowId_BattleCommand].value14 = std::bit_cast<std::uint32_t>(velocity - 1);
        const std::int32_t baseY = gWindowStates[WindowId_BattleCommand].y;
        gWindowStates[WindowId_BattleCommand].value10 = std::bit_cast<std::uint32_t>(offset);
        gCurrentWindow->y = baseY + offset;
        if (std::bit_cast<std::int32_t>(gWindowStates[WindowId_BattleCommand].value14) <= 0) {
            gWindowStates[WindowId_BattleCommand].value14 = 0;
            gWindowStates[WindowId_BattleCommand].userState.value = 0;
        }
    } else if (phase == 2) {
        const auto velocity = std::bit_cast<std::int32_t>(gWindowStates[WindowId_BattleCommand].value14);
        const auto oldOffset = std::bit_cast<std::int32_t>(gWindowStates[WindowId_BattleCommand].value10);
        const std::int32_t nextVelocity = velocity + 1;
        gWindowStates[WindowId_BattleCommand].value14 = std::bit_cast<std::uint32_t>(nextVelocity);
        const std::int32_t offset = oldOffset - nextVelocity;
        const std::int32_t baseY = gWindowStates[WindowId_BattleCommand].y;
        gWindowStates[WindowId_BattleCommand].value10 = std::bit_cast<std::uint32_t>(offset);
        gCurrentWindow->y = baseY + offset;
        if (std::bit_cast<std::int32_t>(gWindowStates[WindowId_BattleCommand].value14) >= 17) {
            gWindowStates[WindowId_BattleCommand].value14 = 17;
            gWindowStates[WindowId_BattleCommand].window->state |= 0x8000u;
            gWindowStates[WindowId_BattleCommand].userState.value = 3;
        }
        WindowInstance* window = gWindowStates[WindowId_Skill].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
        window = gWindowStates[WindowId_BattleItems].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
        window = gWindowStates[WindowId_BattlePets].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    auto mode = static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C);
    if (mode != 3 && mode != 4) {
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBattleCommandElementId::ButtonRebirth])) (*gCurrentWindow)[WindowBattleCommandElementId::ButtonRebirth]->visible = 1;
        mode = static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C);
    }
    if (mode == 2) {
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBattleCommandElementId::ButtonSkill])) (*gCurrentWindow)[WindowBattleCommandElementId::ButtonSkill]->visible = 1;
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBattleCommandElementId::ButtonItem])) (*gCurrentWindow)[WindowBattleCommandElementId::ButtonItem]->visible = 1;
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBattleCommandElementId::ButtonPet])) (*gCurrentWindow)[WindowBattleCommandElementId::ButtonPet]->visible = 1;
        if ((gCurrentWindow && (*gCurrentWindow)[WindowBattleCommandElementId::ButtonFormation])) (*gCurrentWindow)[WindowBattleCommandElementId::ButtonFormation]->visible = 1;
        if (world::gBattleSkillUsed5ABC68 != 0) {
            if ((gCurrentWindow && (*gCurrentWindow)[WindowBattleCommandElementId::ButtonSkill])) (*gCurrentWindow)[WindowBattleCommandElementId::ButtonSkill]->visible = 0;
            if ((gCurrentWindow && (*gCurrentWindow)[WindowBattleCommandElementId::ButtonItem])) (*gCurrentWindow)[WindowBattleCommandElementId::ButtonItem]->visible = 0;
            if ((gCurrentWindow && (*gCurrentWindow)[WindowBattleCommandElementId::ButtonPet])) (*gCurrentWindow)[WindowBattleCommandElementId::ButtonPet]->visible = 0;
            if ((gCurrentWindow && (*gCurrentWindow)[WindowBattleCommandElementId::ButtonFormation])) (*gCurrentWindow)[WindowBattleCommandElementId::ButtonFormation]->visible = 0;
        }
    }
    return 1;
}

// 原函数：0x00496D50。
static std::int32_t WindowBattleCommand_Draw(const std::int32_t)
{
    WindowRuntime_DrawCurrent();
    const auto mode = static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C);
    if (mode == 3 || mode == 4) {
        std::wstring_view name;
        if (world::gSelectedBattlePetTechniqueIndex_553F84 < 0) {
            name = {};
        } else {
            const std::int32_t pet = world::FindBattlePetIndex_43C4B0();
            const std::int32_t technique = world::gSelectedBattlePetTechniqueIndex_553F84;
            if (pet >= 0 && pet < static_cast<std::int32_t>(world::gPetRuntimeRecords.size())
                && technique < world::kPetTechniqueSlotCount)
                name = world::gPetRuntimeRecords[pet].techniquesD8[technique].name;
        }
        if ((gWindowStates[WindowId_BattleCommand].window && (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element13])) (*gWindowStates[WindowId_BattleCommand].window)[WindowBattleCommandElementId::Element13]->data.text->text = name;
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value == 0) {
        std::int32_t first = 0;
        for (; first < 20; ++first) {
            const cg::yobi::YobiAction* const action = world::gBattleEntries59E980[first];
            if (action != nullptr && (cg::yobi::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
                break;
            }
        }
        if (first < 20) {
            const std::int32_t hitIndex = render::gHoveredGraphicPendingIndex;
            std::int8_t selected = -1;
            for (std::int32_t index = 0; index < 20; ++index) {
                const cg::yobi::YobiAction* const action = world::gBattleEntries59E980[index];
                if (action != nullptr && cg::yobi::GetYobiActionCacheIndex(action) == hitIndex
                    && (cg::yobi::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
                    selected = static_cast<std::int8_t>(index);
                    break;
                }
            }
            WindowInstance* const textWindow = gCurrentWindow;
            const auto windowX = static_cast<std::int16_t>(textWindow->x);
            const auto windowY = static_cast<std::int16_t>(textWindow->y);
            const auto textX = static_cast<std::int16_t>(windowX + 17);
            const auto textY = static_cast<std::int16_t>(windowY + 89);
            std::wstring name = L"请选择对象";
            if (selected >= 0) {
                const cg::yobi::YobiAction* const action = world::gBattleEntries59E980[selected];
                const wchar_t* title = cg::yobi::GetYobiTitle(action);
                if (title[0] == '\0') {
                    title = cg::yobi::GetYobiName(action);
                }
                name = title;
            }
            static_cast<void>(render::DrawText_456780(textX, textY, 3, 0, 0, name, 0, 0));
            WindowInstance* const backgroundWindow = gCurrentWindow;
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
static void WindowBattleCommand_Close()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr) {
        return;
    }
    if (std::bit_cast<std::int32_t>(gWindowStates[WindowId_BattleCommand].value10) > 0) {
        gWindowStates[WindowId_BattleCommand].value10 = 0;
    }
    if (std::bit_cast<std::int32_t>(gWindowStates[WindowId_BattleCommand].value14) < 0) {
        gWindowStates[WindowId_BattleCommand].value14 = 0;
    }
    WindowInstance* window = gWindowStates[WindowId_Skill].window;
    gWindowStates[WindowId_BattleCommand].userState.value = 2;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    window = gWindowStates[WindowId_BattleItems].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    window = gWindowStates[WindowId_BattlePets].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    cg::yobi::YobiAction* const skillAction = gBattleSkillWindowActionCC1A10;
    if (skillAction != nullptr) {
        cg::yobi::MarkDeath(skillAction);
        gBattleSkillWindowActionCC1A10 = nullptr;
    }
    cg::yobi::YobiAction* const itemAction = gBattleItemWindowActionD2F790;
    if (itemAction != nullptr) {
        cg::yobi::MarkDeath(itemAction);
        gBattleItemWindowActionD2F790 = nullptr;
    }
    cg::yobi::YobiAction* const petAction = gBattlePetWindowActionD2D60C;
    if (petAction != nullptr) {
        cg::yobi::MarkDeath(petAction);
        gBattlePetWindowActionD2D60C = nullptr;
    }
}

// 原函数：0x00496F10。
static void WindowBattleCommand_ResetSelection()
{
    world::gBattleSelectionModes5ABC7C.fill(0);
    WindowInstance* window = gWindowStates[WindowId_Skill].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    window = gWindowStates[WindowId_BattleItems].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    window = gWindowStates[WindowId_BattlePets].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    cg::yobi::YobiAction* const skillWindow = gBattleSkillWindowActionCC1A10;
    if (skillWindow != nullptr) {
        cg::yobi::MarkDeath(skillWindow);
        gBattleSkillWindowActionCC1A10 = nullptr;
    }
    cg::yobi::YobiAction* const itemWindow = gBattleItemWindowActionD2F790;
    if (itemWindow != nullptr) {
        cg::yobi::MarkDeath(itemWindow);
        gBattleItemWindowActionD2F790 = nullptr;
    }
    cg::yobi::YobiAction* const petWindow = gBattlePetWindowActionD2D60C;
    if (petWindow != nullptr) {
        cg::yobi::MarkDeath(petWindow);
        gBattlePetWindowActionD2D60C = nullptr;
    }
}

// L05：Lua动作已发送后，沿既有战斗窗口和选择状态收尾顺序提交该回合。
void WindowBattleCommand_FinishAutoAction()
{
    gWindowStates[WindowId_BattleCommand].userState.value = 0;
    cg::world::gBattleCountdownActive5ABCB8 = 0;
    cg::yobi::FlushBattleEntries_43C060();
    WindowBattleCommand_Close();
    world::gCurrentSceneState = 5;
    if (cg::world::gBattleCommandAction5ABC00) cg::yobi::MarkDeath(cg::world::gBattleCommandAction5ABC00);
    cg::world::gBattleCommandAction5ABC00 = nullptr;
    cg::world::gBattlePetActionCount5ABBBC = 2;
    cg::world::gBattleSkillActionCount5ABBF4 = 2;
    cg::world::gBattleCommandState5ABC3C = 5;
    cg::world::gBattleSnapshotPetIndex5ABBFC = -1;
    WindowBattleCommand_ResetSelection();
    if (auto* window = gWindowStates[WindowId_BattleCommand].window) window->state |= 0x8000u;
}

// 原函数：0x004972B0。
static std::int32_t WindowBattleCommand_ButtonAttack(const std::int32_t index, const std::int32_t state)
{
    if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
    auto* const graphic = (*gCurrentWindow)[NativeElementKey(index)]->data.graphic;
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9C4;
        if (world::gBattleSelectionModes5ABC7C[0] != 0) {
            graphic->imageNo = 0x3B9C5;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"普通物理攻击。";
    }
    if ((state & 2) != 0) {
        WindowBattleCommand_ResetSelection();
        world::gBattleSelectionModes5ABC7C[0] = 1;
        world::gBattleSelectionMode5ABC70 = 0;
        cg::yobi::FlushBattleEntries_43C060();
        static_cast<void>(cg::yobi::CheckBattleCommand_43C560(
            world::gBattleCurrentPosition5ABCA4, 0x475u));
        cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
        world::gBattleCommandAction5ABC00 = nullptr;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        gBattleValueCB054C = 0;
        consumed = 1;
    }
    graphic->imageNo = 0x3B9C4;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9C6;
    }
    if (world::gBattleSelectionModes5ABC7C[0] != 0) {
        graphic->imageNo = 0x3B9C5;
    }
    return consumed;
}

// 原函数：0x004973C0。
static std::int32_t WindowBattleCommand_ButtonDefend(const std::int32_t index, const std::int32_t state)
{
    if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
    auto* const graphic = (*gCurrentWindow)[NativeElementKey(index)]->data.graphic;
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9D0;
        if (world::gBattleSelectionModes5ABC7C[1] != 0) {
            graphic->imageNo = 0x3B9D1;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"普通物理防御。";
    }
    if ((state & 2) != 0) {
        WindowBattleCommand_ResetSelection();
        world::gBattleSelectionModes5ABC7C[1] = 1;
        world::gBattleSelectionMode5ABC70 = 0;
        cg::yobi::FlushBattleEntries_43C060();
        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"G"));
        cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
        world::gBattleCommandAction5ABC00 = nullptr;
        world::gBattlePlayerActionState5ABC74 = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B9D0;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9D2;
    }
    if (world::gBattleSelectionModes5ABC7C[1] != 0) {
        graphic->imageNo = 0x3B9D1;
    }
    return consumed;
}

// 原函数：0x00497770。
static std::int32_t WindowBattleCommand_ButtonItem(const std::int32_t index, const std::int32_t state)
{
    if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
    auto* const graphic = (*gCurrentWindow)[NativeElementKey(index)]->data.graphic;
    if (world::gBattleSkillUsed5ABC68 != 0) {
        graphic->imageNo = 0x3B9D4;
        return 0;
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9D3;
        if (world::gBattleSelectionModes5ABC7C[3] != 0) {
            graphic->imageNo = 0x3B9D4;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"使用物品。";
    }
    if (world::gBattleSelectionModes5ABC7C[3] == 0) {
        if ((state & 2) != 0) {
            WindowBattleCommand_ResetSelection();
            world::gBattleSelectionMode5ABC70 = 3;
            world::gBattleSelectionModes5ABC7C[3] = 1;
            cg::yobi::FlushBattleEntries_43C060();
            cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
            world::gBattleCommandAction5ABC00 = nullptr;
            world::gBattleItemTargetPending5ABCC4 = 0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            gBattleValueCB054C = 0;
            gBattleItemWindowActionD2F790 = WindowRuntime_CreateAnimation(WindowId_BattleItems, 1, 0, nullptr);
            consumed = 1;
        }
    } else if ((*reinterpret_cast<const std::uint8_t*>(
        &platform::gLegacyMouseState.edgeButtons) & 2u) != 0 && gBattleItemTargetFlagCC4D40 != 0) {
        gBattleItemWindowActionD2F790 = WindowRuntime_CreateAnimation(WindowId_BattleItems, 1, 0, nullptr);
        gBattleItemTargetFlagCC4D40 = 0;
        cg::yobi::FlushBattleEntries_43C060();
    }
    graphic->imageNo = 0x3B9D3;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9D5;
    }
    if (world::gBattleSelectionModes5ABC7C[3] != 0) {
        graphic->imageNo = 0x3B9D4;
    }
    if (gWindowStates[WindowId_BattleItems].window != nullptr) {
        gBattleItemWindowActionD2F790 = nullptr;
    } else if (gBattleItemWindowActionD2F790 == nullptr && gBattleItemTargetFlagCC4D40 == 0) {
        world::gBattleSelectionModes5ABC7C[3] = 0;
    }
    return consumed;
}

// 原函数：0x00497900。
static std::int32_t WindowBattleCommand_ButtonPet(const std::int32_t index, const std::int32_t state)
{
    if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
    auto* const graphic = (*gCurrentWindow)[NativeElementKey(index)]->data.graphic;
    if (world::gBattleSkillUsed5ABC68 != 0) {
        graphic->imageNo = 0x3B9CB;
        return 0;
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9CA;
        if (world::gBattleSelectionModes5ABC7C[4] != 0) {
            graphic->imageNo = 0x3B9CB;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"召唤宠物或封印宠物。";
    }
    if (world::gBattleSelectionModes5ABC7C[4] == 0) {
        if ((state & 2) != 0) {
            WindowBattleCommand_ResetSelection();
            world::gBattleSelectionMode5ABC70 = 4;
            world::gBattleSelectionModes5ABC7C[4] = 1;
            cg::yobi::FlushBattleEntries_43C060();
            cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
            world::gBattleCommandAction5ABC00 = nullptr;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            gBattlePetWindowActionD2D60C = WindowRuntime_CreateAnimation(WindowId_BattlePets, 1, 0, nullptr);
            consumed = 1;
        }
    } else if ((*reinterpret_cast<const std::uint8_t*>(
        &platform::gLegacyMouseState.edgeButtons) & 2u) != 0) {
        WindowInstance* const window = gWindowStates[WindowId_BattlePets].window;
        if (window != nullptr) {
            window->state |= 0x8000u;
        }
    }
    graphic->imageNo = 0x3B9CA;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9CC;
    }
    if (world::gBattleSelectionModes5ABC7C[4] != 0) {
        graphic->imageNo = 0x3B9CB;
    }
    if (gWindowStates[WindowId_BattlePets].window != nullptr) {
        gBattlePetWindowActionD2D60C = nullptr;
    } else if (gBattlePetWindowActionD2D60C == nullptr) {
        world::gBattleSelectionModes5ABC7C[4] = 0;
    }
    return consumed;
}

// 原函数：0x00497A60。
static std::int32_t WindowBattleCommand_ButtonFormation(const std::int32_t index, const std::int32_t state)
{
    if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
    auto* const graphic = (*gCurrentWindow)[NativeElementKey(index)]->data.graphic;
    if (world::gBattleSkillUsed5ABC68 != 0) {
        graphic->imageNo = 0x3B9D7;
        return 0;
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9D6;
        if (world::gBattleSelectionModes5ABC7C[6] != 0) {
            graphic->imageNo = 0x3B9D7;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"变更队形。";
    }
    if ((state & 2) != 0) {
        WindowBattleCommand_ResetSelection();
        world::gBattleSelectionMode5ABC70 = 6;
        world::gBattleSelectionModes5ABC7C[6] = 1;
        cg::yobi::FlushBattleEntries_43C060();
        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"P"));
        cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
        world::gBattleCommandAction5ABC00 = nullptr;
        world::gBattlePlayerActionState5ABC74 = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B9D6;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9D8;
    }
    if (world::gBattleSelectionModes5ABC7C[6] != 0) {
        graphic->imageNo = 0x3B9D7;
    }
    return consumed;
}

// 原函数：0x00497B80。
static std::int32_t WindowBattleCommand_ButtonEscape(const std::int32_t index, const std::int32_t state)
{
    if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
    auto* const graphic = (*gCurrentWindow)[NativeElementKey(index)]->data.graphic;
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9D9;
        if (world::gBattleSelectionModes5ABC7C[7] != 0) {
            graphic->imageNo = 0x3B9DA;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"逃离此场战斗。";
    }
    if ((state & 2) != 0) {
        WindowBattleCommand_ResetSelection();
        world::gBattleSelectionModes5ABC7C[7] = 1;
        world::gBattleSelectionMode5ABC70 = 0;
        cg::yobi::FlushBattleEntries_43C060();
        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"E"));
        cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
        world::gBattleCommandAction5ABC00 = nullptr;
        world::gBattlePlayerActionState5ABC74 = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B9D9;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9DB;
    }
    if (world::gBattleSelectionModes5ABC7C[7] != 0) {
        graphic->imageNo = 0x3B9DA;
    }
    return consumed;
}

// TUC01：原战斗转生门槛/载荷判空保持；持有 hover，刷新后重取命令状态，发送弃返。
// 原函数：0x00497C80。
static std::int32_t WindowBattleCommand_ButtonRebirth(const std::int32_t index, const std::int32_t state)
{
    if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
    WindowElement* const element = (*gCurrentWindow)[NativeElementKey(index)];
    auto* const graphic = element->data.graphic;
    if (world::gBattleRebirthDisabled5ABC40 != 0
        || world::gPlayerRuntimeSlots.value08 == 0
        || world::gPlayerRuntimeSlots.timerRemaining0C <= 0) {
        element->visible = 0;
        graphic->imageNo = 0x3B9CE;
        return 0;
    }
    element->visible = 1;
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9CD;
        if (world::gBattleSelectionModes5ABC7C[8] != 0) {
            graphic->imageNo = 0x3B9CE;
        }
        return 0;
    }
    const cg::yobi::YobiAction* const action =
        world::gBattleEntries59E980[world::gBattleCurrentPosition5ABCA4];
    if (action == nullptr) {
        return 0;
    }
    cg::yobi::YobiAction* const* const rebirth = cg::yobi::FindBattleRebirthSlot(action);
    if (rebirth == nullptr) {
        return 0;
    }
    const bool hover = (state & 1) != 0;
    std::int32_t consumed = 0;
    if (hover) {
        gHoverTextD68AE8 = *rebirth != nullptr ? kSkillDisableRebirth5864B4 : kSkillEnableRebirth5864A0;
    }
    if ((state & 2) != 0) {
        WindowBattleCommand_ResetSelection();
        world::gBattleSelectionModes5ABC7C[8] = 1;
        world::gBattleSelectionMode5ABC70 = 8;
        cg::yobi::FlushBattleEntries_43C060();
        const cg::yobi::YobiAction* const current =
            world::gBattleEntries59E980[world::gBattleCurrentPosition5ABCA4];
        const char* const command =
            *cg::yobi::FindBattleRebirthSlot(current) != nullptr ? "R|0" : "R|1";
        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, cg::text::FromGbkWide(command)));
        cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
        world::gBattleCommandAction5ABC00 = nullptr;
        world::gBattlePlayerActionState5ABC74 = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B9CD;
    if (hover) {
        graphic->imageNo = 0x3B9CF;
    }
    if (world::gBattleSelectionModes5ABC7C[8] != 0) {
        graphic->imageNo = 0x3B9CE;
    }
    return consumed;
}

// 原函数：0x00496FB0。
std::int32_t WindowBattleCommand_CanSelectSkill()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::gBattleCurrentPosition5ABCA4 >= 20) {
        return 0;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::gBattleSkillUsed5ABC68 != 0) {
        return 0;
    }
    return world::gBattleSelectionModes5ABC7C[2] == 0 ? -1 : 0;
}

// 原函数：0x004970B0。
std::int32_t WindowBattleCommand_CanSelectItem()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::gBattleCurrentPosition5ABCA4 >= 20) {
        return 0;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::gBattleSkillUsed5ABC68 != 0) {
        return 0;
    }
    return world::gBattleSelectionModes5ABC7C[3] == 0 ? -1 : 0;
}

// 原函数：0x00497100。
void WindowBattleCommand_OpenItems()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::gBattleCurrentPosition5ABCA4 >= 20) {
        return;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        *reinterpret_cast<const std::uint8_t*>(&gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::gBattleSkillUsed5ABC68 != 0
        || world::gBattleSelectionModes5ABC7C[3] != 0) {
        return;
    }
    WindowBattleCommand_ResetSelection();
    world::gBattleSelectionModes5ABC7C[3] = 1;
    world::gBattleSelectionMode5ABC70 = 3;
    cg::yobi::FlushBattleEntries_43C060();
    cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
    world::gBattleCommandAction5ABC00 = nullptr;
    world::gBattleItemTargetPending5ABCC4 = 0;
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gBattleValueCB054C = 0;
    gBattleItemWindowActionD2F790 = WindowRuntime_CreateAnimation(WindowId_BattleItems, 1, 0, nullptr);
}

// 原函数：0x004971C0。
std::int32_t WindowBattleCommand_CanSelectPet()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::gBattleCurrentPosition5ABCA4 >= 20) {
        return 0;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::gBattleSkillUsed5ABC68 != 0) {
        return 0;
    }
    return world::gBattleSelectionModes5ABC7C[4] == 0 ? -1 : 0;
}

// 原函数：0x00497210。
void WindowBattleCommand_OpenPets()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::gBattleCurrentPosition5ABCA4 >= 20) {
        return;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        *reinterpret_cast<const std::uint8_t*>(&gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::gBattleSkillUsed5ABC68 != 0
        || world::gBattleSelectionModes5ABC7C[4] != 0) {
        return;
    }
    WindowBattleCommand_ResetSelection();
    world::gBattleSelectionModes5ABC7C[4] = 1;
    world::gBattleSelectionMode5ABC70 = 4;
    cg::yobi::FlushBattleEntries_43C060();
    cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
    world::gBattleCommandAction5ABC00 = nullptr;
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gBattlePetWindowActionD2D60C = WindowRuntime_CreateAnimation(WindowId_BattlePets, 1, 0, nullptr);
}

// 原函数：0x00497000。
void WindowBattleCommand_OpenSkills()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::gBattleCurrentPosition5ABCA4 >= 20) {
        return;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        *reinterpret_cast<const std::uint8_t*>(&gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::gBattleSkillUsed5ABC68 != 0
        || world::gBattleSelectionModes5ABC7C[2] != 0) {
        return;
    }
    WindowBattleCommand_ResetSelection();
    world::gBattleSelectionModes5ABC7C[2] = 1;
    world::gBattleSelectionMode5ABC70 = 2;
    cg::yobi::FlushBattleEntries_43C060();
    cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
    world::gBattleCommandAction5ABC00 = nullptr;
    world::gBattleValue5ABCC0 = 0;
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gBattleValueCB054C = 0;
    gBattleSkillWindowActionCC1A10 = WindowSkill_OpenBattleSkills(&WindowBattleCommand_SelectSkill);
}

// 原函数：0x004974C0。
static void WindowBattleCommand_SelectSkill(const std::int32_t skillIndex, const std::int32_t technique)
{
    world::gBattleSelectedSkill5ABB5C = skillIndex;
    world::gBattleSelectedTechnique5ABC78 = technique;
    const auto target = std::bit_cast<std::uint32_t>(
        world::gSkillsF1BAFC[skillIndex].techniques3C[technique].value84);
    const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
    if (cg::yobi::CheckBattleCommand_43C560(position, target) != 0) {
        gBattleSkillTargetPendingD303F8 = 1;
        return;
    }
    const std::int32_t currentPosition = world::gBattleCurrentPosition5ABCA4;
    const std::int32_t currentTechnique = world::gBattleSelectedTechnique5ABC78;
    const std::int32_t currentSkill = world::gBattleSelectedSkill5ABB5C;
    std::array<char, 768> buffer;
    std::sprintf(buffer.data(), "S|%X|%X|%X",
        static_cast<std::uint32_t>(currentSkill),
        static_cast<std::uint32_t>(currentTechnique),
        static_cast<std::uint32_t>(currentPosition));
    static_cast<void>(network::SendBattlePacket_5109B0(
        platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
    cg::yobi::FlushBattleEntries_43C060();
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    const std::int32_t count = world::gBattleSkillActionCount5ABBF4;
    world::gBattleCommandState5ABC3C = 2;
    world::gBattleSkillActionCount5ABBF4 = count + 1;
    cg::yobi::FlushBattleEntries_43C060();
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    WindowBattleCommand_Close();
}

// 原函数：0x004975A0。
static std::int32_t WindowBattleCommand_ButtonSkill(const std::int32_t index, const std::int32_t state)
{
    if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
    auto* const graphic = (*gCurrentWindow)[NativeElementKey(index)]->data.graphic;
    if (world::gBattleSkillUsed5ABC68 != 0) {
        graphic->imageNo = 0x3B9C8;
        return 0;
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9C7;
        if (world::gBattleSelectionModes5ABC7C[2] != 0) {
            graphic->imageNo = 0x3B9C8;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    const std::int32_t hover = state & 1;
    if (hover != 0) {
        gHoverTextD68AE8 = L"使用技能。";
    }
    if (world::gBattleSelectionModes5ABC7C[2] == 0) {
        if ((state & 2) != 0) {
            WindowBattleCommand_ResetSelection();
            world::gBattleSelectionMode5ABC70 = 2;
            world::gBattleSelectionModes5ABC7C[2] = 1;
            cg::yobi::FlushBattleEntries_43C060();
            cg::yobi::MarkDeath(world::gBattleCommandAction5ABC00);
            world::gBattleCommandAction5ABC00 = nullptr;
            world::gBattleValue5ABCC0 = 0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            gBattleValueCB054C = 0;
            gBattleSkillWindowActionCC1A10 = WindowSkill_OpenBattleSkills(&WindowBattleCommand_SelectSkill);
            consumed = 1;
        }
    } else if ((*reinterpret_cast<const std::uint8_t*>(
        &platform::gLegacyMouseState.edgeButtons) & 2u) != 0) {
        if (gBattleSkillTargetPendingD303F8 != 0) {
            gBattleSkillWindowActionCC1A10 = WindowSkill_OpenBattleTechniques(
                world::gBattleSelectedSkill5ABB5C, &WindowBattleCommand_SelectSkill);
            gBattleSkillTargetPendingD303F8 = 0;
            cg::yobi::FlushBattleEntries_43C060();
        } else if (gWindowStates[WindowId_Skill].window != nullptr) {
            if (WindowSkill_Back() != 0) {
                world::gBattleSelectionModes5ABC7C[2] = 0;
            }
        } else {
            gBattleSkillWindowActionCC1A10 = WindowSkill_OpenBattleSkills(&WindowBattleCommand_SelectSkill);
        }
    }
    graphic->imageNo = 0x3B9C7;
    if (hover != 0) {
        graphic->imageNo = 0x3B9C9;
    }
    if (world::gBattleSelectionModes5ABC7C[2] != 0) {
        graphic->imageNo = 0x3B9C8;
    }
    if (gWindowStates[WindowId_Skill].window != nullptr) {
        gBattleSkillWindowActionCC1A10 = nullptr;
    } else if (gBattleSkillWindowActionCC1A10 == nullptr && gBattleSkillTargetPendingD303F8 == 0) {
        world::gBattleSelectionModes5ABC7C[2] = 0;
    }
    return consumed;
}

// 原函数：0x00497E30。
static void WindowBattleCommand_SelectPetSkill(const std::int32_t technique)
{
    if (technique < 0) {
        world::gBattlePetCommandPhase553F88 = 0;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        if (world::gBattleCurrentPosition5ABCA4 < 20) {
            static_cast<void>(network::SendBattlePacket_5109B0(
                platform::gConnectionSocket, L"W|FF"));
        }
        cg::yobi::FlushBattleEntries_43C060();
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        const std::int32_t count = world::gBattlePetActionCount5ABBBC;
        world::gBattleCommandState5ABC3C = 5;
        world::gBattlePetActionCount5ABBBC = count + 1;
        world::gLastBattleCommandPetIndex_553F90 = world::FindBattlePetIndex_43C4B0();
        WindowBattleCommand_Close();
        return;
    }
    world::gSelectedBattlePetTechniqueIndex_553F84 = technique;
    const std::int32_t targetPet = world::FindBattlePetIndex_43C4B0();
    const std::int32_t targetTechnique = world::gSelectedBattlePetTechniqueIndex_553F84;
    const auto target = static_cast<std::uint32_t>(static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>(world::gPetRuntimeRecords[targetPet]
            .techniquesD8[targetTechnique].target)));
    const std::int32_t position = world::GetBattlePetPosition_405B40(
        world::gBattleCurrentPosition5ABCA4);
    if (cg::yobi::CheckBattleCommand_43C560(position, target) == 0) {
        const std::int32_t idPet = world::FindBattlePetIndex_43C4B0();
        const std::int32_t idTechnique = world::gSelectedBattlePetTechniqueIndex_553F84;
        const std::int32_t targetIndex =
            world::gPetRuntimeRecords[idPet].techniquesD8[idTechnique].techniqueId >= 200600
            ? world::GetBattlePetPosition_405B40(world::gBattleCurrentPosition5ABCA4)
            : world::FindBattlePetIndex_43C4B0();
        const std::int32_t currentTechnique = world::gSelectedBattlePetTechniqueIndex_553F84;
        std::array<char, 768> buffer;
        std::sprintf(buffer.data(), "W|%X|%X",
            static_cast<std::uint32_t>(currentTechnique), static_cast<std::uint32_t>(targetIndex));
        static_cast<void>(network::SendBattlePacket_5109B0(
            platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
        cg::yobi::FlushBattleEntries_43C060();
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        const std::int32_t count = world::gBattlePetActionCount5ABBBC;
        world::gBattleCommandState5ABC3C = 5;
        world::gBattlePetActionCount5ABBBC = count + 1;
        world::gLastBattleCommandPetIndex_553F90 = world::FindBattlePetIndex_43C4B0();
        WindowBattleCommand_Close();
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    world::gBattlePetCommandPhase553F88 = 2;
}

// 原函数：0x00497FF0。
static void WindowBattleCommand_SelectRidingSkill(const std::int32_t technique)
{
    if (technique < 0) {
        world::gBattlePetCommandPhase553F88 = 0;
        world::gBattleSelectionMode5ABC70 = 0;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        if (world::gBattleCurrentPosition5ABCA4 < 20) {
            static_cast<void>(network::SendBattlePacket_5109B0(
                platform::gConnectionSocket, L"W|FF"));
        }
        cg::yobi::FlushBattleEntries_43C060();
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        const std::int32_t count = world::gBattlePetActionCount5ABBBC;
        const auto pet = std::bit_cast<std::int32_t>(world::gBattleRidePet5ABC38);
        world::gBattlePetActionCount5ABBBC = count + 1;
        world::gBattleCommandState5ABC3C = 5;
        world::gLastBattleCommandPetIndex_553F90 = pet;
        WindowBattleCommand_Close();
        return;
    }
    world::gBattleSelectionMode5ABC70 = 0;
    const auto targetPet = std::bit_cast<std::int32_t>(world::gBattleRidePet5ABC38);
    world::gSelectedBattlePetTechniqueIndex_553F84 = technique;
    const auto target = static_cast<std::uint32_t>(static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>(world::gPetRuntimeRecords[targetPet]
            .techniquesD8[technique].target)));
    const std::int32_t position = world::gBattleCurrentPosition5ABCA4;
    if (cg::yobi::CheckBattleCommand_43C560(position, target) == 0) {
        const std::uint32_t pet = world::gBattleRidePet5ABC38;
        const std::int32_t currentTechnique = world::gSelectedBattlePetTechniqueIndex_553F84;
        std::array<char, 768> buffer;
        std::sprintf(buffer.data(), "W|%X|%X",
            static_cast<std::uint32_t>(currentTechnique), pet);
        static_cast<void>(network::SendBattlePacket_5109B0(
            platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
        cg::yobi::FlushBattleEntries_43C060();
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        const std::int32_t count = world::gBattlePetActionCount5ABBBC;
        const auto currentPet = std::bit_cast<std::int32_t>(world::gBattleRidePet5ABC38);
        world::gBattlePetActionCount5ABBBC = count + 1;
        world::gBattleCommandState5ABC3C = 5;
        world::gLastBattleCommandPetIndex_553F90 = currentPet;
        WindowBattleCommand_Close();
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    world::gBattlePetCommandPhase553F88 = 2;
}

// 原函数：0x00498160。
static std::int32_t WindowBattleCommand_ButtonPetSkills(const std::int32_t index, const std::int32_t state)
{
    if (!((gCurrentWindow && (*gCurrentWindow)[NativeElementKey(index)]))) { return {}; }
    auto* const graphic = (*gCurrentWindow)[NativeElementKey(index)]->data.graphic;
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    const std::int32_t entryState = state;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    const std::int32_t hover = entryState & 1;
    if (hover != 0 && window != nullptr) {
        gHoverTextD68AE8 = L"关闭技能视窗。";
    }
    cg::yobi::YobiAction* const pendingAction = gBattlePetSkillWindowActionCC423C;
    if (hover != 0 && window == nullptr && pendingAction == nullptr) {
        gHoverTextD68AE8 = L"开启技能视窗。";
    }
    if (pendingAction == nullptr
        && ((entryState & 2) != 0
            || (reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 2u) != 0)) {
        if (window != nullptr) {
            window->state |= 0x8000u;
        } else {
            cg::yobi::FlushBattleEntries_43C060();
            const std::uint8_t actionState =
                reinterpret_cast<const std::uint8_t*>(&gWindowStates[WindowId_BattleCommand].actionFlags1C)[0];
            world::gBattlePetCommandPhase553F88 = 1;
            const bool riding = actionState == 4;
            world::gSelectedBattlePetTechniqueIndex_553F84 = -1;
            gBattlePetSkillWindowActionCC423C = riding
                ? WindowSkill_OpenRidingSkills(&WindowBattleCommand_SelectRidingSkill) : WindowSkill_OpenPetSkills(&WindowBattleCommand_SelectPetSkill);
        }
        consumed = 1;
    }
    graphic->imageNo = 0x3B9C7;
    if (hover != 0) {
        graphic->imageNo = 0x3B9C9;
    }
    if (gWindowStates[WindowId_Skill].window != nullptr || gBattlePetSkillWindowActionCC423C != nullptr) {
        graphic->imageNo = 0x3B9C8;
        if (gWindowStates[WindowId_Skill].window != nullptr) {
            gBattlePetSkillWindowActionCC423C = nullptr;
        }
    }
    return consumed;
}

// 原函数：0x0043C3E0。
static void WindowBattleCommand_DrawTargetHint()
{
    if (world::gBattleCommandValue553F80 != 0) {
        return;
    }
    bool selected = false;
    for (const cg::yobi::YobiAction* const action : world::gBattleEntries59E980) {
        if (action != nullptr && (cg::yobi::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
            selected = true;
            break;
        }
    }
    if (!selected) {
        return;
    }
    const std::int32_t y = world::gBattleCommandState5ABC3C == 4 ? 88 : 91;
    static_cast<void>(cg::render::DrawGraphic(platform::gWindowWidth - 138, y + 20, 212, 0x58B1, 0, nullptr));
    static_cast<void>(render::DrawText2_456670(
        static_cast<std::int16_t>(platform::gWindowWidth - 257), static_cast<std::int16_t>(y + 12), 2, 0, 0,
        L"请选择目标", 0));
    gHoverTextD68AE8 = L"按滑鼠右键可取消。";
}

// 原函数：0x0043C490。
static void WindowBattleCommand_ResetTargets(std::int32_t* const values, const std::int32_t count)
{
    for (std::int32_t index = 0; index < count; ++index) {
        values[index] = -2;
    }
}

// 原函数：0x00498520。
void WindowBattleCommand_Dispatch()
{
    std::wstring buffer;
    const std::int32_t hitIndex = render::gHoveredGraphicPendingIndex;
    for (const cg::yobi::YobiAction* const action : cg::world::gBattleEntries59E980) {
        if (action != nullptr && cg::yobi::GetYobiActionCacheIndex(action) == hitIndex
            && (cg::yobi::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
            static_cast<void>(platform::RequestCursorState_506DE0(2, 0));
            break;
        }
    }
    WindowBattleCommand_DrawTargetHint();
    switch (cg::world::gBattleCommandState5ABC3C) {
    case 0: {
        std::int32_t position = cg::world::gBattleCurrentPosition5ABCA4;
        if (position < 20) {
            cg::yobi::YobiAction* const player = cg::world::gBattleEntries59E980[position];
            if (*cg::yobi::FindBattleRiderSlot(player) != nullptr) {
                if ((reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC)[0] & 8u) != 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    const std::int32_t petCount = cg::world::gBattlePetActionCount5ABBBC;
                    const std::uint32_t flags = cg::world::gBattlePlayerFlags5ABCAC;
                    const bool skipPlayer = (flags & 0x20000u) != 0;
                    cg::world::gBattlePetActionCount5ABBBC = petCount + 1;
                    if (skipPlayer) {
                        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                        const std::int32_t count = cg::world::gBattleSkillActionCount5ABBF4;
                        world::gCurrentSceneState = 5;
                        cg::world::gBattleSkillActionCount5ABBF4 = count + 1;
                    } else {
                        cg::world::gBattlePlayerFlags5ABCAC &= ~8u;
                        cg::world::gBattleRebirthDisabled5ABC40 = 0;
                    }
                    WindowBattleCommand_Close();
                    cg::world::gBattlePlayerActionState5ABC74 = 0;
                    break;
                }
                const std::int32_t technique = cg::world::gSelectedBattlePetTechniqueIndex_553F84;
                bool open = technique == -1 || cg::world::gBattleSelectionMode5ABC70 != 0;
                if (!open) {
                    const std::uint32_t flags = cg::world::gBattlePetSkillFlags5ABB78;
                    open = (flags & (1u << (static_cast<std::uint32_t>(technique) & 31u))) == 0;
                }
                if (!open) {
                    const auto pet = std::bit_cast<std::int32_t>(cg::world::gBattleRidePet5ABC38);
                    const std::int32_t cost = cg::world::gPetRuntimeRecords[pet].techniquesD8[technique].fp;
                    open = cg::yobi::IsCostGreaterThanYobiActionFp(cost, player);
                }
                if (!open) {
                    const auto pet = std::bit_cast<std::int32_t>(cg::world::gBattleRidePet5ABC38);
                    const std::int32_t currentTechnique = cg::world::gSelectedBattlePetTechniqueIndex_553F84;
                    const auto target =
                        std::bit_cast<std::int16_t>(cg::world::gPetRuntimeRecords[pet].techniquesD8[currentTechnique].target);
                    const std::int32_t currentPosition = cg::world::gBattleCurrentPosition5ABCA4;
                    open = cg::yobi::CheckBattleCommand_43C560(
                        currentPosition, static_cast<std::uint32_t>(target)) == 0;
                }
                if (open) {
                    cg::world::gBattlePetCommandPhase553F88 = 1;
                    cg::world::gSelectedBattlePetTechniqueIndex_553F84 = -1;
                    static_cast<void>(WindowSkill_OpenRidingSkills(&WindowBattleCommand_SelectRidingSkill));
                }
                cg::world::gBattlePetSkillGraphicIndex553F8C = -2;
                WindowBattleCommand_ResetSelection();
                const std::int32_t mode = cg::world::gBattleSelectionMode5ABC70;
                if (mode > 0) {
                    cg::world::gBattleSelectionModes5ABC7C[mode] = 1;
                    cg::yobi::FlushBattleEntries_43C060();
                } else if (mode == 0) {
                    cg::world::gBattleSelectionModes5ABC7C[0] = 1;
                }
                cg::world::gBattleCommandState5ABC3C = 4;
                WindowBattleCommand_Open(4, 341, 0);
                static_cast<void>(audio::PlaySe_52C450(56, 320));
                cg::world::gBattlePlayerActionState5ABC74 = 0;
                break;
            }
        }
        std::uint32_t flags = cg::world::gBattlePlayerFlags5ABCAC;
        if ((flags & 4u) != 0) {
            if (position < 20) {
                static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                position = cg::world::gBattleCurrentPosition5ABCA4;
                flags = cg::world::gBattlePlayerFlags5ABCAC;
            }
            const std::int32_t count = cg::world::gBattleSkillActionCount5ABBF4;
            cg::world::gBattleSkillActionCount5ABBF4 = count + 1;
            if ((flags & 0x20u) != 0) {
                cg::world::gBattleCommandState5ABC3C = 3;
            } else if ((flags & 0x10u) != 0) {
                if (position < 20) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                }
                const std::int32_t nextCount = cg::world::gBattleSkillActionCount5ABBF4;
                world::gCurrentSceneState = 5;
                cg::world::gBattleSkillActionCount5ABBF4 = nextCount + 1;
            } else {
                cg::world::gBattlePlayerFlags5ABCAC = flags & ~4u;
                cg::world::gBattleRebirthDisabled5ABC40 = 0;
            }
            WindowBattleCommand_Close();
            cg::world::gBattlePlayerActionState5ABC74 = 0;
            break;
        }
        if ((flags & 0x10u) != 0) {
            const std::int32_t count = cg::world::gBattleSkillActionCount5ABBF4;
            cg::world::gBattleRebirthDisabled5ABC40 = 0;
            cg::world::gBattleSelectionMode5ABC70 = count == 0 ? cg::world::gBattleFirstActionSelectionMode_5ABB70 : cg::world::gBattleSecondActionSelectionMode_5ABB6C;
        } else if (cg::world::gBattleSkillActionCount5ABBF4 == 0) {
            const std::int32_t mode = cg::world::gBattleFirstActionSelectionMode_5ABB70;
            cg::world::gBattleRebirthDisabled5ABC40 = 0;
            cg::world::gBattleSelectionMode5ABC70 = mode;
        } else {
            cg::world::gBattleSelectionMode5ABC70 = cg::world::gBattleSecondActionSelectionMode_5ABB6C;
        }
        WindowBattleCommand_ResetTargets(cg::world::gBattleResetValues5ABB84.data(), 9);
        WindowBattleCommand_ResetSelection();
        std::int32_t mode = cg::world::gBattleSelectionMode5ABC70;
        if (mode >= 0) {
            cg::world::gBattleSelectionModes5ABC7C[mode] = 1;
            cg::yobi::FlushBattleEntries_43C060();
            mode = cg::world::gBattleSelectionMode5ABC70;
        }
        switch (mode) {
        case 0:
            static_cast<void>(cg::yobi::CheckBattleCommand_43C560(
                cg::world::gBattleCurrentPosition5ABCA4, 0x475u));
            break;
        case 2:
            static_cast<void>(WindowSkill_OpenBattleSkills(&WindowBattleCommand_SelectSkill));
            break;
        case 3:
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_BattleItems, 1, 0, nullptr));
            break;
        case 4:
            static_cast<void>(WindowRuntime_CreateAnimation(WindowId_BattlePets, 1, 0, nullptr));
            break;
        default:
            break;
        }
        const auto modeFlags = reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC)[0];
        cg::world::gBattleCommandState5ABC3C = 1;
        std::int32_t commandType = 0;
        if ((modeFlags & 0x10u) == 0) {
            commandType = cg::world::gBattleSkillActionCount5ABBF4 != 0 ? 2 : 1;
        }
        WindowBattleCommand_Open(commandType, 341, 0);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        cg::world::gBattlePlayerActionState5ABC74 = 0;
        break;
    }
    case 1: {
        if (cg::world::gBattleSelectionModes5ABC7C[0] == 1 && (reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 2u) != 0) {
            cg::world::gBattleSelectionModes5ABC7C[0] = 0;
            cg::yobi::FlushBattleEntries_43C060();
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
        if (gBattleValueCB054C != 0) {
            const std::int32_t target = cg::yobi::GetSelectedBattleEntry_43C120();
            if (target != -1 && (reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 1u) != 0) {
                bool send = true;
                switch (cg::world::gBattleSelectionMode5ABC70) {
                case 0:
                    cg::text::FormatTo(buffer, L"H|{:X}", static_cast<std::uint32_t>(target));
                    break;
                case 2: {
                    const std::int32_t technique = cg::world::gBattleSelectedTechnique5ABC78;
                    const std::int32_t skill = cg::world::gBattleSelectedSkill5ABB5C;
                    cg::text::FormatTo(buffer, L"S|{:X}|{:X}|{:X}", static_cast<std::uint32_t>(skill), static_cast<std::uint32_t>(technique), static_cast<std::uint32_t>(target));
                    break;
                }
                case 3: {
                    const std::int32_t item = cg::world::gBattleSelectedItem5ABC48;
                    cg::text::FormatTo(buffer, L"I|{:X}|{:X}", static_cast<std::uint32_t>(item), static_cast<std::uint32_t>(target));
                    break;
                }
                default:
                    send = false;
                    break;
                }
                if (send) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, buffer));
                    cg::yobi::FlushBattleEntries_43C060();
                    cg::world::gBattlePlayerActionState5ABC74 = 1;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
            }
        }
        if (cg::world::gBattlePlayerActionState5ABC74 == 1) {
            const std::int32_t count = cg::world::gBattleSkillActionCount5ABBF4;
            cg::world::gBattleCommandState5ABC3C = 2;
            cg::world::gBattleSkillActionCount5ABBF4 = count + 1;
            cg::yobi::FlushBattleEntries_43C060();
            WindowBattleCommand_Close();
        }
        if (gBattleValueCB054C == 0) {
            gBattleValueCB054C = 1;
        }
        break;
    }
    case 2:
        if (gWindowStates[WindowId_BattleCommand].userState.value == 3) {
            const std::int32_t count = cg::world::gBattleSkillActionCount5ABBF4;
            cg::world::gBattlePlayerActionState5ABC74 = 0;
            if (count == 1) {
                const bool singleAction = (reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC)[0] & 0x10u) != 0;
                std::int32_t mode = cg::world::gBattleSelectionMode5ABC70;
                if (!singleAction) {
                    cg::world::gBattleCommandState5ABC3C = 0;
                    cg::world::gBattleFirstActionSelectionMode_5ABB70 = mode;
                    if (mode == 6) {
                        mode = 0;
                        cg::world::gBattleSkillUsed5ABC68 = 1;
                        cg::world::gBattleFirstActionSelectionMode_5ABB70 = 0;
                        cg::world::gBattleSecondActionSelectionMode_5ABB6C = 0;
                    } else if (mode >= 2 && mode <= 6) {
                        cg::world::gBattleSkillUsed5ABC68 = 1;
                        cg::world::gBattleSecondActionSelectionMode_5ABB6C = 0;
                    }
                } else {
                    cg::world::gBattleFirstActionSelectionMode_5ABB70 = mode;
                    if (mode == 6) {
                        mode = 0;
                        cg::world::gBattleFirstActionSelectionMode_5ABB70 = 0;
                    }
                    cg::world::gBattleCommandState5ABC3C = 3;
                }
                if (mode == 8) {
                    cg::world::gBattleRebirthDisabled5ABC40 = 1;
                }
            } else {
                const std::int32_t mode = cg::world::gBattleSelectionMode5ABC70;
                cg::world::gBattleSecondActionSelectionMode_5ABB6C = mode;
                if (mode == 6) {
                    cg::world::gBattleSecondActionSelectionMode_5ABB6C = 0;
                }
                world::gCurrentSceneState = 5;
            }
        }
        break;
    case 3: {
        const auto flags = reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC)[0];
        if ((flags & 0x20u) == 0) {
            world::gCurrentSceneState = 5;
            break;
        }
        if ((flags & 8u) != 0) {
            if (cg::world::gBattleCurrentPosition5ABCA4 < 20) {
                static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"W|FF"));
            }
            ++cg::world::gBattlePetActionCount5ABBBC;
            world::gCurrentSceneState = 5;
            break;
        }
        const std::int32_t technique = cg::world::gSelectedBattlePetTechniqueIndex_553F84;
        bool open = technique == -1
            || (cg::world::gBattlePetSkillFlags5ABB78 & (1u << (static_cast<std::uint32_t>(technique) & 31u))) == 0;
        if (!open) {
            const std::int32_t position = cg::world::GetBattlePetPosition_405B40(cg::world::gBattleCurrentPosition5ABCA4);
            cg::yobi::YobiAction* const action = cg::world::gBattleEntries59E980[position];
            const std::int32_t pet = cg::world::FindBattlePetIndex_43C4B0();
            const std::int32_t currentTechnique = cg::world::gSelectedBattlePetTechniqueIndex_553F84;
            const std::int32_t cost = cg::world::gPetRuntimeRecords[pet].techniquesD8[currentTechnique].fp;
            open = cg::yobi::IsCostGreaterThanYobiActionFp(cost, action);
        }
        if (!open) {
            const std::int32_t pet = cg::world::FindBattlePetIndex_43C4B0();
            const std::int32_t currentTechnique = cg::world::gSelectedBattlePetTechniqueIndex_553F84;
            const auto target =
                std::bit_cast<std::int16_t>(cg::world::gPetRuntimeRecords[pet].techniquesD8[currentTechnique].target);
            const std::int32_t position = cg::world::GetBattlePetPosition_405B40(cg::world::gBattleCurrentPosition5ABCA4);
            open = cg::yobi::CheckBattleCommand_43C560(position, static_cast<std::uint32_t>(target)) == 0;
        }
        if (open) {
            cg::world::gBattlePetCommandPhase553F88 = 1;
            cg::world::gSelectedBattlePetTechniqueIndex_553F84 = -1;
            static_cast<void>(WindowSkill_OpenPetSkills(&WindowBattleCommand_SelectPetSkill));
        }
        cg::world::gBattlePetSkillGraphicIndex553F8C = -2;
        cg::world::gBattleCommandState5ABC3C = 4;
        WindowBattleCommand_Open(3, 341, 0);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        break;
    }
    case 4: {
        if (cg::world::gBattlePetCommandPhase553F88 == 3) {
            const std::int32_t target = cg::yobi::GetSelectedBattleEntry_43C120();
            if (target != -1 && (reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 1u) != 0) {
                const std::int32_t technique = cg::world::gSelectedBattlePetTechniqueIndex_553F84;
                cg::text::FormatTo(buffer, L"W|{:X}|{:X}", static_cast<std::uint32_t>(technique), static_cast<std::uint32_t>(target));
                static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, buffer));
                cg::yobi::FlushBattleEntries_43C060();
                cg::world::gBattlePlayerActionState5ABC74 = 1;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
            }
        }
        std::int32_t phase = cg::world::gBattlePetCommandPhase553F88;
        if (phase == 2) {
            phase = 3;
            cg::world::gBattlePetCommandPhase553F88 = 3;
        }
        bool draw = false;
        std::int32_t imageNo = 0;
        switch (phase) {
        case 0: {
            if ((reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 2u) != 0) {
                const auto pet = std::bit_cast<std::int32_t>(cg::world::gBattleRidePet5ABC38);
                cg::world::gBattlePetCommandPhase553F88 = 1;
                if (pet < 0) {
                    static_cast<void>(WindowSkill_OpenPetSkills(&WindowBattleCommand_SelectPetSkill));
                } else {
                    static_cast<void>(WindowSkill_OpenRidingSkills(&WindowBattleCommand_SelectRidingSkill));
                }
            }
            const std::int32_t graphicIndex = cg::world::gBattlePetSkillGraphicIndex553F8C;
            const std::int32_t hover = render::gHoveredGraphicPendingIndex;
            if (graphicIndex == hover) {
                const auto clicks = reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0];
                gHoverTextD68AE8 = L"打开技能视窗。";
                if ((clicks & 1u) != 0) {
                    cg::world::gBattlePetCommandPhase553F88 = 1;
                }
            }
            imageNo = 0x589B;
            draw = true;
            break;
        }
        case 1: {
            const std::int32_t graphicIndex = cg::world::gBattlePetSkillGraphicIndex553F8C;
            const std::int32_t hover = render::gHoveredGraphicPendingIndex;
            if (graphicIndex == hover) {
                // 原498DF9复制12B，498DFD取点击位，498E07再复制末3B。
                gHoverTextD68AE8 = L"关闭技能视窗。";
                const auto clicks = reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0];
                static_cast<void>(0);
                if ((clicks & 1u) != 0) {
                    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
                    if (window != nullptr) {
                        window->state |= 0x8000u;
                    }
                    cg::world::gBattlePetCommandPhase553F88 = 0;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
            }
            imageNo = 0x589C;
            draw = true;
            break;
        }
        case 2:
        case 3: {
            if ((reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 2u) != 0) {
                cg::yobi::FlushBattleEntries_43C060();
                const auto pet = std::bit_cast<std::int32_t>(cg::world::gBattleRidePet5ABC38);
                cg::world::gBattlePetCommandPhase553F88 = 1;
                cg::world::gSelectedBattlePetTechniqueIndex_553F84 = -1;
                if (pet < 0) {
                    static_cast<void>(WindowSkill_OpenPetSkills(&WindowBattleCommand_SelectPetSkill));
                } else {
                    static_cast<void>(WindowSkill_OpenRidingSkills(&WindowBattleCommand_SelectRidingSkill));
                }
            }
            const std::int32_t graphicIndex = cg::world::gBattlePetSkillGraphicIndex553F8C;
            const std::int32_t hover = render::gHoveredGraphicPendingIndex;
            bool clickedReturn = false;
            if (graphicIndex == hover) {
                const auto clicks = reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0];
                gHoverTextD68AE8 = L"返回技能视窗。";
                if ((clicks & 1u) != 0) {
                    cg::yobi::FlushBattleEntries_43C060();
                    cg::world::gBattlePetCommandPhase553F88 = 1;
                    cg::world::gSelectedBattlePetTechniqueIndex_553F84 = -1;
                    clickedReturn = true;
                }
            }
            if (!clickedReturn && cg::world::gSelectedBattlePetTechniqueIndex_553F84 != -1) {
                const std::int32_t namePet = cg::world::FindBattlePetIndex_43C4B0();
                const std::int32_t nameTechnique = cg::world::gSelectedBattlePetTechniqueIndex_553F84;
                buffer = std::format(L"{:<16}", cg::world::gPetRuntimeRecords[namePet].techniquesD8[nameTechnique].name);
                const std::int32_t nameY = cg::world::gBattleCommandY553F7C;
                const std::int32_t nameX = cg::world::gBattleCommandX553F78;
                static_cast<void>(render::DrawText2_456670(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(nameX - 114)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(nameY - 15)),
                    2, 0, 0, buffer, 0));
                const std::int32_t costPet = cg::world::FindBattlePetIndex_43C4B0();
                const std::int32_t costTechnique = cg::world::gSelectedBattlePetTechniqueIndex_553F84;
                cg::text::FormatTo(buffer, L"{:4}", cg::world::gPetRuntimeRecords[costPet].techniquesD8[costTechnique].fp);
                const std::int32_t costY = cg::world::gBattleCommandY553F7C;
                const std::int32_t costX = cg::world::gBattleCommandX553F78;
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
            const std::int32_t y = cg::world::gBattleCommandY553F7C;
            const std::int32_t x = cg::world::gBattleCommandX553F78;
            cg::world::gBattlePetSkillGraphicIndex553F8C = cg::render::DrawGraphic(x, y, 212, imageNo, 2, nullptr);
        }
        if (cg::world::gBattlePlayerActionState5ABC74 == 1) {
            const std::int32_t count = cg::world::gBattlePetActionCount5ABBBC;
            cg::world::gBattleCommandState5ABC3C = 5;
            cg::world::gBattlePetActionCount5ABBBC = count + 1;
            cg::world::gLastBattleCommandPetIndex_553F90 = cg::world::FindBattlePetIndex_43C4B0();
            WindowBattleCommand_Close();
        }
        break;
    }
    case 5:
        if (gWindowStates[WindowId_BattleCommand].userState.value == 3) {
            cg::yobi::YobiAction* const player = cg::world::gBattleEntries59E980[cg::world::gBattleCurrentPosition5ABCA4];
            if (*cg::yobi::FindBattleRiderSlot(player) != nullptr && cg::world::gBattlePetActionCount5ABBBC < 2) {
                cg::world::gBattlePlayerActionState5ABC74 = 0;
                cg::world::gBattleCommandState5ABC3C = 0;
            } else {
                world::gCurrentSceneState = 5;
            }
        }
        break;
    default:
        break;
    }
    const auto flags = reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC)[0];
    if ((flags & 0x1Cu) != 0x1Cu) {
        WindowBattleCommand_DrawTimer();
    }
}

// 原函数：0x004982C0。
static void WindowBattleCommand_DrawTimer()
{
    const std::uint64_t tick = platform::ReadTickCount64();
    std::int64_t remaining = std::bit_cast<std::int64_t>(cg::world::gBattleCommandDeadline5ABBF8 - tick);
    if (cg::world::gBattleCountdownActive5ABCB8 == 0) {
        return;
    }
    if (remaining <= 0) {
        cg::world::gBattleCountdownActive5ABCB8 = 0;
        remaining = 0;
        cg::yobi::FlushBattleEntries_43C060();
        WindowBattleCommand_Close();
        cg::yobi::YobiAction* const player = cg::world::gBattleEntries59E980[cg::world::gBattleCurrentPosition5ABCA4];
        bool finish = false;
        if (*cg::yobi::FindBattleRiderSlot(player) != nullptr) {
            static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"W|FF"));
            finish = true;
        } else {
            std::uint8_t flags = reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC)[0];
            const std::int32_t skillCount = cg::world::gBattleSkillActionCount5ABBF4;
            if ((flags & 0x20u) != 0) {
                if (skillCount == 0 && (flags & 4u) == 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    finish = true;
                }
                if (cg::world::gBattlePetActionCount5ABBBC == 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"W|FF"));
                    finish = true;
                }
            } else if (skillCount != 0 || (flags & 4u) == 0) {
                if (skillCount == 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    flags = reinterpret_cast<const std::uint8_t*>(&cg::world::gBattlePlayerFlags5ABCAC)[0];
                    finish = true;
                }
                if (cg::world::gBattleSkillActionCount5ABBF4 == 1 && (flags & 4u) == 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    finish = true;
                }
            }
        }
        if (finish) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            cg::yobi::YobiAction* const action = cg::world::gBattleCommandAction5ABC00;
            world::gCurrentSceneState = 5;
            cg::yobi::MarkDeath(action);
            cg::world::gBattleCommandAction5ABC00 = nullptr;
        }
    }
    std::int32_t imageBase = 0x5690;
    if (remaining < 10000) {
        imageBase = 0x569A;
        const std::int32_t seconds = static_cast<std::int32_t>(remaining / 1000);
        if (seconds != cg::world::gBattleCountdownSecond5ABB80) {
            static_cast<void>(audio::PlaySe_52C450(202, 320));
            cg::world::gBattleCountdownSecond5ABB80 = seconds;
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
    if (cg::world::gBattleModeFlag5ABFCC == 1) {
        const auto turn = std::bit_cast<std::int32_t>(cg::world::gBattleTurnSnapshot5ABC08 + 1u);
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
void WindowBattleCommand_SnapshotPetVitals()
{
    if (world::gBattleCurrentPosition5ABCA4 >= 20) {
        return;
    }
    for (std::size_t index = 0; index < gBattlePetVitalsCF4B78.size(); ++index) {
        auto& pet = world::gPetRuntimeRecords[index];
        gBattlePetVitalsCF4B78[index].hp = static_cast<std::int32_t>(pet.values0C[0]);
        gBattlePetVitalsCF4B78[index].maxHp = static_cast<std::int32_t>(pet.values0C[1]);
        gBattlePetVitalsCF4B78[index].fp = static_cast<std::int32_t>(pet.values0C[2]);
        gBattlePetVitalsCF4B78[index].maxFp = static_cast<std::int32_t>(pet.values0C[3]);
    }
}

// 原函数：0x004965D0。
void WindowBattleCommand_SyncActorVitals()
{
    const std::int32_t position = cg::world::gBattleCurrentPosition5ABCA4;
    if (position >= 20 || cg::world::gBattleSnapshotPetIndex5ABBFC < 0) {
        return;
    }
    const std::int32_t petPosition = cg::world::GetBattlePetPosition_405B40(position);
    cg::yobi::YobiAction* const player = cg::world::gBattleEntries59E980[cg::world::gBattleCurrentPosition5ABCA4];
    if (player == nullptr) {
        return;
    }
    cg::yobi::YobiAction* const* const rider = cg::yobi::FindBattleRiderSlot(player);
    if (rider == nullptr || *rider != nullptr) {
        return;
    }
    const std::int32_t hp = cg::yobi::ReadYobiActionHpOnce(cg::world::gBattleEntries59E980[petPosition]);
    gBattlePetVitalsCF4B78[cg::world::gBattleSnapshotPetIndex5ABBFC].hp = hp;
    const std::int32_t maxHp = cg::yobi::ReadYobiActionMaxHpOnce(cg::world::gBattleEntries59E980[petPosition]);
    const std::int32_t maxHpIndex = cg::world::gBattleSnapshotPetIndex5ABBFC;
    cg::yobi::YobiAction* const fpAction = cg::world::gBattleEntries59E980[petPosition];
    gBattlePetVitalsCF4B78[maxHpIndex].maxHp = maxHp;
    const std::int32_t fp = cg::yobi::ReadYobiActionFpOnce(fpAction);
    gBattlePetVitalsCF4B78[cg::world::gBattleSnapshotPetIndex5ABBFC].fp = fp;
    const std::int32_t maxFp = cg::yobi::ReadYobiActionMaxFpOnce(cg::world::gBattleEntries59E980[petPosition]);
    gBattlePetVitalsCF4B78[cg::world::gBattleSnapshotPetIndex5ABBFC].maxFp = maxFp;
}

} // namespace detail

} // namespace cg::ui
