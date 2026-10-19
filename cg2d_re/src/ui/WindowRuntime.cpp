#include "../scene/login/LoginSavedState.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "WindowRuntimeInternal.hpp"
#include "../script/LuaUI.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../world/AlbumState.hpp"
#include "../world/ChatHistory.hpp"
#include "../world/MailState.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <cstdio>

namespace cg::ui {
namespace detail {

// 原第二原型的内联矩形以借用T*表达；不复制共享表或原矩形数据。
struct WindowDragPrototype {
    std::int32_t count;
    WindowDragRect* rectangles;
};

// 原D40B30的521CH设置文件；不与含原生指针的WindowState布局混用。
struct WindowSavedRecord {
    std::int32_t x; // 原成员偏移：+0x00。
    std::int32_t y; // 原成员偏移：+0x04。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。
    std::array<std::byte, 0x114> bytes14; // 原成员偏移：+0x14。
};

struct WindowSavedData {
    std::int32_t version;
    std::array<WindowSavedRecord, kSavedWindowCount> records; // 原成员偏移：+0x04。
};

// 4C803D/4C8B07两段顺序指令的归纳，不是原端存储表；每项执行后才读取下一键。
struct ShortcutEmoteBinding {
    std::uint8_t keyMode0;
    std::uint8_t keyMode1;
    std::int32_t action;
    std::int32_t selected;
};

// 回调与内部函数前置声明。

// SLC01：负slot先返-1；S8 mode0取8项玩家X表，正mode取6项宠物X表，其它返-1。
// 原函数：0x004C63C0。
static std::int32_t WindowItemSelector_ReadSlotX(const WindowItemSelectorState* state, std::int32_t slot);

// SLC01：负slot先返-1；S8 mode0取8项玩家Y表，正mode取6项宠物Y表，其它返-1。
// 原函数：0x004C6400。
static std::int32_t WindowItemSelector_ReadSlotY(const WindowItemSelectorState* state, std::int32_t slot);

// 原函数：0x004C6440。
static void WindowItemSelector_ShowPlayerPage(WindowItemSelectorState* state); // SPG01：玩家页2显示槽6；W32装配模式0显示槽19，两门槛独立，原正文保持。

// 原函数：0x004C6470。
static void WindowItemSelector_ShowPetPage(WindowItemSelectorState* state); // SPG01：宠物页/名称负重及原动态空位显示保持；只判低AL，不钳制itemSlots或补保护。

// 原函数：0x004C6500。
static void WindowItemSelector_HidePageElements(WindowItemSelectorState* state); // SPG01：按原顺序隐藏槽6/7/8/15/16、六装备槽与17；模式0才隐藏19，主元素不改。

// 原函数：0x004C6550。
static void WindowItemSelector_SelectPage(WindowItemSelectorState* state, std::int32_t page); // SPG01：先发布页再隐藏/新页分发；CC40D4仅低BYTE、四回调先取元素；原五页和写序保持，全机器ABI待。

// 原函数：0x004C6790。
static void WindowItemSelector_UpdatePage(WindowItemSelectorState* state); // SFD01：原四页分发/业务钳制保持；两按钮按原顺序取样，共借descriptor[0]；已知上行由窗口回调返回1。

// 原函数：0x004C6990。
static std::int32_t WindowItemSelector_SelectPet(WindowItemSelectorState* state); // PSP01：同state mode按S8映射，回调返回的S32索引原序发布到selectedPet70并返回。

// 原函数：0x004C69B0。
static void WindowItemSelector_RefreshPetHeader(WindowItemSelectorState* state); // PSP01：同宠物previous/updated BYTE及名字/负重原序保持；首次测宽先字体后文本，负重先文本后字体。

// 原函数：0x004C6AC0。
static void WindowItemSelector_MoveElements(WindowItemSelectorState* state, std::int32_t x); // SPG01：依原序移动主元素、四按钮及六表坐标；原S32结果与固定循环保持，不新增钳制。

// SLC01：两轴先窗口再元素取值，flags/淡入计数只读原BYTE；void不证明原EAX上行等价。
// 原函数：0x004C6B10。
static void WindowItemSelector_DrawTooltip(WindowItemSelectorState* state); // 消费本帧hover，宠物物品固定借CF3B10索引。

// 原函数：0x004C6D90。
static std::int32_t WindowItemSelector_DrawPreview(WindowItemSelectorState* state); // PSP01：同一非拥有预览推进/两轴/队列；掩码替换字节先旧字节，返回/原读取门槛保持。

// 原函数：0x004C71C0。
static std::int32_t WindowItemSelector_FindPetIndex(std::int32_t ordinal); // PSP01：五排序对按used W16/索引S16/状态W16计数；不足仍取末扫描项，不新增匹配/索引保护。

// 原函数：0x004C7340。
static bool WindowItemSelector_SetMode(WindowItemSelectorState* state, std::int32_t mode); // SPG01：写mode低BYTE后持旧页；页3按新mode切2/3、页2不切；bool仅承接原AL=1，高位ABI待。

// 原函数：0x004885A0。
static std::int32_t WindowItem_RequestTransferQuantity(std::int32_t amount); // IMT01：source7短路/重读、41优先38及转义后状态-Y-X取序保持，原S32返回，全callback/ABI待。

// 原函数：0x0048D540。
static std::int32_t WindowPet_DrawTooltip(
    std::int16_t x, std::int16_t y, std::int32_t index,
    std::uint8_t textLayer, std::uint8_t graphicLayer, std::uint32_t color);

// 原函数：0x0048BF30。
static void WindowPetDrag_Handle(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);

// 原函数：0x0048BC90。
static void WindowItemDrag_Handle(std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // IDE01：物品回调，两fresh分类/两点击BYTE及descriptor持址，reset/取消后原继续，完整分发及运行待。

// 原函数：0x004740F0。
static void WindowInput_FormatSkillSlotCount(std::wstring& destination); // 前10项技能索引的S16槽数，直接宽格式化。

// 原函数：0x00472F00。
static std::int32_t WindowRuntime_HitTestRect(
    std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height);

// 原函数：0x00473560。
static void WindowRuntime_DispatchElementMouse(std::int32_t index); // 固定元素派发鼠标事件，回调后重读共享阶段。

// 原函数：0x00472D20。
static std::int32_t WindowDrag_Update(); // WDF01：完整拖动/焦点两位结果，原宽度和现场取样见SPEC。

// 原函数：0x004730A0。
static std::int32_t WindowRuntime_CompactAndReorder(); // WCR01：71槽压紧/关闭/前插，两个标记判断用同一W32 state。

// 原函数：0x00473E40。
static void WindowRuntime_AdvanceMousePhase(); // 每轮窗口处理前的鼠标阶段推进。

// 原函数：0x00503CD0。
static std::int32_t WindowRuntime_IncrementNameMode(); // 名称模式S16递增，保存标量后持久化。

// 原函数：0x004C9240。
static std::int32_t WindowRuntime_ResetSettings(); // 原型默认坐标和设置默认值，不清窗口实例。 // CWP01：见聊天几何生产完整合同。

// 原函数：0x004C93A0。
static std::int32_t WindowRuntime_HasChangedDefaultPositions(); // 文件三处x与固定原型比较，首差异返回1。

// 原函数：0x004C93E0。
static std::int32_t WindowRuntime_RepairSettingsTail(); // 原文件长度尾部修复，不增加索引保护。

// 原函数：0x004C9170。
static std::int32_t WindowRuntime_ApplySettings(); // 文件坐标和设置回填，共用原UI/世界状态。 // CWP01：见聊天几何生产完整合同。

// 原函数：0x004C9060。
static std::int32_t WindowRuntime_LoadSettings(); // WinSaveDate完整加载/默认回退，保留双打开与返回差别。

// 原函数：0x004C92B0。
static std::int32_t WindowRuntime_ClearSavedSettings(); // 仅清521CH设置文件区。

// 原函数：0x004C92D0。
static std::uint32_t WindowRuntime_CaptureSettings(); // 原序捕获窗口设置，返回窗口33的marker快照。

// 原函数：0x00472200。
static void WindowRuntime_ClearSavedWindowMarkers(); // 仅清71个marker50，不释放窗口。

// 原函数：0x00472220。
static void WindowRuntime_MarkExistingWindows(); // 只将现存窗口marker写1，空窗口保留旧值。

// 原函数：0x004722A0。
static void WindowRuntime_ResetMouseAndItemFlags(); // 完整鼠标/拖动重置，先清D67B08七W32。

// 原函数：0x004722E0。
static void WindowRuntime_ClearElementFocus(); // 只清异型元素焦点引用。

// 原函数：0x004722F0。
static void WindowRuntime_RestoreMarkedWindows(); // marker严格1才分派，四个特殊窗口按原序恢复。

// 原函数：0x00479640。
static void WindowRuntime_ResetOperationCount(); // 只清窗口操作S32计数。

// 原函数：0x004796C0。
static void WindowRuntime_ResetGatherOperationCount(); // 只清D67AFC。

// 原函数：0x004796F0。
static void WindowRuntime_ResetSkillOperationCount(); // 只清D67AFD。

// 原函数：0x00479610。
static void WindowRuntime_ResetOperationReferences(); // 清两借用Yobi槽与计数/三个字节，不释放对象。

// 原函数：0x004C7AD0。
static void WindowRuntime_ResetShortcutFlag(); // 只清CF4AAC。

// 原函数：0x00475C10。
static void WindowRuntime_RestorePositions(); // 现存窗口恢复原型坐标，仅五类追加原序随机/边界处理。

// 原函数：0x004C7AE0。
static void WindowRuntime_ProcessShortcuts(); // 两模式完整快捷键，Ctrl优先；唯一caller弃用原返回。

// 原函数：0x00470980。
static void WindowRuntime_OpenNormalScene(); // 普通场景完整重开，保持626C14与22/23门禁。

// 原函数：0x004923D0。
static std::int32_t WindowRuntime_ResetAlbumState(); // 三个UI数值依序写5/0/0。

// 原函数：0x004742B0。
static std::int32_t WindowScroll_ResetPosition(); // 只清CF4E50/54。

// 原函数：0x00470600。
static void WindowRuntime_InitializeLogin(); // 每次登录的完整UI/邮件/profile初始化；不清全部窗口指针。

// 控件载荷、窗口状态、组件表与原型。

static_assert(offsetof(WindowDragPrototype, rectangles) == 8);

static_assert(sizeof(WindowDragPrototype) == 16);

WindowItemSelectorState gItemSelectorCF3B10 {}; // 原全局：0x00CF3B10。

std::array<WindowInstance*, kWindowCount> gWindows {}; // 原全局：0x00C8B094。

std::array<WindowState, kWindowCount> gWindowStates {}; // 原全局：0x00C9A9B0。

WindowInstance* gCurrentWindow = nullptr; // 原全局：0x00CF4694。

WindowState* gCurrentWindowState = nullptr; // 原全局：0x00CF4684。

std::uint8_t gOwnStallOpenForBusiness_D67BE0 = 0; // 原全局：0x00D67BE0。

std::int32_t gDragItemIndex584DA4 = -1; // 原全局：0x00584DA4。

std::int32_t gDragImageNoD67D24 = 0; // 原全局：0x00D67D24。

std::int32_t gItemDragXCC4880 = 0; // 原全局：0x00CC4880。

std::int32_t gItemDragYCC4884 = 0; // 原全局：0x00CC4884。

std::int32_t gDraggingCA0B28 = 0; // 原全局：0x00CA0B28。

std::int32_t gHoverDescriptionEnabled_58589C = 1; // 原全局：0x0058589C。

void* gFocusedElementDataD45F94 = nullptr; // 原全局：0x00D45F94。

static std::int32_t gMouseValue57C110 = 0; // 原全局：0x0057C110。

static std::int32_t gDragSourceIndex57C114 = 0; // 原全局：0x0057C114。

static WindowDragCallback gDragCallback57C118 = nullptr; // 原全局：0x0057C118。

static std::int16_t gDragX57C11C = 0; // 原全局：0x0057C11C。

static std::int16_t gDragY57C11E = 0; // 原全局：0x0057C11E。

static std::int32_t gFocusWindowId57C120 = 0; // 原全局：0x0057C120。

static std::int32_t gFocusElementIndex57C124 = -1; // 原全局：0x0057C124。

static std::int32_t gMousePhase57C128 = 0; // 原全局：0x0057C128。

// 原全局：0x0057C12C。 原映像初值-1，不等同焦点元素。
static std::int32_t gCurrentElementIndex57C12C = -1; // 原全局：0x0057C12C。

static std::int32_t gCurrentWindowIdD67A1C = 0; // 原全局：0x00D67A1C。

std::uint32_t gWindowHasFocusCA0AB4 = 0; // 原全局：0x00CA0AB4。

WindowInputPrototype gMailInputPrototype57E740 { // 原全局：0x0057E740。
    {{&cg::ui::detail::gInputContextD66CA8}, WindowInputReferenceKind::Context},
    22, 70, 3, 12, 0, {}, 10, 28, 12, 0,
};

std::int32_t gChatWindowModeD66E04 = 0; // 原全局：0x00D66E04。

std::uint8_t gSkillRebirthModeEnabled_5F9754 = 0; // 原全局：0x005F9754。

std::int32_t gChatWidthUnitsC9A930 = 0; // 原全局：0x00C9A930。

std::int32_t gChatHeightUnitsC9A938 = 0; // 原全局：0x00C9A938。

std::int32_t gChatScrollOffsetC9A940 = 0; // 原全局：0x00C9A940。

std::array<WindowStoragePet, 5> gPetStorageD64E18 {}; // 原全局：0x00D64E18。

std::array<WindowItemDetailGroup, 2> gItemDetailStorageCC4D58 {}; // 原全局：0x00CC4D58。

std::int32_t gShopItemCountCB1E10 = 0; // 原全局：0x00CB1E10。

std::int32_t gShopImageD2DAD0 = 0; // 原全局：0x00D2DAD0。

std::int32_t gShopExchangeCurrencyItemId_D2D608 = 0; // 原全局：0x00D2D608。

std::int32_t gDialogValueCF4E5C = 0; // 原全局：0x00CF4E5C。

std::int32_t gProfileCategory1085DA0 = 0; // 原全局：0x01085DA0。

WindowShopDetail gShopDetailCC3210 {}; // 原全局：0x00CC3210。

std::uint16_t gShopPriceTailD67420 {}; // 原D673EC+0x34：第五行价格末两位的非文本别名。

std::int32_t gShopAvailableCF4A24 = 0; // 原全局：0x00CF4A24。

std::array<std::int32_t, 5> gShopRowKeysC9A954 {}; // 原全局：0x00C9A954。

std::wstring gConfirmationAcceptHintCC3FCC {}; // 原全局：0x00CC3FCC。

std::wstring gConfirmationCancelHintD45D70 {}; // 原全局：0x00D45D70。

static_assert(sizeof(gPetStorageD64E18) == sizeof(WindowStoragePet) * 5); // 原10技能时总大小0x1E00。

// IWT01：存放区为含宽文本的内部对象，不再固定旧字节大小。

static_assert(0x60 + 10 * 0x8C == 0x5D8);

std::wstring gHoverTextD68AE8 {}; // 原全局：0x00D68AE8。

static_assert(sizeof(WindowListSelectionCallback) == 8);

std::int32_t gSelectedItemCB1DF4 = 0; // 原全局：0x00CB1DF4。

std::int32_t gSelectedItemCB0A44 = 0; // 原全局：0x00CB0A44。

std::uint8_t gItemSelectionModeCC40D4 = 0; // 原全局：0x00CC40D4。

std::int32_t gSelectedItem57EE0C = -1; // 原全局：0x0057EE0C。

// 原全局：0x00D2EA1C。 48C130发布，回调分别采点击和绘制模式。
static std::int32_t gPetDragModeD2EA1C = 0; // 原全局：0x00D2EA1C。

// 原全局：0x00CC4C9C。 48BF10发布，仅门禁48BC90的message2请求。
static std::int32_t gItemDragModeCC4C9C = 0; // 原全局：0x00CC4C9C。

// 原全局：0x00CF46B0。 4886E0发布，4885A0读取的来源槽号。
static std::int32_t gItemMoveSourceCF46B0 = 0; // 原全局：0x00CF46B0。

// 原全局：0x00D2DBF8。 4886E0发布，4885A0读取的目标槽号。
static std::int32_t gItemMoveTargetD2DBF8 = 0; // 原全局：0x00D2DBF8。

std::uint64_t gWindowActionTick62B5AC = 0; // 原全局：0x0062B5AC。

std::int32_t gTooltipColorD67D30 = 0; // 原全局：0x00D67D30。

std::int32_t gTooltipSourceD303FC = 0; // 原全局：0x00D303FC。

std::uint16_t gTooltipXCF46C0 = 0; // 原全局：0x00CF46C0。

std::uint16_t gTooltipYCF46CC = 0; // 原全局：0x00CF46CC。

const std::array<std::array<std::int32_t, 4>, 4> gPetBarImages57E434 {{ // 原全局：0x0057E434。
    {243151, 243152, 243153, 243154},
    {243156, 243157, 243158, 243159},
    {243161, 243162, 243163, 243164},
    {243166, 243167, 243168, 243169},
}};

const std::array<std::array<std::int32_t, 3>, 14> gNumericKeyImages54F030 {{ // 原全局：0x0054F030。
    {243170, 243171, 243172},
    {243173, 243174, 243175},
    {243176, 243177, 243178},
    {243179, 243180, 243181},
    {243182, 243183, 243184},
    {243185, 243186, 243187},
    {243188, 243189, 243190},
    {243191, 243192, 243193},
    {243194, 243195, 243196},
    {243197, 243198, 243199},
    {243200, 243201, 243202},
    {243203, 243204, 243205},
    {243206, 243207, 243208},
    {243209, 243210, 243211},
}};

static constexpr wchar_t kTribeUnopened585C50[] = L"未开放"; // 原常量参考：0x00585C50。

const std::array<const wchar_t*, 20> gTribeNames57CB54 {{ // 原全局：0x0057CB54。
    L"人形系",
    L"龙系",
    L"不死系",
    L"飞行系",
    L"昆虫系",
    L"植物系",
    L"野兽系",
    L"特殊系",
    L"金属系",
    L"邪魔系",
    L"神族",
    L"精灵族",
    kTribeUnopened585C50, kTribeUnopened585C50, kTribeUnopened585C50, kTribeUnopened585C50,
    kTribeUnopened585C50, kTribeUnopened585C50, kTribeUnopened585C50, kTribeUnopened585C50,
}};

WindowMaterialState gMaterialOccupancyCC4894 {}; // 原全局：0x00CC4894。

std::uint8_t gInventoryActorSelectionMode_57DA28 = 0xFF; // 原全局：0x0057DA28。

std::uint8_t gBattleItemTargetFlagCC4D40 = 0; // 原全局：0x00CC4D40。

std::uint8_t gBattleItemActorSelectionMode_57E42C = 0xFF; // 原全局：0x0057E42C。

static const std::array<std::int32_t, 8> gSelectorOwnX54F584 {100, 36, 36, 165, 165, 36, 165, 165}; // 原全局：0x0054F584。

static const std::array<std::int32_t, 8> gSelectorOwnY54F5A4 {30, 130, 80, 80, 130, 30, 30, 180}; // 原全局：0x0054F5A4。

static const std::array<std::int32_t, 6> gSelectorPetX54F5C4 {36, 165, 36, 165, 36, 165}; // 原全局：0x0054F5C4。

static const std::array<std::int32_t, 6> gSelectorPetY54F5DC {80, 80, 130, 130, 30, 30}; // 原全局：0x0054F5DC。

std::int32_t gHotkeyLayoutMode_CF4AAC = 0; // 原全局：0x00CF4AAC。

std::int32_t gStatusBarLayoutMode_CF4690 = 0; // 原全局：0x00CF4690。

std::int32_t gShortcutBarCollapsed_CA0440 = 0; // 原全局：0x00CA0440。

std::int32_t gContactsPreviewMode_D67B38 = 0; // 原全局：0x00D67B38。

cg::yobi::YobiAction* gWindowActionCA0174 = nullptr; // 原全局：0x00CA0174。

cg::yobi::YobiAction* gWindowActionD2E928 = nullptr; // 原全局：0x00D2E928。

cg::yobi::YobiAction* gPreviewActionD67B84 = nullptr; // 原全局：0x00D67B84。

// 原全局：0x00D67AF8。 原窗口操作计数，不随指针扩宽。
static std::int32_t gWindowOperationCount_D67AF8 = 0; // 原全局：0x00D67AF8。

std::uint8_t gGatherOperationCount_D67AFC = 0; // 原全局：0x00D67AFC。

std::uint8_t gSkillOperationCount_D67AFD = 0; // 原全局：0x00D67AFD。

std::uint8_t gSkillStatusIconReferences_D67AFE = 0; // 原全局：0x00D67AFE。

std::uint8_t gItemUseTargetReferences_D67B24 = 0; // 原全局：0x00D67B24。

std::int32_t gUsingSkillIndexD2F32C = 0; // 原全局：0x00D2F32C。

std::int32_t gUsingSkillLevelCA0AC4 = 0; // 原全局：0x00CA0AC4。

std::int32_t gSkillTargetKindCF4A10 = 0; // 原全局：0x00CF4A10。

std::int32_t gSkillSendModeC8A970 = 0; // 原全局：0x00C8A970。

std::array<char, 20> gSkillTargetNameD2F748 {}; // 原全局：0x00D2F748。

std::array<BattlePetVitals, 5> gBattlePetVitalsCF4B78 {}; // 原全局：0x00CF4B78。

// 原CF4FC4表情选择区；4BAA50的-1商品映射复用首字节。
std::array<std::uint8_t, 18> gEmoteSelectionCF4FC4 {};

static_assert(sizeof(gEmoteSelectionCF4FC4) == 18);

std::int32_t gSkillPetImageCB0A18 = 0; // 原全局：0x00CB0A18。

std::int32_t gCraftSkillIndexCC2368 = 0; // 原全局：0x00CC2368。

std::int32_t gCraftRecipeIndexD2D638 = 0; // 原全局：0x00D2D638。

std::int32_t gCraftRequestValueD30400 = 0; // 原全局：0x00D30400。 制造发送保留完整S32值。

std::uint8_t gSkillRequestByteD64E13 = 0; // 原全局：0x00D64E13。 其他加工发送按S8读取。

std::uint64_t gSkillPreviewTickCF46C8 = 0; // 原全局：0x00CF46C8。 多个技能窗口共用的另一计时槽。

std::uint8_t gWindow12SkillIndexCA0470 = 0; // 原全局：0x00CA0470。 原低U8发布，索引消费按S8。

std::uint32_t gSkillExtraDelayCF46C4 = 0; // 原全局：0x00CF46C4。

std::wstring gProfileSearchKeyword1075988; // 原全局：0x01075988。

MiniMailComposeState gMiniMailCompose105F508 {}; // 原全局：0x0105F508。

static_assert(offsetof(WindowSavedRecord, x) == 0);

static_assert(offsetof(WindowSavedRecord, y) == 4);

static_assert(offsetof(WindowSavedRecord, value08) == 8);

static_assert(offsetof(WindowSavedRecord, value0C) == 0x0C);

static_assert(offsetof(WindowSavedRecord, value10) == 0x10);

static_assert(offsetof(WindowSavedRecord, bytes14) == 0x14);

static_assert(sizeof(WindowSavedRecord) == 0x128);

static_assert(std::is_standard_layout_v<WindowSavedData>);

static_assert(std::is_trivially_copyable_v<WindowSavedData>);

static_assert(offsetof(WindowSavedData, records) == 4);

static_assert(sizeof(WindowSavedData) == 0x521C);

static WindowSavedData gWindowSavedDataD40B30 {}; // 原全局：0x00D40B30。

static std::int32_t gWindowSavedLengthCC4238 = 0; // 原全局：0x00CC4238。

LoginWindowRuntimeSlots gLoginWindowRuntime {};

Window47RuntimeSlots gWindow47Runtime {};

// CC24D4首行标题和CC2525起正文必须同源，其他NPC窗口也消费这一区域。
std::array<WindowDialogTextLine, 30> gNpcDialogLinesCC24D4 {}; // 原全局：0x00CC24D4。

// 原57C24C目标的内联矩形；同值异地址仍分别存储，见窗口SPEC。
static WindowDragRect gDragRects561998[] {{0, 0, 330, 25}, {315, 0, 20, 92}}; // 原全局：0x00561998。

static WindowDragRect gDragRects561F38[] {{10, 0, 230, 33}, {243, 5, 18, 79}}; // 原全局：0x00561F38。

static WindowDragRect gDragRects5624C0[] {{10, 0, 553, 27}, {566, 5, 18, 79}}; // 原全局：0x005624C0。

static WindowDragRect gDragRects562800[] {{10, 0, 288, 27}, {201, 5, 18, 79}}; // 原全局：0x00562800。

static WindowDragRect gDragRects562AF0[] {{10, 0, 288, 27}, {201, 5, 18, 79}}; // 原全局：0x00562AF0。

static WindowDragRect gDragRects562FD8[] {{22, 0, 256, 33}, {282, 7, 18, 78}}; // 原全局：0x00562FD8。

static WindowDragRect gDragRects5631F0[] {{12, 0, 113, 26}, {128, 7, 18, 78}}; // 原全局：0x005631F0。

static WindowDragRect gDragRects565560[] {{0, 0, 343, 30}, {323, 0, 20, 122}}; // 原全局：0x00565560。

static WindowDragRect gDragRects566E70[] {{0, 0, 272, 30}, {252, 0, 20, 100}}; // 原全局：0x00566E70。

static WindowDragRect gDragRects567908[] {{0, 0, 231, 25}, {221, 0, 30, 100}}; // 原全局：0x00567908。

static WindowDragRect gDragRects568528[] {{0, 0, 279, 25}, {249, 0, 25, 120}}; // 原全局：0x00568528。

static WindowDragRect gDragRects568DC8[] {{16, 0, 184, 30}, {209, 7, 18, 78}}; // 原全局：0x00568DC8。

static WindowDragRect gDragRects569298[] {{10, 0, 275, 33}, {288, 5, 18, 79}}; // 原全局：0x00569298。

static WindowDragRect gDragRects56B030[] {{0, 0, 184, 21}}; // 原全局：0x0056B030。

static WindowDragRect gDragRects56B538[] {{0, 0, 61, 25}}; // 原全局：0x0056B538。

static WindowDragRect gDragRects56C1F8[] {{9, 0, 481, 27}, {493, 7, 18, 78}}; // 原全局：0x0056C1F8。

static WindowDragRect gDragRects56C6F0[] {{17, 0, 300, 33}, {320, 7, 18, 78}}; // 原全局：0x0056C6F0。

static WindowDragRect gDragRects56C900[] {{17, 0, 339, 33}, {359, 7, 18, 78}}; // 原全局：0x0056C900。

static WindowDragRect gDragRects56E918[] {{0, 0, 342, 30}, {331, 0, 20, 120}}; // 原全局：0x0056E918。

static WindowDragRect gDragRects56FC00[] {{0, 0, 243, 10}}; // 原全局：0x0056FC00。

static WindowDragRect gDragRects570010[] {{0, 0, 267, 25}, {207, 0, 25, 100}}; // 原全局：0x00570010。

static WindowDragRect gDragRects5704B8[] {{0, 0, 232, 30}, {207, 0, 25, 100}}; // 原全局：0x005704B8。

static WindowDragRect gDragRects570F30[] {{0, 0, 100, 25}}; // 原全局：0x00570F30。

static WindowDragRect gDragRects571620[] {{9, 0, 558, 27}, {571, 7, 18, 78}}; // 原全局：0x00571620。

static WindowDragRect gDragRects571800[] {{9, 0, 558, 27}, {571, 7, 18, 78}}; // 原全局：0x00571800。

static WindowDragRect gDragRects571D48[] {{10, 0, 231, 27}, {234, 5, 18, 79}}; // 原全局：0x00571D48。

static WindowDragRect gDragRects572898[] {{0, 0, 320, 25}, {317, 0, 20, 100}}; // 原全局：0x00572898。

static WindowDragRect gDragRects572D78[] {{9, 0, 558, 27}, {571, 7, 18, 78}}; // 原全局：0x00572D78。

static WindowDragRect gDragRects573430[] {{0, 0, 280, 27}, {260, 0, 20, 120}}; // 原全局：0x00573430。

static WindowDragRect gDragRects573A48[] {{0, 0, 634, 30}, {604, 0, 30, 130}}; // 原全局：0x00573A48。

static WindowDragRect gDragRects5745C0[] {{16, 0, 533, 27}, {552, 5, 18, 80}}; // 原全局：0x005745C0。

static WindowDragRect gDragRects575648[] {{16, 0, 533, 27}, {552, 5, 18, 80}}; // 原全局：0x00575648。

static WindowDragRect gDragRects576248[] {{16, 0, 533, 27}, {552, 5, 18, 80}}; // 原全局：0x00576248。

static WindowDragRect gDragRects576E48[] {{16, 0, 533, 27}, {552, 5, 18, 80}}; // 原全局：0x00576E48。

static WindowDragRect gDragRects577698[] {{0, 0, 231, 25}, {221, 0, 30, 100}}; // 原全局：0x00577698。

static WindowDragRect gDragRects579D28[] {{0, 0, 634, 35}, {600, 0, 34, 130}}; // 原全局：0x00579D28。

static WindowDragRect gDragRects579DD0[] {{0, 0, 305, 25}, {300, 0, 30, 130}}; // 原全局：0x00579DD0。

static WindowDragRect gDragRects579E78[] {{0, 0, 317, 25}, {307, 0, 30, 130}}; // 原全局：0x00579E78。

static WindowDragRect gDragRects57AD30[] {{0, 0, 231, 25}, {221, 0, 30, 100}}; // 原全局：0x0057AD30。

static WindowDragRect gDragRects57B1E0[] {{0, 0, 460, 25}, {445, 0, 30, 102}}; // 原全局：0x0057B1E0。

static WindowDragRect gDragRects57B460[] {{16, 0, 533, 27}, {552, 5, 18, 80}}; // 原全局：0x0057B460。

static std::array<WindowDragPrototype, 42> gDragPrototypes {{
    {2, gDragRects561998}, // 0
    {2, gDragRects561F38},
    {2, gDragRects5624C0},
    {2, gDragRects562800},
    {2, gDragRects562AF0},
    {2, gDragRects562FD8},
    {2, gDragRects5631F0},
    {2, gDragRects565560},
    {2, gDragRects566E70},
    {2, gDragRects567908},
    {2, gDragRects568528}, // 10
    {2, gDragRects568DC8},
    {2, gDragRects569298},
    {1, gDragRects56B030},
    {1, gDragRects56B538},
    {2, gDragRects56C1F8},
    {2, gDragRects56C6F0},
    {2, gDragRects56C900},
    {2, gDragRects56E918},
    {1, gDragRects56FC00},
    {2, gDragRects570010}, // 20
    {2, gDragRects5704B8},
    {1, gDragRects570F30},
    {2, gDragRects571620},
    {2, gDragRects571800},
    {2, gDragRects571D48},
    {2, gDragRects572898},
    {2, gDragRects572D78},
    {2, gDragRects573430},
    {2, gDragRects573A48},
    {2, gDragRects5745C0}, // 30
    {2, gDragRects575648},
    {2, gDragRects576248},
    {2, gDragRects576E48},
    {2, gDragRects577698},
    {2, gDragRects579D28},
    {2, gDragRects579DD0},
    {2, gDragRects579E78},
    {2, gDragRects57AD30},
    {2, gDragRects57B1E0},
    {2, gDragRects57B460}, // 40
    {0, nullptr}, // 原D67A20虚拟零区，同一空矩形原型。
}};

// 原54EFE8的71项S8类别，与窗口编号一一对应。
static constexpr std::array<std::int8_t, kWindowCount> kWindowCategories {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 4, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 4, 4, 0, 1, 2, 2, 0, 1, 2, 4, 2, 2, 2, 2, 2,
    1, 1, 2, 4, 2, 2, 2, 1, 2, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 2, 2, 2, 0, // 大地图独立，不关闭其它类别窗口。
};

static constexpr std::array<std::int32_t, 52> kNumberDigitImages54F0D8 { // 原常量参考：0x0054F0D8。
    243300, 243310, 243320, 243330, 243340, 243350, 243360,
    243370, 243380, 243390, 245500, 245500, 245500,
    243500, 243510, 243520, 243530, 243540, 243550, 243560,
    243570, 243580, 243590, 243500, 243500, 243500,
    243700, 243710, 243720, 243730, 243740, 243750, 243760,
    243770, 243780, 243790, 243700, 243700, 243700,
    245520, 245520, 245520, 245520, 245520, 245520, 245560,
    245580, 245520, 245520, 245540, 245540, 245540,
};

static constexpr std::array<std::int32_t, 52> kNumberSymbolImages54F1A8 { // 原常量参考：0x0054F1A8。
    243400, 243410, 243420, 243430, 243440, 243450, 243460,
    243470, 243480, 243490, 245510, 245510, 245510,
    243600, 243610, 243620, 243630, 243640, 243650, 243660,
    243670, 243680, 243690, 243600, 243600, 243600,
    243800, 243810, 243820, 243830, 243840, 243850, 243860,
    243870, 243880, 243890, 243800, 243800, 243800,
    245530, 245530, 245530, 245530, 245530, 245530, 245570,
    245580, 245530, 245530, 245550, 245550, 245550,
};

static constexpr std::array<std::int32_t, 12> kNumberOffsets54F278 { // 原常量参考：0x0054F278。
    2, 2, 5, 5, 4, 8, 5, 5, 8, 2, 3, 5,
};

static constexpr std::array<std::int32_t, 4> kNumberSpacingDeductions54F2A8 {2, 2, 2, 1}; // 原常量参考：0x0054F2A8。

static std::array<std::int32_t, 4> gNumberAdvancesD67AC4 {}; // 原全局：0x00D67AC4。

std::int32_t& gTitleModeCF4E50 = gLoginWindowRuntime.valuesCF4E50[0]; // 原全局：0x00CF4E50。

std::int32_t& gTitlePageCF4E54 = gLoginWindowRuntime.valuesCF4E50[1]; // 原全局：0x00CF4E54。

WindowInputPrototype gPetNameInputPrototype57DCF8 { // 原全局：0x0057DCF8。
    {{&cg::ui::detail::gInputContextD2D4A0}, WindowInputReferenceKind::Context},
    66, 31, 3, 12, 6, {}, 1, 16, 0, 0,
};

// 原71槽逐项绑定；下标仅用于静态初始化，不是对象句柄。
static std::array<WindowDragPrototype*, kWindowCount> gWindowDragPrototypes57C24C { // 原全局：0x0057C24C。
    &gDragPrototypes[41], // 0
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[0],
    &gDragPrototypes[0],
    &gDragPrototypes[0],
    &gDragPrototypes[1], // 10
    &gDragPrototypes[2],
    &gDragPrototypes[3],
    &gDragPrototypes[4],
    &gDragPrototypes[5],
    &gDragPrototypes[7],
    &gDragPrototypes[8],
    &gDragPrototypes[8],
    &gDragPrototypes[8],
    &gDragPrototypes[9],
    &gDragPrototypes[10], // 20
    &gDragPrototypes[18],
    &gDragPrototypes[21],
    &gDragPrototypes[21],
    &gDragPrototypes[11],
    &gDragPrototypes[12],
    &gDragPrototypes[41],
    &gDragPrototypes[16],
    &gDragPrototypes[17],
    &gDragPrototypes[41],
    &gDragPrototypes[15], // 30
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[13],
    &gDragPrototypes[14],
    &gDragPrototypes[22],
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[24],
    &gDragPrototypes[41],
    &gDragPrototypes[20], // 40
    &gDragPrototypes[23],
    &gDragPrototypes[6],
    &gDragPrototypes[27],
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[26],
    &gDragPrototypes[28],
    &gDragPrototypes[29], // 50
    &gDragPrototypes[25],
    &gDragPrototypes[41],
    &gDragPrototypes[30],
    &gDragPrototypes[31],
    &gDragPrototypes[19],
    &gDragPrototypes[41],
    &gDragPrototypes[41],
    &gDragPrototypes[0],
    &gDragPrototypes[34],
    &gDragPrototypes[35], // 60
    &gDragPrototypes[36],
    &gDragPrototypes[37],
    &gDragPrototypes[21],
    &gDragPrototypes[0],
    &gDragPrototypes[38],
    &gDragPrototypes[39],
    &gDragPrototypes[40],
    &gDragPrototypes[40],
    &gDragPrototypes[32],
    &gDragPrototypes[33], // 70
    &gDragPrototypes[41], // 71：标题及地图拖动由大地图元素回调区分。
};

static std::int32_t gWindowUpdateCountCC4844 = 0; // 原全局：0x00CC4844。 473F80的共享数量，draw返回后重读。

static constexpr ShortcutEmoteBinding kShortcutEmotes[] {
    {0x31, 0x31, 0, 0},
    {0x32, 0x32, 2, 1},
    {0x33, 0x33, 4, 2},
    {0x34, 0x34, 6, 3},
    {0x35, 0x35, 8, 4},
    {0x36, 0x36, 10, 5},
    {0x37, 0x37, 12, 6},
    {0x38, 0x38, 1, 9},
    {0x39, 0x39, 3, 10},
    {0x30, 0x42, 5, 11},
    {0x54, 0x4E, 15, 16},
    {0xBD, 0x4D, 7, 12},
    {0xDE, 0xBC, 9, 13},
    {0x47, 0xBE, 11, 14},
    {0x42, 0xBF, 13, 15},
};

// 窗口函数实现。

// 原函数：0x004A6D00。
std::int32_t WindowRuntime_MatchesRuntimePrefix(
    const WindowRuntimePrefixD2F0B8 first, const WindowRuntimePrefixD2F0B8 second)
{
    return first.value00 == second.value00 && first.value04 == second.value04 ? 1 : 0;
}

// 原519680：零单独处理，其余按有符号26/51/76分档，不钳制负值。
// 原函数：0x00519680。
std::int32_t WindowItem_HealthImageLevel(const std::int32_t health)
{
    if (health == 0) {
        return 0;
    }
    if (health < 26) {
        return 1;
    }
    if (health < 51) {
        return 2;
    }
    return health < 76 ? 3 : 4;
}

// 元素事件与阶段推进共用映射，按原edge/held/旧edge/pulse顺序采样与合并。
static std::int32_t WindowRuntime_AppendMouseEvents(std::int32_t events)
{
    const auto edge = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
    if ((edge & 1u) != 0) {
        events |= 0x02;
    }
    if ((edge & 2u) != 0) {
        events |= 0x04;
    }
    const auto held = static_cast<std::uint8_t>(platform::gLegacyMouseState.heldButtons);
    if ((held & 1u) != 0) {
        events |= 0x08;
    }
    if ((held & 2u) != 0) {
        events |= 0x10;
    }
    if ((edge & 0x10u) != 0) {
        events |= 0x100;
    }
    if ((edge & 0x20u) != 0) {
        events |= 0x200;
    }
    const auto pulse = static_cast<std::uint8_t>(platform::gLegacyMouseState.pulseButtons);
    if ((pulse & 1u) != 0) {
        events |= 0x400;
    }
    if ((pulse & 2u) != 0) {
        events |= 0x800;
    }
    return events;
}

void WindowRuntime_FillTwoTextLines(WindowTextData* const first, WindowTextData* const second, const std::wstring_view contents)
{
    const auto font = std::to_integer<std::uint8_t>(first->style.font);
    const auto rows = render::Font::split(contents, font, 20 * render::GetFontMetrics(font).sWidth);
    const std::size_t prefix = rows.front().size();
    first->text = contents.substr(0, prefix);
    second->text = contents.substr(prefix);
}

// WSB01：原33C0/C3只返0，保留当前窗口事件签名，不补业务。
// 原函数：0x004B0F10。
std::int32_t WindowElement_NoOp(const std::int32_t, const std::int32_t)
{
    return 0;
}

// WCL01：原4A4210完整两条只返回1；窗口release适配/存档共用，实际资源回收由FreeWindow执行。
// 原函数：0x004A4210。
std::int32_t WindowRuntime_ReturnOne(const std::int32_t) noexcept
{
    return 1;
}

// 原函数：0x0048BA60。
void WindowItemDrag_RemapHover(const std::int32_t first, const std::int32_t second)
{
    if (first >= 100 || second >= 100) {
        return;
    }
    const std::int32_t selected = gSelectedItemCB1DF4;
    if (selected == first) {
        gSelectedItemCB1DF4 = second;
    } else if (selected == second) {
        gSelectedItemCB1DF4 = first;
    }
    if (world::gPlayerRuntimeSlots.items[first].imageNo == 0x67C9
        || world::gPlayerRuntimeSlots.items[second].imageNo == 0x67C9) {
        const std::int32_t previous = gSelectedItemCB0A44;
        if (previous == first) {
            gSelectedItemCB0A44 = second;
        } else if (previous == second) {
            gSelectedItemCB0A44 = first;
        }
    }
    WindowTrade_ResetSelection(first, second);
    WindowCraft_ResetSelection(first, second);
    WindowTrade_RemapTextListSelection(first, second);
    WindowSkill_AdjustSpecialHoverCount(first, second);
    auto* const hover = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    const unsigned char secondValue = hover[second];
    const unsigned char firstValue = hover[first];
    hover[first] = secondValue;
    hover[second] = firstValue;
}

// 原函数：0x004FB8F0。
std::int32_t WindowItem_ResetItemDrag()
{
    gDragItemIndex584DA4 = -1;
    gDragImageNoD67D24 = 0;
    gItemDragXCC4880 = 0;
    gItemDragYCC4884 = 0;
    gDraggingCA0B28 = 0;
    gHoverDescriptionEnabled_58589C = 1;
    return 0;
}

// 原函数：0x004FB920。
std::int32_t WindowItem_CancelMatchingItemDrag(const std::int32_t index)
{
    const std::int32_t previous = gDragItemIndex584DA4;
    if (previous >= 0 && previous == index) {
        return WindowItem_ResetItemDrag();
    }
    return previous;
}

// IDE01：原21条；原signed三区间及S32 1/2/3/0保持，正确正文保，四CALL/全部消费者与运行待。
// 原函数：0x0048BB30。
std::int32_t WindowItem_ClassifyItemSlot(const std::int32_t slot)
{
    if (slot >= 0 && slot < 8) {
        return 1;
    }
    if (slot >= 8 && slot < 28) {
        return 2;
    }
    return slot >= 100 && slot < 120 ? 3 : 0;
}

// 原函数：0x0048BB70。
std::int32_t WindowItem_ClassifyPetSlot(const std::int32_t slot)
{
    return slot >= 0 && slot < 5 ? 1 : 2;
}

// 原函数：0x0048BC60。
std::int32_t WindowItem_FindSortedPetIndex(const std::int32_t slot)
{
    for (std::int32_t position = 0; position < 5; ++position) {
        if (std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[position].index) == slot) {
            return position;
        }
    }
    return -1;
}

// WCL01：原474170完整15条；bit2置当前窗口bit13，hover优先返回1，否则低BYTE bit5，正确正文保持。
// 原函数：0x00474170。
std::int32_t WindowElement_HandleSideArea(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) != 0) {
        gCurrentWindow->state |= 0x2000u;
    }
    return (state & 1) != 0 ? 1 : (static_cast<std::uint8_t>(state) >> 5) & 1;
}

