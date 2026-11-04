#pragma once

#include "cg/ui/WindowRuntime.hpp"
#include "cg/ui/NativeWindowRuntimeState.hpp"
#include "WindowStorageRecords.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "../world/PlayerState.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <map>
#include <memory>
#include <variant>
#include <set>
#include <vector>
#include <unordered_map>
#include <type_traits>

namespace cg::input {
class InputContext;
}

namespace cg::world {
struct OrderedRecord1051B14;

struct PlayerItemRecord;

} // namespace cg::world

namespace cg::world::detail {
struct ContactRuntimeRecord;
struct MiniMailContactRecord;
}

namespace cg::ui {

struct WindowPngImageData;
struct WindowCanvasData;
struct WindowMapExData;
struct WindowScrollViewData;
struct WindowBinding;
struct WindowAnimationOptions;
struct WindowTextInputState;
// LUP01：Lua扩展属性由UI对象拥有，仅允许原始字符串和S32整数。
using WindowPropertyMap = std::map<std::string, std::variant<std::string, std::int32_t>>;
// 原型业务槽使用固定ID：首项为1，后续槽按枚举基址展开，0留给自动分配。
enum class NativeElementId : ElementId { First = 1 };
constexpr ElementId NativeElementKey(std::int32_t slot) noexcept {
    return slot < 0 ? 0 : static_cast<ElementId>(NativeElementId::First) + static_cast<ElementId>(slot);
}

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
    WindowElementType_PngImage = 8, // LUP01：PNG组件。
    WindowElementType_Canvas = 9, // LUP01：画布组件。
    WindowElementType_MapEx = 10, // LUP01：地图组件。
    WindowElementType_ScrollView = 11, // LUP01：滚动容器组件。
};
static_assert(sizeof(WindowElementType) == sizeof(std::int32_t));

enum WindowMouseState : std::uint8_t { WindowMouseState_None, WindowMouseState_Hover, WindowMouseState_Press };


// 原type1的24B前缀保留；NEU01在宿主载荷末尾扩充图态配置。
struct WindowGraphicData {
    std::int32_t imageNo; // 原成员偏移：+0x00。
    std::int32_t offsetX; // 原成员偏移：+0x04。
    std::int32_t offsetY; // 原成员偏移：+0x08。
    std::int32_t width; // 原成员偏移：+0x0C。
    std::int32_t height; // 原成员偏移：+0x10。
    std::uint32_t color; // 原成员偏移：+0x14。
    std::int32_t imageHover = 0; // NEU01：可选悬停图，0沿用正常图。
    std::int32_t imagePress = 0; // 可选按下图。
    std::int32_t imageNormal = 0; // 图态配置的正常图；imageNo仍为当前绘制图。
};

// 原type5首槽是真实输入上下文引用；外层载荷拥有、上下文不拥有。
struct WindowInputData {
    input::InputContext* context; // 原成员偏移：+0x00。
    WindowTextInputState* ownedInput = nullptr; // NEU01：可选自有输入状态；原生上下文继续借用。
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
    cg::yobi::YobiAction* action; // 原成员偏移：+0x00。
    std::uint32_t value04; // 原成员偏移：+0x04。
    WindowAnimationOptions* options = nullptr; // NEU01：可选资源/动作配置，action为唯一动画资源。
};

// 文本实例与原型共用的4B样式；原472C35读颜色、472C3C读字体，不含指针。
struct WindowTextStyle {
    std::byte color; // 原成员偏移：+0x00；颜色索引，按原位型消费。
    std::byte font; // 原成员偏移：+0x01；字体编号。
    std::byte reserved02; // 原成员偏移：+0x02；保留字节。
    std::byte reserved03; // 原成员偏移：+0x03；保留字节。
};
static_assert(sizeof(WindowTextStyle) == 4);
static_assert(alignof(WindowTextStyle) == 1);
static_assert(offsetof(WindowTextStyle, color) == 0);
static_assert(offsetof(WindowTextStyle, font) == 1);
static_assert(offsetof(WindowTextStyle, reserved02) == 2);
static_assert(offsetof(WindowTextStyle, reserved03) == 3);

// LEL01：布局是文本能力，不按窗口来源判断；原生载荷默认保留锚点语义。
enum class WindowTextLayout : std::uint8_t { NativeAnchor, Box };
enum class WindowTextAlign : std::uint8_t { Left = 0, Center = 1, Right = 2 };

// 原第3类元素的8B载荷；文本由47167F分配，47134F释放。
// UTL02：动态UTF-16正文；原256B私有容量退休，协议/资源模型另行承载。
struct WindowTextData {
    WindowTextStyle style; // 原成员偏移：+0x00。
    std::wstring text; // 原x86成员偏移：+0x04（文本指针）；UTL02动态UTF-16正文，当前按原生对齐。
    WindowTextLayout layout = WindowTextLayout::NativeAnchor; // Box以元素左上角和尺寸布局。
    WindowTextAlign align = WindowTextAlign::Left; // Box水平对齐，垂直方向保持顶对齐。
};

// 原type4的12B鼠标载荷；滚动回调与元素命中共用，不含地址。
struct WindowMouseData {
    std::uint32_t flags; // 原成员偏移：+0x00。
    std::int32_t deltaX; // 原成员偏移：+0x04。
    std::int32_t deltaY; // 原成员偏移：+0x08。
};

// WEO01：由WindowElement::type标记活动成员，载荷所有权仍由窗口管理。
union WindowElementData {
    WindowGraphicData* graphic;
    WindowAnimationData* animation;
    WindowTextData* text;
    WindowMouseData* mouse;
    WindowInputData* input;
    std::uint32_t* inputCaret;
    WindowNumberData* number;
    WindowPngImageData* pngImage;
    WindowCanvasData* canvas;
    WindowMapExData* mapEx;
    WindowScrollViewData* scrollView;
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
    WindowElementData data; // 原+1C；按type访问对应载荷指针。
    WindowElementCallback mouseCallback; // 原+20。
    std::int32_t seqNo; // WEO01：窗口内绘制/鼠标顺序，小值在前，同值按原索引。
    WindowPropertyMap* properties = nullptr; // 按需创建，随控件释放。
    std::int32_t refUpdate = -1; // Lua registry引用，桥接负责调用/解绑。
    std::int32_t refDraw = -1;
    std::int32_t refEvent = -1;
    std::int32_t refClick = -1;
    std::int32_t refPress = -1;
    std::int32_t refHover = -1;
    std::int32_t refLeave = -1;
    std::int32_t refDrag = -1;
    std::int32_t refDrop = -1;
    std::int32_t refChange = -1;
    std::int32_t refEnter = -1;
    ElementId elementId = 0; // LUR01：窗口内唯一身份，不随顺序改变。
    WindowInstance* owner = nullptr;
    ElementId relatedElementId = 0; // 滚动条关联滑块，替代element-1。
    std::wstring name; // NEU01：元素名称，脚本属性与原生元素共用。
    ElementId parentId = 0; // 所属滚动容器，0表示窗口根元素。
    WindowMouseState mouseState = WindowMouseState_None; // 通用交互状态，不复用原生state位。
    bool hitable = true; // 元素是否参加鼠标命中。
    bool detaching = false; // 失焦前即停止交互，尚未从活动表提取。
    bool removed = false; // 回调中立即失效，安全点回收。
    bool ownsPayload = true; // 载荷及其自有资源的释放责任；自动战斗窗口借用成员数组时为false。
    std::uint64_t creationSerial = 0; // 游标按创建序号上限跳过遍历期间新增的元素。
};

// LUR01：显式通知单一sortedMap更新排序，遍历中调用会记录日志并抛异常。
void WindowRuntime_UpdateElementOrder(WindowInstance& window);

// 原实例+40的16B可拖动区域，472D20逐项命中。
struct WindowDragRect {
    std::int32_t x; // 原成员偏移：+0x00。
    std::int32_t y; // 原成员偏移：+0x04。
    std::int32_t width; // 原成员偏移：+0x08。
    std::int32_t height; // 原成员偏移：+0x0C。
};

