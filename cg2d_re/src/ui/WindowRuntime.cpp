#include "WindowRuntimeState.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../LoginSceneDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "../platform/WindowProcedureState.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <type_traits>

namespace cg::ui {
namespace detail {

std::array<WindowInstance*, kWindowCount> gWindows {};
std::array<WindowState, kWindowCount> gWindowStates {};
WindowInstance* gCurrentWindow = nullptr;
WindowState* gCurrentWindowState = nullptr;
std::uint8_t gValueD67BE0 = 0;
std::int32_t gDragItemIndex584DA4 = -1;
std::int32_t gDragImageNoD67D24 = 0;
std::int32_t gItemDragXCC4880 = 0;
std::int32_t gItemDragYCC4884 = 0;
std::int32_t gDraggingCA0B28 = 0;
std::int32_t gValue58589C = 1;
void* gFocusedElementDataD45F94 = nullptr;
std::int32_t gMouseValue57C110 = 0;
std::int32_t gDragSourceIndex57C114 = 0;
WindowDragCallback gDragCallback57C118 = nullptr;
std::int16_t gDragX57C11C = 0;
std::int16_t gDragY57C11E = 0;
std::int32_t gFocusWindowId57C120 = 0;
std::int32_t gFocusElementIndex57C124 = -1;
std::int32_t gMousePhase57C128 = 0;
std::int32_t gCurrentElementIndex57C12C = -1;
std::int32_t gCurrentWindowIdD67A1C = 0;
std::uint32_t gWindowHasFocusCA0AB4 = 0;
WindowInputPrototype gChatInputPrototype57E970 {
    {{&input::gInputContext626AA0}, WindowInputReferenceKind::Context},
    8, 5, 3, 9, 0, {}, 1, 76, 0, 0,
};
std::int32_t gChatWindowModeD66E04 = 0;
std::uint8_t gValue5F9754 = 0;
std::int32_t gChatWidthUnitsC9A930 = 0;
std::int32_t gChatWidthRequestC9A934 = 0;
std::int32_t gChatHeightUnitsC9A938 = 0;
std::int32_t gChatHeightRequestC9A93C = 0;
std::int32_t gChatScrollOffsetC9A940 = 0;
std::array<char, 2 * 20 * 0xC4C> gTradeItemsD460D0 {};
std::array<char, 2 * 5 * 0x5FC> gTradePetsC9C108 {};
std::array<char, 2 * 0xFBF8> gItemDetailStorageCC4D58 {};
static_assert(sizeof(gTradeItemsD460D0) == 0x1EBE0);
static_assert(sizeof(gTradePetsC9C108) == 0x3BD8);
static_assert(sizeof(gItemDetailStorageCC4D58) == 0x1F7F0);
static_assert(0x60 + 10 * 0x8C == 0x5D8);
std::int32_t gTradeValueCC4224 = 0;
std::int32_t gTradeValueCF4688 = 0;
std::int32_t gTradeValueD046D0 = 0;
std::int32_t gTradeValueD2E2B0 = 0;
std::uint8_t gTradeValueD67B80 = 0;
std::int32_t gSelectedItemCB1DF4 = 0;
std::int32_t gSelectedItemCB0A44 = 0;
std::int32_t gSelectedItem57EE0C = -1;
std::uint8_t gSkillPairIndexD67AD8 = 0;
std::int32_t gRecipeIndex5846BC = -1;
std::array<std::uint8_t, 20> gTradeItemSlotsCF3AE8 {};
std::array<std::uint8_t, 28> gTradeItemMarksD2D61C {};
std::array<std::int32_t, 6> gSelectedSlotsCA0A90 {};
std::array<std::int32_t, 6> gMaterialIndicesCA0458 {};
std::array<std::int32_t, 6> gSelectionCountsD66E0C {};
std::array<std::int32_t, 6> gSelectionImagesCF3FF4 {};
std::array<char, 24> gMaterialOccupancyCC4894 {};
std::int32_t gSelectedItemD460C0 = 0;
std::int32_t gSelectedImageCC3ED8 = 0;
std::int32_t gSelectedCountCC483C = 0;
std::uint8_t gSelectionModeD2D650 = 0;
std::array<std::uint8_t, 28> gItemMarksD67290 {};
std::array<std::uint8_t, 20> gItemMarksD68A90 {};
std::uint8_t gValue57DA28 = 0xFF;
std::uint8_t gValue57E42C = 0xFF;
std::int32_t gValueCF4AAC = 0;
std::int32_t gValueCF4690 = 0;
std::int32_t gValueCA0440 = 0;
std::int32_t gValueD67B38 = 0;

namespace {

// 原D40B30的521CH设置文件；不与含原生指针的WindowState布局混用。
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
    std::array<WindowSavedRecord, kWindowCount> records;
};

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
WindowSavedData gWindowSavedDataD40B30 {};
std::int32_t gWindowSavedLengthCC4238 = 0;
std::uint32_t gValueCF4B6C = 0; // 原4705D0独立清零槽，不扩展用途。

struct WindowListRecordD3FAD4 {
    std::int32_t value00;
    std::int32_t value04;
    std::array<std::array<char, 0x100>, 4> texts08;
    std::int32_t value408;
};

struct WindowChoiceSlotsCC4CA0 {
    std::int32_t value00;
    std::array<std::int32_t, 5> values04;
    std::array<std::array<std::int32_t, 2>, 5> pairs18;
};

// 仅承接D2F0B8已消费前缀，不视为完整对象或最终x64布局。
struct WindowRuntimePrefixD2F0B8 {
    std::array<std::byte, 8> bytes00;
    std::int32_t value08;
    std::int32_t value0C;
    std::array<std::byte, 0x25C> bytes10;
    std::uint32_t value26C;
};

static_assert(offsetof(WindowListRecordD3FAD4, value00) == 0);
static_assert(offsetof(WindowListRecordD3FAD4, value04) == 4);
static_assert(offsetof(WindowListRecordD3FAD4, texts08) == 8);
static_assert(offsetof(WindowListRecordD3FAD4, value408) == 0x408);
static_assert(sizeof(WindowListRecordD3FAD4) == 0x40C);
static_assert(offsetof(WindowChoiceSlotsCC4CA0, values04) == 4);
static_assert(offsetof(WindowChoiceSlotsCC4CA0, pairs18) == 0x18);
static_assert(sizeof(WindowChoiceSlotsCC4CA0) == 0x40);
static_assert(offsetof(WindowRuntimePrefixD2F0B8, value08) == 8);
static_assert(offsetof(WindowRuntimePrefixD2F0B8, value0C) == 0x0C);
static_assert(offsetof(WindowRuntimePrefixD2F0B8, value26C) == 0x26C);
static_assert(sizeof(WindowRuntimePrefixD2F0B8) == 0x270);

// 各原地址的唯一UI存储；此聚合的整体内存顺序不是原端连续布局。
struct LoginWindowRuntimeSlots {
    std::int32_t valueCF4A00;
    std::int32_t valueC8A95C;
    std::int32_t valueCB2200;
    std::uint32_t valueD3FAD0;
    std::array<WindowListRecordD3FAD4, 4> recordsD3FAD4;
    WindowChoiceSlotsCC4CA0 choicesCC4CA0;
    WindowRuntimePrefixD2F0B8 prefixD2F0B8;
    std::int32_t valueD673D0;
    std::int32_t selectedPetCC3E5C;
    std::array<std::int32_t, 5> valuesCC1A18;
    std::array<std::int32_t, 5> valuesCC4CE8;
    std::array<std::int32_t, 3> valuesD2EAA0;
    std::array<std::int32_t, 2> valuesCF4E50;
    std::array<std::int32_t, 7> valuesD67B40;
    std::int32_t valueCC31A4;
    std::int32_t valueD2E444;
    std::int32_t valueCB0604;
    std::uint32_t valueD67618;
    platform::LegacyTm53B8C6* calendarTimeD67BAC; // 借用platform同一可变localtime对象。
    std::int32_t timeCB1DF0;
    std::int32_t valueD67BB0;
    std::int32_t dayD67BC0;
};

LoginWindowRuntimeSlots gLoginWindowRuntime {};

// 原57C24C目标的内联矩形；同值异地址仍分别存储，见窗口SPEC。
WindowDragRect gDragRects561998[] {{0, 0, 330, 25}, {315, 0, 20, 92}};
WindowDragRect gDragRects561F38[] {{10, 0, 230, 33}, {243, 5, 18, 79}};
WindowDragRect gDragRects5624C0[] {{10, 0, 553, 27}, {566, 5, 18, 79}};
WindowDragRect gDragRects562800[] {{10, 0, 288, 27}, {201, 5, 18, 79}};
WindowDragRect gDragRects562AF0[] {{10, 0, 288, 27}, {201, 5, 18, 79}};
WindowDragRect gDragRects562FD8[] {{22, 0, 256, 33}, {282, 7, 18, 78}};
WindowDragRect gDragRects5631F0[] {{12, 0, 113, 26}, {128, 7, 18, 78}};
WindowDragRect gDragRects565560[] {{0, 0, 343, 30}, {323, 0, 20, 122}};
WindowDragRect gDragRects566E70[] {{0, 0, 272, 30}, {252, 0, 20, 100}};
WindowDragRect gDragRects567908[] {{0, 0, 231, 25}, {221, 0, 30, 100}};
WindowDragRect gDragRects568528[] {{0, 0, 279, 25}, {249, 0, 25, 120}};
WindowDragRect gDragRects568DC8[] {{16, 0, 184, 30}, {209, 7, 18, 78}};
WindowDragRect gDragRects569298[] {{10, 0, 275, 33}, {288, 5, 18, 79}};
WindowDragRect gDragRects56B030[] {{0, 0, 184, 21}};
WindowDragRect gDragRects56B538[] {{0, 0, 61, 25}};
WindowDragRect gDragRects56C1F8[] {{9, 0, 481, 27}, {493, 7, 18, 78}};
WindowDragRect gDragRects56C6F0[] {{17, 0, 300, 33}, {320, 7, 18, 78}};
WindowDragRect gDragRects56C900[] {{17, 0, 339, 33}, {359, 7, 18, 78}};
WindowDragRect gDragRects56E918[] {{0, 0, 342, 30}, {331, 0, 20, 120}};
WindowDragRect gDragRects56FC00[] {{0, 0, 243, 10}};
WindowDragRect gDragRects570010[] {{0, 0, 267, 25}, {207, 0, 25, 100}};
WindowDragRect gDragRects5704B8[] {{0, 0, 232, 30}, {207, 0, 25, 100}};
WindowDragRect gDragRects570F30[] {{0, 0, 100, 25}};
WindowDragRect gDragRects571620[] {{9, 0, 558, 27}, {571, 7, 18, 78}};
WindowDragRect gDragRects571800[] {{9, 0, 558, 27}, {571, 7, 18, 78}};
WindowDragRect gDragRects571D48[] {{10, 0, 231, 27}, {234, 5, 18, 79}};
WindowDragRect gDragRects572898[] {{0, 0, 320, 25}, {317, 0, 20, 100}};
WindowDragRect gDragRects572D78[] {{9, 0, 558, 27}, {571, 7, 18, 78}};
WindowDragRect gDragRects573430[] {{0, 0, 280, 27}, {260, 0, 20, 120}};
WindowDragRect gDragRects573A48[] {{0, 0, 634, 30}, {604, 0, 30, 130}};
WindowDragRect gDragRects5745C0[] {{16, 0, 533, 27}, {552, 5, 18, 80}};
WindowDragRect gDragRects575648[] {{16, 0, 533, 27}, {552, 5, 18, 80}};
WindowDragRect gDragRects576248[] {{16, 0, 533, 27}, {552, 5, 18, 80}};
WindowDragRect gDragRects576E48[] {{16, 0, 533, 27}, {552, 5, 18, 80}};
WindowDragRect gDragRects577698[] {{0, 0, 231, 25}, {221, 0, 30, 100}};
WindowDragRect gDragRects579D28[] {{0, 0, 634, 35}, {600, 0, 34, 130}};
WindowDragRect gDragRects579DD0[] {{0, 0, 305, 25}, {300, 0, 30, 130}};
WindowDragRect gDragRects579E78[] {{0, 0, 317, 25}, {307, 0, 30, 130}};
WindowDragRect gDragRects57AD30[] {{0, 0, 231, 25}, {221, 0, 30, 100}};
WindowDragRect gDragRects57B1E0[] {{0, 0, 460, 25}, {445, 0, 30, 102}};
WindowDragRect gDragRects57B460[] {{16, 0, 533, 27}, {552, 5, 18, 80}};

std::array<WindowDragPrototype, 42> gDragPrototypes {{
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

// UI原字节区的窄数值通过复制写入，不对未对齐地址构造数值T*。
template <typename T>
void WriteTradeValue(char* const record, const std::size_t offset, const T value)
{
    static_assert(sizeof(T) == 2 || sizeof(T) == 4);
    std::memcpy(record + offset, &value, sizeof(value));
}

// 原54EFE8的71项S8类别，与窗口编号一一对应。
constexpr std::array<std::int8_t, kWindowCount> kWindowCategories {
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 4, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 4, 4, 0, 1, 2, 2, 0, 1, 2, 4, 2, 2, 2, 2, 2,
    1, 1, 2, 4, 2, 2, 2, 1, 2, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 2, 2, 2, 2,
};

constexpr std::array<std::int32_t, 52> kNumberDigitImages54F0D8 {
    243300, 243310, 243320, 243330, 243340, 243350, 243360,
    243370, 243380, 243390, 245500, 245500, 245500,
    243500, 243510, 243520, 243530, 243540, 243550, 243560,
    243570, 243580, 243590, 243500, 243500, 243500,
    243700, 243710, 243720, 243730, 243740, 243750, 243760,
    243770, 243780, 243790, 243700, 243700, 243700,
    245520, 245520, 245520, 245520, 245520, 245520, 245560,
    245580, 245520, 245520, 245540, 245540, 245540,
};

constexpr std::array<std::int32_t, 52> kNumberSymbolImages54F1A8 {
    243400, 243410, 243420, 243430, 243440, 243450, 243460,
    243470, 243480, 243490, 245510, 245510, 245510,
    243600, 243610, 243620, 243630, 243640, 243650, 243660,
    243670, 243680, 243690, 243600, 243600, 243600,
    243800, 243810, 243820, 243830, 243840, 243850, 243860,
    243870, 243880, 243890, 243800, 243800, 243800,
    245530, 245530, 245530, 245530, 245530, 245530, 245570,
    245580, 245530, 245530, 245550, 245550, 245550,
};

constexpr std::array<std::int32_t, 12> kNumberOffsets54F278 {
    2, 2, 5, 5, 4, 8, 5, 5, 8, 2, 3, 5,
};
constexpr std::array<std::int32_t, 4> kNumberSpacingDeductions54F2A8 {2, 2, 2, 1};
std::array<std::int32_t, 4> gNumberAdvancesD67AC4 {};

// 元素事件与阶段推进共用映射，按原edge/held/旧edge/pulse顺序采样与合并。
std::int32_t AppendMouseButtonEvents(std::int32_t events)
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

// 原5FB7F4是最后一条输入上下文的尾槽，不是日志511的font。
std::int32_t& PreviousChatFont(const std::int32_t index)
{
    if (index == 0) {
        return input::gInputContexts5F9758[23].field158;
    }
    return world::detail::gRecords5FB7F8[index - 1].fontType;
}

// 两个原入口只在宽度来源上不同；测量IME及取宽度仍由各入口按序执行。
void ScrollInputAndDraw(input::InputContext* const context, const std::int32_t width)
{
    const std::int32_t caret = context->caretByteOffset;
    const std::int32_t offset = context->field158;
    context->field154 = width;
    if (caret < offset) {
        context->field158 = caret;
    } else {
        for (;;) {
            const std::int32_t currentOffset = context->field158;
            const std::int32_t length = context->caretByteOffset - currentOffset;
            if (length < 0) {
                break;
            }
            const std::int32_t textWidth = render::Font::measure(
                context->text + currentOffset, context->field13C, length);
            if (context->field154 >= textWidth) {
                break;
            }
            const std::uint8_t value = static_cast<std::uint8_t>(
                context->text[context->field158]);
            context->field158 += (value & 0x80u) != 0 ? 2 : 1;
        }
    }
    render::DrawInputText_4568A0(context);
}

} // namespace

// 原71槽逐项绑定；下标仅用于静态初始化，不是对象句柄。
std::array<WindowDragPrototype*, kWindowCount> gWindowDragPrototypes57C24C {
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
};

std::int32_t Sub_4B0F10(const std::int32_t, const std::int32_t)
{
    return 0;
}

std::int32_t Sub_4A4210(const std::int32_t) noexcept
{
    return 1;
}

void Sub_4E5DA0(
    const std::int32_t side, const std::int32_t first, const std::int32_t second)
{
    if (first < 8 || first >= 28 || second < 8 || second >= 28) {
        return;
    }
    char* const left = gTradeItemsD460D0.data() + (side * 20 + first - 8) * 0xC4C;
    char* const right = gTradeItemsD460D0.data() + (side * 20 + second - 8) * 0xC4C;
    std::array<char, 0xC4C> temporary;
    std::memcpy(temporary.data(), left, temporary.size());
    std::memmove(left, right, temporary.size());
    std::memcpy(right, temporary.data(), temporary.size());
}

void Sub_4E1840(
    const std::int32_t side, const std::int32_t first, const std::int32_t second)
{
    if (first < 8 || first >= 28 || second < 8 || second >= 28) {
        return;
    }
    char* const left = gItemDetailStorageCC4D58.data() + side * 0xFBF8 + (first - 8) * 0xC44;
    char* const right = gItemDetailStorageCC4D58.data() + side * 0xFBF8 + (second - 8) * 0xC44;
    std::array<char, 0xC44> temporary;
    std::memcpy(temporary.data(), left, temporary.size());
    std::memmove(left, right, temporary.size());
    std::memcpy(right, temporary.data(), temporary.size());
}

std::int32_t Sub_4F4630(const std::int32_t itemId)
{
    const std::int8_t pairIndex = std::bit_cast<std::int8_t>(gSkillPairIndexD67AD8);
    const std::int16_t skillIndex = std::bit_cast<std::int16_t>(
        world::detail::gSkillPairsF63370[pairIndex].index);
    const std::int32_t recipeIndex = gRecipeIndex5846BC;
    const auto& recipe = world::detail::gSkillsF1BAFC[skillIndex].recipes8E8[recipeIndex];
    for (const world::detail::SkillRecipeItem& item : recipe.items6C) {
        if (item.itemId == itemId) {
            return item.count;
        }
    }
    return 0;
}

void Sub_4B0000(const std::int32_t first, const std::int32_t second)
{
    if (gWindowStates[43].window == nullptr) {
        return;
    }
    for (std::uint8_t& slot : gTradeItemSlotsCF3AE8) {
        const std::int32_t value = std::bit_cast<std::int8_t>(slot);
        if (value == first) {
            slot = static_cast<std::uint8_t>(second);
        } else if (value == second) {
            slot = static_cast<std::uint8_t>(first);
        }
    }
    const std::uint8_t firstMark = gTradeItemMarksD2D61C[first];
    const std::uint8_t secondMark = gTradeItemMarksD2D61C[second];
    gTradeItemMarksD2D61C[first] = secondMark;
    gTradeItemMarksD2D61C[second] = firstMark;
}

std::int32_t Sub_47DC20(const std::int32_t index)
{
    auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    if (index < 6) {
        const std::int32_t slot = gSelectedSlotsCA0A90[index];
        if (slot < 0) {
            return slot;
        }
        const std::uint8_t mode = gSelectionModeD2D650;
        if (mode == 6 || mode == 15) {
            gItemMarksD68A90[slot] = 0;
        }
        const std::int32_t material = gMaterialIndicesCA0458[index];
        if (material >= 0) {
            WriteTradeValue(gMaterialOccupancyCC4894.data(), material * 4, std::int32_t{0});
        }
        hover[slot + 8] = static_cast<unsigned char>(hover[slot + 8] - 1);
        gItemMarksD67290[slot + 8] = 0;
        gSelectionCountsD66E0C[index] = 0;
        gSelectedSlotsCA0A90[index] = -1;
        gSelectionImagesCF3FF4[index] = -1;
        gMaterialIndicesCA0458[index] = -1;
        return -1;
    }
    const std::int32_t slot = gSelectedItemD460C0;
    if (slot < 0) {
        return slot;
    }
    const unsigned char previous = hover[slot];
    gItemMarksD67290[slot] = 0;
    hover[slot] = static_cast<unsigned char>(previous - 1);
    gSelectedItemD460C0 = -1;
    gSelectedImageCC3ED8 = -1;
    gSelectedCountCC483C = -1;
    return Sub_4A7B00();
}

void Sub_47DCD0(const std::int32_t first, const std::int32_t second)
{
    if (gWindowStates[11].window == nullptr) {
        return;
    }
    const std::int32_t firstSlot = first - 8;
    const std::int32_t secondSlot = second - 8;
    for (std::int32_t index = 0; index < 6; ++index) {
        const std::int32_t slot = gSelectedSlotsCA0A90[index];
        if (slot < 0) {
            continue;
        }
        if (slot == firstSlot) {
            if (secondSlot < 0) {
                static_cast<void>(Sub_47DC20(index));
            } else {
                gSelectedSlotsCA0A90[index] = secondSlot;
            }
        } else if (slot == secondSlot) {
            if (firstSlot < 0) {
                static_cast<void>(Sub_47DC20(index));
            } else {
                gSelectedSlotsCA0A90[index] = firstSlot;
            }
        }
    }
    const std::int32_t selected = gSelectedItemD460C0;
    if (selected == first) {
        gSelectedItemD460C0 = second;
        const std::int32_t imageNo = world::detail::gPlayerItemsFDA8A4[second].imageNo;
        // 原47DD63使用Items.itemId+62E0，确与另一分支的基址相差八条。
        const std::int32_t itemId = world::detail::gPlayerItemsFDA8A4[second + 8].itemId;
        gSelectedImageCC3ED8 = imageNo;
        gSelectedCountCC483C = Sub_4F4630(itemId);
    } else if (selected == second) {
        gSelectedItemD460C0 = first;
        const std::int32_t imageNo = world::detail::gPlayerItemsFDA8A4[first].imageNo;
        const std::int32_t itemId = world::detail::gPlayerItemsFDA8A4[first].itemId;
        gSelectedImageCC3ED8 = imageNo;
        gSelectedCountCC483C = Sub_4F4630(itemId);
    }
    if (first < 0 || second < 0) {
        return;
    }
    const std::uint8_t firstMark = gItemMarksD67290[first];
    const std::uint8_t secondMark = gItemMarksD67290[second];
    gItemMarksD67290[first] = secondMark;
    gItemMarksD67290[second] = firstMark;
    const std::uint8_t mode = gSelectionModeD2D650;
    if (mode == 6 || mode == 15) {
        const std::uint8_t firstValue = gItemMarksD68A90[first];
        const std::uint8_t secondValue = gItemMarksD68A90[second];
        gItemMarksD68A90[first] = secondValue;
        gItemMarksD68A90[second] = firstValue;
    }
}

void Sub_4AD060(const std::int32_t first, const std::int32_t second)
{
    if (gWindowStates[51].window != nullptr && gSelectedItem57EE0C == first) {
        gSelectedItem57EE0C = second;
    }
}

void Sub_4797C0(const std::int32_t first, const std::int32_t second)
{
    if (gValue5F9754 == 0) {
        return;
    }
    auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    if (first == 7) {
        hover[7] = static_cast<unsigned char>(hover[7] + 1);
        hover[second] = static_cast<unsigned char>(hover[second] - 1);
    }
    if (second == 7) {
        hover[7] = static_cast<unsigned char>(hover[7] + 1);
        hover[first] = static_cast<unsigned char>(hover[first] - 1);
    }
}

void Sub_48BA60(const std::int32_t first, const std::int32_t second)
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
    if (world::detail::gPlayerItemsFDA8A4[first].imageNo == 0x67C9
        || world::detail::gPlayerItemsFDA8A4[second].imageNo == 0x67C9) {
        const std::int32_t previous = gSelectedItemCB0A44;
        if (previous == first) {
            gSelectedItemCB0A44 = second;
        } else if (previous == second) {
            gSelectedItemCB0A44 = first;
        }
    }
    Sub_4B0000(first, second);
    Sub_47DCD0(first, second);
    Sub_4AD060(first, second);
    Sub_4797C0(first, second);
    auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    const unsigned char secondValue = hover[second];
    const unsigned char firstValue = hover[first];
    hover[first] = secondValue;
    hover[second] = firstValue;
}

std::int32_t Sub_4FB8F0()
{
    gDragItemIndex584DA4 = -1;
    gDragImageNoD67D24 = 0;
    gItemDragXCC4880 = 0;
    gItemDragYCC4884 = 0;
    gDraggingCA0B28 = 0;
    gValue58589C = 1;
    return 0;
}

std::int32_t Sub_4FB920(const std::int32_t index)
{
    const std::int32_t previous = gDragItemIndex584DA4;
    if (previous >= 0 && previous == index) {
        return Sub_4FB8F0();
    }
    return previous;
}

void Sub_4E54E0(const std::int32_t side, const std::int32_t index)
{
    if (index < 0 || index >= 20) {
        return;
    }
    char* const record = gTradeItemsD460D0.data() + (index + side * 20) * 0xC4C;
    std::memset(record, 0, 0xC4C);
    const world::detail::PlayerItemRecord& source = world::detail::gPlayerItemsFDA8A4[index + 8];
    if (source.used == 0) {
        return;
    }
    const std::int32_t imageNo = source.imageNo;
    const std::int32_t count = source.count;
    WriteTradeValue(record, 0, std::uint16_t{1});
    const std::uint16_t flags = source.flags;
    WriteTradeValue(record, 4, imageNo);
    const std::int32_t itemId = source.itemId;
    WriteTradeValue(record, 8, count);
    WriteTradeValue(record, 0x0C, itemId);
    WriteTradeValue(record, 0xC34,
        static_cast<std::int32_t>(std::bit_cast<std::int16_t>(flags)));
    if (std::strlen(source.memo.data()) != 0) {
        std::strcpy(record + 0xC38, source.memo.data());
    }
    std::strcpy(record + 0x10, source.name.data());
    if ((flags & 1u) == 0) {
        WriteTradeValue(record, 2, std::uint16_t{0xFFFF});
    }
}

void Sub_4E5600(const std::int32_t side)
{
    for (std::int32_t index = 0; index < 20; ++index) {
        Sub_4E54E0(side, index);
    }
}

void Sub_4E0100(const std::int32_t side, const std::int32_t index)
{
    if (index < 0 || index >= 20) {
        return;
    }
    const world::detail::PlayerItemRecord& source = world::detail::gPlayerItemsFDA8A4[index + 8];
    const bool used = source.used != 0;
    char* const record = gItemDetailStorageCC4D58.data() + side * 0xFBF8 + index * 0xC44;
    std::memset(record, 0, 0xC44);
    if (!used) {
        return;
    }
    const std::int32_t imageNo = source.imageNo;
    const std::int32_t count = source.count;
    WriteTradeValue(record, 0, std::uint16_t{1});
    WriteTradeValue(record, 4, imageNo);
    WriteTradeValue(record, 8, count);
    const std::uint16_t flags = source.flags;
    WriteTradeValue(record, 0xC40,
        static_cast<std::int32_t>(std::bit_cast<std::int16_t>(flags)));
    if (std::strlen(source.memo.data()) != 0) {
        std::strcpy(record + 0xC29, source.memo.data());
    }
    std::strcpy(record + 0x0C, source.name.data());
    for (std::int32_t line = 0; line < 32; ++line) {
        std::strcpy(record + 0x29 + line * 96, source.description[line].data());
    }
    WriteTradeValue(record, 0xC3C, source.pages);
    if ((flags & 0x60u) != 0) {
        WriteTradeValue(record, 2, std::uint16_t{0xFFFF});
    }
}

void Sub_4E52B0(const std::int32_t side)
{
    for (std::int32_t index = 0; index < 5; ++index) {
        world::detail::PetRuntimeRecord& source = world::detail::gPetRuntimeF633F8[index];
        if (source.used == 0) {
            continue;
        }
        char* const record = gTradePetsC9C108.data() + (index + side * 5) * 0x5FC;
        const bool hasUserName = source.name6BD[0] != '\0';
        WriteTradeValue(record, 0, std::uint16_t{1});
        std::strcpy(record + 0x5D8,
            hasUserName ? source.name6BD.data() : source.name6AC.data());
        const std::int32_t imageNo = source.imageNo6A0;
        const std::int32_t value08 = source.value08;
        std::strcpy(record + 0x5E9, source.name6AC.data());
        WriteTradeValue(record, 4, source.value04);
        WriteTradeValue(record, 8, imageNo);
        WriteTradeValue(record, 0x0C, value08);
        for (std::int32_t value = 0; value < 3; ++value) {
            WriteTradeValue(record, 0x10 + value * 4,
                platform::legacy_crypt_int::Read_401EC0(&source.values0C[value]));
        }
        const std::int32_t fourth =
            platform::legacy_crypt_int::Read_401EC0(&source.values0C[3]);
        std::int32_t first = source.values70[5];
        std::int32_t second = source.values70[6];
        WriteTradeValue(record, 0x1C, fourth);
        std::int32_t third = source.values70[7];
        WriteTradeValue(record, 0x38, first);
        first = source.values70[8];
        WriteTradeValue(record, 0x3C, second);
        second = source.values70[9];
        WriteTradeValue(record, 0x40, third);
        third = source.values70[0];
        WriteTradeValue(record, 0x44, first);
        first = source.values70[1];
        WriteTradeValue(record, 0x48, second);
        second = source.values70[2];
        WriteTradeValue(record, 0x20, third);
        third = source.values70[3];
        WriteTradeValue(record, 0x24, first);
        first = source.values70[4];
        WriteTradeValue(record, 0x28, second);
        second = source.values70[10];
        WriteTradeValue(record, 0x2C, third);
        third = source.value6A4;
        WriteTradeValue(record, 0x30, first);
        WriteTradeValue(record, 0x34, second);
        WriteTradeValue(record, 0x5C, third);
        for (std::int32_t value = 0; value < 4; ++value) {
            WriteTradeValue(record, 0x4C + value * 4, source.values70[21 + value]);
        }
        for (std::int32_t skill = 0; skill < 10; ++skill) {
            const std::int16_t sourceIndex =
                std::bit_cast<std::int16_t>(source.techniquePairs650[skill].index);
            std::memcpy(
                record + 0x60 + skill * 0x8C,
                &source.techniquesD8[sourceIndex], sizeof(world::detail::PetTechniqueRecord));
        }
    }
}

std::int32_t Sub_4E5710(const std::int32_t side)
{
    if (side == 0) {
        gTradeValueCF4688 = 0;
    } else {
        gTradeValueD046D0 = 0;
    }
    std::memset(gTradeItemsD460D0.data() + side * 0xF5F0, 0, 0xF5F0);
    std::memset(gTradePetsC9C108.data() + side * 0x1DEC, 0, 0x1DEC);
    return 0;
}

void Sub_4E5770(
    const std::int32_t index, const std::int32_t side, const char* const text)
{
    char* const record = gTradeItemsD460D0.data() + (index + side * 20) * 0xC4C;
    char* const rows = record + 0x2D;
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char* const row = rows + line * 96;
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            last = line;
        } else {
            row[0] = '\0';
        }
    }
    // 原回扫从最后成功行的前一行开始，不检查最后成功行本身。
    while (last > 0 && std::strlen(rows + (last - 1) * 96) == 0) {
        --last;
    }
    WriteTradeValue(record, 0xC30, last < 0 ? std::int32_t{0} : last / 8 + 1);
}