// IMT01：原88条，source7短路及41/38转义后状态-Y-X/真实返回；旧格式历史及正确正文保，全owner/运行待。
// 原函数：0x004885A0。
static std::int32_t WindowItem_RequestTransferQuantity(const std::int32_t amount)
{
    std::int32_t source = gItemMoveSourceCF46B0;
    std::int32_t target;
    if (source == 7 || (target = gItemMoveTargetD2DBF8) == 7) {
        WindowCraft_ResetItemSelection();
        source = gItemMoveSourceCF46B0;
        target = gItemMoveTargetD2DBF8;
    }
    if (source < 100 && target < 100) {
        return network::MoveItemStackRequest_510EC0(platform::gConnectionSocket, source, target, amount);
    }
    if (source < 8) {
        return source;
    }
    WindowState* windowState = &gWindowStates[WindowId_Bank];
    if (windowState->window == nullptr) {
        windowState = &gWindowStates[WindowId_ItemStorage];
        if (windowState->window == nullptr) {
            return source;
        }
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"I|{}|{}|{}", source, target, amount);
    const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 255);
    const auto userState = std::bit_cast<std::int32_t>(windowState->userState.value);
    const auto value14 = std::bit_cast<std::int32_t>(windowState->value14);
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    return network::SendDialogResponse_512620(
        platform::gConnectionSocket, x, y, value14, userState, 0, escapedReply);
}

// IMT01：原97条，Ctrl BYTE/反转开关与count/used门槛、直接source先target/拆分target先source；正确正文保。
// 原函数：0x004886E0。
void WindowItem_RequestTransfer(
    const std::int32_t sourceWindow, const std::int32_t sourceElement,
    const std::int32_t source, const std::int32_t target)
{
    if (source < 0 || target < 0 || source == target) {
        return;
    }
    bool split = (platform::gKeyStates[0x11] & 2u) == 0;
    if (platform::gInvertItemSplitModifier_D67AE8 != 0) {
        split = !split;
    }
    std::int32_t count;
    if (source < 100) {
        count = world::gPlayerRuntimeSlots.items[source].count;
    } else {
        std::memcpy(
            &count, &gItemDetailStorageCC4D58[0].items[source - 100].count, sizeof(count));
    }
    std::int16_t used;
    if (target < 100) {
        used = std::bit_cast<std::int16_t>(world::gPlayerRuntimeSlots.items[target].used);
    } else {
        std::memcpy(
            &used, &gItemDetailStorageCC4D58[0].items[target - 100], sizeof(used));
    }
    if (source < 8 || target < 8 || count <= 0 || used != 0 || !split) {
        gItemMoveSourceCF46B0 = source;
        gItemMoveTargetD2DBF8 = target;
        static_cast<void>(WindowItem_RequestTransferQuantity(-1));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else {
        gItemMoveTargetD2DBF8 = target;
        gItemMoveSourceCF46B0 = source;
        static_cast<void>(cg::yobi::CreateQuantityWindowOpeningAnimation_48AC60(
            sourceWindow, sourceElement, 244309, count, count, 0,
            [](const std::int32_t amount) { static_cast<void>(WindowItem_RequestTransferQuantity(amount)); },
            1, 0, 0));
    }
}

// 原函数：0x0048BB90。
void WindowItem_RequestPetTransfer(
    const std::int32_t, const std::int32_t,
    const std::int32_t source, const std::int32_t target)
{
    if (source < 0 || target < 0 || source == target) {
        return;
    }
    if (source < 100 && target < 100) {
        static_cast<void>(network::MovePetSlotRequest_511BC0(platform::gConnectionSocket, source, target));
        return;
    }
    if (gWindowStates[WindowId_Bank].window == nullptr) {
        return;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"P|{}|{}", source, target);
    const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 255);
    const auto userState = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].userState.value);
    const auto value14 = std::bit_cast<std::int32_t>(gWindowStates[WindowId_Bank].value14);
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(network::SendDialogResponse_512620(
        platform::gConnectionSocket, x, y, value14, userState, 0, escapedReply));
}

// 原函数：0x004EA670。
std::int32_t WindowPet_HasSkill25600(const std::int32_t index)
{
    return world::HasPetTechnique_52E180(index, 25600) != 0 ? 1 : 0;
}

// 原函数：0x0048D540。
static std::int32_t WindowPet_DrawTooltip(
    const std::int16_t x, const std::int16_t y, const std::int32_t index,
    const std::uint8_t textLayer, const std::uint8_t graphicLayer, const std::uint32_t color)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const auto originX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 150));
    const auto originY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y - 24));
    world::PetRuntimeRecord& pet = world::gPetRuntimeRecords[index];
    if (pet.used != 0) {
        static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
        static_cast<void>(render::DrawText_456780(
            0, 0, textLayer, 12, 0, L"", 0, 0));
        const auto drawText = [&](const std::wstring_view text, const std::int32_t dx,
                                  const std::int32_t dy) {
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(originX + dx)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(originY + dy)),
                textLayer, 12, 0, text, 0, 0));
        };
        const char* const name =
            std::strlen(pet.name6BD.data()) != 0 ? pet.name6BD.data() : pet.name6AC.data();
        drawText(cg::text::FromGbkWide(name), 8, 6);
        std::wstring buffer;
        cg::text::FormatTo(buffer, L"{:3}", pet.value08);
        drawText(buffer, 141, 6);
        const std::int16_t tribe = std::bit_cast<std::int16_t>(pet.value04);
        if (tribe >= 0 && tribe < 20) {
            drawText(gTribeNames57CB54[tribe], 118, 21);
        }
        const std::int32_t maxHp = static_cast<std::int32_t>(pet.values0C[1]);
        const std::int32_t hp = static_cast<std::int32_t>(pet.values0C[0]);
        cg::text::FormatTo(buffer, L"{:6}/{:6}", hp, maxHp);
        drawText(buffer, 37, 19);
        const std::int32_t maxFp = static_cast<std::int32_t>(pet.values0C[3]);
        const std::int32_t fp = static_cast<std::int32_t>(pet.values0C[2]);
        cg::text::FormatTo(buffer, L"{:6}/{:6}", fp, maxFp);
        drawText(buffer, 37, 32);
        if (WindowPet_HasSkill25600(index) == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                originX + 244, originY + 26, graphicLayer,
                pet.follow6D4 != 0 ? 243078 : 243079, 0, 1, 0, &descriptor));
        }
        static_cast<void>(render::DrawGraphic3_520970(
            originX + 270, originY + 26, graphicLayer,
            pet.flags6AA == 1 ? 243081 : 243082, 0, 1, 0, &descriptor));
        static_cast<void>(render::DrawGraphic3_520970(
            originX + 244, originY + 6, graphicLayer,
            pet.flags6AA == 2 ? 243084 : 243085, 0, 1, 0, &descriptor));
        for (std::int32_t row = 0; row < 4; ++row) {
            const std::int32_t segments = pet.values70[21 + row] / 10;
            const std::int32_t drawY = originY + 5 + row * 10;
            std::int32_t drawX = originX + 175;
            // 原57DCB4与57E434完整同值，仅复用图号，不合并原地址身份。
            const auto& images = gPetBarImages57E434[row];
            if (segments > 1) {
                static_cast<void>(render::DrawGraphic3_520970(
                    drawX, drawY, graphicLayer, images[0], 0, 1, 0, &descriptor));
                drawX += 6;
                for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                    static_cast<void>(render::DrawGraphic3_520970(
                        drawX, drawY, graphicLayer, images[1], 0, 1, 0, &descriptor));
                    drawX += 6;
                }
                static_cast<void>(render::DrawGraphic3_520970(
                    drawX, drawY, graphicLayer, images[2], 0, 1, 0, &descriptor));
            } else if (segments == 1) {
                static_cast<void>(render::DrawGraphic3_520970(
                    drawX, drawY, graphicLayer, images[3], 0, 1, 0, &descriptor));
            }
        }
    }
    return cg::render::DrawGraphic(
        originX + 150, originY + 24, graphicLayer, 243140, 0, &descriptor);
}

