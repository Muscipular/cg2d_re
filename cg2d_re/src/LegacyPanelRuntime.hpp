#pragma once

#include "CharacterSelectSceneDependencies.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::login::detail {

// 原5035E0的32B图形按钮；前六字段与503990矩形合同相同，末尾是图号而非文本。
struct LegacyPanelButtonRecord {
    std::int32_t imageX;
    std::int32_t imageY;
    std::int32_t left;
    std::int32_t top;
    std::int32_t width;
    std::int32_t height;
    std::int32_t normalImage;
    std::int32_t pressedImage;
};

// 原面板+28H起五条24B页签，只有末槽为地址。
struct LegacyPanelTabRecord {
    std::int32_t image;
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    const char* hint;
};

// 原503160/503440共用196B记录；新端不使用原偏移遍历指针字段。
struct LegacyPanelRecord {
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::uint32_t id;
    std::array<std::int32_t, 5> backgrounds;
    std::array<LegacyPanelTabRecord, 5> tabs;
    std::int8_t selected;
    std::int8_t hovered;
    std::array<std::byte, 2> reservedA2;
    LegacyPanelButtonRecord closeButton;
};

// 原5030A0/503120共用40B文字按钮；三个原指针分别保留正文/回调/提示身份。
struct LegacyPanelTextButtonRecord {
    character_select::detail::CharacterModalUiRecord label;
    std::int32_t (*callback)();
    std::uint8_t color;
    std::array<std::byte, 3> reserved21;
    const char* hint;
};

static_assert(sizeof(LegacyPanelButtonRecord) == 32);
static_assert(offsetof(LegacyPanelTabRecord, hint) == 24);
static_assert(sizeof(LegacyPanelTabRecord) == 32);
static_assert(offsetof(LegacyPanelRecord, tabs) == 40);
static_assert(offsetof(LegacyPanelRecord, selected) == 200);
static_assert(offsetof(LegacyPanelRecord, closeButton) == 204);
static_assert(sizeof(LegacyPanelRecord) == 240);
static_assert(offsetof(LegacyPanelTextButtonRecord, callback) == 32);
static_assert(offsetof(LegacyPanelTextButtonRecord, color) == 40);
static_assert(offsetof(LegacyPanelTextButtonRecord, hint) == 48);
static_assert(sizeof(LegacyPanelTextButtonRecord) == 56);