std::int32_t Sub_4E5830(const char* const text)
{
    char name[256];
    char memo[256];
    char description[768];
    std::int32_t field = 1;
    std::int32_t index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
    std::int32_t status =
        network::ExtractDelimitedField_52CED0(text, '|', field + 1, 255, name);
    while (status != 1) {
        char* const record = gTradeItemsD460D0.data() + index * 0xC4C;
        std::memset(record, 0, 0xC4C);
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        WriteTradeValue(record, 0, std::uint16_t{1});
        if (std::strlen(name) <= 28) {
            std::strcpy(record + 0x10, name);
        }
        network::ExtractDelimitedField_52CED0(text, '|', field + 3, 767, description);
        input::UnescapeLegacyTextAndNotify_52D2F0(description);
        Sub_4E5770(index, 0, description);
        network::ExtractDelimitedField_52CED0(text, '|', field + 13, 255, memo);
        input::UnescapeLegacyTextAndNotify_52D2F0(memo);
        if (std::strlen(memo) <= 16) {
            std::strcpy(record + 0xC38, memo);
        }
        WriteTradeValue(record, 4,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 4));
        WriteTradeValue(record, 0x0C,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 10));
        WriteTradeValue(record, 8,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 12));
        WriteTradeValue(record, 0xC34,
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 9));
        field += 14;
        index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        status = network::ExtractDelimitedField_52CED0(text, '|', field + 1, 255, name);
    }
    return status;
}

