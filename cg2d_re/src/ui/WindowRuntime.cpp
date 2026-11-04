#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "../scene/login/LoginSavedState.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "WindowRuntimeInternal.hpp"
#include "WindowComponents.hpp"
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
#include <algorithm>
#include <cerrno>
#include <filesystem>

namespace cg::ui {

WindowElementLink& WindowElementLink::operator=(WindowElement* const element)
{
    windowId = element && element->owner ? element->owner->id : -1;
    elementId = element ? element->elementId : 0;
    ownerSession = element && element->owner ? element->owner->ownerSession : nullptr;
    return *this;
}

WindowElement* WindowElementLink::get() const
{
    auto* window = ownerSession ? WindowRuntime_FindInstance(*ownerSession, windowId) : nullptr;
    return window ? window->FindElementById(elementId) : nullptr;
}

namespace detail {

// 原第二原型的内联矩形以借用T*表达；不复制共享表或原矩形数据。
struct WindowDragPrototype {
    std::int32_t count;
    WindowDragRect* rectangles;
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
static void WindowItemSelector_ShowPetPage(session::ClientSession& session, WindowItemSelectorState* state); // SPG01：宠物页/名称负重及原动态空位显示保持；只判低AL，不钳制itemSlots或补保护。

// 原函数：0x004C6500。
static void WindowItemSelector_HidePageElements(WindowItemSelectorState* state); // SPG01：按原顺序隐藏槽6/7/8/15/16、六装备槽与17；模式0才隐藏19，主元素不改。

// 原函数：0x004C6550。
static void WindowItemSelector_SelectPage(session::ClientSession& session, WindowItemSelectorState* state, std::int32_t page); // SPG01：先发布页再隐藏/新页分发；CC40D4仅低BYTE、四回调先取元素；原五页和写序保持，全机器ABI待。

// 原函数：0x004C6790。
static void WindowItemSelector_UpdatePage(WindowInstance* callbackWindow, WindowItemSelectorState* state); // SFD01：原四页分发/业务钳制保持；两按钮按原顺序取样，共借descriptor[0]；已知上行由窗口回调返回1。

// 原函数：0x004C6990。
static std::int32_t WindowItemSelector_SelectPet(session::ClientSession& session, WindowItemSelectorState* state); // PSP01：同state mode按S8映射，回调返回的S32索引原序发布到selectedPet70并返回。

// 原函数：0x004C69B0。
static void WindowItemSelector_RefreshPetHeader(session::ClientSession& session, WindowItemSelectorState* state); // PSP01：同宠物previous/updated BYTE及名字/负重原序保持；首次测宽先字体后文本，负重先文本后字体。

// 原函数：0x004C6AC0。
static void WindowItemSelector_MoveElements(WindowItemSelectorState* state, std::int32_t x); // SPG01：依原序移动主元素、四按钮及六表坐标；原S32结果与固定循环保持，不新增钳制。

// SLC01：两轴先窗口再元素取值，flags/淡入计数只读原BYTE；void不证明原EAX上行等价。
// 原函数：0x004C6B10。
static void WindowItemSelector_DrawTooltip(WindowInstance* callbackWindow, WindowItemSelectorState* state); // 消费本帧hover，宠物物品固定借CF3B10索引。

// 原函数：0x004C6D90。
static std::int32_t WindowItemSelector_DrawPreview(WindowInstance* callbackWindow, WindowItemSelectorState* state); // PSP01：同一非拥有预览推进/两轴/队列；掩码替换字节先旧字节，返回/原读取门槛保持。

// 原函数：0x004C71C0。
static std::int32_t WindowItemSelector_FindPetIndex(session::ClientSession& session, std::int32_t ordinal); // PSP01：五排序对按used W16/索引S16/状态W16计数；不足仍取末扫描项，不新增匹配/索引保护。

// 原函数：0x004C7340。
static bool WindowItemSelector_SetMode(session::ClientSession& session, WindowItemSelectorState* state, std::int32_t mode); // SPG01：写mode低BYTE后持旧页；页3按新mode切2/3、页2不切；bool仅承接原AL=1，高位ABI待。

// 原函数：0x004885A0。
static std::int32_t WindowItem_RequestTransferQuantity(session::ClientSession& session, std::int32_t amount); // IMT01：source7短路/重读、41优先38及转义后状态-Y-X取序保持，原S32返回，全callback/ABI待。

// 原函数：0x0048D540。
static std::int32_t WindowPet_DrawTooltip(
    session::ClientSession& session,
    std::int16_t x, std::int16_t y, std::int32_t index,
    std::uint8_t textLayer, std::uint8_t graphicLayer, std::uint32_t color);

// 原函数：0x0048BF30。
static void WindowPetDrag_Handle(session::ClientSession& session, std::int32_t message, std::int32_t unused, std::int32_t sourceIndex);

// 原函数：0x0048BC90。
static void WindowItemDrag_Handle(session::ClientSession& session, std::int32_t message, std::int32_t unused, std::int32_t sourceIndex); // IDE01：物品回调，两fresh分类/两点击BYTE及descriptor持址，reset/取消后原继续，完整分发及运行待。

// 原函数：0x004740F0。
static void WindowInput_FormatSkillSlotCount(session::ClientSession& session, std::wstring& destination); // 前10项技能索引的S16槽数，直接宽格式化。

// 原函数：0x00472F00。
static std::int32_t WindowRuntime_HitTestRect(
    session::ClientSession& session,
    std::int32_t left, std::int32_t top, std::int32_t width, std::int32_t height);

// 原函数：0x00473560。
static int WindowRuntime_DispatchElementMouseAt(WindowInstance& target, ElementId elementId, int ox, int oy, const UIClipRect* clip); // 共用原生事件分发，父容器只提供坐标与裁剪。

// 原函数：0x00472D20。
static std::int32_t WindowDrag_Update(WindowInstance& window); // WDF01：完整拖动/焦点两位结果，原宽度和现场取样见SPEC。

// 原函数：0x004730A0。
static std::int32_t WindowRuntime_CompactAndReorder(session::ClientSession& session); // WCR01：71槽压紧/关闭/前插，两个标记判断用同一W32 state。

// 原函数：0x00473E40。
static void WindowRuntime_AdvanceMousePhase(session::ClientSession& session); // 每轮窗口处理前的鼠标阶段推进。

// 原函数：0x00503CD0。
static std::int32_t WindowRuntime_IncrementNameMode(session::ClientSession& session); // 名称模式S16递增，保存标量后持久化。

// 原函数：0x004C9240。
static std::int32_t WindowRuntime_ResetSettings(session::ClientSession& session); // 原型默认坐标和设置默认值，不清窗口实例。 // CWP01：见聊天几何生产完整合同。

// 原函数：0x004C93A0。
static std::int32_t WindowRuntime_HasChangedDefaultPositions(session::ClientSession& session); // 文件三处x与固定原型比较，首差异返回1。

// 原函数：0x004C93E0。
static std::int32_t WindowRuntime_RepairSettingsTail(session::ClientSession& session); // 原文件长度尾部修复，不增加索引保护。

// 原函数：0x004C9170。
static std::int32_t WindowRuntime_ApplySettings(session::ClientSession& session, bool restoreCoordinates); // 角色/尺寸文件恢复坐标，旧全局文件仍按原默认布局适配。

// 原函数：0x004C9060。
static std::int32_t WindowRuntime_LoadSettings(session::ClientSession& session); // WinSaveDate完整加载/默认回退，保留双打开与返回差别。

// 原函数：0x004C92B0。
static std::int32_t WindowRuntime_ClearSavedSettings(session::ClientSession& session); // 仅清521CH设置文件区。

// 原函数：0x004C92D0。
static std::uint32_t WindowRuntime_CaptureSettings(session::ClientSession& session); // 原序捕获窗口设置，返回窗口33的marker快照。

// 原函数：0x00472200。
static void WindowRuntime_ClearSavedWindowMarkers(session::ClientSession& session); // 仅清71个marker50，不释放窗口。

// 原函数：0x00472220。
static void WindowRuntime_MarkExistingWindows(session::ClientSession& session); // 只将现存窗口marker写1，空窗口保留旧值。

// 原函数：0x004722A0。
static void WindowRuntime_ResetMouseAndItemFlags(session::ClientSession& session); // 完整鼠标/拖动重置，先清D67B08七W32。

// 原函数：0x004722E0。
static void WindowRuntime_ClearElementFocus(session::ClientSession& session); // 只清异型元素焦点引用。

// 原函数：0x004722F0。
static void WindowRuntime_RestoreMarkedWindows(session::ClientSession& session); // marker严格1才分派，四个特殊窗口按原序恢复。

// 原函数：0x00479640。
static void WindowRuntime_ResetOperationCount(session::ClientSession& session); // 只清窗口操作S32计数。

// 原函数：0x004796C0。
static void WindowRuntime_ResetGatherOperationCount(session::ClientSession& session); // 只清D67AFC。

// 原函数：0x004796F0。
static void WindowRuntime_ResetSkillOperationCount(session::ClientSession& session); // 只清D67AFD。

// 原函数：0x00479610。
static void WindowRuntime_ResetOperationReferences(session::ClientSession& session); // 清两借用Yobi槽与计数/三个字节，不释放对象。

// 原函数：0x004C7AD0。
static void WindowRuntime_ResetShortcutFlag(session::ClientSession& session); // 只清CF4AAC。

// 原函数：0x00475C10。
static void WindowRuntime_RestorePositions(session::ClientSession& session); // 现存窗口恢复原型坐标，仅五类追加原序随机/边界处理。

// 原函数：0x004C7AE0。
static void WindowRuntime_ProcessShortcuts(session::ClientSession& session); // 两模式完整快捷键，Ctrl优先；唯一caller弃用原返回。

// 原函数：0x00470980。
static void WindowRuntime_OpenNormalScene(session::ClientSession& session); // 普通场景完整重开，保持626C14与22/23门禁。

// 原函数：0x004923D0。
static std::int32_t WindowRuntime_ResetAlbumState(session::ClientSession& session); // 三个UI数值依序写5/0/0。

// 原函数：0x004742B0。
static std::int32_t WindowScroll_ResetPosition(session::ClientSession& session); // 只清CF4E50/54。

// 原函数：0x00470600。
static void WindowRuntime_InitializeLogin(session::ClientSession& session); // 每次登录的完整UI/邮件/profile初始化；不清全部窗口指针。

// 控件载荷、窗口状态、组件表与原型。

static_assert(offsetof(WindowDragPrototype, rectangles) == 8);

static_assert(sizeof(WindowDragPrototype) == 16);

std::array<WindowInstance*, kWindowCount>& WindowRuntime_Windows(session::ClientSession& session)
{
    return session.UiFrame().native.windows;
}

std::array<WindowState, kWindowCount>& WindowRuntime_States(session::ClientSession& session)
{
    return session.UiFrame().native.states;
}

WindowLayoutSettings& WindowRuntime_Settings(session::ClientSession& session)
{
    return session.UiFrame().native.settings;
}

// IWT01：存放区为含宽文本的内部对象，不再固定旧字节大小。

static_assert(0x60 + 10 * 0x8C == 0x5D8);

static_assert(sizeof(WindowListSelectionCallback) == 8);

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

static const std::array<std::int32_t, 8> gSelectorOwnX54F584 {100, 36, 36, 165, 165, 36, 165, 165}; // 原全局：0x0054F584。

static const std::array<std::int32_t, 8> gSelectorOwnY54F5A4 {30, 130, 80, 80, 130, 30, 30, 180}; // 原全局：0x0054F5A4。

const std::array<std::int32_t, 6> gSelectorPetX54F5C4 {36, 165, 36, 165, 36, 165}; // 原全局：0x0054F5C4。

const std::array<std::int32_t, 6> gSelectorPetY54F5DC {80, 80, 130, 130, 30, 30}; // 原全局：0x0054F5DC。

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

// 原54EFE8的71项S8类别及扩展窗口；仅作为新建原生窗口的category初值。
static constexpr std::array<std::int8_t, kWindowCount> kWindowCategories {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 4, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 8, 8, 8, 8, 8,
    8, 4, 4, 0, 1, 2, 2, 0, 1, 2, 4, 2, 2, 2, 2, 2,
    1, 1, 2, 4, 2, 2, 2, 1, 2, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 2, 2, 2, 4, // 大地图与小地图同为4类，入口关闭其它非默认窗口。
    1, // 72：宠物装备与其它宠物详情页同类。
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
    &gDragPrototypes[8], // 72：沿用宠物详情页的标题拖动区域。
};

// PSF01：记录不持有窗口，回调退出后按ID/代次复查；离开当前窗口调度即失效。
struct WindowPagePresentation {
    explicit WindowPagePresentation(const WindowInstance& source)
        : ownerSession(*source.ownerSession), sourceId(source.id), sourceGeneration(source.generation),
          previous(ownerSession.UiFrame().native.pagePresentation)
    { ownerSession.UiFrame().native.pagePresentation = this; }
    ~WindowPagePresentation() { ownerSession.UiFrame().native.pagePresentation = previous; }

    session::ClientSession& ownerSession;
    int sourceId;
    std::uint32_t sourceGeneration;
    int targetId = -1;
    std::uint32_t targetGeneration = 0;
    bool accepting = true; // 输入结束后停止登记，补画不会递归产生新批次。
    WindowPagePresentation* previous;
};

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
static std::int32_t WindowRuntime_AppendMouseEvents(session::ClientSession& session, std::int32_t events)
{
    const auto edge = static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session));
    if ((edge & platform::LegacyMouseEdge_LDown) != 0) {
        events |= MouseFlag_LDownEvent;
    }
    if ((edge & platform::LegacyMouseEdge_RDown) != 0) {
        events |= MouseFlag_RDownEvent;
    }
    const auto held = static_cast<std::uint8_t>(platform::ReadSessionMouseHeldButtons(session));
    if ((held & platform::LegacyMouseButton_Left) != 0) {
        events |= MouseFlag_LHoldPhysical;
    }
    if ((held & platform::LegacyMouseButton_Right) != 0) {
        events |= MouseFlag_RHoldPhysical;
    }
    if ((edge & platform::LegacyMouseEdge_LDblClick) != 0) {
        events |= MouseFlag_LDblClickEvent;
    }
    if ((edge & platform::LegacyMouseEdge_RDblClick) != 0) {
        events |= MouseFlag_RDblClickEvent;
    }
    const auto pulse = static_cast<std::uint8_t>(platform::ReadSessionMousePulseButtons(session));
    if ((pulse & platform::LegacyMouseButton_Left) != 0) {
        events |= MouseFlag_LRepeatDown;
    }
    if ((pulse & platform::LegacyMouseButton_Right) != 0) {
        events |= MouseFlag_RRepeatDown;
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
std::int32_t WindowElement_NoOp(WindowInstance& callbackWindow, const ElementId, const std::int32_t)
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
void WindowItemDrag_RemapHover(session::ClientSession& session, const std::int32_t first, const std::int32_t second)
{
    auto& inventory = session.UiFrame().native.inventory;
    if (first >= 100 || second >= 100) {
        return;
    }
    const std::int32_t selected = inventory.selectedItem;
    if (selected == first) {
        inventory.selectedItem = second;
    } else if (selected == second) {
        inventory.selectedItem = first;
    }
    if (session.WorldScene().playerItems[first].imageNo == 0x67C9
        || session.WorldScene().playerItems[second].imageNo == 0x67C9) {
        const std::int32_t previous = inventory.currentItem;
        if (previous == first) {
            inventory.currentItem = second;
        } else if (previous == second) {
            inventory.currentItem = first;
        }
    }
    WindowTrade_ResetSelection(session, first, second);
    WindowCraft_ResetSelection(session, first, second);
    WindowTrade_RemapTextListSelection(session, first, second);
    WindowSkill_AdjustSpecialHoverCount(session, first, second);
    auto* const hover = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    const unsigned char secondValue = hover[second];
    const unsigned char firstValue = hover[first];
    hover[first] = secondValue;
    hover[second] = firstValue;
}

// 原函数：0x004FB8F0。
std::int32_t WindowItem_ResetItemDrag(session::ClientSession& session)
{
    auto& interaction = session.UiFrame().native.interaction;
    interaction.dragItemIndex = -1;
    interaction.dragImageNo = 0;
    interaction.itemDragX = 0;
    interaction.itemDragY = 0;
    interaction.dragging = 0;
    interaction.hoverDescriptionEnabled = 1;
    return 0;
}

// 原函数：0x004FB920。
std::int32_t WindowItem_CancelMatchingItemDrag(session::ClientSession& session, const std::int32_t index)
{
    auto& interaction = session.UiFrame().native.interaction;
    const std::int32_t previous = interaction.dragItemIndex;
    if (previous >= 0 && previous == index) {
        return WindowItem_ResetItemDrag(session);
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
std::int32_t WindowItem_FindSortedPetIndex(session::ClientSession& session, const std::int32_t slot)
{
    for (std::int32_t position = 0; position < 5; ++position) {
        if (std::bit_cast<std::int16_t>(session.WorldScene().petPairs[position].index) == slot) {
            return position;
        }
    }
    return -1;
}

// WCL01：原474170完整15条；bit2置当前窗口bit13，hover优先返回1，否则低BYTE bit5，正确正文保持。
// 原函数：0x00474170。
std::int32_t WindowElement_HandleSideArea(WindowInstance& callbackWindow, const ElementId, const std::int32_t state)
{
    if ((state & MouseFlag_LDownEvent) != 0) {
        callbackWindow.state |= 0x2000u;
    }
    return (state & MouseFlag_Hover) != 0 ? 1 : (static_cast<std::uint8_t>(state) & MouseFlag_Dragging) != 0;
}

// IMT01：原88条，source7短路及41/38转义后状态-Y-X/真实返回；旧格式历史及正确正文保，全owner/运行待。
// 原函数：0x004885A0。
static std::int32_t WindowItem_RequestTransferQuantity(session::ClientSession& session, const std::int32_t amount)
{
    auto& transfer = session.UiFrame().native.itemTransfer;
    std::int32_t source = transfer.source;
    std::int32_t target;
    if (source == 7 || (target = transfer.target) == 7) {
        WindowCraft_ResetItemSelection(session);
        source = transfer.source;
        target = transfer.target;
    }
    if (source < 100 && target < 100) {
        return static_cast<std::int32_t>(session.Connection().MoveItemStackRequest(source, target, amount));
    }
    if (source < 8) {
        return source;
    }
    WindowState* windowState = &WindowRuntime_States(session)[WindowId_Bank];
    if (windowState->window == nullptr) {
        windowState = &WindowRuntime_States(session)[WindowId_ItemStorage];
        if (windowState->window == nullptr) {
            return source;
        }
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"I|{}|{}|{}", source, target, amount);
    const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 255);
    const auto userState = std::bit_cast<std::int32_t>(windowState->userState.value);
    const auto value14 = std::bit_cast<std::int32_t>(windowState->value14);
    const std::int32_t y = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
    const std::int32_t x = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
    return static_cast<std::int32_t>(session.Connection().SendDialogResponse(
        x, y, value14, userState, 0, escapedReply));
}

// IMT01：原97条，Ctrl BYTE/反转开关与count/used门槛、直接source先target/拆分target先source；正确正文保。
// 原函数：0x004886E0。
void WindowItem_RequestTransfer(
    session::ClientSession& session,
    const std::int32_t sourceWindow, const std::int32_t sourceElement,
    const std::int32_t source, const std::int32_t target)
{
    auto& storage = session.UiFrame().native.storage;
    auto& transfer = session.UiFrame().native.itemTransfer;
    if (source < 0 || target < 0 || source == target) {
        return;
    }
    bool split = (platform::ReadSessionKeyState(session, 0x11) & 2u) == 0;
    if (platform::gInvertItemSplitModifier_D67AE8 != 0) {
        split = !split;
    }
    std::int32_t count;
    if (source < 100) {
        count = session.WorldScene().playerItems[source].count;
    } else {
        std::memcpy(
            &count, &storage.groups[0].items[source - 100].count, sizeof(count));
    }
    std::int16_t used;
    if (target < 100) {
        used = std::bit_cast<std::int16_t>(session.WorldScene().playerItems[target].used);
    } else {
        std::memcpy(
            &used, &storage.groups[0].items[target - 100], sizeof(used));
    }
    if (source < 8 || target < 8 || count <= 0 || used != 0 || !split) {
        transfer.source = source;
        transfer.target = target;
        static_cast<void>(WindowItem_RequestTransferQuantity(session, -1));
        static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
    } else {
        transfer.target = target;
        transfer.source = source;
        static_cast<void>(cg::yobi::CreateQuantityWindowOpeningAnimation_48AC60(
            session, sourceWindow, sourceElement, 244309, count, count, 0,
            [](session::ClientSession& owner, const std::int32_t amount) { static_cast<void>(WindowItem_RequestTransferQuantity(owner, amount)); },
            1, 0, 0));
    }
}

// 原函数：0x0048BB90。
void WindowItem_RequestPetTransfer(
    session::ClientSession& session,
    const std::int32_t, const std::int32_t,
    const std::int32_t source, const std::int32_t target)
{
    if (source < 0 || target < 0 || source == target) {
        return;
    }
    if (source < 100 && target < 100) {
        static_cast<void>(session.Connection().MovePetSlotRequest(source, target));
        return;
    }
    if (WindowRuntime_States(session)[WindowId_Bank].window == nullptr) {
        return;
    }
    std::wstring buffer;
    cg::text::FormatTo(buffer, L"P|{}|{}", source, target);
    const std::wstring escapedReply = input::EscapeTextAndNotify(buffer, 255);
    const auto userState = std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_Bank].userState.value);
    const auto value14 = std::bit_cast<std::int32_t>(WindowRuntime_States(session)[WindowId_Bank].value14);
    const std::int32_t y = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
    const std::int32_t x = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
    static_cast<void>(session.Connection().SendDialogResponse(x, y, value14, userState, 0, escapedReply));
}

// 原函数：0x004EA670。
std::int32_t WindowPet_HasSkill25600(session::ClientSession& session, const std::int32_t index)
{
    return world::HasPetTechnique_52E180(session, index, 25600) != 0 ? 1 : 0;
}

// 原函数：0x0048D540。
static std::int32_t WindowPet_DrawTooltip(
    session::ClientSession& session,
    const std::int16_t x, const std::int16_t y, const std::int32_t index,
    const std::uint8_t textLayer, const std::uint8_t graphicLayer, const std::uint32_t color)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    const auto originX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 150));
    const auto originY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y - 24));
    world::PetRuntimeRecord& pet = (*session.WorldScene().petRecords)[index];
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
        const wchar_t* const name =
            !pet.name6BD.empty() ? pet.name6BD.data() : pet.name6AC.data();
        drawText(name, 8, 6);
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
        if (WindowPet_HasSkill25600(session, index) == 1) {
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
    session::ClientSession& session,
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& storage = session.UiFrame().native.storage;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (message != 0) {
        return;
    }
    const std::int32_t kind = WindowItem_ClassifyPetSlot(sourceIndex);
    if (kind == 1) {
        if ((*session.WorldScene().petRecords)[sourceIndex].used == 0) {
            static_cast<void>(WindowDrag_ResetMouse(session));
        }
    } else if (kind == 2) {
        std::uint16_t used;
        std::memcpy(&used, &storage.pets[sourceIndex - 100], sizeof(used));
        if (used == 0) {
            static_cast<void>(WindowDrag_ResetMouse(session));
        }
    }
    const std::int32_t clickMode = interaction.petDragMode;
    const auto clicked = static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session));
    if (clickMode == 0) {
        if ((clicked & platform::LegacyMouseEdge_RDown) != 0) {
            static_cast<void>(WindowDrag_ResetMouse(session));
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX)));
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY)));
            static_cast<void>(WindowDrag_StartAtPosition(session, 3, sourceIndex, x, y, WindowPetDrag_Handle));
        }
    } else {
        if ((clicked & platform::LegacyMouseEdge_LDown) != 0) {
            static_cast<void>(WindowDrag_ResetMouse(session));
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX)));
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY)));
            static_cast<void>(WindowDrag_StartAtPosition(session, 3, sourceIndex, x, y, WindowPetDrag_Handle));
        }
        if ((platform::ReadSessionMouseEdgeButtons(session) & platform::LegacyMouseEdge_RDown) != 0) {
            WindowDrag_Cancel(session);
        }
    }
    static_cast<void>(platform::RequestCursorState_506DE0(session, 1, 0));
    const std::int32_t drawMode = interaction.petDragMode;
    if (drawMode == 1) {
        const std::int32_t drawKind = WindowItem_ClassifyPetSlot(sourceIndex);
        if (drawKind == 1) {
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                (session.UiFrame().mouse.state->mouseY - 35)));
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                (session.UiFrame().mouse.state->mouseX - 120)));
            WindowBank_DrawOwnPetTooltip(session, x, y, sourceIndex, 5);
        } else if (drawKind == 2) {
            const std::int32_t slot = sourceIndex - 100;
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                (session.UiFrame().mouse.state->mouseY - 35)));
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                (session.UiFrame().mouse.state->mouseX - 120)));
            WindowBank_DrawStoredPetTooltip(session, x, y, slot, 5);
        } else {
            return;
        }
        const std::int32_t y =
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY);
        const std::int32_t x =
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX);
        static_cast<void>(cg::render::DrawGraphic(x, y, 230, 243141, 0, &descriptor));
    } else if (drawMode == 0 && WindowItem_ClassifyPetSlot(sourceIndex) == 1) {
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY)));
        static_cast<void>(WindowPet_DrawTooltip(session, x, y, sourceIndex, 5, 230, color));
    }
}