// 原函数：0x0048BF30。
static void WindowPetDrag_Handle(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (message != 0) {
        return;
    }
    const std::int32_t kind = WindowItem_ClassifyPetSlot(sourceIndex);
    if (kind == 1) {
        if (world::gPetRuntimeRecords[sourceIndex].used == 0) {
            static_cast<void>(WindowDrag_ResetMouse());
        }
    } else if (kind == 2) {
        std::uint16_t used;
        std::memcpy(&used, &gPetStorageD64E18[sourceIndex - 100], sizeof(used));
        if (used == 0) {
            static_cast<void>(WindowDrag_ResetMouse());
        }
    }
    const std::int32_t clickMode = gPetDragModeD2EA1C;
    const auto clicked = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
    if (clickMode == 0) {
        if ((clicked & 2u) != 0) {
            static_cast<void>(WindowDrag_ResetMouse());
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX)));
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY)));
            static_cast<void>(WindowDrag_StartAtPosition(3, sourceIndex, x, y, WindowPetDrag_Handle));
        }
    } else {
        if ((clicked & 1u) != 0) {
            static_cast<void>(WindowDrag_ResetMouse());
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX)));
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY)));
            static_cast<void>(WindowDrag_StartAtPosition(3, sourceIndex, x, y, WindowPetDrag_Handle));
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            WindowDrag_Cancel();
        }
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t drawMode = gPetDragModeD2EA1C;
    if (drawMode == 1) {
        const std::int32_t drawKind = WindowItem_ClassifyPetSlot(sourceIndex);
        if (drawKind == 1) {
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                (platform::gLegacyMouseState.mouseY - 35)));
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                (platform::gLegacyMouseState.mouseX - 120)));
            WindowBank_DrawOwnPetTooltip(x, y, sourceIndex, 5);
        } else if (drawKind == 2) {
            const std::int32_t slot = sourceIndex - 100;
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                (platform::gLegacyMouseState.mouseY - 35)));
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                (platform::gLegacyMouseState.mouseX - 120)));
            WindowBank_DrawStoredPetTooltip(x, y, slot, 5);
        } else {
            return;
        }
        const std::int32_t y =
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
        const std::int32_t x =
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
        static_cast<void>(cg::render::DrawGraphic(x, y, 230, 243141, 0, &descriptor));
    } else if (drawMode == 0 && WindowItem_ClassifyPetSlot(sourceIndex) == 1) {
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY)));
        static_cast<void>(WindowPet_DrawTooltip(x, y, sourceIndex, 5, 230, color));
    }
}

// 原函数：0x0048C130。
std::int32_t WindowPetDrag_Start(const std::int32_t sourceIndex, const std::int32_t mode)
{
    gPetDragModeD2EA1C = mode;
    return WindowDrag_Start(3, sourceIndex, WindowPetDrag_Handle);
}

// IDE01：原189条；动作及两点击BYTE/三descriptor持址明确，两fresh分类/取消后绘制保持，完整owner及运行待。
// 原函数：0x0048BC90。
static void WindowItemDrag_Handle(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (message != 0) {
        if (message == 2 && gItemDragModeCC4C9C != 0
            && (world::ReadPlayerActionFlagsByte() & 0x80u) == 0
            && world::IsObservationSceneTransitionActive_440B50() == 0) {
            const std::int32_t group =
                std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28) > 0 && sourceIndex < 6
                ? WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10) + 1 : 0;
            const std::int32_t y =
                static_cast<std::int32_t>(world::gPlayerY);
            const std::int32_t x =
                static_cast<std::int32_t>(world::gPlayerX);
            static_cast<void>(network::DropItemRequest_510CE0(
                platform::gConnectionSocket, x, y, group, sourceIndex));
        }
        return;
    }
    const std::int32_t kind = WindowItem_ClassifyItemSlot(sourceIndex);
    if (kind == 1) {
        const world::PlayerItemRecord* item;
        if (std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28) > 0) {
            item = &world::gPetRuntimeRecords[
                WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10)].items6E4[sourceIndex];
        } else {
            item = &world::gPlayerRuntimeSlots.items[sourceIndex];
        }
        if (item->used == 0) {
            static_cast<void>(WindowDrag_ResetMouse());
        }
    } else if (kind == 2) {
        if (world::gPlayerRuntimeSlots.items[sourceIndex].used == 0) {
            static_cast<void>(WindowDrag_ResetMouse());
        }
    } else if (kind == 3) {
        std::uint16_t used;
        std::memcpy(
            &used, &gItemDetailStorageCC4D58[0].items[sourceIndex - 100], sizeof(used));
        if (used == 0) {
            static_cast<void>(WindowDrag_ResetMouse());
        }
    }
    if ((*reinterpret_cast<const unsigned char*>(&platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
        static_cast<void>(WindowDrag_ResetMouse());
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY)));
        static_cast<void>(WindowDrag_StartAtPosition(2, sourceIndex, x, y, WindowItemDrag_Handle));
    }
    if ((*reinterpret_cast<const unsigned char*>(&platform::gLegacyMouseState.edgeButtons) & 2u) != 0) {
        WindowDrag_Cancel();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t drawKind = WindowItem_ClassifyItemSlot(sourceIndex);
    std::int32_t imageNo;
    const render::GraphicDescriptor16* drawDescriptor;
    if (drawKind == 1) {
        const std::int8_t mode = std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28);
        drawDescriptor = &descriptor;
        if (mode > 0) {
            imageNo = world::gPetRuntimeRecords[
                WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10)].items6E4[sourceIndex].imageNo;
        } else {
            imageNo = world::gPlayerRuntimeSlots.items[sourceIndex].imageNo;
        }
    } else if (drawKind == 2) {
        drawDescriptor = &descriptor;
        imageNo = world::gPlayerRuntimeSlots.items[sourceIndex].imageNo;
    } else if (drawKind == 3) {
        drawDescriptor = &descriptor;
        std::memcpy(&imageNo,
            &gItemDetailStorageCC4D58[0].items[sourceIndex - 100].imageNo, sizeof(imageNo));
    } else {
        return;
    }
    const std::int32_t y =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
    const std::int32_t x =
        static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
    static_cast<void>(cg::render::DrawGraphic(x, y, 230, imageNo, 0, drawDescriptor));
}

// IDE01：原九条；mode S32先写再真实kind2调用/结果透传，正确正文保，十三父/owner/运行待。
// 原函数：0x0048BF10。
std::int32_t WindowItemDrag_Start(const std::int32_t sourceIndex, const std::int32_t mode)
{
    gItemDragModeCC4C9C = mode;
    return WindowDrag_Start(2, sourceIndex, WindowItemDrag_Handle);
}

// NPI01：原474550完整540条；元素X/Y先坐标，四绘制按原descriptor/图号先后取样，逐键CryptInt/优先级及AL结果保。
// 原函数：0x00474550。
std::uint8_t WindowInput_HandleNumber(
    WindowInstance* const window, const std::int32_t index, const std::int32_t state,
    const std::int32_t maximum, const std::uint8_t layer, const wchar_t* const* const hints,
    const std::uint8_t flags, const std::uint32_t color)
{
    WindowElement* const elements = window->elements;
    const std::int32_t windowY = window->y;
    const std::int32_t windowX = window->x;
    const std::int32_t elementX = elements[index].x;
    const std::int32_t elementY = elements[index].y;
    const std::int32_t x = windowX + elementX;
    const std::int32_t y = windowY + elementY;
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    // 每个键独立短路读取四次CryptInt，不能用一次坐标快照替代。
    const auto hit = [](const std::int32_t left, const std::int32_t top,
                       const std::int32_t right, const std::int32_t bottom) {
        return (static_cast<std::int32_t>(left) <= platform::gLegacyMouseState.mouseX)
            && (platform::gLegacyMouseState.mouseX < static_cast<std::int32_t>(right))
            && (static_cast<std::int32_t>(top) <= platform::gLegacyMouseState.mouseY)
            && (platform::gLegacyMouseState.mouseY < static_cast<std::int32_t>(bottom));
    };
    std::int32_t selected = -1;
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t bottom = y + 57 - row * 20;
        for (std::int32_t column = 0; column < 3; ++column) {
            const std::int32_t digit = 1 + row * 3 + column;
            const std::int32_t left = x + column * 24;
            std::int32_t imageState = 0;
            if (hit(left, bottom - 19, left + 21, bottom)) {
                gHoverTextD68AE8 = hints[0];
                imageState = (state & 8) != 0 ? 1 : 2;
                if ((state & 2) != 0) {
                    selected = digit;
                }
            }
            const auto* const digitDescriptor = &descriptor;
            const std::int32_t digitImage = gNumericKeyImages54F030[digit][imageState];
            static_cast<void>(cg::render::DrawGraphic(
                left + 10, bottom - 9, layer, digitImage, 0, digitDescriptor));
        }
    }
    std::int32_t imageState = 0;
    if (hit(x, y + 58, x + 21, y + 77)) {
        gHoverTextD68AE8 = hints[0];
        imageState = (state & 8) != 0 ? 1 : 2;
        if ((state & 2) != 0) {
            selected = 0;
        }
    }
    const std::int32_t zeroImage = gNumericKeyImages54F030[0][imageState];
    const auto* const zeroDescriptor = &descriptor;
    static_cast<void>(cg::render::DrawGraphic(
        x + 10, y + 68, layer, zeroImage, 0, zeroDescriptor));
    imageState = 0;
    if (hit(x + 48, y + 58, x + 69, y + 77)) {
        if ((flags & 1u) != 0) {
            gHoverTextD68AE8 = hints[1];
            imageState = (state & 8) != 0 ? 1 : 2;
            if ((state & 2) != 0) {
                selected = 13;
            }
        } else {
            const std::int32_t value = std::bit_cast<std::int32_t>(window->elements[index].state);
            gHoverTextD68AE8 = hints[value > 0 ? 1 : 2];
            imageState = (state & 8) != 0 ? 1 : 2;
            const std::int32_t current = std::bit_cast<std::int32_t>(window->elements[index].state);
            if (current > 0) {
                if ((state & 2) != 0) {
                    selected = 13;
                }
            } else if ((state & 2) != 0) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
        }
    }
    if ((flags & 1u) == 0 && selected < 0
        && std::bit_cast<std::int32_t>(window->elements[index].state) <= 0) {
        imageState = 1;
    }
    const std::int32_t confirmImage = gNumericKeyImages54F030[13][imageState];
    const auto* const confirmDescriptor = &descriptor;
    static_cast<void>(cg::render::DrawGraphic(
        x + 58, y + 68, layer, confirmImage, 0, confirmDescriptor));
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t centerY = y + 28 + row * 20;
        imageState = 0;
        if (hit(x + 75, centerY - 10, x + 100, centerY + 9)) {
            gHoverTextD68AE8 = hints[row + 3];
            imageState = (state & 8) != 0 ? 1 : 2;
            if ((state & 2) != 0) {
                selected = row + 10;
            }
        }
        const auto* const operationDescriptor = &descriptor;
        const std::int32_t operationImage = gNumericKeyImages54F030[row + 10][imageState];
        static_cast<void>(cg::render::DrawGraphic(
            x + 87, centerY, layer, operationImage, 0, operationDescriptor));
    }
    if ((flags & 2u) != 0) {
        for (std::int32_t digit = 0; digit < 10; ++digit) {
            if (platform::ReadKeyFlag4_4597A0(48 + digit) != 0
                || platform::ReadKeyFlag4_4597A0(96 + digit) != 0) {
                selected = digit;
                break;
            }
        }
    }
    if (platform::ReadKeyFlag4_4597A0(8) != 0) {
        WindowElement* const element = &window->elements[index];
        const std::int32_t value = std::bit_cast<std::int32_t>(element->state);
        element->state = static_cast<std::uint32_t>(value / 10);
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return 1;
    }
    if (selected >= 0 && selected <= 9) {
        WindowElement* const multiplyTarget = &window->elements[index];
        const std::int32_t previous = std::bit_cast<std::int32_t>(multiplyTarget->state);
        multiplyTarget->state = static_cast<std::uint32_t>(previous * 10);
        WindowElement* const appendTarget = &window->elements[index];
        const std::int32_t multiplied = std::bit_cast<std::int32_t>(appendTarget->state);
        appendTarget->state = static_cast<std::uint32_t>(multiplied + selected);
        WindowElement* const element = &window->elements[index];
        if (std::bit_cast<std::int32_t>(element->state) > maximum) {
            element->state = static_cast<std::uint32_t>(maximum);
        }
    }
    if (selected == 10) {
        WindowElement* const element = &window->elements[index];
        const std::int32_t value = std::bit_cast<std::int32_t>(element->state);
        element->state = static_cast<std::uint32_t>(value / 10);
    } else if (selected == 11) {
        window->elements[index].state = static_cast<std::uint32_t>(maximum);
    } else if (selected == 12) {
        window->elements[index].state = 0;
    } else if (selected == 13) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        return 3;
    } else if (selected < 0) {
        return 0;
    }
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    return 1;
}

// WCL01：原4C1FB0完整53条；持同窗口/graphic，bit2仅置关闭位与SE57，图号优先序保，返回0/1。
// 原函数：0x004C1FB0。
std::int32_t WindowRuntime_ButtonClose(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    const bool hover = (state & 1) != 0;
    if (hover) {
        gHoverTextD68AE8 = L"关闭这个视窗。";
    }
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        result = 1;
    }
    graphic->imageNo = 0x3B538;
    if (hover) {
        graphic->imageNo = 0x3B53A;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B539;
    }
    return result;
}

// 原0x004FBA30：UTF-16色码分段与即时测宽；正文使用动态宽字符串。
std::int32_t WindowItem_SplitTooltipLines(
    std::int32_t x, const std::int32_t y, const std::uint8_t renderMode,
    const std::int32_t fontType, std::int8_t color, std::wstring_view text,
    const std::int8_t value7, const std::int8_t marker)
{
    for (std::size_t delimiter = text.find(L'$'); delimiter != std::wstring_view::npos;
         delimiter = text.find(L'$')) {
        std::size_t length = delimiter;
        const wchar_t next = text[delimiter + 1];
        if (next == L'$') { ++length; }
        const std::wstring_view prefix = text.substr(0, length);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)),
            renderMode, static_cast<std::int8_t>(render::ResolveFontId(fontType)),
            color, prefix, value7, marker));
        x += cg::render::TextMeasure(prefix, fontType);
        text.remove_prefix(delimiter + 2);
        if (next != L'$') {
            const std::int32_t digit = next - L'0';
            if (digit >= 0 && digit <= 9) { gTooltipColorD67D30 = digit; color = static_cast<std::int8_t>(digit); }
        }
    }
    return render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)),
        renderMode, static_cast<std::int8_t>(render::ResolveFontId(fontType)),
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gTooltipColorD67D30)),
        text, value7, marker);
}

// TDR01：原48B810完整108条；层5清尾/题目/逐行颜色/可选footer后背景入队，原坐标截断及真实返回保持。
template <typename ReadLine>
static std::int32_t WindowItem_DrawTooltipLines(
    const std::int32_t x, const std::int32_t y,
    const wchar_t* const title, const std::int8_t titleColor,
    const ReadLine& readLine, const std::int32_t lineCount,
    const wchar_t* const footer, const std::int8_t footerColor)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, L"", 0, 0));
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 10)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 10)),
            5, 12, titleColor, title, 0, 0));
    }
    gTooltipColorD67D30 = 0;
    std::int32_t lineY = 0;
    for (std::int32_t line = 0; line < lineCount; ++line) {
        const auto contents = readLine(line);
        if (!contents.empty()) {
            const std::int32_t shortY =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
            const std::int32_t shortX =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
            static_cast<void>(WindowItem_SplitTooltipLines(
                shortX + 10, shortY + lineY + 22, 5, 12, 0, contents, 0, 0));
        }
        lineY += 13;
    }
    if (footer != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 10)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 127)),
            5, 12, footerColor, footer, 0, 0));
    }
    return render::DrawGraphic3_520970(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)),
        0xE6, 0x3BA5D, 0, 1, 0, &descriptor);
}

// 原0x0048B810：已迁移的运行时描述直接借宽行，逐行读序保持。
std::int32_t WindowItem_DrawTooltip(
    const std::int32_t x, const std::int32_t y,
    const wchar_t* const title, const std::int8_t titleColor,
    const std::wstring* const lines, const std::int32_t lineCount,
    const wchar_t* const footer, const std::int8_t footerColor)
{
    return WindowItem_DrawTooltipLines(x, y, title, titleColor,
        [lines](const std::int32_t line) { return std::wstring_view(lines[line]); },
        lineCount, footer, footerColor);
}

// SIC01：入口预览地址贯穿十成员构造与字段发布；原初始化/三descriptor及this返回保持，整对象不清零。
// 原函数：0x004C61B0。
WindowItemSelectorState* WindowItemSelector_ConstructPreview(WindowItemSelectorState* const state, const std::int32_t previewIndex)
{
    std::uint32_t flags;
    cg::yobi::YobiAction* const preview = cg::yobi::ConstructWindowSelectorPreview(previewIndex, flags);
    state->preview = preview;
    state->initialized3E8 = 0;
    state->value64 = 0;
    state->step6C = 21;
    state->previousPet74 = -1;
    state->selectedPet70 = -1;
    state->value3EC = 1;
    state->value3ED = 1;
    state->imageIndex3F0 = 0;
    state->imageIndex3F4 = 0;
    cg::yobi::FinishWindowSelectorPreview(preview, flags);
    state->hoverIndex3B0 = -1;
    state->hoverIndex3F8 = -1;
    state->bytes3B4.fill(std::byte{0});
    state->hoverCount3A8 = 0;
    state->bytes3AC.fill(std::byte{0});
    state->value3FC = 0;
    for (std::size_t index = 0; index < state->descriptors3B8.size(); ++index) {
        auto& descriptor = state->descriptors3B8[index];
        descriptor = {};
        const std::uint32_t color = index == 0 ? 0xFFFFFFFFu : 0x80FFFFFFu;
        descriptor.color = color;
        descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    }
    return state;
}

// SLC01：单读并返回同一选择器selectedPet70的完整S32，不做校验或改owner。
// 原函数：0x004C71B0。
std::int32_t WindowItemSelector_ReadSelectedPet(const WindowItemSelectorState* const state)
{
    return state->selectedPet70;
}

// SFD01：低BYTE模式装配非拥有元素/四回调；逐次借当前窗口，非零模式槽19不写，发布次序保持。
// 原函数：0x004C6EC0。
void WindowItemSelector_BindElements(WindowItemSelectorState* const state, const std::uint8_t mode)
{
    state->value64 = mode;
    if (mode != 0) {
        state->mode68 = &gBattleItemActorSelectionMode_57E42C;
        state->elements00[0] = &gCurrentWindow->elements[3];
        state->elements00[1] = &gCurrentWindow->elements[20];
        state->elements00[2] = &gCurrentWindow->elements[16];
        state->elements00[3] = &gCurrentWindow->elements[17];
        state->elements00[4] = &gCurrentWindow->elements[18];
        state->elements00[5] = &gCurrentWindow->elements[19];
        state->elements00[6] = &gCurrentWindow->elements[4];
        state->elements00[7] = &gCurrentWindow->elements[5];
        state->elements00[9] = &gCurrentWindow->elements[10];
        state->elements00[10] = &gCurrentWindow->elements[11];
        state->elements00[11] = &gCurrentWindow->elements[12];
        state->elements00[12] = &gCurrentWindow->elements[13];
        state->elements00[13] = &gCurrentWindow->elements[14];
        state->elements00[14] = &gCurrentWindow->elements[15];
        state->elements00[8] = &gCurrentWindow->elements[6];
        state->elements00[15] = &gCurrentWindow->elements[7];
        state->elements00[16] = &gCurrentWindow->elements[8];
        state->elements00[17] = &gCurrentWindow->elements[9];
        WindowElement* const last = &gCurrentWindow->elements[21];
        state->callbacks50[0] = WindowBattleItems_HandleInventory;
        state->elements00[18] = last;
        state->callbacks50[1] = WindowElement_NoOp;
        state->callbacks50[2] = WindowBattleItems_HandleEquipment;
        state->callbacks50[3] = WindowElement_NoOp;
    } else {
        state->mode68 = &gInventoryActorSelectionMode_57DA28;
        state->elements00[0] = &gCurrentWindow->elements[4];
        state->elements00[1] = &gCurrentWindow->elements[17];
        state->elements00[2] = &gCurrentWindow->elements[18];
        state->elements00[3] = &gCurrentWindow->elements[19];
        state->elements00[4] = &gCurrentWindow->elements[20];
        state->elements00[5] = &gCurrentWindow->elements[21];
        state->elements00[6] = &gCurrentWindow->elements[5];
        state->elements00[7] = &gCurrentWindow->elements[6];
        state->elements00[9] = &gCurrentWindow->elements[11];
        state->elements00[10] = &gCurrentWindow->elements[12];
        state->elements00[11] = &gCurrentWindow->elements[13];
        state->elements00[12] = &gCurrentWindow->elements[14];
        state->elements00[13] = &gCurrentWindow->elements[15];
        state->elements00[14] = &gCurrentWindow->elements[16];
        state->elements00[8] = &gCurrentWindow->elements[7];
        state->elements00[15] = &gCurrentWindow->elements[8];
        state->elements00[16] = &gCurrentWindow->elements[9];
        state->elements00[17] = &gCurrentWindow->elements[10];
        state->elements00[18] = &gCurrentWindow->elements[23];
        state->elements00[19] = &gCurrentWindow->elements[22];
        state->callbacks50[0] = WindowInventory_HandleItems;
        state->callbacks50[1] = WindowInventory_HandleItemSelection;
        state->callbacks50[2] = WindowInventory_HandleEquipment;
        state->callbacks50[3] = WindowInventory_DrawBlockedEquipment;
    }
}

// SLC01：负slot先返-1；S8 mode0取8项玩家X表，正mode取6项宠物X表，其它返-1。
// 原函数：0x004C63C0。
static std::int32_t WindowItemSelector_ReadSlotX(
    const WindowItemSelectorState* const state, const std::int32_t slot)
{
    if (slot < 0) {
        return -1;
    }
    const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
    if (mode == 0 && slot < 8) {
        return gSelectorOwnX54F584[slot];
    }
    if (mode <= 0 || slot >= 6) {
        return -1;
    }
    return gSelectorPetX54F5C4[slot];
}

// SLC01：负slot先返-1；S8 mode0取8项玩家Y表，正mode取6项宠物Y表，其它返-1。
// 原函数：0x004C6400。
static std::int32_t WindowItemSelector_ReadSlotY(
    const WindowItemSelectorState* const state, const std::int32_t slot)
{
    if (slot < 0) {
        return -1;
    }
    const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
    if (mode == 0 && slot < 8) {
        return gSelectorOwnY54F5A4[slot];
    }
    if (mode <= 0 || slot >= 6) {
        return -1;
    }
    return gSelectorPetY54F5DC[slot];
}

// SPG01：玩家页2显示槽6；W32装配模式0显示槽19，两门槛独立，原正文保持。
// 原函数：0x004C6440。
static void WindowItemSelector_ShowPlayerPage(WindowItemSelectorState* const state)
{
    if (state->value60 == 2) {
        state->elements00[6]->visible = 1;
    }
    if (state->value64 == 0) {
        state->elements00[19]->visible = 1;
    }
}

// SPG01：宠物页/名称负重及原动态空位显示保持；只判低AL，不钳制itemSlots或补保护。
// 原函数：0x004C6470。
static void WindowItemSelector_ShowPetPage(WindowItemSelectorState* const state)
{
    if (state->value60 == 3) {
        state->elements00[7]->visible = 1;
        state->elements00[8]->visible = 1;
    }
    state->elements00[15]->visible = 1;
    state->elements00[16]->visible = 1;
    const std::int32_t itemSlots =
        state->value3ED != 0 && world::IsValidPetIndex_51C410(state->selectedPet70)
        ? world::gPetRuntimeRecords[state->selectedPet70].values6D8[0] : 0;
    for (std::int32_t position = itemSlots; position < 6; ++position) {
        state->elements00[9 + position]->visible = 1;
    }
    state->elements00[17]->visible = 1;
    if (state->value64 == 0) {
        state->elements00[19]->visible = 1;
    }
}

// SPG01：按原顺序隐藏槽6/7/8/15/16、六装备槽与17；模式0才隐藏19，主元素不改。
// 原函数：0x004C6500。
static void WindowItemSelector_HidePageElements(WindowItemSelectorState* const state)
{
    state->elements00[6]->visible = 0;
    state->elements00[7]->visible = 0;
    state->elements00[8]->visible = 0;
    state->elements00[15]->visible = 0;
    state->elements00[16]->visible = 0;
    for (std::int32_t position = 9; position < 15; ++position) {
        state->elements00[position]->visible = 0;
    }
    state->elements00[17]->visible = 0;
    if (state->value64 == 0) {
        state->elements00[19]->visible = 0;
    }
}

// SPG01：先发布页再隐藏/新页分发；CC40D4仅低BYTE、四回调先取元素；原五页和写序保持，全机器ABI待。
// 原函数：0x004C6550。
static void WindowItemSelector_SelectPage(WindowItemSelectorState* const state, const std::int32_t page)
{
    if (page == 1 && std::bit_cast<std::int8_t>(*state->mode68) > 0) {
        static_cast<void>(WindowItemSelector_SelectPet(state));
        if (!world::IsValidPetIndex_51C410(state->selectedPet70)
            || world::gPetRuntimeRecords[state->selectedPet70].used == 0) {
            static_cast<void>(WindowItemSelector_SetMode(state, 0));
        }
    }
    state->value60 = page;
    WindowItemSelector_HidePageElements(state);
    switch (state->value60) {
    case 0:
        state->elements00[1]->mouseCallback = WindowElement_NoOp;
        state->elements00[1]->visible = 0;
        WindowItemSelector_MoveElements(state, 16);
        static_cast<void>(WindowItemSelector_SetMode(state, -1));
        break;
    case 1: {
        const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
        if (mode == 0) {
            WindowItemSelector_ShowPlayerPage(state);
            static_cast<WindowGraphicData*>(state->elements00[1]->data)->imageNo = 98146;
        } else if (mode > 0) {
            WindowItemSelector_ShowPetPage(state);
            static_cast<WindowGraphicData*>(state->elements00[1]->data)->imageNo = 98147;
            if (world::IsValidPetIndex_51C410(state->selectedPet70)) {
                for (std::int32_t position = 0;
                     position < world::gPetRuntimeRecords[state->selectedPet70].values6D8[0];
                     ++position) {
                    state->elements00[9 + position]->visible = 1;
                }
            }
        }
        state->elements00[1]->mouseCallback = WindowElement_NoOp;
        state->elements00[1]->visible = 1;
        state->step6C = 21;
        break;
    }
    case 2:
        *state->mode68 = 0;
        WindowItemSelector_ShowPlayerPage(state);
        if (state->value64 != 0
            || *reinterpret_cast<const unsigned char*>(&gItemSelectionModeCC40D4) == 0) {
            WindowElement* const element = state->elements00[1];
            const WindowElementCallback callback = state->callbacks50[2];
            element->mouseCallback = callback;
        } else {
            WindowElement* const firstElement = state->elements00[0];
            const WindowElementCallback firstCallback = state->callbacks50[1];
            firstElement->mouseCallback = firstCallback;
            WindowElement* const secondElement = state->elements00[1];
            const WindowElementCallback secondCallback = state->callbacks50[3];
            secondElement->mouseCallback = secondCallback;
        }
        WindowItemSelector_MoveElements(state, -182);
        state->elements00[1]->visible = 1;
        static_cast<WindowGraphicData*>(state->elements00[1]->data)->imageNo = 98146;
        break;
    case 3:
        WindowItemSelector_MoveElements(state, -182);
        WindowItemSelector_RefreshPetHeader(state);
        WindowItemSelector_ShowPetPage(state);
        if (state->value3ED == 1) {
            WindowElement* const element = state->elements00[1];
            const WindowElementCallback callback = state->callbacks50[2];
            element->mouseCallback = callback;
        } else {
            state->elements00[1]->mouseCallback = WindowElement_NoOp;
        }
        state->elements00[1]->visible = 1;
        static_cast<WindowGraphicData*>(state->elements00[1]->data)->imageNo = 98147;
        break;
    case 4:
        state->value60 = 0;
        WindowItemSelector_MoveElements(state, 16);
        state->elements00[1]->mouseCallback = WindowElement_NoOp;
        state->elements00[1]->visible = 0;
        static_cast<void>(WindowItemSelector_SetMode(state, -1));
        break;
    default:
        break;
    }
}