std::int32_t Sub_4E5A40(const std::int32_t index, const char* const text)
{
    char* const record = gTradePetsC9C108.data() + index * 0x5FC;
    std::memset(record, 0, 0x5FC);
    char name[256];
    std::int32_t result =
        network::ExtractDelimitedField_52CED0(text, '|', 1, 255, name);
    if (result != 0) {
        return result;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    std::strcpy(record + 0x5D8, std::strlen(name) <= 16 ? name : "???");
    result = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
    WriteTradeValue(record, 0x0C, result);
    if (result < 0) {
        return result;
    }
    WriteTradeValue(record, 0, std::uint16_t{1});
    WriteTradeValue(record, 4, static_cast<std::uint16_t>(
        network::ParseDelimitedInteger_52CFA0(text, '|', 3)));
    WriteTradeValue(record, 0x14, network::ParseDelimitedInteger_52CFA0(text, '|', 4));
    for (std::int32_t field = 5; field <= 11; ++field) {
        WriteTradeValue(record, 0x1C + (field - 5) * 4,
            network::ParseDelimitedInteger_52CFA0(text, '|', field));
    }
    WriteTradeValue(record, 0x5C, network::ParseDelimitedInteger_52CFA0(text, '|', 12));
    network::ExtractDelimitedField_52CED0(text, '|', 13, 255, name);
    input::UnescapeLegacyTextAndNotify_52D2F0(name);
    std::strcpy(record + 0x5E9, std::strlen(name) <= 16 ? name : "???");
    WriteTradeValue(record, 8, network::ParseDelimitedInteger_52CFA0(text, '|', 14));
    for (std::int32_t field = 15; field <= 23; ++field) {
        result = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        WriteTradeValue(record, 0x38 + (field - 15) * 4, result);
    }
    return result;
}

std::int32_t Sub_4E5CD0(const std::int32_t index, const char* const text)
{
    if (index < 0 || index >= 5) {
        return index;
    }
    char* const record = gTradePetsC9C108.data() + index * 0x5FC;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return index;
    }
    char* skill = record + 0x60;
    std::memset(skill, 0, 0x8C);
    char name[256];
    for (std::int32_t field = 1; field <= 10; ++field) {
        const std::int32_t status =
            network::ExtractDelimitedField_52CED0(text, '|', field, 255, name);
        if (status == 1) {
            return status;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        const std::size_t length = std::strlen(name);
        WriteTradeValue(skill, 0, std::uint16_t{1});
        if (length <= 24) {
            std::strcpy(skill + 6, name);
        }
        skill += 0x8C;
    }
    return 10;
}

std::int32_t Sub_503CB0()
{
    const std::int32_t decreased = platform::gValue73ECBC - 1;
    platform::gValue73ECBC = decreased;
    if (decreased < 0) {
        platform::gValue73ECBC = 2;
    }
    return decreased;
}

std::int32_t Sub_503CD0()
{
    const auto mode = static_cast<std::int16_t>(platform::gValue5F974C + 1);
    platform::gValue5F974C = mode;
    if (mode >= 7) {
        platform::gValue5F974C = 0;
    }
    static_cast<void>(login::detail::Sub_520590());
    return login::detail::Sub_520200();
}

std::int32_t Sub_4C9240()
{
    for (std::size_t index = 0; index < kWindowCount; ++index) {
        const WindowPrototype* const prototype = gWindowPrototypes57C130[index];
        gWindowStates[index].x = prototype->defaultX;
        gWindowStates[index].y = prototype->defaultY;
    }
    gValueCF4AAC = 1;
    gValue57DA28 = 0xFF;
    gValue57E42C = 0xFF;
    gChatWidthUnitsC9A930 = 12;
    gChatHeightUnitsC9A938 = 4;
    gValueCF4690 = 0;
    gValueCA0440 = 0;
    gWindowStates[33].marker50 = 0;
    gWindowStates[34].marker50 = 0;
    gValueD67B38 = 0;
    return 0;
}

std::int32_t Sub_4C93A0()
{
    if (gWindowSavedDataD40B30.records[1].x != gWindowPrototype54E2A0.defaultX) {
        return 1;
    }
    if (gWindowSavedDataD40B30.records[4].x != gWindowPrototype54E240.defaultX) {
        return 1;
    }
    return gWindowSavedDataD40B30.records[6].x != gWindowPrototype54E330.defaultX ? 1 : 0;
}

std::int32_t Sub_4C93E0()
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

std::int32_t Sub_4C9170()
{
    for (std::size_t index = 0; index < kWindowCount; ++index) {
        gWindowStates[index].x = gWindowSavedDataD40B30.records[index].x;
        gWindowStates[index].y = gWindowSavedDataD40B30.records[index].y;
    }
    static_cast<void>(Sub_4C93E0());
    const auto firstByte = static_cast<std::uint8_t>(gWindowSavedDataD40B30.records[14].value08);
    const std::int32_t valueCF4AAC = gWindowSavedDataD40B30.records[24].value08;
    const auto secondByte = static_cast<std::uint8_t>(gWindowSavedDataD40B30.records[14].value0C);
    gValue57DA28 = firstByte;
    const std::int32_t width = gWindowSavedDataD40B30.records[35].value08;
    gValueCF4AAC = valueCF4AAC;
    const std::int32_t mode = gWindowSavedDataD40B30.records[35].value10;
    gChatWidthUnitsC9A930 = width;
    const std::int32_t valueCF4690 = gWindowSavedDataD40B30.records[1].value08;
    gValue57E42C = secondByte;
    const std::int32_t height = gWindowSavedDataD40B30.records[35].value0C;
    gChatWindowModeD66E04 = mode;
    const std::int32_t actorBars = gWindowSavedDataD40B30.records[1].value0C;
    gValueCF4690 = valueCF4690;
    gChatHeightUnitsC9A938 = height;
    const std::int32_t valueCA0440 = gWindowSavedDataD40B30.records[4].value08;
    world::detail::gActorBarsEnabled5AC490 = actorBars == 1 ? 1u : 0u;
    const std::int32_t marker33 = gWindowSavedDataD40B30.records[33].value08;
    gValueCA0440 = valueCA0440;
    const std::int32_t marker34 = gWindowSavedDataD40B30.records[34].value08;
    gWindowStates[33].marker50 = static_cast<std::uint32_t>(marker33);
    const std::int32_t valueD67B38 = gWindowSavedDataD40B30.records[21].value08;
    gWindowStates[34].marker50 = static_cast<std::uint32_t>(marker34);
    gValueD67B38 = valueD67B38;
    return 1;
}

std::int32_t Sub_4C9060()
{
    static_cast<void>(Sub_4C9240());
    std::FILE* const stream = std::fopen("data\\WinSaveDate", "rb+");
    if (stream == nullptr) {
        static_cast<void>(Sub_4C9240());
        return 0;
    }
    const int descriptor = ::_open(
        "data\\WinSaveDate", _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE);
    if (descriptor != -1) {
        gWindowSavedLengthCC4238 = static_cast<std::int32_t>(::_filelength(descriptor));
        static_cast<void>(::_close(descriptor));
        if (std::ferror(stream) == 0) {
            static_cast<void>(std::fread(&gWindowSavedDataD40B30, 1, 0x521C, stream));
            if (std::ferror(stream) == 0) {
                if (gWindowSavedDataD40B30.version != 0x13
                    || Sub_4C93A0() == 1 || Sub_4C9170() == 0) {
                    static_cast<void>(Sub_4C9240());
                }
                static_cast<void>(std::fclose(stream));
                return 1;
            }
        }
    }
    static_cast<void>(Sub_4C9240());
    static_cast<void>(std::fclose(stream));
    return 0;
}

std::int32_t Sub_4C92B0()
{
    std::memset(&gWindowSavedDataD40B30, 0, sizeof(gWindowSavedDataD40B30));
    return 0;
}

std::uint32_t Sub_4C92D0()
{
    static_cast<void>(Sub_4C92B0());
    gWindowSavedDataD40B30.version = 0x13;
    for (std::size_t index = 0; index < kWindowCount; ++index) {
        gWindowSavedDataD40B30.records[index].x = gWindowStates[index].x;
        gWindowSavedDataD40B30.records[index].y = gWindowStates[index].y;
    }
    const std::int32_t firstByte = std::bit_cast<std::int8_t>(gValue57DA28);
    const std::int32_t secondByte = std::bit_cast<std::int8_t>(gValue57E42C);
    const std::int32_t valueCF4AAC = gValueCF4AAC;
    gWindowSavedDataD40B30.records[14].value08 = firstByte;
    const std::int32_t width = gChatWidthUnitsC9A930;
    gWindowSavedDataD40B30.records[14].value0C = secondByte;
    const std::int32_t height = gChatHeightUnitsC9A938;
    gWindowSavedDataD40B30.records[35].value08 = width;
    const std::int32_t valueCF4690 = gValueCF4690;
    gWindowSavedDataD40B30.records[24].value08 = valueCF4AAC;
    const std::int32_t mode = gChatWindowModeD66E04;
    gWindowSavedDataD40B30.records[1].value08 = valueCF4690;
    const std::uint32_t actorBars = world::detail::gActorBarsEnabled5AC490;
    gWindowSavedDataD40B30.records[35].value0C = height;
    const std::uint32_t marker33 = gWindowStates[33].marker50;
    gWindowSavedDataD40B30.records[35].value10 = mode;
    const std::int32_t valueCA0440 = gValueCA0440;
    gWindowSavedDataD40B30.records[33].value08 = std::bit_cast<std::int32_t>(marker33);
    gWindowSavedDataD40B30.records[1].value0C = actorBars == 1 ? 1 : 0;
    const std::uint32_t marker34 = gWindowStates[34].marker50;
    gWindowSavedDataD40B30.records[4].value08 = valueCA0440;
    const std::int32_t valueD67B38 = gValueD67B38;
    gWindowSavedDataD40B30.records[34].value08 = std::bit_cast<std::int32_t>(marker34);
    gWindowSavedDataD40B30.records[21].value08 = valueD67B38;
    return marker33;
}

std::int32_t Sub_4C9130()
{
    std::FILE* const stream = std::fopen("data\\WinSaveDate", "wb+");
    if (stream != nullptr) {
        static_cast<void>(Sub_4C92D0());
        static_cast<void>(std::fwrite(&gWindowSavedDataD40B30, 1, 0x521C, stream));
        static_cast<void>(std::fclose(stream));
    }
    return 1;
}

void Sub_472200()
{
    for (WindowState& state : gWindowStates) {
        state.marker50 = 0;
    }
}

std::int32_t InitializeWindowSystem_4705D0()
{
    static_cast<void>(Sub_4A6D80());
    static_cast<void>(Sub_4C9060());
    gValueCF4B6C = 0;
    Sub_472200();
    return world::detail::Sub_534910();
}

std::int32_t Sub_4923D0()
{
    gLoginWindowRuntime.valueCF4A00 = 5;
    gLoginWindowRuntime.valueC8A95C = 0;
    gLoginWindowRuntime.valueCB2200 = 0;
    return 0;
}

void Sub_4B5A90()
{
    gLoginWindowRuntime.valueD3FAD0 = 0;
    for (WindowListRecordD3FAD4& record : gLoginWindowRuntime.recordsD3FAD4) {
        record.value00 = 0;
        record.value408 = 0;
        record.texts08[1][0] = '\0';
        record.texts08[2][0] = '\0';
        record.texts08[3][0] = '\0';
        record.texts08[0][0] = '\0';
        record.value04 = 0;
    }
}

std::int32_t Sub_4A5390()
{
    WindowChoiceSlotsCC4CA0& choices = gLoginWindowRuntime.choicesCC4CA0;
    choices.value00 = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        choices.values04[index] = 0;
        choices.pairs18[index][1] = 0;
        choices.pairs18[index][0] = 0;
    }
    gLoginWindowRuntime.prefixD2F0B8.value26C = 0;
    gLoginWindowRuntime.prefixD2F0B8.value08 = -1;
    gLoginWindowRuntime.prefixD2F0B8.value0C = -1;
    return -1;
}

