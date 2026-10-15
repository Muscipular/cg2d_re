#include "cg/render/GraphicPendingQueue.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

namespace cg::ui {
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
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};

static WindowTextPrototype gWindowText56B700 { // 原全局：0x0056B700。
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, L"魔力", // 原常量参考：0x005861C0。
};

static WindowTextPrototype gWindowText56B708 { // 原全局：0x0056B708。
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, L"技能", // 原常量参考：0x00586094。
};

static WindowElementPrototype gWindowElements56B710[14] { // 原全局：0x0056B710。
    {WindowElementType_Graphic, 13, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B5F8}, &WindowBattleCommand_ButtonAttack},
    {WindowElementType_Graphic, 13, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B610}, &WindowBattleCommand_ButtonDefend},
    {WindowElementType_Graphic, 83, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B628}, &WindowBattleCommand_ButtonSkill},
    {WindowElementType_Graphic, 83, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B640}, &WindowBattleCommand_ButtonItem},
    {WindowElementType_Graphic, 153, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B658}, &WindowBattleCommand_ButtonPet},
    {WindowElementType_Graphic, 153, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B670}, &WindowBattleCommand_ButtonFormation},
    {WindowElementType_Graphic, 223, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B688}, &WindowBattleCommand_ButtonEscape},
    {WindowElementType_Graphic, 223, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B6A0}, &WindowBattleCommand_ButtonRebirth},
    {WindowElementType_Graphic, 216, 46, 64, 20, 1, {.graphic = &gWindowGraphic56B628}, &WindowBattleCommand_ButtonPetSkills},
    {WindowElementType_Graphic, 216, 46, 64, 20, 1, {.graphic = &gWindowGraphic56B688}, &WindowBattleCommand_ButtonEscape},
    {WindowElementType_Text, 20, 24, 0, 0, 1, {.text = &gWindowText56B6F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 215, 24, 0, 0, 1, {.text = &gWindowText56B700}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 48, 0, 0, 1, {.text = &gWindowText56B708}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56B5E0}, &WindowElement_NoOp},
};

// 原型：窗口26；原全局：0x0054E720。
WindowPrototype gWindowPrototypeBattleCommand {2, 5, 341, 0, 299, 69, 0x80010101u, 14, gWindowElements56B710, &WindowBattleCommand_Update, &WindowBattleCommand_Draw, &WindowRuntime_ReleaseDefault}; // 26: 54E720

// 窗口函数实现。