// SFD01：原四页分发/业务钳制保持；两按钮按原顺序取样，共借descriptor[0]；已知上行由窗口回调返回1。
// 原函数：0x004C6790。
static void WindowItemSelector_UpdatePage(WindowItemSelectorState* const state)
{
    // 原常量参考：0x0054F564。
    static constexpr std::int32_t kPreviewToggleImages[] {98133, 98133, 98135, 98134};
    // 原常量参考：0x0054F574。
    static constexpr std::int32_t kEquipmentToggleImages[] {98137, 98137, 98139, 98138};
    const std::int32_t page = state->value60;
    if (page > 0 && page <= 2 && std::bit_cast<std::int8_t>(*state->mode68) > 0) {
        static_cast<void>(WindowItemSelector_SelectPet(state));
        if (!world::IsValidPetIndex_51C410(state->selectedPet70)
            || world::gPetRuntimeRecords[state->selectedPet70].used == 0) {
            static_cast<void>(WindowItemSelector_SetMode(state, 0));
        }
    }
    switch (state->value60) {
    case 1: {
        if (std::bit_cast<std::int8_t>(*state->mode68) > 0) {
            static_cast<void>(WindowItemSelector_SelectPet(state));
        }
        const std::int32_t x = state->elements00[1]->x;
        if (x > -182) {
            WindowItemSelector_MoveElements(state, x - state->step6C);
            const std::int32_t step = state->step6C;
            const WindowElement* const element = state->elements00[1];
            state->step6C = step - 1;
            if (element->x <= -182) {
                WindowItemSelector_MoveElements(state, -182);
            }
        } else {
            WindowItemSelector_SelectPage(state, std::bit_cast<std::int8_t>(*state->mode68) > 0 ? 3 : 2);
        }
        break;
    }
    case 2:
        if (std::bit_cast<std::int8_t>(*state->mode68) > 0) {
            WindowItemSelector_SelectPage(state, 3);
        } else {
            WindowItemSelector_DrawTooltip(state);
        }
        break;
    case 3: {
        static_cast<void>(WindowItemSelector_SelectPet(state));
        if (!world::IsValidPetIndex_51C410(state->selectedPet70)
            || world::gPetRuntimeRecords[state->selectedPet70].used == 0) {
            *state->mode68 = 0;
        }
        if (*state->mode68 == 0) {
            WindowItemSelector_SelectPage(state, 2);
            break;
        }
        WindowItemSelector_DrawTooltip(state);
        WindowItemSelector_RefreshPetHeader(state);
        const std::int32_t firstIndex = state->imageIndex3F0;
        const WindowElement* const firstElement = state->elements00[1];
        const render::GraphicDescriptor16* const descriptor = &state->descriptors3B8[0];
        const std::int32_t firstImage = kPreviewToggleImages[firstIndex];
        const WindowInstance* const firstWindow = gCurrentWindow;
        const std::int32_t firstWindowY = firstWindow->y;
        const std::int32_t firstWindowX = firstWindow->x;
        const std::int32_t firstElementY = firstElement->y;
        const std::int32_t firstElementX = firstElement->x;
        const std::int32_t firstY = firstElementY + firstWindowY + 180;
        const std::int32_t firstX = firstElementX + firstWindowX + 85;
        static_cast<void>(cg::render::DrawGraphic(
            firstX, firstY, 223, firstImage, 0, descriptor));
        const std::int32_t secondIndex = state->imageIndex3F4;
        const WindowElement* const secondElement = state->elements00[1];
        const std::int32_t secondImage = kEquipmentToggleImages[secondIndex];
        const WindowInstance* const secondWindow = gCurrentWindow;
        const std::int32_t secondElementY = secondElement->y;
        const std::int32_t secondWindowY = secondWindow->y;
        const std::int32_t secondElementX = secondElement->x;
        const std::int32_t secondWindowX = secondWindow->x;
        const std::int32_t secondY = secondElementY + secondWindowY + 180;
        const std::int32_t secondX = secondElementX + secondWindowX + 118;
        static_cast<void>(cg::render::DrawGraphic(
            secondX, secondY, 223, secondImage, 0, descriptor));
        static_cast<void>(WindowItemSelector_DrawPreview(state));
        break;
    }
    case 4:
        WindowItemSelector_SelectPage(state, 0);
        break;
    default:
        break;
    }
}

// PSP01：同state mode按S8映射，回调返回的S32索引原序发布到selectedPet70并返回。
// 原函数：0x004C6990。
static std::int32_t WindowItemSelector_SelectPet(WindowItemSelectorState* const state)
{
    const std::int32_t index = WindowItemSelector_FindPetIndex(std::bit_cast<std::int8_t>(*state->mode68));
    state->selectedPet70 = index;
    return index;
}

// PSP01：同宠物previous/updated BYTE及名字/负重原序保持；首次测宽先字体后文本，负重先文本后字体。
// 原函数：0x004C69B0。
static void WindowItemSelector_RefreshPetHeader(WindowItemSelectorState* const state)
{
    if (state->selectedPet70 == -1 || !world::IsValidPetIndex_51C410(state->selectedPet70)) {
        return;
    }
    const std::int32_t selected = state->selectedPet70;
    auto* const pet = &world::gPetRuntimeRecords[selected];
    if (selected == state->previousPet74 && pet->updated510C == 0) {
        return;
    }
    state->previousPet74 = selected;
    pet->updated510C = 0;
    WindowElement* const nameElement = state->elements00[7];
    auto* const nameData = static_cast<WindowTextData*>(nameElement->data);
    const char* const name = pet->name6BD[0] != '\0' ? pet->name6BD.data() : pet->name6AC.data();
    nameData->text = cg::text::FromGbkWide(name);
    const std::uint8_t nameFont = std::to_integer<std::uint8_t>(nameData->style.font);
    const std::wstring& nameText = nameData->text;
    const std::int32_t nameWidth = cg::render::TextMeasure(nameText, nameFont);
    nameElement->x = state->elements00[1]->x - nameWidth / 2 + 101;
    WindowElement* const weightElement = state->elements00[8];
    const std::int32_t maximumWeight = pet->values6D8[2];
    const std::int32_t weight = pet->values6D8[1];
    auto* const weightData = static_cast<WindowTextData*>(weightElement->data);
    cg::text::FormatTo(weightData->text, L"负重:{:3}/{:3}", weight, maximumWeight);
    const std::wstring& weightText = weightData->text;
    const std::uint8_t weightFont = std::to_integer<std::uint8_t>(weightData->style.font);
    const std::int32_t weightWidth = cg::render::TextMeasure(weightText, weightFont);
    weightElement->x = state->elements00[1]->x - weightWidth / 2 + 101;
}

// SPG01：依原序移动主元素、四按钮及六表坐标；原S32结果与固定循环保持，不新增钳制。
// 原函数：0x004C6AC0。
static void WindowItemSelector_MoveElements(WindowItemSelectorState* const state, const std::int32_t x)
{
    state->elements00[1]->x = x;
    for (std::int32_t position = 2; position < 6; ++position) {
        state->elements00[position]->x = x - 15;
    }
    for (std::int32_t position = 0; position < 6; ++position) {
        state->elements00[9 + position]->x = gSelectorPetX54F5C4[position] + x - 24;
    }
}

// SLC01：两轴先窗口再元素取值，flags/淡入计数只读原BYTE；void不证明原EAX上行等价。
// 原函数：0x004C6B10。
static void WindowItemSelector_DrawTooltip(WindowItemSelectorState* const state)
{
    if (state->value3ED == 0 || state->hoverIndex3F8 < 0) {
        state->hoverCount3A8 = 0;
        return;
    }
    const std::int32_t localX = WindowItemSelector_ReadSlotX(state, state->hoverIndex3F8);
    const WindowElement* const horizontalElement = state->elements00[1];
    const WindowInstance* const horizontalWindow = gCurrentWindow;
    const std::int32_t windowX = horizontalWindow->x;
    const std::int32_t elementX = horizontalElement->x;
    const std::int32_t left = localX + (windowX + elementX) - 24;
    const std::int32_t localY = WindowItemSelector_ReadSlotY(state, state->hoverIndex3F8);
    const WindowElement* const verticalElement = state->elements00[1];
    const WindowInstance* const verticalWindow = gCurrentWindow;
    const std::int32_t windowY = verticalWindow->y;
    const std::int32_t elementY = verticalElement->y;
    const std::int32_t top = localY + (windowY + elementY) - 24;
    cg::render::DrawGraphicHover(
        static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
        static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
        223, cg::render::gHoverFramePaletteIndex_D69768, 0);
    const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
    std::int32_t slot;
    world::PlayerItemRecord* item;
    if (mode == 0) {
        slot = state->hoverIndex3F8;
        item = &world::gPlayerRuntimeSlots.items[slot];
    } else {
        if (mode < 0 || state->selectedPet70 < 0) {
            return;
        }
        slot = state->hoverIndex3F8;
        const std::int32_t pet = WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10);
        item = &world::gPetRuntimeRecords[pet].items6E4[slot];
    }
    if (item->used != 0) {
        const auto* const occupancy =
            reinterpret_cast<const unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
        if (occupancy[slot] != 0) {
            cg::render::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, 249, 0);
            if ((*reinterpret_cast<const unsigned char*>(&item->flags) & 0x80u) == 0) {
                static_cast<void>(cg::render::DrawGraphic(
                    left + 129, top + 117, 223, 22739, 0, &state->descriptors3B8[2]));
            }
            static_cast<void>(cg::render::DrawGraphic(
                left + 24, top + 24, 223, item->imageNo, 0, &state->descriptors3B8[1]));
        } else {
            reinterpret_cast<unsigned char*>(&state->descriptors3B8[2].color)[3] = 5 * state->hoverCount3A8 >= 256
                ? 0xFF : static_cast<std::uint8_t>(
                    5 * (*reinterpret_cast<const unsigned char*>(&state->hoverCount3A8)));
            if ((*reinterpret_cast<const unsigned char*>(&item->flags) & 0x80u) == 0) {
                static_cast<void>(cg::render::DrawGraphic(
                    left + 129, top + 117, 223, 22739, 0, &state->descriptors3B8[2]));
            }
            static_cast<void>(cg::render::DrawGraphic(
                left + 24, top + 24, 223, item->imageNo, 0, &state->descriptors3B8[2]));
        }
    }
    static_cast<void>(cg::render::DrawGraphic(
        left + 24, top + 24, 223, 243139, 0, &state->descriptors3B8[0]));
    const std::int32_t hovered = state->hoverIndex3F8;
    if (state->hoverIndex3B0 == hovered) {
        const std::int32_t count = state->hoverCount3A8;
        state->hoverIndex3F8 = -1;
        state->hoverCount3A8 = count + 1;
    } else {
        state->hoverCount3A8 = 0;
        state->hoverIndex3B0 = hovered;
        state->hoverIndex3F8 = -1;
    }
}

// PSP01：同一非拥有预览推进/两轴/队列；掩码替换字节先旧字节，返回/原读取门槛保持。
// 原函数：0x004C6D90。
static std::int32_t WindowItemSelector_DrawPreview(WindowItemSelectorState* const state)
{
    const std::int32_t selected = state->selectedPet70;
    if (selected < 0 || state->value3EC == 0) {
        return selected;
    }
    cg::yobi::YobiAction* const preview = state->preview;
    const std::int32_t imageNo = world::gPetRuntimeRecords[selected].imageNo6A0;
    cg::yobi::SetYobiActionField2A8(preview, imageNo);
    const std::int32_t result = cg::yobi::AdvanceYobiAnimationFrame_517880(preview, 100, 0, 0);
    const std::int32_t graphicIndex = cg::yobi::GetYobiActionField20(preview);
    if (graphicIndex == 0) {
        return result;
    }
    const render::GraphicDescriptor16* const descriptor =
        cg::yobi::ApplyWindowAnimationDrawMode(preview, true);
    const std::uint8_t layer = cg::yobi::GetYobiActionField15(preview);
    const std::int32_t offsetY = cg::yobi::GetYobiGraphicOriginOffset_5182B0(preview, 1);
    const std::int32_t y = offsetY + gCurrentWindow->y + 163;
    const std::int32_t offsetX = cg::yobi::GetYobiGraphicOriginOffset_5182B0(preview, 0);
    const std::int32_t elementX = state->elements00[1]->x;
    const std::int32_t x = elementX + offsetX + gCurrentWindow->x + 101;
    return render::DrawGraphic2_521DD0(x, y, layer, graphicIndex, 0, 0, descriptor);
}

// PSP01：五排序对按used W16/索引S16/状态W16计数；不足仍取末扫描项，不新增匹配/索引保护。
// 原函数：0x004C71C0。
static std::int32_t WindowItemSelector_FindPetIndex(const std::int32_t ordinal)
{
    if (ordinal == -1) {
        return -1;
    }
    std::int32_t position = 0;
    std::int32_t matched = 0;
    for (; position < 5; ++position) {
        if (matched >= ordinal) {
            break;
        }
        const auto& pair = world::gPetPairsDE0B08[position];
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            const std::uint16_t flags = world::gPetRuntimeRecords[index].flags6AA;
            if (flags == 2 || flags == 1) {
                ++matched;
            }
        }
    }
    --position;
    return position < 0 ? -1
        : std::bit_cast<std::int16_t>(world::gPetPairsDE0B08[position].index);
}

// SPG01：写mode低BYTE后持旧页；页3按新mode切2/3、页2不切；bool仅承接原AL=1，高位ABI待。
// 原函数：0x004C7340。
static bool WindowItemSelector_SetMode(WindowItemSelectorState* const state, const std::int32_t mode)
{
    *state->mode68 = static_cast<std::uint8_t>(mode);
    const std::int32_t page = state->value60;
    if (page == 2 || page == 3) {
        if (*state->mode68 == 0) {
            if (page != 2) {
                WindowItemSelector_SelectPage(state, 2);
            }
        } else if (page != 2) {
            WindowItemSelector_SelectPage(state, 3);
        }
    }
    return true;
}

// SEV01：页签提示/点击/绘制与图号原序；中心坐标显式取样，descriptor地址先图号；bool限原AL消费。
// 原函数：0x004C7380。
bool WindowItemSelector_HandleTab(
    WindowItemSelectorState* const state, const std::int32_t tab, const std::int32_t events)
{
    // 原常量参考：0x0054F5F4。
    static constexpr std::int32_t kEquipmentTabImages[] {
        244294, 244294, 244295, 244296, 98148, 98131, 98148, 98132,
        98142, 98140, 98142, 98141, 98145, 98143, 98145, 98144,
    };
    WindowElement* const element = state->elements00[tab + 2];
    auto* const graphic = static_cast<WindowGraphicData*>(element->data);
    const bool hovered = (events & 1) != 0;
    bool consumed = false;
    if (hovered) {
        const std::int32_t page = state->value60;
        if (page == 0) {
            gHoverTextD68AE8 = tab == 0
                ? L"开启装备视窗。"
                : L"开启宠物装备。";
        } else if (page > 1 && page <= 3) {
            if (std::bit_cast<std::int8_t>(*state->mode68) == tab) {
                gHoverTextD68AE8 = tab == 0
                    ? L"关闭装备视窗。"
                    : L"关闭宠物装备。";
            } else {
                gHoverTextD68AE8 = tab == 0
                    ? L"切换到身上装备。"
                    : L"切换宠物装备。";
            }
        }
    }
    if ((events & 2) != 0) {
        gCurrentWindow->state |= 0x2000u;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        const std::int32_t page = state->value60;
        if (page == 0) {
            static_cast<void>(WindowItemSelector_SetMode(state, tab));
            WindowItemSelector_SelectPage(state, 1);
        } else if (page > 1 && page <= 3) {
            if (std::bit_cast<std::int8_t>(*state->mode68) == tab) {
                WindowItemSelector_SelectPage(state, 4);
            } else {
                static_cast<void>(WindowItemSelector_SetMode(state, tab));
                WindowItemSelector_SelectPage(state, 1);
            }
        }
        consumed = true;
    }
    const std::int32_t width = element->width;
    const WindowInstance* const window = gCurrentWindow;
    const std::int32_t windowX = window->x;
    const std::int32_t elementX = element->x;
    const std::int32_t x = width / 2 + windowX + elementX + 1;
    const std::int32_t height = element->height;
    const std::int32_t windowY = window->y;
    const std::int32_t elementY = element->y;
    const std::int32_t y = height / 2 + windowY + elementY + 1;
    const std::int32_t page = state->value60;
    if ((page == 2 || page == 3) && std::bit_cast<std::int8_t>(*state->mode68) == tab) {
        const render::GraphicDescriptor16* const descriptor = &state->descriptors3B8[0];
        const std::int32_t image = kEquipmentTabImages[tab * 4 + (hovered ? 3 : 1)];
        static_cast<void>(cg::render::DrawGraphic(x, y, 223, image, 0, descriptor));
    }
    std::int32_t imageIndex;
    if ((events & 8) != 0) {
        imageIndex = 2;
    } else if (hovered) {
        imageIndex = 3;
    } else if (state->value60 != 0) {
        imageIndex = (std::bit_cast<std::int8_t>(*state->mode68) != tab) + 1;
    } else {
        imageIndex = 0;
    }
    graphic->imageNo = kEquipmentTabImages[tab * 4 + imageIndex];
    return consumed;
}

// SEV01：原event低BYTE bit2触发声音后翻转BYTE预览开关，图号8/1优先，正确body保持，bool仅原AL。
// 原函数：0x004C78A0。
bool WindowItemSelector_TogglePreview(WindowItemSelectorState* const state, const std::int32_t events)
{
    bool consumed = false;
    if ((events & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        state->value3EC = state->value3EC == 0;
        consumed = true;
    }
    state->imageIndex3F0 = (events & 8) != 0 ? 2 : ((events & 1) != 0 ? 3 : 1);
    return consumed;
}

// SEV01：先翻BYTE装备/回调，失效AL早返保部分写；目标先回调、尾selected先推进/count后取，不钳制数量。
// 原函数：0x004C7910。
bool WindowItemSelector_CycleEquipment(WindowItemSelectorState* const state, const std::int32_t events)
{
    bool consumed = false;
    if ((events & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        const std::uint8_t previous = state->value3ED;
        const std::uint8_t* const mode = state->mode68;
        const std::uint8_t enabled = previous == 0;
        state->value3ED = enabled;
        if (std::bit_cast<std::int8_t>(*mode) > 0) {
            if (enabled == 1) {
                WindowElement* const element = state->elements00[1];
                const WindowElementCallback callback = state->callbacks50[2];
                element->mouseCallback = callback;
            } else {
                state->elements00[1]->mouseCallback = WindowElement_NoOp;
            }
            if (!world::IsValidPetIndex_51C410(state->selectedPet70)) {
                return false;
            }
            const std::int32_t selected = state->selectedPet70;
            std::int32_t position = 0;
            std::int32_t itemSlots = world::gPetRuntimeRecords[selected].values6D8[0];
            while (position < itemSlots) {
                const std::uint8_t visible = state->value3ED == 0;
                state->elements00[9 + position]->visible = visible;
                const std::int32_t nextSelected = state->selectedPet70;
                ++position;
                itemSlots = world::gPetRuntimeRecords[nextSelected].values6D8[0];
            }
        }
        consumed = true;
    }
    state->imageIndex3F4 = (events & 8) != 0 ? 2 : ((events & 1) != 0 ? 3 : 1);
    return consumed;
}

// SEV01：低U16两个方向门槛独立，先正向S32余8再反向减1/原加8；两声效/原顺序保，父固定返回0。
// 原函数：0x004C7A10。
bool WindowItemSelector_RotatePreview(WindowItemSelectorState* const state, const std::int32_t events)
{
    bool consumed = false;
    if ((events & 0x402) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        const std::int32_t direction = cg::yobi::GetYobiActionField2B8(state->preview);
        cg::yobi::SetYobiActionField2B8(state->preview, (direction + 1) % 8);
        consumed = true;
    }
    if ((events & 0x804) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        const std::int32_t direction = cg::yobi::GetYobiActionField2B8(state->preview) - 1;
        cg::yobi::SetYobiActionField2B8(state->preview, direction);
        if (direction < 0) {
            cg::yobi::SetYobiActionField2B8(state->preview, direction + 8);
        }
        consumed = true;
    }
    return consumed;
}

// SFD01：首次借mode地址、previous=-1、切页后置initialized；尾逐帧分发，上行两窗口回调固定返回1。
// 原函数：0x004C7A90。
void WindowItemSelector_Update(WindowItemSelectorState* const state)
{
    if (state->initialized3E8 == 0) {
        const std::uint8_t* const mode = state->mode68;
        state->previousPet74 = -1;
        WindowItemSelector_SelectPage(state, *mode == 0xFFu ? 0 : 1);
        state->initialized3E8 = 1;
    }
    WindowItemSelector_UpdatePage(state);
}

// ISP01：原local负值返-1，成功后取当前窗口，再按元素X/窗口X取样累加。
// 原函数：0x004C7240。
std::int32_t WindowItemSelector_ReadScreenX(
    const WindowItemSelectorState* const state, const std::int32_t element,
    const std::int32_t slot, const std::int32_t offset)
{
    const std::int32_t local = WindowItemSelector_ReadSlotX(state, slot);
    if (local < 0) {
        return -1;
    }
    const WindowInstance* const window = gCurrentWindow;
    const std::int32_t elementX = window->elements[element].x;
    const std::int32_t windowX = window->x;
    return elementX + windowX + local + offset;
}

// ISP01：原local负值返-1，成功后取当前窗口，再按元素Y/窗口Y取样累加。
// 原函数：0x004C7280。
std::int32_t WindowItemSelector_ReadScreenY(
    const WindowItemSelectorState* const state, const std::int32_t element,
    const std::int32_t slot, const std::int32_t offset)
{
    const std::int32_t local = WindowItemSelector_ReadSlotY(state, slot);
    if (local < 0) {
        return -1;
    }
    const WindowInstance* const window = gCurrentWindow;
    const std::int32_t elementY = window->elements[element].y;
    const std::int32_t windowY = window->y;
    return elementY + windowY + local + offset;
}

// ISP01：S8模式0借玩家前8槽，正模式经原宠物校验后两次重读索引借列表/数量；失败不改输出。
// 原函数：0x004C72C0。
bool WindowItemSelector_GetItemList_4C72C0(
    const WindowItemSelectorState* const state, world::PlayerItemRecord** const items,
    std::int32_t* const count)
{
    const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
    if (mode == 0) {
        *items = world::gPlayerRuntimeSlots.items.data();
        *count = 8;
        return true;
    }
    if (mode > 0 && world::IsValidPetIndex_51C410(state->selectedPet70)) {
        *items = world::gPetRuntimeRecords[state->selectedPet70].items6E4.data();
        *count = world::gPetRuntimeRecords[state->selectedPet70].values6D8[0];
        return true;
    }
    return false;
}

// SAP01：原391条三根有据；本根完整239条，图形与三种文字坐标按原读取序；格式/GBK与低AL边界保持，运行待。
// 原函数：0x004C75A0。
bool WindowItemSelector_HandlePlayerAttributes(WindowItemSelectorState* const selector, const std::int32_t state)
{
    if ((static_cast<std::uint8_t>(state) & 2u) != 0 || state == 0
        || selector->value3FC == 0 || world::gReplyRecordF7C950.used == 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        WindowElement* const element = selector->elements00[1];
        const WindowElementCallback callback = selector->callbacks50[2];
        selector->value3FC = 0;
        element->mouseCallback = callback;
        return true;
    }
    if (selector->value60 != 2) {
        return false;
    }
    // 原580900二十四项表；标签采用UTF-16，数值索引不是地址。
    struct AttributeDisplay {
        std::int32_t index;
        const wchar_t* label;
        std::uint8_t percent;
        std::int32_t color;
    };
    static constexpr AttributeDisplay attributes[] {
        {10, L"生命", 1, 4},
        {11, L"生命", 0, 4},
        {12, L"魔力", 1, 4},
        {13, L"魔力", 0, 4},
        {0, L"攻击", 1, 1},
        {1, L"攻击", 0, 1},
        {2, L"防御", 1, 1},
        {3, L"防御", 0, 1},
        {4, L"敏捷", 1, 1},
        {5, L"敏捷", 0, 1},
        {6, L"精神", 1, 1},
        {7, L"精神", 0, 1},
        {8, L"回复", 1, 1},
        {9, L"回复", 0, 1},
        {14, L"抗毒", 0, 2},
        {15, L"抗睡眠", 0, 2},
        {16, L"抗石化", 0, 2},
        {17, L"抗酒醉", 0, 2},
        {18, L"抗混乱", 0, 2},
        {19, L"抗遗忘", 0, 2},
        {20, L"必杀", 0, 8},
        {21, L"反击", 0, 8},
        {22, L"命中", 0, 8},
        {23, L"回避", 0, 8},
    };
    const WindowElement* const element = selector->elements00[1];
    const render::GraphicDescriptor16* const descriptor = &selector->descriptors3B8[0];
    const std::int32_t elementY = element->y;
    const std::int32_t elementX = element->x;
    const WindowInstance* const window = gCurrentWindow;
    const std::int32_t windowY = window->y;
    const std::int32_t windowX = window->x;
    static_cast<void>(cg::render::DrawGraphic(
        elementX + windowX + 121, elementY + windowY + 118,
        223, 98244, 0, descriptor));
    // 4C7643/4C7696/4C7777三种坐标取样序，仅用于本属性页绘制。
    enum class TextCoordinateOrder {
        Title,
        WindowBeforeElements,
        WindowAfterElements,
    };
    const auto drawText = [selector](
        const std::int32_t offsetX, const std::int32_t offsetY,
        const std::int32_t textColor, const std::wstring_view text,
        const TextCoordinateOrder order) {
        const WindowElement* const textElement = selector->elements00[1];
        std::int32_t x;
        std::int32_t y;
        if (order == TextCoordinateOrder::Title) {
            const WindowInstance* const textWindow = gCurrentWindow;
            const std::int32_t elementY = textElement->y;
            const std::int32_t windowY = textWindow->y;
            y = elementY + windowY + offsetY;
            const std::int32_t elementX = textElement->x;
            const std::int32_t windowX = textWindow->x;
            x = elementX + windowX + offsetX;
        } else if (order == TextCoordinateOrder::WindowBeforeElements) {
            const WindowInstance* const textWindow = gCurrentWindow;
            const std::int32_t elementY = textElement->y;
            const std::int32_t elementX = textElement->x;
            const std::int32_t windowY = textWindow->y;
            const std::int32_t windowX = textWindow->x;
            y = elementY + windowY + offsetY;
            x = elementX + windowX + offsetX;
        } else {
            const std::int32_t elementY = textElement->y;
            const std::int32_t elementX = textElement->x;
            const WindowInstance* const textWindow = gCurrentWindow;
            const std::int32_t windowY = textWindow->y;
            const std::int32_t windowX = textWindow->x;
            y = elementY + windowY + offsetY;
            x = elementX + windowX + offsetX;
        }
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)), 5, 12,
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(textColor)), text, 0, 0));
    };
    const std::int32_t length =
        static_cast<std::int32_t>(std::strlen(world::gReplyRecordF7C950.text04.data()));
    if (length > 0) {
        drawText(100 - (12 * length) / 2 / 2, 9, 0,
            cg::text::FromGbkWide(world::gReplyRecordF7C950.text04.data()), TextCoordinateOrder::Title);
    }
    if (world::gReplyRecordF7C950.value48 == 1) {
        drawText(64, 28, 0, L"(有特殊称号)",
            TextCoordinateOrder::WindowBeforeElements);
    }
    std::int32_t lines = 0;
    std::int32_t x = 28;
    std::int32_t y = 58;
    std::wstring buffer;
    for (const AttributeDisplay& attribute : attributes) {
        if (lines >= 14) {
            break;
        }
        const std::int32_t value = world::gReplyRecordF7C950.values4C[attribute.index];
        if (value == 0) {
            continue;
        }
        if (lines == 7) {
            x = 116;
            y = 58;
        }
        const wchar_t* const format = attribute.percent != 0
            ? (value < 0 ? L"{}-{:4}%" : L"{}+{:4}%")
            : (value < 0 ? L"{}-{:4}" : L"{}+{:4}");
        cg::text::FormatTo(buffer, format, attribute.label, value < 0 ? -value : value);
        drawText(x, y, attribute.color, buffer, TextCoordinateOrder::WindowAfterElements);
        y += 16;
        ++lines;
    }
    if (world::gReplyRecordF7C950.hasText24 != 0) {
        cg::text::FormatTo(buffer, LR"(能减少使用"{}"的魔)", cg::text::FromGbkWide(world::gReplyRecordF7C950.text25.data()).c_str());
        drawText(28, 188, 0, buffer, TextCoordinateOrder::WindowBeforeElements);
        drawText(28, 204, 0, L"力消耗",
            TextCoordinateOrder::WindowBeforeElements);
    }
    return false;
}