// 原函数：0x0048C130。
std::int32_t WindowPetDrag_Start(session::ClientSession& session, const std::int32_t sourceIndex, const std::int32_t mode)
{
    auto& interaction = session.UiFrame().native.interaction;
    interaction.petDragMode = mode;
    return WindowDrag_Start(session, 3, sourceIndex, WindowPetDrag_Handle);
}

// IDE01：原189条；动作及两点击BYTE/三descriptor持址明确，两fresh分类/取消后绘制保持，完整owner及运行待。
// 原函数：0x0048BC90。
static void WindowItemDrag_Handle(
    session::ClientSession& session,
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& interaction = session.UiFrame().native.interaction;
    auto& storage = session.UiFrame().native.storage;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    descriptor.color = color;
    descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
    if (message != 0) {
        if (message == 2 && interaction.itemDragMode != 0
            && (world::ReadPlayerActionFlagsByte(session) & 0x80u) == 0
            && world::IsObservationSceneTransitionActive_440B50(session) == 0) {
            const std::int32_t group =
                std::bit_cast<std::int8_t>(WindowRuntime_Settings(session).inventoryActorSelection) > 0 && sourceIndex < 6
                ? WindowItemSelector_ReadSelectedPet(&inventory.normalSelector) + 1 : 0;
            const std::int32_t y =
                static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
            const std::int32_t x =
                static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
            static_cast<void>(session.Connection().DropItemRequest(x, y, group, sourceIndex));
        }
        return;
    }
    const std::int32_t kind = WindowItem_ClassifyItemSlot(sourceIndex);
    if (kind == 1) {
        const world::PlayerItemRecord* item;
        if (std::bit_cast<std::int8_t>(WindowRuntime_Settings(session).inventoryActorSelection) > 0) {
            item = &(*session.WorldScene().petRecords)[
                WindowItemSelector_ReadSelectedPet(&inventory.normalSelector)].items6E4[sourceIndex];
        } else {
            item = &session.WorldScene().playerItems[sourceIndex];
        }
        if (item->used == 0) {
            static_cast<void>(WindowDrag_ResetMouse(session));
        }
    } else if (kind == 2) {
        if (session.WorldScene().playerItems[sourceIndex].used == 0) {
            static_cast<void>(WindowDrag_ResetMouse(session));
        }
    } else if (kind == 3) {
        std::uint16_t used;
        std::memcpy(
            &used, &storage.groups[0].items[sourceIndex - 100], sizeof(used));
        if (used == 0) {
            static_cast<void>(WindowDrag_ResetMouse(session));
        }
    }
    if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session)) & platform::LegacyMouseEdge_LDown) != 0) {
        static_cast<void>(WindowDrag_ResetMouse(session));
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY)));
        static_cast<void>(WindowDrag_StartAtPosition(session, 2, sourceIndex, x, y, WindowItemDrag_Handle));
    }
    if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session)) & platform::LegacyMouseEdge_RDown) != 0) {
        WindowDrag_Cancel(session);
    }
    static_cast<void>(platform::RequestCursorState_506DE0(session, 1, 0));
    const std::int32_t drawKind = WindowItem_ClassifyItemSlot(sourceIndex);
    std::int32_t imageNo;
    const render::GraphicDescriptor16* drawDescriptor;
    if (drawKind == 1) {
        const std::int8_t mode = std::bit_cast<std::int8_t>(WindowRuntime_Settings(session).inventoryActorSelection);
        drawDescriptor = &descriptor;
        if (mode > 0) {
            imageNo = (*session.WorldScene().petRecords)[
                WindowItemSelector_ReadSelectedPet(&inventory.normalSelector)].items6E4[sourceIndex].imageNo;
        } else {
            imageNo = session.WorldScene().playerItems[sourceIndex].imageNo;
        }
    } else if (drawKind == 2) {
        drawDescriptor = &descriptor;
        imageNo = session.WorldScene().playerItems[sourceIndex].imageNo;
    } else if (drawKind == 3) {
        drawDescriptor = &descriptor;
        std::memcpy(&imageNo,
            &storage.groups[0].items[sourceIndex - 100].imageNo, sizeof(imageNo));
    } else {
        return;
    }
    const std::int32_t y =
        static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY);
    const std::int32_t x =
        static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX);
    static_cast<void>(cg::render::DrawGraphic(x, y, 230, imageNo, 0, drawDescriptor));
}

// IDE01：原九条；mode S32先写再真实kind2调用/结果透传，正确正文保，十三父/owner/运行待。
// 原函数：0x0048BF10。
std::int32_t WindowItemDrag_Start(session::ClientSession& session, const std::int32_t sourceIndex, const std::int32_t mode)
{
    auto& interaction = session.UiFrame().native.interaction;
    interaction.itemDragMode = mode;
    return WindowDrag_Start(session, 2, sourceIndex, WindowItemDrag_Handle);
}

// NPI01：原474550完整540条；元素X/Y先坐标，四绘制按原descriptor/图号先后取样，逐键CryptInt/优先级及AL结果保。
// 原函数：0x00474550。
std::uint8_t WindowInput_HandleNumber(
    WindowInstance* const window, const std::int32_t index, const std::int32_t state,
    const std::int32_t maximum, const std::uint8_t layer, const wchar_t* const* const hints,
    const std::uint8_t flags, const std::uint32_t color)
{
    auto& uiFrame = window->ownerSession->UiFrame();
    auto* elementsWindow = window;
    const std::int32_t windowY = window->y;
    const std::int32_t windowX = window->x;
    auto* const elementXElement = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(index)) : nullptr;
    if (elementXElement) {
        const std::int32_t elementX = elementXElement->x;
        auto* const elementYElement = elementsWindow ? elementsWindow->FindElementById(NativeElementKey(index)) : nullptr;
        if (elementYElement) {
            const std::int32_t elementY = elementYElement->y;
            const std::int32_t x = windowX + elementX;
            const std::int32_t y = windowY + elementY;
            render::GraphicDescriptor16 descriptor {};
            descriptor.color = color;
            descriptor.mixFlags = cg::render::GraphicMixFlag_OriginalPalette;
            // 每个键独立短路读取四次CryptInt，不能用一次坐标快照替代。
            const auto hit = [window](const std::int32_t left, const std::int32_t top,
                               const std::int32_t right, const std::int32_t bottom) {
                if (!platform::IsSessionInputTarget(*window->ownerSession)) return false;
                return (static_cast<std::int32_t>(left) <= window->ownerSession->UiFrame().mouse.state->mouseX)
                    && (window->ownerSession->UiFrame().mouse.state->mouseX < static_cast<std::int32_t>(right))
                    && (static_cast<std::int32_t>(top) <= window->ownerSession->UiFrame().mouse.state->mouseY)
                    && (window->ownerSession->UiFrame().mouse.state->mouseY < static_cast<std::int32_t>(bottom));
            };
            std::int32_t selected = -1;
            for (std::int32_t row = 0; row < 3; ++row) {
                const std::int32_t bottom = y + 57 - row * 20;
                for (std::int32_t column = 0; column < 3; ++column) {
                    const std::int32_t digit = 1 + row * 3 + column;
                    const std::int32_t left = x + column * 24;
                    std::int32_t imageState = 0;
                    if (hit(left, bottom - 19, left + 21, bottom)) {
                        uiFrame.hoverText = hints[0];
                        imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : 2;
                        if ((state & MouseFlag_LDownEvent) != 0) {
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
                uiFrame.hoverText = hints[0];
                imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : 2;
                if ((state & MouseFlag_LDownEvent) != 0) {
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
                    uiFrame.hoverText = hints[1];
                    imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : 2;
                    if ((state & MouseFlag_LDownEvent) != 0) {
                        selected = 13;
                    }
                } else {
                    auto* const valueElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
                    if (valueElement) {
                        const std::int32_t value = std::bit_cast<std::int32_t>(valueElement->state);
                        uiFrame.hoverText = hints[value > 0 ? 1 : 2];
                    }
                    imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : 2;
                    auto* const currentElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
                    if (currentElement2) {
                        const std::int32_t current = std::bit_cast<std::int32_t>(currentElement2->state);
                        if (current > 0) {
                            if ((state & MouseFlag_LDownEvent) != 0) {
                                selected = 13;
                            }
                        } else if ((state & MouseFlag_LDownEvent) != 0) {
                            static_cast<void>(audio::PlaySe_52C450(*window->ownerSession, 55, 320));
                        }
                    }
                }
            }
            auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
            if (indexedElement) {
                if ((flags & 1u) == 0 && selected < 0
                    && std::bit_cast<std::int32_t>(indexedElement->state) <= 0) {
                    imageState = 1;
                }
            }
            const std::int32_t confirmImage = gNumericKeyImages54F030[13][imageState];
            const auto* const confirmDescriptor = &descriptor;
            static_cast<void>(cg::render::DrawGraphic(
                x + 58, y + 68, layer, confirmImage, 0, confirmDescriptor));
            for (std::int32_t row = 0; row < 3; ++row) {
                const std::int32_t centerY = y + 28 + row * 20;
                imageState = 0;
                if (hit(x + 75, centerY - 10, x + 100, centerY + 9)) {
                    uiFrame.hoverText = hints[row + 3];
                    imageState = (state & MouseFlag_LHoldPhysical) != 0 ? 1 : 2;
                    if ((state & MouseFlag_LDownEvent) != 0) {
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
                    if (platform::ReadKeyFlag4_4597A0(*window->ownerSession, 48 + digit) != 0
                        || platform::ReadKeyFlag4_4597A0(*window->ownerSession, 96 + digit) != 0) {
                        selected = digit;
                        break;
                    }
                }
            }
            if (platform::ReadKeyFlag4_4597A0(*window->ownerSession, 8) != 0) {
                auto* const elementPtr = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
                if (elementPtr) {
                    WindowElement* const element = elementPtr;
                    const std::int32_t value = std::bit_cast<std::int32_t>(element->state);
                    element->state = static_cast<std::uint32_t>(value / 10);
                }
                static_cast<void>(audio::PlaySe_52C450(*window->ownerSession, 51, 320));
                return 1;
            }
            if (selected >= 0 && selected <= 9) {
                auto* const multiplyTargetElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
                if (multiplyTargetElement) {
                    WindowElement* const multiplyTarget = multiplyTargetElement;
                    const std::int32_t previous = std::bit_cast<std::int32_t>(multiplyTarget->state);
                    multiplyTarget->state = static_cast<std::uint32_t>(previous * 10);
                }
                auto* const appendTargetElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
                if (appendTargetElement) {
                    WindowElement* const appendTarget = appendTargetElement;
                    const std::int32_t multiplied = std::bit_cast<std::int32_t>(appendTarget->state);
                    appendTarget->state = static_cast<std::uint32_t>(multiplied + selected);
                }
                auto* const elementPtr = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
                if (elementPtr) {
                    WindowElement* const element = elementPtr;
                    if (std::bit_cast<std::int32_t>(element->state) > maximum) {
                        element->state = static_cast<std::uint32_t>(maximum);
                    }
                }
            }
            if (selected == 10) {
                auto* const elementPtr = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
                if (elementPtr) {
                    WindowElement* const element = elementPtr;
                    const std::int32_t value = std::bit_cast<std::int32_t>(element->state);
                    element->state = static_cast<std::uint32_t>(value / 10);
                }
            } else if (selected == 11) {
                auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
                if (indexedElement2) {
                    indexedElement2->state = static_cast<std::uint32_t>(maximum);
                }
            } else if (selected == 12) {
                auto* const indexedElement2 = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
                if (indexedElement2) {
                    indexedElement2->state = 0;
                }
            } else if (selected == 13) {
                static_cast<void>(audio::PlaySe_52C450(*window->ownerSession, 51, 320));
                return 3;
            } else if (selected < 0) {
                return 0;
            }
        }
    }
    static_cast<void>(audio::PlaySe_52C450(*window->ownerSession, 51, 320));
    return 1;
}

// WCL01：原4C1FB0完整53条；持同窗口/graphic，bit2仅置关闭位与SE57，图号优先序保，返回0/1。
// 原函数：0x004C1FB0。
std::int32_t WindowRuntime_ButtonClose(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    WindowInstance* const window = &callbackWindow;
    auto* const graphicElement = window ? window->FindElementById(elementId) : nullptr;
    if (graphicElement) {
        auto* const graphic = graphicElement->data.graphic;
        const bool hover = (state & MouseFlag_Hover) != 0;
        if (hover) {
            uiFrame.hoverText = L"关闭这个视窗。";
        }
        std::int32_t result = 0;
        if ((state & MouseFlag_LDownEvent) != 0) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(*callbackWindow.ownerSession, 57, 320));
            result = 1;
        }
        graphic->imageNo = 0x3B538;
        if (hover) {
            graphic->imageNo = 0x3B53A;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            graphic->imageNo = 0x3B539;
        }
        return result;
    }
    return 0;
}

// 原0x004FBA30：UTF-16色码分段与即时测宽；正文使用动态宽字符串。
std::int32_t WindowItem_SplitTooltipLines(
    session::ClientSession& session,
    std::int32_t x, const std::int32_t y, const std::uint8_t renderMode,
    const std::int32_t fontType, std::int8_t color, std::wstring_view text,
    const std::int8_t value7, const std::int8_t marker)
{
    auto& interaction = session.UiFrame().native.interaction;
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
            if (digit >= 0 && digit <= 9) { interaction.tooltipColor = digit; color = static_cast<std::int8_t>(digit); }
        }
    }
    return render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)),
        renderMode, static_cast<std::int8_t>(render::ResolveFontId(fontType)),
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(interaction.tooltipColor)),
        text, value7, marker);
}

