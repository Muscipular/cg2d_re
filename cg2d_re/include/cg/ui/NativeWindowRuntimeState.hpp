#pragma once

#include "cg/input/Input.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/world/PlayerSlots.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <string>
#include <type_traits>
#include <vector>

namespace cg::ui::detail {

struct WindowElementPrototype;

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
static_assert(offsetof(WindowPrototype, width) == 0x10);
static_assert(offsetof(WindowPrototype, height) == 0x14);
static_assert(offsetof(WindowPrototype, elements) == 0x20);
static_assert(offsetof(WindowPrototype, update) == 0x28);
static_assert(offsetof(WindowPrototype, draw) == 0x30);
static_assert(offsetof(WindowPrototype, release) == 0x38);
static_assert(sizeof(WindowPrototype) == 0x40);

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
static_assert(sizeof(WindowInputReference) == 16);

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

// 原CC4894材料区与CC48A8商店模式；窗口共享同一24B对象，整区清零包含尾部。
struct WindowMaterialState {
    std::array<std::int32_t, 5> materialValues; // 原成员偏移：+0x00；材料占用值，打包时存材料等级。
    char shopMode; // 原成员偏移：+0x14；原全局：0x00CC48A8。 商店模式，保留原S8解释。
    std::array<std::byte, 3> reserved15; // 原成员偏移：+0x15；未确认用途，随整区清零。
};
static_assert(std::is_standard_layout_v<WindowMaterialState>);
static_assert(std::is_trivially_copyable_v<WindowMaterialState>);
static_assert(sizeof(WindowMaterialState) == 0x18);
static_assert(alignof(WindowMaterialState) == alignof(std::int32_t));
static_assert(offsetof(WindowMaterialState, materialValues) == 0x00);
static_assert(offsetof(WindowMaterialState, shopMode) == 0x14);
static_assert(offsetof(WindowMaterialState, reserved15) == 0x15);

// 原CC4240..CC466C确认上下文；函数指针原生化，不沿用x86字段偏移。
struct Window47RuntimeSlots {
    std::int32_t visibleMaskCC4240; // 原全局：0x00CC4240。
    std::int32_t widthUnitsCC4244; // 原全局：0x00CC4244。
    std::uint32_t valueCC4248; // 原全局：0x00CC4248。
    std::uint32_t valueCC424C; // 原全局：0x00CC424C。
    WindowCallback initializeCC4250; // 原全局：0x00CC4250。
    std::array<WindowElementCallback, 6> actionsCC4254; // 原全局：0x00CC4254。
    std::wstring textCC426C; // 原全局：0x00CC426C。
};

// 原CF4FE0的20条商店商品；4B6E00／4B70D0等生产，现代与旧买卖面板共享。
struct WindowShopItem {
    char used; // 原成员偏移：+0x00；按原字节判非零。
    std::wstring name; // 原成员偏移：+0x01。
    std::array<std::wstring, 32> description; // 原成员偏移：+0x1E。
    std::array<std::byte, 2> reservedC1E; // 原成员偏移：+0xC1E。
    std::int32_t imageNo; // 原成员偏移：+0xC20。
    std::int32_t price; // 原成员偏移：+0xC24。
    std::int32_t selected; // 原成员偏移：+0xC28；所选数量。
    std::int32_t inventoryIndex; // 原成员偏移：+0xC2C。
    std::int32_t count; // 原成员偏移：+0xC30。
    std::int32_t maximum; // 原成员偏移：+0xC34；保留单字节来源的S8扩展。
    std::int32_t valueC38; // 原成员偏移：+0xC38；回复数值，业务含义未定。
    char allowed; // 原成员偏移：+0xC3C；原U8／S8转换由使用方保留。
    std::array<std::byte, 3> reservedC3D; // 原成员偏移：+0xC3D。
    std::int32_t flags; // 原成员偏移：+0xC40；位标志按原位型读取。
    std::wstring memo; // 原成员偏移：+0xC44；内部附文。
};

// 原CB1E40/D2E458/CB0AE8确认显示记录；三商店各自持有，文本采用宽字符串。
struct WindowConfirmationItem {
    std::int32_t imageNo; // +00。
    std::int32_t quantity; // +04，数量文本第二项。
    std::int32_t factor; // +08，非零时显示乘数前缀。
    std::int32_t color; // +0C，绘制只读低S8。
    std::wstring name; // 原+10；内部宽文本。
    std::uint8_t visible; // +2F，不与名称分配另一份重叠存储。
};

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

// 原CC3210独立详情；4B77E0整区清零／生产，六个S32仍属同一对象。
struct WindowShopDetail {
    std::wstring name; // 原成员偏移：+0x00；内部宽文本。
    std::array<std::wstring, 32> description; // 原成员偏移：+0x1D；内部描述行。
    std::array<std::byte, 3> reservedC1D; // 原成员偏移：+0xC1D。
    std::int32_t imageNo; // 原成员偏移：+0xC20。
    std::int32_t quantity; // 原成员偏移：+0xC24。
    std::int32_t itemId; // 原成员偏移：+0xC28。
    std::int32_t price; // 原成员偏移：+0xC2C。
    std::int32_t capacity; // 原成员偏移：+0xC30。
    std::int32_t valueC34; // 原成员偏移：+0xC34；原分支显示值／现有数量，不归并用途。
};

// 原CB2230/D30428晶石商品；不同于普通商店C38H记录，保留区尚不定语义。
struct WindowCrystalItem {
    std::uint8_t used; // 原成员偏移：+0x00。
    std::wstring name; // 原成员偏移：+0x01。
    std::array<std::wstring, 32> description; // 原+0x1E；内部按行存放。
    std::array<std::byte, 2> bytesC1E; // 原成员偏移：+0xC1E。
    std::int32_t imageNo; // 原成员偏移：+0xC20。
    std::int32_t valueC24; // 原成员偏移：+0xC24。
    std::int32_t selectedCount; // +C28，两种晶石商店共用的数量字段语义。
    std::int32_t valueC2C; // 原成员偏移：+0xC2C。
    std::uint32_t flags; // 原成员偏移：+0xC30。
    std::wstring tips; // 原成员偏移：+0xC34。
    std::int32_t limitC4C; // 原成员偏移：+0xC4C。
    std::int32_t remainingC50; // 原成员偏移：+0xC50。
};

// 原CB1398等固定81B对话行；创建入口只借用，不负责释放。
struct WindowDialogTextLine {
    std::wstring text; // UTL02动态UTF-16分行正文；原81字节私有容量退休。
};

using WindowChoiceCallback = std::int32_t (*)(session::ClientSession& session, std::int32_t index);

// 原C9A978..C9A998，所有文本/表及回调均借用；计数保存低U8、循环按S8解释。
struct ChoiceDialogRuntimeSlots {
    std::int32_t type;
    std::int32_t imageNo;
    std::wstring title;
    std::vector<std::wstring> lines;
    std::vector<std::wstring> choices;
    std::uint8_t lineCount;
    std::uint8_t choiceCount;
    std::vector<std::wstring> hints;
    WindowChoiceCallback callback;
    std::uint8_t closeEnabled;
};

// 原D67448五个候选宠物28H记录，协议索引不等同于玩家宠物槽。
struct Window54PetOption {
    std::uint16_t used;
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t slots; // 原成员偏移：+0x08。
    std::wstring name; // 原成员偏移：+0x0C。
    std::uint8_t allowed; // 原成员偏移：+0x1D。
    std::array<std::byte, 2> bytes1E; // 原成员偏移：+0x1E。
    std::int32_t tribe; // 原成员偏移：+0x20。
    std::uint8_t hintFlag; // 原成员偏移：+0x24。
    std::array<std::byte, 3> bytes25; // 原成员偏移：+0x25。
};

// 原D66E38十条5CH技能槽，供模式4显示及确认覆盖层借用。
struct Window54TechniqueSlot {
    std::uint16_t used;
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::wstring name; // 原成员偏移：+0x08。
    std::array<std::wstring, 2> description; // 原成员偏移：+0x21。
    std::byte byte5B; // 原成员偏移：+0x5B。
};

// 原D2EAB8的60H技能报价记录；未消费字节不据此定业务语义。
struct Window54SkillRecord {
    std::uint16_t used;
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::int32_t price; // 原成员偏移：+0x08。
    std::wstring name; // 原成员偏移：+0x0C。
    std::array<std::wstring, 2> description; // 原成员偏移：+0x25。
    std::byte byte5F; // 原成员偏移：+0x5F。
};

// 原CB0FA8..CB0FD8的唯一UI状态；借用指针原生化，不沿用x86字段偏移。
struct MessageDialogRuntimeSlots {
    std::int32_t width;
    std::int32_t height;
    WindowDialogTextLine* lines;
    std::int32_t imageNo;
    std::int32_t buttons;
    std::int32_t lineCount;
    std::int32_t lineHeight;
    std::int32_t firstSelectableLine;
    std::int32_t inputCapacity;
    std::int16_t* inputY;
    std::uint8_t inputCount;
    std::uint8_t distance;
    std::uint8_t options;
    WindowDialogReplyCallback reply;
    WindowDialogFrameCallback frame;
    std::vector<std::wstring> inputTexts; // 原C9A968；UTL02动态UTF-16多输入正文。
    std::int32_t selectedInput; // 原D66E30。
    std::int32_t closeCallbackPending; // 原CC31A8，不是bool。
    std::int32_t hoveredButton; // 原CC18C0的完整W32按钮位。
    std::int32_t hoveredRow; // 原CF4A20。
    std::int32_t events; // 原D2DA90。
};

struct MessageDialogImageRecord {
    std::int32_t imageNo;
    std::uint8_t direction; // 原成员偏移：+0x04。
    std::byte byte05; // 原成员偏移：+0x05。
    std::int16_t x; // 原成员偏移：+0x06。
    std::int16_t y; // 原成员偏移：+0x08。
    std::uint8_t used; // 原成员偏移：+0x0A。
    std::byte byte0B; // 原成员偏移：+0x0B。
};
static_assert(sizeof(MessageDialogImageRecord) == 12);
static_assert(offsetof(MessageDialogImageRecord, direction) == 4);
static_assert(offsetof(MessageDialogImageRecord, x) == 6);
static_assert(offsetof(MessageDialogImageRecord, y) == 8);
static_assert(offsetof(MessageDialogImageRecord, used) == 10);

// 原CB1388..CB1394；49DF30/49E400共同生产，49DA30回复逐次读取。
struct DialogReplyRuntimeSlots {
    std::int32_t type;
    std::int32_t request;
    std::int32_t target;
    std::int32_t firstLine;
};

// 原D672F0七个32B记录；第七条同源D673B0/B8用于合计，4DC290也消费此表。
struct HealingDialogRecord {
    std::int32_t level;
    std::int32_t health;
    std::int32_t cost; // 原成员偏移：+0x08。
    std::wstring name; // 原成员偏移：+0x0C。
    std::array<std::byte, 3> reserved1D; // 原成员偏移：+0x1D。
};

// 原580870..580900：解析后的姓名与两行正文，GBK字节容量仅约束输入边界。
struct Window56TextStorage {
    std::wstring name;
    std::array<std::wstring, 2> messages;
};

struct WindowListRecordD3FAD4 {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::int32_t value04; // 原成员偏移：+0x04。
    std::array<std::wstring, 4> texts08; // 原成员偏移：+0x08。
    std::int32_t value408; // 原成员偏移：+0x408。
};

struct WindowChoiceSlotsCC4CA0 {
    std::int32_t value00; // 原成员偏移：+0x00。
    std::array<std::int32_t, 5> values04; // 原成员偏移：+0x04。
    std::array<std::array<std::int32_t, 2>, 5> pairs18; // 原成员偏移：+0x18。
};

// 仅承接D2F0B8已消费前缀，不视为完整对象或最终x64布局。
struct WindowRuntimePrefixD2F0B8 {
    std::int32_t value00; // 4A2610先写+10再写此槽，后续由邮件窗口消费。
    std::int32_t value04; // 原成员偏移：+0x04。 4A3C70写排序后的记录索引，不是地址。
    std::int32_t value08; // 原成员偏移：+0x08。
    std::int32_t value0C; // 原成员偏移：+0x0C。
    std::int32_t value10; // 原成员偏移：+0x10。 原W32方式值，不是地址。
    std::array<std::byte, 0x258> bytes14; // 原成员偏移：+0x14。
    std::uint32_t value26C; // 原成员偏移：+0x26C。
};

// 原CC1A30..CC207B，窗口22消费、窗口49生产；两个数组各200个数值槽。
struct WindowMailGroupSlotsCC1A30 {
    std::int32_t count;
    std::int32_t mode;
    std::array<std::int32_t, 200> indices;
    std::array<std::int32_t, 200> selected;
    std::int32_t initialize;
};

// 各原地址的唯一UI存储；此聚合的整体内存顺序不是原端连续布局。

struct LoginWindowRuntimeSlots {
    std::int32_t valueCF4A00; // 原全局：0x00CF4A00。
    std::int32_t valueC8A95C; // 原全局：0x00C8A95C。
    std::int32_t valueCB2200; // 原全局：0x00CB2200。
    std::uint32_t valueD3FAD0; // 原全局：0x00D3FAD0。
    std::array<WindowListRecordD3FAD4, 4> recordsD3FAD4; // 原全局：0x00D3FAD4。
    WindowChoiceSlotsCC4CA0 choicesCC4CA0; // 原全局：0x00CC4CA0。
    WindowRuntimePrefixD2F0B8 prefixD2F0B8; // 原全局：0x00D2F0B8。
    WindowRuntimePrefixD2F0B8 mailDraftCF4BC8; // 原全局：0x00CF4BC8。 4A5650只更新五字段和正文，保留其余字节。
    WindowMailGroupSlotsCC1A30 mailGroupCC1A30; // 原全局：0x00CC1A30。
    std::int32_t mailGroupScrollD67B88; // 原全局：0x00D67B88。
    std::int32_t mailGroupSortCategoryCB0600; // 原全局：0x00CB0600。
    std::array<std::int32_t, 200> mailGroupOrderCC2E6C; // 原全局：0x00CC2E6C。 原排序索引，末地址CC318C，不与技能HP区重叠。
    std::int32_t statusHasPointsD66E24; // 原全局：0x00D66E24。 470AC0生产，4723D0两次独立读取。
    std::int32_t statusHasInjuryD046C8; // 原全局：0x00D046C8。 伤势提示门禁，不是玩家伤势副本。
    std::int32_t statusHasMailCC4B5C; // 原全局：0x00CC4B5C。 470AC0的518DA0结果，按钮固定入口快照。
    std::int32_t statusMailCategoryD66E34; // 原全局：0x00D66E34。 4723D0传给518F00的同源输出。
    std::int32_t statusMailIndexD2E2D8; // 原全局：0x00D2E2D8。
    std::int32_t statusBlinkPhaseCF4FBC; // 原全局：0x00CF4FBC。 470AC0递增并在>60时回零，开窗不重置。
    std::int32_t valueD673D0; // 原全局：0x00D673D0。
    std::int32_t selectedPetCC3E5C; // 原全局：0x00CC3E5C。
    std::int32_t previousPetCC4888; // 原全局：0x00CC4888。 详情页记排序位置，不是实际宠物编号。
    std::array<std::int32_t, 5> petSequencesCAFEB0; // 原全局：0x00CAFEB0。
    std::wstring petNameDraftD45E74; // 原全局：0x00D45E74。
    std::int32_t petTechniqueD2D66C; // 原全局：0x00D2D66C。 原物理技能下标，生产见48F620/48F690/48F6C0。
    std::int32_t petTechniqueValueD2F784; // 原全局：0x00D2F784。 原拖动关联槽，48D9A0置-1，48F620/48F6C0后续写。
    std::int32_t petTechniqueDragD460CC; // 原全局：0x00D460CC。 4920F0绘制门禁，初始化由48D9A0负责。
    std::array<std::int32_t, 5> valuesCC1A18; // 原全局：0x00CC1A18。
    std::array<std::int32_t, 5> valuesCC4CE8; // 原全局：0x00CC4CE8。
    std::int32_t countCC4CFC; // 原全局：0x00CC4CFC。 48D460每帧重建的状态1计数，48C150不清此槽。
    std::array<std::int32_t, 3> valuesD2EAA0; // 原全局：0x00D2EAA0。
    std::array<std::int32_t, 2> valuesCF4E50; // 原全局：0x00CF4E50。
    std::array<std::int32_t, 7> valuesD67B40; // 原全局：0x00D67B40。 两组三类滚动位置，末槽为D67B58类别。
    std::uint32_t mailSortModeD67B3C; // 原全局：0x00D67B3C。 4A0970从存档live排序模式复制。
    std::int32_t mailBlinkPhaseD67B5C; // 原全局：0x00D67B5C。 4A32B0有符号余数相位。
    std::int32_t mailBlinkFlagD67B60; // 原全局：0x00D67B60。 每16帧异或1，开窗不清零。
    std::array<std::int32_t, 200> mailIndicesC9FCE4; // 原全局：0x00C9FCE4。 窗口21三类名单共用的数值索引，不与CA0004重叠。
    std::array<std::uint8_t, 2> mailGuildColorsD6784C; // 原全局：0x00D6784C。 公会状态文本两W8颜色，保留原虚拟零存储。
    std::int32_t mailMemberIndexCB1D18; // 原全局：0x00CB1D18。 4A38C0按编号查得，窗口55的4A4540消费。
    std::int32_t mailMemberKeyD45F88; // 原全局：0x00D45F88。 独立于D45F78金额文本；4A4370消费编号。
    std::int32_t mailRankCF4A14; // 原全局：0x00CF4A14。 4A38C0/4A4220生产，4A4370发送时读取。
    std::int32_t mailRankScrollCA0474; // 原全局：0x00CA0474。 窗口55滚动位置，初始化不清所选职级。
    std::int32_t mailCategoryD67B64; // 原全局：0x00D67B64。 4A2B00发布，4A47C0/4A48A0消费。
    std::int32_t mailCategoryCC4D50; // 原全局：0x00CC4D50。 4A2B00随后发布，4A49B0/4A4FC0消费。
    std::int32_t mailDeleteIndexD2DA8C; // 原全局：0x00D2DA8C。 删除确认与后续列表父链共用的记录索引。
    std::int32_t valueCC31A4; // 原全局：0x00CC31A4。
    std::int32_t valueD2E444; // 原全局：0x00D2E444。
    std::int32_t blacklistDeleteModeD67BC4; // 原全局：0x00D67BC4。 只由65删除按钮切换，初始化不重置。
    std::int32_t valueCB0604; // 原全局：0x00CB0604。
    std::uint32_t valueD67618; // 原全局：0x00D67618。 两组金额缓存选择，不再与扩展银行页号复用。
    std::int32_t bagPageCount = 1; // G07：INVCF背包页数，共享于14/27/11/41。
    std::int32_t bankPageCount = 1; // G07：INVCF银行页数。
    std::int32_t bagCurrentPage = 0; // G07：0基背包页，INVCF或页按钮更新。
    std::int32_t bankCurrentPage = 0; // G07：0基银行页，rh发送时加1，不索引金额缓存。
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

// 设置侧栏的动画现场；音量及其显示文本仍随进程共享的音频设置保存。
struct SettingsPanelRuntimeState {
    std::int8_t panel = 0; // 原0xCC4234。
    std::int32_t offset = 0; // 原0xCA02DC。
    std::int32_t step = 0; // 原0xD2D610。
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

// 技能、制作与采集在同一会话内共用，不拆为各窗口的独立副本。
struct SkillOperationRuntimeState {
    std::int32_t sendMode = 0; // 原0xC8A970。
    std::uint8_t rebirthEnabled = 0; // 原0x5F9754，观察进入也会清零。
    std::int32_t petImage = 0; // 原0xCB0A18。
    std::uint64_t previewTick = 0; // 原0xCF46C8，辅助预览同源计时槽。
    std::uint32_t extraDelay = 0; // 原0xCF46C4。
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
    WindowInstance* currentWindow = nullptr; // 原0xCF4694，帧派发和嵌套开窗的非拥有游标。
    detail::WindowState* currentWindowState = nullptr; // 原0xCF4684，派发/关闭共用记录游标。
    void* focusedElementData = nullptr; // 原0xD45F94，异型元素载荷借用，不是InputContext。
};

// 拆分数量框回送时读取的物品来源/目标，保留原独立发布顺序。
struct ItemTransferRuntimeState {
    std::int32_t source = 0; // 原0xCF46B0。
    std::int32_t target = 0; // 原0xD2DBF8。
};

// 普通商店及相关窗口共用槽；控件仅借本会话的稳定成员，不拆原跨窗别名。
struct ShopWindowRuntimeState {
    std::array<detail::WindowShopItem, 20> items {}; // 原0xCF4FE0。
    std::int32_t replyItemCount = 0; // 原0xCF4B58。
    std::int32_t dialogType = 0; // 原0xCC3EC8。
    std::int32_t selectionMask = 0; // 原0xCF467C。
    std::array<std::int8_t, 20> inventoryMap {}; // 原0xD2DAB8，背包8..27到首匹配商品。
    std::array<char, 20> selectedSlots {}; // 原0xCB03DC，保留S8槽读取。
    std::wstring exchangeText; // 原0xCB0184，模式2字段7反转义文本。
    std::int32_t detailMode = 0; // 原0xCAFEA4。
    std::int32_t detailSelected = 0; // 原0xD046C4。
    std::int32_t valueD672D4 = 0; // 原0xD672D4。
    std::int32_t tooltipIndex = -1; // 原0x580844。
    std::int32_t message = -1; // 原0x580848。
    std::int32_t inventoryHover = -2; // 原0x58084C，父初始化不重置。
    std::int32_t tooltipPage = 0; // 原0xD67B8C。
    std::int32_t total = 0; // 原0xD2DAFC。
    std::int32_t quantity = 0; // 原0xC8AF74，不与总金额合并。
    std::int32_t scroll = 0; // 原0xCC488C。
    std::array<std::wstring, 5> priceTexts {}; // 原0xD673EC。
    std::array<std::wstring, 5> countTexts {}; // 原0xD2D988。
    std::wstring totalText; // 原0xCC4B2C。
    std::wstring moneyText; // 原0xCC1A00。
    std::wstring valueText; // 原0xCF4A28。
    std::uint8_t dragFlag = 0; // 原0xD67B90。
    WindowValueCallback confirmationCallback = nullptr; // 原0xC8B088，确认/取消各自重读。
    std::array<detail::WindowConfirmationItem, 20> confirmationItems {}; // 原0xCB1E40。
    std::array<detail::WindowConfirmationAmount, 2> confirmationAmounts {}; // 原0xCB0F90。
    std::wstring confirmationHeading; // 原0xCB1320。
    std::wstring confirmationQuestion; // 原0xCB1210。
    std::array<std::uint64_t, 2> pressedTicks {}; // G08：加减各自计时。
    std::int32_t itemCount = 0; // 原0xCB1E10，普通/晶石商店共用，独立于回复商品数。
    std::int32_t imageNo = 0; // 原0xD2DAD0，外观入口也写入。
    std::int32_t currencyItemId = 0; // 原0xD2D608，数值而非地址。
    detail::WindowShopDetail detail {}; // 原0xCC3210，提示及晶石窗口也借用。
    std::uint16_t priceTail = 0; // 原0xD67420，第五行价格尾两字节，宠物候选-1读取。
    std::int32_t available = 0; // 原0xCF4A24，初始化另写-1。
    std::array<std::int32_t, 5> rowKeys {}; // 原0xC9A954，多窗口共用。
    std::wstring confirmationAcceptHint; // 原0xCC3FCC，宠物技能确认也借用。
    std::wstring confirmationCancelHint; // 原0xD45D70。
};
static_assert(sizeof(ShopWindowRuntimeState::selectedSlots) == 20);

// 窗口69的商品及确认层，不与同会话窗口70的独立状态混用。
struct CrystalShopWindowRuntimeState {
    std::array<detail::WindowCrystalItem, 20> items {}; // 原0xCB2230。
    std::int32_t initialized = 0; // 原0xD67B98。
    std::uint8_t flag = 0; // 原0xD2EA5B。
    std::int32_t imageNo = 0; // 原0xCB01F0。
    std::int32_t dialogType = 0; // 原0xD2D810。
    std::int32_t scroll = 0; // 原0xD2D604。
    std::int32_t dirty = 0; // 原0xD67B9C。
    std::int32_t cachedScroll = -1; // 原0x580858。
    std::int32_t total = 0; // 原0xD45E70。
    std::int32_t slots = 0; // 原0xCA0AC8。
    std::array<std::wstring, 5> priceTexts {}; // 原0xD2D7CC。
    std::array<std::wstring, 5> countTexts {}; // 原0xD2EA24。
    std::wstring totalText; // 原0xCC48B0。
    std::int32_t tooltip = -1; // 原0x580850。
    std::int32_t message = -1; // 原0x580854。
    std::int32_t tooltipPage = 0; // 原0xD67B94。
    std::int32_t inventoryHover = -2; // 原0x58085C。
    std::wstring balanceText; // 原0xD2E448。
    WindowValueCallback confirmCallback = nullptr; // 原0xCF4E48。
    std::array<detail::WindowConfirmationItem, 20> confirmItems {}; // 原0xD2E458。
    std::array<detail::WindowConfirmationAmount, 2> confirmAmounts {}; // 原0xD2E2B8。
    std::wstring confirmHeading; // 原0xCC3F68。
    std::wstring confirmQuestion; // 原0xCC3F04。
    std::wstring confirmAccept; // 原0xD2E81C。
    std::wstring confirmCancel; // 原0xCB076C。
};

// 窗口70保留额外第十协议字段及独立的数量、分页与确认区。
struct CrystalExchangeShopWindowRuntimeState {
    std::array<detail::WindowCrystalItem, 20> items {}; // 原0xD30428。
    std::int32_t initialized = 0; // 原0xD67BA4。
    std::uint8_t flag = 0; // 原0xD673D8。
    std::int32_t imageNo = 0; // 原0xD2E43C。
    std::int32_t exchangeItemId = 0; // 原0xD6791C，第十协议字段S32。
    std::int32_t dialogType = 0; // 原0xD45D6C。
    std::int32_t scroll = 0; // 原0xCC4B68。
    std::int32_t dirty = 0; // 原0xD67BA8。
    std::int32_t cachedScroll = -1; // 原0x580868。
    std::int32_t total = 0; // 原0xCB0F74。
    std::int32_t slots = 0; // 原0xCF4E44。
    std::array<std::wstring, 5> priceTexts {}; // 原0xCF49C4。
    std::array<std::wstring, 5> countTexts {}; // 原0xCC4848。
    std::wstring totalText; // 原0xD2D804。
    std::int32_t tooltip = -1; // 原0x580860。
    std::int32_t message = -1; // 原0x580864。
    std::int32_t tooltipPage = 0; // 原0xD67BA0。
    std::int32_t inventoryHover = -2; // 原0x58086C。
    std::wstring balanceText; // 原0xCF3F14。
    WindowValueCallback confirmCallback = nullptr; // 原0xCF4AA4。
    std::array<detail::WindowConfirmationItem, 20> confirmItems {}; // 原0xCB0AE8。
    std::array<detail::WindowConfirmationAmount, 2> confirmAmounts {}; // 原0xD2E290。
    std::wstring confirmHeading; // 原0xCF4A40。
    std::wstring confirmQuestion; // 原0xCF4ABC。
    std::wstring confirmAccept; // 原0xD2DD6C。
    std::wstring confirmCancel; // 原0xCC48BC。
};

// 窗口52实际配置及三类命令表；正文引用与同会话NPC行表保持同源。
struct ShopChoiceWindowRuntimeState {
    ShopChoiceWindowRuntimeState() noexcept;