// 原函数：0x004966B0。
static void WindowBattleCommand_Open(
    const std::int32_t mode, const std::int16_t x, const std::int16_t y)
{
    gWindowStates[WindowId_BattleCommand].actionFlags1C =
        (gWindowStates[WindowId_BattleCommand].actionFlags1C & ~0xFFu) | static_cast<std::uint8_t>(mode);
    WindowInstance* const window = gWindowStates[WindowId_BattleCommand].window;
    if (window != nullptr) {
        window->state &= ~0x8000u;
    } else {
        gWindowStates[WindowId_BattleCommand].window = WindowRuntime_Open(WindowId_BattleCommand);
    }
    WindowElement* const elements = gWindowStates[WindowId_BattleCommand].window->elements;
    const auto currentMode = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C));
    auto* const graphic = static_cast<WindowGraphicData*>(elements[13].data);
    switch (currentMode) {
    case 0:
    case 1:
    case 2:
        graphic->imageNo = 0x3B9C0 + currentMode;
        for (std::int32_t index = 0; index < 8; ++index) {
            gWindowStates[WindowId_BattleCommand].window->elements[index].visible = 1;
        }
        gWindowStates[WindowId_BattleCommand].window->elements[8].visible = 0;
        gWindowStates[WindowId_BattleCommand].window->elements[9].visible = 0;
        gWindowStates[WindowId_BattleCommand].window->elements[10].visible = 0;
        gWindowStates[WindowId_BattleCommand].window->elements[11].visible = 0;
        gWindowStates[WindowId_BattleCommand].window->elements[12].visible = 0;
        break;
    case 3: {
        graphic->imageNo = 0x3B9C3;
        for (std::int32_t index = 0; index < 8; ++index) {
            gWindowStates[WindowId_BattleCommand].window->elements[index].visible = 0;
        }
        gWindowStates[WindowId_BattleCommand].window->elements[8].visible = 1;
        gWindowStates[WindowId_BattleCommand].window->elements[8].x = 216;
        gWindowStates[WindowId_BattleCommand].window->elements[9].visible = 0;
        gWindowStates[WindowId_BattleCommand].window->elements[10].visible = 1;
        gWindowStates[WindowId_BattleCommand].window->elements[11].visible = 1;
        gWindowStates[WindowId_BattleCommand].window->elements[12].visible = 1;
        const std::int32_t namePosition = world::detail::GetBattlePetPosition_405B40(
            world::detail::gBattleCurrentPosition5ABCA4);
        const cg::yobi::YobiAction* const action = world::detail::gBattleEntries59E980[namePosition];
        const char* name = cg::yobi::detail::GetYobiTitle(action);
        if (name[0] == '\0') {
            name = cg::yobi::detail::GetYobiName(action);
        }
        static_cast<WindowTextData*>(gWindowStates[WindowId_BattleCommand].window->elements[10].data)->text = cg::text::FromGbkWide(name);
        const std::int32_t maximumPosition = world::detail::GetBattlePetPosition_405B40(
            world::detail::gBattleCurrentPosition5ABCA4);
        const std::int32_t maximum = cg::yobi::detail::ReadYobiActionMaxFpOnce(
            world::detail::gBattleEntries59E980[maximumPosition]);
        const std::int32_t currentPosition = world::detail::GetBattlePetPosition_405B40(
            world::detail::gBattleCurrentPosition5ABCA4);
        const std::int32_t current = cg::yobi::detail::ReadYobiActionFpOnce(
            world::detail::gBattleEntries59E980[currentPosition]);
        cg::text::FormatTo(static_cast<WindowTextData*>(gWindowStates[WindowId_BattleCommand].window->elements[11].data)->text, L"{:4}/{:4}", current, maximum);
        break;
    }
    case 4: {
        graphic->imageNo = 0x3B9C3;
        for (std::int32_t index = 0; index < 8; ++index) {
            gWindowStates[WindowId_BattleCommand].window->elements[index].visible = 0;
        }
        gWindowStates[WindowId_BattleCommand].window->elements[8].visible = 1;
        gWindowStates[WindowId_BattleCommand].window->elements[8].x = 146;
        gWindowStates[WindowId_BattleCommand].window->elements[9].visible = 1;
        gWindowStates[WindowId_BattleCommand].window->elements[10].visible = 1;
        gWindowStates[WindowId_BattleCommand].window->elements[11].visible = 1;
        gWindowStates[WindowId_BattleCommand].window->elements[12].visible = 1;
        const std::int32_t namePosition = world::detail::gBattleCurrentPosition5ABCA4;
        const char* const name = cg::yobi::detail::GetYobiName(
            world::detail::gBattleEntries59E980[namePosition]);
        static_cast<WindowTextData*>(gWindowStates[WindowId_BattleCommand].window->elements[10].data)->text = cg::text::FromGbkWide(name);
        const std::int32_t maximumPosition = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t maximum = cg::yobi::detail::ReadYobiActionMaxFpOnce(
            world::detail::gBattleEntries59E980[maximumPosition]);
        const std::int32_t currentPosition = world::detail::gBattleCurrentPosition5ABCA4;
        const std::int32_t current = cg::yobi::detail::ReadYobiActionFpOnce(
            world::detail::gBattleEntries59E980[currentPosition]);
        cg::text::FormatTo(static_cast<WindowTextData*>(gWindowStates[WindowId_BattleCommand].window->elements[11].data)->text, L"{:4}/{:4}", current, maximum);
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
        gCurrentWindow->elements[7].visible = 1;
        mode = static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C);
    }
    if (mode == 2) {
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[3].visible = 1;
        gCurrentWindow->elements[4].visible = 1;
        gCurrentWindow->elements[5].visible = 1;
        if (world::detail::gBattleSkillUsed5ABC68 != 0) {
            gCurrentWindow->elements[2].visible = 0;
            gCurrentWindow->elements[3].visible = 0;
            gCurrentWindow->elements[4].visible = 0;
            gCurrentWindow->elements[5].visible = 0;
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
        const char* name;
        if (world::detail::gValue553F84 < 0) {
            name = "";
        } else {
            const std::int32_t pet = world::detail::Sub_43C4B0();
            const std::int32_t technique = world::detail::gValue553F84;
            name = world::detail::gPetRuntimeRecords[pet].techniquesD8[technique].name.data();
        }
        static_cast<WindowTextData*>(gWindowStates[WindowId_BattleCommand].window->elements[12].data)->text = cg::text::FromGbkWide(name);
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value == 0) {
        std::int32_t first = 0;
        for (; first < 20; ++first) {
            const cg::yobi::YobiAction* const action = world::detail::gBattleEntries59E980[first];
            if (action != nullptr && (cg::yobi::detail::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
                break;
            }
        }
        if (first < 20) {
            const std::int32_t hitIndex = render::gHoveredGraphicPendingIndex;
            std::int8_t selected = -1;
            for (std::int32_t index = 0; index < 20; ++index) {
                const cg::yobi::YobiAction* const action = world::detail::gBattleEntries59E980[index];
                if (action != nullptr && cg::yobi::detail::GetYobiActionCacheIndex(action) == hitIndex
                    && (cg::yobi::detail::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
                    selected = static_cast<std::int8_t>(index);
                    break;
                }
            }
            WindowInstance* const textWindow = gCurrentWindow;
            const auto windowX = static_cast<std::int16_t>(textWindow->x);
            const auto windowY = static_cast<std::int16_t>(textWindow->y);
            const auto textX = static_cast<std::int16_t>(windowX + 17);
            const auto textY = static_cast<std::int16_t>(windowY + 89);
            std::wstring name = L"請選擇對象";
            if (selected >= 0) {
                const cg::yobi::YobiAction* const action = world::detail::gBattleEntries59E980[selected];
                const char* title = cg::yobi::detail::GetYobiTitle(action);
                if (title[0] == '\0') {
                    title = cg::yobi::detail::GetYobiName(action);
                }
                name = cg::text::FromGbkWide(title);
            }
            static_cast<void>(render::DrawText_456780(textX, textY, 3, 0, 0, name, 0, 0));
            WindowInstance* const backgroundWindow = gCurrentWindow;
            const auto backgroundWindowY = static_cast<std::int16_t>(backgroundWindow->y);
            const auto backgroundWindowX = static_cast<std::int16_t>(backgroundWindow->x);
            const auto backgroundY = static_cast<std::int16_t>(backgroundWindowY + 69);
            const auto backgroundX = static_cast<std::int16_t>(backgroundWindowX + 2);
            cg::yobi::detail::DrawNineGrid_49BE00(0x3BE84, backgroundX, backgroundY, 296, 61, 223,
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
        cg::yobi::detail::MarkDeath(skillAction);
        gBattleSkillWindowActionCC1A10 = nullptr;
    }
    cg::yobi::YobiAction* const itemAction = gBattleItemWindowActionD2F790;
    if (itemAction != nullptr) {
        cg::yobi::detail::MarkDeath(itemAction);
        gBattleItemWindowActionD2F790 = nullptr;
    }
    cg::yobi::YobiAction* const petAction = gBattlePetWindowActionD2D60C;
    if (petAction != nullptr) {
        cg::yobi::detail::MarkDeath(petAction);
        gBattlePetWindowActionD2D60C = nullptr;
    }
}

// 原函数：0x00496F10。
static void WindowBattleCommand_ResetSelection()
{
    world::detail::gBattleSelectionModes5ABC7C.fill(0);
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
        cg::yobi::detail::MarkDeath(skillWindow);
        gBattleSkillWindowActionCC1A10 = nullptr;
    }
    cg::yobi::YobiAction* const itemWindow = gBattleItemWindowActionD2F790;
    if (itemWindow != nullptr) {
        cg::yobi::detail::MarkDeath(itemWindow);
        gBattleItemWindowActionD2F790 = nullptr;
    }
    cg::yobi::YobiAction* const petWindow = gBattlePetWindowActionD2D60C;
    if (petWindow != nullptr) {
        cg::yobi::detail::MarkDeath(petWindow);
        gBattlePetWindowActionD2D60C = nullptr;
    }
}

// 原函数：0x004972B0。
static std::int32_t WindowBattleCommand_ButtonAttack(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9C4;
        if (world::detail::gBattleSelectionModes5ABC7C[0] != 0) {
            graphic->imageNo = 0x3B9C5;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"普通物理攻擊。";
    }
    if ((state & 2) != 0) {
        WindowBattleCommand_ResetSelection();
        world::detail::gBattleSelectionModes5ABC7C[0] = 1;
        world::detail::gBattleSelectionMode5ABC70 = 0;
        cg::yobi::detail::FlushBattleEntries_43C060();
        static_cast<void>(cg::yobi::detail::CheckBattleCommand_43C560(
            world::detail::gBattleCurrentPosition5ABCA4, 0x475u));
        cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
        world::detail::gBattleCommandAction5ABC00 = nullptr;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        gBattleValueCB054C = 0;
        consumed = 1;
    }
    graphic->imageNo = 0x3B9C4;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9C6;
    }
    if (world::detail::gBattleSelectionModes5ABC7C[0] != 0) {
        graphic->imageNo = 0x3B9C5;
    }
    return consumed;
}

// 原函数：0x004973C0。
static std::int32_t WindowBattleCommand_ButtonDefend(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9D0;
        if (world::detail::gBattleSelectionModes5ABC7C[1] != 0) {
            graphic->imageNo = 0x3B9D1;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"普通物理防禦。";
    }
    if ((state & 2) != 0) {
        WindowBattleCommand_ResetSelection();
        world::detail::gBattleSelectionModes5ABC7C[1] = 1;
        world::detail::gBattleSelectionMode5ABC70 = 0;
        cg::yobi::detail::FlushBattleEntries_43C060();
        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"G"));
        cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
        world::detail::gBattleCommandAction5ABC00 = nullptr;
        world::detail::gBattlePlayerActionState5ABC74 = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B9D0;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9D2;
    }
    if (world::detail::gBattleSelectionModes5ABC7C[1] != 0) {
        graphic->imageNo = 0x3B9D1;
    }
    return consumed;
}

// 原函数：0x00497770。
static std::int32_t WindowBattleCommand_ButtonItem(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if (world::detail::gBattleSkillUsed5ABC68 != 0) {
        graphic->imageNo = 0x3B9D4;
        return 0;
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9D3;
        if (world::detail::gBattleSelectionModes5ABC7C[3] != 0) {
            graphic->imageNo = 0x3B9D4;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"使用物品。";
    }
    if (world::detail::gBattleSelectionModes5ABC7C[3] == 0) {
        if ((state & 2) != 0) {
            WindowBattleCommand_ResetSelection();
            world::detail::gBattleSelectionMode5ABC70 = 3;
            world::detail::gBattleSelectionModes5ABC7C[3] = 1;
            cg::yobi::detail::FlushBattleEntries_43C060();
            cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
            world::detail::gBattleCommandAction5ABC00 = nullptr;
            world::detail::gBattleItemTargetPending5ABCC4 = 0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            gBattleValueCB054C = 0;
            gBattleItemWindowActionD2F790 = WindowRuntime_CreateAnimation(WindowId_BattleItems, 1, 0, nullptr);
            consumed = 1;
        }
    } else if ((*reinterpret_cast<const std::uint8_t*>(
        &platform::gLegacyMouseState.edgeButtons) & 2u) != 0 && gBattleItemTargetFlagCC4D40 != 0) {
        gBattleItemWindowActionD2F790 = WindowRuntime_CreateAnimation(WindowId_BattleItems, 1, 0, nullptr);
        gBattleItemTargetFlagCC4D40 = 0;
        cg::yobi::detail::FlushBattleEntries_43C060();
    }
    graphic->imageNo = 0x3B9D3;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9D5;
    }
    if (world::detail::gBattleSelectionModes5ABC7C[3] != 0) {
        graphic->imageNo = 0x3B9D4;
    }
    if (gWindowStates[WindowId_BattleItems].window != nullptr) {
        gBattleItemWindowActionD2F790 = nullptr;
    } else if (gBattleItemWindowActionD2F790 == nullptr && gBattleItemTargetFlagCC4D40 == 0) {
        world::detail::gBattleSelectionModes5ABC7C[3] = 0;
    }
    return consumed;
}

