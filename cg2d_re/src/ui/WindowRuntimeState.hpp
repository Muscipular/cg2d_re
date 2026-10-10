#pragma once

#include "cg/ui/WindowRuntime.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::login {
struct YobiAction;
}

namespace cg::login::detail {
struct OrderedRecord1051B14;
}

namespace cg::input {
struct InputContext;
}

namespace cg::world::detail {
struct PlayerItemRecord;
struct ContactRuntimeRecord;
struct MiniMailContactRecord;
}

namespace cg::ui {

using WindowCallback = std::int32_t (*)(std::int32_t value);
using WindowReleaseCallback = void (*)();
using WindowElementCallback = std::int32_t (*)(std::int32_t index, std::int32_t state);
using WindowValueCallback = void (*)(std::int32_t value);
using WindowDragCallback = void (*)(
    std::int32_t message, std::int32_t flags, std::int32_t sourceIndex);
using WindowListSelectionCallback = void (*)(std::int32_t index, const char* text);

// 原type1的24B图形载荷，六个数值字段，不含地址。
struct WindowGraphicData {
    std::int32_t imageNo;
    std::int32_t offsetX;
    std::int32_t offsetY;
    std::int32_t width;
    std::int32_t height;
    std::uint32_t color;
};

// 原type5首槽是真实输入上下文引用；外层载荷拥有、上下文不拥有。
struct WindowInputData {
    input::InputContext* context;
};

// 原type7的文本T*与四个字节；字符串借用，不随外层free。
struct WindowNumberData {
    char* text;
    std::uint8_t color;
    std::int8_t style;
    std::uint8_t flags;
    std::uint8_t byte07;
};

// 原第2类元素的8B载荷；471622写真实Yobi，471334只给它标死。
struct WindowAnimationData {
    login::YobiAction* action;
    std::uint32_t value04;
};

// 原第3类元素的8B载荷；文本由47167F分配，47134F释放。
struct WindowTextData {
    std::array<std::byte, 4> bytes00;
    char* text;
};

// 原type4的12B鼠标载荷；滚动回调与元素命中共用，不含地址。
struct WindowMouseData {
    std::uint32_t flags;
    std::int32_t deltaX;
    std::int32_t deltaY;
};

// 仅按已证字段增量映射；未消费数据不计最终布局冻结，见UI窗口SPEC。
struct WindowElement {
    std::int32_t type;
    std::uint32_t state;
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::int32_t visible;
    void* data; // 原+1C，七类数据共用；后续按type解释，不是地址数值。
    WindowElementCallback mouseCallback; // 原+20。
};

// 原实例+40的16B可拖动区域，472D20逐项命中。
struct WindowDragRect {
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
};

struct WindowInstance {
    std::uint32_t state; // 原+00；关闭请求置bit15。
    std::int32_t id;
    std::int32_t layer;
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::int32_t dragMouseX1C;
    std::int32_t dragMouseY20;
    std::uint32_t flags;
    std::int32_t elementCount;
    WindowElement* elements; // 原+2C。
    WindowCallback update; // 原+30。
    WindowCallback draw; // 原+34。
    WindowReleaseCallback release; // 原+38。
    std::int32_t secondaryCount; // 原+3C。
    std::array<WindowDragRect, 10> dragRects40; // 原[+40,+E0)。
};

namespace detail {

// 窗口45/46选择回调：非零返回允许关闭，文本只借用；释放通知index=-1。
using WindowTextChoiceCallback = std::int32_t (*)(std::int32_t index, const char* text);
// CC4230原回调实际接收技能和等级两个S32，47C7B2/47C7B3压栈，返回不用。
using SkillWindowSelectionCallback = void (*)(std::int32_t skillIndex, std::int32_t technique);
std::int32_t Sub_4EF710(std::int32_t skillId); // 221..224分类0..3，其余-1。
void Sub_4F0200(const char* text); // ysVc完整名单生产；唯一回复调用方弃用混合返回。
login::YobiAction* Sub_4871B0(); // 技能目标名称/颜色借用及45创建，不回滚失败计数。
std::int32_t Sub_487020(std::int32_t index, const char* text); // 请求属性/发送技能及释放通知。
void Sub_47A140(); // 窗口10模式2的技能状态差异请求，原返回无人消费。
void Sub_479B80(WindowInstance* window, std::int32_t skillIndex); // 技能列表定位，不以used过滤索引查找。
void Sub_47B6C0(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t pairIndex,
    std::int32_t enabled, std::int32_t textLayer, std::int32_t graphicLayer,
    std::uint32_t color); // 固定配对对应技能，需求槽数按S8。
std::int32_t Sub_47B820(std::int32_t index, std::int32_t state); // 列表排序请求、打开技术与右键拖动。
void Sub_479700(
    std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // kind4拖动，仅message0推进。
std::int32_t Sub_4797A0(std::int32_t sourceIndex); // 安装479700并透传返回。
void Sub_479C80(WindowInstance* window, std::int32_t pairIndex); // 切技术模式，负参数保留原技能。
void Sub_479DA0(WindowInstance* window, std::int32_t level); // 切配方，滚动先用旧上限。
void Sub_479EA0(WindowInstance* window, std::int32_t skillIndex); // 战斗列表及原过滤表尾字节计数门槛。
void Sub_47A000(WindowInstance* window, std::int32_t index, std::uint8_t directSkill); // 战斗技术初始化。
std::int32_t Sub_47C570(std::int32_t index, std::int32_t state); // 战斗列表行，技能标志决定可用。
std::int32_t Sub_47C700(std::int32_t index, std::int32_t state); // 战斗技术选择，独立FP读取与回调后重取窗口。
login::YobiAction* Sub_47A490(); // 清选择回调并创建普通技能窗。
login::YobiAction* Sub_47A4C0(SkillWindowSelectionCallback callback); // 设置回调并创建战斗列表窗。
login::YobiAction* Sub_47A500(std::int32_t skillIndex, SkillWindowSelectionCallback callback); // 战斗技术窗。
void Sub_47A1A0(WindowInstance* window); // 宠物模式初始化，保留弃用结果的FP读取。
void Sub_47A320(WindowInstance* window); // 骑乘模式初始化，名称借自身战斗对象。
void Sub_47CB00(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t pairIndex,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 宠物行绘制，两次宠物查询。
void Sub_47CE70(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t pairIndex,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 骑乘行绘制，固定技术记录。
std::int32_t Sub_47CC80(std::int32_t index, std::int32_t state); // 宠物选择，不加滚动偏移。
std::int32_t Sub_47CFE0(std::int32_t index, std::int32_t state); // 骑乘选择，FP不足不补失败音效。
login::YobiAction* Sub_47A540(WindowValueCallback callback); // 设置CC4D14并创建宠物技能窗。
login::YobiAction* Sub_47A580(WindowValueCallback callback); // 设置CC4D14并创建骑乘技能窗。
void Sub_4966B0(std::int32_t mode, std::int16_t x, std::int16_t y); // 窗口26五模式创建/配置及入场偏移。
std::int32_t Sub_496B90(std::int32_t value); // 窗口26初始化、入退场与按钮显隐。
std::int32_t Sub_496D50(std::int32_t unused); // 先绘制再更新技术名，按命中索引追加目标说明。
void Sub_496AE0(); // 窗口26存在才收尾、关闭10/27/28及清三展开引用，原返回无人消费。
void Sub_496F10(); // 先清九个选择槽，再关闭三子窗及清同源展开引用。
void Sub_497000(); // 战斗技能快捷入口，保留actionFlags低S8门槛；原返回不用。
std::int32_t Sub_496FB0(); // 技能可用性：门禁失败/已选返回0，否则-1。
std::int32_t Sub_4970B0(); // 同一门禁，读取物品选择槽3。
void Sub_497100(); // 物品快捷入口，清目标待选状态并打开27。
std::int32_t Sub_4971C0(); // 同一门禁，读取宠物选择槽4。
void Sub_497210(); // 宠物快捷入口，打开28，不清物品待选状态。
std::int32_t Sub_473840(std::int32_t index, std::int32_t state); // 窗口6完整九分支按钮。
std::int32_t Sub_46FBD0(std::int32_t unused, std::int32_t state); // 快捷栏4点击收起到5，打开后发布CA0440。
std::int32_t Sub_46FC10(std::int32_t unused, std::int32_t state); // 快捷栏5点击展开到4。
std::int32_t Sub_46FC60(std::int32_t index, std::int32_t state); // 完整九按钮，保留6/7子入口消费覆盖。
std::int32_t Sub_4708A0(std::int32_t index, std::int32_t state); // 窗口34存在时更新图形，点击开关。
std::int32_t Sub_470910(std::int32_t index, std::int32_t state); // 窗口33开关，创建使用专有尺寸入口。
login::YobiAction* Sub_495F00(
    std::uint32_t flags, std::uint32_t mode, login::YobiAction** completionSlot); // 小地图184x159展开，完成槽原生T**。
std::int32_t Sub_495E10(std::int32_t index, std::int32_t unused); // 小地图按窗口/元素坐标入既有绘制队列。
std::int32_t Sub_495E60(std::int32_t index, std::int32_t unused); // 固定文本实例，门禁后刷新玩家X。
std::int32_t Sub_495EB0(std::int32_t index, std::int32_t unused); // 固定文本实例，门禁后刷新玩家Y。
std::int32_t Sub_495F70(std::int32_t index); // 清18B后选中指定字节，Y/X快照交4D3020。
std::int32_t Sub_495FC0(); // 按四W32/一W16清唯一18B选择区，返回0。
std::int32_t Sub_495FE0(std::int32_t index, std::int32_t state); // 情绪窗18按钮，请求被拒仍更新选择并发声。
std::int32_t Sub_4723D0(std::int32_t index, std::int32_t state); // SHP01：三项提示完整点击/取样序，原门槛和关闭开窗保持。
void Sub_470AC0(); // TUC01：窗口1HP/FP、时钟/四位倒计时/提示原生产序；机器ABI与运行待。
void Sub_4718F0(); // TUC01：窗口2直接FP复制/四位倒计时，首窗口取样晚于描述符发布。
std::int32_t Sub_471CD0(std::int32_t unused, std::int32_t state); // 状态栏1点击切2，调用后发布保存模式。
std::int32_t Sub_471D10(std::int32_t unused, std::int32_t state); // 状态栏2点击切3。
std::int32_t Sub_471D60(std::int32_t unused, std::int32_t state); // 状态栏3按场景返回2或1。
std::int32_t Sub_471DD0(std::int32_t unused, std::int32_t state); // 仅hover命中消费。
std::int32_t Sub_471DF0(std::int32_t index, std::int32_t state); // 六选项命中区操作前15项图形，发送后SE51。
std::int32_t Sub_473080(); // SHP01：玩家伤势优先返0，否则宠物原编号+10/未命中-1。
std::int32_t Sub_4731B0(std::int32_t index); // SHP01：五个S16配对首匹配位置，无则-1，不查used。
std::int32_t Sub_4972B0(std::int32_t index, std::int32_t state); // 攻击选择，原目标标志475H。
std::int32_t Sub_4973C0(std::int32_t index, std::int32_t state); // 防御G命令及玩家动作完成。
void Sub_4974C0(std::int32_t skillIndex, std::int32_t technique); // CC4230真实双参数命令回调。
std::int32_t Sub_4975A0(std::int32_t index, std::int32_t state); // 技能按钮及选目标返回技术窗。
std::int32_t Sub_497770(std::int32_t index, std::int32_t state); // 物品选择/返回27，保留CC4D40独立门禁。
std::int32_t Sub_497900(std::int32_t index, std::int32_t state); // 宠物选择及28窗口关闭。
std::int32_t Sub_497A60(std::int32_t index, std::int32_t state); // P命令，使用九槽中的槽6。
std::int32_t Sub_497B80(std::int32_t index, std::int32_t state); // E命令，使用槽7，无已用技能门禁。
std::int32_t Sub_497C80(std::int32_t index, std::int32_t state); // TUC01：原载荷判空/hover持有，刷新后重取R命令状态。
void Sub_497E30(std::int32_t technique); // 宠物命令，保留重复查询与负参数略过。
void Sub_497FF0(std::int32_t technique); // 骑乘命令，清选择模式并保留骑乘快照写序。
std::int32_t Sub_498160(std::int32_t index, std::int32_t state); // 宠物/骑乘技能窗开关。
void Sub_4982C0(); // 完整倒计时/超时发送，本帧超时后仍绘制。
void Sub_43C3E0(); // 553F80门禁及首个可选对象的目标提示，不要求鼠标命中。
void Sub_43C490(std::int32_t* values, std::int32_t count); // 正数count逐槽写-2，原caller弃返回。
void Sub_498520(); // 完整六态战斗命令分派，所有出口共用倒计时尾。
void Sub_43C950(); // 玩家入场位且动画/窗口32均空才创建，失败也播放原音效。
void Sub_496570(); // 五宠物四项CryptInt快照，唯一CF4B78数组生产。
void Sub_4965D0(); // 战斗对象四值更新，保留独立5ABBFC及原空值门禁。
std::int32_t Sub_49B630(std::int32_t unused); // 窗口28归还按钮显隐。
std::int32_t Sub_49BA10(std::int32_t index, std::int32_t state); // 窗口28宠物归还M|FF。
std::int32_t Sub_49BB00(std::int32_t index, std::int32_t state); // 窗口28三条宠物选择行。
void Sub_49B670(
    std::int16_t x, std::int16_t y, std::int32_t petIndex,
    std::uint8_t textLayer, std::int32_t graphicLayer,
    const render::GraphicDescriptor16* descriptor); // 快照数值与宠物属性行，唯一caller弃返回。
std::int32_t Sub_47B110(std::int32_t index, std::int32_t state); // 固定入口窗口的返回按钮。
std::int32_t Sub_47B210(std::int32_t index, std::int32_t state); // TUC01：先借元素再读U8动作标志；Y/X发送后更新同源占用。
std::int32_t Sub_47C950(std::int32_t index, std::int32_t state); // 无可用宠物技能时略过，通知-1。
std::int32_t Sub_47A5C0(); // 战斗返回：模式4退列表，0/3/5/6关闭。
std::int32_t Sub_47B3D0(std::int32_t index, std::int32_t state); // 前一行，bit400。
std::int32_t Sub_47B470(std::int32_t index, std::int32_t state); // 后一行，bit400。
std::int32_t Sub_47B520(std::int32_t index, std::int32_t state); // 前十行，音效后重读页码。
std::int32_t Sub_47B5D0(std::int32_t index, std::int32_t state); // 后十行，不额外钳制负上限。
std::int32_t Sub_47B680(std::int32_t unusedIndex, std::int32_t state); // 同源滚轮，恒返0。
void Sub_47B9B0(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t technique,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 技术行，>=100按战斗FP着色。
void Sub_47C240(
    std::int16_t x, std::int16_t y, std::int8_t visualState, std::int32_t recipe,
    std::int32_t textLayer, std::int32_t graphicLayer, std::uint32_t color); // 配方行与独立图号表。
std::int32_t Sub_47BC00(std::int32_t index, std::int32_t state); // 五类技能选择与完整失败音效。
std::int32_t Sub_47C3A0(std::int32_t index, std::int32_t state); // 转生一致才处理配方，恒返0。
void Sub_479650(); // 原窗口操作S32计数加1。
void Sub_479660(); // 原操作计数减1及严格零时的退出发送/技能状态刷新。
std::int32_t Sub_47D170(
    WindowInstance* window, std::int32_t anchor, std::int32_t first, std::int32_t second,
    std::int32_t dx, std::int32_t dy); // 双预览定位/显隐，返回原36B元素数值偏移。
WindowAnimationData* Sub_47D220(
    WindowInstance* window, std::int32_t first, std::int32_t second,
    std::int32_t flag, std::int32_t actionId, std::int32_t direction); // 返回第二个借用载荷。
std::int32_t Sub_484D60(std::int32_t index, std::int32_t state); // 六材料槽悬停、翻页与图形绘制。
std::int32_t Sub_47D4B0(); // 晶石等级读取后比较当前配方等级。
std::int32_t Sub_47D4F0(); // 单选物品不加8，晶石读取后重新取得选择索引。
std::int32_t Sub_47D530(); // 六材料首个匹配类别，晶石读取后重取槽号再比较等级。
std::int32_t Sub_47D5B0(); // 首材料slot+8等级在晶石读取前快照。
login::YobiAction* Sub_47D5F0(
    std::int32_t skill, std::int32_t recipe, std::int32_t request); // 制造开窗，保存同源配方id。
login::YobiAction* Sub_47D780(std::int32_t skill, std::int32_t request); // 单选加工开窗。
login::YobiAction* Sub_47D8F0(
    std::int32_t skill, std::int32_t request, std::int32_t kind); // 装饰开窗，kind严格1为宠物装备。
login::YobiAction* Sub_47DA70(std::int32_t skill, std::int32_t request); // 精炼开窗，条件设置世界状态9。
login::YobiAction* Sub_47DB50(std::int32_t skill, std::int32_t request); // 材料开窗，无条件设置世界状态9。
WindowAnimationData* Sub_47E1B0(WindowInstance* window); // 制造等待模式1，按配方等级生成同源截止。
WindowAnimationData* Sub_47EF00(WindowInstance* window); // 单选等待模式4，刻印排除宠物辅助。
WindowAnimationData* Sub_4801B0(WindowInstance* window); // 打包等待模式13，仍读取绝对单选槽等级。
void Sub_480600(WindowInstance* window); // 装饰等待固定8000ms，原被AL覆盖的返回值由caller弃用。
WindowAnimationData* Sub_480C20(WindowInstance* window); // 精炼等待模式10，一次RAND且不读物品等级。
std::int32_t Sub_483EB0(std::int32_t index, std::int32_t state); // 六材料完整投放/换位/逐帧校验与绘制。
WindowAnimationData* Sub_47DF50(WindowInstance* window); // 制造模式0完整初始化，逆序发布五材料需求。
WindowAnimationData* Sub_47FAE0(WindowInstance* window); // 装饰模式6，固定魔力100。
WindowAnimationData* Sub_47FD00(WindowInstance* window); // 宠物装备装饰模式15，独立标题。
WindowAnimationData* Sub_47FF20(WindowInstance* window); // 模式12材料类别文本与完整六材料交互。
WindowAnimationData* Sub_480A00(WindowInstance* window); // 精炼模式9，采样同源计时并按发送模式配置动作。
std::int32_t Sub_4F4430(std::int32_t itemType, std::int32_t skillId); // 技能分类对应的特殊材料类别。
void Sub_483E30(
    std::int32_t destination, std::int32_t source, std::int32_t material); // 先释放再发布材料槽/占用。
std::int32_t Sub_4880D0(std::int32_t skillId, std::int32_t extraLevel); // 完整快照后查技能等级，附加值只读低U8。
std::int32_t Sub_488160(); // 前三材料占用等级与0的最大值。
std::int32_t Sub_488180(); // 六槽中首个材料类别0/1/2对应的itemId。
void Sub_4881D0(std::int32_t destination, std::int32_t source); // 模式12完整材料校验、清理与发布。
void Sub_47DE00(
    std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // 材料/单选拖动与释放。
std::int32_t Sub_47DF30(std::int32_t sourceIndex); // 安装类型10的47DE00拖动回调。
std::int32_t Sub_482F70(std::int32_t itemType); // 加工分类；类别7优先归2。
std::int32_t Sub_482270(std::int32_t index, std::int32_t state); // 执行资格、五等待及刻印直接发送。
std::int32_t Sub_482FA0(std::int32_t index, std::int32_t state); // 续作，初始化后独立重读模式5。
std::int32_t Sub_4830B0(std::int32_t index, std::int32_t state); // 底部关闭，点击仍返回0。
std::int32_t Sub_483150(std::int32_t index, std::int32_t state); // 20格背包投放/快速选择/拖动及绘制。
std::int32_t Sub_484FB0(std::int32_t index, std::int32_t unusedState); // 结果图，负图号不绘制。
std::int32_t Sub_485030(std::int32_t index, std::int32_t state); // 刻印输入焦点，借用唯一D2DE70。
std::int32_t Sub_485160(std::int32_t index, std::int32_t state); // 右上关闭，点击返回1。
std::int32_t Sub_485200(std::int32_t index, std::int32_t unusedState); // 非法刻印字符覆盖提示。
std::int32_t Sub_485350(std::int32_t index, std::int32_t unusedState); // 空白刻印覆盖提示。
std::int32_t Sub_4847C0(std::int32_t index, std::int32_t state); // 单选投放/校验/拖动与绘制。
WindowAnimationData* Sub_47EC00(WindowInstance* window); // 单选模式3完整初始化，返回借用载荷。
WindowAnimationData* Sub_47E5E0(WindowInstance* window); // 制造结果完整窗口11展示。
WindowAnimationData* Sub_47EBE0(); // 仅已有窗口11才更新，不自动创建。
WindowAnimationData* Sub_4F39F0(std::int32_t result, const char* text); // JgO制造分支，原字节摘要与展示。
std::int32_t Sub_484E90(std::int32_t index, std::int32_t state); // 单物品悬停/翻页，物品编号不加8。
WindowAnimationData* Sub_47F3D0(WindowInstance* window); // 物品加工结果展示，保留入口选择快照及交叉门禁。
WindowAnimationData* Sub_47FAC0(); // 仅已有窗口11才展示物品结果。
WindowAnimationData* Sub_4F1800(std::int32_t result, const char* text); // JgO第二分支七字段及独立摘要。
void Sub_4796E0(); // 只递减既有D67AFC低U8，不清其他操作计数。
void Sub_4796D0(); // 仅递增既有D67AFC低U8。
login::YobiAction* Sub_4854A0(std::int32_t skill, std::int32_t level); // 低U8请求状态及窗口12创建。
WindowAnimationData* Sub_485700(WindowInstance* window); // 取消窗口12操作并归空闲，保留末动画载荷返回。
WindowAnimationData* Sub_485890(WindowInstance* window); // 窗口12计时/操作初始化。
std::int32_t Sub_486CF0(std::int32_t index, std::int32_t state); // FP/背包检查与重启。
std::int32_t Sub_486E50(std::int32_t index, std::int32_t state); // 关闭或取消，hover后重取窗口。
std::int32_t Sub_486F20(std::int32_t index, std::int32_t unusedState); // 结果图及固定框，恒返0。
WindowAnimationData* Sub_47D2E0(
    WindowInstance* unusedWindow, std::int32_t anchor,
    std::int32_t first, std::int32_t second, std::uint32_t deadline); // 双预览六点插值，实际借当前窗口。
WindowAnimationData* Sub_485AF0(WindowInstance* window); // 窗口12完整结果展示及双计时槽发布。
WindowAnimationData* Sub_486020(); // 仅更新已有窗口12，不自动创建。
WindowAnimationData* Sub_4ED6B0(std::int32_t result, const char* text); // JgO第四分支，失败不解析正文。
std::int32_t Sub_47D280(
    WindowInstance* window, std::int32_t first, std::int32_t second,
    std::int32_t imageNo); // 双动画图号，非正才隐藏；返回原数值元素偏移。
std::int32_t Sub_487F30(std::int32_t imageNo, std::int32_t offset); // 技能预览分组图号转换。
login::YobiAction* Sub_4875E0(); // 原操作计数、关联关闭及窗口13创建。
WindowAnimationData* Sub_4876B0(WindowInstance* window); // 窗口13按分类完整填充结果。
std::int32_t Sub_487D60(std::int32_t index, std::int32_t state); // 先比较FP，再继续技能请求。
std::int32_t Sub_487E80(std::int32_t index, std::int32_t state); // 关闭入口窗口，音效后写图号。
login::YobiAction* Sub_4EEA40(std::int32_t result, const char* text); // JgO第三分支，七字段及窗口13。
login::YobiAction* Sub_4EF750(const char* text); // ND自身/远端属性生产，分类1跳过人物行。
login::YobiAction* Sub_4874B0(); // 同源六属性视图与46创建，不回滚计数。
std::int32_t Sub_4872E0(std::int32_t index, const char* unusedText); // 按属性目标编号发送技能。
std::int32_t Sub_479820(); // 技能索引对中查1009，按S16读取槽号。
std::int32_t Sub_479860(); // 玩家物品槽7的晶石等级，未用/未知返回0。
std::int32_t Sub_479920(); // 同一槽7的属性0..3；未用0，未知ID为-1。
std::int32_t Sub_4799D0(); // 重新选取出战宠物并读取原图号，未命中0。
std::int32_t Sub_479A00(std::int32_t petIndex); // 宠物技术索引对中查40330..40332。
std::int32_t Sub_479A70(std::int32_t petIndex, std::int32_t crystalLevel); // 宠物等级闭区间。
std::int32_t Sub_479AB0(); // 当前宠物FP的单次CryptInt比较。
std::int32_t Sub_479B00(); // 技能/晶石/宠物技术/属性/等级条件链。
std::int32_t Sub_486FE0(); // 当前技能技术value90与重读晶石等级等值。
std::int32_t Sub_48AEA0(std::int32_t index, const char* unusedText); // 选择发送/释放计数；-2仅允许关闭。
login::YobiAction* Sub_48AF00(); // 名称/颜色借用及物品占用计数，创建失败按原条件回滚。
login::YobiAction* Sub_4E4860(const char* text); // LY完整目标名单生产，保留原Yobi返回。
std::int32_t Sub_48B010(std::int32_t index, const char* unusedText); // 用物品/请求回退/释放通知。
login::YobiAction* Sub_48B0A0(); // 属性展示源及46创建成功后的引用计数。
login::YobiAction* Sub_4E4020(const char* text); // nFV完整属性值生产，不清旧表或替换宠物状态owner。
// 原CB10D8的52B值记录；旧78只读借用，实际六行存储仍由WindowRuntime私有持有。
struct ItemUseStatusRecord {
    std::array<char, 20> name;
    std::int32_t level;
    std::int32_t injury;
    std::int32_t hp;
    std::int32_t maxHp;
    std::int32_t fp;
    std::int32_t maxFp;
    std::int32_t kind;
    std::int32_t target;
};
static_assert(offsetof(ItemUseStatusRecord, level) == 0x14);
static_assert(offsetof(ItemUseStatusRecord, injury) == 0x18);
static_assert(offsetof(ItemUseStatusRecord, hp) == 0x1C);
static_assert(offsetof(ItemUseStatusRecord, maxFp) == 0x28);
static_assert(offsetof(ItemUseStatusRecord, kind) == 0x2C);
static_assert(offsetof(ItemUseStatusRecord, target) == 0x30);
static_assert(sizeof(ItemUseStatusRecord) == 52);
const ItemUseStatusRecord& ReadLegacyItemUseStatus(std::int32_t index) noexcept;
std::int32_t ReadLegacyItemUseStatusCount() noexcept; // 原CC4D44，不固定为六行或复制数量。
login::YobiAction* Sub_49F000(
    const char* const* names, const std::uint8_t* colors,
    std::int32_t count, WindowTextChoiceCallback callback);
void Sub_49F040();
std::int32_t Sub_49F060(std::int32_t value);
void Sub_49F0A0(std::int32_t offset); // 刷新五行，未用行复制593850但保留颜色。
std::int32_t Sub_49F1A0(std::int32_t value);
std::int32_t Sub_49F260(std::int32_t index, std::int32_t state);
std::int32_t Sub_49F310(std::int32_t index, std::int32_t state);
std::int32_t Sub_49F3D0(std::int32_t index, std::int32_t state);
std::int32_t Sub_49F490(std::int32_t index, std::int32_t state);
std::int32_t Sub_49F570(std::int32_t index, std::int32_t state);
std::int32_t Sub_49F5C0(std::int32_t index, std::int32_t state);
std::int32_t Sub_49F680(std::int32_t index, std::int32_t state);

// 原窗口46的28B属性源；仅名称地址扩宽，4874B0/48B0A0分别生产。
struct WindowStatusChoiceRecord {
    const char* name;
    std::int32_t level;
    std::int32_t hp;
    std::int32_t maxHp;
    std::int32_t fp;
    std::int32_t maxFp;
    std::int8_t injury;
    std::uint8_t color;
    std::array<std::byte, 2> bytes1A;
};
static_assert(offsetof(WindowStatusChoiceRecord, level) == 8);
static_assert(offsetof(WindowStatusChoiceRecord, injury) == 0x1C);
static_assert(offsetof(WindowStatusChoiceRecord, color) == 0x1D);
static_assert(sizeof(WindowStatusChoiceRecord) == 32);
login::YobiAction* Sub_49F760(
    const WindowStatusChoiceRecord* records, std::int32_t count,
    WindowTextChoiceCallback callback); // 三槽按序发布后打开46，透传借用Yobi。
void Sub_49F790();
std::int32_t Sub_49F7B0(std::int32_t value);
void Sub_49F7E0(std::int32_t offset); // 六行名称/等级/HP/FP及伤势图标。
std::int32_t Sub_499830(std::int32_t value); // 31/46共用绘制与九宫格尾。
std::int32_t Sub_499880(std::int32_t index, std::int32_t state); // 结果窗关闭按钮，固定入口图形载荷。
std::int32_t Sub_4990F0(std::int32_t index, std::int32_t state); // 窗口29观战结束，hover不消费。
std::int32_t Sub_499750(std::int32_t index, std::int32_t state); // 窗口30点击关闭，索引未使用。
std::int32_t Sub_49FA40(std::int32_t index, std::int32_t state);
std::int32_t Sub_49FB00(std::int32_t index, std::int32_t state);
std::int32_t Sub_49FBC0(std::int32_t index, std::int32_t state);

// 原4B1260的14参槽位回调；借用数值与描述符，返回值原端弃用。
using WindowTradePanelCallback = void (*)(
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* amount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);

// 原CF3B10/D2F338的已消费字段；不是x86字节布局，不冻结未消费前缀。
struct WindowItemSelectorState {
    std::array<WindowElement*, 20> elements00; // 4C6EC0绑定的非拥有元素，非零模式保留槽19。
    std::array<WindowElementCallback, 4> callbacks50; // 4C6550写回元素mouseCallback。
    std::int32_t value60; // 4C6550发布的数值页状态，4C75A0仅在2时绘制。
    std::int32_t value64;
    std::uint8_t* mode68; // 借用57DA28/57E42C，消费者按S8解释。
    std::int32_t step6C;
    std::int32_t selectedPet70;
    std::int32_t previousPet74;
    login::YobiAction* preview; // 借用原内嵌+78的静态对象，不负责释放。
    std::int32_t hoverCount3A8;
    std::array<std::byte, 4> bytes3AC;
    std::int32_t hoverIndex3B0;
    std::array<std::byte, 4> bytes3B4;
    std::array<render::GraphicDescriptor16, 3> descriptors3B8;
    std::int32_t initialized3E8; // 7A90首次切页后置1，库存更新/释放及构造清零。
    std::uint8_t value3EC;
    std::uint8_t value3ED;
    std::int32_t imageIndex3F0;
    std::int32_t imageIndex3F4;
    std::int32_t hoverIndex3F8;
    std::uint8_t value3FC;
};

extern WindowItemSelectorState gItemSelectorCF3B10;
extern WindowItemSelectorState gItemSelectorD2F338;
WindowItemSelectorState* Sub_4C61B0(WindowItemSelectorState* state); // 完整构造，不整区清零。
// SLC01：单读并返回同一选择器selectedPet70的完整S32，不做校验或改owner。
std::int32_t Sub_4C71B0(const WindowItemSelectorState* state); // 原+70同源S32。
void Sub_4C6EC0(WindowItemSelectorState* state, std::uint8_t mode); // SFD01：低BYTE模式装配非拥有元素/四回调；逐次借当前窗口，非零模式槽19不写，发布次序保持。
// SLC01：负slot先返-1；S8 mode0取8项玩家X表，正mode取6项宠物X表，其它返-1。
std::int32_t Sub_4C63C0(const WindowItemSelectorState* state, std::int32_t slot);
// SLC01：负slot先返-1；S8 mode0取8项玩家Y表，正mode取6项宠物Y表，其它返-1。
std::int32_t Sub_4C6400(const WindowItemSelectorState* state, std::int32_t slot);
void Sub_4C6440(WindowItemSelectorState* state); // SPG01：玩家页2显示槽6；W32装配模式0显示槽19，两门槛独立，原正文保持。
void Sub_4C6470(WindowItemSelectorState* state); // SPG01：宠物页/名称负重及原动态空位显示保持；只判低AL，不钳制itemSlots或补保护。
void Sub_4C6500(WindowItemSelectorState* state); // SPG01：按原顺序隐藏槽6/7/8/15/16、六装备槽与17；模式0才隐藏19，主元素不改。
void Sub_4C6550(WindowItemSelectorState* state, std::int32_t page); // SPG01：先发布页再隐藏/新页分发；CC40D4仅低BYTE、四回调先取元素；原五页和写序保持，全机器ABI待。
void Sub_4C6790(WindowItemSelectorState* state); // SFD01：原四页分发/业务钳制保持；两按钮按原顺序取样，共借descriptor[0]；已知上行由窗口回调返回1。
std::int32_t Sub_4C6990(WindowItemSelectorState* state); // PSP01：同state mode按S8映射，回调返回的S32索引原序发布到selectedPet70并返回。
void Sub_4C69B0(WindowItemSelectorState* state); // PSP01：同宠物previous/updated BYTE及名字/负重原序保持；首次测宽先字体后文本，负重先文本后字体。
void Sub_4C6AC0(WindowItemSelectorState* state, std::int32_t x); // SPG01：依原序移动主元素、四按钮及六表坐标；原S32结果与固定循环保持，不新增钳制。
// SLC01：两轴先窗口再元素取值，flags/淡入计数只读原BYTE；void不证明原EAX上行等价。
void Sub_4C6B10(WindowItemSelectorState* state); // 消费本帧hover，宠物物品固定借CF3B10索引。
std::int32_t Sub_4C6D90(WindowItemSelectorState* state); // PSP01：同一非拥有预览推进/两轴/队列；掩码替换字节先旧字节，返回/原读取门槛保持。
std::int32_t Sub_4C71C0(std::int32_t ordinal); // PSP01：五排序对按used W16/索引S16/状态W16计数；不足仍取末扫描项，不新增匹配/索引保护。
bool Sub_4C7340(WindowItemSelectorState* state, std::int32_t mode); // SPG01：写mode低BYTE后持旧页；页3按新mode切2/3、页2不切；bool仅承接原AL=1，高位ABI待。
bool Sub_4C7380(WindowItemSelectorState* state, std::int32_t tab, std::int32_t events);
bool Sub_4C78A0(WindowItemSelectorState* state, std::int32_t events); // 预览显示开关及按钮图。
bool Sub_4C7910(WindowItemSelectorState* state, std::int32_t events); // 装备开关，索引无效时保留部分改写。
bool Sub_4C7A10(WindowItemSelectorState* state, std::int32_t events); // 低U16两方向事件独立处理。
void Sub_4C7A90(WindowItemSelectorState* state); // SFD01：首次借mode地址、previous=-1、切页后置initialized；尾逐帧分发，上行两窗口回调固定返回1。
// ISP01：原local负值返-1，成功后取当前窗口，再按元素X/窗口X取样累加。
std::int32_t Sub_4C7240(
    const WindowItemSelectorState* state, std::int32_t element, std::int32_t slot,
    std::int32_t offset); // X；负局部坐标不读取窗口。
// ISP01：原local负值返-1，成功后取当前窗口，再按元素Y/窗口Y取样累加。
std::int32_t Sub_4C7280(
    const WindowItemSelectorState* state, std::int32_t element, std::int32_t slot,
    std::int32_t offset); // Y；与X分开采样模式和当前窗口。
// ISP01：S8模式0借玩家前8槽，正模式经原宠物校验后两次重读索引借列表/数量；失败不改输出。
bool Sub_4C72C0(
    const WindowItemSelectorState* state, world::detail::PlayerItemRecord** items,
    std::int32_t* count); // 借用同源物品表；失败不写输出。
// ISP01：借同一列表/原动态count，state低BYTE消费；循环尾先重取count再推进槽。
std::int32_t Sub_48A6C0(std::int32_t index, std::int32_t state); // 装备屏蔽页完整绘制。
// ISP01：原两mode选物/发送/独立取消及绘制保持，按钮和mode2 flags只读原BYTE。
std::int32_t Sub_489F00(std::int32_t index, std::int32_t state); // 两模式物品选择/退出与完整绘制。
// ISP01：对话29选物先占用/发布两个回调与mode2，启动返回丢弃，再透传焦点绑定S32结果。
std::int32_t Sub_4888C0(const char* text); // 对话29选择入口，发布特殊回调后绑定焦点。
std::int32_t Sub_488930(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_488950(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_488970(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_488EA0(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_488EC0(std::int32_t index, std::int32_t state); // 五索引对中选择对应宠物页签。
std::int32_t Sub_488C50(std::int32_t index, std::int32_t state); // 库存关闭按钮。
void Sub_488CF0(std::int32_t amount); // 数量窗回调，经玩家动作/场景门禁发送丢钱。
std::int32_t Sub_488D30(std::int32_t index, std::int32_t state); // 丢钱数量窗开关及金额同步。
std::int32_t Sub_488E60(std::int32_t unusedIndex, std::int32_t unusedState);
std::int32_t Sub_488E80(std::int32_t unusedIndex, std::int32_t unusedState);
std::int32_t Sub_488FB0(std::int32_t index, std::int32_t state); // 普通背包完整交互/绘制。
std::int32_t Sub_489760(std::int32_t index, std::int32_t state); // 玩家/宠物装备完整交互。
std::int32_t Sub_489EE0(std::int32_t unusedIndex, std::int32_t state);
bool Sub_4C75A0(WindowItemSelectorState* selector, std::int32_t state); // 附加属性页及回调恢复。

// 4C6EC0发布的战斗背包/装备交互。
std::int32_t Sub_499930(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_499950(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_499970(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_499B00(std::int32_t unusedIndex, std::int32_t unusedState);
std::int32_t Sub_499B20(std::int32_t unusedIndex, std::int32_t unusedState);
std::int32_t Sub_499B40(std::int32_t unusedIndex, std::int32_t state); // 战斗页签忽略内部返回。
std::int32_t Sub_499B60(std::int32_t index, std::int32_t state); // 战斗宠物页签及无效按钮图。
std::int32_t Sub_499C30(std::int32_t index, std::int32_t state);
std::int32_t Sub_49AC40(std::int32_t index, std::int32_t state); // 战斗装备页及宠物禁用页。
std::int32_t Sub_49B610(std::int32_t unusedIndex, std::int32_t state); // D2F338附加属性页包装。

// 原CB1398等固定81B对话行；创建入口只借用，不负责释放。
struct WindowDialogTextLine {
    std::array<char, 81> text;
};
static_assert(sizeof(WindowDialogTextLine) == 81);

const WindowDialogTextLine* BorrowNpcDialogLinesCC24D4() noexcept; // 借用标题及正文，旧/现代对话同源。
std::int32_t ReadLegacyChoiceImageCC4890() noexcept; // 4A0640生产，旧55/56/57共享预览消费。
std::int32_t ReadLegacyChoiceImageC9A99C() noexcept; // 4A0790生产，旧74预览消费。
std::int32_t ReadLegacyShopImageC9A9A0() noexcept; // 4A0140生产，旧62预览借读同源图号。
std::int32_t ReadLegacyChoiceImageD2EA74() noexcept; // 4A0850生产，旧83预览消费。

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
    std::int16_t x;
    std::int16_t y;
    std::int32_t visible;
};
static_assert(sizeof(WindowConfirmationAmount) == 12);
static_assert(offsetof(WindowConfirmationAmount, x) == 4);
static_assert(offsetof(WindowConfirmationAmount, y) == 6);
static_assert(offsetof(WindowConfirmationAmount, visible) == 8);

// 原D2EAB8的60H技能报价记录；未消费字节不据此定业务语义。
struct Window54SkillRecord {
    std::uint16_t used;
    std::array<std::byte, 2> bytes02;
    std::int32_t value04;
    std::int32_t price;
    std::array<char, 25> name;
    std::array<std::array<char, 29>, 2> description;
    std::byte byte5F;
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
    std::array<std::byte, 2> bytes02;
    std::int32_t value04;
    std::int32_t slots;
    std::array<char, 17> name;
    std::uint8_t allowed;
    std::array<std::byte, 2> bytes1E;
    std::int32_t tribe;
    std::uint8_t hintFlag;
    std::array<std::byte, 3> bytes25;
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
    std::array<std::byte, 2> bytes02;
    std::int32_t value04;
    std::array<char, 25> name;
    std::array<std::array<char, 29>, 2> description;
    std::byte byte5B;
};
static_assert(sizeof(Window54TechniqueSlot) == 0x5C);
static_assert(offsetof(Window54TechniqueSlot, value04) == 4);
static_assert(offsetof(Window54TechniqueSlot, name) == 8);
static_assert(offsetof(Window54TechniqueSlot, description) == 0x21);
static_assert(offsetof(Window54TechniqueSlot, byte5B) == 0x5B);

// 原CB2230/D30428晶石商品；不同于普通商店C38H记录，保留区尚不定语义。
struct WindowCrystalItem {
    std::uint8_t used;
    std::array<char, 29> name;
    std::array<char, 3072> description;
    std::array<std::byte, 2> bytesC1E;
    std::int32_t imageNo;
    std::int32_t valueC24;
    std::int32_t selectedCount; // +C28，两种晶石商店共用的数量字段语义。
    std::int32_t valueC2C;
    std::uint32_t flags;
    std::array<char, 24> tips;
    std::int32_t limitC4C;
    std::int32_t remainingC50;
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
    std::int32_t button, std::int32_t row, char* const* texts, std::uint8_t inputCount);
using WindowDialogFrameCallback = std::int32_t (*)(
    WindowInstance* window, std::int32_t button, std::int32_t row, std::int32_t events);
using WindowChoiceCallback = std::int32_t (*)(std::int32_t index);
login::YobiAction* Sub_49FCA0(
    const char* title, std::int32_t imageNo, const char* const* lines, std::uint8_t lineCount,
    const char* const* choices, std::uint8_t choiceCount,
    const char* const* hints, WindowChoiceCallback callback); // 窗口52只借用文本及回调。
std::int32_t Sub_49FFA0(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_4A00C0(std::int32_t index); // 商店回复仅索引0..1发送，其他返回原索引。
login::YobiAction* Sub_4A0140(const char* text); // 商店图号/标题/五行正文及买卖选择。
login::YobiAction* Sub_4B6E00(const char* text); // 窗口53买入商品及三组四行正文。
login::YobiAction* Sub_4B70D0(const char* text); // 窗口53出售商品、库存映射及占用。
login::YobiAction* Sub_4B74E0(const char* text); // 窗口53模式2五字段商品及指定物品文本。
login::YobiAction* Sub_4B77E0(const char* text); // 窗口53模式3独立详情，保留其他商品状态。
login::YobiAction* Sub_4B7AB0(const char* text); // 窗口53鉴定，库存映射保留未匹配物品哨兵20。
login::YobiAction* Sub_4B7EE0(const char* text); // 窗口53修理，与鉴定仅模式和提示不同。
void Sub_4B8310(); // 买入五行名称/价格/不可装备提示，保留原分行终止偏移。
void Sub_4B8670(); // 买入五行所选数量文本。
void Sub_4B86C0(); // 全20商品金额合计及元素7颜色。
void Sub_4B8730(); // 全20商品按上限分组计算占用数量。
void Sub_4B8780(); // 出售五行按S8选择槽取商品，不另检查use。
void Sub_4B8B10(); // 出售数量及上限>1时的加减元素可见性。
void Sub_4B8BE0(); // 全20选择槽的金额合计，元素7颜色固定0。
void Sub_4B8C50(); // 全20选择槽直接累计所选数，不做买入分组。
void Sub_4B8C90(); // 模式2五行，按指定物品数量与价格加合计比较。
void Sub_4B8FF0(); // 模式2数量，使用项直接重显加减元素。
void Sub_4B9090(); // 模式2合计写CF4A28，不写金额颜色。
void Sub_4B90E0(); // 模式2全部使用商品累计所选数。
void Sub_4B9110(); // 模式3首行详情名与数值，短名称的次行借593850。
void Sub_4B9270(); // 模式3三次读取D046C4，先两个显示值后乘详情单价。
void Sub_4B92C0(); // 模式3详情数值或全部28玩家物品的匹配数量。
std::int32_t Sub_4BBED0(std::int32_t index, std::int32_t state); // 商品图标，仅hover内bit4翻页。
std::int32_t Sub_4BC060(std::int32_t index, std::int32_t state); // 独立详情图标，bit4无hover门禁。
std::int32_t Sub_4BC230(std::int32_t index, std::int32_t state); // 出售选择行，接kind9交换/取消/拖动。
std::int32_t Sub_4BC660(std::int32_t index, std::int32_t state); // 背包20格命中、加入/拖动/翻页及两组绘制。
void Sub_4B9B10(std::int32_t selection); // 确认选择低U8为0才按模式发送，返回值不消费。
std::int32_t Sub_4B9F40(std::int32_t index, std::int32_t state); // 六模式确认生产，借用同源覆盖层。
std::int32_t Sub_4BD100(std::int32_t index, std::int32_t unusedState); // 确认覆盖层，事件直接读共享鼠标。
login::YobiAction* Sub_4BD540(const char* text); // 窗口54模式0，五行正文及八行说明。
login::YobiAction* Sub_4BD680(const char* text); // 窗口54模式1，固定十五项名称/双数值。
login::YobiAction* Sub_4BD810(const char* text); // 窗口54模式2，技能报价及旧窗刷新。
login::YobiAction* Sub_4BDAD0(const char* text); // 同创建流程，模式12显示种族。
void Sub_4BDD90(const char* text); // 模式3候选回复，仅更新存在的窗口54。
void Sub_4BDF70(const char* text); // 模式13候选回复，带种族条件及提示标记。
void Sub_4BE200(const char* text); // 模式4固定十槽回复，空名不终止后续槽。
void Sub_4BE3F0(WindowInstance* window); // 模式0技能栏/说明/花费，reqSlot按低S8。
void Sub_4BE600(WindowInstance* window); // 模式1遗忘映射/警告/回调，与原技能表同源。
void Sub_4BE930(WindowInstance* window); // 固定旧窗的五宠物名称/元素及技能分页刷新。
void Sub_4BEC20(WindowInstance* window); // 玩家索引对转候选映射，绑定真实选择回调。
void Sub_4BEF30(WindowInstance* window); // 技能槽计数/候选原序显示，先右侧再左侧刷新。
void Sub_4BF160(WindowInstance* window, std::int32_t scroll, std::int32_t count); // 模式0十行原技能。
void Sub_4BF2E0(WindowInstance* window, std::int32_t scroll, std::int32_t count); // 模式1十行协议名称/双数值。
void Sub_4BF460(WindowInstance* window, std::int32_t scroll); // 五行，存在性与内容索引不同。
void Sub_4BF620(WindowInstance* window, std::int32_t scroll); // 按候选槽数逐轮重读，不加十行钳制。
std::int32_t Sub_4BFBC0(std::int32_t index, std::int32_t state); // 模式0学习请求，bit2恒消费。
std::int32_t Sub_4BFCB0(std::int32_t index, std::int32_t state); // 分模式关闭/返回，音效后重读模式。
std::int32_t Sub_4BFF60(std::int32_t index, std::int32_t state); // bit400上一行，触边仍刷新。
std::int32_t Sub_4C0160(std::int32_t index, std::int32_t state); // 下一行，12/13固定左侧且目标index-1。
std::int32_t Sub_4C0380(std::int32_t index, std::int32_t state); // 上一页，音效后重读位置。
std::int32_t Sub_4C0590(std::int32_t index, std::int32_t state); // 下一页，先上限后负值修正。
std::int32_t Sub_4C0800(std::int32_t index, std::int32_t state); // 两侧滚轮；模式0/1只更新位置。
std::int32_t Sub_4C0A50(std::int32_t index, std::int32_t state); // 候选门禁/提示/发送，hintFlag不阻止发送。
std::int32_t Sub_4C0910(std::int32_t index, std::int32_t state); // 遗忘映射+1发送，发送后关闭窗口。
std::int32_t Sub_4C0BF0(std::int32_t index, std::int32_t state); // 选技能槽并显示元素0，不提前发送。
std::int32_t Sub_4C0CB0(std::int32_t index, std::int32_t state); // 技能选择/请求，发送后对话类型=-2。
std::int32_t Sub_4C0EA0(std::int32_t index, std::int32_t unusedState); // 已选技能内框，恒0。
std::int32_t Sub_4C0F20(std::int32_t index, std::int32_t unusedState); // 确认覆盖层，发原选槽文本后请求关闭。
login::YobiAction* Sub_4C5790(
    std::int32_t request, std::int32_t target, const char* text); // 两行正文后打开外观选择56。
std::int32_t Sub_4C5C30(std::int32_t index, std::int32_t state); // 图号原文本直接发送，不转义。
std::int32_t Sub_4C5D00(std::int32_t index, std::int32_t state); // class按版本上下界循环。
std::int32_t Sub_4C5DE0(std::int32_t index, std::int32_t state); // fifth分量0..4循环。
std::int32_t Sub_4C5EB0(std::int32_t index, std::int32_t state); // ones分量0..4循环。
std::int32_t Sub_4C5F80(
    std::int32_t button, std::int32_t row, char* const* unusedTexts,
    std::uint8_t unusedInputCount); // 类型37回复，图号转义后发完整button。
login::YobiAction* Sub_4C6060(
    std::int32_t buttons, std::int32_t request, std::int32_t target,
    const char* text); // 类型37内嵌图号及左侧14空格正文。
login::YobiAction* Sub_4C13B0(const char* text); // 窗口69商品，remaining非-1才可能下调limit。
login::YobiAction* Sub_4C3550(const char* text); // 窗口70七组正文/附加值，不下调limit。
void Sub_4C1700(); // 窗口69五条剩余量文本。
void Sub_4C17B0(); // 名称两行和价格，保留原分行空隙字节。
void Sub_4C1AA0(); // 五条已选数量。
void Sub_4C1B00(); // 总额与玩家晶石比较，写type7 color。
void Sub_4C1B70(); // 二十条选中商品占槽。
void Sub_4C1BC0(); // 首次/分页/数量脏标刷新。
void Sub_4C2060(std::int32_t value); // 低U8为0才按实时商品组包，命令0。
std::int32_t Sub_4C21B0(std::int32_t index, std::int32_t state); // 余额/背包门禁及确认预览生产。
std::int32_t Sub_4C2FA0(std::int32_t index, std::int32_t unusedState); // 窗口69确认覆盖层。
std::int32_t Sub_4C2B20(std::int32_t index, std::int32_t state); // 背包20格命中和绘制。
std::int32_t Sub_4C29E0(std::int32_t index, std::int32_t state); // 窗口69商品图标及四页提示。
std::int32_t Sub_4C2460(std::int32_t index, std::int32_t state); // 两窗口共用取消，发送command2。
std::int32_t Sub_4C2530(std::int32_t index, std::int32_t state); // 窗口69上一行。
std::int32_t Sub_4C25B0(std::int32_t index, std::int32_t state); // 下一行。
std::int32_t Sub_4C2630(std::int32_t index, std::int32_t state); // 上一页，真实入口为4C2630。
std::int32_t Sub_4C26B0(std::int32_t index, std::int32_t state); // 下一页。
std::int32_t Sub_4C2760(std::int32_t index, std::int32_t state); // 滚轮，仅回写分页。
std::int32_t Sub_4C27B0(std::int32_t index, std::int32_t state); // 减数量，无used门禁，事件尾清消息。
std::int32_t Sub_4C28C0(std::int32_t index, std::int32_t state); // 加数量，used/limit门禁，不清消息。
std::int32_t Sub_4C33B0(
    std::int32_t index, std::int32_t page, std::int32_t unused); // 保留caller第三实参。
void Sub_4C39A0(); // 窗口70五条剩余量文本。
void Sub_4C3A50(); // 名称两行和价格，独立分页/显示缓冲。
void Sub_4C3D40(); // 五条已选数量。
void Sub_4C3DA0(); // 总额与指定背包物品数量比较。
void Sub_4C3E20(); // 二十条选中商品占槽。
void Sub_4C3E70(); // 首次/分页/数量脏标刷新。
void Sub_4C4270(std::int32_t value); // 低U8为0才按实时商品组包，命令1。
std::int32_t Sub_4C43C0(std::int32_t index, std::int32_t state); // 交换物数量/背包门禁及确认预览。
std::int32_t Sub_4C50C0(std::int32_t index, std::int32_t unusedState); // 窗口70确认覆盖层。
std::int32_t Sub_4C4CB0(std::int32_t index, std::int32_t state); // 仅首格命中，仍绘完整20格。
std::int32_t Sub_4C4B70(std::int32_t index, std::int32_t state); // 窗口70商品图标及四页提示。
std::int32_t Sub_4C4680(std::int32_t index, std::int32_t state); // 两窗口共用按钮图，恒0。
std::int32_t Sub_4C46C0(std::int32_t index, std::int32_t state); // 窗口70上一行。
std::int32_t Sub_4C4740(std::int32_t index, std::int32_t state); // 下一行。
std::int32_t Sub_4C47C0(std::int32_t index, std::int32_t state); // 上一页。
std::int32_t Sub_4C4840(std::int32_t index, std::int32_t state); // 下一页。
std::int32_t Sub_4C48F0(std::int32_t index, std::int32_t state); // 滚轮，仅回写分页。
std::int32_t Sub_4C4940(std::int32_t index, std::int32_t state); // 减数量，窗口70独立脏标。
std::int32_t Sub_4C4A50(std::int32_t index, std::int32_t state); // 加数量，零上限采用交换文案。
std::int32_t Sub_4C54D0(
    std::int32_t index, std::int32_t page, std::int32_t unused); // 共用D303FC提示键。
std::int32_t Sub_4BA950(std::int32_t index, std::int32_t state); // 取消发送command2，发送后关闭。
std::int32_t Sub_4BAA50(std::int32_t index, std::int32_t state); // 批量加入出售选择，每轮固定重置/音效尾。
std::int32_t Sub_4BCEE0(std::int32_t index, std::int32_t state); // kind2拖放加入，hover模式门禁独立。
std::int32_t Sub_4BAD10(std::int32_t index, std::int32_t state); // bit400向前一行，触边仍刷新。
std::int32_t Sub_4BAE10(std::int32_t index, std::int32_t state); // bit400向后一行，触边仍刷新。
std::int32_t Sub_4BAF10(std::int32_t index, std::int32_t state); // bit2向前五行，音效后重读位置。
std::int32_t Sub_4BB010(std::int32_t index, std::int32_t state); // bit2向后五行，先上限后负值修正。
std::int32_t Sub_4BB130(std::int32_t index, std::int32_t state); // 共享滚轮更新并刷新，恒返回0。
std::int32_t Sub_4BB1D0(std::int32_t index, std::int32_t state); // 数量减少，兑换模式两轮背包占槽计算。
std::int32_t Sub_4BB6E0(std::int32_t index, std::int32_t state); // 数量增加，保留行价格与选择槽索引差别。
void Sub_4B6C50(
    std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // kind9出售拖动生命周期。
std::int32_t Sub_4B6DA0(std::int32_t sourceIndex); // 透传kind9注册返回。
void Sub_4BC150(); // 三次读取来源，撤销选择并刷新窗口53。
std::int32_t Sub_4A0280(std::int32_t index); // CB03D4命令表的选择回复。
std::int32_t Sub_4A0420(std::int32_t index); // CF47D8命令表的选择回复。
std::int32_t Sub_4A05C0(std::int32_t index); // 索引0..1分别发送1/2。
std::int32_t Sub_4A0700(std::int32_t index); // 仅索引0发送原字节1。
login::YobiAction* Sub_4A0300(const char* text);
login::YobiAction* Sub_4A04A0(const char* text);
login::YobiAction* Sub_4A0640(const char* text); // 技能三选项。
login::YobiAction* Sub_4A0790(const char* text); // 交易两选项。
login::YobiAction* Sub_4A0850(const char* text); // 给出/接收标签，正文取第四字段。
void Sub_49C4D0(); // 只清内嵌图号/计数及窗口36十元素可见性。
void Sub_49C510(
    std::uint8_t used, std::int32_t imageNo, std::uint8_t direction,
    std::int16_t x, std::int16_t y);
std::int32_t Sub_49CE70(std::int32_t index, std::int32_t state); // 六按钮与原位图号恢复。
std::int32_t Sub_49CF70(std::int32_t unusedIndex, std::int32_t state); // 输入行切换/文本行选择。
std::int32_t Sub_49DA30(
    std::int32_t button, std::int32_t row, char* const* texts, std::uint8_t inputCount);
void Sub_49D2E0(
    std::int32_t type, std::int32_t value, std::int32_t request,
    std::int32_t target, char* text); // 全部分派，保留同类续接后新建及原位文本。
void Sub_49D8F0(
    std::int32_t type, std::int32_t value, std::int32_t request,
    std::int32_t target, char* text); // 先发布同源对话状态，再生产并打开60/61/62。
void Sub_50E130(
    std::uintptr_t unusedSocket, std::int32_t type, std::int32_t value,
    std::int32_t request, std::int32_t target, char* text); // 50E110门禁后原共享尾入口。
extern std::int32_t gDialogValueCF4E5C; // 原第二协议参数；虚拟零S32，后续UI按位读取。
void Sub_49DDB0(const char* text, std::int16_t x, std::int16_t y); // 内嵌g,标记原字节解析。
login::YobiAction* Sub_49DF30(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t imageNo, std::int32_t buttons, std::int32_t request,
    std::int32_t target, char* text); // 原位文本处理，借用共享行表创建常规对话。
std::int32_t Sub_49E1D0(
    WindowInstance* window, std::int32_t button, std::int32_t row, std::int32_t unusedEvents);
login::YobiAction* Sub_49E400(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t imageNo, std::int32_t unusedButtons, std::int32_t request,
    std::int32_t target, const char* text); // 六治疗记录及第七合计项，共借CB1398行表。
login::YobiAction* Sub_49C600(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t imageNo, std::int32_t buttons, WindowDialogTextLine* lines,
    std::int32_t lineCount, std::int32_t lineHeight, std::int32_t firstSelectableLine,
    std::uint8_t inputCount, std::int16_t* inputY, std::int32_t inputCapacity,
    std::uint8_t distance, std::uint8_t options,
    WindowDialogReplyCallback reply, WindowDialogFrameCallback frame,
    std::uint32_t flags, std::uint32_t mode);

// 文本原型只借用源字节；实例仍通过WindowTextData拥有256B可写副本。
struct WindowTextPrototype {
    std::array<std::byte, 4> bytes00;
    const char* text;
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
    std::int32_t type;
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::int32_t visible;
    WindowElementPrototypeSource source;
    WindowElementCallback mouseCallback;
};

// 原48B可变原型的原生x64表示，36/37等同源原型须共用对象。
struct WindowPrototype {
    std::uint32_t state;
    std::int32_t layer;
    std::int32_t defaultX;
    std::int32_t defaultY;
    std::int32_t width;
    std::int32_t height;
    std::uint32_t flags;
    std::int32_t elementCount;
    WindowElementPrototype* elements;
    WindowCallback update;
    WindowCallback draw;
    WindowReleaseCallback release;
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
    WindowInputReference input;
    std::int32_t x;
    std::int32_t y;
    std::int32_t renderMode;
    std::int32_t fontType;
    std::int32_t color;
    std::array<char, 0x204> text; // 原+18文本起点至+21C前的字节区，未消费尾部不冻结语义。
    std::int32_t maxLine;
    std::int32_t maxCharPerLine;
    std::int32_t value224;
    std::int32_t encrypt;
};
extern WindowInputPrototype gDialogInputPrototype57E478; // 同源可写输入原型，57E698为容量成员。
extern WindowInputPrototype gWindow50InputPrototype580618; // 借Input唯一CC2080，供更新与编辑交互。
extern WindowInputPrototype gTitleInputPrototype57CCB8; // 原型当前context为编辑正文来源。
extern WindowInputPrototype gPetNameInputPrototype57DCF8; // 窗口15与16/17/18共用名称输入原型。
extern WindowInputPrototype gMailInputPrototype57E740; // 窗口22/23共用，借Input唯一D66CA8。
extern std::array<std::array<char, 11>, 10> gPetTechniqueCostsD66C34; // 原连续110B费用文本，491AA0生产。
std::int32_t Sub_48C1E0(std::int32_t index, std::int32_t state); // 窗口15完整五组交互。
std::int32_t Sub_48D4E0(); // 五状态局部快照叠加真实跟随位，透传发送结果。
std::int32_t Sub_48ECA0(std::int32_t index, std::int32_t state); // 宠物技能页六按钮。
std::int32_t Sub_48DD80(std::int32_t index, std::int32_t state); // 宠物详情名称/放地/选择/旋转。
std::int32_t Sub_48E430(std::int32_t index, std::int32_t state); // 宠物详细属性九按钮，9..13发送加点。
std::int32_t Sub_492430(std::int32_t index, std::int32_t state); // 图鉴列表打开选中详情。
std::int32_t Sub_492520(std::int32_t index, std::int32_t state); // 单行/十行滚动按钮。
std::int32_t Sub_492800(std::int32_t index, std::int32_t state); // WSP01：原950参数拖动换算；旧X代码1961。
std::int32_t Sub_492840(std::int32_t index, std::int32_t state); // 图鉴滚轮，恒返0。
std::int32_t Sub_492880(std::int32_t index, std::int32_t state); // 图鉴详情四种翻页与旋转。
void Sub_493A70(std::int32_t offset); // 设置窗口登出侧栏裁剪与两按钮显隐。
void Sub_493AF0(std::int32_t offset); // 设置窗口输入侧栏裁剪与三按钮显隐。
void Sub_493B70(std::int32_t offset); // 设置窗口音量侧栏，两同源数字分别测宽。
void Sub_493CB0(std::int32_t offset); // 设置窗口显示侧栏，读取当前模式S8。
std::int32_t Sub_493D60(std::int32_t value); // -1仅隐藏侧栏、清活动索引和位移。
std::int32_t Sub_493DB0(std::int32_t unusedValue); // 四侧栏减速展开与公共绘制尾。
void Sub_493E10(); // 设置写回/持久化后播放关闭音效。
std::int32_t Sub_493EE0(std::int32_t index, std::int32_t state); // 登出侧栏开关。
std::int32_t Sub_494000(std::int32_t index, std::int32_t state); // 输入侧栏，展开后保留元素11载荷别名。
std::int32_t Sub_494180(std::int32_t index, std::int32_t state); // 音量侧栏，生成两个type7同源文本。
std::int32_t Sub_494370(std::int32_t index, std::int32_t state); // 显示侧栏，展开后保留元素21载荷别名。
std::int32_t Sub_493E50(std::int32_t index, std::int32_t state); // 设置窗关闭标记，无直接音效。
std::int32_t Sub_4944A0(std::int32_t index, std::int32_t state); // 快捷键窗口25开关。
std::int32_t Sub_494550(std::int32_t index, std::int32_t state); // lO请求与场景10状态7。
std::int32_t Sub_494620(std::int32_t index, std::int32_t state); // 返回服务器选择，非场景10构造CryptInt零值。
std::int32_t Sub_494720(std::int32_t index, std::int32_t state); // 物品堆叠输入设置。
std::int32_t Sub_4947F0(std::int32_t index, std::int32_t state); // 光标切换，非零模式逐帧预览。
std::int32_t Sub_4948F0(std::int32_t index, std::int32_t state); // 快捷键模式，同源六项图号表。
std::int32_t Sub_4949C0(std::int32_t index, std::int32_t state); // 音乐减量，bit400触发。
std::int32_t Sub_494AE0(std::int32_t index, std::int32_t state); // 音乐增量。
std::int32_t Sub_494C10(std::int32_t index, std::int32_t state); // 音效减量，仍调用BGM音量入口。
std::int32_t Sub_494D30(std::int32_t index, std::int32_t state); // 音效增量。
std::int32_t Sub_494E60(std::int32_t index, std::int32_t state); // 声道设置，保留W32奇偶切换。
std::int32_t Sub_494F30(std::int32_t index, std::int32_t state); // 554644切换，提示仍复用声道文案。
std::int32_t Sub_495000(std::int32_t index, std::int32_t state); // 低U8切换后写W16扫描线设置。
std::int32_t Sub_4950D0(std::int32_t index, std::int32_t state); // 当前渲染模式提示/图号，恒返0。
std::int32_t Sub_495130(std::int32_t value); // 窗口25初始化，按分类绑定十行回调。
std::int32_t Sub_4951C0(std::int32_t value); // 窗口25说明/预设文本绘制及编辑焦点收尾。
std::int32_t Sub_4953B0(std::int32_t index, std::int32_t state); // 关闭并写回预设文本，不直接持久化。
std::int32_t Sub_495450(std::int32_t index, std::int32_t state); // 上一行，bit400。
std::int32_t Sub_495500(std::int32_t index, std::int32_t state); // 下一行，音效后重新取计数。
std::int32_t Sub_4955C0(std::int32_t index, std::int32_t state); // 上十行，音效先于位置修改。
std::int32_t Sub_495670(std::int32_t index, std::int32_t state); // 下十行，保留三阶段计数读取。
std::int32_t Sub_495750(std::int32_t index, std::int32_t state); // 窗口25滚轮，恒返0。
std::int32_t Sub_4957B0(std::int32_t index, std::int32_t state); // 分类0与十行复位。
std::int32_t Sub_4958D0(std::int32_t index, std::int32_t state); // 分类1与十行复位。
std::int32_t Sub_4959F0(std::int32_t index, std::int32_t state); // 分类2与十行复位。
std::int32_t Sub_495B10(std::int32_t index, std::int32_t state); // 分类3只换回调，不复位行图号。
std::int32_t Sub_495C20(std::int32_t index, std::int32_t state); // 预设文本备份、初始化及同源焦点。
extern std::int8_t gHotkeySelectedCC4B20; // S8选中记录，495130初始化为-1。
extern WindowInputPrototype gHotkeyInputPrototype57DF48; // 借24项InputContext中的当前编辑项。
extern std::int8_t gSettingsPanelCC4234;
extern std::int32_t gSettingsOffsetCA02DC;
extern std::int32_t gSettingsStepD2D610;
extern std::array<char, 12> gSettingsMusicTextD3041C; // 5689C0借用，音乐音量S32的%d文本。
extern std::array<char, 12> gSettingsEffectTextD2EA64; // 5689B8借用，音效音量S32的%d文本。
std::int32_t Sub_48F380(std::int32_t index, std::int32_t state); // 共享名称输入焦点。
std::int32_t Sub_48F410(std::int32_t index, std::int32_t state); // 三详情页切换。
std::int32_t Sub_48F6C0(std::int32_t index, std::int32_t state); // 描述、技能换位与拖动。
extern std::array<char, 256> gTitleDraftD45FC0; // 窗口9草稿，区别于玩家ownTitle和原型内嵌文本。
extern std::int32_t& gTitleModeCF4E50; // 借既有登录窗口状态槽0，0选择/1删除。
extern std::int32_t& gTitlePageCF4E54; // 借既有登录窗口状态槽1，重置/刷新/滚动同源。
std::int32_t Sub_477540(std::int32_t unusedIndex, std::int32_t state); // 草稿编辑焦点。
std::int32_t Sub_477600(std::int32_t index, std::int32_t state); // 六行称号选择/删除。
std::int32_t Sub_477710(std::int32_t index, std::int32_t state); // 完整按钮分派与背景清模式。
std::int32_t Sub_4782A0(std::int32_t unusedIndex, std::int32_t state); // 称号滚轮，恒返0。
std::int32_t Sub_4782E0(std::int32_t index, std::int32_t state); // WSP01：原称号拖动页码；旧X代码1959。
extern WindowInputPrototype gOwnProfileInputPrototype57CEE8; // 窗口58三项/正文的可写输入原型。
extern WindowInputPrototype gOwnProfileInputPrototype57D118;
extern WindowInputPrototype gOwnProfileInputPrototype57D348;
extern WindowInputPrototype gOwnProfileInputPrototype57D578;
std::int32_t Sub_4792D0(std::int32_t index, std::int32_t state); // 窗口58元素5..8编辑焦点。

union WindowUserState {
    std::uint32_t value; // 原+18的数值路径。
    WindowValueCallback callback; // 原窗口42的+18，禁止截成W32。
};

struct WindowState {
    std::uint32_t flags; // 原+00，独立于实例state。
    std::int32_t x;
    std::int32_t y;
    WindowInstance* window; // 原+0C；不沿用原记录0x54步长。
    std::uint32_t value10;
    std::uint32_t value14;
    WindowUserState userState; // 原+18；后续生产者按实际用途激活成员。
    std::uint32_t actionFlags1C; // 原+1C，窗口42数值面板消费低U8。
    std::uint32_t value20;
    std::uint32_t value24;
    std::uint32_t value28;
    std::array<std::byte, 36> bytes2C; // 未消费区，不据此冻结未知指针映射。
    std::uint32_t marker50;
};

inline constexpr std::size_t kWindowCount = 71;
// 两表已按原地址共享绑定；主表的其余元素与回调依赖见WindowPrototypeDependencies。
extern std::array<WindowPrototype*, kWindowCount> gWindowPrototypes57C130;
extern WindowPrototype& gWindowPrototype54E2A0; // 4C93A0直接比较的原对象，不追随表槽改指向。
extern WindowPrototype& gWindowPrototype54E240;
extern WindowPrototype& gWindowPrototype54E330;
extern std::array<WindowDragPrototype*, kWindowCount> gWindowDragPrototypes57C24C;
extern std::array<WindowInstance*, kWindowCount> gWindows; // 原C8B094。
extern std::array<WindowState, kWindowCount> gWindowStates; // 原C9A9B0。
extern WindowInstance* gCurrentWindow; // 原CF4694。
extern WindowState* gCurrentWindowState; // 原CF4684。
extern std::uint8_t gValueD67BE0; // 原多个UI生产/消费入口共用，不并入窗口记录。
extern std::array<char, 256> gStallMessageC8A978; // 摆摊结果提示，4D0230消费同一文本。
extern std::array<char, 17> gStallNameD67BE4; // 原strncpy16及D67BF4终止字节。
extern std::uint8_t gStallPendingD67C31; // 请求与回复共用等待标记。
extern std::int32_t gStallMessageModeD67C34;
extern std::int32_t gStallMessageValueD67C38;
void Sub_4CE700(std::int32_t result); // 开摊结果：玩家标志、名字及窗口67首元素。
void Sub_4CE8B0(std::int32_t result); // 收摊结果：两路均先清玩家标志/等待/门禁。
void Sub_4CE930(std::int32_t result); // 购买结果：浏览记录和两并行表原序搬移，或显示原提示。
void Sub_4CF8A0(world::detail::PlayerItemRecord* item, const char* text); // 固定32行描述及前一行回扫。
std::int32_t Sub_4CECB0(); // 选择索引直接查询后发购买请求，返回提示字节数或原负值。
std::int32_t Sub_4CD420(std::int32_t index, std::int32_t state); // 两等待门禁、关闭标记和原图形快照。
void Sub_4CDE00(); // 67/68共用释放，条件发送后无条件SE57。
std::int32_t Sub_4CDB10(std::int32_t unusedValue); // 两类摊位预览、失效上架剔除与提示绘制。
std::int32_t Sub_4D27C0(std::int32_t value); // 当前选择追加到自身上架表，先清选择最后发布数量。
std::int32_t Sub_4CD4F0(std::int32_t value); // 共用初始化、双滚动/输入同步、文本绘制及金额尾。
std::int32_t Sub_4D2080(std::int32_t unusedIndex, std::int32_t state); // 固定元素8说明编辑，hover不消费。
std::int32_t Sub_4D2170(std::int32_t unusedIndex, std::int32_t state); // 固定元素6名称编辑，同源Input对象。
extern std::array<char, 16> gStallMoneyText57B440; // 原可变金额串，止于下一数字原型57B450。
std::int32_t Sub_4CDE40(std::int32_t index, std::int32_t state); // 上架列表向前，bit400动作。
std::int32_t Sub_4CDFA0(std::int32_t index, std::int32_t state); // 上架列表向后，音效后重读滚动参数。
std::int32_t Sub_4CE0F0(std::int32_t unusedIndex, std::int32_t state); // 上架列表滚轮，始终返回0。
std::int32_t Sub_4CE1F0(std::int32_t index, std::int32_t state); // 宠物候选向前，非宠物模式仍可消费bit400。
std::int32_t Sub_4CE2B0(std::int32_t index, std::int32_t state); // 宠物候选向后，最大页1。
std::int32_t Sub_4CE370(std::int32_t unusedIndex, std::int32_t state); // 宠物候选滚轮，始终返回0。
std::int32_t Sub_4CE3F0(std::int32_t index, std::int32_t state); // 类别页签及七项候选控件原序显隐。
std::int32_t Sub_4CFE70(); // 槽序上架组包、转义发送及同源保存记录回写，返回0。
std::int32_t Sub_4D0230(std::int32_t index, std::int32_t state); // 共用提示覆盖层，只返回hover位。
std::int32_t Sub_4CF940(std::int32_t index, std::int32_t state); // 自身/浏览主要按钮，禁用与普通收尾分离。
std::int32_t Sub_4CE8A0(); // 收摊请求，透传原发送返回。
WindowInstance* Sub_4CED50(); // 显示购买确认，返回入口窗口。
void Sub_4CED60(
    std::int32_t status, std::int32_t target, const char* stallName,
    const char* description, const char* itemPayload, const char* petPayload); // 浏览区完整生产后打开68。
WindowElement* Sub_4CFC40(); // 物品定价输入初始化，返回末次取得的元素表。
void Sub_4CFCA0(std::int32_t petIndex); // 固定六装备槽检查后进入宠物定价。
std::int32_t Sub_4CFD70(); // 删除上架项，保持标记/数量/搬移/选择与页回退顺序。
std::int32_t Sub_4D2260(std::int32_t unusedIndex, std::int32_t state); // 完整定价键盘和鼠标交互，返回hover位。
std::int32_t Sub_4D2840(std::int32_t unusedIndex, std::int32_t state); // 购买确认及物品/宠物详情，二次读模式。
std::int32_t Sub_4D1D50(std::int32_t index, std::int32_t state); // 玩家背包20格，首命中交互但完整绘制。
std::int32_t Sub_4D04A0(std::int32_t index, std::int32_t state); // 上架/浏览物品行，固定记录与价格重读分离。
std::int32_t Sub_4D16F0(std::int32_t index, std::int32_t state); // 宠物候选，逐项重读候选页并保持CryptInt顺序。
std::int32_t Sub_4D1C00(
    std::int32_t petIndex, std::int32_t attribute, std::int32_t x, std::int32_t y); // 候选属性条，透传末绘制返回。
std::int32_t Sub_4D0990(
    std::int32_t x, std::int32_t y, std::int32_t petIndex, std::int32_t skillPosition); // 浏览宠物技能，使用独立浏览记录。
std::int32_t Sub_4D0A80(
    std::int32_t x, std::int32_t y, std::int32_t index); // 完整浏览宠物详情，复用D67B84预览。
std::int32_t Sub_4D0EB0(std::int32_t index, std::int32_t state); // 上架/浏览宠物行，末尾金额两次读取及详情分派。
extern std::int32_t gDragItemIndex584DA4; // 4FB8F0重置与旧14的4FAEF0拖动生产共用。
extern std::int32_t gDragImageNoD67D24;
extern std::int32_t gItemDragXCC4880;
extern std::int32_t gItemDragYCC4884;
extern std::int32_t gDraggingCA0B28;
extern std::int32_t gValue58589C; // 原初值1，命中描述门禁与物品拖动重置共享。
extern void* gFocusedElementDataD45F94; // 原异型元素载荷地址，仅借用，不是InputContext。
extern std::int32_t gMouseValue57C110;
extern std::int32_t gDragSourceIndex57C114;
extern WindowDragCallback gDragCallback57C118;
extern std::int16_t gDragX57C11C;
extern std::int16_t gDragY57C11E;
extern std::int32_t gFocusWindowId57C120;
extern std::int32_t gFocusElementIndex57C124;
extern std::int32_t gMousePhase57C128;
extern std::int32_t gCurrentElementIndex57C12C; // 原映像初值-1，不等同焦点元素。
extern std::int32_t gCurrentWindowIdD67A1C;
extern std::uint32_t gWindowHasFocusCA0AB4; // 元素回调成功后占用，独立于焦点窗口id。
extern WindowInputPrototype gChatInputPrototype57E970;
extern WindowInputPrototype gTradeInputPrototype5803D0; // 借用input唯一CC21E8上下文。
extern std::int32_t gChatWindowModeD66E04; // 原独立S32，窗口35/57选择与设置文件共用。
extern std::int32_t gLogDisplayGate593F84; // 窗口66与旧402020/4021A0共用，非恒1。
extern std::uint8_t gValue57DA28; // 原独立U8，初值FF，设置文件与物品窗口共用。
extern std::int32_t gInventoryHover57DA2C; // 普通背包悬停槽，初值-2，仅488FB0读写。
extern std::int32_t gEquipmentHover57DA68; // 装备页独立悬停槽，初值-2，仅489760读写。
extern std::array<char, 12> gInventoryMoneyTextCB086C; // 488E60格式化，type7载荷借用。
extern std::array<char, 12> gInventoryValueTextCC3ECC; // 488E80格式化FF02CC，独立于输入区。
extern std::uint8_t gValue57E42C; // 原独立U8，初值FF，设置文件与宠物窗口共用。
extern std::int32_t gBattleItemHoverCF49B0; // 窗口27每帧清-1，499C30发布提示槽。
extern std::int32_t gBattleItemPageCB01E0; // 窗口27物品提示页，同源于两战斗交互。
extern std::uint8_t gBattleItemTargetFlagCC4D40; // 499C30发布，496B90/497770后续消费。
extern std::array<char, 12> gBattleMoneyTextCF469C; // 499B00格式化，type7原型借用。
extern std::array<char, 12> gBattleValueTextC8A960; // 499B20格式化，独立于普通库存文本。
extern std::int32_t gValueCF4AAC; // 原设置文件回填与多个UI入口共用的数值。
extern std::int32_t gValueCF4690;
extern std::int32_t gValueCA0440;
extern std::int32_t gValueD67B38;
extern login::YobiAction* gWindowActionCA0174; // 原475AA0返回的借用对象，不是W32标志。
extern login::YobiAction* gWindowActionD2E928; // 另一窗口动画引用，479610仅清槽。
extern login::YobiAction* gPreviewActionD67B84; // 原404AE0生产，重开只清引用不释放。
extern std::int32_t gValueD67AF8; // 原窗口操作计数，不随指针扩宽。
extern std::uint8_t gValueD67AFC;
extern std::uint8_t gValueD67AFD;
extern std::uint8_t gValueD67AFE;
extern std::uint8_t gValueD67B24;
extern std::uint8_t gValue5F9754; // 原多窗口/物品UI共享字节，观察进入可能清零。
extern std::int32_t gUsingSkillIndexD2F32C; // 技能请求索引，不与窗口当前选中槽合并。
extern std::int32_t gUsingSkillLevelCA0AC4;
extern std::int32_t gSkillTargetKindCF4A10;
extern std::int32_t gSkillTargetCountCA0004;
extern std::int32_t gSkillTargetPageC8A974;
extern std::int32_t gSkillTargetPageCountCA0178;
extern std::int32_t gSkillSendModeC8A970;
extern std::int32_t gSkillSelectedTargetCC4B1C;
extern std::int32_t gSkillWindowModeD672AC;
extern std::int32_t gSkillWindowIndexD2EA60;
extern std::array<std::array<char, 17>, 10> gSkillTargetNamesCB0550; // 独立原字节名单，非LY表。
extern std::array<std::int32_t, 10> gSkillTargetKindsD2F75C;
extern std::array<const char*, 10> gSkillTargetNameViewsD45F98;
extern std::array<std::uint8_t, 10> gSkillTargetColorsCB1DE4;
extern std::uint8_t gSkillTargetPendingC8AF5F;
extern std::array<char, 20> gSkillTargetNameD2F748; // 选择发送前复制；后续属性/技能消费者共享。
extern std::int32_t gChatWidthUnitsC9A930; // 原聊天宽度单位，输入滚动/聊天绘制共用。
extern std::int32_t gChatWidthRequestC9A934; // 原宽度调整请求，独立于实际宽度。
extern std::int32_t gChatHeightUnitsC9A938; // 原聊天可见高度的字体9行高倍数。
extern std::int32_t gChatHeightRequestC9A93C; // 原高度调整请求，消费后清零。
extern std::int32_t gChatScrollOffsetC9A940; // 原相对共享写指针的回看条数。

// 交易UI唯一原字节区，非玩家背包/宠物owner；未知字段不冻结语义。
extern std::array<char, 2 * 20 * 0xC4C> gTradeItemsD460D0;
extern std::array<char, 2 * 5 * 0x5FC> gTradePetsC9C108;
extern std::array<char, 5 * 0x600> gPetStorageD64E18; // 外部存放宠物，4A84A0原回复区。
extern std::array<char, 2 * 0xFBF8> gItemDetailStorageCC4D58; // 每组首20条C44物品、末W32金额，间隔未冻结。
std::int32_t ReadLegacyStorageSideD67618() noexcept; // 旧72借读既有存放模式，不复制私有状态。
const std::byte* BorrowLegacyMessageRecordsD30138() noexcept; // 旧73只读借现代50同源2C0H字节区，四条B0H。
extern std::array<char, 20 * 0xC58> gShopItemsCF4FE0; // 买卖共用原字节记录，字段见窗口53 SPEC。
extern std::int32_t gShopItemCountCB1E10; // 买入实际数量/出售固定20，与回复商品数独立。
extern std::int32_t gShopReplyItemCountCF4B58; // 出售回复中实际读取的商品数。
extern std::int32_t gShopImageD2DAD0;
extern std::int32_t gValueD2D608; // 商店和其他窗口共用数值，不是地址。
extern std::int32_t gShopDialogTypeCC3EC8;
extern std::int32_t gShopValueCF467C;
extern std::array<std::int8_t, 20> gShopInventoryMapD2DAB8; // 背包8..27到商品首匹配索引。
extern std::array<char, 20> gShopSelectedSlotsCB03DC; // 原五次W32清-1，消费者按S8槽读取。
extern std::array<char, 32> gShopTextCB0184; // 模式2字段7反转义结果，不套商品名回退。
extern std::array<char, 0xC38> gShopDetailCC3210; // 名称/32行说明/六W32同源，见窗口53 SPEC。
extern std::int32_t gShopValueCAFEA4;
extern std::int32_t gShopValueD046C4;
extern std::int32_t gShopValueD672D4;
extern std::int32_t gShopTooltipIndex580844; // 玩家/商品/详情提示编号，原装载初值-1。
extern std::int32_t gShopMessage580848; // 四行正文组的负编号，原装载初值-1。
extern std::int32_t gShopInventoryHover58084C; // 原装载值-2，父初始化不重置的背包命中缓存。
extern std::int32_t gShopTooltipPageD67B8C;
extern std::int32_t gShopTotalD2DAFC;
extern std::int32_t gShopValueC8AF74; // 各模式计算的数量值，不与总金额合并。
extern std::int32_t gShopScrollCC488C;
extern std::array<char, 55> gShopPriceTextsD673EC; // 五行、11B步长；type7原型借用。
extern std::array<char, 55> gShopCountTextsD2D988; // 五行所选数量文本，与价格区分离。
extern std::array<char, 48> gShopTotalTextCC4B2C; // 金额或模式相关数值的共用显示文本。
extern std::array<char, 16> gShopMoneyTextCC1A00; // 玩家金额文本，573D18 type7借用；尾4B别名旧63页号CC1A0C。
extern std::int32_t gShopAvailableCF4A24; // 模式2指定物品数量，装载0，窗口初始化再写-1。
extern std::array<char, 20> gShopValueTextCF4A28; // 模式2/3数值显示，573D20借用。
extern std::array<std::int32_t, 5> gShopRowKeysC9A954; // 多窗口共用的行编号缓存，非地址。
extern std::uint8_t gShopDragFlagD67B90; // 非零仅清自身，零分支移除选择；原虚拟零初值。
extern std::int32_t gQuantityLimit581C94; // 原装载值999，商店及其他数量窗口同源上限。
extern WindowValueCallback gConfirmationCallbackC8B088; // 原生借用函数指针，确认/取消各自重读。
extern std::array<WindowConfirmationItem, 20> gConfirmationItemsCB1E40;
extern std::array<WindowConfirmationAmount, 2> gConfirmationAmountsCB0F90;
extern std::array<char, 100> gConfirmationLineCB1320;
extern std::array<char, 104> gConfirmationLineCB1210;
extern std::array<char, 256> gConfirmationAcceptHintCC3FCC; // 4C0F20也借用，不建窗口私有副本。
extern std::array<char, 256> gConfirmationCancelHintD45D70;
extern std::int32_t gWindow54DialogTypeCB2224; // 创建快照581800，更新/释放共同读取。
extern std::int32_t gWindow54ImageCC18C4; // 协议字段1的图号，非地址。
extern std::int32_t gWindow54ValueCC2E68; // 4BD540字段6，4D6A60也消费。
extern std::int32_t gWindow54ValueCF4A3C; // 4BD540字段7，4D6A60也消费。
extern std::int32_t gWindow54ValueCB01E4; // 创建尾清零的独立数值，不与CB01E8合并。
extern std::int32_t gWindow54ValueCB01E8;
extern std::uint8_t gWindow54ModeC8AF64;
extern std::int32_t gWindow54LifecycleD67434; // 值2阻止释放回调重置当前对话类型。
extern std::array<WindowDialogTextLine, 15> gWindow54NamesC8AAA0; // 4BD680复制、4BF2E0消费。
extern std::array<std::int32_t, 15> gWindow54ValuesCB01A4; // 两表也被4D6EA0消费，保持同源。
extern std::array<std::int32_t, 15> gWindow54ValuesCB09D8;
extern std::array<Window54SkillRecord, 16> gWindow54SkillsD2EAB8; // 600H，到D2F0B8前结束。
extern std::int32_t gWindow54SkillScrollCC31F4;
extern std::int32_t gWindow54MessageC9FCE0; // 此地址也用于交易宠物区结束比较，不属宠物数据。
extern std::int32_t gWindow54HoverD2DAD8;
extern std::int32_t gWindow54SelectedD2DC04;
extern std::array<char, 12> gWindow54AmountTextD45F78; // 下一独立槽从D45F84起。
extern std::array<char, 110> gWindow54ValueTextsCB035C; // 10行，每行11B。
extern std::array<char, 110> gWindow54ValueTextsC8B318;
extern std::array<char, 55> gWindow54SkillValueTextsCF4978; // 5行，每行11B。
extern std::array<char, 55> gWindow54SkillPriceTextsCF493C;
extern std::array<Window54PetOption, 5> gWindow54PetOptionsD67448; // 4D7C20/4D8230也读，保持同源。
extern std::array<std::int8_t, 5> gWindow54PetMapCF4AB0; // 4C0A50原CF4A6B基址含元素索引69的别名。
extern std::int32_t gWindow54TribeCA0B2C;
extern std::int32_t gWindow54PetChoiceC8AA90; // 已选协议候选，不是原生地址或玩家宠物槽。
extern std::int32_t gWindow54PetScrollCC31DC;
extern std::array<Window54TechniqueSlot, 10> gWindow54TechniqueSlotsD66E38; // 398H，其他窗口同源。
extern std::int32_t gWindow54ReplacementC8B08C; // 4C0BF0发布、4C0F20消费的技能槽索引。
extern std::array<std::int8_t, 15> gWindow54SkillMapCC4D04; // 技能索引对扣除前序空名后的协议映射。
extern std::array<char, 12> gWindow54MoneyTextC9A9A4; // 止于C9A9B0窗口状态表之前。
extern std::array<WindowCrystalItem, 20> gWindow69ItemsCB2230;
extern std::array<WindowCrystalItem, 20> gWindow70ItemsD30428;
extern std::int32_t gWindow69ValueD67B98;
extern std::uint8_t gWindow69FlagD2EA5B;
extern std::int32_t gWindow69ImageCB01F0;
extern std::int32_t gWindow69DialogTypeD2D810;
extern std::int32_t gWindow70ValueD67BA4;
extern std::uint8_t gWindow70FlagD673D8;
extern std::int32_t gWindow70ImageD2E43C;
extern std::int32_t gWindow70ValueD6791C; // 第十协议字段，仍S32。
extern std::int32_t gWindow70DialogTypeD45D6C;
extern std::int32_t gWindow69ScrollD2D604;
extern std::int32_t gWindow70ScrollCC4B68;
extern std::int32_t gWindow69DirtyD67B9C;
extern std::int32_t gWindow70DirtyD67BA8;
extern std::int32_t gWindow69CachedScroll580858; // 原文件初值-1。
extern std::int32_t gWindow70CachedScroll580868;
extern std::int32_t gWindow69TotalD45E70;
extern std::int32_t gWindow70TotalCB0F74;
extern std::int32_t gWindow69SlotsCA0AC8;
extern std::int32_t gWindow70SlotsCF4E44;
extern std::array<char, 55> gWindow69PriceTextsD2D7CC;
extern std::array<char, 55> gWindow70PriceTextsCF49C4;
extern std::array<char, 55> gWindow69CountTextsD2EA24;
extern std::array<char, 55> gWindow70CountTextsCC4848;
extern std::array<char, 12> gWindow69TotalTextCC48B0;
extern std::array<char, 12> gWindow70TotalTextD2D804;
extern std::int32_t gWindow69Tooltip580850; // 悬停编号，原文件初值-1。
extern std::int32_t gWindow70Tooltip580860;
extern std::int32_t gWindow69Message580854; // NPC正文组，原文件初值-1。
extern std::int32_t gWindow70Message580864;
extern std::int32_t gWindow69TooltipPageD67B94;
extern std::int32_t gWindow70TooltipPageD67BA0;
extern std::int32_t gWindow69InventoryHover58085C; // 原文件初值-2；背包命中变化才更新。
extern std::int32_t gWindow70InventoryHover58086C;
extern std::array<char, 12> gWindow69BalanceTextD2E448; // 完整S32格式化前缀，不冻结未消费尾区。
extern std::array<char, 12> gWindow70BalanceTextCF3F14;
extern WindowValueCallback gWindow69ConfirmCallbackCF4E48;
extern WindowValueCallback gWindow70ConfirmCallbackCF4AA4;
extern std::array<WindowConfirmationItem, 20> gWindow69ConfirmItemsD2E458;
extern std::array<WindowConfirmationItem, 20> gWindow70ConfirmItemsCB0AE8;
extern std::array<WindowConfirmationAmount, 2> gWindow69ConfirmAmountsD2E2B8;
extern std::array<WindowConfirmationAmount, 2> gWindow70ConfirmAmountsD2E290;
// 固定原文及NUL的已消费前缀，不据此冻结未消费尾区容量。
extern std::array<char, 50> gWindow69ConfirmHeadingCC3F68;
extern std::array<char, 48> gWindow70ConfirmHeadingCF4A40;
extern std::array<char, 21> gWindow69ConfirmQuestionCC3F04;
extern std::array<char, 21> gWindow70ConfirmQuestionCF4ABC;
extern std::array<char, 11> gWindow69ConfirmAcceptD2E81C;
extern std::array<char, 11> gWindow70ConfirmAcceptD2DD6C;
extern std::array<char, 11> gWindow69ConfirmCancelCB076C;
extern std::array<char, 11> gWindow70ConfirmCancelCC48BC;
extern std::int32_t gStorageValueCC4218; // 存放窗口41每帧清零、元素13绘制的数值。
extern std::array<char, 22> gStorageAmountTextsCB1DF8; // 两金额文本起点相隔11B，571150/158借用。
extern const std::array<std::int32_t, 2> gStorageTitleImages54F4E8;
extern const std::array<std::int32_t, 2> gStoragePanelImages54F4F0;
extern std::int32_t gTradeValueCC4224;
extern std::int32_t gTradeValueCF4688;
extern std::int32_t gTradeValueD046D0;
extern std::int32_t gTradeValueD2E2B0;
extern std::uint8_t gTradeValueD67B80;
extern login::YobiAction* gTradeWindowActionD67B7C; // 原43窗口展开动画，仅借用。
extern std::int32_t gTradeValueCF4A1C; // 原第一个页签数值索引。
extern std::int32_t gTradeValueD673E0; // 原另一页签，均为数值索引。
extern std::int32_t gTradeValueCC4C94; // 原四个物品/宠物计数。
extern std::int32_t gTradeValueC8AF68;
extern std::int32_t gTradeValueCB03CC;
extern std::int32_t gTradeValueCF4AB8;
extern std::int32_t gTradeValueD67CB8; // 交易阶段相关数值，不是地址。
extern std::int32_t gTradeValueD67CBC;
extern std::int32_t gTradeValueCC3EC4; // 所选物品数值索引，4E5620写-1。
extern std::int32_t gTradeValueD66E08; // 所选图号数值。
extern std::int32_t gTradeValueCC2344; // 交易选择状态。
extern std::array<char, 17> gTradeNameD2D654;
extern std::array<std::array<char, 17>, 10> gWindowListNamesCF3F20;
extern std::int32_t gWindowListCountCF46A8;
extern WindowListSelectionCallback gWindowListCallbackCB1E14; // 49EF60借用名单文本并弃返回。
extern std::int32_t gWindowListOffsetCC3EF4;
extern std::array<char, 256> gHoverTextD68AE8; // 已识别地址跨度，不是新增文本长度限制。
extern std::array<char, 24> gStatusHpTextD40B18; // 状态栏type7共用原文本跨度，非新截断门槛。
extern std::array<char, 28> gStatusFpTextD45D50;
extern std::int32_t gHoverObjectValueD68AC8; // 507140命中伴随值，4731E0后续须共享。
extern std::array<char, 260> gHoverTitleD68BF4; // 至既有鼠标D68CF8之前的原字节区。
extern std::int32_t gHoverTitleValueD69748; // 副标题伴随值，非主描述值。
extern std::int32_t gWindow6HoverD2EA70; // 473840的case0写U8(state)&1，4731E0严格判1。
extern std::int32_t gValueCC4D34; // 4D3020保存x/y数值，非CryptInt对象。
extern std::int32_t gValueCC4D3C;
extern std::int32_t gSelectedItemCB1DF4;
extern std::int32_t gSelectedItemCB0A44;
extern std::int32_t gSelectedItem57EE0C; // 原装载初值-1，窗口51只做单向迁移。
extern std::uint8_t gSkillPairIndexD67AD8; // 配方查询按S8解释。
std::int32_t ReadSkillRequestD64E13() noexcept; // 旧技能请求读取同源低S8，4FF650实际写者已接。
void WriteSkillRequestD64E13(std::uint8_t value) noexcept; // 4FF650四原写点共用私有唯一D64E13。
std::uint32_t ReadSkillExtraDelayCF46C4() noexcept; // 新旧技能面板共用额外时长。
void WriteSkillExtraDelayCF46C4(std::uint32_t delay) noexcept;
std::int32_t ReadGatherResultD2D820() noexcept; // 旧选择39借用窗口12同源回复。
std::int32_t ReadGatherResultImageCF3FF0() noexcept;
const char* BorrowGatherResultNameD2DA94() noexcept;
const char* BorrowGatherResultSuffixCB1E1C() noexcept;
const char* BorrowGatherResultDetailsCC190C() noexcept; // 借用NUL文本，不复制到另一结果缓存。
std::int32_t ReadLegacySkillResultD66E2C() noexcept; // 旧选择37与窗口13共享回复结果。
const char* BorrowLegacySkillSummaryCC18CC() noexcept;
const char* BorrowLegacySkillDetailsCB0FDC() noexcept;
std::int32_t ReadLegacyItemWorkResultCB05FC() noexcept;
const char* BorrowLegacyItemWorkSuffixD2DADC() noexcept;
const char* BorrowLegacyItemWorkDetailsD2DB00() noexcept;
std::int32_t ReadLegacyItemWorkSkillD67CF0() noexcept; // 新旧加工入口与结果回复同源技能类别。
void WriteLegacyItemWorkSkillD67CF0(std::int32_t skillId) noexcept;
std::int32_t ReadLegacyCraftResultCC46D8() noexcept;
std::int32_t ReadLegacyCraftImageCF4B60() noexcept;
const char* BorrowLegacyCraftNameCF46D4() noexcept;
void WriteLegacyCraftNameCF46D4(const char* name); // 旧26发送前NUL复制，沿既有60B结果名称。
const char* BorrowLegacyCraftSuffixD672B4() noexcept;
const char* BorrowLegacyCraftDetailsD2E92C() noexcept;
extern std::int32_t gRecipeIndex5846BC; // 原装载初值-1。
extern std::array<std::uint8_t, 20> gTradeItemSlotsCF3AE8; // 比较按S8，写入只取低U8。
extern std::array<std::uint8_t, 28> gTradeItemMarksD2D61C;
extern std::array<std::uint8_t, 5> gTradePetSlotsCC4D28; // 末项即CC4D2C，按S8读槽号。
extern std::array<std::uint8_t, 5> gTradePetMarksCF3AFC; // 末项即CF3B00，不拆独立标量。
extern std::uint8_t gTradePhaseCAFEC4;
extern std::uint8_t gTradeStepD3040C; // 位移按S8读，递减只写U8。
extern std::uint8_t gTradeValueD2EA20;
extern std::uint8_t gTradeValueD6742C;
extern std::uint8_t gTradeValueC8AF81;
extern std::uint8_t gTradeTabCC4D1A; // 对方页签，不套用己方窗口初始化重置。
extern std::int32_t gTradeHoverSlotC8B314; // 对方提示槽缓存，非地址。
extern std::int32_t gTradeHoverSlotC8AA78; // 己方已选物品提示槽缓存。
extern std::int32_t gTradeHoverSlotCB0A4C; // 己方自有物品提示槽缓存。
extern std::int32_t gPetDragModeD2EA1C; // 48C130发布，回调分别采点击和绘制模式。
extern std::int32_t gItemDragModeCC4C9C; // 48BF10发布，仅门禁48BC90的message2请求。
extern std::int32_t gItemMoveSourceCF46B0; // 4886E0发布，4885A0读取的来源槽号。
extern std::int32_t gItemMoveTargetD2DBF8; // 4886E0发布，4885A0读取的目标槽号。
extern std::int32_t gValueD67B00; // 原数值状态，47D490清零；非地址。
extern std::array<char, 12> gNumberWindowTextCC4D1C; // 原CC4D1C至宠物槽CC4D28前的文本区。
extern std::int32_t gTradeScrollCF3B0C;
extern std::int32_t gTradeScrollCC4C90;
extern std::int32_t gTradeScrollD67430;
extern std::int32_t gTradePreviewCC2E64; // 原预览分类数值，非地址。
extern std::uint8_t gTradeTooltipPageCB10D4; // 物品提示页，绘制按S8读取。
extern std::uint32_t gWindowActionTick62B5AC; // 跨UI动作共用，不是交易私有计时槽。
extern std::int32_t gTooltipColorD67D30; // 分段文本跨行共享颜色，尾串取低S8。
extern std::int32_t gTooltipSourceD303FC; // 原提示源分类/编号缓存，非地址。
extern std::uint16_t gTooltipXCF46C0; // 同源低W16坐标，绘制按S16解释。
extern std::uint16_t gTooltipYCF46CC;
extern const std::array<std::array<std::int32_t, 4>, 4> gPetBarImages57E434; // left/middle/right/single。
extern const std::array<std::array<std::int32_t, 3>, 14> gNumericKeyImages54F030;
extern const std::array<const char*, 6> gTradeNumberHints5805FC; // 原字节提示的借用指针表。
extern const std::array<const char*, 20> gTribeNames57CB54; // 后八项共用585C50原字节串。
const std::byte* BorrowLegacyHealingRecordsD672F0() noexcept; // 借既有七条32B治疗记录，不扩到D673D0。
// 原LY十条20B记录及分立S32状态的非拥有视图；只借页号/选择写权，不复制名单。
struct LegacyItemUseTargetView {
    const std::int32_t* count; // CC40CC。
    std::int32_t* page; // CC46DC。
    const std::int32_t* pageCount; // CC3E70。
    std::int32_t* selected; // CB0A48。
    const std::byte* records; // CB0EA8，kind S16/+2名称，步长20。
};
LegacyItemUseTargetView BorrowLegacyItemUseTargetView() noexcept;

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
LegacySkillStatusView BorrowLegacySkillStatusView() noexcept; // 借4EF750已有唯一owner，地址拓宽，数值仍S32。

std::int32_t ReadLegacyHealthImage(std::int32_t health); // 复用519680分级与57CBA4图号表。
extern std::array<std::int32_t, 6> gSelectedSlotsCA0A90;
extern std::array<std::int32_t, 6> gMaterialIndicesCA0458;
extern std::array<std::int32_t, 6> gSelectionCountsD66E0C;
extern std::array<std::int32_t, 6> gSelectionImagesCF3FF4;
extern std::array<char, 24> gMaterialOccupancyCC4894; // 前五W32占用，末4B与CC48A8同源。
extern std::int32_t gSelectedItemD460C0;
extern std::int32_t gSelectedImageCC3ED8;
extern std::int32_t gSelectedCountCC483C;
extern std::uint8_t gSelectionModeD2D650;
extern std::array<std::uint8_t, 28> gItemMarksD67290;
extern std::array<std::uint8_t, 20> gItemMarksD68A90; // 不为原越界访问扩展为28B。

std::int32_t Sub_4E5710(std::int32_t side); // 先清侧状态，再清物品/宠物整组。
void Sub_4E52B0(std::int32_t side); // 同源玩家宠物属性及十项技能按存储索引复制。
void Sub_4E54E0(std::int32_t side, std::int32_t index); // 背包index+8到交易，不复制描述。
void Sub_4E5600(std::int32_t side); // 顺序同步20条交易物品。
std::int32_t Sub_4D3020(std::int32_t x, std::int32_t y); // 同源坐标依序保存，返回x。
std::int32_t Sub_4D3040(std::int32_t rangeX, std::int32_t rangeY); // 到格才判断距快照的范围。
std::int32_t Sub_4CAF60(std::int32_t unusedValue); // 窗口62更新，标关闭后仍刷新全部资料。
std::uint32_t Sub_4CAFA0(); // 四图号、八资料文本和四分行，返回末行含NUL字节数。
void Sub_4CB530(); // 先清对话类型，再标记关联窗口63。
WindowInstance* Sub_4CB9B0(std::int32_t index); // 借用窗口59+index，只标记关闭并返回原T*。
std::int32_t Sub_4CB2F0(std::int32_t index, std::int32_t state); // 查看资料发信，先金额门禁再打开63。
std::int32_t Sub_4CB550(std::int32_t value); // 窗口63首次按目标决定保留输入，再每帧刷新。
void Sub_4CB5D0(); // 同源收件人与八位费用刷新，原混合返回无人消费。
std::int32_t Sub_4CB6A0(std::int32_t index, std::int32_t state); // 双层转义发送、插入本地迷你邮件及完整关闭尾。
std::int32_t Sub_4CB920(std::int32_t unusedIndex, std::int32_t state); // 固定元素1输入焦点与非活动绘制。
std::int32_t Sub_478330(std::int32_t index, std::int32_t state); // 资料公开/切窗/三目录/确认的完整八分支。
extern std::int32_t gProfileCategory1085DA0; // 478330生产，窗口59按0/1/2选择类别，非地址。
extern std::int32_t gProfileScrollCA0444; // 窗口59六入口共用的S32页码。
std::int32_t Sub_4C94F0(std::int32_t index, std::int32_t state); // 十行分类选择，写当前资料。
std::int32_t Sub_4C96F0(std::int32_t index, std::int32_t state); // 窗口59四滚动/翻页按钮。
std::int32_t Sub_4C9B80(std::int32_t index, std::int32_t state); // WSP01：原分类count-10拖动换算；旧X代码1992。
std::int32_t Sub_4C9BE0(std::int32_t unusedIndex, std::int32_t state); // 三模式滚轮，恒返0。
std::int32_t Sub_4CB9E0(std::int32_t unusedValue); // 窗口64每帧完整刷新，返回1。
std::uint32_t Sub_4CB9F0(); // 图号转换、八文本及256B四行提取，返回末行含NUL长度。
std::int32_t Sub_4C9EE0(std::int32_t value); // 窗口60仅初始化刷新，随后检查距离。
void Sub_4C9F20(); // 固定32/32/15项分类的显隐、文本和选择颜色。
std::int32_t Sub_4CA1A0(std::int32_t index, std::int32_t state); // 分类点击发送后复制共享默认关键词。
void Sub_4CA300(); // 先清对话类型，再标记61/62/63。
std::int32_t Sub_4CB430(std::int32_t index); // 只读selected10C门禁，不检查used。
std::int32_t Sub_4CB4A0(std::int32_t index, std::int32_t* group, std::int32_t* value);
std::int32_t Sub_4CA410(std::int32_t value); // 窗口61首次刷结果，距离关闭后仍同步关键词。
std::uint32_t Sub_4CA450(); // 活动输入与关键词显示互斥，返回复制含NUL长度。
void Sub_4CA500(); // 固定20条结果、三组标题刷新；不重新显示已隐藏的非空行。
std::int32_t Sub_4CA6F0(std::int32_t index, std::int32_t state); // 搜索/上页/下页双层转义请求。
std::int32_t Sub_4CA9C0(std::int32_t index, std::int32_t state); // 按行发查看请求，随后重读窗口62高亮。
void Sub_4CAB60(); // 先清对话类型，再标记62/63。
std::int32_t Sub_4CAB90(std::int32_t unusedIndex, std::int32_t state); // 固定元素7关键词编辑入口。
login::YobiAction* Sub_4B0110(); // 清交易U8，创建窗口43动画并发布借用T*。
// 窗口43三元素组：低S16位置、仅隐藏时联动、只写图形颜色。
void Sub_4B0130(
    WindowInstance* window, std::int32_t index, std::int32_t x, std::int32_t y);
void Sub_4B0180(WindowInstance* window, std::int32_t index, std::int32_t visible);
void Sub_4B01B0(WindowInstance* window, std::int32_t index, std::uint32_t color);
std::int32_t Sub_4AFF70(std::int32_t index); // 释放物品选择，再减同源悬停字节。
std::int32_t Sub_4B0090(std::int32_t index); // 只释放宠物选择及标记。
void Sub_4B0200(); // 窗口43释放回调，原返回值弃用。
std::int32_t Sub_4B0290(std::int32_t value); // 窗口43初始化、关闭检查与四阶段更新。
std::int32_t Sub_4B08D0(std::int32_t unused); // 四类提示后绘制完整当前窗口。
std::int32_t Sub_4AFF30(std::int32_t index, std::int32_t slot); // 空槽领取并标记物品。
std::int32_t Sub_4AFFB0(std::int32_t first, std::int32_t second); // 仅交换选择槽字节。
std::int32_t Sub_4B0060(std::int32_t index, std::int32_t slot); // 空槽领取并标记宠物。
std::int32_t Sub_4B00C0(std::int32_t first, std::int32_t second);
std::int32_t Sub_48BB30(std::int32_t slot); // 物品槽分类0/1/2/3，不是bool。
std::int32_t Sub_48BB70(std::int32_t slot); // 宠物槽分类1/2。
std::int32_t Sub_48BC60(std::int32_t slot); // DE0B08五个S16索引的首匹配位置。
void Sub_47D490(); // 仅清D67B00，不改变其他物品状态。
std::int32_t Sub_4885A0(std::int32_t amount); // 普通移动或窗口41/38文本请求。
void Sub_4886E0(
    std::int32_t sourceWindow, std::int32_t sourceElement,
    std::int32_t source, std::int32_t target); // 完整直接移动/数量拆分选择。
void Sub_488800(
    std::int32_t sourceMode, std::int32_t source,
    std::int32_t targetMode, std::int32_t target); // 两模式按低U8比较，宠物组借CF3B10。
WindowInstance* Sub_48A830(
    std::int32_t sourceWindow, std::int32_t sourceElement, std::int32_t imageNo,
    std::int32_t maximum, std::int32_t value14, std::int32_t initialValue,
    WindowValueCallback callback); // 窗口42发布，回调按原生指针保存。
std::int32_t Sub_48A8C0(std::int32_t value);
std::int32_t Sub_48A900(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_48A950(std::int32_t index, std::int32_t state);
std::int32_t Sub_48AA60(std::int32_t index, std::int32_t state);
std::int32_t Sub_48AB10(std::int32_t index, std::int32_t state);
std::int32_t Sub_474170(std::int32_t unusedIndex, std::int32_t state);
std::int32_t Sub_474BE0(std::int32_t index, std::int32_t state); // 窗口7资料切页及前后排请求。
std::int32_t Sub_475E00(std::int32_t index, std::int32_t state); // 窗口8入口六值快照、加点及切页。
void Sub_48BB90(
    std::int32_t unusedWindow, std::int32_t unusedElement,
    std::int32_t source, std::int32_t target); // 普通/窗口41两路宠物换位请求。
void Sub_4AAB70(
    std::int16_t x, std::int16_t y, std::int32_t index, std::uint8_t textLayer);
void Sub_4AB400(
    std::int16_t x, std::int16_t y, std::int32_t index, std::uint8_t textLayer);
std::int32_t Sub_48D540(
    std::int16_t x, std::int16_t y, std::int32_t index,
    std::uint8_t textLayer, std::uint8_t graphicLayer, std::uint32_t color);
std::int32_t Sub_4EA670(std::int32_t index); // 原25600技能检查，返回0/1。
void Sub_48BF30(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);
std::int32_t Sub_48C130(std::int32_t sourceIndex, std::int32_t mode);
void Sub_48BC90(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);
std::int32_t Sub_48BF10(std::int32_t sourceIndex, std::int32_t mode); // kind2真实物品拖动。
void Sub_4B1940(
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state,
    const render::GraphicDescriptor16* descriptor); // 自有物品交易页，20格玩家背包。
std::int32_t Sub_4B2480(std::int32_t index, std::int32_t state);
void Sub_4B2500(
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* unusedAmount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);
void Sub_4B2060(
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state, std::int32_t scroll,
    const render::GraphicDescriptor16* descriptor); // 自有宠物交易页，按索引对取原宠物。
void Sub_4B25D0(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);
void Sub_4B2E00(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);
std::int32_t Sub_4B2710(std::int32_t index); // kind6，绑定真实物品拖动回调。
std::int32_t Sub_4B2F50(std::int32_t index); // kind7，绑定真实宠物拖动回调。
void Sub_4B2730(
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state,
    const render::GraphicDescriptor16* descriptor); // 己方已选物品完整交互/绘制。
void Sub_4B2F70(
    std::uint8_t* tab, std::int16_t x, std::int16_t y,
    std::int32_t mouseX, std::int32_t mouseY, std::int32_t state, std::int32_t scroll,
    const render::GraphicDescriptor16* descriptor); // 己方已选宠物完整交互/绘制。
std::int32_t Sub_4B3430(std::int32_t index, std::int32_t state);
void Sub_4B34C0(
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* amount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);
std::int32_t Sub_4B0F20(
    std::int16_t x, std::int16_t y, std::int8_t side, std::int32_t index,
    std::uint8_t textLayer, std::uint8_t graphicLayer,
    const render::GraphicDescriptor16* descriptor); // 空宠物槽仍画背景。
std::int32_t Sub_4B1260(
    WindowInstance* window, std::int32_t index, std::int32_t state,
    const std::int32_t* backgroundImages, std::uint8_t* tab,
    std::int32_t scrollElement, std::int32_t* scrollValue,
    std::int32_t numericElement, std::int32_t maximum,
    std::int32_t itemCount, std::int32_t petCount, std::int32_t amount,
    render::GraphicDescriptor16* descriptor, WindowTradePanelCallback callback);
std::int32_t Sub_4B3580(std::int32_t index, std::int32_t state); // 对方交易面板入口。
void Sub_4B3610(
    std::int16_t x, std::int16_t y, std::int32_t mouseX, std::int32_t mouseY,
    std::int32_t state, std::uint8_t* tab, std::int32_t scroll,
    std::uint8_t numericResult, std::uint32_t* valueSlot,
    std::int32_t* itemCount, std::int32_t* petCount, std::int32_t* amount,
    std::int32_t* displayAmount, render::GraphicDescriptor16* descriptor);
bool Sub_4B1900(
    std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height); // X未命中则不读Y。
std::uint8_t Sub_474550(
    WindowInstance* window, std::int32_t index, std::int32_t state,
    std::int32_t maximum, std::uint8_t layer, const char* const* hints,
    std::uint8_t flags, std::uint32_t color); // 数值面板，返回0/1/3。
std::int32_t Sub_4AFFE0(std::int32_t index); // S8物品槽，负值归-1。
std::int32_t Sub_4B00F0(std::int32_t index); // S8宠物槽，负值归-1。
std::int32_t Sub_4B0A50(std::int32_t index, std::int32_t state); // 公开两组交易槽及数值。
std::int32_t Sub_4B0CF0(std::int32_t index, std::int32_t state); // 撤回公开，无计时门禁。
std::int32_t Sub_4B0DB0(std::int32_t index, std::int32_t state); // 确认/撤销确认，共享500ms门槛。
std::int32_t Sub_4B09E0(std::int32_t unusedIndex, std::int32_t state); // 交易输入激活/聊天恢复。
std::int32_t Sub_4C1FB0(std::int32_t index, std::int32_t state); // 关闭入口固定窗口。
std::int32_t Sub_4FBA30(
    std::int32_t x, std::int32_t y, std::uint8_t renderMode,
    std::int32_t fontType, std::int8_t color, const char* text,
    std::int8_t value7, std::int8_t marker); // 原$分段、颜色续接与即时测宽。
std::int32_t Sub_48B810(
    std::int32_t x, std::int32_t y, const char* title, std::int8_t titleColor,
    const char* lines, std::int32_t stride, std::int32_t lineCount,
    const char* footer, std::int8_t footerColor); // 原完整层5提示框。
std::int32_t Sub_48B930(
    std::int32_t x, std::int32_t y, const char* title, std::int8_t titleColor,
    const char* lines, std::int32_t unusedStride, std::int32_t lineCount,
    const char* footer, std::int8_t footerColor); // 连续NUL分段，空行仍推进，不使用stride。
std::int32_t Sub_48B450(std::int32_t side, std::int32_t index, std::int32_t page);
std::int32_t Sub_48B1A0(std::int32_t index, std::int32_t page); // 玩家/所选宠物装备提示。
std::int32_t Sub_48B350(std::int32_t index, std::int32_t page); // 外部存放物品提示。
std::int32_t Sub_48B560(
    std::int32_t index, std::int32_t page, std::int32_t showUnavailable); // 同源商品及可选不可装备提示。
std::int32_t Sub_48B750(std::int32_t page); // CC3210独立详情提示，缓存数值键400。
std::int32_t Sub_4AC200(std::int32_t index, std::int32_t state); // 窗口38右上关闭。
std::int32_t Sub_4A9350(std::int32_t index, std::int32_t state); // 窗口38/41共用关闭按钮。
std::int32_t Sub_4AC290(std::int32_t index, std::int32_t state); // 窗口38自有物品面板。
std::int32_t Sub_4AC890(std::int32_t index, std::int32_t state); // 窗口38外部物品面板。
std::int32_t Sub_4A92D0(std::int32_t unused, std::int32_t state); // 存取数值输入焦点。
std::int32_t Sub_4A93E0(std::int32_t index, std::int32_t state); // 存放第一页请求。
std::int32_t Sub_4A94A0(std::int32_t index, std::int32_t state); // 存放第二页请求。
std::int32_t Sub_4A9560(std::int32_t index, std::int32_t unused); // 第一页选中图。
std::int32_t Sub_4A9580(std::int32_t index, std::int32_t unused); // 第二页选中图。
std::int32_t Sub_4A9B00(std::int32_t unusedIndex, std::int32_t unusedState); // 双侧数量和金额。
std::int32_t Sub_4A95A0(std::int32_t index, std::int32_t state); // 物品页签。
std::int32_t Sub_4A96D0(std::int32_t index, std::int32_t state); // 宠物页签。
std::int32_t Sub_4A9820(std::int32_t index, std::int32_t state); // 存款页签。
std::int32_t Sub_4A9990(std::int32_t index, std::int32_t state); // 取款页签。
std::int32_t Sub_4A9D10(std::int32_t index, std::int32_t state); // 自有存放物品面板。
std::int32_t Sub_4AA3D0(std::int32_t index, std::int32_t state); // 外部存放物品面板。
std::int32_t Sub_4AA9C0(std::int32_t index, std::int32_t state); // 宠物上一行。
std::int32_t Sub_4AAA70(std::int32_t index, std::int32_t state); // 宠物下一行。
std::int32_t Sub_4AAB20(std::int32_t index, std::int32_t state); // 两侧宠物滚轮。
std::int32_t Sub_4ABB20(std::int32_t index, std::int32_t state); // 存取金额输入及请求。
std::int32_t Sub_4AAEC0(std::int32_t index, std::int32_t state); // 自有宠物存放行。
std::int32_t Sub_4AB710(std::int32_t index, std::int32_t state); // 外部宠物提领行。
std::int32_t Sub_4B4070(
    std::int32_t x, std::int32_t y, std::int32_t index,
    std::int32_t skill); // 外部宠物技能名称行，空行仍绘背景。
std::int32_t Sub_4B4140(
    std::int32_t x, std::int32_t y, std::int32_t index); // 外部宠物提示，重量行固定Y=31。
std::int32_t Sub_4B45A0(
    std::int32_t x, std::int32_t y, std::int32_t side,
    std::int32_t index, std::int32_t skill); // 交易宠物技能行，空文本仍绘制行图。
std::int32_t Sub_4B4680(
    std::int32_t x, std::int32_t y, std::int32_t side, std::int32_t index); // 完整交易宠物提示。
std::int32_t Sub_4B3B00(
    std::int32_t x, std::int32_t y, std::int32_t petIndex,
    std::int32_t skillPosition); // 己方技能按S16索引对取名称。
std::int32_t Sub_4B3BF0(
    std::int32_t x, std::int32_t y, std::int32_t index); // 完整己方宠物提示，含重量行。
login::YobiAction* Sub_4E5620(const char* name); // 全量清交易双侧后复制己方，最后打开43。
void Sub_4E4E60(std::int32_t index, const char* name); // 先发送选择请求，失败仍继续本地打开。
login::YobiAction* Sub_49E800(const char* text, WindowListSelectionCallback callback);
login::YobiAction* Sub_4E4E80(const char* text); // 选择表绑定真实交易回调后打开44。
void Sub_49E990(std::int32_t offset); // 逐项复制五行选择文本，重读当前窗口载荷。
std::int32_t Sub_49E8F0(std::int32_t value); // 窗口44初始化与移动关闭。
std::int32_t Sub_49EAA0(std::int32_t unused); // 窗口44滚动消费、完整绘制和九宫格。
std::int32_t Sub_49EB50(std::int32_t index, std::int32_t state); // 上一行。
std::int32_t Sub_49EC00(std::int32_t index, std::int32_t state); // 下一行。
std::int32_t Sub_49ECC0(std::int32_t index, std::int32_t state); // 上五行。
std::int32_t Sub_49ED80(std::int32_t index, std::int32_t state); // 下五行。
std::int32_t Sub_49EE60(std::int32_t index, std::int32_t state); // 滚轮像素余量与偏移。
std::int32_t Sub_49EEB0(std::int32_t index, std::int32_t state); // 取消提示/关闭及按钮三态。
std::int32_t Sub_49EF60(std::int32_t index, std::int32_t state); // 实际选择回调，随后重读窗口关闭。
void Sub_485110(WindowElement* element, std::int32_t state, std::uint32_t imageNo); // 元素外扩一像素悬停框。
std::int32_t Sub_474270(WindowElement* element, std::int32_t maximum); // WSP01：原整数位置换算及两高度取样，借用字节载荷。
std::int32_t Sub_4742C0(WindowElement* element, std::int32_t maximum, std::int32_t position);
std::int32_t Sub_4743B0(
    WindowElement* element, std::int32_t position, std::int32_t maximum,
    std::int32_t wheel); // WWS01：原滚轮像素余量、坐标持样/字节载荷，宿主数学图保持。
void Sub_4E0100(std::int32_t side, std::int32_t index); // 另一展示副本，逐行复制描述。
void Sub_4E02B0(std::int32_t index, std::int32_t side, const char* text); // 外部物品描述及页数。
std::int32_t Sub_4A84A0(const char* text); // 外部存放A/G/I/P完整解析，只有A返回1。
WindowInstance* Sub_4A83A0(std::int32_t kind, std::int32_t value14, std::int32_t userState);
WindowInstance* Sub_4AC0C0(
    std::int32_t kind, std::int32_t value14, std::int32_t userState,
    std::int32_t unusedPhase); // 原38窗口发布，两个caller均保留第四实参。
std::int32_t Sub_4FB8F0(); // 物品拖动重置，不等同窗口拖动。
std::int32_t Sub_4FB920(std::int32_t index); // 仅清与本次回复槽号相同的非负拖动编号。
void Sub_4E5DA0(std::int32_t side, std::int32_t first, std::int32_t second); // 交易整条换位。
void Sub_4E1840(std::int32_t side, std::int32_t first, std::int32_t second); // 展示整条换位。
void Sub_48BA60(std::int32_t first, std::int32_t second); // UI迁移后再交换同源悬停字节。
std::int32_t Sub_4B0F10(std::int32_t index, std::int32_t events); // WSB01：原完整零返回事件回调。
std::int32_t Sub_4A4210(std::int32_t unused) noexcept; // 原mov eax,1/ret，存档/窗口共用。
void Sub_4B0000(std::int32_t first, std::int32_t second); // 窗口43的20个槽号及28B标记。
void Sub_47DCD0(std::int32_t first, std::int32_t second); // 窗口11选择/材料及两组标记。
std::int32_t Sub_47DC20(std::int32_t index); // 六槽/单选两路释放，单选尾接聊天恢复。
void Sub_4AD060(std::int32_t first, std::int32_t second); // 窗口51仅first命中时迁移。
void Sub_4AD090(); // 释放窗口51的行文本及原生指针表。
std::uint32_t Sub_4AD0F0(WindowInstance* window); // 九行刷新，返回末行含NUL长度。
login::YobiAction* Sub_4AD180(
    std::int32_t request, std::int32_t target, const char* text); // 解析标题/正文并展开51。
void Sub_4AD470(const char* text); // 同类对话续接，唯一caller不消费原混合返回。
void Sub_4AD780(); // 51释放回调：行表、物品标记、条件关闭请求。
std::int32_t Sub_4AD800(std::int32_t value); // 51初始化、移动关闭及权限可见性。
std::int32_t Sub_4AD9F0(std::int32_t unused); // 51滚动消费、标题输入及完整绘制。
std::int32_t Sub_4ADAA0(std::int32_t index, std::int32_t unused); // 51模态提示及固定确认按钮。
std::int32_t Sub_4ADBD0(std::int32_t index, std::int32_t state); // 上一行，bit400。
std::int32_t Sub_4ADC80(std::int32_t index, std::int32_t state); // 下一行，bit400。
std::int32_t Sub_4ADD50(std::int32_t index, std::int32_t state); // 上九行，bit2。
std::int32_t Sub_4ADE10(std::int32_t index, std::int32_t state); // 下九行，bit2。
std::int32_t Sub_4ADEF0(std::int32_t index, std::int32_t state); // 51滚轮，draw阶段再刷新文本。
std::int32_t Sub_4ADF40(std::int32_t index, std::int32_t state); // 标题双次转义与N请求。
std::int32_t Sub_4AE170(std::int32_t index, std::int32_t state); // 当前选择I请求，不预拒绝负值。
std::int32_t Sub_4AE260(std::int32_t index, std::int32_t state); // 标题编辑焦点与InputContext借用。
std::int32_t Sub_4AE350(std::int32_t index, std::int32_t state); // 51物品选择落点/拖出与失效清理。
std::int32_t Sub_4AE5B0(std::int32_t unused, std::int32_t state); // 仅在提示为空时填背景提示。
void Sub_4797C0(std::int32_t first, std::int32_t second); // 槽7特殊悬停计数，两条件独立。
std::int32_t Sub_4F4630(std::int32_t itemId); // 当前配方五材料首匹配数量，不读used。
void Sub_4E5770(std::int32_t index, std::int32_t side, const char* text); // 32行及页数。
std::int32_t Sub_4E5830(const char* text); // 每条14字段，返回终止字段状态1。
std::int32_t Sub_4E5A40(std::int32_t index, const char* text); // 单宠物属性与双名称。
std::int32_t Sub_4E5CD0(std::int32_t index, const char* text); // 只预清第一条技能。

// 原输入原型四模式更新及聊天恢复，复用既有InputContext owner。
// WIC01：原型四模式、现场context与BYTE结果合同，见doc/cg2d_window_input_context_dispatch_spec.md。
std::uint8_t Sub_476570(
    WindowInputPrototype* prototype, std::int32_t x, std::int32_t y,
    std::uint32_t mode);
std::int32_t Sub_4A7B00();
// CFD01：聊天点击创建与35槽焦点/当前窗context分离，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
std::int32_t Sub_4A6EA0(std::int32_t unused, std::int32_t state); // 共用聊天点击，焦点取35槽、上下文写当前窗。
std::int32_t Sub_4A80F0(std::int32_t index, std::int32_t state); // WSP01：日志数量前后分别取样，返回滚动结果。
// CFD01：首窗口滚轮按原门槛和调用后新count发布scroll，正文保持；见doc/cg2d_chat_focus_wheel_draw_spec.md。
std::int32_t Sub_4A81D0(std::int32_t unused, std::int32_t state); // 首窗口滚轮，零日志只调整滑块Y。
WindowInstance* FindWindowById_477490(std::int32_t id); // 顺序扫描71显示槽；原循环不跳过空槽。
void Sub_4A7C40(); // 窗口35宽高循环与完整几何，见window_35.md。 // CWP01：见聊天几何生产完整合同。
void Sub_4A8140(); // 查显示中的35，按回看比例更新滑块。 // CWC01：见聊天窗口控制完整合同。
std::int32_t Sub_4A6FC0(std::int32_t index, std::int32_t state); // 聊天宽高、范围、字体、颜色与翻页。 // CWC01：见聊天窗口控制完整合同。
extern WindowElementPrototype gWindowElements570CB0[20]; // 几何更新所读五按钮原型坐标。
void Sub_4740F0(char* destination); // 前10项技能索引的S16槽数，格式化后复制原字节。
// CIG01：IME测量后先字体宽度再聊天单位取样；见doc/cg2d_input_scroll_draw_spec.md。
void Sub_4A7590(input::InputContext* context); // 按聊天宽度滚动目标输入并绘制。
// CIG01：固定1024宽度及滚动绘制正文保持；见doc/cg2d_input_scroll_draw_spec.md。
void Sub_4A7640(input::InputContext* context); // 按固定1024像素滚动目标输入并绘制。
// CLD01：日志分行/字体回写与尺寸合同；见doc/cg2d_chat_log_line_drawing_spec.md。
void Sub_4A7750(std::int32_t x, std::int32_t y); // 原字节分行、底部逆序绘制及前项字体回写。
std::int32_t Sub_46FBC0(std::int32_t unused); // 原默认draw回调，完整绘制后返回1。
std::int32_t Sub_49E750(std::int32_t value); // 窗口39文本居中/到时关闭请求。
std::int32_t Sub_49BDB0(std::int32_t unused); // 完整元素绘制后叠加九宫格背景。
std::int32_t Sub_472F00(
    std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height);
void Sub_473560(std::int32_t index); // 固定元素派发鼠标事件，回调后重读共享阶段。
std::int32_t Sub_4741A0(std::int32_t index, std::int32_t state); // WSB01：低BYTE/W32标志与前项垂直滚动，保原边界取样序。
// DLP01：只清焦点索引/kind/焦点窗口/回调/phase五槽，来源及两坐标保持，返S32 0。
std::int32_t Sub_4763E0(); // 通用鼠标重置，不清来源索引/坐标。
// DLP01：已有kind才绑定两个S32焦点槽，不改变会话阶段，返S32 0/1。
std::int32_t Sub_4763B0(std::int32_t windowId, std::int32_t elementIndex); // 已有kind才写焦点两槽。
std::int32_t Sub_475D50(); // 同源焦点元素。
std::int32_t Sub_475D40(); // 同源焦点窗口，不是当前窗口。
std::int32_t Sub_475D60(); // 同源拖动种类。
std::int32_t Sub_475D70(); // 同源落点X，S16扩展。
std::int32_t Sub_475D80(); // 同源落点Y，S16扩展。
std::int32_t Sub_475D90(); // 同源来源索引。
// DLP01：仅phase0且kind非零启动阶段1，按原焦点/回调/来源写序发布，返S32 0/1。
std::int32_t Sub_475DA0(
    std::int32_t kind, std::int32_t sourceIndex, WindowDragCallback callback);
// DLP01：仅phase0且kind非零启动阶段3，坐标保S16，原焦点与回调写序保持，返S32 0/1。
std::int32_t Sub_476490(
    std::int32_t kind, std::int32_t sourceIndex, std::int16_t x, std::int16_t y,
    WindowDragCallback callback);
// DLP01：入口phase4/5才可选回调message3，之后按原八槽顺序清理；不重验回调后phase。
void Sub_476500(); // 仅阶段4/5调用message3后完整清理。
std::int32_t Sub_472D20(); // WDF01：完整拖动/焦点两位结果，原宽度和现场取样见SPEC。
std::int32_t Sub_4730A0(); // WCR01：71槽压紧/关闭/前插，两个标记判断用同一W32 state。
void Sub_473E40(); // 每轮窗口处理前的鼠标阶段推进。
std::int32_t Sub_503CB0(); // 73ECBC先减后负值归2，返回归位前结果。
std::int32_t Sub_503CD0(); // 名称模式S16递增，保存标量后持久化。
std::int32_t Sub_4A6D80(); // 聊天初始范围、回看、数量、调整请求和颜色。
std::int32_t Sub_4C9240(); // 原型默认坐标和设置默认值，不清窗口实例。 // CWP01：见聊天几何生产完整合同。
std::int32_t Sub_4C93A0(); // 文件三处x与固定原型比较，首差异返回1。
std::int32_t Sub_4C93E0(); // 原文件长度尾部修复，不增加索引保护。
std::int32_t Sub_4C9170(); // 文件坐标和设置回填，共用原UI/世界状态。 // CWP01：见聊天几何生产完整合同。
std::int32_t Sub_4C9060(); // WinSaveDate完整加载/默认回退，保留双打开与返回差别。
std::int32_t Sub_4C92B0(); // 仅清521CH设置文件区。
std::uint32_t Sub_4C92D0(); // 原序捕获窗口设置，返回窗口33的marker快照。
std::int32_t Sub_4C9130(); // 打开成功才捕获并保存；原打开失败同样返回1。
void Sub_472200(); // 仅清71个marker50，不释放窗口。
void Sub_472220(); // 只将现存窗口marker写1，空窗口保留旧值。
void Sub_4722A0(); // 完整鼠标/拖动重置，先清D67B08七W32。
void Sub_4722E0(); // 只清异型元素焦点引用。
void Sub_4722F0(); // marker严格1才分派，四个特殊窗口按原序恢复。
WindowInstance* Sub_4A53D0(); // 先写D2F0B8.+26C，再真实打开22，返回借用窗口。
void Sub_479640(); // 只清窗口操作S32计数。
void Sub_4796C0(); // 只清D67AFC。
void Sub_4796F0(); // 只清D67AFD。
void Sub_479610(); // 清两借用Yobi槽与计数/三个字节，不释放对象。
void Sub_4C7AD0(); // 只清CF4AAC。
void RestoreWindowPositions_475C10(); // 现存窗口恢复原型坐标，仅五类追加原序随机/边界处理。
void ProcessWindowShortcuts_4C7AE0(); // 两模式完整快捷键，Ctrl优先；唯一caller弃用原返回。
void Sub_470980(); // 普通场景完整重开，保持626C14与22/23门禁。
std::int32_t InitializeWindowSystem_4705D0(); // 启动UI完整初始化，返回category加载结果。
std::int32_t Sub_4923D0(); // 三个UI数值依序写5/0/0。
void Sub_4B5A90(); // 四条记录只清已证字段和四个文本首字节。
std::int32_t Sub_4B5B70(std::int32_t index, std::int32_t state); // 输入焦点及非空文本提示。
std::int32_t Sub_4B5C30(std::int32_t index, std::int32_t state); // 删除/写入/取消/翻页完整交互。
void Sub_4B62A0(); // 四条记录刷新元素及输入位置；唯一caller弃用原地址低字节返回。
std::int32_t Sub_4B6C20(); // 清对话类型、关闭入口时实例并透传SE57结果。
std::int32_t Sub_4B6970(std::int32_t flags, char* text); // 原位反转义后生产同源四记录。
std::int32_t Sub_4A5390(); // 五组选择槽及D2F0B8已证前缀重置。
std::int32_t Sub_48C150(); // 所选宠物与两组五槽按原序重置。
std::int32_t Sub_4742B0(); // 只清CF4E50/54。
std::int32_t Sub_4A0940(); // D67B40七槽交错清零。
extern WindowElementPrototype gWindowElements56DB38[111]; // 窗口21完整元素及原型坐标唯一来源。
std::int32_t Sub_4A0970(std::int32_t value); // 窗口21布局、未读扫描与滚动条初始化。
std::int32_t Sub_4A09B0(std::int32_t value); // 完整滚动/闪烁/内容刷新后绘制窗口21。
std::int32_t Sub_4A09E0(); // 三行/九行联系人完整内容刷新，不调用公共Draw。
void Sub_4A1690(std::int32_t mode, std::int32_t category); // 显示模式后接名单类别布局。
void Sub_4A16B0(std::int32_t mode); // 联系人三行/九行显隐，不触碰末三元素。
void Sub_4A1780(std::int32_t category); // 普通/公会/迷你名单布局，借原型坐标。
std::int32_t Sub_4A1C30(std::int32_t index, std::int32_t state); // 单行滚动、边界音效及保存。
std::int32_t Sub_4A1EF0(std::int32_t index, std::int32_t state); // 显示/排序/翻页及三行记录TLKH发送。
std::int32_t Sub_4A2610(std::int32_t index, std::int32_t state); // 群发入口：检查49，动画打开22。
std::int32_t Sub_4A2870(std::int32_t index, std::int32_t state); // 普通/公会/迷你类别切换。
std::int32_t Sub_4A2BD0(std::int32_t index, std::int32_t state); // 动画打开窗口48公会详情。
std::int32_t Sub_4A37E0(std::int32_t index); // 三行/九行元素编号到同源排序后记录索引。
std::int32_t Sub_4A40C0(std::int32_t unused, std::int32_t state); // 滚轮推进，重读类别写回，恒返回0。
std::int32_t Sub_4A2530(std::int32_t index, std::int32_t state); // 六职级按钮，bit400转交完整4A38C0。
void Sub_4A38C0(std::int32_t index); // 两次联系人索引读取，生产职级状态后直接打开55。
std::int32_t Sub_4A2B00(std::int32_t index, std::int32_t state); // 发布同源记录/两类别槽，动画打开40。
std::int32_t Sub_4A3780(std::int32_t index); // 三行/九行分组按钮到实际记录索引。
std::int32_t Sub_4A39C0(std::int32_t index, std::int32_t state); // 九行头像悬停及独立TLKH双击发送。
std::int32_t Sub_4A3C70(std::int32_t index, std::int32_t state); // 十二写信按钮，发布类别/索引后动画打开22。
std::int32_t Sub_4A32E0(std::int32_t index, std::int32_t state); // 三类联系人删除确认分派。
login::YobiAction* Sub_4A33C0(std::int32_t index); // 普通联系人删除确认，窗口47共享上下文。
login::YobiAction* Sub_4A3480(std::int32_t index); // 公会成员除名确认。
login::YobiAction* Sub_4A3540(std::int32_t index); // 迷你邮件联系人删除确认。
std::int32_t& BorrowLegacyMailDeleteIndexD2DA8C(); // 旧42/后续旧44借同一现代删除索引owner，不复制状态。
std::int32_t Sub_4A3600(std::int32_t unusedIndex, std::int32_t unusedState); // FPkR发送、重读清理及保存。
std::int32_t Sub_4A3660(std::int32_t unusedIndex, std::int32_t unusedState); // 公会编号请求与两保存。
std::int32_t Sub_4A36B0(std::int32_t unusedIndex, std::int32_t unusedState); // 迷你姓名转义、发送及重读清理。
std::int32_t Sub_4A3760(std::int32_t unusedIndex, std::int32_t unusedState); // 取消时必要重开窗口21。
std::int32_t Sub_4AE5F0(std::int32_t value); // 窗口47初始化：显隐、文本、初始化回调。
std::int32_t Sub_4AE620(std::int32_t index, std::int32_t state); // CTRL确认，动作槽0。
std::int32_t Sub_4AE690(std::int32_t index, std::int32_t state); // 动作槽1，取消音效55。
std::int32_t Sub_4AE700(std::int32_t index, std::int32_t state); // 动作槽2，音效51。
std::int32_t Sub_4AE770(std::int32_t index, std::int32_t state); // 动作槽3，音效55。
std::int32_t Sub_4AE7E0(std::int32_t index, std::int32_t state); // 动作槽4，音效51。
std::int32_t Sub_4AE850(std::int32_t index, std::int32_t state); // 动作槽5，音效51。
WindowInstance* Sub_4AE8C0(); // 固定窗口47标关闭，返回同一实例。
char* Sub_4AE8D0(); // 十一文本清理与原字节LF分行刷新。
std::int32_t Sub_4AE9C0(); // 六按钮逐项显隐，原最终偏移216。
std::int32_t Sub_4AEA10(char* destination, const char* text); // 按字节长度与宽度单位补空格。
std::int32_t Sub_4A4180(std::int32_t value); // 窗口55只初始化滚动位置。
std::int32_t Sub_4A41A0(std::int32_t unused); // 刷新/公共绘制后反求滚动，再画九宫格。
std::int32_t Sub_4A4220(std::int32_t index, std::int32_t state); // 职级行选择及当前选择图号。
std::int32_t Sub_4A42C0(std::int32_t index, std::int32_t state); // 固定入口窗口关闭、声音后的图形更新。
std::int32_t Sub_4A4370(std::int32_t index, std::int32_t state); // uHb设置后重取窗口关闭，返回仅由hover决定。
std::int32_t Sub_4A4420(std::int32_t index, std::int32_t state); // 上下滚动先发声，只修各自一侧边界。
std::uint32_t Sub_4A4540(); // 十行职级/成员姓名刷新，返回含NUL姓名长度。
std::int32_t Sub_4A45E0(std::int32_t unused, std::int32_t state); // 非零滚轮推进，恒返回0。
std::int32_t Sub_4A4620(std::int32_t value); // 读信初始化：选页后保存，再扫描未读。
std::int32_t Sub_4A4650(std::int32_t unused); // 刷新清当前未读，再公共绘制。
std::int32_t Sub_4A4660(std::int32_t index, std::int32_t state); // 上一封，负页跳9。
std::int32_t Sub_4A4710(std::int32_t index, std::int32_t state); // 下一封，页>=10跳0。
std::int32_t Sub_4A47C0(std::int32_t index, std::int32_t state); // 回复上下文与旧物品占用释放。
void Sub_4A48A0(); // 最后未读位置；原唯一caller不消费混合地址/整数返回。
std::uint32_t Sub_4A49B0(); // 三类邮件十行、姓名、时间和页码，返回末复制长度。
std::int32_t Sub_4A4FC0(std::int32_t unused, std::int32_t state); // 悬停双图，区分三类头像采样序。
std::int32_t Sub_4A53F0(std::int32_t value); // 发信初始化、宠物候选和收件人刷新。
void Sub_4A5650(); // 22/23共享释放：保存草稿、释放占用、关闭49。
std::int32_t Sub_4A58E0(std::int32_t unused, std::int32_t state); // 输入聚焦及非当前输入补绘。
std::int32_t Sub_4A5970(std::int32_t index, std::int32_t state); // 宠物邮件切换、单发/群发完整六路径。
std::uint8_t Sub_4A6160(); // 收件人/宠物按钮显隐及输入位置刷新，透传476570低W8。
std::int32_t Sub_4A56F0(std::int32_t value); // 宠物邮件两遍候选与刷新，标关闭后仍继续。
std::int32_t Sub_4A6300(std::int32_t index, std::int32_t state); // 物品占用迁移、取出拖动及悬停框。
std::int32_t Sub_4A6450(std::int32_t index, std::int32_t state); // 四按钮，两类发送保留不同失败声音。
std::uint8_t Sub_4A6AC0(); // 宠物名称/颜色和携带物品图、数量刷新。
std::int32_t Sub_4A6D30(std::int32_t index); // 同源flags6AA完整W16恰3。
std::int32_t Sub_4A6D50(std::int32_t index); // 先47D4A0，非零才读取flags6AA恰2。
std::int32_t Sub_47D4A0(); // 原S32 D67B00窄读，不返回其他状态。
extern WindowTextPrototype gWindowText572F58; // 49三类副标题，元素原型与刷新同源。
extern WindowTextPrototype gWindowText572F60;
extern WindowTextPrototype gWindowText572F68;
extern WindowTextPrototype gWindowText572F78; // 49三类标题。
extern WindowTextPrototype gWindowText572F80;
extern WindowTextPrototype gWindowText572F88;
std::int32_t Sub_4B4B00(std::int32_t value); // 群组选人初始化/恢复与父窗门禁。
void Sub_4B4B70(); // 释放时仅关闭仍处于群发方式的22。
std::int32_t Sub_4B4B90(std::int32_t value); // 滚动反求、七行刷新、公共绘制。
std::int32_t Sub_4B4BB0(std::int32_t index, std::int32_t state); // 前后翻七行，仅hover消费。
std::int32_t Sub_4B4CA0(std::int32_t index, std::int32_t state); // bit400单行滚动，发声后修正位置。
std::int32_t Sub_4B4DB0(); // 顺序修正上下界，保留count<7时负上界。
std::int32_t Sub_4B4DF0(std::int32_t unused); // 非负最大值反求并发布滚动位置。
void Sub_4B4E20(); // 三种标题和七行显隐，不改选择标志。
std::int32_t Sub_4B5210(std::int32_t index, std::int32_t state); // 切换三模式后清滚动位置。
std::int32_t Sub_4B53C0(std::int32_t index, std::int32_t state); // 全组选择，普通联系人借原排序索引。
std::int32_t Sub_4B5550(std::int32_t index, std::int32_t state); // 七行选择，点击时重读滚动位置。
std::int32_t Sub_4B5A40(std::int32_t unused, std::int32_t state); // 零wheel仍调用4743B0。
void Sub_4B5630(); // 清计数/模式及两200项数组，不清初始化标志。
void Sub_4B5660(std::int32_t mode); // 稀疏名单生产，不按count压紧槽位。
void Sub_4B5760(std::int32_t category); // 原CRT次序排序60或200个S32索引。
std::int32_t Sub_4B57B0(const std::int32_t* left, const std::int32_t* right);
void Sub_4A2700(); // 修正当前名单位置并更新元素17滚动条。
std::int32_t Sub_4A3820(std::int32_t unused); // 滚动条反求位置；调用后重读类别写回。
std::int32_t Sub_4A32B0(); // 邮件闪烁相位推进；开窗不重置。
void Sub_4A2C60(); // 填200项C9FCE4后按类别依原CRT次序排序60/200/20项。
std::int32_t Sub_4A2CC0(const std::int32_t* left, const std::int32_t* right); // 原索引比较，先查右侧used。
std::int32_t Sub_4A3010(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通姓名原字节比较。
std::int32_t Sub_4A3070(
    const login::detail::OrderedRecord1051B14* left,
    const login::detail::OrderedRecord1051B14* right); // 公会姓名原字节比较。
std::int32_t Sub_4A30D0(
    const login::detail::OrderedRecord1051B14* left,
    const login::detail::OrderedRecord1051B14* right); // 公会value00完整S32差值。
std::int32_t Sub_4A3100(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通text32原字节比较。
std::int32_t Sub_4A3160(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通value08完整S32差值。
std::int32_t Sub_4A3190(
    const login::detail::OrderedRecord1051B14* left,
    const login::detail::OrderedRecord1051B14* right); // 公会value10完整S32差值。
std::int32_t Sub_4A31C0(
    const std::int32_t* leftValue04, const std::int32_t* rightValue04); // 借原记录+4字段，相等仍-1。
std::int32_t Sub_4A31E0(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通S16 value02，零映射最大S32。
std::int32_t Sub_4A3230(
    const world::detail::MiniMailContactRecord* left,
    const world::detail::MiniMailContactRecord* right); // 迷你序号按S16比较，零不替换。
std::int32_t Sub_4A3260(
    const login::detail::OrderedRecord1051B14* left,
    const login::detail::OrderedRecord1051B14* right); // 公会S16 value0A，零映射最大S32。
std::int32_t Sub_4A3950(); // 当前公会职级权限80H。
std::uint32_t Sub_4AF960(); // 窗口48管理权限下复制草稿，返回末文本含NUL长度。
std::int32_t Sub_4AFE20(std::int32_t index); // 公会草稿差异；原职级含80H时忽略纯flags变化。
std::int32_t Sub_4AFEA0(); // 完整扫描32职级，不首个差异早返。
std::int32_t Sub_4AFB10(); // 公会名称变更校验/转义/Ufm类型2发送。
std::int32_t Sub_4AFC00(); // 公会第二文本变更校验/转义/Ufm类型3发送。
std::int32_t Sub_4AFCF0(); // 先验证全体变化文本，再逐条GY发送并恢复聊天。
void Sub_4AF180(); // 权限分流与只读刷新，原混合返回由caller弃用。
void Sub_4AF250(); // 管理模式焦点/草稿/按钮与颜色刷新。
std::int32_t Sub_4AF650(); // 七职级与三附加文本刷新，返回末行Y推进值304。
void Sub_4AFA00(std::int32_t index); // 输入原型正文写入当前编辑号对应的同源草稿。
void Sub_4AFA60(); // 页码原范围0..25与音效、滑块同步。
void Sub_4AFAE0(std::int32_t unused); // 拖动反求页码；保留主绘制转发的未用实参。
std::int32_t Sub_4AFED0(); // 固定窗口/焦点，扫描九个输入载荷。
std::int32_t Sub_4AEC30(std::int32_t index, std::int32_t state);
std::int32_t Sub_4AED40(std::int32_t index, std::int32_t state);
std::int32_t Sub_4AEE00(std::int32_t index, std::int32_t state);
std::int32_t Sub_4AF000(std::int32_t index, std::int32_t state);
std::int32_t Sub_4AF090(std::int32_t index, std::int32_t state); // WSP01：滚动恰1时调用既有焦点页依赖。
std::int32_t Sub_4AF0C0(std::int32_t unused, std::int32_t state);
std::int32_t Sub_4AF100(std::int32_t index, std::int32_t state);
std::int32_t Sub_4A3960(std::int32_t mask); // 当前职级-1才拒绝，再读同源权限。
std::int32_t Sub_4A3990(std::int32_t index, std::int32_t mask); // 当前职级门禁通过后读取指定职级。
std::int32_t Sub_4CC6A0(); // 三个UI数值依序写5/0/0。
void Sub_4CBD30(); // 首次借用原localtime对象并写同源日志文件名。
void Sub_4CC340(); // 窗口66原字节分行/十四行刷新，固定651000B局部缓冲。
void Sub_4CC540(); // 日志文件重开/计数，保留原双关闭槽语义，见window_66.md。
std::int32_t Sub_4CBDB0(std::int32_t index, std::int32_t state); // 上下翻与原非标准日期前后切换。
WindowInstance* Sub_4A8330(); // 标旧聊天窗关闭、开另一窗后发布模式，透传新窗T*。
void Sub_4CCED0(); // 窗口65十行序号/名称与活动输入刷新。
std::int32_t Sub_4CC6E0(std::int32_t index, std::int32_t state); // 删除后重读行，再决定提示或编辑。
std::int32_t Sub_4CC9B0(std::int32_t index, std::int32_t state); // 六个黑名单按钮及原消费差异。
std::int32_t Sub_4CCE90(std::int32_t index, std::int32_t state); // WSP01：原90参数拖动换算；旧X代码1999。
login::YobiAction* Sub_4CD150(); // 生产窗口47的清空确认及两个真实回调。
std::int32_t Sub_4CD1E0(std::int32_t unusedIndex, std::int32_t unusedState);
std::int32_t Sub_4CD220(std::int32_t unusedIndex, std::int32_t unusedState);
void InitializeLoginWindows_470600(); // 每次登录的完整UI/邮件/profile初始化；不清全部窗口指针。
std::int32_t Sub_4A6DB0(); // 仅重置回看、两调整请求与数量。
std::int32_t Sub_44B170(); // 清512条日志首字节，再清数量与回看。
std::int32_t Sub_4A7700(); // 不清日志正文或写指针。
void Sub_4A7710(); // 请求高度+1。
void Sub_4A7720(); // 请求高度-1。
void Sub_4A7730(); // 请求宽度+1。
void Sub_4A7740(); // 请求宽度-1。
std::int32_t Sub_4A82E0(); // 窗口57高度循环调整并清两个请求。
// WND01：数字与四符号原字节正反绘制，见doc/cg2d_window_number_draw_spec.md。
void Sub_4771B0(
    std::int32_t x, std::int32_t y, const char* text,
    std::int8_t style, std::int8_t color, std::uint8_t flags, std::uint8_t layer);
// WND01：运行步进乘长度后取扣宽，见doc/cg2d_window_number_draw_spec.md。
std::int32_t Sub_477460(const char* text, std::int8_t style); // 原字节长度乘运行步进减扣宽。

static_assert(sizeof(WindowInputReference) == 16);
static_assert(offsetof(WindowTextPrototype, text) == 8);
static_assert(sizeof(WindowTextPrototype) == 16);
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
static_assert(offsetof(WindowInputPrototype, text) == 0x24);
static_assert(offsetof(WindowInputPrototype, maxLine) == 0x228);
static_assert(sizeof(WindowInputPrototype) == 0x238);
static_assert(offsetof(WindowAnimationData, action) == 0);
static_assert(offsetof(WindowAnimationData, value04) == 8);
static_assert(sizeof(WindowAnimationData) == 16);
static_assert(offsetof(WindowTextData, text) == 8);
static_assert(sizeof(WindowTextData) == 16);
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
