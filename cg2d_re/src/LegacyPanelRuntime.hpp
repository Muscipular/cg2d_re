#pragma once

#include "CharacterSelectSceneDependencies.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace cg::login {
struct YobiAction;
}

namespace cg::login::detail {

// 原5035E0的32B图形按钮；前六字段与503990矩形合同相同，末尾是图号而非文本。
struct LegacyPanelButtonRecord {
    std::int32_t imageX; // 原成员偏移：+0x00。
    std::int32_t imageY; // 原成员偏移：+0x04。
    std::int32_t left; // 原成员偏移：+0x08。
    std::int32_t top; // 原成员偏移：+0x0C。
    std::int32_t width; // 原成员偏移：+0x10。
    std::int32_t height; // 原成员偏移：+0x14。
    std::int32_t normalImage; // 原成员偏移：+0x18。
    std::int32_t pressedImage; // 原成员偏移：+0x1C。
};

// 面板按钮只按原top/left/height/width次序借矩形；旧目录拆分共用同一帮助合同。
inline std::int32_t QueryPanelButtonState(const LegacyPanelButtonRecord* const button)
{
    // 503990仅消费矩形；按原top/left/height/width次序取齐，避免将图号尾部当作文本指针。
    const std::int32_t top = button->top;
    const std::int32_t left = button->left;
    const std::int32_t height = button->height;
    const std::int32_t width = button->width;
    const cg::character_select::detail::CharacterModalUiRecord rectangle {
        0, 0, left, top, width, height, nullptr,
    };
    return cg::character_select::detail::Sub_503990(&rectangle);
}

// 原面板+28H起五条24B页签，只有末槽为地址。
struct LegacyPanelTabRecord {
    std::int32_t image; // 原成员偏移：+0x00。
    std::int32_t x; // 原成员偏移：+0x04。
    std::int32_t y; // 原成员偏移：+0x08。
    std::int32_t width; // 原成员偏移：+0x0C。
    std::int32_t height; // 原成员偏移：+0x10。
    const wchar_t* hint; // 原成员偏移：+0x14。
};

// 原503160/503440共用196B记录；新端不使用原偏移遍历指针字段。
struct LegacyPanelRecord {
    std::int32_t x; // 原成员偏移：+0x00。
    std::int32_t y; // 原成员偏移：+0x04。
    std::int32_t width; // 原成员偏移：+0x08。
    std::int32_t height; // 原成员偏移：+0x0C。
    std::uint32_t id; // 原成员偏移：+0x10。
    std::array<std::int32_t, 5> backgrounds; // 原成员偏移：+0x14。
    std::array<LegacyPanelTabRecord, 5> tabs; // 原成员偏移：+0x28。
    std::int8_t selected; // 原成员偏移：+0xA0。
    std::int8_t hovered; // 原成员偏移：+0xA1。
    std::array<std::byte, 2> reservedA2; // 原成员偏移：+0xA2。
    LegacyPanelButtonRecord closeButton; // 原成员偏移：+0xA4。
};

// 原5030A0/503120共用40B文字按钮；三个原指针分别保留正文/回调/提示身份。
struct LegacyPanelTextButtonRecord {
    character_select::detail::CharacterModalUiRecord label; // 原成员偏移：+0x00。
    std::int32_t (*callback)(); // 原成员偏移：+0x1C。
    std::uint8_t color; // 原成员偏移：+0x20。
    std::array<std::byte, 3> reserved21; // 原成员偏移：+0x21。
    const wchar_t* hint; // 原成员偏移：+0x24。
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

extern std::uint32_t gLegacyPanelHitIdD046C0; // 原全局：0x00D046C0。 503160写入，旧面板父函数借用。
extern LegacyPanelRecord gLegacySkillList585708; // 原全局：0x00585708。 选择编号18；5857A8是同源selected。
extern LegacyPanelRecord gLegacyProfilePanel5857D0; // 原全局：0x005857D0。 旧选择编号2；585870是三页共享selected。
extern std::uint8_t gLegacySkillPendingD67C48; // 原全局：0x00D67C48。 500370清零，旧技能/加工父入口共享。
extern std::int32_t gLegacyRequiredFpCC31D8; // 原全局：0x00CC31D8。 4EF0F0/4FF650生产，旧选择34显示；不复制玩家FP。
extern std::int32_t gLegacyItemWorkSelection584394; // 原全局：0x00584394。 旧物品加工槽号，初值-1，不是现代选中物品状态。
extern std::wstring gLegacyItemWorkNameCC40D8; // 原全局：0x00CC40D8。 4F24A0/4F2A30共用的动态宽名称。
extern std::int32_t gLegacyItemWorkImageCC320C; // 原全局：0x00CC320C。
extern std::int32_t gLegacyCraftPhaseD67D00; // 原全局：0x00D67D00。 旧选择26/27共享，不是现代窗口11状态。
extern std::int32_t gLegacyGemPhaseD68A8C; // 原全局：0x00D68A8C。 旧29/30共享阶段，独立于D68A7C字符串探针。
extern std::int32_t gLegacyRecipePageD67D0C; // 原全局：0x00D67D0C。 旧25页号；4FF650真实复位写同一槽。
extern std::int32_t gLegacyInventoryValueC8B388; // 原全局：0x00C8B388。 旧14特殊使用只见写0，不猜其他用途。
extern std::int32_t gLegacySpecialUseItemC8A96C; // 原全局：0x00C8A96C。 旧79的原物品槽/列/行。
extern std::int32_t gLegacySpecialUseColumnCF4AA8; // 原全局：0x00CF4AA8。
extern std::int32_t gLegacySpecialUseRowD2D614; // 原全局：0x00D2D614。
extern std::int32_t gLegacySplitSourceCC3E60; // 原全局：0x00CC3E60。 旧16拆分，来源/目标/数量非现代窗口42状态。
extern std::int32_t gLegacySplitTargetCC21DC; // 原全局：0x00CC21DC。
extern std::int32_t gLegacySplitAmountC8A948; // 原全局：0x00C8A948。
extern std::int32_t gLegacyLastUsedItemCB01EC; // 原全局：0x00CB01EC。 4E4460后续消费的真实使用来源。
// 原402020旧日志载入；0打开失败，否则S32总页数，1000B局部按D01仅入口清零。
std::int32_t ReloadLegacyLog_402020();
// 原CRT 55302C..553050；只公开真实初始化入口，记录存储保持私有。
// 原函数：0x00455490。
std::int32_t Sub_455490();
// 原函数：0x004554A0。
std::int32_t Sub_4554A0(); // 62B39C=640-556C30，返回写入值。
// 原函数：0x004554C0。
std::int32_t Sub_4554C0();
// 原函数：0x004554D0。
std::int32_t Sub_4554D0(); // 62B3B8起第0组。
// 原函数：0x00455540。
std::int32_t Sub_455540();
// 原函数：0x00455550。
std::int32_t Sub_455550(); // 第1组。
// 原函数：0x004555C0。
std::int32_t Sub_4555C0();
// 原函数：0x004555D0。
std::int32_t Sub_4555D0(); // 第2组。
// 原函数：0x00455640。
std::int32_t Sub_455640();
// 原函数：0x00455650。
std::int32_t Sub_455650(); // 第3组。
// 原函数：0x004556C0。
std::int32_t Sub_4556C0();
// 原函数：0x004556D0。
std::int32_t Sub_4556D0(); // 第4组。
// 原函数：0x00455740。
std::int32_t Sub_455740();
// 原函数：0x00455750。
std::int32_t Sub_455750(); // 62B460，两个中心偏移不同。
// 原函数：0x004557C0。
std::int32_t Sub_4557C0();
// 原函数：0x004557D0。
std::int32_t Sub_4557D0(); // 62B488，返回Y+38。
// 原函数：0x00455840。
std::int32_t Sub_455840();
// 原函数：0x00455850。
std::int32_t Sub_455850(); // 62B4A8起三组交错写入。
// 原函数：0x00455930。
std::int32_t Sub_455930();
// 原函数：0x00455940。
std::int32_t Sub_455940(); // 62B508起五组交错写入。
// 原函数：0x004D36B0。
YobiAction* Sub_4D36B0(); // 旧75独立图号预览，发布D67C50。
// 原函数：0x004D4960。
void Sub_4D4960(std::int32_t x, std::int32_t y, std::int32_t index, std::int32_t page);
// 原函数：0x004D4ED0。
YobiAction* Sub_4D4ED0(); // 旧74预览，始终创建并发布T*。
// 原函数：0x004D5220。
YobiAction* Sub_4D5220(); // 旧83预览，图号借4A0850同源状态。
// 原函数：0x004D5580。
YobiAction* Sub_4D5580(); // 旧84独立D2D814图号，创建后发布CB1DD4。
// 原函数：0x004D61E0。
std::int32_t Sub_4D61E0(std::int32_t x, std::int32_t y, std::int32_t page); // CC3210八行详情及背景。
// 原函数：0x004D3180。
void Sub_4D3180(); // 旧53/54共用六按钮，已证caller弃寄存器结果。
// 原函数：0x004D31C0。
std::int32_t Sub_4D31C0(); // 首个许可且点击的按钮位，全部未命中为0。
// 原函数：0x004D66E0。
YobiAction* Sub_4D66E0(); // 旧55预览初始化，共用CC4890图号并发布D67C58。
// 原函数：0x004D6A30。
YobiAction* Sub_4D6A30(); // 旧56预览初始化，沿旧55同源图号/预览槽。
// 原函数：0x004D6E70。
YobiAction* Sub_4D6E70(); // 旧57预览初始化，沿旧55/56同源工厂与T*槽。
// 原函数：0x004D8D70。
YobiAction* Sub_4D8D70(); // 旧62预览初始化：借同源图号，发布并返回工厂结果。
// 原函数：0x004D91E0。
YobiAction* Sub_4D91E0(); // 旧63独立图号预览，发布D67C6C。
// 原函数：0x004DA400。
void Sub_4DA400(std::int32_t x, std::int32_t y, std::int32_t index,
    std::int32_t page, std::int32_t mode); // 旧63/64同源商品七行详情。
// 原函数：0x004DAA00。
YobiAction* Sub_4DAA00(); // 旧64独立CC3EFC图号，创建后发布D67C74。
// 原函数：0x004DC7A0。
YobiAction* Sub_4DC7A0(); // 旧70独立D673E8图号，发布预览后清提示。
// 原函数：0x004DDA30。
void Sub_4DDA30(std::int32_t x, std::int32_t y, std::int32_t index,
    std::int32_t page); // 旧70独立CE4548详情，每页八行，已证caller弃AL返回。
// 原函数：0x004DDF80。
YobiAction* Sub_4DDF80(); // 旧68独立CF4698图号，发布预览后清提示。
// 原函数：0x004DF210。
void Sub_4DF210(std::int32_t x, std::int32_t y, std::int32_t index,
    std::int32_t page); // 旧68独立C8B390详情，每页八行，已证caller弃AL返回。
// 原函数：0x004E0270。
char* Sub_4E0270(std::int32_t side); // 旧72统计展示区空槽，返回第20项末端原生指针。
// 原函数：0x004E0BC0。
std::int32_t Sub_4E0BC0(std::int32_t side); // 旧72物品格命中，每行首命中而非全表首命中。
// 原函数：0x004E0C70。
void Sub_4E0C70(std::int32_t side, std::int32_t item, std::uint32_t color);
// 原函数：0x004E0D10。
std::int32_t Sub_4E0D10(std::int32_t side); // 旧72两组物品绘制，状态-2不以used门禁。
// 原函数：0x004E0FF0。
std::int32_t Sub_4E0FF0(
    std::int32_t x, std::int32_t y, std::int32_t side, std::int32_t item, std::int32_t page);
// 原函数：0x004E10F0。
std::int32_t Sub_4E10F0(std::int32_t x, std::int32_t y); // 旧72内嵌数量层，不关闭面板。
// 原函数：0x004E18F0。
std::int32_t Sub_4E18F0(std::int32_t side);
// 原函数：0x004E1980。
void Sub_4E1980(std::int32_t side, std::int32_t pet, std::uint32_t color);
// 原函数：0x004E19E0。
void Sub_4E19E0(std::int32_t side); // 五条同源154H宠物记录，负状态图不以used门禁。
// 原函数：0x004E1B10。
std::int32_t Sub_4E1B10(std::int32_t x, std::int32_t y, std::int32_t side, std::int32_t pet);
// 原函数：0x004E1EE0。
std::int32_t Sub_4E1EE0(std::int32_t x, std::int32_t y, std::int32_t side, std::int32_t pet);
// 原函数：0x004E2030。
std::uint32_t Sub_4E2030(); // 旧72物品存放子页，外层4E0380调用。
// 原函数：0x004E28F0。
std::uint32_t Sub_4E28F0(); // 旧72宠物子页，点击费用检查与提示容量检查分开。
// 原函数：0x004E2D90。
std::uint32_t Sub_4E2D90(); // 旧72存入金额，草稿别名CC4B2C末4B，确认只登记。
// 原函数：0x004E3750。
std::uint32_t Sub_4E3750(); // 旧72取出金额，先登记旧草稿再处理键盘/上限。
// 原函数：0x004E6CC0。
std::int32_t Sub_4E6CC0(std::int32_t side); // 旧52交易格命中，两组各20格。
// 原函数：0x004E6D70。
void Sub_4E6D70(std::int32_t side, std::int32_t item, std::uint32_t color);
// 原函数：0x004E6E10。
std::int32_t Sub_4E6E10(std::int32_t side); // 借C4C展示记录，正常返回行数4。
// 原函数：0x004E70B0。
std::int32_t Sub_4E70B0(std::int32_t side); // 旧52五行宠物首命中。
// 原函数：0x004E7140。
void Sub_4E7140(std::int32_t side, std::int32_t pet, std::uint32_t color);
// 原函数：0x004E71A0。
void Sub_4E71A0(std::int32_t side); // 同源5FC宠物名称与正选择框，无负选择覆盖图。
// 原函数：0x004E7290。
std::int32_t Sub_4E7290(std::int32_t x, std::int32_t y, std::int32_t item, std::int32_t page);
// 原函数：0x004E7380。
std::int32_t Sub_4E7380(std::int32_t x, std::int32_t y, std::int32_t side, std::int32_t pet);
// 原函数：0x004E7740。
std::int32_t Sub_4E7740(std::int32_t x, std::int32_t y, std::int32_t side, std::int32_t pet);
// 原函数：0x004E7870。
std::int32_t Sub_4E7870(std::int32_t x, std::int32_t y); // 旧52内嵌拆分，使用独立来源/目标/数量。
// 原函数：0x004E7FC0。
std::int32_t Sub_4E7FC0(); // 对方物品详情，翻页上限依原指令借玩家背包页数。
// 原函数：0x004E81B0。
std::int32_t Sub_4E81B0(); // 对方宠物属性/技能双页。
// 原函数：0x004E82F0。
std::int32_t Sub_4E82F0(); // 对方金额及悬停提示。
// 原函数：0x004E83D0。
std::int32_t Sub_4E83D0(); // 己方背包，选择/拖动撤回已公开报价。
// 原函数：0x004E8920。
std::int32_t Sub_4E8920(); // 己方宠物选择/撤回及双页详情。
// 原函数：0x004E8AD0。
std::int32_t Sub_4E8AD0(); // 己方金额，键盘实际变化才撤回，裁剪不撤回。
// 原函数：0x004E9280。
void Sub_4E9280(); // 旧79详情页/目标初始化，不重置上次悬停或来源参数。
// 原函数：0x004E97C0。
void Sub_4E97C0(); // 旧80详情页/选择初始化，不改排除三槽或上次悬停。
// 原函数：0x004E9C60。
void Sub_4E9C60(); // 旧50详情页/赠送选择初始化，不清选择坐标。
// 原函数：0x004EA130。
void Sub_4EA130(); // 旧49保留草稿后初始化并发布同源CB03F0输入。
// 原函数：0x004EC1D0。
std::uintptr_t Sub_4EC1D0(); // 旧42排序/未读扫描/补四空动画，返回页号W32或末次工厂T*。
// 原函数：0x004EAA50。
std::byte* Sub_4EAA50(); // 旧47倒序查十条未读消息，返回同源非拥有消息字节T*。
// 原函数：0x004EB120。
std::int32_t Sub_4EB120(); // 旧46保留同源D66CA8草稿，清60选择后按旧42目标置1。
// 原函数：0x0052D790。
void Sub_52D790(); // 旧86清详情页并将同源赠品选择置-1，不清上次悬停/行列。
// 原函数：0x0052DC60。
void Sub_52DC60(); // 旧87保留公会草稿后初始化并发布同源1050B80输入。
// 原函数：0x0052E600。
std::byte* Sub_52E600(); // 旧89倒扫十条公会未读，返回非拥有字节游标。
// 原函数：0x0052ECB0。
std::int32_t Sub_52ECB0(); // 旧90保留独立公会草稿，清200选择后按旧94目标置1。
// 原函数：0x0052FCF0。
std::uintptr_t Sub_52FCF0(); // 旧94排序/扫描/补四空动画，返回初始页W32或末次工厂T*。
// 原函数：0x00530B70。
void Sub_530B70(); // 旧95按公会key/本人职级发布同源资格；无公会分支不定义EAX。
// 原函数：0x00532430。
std::int32_t Sub_532430(); // 旧96按同源本人职级是否0发布共享编辑资格，返完整W32。
// 原函数：0x004F24A0。
std::int32_t Sub_4F24A0(); // 名称双快照、真实请求及64B结果名/图号生产。
// 原函数：0x004F25C0。
void Sub_4F25C0(); // 旧加工预览动作，五图号闭区间，不含110350族。
// 原函数：0x004F2650。
std::int32_t Sub_4F2650(); // 旧选择31初始化，218/2032保留原按钮分类。
// 原函数：0x004F2720。
std::int32_t Sub_4F2720(std::int32_t cost, std::int32_t skill); // 旧加工资格检查，失败只改对应阶段。
// 原函数：0x004F4470。
std::int32_t Sub_4F4470(); // 旧材料完整校验：重复/特殊/成员/单堆数量，返回1/2/4。
// 原函数：0x004F4690。
void Sub_4F4690(); // 20标记到六材料槽；特殊第5项与数组同源，唯一caller弃返回。
// 原函数：0x004F4720。
std::int32_t Sub_4F4720(); // 20个非零标记计数，不读取物品used。
// 原函数：0x004F4740。
std::int32_t Sub_4F4740(); // 清同源20B材料标记，返回0。
// 原函数：0x004F4760。
std::int32_t Sub_4F4760(); // 旧27完整初始化，不清26的六材料数组。
// 原函数：0x004F50D0。
std::int32_t Sub_4F50D0(); // 旧制造预览动作，尾部总会517880，含空对象路径。
// 原函数：0x004F5170。
std::int32_t Sub_4F5170(); // 三组六槽依序填-1，数量不填0。
// 原函数：0x004F51A0。
std::int32_t Sub_4F51A0(); // 旧选择26完整初始化，末尾透传4D3020。
// 原函数：0x004F61C0。
YobiAction* Sub_4F61C0(); // 旧24初始化，清返回标记，仅空预览才创建。
// 原函数：0x004F6BF0。
void Sub_4F6BF0(std::int32_t x, std::int32_t y, std::int32_t rating, std::uint8_t layer);
// 原函数：0x004F6C60。
std::uint32_t Sub_4F6C60(); // 旧23返回时页=选中/15，返回原商修正符号而非页号。
// 原函数：0x004F7140。
std::int32_t Sub_4F7140(); // 旧20按序清两个W16展开状态，返回零。
// 原函数：0x004F7870。
void Sub_4F7870(std::int32_t x, std::int32_t y); // 原唯一caller弃返回；完整本体/P06两hover入口-1已接。
// 原函数：0x004F8340。
YobiAction* Sub_4F8340(); // 旧21空预览创建并设方向6。
// 原函数：0x004F8550。
std::int32_t Sub_4F8550(std::int32_t x, std::int32_t y); // 基本资料，导航后才固定宠物。
// 原函数：0x004F8E40。
std::int32_t Sub_4F8E40(std::int32_t x, std::int32_t y); // 属性/加点，导航前固定本帧宠物及上限快照。
// 原函数：0x004F9B10。
std::int32_t Sub_4F9B10(std::int32_t x, std::int32_t y); // 技能行，空槽仍绘底图，说明步长19。
std::int32_t Sub_503BB0(); // 旧人物快捷键，切换选择2。
std::int32_t Sub_503D00(); // 旧动作0快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503D10(); // 旧动作2快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503D20(); // 旧动作4快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503D30(); // 旧动作6快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503D40(); // 旧动作8快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503D50(); // 旧动作10快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503D60(); // 旧动作1快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503D70(); // 旧动作3快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503D80(); // 旧动作7快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503D90(); // 旧动作5快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503DA0(); // 旧动作9快捷键，动作请求后无条件取一次随机数并返回。
std::int32_t Sub_503DB0(); // 旧动作12快捷键，动作请求后无条件取一次随机数并返回。
std::uintptr_t Sub_503DC0(); // 先随机选择14/16/17，透传请求的地址/W32结果，无尾部rand。
std::int32_t Sub_503E00(); // 旧显示快捷跳板，透传4FD4C0。
std::int32_t Sub_503E10(); // 旧显示快捷跳板，透传4FD510。
std::int32_t Sub_503E20(); // 旧显示快捷跳板，透传4FD5F0。
std::int32_t Sub_503E30(); // 旧显示快捷跳板，透传4FD640。
// 原函数：0x00402B30。
std::int32_t Sub_402B30(); // 旧F9：类别2有选择则关闭100/SE57，否则请求日志选择98。
// 原函数：0x00503B70。
std::int32_t Sub_503B70(); // 旧快捷设置：类别2有选择则关闭100/SE57，否则打开3。
// 原函数：0x00503BC0。
std::int32_t Sub_503BC0(); // 旧快捷技能：切换18后关闭31，返回关闭结果。
// 原函数：0x00503BE0。
std::int32_t Sub_503BE0(); // 旧快捷物品：依序关闭16或15，否则切换14。
// 原函数：0x00503C30。
std::int32_t Sub_503C30(); // 旧快捷联系人：切换42。
// 原函数：0x00503C40。
std::int32_t Sub_503C40(); // 旧快捷图鉴：切换23。
// 原函数：0x00503C50。
std::int32_t Sub_503C50(); // 旧快捷地图：切换1。
// 原函数：0x00503C60。
std::int32_t Sub_503C60(); // 旧快捷宠物：切换20。
// 原函数：0x00503C70。
std::int32_t Sub_503C70(); // 旧快捷动作：切换17。
// 原函数：0x004FD690。
std::int32_t Sub_4FD690(); // 旧6清面板selected别名和独立行号，返回零。
// 原函数：0x00519640。
std::int32_t Sub_519640(std::int32_t skill); // 连续非负配方每8项一页，空表仍1页。
// 原函数：0x004FAED0。
std::int32_t Sub_4FAED0(); // 旧14重置既有物品拖动及本页详情/悬停。
// 原函数：0x00504C50。
std::int32_t Sub_504C50(std::int32_t currentImage, std::int32_t requestedImage); // 旧技能面板预览图号重映射。
// 原函数：0x004FB940。
void Sub_4FB940(std::int32_t x, std::int32_t y, std::int32_t item, std::int32_t page); // 旧物品详情八行绘制，原caller均不消费返回。
// 原函数：0x004FBB30。
void Sub_4FBB30(); // 旧15金额草稿清零，原指令不改EAX。
// 原函数：0x00504BA0。
void Sub_504BA0(
    std::int32_t x, std::int32_t y, std::int32_t width, std::uint8_t layer,
    std::int32_t font, std::wstring_view name, std::wstring_view memo,
    std::int32_t unused1, std::int32_t unused2, std::int16_t flags); // 旧物品标题，先测名称再按标志选正文。
// 原函数：0x004FC420。
std::int32_t Sub_4FC420(); // 原CRT 5530A8入口，尾跳旧设置菜单八按钮构造。
// 原函数：0x004FC430。
std::int32_t Sub_4FC430(); // 原D67D38八条文字按钮，保留未写字节并返回Y快照+260。
// 原函数：0x004FCB90。
std::int32_t Sub_4FCB90(); // 原CRT 5530AC入口，尾跳旧聊天设置18条记录构造。
// 原函数：0x004FCBA0。
std::int32_t Sub_4FCBA0(); // D67E80同源文字记录，不重置原+23H未写字节。
// 原函数：0x004FD9F0。
std::int32_t Sub_4FD9F0(); // 原CRT 5530B0入口，尾跳旧音乐设置五控件构造。
// 原函数：0x004FDA00。
std::int32_t Sub_4FDA00(); // D68150五条文字记录，绑定两个音乐音量回调及返回。
// 原函数：0x004FDE10。
std::int32_t Sub_4FDE10(); // 原CRT 5530B4入口，尾跳旧音效设置八控件构造。
// 原函数：0x004FDE20。
std::int32_t Sub_4FDE20(); // D68218八条文字记录，绑定音效音量、立体声及返回。
// 原函数：0x004FE490。
std::int32_t Sub_4FE490(); // 原CRT 5530B8入口，尾跳旧鼠标设置11条记录构造。
// 原函数：0x004FE4A0。
std::int32_t Sub_4FE4A0(); // D68358同源文字记录，原+23H字节不写。
// 原函数：0x004FEB00。
std::int32_t Sub_4FEB00(); // 原CRT 5530BC入口，尾跳快捷键说明页构造。
// 原函数：0x004FEB10。
std::int32_t Sub_4FEB10(); // D68510的13条记录，不增加真实键盘分发。
// 原函数：0x004FF020。
std::int32_t Sub_4FF020(); // 原CRT 5530C0入口，尾跳扫描线设置构造。
// 原函数：0x004FF030。
std::int32_t Sub_4FF030(); // D68718四条记录，共享既有扫描线模式。
// 原函数：0x004FF340。
std::int32_t Sub_4FF340(); // 原CRT 5530C4入口，尾跳旧按钮设置构造。
// 原函数：0x004FF350。
std::int32_t Sub_4FF350(); // D687B8四条文字记录，所有初始颜色为0。
// 原函数：0x005008C0。
std::int32_t Sub_5008C0(std::int32_t x, std::int32_t y); // 旧人物基本资料、属性条及前后排按钮。
// 原函数：0x005014A0。
std::int32_t Sub_5014A0(std::int32_t x, std::int32_t y); // 旧人物面板完整属性页，返回0。
// 原函数：0x00501FD0。
std::int32_t Sub_501FD0(std::int32_t x, std::int32_t y); // 旧称号输入、选择/删除、八行分页。
// 原函数：0x00502930。
std::int32_t Sub_502930(); // 原CRT 5530C8入口，尾跳旧地图面板构造。
// 原函数：0x00502940。
std::int32_t Sub_502940(); // D68868同源面板初始化，不触碰玩家坐标或地图数据。
// 原函数：0x00502B80。
std::int32_t Sub_502B80(std::int32_t* value, std::int32_t digits); // 旧数字键输入，位数门槛先于退格。
// 原函数：0x00504EE0。
std::int32_t Sub_504EE0(); // 旧30清同源20B标记与拖动/阶段，不清六槽输出。
// 原函数：0x00504FA0。
std::int32_t Sub_504FA0(); // 旧30普通/特殊各一项校验，返回2或4。
// 原函数：0x00505BC0。
std::int32_t Sub_505BC0(); // 旧29初始化，预览置2位后透传4F5170材料重置。
// 原函数：0x005062A0。
std::int32_t Sub_5062A0(); // 旧29动作16/6与重置，空对象仍调用FrameMove。
// 原CRT 5530CC..5530F4；底部栏初始化值，不公开未证的完整记录布局。
// 原函数：0x00502C30。
std::int32_t Sub_502C30();
// 原函数：0x00502C40。
std::int32_t Sub_502C40(); // CB1DD8=480-5858A4，返回写入值。
// 原函数：0x00502C60。
std::int32_t Sub_502C60();
// 原函数：0x00502C70。
std::int32_t Sub_502C70(); // CA0014复制5858A4。
// 原函数：0x00502C80。
std::int32_t Sub_502C80();
// 原函数：0x00502C90。
std::int32_t Sub_502C90(); // D2DAB4复制CA0014。
// 原函数：0x00502CA0。
std::int32_t Sub_502CA0();
// 原函数：0x00502CB0。
std::int32_t Sub_502CB0(); // 第0组，写序不同于后七组。
// 原函数：0x00502D20。
std::int32_t Sub_502D20();
// 原函数：0x00502D30。
std::int32_t Sub_502D30(); // 第1组。
// 原函数：0x00502DA0。
std::int32_t Sub_502DA0();
// 原函数：0x00502DB0。
std::int32_t Sub_502DB0(); // 第2组。
// 原函数：0x00502E20。
std::int32_t Sub_502E20();
// 原函数：0x00502E30。
std::int32_t Sub_502E30(); // 第3组。
// 原函数：0x00502EA0。
std::int32_t Sub_502EA0();
// 原函数：0x00502EB0。
std::int32_t Sub_502EB0(); // 第4组。
// 原函数：0x00502F20。
std::int32_t Sub_502F20();
// 原函数：0x00502F30。
std::int32_t Sub_502F30(); // 第5组。
// 原函数：0x00502FA0。
std::int32_t Sub_502FA0();
// 原函数：0x00502FB0。
std::int32_t Sub_502FB0(); // 第6组。
// 原函数：0x00503020。
std::int32_t Sub_503020();
// 原函数：0x00503030。
std::int32_t Sub_503030(); // 第7组。
// 原函数：0x005030A0。
std::int32_t Sub_5030A0(LegacyPanelTextButtonRecord* records, std::int32_t count);
// 原函数：0x00503120。
void Sub_503120(LegacyPanelTextButtonRecord* records, std::int32_t count);
// 原函数：0x00503160。
std::int32_t Sub_503160(
    std::int32_t opened, LegacyPanelRecord* panel, void (*initialize)(), std::int32_t flags);
// 原函数：0x00503440。
std::uint32_t Sub_503440(LegacyPanelRecord* panel, std::int32_t flags);
// 原函数：0x00503590。
std::int32_t Sub_503590(LegacyPanelRecord* panel);
// 原函数：0x005035E0。
void Sub_5035E0(
    LegacyPanelButtonRecord* button, std::uint8_t layer, std::int32_t hoverMode,
    std::uint32_t hoverImage, std::int32_t style);
// 原函数：0x005038B0。
void Sub_5038B0(
    LegacyPanelButtonRecord* button, std::uint8_t layer, std::int32_t hoverMode,
    std::uint32_t hoverImage, std::int32_t selected); // 非零选中图号，不屏蔽hover。

} // namespace cg::login::detail