// 原函数：0x00497900。
static std::int32_t WindowBattleCommand_ButtonPet(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if (world::detail::gBattleSkillUsed5ABC68 != 0) {
        graphic->imageNo = 0x3B9CB;
        return 0;
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9CA;
        if (world::detail::gBattleSelectionModes5ABC7C[4] != 0) {
            graphic->imageNo = 0x3B9CB;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"召喚寵物或封印寵物。";
    }
    if (world::detail::gBattleSelectionModes5ABC7C[4] == 0) {
        if ((state & 2) != 0) {
            WindowBattleCommand_ResetSelection();
            world::detail::gBattleSelectionMode5ABC70 = 4;
            world::detail::gBattleSelectionModes5ABC7C[4] = 1;
            cg::yobi::detail::FlushBattleEntries_43C060();
            cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
            world::detail::gBattleCommandAction5ABC00 = nullptr;
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
    if (world::detail::gBattleSelectionModes5ABC7C[4] != 0) {
        graphic->imageNo = 0x3B9CB;
    }
    if (gWindowStates[WindowId_BattlePets].window != nullptr) {
        gBattlePetWindowActionD2D60C = nullptr;
    } else if (gBattlePetWindowActionD2D60C == nullptr) {
        world::detail::gBattleSelectionModes5ABC7C[4] = 0;
    }
    return consumed;
}

// 原函数：0x00497A60。
static std::int32_t WindowBattleCommand_ButtonFormation(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if (world::detail::gBattleSkillUsed5ABC68 != 0) {
        graphic->imageNo = 0x3B9D7;
        return 0;
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9D6;
        if (world::detail::gBattleSelectionModes5ABC7C[6] != 0) {
            graphic->imageNo = 0x3B9D7;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"變更隊形。";
    }
    if ((state & 2) != 0) {
        WindowBattleCommand_ResetSelection();
        world::detail::gBattleSelectionMode5ABC70 = 6;
        world::detail::gBattleSelectionModes5ABC7C[6] = 1;
        cg::yobi::detail::FlushBattleEntries_43C060();
        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"P"));
        cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
        world::detail::gBattleCommandAction5ABC00 = nullptr;
        world::detail::gBattlePlayerActionState5ABC74 = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B9D6;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9D8;
    }
    if (world::detail::gBattleSelectionModes5ABC7C[6] != 0) {
        graphic->imageNo = 0x3B9D7;
    }
    return consumed;
}

// 原函数：0x00497B80。
static std::int32_t WindowBattleCommand_ButtonEscape(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9D9;
        if (world::detail::gBattleSelectionModes5ABC7C[7] != 0) {
            graphic->imageNo = 0x3B9DA;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        gHoverTextD68AE8 = L"逃離此場戰鬥。";
    }
    if ((state & 2) != 0) {
        WindowBattleCommand_ResetSelection();
        world::detail::gBattleSelectionModes5ABC7C[7] = 1;
        world::detail::gBattleSelectionMode5ABC70 = 0;
        cg::yobi::detail::FlushBattleEntries_43C060();
        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"E"));
        cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
        world::detail::gBattleCommandAction5ABC00 = nullptr;
        world::detail::gBattlePlayerActionState5ABC74 = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B9D9;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B9DB;
    }
    if (world::detail::gBattleSelectionModes5ABC7C[7] != 0) {
        graphic->imageNo = 0x3B9DA;
    }
    return consumed;
}

// TUC01：原战斗转生门槛/载荷判空保持；持有 hover，刷新后重取命令状态，发送弃返。
// 原函数：0x00497C80。
static std::int32_t WindowBattleCommand_ButtonRebirth(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const element = &gCurrentWindow->elements[index];
    auto* const graphic = static_cast<WindowGraphicData*>(element->data);
    if (world::detail::gBattleRebirthDisabled5ABC40 != 0
        || world::detail::gPlayerRuntimeSlots.value08 == 0
        || world::detail::gPlayerRuntimeSlots.timerRemaining0C <= 0) {
        element->visible = 0;
        graphic->imageNo = 0x3B9CE;
        return 0;
    }
    element->visible = 1;
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9CD;
        if (world::detail::gBattleSelectionModes5ABC7C[8] != 0) {
            graphic->imageNo = 0x3B9CE;
        }
        return 0;
    }
    const cg::yobi::YobiAction* const action =
        world::detail::gBattleEntries59E980[world::detail::gBattleCurrentPosition5ABCA4];
    if (action == nullptr) {
        return 0;
    }
    cg::yobi::YobiAction* const* const rebirth = cg::yobi::detail::FindBattleRebirthSlot(action);
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
        world::detail::gBattleSelectionModes5ABC7C[8] = 1;
        world::detail::gBattleSelectionMode5ABC70 = 8;
        cg::yobi::detail::FlushBattleEntries_43C060();
        const cg::yobi::YobiAction* const current =
            world::detail::gBattleEntries59E980[world::detail::gBattleCurrentPosition5ABCA4];
        const char* const command =
            *cg::yobi::detail::FindBattleRebirthSlot(current) != nullptr ? "R|0" : "R|1";
        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, cg::text::FromGbkWide(command)));
        cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
        world::detail::gBattleCommandAction5ABC00 = nullptr;
        world::detail::gBattlePlayerActionState5ABC74 = 1;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        consumed = 1;
    }
    graphic->imageNo = 0x3B9CD;
    if (hover) {
        graphic->imageNo = 0x3B9CF;
    }
    if (world::detail::gBattleSelectionModes5ABC7C[8] != 0) {
        graphic->imageNo = 0x3B9CE;
    }
    return consumed;
}

// 原函数：0x00496FB0。
std::int32_t WindowBattleCommand_CanSelectSkill()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::detail::gBattleCurrentPosition5ABCA4 >= 20) {
        return 0;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::detail::gBattleSkillUsed5ABC68 != 0) {
        return 0;
    }
    return world::detail::gBattleSelectionModes5ABC7C[2] == 0 ? -1 : 0;
}

// 原函数：0x004970B0。
std::int32_t WindowBattleCommand_CanSelectItem()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::detail::gBattleCurrentPosition5ABCA4 >= 20) {
        return 0;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::detail::gBattleSkillUsed5ABC68 != 0) {
        return 0;
    }
    return world::detail::gBattleSelectionModes5ABC7C[3] == 0 ? -1 : 0;
}

// 原函数：0x00497100。
void WindowBattleCommand_OpenItems()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::detail::gBattleCurrentPosition5ABCA4 >= 20) {
        return;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        *reinterpret_cast<const std::uint8_t*>(&gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::detail::gBattleSkillUsed5ABC68 != 0
        || world::detail::gBattleSelectionModes5ABC7C[3] != 0) {
        return;
    }
    WindowBattleCommand_ResetSelection();
    world::detail::gBattleSelectionModes5ABC7C[3] = 1;
    world::detail::gBattleSelectionMode5ABC70 = 3;
    cg::yobi::detail::FlushBattleEntries_43C060();
    cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
    world::detail::gBattleCommandAction5ABC00 = nullptr;
    world::detail::gBattleItemTargetPending5ABCC4 = 0;
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gBattleValueCB054C = 0;
    gBattleItemWindowActionD2F790 = WindowRuntime_CreateAnimation(WindowId_BattleItems, 1, 0, nullptr);
}