// 原函数：0x0048B1A0。
std::int32_t WindowItem_DrawEquipmentTooltip(const std::int32_t index, const std::int32_t page)
{
    if (gTooltipSourceD303FC != index) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX));
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY));
        const bool left =
            (platform::gLegacyMouseState.mouseX < 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            (platform::gLegacyMouseState.mouseY < 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    const std::int8_t mode = std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28);
    gTooltipSourceD303FC = index;
    const bool petEquipment = mode > 0 && index < 6;
    const world::PlayerItemRecord* record;
    if (petEquipment) {
        const std::int32_t petIndex = WindowItemSelector_ReadSelectedPet(&gItemSelectorCF3B10);
        const std::int32_t itemIndex = gTooltipSourceD303FC;
        record = &world::gPetRuntimeRecords[petIndex].items6E4[itemIndex];
    } else {
        record = &world::gPlayerRuntimeSlots.items[index];
    }
    const std::uint16_t flags = record->flags;
    const std::wstring title = (flags & 0x100u) != 0 ? record->memo : record->name;
    const std::int8_t titleColor = (flags & 0x100u) != 0
        ? 5 : static_cast<std::int8_t>((flags & 0x200u) >> 9);
    const std::wstring* const lines = record->description.data() + page * 8;
    if (petEquipment) {
        const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
        const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
        return WindowItem_DrawTooltip(x, y, title.c_str(), titleColor, lines, 8, nullptr, 0);
    }
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    return WindowItem_DrawTooltip(x, y, title.c_str(), titleColor, lines, 8, nullptr, 0);
}