std::int32_t Sub_48C150()
{
    gLoginWindowRuntime.valueD673D0 = 5;
    gLoginWindowRuntime.selectedPetCC3E5C = 0;
    for (std::int32_t& value : gLoginWindowRuntime.valuesCC1A18) {
        value = 0;
    }
    gLoginWindowRuntime.valuesCC4CE8[0] = 0;
    gLoginWindowRuntime.valuesCC4CE8[2] = 0;
    gLoginWindowRuntime.valuesCC4CE8[3] = 0;
    gLoginWindowRuntime.valuesCC4CE8[4] = 0;
    gLoginWindowRuntime.valuesCC4CE8[1] = 0;
    gLoginWindowRuntime.valuesD2EAA0[0] = 0;
    gLoginWindowRuntime.valuesD2EAA0[1] = -1;
    gLoginWindowRuntime.valuesD2EAA0[2] = -1;
    return -1;
}

std::int32_t Sub_4742B0()
{
    gLoginWindowRuntime.valuesCF4E50[0] = 0;
    gLoginWindowRuntime.valuesCF4E50[1] = 0;
    return 0;
}

std::int32_t Sub_4A0940()
{
    gLoginWindowRuntime.valuesD67B40[0] = 0;
    gLoginWindowRuntime.valuesD67B40[3] = 0;
    gLoginWindowRuntime.valuesD67B40[1] = 0;
    gLoginWindowRuntime.valuesD67B40[4] = 0;
    gLoginWindowRuntime.valuesD67B40[6] = 0;
    gLoginWindowRuntime.valuesD67B40[2] = 0;
    gLoginWindowRuntime.valuesD67B40[5] = 0;
    return 0;
}

