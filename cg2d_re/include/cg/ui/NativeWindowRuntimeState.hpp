#pragma once

#include "cg/ui/WindowRuntime.hpp"
#include "cg/world/PlayerSlots.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace cg::ui::detail {

// 原CC4230接收技能、等级两个S32；宿主回调另携发起会话，不改变命令字段。
using SkillWindowSelectionCallback = void (*)(
    session::ClientSession& session, std::int32_t skillIndex, std::int32_t technique);
using PetSkillSelectionCallback = void (*)(
    session::ClientSession& session, std::int32_t technique);
// 选择结果非零允许关闭；文本借用，返回/释放通知可为nullptr。
using WindowTextChoiceCallback = std::int32_t (*)(
    session::ClientSession& session, std::int32_t index, const wchar_t* text);

// 原窗口46的28B属性源；仅名称地址扩宽，技能/物品目标分别生产。
struct WindowStatusChoiceRecord {
    const wchar_t* name; // 原成员偏移：+0x00。
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

// 原CB10D8的六行物品目标属性；不与技能ND属性槽混用。
struct ItemUseStatusRecord {
    std::wstring name;
    std::int32_t level; // 原+14。
    std::int32_t injury; // 原+18。
    std::int32_t hp; // 原+1C。
    std::int32_t maxHp;
    std::int32_t fp;
    std::int32_t maxFp; // 原+28。
    std::int32_t kind; // 原+2C。
    std::int32_t target; // 原+30。
};

// 原CB0EA8十条物品目标名单，名称由记录实际持有。
struct ItemUseTargetRecord {
    std::int16_t kind;
    std::wstring name; // 原+02。
    std::byte byte13; // 原+13。
};

union WindowUserState {
    std::uint32_t value; // 原+18的数值路径。
    WindowQuantityCallback callback; // 原窗口42的+18，禁止截成W32；回送显式携会话。
};

struct WindowState {
    std::uint32_t flags; // 原+00，独立于实例state。
    std::int32_t x; // 原+04。
    std::int32_t y; // 原+08。
    WindowInstance* window; // 原+0C；不沿用原记录0x54步长。
    std::uint32_t value10;
    std::uint32_t value14;
    WindowUserState userState; // 原+18；由生产者按实际用途激活成员。
    std::uint32_t actionFlags1C; // 窗口42数值面板消费低U8。
    std::uint32_t value20;
    std::uint32_t value24;
    std::uint32_t value28;
    std::array<std::byte, 36> bytes2C; // 未消费区，不据此冻结未知指针映射。
    std::uint32_t marker50;
};

inline constexpr std::size_t kSavedWindowCount = 71; // WinSaveDate格式固定，新增窗口不扩磁盘记录。
inline constexpr std::size_t kWindowCount = 73; // 原71槽加大地图、宠物装备原型。

// 原D40B30的521CH磁盘格式，不与含原生指针的WindowState布局混用。
struct WindowSavedRecord {
    std::int32_t x;
    std::int32_t y;
    std::int32_t value08;
    std::int32_t value0C;
    std::int32_t value10;
    std::array<std::byte, 0x114> bytes14;
};

struct WindowSavedData {
    std::int32_t version;
    std::array<WindowSavedRecord, kSavedWindowCount> records;
};

} // namespace cg::ui::detail