extern std::uint32_t gLegacyPanelHitIdD046C0; // 503160写入，旧面板父函数借用。
extern LegacyPanelRecord gLegacySkillList585708; // 选择编号18；5857A8是同源selected。
extern LegacyPanelRecord gLegacyProfilePanel5857D0; // 旧选择编号2；585870是三页共享selected。
extern std::uint8_t gLegacySkillPendingD67C48; // 500370清零，旧技能/加工父入口共享。
extern std::int32_t gLegacyRequiredFpCC31D8; // 4EF0F0/4FF650生产，旧选择34显示；不复制玩家FP。
extern std::int32_t gLegacyItemWorkSelection584394; // 旧物品加工槽号，初值-1，不是现代选中物品状态。
extern std::array<char, 64> gLegacyItemWorkNameCC40D8; // 4F24A0/4F2A30的固定64B复制目标。
extern std::int32_t gLegacyItemWorkImageCC320C;
extern std::int32_t gLegacyCraftPhaseD67D00; // 旧选择26/27共享，不是现代窗口11状态。
extern std::int32_t gLegacyRecipePageD67D0C; // 旧25页号；4FF650待实现的复位须写同一槽。
extern std::int32_t gLegacyInventoryValueC8B388; // 旧14特殊使用只见写0，不猜其他用途。
extern std::int32_t gLegacySpecialUseItemC8A96C; // 旧79的原物品槽/列/行。
extern std::int32_t gLegacySpecialUseColumnCF4AA8;
extern std::int32_t gLegacySpecialUseRowD2D614;
extern std::int32_t gLegacySplitSourceCC3E60; // 旧16拆分，来源/目标/数量非现代窗口42状态。
extern std::int32_t gLegacySplitTargetCC21DC;
extern std::int32_t gLegacySplitAmountC8A948;
extern std::int32_t gLegacyLastUsedItemCB01EC; // 4E4460后续消费的真实使用来源。
// 原CRT 55302C..553050；只公开真实初始化入口，记录存储保持私有。
std::int32_t Sub_455490();
std::int32_t Sub_4554A0(); // 62B39C=640-556C30，返回写入值。
std::int32_t Sub_4554C0();
std::int32_t Sub_4554D0(); // 62B3B8起第0组。
std::int32_t Sub_455540();
std::int32_t Sub_455550(); // 第1组。
std::int32_t Sub_4555C0();
std::int32_t Sub_4555D0(); // 第2组。
std::int32_t Sub_455640();
std::int32_t Sub_455650(); // 第3组。
std::int32_t Sub_4556C0();
std::int32_t Sub_4556D0(); // 第4组。
std::int32_t Sub_455740();
std::int32_t Sub_455750(); // 62B460，两个中心偏移不同。
std::int32_t Sub_4557C0();
std::int32_t Sub_4557D0(); // 62B488，返回Y+38。
std::int32_t Sub_455840();
std::int32_t Sub_455850(); // 62B4A8起三组交错写入。
std::int32_t Sub_455930();
std::int32_t Sub_455940(); // 62B508起五组交错写入。
void Sub_4E9280(); // 旧79详情页/目标初始化，不重置上次悬停或来源参数。
std::uint32_t Sub_4E92A0(std::int32_t opened); // 旧79刻印选择，发送后仍保留绘制/提示公共尾。
std::uint32_t Sub_4ECE00(std::int32_t opened); // 旧点数结果页，选择编号41。
std::uint32_t Sub_4ED020(std::int32_t opened); // 旧战斗结果页，选择编号40。
std::uintptr_t Sub_4EDB20(std::int32_t opened); // 旧选择39采集结果；关闭返回非拥有对象地址。
std::uintptr_t Sub_4EE380(std::int32_t opened); // 旧选择编号38采集动画；关闭可返回原生对象地址。
std::uintptr_t Sub_4EF0F0(std::int32_t opened); // 旧选择37技能结果/再次使用，关闭可返回对象地址。
std::uint32_t Sub_4F0B40(std::int32_t opened); // 旧选择34不足FP提示，保留完整发送/绘制返回。
std::uintptr_t Sub_4F0F70(std::int32_t opened); // 旧16拆分数量，关闭可返回聊天上下文地址。
std::uintptr_t Sub_4F1C40(std::int32_t opened); // 旧选择33物品加工结果，按钮后才读取技能配对。
std::uintptr_t Sub_4F2130(std::int32_t opened); // 旧选择32名称输入，关闭可返回聊天上下文地址。
std::int32_t Sub_4F24A0(); // 名称双快照、真实请求及64B结果名/图号生产。
void Sub_4F25C0(); // 旧加工预览动作，五图号闭区间，不含110350族。
std::int32_t Sub_4F2650(); // 旧选择31初始化，218/2032保留原按钮分类。
std::int32_t Sub_4F2720(std::int32_t cost, std::int32_t skill); // 旧加工资格检查，失败只改对应阶段。
std::uintptr_t Sub_4F2A30(std::int32_t opened); // 旧选择31完整背包选择、计时发送及提示。
std::uintptr_t Sub_4F3ED0(std::int32_t opened); // 旧选择28制造结果，含25/29/30选择分支。
std::int32_t Sub_4F4470(); // 旧材料完整校验：重复/特殊/成员/单堆数量，返回1/2/4。
void Sub_4F4690(); // 20标记到六材料槽；特殊第5项与数组同源，唯一caller弃返回。
std::int32_t Sub_4F4720(); // 20个非零标记计数，不读取物品used。
std::int32_t Sub_4F4740(); // 清同源20B材料标记，返回0。
std::int32_t Sub_4F4760(); // 旧27完整初始化，不清26的六材料数组。
std::uint32_t Sub_4F4790(std::int32_t opened); // 旧选择27材料页，含真实换位请求与拖动。
std::int32_t Sub_4F50D0(); // 旧制造预览动作，尾部总会517880，含空对象路径。
std::int32_t Sub_4F5170(); // 三组六槽依序填-1，数量不填0。
std::int32_t Sub_4F51A0(); // 旧选择26完整初始化，末尾透传4D3020。
std::uint32_t Sub_4F51D0(std::int32_t opened); // 旧选择26制造执行，关闭尾返回关闭27结果。
std::uint32_t Sub_4F5A60(std::int32_t opened); // 旧25配方选择，原始FP token显示与分页前提示索引。
std::int32_t Sub_519640(std::int32_t skill); // 连续非负配方每8项一页，空表仍1页。
std::int32_t Sub_4FAED0(); // 旧14重置既有物品拖动及本页详情/悬停。
std::uintptr_t Sub_4FAEF0(std::int32_t opened); // 旧14完整物品页，关闭返回非拥有预览地址。
std::int32_t Sub_504C50(std::int32_t currentImage, std::int32_t requestedImage); // 旧技能面板预览图号重映射。
void Sub_4FB940(std::int32_t x, std::int32_t y, std::int32_t item, std::int32_t page); // 旧物品详情八行绘制，原caller均不消费返回。
void Sub_4FBB30(); // 旧15金额草稿清零，原指令不改EAX。
std::uintptr_t Sub_4FBB40(std::int32_t opened); // 旧15金钱页；发送留本页，关闭重置保留焦点返回。
void Sub_504BA0(
    std::int32_t x, std::int32_t y, std::int32_t width, std::uint8_t layer,
    std::int32_t font, const char* name, const char* memo,
    std::int32_t unused1, std::int32_t unused2, std::int16_t flags); // 旧物品标题，先测名称再按标志选正文。