std::int32_t Sub_4CC6A0()
{
    gLoginWindowRuntime.valueCC31A4 = 5;
    gLoginWindowRuntime.valueD2E444 = 0;
    gLoginWindowRuntime.valueCB0604 = 0;
    return 0;
}

void Sub_4CBD30()
{
    platform::LegacyTm53B8C6* const previous = gLoginWindowRuntime.calendarTimeD67BAC;
    gLoginWindowRuntime.valueD67BB0 = 0;
    if (previous != nullptr) {
        return;
    }
    platform::ReadLegacyTime32_53BA26(&gLoginWindowRuntime.timeCB1DF0);
    platform::LegacyTm53B8C6* const current =
        platform::LocalTime_53B8C6(&gLoginWindowRuntime.timeCB1DF0);
    gLoginWindowRuntime.calendarTimeD67BAC = current;
    const auto* const bytes = reinterpret_cast<const std::byte*>(current);
    std::int32_t day;
    std::memcpy(&day, bytes + 0x0C, sizeof(day));
    gLoginWindowRuntime.dayD67BC0 = day;
    std::int32_t month;
    std::memcpy(&month, bytes + 0x10, sizeof(month));
    std::int32_t year;
    std::memcpy(&year, bytes + 0x14, sizeof(year));
    std::sprintf(world::detail::gLogFileName554648, "chat_%02d%02d%02d.txt",
        year % 100, month + 1, day);
}

void InitializeLoginWindows_470600()
{
    static_cast<void>(Sub_4C9060());
    static_cast<void>(Sub_4923D0());
    Sub_4B5A90();
    static_cast<void>(Sub_4A6DB0());
    static_cast<void>(Sub_4A5390());
    static_cast<void>(Sub_48C150());
    static_cast<void>(Sub_4742B0());
    world::detail::Sub_4C9490();
    world::detail::Sub_533910();
    if (world::detail::Sub_5347F0() == 0) {
        world::detail::Sub_533950();
    }
    static_cast<void>(Sub_4A0940());
    gWindowStates[21].marker50 = 0;
    gWindowStates[22].marker50 = 0;
    gWindowStates[23].marker50 = 0;
    gWindowStates[40].marker50 = 0;
    static_cast<void>(Sub_4CC6A0());
    Sub_4CBD30();
    gLoginWindowRuntime.valueD67618 = 0;
}

std::int32_t Sub_4A6D80()
{
    platform::gValue554640 = 1;
    gChatScrollOffsetC9A940 = 0;
    world::detail::gCountC9A944 = 0;
    gChatWidthRequestC9A934 = 0;
    gChatHeightRequestC9A93C = 0;
    input::gInputContext626AA0.field140 = 0;
    return 0;
}

std::int32_t Sub_4A6DB0()
{
    gChatScrollOffsetC9A940 = 0;
    gChatWidthRequestC9A934 = 0;
    world::detail::gCountC9A944 = 0;
    gChatHeightRequestC9A93C = 0;
    return 0;
}

std::int32_t Sub_4A7700()
{
    world::detail::gCountC9A944 = 0;
    gChatScrollOffsetC9A940 = 0;
    return 0;
}

std::int32_t Sub_44B170()
{
    for (auto& record : world::detail::gRecords5FB7F8) {
        record.text[0] = '\0';
    }
    return Sub_4A7700();
}

void Sub_4A7710()
{
    gChatHeightRequestC9A93C = 1;
}

void Sub_4A7720()
{
    gChatHeightRequestC9A93C = -1;
}

void Sub_4A7730()
{
    gChatWidthRequestC9A934 = 1;
}

void Sub_4A7740()
{
    gChatWidthRequestC9A934 = -1;
}

std::int32_t Sub_4A82E0()
{
    const std::int32_t height = gChatHeightUnitsC9A938 + gChatHeightRequestC9A93C;
    gChatHeightUnitsC9A938 = height;
    if (height > 26) {
        gChatHeightUnitsC9A938 = 4;
    } else if (height < 4) {
        gChatHeightUnitsC9A938 = 26;
    }
    gChatHeightRequestC9A93C = 0;
    gChatWidthRequestC9A934 = 0;
    return 0;
}

void Sub_4A7590(input::InputContext* const context)
{
    const std::int32_t font = input::gCurrentInputData->field13C;
    static_cast<void>(login::detail::TextMeasure(platform::gImeComposition, font));
    const std::int32_t width = render::gFontMetrics[9].doubleWidth * gChatWidthUnitsC9A930;
    ScrollInputAndDraw(context, width);
}

void Sub_4A7640(input::InputContext* const context)
{
    const std::int32_t font = input::gCurrentInputData->field13C;
    static_cast<void>(login::detail::TextMeasure(platform::gImeComposition, font));
    ScrollInputAndDraw(context, 1024);
}

