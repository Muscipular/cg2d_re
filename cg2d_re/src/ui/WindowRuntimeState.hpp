#pragma once

#include "cg/ui/WindowRuntime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::login {
struct YobiAction;
}

namespace cg::input {
struct InputContext;
}

namespace cg::ui {

using WindowCallback = std::int32_t (*)(std::int32_t value);
using WindowReleaseCallback = void (*)();
using WindowElementCallback = std::int32_t (*)(std::int32_t index, std::int32_t state);
using WindowValueCallback = void (*)(std::int32_t value);
using WindowDragCallback = void (*)(
    std::int32_t message, std::int32_t flags, std::int32_t sourceIndex);

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
    std::array<std::byte, 4> bytes1C;
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
extern std::int32_t gDragItemIndex584DA4; // 4FB8F0已接重置，4FAEF0拖动生产仍待接。
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
extern std::int32_t gChatWindowModeD66E04; // 原独立S32，窗口35/57选择与设置文件共用。
extern std::uint8_t gValue57DA28; // 原独立U8，初值FF，设置文件与物品窗口共用。
extern std::uint8_t gValue57E42C; // 原独立U8，初值FF，设置文件与宠物窗口共用。
extern std::int32_t gValueCF4AAC; // 原设置文件回填与多个UI入口共用的数值。
extern std::int32_t gValueCF4690;
extern std::int32_t gValueCA0440;
extern std::int32_t gValueD67B38;
extern std::uint8_t gValue5F9754; // 原多窗口/物品UI共享字节，观察进入可能清零。
extern std::int32_t gChatWidthUnitsC9A930; // 原聊天宽度单位，输入滚动/聊天绘制共用。
extern std::int32_t gChatWidthRequestC9A934; // 原宽度调整请求，独立于实际宽度。
extern std::int32_t gChatHeightUnitsC9A938; // 原聊天可见高度的字体9行高倍数。
extern std::int32_t gChatHeightRequestC9A93C; // 原高度调整请求，消费后清零。
extern std::int32_t gChatScrollOffsetC9A940; // 原相对共享写指针的回看条数。

// 交易UI唯一原字节区，非玩家背包/宠物owner；未知字段不冻结语义。
extern std::array<char, 2 * 20 * 0xC4C> gTradeItemsD460D0;
extern std::array<char, 2 * 5 * 0x5FC> gTradePetsC9C108;
extern std::array<char, 2 * 0xFBF8> gItemDetailStorageCC4D58; // 每组首20条C44物品，剩余区未冻结。
extern std::int32_t gTradeValueCC4224;
extern std::int32_t gTradeValueCF4688;
extern std::int32_t gTradeValueD046D0;
extern std::int32_t gTradeValueD2E2B0;
extern std::uint8_t gTradeValueD67B80;
extern std::int32_t gSelectedItemCB1DF4;
extern std::int32_t gSelectedItemCB0A44;
extern std::int32_t gSelectedItem57EE0C; // 原装载初值-1，窗口51只做单向迁移。
extern std::uint8_t gSkillPairIndexD67AD8; // 配方查询按S8解释。
extern std::int32_t gRecipeIndex5846BC; // 原装载初值-1。
extern std::array<std::uint8_t, 20> gTradeItemSlotsCF3AE8; // 比较按S8，写入只取低U8。
extern std::array<std::uint8_t, 28> gTradeItemMarksD2D61C;
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
void Sub_4E0100(std::int32_t side, std::int32_t index); // 另一展示副本，逐行复制描述。
std::int32_t Sub_4FB8F0(); // 物品拖动重置，不等同窗口拖动。
std::int32_t Sub_4FB920(std::int32_t index); // 仅清与本次回复槽号相同的非负拖动编号。
void Sub_4E5DA0(std::int32_t side, std::int32_t first, std::int32_t second); // 交易整条换位。
void Sub_4E1840(std::int32_t side, std::int32_t first, std::int32_t second); // 展示整条换位。
void Sub_48BA60(std::int32_t first, std::int32_t second); // UI迁移后再交换同源悬停字节。
std::int32_t Sub_4B0F10(std::int32_t index, std::int32_t events); // 原真实NULL_FUNC，返回0。
std::int32_t Sub_4A4210(std::int32_t unused) noexcept; // 原mov eax,1/ret，存档/窗口共用。
void Sub_4B0000(std::int32_t first, std::int32_t second); // 窗口43的20个槽号及28B标记。
void Sub_47DCD0(std::int32_t first, std::int32_t second); // 窗口11选择/材料及两组标记。
std::int32_t Sub_47DC20(std::int32_t index); // 六槽/单选两路释放，单选尾接聊天恢复。
void Sub_4AD060(std::int32_t first, std::int32_t second); // 窗口51仅first命中时迁移。
void Sub_4797C0(std::int32_t first, std::int32_t second); // 槽7特殊悬停计数，两条件独立。
std::int32_t Sub_4F4630(std::int32_t itemId); // 当前配方五材料首匹配数量，不读used。
void Sub_4E5770(std::int32_t index, std::int32_t side, const char* text); // 32行及页数。
std::int32_t Sub_4E5830(const char* text); // 每条14字段，返回终止字段状态1。
std::int32_t Sub_4E5A40(std::int32_t index, const char* text); // 单宠物属性与双名称。
std::int32_t Sub_4E5CD0(std::int32_t index, const char* text); // 只预清第一条技能。

// 原输入原型四模式更新及聊天恢复，复用既有InputContext owner。
std::uint8_t Sub_476570(
    WindowInputPrototype* prototype, std::int32_t x, std::int32_t y,
    std::uint32_t mode);
std::int32_t Sub_4A7B00();
void Sub_4740F0(char* destination); // 前10项技能索引的S16槽数，格式化后复制原字节。
void Sub_4A7590(input::InputContext* context); // 按聊天宽度滚动目标输入并绘制。
void Sub_4A7640(input::InputContext* context); // 按固定1024像素滚动目标输入并绘制。
void Sub_4A7750(std::int32_t x, std::int32_t y); // 原字节分行、底部逆序绘制及前项字体回写。
std::int32_t Sub_46FBC0(std::int32_t unused); // 原默认draw回调，完整绘制后返回1。
std::int32_t Sub_49E750(std::int32_t value); // 窗口39文本居中/到时关闭请求。
std::int32_t Sub_49BDB0(std::int32_t unused); // 完整元素绘制后叠加九宫格背景。
std::int32_t Sub_472F00(
    std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height);
void Sub_473560(std::int32_t index); // 固定元素派发鼠标事件，回调后重读共享阶段。
std::int32_t Sub_4741A0(std::int32_t index, std::int32_t state); // 前一元素的垂直滚动。
std::int32_t Sub_4763E0(); // 通用鼠标重置，不清来源索引/坐标。
std::int32_t Sub_472D20(); // 窗口拖动/焦点命中，返回两位处理掩码。
std::int32_t Sub_4730A0(); // 原表压紧、关闭释放及按2000标记前插排序。
void Sub_473E40(); // 每轮窗口处理前的鼠标阶段推进。
std::int32_t Sub_503CB0(); // 73ECBC先减后负值归2，返回归位前结果。
std::int32_t Sub_503CD0(); // 名称模式S16递增，保存标量后持久化。
std::int32_t Sub_4A6D80(); // 聊天初始范围、回看、数量、调整请求和颜色。
std::int32_t Sub_4C9240(); // 原型默认坐标和设置默认值，不清窗口实例。
std::int32_t Sub_4C93A0(); // 文件三处x与固定原型比较，首差异返回1。
std::int32_t Sub_4C93E0(); // 原文件长度尾部修复，不增加索引保护。
std::int32_t Sub_4C9170(); // 文件坐标和设置回填，共用原UI/世界状态。
std::int32_t Sub_4C9060(); // WinSaveDate完整加载/默认回退，保留双打开与返回差别。
std::int32_t Sub_4C92B0(); // 仅清521CH设置文件区。
std::uint32_t Sub_4C92D0(); // 原序捕获窗口设置，返回窗口33的marker快照。
std::int32_t Sub_4C9130(); // 打开成功才捕获并保存；原打开失败同样返回1。
void Sub_472200(); // 仅清71个marker50，不释放窗口。
std::int32_t InitializeWindowSystem_4705D0(); // 启动UI完整初始化，返回category加载结果。
std::int32_t Sub_4923D0(); // 三个UI数值依序写5/0/0。
void Sub_4B5A90(); // 四条记录只清已证字段和四个文本首字节。
std::int32_t Sub_4A5390(); // 五组选择槽及D2F0B8已证前缀重置。
std::int32_t Sub_48C150(); // 所选宠物与两组五槽按原序重置。
std::int32_t Sub_4742B0(); // 只清CF4E50/54。
std::int32_t Sub_4A0940(); // D67B40七槽交错清零。
std::int32_t Sub_4CC6A0(); // 三个UI数值依序写5/0/0。
void Sub_4CBD30(); // 首次借用原localtime对象并写同源日志文件名。
void InitializeLoginWindows_470600(); // 每次登录的完整UI/邮件/profile初始化；不清全部窗口指针。
std::int32_t Sub_4A6DB0(); // 仅重置回看、两调整请求与数量。
std::int32_t Sub_44B170(); // 清512条日志首字节，再清数量与回看。
std::int32_t Sub_4A7700(); // 不清日志正文或写指针。
void Sub_4A7710(); // 请求高度+1。
void Sub_4A7720(); // 请求高度-1。
void Sub_4A7730(); // 请求宽度+1。
void Sub_4A7740(); // 请求宽度-1。
std::int32_t Sub_4A82E0(); // 窗口57高度循环调整并清两个请求。
void Sub_4771B0(
    std::int32_t x, std::int32_t y, const char* text,
    std::int8_t style, std::int8_t color, std::uint8_t flags, std::uint8_t layer);
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
static_assert(offsetof(WindowState, marker50) == 0x5C);
static_assert(sizeof(WindowState) == 0x60);

} // namespace detail
} // namespace cg::ui