// TDR01：原48B810完整108条；层5清尾/题目/逐行颜色/可选footer后背景入队，原坐标截断及真实返回保持。
template <typename ReadLine>
static std::int32_t WindowItem_DrawTooltipLines(
    session::ClientSession& session,
    const std::int32_t x, const std::int32_t y,
    const wchar_t* const title, const std::int8_t titleColor,
    const ReadLine& readLine, const std::int32_t lineCount,
    const wchar_t* const footer, const std::int8_t footerColor)
{
    auto& interaction = session.UiFrame().native.interaction;
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
    interaction.tooltipColor = 0;
    std::int32_t lineY = 0;
    for (std::int32_t line = 0; line < lineCount; ++line) {
        const auto contents = readLine(line);
        if (!contents.empty()) {
            const std::int32_t shortY =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
            const std::int32_t shortX =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
            static_cast<void>(WindowItem_SplitTooltipLines(
                session,
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
    session::ClientSession& session,
    const std::int32_t x, const std::int32_t y,
    const wchar_t* const title, const std::int8_t titleColor,
    const std::wstring* const lines, const std::int32_t lineCount,
    const wchar_t* const footer, const std::int8_t footerColor)
{
    return WindowItem_DrawTooltipLines(session, x, y, title, titleColor,
        [lines](const std::int32_t line) { return std::wstring_view(lines[line]); },
        lineCount, footer, footerColor);
}

// SIC01：入口预览地址贯穿十成员构造与字段发布；原初始化/三descriptor及this返回保持，整对象不清零。
// 原函数：0x004C61B0。
WindowItemSelectorState* WindowItemSelector_ConstructPreview(session::ClientSession& session, WindowItemSelectorState* const state, const std::int32_t previewIndex)
{
    std::uint32_t flags;
    cg::yobi::YobiAction* const preview = cg::yobi::ConstructWindowSelectorPreview(session, previewIndex, flags);
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
void WindowItemSelector_BindElements(WindowInstance* callbackWindow, WindowItemSelectorState* const state, const std::uint8_t mode)
{
    WindowInstance* const window = callbackWindow;
    if (!window) return;
    state->value64 = mode;
    if (mode != 0) {
        state->mode68 = &WindowRuntime_Settings(*callbackWindow->ownerSession).battleItemActorSelection;
        state->elements00[0] = WindowElementLink{window->id, NativeElementKey(3), window->ownerSession};
        state->elements00[1] = WindowElementLink{window->id, NativeElementKey(20), window->ownerSession};
        state->elements00[2] = WindowElementLink{window->id, NativeElementKey(16), window->ownerSession};
        state->elements00[3] = WindowElementLink{window->id, NativeElementKey(17), window->ownerSession};
        state->elements00[4] = WindowElementLink{window->id, NativeElementKey(18), window->ownerSession};
        state->elements00[5] = WindowElementLink{window->id, NativeElementKey(19), window->ownerSession};
        state->elements00[6] = WindowElementLink{window->id, NativeElementKey(4), window->ownerSession};
        state->elements00[7] = WindowElementLink{window->id, NativeElementKey(5), window->ownerSession};
        state->elements00[9] = WindowElementLink{window->id, NativeElementKey(10), window->ownerSession};
        state->elements00[10] = WindowElementLink{window->id, NativeElementKey(11), window->ownerSession};
        state->elements00[11] = WindowElementLink{window->id, NativeElementKey(12), window->ownerSession};
        state->elements00[12] = WindowElementLink{window->id, NativeElementKey(13), window->ownerSession};
        state->elements00[13] = WindowElementLink{window->id, NativeElementKey(14), window->ownerSession};
        state->elements00[14] = WindowElementLink{window->id, NativeElementKey(15), window->ownerSession};
        state->elements00[8] = WindowElementLink{window->id, NativeElementKey(6), window->ownerSession};
        state->elements00[15] = WindowElementLink{window->id, NativeElementKey(7), window->ownerSession};
        state->elements00[16] = WindowElementLink{window->id, NativeElementKey(8), window->ownerSession};
        state->elements00[17] = WindowElementLink{window->id, NativeElementKey(9), window->ownerSession};
        state->callbacks50[0] = WindowBattleItems_HandleInventory;
        state->elements00[18] = WindowElementLink{window->id, NativeElementKey(21), window->ownerSession};
        state->callbacks50[1] = WindowElement_NoOp;
        state->callbacks50[2] = WindowBattleItems_HandleEquipment;
        state->callbacks50[3] = WindowElement_NoOp;
    } else {
        state->mode68 = &WindowRuntime_Settings(*callbackWindow->ownerSession).inventoryActorSelection;
        state->elements00[0] = WindowElementLink{window->id, NativeElementKey(4), window->ownerSession};
        state->elements00[1] = WindowElementLink{window->id, NativeElementKey(17), window->ownerSession};
        state->elements00[2] = WindowElementLink{window->id, NativeElementKey(18), window->ownerSession};
        state->elements00[3] = WindowElementLink{window->id, NativeElementKey(19), window->ownerSession};
        state->elements00[4] = WindowElementLink{window->id, NativeElementKey(20), window->ownerSession};
        state->elements00[5] = WindowElementLink{window->id, NativeElementKey(21), window->ownerSession};
        state->elements00[6] = WindowElementLink{window->id, NativeElementKey(5), window->ownerSession};
        state->elements00[7] = WindowElementLink{window->id, NativeElementKey(6), window->ownerSession};
        state->elements00[9] = WindowElementLink{window->id, NativeElementKey(11), window->ownerSession};
        state->elements00[10] = WindowElementLink{window->id, NativeElementKey(12), window->ownerSession};
        state->elements00[11] = WindowElementLink{window->id, NativeElementKey(13), window->ownerSession};
        state->elements00[12] = WindowElementLink{window->id, NativeElementKey(14), window->ownerSession};
        state->elements00[13] = WindowElementLink{window->id, NativeElementKey(15), window->ownerSession};
        state->elements00[14] = WindowElementLink{window->id, NativeElementKey(16), window->ownerSession};
        state->elements00[8] = WindowElementLink{window->id, NativeElementKey(7), window->ownerSession};
        state->elements00[15] = WindowElementLink{window->id, NativeElementKey(8), window->ownerSession};
        state->elements00[16] = WindowElementLink{window->id, NativeElementKey(9), window->ownerSession};
        state->elements00[17] = WindowElementLink{window->id, NativeElementKey(10), window->ownerSession};
        state->elements00[18] = WindowElementLink{window->id, NativeElementKey(23), window->ownerSession};
        state->elements00[19] = WindowElementLink{window->id, NativeElementKey(22), window->ownerSession};
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
        if (auto* element = state->elements00[6].get()) element->visible = 1;
    }
    if (state->value64 == 0) {
        if (auto* element = state->elements00[19].get()) element->visible = 1;
    }
}

// SPG01：宠物页/名称负重及原动态空位显示保持；只判低AL，不钳制itemSlots或补保护。
// 原函数：0x004C6470。
static void WindowItemSelector_ShowPetPage(session::ClientSession& session, WindowItemSelectorState* const state)
{
    if (state->value60 == 3) {
        if (auto* element = state->elements00[7].get()) element->visible = 1;
        if (auto* element = state->elements00[8].get()) element->visible = 1;
    }
    if (auto* element = state->elements00[15].get()) element->visible = 1;
    if (auto* element = state->elements00[16].get()) element->visible = 1;
    const std::int32_t itemSlots =
        state->value3ED != 0 && world::IsValidPetIndex_51C410(state->selectedPet70)
        ? (*session.WorldScene().petRecords)[state->selectedPet70].values6D8[0] : 0;
    for (std::int32_t position = itemSlots; position < 6; ++position) {
        if (auto* element = state->elements00[9 + position].get()) element->visible = 1;
    }
    if (auto* element = state->elements00[17].get()) element->visible = 1;
    if (state->value64 == 0) {
        if (auto* element = state->elements00[19].get()) element->visible = 1;
    }
}

// SPG01：按原顺序隐藏槽6/7/8/15/16、六装备槽与17；模式0才隐藏19，主元素不改。
// 原函数：0x004C6500。
static void WindowItemSelector_HidePageElements(WindowItemSelectorState* const state)
{
    if (auto* element = state->elements00[6].get()) element->visible = 0;
    if (auto* element = state->elements00[7].get()) element->visible = 0;
    if (auto* element = state->elements00[8].get()) element->visible = 0;
    if (auto* element = state->elements00[15].get()) element->visible = 0;
    if (auto* element = state->elements00[16].get()) element->visible = 0;
    for (std::int32_t position = 9; position < 15; ++position) {
        if (auto* element = state->elements00[position].get()) element->visible = 0;
    }
    if (auto* element = state->elements00[17].get()) element->visible = 0;
    if (state->value64 == 0) {
        if (auto* element = state->elements00[19].get()) element->visible = 0;
    }
}

// SPG01：先发布页再隐藏/新页分发；CC40D4仅低BYTE、四回调先取元素；原五页和写序保持，全机器ABI待。
// 原函数：0x004C6550。
static void WindowItemSelector_SelectPage(session::ClientSession& session, WindowItemSelectorState* const state, const std::int32_t page)
{
    auto& inventory = session.UiFrame().native.inventory;
    if (page == 1 && std::bit_cast<std::int8_t>(*state->mode68) > 0) {
        static_cast<void>(WindowItemSelector_SelectPet(session, state));
        if (!world::IsValidPetIndex_51C410(state->selectedPet70)
            || (*session.WorldScene().petRecords)[state->selectedPet70].used == 0) {
            static_cast<void>(WindowItemSelector_SetMode(session, state, 0));
        }
    }
    state->value60 = page;
    WindowItemSelector_HidePageElements(state);
    switch (state->value60) {
    case 0:
        if (auto* element = state->elements00[1].get()) element->mouseCallback = WindowElement_NoOp;
        if (auto* element = state->elements00[1].get()) element->visible = 0;
        WindowItemSelector_MoveElements(state, 16);
        static_cast<void>(WindowItemSelector_SetMode(session, state, -1));
        break;
    case 1: {
        const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
        if (mode == 0) {
            WindowItemSelector_ShowPlayerPage(state);
            if (auto* element = state->elements00[1].get()) element->data.graphic->imageNo = 98146;
        } else if (mode > 0) {
            WindowItemSelector_ShowPetPage(session, state);
            if (auto* element = state->elements00[1].get()) element->data.graphic->imageNo = 98147;
            if (world::IsValidPetIndex_51C410(state->selectedPet70)) {
                for (std::int32_t position = 0;
                     position < (*session.WorldScene().petRecords)[state->selectedPet70].values6D8[0];
                     ++position) {
                    if (auto* element = state->elements00[9 + position].get()) element->visible = 1;
                }
            }
        }
        if (auto* element = state->elements00[1].get()) element->mouseCallback = WindowElement_NoOp;
        if (auto* element = state->elements00[1].get()) element->visible = 1;
        state->step6C = 21;
        break;
    }
    case 2:
        *state->mode68 = 0;
        WindowItemSelector_ShowPlayerPage(state);
        if (state->value64 != 0
            || *reinterpret_cast<const unsigned char*>(&inventory.selectionMode) == 0) {
            WindowElement* const element = state->elements00[1];
            const WindowElementCallback callback = state->callbacks50[2];
            if (element) element->mouseCallback = callback;
        } else {
            WindowElement* const firstElement = state->elements00[0];
            const WindowElementCallback firstCallback = state->callbacks50[1];
            if (firstElement) firstElement->mouseCallback = firstCallback;
            WindowElement* const secondElement = state->elements00[1];
            const WindowElementCallback secondCallback = state->callbacks50[3];
            if (secondElement) secondElement->mouseCallback = secondCallback;
        }
        WindowItemSelector_MoveElements(state, -182);
        if (auto* element = state->elements00[1].get()) element->visible = 1;
        if (auto* element = state->elements00[1].get()) element->data.graphic->imageNo = 98146;
        break;
    case 3:
        WindowItemSelector_MoveElements(state, -182);
        WindowItemSelector_RefreshPetHeader(session, state);
        WindowItemSelector_ShowPetPage(session, state);
        if (state->value3ED == 1) {
            WindowElement* const element = state->elements00[1];
            const WindowElementCallback callback = state->callbacks50[2];
            if (element) element->mouseCallback = callback;
        } else {
            if (auto* element = state->elements00[1].get()) element->mouseCallback = WindowElement_NoOp;
        }
        if (auto* element = state->elements00[1].get()) element->visible = 1;
        if (auto* element = state->elements00[1].get()) element->data.graphic->imageNo = 98147;
        break;
    case 4:
        state->value60 = 0;
        WindowItemSelector_MoveElements(state, 16);
        if (auto* element = state->elements00[1].get()) element->mouseCallback = WindowElement_NoOp;
        if (auto* element = state->elements00[1].get()) element->visible = 0;
        static_cast<void>(WindowItemSelector_SetMode(session, state, -1));
        break;
    default:
        break;
    }
}

// SFD01：原四页分发/业务钳制保持；两按钮按原顺序取样，共借descriptor[0]；已知上行由窗口回调返回1。
// 原函数：0x004C6790。
static void WindowItemSelector_UpdatePage(WindowInstance* callbackWindow, WindowItemSelectorState* const state)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    // 原常量参考：0x0054F564。
    static constexpr std::int32_t kPreviewToggleImages[] {98133, 98133, 98135, 98134};
    // 原常量参考：0x0054F574。
    static constexpr std::int32_t kEquipmentToggleImages[] {98137, 98137, 98139, 98138};
    const std::int32_t page = state->value60;
    if (page > 0 && page <= 2 && std::bit_cast<std::int8_t>(*state->mode68) > 0) {
        static_cast<void>(WindowItemSelector_SelectPet(*callbackWindow->ownerSession, state));
        if (!world::IsValidPetIndex_51C410(state->selectedPet70)
            || (*callbackWindow->ownerSession->WorldScene().petRecords)[state->selectedPet70].used == 0) {
            static_cast<void>(WindowItemSelector_SetMode(*callbackWindow->ownerSession, state, 0));
        }
    }
    switch (state->value60) {
    case 1: {
        if (std::bit_cast<std::int8_t>(*state->mode68) > 0) {
            static_cast<void>(WindowItemSelector_SelectPet(*callbackWindow->ownerSession, state));
        }
        auto* const panelElement = state->elements00[1].get();
        if (!panelElement) break;
        const std::int32_t x = panelElement->x;
        if (x > -182) {
            WindowItemSelector_MoveElements(state, x - state->step6C);
            const std::int32_t step = state->step6C;
            const WindowElement* const element = state->elements00[1];
            state->step6C = step - 1;
            if (element->x <= -182) {
                WindowItemSelector_MoveElements(state, -182);
            }
        } else {
            WindowItemSelector_SelectPage(*callbackWindow->ownerSession, state, std::bit_cast<std::int8_t>(*state->mode68) > 0 ? 3 : 2);
        }
        break;
    }
    case 2:
        if (std::bit_cast<std::int8_t>(*state->mode68) > 0) {
            WindowItemSelector_SelectPage(*callbackWindow->ownerSession, state, 3);
        } else {
            WindowItemSelector_DrawTooltip(callbackWindow, state);
        }
        break;
    case 3: {
        static_cast<void>(WindowItemSelector_SelectPet(*callbackWindow->ownerSession, state));
        if (!world::IsValidPetIndex_51C410(state->selectedPet70)
            || (*callbackWindow->ownerSession->WorldScene().petRecords)[state->selectedPet70].used == 0) {
            *state->mode68 = 0;
        }
        if (*state->mode68 == 0) {
            WindowItemSelector_SelectPage(*callbackWindow->ownerSession, state, 2);
            break;
        }
        WindowItemSelector_DrawTooltip(callbackWindow, state);
        WindowItemSelector_RefreshPetHeader(*callbackWindow->ownerSession, state);
        const std::int32_t firstIndex = state->imageIndex3F0;
        const WindowElement* const firstElement = state->elements00[1].get();
        if (!firstElement) break;
        const render::GraphicDescriptor16* const descriptor = &state->descriptors3B8[0];
        const std::int32_t firstImage = kPreviewToggleImages[firstIndex];
        const WindowInstance* const firstWindow = currentWindow;
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
        const WindowInstance* const secondWindow = currentWindow;
        const std::int32_t secondElementY = secondElement->y;
        const std::int32_t secondWindowY = secondWindow->y;
        const std::int32_t secondElementX = secondElement->x;
        const std::int32_t secondWindowX = secondWindow->x;
        const std::int32_t secondY = secondElementY + secondWindowY + 180;
        const std::int32_t secondX = secondElementX + secondWindowX + 118;
        static_cast<void>(cg::render::DrawGraphic(
            secondX, secondY, 223, secondImage, 0, descriptor));
        static_cast<void>(WindowItemSelector_DrawPreview(callbackWindow, state));
        break;
    }
    case 4:
        WindowItemSelector_SelectPage(*callbackWindow->ownerSession, state, 0);
        break;
    default:
        break;
    }
}

// PSP01：同state mode按S8映射，回调返回的S32索引原序发布到selectedPet70并返回。
// 原函数：0x004C6990。
static std::int32_t WindowItemSelector_SelectPet(session::ClientSession& session, WindowItemSelectorState* const state)
{
    const std::int32_t index = WindowItemSelector_FindPetIndex(session, std::bit_cast<std::int8_t>(*state->mode68));
    state->selectedPet70 = index;
    return index;
}

// PSP01：同宠物previous/updated BYTE及名字/负重原序保持；首次测宽先字体后文本，负重先文本后字体。
// 原函数：0x004C69B0。
static void WindowItemSelector_RefreshPetHeader(session::ClientSession& session, WindowItemSelectorState* const state)
{
    if (state->selectedPet70 == -1 || !world::IsValidPetIndex_51C410(state->selectedPet70)) {
        return;
    }
    const std::int32_t selected = state->selectedPet70;
    auto* const pet = &(*session.WorldScene().petRecords)[selected];
    if (selected == state->previousPet74 && pet->updated510C == 0) {
        return;
    }
    state->previousPet74 = selected;
    pet->updated510C = 0;
    auto* const panelElement = state->elements00[1].get();
    if (auto* const nameElement = state->elements00[7].get()) {
        auto* const nameData = nameElement->data.text;
        nameData->text = pet->name6BD[0] != '\0' ? pet->name6BD.data() : pet->name6AC.data();
        if (panelElement) {
            const auto font = std::to_integer<std::uint8_t>(nameData->style.font);
            const auto width = cg::render::TextMeasure(nameData->text, font);
            nameElement->x = panelElement->x - width / 2 + 101;
        }
    }
    if (auto* const weightElement = state->elements00[8].get()) {
        auto* const weightData = weightElement->data.text;
        cg::text::FormatTo(weightData->text, L"负重:{:3}/{:3}", pet->values6D8[1], pet->values6D8[2]);
        if (panelElement) {
            const auto font = std::to_integer<std::uint8_t>(weightData->style.font);
            const auto width = cg::render::TextMeasure(weightData->text, font);
            weightElement->x = panelElement->x - width / 2 + 101;
        }
    }
}

// SPG01：依原序移动主元素、四按钮及六表坐标；原S32结果与固定循环保持，不新增钳制。
// 原函数：0x004C6AC0。
static void WindowItemSelector_MoveElements(WindowItemSelectorState* const state, const std::int32_t x)
{
    if (auto* element = state->elements00[1].get()) element->x = x;
    for (std::int32_t position = 2; position < 6; ++position) {
        if (auto* element = state->elements00[position].get()) element->x = x - 15;
    }
    for (std::int32_t position = 0; position < 6; ++position) {
        if (auto* element = state->elements00[9 + position].get()) element->x = gSelectorPetX54F5C4[position] + x - 24;
    }
}

// SLC01：两轴先窗口再元素取值，flags/淡入计数只读原BYTE；void不证明原EAX上行等价。
// 原函数：0x004C6B10。
static void WindowItemSelector_DrawTooltip(WindowInstance* callbackWindow, WindowItemSelectorState* const state)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return;
    auto& inventory = callbackWindow->ownerSession->UiFrame().native.inventory;
    auto* const panelElement = state->elements00[1].get();
    if (!panelElement) return;
    if (state->value3ED == 0 || state->hoverIndex3F8 < 0) {
        state->hoverCount3A8 = 0;
        return;
    }
    const std::int32_t localX = WindowItemSelector_ReadSlotX(state, state->hoverIndex3F8);
    const WindowElement* const horizontalElement = panelElement;
    const WindowInstance* const horizontalWindow = currentWindow;
    const std::int32_t windowX = horizontalWindow->x;
    const std::int32_t elementX = horizontalElement->x;
    const std::int32_t left = localX + (windowX + elementX) - 24;
    const std::int32_t localY = WindowItemSelector_ReadSlotY(state, state->hoverIndex3F8);
    const WindowElement* const verticalElement = panelElement;
    const WindowInstance* const verticalWindow = currentWindow;
    const std::int32_t windowY = verticalWindow->y;
    const std::int32_t elementY = verticalElement->y;
    const std::int32_t top = localY + (windowY + elementY) - 24;
    cg::render::DrawGraphicHover(
        static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
        static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
        223, callbackWindow->ownerSession->UiFrame().hoverFramePaletteIndex, 0);
    const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
    std::int32_t slot;
    world::PlayerItemRecord* item;
    if (mode == 0) {
        slot = state->hoverIndex3F8;
        item = &callbackWindow->ownerSession->WorldScene().playerItems[slot];
    } else {
        if (mode < 0 || state->selectedPet70 < 0) {
            return;
        }
        slot = state->hoverIndex3F8;
        const std::int32_t pet = WindowItemSelector_ReadSelectedPet(&inventory.normalSelector);
        item = &(*callbackWindow->ownerSession->WorldScene().petRecords)[pet].items6E4[slot];
    }
    if (item->used != 0) {
        const auto* const occupancy =
            reinterpret_cast<const unsigned char*>(inventory.slotUseCounts.data());
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
static std::int32_t WindowItemSelector_DrawPreview(WindowInstance* callbackWindow, WindowItemSelectorState* const state)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    auto* const panelElement = state->elements00[1].get();
    if (!panelElement) return 0;
    const std::int32_t selected = state->selectedPet70;
    if (selected < 0 || state->value3EC == 0) {
        return selected;
    }
    cg::yobi::YobiAction* const preview = state->preview;
    const std::int32_t imageNo = (*callbackWindow->ownerSession->WorldScene().petRecords)[selected].imageNo6A0;
    cg::yobi::SetYobiActionField2A8(preview, imageNo);
    const std::int32_t result = cg::yobi::AdvanceYobiAnimationFrame_517880(preview, 100, 0, 0);
    const std::int32_t graphicIndex = cg::yobi::GetYobiActionField20(preview);
    if (graphicIndex == 0) {
        return result;
    }
    const render::GraphicDescriptor16* const descriptor =
        cg::yobi::ApplyWindowAnimationDrawMode(preview, true);
    const std::uint8_t layer = cg::yobi::GetYobiActionField15(preview);
    const std::int32_t offsetY = cg::yobi::GetYobiGraphicOriginOffset_5182B0(preview, cg::yobi::YobiGraphicAxis_Y);
    const std::int32_t y = offsetY + currentWindow->y + 163;
    const std::int32_t offsetX = cg::yobi::GetYobiGraphicOriginOffset_5182B0(preview, cg::yobi::YobiGraphicAxis_X);
    const std::int32_t elementX = panelElement->x;
    const std::int32_t x = elementX + offsetX + currentWindow->x + 101;
    return render::DrawGraphic2_521DD0(x, y, layer, graphicIndex, 0, 0, descriptor);
}

// PSP01：五排序对按used W16/索引S16/状态W16计数；不足仍取末扫描项，不新增匹配/索引保护。
// 原函数：0x004C71C0。
static std::int32_t WindowItemSelector_FindPetIndex(session::ClientSession& session, const std::int32_t ordinal)
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
        const auto& pair = session.WorldScene().petPairs[position];
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            const std::uint16_t flags = (*session.WorldScene().petRecords)[index].flags6AA;
            if (flags == 2 || flags == 1) {
                ++matched;
            }
        }
    }
    --position;
    return position < 0 ? -1
        : std::bit_cast<std::int16_t>(session.WorldScene().petPairs[position].index);
}

// SPG01：写mode低BYTE后持旧页；页3按新mode切2/3、页2不切；bool仅承接原AL=1，高位ABI待。
// 原函数：0x004C7340。
static bool WindowItemSelector_SetMode(session::ClientSession& session, WindowItemSelectorState* const state, const std::int32_t mode)
{
    *state->mode68 = static_cast<std::uint8_t>(mode);
    const std::int32_t page = state->value60;
    if (page == 2 || page == 3) {
        if (*state->mode68 == 0) {
            if (page != 2) {
                WindowItemSelector_SelectPage(session, state, 2);
            }
        } else if (page != 2) {
            WindowItemSelector_SelectPage(session, state, 3);
        }
    }
    return true;
}

// SEV01：页签提示/点击/绘制与图号原序；中心坐标显式取样，descriptor地址先图号；bool限原AL消费。
// 原函数：0x004C7380。
bool WindowItemSelector_HandleTab(WindowInstance* callbackWindow,
    WindowItemSelectorState* const state, const std::int32_t tab, const std::int32_t events)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return false;
    auto& uiFrame = callbackWindow->ownerSession->UiFrame();
    // 原常量参考：0x0054F5F4。
    static constexpr std::int32_t kEquipmentTabImages[] {
        244294, 244294, 244295, 244296, 98148, 98131, 98148, 98132,
        98142, 98140, 98142, 98141, 98145, 98143, 98145, 98144,
    };
    WindowElement* const element = state->elements00[tab + 2].get();
    if (!element) return false;
    auto* const graphic = element->data.graphic;
    const bool hovered = (events & MouseFlag_Hover) != 0;
    bool consumed = false;
    if (hovered) {
        const std::int32_t page = state->value60;
        if (page == 0) {
            uiFrame.hoverText = tab == 0
                ? L"开启装备视窗。"
                : L"开启宠物装备。";
        } else if (page > 1 && page <= 3) {
            if (std::bit_cast<std::int8_t>(*state->mode68) == tab) {
                uiFrame.hoverText = tab == 0
                    ? L"关闭装备视窗。"
                    : L"关闭宠物装备。";
            } else {
                uiFrame.hoverText = tab == 0
                    ? L"切换到身上装备。"
                    : L"切换宠物装备。";
            }
        }
    }
    if ((events & MouseFlag_LDownEvent) != 0) {
        currentWindow->state |= 0x2000u;
        static_cast<void>(audio::PlaySe_52C450(*callbackWindow->ownerSession, 51, 320));
        const std::int32_t page = state->value60;
        if (page == 0) {
            static_cast<void>(WindowItemSelector_SetMode(*callbackWindow->ownerSession, state, tab));
            WindowItemSelector_SelectPage(*callbackWindow->ownerSession, state, 1);
        } else if (page > 1 && page <= 3) {
            if (std::bit_cast<std::int8_t>(*state->mode68) == tab) {
                WindowItemSelector_SelectPage(*callbackWindow->ownerSession, state, 4);
            } else {
                static_cast<void>(WindowItemSelector_SetMode(*callbackWindow->ownerSession, state, tab));
                WindowItemSelector_SelectPage(*callbackWindow->ownerSession, state, 1);
            }
        }
        consumed = true;
    }
    const std::int32_t width = element->width;
    const WindowInstance* const window = currentWindow;
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
    if ((events & MouseFlag_LHoldPhysical) != 0) {
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
bool WindowItemSelector_TogglePreview(session::ClientSession& session, WindowItemSelectorState* const state, const std::int32_t events)
{
    bool consumed = false;
    if ((events & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        state->value3EC = state->value3EC == 0;
        consumed = true;
    }
    state->imageIndex3F0 = (events & MouseFlag_LHoldPhysical) != 0 ? 2 : ((events & MouseFlag_Hover) != 0 ? 3 : 1);
    return consumed;
}

// SEV01：先翻BYTE装备/回调，失效AL早返保部分写；目标先回调、尾selected先推进/count后取，不钳制数量。
// 原函数：0x004C7910。
bool WindowItemSelector_CycleEquipment(session::ClientSession& session, WindowItemSelectorState* const state, const std::int32_t events)
{
    bool consumed = false;
    if ((events & MouseFlag_LDownEvent) != 0) {
        static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        const std::uint8_t previous = state->value3ED;
        const std::uint8_t* const mode = state->mode68;
        const std::uint8_t enabled = previous == 0;
        state->value3ED = enabled;
        if (std::bit_cast<std::int8_t>(*mode) > 0) {
            if (enabled == 1) {
                WindowElement* const element = state->elements00[1];
                const WindowElementCallback callback = state->callbacks50[2];
                if (element) element->mouseCallback = callback;
            } else {
                if (auto* element = state->elements00[1].get()) element->mouseCallback = WindowElement_NoOp;
            }
            if (!world::IsValidPetIndex_51C410(state->selectedPet70)) {
                return false;
            }
            const std::int32_t selected = state->selectedPet70;
            std::int32_t position = 0;
            std::int32_t itemSlots = (*session.WorldScene().petRecords)[selected].values6D8[0];
            while (position < itemSlots) {
                const std::uint8_t visible = state->value3ED == 0;
                if (auto* element = state->elements00[9 + position].get()) element->visible = visible;
                const std::int32_t nextSelected = state->selectedPet70;
                ++position;
                itemSlots = (*session.WorldScene().petRecords)[nextSelected].values6D8[0];
            }
        }
        consumed = true;
    }
    state->imageIndex3F4 = (events & MouseFlag_LHoldPhysical) != 0 ? 2 : ((events & MouseFlag_Hover) != 0 ? 3 : 1);
    return consumed;
}

// SEV01：低U16两个方向门槛独立，先正向S32余8再反向减1/原加8；两声效/原顺序保，父固定返回0。
// 原函数：0x004C7A10。
bool WindowItemSelector_RotatePreview(session::ClientSession& session, WindowItemSelectorState* const state, const std::int32_t events)
{
    bool consumed = false;
    if ((events & (MouseFlag_LDownEvent | MouseFlag_LRepeatDown)) != 0) {
        static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        const std::int32_t direction = cg::yobi::GetYobiActionField2B8(state->preview);
        cg::yobi::SetYobiActionField2B8(state->preview, (direction + 1) % 8);
        consumed = true;
    }
    if ((events & (MouseFlag_RDownEvent | MouseFlag_RRepeatDown)) != 0) {
        static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
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
void WindowItemSelector_Update(session::ClientSession& session, WindowInstance* callbackWindow, WindowItemSelectorState* const state)
{
    if (state->initialized3E8 == 0) {
        const std::uint8_t* const mode = state->mode68;
        state->previousPet74 = -1;
        WindowItemSelector_SelectPage(session, state, *mode == 0xFFu ? 0 : 1);
        state->initialized3E8 = 1;
    }
    WindowItemSelector_UpdatePage(callbackWindow, state);
}

// ISP01：原local负值返-1，成功后取当前窗口，再按元素X/窗口X取样累加。
// 原函数：0x004C7240。
std::int32_t WindowItemSelector_ReadScreenX(WindowInstance* callbackWindow,
    const WindowItemSelectorState* const state, const std::int32_t element,
    const std::int32_t slot, const std::int32_t offset)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t local = WindowItemSelector_ReadSlotX(state, slot);
    if (local < 0) {
        return -1;
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementXElement = window->FindElementById(NativeElementKey(element));
    if (elementXElement) {
        const std::int32_t elementX = elementXElement->x;
        const std::int32_t windowX = window->x;
        return elementX + windowX + local + offset;
    }
    return 0;
}

// ISP01：原local负值返-1，成功后取当前窗口，再按元素Y/窗口Y取样累加。
// 原函数：0x004C7280。
std::int32_t WindowItemSelector_ReadScreenY(WindowInstance* callbackWindow,
    const WindowItemSelectorState* const state, const std::int32_t element,
    const std::int32_t slot, const std::int32_t offset)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return 0;
    const std::int32_t local = WindowItemSelector_ReadSlotY(state, slot);
    if (local < 0) {
        return -1;
    }
    const WindowInstance* const window = currentWindow;
    auto* const elementYElement = window->FindElementById(NativeElementKey(element));
    if (elementYElement) {
        const std::int32_t elementY = elementYElement->y;
        const std::int32_t windowY = window->y;
        return elementY + windowY + local + offset;
    }
    return 0;
}

// ISP01：S8模式0借玩家前8槽，正模式经原宠物校验后两次重读索引借列表/数量；失败不改输出。
// 原函数：0x004C72C0。
bool WindowItemSelector_GetItemList_4C72C0(
    session::ClientSession& session,
    const WindowItemSelectorState* const state, world::PlayerItemRecord** const items,
    std::int32_t* const count)
{
    const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
    if (mode == 0) {
        *items = session.WorldScene().playerItems.data();
        *count = 8;
        return true;
    }
    if (mode > 0 && world::IsValidPetIndex_51C410(state->selectedPet70)) {
        *items = (*session.WorldScene().petRecords)[state->selectedPet70].items6E4.data();
        *count = (*session.WorldScene().petRecords)[state->selectedPet70].values6D8[0];
        return true;
    }
    return false;
}

// SAP01：原391条三根有据；本根完整239条，图形与三种文字坐标按原读取序；格式/GBK与低AL边界保持，运行待。
// 原函数：0x004C75A0。
bool WindowItemSelector_HandlePlayerAttributes(WindowInstance* callbackWindow, WindowItemSelectorState* const selector, const std::int32_t state)
{
    WindowInstance* const currentWindow = callbackWindow;
    if (!currentWindow) return false;
    auto* const panelElement = selector->elements00[1].get();
    if (!panelElement) return false;
    if ((static_cast<std::uint8_t>(state) & MouseFlag_LDownEvent) != 0 || state == 0
        || selector->value3FC == 0 || callbackWindow->ownerSession->WorldScene().replyRecordF7C950.used == 0) {
        static_cast<void>(audio::PlaySe_52C450(*callbackWindow->ownerSession, 51, 320));
        WindowElement* const element = panelElement;
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
    const WindowElement* const element = panelElement;
    const render::GraphicDescriptor16* const descriptor = &selector->descriptors3B8[0];
    const std::int32_t elementY = element->y;
    const std::int32_t elementX = element->x;
    const WindowInstance* const window = currentWindow;
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
    const auto drawText = [panelElement, currentWindow](
        const std::int32_t offsetX, const std::int32_t offsetY,
        const std::int32_t textColor, const std::wstring_view text,
        const TextCoordinateOrder order) {
        const WindowElement* const textElement = panelElement;
        std::int32_t x;
        std::int32_t y;
        if (order == TextCoordinateOrder::Title) {
            const WindowInstance* const textWindow = currentWindow;
            const std::int32_t elementY = textElement->y;
            const std::int32_t windowY = textWindow->y;
            y = elementY + windowY + offsetY;
            const std::int32_t elementX = textElement->x;
            const std::int32_t windowX = textWindow->x;
            x = elementX + windowX + offsetX;
        } else if (order == TextCoordinateOrder::WindowBeforeElements) {
            const WindowInstance* const textWindow = currentWindow;
            const std::int32_t elementY = textElement->y;
            const std::int32_t elementX = textElement->x;
            const std::int32_t windowY = textWindow->y;
            const std::int32_t windowX = textWindow->x;
            y = elementY + windowY + offsetY;
            x = elementX + windowX + offsetX;
        } else {
            const std::int32_t elementY = textElement->y;
            const std::int32_t elementX = textElement->x;
            const WindowInstance* const textWindow = currentWindow;
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
        static_cast<std::int32_t>(cg::text::ToGbk(callbackWindow->ownerSession->WorldScene().replyRecordF7C950.text04).size());
    if (length > 0) {
        drawText(100 - (12 * length) / 2 / 2, 9, 0,
            callbackWindow->ownerSession->WorldScene().replyRecordF7C950.text04, TextCoordinateOrder::Title);
    }
    if (callbackWindow->ownerSession->WorldScene().replyRecordF7C950.value48 == 1) {
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
        const std::int32_t value = callbackWindow->ownerSession->WorldScene().replyRecordF7C950.values4C[attribute.index];
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
    if (callbackWindow->ownerSession->WorldScene().replyRecordF7C950.hasText24 != 0) {
        cg::text::FormatTo(buffer, LR"(能减少使用"{}"的魔)", callbackWindow->ownerSession->WorldScene().replyRecordF7C950.text25);
        drawText(28, 188, 0, buffer, TextCoordinateOrder::WindowBeforeElements);
        drawText(28, 204, 0, L"力消耗",
            TextCoordinateOrder::WindowBeforeElements);
    }
    return false;
}

// 原函数：0x0048B1A0。
std::int32_t WindowItem_DrawEquipmentTooltip(session::ClientSession& session, const std::int32_t index, const std::int32_t page)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& interaction = session.UiFrame().native.interaction;
    if (interaction.tooltipSource != index) {
        interaction.tooltipX = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX));
        interaction.tooltipY = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY));
        const bool left =
            (session.UiFrame().mouse.state->mouseX < 320);
        interaction.tooltipX = static_cast<std::uint16_t>(interaction.tooltipX + (left ? 60 : -257));
        const bool top =
            (session.UiFrame().mouse.state->mouseY < 240);
        interaction.tooltipY = static_cast<std::uint16_t>(interaction.tooltipY + (top ? 60 : -174));
    }
    const std::int8_t mode = std::bit_cast<std::int8_t>(WindowRuntime_Settings(session).inventoryActorSelection);
    interaction.tooltipSource = index;
    const bool petEquipment = mode > 0 && index < 6;
    const world::PlayerItemRecord* record;
    if (petEquipment) {
        const std::int32_t petIndex = WindowItemSelector_ReadSelectedPet(&inventory.normalSelector);
        const std::int32_t itemIndex = interaction.tooltipSource;
        record = &(*session.WorldScene().petRecords)[petIndex].items6E4[itemIndex];
    } else {
        record = &session.WorldScene().playerItems[index];
    }
    const std::uint16_t flags = record->flags;
    const std::wstring title = (flags & 0x100u) != 0 ? record->memo : record->name;
    const std::int8_t titleColor = (flags & 0x100u) != 0
        ? 5 : static_cast<std::int8_t>((flags & 0x200u) >> 9);
    const std::wstring* const lines = record->description.data() + page * 8;
    if (petEquipment) {
        const std::int16_t y = std::bit_cast<std::int16_t>(interaction.tooltipY);
        const std::int16_t x = std::bit_cast<std::int16_t>(interaction.tooltipX);
        return WindowItem_DrawTooltip(session, x, y, title.c_str(), titleColor, lines, 8, nullptr, 0);
    }
    const std::int16_t x = std::bit_cast<std::int16_t>(interaction.tooltipX);
    const std::int16_t y = std::bit_cast<std::int16_t>(interaction.tooltipY);
    return WindowItem_DrawTooltip(session, x, y, title.c_str(), titleColor, lines, 8, nullptr, 0);
}

// SPD01：原48B350完整65条；W32标题字段、四次鼠标CryptInt原序和八行描述/真实返回保持。
// 原函数：0x0048B350。
std::int32_t WindowItem_DrawDescriptionTooltip(session::ClientSession& session, const std::int32_t index, const std::int32_t page)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& storage = session.UiFrame().native.storage;
    const std::int32_t key = index + 100;
    if (interaction.tooltipSource != key) {
        interaction.tooltipX = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX));
        interaction.tooltipY = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY));
        const bool left =
            (session.UiFrame().mouse.state->mouseX < 320);
        interaction.tooltipX = static_cast<std::uint16_t>(interaction.tooltipX + (left ? 60 : -257));
        const bool top =
            (session.UiFrame().mouse.state->mouseY < 240);
        interaction.tooltipY = static_cast<std::uint16_t>(interaction.tooltipY + (top ? 60 : -174));
    }
    interaction.tooltipSource = key;
    const auto* const record = &storage.groups[0].items[index];
    std::uint32_t flags;
    std::memcpy(&flags, &record->flags, sizeof(flags));
    const std::wstring title = (flags & 0x100u) != 0 ? record->memo : record->name;
    const std::int8_t titleColor = (flags & 0x100u) != 0
        ? 5 : static_cast<std::int8_t>((flags >> 9) & 1u);
    const auto* const lines = record->description.data() + page * 8;
    const std::int16_t x = std::bit_cast<std::int16_t>(interaction.tooltipX);
    const std::int16_t y = std::bit_cast<std::int16_t>(interaction.tooltipY);
    return WindowItem_DrawTooltip(session, x, y, title.c_str(), titleColor, lines, 8, nullptr, 0);
}