void Sub_4A7750(const std::int32_t x, const std::int32_t y)
{
    std::int32_t totalHeight = 0;
    const std::int32_t width = gChatWindowModeD66E04 == 1
        ? render::gFontMetrics[9].doubleWidth * 44
        : (gChatWidthUnitsC9A930 + 7) * render::gFontMetrics[9].doubleWidth;
    std::int32_t logIndex = world::detail::gWriteIndex626C08;
    logIndex -= gChatScrollOffsetC9A940;
    --logIndex;
    if (logIndex < 0) {
        logIndex = 511;
    }
    for (auto& record : world::detail::gRecords5FB7F8) {
        if (record.fontType > 14 || PreviousChatFont(logIndex) < 0) {
            record.fontType = 0;
        }
    }
    std::int32_t drawY = y - render::gFontMetrics[
        world::detail::gRecords5FB7F8[logIndex].fontType].doubleHeight;
    std::int32_t processedRecords = 0;
    char scratch[0xC70];
    char* const rows = scratch + 0x124;
    static_assert(0x124 + 10 * 0x121 + 2 == sizeof(scratch));
    for (;;) {
        auto& source = world::detail::gRecords5FB7F8[logIndex];
        if (source.text[0] == '\0') {
            return;
        }
        std::strcpy(scratch, source.text);
        const auto textLength = static_cast<std::int32_t>(std::strlen(scratch));
        std::int32_t* fontSlot = &source.fontType;
        std::int32_t lineCount = login::detail::TextMeasure(scratch, *fontSlot) / width;
        if (login::detail::TextMeasure(scratch, *fontSlot) % width != 0) {
            ++lineCount;
        }

        std::int32_t sourceOffset = 0;
        for (std::int32_t lineIndex = 0; lineIndex < lineCount; ++lineIndex) {
            std::int32_t column = 0;
            if (textLength > 0) {
                std::int32_t rowOffset = lineIndex * 0x121;
                for (;;) {
                    char* const outputByte = rows + rowOffset + column;
                    *outputByte = scratch[sourceOffset];
                    outputByte[1] = '\0';
                    outputByte[2] = '\0';
                    if (logIndex > 512) {
                        logIndex = world::detail::gWriteIndex626C08;
                        fontSlot = &world::detail::gRecords5FB7F8[logIndex].fontType;
                    }
                    char* const currentRow = rows + rowOffset;
                    const std::int32_t rowWidth =
                        login::detail::TextMeasure(currentRow, *fontSlot);
                    if (width < rowWidth) {
                        if (input::ClassifyLastAnsiUnit_44B190(currentRow) == 3) {
                            *outputByte = '\0';
                            --sourceOffset;
                        }
                        rowOffset += 0x121;
                        ++lineIndex;
                        column = -1;
                    }
                    ++sourceOffset;
                    if (sourceOffset > textLength) {
                        break;
                    }
                    ++column;
                    if (column >= textLength) {
                        break;
                    }
                }
            }
            if (sourceOffset > textLength) {
                break;
            }
        }

        if (lineCount > 0) {
            auto& record = world::detail::gRecords5FB7F8[logIndex];
            const char* row = rows + (lineCount - 1) * 0x121;
            for (std::int32_t line = 0; line < lineCount; ++line) {
                const std::int32_t font = record.fontType;
                totalHeight += render::gFontMetrics[font].doubleHeight;
                const std::int32_t heightLimit =
                    render::gFontMetrics[9].doubleHeight * gChatHeightUnitsC9A938;
                if (totalHeight > heightLimit) {
                    return;
                }
                if (*row != '\0') {
                    const std::int32_t color = record.color;
                    static_cast<void>(render::DrawText2_456670(
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                        3,
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(font)),
                        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(color)),
                        row, 0));
                }

                std::int32_t spacingFont;
                if (line + 1 == lineCount) {
                    const std::int32_t previous = PreviousChatFont(logIndex);
                    if (previous > 14 || previous < 0) {
                        PreviousChatFont(logIndex) = 0;
                    }
                    spacingFont = PreviousChatFont(logIndex);
                } else {
                    if (record.fontType > 14 || PreviousChatFont(logIndex) < 0) {
                        record.fontType = 0;
                    }
                    spacingFont = record.fontType;
                }
                row -= 0x121;
                drawY -= render::gFontMetrics[spacingFont].doubleHeight;
            }
        }

        --logIndex;
        ++processedRecords;
        if (logIndex < 0) {
            logIndex = 511;
        }
        if (processedRecords >= world::detail::gCountC9A944) {
            return;
        }
    }
}

void Sub_4771B0(
    const std::int32_t x, const std::int32_t y, const char* const text,
    const std::int8_t style, const std::int8_t color,
    const std::uint8_t flags, const std::uint8_t layer)
{
    render::GraphicDescriptor16 descriptor {};
    descriptor.bytes[8] = 0xFFu;
    descriptor.bytes[9] = 0xFFu;
    descriptor.bytes[10] = 0xFFu;
    descriptor.bytes[11] = 0xFFu;
    descriptor.bytes[13] = 4;

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

    const auto drawByte = [&](const char character) {
        std::int32_t imageNo;
        if (character >= '0' && character <= '9') {
            imageNo = kNumberDigitImages54F0D8[style * 13 + color] + character - '0';
        } else {
            std::int32_t symbol;
            switch (character) {
            case '+': symbol = 0; break;
            case '-': symbol = 1; break;
            case ':': symbol = 2; break;
            case '/': symbol = 3; break;
            default: return;
            }
            imageNo = kNumberSymbolImages54F1A8[style * 13 + color] + symbol;
        }
        static_cast<void>(login::detail::DrawGraphic(
            drawX, drawY, layer, imageNo, 0, &descriptor));
    };

    const char* cursor = text;
    if (reverse) {
        while (*cursor != '\0') {
            ++cursor;
        }
        while (cursor != text) {
            --cursor;
            drawByte(*cursor);
            drawX += step;
        }
    } else {
        while (*cursor != '\0') {
            drawByte(*cursor);
            drawX += step;
            ++cursor;
        }
    }
}

std::int32_t Sub_477460(const char* const text, const std::int8_t style)
{
    const auto length = static_cast<std::int32_t>(std::strlen(text));
    if (length <= 0) {
        return 0;
    }
    return gNumberAdvancesD67AC4[style] * length - kNumberSpacingDeductions54F2A8[style];
}

std::int32_t Sub_46FBC0(const std::int32_t)
{
    DrawCurrentWindow_4729B0();
    return 1;
}

std::int32_t Sub_49E750(const std::int32_t value)
{
    if (value == -1) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[0].data);
        const char* const text = world::ReadText9DAD30();
        std::memcpy(data->text, text, std::strlen(text) + 1);
        const std::int32_t textWidth = login::detail::TextMeasure(text, 0);
        WindowInstance* const window = gCurrentWindow;
        window->elements[0].x = (window->width - textWidth) >> 1;
    } else {
        const std::uint32_t tick = platform::ReadTickCount32();
        if (world::detail::gValue62B5B0 + 2000u <= tick) {
            gCurrentWindow->state |= 0x8000u;
            world::detail::gValue62B5B0 = 0;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
    return 1;
}

std::int32_t Sub_49BDB0(const std::int32_t)
{
    DrawCurrentWindow_4729B0();
    WindowInstance* const window = gCurrentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    login::detail::DrawNineGrid_49BE00(
        0x3BE3E, x, y, width, height, 0xDF, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    render::ClearTextQueueTail_456D50(3);
    return 1;
}

std::int32_t Sub_472F00(
    const std::int32_t left, const std::int32_t top,
    const std::int32_t width, const std::int32_t height)
{
    const std::int32_t x = platform::legacy_crypt_int::Subtract_401E60(
        &platform::gLegacyMouseState.mouseX, left);
    if (x < 0 || x >= width) {
        return 0;
    }
    const std::int32_t y = platform::legacy_crypt_int::Subtract_401E60(
        &platform::gLegacyMouseState.mouseY, top);
    return y >= 0 && y < height ? 1 : 0;
}

std::int32_t Sub_4763E0()
{
    gFocusElementIndex57C124 = -1;
    gMouseValue57C110 = 0;
    gFocusWindowId57C120 = 0;
    gDragCallback57C118 = nullptr;
    gMousePhase57C128 = 0;
    return 0;
}

std::int32_t Sub_472D20()
{
    const std::int32_t initialPhase = gMousePhase57C128;
    WindowInstance* window = gCurrentWindow;
    std::int32_t result = 0;
    if (initialPhase != 1 && (window->state & 2u) == 0) {
        const std::uint32_t state = window->state;
        if ((state & 0x800u) != 0) {
            if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.heldButtons) & 1u) != 0) {
                std::int32_t* const x = &window->x;
                const std::int32_t deltaX = platform::legacy_crypt_int::Subtract_401E60(
                    &platform::gLegacyMouseState.mouseX, window->dragMouseX1C);
                *x += deltaX;
                window = gCurrentWindow;
                std::int32_t* const y = &window->y;
                const std::int32_t deltaY = platform::legacy_crypt_int::Subtract_401E60(
                    &platform::gLegacyMouseState.mouseY, window->dragMouseY20);
                *y += deltaY;
                const std::int32_t mouseX = platform::legacy_crypt_int::Read_401EC0(
                    &platform::gLegacyMouseState.mouseX);
                gCurrentWindow->dragMouseX1C = mouseX;
                const std::int32_t mouseY = platform::legacy_crypt_int::Read_401EC0(
                    &platform::gLegacyMouseState.mouseY);
                gCurrentWindow->dragMouseY20 = mouseY;
            } else {
                window->state = state & ~std::uint32_t{0x800};
            }
            result = 2;
            window = gCurrentWindow;
        } else if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
            for (std::int32_t index = 0; index < window->secondaryCount; ++index) {
                const WindowDragRect& rect = window->dragRects40[index];
                const std::int32_t left = window->x + rect.x;
                const std::int32_t top = window->y + rect.y;
                const std::int32_t height = rect.height;
                const std::int32_t width = rect.width;
                if (gWindowHasFocusCA0AB4 == 0) {
                    if (Sub_472F00(left, top, width, height) != 0) {
                        const std::int32_t mouseX = platform::legacy_crypt_int::Read_401EC0(
                            &platform::gLegacyMouseState.mouseX);
                        gCurrentWindow->dragMouseX1C = mouseX;
                        const std::int32_t mouseY = platform::legacy_crypt_int::Read_401EC0(
                            &platform::gLegacyMouseState.mouseY);
                        gCurrentWindow->dragMouseY20 = mouseY;
                        gCurrentWindow->state |= 0x800u;
                        result |= 2;
                    }
                    window = gCurrentWindow;
                }
            }
        }
    }
    if ((window->state & 1u) == 0) {
        const std::int32_t phase = gMousePhase57C128;
        if (phase == 1 || phase == 4) {
            if (Sub_472F00(window->x, window->y, window->width, window->height) != 0) {
                result |= 1;
            }
        } else if (gWindowHasFocusCA0AB4 == 0) {
            if (Sub_472F00(window->x, window->y, window->width, window->height) != 0) {
                result |= 1;
                if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
                    gCurrentWindow->state |= 0x2000u;
                }
            }
        }
    }
    return result;
}