// SPD01：原48B350完整65条；W32标题字段、四次鼠标CryptInt原序和八行描述/真实返回保持。
// 原函数：0x0048B350。
std::int32_t WindowItem_DrawDescriptionTooltip(const std::int32_t index, const std::int32_t page)
{
    const std::int32_t key = index + 100;
    if (gTooltipSourceD303FC != key) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX));
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY));
        const bool left =
            (platform::gLegacyMouseState.mouseX < 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            (platform::gLegacyMouseState.mouseY < 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    gTooltipSourceD303FC = key;
    const auto* const record = &gItemDetailStorageCC4D58[0].items[index];
    std::uint32_t flags;
    std::memcpy(&flags, &record->flags, sizeof(flags));
    const std::wstring title = (flags & 0x100u) != 0 ? record->memo : record->name;
    const std::int8_t titleColor = (flags & 0x100u) != 0
        ? 5 : static_cast<std::int8_t>((flags >> 9) & 1u);
    const auto* const lines = record->description.data() + page * 8;
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    return WindowItem_DrawTooltip(x, y, title.c_str(), titleColor, lines, 8, nullptr, 0);
}

// 原函数：0x0048B750。
std::int32_t WindowItem_DrawWorkTooltip(const std::int32_t page)
{
    if (gTooltipSourceD303FC != 400) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX));
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY));
        const bool left =
            (platform::gLegacyMouseState.mouseX < 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            (platform::gLegacyMouseState.mouseY < 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    gTooltipSourceD303FC = 400;
    const auto& title = gShopDetailCC3210.name;
    return WindowItem_DrawTooltip(
        x, y, title.c_str(), 0, gShopDetailCC3210.description.data() + page * 8,
        8, nullptr, 0);
}

// 原函数：0x00503CB0。
std::int32_t WindowRuntime_CycleNameMode()
{
    const std::int32_t decreased = platform::gChatTextSizeMode_73ECBC - 1;
    const bool belowFirstMode = decreased < 0;
    platform::gChatTextSizeMode_73ECBC = decreased;
    if (belowFirstMode) {
        platform::gChatTextSizeMode_73ECBC = 2;
    }
    return decreased;
}

// 原函数：0x00503CD0。
static std::int32_t WindowRuntime_IncrementNameMode()
{
    const auto mode = static_cast<std::int16_t>(platform::gActorOverheadTextMode_5F974C + 1);
    const bool pastLastMode = mode >= 7;
    platform::gActorOverheadTextMode_5F974C = mode;
    if (pastLastMode) {
        platform::gActorOverheadTextMode_5F974C = 0;
    }
    static_cast<void>(login::detail::CaptureClientPreferenceBytes_520590());
    return login::detail::SaveCurrentLoginRecord_520200();
}

// CWP01：原71项默认坐标/设置与返回保持，地址索引仅作原生寻址。
// 原函数：0x004C9240。
static std::int32_t WindowRuntime_ResetSettings()
{
    for (std::size_t index = 0; index < kWindowCount; ++index) {
        const WindowPrototype* const prototype = gWindowPrototypes57C130[index];
        gWindowStates[index].x = prototype->defaultX;
        gWindowStates[index].y = prototype->defaultY;
    }
    gHotkeyLayoutMode_CF4AAC = 1;
    gInventoryActorSelectionMode_57DA28 = 0xFF;
    gBattleItemActorSelectionMode_57E42C = 0xFF;
    gChatWidthUnitsC9A930 = 12;
    gChatHeightUnitsC9A938 = 4;
    gStatusBarLayoutMode_CF4690 = 0;
    gShortcutBarCollapsed_CA0440 = 0;
    gWindowStates[WindowId_MiniMap].marker50 = 0;
    gWindowStates[WindowId_Emote].marker50 = 0;
    gContactsPreviewMode_D67B38 = 0;
    return 0;
}

// 原函数：0x004C93A0。
static std::int32_t WindowRuntime_HasChangedDefaultPositions()
{
    if (gWindowSavedDataD40B30.records[1].x != gWindowPrototype54E2A0.defaultX) {
        return 1;
    }
    if (gWindowSavedDataD40B30.records[4].x != gWindowPrototype54E240.defaultX) {
        return 1;
    }
    return gWindowSavedDataD40B30.records[6].x != gWindowPrototype54E330.defaultX ? 1 : 0;
}

// 原函数：0x004C93E0。
static std::int32_t WindowRuntime_RepairSettingsTail()
{
    const std::int32_t fileLength = gWindowSavedLengthCC4238;
    if (fileLength == 0x521C) {
        return 1;
    }
    const std::int32_t missingBytes = 0x521C - fileLength;
    const std::uint32_t first =
        (static_cast<std::uint32_t>(fileLength)
            - static_cast<std::uint32_t>(gWindowSavedDataD40B30.version)) / 0x128u;
    const std::uint32_t count = static_cast<std::uint32_t>(missingBytes) / 0x128u;
    if (missingBytes < 0 || static_cast<std::uint32_t>(missingBytes) % 0x128u != 0) {
        return 0;
    }
    const std::uint32_t end = first + count;
    if (std::bit_cast<std::int32_t>(first) < std::bit_cast<std::int32_t>(end)) {
        for (std::uint32_t offset = 0; offset < count; ++offset) {
            const std::uint32_t index = first + offset + 1u;
            const WindowPrototype* const prototype = gWindowPrototypes57C130[index];
            gWindowStates[index].x = prototype->defaultX;
            gWindowStates[index].y = prototype->defaultY;
        }
    }
    return 1;
}

// CWP01：原两设置BYTE现场读取，完整S32字段/交错发布与返回保持。
// 原函数：0x004C9170。
static std::int32_t WindowRuntime_ApplySettings()
{
    for (std::size_t index = 0; index < kWindowCount; ++index) {
        // R02：旧分辨率的保存坐标不覆盖当前布局；下方非坐标设置仍按原流程恢复。
        const WindowPrototype* const prototype = gWindowPrototypes57C130[index];
        gWindowStates[index].x = prototype->defaultX;
        gWindowStates[index].y = prototype->defaultY;
    }
    static_cast<void>(WindowRuntime_RepairSettingsTail());
    const auto firstByte = *reinterpret_cast<const unsigned char*>(&gWindowSavedDataD40B30.records[14].value08);
    const std::int32_t valueCF4AAC = gWindowSavedDataD40B30.records[24].value08;
    const auto secondByte = *reinterpret_cast<const unsigned char*>(&gWindowSavedDataD40B30.records[14].value0C);
    gInventoryActorSelectionMode_57DA28 = firstByte;
    const std::int32_t width = gWindowSavedDataD40B30.records[35].value08;
    gHotkeyLayoutMode_CF4AAC = valueCF4AAC;
    const std::int32_t mode = gWindowSavedDataD40B30.records[35].value10;
    gChatWidthUnitsC9A930 = width;
    const std::int32_t valueCF4690 = gWindowSavedDataD40B30.records[1].value08;
    gBattleItemActorSelectionMode_57E42C = secondByte;
    const std::int32_t height = gWindowSavedDataD40B30.records[35].value0C;
    gChatWindowModeD66E04 = mode;
    const std::int32_t actorBars = gWindowSavedDataD40B30.records[1].value0C;
    gStatusBarLayoutMode_CF4690 = valueCF4690;
    gChatHeightUnitsC9A938 = height;
    const std::int32_t valueCA0440 = gWindowSavedDataD40B30.records[4].value08;
    world::gActorBarsEnabled5AC490 = actorBars == 1 ? 1u : 0u;
    const std::int32_t marker33 = gWindowSavedDataD40B30.records[33].value08;
    gShortcutBarCollapsed_CA0440 = valueCA0440;
    const std::int32_t marker34 = gWindowSavedDataD40B30.records[34].value08;
    gWindowStates[WindowId_MiniMap].marker50 = static_cast<std::uint32_t>(marker33);
    const std::int32_t valueD67B38 = gWindowSavedDataD40B30.records[21].value08;
    gWindowStates[WindowId_Emote].marker50 = static_cast<std::uint32_t>(marker34);
    gContactsPreviewMode_D67B38 = valueD67B38;
    return 1;
}

// 原函数：0x004C9060。
static std::int32_t WindowRuntime_LoadSettings()
{
    static_cast<void>(WindowRuntime_ResetSettings());
    std::FILE* const stream =
        std::fopen("data\\WinSaveDate", "rb+");
    if (stream == nullptr) {
        static_cast<void>(WindowRuntime_ResetSettings());
        return 0;
    }
    // 原0x0053D435：同一MSVC描述符的创建／读写及权限。
    const std::int32_t descriptor = ::_open(
        "data\\WinSaveDate", _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE);
    if (descriptor != -1) {
        // 原0x0053D3BD：保留S32文件长度。
        gWindowSavedLengthCC4238 = static_cast<std::int32_t>(::_filelength(descriptor));
        // 原0x0053D30A：关闭上述描述符，原返回值不消费。
        static_cast<void>(::_close(descriptor));
        // 原4C90B8/4C90D4均读取flags低U8，只有20H是错误。
        if (std::ferror(stream) == 0) {
            static_cast<void>(static_cast<std::uint32_t>(std::fread(
                &gWindowSavedDataD40B30, 1, 0x521C, stream)));
            if (std::ferror(stream) == 0) {
                if (gWindowSavedDataD40B30.version != 0x13
                    || WindowRuntime_HasChangedDefaultPositions() == 1 || WindowRuntime_ApplySettings() == 0) {
                    static_cast<void>(WindowRuntime_ResetSettings());
                }
                static_cast<void>(std::fclose(stream));
                return 1;
            }
        }
    }
    static_cast<void>(WindowRuntime_ResetSettings());
    static_cast<void>(std::fclose(stream));
    return 0;
}

// 原函数：0x004C92B0。
static std::int32_t WindowRuntime_ClearSavedSettings()
{
    std::memset(&gWindowSavedDataD40B30, 0, sizeof(gWindowSavedDataD40B30));
    return 0;
}

// 原函数：0x004C92D0。
static std::uint32_t WindowRuntime_CaptureSettings()
{
    static_cast<void>(WindowRuntime_ClearSavedSettings());
    gWindowSavedDataD40B30.version = 0x13;
    for (std::size_t index = 0; index < kSavedWindowCount; ++index) {
        gWindowSavedDataD40B30.records[index].x = gWindowStates[index].x;
        gWindowSavedDataD40B30.records[index].y = gWindowStates[index].y;
    }
    const std::int32_t firstByte = std::bit_cast<std::int8_t>(gInventoryActorSelectionMode_57DA28);
    const std::int32_t secondByte = std::bit_cast<std::int8_t>(gBattleItemActorSelectionMode_57E42C);
    const std::int32_t valueCF4AAC = gHotkeyLayoutMode_CF4AAC;
    gWindowSavedDataD40B30.records[14].value08 = firstByte;
    const std::int32_t width = gChatWidthUnitsC9A930;
    gWindowSavedDataD40B30.records[14].value0C = secondByte;
    const std::int32_t height = gChatHeightUnitsC9A938;
    gWindowSavedDataD40B30.records[35].value08 = width;
    const std::int32_t valueCF4690 = gStatusBarLayoutMode_CF4690;
    gWindowSavedDataD40B30.records[24].value08 = valueCF4AAC;
    const std::int32_t mode = gChatWindowModeD66E04;
    gWindowSavedDataD40B30.records[1].value08 = valueCF4690;
    const std::uint32_t actorBars = world::gActorBarsEnabled5AC490;
    gWindowSavedDataD40B30.records[35].value0C = height;
    const std::uint32_t marker33 = gWindowStates[WindowId_MiniMap].marker50;
    gWindowSavedDataD40B30.records[35].value10 = mode;
    const std::int32_t valueCA0440 = gShortcutBarCollapsed_CA0440;
    gWindowSavedDataD40B30.records[33].value08 = std::bit_cast<std::int32_t>(marker33);
    gWindowSavedDataD40B30.records[1].value0C = actorBars == 1 ? 1 : 0;
    const std::uint32_t marker34 = gWindowStates[WindowId_Emote].marker50;
    gWindowSavedDataD40B30.records[4].value08 = valueCA0440;
    const std::int32_t valueD67B38 = gContactsPreviewMode_D67B38;
    gWindowSavedDataD40B30.records[34].value08 = std::bit_cast<std::int32_t>(marker34);
    gWindowSavedDataD40B30.records[21].value08 = valueD67B38;
    return marker33;
}

// 原函数：0x004C9130。
std::int32_t WindowRuntime_SaveSettings()
{
    std::FILE* const stream =
        std::fopen("data\\WinSaveDate", "wb+");
    if (stream != nullptr) {
        static_cast<void>(WindowRuntime_CaptureSettings());
        static_cast<void>(static_cast<std::uint32_t>(std::fwrite(
            &gWindowSavedDataD40B30, 1, 0x521C, stream)));
        static_cast<void>(std::fclose(stream));
    }
    return 1;
}

// 原函数：0x00472200。
static void WindowRuntime_ClearSavedWindowMarkers()
{
    for (WindowState& state : gWindowStates) {
        state.marker50 = 0;
    }
}

// 原函数：0x00472220。
static void WindowRuntime_MarkExistingWindows()
{
    for (WindowState& state : gWindowStates) {
        if (state.window != nullptr) {
            state.marker50 = 1;
        }
    }
}

// 原函数：0x004722A0。
static void WindowRuntime_ResetMouseAndItemFlags()
{
    world::gInventorySlotUseCounts_D67B08.fill(0);
    gMouseValue57C110 = 0;
    gDragSourceIndex57C114 = 0;
    gDragCallback57C118 = nullptr;
    gDragY57C11E = 0;
    gDragX57C11C = 0;
    gFocusWindowId57C120 = 0;
    gFocusElementIndex57C124 = -1;
    gMousePhase57C128 = 0;
}

// 原函数：0x004722E0。
static void WindowRuntime_ClearElementFocus()
{
    gFocusedElementDataD45F94 = nullptr;
}

// 原函数：0x004722F0。
static void WindowRuntime_RestoreMarkedWindows()
{
    for (std::int32_t index = 0; index < static_cast<std::int32_t>(kWindowCount); ++index) {
        if (gWindowStates[index].marker50 != 1) {
            continue;
        }
        switch (index) {
        case 21:
        case 22:
        case 23:
        case 40:
            if (gWindowStates[WindowId_Contacts].marker50 != 0) {
                static_cast<void>(WindowRuntime_Open(WindowId_Contacts));
                gWindowStates[WindowId_Contacts].marker50 = 0;
            }
            if (gWindowStates[WindowId_MailRead].marker50 != 0) {
                const std::int32_t position = world::detail::gViewedMailMessageIndex_D046D4;
                static_cast<void>(WindowRuntime_Open(WindowId_MailRead));
                world::detail::gViewedMailMessageIndex_D046D4 = position;
                gWindowStates[WindowId_MailRead].marker50 = 0;
            }
            if (gWindowStates[WindowId_MailCompose].marker50 != 0) {
                static_cast<void>(WindowMailCompose_Open());
                gWindowStates[WindowId_MailCompose].marker50 = 0;
            }
            if (gWindowStates[WindowId_PetMailCompose].marker50 != 0) {
                static_cast<void>(WindowRuntime_Open(WindowId_PetMailCompose));
                gWindowStates[WindowId_PetMailCompose].marker50 = 0;
            }
            break;
        case 33:
        case 34:
            static_cast<void>(WindowRuntime_Open(index));
            gWindowStates[index].marker50 = 0;
            break;
        default:
            gWindowStates[index].marker50 = 0;
            break;
        }
    }
}

// 原函数：0x00479640。
static void WindowRuntime_ResetOperationCount()
{
    gWindowOperationCount_D67AF8 = 0;
}

// 原函数：0x004796C0。
static void WindowRuntime_ResetGatherOperationCount()
{
    gGatherOperationCount_D67AFC = 0;
}

// 原函数：0x004796F0。
static void WindowRuntime_ResetSkillOperationCount()
{
    gSkillOperationCount_D67AFD = 0;
}

// 原函数：0x00479610。
static void WindowRuntime_ResetOperationReferences()
{
    gWindowActionCA0174 = nullptr;
    gWindowActionD2E928 = nullptr;
    WindowRuntime_ResetOperationCount();
    WindowRuntime_ResetGatherOperationCount();
    WindowRuntime_ResetSkillOperationCount();
    gSkillStatusIconReferences_D67AFE = 0;
}

// 原函数：0x004C7AD0。
static void WindowRuntime_ResetShortcutFlag()
{
    gHotkeyLayoutMode_CF4AAC = 0;
}

// 原函数：0x00475C10。
static void WindowRuntime_RestorePositions()
{
    for (std::size_t index = 0; index < kWindowCount; ++index) {
        WindowState& state = gWindowStates[index];
        WindowInstance* window = state.window;
        if (window == nullptr) {
            continue;
        }
        const WindowPrototype* const prototype = gWindowPrototypes57C130[index];
        const bool isPrimaryOffsetWindow = index == 10;
        window->x = prototype->defaultX;
        window = state.window;
        window->y = prototype->defaultY;
        if (!isPrimaryOffsetWindow && index != 11 && index != 12 && index != 14 && index != 27) {
            if (index == WindowId_Chat) {
                window->x = platform::gWindowWidth - (gChatWidthUnitsC9A930 * 12 + 123) - 9;
            }
            static_cast<void>(CenterPopupWindowPosition(static_cast<std::int32_t>(index),
                window->width, window->height, window->x, window->y));
            continue;
        }

        // 两次偏移固定各自调用前实例，越界修正则写回调用后重取的实例。
        window = state.window;
        // const std::int32_t offsetX = platform::RandomRange_46FAF0(-15, 15);
        // window->x += offsetX;
        // window = state.window;
        // const std::int32_t offsetY = platform::RandomRange_46FAF0(-15, 15);
        // window->y += offsetY;
        // window = state.window;
        if (window->x < 0) {
            const std::int32_t x = platform::RandomRange_46FAF0(10, 20);
            state.window->x = x;
        }
        window = state.window;
        if (window->y < 0) {
            const std::int32_t y = platform::RandomRange_46FAF0(10, 20);
            state.window->y = y;
        }
        window = state.window;
        const std::int32_t width = prototype->width;
        if (window->x + width > platform::gWindowWidth) {
            const std::int32_t x = platform::RandomRange_46FAF0(platform::gWindowWidth - 20 - width, platform::gWindowWidth - width);
            state.window->x = x;
        }
        window = state.window;
        const std::int32_t height = prototype->height;
        if (window->y + height > platform::gWindowHeight) {
            const std::int32_t y = platform::RandomRange_46FAF0(platform::gWindowHeight - 20 - height, platform::gWindowHeight - height);
            state.window->y = y;
        }
        // 保留上方随机数消费；仅原型中心符合±50规则的窗口按实例尺寸居中。
        window = state.window;
        static_cast<void>(CenterPopupWindowPosition(static_cast<std::int32_t>(index),
            window->width, window->height, window->x, window->y));
    }
}

static void WindowRuntime_ToggleShortcutWindow(const std::int32_t id)
{
    WindowInstance* const window = gWindowStates[id].window;
    if (window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(id, 1, 0, nullptr));
    } else {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
}

static void WindowRuntime_PlayShortcutEmote(const std::int32_t action, const std::int32_t selected)
{
    world::RequestPlayerEmote_4FA8E0(action);
    static_cast<void>(WindowEmote_Select(selected));
    static_cast<void>(audio::PlaySe_52C450(51, 320));
}

// 原函数：0x004C7AE0。
static void WindowRuntime_ProcessShortcuts()
{
    const std::int32_t mode = gHotkeyLayoutMode_CF4AAC;
    if (mode != 0 && mode != 1) {
        return;
    }
    const bool modeOne = mode == 1;
    if ((platform::gKeyStates[0x23] & 4u) != 0) {
        WindowRuntime_ToggleShortcutWindow(65);
    }
    if ((platform::gKeyStates[0x78] & 4u) != 0) {
        WindowRuntime_ToggleShortcutWindow(66);
    }
    if ((platform::gKeyStates[0x11] & 2u) != 0) {
        if ((platform::gKeyStates[modeOne ? 0x51 : 0x57] & 4u) != 0) {
            constexpr std::int32_t ids[] {7, 8, 9, 58};
            bool found = false;
            for (const std::int32_t id : ids) {
                WindowInstance* const window = gWindowStates[id].window;
                if (window != nullptr) {
                    found = true;
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
            }
            if (!found) {
                static_cast<void>(WindowRuntime_CreateAnimation(WindowId_PlayerProfile, 1, 0, nullptr));
            }
        }
        if ((platform::gKeyStates[modeOne ? 0x57 : 0x45] & 4u) != 0) {
            if (world::gCurrentSceneType == 10) {
                WindowBattleCommand_OpenSkills();
            } else {
                WindowInstance* const window = gWindowStates[WindowId_Skill].window;
                if (window == nullptr) {
                    static_cast<void>(WindowSkill_Open());
                } else {
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
            }
        }
        if ((platform::gKeyStates[modeOne ? 0x45 : 0x52] & 4u) != 0) {
            if (world::gCurrentSceneType == 10) {
                WindowBattleCommand_OpenItems();
            } else {
                WindowRuntime_ToggleShortcutWindow(14);
            }
        }
        if ((platform::gKeyStates[modeOne ? 0x52 : 0x41] & 4u) != 0) {
            if (world::gCurrentSceneType == 10) {
                WindowBattleCommand_OpenPets();
            } else {
                WindowRuntime_ToggleShortcutWindow(15);
            }
        }
        if ((platform::gKeyStates[modeOne ? 0x54 : 0x53] & 4u) != 0) {
            WindowRuntime_ToggleShortcutWindow(21);
        }
        if ((platform::gKeyStates[modeOne ? 0x59 : 0x44] & 4u) != 0) {
            WindowRuntime_ToggleShortcutWindow(19);
        }
        if ((platform::gKeyStates[modeOne ? 0x41 : 0x46] & 4u) != 0
            && world::gCurrentSceneType != 10) {
            WindowRuntime_ToggleShortcutWindow(34);
        }
        if ((platform::gKeyStates[modeOne ? 0x53 : 0x51] & 4u) != 0
            && world::gCurrentSceneType != 10) {
            WindowRuntime_ToggleShortcutWindow(33);
        }
        if ((platform::gKeyStates[modeOne ? 0x44 : 0x59] & 4u) != 0) {
            WindowRuntime_ToggleShortcutWindow(25);
        }
        if ((platform::gKeyStates[modeOne ? 0x5A : 0x4F] & 4u) != 0) {
            static_cast<void>(WindowRuntime_CycleNameMode());
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[0x26] & 4u) != 0) {
            WindowChat_RequestHeightIncrease();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[0x28] & 4u) != 0) {
            WindowChat_RequestHeightDecrease();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[0x27] & 4u) != 0) {
            WindowChat_RequestWidthDecrease();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[0x25] & 4u) != 0) {
            WindowChat_RequestWidthIncrease();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[modeOne ? 0x55 : 0x50] & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            static_cast<void>(world::ToggleDuelPermission_472F40());
        }
        if ((platform::gKeyStates[modeOne ? 0x49 : 0xBA] & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            static_cast<void>(world::TogglePartyChat_472F70());
        }
        if ((platform::gKeyStates[modeOne ? 0x4F : 0xBE] & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            static_cast<void>(world::TogglePartyJoinPermission_472FA0());
        }
        if ((platform::gKeyStates[modeOne ? 0x50 : 0x4C] & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            static_cast<void>(world::ToggleContactCardExchangePermission_472FD0());
        }
        if ((platform::gKeyStates[modeOne ? 0x46 : 0xBC] & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            static_cast<void>(world::ToggleTradePermission_473000());
        }
        if ((platform::gKeyStates[modeOne ? 0xDB : 0x4E] & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            static_cast<void>(world::ToggleGuildInvitePermission_473030());
        }

        // 该门禁只采一次场景，覆盖全部请求和动作；门内回调不触发再次判场景。
        if (world::gCurrentSceneType != 10) {
            if ((platform::gKeyStates[modeOne ? 0x4A : 0x4B] & 4u) != 0) {
                world::RequestDuelWithFacingPlayer_470050();
            }
            if ((platform::gKeyStates[modeOne ? 0x4B : 0x56] & 4u) != 0) {
                world::RequestSpectateFacingBattle_470150();
            }
            if (!modeOne && (platform::gKeyStates[0x48] & 4u) != 0) {
                WindowRuntime_PlayShortcutEmote(9, 13);
            }
            if ((platform::gKeyStates[modeOne ? 0x4C : 0xBF] & 4u) != 0) {
                world::RequestPartyJoinOrLeave_470270();
            }
            if ((platform::gKeyStates[modeOne ? 0xBA : 0xDB] & 4u) != 0) {
                world::RequestFacingPlayerContactCard_4703C0();
            }
            if ((platform::gKeyStates[modeOne ? 0xDE : 0xDD] & 4u) != 0) {
                world::RequestNearbyPlayerTrade_4704A0();
            }
            if ((platform::gKeyStates[modeOne ? 0xDD : 0x4D] & 4u) != 0) {
                world::RequestFacingPlayerGuildInvitation_470670();
            }
            for (const ShortcutEmoteBinding& binding : kShortcutEmotes) {
                const std::uint8_t key = modeOne ? binding.keyMode1 : binding.keyMode0;
                if ((platform::gKeyStates[key] & 4u) != 0) {
                    WindowRuntime_PlayShortcutEmote(binding.action, binding.selected);
                }
            }
            if ((platform::gKeyStates[0xDC] & 4u) != 0) {
                const std::int32_t remainder = platform::Rand16() % 3;
                if (remainder == 0) {
                    WindowRuntime_PlayShortcutEmote(14, 7);
                } else if (remainder == 1) {
                    WindowRuntime_PlayShortcutEmote(16, 8);
                } else {
                    WindowRuntime_PlayShortcutEmote(17, 17);
                }
            }
        }
        if ((platform::gKeyStates[0x7B] & 4u) != 0) {
            WindowRuntime_RestorePositions();
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[0x7A] & 4u) != 0) {
            world::gActorBarsEnabled5AC490 =
                world::gActorBarsEnabled5AC490 != 1 ? 1u : 0u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[0x24] & 4u) != 0) {
            static_cast<void>(WindowChat_SwitchMode());
        }
        return;
    }
    if ((platform::gKeyStates[0x10] & 2u) != 0) {
        if ((platform::gKeyStates[0x7B] & 4u) != 0) {
            WindowRuntime_CloseByCategory(7);
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
        if ((platform::gKeyStates[0x1B] & 4u) != 0) {
            WindowRuntime_CloseByCategory(7);
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
        if ((platform::gKeyStates[0x24] & 4u) != 0) {
            if (input::gChatPanelVisible_C9A96C == 1) {
                input::gChatPanelVisible_C9A96C = 0;
                input::gChatLogSuppressed_626C14 = 1;
            } else {
                input::gChatPanelVisible_C9A96C = 1;
                input::gChatLogSuppressed_626C14 = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[0x27] & 4u) != 0) {
            const std::int32_t range = platform::gChatRange_554640 + 1;
            const bool pastLastRange = range > 5;
            platform::gChatRange_554640 = range;
            if (pastLastRange) {
                platform::gChatRange_554640 = 1;
            }
            world::gChatRangeOverlayFrames_626BFC = 180;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[0x25] & 4u) != 0) {
            const std::int32_t range = platform::gChatRange_554640 - 1;
            const bool belowFirstRange = range < 1;
            platform::gChatRange_554640 = range;
            if (belowFirstRange) {
                platform::gChatRange_554640 = 5;
            }
            world::gChatRangeOverlayFrames_626BFC = 180;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((platform::gKeyStates[0x26] & 0x10u) != 0) {
            const std::int32_t previous = gChatScrollOffsetC9A940;
            const std::int32_t count = world::gChatRecordCount_C9A944;
            const std::int32_t scroll = previous + 1;
            const bool pastChatCount = scroll > count;
            gChatScrollOffsetC9A940 = scroll;
            if (pastChatCount) {
                gChatScrollOffsetC9A940 = count;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
        if ((platform::gKeyStates[0x28] & 0x10u) != 0) {
            const std::int32_t scroll = gChatScrollOffsetC9A940 - 1;
            const bool belowFirstChatLine = scroll < 0;
            gChatScrollOffsetC9A940 = scroll;
            if (belowFirstChatLine) {
                gChatScrollOffsetC9A940 = 0;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
        }
        return;
    }
    if ((platform::gKeyStates[0x1B] & 4u) != 0) {
        WindowRuntime_ToggleShortcutWindow(24);
    }
    if ((platform::gKeyStates[0x7A] & 4u) != 0) {
        static_cast<void>(WindowRuntime_IncrementNameMode());
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if ((platform::gKeyStates[0x7B] & 4u) != 0) {
        static_cast<void>(render::TakeScreenshotThrottled_503C80());
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if ((platform::gKeyStates[0x24] & 4u) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        static_cast<void>(WindowChat_ClearHistory());
    }
}

// 三条重开共用的两轮窄清理；不释放实例，也不清记录的其他字段。
static void WindowRuntime_ResetReferences()
{
    gWindows.fill(nullptr);
    for (WindowState& state : gWindowStates) {
        state.window = nullptr;
    }
    WindowRuntime_ResetMouseAndItemFlags();
    gCurrentWindowIdD67A1C = 0;
    gCurrentElementIndex57C12C = -1;
}

static void WindowRuntime_OpenConfiguredWindows()
{
    static_cast<void>(WindowRuntime_Open(WindowId_BottomBar));
    switch (gStatusBarLayoutMode_CF4690) {
    case 0:
        static_cast<void>(WindowRuntime_Open(WindowId_StatusBar));
        break;
    case 1:
        static_cast<void>(WindowRuntime_Open(WindowId_CompactStatusBar));
        break;
    case 2:
        static_cast<void>(WindowRuntime_Open(WindowId_CollapsedStatusBar));
        break;
    default:
        break;
    }
    switch (gShortcutBarCollapsed_CA0440) {
    case 0:
        static_cast<void>(WindowRuntime_Open(WindowId_ShortcutBar));
        break;
    case 1:
        static_cast<void>(WindowRuntime_Open(WindowId_CollapsedShortcutBar));
        break;
    default:
        break;
    }
    switch (gChatWindowModeD66E04) {
    case 0:
        static_cast<void>(WindowRuntime_Open(WindowId_Chat));
        break;
    case 1:
        static_cast<void>(WindowRuntime_Open(WindowId_CompactChat));
        break;
    default:
        break;
    }
}

// 原函数：0x004707C0。
static void WindowRuntime_OpenLoginScene()
{
    WindowRuntime_ResetReferences();
    WindowRuntime_OpenConfiguredWindows();
    WindowRuntime_RestoreMarkedWindows();
    WindowRuntime_ClearElementFocus();
    gSelectedItemCB1DF4 = -1;
    gItemUseTargetReferences_D67B24 = 0;
    WindowRuntime_ResetOperationReferences();
    gPreviewActionD67B84 = nullptr;
    input::gChatPanelVisible_C9A96C = 1;
    static_cast<void>(WindowChat_RestoreInput());
}

// 原函数：0x00471170。
static void WindowRuntime_OpenBattleScene()
{
    WindowRuntime_ResetReferences();
    WindowRuntime_ClearElementFocus();
    static_cast<void>(WindowRuntime_Open(WindowId_BottomBar));
    static_cast<void>(WindowRuntime_Open(WindowId_CompactStatusBar));
    switch (gChatWindowModeD66E04) {
    case 0:
        static_cast<void>(WindowRuntime_Open(WindowId_Chat));
        break;
    case 1:
        static_cast<void>(WindowRuntime_Open(WindowId_CompactChat));
        break;
    default:
        break;
    }
    if (gWindowStates[WindowId_Contacts].marker50 != 0) {
        static_cast<void>(WindowRuntime_Open(WindowId_Contacts));
        gWindowStates[WindowId_Contacts].marker50 = 0;
    }
    if (gWindowStates[WindowId_MailRead].marker50 != 0) {
        const std::int32_t position = world::detail::gViewedMailMessageIndex_D046D4;
        static_cast<void>(WindowRuntime_Open(WindowId_MailRead));
        world::detail::gViewedMailMessageIndex_D046D4 = position;
        gWindowStates[WindowId_MailRead].marker50 = 0;
    }
    if (gWindowStates[WindowId_MailCompose].marker50 != 0 || gWindowStates[WindowId_PetMailCompose].marker50 != 0) {
        static_cast<void>(WindowRuntime_Open(WindowId_MailCompose));
        gWindowStates[WindowId_MailCompose].marker50 = 0;
        gWindowStates[WindowId_PetMailCompose].marker50 = 0;
    }
    WindowRuntime_ResetOperationReferences();
    WindowInstance* const window22 = gWindowStates[WindowId_MailCompose].window;
    gPreviewActionD67B84 = nullptr;
    input::gChatPanelVisible_C9A96C = 1;
    if (window22 == nullptr && gWindowStates[WindowId_PetMailCompose].window == nullptr) {
        static_cast<void>(WindowChat_RestoreInput());
    }
}

static void WindowRuntime_MarkSceneWindows()
{
    for (const std::int32_t id : std::array<std::int32_t, 4>{21, 40, 22, 23}) {
        if (gWindowStates[id].window != nullptr) {
            gWindowStates[id].marker50 = 1;
        }
    }
}

// 原函数：0x00470980。
static void WindowRuntime_OpenNormalScene()
{
    WindowRuntime_ResetReferences();
    WindowRuntime_ClearElementFocus();
    WindowRuntime_OpenConfiguredWindows();
    WindowRuntime_RestoreMarkedWindows();
    gSelectedItemCB1DF4 = -1;
    gItemUseTargetReferences_D67B24 = 0;
    WindowRuntime_ResetOperationReferences();
    const std::int32_t inputGate = input::gChatLogSuppressed_626C14;
    gPreviewActionD67B84 = nullptr;
    if (inputGate == 0) {
        input::gChatPanelVisible_C9A96C = 1;
    }
    if (gWindowStates[WindowId_MailCompose].window == nullptr && gWindowStates[WindowId_PetMailCompose].window == nullptr) {
        static_cast<void>(WindowChat_RestoreInput());
    }
}

// 原函数：0x004705D0。
std::int32_t WindowRuntime_Initialize()
{
    static_cast<void>(WindowChat_ResetState());
    static_cast<void>(WindowRuntime_LoadSettings());
    WindowRuntime_ClearSavedWindowMarkers();
    return world::detail::LoadProfileCategoryFile_534910();
}

// 原函数：0x004923D0。
static std::int32_t WindowRuntime_ResetAlbumState()
{
    gLoginWindowRuntime.valueCF4A00 = 5;
    gLoginWindowRuntime.valueC8A95C = 0;
    gLoginWindowRuntime.valueCB2200 = 0;
    return 0;
}

// 原函数：0x004742B0。
static std::int32_t WindowScroll_ResetPosition()
{
    gLoginWindowRuntime.valuesCF4E50[0] = 0;
    gLoginWindowRuntime.valuesCF4E50[1] = 0;
    return 0;
}

// 原函数：0x00470600。
static void WindowRuntime_InitializeLogin()
{
    static_cast<void>(WindowRuntime_LoadSettings());
    static_cast<void>(WindowRuntime_ResetAlbumState());
    WindowMessageBoard_ResetRecords();
    static_cast<void>(WindowChat_ResetScrollRequests());
    static_cast<void>(WindowMail_ResetComposeState());
    static_cast<void>(WindowPetList_ResetSelection());
    static_cast<void>(WindowScroll_ResetPosition());
    world::detail::ResetMiniMailSequenceAndSelection_4C9490();
    world::detail::ClearMiniMailAndProfiles_533910();
    if (world::detail::LoadPlayerProfilesFile_5347F0() == 0) {
        world::detail::InitializeDefaultProfiles_533950();
    }
    static_cast<void>(WindowMail_ResetSelectionState());
    gWindowStates[WindowId_Contacts].marker50 = 0;
    gWindowStates[WindowId_MailCompose].marker50 = 0;
    gWindowStates[WindowId_PetMailCompose].marker50 = 0;
    gWindowStates[WindowId_MailRead].marker50 = 0;
    static_cast<void>(WindowBlacklist_ResetState());
    WindowAdventureLog_InitializeDate();
    gLoginWindowRuntime.valueD67618 = 0;
    gLoginWindowRuntime.bankCurrentPage = 0; // G07：与原银行入口同一生命周期，金额组仍独立。
}

// 原函数：0x00477490。
WindowInstance* WindowRuntime_FindById(const std::int32_t id)
{
    for (std::size_t index = 0; index < gWindows.size(); ++index) {
        WindowInstance* const window = gWindows[index];
        if (window->id == id) {
            return gWindows[index];
        }
    }
    return nullptr;
}

// WND01：数字与四符号原字节正反绘制，见doc/cg2d_window_number_draw_spec.md。
// 原函数：0x004771B0。
void WindowNumber_Draw(
    const std::int32_t x, const std::int32_t y, const std::wstring_view text,
    const std::int8_t style, const std::int8_t color,
    const std::uint8_t flags, const std::uint8_t layer)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = 0xFFFFFFFFu;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;

    std::int32_t drawX;
    std::int32_t drawY;
    std::int32_t step;
    const bool reverse = (flags & 1u) != 0;
    if (reverse) {
        drawX = x - kNumberOffsets54F278[style * 3 + 1];
        drawY = kNumberOffsets54F278[style * 3 + 2];
        step = -gNumberAdvancesD67AC4[style];
        drawY += y;
    } else {
        step = gNumberAdvancesD67AC4[style];
        drawX = kNumberOffsets54F278[style * 3] + x;
        drawY = kNumberOffsets54F278[style * 3 + 2] + y;
    }

    const auto drawCharacter = [&](const wchar_t character) {
        std::int32_t imageNo;
        if (character >= L'0' && character <= L'9') {
            imageNo = kNumberDigitImages54F0D8[style * 13 + color] + character - L'0';
        } else {
            std::int32_t symbol;
            switch (character) {
            case L'+': symbol = 0; break;
            case L'-': symbol = 1; break;
            case L':': symbol = 2; break;
            case L'/': symbol = 3; break;
            default: return;
            }
            imageNo = kNumberSymbolImages54F1A8[style * 13 + color] + symbol;
        }
        static_cast<void>(cg::render::DrawGraphic(
            drawX, drawY, layer, imageNo, 0, &descriptor));
    };

    if (reverse) {
        for (auto cursor = text.rbegin(); cursor != text.rend(); ++cursor) {
            drawCharacter(*cursor);
            drawX += step;
        }
    } else {
        for (const wchar_t character : text) {
            drawCharacter(character);
            drawX += step;
        }
    }
}

// WND01：运行步进乘长度后取扣宽，见doc/cg2d_window_number_draw_spec.md。
// 原函数：0x00477460。
std::int32_t WindowNumber_MeasureWidth(const std::wstring_view text, const std::int8_t style)
{
    const auto length = static_cast<std::int32_t>(text.size());
    if (length <= 0) {
        return 0;
    }
    const std::int32_t advance = gNumberAdvancesD67AC4[style];
    const std::int32_t width = advance * length;
    const std::int32_t deduction = kNumberSpacingDeductions54F2A8[style];
    return width - deduction;
}

// 原函数：0x0046FBC0。
std::int32_t WindowRuntime_DrawDefault(const std::int32_t)
{
    WindowRuntime_DrawCurrent();
    return 1;
}

// 原函数：0x0049BDB0。
std::int32_t WindowRuntime_DrawCompactFrame(const std::int32_t)
{
    WindowRuntime_DrawCurrent();
    WindowInstance* const window = gCurrentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    cg::yobi::DrawNineGrid_49BE00(
        0x3BE3E, x, y, width, height, 0xDF, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    render::ClearTextQueueTail_456D50(3);
    return 1;
}

// 原函数：0x004796D0。
void WindowRuntime_IncrementGatherOperationCount()
{
    ++gGatherOperationCount_D67AFC;
}

// 原函数：0x004796E0。
void WindowRuntime_DecrementGatherOperationCount()
{
    --gGatherOperationCount_D67AFC;
}

// 原函数：0x00479650。
void WindowRuntime_IncrementOperationCount()
{
    ++gWindowOperationCount_D67AF8;
}

// 原函数：0x00479660。
void WindowRuntime_DecrementOperationCount()
{
    const std::int32_t remainingCount = gWindowOperationCount_D67AF8 - 1;
    gWindowOperationCount_D67AF8 = remainingCount;
    if (remainingCount != 0 || gSkillRebirthModeEnabled_5F9754 == 0) {
        return;
    }
    const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
    static_cast<void>(network::SendWorldActionRequest_511DD0(platform::gConnectionSocket, x, y, 62));
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    const std::uint8_t previous = occupancy[7];
    const auto next = static_cast<std::uint8_t>(previous - 1);
    gSkillRebirthModeEnabled_5F9754 = 0;
    occupancy[7] = next;
    WindowSkill_SyncStates();
}

// 原函数：0x00499830。
std::int32_t WindowRuntime_DrawLargeFrame(const std::int32_t)
{
    WindowRuntime_DrawCurrent();
    WindowInstance* const window = gCurrentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    cg::yobi::DrawNineGrid_49BE00(
        245300, x, y, width, height, 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

// WSP01：先当前/前项高度，再两次整数换算；非正maximum原分支和+8字节载荷保。
// 原函数：0x00474270。
std::int32_t WindowScroll_ReadPosition(WindowElement* const element, const std::int32_t maximum)
{
    if (maximum <= 0) {
        return 0;
    }
    const std::int32_t height = element->height;
    const std::int32_t previousHeight = (element - 1)->height;
    const std::int32_t span = height - previousHeight;
    const std::int32_t halfStep = span / (maximum * 2);
    const auto* const data = static_cast<const std::byte*>(element->data);
    std::int32_t delta;
    std::memcpy(&delta, data + offsetof(WindowMouseData, deltaY), sizeof(delta));
    return (halfStep + delta) * maximum / span;
}

// 原函数：0x004742C0。
std::int32_t WindowScroll_SetPosition(
    WindowElement* const element, const std::int32_t maximum, const std::int32_t position)
{
    auto* const data = static_cast<std::byte*>(element->data);
    std::int32_t y;
    if (maximum <= 0) {
        y = element->y;
    } else {
        const std::int32_t height = element->height;
        const std::int32_t previousHeight = (element - 1)->height;
        y = (height - previousHeight) * position / maximum;
        y += element->y;
    }
    (element - 1)->y = y;
    const std::int32_t delta = y - element->y;
    // 4C0160也传入type1图形载荷；原+8写入对应其offsetY。
    static_assert(offsetof(WindowMouseData, deltaY) == offsetof(WindowGraphicData, offsetY));
    std::memcpy(data + offsetof(WindowMouseData, deltaY), &delta, sizeof(delta));
    return delta;
}

// WWS01：原余量/像素结果持样和边界/flags取样序；现宿主数学图及原钳制保。
// 原函数：0x004743B0。
std::int32_t WindowScroll_ApplyWheel(
    WindowElement* const element, std::int32_t position, std::int32_t maximum,
    const std::int32_t wheel)
{
    auto* const data = static_cast<std::byte*>(element->data);
    if (wheel == 0) {
        return position;
    }
    if (maximum < 0) {
        maximum = 0;
    }
    WindowElement* const previous = element - 1;
    const std::int32_t oldY = previous->y;
    static_cast<void>(WindowScroll_SetPosition(element, maximum, position));
    std::int32_t remainder = oldY - previous->y;
    constexpr float kWheelStepScale = 1.0F / 120.0F; // 滚轮输入到滚动步数的比例。
    const double product = static_cast<double>(wheel)
        * kWheelStepScale;
    const float roundedProduct = static_cast<float>(product);
    const auto whole = static_cast<std::int32_t>(product);
    position += whole;
    if (position < 0) {
        position = 0;
        remainder = 0;
    }
    if (position > maximum) {
        position = maximum;
        remainder = 0;
    }
    static_cast<void>(WindowScroll_SetPosition(element, maximum, position));
    const std::int32_t remainderY = previous->y + remainder;
    previous->y = remainderY;
    const std::int32_t height = element->height;
    const std::int32_t previousHeight = previous->height;
    const std::int32_t span = height - previousHeight;
    const double fraction = static_cast<double>(roundedProduct) - whole;
    const double pixels = fraction * span / maximum;
    const std::int32_t step = static_cast<std::int32_t>(pixels);
    const std::int32_t steppedY = remainderY + step;
    previous->y = steppedY;
    const std::int32_t topY = element->y;
    if (steppedY < topY) {
        previous->y = topY;
    }
    const std::int32_t bottomHeight = element->height;
    const std::int32_t bottomY = element->y;
    const std::int32_t previousBottomHeight = previous->height;
    const std::int32_t previousBottomY = previous->y;
    if (previousBottomY + previousBottomHeight > bottomY + bottomHeight) {
        previous->y = bottomY - previousBottomHeight + bottomHeight;
    }
    const std::int32_t previousY = previous->y;
    const std::int32_t baseY = element->y;
    std::uint32_t flags;
    std::memcpy(&flags, data + offsetof(WindowMouseData, flags), sizeof(flags));
    const std::int32_t delta = previousY - baseY;
    flags |= 1u;
    std::memcpy(data + offsetof(WindowMouseData, deltaY), &delta, sizeof(delta));
    std::memcpy(data + offsetof(WindowMouseData, flags), &flags, sizeof(flags));
    return position;
}

// 原函数：0x00485110。
void WindowElement_DrawHoverFrame(
    WindowElement* const element, const std::int32_t state, const std::uint32_t imageNo)
{
    if ((state & 1) == 0) {
        return;
    }
    WindowInstance* const window = gCurrentWindow;
    const std::int32_t left = window->x + element->x - 1;
    const std::int32_t top = window->y + element->y - 1;
    const std::int32_t bottom = element->height + top + 2;
    const std::int32_t right = element->width + left + 2;
    cg::render::DrawGraphicHover(
        static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
        static_cast<std::uint16_t>(right), static_cast<std::uint16_t>(bottom),
        0xDF, imageNo, 0);
}

// 原函数：0x00472F00。
static std::int32_t WindowRuntime_HitTestRect(
    const std::int32_t left, const std::int32_t top,
    const std::int32_t width, const std::int32_t height)
{
    const std::int32_t x = (platform::gLegacyMouseState.mouseX - static_cast<std::int32_t>(left));
    if (x < 0 || x >= width) {
        return 0;
    }
    const std::int32_t y = (platform::gLegacyMouseState.mouseY - static_cast<std::int32_t>(top));
    return y >= 0 && y < height ? 1 : 0;
}

// 原函数：0x00475D40。
std::int32_t WindowDrag_ReadFocusWindowId()
{
    return gFocusWindowId57C120;
}

// 原函数：0x00475D50。
std::int32_t WindowDrag_ReadFocusElement()
{
    return gFocusElementIndex57C124;
}

// 原函数：0x00475D60。
std::int32_t WindowDrag_ReadKind()
{
    return gMouseValue57C110;
}

// 原函数：0x00475D70。
std::int32_t WindowDrag_ReadX()
{
    return gDragX57C11C;
}

// 原函数：0x00475D80。
std::int32_t WindowDrag_ReadY()
{
    return gDragY57C11E;
}

// 原函数：0x00475D90。
std::int32_t WindowDrag_ReadSourceIndex()
{
    return gDragSourceIndex57C114;
}

// DLP01：仅phase0且kind非零启动阶段1，按原焦点/回调/来源写序发布，返S32 0/1。
// 原函数：0x00475DA0。
std::int32_t WindowDrag_Start(
    const std::int32_t kind, const std::int32_t sourceIndex, const WindowDragCallback callback)
{
    if (gMousePhase57C128 != 0 || kind == 0) {
        return 0;
    }
    gMouseValue57C110 = kind;
    const std::int32_t windowId = gCurrentWindowIdD67A1C;
    gDragCallback57C118 = callback;
    const std::int32_t elementIndex = gCurrentElementIndex57C12C;
    gFocusWindowId57C120 = windowId;
    gDragSourceIndex57C114 = sourceIndex;
    gFocusElementIndex57C124 = elementIndex;
    gMousePhase57C128 = 1;
    return 1;
}

// DLP01：仅phase0且kind非零启动阶段3，坐标保S16，原焦点与回调写序保持，返S32 0/1。
// 原函数：0x00476490。
std::int32_t WindowDrag_StartAtPosition(
    const std::int32_t kind, const std::int32_t sourceIndex,
    const std::int16_t x, const std::int16_t y, const WindowDragCallback callback)
{
    if (gMousePhase57C128 != 0 || kind == 0) {
        return 0;
    }
    gMouseValue57C110 = kind;
    gDragSourceIndex57C114 = sourceIndex;
    gDragCallback57C118 = callback;
    const std::int32_t windowId = gCurrentWindowIdD67A1C;
    gDragX57C11C = x;
    const std::int32_t elementIndex = gCurrentElementIndex57C12C;
    gDragY57C11E = y;
    gFocusWindowId57C120 = windowId;
    gFocusElementIndex57C124 = elementIndex;
    gMousePhase57C128 = 3;
    return 1;
}

// DLP01：入口phase4/5才可选回调message3，之后按原八槽顺序清理；不重验回调后phase。
// 原函数：0x00476500。
void WindowDrag_Reset()
{
    const std::int32_t phase = gMousePhase57C128;
    if (phase != 4 && phase != 5) {
        return;
    }
    const WindowDragCallback callback = gDragCallback57C118;
    if (callback != nullptr) {
        callback(3, 0, gDragSourceIndex57C114);
    }
    gMouseValue57C110 = 0;
    gDragSourceIndex57C114 = 0;
    gDragCallback57C118 = nullptr;
    gDragX57C11C = 0;
    gDragY57C11E = 0;
    gFocusWindowId57C120 = 0;
    gMousePhase57C128 = 0;
    gFocusElementIndex57C124 = -1;
}

// DLP01：已有kind才绑定两个S32焦点槽，不改变会话阶段，返S32 0/1。
// 原函数：0x004763B0。
std::int32_t WindowDrag_SetFocus(
    const std::int32_t windowId, const std::int32_t elementIndex)
{
    if (gMouseValue57C110 == 0) {
        return 0;
    }
    gFocusWindowId57C120 = windowId;
    gFocusElementIndex57C124 = elementIndex;
    return 1;
}

// DLP01：只清焦点索引/kind/焦点窗口/回调/phase五槽，来源及两坐标保持，返S32 0。
// 原函数：0x004763E0。
std::int32_t WindowDrag_ResetMouse()
{
    gFocusElementIndex57C124 = -1;
    gMouseValue57C110 = 0;
    gFocusWindowId57C120 = 0;
    gDragCallback57C118 = nullptr;
    gMousePhase57C128 = 0;
    return 0;
}

// WDF01：同一拖动W32样本/四BYTE消费与矩形/焦点原取序；结果为1/2掩码。
// 原函数：0x00472D20。
static std::int32_t WindowDrag_Update()
{
    const std::int32_t initialPhase = gMousePhase57C128;
    WindowInstance* window = gCurrentWindow;
    std::int32_t result = 0;
    if (initialPhase != 1) {
        const std::uint32_t state = window->state;
        if ((state & 2u) == 0) {
            if ((state & 0x800u) != 0) {
                if ((*reinterpret_cast<const unsigned char*>(&platform::gLegacyMouseState.heldButtons) & 1u) != 0) {
                    std::int32_t* const x = &window->x;
                    const std::int32_t deltaX = (platform::gLegacyMouseState.mouseX - static_cast<std::int32_t>(window->dragMouseX1C));
                    *x += deltaX;
                    window = gCurrentWindow;
                    std::int32_t* const y = &window->y;
                    const std::int32_t deltaY = (platform::gLegacyMouseState.mouseY - static_cast<std::int32_t>(window->dragMouseY20));
                    *y += deltaY;
                    const std::int32_t mouseX = static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
                    gCurrentWindow->dragMouseX1C = mouseX;
                    const std::int32_t mouseY = static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
                    gCurrentWindow->dragMouseY20 = mouseY;
                } else {
                    window->state = state & ~std::uint32_t{0x800};
                }
                result = 2;
                window = gCurrentWindow;
            } else if ((*reinterpret_cast<const unsigned char*>(&platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
                std::int32_t count = window->secondaryCount;
                for (std::int32_t index = 0; index < count; ++index) {
                    const WindowDragRect& rect = window->dragRects40[index];
                    const std::int32_t rectX = rect.x;
                    const std::int32_t windowX = window->x;
                    const std::int32_t rectY = rect.y;
                    const std::int32_t windowY = window->y;
                    const std::int32_t left = rectX + windowX;
                    const std::int32_t height = rect.height;
                    const std::int32_t top = rectY + windowY;
                    const bool focusAvailable = gWindowHasFocusCA0AB4 == 0;
                    const std::int32_t width = rect.width;
                    if (focusAvailable) {
                        if (WindowRuntime_HitTestRect(left, top, width, height) != 0) {
                            const std::int32_t mouseX = static_cast<std::int32_t>(platform::gLegacyMouseState.mouseX);
                            gCurrentWindow->dragMouseX1C = mouseX;
                            const std::int32_t mouseY = static_cast<std::int32_t>(platform::gLegacyMouseState.mouseY);
                            gCurrentWindow->dragMouseY20 = mouseY;
                            gCurrentWindow->state |= 0x800u;
                            result |= 2;
                        }
                        window = gCurrentWindow;
                    }
                    count = window->secondaryCount;
                }
            }
        }
    }
    if ((*reinterpret_cast<const unsigned char*>(&window->state) & 1u) == 0) {
        const std::int32_t phase = gMousePhase57C128;
        if (phase == 1 || phase == 4) {
            const std::int32_t height = window->height;
            const std::int32_t width = window->width;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            if (WindowRuntime_HitTestRect(x, y, width, height) != 0) {
                result |= 1;
            }
        } else if (gWindowHasFocusCA0AB4 == 0) {
            const std::int32_t height = window->height;
            const std::int32_t width = window->width;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            if (WindowRuntime_HitTestRect(x, y, width, height) != 0) {
                result |= 1;
                if ((*reinterpret_cast<const unsigned char*>(&platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
                    gCurrentWindow->state |= 0x2000u;
                }
            }
        }
    }
    return result;
}

// WCR01：71槽整理共用原state样本分流；关闭后压紧、等层前插与计数见对应SPEC。
// 原函数：0x004730A0。
static std::int32_t WindowRuntime_CompactAndReorder()
{
    std::int32_t count = 0;
    for (std::size_t index = 0; index < kWindowCount;) {
        WindowInstance* const window = gWindows[index];
        if (window == nullptr) {
            std::size_t source = index;
            while (source < kWindowCount && gWindows[source] == nullptr) {
                ++source;
            }
            if (source == kWindowCount) {
                break;
            }
            gWindows[index] = gWindows[source];
            gWindows[source] = nullptr;
            continue;
        }
        const std::uint32_t state = window->state;
        if ((state & 0x8000u) != 0) {
            WindowRuntime_Free(window);
            for (std::size_t target = index; target + 1 < kWindowCount; ++target) {
                gWindows[target] = gWindows[target + 1];
            }
            gWindows[kWindowCount - 1] = nullptr;
            if (index + 1 != kWindowCount) {
                continue;
            }
        } else {
            if ((state & 0x2000u) != 0) {
                std::size_t target = index;
                while (target > 0) {
                    WindowInstance* const previous = gWindows[target - 1];
                    if (previous->layer < window->layer) {
                        break;
                    }
                    gWindows[target] = previous;
                    --target;
                }
                gWindows[target] = window;
                window->state &= ~std::uint32_t{0x2000};
            }
            ++count;
        }
        ++index;
    }
    return count;
}

// 原函数：0x00473E40。
static void WindowRuntime_AdvanceMousePhase()
{
    std::int32_t phase = gMousePhase57C128;
    if (phase == 1) {
        const WindowDragCallback callback = gDragCallback57C118;
        if (callback == nullptr) {
            gWindowHasFocusCA0AB4 = 1;
            *reinterpret_cast<unsigned char*>(&platform::gLegacyMouseState.valueD68DB4) = 5;
            return;
        }
        const std::int32_t events = WindowRuntime_AppendMouseEvents(0);
        callback(0, events, gDragSourceIndex57C114);
        phase = gMousePhase57C128;
    } else if (phase == 2) {
        gMousePhase57C128 = 0;
        return;
    }
    switch (phase) {
    case 3:
        gMousePhase57C128 = 4;
        gWindowHasFocusCA0AB4 = 1;
        *reinterpret_cast<unsigned char*>(&platform::gLegacyMouseState.valueD68DB4) = 5;
        return;
    case 4: {
        const WindowDragCallback callback = gDragCallback57C118;
        if (callback != nullptr) {
            callback(2, 0, gDragSourceIndex57C114);
        }
        gMouseValue57C110 = 0;
        gDragSourceIndex57C114 = 0;
        gDragCallback57C118 = nullptr;
        gDragX57C11C = 0;
        gDragY57C11E = 0;
        gFocusWindowId57C120 = 0;
        gMousePhase57C128 = 0;
        gFocusElementIndex57C124 = -1;
        return;
    }
    case 6:
        gMousePhase57C128 = 0;
        return;
    case 1:
        gWindowHasFocusCA0AB4 = 1;
        *reinterpret_cast<unsigned char*>(&platform::gLegacyMouseState.valueD68DB4) = 5;
        break;
    default:
        break;
    }
}

// 原函数：0x00473560。
static void WindowRuntime_DispatchElementMouse(const std::int32_t index)
{
    WindowInstance* const window = gCurrentWindow;
    WindowElement* const element = &window->elements[index];
    const std::int32_t left = window->x + element->x;
    const std::int32_t top = window->y + element->y;
    const std::int32_t width = element->width;
    const std::int32_t height = element->height;
    std::int32_t events = 0;
    std::int32_t phase = gMousePhase57C128;
    if (phase == 1 || phase == 4 || phase == 2 || phase == 6) {
        if (static_cast<std::uint8_t>(platform::gLegacyMouseState.valueD68DB4) < 0xD4u) {
            if (WindowRuntime_HitTestRect(left, top, width, height) != 0) {
                phase = gMousePhase57C128;
                if (phase == 1 || phase == 4) {
                    events = 0x20;
                }
                if (gFocusWindowId57C120 == gCurrentWindowIdD67A1C
                    && gFocusElementIndex57C124 == gCurrentElementIndex57C12C) {
                    events = WindowRuntime_AppendMouseEvents(events);
                }
            } else {
                phase = gMousePhase57C128;
            }
        }
    } else if (gWindowHasFocusCA0AB4 == 0) {
        if (WindowRuntime_HitTestRect(left, top, width, height) != 0) {
            events = WindowRuntime_AppendMouseEvents(1);
            WindowInstance* const currentWindow = gCurrentWindow;
            WindowElement* const currentElement = &currentWindow->elements[index];
            if (currentElement->type == WindowElementType_MouseArea) {
                auto* const data = static_cast<WindowMouseData*>(currentElement->data);
                data->deltaX = (platform::gLegacyMouseState.mouseX - static_cast<std::int32_t>(currentWindow->x + currentElement->x));
                WindowInstance* const yWindow = gCurrentWindow;
                data->deltaY = (platform::gLegacyMouseState.mouseY - static_cast<std::int32_t>(yWindow->y + yWindow->elements[index].y));
            }
        }
        phase = gMousePhase57C128;
    }
    if (phase == 4) {
        const std::int32_t dragX = gDragX57C11C - left;
        if (dragX >= 0 && dragX < width) {
            const std::int32_t dragY = gDragY57C11E - top;
            if (dragY >= 0 && dragY < height) {
                events |= 0x40;
            }
        }
    }
    if (element->visible == 1 && element->mouseCallback(index, events) != 0) {
        phase = gMousePhase57C128;
        auto* const eventByte = reinterpret_cast<unsigned char*>(
            &platform::gLegacyMouseState.valueD68DB4);
        *eventByte = 0xD4;
        gWindowHasFocusCA0AB4 = 1;
        if (phase == 4) {
            const WindowDragCallback callback = gDragCallback57C118;
            if (callback != nullptr) {
                callback(4, 0, gDragSourceIndex57C114);
            }
            static_cast<void>(WindowDrag_ResetMouse());
        }
    }
}

void WindowRuntime_UpdateExtendedElements()
{
    for (std::int32_t index = 0; index < gCurrentWindow->elementCount; ++index) {
        gCurrentElementIndex57C12C = index;
        if (gCurrentWindow->elements[index].visible == 1) {
            WindowRuntime_DispatchElementMouse(index);
        }
    }
    gCurrentElementIndex57C12C = -1;
}

// WSB01：原载荷字节借用及上下界/差值三个现场取样阶段见滚动回调SPEC。
// 原函数：0x004741A0。
std::int32_t WindowElement_HandleVerticalScroll(const std::int32_t index, const std::int32_t state)
{
    // 窗口49的type1与普通type4共享+0/+8语义，按对象字节访问而不强转异型结构。
    auto* const data = static_cast<std::byte*>(gCurrentWindow->elements[index].data);
    static_assert(offsetof(WindowMouseData, flags) == offsetof(WindowGraphicData, imageNo));
    static_assert(offsetof(WindowMouseData, deltaY) == offsetof(WindowGraphicData, offsetY));
    std::int32_t result = 0;
    if ((std::to_integer<std::uint8_t>(data[0]) & 2u) == 0) {
        const std::uint32_t flags = 0;
        std::memcpy(data, &flags, sizeof(flags));
    }
    if ((state & 8) != 0) {
        const std::uint32_t flags = 3;
        std::memcpy(data, &flags, sizeof(flags));
        result = 1;
    }
    if ((std::to_integer<std::uint8_t>(data[0]) & 2u) != 0) {
        WindowInstance* const window = gCurrentWindow;
        WindowElement* const previous = &window->elements[index - 1];
        const std::int32_t y = (platform::gLegacyMouseState.mouseY - static_cast<std::int32_t>(window->y));
        previous->y = y - (previous->height >> 1);
    }
    WindowElement* element = &gCurrentWindow->elements[index];
    const std::int32_t previousTopY = element[-1].y;
    const std::int32_t topY = element->y;
    if (previousTopY < topY) {
        element[-1].y = topY;
    }
    element = &gCurrentWindow->elements[index];
    const std::int32_t height = element->height;
    const std::int32_t bottomY = element->y;
    const std::int32_t bottom = height + bottomY;
    const std::int32_t previousHeight = element[-1].height;
    const std::int32_t previousBottomY = element[-1].y;
    if (previousBottomY + previousHeight > bottom) {
        element[-1].y = bottom - previousHeight;
    }
    element = &gCurrentWindow->elements[index];
    const std::int32_t currentY = element->y;
    const std::int32_t previousY = element[-1].y;
    const std::int32_t delta = previousY - currentY;
    std::memcpy(data + offsetof(WindowMouseData, deltaY), &delta, sizeof(delta));
    if ((state & 8) == 0) {
        std::uint32_t flags;
        std::memcpy(&flags, data, sizeof(flags));
        flags &= ~std::uint32_t{2};
        std::memcpy(data, &flags, sizeof(flags));
    }
    return result;
}

// WIC01：原型四模式、现场context与BYTE结果合同，见doc/cg2d_window_input_context_dispatch_spec.md。
// 原函数：0x00476570。
std::uint8_t WindowInput_ActivateElement(
    WindowInputPrototype* const prototype, const std::int32_t x,
    const std::int32_t y, const std::uint32_t mode)
{
    if (prototype->input.kind == WindowInputReferenceKind::RejectedValue) {
        return 0;
    }
    input::InputContext* const context = prototype->input.context;
    if (context == nullptr) {
        return 0;
    }
    switch (mode) {
    case 0: {
        const std::int32_t encrypt = prototype->encrypt;
        const std::int32_t value224 = prototype->value224;
        const std::int32_t maxCharPerLine = prototype->maxCharPerLine;
        const std::int32_t maxLine = prototype->maxLine;
        const std::wstring& text = prototype->text;
        const std::int32_t color = prototype->color;
        const std::int32_t fontType = prototype->fontType;
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t inputY = prototype->y + y;
        const std::int32_t inputX = prototype->x + x;
        input::InitializeContext_449CF0(
            context, inputX, inputY, renderMode, fontType, color, text,
            maxLine, maxCharPerLine, value224, encrypt);
        return 1;
    }
    case 1: {
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t color = prototype->color;
        const std::int32_t maxCharPerLine = prototype->maxCharPerLine;
        const std::int32_t inputY = prototype->y + y;
        const std::int32_t inputX = prototype->x + x;
        input::ConfigureInputLayoutAndResetScroll_45D720(context, inputX, inputY, maxCharPerLine, color, renderMode);
        [[fallthrough]];
    }
    case 2: {
        const std::int32_t inputX = prototype->x + x;
        prototype->input.context->field130 = inputX;
        const std::int32_t inputY = prototype->y + y;
        prototype->input.context->field134 = inputY;
        return 1;
    }
    case 3: {
        const std::int32_t fontType = prototype->fontType;
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t color = context->field140;
        const std::int32_t inputY = prototype->y + y;
        const std::int32_t inputX = prototype->x + x;
        input::SetInputPositionAndStyle_45D760(context, inputX, inputY, color, renderMode, fontType);
        break;
    }
    default:
        break;
    }
    return 1;
}

// 原函数：0x004740F0。
static void WindowInput_FormatSkillSlotCount(std::wstring& destination)
{
    std::int32_t count = 0;
    for (std::size_t position = 0; position < world::kSkillSlotCount; ++position) {
        const auto& pair = world::gSkillPairsF63370[position];
        if (pair.used != 0) {
            const std::int16_t index = std::bit_cast<std::int16_t>(pair.index);
            const auto& skill = world::gSkillsF1BAFC[index];
            std::int16_t requiredSlots;
            std::memcpy(&requiredSlots, skill.requiredSlots19.data(), sizeof(requiredSlots));
            count += requiredSlots;
        }
    }
    cg::text::FormatTo(destination, L"{:2}/{}", count, world::gPlayerRuntimeSlots.value16C);
}

// 同一4A4210在原表也用于release；仅适配未消费返回值，不转换函数指针。
// WCL01：窗口release家族丢弃原4A4210常量1；真实七类资源回收留在FreeWindow，非此适配。
// 原函数：0x004A4210。
void WindowRuntime_ReleaseDefault()
{
    static_cast<void>(WindowRuntime_ReturnOne(0));
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x00473F80。
std::int32_t WindowRuntime_Update()
{
    using namespace detail;
    // 仅诊断去重，不参与字段／协议状态；失败回调结束后仍继续输入和绘制。
    static input::InputContext* reportedContext = nullptr;
    static std::wstring reportedText;
    static bool reportedFailure = false;
    bool encodingFailed = false;
    const auto invoke = [&](auto&& callback) -> std::int32_t {
        try { return callback(); }
        catch (const cg::text::GbkStorageError&) {
            encodingFailed = true;
            input::InputContext* const context = input::gCurrentInputData;
            const std::wstring contents = context != nullptr ? input::ContextText(*context) : std::wstring{};
            if (!reportedFailure || reportedContext != context || reportedText != contents) {
                reportedContext = context;
                reportedText = contents;
                reportedFailure = true;
                m_log_iw(L"文字无法按GBK保存，或已超出字段容量。");
            }
            return 0;
        }
    };
    std::int32_t carried = 1;
    world::gMapTargetInputOverride_C8A77C = 0;
    world::gMapTargetDrawOverride_C8A780 = 0;
    gWindowHasFocusCA0AB4 = 0;
    WindowRuntime_AdvanceMousePhase();
    script::PrepareLuaWindowFrame();
    const std::int32_t count = WindowRuntime_CompactAndReorder();
    const auto scheduledWindows = script::MergeLuaWindowOrder(count);
    const bool hasWindows = !scheduledWindows.empty();
    gWindowUpdateCountCC4844 = count;
    if (hasWindows) {
        std::int32_t index = 0;
        for (;;) {
            const auto& scheduled = scheduledWindows[index];
            WindowInstance* window = scheduled.window;
            const auto& extension = scheduled.extension;
            if ((!extension && gWindowStates[scheduled.id].window != window)
                || (window->state & 0x8000u) != 0) {
                if (++index >= static_cast<std::int32_t>(scheduledWindows.size())) break;
                continue;
            }
            gCurrentWindow = window;
            gCurrentWindowState = extension ? &extension->state : &gWindowStates[window->id];
            gCurrentWindowIdD67A1C = window->id;
            const std::int32_t focused = WindowDrag_Update();
            std::int32_t updateResult;
            if (extension) {
                updateResult = invoke([&] { return extension->update(carried, focused); });
            } else {
                updateResult = invoke([&] { return gCurrentWindow->update(carried); });
                window = gCurrentWindow;
                // G07：页按钮先接收放下事件，避免原背包背景先重置拖动会话。
                const std::int32_t originalCount = gWindowPrototypes57C130[window->id]->elementCount;
                const bool hasPageElements = window->elementCount > originalCount;
                for (std::int32_t pageIndex = originalCount; pageIndex < window->elementCount; ++pageIndex) {
                    gCurrentElementIndex57C12C = pageIndex;
                    if (window->elements[pageIndex].visible == 1) {
                        static_cast<void>(invoke([&] { WindowRuntime_DispatchElementMouse(pageIndex); return 0; }));
                        window = gCurrentWindow;
                    }
                }
                std::int32_t elementIndex = 0;
                gCurrentElementIndex57C12C = 0;
                while (elementIndex < (hasPageElements ? originalCount : window->elementCount)) {
                    if (window->elements[elementIndex].visible == 1) {
                        static_cast<void>(invoke([&] { WindowRuntime_DispatchElementMouse(elementIndex); return 0; }));
                        window = gCurrentWindow;
                        elementIndex = gCurrentElementIndex57C12C;
                    }
                    ++elementIndex;
                    gCurrentElementIndex57C12C = elementIndex;
                }
            }
            const bool hasFocus = focused != 0;
            gCurrentElementIndex57C12C = -1;
            if (hasFocus) {
                const std::int32_t phase = gMousePhase57C128;
                *reinterpret_cast<unsigned char*>(
                    &platform::gLegacyMouseState.valueD68DB4) = 212;
                const bool releaseDrag = phase == 4;
                gWindowHasFocusCA0AB4 = 1;
                if (releaseDrag) {
                    const WindowDragCallback callback = gDragCallback57C118;
                    if (callback != nullptr) {
                        callback(4, 0, gDragSourceIndex57C114);
                    }
                    static_cast<void>(WindowDrag_ResetMouse());
                    window = gCurrentWindow;
                }
                updateResult = 0;
            }
            carried = invoke([&] { return extension ? extension->render(updateResult) : window->draw(updateResult); });
            const std::int32_t currentCount = static_cast<std::int32_t>(scheduledWindows.size());
            ++index;
            if (index >= currentCount) {
                break;
            }
        }
    }
    WindowRuntime_ProcessShortcuts();
    static_cast<void>(input::ProcessPresetHotkeys_44B340());
    const std::int32_t phase = gMousePhase57C128;
    gCurrentWindowIdD67A1C = 0;
    if (phase == 1
        && static_cast<std::uint8_t>(platform::gLegacyMouseState.valueD68DB4) < 212u) {
        world::gMapTargetDrawOverride_C8A780 = 1;
        *reinterpret_cast<unsigned char*>(
            &platform::gLegacyMouseState.valueD68DB4) = 212;
    }
    if (!encodingFailed) { reportedFailure = false; reportedContext = nullptr; reportedText.clear(); }
    script::FinishLuaWindowFrame();
    return 1;

}

// 原函数：0x00504120。
std::int32_t WindowRuntime_Render()
{
    using namespace detail;
    if (gSkillRebirthModeEnabled_5F9754 != 0 && world::gPlayerRuntimeSlots.timerRemaining0C <= 0) {
        const std::int32_t y = static_cast<std::int32_t>(world::gPlayerY);
        const std::int32_t x = static_cast<std::int32_t>(world::gPlayerX);
        static_cast<void>(network::SendWorldActionRequest_511DD0(platform::gConnectionSocket, x, y, 62));
        auto* const occupancy = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
        const auto decreased = static_cast<std::uint8_t>(occupancy[7] - 1);
        gSkillRebirthModeEnabled_5F9754 = 0;
        occupancy[7] = decreased;
        WindowSkill_SyncStates();
    }
    return WindowRuntime_Update();
}

// WPL01：七类载荷创建/owner与动画现场源槽，见doc/cg2d_window_payload_creation_spec.md。
// 原函数：0x00471440。
WindowInstance* WindowRuntime_Open(const std::int32_t id)
{
    using namespace detail;
    const std::int8_t category = kWindowCategories[id];
    if (category == 1) {
        WindowRuntime_CloseByCategory(2);
    } else if (category == 2) {
        WindowRuntime_CloseByCategory(3);
    }
    for (std::int32_t index = 0; index < static_cast<std::int32_t>(kWindowCount); ++index) {
        WindowInstance* const existing = gWindows[index];
        if (existing != nullptr && existing->id == id) {
            gWindows[index]->state |= 0x2000u;
            static_cast<void>(CenterPopupWindowPosition(id,
                existing->width, existing->height, existing->x, existing->y));
            return gWindows[index];
        }
    }
    std::int32_t freeSlot = 0;
    while (freeSlot < static_cast<std::int32_t>(kWindowCount) && gWindows[freeSlot] != nullptr) {
        ++freeSlot;
    }
    if (freeSlot == static_cast<std::int32_t>(kWindowCount)) {
        return nullptr;
    }
    auto* const window = static_cast<WindowInstance*>(std::calloc(1, sizeof(WindowInstance)));
    if (window == nullptr) {
        return nullptr;
    }
    WindowPrototype* const prototype = gWindowPrototypes57C130[id];
    window->id = id;
    window->state = prototype->state;
    window->layer = prototype->layer;
    window->width = prototype->width;
    window->height = prototype->height;
    window->flags = prototype->flags;
    window->update = prototype->update;
    window->draw = prototype->draw;
    window->release = prototype->release;
    const std::int32_t count = prototype->elementCount;
    const std::int32_t pageCount = WindowInventory_GetPageElementCount(id);
    window->elementCount = count + pageCount;
    if (window->elementCount != 0) {
        window->elements = static_cast<WindowElement*>(
            std::calloc(static_cast<std::size_t>(window->elementCount), sizeof(WindowElement)));
        if (window->elements == nullptr) {
            WindowRuntime_Free(window);
            return nullptr;
        }
        WindowElementPrototype* source = prototype->elements;
        for (std::int32_t index = 0; index < count; ++index, ++source) {
            WindowElement* const element = &window->elements[index];
            element->type = source->type;
            element->state = 0;
            element->x = source->x;
            element->y = source->y;
            element->width = source->width;
            element->height = source->height;
            element->visible = source->visible;
            switch (element->type) {
            case WindowElementType_Graphic: {
                auto* const data = static_cast<WindowGraphicData*>(
                    std::calloc(1, sizeof(WindowGraphicData)));
                if (data != nullptr) {
                    const WindowGraphicData* const original = source->source.graphic;
                    data->imageNo = original->imageNo;
                    data->color = original->color;
                    data->offsetX = original->offsetX;
                    data->offsetY = original->offsetY;
                    data->width = original->width;
                    data->height = original->height;
                }
                element->data = data;
                break;
            }
            case WindowElementType_Animation: {
                auto* const data = static_cast<WindowAnimationData*>(
                    std::calloc(1, sizeof(WindowAnimationData)));
                if (data != nullptr) {
                    cg::yobi::YobiAction* const action = cg::yobi::NewYobiAction_404AE0(2, 0);
                    data->action = action;
                    cg::yobi::ConfigureWindowElementAction(
                        action, data->action, source->source.animationImageNo);
                    data->value04 = 0;
                }
                element->data = data;
                break;
            }
            case WindowElementType_Text: {
                auto* const data = new (std::nothrow) WindowTextData{};
                if (data != nullptr) {
                    const WindowTextPrototype* const original = source->source.text;
                    data->text = original->text;
                    std::wstring text;
                    WindowInput_FormatSkillSlotCount(text);
                    if (data->text == L"10/10") { data->text = text; }
                    data->style.color = original->style.color;
                    data->style.font = original->style.font;
                }
                element->data = data;
                break;
            }
            case WindowElementType_MouseArea: {
                auto* const data = static_cast<WindowMouseData*>(
                    std::calloc(1, sizeof(WindowMouseData)));
                if (data != nullptr) {
                    data->flags = *source->source.mouseFlags;
                }
                element->data = data;
                break;
            }
            case WindowElementType_Input:
                element->data = std::calloc(1, sizeof(WindowInputData));
                break;
            case WindowElementType_InputCaret:
                element->data = std::calloc(1, sizeof(std::uint32_t));
                break;
            case WindowElementType_Number: {
                auto* const data = static_cast<WindowNumberData*>(
                    std::calloc(1, sizeof(WindowNumberData)));
                if (data != nullptr) {
                    const WindowNumberData* const original = source->source.number;
                    data->text = original->text;
                    std::memcpy(
                        reinterpret_cast<unsigned char*>(data) + offsetof(WindowNumberData, color),
                        reinterpret_cast<const unsigned char*>(original) + offsetof(WindowNumberData, color),
                        sizeof(std::uint32_t));
                }
                element->data = data;
                break;
            }
            default:
                break;
            }
            const WindowElementCallback callback = source->mouseCallback;
            element->mouseCallback = callback != nullptr ? callback : &WindowElement_NoOp;
        }
    }
    if (!WindowInventory_CreatePageElements(window, count)) {
        WindowRuntime_Free(window);
        return nullptr;
    }
    WindowDragPrototype* drag = gWindowDragPrototypes57C24C[id];
    const std::int32_t dragCount = drag->count;
    if (dragCount > 0) {
        window->secondaryCount = dragCount;
        drag = gWindowDragPrototypes57C24C[id];
        for (std::int32_t index = 0; index < drag->count; ++index) {
            window->dragRects40[index].x = drag->rectangles[index].x;
            drag = gWindowDragPrototypes57C24C[id];
            window->dragRects40[index].y = drag->rectangles[index].y;
            drag = gWindowDragPrototypes57C24C[id];
            window->dragRects40[index].width = drag->rectangles[index].width;
            drag = gWindowDragPrototypes57C24C[id];
            window->dragRects40[index].height = drag->rectangles[index].height;
            drag = gWindowDragPrototypes57C24C[id];
        }
    }
    gWindows[freeSlot] = window;
    window->state |= 0x2000u;
    window->x = gWindowStates[id].x;
    window->y = gWindowStates[id].y;
    // 分页区在下方发布；初始化控件前即使用最终高度确定弹窗位置。
    const std::int32_t layoutHeight = window->height
        + ((id == WindowId_Inventory || id == WindowId_BattleItems || id == WindowId_Craft) ? 30 : 0);
    static_cast<void>(CenterPopupWindowPosition(id,
        window->width, layoutHeight, window->x, window->y));
    const std::uint32_t flags = gWindowStates[id].flags;
    WindowInstance* const previous = gCurrentWindow;
    gWindowStates[id].flags = flags | 0x80u;
    gCurrentWindow = window;
    window->elementCount = count;
    static_cast<void>(window->update(-1));
    gWindowStates[id].window = window;
    window->elementCount = count + pageCount;
    switch (id) {
    case WindowId_Inventory:
    case WindowId_BattleItems:
    case WindowId_Craft:
        window->height += 30;
        static_cast<void>(window->update(-1));
        break;
    case WindowId_Bank:
        for (std::int32_t index = 3; index <= 6; ++index) {
            window->elements[index].visible = 0;
        }
        break;
    default:
        break;
    }
    // 初始化会发布实际尺寸；仅符合原型中心规则的窗口按最终几何居中。
    static_cast<void>(CenterPopupWindowPosition(id,
        window->width, window->height, window->x, window->y));
    gCurrentWindow = previous;
    return window;
}

// 原函数：0x00475AA0。
cg::yobi::YobiAction* WindowRuntime_CreateAnimation(
    const std::int32_t id, const std::uint32_t flags, const std::uint32_t mode,
    cg::yobi::YobiAction** const completionSlot)
{
    using namespace detail;
    const WindowPrototype* const prototype = gWindowPrototypes57C130[id];
    const std::int32_t width = prototype->width;
    const std::int32_t height = prototype->height;
    if (id == 10 || id == 11 || id == 12 || id == 14 || id == 27) {
        const std::int32_t offsetX = platform::RandomRange_46FAF0(-15, 15);
        gWindowStates[id].x += offsetX;
        const std::int32_t offsetY = platform::RandomRange_46FAF0(-15, 15);
        gWindowStates[id].y += offsetY;
        if (gWindowStates[id].x < 0) {
            gWindowStates[id].x = platform::RandomRange_46FAF0(10, 20);
        }
        if (gWindowStates[id].y < 0) {
            gWindowStates[id].y = platform::RandomRange_46FAF0(10, 20);
        }
        if (gWindowStates[id].x + width > platform::gWindowWidth) {
            gWindowStates[id].x = platform::RandomRange_46FAF0(platform::gWindowWidth - 20 - width, platform::gWindowWidth - width);
        }
        if (gWindowStates[id].y + height > platform::gWindowHeight) {
            gWindowStates[id].y = platform::RandomRange_46FAF0(platform::gWindowHeight - 20 - height, platform::gWindowHeight - height);
        }
    }
    WindowInstance* const window = gWindowStates[id].window;
    std::int32_t x;
    std::int32_t y;
    if (window != nullptr) {
        window->state |= 0x8000u;
        WindowInstance* const current = gWindowStates[id].window;
        x = current->x;
        y = current->y;
    } else {
        x = gWindowStates[id].x;
        y = gWindowStates[id].y;
    }
    // 展开矩形计入实际分页区；位置由Sub_475950与创建入口共用的规则确定。
    const std::int32_t openingHeight = height
        + ((id == WindowId_Inventory || id == WindowId_BattleItems || id == WindowId_Craft) ? 30 : 0);
    return cg::yobi::CreateWindowOpeningAnimation_475950(x, y, width, openingHeight, id, flags, mode, completionSlot);
}

// 原函数：0x00477160。
void WindowNumber_InitializeLegacySpacing()
{
    WindowNumber_InitializeSpacing();
}

// WND01：477160尾跳的四S32步进生产，见doc/cg2d_window_number_draw_spec.md。
// 原函数：0x00477170。
void WindowNumber_InitializeSpacing()
{
    using namespace detail;
    const std::int32_t first = kNumberSpacingDeductions54F2A8[0];
    const std::int32_t second = kNumberSpacingDeductions54F2A8[1];
    const std::int32_t third = kNumberSpacingDeductions54F2A8[2];
    gNumberAdvancesD67AC4[0] = first + 5;
    const std::int32_t fourth = kNumberSpacingDeductions54F2A8[3];
    gNumberAdvancesD67AC4[1] = second + 10;
    gNumberAdvancesD67AC4[2] = third + 11;
    gNumberAdvancesD67AC4[3] = fourth + 7;
}

std::uint8_t WindowRuntime_ReadUiGate() noexcept
{
    return detail::gOwnStallOpenForBusiness_D67BE0;
}

std::uint8_t WindowRuntime_ReadObservationFlag() noexcept
{
    return detail::gSkillRebirthModeEnabled_5F9754;
}

// 原数据：0x00584DA4。
std::int32_t WindowItemDrag_ReadIndex() noexcept
{
    return detail::gDragItemIndex584DA4;
}

// DLP01：非零phase才取消，可选message1/4后重取phase；八槽清理末按该样本进入2/6。
// 原函数：0x00476410。
void WindowDrag_Cancel()
{
    using namespace detail;
    std::int32_t phase = gMousePhase57C128;
    if (phase == 0) {
        return;
    }
    const WindowDragCallback callback = gDragCallback57C118;
    if (callback != nullptr) {
        callback(phase == 1 ? 1 : 4, 0, gDragSourceIndex57C114);
        phase = gMousePhase57C128;
    }
    gMouseValue57C110 = 0;
    gDragSourceIndex57C114 = 0;
    gDragCallback57C118 = nullptr;
    gDragX57C11C = 0;
    gDragY57C11E = 0;
    gFocusWindowId57C120 = 0;
    gFocusElementIndex57C124 = -1;
    gMousePhase57C128 = phase == 1 ? 2 : 6;
}

// 原函数：0x00471270。
void WindowRuntime_Free(WindowInstance* const window)
{
    using namespace detail;
    script::NativeLuaWindowClosed(window->id);
    window->release();
    gCurrentWindowState = &gWindowStates[window->id];
    gCurrentWindowState->x = window->x;
    gCurrentWindowState->y = window->y;
    gCurrentWindowState->flags &= ~std::uint32_t{0x80};
    gCurrentWindowState->flags |= 1u;
    gCurrentWindowState->window = nullptr;
    const std::int32_t focusWindowId = gFocusWindowId57C120;
    const std::int32_t windowId = window->id;
    if (focusWindowId == windowId) {
        WindowDrag_Cancel();
    }
    for (std::int32_t index = 0; index < window->elementCount; ++index) {
        WindowElement* const element = &window->elements[index];
        switch (element->type) {
        case WindowElementType_Graphic:
        case WindowElementType_Input: {
            void* const focusedData = gFocusedElementDataD45F94;
            void* const elementData = element->data;
            if (elementData == focusedData) {
                static_cast<void>(WindowChat_RestoreInput());
            }
            if (void* const data = window->elements[index].data; data != nullptr) {
                std::free(data);
                window->elements[index].data = nullptr;
            }
            break;
        }
        case WindowElementType_Animation: {
            const auto* const data = static_cast<const WindowAnimationData*>(element->data);
            if (data != nullptr) {
                cg::yobi::MarkDeathUnchecked(data->action);
                std::free(window->elements[index].data);
                window->elements[index].data = nullptr;
            }
            break;
        }
        case WindowElementType_Text: {
            auto* const data = static_cast<WindowTextData*>(element->data);
            delete data; // 原0x0047134F：UTL02正文随原生析构释放。
            window->elements[index].data = nullptr;
            break;
        }
        case WindowElementType_MouseArea:
        case WindowElementType_InputCaret:
        case WindowElementType_Number: {
            void* const data = element->data;
            if (data != nullptr) {
                std::free(data);
                window->elements[index].data = nullptr;
            }
            break;
        }
        default:
            break;
        }
    }
    WindowElement* const elementsToFree = window->elements;
    if (elementsToFree != nullptr) {
        std::free(elementsToFree);
    }
    window->elements = nullptr;
    std::free(window);
}

// WCR01：发布及bit1写沿同一状态记录地址，清槽由既有472190调用父负责。
// 原函数：0x00472160。
void WindowRuntime_Close(WindowInstance* const window)
{
    detail::WindowState* const state = &detail::gWindowStates[window->id];
    detail::gCurrentWindowState = state;
    state->flags |= 2u;
    WindowRuntime_Free(window);
}

// 原函数：0x00472190。
std::int32_t WindowRuntime_CloseAll()
{
    using namespace detail;
    for (std::size_t index = 0; index < gWindows.size(); ++index) {
        WindowInstance* const window = gWindows[index];
        if (window != nullptr) {
            WindowRuntime_Close(window);
            gWindows[index] = nullptr;
        }
    }
    world::gInventorySlotUseCounts_D67B08.fill(0);
    if (gSkillRebirthModeEnabled_5F9754 == 0) {
        return 0;
    }
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    const std::uintptr_t socket = platform::gConnectionSocket;
    const std::int32_t result = network::SendWorldActionRequest_511DD0(socket, x, y, 62);
    gSkillRebirthModeEnabled_5F9754 = 0;
    return result;
}

// 原函数：0x00503E80。
void WindowRuntime_ProcessScene()
{
    using namespace detail;
    switch (world::gCurrentSceneType) {
    case 5:
        WindowRuntime_ResetShortcutFlag();
        WindowRuntime_InitializeLogin();
        WindowRuntime_OpenLoginScene();
        return;
    case 7:
    case 11:
        WindowRuntime_MarkExistingWindows();
        static_cast<void>(WindowRuntime_CloseAll());
        static_cast<void>(WindowRuntime_SaveSettings());
        world::SaveAlbumData_5186A0();
        static_cast<void>(cg::world::detail::SaveContactMailFile_519300());
        static_cast<void>(cg::world::detail::SaveGuildMailFile_519550());
        static_cast<void>(world::detail::SavePlayerProfilesFile_534770());
        static_cast<void>(world::detail::SaveProfileCategoryFile_5348A0());
        return;
    case 9:
        switch (world::gCurrentSceneState) {
        case 1:
            WindowRuntime_MarkSceneWindows();
            static_cast<void>(WindowRuntime_CloseAll());
            WindowRuntime_OpenNormalScene();
            return;
        case 4:
        case 200:
            WindowRuntime_MarkExistingWindows();
            static_cast<void>(WindowRuntime_CloseAll());
            return;
        case 204:
            WindowRuntime_OpenNormalScene();
            return;
        case 250:
        case 252:
            WindowRuntime_MarkExistingWindows();
            static_cast<void>(WindowRuntime_CloseAll());
            WindowRuntime_OpenNormalScene();
            if (world::gCurrentSceneState == 250) {
                auto* const bytes = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
                for (std::size_t index = 0; index < 8; ++index) {
                    bytes[index] = static_cast<unsigned char>(bytes[index] + 1);
                }
            }
            return;
        default:
            return;
        }
    case 10:
        switch (world::gCurrentSceneState) {
        case 1:
            WindowRuntime_OpenBattleScene();
            return;
        case 8:
            WindowRuntime_MarkSceneWindows();
            static_cast<void>(WindowRuntime_CloseAll());
            return;
        default:
            return;
        }
    default:
        return;
    }
}

// 原函数：0x00472240。
void WindowRuntime_CloseByCategory(const std::uint32_t mask)
{
    using namespace detail;
    for (std::size_t index = 0; index < gWindows.size(); ++index) {
        WindowInstance* const window = gWindows[index];
        if (window == nullptr) {
            continue;
        }
        const std::int32_t id = window->id;
        const auto category = static_cast<std::uint32_t>(
            static_cast<std::int32_t>(kWindowCategories[id]));
        if ((mask & category) != 0) {
            gCurrentWindowState = &gWindowStates[id];
            gCurrentWindowState->flags |= 2u;
            WindowRuntime_Free(gWindows[index]);
            gWindows[index] = nullptr;
        }
    }
}

// WDP01：完整元素分发、低W16裁剪与现场取样；见doc/cg2d_window_element_draw_pipeline_spec.md。
// 原函数：0x004729B0。
void WindowRuntime_DrawCurrent()
{
    using namespace detail;
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* window = gCurrentWindow;
    // G07：只调整绘制访问顺序，原控件和借用指针不随排序移动。
    const std::int32_t originalCount = window->id >= 0 && window->id < kWindowCount
        ? gWindowPrototypes57C130[window->id]->elementCount : window->elementCount;
    const std::int32_t pageCount = window->elementCount - originalCount;
    const std::int32_t prefixCount = window->id == WindowId_Bank ? 2 : 0;
    for (std::int32_t drawIndex = 0; drawIndex < window->elementCount; ++drawIndex) {
        std::int32_t index = drawIndex;
        if (pageCount > 0 && drawIndex >= prefixCount) {
            index = drawIndex < prefixCount + pageCount
                ? originalCount + drawIndex - prefixCount : drawIndex - pageCount;
        }
        WindowElement* const element = &window->elements[index];
        if (element->visible == 1) {
            const std::int32_t elementX = element->x;
            const std::int32_t windowX = window->x;
            const std::int32_t windowY = window->y;
            const std::int32_t x = windowX + elementX;
            const std::int32_t elementY = element->y;
            const std::int32_t y = windowY + elementY;
            switch (element->type) {
            case WindowElementType_Graphic: {
                const auto* const data = static_cast<const WindowGraphicData*>(element->data);
                const std::int32_t entryImageNo = data->imageNo;
                if (entryImageNo == 0) {
                    break;
                }
                std::array<std::uint16_t, 4> rectangle {};
                static_assert(sizeof(rectangle) == 8);
                std::uint8_t cropped = 0;
                if (data->offsetX != 0 || data->offsetY != 0
                    || data->width != 0 || data->height != 0) {
                    std::int32_t graphicIndex;
                    resource::GetGraphicMapNoIndex(entryImageNo, &graphicIndex);
                    std::int16_t graphicX;
                    std::int16_t graphicY;
                    resource::GetGraphicXY(
                        static_cast<std::uint32_t>(graphicIndex), &graphicX, &graphicY);
                    std::uint16_t offsetX;
                    std::memcpy(&offsetX, &data->offsetX, sizeof(offsetX));
                    rectangle[0] = static_cast<std::uint16_t>(offsetX + graphicX);
                    std::uint16_t offsetY;
                    std::memcpy(&offsetY, &data->offsetY, sizeof(offsetY));
                    rectangle[1] = static_cast<std::uint16_t>(offsetY + graphicY);
                    std::uint16_t cropWidth;
                    std::memcpy(&cropWidth, &data->width, sizeof(cropWidth));
                    rectangle[2] = cropWidth;
                    std::uint16_t cropHeight;
                    std::memcpy(&cropHeight, &data->height, sizeof(cropHeight));
                    rectangle[3] = cropHeight;
                    cropped = 1;
                }
                descriptor.sourceX = std::bit_cast<std::int16_t>(rectangle[0]);
                descriptor.sourceY = std::bit_cast<std::int16_t>(rectangle[1]);
                descriptor.sourceWidth = std::bit_cast<std::int16_t>(rectangle[2]);
                descriptor.sourceHeight = std::bit_cast<std::int16_t>(rectangle[3]);
                descriptor.color = data->color;
                descriptor.clipMode = cropped;
                descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
                descriptor.paletteIndex = 0;
                std::int32_t imageNo;
                std::int32_t drawY;
                std::int32_t drawX;
                if (cropped == 0) {
                    WindowInstance* const drawWindow = gCurrentWindow;
                    imageNo = data->imageNo;
                    const std::int32_t drawElementY = element->y;
                    const std::int32_t drawWindowY = drawWindow->y;
                    const std::int32_t drawElementX = element->x;
                    const std::int32_t drawWindowX = drawWindow->x;
                    drawY = drawElementY + drawWindowY;
                    drawX = drawElementX + drawWindowX;
                } else {
                    imageNo = data->imageNo;
                    const std::int32_t drawElementY = element->y;
                    const std::int32_t drawElementX = element->x;
                    WindowInstance* const drawWindow = gCurrentWindow;
                    const std::int32_t drawWindowY = drawWindow->y;
                    drawY = drawWindowY + drawElementY;
                    const std::int32_t drawWindowX = drawWindow->x;
                    drawX = drawWindowX + drawElementX;
                }
                static_cast<void>(render::DrawGraphic3_520970(
                    drawX, drawY, 0xDF, imageNo, 0, 1, 0, &descriptor));
                break;
            }
            case WindowElementType_Animation: {
                const auto* const data = static_cast<const WindowAnimationData*>(element->data);
                const std::uint32_t stop = data->value04;
                cg::yobi::YobiAction* const action = data->action;
                static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(action, 100, stop != 0, 0));
                const std::int32_t graphicIndex = cg::yobi::GetYobiActionField20(action);
                if (graphicIndex == 0) {
                    break;
                }
                const render::GraphicDescriptor16* const mode =
                    cg::yobi::ApplyWindowAnimationDrawMode(action);
                const std::int32_t offsetY = cg::yobi::GetYobiGraphicOriginOffset_5182B0(action, 1);
                WindowInstance* const verticalWindow = gCurrentWindow;
                const std::int32_t animationElementY = element->y;
                const std::int32_t drawY = offsetY + verticalWindow->y + animationElementY;
                const std::int32_t offsetX = cg::yobi::GetYobiGraphicOriginOffset_5182B0(action, 0);
                WindowInstance* const horizontalWindow = gCurrentWindow;
                const std::int32_t animationElementX = element->x;
                const std::int32_t drawX = offsetX + horizontalWindow->x + animationElementX;
                static_cast<void>(render::DrawGraphic2_521DD0(
                    drawX, drawY, 0xDF, graphicIndex, 0, 0, mode));
                break;
            }
            case WindowElementType_Text: {
                const auto* const data = static_cast<const WindowTextData*>(element->data);
                const std::wstring& text = data->text;

                    const std::int32_t drawY = y - element->height;
                    const auto color = std::bit_cast<std::int8_t>(
                        std::to_integer<std::uint8_t>(data->style.color));
                    const auto font = std::bit_cast<std::int8_t>(
                        std::to_integer<std::uint8_t>(data->style.font));
                    const std::int32_t drawX = x - element->width;
                    static_cast<void>(render::DrawText2_456670(
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                        3, font, color, text, 0));

                break;
            }
            case WindowElementType_Input: {
                const auto* const data = static_cast<const WindowInputData*>(element->data);
                if (data == gFocusedElementDataD45F94) {
                    if (input::gCurrentInputData == &cg::ui::detail::gInputContext626AA0) {
                        if (input::gChatPanelVisible_C9A96C == 1) {
                            WindowChat_DrawInput(data->context);
                        }
                    } else if (data != nullptr) {
                        WindowChat_DrawFixedWidthInput(data->context);
                    }
                }
                break;
            }
            case WindowElementType_InputCaret:
                WindowChat_DrawHistory(x + 5, y);
                break;
            case WindowElementType_Number: {
                const auto* const data = static_cast<const WindowNumberData*>(element->data);
                if (data != nullptr) {
                    const std::uint8_t flags = data->flags;
                    const std::int8_t color = std::bit_cast<std::int8_t>(data->color);
                    const std::int8_t style = data->style;
                    const std::wstring& text = *data->text;
                    WindowNumber_Draw(x, y, text, style, color, flags, 0xDF);
                }
                break;
            }
            default:
                break;
            }
        }
        window = gCurrentWindow;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    world::DrawChatRangeOverlay_476680();
}

// 原代码位置：0x00440ADB。
void WindowRuntime_ApplyObservationInputGate()
{
    auto* const bytes = reinterpret_cast<unsigned char*>(world::gInventorySlotUseCounts_D67B08.data());
    if (detail::gSkillRebirthModeEnabled_5F9754 != 0) {
        const unsigned char previous = bytes[7];
        detail::gSkillRebirthModeEnabled_5F9754 = 0;
        bytes[7] = static_cast<unsigned char>(previous - 1);
    }
    for (std::size_t index = 0; index < 8; ++index) {
        bytes[index] = static_cast<unsigned char>(bytes[index] + 1);
    }
}

int WindowDrag_ReadMousePhase() { return detail::gMousePhase57C128; }

void WindowDrag_ConsumeDrop()
{
    if (detail::gMousePhase57C128 != 4) return;
    if (detail::gDragCallback57C118) {
        detail::gDragCallback57C118(4, 0, detail::gDragSourceIndex57C114);
    }
    static_cast<void>(detail::WindowDrag_ResetMouse());
}
} // namespace cg::ui
