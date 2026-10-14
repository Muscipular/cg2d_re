#pragma once

#include "cg/ui/WindowRuntime.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace cg::login {
struct YobiAction;
}

namespace cg::login::detail {
struct OrderedRecord1051B14;
}

namespace cg::input {
class InputContext;
}

namespace cg::world::detail {
struct PlayerItemRecord;
struct ContactRuntimeRecord;
struct MiniMailContactRecord;
}

namespace cg::ui {

// 原控件type的S32编号；创建、绘制、输入及释放共用同一类型。
enum WindowElementType : std::int32_t {
    WindowElementType_EventArea = 0, // 事件区域。
    WindowElementType_Graphic = 1, // 图片载荷。
    WindowElementType_Animation = 2, // 动画载荷。
    WindowElementType_Text = 3, // 文本载荷。
    WindowElementType_MouseArea = 4, // 鼠标区域／滚动载荷。
    WindowElementType_Input = 5, // 输入上下文。
    WindowElementType_InputCaret = 6, // 输入光标。
    WindowElementType_Number = 7, // 数字图片文本。
};
static_assert(sizeof(WindowElementType) == sizeof(std::int32_t));

using WindowCallback = std::int32_t (*)(std::int32_t value);
using WindowReleaseCallback = void (*)();
using WindowElementCallback = std::int32_t (*)(std::int32_t index, std::int32_t state);
using WindowValueCallback = void (*)(std::int32_t value);
using WindowDragCallback = void (*)(
    std::int32_t message, std::int32_t flags, std::int32_t sourceIndex);
using WindowListSelectionCallback = void (*)(std::int32_t index, const char* text);

// 原type1的24B图形载荷，六个数值字段，不含地址。
struct WindowGraphicData {
    std::int32_t imageNo; // 原成员偏移：+0x00。
    std::int32_t offsetX; // 原成员偏移：+0x04。
    std::int32_t offsetY; // 原成员偏移：+0x08。
    std::int32_t width; // 原成员偏移：+0x0C。
    std::int32_t height; // 原成员偏移：+0x10。
    std::uint32_t color; // 原成员偏移：+0x14。
};

// 原type5首槽是真实输入上下文引用；外层载荷拥有、上下文不拥有。
struct WindowInputData {
    input::InputContext* context; // 原成员偏移：+0x00。
};

// 原type7的文本T*与四个字节；字符串借用，不随外层free。
struct WindowNumberData {
    const std::wstring* text; // 原成员偏移：+0x00；借稳定owner，正文扩容不使控件悬空。
    std::uint8_t color; // 原成员偏移：+0x04。
    std::int8_t style; // 原成员偏移：+0x05。
    std::uint8_t flags; // 原成员偏移：+0x06。
    std::uint8_t byte07; // 原成员偏移：+0x07。
};

// 原第2类元素的8B载荷；471622写真实Yobi，471334只给它标死。
struct WindowAnimationData {
    login::YobiAction* action; // 原成员偏移：+0x00。
    std::uint32_t value04; // 原成员偏移：+0x04。
};

// 原第3类元素的8B载荷；文本由47167F分配，47134F释放。
// UTL02：动态UTF-16正文；原256B私有容量退休，协议/资源模型另行承载。
struct WindowTextData {
    std::array<std::byte, 4> bytes00; // 原成员偏移：+0x00。
    std::wstring text; // 原成员偏移：+0x04；UTL02动态UTF-16正文。
};

// 原type4的12B鼠标载荷；滚动回调与元素命中共用，不含地址。
struct WindowMouseData {
    std::uint32_t flags; // 原成员偏移：+0x00。
    std::int32_t deltaX; // 原成员偏移：+0x04。
    std::int32_t deltaY; // 原成员偏移：+0x08。
};

// 仅按已证字段增量映射；未消费数据不计最终布局冻结，见UI窗口SPEC。
struct WindowElement {
    WindowElementType type; // 原成员偏移：+0x00。 原S32控件类型。
    std::uint32_t state; // 原成员偏移：+0x04。
    std::int32_t x; // 原成员偏移：+0x08。
    std::int32_t y; // 原成员偏移：+0x0C。
    std::int32_t width; // 原成员偏移：+0x10。
    std::int32_t height; // 原成员偏移：+0x14。
    std::int32_t visible; // 原成员偏移：+0x18。
    void* data; // 原+1C，七类数据共用；后续按type解释，不是地址数值。
    WindowElementCallback mouseCallback; // 原+20。
};

// 原实例+40的16B可拖动区域，472D20逐项命中。
struct WindowDragRect {
    std::int32_t x; // 原成员偏移：+0x00。
    std::int32_t y; // 原成员偏移：+0x04。
    std::int32_t width; // 原成员偏移：+0x08。
    std::int32_t height; // 原成员偏移：+0x0C。
};

struct WindowInstance {
    std::uint32_t state; // 原+00；关闭请求置bit15。
    std::int32_t id; // 原成员偏移：+0x04。
    std::int32_t layer; // 原成员偏移：+0x08。
    std::int32_t x; // 原成员偏移：+0x0C。
    std::int32_t y; // 原成员偏移：+0x10。
    std::int32_t width; // 原成员偏移：+0x14。
    std::int32_t height; // 原成员偏移：+0x18。
    std::int32_t dragMouseX1C; // 原成员偏移：+0x1C。
    std::int32_t dragMouseY20; // 原成员偏移：+0x20。
    std::uint32_t flags; // 原成员偏移：+0x24。
    std::int32_t elementCount; // 原成员偏移：+0x28。
    WindowElement* elements; // 原+2C。
    WindowCallback update; // 原+30。
    WindowCallback draw; // 原+34。
    WindowReleaseCallback release; // 原+38。
    std::int32_t secondaryCount; // 原+3C。
    std::array<WindowDragRect, 10> dragRects40; // 原[+40,+E0)。
};

namespace detail {

// 窗口45/46选择回调：非零返回允许关闭，文本只借用；释放通知index=-1。
using WindowTextChoiceCallback = std::int32_t (*)(std::int32_t index, const char* text); // text借用原GBK名称，取消／关闭时可为nullptr。
// CC4230原回调实际接收技能和等级两个S32，47C7B2/47C7B3压栈，返回不用。
using SkillWindowSelectionCallback = void (*)(std::int32_t skillIndex, std::int32_t technique);
// 原函数：0x004EF710。
std::int32_t WindowSkill_ClassifyAttributeSkill(std::int32_t skillId); // 221..224分类0..3，其余-1。
// 原函数：0x004F0200。
void WindowSkill_ReceiveTextTargets(const char* text); // ysVc完整名单生产；唯一回复调用方弃用混合返回。
// 原函数：0x004871B0。
login::YobiAction* WindowSkill_OpenTextTargets(); // 技能目标名称/颜色借用及45创建，不回滚失败计数。
// 原函数：0x00487020。
std::int32_t WindowSkill_SelectTextTarget(std::int32_t index, const char* text); // 请求属性/发送技能及释放通知。
// 原函数：0x0047A140。
void WindowSkill_SyncStates(); // 窗口10模式2的技能状态差异请求，原返回无人消费。
// 原函数：0x00479B80。
void WindowSkill_ShowSkillList(WindowInstance* window, std::int32_t skillIndex); // 技能列表定位，不以used过滤索引查找。
// 原函数：0x0047B6C0。
void WindowSkill_DrawSkillRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t pairIndex,
    std::int32_t enabled, std::int32_t textLayer, std::int32_t graphicLayer,
    std::uint32_t color); // 固定配对对应技能，需求槽数按S8。
// 原函数：0x0047B820。
std::int32_t WindowSkill_HandleSkillRow(std::int32_t index, std::int32_t state); // 列表排序请求、打开技术与右键拖动。
// 原函数：0x00479700。
void WindowSkill_HandleDrag(
    std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // kind4拖动，仅message0推进。
// 原函数：0x004797A0。
std::int32_t WindowSkill_StartDrag(std::int32_t sourceIndex); // 安装479700并透传返回。
// 原函数：0x00479C80。
void WindowSkill_ShowTechniques(WindowInstance* window, std::int32_t pairIndex); // 切技术模式，负参数保留原技能。
// 原函数：0x00479DA0。
void WindowSkill_ShowRecipes(WindowInstance* window, std::int32_t level); // 切配方，滚动先用旧上限。
// 原函数：0x00479EA0。
void WindowSkill_ShowBattleSkills(WindowInstance* window, std::int32_t skillIndex); // 战斗列表及原过滤表尾字节计数门槛。
// 原函数：0x0047A000。
void WindowSkill_ShowBattleTechniques(WindowInstance* window, std::int32_t index, std::uint8_t directSkill); // 战斗技术初始化。
// 原函数：0x0047C570。
std::int32_t WindowSkill_HandleBattleSkillRow(std::int32_t index, std::int32_t state); // 战斗列表行，技能标志决定可用。
// 原函数：0x0047C700。
std::int32_t WindowSkill_SelectBattleTechnique(std::int32_t index, std::int32_t state); // 战斗技术选择，独立FP读取与回调后重取窗口。
// 原函数：0x0047A490。
login::YobiAction* WindowSkill_Open(); // 清选择回调并创建普通技能窗。
// 原函数：0x0047A4C0。
login::YobiAction* WindowSkill_OpenBattleSkills(SkillWindowSelectionCallback callback); // 设置回调并创建战斗列表窗。
// 原函数：0x0047A500。
login::YobiAction* WindowSkill_OpenBattleTechniques(std::int32_t skillIndex, SkillWindowSelectionCallback callback); // 战斗技术窗。
// 原函数：0x0047A1A0。
void WindowSkill_ShowPetSkills(WindowInstance* window); // 宠物模式初始化，保留弃用结果的FP读取。
// 原函数：0x0047A320。
void WindowSkill_ShowRidingSkills(WindowInstance* window); // 骑乘模式初始化，名称借自身战斗对象。
// 原函数：0x0047CB00。
void WindowSkill_DrawPetSkillRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t pairIndex,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 宠物行绘制，两次宠物查询。
// 原函数：0x0047CE70。
void WindowSkill_DrawRidingSkillRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t pairIndex,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 骑乘行绘制，固定技术记录。
// 原函数：0x0047CC80。
std::int32_t WindowSkill_SelectPetSkill(std::int32_t index, std::int32_t state); // 宠物选择，不加滚动偏移。
// 原函数：0x0047CFE0。
std::int32_t WindowSkill_SelectRidingSkill(std::int32_t index, std::int32_t state); // 骑乘选择，FP不足不补失败音效。
// 原函数：0x0047A540。
login::YobiAction* WindowSkill_OpenPetSkills(WindowValueCallback callback); // 设置CC4D14并创建宠物技能窗。
// 原函数：0x0047A580。
login::YobiAction* WindowSkill_OpenRidingSkills(WindowValueCallback callback); // 设置CC4D14并创建骑乘技能窗。
// 原函数：0x004966B0。
void WindowBattleCommand_Open(std::int32_t mode, std::int16_t x, std::int16_t y); // 窗口26五模式创建/配置及入场偏移。
// 原函数：0x00496B90。
std::int32_t WindowBattleCommand_Update(std::int32_t value); // 窗口26初始化、入退场与按钮显隐。
// 原函数：0x00496D50。
std::int32_t WindowBattleCommand_Draw(std::int32_t unused); // 先绘制再更新技术名，按命中索引追加目标说明。
// 原函数：0x00496AE0。
void WindowBattleCommand_Close(); // 窗口26存在才收尾、关闭10/27/28及清三展开引用，原返回无人消费。
// 原函数：0x00496F10。
void WindowBattleCommand_ResetSelection(); // 先清九个选择槽，再关闭三子窗及清同源展开引用。
// 原函数：0x00497000。
void WindowBattleCommand_OpenSkills(); // 战斗技能快捷入口，保留actionFlags低S8门槛；原返回不用。
// 原函数：0x00496FB0。
std::int32_t WindowBattleCommand_CanSelectSkill(); // 技能可用性：门禁失败/已选返回0，否则-1。
// 原函数：0x004970B0。
std::int32_t WindowBattleCommand_CanSelectItem(); // 同一门禁，读取物品选择槽3。
// 原函数：0x00497100。
void WindowBattleCommand_OpenItems(); // 物品快捷入口，清目标待选状态并打开27。
// 原函数：0x004971C0。
std::int32_t WindowBattleCommand_CanSelectPet(); // 同一门禁，读取宠物选择槽4。
// 原函数：0x00497210。
void WindowBattleCommand_OpenPets(); // 宠物快捷入口，打开28，不清物品待选状态。
// 原函数：0x00473840。
std::int32_t WindowBottomBar_HandleButtons(std::int32_t index, std::int32_t state); // 窗口6完整九分支按钮。
// 原函数：0x0046FBD0。
std::int32_t WindowShortcutBar_ButtonCollapse(std::int32_t unused, std::int32_t state); // 快捷栏4点击收起到5，打开后发布CA0440。
// 原函数：0x0046FC10。
std::int32_t WindowShortcutBar_ButtonExpand(std::int32_t unused, std::int32_t state); // 快捷栏5点击展开到4。
// 原函数：0x0046FC60。
std::int32_t WindowShortcutBar_HandleButtons(std::int32_t index, std::int32_t state); // 完整九按钮，保留6/7子入口消费覆盖。
// 原函数：0x004708A0。
std::int32_t WindowStatusBar_ButtonEmote(std::int32_t index, std::int32_t state); // 窗口34存在时更新图形，点击开关。
// 原函数：0x00470910。
std::int32_t WindowStatusBar_ButtonMiniMap(std::int32_t index, std::int32_t state); // 窗口33开关，创建使用专有尺寸入口。
// 原函数：0x00495F00。
login::YobiAction* WindowMiniMap_Open(
    std::uint32_t flags, std::uint32_t mode, login::YobiAction** completionSlot); // 小地图184x159展开，完成槽原生T**。
// 原函数：0x00495E10。
std::int32_t WindowMiniMap_DrawMap(std::int32_t index, std::int32_t unused); // 小地图按窗口/元素坐标入既有绘制队列。
// 原函数：0x00495E60。
std::int32_t WindowMiniMap_RefreshPlayerX(std::int32_t index, std::int32_t unused); // 固定文本实例，门禁后刷新玩家X。
// 原函数：0x00495EB0。
std::int32_t WindowMiniMap_RefreshPlayerY(std::int32_t index, std::int32_t unused); // 固定文本实例，门禁后刷新玩家Y。
// 原函数：0x00495F70。
std::int32_t WindowEmote_Select(std::int32_t index); // 清18B后选中指定字节，Y/X快照交4D3020。
// 原函数：0x00495FC0。
std::int32_t WindowEmote_ResetSelection(); // 按四W32/一W16清唯一18B选择区，返回0。
// 原函数：0x00495FE0。
std::int32_t WindowEmote_ButtonEmote(std::int32_t index, std::int32_t state); // 情绪窗18按钮，请求被拒仍更新选择并发声。
// 原函数：0x004723D0。
std::int32_t WindowStatusBar_HandleHintButton(std::int32_t index, std::int32_t state); // SHP01：三项提示完整点击/取样序，原门槛和关闭开窗保持。
// 原函数：0x00470AC0。
void WindowStatusBar_Refresh(); // TUC01：窗口1HP/FP、时钟/四位倒计时/提示原生产序；机器ABI与运行待。
// 原函数：0x004718F0。
void WindowCompactStatusBar_Refresh(); // TUC01：窗口2直接FP复制/四位倒计时，首窗口取样晚于描述符发布。
// 原函数：0x00471CD0。
std::int32_t WindowStatusBar_ButtonCompact(std::int32_t unused, std::int32_t state); // 状态栏1点击切2，调用后发布保存模式。
// 原函数：0x00471D10。
std::int32_t WindowCompactStatusBar_ButtonCollapse(std::int32_t unused, std::int32_t state); // 状态栏2点击切3。
// 原函数：0x00471D60。
std::int32_t WindowCollapsedStatusBar_ButtonExpand(std::int32_t unused, std::int32_t state); // 状态栏3按场景返回2或1。
// 原函数：0x00471DD0。
std::int32_t WindowStatusBar_HandleHover(std::int32_t unused, std::int32_t state); // 仅hover命中消费。
// 原函数：0x00471DF0。
std::int32_t WindowStatusBar_HandleOptionButtons(std::int32_t index, std::int32_t state); // 六选项命中区操作前15项图形，发送后SE51。
// 原函数：0x00473080。
std::int32_t WindowStatusBar_ReadInjuredPetIndex(); // SHP01：玩家伤势优先返0，否则宠物原编号+10/未命中-1。
// 原函数：0x004731B0。
std::int32_t WindowStatusBar_FindSortedPetIndex(std::int32_t index); // SHP01：五个S16配对首匹配位置，无则-1，不查used。
// 原函数：0x004972B0。
std::int32_t WindowBattleCommand_ButtonAttack(std::int32_t index, std::int32_t state); // 攻击选择，原目标标志475H。
// 原函数：0x004973C0。
std::int32_t WindowBattleCommand_ButtonDefend(std::int32_t index, std::int32_t state); // 防御G命令及玩家动作完成。
// 原函数：0x004974C0。
void WindowBattleCommand_SelectSkill(std::int32_t skillIndex, std::int32_t technique); // CC4230真实双参数命令回调。
// 原函数：0x004975A0。
std::int32_t WindowBattleCommand_ButtonSkill(std::int32_t index, std::int32_t state); // 技能按钮及选目标返回技术窗。
// 原函数：0x00497770。
std::int32_t WindowBattleCommand_ButtonItem(std::int32_t index, std::int32_t state); // 物品选择/返回27，保留CC4D40独立门禁。
// 原函数：0x00497900。
std::int32_t WindowBattleCommand_ButtonPet(std::int32_t index, std::int32_t state); // 宠物选择及28窗口关闭。
// 原函数：0x00497A60。
std::int32_t WindowBattleCommand_ButtonFormation(std::int32_t index, std::int32_t state); // P命令，使用九槽中的槽6。
// 原函数：0x00497B80。
std::int32_t WindowBattleCommand_ButtonEscape(std::int32_t index, std::int32_t state); // E命令，使用槽7，无已用技能门禁。
// 原函数：0x00497C80。
std::int32_t WindowBattleCommand_ButtonRebirth(std::int32_t index, std::int32_t state); // TUC01：原载荷判空/hover持有，刷新后重取R命令状态。
// 原函数：0x00497E30。
void WindowBattleCommand_SelectPetSkill(std::int32_t technique); // 宠物命令，保留重复查询与负参数略过。
// 原函数：0x00497FF0。
void WindowBattleCommand_SelectRidingSkill(std::int32_t technique); // 骑乘命令，清选择模式并保留骑乘快照写序。
// 原函数：0x00498160。
std::int32_t WindowBattleCommand_ButtonPetSkills(std::int32_t index, std::int32_t state); // 宠物/骑乘技能窗开关。
// 原函数：0x004982C0。
void WindowBattleCommand_DrawTimer(); // 完整倒计时/超时发送，本帧超时后仍绘制。
// 原函数：0x0043C3E0。
void WindowBattleCommand_DrawTargetHint(); // 553F80门禁及首个可选对象的目标提示，不要求鼠标命中。
// 原函数：0x0043C490。
void WindowBattleCommand_ResetTargets(std::int32_t* values, std::int32_t count); // 正数count逐槽写-2，原caller弃返回。
// 原函数：0x00498520。
void WindowBattleCommand_Dispatch(); // 完整六态战斗命令分派，所有出口共用倒计时尾。
// 原函数：0x0043C950。
void WindowBattleStateNotice_Open(); // 玩家入场位且动画/窗口32均空才创建，失败也播放原音效。
// 原函数：0x00496570。
void WindowBattleCommand_SnapshotPetVitals(); // 五宠物四项CryptInt快照，唯一CF4B78数组生产。
// 原函数：0x004965D0。
void WindowBattleCommand_SyncActorVitals(); // 战斗对象四值更新，保留独立5ABBFC及原空值门禁。
// 原函数：0x0049B630。
std::int32_t WindowBattlePets_Update(std::int32_t unused); // 窗口28归还按钮显隐。
// 原函数：0x0049BA10。
std::int32_t WindowBattlePets_ButtonReturn(std::int32_t index, std::int32_t state); // 窗口28宠物归还M|FF。
// 原函数：0x0049BB00。
std::int32_t WindowBattlePets_SelectRow(std::int32_t index, std::int32_t state); // 窗口28三条宠物选择行。
// 原函数：0x0049B670。
void WindowBattlePets_Refresh(
    std::int16_t x, std::int16_t y, std::int32_t petIndex,
    std::uint8_t textLayer, std::int32_t graphicLayer,
    const render::GraphicDescriptor16* descriptor); // 快照数值与宠物属性行，唯一caller弃返回。
// 原函数：0x0047B110。
std::int32_t WindowSkill_ButtonBack(std::int32_t index, std::int32_t state); // 固定入口窗口的返回按钮。
// 原函数：0x0047B210。
std::int32_t WindowSkill_ButtonUse(std::int32_t index, std::int32_t state); // TUC01：先借元素再读U8动作标志；Y/X发送后更新同源占用。
// 原函数：0x0047C950。
std::int32_t WindowSkill_ButtonPetSkillBack(std::int32_t index, std::int32_t state); // 无可用宠物技能时略过，通知-1。
// 原函数：0x0047A5C0。
std::int32_t WindowSkill_Back(); // 战斗返回：模式4退列表，0/3/5/6关闭。
// 原函数：0x0047B3D0。
std::int32_t WindowSkill_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 前一行，bit400。
// 原函数：0x0047B470。
std::int32_t WindowSkill_ButtonNextRow(std::int32_t index, std::int32_t state); // 后一行，bit400。
// 原函数：0x0047B520。
std::int32_t WindowSkill_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 前十行，音效后重读页码。
// 原函数：0x0047B5D0。
std::int32_t WindowSkill_ButtonNextPage(std::int32_t index, std::int32_t state); // 后十行，不额外钳制负上限。
// 原函数：0x0047B680。
std::int32_t WindowSkill_Scroll(std::int32_t unusedIndex, std::int32_t state); // 同源滚轮，恒返0。
// 原函数：0x0047B9B0。
void WindowSkill_HandleTechniqueRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t technique,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 技术行，>=100按战斗FP着色。
// 原函数：0x0047C240。
void WindowSkill_HandleRecipeRow(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t recipe,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 配方行与独立图号表。
// 原函数：0x0047BC00。
std::int32_t WindowSkill_SelectSkillType(std::int32_t index, std::int32_t state); // 五类技能选择与完整失败音效。
// 原函数：0x0047C3A0。
std::int32_t WindowSkill_SelectRecipe(std::int32_t index, std::int32_t state); // 转生一致才处理配方，恒返0。
// 原函数：0x00479650。
void WindowRuntime_IncrementOperationCount(); // 原窗口操作S32计数加1。
// 原函数：0x00479660。
void WindowRuntime_DecrementOperationCount(); // 原操作计数减1及严格零时的退出发送/技能状态刷新。
// 原函数：0x0047D170。
std::int32_t WindowCraft_SetPreviewPositions(
    WindowInstance* window, std::int32_t anchor, std::int32_t first, std::int32_t second,
    std::int32_t dx, std::int32_t dy); // 双预览定位/显隐，返回原36B元素数值偏移。
// 原函数：0x0047D220。
WindowAnimationData* WindowCraft_BorrowSecondPreview(
    WindowInstance* window, std::int32_t first, std::int32_t second,
    std::int32_t flag, std::int32_t actionId, std::int32_t direction); // 返回第二个借用载荷。
// 原函数：0x00484D60。
std::int32_t WindowCraft_HandleMaterialTooltip(std::int32_t index, std::int32_t state); // 六材料槽悬停、翻页与图形绘制。
// 原函数：0x0047D4B0。
std::int32_t WindowCraft_HasRecipeCrystalLevel(); // 晶石等级读取后比较当前配方等级。
// 原函数：0x0047D4F0。
std::int32_t WindowCraft_HasSelectedCrystalLevel(); // 单选物品不加8，晶石读取后重新取得选择索引。
// 原函数：0x0047D530。
std::int32_t WindowCraft_HasMaterialCrystalLevel(); // 六材料首个匹配类别，晶石读取后重取槽号再比较等级。
// 原函数：0x0047D5B0。
std::int32_t WindowCraft_HasFirstMaterialCrystalLevel(); // 首材料slot+8等级在晶石读取前快照。
// 原函数：0x0047D5F0。
login::YobiAction* WindowCraft_OpenRecipe(
    std::int32_t skill, std::int32_t recipe, std::int32_t request); // 制造开窗，保存同源配方id。
// 原函数：0x0047D780。
login::YobiAction* WindowCraft_OpenItemWork(std::int32_t skill, std::int32_t request); // 单选加工开窗。
// 原函数：0x0047D8F0。
login::YobiAction* WindowCraft_OpenDecoration(
    std::int32_t skill, std::int32_t request, std::int32_t kind); // 装饰开窗，kind严格1为宠物装备。
// 原函数：0x0047DA70。
login::YobiAction* WindowCraft_OpenRefining(std::int32_t skill, std::int32_t request); // 精炼开窗，条件设置世界状态9。
// 原函数：0x0047DB50。
login::YobiAction* WindowCraft_OpenMaterials(std::int32_t skill, std::int32_t request); // 材料开窗，无条件设置世界状态9。
// 原函数：0x0047E1B0。
WindowAnimationData* WindowCraft_WaitCraft(WindowInstance* window); // 制造等待模式1，按配方等级生成同源截止。
// 原函数：0x0047EF00。
WindowAnimationData* WindowCraft_WaitItemWork(WindowInstance* window); // 单选等待模式4，刻印排除宠物辅助。
// 原函数：0x004801B0。
WindowAnimationData* WindowCraft_WaitPacking(WindowInstance* window); // 打包等待模式13，仍读取绝对单选槽等级。
// 原函数：0x00480600。
void WindowCraft_WaitDecoration(WindowInstance* window); // 装饰等待固定8000ms，原被AL覆盖的返回值由caller弃用。
// 原函数：0x00480C20。
WindowAnimationData* WindowCraft_WaitRefining(WindowInstance* window); // 精炼等待模式10，一次RAND且不读物品等级。
// 原函数：0x00483EB0。
std::int32_t WindowCraft_HandleMaterialSlot(std::int32_t index, std::int32_t state); // 六材料完整投放/换位/逐帧校验与绘制。
// 原函数：0x0047DF50。
WindowAnimationData* WindowCraft_InitializeRecipe(WindowInstance* window); // 制造模式0完整初始化，逆序发布五材料需求。
// 原函数：0x0047FAE0。
WindowAnimationData* WindowCraft_InitializeDecoration(WindowInstance* window); // 装饰模式6，固定魔力100。
// 原函数：0x0047FD00。
WindowAnimationData* WindowCraft_InitializePetDecoration(WindowInstance* window); // 宠物装备装饰模式15，独立标题。
// 原函数：0x0047FF20。
WindowAnimationData* WindowCraft_InitializePacking(WindowInstance* window); // 模式12材料类别文本与完整六材料交互。
// 原函数：0x00480A00。
WindowAnimationData* WindowCraft_InitializeRefining(WindowInstance* window); // 精炼模式9，采样同源计时并按发送模式配置动作。
// 原函数：0x004F4430。
std::int32_t WindowCraft_ClassifySpecialMaterial(std::int32_t itemType, std::int32_t skillId); // 技能分类对应的特殊材料类别。
// 原函数：0x00483E30。
void WindowCraft_ReleaseMaterialSlot(
    std::int32_t destination, std::int32_t source, std::int32_t material); // 先释放再发布材料槽/占用。
// 原函数：0x004880D0。
std::int32_t WindowCraft_ReadRequiredSkillLevel(std::int32_t skillId, std::int32_t extraLevel); // 完整快照后查技能等级，附加值只读低U8。
// 原函数：0x00488160。
std::int32_t WindowCraft_ReadMaxMaterialLevel(); // 前三材料占用等级与0的最大值。
// 原函数：0x00488180。
std::int32_t WindowCraft_ReadFirstMaterialId(); // 六槽中首个材料类别0/1/2对应的itemId。
// 原函数：0x004881D0。
void WindowCraft_ValidatePackingMaterials(std::int32_t destination, std::int32_t source); // 模式12完整材料校验、清理与发布。
// 原函数：0x0047DE00。
void WindowCraft_HandleMaterialDrag(
    std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // 材料/单选拖动与释放。
// 原函数：0x0047DF30。
std::int32_t WindowCraft_StartMaterialDrag(std::int32_t sourceIndex); // 安装类型10的47DE00拖动回调。
// 原函数：0x00482F70。
std::int32_t WindowCraft_ClassifyWork(std::int32_t itemType); // 加工分类；类别7优先归2。
// 原函数：0x00482270。
std::int32_t WindowCraft_ButtonExecute(std::int32_t index, std::int32_t state); // 执行资格、五等待及刻印直接发送。
// 原函数：0x00482FA0。
std::int32_t WindowCraft_ButtonContinue(std::int32_t index, std::int32_t state); // 续作，初始化后独立重读模式5。
// 原函数：0x004830B0。
std::int32_t WindowCraft_ButtonClose(std::int32_t index, std::int32_t state); // 底部关闭，点击仍返回0。
// 原函数：0x00483150。
std::int32_t WindowCraft_HandleInventory(std::int32_t index, std::int32_t state); // 20格背包投放/快速选择/拖动及绘制。
// 原函数：0x00484FB0。
std::int32_t WindowCraft_DrawResultImage(std::int32_t index, std::int32_t unusedState); // 结果图，负图号不绘制。
// 原函数：0x00485030。
std::int32_t WindowCraft_HandleEngravingInput(std::int32_t index, std::int32_t state); // 刻印输入焦点，借用唯一D2DE70。
// 原函数：0x00485160。
std::int32_t WindowCraft_ButtonCloseIcon(std::int32_t index, std::int32_t state); // 右上关闭，点击返回1。
// 原函数：0x00485200。
std::int32_t WindowCraft_DrawInvalidEngravingNotice(std::int32_t index, std::int32_t unusedState); // 非法刻印字符覆盖提示。
// 原函数：0x00485350。
std::int32_t WindowCraft_DrawEmptyEngravingNotice(std::int32_t index, std::int32_t unusedState); // 空白刻印覆盖提示。
// 原函数：0x004847C0。
std::int32_t WindowCraft_HandleSingleMaterial(std::int32_t index, std::int32_t state); // 单选投放/校验/拖动与绘制。
// 原函数：0x0047EC00。
WindowAnimationData* WindowCraft_InitializeItemWork(WindowInstance* window); // 单选模式3完整初始化，返回借用载荷。
// 原函数：0x0047E5E0。
WindowAnimationData* WindowCraft_ShowRecipeResult(WindowInstance* window); // 制造结果完整窗口11展示。
// 原函数：0x0047EBE0。
WindowAnimationData* WindowCraft_RefreshRecipeResult(); // 仅已有窗口11才更新，不自动创建。
// 原函数：0x004F39F0。
WindowAnimationData* WindowCraft_ReceiveRecipeResult(std::int32_t result, const char* text); // JgO制造分支，原字节摘要与展示。
// 原函数：0x00484E90。
std::int32_t WindowCraft_HandleItemTooltip(std::int32_t index, std::int32_t state); // 单物品悬停/翻页，物品编号不加8。
// 原函数：0x0047F3D0。
WindowAnimationData* WindowCraft_ShowItemWorkResult(WindowInstance* window); // 物品加工结果展示，保留入口选择快照及交叉门禁。
// 原函数：0x0047FAC0。
WindowAnimationData* WindowCraft_RefreshItemWorkResult(); // 仅已有窗口11才展示物品结果。
// 原函数：0x004F1800。
WindowAnimationData* WindowCraft_ReceiveItemWorkResult(std::int32_t result, const char* text); // JgO第二分支七字段及独立摘要。
// 原函数：0x004796E0。
void WindowRuntime_DecrementGatherOperationCount(); // 只递减既有D67AFC低U8，不清其他操作计数。
// 原函数：0x004796D0。
void WindowRuntime_IncrementGatherOperationCount(); // 仅递增既有D67AFC低U8。
// 原函数：0x004854A0。
login::YobiAction* WindowGather_Open(std::int32_t skill, std::int32_t level); // 低U8请求状态及窗口12创建。
// 原函数：0x00485700。
WindowAnimationData* WindowGather_Cancel(WindowInstance* window); // 取消窗口12操作并归空闲，保留末动画载荷返回。
// 原函数：0x00485890。
WindowAnimationData* WindowGather_InitializeAction(WindowInstance* window); // 窗口12计时/操作初始化。
// 原函数：0x00486CF0。
std::int32_t WindowGather_ButtonContinue(std::int32_t index, std::int32_t state); // FP/背包检查与重启。
// 原函数：0x00486E50。
std::int32_t WindowGather_ButtonClose(std::int32_t index, std::int32_t state); // 关闭或取消，hover后重取窗口。
// 原函数：0x00486F20。
std::int32_t WindowGather_DrawResultImage(std::int32_t index, std::int32_t unusedState); // 结果图及固定框，恒返0。
// 原函数：0x0047D2E0。
WindowAnimationData* WindowCraft_AnimatePreviewPositions(
    WindowInstance* unusedWindow, std::int32_t anchor,
    std::int32_t first, std::int32_t second, std::uint32_t deadline); // 双预览六点插值，实际借当前窗口。
// 原函数：0x00485AF0。
WindowAnimationData* WindowGather_ShowResult(WindowInstance* window); // 窗口12完整结果展示及双计时槽发布。
// 原函数：0x00486020。
WindowAnimationData* WindowGather_RefreshResult(); // 仅更新已有窗口12，不自动创建。
// 原函数：0x004ED6B0。
WindowAnimationData* WindowGather_ReceiveResult(std::int32_t result, const char* text); // JgO第四分支，失败不解析正文。
// 原函数：0x0047D280。
std::int32_t WindowCraft_SetPreviewImages(
    WindowInstance* window, std::int32_t first, std::int32_t second,
    std::int32_t imageNo); // 双动画图号，非正才隐藏；返回原数值元素偏移。
// 原函数：0x00487F30。
std::int32_t WindowSkillResult_ReadPreviewImage(std::int32_t imageNo, std::int32_t offset); // 技能预览分组图号转换。
// 原函数：0x004875E0。
login::YobiAction* WindowSkillResult_Open(); // 原操作计数、关联关闭及窗口13创建。
// 原函数：0x004876B0。
WindowAnimationData* WindowSkillResult_RefreshResult(WindowInstance* window); // 窗口13按分类完整填充结果。
// 原函数：0x00487D60。
std::int32_t WindowSkillResult_ButtonContinue(std::int32_t index, std::int32_t state); // 先比较FP，再继续技能请求。
// 原函数：0x00487E80。
std::int32_t WindowSkillResult_ButtonClose(std::int32_t index, std::int32_t state); // 关闭入口窗口，音效后写图号。
// 原函数：0x004EEA40。
login::YobiAction* WindowSkillResult_ReceiveResult(std::int32_t result, const char* text); // JgO第三分支，七字段及窗口13。
// 原函数：0x004EF750。
login::YobiAction* WindowSkill_ReceiveAttributeTargets(const char* text); // ND自身/远端属性生产，分类1跳过人物行。
// 原函数：0x004874B0。
login::YobiAction* WindowSkill_OpenAttributeTargets(); // 同源六属性视图与46创建，不回滚计数。
// 原函数：0x004872E0。
std::int32_t WindowSkill_SelectAttributeTarget(std::int32_t index, const char* unusedText); // 按属性目标编号发送技能。
// 原函数：0x00479820。
std::int32_t WindowSkill_FindSpecialSkill(); // 技能索引对中查1009，按S16读取槽号。
// 原函数：0x00479860。
std::int32_t WindowSkill_ReadCrystalLevel(); // 玩家物品槽7的晶石等级，未用/未知返回0。
// 原函数：0x00479920。
std::int32_t WindowSkill_ReadCrystalElement(); // 同一槽7的属性0..3；未用0，未知ID为-1。
// 原函数：0x004799D0。
std::int32_t WindowSkill_ReadBattlePetImage(); // 重新选取出战宠物并读取原图号，未命中0。
// 原函数：0x00479A00。
std::int32_t WindowSkill_FindPetTechnique(std::int32_t petIndex); // 宠物技术索引对中查40330..40332。
// 原函数：0x00479A70。
std::int32_t WindowSkill_CheckPetLevel(std::int32_t petIndex, std::int32_t crystalLevel); // 宠物等级闭区间。
// 原函数：0x00479AB0。
std::int32_t WindowSkill_CheckPetFp(); // 当前宠物FP的单次CryptInt比较。
// 原函数：0x00479B00。
std::int32_t WindowSkill_CanUseSpecialSkill(); // 技能/晶石/宠物技术/属性/等级条件链。
// 原函数：0x00486FE0。
std::int32_t WindowSkill_MatchesCrystalLevel(); // 当前技能技术value90与重读晶石等级等值。
// 原函数：0x0048AEA0。
std::int32_t WindowItemUse_SelectTextTarget(std::int32_t index, const char* unusedText); // 选择发送/释放计数；-2仅允许关闭。
// 原函数：0x0048AF00。
login::YobiAction* WindowItemUse_OpenTextTargets(); // 名称/颜色借用及物品占用计数，创建失败按原条件回滚。
// 原函数：0x004E4860。
login::YobiAction* WindowItemUse_ReceiveTextTargets(const char* text); // LY完整目标名单生产，保留原Yobi返回。
// 原函数：0x0048B010。
std::int32_t WindowItemUse_SelectAttributeTarget(std::int32_t index, const char* unusedText); // 用物品/请求回退/释放通知。
// 原函数：0x0048B0A0。
login::YobiAction* WindowItemUse_OpenAttributeTargets(); // 属性展示源及46创建成功后的引用计数。
// 原函数：0x004E4020。
login::YobiAction* WindowItemUse_ReceiveAttributeTargets(const char* text); // nFV完整属性值生产，不清旧表或替换宠物状态owner。
// 原CB10D8的52B值记录；旧78只读借用，实际六行存储仍由WindowRuntime私有持有。
struct ItemUseStatusRecord {
    std::array<char, 20> name;
    std::int32_t level; // 原成员偏移：+0x14。
    std::int32_t injury; // 原成员偏移：+0x18。
    std::int32_t hp; // 原成员偏移：+0x1C。
    std::int32_t maxHp;
    std::int32_t fp;
    std::int32_t maxFp; // 原成员偏移：+0x28。
    std::int32_t kind; // 原成员偏移：+0x2C。
    std::int32_t target; // 原成员偏移：+0x30。
};
static_assert(offsetof(ItemUseStatusRecord, level) == 0x14);
static_assert(offsetof(ItemUseStatusRecord, injury) == 0x18);
static_assert(offsetof(ItemUseStatusRecord, hp) == 0x1C);
static_assert(offsetof(ItemUseStatusRecord, maxFp) == 0x28);
static_assert(offsetof(ItemUseStatusRecord, kind) == 0x2C);
static_assert(offsetof(ItemUseStatusRecord, target) == 0x30);
static_assert(sizeof(ItemUseStatusRecord) == 52);
const ItemUseStatusRecord& WindowItemUse_ReadStatus(std::int32_t index) noexcept;
std::int32_t WindowItemUse_ReadStatusCount() noexcept; // 原CC4D44，不固定为六行或复制数量。
// 原函数：0x0049F000。names借用GBK名单；49F0A0刷新时直接转为宽正文。
login::YobiAction* WindowTextTarget_Open(
    const char* const* names, const std::uint8_t* colors,
    std::int32_t count, WindowTextChoiceCallback callback);
// 原函数：0x0049F040。
void WindowTextTarget_Release();
// 原函数：0x0049F060。
std::int32_t WindowTextTarget_Update(std::int32_t value);
// 原函数：0x0049F0A0。
void WindowTextTarget_Refresh(std::int32_t offset); // 刷新五行，未用行复制593850但保留颜色。
// 原函数：0x0049F1A0。
std::int32_t WindowTextTarget_Draw(std::int32_t value);
// 原函数：0x0049F260。
std::int32_t WindowTextTarget_ButtonPreviousRow(std::int32_t index, std::int32_t state);
// 原函数：0x0049F310。
std::int32_t WindowTextTarget_ButtonNextRow(std::int32_t index, std::int32_t state);
// 原函数：0x0049F3D0。
std::int32_t WindowTextTarget_ButtonPreviousPage(std::int32_t index, std::int32_t state);
// 原函数：0x0049F490。
std::int32_t WindowTextTarget_ButtonNextPage(std::int32_t index, std::int32_t state);
// 原函数：0x0049F570。
std::int32_t WindowTextTarget_Scroll(std::int32_t index, std::int32_t state);
// 原函数：0x0049F5C0。
std::int32_t WindowTextTarget_ButtonCancel(std::int32_t index, std::int32_t state);
// 原函数：0x0049F680。
std::int32_t WindowTextTarget_SelectRow(std::int32_t index, std::int32_t state);

// 原窗口46的28B属性源；仅名称地址扩宽，4874B0/48B0A0分别生产。
struct WindowStatusChoiceRecord {
    const char* name; // 原成员偏移：+0x00。
    std::int32_t level; // 原成员偏移：+0x04。
    std::int32_t hp; // 原成员偏移：+0x08。
    std::int32_t maxHp; // 原成员偏移：+0x0C。
    std::int32_t fp; // 原成员偏移：+0x10。
    std::int32_t maxFp; // 原成员偏移：+0x14。
    std::int8_t injury; // 原成员偏移：+0x18。
    std::uint8_t color; // 原成员偏移：+0x19。
    std::array<std::byte, 2> bytes1A; // 原成员偏移：+0x1A。
};
static_assert(offsetof(WindowStatusChoiceRecord, level) == 8);
static_assert(offsetof(WindowStatusChoiceRecord, injury) == 0x1C);
static_assert(offsetof(WindowStatusChoiceRecord, color) == 0x1D);
static_assert(sizeof(WindowStatusChoiceRecord) == 32);
// 原函数：0x0049F760。
login::YobiAction* WindowAttributeTarget_Open(
    const WindowStatusChoiceRecord* records, std::int32_t count,
    WindowTextChoiceCallback callback); // 三槽按序发布后打开46，透传借用Yobi。
// 原函数：0x0049F790。
void WindowAttributeTarget_Release();
// 原函数：0x0049F7B0。
std::int32_t WindowAttributeTarget_Update(std::int32_t value);
// 原函数：0x0049F7E0。
void WindowAttributeTarget_Refresh(std::int32_t offset); // 六行名称/等级/HP/FP及伤势图标。
// 原函数：0x00499830。
std::int32_t WindowRuntime_DrawLargeFrame(std::int32_t value); // 31/46共用绘制与九宫格尾。
// 原函数：0x00499880。
std::int32_t WindowRuntime_ButtonCloseResult(std::int32_t index, std::int32_t state); // 结果窗关闭按钮，固定入口图形载荷。
// 原函数：0x004990F0。
std::int32_t WindowSpectator_ButtonEnd(std::int32_t index, std::int32_t state); // 窗口29观战结束，hover不消费。
// 原函数：0x00499750。
std::int32_t WindowBattleResult_ButtonClose(std::int32_t index, std::int32_t state); // 窗口30点击关闭，索引未使用。
// 原函数：0x0049FA40。
std::int32_t WindowAttributeTarget_ButtonCancel(std::int32_t index, std::int32_t state);
// 原函数：0x0049FB00。
std::int32_t WindowAttributeTarget_ButtonBack(std::int32_t index, std::int32_t state);
// 原函数：0x0049FBC0。
std::int32_t WindowAttributeTarget_SelectRow(std::int32_t index, std::int32_t state);

// 原4B1260的14参槽位回调；借用数值与描述符，返回值原端弃用。
using WindowTradePanelCallback = void (*)(
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* amount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);

// 原CF3B10/D2F338的已消费字段；不是x86字节布局，不冻结未消费前缀。
struct WindowItemSelectorState {
    std::array<WindowElement*, 20> elements00; // 原成员偏移：+0x00。 4C6EC0绑定的非拥有元素，非零模式保留槽19。
    std::array<WindowElementCallback, 4> callbacks50; // 原成员偏移：+0x50。 4C6550写回元素mouseCallback。
    std::int32_t value60; // 原成员偏移：+0x60。 4C6550发布的数值页状态，4C75A0仅在2时绘制。
    std::int32_t value64; // 原成员偏移：+0x64。
    std::uint8_t* mode68; // 原成员偏移：+0x68。 借用57DA28/57E42C，消费者按S8解释。
    std::int32_t step6C; // 原成员偏移：+0x6C。
    std::int32_t selectedPet70; // 原成员偏移：+0x70。
    std::int32_t previousPet74; // 原成员偏移：+0x74。
    login::YobiAction* preview; // 借用原内嵌+78的静态对象，不负责释放。
    std::int32_t hoverCount3A8; // 原成员偏移：+0x3A8。
    std::array<std::byte, 4> bytes3AC; // 原成员偏移：+0x3AC。
    std::int32_t hoverIndex3B0; // 原成员偏移：+0x3B0。
    std::array<std::byte, 4> bytes3B4; // 原成员偏移：+0x3B4。
    std::array<render::GraphicDescriptor16, 3> descriptors3B8; // 原成员偏移：+0x3B8。
    std::int32_t initialized3E8; // 原成员偏移：+0x3E8。 7A90首次切页后置1，库存更新/释放及构造清零。
    std::uint8_t value3EC; // 原成员偏移：+0x3EC。
    std::uint8_t value3ED; // 原成员偏移：+0x3ED。
    std::int32_t imageIndex3F0; // 原成员偏移：+0x3F0。
    std::int32_t imageIndex3F4; // 原成员偏移：+0x3F4。
    std::int32_t hoverIndex3F8; // 原成员偏移：+0x3F8。
    std::uint8_t value3FC; // 原成员偏移：+0x3FC。
};

extern WindowItemSelectorState gItemSelectorCF3B10; // 原全局：0x00CF3B10。
extern WindowItemSelectorState gItemSelectorD2F338; // 原全局：0x00D2F338。
// 原函数：0x004C61B0。
WindowItemSelectorState* WindowItemSelector_ConstructPreview(WindowItemSelectorState* state, std::int32_t previewIndex); // SIC01：入口预览地址贯穿十成员构造与字段发布；原初始化/三descriptor及this返回保持，整对象不清零。
// SLC01：单读并返回同一选择器selectedPet70的完整S32，不做校验或改owner。
// 原函数：0x004C71B0。
std::int32_t WindowItemSelector_ReadSelectedPet(const WindowItemSelectorState* state); // 原+70同源S32。
// 原函数：0x004C6EC0。
void WindowItemSelector_BindElements(WindowItemSelectorState* state, std::uint8_t mode); // SFD01：低BYTE模式装配非拥有元素/四回调；逐次借当前窗口，非零模式槽19不写，发布次序保持。
// SLC01：负slot先返-1；S8 mode0取8项玩家X表，正mode取6项宠物X表，其它返-1。
// 原函数：0x004C63C0。
std::int32_t WindowItemSelector_ReadSlotX(const WindowItemSelectorState* state, std::int32_t slot);
// SLC01：负slot先返-1；S8 mode0取8项玩家Y表，正mode取6项宠物Y表，其它返-1。
// 原函数：0x004C6400。
std::int32_t WindowItemSelector_ReadSlotY(const WindowItemSelectorState* state, std::int32_t slot);
// 原函数：0x004C6440。
void WindowItemSelector_ShowPlayerPage(WindowItemSelectorState* state); // SPG01：玩家页2显示槽6；W32装配模式0显示槽19，两门槛独立，原正文保持。
// 原函数：0x004C6470。
void WindowItemSelector_ShowPetPage(WindowItemSelectorState* state); // SPG01：宠物页/名称负重及原动态空位显示保持；只判低AL，不钳制itemSlots或补保护。
// 原函数：0x004C6500。
void WindowItemSelector_HidePageElements(WindowItemSelectorState* state); // SPG01：按原顺序隐藏槽6/7/8/15/16、六装备槽与17；模式0才隐藏19，主元素不改。
// 原函数：0x004C6550。
void WindowItemSelector_SelectPage(WindowItemSelectorState* state, std::int32_t page); // SPG01：先发布页再隐藏/新页分发；CC40D4仅低BYTE、四回调先取元素；原五页和写序保持，全机器ABI待。
// 原函数：0x004C6790。
void WindowItemSelector_UpdatePage(WindowItemSelectorState* state); // SFD01：原四页分发/业务钳制保持；两按钮按原顺序取样，共借descriptor[0]；已知上行由窗口回调返回1。
// 原函数：0x004C6990。
std::int32_t WindowItemSelector_SelectPet(WindowItemSelectorState* state); // PSP01：同state mode按S8映射，回调返回的S32索引原序发布到selectedPet70并返回。
// 原函数：0x004C69B0。
void WindowItemSelector_RefreshPetHeader(WindowItemSelectorState* state); // PSP01：同宠物previous/updated BYTE及名字/负重原序保持；首次测宽先字体后文本，负重先文本后字体。
// 原函数：0x004C6AC0。
void WindowItemSelector_MoveElements(WindowItemSelectorState* state, std::int32_t x); // SPG01：依原序移动主元素、四按钮及六表坐标；原S32结果与固定循环保持，不新增钳制。
// SLC01：两轴先窗口再元素取值，flags/淡入计数只读原BYTE；void不证明原EAX上行等价。
// 原函数：0x004C6B10。
void WindowItemSelector_DrawTooltip(WindowItemSelectorState* state); // 消费本帧hover，宠物物品固定借CF3B10索引。
// 原函数：0x004C6D90。
std::int32_t WindowItemSelector_DrawPreview(WindowItemSelectorState* state); // PSP01：同一非拥有预览推进/两轴/队列；掩码替换字节先旧字节，返回/原读取门槛保持。
// 原函数：0x004C71C0。
std::int32_t WindowItemSelector_FindPetIndex(std::int32_t ordinal); // PSP01：五排序对按used W16/索引S16/状态W16计数；不足仍取末扫描项，不新增匹配/索引保护。
// 原函数：0x004C7340。
bool WindowItemSelector_SetMode(WindowItemSelectorState* state, std::int32_t mode); // SPG01：写mode低BYTE后持旧页；页3按新mode切2/3、页2不切；bool仅承接原AL=1，高位ABI待。
// 原函数：0x004C7380。
bool WindowItemSelector_HandleTab(WindowItemSelectorState* state, std::int32_t tab, std::int32_t events); // SEV01：页签提示/点击/绘制与图号原序；中心坐标显式取样，descriptor地址先图号；bool限原AL消费。
// 原函数：0x004C78A0。
bool WindowItemSelector_TogglePreview(WindowItemSelectorState* state, std::int32_t events); // SEV01：原event低BYTE bit2触发声音后翻转BYTE预览开关，图号8/1优先，正确body保持，bool仅原AL。
// 原函数：0x004C7910。
bool WindowItemSelector_CycleEquipment(WindowItemSelectorState* state, std::int32_t events); // SEV01：先翻BYTE装备/回调，失效AL早返保部分写；目标先回调、尾selected先推进/count后取，不钳制数量。
// 原函数：0x004C7A10。
bool WindowItemSelector_RotatePreview(WindowItemSelectorState* state, std::int32_t events); // SEV01：低U16两个方向门槛独立，先正向S32余8再反向减1/原加8；两声效/原顺序保，父固定返回0。
// 原函数：0x004C7A90。
void WindowItemSelector_Update(WindowItemSelectorState* state); // SFD01：首次借mode地址、previous=-1、切页后置initialized；尾逐帧分发，上行两窗口回调固定返回1。
// ISP01：原local负值返-1，成功后取当前窗口，再按元素X/窗口X取样累加。
// 原函数：0x004C7240。
std::int32_t WindowItemSelector_ReadScreenX(
    const WindowItemSelectorState* state, std::int32_t element, std::int32_t slot,
    std::int32_t offset); // X；负局部坐标不读取窗口。
// ISP01：原local负值返-1，成功后取当前窗口，再按元素Y/窗口Y取样累加。
// 原函数：0x004C7280。
std::int32_t WindowItemSelector_ReadScreenY(
    const WindowItemSelectorState* state, std::int32_t element, std::int32_t slot,
    std::int32_t offset); // Y；与X分开采样模式和当前窗口。
// ISP01：S8模式0借玩家前8槽，正模式经原宠物校验后两次重读索引借列表/数量；失败不改输出。
// 原函数：0x004C72C0。
bool WindowItemSelector_BorrowSelectedItem(
    const WindowItemSelectorState* state, world::detail::PlayerItemRecord** items,
    std::int32_t* count); // 借用同源物品表；失败不写输出。
// ISP01：借同一列表/原动态count，state低BYTE消费；循环尾先重取count再推进槽。
// 原函数：0x0048A6C0。
std::int32_t WindowInventory_DrawBlockedEquipment(std::int32_t index, std::int32_t state); // 装备屏蔽页完整绘制。
// ISP01：原两mode选物/发送/独立取消及绘制保持，按钮和mode2 flags只读原BYTE。
// 原函数：0x00489F00。
std::int32_t WindowInventory_HandleItemSelection(std::int32_t index, std::int32_t state); // 两模式物品选择/退出与完整绘制。
// ISP01：对话29选物先占用/发布两个回调与mode2，启动返回丢弃，再透传焦点绑定S32结果。
// 原函数：0x004888C0。
std::int32_t WindowInventory_OpenSpecialSelection(const char* text); // 对话29选择入口，发布特殊回调后绑定焦点。
// 原函数：0x00488930。
std::int32_t WindowInventory_TogglePreview(std::int32_t unusedIndex, std::int32_t state);
// 原函数：0x00488950。
std::int32_t WindowInventory_CycleEquipment(std::int32_t unusedIndex, std::int32_t state);
// 原函数：0x00488970。
std::int32_t WindowInventory_RotatePreview(std::int32_t unusedIndex, std::int32_t state);
// 原函数：0x00488EA0。
std::int32_t WindowInventory_TabEquipment(std::int32_t unusedIndex, std::int32_t state);
// 原函数：0x00488EC0。
std::int32_t WindowInventory_TabPet(std::int32_t index, std::int32_t state); // SAP01：库存宠物页签，原低AL归一为S32，五对/无效图处理保持。
// 原函数：0x00488C50。
std::int32_t WindowInventory_ButtonClose(std::int32_t index, std::int32_t state); // 库存关闭按钮。
// 原函数：0x00488CF0。
void WindowInventory_DropMoney(std::int32_t amount); // MDR01：原动作BYTE/场景门槛短路，Y-X发送金额，void副作用/完整ABI待。
// 原函数：0x00488D30。
std::int32_t WindowInventory_HandleMoney(std::int32_t index, std::int32_t state); // MDR01：三次fresh窗口匹配/实时金额同步，held graphic及0/1返回保。
// 原函数：0x00488E60。
std::int32_t WindowInventory_RefreshMoney(std::int32_t unusedIndex, std::int32_t unusedState);
// 原函数：0x00488E80。
std::int32_t WindowInventory_RefreshExtraValue(std::int32_t unusedIndex, std::int32_t unusedState);
// 原函数：0x00488FB0。
std::int32_t WindowInventory_HandleItems(std::int32_t index, std::int32_t state); // 普通背包完整交互/绘制。
// 原函数：0x00489760。
std::int32_t WindowInventory_HandleEquipment(std::int32_t index, std::int32_t state); // IER01：库存装备完整原根，四读取合同对齐；原S32返回/全部callee与运行待。
// 原函数：0x00489EE0。
std::int32_t WindowInventory_TabPlayerAttributes(std::int32_t unusedIndex, std::int32_t state);
// 原函数：0x004C75A0。
bool WindowItemSelector_HandlePlayerAttributes(WindowItemSelectorState* selector, std::int32_t state); // SAP01：附加属性页/回调恢复，bool仅原AL，两父低AL归一；完整业务生产/运行待。

// 4C6EC0发布的战斗背包/装备交互。
// 原函数：0x00499930。
std::int32_t WindowBattleItems_TogglePreview(std::int32_t unusedIndex, std::int32_t state);
// 原函数：0x00499950。
std::int32_t WindowBattleItems_CycleEquipment(std::int32_t unusedIndex, std::int32_t state);
// 原函数：0x00499970。
std::int32_t WindowBattleItems_RotatePreview(std::int32_t unusedIndex, std::int32_t state);
// 原函数：0x00499B00。
std::int32_t WindowBattleItems_RefreshMoney(std::int32_t unusedIndex, std::int32_t unusedState);
// 原函数：0x00499B20。
std::int32_t WindowBattleItems_RefreshExtraValue(std::int32_t unusedIndex, std::int32_t unusedState);
// 原函数：0x00499B40。
std::int32_t WindowBattleItems_TabEquipment(std::int32_t unusedIndex, std::int32_t state); // 战斗页签忽略内部返回。
// 原函数：0x00499B60。
std::int32_t WindowBattleItems_TabPet(std::int32_t index, std::int32_t state); // SAP01：战斗宠物页签，成功忽略4C7380结果返回S32零，无效图处理保持。
// 原函数：0x00499C30。
std::int32_t WindowBattleItems_HandleInventory(std::int32_t index, std::int32_t state); // BIP01：完整战斗背包/目标生产，六BYTE及六坐标取序明确；旧格式/CAB30保持，全生命周期与运行待。
// 原函数：0x0049AC40。
std::int32_t WindowBattleItems_HandleEquipment(std::int32_t index, std::int32_t state); // BEI01：战斗装备/宠物禁用页完整原根，三BYTE及区域描边取序明确，全callback/命令与运行待。
// 原函数：0x0049B610。
std::int32_t WindowBattleItems_ShowExtraAttributes(std::int32_t unusedIndex, std::int32_t state); // D2F338附加属性页包装。

// 原CB1398等固定81B对话行；创建入口只借用，不负责释放。
struct WindowDialogTextLine {
    std::wstring text; // UTL02动态UTF-16分行正文；原81字节私有容量退休。
};

const WindowDialogTextLine* WindowDialog_BorrowNpcLines() noexcept; // 借用标题及正文，旧/现代对话同源。
std::int32_t WindowShopChoice_ReadSkillImage() noexcept; // 4A0640生产，旧55/56/57共享预览消费。
std::int32_t WindowShopChoice_ReadExchangeImage() noexcept; // 4A0790生产，旧74预览消费。
std::int32_t WindowShopChoice_ReadShopImage() noexcept; // 4A0140生产，旧62预览借读同源图号。
std::int32_t WindowShopChoice_ReadGiveReceiveImage() noexcept; // 4A0850生产，旧83预览消费。

// 原CB1E40/D2E458/CB0AE8确认显示记录；三商店各自持有，数值布局保持48B。
struct WindowConfirmationItem {
    std::int32_t imageNo; // +00。
    std::int32_t quantity; // +04，数量文本第二项。
    std::int32_t factor; // +08，非零时显示乘数前缀。
    std::int32_t color; // +0C，绘制只读低S8。
    std::array<char, 31> name; // +10原字节文本。
    std::uint8_t visible; // +2F，不与名称分配另一份重叠存储。
};
static_assert(sizeof(WindowConfirmationItem) == 48);
static_assert(offsetof(WindowConfirmationItem, quantity) == 4);
static_assert(offsetof(WindowConfirmationItem, factor) == 8);
static_assert(offsetof(WindowConfirmationItem, color) == 12);
static_assert(offsetof(WindowConfirmationItem, name) == 16);
static_assert(offsetof(WindowConfirmationItem, visible) == 47);

// 普通/晶石商店各两条确认数值，不持有指针。
struct WindowConfirmationAmount {
    std::int32_t value;
    std::int16_t x; // 原成员偏移：+0x04。
    std::int16_t y; // 原成员偏移：+0x06。
    std::int32_t visible; // 原成员偏移：+0x08。
};
static_assert(sizeof(WindowConfirmationAmount) == 12);
static_assert(offsetof(WindowConfirmationAmount, x) == 4);
static_assert(offsetof(WindowConfirmationAmount, y) == 6);
static_assert(offsetof(WindowConfirmationAmount, visible) == 8);

// 原D2EAB8的60H技能报价记录；未消费字节不据此定业务语义。
struct Window54SkillRecord {
    std::uint16_t used;
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t price; // 原成员偏移：+0x08。
    std::array<char, 25> name; // 原成员偏移：+0x0C。
    std::array<std::array<char, 29>, 2> description; // 原成员偏移：+0x25。
    std::byte byte5F; // 原成员偏移：+0x5F。
};
static_assert(sizeof(Window54SkillRecord) == 0x60);
static_assert(offsetof(Window54SkillRecord, value04) == 4);
static_assert(offsetof(Window54SkillRecord, price) == 8);
static_assert(offsetof(Window54SkillRecord, name) == 0x0C);
static_assert(offsetof(Window54SkillRecord, description) == 0x25);
static_assert(offsetof(Window54SkillRecord, byte5F) == 0x5F);

// 原D67448五个候选宠物28H记录，协议索引不等同于玩家宠物槽。
struct Window54PetOption {
    std::uint16_t used;
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t slots; // 原成员偏移：+0x08。
    std::array<char, 17> name; // 原成员偏移：+0x0C。
    std::uint8_t allowed; // 原成员偏移：+0x1D。
    std::array<std::byte, 2> bytes1E; // 原成员偏移：+0x1E。
    std::int32_t tribe; // 原成员偏移：+0x20。
    std::uint8_t hintFlag; // 原成员偏移：+0x24。
    std::array<std::byte, 3> bytes25; // 原成员偏移：+0x25。
};
static_assert(sizeof(Window54PetOption) == 0x28);
static_assert(offsetof(Window54PetOption, value04) == 4);
static_assert(offsetof(Window54PetOption, slots) == 8);
static_assert(offsetof(Window54PetOption, name) == 0x0C);
static_assert(offsetof(Window54PetOption, allowed) == 0x1D);
static_assert(offsetof(Window54PetOption, tribe) == 0x20);
static_assert(offsetof(Window54PetOption, hintFlag) == 0x24);

// 原D66E38十条5CH技能槽，供模式4显示及确认覆盖层借用。
struct Window54TechniqueSlot {
    std::uint16_t used;
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::array<char, 25> name; // 原成员偏移：+0x08。
    std::array<std::array<char, 29>, 2> description; // 原成员偏移：+0x21。
    std::byte byte5B; // 原成员偏移：+0x5B。
};
static_assert(sizeof(Window54TechniqueSlot) == 0x5C);
static_assert(offsetof(Window54TechniqueSlot, value04) == 4);
static_assert(offsetof(Window54TechniqueSlot, name) == 8);
static_assert(offsetof(Window54TechniqueSlot, description) == 0x21);
static_assert(offsetof(Window54TechniqueSlot, byte5B) == 0x5B);

// 原CB2230/D30428晶石商品；不同于普通商店C38H记录，保留区尚不定语义。
struct WindowCrystalItem {
    std::uint8_t used; // 原成员偏移：+0x00。
    std::array<char, 29> name; // 原成员偏移：+0x01。
    std::array<char, 3072> description; // 原成员偏移：+0x1E。
    std::array<std::byte, 2> bytesC1E; // 原成员偏移：+0xC1E。
    std::int32_t imageNo; // 原成员偏移：+0xC20。
    std::int32_t valueC24; // 原成员偏移：+0xC24。
    std::int32_t selectedCount; // +C28，两种晶石商店共用的数量字段语义。
    std::int32_t valueC2C; // 原成员偏移：+0xC2C。
    std::uint32_t flags; // 原成员偏移：+0xC30。
    std::array<char, 24> tips; // 原成员偏移：+0xC34。
    std::int32_t limitC4C; // 原成员偏移：+0xC4C。
    std::int32_t remainingC50; // 原成员偏移：+0xC50。
};
static_assert(sizeof(WindowCrystalItem) == 0xC54);
static_assert(offsetof(WindowCrystalItem, used) == 0);
static_assert(offsetof(WindowCrystalItem, name) == 1);
static_assert(offsetof(WindowCrystalItem, description) == 0x1E);
static_assert(offsetof(WindowCrystalItem, imageNo) == 0xC20);
static_assert(offsetof(WindowCrystalItem, valueC24) == 0xC24);
static_assert(offsetof(WindowCrystalItem, selectedCount) == 0xC28);
static_assert(offsetof(WindowCrystalItem, valueC2C) == 0xC2C);
static_assert(offsetof(WindowCrystalItem, flags) == 0xC30);
static_assert(offsetof(WindowCrystalItem, tips) == 0xC34);
static_assert(offsetof(WindowCrystalItem, limitC4C) == 0xC4C);
static_assert(offsetof(WindowCrystalItem, remainingC50) == 0xC50);

using WindowDialogReplyCallback = std::int32_t (*)(
    std::int32_t button, std::int32_t row, const std::wstring* texts, std::uint8_t inputCount);
using WindowDialogFrameCallback = std::int32_t (*)(
    WindowInstance* window, std::int32_t button, std::int32_t row, std::int32_t events);
using WindowChoiceCallback = std::int32_t (*)(std::int32_t index);
// 原函数：0x0049FCA0。
login::YobiAction* WindowShopChoice_Open(
    std::wstring_view title, std::int32_t imageNo, const std::wstring* const* lines, std::uint8_t lineCount,
    const wchar_t* const* choices, std::uint8_t choiceCount,
    const wchar_t* const* hints, WindowChoiceCallback callback); // 窗口52只借用文本及回调。
// 原函数：0x0049FFA0。
std::int32_t WindowShopChoice_SelectRow(std::int32_t unusedIndex, std::int32_t state);
// 原函数：0x004A00C0。
std::int32_t WindowShopChoice_SendBuySellChoice(std::int32_t index); // 商店回复仅索引0..1发送，其他返回原索引。
// 原函数：0x004A0140。
login::YobiAction* WindowShopChoice_OpenBuySell(const char* text); // 商店图号/标题/五行正文及买卖选择。
// 原函数：0x004B6E00。
login::YobiAction* WindowShop_OpenBuy(const char* text); // 窗口53买入商品及三组四行正文。
// 原函数：0x004B70D0。
login::YobiAction* WindowShop_OpenSell(const char* text); // 窗口53出售商品、库存映射及占用。
// 原函数：0x004B74E0。
login::YobiAction* WindowShop_OpenExchange(const char* text); // 窗口53模式2五字段商品及指定物品文本。
// 原函数：0x004B77E0。
login::YobiAction* WindowShop_OpenItemDetails(const char* text); // 窗口53模式3独立详情，保留其他商品状态。
// 原函数：0x004B7AB0。
login::YobiAction* WindowShop_OpenIdentify(const char* text); // 窗口53鉴定，库存映射保留未匹配物品哨兵20。
// 原函数：0x004B7EE0。
login::YobiAction* WindowShop_OpenRepair(const char* text); // 窗口53修理，与鉴定仅模式和提示不同。
// 原函数：0x004B8310。
void WindowShop_RefreshBuyRows(); // 买入五行名称/价格/不可装备提示，保留原分行终止偏移。
// 原函数：0x004B8670。
void WindowShop_RefreshBuyQuantities(); // 买入五行所选数量文本。
// 原函数：0x004B86C0。
void WindowShop_RefreshBuyAmount(); // 全20商品金额合计及元素7颜色。
// 原函数：0x004B8730。
void WindowShop_CountBuySlots(); // 全20商品按上限分组计算占用数量。
// 原函数：0x004B8780。
void WindowShop_RefreshSellRows(); // 出售五行按S8选择槽取商品，不另检查use。
// 原函数：0x004B8B10。
void WindowShop_RefreshSellQuantities(); // 出售数量及上限>1时的加减元素可见性。
// 原函数：0x004B8BE0。
void WindowShop_RefreshSellAmount(); // 全20选择槽的金额合计，元素7颜色固定0。
// 原函数：0x004B8C50。
void WindowShop_CountSellItems(); // 全20选择槽直接累计所选数，不做买入分组。
// 原函数：0x004B8C90。
void WindowShop_RefreshExchangeRows(); // 模式2五行，按指定物品数量与价格加合计比较。
// 原函数：0x004B8FF0。
void WindowShop_RefreshExchangeQuantities(); // 模式2数量，使用项直接重显加减元素。
// 原函数：0x004B9090。
void WindowShop_RefreshExchangeAmount(); // 模式2合计写CF4A28，不写金额颜色。
// 原函数：0x004B90E0。
void WindowShop_CountExchangeItems(); // 模式2全部使用商品累计所选数。
// 原函数：0x004B9110。
void WindowShop_RefreshDetailName(); // 模式3首行详情名与数值，短名称的次行借593850。
// 原函数：0x004B9270。
void WindowShop_RefreshDetailAmount(); // 模式3三次读取D046C4，先两个显示值后乘详情单价。
// 原函数：0x004B92C0。
void WindowShop_CountDetailItems(); // 模式3详情数值或全部28玩家物品的匹配数量。
// 原函数：0x004BBED0。
std::int32_t WindowShop_HandleItemIcon(std::int32_t index, std::int32_t state); // 商品图标，仅hover内bit4翻页。
// 原函数：0x004BC060。
std::int32_t WindowShop_HandleDetailIcon(std::int32_t index, std::int32_t state); // 独立详情图标，bit4无hover门禁。
// 原函数：0x004BC230。
std::int32_t WindowShop_HandleSaleSelection(std::int32_t index, std::int32_t state); // 出售选择行，接kind9交换/取消/拖动。
// 原函数：0x004BC660。
std::int32_t WindowShop_HandleInventory(std::int32_t index, std::int32_t state); // 背包20格命中、加入/拖动/翻页及两组绘制。
// 原函数：0x004B9B10。
void WindowShop_Submit(std::int32_t selection); // 确认选择低U8为0才按模式发送，返回值不消费。
// 原函数：0x004B9F40。
std::int32_t WindowShop_ButtonOk(std::int32_t index, std::int32_t state); // 六模式确认生产，借用同源覆盖层。
// 原函数：0x004BD100。
std::int32_t WindowShop_HandleConfirmation(std::int32_t index, std::int32_t unusedState); // 确认覆盖层，事件直接读共享鼠标。
// 原函数：0x004BD540。
login::YobiAction* WindowPetSkillChoice_OpenLearn(const char* text); // 窗口54模式0，五行正文及八行说明。
// 原函数：0x004BD680。
login::YobiAction* WindowPetSkillChoice_OpenForget(const char* text); // 窗口54模式1，固定十五项名称/双数值。
// 原函数：0x004BD810。
login::YobiAction* WindowPetSkillChoice_OpenPetSkills(const char* text); // 窗口54模式2，技能报价及旧窗刷新。
// 原函数：0x004BDAD0。
login::YobiAction* WindowPetSkillChoice_OpenPetSkillsByTribe(const char* text); // 同创建流程，模式12显示种族。
// 原函数：0x004BDD90。
void WindowPetSkillChoice_ReceivePetCandidates(const char* text); // 模式3候选回复，仅更新存在的窗口54。
// 原函数：0x004BDF70。
void WindowPetSkillChoice_ReceiveTribeCandidates(const char* text); // 模式13候选回复，带种族条件及提示标记。
// 原函数：0x004BE200。
void WindowPetSkillChoice_ReceiveSelectedPetSkills(const char* text); // 模式4固定十槽回复，空名不终止后续槽。
// 原函数：0x004BE3F0。
void WindowPetSkillChoice_RefreshLearn(WindowInstance* window); // 模式0技能栏/说明/花费，reqSlot按低S8。
// 原函数：0x004BE600。
void WindowPetSkillChoice_RefreshForget(WindowInstance* window); // 模式1遗忘映射/警告/回调，与原技能表同源。
// 原函数：0x004BE930。
void WindowPetSkillChoice_RefreshPetNames(WindowInstance* window); // 固定旧窗的五宠物名称/元素及技能分页刷新。
// 原函数：0x004BEC20。
void WindowPetSkillChoice_RefreshPetCandidates(WindowInstance* window); // 玩家索引对转候选映射，绑定真实选择回调。
// 原函数：0x004BEF30。
void WindowPetSkillChoice_RefreshSelectedPetSkills(WindowInstance* window); // 技能槽计数/候选原序显示，先右侧再左侧刷新。
// 原函数：0x004BF160。
void WindowPetSkillChoice_RefreshLearnRows(WindowInstance* window, std::int32_t scroll, std::int32_t count); // 模式0十行原技能。
// 原函数：0x004BF2E0。
void WindowPetSkillChoice_RefreshForgetRows(WindowInstance* window, std::int32_t scroll, std::int32_t count); // 模式1十行协议名称/双数值。
// 原函数：0x004BF460。
void WindowPetSkillChoice_RefreshPetRows(WindowInstance* window, std::int32_t scroll); // 五行，存在性与内容索引不同。
// 原函数：0x004BF620。
void WindowPetSkillChoice_RefreshSelectedSkillRows(WindowInstance* window, std::int32_t scroll); // 按候选槽数逐轮重读，不加十行钳制。
// 原函数：0x004BFBC0。
std::int32_t WindowPetSkillChoice_ButtonLearn(std::int32_t index, std::int32_t state); // 模式0学习请求，bit2恒消费。
// 原函数：0x004BFCB0。
std::int32_t WindowPetSkillChoice_ButtonBack(std::int32_t index, std::int32_t state); // 分模式关闭/返回，音效后重读模式。
// 原函数：0x004BFF60。
std::int32_t WindowPetSkillChoice_ButtonPreviousRow(std::int32_t index, std::int32_t state); // bit400上一行，触边仍刷新。
// 原函数：0x004C0160。
std::int32_t WindowPetSkillChoice_ButtonNextRow(std::int32_t index, std::int32_t state); // 下一行，12/13固定左侧且目标index-1。
// 原函数：0x004C0380。
std::int32_t WindowPetSkillChoice_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 上一页，音效后重读位置。
// 原函数：0x004C0590。
std::int32_t WindowPetSkillChoice_ButtonNextPage(std::int32_t index, std::int32_t state); // 下一页，先上限后负值修正。
// 原函数：0x004C0800。
std::int32_t WindowPetSkillChoice_Scroll(std::int32_t index, std::int32_t state); // 两侧滚轮；模式0/1只更新位置。
// 原函数：0x004C0A50。
std::int32_t WindowPetSkillChoice_SelectPetCandidate(std::int32_t index, std::int32_t state); // 候选门禁/提示/发送，hintFlag不阻止发送。
// 原函数：0x004C0910。
std::int32_t WindowPetSkillChoice_SubmitForget(std::int32_t index, std::int32_t state); // 遗忘映射+1发送，发送后关闭窗口。
// 原函数：0x004C0BF0。
std::int32_t WindowPetSkillChoice_SelectSkillSlot(std::int32_t index, std::int32_t state); // 选技能槽并显示元素0，不提前发送。
// 原函数：0x004C0CB0。
std::int32_t WindowPetSkillChoice_SubmitPetSkill(std::int32_t index, std::int32_t state); // 技能选择/请求，发送后对话类型=-2。
// 原函数：0x004C0EA0。
std::int32_t WindowPetSkillChoice_DrawSelectedSkillFrame(std::int32_t index, std::int32_t unusedState); // 已选技能内框，恒0。
// 原函数：0x004C0F20。
std::int32_t WindowPetSkillChoice_HandleConfirmation(std::int32_t index, std::int32_t unusedState); // 确认覆盖层，发原选槽文本后请求关闭。
// 原函数：0x004C5790。
login::YobiAction* WindowAppearance_Open(
    std::int32_t request, std::int32_t target, const char* text); // 两行正文后打开外观选择56。
// 原函数：0x004C5C30。
std::int32_t WindowAppearance_ButtonOk(std::int32_t index, std::int32_t state); // 图号原文本直接发送，不转义。
// 原函数：0x004C5D00。
std::int32_t WindowAppearance_CycleClass(std::int32_t index, std::int32_t state); // class按版本上下界循环。
// 原函数：0x004C5DE0。
std::int32_t WindowAppearance_CycleFifthDigit(std::int32_t index, std::int32_t state); // fifth分量0..4循环。
// 原函数：0x004C5EB0。
std::int32_t WindowAppearance_CycleOnesDigit(std::int32_t index, std::int32_t state); // ones分量0..4循环。
// 原函数：0x004C5F80。
std::int32_t WindowAppearance_SendDialogReply(
    std::int32_t button, std::int32_t row, const std::wstring* unusedTexts,
    std::uint8_t unusedInputCount); // 类型37回复，图号转义后发完整button。
// 原函数：0x004C6060。
login::YobiAction* WindowAppearance_OpenDialogPreview(
    std::int32_t buttons, std::int32_t request, std::int32_t target,
    const char* text); // 类型37内嵌图号及左侧14空格正文。
// 原函数：0x004C13B0。
login::YobiAction* WindowCrystalShop_Open(const char* text); // 窗口69商品，remaining非-1才可能下调limit。
// 原函数：0x004C3550。
login::YobiAction* WindowCrystalExchangeShop_Open(const char* text); // 窗口70七组正文/附加值，不下调limit。
// 原函数：0x004C1700。
void WindowCrystalShop_RefreshRemaining(); // 窗口69五条剩余量文本。
// 原函数：0x004C17B0。
void WindowCrystalShop_RefreshRows(); // 名称两行和价格，保留原分行空隙字节。
// 原函数：0x004C1AA0。
void WindowCrystalShop_RefreshQuantities(); // 五条已选数量。
// 原函数：0x004C1B00。
void WindowCrystalShop_RefreshAmount(); // 总额与玩家晶石比较，写type7 color。
// 原函数：0x004C1B70。
void WindowCrystalShop_CountSlots(); // 二十条选中商品占槽。
// 原函数：0x004C1BC0。
void WindowCrystalShop_Refresh(); // 首次/分页/数量脏标刷新。
// 原函数：0x004C2060。
void WindowCrystalShop_Submit(std::int32_t value); // 低U8为0才按实时商品组包，命令0。
// 原函数：0x004C21B0。
std::int32_t WindowCrystalShop_ButtonOk(std::int32_t index, std::int32_t state); // 余额/背包门禁及确认预览生产。
// 原函数：0x004C2FA0。
std::int32_t WindowCrystalShop_HandleConfirmation(std::int32_t index, std::int32_t unusedState); // 窗口69确认覆盖层。
// 原函数：0x004C2B20。
std::int32_t WindowCrystalShop_HandleInventory(std::int32_t index, std::int32_t state); // 背包20格命中和绘制。
// 原函数：0x004C29E0。
std::int32_t WindowCrystalShop_HandleItemIcon(std::int32_t index, std::int32_t state); // 窗口69商品图标及四页提示。
// 原函数：0x004C2460。
std::int32_t WindowCrystalTrade_ButtonCancel(std::int32_t index, std::int32_t state); // 两窗口共用取消，发送command2。
// 原函数：0x004C2530。
std::int32_t WindowCrystalShop_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 窗口69上一行。
// 原函数：0x004C25B0。
std::int32_t WindowCrystalShop_ButtonNextRow(std::int32_t index, std::int32_t state); // 下一行。
// 原函数：0x004C2630。
std::int32_t WindowCrystalShop_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 上一页，真实入口为4C2630。
// 原函数：0x004C26B0。
std::int32_t WindowCrystalShop_ButtonNextPage(std::int32_t index, std::int32_t state); // 下一页。
// 原函数：0x004C2760。
std::int32_t WindowCrystalShop_Scroll(std::int32_t index, std::int32_t state); // 滚轮，仅回写分页。
// 原函数：0x004C27B0。
std::int32_t WindowCrystalShop_ButtonDecrease(std::int32_t index, std::int32_t state); // 减数量，无used门禁，事件尾清消息。
// 原函数：0x004C28C0。
std::int32_t WindowCrystalShop_ButtonIncrease(std::int32_t index, std::int32_t state); // 加数量，used/limit门禁，不清消息。
// 原函数：0x004C33B0。
std::int32_t WindowCrystalShop_ShowTooltip(
    std::int32_t index, std::int32_t page, std::int32_t unused); // 保留caller第三实参。
// 原函数：0x004C39A0。
void WindowCrystalExchangeShop_RefreshRemaining(); // 窗口70五条剩余量文本。
// 原函数：0x004C3A50。
void WindowCrystalExchangeShop_RefreshRows(); // 名称两行和价格，独立分页/显示缓冲。
// 原函数：0x004C3D40。
void WindowCrystalExchangeShop_RefreshQuantities(); // 五条已选数量。
// 原函数：0x004C3DA0。
void WindowCrystalExchangeShop_RefreshAmount(); // 总额与指定背包物品数量比较。
// 原函数：0x004C3E20。
void WindowCrystalExchangeShop_CountSlots(); // 二十条选中商品占槽。
// 原函数：0x004C3E70。
void WindowCrystalExchangeShop_Refresh(); // 首次/分页/数量脏标刷新。
// 原函数：0x004C4270。
void WindowCrystalExchangeShop_Submit(std::int32_t value); // 低U8为0才按实时商品组包，命令1。
// 原函数：0x004C43C0。
std::int32_t WindowCrystalExchangeShop_ButtonOk(std::int32_t index, std::int32_t state); // 交换物数量/背包门禁及确认预览。
// 原函数：0x004C50C0。
std::int32_t WindowCrystalExchangeShop_HandleConfirmation(std::int32_t index, std::int32_t unusedState); // 窗口70确认覆盖层。
// 原函数：0x004C4CB0。
std::int32_t WindowCrystalExchangeShop_HandleInventory(std::int32_t index, std::int32_t state); // 仅首格命中，仍绘完整20格。
// 原函数：0x004C4B70。
std::int32_t WindowCrystalExchangeShop_HandleItemIcon(std::int32_t index, std::int32_t state); // 窗口70商品图标及四页提示。
// 原函数：0x004C4680。
std::int32_t WindowCrystalTrade_DrawButton(std::int32_t index, std::int32_t state); // 两窗口共用按钮图，恒0。
// 原函数：0x004C46C0。
std::int32_t WindowCrystalExchangeShop_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 窗口70上一行。
// 原函数：0x004C4740。
std::int32_t WindowCrystalExchangeShop_ButtonNextRow(std::int32_t index, std::int32_t state); // 下一行。
// 原函数：0x004C47C0。
std::int32_t WindowCrystalExchangeShop_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 上一页。
// 原函数：0x004C4840。
std::int32_t WindowCrystalExchangeShop_ButtonNextPage(std::int32_t index, std::int32_t state); // 下一页。
// 原函数：0x004C48F0。
std::int32_t WindowCrystalExchangeShop_Scroll(std::int32_t index, std::int32_t state); // 滚轮，仅回写分页。
// 原函数：0x004C4940。
std::int32_t WindowCrystalExchangeShop_ButtonDecrease(std::int32_t index, std::int32_t state); // 减数量，窗口70独立脏标。
// 原函数：0x004C4A50。
std::int32_t WindowCrystalExchangeShop_ButtonIncrease(std::int32_t index, std::int32_t state); // 加数量，零上限采用交换文案。
// 原函数：0x004C54D0。
std::int32_t WindowCrystalExchangeShop_ShowTooltip(
    std::int32_t index, std::int32_t page, std::int32_t unused); // 共用D303FC提示键。
// 原函数：0x004BA950。
std::int32_t WindowShop_ButtonCancel(std::int32_t index, std::int32_t state); // 取消发送command2，发送后关闭。
// 原函数：0x004BAA50。
std::int32_t WindowShop_ButtonAddAll(std::int32_t index, std::int32_t state); // 批量加入出售选择，每轮固定重置/音效尾。
// 原函数：0x004BCEE0。
std::int32_t WindowShop_HandleSaleDrop(std::int32_t index, std::int32_t state); // kind2拖放加入，hover模式门禁独立。
// 原函数：0x004BAD10。
std::int32_t WindowShop_ButtonPreviousRow(std::int32_t index, std::int32_t state); // bit400向前一行，触边仍刷新。
// 原函数：0x004BAE10。
std::int32_t WindowShop_ButtonNextRow(std::int32_t index, std::int32_t state); // bit400向后一行，触边仍刷新。
// 原函数：0x004BAF10。
std::int32_t WindowShop_ButtonPreviousPage(std::int32_t index, std::int32_t state); // bit2向前五行，音效后重读位置。
// 原函数：0x004BB010。
std::int32_t WindowShop_ButtonNextPage(std::int32_t index, std::int32_t state); // bit2向后五行，先上限后负值修正。
// 原函数：0x004BB130。
std::int32_t WindowShop_Scroll(std::int32_t index, std::int32_t state); // 共享滚轮更新并刷新，恒返回0。
// 原函数：0x004BB1D0。
std::int32_t WindowShop_ButtonDecrease(std::int32_t index, std::int32_t state); // 数量减少，兑换模式两轮背包占槽计算。
// 原函数：0x004BB6E0。
std::int32_t WindowShop_ButtonIncrease(std::int32_t index, std::int32_t state); // 数量增加，保留行价格与选择槽索引差别。
// 原函数：0x004B6C50。
void WindowShop_HandleSaleDrag(
    std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // kind9出售拖动生命周期。
// 原函数：0x004B6DA0。
std::int32_t WindowShop_StartSaleDrag(std::int32_t sourceIndex); // 透传kind9注册返回。
// 原函数：0x004BC150。
void WindowShop_RemoveSaleSelection(); // 三次读取来源，撤销选择并刷新窗口53。
// 原函数：0x004A0280。
std::int32_t WindowShopChoice_SendFirstCommandChoice(std::int32_t index); // CB03D4命令表的选择回复。
// 原函数：0x004A0420。
std::int32_t WindowShopChoice_SendSecondCommandChoice(std::int32_t index); // CF47D8命令表的选择回复。
// 原函数：0x004A05C0。
std::int32_t WindowShopChoice_SendExchangeChoice(std::int32_t index); // 索引0..1分别发送1/2。
// 原函数：0x004A0700。
std::int32_t WindowShopChoice_SendGiveReceiveChoice(std::int32_t index); // 仅索引0发送原字节1。
// 原函数：0x004A0300。
login::YobiAction* WindowShopChoice_OpenFirstCommandList(const char* text);
// 原函数：0x004A04A0。
login::YobiAction* WindowShopChoice_OpenSecondCommandList(const char* text);
// 原函数：0x004A0640。
login::YobiAction* WindowShopChoice_OpenSkillChoice(const char* text); // 技能三选项。
// 原函数：0x004A0790。
login::YobiAction* WindowShopChoice_OpenExchangeChoice(const char* text); // 交易两选项。
// 原函数：0x004A0850。
login::YobiAction* WindowShopChoice_OpenGiveReceiveChoice(const char* text); // 给出/接收标签，正文取第四字段。
// 原函数：0x0049C4D0。
void WindowDialog_Reset(); // 只清内嵌图号/计数及窗口36十元素可见性。
// 原函数：0x0049C510。
void WindowDialog_AddImage(
    std::uint8_t used, std::int32_t imageNo, std::uint8_t direction,
    std::int16_t x, std::int16_t y);
// 原函数：0x0049CE70。
std::int32_t WindowDialog_HandleButtons(std::int32_t index, std::int32_t state); // 六按钮与原位图号恢复。
// 原函数：0x0049CF70。
std::int32_t WindowDialog_HandleTextSelection(std::int32_t unusedIndex, std::int32_t state); // 输入行切换/文本行选择。
// 原函数：0x0049DA30。
std::int32_t WindowDialog_Reply(
    std::int32_t button, std::int32_t row, const std::wstring* texts, std::uint8_t inputCount);
// 原函数：0x0049D2E0。
void WindowDialog_Route(
    std::int32_t type, std::int32_t value, std::int32_t request,
    std::int32_t target, char* text); // SRD01：完整同类续接/重入新建及原五参数分派，原位text与副作用保持，非全机器返回等价。
// 原函数：0x0049D8F0。
void WindowProfile_RouteReply(
    std::int32_t type, std::int32_t value, std::int32_t request,
    std::int32_t target, char* text); // 先发布同源对话状态，再生产并打开60/61/62。
// 原函数：0x0050E130。
void WindowDialog_RouteReply(
    std::uintptr_t unusedSocket, std::int32_t type, std::int32_t value,
    std::int32_t request, std::int32_t target, char* text); // 50E110门禁后原共享尾入口。
extern std::int32_t gDialogValueCF4E5C; // 原全局：0x00CF4E5C。 原第二协议参数；虚拟零S32，后续UI按位读取。
// 原函数：0x0049DDB0。
void WindowDialog_ParseGraphicTag(const char* text, std::int16_t x, std::int16_t y); // 内嵌g,标记原字节解析。
// 原函数：0x0049DF30。
login::YobiAction* WindowDialog_OpenText(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t imageNo, std::int32_t buttons, std::int32_t request,
    std::int32_t target, char* text); // 原位文本处理，借用共享行表创建常规对话。
// 原函数：0x0049E1D0。
std::int32_t WindowDialog_DrawHealing(
    WindowInstance* window, std::int32_t button, std::int32_t row, std::int32_t unusedEvents);
// 原函数：0x0049E400。
login::YobiAction* WindowDialog_OpenHealing(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t imageNo, std::int32_t unusedButtons, std::int32_t request,
    std::int32_t target, const char* text); // 六治疗记录及第七合计项，共借CB1398行表。
// 原函数：0x0049C600。
login::YobiAction* WindowDialog_Open(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t imageNo, std::int32_t buttons, WindowDialogTextLine* lines,
    std::int32_t lineCount, std::int32_t lineHeight, std::int32_t firstSelectableLine,
    std::uint8_t inputCount, std::int16_t* inputY, std::int32_t inputCapacity,
    std::uint8_t distance, std::uint8_t options,
    WindowDialogReplyCallback reply, WindowDialogFrameCallback frame,
    std::uint32_t flags, std::uint32_t mode);

// UTL02：原型与实例各自拥有动态UTF-16正文，复制使用宽字符串赋值。
struct WindowTextPrototype {
    std::array<std::byte, 4> bytes00; // 原成员偏移：+0x00。
    std::wstring text; // 原成员偏移：+0x04；UTL02拥有UTF-16原型正文。
    WindowTextPrototype() = default;
    WindowTextPrototype(std::array<std::byte, 4> bytes, std::wstring_view wide)
        : bytes00(bytes), text(wide) {}
};

// 原元素+18按type解释；type0/5/6当前原型值为0且471440不消费。
union WindowElementPrototypeSource {
    std::uint32_t unusedValue;
    WindowGraphicData* graphic;
    std::int32_t* animationImageNo;
    WindowTextPrototype* text;
    std::uint32_t* mouseFlags;
    WindowNumberData* number;
};

struct WindowElementPrototype {
    WindowElementType type; // 原成员偏移：+0x00。 原S32控件类型。
    std::int32_t x; // 原成员偏移：+0x04。
    std::int32_t y; // 原成员偏移：+0x08。
    std::int32_t width; // 原成员偏移：+0x0C。
    std::int32_t height; // 原成员偏移：+0x10。
    std::int32_t visible; // 原成员偏移：+0x14。
    WindowElementPrototypeSource source; // 原成员偏移：+0x18。
    WindowElementCallback mouseCallback; // 原成员偏移：+0x1C。
};

// 原48B可变原型的原生x64表示，36/37等同源原型须共用对象。
struct WindowPrototype {
    std::uint32_t state; // 原成员偏移：+0x00。
    std::int32_t layer; // 原成员偏移：+0x04。
    std::int32_t defaultX; // 原成员偏移：+0x08。
    std::int32_t defaultY; // 原成员偏移：+0x0C。
    std::int32_t width; // 原成员偏移：+0x10。
    std::int32_t height; // 原成员偏移：+0x14。
    std::uint32_t flags; // 原成员偏移：+0x18。
    std::int32_t elementCount; // 原成员偏移：+0x1C。
    WindowElementPrototype* elements; // 原成员偏移：+0x20。
    WindowCallback update; // 原成员偏移：+0x24。
    WindowCallback draw; // 原成员偏移：+0x28。
    WindowReleaseCallback release; // 原成员偏移：+0x2C。
};

// 原第二原型的内联矩形以借用T*表达；不复制共享表或原矩形数据。
struct WindowDragPrototype {
    std::int32_t count;
    WindowDragRect* rectangles;
};

// 原输入原型首W32的两种解释；不对原生T*进行原x86高字节测试。
enum class WindowInputReferenceKind : std::uint8_t {
    Context,
    RejectedValue,
};

struct WindowInputReference {
    union {
        input::InputContext* context;
        std::uint32_t rejectedValue; // 原高字节非零的拒绝值，低24位也保留。
    };
    WindowInputReferenceKind kind;
};

// 原57E970的0x22C输入原型，不同于窗口type1的图形原型。
struct WindowInputPrototype {
    WindowInputReference input; // 原成员偏移：+0x00。
    std::int32_t x; // 原成员偏移：+0x04。
    std::int32_t y; // 原成员偏移：+0x08。
    std::int32_t renderMode; // 原成员偏移：+0x0C。
    std::int32_t fontType; // 原成员偏移：+0x10。
    std::int32_t color; // 原成员偏移：+0x14。
    std::wstring text; // 原+18文本；UTL02动态UTF-16输入原型正文。
    std::int32_t maxLine; // 原成员偏移：+0x21C。
    std::int32_t maxCharPerLine; // 原成员偏移：+0x220。
    std::int32_t value224; // 原成员偏移：+0x224。
    std::int32_t encrypt; // 原成员偏移：+0x228。
};
extern WindowInputPrototype gDialogInputPrototype57E478; // 原全局：0x0057E478。 同源可写输入原型，57E698为容量成员。
extern WindowInputPrototype gWindow50InputPrototype580618; // 原全局：0x00580618。 借Input唯一CC2080，供更新与编辑交互。
extern WindowInputPrototype gTitleInputPrototype57CCB8; // 原全局：0x0057CCB8。 原型当前context为编辑正文来源。
extern WindowInputPrototype gPetNameInputPrototype57DCF8; // 原全局：0x0057DCF8。 窗口15与16/17/18共用名称输入原型。
extern WindowInputPrototype gMailInputPrototype57E740; // 原全局：0x0057E740。 窗口22/23共用，借Input唯一D66CA8。
extern std::array<std::wstring, 10> gPetTechniqueCostsD66C34; // 原全局：0x00D66C34。 动态宽文本，控件借稳定owner。
// 原函数：0x0048C1E0。
std::int32_t WindowPetList_HandleButtons(std::int32_t index, std::int32_t state); // 窗口15完整五组交互。
// 原函数：0x0048D4E0。
std::int32_t WindowPetList_SubmitStates(); // 五状态局部快照叠加真实跟随位，透传发送结果。
// 原函数：0x0048ECA0。
std::int32_t WindowPetSkills_HandleButtons(std::int32_t index, std::int32_t state); // 宠物技能页六按钮。
// 原函数：0x0048DD80。
std::int32_t WindowPetDetail_HandleButtons(std::int32_t index, std::int32_t state); // 宠物详情名称/放地/选择/旋转。
// 原函数：0x0048E430。
std::int32_t WindowPetAttributes_HandleButtons(std::int32_t index, std::int32_t state); // 宠物详细属性九按钮，9..13发送加点。
// 原函数：0x00492430。
std::int32_t WindowAlbumList_SelectEntry(std::int32_t index, std::int32_t state); // 图鉴列表打开选中详情。
// 原函数：0x00492520。
std::int32_t WindowAlbumList_HandleScrollButtons(std::int32_t index, std::int32_t state); // 单行/十行滚动按钮。
// 原函数：0x00492800。
std::int32_t WindowAlbumList_DragScroll(std::int32_t index, std::int32_t state); // WSP01：原950参数拖动换算；旧X代码1961。
// 原函数：0x00492840。
std::int32_t WindowAlbumList_Scroll(std::int32_t index, std::int32_t state); // 图鉴滚轮，恒返0。
// 原函数：0x00492880。
std::int32_t WindowAlbumDetail_HandleNavigation(std::int32_t index, std::int32_t state); // 图鉴详情四种翻页与旋转。
// 原函数：0x00493A70。
void WindowSettings_RefreshLogoutPanel(std::int32_t offset); // 设置窗口登出侧栏裁剪与两按钮显隐。
// 原函数：0x00493AF0。
void WindowSettings_RefreshInputPanel(std::int32_t offset); // 设置窗口输入侧栏裁剪与三按钮显隐。
// 原函数：0x00493B70。
void WindowSettings_RefreshAudioPanel(std::int32_t offset); // 设置窗口音量侧栏，两同源数字分别测宽。
// 原函数：0x00493CB0。
void WindowSettings_RefreshDisplayPanel(std::int32_t offset); // 设置窗口显示侧栏，读取当前模式S8。
// 原函数：0x00493D60。
std::int32_t WindowSettings_Update(std::int32_t value); // -1仅隐藏侧栏、清活动索引和位移。
// 原函数：0x00493DB0。
// 关联原入口：0x00493DF9（共享尾已并入本函数正文）。
std::int32_t WindowSettings_Draw(std::int32_t unusedValue); // 四侧栏减速展开与公共绘制尾。
// 原函数：0x00493E10。
void WindowSettings_Release(); // 设置写回/持久化后播放关闭音效。
// 原函数：0x00493EE0。
std::int32_t WindowSettings_ButtonLogoutPanel(std::int32_t index, std::int32_t state); // 登出侧栏开关。
// 原函数：0x00494000。
std::int32_t WindowSettings_ButtonInputPanel(std::int32_t index, std::int32_t state); // 输入侧栏，展开后保留元素11载荷别名。
// 原函数：0x00494180。
std::int32_t WindowSettings_ButtonAudioPanel(std::int32_t index, std::int32_t state); // 音量侧栏，生成两个type7同源文本。
// 原函数：0x00494370。
std::int32_t WindowSettings_ButtonDisplayPanel(std::int32_t index, std::int32_t state); // 显示侧栏，展开后保留元素21载荷别名。
// 原函数：0x00493E50。
std::int32_t WindowSettings_ButtonClose(std::int32_t index, std::int32_t state); // 设置窗关闭标记，无直接音效。
// 原函数：0x004944A0。
std::int32_t WindowSettings_ButtonHotkeys(std::int32_t index, std::int32_t state); // 快捷键窗口25开关。
// 原函数：0x00494550。
std::int32_t WindowSettings_ButtonLogout(std::int32_t index, std::int32_t state); // lO请求与场景10状态7。
// 原函数：0x00494620。
std::int32_t WindowSettings_ButtonServerSelect(std::int32_t index, std::int32_t state); // 返回服务器选择，非场景10构造CryptInt零值。
// 原函数：0x00494720。
std::int32_t WindowSettings_ToggleItemStacking(std::int32_t index, std::int32_t state); // 物品堆叠输入设置。
// 原函数：0x004947F0。
std::int32_t WindowSettings_ToggleCursor(std::int32_t index, std::int32_t state); // 光标切换，非零模式逐帧预览。
// 原函数：0x004948F0。
std::int32_t WindowSettings_CycleHotkeyMode(std::int32_t index, std::int32_t state); // 快捷键模式，同源六项图号表。
// 原函数：0x004949C0。
std::int32_t WindowSettings_ButtonMusicDecrease(std::int32_t index, std::int32_t state); // 音乐减量，bit400触发。
// 原函数：0x00494AE0。
std::int32_t WindowSettings_ButtonMusicIncrease(std::int32_t index, std::int32_t state); // 音乐增量。
// 原函数：0x00494C10。
std::int32_t WindowSettings_ButtonSoundDecrease(std::int32_t index, std::int32_t state); // 音效减量，仍调用BGM音量入口。
// 原函数：0x00494D30。
std::int32_t WindowSettings_ButtonSoundIncrease(std::int32_t index, std::int32_t state); // 音效增量。
// 原函数：0x00494E60。
std::int32_t WindowSettings_ToggleChannels(std::int32_t index, std::int32_t state); // 声道设置，保留W32奇偶切换。
// 原函数：0x00494F30。
std::int32_t WindowSettings_ToggleAudioFlag(std::int32_t index, std::int32_t state); // 554644切换，提示仍复用声道文案。
// 原函数：0x00495000。
std::int32_t WindowSettings_ToggleScanlines(std::int32_t index, std::int32_t state); // 低U8切换后写W16扫描线设置。
// 原函数：0x004950D0。
std::int32_t WindowSettings_DrawRenderModeButton(std::int32_t index, std::int32_t state); // 当前渲染模式提示/图号，恒返0。
// 原函数：0x00495130。
std::int32_t WindowHotkeys_Update(std::int32_t value); // 窗口25初始化，按分类绑定十行回调。
// 原函数：0x004951C0。
std::int32_t WindowHotkeys_Draw(std::int32_t value); // 窗口25说明/预设文本绘制及编辑焦点收尾。
// 原函数：0x004953B0。
std::int32_t WindowHotkeys_ButtonClose(std::int32_t index, std::int32_t state); // 关闭并写回预设文本，不直接持久化。
// 原函数：0x00495450。
std::int32_t WindowHotkeys_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 上一行，bit400。
// 原函数：0x00495500。
std::int32_t WindowHotkeys_ButtonNextRow(std::int32_t index, std::int32_t state); // 下一行，音效后重新取计数。
// 原函数：0x004955C0。
std::int32_t WindowHotkeys_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 上十行，音效先于位置修改。
// 原函数：0x00495670。
std::int32_t WindowHotkeys_ButtonNextPage(std::int32_t index, std::int32_t state); // 下十行，保留三阶段计数读取。
// 原函数：0x00495750。
std::int32_t WindowHotkeys_Scroll(std::int32_t index, std::int32_t state); // 窗口25滚轮，恒返0。
// 原函数：0x004957B0。
std::int32_t WindowHotkeys_TabFirstCategory(std::int32_t index, std::int32_t state); // 分类0与十行复位。
// 原函数：0x004958D0。
std::int32_t WindowHotkeys_TabSecondCategory(std::int32_t index, std::int32_t state); // 分类1与十行复位。
// 原函数：0x004959F0。
std::int32_t WindowHotkeys_TabThirdCategory(std::int32_t index, std::int32_t state); // 分类2与十行复位。
// 原函数：0x00495B10。
std::int32_t WindowHotkeys_TabPresetText(std::int32_t index, std::int32_t state); // 分类3只换回调，不复位行图号。
// 原函数：0x00495C20。
std::int32_t WindowHotkeys_EditPresetText(std::int32_t index, std::int32_t state); // 预设文本备份、初始化及同源焦点。
extern std::int8_t gHotkeySelectedCC4B20; // 原全局：0x00CC4B20。 S8选中记录，495130初始化为-1。
extern WindowInputPrototype gHotkeyInputPrototype57DF48; // 原全局：0x0057DF48。 借24项InputContext中的当前编辑项。
extern std::int8_t gSettingsPanelCC4234; // 原全局：0x00CC4234。
extern std::int32_t gSettingsOffsetCA02DC; // 原全局：0x00CA02DC。
extern std::int32_t gSettingsStepD2D610; // 原全局：0x00D2D610。
extern std::wstring gSettingsMusicTextD3041C; // 原全局：0x00D3041C。 5689C0借用，音乐音量S32的%d文本。
extern std::wstring gSettingsEffectTextD2EA64; // 原全局：0x00D2EA64。 5689B8借用，音效音量S32的%d文本。
// 原函数：0x0048F380。
std::int32_t WindowPetDetail_EditName(std::int32_t index, std::int32_t state); // 共享名称输入焦点。
// 原函数：0x0048F410。
std::int32_t WindowPetDetail_SwitchPage(std::int32_t index, std::int32_t state); // 三详情页切换。
// 原函数：0x0048F6C0。
std::int32_t WindowPetSkills_HandleSkillRow(std::int32_t index, std::int32_t state); // 描述、技能换位与拖动。
extern std::wstring gTitleDraftD45FC0; // 原全局：0x00D45FC0。 窗口9草稿，区别于玩家ownTitle和原型内嵌文本。
extern std::int32_t& gTitleModeCF4E50; // 原全局：0x00CF4E50。 借既有登录窗口状态槽0，0选择/1删除。
extern std::int32_t& gTitlePageCF4E54; // 原全局：0x00CF4E54。 借既有登录窗口状态槽1，重置/刷新/滚动同源。
// 原函数：0x00477540。
std::int32_t WindowPlayerTitles_EditDraft(std::int32_t unusedIndex, std::int32_t state); // 草稿编辑焦点。
// 原函数：0x00477600。
std::int32_t WindowPlayerTitles_SelectTitle(std::int32_t index, std::int32_t state); // 六行称号选择/删除。
// 原函数：0x00477710。
std::int32_t WindowPlayerTitles_HandleButtons(std::int32_t index, std::int32_t state); // 完整按钮分派与背景清模式。
// 原函数：0x004782A0。
std::int32_t WindowPlayerTitles_Scroll(std::int32_t unusedIndex, std::int32_t state); // 称号滚轮，恒返0。
// 原函数：0x004782E0。
std::int32_t WindowPlayerTitles_DragScroll(std::int32_t index, std::int32_t state); // WSP01：原称号拖动页码；旧X代码1959。
extern WindowInputPrototype gOwnProfileInputPrototype57CEE8; // 原全局：0x0057CEE8。 窗口58三项/正文的可写输入原型。
extern WindowInputPrototype gOwnProfileInputPrototype57D118; // 原全局：0x0057D118。
extern WindowInputPrototype gOwnProfileInputPrototype57D348; // 原全局：0x0057D348。
extern WindowInputPrototype gOwnProfileInputPrototype57D578; // 原全局：0x0057D578。
// 原函数：0x004792D0。
std::int32_t WindowOwnProfile_EditField(std::int32_t index, std::int32_t state); // 窗口58元素5..8编辑焦点。

union WindowUserState {
    std::uint32_t value; // 原+18的数值路径。
    WindowValueCallback callback; // 原窗口42的+18，禁止截成W32。
};

struct WindowState {
    std::uint32_t flags; // 原+00，独立于实例state。
    std::int32_t x; // 原成员偏移：+0x04。
    std::int32_t y; // 原成员偏移：+0x08。
    WindowInstance* window; // 原+0C；不沿用原记录0x54步长。
    std::uint32_t value10; // 原成员偏移：+0x10。
    std::uint32_t value14; // 原成员偏移：+0x14。
    WindowUserState userState; // 原+18；后续生产者按实际用途激活成员。
    std::uint32_t actionFlags1C; // 原+1C，窗口42数值面板消费低U8。
    std::uint32_t value20; // 原成员偏移：+0x20。
    std::uint32_t value24; // 原成员偏移：+0x24。
    std::uint32_t value28; // 原成员偏移：+0x28。
    std::array<std::byte, 36> bytes2C; // 原成员偏移：+0x2C。 未消费区，不据此冻结未知指针映射。
    std::uint32_t marker50; // 原成员偏移：+0x50。
};

inline constexpr std::size_t kWindowCount = 71;
// 两表已按原地址共享绑定；主表的其余元素与回调依赖见WindowPrototypeDependencies。
extern std::array<WindowPrototype*, kWindowCount> gWindowPrototypes57C130; // 原全局：0x0057C130。
extern WindowPrototype& gWindowPrototype54E2A0; // 原全局：0x0054E2A0。 4C93A0直接比较的原对象，不追随表槽改指向。
extern WindowPrototype& gWindowPrototype54E240; // 原全局：0x0054E240。
extern WindowPrototype& gWindowPrototype54E330; // 原全局：0x0054E330。
extern std::array<WindowDragPrototype*, kWindowCount> gWindowDragPrototypes57C24C; // 原全局：0x0057C24C。
extern std::array<WindowInstance*, kWindowCount> gWindows; // 原C8B094。
extern std::array<WindowState, kWindowCount> gWindowStates; // 原C9A9B0。
extern WindowInstance* gCurrentWindow; // 原CF4694。
extern WindowState* gCurrentWindowState; // 原CF4684。
extern std::uint8_t gValueD67BE0; // 原全局：0x00D67BE0。 原多个UI生产/消费入口共用，不并入窗口记录。
extern std::wstring gStallMessageC8A978; // 原全局：0x00C8A978。 摆摊结果提示，4D0230消费同一文本。
extern std::array<char, 17> gStallNameD67BE4; // 原全局：0x00D67BE4。 原strncpy16及D67BF4终止字节。
extern std::uint8_t gStallPendingD67C31; // 原全局：0x00D67C31。 请求与回复共用等待标记。
extern std::int32_t gStallMessageModeD67C34; // 原全局：0x00D67C34。
extern std::int32_t gStallMessageValueD67C38; // 原全局：0x00D67C38。
// 原函数：0x004CE700。
void WindowStall_ReceiveOpenResult(std::int32_t result); // 开摊结果：玩家标志、名字及窗口67首元素。
// 原函数：0x004CE8B0。
void WindowStall_ReceiveCloseResult(std::int32_t result); // 收摊结果：两路均先清玩家标志/等待/门禁。
// 原函数：0x004CE930。
void WindowStall_ReceivePurchaseResult(std::int32_t result); // 购买结果：浏览记录和两并行表原序搬移，或显示原提示。
// 原函数：0x004CF8A0。
void WindowStall_BuildItemDescription(world::detail::PlayerItemRecord* item, const char* text); // 固定32行描述及前一行回扫。
// 原函数：0x004CECB0。
std::int32_t WindowStall_RequestPurchase(); // 选择索引直接查询后发购买请求，返回宽提示长度（含NUL）或原负值。
// 原函数：0x004CD420。
std::int32_t WindowStall_ButtonClose(std::int32_t index, std::int32_t state); // 两等待门禁、关闭标记和原图形快照。
// 原函数：0x004CDE00。
void WindowStall_Release(); // 67/68共用释放，条件发送后无条件SE57。
// 原函数：0x004CDB10。
std::int32_t WindowStall_Draw(std::int32_t unusedValue); // 两类摊位预览、失效上架剔除与提示绘制。
// 原函数：0x004D27C0。
std::int32_t WindowStall_AddListing(std::int32_t value); // 当前选择追加到自身上架表，先清选择最后发布数量。
// 原函数：0x004CD4F0。
std::int32_t WindowStall_Update(std::int32_t value); // 共用初始化、双滚动/输入同步、文本绘制及金额尾。
// 原函数：0x004D2080。
std::int32_t WindowStall_EditDescription(std::int32_t unusedIndex, std::int32_t state); // 固定元素8说明编辑，hover不消费。
// 原函数：0x004D2170。
std::int32_t WindowStall_EditName(std::int32_t unusedIndex, std::int32_t state); // 固定元素6名称编辑，同源Input对象。
extern std::wstring gStallMoneyText57B440; // 原全局：0x0057B440。 原可变金额串，止于下一数字原型57B450。
// 原函数：0x004CDE40。
std::int32_t WindowStall_ButtonPreviousListing(std::int32_t index, std::int32_t state); // 上架列表向前，bit400动作。
// 原函数：0x004CDFA0。
std::int32_t WindowStall_ButtonNextListing(std::int32_t index, std::int32_t state); // 上架列表向后，音效后重读滚动参数。
// 原函数：0x004CE0F0。
std::int32_t WindowStall_ScrollListings(std::int32_t unusedIndex, std::int32_t state); // 上架列表滚轮，始终返回0。
// 原函数：0x004CE1F0。
std::int32_t WindowStall_ButtonPreviousPet(std::int32_t index, std::int32_t state); // 宠物候选向前，非宠物模式仍可消费bit400。
// 原函数：0x004CE2B0。
std::int32_t WindowStall_ButtonNextPet(std::int32_t index, std::int32_t state); // 宠物候选向后，最大页1。
// 原函数：0x004CE370。
std::int32_t WindowStall_ScrollPetCandidates(std::int32_t unusedIndex, std::int32_t state); // 宠物候选滚轮，始终返回0。
// 原函数：0x004CE3F0。
std::int32_t WindowStall_TabCategory(std::int32_t index, std::int32_t state); // 类别页签及七项候选控件原序显隐。
// 原函数：0x004CFE70。
std::int32_t WindowStall_SubmitListings(); // 槽序上架组包、转义发送及同源保存记录回写，返回0。
// 原函数：0x004D0230。
std::int32_t WindowStall_DrawTooltipOverlay(std::int32_t index, std::int32_t state); // 共用提示覆盖层，只返回hover位。
// 原函数：0x004CF940。
std::int32_t WindowStall_HandleButtons(std::int32_t index, std::int32_t state); // 自身/浏览主要按钮，禁用与普通收尾分离。
// 原函数：0x004CE8A0。
std::int32_t WindowStall_RequestClose(); // 收摊请求，透传原发送返回。
// 原函数：0x004CED50。
WindowInstance* WindowStall_ShowPurchaseConfirmation(); // 显示购买确认，返回入口窗口。
// 原函数：0x004CED60。
void WindowStall_OpenBrowse(
    std::int32_t status, std::int32_t target, const char* stallName,
    const char* description, const char* itemPayload, const char* petPayload); // 浏览区完整生产后打开68。
// 原函数：0x004CFC40。
WindowElement* WindowStall_EditItemPrice(); // 物品定价输入初始化，返回末次取得的元素表。
// 原函数：0x004CFCA0。
void WindowStall_EditPetPrice(std::int32_t petIndex); // 固定六装备槽检查后进入宠物定价。
// 原函数：0x004CFD70。
std::int32_t WindowStall_RemoveListing(); // 删除上架项，保持标记/数量/搬移/选择与页回退顺序。
// 原函数：0x004D2260。
std::int32_t WindowStall_HandlePriceInput(std::int32_t unusedIndex, std::int32_t state); // 完整定价键盘和鼠标交互，返回hover位。
// 原函数：0x004D2840。
std::int32_t WindowStall_HandlePurchaseConfirmation(std::int32_t unusedIndex, std::int32_t state); // 购买确认及物品/宠物详情，二次读模式。
// 原函数：0x004D1D50。
std::int32_t WindowStall_HandleInventory(std::int32_t index, std::int32_t state); // 玩家背包20格，首命中交互但完整绘制。
// 原函数：0x004D04A0。
std::int32_t WindowStall_HandleItemRow(std::int32_t index, std::int32_t state); // 上架/浏览物品行，固定记录与价格重读分离。
// 原函数：0x004D16F0。
std::int32_t WindowStall_HandlePetCandidate(std::int32_t index, std::int32_t state); // 宠物候选，逐项重读候选页并保持CryptInt顺序。
// 原函数：0x004D1C00。
std::int32_t WindowStall_DrawCandidateAttributes(
    std::int32_t petIndex, std::int32_t attribute, std::int32_t x, std::int32_t y); // 候选属性条，透传末绘制返回。
// 原函数：0x004D0990。
std::int32_t WindowStall_DrawPetSkills(
    std::int32_t x, std::int32_t y, std::int32_t petIndex, std::int32_t skillPosition); // 浏览宠物技能，使用独立浏览记录。
// 原函数：0x004D0A80。
std::int32_t WindowStall_DrawPetDetails(
    std::int32_t x, std::int32_t y, std::int32_t index); // 完整浏览宠物详情，复用D67B84预览。
// 原函数：0x004D0EB0。
std::int32_t WindowStall_HandlePetRow(std::int32_t index, std::int32_t state); // 上架/浏览宠物行，末尾金额两次读取及详情分派。
extern std::int32_t gDragItemIndex584DA4; // 原全局：0x00584DA4。 4FB8F0重置与旧14的4FAEF0拖动生产共用。
extern std::int32_t gDragImageNoD67D24; // 原全局：0x00D67D24。
extern std::int32_t gItemDragXCC4880; // 原全局：0x00CC4880。
extern std::int32_t gItemDragYCC4884; // 原全局：0x00CC4884。
extern std::int32_t gDraggingCA0B28; // 原全局：0x00CA0B28。
extern std::int32_t gValue58589C; // 原全局：0x0058589C。 原初值1，命中描述门禁与物品拖动重置共享。
extern void* gFocusedElementDataD45F94; // 原全局：0x00D45F94。 原异型元素载荷地址，仅借用，不是InputContext。
extern std::int32_t gMouseValue57C110; // 原全局：0x0057C110。
extern std::int32_t gDragSourceIndex57C114; // 原全局：0x0057C114。
extern WindowDragCallback gDragCallback57C118; // 原全局：0x0057C118。
extern std::int16_t gDragX57C11C; // 原全局：0x0057C11C。
extern std::int16_t gDragY57C11E; // 原全局：0x0057C11E。
extern std::int32_t gFocusWindowId57C120; // 原全局：0x0057C120。
extern std::int32_t gFocusElementIndex57C124; // 原全局：0x0057C124。
extern std::int32_t gMousePhase57C128; // 原全局：0x0057C128。
extern std::int32_t gCurrentElementIndex57C12C; // 原全局：0x0057C12C。 原映像初值-1，不等同焦点元素。
extern std::int32_t gCurrentWindowIdD67A1C; // 原全局：0x00D67A1C。
extern std::uint32_t gWindowHasFocusCA0AB4; // 原全局：0x00CA0AB4。 元素回调成功后占用，独立于焦点窗口id。
extern WindowInputPrototype gChatInputPrototype57E970; // 原全局：0x0057E970。
extern WindowInputPrototype gTradeInputPrototype5803D0; // 原全局：0x005803D0。 借用input唯一CC21E8上下文。
extern std::int32_t gChatWindowModeD66E04; // 原全局：0x00D66E04。 原独立S32，窗口35/57选择与设置文件共用。
extern std::int32_t gLogDisplayGate593F84; // 原全局：0x00593F84。 窗口66与旧402020/4021A0共用，非恒1。
extern std::uint8_t gValue57DA28; // 原全局：0x0057DA28。 原独立U8，初值FF，设置文件与物品窗口共用。
extern std::int32_t gInventoryHover57DA2C; // 原全局：0x0057DA2C。 普通背包悬停槽，初值-2，仅488FB0读写。
extern std::int32_t gEquipmentHover57DA68; // 原全局：0x0057DA68。 装备页独立悬停槽，初值-2，仅489760读写。
extern std::wstring gInventoryMoneyTextCB086C; // 原全局：0x00CB086C。 488E60格式化，type7载荷借用。
extern std::wstring gInventoryValueTextCC3ECC; // 原全局：0x00CC3ECC。 488E80格式化FF02CC，独立于输入区。
extern std::uint8_t gValue57E42C; // 原全局：0x0057E42C。 原独立U8，初值FF，设置文件与宠物窗口共用。
extern std::int32_t gBattleItemHoverCF49B0; // 原全局：0x00CF49B0。 窗口27每帧清-1，499C30发布提示槽。
extern std::int32_t gBattleItemPageCB01E0; // 原全局：0x00CB01E0。 窗口27物品提示页，同源于两战斗交互。
extern std::uint8_t gBattleItemTargetFlagCC4D40; // 原全局：0x00CC4D40。 499C30发布，496B90/497770后续消费。
extern std::wstring gBattleMoneyTextCF469C; // 原全局：0x00CF469C。 499B00格式化，type7原型借用。
extern std::wstring gBattleValueTextC8A960; // 原全局：0x00C8A960。 499B20格式化，独立于普通库存文本。
extern std::int32_t gValueCF4AAC; // 原全局：0x00CF4AAC。 原设置文件回填与多个UI入口共用的数值。
extern std::int32_t gValueCF4690; // 原全局：0x00CF4690。
extern std::int32_t gValueCA0440; // 原全局：0x00CA0440。
extern std::int32_t gValueD67B38; // 原全局：0x00D67B38。
extern login::YobiAction* gWindowActionCA0174; // 原全局：0x00CA0174。 原475AA0返回的借用对象，不是W32标志。
extern login::YobiAction* gWindowActionD2E928; // 原全局：0x00D2E928。 另一窗口动画引用，479610仅清槽。
extern login::YobiAction* gPreviewActionD67B84; // 原全局：0x00D67B84。 原404AE0生产，重开只清引用不释放。
extern std::int32_t gValueD67AF8; // 原全局：0x00D67AF8。 原窗口操作计数，不随指针扩宽。
extern std::uint8_t gValueD67AFC; // 原全局：0x00D67AFC。
extern std::uint8_t gValueD67AFD; // 原全局：0x00D67AFD。
extern std::uint8_t gValueD67AFE; // 原全局：0x00D67AFE。
extern std::uint8_t gValueD67B24; // 原全局：0x00D67B24。
extern std::uint8_t gValue5F9754; // 原全局：0x005F9754。 原多窗口/物品UI共享字节，观察进入可能清零。
extern std::int32_t gUsingSkillIndexD2F32C; // 原全局：0x00D2F32C。 技能请求索引，不与窗口当前选中槽合并。
extern std::int32_t gUsingSkillLevelCA0AC4; // 原全局：0x00CA0AC4。
extern std::int32_t gSkillTargetKindCF4A10; // 原全局：0x00CF4A10。
extern std::int32_t gSkillTargetCountCA0004; // 原全局：0x00CA0004。
extern std::int32_t gSkillTargetPageC8A974; // 原全局：0x00C8A974。
extern std::int32_t gSkillTargetPageCountCA0178; // 原全局：0x00CA0178。
extern std::int32_t gSkillSendModeC8A970; // 原全局：0x00C8A970。
extern std::int32_t gSkillSelectedTargetCC4B1C; // 原全局：0x00CC4B1C。
extern std::int32_t gSkillWindowModeD672AC; // 原全局：0x00D672AC。
extern std::int32_t gSkillWindowIndexD2EA60; // 原全局：0x00D2EA60。
extern std::array<std::array<char, 17>, 10> gSkillTargetNamesCB0550; // 原全局：0x00CB0550。 独立原字节名单，非LY表。
extern std::array<std::int32_t, 10> gSkillTargetKindsD2F75C; // 原全局：0x00D2F75C。
extern std::array<const char*, 10> gSkillTargetNameViewsD45F98; // 原全局：0x00D45F98。
extern std::array<std::uint8_t, 10> gSkillTargetColorsCB1DE4; // 原全局：0x00CB1DE4。
extern std::uint8_t gSkillTargetPendingC8AF5F; // 原全局：0x00C8AF5F。
extern std::array<char, 20> gSkillTargetNameD2F748; // 原全局：0x00D2F748。 选择发送前复制；后续属性/技能消费者共享。
extern std::int32_t gChatWidthUnitsC9A930; // 原全局：0x00C9A930。 原聊天宽度单位，输入滚动/聊天绘制共用。
extern std::int32_t gChatWidthRequestC9A934; // 原全局：0x00C9A934。 原宽度调整请求，独立于实际宽度。
extern std::int32_t gChatHeightUnitsC9A938; // 原全局：0x00C9A938。 原聊天可见高度的字体9行高倍数。
extern std::int32_t gChatHeightRequestC9A93C; // 原全局：0x00C9A93C。 原高度调整请求，消费后清零。
extern std::int32_t gChatScrollOffsetC9A940; // 原全局：0x00C9A940。 原相对共享写指针的回看条数。

// 交易UI唯一原字节区，非玩家背包/宠物owner；未知字段不冻结语义。
extern std::array<char, 2 * 20 * 0xC4C> gTradeItemsD460D0; // 原全局：0x00D460D0。
extern std::array<char, 2 * 5 * 0x5FC> gTradePetsC9C108; // 原全局：0x00C9C108。
extern std::array<char, 5 * 0x600> gPetStorageD64E18; // 原全局：0x00D64E18。 外部存放宠物，4A84A0原回复区。
extern std::array<char, 2 * 0xFBF8> gItemDetailStorageCC4D58; // 原全局：0x00CC4D58。 每组首20条C44物品、末W32金额，间隔未冻结。
std::int32_t WindowBank_ReadSide() noexcept; // 旧72借读既有存放模式，不复制私有状态。
const std::byte* WindowMessageBoard_BorrowRecords() noexcept; // 旧73只读借现代50同源2C0H字节区，四条B0H。
extern std::array<char, 20 * 0xC58> gShopItemsCF4FE0; // 原全局：0x00CF4FE0。 买卖共用原字节记录，字段见窗口53 SPEC。
extern std::int32_t gShopItemCountCB1E10; // 原全局：0x00CB1E10。 买入实际数量/出售固定20，与回复商品数独立。
extern std::int32_t gShopReplyItemCountCF4B58; // 原全局：0x00CF4B58。 出售回复中实际读取的商品数。
extern std::int32_t gShopImageD2DAD0; // 原全局：0x00D2DAD0。
extern std::int32_t gValueD2D608; // 原全局：0x00D2D608。 商店和其他窗口共用数值，不是地址。
extern std::int32_t gShopDialogTypeCC3EC8; // 原全局：0x00CC3EC8。
extern std::int32_t gShopValueCF467C; // 原全局：0x00CF467C。
extern std::array<std::int8_t, 20> gShopInventoryMapD2DAB8; // 原全局：0x00D2DAB8。 背包8..27到商品首匹配索引。
extern std::array<char, 20> gShopSelectedSlotsCB03DC; // 原全局：0x00CB03DC。 原五次W32清-1，消费者按S8槽读取。
extern std::array<char, 32> gShopTextCB0184; // 原全局：0x00CB0184。 模式2字段7反转义结果，不套商品名回退。
extern std::array<char, 0xC38> gShopDetailCC3210; // 原全局：0x00CC3210。 名称/32行说明/六W32同源，见窗口53 SPEC。
extern std::int32_t gShopValueCAFEA4; // 原全局：0x00CAFEA4。
extern std::int32_t gShopValueD046C4; // 原全局：0x00D046C4。
extern std::int32_t gShopValueD672D4; // 原全局：0x00D672D4。
extern std::int32_t gShopTooltipIndex580844; // 原全局：0x00580844。 玩家/商品/详情提示编号，原装载初值-1。
extern std::int32_t gShopMessage580848; // 原全局：0x00580848。 四行正文组的负编号，原装载初值-1。
extern std::int32_t gShopInventoryHover58084C; // 原全局：0x0058084C。 原装载值-2，父初始化不重置的背包命中缓存。
extern std::int32_t gShopTooltipPageD67B8C; // 原全局：0x00D67B8C。
extern std::int32_t gShopTotalD2DAFC; // 原全局：0x00D2DAFC。
extern std::int32_t gShopValueC8AF74; // 原全局：0x00C8AF74。 各模式计算的数量值，不与总金额合并。
extern std::int32_t gShopScrollCC488C; // 原全局：0x00CC488C。
extern std::array<std::wstring, 5> gShopPriceTextsD673EC; // 原全局：0x00D673EC。 动态宽文本，控件借稳定owner。
extern std::array<std::wstring, 5> gShopCountTextsD2D988; // 原全局：0x00D2D988。 五行所选数量文本，与价格区分离。
extern std::wstring gShopTotalTextCC4B2C; // 原全局：0x00CC4B2C。 金额或模式相关数值的共用显示文本。
extern std::wstring gShopMoneyTextCC1A00; // 原全局：0x00CC1A00。 玩家金额文本，573D18 type7借用；旧尾页号CC1A0C已迁独立数值owner。
extern std::int32_t gShopAvailableCF4A24; // 原全局：0x00CF4A24。 模式2指定物品数量，装载0，窗口初始化再写-1。
extern std::wstring gShopValueTextCF4A28; // 原全局：0x00CF4A28。 模式2/3数值显示，573D20借用。
extern std::array<std::int32_t, 5> gShopRowKeysC9A954; // 原全局：0x00C9A954。 多窗口共用的行编号缓存，非地址。
extern std::uint8_t gShopDragFlagD67B90; // 原全局：0x00D67B90。 非零仅清自身，零分支移除选择；原虚拟零初值。
extern std::int32_t gQuantityLimit581C94; // 原全局：0x00581C94。 原装载值999，商店及其他数量窗口同源上限。
extern WindowValueCallback gConfirmationCallbackC8B088; // 原全局：0x00C8B088。 原生借用函数指针，确认/取消各自重读。
extern std::array<WindowConfirmationItem, 20> gConfirmationItemsCB1E40; // 原全局：0x00CB1E40。
extern std::array<WindowConfirmationAmount, 2> gConfirmationAmountsCB0F90; // 原全局：0x00CB0F90。
extern std::wstring gConfirmationLineCB1320; // 原全局：0x00CB1320。
extern std::wstring gConfirmationLineCB1210; // 原全局：0x00CB1210。
extern std::wstring gConfirmationAcceptHintCC3FCC; // 原全局：0x00CC3FCC。 4C0F20也借用，不建窗口私有副本。
extern std::wstring gConfirmationCancelHintD45D70; // 原全局：0x00D45D70。
extern std::int32_t gWindow54DialogTypeCB2224; // 原全局：0x00CB2224。 创建快照581800，更新/释放共同读取。
extern std::int32_t gWindow54ImageCC18C4; // 原全局：0x00CC18C4。 协议字段1的图号，非地址。
extern std::int32_t gWindow54ValueCC2E68; // 原全局：0x00CC2E68。 4BD540字段6，4D6A60也消费。
extern std::int32_t gWindow54ValueCF4A3C; // 原全局：0x00CF4A3C。 4BD540字段7，4D6A60也消费。
extern std::int32_t gWindow54ValueCB01E4; // 原全局：0x00CB01E4。 创建尾清零的独立数值，不与CB01E8合并。
extern std::int32_t gWindow54ValueCB01E8; // 原全局：0x00CB01E8。
extern std::uint8_t gWindow54ModeC8AF64; // 原全局：0x00C8AF64。
extern std::int32_t gWindow54LifecycleD67434; // 原全局：0x00D67434。 值2阻止释放回调重置当前对话类型。
extern std::array<WindowDialogTextLine, 15> gWindow54NamesC8AAA0; // 原全局：0x00C8AAA0。 4BD680复制、4BF2E0消费。
extern std::array<std::int32_t, 15> gWindow54ValuesCB01A4; // 原全局：0x00CB01A4。 两表也被4D6EA0消费，保持同源。
extern std::array<std::int32_t, 15> gWindow54ValuesCB09D8; // 原全局：0x00CB09D8。
extern std::array<Window54SkillRecord, 16> gWindow54SkillsD2EAB8; // 原全局：0x00D2EAB8。 600H，到D2F0B8前结束。
extern std::int32_t gWindow54SkillScrollCC31F4; // 原全局：0x00CC31F4。
extern std::int32_t gWindow54MessageC9FCE0; // 原全局：0x00C9FCE0。 此地址也用于交易宠物区结束比较，不属宠物数据。
extern std::int32_t gWindow54HoverD2DAD8; // 原全局：0x00D2DAD8。
extern std::int32_t gWindow54SelectedD2DC04; // 原全局：0x00D2DC04。
extern std::wstring gWindow54AmountTextD45F78; // 原全局：0x00D45F78。 下一独立槽从D45F84起。
extern std::array<std::wstring, 10> gWindow54ValueTextsCB035C; // 原全局：0x00CB035C。 动态宽文本，控件借稳定owner。
extern std::array<std::wstring, 10> gWindow54ValueTextsC8B318; // 原全局：0x00C8B318。
extern std::array<std::wstring, 5> gWindow54SkillValueTextsCF4978; // 原全局：0x00CF4978。 动态宽文本，控件借稳定owner。
extern std::array<std::wstring, 5> gWindow54SkillPriceTextsCF493C; // 原全局：0x00CF493C。
extern std::array<Window54PetOption, 5> gWindow54PetOptionsD67448; // 原全局：0x00D67448。 4D7C20/4D8230也读，保持同源。
extern std::array<std::int8_t, 5> gWindow54PetMapCF4AB0; // 原全局：0x00CF4AB0。 4C0A50原CF4A6B基址含元素索引69的别名。
extern std::int32_t gWindow54TribeCA0B2C; // 原全局：0x00CA0B2C。
extern std::int32_t gWindow54PetChoiceC8AA90; // 原全局：0x00C8AA90。 已选协议候选，不是原生地址或玩家宠物槽。
extern std::int32_t gWindow54PetScrollCC31DC; // 原全局：0x00CC31DC。
extern std::array<Window54TechniqueSlot, 10> gWindow54TechniqueSlotsD66E38; // 原全局：0x00D66E38。 398H，其他窗口同源。
extern std::int32_t gWindow54ReplacementC8B08C; // 原全局：0x00C8B08C。 4C0BF0发布、4C0F20消费的技能槽索引。
extern std::array<std::int8_t, 15> gWindow54SkillMapCC4D04; // 原全局：0x00CC4D04。 技能索引对扣除前序空名后的协议映射。
extern std::wstring gWindow54MoneyTextC9A9A4; // 原全局：0x00C9A9A4。 止于C9A9B0窗口状态表之前。
extern std::array<WindowCrystalItem, 20> gWindow69ItemsCB2230; // 原全局：0x00CB2230。
extern std::array<WindowCrystalItem, 20> gWindow70ItemsD30428; // 原全局：0x00D30428。
extern std::int32_t gWindow69ValueD67B98; // 原全局：0x00D67B98。
extern std::uint8_t gWindow69FlagD2EA5B; // 原全局：0x00D2EA5B。
extern std::int32_t gWindow69ImageCB01F0; // 原全局：0x00CB01F0。
extern std::int32_t gWindow69DialogTypeD2D810; // 原全局：0x00D2D810。
extern std::int32_t gWindow70ValueD67BA4; // 原全局：0x00D67BA4。
extern std::uint8_t gWindow70FlagD673D8; // 原全局：0x00D673D8。
extern std::int32_t gWindow70ImageD2E43C; // 原全局：0x00D2E43C。
extern std::int32_t gWindow70ValueD6791C; // 原全局：0x00D6791C。 第十协议字段，仍S32。
extern std::int32_t gWindow70DialogTypeD45D6C; // 原全局：0x00D45D6C。
extern std::int32_t gWindow69ScrollD2D604; // 原全局：0x00D2D604。
extern std::int32_t gWindow70ScrollCC4B68; // 原全局：0x00CC4B68。
extern std::int32_t gWindow69DirtyD67B9C; // 原全局：0x00D67B9C。
extern std::int32_t gWindow70DirtyD67BA8; // 原全局：0x00D67BA8。
extern std::int32_t gWindow69CachedScroll580858; // 原全局：0x00580858。 原文件初值-1。
extern std::int32_t gWindow70CachedScroll580868; // 原全局：0x00580868。
extern std::int32_t gWindow69TotalD45E70; // 原全局：0x00D45E70。
extern std::int32_t gWindow70TotalCB0F74; // 原全局：0x00CB0F74。
extern std::int32_t gWindow69SlotsCA0AC8; // 原全局：0x00CA0AC8。
extern std::int32_t gWindow70SlotsCF4E44; // 原全局：0x00CF4E44。
extern std::array<std::wstring, 5> gWindow69PriceTextsD2D7CC; // 原全局：0x00D2D7CC。
extern std::array<std::wstring, 5> gWindow70PriceTextsCF49C4; // 原全局：0x00CF49C4。
extern std::array<std::wstring, 5> gWindow69CountTextsD2EA24; // 原全局：0x00D2EA24。
extern std::array<std::wstring, 5> gWindow70CountTextsCC4848; // 原全局：0x00CC4848。
extern std::wstring gWindow69TotalTextCC48B0; // 原全局：0x00CC48B0。
extern std::wstring gWindow70TotalTextD2D804; // 原全局：0x00D2D804。
extern std::int32_t gWindow69Tooltip580850; // 原全局：0x00580850。 悬停编号，原文件初值-1。
extern std::int32_t gWindow70Tooltip580860; // 原全局：0x00580860。
extern std::int32_t gWindow69Message580854; // 原全局：0x00580854。 NPC正文组，原文件初值-1。
extern std::int32_t gWindow70Message580864; // 原全局：0x00580864。
extern std::int32_t gWindow69TooltipPageD67B94; // 原全局：0x00D67B94。
extern std::int32_t gWindow70TooltipPageD67BA0; // 原全局：0x00D67BA0。
extern std::int32_t gWindow69InventoryHover58085C; // 原全局：0x0058085C。 原文件初值-2；背包命中变化才更新。
extern std::int32_t gWindow70InventoryHover58086C; // 原全局：0x0058086C。
extern std::wstring gWindow69BalanceTextD2E448; // 原全局：0x00D2E448。 完整S32格式化前缀，不冻结未消费尾区。
extern std::wstring gWindow70BalanceTextCF3F14; // 原全局：0x00CF3F14。
extern WindowValueCallback gWindow69ConfirmCallbackCF4E48; // 原全局：0x00CF4E48。
extern WindowValueCallback gWindow70ConfirmCallbackCF4AA4; // 原全局：0x00CF4AA4。
extern std::array<WindowConfirmationItem, 20> gWindow69ConfirmItemsD2E458; // 原全局：0x00D2E458。
extern std::array<WindowConfirmationItem, 20> gWindow70ConfirmItemsCB0AE8; // 原全局：0x00CB0AE8。
extern std::array<WindowConfirmationAmount, 2> gWindow69ConfirmAmountsD2E2B8; // 原全局：0x00D2E2B8。
extern std::array<WindowConfirmationAmount, 2> gWindow70ConfirmAmountsD2E290; // 原全局：0x00D2E290。
// 固定原文及NUL的已消费前缀，不据此冻结未消费尾区容量。
extern std::wstring gWindow69ConfirmHeadingCC3F68; // 原全局：0x00CC3F68。
extern std::wstring gWindow70ConfirmHeadingCF4A40; // 原全局：0x00CF4A40。
extern std::wstring gWindow69ConfirmQuestionCC3F04; // 原全局：0x00CC3F04。
extern std::wstring gWindow70ConfirmQuestionCF4ABC; // 原全局：0x00CF4ABC。
extern std::wstring gWindow69ConfirmAcceptD2E81C; // 原全局：0x00D2E81C。
extern std::wstring gWindow70ConfirmAcceptD2DD6C; // 原全局：0x00D2DD6C。
extern std::wstring gWindow69ConfirmCancelCB076C; // 原全局：0x00CB076C。
extern std::wstring gWindow70ConfirmCancelCC48BC; // 原全局：0x00CC48BC。
extern std::int32_t gStorageValueCC4218; // 原全局：0x00CC4218。 存放窗口41每帧清零、元素13绘制的数值。
extern std::array<std::wstring, 2> gStorageAmountTextsCB1DF8; // 原全局：0x00CB1DF8。 动态宽文本，控件借稳定owner。
extern const std::array<std::int32_t, 2> gStorageTitleImages54F4E8; // 原全局：0x0054F4E8。
extern const std::array<std::int32_t, 2> gStoragePanelImages54F4F0; // 原全局：0x0054F4F0。
extern std::int32_t gTradeValueCC4224; // 原全局：0x00CC4224。
extern std::int32_t gTradeValueCF4688; // 原全局：0x00CF4688。
extern std::int32_t gTradeValueD046D0; // 原全局：0x00D046D0。
extern std::int32_t gTradeValueD2E2B0; // 原全局：0x00D2E2B0。
extern std::uint8_t gTradeValueD67B80; // 原全局：0x00D67B80。
extern login::YobiAction* gTradeWindowActionD67B7C; // 原全局：0x00D67B7C。 原43窗口展开动画，仅借用。
extern std::int32_t gTradeValueCF4A1C; // 原全局：0x00CF4A1C。 原第一个页签数值索引。
extern std::int32_t gTradeValueD673E0; // 原全局：0x00D673E0。 原另一页签，均为数值索引。
extern std::int32_t gTradeValueCC4C94; // 原全局：0x00CC4C94。 原四个物品/宠物计数。
extern std::int32_t gTradeValueC8AF68; // 原全局：0x00C8AF68。
extern std::int32_t gTradeValueCB03CC; // 原全局：0x00CB03CC。
extern std::int32_t gTradeValueCF4AB8; // 原全局：0x00CF4AB8。
extern std::int32_t gTradeValueD67CB8; // 原全局：0x00D67CB8。 交易阶段相关数值，不是地址。
extern std::int32_t gTradeValueD67CBC; // 原全局：0x00D67CBC。
extern std::int32_t gTradeValueCC3EC4; // 原全局：0x00CC3EC4。 所选物品数值索引，4E5620写-1。
extern std::int32_t gTradeValueD66E08; // 原全局：0x00D66E08。 所选图号数值。
extern std::int32_t gTradeValueCC2344; // 原全局：0x00CC2344。 交易选择状态。
extern std::array<char, 17> gTradeNameD2D654; // 原全局：0x00D2D654。
extern std::array<std::array<char, 17>, 10> gWindowListNamesCF3F20; // 原全局：0x00CF3F20。
extern std::int32_t gWindowListCountCF46A8; // 原全局：0x00CF46A8。
extern WindowListSelectionCallback gWindowListCallbackCB1E14; // 原全局：0x00CB1E14。 49EF60借用名单文本并弃返回。
extern std::int32_t gWindowListOffsetCC3EF4; // 原全局：0x00CC3EF4。
extern std::wstring gHoverTextD68AE8; // 原全局：0x00D68AE8。 已识别地址跨度，不是新增文本长度限制。
extern std::wstring gStatusHpTextD40B18; // 原全局：0x00D40B18。 状态栏type7共用原文本跨度，非新截断门槛。
extern std::wstring gStatusFpTextD45D50; // 原全局：0x00D45D50。
extern std::int32_t gHoverObjectValueD68AC8; // 原全局：0x00D68AC8。 507140命中伴随值，4731E0后续须共享。
extern std::wstring gHoverTitleD68BF4; // 原全局：0x00D68BF4。 至既有鼠标D68CF8之前的原字节区。
extern std::int32_t gHoverTitleValueD69748; // 原全局：0x00D69748。 副标题伴随值，非主描述值。
extern std::int32_t gWindow6HoverD2EA70; // 原全局：0x00D2EA70。 473840的case0写U8(state)&1，4731E0严格判1。
extern std::int32_t gValueCC4D34; // 原全局：0x00CC4D34。 4D3020保存x/y数值，非CryptInt对象。
extern std::int32_t gValueCC4D3C; // 原全局：0x00CC4D3C。
extern std::int32_t gSelectedItemCB1DF4; // 原全局：0x00CB1DF4。
extern std::int32_t gSelectedItemCB0A44; // 原全局：0x00CB0A44。
extern std::int32_t gSelectedItem57EE0C; // 原全局：0x0057EE0C。 原装载初值-1，窗口51只做单向迁移。
extern std::uint8_t gSkillPairIndexD67AD8; // 原全局：0x00D67AD8。 配方查询按S8解释。
std::int32_t WindowSkill_ReadRequestFlag() noexcept; // 旧技能请求读取同源低S8，4FF650实际写者已接。
void WindowSkill_WriteRequestFlag(std::uint8_t value) noexcept; // 4FF650四原写点共用私有唯一D64E13。
std::uint32_t WindowSkill_ReadExtraDelay() noexcept; // 新旧技能面板共用额外时长。
void WindowSkill_WriteExtraDelay(std::uint32_t delay) noexcept;
std::int32_t WindowGather_ReadResult() noexcept; // 旧选择39借用窗口12同源回复。
std::int32_t WindowGather_ReadResultImage() noexcept;
std::wstring_view WindowGather_BorrowResultName() noexcept;
std::wstring_view WindowGather_BorrowResultSuffix() noexcept;
std::wstring_view WindowGather_BorrowResultDetails() noexcept; // 借用NUL文本，不复制到另一结果缓存。
std::int32_t WindowSkillResult_ReadResult() noexcept; // 旧选择37与窗口13共享回复结果。
std::wstring_view WindowSkillResult_BorrowSummary() noexcept;
std::wstring_view WindowSkillResult_BorrowDetails() noexcept;
std::int32_t WindowCraft_ReadItemWorkResult() noexcept;
std::wstring_view WindowCraft_BorrowItemWorkSuffix() noexcept;
std::wstring_view WindowCraft_BorrowItemWorkDetails() noexcept;
std::int32_t WindowCraft_ReadSkill() noexcept; // 新旧加工入口与结果回复同源技能类别。
void WindowCraft_WriteSkill(std::int32_t skillId) noexcept;
std::int32_t WindowCraft_ReadResult() noexcept;
std::int32_t WindowCraft_ReadResultImage() noexcept;
std::wstring_view WindowCraft_BorrowResultName() noexcept;
void WindowCraft_WriteResultName(std::wstring_view name); // 旧26发送前复制结果名称；原0x00CF46D4现持动态UTF-16。
std::wstring_view WindowCraft_BorrowResultSuffix() noexcept;
std::wstring_view WindowCraft_BorrowResultDetails() noexcept;
extern std::int32_t gRecipeIndex5846BC; // 原全局：0x005846BC。 原装载初值-1。
extern std::array<std::uint8_t, 20> gTradeItemSlotsCF3AE8; // 原全局：0x00CF3AE8。 比较按S8，写入只取低U8。
extern std::array<std::uint8_t, 28> gTradeItemMarksD2D61C; // 原全局：0x00D2D61C。
extern std::array<std::uint8_t, 5> gTradePetSlotsCC4D28; // 原全局：0x00CC4D28。 末项即CC4D2C，按S8读槽号。
extern std::array<std::uint8_t, 5> gTradePetMarksCF3AFC; // 原全局：0x00CF3AFC。 末项即CF3B00，不拆独立标量。
extern std::uint8_t gTradePhaseCAFEC4; // 原全局：0x00CAFEC4。
extern std::uint8_t gTradeStepD3040C; // 原全局：0x00D3040C。 位移按S8读，递减只写U8。
extern std::uint8_t gTradeValueD2EA20; // 原全局：0x00D2EA20。
extern std::uint8_t gTradeValueD6742C; // 原全局：0x00D6742C。
extern std::uint8_t gTradeValueC8AF81; // 原全局：0x00C8AF81。
extern std::uint8_t gTradeTabCC4D1A; // 原全局：0x00CC4D1A。 对方页签，不套用己方窗口初始化重置。
extern std::int32_t gTradeHoverSlotC8B314; // 原全局：0x00C8B314。 对方提示槽缓存，非地址。
extern std::int32_t gTradeHoverSlotC8AA78; // 原全局：0x00C8AA78。 己方已选物品提示槽缓存。
extern std::int32_t gTradeHoverSlotCB0A4C; // 原全局：0x00CB0A4C。 己方自有物品提示槽缓存。
extern std::int32_t gPetDragModeD2EA1C; // 原全局：0x00D2EA1C。 48C130发布，回调分别采点击和绘制模式。
extern std::int32_t gItemDragModeCC4C9C; // 原全局：0x00CC4C9C。 48BF10发布，仅门禁48BC90的message2请求。
extern std::int32_t gItemMoveSourceCF46B0; // 原全局：0x00CF46B0。 4886E0发布，4885A0读取的来源槽号。
extern std::int32_t gItemMoveTargetD2DBF8; // 原全局：0x00D2DBF8。 4886E0发布，4885A0读取的目标槽号。
extern std::int32_t gValueD67B00; // 原全局：0x00D67B00。 原数值状态，47D490清零；非地址。
extern std::wstring gNumberWindowTextCC4D1C; // 原CC4D1C至宠物槽CC4D28前的文本区。
extern std::int32_t gTradeScrollCF3B0C; // 原全局：0x00CF3B0C。
extern std::int32_t gTradeScrollCC4C90; // 原全局：0x00CC4C90。
extern std::int32_t gTradeScrollD67430; // 原全局：0x00D67430。
extern std::int32_t gTradePreviewCC2E64; // 原全局：0x00CC2E64。 原预览分类数值，非地址。
extern std::uint8_t gTradeTooltipPageCB10D4; // 原全局：0x00CB10D4。 物品提示页，绘制按S8读取。
extern std::uint32_t gWindowActionTick62B5AC; // 原全局：0x0062B5AC。 跨UI动作共用，不是交易私有计时槽。
extern std::int32_t gTooltipColorD67D30; // 原全局：0x00D67D30。 分段文本跨行共享颜色，尾串取低S8。
extern std::int32_t gTooltipSourceD303FC; // 原全局：0x00D303FC。 原提示源分类/编号缓存，非地址。
extern std::uint16_t gTooltipXCF46C0; // 原全局：0x00CF46C0。 同源低W16坐标，绘制按S16解释。
extern std::uint16_t gTooltipYCF46CC; // 原全局：0x00CF46CC。
extern const std::array<std::array<std::int32_t, 4>, 4> gPetBarImages57E434; // 原全局：0x0057E434。 left/middle/right/single。
extern const std::array<std::array<std::int32_t, 3>, 14> gNumericKeyImages54F030; // 原全局：0x0054F030。
extern const std::array<const wchar_t*, 6> gTradeNumberHints5805FC; // 原全局：0x005805FC。 原字节提示的借用指针表。
extern const std::array<const wchar_t*, 20> gTribeNames57CB54; // 原全局：0x0057CB54。 后八项共用585C50原字节串。
const std::byte* WindowDialog_BorrowHealingRecords() noexcept; // 借既有七条32B治疗记录，不扩到D673D0。
// 原LY十条20B记录及分立S32状态的非拥有视图；只借页号/选择写权，不复制名单。
struct LegacyItemUseTargetView {
    const std::int32_t* count; // CC40CC。
    std::int32_t* page; // CC46DC。
    const std::int32_t* pageCount; // CC3E70。
    std::int32_t* selected; // CB0A48。
    const std::byte* records; // CB0EA8，kind S16/+2名称，步长20。
};
LegacyItemUseTargetView WindowItemUse_BorrowTargetView() noexcept;

// 原ND六行分立列的只读非拥有视图；count必须在原循环边界重读，不复制成新记录。
struct LegacySkillStatusView {
    const std::int32_t* count; // C8AA94。
    const std::array<char, 17>* names; // CC4670。
    const std::int32_t* levels; // CC31C0。
    const std::int32_t* injuries; // D67600。
    const std::int32_t* hp; // CC318C。
    const std::int32_t* maxHp; // D66C1C。
    const std::int32_t* fp; // CC2348。
    const std::int32_t* maxFp; // D2EA80。
    const std::int32_t* kinds; // CA0A78。
    const std::int32_t* targets; // CF3FD0。
};
LegacySkillStatusView WindowSkill_BorrowStatusView() noexcept; // 借4EF750已有唯一owner，地址拓宽，数值仍S32。

std::int32_t WindowPet_ReadHealthImage(std::int32_t health); // 复用519680分级与57CBA4图号表。
extern std::array<std::int32_t, 6> gSelectedSlotsCA0A90; // 原全局：0x00CA0A90。
extern std::array<std::int32_t, 6> gMaterialIndicesCA0458; // 原全局：0x00CA0458。
extern std::array<std::int32_t, 6> gSelectionCountsD66E0C; // 原全局：0x00D66E0C。
extern std::array<std::int32_t, 6> gSelectionImagesCF3FF4; // 原全局：0x00CF3FF4。
extern std::array<char, 24> gMaterialOccupancyCC4894; // 原全局：0x00CC4894。 前五W32占用，末4B与CC48A8同源。
extern std::int32_t gSelectedItemD460C0; // 原全局：0x00D460C0。
extern std::int32_t gSelectedImageCC3ED8; // 原全局：0x00CC3ED8。
extern std::int32_t gSelectedCountCC483C; // 原全局：0x00CC483C。
extern std::uint8_t gSelectionModeD2D650; // 原全局：0x00D2D650。
extern std::array<std::uint8_t, 28> gItemMarksD67290; // 原全局：0x00D67290。
extern std::array<std::uint8_t, 20> gItemMarksD68A90; // 原全局：0x00D68A90。 不为原越界访问扩展为28B。

// 原函数：0x004E5710。
std::int32_t WindowTrade_ResetRemoteItems(std::int32_t side); // 先清侧状态，再清物品/宠物整组。
// 原函数：0x004E52B0。
void WindowTrade_CopyOwnPet(std::int32_t side); // 同源玩家宠物属性及十项技能按存储索引复制。
// 原函数：0x004E54E0。
void WindowTrade_CopyOwnItem(std::int32_t side, std::int32_t index); // 背包index+8到交易，不复制描述。
// 原函数：0x004E5600。
void WindowTrade_SyncOwnItems(std::int32_t side); // 顺序同步20条交易物品。
// 原函数：0x004D3020。
std::int32_t WindowProfile_SaveOrigin(std::int32_t x, std::int32_t y); // 同源坐标依序保存，返回x。
// 原函数：0x004D3040。
std::int32_t WindowProfile_CheckDistance(std::int32_t rangeX, std::int32_t rangeY); // 到格才判断距快照的范围。
// 原函数：0x004CAF60。
// 关联原入口：0x004CAF88（共享尾已并入本函数正文）。
std::int32_t WindowProfileView_Update(std::int32_t unusedValue); // 窗口62更新，标关闭后仍刷新全部资料。
// 原函数：0x004CAFA0。
std::uint32_t WindowProfileView_Refresh(); // 四图号、八资料文本和四分行，返回末行含NUL字节数。
// 原函数：0x004CB530。
void WindowProfileView_Release(); // 先清对话类型，再标记关联窗口63。
// 原函数：0x004CB9B0。
WindowInstance* WindowProfileView_RequestCategoryClose(std::int32_t index); // 借用窗口59+index，只标记关闭并返回原T*。
// 原函数：0x004CB2F0。
std::int32_t WindowProfileView_ButtonMail(std::int32_t index, std::int32_t state); // 查看资料发信，先金额门禁再打开63。
// 原函数：0x004CB550。
std::int32_t WindowMiniMail_Update(std::int32_t value); // 窗口63首次按目标决定保留输入，再每帧刷新。
// 原函数：0x004CB5D0。
void WindowMiniMail_Refresh(); // 同源收件人与八位费用刷新，原混合返回无人消费。
// 原函数：0x004CB6A0。
std::int32_t WindowMiniMail_Send(std::int32_t index, std::int32_t state); // 双层转义发送、插入本地迷你邮件及完整关闭尾。
// 原函数：0x004CB920。
std::int32_t WindowMiniMail_HandleInput(std::int32_t unusedIndex, std::int32_t state); // 固定元素1输入焦点与非活动绘制。
// 原函数：0x00478330。
std::int32_t WindowProfile_HandleButtons(std::int32_t index, std::int32_t state); // 资料公开/切窗/三目录/确认的完整八分支。
extern std::int32_t gProfileCategory1085DA0; // 原全局：0x01085DA0。 478330生产，窗口59按0/1/2选择类别，非地址。
extern std::int32_t gProfileScrollCA0444; // 原全局：0x00CA0444。 窗口59六入口共用的S32页码。
// 原函数：0x004C94F0。
std::int32_t WindowOwnProfileCategory_SelectCategory(std::int32_t index, std::int32_t state); // 十行分类选择，写当前资料。
// 原函数：0x004C96F0。
std::int32_t WindowOwnProfileCategory_HandleScrollButtons(std::int32_t index, std::int32_t state); // 窗口59四滚动/翻页按钮。
// 原函数：0x004C9B80。
std::int32_t WindowOwnProfileCategory_DragScroll(std::int32_t index, std::int32_t state); // WSP01：原分类count-10拖动换算；旧X代码1992。
// 原函数：0x004C9BE0。
std::int32_t WindowOwnProfileCategory_Scroll(std::int32_t unusedIndex, std::int32_t state); // 三模式滚轮，恒返0。
// 原函数：0x004CB9E0。
std::int32_t WindowProfileReply_Update(std::int32_t unusedValue); // 窗口64每帧完整刷新，返回1。
// 原函数：0x004CB9F0。
std::uint32_t WindowProfileReply_Refresh(); // 图号转换、八文本及256B四行提取，返回末行含NUL长度。
// 原函数：0x004C9EE0。
// 关联原入口：0x004C9F08（共享尾已并入本函数正文）。
std::int32_t WindowProfileCategory_Update(std::int32_t value); // 窗口60仅初始化刷新，随后检查距离。
// 原函数：0x004C9F20。
void WindowProfileCategory_Refresh(); // 固定32/32/15项分类的显隐、文本和选择颜色。
// 原函数：0x004CA1A0。
std::int32_t WindowProfileCategory_SelectCategory(std::int32_t index, std::int32_t state); // 分类点击发送后复制共享默认关键词。
// 原函数：0x004CA300。
void WindowProfileCategory_Release(); // 先清对话类型，再标记61/62/63。
// 原函数：0x004CB430。
std::int32_t WindowProfileView_ReadSelection(std::int32_t index); // 只读selected10C门禁，不检查used。
// 原函数：0x004CB4A0。
std::int32_t WindowProfileCategory_ResolveCategory(std::int32_t index, std::int32_t* group, std::int32_t* value);
// 原函数：0x004CA410。
// 关联原入口：0x004CA438（共享尾已并入本函数正文）。
std::int32_t WindowProfileSearch_Update(std::int32_t value); // 窗口61首次刷结果，距离关闭后仍同步关键词。
// 原函数：0x004CA450。
std::uint32_t WindowProfileSearch_DrawKeyword(); // 活动输入与关键词显示互斥，返回含NUL的UTF-16码元数。
// 原函数：0x004CA500。
void WindowProfileSearch_RefreshResults(); // 固定20条结果、三组标题刷新；不重新显示已隐藏的非空行。
// 原函数：0x004CA6F0。
std::int32_t WindowProfileSearch_HandleSearchButtons(std::int32_t index, std::int32_t state); // 搜索/上页/下页双层转义请求。
// 原函数：0x004CA9C0。
std::int32_t WindowProfileSearch_SelectResult(std::int32_t index, std::int32_t state); // 按行发查看请求，随后重读窗口62高亮。
// 原函数：0x004CAB60。
void WindowProfileSearch_Release(); // 先清对话类型，再标记62/63。
// 原函数：0x004CAB90。
std::int32_t WindowProfileSearch_EditKeyword(std::int32_t unusedIndex, std::int32_t state); // 固定元素7关键词编辑入口。
// 原函数：0x004B0110。
login::YobiAction* WindowTrade_Open(); // 清交易U8，创建窗口43动画并发布借用T*。
// 窗口43三元素组：低S16位置、仅隐藏时联动、只写图形颜色。
// 原函数：0x004B0130。
void WindowTrade_MovePanel(
    WindowInstance* window, std::int32_t index, std::int32_t x, std::int32_t y);
// 原函数：0x004B0180。
void WindowTrade_SetPanelVisible(WindowInstance* window, std::int32_t index, std::int32_t visible);
// 原函数：0x004B01B0。
void WindowTrade_SetPanelColor(WindowInstance* window, std::int32_t index, std::uint32_t color);
// 原函数：0x004AFF70。
std::int32_t WindowTrade_RemoveItem(std::int32_t index); // 释放物品选择，再减同源悬停字节。
// 原函数：0x004B0090。
std::int32_t WindowTrade_RemovePet(std::int32_t index); // 只释放宠物选择及标记。
// 原函数：0x004B0200。
void WindowTrade_Release(); // 窗口43释放回调，原返回值弃用。
// 原函数：0x004B0290。
std::int32_t WindowTrade_Update(std::int32_t value); // 窗口43初始化、关闭检查与四阶段更新。
// 原函数：0x004B08D0。
std::int32_t WindowTrade_Draw(std::int32_t unused); // 四类提示后绘制完整当前窗口。
// 原函数：0x004AFF30。
std::int32_t WindowTrade_AddItem(std::int32_t index, std::int32_t slot); // 空槽领取并标记物品。
// 原函数：0x004AFFB0。
std::int32_t WindowTrade_SwapSelectedItems(std::int32_t first, std::int32_t second); // 仅交换选择槽字节。
// 原函数：0x004B0060。
std::int32_t WindowTrade_AddPet(std::int32_t index, std::int32_t slot); // 空槽领取并标记宠物。
// 原函数：0x004B00C0。
std::int32_t WindowTrade_SwapSelectedPets(std::int32_t first, std::int32_t second);
// 原函数：0x0048BB30。
std::int32_t WindowItem_ClassifyItemSlot(std::int32_t slot); // IDE01：signed槽0..7/8..27/100..119分类1/2/3，其它0，真实S32，四CALL只来源/全消费待。
// 原函数：0x0048BB70。
std::int32_t WindowItem_ClassifyPetSlot(std::int32_t slot); // 宠物槽分类1/2。
// 原函数：0x0048BC60。
std::int32_t WindowItem_FindSortedPetIndex(std::int32_t slot); // DE0B08五个S16索引的首匹配位置。
// 原函数：0x0047D490。
void WindowCraft_ResetItemSelection(); // 仅清D67B00，不改变其他物品状态。
// 原函数：0x004885A0。
std::int32_t WindowItem_RequestTransferQuantity(std::int32_t amount); // IMT01：source7短路/重读、41优先38及转义后状态-Y-X取序保持，原S32返回，全callback/ABI待。
// 原函数：0x004886E0。
void WindowItem_RequestTransfer(
    std::int32_t sourceWindow, std::int32_t sourceElement,
    std::int32_t source, std::int32_t target); // IMT01：完整直接/拆分门槛及两全局不同发布序，原数量callback副作用保持；全数量框/运行待。
// 原函数：0x00488800。
void WindowItem_TransferQuantity(
    std::int32_t sourceMode, std::int32_t source,
    std::int32_t targetMode, std::int32_t target); // IMT01：两低U8模式/原-1及同mode两fresh组读取保持，发送原不消费；全互通待。
// 原函数：0x0048A830。
WindowInstance* WindowQuantity_Open(
    std::int32_t sourceWindow, std::int32_t sourceElement, std::int32_t imageNo,
    std::int32_t maximum, std::int32_t value14, std::int32_t initialValue,
    WindowValueCallback callback); // QIP01：窗口42按原两次持址发布；callback原生非拥有，返回先取样。
// 原函数：0x0048A8C0。
std::int32_t WindowQuantity_Update(std::int32_t value); // QVC01：-1初始化后总返回1。
// 原函数：0x0048A900。
std::int32_t WindowQuantity_HandleInput(std::int32_t unusedIndex, std::int32_t state); // QVC01：输入bit2切换/焦点发布，返回0。
// 原函数：0x0048A950。
std::int32_t WindowQuantity_ButtonOk(std::int32_t index, std::int32_t state); // QVC01：确认回调后复读取窗/关闭/格式，返回0或1。
// 原函数：0x0048AA60。
std::int32_t WindowQuantity_ButtonIncrease(std::int32_t index, std::int32_t state); // QVC01：增加数量先上限后数值，原图号/声效。
// 原函数：0x0048AB10。
std::int32_t WindowQuantity_ButtonDecrease(std::int32_t index, std::int32_t state); // QVC01：减少数量S32正值门槛，原图号/声效。
// 原函数：0x00474170。
std::int32_t WindowElement_HandleSideArea(std::int32_t unusedIndex, std::int32_t state); // WCL01：bit2置bit13，hover优先，否则低BYTE bit5。
// 原函数：0x00474BE0。
std::int32_t WindowPlayerProfile_HandleButtons(std::int32_t index, std::int32_t state); // 窗口7资料切页及前后排请求。
// 原函数：0x00475E00。
std::int32_t WindowPlayerAttributes_HandleButtons(std::int32_t index, std::int32_t state); // 窗口8入口六值快照、加点及切页。
// 原函数：0x0048BB90。
void WindowItem_RequestPetTransfer(
    std::int32_t unusedWindow, std::int32_t unusedElement,
    std::int32_t source, std::int32_t target); // 普通/窗口41两路宠物换位请求。
// 原函数：0x004AAB70。
void WindowBank_DrawOwnPetTooltip(
    std::int16_t x, std::int16_t y, std::int32_t index, std::uint8_t textLayer);
// 原函数：0x004AB400。
void WindowBank_DrawStoredPetTooltip(
    std::int16_t x, std::int16_t y, std::int32_t index, std::uint8_t textLayer);
// 原函数：0x0048D540。
std::int32_t WindowPet_DrawTooltip(
    std::int16_t x, std::int16_t y, std::int32_t index,
    std::uint8_t textLayer, std::uint8_t graphicLayer, std::uint32_t color);
// 原函数：0x004EA670。
std::int32_t WindowPet_HasSkill25600(std::int32_t index); // 原25600技能检查，返回0/1。
// 原函数：0x0048BF30。
void WindowPetDrag_Handle(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);
// 原函数：0x0048C130。
std::int32_t WindowPetDrag_Start(std::int32_t sourceIndex, std::int32_t mode);
// 原函数：0x0048BC90。
void WindowItemDrag_Handle(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // IDE01：物品回调，两fresh分类/两点击BYTE及descriptor持址，reset/取消后原继续，完整分发及运行待。
// 原函数：0x0048BF10。
std::int32_t WindowItemDrag_Start(std::int32_t sourceIndex, std::int32_t mode); // IDE01：mode先发布/真实kind2入口及S32透传，十三CALL只来源，完整回调/owner及运行待。
// 原函数：0x004B1940。
void WindowTrade_HandleOwnInventory(
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state,
    const render::GraphicDescriptor16* descriptor); // 自有物品交易页，20格玩家背包。
// 原函数：0x004B2480。
std::int32_t WindowTrade_HandleOwnPanel(std::int32_t index, std::int32_t state);
// 原函数：0x004B2500。
void WindowTrade_RefreshOwnPanel(
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* unusedAmount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);
// 原函数：0x004B2060。
void WindowTrade_HandleOwnPets(
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state, std::int32_t scroll,
    const render::GraphicDescriptor16* descriptor); // 自有宠物交易页，按索引对取原宠物。
// 原函数：0x004B25D0。
void WindowTrade_HandleOwnItemDrag(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);
// 原函数：0x004B2E00。
void WindowTrade_HandleOwnPetDrag(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);
// 原函数：0x004B2710。
std::int32_t WindowTrade_StartOwnItemDrag(std::int32_t index); // kind6，绑定真实物品拖动回调。
// 原函数：0x004B2F50。
std::int32_t WindowTrade_StartOwnPetDrag(std::int32_t index); // kind7，绑定真实宠物拖动回调。
// 原函数：0x004B2730。
void WindowTrade_HandleSelectedItem(
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state,
    const render::GraphicDescriptor16* descriptor); // 己方已选物品完整交互/绘制。
// 原函数：0x004B2F70。
void WindowTrade_HandleSelectedPet(
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state, std::int32_t scroll,
    const render::GraphicDescriptor16* descriptor); // 己方已选宠物完整交互/绘制。
// 原函数：0x004B3430。
std::int32_t WindowTrade_HandleSelectedPanel(std::int32_t index, std::int32_t state);
// 原函数：0x004B34C0。
void WindowTrade_RefreshSelectedPanel(
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* amount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);
// 原函数：0x004B0F20。
std::int32_t WindowTrade_DrawPetRow(
    std::int16_t x, std::int16_t y, std::int8_t side, std::int32_t index,
    std::uint8_t textLayer, std::uint8_t graphicLayer,
    const render::GraphicDescriptor16* descriptor); // 空宠物槽仍画背景。
// 原函数：0x004B1260。
std::int32_t WindowTrade_HandlePanel(
    WindowInstance* window, std::int32_t index, std::int32_t state,
    const std::int32_t* backgroundImages, std::uint8_t* tab,
    std::int32_t scrollElement, std::int32_t* scrollValue,
    std::int32_t numericElement, std::int32_t maximum,
    std::int32_t itemCount, std::int32_t petCount, std::int32_t amount,
    render::GraphicDescriptor16* descriptor, WindowTradePanelCallback callback); // TQN01：低W16坐标/scroll BYTE及声效后fresh滚动目标，callback与真实S32 0/2返回保。
// 原函数：0x004B3580。
std::int32_t WindowTrade_HandleRemoteSelection(std::int32_t index, std::int32_t state); // 对方交易面板入口。
// 原函数：0x004B3610。
void WindowTrade_HandleRemoteItem(
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* amount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);
// 原函数：0x004B1900。
bool WindowTrade_HitTestOwnPetRow(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height); // X未命中则不读Y。
// 原函数：0x00474550。
std::uint8_t WindowInput_HandleNumber(
    WindowInstance* window, std::int32_t index, std::int32_t state,
    std::int32_t maximum, std::uint8_t layer, const wchar_t* const* hints,
    std::uint8_t flags, std::uint32_t color); // NPI01：数值键/Backspace/四鼠标操作原序，AL返回0/1/3；非Enter/Esc处理器。
// 原函数：0x004AFFE0。
std::int32_t WindowTrade_ReadSelectedItem(std::int32_t index); // S8物品槽，负值归-1。
// 原函数：0x004B00F0。
std::int32_t WindowTrade_ReadSelectedPet(std::int32_t index); // S8宠物槽，负值归-1。
// 原函数：0x004B0A50。
std::int32_t WindowTrade_ButtonPublish(std::int32_t index, std::int32_t state); // 公开两组交易槽及数值。
// 原函数：0x004B0CF0。
std::int32_t WindowTrade_ButtonWithdraw(std::int32_t index, std::int32_t state); // 撤回公开，无计时门禁。
// 原函数：0x004B0DB0。
std::int32_t WindowTrade_ButtonConfirm(std::int32_t index, std::int32_t state); // 确认/撤销确认，共享500ms门槛。
// 原函数：0x004B09E0。
std::int32_t WindowTrade_HandleAmountInput(std::int32_t unusedIndex, std::int32_t state); // 交易输入激活/聊天恢复。
// 原函数：0x004C1FB0。
std::int32_t WindowRuntime_ButtonClose(std::int32_t index, std::int32_t state); // WCL01：持同窗口/graphic置关闭位，返回0/1，未在本回调释放。
// 原函数：0x004FBA30。
std::int32_t WindowItem_SplitTooltipLines(
    std::int32_t x, std::int32_t y, std::uint8_t renderMode,
    std::int32_t fontType, std::int8_t color, std::wstring_view text,
    std::int8_t value7, std::int8_t marker); // 原$分段、颜色续接与即时测宽。
// 原函数：0x0048B810。
// NIO01：原0x0048B810，运行时物品描述直接借宽行。
std::int32_t WindowItem_DrawTooltip(
    std::int32_t x, std::int32_t y, const wchar_t* title, std::int8_t titleColor,
    const std::wstring* lines, std::int32_t lineCount, const wchar_t* footer, std::int8_t footerColor);
std::int32_t WindowItem_DrawTooltip(
    std::int32_t x, std::int32_t y, const wchar_t* title, std::int8_t titleColor,
    const char* gbkLines, std::int32_t stride, std::int32_t lineCount,
    const wchar_t* footer, std::int8_t footerColor); // TDR01：完整层5提示，S16正文坐标/色彩续接、可选标题footer与真实背景返回保持。
// 原函数：0x0048B930。
std::int32_t WindowItem_DrawDescriptionLines(
    std::int32_t x, std::int32_t y, const wchar_t* title, std::int8_t titleColor,
    const std::wstring_view* lines, std::int32_t unusedStride, std::int32_t lineCount,
    const wchar_t* footer, std::int8_t footerColor); // 原连续NUL分段改为宽行表，空行仍推进，不使用stride。
// 原函数：0x0048B450。
std::int32_t WindowItem_DrawPetTooltip(std::int32_t side, std::int32_t index, std::int32_t page);
// 原函数：0x0048B1A0。
std::int32_t WindowItem_DrawEquipmentTooltip(std::int32_t index, std::int32_t page); // 玩家/所选宠物装备提示。
// 原函数：0x0048B350。
std::int32_t WindowItem_DrawDescriptionTooltip(std::int32_t index, std::int32_t page); // SPD01：W32标题字段/页*768八行描述，四次CryptInt及真实提示返回保持。
// 原函数：0x0048B560。
std::int32_t WindowItem_DrawShopTooltip(
    std::int32_t index, std::int32_t page, std::int32_t showUnavailable); // 同源商品及可选不可装备提示。
// 原函数：0x0048B750。
std::int32_t WindowItem_DrawWorkTooltip(std::int32_t page); // CC3210独立详情提示，缓存数值键400。
// 原函数：0x004AC200。
std::int32_t WindowBank_ButtonCloseIcon(std::int32_t index, std::int32_t state); // 窗口38右上关闭。
// 原函数：0x004A9350。
std::int32_t WindowBank_ButtonClose(std::int32_t index, std::int32_t state); // 窗口38/41共用关闭按钮。
// 原函数：0x004AC290。
std::int32_t WindowItemStorage_HandleOwnItems(std::int32_t index, std::int32_t state); // 窗口38自有物品面板。
// 原函数：0x004AC890。
std::int32_t WindowItemStorage_HandleStoredItems(std::int32_t index, std::int32_t state); // SEP01：完整窗口38外部物品消费，低BYTE/坐标原序与S32 consumed保持。
// 原函数：0x004A92D0。
std::int32_t WindowBank_FocusAmountInput(std::int32_t unused, std::int32_t state); // 存取数值输入焦点。
// 原函数：0x004A93E0。
std::int32_t WindowBank_ButtonFirstPage(std::int32_t index, std::int32_t state); // 存放第一页请求。
// 原函数：0x004A94A0。
std::int32_t WindowBank_ButtonSecondPage(std::int32_t index, std::int32_t state); // 存放第二页请求。
// 原函数：0x004A9560。
std::int32_t WindowBank_DrawFirstPageSelection(std::int32_t index, std::int32_t unused); // 第一页选中图。
// 原函数：0x004A9580。
std::int32_t WindowBank_DrawSecondPageSelection(std::int32_t index, std::int32_t unused); // 第二页选中图。
// 原函数：0x004A9B00。
std::int32_t WindowBank_DrawCounts(std::int32_t unusedIndex, std::int32_t unusedState); // 双侧数量和金额。
// 原函数：0x004A95A0。
std::int32_t WindowBank_TabItems(std::int32_t index, std::int32_t state); // 物品页签。
// 原函数：0x004A96D0。
std::int32_t WindowBank_TabPets(std::int32_t index, std::int32_t state); // 宠物页签。
// 原函数：0x004A9820。
std::int32_t WindowBank_TabDeposit(std::int32_t index, std::int32_t state); // 存款页签。
// 原函数：0x004A9990。
std::int32_t WindowBank_TabWithdraw(std::int32_t index, std::int32_t state); // 取款页签。
// 原函数：0x004A9D10。
std::int32_t WindowBank_HandleOwnItems(std::int32_t index, std::int32_t state); // 自有存放物品面板。
// 原函数：0x004AA3D0。
std::int32_t WindowBank_HandleStoredItems(std::int32_t index, std::int32_t state); // SEP01：完整窗口41外部物品消费，低BYTE/坐标原序与S32 consumed保持。
// 原函数：0x004AA9C0。
std::int32_t WindowBank_ButtonPreviousPet(std::int32_t index, std::int32_t state); // 宠物上一行。
// 原函数：0x004AAA70。
std::int32_t WindowBank_ButtonNextPet(std::int32_t index, std::int32_t state); // 宠物下一行。
// 原函数：0x004AAB20。
std::int32_t WindowBank_ScrollPets(std::int32_t index, std::int32_t state); // 两侧宠物滚轮。
// 原函数：0x004ABB20。
std::int32_t WindowBank_HandleAmount(std::int32_t index, std::int32_t state); // SDM01：存取原上限/AL确认、发送后fresh金额再清槽/双文本，返回0。
// 原函数：0x004AAEC0。
std::int32_t WindowBank_HandleOwnPetRow(std::int32_t index, std::int32_t state); // 自有宠物存放行。
// 原函数：0x004AB710。
std::int32_t WindowBank_HandleStoredPetRow(std::int32_t index, std::int32_t state); // 外部宠物提领行。
// 原函数：0x004B4070。
std::int32_t WindowTrade_DrawRemotePetSkillRow(
    std::int32_t x, std::int32_t y, std::int32_t index,
    std::int32_t skill); // 外部宠物技能名称行，空行仍绘背景。
// 原函数：0x004B4140。
std::int32_t WindowTrade_DrawRemotePetTooltip(
    std::int32_t x, std::int32_t y, std::int32_t index); // 外部宠物提示，重量行固定Y=31。
// 原函数：0x004B45A0。
std::int32_t WindowTrade_DrawSelectedPetSkillRow(
    std::int32_t x, std::int32_t y, std::int32_t side,
    std::int32_t index, std::int32_t skill); // 交易宠物技能行，空文本仍绘制行图。
// 原函数：0x004B4680。
std::int32_t WindowTrade_DrawSelectedPetTooltip(
    std::int32_t x, std::int32_t y, std::int32_t side, std::int32_t index); // 完整交易宠物提示。
// 原函数：0x004B3B00。
std::int32_t WindowTrade_DrawOwnPetSkillRow(
    std::int32_t x, std::int32_t y, std::int32_t petIndex,
    std::int32_t skillPosition); // 己方技能按S16索引对取名称。
// 原函数：0x004B3BF0。
std::int32_t WindowTrade_DrawOwnPetTooltip(
    std::int32_t x, std::int32_t y, std::int32_t index); // 完整己方宠物提示，含重量行。
// 原函数：0x004E5620。
login::YobiAction* WindowTrade_ReceiveStart(const char* name); // 全量清交易双侧后复制己方，最后打开43。
// 原函数：0x004E4E60。
void WindowTrade_RequestTargetList(std::int32_t index, const char* name); // 先发送选择请求，失败仍继续本地打开。
// 原函数：0x0049E800。
login::YobiAction* WindowTradeTarget_ReceiveTargets(const char* text, WindowListSelectionCallback callback);
// 原函数：0x004E4E80。
login::YobiAction* WindowTrade_OpenTargetList(const char* text); // 选择表绑定真实交易回调后打开44。
// 原函数：0x0049E990。
void WindowTradeTarget_Refresh(std::int32_t offset); // 逐项复制五行选择文本，重读当前窗口载荷。
// 原函数：0x0049E8F0。
std::int32_t WindowTradeTarget_Update(std::int32_t value); // 窗口44初始化与移动关闭。
// 原函数：0x0049EAA0。
std::int32_t WindowTradeTarget_Draw(std::int32_t unused); // 窗口44滚动消费、完整绘制和九宫格。
// 原函数：0x0049EB50。
std::int32_t WindowTradeTarget_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 上一行。
// 原函数：0x0049EC00。
std::int32_t WindowTradeTarget_ButtonNextRow(std::int32_t index, std::int32_t state); // 下一行。
// 原函数：0x0049ECC0。
std::int32_t WindowTradeTarget_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 上五行。
// 原函数：0x0049ED80。
std::int32_t WindowTradeTarget_ButtonNextPage(std::int32_t index, std::int32_t state); // 下五行。
// 原函数：0x0049EE60。
std::int32_t WindowTradeTarget_Scroll(std::int32_t index, std::int32_t state); // 滚轮像素余量与偏移。
// 原函数：0x0049EEB0。
std::int32_t WindowTradeTarget_ButtonCancel(std::int32_t index, std::int32_t state); // 取消提示/关闭及按钮三态。
// 原函数：0x0049EF60。
std::int32_t WindowTradeTarget_SelectRow(std::int32_t index, std::int32_t state); // 实际选择回调，随后重读窗口关闭。
// 原函数：0x00485110。
void WindowElement_DrawHoverFrame(WindowElement* element, std::int32_t state, std::uint32_t imageNo); // 元素外扩一像素悬停框。
// 原函数：0x00474270。
std::int32_t WindowScroll_ReadPosition(WindowElement* element, std::int32_t maximum); // WSP01：原整数位置换算及两高度取样，借用字节载荷。
// 原函数：0x004742C0。
std::int32_t WindowScroll_SetPosition(WindowElement* element, std::int32_t maximum, std::int32_t position);
// 原函数：0x004743B0。
std::int32_t WindowScroll_ApplyWheel(
    WindowElement* element, std::int32_t position, std::int32_t maximum,
    std::int32_t wheel); // WWS01：原滚轮像素余量、坐标持样/字节载荷，宿主数学图保持。
// 原函数：0x004E0100。
void WindowTrade_CopyDisplayItems(std::int32_t side, std::int32_t index); // 另一展示副本，逐行复制描述。
// 原函数：0x004E02B0。
void WindowItem_BuildStorageDescription(std::int32_t index, std::int32_t side, const char* text); // IDR01：外部物品32行描述/S32页数生产；正常正文及原回扫顺序保持。
// 原函数：0x004A84A0。
std::int32_t WindowBank_ReceiveStorageData(const char* text); // SGM01：完整A/G/I/P数据生产，固定A/增量I-P字段续行与fresh金额组保持，只有A返回1。
// 原函数：0x004A83A0。
WindowInstance* WindowBank_Open(std::int32_t kind, std::int32_t value14, std::int32_t userState);
// 原函数：0x004AC0C0。
WindowInstance* WindowItemStorage_Open(
    std::int32_t kind, std::int32_t value14, std::int32_t userState,
    std::int32_t unusedPhase); // 原38窗口发布，两个caller均保留第四实参。
// 原函数：0x004FB8F0。
std::int32_t WindowItem_ResetItemDrag(); // 物品拖动重置，不等同窗口拖动。
// 原函数：0x004FB920。
std::int32_t WindowItem_CancelMatchingItemDrag(std::int32_t index); // 仅清与本次回复槽号相同的非负拖动编号。
// 原函数：0x004E5DA0。
void WindowTrade_SwapRemoteItems(std::int32_t side, std::int32_t first, std::int32_t second); // 交易整条换位。
// 原函数：0x004E1840。
void WindowTrade_SwapDisplayItems(std::int32_t side, std::int32_t first, std::int32_t second); // 展示整条换位。
// 原函数：0x0048BA60。
void WindowItemDrag_RemapHover(std::int32_t first, std::int32_t second); // UI迁移后再交换同源悬停字节。
// 原函数：0x004B0F10。
std::int32_t WindowElement_NoOp(std::int32_t index, std::int32_t events); // WSB01：原完整零返回事件回调。
// 原函数：0x004A4210。
std::int32_t WindowRuntime_ReturnOne(std::int32_t unused) noexcept; // WCL01：原mov eax,1/ret；release适配不执行资源回收，存档共用。
// 原函数：0x004B0000。
void WindowTrade_ResetSelection(std::int32_t first, std::int32_t second); // 窗口43的20个槽号及28B标记。
// 原函数：0x0047DCD0。
void WindowCraft_ResetSelection(std::int32_t first, std::int32_t second); // 窗口11选择/材料及两组标记。
// 原函数：0x0047DC20。
std::int32_t WindowCraft_ReleaseMaterials(std::int32_t index); // 六槽/单选两路释放，单选尾接聊天恢复。
// 原函数：0x004AD060。
void WindowTrade_RemapTextListSelection(std::int32_t first, std::int32_t second); // 窗口51仅first命中时迁移。
// 原函数：0x004AD090。
void WindowTextList_ReleaseLines(); // 释放窗口51的行文本及原生指针表。
// 原函数：0x004AD0F0。
std::uint32_t WindowTextList_RefreshLines(WindowInstance* window); // 九行刷新，返回末行含NUL长度。
// 原函数：0x004AD180。
login::YobiAction* WindowTextList_Open(
    std::int32_t request, std::int32_t target, const char* text); // 解析标题/正文并展开51。
// 原函数：0x004AD470。
void WindowTextList_ContinueDialog(const char* text); // 同类对话续接，唯一caller不消费原混合返回。
// 原函数：0x004AD780。
void WindowTextList_Release(); // 51释放回调：行表、物品标记、条件关闭请求。
// 原函数：0x004AD800。
std::int32_t WindowTextList_Update(std::int32_t value); // 51初始化、移动关闭及权限可见性。
// 原函数：0x004AD9F0。
std::int32_t WindowTextList_Draw(std::int32_t unused); // 51滚动消费、标题输入及完整绘制。
// 原函数：0x004ADAA0。
std::int32_t WindowTextList_DrawConfirmation(std::int32_t index, std::int32_t unused); // 51模态提示及固定确认按钮。
// 原函数：0x004ADBD0。
std::int32_t WindowTextList_ButtonPreviousRow(std::int32_t index, std::int32_t state); // 上一行，bit400。
// 原函数：0x004ADC80。
std::int32_t WindowTextList_ButtonNextRow(std::int32_t index, std::int32_t state); // 下一行，bit400。
// 原函数：0x004ADD50。
std::int32_t WindowTextList_ButtonPreviousPage(std::int32_t index, std::int32_t state); // 上九行，bit2。
// 原函数：0x004ADE10。
std::int32_t WindowTextList_ButtonNextPage(std::int32_t index, std::int32_t state); // 下九行，bit2。
// 原函数：0x004ADEF0。
std::int32_t WindowTextList_Scroll(std::int32_t index, std::int32_t state); // 51滚轮，draw阶段再刷新文本。
// 原函数：0x004ADF40。
std::int32_t WindowTextList_SubmitTitle(std::int32_t index, std::int32_t state); // 标题双次转义与N请求。
// 原函数：0x004AE170。
std::int32_t WindowTextList_SubmitSelection(std::int32_t index, std::int32_t state); // 当前选择I请求，不预拒绝负值。
// 原函数：0x004AE260。
std::int32_t WindowTextList_EditTitle(std::int32_t index, std::int32_t state); // 标题编辑焦点与InputContext借用。
// 原函数：0x004AE350。
std::int32_t WindowTextList_HandleItemSelection(std::int32_t index, std::int32_t state); // 51物品选择落点/拖出与失效清理。
// 原函数：0x004AE5B0。
std::int32_t WindowTextList_DrawBackgroundTooltip(std::int32_t unused, std::int32_t state); // 仅在提示为空时填背景提示。
// 原函数：0x004797C0。
void WindowSkill_AdjustSpecialHoverCount(std::int32_t first, std::int32_t second); // 槽7特殊悬停计数，两条件独立。
// 原函数：0x004F4630。
std::int32_t WindowCraft_CountRecipeMaterial(std::int32_t itemId); // 当前配方五材料首匹配数量，不读used。
// 原函数：0x004E5770。
void WindowTrade_BuildRemoteItemDescription(std::int32_t index, std::int32_t side, const char* text); // 32行及页数。
// 原函数：0x004E5830。
std::int32_t WindowTrade_ReceiveRemoteItems(const char* text); // 每条14字段，返回终止字段状态1。
// 原函数：0x004E5A40。
std::int32_t WindowTrade_ReceiveRemotePet(std::int32_t index, const char* text); // 单宠物属性与双名称。
// 原函数：0x004E5CD0。
std::int32_t WindowTrade_ReceiveRemotePetSkills(std::int32_t index, const char* text); // 只预清第一条技能。

// 原输入原型四模式更新及聊天恢复，复用既有InputContext owner。
// WIC01：原型四模式、现场context与BYTE结果合同，见doc/cg2d_window_input_context_dispatch_spec.md。
// 原函数：0x00476570。
std::uint8_t WindowInput_ActivateElement(
    WindowInputPrototype* prototype, std::int32_t x, std::int32_t y,
    std::uint32_t mode);
// 原函数：0x004A7B00。
std::int32_t WindowChat_RestoreInput();
// CFD01：聊天点击创建与35槽焦点/当前窗context分离，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A6EA0。
std::int32_t WindowChat_HandleInput(std::int32_t unused, std::int32_t state); // 共用聊天点击，焦点取35槽、上下文写当前窗。
// 原函数：0x004A80F0。
std::int32_t WindowChat_DragScroll(std::int32_t index, std::int32_t state); // WSP01：日志数量前后分别取样，返回滚动结果。
// CFD01：首窗口滚轮按原门槛和调用后新count发布scroll，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
// 原函数：0x004A81D0。
std::int32_t WindowChat_Scroll(std::int32_t unused, std::int32_t state); // 首窗口滚轮，零日志只调整滑块Y。
// 原函数：0x00477490。
WindowInstance* WindowRuntime_FindById(std::int32_t id); // 顺序扫描71显示槽；原循环不跳过空槽。
// 原函数：0x004A7C40。
void WindowChat_RefreshGeometry(); // 窗口35宽高循环与完整几何，见window_35.md。 // CWP01：见聊天几何生产完整合同。
// 原函数：0x004A8140。
void WindowChat_SyncScrollThumb(); // 查显示中的35，按回看比例更新滑块。 // CWC01：见聊天窗口控制完整合同。
// 原函数：0x004A6FC0。
std::int32_t WindowChat_HandleButtons(std::int32_t index, std::int32_t state); // 聊天宽高、范围、字体、颜色与翻页。 // CWC01：见聊天窗口控制完整合同。
extern WindowElementPrototype gWindowElements570CB0[20]; // 原全局：0x00570CB0。 几何更新所读五按钮原型坐标。
// 原函数：0x004740F0。
void WindowInput_FormatSkillSlotCount(std::wstring& destination); // 前10项技能索引的S16槽数，直接宽格式化。
// CIG01：IME测量后先字体宽度再聊天单位取样；见doc/cg2d_input_scroll_draw_spec.md。
// 原函数：0x004A7590。
void WindowChat_DrawInput(input::InputContext* context); // 按聊天宽度滚动目标输入并绘制。
// CIG01：固定1024宽度及滚动绘制正文保持；见doc/cg2d_input_scroll_draw_spec.md。
// 原函数：0x004A7640。
void WindowChat_DrawFixedWidthInput(input::InputContext* context); // 按固定1024像素滚动目标输入并绘制。
// CLD01：日志分行/字体回写与尺寸合同；见doc/cg2d_chat_log_line_drawing_spec.md。
// 原函数：0x004A7750。
void WindowChat_DrawHistory(std::int32_t x, std::int32_t y); // 原字节分行、底部逆序绘制及前项字体回写。
// 原函数：0x0046FBC0。
std::int32_t WindowRuntime_DrawDefault(std::int32_t unused); // 原默认draw回调，完整绘制后返回1。
// 原函数：0x0049E750。
std::int32_t WindowTimerNotice_Update(std::int32_t value); // 窗口39文本居中/到时关闭请求。
// 原函数：0x0049BDB0。
std::int32_t WindowRuntime_DrawCompactFrame(std::int32_t unused); // 完整元素绘制后叠加九宫格背景。
// 原函数：0x00472F00。
std::int32_t WindowRuntime_HitTestRect(
    std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height);
// 原函数：0x00473560。
void WindowRuntime_DispatchElementMouse(std::int32_t index); // 固定元素派发鼠标事件，回调后重读共享阶段。
// 原函数：0x004741A0。
std::int32_t WindowElement_HandleVerticalScroll(std::int32_t index, std::int32_t state); // WSB01：低BYTE/W32标志与前项垂直滚动，保原边界取样序。
// DLP01：只清焦点索引/kind/焦点窗口/回调/phase五槽，来源及两坐标保持，返S32 0。
// 原函数：0x004763E0。
std::int32_t WindowDrag_ResetMouse(); // 通用鼠标重置，不清来源索引/坐标。
// DLP01：已有kind才绑定两个S32焦点槽，不改变会话阶段，返S32 0/1。
// 原函数：0x004763B0。
std::int32_t WindowDrag_SetFocus(std::int32_t windowId, std::int32_t elementIndex); // 已有kind才写焦点两槽。
// 原函数：0x00475D50。
std::int32_t WindowDrag_ReadFocusElement(); // 同源焦点元素。
// 原函数：0x00475D40。
std::int32_t WindowDrag_ReadFocusWindowId(); // 同源焦点窗口，不是当前窗口。
// 原函数：0x00475D60。
std::int32_t WindowDrag_ReadKind(); // 同源拖动种类。
// 原函数：0x00475D70。
std::int32_t WindowDrag_ReadX(); // 同源落点X，S16扩展。
// 原函数：0x00475D80。
std::int32_t WindowDrag_ReadY(); // 同源落点Y，S16扩展。
// 原函数：0x00475D90。
std::int32_t WindowDrag_ReadSourceIndex(); // 同源来源索引。
// DLP01：仅phase0且kind非零启动阶段1，按原焦点/回调/来源写序发布，返S32 0/1。
// 原函数：0x00475DA0。
std::int32_t WindowDrag_Start(
    std::int32_t kind, std::int32_t sourceIndex, WindowDragCallback callback);
// DLP01：仅phase0且kind非零启动阶段3，坐标保S16，原焦点与回调写序保持，返S32 0/1。
// 原函数：0x00476490。
std::int32_t WindowDrag_StartAtPosition(
    std::int32_t kind, std::int32_t sourceIndex, std::int16_t x, std::int16_t y,
    WindowDragCallback callback);
// DLP01：入口phase4/5才可选回调message3，之后按原八槽顺序清理；不重验回调后phase。
// 原函数：0x00476500。
void WindowDrag_Reset(); // 仅阶段4/5调用message3后完整清理。
// 原函数：0x00472D20。
std::int32_t WindowDrag_Update(); // WDF01：完整拖动/焦点两位结果，原宽度和现场取样见SPEC。
// 原函数：0x004730A0。
std::int32_t WindowRuntime_CompactAndReorder(); // WCR01：71槽压紧/关闭/前插，两个标记判断用同一W32 state。
// 原函数：0x00473E40。
void WindowRuntime_AdvanceMousePhase(); // 每轮窗口处理前的鼠标阶段推进。
// 原函数：0x00503CB0。
std::int32_t WindowRuntime_CycleNameMode(); // 73ECBC先减后负值归2，返回归位前结果。
// 原函数：0x00503CD0。
std::int32_t WindowRuntime_IncrementNameMode(); // 名称模式S16递增，保存标量后持久化。
// 原函数：0x004A6D80。
std::int32_t WindowChat_ResetState(); // 聊天初始范围、回看、数量、调整请求和颜色。
// 原函数：0x004C9240。
std::int32_t WindowRuntime_ResetSettings(); // 原型默认坐标和设置默认值，不清窗口实例。 // CWP01：见聊天几何生产完整合同。
// 原函数：0x004C93A0。
std::int32_t WindowRuntime_HasChangedDefaultPositions(); // 文件三处x与固定原型比较，首差异返回1。
// 原函数：0x004C93E0。
std::int32_t WindowRuntime_RepairSettingsTail(); // 原文件长度尾部修复，不增加索引保护。
// 原函数：0x004C9170。
std::int32_t WindowRuntime_ApplySettings(); // 文件坐标和设置回填，共用原UI/世界状态。 // CWP01：见聊天几何生产完整合同。
// 原函数：0x004C9060。
std::int32_t WindowRuntime_LoadSettings(); // WinSaveDate完整加载/默认回退，保留双打开与返回差别。
// 原函数：0x004C92B0。
std::int32_t WindowRuntime_ClearSavedSettings(); // 仅清521CH设置文件区。
// 原函数：0x004C92D0。
std::uint32_t WindowRuntime_CaptureSettings(); // 原序捕获窗口设置，返回窗口33的marker快照。
// 原函数：0x004C9130。
std::int32_t WindowRuntime_SaveSettings(); // 打开成功才捕获并保存；原打开失败同样返回1。
// 原函数：0x00472200。
void WindowRuntime_ClearSavedWindowMarkers(); // 仅清71个marker50，不释放窗口。
// 原函数：0x00472220。
void WindowRuntime_MarkExistingWindows(); // 只将现存窗口marker写1，空窗口保留旧值。
// 原函数：0x004722A0。
void WindowRuntime_ResetMouseAndItemFlags(); // 完整鼠标/拖动重置，先清D67B08七W32。
// 原函数：0x004722E0。
void WindowRuntime_ClearElementFocus(); // 只清异型元素焦点引用。
// 原函数：0x004722F0。
void WindowRuntime_RestoreMarkedWindows(); // marker严格1才分派，四个特殊窗口按原序恢复。
// 原函数：0x004A53D0。
WindowInstance* WindowMailCompose_Open(); // 先写D2F0B8.+26C，再真实打开22，返回借用窗口。
// 原函数：0x00479640。
void WindowRuntime_ResetOperationCount(); // 只清窗口操作S32计数。
// 原函数：0x004796C0。
void WindowRuntime_ResetGatherOperationCount(); // 只清D67AFC。
// 原函数：0x004796F0。
void WindowRuntime_ResetSkillOperationCount(); // 只清D67AFD。
// 原函数：0x00479610。
void WindowRuntime_ResetOperationReferences(); // 清两借用Yobi槽与计数/三个字节，不释放对象。
// 原函数：0x004C7AD0。
void WindowRuntime_ResetShortcutFlag(); // 只清CF4AAC。
// 原函数：0x00475C10。
void WindowRuntime_RestorePositions(); // 现存窗口恢复原型坐标，仅五类追加原序随机/边界处理。
// 原函数：0x004C7AE0。
void WindowRuntime_ProcessShortcuts(); // 两模式完整快捷键，Ctrl优先；唯一caller弃用原返回。
// 原函数：0x00470980。
void WindowRuntime_OpenNormalScene(); // 普通场景完整重开，保持626C14与22/23门禁。
// 原函数：0x004705D0。
std::int32_t WindowRuntime_Initialize(); // 启动UI完整初始化，返回category加载结果。
// 原函数：0x004923D0。
std::int32_t WindowRuntime_ResetAlbumState(); // 三个UI数值依序写5/0/0。
// 原函数：0x004B5A90。
void WindowMessageBoard_ResetRecords(); // 四条记录只清已证字段和四个文本首字节。
// 原函数：0x004B5B70。
std::int32_t WindowMessageBoard_HandleInput(std::int32_t index, std::int32_t state); // 输入焦点及非空文本提示。
// 原函数：0x004B5C30。
std::int32_t WindowMessageBoard_HandleButtons(std::int32_t index, std::int32_t state); // 删除/写入/取消/翻页完整交互。
// 原函数：0x004B62A0。
void WindowMessageBoard_RefreshRecords(); // 四条记录刷新元素及输入位置；唯一caller弃用原地址低字节返回。
// 原函数：0x004B6C20。
std::int32_t WindowMessageBoard_Close(); // 清对话类型、关闭入口时实例并透传SE57结果。
// 原函数：0x004B6970。
std::int32_t WindowMessageBoard_ReceiveRecords(std::int32_t flags, char* text); // 原位反转义后生产同源四记录。
// 原函数：0x004A5390。
std::int32_t WindowMail_ResetComposeState(); // 五组选择槽及D2F0B8已证前缀重置。
// 原函数：0x0048C150。
std::int32_t WindowPetList_ResetSelection(); // 所选宠物与两组五槽按原序重置。
// 原函数：0x004742B0。
std::int32_t WindowScroll_ResetPosition(); // 只清CF4E50/54。
// 原函数：0x004A0940。
std::int32_t WindowMail_ResetSelectionState(); // D67B40七槽交错清零。
extern WindowElementPrototype gWindowElements56DB38[111]; // 原全局：0x0056DB38。 窗口21完整元素及原型坐标唯一来源。
// 原函数：0x004A0970。
std::int32_t WindowContacts_Update(std::int32_t value); // 窗口21布局、未读扫描与滚动条初始化。
// 原函数：0x004A09B0。
std::int32_t WindowContacts_Draw(std::int32_t value); // 完整滚动/闪烁/内容刷新后绘制窗口21。
// 原函数：0x004A09E0。
std::int32_t WindowContacts_Refresh(); // 三行/九行联系人完整内容刷新，不调用公共Draw。
// 原函数：0x004A1690。
void WindowContacts_SetDisplayMode(std::int32_t mode, std::int32_t category); // 显示模式后接名单类别布局。
// 原函数：0x004A16B0。
void WindowContacts_ApplyDisplayLayout(std::int32_t mode); // 联系人三行/九行显隐，不触碰末三元素。
// 原函数：0x004A1780。
void WindowContacts_ApplyCategoryLayout(std::int32_t category); // 普通/公会/迷你名单布局，借原型坐标。
// 原函数：0x004A1C30。
std::int32_t WindowContacts_ScrollRow(std::int32_t index, std::int32_t state); // 单行滚动、边界音效及保存。
// 原函数：0x004A1EF0。
std::int32_t WindowContacts_HandleNavigation(std::int32_t index, std::int32_t state); // 显示/排序/翻页及三行记录TLKH发送。
// 原函数：0x004A2610。
std::int32_t WindowContacts_ButtonGroupMail(std::int32_t index, std::int32_t state); // 群发入口：检查49，动画打开22。
// 原函数：0x004A2870。
std::int32_t WindowContacts_TabCategory(std::int32_t index, std::int32_t state); // 普通/公会/迷你类别切换。
// 原函数：0x004A2BD0。
std::int32_t WindowContacts_ButtonGuildDetails(std::int32_t index, std::int32_t state); // 动画打开窗口48公会详情。
// 原函数：0x004A37E0。
std::int32_t WindowContacts_ResolveRecordIndex(std::int32_t index); // 三行/九行元素编号到同源排序后记录索引。
// 原函数：0x004A40C0。
std::int32_t WindowContacts_Scroll(std::int32_t unused, std::int32_t state); // 滚轮推进，重读类别写回，恒返回0。
// 原函数：0x004A2530。
std::int32_t WindowContacts_HandleRankButton(std::int32_t index, std::int32_t state); // 六职级按钮，bit400转交完整4A38C0。
// 原函数：0x004A38C0。
void WindowContacts_OpenRankSelection(std::int32_t index); // 两次联系人索引读取，生产职级状态后直接打开55。
// 原函数：0x004A2B00。
std::int32_t WindowContacts_OpenMail(std::int32_t index, std::int32_t state); // 发布同源记录/两类别槽，动画打开40。
// 原函数：0x004A3780。
std::int32_t WindowContacts_ResolveButtonRecord(std::int32_t index); // 三行/九行分组按钮到实际记录索引。
// 原函数：0x004A39C0。
std::int32_t WindowContacts_HandleAvatar(std::int32_t index, std::int32_t state); // 九行头像悬停及独立TLKH双击发送。
// 原函数：0x004A3C70。
std::int32_t WindowContacts_ButtonCompose(std::int32_t index, std::int32_t state); // 十二写信按钮，发布类别/索引后动画打开22。
// 原函数：0x004A32E0。
std::int32_t WindowContacts_ButtonDelete(std::int32_t index, std::int32_t state); // 三类联系人删除确认分派。
// 原函数：0x004A33C0。
login::YobiAction* WindowContacts_ConfirmDeleteContact(std::int32_t index); // 普通联系人删除确认，窗口47共享上下文。
// 原函数：0x004A3480。
login::YobiAction* WindowContacts_ConfirmDeleteGuildMember(std::int32_t index); // 公会成员除名确认。
// 原函数：0x004A3540。
login::YobiAction* WindowContacts_ConfirmDeleteMiniMail(std::int32_t index); // 迷你邮件联系人删除确认。
std::int32_t& WindowMail_BorrowDeleteIndex(); // 旧42/后续旧44借同一现代删除索引owner，不复制状态。
// 原函数：0x004A3600。
std::int32_t WindowMail_DeleteContact(std::int32_t unusedIndex, std::int32_t unusedState); // FPkR发送、重读清理及保存。
// 原函数：0x004A3660。
std::int32_t WindowMail_DeleteGuildMember(std::int32_t unusedIndex, std::int32_t unusedState); // 公会编号请求与两保存。
// 原函数：0x004A36B0。
std::int32_t WindowMail_DeleteMiniMailContact(std::int32_t unusedIndex, std::int32_t unusedState); // 迷你姓名转义、发送及重读清理。
// 原函数：0x004A3760。
std::int32_t WindowMail_CancelDelete(std::int32_t unusedIndex, std::int32_t unusedState); // 取消时必要重开窗口21。
// 原函数：0x004AE5F0。
std::int32_t WindowConfirmation_Update(std::int32_t value); // 窗口47初始化：显隐、文本、初始化回调。
// 原函数：0x004AE620。
std::int32_t WindowConfirmation_ButtonOk(std::int32_t index, std::int32_t state); // CTRL确认，动作槽0。
// 原函数：0x004AE690。
std::int32_t WindowConfirmation_ButtonCancel(std::int32_t index, std::int32_t state); // 动作槽1，取消音效55。
// 原函数：0x004AE700。
std::int32_t WindowConfirmation_ButtonActionTwo(std::int32_t index, std::int32_t state); // 动作槽2，音效51。
// 原函数：0x004AE770。
std::int32_t WindowConfirmation_ButtonActionThree(std::int32_t index, std::int32_t state); // 动作槽3，音效55。
// 原函数：0x004AE7E0。
std::int32_t WindowConfirmation_ButtonActionFour(std::int32_t index, std::int32_t state); // 动作槽4，音效51。
// 原函数：0x004AE850。
std::int32_t WindowConfirmation_ButtonActionFive(std::int32_t index, std::int32_t state); // 动作槽5，音效51。
// 原函数：0x004AE8C0。
WindowInstance* WindowConfirmation_RequestClose(); // 固定窗口47标关闭，返回同一实例。
// 原函数：0x004AE8D0。
wchar_t* WindowConfirmation_RefreshText(); // 十一文本清理与宽字符LF分行刷新。
// 原函数：0x004AE9C0。
std::int32_t WindowConfirmation_RefreshButtons(); // 六按钮逐项显隐，原最终偏移216。
// 原函数：0x004AEA10。
std::int32_t WindowConfirmation_PadText(std::wstring& destination, std::wstring_view text); // 按原GBK列宽单位补空格，正文保持UTF-16。
// 原函数：0x004A4180。
std::int32_t WindowGuildRank_Update(std::int32_t value); // 窗口55只初始化滚动位置。
// 原函数：0x004A41A0。
std::int32_t WindowGuildRank_Draw(std::int32_t unused); // 刷新/公共绘制后反求滚动，再画九宫格。
// 原函数：0x004A4220。
std::int32_t WindowGuildRank_SelectRow(std::int32_t index, std::int32_t state); // 职级行选择及当前选择图号。
// 原函数：0x004A42C0。
std::int32_t WindowGuildRank_ButtonClose(std::int32_t index, std::int32_t state); // 固定入口窗口关闭、声音后的图形更新。
// 原函数：0x004A4370。
std::int32_t WindowGuildRank_ButtonOk(std::int32_t index, std::int32_t state); // uHb设置后重取窗口关闭，返回仅由hover决定。
// 原函数：0x004A4420。
std::int32_t WindowGuildRank_HandleScrollButtons(std::int32_t index, std::int32_t state); // 上下滚动先发声，只修各自一侧边界。
// 原函数：0x004A4540。
std::uint32_t WindowGuildRank_Refresh(); // 十行职级/成员姓名刷新，返回含NUL姓名长度。
// 原函数：0x004A45E0。
std::int32_t WindowGuildRank_Scroll(std::int32_t unused, std::int32_t state); // 非零滚轮推进，恒返回0。
// 原函数：0x004A4620。
std::int32_t WindowMailRead_Update(std::int32_t value); // 读信初始化：选页后保存，再扫描未读。
// 原函数：0x004A4650。
std::int32_t WindowMailRead_Draw(std::int32_t unused); // 刷新清当前未读，再公共绘制。
// 原函数：0x004A4660。
std::int32_t WindowMailRead_ButtonPrevious(std::int32_t index, std::int32_t state); // 上一封，负页跳9。
// 原函数：0x004A4710。
std::int32_t WindowMailRead_ButtonNext(std::int32_t index, std::int32_t state); // 下一封，页>=10跳0。
// 原函数：0x004A47C0。
std::int32_t WindowMailRead_ButtonReply(std::int32_t index, std::int32_t state); // 回复上下文与旧物品占用释放。
// 原函数：0x004A48A0。
void WindowMailRead_FindUnread(); // 最后未读位置；原唯一caller不消费混合地址/整数返回。
// 原函数：0x004A49B0。
std::uint32_t WindowMailRead_Refresh(); // 三类邮件十行、姓名、时间和页码，返回末复制长度。
// 原函数：0x004A4FC0。
std::int32_t WindowMailRead_HandleAvatar(std::int32_t unused, std::int32_t state); // 悬停双图，区分三类头像采样序。
// 原函数：0x004A53F0。
std::int32_t WindowMailCompose_Update(std::int32_t value); // 发信初始化、宠物候选和收件人刷新。
// 原函数：0x004A5650。
void WindowMailCompose_Release(); // 22/23共享释放：保存草稿、释放占用、关闭49。
// 原函数：0x004A58E0。
std::int32_t WindowMailCompose_HandleInput(std::int32_t unused, std::int32_t state); // 输入聚焦及非当前输入补绘。
// 原函数：0x004A5970。
std::int32_t WindowMailCompose_HandleRecipients(std::int32_t index, std::int32_t state); // 宠物邮件切换、单发/群发完整六路径。
// 原函数：0x004A6160。
std::uint8_t WindowMailCompose_RefreshRecipients(); // 收件人/宠物按钮显隐及输入位置刷新，透传476570低W8。
// 原函数：0x004A56F0。
std::int32_t WindowPetMailCompose_Update(std::int32_t value); // 宠物邮件两遍候选与刷新，标关闭后仍继续。
// 原函数：0x004A6300。
std::int32_t WindowPetMailCompose_HandleGiftItem(std::int32_t index, std::int32_t state); // 物品占用迁移、取出拖动及悬停框。
// 原函数：0x004A6450。
std::int32_t WindowPetMailCompose_HandleSendButtons(std::int32_t index, std::int32_t state); // 四按钮，两类发送保留不同失败声音。
// 原函数：0x004A6AC0。
std::uint8_t WindowPetMailCompose_RefreshPet(); // 宠物名称/颜色和携带物品图、数量刷新。
// 原函数：0x004A6D30。
std::int32_t WindowPetMailCompose_HasGiftPet(std::int32_t index); // 同源flags6AA完整W16恰3。
// 原函数：0x004A6D50。
std::int32_t WindowPetMailCompose_HasGiftItem(std::int32_t index); // 先47D4A0，非零才读取flags6AA恰2。
// 原函数：0x0047D4A0。
std::int32_t WindowCraft_ReadItemSelection(); // 原S32 D67B00窄读，不返回其他状态。
extern WindowTextPrototype gWindowText572F58; // 原全局：0x00572F58。 49三类副标题，元素原型与刷新同源。
extern WindowTextPrototype gWindowText572F60; // 原全局：0x00572F60。
extern WindowTextPrototype gWindowText572F68; // 原全局：0x00572F68。
extern WindowTextPrototype gWindowText572F78; // 原全局：0x00572F78。 49三类标题。
extern WindowTextPrototype gWindowText572F80; // 原全局：0x00572F80。
extern WindowTextPrototype gWindowText572F88; // 原全局：0x00572F88。
// 原函数：0x004B4B00。
std::int32_t WindowGroupSelect_Update(std::int32_t value); // 群组选人初始化/恢复与父窗门禁。
// 原函数：0x004B4B70。
void WindowGroupSelect_Release(); // 释放时仅关闭仍处于群发方式的22。
// 原函数：0x004B4B90。
std::int32_t WindowGroupSelect_Draw(std::int32_t value); // 滚动反求、七行刷新、公共绘制。
// 原函数：0x004B4BB0。
std::int32_t WindowGroupSelect_HandlePageButtons(std::int32_t index, std::int32_t state); // 前后翻七行，仅hover消费。
// 原函数：0x004B4CA0。
std::int32_t WindowGroupSelect_HandleScrollButtons(std::int32_t index, std::int32_t state); // bit400单行滚动，发声后修正位置。
// 原函数：0x004B4DB0。
std::int32_t WindowGroupSelect_ClampScroll(); // 顺序修正上下界，保留count<7时负上界。
// 原函数：0x004B4DF0。
std::int32_t WindowGroupSelect_DragScroll(std::int32_t unused); // 非负最大值反求并发布滚动位置。
// 原函数：0x004B4E20。
void WindowGroupSelect_Refresh(); // 三种标题和七行显隐，不改选择标志。
// 原函数：0x004B5210。
std::int32_t WindowGroupSelect_TabCategory(std::int32_t index, std::int32_t state); // 切换三模式后清滚动位置。
// 原函数：0x004B53C0。
std::int32_t WindowGroupSelect_ButtonSelectAll(std::int32_t index, std::int32_t state); // 全组选择，普通联系人借原排序索引。
// 原函数：0x004B5550。
std::int32_t WindowGroupSelect_SelectRow(std::int32_t index, std::int32_t state); // 七行选择，点击时重读滚动位置。
// 原函数：0x004B5A40。
std::int32_t WindowGroupSelect_Scroll(std::int32_t unused, std::int32_t state); // 零wheel仍调用4743B0。
// 原函数：0x004B5630。
void WindowGroupSelect_ResetRecipients(); // 清计数/模式及两200项数组，不清初始化标志。
// 原函数：0x004B5660。
void WindowGroupSelect_BuildRecipients(std::int32_t mode); // 稀疏名单生产，不按count压紧槽位。
// 原函数：0x004B5760。
void WindowGroupSelect_SortRecipients(std::int32_t category); // 原CRT次序排序60或200个S32索引。
// 原函数：0x004B57B0。
std::int32_t WindowGroupSelect_CompareRecipients(const std::int32_t* left, const std::int32_t* right);
// 原函数：0x004A2700。
void WindowContacts_ClampScroll(); // 修正当前名单位置并更新元素17滚动条。
// 原函数：0x004A3820。
std::int32_t WindowContacts_DragScroll(std::int32_t unused); // 滚动条反求位置；调用后重读类别写回。
// 原函数：0x004A32B0。
std::int32_t WindowMail_AdvanceUnreadBlink(); // 邮件闪烁相位推进；开窗不重置。
// 原函数：0x004A2C60。
void WindowContacts_Sort(); // 填200项C9FCE4后按类别依原CRT次序排序60/200/20项。
// 原函数：0x004A2CC0。
std::int32_t WindowContacts_CompareRecords(const std::int32_t* left, const std::int32_t* right); // MC01-C8标准索引比较，有效性对称，同键返回0。
// 原函数：0x004A3010。
std::int32_t WindowContacts_CompareContactName(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通姓名原字节比较。
// 原函数：0x004A3070。
std::int32_t WindowContacts_CompareGuildName(
    const login::detail::OrderedRecord1051B14* left,
    const login::detail::OrderedRecord1051B14* right); // 公会姓名原字节比较。
// 原函数：0x004A30D0。
std::int32_t WindowContacts_CompareGuildValue(
    const login::detail::OrderedRecord1051B14* left,
    const login::detail::OrderedRecord1051B14* right); // 公会value00完整S32差值。
// 原函数：0x004A3100。
std::int32_t WindowContacts_CompareContactTitle(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通text32原字节比较。
// 原函数：0x004A3160。
std::int32_t WindowContacts_CompareContactValue(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通value08完整S32差值。
// 原函数：0x004A3190。
std::int32_t WindowContacts_CompareGuildStatus(
    const login::detail::OrderedRecord1051B14* left,
    const login::detail::OrderedRecord1051B14* right); // 公会value10完整S32差值。
// 原函数：0x004A31C0。
std::int32_t WindowContacts_CompareRecordKey(
    const std::int32_t* leftValue04, const std::int32_t* rightValue04); // 借原记录+4字段，相等仍-1。
// 原函数：0x004A31E0。
std::int32_t WindowContacts_CompareContactNumber(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通S16 value02，零映射最大S32。
// 原函数：0x004A3230。
std::int32_t WindowContacts_CompareMiniMailNumber(
    const world::detail::MiniMailContactRecord* left,
    const world::detail::MiniMailContactRecord* right); // 迷你序号按S16比较，零不替换。
// 原函数：0x004A3260。
std::int32_t WindowContacts_CompareGuildNumber(
    const login::detail::OrderedRecord1051B14* left,
    const login::detail::OrderedRecord1051B14* right); // 公会S16 value0A，零映射最大S32。
// 原函数：0x004A3950。
std::int32_t WindowGuildDetails_ReadRankPermissions(); // 当前公会职级权限80H。
// 原函数：0x004AF960。
std::uint32_t WindowGuildDetails_RefreshDraft(); // 窗口48管理权限下复制草稿，返回末文本含NUL长度。
// 原函数：0x004AFE20。
std::int32_t WindowGuildDetails_HasChangedRank(std::int32_t index); // 公会草稿差异；原职级含80H时忽略纯flags变化。
// 原函数：0x004AFEA0。
std::int32_t WindowGuildDetails_HasChangedRanks(); // 完整扫描32职级，不首个差异早返。
// 原函数：0x004AFB10。
std::int32_t WindowGuildDetails_SubmitName(); // 公会名称变更校验/转义/Ufm类型2发送。
// 原函数：0x004AFC00。
std::int32_t WindowGuildDetails_SubmitDescription(); // 公会第二文本变更校验/转义/Ufm类型3发送。
// 原函数：0x004AFCF0。
std::int32_t WindowGuildDetails_SubmitRanks(); // 先验证全体变化文本，再逐条GY发送并恢复聊天。
// 原函数：0x004AF180。
void WindowGuildDetails_RefreshReadOnly(); // 权限分流与只读刷新，原混合返回由caller弃用。
// 原函数：0x004AF250。
void WindowGuildDetails_RefreshEditor(); // 管理模式焦点/草稿/按钮与颜色刷新。
// 原函数：0x004AF650。
std::int32_t WindowGuildDetails_RefreshRanks(); // 七职级与三附加文本刷新，返回末行Y推进值304。
// 原函数：0x004AFA00。
void WindowGuildDetails_SaveInputDraft(std::int32_t index); // 输入原型正文写入当前编辑号对应的同源草稿。
// 原函数：0x004AFA60。
void WindowGuildDetails_ScrollRow(); // 页码原范围0..25与音效、滑块同步。
// 原函数：0x004AFAE0。
void WindowGuildDetails_DragScroll(std::int32_t unused); // 拖动反求页码；保留主绘制转发的未用实参。
// 原函数：0x004AFED0。
std::int32_t WindowGuildDetails_CommitFocusedInput(); // 固定窗口/焦点，扫描九个输入载荷。
// 原函数：0x004AEC30。
std::int32_t WindowGuildDetails_ToggleRankPermission(std::int32_t index, std::int32_t state);
// 原函数：0x004AED40。
std::int32_t WindowGuildDetails_HandleActionButton(std::int32_t index, std::int32_t state);
// 原函数：0x004AEE00。
std::int32_t WindowGuildDetails_HandleInput(std::int32_t index, std::int32_t state);
// 原函数：0x004AF000。
std::int32_t WindowGuildDetails_HandleScrollButtons(std::int32_t index, std::int32_t state);
// 原函数：0x004AF090。
std::int32_t WindowGuildDetails_HandleScrollFocus(std::int32_t index, std::int32_t state); // WSP01：滚动恰1时调用既有焦点页依赖。
// 原函数：0x004AF0C0。
std::int32_t WindowGuildDetails_Scroll(std::int32_t unused, std::int32_t state);
// 原函数：0x004AF100。
std::int32_t WindowGuildDetails_ButtonClose(std::int32_t index, std::int32_t state);
// 原函数：0x004A3960。
std::int32_t WindowGuildDetails_CanEdit(std::int32_t mask); // 当前职级-1才拒绝，再读同源权限。
// 原函数：0x004A3990。
std::int32_t WindowGuildDetails_CanEditRank(std::int32_t index, std::int32_t mask); // 当前职级门禁通过后读取指定职级。
// 原函数：0x004CC6A0。
std::int32_t WindowBlacklist_ResetState(); // 三个UI数值依序写5/0/0。
// 原函数：0x004CBD30。
void WindowAdventureLog_InitializeDate(); // 首次借用原localtime对象并写同源日志文件名。
// 原函数：0x004CC340。
void WindowAdventureLog_Refresh(); // 窗口66原字节分行/十四行刷新，固定651000B局部缓冲。
// 原函数：0x004CC540。
void WindowAdventureLog_Load(); // 日志文件重开/计数，保留原双关闭槽语义，见window_66.md。
// 原函数：0x004CBDB0。
std::int32_t WindowAdventureLog_HandleNavigation(std::int32_t index, std::int32_t state); // 上下翻与原非标准日期前后切换。
// 原函数：0x004A8330。
WindowInstance* WindowChat_SwitchMode(); // 标旧聊天窗关闭、开另一窗后发布模式，透传新窗T*。
// 原函数：0x004CCED0。
void WindowBlacklist_Refresh(); // 窗口65十行序号/名称与活动输入刷新。
// 原函数：0x004CC6E0。
std::int32_t WindowBlacklist_EditRow(std::int32_t index, std::int32_t state); // 删除后重读行，再决定提示或编辑。
// 原函数：0x004CC9B0。
std::int32_t WindowBlacklist_HandleButtons(std::int32_t index, std::int32_t state); // 六个黑名单按钮及原消费差异。
// 原函数：0x004CCE90。
std::int32_t WindowBlacklist_DragScroll(std::int32_t index, std::int32_t state); // WSP01：原90参数拖动换算；旧X代码1999。
// 原函数：0x004CD150。
login::YobiAction* WindowBlacklist_ConfirmClear(); // 生产窗口47的清空确认及两个真实回调。
// 原函数：0x004CD1E0。
std::int32_t WindowBlacklist_Clear(std::int32_t unusedIndex, std::int32_t unusedState);
// 原函数：0x004CD220。
std::int32_t WindowBlacklist_CancelClear(std::int32_t unusedIndex, std::int32_t unusedState);
// 原函数：0x00470600。
void WindowRuntime_InitializeLogin(); // 每次登录的完整UI/邮件/profile初始化；不清全部窗口指针。
// 原函数：0x004A6DB0。
std::int32_t WindowChat_ResetScrollRequests(); // 仅重置回看、两调整请求与数量。
// 原函数：0x0044B170。
std::int32_t WindowChat_ClearHistory(); // 清512条日志首字节，再清数量与回看。
// 原函数：0x004A7700。
std::int32_t WindowChat_ResetScroll(); // 不清日志正文或写指针。
// 原函数：0x004A7710。
void WindowChat_RequestHeightIncrease(); // 请求高度+1。
// 原函数：0x004A7720。
void WindowChat_RequestHeightDecrease(); // 请求高度-1。
// 原函数：0x004A7730。
void WindowChat_RequestWidthIncrease(); // 请求宽度+1。
// 原函数：0x004A7740。
void WindowChat_RequestWidthDecrease(); // 请求宽度-1。
// 原函数：0x004A82E0。
std::int32_t WindowChat_RefreshCompactHeight(); // 窗口57高度循环调整并清两个请求。
// WND01：数字与四符号原字节正反绘制，见doc/cg2d_window_number_draw_spec.md。
// 原函数：0x004771B0。
void WindowNumber_Draw(
    std::int32_t x, std::int32_t y, std::wstring_view text,
    std::int8_t style, std::int8_t color, std::uint8_t flags, std::uint8_t layer);
// WND01：运行步进乘长度后取扣宽，见doc/cg2d_window_number_draw_spec.md。
// 原函数：0x00477460。
std::int32_t WindowNumber_MeasureWidth(std::wstring_view text, std::int8_t style); // 原数字字符数乘运行步进减扣宽。

static_assert(sizeof(WindowInputReference) == 16);
static_assert(sizeof(WindowElementPrototypeSource) == 8);
static_assert(offsetof(WindowElementPrototype, source) == 0x18);
static_assert(offsetof(WindowElementPrototype, mouseCallback) == 0x20);
static_assert(sizeof(WindowElementPrototype) == 0x28);
static_assert(offsetof(WindowPrototype, width) == 0x10);
static_assert(offsetof(WindowPrototype, height) == 0x14);
static_assert(offsetof(WindowPrototype, elements) == 0x20);
static_assert(offsetof(WindowPrototype, update) == 0x28);
static_assert(offsetof(WindowPrototype, draw) == 0x30);
static_assert(offsetof(WindowPrototype, release) == 0x38);
static_assert(sizeof(WindowPrototype) == 0x40);
static_assert(offsetof(WindowDragPrototype, rectangles) == 8);
static_assert(sizeof(WindowDragPrototype) == 16);
static_assert(offsetof(WindowGraphicData, offsetX) == 4);
static_assert(offsetof(WindowGraphicData, offsetY) == 8);
static_assert(offsetof(WindowGraphicData, width) == 0x0C);
static_assert(offsetof(WindowGraphicData, height) == 0x10);
static_assert(offsetof(WindowGraphicData, color) == 0x14);
static_assert(sizeof(WindowGraphicData) == 0x18);
static_assert(sizeof(WindowInputData) == 8);
static_assert(offsetof(WindowNumberData, color) == 8);
static_assert(offsetof(WindowNumberData, style) == 9);
static_assert(offsetof(WindowNumberData, flags) == 0x0A);
static_assert(offsetof(WindowNumberData, byte07) == 0x0B);
static_assert(sizeof(WindowNumberData) == 16);
static_assert(offsetof(WindowAnimationData, action) == 0);
static_assert(offsetof(WindowAnimationData, value04) == 8);
static_assert(sizeof(WindowAnimationData) == 16);
static_assert(offsetof(WindowMouseData, deltaX) == 4);
static_assert(offsetof(WindowMouseData, deltaY) == 8);
static_assert(sizeof(WindowMouseData) == 12);
static_assert(offsetof(WindowElement, data) == 0x20);
static_assert(offsetof(WindowElement, mouseCallback) == 0x28);
static_assert(sizeof(WindowElement) == 0x30);
static_assert(offsetof(WindowInstance, state) == 0);
static_assert(offsetof(WindowInstance, elements) == 0x30);
static_assert(offsetof(WindowInstance, update) == 0x38);
static_assert(offsetof(WindowInstance, draw) == 0x40);
static_assert(offsetof(WindowInstance, release) == 0x48);
static_assert(offsetof(WindowInstance, secondaryCount) == 0x50);
static_assert(offsetof(WindowDragRect, y) == 4);
static_assert(offsetof(WindowDragRect, width) == 8);
static_assert(offsetof(WindowDragRect, height) == 12);
static_assert(sizeof(WindowDragRect) == 16);
static_assert(offsetof(WindowInstance, dragMouseX1C) == 0x1C);
static_assert(offsetof(WindowInstance, dragMouseY20) == 0x20);
static_assert(offsetof(WindowInstance, dragRects40) == 0x54);
static_assert(sizeof(WindowInstance) == 0xF8);
static_assert(offsetof(WindowState, window) == 0x10);
static_assert(offsetof(WindowState, userState) == 0x20);
static_assert(offsetof(WindowState, actionFlags1C) == 0x28);
static_assert(offsetof(WindowState, marker50) == 0x5C);
static_assert(sizeof(WindowState) == 0x60);

} // namespace detail
} // namespace cg::ui