std::int32_t Sub_4FC420(); // 原CRT 5530A8入口，尾跳旧设置菜单八按钮构造。
std::int32_t Sub_4FC430(); // 原D67D38八条文字按钮，保留未写字节并返回Y快照+260。
std::uint32_t Sub_4FC730(std::int32_t opened); // 旧选择编号3；关闭保存/八按钮交互及完整绘制。
std::uint32_t Sub_4FC8F0(std::int32_t opened); // 旧选择编号4；服务器选择/回城/返回及完整绘制。
std::int32_t Sub_4FCB90(); // 原CRT 5530AC入口，尾跳旧聊天设置18条记录构造。
std::int32_t Sub_4FCBA0(); // D67E80同源文字记录，不重置原+23H未写字节。
std::uint32_t Sub_4FD1A0(std::int32_t opened); // 旧选择编号5，颜色/行数/范围显示与完整交互。
std::int32_t Sub_4FE490(); // 原CRT 5530B8入口，尾跳旧鼠标设置11条记录构造。
std::int32_t Sub_4FE4A0(); // D68358同源文字记录，原+23H字节不写。
std::uint32_t Sub_4FE860(std::int32_t opened); // 旧选择编号9，模式/速度绘字、交互及完整保存。
std::int32_t Sub_4FEB00(); // 原CRT 5530BC入口，尾跳快捷键说明页构造。
std::int32_t Sub_4FEB10(); // D68510的13条记录，不增加真实键盘分发。
std::uint32_t Sub_4FEF30(std::int32_t opened); // 旧选择编号10，说明文字/返回交互及关闭保存。
std::int32_t Sub_4FF020(); // 原CRT 5530C0入口，尾跳扫描线设置构造。
std::int32_t Sub_4FF030(); // D68718四条记录，共享既有扫描线模式。
std::uint32_t Sub_4FF1A0(std::int32_t opened); // 旧选择编号11，S16索引显示与完整交互。
std::int32_t Sub_4FF340(); // 原CRT 5530C4入口，尾跳旧按钮设置构造。
std::int32_t Sub_4FF350(); // D687B8四条文字记录，所有初始颜色为0。
std::uint32_t Sub_4FF4C0(std::int32_t opened); // 旧选择编号12，测宽后右对齐绘字与完整交互。
std::int32_t Sub_500370(std::int32_t opened); // 旧技能列表完整交互/绘制，不同于窗口主表18。
std::uintptr_t Sub_5006F0(std::int32_t opened); // 三子页总入口；混合W32/上下文T*仅在返回边界拓宽。
std::int32_t Sub_5008C0(std::int32_t x, std::int32_t y); // 旧人物基本资料、属性条及前后排按钮。
std::int32_t Sub_5014A0(std::int32_t x, std::int32_t y); // 旧人物面板完整属性页，返回0。
std::int32_t Sub_501FD0(std::int32_t x, std::int32_t y); // 旧称号输入、选择/删除、八行分页。
std::int32_t Sub_502930(); // 原CRT 5530C8入口，尾跳旧地图面板构造。
std::int32_t Sub_502940(); // D68868同源面板初始化，不触碰玩家坐标或地图数据。
std::uint32_t Sub_5029B0(std::int32_t opened); // 旧地图面板；正常尾返回D046C0而非绘图结果。
std::int32_t Sub_502B80(std::int32_t* value, std::int32_t digits); // 旧数字键输入，位数门槛先于退格。
// 原CRT 5530CC..5530F4；底部栏初始化值，不公开未证的完整记录布局。
std::int32_t Sub_502C30();
std::int32_t Sub_502C40(); // CB1DD8=480-5858A4，返回写入值。
std::int32_t Sub_502C60();
std::int32_t Sub_502C70(); // CA0014复制5858A4。
std::int32_t Sub_502C80();
std::int32_t Sub_502C90(); // D2DAB4复制CA0014。
std::int32_t Sub_502CA0();
std::int32_t Sub_502CB0(); // 第0组，写序不同于后七组。
std::int32_t Sub_502D20();
std::int32_t Sub_502D30(); // 第1组。
std::int32_t Sub_502DA0();
std::int32_t Sub_502DB0(); // 第2组。
std::int32_t Sub_502E20();
std::int32_t Sub_502E30(); // 第3组。
std::int32_t Sub_502EA0();
std::int32_t Sub_502EB0(); // 第4组。
std::int32_t Sub_502F20();
std::int32_t Sub_502F30(); // 第5组。
std::int32_t Sub_502FA0();
std::int32_t Sub_502FB0(); // 第6组。
std::int32_t Sub_503020();
std::int32_t Sub_503030(); // 第7组。
std::int32_t Sub_5030A0(LegacyPanelTextButtonRecord* records, std::int32_t count);
void Sub_503120(LegacyPanelTextButtonRecord* records, std::int32_t count);
std::int32_t Sub_503160(
    std::int32_t opened, LegacyPanelRecord* panel, void (*initialize)(), std::int32_t flags);
std::uint32_t Sub_503440(LegacyPanelRecord* panel, std::int32_t flags);
std::int32_t Sub_503590(LegacyPanelRecord* panel);
void Sub_5035E0(
    LegacyPanelButtonRecord* button, std::uint8_t layer, std::int32_t hoverMode,
    std::uint32_t hoverImage, std::int32_t style);
void Sub_5038B0(
    LegacyPanelButtonRecord* button, std::uint8_t layer, std::int32_t hoverMode,
    std::uint32_t hoverImage, std::int32_t selected); // 非零选中图号，不屏蔽hover。

} // namespace cg::login::detail