// 原函数：0x0048B750。
std::int32_t WindowItem_DrawWorkTooltip(session::ClientSession& session, const std::int32_t page)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& shop = session.UiFrame().native.shop;
    if (interaction.tooltipSource != 400) {
        interaction.tooltipX = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseX));
        interaction.tooltipY = static_cast<std::uint16_t>(
            static_cast<std::int32_t>(session.UiFrame().mouse.state->mouseY));
        const bool left =
            (session.UiFrame().mouse.state->mouseX < 320);
        interaction.tooltipX = static_cast<std::uint16_t>(interaction.tooltipX + (left ? 60 : -257));
        const bool top =
            (session.UiFrame().mouse.state->mouseY < 240);
        interaction.tooltipY = static_cast<std::uint16_t>(interaction.tooltipY + (top ? 60 : -174));
    }
    const std::int16_t y = std::bit_cast<std::int16_t>(interaction.tooltipY);
    const std::int16_t x = std::bit_cast<std::int16_t>(interaction.tooltipX);
    interaction.tooltipSource = 400;
    const auto& title = shop.detail.name;
    return WindowItem_DrawTooltip(
        session,
        x, y, title.c_str(), 0, shop.detail.description.data() + page * 8,
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
static std::int32_t WindowRuntime_IncrementNameMode(session::ClientSession& session)
{
    const auto mode = static_cast<std::int16_t>(platform::gActorOverheadTextMode_5F974C + 1);
    const bool pastLastMode = mode >= 7;
    platform::gActorOverheadTextMode_5F974C = mode;
    if (pastLastMode) {
        platform::gActorOverheadTextMode_5F974C = 0;
    }
    static_cast<void>(login::detail::CaptureClientPreferenceBytes_520590(session));
    return login::detail::SaveCurrentLoginRecord_520200(session);
}

// CWP01：原71项默认坐标/设置与返回保持，地址索引仅作原生寻址。
// 原函数：0x004C9240。
static std::int32_t WindowRuntime_ResetSettings(session::ClientSession& session)
{
    auto& states = WindowRuntime_States(session);
    auto& settings = WindowRuntime_Settings(session);
    for (std::size_t index = 0; index < kWindowCount; ++index) {
        const WindowPrototype* const prototype = &WindowRuntime_Prototype(session, index);
        states[index].x = prototype->defaultX;
        states[index].y = prototype->defaultY;
    }
    settings.hotkeyLayoutMode = 1;
    settings.inventoryActorSelection = 0xFF;
    settings.battleItemActorSelection = 0xFF;
    settings.chatWidthUnits = 12;
    settings.chatHeightUnits = 4;
    settings.statusBarMode = 0;
    settings.shortcutBarCollapsed = 0;
    states[WindowId_MiniMap].marker50 = 0;
    states[WindowId_Emote].marker50 = 0;
    settings.contactsPreviewMode = 0;
    return 0;
}

// 原函数：0x004C93A0。
static std::int32_t WindowRuntime_HasChangedDefaultPositions(session::ClientSession& session)
{
    auto& savedData = session.UiFrame().native.savedData;
    if (savedData.records[1].x != gWindowPrototype54E2A0.defaultX) {
        return 1;
    }
    if (savedData.records[4].x != gWindowPrototype54E240.defaultX) {
        return 1;
    }
    return savedData.records[6].x != gWindowPrototype54E330.defaultX ? 1 : 0;
}

// 原函数：0x004C93E0。
static std::int32_t WindowRuntime_RepairSettingsTail(session::ClientSession& session)
{
    auto& savedData = session.UiFrame().native.savedData;
    auto& states = WindowRuntime_States(session);
    const std::int32_t fileLength = session.UiFrame().native.savedLength;
    if (fileLength == 0x521C) {
        return 1;
    }
    const std::int32_t missingBytes = 0x521C - fileLength;
    const std::uint32_t first =
        (static_cast<std::uint32_t>(fileLength)
            - static_cast<std::uint32_t>(savedData.version)) / 0x128u;
    const std::uint32_t count = static_cast<std::uint32_t>(missingBytes) / 0x128u;
    if (missingBytes < 0 || static_cast<std::uint32_t>(missingBytes) % 0x128u != 0) {
        return 0;
    }
    const std::uint32_t end = first + count;
    if (std::bit_cast<std::int32_t>(first) < std::bit_cast<std::int32_t>(end)) {
        for (std::uint32_t offset = 0; offset < count; ++offset) {
            const std::uint32_t index = first + offset + 1u;
            const WindowPrototype* const prototype = &WindowRuntime_Prototype(session, index);
            states[index].x = prototype->defaultX;
            states[index].y = prototype->defaultY;
        }
    }
    return 1;
}

// CWP01：原两设置BYTE现场读取，完整S32字段/交错发布与返回保持。
// 原函数：0x004C9170。
static std::int32_t WindowRuntime_ApplySettings(session::ClientSession& session, const bool restoreCoordinates)
{
    auto& savedData = session.UiFrame().native.savedData;
    auto& states = WindowRuntime_States(session);
    auto& settings = WindowRuntime_Settings(session);
    for (std::size_t index = 0; index < kWindowCount; ++index) {
        const WindowPrototype* const prototype = &WindowRuntime_Prototype(session, index);
        states[index].x = restoreCoordinates && index < kSavedWindowCount
            ? savedData.records[index].x : prototype->defaultX;
        states[index].y = restoreCoordinates && index < kSavedWindowCount
            ? savedData.records[index].y : prototype->defaultY;
    }
    static_cast<void>(WindowRuntime_RepairSettingsTail(session));
    const auto firstByte = *reinterpret_cast<const unsigned char*>(&savedData.records[14].value08);
    const std::int32_t valueCF4AAC = savedData.records[24].value08;
    const auto secondByte = *reinterpret_cast<const unsigned char*>(&savedData.records[14].value0C);
    settings.inventoryActorSelection = firstByte;
    const std::int32_t width = savedData.records[35].value08;
    settings.hotkeyLayoutMode = valueCF4AAC;
    const std::int32_t mode = savedData.records[35].value10;
    settings.chatWidthUnits = width;
    const std::int32_t valueCF4690 = savedData.records[1].value08;
    settings.battleItemActorSelection = secondByte;
    const std::int32_t height = savedData.records[35].value0C;
    settings.chatMode = mode;
    const std::int32_t actorBars = savedData.records[1].value0C;
    settings.statusBarMode = valueCF4690;
    settings.chatHeightUnits = height;
    const std::int32_t valueCA0440 = savedData.records[4].value08;
    settings.actorBarsEnabled = actorBars == 1 ? 1u : 0u;
    const std::int32_t marker33 = savedData.records[33].value08;
    settings.shortcutBarCollapsed = valueCA0440;
    const std::int32_t marker34 = savedData.records[34].value08;
    states[WindowId_MiniMap].marker50 = static_cast<std::uint32_t>(marker33);
    const std::int32_t valueD67B38 = savedData.records[21].value08;
    states[WindowId_Emote].marker50 = static_cast<std::uint32_t>(marker34);
    settings.contactsPreviewMode = valueD67B38;
    return 1;
}

// 原函数：0x004C9060。
static std::int32_t WindowRuntime_LoadSettings(session::ClientSession& session)
{
    auto& savedData = session.UiFrame().native.savedData;
    static_cast<void>(WindowRuntime_ResetSettings(session));
    const auto& rolePath = session.UiFrame().native.savedSettingsPath;
    const wchar_t* path = rolePath.empty() ? L"data\\WinSaveDate" : rolePath.c_str();
    bool restoreCoordinates = !rolePath.empty();
    std::FILE* stream = _wfopen(path, L"rb+");
    if (stream == nullptr && restoreCoordinates && errno == ENOENT) {
        path = L"data\\WinSaveDate";
        restoreCoordinates = false;
        stream = _wfopen(path, L"rb+");
    }
    if (stream == nullptr) {
        static_cast<void>(WindowRuntime_ResetSettings(session));
        return 0;
    }
    // 原0x0053D435：同一MSVC描述符的创建／读写及权限。
    const std::int32_t descriptor = ::_wopen(
        path, _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE);
    if (descriptor != -1) {
        // 原0x0053D3BD：保留S32文件长度。
        session.UiFrame().native.savedLength = static_cast<std::int32_t>(::_filelength(descriptor));
        // 原0x0053D30A：关闭上述描述符，原返回值不消费。
        static_cast<void>(::_close(descriptor));
        // 原4C90B8/4C90D4均读取flags低U8，只有20H是错误。
        if (std::ferror(stream) == 0) {
            static_cast<void>(static_cast<std::uint32_t>(std::fread(
                &savedData, 1, 0x521C, stream)));
            if (std::ferror(stream) == 0) {
                if (savedData.version != 0x13
                    || (!restoreCoordinates && WindowRuntime_HasChangedDefaultPositions(session) == 1)
                    || WindowRuntime_ApplySettings(session, restoreCoordinates) == 0) {
                    static_cast<void>(WindowRuntime_ResetSettings(session));
                }
                static_cast<void>(std::fclose(stream));
                return 1;
            }
        }
    }
    static_cast<void>(WindowRuntime_ResetSettings(session));
    static_cast<void>(std::fclose(stream));
    return 0;
}

// 原函数：0x004C92B0。
static std::int32_t WindowRuntime_ClearSavedSettings(session::ClientSession& session)
{
    auto& savedData = session.UiFrame().native.savedData;
    std::memset(&savedData, 0, sizeof(savedData));
    return 0;
}

// 原函数：0x004C92D0。
static std::uint32_t WindowRuntime_CaptureSettings(session::ClientSession& session)
{
    auto& savedData = session.UiFrame().native.savedData;
    const auto& states = WindowRuntime_States(session);
    const auto& settings = WindowRuntime_Settings(session);
    static_cast<void>(WindowRuntime_ClearSavedSettings(session));
    savedData.version = 0x13;
    for (std::size_t index = 0; index < kSavedWindowCount; ++index) {
        const auto* const window = states[index].window;
        savedData.records[index].x = window != nullptr ? window->x : states[index].x;
        savedData.records[index].y = window != nullptr ? window->y : states[index].y;
    }
    const std::int32_t firstByte = std::bit_cast<std::int8_t>(settings.inventoryActorSelection);
    const std::int32_t secondByte = std::bit_cast<std::int8_t>(settings.battleItemActorSelection);
    const std::int32_t valueCF4AAC = settings.hotkeyLayoutMode;
    savedData.records[14].value08 = firstByte;
    const std::int32_t width = settings.chatWidthUnits;
    savedData.records[14].value0C = secondByte;
    const std::int32_t height = settings.chatHeightUnits;
    savedData.records[35].value08 = width;
    const std::int32_t valueCF4690 = settings.statusBarMode;
    savedData.records[24].value08 = valueCF4AAC;
    const std::int32_t mode = settings.chatMode;
    savedData.records[1].value08 = valueCF4690;
    const std::uint32_t actorBars = settings.actorBarsEnabled;
    savedData.records[35].value0C = height;
    const std::uint32_t marker33 = states[WindowId_MiniMap].marker50;
    savedData.records[35].value10 = mode;
    const std::int32_t valueCA0440 = settings.shortcutBarCollapsed;
    savedData.records[33].value08 = std::bit_cast<std::int32_t>(marker33);
    savedData.records[1].value0C = actorBars == 1 ? 1 : 0;
    const std::uint32_t marker34 = states[WindowId_Emote].marker50;
    savedData.records[4].value08 = valueCA0440;
    const std::int32_t valueD67B38 = settings.contactsPreviewMode;
    savedData.records[34].value08 = std::bit_cast<std::int32_t>(marker34);
    savedData.records[21].value08 = valueD67B38;
    return marker33;
}

// 原函数：0x004C9130。
std::int32_t WindowRuntime_SaveSettings(session::ClientSession& session)
{
    auto& savedData = session.UiFrame().native.savedData;
    const auto& path = session.UiFrame().native.savedSettingsPath;
    if (path.empty()) return 1;
    std::error_code error;
    static_cast<void>(std::filesystem::create_directories(std::filesystem::path(path).parent_path(), error));
    if (error) {
        m_log_iw(L"无法创建角色布局目录，错误 %d。", error.value());
        return 1;
    }
    std::FILE* const stream =
        _wfopen(path.c_str(), L"wb+");
    if (stream != nullptr) {
        static_cast<void>(WindowRuntime_CaptureSettings(session));
        static_cast<void>(static_cast<std::uint32_t>(std::fwrite(
            &savedData, 1, 0x521C, stream)));
        static_cast<void>(std::fclose(stream));
    } else {
        m_log_iw(L"无法打开角色布局保存文件，错误 %d。", errno);
    }
    return 1;
}

// 原函数：0x00472200。
static void WindowRuntime_ClearSavedWindowMarkers(session::ClientSession& session)
{
    for (WindowState& state : WindowRuntime_States(session)) {
        state.marker50 = 0;
    }
}

// 原函数：0x00472220。
static void WindowRuntime_MarkExistingWindows(session::ClientSession& session)
{
    for (WindowState& state : WindowRuntime_States(session)) {
        if (state.window != nullptr) {
            state.marker50 = 1;
        }
    }
}

// 原函数：0x004722A0。
static void WindowRuntime_ResetMouseAndItemFlags(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& pointer = session.UiFrame().native.pointer;
    inventory.slotUseCounts.fill(0);
    pointer.kind = 0;
    pointer.sourceIndex = 0;
    pointer.callback = nullptr;
    pointer.y = 0;
    pointer.x = 0;
    pointer.focusWindowId = 0;
    pointer.focusElementId = 0;
    pointer.phase = 0;
}

// 原函数：0x004722E0。
static void WindowRuntime_ClearElementFocus(session::ClientSession& session)
{
    session.UiFrame().native.pointer.focusedElementData = nullptr;
}

// 原函数：0x004722F0。
static void WindowRuntime_RestoreMarkedWindows(session::ClientSession& session)
{
    auto& states = WindowRuntime_States(session);
    for (std::int32_t index = 0; index < static_cast<std::int32_t>(kWindowCount); ++index) {
        if (states[index].marker50 != 1) {
            continue;
        }
        switch (index) {
        case 21:
        case 22:
        case 23:
        case 40:
            if (states[WindowId_Contacts].marker50 != 0) {
                static_cast<void>(WindowRuntime_Open(session, WindowId_Contacts));
                states[WindowId_Contacts].marker50 = 0;
            }
            if (states[WindowId_MailRead].marker50 != 0) {
                const std::int32_t position = session.WorldScene().mail.data->viewedMessageIndex;
                static_cast<void>(WindowRuntime_Open(session, WindowId_MailRead));
                session.WorldScene().mail.data->viewedMessageIndex = position;
                states[WindowId_MailRead].marker50 = 0;
            }
            if (states[WindowId_MailCompose].marker50 != 0) {
                static_cast<void>(WindowMailCompose_Open(session));
                states[WindowId_MailCompose].marker50 = 0;
            }
            if (states[WindowId_PetMailCompose].marker50 != 0) {
                static_cast<void>(WindowRuntime_Open(session, WindowId_PetMailCompose));
                states[WindowId_PetMailCompose].marker50 = 0;
            }
            break;
        case 33:
        case 34:
            static_cast<void>(WindowRuntime_Open(session, index));
            states[index].marker50 = 0;
            break;
        default:
            states[index].marker50 = 0;
            break;
        }
    }
}

// 原函数：0x00479640。
static void WindowRuntime_ResetOperationCount(session::ClientSession& session)
{
    auto& operations = session.UiFrame().native.operations;
    operations.count = 0;
}

// 原函数：0x004796C0。
static void WindowRuntime_ResetGatherOperationCount(session::ClientSession& session)
{
    auto& operations = session.UiFrame().native.operations;
    operations.gatherCount = 0;
}

// 原函数：0x004796F0。
static void WindowRuntime_ResetSkillOperationCount(session::ClientSession& session)
{
    auto& operations = session.UiFrame().native.operations;
    operations.skillCount = 0;
}

// 原函数：0x00479610。
static void WindowRuntime_ResetOperationReferences(session::ClientSession& session)
{
    auto& operations = session.UiFrame().native.operations;
    operations.craftOpening = nullptr;
    operations.gatherOpening = nullptr;
    WindowRuntime_ResetOperationCount(session);
    WindowRuntime_ResetGatherOperationCount(session);
    WindowRuntime_ResetSkillOperationCount(session);
    operations.statusIconReferences = 0;
}

// 原函数：0x004C7AD0。
static void WindowRuntime_ResetShortcutFlag(session::ClientSession& session)
{
    WindowRuntime_Settings(session).hotkeyLayoutMode = 0;
}

// 原函数：0x00475C10。
static void WindowRuntime_RestorePositions(session::ClientSession& session)
{
    for (std::size_t index = 0; index < kWindowCount; ++index) {
        WindowState& state = WindowRuntime_States(session)[index];
        WindowInstance* window = state.window;
        if (window == nullptr) {
            continue;
        }
        const WindowPrototype* const prototype = &WindowRuntime_Prototype(session, index);
        const bool isPrimaryOffsetWindow = index == 10;
        window->x = prototype->defaultX;
        window = state.window;
        window->y = prototype->defaultY;
        if (!isPrimaryOffsetWindow && index != 11 && index != 12 && index != 14 && index != 27) {
            if (index == WindowId_Chat) {
                window->x = platform::gWindowWidth - (WindowRuntime_Settings(session).chatWidthUnits * 12 + 123) - 9;
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

static void WindowRuntime_ToggleShortcutWindow(session::ClientSession& session, const std::int32_t id)
{
    WindowInstance* const window = WindowRuntime_States(session)[id].window;
    if (window == nullptr) {
        static_cast<void>(WindowRuntime_CreateAnimation(session, id, 1, 0, nullptr));
    } else {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
    }
}

static void WindowRuntime_PlayShortcutEmote(session::ClientSession& session, const std::int32_t action, const std::int32_t selected)
{
    world::RequestPlayerEmote_4FA8E0(session, action);
    static_cast<void>(WindowEmote_Select(session, selected));
    static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
}

// 原函数：0x004C7AE0。
static void WindowRuntime_ProcessShortcuts(session::ClientSession& session)
{
    auto& chatDisplay = session.WorldScene().chatDisplay;
    auto& chatWindow = session.UiFrame().native.chat;
    if ((platform::ReadSessionKeyState(session, 0x1B) & 4u) != 0) {
        session.UiFrame().keyStates[0x1B] &= static_cast<std::uint8_t>(~4u);
        if (script::CloseTopmostNonDefaultWindow(session)) {
            static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
        } else {
            WindowRuntime_ToggleShortcutWindow(session, WindowId_Settings);
        }
        return;
    }
    const std::int32_t mode = WindowRuntime_Settings(session).hotkeyLayoutMode;
    if (mode != 0 && mode != 1) {
        return;
    }
    const bool modeOne = mode == 1;
    if ((platform::ReadSessionKeyState(session, 0x23) & 4u) != 0) {
        WindowRuntime_ToggleShortcutWindow(session, 65);
    }
    if ((platform::ReadSessionKeyState(session, 0x78) & 4u) != 0) {
        WindowRuntime_ToggleShortcutWindow(session, 66);
    }
    if ((platform::ReadSessionKeyState(session, 0x11) & 2u) != 0) {
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x51 : 0x57) & 4u) != 0) {
            constexpr std::int32_t ids[] {7, 8, 9, 58};
            bool found = false;
            for (const std::int32_t id : ids) {
                WindowInstance* const window = WindowRuntime_States(session)[id].window;
                if (window != nullptr) {
                    found = true;
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
                }
            }
            if (!found) {
                static_cast<void>(WindowRuntime_CreateAnimation(session, WindowId_PlayerProfile, 1, 0, nullptr));
            }
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x57 : 0x45) & 4u) != 0) {
            if (session.WorldScene().sceneType == 10) {
                WindowBattleCommand_OpenSkills(session);
            } else {
                WindowInstance* const window = WindowRuntime_States(session)[WindowId_Skill].window;
                if (window == nullptr) {
                    static_cast<void>(WindowSkill_Open(session));
                } else {
                    window->state |= 0x8000u;
                    static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
                }
            }
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x45 : 0x52) & 4u) != 0) {
            if (session.WorldScene().sceneType == 10) {
                WindowBattleCommand_OpenItems(session);
            } else {
                WindowRuntime_ToggleShortcutWindow(session, 14);
            }
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x52 : 0x41) & 4u) != 0) {
            if (session.WorldScene().sceneType == 10) {
                WindowBattleCommand_OpenPets(session);
            } else {
                WindowRuntime_ToggleShortcutWindow(session, 15);
            }
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x54 : 0x53) & 4u) != 0) {
            WindowRuntime_ToggleShortcutWindow(session, 21);
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x59 : 0x44) & 4u) != 0) {
            WindowRuntime_ToggleShortcutWindow(session, 19);
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x41 : 0x46) & 4u) != 0
            && session.WorldScene().sceneType != 10) {
            WindowRuntime_ToggleShortcutWindow(session, 34);
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x53 : 0x51) & 4u) != 0
            && session.WorldScene().sceneType != 10) {
            WindowRuntime_ToggleShortcutWindow(session, 33);
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x44 : 0x59) & 4u) != 0) {
            WindowRuntime_ToggleShortcutWindow(session, 25);
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x5A : 0x4F) & 4u) != 0) {
            static_cast<void>(WindowRuntime_CycleNameMode());
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x26) & 4u) != 0) {
            WindowChat_RequestHeightIncrease(session);
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x28) & 4u) != 0) {
            WindowChat_RequestHeightDecrease(session);
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x27) & 4u) != 0) {
            WindowChat_RequestWidthDecrease(session);
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x25) & 4u) != 0) {
            WindowChat_RequestWidthIncrease(session);
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x55 : 0x50) & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
            static_cast<void>(world::ToggleDuelPermission_472F40(session));
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x49 : 0xBA) & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
            static_cast<void>(world::TogglePartyChat_472F70(session));
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x4F : 0xBE) & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
            static_cast<void>(world::TogglePartyJoinPermission_472FA0(session));
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x50 : 0x4C) & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
            static_cast<void>(world::ToggleContactCardExchangePermission_472FD0(session));
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0x46 : 0xBC) & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
            static_cast<void>(world::ToggleTradePermission_473000(session));
        }
        if ((platform::ReadSessionKeyState(session, modeOne ? 0xDB : 0x4E) & 4u) != 0) {
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
            static_cast<void>(world::ToggleGuildInvitePermission_473030(session));
        }

        // 该门禁只采一次场景，覆盖全部请求和动作；门内回调不触发再次判场景。
        if (session.WorldScene().sceneType != 10) {
            if ((platform::ReadSessionKeyState(session, modeOne ? 0x4A : 0x4B) & 4u) != 0) {
                world::RequestDuelWithFacingPlayer_470050(session);
            }
            if ((platform::ReadSessionKeyState(session, modeOne ? 0x4B : 0x56) & 4u) != 0) {
                world::RequestSpectateFacingBattle_470150(session);
            }
            if (!modeOne && (platform::ReadSessionKeyState(session, 0x48) & 4u) != 0) {
                WindowRuntime_PlayShortcutEmote(session, 9, 13);
            }
            if ((platform::ReadSessionKeyState(session, modeOne ? 0x4C : 0xBF) & 4u) != 0) {
                world::RequestPartyJoinOrLeave_470270(session);
            }
            if ((platform::ReadSessionKeyState(session, modeOne ? 0xBA : 0xDB) & 4u) != 0) {
                world::RequestFacingPlayerContactCard_4703C0(session);
            }
            if ((platform::ReadSessionKeyState(session, modeOne ? 0xDE : 0xDD) & 4u) != 0) {
                world::RequestNearbyPlayerTrade_4704A0(session);
            }
            if ((platform::ReadSessionKeyState(session, modeOne ? 0xDD : 0x4D) & 4u) != 0) {
                world::RequestFacingPlayerGuildInvitation_470670(session);
            }
            for (const ShortcutEmoteBinding& binding : kShortcutEmotes) {
                const std::uint8_t key = modeOne ? binding.keyMode1 : binding.keyMode0;
                if ((platform::ReadSessionKeyState(session, key) & 4u) != 0) {
                    WindowRuntime_PlayShortcutEmote(session, binding.action, binding.selected);
                }
            }
            if ((platform::ReadSessionKeyState(session, 0xDC) & 4u) != 0) {
                const std::int32_t remainder = platform::Rand16() % 3;
                if (remainder == 0) {
                    WindowRuntime_PlayShortcutEmote(session, 14, 7);
                } else if (remainder == 1) {
                    WindowRuntime_PlayShortcutEmote(session, 16, 8);
                } else {
                    WindowRuntime_PlayShortcutEmote(session, 17, 17);
                }
            }
        }
        if ((platform::ReadSessionKeyState(session, 0x7B) & 4u) != 0) {
            WindowRuntime_RestorePositions(session);
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x7A) & 4u) != 0) {
            auto& enabled = WindowRuntime_Settings(session).actorBarsEnabled;
            enabled = enabled != 1 ? 1u : 0u;
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x24) & 4u) != 0) {
            static_cast<void>(WindowChat_SwitchMode(session));
        }
        return;
    }
    if ((platform::ReadSessionKeyState(session, 0x10) & 2u) != 0) {
        if ((platform::ReadSessionKeyState(session, 0x7B) & 4u) != 0) {
            WindowRuntime_CloseByCategory(session, 7);
            static_cast<void>(audio::PlaySe_52C450(session, 57, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x24) & 4u) != 0) {
            if (session.UiFrame().inputState.chatPanelVisible == 1) {
                session.UiFrame().inputState.chatPanelVisible = 0;
                session.UiFrame().inputState.chatLogSuppressed = 1;
            } else {
                session.UiFrame().inputState.chatPanelVisible = 1;
                session.UiFrame().inputState.chatLogSuppressed = 0;
            }
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x27) & 4u) != 0) {
            const std::int32_t range = chatDisplay.range + 1;
            const bool pastLastRange = range > 5;
            chatDisplay.range = range;
            if (pastLastRange) {
                chatDisplay.range = 1;
            }
            chatDisplay.rangeOverlayFrames = 180;
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x25) & 4u) != 0) {
            const std::int32_t range = chatDisplay.range - 1;
            const bool belowFirstRange = range < 1;
            chatDisplay.range = range;
            if (belowFirstRange) {
                chatDisplay.range = 5;
            }
            chatDisplay.rangeOverlayFrames = 180;
            static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        }
        if ((platform::ReadSessionKeyState(session, 0x26) & 0x10u) != 0) {
            const std::int32_t previous = chatWindow.scrollOffset;
            const std::int32_t count = chatDisplay.count;
            const std::int32_t scroll = previous + 1;
            const bool pastChatCount = scroll > count;
            chatWindow.scrollOffset = scroll;
            if (pastChatCount) {
                chatWindow.scrollOffset = count;
                static_cast<void>(audio::PlaySe_52C450(session, 55, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
            }
        }
        if ((platform::ReadSessionKeyState(session, 0x28) & 0x10u) != 0) {
            const std::int32_t scroll = chatWindow.scrollOffset - 1;
            const bool belowFirstChatLine = scroll < 0;
            chatWindow.scrollOffset = scroll;
            if (belowFirstChatLine) {
                chatWindow.scrollOffset = 0;
                static_cast<void>(audio::PlaySe_52C450(session, 55, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
            }
        }
        return;
    }
    if ((platform::ReadSessionKeyState(session, 0x7A) & 4u) != 0) {
        static_cast<void>(WindowRuntime_IncrementNameMode(session));
        static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
    }
    if ((platform::ReadSessionKeyState(session, 0x7B) & 4u) != 0) {
        static_cast<void>(render::TakeScreenshotThrottled_503C80(session));
        static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
    }
    if ((platform::ReadSessionKeyState(session, 0x24) & 4u) != 0) {
        static_cast<void>(audio::PlaySe_52C450(session, 51, 320));
        static_cast<void>(WindowChat_ClearHistory(session));
    }
}

// 三条重开共用的两轮窄清理；不释放实例，也不清记录的其他字段。
static void WindowRuntime_ResetReferences(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    WindowRuntime_Windows(session).fill(nullptr);
    for (WindowState& state : WindowRuntime_States(session)) {
        state.window = nullptr;
    }
    WindowRuntime_ResetMouseAndItemFlags(session);
    pointer.currentWindowId = 0;
    pointer.currentElementId = 0;
}

static void WindowRuntime_OpenConfiguredWindows(session::ClientSession& session)
{
    static_cast<void>(WindowRuntime_Open(session, WindowId_BottomBar));
    switch (WindowRuntime_Settings(session).statusBarMode) {
    case 0:
        static_cast<void>(WindowRuntime_Open(session, WindowId_StatusBar));
        break;
    case 1:
        static_cast<void>(WindowRuntime_Open(session, WindowId_CompactStatusBar));
        break;
    case 2:
        static_cast<void>(WindowRuntime_Open(session, WindowId_CollapsedStatusBar));
        break;
    default:
        break;
    }
    switch (WindowRuntime_Settings(session).shortcutBarCollapsed) {
    case 0:
        static_cast<void>(WindowRuntime_Open(session, WindowId_ShortcutBar));
        break;
    case 1:
        static_cast<void>(WindowRuntime_Open(session, WindowId_CollapsedShortcutBar));
        break;
    default:
        break;
    }
    switch (WindowRuntime_Settings(session).chatMode) {
    case 0:
        static_cast<void>(WindowRuntime_Open(session, WindowId_Chat));
        break;
    case 1:
        static_cast<void>(WindowRuntime_Open(session, WindowId_CompactChat));
        break;
    default:
        break;
    }
}

// 原函数：0x004707C0。
static void WindowRuntime_OpenLoginScene(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& interaction = session.UiFrame().native.interaction;
    WindowRuntime_ResetReferences(session);
    WindowRuntime_OpenConfiguredWindows(session);
    WindowRuntime_RestoreMarkedWindows(session);
    WindowRuntime_ClearElementFocus(session);
    inventory.selectedItem = -1;
    session.UiFrame().native.itemTargets.references = 0;
    WindowRuntime_ResetOperationReferences(session);
    interaction.previewAction = nullptr;
    session.UiFrame().inputState.chatPanelVisible = 1;
    static_cast<void>(WindowChat_RestoreInput(session));
}

// 原函数：0x00471170。
static void WindowRuntime_OpenBattleScene(session::ClientSession& session)
{
    auto& interaction = session.UiFrame().native.interaction;
    auto& states = WindowRuntime_States(session);
    WindowRuntime_ResetReferences(session);
    WindowRuntime_ClearElementFocus(session);
    static_cast<void>(WindowRuntime_Open(session, WindowId_BottomBar));
    static_cast<void>(WindowRuntime_Open(session, WindowId_CompactStatusBar));
    switch (WindowRuntime_Settings(session).chatMode) {
    case 0:
        static_cast<void>(WindowRuntime_Open(session, WindowId_Chat));
        break;
    case 1:
        static_cast<void>(WindowRuntime_Open(session, WindowId_CompactChat));
        break;
    default:
        break;
    }
    if (states[WindowId_Contacts].marker50 != 0) {
        static_cast<void>(WindowRuntime_Open(session, WindowId_Contacts));
        states[WindowId_Contacts].marker50 = 0;
    }
    if (states[WindowId_MailRead].marker50 != 0) {
        const std::int32_t position = session.WorldScene().mail.data->viewedMessageIndex;
        static_cast<void>(WindowRuntime_Open(session, WindowId_MailRead));
        session.WorldScene().mail.data->viewedMessageIndex = position;
        states[WindowId_MailRead].marker50 = 0;
    }
    if (states[WindowId_MailCompose].marker50 != 0 || states[WindowId_PetMailCompose].marker50 != 0) {
        static_cast<void>(WindowRuntime_Open(session, WindowId_MailCompose));
        states[WindowId_MailCompose].marker50 = 0;
        states[WindowId_PetMailCompose].marker50 = 0;
    }
    WindowRuntime_ResetOperationReferences(session);
    WindowInstance* const window22 = states[WindowId_MailCompose].window;
    interaction.previewAction = nullptr;
    session.UiFrame().inputState.chatPanelVisible = 1;
    if (window22 == nullptr && states[WindowId_PetMailCompose].window == nullptr) {
        static_cast<void>(WindowChat_RestoreInput(session));
    }
}

static void WindowRuntime_MarkSceneWindows(session::ClientSession& session)
{
    for (const std::int32_t id : std::array<std::int32_t, 4>{21, 40, 22, 23}) {
        if (WindowRuntime_States(session)[id].window != nullptr) {
            WindowRuntime_States(session)[id].marker50 = 1;
        }
    }
}

// 原函数：0x00470980。
static void WindowRuntime_OpenNormalScene(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& interaction = session.UiFrame().native.interaction;
    WindowRuntime_ResetReferences(session);
    WindowRuntime_ClearElementFocus(session);
    WindowRuntime_OpenConfiguredWindows(session);
    WindowRuntime_RestoreMarkedWindows(session);
    inventory.selectedItem = -1;
    session.UiFrame().native.itemTargets.references = 0;
    WindowRuntime_ResetOperationReferences(session);
    const std::int32_t inputGate = session.UiFrame().inputState.chatLogSuppressed;
    interaction.previewAction = nullptr;
    if (inputGate == 0) {
        session.UiFrame().inputState.chatPanelVisible = 1;
    }
    if (WindowRuntime_States(session)[WindowId_MailCompose].window == nullptr && WindowRuntime_States(session)[WindowId_PetMailCompose].window == nullptr) {
        static_cast<void>(WindowChat_RestoreInput(session));
    }
}

// 原函数：0x004705D0。
std::int32_t WindowRuntime_Initialize(session::ClientSession& session)
{
    static_cast<void>(WindowChat_ResetState(session));
    static_cast<void>(WindowRuntime_LoadSettings(session));
    WindowRuntime_ClearSavedWindowMarkers(session);
    return world::detail::LoadProfileCategoryFile_534910(session);
}

// 原函数：0x004923D0。
static std::int32_t WindowRuntime_ResetAlbumState(session::ClientSession& session)
{
    auto& business = session.UiFrame().native.business;
    business.valueCF4A00 = 5;
    business.valueC8A95C = 0;
    business.valueCB2200 = 0;
    return 0;
}

// 原函数：0x004742B0。
static std::int32_t WindowScroll_ResetPosition(session::ClientSession& session)
{
    auto& business = session.UiFrame().native.business;
    business.valuesCF4E50[0] = 0;
    business.valuesCF4E50[1] = 0;
    return 0;
}

// 原函数：0x00470600。
static void WindowRuntime_InitializeLogin(session::ClientSession& session)
{
    auto& business = session.UiFrame().native.business;
    auto& states = WindowRuntime_States(session);
    const auto& request = session.Connection().LoginRequests();
    const auto& character = request.characters[static_cast<std::size_t>(request.selectedCharacterIndex)];
    const auto accountKey = login::detail::EncodeSavedAccountName(request.account);
    session.UiFrame().native.savedSettingsPath = accountKey.empty() ? std::wstring {}
        : std::format(L"{}\\layout\\{}\\g{}_s{}_r{}\\{}x{}\\WinSaveDate", platform::gDataPath,
            accountKey, request.selectedLoginServerGroup, request.selectedServerIndex,
            character.registNumber, platform::gWindowWidth, platform::gWindowHeight);
    static_cast<void>(WindowRuntime_LoadSettings(session));
    static_cast<void>(WindowRuntime_ResetAlbumState(session));
    WindowMessageBoard_ResetRecords(session);
    static_cast<void>(WindowChat_ResetScrollRequests(session));
    static_cast<void>(WindowMail_ResetComposeState(session));
    static_cast<void>(WindowPetList_ResetSelection(session));
    static_cast<void>(WindowScroll_ResetPosition(session));
    world::detail::ResetMiniMailSequenceAndSelection_4C9490(session);
    world::detail::ClearMiniMailAndProfiles_533910(session);
    if (world::detail::LoadPlayerProfilesFile_5347F0(session) == 0) {
        world::detail::InitializeDefaultProfiles_533950(session);
    }
    static_cast<void>(WindowMail_ResetSelectionState(session));
    states[WindowId_Contacts].marker50 = 0;
    states[WindowId_MailCompose].marker50 = 0;
    states[WindowId_PetMailCompose].marker50 = 0;
    states[WindowId_MailRead].marker50 = 0;
    static_cast<void>(WindowBlacklist_ResetState(session));
    WindowAdventureLog_InitializeDate(session);
    business.valueD67618 = 0;
    business.bankCurrentPage = 0; // G07：与原银行入口同一生命周期，金额组仍独立。
}

// 原函数：0x00477490。
WindowInstance* WindowRuntime_FindById(session::ClientSession& session, const std::int32_t id)
{
    for (std::size_t index = 0; index < WindowRuntime_Windows(session).size(); ++index) {
        WindowInstance* const window = WindowRuntime_Windows(session)[index];
        if (window->id == id) {
            return WindowRuntime_Windows(session)[index];
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
std::int32_t WindowRuntime_DrawDefault(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowRuntime_DrawCurrent(callbackWindow);
    return 1;
}

// 原函数：0x0049BDB0。
std::int32_t WindowRuntime_DrawCompactFrame(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowRuntime_DrawCurrent(callbackWindow);
    WindowInstance* const window = &callbackWindow;
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
void WindowRuntime_IncrementGatherOperationCount(session::ClientSession& session)
{
    auto& operations = session.UiFrame().native.operations;
    ++operations.gatherCount;
}

// 原函数：0x004796E0。
void WindowRuntime_DecrementGatherOperationCount(session::ClientSession& session)
{
    auto& operations = session.UiFrame().native.operations;
    --operations.gatherCount;
}

// 原函数：0x00479650。
void WindowRuntime_IncrementOperationCount(session::ClientSession& session)
{
    auto& operations = session.UiFrame().native.operations;
    ++operations.count;
}

// 原函数：0x00479660。
void WindowRuntime_DecrementOperationCount(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& skillOperation = session.UiFrame().native.skillOperation;
    auto& operations = session.UiFrame().native.operations;
    const std::int32_t remainingCount = operations.count - 1;
    operations.count = remainingCount;
    if (remainingCount != 0 || skillOperation.rebirthEnabled == 0) {
        return;
    }
    const std::int32_t y = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
    const std::int32_t x = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
    static_cast<void>(session.Connection().SendWorldActionRequest(x, y, 62));
    auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    const std::uint8_t previous = occupancy[7];
    const auto next = static_cast<std::uint8_t>(previous - 1);
    skillOperation.rebirthEnabled = 0;
    occupancy[7] = next;
    WindowSkill_SyncStates(session);
}

// 原函数：0x00499830。
std::int32_t WindowRuntime_DrawLargeFrame(WindowInstance& callbackWindow, const std::int32_t)
{
    WindowRuntime_DrawCurrent(callbackWindow);
    WindowInstance* const window = &callbackWindow;
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
    auto* previous = element && element->owner ? element->owner->FindElementById(element->relatedElementId) : nullptr;
    if (!previous) return 0;
    if (maximum <= 0) {
        return 0;
    }
    const std::int32_t height = element->height;
    const std::int32_t previousHeight = previous->height;
    const std::int32_t span = height - previousHeight;
    const std::int32_t halfStep = span / (maximum * 2);
    const std::int32_t delta = element->type == WindowElementType_Graphic
        ? element->data.graphic->offsetY : element->data.mouse->deltaY;
    return (halfStep + delta) * maximum / span;
}

// 原函数：0x004742C0。
std::int32_t WindowScroll_SetPosition(
    WindowElement* const element, const std::int32_t maximum, const std::int32_t position)
{
    auto* previous = element && element->owner ? element->owner->FindElementById(element->relatedElementId) : nullptr;
    if (!previous) return 0;
    std::int32_t y;
    if (maximum <= 0) {
        y = element->y;
    } else {
        const std::int32_t height = element->height;
        const std::int32_t previousHeight = previous->height;
        y = (height - previousHeight) * position / maximum;
        y += element->y;
    }
    previous->y = y;
    const std::int32_t delta = y - element->y;
    // 4C0160也传入type1图形载荷；原+8写入对应其offsetY。
    (element->type == WindowElementType_Graphic
        ? element->data.graphic->offsetY : element->data.mouse->deltaY) = delta;
    return delta;
}

// WWS01：原余量/像素结果持样和边界/flags取样序；现宿主数学图及原钳制保。
// 原函数：0x004743B0。
std::int32_t WindowScroll_ApplyWheel(
    WindowElement* const element, std::int32_t position, std::int32_t maximum,
    const std::int32_t wheel)
{
    if (wheel == 0) {
        return position;
    }
    if (maximum < 0) {
        maximum = 0;
    }
    WindowElement* const previous = element && element->owner ? element->owner->FindElementById(element->relatedElementId) : nullptr;
    if (!previous) return position;
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
    const std::int32_t delta = previousY - baseY;
    if (element->type == WindowElementType_Graphic) {
        element->data.graphic->offsetY = delta;
        element->data.graphic->imageNo |= 1;
    } else {
        element->data.mouse->deltaY = delta;
        element->data.mouse->flags |= 1u;
    }
    return position;
}

// 原函数：0x00485110。
void WindowElement_DrawHoverFrame(
    WindowElement* const element, const std::int32_t state, const std::uint32_t imageNo)
{
    if ((state & MouseFlag_Hover) == 0) {
        return;
    }
    WindowInstance* const window = element->owner;
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
    session::ClientSession& session,
    const std::int32_t left, const std::int32_t top,
    const std::int32_t width, const std::int32_t height)
{
    if (!platform::IsSessionInputTarget(session)) return 0;
    const std::int32_t x = (session.UiFrame().mouse.state->mouseX - static_cast<std::int32_t>(left));
    if (x < 0 || x >= width) {
        return 0;
    }
    const std::int32_t y = (session.UiFrame().mouse.state->mouseY - static_cast<std::int32_t>(top));
    return y >= 0 && y < height ? 1 : 0;
}

// 原函数：0x00475D40。
std::int32_t WindowDrag_ReadFocusWindowId(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    return pointer.focusWindowId;
}

// 原函数：0x00475D50。
ElementId WindowDrag_ReadFocusElement(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    return pointer.focusElementId;
}

// 原函数：0x00475D60。
std::int32_t WindowDrag_ReadKind(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    return pointer.kind;
}

// 原函数：0x00475D70。
std::int32_t WindowDrag_ReadX(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    return pointer.x;
}

// 原函数：0x00475D80。
std::int32_t WindowDrag_ReadY(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    return pointer.y;
}

// 原函数：0x00475D90。
std::int32_t WindowDrag_ReadSourceIndex(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    return pointer.sourceIndex;
}

// DLP01：仅phase0且kind非零启动阶段1，按原焦点/回调/来源写序发布，返S32 0/1。
// 原函数：0x00475DA0。
std::int32_t WindowDrag_Start(
    session::ClientSession& session,
    const std::int32_t kind, const std::int32_t sourceIndex, const WindowDragCallback callback)
{
    auto& pointer = session.UiFrame().native.pointer;
    if (pointer.phase != 0 || kind == 0) {
        return 0;
    }
    pointer.kind = kind;
    const std::int32_t windowId = pointer.currentWindowId;
    pointer.callback = callback;
    const ElementId elementId = pointer.currentElementId;
    pointer.focusWindowId = windowId;
    pointer.sourceIndex = sourceIndex;
    pointer.focusElementId = elementId;
    pointer.phase = 1;
    return 1;
}

// DLP01：仅phase0且kind非零启动阶段3，坐标保S16，原焦点与回调写序保持，返S32 0/1。
// 原函数：0x00476490。
std::int32_t WindowDrag_StartAtPosition(
    session::ClientSession& session,
    const std::int32_t kind, const std::int32_t sourceIndex,
    const std::int16_t x, const std::int16_t y, const WindowDragCallback callback)
{
    auto& pointer = session.UiFrame().native.pointer;
    if (pointer.phase != 0 || kind == 0) {
        return 0;
    }
    pointer.kind = kind;
    pointer.sourceIndex = sourceIndex;
    pointer.callback = callback;
    const std::int32_t windowId = pointer.currentWindowId;
    pointer.x = x;
    const ElementId elementId = pointer.currentElementId;
    pointer.y = y;
    pointer.focusWindowId = windowId;
    pointer.focusElementId = elementId;
    pointer.phase = 3;
    return 1;
}

// DLP01：入口phase4/5才可选回调message3，之后按原八槽顺序清理；不重验回调后phase。
// 原函数：0x00476500。
void WindowDrag_Reset(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    const std::int32_t phase = pointer.phase;
    if (phase != 4 && phase != 5) {
        return;
    }
    const WindowDragCallback callback = pointer.callback;
    if (callback != nullptr) {
        callback(session, 3, MouseFlag_None, pointer.sourceIndex);
    }
    pointer.kind = 0;
    pointer.sourceIndex = 0;
    pointer.callback = nullptr;
    pointer.x = 0;
    pointer.y = 0;
    pointer.focusWindowId = 0;
    pointer.phase = 0;
    pointer.focusElementId = 0;
}

// DLP01/MEI01：已有kind才绑定焦点窗口与元素ID，不改变会话阶段，返S32 0/1。
// 原函数：0x004763B0。
std::int32_t WindowDrag_SetFocus(
    session::ClientSession& session,
    const std::int32_t windowId, const ElementId elementId)
{
    auto& pointer = session.UiFrame().native.pointer;
    if (pointer.kind == 0) {
        return 0;
    }
    pointer.focusWindowId = windowId;
    pointer.focusElementId = elementId;
    return 1;
}

// DLP01：只清焦点元素ID/kind/焦点窗口/回调/phase五槽，来源及两坐标保持，返S32 0。
// 原函数：0x004763E0。
std::int32_t WindowDrag_ResetMouse(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    pointer.focusElementId = 0;
    pointer.kind = 0;
    pointer.focusWindowId = 0;
    pointer.callback = nullptr;
    pointer.phase = 0;
    return 0;
}

// WDF01：同一拖动W32样本/四BYTE消费与矩形/焦点原取序；结果为1/2掩码。
// 原函数：0x00472D20。
static std::int32_t WindowDrag_Update(WindowInstance& callbackWindow)
{
    if (!platform::IsSessionInputTarget(*callbackWindow.ownerSession)) return 0;
    auto& uiFrame = callbackWindow.ownerSession->UiFrame();
    auto& session = *callbackWindow.ownerSession;
    auto& pointer = session.UiFrame().native.pointer;
    const std::int32_t initialPhase = pointer.phase;
    WindowInstance* window = &callbackWindow;
    std::int32_t result = 0;
    if (initialPhase != 1) {
        const std::uint32_t state = window->state;
        if ((state & 2u) == 0) {
            if ((state & 0x800u) != 0) {
                if ((static_cast<std::uint8_t>(platform::ReadSessionMouseHeldButtons(*callbackWindow.ownerSession)) & platform::LegacyMouseButton_Left) != 0) {
                    std::int32_t* const x = &window->x;
                    const std::int32_t deltaX = (callbackWindow.ownerSession->UiFrame().mouse.state->mouseX - static_cast<std::int32_t>(window->dragMouseX1C));
                    *x += deltaX;
                    window = &callbackWindow;
                    std::int32_t* const y = &window->y;
                    const std::int32_t deltaY = (callbackWindow.ownerSession->UiFrame().mouse.state->mouseY - static_cast<std::int32_t>(window->dragMouseY20));
                    *y += deltaY;
                    const std::int32_t mouseX = static_cast<std::int32_t>(callbackWindow.ownerSession->UiFrame().mouse.state->mouseX);
                    callbackWindow.dragMouseX1C = mouseX;
                    const std::int32_t mouseY = static_cast<std::int32_t>(callbackWindow.ownerSession->UiFrame().mouse.state->mouseY);
                    callbackWindow.dragMouseY20 = mouseY;
                } else {
                    window->state = state & ~std::uint32_t{0x800};
                }
                result = 2;
                window = &callbackWindow;
            } else if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(*callbackWindow.ownerSession)) & platform::LegacyMouseEdge_LDown) != 0) {
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
                    const bool focusAvailable = uiFrame.windowHasFocus == 0;
                    const std::int32_t width = rect.width;
                    if (focusAvailable) {
                        if (WindowRuntime_HitTestRect(*callbackWindow.ownerSession, left, top, width, height) != 0) {
                            const std::int32_t mouseX = static_cast<std::int32_t>(callbackWindow.ownerSession->UiFrame().mouse.state->mouseX);
                            callbackWindow.dragMouseX1C = mouseX;
                            const std::int32_t mouseY = static_cast<std::int32_t>(callbackWindow.ownerSession->UiFrame().mouse.state->mouseY);
                            callbackWindow.dragMouseY20 = mouseY;
                            callbackWindow.state |= 0x800u;
                            result |= 2;
                        }
                        window = &callbackWindow;
                    }
                    count = window->secondaryCount;
                }
            }
        }
    }
    if ((*reinterpret_cast<const unsigned char*>(&window->state) & 1u) == 0) {
        const std::int32_t phase = pointer.phase;
        if (phase == 1 || phase == 4) {
            const std::int32_t height = window->height;
            const std::int32_t width = window->width;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            if (WindowRuntime_HitTestRect(*callbackWindow.ownerSession, x, y, width, height) != 0) {
                result |= 1;
            }
        } else if (uiFrame.windowHasFocus == 0) {
            const std::int32_t height = window->height;
            const std::int32_t width = window->width;
            const std::int32_t y = window->y;
            const std::int32_t x = window->x;
            if (WindowRuntime_HitTestRect(*callbackWindow.ownerSession, x, y, width, height) != 0) {
                result |= 1;
                if ((static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(*callbackWindow.ownerSession)) & platform::LegacyMouseEdge_LDown) != 0) {
                    callbackWindow.state |= 0x2000u;
                }
            }
        }
    }
    return result;
}

// WCR01：71槽整理共用原state样本分流；关闭后压紧、等层前插与计数见对应SPEC。
// 原函数：0x004730A0。
static std::int32_t WindowRuntime_CompactAndReorder(session::ClientSession& session)
{
    auto& windows = WindowRuntime_Windows(session);
    std::int32_t count = 0;
    for (std::size_t index = 0; index < kWindowCount;) {
        WindowInstance* const window = windows[index];
        if (window == nullptr) {
            std::size_t source = index;
            while (source < kWindowCount && windows[source] == nullptr) {
                ++source;
            }
            if (source == kWindowCount) {
                break;
            }
            windows[index] = windows[source];
            windows[source] = nullptr;
            continue;
        }
        const std::uint32_t state = window->state;
        if ((state & 0x8000u) != 0) {
            WindowRuntime_Free(window);
            for (std::size_t target = index; target + 1 < kWindowCount; ++target) {
                windows[target] = windows[target + 1];
            }
            windows[kWindowCount - 1] = nullptr;
            if (index + 1 != kWindowCount) {
                continue;
            }
        } else {
            if ((state & 0x2000u) != 0) {
                std::size_t target = index;
                while (target > 0) {
                    WindowInstance* const previous = windows[target - 1];
                    if (previous->layer < window->layer) {
                        break;
                    }
                    windows[target] = previous;
                    --target;
                }
                windows[target] = window;
                window->state &= ~std::uint32_t{0x2000};
            }
            ++count;
        }
        ++index;
    }
    return count;
}

// 原函数：0x00473E40。
static void WindowRuntime_AdvanceMousePhase(session::ClientSession& session)
{
    if (!platform::IsSessionInputTarget(session)) return;
    auto& uiFrame = session.UiFrame();
    auto& pointer = session.UiFrame().native.pointer;
    std::int32_t phase = pointer.phase;
    if (phase == 1) {
        const WindowDragCallback callback = pointer.callback;
        if (callback == nullptr) {
            uiFrame.windowHasFocus = 1;
            *reinterpret_cast<unsigned char*>(&session.UiFrame().mouse.state->valueD68DB4) = 5;
            return;
        }
        const std::int32_t events = WindowRuntime_AppendMouseEvents(session, MouseFlag_None);
        callback(session, 0, events, pointer.sourceIndex);
        phase = pointer.phase;
    } else if (phase == 2) {
        pointer.phase = 0;
        return;
    }
    switch (phase) {
    case 3:
        pointer.phase = 4;
        uiFrame.windowHasFocus = 1;
        *reinterpret_cast<unsigned char*>(&session.UiFrame().mouse.state->valueD68DB4) = 5;
        return;
    case 4: {
        const WindowDragCallback callback = pointer.callback;
        if (callback != nullptr) {
            callback(session, 2, MouseFlag_None, pointer.sourceIndex);
        }
        pointer.kind = 0;
        pointer.sourceIndex = 0;
        pointer.callback = nullptr;
        pointer.x = 0;
        pointer.y = 0;
        pointer.focusWindowId = 0;
        pointer.phase = 0;
        pointer.focusElementId = 0;
        return;
    }
    case 6:
        pointer.phase = 0;
        return;
    case 1:
        uiFrame.windowHasFocus = 1;
        *reinterpret_cast<unsigned char*>(&session.UiFrame().mouse.state->valueD68DB4) = 5;
        break;
    default:
        break;
    }
}

// 原函数：0x00473560。
static int WindowRuntime_DispatchElementMouseAt(WindowInstance& target, const ElementId elementId, int ox, int oy, const UIClipRect* clip)
{
    auto& uiFrame = target.ownerSession->UiFrame();
    auto& session = *target.ownerSession;
    auto& pointer = session.UiFrame().native.pointer;
    if (WindowRuntime_PointerDragBlocksMouse(target)) return 0;
    WindowInstance* const window = &target;
    WindowElementCursor lease(target);
    WindowElement* const element = window->FindElementById(elementId);
    if (!element) return 0;
    const auto identity = WindowRuntime_ElementRef(*element);
    if (!WindowRuntime_IsElementInteractive(*window, *element)) { WindowRuntime_ResetElement(*element); return 0; }
    pointer.currentElementId = elementId;
    if (element->type == WindowElementType_Input) {
        WindowElement_PrepareInput(*element, ox, oy, clip);
        WindowElement_SyncInputText(*element);
        if (WindowRuntime_ResolveElement(identity) != element || !WindowRuntime_IsElementInteractive(*window, *element)) return 0;
    }
    if (!platform::IsSessionInputTarget(session)) {
        if (element->type == WindowElementType_ScrollView && element->data.scrollView)
            return WindowElement_ProcessScroll(*element, MouseFlag_None, ox, oy, clip);
        return 0;
    }
    const std::int32_t left = ox + element->x;
    const std::int32_t top = oy + element->y;
    const std::int32_t width = element->width;
    const std::int32_t height = element->height;
    std::int32_t events = MouseFlag_None;
    std::int32_t phase = pointer.phase;
    if (phase == 1 || phase == 4 || phase == 2 || phase == 6) {
        if (static_cast<std::uint8_t>(target.ownerSession->UiFrame().mouse.state->valueD68DB4) < 0xD4u) {
            if (WindowRuntime_HitTestRect(*target.ownerSession, left, top, width, height) != 0
                && UIPointInClipRect(target.ownerSession->UiFrame().mouse.state->mouseX, target.ownerSession->UiFrame().mouse.state->mouseY, clip)) {
                phase = pointer.phase;
                if (phase == 1 || phase == 4) {
                    events = MouseFlag_Dragging;
                }
                if (pointer.focusWindowId == pointer.currentWindowId
                    && pointer.focusElementId == pointer.currentElementId) {
                    events = WindowRuntime_AppendMouseEvents(*target.ownerSession, events);
                }
            } else {
                phase = pointer.phase;
            }
        }
    } else if (uiFrame.windowHasFocus == 0) {
        if (WindowRuntime_HitTestRect(*target.ownerSession, left, top, width, height) != 0
                && UIPointInClipRect(target.ownerSession->UiFrame().mouse.state->mouseX, target.ownerSession->UiFrame().mouse.state->mouseY, clip)) {
            events = WindowRuntime_AppendMouseEvents(*target.ownerSession, MouseFlag_Hover);
            WindowInstance* const currentWindow = &target;
            WindowElement* const currentElement = currentWindow->FindElementById(elementId);
            if (currentElement && currentElement->type == WindowElementType_MouseArea) {
                auto* const data = currentElement->data.mouse;
                data->deltaX = (target.ownerSession->UiFrame().mouse.state->mouseX - static_cast<std::int32_t>(ox + currentElement->x));
                data->deltaY = (target.ownerSession->UiFrame().mouse.state->mouseY - static_cast<std::int32_t>(oy + element->y));
            }
        }
        phase = pointer.phase;
    }
    if (phase == 4) {
        const std::int32_t dragX = pointer.x - left;
        if (dragX >= 0 && dragX < width) {
            const std::int32_t dragY = pointer.y - top;
            if (dragY >= 0 && dragY < height && UIPointInClipRect(pointer.x, pointer.y, clip)) {
                events |= MouseFlag_DragFinish;
            }
        }
    }
    const auto previousMouse = element->mouseState;
    element->mouseState = (events & (MouseFlag_Dragging | MouseFlag_LDownEvent | MouseFlag_LHoldPhysical | MouseFlag_LRepeatDown))
        ? WindowMouseState_Press : (events & MouseFlag_Hover) ? WindowMouseState_Hover : WindowMouseState_None;
    int handled = 0;
    const bool inputFocused = element->type == WindowElementType_Input && element->data.input
        && element->data.input->ownedInput && (events & MouseFlag_LDownEvent);
    if (element->type == WindowElementType_Graphic) WindowElement_UpdateImageState(*element);
    if (element->type == WindowElementType_MapEx && element->data.mapEx) element->data.mapEx->UpdateMouse(*element, ox, oy, clip, events);
    if (element->type == WindowElementType_Input) {
        if (events & MouseFlag_LDownEvent) WindowElement_FocusInput(*element);
        else if ((platform::ReadSessionMouseEdgeButtons(*target.ownerSession) & platform::LegacyMouseEdge_LDown) && !(events & MouseFlag_Hover)) WindowElement_BlurInput(*element);
    }
    if (element->type == WindowElementType_ScrollView && element->data.scrollView)
        handled = WindowElement_ProcessScroll(*element, events, ox, oy, clip);
    pointer.currentElementId = elementId;
    if (WindowRuntime_ResolveElement(identity) != element) return handled;
    if (previousMouse == WindowMouseState_Hover && element->mouseState == WindowMouseState_None && window->binding)
        window->binding->elementEvent(*element, static_cast<int>(elementId), MouseFlag_Leave);
    if (WindowRuntime_ResolveElement(identity) != element || !WindowRuntime_IsElementInteractive(*window, *element)) return handled;
    const int ownerId = window->id;
    const auto ownerGeneration = window->generation;
    if (element->visible == 1 && !handled) {
        if (element->mouseCallback) handled = element->mouseCallback(*window, elementId, events);
        const auto* live = WindowRuntime_FindInstance(session, ownerId);
        if (live == window && live->generation == ownerGeneration && window->binding
            && WindowRuntime_ResolveElement(identity) == element && WindowRuntime_IsElementInteractive(*window, *element)) {
            if (WindowRuntime_BeginPointerDrag(*window, *element, events)) handled |= 1;
            handled |= window->binding->elementEvent(*element, static_cast<int>(elementId), events);
            if (WindowRuntime_ResolveElement(identity) == element && window->binding && WindowRuntime_IsElementInteractive(*window, *element))
                handled |= window->binding->event(static_cast<int>(elementId), events);
        }
    }
    handled |= inputFocused ? 1 : 0;
    if (handled != 0) {
        phase = pointer.phase;
        auto* const eventByte = reinterpret_cast<unsigned char*>(
            &target.ownerSession->UiFrame().mouse.state->valueD68DB4);
        *eventByte = 0xD4;
        uiFrame.windowHasFocus = 1;
        if (phase == 4) {
            const WindowDragCallback callback = pointer.callback;
            if (callback != nullptr) {
                callback(session, 4, MouseFlag_None, pointer.sourceIndex);
            }
            static_cast<void>(WindowDrag_ResetMouse(session));
        }
    }
    return handled;
}

void WindowRuntime_UpdateExtendedElements(WindowInstance& window)
{
    auto& session = *window.ownerSession;
    auto& pointer = session.UiFrame().native.pointer;
    WindowRuntime_UpdateElementOrder(window);
    WindowElementCursor cursor(window);
    while (auto* element = cursor.next()) {
        if (element->parentId) continue;
        const auto id = element->elementId;
        const auto identity = WindowRuntime_ElementRef(*element);
        if (window.binding) window.binding->elementUpdate(*element);
        element = WindowRuntime_ResolveElement(identity);
        if (!element || (window.state & 0x8000u)) continue;
        pointer.currentElementId = element->elementId;
        WindowRuntime_DispatchElementMouse(*element->owner, id, element->owner->x, element->owner->y, nullptr);
    }
}

// WSB01：原载荷字节借用及上下界/差值三个现场取样阶段见滚动回调SPEC。
// 原函数：0x004741A0。
std::int32_t WindowElement_HandleVerticalScroll(WindowInstance& callbackWindow, const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const window = &callbackWindow;
    if (!window) return 0;
    // 窗口49的Graphic与普通MouseArea共享滚动语义，按活动类型读写对应字段。
    auto* const scrollElement = window->FindElementById(elementId);
    if (!scrollElement) return 0;
    auto* const previous = window->FindElementById(scrollElement->relatedElementId);
    if (!previous) return 0; // 仅取消当前滚动控件操作，调度器继续后续元素。
    std::uint32_t flags = scrollElement->type == WindowElementType_Graphic
        ? std::bit_cast<std::uint32_t>(scrollElement->data.graphic->imageNo)
        : scrollElement->data.mouse->flags;
    std::int32_t result = 0;
    if ((flags & 2u) == 0) flags = 0;
    if ((state & MouseFlag_LHoldPhysical) != 0) {
        flags = 3;
        result = 1;
    }
    if ((flags & 2u) != 0) {
        const std::int32_t y = callbackWindow.ownerSession->UiFrame().mouse.state->mouseY - window->y;
        previous->y = y - (previous->height >> 1);
    }
    if (previous->y < scrollElement->y) previous->y = scrollElement->y;
    const std::int32_t bottom = scrollElement->height + scrollElement->y;
    if (previous->y + previous->height > bottom) previous->y = bottom - previous->height;
    const std::int32_t delta = previous->y - scrollElement->y;
    if ((state & MouseFlag_LHoldPhysical) == 0) flags &= ~std::uint32_t{2};
    if (scrollElement->type == WindowElementType_Graphic) {
        scrollElement->data.graphic->offsetY = delta;
        scrollElement->data.graphic->imageNo = std::bit_cast<std::int32_t>(flags);
    } else {
        scrollElement->data.mouse->deltaY = delta;
        scrollElement->data.mouse->flags = flags;
    }
    return result;
}

// WIC01：原型四模式、现场context与BYTE结果合同，见doc/cg2d_window_input_context_dispatch_spec.md。
// 原函数：0x00476570。
std::uint8_t WindowInput_ActivateElement(
    session::ClientSession& session,
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
            session,
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
        prototype->input.context->textX = inputX;
        const std::int32_t inputY = prototype->y + y;
        prototype->input.context->textY = inputY;
        return 1;
    }
    case 3: {
        const std::int32_t fontType = prototype->fontType;
        const std::int32_t renderMode = prototype->renderMode;
        const std::int32_t color = context->color;
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
static void WindowInput_FormatSkillSlotCount(session::ClientSession& session, std::wstring& destination)
{
    std::int32_t count = 0;
    for (std::size_t position = 0; position < world::kSkillSlotCount; ++position) {
        const auto& pair = session.WorldScene().playerSkills.pairs[position];
        if (pair.used != 0) {
            const std::int16_t index = std::bit_cast<std::int16_t>(pair.index);
            const auto& skill = session.WorldScene().playerSkills.skills[index];
            std::int16_t requiredSlots;
            std::memcpy(&requiredSlots, skill.requiredSlots19.data(), sizeof(requiredSlots));
            count += requiredSlots;
        }
    }
    cg::text::FormatTo(destination, L"{:2}/{}", count, session.WorldScene().player->value16C);
}

// 同一4A4210在原表也用于release；仅适配未消费返回值，不转换函数指针。
// WCL01：窗口release家族丢弃原4A4210常量1；真实七类资源回收留在FreeWindow，非此适配。
// 原函数：0x004A4210。
void WindowRuntime_ReleaseDefault(WindowInstance&)
{
    static_cast<void>(WindowRuntime_ReturnOne(0));
}

std::int32_t WindowRuntime_UpdateDefault(WindowInstance&, const std::int32_t value) noexcept
{
    return WindowRuntime_ReturnOne(value);
}

void WindowRuntime_QueuePagePresentation(const WindowInstance& source, const WindowInstance& target)
{
    auto* const pagePresentation = source.ownerSession->UiFrame().native.pagePresentation;
    if (!pagePresentation || !pagePresentation->accepting
        || pagePresentation->sourceId != source.id
        || pagePresentation->sourceGeneration != source.generation) return;
    pagePresentation->targetId = target.id;
    pagePresentation->targetGeneration = target.generation;
}

} // namespace detail

// 窗口函数实现。

// 原函数：0x00473F80。
std::int32_t WindowRuntime_Update(session::ClientSession& session)
{
    auto& uiFrame = session.UiFrame();
    auto& pointer = session.UiFrame().native.pointer;
    using namespace detail;
    auto& states = WindowRuntime_States(session);
    // 仅诊断去重，不参与字段／协议状态；失败回调结束后仍继续输入和绘制。
    auto& frame = session.UiFrame();
    bool encodingFailed = false;
    const auto invoke = [&](auto&& callback) -> std::int32_t {
        try { return callback(); }
        catch (const cg::text::GbkStorageError&) {
            encodingFailed = true;
            input::InputContext* const context = session.UiFrame().inputState.currentContext;
            const std::wstring contents = context != nullptr ? input::ContextText(*context) : std::wstring{};
            if (!frame.reportedFailure || frame.reportedContext != context || frame.reportedText != contents) {
                frame.reportedContext = context;
                frame.reportedText = contents;
                frame.reportedFailure = true;
                m_log_iw(L"文字无法按GBK保存，或已超出字段容量。");
            }
            return 0;
        }
    };
    std::int32_t carried = 1;
    session.WorldScene().movement.targetInputOverride = 0;
    session.WorldScene().movement.targetDrawOverride = 0;
    uiFrame.windowHasFocus = 0;
    WindowRuntime_AdvanceMousePhase(session);
    PrepareWindowFrame(session);
    WindowRuntime_BeginPointerDragFrame(session);
    const std::int32_t count = WindowRuntime_CompactAndReorder(session);
    const auto scheduledWindows = MergeWindowOrder(session, count);
    const bool hasWindows = !scheduledWindows.empty();
    uiFrame.nativeWindowCount = count;
    if (hasWindows) {
        std::int32_t index = 0;
        for (;;) {
            const auto& scheduled = scheduledWindows[index];
            WindowInstance* window = scheduled.window;
            const auto& extension = scheduled.extension;
            if ((!extension && states[scheduled.id].window != window)
                || (window->state & 0x8000u) != 0) {
                if (++index >= static_cast<std::int32_t>(scheduledWindows.size())) break;
                continue;
            }
            WindowPagePresentation pagePresentation(*window);
            pointer.currentWindow = window;
            pointer.currentWindowState = extension ? &extension->state : &states[window->id];
            pointer.currentWindowId = window->id;
            const std::int32_t focused = WindowRuntime_PointerDragBlocksMouse(session) ? 0 : WindowDrag_Update(*window);
            std::int32_t updateResult;
            if (extension) {
                if (window->binding && !(window->state & 0x8000u)) window->binding->update();
                updateResult = (window->state & 0x8000u) ? 0
                    : invoke([&] { return extension->update(carried, focused); });
            } else {
                updateResult = invoke([&] { return window->update(*window, carried); });
                window = pointer.currentWindow;
                if (window->binding && !(window->state & 0x8000u)) window->binding->update();
                // WEO01：update维护排序缓存，分页的前景顺序由创建时的seqNo表达。
                WindowRuntime_UpdateElementOrder(*window);
                WindowElementCursor cursor(*window);
                while (auto* element = cursor.next()) {
                    if (element->parentId) continue;
                    const auto elementId = element->elementId;
                    const auto identity = WindowRuntime_ElementRef(*element);
                    pointer.currentElementId = element->elementId;
                    if ((window->state & 0x8000u) || window->scriptHidden) break;
                    if (window->binding) window->binding->elementUpdate(*element);
                    if ((window->state & 0x8000u) || window->scriptHidden) break;
                    element = WindowRuntime_ResolveElement(identity);
                    if (element)
                        static_cast<void>(invoke([&] { return WindowRuntime_DispatchElementMouse(*window, elementId, window->x, window->y, nullptr); }));
                }
                static_cast<void>(WindowRuntime_ProcessPointerDrag(*window));
            }
            const bool hasFocus = focused != 0;
            pointer.currentElementId = 0;
            if (hasFocus) {
                const std::int32_t phase = pointer.phase;
                *reinterpret_cast<unsigned char*>(
                    &session.UiFrame().mouse.state->valueD68DB4) = 212;
                const bool releaseDrag = phase == 4;
                uiFrame.windowHasFocus = 1;
                if (releaseDrag) {
                    const WindowDragCallback callback = pointer.callback;
                    if (callback != nullptr) {
                        callback(session, 4, MouseFlag_None, pointer.sourceIndex);
                    }
                    static_cast<void>(WindowDrag_ResetMouse(session));
                    window = pointer.currentWindow;
                }
                updateResult = 0;
            }
            pagePresentation.accepting = false;
            if (window->binding && !(window->state & 0x8000u) && !window->scriptHidden) {
                WindowElementCursor cursor(*window);
                while (auto* currentElement = cursor.next()) {
                    const auto id = currentElement->elementId;
                    if ((window->state & 0x8000u) || window->scriptHidden) break;
                    if (auto* element = window->FindElementById(id); element && !element->detaching)
                        window->binding->elementDraw(*element);
                }
            }
            if (!(window->state & 0x8000u) && !window->scriptHidden) {
                carried = invoke([&] { return extension ? extension->render(updateResult) : window->draw(*window, updateResult); });
                if (window->binding && !(window->state & 0x8000u) && !window->scriptHidden) window->binding->draw();
            }
            // 切页回调已消费点击；新页只刷新原生内容并接替旧页的绘制位置，不参与本帧输入。
            if (!extension && pagePresentation.targetId >= 0
                && states[pagePresentation.sourceId].window == window
                && window->generation == pagePresentation.sourceGeneration && (window->state & 0x8000u)) {
                auto* const target = WindowRuntime_FindInstance(session, pagePresentation.targetId);
                if (target && target->generation == pagePresentation.targetGeneration
                    && !(target->state & 0x8000u) && !target->scriptHidden
                    && std::none_of(scheduledWindows.begin(), scheduledWindows.end(),
                        [target](const ScheduledWindow& entry) { return entry.window == target; })) {
                    auto* const previousWindow = pointer.currentWindow;
                    auto* const previousState = pointer.currentWindowState;
                    const auto previousId = pointer.currentWindowId;
                    const auto previousElementId = pointer.currentElementId;
                    pointer.currentWindow = target;
                    pointer.currentWindowState = &states[target->id];
                    pointer.currentWindowId = target->id;
                    pointer.currentElementId = 0;
                    // 宠物页的update(-1)只初始化；正常update补齐属性/技能，沿各页自己的draw。
                    const auto targetResult = invoke([&] { return target->update(*target, carried); });
                    if (WindowRuntime_FindInstance(session, pagePresentation.targetId) == target
                        && target->generation == pagePresentation.targetGeneration
                        && !(target->state & 0x8000u) && !target->scriptHidden) {
                        carried = invoke([&] { return target->draw(*target, targetResult); });
                    }
                    pointer.currentWindow = previousWindow;
                    pointer.currentWindowState = previousState;
                    pointer.currentWindowId = previousId;
                    pointer.currentElementId = previousElementId;
                }
            }
            const std::int32_t currentCount = static_cast<std::int32_t>(scheduledWindows.size());
            ++index;
            if (index >= currentCount) {
                break;
            }
        }
    }
    WindowRuntime_ProcessShortcuts(session);
    static_cast<void>(input::ProcessPresetHotkeys_44B340(session));
    const std::int32_t phase = pointer.phase;
    pointer.currentWindowId = 0;
    if (phase == 1
        && static_cast<std::uint8_t>(session.UiFrame().mouse.state->valueD68DB4) < 212u) {
        session.WorldScene().movement.targetDrawOverride = 1;
        *reinterpret_cast<unsigned char*>(
            &session.UiFrame().mouse.state->valueD68DB4) = 212;
    }
    if (!encodingFailed) { frame.reportedFailure = false; frame.reportedContext = nullptr; frame.reportedText.clear(); }
    FinishWindowFrame(session);
    return 1;

}

// 原函数：0x00504120。
std::int32_t WindowRuntime_Render(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& skillOperation = session.UiFrame().native.skillOperation;
    using namespace detail;
    if (skillOperation.rebirthEnabled != 0 && session.WorldScene().player->timerRemaining0C <= 0) {
        const std::int32_t y = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
        const std::int32_t x = static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
        static_cast<void>(session.Connection().SendWorldActionRequest(x, y, 62));
        auto* const occupancy = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
        const auto decreased = static_cast<std::uint8_t>(occupancy[7] - 1);
        skillOperation.rebirthEnabled = 0;
        occupancy[7] = decreased;
        WindowSkill_SyncStates(session);
    }
    return WindowRuntime_Update(session);
}

// WPL01：七类载荷创建/owner与动画现场源槽，见doc/cg2d_window_payload_creation_spec.md。
// 原函数：0x00471440。
WindowInstance* WindowRuntime_Open(session::ClientSession& session, const std::int32_t id)
{
    auto& pointer = session.UiFrame().native.pointer;
    using namespace detail;
    auto& windows = WindowRuntime_Windows(session);
    auto& states = WindowRuntime_States(session);
    const auto existingEntry = std::find_if(windows.begin(), windows.end(),
        [id](const WindowInstance* window) { return window && window->id == id; });
    const std::int8_t category = existingEntry != windows.end()
        ? (*existingEntry)->category : kWindowCategories[id];
    if (category == 1) {
        WindowRuntime_CloseByCategory(session, 2);
    } else if (category == 2) {
        WindowRuntime_CloseByCategory(session, 3);
    }
    for (std::int32_t index = 0; index < static_cast<std::int32_t>(kWindowCount); ++index) {
        WindowInstance* const existing = windows[index];
        if (existing != nullptr && existing->id == id) {
            windows[index]->state |= 0x2000u;
            static_cast<void>(CenterPopupWindowPosition(id,
                existing->width, existing->height, existing->x, existing->y));
            return windows[index];
        }
    }
    std::int32_t freeSlot = 0;
    while (freeSlot < static_cast<std::int32_t>(kWindowCount) && windows[freeSlot] != nullptr) {
        ++freeSlot;
    }
    if (freeSlot == static_cast<std::int32_t>(kWindowCount)) {
        return nullptr;
    }
    auto* const window = new (std::nothrow) WindowInstance{};
    if (window == nullptr) {
        return nullptr;
    }
    const WindowPrototype* const prototype = &WindowRuntime_Prototype(session, id);
    window->ownerSession = &session;
    window->id = id;
    window->category = category;
    WindowRuntime_EnsureIdentity(*window);
    window->state = prototype->state;
    window->layer = prototype->layer;
    window->width = prototype->width;
    window->height = prototype->height;
    window->flags = prototype->flags;
    window->update = prototype->update;
    window->draw = prototype->draw;
    window->release = prototype->release;
    const std::int32_t count = prototype->elementCount;
    const std::int32_t pageCount = WindowInventory_GetPageElementCount(session, id);
    window->elementCount = count + pageCount;
    if (window->elementCount != 0) {
        ElementId previousId = 0;
        for (int slot = 0; slot < count + pageCount; ++slot) {
            const auto fixedId = slot < count && prototype->elements[slot].elementId
                ? prototype->elements[slot].elementId : NativeElementKey(slot);
            auto& element = WindowRuntime_AddElement(*window, fixedId);
            element.relatedElementId = previousId;
            previousId = fixedId;
        }
        WindowElementPrototype* source = prototype->elements;
        for (std::int32_t index = 0; index < count; ++index, ++source) {
            // 与创建阶段共用原型固定ID，允许页签插入和不连续ID，前后关系也沿实际ID。
            const ElementId elementId = source->elementId ? source->elementId : NativeElementKey(index);
            auto* const elementPtr = window->FindElementById(elementId);
            if (!elementPtr) { continue; }
            WindowElement* const element = elementPtr;
            element->refUpdate = element->refDraw = element->refEvent = -1;
            element->refClick = element->refPress = element->refHover = element->refLeave = -1;
            element->refDrag = element->refDrop = element->refChange = element->refEnter = -1;
            element->type = source->type;
            element->state = 0;
            // 保留动态分页既有前景层次，原型显式seqNo优先。
            const std::int32_t prefixCount = id == WindowId_Bank ? 2 : 0;
            element->seqNo = source->seqNo == -1
                ? index + (index >= prefixCount ? pageCount : 0) : source->seqNo;
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
                element->data.graphic = data;
                break;
            }
            case WindowElementType_Animation: {
                auto* const data = static_cast<WindowAnimationData*>(
                    std::calloc(1, sizeof(WindowAnimationData)));
                if (data != nullptr) {
                    cg::yobi::YobiAction* const action = cg::yobi::NewYobiAction_404AE0(session, 2, 0);
                    data->action = action;
                    cg::yobi::ConfigureWindowElementAction(
                        action, data->action, source->source.animationImageNo);
                    data->value04 = 0;
                }
                element->data.animation = data;
                break;
            }
            case WindowElementType_Text: {
                auto* const data = new (std::nothrow) WindowTextData{};
                if (data != nullptr) {
                    const WindowTextPrototype* const original = source->source.text;
                    data->text = original->text;
                    std::wstring text;
                    WindowInput_FormatSkillSlotCount(session, text);
                    if (data->text == L"10/10") { data->text = text; }
                    data->style.color = original->style.color;
                    data->style.font = original->style.font;
                }
                element->data.text = data;
                break;
            }
            case WindowElementType_MouseArea: {
                auto* const data = static_cast<WindowMouseData*>(
                    std::calloc(1, sizeof(WindowMouseData)));
                if (data != nullptr) {
                    data->flags = *source->source.mouseFlags;
                }
                element->data.mouse = data;
                break;
            }
            case WindowElementType_Input:
                element->data.input = static_cast<WindowInputData*>(std::calloc(1, sizeof(WindowInputData)));
                break;
            case WindowElementType_InputCaret:
                element->data.inputCaret = static_cast<std::uint32_t*>(std::calloc(1, sizeof(std::uint32_t)));
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
                element->data.number = data;
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
    windows[freeSlot] = window;
    window->state |= 0x2000u;
    window->x = states[id].x;
    window->y = states[id].y;
    // 分页区在下方发布；初始化控件前即使用最终高度确定弹窗位置。
    const std::int32_t layoutHeight = window->height
        + ((id == WindowId_Inventory || id == WindowId_BattleItems || id == WindowId_Craft) ? 30 : 0);
    static_cast<void>(CenterPopupWindowPosition(id,
        window->width, layoutHeight, window->x, window->y));
    const std::uint32_t flags = states[id].flags;
    WindowInstance* const previous = pointer.currentWindow;
    states[id].flags = flags | 0x80u;
    pointer.currentWindow = window;
    window->elementCount = count;
    static_cast<void>(window->update(*window, -1));
    states[id].window = window;
    window->elementCount = count + pageCount;
    switch (id) {
    case WindowId_Inventory:
    case WindowId_BattleItems:
    case WindowId_Craft:
        window->height += 30;
        static_cast<void>(window->update(*window, -1));
        break;
    case WindowId_Bank:
        for (std::int32_t index = 3; index <= 6; ++index) {
            auto* const indexedElement = window ? window->FindElementById(NativeElementKey(index)) : nullptr;
            if (indexedElement) {
                indexedElement->visible = 0;
            }
        }
        break;
    default:
        break;
    }
    // 初始化会发布实际尺寸；仅符合原型中心规则的窗口按最终几何居中。
    static_cast<void>(CenterPopupWindowPosition(id,
        window->width, window->height, window->x, window->y));
    WindowRuntime_UpdateElementOrder(*window);
    pointer.currentWindow = previous;
    WindowRuntime_QueueWindowOpen(*window);
    return window;
}

// 原函数：0x00475AA0。
cg::yobi::YobiAction* WindowRuntime_CreateAnimation(
    session::ClientSession& session, const std::int32_t id,
    const std::uint32_t flags, const std::uint32_t mode,
    cg::yobi::YobiAction** const completionSlot)
{
    using namespace detail;
    auto& states = WindowRuntime_States(session);
    const WindowPrototype* const prototype = &WindowRuntime_Prototype(session, id);
    const std::int32_t width = prototype->width;
    const std::int32_t height = prototype->height;
    if (id == 10 || id == 11 || id == 12 || id == 14 || id == 27) {
        const std::int32_t offsetX = platform::RandomRange_46FAF0(-15, 15);
        states[id].x += offsetX;
        const std::int32_t offsetY = platform::RandomRange_46FAF0(-15, 15);
        states[id].y += offsetY;
        if (states[id].x < 0) {
            states[id].x = platform::RandomRange_46FAF0(10, 20);
        }
        if (states[id].y < 0) {
            states[id].y = platform::RandomRange_46FAF0(10, 20);
        }
        if (states[id].x + width > platform::gWindowWidth) {
            states[id].x = platform::RandomRange_46FAF0(platform::gWindowWidth - 20 - width, platform::gWindowWidth - width);
        }
        if (states[id].y + height > platform::gWindowHeight) {
            states[id].y = platform::RandomRange_46FAF0(platform::gWindowHeight - 20 - height, platform::gWindowHeight - height);
        }
    }
    WindowInstance* const window = states[id].window;
    std::int32_t x;
    std::int32_t y;
    if (window != nullptr) {
        window->state |= 0x8000u;
        WindowInstance* const current = states[id].window;
        x = current->x;
        y = current->y;
    } else {
        x = states[id].x;
        y = states[id].y;
    }
    // 展开矩形计入实际分页区；位置由Sub_475950与创建入口共用的规则确定。
    const std::int32_t openingHeight = height
        + ((id == WindowId_Inventory || id == WindowId_BattleItems || id == WindowId_Craft) ? 30 : 0);
    return cg::yobi::CreateWindowOpeningAnimation_475950(session, x, y, width, openingHeight, id, flags, mode, completionSlot);
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

std::uint8_t WindowRuntime_ReadUiGate(session::ClientSession& session) noexcept
{
    return session.UiFrame().native.stall.openForBusiness;
}

std::uint8_t WindowRuntime_ReadObservationFlag(session::ClientSession& session) noexcept
{
    auto& skillOperation = session.UiFrame().native.skillOperation;
    return skillOperation.rebirthEnabled;
}

// 原数据：0x00584DA4。
std::int32_t WindowItemDrag_ReadIndex(session::ClientSession& session) noexcept
{
    auto& interaction = session.UiFrame().native.interaction;
    return interaction.dragItemIndex;
}

// DLP01：非零phase才取消，可选message1/4后重取phase；八槽清理末按该样本进入2/6。
// 原函数：0x00476410。
void WindowDrag_Cancel(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    using namespace detail;
    std::int32_t phase = pointer.phase;
    if (phase == 0) {
        return;
    }
    const WindowDragCallback callback = pointer.callback;
    if (callback != nullptr) {
        callback(session, phase == 1 ? 1 : 4, MouseFlag_None, pointer.sourceIndex);
        phase = pointer.phase;
    }
    pointer.kind = 0;
    pointer.sourceIndex = 0;
    pointer.callback = nullptr;
    pointer.x = 0;
    pointer.y = 0;
    pointer.focusWindowId = 0;
    pointer.focusElementId = 0;
    pointer.phase = phase == 1 ? 2 : 6;
}

// 原函数：0x00471270。
void WindowRuntime_Free(WindowInstance* const window)
{
    auto& session = *window->ownerSession;
    auto& pointer = session.UiFrame().native.pointer;
    using namespace detail;
    window->state |= 0x8000u;
    NativeWindowClosed(*window);
    window->release(*window);
    WindowRuntime_DestroyElements(*window);
    WindowRuntime_ReleaseBindings(*window);
    pointer.currentWindowState = &WindowRuntime_States(*window->ownerSession)[window->id];
    pointer.currentWindowState->x = window->x;
    pointer.currentWindowState->y = window->y;
    pointer.currentWindowState->flags &= ~std::uint32_t{0x80};
    pointer.currentWindowState->flags |= 1u;
    pointer.currentWindowState->window = nullptr;
    const std::int32_t focusWindowId = pointer.focusWindowId;
    const std::int32_t windowId = window->id;
    if (focusWindowId == windowId) {
        WindowDrag_Cancel(session);
    }
    delete window;
}

// WCR01：发布及bit1写沿同一状态记录地址，清槽由既有472190调用父负责。
// 原函数：0x00472160。
void WindowRuntime_Close(WindowInstance* const window)
{
    auto& pointer = window->ownerSession->UiFrame().native.pointer;
    detail::WindowState* const state = &detail::WindowRuntime_States(*window->ownerSession)[window->id];
    pointer.currentWindowState = state;
    state->flags |= 2u;
    WindowRuntime_Free(window);
}

// 原函数：0x00472190。
std::int32_t WindowRuntime_CloseAll(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& skillOperation = session.UiFrame().native.skillOperation;
    using namespace detail;
    auto& windows = WindowRuntime_Windows(session);
    for (std::size_t index = 0; index < windows.size(); ++index) {
        WindowInstance* const window = windows[index];
        if (window != nullptr) {
            WindowRuntime_Close(window);
            windows[index] = nullptr;
        }
    }
    inventory.slotUseCounts.fill(0);
    if (skillOperation.rebirthEnabled == 0) {
        return 0;
    }
    const std::int32_t y =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerY);
    const std::int32_t x =
        static_cast<std::int32_t>(session.WorldScene().map.crypt->playerX);
    const auto networkSession = session.Connection().CaptureSession();
    const auto result = session.Connection().SendWorldActionRequest(x, y, 62);
    skillOperation.rebirthEnabled = 0;
    return static_cast<std::int32_t>(result);
}

// 原函数：0x00503E80。
void WindowRuntime_ProcessScene(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    using namespace detail;
    switch (session.WorldScene().sceneType) {
    case 5:
        WindowRuntime_ResetShortcutFlag(session);
        WindowRuntime_InitializeLogin(session);
        WindowRuntime_OpenLoginScene(session);
        return;
    case 7:
    case 11:
        WindowRuntime_MarkExistingWindows(session);
        static_cast<void>(WindowRuntime_CloseAll(session));
        static_cast<void>(WindowRuntime_SaveSettings(session));
        world::SaveAlbumData_5186A0(session);
        static_cast<void>(cg::world::detail::SaveContactMailFile_519300(session));
        static_cast<void>(cg::world::detail::SaveGuildMailFile_519550(session));
        static_cast<void>(world::detail::SavePlayerProfilesFile_534770(session));
        static_cast<void>(world::detail::SaveProfileCategoryFile_5348A0(session));
        return;
    case 9:
        switch (session.WorldScene().sceneState) {
        case 1:
            WindowRuntime_MarkSceneWindows(session);
            static_cast<void>(WindowRuntime_CloseAll(session));
            WindowRuntime_OpenNormalScene(session);
            return;
        case 4:
        case 200:
            WindowRuntime_MarkExistingWindows(session);
            static_cast<void>(WindowRuntime_CloseAll(session));
            return;
        case 204:
            WindowRuntime_OpenNormalScene(session);
            return;
        case 250:
        case 252:
            WindowRuntime_MarkExistingWindows(session);
            static_cast<void>(WindowRuntime_CloseAll(session));
            WindowRuntime_OpenNormalScene(session);
            if (session.WorldScene().sceneState == 250) {
                auto* const bytes = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
                for (std::size_t index = 0; index < 8; ++index) {
                    bytes[index] = static_cast<unsigned char>(bytes[index] + 1);
                }
            }
            return;
        default:
            return;
        }
    case 10:
        switch (session.WorldScene().sceneState) {
        case 1:
            WindowRuntime_OpenBattleScene(session);
            return;
        case 8:
            WindowRuntime_MarkSceneWindows(session);
            static_cast<void>(WindowRuntime_CloseAll(session));
            return;
        default:
            return;
        }
    default:
        return;
    }
}

// 原函数：0x00472240。
void WindowRuntime_CloseByCategory(session::ClientSession& session, const std::uint32_t mask)
{
    auto& pointer = session.UiFrame().native.pointer;
    using namespace detail;
    auto& windows = WindowRuntime_Windows(session);
    auto& states = WindowRuntime_States(session);
    for (std::size_t index = 0; index < windows.size(); ++index) {
        WindowInstance* const window = windows[index];
        if (window == nullptr) {
            continue;
        }
        const std::int32_t id = window->id;
        const auto category = static_cast<std::uint32_t>(
            static_cast<std::int32_t>(window->category));
        if ((mask & category) != 0) {
            pointer.currentWindowState = &states[id];
            pointer.currentWindowState->flags |= 2u;
            WindowRuntime_Free(windows[index]);
            windows[index] = nullptr;
        }
    }
}

int WindowRuntime_DispatchElementMouse(WindowInstance& window, ElementId id, int ox, int oy, const UIClipRect* clip) {
    return detail::WindowRuntime_DispatchElementMouseAt(window, id, ox, oy, clip);
}

void WindowRuntime_UpdateElementOrder(WindowInstance& window)
{
    window.sortedMap.notifyUpdate();
}

// WDP01：完整元素分发、低W16裁剪与现场取样；见doc/cg2d_window_element_draw_pipeline_spec.md。
// 原函数：0x004729B0。
void WindowRuntime_DrawElement(WindowInstance& window, ElementId id, int ox, int oy, const UIClipRect* clip) {
    auto& inputs = window.ownerSession->UiFrame().native.inputs;
    using namespace detail;
    WindowElementCursor lease(window);
    auto* element = window.FindElementById(id);
    if (!element || element->detaching || (window.state & 0x8000u) || window.scriptHidden) return;
    if (element->visible != 1) { WindowRuntime_BlurElement(*element); return; }
    render::GraphicDescriptor16 descriptor{};
    const int x = ox + element->x, y = oy + element->y;
    const auto firstGraphic = cg::render::gGraphicQueue.count;
    switch (element->type) {
    case WindowElementType_Graphic: {
        WindowElement_UpdateImageState(*element);
        const auto* const data = element->data.graphic;
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
            imageNo = data->imageNo;
            const std::int32_t drawElementY = element->y;
            const std::int32_t drawWindowY = oy;
            const std::int32_t drawElementX = element->x;
            const std::int32_t drawWindowX = ox;
            drawY = drawElementY + drawWindowY;
            drawX = drawElementX + drawWindowX;
        } else {
            imageNo = data->imageNo;
            const std::int32_t drawElementY = element->y;
            const std::int32_t drawElementX = element->x;
            const std::int32_t drawWindowY = oy;
            drawY = drawWindowY + drawElementY;
            const std::int32_t drawWindowX = ox;
            drawX = drawWindowX + drawElementX;
        }
        static_cast<void>(render::DrawGraphic3_520970(
            drawX, drawY, 0xDF, imageNo, 0, 1, 0, &descriptor));
        break;
    }
    case WindowElementType_Animation: {
        auto* const data = element->data.animation;
        WindowElement_PrepareAnimation(*element->owner->ownerSession, *data);
        if (data->options && !data->action) break;
        const std::uint32_t stop = data->value04;
        cg::yobi::YobiAction* const action = data->action;
        static_cast<void>(cg::yobi::AdvanceYobiAnimationFrame_517880(action, 100, stop != 0, 0));
        const std::int32_t graphicIndex = cg::yobi::GetYobiActionField20(action);
        if (graphicIndex == 0) {
            break;
        }
        const render::GraphicDescriptor16* const mode =
            cg::yobi::ApplyWindowAnimationDrawMode(action);
        const std::int32_t offsetY = cg::yobi::GetYobiGraphicOriginOffset_5182B0(action, cg::yobi::YobiGraphicAxis_Y);
        const std::int32_t animationElementY = element->y;
        const std::int32_t drawY = offsetY + oy + animationElementY;
        const std::int32_t offsetX = cg::yobi::GetYobiGraphicOriginOffset_5182B0(action, cg::yobi::YobiGraphicAxis_X);
        const std::int32_t animationElementX = element->x;
        const std::int32_t drawX = offsetX + ox + animationElementX;
        static_cast<void>(render::DrawGraphic2_521DD0(
            drawX, drawY, 0xDF, graphicIndex, 0, 0, mode));
        break;
    }
    case WindowElementType_Text:
        WindowElement_DrawText(*element, x, y, clip);
        break;
    case WindowElementType_Input: {
        WindowElement_PrepareInput(*element, ox, oy, clip);
        const auto* const data = element->data.input;
        if (data == window.ownerSession->UiFrame().native.pointer.focusedElementData) {
            if (window.ownerSession->UiFrame().inputState.currentContext == &inputs.chat) {
                if (window.ownerSession->UiFrame().inputState.chatPanelVisible == 1) {
                    WindowChat_DrawInput(*window.ownerSession, data->context);
                }
            } else if (data != nullptr && data->ownedInput != nullptr) {
                // Lua输入沿原生正文/光标坐标计算，但独立入队并使用控件实际宽度。
                render::ClearTextQueueTail_456D50(3);
                render::DrawInputText_4568A0(*window.ownerSession, data->context);
                render::ClearTextQueueTail_456D50(3);
            } else if (data != nullptr) {
                WindowChat_DrawFixedWidthInput(*window.ownerSession, data->context);
            }
        }
        break;
    }
    case WindowElementType_InputCaret:
        WindowChat_DrawHistory(*window.ownerSession, x + 5, y);
        break;
    case WindowElementType_Number: {
        const auto* const data = element->data.number;
        if (data != nullptr) {
            const std::uint8_t flags = data->flags;
            const std::int8_t color = std::bit_cast<std::int8_t>(data->color);
            const std::int8_t style = data->style;
            const std::wstring& text = *data->text;
            WindowNumber_Draw(x, y, text, style, color, flags, 0xDF);
        }
        break;
    }
    case WindowElementType_PngImage: WindowElement_DrawPng(*element, ox, oy, clip); break;
    case WindowElementType_Canvas: WindowElement_DrawCanvas(*element, ox, oy, clip); break;
    case WindowElementType_MapEx: element->data.mapEx->Render(*element, ox, oy, clip); break;
    case WindowElementType_ScrollView: WindowElement_DrawScroll(*element, ox, oy, clip); break;
    default:
        break;
    }
    if (clip) WindowRuntime_ClipGraphics(firstGraphic, *clip);
}

void WindowRuntime_DrawCurrent(WindowInstance& target)
{
    using namespace detail;
    WindowInstance* const window = &target;
    WindowRuntime_UpdateElementOrder(*window);
    WindowElementCursor cursor(*window);
    while (auto* element = cursor.next()) {
        if (element->parentId) continue;
        WindowRuntime_DrawElement(*window, element->elementId, window->x, window->y, nullptr);
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    world::DrawChatRangeOverlay_476680(*target.ownerSession);
}

// 原代码位置：0x00440ADB。
void WindowRuntime_ApplyObservationInputGate(session::ClientSession& session)
{
    auto& inventory = session.UiFrame().native.inventory;
    auto& skillOperation = session.UiFrame().native.skillOperation;
    auto* const bytes = reinterpret_cast<unsigned char*>(inventory.slotUseCounts.data());
    if (skillOperation.rebirthEnabled != 0) {
        const unsigned char previous = bytes[7];
        skillOperation.rebirthEnabled = 0;
        bytes[7] = static_cast<unsigned char>(previous - 1);
    }
    for (std::size_t index = 0; index < 8; ++index) {
        bytes[index] = static_cast<unsigned char>(bytes[index] + 1);
    }
}

int WindowDrag_ReadMousePhase(session::ClientSession& session)
{
    return session.UiFrame().native.pointer.phase;
}

void WindowDrag_ConsumeDrop(session::ClientSession& session)
{
    auto& pointer = session.UiFrame().native.pointer;
    if (pointer.phase != 4) return;
    if (pointer.callback) {
        pointer.callback(session, 4, 0, pointer.sourceIndex);
    }
    static_cast<void>(detail::WindowDrag_ResetMouse(session));
}
} // namespace cg::ui