// SEV01：有序vector独占元素对象，ID索引借用稳定对象地址，不复制元素。
class SortedElementMap {
public:
    using OrderKey = std::pair<std::int32_t, ElementId>; // notifyUpdate发布的排序键。
    using ElementOwner = std::unique_ptr<WindowElement>; // 活动与退役列表之间转移的唯一owner。
private:
    using Storage = std::vector<std::pair<OrderKey, ElementOwner>>;
    Storage values;
    std::unordered_map<ElementId, WindowElement*> byId;
    std::uint32_t traversalDepth = 0;
public:
    using iterator = Storage::iterator;
    auto begin() noexcept { return values.begin(); }
    auto end() noexcept { return values.end(); }
    auto begin() const noexcept { return values.begin(); }
    auto end() const noexcept { return values.end(); }
    iterator upper_bound(const OrderKey& key); // 游标按已发布键重新定位，不缓存vector迭代器。
    auto size() const noexcept { return values.size(); }
    bool empty() const noexcept { return values.empty(); }
    void notifyUpdate();
    void beginTraversal() { ++traversalDepth; }
    void endTraversal() { --traversalDepth; }
    bool isTraversing() const noexcept { return traversalDepth != 0; } // 有游标借用时禁止回收退役对象。
    // O(1)按已发布(seqNo, elementId)顺序从0查找；负数或越界返回nullptr。
    // 返回借用指针；index不是elementId，增删或notifyUpdate后位置可能改变。
    WindowElement* elementAtIndex(std::int32_t index) noexcept {
        if (index < 0 || static_cast<std::size_t>(index) >= values.size()) return nullptr;
        return values[static_cast<std::size_t>(index)].second.get();
    }
    const WindowElement* elementAtIndex(std::int32_t index) const noexcept {
        if (index < 0 || static_cast<std::size_t>(index) >= values.size()) return nullptr;
        return values[static_cast<std::size_t>(index)].second.get();
    }
    // 单次ID查询；不存在或已逻辑删除时返回nullptr，不插入元素。
    WindowElement* FindElementById(ElementId id) noexcept {
        const auto it = byId.find(id);
        return it == byId.end() || it->second->removed ? nullptr : it->second;
    }
    const WindowElement* FindElementById(ElementId id) const noexcept {
        const auto it = byId.find(id);
        return it == byId.end() || it->second->removed ? nullptr : it->second;
    }
    template<class E> requires std::is_enum_v<E>
    WindowElement* FindElementById(E id) noexcept { return FindElementById(static_cast<ElementId>(id)); }
    template<class E> requires std::is_enum_v<E>
    const WindowElement* FindElementById(E id) const noexcept { return FindElementById(static_cast<ElementId>(id)); }
    WindowElement* operator[](ElementId id) noexcept { return FindElementById(id); }
    const WindowElement* operator[](ElementId id) const noexcept { return FindElementById(id); }
    WindowElement* insert(ElementId id); // 初始键为(0,id)，失败回滚；重复ID返回nullptr。
    ElementOwner extract(WindowElement& element) noexcept; // 按对象身份移出，避免误删同ID新对象。
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
    SortedElementMap sortedMap; // 唯一元素存储，按seqNo/ID排序，按ID高效查询。
    WindowCallback update; // 原+30。
    WindowCallback draw; // 原+34。
    WindowReleaseCallback release; // 原+38。
    std::int32_t secondaryCount; // 原+3C。
    std::array<WindowDragRect, 10> dragRects40; // 原[+40,+E0)。
    std::vector<SortedElementMap::ElementOwner> retiredElements; // 延迟释放的独占对象，子孙排在父对象前。
    WindowBinding* binding = nullptr; // LUP01：随窗口释放的外部事件绑定。
    std::uint32_t generation = 0; // LUP01：每次创建实例分配新代次。
    WindowPropertyMap* properties = nullptr; // 按需创建，随窗口释放。
    std::int32_t refUpdate = -1;
    std::int32_t refDraw = -1;
    std::int32_t refEvent = -1;
    bool scriptHidden = false; // 旧原生state不含统一visible位，脚本显隐独立门控。
    std::int8_t category = 0; // 窗口类别位码；原生窗口创建时赋值，类别判断直接读取实例。
    session::ClientSession* ownerSession = nullptr; // NSM01：原生/扩展窗口的稳定会话 owner，生命周期内不随菜单切换。
    // 返回借用指针；跨回调或结构变更后需重新查询。
    WindowElement* FindElementById(ElementId elementId) noexcept {
        return sortedMap.FindElementById(elementId);
    }
    const WindowElement* FindElementById(ElementId elementId) const noexcept {
        return sortedMap.FindElementById(elementId);
    }
    template<class E> requires std::is_enum_v<E>
    WindowElement* FindElementById(E id) noexcept { return FindElementById(static_cast<ElementId>(id)); }
    template<class E> requires std::is_enum_v<E>
    const WindowElement* FindElementById(E id) const noexcept { return FindElementById(static_cast<ElementId>(id)); }
    WindowElement* operator[](ElementId elementId) noexcept { return FindElementById(elementId); }
    const WindowElement* operator[](ElementId elementId) const noexcept { return FindElementById(elementId); }
    template<class E> requires std::is_enum_v<E>
    WindowElement* operator[](E id) noexcept { return (*this)[static_cast<ElementId>(id)]; }
    template<class E> requires std::is_enum_v<E>
    const WindowElement* operator[](E id) const noexcept { return (*this)[static_cast<ElementId>(id)]; }
};

ElementId WindowRuntime_AllocateElementId(const WindowInstance& window, ElementId requested = 0);
WindowElement& WindowRuntime_AddElement(WindowInstance& window, ElementId id);
bool WindowRuntime_RemoveElement(WindowInstance& window, ElementId id);
void WindowRuntime_ClearElements(WindowInstance& window);
void WindowRuntime_CollectElements(WindowInstance& window);
void WindowRuntime_DestroyElements(WindowInstance& window);
// 不保存vector迭代器，回调增删后以已发布排序键继续；本轮不处理新建元素。
struct WindowElementCursor {
    WindowInstance& window;
    std::pair<std::int32_t, ElementId> key{};
    std::uint64_t limit = 0;
    bool started = false;
    explicit WindowElementCursor(WindowInstance& value);
    ~WindowElementCursor() { window.sortedMap.endTraversal(); }
    WindowElementCursor(const WindowElementCursor&) = delete;
    WindowElementCursor& operator=(const WindowElementCursor&) = delete;
    WindowElement* next();
};

// 旧选择器缓存改为身份引用，元素删除后查询返回空。
WindowInstance* WindowRuntime_FindInstance(int id);
WindowInstance* WindowRuntime_FindInstance(session::ClientSession& session, int id);