// 原函数：0x004971C0。
std::int32_t WindowBattleCommand_CanSelectPet()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::detail::gBattleCurrentPosition5ABCA4 >= 20) {
        return 0;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::detail::gBattleSkillUsed5ABC68 != 0) {
        return 0;
    }
    return world::detail::gBattleSelectionModes5ABC7C[4] == 0 ? -1 : 0;
}

// 原函数：0x00497210。
void WindowBattleCommand_OpenPets()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::detail::gBattleCurrentPosition5ABCA4 >= 20) {
        return;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        *reinterpret_cast<const std::uint8_t*>(&gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::detail::gBattleSkillUsed5ABC68 != 0
        || world::detail::gBattleSelectionModes5ABC7C[4] != 0) {
        return;
    }
    WindowBattleCommand_ResetSelection();
    world::detail::gBattleSelectionModes5ABC7C[4] = 1;
    world::detail::gBattleSelectionMode5ABC70 = 4;
    cg::yobi::detail::FlushBattleEntries_43C060();
    cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
    world::detail::gBattleCommandAction5ABC00 = nullptr;
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gBattlePetWindowActionD2D60C = WindowRuntime_CreateAnimation(WindowId_BattlePets, 1, 0, nullptr);
}

// 原函数：0x00497000。
void WindowBattleCommand_OpenSkills()
{
    if (gWindowStates[WindowId_BattleCommand].window == nullptr
        || gWindowStates[WindowId_BattleCommand].userState.value != 0
        || world::detail::gBattleCurrentPosition5ABCA4 >= 20) {
        return;
    }
    const auto actionState = std::bit_cast<std::int8_t>(
        *reinterpret_cast<const std::uint8_t*>(&gWindowStates[WindowId_BattleCommand].actionFlags1C));
    if (actionState < 0 || actionState > 2
        || world::detail::gBattleSkillUsed5ABC68 != 0
        || world::detail::gBattleSelectionModes5ABC7C[2] != 0) {
        return;
    }
    WindowBattleCommand_ResetSelection();
    world::detail::gBattleSelectionModes5ABC7C[2] = 1;
    world::detail::gBattleSelectionMode5ABC70 = 2;
    cg::yobi::detail::FlushBattleEntries_43C060();
    cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
    world::detail::gBattleCommandAction5ABC00 = nullptr;
    world::detail::gBattleValue5ABCC0 = 0;
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    gBattleValueCB054C = 0;
    gBattleSkillWindowActionCC1A10 = WindowSkill_OpenBattleSkills(&WindowBattleCommand_SelectSkill);
}