std::int32_t Sub_4730A0()
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
        if ((window->state & 0x8000u) != 0) {
            FreeWindow_471270(window);
            for (std::size_t target = index; target + 1 < kWindowCount; ++target) {
                gWindows[target] = gWindows[target + 1];
            }
            gWindows[kWindowCount - 1] = nullptr;
            if (index + 1 != kWindowCount) {
                continue;
            }
        } else {
            if ((window->state & 0x2000u) != 0) {
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

void Sub_473E40()
{
    std::int32_t phase = gMousePhase57C128;
    if (phase == 1) {
        const WindowDragCallback callback = gDragCallback57C118;
        if (callback == nullptr) {
            gWindowHasFocusCA0AB4 = 1;
            *reinterpret_cast<unsigned char*>(&platform::gLegacyMouseState.valueD68DB4) = 5;
            return;
        }
        const std::int32_t events = AppendMouseButtonEvents(0);
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

void Sub_473560(const std::int32_t index)
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
            if (Sub_472F00(left, top, width, height) != 0) {
                phase = gMousePhase57C128;
                if (phase == 1 || phase == 4) {
                    events = 0x20;
                }
                if (gFocusWindowId57C120 == gCurrentWindowIdD67A1C
                    && gFocusElementIndex57C124 == gCurrentElementIndex57C12C) {
                    events = AppendMouseButtonEvents(events);
                }
            } else {
                phase = gMousePhase57C128;
            }
        }
    } else if (gWindowHasFocusCA0AB4 == 0) {
        if (Sub_472F00(left, top, width, height) != 0) {
            events = AppendMouseButtonEvents(1);
            WindowInstance* const currentWindow = gCurrentWindow;
            WindowElement* const currentElement = &currentWindow->elements[index];
            if (currentElement->type == 4) {
                auto* const data = static_cast<WindowMouseData*>(currentElement->data);
                data->deltaX = platform::legacy_crypt_int::Subtract_401E60(
                    &platform::gLegacyMouseState.mouseX,
                    currentWindow->x + currentElement->x);
                WindowInstance* const yWindow = gCurrentWindow;
                data->deltaY = platform::legacy_crypt_int::Subtract_401E60(
                    &platform::gLegacyMouseState.mouseY,
                    yWindow->y + yWindow->elements[index].y);
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
            static_cast<void>(Sub_4763E0());
        }
    }
}

std::int32_t Sub_4741A0(const std::int32_t index, const std::int32_t state)
{
    auto* const data = static_cast<WindowMouseData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((data->flags & 2u) == 0) {
        data->flags = 0;
    }
    if ((state & 8) != 0) {
        data->flags = 3;
        result = 1;
    }
    if ((data->flags & 2u) != 0) {
        WindowInstance* const window = gCurrentWindow;
        WindowElement* const previous = &window->elements[index - 1];
        const std::int32_t y = platform::legacy_crypt_int::Subtract_401E60(
            &platform::gLegacyMouseState.mouseY, window->y);
        previous->y = y - (previous->height >> 1);
    }
    WindowElement* element = &gCurrentWindow->elements[index];
    if (element[-1].y < element->y) {
        element[-1].y = element->y;
    }
    element = &gCurrentWindow->elements[index];
    const std::int32_t bottom = element->y + element->height;
    const std::int32_t previousHeight = element[-1].height;
    if (element[-1].y + previousHeight > bottom) {
        element[-1].y = bottom - previousHeight;
    }
    element = &gCurrentWindow->elements[index];
    data->deltaY = element[-1].y - element->y;
    if ((state & 8) == 0) {
        data->flags &= ~std::uint32_t{2};
    }
    return result;
}

std::uint8_t Sub_476570(
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
        const char* const text = prototype->text.data();
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
        input::Sub_45D720(context, inputX, inputY, maxCharPerLine, color, renderMode);
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
        input::Sub_45D760(context, inputX, inputY, color, renderMode, fontType);
        break;
    }
    default:
        break;
    }
    return 1;
}

void Sub_4740F0(char* const destination)
{
    std::int32_t count = 0;
    for (std::size_t position = 0; position < 10; ++position) {
        const auto& pair = world::detail::gSkillPairsF63370[position];
        if (pair.used != 0) {
            const std::int16_t index = std::bit_cast<std::int16_t>(pair.index);
            const auto& skill = world::detail::gSkillsF1BAFC[index];
            std::int16_t requiredSlots;
            std::memcpy(&requiredSlots, skill.requiredSlots19.data(), sizeof(requiredSlots));
            count += requiredSlots;
        }
    }
    char text[256];
    std::sprintf(text, "%2d/10", count);
    std::strcpy(destination, text);
}

std::int32_t Sub_4A7B00()
{
    const std::int32_t mode = gChatWindowModeD66E04;
    if (mode == 1) {
        WindowInstance* const window = gWindowStates[57].window;
        if (window == nullptr) {
            return 0;
        }
        const std::int32_t y = window->y;
        const std::int32_t x = window->x;
        static_cast<void>(Sub_476570(&gChatInputPrototype57E970, x, y, 3));
        gFocusedElementDataD45F94 = gWindowStates[57].window->elements[0].data;
        input::SetContext_44B330(&input::gInputContext626AA0);
        return 1;
    }
    if (mode != 0) {
        return 0;
    }
    WindowInstance* const window = gWindowStates[35].window;
    if (window == nullptr) {
        return 0;
    }
    const std::int32_t y = window->y;
    const std::int32_t x = window->x;
    if (Sub_476570(&gChatInputPrototype57E970, x, y, 3) == 0) {
        for (;;) {
            WindowInstance* const currentWindow = gWindowStates[35].window;
            gChatInputPrototype57E970.input.context = &input::gInputContext626AA0;
            gChatInputPrototype57E970.input.kind = WindowInputReferenceKind::Context;
            gChatInputPrototype57E970.x = 8;
            std::memcpy(
                &input::gInputContext626AA0, &world::detail::gInputContext626840,
                sizeof(input::InputContext));
            gChatInputPrototype57E970.y = 5;
            gChatInputPrototype57E970.renderMode = 3;
            gChatInputPrototype57E970.fontType = 9;
            gChatInputPrototype57E970.color = 0;
            gChatInputPrototype57E970.maxLine = 1;
            gChatInputPrototype57E970.maxCharPerLine = 76;
            gChatInputPrototype57E970.value224 = 0;
            gChatInputPrototype57E970.encrypt = 0;
            const std::int32_t currentY = currentWindow->y;
            const std::int32_t currentX = currentWindow->x;
            if (Sub_476570(&gChatInputPrototype57E970, currentX, currentY, 3) != 0) {
                break;
            }
        }
    }
    gFocusedElementDataD45F94 = gWindowStates[35].window->elements[0].data;
    input::SetContext_44B330(&input::gInputContext626AA0);
    return 1;
}

} // namespace detail