    detail::ChoiceDialogRuntimeSlots dialog {}; // 原0xC9A978..0xC9A998。
    std::array<const std::wstring*, 5> lines {}; // 原0xD672D8。
    std::int32_t shopImage = 0; // 原0xC9A9A0。
    std::array<const wchar_t*, 3> shopChoices {}; // 原0xCB1DC8。
    std::array<const wchar_t*, 3> shopHints {}; // 原0xD2F738。
    std::array<std::int8_t, 3> shopCommands {}; // 原0xD64E10。
    std::int32_t firstCommandImage = 0; // 原0xCB0AE4。
    std::int32_t secondCommandImage = 0; // 原0xCF4A18。
    std::int32_t skillImage = 0; // 原0xCC4890。
    std::int32_t exchangeImage = 0; // 原0xC9A99C。
    std::int32_t giveReceiveImage = 0; // 原0xD2EA74。
    std::array<const wchar_t*, 2> firstChoices {}; // 原0xD67438。
    std::array<const wchar_t*, 2> firstHints {}; // 原0xCC31FC。
    std::array<std::int8_t, 2> firstCommands {}; // 原0xCB03D4。
    std::array<const wchar_t*, 2> secondChoices {}; // 原0xD2D818。
    std::array<const wchar_t*, 2> secondHints {}; // 原0xCA0AAC。
    std::array<std::int8_t, 2> secondCommands {}; // 原0xCF47D8。
    std::array<const wchar_t*, 2> giveReceiveChoices; // 原0x57E728，构造时保留原常量指针。
};

// 窗口54候选、报价、映射和显示文本；保留所有模式之间的共用状态。
struct PetSkillChoiceWindowRuntimeState {
    std::int32_t dialogType = 0; // 原0xCB2224。
    std::int32_t imageNo = 0; // 原0xCC18C4。
    std::int32_t valueCC2E68 = 0;
    std::int32_t valueCF4A3C = 0;
    std::int32_t firstCount = 0; // 原0xCB01E4。
    std::int32_t secondCount = 0; // 原0xCB01E8。
    std::uint8_t mode = 0; // 原0xC8AF64。
    std::int32_t lifecycle = 0; // 原0xD67434。
    std::array<detail::WindowDialogTextLine, 15> names {}; // 原0xC8AAA0。
    std::array<std::int32_t, 15> firstValues {}; // 原0xCB01A4。
    std::array<std::int32_t, 15> secondValues {}; // 原0xCB09D8。
    std::array<detail::Window54SkillRecord, 16> skills {}; // 原0xD2EAB8。
    std::int32_t skillScroll = 0; // 原0xCC31F4。
    std::int32_t message = 0; // 原0xC9FCE0。
    std::int32_t hover = 0; // 原0xD2DAD8。
    std::int32_t selected = 0; // 原0xD2DC04。
    std::wstring amountText {}; // 原0xD45F78。
    std::array<std::wstring, 10> firstValueTexts {}; // 原0xCB035C。
    std::array<std::wstring, 10> secondValueTexts {}; // 原0xC8B318。
    std::array<std::wstring, 5> skillValueTexts {}; // 原0xCF4978。
    std::array<std::wstring, 5> skillPriceTexts {}; // 原0xCF493C。
    std::array<detail::Window54PetOption, 5> petOptions {}; // 原0xD67448。
    std::array<std::int8_t, 5> petMap {}; // 原0xCF4AB0。
    std::int32_t tribe = 0; // 原0xCA0B2C。
    std::int32_t petChoice = 0; // 原0xC8AA90。
    std::int32_t petScroll = 0; // 原0xCC31DC。
    std::array<detail::Window54TechniqueSlot, world::kPetTechniqueSlotCount> techniqueSlots {}; // 原0xD66E38。
    std::int32_t replacement = 0; // 原0xC8B08C。
    std::array<std::int8_t, world::kSkillSlotCount> skillMap {}; // 原0xCC4D04。
    std::wstring moneyText {}; // 原0xC9A9A4。
};

// 窗口51自有正文、请求与选择，选择在背包换位时保留原单向重映射。
struct TextListWindowRuntimeState {
    std::vector<std::wstring> lines; // 原0xD67B70。
    std::int32_t lineCount = 0; // 原0xD67B74。
    std::int32_t scroll = 0; // 原0xCF46B8。
    std::wstring title {}; // 原0xCC4118。
    std::int32_t request = 0; // 原0xD672B0。
    std::int32_t target = 0; // 原0xCF46AC。
    std::int32_t closeRequest = 0; // 原0xCC4B28。
    std::wstring modalText {}; // 原0xC8AF88。
    std::int32_t selectedItem = -1; // 原0x57EE0C。
};

// 通用对话配置与实际借用源；图像、回复及治疗记录保留原独立清理范围。
struct MessageDialogWindowRuntimeState {
    detail::MessageDialogRuntimeSlots message {};
    input::InputContext input {}; // 原0xCA02E0，36/37共用的实体输入。
    detail::WindowInputPrototype inputPrototype { // 原0x57E478，容量成员原0x57E698。
        {{&input}, detail::WindowInputReferenceKind::Context},
        0, 0, 3, 0, 0, {}, 1, 32, 0, 0,
    };
    std::array<detail::MessageDialogImageRecord, 10> images {}; // 原0xCC4B70。
    std::uint8_t imageCount = 0; // 原0xD67B30，按S8消费。
    detail::DialogReplyRuntimeSlots reply {};
    std::array<detail::WindowDialogTextLine, 30> lines {}; // 原0xCB1398。
    std::array<std::int16_t, 2> inputY {}; // 原0xD67B34。
    std::array<detail::HealingDialogRecord, 7> healing {}; // 原0xD672F0。
    std::int32_t health = 0; // 原0xD2DC00。
};

// 窗口56的文本、版本上下界及五个外观分量。
struct AppearanceWindowRuntimeState {
    detail::Window56TextStorage texts {L"NPCNAME", {L"MESSAGE", L"MESSAGE"}}; // 原0x580870。
    std::int32_t dialogType = 0; // 原0xCB1DE0。
    std::int32_t request = 0; // 原0xCF49C0，确认仍按原读取公共对话参数。
    std::int32_t target = 0; // 原0xC8AF60。
    std::int32_t minimum = 0; // 原0xCB0F80。
    std::int32_t maximum = 0; // 原0xC8A958。
    std::int32_t base = 0; // 原0xCB0F8C。
    std::int32_t imageClass = 0; // 原0xCC466C。
    std::int32_t quarter = 0; // 原0xCC3E88。
    std::int32_t fifth = 0; // 原0xCB0F88。
    std::int32_t ones = 0; // 原0xCF4680。
};

// 类型37的图像预览对话，行表由通用对话借用，不转移所有权。
struct ImageDialogRuntimeState {
    std::int32_t type = 0; // 原0xD2F7A0。
    std::int32_t request = 0; // 原0xD2F7A4。
    std::int32_t target = 0; // 原0xD2F7A8。
    std::int32_t imageNo = 0; // 原0xD2F7AC。
    std::array<detail::WindowDialogTextLine, 13> lines {}; // 原0xD2F7B0，固定240高的可消费行。
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

// 普通/紧凑聊天窗共用的回看与尺寸请求，独立于实际布局尺寸。
struct ChatWindowRuntimeState {
    std::int32_t scrollOffset = 0; // 原0xC9A940。
    std::int32_t widthRequest = 0; // 原0xC9A934。
    std::int32_t heightRequest = 0; // 原0xC9A93C。
};

// 冒险日志的日期借用与读流；关闭窗口保留现场，移除会话时释放文件。
struct AdventureLogRuntimeState {
    std::tm* calendar = nullptr; // 原0xD67BAC，借所属会话的唯一日历对象。
    std::int64_t timestamp = 0; // 原0xCB1DF0。
    std::int32_t page = 0; // 原0xD67BB0。
    std::int32_t maximum = 0; // 原0xD67BB4，保留EOF空读取计数。
    std::FILE* file = nullptr; // 原0xD67BB8。
    std::int32_t error = 0; // 原0xD67BBC。
    std::int32_t day = 0; // 原0xD67BC0。
    std::int32_t displayGate = 0; // 原0x593F84。