// 原函数：0x004974C0。
static void WindowBattleCommand_SelectSkill(const std::int32_t skillIndex, const std::int32_t technique)
{
    world::detail::gBattleSelectedSkill5ABB5C = skillIndex;
    world::detail::gBattleSelectedTechnique5ABC78 = technique;
    const auto target = std::bit_cast<std::uint32_t>(
        world::detail::gSkillsF1BAFC[skillIndex].techniques3C[technique].value84);
    const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
    if (cg::yobi::detail::CheckBattleCommand_43C560(position, target) != 0) {
        gBattleSkillTargetPendingD303F8 = 1;
        return;
    }
    const std::int32_t currentPosition = world::detail::gBattleCurrentPosition5ABCA4;
    const std::int32_t currentTechnique = world::detail::gBattleSelectedTechnique5ABC78;
    const std::int32_t currentSkill = world::detail::gBattleSelectedSkill5ABB5C;
    std::array<char, 768> buffer;
    std::sprintf(buffer.data(), "S|%X|%X|%X",
        static_cast<std::uint32_t>(currentSkill),
        static_cast<std::uint32_t>(currentTechnique),
        static_cast<std::uint32_t>(currentPosition));
    static_cast<void>(network::SendBattlePacket_5109B0(
        platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
    cg::yobi::detail::FlushBattleEntries_43C060();
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    const std::int32_t count = world::detail::gBattleSkillActionCount5ABBF4;
    world::detail::gBattleCommandState5ABC3C = 2;
    world::detail::gBattleSkillActionCount5ABBF4 = count + 1;
    cg::yobi::detail::FlushBattleEntries_43C060();
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    WindowBattleCommand_Close();
}

// 原函数：0x004975A0。
static std::int32_t WindowBattleCommand_ButtonSkill(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if (world::detail::gBattleSkillUsed5ABC68 != 0) {
        graphic->imageNo = 0x3B9C8;
        return 0;
    }
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        graphic->imageNo = 0x3B9C7;
        if (world::detail::gBattleSelectionModes5ABC7C[2] != 0) {
            graphic->imageNo = 0x3B9C8;
        }
        return 0;
    }
    std::int32_t consumed = 0;
    const std::int32_t hover = state & 1;
    if (hover != 0) {
        gHoverTextD68AE8 = L"使用技能。";
    }
    if (world::detail::gBattleSelectionModes5ABC7C[2] == 0) {
        if ((state & 2) != 0) {
            WindowBattleCommand_ResetSelection();
            world::detail::gBattleSelectionMode5ABC70 = 2;
            world::detail::gBattleSelectionModes5ABC7C[2] = 1;
            cg::yobi::detail::FlushBattleEntries_43C060();
            cg::yobi::detail::MarkDeath(world::detail::gBattleCommandAction5ABC00);
            world::detail::gBattleCommandAction5ABC00 = nullptr;
            world::detail::gBattleValue5ABCC0 = 0;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            gBattleValueCB054C = 0;
            gBattleSkillWindowActionCC1A10 = WindowSkill_OpenBattleSkills(&WindowBattleCommand_SelectSkill);
            consumed = 1;
        }
    } else if ((*reinterpret_cast<const std::uint8_t*>(
        &platform::gLegacyMouseState.edgeButtons) & 2u) != 0) {
        if (gBattleSkillTargetPendingD303F8 != 0) {
            gBattleSkillWindowActionCC1A10 = WindowSkill_OpenBattleTechniques(
                world::detail::gBattleSelectedSkill5ABB5C, &WindowBattleCommand_SelectSkill);
            gBattleSkillTargetPendingD303F8 = 0;
            cg::yobi::detail::FlushBattleEntries_43C060();
        } else if (gWindowStates[WindowId_Skill].window != nullptr) {
            if (WindowSkill_Back() != 0) {
                world::detail::gBattleSelectionModes5ABC7C[2] = 0;
            }
        } else {
            gBattleSkillWindowActionCC1A10 = WindowSkill_OpenBattleSkills(&WindowBattleCommand_SelectSkill);
        }
    }
    graphic->imageNo = 0x3B9C7;
    if (hover != 0) {
        graphic->imageNo = 0x3B9C9;
    }
    if (world::detail::gBattleSelectionModes5ABC7C[2] != 0) {
        graphic->imageNo = 0x3B9C8;
    }
    if (gWindowStates[WindowId_Skill].window != nullptr) {
        gBattleSkillWindowActionCC1A10 = nullptr;
    } else if (gBattleSkillWindowActionCC1A10 == nullptr && gBattleSkillTargetPendingD303F8 == 0) {
        world::detail::gBattleSelectionModes5ABC7C[2] = 0;
    }
    return consumed;
}

// 原函数：0x00497E30。
static void WindowBattleCommand_SelectPetSkill(const std::int32_t technique)
{
    if (technique < 0) {
        world::detail::gBattlePetCommandPhase553F88 = 0;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        if (world::detail::gBattleCurrentPosition5ABCA4 < 20) {
            static_cast<void>(network::SendBattlePacket_5109B0(
                platform::gConnectionSocket, L"W|FF"));
        }
        cg::yobi::detail::FlushBattleEntries_43C060();
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        const std::int32_t count = world::detail::gBattlePetActionCount5ABBBC;
        world::detail::gBattleCommandState5ABC3C = 5;
        world::detail::gBattlePetActionCount5ABBBC = count + 1;
        world::detail::gValue553F90 = world::detail::Sub_43C4B0();
        WindowBattleCommand_Close();
        return;
    }
    world::detail::gValue553F84 = technique;
    const std::int32_t targetPet = world::detail::Sub_43C4B0();
    const std::int32_t targetTechnique = world::detail::gValue553F84;
    const auto target = static_cast<std::uint32_t>(static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>(world::detail::gPetRuntimeRecords[targetPet]
            .techniquesD8[targetTechnique].target)));
    const std::int32_t position = world::detail::GetBattlePetPosition_405B40(
        world::detail::gBattleCurrentPosition5ABCA4);
    if (cg::yobi::detail::CheckBattleCommand_43C560(position, target) == 0) {
        const std::int32_t idPet = world::detail::Sub_43C4B0();
        const std::int32_t idTechnique = world::detail::gValue553F84;
        const std::int32_t targetIndex =
            world::detail::gPetRuntimeRecords[idPet].techniquesD8[idTechnique].techniqueId >= 200600
            ? world::detail::GetBattlePetPosition_405B40(world::detail::gBattleCurrentPosition5ABCA4)
            : world::detail::Sub_43C4B0();
        const std::int32_t currentTechnique = world::detail::gValue553F84;
        std::array<char, 768> buffer;
        std::sprintf(buffer.data(), "W|%X|%X",
            static_cast<std::uint32_t>(currentTechnique), static_cast<std::uint32_t>(targetIndex));
        static_cast<void>(network::SendBattlePacket_5109B0(
            platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
        cg::yobi::detail::FlushBattleEntries_43C060();
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        const std::int32_t count = world::detail::gBattlePetActionCount5ABBBC;
        world::detail::gBattleCommandState5ABC3C = 5;
        world::detail::gBattlePetActionCount5ABBBC = count + 1;
        world::detail::gValue553F90 = world::detail::Sub_43C4B0();
        WindowBattleCommand_Close();
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    world::detail::gBattlePetCommandPhase553F88 = 2;
}

// 原函数：0x00497FF0。
static void WindowBattleCommand_SelectRidingSkill(const std::int32_t technique)
{
    if (technique < 0) {
        world::detail::gBattlePetCommandPhase553F88 = 0;
        world::detail::gBattleSelectionMode5ABC70 = 0;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        if (world::detail::gBattleCurrentPosition5ABCA4 < 20) {
            static_cast<void>(network::SendBattlePacket_5109B0(
                platform::gConnectionSocket, L"W|FF"));
        }
        cg::yobi::detail::FlushBattleEntries_43C060();
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        const std::int32_t count = world::detail::gBattlePetActionCount5ABBBC;
        const auto pet = std::bit_cast<std::int32_t>(world::detail::gBattleRidePet5ABC38);
        world::detail::gBattlePetActionCount5ABBBC = count + 1;
        world::detail::gBattleCommandState5ABC3C = 5;
        world::detail::gValue553F90 = pet;
        WindowBattleCommand_Close();
        return;
    }
    world::detail::gBattleSelectionMode5ABC70 = 0;
    const auto targetPet = std::bit_cast<std::int32_t>(world::detail::gBattleRidePet5ABC38);
    world::detail::gValue553F84 = technique;
    const auto target = static_cast<std::uint32_t>(static_cast<std::int32_t>(
        std::bit_cast<std::int16_t>(world::detail::gPetRuntimeRecords[targetPet]
            .techniquesD8[technique].target)));
    const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
    if (cg::yobi::detail::CheckBattleCommand_43C560(position, target) == 0) {
        const std::uint32_t pet = world::detail::gBattleRidePet5ABC38;
        const std::int32_t currentTechnique = world::detail::gValue553F84;
        std::array<char, 768> buffer;
        std::sprintf(buffer.data(), "W|%X|%X",
            static_cast<std::uint32_t>(currentTechnique), pet);
        static_cast<void>(network::SendBattlePacket_5109B0(
            platform::gConnectionSocket, cg::text::FromGbkWide(buffer.data())));
        cg::yobi::detail::FlushBattleEntries_43C060();
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        const std::int32_t count = world::detail::gBattlePetActionCount5ABBBC;
        const auto currentPet = std::bit_cast<std::int32_t>(world::detail::gBattleRidePet5ABC38);
        world::detail::gBattlePetActionCount5ABBBC = count + 1;
        world::detail::gBattleCommandState5ABC3C = 5;
        world::detail::gValue553F90 = currentPet;
        WindowBattleCommand_Close();
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    world::detail::gBattlePetCommandPhase553F88 = 2;
}

// 原函数：0x00498160。
static std::int32_t WindowBattleCommand_ButtonPetSkills(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    if (gWindowStates[WindowId_BattleCommand].userState.value != 0) {
        return 0;
    }
    std::int32_t consumed = 0;
    const std::int32_t entryState = state;
    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
    const std::int32_t hover = entryState & 1;
    if (hover != 0 && window != nullptr) {
        gHoverTextD68AE8 = L"關閉技能視窗。";
    }
    cg::yobi::YobiAction* const pendingAction = gBattlePetSkillWindowActionCC423C;
    if (hover != 0 && window == nullptr && pendingAction == nullptr) {
        gHoverTextD68AE8 = L"開啟技能視窗。";
    }
    if (pendingAction == nullptr
        && ((entryState & 2) != 0
            || (reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 2u) != 0)) {
        if (window != nullptr) {
            window->state |= 0x8000u;
        } else {
            cg::yobi::detail::FlushBattleEntries_43C060();
            const std::uint8_t actionState =
                reinterpret_cast<const std::uint8_t*>(&gWindowStates[WindowId_BattleCommand].actionFlags1C)[0];
            world::detail::gBattlePetCommandPhase553F88 = 1;
            const bool riding = actionState == 4;
            world::detail::gValue553F84 = -1;
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
    if (world::detail::gBattleCommandValue553F80 != 0) {
        return;
    }
    bool selected = false;
    for (const cg::yobi::YobiAction* const action : world::detail::gBattleEntries59E980) {
        if (action != nullptr && (cg::yobi::detail::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
            selected = true;
            break;
        }
    }
    if (!selected) {
        return;
    }
    const std::int32_t y = world::detail::gBattleCommandState5ABC3C == 4 ? 88 : 91;
    static_cast<void>(cg::render::DrawGraphic(502, y + 20, 212, 0x58B1, 0, nullptr));
    static_cast<void>(render::DrawText2_456670(
        383, static_cast<std::int16_t>(y + 12), 2, 0, 0,
        L"請選擇目標", 0));
    gHoverTextD68AE8 = L"按滑鼠右鍵可取消。";
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
    using namespace world::detail;
    std::wstring buffer;
    const std::int32_t hitIndex = render::gHoveredGraphicPendingIndex;
    for (const cg::yobi::YobiAction* const action : gBattleEntries59E980) {
        if (action != nullptr && cg::yobi::detail::GetYobiActionCacheIndex(action) == hitIndex
            && (cg::yobi::detail::GetYobiActionFlagsLowByte(action) & 8u) != 0) {
            static_cast<void>(platform::RequestCursorState_506DE0(2, 0));
            break;
        }
    }
    WindowBattleCommand_DrawTargetHint();
    switch (gBattleCommandState5ABC3C) {
    case 0: {
        std::int32_t position = gBattleCurrentPosition5ABCA4;
        if (position < 20) {
            cg::yobi::YobiAction* const player = gBattleEntries59E980[position];
            if (*cg::yobi::detail::FindBattleRiderSlot(player) != nullptr) {
                if ((reinterpret_cast<const std::uint8_t*>(&gBattlePlayerFlags5ABCAC)[0] & 8u) != 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    const std::int32_t petCount = gBattlePetActionCount5ABBBC;
                    const std::uint32_t flags = gBattlePlayerFlags5ABCAC;
                    const bool skipPlayer = (flags & 0x20000u) != 0;
                    gBattlePetActionCount5ABBBC = petCount + 1;
                    if (skipPlayer) {
                        static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                        const std::int32_t count = gBattleSkillActionCount5ABBF4;
                        world::gCurrentSceneState = 5;
                        gBattleSkillActionCount5ABBF4 = count + 1;
                    } else {
                        gBattlePlayerFlags5ABCAC &= ~8u;
                        gBattleRebirthDisabled5ABC40 = 0;
                    }
                    WindowBattleCommand_Close();
                    gBattlePlayerActionState5ABC74 = 0;
                    break;
                }
                const std::int32_t technique = gValue553F84;
                bool open = technique == -1 || gBattleSelectionMode5ABC70 != 0;
                if (!open) {
                    const std::uint32_t flags = gBattlePetSkillFlags5ABB78;
                    open = (flags & (1u << (static_cast<std::uint32_t>(technique) & 31u))) == 0;
                }
                if (!open) {
                    const auto pet = std::bit_cast<std::int32_t>(gBattleRidePet5ABC38);
                    const std::int32_t cost = gPetRuntimeRecords[pet].techniquesD8[technique].fp;
                    open = cg::yobi::detail::IsCostGreaterThanYobiActionFp(cost, player);
                }
                if (!open) {
                    const auto pet = std::bit_cast<std::int32_t>(gBattleRidePet5ABC38);
                    const std::int32_t currentTechnique = gValue553F84;
                    const auto target =
                        std::bit_cast<std::int16_t>(gPetRuntimeRecords[pet].techniquesD8[currentTechnique].target);
                    const std::int32_t currentPosition = gBattleCurrentPosition5ABCA4;
                    open = cg::yobi::detail::CheckBattleCommand_43C560(
                        currentPosition, static_cast<std::uint32_t>(target)) == 0;
                }
                if (open) {
                    gBattlePetCommandPhase553F88 = 1;
                    gValue553F84 = -1;
                    static_cast<void>(WindowSkill_OpenRidingSkills(&WindowBattleCommand_SelectRidingSkill));
                }
                gBattlePetSkillGraphicIndex553F8C = -2;
                WindowBattleCommand_ResetSelection();
                const std::int32_t mode = gBattleSelectionMode5ABC70;
                if (mode > 0) {
                    gBattleSelectionModes5ABC7C[mode] = 1;
                    cg::yobi::detail::FlushBattleEntries_43C060();
                } else if (mode == 0) {
                    gBattleSelectionModes5ABC7C[0] = 1;
                }
                gBattleCommandState5ABC3C = 4;
                WindowBattleCommand_Open(4, 341, 0);
                static_cast<void>(audio::PlaySe_52C450(56, 320));
                gBattlePlayerActionState5ABC74 = 0;
                break;
            }
        }
        std::uint32_t flags = gBattlePlayerFlags5ABCAC;
        if ((flags & 4u) != 0) {
            if (position < 20) {
                static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                position = gBattleCurrentPosition5ABCA4;
                flags = gBattlePlayerFlags5ABCAC;
            }
            const std::int32_t count = gBattleSkillActionCount5ABBF4;
            gBattleSkillActionCount5ABBF4 = count + 1;
            if ((flags & 0x20u) != 0) {
                gBattleCommandState5ABC3C = 3;
            } else if ((flags & 0x10u) != 0) {
                if (position < 20) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                }
                const std::int32_t nextCount = gBattleSkillActionCount5ABBF4;
                world::gCurrentSceneState = 5;
                gBattleSkillActionCount5ABBF4 = nextCount + 1;
            } else {
                gBattlePlayerFlags5ABCAC = flags & ~4u;
                gBattleRebirthDisabled5ABC40 = 0;
            }
            WindowBattleCommand_Close();
            gBattlePlayerActionState5ABC74 = 0;
            break;
        }
        if ((flags & 0x10u) != 0) {
            const std::int32_t count = gBattleSkillActionCount5ABBF4;
            gBattleRebirthDisabled5ABC40 = 0;
            gBattleSelectionMode5ABC70 = count == 0 ? gValue5ABB70 : gValue5ABB6C;
        } else if (gBattleSkillActionCount5ABBF4 == 0) {
            const std::int32_t mode = gValue5ABB70;
            gBattleRebirthDisabled5ABC40 = 0;
            gBattleSelectionMode5ABC70 = mode;
        } else {
            gBattleSelectionMode5ABC70 = gValue5ABB6C;
        }
        WindowBattleCommand_ResetTargets(gBattleResetValues5ABB84.data(), 9);
        WindowBattleCommand_ResetSelection();
        std::int32_t mode = gBattleSelectionMode5ABC70;
        if (mode >= 0) {
            gBattleSelectionModes5ABC7C[mode] = 1;
            cg::yobi::detail::FlushBattleEntries_43C060();
            mode = gBattleSelectionMode5ABC70;
        }
        switch (mode) {
        case 0:
            static_cast<void>(cg::yobi::detail::CheckBattleCommand_43C560(
                gBattleCurrentPosition5ABCA4, 0x475u));
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
        const auto modeFlags = reinterpret_cast<const std::uint8_t*>(&gBattlePlayerFlags5ABCAC)[0];
        gBattleCommandState5ABC3C = 1;
        std::int32_t commandType = 0;
        if ((modeFlags & 0x10u) == 0) {
            commandType = gBattleSkillActionCount5ABBF4 != 0 ? 2 : 1;
        }
        WindowBattleCommand_Open(commandType, 341, 0);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        gBattlePlayerActionState5ABC74 = 0;
        break;
    }
    case 1: {
        if (gBattleSelectionModes5ABC7C[0] == 1 && (reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 2u) != 0) {
            gBattleSelectionModes5ABC7C[0] = 0;
            cg::yobi::detail::FlushBattleEntries_43C060();
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
        if (gBattleValueCB054C != 0) {
            const std::int32_t target = cg::yobi::detail::GetSelectedBattleEntry_43C120();
            if (target != -1 && (reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 1u) != 0) {
                bool send = true;
                switch (gBattleSelectionMode5ABC70) {
                case 0:
                    cg::text::FormatTo(buffer, L"H|{:X}", static_cast<std::uint32_t>(target));
                    break;
                case 2: {
                    const std::int32_t technique = gBattleSelectedTechnique5ABC78;
                    const std::int32_t skill = gBattleSelectedSkill5ABB5C;
                    cg::text::FormatTo(buffer, L"S|{:X}|{:X}|{:X}", static_cast<std::uint32_t>(skill), static_cast<std::uint32_t>(technique), static_cast<std::uint32_t>(target));
                    break;
                }
                case 3: {
                    const std::int32_t item = gBattleSelectedItem5ABC48;
                    cg::text::FormatTo(buffer, L"I|{:X}|{:X}", static_cast<std::uint32_t>(item), static_cast<std::uint32_t>(target));
                    break;
                }
                default:
                    send = false;
                    break;
                }
                if (send) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, buffer));
                    cg::yobi::detail::FlushBattleEntries_43C060();
                    gBattlePlayerActionState5ABC74 = 1;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
            }
        }
        if (gBattlePlayerActionState5ABC74 == 1) {
            const std::int32_t count = gBattleSkillActionCount5ABBF4;
            gBattleCommandState5ABC3C = 2;
            gBattleSkillActionCount5ABBF4 = count + 1;
            cg::yobi::detail::FlushBattleEntries_43C060();
            WindowBattleCommand_Close();
        }
        if (gBattleValueCB054C == 0) {
            gBattleValueCB054C = 1;
        }
        break;
    }
    case 2:
        if (gWindowStates[WindowId_BattleCommand].userState.value == 3) {
            const std::int32_t count = gBattleSkillActionCount5ABBF4;
            gBattlePlayerActionState5ABC74 = 0;
            if (count == 1) {
                const bool singleAction = (reinterpret_cast<const std::uint8_t*>(&gBattlePlayerFlags5ABCAC)[0] & 0x10u) != 0;
                std::int32_t mode = gBattleSelectionMode5ABC70;
                if (!singleAction) {
                    gBattleCommandState5ABC3C = 0;
                    gValue5ABB70 = mode;
                    if (mode == 6) {
                        mode = 0;
                        gBattleSkillUsed5ABC68 = 1;
                        gValue5ABB70 = 0;
                        gValue5ABB6C = 0;
                    } else if (mode >= 2 && mode <= 6) {
                        gBattleSkillUsed5ABC68 = 1;
                        gValue5ABB6C = 0;
                    }
                } else {
                    gValue5ABB70 = mode;
                    if (mode == 6) {
                        mode = 0;
                        gValue5ABB70 = 0;
                    }
                    gBattleCommandState5ABC3C = 3;
                }
                if (mode == 8) {
                    gBattleRebirthDisabled5ABC40 = 1;
                }
            } else {
                const std::int32_t mode = gBattleSelectionMode5ABC70;
                gValue5ABB6C = mode;
                if (mode == 6) {
                    gValue5ABB6C = 0;
                }
                world::gCurrentSceneState = 5;
            }
        }
        break;
    case 3: {
        const auto flags = reinterpret_cast<const std::uint8_t*>(&gBattlePlayerFlags5ABCAC)[0];
        if ((flags & 0x20u) == 0) {
            world::gCurrentSceneState = 5;
            break;
        }
        if ((flags & 8u) != 0) {
            if (gBattleCurrentPosition5ABCA4 < 20) {
                static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"W|FF"));
            }
            ++gBattlePetActionCount5ABBBC;
            world::gCurrentSceneState = 5;
            break;
        }
        const std::int32_t technique = gValue553F84;
        bool open = technique == -1
            || (gBattlePetSkillFlags5ABB78 & (1u << (static_cast<std::uint32_t>(technique) & 31u))) == 0;
        if (!open) {
            const std::int32_t position = GetBattlePetPosition_405B40(gBattleCurrentPosition5ABCA4);
            cg::yobi::YobiAction* const action = gBattleEntries59E980[position];
            const std::int32_t pet = Sub_43C4B0();
            const std::int32_t currentTechnique = gValue553F84;
            const std::int32_t cost = gPetRuntimeRecords[pet].techniquesD8[currentTechnique].fp;
            open = cg::yobi::detail::IsCostGreaterThanYobiActionFp(cost, action);
        }
        if (!open) {
            const std::int32_t pet = Sub_43C4B0();
            const std::int32_t currentTechnique = gValue553F84;
            const auto target =
                std::bit_cast<std::int16_t>(gPetRuntimeRecords[pet].techniquesD8[currentTechnique].target);
            const std::int32_t position = GetBattlePetPosition_405B40(gBattleCurrentPosition5ABCA4);
            open = cg::yobi::detail::CheckBattleCommand_43C560(position, static_cast<std::uint32_t>(target)) == 0;
        }
        if (open) {
            gBattlePetCommandPhase553F88 = 1;
            gValue553F84 = -1;
            static_cast<void>(WindowSkill_OpenPetSkills(&WindowBattleCommand_SelectPetSkill));
        }
        gBattlePetSkillGraphicIndex553F8C = -2;
        gBattleCommandState5ABC3C = 4;
        WindowBattleCommand_Open(3, 341, 0);
        static_cast<void>(audio::PlaySe_52C450(56, 320));
        break;
    }
    case 4: {
        if (gBattlePetCommandPhase553F88 == 3) {
            const std::int32_t target = cg::yobi::detail::GetSelectedBattleEntry_43C120();
            if (target != -1 && (reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 1u) != 0) {
                const std::int32_t technique = gValue553F84;
                cg::text::FormatTo(buffer, L"W|{:X}|{:X}", static_cast<std::uint32_t>(technique), static_cast<std::uint32_t>(target));
                static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, buffer));
                cg::yobi::detail::FlushBattleEntries_43C060();
                gBattlePlayerActionState5ABC74 = 1;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
            }
        }
        std::int32_t phase = gBattlePetCommandPhase553F88;
        if (phase == 2) {
            phase = 3;
            gBattlePetCommandPhase553F88 = 3;
        }
        bool draw = false;
        std::int32_t imageNo = 0;
        switch (phase) {
        case 0: {
            if ((reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0] & 2u) != 0) {
                const auto pet = std::bit_cast<std::int32_t>(gBattleRidePet5ABC38);
                gBattlePetCommandPhase553F88 = 1;
                if (pet < 0) {
                    static_cast<void>(WindowSkill_OpenPetSkills(&WindowBattleCommand_SelectPetSkill));
                } else {
                    static_cast<void>(WindowSkill_OpenRidingSkills(&WindowBattleCommand_SelectRidingSkill));
                }
            }
            const std::int32_t graphicIndex = gBattlePetSkillGraphicIndex553F8C;
            const std::int32_t hover = render::gHoveredGraphicPendingIndex;
            if (graphicIndex == hover) {
                const auto clicks = reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0];
                gHoverTextD68AE8 = L"打開技能視窗。";
                if ((clicks & 1u) != 0) {
                    gBattlePetCommandPhase553F88 = 1;
                }
            }
            imageNo = 0x589B;
            draw = true;
            break;
        }
        case 1: {
            const std::int32_t graphicIndex = gBattlePetSkillGraphicIndex553F8C;
            const std::int32_t hover = render::gHoveredGraphicPendingIndex;
            if (graphicIndex == hover) {
                // 原498DF9复制12B，498DFD取点击位，498E07再复制末3B。
                gHoverTextD68AE8 = L"關閉技能視窗。";
                const auto clicks = reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0];
                static_cast<void>(0);
                if ((clicks & 1u) != 0) {
                    WindowInstance* const window = gWindowStates[WindowId_Skill].window;
                    if (window != nullptr) {
                        window->state |= 0x8000u;
                    }
                    gBattlePetCommandPhase553F88 = 0;
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
                cg::yobi::detail::FlushBattleEntries_43C060();
                const auto pet = std::bit_cast<std::int32_t>(gBattleRidePet5ABC38);
                gBattlePetCommandPhase553F88 = 1;
                gValue553F84 = -1;
                if (pet < 0) {
                    static_cast<void>(WindowSkill_OpenPetSkills(&WindowBattleCommand_SelectPetSkill));
                } else {
                    static_cast<void>(WindowSkill_OpenRidingSkills(&WindowBattleCommand_SelectRidingSkill));
                }
            }
            const std::int32_t graphicIndex = gBattlePetSkillGraphicIndex553F8C;
            const std::int32_t hover = render::gHoveredGraphicPendingIndex;
            bool clickedReturn = false;
            if (graphicIndex == hover) {
                const auto clicks = reinterpret_cast<const std::uint8_t*>(&platform::gLegacyMouseState.edgeButtons)[0];
                gHoverTextD68AE8 = L"返回技能視窗。";
                if ((clicks & 1u) != 0) {
                    cg::yobi::detail::FlushBattleEntries_43C060();
                    gBattlePetCommandPhase553F88 = 1;
                    gValue553F84 = -1;
                    clickedReturn = true;
                }
            }
            if (!clickedReturn && gValue553F84 != -1) {
                const std::int32_t namePet = Sub_43C4B0();
                const std::int32_t nameTechnique = gValue553F84;
                buffer = std::format(L"{:<16}", cg::text::FromGbkWide(gPetRuntimeRecords[namePet].techniquesD8[nameTechnique].name.data()));
                const std::int32_t nameY = gBattleCommandY553F7C;
                const std::int32_t nameX = gBattleCommandX553F78;
                static_cast<void>(render::DrawText2_456670(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(nameX - 114)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(nameY - 15)),
                    2, 0, 0, buffer, 0));
                const std::int32_t costPet = Sub_43C4B0();
                const std::int32_t costTechnique = gValue553F84;
                cg::text::FormatTo(buffer, L"{:4}", gPetRuntimeRecords[costPet].techniquesD8[costTechnique].fp);
                const std::int32_t costY = gBattleCommandY553F7C;
                const std::int32_t costX = gBattleCommandX553F78;
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
            const std::int32_t y = gBattleCommandY553F7C;
            const std::int32_t x = gBattleCommandX553F78;
            gBattlePetSkillGraphicIndex553F8C = cg::render::DrawGraphic(x, y, 212, imageNo, 2, nullptr);
        }
        if (gBattlePlayerActionState5ABC74 == 1) {
            const std::int32_t count = gBattlePetActionCount5ABBBC;
            gBattleCommandState5ABC3C = 5;
            gBattlePetActionCount5ABBBC = count + 1;
            gValue553F90 = Sub_43C4B0();
            WindowBattleCommand_Close();
        }
        break;
    }
    case 5:
        if (gWindowStates[WindowId_BattleCommand].userState.value == 3) {
            cg::yobi::YobiAction* const player = gBattleEntries59E980[gBattleCurrentPosition5ABCA4];
            if (*cg::yobi::detail::FindBattleRiderSlot(player) != nullptr && gBattlePetActionCount5ABBBC < 2) {
                gBattlePlayerActionState5ABC74 = 0;
                gBattleCommandState5ABC3C = 0;
            } else {
                world::gCurrentSceneState = 5;
            }
        }
        break;
    default:
        break;
    }
    const auto flags = reinterpret_cast<const std::uint8_t*>(&gBattlePlayerFlags5ABCAC)[0];
    if ((flags & 0x1Cu) != 0x1Cu) {
        WindowBattleCommand_DrawTimer();
    }
}

// 原函数：0x004982C0。
static void WindowBattleCommand_DrawTimer()
{
    using namespace world::detail;
    const std::uint32_t tick = platform::ReadTickCount32();
    std::int32_t remaining = std::bit_cast<std::int32_t>(gBattleCommandDeadline5ABBF8 - tick);
    if (gBattleCountdownActive5ABCB8 == 0) {
        return;
    }
    if (remaining <= 0) {
        gBattleCountdownActive5ABCB8 = 0;
        remaining = 0;
        cg::yobi::detail::FlushBattleEntries_43C060();
        WindowBattleCommand_Close();
        cg::yobi::YobiAction* const player = gBattleEntries59E980[gBattleCurrentPosition5ABCA4];
        bool finish = false;
        if (*cg::yobi::detail::FindBattleRiderSlot(player) != nullptr) {
            static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"W|FF"));
            finish = true;
        } else {
            std::uint8_t flags = reinterpret_cast<const std::uint8_t*>(&gBattlePlayerFlags5ABCAC)[0];
            const std::int32_t skillCount = gBattleSkillActionCount5ABBF4;
            if ((flags & 0x20u) != 0) {
                if (skillCount == 0 && (flags & 4u) == 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    finish = true;
                }
                if (gBattlePetActionCount5ABBBC == 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"W|FF"));
                    finish = true;
                }
            } else if (skillCount != 0 || (flags & 4u) == 0) {
                if (skillCount == 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    flags = reinterpret_cast<const std::uint8_t*>(&gBattlePlayerFlags5ABCAC)[0];
                    finish = true;
                }
                if (gBattleSkillActionCount5ABBF4 == 1 && (flags & 4u) == 0) {
                    static_cast<void>(network::SendBattlePacket_5109B0(platform::gConnectionSocket, L"N"));
                    finish = true;
                }
            }
        }
        if (finish) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            cg::yobi::YobiAction* const action = gBattleCommandAction5ABC00;
            world::gCurrentSceneState = 5;
            cg::yobi::detail::MarkDeath(action);
            gBattleCommandAction5ABC00 = nullptr;
        }
    }
    std::int32_t imageBase = 0x5690;
    if (remaining < 10000) {
        imageBase = 0x569A;
        const std::int32_t seconds = remaining / 1000;
        if (seconds != gBattleCountdownSecond5ABB80) {
            static_cast<void>(audio::PlaySe_52C450(202, 320));
            gBattleCountdownSecond5ABB80 = seconds;
        }
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"{:2}", remaining / 1000);
    for (std::int32_t index = 0; index < 2; ++index) {
        if (buffer[index] != ' ') {
            const std::int32_t digit =
                std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(buffer[index]));
            static_cast<void>(cg::render::DrawGraphic(
                304 + index * 32, 240, 209, imageBase + digit - '0', 0, nullptr));
        }
    }
    if (gBattleModeFlag5ABFCC == 1) {
        const auto turn = std::bit_cast<std::int32_t>(gBattleTurnSnapshot5ABC08 + 1u);
        cg::text::FormatTo(buffer, L"第 {} 回合", turn);
        static_cast<void>(render::DrawText3_456560(288, 180, 0, 4, buffer, 0));
    }
}