WindowInstance* OpenWindow_471440(const std::int32_t id)
{
    using namespace detail;
    const std::int8_t category = kWindowCategories[id];
    if (category == 1) {
        CloseWindowsByCategory_472240(2);
    } else if (category == 2) {
        CloseWindowsByCategory_472240(3);
    }
    for (std::int32_t index = 0; index < static_cast<std::int32_t>(kWindowCount); ++index) {
        WindowInstance* const existing = gWindows[index];
        if (existing != nullptr && existing->id == id) {
            gWindows[index]->state |= 0x2000u;
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
    window->elementCount = count;
    if (count != 0) {
        window->elements = static_cast<WindowElement*>(
            std::calloc(static_cast<std::size_t>(count), sizeof(WindowElement)));
        if (window->elements == nullptr) {
            FreeWindow_471270(window);
            return nullptr;
        }
        WindowElementPrototype* source = prototype->elements;
        for (std::int32_t index = 0; index < window->elementCount; ++index, ++source) {
            WindowElement* const element = &window->elements[index];
            element->type = source->type;
            element->state = 0;
            element->x = source->x;
            element->y = source->y;
            element->width = source->width;
            element->height = source->height;
            element->visible = source->visible;
            switch (element->type) {
            case 1: {
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
            case 2: {
                auto* const data = static_cast<WindowAnimationData*>(
                    std::calloc(1, sizeof(WindowAnimationData)));
                if (data != nullptr) {
                    data->action = login::detail::NewYobiAction_404AE0(2, 0);
                    login::detail::ConfigureWindowElementAction(
                        data->action, source->source.animationImageNo);
                    data->value04 = 0;
                }
                element->data = data;
                break;
            }
            case 3: {
                auto* const data = static_cast<WindowTextData*>(
                    std::calloc(1, sizeof(WindowTextData)));
                if (data != nullptr) {
                    const WindowTextPrototype* const original = source->source.text;
                    data->text = static_cast<char*>(std::calloc(256, 1));
                    std::strcpy(data->text, original->text);
                    char text[256];
                    Sub_4740F0(text);
                    if (std::strcmp(data->text, "10/10") == 0) {
                        std::strcpy(data->text, text);
                    }
                    data->bytes00[0] = original->bytes00[0];
                    data->bytes00[1] = original->bytes00[1];
                }
                element->data = data;
                break;
            }
            case 4: {
                auto* const data = static_cast<WindowMouseData*>(
                    std::calloc(1, sizeof(WindowMouseData)));
                if (data != nullptr) {
                    data->flags = *source->source.mouseFlags;
                }
                element->data = data;
                break;
            }
            case 5:
                element->data = std::calloc(1, sizeof(WindowInputData));
                break;
            case 6:
                element->data = std::calloc(1, sizeof(std::uint32_t));
                break;
            case 7: {
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
            element->mouseCallback = callback != nullptr ? callback : &Sub_4B0F10;
        }
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
    const std::uint32_t flags = gWindowStates[id].flags;
    WindowInstance* const previous = gCurrentWindow;
    gWindowStates[id].flags = flags | 0x80u;
    gCurrentWindow = window;
    static_cast<void>(window->update(-1));
    gWindowStates[id].window = window;
    gCurrentWindow = previous;
    return window;
}

login::YobiAction* CreateWindowAnimation_475AA0(
    const std::int32_t id, const std::uint32_t flags, const std::uint32_t mode,
    login::YobiAction** const completionSlot)
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
        if (gWindowStates[id].x + width > 640) {
            gWindowStates[id].x = platform::RandomRange_46FAF0(620 - width, 640 - width);
        }
        if (gWindowStates[id].y + height > 480) {
            gWindowStates[id].y = platform::RandomRange_46FAF0(460 - height, 480 - height);
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
    return login::detail::Sub_475950(x, y, width, height, id, flags, mode, completionSlot);
}

std::int32_t Sub_50E440(const std::uintptr_t)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    detail::gTradeValueCC4224 = 1;
    return gate;
}

std::int32_t Sub_50E470(const std::uintptr_t, const std::int32_t value)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    detail::gTradeValueCF4688 = value;
    return value;
}

std::int32_t Sub_50E4A0(const std::uintptr_t, const char* const text)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    return detail::Sub_4E5830(text);
}

std::int32_t Sub_50E4D0(
    const std::uintptr_t, const std::int32_t index, const char* const text)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    return detail::Sub_4E5A40(index, text);
}

std::int32_t Sub_50E500(
    const std::uintptr_t, const std::int32_t index, const char* const text)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    return detail::Sub_4E5CD0(index, text);
}

std::int32_t Sub_50E530(const std::uintptr_t)
{
    const std::int32_t scene = world::detail::gValue9A6E0C;
    if (scene != 0) {
        return scene;
    }
    const std::int32_t gate = world::ReadRippleGateOnce();
    if (gate != 0) {
        return gate;
    }
    detail::gTradeValueCC4224 = 0;
    static_cast<void>(detail::Sub_4E5710(0));
    const std::int32_t previous = detail::gTradeValueD2E2B0;
    if (previous == 2) {
        detail::gTradeValueD2E2B0 = 1;
    }
    return previous;
}

void Sub_50E570(const std::uintptr_t, const std::int32_t value)
{
    if (world::detail::gValue9A6E0C != 0) {
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    if (value == 1) {
        detail::gTradeValueCC4224 = 2;
    } else if (value == 0 || value == 2) {
        WindowInstance* const window = detail::gWindowStates[43].window;
        detail::gTradeValueD67B80 = 1;
        if (window != nullptr) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
        }
    }
}

void InitializeNumberSpacing_477170()
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

std::uint8_t ReadValueD67BE0() noexcept
{
    return detail::gValueD67BE0;
}

std::uint8_t ReadValue5F9754() noexcept
{
    return detail::gValue5F9754;
}

std::int32_t ReadDragItemIndex_584DA4() noexcept
{
    return detail::gDragItemIndex584DA4;
}

WindowInstance* Sub_4CD3E0()
{
    WindowInstance* const window = detail::gWindowStates[68].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

WindowInstance* Sub_4CD400()
{
    WindowInstance* const window = detail::gWindowStates[67].window;
    if (window != nullptr) {
        window->state |= 0x8000u;
    }
    return window;
}

void CancelWindowDrag_476410()
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

void FreeWindow_471270(WindowInstance* const window)
{
    using namespace detail;
    window->release();
    gCurrentWindowState = &gWindowStates[window->id];
    gCurrentWindowState->x = window->x;
    gCurrentWindowState->y = window->y;
    gCurrentWindowState->flags &= ~std::uint32_t{0x80};
    gCurrentWindowState->flags |= 1u;
    gCurrentWindowState->window = nullptr;
    if (gFocusWindowId57C120 == window->id) {
        CancelWindowDrag_476410();
    }
    for (std::int32_t index = 0; index < window->elementCount; ++index) {
        WindowElement* const element = &window->elements[index];
        switch (element->type) {
        case 1:
        case 5:
            if (element->data == gFocusedElementDataD45F94) {
                static_cast<void>(Sub_4A7B00());
            }
            if (void* const data = window->elements[index].data; data != nullptr) {
                std::free(data);
                window->elements[index].data = nullptr;
            }
            break;
        case 2:
            if (element->data != nullptr) {
                const auto* const data = static_cast<const WindowAnimationData*>(element->data);
                login::detail::MarkDeath(data->action);
                std::free(window->elements[index].data);
                window->elements[index].data = nullptr;
            }
            break;
        case 3: {
            auto* const data = static_cast<WindowTextData*>(element->data);
            if (data->text != nullptr) {
                std::free(data->text);
                static_cast<WindowTextData*>(window->elements[index].data)->text = nullptr;
            }
            if (void* const current = window->elements[index].data; current != nullptr) {
                std::free(current);
                window->elements[index].data = nullptr;
            }
            break;
        }
        case 4:
        case 6:
        case 7:
            if (element->data != nullptr) {
                std::free(element->data);
                window->elements[index].data = nullptr;
            }
            break;
        default:
            break;
        }
    }
    if (window->elements != nullptr) {
        std::free(window->elements);
    }
    window->elements = nullptr;
    std::free(window);
}

void CloseWindow_472160(WindowInstance* const window)
{
    detail::gCurrentWindowState = &detail::gWindowStates[window->id];
    detail::gCurrentWindowState->flags |= 2u;
    FreeWindow_471270(window);
}

std::int32_t CloseAllWindows_472190()
{
    using namespace detail;
    for (std::size_t index = 0; index < gWindows.size(); ++index) {
        WindowInstance* const window = gWindows[index];
        if (window != nullptr) {
            CloseWindow_472160(window);
            gWindows[index] = nullptr;
        }
    }
    world::gValueD67B08.fill(0);
    if (gValue5F9754 == 0) {
        return 0;
    }
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    const std::uintptr_t socket = platform::gConnectionSocket;
    const std::int32_t result = network::Sub_511DD0(socket, x, y, 62);
    gValue5F9754 = 0;
    return result;
}

void CloseWindowsByCategory_472240(const std::uint32_t mask)
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
            FreeWindow_471270(gWindows[index]);
            gWindows[index] = nullptr;
        }
    }
}

void DrawCurrentWindow_4729B0()
{
    using namespace detail;
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* window = gCurrentWindow;
    for (std::int32_t index = 0; index < window->elementCount; ++index) {
        WindowElement* const element = &window->elements[index];
        if (element->visible == 1) {
            const std::int32_t x = window->x + element->x;
            const std::int32_t y = window->y + element->y;
            switch (element->type) {
            case 1: {
                const auto* const data = static_cast<const WindowGraphicData*>(element->data);
                if (data->imageNo == 0) {
                    break;
                }
                std::array<std::uint16_t, 4> rectangle {};
                static_assert(sizeof(rectangle) == 8);
                std::uint8_t cropped = 0;
                if (data->offsetX != 0 || data->offsetY != 0
                    || data->width != 0 || data->height != 0) {
                    std::int32_t graphicIndex;
                    resource::GetGraphicMapNoIndex(data->imageNo, &graphicIndex);
                    std::int16_t graphicX;
                    std::int16_t graphicY;
                    resource::GetGraphicXY(
                        static_cast<std::uint32_t>(graphicIndex), &graphicX, &graphicY);
                    rectangle[0] = static_cast<std::uint16_t>(
                        static_cast<std::uint16_t>(data->offsetX) + graphicX);
                    rectangle[1] = static_cast<std::uint16_t>(
                        static_cast<std::uint16_t>(data->offsetY) + graphicY);
                    rectangle[2] = static_cast<std::uint16_t>(data->width);
                    rectangle[3] = static_cast<std::uint16_t>(data->height);
                    cropped = 1;
                }
                std::memcpy(descriptor.bytes.data(), rectangle.data(), sizeof(rectangle));
                std::memcpy(descriptor.bytes.data() + 8, &data->color, sizeof(data->color));
                descriptor.bytes[12] = cropped;
                descriptor.bytes[13] = 4;
                descriptor.bytes[14] = 0;
                descriptor.bytes[15] = 0;
                WindowInstance* const drawWindow = gCurrentWindow;
                const std::int32_t imageNo = data->imageNo;
                const std::int32_t drawY = drawWindow->y + element->y;
                const std::int32_t drawX = drawWindow->x + element->x;
                static_cast<void>(render::DrawGraphic3_520970(
                    drawX, drawY, 0xDF, imageNo, 0, 1, 0, &descriptor));
                break;
            }
            case 2: {
                const auto* const data = static_cast<const WindowAnimationData*>(element->data);
                const std::uint32_t stop = data->value04;
                login::YobiAction* const action = data->action;
                static_cast<void>(login::detail::Sub_517880(action, 100, stop != 0, 0));
                const std::int32_t graphicIndex = login::detail::GetYobiActionField20(action);
                if (graphicIndex == 0) {
                    break;
                }
                const render::GraphicDescriptor16* const mode =
                    login::detail::ApplyWindowAnimationDrawMode(action);
                const std::int32_t offsetY = login::detail::Sub_5182B0(action, 1);
                WindowInstance* const verticalWindow = gCurrentWindow;
                const std::int32_t elementY = element->y;
                const std::int32_t drawY = offsetY + verticalWindow->y + elementY;
                const std::int32_t offsetX = login::detail::Sub_5182B0(action, 0);
                WindowInstance* const horizontalWindow = gCurrentWindow;
                const std::int32_t elementX = element->x;
                const std::int32_t drawX = offsetX + horizontalWindow->x + elementX;
                static_cast<void>(render::DrawGraphic2_521DD0(
                    drawX, drawY, 0xDF, graphicIndex, 0, 0, mode));
                break;
            }
            case 3: {
                const auto* const data = static_cast<const WindowTextData*>(element->data);
                const char* const text = data->text;
                if (text != nullptr) {
                    const std::int32_t drawY = y - element->height;
                    const auto color = std::bit_cast<std::int8_t>(
                        std::to_integer<std::uint8_t>(data->bytes00[0]));
                    const auto font = std::bit_cast<std::int8_t>(
                        std::to_integer<std::uint8_t>(data->bytes00[1]));
                    const std::int32_t drawX = x - element->width;
                    static_cast<void>(render::DrawText2_456670(
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawX)),
                        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(drawY)),
                        3, font, color, text, 0));
                }
                break;
            }
            case 5: {
                const auto* const data = static_cast<const WindowInputData*>(element->data);
                if (data == gFocusedElementDataD45F94) {
                    if (input::gCurrentInputData == &input::gInputContext626AA0) {
                        if (input::gValueC9A96C == 1) {
                            Sub_4A7590(data->context);
                        }
                    } else if (data != nullptr) {
                        Sub_4A7640(data->context);
                    }
                }
                break;
            }
            case 6:
                Sub_4A7750(x + 5, y);
                break;
            case 7: {
                const auto* const data = static_cast<const WindowNumberData*>(element->data);
                if (data != nullptr) {
                    Sub_4771B0(
                        x, y, data->text, data->style,
                        std::bit_cast<std::int8_t>(data->color), data->flags, 0xDF);
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
    world::detail::Sub_476680();
}

void ApplyObservationInputGate_440ADB()
{
    auto* const bytes = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    if (detail::gValue5F9754 != 0) {
        const unsigned char previous = bytes[7];
        detail::gValue5F9754 = 0;
        bytes[7] = static_cast<unsigned char>(previous - 1);
    }
    for (std::size_t index = 0; index < 8; ++index) {
        bytes[index] = static_cast<unsigned char>(bytes[index] + 1);
    }
}

} // namespace cg::ui