    AdventureLogRuntimeState() = default;
    ~AdventureLogRuntimeState()
    {
        if (file != nullptr) { std::fclose(file); }
    }
    AdventureLogRuntimeState(const AdventureLogRuntimeState&) = delete;
    AdventureLogRuntimeState& operator=(const AdventureLogRuntimeState&) = delete;
    AdventureLogRuntimeState(AdventureLogRuntimeState&&) = delete;
    AdventureLogRuntimeState& operator=(AdventureLogRuntimeState&&) = delete;
};

// 同会话跨窗共用输入；原型仅借本对象内的稳定实体，不拆邮件/宠物页及快捷文字别名。
struct NativeInputRuntimeState {
    input::InputContext chat {}; // 原0x626AA0。
    input::InputContext chatDefaults {}; // 原0x626840，聊天恢复源。
    std::array<input::InputContext, 24> hotkeys {}; // 原0x5F9758，末项visibleStartOffset另供聊天借用。
    input::InputContext mail {}; // 原0xD66CA8，普通/宠物邮件共用。
    input::InputContext petName {}; // 原0xD2D4A0，多宠物页共用。
    input::InputContext contextCB03F0 {}; // 原0xCB03F0，独立回车分派身份。
    input::InputContext messageBoard {}; // 原0xCC2080。
    input::InputContext profileDescription {}; // 原0xD2D828。
    input::InputContext miniMail {}; // 原0xCA0180。
    input::InputContext profileSearch {}; // 原0xCB0610。
    input::InputContext stallName {}; // 原0xCB0028。
    input::InputContext stallDescription {}; // 原0xCC2378。
    input::InputContext stallPrice {}; // 原0xCF4E60。
    input::InputContext engraving {}; // 原0xD2DE70，不含相邻结果经验槽。
    input::InputContext playerTitle {}; // 原0xD2DC10。
    input::InputContext profileFirst {}; // 原0xD64CB0。
    input::InputContext profileSecond {}; // 原0xD2D670。
    input::InputContext profileThird {}; // 原0xCAFEC8。
    input::InputContext blacklist {}; // 原0xC8B1B8。
    input::InputContext tradeAmount {}; // 原0xCC21E8。
    input::InputContext quantity {}; // 原0xCB0878。
    input::InputContext storageAmount {}; // 原0xCA0018。
    input::InputContext textListTitle {}; // 原0xCC49C0。
    input::InputContext guild {}; // 原0xD2E130，九个公会输入原型共用。
    std::int8_t hotkeySelected = 0; // 原0xCC4B20，窗口初始化另写-1。
    detail::WindowInputPrototype chatPrototype { // 原0x57E970。
        {{&chat}, detail::WindowInputReferenceKind::Context},
        8, 5, 3, 9, 0, {}, 1, 76, 0, 0,
    };
    detail::WindowInputPrototype mailPrototype { // 原0x57E740。
        {{&mail}, detail::WindowInputReferenceKind::Context},
        22, 70, 3, 12, 0, {}, 10, 28, 12, 0,
    };
    detail::WindowInputPrototype petNamePrototype { // 原0x57DCF8。
        {{&petName}, detail::WindowInputReferenceKind::Context},
        66, 31, 3, 12, 6, {}, 1, 16, 0, 0,
    };
    detail::WindowInputPrototype hotkeyPrototype { // 原0x57DF48，编辑时借当前快捷文字项。
        {{nullptr}, detail::WindowInputReferenceKind::Context},
        0, 0, 3, 8, 0, {}, 1, 24, 0, 0,
    };
    detail::WindowInputPrototype storagePrototype { // 原0x57EBA0。
        {{&storageAmount}, detail::WindowInputReferenceKind::Context},
        0, -100, 3, 12, 127, {}, 1, 0, 0, 0,
    };
    detail::WindowInputPrototype engravingPrototype { // 原0x57D7F8。
        {{&engraving}, detail::WindowInputReferenceKind::Context},
        0, 0, 3, 12, 127, {}, 1, 16, 0, 0,
    };
    detail::WindowInputPrototype blacklistPrototype { // 原0x580EF0。
        {{&blacklist}, detail::WindowInputReferenceKind::Context},
        75, 30, 3, 9, 0, {}, 1, 20, 0, 0,
    };
    std::array<detail::WindowInputPrototype, 9> guildPrototypes {{ // 原0x57F040。
        {{{&guild}, detail::WindowInputReferenceKind::Context},
            116, 31, 3, 12, 6, {}, 1, 24, 0, 0},
        {{{&guild}, detail::WindowInputReferenceKind::Context},
            116, 49, 3, 12, 6, {}, 1, 24, 0, 0},
        {{{&guild}, detail::WindowInputReferenceKind::Context},
            38, 165, 3, 12, 6, {}, 1, 16, 0, 0},
        {{{&guild}, detail::WindowInputReferenceKind::Context},
            38, 184, 3, 12, 6, {}, 1, 16, 0, 0},
        {{{&guild}, detail::WindowInputReferenceKind::Context},
            38, 203, 3, 12, 6, {}, 1, 16, 0, 0},
        {{{&guild}, detail::WindowInputReferenceKind::Context},
            38, 222, 3, 12, 6, {}, 1, 16, 0, 0},
        {{{&guild}, detail::WindowInputReferenceKind::Context},
            38, 241, 3, 12, 6, {}, 1, 16, 0, 0},
        {{{&guild}, detail::WindowInputReferenceKind::Context},
            38, 260, 3, 12, 6, {}, 1, 16, 0, 0},
        {{{&guild}, detail::WindowInputReferenceKind::Context},
            38, 279, 3, 12, 6, {}, 1, 16, 0, 0},
    }};
    detail::WindowInputPrototype miniMailPrototype { // 原0x580CB0。
        {{&miniMail}, detail::WindowInputReferenceKind::Context},
        22, 70, 3, 12, 0, {}, 10, 28, 12, 0,
    };
    detail::WindowInputPrototype messageBoardPrototype { // 原0x580618。
        {{&messageBoard}, detail::WindowInputReferenceKind::Context},
        33, 368, 3, 12, 0, {}, 2, 70, 16, 0,
    };
    detail::WindowInputPrototype profileFirstPrototype { // 原0x57CEE8。
        {{&profileFirst}, detail::WindowInputReferenceKind::Context},
        97, 108, 3, 12, 6, {}, 1, 26, 0, 0,
    };
    detail::WindowInputPrototype profileSecondPrototype { // 原0x57D118。
        {{&profileSecond}, detail::WindowInputReferenceKind::Context},
        97, 130, 3, 12, 6, {}, 1, 26, 0, 0,
    };
    detail::WindowInputPrototype profileThirdPrototype { // 原0x57D348。
        {{&profileThird}, detail::WindowInputReferenceKind::Context},
        97, 152, 3, 12, 6, {}, 1, 26, 0, 0,
    };
    detail::WindowInputPrototype profileDescriptionPrototype { // 原0x57D578。
        {{&profileDescription}, detail::WindowInputReferenceKind::Context},
        97, 170, 3, 12, 6, {}, 4, 28, 14, 0,
    };
    detail::WindowInputPrototype playerTitlePrototype { // 原0x57CCB8。
        {{&playerTitle}, detail::WindowInputReferenceKind::Context},
        105, 31, 3, 12, 6, {}, 1, 16, 0, 0,
    };
    detail::WindowInputPrototype quantityPrototype { // 原0x57DA70。
        {{&quantity}, detail::WindowInputReferenceKind::Context},
        0, -100, 3, 8, 127, {}, 1, 0, 0, 0,
    };
    detail::WindowInputPrototype profileSearchPrototype { // 原0x580A80。
        {{&profileSearch}, detail::WindowInputReferenceKind::Context},
        20, 370, 3, 12, 6, {}, 1, 26, 0, 0,
    };
    detail::WindowInputPrototype stallNamePrototype { // 原0x581130。
        {{&stallName}, detail::WindowInputReferenceKind::Context},
        230, 11, 3, 4, 0, {}, 1, 16, 0, 0,
    };
    detail::WindowInputPrototype stallDescriptionPrototype { // 原0x581360。
        {{&stallDescription}, detail::WindowInputReferenceKind::Context},
        299, 51, 3, 12, 0, {}, 2, 28, 14, 0,
    };
    detail::WindowInputPrototype stallPricePrototype { // 原0x581590。
        {{&stallPrice}, detail::WindowInputReferenceKind::Context},
        0, -100, 3, 12, 127, {}, 1, 0, 0, 0,
    };
    detail::WindowInputPrototype textListTitlePrototype { // 原0x57EE10。
        {{&textListTitle}, detail::WindowInputReferenceKind::Context},
        0, 0, 3, 12, 0, {}, 1, 16, 0, 0,
    };
    detail::WindowInputPrototype tradeAmountPrototype { // 原0x5803D0。
        {{&tradeAmount}, detail::WindowInputReferenceKind::Context},
        0, -100, 3, 8, 127, {}, 1, 0, 0, 0,
    };
};

// 原生窗口实例地址稳定，状态记录与排序数组由同一账号会话持有。
struct NativeWindowRuntimeState {
    std::array<WindowInstance*, detail::kWindowCount> windows {}; // 原0xC8B094，非拥有排序引用。
    std::array<detail::WindowState, detail::kWindowCount> states {}; // 原0xC9A9B0，按ID寻址。
    detail::WindowPrototype dialogPrototype {}; // 原36/37共用的可变原型，只在首次借用时从宿主布局基线初始化。
    bool dialogPrototypeInitialized = false;
    WindowLayoutSettings settings {};
    SettingsPanelRuntimeState settingsPanel {};
    BattleCommandWindowState battleCommand {};
    SkillWindowRuntimeState skill {};
    TextTargetWindowState textTarget {};
    AttributeTargetWindowState attributeTarget {};
    SkillTargetRuntimeState skillTargets {};
    SkillRequestRuntimeState skillRequest {};
    SkillOperationRuntimeState skillOperation {};
    SkillResultRuntimeState skillResult {};
    GatherWindowRuntimeState gather {};
    CraftWindowRuntimeState craft {};
    WindowOperationRuntimeState operations {};
    WindowPointerRuntimeState pointer {};
    detail::LoginWindowRuntimeSlots business {};
    NativeInputRuntimeState inputs {};
    ChatWindowRuntimeState chat {};
    AdventureLogRuntimeState adventureLog {};
    ItemTransferRuntimeState itemTransfer {};
    ShopWindowRuntimeState shop {};
    CrystalShopWindowRuntimeState crystalShop {};
    CrystalExchangeShopWindowRuntimeState crystalExchangeShop {};
    ShopChoiceWindowRuntimeState shopChoice {};
    PetSkillChoiceWindowRuntimeState petSkillChoice {};
    TextListWindowRuntimeState textList {};
    std::array<detail::WindowDialogTextLine, 30> npcDialogLines {}; // 原0xCC24D4，首行标题与其余正文共用。
    MessageDialogWindowRuntimeState dialog {};
    AppearanceWindowRuntimeState appearance {};
    ImageDialogRuntimeState imageDialog {};
    detail::WindowMaterialState material {}; // 原0xCC4894，含商店模式别名。
    detail::Window47RuntimeSlots confirmation {};
    ItemTargetRuntimeState itemTargets {};
    detail::WindowSavedData savedData {}; // 原0xD40B30。
    std::int32_t savedLength = 0; // 原0xCC4238。
};

} // namespace cg::ui