// 原函数：0x00496570。
void WindowBattleCommand_SnapshotPetVitals()
{
    if (world::detail::gBattleCurrentPosition5ABCA4 >= 20) {
        return;
    }
    for (std::size_t index = 0; index < gBattlePetVitalsCF4B78.size(); ++index) {
        auto& pet = world::detail::gPetRuntimeRecords[index];
        gBattlePetVitalsCF4B78[index].hp = pet.values0C[0].value();
        gBattlePetVitalsCF4B78[index].maxHp = pet.values0C[1].value();
        gBattlePetVitalsCF4B78[index].fp = pet.values0C[2].value();
        gBattlePetVitalsCF4B78[index].maxFp = pet.values0C[3].value();
    }
}

// 原函数：0x004965D0。
void WindowBattleCommand_SyncActorVitals()
{
    using namespace world::detail;
    const std::int32_t position = gBattleCurrentPosition5ABCA4;
    if (position >= 20 || gBattleSnapshotPetIndex5ABBFC < 0) {
        return;
    }
    const std::int32_t petPosition = GetBattlePetPosition_405B40(position);
    cg::yobi::YobiAction* const player = gBattleEntries59E980[gBattleCurrentPosition5ABCA4];
    if (player == nullptr) {
        return;
    }
    cg::yobi::YobiAction* const* const rider = cg::yobi::detail::FindBattleRiderSlot(player);
    if (rider == nullptr || *rider != nullptr) {
        return;
    }
    const std::int32_t hp = cg::yobi::detail::ReadYobiActionHpOnce(gBattleEntries59E980[petPosition]);
    gBattlePetVitalsCF4B78[gBattleSnapshotPetIndex5ABBFC].hp = hp;
    const std::int32_t maxHp = cg::yobi::detail::ReadYobiActionMaxHpOnce(gBattleEntries59E980[petPosition]);
    const std::int32_t maxHpIndex = gBattleSnapshotPetIndex5ABBFC;
    cg::yobi::YobiAction* const fpAction = gBattleEntries59E980[petPosition];
    gBattlePetVitalsCF4B78[maxHpIndex].maxHp = maxHp;
    const std::int32_t fp = cg::yobi::detail::ReadYobiActionFpOnce(fpAction);
    gBattlePetVitalsCF4B78[gBattleSnapshotPetIndex5ABBFC].fp = fp;
    const std::int32_t maxFp = cg::yobi::detail::ReadYobiActionMaxFpOnce(gBattleEntries59E980[petPosition]);
    gBattlePetVitalsCF4B78[gBattleSnapshotPetIndex5ABBFC].maxFp = maxFp;
}

} // namespace detail

} // namespace cg::ui