namespace cg::ui {

// UI布局及选择偏好；装载初值与加载文件后的默认值分开保留。
struct WindowLayoutSettings {
    std::int32_t chatMode = 0; // 原0xD66E04。
    std::int32_t chatWidthUnits = 0; // 原0xC9A930。
    std::int32_t chatHeightUnits = 0; // 原0xC9A938。
    std::uint8_t inventoryActorSelection = 0xFF; // 原0x57DA28。
    std::uint8_t battleItemActorSelection = 0xFF; // 原0x57E42C。
    std::int32_t hotkeyLayoutMode = 0; // 原0xCF4AAC。
    std::int32_t statusBarMode = 0; // 原0xCF4690。
    std::int32_t shortcutBarCollapsed = 0; // 原0xCA0440。
    std::int32_t contactsPreviewMode = 0; // 原0xD67B38。
    std::uint32_t actorBarsEnabled = 0; // 原0x5AC490，地图角色绘制与快捷键共用。
};

// 战斗命令窗口借用的四类开窗动作及目标选择门禁。
struct BattleCommandWindowState {
    yobi::YobiAction* skillOpening = nullptr; // 原0xCC1A10。
    yobi::YobiAction* itemOpening = nullptr; // 原0xD2F790。
    yobi::YobiAction* petOpening = nullptr; // 原0xD2D60C。
    yobi::YobiAction* petSkillOpening = nullptr; // 原0xCC423C，独立于前三槽清理。
    std::uint8_t skillTargetPending = 0; // 原0xD303F8。
    std::uint8_t targetSelectionReady = 0; // 原0xCB054C。
};

// 技能窗的页状态和选择回送；目标名单与制作业务另沿各自链迁移。
struct SkillWindowRuntimeState {
    detail::SkillWindowSelectionCallback selectionCallback = nullptr; // 原0xCC4230。
    detail::PetSkillSelectionCallback petSelectionCallback = nullptr; // 原0xCC4D14。
    std::int32_t mode = 0; // 原0xD672AC。
    std::int32_t index = 0; // 原0xD2EA60。
    std::int32_t scroll = 0; // 原0xD2D648。
    std::int32_t hover = 0; // 原0xD2F744。
    std::int32_t recipeLevel = 0; // 原0xC8B1B4。
    std::int32_t maximum = 0; // 原0xCA000C。
    // 原前15项为配对行号；现有30行仅统计有效槽，保留原S8存储。
    std::array<std::int8_t, world::kSkillSlotCount> battleRows {}; // 原0xD3FABC。
};

// 窗口45借用该会话生产的名称/颜色表，待安装和当前回调保持独立。
struct TextTargetWindowState {
    const wchar_t* const* names = nullptr; // 原0xD30130。
    const std::uint8_t* colors = nullptr; // 原0xCB1E18。
    std::int32_t count = 0; // 原0xCC4CE4。
    detail::WindowTextChoiceCallback pending = nullptr; // 原0xC9A974。
    detail::WindowTextChoiceCallback callback = nullptr; // 原0xCA0AC0。
    std::int32_t offset = 0; // 原0xD2D9C0。
    std::uint8_t enabled = 0; // 原0xCA0008。
};

// 窗口46借用该会话的技能或物品属性视图，不拥有名称文本。
struct AttributeTargetWindowState {
    const detail::WindowStatusChoiceRecord* records = nullptr; // 原0xCB03D0。
    std::int32_t count = 0; // 原0xCC2364。
    detail::WindowTextChoiceCallback callback = nullptr; // 原0xCF47D4。
    std::int32_t offset = 0; // 原0xD671D4。
    std::uint8_t enabled = 0; // 原0xC9A970。
};

// 技能目标名称与ND属性值的实际owner；视图仅借同一会话内的稳定数组。
struct SkillTargetRuntimeState {
    std::int32_t count = 0; // 原0xCA0004。
    std::int32_t page = 0; // 原0xC8A974。
    std::int32_t pageCount = 0; // 原0xCA0178。
    std::int32_t selected = 0; // 原0xCC4B1C。
    std::array<std::wstring, 10> names {}; // 原0xCB0550，非物品LY名单。
    std::array<std::int32_t, 10> kinds {}; // 原0xD2F75C。
    std::array<const wchar_t*, 10> nameViews {}; // 原0xD45F98。
    std::array<std::uint8_t, 10> colors {}; // 原0xCB1DE4。
    std::uint8_t pending = 0; // 原0xC8AF5F。
    std::int32_t attributeCount = 0; // 原0xC8AA94。
    std::array<std::wstring, 6> attributeNames {}; // 原0xCC4670。
    std::array<std::int32_t, 6> levels {}; // 原0xCC31C0。
    std::array<std::int32_t, 6> injuries {}; // 原0xD67600。
    std::array<std::int32_t, 6> hp {}; // 原0xCC318C。
    std::array<std::int32_t, 6> maxHp {}; // 原0xD66C1C。
    std::array<std::int32_t, 6> fp {}; // 原0xCC2348。
    std::array<std::int32_t, 6> maxFp {}; // 原0xD2EA80。
    std::array<std::int32_t, 6> attributeKinds {}; // 原0xCA0A78。
    std::array<std::int32_t, 6> targetIndices {}; // 原0xCF3FD0。
    std::array<detail::WindowStatusChoiceRecord, 6> attributeViews {}; // 原0xCC4BE8。
};

// 已提交技能请求，与技能窗口当前浏览页/所选槽分离。
struct SkillRequestRuntimeState {
    std::int32_t index = 0; // 原0xD2F32C。
    std::int32_t level = 0; // 原0xCA0AC4。
    std::int32_t targetKind = 0; // 原0xCF4A10。
    std::wstring targetName {}; // 原0xD2F748。
};

// JgO第三分支由连接生产，窗口13及其文本只借同一会话的数据。
struct SkillResultRuntimeState {
    std::int32_t result = 0; // 原0xD66E2C。
    std::int32_t experience = 0; // 原0xCC2E5C。
    std::int32_t value = 0; // 原0xCC3E8C。
    std::int32_t levelUp = 0; // 原0xCF468C。
    std::int32_t endurance = 0; // 原0xCC421C。
    std::int32_t dexterity = 0; // 原0xD2D644。
    std::int32_t intelligence = 0; // 原0xCC19FC。
    std::int32_t extra = 0; // 原0xD2DBF0。
    std::wstring summary {}; // 原0xCC18CC。
    std::wstring title {}; // 原0xCC18EC。
    std::wstring details {}; // 原0xCB0FDC。
};

// 窗口12的采集请求、阶段与结果；W8请求索引仍按原S8路径消费。
struct GatherWindowRuntimeState {
    std::uint8_t skillIndex = 0; // 原0xCA0470。
    std::uint8_t skillLevel = 0; // 原0xCB0608。
    std::uint8_t kind = 0; // 原0xD046CC。
    std::uint8_t mode = 0; // 原0xCC3E80。
    std::uint8_t flag = 0; // 原0xCC4D27。
    std::uint64_t tick = 0; // 原0xD45D4C。
    std::uint64_t deadline = 0; // 原0xCB0F7C。
    std::int32_t result = 0; // 原0xD2D820。
    std::int32_t experience = 0; // 原0xD2DFCC。
    std::int32_t value = 0; // 原0xD40B08。
    std::int32_t levelUp = 0; // 原0xC9C0FC。
    std::int32_t endurance = 0; // 原0xCB0358。
    std::int32_t dexterity = 0; // 原0xCC3EC0。
    std::int32_t intelligence = 0; // 原0xCC4B60。
    std::int32_t resultImage = 0; // 原0xCF3FF0。
    std::int32_t extra = 0; // 原0xD66E28。
    std::int32_t petImage = 0; // 原0xCB10CC。
    std::int32_t crystalLevel = 0; // 原0xCC3E90。
    std::wstring name {}; // 原0xD2DA94。
    std::wstring suffix {}; // 原0xCB1E1C。
    std::wstring details {}; // 原0xCC190C。
};

// 制作窗的选择、请求、阶段与两类结果；保留查询索引和实际请求索引的独立身份。
struct CraftWindowRuntimeState {
    std::uint8_t lookupSkillPair = 0; // 原0xD67AD8，按S8解释。
    std::int32_t lookupRecipe = -1; // 原0x5846BC。
    std::int32_t petAssistActive = 0; // 原0xD67B00，非地址。
    std::array<std::int32_t, 6> selectedSlots {}; // 原0xCA0A90。
    std::array<std::int32_t, 6> materialIndices {}; // 原0xCA0458。
    std::array<std::int32_t, 6> selectionCounts {}; // 原0xD66E0C。
    std::array<std::int32_t, 6> selectionImages {}; // 原0xCF3FF4。
    std::int32_t selectedItem = 0; // 原0xD460C0。
    std::int32_t selectedImage = 0; // 原0xCC3ED8。
    std::int32_t selectedCount = 0; // 原0xCC483C。
    std::uint8_t selectionMode = 0; // 原0xD2D650。
    std::array<std::uint8_t, 28> itemMarks {}; // 原0xD67290。
    std::array<std::uint8_t, 20> materialItemMarks {}; // 原0xD68A90，不扩展原越界访问。
    std::int32_t backpackHover = -2; // 原0x57DA24，独立于材料/单选悬停。
    std::int32_t recipeId = 0; // 原0xD2E440，配方重定位快照。
    std::uint64_t deadline = 0; // 原0xCF3B04，等待阶段同源64位截止值。
    std::int32_t result = 0; // 原0xCC46D8。
    std::int32_t experience = 0; // 原0xCC3204。
    std::int32_t value = 0; // 原0xCC4840。
    std::int32_t levelUp = 0; // 原0xCF4974。
    std::int32_t endurance = 0; // 原0xD2F328。
    std::int32_t dexterity = 0; // 原0xCA0448。
    std::int32_t intelligence = 0; // 原0xCB0F84。
    std::int32_t injury = 0; // 原0xD2EA78。
    std::int32_t resultImage = 0; // 原0xCF4B60。
    std::int32_t displayImage = 0; // 原0xCB2204。
    std::wstring name {}; // 原0xCF46D4。
    std::wstring resultSuffix {}; // 原0xD672B4。
    std::wstring resultDetails {}; // 原0xD2E92C，既有NUL文本合同，不猜容量。
    std::array<std::int32_t, 6> materialHoverSlots {}; // 原0xCB0A8C。
    std::int32_t materialHintPage = 0; // 原0xCF4FD8。
    std::int32_t materialHoveredItem = 0; // 原0xD67424。
    std::int32_t itemWorkResult = 0; // 原0xCB05FC。
    std::int32_t itemWorkExperience = 0; // 原0xC8A950。
    std::int32_t itemWorkValue = 0; // 原0xD2E2AC。
    std::int32_t itemWorkLevelUp = 0; // 原0xCB03D8。
    std::int32_t itemWorkEndurance = 0; // 原0xCF46B4。
    std::int32_t itemWorkDexterity = 0; // 原0xCB0A14。
    std::int32_t itemWorkIntelligence = 0; // 原0xD30418。
    std::int32_t itemWorkInjury = 0; // 原0xD2E2A8。
    std::int32_t itemWorkSkillId = 0; // 原0xD67CF0。
    std::wstring itemWorkSuffix {}; // 原0xD2DADC。
    std::wstring itemWorkDetails {}; // 原0xD2DB00，独立于制造详情。
    std::int32_t materialSingleHover = 0; // 原0xCB0AA4。
    std::int32_t skillIndex = 0; // 原0xCC2368。
    std::int32_t recipeIndex = 0; // 原0xD2D638。
    std::int32_t requestValue = 0; // 原0xD30400，制造发送完整S32。
    std::uint8_t requestByte = 0; // 原0xD64E13，其他加工按S8读取。
};

// 同一会话内制作、采集和技能选择共用，保留原计数宽度与两独立借用槽。
struct WindowOperationRuntimeState {
    std::int32_t count = 0; // 原0xD67AF8。
    std::uint8_t gatherCount = 0; // 原0xD67AFC。
    std::uint8_t skillCount = 0; // 原0xD67AFD。
    std::uint8_t statusIconReferences = 0; // 原0xD67AFE。
    yobi::YobiAction* craftOpening = nullptr; // 原0xCA0174，非拥有。
    yobi::YobiAction* gatherOpening = nullptr; // 原0xD2E928，非拥有。
};

// 原生物品/技能拖放现场；当前派发元素与焦点元素分开保存。
struct WindowPointerRuntimeState {
    std::int32_t kind = 0; // 原0x57C110。
    std::int32_t sourceIndex = 0; // 原0x57C114。
    WindowDragCallback callback = nullptr; // 原0x57C118，非拥有。
    std::int16_t x = 0; // 原0x57C11C。
    std::int16_t y = 0; // 原0x57C11E。
    std::int32_t focusWindowId = 0; // 原0x57C120。
    ElementId focusElementId = 0; // 原0x57C124，MEI01以0表示无焦点元素。
    std::int32_t phase = 0; // 原0x57C128。
    ElementId currentElementId = 0; // 原0x57C12C，MEI01以0表示无当前元素。
    std::int32_t currentWindowId = 0; // 原0xD67A1C。
};

// 拆分数量框回送时读取的物品来源/目标，保留原独立发布顺序。
struct ItemTransferRuntimeState {
    std::int32_t source = 0; // 原0xCF46B0。
    std::int32_t target = 0; // 原0xD2DBF8。
};

// 物品目标的LY名单与nFV属性值各自持有，不复制或借其他会话的字符串。
struct ItemTargetRuntimeState {
    std::array<detail::ItemUseTargetRecord, 10> records {}; // 原0xCB0EA8。
    std::int32_t count = 0; // 原0xCC40CC。
    std::int32_t page = 0; // 原0xCC46DC。
    std::int32_t pageCount = 0; // 原0xCC3E70。
    std::int32_t selected = 0; // 原0xCB0A48。
    std::array<const wchar_t*, 10> names {}; // 原0xCB0A1C。
    std::array<std::uint8_t, 10> colors {}; // 原0xD2E91C。
    std::array<detail::ItemUseStatusRecord, 6> attributes {}; // 原0xCB10D8。
    std::int32_t attributeCount = 0; // 原0xCC4D44。
    std::array<detail::WindowStatusChoiceRecord, 6> attributeViews {}; // 原0xCB1278。
    std::uint8_t references = 0; // 原0xD67B24，保留W8增减和S8门禁。
};

// 原生窗口实例地址稳定，状态记录与排序数组由同一账号会话持有。
struct NativeWindowRuntimeState {
    std::array<WindowInstance*, detail::kWindowCount> windows {}; // 原0xC8B094，非拥有排序引用。
    std::array<detail::WindowState, detail::kWindowCount> states {}; // 原0xC9A9B0，按ID寻址。
    WindowLayoutSettings settings {};
    BattleCommandWindowState battleCommand {};
    SkillWindowRuntimeState skill {};
    TextTargetWindowState textTarget {};
    AttributeTargetWindowState attributeTarget {};
    SkillTargetRuntimeState skillTargets {};
    SkillRequestRuntimeState skillRequest {};
    SkillResultRuntimeState skillResult {};
    GatherWindowRuntimeState gather {};
    CraftWindowRuntimeState craft {};
    WindowOperationRuntimeState operations {};
    WindowPointerRuntimeState pointer {};
    ItemTransferRuntimeState itemTransfer {};
    ItemTargetRuntimeState itemTargets {};
    detail::WindowSavedData savedData {}; // 原0xD40B30。
    std::int32_t savedLength = 0; // 原0xCC4238。
};

} // namespace cg::ui