namespace detail {

// L05：扩展自动战斗面板沿自身实例复用原生元素鼠标分派。
void WindowRuntime_UpdateExtendedElements(WindowInstance& window);
void WindowBattleCommand_FinishAutoAction(session::ClientSession& session);

// G07：创建窗口时预留页按钮；载荷沿原type1分配／释放，原控件地址不变。
std::int32_t WindowInventory_GetPageElementCount(session::ClientSession& session, std::int32_t windowId);
bool WindowInventory_CreatePageElements(WindowInstance* window, std::int32_t originalCount);

// 原函数：0x0047A140。
void WindowSkill_SyncStates(session::ClientSession& session); // 窗口10模式2的技能状态差异请求，原返回无人消费。
// 原函数：0x0047A490。
cg::yobi::YobiAction* WindowSkill_Open(session::ClientSession& session); // 清选择回调并创建普通技能窗。
// 原函数：0x0047A4C0。
cg::yobi::YobiAction* WindowSkill_OpenBattleSkills(session::ClientSession& session, SkillWindowSelectionCallback callback); // 设置回调并创建战斗列表窗。
// 原函数：0x0047A500。
cg::yobi::YobiAction* WindowSkill_OpenBattleTechniques(session::ClientSession& session, std::int32_t skillIndex, SkillWindowSelectionCallback callback); // 战斗技术窗。
// 原函数：0x0047A540。
cg::yobi::YobiAction* WindowSkill_OpenPetSkills(session::ClientSession& session, PetSkillSelectionCallback callback); // 设置CC4D14并创建宠物技能窗。
// 原函数：0x0047A580。
cg::yobi::YobiAction* WindowSkill_OpenRidingSkills(session::ClientSession& session, PetSkillSelectionCallback callback); // 设置CC4D14并创建骑乘技能窗。
// 原函数：0x00497000。
void WindowBattleCommand_OpenSkills(session::ClientSession& session); // 战斗技能快捷入口，保留actionFlags低S8门槛；原返回不用。
// 原函数：0x00496FB0。
std::int32_t WindowBattleCommand_CanSelectSkill(session::ClientSession& session); // 技能可用性：门禁失败/已选返回0，否则-1。
// 原函数：0x004970B0。
std::int32_t WindowBattleCommand_CanSelectItem(session::ClientSession& session); // 同一门禁，读取物品选择槽3。
// 原函数：0x00497100。
void WindowBattleCommand_OpenItems(session::ClientSession& session); // 物品快捷入口，清目标待选状态并打开27。
// 原函数：0x004971C0。
std::int32_t WindowBattleCommand_CanSelectPet(session::ClientSession& session); // 同一门禁，读取宠物选择槽4。
// 原函数：0x00497210。
void WindowBattleCommand_OpenPets(session::ClientSession& session); // 宠物快捷入口，打开28，不清物品待选状态。
// 原函数：0x004708A0。
std::int32_t WindowStatusBar_ButtonEmote(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口34存在时更新图形，点击开关。
// 原函数：0x00470910。
std::int32_t WindowStatusBar_ButtonMiniMap(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口33开关，创建使用专有尺寸入口。
// 原函数：0x00495F00。
cg::yobi::YobiAction* WindowMiniMap_Open(
    session::ClientSession& session,
    std::uint32_t flags, std::uint32_t mode, cg::yobi::YobiAction** completionSlot); // 小地图184x159展开，完成槽原生T**。
// 原函数：0x00495F70。
std::int32_t WindowEmote_Select(session::ClientSession& session, std::int32_t index); // 清18B后选中指定字节，Y/X快照交4D3020。
// 原函数：0x00498520。
void WindowBattleCommand_Dispatch(session::ClientSession& session); // 完整六态战斗命令分派，所有出口共用倒计时尾。
// 原函数：0x0043C950。
void WindowBattleStateNotice_Open(session::ClientSession& session); // 玩家入场位且动画/窗口32均空才创建，失败也播放原音效。
// 原函数：0x00496570。
void WindowBattleCommand_SnapshotPetVitals(session::ClientSession& session); // 五宠物四项CryptInt快照，唯一CF4B78数组生产。
// 原函数：0x004965D0。
void WindowBattleCommand_SyncActorVitals(session::ClientSession& session); // 战斗对象四值更新，保留独立5ABBFC及原空值门禁。
// 原函数：0x0047A5C0。
std::int32_t WindowSkill_Back(session::ClientSession& session); // 战斗返回：模式4退列表，0/3/5/6关闭。
// 原函数：0x00479650。
void WindowRuntime_IncrementOperationCount(session::ClientSession& session); // 原窗口操作S32计数加1。
// 原函数：0x00479660。
void WindowRuntime_DecrementOperationCount(session::ClientSession& session); // 原操作计数减1及严格零时的退出发送/技能状态刷新。
// 原函数：0x0047D170。
std::int32_t WindowCraft_SetPreviewPositions(
    WindowInstance* window, std::int32_t anchor, std::int32_t first, std::int32_t second,
    std::int32_t dx, std::int32_t dy); // 双预览定位/显隐，返回原36B元素数值偏移。
// 原函数：0x0047D220。
WindowAnimationData* WindowCraft_ConfigurePreviews_47D220(
    WindowInstance* window, std::int32_t first, std::int32_t second,
    std::int32_t flag, std::int32_t actionId, std::int32_t direction); // 返回第二个借用载荷。
// 原函数：0x0047D5F0。
cg::yobi::YobiAction* WindowCraft_OpenRecipe(
    session::ClientSession& session, std::int32_t skill, std::int32_t recipe, std::int32_t request); // 制造开窗，保存同源配方id。
// 原函数：0x0047D780。
cg::yobi::YobiAction* WindowCraft_OpenItemWork(session::ClientSession& session, std::int32_t skill, std::int32_t request); // 单选加工开窗。
// 原函数：0x0047D8F0。
cg::yobi::YobiAction* WindowCraft_OpenDecoration(
    session::ClientSession& session, std::int32_t skill, std::int32_t request, std::int32_t kind); // 装饰开窗，kind严格1为宠物装备。
// 原函数：0x0047DA70。
cg::yobi::YobiAction* WindowCraft_OpenRefining(session::ClientSession& session, std::int32_t skill, std::int32_t request); // 精炼开窗，条件设置世界状态9。
// 原函数：0x0047DB50。
cg::yobi::YobiAction* WindowCraft_OpenMaterials(session::ClientSession& session, std::int32_t skill, std::int32_t request); // 材料开窗，无条件设置世界状态9。
// 原函数：0x004F4430。
std::int32_t WindowCraft_ClassifySpecialMaterial(std::int32_t itemType, std::int32_t skillId); // 技能分类对应的特殊材料类别。
// 原函数：0x00482F70。
std::int32_t WindowCraft_ClassifyWork(std::int32_t itemType); // 加工分类；类别7优先归2。
// 原函数：0x004F39F0。
WindowAnimationData* WindowCraft_ReceiveRecipeResult(session::ClientSession& session, std::int32_t result, const char* text); // JgO制造分支，原字节摘要与展示。
// 原函数：0x004F1800。
WindowAnimationData* WindowCraft_ReceiveItemWorkResult(session::ClientSession& session, std::int32_t result, const char* text); // JgO第二分支七字段及独立摘要。
// 原函数：0x004796E0。
void WindowRuntime_DecrementGatherOperationCount(session::ClientSession& session); // 只递减既有D67AFC低U8，不清其他操作计数。
// 原函数：0x004796D0。
void WindowRuntime_IncrementGatherOperationCount(session::ClientSession& session); // 仅递增既有D67AFC低U8。
// 原函数：0x004854A0。
cg::yobi::YobiAction* WindowGather_Open(session::ClientSession& session, std::int32_t skill, std::int32_t level); // 低U8请求状态及窗口12创建。
// 原函数：0x0047D2E0。
WindowAnimationData* WindowCraft_AnimatePreviewPositions(
    WindowInstance* unusedWindow, std::int32_t anchor,
    std::int32_t first, std::int32_t second, std::uint64_t deadline); // 双预览六点插值，实际借当前窗口。
// 原函数：0x004ED6B0。
WindowAnimationData* WindowGather_ReceiveResult(session::ClientSession& session, std::int32_t result, const char* text); // JgO第四分支，失败不解析正文。
// 原函数：0x0047D280。
std::int32_t WindowCraft_SetPreviewImages(
    WindowInstance* window, std::int32_t first, std::int32_t second,
    std::int32_t imageNo); // 双动画图号，非正才隐藏；返回原数值元素偏移。
// 原函数：0x00487F30。
std::int32_t WindowSkillResult_ReadPreviewImage(std::int32_t imageNo, std::int32_t offset); // 技能预览分组图号转换。
// 原函数：0x004EEA40。
cg::yobi::YobiAction* WindowSkillResult_ReceiveResult(session::ClientSession& session, std::int32_t result, const char* text); // JgO第三分支，七字段及窗口13。
// 原函数：0x00479860。
std::int32_t WindowSkill_ReadCrystalLevel(session::ClientSession& session); // 玩家物品槽7的晶石等级，未用/未知返回0。
// 原函数：0x004799D0。
std::int32_t WindowSkill_ReadBattlePetImage(session::ClientSession& session); // 重新选取出战宠物并读取原图号，未命中0。
// 原函数：0x00479AB0。
std::int32_t WindowSkill_CheckPetFp(session::ClientSession& session); // 当前宠物FP的单次CryptInt比较。
// 原函数：0x00479B00。
std::int32_t WindowSkill_CanUseSpecialSkill(session::ClientSession& session); // 技能/晶石/宠物技术/属性/等级条件链。
// 原函数：0x0049F000。names借用宽文本名单，刷新时直接设置正文。
cg::yobi::YobiAction* WindowTextTarget_Open(
    session::ClientSession& session, const wchar_t* const* names, const std::uint8_t* colors,
    std::int32_t count, WindowTextChoiceCallback callback);

// 原函数：0x0049F760。
cg::yobi::YobiAction* WindowAttributeTarget_Open(
    session::ClientSession& session, const WindowStatusChoiceRecord* records, std::int32_t count,
    WindowTextChoiceCallback callback); // 三槽按序发布后打开46，透传借用Yobi。
// 原函数：0x00499830。
std::int32_t WindowRuntime_DrawLargeFrame(WindowInstance& callbackWindow, std::int32_t value); // 31/46共用绘制与九宫格尾。

// 原宠物装备栏图98147的六槽中心坐标；详情装备页共用，顺序与items6E4一致。
extern const std::array<std::int32_t, 6> gSelectorPetX54F5C4;
extern const std::array<std::int32_t, 6> gSelectorPetY54F5DC;
// 原函数：0x004C61B0。
WindowItemSelectorState* WindowItemSelector_ConstructPreview(session::ClientSession& session, WindowItemSelectorState* state, std::int32_t previewIndex); // SIC01：入口预览地址贯穿十成员构造与字段发布；原初始化/三descriptor及this返回保持，整对象不清零。
// SLC01：单读并返回同一选择器selectedPet70的完整S32，不做校验或改owner。
// 原函数：0x004C71B0。
std::int32_t WindowItemSelector_ReadSelectedPet(const WindowItemSelectorState* state); // 原+70同源S32。
// 原函数：0x004C6EC0。
void WindowItemSelector_BindElements(WindowInstance* callbackWindow, WindowItemSelectorState* state, std::uint8_t mode); // SFD01：低BYTE模式装配非拥有元素/四回调；逐次借当前窗口，非零模式槽19不写，发布次序保持。
// 原函数：0x004C7380。
bool WindowItemSelector_HandleTab(WindowInstance* callbackWindow, WindowItemSelectorState* state, std::int32_t tab, std::int32_t events); // SEV01：页签提示/点击/绘制与图号原序；中心坐标显式取样，descriptor地址先图号；bool限原AL消费。
// 原函数：0x004C78A0。
bool WindowItemSelector_TogglePreview(WindowItemSelectorState* state, std::int32_t events); // SEV01：原event低BYTE bit2触发声音后翻转BYTE预览开关，图号8/1优先，正确body保持，bool仅原AL。
// 原函数：0x004C7910。
bool WindowItemSelector_CycleEquipment(session::ClientSession& session, WindowItemSelectorState* state, std::int32_t events); // SEV01：先翻BYTE装备/回调，失效AL早返保部分写；目标先回调、尾selected先推进/count后取，不钳制数量。
// 原函数：0x004C7A10。
bool WindowItemSelector_RotatePreview(WindowItemSelectorState* state, std::int32_t events); // SEV01：低U16两个方向门槛独立，先正向S32余8再反向减1/原加8；两声效/原顺序保，父固定返回0。
// 原函数：0x004C7A90。
void WindowItemSelector_Update(session::ClientSession& session, WindowInstance* callbackWindow, WindowItemSelectorState* state); // SFD01：首次借mode地址、previous=-1、切页后置initialized；尾逐帧分发，上行两窗口回调固定返回1。
// ISP01：原local负值返-1，成功后取当前窗口，再按元素X/窗口X取样累加。
// 原函数：0x004C7240。
std::int32_t WindowItemSelector_ReadScreenX(WindowInstance* callbackWindow,
    const WindowItemSelectorState* state, std::int32_t element, std::int32_t slot,
    std::int32_t offset); // X；负局部坐标不读取窗口。
// ISP01：原local负值返-1，成功后取当前窗口，再按元素Y/窗口Y取样累加。
// 原函数：0x004C7280。
std::int32_t WindowItemSelector_ReadScreenY(WindowInstance* callbackWindow,
    const WindowItemSelectorState* state, std::int32_t element, std::int32_t slot,
    std::int32_t offset); // Y；与X分开采样模式和当前窗口。
// ISP01：S8模式0借玩家前8槽，正模式经原宠物校验后两次重读索引借列表/数量；失败不改输出。
// 原函数：0x004C72C0。
bool WindowItemSelector_GetItemList_4C72C0(
    session::ClientSession& session,
    const WindowItemSelectorState* state, world::PlayerItemRecord** items,
    std::int32_t* count); // 借用同源物品表；失败不写输出。
// ISP01：借同一列表/原动态count，state低BYTE消费；循环尾先重取count再推进槽。
// 原函数：0x0048A6C0。
std::int32_t WindowInventory_DrawBlockedEquipment(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 装备屏蔽页完整绘制。
// ISP01：原两mode选物/发送/独立取消及绘制保持，按钮和mode2 flags只读原BYTE。
// 原函数：0x00489F00。
std::int32_t WindowInventory_HandleItemSelection(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 两模式物品选择/退出与完整绘制。
// ISP01：对话29选物先占用/发布两个回调与mode2，启动返回丢弃，再透传焦点绑定S32结果。
// 原函数：0x004888C0。
std::int32_t WindowInventory_OpenSpecialSelection(session::ClientSession& session, const char* text); // 对话29选择入口，发布特殊回调后绑定焦点。
// 原函数：0x00488FB0。
std::int32_t WindowInventory_HandleItems(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 普通背包完整交互/绘制。
// 原函数：0x00489760。
std::int32_t WindowInventory_HandleEquipment(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // IER01：库存装备完整原根，四读取合同对齐；原S32返回/全部callee与运行待。
// 原函数：0x004C75A0。
bool WindowItemSelector_HandlePlayerAttributes(WindowInstance* callbackWindow, WindowItemSelectorState* selector, std::int32_t state); // SAP01：附加属性页/回调恢复，bool仅原AL，两父低AL归一；完整业务生产/运行待。
// 原函数：0x00499C30。
std::int32_t WindowBattleItems_HandleInventory(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // BIP01：完整战斗背包/目标生产，六BYTE及六坐标取序明确；旧格式/CAB30保持，全生命周期与运行待。
// 原函数：0x0049AC40。
std::int32_t WindowBattleItems_HandleEquipment(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // BEI01：战斗装备/宠物禁用页完整原根，三BYTE及区域描边取序明确，全callback/命令与运行待。

// 原函数：0x004A0140。
cg::yobi::YobiAction* WindowShopChoice_OpenBuySell(session::ClientSession& session, const char* text); // 商店图号/标题/五行正文及买卖选择。
// 原函数：0x004B6E00。
cg::yobi::YobiAction* WindowShop_OpenBuy(session::ClientSession& session, const char* text); // 窗口53买入商品及三组四行正文。
// 原函数：0x004B70D0。
cg::yobi::YobiAction* WindowShop_OpenSell(session::ClientSession& session, const char* text); // 窗口53出售商品、库存映射及占用。
// 原函数：0x004B74E0。
cg::yobi::YobiAction* WindowShop_OpenExchange(session::ClientSession& session, const char* text); // 窗口53模式2五字段商品及指定物品文本。
// 原函数：0x004B77E0。
cg::yobi::YobiAction* WindowShop_OpenItemDetails(session::ClientSession& session, const char* text); // 窗口53模式3独立详情，保留其他商品状态。
// 原函数：0x004B7AB0。
cg::yobi::YobiAction* WindowShop_OpenIdentify(session::ClientSession& session, const char* text); // 窗口53鉴定，库存映射保留未匹配物品哨兵20。
// 原函数：0x004B7EE0。
cg::yobi::YobiAction* WindowShop_OpenRepair(session::ClientSession& session, const char* text); // 窗口53修理，与鉴定仅模式和提示不同。
// 原函数：0x004BD540。
cg::yobi::YobiAction* WindowPetSkillChoice_OpenLearn(session::ClientSession& session, const char* text); // 窗口54模式0，五行正文及八行说明。
// 原函数：0x004BD680。
cg::yobi::YobiAction* WindowPetSkillChoice_OpenForget(session::ClientSession& session, const char* text); // 窗口54模式1，固定十五项名称/双数值。
// 原函数：0x004BD810。
cg::yobi::YobiAction* WindowPetSkillChoice_OpenPetSkills(session::ClientSession& session, const char* text); // 窗口54模式2，技能报价及旧窗刷新。
// 原函数：0x004BDAD0。
cg::yobi::YobiAction* WindowPetSkillChoice_OpenPetSkillsByTribe(session::ClientSession& session, const char* text); // 同创建流程，模式12显示种族。
// 原函数：0x004BDD90。
void WindowPetSkillChoice_ReceivePetCandidates(session::ClientSession& session, const char* text); // 模式3候选回复，仅更新存在的窗口54。
// 原函数：0x004BDF70。
void WindowPetSkillChoice_ReceiveTribeCandidates(session::ClientSession& session, const char* text); // 模式13候选回复，带种族条件及提示标记。
// 原函数：0x004BE200。
void WindowPetSkillChoice_ReceiveSelectedPetSkills(session::ClientSession& session, const char* text); // 模式4固定十槽回复，空名不终止后续槽。
// 原函数：0x004C5790。
cg::yobi::YobiAction* WindowAppearance_Open(
    session::ClientSession& session, std::int32_t request, std::int32_t target, const char* text); // 两行正文后打开外观选择56。
// 原函数：0x004C6060。
cg::yobi::YobiAction* WindowAppearance_OpenDialogPreview(
    session::ClientSession& session, std::int32_t buttons, std::int32_t request, std::int32_t target,
    const char* text); // 类型37内嵌图号及左侧14空格正文。
// 原函数：0x004C13B0。
cg::yobi::YobiAction* WindowCrystalShop_Open(session::ClientSession& session, const char* text); // 窗口69商品，remaining非-1才可能下调limit。
// 原函数：0x004C3550。
cg::yobi::YobiAction* WindowCrystalExchangeShop_Open(session::ClientSession& session, const char* text); // 窗口70七组正文/附加值，不下调limit。
// 原函数：0x004C2460。
std::int32_t WindowCrystalTrade_ButtonCancel(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 两窗口共用取消，发送command2。
// 原函数：0x004C4680。
std::int32_t WindowCrystalTrade_DrawButton(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 两窗口共用按钮图，恒0。
// 原函数：0x004A0300。
cg::yobi::YobiAction* WindowShopChoice_OpenFirstCommandList(session::ClientSession& session, const char* text);
// 原函数：0x004A04A0。
cg::yobi::YobiAction* WindowShopChoice_OpenSecondCommandList(session::ClientSession& session, const char* text);
// 原函数：0x004A0640。
cg::yobi::YobiAction* WindowShopChoice_OpenSkillChoice(session::ClientSession& session, const char* text); // 技能三选项。
// 原函数：0x004A0790。
cg::yobi::YobiAction* WindowShopChoice_OpenExchangeChoice(session::ClientSession& session, const char* text); // 交易两选项。
// 原函数：0x004A0850。
cg::yobi::YobiAction* WindowShopChoice_OpenGiveReceiveChoice(session::ClientSession& session, const char* text); // 给出/接收标签，正文取第四字段。
// 原函数：0x0049C4D0。
void WindowDialog_Reset(session::ClientSession& session); // 只清内嵌图号/计数及窗口36十元素可见性。
// 原函数：0x0049C510。
void WindowDialog_AddImage(
    session::ClientSession& session, std::uint8_t used, std::int32_t imageNo, std::uint8_t direction,
    std::int16_t x, std::int16_t y);
// 原函数：0x0049C600。
cg::yobi::YobiAction* WindowDialog_Open(
    session::ClientSession& session, std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height,
    std::int32_t imageNo, std::int32_t buttons, WindowDialogTextLine* lines,
    std::int32_t lineCount, std::int32_t lineHeight, std::int32_t firstSelectableLine,
    std::uint8_t inputCount, std::int16_t* inputY, std::int32_t inputCapacity,
    std::uint8_t distance, std::uint8_t options,
    WindowDialogReplyCallback reply, WindowDialogFrameCallback frame,
    std::uint32_t flags, std::uint32_t mode);

// UTL02：原型与实例各自拥有动态UTF-16正文，复制使用宽字符串赋值。
struct WindowTextPrototype {
    WindowTextStyle style; // 原成员偏移：+0x00。
    std::wstring text; // 原x86成员偏移：+0x04（文本指针）；UTL02拥有UTF-16原型正文，当前按原生对齐。
    WindowTextPrototype() = default;
    WindowTextPrototype(WindowTextStyle styleValue, std::wstring_view wide)
        : style(styleValue), text(wide) {}
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
    std::int32_t seqNo = -1; // WEO01：-1使用原型索引，其余值作为实例初始顺序。
    ElementId elementId = 0; // C++原型由对应窗口的enum显式指定固定ID。
};

// 原函数：0x0048F380。
std::int32_t WindowPetDetail_EditName(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 共享名称输入焦点。
// 原函数：0x0048F410。
std::int32_t WindowPetDetail_SwitchPage(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 三详情页切换。

// 两表已按原地址共享绑定；主表的其余元素与回调依赖见WindowPrototypeDependencies。
extern std::array<WindowPrototype*, kWindowCount> gWindowPrototypes57C130; // 原全局：0x0057C130。
const WindowPrototype& WindowRuntime_Prototype(session::ClientSession& session, std::size_t id); // 原型读者显式取会话；只有同源36/37是运行期可变对象。
WindowPrototype& WindowRuntime_DialogPrototype(session::ClientSession& session); // 业务只写所属会话的对话原型。
extern WindowPrototype& gWindowPrototype54E2A0; // 原全局：0x0054E2A0。 4C93A0直接比较的原对象，不追随表槽改指向。
extern WindowPrototype& gWindowPrototype54E240; // 原全局：0x0054E240。
extern WindowPrototype& gWindowPrototype54E330; // 原全局：0x0054E330。
std::array<WindowInstance*, kWindowCount>& WindowRuntime_Windows(session::ClientSession& session);
std::array<WindowState, kWindowCount>& WindowRuntime_States(session::ClientSession& session);
WindowLayoutSettings& WindowRuntime_Settings(session::ClientSession& session);
extern std::uint8_t gOwnStallOpenForBusiness_D67BE0; // 原全局：0x00D67BE0。 原多个UI生产/消费入口共用，不并入窗口记录。
extern const std::array<std::array<std::int32_t, 4>, 4> gPetBarImages57E434; // 原全局：0x0057E434。 left/middle/right/single。
extern const std::array<std::array<std::int32_t, 3>, 14> gNumericKeyImages54F030; // 原全局：0x0054F030。
extern const std::array<const wchar_t*, 20> gTribeNames57CB54; // 原全局：0x0057CB54。 后八项共用585C50原字节串。

// 原函数：0x004E54E0。
void WindowTrade_CopyOwnItem(session::ClientSession& session, std::int32_t side, std::int32_t index); // 背包index+8到交易，不复制描述。
// 原函数：0x004D3020。
std::int32_t WindowProfile_SaveOrigin(session::ClientSession& session, std::int32_t x, std::int32_t y); // 同源坐标依序保存，返回x。
// 原函数：0x004D3040。
std::int32_t WindowProfile_CheckDistance(session::ClientSession& session, std::int32_t rangeX, std::int32_t rangeY); // 到格才判断距快照的范围。
// 原函数：0x004CB9B0。
WindowInstance* WindowProfileView_RequestCategoryClose(session::ClientSession& session, std::int32_t index); // 借用窗口59+index，只标记关闭并返回原T*。
// 原函数：0x00478330。
std::int32_t WindowProfile_HandleButtons(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 资料公开/切窗/三目录/确认的完整八分支。
// 原函数：0x004CB430。
std::int32_t WindowProfileView_ReadSelection(session::ClientSession& session, std::int32_t index); // 只读selected10C门禁，不检查used。
// 原函数：0x0048BB30。
std::int32_t WindowItem_ClassifyItemSlot(std::int32_t slot); // IDE01：signed槽0..7/8..27/100..119分类1/2/3，其它0，真实S32，四CALL只来源/全消费待。
// 原函数：0x0048BB70。
std::int32_t WindowItem_ClassifyPetSlot(std::int32_t slot); // 宠物槽分类1/2。
// 原函数：0x0048BC60。
std::int32_t WindowItem_FindSortedPetIndex(session::ClientSession& session, std::int32_t slot); // DE0B08五个S16索引的首匹配位置。
// 原函数：0x0047D490。
void WindowCraft_ResetItemSelection(session::ClientSession& session); // 仅清D67B00，不改变其他物品状态。
// 原函数：0x004886E0。
void WindowItem_RequestTransfer(
    session::ClientSession& session,
    std::int32_t sourceWindow, std::int32_t sourceElement,
    std::int32_t source, std::int32_t target); // IMT01：完整直接/拆分门槛及两全局不同发布序，原数量callback副作用保持；全数量框/运行待。
// 原函数：0x0048A830。
WindowInstance* WindowQuantity_Open(
    session::ClientSession& session,
    std::int32_t sourceWindow, std::int32_t sourceElement, std::int32_t imageNo,
    std::int32_t maximum, std::int32_t value14, std::int32_t initialValue,
    WindowQuantityCallback callback); // QIP01：窗口42按原两次持址发布；callback原生非拥有，返回先取样。
// 原函数：0x00474170。
std::int32_t WindowElement_HandleSideArea(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // WCL01：bit2置bit13，hover优先，否则低BYTE bit5。
// 原函数：0x0048BB90。
void WindowItem_RequestPetTransfer(
    session::ClientSession& session,
    std::int32_t unusedWindow, std::int32_t unusedElement,
    std::int32_t source, std::int32_t target); // 普通/窗口41两路宠物换位请求。
// 原函数：0x004AAB70。
void WindowBank_DrawOwnPetTooltip(
    session::ClientSession& session,
    std::int16_t x, std::int16_t y, std::int32_t index, std::uint8_t textLayer);
// 原函数：0x004AB400。
void WindowBank_DrawStoredPetTooltip(
    session::ClientSession& session,
    std::int16_t x, std::int16_t y, std::int32_t index, std::uint8_t textLayer);
// 原函数：0x004EA670。
std::int32_t WindowPet_HasSkill25600(session::ClientSession& session, std::int32_t index); // 原25600技能检查，返回0/1。
// 原函数：0x0048C130。
std::int32_t WindowPetDrag_Start(session::ClientSession& session, std::int32_t sourceIndex, std::int32_t mode);
// 原函数：0x0048BF10。
std::int32_t WindowItemDrag_Start(session::ClientSession& session, std::int32_t sourceIndex, std::int32_t mode); // IDE01：mode先发布/真实kind2入口及S32透传，十三CALL只来源，完整回调/owner及运行待。
// 原函数：0x00474550。
std::uint8_t WindowInput_HandleNumber(
    WindowInstance* window, std::int32_t index, std::int32_t state,
    std::int32_t maximum, std::uint8_t layer, const wchar_t* const* hints,
    std::uint8_t flags, std::uint32_t color); // NPI01：数值键/Backspace/四鼠标操作原序，AL返回0/1/3；非Enter/Esc处理器。
// 原函数：0x004C1FB0。
std::int32_t WindowRuntime_ButtonClose(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // WCL01：持同窗口/graphic置关闭位，返回0/1，未在本回调释放。
// 原函数：0x004FBA30。
std::int32_t WindowItem_SplitTooltipLines(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::uint8_t renderMode,
    std::int32_t fontType, std::int8_t color, std::wstring_view text,
    std::int8_t value7, std::int8_t marker); // 原$分段、颜色续接与即时测宽。
// 原函数：0x0048B810。
// NIO01：原0x0048B810，运行时物品描述直接借宽行。
std::int32_t WindowItem_DrawTooltip(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, const wchar_t* title, std::int8_t titleColor,
    const std::wstring* lines, std::int32_t lineCount, const wchar_t* footer, std::int8_t footerColor);
// 原函数：0x0048B1A0。
std::int32_t WindowItem_DrawEquipmentTooltip(session::ClientSession& session, std::int32_t index, std::int32_t page); // 玩家/所选宠物装备提示。
// 原函数：0x0048B350。
std::int32_t WindowItem_DrawDescriptionTooltip(session::ClientSession& session, std::int32_t index, std::int32_t page); // SPD01：W32标题字段/页*768八行描述，四次CryptInt及真实提示返回保持。
// 原函数：0x0048B750。
std::int32_t WindowItem_DrawWorkTooltip(session::ClientSession& session, std::int32_t page); // CC3210独立详情提示，缓存数值键400。
// 原函数：0x004AC200。
std::int32_t WindowBank_ButtonCloseIcon(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口38右上关闭。
// 原函数：0x004A9350。
std::int32_t WindowBank_ButtonClose(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 窗口38/41共用关闭按钮。
// 原函数：0x004B4140。
std::int32_t WindowTrade_DrawRemotePetTooltip(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::int32_t index); // 外部宠物提示，重量行固定Y=31。
// 原函数：0x004B3BF0。
std::int32_t WindowTrade_DrawOwnPetTooltip(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::int32_t index,
    bool showEquipmentPage = false); // 完整己方宠物提示；银行／交易开启第三页装备名称。
// 原函数：0x004E5620。
cg::yobi::YobiAction* WindowTrade_ReceiveStart(session::ClientSession& session, std::wstring_view name); // 全量清交易双侧后复制己方，最后打开43。
// 原函数：0x0049E800。
cg::yobi::YobiAction* WindowTradeTarget_ReceiveTargets(session::ClientSession& session, const char* text, WindowListSelectionCallback callback);
// 原函数：0x00485110。
void WindowElement_DrawHoverFrame(WindowElement* element, std::int32_t state, std::uint32_t imageNo); // 元素外扩一像素悬停框。
// 原函数：0x00474270。
std::int32_t WindowScroll_ReadPosition(WindowElement* element, std::int32_t maximum); // WSP01：原整数位置换算及两高度取样，按控件类型读取滚动字段。
// 原函数：0x004742C0。
std::int32_t WindowScroll_SetPosition(WindowElement* element, std::int32_t maximum, std::int32_t position);
// 原函数：0x004743B0。
std::int32_t WindowScroll_ApplyWheel(
    WindowElement* element, std::int32_t position, std::int32_t maximum,
    std::int32_t wheel); // WWS01：原滚轮像素余量、坐标持样/字节载荷，宿主数学图保持。
// 原函数：0x004E0100。
void WindowTrade_CopyDisplayItems(session::ClientSession& session, std::int32_t side, std::int32_t index); // 另一展示副本，逐行复制描述。
// 原函数：0x004A84A0。
std::int32_t WindowBank_ReceiveStorageData(session::ClientSession& session, const char* text); // SGM01：完整A/G/I/P数据生产，固定A/增量I-P字段续行与fresh金额组保持，只有A返回1。
// 原函数：0x004A83A0。
WindowInstance* WindowBank_Open(
    session::ClientSession& session,
    std::int32_t kind, std::int32_t value14, std::int32_t userState);
// 原函数：0x004AC0C0。
WindowInstance* WindowItemStorage_Open(
    session::ClientSession& session,
    std::int32_t kind, std::int32_t value14, std::int32_t userState,
    std::int32_t unusedPhase); // 原38窗口发布，两个caller均保留第四实参。
// 原函数：0x004FB8F0。
std::int32_t WindowItem_ResetItemDrag(session::ClientSession& session); // 物品拖动重置，不等同窗口拖动。
// 原函数：0x004FB920。
std::int32_t WindowItem_CancelMatchingItemDrag(session::ClientSession& session, std::int32_t index); // 仅清与本次回复槽号相同的非负拖动编号。
// 原函数：0x004E5DA0。
void WindowTrade_SwapRemoteItems(session::ClientSession& session, std::int32_t side, std::int32_t first, std::int32_t second); // 交易整条换位。
// 原函数：0x004E1840。
void WindowTrade_SwapDisplayItems(session::ClientSession& session, std::int32_t side, std::int32_t first, std::int32_t second); // 展示整条换位。
// 原函数：0x0048BA60。
void WindowItemDrag_RemapHover(session::ClientSession& session, std::int32_t first, std::int32_t second); // UI迁移后再交换同源悬停字节。
// 原函数：0x004B0F10。
std::int32_t WindowElement_NoOp(WindowInstance& callbackWindow, ElementId elementId, std::int32_t events); // WSB01：原完整零返回事件回调。
// 原函数：0x004A4210。
std::int32_t WindowRuntime_ReturnOne(std::int32_t unused) noexcept; // WCL01：原mov eax,1/ret；release适配不执行资源回收，存档共用。
std::int32_t WindowRuntime_UpdateDefault(WindowInstance& window, std::int32_t unused) noexcept; // 原恒1更新的实例回调适配。
// 原函数：0x004B0000。
void WindowTrade_ResetSelection(session::ClientSession& session, std::int32_t first, std::int32_t second); // 窗口43的20个槽号及28B标记。
// 原函数：0x0047DCD0。
void WindowCraft_ResetSelection(session::ClientSession& session, std::int32_t first, std::int32_t second); // 窗口11选择/材料及两组标记。
// 原函数：0x004AD060。
void WindowTrade_RemapTextListSelection(session::ClientSession& session, std::int32_t first, std::int32_t second); // 窗口51仅first命中时迁移。
// 原函数：0x004AD180。
cg::yobi::YobiAction* WindowTextList_Open(
    session::ClientSession& session,
    std::int32_t request, std::int32_t target, const char* text); // 解析标题/正文并展开51。
// 原函数：0x004AD470。
void WindowTextList_ContinueDialog(session::ClientSession& session, const char* text); // 同类对话续接，唯一caller不消费原混合返回。
// 原函数：0x004797C0。
void WindowSkill_AdjustSpecialHoverCount(session::ClientSession& session, std::int32_t first, std::int32_t second); // 槽7特殊悬停计数，两条件独立。
// 原函数：0x004F4630。
std::int32_t WindowCraft_CountRecipeMaterial(session::ClientSession& session, std::int32_t itemId); // 当前配方五材料首匹配数量，不读used。

// 原输入原型四模式更新及聊天恢复，复用既有InputContext owner。
// WIC01：原型四模式、现场context与BYTE结果合同，见doc/cg2d_window_input_context_dispatch_spec.md。
// 原函数：0x00476570。
std::uint8_t WindowInput_ActivateElement(
    session::ClientSession& session,
    WindowInputPrototype* prototype, std::int32_t x, std::int32_t y,
    std::uint32_t mode);
// 原函数：0x004A7B00。
std::int32_t WindowChat_RestoreInput(session::ClientSession& session);
// 原函数：0x00477490。
WindowInstance* WindowRuntime_FindById(session::ClientSession& session, std::int32_t id); // 顺序扫描所属会话原生显示槽；原循环不跳过空槽。
// CIG01：IME测量后先字体宽度再聊天单位取样；见doc/cg2d_input_scroll_draw_spec.md。
// 原函数：0x004A7590。
void WindowChat_DrawInput(session::ClientSession& session, input::InputContext* context); // 按聊天宽度滚动目标输入并绘制。
// CIG01：固定1024宽度及滚动绘制正文保持；见doc/cg2d_input_scroll_draw_spec.md。
// 原函数：0x004A7640。
void WindowChat_DrawFixedWidthInput(session::ClientSession& session, input::InputContext* context); // 按固定1024像素滚动目标输入并绘制。
// CLD01：日志分行/字体回写与尺寸合同；见doc/cg2d_chat_log_line_drawing_spec.md。
// 原函数：0x004A7750。
void WindowChat_DrawHistory(session::ClientSession& session, std::int32_t x, std::int32_t y); // 原字节分行、底部逆序绘制及前项字体回写。
// 原函数：0x0046FBC0。
std::int32_t WindowRuntime_DrawDefault(WindowInstance& callbackWindow, std::int32_t unused); // 原默认draw回调，完整绘制后返回1。
// 原函数：0x0049BDB0。
std::int32_t WindowRuntime_DrawCompactFrame(WindowInstance& callbackWindow, std::int32_t unused); // 完整元素绘制后叠加九宫格背景。
// 原函数：0x004741A0。
std::int32_t WindowElement_HandleVerticalScroll(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // WSB01：低BYTE/W32标志与前项垂直滚动，保原边界取样序。
// DLP01：只清焦点元素ID/kind/焦点窗口/回调/phase五槽，来源及两坐标保持，返S32 0。
// 原函数：0x004763E0。
std::int32_t WindowDrag_ResetMouse(session::ClientSession& session); // 通用鼠标重置，不清来源索引/坐标。
// DLP01/MEI01：已有kind才绑定焦点窗口与元素ID，不改变会话阶段，返S32 0/1。
// 原函数：0x004763B0。
std::int32_t WindowDrag_SetFocus(session::ClientSession& session, std::int32_t windowId, ElementId elementId); // 已有kind才写焦点窗口与元素ID。
// 原函数：0x00475D50。
ElementId WindowDrag_ReadFocusElement(session::ClientSession& session); // 焦点元素ID，0表示无焦点元素。
// 原函数：0x00475D40。
std::int32_t WindowDrag_ReadFocusWindowId(session::ClientSession& session); // 同源焦点窗口，不是当前窗口。
// 原函数：0x00475D60。
std::int32_t WindowDrag_ReadKind(session::ClientSession& session); // 同源拖动种类。
// 原函数：0x00475D70。
std::int32_t WindowDrag_ReadX(session::ClientSession& session); // 同源落点X，S16扩展。
// 原函数：0x00475D80。
std::int32_t WindowDrag_ReadY(session::ClientSession& session); // 同源落点Y，S16扩展。
// 原函数：0x00475D90。
std::int32_t WindowDrag_ReadSourceIndex(session::ClientSession& session); // 同源来源索引。
// DLP01：仅phase0且kind非零启动阶段1，按原焦点/回调/来源写序发布，返S32 0/1。
// 原函数：0x00475DA0。
std::int32_t WindowDrag_Start(
    session::ClientSession& session,
    std::int32_t kind, std::int32_t sourceIndex, WindowDragCallback callback);
// DLP01：仅phase0且kind非零启动阶段3，坐标保S16，原焦点与回调写序保持，返S32 0/1。
// 原函数：0x00476490。
std::int32_t WindowDrag_StartAtPosition(
    session::ClientSession& session,
    std::int32_t kind, std::int32_t sourceIndex, std::int16_t x, std::int16_t y,
    WindowDragCallback callback);
// DLP01：入口phase4/5才可选回调message3，之后按原八槽顺序清理；不重验回调后phase。
// 原函数：0x00476500。
void WindowDrag_Reset(session::ClientSession& session); // 仅阶段4/5调用message3后完整清理。
// 原函数：0x00503CB0。
std::int32_t WindowRuntime_CycleNameMode(); // 73ECBC先减后负值归2，返回归位前结果。
// 原函数：0x004A6D80。
std::int32_t WindowChat_ResetState(session::ClientSession& session); // 聊天初始范围、回看、数量、调整请求和颜色。
// 原函数：0x004C9130。
std::int32_t WindowRuntime_SaveSettings(session::ClientSession& session); // 打开成功才捕获并保存；原打开失败同样返回1。
// 原函数：0x004A53D0。
WindowInstance* WindowMailCompose_Open(session::ClientSession& session); // 先写D2F0B8.+26C，再真实打开22，返回借用窗口。
// 原函数：0x004705D0。
std::int32_t WindowRuntime_Initialize(session::ClientSession& session); // 启动UI完整初始化，返回category加载结果。
// 原函数：0x004B5A90。
void WindowMessageBoard_ResetRecords(session::ClientSession& session); // 四条记录只清已证字段和四个文本首字节。
// 原函数：0x004B6970。
std::int32_t WindowMessageBoard_ReceiveRecords(session::ClientSession& session, std::int32_t flags, char* text); // 原位反转义后生产同源四记录。
// 原函数：0x004A5390。
std::int32_t WindowMail_ResetComposeState(session::ClientSession& session); // 五组选择槽及D2F0B8已证前缀重置。
// 原函数：0x0048C150。
std::int32_t WindowPetList_ResetSelection(session::ClientSession& session); // 所选宠物与两组五槽按原序重置。
// 原函数：0x004A0940。
std::int32_t WindowMail_ResetSelectionState(session::ClientSession& session); // D67B40七槽交错清零。
// 原函数：0x004A3600。
std::int32_t WindowMail_DeleteContact(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState); // FPkR发送、重读清理及保存。
// 原函数：0x004A3660。
std::int32_t WindowMail_DeleteGuildMember(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState); // 公会编号请求与两保存。
// 原函数：0x004A36B0。
std::int32_t WindowMail_DeleteMiniMailContact(WindowInstance& callbackWindow, ElementId elementId, std::int32_t unusedState); // 迷你姓名转义、发送及重读清理。
// 原函数：0x004A5650。
void WindowMailCompose_Release(WindowInstance&); // 22/23共享释放：保存草稿、释放占用、关闭49。
// 原函数：0x004A58E0。
std::int32_t WindowMailCompose_HandleInput(WindowInstance& callbackWindow, ElementId elementId, std::int32_t state); // 输入聚焦及非当前输入补绘。
// 原函数：0x0047D4A0。
std::int32_t WindowCraft_ReadItemSelection(session::ClientSession& session); // 原S32 D67B00窄读，不返回其他状态。
// 原函数：0x004A32B0。
std::int32_t WindowMail_AdvanceUnreadBlink(session::ClientSession& session); // 邮件闪烁相位推进；开窗不重置。
// 原函数：0x004A3010。
std::int32_t WindowContacts_CompareContactName(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通姓名原字节比较。
// 原函数：0x004A3070。
std::int32_t WindowContacts_CompareGuildName(
    const cg::world::OrderedRecord1051B14* left,
    const cg::world::OrderedRecord1051B14* right); // 公会姓名原字节比较。
// 原函数：0x004A30D0。
std::int32_t WindowContacts_CompareGuildValue(
    const cg::world::OrderedRecord1051B14* left,
    const cg::world::OrderedRecord1051B14* right); // 公会value00完整S32差值。
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
    const cg::world::OrderedRecord1051B14* left,
    const cg::world::OrderedRecord1051B14* right); // 公会value10完整S32差值。
// 原函数：0x004A31C0。
std::int32_t WindowContacts_CompareRecordKey(
    const std::int32_t* leftValue04, const std::int32_t* rightValue04); // 借原记录+4字段，相等仍-1。
// 原函数：0x004A31E0。
std::int32_t WindowContacts_CompareContactNumber(
    const world::detail::ContactRuntimeRecord* left,
    const world::detail::ContactRuntimeRecord* right); // 普通S16 value02，零映射最大S32。
// 原函数：0x004A3260。
std::int32_t WindowContacts_CompareGuildNumber(
    const cg::world::OrderedRecord1051B14* left,
    const cg::world::OrderedRecord1051B14* right); // 公会S16 value0A，零映射最大S32。
// 原函数：0x004A3950。
std::int32_t WindowGuildDetails_ReadRankPermissions(session::ClientSession& session); // 当前公会职级权限80H。
// 原函数：0x004A3960。
std::int32_t WindowGuildDetails_CanEdit(session::ClientSession& session, std::int32_t mask); // 当前职级-1才拒绝，再读同源权限。
// 原函数：0x004A3990。
std::int32_t WindowGuildDetails_CanEditRank(session::ClientSession& session, std::int32_t index, std::int32_t mask); // 当前职级门禁通过后读取指定职级。
// 原函数：0x004CC6A0。
std::int32_t WindowBlacklist_ResetState(session::ClientSession& session); // 三个UI数值依序写5/0/0。
// 原函数：0x004CBD30。
void WindowAdventureLog_InitializeDate(session::ClientSession& session); // 首次借用会话日历并写同源日志文件名。
// 原函数：0x004A8330。
WindowInstance* WindowChat_SwitchMode(session::ClientSession& session); // 标旧聊天窗关闭、开另一窗后发布模式，透传新窗T*。
// 原函数：0x004A6DB0。
std::int32_t WindowChat_ResetScrollRequests(session::ClientSession& session); // 仅重置回看、两调整请求与数量。
// 原函数：0x0044B170。
std::int32_t WindowChat_ClearHistory(session::ClientSession& session); // 清512条日志首字节，再清数量与回看。
// 原函数：0x004A7710。
void WindowChat_RequestHeightIncrease(session::ClientSession& session); // 请求高度+1。
// 原函数：0x004A7720。
void WindowChat_RequestHeightDecrease(session::ClientSession& session); // 请求高度-1。
// 原函数：0x004A7730。
void WindowChat_RequestWidthIncrease(session::ClientSession& session); // 请求宽度+1。
// 原函数：0x004A7740。
void WindowChat_RequestWidthDecrease(session::ClientSession& session); // 请求宽度-1。
// WND01：数字与四符号原字节正反绘制，见doc/cg2d_window_number_draw_spec.md。
// 原函数：0x004771B0。
void WindowNumber_Draw(
    std::int32_t x, std::int32_t y, std::wstring_view text,
    std::int8_t style, std::int8_t color, std::uint8_t flags, std::uint8_t layer);
// WND01：运行步进乘长度后取扣宽，见doc/cg2d_window_number_draw_spec.md。
// 原函数：0x00477460。
std::int32_t WindowNumber_MeasureWidth(std::wstring_view text, std::int8_t style); // 原数字字符数乘运行步进减扣宽。

static_assert(sizeof(WindowElementPrototypeSource) == 8);
static_assert(offsetof(WindowElementPrototype, source) == 0x18);
static_assert(offsetof(WindowElementPrototype, mouseCallback) == 0x20);
static_assert(offsetof(WindowElementPrototype, seqNo) == 0x28);
static_assert(sizeof(WindowElementPrototype) == 0x30);
static_assert(offsetof(WindowGraphicData, offsetX) == 4);
static_assert(offsetof(WindowGraphicData, offsetY) == 8);
static_assert(offsetof(WindowGraphicData, width) == 0x0C);
static_assert(offsetof(WindowGraphicData, height) == 0x10);
static_assert(offsetof(WindowGraphicData, color) == 0x14);
static_assert(offsetof(WindowGraphicData, imageHover) == 0x18);
static_assert(offsetof(WindowGraphicData, imagePress) == 0x1C);
static_assert(offsetof(WindowGraphicData, imageNormal) == 0x20);
static_assert(sizeof(WindowGraphicData) == 0x24);
static_assert(offsetof(WindowInputData, ownedInput) == 8);
static_assert(sizeof(WindowInputData) == 16);
static_assert(offsetof(WindowNumberData, color) == 8);
static_assert(offsetof(WindowNumberData, style) == 9);
static_assert(offsetof(WindowNumberData, flags) == 0x0A);
static_assert(offsetof(WindowNumberData, byte07) == 0x0B);
static_assert(sizeof(WindowNumberData) == 16);
static_assert(offsetof(WindowAnimationData, action) == 0);
static_assert(offsetof(WindowAnimationData, value04) == 8);
static_assert(offsetof(WindowAnimationData, options) == 16);
static_assert(sizeof(WindowAnimationData) == 24);
static_assert(offsetof(WindowMouseData, deltaX) == 4);
static_assert(offsetof(WindowMouseData, deltaY) == 8);
static_assert(sizeof(WindowMouseData) == 12);
static_assert(offsetof(WindowElement, data) == 0x20);
static_assert(offsetof(WindowElement, mouseCallback) == 0x28);
static_assert(sizeof(WindowElementData) == sizeof(void*));
static_assert(offsetof(WindowElement, seqNo) == 0x30);
static_assert(offsetof(WindowDragRect, y) == 4);
static_assert(offsetof(WindowDragRect, width) == 8);
static_assert(offsetof(WindowDragRect, height) == 12);
static_assert(sizeof(WindowDragRect) == 16);
static_assert(offsetof(WindowState, window) == 0x10);
static_assert(offsetof(WindowState, userState) == 0x20);
static_assert(offsetof(WindowState, actionFlags1C) == 0x28);
static_assert(offsetof(WindowState, marker50) == 0x5C);
static_assert(sizeof(WindowState) == 0x60);

} // namespace detail
} // namespace cg::ui
