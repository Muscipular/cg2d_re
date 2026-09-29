#include "WindowRuntimeState.hpp"

#include "../CharacterSelectSceneDependencies.hpp"
#include "../LoginMessageDependencies.hpp"
#include "../LoginSceneDependencies.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "../platform/WindowProcedureState.hpp"
#include "../world/MailState.hpp"
#include "../world/WorldUpdateDependencies.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/LegacyAudio.hpp"
#include "cg/LegacyRecordOrder.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyCursor.hpp"
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

WindowItemSelectorState gItemSelectorCF3B10 {};
WindowItemSelectorState gItemSelectorD2F338 {};

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
WindowInputPrototype gTradeInputPrototype5803D0 {
    {{&input::gInputContextCC21E8}, WindowInputReferenceKind::Context},
    0, -100, 3, 8, 127, {}, 1, 0, 0, 0,
};
WindowInputPrototype gNumberInputPrototype57DA70 {
    {{&input::gInputContextCB0878}, WindowInputReferenceKind::Context},
    0, -100, 3, 8, 127, {}, 1, 0, 0, 0,
};
WindowInputPrototype gStorageInputPrototype57EBA0 {
    {{&input::gInputContextCA0018}, WindowInputReferenceKind::Context},
    0, -100, 3, 12, 127, {}, 1, 0, 0, 0,
};
WindowInputPrototype gTitleInputPrototype57EE10 {
    {{&input::gInputContextCC49C0}, WindowInputReferenceKind::Context},
    0, 0, 3, 12, 0, {}, 1, 16, 0, 0,
};
WindowInputPrototype gDialogInputPrototype57E478 {
    {{&input::gInputContextCA02E0}, WindowInputReferenceKind::Context},
    0, 0, 3, 0, 0, {}, 1, 32, 0, 0,
};
WindowInputPrototype gWindow50InputPrototype580618 {
    {{&input::gInputContextCC2080}, WindowInputReferenceKind::Context},
    33, 368, 3, 12, 0, {}, 2, 70, 16, 0,
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
std::array<char, 5 * 0x600> gPetStorageD64E18 {};
std::array<char, 2 * 0xFBF8> gItemDetailStorageCC4D58 {};
std::array<char, 20 * 0xC58> gShopItemsCF4FE0 {};
std::int32_t gShopItemCountCB1E10 = 0;
std::int32_t gShopReplyItemCountCF4B58 = 0;
std::int32_t gShopImageD2DAD0 = 0;
std::int32_t gValueD2D608 = 0;
std::int32_t gShopDialogTypeCC3EC8 = 0;
std::int32_t gShopValueCF467C = 0;
std::array<std::int8_t, 20> gShopInventoryMapD2DAB8 {};
std::array<char, 20> gShopSelectedSlotsCB03DC {};
std::array<char, 32> gShopTextCB0184 {};
std::array<char, 0xC38> gShopDetailCC3210 {};
std::int32_t gShopValueCAFEA4 = 0;
std::int32_t gShopValueD046C4 = 0;
std::int32_t gShopValueD672D4 = 0;
std::int32_t gShopTooltipIndex580844 = -1;
std::int32_t gShopMessage580848 = -1;
std::int32_t gShopTooltipPageD67B8C = 0;
std::int32_t gShopTotalD2DAFC = 0;
std::int32_t gShopValueC8AF74 = 0;
std::int32_t gShopScrollCC488C = 0;
std::array<char, 55> gShopPriceTextsD673EC {};
std::array<char, 55> gShopCountTextsD2D988 {};
std::array<char, 48> gShopTotalTextCC4B2C {};
std::array<char, 16> gShopMoneyTextCC1A00 {};
std::int32_t gShopAvailableCF4A24 = 0;
std::array<char, 20> gShopValueTextCF4A28 {};
std::array<std::int32_t, 5> gShopRowKeysC9A954 {};
std::uint8_t gShopDragFlagD67B90 = 0;
static_assert(sizeof(gTradeItemsD460D0) == 0x1EBE0);
static_assert(sizeof(gTradePetsC9C108) == 0x3BD8);
static_assert(sizeof(gPetStorageD64E18) == 0x1E00);
static_assert(sizeof(gItemDetailStorageCC4D58) == 0x1F7F0);
static_assert(sizeof(gShopItemsCF4FE0) == 0xF6E0);
static_assert(0x60 + 10 * 0x8C == 0x5D8);
std::int32_t gTradeValueCC4224 = 0;
std::int32_t gStorageValueCC4218 = 0;
std::int32_t gStorageOwnHover57EE04 = -2;
std::int32_t gStorageExternalHover57EE08 = -2;
std::int32_t gBankOwnHover57EDCC = -2;
std::int32_t gBankExternalHover57EDD0 = -2;
const std::array<std::int32_t, 2> gStorageTitleImages54F4E8 {244390, 244320};
const std::array<std::int32_t, 2> gStoragePanelImages54F4F0 {244391, 244321};
const std::array<std::int32_t, 2> gStoragePetImages54F4F8 {244392, 244323};
const std::array<std::int32_t, 2> gStorageMoneyImages54F500 {244393, 244322};
std::array<std::int32_t, 2> gStorageLastAmountsD67B68 {};
std::array<char, 22> gStorageAmountTextsCB1DF8 {};
// 窗口51拥有行表/行文本；其他载荷仅复制或借用，不另建文本owner。
char** gWindowLinesD67B70 = nullptr;
std::int32_t gWindowLineCountD67B74 = 0;
std::int32_t gWindowLineScrollCF46B8 = 0;
std::array<char, 56> gWindowTitleCC4118 {};
std::int32_t gWindowRequestD672B0 = 0;
std::int32_t gWindowRequestCF46AC = 0;
std::int32_t gWindowCloseRequestCC4B28 = 0;
std::array<char, 256> gWindowModalTextC8AF88 {};
std::int32_t gTradeValueCF4688 = 0;
std::int32_t gTradeValueD046D0 = 0;
std::int32_t gTradeValueD2E2B0 = 0;
std::uint8_t gTradeValueD67B80 = 0;
login::YobiAction* gTradeWindowActionD67B7C = nullptr;
std::int32_t gTradeValueCF4A1C = 0;
std::int32_t gTradeValueD673E0 = 0;
std::int32_t gTradeValueCC4C94 = 0;
std::int32_t gTradeValueC8AF68 = 0;
std::int32_t gTradeValueCB03CC = 0;
std::int32_t gTradeValueCF4AB8 = 0;
std::int32_t gTradeValueD67CB8 = 0;
std::int32_t gTradeValueD67CBC = 0;
std::int32_t gTradeValueCC3EC4 = 0;
std::int32_t gTradeValueD66E08 = 0;
std::int32_t gTradeValueCC2344 = 0;
std::array<char, 17> gTradeNameD2D654 {};
std::array<std::array<char, 17>, 10> gWindowListNamesCF3F20 {};
std::int32_t gWindowListCountCF46A8 = 0;
WindowListSelectionCallback gWindowListCallbackCB1E14 = nullptr;
std::int32_t gWindowListOffsetCC3EF4 = 0;
std::array<char, 256> gHoverTextD68AE8 {};
std::int32_t gValueCC4D34 = 0;
std::int32_t gValueCC4D3C = 0;
static_assert(sizeof(gTradeNameD2D654) == 17);
static_assert(sizeof(gWindowListNamesCF3F20) == 170);
static_assert(sizeof(WindowListSelectionCallback) == 8);
std::int32_t gSelectedItemCB1DF4 = 0;
std::int32_t gSelectedItemCB0A44 = 0;
std::uint8_t gItemSelectionModeCC40D4 = 0;
std::int32_t gSelectedItem57EE0C = -1;
std::uint8_t gSkillPairIndexD67AD8 = 0;
std::int32_t gRecipeIndex5846BC = -1;
std::array<std::uint8_t, 20> gTradeItemSlotsCF3AE8 {};
std::array<std::uint8_t, 28> gTradeItemMarksD2D61C {};
std::array<std::uint8_t, 5> gTradePetSlotsCC4D28 {};
std::array<std::uint8_t, 5> gTradePetMarksCF3AFC {};
std::uint8_t gTradePhaseCAFEC4 = 0;
std::uint8_t gTradeStepD3040C = 0;
std::uint8_t gTradeValueD2EA20 = 0;
std::uint8_t gTradeValueD6742C = 0;
std::uint8_t gTradeValueC8AF81 = 0;
std::uint8_t gTradeTabCC4D1A = 0;
std::int32_t gTradeHoverSlotC8B314 = 0;
std::int32_t gTradeHoverSlotC8AA78 = 0;
std::int32_t gTradeHoverSlotCB0A4C = 0;
std::int32_t gPetDragModeD2EA1C = 0;
std::int32_t gItemDragModeCC4C9C = 0;
std::int32_t gItemMoveSourceCF46B0 = 0;
std::int32_t gItemMoveTargetD2DBF8 = 0;
std::int32_t gValueD67B00 = 0;
std::array<char, 12> gNumberWindowTextCC4D1C {};
std::int32_t gTradeScrollCF3B0C = 0;
std::int32_t gTradeScrollCC4C90 = 0;
std::int32_t gTradeScrollD67430 = 0;
std::int32_t gTradePreviewCC2E64 = 0;
std::uint8_t gTradeTooltipPageCB10D4 = 0;
std::uint32_t gWindowActionTick62B5AC = 0;
std::int32_t gTooltipColorD67D30 = 0;
std::int32_t gTooltipSourceD303FC = 0;
std::uint16_t gTooltipXCF46C0 = 0;
std::uint16_t gTooltipYCF46CC = 0;
const std::array<std::array<std::int32_t, 4>, 4> gPetBarImages57E434 {{
    {243151, 243152, 243153, 243154},
    {243156, 243157, 243158, 243159},
    {243161, 243162, 243163, 243164},
    {243166, 243167, 243168, 243169},
}};
const std::array<std::array<std::int32_t, 3>, 14> gNumericKeyImages54F030 {{
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
const std::array<const char*, 6> gTradeNumberHints5805FC {{
    "\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43",
    "\xAB\xFC\xA9\x77\xB4\xA3\xBB\xE2\xAA\xF7\xC3\x42\xA1\x43",
    "\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xBC\xC6\xB6\x71\xA1\x43",
    "\xB4\xEE\xA4\xD6\xA4\x40\xAD\xD3\xA6\xEC\xBC\xC6\xA1\x43",
    "\xAB\xFC\xA9\x77\xA5\xFE\xB3\xA1\xAA\xF7\xC3\x42\xA1\x43",
    "\xA7\xE2\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xC2\x6B\xB9\x73\xA1\x43",
}};
const std::array<const char*, 6> gNumberWindowHints57DC9C {{
    "\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43",
    "\xB1\x4E\xAB\xFC\xA9\x77\xAA\xAB\xAA\xF7\xC3\x42\xA9\xF1\xA6\x61\xA4\x57\xA1\x43",
    "\xBD\xD0\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43",
    "\xB4\xEE\xA4\xD6\xA4\x40\xA6\xEC\xBC\xC6\xA1\x43",
    "\xAB\xFC\xA9\x77\xA5\xFE\xC3\x42\xA1\x43",
    "\xB1\x4E\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xC2\x6B\xB9\x73\xA1\x43",
}};
const std::array<const char*, 6> gStorageDepositHints57EDD4 {{
    "\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43",
    "\xAB\xFC\xA9\x77\xA6\x73\xA4\x4A\xAA\xF7\xC3\x42\xA1\x43",
    "\xBD\xD0\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43",
    "\xB4\xEE\xA4\xD6\xA4\x40\xAD\xD3\xA6\xEC\xBC\xC6\xA1\x43",
    "\xAB\xFC\xA9\x77\xA5\xFE\xB3\xA1\xAA\xF7\xC3\x42\xA1\x43",
    "\xA7\xE2\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xC2\x6B\xB9\x73\xA1\x43",
}};
const std::array<const char*, 6> gStorageWithdrawHints57EDEC {{
    "\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xA1\x43",
    "\xAB\xFC\xA9\x77\xB4\xA3\xBB\xE2\xAA\xF7\xC3\x42\xA1\x43",
    "\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xBC\xC6\xB6\x71\xA1\x43",
    "\xB4\xEE\xA4\xD6\xA4\x40\xAD\xD3\xA6\xEC\xBC\xC6\xA1\x43",
    "\xAB\xFC\xA9\x77\xA5\xFE\xB3\xA1\xAA\xF7\xC3\x42\xA1\x43",
    "\xA7\xE2\xAB\xFC\xA9\x77\xAA\xF7\xC3\x42\xC2\x6B\xB9\x73\xA1\x43",
}};
static constexpr char kStoragePetEquipmentWarning587900[] =
    "\xC3\x64\xAA\xAB\xB1\xFD\xA9\xF1\xA4\x4A\xAE\x61\xB1\xDA\xC3\x64"
    "\xAA\xAB\xAE\x77\xA1\x41\xBD\xD0\xA8\xF8\xA4\x55\xC3\x64\xAA\xAB"
    "\xA8\xAD\xA4\x57\xAA\xBA\xB8\xCB\xB3\xC6\xAB\xE1\xA6\x41\xBE\xDE"
    "\xA7\x40\xA1\x43";
static constexpr char kTribeUnopened585C50[] = "\xA5\xBC\xB6\x7D\xA9\xF1";
const std::array<const char*, 20> gTribeNames57CB54 {{
    "\xA4\x48\xA7\xCE\xA8\x74",
    "\xC0\x73\xA8\x74",
    "\xA4\xA3\xA6\xBA\xA8\x74",
    "\xAD\xB8\xA6\xE6\xA8\x74",
    "\xA9\xF8\xC2\xCE\xA8\x74",
    "\xB4\xD3\xAA\xAB\xA8\x74",
    "\xB3\xA5\xC3\x7E\xA8\x74",
    "\xAF\x53\xAE\xED\xA8\x74",
    "\xAA\xF7\xC4\xDD\xA8\x74",
    "\xA8\xB8\xC5\x5D\xA8\x74",
    "\xAF\xAB\xB1\xDA",
    "\xBA\xEB\xC6\x46\xB1\xDA",
    kTribeUnopened585C50, kTribeUnopened585C50, kTribeUnopened585C50, kTribeUnopened585C50,
    kTribeUnopened585C50, kTribeUnopened585C50, kTribeUnopened585C50, kTribeUnopened585C50,
}};
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
std::int32_t gInventoryHover57DA2C = -2;
std::int32_t gEquipmentHover57DA68 = -2;
std::array<char, 12> gInventoryMoneyTextCB086C {};
std::array<char, 12> gInventoryValueTextCC3ECC {};
std::int32_t gBattleItemHoverCF49B0 = 0;
std::int32_t gBattleItemPageCB01E0 = 0;
std::uint8_t gBattleItemTargetFlagCC4D40 = 0;
static std::int32_t gBattleBackpackHover57E430 = -2;
std::array<char, 12> gBattleMoneyTextCF469C {};
std::array<char, 12> gBattleValueTextC8A960 {};
std::uint8_t gValue57E42C = 0xFF;
const std::array<std::int32_t, 8> gSelectorOwnX54F584 {100, 36, 36, 165, 165, 36, 165, 165};
const std::array<std::int32_t, 8> gSelectorOwnY54F5A4 {30, 130, 80, 80, 130, 30, 30, 180};
const std::array<std::int32_t, 6> gSelectorPetX54F5C4 {36, 165, 36, 165, 36, 165};
const std::array<std::int32_t, 6> gSelectorPetY54F5DC {80, 80, 130, 130, 30, 30};
std::int32_t gValueCF4AAC = 0;
std::int32_t gValueCF4690 = 0;
std::int32_t gValueCA0440 = 0;
std::int32_t gValueD67B38 = 0;
login::YobiAction* gWindowActionCA0174 = nullptr;
login::YobiAction* gWindowActionD2E928 = nullptr;
login::YobiAction* gPreviewActionD67B84 = nullptr;
std::int32_t gValueD67AF8 = 0;
std::uint8_t gValueD67AFC = 0;
std::uint8_t gValueD67AFD = 0;
std::uint8_t gValueD67AFE = 0;
std::uint8_t gValueD67B24 = 0;

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
// 原D30138..D303F8；4B6970整区清零，4DFD38从+9起消费，其他字段尚未冻结。
std::array<std::byte, 0x2C0> gWindow50AlternateRecordsD30138 {};
constexpr char kWindow50WriteHint587C84[] = "\xBC\x67\xA4\x55\xB0\x54\xAE\xA7\xA1\x43";

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
    char** inputTexts; // 原C9A968，拥有表和各行；失败处理/释放仍走原分支。
    std::int32_t selectedInput; // 原D66E30。
    std::int32_t closeCallbackPending; // 原CC31A8，不是bool。
    std::int32_t hoveredButton; // 原CC18C0的完整W32按钮位。
    std::int32_t hoveredRow; // 原CF4A20。
    std::int32_t events; // 原D2DA90。
};

struct MessageDialogImageRecord {
    std::int32_t imageNo;
    std::uint8_t direction;
    std::byte byte05;
    std::int16_t x;
    std::int16_t y;
    std::uint8_t used;
    std::byte byte0B;
};
static_assert(sizeof(MessageDialogImageRecord) == 12);
static_assert(offsetof(MessageDialogImageRecord, direction) == 4);
static_assert(offsetof(MessageDialogImageRecord, x) == 6);
static_assert(offsetof(MessageDialogImageRecord, y) == 8);
static_assert(offsetof(MessageDialogImageRecord, used) == 10);
MessageDialogRuntimeSlots gMessageDialog {};
std::array<MessageDialogImageRecord, 10> gMessageDialogImagesCC4B70 {};
std::uint8_t gMessageDialogImageCountD67B30 = 0;

// 原CB1388..CB1394；49DF30/49E400共同生产，49DA30回复逐次读取。
struct DialogReplyRuntimeSlots {
    std::int32_t type;
    std::int32_t request;
    std::int32_t target;
    std::int32_t firstLine;
};
DialogReplyRuntimeSlots gDialogReply {};
std::array<WindowDialogTextLine, 30> gDialogLinesCB1398 {};
std::array<std::int16_t, 2> gDialogInputYD67B34 {};

// 原C9A978..C9A998，所有文本/表及回调均借用；计数保存低U8、循环按S8解释。
struct ChoiceDialogRuntimeSlots {
    std::int32_t type;
    std::int32_t imageNo;
    const char* title;
    const char* const* lines;
    const char* const* choices;
    std::uint8_t lineCount;
    std::uint8_t choiceCount;
    const char* const* hints;
    WindowChoiceCallback callback;
    std::uint8_t closeEnabled;
};
ChoiceDialogRuntimeSlots gChoiceDialog {};
// CC24D4首行标题和CC2525起正文必须同源，其他NPC窗口也消费这一区域。
std::array<WindowDialogTextLine, 30> gNpcDialogLinesCC24D4 {};
std::array<const char*, 5> gChoiceDialogLinesD672D8 {};
std::int32_t gShopImageC9A9A0 = 0;
std::array<const char*, 3> gShopChoicesCB1DC8 {};
std::array<const char*, 3> gShopHintsD2F738 {};
std::array<std::int8_t, 3> gShopCommandsD64E10 {};
constexpr std::array<const char*, 3> kShopChoices57E6B8 {
    "\xB6\x52", "\xBD\xE6", "\xA4\xA3\xA5\xCE\xA4\x46",
};
constexpr std::array<const char*, 3> kShopHints57E6C4 {
    "\xA6\x62\xB3\x6F\xAE\x61\xA9\xB1\xB6\x52\xAA\xAB\xAB\x7E\xA1\x43",
    "\xA6\x62\xB3\x6F\xAE\x61\xA9\xB1\xBD\xE6\xAA\xAB\xAB\x7E\xA1\x43",
    "\xC1\xD9\xAC\x4F\xBA\xE2\xA4\x46\xA1\x43",
};
constexpr char kShopUnavailableHint586BF4[] = "\xA1\xAF\xB2\x7B\xA6\x62\xB5\x4C\xAA\x6B\xB8\xCB\xB3\xC6";

std::int32_t gChoiceImageCB0AE4 = 0;
std::int32_t gChoiceImageCF4A18 = 0;
std::int32_t gChoiceImageCC4890 = 0;
std::int32_t gChoiceImageC9A99C = 0;
std::int32_t gChoiceImageD2EA74 = 0;
std::array<const char*, 2> gChoicesD67438 {};
std::array<const char*, 2> gChoiceHintsCC31FC {};
std::array<std::int8_t, 2> gChoiceCommandsCB03D4 {};
std::array<const char*, 2> gChoicesD2D818 {};
std::array<const char*, 2> gChoiceHintsCA0AAC {};
std::array<std::int8_t, 2> gChoiceCommandsCF47D8 {};
constexpr std::array<const char*, 3> kSkillChoices57E6F0 {
    "\xB7\x51\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0",
    "\xB7\x51\xBF\xF2\xA7\xD1\xA7\xDE\xAF\xE0",
    kShopChoices57E6B8[2],
};
constexpr std::array<const char*, 3> kSkillHints57E6FC {
    "\xA6\x56\xB3\x6F\xA6\xEC\xA7\xDE\xAF\xE0\xAE\x76\xBD\x64"
    "\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0\xA1\x43",
    "\xBF\xF2\xA7\xD1\xA6\xDB\xA4\x76\xBE\xC7\xB2\xDF\xA8\xEC"
    "\xAA\xBA\xA7\xDE\xAF\xE0\xA1\x43",
    "\xA4\xA3\xBE\xC7\xB2\xDF\xA7\xDE\xAF\xE0\xA1\x43",
};
constexpr std::array<const char*, 3> kExchangeChoices57E708 {
    "\xA5\xE6\xA9\xF6", kShopChoices57E6B8[2], nullptr,
};
constexpr std::array<const char*, 3> kExchangeHints57E714 {
    "\xA5\xE6\xB4\xAB\xB3\x6F\xAE\x61\xA9\xB1\xAA\xBA\xAA\xAB\xAB\x7E\xA1\x43",
    kShopHints57E6C4[2], nullptr,
};
constexpr std::array<const char*, 2> kGiveReceiveLabels57E720 {
    "\xB5\xB9\xA4\xA9\xAA\xAB\xAB\x7E", "\xB1\xB5\xA8\xFC\xAA\xAB\xAB\x7E",
};
std::array<const char*, 2> gGiveReceiveChoices57E728 {nullptr, kShopChoices57E6B8[2]};
constexpr std::array<const char*, 3> kGiveReceiveHints57E730 {
    kExchangeHints57E714[0], kShopHints57E6C4[2], nullptr,
};

// 五个选择创建入口共用的原字段提取/反转义/五行发布序列。
void FillChoiceDialogBody(
    const char* const text, const std::int32_t field, const std::int32_t capacity)
{
    std::array<char, 256> body;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field, capacity, body.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(body.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        char* const line = gNpcDialogLinesCC24D4[row + 1].text.data();
        static_cast<void>(input::ExtractLegacyLine_503AB0(line, 81, body.data(), row, 20));
        gChoiceDialogLinesD672D8[row] = line;
    }
}

// 调用者传原连续32B临时区；不缓存跨回复的目标/请求/坐标。
std::int32_t SendChoiceDialogReply(char* const buffer)
{
    input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 16, 15);
    const std::int32_t target = world::detail::gDialogTargetD2DE6C;
    const std::int32_t request = world::detail::gDialogRequestCC21E0;
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    return network::Sub_512620(platform::gConnectionSocket, x, y, request, target, 0, buffer + 16);
}

// 原D672F0七个32B记录；第七条同源D673B0/B8用于合计，4DC290也消费此表。
struct HealingDialogRecord {
    std::int32_t level;
    std::int32_t health;
    std::int32_t cost;
    std::array<char, 17> name;
    std::array<std::byte, 3> reserved1D;
};
static_assert(sizeof(HealingDialogRecord) == 32);
static_assert(offsetof(HealingDialogRecord, cost) == 8);
static_assert(offsetof(HealingDialogRecord, name) == 12);
std::array<HealingDialogRecord, 7> gHealingDialogRecordsD672F0 {};
std::int32_t gHealingDialogHealthD2DC00 = 0;
constexpr std::array<std::int32_t, 5> gHealthImages57CBA4 {22520, 22521, 22522, 22523, 22524};

// 原519680：零单独处理，其余按有符号26/51/76分档，不钳制负值。
std::int32_t Sub_519680(const std::int32_t health)
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

std::int32_t Sub_4D3020(const std::int32_t x, const std::int32_t y)
{
    gValueCC4D34 = x;
    gValueCC4D3C = y;
    return x;
}

std::int32_t Sub_4D3040(const std::int32_t rangeX, const std::int32_t rangeY)
{
    const double playerX = static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX)) * 64.0;
    if (playerX != static_cast<double>(std::bit_cast<float>(
            static_cast<std::uint32_t>(world::gWorldCurrentXBits)))) {
        return 0;
    }
    const double playerY = static_cast<double>(
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY)) * 64.0;
    if (playerY != static_cast<double>(std::bit_cast<float>(
            static_cast<std::uint32_t>(world::gWorldCurrentYBits)))) {
        return 0;
    }
    double distanceX = static_cast<double>(gValueCC4D34 * 64)
        - static_cast<double>(std::bit_cast<float>(
            static_cast<std::uint32_t>(world::gWorldCurrentXBits)));
    if (distanceX < 0.0) {
        distanceX = -distanceX;
    }
    if (distanceX >= static_cast<double>(rangeX * 64)) {
        return 1;
    }
    double distanceY = static_cast<double>(gValueCC4D3C * 64)
        - static_cast<double>(std::bit_cast<float>(
            static_cast<std::uint32_t>(world::gWorldCurrentYBits)));
    if (distanceY < 0.0) {
        distanceY = -distanceY;
    }
    return distanceY >= static_cast<double>(rangeY * 64) ? 1 : 0;
}

login::YobiAction* Sub_4B0110()
{
    gTradeValueD67B80 = 0;
    login::YobiAction* const action = CreateWindowAnimation_475AA0(43, 1, 0, nullptr);
    gTradeWindowActionD67B7C = action;
    return action;
}

void Sub_4B0130(
    WindowInstance* const window, const std::int32_t index,
    const std::int32_t x, const std::int32_t y)
{
    const std::int32_t shortX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const std::int32_t shortY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    window->elements[index].x = shortX;
    window->elements[index].y = shortY;
    window->elements[index - 1].x = shortX + 252;
    window->elements[index - 1].y = shortY + 37;
    window->elements[index - 2].x = shortX + 252;
}

void Sub_4B0180(
    WindowInstance* const window, const std::int32_t index, const std::int32_t visible)
{
    window->elements[index].visible = visible;
    if (visible == 0) {
        window->elements[index - 1].visible = 0;
        window->elements[index - 2].visible = 0;
    }
}

void Sub_4B01B0(
    WindowInstance* const window, const std::int32_t index, const std::uint32_t color)
{
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    graphic->color = color;
}

std::int32_t Sub_4AFF30(const std::int32_t index, const std::int32_t slot)
{
    if (std::bit_cast<std::int8_t>(gTradeItemSlotsCF3AE8[index]) >= 0) {
        return 0;
    }
    gTradeItemSlotsCF3AE8[index] = static_cast<std::uint8_t>(slot);
    auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    const std::uint8_t previous = hover[slot];
    gTradeItemMarksD2D61C[slot] = 1;
    hover[slot] = static_cast<std::uint8_t>(previous + 1);
    return 1;
}

std::int32_t Sub_4AFFB0(const std::int32_t first, const std::int32_t second)
{
    const std::uint8_t previous = gTradeItemSlotsCF3AE8[first];
    gTradeItemSlotsCF3AE8[first] = gTradeItemSlotsCF3AE8[second];
    gTradeItemSlotsCF3AE8[second] = previous;
    return first;
}

std::int32_t Sub_4B0060(const std::int32_t index, const std::int32_t slot)
{
    if (std::bit_cast<std::int8_t>(gTradePetSlotsCC4D28[index]) >= 0) {
        return 0;
    }
    gTradePetSlotsCC4D28[index] = static_cast<std::uint8_t>(slot);
    gTradePetMarksCF3AFC[slot] = 1;
    return 1;
}

std::int32_t Sub_4B00C0(const std::int32_t first, const std::int32_t second)
{
    const std::uint8_t previous = gTradePetSlotsCC4D28[first];
    gTradePetSlotsCC4D28[first] = gTradePetSlotsCC4D28[second];
    gTradePetSlotsCC4D28[second] = previous;
    return first;
}

std::int32_t Sub_48BB30(const std::int32_t slot)
{
    if (slot >= 0 && slot < 8) {
        return 1;
    }
    if (slot >= 8 && slot < 28) {
        return 2;
    }
    return slot >= 100 && slot < 120 ? 3 : 0;
}

std::int32_t Sub_48BB70(const std::int32_t slot)
{
    return slot >= 0 && slot < 5 ? 1 : 2;
}

std::int32_t Sub_48BC60(const std::int32_t slot)
{
    for (std::int32_t position = 0; position < 5; ++position) {
        if (std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index) == slot) {
            return position;
        }
    }
    return -1;
}

void Sub_47D490()
{
    gValueD67B00 = 0;
}

WindowInstance* Sub_48A830(
    const std::int32_t sourceWindow, const std::int32_t sourceElement,
    const std::int32_t imageNo, const std::int32_t maximum, const std::int32_t value14,
    const std::int32_t initialValue, const WindowValueCallback callback)
{
    if (gWindowStates[42].window != nullptr) {
        gWindowStates[42].window->state &= ~std::uint32_t{0x8000};
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    } else {
        gWindowStates[42].window = OpenWindow_471440(42);
    }
    static_cast<WindowGraphicData*>(gWindowStates[42].window->elements[6].data)->imageNo = imageNo;
    gWindowStates[42].value10 = static_cast<std::uint32_t>(maximum);
    gWindowStates[42].value14 = static_cast<std::uint32_t>(value14);
    gWindowStates[42].userState.callback = callback;
    gWindowStates[42].window->elements[2].state = static_cast<std::uint32_t>(initialValue);
    gWindowStates[42].value20 = static_cast<std::uint32_t>(sourceWindow);
    gWindowStates[42].value24 = static_cast<std::uint32_t>(sourceElement);
    return gWindowStates[42].window;
}

std::int32_t Sub_48A8C0(const std::int32_t value)
{
    if (value == -1) {
        WindowInstance* const window = gCurrentWindow;
        gWindowStates[42].value10 = 999999999;
        gWindowStates[42].value14 = 0;
        gWindowStates[42].userState.callback = nullptr;
        window->elements[2].state = 0;
        static_cast<void>(Sub_48A900(0, 2));
    }
    return 1;
}

std::int32_t Sub_48A900(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) != 0) {
        static_cast<void>(Sub_476570(&gNumberInputPrototype57DA70, 0, -100, 0));
        input::SetContext_44B330(&input::gInputContextCB0878);
        WindowInstance* const window = gCurrentWindow;
        gFocusedElementDataD45F94 = window->elements[0].data;
        static_cast<WindowInputData*>(window->elements[0].data)->context = &input::gInputContextCB0878;
    }
    return 0;
}

std::int32_t Sub_48A950(const std::int32_t index, const std::int32_t state)
{
    const input::InputContext* const currentInput = input::gCurrentInputData;
    const std::uint8_t flags = static_cast<std::uint8_t>(gWindowStates[42].actionFlags1C)
        | (currentInput == &input::gInputContextCB0878 ? 2u : 0u);
    const std::uint8_t numeric = Sub_474550(
        gCurrentWindow, index, state, std::bit_cast<std::int32_t>(gWindowStates[42].value14),
        223, gNumberWindowHints57DC9C.data(), flags, 0xFFFFFFFFu);
    WindowInstance* window = gCurrentWindow;
    std::int32_t maximum = std::bit_cast<std::int32_t>(gWindowStates[42].value10);
    if (std::bit_cast<std::int32_t>(window->elements[index].state) > maximum) {
        window->elements[index].state = static_cast<std::uint32_t>(maximum);
        maximum = std::bit_cast<std::int32_t>(gWindowStates[42].value10);
        window = gCurrentWindow;
    }
    std::int32_t result = 0;
    if ((numeric & 1u) != 0) {
        if (std::bit_cast<std::int32_t>(window->elements[index].state) > maximum) {
            window->elements[index].state = static_cast<std::uint32_t>(maximum);
            window = gCurrentWindow;
        }
        result = 1;
    }
    if ((numeric & 2u) != 0) {
        const WindowValueCallback callback = gWindowStates[42].userState.callback;
        if (callback != nullptr) {
            callback(std::bit_cast<std::int32_t>(window->elements[index].state));
            window = gCurrentWindow;
        }
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        window = gCurrentWindow;
        result = 1;
    }
    std::sprintf(gNumberWindowTextCC4D1C.data(), "%9d",
        std::bit_cast<std::int32_t>(window->elements[index].state));
    return result;
}

std::int32_t Sub_48AA60(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xBC\x57\xA5\x5B\xC5\xE3\xA5\xDC\xBC\xC6\xB6\x71\xA1\x43");
    }
    if ((state & 2) != 0) {
        const std::int32_t value = std::bit_cast<std::int32_t>(window->elements[2].state);
        if (value < std::bit_cast<std::int32_t>(gWindowStates[42].value10)) {
            window->elements[2].state = static_cast<std::uint32_t>(value + 1);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return 0;
}

std::int32_t Sub_48AB10(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xB4\xEE\xA4\xD6\xC5\xE3\xA5\xDC\xBC\xC6\xB6\x71\xA1\x43");
    }
    if ((state & 2) != 0) {
        const std::int32_t value = std::bit_cast<std::int32_t>(window->elements[2].state);
        if (value > 0) {
            window->elements[2].state = static_cast<std::uint32_t>(value - 1);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return 0;
}

std::int32_t Sub_474170(const std::int32_t, const std::int32_t state)
{
    if ((state & 2) != 0) {
        gCurrentWindow->state |= 0x2000u;
    }
    return (state & 1) != 0 ? 1 : (static_cast<std::uint8_t>(state) >> 5) & 1;
}

std::int32_t Sub_4885A0(const std::int32_t amount)
{
    std::int32_t source = gItemMoveSourceCF46B0;
    std::int32_t target;
    if (source == 7 || (target = gItemMoveTargetD2DBF8) == 7) {
        Sub_47D490();
        source = gItemMoveSourceCF46B0;
        target = gItemMoveTargetD2DBF8;
    }
    if (source < 100 && target < 100) {
        return network::Sub_510EC0(platform::gConnectionSocket, source, target, amount);
    }
    if (source < 8) {
        return source;
    }
    WindowState* windowState = &gWindowStates[41];
    if (windowState->window == nullptr) {
        windowState = &gWindowStates[38];
        if (windowState->window == nullptr) {
            return source;
        }
    }
    char buffer[384];
    std::sprintf(buffer, "I|%d|%d|%d", source, target, amount);
    input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 128, 255);
    const auto userState = std::bit_cast<std::int32_t>(windowState->userState.value);
    const auto value14 = std::bit_cast<std::int32_t>(windowState->value14);
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    return network::Sub_512620(
        platform::gConnectionSocket, x, y, value14, userState, 0, buffer + 128);
}

void Sub_4886E0(
    const std::int32_t sourceWindow, const std::int32_t sourceElement,
    const std::int32_t source, const std::int32_t target)
{
    if (source < 0 || target < 0 || source == target) {
        return;
    }
    bool split = (platform::gKeyStates[0x11] & 2u) == 0;
    if (platform::gValueD67AE8 != 0) {
        split = !split;
    }
    std::int32_t count;
    if (source < 100) {
        count = world::detail::gPlayerItemsFDA8A4[source].count;
    } else {
        std::memcpy(
            &count, gItemDetailStorageCC4D58.data() + (source - 100) * 0xC44 + 8, sizeof(count));
    }
    std::int16_t used;
    if (target < 100) {
        used = std::bit_cast<std::int16_t>(world::detail::gPlayerItemsFDA8A4[target].used);
    } else {
        std::memcpy(
            &used, gItemDetailStorageCC4D58.data() + (target - 100) * 0xC44, sizeof(used));
    }
    if (source < 8 || target < 8 || count <= 0 || used != 0 || !split) {
        gItemMoveSourceCF46B0 = source;
        gItemMoveTargetD2DBF8 = target;
        static_cast<void>(Sub_4885A0(-1));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    } else {
        gItemMoveTargetD2DBF8 = target;
        gItemMoveSourceCF46B0 = source;
        static_cast<void>(login::detail::Sub_48AC60(
            sourceWindow, sourceElement, 244309, count, count, 0,
            [](const std::int32_t amount) { static_cast<void>(Sub_4885A0(amount)); },
            1, 0, 0));
    }
}

void Sub_488800(
    const std::int32_t sourceMode, const std::int32_t source,
    const std::int32_t targetMode, const std::int32_t target)
{
    if (target == -1) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (source < 0 || target < -1) {
        return;
    }
    const std::uint8_t from = static_cast<std::uint8_t>(sourceMode);
    const std::uint8_t to = static_cast<std::uint8_t>(targetMode);
    if (from != 0) {
        if (from == to) {
            const std::int32_t targetGroup = Sub_4C71B0(&gItemSelectorCF3B10) + 1;
            const std::int32_t sourceGroup = Sub_4C71B0(&gItemSelectorCF3B10) + 1;
            static_cast<void>(network::Sub_516C90(
                platform::gConnectionSocket, sourceGroup, source, targetGroup, target, -1));
        } else {
            const std::int32_t sourceGroup = Sub_4C71B0(&gItemSelectorCF3B10) + 1;
            static_cast<void>(network::Sub_516C90(
                platform::gConnectionSocket, sourceGroup, source, 0, target, -1));
        }
    } else if (to != 0) {
        const std::int32_t targetGroup = Sub_4C71B0(&gItemSelectorCF3B10) + 1;
        static_cast<void>(network::Sub_516C90(
            platform::gConnectionSocket, 0, source, targetGroup, target, -1));
    }
}

void Sub_48BB90(
    const std::int32_t, const std::int32_t,
    const std::int32_t source, const std::int32_t target)
{
    if (source < 0 || target < 0 || source == target) {
        return;
    }
    if (source < 100 && target < 100) {
        static_cast<void>(network::Sub_511BC0(platform::gConnectionSocket, source, target));
        return;
    }
    if (gWindowStates[41].window == nullptr) {
        return;
    }
    char buffer[384];
    std::sprintf(buffer, "P|%d|%d", source, target);
    input::EscapeLegacyTextAndNotify_52D2D0(buffer, buffer + 128, 255);
    const auto userState = std::bit_cast<std::int32_t>(gWindowStates[41].userState.value);
    const auto value14 = std::bit_cast<std::int32_t>(gWindowStates[41].value14);
    const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    static_cast<void>(network::Sub_512620(
        platform::gConnectionSocket, x, y, value14, userState, 0, buffer + 128));
}

std::int32_t Sub_4AFF70(const std::int32_t index)
{
    const std::int32_t slot = std::bit_cast<std::int8_t>(gTradeItemSlotsCF3AE8[index]);
    if (slot < 0) {
        return -1;
    }
    gTradeItemSlotsCF3AE8[index] = 0xFF;
    auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    const std::uint8_t previous = hover[slot];
    gTradeItemMarksD2D61C[slot] = 0;
    hover[slot] = static_cast<std::uint8_t>(previous - 1);
    return slot;
}

std::int32_t Sub_4B0090(const std::int32_t index)
{
    const std::int32_t slot = std::bit_cast<std::int8_t>(gTradePetSlotsCC4D28[index]);
    if (slot < 0) {
        return -1;
    }
    gTradePetSlotsCC4D28[index] = 0xFF;
    gTradePetMarksCF3AFC[slot] = 0;
    return slot;
}

void Sub_4B0200()
{
    WindowInstance* const window = gWindowStates[42].window;
    if (window != nullptr && gWindowStates[42].value20 == 43) {
        window->state |= 0x8000u;
    }
    if (gTradeValueD67B80 == 0) {
        static_cast<void>(network::Sub_512270(platform::gConnectionSocket, 0));
        gTradeValueD67CBC = 1;
    }
    for (std::int32_t index = 0; index < 20; ++index) {
        static_cast<void>(Sub_4AFF70(index));
    }
    for (std::int32_t index = 0; index < 5; ++index) {
        static_cast<void>(Sub_4B0090(index));
    }
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    gTradeValueD67B80 = 0;
}

std::int32_t Sub_4B0290(const std::int32_t value)
{
    if (value == -1) {
        WindowInstance* const initialWindow = gCurrentWindow;
        gTradeWindowActionD67B7C = nullptr;
        gTradeValueCC4224 = 0;
        gTradeValueD2E2B0 = 0;
        gTradePhaseCAFEC4 = 0;
        Sub_4B0180(initialWindow, 11, 1);
        Sub_4B0130(gCurrentWindow, 11, 287, 28);
        Sub_4B01B0(gCurrentWindow, 11, 0xFFFFFFFFu);
        Sub_4B0180(gCurrentWindow, 8, 1);
        Sub_4B0130(gCurrentWindow, 8, 15, 28);
        Sub_4B01B0(gCurrentWindow, 8, 0xFFFFFFFFu);
        Sub_4B0180(gCurrentWindow, 14, 0);
        Sub_4B0130(gCurrentWindow, 14, 15, 28);
        Sub_4B01B0(gCurrentWindow, 14, 0xFFFFFFFFu);
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[3].visible = 0;
        gCurrentWindow->elements[4].visible = 0;
        WindowInstance* const scrollWindow = gCurrentWindow;
        gTradeValueD2EA20 = 0xFF;
        gTradeValueD6742C = 0;
        gTradeValueC8AF81 = 0;
        gTradeScrollCF3B0C = 0;
        gTradeScrollCC4C90 = 0;
        gTradeScrollD67430 = 0;
        static_cast<void>(Sub_4742C0(&scrollWindow->elements[10], 2, 0));
        const std::int32_t firstScroll = gTradeScrollCC4C90;
        static_cast<void>(Sub_4742C0(&gCurrentWindow->elements[7], 2, firstScroll));
        WindowInstance* const lastScrollWindow = gCurrentWindow;
        const std::int32_t lastScroll = gTradeScrollD67430;
        static_cast<void>(Sub_4742C0(&lastScrollWindow->elements[13], 2, lastScroll));
        gTradeItemMarksD2D61C.fill(0);
        // 原W32/U8交错写入同一数组，保留4B0424的窗口快照时点。
        std::memset(gTradePetMarksCF3AFC.data(), 0, 4);
        std::memset(gTradeItemSlotsCF3AE8.data(), 0xFF, 4);
        std::memset(gTradeItemSlotsCF3AE8.data() + 4, 0xFF, 4);
        gTradePetMarksCF3AFC[4] = 0;
        WindowInstance* const textWindow = gCurrentWindow;
        std::memset(gTradeItemSlotsCF3AE8.data() + 8, 0xFF, 4);
        std::memset(gTradeItemSlotsCF3AE8.data() + 12, 0xFF, 4);
        std::memset(gTradePetSlotsCC4D28.data(), 0xFF, 4);
        std::memset(gTradeItemSlotsCF3AE8.data() + 16, 0xFF, 4);
        gTradePetSlotsCC4D28[4] = 0xFF;
        auto* const otherName = static_cast<WindowTextData*>(textWindow->elements[15].data);
        std::sprintf(otherName->text, "%16s", gTradeNameD2D654.data());
        WindowElement* const elements = gCurrentWindow->elements;
        const char* const playerName = world::detail::BorrowPlayerDisplayText(1);
        auto* const separator = static_cast<WindowTextData*>(elements[16].data);
        std::strcpy(separator->text, "<=>");
        auto* const ownName = static_cast<WindowTextData*>(gCurrentWindow->elements[17].data);
        std::sprintf(ownName->text, "%-16s", playerName);
        const std::int32_t y =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t x =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(Sub_4D3020(x, y));
    }
    if (Sub_4D3040(1, 1) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (gTradeValueD67B80 != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    switch (std::bit_cast<std::int8_t>(gTradePhaseCAFEC4)) {
    case 0:
        if (gTradeValueD2E2B0 > 0) {
            WindowInstance* const window = gCurrentWindow;
            gTradePhaseCAFEC4 = 1;
            Sub_4B0180(window, 14, 1);
            Sub_4B0130(gCurrentWindow, 14, 15, 28);
            Sub_4B01B0(gCurrentWindow, 14, 0x00FFFFFFu);
            gTradeStepD3040C = 23;
        }
        break;
    case 1: {
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t step = std::bit_cast<std::int8_t>(gTradeStepD3040C);
        if (window->elements[8].x + step < 287) {
            Sub_4B0180(window, 8, 1);
            WindowInstance* const moveWindow = gCurrentWindow;
            const std::int32_t moveStep = std::bit_cast<std::int8_t>(gTradeStepD3040C);
            const std::int32_t x = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(moveWindow->elements[8].x));
            Sub_4B0130(moveWindow, 8, x + moveStep, 28);
            const std::uint8_t oldStep = gTradeStepD3040C;
            WindowInstance* const firstWindow = gCurrentWindow;
            gTradeStepD3040C = static_cast<std::uint8_t>(oldStep - 1);
            const auto* first = static_cast<WindowGraphicData*>(firstWindow->elements[11].data);
            std::int32_t alpha = static_cast<std::int32_t>(first->color >> 24) - 10;
            if (alpha < 0) {
                alpha = 0;
            }
            Sub_4B01B0(firstWindow, 11,
                (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
            WindowInstance* const secondWindow = gCurrentWindow;
            const auto* second = static_cast<WindowGraphicData*>(secondWindow->elements[14].data);
            alpha = static_cast<std::int32_t>(second->color >> 24) + 10;
            if (alpha > 255) {
                alpha = 255;
            }
            Sub_4B01B0(secondWindow, 14,
                (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
        } else {
            Sub_4B01B0(window, 14, 0xFFFFFFFFu);
            Sub_4B0180(gCurrentWindow, 8, 1);
            Sub_4B0130(gCurrentWindow, 8, 287, 28);
            Sub_4B0180(gCurrentWindow, 11, 0);
            gCurrentWindow->elements[2].visible = 0;
            gCurrentWindow->elements[3].visible = 1;
            gCurrentWindow->elements[4].visible = 1;
            gTradePhaseCAFEC4 = 3;
        }
        break;
    }
    case 2: {
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t step = std::bit_cast<std::int8_t>(gTradeStepD3040C);
        if (window->elements[8].x - step > 15) {
            Sub_4B0180(window, 8, 1);
            WindowInstance* const moveWindow = gCurrentWindow;
            const std::int32_t moveStep = std::bit_cast<std::int8_t>(gTradeStepD3040C);
            const std::int32_t x = std::bit_cast<std::int16_t>(
                static_cast<std::uint16_t>(moveWindow->elements[8].x));
            Sub_4B0130(moveWindow, 8, x - moveStep, 28);
            const std::uint8_t oldStep = gTradeStepD3040C;
            WindowInstance* const firstWindow = gCurrentWindow;
            gTradeStepD3040C = static_cast<std::uint8_t>(oldStep - 1);
            const auto* first = static_cast<WindowGraphicData*>(firstWindow->elements[11].data);
            std::int32_t alpha = static_cast<std::int32_t>(first->color >> 24) + 10;
            if (alpha > 255) {
                alpha = 255;
            }
            Sub_4B01B0(firstWindow, 11,
                (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
            WindowInstance* const secondWindow = gCurrentWindow;
            const auto* second = static_cast<WindowGraphicData*>(secondWindow->elements[14].data);
            alpha = static_cast<std::int32_t>(second->color >> 24) - 10;
            if (alpha < 0) {
                alpha = 0;
            }
            Sub_4B01B0(secondWindow, 14,
                (static_cast<std::uint32_t>(alpha) << 24) | 0x00FFFFFFu);
        } else {
            Sub_4B01B0(window, 11, 0xFFFFFFFFu);
            Sub_4B0180(gCurrentWindow, 8, 1);
            Sub_4B0130(gCurrentWindow, 8, 15, 28);
            Sub_4B0180(gCurrentWindow, 14, 0);
            gCurrentWindow->elements[2].visible = 1;
            gCurrentWindow->elements[3].visible = 0;
            gCurrentWindow->elements[4].visible = 0;
            gTradePhaseCAFEC4 = 0;
        }
        break;
    }
    case 3:
        if (gTradeValueD2E2B0 <= 0) {
            WindowInstance* const window = gCurrentWindow;
            gTradePhaseCAFEC4 = 2;
            Sub_4B0180(window, 11, 1);
            Sub_4B0130(gCurrentWindow, 11, 287, 28);
            Sub_4B01B0(gCurrentWindow, 11, 0x00FFFFFFu);
            gTradeStepD3040C = 23;
        }
        break;
    default:
        break;
    }
    gTradePreviewCC2E64 = -1;
    return 1;
}

std::int32_t Sub_4B08D0(const std::int32_t)
{
    std::int32_t preview = gTradePreviewCC2E64;
    if (preview >= 0) {
        if (preview < 300) {
            const bool ownItem = preview < 100;
            if (!ownItem) {
                preview -= 200;
                gTradePreviewCC2E64 = preview;
            }
            if (preview >= 0) {
                const std::int32_t page = std::bit_cast<std::int8_t>(gTradeTooltipPageCB10D4);
                if (ownItem) {
                    static_cast<void>(Sub_48B1A0(preview, page));
                } else {
                    static_cast<void>(Sub_48B450(0, preview, page));
                }
            }
        } else {
            preview -= 300;
            gTradePreviewCC2E64 = preview;
            // 原4B0970在异常分类>=600时读未初始化字节；不补零或指定分支。
            std::uint8_t ownPet;
            if (preview < 100) {
                ownPet = 1;
            } else if (preview < 300) {
                ownPet = 0;
                preview -= 200;
                gTradePreviewCC2E64 = preview;
            }
            if (preview >= 0) {
                const bool left = platform::legacy_crypt_int::Less_401DB0(
                    &platform::gLegacyMouseState.mouseX, 320);
                const std::int32_t x = left ? 413 : 0;
                const std::uint8_t index = static_cast<std::uint8_t>(gTradePreviewCC2E64);
                if (ownPet != 0) {
                    static_cast<void>(Sub_4B3BF0(x, 0, index));
                } else {
                    static_cast<void>(Sub_4B4680(x, 0, 0, index));
                }
            }
        }
    }
    DrawCurrentWindow_4729B0();
    return 1;
}

void Sub_4AAB70(
    const std::int16_t x, const std::int16_t y, const std::int32_t index,
    const std::uint8_t textLayer)
{
    world::detail::PetRuntimeRecord& pet = world::detail::gPetRuntimeF633F8[index];
    if (pet.used == 0) {
        return;
    }
    constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{138});
    static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
    static_cast<void>(render::DrawText_456780(
        0, 0, textLayer, 13, 0, input::LegacyText593850(), 0, 0));
    const char* const name =
        std::strlen(pet.name6BD.data()) != 0 ? pet.name6BD.data() : pet.name6AC.data();
    static_cast<void>(render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 45)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 5)),
        textLayer, 13, color, name, 0, 0));
    char buffer[256];
    const auto drawNumber = [&](const std::int32_t value, const char* const format,
                                const std::int32_t dx, const std::int32_t dy) {
        std::sprintf(buffer, format, value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + dx)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + dy)),
            textLayer, 13, color, buffer, 0, 0));
    };
    drawNumber(pet.value08, "%3d", 35, 22);
    const std::int16_t tribe = std::bit_cast<std::int16_t>(pet.value04);
    if (tribe >= 0 && tribe < 20) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 100)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 22)),
            textLayer, 13, color, gTribeNames57CB54[tribe], 0, 0));
    }
    drawNumber(platform::legacy_crypt_int::Read_401EC0(&pet.values0C[1]), "%4d", 210, 5);
    drawNumber(platform::legacy_crypt_int::Read_401EC0(&pet.values0C[3]), "%4d", 210, 22);
    drawNumber(pet.values70[0], "%3d", 95, 40);
    drawNumber(pet.values70[1], "%3d", 155, 40);
    drawNumber(pet.values70[2], "%3d", 215, 40);
    drawNumber(pet.values70[3], "%3d", 95, 55);
    drawNumber(pet.values70[4], "%3d", 155, 55);
    drawNumber(pet.values70[10], "%3d", 215, 55);
    drawNumber(pet.value6A4, "%2d", 45, 55);
}

void Sub_4AB400(
    const std::int16_t x, const std::int16_t y, const std::int32_t index,
    const std::uint8_t textLayer)
{
    const char* const record = gPetStorageD64E18.data() + index * 0x600;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return;
    }
    constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{138});
    static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
    static_cast<void>(render::DrawText_456780(
        0, 0, textLayer, 13, color, input::LegacyText593850(), 0, 0));
    static_cast<void>(render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 45)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 5)),
        textLayer, 13, color, record + 0x5D4, 0, 0));
    char buffer[256];
    const auto drawNumber = [&](const std::size_t offset, const char* const format,
                                const std::int32_t dx, const std::int32_t dy) {
        std::int32_t value;
        std::memcpy(&value, record + offset, sizeof(value));
        std::sprintf(buffer, format, value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + dx)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + dy)),
            textLayer, 13, color, buffer, 0, 0));
    };
    drawNumber(0x08, "%3d", 35, 22);
    std::int16_t tribe;
    std::memcpy(&tribe, record + 2, sizeof(tribe));
    if (tribe >= 0 && tribe < 20) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 100)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 22)),
            textLayer, 13, color, gTribeNames57CB54[tribe], 0, 0));
    }
    drawNumber(0x10, "%4d", 210, 5);
    drawNumber(0x18, "%4d", 210, 22);
    drawNumber(0x1C, "%3d", 95, 40);
    drawNumber(0x20, "%3d", 155, 40);
    drawNumber(0x24, "%3d", 215, 40);
    drawNumber(0x28, "%3d", 95, 55);
    drawNumber(0x2C, "%3d", 155, 55);
    drawNumber(0x44, "%3d", 215, 55);
    drawNumber(0x58, "%2d", 45, 55);
}

std::int32_t Sub_4EA670(const std::int32_t index)
{
    return world::detail::Sub_52E180(index, 25600) != 0 ? 1 : 0;
}

std::int32_t Sub_48D540(
    const std::int16_t x, const std::int16_t y, const std::int32_t index,
    const std::uint8_t textLayer, const std::uint8_t graphicLayer, const std::uint32_t color)
{
    render::GraphicDescriptor16 descriptor {};
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const auto originX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 150));
    const auto originY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y - 24));
    world::detail::PetRuntimeRecord& pet = world::detail::gPetRuntimeF633F8[index];
    if (pet.used != 0) {
        static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
        static_cast<void>(render::DrawText_456780(
            0, 0, textLayer, 12, 0, input::LegacyText593850(), 0, 0));
        const auto drawText = [&](const char* const text, const std::int32_t dx,
                                  const std::int32_t dy) {
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(originX + dx)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(originY + dy)),
                textLayer, 12, 0, text, 0, 0));
        };
        const char* const name =
            std::strlen(pet.name6BD.data()) != 0 ? pet.name6BD.data() : pet.name6AC.data();
        drawText(name, 8, 6);
        char buffer[256];
        std::sprintf(buffer, "%3d", pet.value08);
        drawText(buffer, 141, 6);
        const std::int16_t tribe = std::bit_cast<std::int16_t>(pet.value04);
        if (tribe >= 0 && tribe < 20) {
            drawText(gTribeNames57CB54[tribe], 118, 21);
        }
        const std::int32_t maxHp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[1]);
        const std::int32_t hp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[0]);
        std::sprintf(buffer, "%6d/%6d", hp, maxHp);
        drawText(buffer, 37, 19);
        const std::int32_t maxFp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[3]);
        const std::int32_t fp = platform::legacy_crypt_int::Read_401EC0(&pet.values0C[2]);
        std::sprintf(buffer, "%6d/%6d", fp, maxFp);
        drawText(buffer, 37, 32);
        if (Sub_4EA670(index) == 1) {
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
    return login::detail::DrawGraphic(
        originX + 150, originY + 24, graphicLayer, 243140, 0, &descriptor);
}

std::int32_t Sub_4B0F20(
    const std::int16_t x, const std::int16_t y, const std::int8_t side,
    const std::int32_t index, const std::uint8_t textLayer, const std::uint8_t graphicLayer,
    const render::GraphicDescriptor16* const descriptor)
{
    const char* const record = gTradePetsC9C108.data() + (index + side * 5) * 0x5FC;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used != 0) {
        constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{138});
        static_cast<void>(render::ClearTextQueueTail_456D50(textLayer));
        static_cast<void>(render::DrawText_456780(
            0, 0, textLayer, 8, color, input::LegacyText593850(), 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 45)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 5)),
            textLayer, 8, color, record + 0x5D8, 0, 0));
        char buffer[256];
        const auto drawNumber = [&](const std::size_t offset, const char* const format,
                                    const std::int32_t dx, const std::int32_t dy) {
            std::int32_t value;
            std::memcpy(&value, record + offset, sizeof(value));
            std::sprintf(buffer, format, value);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + dx)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + dy)),
                textLayer, 8, color, buffer, 0, 0));
        };
        drawNumber(0x0C, "%3d", 35, 22);
        std::int16_t tribe;
        std::memcpy(&tribe, record + 4, sizeof(tribe));
        if (tribe >= 0 && tribe < 20) {
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 100)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 22)),
                textLayer, 8, color, gTribeNames57CB54[tribe], 0, 0));
        }
        drawNumber(0x14, "%4d", 210, 5);
        drawNumber(0x1C, "%4d", 210, 22);
        drawNumber(0x20, "%3d", 95, 40);
        drawNumber(0x24, "%3d", 155, 40);
        drawNumber(0x28, "%3d", 215, 40);
        drawNumber(0x2C, "%3d", 95, 55);
        drawNumber(0x30, "%3d", 155, 55);
        drawNumber(0x34, "%3d", 215, 55);
        drawNumber(0x5C, "%2d", 45, 55);
    }
    return render::DrawGraphic3_520970(
        x, y, graphicLayer, 243141, 0, 1, 0, descriptor);
}

std::int32_t Sub_4B1260(
    WindowInstance* const window, const std::int32_t index, const std::int32_t state,
    const std::int32_t* const backgroundImages, std::uint8_t* const tab,
    const std::int32_t scrollElement, std::int32_t* const scrollValue,
    const std::int32_t numericElement, const std::int32_t maximum,
    std::int32_t itemCount, std::int32_t petCount, std::int32_t amount,
    render::GraphicDescriptor16* const descriptor, const WindowTradePanelCallback callback)
{
    WindowElement* const elements = window->elements;
    const std::int16_t x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(elements[index].x) + static_cast<std::uint16_t>(window->x)));
    const std::int16_t y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        static_cast<std::uint16_t>(elements[index].y) + static_cast<std::uint16_t>(window->y)));
    if ((state & 0x21) != 0) {
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        *scrollValue = Sub_4743B0(&elements[scrollElement], *scrollValue, 2, wheel);
    }
    WindowElement* const scroll = &window->elements[scrollElement];
    if ((static_cast<const WindowMouseData*>(scroll->data)->flags & 1u) != 0) {
        *scrollValue = Sub_474270(scroll, 2);
    }
    static constexpr std::array<std::array<std::int16_t, 4>, 5> rectangles54F508 {{
        {12, 240, 80, 15}, {97, 240, 80, 15}, {182, 240, 80, 15},
        {252, 28, 11, 11}, {252, 220, 11, 11},
    }};
    std::int32_t selected = -1;
    if ((state & 1) != 0) {
        for (std::int32_t part = 0; part < 5; ++part) {
            const auto& rectangle = rectangles54F508[part];
            if (Sub_4B1900(x + rectangle[0], y + rectangle[1], rectangle[2], rectangle[3])) {
                selected = part;
            }
        }
    }
    const std::int32_t result = state & 2;
    if (result != 0 && selected >= 0 && selected <= 2) {
        static_cast<void>(audio::PlaySe_52C450(
            std::bit_cast<std::int8_t>(*tab) == selected ? 55 : 51, 320));
        *tab = static_cast<std::uint8_t>(selected);
        static_cast<void>(Sub_4B09E0(0, 2));
    }
    if ((state & 0x400) != 0 && *tab == 1) {
        if (selected == 3) {
            if (*scrollValue > 0) {
                --*scrollValue;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            static_cast<void>(Sub_4742C0(&window->elements[scrollElement], 2, *scrollValue));
        } else if (selected == 4) {
            if (*scrollValue < 2) {
                ++*scrollValue;
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            static_cast<void>(Sub_4742C0(&window->elements[scrollElement], 2, *scrollValue));
        }
    }
    std::uint8_t numericResult = 0;
    if (numericElement >= 0 && *tab == 2) {
        std::uint32_t color;
        std::memcpy(&color, descriptor->bytes.data() + 8, sizeof(color));
        const std::uint8_t flags =
            input::gCurrentInputData == &input::gInputContextCC21E8 ? 3 : 1;
        numericResult = Sub_474550(
            window, numericElement, state, maximum, 223, gTradeNumberHints5805FC.data(),
            flags, color);
    }
    std::int32_t displayAmount; // 原回调负责写入，不为无回调的异常路径补零。
    if (callback != nullptr) {
        // 仅适配已证不消费该参数的-1哨兵；正索引仍借用同一W32，见窗口SPEC。
        std::uint32_t* const valueSlot =
            numericElement == -1 ? nullptr : &window->elements[numericElement].state;
        const std::int32_t scrollSnapshot = *scrollValue;
        const std::int32_t mouseY =
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
        const std::int32_t mouseX =
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
        callback(x, y, mouseX, mouseY, state, tab, scrollSnapshot, numericResult, valueSlot,
                 &itemCount, &petCount, &amount, &displayAmount, descriptor);
    }
    std::int32_t imageNo = *tab == 0 ? 243028 : (selected == 0 ? 243029 : 243027);
    static_cast<void>(render::DrawGraphic3_520970(
        x + 12, y + 240, 223, imageNo, 0, 1, 0, descriptor));
    imageNo = *tab == 1 ? 243022 : (selected == 1 ? 243023 : 243021);
    static_cast<void>(render::DrawGraphic3_520970(
        x + 97, y + 240, 223, imageNo, 0, 1, 0, descriptor));
    imageNo = *tab == 2 ? 243025 : (selected == 2 ? 243026 : 243024);
    static_cast<void>(render::DrawGraphic3_520970(
        x + 182, y + 240, 223, imageNo, 0, 1, 0, descriptor));
    window->elements[scrollElement].visible = 0;
    window->elements[scrollElement - 1].visible = 0;
    if (*tab == 1) {
        WindowElement* const previous = &window->elements[scrollElement - 1];
        const std::int32_t drawY = window->y + previous->y;
        const std::int32_t drawX = window->x + previous->x;
        static_cast<void>(render::DrawGraphic3_520970(
            drawX, drawY, 223, 243096, 0, 1, 0, descriptor));
        imageNo = selected == 3 ? ((state & 8) != 0 ? 243091 : 243092) : 243090;
        static_cast<void>(render::DrawGraphic3_520970(
            x + 252, y + 28, 223, imageNo, 0, 1, 0, descriptor));
        imageNo = selected == 4 ? ((state & 8) != 0 ? 243094 : 243095) : 243093;
        static_cast<void>(render::DrawGraphic3_520970(
            x + 252, y + 220, 223, imageNo, 0, 1, 0, descriptor));
        window->elements[scrollElement].visible = 1;
        window->elements[scrollElement - 1].visible = 1;
    }
    char buffer[52];
    if (*tab == 2) {
        std::sprintf(buffer, "%9d", displayAmount);
        Sub_4771B0(x + 179, y + 88, buffer, 3, 0, 1, 223);
    }
    std::sprintf(buffer, "%3d", itemCount);
    Sub_4771B0(x + 71, y + 258, buffer, 3, 0, 1, 223);
    std::sprintf(buffer, "%3d", petCount);
    Sub_4771B0(x + 156, y + 258, buffer, 3, 0, 1, 223);
    std::sprintf(buffer, "%10d", amount);
    Sub_4771B0(x + 241, y + 258, buffer, 3, 0, 1, 223);
    imageNo = backgroundImages[std::bit_cast<std::int8_t>(*tab)];
    static_cast<void>(render::DrawGraphic3_520970(
        x, y, 223, imageNo, 0, 1, 0, descriptor));
    return result;
}

void Sub_48BF30(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (message != 0) {
        return;
    }
    const std::int32_t kind = Sub_48BB70(sourceIndex);
    if (kind == 1) {
        if (world::detail::gPetRuntimeF633F8[sourceIndex].used == 0) {
            static_cast<void>(Sub_4763E0());
        }
    } else if (kind == 2) {
        std::uint16_t used;
        std::memcpy(&used, gPetStorageD64E18.data() + (sourceIndex - 100) * 0x600, sizeof(used));
        if (used == 0) {
            static_cast<void>(Sub_4763E0());
        }
    }
    const std::int32_t clickMode = gPetDragModeD2EA1C;
    const auto clicked = static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons);
    if (clickMode == 0) {
        if ((clicked & 2u) != 0) {
            static_cast<void>(Sub_4763E0());
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX)));
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY)));
            static_cast<void>(Sub_476490(3, sourceIndex, x, y, Sub_48BF30));
        }
    } else {
        if ((clicked & 1u) != 0) {
            static_cast<void>(Sub_4763E0());
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX)));
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY)));
            static_cast<void>(Sub_476490(3, sourceIndex, x, y, Sub_48BF30));
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            CancelWindowDrag_476410();
        }
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t drawMode = gPetDragModeD2EA1C;
    if (drawMode == 1) {
        const std::int32_t drawKind = Sub_48BB70(sourceIndex);
        if (drawKind == 1) {
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                platform::legacy_crypt_int::Subtract_401E60(&platform::gLegacyMouseState.mouseY, 35)));
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                platform::legacy_crypt_int::Subtract_401E60(&platform::gLegacyMouseState.mouseX, 120)));
            Sub_4AAB70(x, y, sourceIndex, 5);
        } else if (drawKind == 2) {
            const std::int32_t slot = sourceIndex - 100;
            const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                platform::legacy_crypt_int::Subtract_401E60(&platform::gLegacyMouseState.mouseY, 35)));
            const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
                platform::legacy_crypt_int::Subtract_401E60(&platform::gLegacyMouseState.mouseX, 120)));
            Sub_4AB400(x, y, slot, 5);
        } else {
            return;
        }
        const std::int32_t y =
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
        const std::int32_t x =
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
        static_cast<void>(login::detail::DrawGraphic(x, y, 230, 243141, 0, &descriptor));
    } else if (drawMode == 0 && Sub_48BB70(sourceIndex) == 1) {
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY)));
        static_cast<void>(Sub_48D540(x, y, sourceIndex, 5, 230, color));
    }
}

std::int32_t Sub_48C130(const std::int32_t sourceIndex, const std::int32_t mode)
{
    gPetDragModeD2EA1C = mode;
    return Sub_475DA0(3, sourceIndex, Sub_48BF30);
}

void Sub_48BC90(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (message != 0) {
        if (message == 2 && gItemDragModeCC4C9C != 0
            && (world::detail::ReadPlayerActionFlags() & 0x80u) == 0
            && world::detail::Sub_440B50() == 0) {
            const std::int32_t group =
                std::bit_cast<std::int8_t>(gValue57DA28) > 0 && sourceIndex < 6
                ? Sub_4C71B0(&gItemSelectorCF3B10) + 1 : 0;
            const std::int32_t y =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t x =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_510CE0(
                platform::gConnectionSocket, x, y, group, sourceIndex));
        }
        return;
    }
    const std::int32_t kind = Sub_48BB30(sourceIndex);
    if (kind == 1) {
        const world::detail::PlayerItemRecord* item;
        if (std::bit_cast<std::int8_t>(gValue57DA28) > 0) {
            item = &world::detail::gPetRuntimeF633F8[
                Sub_4C71B0(&gItemSelectorCF3B10)].items6E4[sourceIndex];
        } else {
            item = &world::detail::gPlayerItemsFDA8A4[sourceIndex];
        }
        if (item->used == 0) {
            static_cast<void>(Sub_4763E0());
        }
    } else if (kind == 2) {
        if (world::detail::gPlayerItemsFDA8A4[sourceIndex].used == 0) {
            static_cast<void>(Sub_4763E0());
        }
    } else if (kind == 3) {
        std::uint16_t used;
        std::memcpy(
            &used, gItemDetailStorageCC4D58.data() + (sourceIndex - 100) * 0xC44, sizeof(used));
        if (used == 0) {
            static_cast<void>(Sub_4763E0());
        }
    }
    if ((platform::gLegacyMouseState.edgeButtons & 1u) != 0) {
        static_cast<void>(Sub_4763E0());
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY)));
        static_cast<void>(Sub_476490(2, sourceIndex, x, y, Sub_48BC90));
    }
    if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
        CancelWindowDrag_476410();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t drawKind = Sub_48BB30(sourceIndex);
    std::int32_t imageNo;
    if (drawKind == 1) {
        if (std::bit_cast<std::int8_t>(gValue57DA28) > 0) {
            imageNo = world::detail::gPetRuntimeF633F8[
                Sub_4C71B0(&gItemSelectorCF3B10)].items6E4[sourceIndex].imageNo;
        } else {
            imageNo = world::detail::gPlayerItemsFDA8A4[sourceIndex].imageNo;
        }
    } else if (drawKind == 2) {
        imageNo = world::detail::gPlayerItemsFDA8A4[sourceIndex].imageNo;
    } else if (drawKind == 3) {
        std::memcpy(&imageNo,
            gItemDetailStorageCC4D58.data() + (sourceIndex - 100) * 0xC44 + 4, sizeof(imageNo));
    } else {
        return;
    }
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
    static_cast<void>(login::detail::DrawGraphic(x, y, 230, imageNo, 0, &descriptor));
}

std::int32_t Sub_48BF10(const std::int32_t sourceIndex, const std::int32_t mode)
{
    gItemDragModeCC4C9C = mode;
    return Sub_475DA0(2, sourceIndex, Sub_48BC90);
}

void Sub_4B1940(
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const render::GraphicDescriptor16* const descriptor)
{
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(dimmed.bytes.data() + 8, &color, sizeof(color));
    dimmed.bytes[13] = 4;
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && Sub_475D60() != 2) {
        events &= 0x9Fu;
    }
    if (gTradePhaseCAFEC4 != 0) {
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t source = Sub_475D90();
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const std::int32_t left = x + 50 * (position % 5) + 13;
            const std::int32_t top = y + 52 * (position / 5) + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (source != position + 8) {
                Sub_4886E0(43, 11, source, position + 8);
            }
            break;
        }
        Sub_476500();
    }
    if (*tab != 0) {
        return;
    }
    std::int32_t hovered = -1;
    const bool hover = (events & 0x21u) != 0;
    if (hover) {
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const std::int32_t left = x + 50 * (position % 5) + 13;
            const std::int32_t top = y + 52 * (position / 5) + 26;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position + 8;
                break;
            }
        }
        if (hovered >= 0 && gTradePhaseCAFEC4 == 0
            && world::detail::gPlayerItemsFDA8A4[hovered].used != 0) {
            if ((world::detail::gPlayerItemsFDA8A4[hovered].flags & 1u) == 0) {
                std::strcpy(gHoverTextD68AE8.data(),
                    "\xA6\xB9\xAA\xAB\xAB\x7E\xB5\x4C\xAA\x6B\xA5\xE6\xA9\xF6\xA1\x43");
            } else if (gTradeItemMarksD2D61C[hovered] == 0) {
                std::strcpy(gHoverTextD68AE8.data(),
                    "\xA5\xCE\xB7\xC6\xB9\xAB\xA9\xEC\xA9\xF1\xA4\xE8\xA6\xA1\xA1\x41"
                    "\xB5\x6E\xBF\xFD\xA6\xB9\xB5\xA7\xAA\xAB\xAB\x7E\xA5\xE6\xA9\xF6\xA1\x43");
            }
        }
    }
    if (gTradeValueD2EA20 == 0) {
        if (gTradeHoverSlotCB0A4C != hovered) {
            gTradeTooltipPageCB10D4 = 0;
        }
        gTradeHoverSlotCB0A4C = hovered;
    }
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0
        && (Sub_475D40() == 43 || Sub_475D40() == 0)) {
        if (world::detail::gPlayerItemsFDA8A4[hovered].used != 0
            && (world::detail::gPlayerItemsFDA8A4[hovered].flags & 1u) != 0
            && gTradeItemMarksD2D61C[hovered] == 0) {
            std::int32_t target = 0;
            while (target < 20 && Sub_4AFFE0(target) >= 0) {
                ++target;
            }
            if (target < 20) {
                static_cast<void>(Sub_4AFF70(target));
                static_cast<void>(Sub_4AFF30(target, hovered));
                *tab = 0;
            }
            if (gTradeValueC8AF81 != 0) {
                gTradeValueC8AF81 = 0;
            }
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        if (gTradePhaseCAFEC4 == 0 && (events & 2u) != 0
            && world::detail::gPlayerItemsFDA8A4[hovered].used != 0
            && gTradeItemMarksD2D61C[hovered] == 0) {
            static_cast<void>(Sub_48BF10(hovered, 1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((events & 4u) != 0 && world::detail::gPlayerItemsFDA8A4[hovered].used != 0) {
            const std::int32_t pages = world::detail::gPlayerItemsFDA8A4[hovered].pages;
            gTradeTooltipPageCB10D4 = static_cast<std::uint8_t>(gTradeTooltipPageCB10D4 + 1);
            if (std::bit_cast<std::int8_t>(gTradeTooltipPageCB10D4) >= pages) {
                gTradeTooltipPageCB10D4 = 0;
            }
        }
    }
    if (Sub_475D60() == 2) {
        const std::int32_t source = Sub_475D90();
        if (Sub_475D40() == 43 && Sub_475D50() == 11) {
            const std::int32_t left = x + 50 * ((source - 8) % 5) + 13;
            const std::int32_t top = y + 52 * ((source - 8) / 5) + 26;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, 254, 0);
        }
    }
    char buffer[256];
    if (hover && hovered >= 0) {
        const std::int32_t left = x + 50 * ((hovered - 8) % 5) + 13;
        const std::int32_t top = y + 52 * ((hovered - 8) / 5) + 26;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        const world::detail::PlayerItemRecord& item = world::detail::gPlayerItemsFDA8A4[hovered];
        if (item.used != 0) {
            const std::size_t memoLength = std::strlen(item.memo.data());
            gTradePreviewCC2E64 = hovered;
            if (memoLength != 0 && (item.flags & 0x100u) != 0) {
                std::sprintf(buffer, "%s[kok]", item.name.data());
                std::strcpy(gHoverTextD68AE8.data(), buffer);
            } else if ((item.flags & 0x200u) != 0) {
                std::sprintf(buffer, "%s[han]", item.memo.data());
                std::strcpy(gHoverTextD68AE8.data(), buffer);
            }
        }
        gTradeValueD2EA20 = 0;
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
    dimmed = *descriptor;
    dimmed.bytes[11] >>= 1;
    for (std::int32_t position = 0; position < 20; ++position) {
        const std::int32_t slot = position + 8;
        const world::detail::PlayerItemRecord& item = world::detail::gPlayerItemsFDA8A4[slot];
        if (item.used == 0) {
            continue;
        }
        const std::int32_t centerX = x + 50 * (position % 5) + 37;
        const std::int32_t centerY = y + 52 * (position / 5) + 51;
        if (std::bit_cast<std::int8_t>(gTradeItemMarksD2D61C[slot]) > 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(centerX - 21), static_cast<std::uint16_t>(centerY - 21),
                static_cast<std::uint16_t>(centerX + 21), static_cast<std::uint16_t>(centerY + 21),
                223, 251, 0);
        } else if (occupancy[slot] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(centerX - 24), static_cast<std::uint16_t>(centerY - 24),
                static_cast<std::uint16_t>(centerX + 24), static_cast<std::uint16_t>(centerY + 24),
                223, 249, 0);
        }
        if ((item.flags & 1u) == 0) {
            static_cast<void>(login::detail::DrawGraphic(centerX, centerY, 223, 23040, 0, nullptr));
        }
        const render::GraphicDescriptor16* const mode =
            std::bit_cast<std::int8_t>(gTradeItemMarksD2D61C[slot]) > 0 || occupancy[slot] != 0
            ? &dimmed : descriptor;
        static_cast<void>(login::detail::DrawGraphic(centerX, centerY, 223, item.imageNo, 0, mode));
        const std::int32_t count = item.count;
        if (count > 0) {
            std::sprintf(buffer, "%3d", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerX - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerY + 7)),
                3, 1, 1, buffer, 0, 0));
        }
    }
}

void Sub_4B2060(
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const std::int32_t scroll, const render::GraphicDescriptor16* const descriptor)
{
    std::int16_t rowY[3] {26, 95, 164};
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && Sub_475D60() != 3) {
        events &= 0x9Fu;
    }
    if (gTradePhaseCAFEC4 != 0) {
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t source = Sub_475D90();
        // 原三坐标却扫描五次，沿既定异常边界，不补坐标或模拟栈残值。
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t top = y + rowY[row];
            if (dropX >= x + 251 || x + 11 > dropX || dropY >= top + 69 || top > dropY) {
                continue;
            }
            const std::int32_t position = scroll + row;
            if (position >= 0) {
                const std::int32_t target =
                    std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index);
                if (gTradePetMarksCF3AFC[target] == 0) {
                    Sub_48BB90(43, 11, source, target);
                }
            }
            break;
        }
        Sub_476500();
    }
    if (*tab != 1) {
        return;
    }
    std::int32_t hovered = -1;
    const bool hover = (events & 0x21u) != 0;
    if (hover) {
        for (std::int32_t row = 0; row < 3; ++row) {
            const std::int32_t top = y + rowY[row];
            if (login::detail::Sub_5049A0(x + 11, top, x + 251, top + 69, -1) != 0) {
                hovered = scroll + row;
                break;
            }
        }
    }
    std::int32_t petIndex = -1;
    if (hovered >= 0) {
        petIndex = std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[hovered].index);
        if (petIndex >= 0) {
            std::strcpy(gHoverTextD68AE8.data(),
                "\xA5\xCE\xB7\xC6\xB9\xAB\xA9\xEC\xA9\xF1\xA4\xE8\xA6\xA1\xA1\x41"
                "\xB5\x6E\xBF\xFD\xA6\xB9\xB5\xA7\xC3\x64\xAA\xAB\xA5\xE6\xA9\xF6\xA1\x43");
            if ((platform::gLegacyMouseState.edgeButtons & 0x10u) != 0
                && (Sub_475D40() == 43 || Sub_475D40() == 0)) {
                if (world::detail::gPetRuntimeF633F8[petIndex].used != 0
                    && gTradePetMarksCF3AFC[petIndex] == 0) {
                    std::int32_t target = 0;
                    while (target < 5 && Sub_4B00F0(target) >= 0) {
                        ++target;
                    }
                    if (target < 5) {
                        static_cast<void>(Sub_4B0090(target));
                        static_cast<void>(Sub_4B0060(target, petIndex));
                        *tab = 1;
                    }
                    if (gTradeValueC8AF81 != 1) {
                        gTradeValueC8AF81 = 1;
                    }
                } else {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                }
            }
        }
    }
    if ((events & 1u) != 0 && petIndex >= 0 && gTradePhaseCAFEC4 == 0
        && (events & 2u) != 0 && world::detail::gPetRuntimeF633F8[petIndex].used != 0
        && gTradePetMarksCF3AFC[petIndex] == 0) {
        static_cast<void>(Sub_48C130(petIndex, 1));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (Sub_475D60() == 3) {
        const std::int32_t source = Sub_475D90();
        if (Sub_475D40() == 43 && Sub_475D50() == 11) {
            const std::int32_t row = Sub_48BC60(source) - scroll;
            if (row >= 0 && row < 3) {
                const std::int32_t top = y + rowY[row];
                login::detail::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                    223, 254, 0);
            }
        }
    }
    if (hover) {
        const std::int32_t row = hovered - scroll;
        if (row >= 0 && row < 3) {
            const std::int32_t top = y + rowY[row];
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                223, login::detail::gValueD69768, 0);
            if (world::detail::gPetRuntimeF633F8[petIndex].used != 0) {
                gTradePreviewCC2E64 = petIndex + 300;
            }
        }
    }
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t slot =
            std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[scroll + row].index);
        const std::int32_t left = x + 11;
        const std::int32_t top = y + rowY[row];
        if (std::bit_cast<std::int8_t>(gTradePetMarksCF3AFC[slot]) > 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 238), static_cast<std::uint16_t>(top + 67),
                223, 251, 0);
        }
        const std::int32_t current =
            std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[scroll + row].index);
        Sub_4AAB70(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(left)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(top)), current, 3);
        static_cast<void>(login::detail::DrawGraphic(
            left + 120, top + 35, 223, 243141, 0, descriptor));
    }
}

std::int32_t Sub_4B2480(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* const window = gCurrentWindow;
    const auto* const graphic = static_cast<const WindowGraphicData*>(window->elements[index].data);
    const std::uint32_t color = graphic->color;
    descriptor.bytes[13] = 4;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    static constexpr std::array<std::int32_t, 3> backgrounds54F530 {243143, 243145, 243144};
    const std::int32_t amount = world::detail::ReadPlayerValueFC();
    return Sub_4B1260(
        window, index, state, backgrounds54F530.data(), &gTradeValueD6742C,
        10, &gTradeScrollCF3B0C, 5, amount, 0, 0, amount, &descriptor, Sub_4B2500);
}

void Sub_4B2500(
    const std::int16_t x, const std::int16_t y,
    const std::int32_t mouseX, const std::int32_t mouseY,
    const std::int32_t state, std::uint8_t* const tab, const std::int32_t scroll,
    const std::uint8_t numericResult, std::uint32_t* const valueSlot,
    std::int32_t* const itemCount, std::int32_t* const petCount, std::int32_t*,
    std::int32_t* const displayAmount, render::GraphicDescriptor16* const descriptor)
{
    const bool confirmed = (numericResult & 2u) != 0;
    if (confirmed && gTradeValueC8AF81 != 2) {
        gTradeValueC8AF81 = 2;
    }
    Sub_4B1940(tab, x, y, mouseX, mouseY, state, descriptor);
    Sub_4B2060(tab, x, y, mouseX, mouseY, state, scroll, descriptor);
    if (confirmed) {
        gTradeValueD046D0 = std::bit_cast<std::int32_t>(*valueSlot);
        *valueSlot = 0;
    }
    *displayAmount = std::bit_cast<std::int32_t>(*valueSlot);
    std::int32_t items = 0;
    for (std::int32_t position = 0; position < 20; ++position) {
        std::uint16_t used;
        std::memcpy(&used, gTradeItemsD460D0.data() + (position + 20) * 0xC4C, sizeof(used));
        if (used != 0) {
            ++items;
        }
    }
    *itemCount = items;
    std::int32_t pets = 0;
    for (std::int32_t position = 0; position < 5; ++position) {
        std::uint16_t used;
        std::memcpy(&used, gTradePetsC9C108.data() + (position + 5) * 0x5FC, sizeof(used));
        if (used != 0) {
            ++pets;
        }
    }
    *petCount = pets;
}

void Sub_4B25D0(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (message != 0) {
        if (message == 2 || message == 4) {
            static_cast<void>(Sub_4AFF70(sourceIndex));
        }
        return;
    }
    if (Sub_4AFFE0(sourceIndex) < 0
        || world::detail::gPlayerItemsFDA8A4[Sub_4AFFE0(sourceIndex)].used == 0) {
        static_cast<void>(Sub_4763E0());
    }
    if ((platform::gLegacyMouseState.edgeButtons & 1u) != 0) {
        static_cast<void>(Sub_4763E0());
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY)));
        static_cast<void>(Sub_476490(6, sourceIndex, x, y, Sub_4B25D0));
    }
    if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
        CancelWindowDrag_476410();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t imageNo =
        world::detail::gPlayerItemsFDA8A4[Sub_4AFFE0(sourceIndex)].imageNo;
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
    static_cast<void>(login::detail::DrawGraphic(x, y, 230, imageNo, 0, &descriptor));
}

void Sub_4B2E00(
    const std::int32_t message, const std::int32_t, const std::int32_t sourceIndex)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (message != 0) {
        if (message == 2 || message == 4) {
            static_cast<void>(Sub_4B0090(sourceIndex));
        }
        return;
    }
    if (Sub_4B00F0(sourceIndex) < 0
        || world::detail::gPetRuntimeF633F8[Sub_4B00F0(sourceIndex)].used == 0) {
        static_cast<void>(Sub_4763E0());
    }
    if ((platform::gLegacyMouseState.edgeButtons & 1u) != 0) {
        static_cast<void>(Sub_4763E0());
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY)));
        static_cast<void>(Sub_476490(7, sourceIndex, x, y, Sub_4B2E00));
    }
    if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
        CancelWindowDrag_476410();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    const std::int32_t slot = Sub_4B00F0(sourceIndex);
    const auto petY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        platform::legacy_crypt_int::Subtract_401E60(&platform::gLegacyMouseState.mouseY, 35)));
    const auto petX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        platform::legacy_crypt_int::Subtract_401E60(&platform::gLegacyMouseState.mouseX, 120)));
    Sub_4AAB70(petX, petY, slot, 5);
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
    static_cast<void>(login::detail::DrawGraphic(x, y, 230, 243141, 0, &descriptor));
}

std::int32_t Sub_4B2710(const std::int32_t index)
{
    return Sub_475DA0(6, index, Sub_4B25D0);
}

std::int32_t Sub_4B2F50(const std::int32_t index)
{
    return Sub_475DA0(7, index, Sub_4B2E00);
}

void Sub_4B2730(
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const render::GraphicDescriptor16* const descriptor)
{
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && Sub_475D60() != 2 && Sub_475D60() != 6) {
        events &= 0x9Fu;
    }
    if (gTradePhaseCAFEC4 != 0) {
        events &= 0x9Fu;
    }
    if (*tab != 0) {
        if ((events & 0x40u) != 0) {
            const std::int32_t source = Sub_475D90();
            if (Sub_48BB30(source) == 2) {
                std::int32_t target = 0;
                while (target < 20 && Sub_4AFFE0(target) >= 0) {
                    ++target;
                }
                if (target < 20 && (world::detail::gPlayerItemsFDA8A4[source].flags & 1u) != 0) {
                    static_cast<void>(Sub_4AFF70(target));
                    static_cast<void>(Sub_4AFF30(target, source));
                    *tab = 0;
                }
            }
            Sub_476500();
        }
        if ((events & 0x20u) != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 273), static_cast<std::uint16_t>(y + 280),
                223, login::detail::gValueD69768, 0);
        }
        return;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t kind = Sub_475D60();
        const std::int32_t source = Sub_475D90();
        for (std::int32_t target = 0; target < 20; ++target) {
            const std::int32_t left = x + 50 * (target % 5) + 13;
            const std::int32_t top = y + 52 * (target / 5) + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (kind == 2) {
                if (Sub_48BB30(source) == 2
                    && (world::detail::gPlayerItemsFDA8A4[source].flags & 1u) != 0) {
                    static_cast<void>(Sub_4AFF70(target));
                    static_cast<void>(Sub_4AFF30(target, source));
                }
            } else if (kind == 6) {
                static_cast<void>(Sub_4AFFB0(source, target));
            }
            Sub_476500();
            break;
        }
    }
    std::int32_t hovered = -1;
    const bool hover = (events & 0x21u) != 0;
    if (hover) {
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            const std::int32_t left = x + 50 * (slot % 5) + 13;
            const std::int32_t top = y + 52 * (slot / 5) + 26;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = slot;
                break;
            }
        }
    }
    const std::uint8_t phase = gTradePhaseCAFEC4;
    if (phase == 0 && hovered >= 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xA5\xCE\xB7\xC6\xB9\xAB\xA9\xEC\xA9\xF1\xA4\xE8\xA6\xA1\xA1\x41"
            "\xA8\xFA\xAE\xF8\xA6\xB9\xB5\xA7\xAA\xAB\xAB\x7E\xA5\xE6\xA9\xF6\xA1\x43");
    }
    if (gTradeValueD2EA20 == 1) {
        if (gTradeHoverSlotC8AA78 != hovered) {
            gTradeTooltipPageCB10D4 = 0;
        }
        gTradeHoverSlotC8AA78 = hovered;
    }
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0
        && (phase != 0 || ((Sub_475D40() == 43 || Sub_475D40() == 0)
                           && Sub_4AFF70(hovered) < 0))) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    if ((events & 1u) != 0 && hovered >= 0 && Sub_4AFFE0(hovered) >= 0) {
        if (gTradePhaseCAFEC4 == 0 && (events & 2u) != 0) {
            static_cast<void>(Sub_4B2710(hovered));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((events & 4u) != 0) {
            gTradeTooltipPageCB10D4 = static_cast<std::uint8_t>(gTradeTooltipPageCB10D4 + 1);
            const std::int32_t slot = Sub_4AFFE0(hovered);
            if (std::bit_cast<std::int8_t>(gTradeTooltipPageCB10D4)
                >= world::detail::gPlayerItemsFDA8A4[slot].pages) {
                gTradeTooltipPageCB10D4 = 0;
            }
        }
    }
    if (Sub_475D60() == 6 && Sub_475D40() == 43 && Sub_475D50() == 8) {
        const std::int32_t source = Sub_475D90();
        if (Sub_4AFFE0(source) >= 0
            && world::detail::gPlayerItemsFDA8A4[Sub_4AFFE0(source)].used != 0) {
            const std::int32_t left = x + 50 * (source % 5) + 13;
            const std::int32_t top = y + 52 * (source / 5) + 26;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    char buffer[256];
    if (hover && hovered >= 0) {
        const std::int32_t left = x + 50 * (hovered % 5) + 13;
        const std::int32_t top = y + 52 * (hovered / 5) + 26;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        const std::int32_t slot = Sub_4AFFE0(hovered);
        if (slot >= 0) {
            const world::detail::PlayerItemRecord& item = world::detail::gPlayerItemsFDA8A4[slot];
            if (item.used != 0) {
                gTradePreviewCC2E64 = slot;
                if (std::strlen(item.memo.data()) != 0 && (item.flags & 0x100u) != 0) {
                    std::sprintf(buffer, "%s[kok]", item.name.data());
                    std::strcpy(gHoverTextD68AE8.data(), buffer);
                } else if ((item.flags & 0x200u) != 0) {
                    std::sprintf(buffer, "%s[han]", item.memo.data());
                    std::strcpy(gHoverTextD68AE8.data(), buffer);
                }
            }
        }
        gTradeValueD2EA20 = 1;
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        const std::int32_t slot = Sub_4AFFE0(position);
        if (slot >= 0 && world::detail::gPlayerItemsFDA8A4[slot].used != 0
            && (world::detail::gPlayerItemsFDA8A4[slot].flags & 1u) == 0) {
            static_cast<void>(Sub_4AFF70(position));
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
    for (std::int32_t position = 0; position < 20; ++position) {
        const std::int32_t slot = Sub_4AFFE0(position);
        if (slot < 0) {
            continue;
        }
        const world::detail::PlayerItemRecord& item = world::detail::gPlayerItemsFDA8A4[slot];
        if (item.used == 0) {
            continue;
        }
        const std::int32_t centerX = x + 50 * (position % 5) + 37;
        const std::int32_t centerY = y + 52 * (position / 5) + 51;
        if ((item.flags & 1u) == 0) {
            static_cast<void>(login::detail::DrawGraphic(
                centerX, centerY, 223, 23040, 0, nullptr));
        }
        static_cast<void>(login::detail::DrawGraphic(
            centerX, centerY, 223, item.imageNo, 0, descriptor));
        const std::int32_t count = item.count;
        if (count > 0) {
            std::sprintf(buffer, "%3d", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerX - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerY + 7)),
                3, 1, 1, buffer, 0, 0));
        }
    }
}

void Sub_4B2F70(
    std::uint8_t* const tab, const std::int16_t x, const std::int16_t y,
    const std::int32_t, const std::int32_t, const std::int32_t state,
    const std::int32_t scroll, const render::GraphicDescriptor16* const descriptor)
{
    std::int16_t rowY[3] {26, 95, 164};
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && Sub_475D60() != 3 && Sub_475D60() != 7) {
        events &= 0x9Fu;
    }
    if (gTradePhaseCAFEC4 != 0) {
        events &= 0x9Fu;
    }
    if (*tab != 1) {
        if ((events & 0x40u) != 0) {
            const std::int32_t source = Sub_475D90();
            if (Sub_48BB70(source) == 1) {
                std::int32_t target = 0;
                while (target < 5 && Sub_4B00F0(target) >= 0) {
                    ++target;
                }
                if (target < 5) {
                    static_cast<void>(Sub_4B0090(target));
                    static_cast<void>(Sub_4B0060(target, source));
                    *tab = 1;
                }
            }
            Sub_476500();
        }
        if ((events & 0x20u) != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
                static_cast<std::uint16_t>(x + 273), static_cast<std::uint16_t>(y + 280),
                223, login::detail::gValueD69768, 0);
        }
        return;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t kind = Sub_475D60();
        const std::int32_t source = Sub_475D90();
        // 原仅初始化三项却扫描五次；不补坐标或模拟x86栈残值，异常边界见SPEC。
        for (std::int32_t row = 0; row < 5; ++row) {
            const std::int32_t top = y + rowY[row];
            if (dropX >= x + 251 || x + 11 > dropX || dropY >= top + 69 || top > dropY) {
                continue;
            }
            const std::int32_t target = scroll + row;
            if (target >= 0) {
                if (kind == 3) {
                    if (Sub_48BB70(source) == 1) {
                        static_cast<void>(Sub_4B0090(target));
                        static_cast<void>(Sub_4B0060(target, source));
                    }
                } else if (kind == 7) {
                    static_cast<void>(Sub_4B00C0(source, target));
                }
                Sub_476500();
            }
            break;
        }
    }
    if (*tab != 1) {
        return;
    }
    std::int32_t hovered = -1;
    const bool hover = (events & 0x21u) != 0;
    if (hover) {
        for (std::int32_t row = 0; row < 3; ++row) {
            const std::int32_t top = y + rowY[row];
            if (login::detail::Sub_5049A0(x + 11, top, x + 251, top + 69, -1) != 0) {
                hovered = scroll + row;
                break;
            }
        }
    }
    const std::uint8_t phase = gTradePhaseCAFEC4;
    if (phase == 0 && hovered >= 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xA5\xCE\xB7\xC6\xB9\xAB\xA9\xEC\xA9\xF1\xA4\xE8\xA6\xA1\xA1\x41"
            "\xA8\xFA\xAE\xF8\xA6\xB9\xB5\xA7\xC3\x64\xAA\xAB\xA5\xE6\xA9\xF6\xA1\x43");
    }
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0
        && (phase != 0 || ((Sub_475D40() == 43 || Sub_475D40() == 0)
                           && Sub_4B0090(hovered) < 0))) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    if ((events & 1u) != 0 && hovered >= 0 && Sub_4B00F0(hovered) >= 0
        && gTradePhaseCAFEC4 == 0 && (events & 2u) != 0) {
        static_cast<void>(Sub_4B2F50(hovered));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
    }
    if (Sub_475D60() == 7 && Sub_475D40() == 43 && Sub_475D50() == 8) {
        const std::int32_t source = Sub_475D90();
        if (Sub_4B00F0(source) >= 0
            && world::detail::gPetRuntimeF633F8[Sub_4B00F0(source)].used != 0) {
            const std::int32_t row = source - scroll;
            if (row >= 0 && row < 3) {
                const std::int32_t top = y + rowY[row];
                login::detail::DrawGraphicHover(
                    static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                    223, 254, 0);
            }
        }
    }
    if (hover) {
        const std::int32_t row = hovered - scroll;
        if (row >= 0 && row < 3) {
            const std::int32_t top = y + rowY[row];
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x + 11), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(x + 251), static_cast<std::uint16_t>(top + 69),
                223, login::detail::gValueD69768, 0);
            const std::int32_t slot = Sub_4B00F0(hovered);
            if (slot >= 0 && world::detail::gPetRuntimeF633F8[slot].used != 0) {
                gTradePreviewCC2E64 = slot + 300;
            }
        }
    }
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t left = x + 11;
        const std::int32_t top = y + rowY[row];
        const std::int32_t slot = Sub_4B00F0(scroll + row);
        if (slot >= 0 && world::detail::gPetRuntimeF633F8[slot].used != 0) {
            Sub_4AAB70(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(left)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(top)), slot, 3);
            static_cast<void>(login::detail::DrawGraphic(
                left + 120, top + 35, 223, 243141, 0, descriptor));
        } else {
            static_cast<void>(render::DrawGraphic3_520970(
                left, top, 223, 243141, 0, 1, 0, descriptor));
        }
    }
}

std::int32_t Sub_4B3430(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* const window = gCurrentWindow;
    const auto* const graphic = static_cast<const WindowGraphicData*>(window->elements[index].data);
    const std::uint32_t color = graphic->color;
    descriptor.bytes[13] = 4;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    if (gTradeValueD2E2B0 == 2) {
        descriptor.bytes[11] >>= 1;
    }
    static constexpr std::array<std::int32_t, 3> backgrounds54F53C {244367, 244371, 244369};
    const std::int32_t amount = world::detail::ReadPlayerValueFC();
    return Sub_4B1260(
        window, index, state, backgrounds54F53C.data(), &gTradeValueC8AF81,
        7, &gTradeScrollCC4C90, -1, 0, 0, 0, amount, &descriptor, Sub_4B34C0);
}

void Sub_4B34C0(
    const std::int16_t x, const std::int16_t y,
    const std::int32_t mouseX, const std::int32_t mouseY, const std::int32_t state,
    std::uint8_t* const tab, const std::int32_t scroll,
    const std::uint8_t numericResult, std::uint32_t*,
    std::int32_t* const itemCount, std::int32_t* const petCount, std::int32_t* const amount,
    std::int32_t* const displayAmount, render::GraphicDescriptor16* const descriptor)
{
    if ((numericResult & 2u) != 0 && gTradeValueD6742C != 2) {
        gTradeValueD6742C = 2;
    }
    Sub_4B2730(tab, x, y, mouseX, mouseY, state, descriptor);
    Sub_4B2F70(tab, x, y, mouseX, mouseY, state, scroll, descriptor);
    *displayAmount = gTradeValueD046D0;
    std::int32_t items = 0;
    for (std::int32_t position = 0; position < 20; ++position) {
        if (Sub_4AFFE0(position) >= 0) {
            ++items;
        }
    }
    *itemCount = items;
    std::int32_t pets = 0;
    for (std::int32_t position = 0; position < 5; ++position) {
        if (Sub_4B00F0(position) >= 0) {
            ++pets;
        }
    }
    *petCount = pets;
    *amount = gTradeValueD046D0;
}

std::int32_t Sub_4B3580(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    WindowInstance* const window = gCurrentWindow;
    const auto* const graphic = static_cast<const WindowGraphicData*>(window->elements[index].data);
    const std::uint32_t color = graphic->color;
    descriptor.bytes[13] = 4;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    if (gTradeValueCC4224 == 2) {
        descriptor.bytes[11] >>= 1;
    }
    static constexpr std::array<std::int32_t, 3> backgrounds54F548 {244368, 244372, 244370};
    const std::int32_t amount = world::detail::ReadPlayerValueFC();
    return Sub_4B1260(
        window, index, state, backgrounds54F548.data(), &gTradeTabCC4D1A,
        13, &gTradeScrollD67430, -1, 0, 0, 0, amount, &descriptor, Sub_4B3610);
}

void Sub_4B3610(
    const std::int16_t x, const std::int16_t y, const std::int32_t, const std::int32_t,
    const std::int32_t state, std::uint8_t* const tab, const std::int32_t scroll,
    const std::uint8_t, std::uint32_t*,
    std::int32_t* const itemCount, std::int32_t* const petCount, std::int32_t* const amount,
    std::int32_t* const displayAmount, render::GraphicDescriptor16* const descriptor)
{
    const std::uint8_t page = *tab;
    char buffer[256];
    if (page == 0) {
        std::int32_t hovered = -1;
        if ((state & 1) != 0) {
            for (std::int32_t slot = 0; slot < 20; ++slot) {
                const std::int32_t left = x + 50 * (slot % 5) + 13;
                const std::int32_t top = y + 52 * (slot / 5) + 26;
                if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    hovered = slot;
                    std::uint16_t used;
                    std::memcpy(&used, gTradeItemsD460D0.data() + slot * 0xC4C, sizeof(used));
                    if (used != 0) {
                        std::strcpy(gHoverTextD68AE8.data(),
                            "\xA5\x48\xA6\xB9\xAA\xAB\xAB\x7E\xA6\x56\xB9\xEF\xA4\xE8"
                            "\xB4\xA3\xA5\x58\xA5\xE6\xA9\xF6\xA1\x43");
                    }
                    break;
                }
            }
        }
        if (gTradeValueD2EA20 == 2) {
            if (gTradeHoverSlotC8B314 != hovered) {
                gTradeTooltipPageCB10D4 = 0;
            }
            gTradeHoverSlotC8B314 = hovered;
        }
        if (hovered >= 0 && (state & 4) != 0) {
            const char* const record = gTradeItemsD460D0.data() + hovered * 0xC4C;
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                std::int32_t pages;
                std::memcpy(&pages, record + 0xC30, sizeof(pages));
                gTradeTooltipPageCB10D4 = static_cast<std::uint8_t>(gTradeTooltipPageCB10D4 + 1);
                if (std::bit_cast<std::int8_t>(gTradeTooltipPageCB10D4) >= pages) {
                    gTradeTooltipPageCB10D4 = 0;
                }
            }
        }
        if ((state & 1) != 0 && hovered >= 0) {
            const std::int32_t left = x + 50 * (hovered % 5) + 13;
            const std::int32_t top = y + 52 * (hovered / 5) + 25;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, login::detail::gValueD69768, 0);
            const char* const record = gTradeItemsD460D0.data() + hovered * 0xC4C;
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                const char* const memo = record + 0xC38;
                const std::size_t memoLength = std::strlen(memo);
                gTradePreviewCC2E64 = hovered + 200;
                bool showName = false;
                if (memoLength != 0) {
                    std::uint32_t flags;
                    std::memcpy(&flags, record + 0xC34, sizeof(flags));
                    showName = (flags & 0x100u) != 0;
                }
                if (showName) {
                    std::sprintf(buffer, "%s[kok]", record + 0x10);
                    std::strcpy(gHoverTextD68AE8.data(), buffer);
                } else {
                    std::uint32_t flags;
                    std::memcpy(&flags, record + 0xC34, sizeof(flags));
                    if ((flags & 0x200u) != 0) {
                        std::sprintf(buffer, "%s[han]", memo);
                        std::strcpy(gHoverTextD68AE8.data(), buffer);
                    }
                }
            }
            gTradeValueD2EA20 = 2;
        }
        static_cast<void>(render::ClearTextQueueTail_456D50(3));
        static_cast<void>(render::DrawText_456780(
            0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
        for (std::int32_t slot = 0; slot < 20; ++slot) {
            const char* const record = gTradeItemsD460D0.data() + slot * 0xC4C;
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used == 0) {
                continue;
            }
            const std::int32_t centerX = x + 50 * (slot % 5) + 37;
            const std::int32_t centerY = y + 52 * (slot / 5) + 51;
            std::int16_t value02;
            std::memcpy(&value02, record + 2, sizeof(value02));
            if (value02 < 0) {
                static_cast<void>(login::detail::DrawGraphic(
                    centerX, centerY, 223, 23040, 0, nullptr));
            }
            std::int32_t imageNo;
            std::memcpy(&imageNo, record + 4, sizeof(imageNo));
            static_cast<void>(login::detail::DrawGraphic(
                centerX, centerY, 223, imageNo, 0, descriptor));
            std::int32_t count;
            std::memcpy(&count, record + 8, sizeof(count));
            if (count > 0) {
                std::sprintf(buffer, "%3d", count);
                static_cast<void>(render::DrawText_456780(
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerX - 3)),
                    std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(centerY + 7)),
                    3, 1, 1, buffer, 0, 0));
            }
        }
    } else if (page == 1) {
        constexpr std::array<std::int16_t, 3> rowY {26, 95, 164};
        if ((state & 1) != 0) {
            for (std::int32_t row = 0; row < 3; ++row) {
                const std::int32_t top = y + rowY[row];
                if (login::detail::Sub_5049A0(x + 11, top, x + 251, top + 69, -1) != 0) {
                    const std::int32_t slot = scroll + row;
                    if (slot >= 0) {
                        std::uint16_t used;
                        std::memcpy(&used, gTradePetsC9C108.data() + slot * 0x5FC, sizeof(used));
                        if (used != 0) {
                            std::strcpy(gHoverTextD68AE8.data(),
                                "\xA5\x48\xA6\xB9\xC3\x64\xAA\xAB\xA6\x56\xB9\xEF\xA4\xE8"
                                "\xB4\xA3\xA5\x58\xA5\xE6\xA9\xF6\xA1\x43");
                            gTradePreviewCC2E64 = slot + 500;
                        }
                    }
                    break;
                }
            }
        }
        for (std::int32_t row = 0; row < 3; ++row) {
            static_cast<void>(Sub_4B0F20(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 11)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + rowY[row])),
                0, scroll + row, 3, 223, descriptor));
        }
    }
    *displayAmount = gTradeValueCF4688;
    std::int32_t items = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        std::uint16_t used;
        std::memcpy(&used, gTradeItemsD460D0.data() + slot * 0xC4C, sizeof(used));
        if (used != 0) {
            ++items;
        }
    }
    *itemCount = items;
    std::int32_t pets = 0;
    for (std::int32_t slot = 0; slot < 5; ++slot) {
        std::uint16_t used;
        std::memcpy(&used, gTradePetsC9C108.data() + slot * 0x5FC, sizeof(used));
        if (used != 0) {
            ++pets;
        }
    }
    *petCount = pets;
    *amount = gTradeValueCF4688;
}

bool Sub_4B1900(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height)
{
    const std::int32_t relativeX =
        platform::legacy_crypt_int::Subtract_401E60(&platform::gLegacyMouseState.mouseX, x);
    if (relativeX < 0 || relativeX >= width) {
        return false;
    }
    const std::int32_t relativeY =
        platform::legacy_crypt_int::Subtract_401E60(&platform::gLegacyMouseState.mouseY, y);
    return relativeY >= 0 && relativeY < height;
}

std::uint8_t Sub_474550(
    WindowInstance* const window, const std::int32_t index, const std::int32_t state,
    const std::int32_t maximum, const std::uint8_t layer, const char* const* const hints,
    const std::uint8_t flags, const std::uint32_t color)
{
    WindowElement* const elements = window->elements;
    const std::int32_t windowY = window->y;
    const std::int32_t windowX = window->x;
    const std::int32_t x = windowX + elements[index].x;
    const std::int32_t y = windowY + elements[index].y;
    render::GraphicDescriptor16 descriptor {};
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    // 每个键独立短路读取四次CryptInt，不能用一次坐标快照替代。
    const auto hit = [](const std::int32_t left, const std::int32_t top,
                       const std::int32_t right, const std::int32_t bottom) {
        return platform::legacy_crypt_int::IntLessEqualCrypt_401FC0(
                   left, &platform::gLegacyMouseState.mouseX)
            && platform::legacy_crypt_int::Less_401DB0(
                   &platform::gLegacyMouseState.mouseX, right)
            && platform::legacy_crypt_int::IntLessEqualCrypt_401FC0(
                   top, &platform::gLegacyMouseState.mouseY)
            && platform::legacy_crypt_int::Less_401DB0(
                   &platform::gLegacyMouseState.mouseY, bottom);
    };
    std::int32_t selected = -1;
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t bottom = y + 57 - row * 20;
        for (std::int32_t column = 0; column < 3; ++column) {
            const std::int32_t digit = 1 + row * 3 + column;
            const std::int32_t left = x + column * 24;
            std::int32_t imageState = 0;
            if (hit(left, bottom - 19, left + 21, bottom)) {
                std::strcpy(gHoverTextD68AE8.data(), hints[0]);
                imageState = (state & 8) != 0 ? 1 : 2;
                if ((state & 2) != 0) {
                    selected = digit;
                }
            }
            static_cast<void>(login::detail::DrawGraphic(
                left + 10, bottom - 9, layer, gNumericKeyImages54F030[digit][imageState],
                0, &descriptor));
        }
    }
    std::int32_t imageState = 0;
    if (hit(x, y + 58, x + 21, y + 77)) {
        std::strcpy(gHoverTextD68AE8.data(), hints[0]);
        imageState = (state & 8) != 0 ? 1 : 2;
        if ((state & 2) != 0) {
            selected = 0;
        }
    }
    static_cast<void>(login::detail::DrawGraphic(
        x + 10, y + 68, layer, gNumericKeyImages54F030[0][imageState], 0, &descriptor));
    imageState = 0;
    if (hit(x + 48, y + 58, x + 69, y + 77)) {
        if ((flags & 1u) != 0) {
            std::strcpy(gHoverTextD68AE8.data(), hints[1]);
            imageState = (state & 8) != 0 ? 1 : 2;
            if ((state & 2) != 0) {
                selected = 13;
            }
        } else {
            const std::int32_t value = std::bit_cast<std::int32_t>(window->elements[index].state);
            std::strcpy(gHoverTextD68AE8.data(), hints[value > 0 ? 1 : 2]);
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
    static_cast<void>(login::detail::DrawGraphic(
        x + 58, y + 68, layer, gNumericKeyImages54F030[13][imageState], 0, &descriptor));
    for (std::int32_t row = 0; row < 3; ++row) {
        const std::int32_t centerY = y + 28 + row * 20;
        imageState = 0;
        if (hit(x + 75, centerY - 10, x + 100, centerY + 9)) {
            std::strcpy(gHoverTextD68AE8.data(), hints[row + 3]);
            imageState = (state & 8) != 0 ? 1 : 2;
            if ((state & 2) != 0) {
                selected = row + 10;
            }
        }
        static_cast<void>(login::detail::DrawGraphic(
            x + 87, centerY, layer, gNumericKeyImages54F030[row + 10][imageState],
            0, &descriptor));
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

std::int32_t Sub_4B09E0(const std::int32_t, const std::int32_t state)
{
    if (gTradeValueD6742C == 2) {
        if ((state & 2) != 0) {
            WindowInstance* const window = gWindowStates[43].window;
            static_cast<void>(Sub_476570(&gTradeInputPrototype5803D0, 0, -100, 0));
            input::SetContext_44B330(&input::gInputContextCC21E8);
            gFocusedElementDataD45F94 = window->elements[0].data;
            auto* const data = static_cast<WindowInputData*>(window->elements[0].data);
            data->context = &input::gInputContextCC21E8;
        }
    } else if (input::gCurrentInputData == &input::gInputContextCC21E8) {
        static_cast<void>(Sub_4A7B00());
    }
    return 0;
}

std::int32_t Sub_4C1FB0(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    const bool hover = (state & 1) != 0;
    if (hover) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xC3\xF6\xB3\xAC\xB3\x6F\xAD\xD3\xB5\xF8\xB5\xA1\xA1\x43");
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

std::int32_t Sub_4FBA30(
    std::int32_t x, const std::int32_t y, const std::uint8_t renderMode,
    const std::int32_t fontType, std::int8_t color, const char* text,
    const std::int8_t value7, const std::int8_t marker)
{
    const char* delimiter = std::strchr(text, '$');
    while (delimiter != nullptr) {
        std::size_t length = static_cast<std::size_t>(delimiter - text);
        if (delimiter[1] == '$') {
            ++length;
        }
        auto* const prefix = static_cast<char*>(std::malloc(length + 1));
        if (prefix == nullptr) {
            return 0;
        }
        std::strncpy(prefix, text, length);
        prefix[length] = '\0';
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)),
            renderMode, std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(fontType)),
            color, prefix, value7, marker));
        x += login::detail::TextMeasure(prefix, fontType);
        text = delimiter + 2;
        std::free(prefix);
        const signed char next = static_cast<signed char>(delimiter[1]);
        if (next != '$') {
            const std::int32_t digit = next - '0';
            if (digit >= 0 && digit <= 9) {
                gTooltipColorD67D30 = digit;
                color = static_cast<std::int8_t>(digit);
            }
        }
        delimiter = std::strchr(text, '$');
    }
    return render::DrawText_456780(
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)),
        renderMode, std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(fontType)),
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(gTooltipColorD67D30)),
        text, value7, marker);
}

std::int32_t Sub_48B810(
    const std::int32_t x, const std::int32_t y,
    const char* const title, const std::int8_t titleColor,
    const char* lines, const std::int32_t stride, const std::int32_t lineCount,
    const char* const footer, const std::int8_t footerColor)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, input::LegacyText593850(), 0, 0));
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 10)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 10)),
            5, 12, titleColor, title, 0, 0));
    }
    gTooltipColorD67D30 = 0;
    std::int32_t lineY = 0;
    for (std::int32_t line = 0; line < lineCount; ++line) {
        if (*lines != '\0') {
            const std::int32_t shortY =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
            const std::int32_t shortX =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
            static_cast<void>(Sub_4FBA30(
                shortX + 10, shortY + lineY + 22, 5, 12, 0, lines, 0, 0));
        }
        lines += stride;
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

std::int32_t Sub_48B930(
    const std::int32_t x, const std::int32_t y,
    const char* const title, const std::int8_t titleColor,
    const char* lines, const std::int32_t, const std::int32_t lineCount,
    const char* const footer, const std::int8_t footerColor)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, input::LegacyText593850(), 0, 0));
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 10)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 10)),
            5, 12, titleColor, title, 0, 0));
    }
    gTooltipColorD67D30 = 0;
    std::int32_t lineY = 0;
    for (std::int32_t line = 0; line < lineCount; ++line) {
        if (*lines != '\0') {
            const std::int32_t shortY =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
            const std::int32_t shortX =
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
            static_cast<void>(Sub_4FBA30(
                shortX + 10, shortY + lineY + 22, 5, 12, 0, lines, 0, 0));
        }
        lineY += 13;
        lines += std::strlen(lines) + 1;
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
        230, 244317, 0, 1, 0, &descriptor);
}

std::int32_t Sub_48B450(
    const std::int32_t side, const std::int32_t index, const std::int32_t page)
{
    const std::int32_t key = index + 200;
    if (gTooltipSourceD303FC != key) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX));
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY));
        const bool left =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseX, 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseY, 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    const std::int32_t sideOffset =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(side)) - 10;
    gTooltipSourceD303FC = key;
    const char* const record = gTradeItemsD460D0.data() + (key + sideOffset * 20) * 0xC4C;
    std::uint32_t flags;
    std::memcpy(&flags, record + 0xC34, sizeof(flags));
    const char* const title = (flags & 0x100u) != 0 ? record + 0xC38 : record + 0x10;
    const std::int8_t titleColor = (flags & 0x100u) != 0
        ? 5 : static_cast<std::int8_t>((flags >> 9) & 1u);
    const char* const lines = record + 0x2D + page * 768;
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    return Sub_48B810(x, y, title, titleColor, lines, 96, 8, nullptr, 0);
}

WindowItemSelectorState* Sub_4C61B0(WindowItemSelectorState* const state)
{
    const std::uint32_t flags = login::detail::ConstructWindowSelectorPreview(state->preview);
    state->initialized3E8 = 0;
    state->value64 = 0;
    state->step6C = 21;
    state->previousPet74 = -1;
    state->selectedPet70 = -1;
    state->value3EC = 1;
    state->value3ED = 1;
    state->imageIndex3F0 = 0;
    state->imageIndex3F4 = 0;
    login::detail::FinishWindowSelectorPreview(state->preview, flags);
    state->hoverIndex3B0 = -1;
    state->hoverIndex3F8 = -1;
    state->bytes3B4.fill(std::byte{0});
    state->hoverCount3A8 = 0;
    state->bytes3AC.fill(std::byte{0});
    state->value3FC = 0;
    for (std::size_t index = 0; index < state->descriptors3B8.size(); ++index) {
        auto& descriptor = state->descriptors3B8[index];
        descriptor.bytes.fill(0);
        const std::uint32_t color = index == 0 ? 0xFFFFFFFFu : 0x80FFFFFFu;
        std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
        descriptor.bytes[13] = 4;
    }
    return state;
}

std::int32_t Sub_4C71B0(const WindowItemSelectorState* const state)
{
    return state->selectedPet70;
}

void Sub_4C6EC0(WindowItemSelectorState* const state, const std::uint8_t mode)
{
    state->value64 = mode;
    if (mode != 0) {
        state->mode68 = &gValue57E42C;
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
        state->callbacks50[0] = Sub_499C30;
        state->elements00[18] = last;
        state->callbacks50[1] = Sub_4B0F10;
        state->callbacks50[2] = Sub_49AC40;
        state->callbacks50[3] = Sub_4B0F10;
    } else {
        state->mode68 = &gValue57DA28;
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
        state->callbacks50[0] = Sub_488FB0;
        state->callbacks50[1] = Sub_489F00;
        state->callbacks50[2] = Sub_489760;
        state->callbacks50[3] = Sub_48A6C0;
    }
}

std::int32_t Sub_4C63C0(
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

std::int32_t Sub_4C6400(
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

void Sub_4C6440(WindowItemSelectorState* const state)
{
    if (state->value60 == 2) {
        state->elements00[6]->visible = 1;
    }
    if (state->value64 == 0) {
        state->elements00[19]->visible = 1;
    }
}

void Sub_4C6470(WindowItemSelectorState* const state)
{
    if (state->value60 == 3) {
        state->elements00[7]->visible = 1;
        state->elements00[8]->visible = 1;
    }
    state->elements00[15]->visible = 1;
    state->elements00[16]->visible = 1;
    const std::int32_t itemSlots =
        state->value3ED != 0 && world::IsValidPetIndex_51C410(state->selectedPet70)
        ? world::detail::gPetRuntimeF633F8[state->selectedPet70].values6D8[0] : 0;
    for (std::int32_t position = itemSlots; position < 6; ++position) {
        state->elements00[9 + position]->visible = 1;
    }
    state->elements00[17]->visible = 1;
    if (state->value64 == 0) {
        state->elements00[19]->visible = 1;
    }
}

void Sub_4C6500(WindowItemSelectorState* const state)
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

void Sub_4C6550(WindowItemSelectorState* const state, const std::int32_t page)
{
    if (page == 1 && std::bit_cast<std::int8_t>(*state->mode68) > 0) {
        static_cast<void>(Sub_4C6990(state));
        if (!world::IsValidPetIndex_51C410(state->selectedPet70)
            || world::detail::gPetRuntimeF633F8[state->selectedPet70].used == 0) {
            static_cast<void>(Sub_4C7340(state, 0));
        }
    }
    state->value60 = page;
    Sub_4C6500(state);
    switch (state->value60) {
    case 0:
        state->elements00[1]->mouseCallback = Sub_4B0F10;
        state->elements00[1]->visible = 0;
        Sub_4C6AC0(state, 16);
        static_cast<void>(Sub_4C7340(state, -1));
        break;
    case 1: {
        const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
        if (mode == 0) {
            Sub_4C6440(state);
            static_cast<WindowGraphicData*>(state->elements00[1]->data)->imageNo = 98146;
        } else if (mode > 0) {
            Sub_4C6470(state);
            static_cast<WindowGraphicData*>(state->elements00[1]->data)->imageNo = 98147;
            if (world::IsValidPetIndex_51C410(state->selectedPet70)) {
                for (std::int32_t position = 0;
                     position < world::detail::gPetRuntimeF633F8[state->selectedPet70].values6D8[0];
                     ++position) {
                    state->elements00[9 + position]->visible = 1;
                }
            }
        }
        state->elements00[1]->mouseCallback = Sub_4B0F10;
        state->elements00[1]->visible = 1;
        state->step6C = 21;
        break;
    }
    case 2:
        *state->mode68 = 0;
        Sub_4C6440(state);
        if (state->value64 != 0 || gItemSelectionModeCC40D4 == 0) {
            state->elements00[1]->mouseCallback = state->callbacks50[2];
        } else {
            state->elements00[0]->mouseCallback = state->callbacks50[1];
            state->elements00[1]->mouseCallback = state->callbacks50[3];
        }
        Sub_4C6AC0(state, -182);
        state->elements00[1]->visible = 1;
        static_cast<WindowGraphicData*>(state->elements00[1]->data)->imageNo = 98146;
        break;
    case 3:
        Sub_4C6AC0(state, -182);
        Sub_4C69B0(state);
        Sub_4C6470(state);
        state->elements00[1]->mouseCallback =
            state->value3ED == 1 ? state->callbacks50[2] : Sub_4B0F10;
        state->elements00[1]->visible = 1;
        static_cast<WindowGraphicData*>(state->elements00[1]->data)->imageNo = 98147;
        break;
    case 4:
        state->value60 = 0;
        Sub_4C6AC0(state, 16);
        state->elements00[1]->mouseCallback = Sub_4B0F10;
        state->elements00[1]->visible = 0;
        static_cast<void>(Sub_4C7340(state, -1));
        break;
    default:
        break;
    }
}

void Sub_4C6790(WindowItemSelectorState* const state)
{
    static constexpr std::int32_t images54F564[] {98133, 98133, 98135, 98134};
    static constexpr std::int32_t images54F574[] {98137, 98137, 98139, 98138};
    const std::int32_t page = state->value60;
    if (page > 0 && page <= 2 && std::bit_cast<std::int8_t>(*state->mode68) > 0) {
        static_cast<void>(Sub_4C6990(state));
        if (!world::IsValidPetIndex_51C410(state->selectedPet70)
            || world::detail::gPetRuntimeF633F8[state->selectedPet70].used == 0) {
            static_cast<void>(Sub_4C7340(state, 0));
        }
    }
    switch (state->value60) {
    case 1: {
        if (std::bit_cast<std::int8_t>(*state->mode68) > 0) {
            static_cast<void>(Sub_4C6990(state));
        }
        const std::int32_t x = state->elements00[1]->x;
        if (x > -182) {
            Sub_4C6AC0(state, x - state->step6C);
            const std::int32_t step = state->step6C;
            const WindowElement* const element = state->elements00[1];
            state->step6C = step - 1;
            if (element->x <= -182) {
                Sub_4C6AC0(state, -182);
            }
        } else {
            Sub_4C6550(state, std::bit_cast<std::int8_t>(*state->mode68) > 0 ? 3 : 2);
        }
        break;
    }
    case 2:
        if (std::bit_cast<std::int8_t>(*state->mode68) > 0) {
            Sub_4C6550(state, 3);
        } else {
            Sub_4C6B10(state);
        }
        break;
    case 3: {
        static_cast<void>(Sub_4C6990(state));
        if (!world::IsValidPetIndex_51C410(state->selectedPet70)
            || world::detail::gPetRuntimeF633F8[state->selectedPet70].used == 0) {
            *state->mode68 = 0;
        }
        if (*state->mode68 == 0) {
            Sub_4C6550(state, 2);
            break;
        }
        Sub_4C6B10(state);
        Sub_4C69B0(state);
        const std::int32_t firstIndex = state->imageIndex3F0;
        const WindowElement* const firstElement = state->elements00[1];
        const std::int32_t firstImage = images54F564[firstIndex];
        const WindowInstance* const firstWindow = gCurrentWindow;
        static_cast<void>(login::detail::DrawGraphic(
            firstElement->x + firstWindow->x + 85, firstElement->y + firstWindow->y + 180,
            223, firstImage, 0, &state->descriptors3B8[0]));
        const std::int32_t secondIndex = state->imageIndex3F4;
        const WindowElement* const secondElement = state->elements00[1];
        const std::int32_t secondImage = images54F574[secondIndex];
        const WindowInstance* const secondWindow = gCurrentWindow;
        static_cast<void>(login::detail::DrawGraphic(
            secondElement->x + secondWindow->x + 118, secondElement->y + secondWindow->y + 180,
            223, secondImage, 0, &state->descriptors3B8[0]));
        static_cast<void>(Sub_4C6D90(state));
        break;
    }
    case 4:
        Sub_4C6550(state, 0);
        break;
    default:
        break;
    }
}

std::int32_t Sub_4C6990(WindowItemSelectorState* const state)
{
    const std::int32_t index = Sub_4C71C0(std::bit_cast<std::int8_t>(*state->mode68));
    state->selectedPet70 = index;
    return index;
}

void Sub_4C69B0(WindowItemSelectorState* const state)
{
    if (state->selectedPet70 == -1 || !world::IsValidPetIndex_51C410(state->selectedPet70)) {
        return;
    }
    const std::int32_t selected = state->selectedPet70;
    auto* const pet = &world::detail::gPetRuntimeF633F8[selected];
    if (selected == state->previousPet74 && pet->updated510C == 0) {
        return;
    }
    state->previousPet74 = selected;
    pet->updated510C = 0;
    WindowElement* const nameElement = state->elements00[7];
    auto* const nameData = static_cast<WindowTextData*>(nameElement->data);
    const char* const name = pet->name6BD[0] != '\0' ? pet->name6BD.data() : pet->name6AC.data();
    std::strcpy(nameData->text, name);
    const std::int32_t nameWidth = login::detail::TextMeasure(
        nameData->text, std::to_integer<std::uint8_t>(nameData->bytes00[1]));
    nameElement->x = state->elements00[1]->x - nameWidth / 2 + 101;
    WindowElement* const weightElement = state->elements00[8];
    const std::int32_t maximumWeight = pet->values6D8[2];
    const std::int32_t weight = pet->values6D8[1];
    auto* const weightData = static_cast<WindowTextData*>(weightElement->data);
    std::sprintf(weightData->text, "\xAD\x74\xAD\xAB:%3d/%3d", weight, maximumWeight);
    const std::int32_t weightWidth = login::detail::TextMeasure(
        weightData->text, std::to_integer<std::uint8_t>(weightData->bytes00[1]));
    weightElement->x = state->elements00[1]->x - weightWidth / 2 + 101;
}

void Sub_4C6AC0(WindowItemSelectorState* const state, const std::int32_t x)
{
    state->elements00[1]->x = x;
    for (std::int32_t position = 2; position < 6; ++position) {
        state->elements00[position]->x = x - 15;
    }
    for (std::int32_t position = 0; position < 6; ++position) {
        state->elements00[9 + position]->x = gSelectorPetX54F5C4[position] + x - 24;
    }
}

void Sub_4C6B10(WindowItemSelectorState* const state)
{
    if (state->value3ED == 0 || state->hoverIndex3F8 < 0) {
        state->hoverCount3A8 = 0;
        return;
    }
    const std::int32_t localX = Sub_4C63C0(state, state->hoverIndex3F8);
    const WindowElement* const horizontalElement = state->elements00[1];
    const WindowInstance* const horizontalWindow = gCurrentWindow;
    const std::int32_t left = localX + horizontalElement->x + horizontalWindow->x - 24;
    const std::int32_t localY = Sub_4C6400(state, state->hoverIndex3F8);
    const WindowElement* const verticalElement = state->elements00[1];
    const WindowInstance* const verticalWindow = gCurrentWindow;
    const std::int32_t top = localY + verticalElement->y + verticalWindow->y - 24;
    login::detail::DrawGraphicHover(
        static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
        static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
        223, login::detail::gValueD69768, 0);
    const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
    std::int32_t slot;
    world::detail::PlayerItemRecord* item;
    if (mode == 0) {
        slot = state->hoverIndex3F8;
        item = &world::detail::gPlayerItemsFDA8A4[slot];
    } else {
        if (mode < 0 || state->selectedPet70 < 0) {
            return;
        }
        slot = state->hoverIndex3F8;
        const std::int32_t pet = Sub_4C71B0(&gItemSelectorCF3B10);
        item = &world::detail::gPetRuntimeF633F8[pet].items6E4[slot];
    }
    if (item->used != 0) {
        const auto* const occupancy =
            reinterpret_cast<const unsigned char*>(world::gValueD67B08.data());
        if (occupancy[slot] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                223, 249, 0);
            if ((item->flags & 0x80u) == 0) {
                static_cast<void>(login::detail::DrawGraphic(
                    left + 129, top + 117, 223, 22739, 0, &state->descriptors3B8[2]));
            }
            static_cast<void>(login::detail::DrawGraphic(
                left + 24, top + 24, 223, item->imageNo, 0, &state->descriptors3B8[1]));
        } else {
            state->descriptors3B8[2].bytes[11] = 5 * state->hoverCount3A8 >= 256
                ? 0xFF : static_cast<std::uint8_t>(
                    5 * static_cast<std::uint8_t>(state->hoverCount3A8));
            if ((item->flags & 0x80u) == 0) {
                static_cast<void>(login::detail::DrawGraphic(
                    left + 129, top + 117, 223, 22739, 0, &state->descriptors3B8[2]));
            }
            static_cast<void>(login::detail::DrawGraphic(
                left + 24, top + 24, 223, item->imageNo, 0, &state->descriptors3B8[2]));
        }
    }
    static_cast<void>(login::detail::DrawGraphic(
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

std::int32_t Sub_4C6D90(WindowItemSelectorState* const state)
{
    const std::int32_t selected = state->selectedPet70;
    if (selected < 0 || state->value3EC == 0) {
        return selected;
    }
    login::YobiAction* const preview = state->preview;
    const std::int32_t imageNo = world::detail::gPetRuntimeF633F8[selected].imageNo6A0;
    login::detail::SetYobiActionField2A8(preview, imageNo);
    const std::int32_t result = login::detail::Sub_517880(preview, 100, 0, 0);
    const std::int32_t graphicIndex = login::detail::GetYobiActionField20(preview);
    if (graphicIndex == 0) {
        return result;
    }
    const render::GraphicDescriptor16* const descriptor =
        login::detail::ApplyWindowAnimationDrawMode(preview);
    const std::uint8_t layer = login::detail::GetYobiActionField15(preview);
    const std::int32_t offsetY = login::detail::Sub_5182B0(preview, 1);
    const std::int32_t y = offsetY + gCurrentWindow->y + 163;
    const std::int32_t offsetX = login::detail::Sub_5182B0(preview, 0);
    const std::int32_t elementX = state->elements00[1]->x;
    const std::int32_t x = elementX + offsetX + gCurrentWindow->x + 101;
    return render::DrawGraphic2_521DD0(x, y, layer, graphicIndex, 0, 0, descriptor);
}

std::int32_t Sub_4C71C0(const std::int32_t ordinal)
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
        const auto& pair = world::detail::gPetPairsDE0B08[position];
        if (pair.used != 0) {
            const std::int32_t index = std::bit_cast<std::int16_t>(pair.index);
            const std::uint16_t flags = world::detail::gPetRuntimeF633F8[index].flags6AA;
            if (flags == 2 || flags == 1) {
                ++matched;
            }
        }
    }
    --position;
    return position < 0 ? -1
        : std::bit_cast<std::int16_t>(world::detail::gPetPairsDE0B08[position].index);
}

bool Sub_4C7340(WindowItemSelectorState* const state, const std::int32_t mode)
{
    *state->mode68 = static_cast<std::uint8_t>(mode);
    const std::int32_t page = state->value60;
    if (page == 2 || page == 3) {
        if (*state->mode68 == 0) {
            if (page != 2) {
                Sub_4C6550(state, 2);
            }
        } else if (page != 2) {
            Sub_4C6550(state, 3);
        }
    }
    return true;
}

bool Sub_4C7380(
    WindowItemSelectorState* const state, const std::int32_t tab, const std::int32_t events)
{
    static constexpr std::int32_t images54F5F4[] {
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
            std::strcpy(gHoverTextD68AE8.data(), tab == 0
                ? "\xB6\x7D\xB1\xD2\xB8\xCB\xB3\xC6\xB5\xF8\xB5\xA1\xA1\x43"
                : "\xB6\x7D\xB1\xD2\xC3\x64\xAA\xAB\xB8\xCB\xB3\xC6\xA1\x43");
        } else if (page > 1 && page <= 3) {
            if (std::bit_cast<std::int8_t>(*state->mode68) == tab) {
                std::strcpy(gHoverTextD68AE8.data(), tab == 0
                    ? "\xC3\xF6\xB3\xAC\xB8\xCB\xB3\xC6\xB5\xF8\xB5\xA1\xA1\x43"
                    : "\xC3\xF6\xB3\xAC\xC3\x64\xAA\xAB\xB8\xCB\xB3\xC6\xA1\x43");
            } else {
                std::strcpy(gHoverTextD68AE8.data(), tab == 0
                    ? "\xA4\xC1\xB4\xAB\xA8\xEC\xA8\xAD\xA4\x57\xB8\xCB\xB3\xC6\xA1\x43"
                    : "\xA4\xC1\xB4\xAB\xC3\x64\xAA\xAB\xB8\xCB\xB3\xC6\xA1\x43");
            }
        }
    }
    if ((events & 2) != 0) {
        gCurrentWindow->state |= 0x2000u;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        const std::int32_t page = state->value60;
        if (page == 0) {
            static_cast<void>(Sub_4C7340(state, tab));
            Sub_4C6550(state, 1);
        } else if (page > 1 && page <= 3) {
            if (std::bit_cast<std::int8_t>(*state->mode68) == tab) {
                Sub_4C6550(state, 4);
            } else {
                static_cast<void>(Sub_4C7340(state, tab));
                Sub_4C6550(state, 1);
            }
        }
        consumed = true;
    }
    const WindowInstance* const window = gCurrentWindow;
    const std::int32_t x = element->width / 2 + window->x + element->x + 1;
    const std::int32_t y = element->height / 2 + window->y + element->y + 1;
    const std::int32_t page = state->value60;
    if ((page == 2 || page == 3) && std::bit_cast<std::int8_t>(*state->mode68) == tab) {
        static_cast<void>(login::detail::DrawGraphic(
            x, y, 223, images54F5F4[tab * 4 + (hovered ? 3 : 1)],
            0, &state->descriptors3B8[0]));
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
    graphic->imageNo = images54F5F4[tab * 4 + imageIndex];
    return consumed;
}

bool Sub_4C78A0(WindowItemSelectorState* const state, const std::int32_t events)
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

bool Sub_4C7910(WindowItemSelectorState* const state, const std::int32_t events)
{
    bool consumed = false;
    if ((events & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        const std::uint8_t previous = state->value3ED;
        const std::uint8_t* const mode = state->mode68;
        const std::uint8_t enabled = previous == 0;
        state->value3ED = enabled;
        if (std::bit_cast<std::int8_t>(*mode) > 0) {
            state->elements00[1]->mouseCallback = enabled == 1
                ? state->callbacks50[2] : Sub_4B0F10;
            if (!world::IsValidPetIndex_51C410(state->selectedPet70)) {
                return false;
            }
            for (std::int32_t position = 0;
                 position < world::detail::gPetRuntimeF633F8[state->selectedPet70].values6D8[0];
                 ++position) {
                const std::uint8_t visible = state->value3ED == 0;
                state->elements00[9 + position]->visible = visible;
            }
        }
        consumed = true;
    }
    state->imageIndex3F4 = (events & 8) != 0 ? 2 : ((events & 1) != 0 ? 3 : 1);
    return consumed;
}

bool Sub_4C7A10(WindowItemSelectorState* const state, const std::int32_t events)
{
    bool consumed = false;
    if ((events & 0x402) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        const std::int32_t direction = login::detail::GetYobiActionField2B8(state->preview);
        login::detail::SetYobiActionField2B8(state->preview, (direction + 1) % 8);
        consumed = true;
    }
    if ((events & 0x804) != 0) {
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        const std::int32_t direction = login::detail::GetYobiActionField2B8(state->preview) - 1;
        login::detail::SetYobiActionField2B8(state->preview, direction);
        if (direction < 0) {
            login::detail::SetYobiActionField2B8(state->preview, direction + 8);
        }
        consumed = true;
    }
    return consumed;
}

void Sub_4C7A90(WindowItemSelectorState* const state)
{
    if (state->initialized3E8 == 0) {
        const std::uint8_t* const mode = state->mode68;
        state->previousPet74 = -1;
        Sub_4C6550(state, *mode == 0xFFu ? 0 : 1);
        state->initialized3E8 = 1;
    }
    Sub_4C6790(state);
}

std::int32_t Sub_4C7240(
    const WindowItemSelectorState* const state, const std::int32_t element,
    const std::int32_t slot, const std::int32_t offset)
{
    const std::int32_t local = Sub_4C63C0(state, slot);
    if (local < 0) {
        return -1;
    }
    const WindowInstance* const window = gCurrentWindow;
    return window->elements[element].x + window->x + local + offset;
}

std::int32_t Sub_4C7280(
    const WindowItemSelectorState* const state, const std::int32_t element,
    const std::int32_t slot, const std::int32_t offset)
{
    const std::int32_t local = Sub_4C6400(state, slot);
    if (local < 0) {
        return -1;
    }
    const WindowInstance* const window = gCurrentWindow;
    return window->elements[element].y + window->y + local + offset;
}

bool Sub_4C72C0(
    const WindowItemSelectorState* const state, world::detail::PlayerItemRecord** const items,
    std::int32_t* const count)
{
    const std::int8_t mode = std::bit_cast<std::int8_t>(*state->mode68);
    if (mode == 0) {
        *items = world::detail::gPlayerItemsFDA8A4.data();
        *count = 8;
        return true;
    }
    if (mode > 0 && world::IsValidPetIndex_51C410(state->selectedPet70)) {
        *items = world::detail::gPetRuntimeF633F8[state->selectedPet70].items6E4.data();
        *count = world::detail::gPetRuntimeF633F8[state->selectedPet70].values6D8[0];
        return true;
    }
    return false;
}

std::int32_t Sub_48A6C0(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::memcpy(dimmed.bytes.data() + 8, &dimmedColor, sizeof(dimmedColor));
    descriptor.bytes[13] = 4;
    dimmed.bytes[13] = 4;
    world::detail::PlayerItemRecord* items;
    std::int32_t count;
    if (!Sub_4C72C0(&gItemSelectorCF3B10, &items, &count)) {
        return 0;
    }
    if ((state & 2) != 0) {
        gCurrentWindow->state |= 0x2000u;
    }
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gValueD67B08.data());
    for (std::int32_t slot = 0; slot < count; ++slot) {
        const world::detail::PlayerItemRecord* const item = items + slot;
        if (item->used != 0) {
            const std::int32_t x = Sub_4C7240(&gItemSelectorCF3B10, index, slot, 0);
            const std::int32_t y = Sub_4C7280(&gItemSelectorCF3B10, index, slot, 0);
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, 23040, 0, nullptr));
            if (occupancy[slot] != 0) {
                login::detail::DrawGraphicHover(
                    static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                    static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                    223, 249, 0);
                static_cast<void>(login::detail::DrawGraphic(x, y, 223, item->imageNo, 0, &dimmed));
            } else {
                static_cast<void>(login::detail::DrawGraphic(x, y, 223, item->imageNo, 0, &descriptor));
            }
        }
    }
    return 0;
}

std::int32_t Sub_4888C0(const char* const text)
{
    const std::int32_t selected = std::atoi(text);
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    const unsigned char previous = occupancy[selected];
    gSelectedItemCB0A44 = selected;
    occupancy[selected] = static_cast<unsigned char>(previous + 1);
    gWindowStates[14].window->elements[4].mouseCallback = Sub_489F00;
    gWindowStates[14].window->elements[17].mouseCallback = Sub_48A6C0;
    gItemSelectionModeCC40D4 = 2;
    static_cast<void>(Sub_475DA0(1, 0, nullptr));
    return Sub_4763B0(14, 0);
}

std::int32_t Sub_488930(const std::int32_t, const std::int32_t state)
{
    return Sub_4C78A0(&gItemSelectorCF3B10, state) ? 1 : 0;
}

std::int32_t Sub_488950(const std::int32_t, const std::int32_t state)
{
    return Sub_4C7910(&gItemSelectorCF3B10, state) ? 1 : 0;
}

std::int32_t Sub_488970(const std::int32_t, const std::int32_t state)
{
    static_cast<void>(Sub_4C7A10(&gItemSelectorCF3B10, state));
    return 0;
}

std::int32_t Sub_488EA0(const std::int32_t, const std::int32_t state)
{
    return Sub_4C7380(&gItemSelectorCF3B10, 0, state) ? 1 : 0;
}

std::int32_t Sub_488EC0(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t ordinal = index - 19;
    std::int32_t remaining = ordinal;
    bool found = false;
    for (std::int32_t position = 0; position < 5; ++position) {
        const auto& pair = world::detail::gPetPairsDE0B08[position];
        if (pair.used == 0) {
            continue;
        }
        const std::int32_t pet = std::bit_cast<std::int16_t>(pair.index);
        const std::uint16_t flags = world::detail::gPetRuntimeF633F8[pet].flags6AA;
        if (flags == 1 || flags == 2) {
            --remaining;
            if (remaining < 0) {
                found = true;
                break;
            }
        }
    }
    if (found) {
        if (ordinal >= 0) {
            return Sub_4C7380(&gItemSelectorCF3B10, ordinal + 1, state) ? 1 : 0;
        }
    } else if (std::bit_cast<std::int8_t>(gValue57DA28) == ordinal + 1) {
        --gValue57DA28;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        consumed = 1;
    }
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 244306;
    return consumed;
}

void Sub_488510()
{
    WindowInstance* const quantityWindow = gWindowStates[42].window;
    if (quantityWindow != nullptr && gWindowStates[42].value20 == 14) {
        quantityWindow->state |= 0x8000u;
    }
    const std::uint8_t mode = gItemSelectionModeCC40D4;
    if (mode == 1 || mode == 2) {
        WindowInstance* const window = gWindowStates[14].window;
        gItemSelectionModeCC40D4 = 0;
        window->elements[4].mouseCallback = Sub_488FB0;
        gWindowStates[14].window->elements[17].mouseCallback = Sub_489760;
        const std::int32_t selected = gSelectedItemCB0A44;
        gSelectedItemCB0A44 = -1;
        --reinterpret_cast<unsigned char*>(world::gValueD67B08.data())[selected];
        static_cast<void>(Sub_4763E0());
    }
    if (world::detail::gValue581800 == 29) {
        world::detail::gValue581800 = -1;
    }
    gItemSelectorCF3B10.initialized3E8 = 0;
}

std::int32_t Sub_488990(const std::int32_t value)
{
    std::int32_t selected;
    if (value == -1) {
        Sub_4C6EC0(&gItemSelectorCF3B10, 0);
        WindowInstance* const window = gCurrentWindow;
        gItemSelectorCF3B10.initialized3E8 = 0;
        gWindowStates[14].value14 = 0;
        gItemSelectionModeCC40D4 = 0;
        window->elements[4].mouseCallback = Sub_488FB0;
        selected = -1;
        gCurrentWindow->elements[17].mouseCallback = Sub_489760;
        gSelectedItemCB0A44 = -1;
    } else {
        selected = gSelectedItemCB0A44;
    }
    if (gItemSelectionModeCC40D4 == 1) {
        if (world::detail::gPlayerItemsFDA8A4[selected].imageNo == 26569) {
            goto updateSelector;
        }
        WindowInstance* const window = gCurrentWindow;
        gItemSelectionModeCC40D4 = 0;
        window->elements[4].mouseCallback = Sub_488FB0;
        gCurrentWindow->elements[17].mouseCallback = Sub_489760;
        const std::int32_t oldSelection = gSelectedItemCB0A44;
        gSelectedItemCB0A44 = -1;
        --reinterpret_cast<unsigned char*>(world::gValueD67B08.data())[oldSelection];
        if (world::detail::gValue581800 == 29) {
            world::detail::gValue581800 = -1;
        }
        static_cast<void>(Sub_4763E0());
    }
    if (gItemSelectionModeCC40D4 == 2 && world::detail::gValue581800 != 29) {
        gCurrentWindow->elements[4].mouseCallback = Sub_488FB0;
        gCurrentWindow->elements[17].mouseCallback = Sub_489760;
        const std::int32_t oldSelection = gSelectedItemCB0A44;
        gSelectedItemCB0A44 = -1;
        gItemSelectionModeCC40D4 = 0;
        --reinterpret_cast<unsigned char*>(world::gValueD67B08.data())[oldSelection];
        static_cast<void>(Sub_4763E0());
    }
updateSelector:
    WindowInstance* const quantityWindow = gWindowStates[42].window;
    gWindowStates[14].value10 = 0xFFFFFFFFu;
    if (quantityWindow != nullptr
        && gWindowStates[42].value20 == 14 && gWindowStates[42].value24 == 1) {
        const std::int32_t amount = world::detail::ReadPlayerValueFC();
        gWindowStates[42].value10 = static_cast<std::uint32_t>(amount);
        gWindowStates[42].value14 = static_cast<std::uint32_t>(amount);
    }
    Sub_4C7A90(&gItemSelectorCF3B10);
    return 1;
}

std::int32_t Sub_488B10(const std::int32_t)
{
    static constexpr char selectionText586B14[] =
        "\xBF\xEF\xA6\x6E\xB7\x51\xBB\x5C\xA6\x4C\xAA\xBA\xAA\xAB\xAB\x7E\xA1\x41\0"
        "\xB3\xE6\xC2\x49\xB7\xC6\xB9\xAB\xA5\xAA\xC1\xE4\xA1\x41\0"
        "\xA5\x69\xBB\x5C\xA4\x55\xA6\x4C\xB0\x4F\xA1\x43\0"
        "\xB3\xE6\xC2\x49\xB7\xC6\xB9\xAB\xA5\x6B\xC1\xE4\xA7\x59\xB5\xB2\xA7\xF4\xA1\x43";
    static constexpr char selectionText586B5C[] =
        "\xBF\xEF\xA6\x6E\xB7\x51\xB1\xBB\xAE\x49\xAA\xBA\xAA\xAB\xAB\x7E\xA1\x41\0"
        "\xB3\xE6\xC2\x49\xB7\xC6\xB9\xAB\xA5\xAA\xC1\xE4\xA5\x69\xB1\xBB\xAE\x49"
        "\xAA\xAB\xAB\x7E\xA1\x43\0"
        "\xB3\xE6\xC2\x49\xB7\xC6\xB9\xAB\xA5\x6B\xC1\xE4\xA7\x59\xB5\xB2\xA7\xF4\xA1\x43\0";
    static_assert(sizeof(selectionText586B14) == 68);
    static_assert(sizeof(selectionText586B5C) == 66);
    const std::uint8_t mode = gItemSelectionModeCC40D4;
    if (mode == 0) {
        const std::int32_t selected = std::bit_cast<std::int32_t>(gWindowStates[14].value10);
        if (selected >= 0) {
            const std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[14].value14);
            static_cast<void>(Sub_48B1A0(selected, page));
        }
    } else if (mode == 1 || mode == 2) {
        const char* const text = mode == 1 ? selectionText586B14 : selectionText586B5C;
        const bool top =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseY, 240);
        const std::int32_t y = platform::legacy_crypt_int::Add_401E10(
            &platform::gLegacyMouseState.mouseY, top ? 60 : -174);
        const bool left =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseX, 320);
        const std::int32_t x = platform::legacy_crypt_int::Add_401E10(
            &platform::gLegacyMouseState.mouseX, left ? 60 : -257);
        static_cast<void>(Sub_48B930(
            x, y, input::LegacyText593850(), 0, text, 31, 4, nullptr, 0));
    }
    const std::int32_t mappedImage =
        character_select::detail::Sub_51B740(world::detail::ReadPlayerImageNo());
    login::detail::SetYobiActionField2A8(
        static_cast<WindowAnimationData*>(gCurrentWindow->elements[5].data)->action, mappedImage);
    login::detail::SetYobiActionField2B8(
        static_cast<WindowAnimationData*>(gCurrentWindow->elements[5].data)->action, 5);
    const std::int32_t imageNo = world::detail::ReadPlayerImageNo();
    if (imageNo >= 110350 && imageNo <= 110373) {
        login::detail::AddYobiActionDrawFlags(
            static_cast<WindowAnimationData*>(gCurrentWindow->elements[5].data)->action, 4);
    }
    DrawCurrentWindow_4729B0();
    return 1;
}

std::int32_t Sub_488C50(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    graphic->imageNo = 243000;
    if ((state & 1) != 0) {
        graphic->imageNo = 243002;
        std::strcpy(gHoverTextD68AE8.data(), "\xC3\xF6\xB3\xAC\xB3\x6F\xAD\xD3\xB5\xF8\xB5\xA1\xA1\x43");
    }
    if ((state & 2) == 0) {
        return 0;
    }
    gItemSelectorCF3B10.initialized3E8 = 0;
    graphic->imageNo = 243001;
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(audio::PlaySe_52C450(57, 320));
    return 1;
}

void Sub_488CF0(const std::int32_t amount)
{
    if ((world::detail::ReadPlayerActionFlags() & 0x80u) == 0
        && world::detail::Sub_440B50() == 0) {
        const std::int32_t y = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t x = platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_510DA0(platform::gConnectionSocket, x, y, amount));
    }
}

std::int32_t Sub_488D30(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xB7\x51\xA9\xF1\xBF\xFA\xA6\x62\xA6\x61\xAD\xB1\xAE\xC9"
            "\xA8\xCF\xA5\xCE\xA6\xB9\xAB\xF6\xC1\xE4\xA1\x43");
    }
    if ((state & 2) != 0) {
        if (gWindowStates[42].window != nullptr && gWindowStates[42].value20 == 14
            && gWindowStates[42].value24 == static_cast<std::uint32_t>(index)) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        } else {
            const std::int32_t amount = world::detail::ReadPlayerValueFC();
            static_cast<void>(login::detail::Sub_48AC60(
                14, index, 244310, amount, amount, 0, Sub_488CF0, 1, 0, 0));
        }
        consumed = 1;
    }
    if (gWindowStates[42].window != nullptr && gWindowStates[42].value20 == 14
        && gWindowStates[42].value24 == static_cast<std::uint32_t>(index)) {
        const std::int32_t amount = world::detail::ReadPlayerValueFC();
        gWindowStates[42].value10 = static_cast<std::uint32_t>(amount);
        gWindowStates[42].value14 = static_cast<std::uint32_t>(amount);
    }
    graphic->imageNo = 244291;
    if ((state & 1) != 0) {
        graphic->imageNo = 244293;
    }
    if (gWindowStates[42].window != nullptr && gWindowStates[42].value20 == 14
        && gWindowStates[42].value24 == static_cast<std::uint32_t>(index)) {
        graphic->imageNo = 244292;
    }
    return consumed;
}

std::int32_t Sub_488E60(const std::int32_t, const std::int32_t)
{
    std::sprintf(gInventoryMoneyTextCB086C.data(), "%10d", world::detail::ReadPlayerValueFC());
    return 0;
}

std::int32_t Sub_488E80(const std::int32_t, const std::int32_t)
{
    std::sprintf(gInventoryValueTextCC3ECC.data(), "%10d", world::detail::ReadPlayerValue15BCC());
    return 0;
}

void Sub_49C4D0()
{
    for (auto& image : gMessageDialogImagesCC4B70) {
        image.imageNo = 0;
    }
    gMessageDialogImageCountD67B30 = 0;
    WindowInstance* const window = gWindowStates[36].window;
    if (window != nullptr) {
        for (std::int32_t index = 8; index < 18; ++index) {
            window->elements[index].visible = 0;
        }
    }
}

void Sub_49C510(
    const std::uint8_t used, const std::int32_t imageNo, const std::uint8_t direction,
    const std::int16_t x, const std::int16_t y)
{
    const std::int32_t count = std::bit_cast<std::int8_t>(gMessageDialogImageCountD67B30);
    if (count >= 10) {
        return;
    }
    auto& image = gMessageDialogImagesCC4B70[count];
    image.imageNo = imageNo;
    image.direction = direction;
    image.x = x;
    image.y = y;
    image.used = used;
    WindowInstance* const window = gWindowStates[36].window;
    if (window != nullptr) {
        WindowElement* const element = window->elements + count + 8;
        login::detail::SetYobiActionField2A8(
            static_cast<WindowAnimationData*>(element->data)->action, imageNo);
        login::detail::SetYobiActionField2B8(
            static_cast<WindowAnimationData*>(element->data)->action,
            std::bit_cast<std::int8_t>(direction));
        element->visible = 0;
        element->x = x;
        element->y = y;
        if (used != 0) {
            element->visible = 1;
        }
    }
    ++gMessageDialogImageCountD67B30;
}

login::YobiAction* Sub_49C600(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::int32_t imageNo, const std::int32_t buttons, WindowDialogTextLine* const lines,
    const std::int32_t lineCount, const std::int32_t lineHeight,
    const std::int32_t firstSelectableLine, const std::uint8_t inputCount,
    std::int16_t* const inputY, const std::int32_t inputCapacity,
    const std::uint8_t distance, const std::uint8_t options,
    const WindowDialogReplyCallback reply, const WindowDialogFrameCallback frame,
    const std::uint32_t flags, const std::uint32_t mode)
{
    gMessageDialog.width = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(width));
    gMessageDialog.lines = lines;
    gMessageDialog.firstSelectableLine = firstSelectableLine;
    gMessageDialog.inputY = inputY;
    gMessageDialog.inputCount = inputCount;
    gWindowStates[37].y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    gMessageDialog.inputCapacity = inputCapacity;
    gWindowStates[37].x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    gMessageDialog.imageNo = imageNo;
    gMessageDialog.distance = distance;
    gMessageDialog.lineCount = lineCount;
    gMessageDialog.options = options;
    const auto height16 = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(height));
    gMessageDialog.height = height16;
    gMessageDialog.reply = reply;
    gMessageDialog.buttons = buttons;
    gMessageDialog.lineHeight = lineHeight;
    gMessageDialog.frame = frame;
    const std::int32_t maximumLines = login::detail::Sub_49C5E0(height16, buttons, lineHeight);
    if (gMessageDialog.lineCount > maximumLines) {
        gMessageDialog.lineCount = maximumLines;
    }
    WindowInstance* const first = gWindowStates[36].window;
    if (first != nullptr) {
        first->state |= 0x8000u;
    }
    WindowInstance* const second = gWindowStates[37].window;
    if (second != nullptr) {
        second->state |= 0x8000u;
    }
    return login::detail::Sub_475950(x, y, width, height, 37, flags, mode, nullptr);
}

std::int32_t Sub_49C710(const std::int32_t value)
{
    WindowInstance* const initialWindow = gCurrentWindow;
    if (gMessageDialog.closeCallbackPending != 0 && (initialWindow->state & 0x8000u) != 0) {
        for (std::int32_t index = 0; index < 19; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        return 1;
    }
    if (value == -1) {
        const std::int32_t width = gMessageDialog.width;
        const std::int32_t buttons = gMessageDialog.buttons;
        initialWindow->width = width;
        const std::int32_t margin = buttons != 0 ? 24 : 0;
        gCurrentWindow->height = gMessageDialog.height;
        const std::uint32_t mask = static_cast<std::uint32_t>(gMessageDialog.buttons);
        std::int32_t buttonCount = 0;
        for (std::uint32_t bit = 1; bit < 64; bit <<= 1) {
            if ((mask & bit) != 0) {
                ++buttonCount;
            }
        }
        const std::int32_t spacing = (gCurrentWindow->width - 48) / (buttonCount + 1);
        std::int32_t x = spacing - 9;
        for (std::int32_t index = 0, bit = 1; index < 6; ++index, bit <<= 1) {
            gCurrentWindow->elements[index].visible = 0;
            if ((gMessageDialog.buttons & bit) != 0) {
                gCurrentWindow->elements[index].visible = 1;
                gCurrentWindow->elements[index].width = 66;
                gCurrentWindow->elements[index].height = 17;
                gCurrentWindow->elements[index].x = x;
                x += spacing;
                WindowInstance* const window = gCurrentWindow;
                window->elements[index].y = window->height + margin / 2 - margin - 25;
            }
        }
        gCurrentWindow->elements[6].x = 0;
        gCurrentWindow->elements[6].y = 0;
        WindowInstance* window = gCurrentWindow;
        window->elements[6].width = window->width;
        window = gCurrentWindow;
        window->elements[6].height = window->height;
        const std::int32_t count = std::bit_cast<std::int8_t>(gMessageDialog.inputCount);
        gMessageDialog.selectedInput = -1;
        if (count > 0) {
            char** storage = static_cast<char**>(std::calloc(count, sizeof(char*)));
            gMessageDialog.inputTexts = storage;
            if (storage != nullptr) {
                std::int32_t index = 0;
                while (index < std::bit_cast<std::int8_t>(gMessageDialog.inputCount)) {
                    char* const text = static_cast<char*>(std::calloc(gMessageDialog.inputCapacity + 1, 1));
                    gMessageDialog.inputTexts[index] = text;
                    storage = gMessageDialog.inputTexts;
                    // 原49C8EB检查表指针，不能替换为本行text的分配检查。
                    if (storage == nullptr) {
                        break;
                    }
                    ++index;
                }
                if (index < std::bit_cast<std::int8_t>(gMessageDialog.inputCount)) {
                    for (std::int32_t row = 0;
                         row < std::bit_cast<std::int8_t>(gMessageDialog.inputCount); ++row) {
                        if (storage[row] == nullptr) {
                            break;
                        }
                        std::free(storage[row]);
                        gMessageDialog.inputTexts[row] = nullptr;
                        storage = gMessageDialog.inputTexts;
                    }
                    std::free(storage);
                    gMessageDialog.inputTexts = nullptr;
                }
            }
            gDialogInputPrototype57E478.maxCharPerLine = gMessageDialog.inputCapacity;
            gCurrentWindow->elements[7].visible = 1;
            const std::int16_t inputY = gMessageDialog.inputY[0];
            window = gCurrentWindow;
            const std::int32_t y = window->y + inputY;
            const std::int32_t inputX = window->x + 24;
            static_cast<void>(Sub_476570(&gDialogInputPrototype57E478, inputX, y, 0));
            input::SetContext_44B330(&input::gInputContextCA02E0);
            window = gCurrentWindow;
            gFocusedElementDataD45F94 = window->elements[7].data;
            static_cast<WindowInputData*>(window->elements[7].data)->context = &input::gInputContextCA02E0;
            gMessageDialog.selectedInput = 0;
        }
        gMessageDialog.closeCallbackPending = 1;
        for (std::int32_t index = 8; index < 18; ++index) {
            const auto& image = gMessageDialogImagesCC4B70[index - 8];
            login::detail::SetYobiActionField2A8(
                static_cast<WindowAnimationData*>(gCurrentWindow->elements[index].data)->action,
                image.imageNo);
            login::detail::SetYobiActionField2B8(
                static_cast<WindowAnimationData*>(gCurrentWindow->elements[index].data)->action,
                std::bit_cast<std::int8_t>(image.direction));
            login::detail::SetYobiActionField310(
                static_cast<WindowAnimationData*>(gCurrentWindow->elements[index].data)->action, 4);
            login::detail::SetYobiActionField309(
                static_cast<WindowAnimationData*>(gCurrentWindow->elements[index].data)->action, 4);
            gCurrentWindow->elements[index].x = image.x;
            gCurrentWindow->elements[index].y = image.y;
            gCurrentWindow->elements[index].visible = 0;
            if (image.used != 0) {
                gCurrentWindow->elements[index].visible = 1;
            }
        }
        return 1;
    }
    if (std::bit_cast<std::int8_t>(gMessageDialog.inputCount) > 0
        && gMessageDialog.inputTexts == nullptr) {
        initialWindow->state |= 0x8000u;
        gMessageDialog.closeCallbackPending = 0;
    }
    gMessageDialog.hoveredButton = 0;
    gMessageDialog.hoveredRow = -1;
    gMessageDialog.events = 0;
    return 1;
}

std::int32_t Sub_49CAD0(const std::int32_t)
{
    WindowInstance* window = gCurrentWindow;
    if (gMessageDialog.closeCallbackPending != 0 && (window->state & 0x8000u) != 0) {
        return 1;
    }
    render::GraphicDescriptor16 descriptor {};
    const std::int16_t offsetX = -150;
    const std::int16_t offsetY = -8;
    const std::int16_t height = 16;
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data(), &offsetX, sizeof(offsetX));
    std::memcpy(descriptor.bytes.data() + 2, &offsetY, sizeof(offsetY));
    std::memcpy(descriptor.bytes.data() + 6, &height, sizeof(height));
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[12] = 1;
    descriptor.bytes[13] = 4;
    if ((gMessageDialog.options & 4u) == 0) {
        const std::int32_t distance = std::bit_cast<std::int8_t>(gMessageDialog.distance);
        const std::int32_t close = Sub_4D3040(distance, distance);
        window = gCurrentWindow;
        if (close != 0) {
            window->state |= 0x8000u;
            static_cast<void>(audio::PlaySe_52C450(57, 320));
            window = gCurrentWindow;
        }
    }
    const WindowDialogFrameCallback frame = gMessageDialog.frame;
    if (frame != nullptr) {
        const std::int32_t button = gMessageDialog.hoveredButton;
        if (button != 0) {
            gMessageDialog.events = 0;
        }
        const std::int32_t row = gMessageDialog.hoveredRow;
        if (row > 0) {
            gMessageDialog.events = 0;
        }
        const std::int32_t events = gMessageDialog.events;
        const std::int32_t close = frame(window, button, row, events);
        window = gCurrentWindow;
        if (close != 0) {
            window->state |= 0x8000u;
            window = gCurrentWindow;
            gMessageDialog.closeCallbackPending = 0;
        }
    }
    for (std::int32_t row = 0; row < gMessageDialog.lineCount; ++row) {
        const char* const text = gMessageDialog.lines[row].text.data();
        if (text[0] != '\0') {
            static_cast<void>(render::DrawText_456780(
                static_cast<std::int16_t>(window->x + 24),
                static_cast<std::int16_t>(window->y + row * gMessageDialog.lineHeight + 20),
                3, 0, 0, text, 0, static_cast<std::int8_t>(login::detail::gValueD69768)));
            window = gCurrentWindow;
        }
    }
    std::int32_t inputCount = std::bit_cast<std::int8_t>(gMessageDialog.inputCount);
    if (inputCount > 0) {
        const std::int32_t inputX = window->x + 24;
        std::int32_t selected = gMessageDialog.selectedInput;
        if (selected >= 0 && input::gCurrentInputData != &input::gInputContextCA02E0) {
            std::strcpy(gMessageDialog.inputTexts[selected], input::gInputContextCA02E0.text);
            selected = -1;
            window = gCurrentWindow;
            inputCount = std::bit_cast<std::int8_t>(gMessageDialog.inputCount);
            gMessageDialog.selectedInput = -1;
        }
        std::int32_t row = 0;
        while (row < inputCount) {
            if (row == selected) {
                render::DrawInputText_4568A0(input::gCurrentInputData);
            } else {
                static_cast<void>(render::DrawText_456780(
                    static_cast<std::int16_t>(inputX),
                    static_cast<std::int16_t>(window->y + gMessageDialog.inputY[row]),
                    3, 0, 0, gMessageDialog.inputTexts[row], 0,
                    static_cast<std::int8_t>(login::detail::gValueD69768)));
            }
            inputCount = std::bit_cast<std::int8_t>(gMessageDialog.inputCount);
            ++row;
            window = gCurrentWindow;
            if (row < inputCount) {
                selected = gMessageDialog.selectedInput;
            }
        }
        if ((gMessageDialog.options & 2u) != 0) {
            const std::int32_t right = window->width - 24;
            std::int32_t left = 24;
            while (left < right) {
                auto width = static_cast<std::int16_t>(right - left);
                if (width > 300) {
                    width = 300;
                }
                std::memcpy(descriptor.bytes.data() + 4, &width, sizeof(width));
                std::int32_t input = 0;
                if (inputCount > 0) {
                    do {
                        const std::int32_t y = window->y + gMessageDialog.inputY[input];
                        const std::int32_t x = window->x + left;
                        static_cast<void>(render::DrawGraphic3_520970(
                            x, y, 223, 243225, 0, 1, 0, &descriptor));
                        inputCount = std::bit_cast<std::int8_t>(gMessageDialog.inputCount);
                        ++input;
                        if (input < inputCount) {
                            window = gCurrentWindow;
                        }
                    } while (input < inputCount);
                    std::memcpy(&width, descriptor.bytes.data() + 4, sizeof(width));
                    window = gCurrentWindow;
                }
                left += width;
            }
        }
    }
    DrawCurrentWindow_4729B0();
    window = gCurrentWindow;
    login::detail::DrawNineGrid_49BE00(
        gMessageDialog.imageNo, static_cast<std::int16_t>(window->x),
        static_cast<std::int16_t>(window->y), static_cast<std::int16_t>(window->width),
        static_cast<std::int16_t>(window->height), 223, -1,
        std::bit_cast<std::int32_t>(0x80FFFFFFu), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

void Sub_49CDF0()
{
    if (gMessageDialog.closeCallbackPending != 0) {
        const WindowDialogReplyCallback reply = gMessageDialog.reply;
        if (reply != nullptr) {
            static_cast<void>(reply(0, -1, nullptr, 0));
        }
    }
    std::int32_t count = std::bit_cast<std::int8_t>(gMessageDialog.inputCount);
    if (count <= 0) {
        return;
    }
    char** storage = gMessageDialog.inputTexts;
    if (storage == nullptr) {
        return;
    }
    for (std::int32_t index = 0; index < count; ++index) {
        if (storage[index] != nullptr) {
            std::free(storage[index]);
            count = std::bit_cast<std::int8_t>(gMessageDialog.inputCount);
            storage = gMessageDialog.inputTexts;
        }
    }
    std::free(storage);
    gMessageDialog.inputTexts = nullptr;
}

std::int32_t Sub_49CE70(const std::int32_t index, const std::int32_t state)
{
    static constexpr std::int32_t images[6][3] {
        {243048, 243049, 243050},
        {245320, 245321, 245322},
        {245323, 245324, 245325},
        {245326, 245327, 245328},
        {245332, 245333, 245334},
        {245329, 245330, 245331},
    };
    auto* const data = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hovered = (state & 1) != 0;
    std::int32_t consumed = 0;
    if (hovered) {
        gMessageDialog.hoveredButton =
            std::bit_cast<std::int32_t>(1u << (static_cast<std::uint32_t>(index) & 31u));
    }
    if ((state & 2) != 0) {
        if (gMessageDialog.reply != nullptr) {
            const std::int32_t selected = gMessageDialog.selectedInput;
            if (selected >= 0) {
                std::strcpy(gMessageDialog.inputTexts[selected], input::gInputContextCA02E0.text);
            }
            const std::uint8_t count = gMessageDialog.inputCount;
            char* const* const texts = gMessageDialog.inputTexts;
            const auto button =
                std::bit_cast<std::int32_t>(1u << (static_cast<std::uint32_t>(index) & 31u));
            if (gMessageDialog.reply(button, -1, texts, count) != 0) {
                gCurrentWindow->state |= 0x8000u;
                gMessageDialog.closeCallbackPending = 0;
            }
        }
        consumed = 1;
    }
    data->imageNo = images[index][0];
    if (hovered) {
        data->imageNo = images[index][2];
    }
    if ((state & 8) != 0) {
        data->imageNo = images[index][1];
    }
    return consumed;
}

std::int32_t Sub_49CF70(const std::int32_t, const std::int32_t state)
{
    std::int32_t inputCount = std::bit_cast<std::int8_t>(gMessageDialog.inputCount);
    gMessageDialog.events = state;
    WindowInstance* window = gCurrentWindow;
    const auto inputX = static_cast<std::int16_t>(window->x + 24);
    const auto inputWidth = static_cast<std::int16_t>(window->width - 48);
    std::int32_t consumed = 0;
    std::int32_t selected = 0;
    if (inputCount > 0) {
        do {
            const std::int32_t top = gMessageDialog.inputY[selected] + window->y;
            if (login::detail::Sub_5049A0(inputX, top, inputX + inputWidth, top + 16, -1) != 0) {
                break;
            }
            inputCount = std::bit_cast<std::int8_t>(gMessageDialog.inputCount);
            ++selected;
            if (selected < inputCount) {
                window = gCurrentWindow;
            }
        } while (selected < inputCount);
        window = gCurrentWindow;
    }
    if (selected < std::bit_cast<std::int8_t>(gMessageDialog.inputCount)) {
        if (selected != gMessageDialog.selectedInput) {
            const std::int32_t top = gMessageDialog.inputY[selected] + window->y;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(inputX), static_cast<std::uint16_t>(top),
                static_cast<std::uint16_t>(inputX + inputWidth), static_cast<std::uint16_t>(top + 16),
                223, login::detail::gValueD69768, 0);
            if ((state & 2) != 0) {
                const std::int32_t previous = gMessageDialog.selectedInput;
                if (previous >= 0) {
                    std::strcpy(gMessageDialog.inputTexts[previous], input::gInputContextCA02E0.text);
                }
                gCurrentWindow->elements[7].visible = 1;
                const std::int16_t inputY = gMessageDialog.inputY[selected];
                const std::int32_t y = inputY + gCurrentWindow->y;
                static_cast<void>(Sub_476570(&gDialogInputPrototype57E478, inputX, y, 0));
                input::SetContext_44B330(&input::gInputContextCA02E0);
                window = gCurrentWindow;
                gFocusedElementDataD45F94 = window->elements[7].data;
                static_cast<WindowInputData*>(window->elements[7].data)->context = &input::gInputContextCA02E0;
                char* const* const texts = gMessageDialog.inputTexts;
                gMessageDialog.selectedInput = selected;
                input::LoadInitialText_44A160(texts[selected]);
                consumed = 1;
            }
        }
    } else if (gMessageDialog.firstSelectableLine > 0) {
        const auto left = static_cast<std::int16_t>(window->x + 22);
        const auto top = static_cast<std::int16_t>(window->y + 18);
        const auto width = static_cast<std::int16_t>(window->width - 48);
        std::int32_t lineCount = gMessageDialog.lineCount;
        std::int32_t offset = 0;
        std::int32_t row = 0;
        while (row < lineCount) {
            if (login::detail::Sub_5049A0(left, top + offset, left + width, top + offset + 20, -1) != 0) {
                lineCount = gMessageDialog.lineCount;
                if (row < lineCount) {
                    gMessageDialog.hoveredRow = row;
                }
                break;
            }
            const std::int32_t lineHeight = gMessageDialog.lineHeight;
            lineCount = gMessageDialog.lineCount;
            offset += lineHeight;
            ++row;
        }
        const std::int32_t lineHeight = gMessageDialog.lineHeight;
        row = gMessageDialog.firstSelectableLine;
        offset = row * lineHeight;
        if (row < lineCount) {
            bool found = false;
            do {
                if (login::detail::Sub_5049A0(
                        left, top + offset, left + width, top + offset + 20, -1) != 0) {
                    found = true;
                    break;
                }
                const std::int32_t currentLineHeight = gMessageDialog.lineHeight;
                lineCount = gMessageDialog.lineCount;
                offset += currentLineHeight;
                ++row;
            } while (row < lineCount);
            if (found && row < gMessageDialog.lineCount
                && gMessageDialog.lines[row].text[0] != '\0' && row >= 0) {
                login::detail::DrawGraphicHover(
                    static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top + offset),
                    static_cast<std::uint16_t>(left + width), static_cast<std::uint16_t>(top + offset + 20),
                    223, login::detail::gValueD69768, 0);
                if ((state & 2) != 0) {
                    const std::int32_t previous = gMessageDialog.selectedInput;
                    if (previous >= 0) {
                        std::strcpy(gMessageDialog.inputTexts[previous], input::gInputContextCA02E0.text);
                    }
                    const WindowDialogReplyCallback reply = gMessageDialog.reply;
                    if (reply != nullptr) {
                        const std::uint8_t count = gMessageDialog.inputCount;
                        const std::int32_t first = gMessageDialog.firstSelectableLine;
                        char* const* const texts = gMessageDialog.inputTexts;
                        if (reply(0, row - first, texts, count) != 0) {
                            gCurrentWindow->state |= 0x8000u;
                            gMessageDialog.closeCallbackPending = 0;
                        }
                    }
                    consumed = 1;
                }
            }
        }
    }
    return (gMessageDialog.options & 1u) != 0 ? 1 : consumed;
}

std::int32_t Sub_49DA30(
    const std::int32_t button, const std::int32_t row,
    char* const* const texts, const std::uint8_t inputCount)
{
    // 五个原256B临时区保持连续，原位转义不另建替代缓冲。
    std::array<char, 1280> buffer;
    char* const reply = buffer.data();
    char* const first = reply + 256;
    char* const rowReply = reply + 512;
    char* const second = reply + 768;
    char* const combined = reply + 1024;
    if (button == 0) {
        if (row < 0) {
            world::detail::gValue581800 = -1;
        }
    } else {
        if (gDialogReply.type == 2) {
            std::sprintf(rowReply, "%d", -gDialogReply.firstLine);
            input::EscapeLegacyTextAndNotify_52D2D0(rowReply, reply, 255);
        } else if (gDialogReply.type == 35) {
            if ((button & 1) != 0) {
                if (texts[0][0] == '\0' && texts[1][0] == '\0') {
                    return 0;
                }
                if (input::Sub_476740(texts[0]) == 0) {
                    return 0;
                }
                if (input::Sub_476740(texts[1]) == 0) {
                    return 0;
                }
                if (input::Sub_45E770(texts[0]) == 1) {
                    return 0;
                }
                if (input::Sub_45E770(texts[1]) == 1) {
                    return 0;
                }
            }
            std::strcpy(first, texts[0]);
            input::EscapeLegacyTextAndNotify_52D2D0(first, first, 255);
            std::strcpy(second, texts[1]);
            input::EscapeLegacyTextAndNotify_52D2D0(second, second, 255);
            std::sprintf(combined, "%s|%s", first, second);
            input::EscapeLegacyText_52D1B0(combined, reply, 255);
        } else {
            input::EscapeLegacyTextAndNotify_52D2D0(
                inputCount == 1 ? texts[0] : input::LegacyText593850(), reply, 255);
        }
        const std::int32_t target = gDialogReply.target;
        const std::int32_t request = gDialogReply.request;
        const std::int32_t y =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t x =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, button, reply));
        world::detail::gValue581800 = -1;
        static_cast<void>(audio::PlaySe_52C450(79, 320));
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (row >= 0) {
        std::sprintf(reply, "%d", row + 1);
        input::EscapeLegacyTextAndNotify_52D2D0(reply, rowReply, 255);
        const std::int32_t target = gDialogReply.target;
        const std::int32_t request = gDialogReply.request;
        const std::int32_t y =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t x =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 0, rowReply));
        world::detail::gValue581800 = -1;
        static_cast<void>(audio::PlaySe_52C450(79, 320));
    }
    return 1;
}
static_assert(std::is_same_v<decltype(&Sub_49DA30), WindowDialogReplyCallback>);

void Sub_49DDB0(const char* const text, std::int16_t x, std::int16_t y)
{
    if (text[0] != 'g' || text[1] != ',') {
        return;
    }
    // 原32B区跨字段复用；空格只跳过写入，不压缩索引或初始化残留。
    std::array<char, 32> number;
    const char* cursor = text + 2;
    std::int32_t index = 0;
    while (cursor[index] != '\0' && cursor[index] != ',' && cursor[index] != '(') {
        if (cursor[index] != ' ') {
            number[index] = cursor[index];
        }
        ++index;
    }
    const char delimiter = cursor[index];
    number[index] = '\0';
    cursor += index + 1;
    const std::int32_t imageNo = std::atoi(number.data());
    std::uint8_t direction = 0;
    if (delimiter == ',') {
        ++cursor;
        index = 0;
        while (cursor[index] != '\0' && cursor[index] != ',' && cursor[index] != '(') {
            if (cursor[index] != ' ') {
                number[index] = cursor[index];
            }
            ++index;
        }
        number[index] = '\0';
        cursor += index + 1;
        direction = static_cast<std::uint8_t>(std::atoi(number.data()));
    }
    const char* const coordinates = std::strchr(cursor, '(');
    if (coordinates != nullptr) {
        cursor = coordinates + 1;
        if (std::strchr(cursor, ')') != nullptr) {
            index = 0;
            while (cursor[index] != '\0' && cursor[index] != ',') {
                if (cursor[index] != ' ') {
                    number[index] = cursor[index];
                }
                ++index;
            }
            number[index] = '\0';
            cursor += index + 1;
            x = static_cast<std::int16_t>(std::atoi(number.data()));
            index = 0;
            while (cursor[index] != '\0' && cursor[index] != ',') {
                if (cursor[index] != ' ') {
                    number[index] = cursor[index];
                }
                ++index;
            }
            number[index] = '\0';
            y = static_cast<std::int16_t>(std::atoi(number.data()));
        }
    }
    Sub_49C510(1, imageNo, direction, x, y);
}

login::YobiAction* Sub_49DF30(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::int32_t imageNo, const std::int32_t buttons,
    const std::int32_t request, const std::int32_t target, char* text)
{
    Sub_49C4D0();
    gDialogReply.type = world::detail::gValue581800;
    gDialogReply.request = request;
    gDialogReply.target = target;
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    const std::int32_t buttonHeight = buttons != 0 ? 24 : 0;
    const std::int32_t type = gDialogReply.type;
    std::int32_t inputHeight = 0;
    if (type == 1 || type == 11) {
        inputHeight = 28;
        gDialogInputYD67B34[0] = static_cast<std::int16_t>(height - buttonHeight - 44);
    }
    if (world::detail::gValue581800 == 35) {
        const std::int32_t contentHeight = height - inputHeight - buttonHeight;
        gDialogInputYD67B34[0] = static_cast<std::int16_t>(contentHeight - 118);
        gDialogInputYD67B34[1] = static_cast<std::int16_t>(contentHeight - 67);
    }
    std::int32_t lineHeight = 16;
    gDialogReply.firstLine = 0;
    if (type == 2) {
        lineHeight = 20;
        gDialogReply.firstLine = network::ParseDelimitedInteger_52CFA0(text, '\n', 1);
        text = std::strchr(text, '\n') + 1;
    }
    const std::int32_t columns = login::detail::Sub_49C5C0(static_cast<std::int16_t>(width));
    const std::int32_t lines = login::detail::Sub_49C5E0(
        static_cast<std::int16_t>(height - inputHeight), buttons, lineHeight);
    char* marker = input::FindLegacyByte_44B4A0(text, "@");
    while (marker != nullptr) {
        char* const content = marker + 1;
        if (content[0] == '@') {
            content[1] = static_cast<char>(0xFF);
        } else if (content[0] == 'g' && content[1] == ',') {
            char* const end = input::FindLegacyByte_44B4A0(content, "@");
            if (end == nullptr) {
                break;
            }
            *end = '\0';
            Sub_49DDB0(content,
                static_cast<std::int16_t>(static_cast<std::int16_t>(width) / 2),
                static_cast<std::int16_t>(static_cast<std::int16_t>(height) / 2));
            std::memset(marker, 0xFF, static_cast<std::size_t>(end - marker + 1));
        }
        marker = input::FindLegacyByte_44B4A0(content, "@");
    }
    char* output = text;
    for (char* cursor = text; ; ++cursor) {
        if (static_cast<std::uint8_t>(*cursor) == 0xFFu) {
            continue;
        }
        *output = *cursor;
        if (*cursor == '\0') {
            break;
        }
        ++output;
    }
    for (std::int32_t row = 0; row < lines; ++row) {
        static_cast<void>(input::ExtractLegacyLine_503AB0(
            gDialogLinesCB1398[row].text.data(), 81, text, row, columns));
    }
    if (world::detail::gValue581800 == 35) {
        return Sub_49C600(
            x, y, width, height, imageNo, buttons, gDialogLinesCB1398.data(),
            lines, lineHeight, gDialogReply.firstLine, 2, gDialogInputYD67B34.data(),
            columns, 2, 2, &Sub_49DA30, nullptr, 1, 0);
    }
    return Sub_49C600(
        x, y, width, height, imageNo, buttons, gDialogLinesCB1398.data(),
        lines, lineHeight, gDialogReply.firstLine, inputHeight != 0 ? 1 : 0,
        inputHeight != 0 ? gDialogInputYD67B34.data() : nullptr,
        columns, 2, 0, &Sub_49DA30, nullptr, 1, 0);
}

std::int32_t Sub_49E1D0(
    WindowInstance* const window, const std::int32_t button,
    std::int32_t row, const std::int32_t)
{
    static_cast<void>(login::detail::Sub_49C5C0(static_cast<std::int16_t>(window->width)));
    const std::int32_t visibleLines = login::detail::Sub_49C5E0(
        static_cast<std::int16_t>(window->height), 1, 16);
    if (button != 0) {
        std::strcpy(gHoverTextD68AE8.data(), "\xA4\xA3\xAA\x76\xC0\xF8\xA4\x46\xA1\x43");
    }
    std::array<char, 64> buffer;
    for (std::int32_t index = 0; index < 7; ++index) {
        if (gHealingDialogRecordsD672F0[index].level != 0) {
            std::sprintf(buffer.data(), "%7d G", gHealingDialogRecordsD672F0[index].cost);
            static_cast<void>(render::DrawText_456780(
                static_cast<std::int16_t>(window->x + 336),
                static_cast<std::int16_t>(window->y + index * 16 + 52),
                3, 0, 0, buffer.data(), 0, static_cast<std::int8_t>(login::detail::gValueD69768)));
            if (index < 6) {
                const std::int32_t imageIndex = Sub_519680(gHealingDialogRecordsD672F0[index].health);
                static_cast<void>(login::detail::DrawGraphic(
                    window->x + 68, window->y + index * 16 + 60,
                    223, gHealthImages57CBA4[imageIndex], 0, nullptr));
            }
        }
    }
    std::sprintf(buffer.data(), "               \xA9\xD2\xAB\xF9\xAA\xF7 %7d \xAA\xF7\xB9\xF4",
        world::detail::ReadPlayerValueFC());
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(window->x + 24),
        static_cast<std::int16_t>(window->y + 7 * 16 + 68),
        3, 0, 0, buffer.data(), 0, static_cast<std::int8_t>(login::detail::gValueD69768)));
    if (row >= visibleLines) {
        row = -1;
    }
    // row=-1时原前缀读取无论为何值都仍得到-1，只消去这次无结果读取。
    if (row != -1 && gDialogLinesCB1398[row].text[0] == '\0') {
        row = -1;
    }
    const std::int32_t first = gDialogReply.firstLine;
    if (row >= first && row <= 8) {
        const char* text;
        if (row == 8) {
            text = gHealingDialogHealthD2DC00 != 0
                ? "\xBD\xD0\xA8\x44\xAA\x76\xC0\xF8\xA9\xD2\xA6\xB3\xA8\xFC\xB6\xCB"
                  "\xAA\xBA\xA4\x48\xAA\xAB\xA1\x43"
                : "\xA8\x53\xA6\xB3\xA8\xFC\xB6\xCB\xAA\xBA\xA4\x48\xAA\xAB\xA1\x43";
        } else {
            text = gHealingDialogRecordsD672F0[row - first].health != 0
                ? "\xBD\xD0\xA8\x44\xAA\x76\xC0\xF8\xA6\xB9\xA4\x48\xAA\xAB"
                  "\xAA\xBA\xB6\xCB\xB6\xD5\xA1\x43"
                : "\xA6\xB9\xA4\x48\xAA\xAB\xA8\x53\xA6\xB3\xA8\xFC\xB6\xCB\xA1\x43";
        }
        std::strcpy(gHoverTextD68AE8.data(), text);
    }
    return 0;
}
static_assert(std::is_same_v<decltype(&Sub_49E1D0), WindowDialogFrameCallback>);

login::YobiAction* Sub_49E400(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t width, const std::int32_t height,
    const std::int32_t imageNo, const std::int32_t,
    const std::int32_t request, const std::int32_t target, const char* const text)
{
    Sub_49C4D0();
    gDialogReply.type = world::detail::gValue581800;
    gDialogReply.request = request;
    gDialogReply.target = target;
    for (auto& record : gHealingDialogRecordsD672F0) {
        record.level = 0;
    }
    std::int32_t totalCost = 0;
    std::int32_t field = 1;
    gHealingDialogHealthD2DC00 = 0;
    std::array<char, 512> buffer;
    char* const name = buffer.data() + 256;
    for (std::int32_t index = 0; index < 6; ++index) {
        auto& record = gHealingDialogRecordsD672F0[index];
        record.level = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        if (record.level < 0) {
            record.level = 0;
            break;
        }
        record.health = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field++, 255, name));
        input::UnescapeLegacyTextAndNotify_52D2F0(name);
        std::strcpy(record.name.data(), std::strlen(name) > 16 ? "???" : name);
        const std::int32_t cost = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        const std::int32_t totalHealth = gHealingDialogHealthD2DC00;
        record.cost = cost;
        totalCost += cost;
        gHealingDialogHealthD2DC00 = totalHealth + record.health;
    }
    gHealingDialogRecordsD672F0[6].level = 1;
    gHealingDialogRecordsD672F0[6].cost = totalCost;
    std::sprintf(buffer.data(), "         \xBD\xD0\xBF\xEF\xBE\xDC\xAA\x76\xC0\xF8\xB9\xEF\xB6\x48");
    std::strcpy(gDialogLinesCB1398[0].text.data(), buffer.data());
    std::sprintf(buffer.data(), "   Health   LV        Name             Gold");
    std::strcpy(gDialogLinesCB1398[1].text.data(), buffer.data());
    for (std::int32_t index = 0; index < 6; ++index) {
        const auto& record = gHealingDialogRecordsD672F0[index];
        if (record.level != 0) {
            std::sprintf(buffer.data(), "           %3d  %-16s", record.level, record.name.data());
            std::strcpy(gDialogLinesCB1398[index + 2].text.data(), buffer.data());
        } else {
            gDialogLinesCB1398[index + 2].text[0] = '\0';
        }
    }
    std::sprintf(buffer.data(), "           \xA7\xB9\xBE\xE3\xAA\x76\xC0\xF8");
    std::strcpy(gDialogLinesCB1398[8].text.data(), buffer.data());
    buffer[0] = '\0'; // 原String1@D68A7C空格式的结果，不借用593850。
    std::strcpy(gDialogLinesCB1398[9].text.data(), buffer.data());
    std::strcpy(gDialogLinesCB1398[10].text.data(), input::LegacyText593850());
    gDialogReply.firstLine = 2;
    const std::int32_t columns = login::detail::Sub_49C5C0(static_cast<std::int16_t>(width));
    const std::int32_t first = gDialogReply.firstLine;
    return Sub_49C600(
        x, y, width, height, imageNo, 2, gDialogLinesCB1398.data(), first + 9,
        16, first, 0, nullptr, columns, 2, 0, &Sub_49DA30, &Sub_49E1D0, 1, 0);
}

login::YobiAction* Sub_49FCA0(
    const char* const title, const std::int32_t imageNo,
    const char* const* const lines, const std::uint8_t lineCount,
    const char* const* const choices, const std::uint8_t choiceCount,
    const char* const* const hints, const WindowChoiceCallback callback)
{
    gChoiceDialog.type = world::detail::gValue581800;
    gChoiceDialog.title = title;
    gChoiceDialog.lines = lines;
    gChoiceDialog.imageNo = imageNo;
    gChoiceDialog.choiceCount = choiceCount;
    WindowInstance* const previous = gWindowStates[52].window;
    gChoiceDialog.lineCount = lineCount;
    gChoiceDialog.choices = choices;
    gChoiceDialog.callback = callback;
    gChoiceDialog.hints = hints;
    if (previous != nullptr) {
        previous->state |= 0x8000u;
        gChoiceDialog.closeEnabled = 0;
    }
    return CreateWindowAnimation_475AA0(52, 1, 0, nullptr);
}

std::int32_t Sub_49FD20(const std::int32_t value)
{
    if (value == -1) {
        auto* const title = static_cast<WindowTextData*>(gCurrentWindow->elements[0].data);
        std::strcpy(title->text, gChoiceDialog.title);
        const std::int32_t measured = login::detail::TextMeasure(gChoiceDialog.title, 1);
        WindowInstance* const window = gCurrentWindow;
        window->elements[0].x = ((window->width - measured) >> 1) + 6;
        auto* animation = static_cast<WindowAnimationData*>(gCurrentWindow->elements[1].data);
        login::detail::SetYobiActionField2A8(animation->action, gChoiceDialog.imageNo);
        animation = static_cast<WindowAnimationData*>(gCurrentWindow->elements[1].data);
        login::detail::SetYobiActionField2B8(animation->action, 5);
        const std::uint8_t count = gChoiceDialog.choiceCount;
        gCurrentWindow->elements[2].height =
            count != 0 ? 14 * std::bit_cast<std::int8_t>(count) - 2 : 0;
        gChoiceDialog.closeEnabled = 1;
    }
    if (Sub_4D3040(2, 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        static_cast<void>(gChoiceDialog.callback(-1));
    }
    return 1;
}

std::int32_t Sub_49FE40(const std::int32_t)
{
    DrawCurrentWindow_4729B0();
    for (std::int32_t row = 0; row < std::bit_cast<std::int8_t>(gChoiceDialog.lineCount); ++row) {
        const char* const text = gChoiceDialog.lines[row];
        if (text != nullptr) {
            WindowInstance* const window = gCurrentWindow;
            static_cast<void>(render::DrawText_456780(
                static_cast<std::int16_t>(window->x + 87),
                static_cast<std::int16_t>(window->y + 40 + row * 13),
                3, 12, 0, text, 0, 0));
        }
    }
    for (std::int32_t row = 0; row < std::bit_cast<std::int8_t>(gChoiceDialog.choiceCount); ++row) {
        const char* const text = gChoiceDialog.choices[row];
        if (text != nullptr) {
            WindowInstance* const window = gCurrentWindow;
            const auto& element = window->elements[2];
            static_cast<void>(render::DrawText_456780(
                static_cast<std::int16_t>(window->x + element.x),
                static_cast<std::int16_t>(window->y + element.y + row * 14),
                3, 12, 0, text, 0, 0));
        }
    }
    WindowInstance* const window = gCurrentWindow;
    login::detail::DrawNineGrid_49BE00(245300, static_cast<std::int16_t>(window->x),
        static_cast<std::int16_t>(window->y), static_cast<std::int16_t>(window->width),
        static_cast<std::int16_t>(window->height), 223, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    if (world::detail::gValue581800 != gChoiceDialog.type) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

void Sub_49FF70()
{
    if (gChoiceDialog.closeEnabled != 0 && world::detail::gValue581800 == gChoiceDialog.type) {
        world::detail::gValue581800 = -1;
    }
}

std::int32_t Sub_49FFA0(const std::int32_t, const std::int32_t state)
{
    if ((state & 1) == 0) {
        return 0;
    }
    for (std::int32_t index = 0;
         index < std::bit_cast<std::int8_t>(gChoiceDialog.choiceCount); ++index) {
        WindowInstance* const window = gCurrentWindow;
        const auto& element = window->elements[2];
        const auto x = static_cast<std::int16_t>(window->x + element.x);
        const auto y = static_cast<std::int16_t>(window->y + element.y + index * 14);
        if (login::detail::Sub_5049A0(x, y, x + element.width, y + 12, -1) == 0) {
            continue;
        }
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x - 2), static_cast<std::uint16_t>(y - 2),
            static_cast<std::uint16_t>(x + gCurrentWindow->elements[2].width + 2),
            static_cast<std::uint16_t>(y + 14), 223, login::detail::gValueD69768, 0);
        std::strcpy(gHoverTextD68AE8.data(), gChoiceDialog.hints[index]);
        if ((platform::gLegacyMouseState.edgeButtons & 1u) != 0) {
            const WindowChoiceCallback callback = gChoiceDialog.callback;
            if (callback != nullptr) {
                static_cast<void>(callback(index));
            }
            static_cast<void>(audio::PlaySe_52C450(79, 320));
            gCurrentWindow->state |= 0x8000u;
            break;
        }
    }
    return 1;
}

std::int32_t Sub_4A00C0(const std::int32_t index)
{
    if (index < 0 || index > 1) {
        return index;
    }
    std::array<char, 32> buffer;
    std::sprintf(buffer.data(), "%d", static_cast<std::int32_t>(gShopCommandsD64E10[index]));
    input::EscapeLegacyTextAndNotify_52D2D0(buffer.data(), buffer.data() + 16, 15);
    const std::int32_t target = world::detail::gDialogTargetD2DE6C;
    const std::int32_t request = world::detail::gDialogRequestCC21E0;
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    return network::Sub_512620(
        platform::gConnectionSocket, x, y, request, target, 0, buffer.data() + 16);
}
static_assert(std::is_same_v<decltype(&Sub_4A00C0), WindowChoiceCallback>);

login::YobiAction* Sub_4A0140(const char* const text)
{
    gShopImageC9A9A0 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    std::array<char, 256> body;
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 255, body.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(body.data());
    for (std::int32_t row = 0; row < 5; ++row) {
        char* const line = gNpcDialogLinesCC24D4[row + 1].text.data();
        static_cast<void>(input::ExtractLegacyLine_503AB0(line, 81, body.data(), row, 20));
        gChoiceDialogLinesD672D8[row] = line;
    }
    const auto mask = static_cast<std::uint8_t>(network::ParseDelimitedInteger_52CFA0(text, '|', 4));
    std::uint8_t count = 0;
    if ((mask & 1u) != 0) {
        gShopChoicesCB1DC8[0] = kShopChoices57E6B8[0];
        gShopHintsD2F738[0] = kShopHints57E6C4[0];
        gShopCommandsD64E10[0] = 1;
        count = 1;
    }
    if ((mask & 2u) != 0) {
        gShopChoicesCB1DC8[count] = kShopChoices57E6B8[1];
        gShopHintsD2F738[count] = kShopHints57E6C4[1];
        gShopCommandsD64E10[count] = 2;
        ++count;
    }
    gShopChoicesCB1DC8[count] = kShopChoices57E6B8[2];
    const std::int32_t imageNo = gShopImageC9A9A0;
    gShopHintsD2F738[count] = kShopHints57E6C4[2];
    gShopCommandsD64E10[count] = 3;
    return Sub_49FCA0(title, imageNo, gChoiceDialogLinesD672D8.data(), 5,
        gShopChoicesCB1DC8.data(), static_cast<std::uint8_t>(count + 1),
        gShopHintsD2F738.data(), &Sub_4A00C0);
}

std::int32_t Sub_4A0280(const std::int32_t index)
{
    if (index < 0 || index > 1) {
        return index;
    }
    std::array<char, 32> buffer;
    std::sprintf(buffer.data(), "%d", static_cast<std::int32_t>(gChoiceCommandsCB03D4[index]));
    return SendChoiceDialogReply(buffer.data());
}

std::int32_t Sub_4A0420(const std::int32_t index)
{
    if (index < 0 || index > 1) {
        return index;
    }
    std::array<char, 32> buffer;
    std::sprintf(buffer.data(), "%d", static_cast<std::int32_t>(gChoiceCommandsCF47D8[index]));
    return SendChoiceDialogReply(buffer.data());
}

std::int32_t Sub_4A05C0(const std::int32_t index)
{
    if (index < 0 || index > 1) {
        return index;
    }
    std::array<char, 32> buffer;
    std::sprintf(buffer.data(), "%d", index + 1);
    return SendChoiceDialogReply(buffer.data());
}

std::int32_t Sub_4A0700(const std::int32_t index)
{
    if (index != 0) {
        return index;
    }
    std::array<char, 32> buffer;
    std::strcpy(buffer.data(), "1");
    return SendChoiceDialogReply(buffer.data());
}
static_assert(std::is_same_v<decltype(&Sub_4A0280), WindowChoiceCallback>);
static_assert(std::is_same_v<decltype(&Sub_4A0420), WindowChoiceCallback>);
static_assert(std::is_same_v<decltype(&Sub_4A05C0), WindowChoiceCallback>);
static_assert(std::is_same_v<decltype(&Sub_4A0700), WindowChoiceCallback>);

login::YobiAction* Sub_4A0300(const char* const text)
{
    gChoiceImageCB0AE4 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    FillChoiceDialogBody(text, 3, 255);
    std::uint8_t count = 0;
    if ((network::ParseDelimitedInteger_52CFA0(text, '|', 4) & 1) != 0) {
        gChoicesD67438[0] = kShopChoices57E6B8[0];
        gChoiceHintsCC31FC[0] = kShopHints57E6C4[0];
        gChoiceCommandsCB03D4[0] = 1;
        count = 1;
    }
    const std::int32_t imageNo = gChoiceImageCB0AE4;
    gChoicesD67438[count] = kShopChoices57E6B8[2];
    gChoiceHintsCC31FC[count] = kShopHints57E6C4[2];
    gChoiceCommandsCB03D4[count] = 3;
    return Sub_49FCA0(title, imageNo, gChoiceDialogLinesD672D8.data(), 5,
        gChoicesD67438.data(), static_cast<std::uint8_t>(count + 1),
        gChoiceHintsCC31FC.data(), &Sub_4A0280);
}

login::YobiAction* Sub_4A04A0(const char* const text)
{
    gChoiceImageCF4A18 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    FillChoiceDialogBody(text, 3, 255);
    std::uint8_t count = 0;
    if ((network::ParseDelimitedInteger_52CFA0(text, '|', 4) & 1) != 0) {
        gChoicesD2D818[0] = kShopChoices57E6B8[0];
        gChoiceHintsCA0AAC[0] = kShopHints57E6C4[0];
        gChoiceCommandsCF47D8[0] = 1;
        count = 1;
    }
    const std::int32_t imageNo = gChoiceImageCF4A18;
    gChoicesD2D818[count] = kShopChoices57E6B8[2];
    gChoiceHintsCA0AAC[count] = kShopHints57E6C4[2];
    gChoiceCommandsCF47D8[count] = 3;
    return Sub_49FCA0(title, imageNo, gChoiceDialogLinesD672D8.data(), 5,
        gChoicesD2D818.data(), static_cast<std::uint8_t>(count + 1),
        gChoiceHintsCA0AAC.data(), &Sub_4A0420);
}

login::YobiAction* Sub_4A0640(const char* const text)
{
    gChoiceImageCC4890 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    FillChoiceDialogBody(text, 3, 255);
    return Sub_49FCA0(title, gChoiceImageCC4890, gChoiceDialogLinesD672D8.data(), 5,
        kSkillChoices57E6F0.data(), 3, kSkillHints57E6FC.data(), &Sub_4A05C0);
}

login::YobiAction* Sub_4A0790(const char* const text)
{
    gChoiceImageC9A99C = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    FillChoiceDialogBody(text, 3, 255);
    return Sub_49FCA0(title, gChoiceImageC9A99C, gChoiceDialogLinesD672D8.data(), 5,
        kExchangeChoices57E708.data(), 2, kExchangeHints57E714.data(), &Sub_4A0700);
}

login::YobiAction* Sub_4A0850(const char* const text)
{
    gChoiceImageD2EA74 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 21, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    const std::int32_t receive = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    gGiveReceiveChoices57E728[0] = kGiveReceiveLabels57E720[0];
    if (receive != 0) {
        gGiveReceiveChoices57E728[0] = kGiveReceiveLabels57E720[1];
    }
    FillChoiceDialogBody(text, 4, 256);
    return Sub_49FCA0(title, gChoiceImageD2EA74, gChoiceDialogLinesD672D8.data(), 5,
        gGiveReceiveChoices57E728.data(), 2, kGiveReceiveHints57E730.data(), &Sub_4A05C0);
}

std::int32_t Sub_499930(const std::int32_t, const std::int32_t state)
{
    return Sub_4C78A0(&gItemSelectorD2F338, state) ? 1 : 0;
}

std::int32_t Sub_499950(const std::int32_t, const std::int32_t state)
{
    return Sub_4C7910(&gItemSelectorD2F338, state) ? 1 : 0;
}

std::int32_t Sub_499970(const std::int32_t, const std::int32_t state)
{
    static_cast<void>(Sub_4C7A10(&gItemSelectorD2F338, state));
    return 0;
}

std::int32_t Sub_499B00(const std::int32_t, const std::int32_t)
{
    std::sprintf(gBattleMoneyTextCF469C.data(), "%10d", world::detail::ReadPlayerValueFC());
    return 0;
}

std::int32_t Sub_499B20(const std::int32_t, const std::int32_t)
{
    std::sprintf(gBattleValueTextC8A960.data(), "%10d", world::detail::ReadPlayerValue15BCC());
    return 0;
}

std::int32_t Sub_499B40(const std::int32_t, const std::int32_t state)
{
    static_cast<void>(Sub_4C7380(&gItemSelectorD2F338, 0, state));
    return 0;
}

std::int32_t Sub_499B60(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t ordinal = index - 17;
    std::int32_t remaining = ordinal;
    bool found = false;
    for (std::int32_t position = 0; position < 5; ++position) {
        const auto& pair = world::detail::gPetPairsDE0B08[position];
        if (pair.used == 0) {
            continue;
        }
        const std::int32_t pet = std::bit_cast<std::int16_t>(pair.index);
        const std::uint16_t flags = world::detail::gPetRuntimeF633F8[pet].flags6AA;
        if (flags == 2 || flags == 1) {
            --remaining;
            if (remaining < 0) {
                found = true;
                break;
            }
        }
    }
    if (found) {
        if (ordinal >= 0) {
            static_cast<void>(Sub_4C7380(&gItemSelectorD2F338, ordinal + 1, state));
            return 0;
        }
    } else if (std::bit_cast<std::int8_t>(gValue57E42C) == ordinal + 1) {
        --gValue57E42C;
    }
    std::int32_t consumed = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
        consumed = 1;
    }
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 244306;
    return consumed;
}

std::int32_t Sub_499990(const std::int32_t value)
{
    if (value == -1) {
        Sub_4C6EC0(&gItemSelectorD2F338, 1);
        gItemSelectorD2F338.initialized3E8 = 0;
        gBattleItemPageCB01E0 = 0;
    }
    gBattleItemHoverCF49B0 = -1;
    Sub_4C7A90(&gItemSelectorD2F338);
    return 1;
}

std::int32_t Sub_4999D0(const std::int32_t)
{
    const std::int32_t hovered = gBattleItemHoverCF49B0;
    if (hovered >= 0) {
        static_cast<void>(Sub_48B1A0(hovered, gBattleItemPageCB01E0));
    } else {
        const std::int32_t kind = Sub_475D60();
        if (kind == 2) {
            if (Sub_475D40() == 27 && (platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
                static_cast<void>(Sub_4763E0());
            }
        } else if (kind == 0 && (platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            gCurrentWindow->state |= 0x8000u;
        }
    }
    const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
    WindowInstance* const window = gCurrentWindow;
    login::YobiAction* const source = world::detail::gBattleEntries59E980[position];
    auto* const data = static_cast<WindowAnimationData*>(window->elements[4].data);
    const std::int32_t imageNo = login::detail::GetYobiActionField2A8(source);
    login::detail::SetYobiActionField2A8(data->action, imageNo);
    login::detail::SetYobiActionField2B8(
        static_cast<WindowAnimationData*>(gCurrentWindow->elements[4].data)->action, 5);
    login::detail::SetYobiActionField309(
        static_cast<WindowAnimationData*>(gCurrentWindow->elements[4].data)->action, 0);
    WindowElement* const elements = gCurrentWindow->elements;
    if (elements[4].visible != 0) {
        const std::int32_t currentImage = login::detail::GetYobiActionField2A8(
            world::detail::gBattleEntries59E980[world::detail::gBattleCurrentPosition5ABCA4]);
        if (currentImage >= 110350 && currentImage <= 110373) {
            login::detail::SetYobiActionField309(
                static_cast<WindowAnimationData*>(elements[4].data)->action, 4);
            login::detail::SetYobiActionField310(
                static_cast<WindowAnimationData*>(gCurrentWindow->elements[4].data)->action, 4);
        }
    }
    DrawCurrentWindow_4729B0();
    return 1;
}

std::int32_t Sub_499C30(const std::int32_t index, const std::int32_t state)
{
    static constexpr std::int32_t kBombType = 51;
    static constexpr std::int32_t kMedicineType = 43;
    static constexpr std::int32_t levelLimits[] {
        9, 19, 29, 39, 49, 59, 69, 79, 89, 99, 109, 119, 129, 139, 149,
    };
    static constexpr char noSpace[] = "\xAA\xAB\xAB\x7E\xC4\xE6\xA6\xEC\xA4\xA3\xA8\xAC\xA1\x49";
    static constexpr char noWeapon[] = "\xA8\x53\xA6\xB3\xB8\xCB\xB3\xC6\xAA\x5A\xBE\xB9\xA1\x49";
    static constexpr char unidentified[] =
        "\xB5\x4C\xAA\x6B\xB8\xCB\xB3\xC6\xA5\xBC\xC5\xB2\xA9\x77\xAA\xAB\xAB\x7E\xA1\x49";
    static constexpr char canEquip[] = "\xA5\x69\xB8\xCB\xB3\xC6\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
    static constexpr char cannotEquip[] =
        "\xB5\x4C\xAA\x6B\xB8\xCB\xB3\xC6\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
    static constexpr char canUse[] = "\xA5\x69\xA8\xCF\xA5\xCE\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
    static constexpr char cannotUse[] =
        "\xB5\x4C\xAA\x6B\xA8\xCF\xA5\xCE\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
    std::int32_t skillIndex = -1;
    std::int32_t usableLevel = -1;
    for (std::int32_t position = 0; position < 15; ++position) {
        const auto& skill = world::detail::gSkillsF1BAFC[position];
        if (skill.skillId != 2002) {
            continue;
        }
        skillIndex = position;
        for (std::int32_t technique = 0; technique < 15; ++technique) {
            if (skill.techniques3C[technique].value8C <= 0) {
                break;
            }
            usableLevel = technique + 1;
        }
        break;
    }
    const std::int32_t playerLevel = world::detail::ReadPlayerLevel();
    std::int32_t levelLimit = 1;
    while (levelLimit < 15 && playerLevel > levelLimits[levelLimit - 1]) {
        ++levelLimit;
    }
    if (usableLevel > levelLimit) {
        usableLevel = levelLimit;
    }
    // 原技能+24+94H*等级；等级0是experience，不能改作技术数组[-1]。
    const auto readBombCost = [skillIndex](const std::int32_t level) {
        const auto* const skill = reinterpret_cast<const std::byte*>(
            &world::detail::gSkillsF1BAFC[skillIndex]);
        const std::ptrdiff_t offset =
            static_cast<std::ptrdiff_t>(offsetof(world::detail::SkillRuntimeRecord, experience))
            + static_cast<std::ptrdiff_t>(level)
                * static_cast<std::ptrdiff_t>(sizeof(world::detail::SkillTechniqueRecord));
        std::int32_t cost;
        std::memcpy(&cost, skill + offset, sizeof(cost));
        return cost;
    };
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::memcpy(dimmed.bytes.data() + 8, &dimmedColor, sizeof(dimmedColor));
    descriptor.bytes[13] = 4;
    dimmed.bytes[13] = 4;
    auto& items = world::detail::gPlayerItemsFDA8A4;
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gValueD67B08.data());
    std::uint8_t events = static_cast<std::uint8_t>(state);
    std::int32_t consumed = 0;
    std::array<char, 12> countText;
    if (std::bit_cast<std::int8_t>(gValue57E42C) > 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 15;
            const std::int32_t top = element.y + 50 * (position / 5) + window->y + 48;
            const auto& item = items[position + 8];
            if ((events & 0x21u) != 0) {
                if (occupancy[position + 8] != 0) {
                    continue;
                }
                if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0
                    && item.used != 0) {
                    const std::int32_t type = item.itemType;
                    std::strcpy(gHoverTextD68AE8.data(),
                        ((type >= 0 && type <= 7) || (type >= 70 && type <= 79))
                            ? cannotEquip : cannotUse);
                    if ((events & 1u) != 0) {
                        login::detail::DrawGraphicHover(
                            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                            223, login::detail::gValueD69768, 0);
                    }
                }
                consumed = 1;
            }
            if (item.used == 0) {
                continue;
            }
            static_cast<void>(login::detail::DrawGraphic(left + 129, top + 118, 223, 22738, 0, nullptr));
            if (occupancy[position + 8] != 0) {
                login::detail::DrawGraphicHover(
                    static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                    223, 249, 0);
                static_cast<void>(login::detail::DrawGraphic(
                    left + 24, top + 24, 223, item.imageNo, 0, &dimmed));
            } else {
                static_cast<void>(login::detail::DrawGraphic(
                    left + 24, top + 24, 223, item.imageNo, 0, &descriptor));
            }
            if (item.count > 0) {
                std::sprintf(countText.data(), "%3d", item.count);
                static_cast<void>(render::DrawText_456780(
                    left + 21, top + 31, 3, 1, 1, countText.data(), 0, 0));
            }
        }
        return consumed;
    }
    if ((events & 0x60u) != 0 && Sub_475D60() != 2) {
        events &= 0x9Fu;
        consumed = 1;
    }
    std::array<char, 256> buffer;
    if ((events & 0x40u) != 0) {
        const std::int32_t source = Sub_475D90();
        if (source == 2 || source == 3) {
            std::int32_t weapons = 0;
            if (items[2].used == 1 && items[2].itemType != 7) {
                weapons = 1;
            }
            if (items[3].used == 1 && items[3].itemType != 7) {
                ++weapons;
            }
            if (world::detail::Sub_518C60() < weapons) {
                world::detail::Sub_44B050(noSpace, 4, 3);
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else if (weapons == 0) {
                world::detail::Sub_44B050(noWeapon, 4, 3);
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                gCurrentWindow->state |= 0x8000u;
                world::detail::gBattleSelectionModes5ABC7C[5] = 0;
                std::sprintf(buffer.data(), "Q|%X|", 255u);
                static_cast<void>(network::SendBattlePacket_5109B0(
                    platform::gConnectionSocket, buffer.data()));
                world::detail::gBattlePlayerActionState5ABC74 = 1;
                static_cast<void>(audio::PlaySe_52C450(57, 320));
            }
        }
        Sub_476500();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 15;
            const std::int32_t top = element.y + 50 * (position / 5) + window->y + 48;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
        if (hovered >= 0 && items[hovered + 8].used != 0) {
            const auto& item = items[hovered + 8];
            const std::int32_t type = item.itemType;
            const char* hint = cannotUse;
            if (type < 23) {
                hint = ((type >= 0 && type <= 7) || (type >= 70 && type <= 79))
                    && item.hasDescription != 0 ? canEquip : cannotEquip;
            } else if ((item.battleUseFlag & 1u) != 0) {
                if (item.hasDescription != 0) {
                    if (type == kMedicineType) {
                        if ((world::detail::gBattlePlayerFlags5ABCAC & 0x40000u) == 0) {
                            hint = canUse;
                        }
                    } else if (type == kBombType) {
                        const std::int32_t level = item.level;
                        world::detail::gBattleBombLevelD67B2C = level;
                        const std::int32_t cost = readBombCost(level);
                        if (usableLevel >= level && items[2].used == 0 && items[3].used == 0
                            && !world::detail::IsPlayerFpLessThan(cost)
                            && world::detail::gBattleModeFlag5ABFCC != 1) {
                            hint = canUse;
                        }
                    } else {
                        hint = canUse;
                    }
                }
            } else {
                std::array<char, 512> name;
                std::strcpy(name.data(), item.name.data());
                std::strcpy(gHoverTextD68AE8.data(), canUse);
            }
            std::strcpy(gHoverTextD68AE8.data(), hint);
        }
    }
    if (gBattleBackpackHover57E430 != hovered) {
        gBattleItemPageCB01E0 = 0;
    }
    gBattleBackpackHover57E430 = hovered;
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (Sub_475D40() == 27 || Sub_475D40() == 0) {
            const auto& item = items[hovered + 8];
            bool useItem = false;
            if (item.used != 1) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else if ((item.battleUseFlag & 1u) != 0 && item.itemType == kBombType) {
                const std::int32_t cost = readBombCost(items[hovered + 8].level);
                if (usableLevel < item.level || items[2].used != 0 || items[3].used != 0
                    || world::detail::IsPlayerFpLessThan(cost)
                    || world::detail::gBattleModeFlag5ABFCC == 1) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    useItem = true;
                }
            } else {
                const std::int32_t type = item.itemType;
                if ((type >= 0 && type <= 7) || (type >= 70 && type <= 79)) {
                    std::int32_t occupiedHands = items[2].used == 1;
                    if (items[3].used == 1) {
                        ++occupiedHands;
                    }
                    // 原49A510以未加8的hover索引读取battleUseFlag。
                    if (occupiedHands == 2 && world::detail::Sub_518C60() == 0
                        && (items[hovered].battleUseFlag & 8u) != 0) {
                        world::detail::Sub_44B050(noSpace, 4, 3);
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    } else if (item.hasDescription == 0) {
                        world::detail::Sub_44B050(unidentified, 4, 3);
                        static_cast<void>(audio::PlaySe_52C450(55, 320));
                    } else {
                        gCurrentWindow->state |= 0x8000u;
                        world::detail::gBattleSelectionModes5ABC7C[5] = 0;
                        std::sprintf(buffer.data(), "Q|%X|", static_cast<std::uint32_t>(hovered + 8));
                        static_cast<void>(network::SendBattlePacket_5109B0(
                            platform::gConnectionSocket, buffer.data()));
                        world::detail::gBattlePlayerActionState5ABC74 = 1;
                        static_cast<void>(audio::PlaySe_52C450(77, 320));
                    }
                } else if ((item.battleUseFlag & 1u) == 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else if (item.hasDescription == 0) {
                    world::detail::Sub_44B050(unidentified, 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else if (item.itemType == kMedicineType
                    && (world::detail::gBattlePlayerFlags5ABCAC & 0x40000u) != 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    useItem = true;
                }
            }
            if (useItem) {
                gCurrentWindow->state |= 0x8000u;
                const std::int32_t position = world::detail::gBattleCurrentPosition5ABCA4;
                const auto target = static_cast<std::uint32_t>(
                    static_cast<std::int32_t>(std::bit_cast<std::int16_t>(item.target)));
                if (login::detail::CheckBattleCommand_43C560(position, target) != 0) {
                    world::detail::gBattleSelectedItem5ABC48 = hovered + 8;
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                    world::detail::gBattleItemTargetPending5ABCC4 = 1;
                    gBattleItemTargetFlagCC4D40 = 1;
                } else {
                    const std::int32_t currentPosition = world::detail::gBattleCurrentPosition5ABCA4;
                    world::detail::gBattleSelectedItem5ABC48 = hovered + 8;
                    std::sprintf(buffer.data(), "I|%X|%X",
                        static_cast<std::uint32_t>(hovered + 8),
                        static_cast<std::uint32_t>(currentPosition));
                    static_cast<void>(network::SendBattlePacket_5109B0(
                        platform::gConnectionSocket, buffer.data()));
                    login::detail::FlushBattleEntries_43C060();
                    world::detail::gBattlePlayerActionState5ABC74 = 1;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                }
            }
            static_cast<void>(Sub_4763E0());
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            const auto& item = items[hovered + 8];
            bool canDrag;
            if (item.itemType == kBombType) {
                const std::int32_t cost = readBombCost(items[hovered + 8].level);
                canDrag = usableLevel >= item.level && items[2].used == 0 && items[3].used == 0
                    && !world::detail::IsPlayerFpLessThan(cost)
                    && world::detail::gBattleModeFlag5ABFCC != 1;
            } else {
                canDrag = item.used != 0;
            }
            if (canDrag) {
                const std::int32_t type = item.itemType;
                if (((item.battleUseFlag & 1u) != 0
                        || (type >= 0 && type <= 7) || (type >= 70 && type <= 79))
                    && (item.itemType != kMedicineType
                        || (world::detail::gBattlePlayerFlags5ABCAC & 0x40000u) == 0)) {
                    static_cast<void>(Sub_48BF10(hovered + 8, 0));
                    static_cast<void>(audio::PlaySe_52C450(51, 320));
                }
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            const auto& item = items[hovered + 8];
            if (item.used != 0) {
                const std::int32_t pages = item.pages;
                const std::int32_t page = gBattleItemPageCB01E0 + 1;
                gBattleItemPageCB01E0 = page;
                if (page >= pages) {
                    gBattleItemPageCB01E0 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (Sub_475D60() == 2) {
        const std::int32_t source = Sub_475D90();
        if (source >= 8) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * ((source - 8) % 5) + window->x + 15;
            const std::int32_t top = element.y + 50 * ((source - 8) / 5) + window->y + 48;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 15;
        const std::int32_t top = element.y + 50 * (hovered / 5) + window->y + 48;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        if (items[hovered + 8].used != 0) {
            gBattleItemHoverCF49B0 = hovered + 8;
        }
    }
    if (Sub_475D60() != 0 && (events & 0x20u) != 0 && Sub_475D90() < 8) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + window->x;
        const std::int32_t top = element.y + window->y;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + element.width),
            static_cast<std::uint16_t>(top + element.height),
            223, login::detail::gValueD69768, 0);
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = items[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (position % 5) + window->x + 39;
        const std::int32_t y = element.y + 50 * (position / 5) + window->y + 73;
        if (item.itemType == kBombType) {
            const std::int32_t level = item.level;
            const std::int32_t cost = readBombCost(level);
            if (usableLevel < level || items[2].used != 0 || items[3].used != 0
                || world::detail::IsPlayerFpLessThan(cost)
                || world::detail::gBattleModeFlag5ABFCC == 1) {
                static_cast<void>(login::detail::DrawGraphic(
                    x + 105, y + 93, 223, 22738, 0, nullptr));
            }
        }
        if ((item.battleUseFlag & 1u) == 0) {
            const std::int32_t type = item.itemType;
            if (!((type >= 0 && type <= 7) || (type >= 70 && type <= 79))) {
                static_cast<void>(login::detail::DrawGraphic(
                    x + 105, y + 93, 223, 22738, 0, nullptr));
            }
        }
        if (item.hasDescription == 0 || (item.itemType == kMedicineType
                && (world::detail::gBattlePlayerFlags5ABCAC & 0x40000u) != 0)) {
            static_cast<void>(login::detail::DrawGraphic(
                x + 105, y + 93, 223, 22738, 0, nullptr));
        }
        if (occupancy[position + 8] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
        }
        if (item.count > 0) {
            std::sprintf(countText.data(), "%3d", item.count);
            static_cast<void>(render::DrawText_456780(
                x - 3, y + 7, 3, 1, 1, countText.data(), 0, 0));
        }
    }
    return consumed;
}

std::int32_t Sub_49B610(const std::int32_t, const std::int32_t state)
{
    return Sub_4C75A0(&gItemSelectorD2F338, state) ? 1 : 0;
}

std::int32_t Sub_49AC40(const std::int32_t index, const std::int32_t state)
{
    static constexpr char noSpace[] = "\xAA\xAB\xAB\x7E\xC4\xE6\xA6\xEC\xA4\xA3\xA8\xAC\xA1\x49";
    static constexpr char unidentified[] =
        "\xB5\x4C\xAA\x6B\xB8\xCB\xB3\xC6\xA5\xBC\xC5\xB2\xA9\x77\xAA\xAB\xAB\x7E\xA1\x49";
    static constexpr char noWeapon[] = "\xA8\x53\xA6\xB3\xB8\xCB\xB3\xC6\xAA\x5A\xBE\xB9\xA1\x49";
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::memcpy(dimmed.bytes.data() + 8, &dimmedColor, sizeof(dimmedColor));
    descriptor.bytes[13] = 4;
    dimmed.bytes[13] = 4;
    world::detail::PlayerItemRecord* items;
    std::int32_t count;
    if (!Sub_4C72C0(&gItemSelectorD2F338, &items, &count)) {
        return 0;
    }
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gValueD67B08.data());
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if (std::bit_cast<std::int8_t>(gValue57E42C) > 0) {
        const std::int32_t pet = Sub_4C71B0(&gItemSelectorD2F338);
        const std::int32_t slots = world::detail::gPetRuntimeF633F8[pet].values6D8[0];
        for (std::int32_t position = 0; position < slots; ++position) {
            const auto& item = items[position];
            const std::int32_t left = Sub_4C7240(&gItemSelectorD2F338, index, position, -24);
            const std::int32_t top = Sub_4C7280(&gItemSelectorD2F338, index, position, -24);
            if (item.used == 0) {
                continue;
            }
            if ((events & 0x21u) != 0) {
                if (occupancy[position] != 0) {
                    continue;
                }
                if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    std::strcpy(gHoverTextD68AE8.data(),
                        "\xB5\x4C\xAA\x6B\xA6\x62\xBE\xD4\xB0\xAB\xA4\xA4"
                        "\xA7\xF3\xB4\xAB\xB8\xCB\xB3\xC6\xA1\x43");
                    if ((events & 1u) != 0) {
                        login::detail::DrawGraphicHover(
                            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                            223, login::detail::gValueD69768, 0);
                    }
                }
                consumed = 1;
            }
            static_cast<void>(login::detail::DrawGraphic(left + 129, top + 117, 223, 22738, 0, nullptr));
            if (occupancy[position] != 0) {
                login::detail::DrawGraphicHover(
                    static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
                    223, 249, 0);
                static_cast<void>(login::detail::DrawGraphic(
                    left + 24, top + 24, 223, item.imageNo, 0, &dimmed));
            } else {
                static_cast<void>(login::detail::DrawGraphic(
                    left + 24, top + 24, 223, item.imageNo, 0, &descriptor));
            }
        }
        return consumed;
    }
    if ((events & 0x60u) != 0 && (Sub_475D60() != 2 || Sub_475D90() >= 100)) {
        events &= 0x9Fu;
        consumed = 1;
    }
    std::array<char, 256> buffer;
    if ((events & 0x40u) != 0) {
        const std::int32_t source = Sub_475D90();
        if (source >= count && world::detail::gPlayerItemsFDA8A4[source].used == 1) {
            const std::int32_t type = world::detail::gPlayerItemsFDA8A4[source].itemType;
            if ((type >= 0 && type <= 7) || (type >= 70 && type <= 79)) {
                std::int32_t occupiedHands = world::detail::gPlayerItemsFDA8A4[2].used == 1;
                if (world::detail::gPlayerItemsFDA8A4[3].used == 1) {
                    ++occupiedHands;
                }
                if (occupiedHands == 2 && world::detail::Sub_518C60() == 0
                    && (world::detail::gPlayerItemsFDA8A4[source].battleUseFlag & 8u) != 0) {
                    world::detail::Sub_44B050(noSpace, 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else if (world::detail::gPlayerItemsFDA8A4[source].hasDescription == 0) {
                    world::detail::Sub_44B050(unidentified, 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    gCurrentWindow->state |= 0x8000u;
                    world::detail::gBattleSelectionModes5ABC7C[5] = 0;
                    std::sprintf(buffer.data(), "Q|%X|", static_cast<std::uint32_t>(source));
                    static_cast<void>(network::SendBattlePacket_5109B0(
                        platform::gConnectionSocket, buffer.data()));
                    world::detail::gBattlePlayerActionState5ABC74 = 1;
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                }
            }
        }
        Sub_476500();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < count; ++position) {
            if (occupancy[position] != 0) {
                continue;
            }
            const std::int32_t left = Sub_4C7240(&gItemSelectorD2F338, index, position, -24);
            const std::int32_t top = Sub_4C7280(&gItemSelectorD2F338, index, position, -24);
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
        consumed = 1;
    }
    auto* const item = items + hovered;
    if ((events & 2u) != 0) {
        gCurrentWindow->state |= 0x2000u;
    }
    if ((events & 2u) != 0 && hovered == -1) {
        if (world::detail::gReplyRecordF7C950.used != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            WindowInstance* const window = gCurrentWindow;
            gItemSelectorD2F338.value3FC = 1;
            window->elements[20].mouseCallback = Sub_49B610;
            return consumed;
        }
    } else if (hovered >= 0) {
        const char* hint;
        if (item->used != 0 && item->itemType < 23) {
            const std::int32_t type = item->itemType;
            hint = ((type >= 0 && type <= 7) || (type >= 70 && type <= 79))
                && item->hasDescription != 0
                ? "\xA5\x69\xB8\xCB\xB3\xC6\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43"
                : "\xB5\x4C\xAA\x6B\xB8\xCB\xB3\xC6\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
        } else {
            hint = (item->battleUseFlag & 1u) != 0 && item->hasDescription != 0
                ? "\xA5\x69\xA8\xCF\xA5\xCE\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43"
                : "\xB5\x4C\xAA\x6B\xA8\xCF\xA5\xCE\xA6\xB9\xAA\xAB\xAB\x7E\xA1\x43";
        }
        std::strcpy(gHoverTextD68AE8.data(), hint);
    }
    if ((platform::gLegacyMouseState.edgeButtons & 0x10u) != 0 && hovered >= 0 && item->used != 0) {
        if (Sub_475D40() == 14 || Sub_475D40() == 0) {
            if (hovered == 2 || hovered == 3) {
                std::int32_t weapons = 0;
                if (items[2].used == 1 && items[2].itemType != 7) {
                    weapons = 1;
                }
                if (items[3].used == 1 && items[3].itemType != 7) {
                    ++weapons;
                }
                if (world::detail::Sub_518C60() < weapons) {
                    world::detail::Sub_44B050(noSpace, 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else if (weapons == 0) {
                    world::detail::Sub_44B050(noWeapon, 4, 3);
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    gCurrentWindow->state |= 0x8000u;
                    world::detail::gBattleSelectionModes5ABC7C[5] = 0;
                    std::sprintf(buffer.data(), "Q|%X|", 255u);
                    static_cast<void>(network::SendBattlePacket_5109B0(
                        platform::gConnectionSocket, buffer.data()));
                    world::detail::gBattlePlayerActionState5ABC74 = 1;
                    static_cast<void>(audio::PlaySe_52C450(57, 320));
                }
            }
            static_cast<void>(Sub_4763E0());
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            if ((hovered == 2 || hovered == 3) && item->used != 0 && item->itemType != 7) {
                static_cast<void>(Sub_48BF10(hovered, 0));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            if (item->used != 0) {
                ++gBattleItemPageCB01E0;
                if (gBattleItemPageCB01E0 >= item->pages) {
                    gBattleItemPageCB01E0 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (Sub_475D60() == 2) {
        const std::int32_t source = Sub_475D90();
        if (source < count) {
            const std::int32_t left = Sub_4C7240(&gItemSelectorD2F338, index, source, -24);
            const std::int32_t top = Sub_4C7280(&gItemSelectorD2F338, index, source, -24);
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        const std::int32_t left = Sub_4C7240(&gItemSelectorD2F338, index, hovered, -24);
        const std::int32_t top = Sub_4C7280(&gItemSelectorD2F338, index, hovered, -24);
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
    }
    if (Sub_475D60() != 0 && (events & 0x20u) != 0) {
        const std::int32_t source = Sub_475D90();
        if (source >= count) {
            const std::int32_t type = items[source].itemType;
            if ((type >= 0 && type <= 7) || (type >= 70 && type <= 79)) {
                const WindowInstance* const window = gCurrentWindow;
                const WindowElement& element = window->elements[index];
                const std::int32_t left = element.x + window->x;
                const std::int32_t top = element.y + window->y;
                login::detail::DrawGraphicHover(
                    static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
                    static_cast<std::uint16_t>(left + element.width),
                    static_cast<std::uint16_t>(top + element.height),
                    223, login::detail::gValueD69768, 0);
            }
        }
    }
    for (std::int32_t position = 0; position < count; ++position) {
        const auto& record = items[position];
        const std::int32_t x = Sub_4C7240(&gItemSelectorD2F338, index, position, 0);
        const std::int32_t y = Sub_4C7280(&gItemSelectorD2F338, index, position, 0);
        if (record.used == 0) {
            continue;
        }
        if ((position != 2 && position != 3) || record.itemType == 7) {
            static_cast<void>(login::detail::DrawGraphic(x + 105, y + 93, 223, 22738, 0, nullptr));
        }
        if (record.hasDescription == 0) {
            static_cast<void>(login::detail::DrawGraphic(x + 105, y + 93, 223, 22738, 0, nullptr));
        }
        if ((record.flags & 0x80u) == 0) {
            static_cast<void>(login::detail::DrawGraphic(x + 105, y + 93, 223, 22739, 0, &descriptor));
        }
        if (occupancy[position] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, record.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, record.imageNo, 0, &descriptor));
        }
    }
    return consumed;
}

std::int32_t Sub_488FB0(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::memcpy(dimmed.bytes.data() + 8, &dimmedColor, sizeof(dimmedColor));
    descriptor.bytes[13] = 4;
    dimmed.bytes[13] = 4;
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if (state != 0) {
        gItemSelectorCF3B10.value3FC = 0;
    }
    if ((events & 0x60u) != 0 && Sub_475D60() != 2) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t source = Sub_475D90();
        const WindowInstance* const window = gCurrentWindow;
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 15;
            const std::int32_t top = element.y + 50 * (position / 5) + window->y + 48;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (std::bit_cast<std::int8_t>(gValue57DA28) > 0 && source < 6) {
                Sub_488800(1, source, 0, position + 8);
            } else if (source != position + 8) {
                Sub_4886E0(14, index, source, position + 8);
            }
            break;
        }
        Sub_476500();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 15;
            const std::int32_t top = element.y + 50 * (position / 5) + window->y + 48;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
    }
    if (Sub_475D60() != 0 && (events & 0x20u) != 0 && Sub_475D90() >= 100
        && (world::detail::gPlayerItemsFDA8A4[hovered + 8].flags & 0x60u) != 0) {
        hovered = -1;
    }
    if ((events & 2u) != 0) {
        gCurrentWindow->state |= 0x2000u;
    }
    if (gInventoryHover57DA2C != hovered) {
        gWindowStates[14].value14 = 0;
    }
    gInventoryHover57DA2C = hovered;
    if (gSelectedItemCB1DF4 < 0 && hovered >= 0
        && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (Sub_475D40() == 14 || Sub_475D40() == 0) {
            const auto& item = world::detail::gPlayerItemsFDA8A4[hovered + 8];
            if (item.used != 0 && (item.canUseInField & 1u) != 0) {
                if (item.imageNo == 26569) {
                    WindowInstance* const window = gCurrentWindow;
                    gItemSelectionModeCC40D4 = 1;
                    window->elements[4].mouseCallback = Sub_489F00;
                    gCurrentWindow->elements[17].mouseCallback = Sub_48A6C0;
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                    gSelectedItemCB0A44 = hovered + 8;
                    ++occupancy[gSelectedItemCB0A44];
                    static_cast<void>(Sub_4763E0());
                    static_cast<void>(Sub_475DA0(1, 0, nullptr));
                } else {
                    if ((world::detail::ReadPlayerActionFlags() & 0x80u) == 0
                        && world::detail::Sub_440B50() == 0) {
                        const std::int8_t mode = std::bit_cast<std::int8_t>(gValue57DA28);
                        gSelectedItemCB0A44 = hovered + 8;
                        const std::int32_t group = mode > 0
                            ? Sub_4C71B0(&gItemSelectorCF3B10) + 1 : 0;
                        const std::int32_t y =
                            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
                        const std::int32_t x =
                            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
                        static_cast<void>(network::Sub_510A00(
                            platform::gConnectionSocket, x, y, hovered + 8, group));
                        static_cast<void>(audio::PlaySe_52C450(77, 320));
                    }
                    static_cast<void>(Sub_4763E0());
                }
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                static_cast<void>(Sub_4763E0());
            }
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
                static_cast<void>(Sub_48BF10(hovered + 8, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[14].value14);
                const std::int32_t pages = world::detail::gPlayerItemsFDA8A4[hovered + 8].pages;
                ++page;
                gWindowStates[14].value14 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[14].value14 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (Sub_475D60() == 2) {
        const std::int32_t source = Sub_475D90();
        if (source >= 8) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * ((source - 8) % 5) + window->x + 15;
            const std::int32_t top = element.y + 50 * ((source - 8) / 5) + window->y + 48;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    std::array<char, 256> buffer;
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 15;
        const std::int32_t top = element.y + 50 * (hovered / 5) + window->y + 48;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        const auto& item = world::detail::gPlayerItemsFDA8A4[hovered + 8];
        if (item.used != 0) {
            gWindowStates[14].value10 = static_cast<std::uint32_t>(hovered + 8);
            if (std::strlen(item.memo.data()) != 0 && (item.flags & 0x100u) != 0) {
                std::sprintf(buffer.data(), "%s[kok]", item.name.data());
                std::strcpy(gHoverTextD68AE8.data(), buffer.data());
            } else if ((item.flags & 0x200u) != 0) {
                std::sprintf(buffer.data(), "%s[han]", item.memo.data());
                std::strcpy(gHoverTextD68AE8.data(), buffer.data());
            }
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = world::detail::gPlayerItemsFDA8A4[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (position % 5) + window->x + 39;
        const std::int32_t y = element.y + 50 * (position / 5) + window->y + 73;
        if (occupancy[position + 8] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
        }
        const std::int32_t count = item.count;
        if (count > 0) {
            std::sprintf(buffer.data(), "%3d", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                3, 1, 1, buffer.data(), 0, 0));
        }
    }
    return consumed;
}

std::int32_t Sub_489760(const std::int32_t index, const std::int32_t state)
{
    static constexpr const char* playerHints[] {
        "\xB8\xCB\xB3\xC6\xC0\x59\xB3\xA1\xA8\xBE\xA8\xE3\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xA8\xAD\xC5\xE9\xA8\xBE\xA8\xE3\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xAA\x5A\xBE\xB9\xB8\xF2\xAC\xDE\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xAA\x5A\xBE\xB9\xB8\xF2\xAC\xDE\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xB8\x7D\xB3\xA1\xA8\xBE\xA8\xE3\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xB8\xCB\xB9\xA2\xAB\x7E\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xB8\xCB\xB9\xA2\xAB\x7E\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xA4\xF4\xB4\xB9\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
    };
    static constexpr const char* petHints[] {
        "\xB8\xCB\xB3\xC6\xB4\xB9\xA5\xDB\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xA8\xBE\xA8\xE3\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xB6\xB5\xB0\xE9\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xB9\xA2\xAB\x7E\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xB9\xA2\xAB\x7E\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
        "\xB8\xCB\xB3\xC6\xB9\xA2\xAB\x7E\xAA\xBA\xA6\xEC\xB8\x6D\xA1\x43",
    };
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::memcpy(dimmed.bytes.data() + 8, &dimmedColor, sizeof(dimmedColor));
    descriptor.bytes[13] = 4;
    dimmed.bytes[13] = 4;
    world::detail::PlayerItemRecord* items;
    std::int32_t count;
    if (!Sub_4C72C0(&gItemSelectorCF3B10, &items, &count)) {
        return 0;
    }
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && (Sub_475D60() != 2 || Sub_475D90() >= 100)) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t source = Sub_475D90();
        for (std::int32_t position = 0; position < count; ++position) {
            if (occupancy[position] != 0) {
                continue;
            }
            const std::int32_t left = Sub_4C7240(&gItemSelectorCF3B10, index, position, -24);
            const std::int32_t top = Sub_4C7280(&gItemSelectorCF3B10, index, position, -24);
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (std::bit_cast<std::int8_t>(gValue57DA28) <= 0) {
                if (source != position) {
                    Sub_4886E0(14, index, source, position);
                }
            } else if (source < 6) {
                if (position >= 6) {
                    Sub_488800(1, source, 0, position);
                }
            } else if (position < 6) {
                Sub_488800(0, source, 1, position);
            } else {
                Sub_4886E0(14, index, source, position);
            }
            break;
        }
        Sub_476500();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < count; ++position) {
            if (occupancy[position] != 0) {
                continue;
            }
            const std::int32_t left = Sub_4C7240(&gItemSelectorCF3B10, index, position, -24);
            const std::int32_t top = Sub_4C7280(&gItemSelectorCF3B10, index, position, -24);
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                gItemSelectorCF3B10.hoverIndex3F8 = position;
                break;
            }
        }
    }
    // 后续清hover不改变原来选中的物品引用及用于绘框的命中索引。
    auto* const item = items + hovered;
    const std::int32_t hitIndex = hovered;
    if ((events & 1u) != 0 && hovered >= 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            gValue57DA28 != 0 ? petHints[hovered] : playerHints[hovered]);
    }
    if (Sub_475D60() != 0 && (events & 0x20u) != 0 && Sub_475D90() >= 100) {
        hovered = -1;
    }
    if ((events & 2u) != 0) {
        gCurrentWindow->state |= 0x2000u;
        if (hovered == -1 && world::detail::gReplyRecordF7C950.used != 0) {
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            WindowInstance* const window = gCurrentWindow;
            gItemSelectorCF3B10.value3FC = 1;
            window->elements[17].mouseCallback = Sub_489EE0;
            return consumed;
        }
    }
    if (gEquipmentHover57DA68 != hovered) {
        gWindowStates[14].value14 = 0;
    }
    gEquipmentHover57DA68 = hovered;
    if (gSelectedItemCB1DF4 < 0 && hovered >= 0
        && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (Sub_475D40() == 14 || Sub_475D40() == 0) {
            if (item->used != 0 && (item->canUseInField & 1u) != 0) {
                if (item->imageNo == 26569) {
                    WindowInstance* const window = gCurrentWindow;
                    gItemSelectionModeCC40D4 = 1;
                    window->elements[4].mouseCallback = Sub_489F00;
                    gCurrentWindow->elements[17].mouseCallback = Sub_48A6C0;
                    static_cast<void>(audio::PlaySe_52C450(77, 320));
                    const std::uint8_t previousOccupancy = occupancy[hovered];
                    gSelectedItemCB0A44 = hovered;
                    occupancy[hovered] = static_cast<std::uint8_t>(previousOccupancy + 1u);
                    static_cast<void>(Sub_4763E0());
                    static_cast<void>(Sub_475DA0(1, 0, nullptr));
                } else {
                    if ((world::detail::ReadPlayerActionFlags() & 0x80u) == 0
                        && world::detail::Sub_440B50() == 0) {
                        const std::int8_t mode = std::bit_cast<std::int8_t>(gValue57DA28);
                        gSelectedItemCB0A44 = hovered;
                        if (mode > 0) {
                            const std::int32_t group = Sub_4C71B0(&gItemSelectorCF3B10) + 1;
                            static_cast<void>(network::Sub_516C90(
                                platform::gConnectionSocket, group, hovered, 0, -1, -1));
                        } else {
                            const std::int32_t y =
                                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
                            const std::int32_t x =
                                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
                            static_cast<void>(network::Sub_510A00(
                                platform::gConnectionSocket, x, y, hovered, 0));
                        }
                        static_cast<void>(audio::PlaySe_52C450(77, 320));
                    }
                    static_cast<void>(Sub_4763E0());
                }
            } else {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
                static_cast<void>(Sub_4763E0());
            }
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            if (item->used != 0) {
                static_cast<void>(Sub_48BF10(hovered, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            if (item->used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[14].value14);
                ++page;
                gWindowStates[14].value14 = static_cast<std::uint32_t>(page);
                if (page >= item->pages) {
                    gWindowStates[14].value14 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (Sub_475D60() == 2) {
        const std::int32_t source = Sub_475D90();
        if (source < count) {
            const std::int32_t left = Sub_4C7240(&gItemSelectorCF3B10, index, source, -24);
            const std::int32_t top = Sub_4C7280(&gItemSelectorCF3B10, index, source, -24);
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    std::array<char, 256> buffer;
    if (hovered >= 0) {
        const std::int32_t left = Sub_4C7240(&gItemSelectorCF3B10, index, hitIndex, -24);
        const std::int32_t top = Sub_4C7280(&gItemSelectorCF3B10, index, hitIndex, -24);
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        if (item->used != 0) {
            gWindowStates[14].value10 = static_cast<std::uint32_t>(hovered);
            if (std::strlen(item->memo.data()) != 0 && (item->flags & 0x100u) != 0) {
                std::sprintf(buffer.data(), "%s[kok]", item->name.data());
                std::strcpy(gHoverTextD68AE8.data(), buffer.data());
            } else if ((item->flags & 0x200u) != 0) {
                std::sprintf(buffer.data(), "%s[han]", item->memo.data());
                std::strcpy(gHoverTextD68AE8.data(), buffer.data());
            }
        }
    }
    for (std::int32_t position = 0; position < count; ++position) {
        const auto& record = items[position];
        if (record.used == 0) {
            continue;
        }
        const std::int32_t x = Sub_4C7240(&gItemSelectorCF3B10, index, position, 0);
        const std::int32_t y = Sub_4C7280(&gItemSelectorCF3B10, index, position, 0);
        if ((record.flags & 0x80u) == 0) {
            static_cast<void>(login::detail::DrawGraphic(
                x + 105, y + 93, 223, 22739, 0, &descriptor));
        }
        if (occupancy[position] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, record.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, record.imageNo, 0, &descriptor));
        }
    }
    return consumed;
}

bool Sub_4C75A0(WindowItemSelectorState* const selector, const std::int32_t state)
{
    if ((static_cast<std::uint8_t>(state) & 2u) != 0 || state == 0
        || selector->value3FC == 0 || world::detail::gReplyRecordF7C950.used == 0) {
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
    // 原580900二十四项表；标签为原编码字节，数值索引不是地址。
    struct AttributeDisplay {
        std::int32_t index;
        const char* label;
        std::uint8_t percent;
        std::int32_t color;
    };
    static constexpr AttributeDisplay attributes[] {
        {10, "\xA5\xCD\xA9\x52", 1, 4},
        {11, "\xA5\xCD\xA9\x52", 0, 4},
        {12, "\xC5\x5D\xA4\x4F", 1, 4},
        {13, "\xC5\x5D\xA4\x4F", 0, 4},
        {0, "\xA7\xF0\xC0\xBB", 1, 1},
        {1, "\xA7\xF0\xC0\xBB", 0, 1},
        {2, "\xA8\xBE\xBF\x6D", 1, 1},
        {3, "\xA8\xBE\xBF\x6D", 0, 1},
        {4, "\xB1\xD3\xB1\xB6", 1, 1},
        {5, "\xB1\xD3\xB1\xB6", 0, 1},
        {6, "\xBA\xEB\xAF\xAB", 1, 1},
        {7, "\xBA\xEB\xAF\xAB", 0, 1},
        {8, "\xA6\x5E\xB4\x5F", 1, 1},
        {9, "\xA6\x5E\xB4\x5F", 0, 1},
        {14, "\xA7\xDC\xAC\x72", 0, 2},
        {15, "\xA7\xDC\xBA\xCE\xAF\x76", 0, 2},
        {16, "\xA7\xDC\xA5\xDB\xA4\xC6", 0, 2},
        {17, "\xA7\xDC\xB0\x73\xBE\x4B", 0, 2},
        {18, "\xA7\xDC\xB2\x56\xB6\xC3", 0, 2},
        {19, "\xA7\xDC\xBF\xF2\xA7\xD1", 0, 2},
        {20, "\xA5\xB2\xB1\xFE", 0, 8},
        {21, "\xA4\xCF\xC0\xBB", 0, 8},
        {22, "\xA9\x52\xA4\xA4", 0, 8},
        {23, "\xB0\x6A\xC1\xD7", 0, 8},
    };
    const WindowElement* const element = selector->elements00[1];
    const std::int32_t elementY = element->y;
    const std::int32_t elementX = element->x;
    const WindowInstance* const window = gCurrentWindow;
    static_cast<void>(login::detail::DrawGraphic(
        elementX + window->x + 121, elementY + window->y + 118,
        223, 98244, 0, &selector->descriptors3B8[0]));
    const auto drawText = [selector](
        const std::int32_t offsetX, const std::int32_t offsetY,
        const std::int32_t textColor, const char* const text) {
        const WindowElement* const textElement = selector->elements00[1];
        const WindowInstance* const textWindow = gCurrentWindow;
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            textElement->x + textWindow->x + offsetX));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            textElement->y + textWindow->y + offsetY));
        static_cast<void>(render::DrawText_456780(
            x, y, 5, 12,
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(textColor)), text, 0, 0));
    };
    const std::int32_t length =
        static_cast<std::int32_t>(std::strlen(world::detail::gReplyRecordF7C950.text04.data()));
    if (length > 0) {
        drawText(100 - (12 * length) / 2 / 2, 9, 0,
            world::detail::gReplyRecordF7C950.text04.data());
    }
    if (world::detail::gReplyRecordF7C950.value48 == 1) {
        drawText(64, 28, 0, "\x28\xA6\xB3\xAF\x53\xAE\xED\xBA\xD9\xB8\xB9\x29");
    }
    std::int32_t lines = 0;
    std::int32_t x = 28;
    std::int32_t y = 58;
    std::array<char, 512> buffer;
    for (const AttributeDisplay& attribute : attributes) {
        if (lines >= 14) {
            break;
        }
        const std::int32_t value = world::detail::gReplyRecordF7C950.values4C[attribute.index];
        if (value == 0) {
            continue;
        }
        if (lines == 7) {
            x = 116;
            y = 58;
        }
        const char* const format = attribute.percent != 0
            ? (value < 0 ? "%s-%4d%%" : "%s+%4d%%")
            : (value < 0 ? "%s-%4d" : "%s+%4d");
        std::sprintf(buffer.data(), format, attribute.label, value < 0 ? -value : value);
        drawText(x, y, attribute.color, buffer.data());
        y += 16;
        ++lines;
    }
    if (world::detail::gReplyRecordF7C950.hasText24 != 0) {
        std::sprintf(buffer.data(),
            "\xAF\xE0\xB4\xEE\xA4\xD6\xA8\xCF\xA5\xCE\x22%s\x22\xAA\xBA\xC5\x5D",
            world::detail::gReplyRecordF7C950.text25.data());
        drawText(28, 188, 0, buffer.data());
        drawText(28, 204, 0, "\xA4\x4F\xAE\xF8\xAF\xD3");
    }
    return false;
}

std::int32_t Sub_489EE0(const std::int32_t, const std::int32_t state)
{
    return Sub_4C75A0(&gItemSelectorCF3B10, state) ? 1 : 0;
}

std::int32_t Sub_489F00(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::memcpy(dimmed.bytes.data() + 8, &dimmedColor, sizeof(dimmedColor));
    descriptor.bytes[13] = 4;
    dimmed.bytes[13] = 4;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    std::int32_t hovered = -1;
    std::int32_t consumed = 0;
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    if ((events & 0x20u) != 0 && Sub_475D60() == 1 && Sub_475D40() == 14) {
        events |= 1u;
        if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
            events |= 2u;
        }
    }
    const auto isMode1Blocked = [](const world::detail::PlayerItemRecord& item) {
        if (item.itemType == 47) {
            return true;
        }
        const std::uint16_t flags = item.flags;
        return (flags & 0x100u) != 0 || item.count > 0
            || ((flags & 0x200u) != 0 && (flags & 0x400u) == 0);
    };
    if ((events & 1u) != 0) {
        const std::int8_t mode = std::bit_cast<std::int8_t>(gItemSelectionModeCC40D4);
        if (mode == 1 || mode == 2) {
            for (std::int32_t position = 0; position < 20; ++position) {
                const auto& item = world::detail::gPlayerItemsFDA8A4[position + 8];
                if (item.used == 0 || occupancy[position + 8] != 0) {
                    continue;
                }
                if (mode == 1 ? isMode1Blocked(item) : (item.flags & 0x60u) != 0) {
                    continue;
                }
                const WindowInstance* const window = gCurrentWindow;
                const WindowElement& element = window->elements[index];
                const std::int32_t left = element.x + 50 * (position % 5) + window->x + 15;
                const std::int32_t top = element.y + 50 * (position / 5) + window->y + 48;
                if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                    hovered = position;
                    break;
                }
            }
        }
    }
    if ((events & 2u) != 0) {
        gCurrentWindow->state |= 0x2000u;
    }

    // 发送完成与右键取消各自执行；第二次退出不得因前面已清理而省略。
    const auto leaveSelection = [&]() {
        WindowInstance* const window = gCurrentWindow;
        gItemSelectionModeCC40D4 = 0;
        window->elements[4].mouseCallback = Sub_488FB0;
        gCurrentWindow->elements[17].mouseCallback = Sub_489760;
        const std::int32_t selected = gSelectedItemCB0A44;
        const std::int32_t dialog = world::detail::gValue581800;
        --occupancy[selected];
        gSelectedItemCB0A44 = -1;
        if (dialog == 29) {
            world::detail::gValue581800 = -1;
        }
        static_cast<void>(Sub_4763E0());
    };
    std::array<char, 256> buffer;
    std::array<char, 16> escaped;
    if (hovered >= 0 && (events & 2u) != 0) {
        if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
            const std::uint8_t mode = gItemSelectionModeCC40D4;
            if (mode == 1) {
                const std::int32_t selected = gSelectedItemCB0A44;
                static_cast<void>(network::Sub_511D60(
                    platform::gConnectionSocket, hovered + 8, selected));
            } else if (mode == 2) {
                std::sprintf(buffer.data(), "%d", hovered + 8);
                input::EscapeLegacyTextAndNotify_52D2D0(buffer.data(), escaped.data(), 15);
                const std::int32_t target = world::detail::gDialogTargetD2DE6C;
                const std::int32_t request = world::detail::gDialogRequestCC21E0;
                const std::int32_t y =
                    platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
                const std::int32_t x =
                    platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
                static_cast<void>(network::Sub_512620(
                    platform::gConnectionSocket, x, y, request, target, 0, escaped.data()));
                leaveSelection();
            }
        }
        consumed = 1;
    }
    if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons) & 2u) != 0) {
        leaveSelection();
    }
    const std::int32_t imageNo = world::detail::gPlayerItemsFDA8A4[gSelectedItemCB0A44].imageNo;
    const std::int32_t mouseY =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
    const std::int32_t mouseX =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
    static_cast<void>(login::detail::DrawGraphic(mouseX, mouseY, 230, imageNo, 0, &descriptor));
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 15;
        const std::int32_t top = element.y + 50 * (hovered / 5) + window->y + 48;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
    const std::int8_t mode = std::bit_cast<std::int8_t>(gItemSelectionModeCC40D4);
    if (mode != 1 && mode != 2) {
        return consumed;
    }
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = world::detail::gPlayerItemsFDA8A4[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (position % 5) + window->x + 39;
        const std::int32_t y = element.y + 50 * (position / 5) + window->y + 73;
        if (mode == 1 ? isMode1Blocked(item) : (item.flags & 0x60u) != 0) {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, 23040, 0, nullptr));
        }
        if (occupancy[position + 8] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
        }
        const std::int32_t count = item.count;
        if (count > 0) {
            std::sprintf(buffer.data(), "%3d", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                3, 1, 1, buffer.data(), 0, 0));
        }
    }
    return consumed;
}

std::int32_t Sub_48B1A0(const std::int32_t index, const std::int32_t page)
{
    if (gTooltipSourceD303FC != index) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX));
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY));
        const bool left =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseX, 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseY, 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    const std::int8_t mode = std::bit_cast<std::int8_t>(gValue57DA28);
    gTooltipSourceD303FC = index;
    const bool petEquipment = mode > 0 && index < 6;
    const world::detail::PlayerItemRecord* record;
    if (petEquipment) {
        const std::int32_t petIndex = Sub_4C71B0(&gItemSelectorCF3B10);
        const std::int32_t itemIndex = gTooltipSourceD303FC;
        record = &world::detail::gPetRuntimeF633F8[petIndex].items6E4[itemIndex];
    } else {
        record = &world::detail::gPlayerItemsFDA8A4[index];
    }
    const std::uint16_t flags = record->flags;
    const char* const title = (flags & 0x100u) != 0
        ? record->memo.data() : record->name.data();
    const std::int8_t titleColor = (flags & 0x100u) != 0
        ? 5 : static_cast<std::int8_t>((flags & 0x200u) >> 9);
    const char* const lines = reinterpret_cast<const char*>(record)
        + offsetof(world::detail::PlayerItemRecord, description) + page * 768;
    if (petEquipment) {
        const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
        const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
        return Sub_48B810(x, y, title, titleColor, lines, 96, 8, nullptr, 0);
    }
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    return Sub_48B810(x, y, title, titleColor, lines, 96, 8, nullptr, 0);
}

std::int32_t Sub_48B350(const std::int32_t index, const std::int32_t page)
{
    const std::int32_t key = index + 100;
    if (gTooltipSourceD303FC != key) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX));
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY));
        const bool left =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseX, 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseY, 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    gTooltipSourceD303FC = key;
    const char* const record = gItemDetailStorageCC4D58.data() + index * 0xC44;
    std::uint32_t flags;
    std::memcpy(&flags, record + 0xC40, sizeof(flags));
    const char* const title = (flags & 0x100u) != 0 ? record + 0xC29 : record + 0xC;
    const std::int8_t titleColor = (flags & 0x100u) != 0
        ? 5 : static_cast<std::int8_t>((flags >> 9) & 1u);
    const char* const lines = record + 0x29 + page * 768;
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    return Sub_48B810(x, y, title, titleColor, lines, 96, 8, nullptr, 0);
}

std::int32_t Sub_48B560(
    const std::int32_t index, const std::int32_t page, const std::int32_t showUnavailable)
{
    std::int32_t key = index + 300;
    if (gTooltipSourceD303FC != key) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX));
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY));
        const bool left =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseX, 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseY, 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    gTooltipSourceD303FC = key;
    const char* record = gShopItemsCF4FE0.data() + index * 0xC58;
    std::uint32_t flags;
    std::memcpy(&flags, record + 0xC40, sizeof(flags));
    std::array<char, 256> buffer;
    std::strcpy(buffer.data(), record + 0xC44);
    std::int8_t titleColor;
    if ((flags & 0x100u) != 0) {
        titleColor = 5;
    } else {
        std::int32_t count;
        std::memcpy(&count, record + 0xC30, sizeof(count));
        if (count <= 1) {
            std::strcpy(buffer.data(), record + 1);
        } else {
            static_cast<void>(_snprintf(buffer.data(), 256, "%s(%d)", record + 1, count));
            key = gTooltipSourceD303FC;
        }
        record = gShopItemsCF4FE0.data() + (key - 300) * 0xC58;
        std::memcpy(&flags, record + 0xC40, sizeof(flags));
        titleColor = static_cast<std::int8_t>((flags >> 9) & 1u);
    }
    const char* const footer = record[0xC3C] == 0 && showUnavailable != 0
        ? kShopUnavailableHint586BF4 : nullptr;
    const char* const lines = record + 0x1E + page * 768;
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    return Sub_48B810(x, y, buffer.data(), titleColor, lines, 96, 8, footer, 4);
}

std::int32_t Sub_48B750(const std::int32_t page)
{
    if (gTooltipSourceD303FC != 400) {
        gTooltipXCF46C0 = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX));
        gTooltipYCF46CC = static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY));
        const bool left =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseX, 320);
        gTooltipXCF46C0 = static_cast<std::uint16_t>(gTooltipXCF46C0 + (left ? 60 : -257));
        const bool top =
            platform::legacy_crypt_int::Less_401DB0(&platform::gLegacyMouseState.mouseY, 240);
        gTooltipYCF46CC = static_cast<std::uint16_t>(gTooltipYCF46CC + (top ? 60 : -174));
    }
    const std::int16_t y = std::bit_cast<std::int16_t>(gTooltipYCF46CC);
    const std::int16_t x = std::bit_cast<std::int16_t>(gTooltipXCF46C0);
    gTooltipSourceD303FC = 400;
    return Sub_48B810(
        x, y, gShopDetailCC3210.data(), 0, gShopDetailCC3210.data() + 0x1D + page * 768,
        96, 8, nullptr, 0);
}

std::int32_t Sub_4B4070(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t skill)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const char* const text = gPetStorageD64E18.data() + index * 0x600 + 0x62 + skill * 0x8C;
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, input::LegacyText593850(), 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 7)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
            5, 12, 0, text, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return login::detail::DrawGraphic(
        shortX + 105, shortY + 8, 0xE6, 0x3B59C, 0, &descriptor);
}

std::int32_t Sub_4B4140(
    const std::int32_t x, const std::int32_t y, const std::int32_t index)
{
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortIndex =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(index));
    const char* const record = gPetStorageD64E18.data() + shortIndex * 0x600;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return 0;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, input::LegacyText593850(), 0, 0));
    const char* const title = record + 0x5E5;
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 60)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 11)),
            5, 12, 0, title, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    std::array<char, 100> buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::int32_t value;
        std::memcpy(&value, record + 0x30 + row * 4, sizeof(value));
        std::sprintf(buffer.data(), "%10d", value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 144)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(shortY + 23 + row * 15)),
            5, 12, 0, buffer.data(), 0, 0));
    }
    std::int32_t maximumWeight;
    std::memcpy(&maximumWeight, record + 0x5FC, sizeof(maximumWeight));
    if (maximumWeight > 0) {
        std::int32_t weight;
        std::memcpy(&weight, record + 0x5F8, sizeof(weight));
        std::sprintf(buffer.data(), "%3d / %3d", weight, maximumWeight);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 61)),
            31, 5, 12, 0, buffer.data(), 0, 0));
    }
    if (gPreviewActionD67B84 == nullptr) {
        gPreviewActionD67B84 = login::detail::NewYobiAction_404AE0(2, 0);
    }
    std::int32_t imageNo;
    std::memcpy(&imageNo, record + 4, sizeof(imageNo));
    login::detail::ConfigureWindowPetPreview(gPreviewActionD67B84, imageNo);
    static_cast<void>(login::detail::Sub_517880(gPreviewActionD67B84, 100, 0, 0));
    login::YobiAction* const action = gPreviewActionD67B84;
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const render::GraphicDescriptor16* const mode =
        login::detail::BorrowYobiActionDrawMode(action);
    const std::int32_t graphicIndex = login::detail::GetYobiActionField20(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 0xE6, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        std::int32_t value;
        std::memcpy(&value, record + 0x48 + row * 4, sizeof(value));
        const std::int32_t segments = value / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 0xE6, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 0xE6, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[3], 0, 1, 0, &descriptor));
        }
    }
    for (std::int32_t skill = 0; skill < 10; ++skill) {
        static_cast<void>(Sub_4B4070(
            shortX + 10, shortY + 141 + skill * 16, shortIndex, skill));
    }
    return render::DrawGraphic3_520970(
        shortX, shortY, 0xE6, 0x3BA95, 0, 1, 0, &descriptor);
}

std::int32_t Sub_4B45A0(
    const std::int32_t x, const std::int32_t y, const std::int32_t side,
    const std::int32_t index, const std::int32_t skill)
{
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortSide =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(side));
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const char* const text =
        gTradePetsC9C108.data() + (index + shortSide * 5) * 0x5FC + 0x66 + skill * 0x8C;
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, input::LegacyText593850(), 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 7)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
            5, 12, 0, text, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return login::detail::DrawGraphic(
        shortX + 105, shortY + 8, 0xE6, 0x3B59C, 0, &descriptor);
}

std::int32_t Sub_4B4680(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t side, const std::int32_t index)
{
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortIndex =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(index));
    const std::int32_t shortSide =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(side));
    const std::int32_t slot = shortIndex + shortSide * 5;
    const char* const record = gTradePetsC9C108.data() + slot * 0x5FC;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used == 0) {
        return slot;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, input::LegacyText593850(), 0, 0));
    const char* const title = record + 0x5E9;
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 60)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 11)),
            5, 12, 0, title, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    std::array<char, 100> buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        std::int32_t value;
        std::memcpy(&value, record + 0x38 + row * 4, sizeof(value));
        std::sprintf(buffer.data(), "%10d", value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 144)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(shortY + 23 + row * 15)),
            5, 12, 0, buffer.data(), 0, 0));
    }
    if (gPreviewActionD67B84 == nullptr) {
        gPreviewActionD67B84 = login::detail::NewYobiAction_404AE0(2, 0);
    }
    std::int32_t imageNo;
    std::memcpy(&imageNo, record + 8, sizeof(imageNo));
    login::detail::ConfigureWindowPetPreview(gPreviewActionD67B84, imageNo);
    static_cast<void>(login::detail::Sub_517880(gPreviewActionD67B84, 100, 0, 0));
    login::YobiAction* const action = gPreviewActionD67B84;
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const render::GraphicDescriptor16* const mode =
        login::detail::BorrowYobiActionDrawMode(action);
    const std::int32_t graphicIndex = login::detail::GetYobiActionField20(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 0xE6, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        std::int32_t value;
        std::memcpy(&value, record + 0x4C + row * 4, sizeof(value));
        const std::int32_t segments = value / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 0xE6, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 0xE6, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[3], 0, 1, 0, &descriptor));
        }
    }
    for (std::int32_t skill = 0; skill < 10; ++skill) {
        static_cast<void>(Sub_4B45A0(
            shortX + 10, shortY + 141 + skill * 16, side, shortIndex, skill));
    }
    return render::DrawGraphic3_520970(
        shortX, shortY, 0xE6, 0x3BA95, 0, 1, 0, &descriptor);
}

std::int32_t Sub_4B3B00(
    const std::int32_t x, const std::int32_t y,
    const std::int32_t petIndex, const std::int32_t skillPosition)
{
    render::GraphicDescriptor16 descriptor {};
    const auto& pet = world::detail::gPetRuntimeF633F8[petIndex];
    const std::int16_t skillIndex =
        std::bit_cast<std::int16_t>(pet.techniquePairs650[skillPosition].index);
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    const char* const text = pet.techniquesD8[skillIndex].name.data();
    if (*text != '\0') {
        static_cast<void>(render::ClearTextQueueTail_456D50(5));
        static_cast<void>(render::DrawText_456780(
            0, 0, 5, 1, 0, input::LegacyText593850(), 0, 0));
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 7)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 2)),
            5, 12, 0, text, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    return login::detail::DrawGraphic(
        shortX + 105, shortY + 8, 0xE6, 0x3B59C, 0, &descriptor);
}

std::int32_t Sub_4B3BF0(
    const std::int32_t x, const std::int32_t y, const std::int32_t index)
{
    render::GraphicDescriptor16 descriptor {};
    const std::int32_t shortIndex =
        std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(index));
    const auto& pet = world::detail::gPetRuntimeF633F8[shortIndex];
    if (pet.used == 0) {
        return 0;
    }
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    static_cast<void>(render::ClearTextQueueTail_456D50(5));
    static_cast<void>(render::DrawText_456780(
        0, 0, 5, 1, 0, input::LegacyText593850(), 0, 0));
    const char* const title = pet.name6AC.data();
    if (title != nullptr) {
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 60)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 11)),
            5, 12, 0, title, 0, 0));
    }
    const std::int32_t shortY =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y));
    std::array<char, 100> buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t value = pet.values70[5 + row];
        std::sprintf(buffer.data(), "%10d", value);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 144)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(shortY + 23 + row * 15)),
            5, 12, 0, buffer.data(), 0, 0));
    }
    const std::int32_t maximumWeight = pet.values6D8[2];
    if (maximumWeight > 0) {
        const std::int32_t weight = pet.values6D8[1];
        std::sprintf(buffer.data(), "%3d / %3d", weight, maximumWeight);
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x + 61)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 31)),
            5, 12, 0, buffer.data(), 0, 0));
    }
    if (gPreviewActionD67B84 == nullptr) {
        gPreviewActionD67B84 = login::detail::NewYobiAction_404AE0(2, 0);
    }
    const std::int32_t imageNo = pet.imageNo6A0;
    login::detail::ConfigureWindowPetPreview(gPreviewActionD67B84, imageNo);
    static_cast<void>(login::detail::Sub_517880(gPreviewActionD67B84, 100, 0, 0));
    login::YobiAction* const action = gPreviewActionD67B84;
    const std::int32_t shortX =
        std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x));
    const render::GraphicDescriptor16* const mode =
        login::detail::BorrowYobiActionDrawMode(action);
    const std::int32_t graphicIndex = login::detail::GetYobiActionField20(action);
    static_cast<void>(render::DrawGraphic2_521DD0(
        shortX + 70, shortY + 120, 0xE6, graphicIndex, 0, 0, mode));
    for (std::int32_t row = 0; row < 4; ++row) {
        const std::int32_t segments = pet.values70[21 + row] / 10;
        const auto& images = gPetBarImages57E434[row];
        const std::int32_t barY = shortY + 97 + row * 11;
        if (segments > 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[0], 0, 1, 0, &descriptor));
            std::int32_t barX = shortX + 155;
            for (std::int32_t middle = 0; middle < segments - 2; ++middle) {
                static_cast<void>(render::DrawGraphic3_520970(
                    barX, barY, 0xE6, images[1], 0, 1, 0, &descriptor));
                barX += 6;
            }
            static_cast<void>(render::DrawGraphic3_520970(
                barX, barY, 0xE6, images[2], 0, 1, 0, &descriptor));
        } else if (segments == 1) {
            static_cast<void>(render::DrawGraphic3_520970(
                shortX + 149, barY, 0xE6, images[3], 0, 1, 0, &descriptor));
        }
    }
    for (std::int32_t skill = 0; skill < 10; ++skill) {
        static_cast<void>(Sub_4B3B00(
            shortX + 10, shortY + 141 + skill * 16, shortIndex, skill));
    }
    return render::DrawGraphic3_520970(
        shortX, shortY, 0xE6, 0x3BA95, 0, 1, 0, &descriptor);
}

std::int32_t Sub_4AFFE0(const std::int32_t index)
{
    const std::int32_t slot = std::bit_cast<std::int8_t>(gTradeItemSlotsCF3AE8[index]);
    return slot < 0 ? -1 : slot;
}

std::int32_t Sub_4B00F0(const std::int32_t index)
{
    const std::int32_t slot = std::bit_cast<std::int8_t>(gTradePetSlotsCC4D28[index]);
    return slot < 0 ? -1 : slot;
}

std::int32_t Sub_4B0A50(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    if (hover) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xA6\x56\xB9\xEF\xA4\xE8\xA4\xBD\xB6\x7D"
            "\xA5\xE6\xA9\xF6\xA4\xBA\xAE\x65\xA1\x43");
    }
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        std::array<char, 52> buffer;
        std::array<char, 1024> petSource;
        std::array<char, 1024> itemSource;
        std::array<char, 1024> petCopy;
        std::array<char, 1024> itemCopy;
        itemSource[0] = '\0';
        petSource[0] = '\0';
        std::int32_t count = 0;
        for (std::int32_t position = 0; position < 20; ++position) {
            const std::int32_t slot = Sub_4AFFE0(position);
            if (slot >= 0) {
                if (count != 0) {
                    std::strcat(itemSource.data(), "|");
                }
                std::sprintf(buffer.data(), "%d|%d", slot, position);
                std::strcat(itemSource.data(), buffer.data());
                ++count;
            }
        }
        std::strncpy(itemCopy.data(), itemSource.data(), 1024);
        count = 0;
        for (std::int32_t position = 0; position < 5; ++position) {
            const std::int32_t slot = Sub_4B00F0(position);
            if (slot >= 0) {
                if (count != 0) {
                    std::strcat(petSource.data(), "|");
                }
                std::sprintf(buffer.data(), "%d|%d", slot, position);
                std::strcat(petSource.data(), buffer.data());
                ++count;
            }
        }
        std::strncpy(petCopy.data(), petSource.data(), 1024);
        const std::int32_t amount = gTradeValueD046D0;
        static_cast<void>(network::Sub_512040(
            platform::gConnectionSocket, itemCopy.data(), petCopy.data(), amount));
        WindowInstance* const window = gCurrentWindow;
        gTradeValueD2E2B0 = 1;
        gTradeScrollD67430 = 0;
        static_cast<void>(Sub_4742C0(&window->elements[13], 2, 0));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        result = 1;
    }
    graphic->imageNo = 0x3BA89;
    if (hover) {
        graphic->imageNo = 0x3BA8B;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3BA8A;
    }
    return result;
}

std::int32_t Sub_4B0CF0(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    if (hover) {
        std::strcpy(gHoverTextD68AE8.data(), "\xC1\xD9\xAC\x4F\xBA\xE2\xA4\x46\xA1\x43");
    }
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        static_cast<void>(network::Sub_512230(platform::gConnectionSocket));
        const std::int32_t previous = gTradeValueCC4224;
        gTradeValueD2E2B0 = 0;
        if (previous == 2) {
            gTradeValueCC4224 = 1;
        }
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        gTradeStepD3040C = 23;
        result = 1;
    }
    graphic->imageNo = 0x3B562;
    if (hover) {
        graphic->imageNo = 0x3B564;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B563;
    }
    return result;
}

std::int32_t Sub_4B0DB0(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    const bool hover = (state & 1) != 0;
    if (hover) {
        std::strcpy(gHoverTextD68AE8.data(), "\xBD\x54\xA9\x77\xA5\xE6\xA9\xF6\xA1\x43");
    }
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        const std::int32_t phase = gTradeValueD2E2B0;
        if ((phase == 1 && gTradeValueCC4224 >= 1) || phase == 2) {
            const std::uint32_t tick = platform::ReadTickCount32();
            if (gWindowActionTick62B5AC + 500u < tick) {
                if (phase == 1) {
                    static_cast<void>(network::Sub_512270(platform::gConnectionSocket, 1));
                    gTradeValueD2E2B0 = 2;
                    gTradeValueD67CB8 = 1;
                    gTradeValueD67CBC = 1;
                } else {
                    static_cast<void>(network::Sub_512230(platform::gConnectionSocket));
                    const std::int32_t previous = gTradeValueCC4224;
                    gTradeValueD2E2B0 = 0;
                    if (previous == 2) {
                        gTradeValueCC4224 = 1;
                    }
                }
                gWindowActionTick62B5AC = platform::ReadTickCount32();
                static_cast<void>(audio::PlaySe_52C450(78, 320));
            }
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        result = 1;
    }
    graphic->imageNo = 0x3BA8C;
    if (hover) {
        graphic->imageNo = 0x3BA8E;
    }
    if (gTradeValueD2E2B0 == 2) {
        graphic->imageNo = 0x3BA8D;
    }
    return result;
}

login::YobiAction* Sub_4E5620(const char* const name)
{
    gTradeItemsD460D0.fill('\0');
    gTradePetsC9C108.fill('\0');
    gTradeValueCF4A1C = 0;
    gTradeValueD673E0 = 0;
    gTradeValueCC4C94 = 0;
    gTradeValueC8AF68 = 0;
    gTradeValueCF4688 = 0;
    gTradeValueCB03CC = 0;
    gTradeValueCF4AB8 = 0;
    gTradeValueD046D0 = 0;
    gTradeValueCC4224 = 0;
    gTradeValueD2E2B0 = 0;
    Sub_4E5600(1);
    Sub_4E52B0(1);
    std::strcpy(gTradeNameD2D654.data(), std::strlen(name) <= 16 ? name : "???");
    gTradeValueD67CB8 = 0;
    gTradeValueD67CBC = 0;
    gTradeValueCC3EC4 = -1;
    gTradeValueD66E08 = 0;
    gTradeValueCC2344 = 0;
    return Sub_4B0110();
}

void Sub_4E4E60(const std::int32_t index, const char* const name)
{
    static_cast<void>(network::Sub_511FF0(platform::gConnectionSocket, index));
    static_cast<void>(Sub_4E5620(name));
}

login::YobiAction* Sub_49E800(
    const char* const text, const WindowListSelectionCallback callback)
{
    gWindowListCountCF46A8 = 0;
    for (auto& name : gWindowListNamesCF3F20) {
        name[0] = '\0';
    }
    std::array<char, 256> buffer;
    std::int32_t count = 0;
    for (std::int32_t processed = 0; processed < 10 && count < 10; ++processed) {
        if (network::ExtractDelimitedField_52CED0(
                text, '|', processed + 1, 255, buffer.data()) == 1) {
            break;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        count = gWindowListCountCF46A8;
        char* const destination = gWindowListNamesCF3F20[count].data();
        const std::size_t length = std::strlen(buffer.data()) + 1;
        const std::size_t prefix = length & ~std::size_t{3};
        std::memcpy(destination, buffer.data(), prefix);
        gWindowListCountCF46A8 = ++count;
        std::memcpy(destination + prefix, buffer.data() + prefix, length - prefix);
    }
    gWindowListCallbackCB1E14 = callback;
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    static_cast<void>(Sub_4D3020(x, y));
    return CreateWindowAnimation_475AA0(44, 1, 0, nullptr);
}

login::YobiAction* Sub_4E4E80(const char* const text)
{
    return Sub_49E800(text, &Sub_4E4E60);
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

WindowInstance* Sub_4A83A0(
    const std::int32_t kind, const std::int32_t value14, const std::int32_t userState)
{
    gWindowStates[41].value10 = static_cast<std::uint32_t>(kind);
    WindowInstance* const window = OpenWindow_471440(41);
    gWindowStates[41].window = window;
    gWindowStates[41].value14 = static_cast<std::uint32_t>(value14);
    gWindowStates[41].userState.value = static_cast<std::uint32_t>(userState);
    return window;
}

WindowInstance* Sub_4AC0C0(
    const std::int32_t kind, const std::int32_t value14,
    const std::int32_t userState, const std::int32_t)
{
    gWindowStates[38].value10 = static_cast<std::uint32_t>(kind);
    WindowInstance* const window = OpenWindow_471440(38);
    gWindowStates[38].window = window;
    gWindowStates[38].value14 = static_cast<std::uint32_t>(value14);
    gWindowStates[38].userState.value = static_cast<std::uint32_t>(userState);
    return window;
}

void Sub_4AD090()
{
    if (gWindowLinesD67B70 != nullptr) {
        for (std::int32_t index = 0; index < gWindowLineCountD67B74; ++index) {
            std::free(gWindowLinesD67B70[index]);
        }
        std::free(gWindowLinesD67B70);
        gWindowLinesD67B70 = nullptr;
        gWindowLineCountD67B74 = 0;
    }
}

std::uint32_t Sub_4AD0F0(WindowInstance* const window)
{
    std::uint32_t result = 0;
    for (std::int32_t line = 0; line < 9; ++line) {
        const std::int32_t index = gWindowLineScrollCF46B8 + line;
        const std::int32_t count = gWindowLineCountD67B74;
        auto* const data = static_cast<WindowTextData*>(window->elements[line + 5].data);
        const char* const source =
            index < count ? gWindowLinesD67B70[index] : input::LegacyText593850();
        result = static_cast<std::uint32_t>(std::strlen(source) + 1);
        std::memcpy(data->text, source, result);
    }
    return result;
}

namespace {

// 两个生产入口共享原双遍算法；预计数的<=和实际末段推进30不能合并。
void RebuildWindowLines51(char* const text)
{
    char* const end = text + std::strlen(text);
    gWindowLineCountD67B74 = 0;
    char* segment = text;
    for (;;) {
        char* separator = std::strchr(segment, '\n');
        if (separator == nullptr) {
            separator = end;
        }
        const std::int32_t length = static_cast<std::int32_t>(separator - segment);
        if (length > 30) {
            std::int32_t processed = 0;
            --gWindowLineCountD67B74;
            do {
                const std::int32_t step =
                    input::ClassifyLegacyBoundary_44B440(segment, 30) == 2 ? 29 : 30;
                processed += step;
                segment += step;
                ++gWindowLineCountD67B74;
            } while (processed <= length);
        }
        ++gWindowLineCountD67B74;
        if (separator >= end) {
            break;
        }
        segment = separator + 1;
    }
    gWindowLinesD67B70 = static_cast<char**>(
        std::calloc(gWindowLineCountD67B74, sizeof(char*)));

    segment = text;
    for (std::int32_t line = 0; line < gWindowLineCountD67B74; ++line) {
        char* separator = std::strchr(segment, '\n');
        if (separator != nullptr) {
            *separator = '\0';
        } else {
            separator = end;
        }
        const std::int32_t length = static_cast<std::int32_t>(std::strlen(segment));
        if (length == 0) {
            gWindowLinesD67B70[line] = static_cast<char*>(std::malloc(1));
            gWindowLinesD67B70[line][0] = '\0';
        }
        std::int32_t produced = 0;
        std::int32_t remaining = length;
        for (std::int32_t processed = 0; processed < length; ++produced) {
            gWindowLinesD67B70[line + produced] = static_cast<char*>(std::malloc(31));
            std::int32_t step = 30;
            if (remaining > 30) {
                if (input::ClassifyLegacyBoundary_44B440(segment, 30) == 2) {
                    step = 29;
                }
                std::memcpy(gWindowLinesD67B70[line + produced], segment, step);
                gWindowLinesD67B70[line + produced][step] = '\0';
            } else {
                std::strcpy(gWindowLinesD67B70[line + produced], segment);
            }
            remaining -= step;
            processed += step;
            segment += step;
        }
        if (produced > 0) {
            line += produced - 1;
        }
        segment = separator + 1;
    }
}

} // namespace

login::YobiAction* Sub_4AD180(
    const std::int32_t request, const std::int32_t target, const char* const text)
{
    gWindowRequestD672B0 = request;
    gWindowRequestCF46AC = target;
    std::array<char, 1024> buffer;
    if (network::ExtractDelimitedField_52CED0(text, '|', 1, 1023, buffer.data()) != 0) {
        return nullptr;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    std::strcpy(gWindowTitleCC4118.data(), buffer.data());
    Sub_4AD090();
    network::ExtractDelimitedField_52CED0(text, '|', 2, 1023, buffer.data());
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    RebuildWindowLines51(buffer.data());
    return CreateWindowAnimation_475AA0(51, 1, 0, nullptr);
}

void Sub_4AD470(const char* const text)
{
    std::array<char, 1024> buffer;
    if (network::ExtractDelimitedField_52CED0(text, '|', 1, 1023, buffer.data()) != 0) {
        return;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    WindowInstance* const window = gWindowStates[51].window;
    std::strcpy(gWindowTitleCC4118.data(), buffer.data());
    auto* const title = static_cast<WindowTextData*>(window->elements[3].data);
    std::strcpy(title->text, gWindowTitleCC4118.data());
    Sub_4AD090();
    network::ExtractDelimitedField_52CED0(text, '|', 2, 1023, buffer.data());
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    RebuildWindowLines51(buffer.data());
    WindowInstance* const updatedWindow = gWindowStates[51].window;
    if (updatedWindow != nullptr) {
        static_cast<void>(Sub_4AD0F0(updatedWindow));
    }
}

void Sub_4AD780()
{
    Sub_4AD090();
    if (gSelectedItem57EE0C >= 0) {
        auto* const hover = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
        const unsigned char value = hover[gSelectedItem57EE0C];
        if (std::bit_cast<std::int8_t>(value) >= 0) {
            hover[gSelectedItem57EE0C] = static_cast<unsigned char>(value - 1);
        }
        gSelectedItem57EE0C = -1;
    }
    if (gWindowCloseRequestCC4B28 != 0) {
        const std::int32_t target = gWindowRequestCF46AC;
        const std::int32_t request = gWindowRequestD672B0;
        const std::int32_t y =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t x =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 0, "C"));
    }
    world::detail::gValue581800 = -1;
}

std::int32_t Sub_4AD800(const std::int32_t value)
{
    if (value == -1) {
        gSelectedItem57EE0C = -1;
        gWindowLineScrollCF46B8 = 0;
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[3].visible = 1;
        gCurrentWindow->elements[4].visible = 0;
        auto* const title = static_cast<WindowTextData*>(gCurrentWindow->elements[3].data);
        std::strcpy(title->text, gWindowTitleCC4118.data());
        static_cast<void>(Sub_4AD0F0(gCurrentWindow));
        gCurrentWindow->elements[0].visible = 0;
        gWindowCloseRequestCC4B28 = 1;
    }
    bool close = true;
    if (world::detail::gValue581800 == 32) {
        close = Sub_4D3040(1, 1) != 0;
    } else {
        gWindowCloseRequestCC4B28 = 0;
    }
    if (close) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (input::gCurrentInputData != &input::gInputContextCC49C0) {
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[3].visible = 1;
        gCurrentWindow->elements[4].visible = 0;
    }
    if ((static_cast<std::uint8_t>(login::detail::ReadGuildValue10514B4()) & 4u) != 0) {
        gCurrentWindow->elements[22].visible = 0;
        if (gSelectedItem57EE0C >= 0) {
            gCurrentWindow->elements[22].visible = 1;
        }
    }
    gCurrentWindow->elements[2].visible = 0;
    gCurrentWindow->elements[21].visible = 0;
    if ((static_cast<std::uint8_t>(login::detail::ReadGuildValue10514B4()) & 0x80u) != 0) {
        gCurrentWindow->elements[2].visible = 1;
        gCurrentWindow->elements[21].visible = 1;
    }
    gCurrentWindow->elements[22].visible = 0;
    if ((static_cast<std::uint8_t>(login::detail::ReadGuildValue10514B4()) & 4u) != 0) {
        if (gSelectedItem57EE0C < 0) {
            return 1;
        }
        gCurrentWindow->elements[22].visible = 1;
    }
    if (gSelectedItem57EE0C >= 0
        && world::detail::gPlayerItemsFDA8A4[gSelectedItem57EE0C].itemType == 53) {
        gCurrentWindow->elements[22].visible = 1;
    }
    return 1;
}

std::int32_t Sub_4AD9F0(const std::int32_t)
{
    gCurrentWindow->elements[14].visible = 1;
    if (gWindowLineCountD67B74 - 9 <= 0) {
        gCurrentWindow->elements[14].visible = 0;
    }
    WindowInstance* window = gCurrentWindow;
    auto* const mouse = static_cast<WindowMouseData*>(window->elements[15].data);
    if ((static_cast<std::uint8_t>(mouse->flags) & 1u) != 0) {
        gWindowLineScrollCF46B8 = Sub_474270(
            &window->elements[15], gWindowLineCountD67B74 - 9);
        static_cast<void>(Sub_4AD0F0(gCurrentWindow));
        window = gCurrentWindow;
    }
    const std::int32_t y = window->y + window->elements[4].y;
    const std::int32_t x = window->x + window->elements[4].x;
    static_cast<void>(Sub_476570(&gTitleInputPrototype57EE10, x, y, 2));
    DrawCurrentWindow_4729B0();
    return 1;
}

std::int32_t Sub_4ADAA0(const std::int32_t index, const std::int32_t)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t imageNo = 243048;
    if (login::detail::Sub_5049A0(287, 218, 353, 235, -1) != 0) {
        imageNo = 243050;
        if ((static_cast<std::uint8_t>(platform::gLegacyMouseState.edgeButtons) & 1u) != 0) {
            gCurrentWindow->elements[index].visible = 0;
            gCurrentWindow->state &= ~2u;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            imageNo = 243049;
        }
    }
    static_cast<void>(login::detail::DrawGraphic(320, 227, 226, imageNo, 0, &descriptor));
    static_cast<void>(render::DrawText_456780(
        185, 185, 4, 0, 0, gWindowModalTextC8AF88.data(), 0,
        static_cast<std::uint8_t>(login::detail::gValueD69768)));
    login::detail::DrawNineGrid_49BE00(
        245300, 155, 155, 330, 110, 226, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

std::int32_t Sub_4ADBD0(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        if (gWindowLineScrollCF46B8 > 0) {
            --gWindowLineScrollCF46B8;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gWindowLineScrollCF46B8;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t maximum = gWindowLineCountD67B74 - 9;
        static_cast<void>(Sub_4742C0(&window->elements[index - 1], maximum, position));
        static_cast<void>(Sub_4AD0F0(gCurrentWindow));
        result = 1;
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return result;
}

std::int32_t Sub_4ADC80(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t last = gWindowLineCountD67B74 - 9;
        if (gWindowLineScrollCF46B8 < (last < 0 ? 0 : last)) {
            ++gWindowLineScrollCF46B8;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gWindowLineScrollCF46B8;
        const std::int32_t maximum = gWindowLineCountD67B74 - 9;
        static_cast<void>(Sub_4742C0(&gCurrentWindow->elements[index - 2], maximum, position));
        static_cast<void>(Sub_4AD0F0(gCurrentWindow));
        result = 1;
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return result;
}

std::int32_t Sub_4ADD50(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(gWindowLineScrollCF46B8 > 0 ? 51 : 55, 320));
        gWindowLineScrollCF46B8 -= 9;
        if (gWindowLineScrollCF46B8 < 0) {
            gWindowLineScrollCF46B8 = 0;
        }
        const std::int32_t position = gWindowLineScrollCF46B8;
        const std::int32_t maximum = gWindowLineCountD67B74 - 9;
        static_cast<void>(Sub_4742C0(&gCurrentWindow->elements[index - 3], maximum, position));
        static_cast<void>(Sub_4AD0F0(gCurrentWindow));
        result = 1;
    }
    graphic->imageNo = 243030;
    if ((state & 1) != 0) {
        graphic->imageNo = 243032;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243031;
    }
    return result;
}

std::int32_t Sub_4ADE10(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        const std::int32_t last = gWindowLineCountD67B74 - 9;
        static_cast<void>(audio::PlaySe_52C450(
            last > 0 && gWindowLineScrollCF46B8 < last ? 51 : 55, 320));
        const std::int32_t maximum = gWindowLineCountD67B74 - 9;
        gWindowLineScrollCF46B8 += 9;
        const std::int32_t limit = maximum < 0 ? 0 : maximum;
        if (gWindowLineScrollCF46B8 > limit) {
            gWindowLineScrollCF46B8 = limit;
        }
        const std::int32_t position = gWindowLineScrollCF46B8;
        static_cast<void>(Sub_4742C0(&gCurrentWindow->elements[index - 4], maximum, position));
        static_cast<void>(Sub_4AD0F0(gCurrentWindow));
        result = 1;
    }
    graphic->imageNo = 243033;
    if ((state & 1) != 0) {
        graphic->imageNo = 243035;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243034;
    }
    return result;
}

std::int32_t Sub_4ADEF0(const std::int32_t index, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gWindowLineCountD67B74;
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t position = gWindowLineScrollCF46B8;
        gWindowLineScrollCF46B8 =
            Sub_4743B0(&gCurrentWindow->elements[index - 5], position, count - 9, wheel);
    }
    return 0;
}

std::int32_t Sub_4ADF40(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    std::int32_t consumed = 0;
    if (elements[4].visible != 0) {
        if ((state & 1) != 0) {
            std::strcpy(gHoverTextD68AE8.data(),
                "\xC5\xDC\xA7\xF3\xAE\x61\xB1\xDA\xC3\x7E\xA6\x57\xBA\xD9\xA1\x43");
        }
        if ((state & 2) != 0 || platform::ReadKeyFlag4_4597A0(13) != 0) {
            std::strcpy(gWindowTitleCC4118.data(), input::gInputContextCC49C0.text);
            static_cast<void>(Sub_4A7B00());
            auto* const title = static_cast<WindowTextData*>(gCurrentWindow->elements[3].data);
            std::strcpy(title->text, gWindowTitleCC4118.data());
            gCurrentWindow->elements[4].visible = 0;
            gCurrentWindow->elements[2].visible = 1;
            gCurrentWindow->elements[3].visible = 1;
            std::array<char, 512> buffer;
            std::strcpy(buffer.data(), gWindowTitleCC4118.data());
            input::EscapeLegacyTextAndNotify_52D2D0(buffer.data(), buffer.data() + 256, 255);
            std::sprintf(buffer.data(), "N|%s", buffer.data() + 256);
            input::EscapeLegacyText_52D1B0(buffer.data(), buffer.data() + 256, 255);
            const std::int32_t target = gWindowRequestCF46AC;
            const std::int32_t request = gWindowRequestD672B0;
            const std::int32_t y =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t x =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, 0, buffer.data() + 256));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            consumed = 1;
        }
    } else if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(55, 320));
    }
    graphic->imageNo = 244215;
    if ((state & 1) != 0) {
        graphic->imageNo = 244217;
    }
    if (input::gCurrentInputData != &input::gInputContextCC49C0) {
        graphic->imageNo = 244216;
    }
    return consumed;
}

std::int32_t Sub_4AE170(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xA5\xCE\xB7\xC6\xB9\xAB\xA9\xEC\xA9\xF1\xAA\xAB\xAB\x7E\xA6\xDC"
            "\xA6\xB9\xB5\xF8\xB5\xA1\xB0\xB5\xAC\xB0\xB9\x7D\xAE\xC6\xA1\x43");
    }
    if ((state & 2) != 0) {
        std::array<char, 52> buffer;
        std::sprintf(buffer.data(), "I|%d", gSelectedItem57EE0C);
        const std::int32_t target = gWindowRequestCF46AC;
        const std::int32_t request = gWindowRequestD672B0;
        const std::int32_t y =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t x =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 0, buffer.data()));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    graphic->imageNo = 244332;
    if ((state & 1) != 0) {
        graphic->imageNo = 244334;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 244333;
    }
    if (gSelectedItem57EE0C < 0) {
        graphic->imageNo = 244333;
    }
    return consumed;
}

std::int32_t Sub_4AE260(const std::int32_t index, const std::int32_t state)
{
    const std::uint32_t imageNo = login::detail::gValueD69768;
    Sub_485110(&gCurrentWindow->elements[index], state, imageNo);
    if ((state & 2) == 0) {
        return 0;
    }
    gCurrentWindow->elements[2].visible = 0;
    gCurrentWindow->elements[3].visible = 0;
    gCurrentWindow->elements[4].visible = 1;
    WindowInstance* const window = gCurrentWindow;
    const std::int32_t y = window->y + window->elements[4].y;
    const std::int32_t x = window->x + window->elements[4].x;
    static_cast<void>(Sub_476570(&gTitleInputPrototype57EE10, x, y, 0));
    input::SetContext_44B330(&input::gInputContextCC49C0);
    input::LoadInitialText_44A160(gWindowTitleCC4118.data());
    WindowInstance* const focusedWindow = gCurrentWindow;
    gFocusedElementDataD45F94 = focusedWindow->elements[4].data;
    static_cast<WindowInputData*>(focusedWindow->elements[4].data)->context = &input::gInputContextCC49C0;
    static_cast<void>(audio::PlaySe_52C450(51, 320));
    return 1;
}

std::int32_t Sub_4AE350(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    const auto x = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(window->elements[index].x + window->x));
    const auto y = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(window->elements[index].y + window->y));
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    if ((events & 0x60u) != 0 && Sub_475D60() != 2 && Sub_475D60() != 8) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t source = Sub_475D90();
        if (source >= 8) {
            if ((static_cast<std::uint8_t>(login::detail::ReadGuildValue10514B4()) & 4u) != 0
                || world::detail::gPlayerItemsFDA8A4[source].itemType == 53) {
                const unsigned char previous = occupancy[gSelectedItem57EE0C];
                if (std::bit_cast<std::int8_t>(previous) >= 0) {
                    occupancy[gSelectedItem57EE0C] = static_cast<unsigned char>(previous - 1);
                }
                const unsigned char next = occupancy[source];
                gSelectedItem57EE0C = source;
                occupancy[source] = static_cast<unsigned char>(next + 1);
            } else {
                WindowInstance* const modalWindow = gCurrentWindow;
                std::strcpy(gWindowModalTextC8AF88.data(),
                    "\xA7\x41\xA8\x53\xA6\xB3\xC1\xFD\xAD\xB9\xC3\x64"
                    "\xAA\xAB\xAA\xBA\xC5\x76\xAD\xAD\xA1\x43");
                modalWindow->elements[0].visible = 1;
                gCurrentWindow->state |= 2u;
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            }
            Sub_476500();
        }
    }
    std::int32_t selected;
    if ((events & 1u) != 0 && (events & 2u) != 0) {
        selected = gSelectedItem57EE0C;
        if (selected >= 0) {
            static_cast<void>(Sub_48BF10(selected, 1));
            const unsigned char previous = occupancy[gSelectedItem57EE0C];
            if (std::bit_cast<std::int8_t>(previous) >= 0) {
                occupancy[gSelectedItem57EE0C] = static_cast<unsigned char>(previous - 1);
            }
            gSelectedItem57EE0C = -1;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
            selected = gSelectedItem57EE0C;
        }
        consumed = 1;
    } else {
        selected = gSelectedItem57EE0C;
    }
    if ((events & 0x21u) != 0) {
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
            static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
            223, login::detail::gValueD69768, 0);
        selected = gSelectedItem57EE0C;
    }
    if (selected >= 0) {
        const auto& item = world::detail::gPlayerItemsFDA8A4[selected];
        if (item.used != 0) {
            static_cast<void>(login::detail::DrawGraphic(
                x + 24, y + 24, 223, item.imageNo, 0, &descriptor));
            return consumed;
        }
        const unsigned char previous = occupancy[selected];
        if (std::bit_cast<std::int8_t>(previous) >= 0) {
            occupancy[selected] = static_cast<unsigned char>(previous - 1);
        }
        gSelectedItem57EE0C = -1;
    }
    return consumed;
}

std::int32_t Sub_4AE5B0(const std::int32_t, const std::int32_t state)
{
    if ((state & 1) != 0 && gHoverTextD68AE8[0] == '\0') {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xB1\x71\xAA\xAB\xAB\x7E\xB5\xF8\xB5\xA1\xA5\x48\xB7\xC6\xB9\xAB"
            "\xA9\xEC\xA9\xF1\xA4\xE8\xA6\xA1\xBF\xEF\xBE\xDC\xAD\x6E\xC1\xFD"
            "\xAA\xBA\xB9\x7D\xAE\xC6\xA1\x43");
    }
    return 0;
}

void Sub_4A83D0()
{
    const std::uint32_t kind = gWindowStates[41].value10;
    if ((kind == 0 && world::detail::gValue581800 == 23)
        || (kind == 1 && world::detail::gValue581800 == 33)) {
        world::detail::gValue581800 = -1;
    }
    std::array<char, 256> buffer;
    input::EscapeLegacyTextAndNotify_52D2D0("C", buffer.data(), 255);
    const std::int32_t userState = std::bit_cast<std::int32_t>(gWindowStates[41].userState.value);
    const std::int32_t value14 = std::bit_cast<std::int32_t>(gWindowStates[41].value14);
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
    static_cast<void>(network::Sub_512620(
        platform::gConnectionSocket, x, y, value14, userState, 0, buffer.data()));
    WindowInstance* const numberWindow = gWindowStates[42].window;
    if (numberWindow != nullptr && gWindowStates[42].value20 == 41) {
        numberWindow->state |= 0x8000u;
    }
    if (input::IsCurrentContext_503E40(nullptr)) {
        input::SetContext_44B330(&input::gInputContext626AA0);
    }
}

std::int32_t Sub_4A9020(const std::int32_t value)
{
    if (value == -1) {
        auto* const title = static_cast<WindowGraphicData*>(gCurrentWindow->elements[36].data);
        title->imageNo = gStorageTitleImages54F4E8[gWindowStates[41].value10];
        WindowInstance* const initialWindow = gCurrentWindow;
        gWindowStates[41].value28 = 0;
        gWindowStates[41].value24 = 0;
        auto* const ownPanel = static_cast<WindowGraphicData*>(initialWindow->elements[34].data);
        ownPanel->imageNo = 243143;
        auto* const otherPanel = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        otherPanel->imageNo = gStoragePanelImages54F4F0[gWindowStates[41].value10];
        for (std::int32_t index = 14; index < 34; ++index) {
            gCurrentWindow->elements[index].visible = 0;
        }
        gCurrentWindow->elements[13].visible = 0;
    }
    if (Sub_4D3040(1, 1) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (gWindowStates[41].value10 == 0 && world::detail::gValue581800 != 23) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (gWindowStates[41].value10 == 1 && world::detail::gValue581800 != 33) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    gStorageValueCC4218 = 0;
    gWindowStates[41].actionFlags1C = 0xFFFFFFFFu;
    return 1;
}

std::int32_t Sub_4A9160(const std::int32_t)
{
    std::array<char, 16> buffer;
    std::sprintf(buffer.data(), "%10d", gStorageValueCC4218);
    auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[13].data);
    std::strcpy(text->text, buffer.data());
    WindowElement* elements = gCurrentWindow->elements;
    const auto* const firstScroll = static_cast<WindowMouseData*>(elements[21].data);
    if ((firstScroll->flags & 1u) != 0) {
        gWindowStates[41].value24 = static_cast<std::uint32_t>(Sub_474270(elements + 21, 2));
    }
    elements = gCurrentWindow->elements;
    const auto* const secondScroll = static_cast<WindowMouseData*>(elements[26].data);
    if ((secondScroll->flags & 1u) != 0) {
        gWindowStates[41].value28 = static_cast<std::uint32_t>(Sub_474270(elements + 26, 2));
    }
    const std::int32_t preview = std::bit_cast<std::int32_t>(gWindowStates[41].actionFlags1C);
    if (preview >= 0) {
        if (preview >= 200) {
            gWindowStates[41].actionFlags1C = static_cast<std::uint32_t>(preview - 200);
            const bool left = platform::legacy_crypt_int::Less_401DB0(
                &platform::gLegacyMouseState.mouseX, 320);
            const std::int32_t x = left ? 413 : 0;
            const std::int32_t index =
                std::bit_cast<std::int32_t>(gWindowStates[41].actionFlags1C);
            if (index < 100) {
                static_cast<void>(Sub_4B3BF0(x, 0, index));
            } else {
                const std::uint8_t externalIndex = static_cast<std::uint8_t>(
                    static_cast<std::uint8_t>(index) - 100);
                static_cast<void>(Sub_4B4140(x, 0, externalIndex));
            }
        } else {
            const std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[41].value20);
            if (preview >= 100) {
                static_cast<void>(Sub_48B350(preview - 100, page));
            } else {
                static_cast<void>(Sub_48B1A0(preview, page));
            }
        }
    }
    DrawCurrentWindow_4729B0();
    return 1;
}

void Sub_4AC0F0()
{
    if (world::detail::gValue581800 == 34) {
        world::detail::gValue581800 = -1;
    }
    WindowInstance* const numberWindow = gWindowStates[42].window;
    if (numberWindow != nullptr && gWindowStates[42].value20 == 41) {
        numberWindow->state |= 0x8000u;
    }
    if (input::IsCurrentContext_503E40(nullptr)) {
        input::SetContext_44B330(&input::gInputContext626AA0);
    }
}

std::int32_t Sub_4AC140(const std::int32_t)
{
    if (Sub_4D3040(1, 1) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    if (world::detail::gValue581800 != 34) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    gWindowStates[38].actionFlags1C = 0xFFFFFFFFu;
    return 1;
}

std::int32_t Sub_4AC1B0(const std::int32_t)
{
    const std::int32_t preview = std::bit_cast<std::int32_t>(gWindowStates[38].actionFlags1C);
    if (preview >= 0) {
        const std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[38].value20);
        if (preview < 100) {
            static_cast<void>(Sub_48B1A0(preview, page));
        } else {
            static_cast<void>(Sub_48B350(preview - 100, page));
        }
    }
    DrawCurrentWindow_4729B0();
    return 1;
}

std::int32_t Sub_4A92D0(const std::int32_t, const std::int32_t state)
{
    const WindowElement* const elements = gCurrentWindow->elements;
    if (elements[30].visible == 0 && elements[31].visible == 0) {
        if (input::gCurrentInputData == &input::gInputContextCA0018) {
            static_cast<void>(Sub_4A7B00());
        }
        return 0;
    }
    if ((state & 2) != 0) {
        WindowInstance* const window = gWindowStates[41].window;
        static_cast<void>(Sub_476570(&gStorageInputPrototype57EBA0, 0, -100, 0));
        input::SetContext_44B330(&input::gInputContextCA0018);
        gFocusedElementDataD45F94 = window->elements[0].data;
        static_cast<WindowInputData*>(window->elements[0].data)->context = &input::gInputContextCA0018;
    }
    return 0;
}

std::int32_t Sub_4A93E0(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    if (gLoginWindowRuntime.valueD67618 == 0) {
        elements[3].visible = 0;
        gCurrentWindow->elements[4].visible = 1;
    }
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xBD\xD0\xBF\xEF\xBE\xDC\xBB\xC8\xA6\xE6\xA4\xC0\xAD\xB6");
    }
    if ((state & 2) != 0) {
        gLoginWindowRuntime.valueD67618 = 0;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        static_cast<void>(network::Sub_5127B0(platform::gConnectionSocket, 1));
    }
    graphic->imageNo = 243173;
    if ((state & 1) != 0) {
        graphic->imageNo = 243175;
    }
    return 0;
}

std::int32_t Sub_4A94A0(const std::int32_t index, const std::int32_t state)
{
    WindowElement* const elements = gCurrentWindow->elements;
    auto* const graphic = static_cast<WindowGraphicData*>(elements[index].data);
    if (gLoginWindowRuntime.valueD67618 == 1) {
        elements[3].visible = 1;
        gCurrentWindow->elements[4].visible = 0;
    }
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xBD\xD0\xBF\xEF\xBE\xDC\xBB\xC8\xA6\xE6\xA4\xC0\xAD\xB6");
    }
    if ((state & 2) != 0) {
        gLoginWindowRuntime.valueD67618 = 1;
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        static_cast<void>(network::Sub_5127B0(platform::gConnectionSocket, 2));
    }
    graphic->imageNo = 243176;
    if ((state & 1) != 0) {
        graphic->imageNo = 243178;
    }
    return 0;
}

std::int32_t Sub_4A9560(const std::int32_t index, const std::int32_t)
{
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 243174;
    return 0;
}

std::int32_t Sub_4A9580(const std::int32_t index, const std::int32_t)
{
    static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = 243177;
    return 0;
}

std::int32_t Sub_4A95A0(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(), "\xA6\x73\xA9\xF1\xAA\xAB\xAB\x7E\xA1\x43");
    }
    if ((state & 2) != 0) {
        const auto* const panel = static_cast<WindowGraphicData*>(window->elements[34].data);
        static_cast<void>(audio::PlaySe_52C450(panel->imageNo == 243143 ? 55 : 51, 320));
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo = 243143;
        auto* const external = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        external->imageNo = gStoragePanelImages54F4F0[gWindowStates[41].value10];
        for (std::int32_t element = 14; element < 34; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gCurrentWindow->elements[34].mouseCallback = Sub_4A9D10;
        gCurrentWindow->elements[35].mouseCallback = Sub_4AA3D0;
    }
    graphic->imageNo = 243027;
    if ((state & 1) != 0) {
        graphic->imageNo = 243029;
    }
    if (static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo == 243143) {
        graphic->imageNo = 243028;
    }
    return 0;
}

std::int32_t Sub_4A96D0(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(), "\xA6\x73\xA9\xF1\xC3\x64\xAA\xAB\xA1\x43");
    }
    if ((state & 2) != 0) {
        const auto* const panel = static_cast<WindowGraphicData*>(window->elements[34].data);
        static_cast<void>(audio::PlaySe_52C450(panel->imageNo == 243145 ? 55 : 51, 320));
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo = 243145;
        auto* const external = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        external->imageNo = gStoragePetImages54F4F8[gWindowStates[41].value10];
        for (std::int32_t element = 14; element < 34; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        for (std::int32_t element = 14; element <= 29; ++element) {
            gCurrentWindow->elements[element].visible = 1;
        }
        gCurrentWindow->elements[34].mouseCallback = Sub_4B0F10;
        gCurrentWindow->elements[35].mouseCallback = Sub_4B0F10;
    }
    graphic->imageNo = 243021;
    if ((state & 1) != 0) {
        graphic->imageNo = 243023;
    }
    if (static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo == 243145) {
        graphic->imageNo = 243022;
    }
    return 0;
}

std::int32_t Sub_4A9820(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(), "\xA6\x73\xA9\xF1\xAA\xF7\xBF\xFA\xA1\x43");
    }
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(window->elements[30].visible != 0 ? 55 : 51, 320));
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo = 243144;
        auto* const external = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        external->imageNo = gStorageMoneyImages54F500[gWindowStates[41].value10];
        for (std::int32_t element = 14; element < 34; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gCurrentWindow->elements[30].visible = 1;
        gCurrentWindow->elements[32].visible = 1;
        gCurrentWindow->elements[33].visible = 1;
        gCurrentWindow->elements[34].mouseCallback = Sub_4B0F10;
        gCurrentWindow->elements[35].mouseCallback = Sub_4B0F10;
        gCurrentWindow->elements[30].state = 0;
        static_cast<void>(Sub_4A92D0(0, 2));
    }
    graphic->imageNo = 243024;
    if ((state & 1) != 0) {
        graphic->imageNo = 243026;
    }
    if (gCurrentWindow->elements[30].visible != 0) {
        graphic->imageNo = 243025;
    }
    return 0;
}

std::int32_t Sub_4A9990(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(), "\xB4\xA3\xBB\xE2\xAA\xF7\xBF\xFA\xA1\x43");
    }
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(window->elements[31].visible != 0 ? 55 : 51, 320));
        static_cast<WindowGraphicData*>(gCurrentWindow->elements[34].data)->imageNo = 243144;
        auto* const external = static_cast<WindowGraphicData*>(gCurrentWindow->elements[35].data);
        external->imageNo = gStorageMoneyImages54F500[gWindowStates[41].value10];
        for (std::int32_t element = 14; element < 34; ++element) {
            gCurrentWindow->elements[element].visible = 0;
        }
        gCurrentWindow->elements[31].visible = 1;
        gCurrentWindow->elements[32].visible = 1;
        gCurrentWindow->elements[33].visible = 1;
        gCurrentWindow->elements[34].mouseCallback = Sub_4B0F10;
        gCurrentWindow->elements[35].mouseCallback = Sub_4B0F10;
        gCurrentWindow->elements[31].state = 0;
        static_cast<void>(Sub_4A92D0(0, 2));
    }
    graphic->imageNo = 243024;
    if ((state & 1) != 0) {
        graphic->imageNo = 243026;
    }
    if (gCurrentWindow->elements[31].visible != 0) {
        graphic->imageNo = 243025;
    }
    return 0;
}

std::int32_t Sub_4A9D10(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::memcpy(dimmed.bytes.data() + 8, &dimmedColor, sizeof(dimmedColor));
    dimmed.bytes[13] = 4;
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gValueD67B08.data());
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && Sub_475D60() != 2) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t source = Sub_475D90();
        const WindowInstance* const window = gCurrentWindow;
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 13;
            const std::int32_t top = element.y + 52 * (position / 5) + window->y + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (source != position + 8) {
                Sub_4886E0(41, index, source, position + 8);
            }
            break;
        }
        Sub_476500();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            if (occupancy[position + 8] != 0) {
                continue;
            }
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 13;
            const std::int32_t top = element.y + 52 * (position / 5) + window->y + 26;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
    }
    if ((events & 1u) != 0 && hovered >= 0
        && world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
        if ((world::detail::gPlayerItemsFDA8A4[hovered + 8].flags & 0x60u) != 0) {
            std::strcpy(gHoverTextD68AE8.data(),
                "\xA6\xB9\xAA\xAB\xAB\x7E\xB5\x4C\xAA\x6B\xA6\x73\xA9\xF1\xA1\x43");
        } else {
            std::strcpy(gHoverTextD68AE8.data(),
                "\xA5\xCE\xB7\xC6\xB9\xAB\xA9\xEC\xA9\xF1\xA8\xD3"
                "\xA6\x73\xA9\xF1\xAA\xAB\xAB\x7E\xA1\x43");
        }
    }
    if (Sub_475D60() != 0 && (events & 0x20u) != 0 && Sub_475D90() >= 100
        && world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0
        && (world::detail::gPlayerItemsFDA8A4[hovered + 8].flags & 0x60u) != 0) {
        hovered = -1;
    }
    if (gBankOwnHover57EDCC != hovered) {
        gWindowStates[41].value20 = 0;
    }
    gBankOwnHover57EDCC = hovered;
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (Sub_475D40() == 41 || Sub_475D40() == 0) {
            static_cast<void>(Sub_4763E0());
            std::int32_t target = 0;
            for (; target < 20; ++target) {
                std::uint16_t used;
                std::memcpy(&used, gItemDetailStorageCC4D58.data() + target * 0xC44, sizeof(used));
                if (used == 0) {
                    break;
                }
            }
            if (target >= 20 || world::detail::gPlayerItemsFDA8A4[hovered + 8].used == 0) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                Sub_4886E0(41, index, hovered + 8, target + 100);
            }
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
                static_cast<void>(Sub_48BF10(hovered + 8, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[41].value20);
                const std::int32_t pages = world::detail::gPlayerItemsFDA8A4[hovered + 8].pages;
                ++page;
                gWindowStates[41].value20 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[41].value20 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (Sub_475D60() == 2) {
        const std::int32_t source = Sub_475D90();
        if (source >= 8) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * ((source - 8) % 5) + window->x + 13;
            const std::int32_t top = element.y + 52 * ((source - 8) / 5) + window->y + 26;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 13;
        const std::int32_t top = element.y + 52 * (hovered / 5) + window->y + 26;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
            gWindowStates[41].actionFlags1C = static_cast<std::uint32_t>(hovered + 8);
            if (Sub_475D60() == 2 && Sub_475D90() >= 100) {
                ++gStorageValueCC4218;
            }
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::array<char, 12> buffer;
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = world::detail::gPlayerItemsFDA8A4[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (position % 5) + window->x + 37;
        const std::int32_t y = element.y + 52 * (position / 5) + window->y + 51;
        if ((item.flags & 0x60u) != 0) {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, 23040, 0, nullptr));
        }
        if (occupancy[position + 8] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
        }
        const std::int32_t count = item.count;
        if (count > 0) {
            std::sprintf(buffer.data(), "%3d", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                3, 1, 1, buffer.data(), 0, 0));
        }
    }
    return consumed;
}

std::int32_t Sub_4AA3D0(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && (Sub_475D60() != 2 || Sub_475D90() < 8)) {
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t source = Sub_475D90();
        const WindowInstance* const window = gCurrentWindow;
        const std::int32_t elementX = window->elements[index].x;
        const std::int32_t windowX = window->x;
        const std::int32_t windowY = window->y;
        const std::int32_t elementY = window->elements[index].y;
        for (std::int32_t position = 0; position < 20; ++position) {
            const std::int32_t left = windowX + 50 * (position % 5) + elementX + 13;
            const std::int32_t top = windowY + 52 * (position / 5) + elementY + 26;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (source != position + 100
                && (source >= 100 || (world::detail::gPlayerItemsFDA8A4[source].flags & 0x60u) == 0)) {
                Sub_4886E0(41, index, source, position + 100);
            }
            break;
        }
        Sub_476500();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 13;
            const std::int32_t top = element.y + 52 * (position / 5) + window->y + 26;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
    }
    if (Sub_475D60() != 0 && (events & 0x20u) != 0) {
        const std::int32_t source = Sub_475D90();
        if (source < 100 && (world::detail::gPlayerItemsFDA8A4[source].flags & 0x60u) != 0) {
            hovered = -1;
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        std::uint16_t used;
        std::memcpy(&used, gItemDetailStorageCC4D58.data() + hovered * 0xC44, sizeof(used));
        if (used != 0) {
            std::strcpy(gHoverTextD68AE8.data(),
                "\xA5\xCE\xB7\xC6\xB9\xAB\xA9\xEC\xA9\xF1\xA8\xD3"
                "\xB4\xA3\xBB\xE2\xAA\xAB\xAB\x7E\xA1\x43");
        }
    }
    if (gBankExternalHover57EDD0 != hovered) {
        gWindowStates[41].value20 = 0;
    }
    gBankExternalHover57EDD0 = hovered;
    if (hovered >= 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (Sub_475D40() == 41 || Sub_475D40() == 0) {
            static_cast<void>(Sub_4763E0());
            std::int32_t target = 0;
            while (target < 20 && world::detail::gPlayerItemsFDA8A4[target + 8].used != 0) {
                ++target;
            }
            if (target >= 20) {
                static_cast<void>(audio::PlaySe_52C450(55, 320));
            } else {
                std::uint16_t used;
                std::memcpy(&used, gItemDetailStorageCC4D58.data() + hovered * 0xC44, sizeof(used));
                if (used == 0) {
                    static_cast<void>(audio::PlaySe_52C450(55, 320));
                } else {
                    Sub_4886E0(41, index, hovered + 100, target + 8);
                }
            }
        }
        consumed = 1;
    } else if ((events & 1u) != 0 && hovered >= 0) {
        const char* const record = gItemDetailStorageCC4D58.data() + hovered * 0xC44;
        if ((events & 2u) != 0) {
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                static_cast<void>(Sub_48BF10(hovered + 100, 0));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[41].value20);
                std::int32_t pages;
                std::memcpy(&pages, record + 0xC3C, sizeof(pages));
                ++page;
                gWindowStates[41].value20 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[41].value20 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (Sub_475D60() == 2) {
        const std::int32_t source = Sub_475D90();
        if (source >= 100) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * ((source - 100) % 5) + window->x + 13;
            const std::int32_t top = element.y + 52 * ((source - 100) / 5) + window->y + 26;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 13;
        const std::int32_t top = element.y + 52 * (hovered / 5) + window->y + 26;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        std::uint16_t used;
        std::memcpy(&used, gItemDetailStorageCC4D58.data() + hovered * 0xC44, sizeof(used));
        if (used != 0) {
            gWindowStates[41].actionFlags1C = static_cast<std::uint32_t>(hovered + 100);
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::array<char, 12> buffer;
    for (std::int32_t position = 0; position < 20; ++position) {
        const char* const record = gItemDetailStorageCC4D58.data() + position * 0xC44;
        std::uint16_t used;
        std::memcpy(&used, record, sizeof(used));
        if (used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (position % 5) + window->x + 37;
        const std::int32_t y = element.y + 52 * (position / 5) + window->y + 51;
        std::int32_t imageNo;
        std::memcpy(&imageNo, record + 4, sizeof(imageNo));
        static_cast<void>(login::detail::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
        std::int32_t count;
        std::memcpy(&count, record + 8, sizeof(count));
        if (count > 0) {
            std::sprintf(buffer.data(), "%3d", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                3, 1, 1, buffer.data(), 0, 0));
        }
    }
    return consumed;
}

std::int32_t Sub_4AA9C0(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    WindowState* const windowState = gCurrentWindowState;
    std::uint32_t* const scroll = index >= 25 ? &windowState->value28 : &windowState->value24;
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t value = std::bit_cast<std::int32_t>(*scroll);
        if (value > 0) {
            *scroll = static_cast<std::uint32_t>(value - 1);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t current = std::bit_cast<std::int32_t>(*scroll);
        static_cast<void>(Sub_4742C0(&window->elements[index - 1], 2, current));
        result = 1;
    }
    graphic->imageNo = 243090;
    if ((state & 1) != 0) {
        graphic->imageNo = 243092;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243091;
    }
    return result;
}

std::int32_t Sub_4AAA70(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    WindowState* const windowState = gCurrentWindowState;
    std::uint32_t* const scroll = index >= 25 ? &windowState->value28 : &windowState->value24;
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        const std::int32_t value = std::bit_cast<std::int32_t>(*scroll);
        if (value < 2) {
            *scroll = static_cast<std::uint32_t>(value + 1);
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t current = std::bit_cast<std::int32_t>(*scroll);
        WindowInstance* const window = gCurrentWindow;
        static_cast<void>(Sub_4742C0(&window->elements[index - 2], 2, current));
        result = 1;
    }
    graphic->imageNo = 243093;
    if ((state & 1) != 0) {
        graphic->imageNo = 243095;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243094;
    }
    return result;
}

std::int32_t Sub_4AAB20(const std::int32_t index, const std::int32_t state)
{
    WindowState* const windowState = gCurrentWindowState;
    std::uint32_t* const scroll = index >= 25 ? &windowState->value28 : &windowState->value24;
    if ((state & 0x21) != 0) {
        const std::int32_t wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t current = std::bit_cast<std::int32_t>(*scroll);
        *scroll = static_cast<std::uint32_t>(
            Sub_4743B0(&gCurrentWindow->elements[index - 3], current, 2, wheel));
    }
    return 0;
}

std::int32_t Sub_4AAEC0(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t scroll = std::bit_cast<std::int32_t>(gWindowStates[41].value24);
    const std::int32_t petIndex = std::bit_cast<std::int16_t>(
        world::detail::gPetPairsDE0B08[index + scroll - 14].index);
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    std::array<char, 128> buffer;
    // 原4AAEEC/4AB245各执行一次，三次索引查询和费用累计都不能复用前次结果。
    const auto drawDragSource = [index]() {
        if (Sub_475D60() == 3) {
            const std::int32_t source = Sub_475D90();
            if (source < 100 && Sub_475D40() == 41 && Sub_475D50() == index) {
                const std::int32_t firstPosition = Sub_48BC60(source);
                const std::int32_t firstScroll = std::bit_cast<std::int32_t>(gWindowStates[41].value24);
                if (firstPosition - firstScroll >= 0) {
                    const std::int32_t secondPosition = Sub_48BC60(source);
                    const std::int32_t secondScroll =
                        std::bit_cast<std::int32_t>(gWindowStates[41].value24);
                    if (secondPosition - secondScroll < 3) {
                        const std::int32_t position = Sub_48BC60(source);
                        const std::int32_t currentScroll =
                            std::bit_cast<std::int32_t>(gWindowStates[41].value24);
                        const std::int32_t elementIndex = 14 - currentScroll + position;
                        const WindowInstance* const window = gCurrentWindow;
                        const std::int32_t y = window->y;
                        const std::int32_t x = window->x;
                        const WindowElement& element = window->elements[elementIndex];
                        login::detail::DrawGraphicHover(
                            static_cast<std::uint16_t>(x + element.x + 2),
                            static_cast<std::uint16_t>(y + element.y + 2),
                            static_cast<std::uint16_t>(x + element.width + element.x - 2),
                            static_cast<std::uint16_t>(y + element.height + element.y - 2),
                            223, 254, 0);
                    }
                }
                const std::int32_t level = world::detail::ReadPlayerLevel();
                gStorageValueCC4218 += level;
            }
        }
    };
    drawDragSource();
    if ((events & 0x60u) != 0 && Sub_475D60() != 3) {
        consumed = 1;
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t source = Sub_475D90();
        if (source != petIndex) {
            bool allowed = true;
            if (gWindowStates[41].value10 == 1 && petIndex < 5 && source >= 100) {
                const auto& pet = world::detail::gPetRuntimeF633F8[petIndex];
                const std::int32_t itemSlots = pet.values6D8[0];
                bool empty = true;
                for (std::int32_t slot = 0; slot < itemSlots; ++slot) {
                    if (pet.items6E4[slot].used != 0) {
                        empty = false;
                        break;
                    }
                }
                allowed = pet.values6D8[1] <= 0 && empty;
            }
            if (allowed) {
                Sub_48BB90(41, index, source, petIndex);
            } else {
                std::sprintf(buffer.data(), kStoragePetEquipmentWarning587900);
                world::detail::Sub_44B050(buffer.data(), 4, 3);
            }
        }
        Sub_476500();
    }
    const bool hover = (events & 1u) != 0;
    if (hover && world::detail::gPetRuntimeF633F8[petIndex].used != 0) {
        const std::int32_t level = world::detail::ReadPlayerLevel();
        const std::int32_t amount = world::detail::ReadPlayerValueFC();
        if (amount >= level) {
            std::strcpy(gHoverTextD68AE8.data(),
                "\xA5\xCE\xB7\xC6\xB9\xAB\xA9\xEC\xA9\xF1\xA8\xD3"
                "\xA6\x73\xA9\xF1\xC3\x64\xAA\xAB\xA1\x43");
        }
    }
    const auto& pet = world::detail::gPetRuntimeF633F8[petIndex];
    const std::uint16_t used = pet.used;
    if (used != 0 && (events & 0x21u) != 0
        && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (Sub_475D40() == 41 || Sub_475D40() == 0) {
            static_cast<void>(Sub_4763E0());
            std::int32_t target = 0;
            for (; target < 5; ++target) {
                std::uint16_t targetUsed;
                std::memcpy(&targetUsed, gPetStorageD64E18.data() + target * 0x600, sizeof(targetUsed));
                if (targetUsed == 0) {
                    break;
                }
            }
            if (target < 5) {
                bool allowed = true;
                if (gWindowStates[41].value10 == 1 && petIndex < 5) {
                    const std::int32_t itemSlots = pet.values6D8[0];
                    bool empty = true;
                    for (std::int32_t slot = 0; slot < itemSlots; ++slot) {
                        if (pet.items6E4[slot].used != 0) {
                            empty = false;
                            break;
                        }
                    }
                    allowed = pet.values6D8[1] <= 0 && empty;
                }
                if (allowed) {
                    Sub_48BB90(41, index, petIndex, target + 100);
                } else {
                    std::sprintf(buffer.data(), kStoragePetEquipmentWarning587900);
                    world::detail::Sub_44B050(buffer.data(), 4, 3);
                }
            }
        }
        consumed = 1;
    } else if (hover && (events & 2u) != 0) {
        if (used != 0) {
            static_cast<void>(Sub_48C130(petIndex, 1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    drawDragSource();
    if (hover) {
        const std::uint32_t imageNo = login::detail::gValueD69768;
        Sub_485110(&gCurrentWindow->elements[index], 1, imageNo);
        if (pet.used != 0) {
            gWindowStates[41].actionFlags1C = static_cast<std::uint32_t>(petIndex + 200);
        }
    }
    if ((events & 0x20u) != 0) {
        const std::uint32_t imageNo = login::detail::gValueD69768;
        Sub_485110(&gCurrentWindow->elements[index], 1, imageNo);
        if (Sub_475D60() == 3 && Sub_475D90() >= 100) {
            const std::int32_t level = world::detail::ReadPlayerLevel();
            gStorageValueCC4218 += level;
        }
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement& element = window->elements[index];
    const std::int16_t y = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(element.y + window->y));
    const std::int16_t x = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(element.x + window->x));
    Sub_4AAB70(x, y, petIndex, 3);
    return consumed;
}

std::int32_t Sub_4AB710(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t scroll = std::bit_cast<std::int32_t>(gWindowStates[41].value28);
    const std::int32_t petIndex = scroll + index - 17;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    std::array<char, 512> buffer;
    if ((events & 0x60u) != 0 && Sub_475D60() != 3) {
        consumed = 1;
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t source = Sub_475D90();
        if (source != petIndex + 100) {
            bool allowed = true;
            if (gWindowStates[41].value10 == 1 && source < 5) {
                const auto& sourcePet = world::detail::gPetRuntimeF633F8[source];
                const std::int32_t weight = sourcePet.values6D8[1];
                const std::int32_t itemSlots = sourcePet.values6D8[0];
                const bool hasWeight = weight > 0;
                // 此路径仅在重量与有效装备同时存在时拒绝，原循环固定六槽。
                for (std::int32_t slot = 0; slot < 6; ++slot) {
                    if (slot < itemSlots && sourcePet.items6E4[slot].used != 0) {
                        allowed = !hasWeight;
                        break;
                    }
                }
            }
            if (allowed) {
                Sub_48BB90(41, index, source, petIndex + 100);
            } else {
                std::sprintf(buffer.data() + 128, kStoragePetEquipmentWarning587900);
                world::detail::Sub_44B050(buffer.data() + 128, 4, 3);
            }
        }
        Sub_476500();
    }
    const bool hover = (events & 1u) != 0;
    if (hover) {
        std::uint16_t used;
        std::memcpy(&used, gPetStorageD64E18.data() + petIndex * 0x600, sizeof(used));
        if (used != 0) {
            std::strcpy(gHoverTextD68AE8.data(),
                "\xA5\xCE\xB7\xC6\xB9\xAB\xA9\xEC\xA9\xF1\xA8\xD3"
                "\xB4\xA3\xBB\xE2\xC3\x64\xAA\xAB\xA1\x43");
        }
    }
    const char* const record = gPetStorageD64E18.data() + petIndex * 0x600;
    std::uint16_t used;
    std::memcpy(&used, record, sizeof(used));
    if (used != 0 && (events & 0x21u) != 0
        && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        if (Sub_475D40() == 41 || Sub_475D40() == 0) {
            static_cast<void>(Sub_4763E0());
            std::int32_t position = 0;
            for (; position < 5; ++position) {
                const std::int32_t target = std::bit_cast<std::int16_t>(
                    world::detail::gPetPairsDE0B08[position].index);
                if (world::detail::gPetRuntimeF633F8[target].used == 0) {
                    break;
                }
            }
            if (position < 5) {
                const std::int32_t target = std::bit_cast<std::int16_t>(
                    world::detail::gPetPairsDE0B08[position].index);
                std::sprintf(buffer.data(), "P|%d|%d", petIndex + 100, target);
                input::EscapeLegacyTextAndNotify_52D2D0(buffer.data(), buffer.data() + 256, 255);
                const std::int32_t userState =
                    std::bit_cast<std::int32_t>(gWindowStates[41].userState.value);
                const std::int32_t value14 = std::bit_cast<std::int32_t>(gWindowStates[41].value14);
                const std::int32_t y =
                    platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
                const std::int32_t x =
                    platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
                static_cast<void>(network::Sub_512620(
                    platform::gConnectionSocket, x, y, value14, userState, 0, buffer.data() + 256));
            }
        }
        consumed = 1;
    } else if (hover && (events & 2u) != 0) {
        if (used != 0) {
            static_cast<void>(Sub_48C130(petIndex + 100, 1));
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        consumed = 1;
    }
    if (Sub_475D60() == 3) {
        const std::int32_t source = Sub_475D90();
        if (source < 100) {
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t level = world::detail::ReadPlayerLevel();
            auto* const text = static_cast<WindowTextData*>(window->elements[13].data);
            std::sprintf(text->text, "%10d", level);
        } else {
            const std::int32_t slot = source - 100;
            if (Sub_475D40() == 41 && Sub_475D50() == index) {
                const std::int32_t currentScroll =
                    std::bit_cast<std::int32_t>(gWindowStates[41].value28);
                const std::int32_t row = slot - currentScroll;
                if (row >= 0 && row < 3) {
                    const WindowInstance* const window = gCurrentWindow;
                    const std::int32_t y = window->y;
                    const std::int32_t x = window->x;
                    const WindowElement& element = window->elements[row + 17];
                    login::detail::DrawGraphicHover(
                        static_cast<std::uint16_t>(x + element.x + 2),
                        static_cast<std::uint16_t>(y + element.y + 2),
                        static_cast<std::uint16_t>(x + element.width + element.x - 2),
                        static_cast<std::uint16_t>(y + element.height + element.y - 2),
                        223, 254, 0);
                }
            }
        }
    }
    if (hover) {
        std::memcpy(&used, record, sizeof(used));
        if (used != 0) {
            const std::uint32_t imageNo = login::detail::gValueD69768;
            Sub_485110(&gCurrentWindow->elements[index], 1, imageNo);
            gWindowStates[41].actionFlags1C = static_cast<std::uint32_t>(petIndex + 300);
        }
    }
    if ((events & 0x20u) != 0) {
        const std::uint32_t imageNo = login::detail::gValueD69768;
        Sub_485110(&gCurrentWindow->elements[index], 1, imageNo);
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement& element = window->elements[index];
    const std::int16_t y = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(element.y + window->y));
    const std::int16_t x = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(element.x + window->x));
    Sub_4AB400(x, y, petIndex, 3);
    return consumed;
}

std::int32_t Sub_4ABB20(const std::int32_t index, const std::int32_t state)
{
    // 原未识别kind/side或非30/31元素走未初始化局部路径，不补默认值。
    std::int32_t maximum;
    std::uint8_t result;
    if (index == 30) {
        const std::uint32_t kind = gWindowStates[41].value10;
        if (kind == 0 || kind == 1) {
            const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
            if (side == 0 || side == 1) {
                if (kind == 0) {
                    const std::int32_t limit = world::detail::ReadPlayerValue15BD0(side + 1);
                    std::int32_t amount;
                    std::memcpy(&amount,
                        gItemDetailStorageCC4D58.data() + side * 0xFBF8 + 0xFBF4, sizeof(amount));
                    maximum = limit - amount;
                } else {
                    std::int32_t amount;
                    std::memcpy(&amount,
                        gItemDetailStorageCC4D58.data() + side * 0xFBF8 + 0xFBF4, sizeof(amount));
                    maximum = 1000000 - amount;
                }
            }
        }
        const std::int32_t amount = world::detail::ReadPlayerValueFC();
        if (maximum > amount) {
            maximum = amount;
        }
        const std::uint8_t keyboard =
            input::gCurrentInputData == &input::gInputContextCA0018 ? 2 : 0;
        result = Sub_474550(gCurrentWindow, 30, state, maximum, 223,
            gStorageDepositHints57EDD4.data(), keyboard, 0xFFFFFFFFu);
    } else if (index == 31) {
        const std::int32_t limit = world::detail::ReadPlayerValue15BD0(0);
        maximum = limit - world::detail::ReadPlayerValueFC();
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        std::int32_t amount;
        std::memcpy(&amount,
            gItemDetailStorageCC4D58.data() + side * 0xFBF8 + 0xFBF4, sizeof(amount));
        if (maximum > amount) {
            maximum = amount;
        }
        const std::uint8_t keyboard =
            input::gCurrentInputData == &input::gInputContextCA0018 ? 2 : 0;
        result = Sub_474550(gCurrentWindow, 31, state, maximum, 223,
            gStorageWithdrawHints57EDEC.data(), keyboard, 0xFFFFFFFFu);
    }
    WindowInstance* window;
    if ((result & 1u) != 0) {
        window = gCurrentWindow;
        WindowElement* const element = &window->elements[index];
        if (std::bit_cast<std::int32_t>(element->state) > maximum) {
            element->state = static_cast<std::uint32_t>(maximum);
            window = gCurrentWindow;
        }
    } else {
        window = gCurrentWindow;
    }
    if ((result & 2u) != 0) {
        if (index == 30 || index == 31) {
            std::array<char, 384> buffer;
            const std::int32_t value = std::bit_cast<std::int32_t>(window->elements[index].state);
            std::sprintf(buffer.data(), index == 30 ? "G|%d" : "D|%d", value);
            input::EscapeLegacyTextAndNotify_52D2D0(buffer.data(), buffer.data() + 128, 255);
            const std::int32_t userState =
                std::bit_cast<std::int32_t>(gWindowStates[41].userState.value);
            const std::int32_t value14 = std::bit_cast<std::int32_t>(gWindowStates[41].value14);
            const std::int32_t y =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t x =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, value14, userState, 0, buffer.data() + 128));
            window = gCurrentWindow;
            const std::int32_t confirmed = std::bit_cast<std::int32_t>(window->elements[index].state);
            const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
            gStorageLastAmountsD67B68[side] = confirmed;
        }
        window->elements[index].state = 0;
        window = gCurrentWindow;
    }
    if (index == 30) {
        std::sprintf(gStorageAmountTextsCB1DF8.data(), "%9d",
            std::bit_cast<std::int32_t>(window->elements[30].state));
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        std::sprintf(gStorageAmountTextsCB1DF8.data() + 11, "%9d", gStorageLastAmountsD67B68[side]);
    } else if (index == 31) {
        std::sprintf(gStorageAmountTextsCB1DF8.data() + 11, "%9d",
            std::bit_cast<std::int32_t>(window->elements[31].state));
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        std::sprintf(gStorageAmountTextsCB1DF8.data(), "%9d", gStorageLastAmountsD67B68[side]);
    }
    return 0;
}

std::int32_t Sub_4A9B00(const std::int32_t, const std::int32_t)
{
    std::array<char, 20> buffer;
    std::int32_t count = 0;
    for (std::int32_t index = 8; index < 28; ++index) {
        if (world::detail::gPlayerItemsFDA8A4[index].used != 0) {
            ++count;
        }
    }
    std::sprintf(buffer.data(), "%3d", count);
    WindowInstance* window = gCurrentWindow;
    std::int32_t y = window->y + 285;
    std::int32_t x = window->x + 357;
    Sub_4771B0(x, y, buffer.data(), 3, 0, 1, 223);
    count = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        if (world::detail::gPetRuntimeF633F8[index].used != 0) {
            ++count;
        }
    }
    std::sprintf(buffer.data(), "%3d", count);
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 442;
    Sub_4771B0(x, y, buffer.data(), 3, 0, 1, 223);
    std::sprintf(buffer.data(), "%10d", world::detail::ReadPlayerValueFC());
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 527;
    Sub_4771B0(x, y, buffer.data(), 3, 0, 1, 223);
    count = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        std::uint16_t used;
        std::memcpy(&used, gItemDetailStorageCC4D58.data() + index * 0xC44, sizeof(used));
        if (used != 0) {
            ++count;
        }
    }
    std::sprintf(buffer.data(), "%3d", count);
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 85;
    Sub_4771B0(x, y, buffer.data(), 3, 0, 1, 223);
    count = 0;
    for (std::int32_t index = 0; index < 5; ++index) {
        std::uint16_t used;
        std::memcpy(&used, gPetStorageD64E18.data() + index * 0x600, sizeof(used));
        if (used != 0) {
            ++count;
        }
    }
    std::sprintf(buffer.data(), "%3d", count);
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 170;
    Sub_4771B0(x, y, buffer.data(), 3, 0, 1, 223);
    const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
    std::int32_t amount;
    std::memcpy(&amount,
        gItemDetailStorageCC4D58.data() + side * 0xFBF8 + 0xFBF4, sizeof(amount));
    std::sprintf(buffer.data(), "%10d", amount);
    window = gCurrentWindow;
    y = window->y + 285;
    x = window->x + 255;
    Sub_4771B0(x, y, buffer.data(), 3, 0, 1, 223);
    return 0;
}

std::int32_t Sub_4AC200(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xC3\xF6\xB3\xAC\xB3\x6F\xAD\xD3\xB5\xF8\xB5\xA1\xA1\x43");
    }
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    graphic->imageNo = 243000;
    if ((state & 1) != 0) {
        graphic->imageNo = 243002;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243001;
    }
    return 0;
}

std::int32_t Sub_4A9350(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(), "\xC3\xF6\xB3\xAC\xA6\xB9\xB5\xF8\xB5\xA1");
    }
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    graphic->imageNo = 243048;
    if ((state & 1) != 0) {
        graphic->imageNo = 243050;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 243049;
    }
    return 0;
}

std::int32_t Sub_4AC290(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    render::GraphicDescriptor16 dimmed {};
    const std::uint32_t color = 0xFFFFFFFFu;
    const std::uint32_t dimmedColor = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::memcpy(dimmed.bytes.data() + 8, &dimmedColor, sizeof(dimmedColor));
    dimmed.bytes[13] = 4;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && Sub_475D60() != 2) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t source = Sub_475D90();
        if (source < 100) {
            const WindowInstance* const window = gCurrentWindow;
            const std::int32_t windowX = window->x;
            const std::int32_t windowY = window->y;
            const std::int32_t elementX = window->elements[index].x;
            const std::int32_t elementY = window->elements[index].y;
            for (std::int32_t position = 0; position < 20; ++position) {
                const std::int32_t left = windowX + 50 * (position % 5) + elementX + 12;
                const std::int32_t top = windowY + 52 * (position / 5) + elementY + 39;
                if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                    continue;
                }
                if (source != position + 8) {
                    Sub_4886E0(38, index, source, position + 8);
                }
                break;
            }
        }
        Sub_476500();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 12;
            const std::int32_t top = element.y + 52 * (position / 5) + window->y + 39;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
    }
    if ((events & 1u) != 0 && hovered >= 0) {
        std::strcpy(gHoverTextD68AE8.data(),
            "\xA7\xE2\xA8\xAD\xA4\x57\xAA\xBA\xAA\xAB\xAB\x7E\xA5\xCE\xB7\xC6"
            "\xB9\xAB\xA9\xEC\xA9\xF1\xA8\xEC\xB9\x7D\xAE\xC6\xBD\x63\xA1\x43");
    }
    if (Sub_475D60() != 0 && (events & 0x20u) != 0 && Sub_475D90() >= 100
        && (world::detail::gPlayerItemsFDA8A4[hovered + 8].flags & 0x60u) != 0) {
        hovered = -1;
    }
    if (gStorageOwnHover57EE04 != hovered) {
        gWindowStates[38].value20 = 0;
    }
    gStorageOwnHover57EE04 = hovered;
    if ((events & 1u) != 0 && hovered >= 0) {
        if ((events & 2u) != 0) {
            if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
                static_cast<void>(Sub_48BF10(hovered + 8, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[38].value20);
                const std::int32_t pages = world::detail::gPlayerItemsFDA8A4[hovered + 8].pages;
                ++page;
                gWindowStates[38].value20 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[38].value20 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (Sub_475D60() == 2) {
        const std::int32_t source = Sub_475D90();
        if (source >= 8) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * ((source - 8) % 5) + window->x + 12;
            const std::int32_t top = element.y + 52 * ((source - 8) / 5) + window->y + 39;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 12;
        const std::int32_t top = element.y + 52 * (hovered / 5) + window->y + 39;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        if (world::detail::gPlayerItemsFDA8A4[hovered + 8].used != 0) {
            gWindowStates[38].actionFlags1C = static_cast<std::uint32_t>(hovered + 8);
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    const auto* const occupancy = reinterpret_cast<const unsigned char*>(world::gValueD67B08.data());
    std::array<char, 12> buffer;
    for (std::int32_t position = 0; position < 20; ++position) {
        const auto& item = world::detail::gPlayerItemsFDA8A4[position + 8];
        if (item.used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (position % 5) + window->x + 36;
        const std::int32_t y = element.y + 52 * (position / 5) + window->y + 64;
        if ((item.flags & 0x60u) != 0
            || item.itemType == 38 || item.itemType == 52 || item.itemType == 54) {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, 23040, 0, nullptr));
        }
        if (occupancy[position + 8] != 0) {
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(x - 24), static_cast<std::uint16_t>(y - 24),
                static_cast<std::uint16_t>(x + 24), static_cast<std::uint16_t>(y + 24),
                223, 249, 0);
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &dimmed));
        } else {
            static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
        }
        // 原4AC80F在两分支后均再次绘制普通图，不合并这次入队。
        static_cast<void>(login::detail::DrawGraphic(x, y, 223, item.imageNo, 0, &descriptor));
        const std::int32_t count = item.count;
        if (count > 0) {
            std::sprintf(buffer.data(), "%3d", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                3, 1, 1, buffer.data(), 0, 0));
        }
    }
    return consumed;
}

std::int32_t Sub_4AC890(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t consumed = 0;
    std::uint8_t events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0
        && ((Sub_475D60() != 2 && Sub_475D60() != 5) || Sub_475D90() < 8)) {
        events &= 0x9Fu;
        consumed = 1;
    }
    if ((events & 0x40u) != 0) {
        const std::int32_t dropX = Sub_475D70();
        const std::int32_t dropY = Sub_475D80();
        const std::int32_t source = Sub_475D90();
        const WindowInstance* const window = gCurrentWindow;
        const std::int32_t elementX = window->elements[index].x;
        const std::int32_t windowX = window->x;
        const std::int32_t windowY = window->y;
        const std::int32_t elementY = window->elements[index].y;
        for (std::int32_t position = 0; position < 20; ++position) {
            const std::int32_t left = windowX + 50 * (position % 5) + elementX + 12;
            const std::int32_t top = windowY + 52 * (position / 5) + elementY + 39;
            if (dropX >= left + 48 || left > dropX || dropY >= top + 48 || top > dropY) {
                continue;
            }
            if (source != position + 100
                && (source >= 100 || (world::detail::gPlayerItemsFDA8A4[source].flags & 0x60u) == 0)) {
                // 原4AC9E6始终按玩家表取类型；不为外部索引补造另一来源。
                const std::int32_t itemType = world::detail::gPlayerItemsFDA8A4[source].itemType;
                if (itemType != 38 && itemType != 52) {
                    Sub_4886E0(38, index, source, position + 100);
                }
            }
            break;
        }
        Sub_476500();
    }
    std::int32_t hovered = -1;
    if ((events & 0x21u) != 0) {
        for (std::int32_t position = 0; position < 20; ++position) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * (position % 5) + window->x + 12;
            const std::int32_t top = element.y + 52 * (position / 5) + window->y + 39;
            if (login::detail::Sub_5049A0(left, top, left + 48, top + 48, -1) != 0) {
                hovered = position;
                break;
            }
        }
    }
    if (Sub_475D60() != 0 && (events & 0x20u) != 0) {
        const std::int32_t source = Sub_475D90();
        if (source < 100 && (world::detail::gPlayerItemsFDA8A4[source].flags & 0x60u) != 0) {
            hovered = -1;
        }
    }
    if (gStorageExternalHover57EE08 != hovered) {
        gWindowStates[38].value20 = 0;
    }
    gStorageExternalHover57EE08 = hovered;
    if ((events & 1u) != 0 && hovered >= 0) {
        const char* const record = gItemDetailStorageCC4D58.data() + hovered * 0xC44;
        if ((events & 2u) != 0) {
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                static_cast<void>(Sub_48BF10(hovered + 100, 1));
                static_cast<void>(audio::PlaySe_52C450(51, 320));
            }
            consumed = 1;
        }
        if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
            std::uint16_t used;
            std::memcpy(&used, record, sizeof(used));
            if (used != 0) {
                std::int32_t page = std::bit_cast<std::int32_t>(gWindowStates[38].value20);
                std::int32_t pages;
                std::memcpy(&pages, record + 0xC3C, sizeof(pages));
                ++page;
                gWindowStates[38].value20 = static_cast<std::uint32_t>(page);
                if (page >= pages) {
                    gWindowStates[38].value20 = 0;
                }
            }
            consumed = 1;
        }
    }
    if (Sub_475D60() == 2 || Sub_475D60() == 5) {
        const std::int32_t source = Sub_475D90();
        if (source >= 100) {
            const WindowInstance* const window = gCurrentWindow;
            const WindowElement& element = window->elements[index];
            const std::int32_t left = element.x + 50 * ((source - 100) % 5) + window->x + 12;
            const std::int32_t top = element.y + 52 * ((source - 100) / 5) + window->y + 39;
            login::detail::DrawGraphicHover(
                static_cast<std::uint16_t>(left + 2), static_cast<std::uint16_t>(top + 2),
                static_cast<std::uint16_t>(left + 46), static_cast<std::uint16_t>(top + 46),
                223, 254, 0);
        }
    }
    if (hovered >= 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t left = element.x + 50 * (hovered % 5) + window->x + 12;
        const std::int32_t top = element.y + 52 * (hovered / 5) + window->y + 39;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
            static_cast<std::uint16_t>(left + 48), static_cast<std::uint16_t>(top + 48),
            223, login::detail::gValueD69768, 0);
        std::uint16_t used;
        std::memcpy(&used, gItemDetailStorageCC4D58.data() + hovered * 0xC44, sizeof(used));
        if (used != 0) {
            gWindowStates[38].actionFlags1C = static_cast<std::uint32_t>(hovered + 100);
        }
    }
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    static_cast<void>(render::DrawText_456780(
        0, 0, 3, 1, 0, input::LegacyText593850(), 0, 0));
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    std::array<char, 12> buffer;
    for (std::int32_t position = 0; position < 20; ++position) {
        const char* const record = gItemDetailStorageCC4D58.data() + position * 0xC44;
        std::uint16_t used;
        std::memcpy(&used, record, sizeof(used));
        if (used == 0) {
            continue;
        }
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = element.x + 50 * (position % 5) + window->x + 36;
        const std::int32_t y = element.y + 52 * (position / 5) + window->y + 64;
        std::int32_t imageNo;
        std::memcpy(&imageNo, record + 4, sizeof(imageNo));
        static_cast<void>(login::detail::DrawGraphic(x, y, 223, imageNo, 0, &descriptor));
        std::int32_t count;
        std::memcpy(&count, record + 8, sizeof(count));
        if (count > 0) {
            std::sprintf(buffer.data(), "%3d", count);
            static_cast<void>(render::DrawText_456780(
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - 3)),
                std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y + 7)),
                3, 1, 1, buffer.data(), 0, 0));
        }
    }
    return consumed;
}

void Sub_4E02B0(
    const std::int32_t index, const std::int32_t side, const char* const text)
{
    char* const record = gItemDetailStorageCC4D58.data() + index * 0xC44 + side * 0xFBF8;
    char* const rows = record + 0x29;
    std::int32_t last = -1;
    for (std::int32_t line = 0; line < 32; ++line) {
        char* const row = rows + line * 96;
        if (input::ExtractLegacyLine_503AB0(row, 3072, text, line, 32) != 0) {
            last = line;
        } else {
            row[0] = '\0';
        }
    }
    // 原回扫从最后成功行的前一行开始，空行不清整行残留。
    while (last > 0 && std::strlen(rows + (last - 1) * 96) == 0) {
        --last;
    }
    WriteTradeValue(record, 0xC3C, last < 0 ? std::int32_t{0} : last / 8 + 1);
}

std::int32_t Sub_4A84A0(const char* const text)
{
    char buffer[1024];
    if (network::ExtractDelimitedField_52CED0(text, '|', 1, 1023, buffer) != 0) {
        return 0;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
    if (std::strlen(buffer) != 1) {
        return 0;
    }
    const auto readItem = [&](char* const record, const std::int32_t index,
                              const std::int32_t field, const bool requireDescriptionField) {
        const std::int32_t imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        if (imageNo <= 0) {
            return;
        }
        WriteTradeValue(record, 0, std::uint16_t{1});
        WriteTradeValue(record, 4, imageNo);
        WriteTradeValue(record, 8, network::ParseDelimitedInteger_52CFA0(text, '|', field + 1));
        WriteTradeValue(record, 0xC40, network::ParseDelimitedInteger_52CFA0(text, '|', field + 2));
        if (network::ExtractDelimitedField_52CED0(text, '|', field + 3, 1023, buffer) != 0) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        std::strcpy(record + 0x0C, std::strlen(buffer) > 28 ? "???" : buffer);
        if (network::ExtractDelimitedField_52CED0(text, '|', field + 4, 1023, buffer) != 0) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        if (std::strlen(buffer) != 0) {
            std::strcpy(record + 0xC29, buffer);
        }
        const std::int32_t descriptionStatus =
            network::ExtractDelimitedField_52CED0(text, '|', field + 5, 1023, buffer);
        if (requireDescriptionField && descriptionStatus != 0) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        Sub_4E02B0(index, 0, buffer);
    };
    const auto readPet = [&](char* const record, const std::int32_t field) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field, 1023, buffer) != 0) {
            return;
        }
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        std::strcpy(record + 0x5D4, std::strlen(buffer) > 16 ? "???" : buffer);
        const std::int32_t level = network::ParseDelimitedInteger_52CFA0(text, '|', field + 1);
        WriteTradeValue(record, 8, level);
        if (level < 0) {
            return;
        }
        WriteTradeValue(record, 0, std::uint16_t{1});
        WriteTradeValue(record, 2, static_cast<std::uint16_t>(
            network::ParseDelimitedInteger_52CFA0(text, '|', field + 2)));
        static constexpr std::array<std::size_t, 9> firstOffsets {
            0x10, 0x18, 0x1C, 0x20, 0x24, 0x28, 0x2C, 0x44, 0x58,
        };
        for (std::int32_t value = 0; value < 9; ++value) {
            WriteTradeValue(record, firstOffsets[value],
                network::ParseDelimitedInteger_52CFA0(text, '|', field + 3 + value));
        }
        network::ExtractDelimitedField_52CED0(text, '|', field + 12, 1023, buffer);
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
        std::strcpy(record + 0x5E5, buffer);
        static constexpr std::array<std::size_t, 12> lastOffsets {
            4, 0x30, 0x34, 0x38, 0x3C, 0x40, 0x48, 0x4C, 0x50, 0x54, 0x5F8, 0x5FC,
        };
        for (std::int32_t value = 0; value < 12; ++value) {
            WriteTradeValue(record, lastOffsets[value],
                network::ParseDelimitedInteger_52CFA0(text, '|', field + 13 + value));
        }
        for (std::int32_t skill = 0; skill < 10; ++skill) {
            network::ExtractDelimitedField_52CED0(text, '|', field + 25 + skill, 1023, buffer);
            input::UnescapeLegacyTextAndNotify_52D2F0(buffer);
            std::strcpy(record + 0x62 + skill * 0x8C, std::strlen(buffer) > 24 ? "???" : buffer);
        }
    };
    switch (buffer[0]) {
    case 'A': {
        std::memset(gItemDetailStorageCC4D58.data(), 0, 20 * 0xC44);
        for (std::int32_t index = 0; index < 20; ++index) {
            readItem(gItemDetailStorageCC4D58.data() + index * 0xC44, index, 2 + index * 6, true);
        }
        std::memset(gPetStorageD64E18.data(), 0, gPetStorageD64E18.size());
        for (std::int32_t index = 0; index < 5; ++index) {
            readPet(gPetStorageD64E18.data() + index * 0x600, 122 + index * 35);
        }
        const std::int32_t amount = network::ParseDelimitedInteger_52CFA0(text, '|', 297);
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        WriteTradeValue(gItemDetailStorageCC4D58.data() + side * 0xFBF8, 0xFBF4, amount);
        return 1;
    }
    case 'G': {
        const std::int32_t amount = network::ParseDelimitedInteger_52CFA0(text, '|', 2);
        const std::int32_t side = std::bit_cast<std::int32_t>(gLoginWindowRuntime.valueD67618);
        WriteTradeValue(gItemDetailStorageCC4D58.data() + side * 0xFBF8, 0xFBF4, amount);
        return 0;
    }
    case 'I': {
        std::int32_t field = 2;
        std::int32_t index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        while (index >= 0) {
            char* const record = gItemDetailStorageCC4D58.data() + index * 0xC44;
            std::memset(record, 0, 0xC44);
            readItem(record, index, field + 1, false);
            field += 7;
            index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        }
        return 0;
    }
    case 'P': {
        std::int32_t field = 2;
        std::int32_t index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        while (index >= 0) {
            char* const record = gPetStorageD64E18.data() + index * 0x600;
            std::memset(record, 0, 0x600);
            readPet(record, field + 1);
            field += 36;
            index = network::ParseDelimitedInteger_52CFA0(text, '|', field);
        }
        return 0;
    }
    default:
        return 0;
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

void Sub_472220()
{
    for (WindowState& state : gWindowStates) {
        if (state.window != nullptr) {
            state.marker50 = 1;
        }
    }
}

void Sub_4722A0()
{
    world::gValueD67B08.fill(0);
    gMouseValue57C110 = 0;
    gDragSourceIndex57C114 = 0;
    gDragCallback57C118 = nullptr;
    gDragY57C11E = 0;
    gDragX57C11C = 0;
    gFocusWindowId57C120 = 0;
    gFocusElementIndex57C124 = -1;
    gMousePhase57C128 = 0;
}

void Sub_4722E0()
{
    gFocusedElementDataD45F94 = nullptr;
}

WindowInstance* Sub_4A53D0()
{
    gLoginWindowRuntime.prefixD2F0B8.value26C = 1;
    return OpenWindow_471440(22);
}

void Sub_4722F0()
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
            if (gWindowStates[21].marker50 != 0) {
                static_cast<void>(OpenWindow_471440(21));
                gWindowStates[21].marker50 = 0;
            }
            if (gWindowStates[40].marker50 != 0) {
                const std::int32_t position = world::detail::gValueD046D4;
                static_cast<void>(OpenWindow_471440(40));
                world::detail::gValueD046D4 = position;
                gWindowStates[40].marker50 = 0;
            }
            if (gWindowStates[22].marker50 != 0) {
                static_cast<void>(Sub_4A53D0());
                gWindowStates[22].marker50 = 0;
            }
            if (gWindowStates[23].marker50 != 0) {
                static_cast<void>(OpenWindow_471440(23));
                gWindowStates[23].marker50 = 0;
            }
            break;
        case 33:
        case 34:
            static_cast<void>(OpenWindow_471440(index));
            gWindowStates[index].marker50 = 0;
            break;
        default:
            gWindowStates[index].marker50 = 0;
            break;
        }
    }
}

void Sub_479640()
{
    gValueD67AF8 = 0;
}

void Sub_4796C0()
{
    gValueD67AFC = 0;
}

void Sub_4796F0()
{
    gValueD67AFD = 0;
}

void Sub_479610()
{
    gWindowActionCA0174 = nullptr;
    gWindowActionD2E928 = nullptr;
    Sub_479640();
    Sub_4796C0();
    Sub_4796F0();
    gValueD67AFE = 0;
}

void Sub_4C7AD0()
{
    gValueCF4AAC = 0;
}

namespace {

// 三条重开共用的两轮窄清理；不释放实例，也不清记录的其他字段。
void ResetWindowReferences()
{
    gWindows.fill(nullptr);
    for (WindowState& state : gWindowStates) {
        state.window = nullptr;
    }
    Sub_4722A0();
    gCurrentWindowIdD67A1C = 0;
    gCurrentElementIndex57C12C = -1;
}

void OpenConfiguredMainWindows()
{
    static_cast<void>(OpenWindow_471440(6));
    switch (gValueCF4690) {
    case 0:
        static_cast<void>(OpenWindow_471440(1));
        break;
    case 1:
        static_cast<void>(OpenWindow_471440(2));
        break;
    case 2:
        static_cast<void>(OpenWindow_471440(3));
        break;
    default:
        break;
    }
    switch (gValueCA0440) {
    case 0:
        static_cast<void>(OpenWindow_471440(4));
        break;
    case 1:
        static_cast<void>(OpenWindow_471440(5));
        break;
    default:
        break;
    }
    switch (gChatWindowModeD66E04) {
    case 0:
        static_cast<void>(OpenWindow_471440(35));
        break;
    case 1:
        static_cast<void>(OpenWindow_471440(57));
        break;
    default:
        break;
    }
}

void OpenLoginSceneWindows_4707C0()
{
    ResetWindowReferences();
    OpenConfiguredMainWindows();
    Sub_4722F0();
    Sub_4722E0();
    gSelectedItemCB1DF4 = -1;
    gValueD67B24 = 0;
    Sub_479610();
    gPreviewActionD67B84 = nullptr;
    input::gValueC9A96C = 1;
    static_cast<void>(Sub_4A7B00());
}

void OpenBattleSceneWindows_471170()
{
    ResetWindowReferences();
    Sub_4722E0();
    static_cast<void>(OpenWindow_471440(6));
    static_cast<void>(OpenWindow_471440(2));
    switch (gChatWindowModeD66E04) {
    case 0:
        static_cast<void>(OpenWindow_471440(35));
        break;
    case 1:
        static_cast<void>(OpenWindow_471440(57));
        break;
    default:
        break;
    }
    if (gWindowStates[21].marker50 != 0) {
        static_cast<void>(OpenWindow_471440(21));
        gWindowStates[21].marker50 = 0;
    }
    if (gWindowStates[40].marker50 != 0) {
        const std::int32_t position = world::detail::gValueD046D4;
        static_cast<void>(OpenWindow_471440(40));
        world::detail::gValueD046D4 = position;
        gWindowStates[40].marker50 = 0;
    }
    if (gWindowStates[22].marker50 != 0 || gWindowStates[23].marker50 != 0) {
        static_cast<void>(OpenWindow_471440(22));
        gWindowStates[22].marker50 = 0;
        gWindowStates[23].marker50 = 0;
    }
    Sub_479610();
    WindowInstance* const window22 = gWindowStates[22].window;
    gPreviewActionD67B84 = nullptr;
    input::gValueC9A96C = 1;
    if (window22 == nullptr && gWindowStates[23].window == nullptr) {
        static_cast<void>(Sub_4A7B00());
    }
}

void MarkSceneWindows()
{
    for (const std::int32_t id : std::array<std::int32_t, 4>{21, 40, 22, 23}) {
        if (gWindowStates[id].window != nullptr) {
            gWindowStates[id].marker50 = 1;
        }
    }
}

} // namespace

void Sub_470980()
{
    ResetWindowReferences();
    Sub_4722E0();
    OpenConfiguredMainWindows();
    Sub_4722F0();
    gSelectedItemCB1DF4 = -1;
    gValueD67B24 = 0;
    Sub_479610();
    const std::int32_t inputGate = input::gValue626C14;
    gPreviewActionD67B84 = nullptr;
    if (inputGate == 0) {
        input::gValueC9A96C = 1;
    }
    if (gWindowStates[22].window == nullptr && gWindowStates[23].window == nullptr) {
        static_cast<void>(Sub_4A7B00());
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

std::int32_t Sub_4B5AD0(const std::int32_t value)
{
    if (value == -1) {
        gWindow50InputPrototype580618.input.context->text[0] = '\0';
        WindowInstance* const window = gCurrentWindow;
        static_cast<void>(Sub_476570(&gWindow50InputPrototype580618, window->x, window->y, 0));
        input::SetContext_44B330(gWindow50InputPrototype580618.input.context);
        gFocusedElementDataD45F94 = gCurrentWindow->elements[1].data;
        static_cast<WindowInputData*>(gCurrentWindow->elements[1].data)->context =
            gWindow50InputPrototype580618.input.context;
    }
    Sub_4B62A0();
    if (Sub_4D3040(2, 2) != 0) {
        static_cast<void>(Sub_4B6C20());
    }
    return 1;
}

void Sub_4B5B50()
{
    if (world::detail::gValue581800 == 21) {
        world::detail::gValue581800 = -1;
    }
}

std::int32_t Sub_4B5B70(const std::int32_t index, const std::int32_t state)
{
    if ((state & 1) != 0 && gWindow50InputPrototype580618.input.context->text[0] != '\0') {
        std::strcpy(gHoverTextD68AE8.data(), kWindow50WriteHint587C84);
    }
    if ((state & 2) == 0) {
        return 0;
    }
    WindowInstance* const window = gCurrentWindow;
    if (gFocusedElementDataD45F94 != window->elements[index].data) {
        static_cast<void>(Sub_476570(&gWindow50InputPrototype580618, window->x, window->y, 0));
        input::SetContext_44B330(gWindow50InputPrototype580618.input.context);
        gFocusedElementDataD45F94 = gCurrentWindow->elements[index].data;
        static_cast<WindowInputData*>(gCurrentWindow->elements[index].data)->context =
            gWindow50InputPrototype580618.input.context;
    }
    return 1;
}

std::int32_t Sub_4B5C30(const std::int32_t index, const std::int32_t state)
{
    std::array<char, 512> buffer;
    char* const escaped = buffer.data();
    char* const text = buffer.data() + 256;
    std::int32_t consumed = 0;
    switch (index) {
    case 0:
    case 31: {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        if ((state & 1) != 0) {
            graphic->imageNo = index == 0 ? 243002 : 244547;
            const char* const hint = index == 0
                ? "\xC3\xF6\xB3\xAC\xB3\x6F\xAD\xD3\xB5\xF8\xB5\xA1\xA1\x43"
                : kShopHints57E6C4[2];
            std::strcpy(gHoverTextD68AE8.data(), hint);
            consumed = 1;
        } else {
            graphic->imageNo = index == 0 ? 243001 : 244545;
        }
        if ((state & 2) != 0) {
            static_cast<void>(Sub_4B6C20());
        }
        return consumed;
    }
    case 5:
    case 12:
    case 19:
    case 26: {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        if ((state & 1) != 0) {
            graphic->imageNo = 36818;
            std::strcpy(gHoverTextD68AE8.data(), "\xA7\x52\xB0\xA3\xA6\xB9\xB0\x54\xAE\xA7\xA1\x43");
            consumed = 1;
        } else {
            graphic->imageNo = 36816;
        }
        if ((state & 2) == 0) {
            return consumed;
        }
        std::sprintf(text, "%d", (index - 5) / 7);
        input::EscapeLegacyTextAndNotify_52D2D0(text, escaped, 255);
        const std::int32_t target = world::detail::gDialogTargetD2DE6C;
        const std::int32_t request = world::detail::gDialogRequestCC21E0;
        const std::int32_t y =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
        const std::int32_t x =
            platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
        static_cast<void>(network::Sub_512620(
            platform::gConnectionSocket, x, y, request, target, 64, escaped));
        static_cast<void>(Sub_4B6C20());
        return 1;
    }
    case 30: {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[30].data);
        if ((state & 1) != 0) {
            graphic->imageNo = 244544;
            std::strcpy(gHoverTextD68AE8.data(), kWindow50WriteHint587C84);
            consumed = 1;
        } else {
            graphic->imageNo = 244542;
        }
        if ((state & 2) == 0) {
            return consumed;
        }
        input::InputContext* const context = gWindow50InputPrototype580618.input.context;
        if (context->text[0] != '\0') {
            std::strcpy(text, context->text);
            input::EscapeLegacyTextAndNotify_52D2D0(text, escaped, 256);
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            const std::int32_t y =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t x =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, 1, escaped));
            gWindow50InputPrototype580618.input.context->text[0] = '\0';
            static_cast<void>(Sub_4B6C20());
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        return 1;
    }
    case 32:
    case 33: {
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
        const std::int32_t baseImage = index == 32 ? 243033 : 243030;
        if ((state & 1) != 0) {
            graphic->imageNo = baseImage + 2;
            const char* const hint = index == 32
                ? "\xA4\xC1\xB4\xAB\xA6\xDC\xA4\x55\xA4\x40\xAD\xB6\xB5\xF8\xB5\xA1\xA1\x43"
                : "\xA6\x5E\xA8\xEC\xA4\x57\xA4\x40\xAD\xB6\xB5\xF8\xB5\xA1\xA1\x43";
            std::strcpy(gHoverTextD68AE8.data(), hint);
            consumed = 1;
        } else {
            graphic->imageNo = baseImage;
        }
        if ((state & 2) != 0) {
            const std::int32_t target = world::detail::gDialogTargetD2DE6C;
            const std::int32_t request = world::detail::gDialogRequestCC21E0;
            escaped[0] = '\0';
            const std::int32_t y =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerY);
            const std::int32_t x =
                platform::legacy_crypt_int::Read_401EC0(&world::detail::gPlayerX);
            static_cast<void>(network::Sub_512620(
                platform::gConnectionSocket, x, y, request, target, index == 32 ? 32 : 16, escaped));
            static_cast<void>(Sub_4B6C20());
            consumed = 1;
        }
        if ((state & 8) != 0) {
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data)->imageNo = baseImage + 1;
        }
        return consumed;
    }
    default:
        return 0;
    }
}

void Sub_4B62A0()
{
    std::array<char, 256> buffer;
    for (std::int32_t index = 0; index < 4; ++index) {
        const auto& record = gLoginWindowRuntime.recordsD3FAD4[index];
        const std::int32_t first = 2 + index * 7;
        auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[first].data);
        graphic->imageNo = record.value00;
        auto* const name = static_cast<WindowTextData*>(gCurrentWindow->elements[first + 1].data);
        std::strcpy(name->text, record.texts08[0].data());
        std::int32_t time = record.value04;
        if (time > 0) {
            const platform::LegacyTm53B8C6* const calendar = platform::LocalTime_53B8C6(&time);
            const auto* const bytes = reinterpret_cast<const std::byte*>(calendar);
            std::int32_t minute;
            std::memcpy(&minute, bytes + 4, sizeof(minute));
            std::int32_t hour;
            std::memcpy(&hour, bytes + 8, sizeof(hour));
            std::int32_t day;
            std::memcpy(&day, bytes + 12, sizeof(day));
            std::int32_t month;
            std::memcpy(&month, bytes + 16, sizeof(month));
            std::int32_t year;
            std::memcpy(&year, bytes + 20, sizeof(year));
            std::sprintf(buffer.data(), "%4d/%02d/%02d/%02d:%02d",
                year + 1900, month + 1, day, hour, minute);
            auto* const date = static_cast<WindowTextData*>(gCurrentWindow->elements[first + 2].data);
            std::strcpy(date->text, buffer.data());
        } else {
            gCurrentWindow->elements[first + 2].visible = 0;
        }
        for (std::int32_t line = 1; line < 4; ++line) {
            auto* const text = static_cast<WindowTextData*>(gCurrentWindow->elements[first + 3 + line].data);
            std::strcpy(text->text, record.texts08[line].data());
        }
        gCurrentWindow->elements[first + 3].visible = record.value408 != 0 ? 1 : 0;
    }
    gCurrentWindow->elements[32].visible = (gLoginWindowRuntime.valueD3FAD0 & 0x20u) != 0 ? 1 : 0;
    gCurrentWindow->elements[33].visible = (gLoginWindowRuntime.valueD3FAD0 & 0x10u) != 0 ? 1 : 0;
    WindowInstance* const window = gCurrentWindow;
    if (gFocusedElementDataD45F94 == window->elements[1].data) {
        static_cast<void>(Sub_476570(&gWindow50InputPrototype580618, window->x, window->y, 2));
    }
}

std::int32_t Sub_4B6C20()
{
    WindowInstance* const window = gCurrentWindow;
    world::detail::gValue581800 = -1;
    window->state |= 0x8000u;
    return audio::PlaySe_52C450(57, 320);
}

std::int32_t Sub_4B6970(const std::int32_t flags, char* const text)
{
    gLoginWindowRuntime.valueD3FAD0 = static_cast<std::uint32_t>(flags);
    input::UnescapeLegacyText_52D150(text);
    gWindow50AlternateRecordsD30138.fill(std::byte{0});
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        text, '|', 1, 16, gNpcDialogLinesCC24D4[0].text.data()));
    std::array<char, 1024> buffer;
    char* const fieldText = buffer.data();
    char* const copiedText = buffer.data() + 768;
    std::int32_t field = 2;
    std::int32_t offset = 0;
    do {
        auto& record = gLoginWindowRuntime.recordsD3FAD4[offset / 0x40C];
        const std::int32_t imageNo = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record.value00 = character_select::detail::Sub_51BC50(imageNo);
        record.value04 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        record.value408 = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field++, 767, fieldText));
        input::UnescapeLegacyTextAndNotify_52D2F0(fieldText);
        std::strcpy(record.texts08[0].data(), std::strlen(fieldText) > 16 ? "???" : fieldText);
        static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', field++, 767, fieldText));
        input::UnescapeLegacyTextAndNotify_52D2F0(fieldText);
        std::strcpy(copiedText, fieldText);
        const auto length = static_cast<std::int32_t>(std::strlen(fieldText));
        if (std::strlen(fieldText) > 140) {
            std::strcpy(record.texts08[1].data(), input::LegacyText593850());
            std::strcpy(record.texts08[2].data(), input::LegacyText593850());
            std::strcpy(record.texts08[3].data(), input::LegacyText593850());
        } else {
            std::int32_t position = 0;
            for (std::int32_t line = 1; line < 4; ++line) {
                char* const destination = record.texts08[line].data();
                std::int32_t used = 0;
                for (;;) {
                    const char value = copiedText[position];
                    if (value == '\r') {
                        ++position;
                        break;
                    }
                    ++position;
                    destination[used] = value;
                    if (position > length) {
                        break;
                    }
                    if (++used >= 70) {
                        break;
                    }
                }
                destination[used] = '\0';
                if (input::ClassifyLastAnsiUnit_44B190(destination) == 3) {
                    --position;
                    destination[used - 1] = '\0';
                }
            }
        }
        offset += 0x40C;
    } while (offset < 0x1030);
    return offset;
}

void Sub_4B6C50(
    const std::int32_t message, const std::int32_t unused, const std::int32_t sourceIndex)
{
    static_cast<void>(unused);
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x80FFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    if (message != 0) {
        if (message == 2 || message == 4) {
            WindowInstance* const target = gWindowStates[53].window;
            WindowInstance* const previous = gCurrentWindow;
            gCurrentWindow = target;
            Sub_4BC150();
            gCurrentWindow = previous;
        }
        return;
    }
    std::int32_t item = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[sourceIndex]);
    if (gShopItemsCF4FE0[item * 0xC58] == 0) {
        static_cast<void>(Sub_4763E0());
    }
    if ((platform::gLegacyMouseState.edgeButtons & 1u) != 0) {
        static_cast<void>(Sub_4763E0());
        const auto x = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX)));
        const auto y = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
            platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY)));
        static_cast<void>(Sub_476490(9, sourceIndex, x, y, &Sub_4B6C50));
    }
    if ((platform::gLegacyMouseState.edgeButtons & 2u) != 0) {
        CancelWindowDrag_476410();
    }
    static_cast<void>(platform::RequestCursorState_506DE0(1, 0));
    item = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[sourceIndex]);
    std::int32_t imageNo;
    std::memcpy(&imageNo, gShopItemsCF4FE0.data() + item * 0xC58 + 0xC20, sizeof(imageNo));
    const std::int32_t y =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseY);
    const std::int32_t x =
        platform::legacy_crypt_int::Read_401EC0(&platform::gLegacyMouseState.mouseX);
    static_cast<void>(login::detail::DrawGraphic(x, y, 230, imageNo, 0, &descriptor));
}

std::int32_t Sub_4B6DA0(const std::int32_t sourceIndex)
{
    return Sub_475DA0(9, sourceIndex, &Sub_4B6C50);
}

void Sub_4BC150()
{
    if (gShopDragFlagD67B90 != 0) {
        gShopDragFlagD67B90 = 0;
        return;
    }
    std::int32_t source = Sub_475D90();
    std::int32_t item = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[source]);
    WriteTradeValue(gShopItemsCF4FE0.data() + item * 0xC58, 0xC28, std::int32_t{0});
    source = Sub_475D90();
    item = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[source]);
    const auto selectedMask = static_cast<std::uint32_t>(gShopValueCF467C);
    std::int32_t inventoryIndex;
    std::memcpy(&inventoryIndex, gShopItemsCF4FE0.data() + item * 0xC58 + 0xC2C, sizeof(inventoryIndex));
    gShopValueCF467C = std::bit_cast<std::int32_t>(selectedMask & ~(1u << (inventoryIndex - 8)));
    source = Sub_475D90();
    std::int32_t position = gShopScrollCC488C;
    gShopSelectedSlotsCB03DC[source] = static_cast<char>(-1);
    const std::int32_t maximum = gShopItemCountCB1E10 - 5;
    if (position > maximum) {
        position = maximum;
        gShopScrollCC488C = position;
    }
    if (position < 0) {
        position = 0;
        gShopScrollCC488C = 0;
    }
    static_cast<void>(Sub_4742C0(gCurrentWindow->elements + 11, maximum, position));
    Sub_4B8780();
    Sub_4B8B10();
    Sub_4B8BE0();
    Sub_4B8C50();
    gShopMessage580848 = -1;
}

void Sub_4B6DC0()
{
    if (gShopDialogTypeCC3EC8 == world::detail::gValue581800) {
        world::detail::gValue581800 = -1;
    }
    if (std::bit_cast<std::int8_t>(gMaterialOccupancyCC4894[20]) > 0) {
        auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
        for (std::int32_t slot = 0; slot < 28; ++slot) {
            --occupancy[slot];
        }
    }
}

login::YobiAction* Sub_4B6E00(const char* const text)
{
    std::array<char, 768> buffer;
    gMaterialOccupancyCC4894[20] = 0;
    gShopImageD2DAD0 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            char* const line = gNpcDialogLinesCC24D4[1 + group * 4 + row].text.data();
            static_cast<void>(input::ExtractLegacyLine_503AB0(line, 81, buffer.data(), row, 20));
        }
    }
    gShopItemCountCB1E10 = 0;
    gShopItemsCF4FE0.fill(0);
    std::int32_t field = 6;
    for (std::int32_t index = 0; index < 20; ++index) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 767, buffer.data()) == 1) {
            break;
        }
        char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
        record[0] = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::strcpy(record + 1, std::strlen(buffer.data()) > 28 ? "???" : buffer.data());
        WriteTradeValue(record, 0xC20, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC24, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 32; ++row) {
            char* const line = record + 0x1E + row * 96;
            if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
                line[0] = '\0';
            }
        }
        record[0xC3C] = static_cast<char>(
            network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC34, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        ++gShopItemCountCB1E10;
    }
    const std::int32_t dialogType = world::detail::gValue581800;
    gValueD2D608 = -1;
    gShopDialogTypeCC3EC8 = dialogType;
    return CreateWindowAnimation_475AA0(53, 1, 0, nullptr);
}

login::YobiAction* Sub_4B70D0(const char* const text)
{
    std::array<char, 768> buffer;
    gMaterialOccupancyCC4894[20] = 1;
    gShopImageD2DAD0 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 3, 767, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 4; ++row) {
        char* const line = gNpcDialogLinesCC24D4[row + 1].text.data();
        static_cast<void>(input::ExtractLegacyLine_503AB0(line, 81, buffer.data(), row, 20));
    }
    std::strcpy(gNpcDialogLinesCC24D4[5].text.data(), "\xA1\x40\xB5\x4C\xAA\x6B\xA6\x41\xAB\xF9\xA6\xB3");
    std::strcpy(gNpcDialogLinesCC24D4[6].text.data(), "\xA1\x40\xA7\xF3\xB0\xAA\xAA\xBA\xAA\xF7\xC3\x42\xA4\x46");
    std::strcpy(gNpcDialogLinesCC24D4[7].text.data(), input::LegacyText593850());
    gShopReplyItemCountCF4B58 = 0;
    std::strcpy(gNpcDialogLinesCC24D4[8].text.data(), input::LegacyText593850());
    gShopItemsCF4FE0.fill(0);
    std::int32_t field = 4;
    for (std::int32_t index = 0; index < 20; ++index) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 767, buffer.data()) == 1) {
            break;
        }
        char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
        record[0] = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::strcpy(record + 1, std::strlen(buffer.data()) > 28 ? "???" : buffer.data());
        WriteTradeValue(record, 0xC30, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC20, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC24, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC2C, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC40, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        if (std::strlen(buffer.data()) != 0) {
            std::strcpy(record + 0xC44, buffer.data());
        }
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 20; ++row) {
            char* const line = record + 0x1E + row * 96;
            if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
                line[0] = '\0';
            }
        }
        const auto value = static_cast<std::uint8_t>(
            network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        record[0xC3C] = std::bit_cast<char>(value);
        WriteTradeValue(record, 0xC34, static_cast<std::int32_t>(std::bit_cast<std::int8_t>(value)));
        WriteTradeValue(record, 0xC38, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        ++gShopReplyItemCountCF4B58;
    }
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        std::int32_t index = 0;
        for (; index < 20; ++index) {
            const char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
            if (record[0] != 0) {
                std::int32_t inventoryIndex;
                std::memcpy(&inventoryIndex, record + 0xC2C, sizeof(inventoryIndex));
                if (inventoryIndex == slot + 8) {
                    break;
                }
            }
        }
        gShopInventoryMapD2DAB8[slot] = -1;
        if (index < 20) {
            gShopInventoryMapD2DAB8[slot] = static_cast<std::int8_t>(index);
        }
    }
    gShopValueCF467C = 0;
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 0, std::int32_t{-1});
    gShopItemCountCB1E10 = 20;
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 4, std::int32_t{-1});
    gValueD2D608 = -1;
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 8, std::int32_t{-1});
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 12, std::int32_t{-1});
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 16, std::int32_t{-1});
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    for (std::int32_t slot = 0; slot < 28; ++slot) {
        ++occupancy[slot];
    }
    gShopDialogTypeCC3EC8 = world::detail::gValue581800;
    return CreateWindowAnimation_475AA0(53, 1, 0, nullptr);
}

login::YobiAction* Sub_4B74E0(const char* const text)
{
    std::array<char, 768> buffer;
    gMaterialOccupancyCC4894[20] = 2;
    gShopImageD2DAD0 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    for (std::int32_t group = 0; group < 3; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 3, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            char* const line = gNpcDialogLinesCC24D4[1 + group * 4 + row].text.data();
            static_cast<void>(input::ExtractLegacyLine_503AB0(line, 81, buffer.data(), row, 20));
        }
    }
    gValueD2D608 = network::ParseDelimitedInteger_52CFA0(text, '|', 6);
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        text, '|', 7, 28, gShopTextCB0184.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(gShopTextCB0184.data());
    gShopItemsCF4FE0.fill(0);
    gShopItemCountCB1E10 = 0;
    std::int32_t field = 8;
    for (std::int32_t index = 0; index < 20; ++index) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 767, buffer.data()) == 1) {
            break;
        }
        char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
        record[0] = 1;
        record[0xC3C] = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::strcpy(record + 1, std::strlen(buffer.data()) > 28 ? "???" : buffer.data());
        WriteTradeValue(record, 0xC20, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC24, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        const std::int32_t maximum = network::ParseDelimitedInteger_52CFA0(text, '|', field++);
        WriteTradeValue(record, 0xC34, maximum);
        if (maximum <= 0) {
            WriteTradeValue(record, 0xC34, std::int32_t{1});
        }
        WriteTradeValue(record, 0xC30, std::int32_t{0});
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 32; ++row) {
            char* const line = record + 0x1E + row * 96;
            if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
                line[0] = '\0';
            }
        }
        ++gShopItemCountCB1E10;
    }
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    for (std::int32_t slot = 0; slot < 28; ++slot) {
        ++occupancy[slot];
    }
    gShopDialogTypeCC3EC8 = world::detail::gValue581800;
    return CreateWindowAnimation_475AA0(53, 1, 0, nullptr);
}

login::YobiAction* Sub_4B77E0(const char* const text)
{
    std::array<char, 768> buffer;
    gMaterialOccupancyCC4894[20] = 3;
    gShopDetailCC3210.fill(0);
    gShopImageD2DAD0 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    gShopValueCAFEA4 = network::ParseDelimitedInteger_52CFA0(text, '|', 3);
    for (std::int32_t group = 0; group < 4; ++group) {
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', group + 4, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 4; ++row) {
            char* const line = gNpcDialogLinesCC24D4[1 + group * 4 + row].text.data();
            static_cast<void>(input::ExtractLegacyLine_503AB0(line, 81, buffer.data(), row, 20));
        }
    }
    char* const detail = gShopDetailCC3210.data();
    WriteTradeValue(detail, 0xC2C, network::ParseDelimitedInteger_52CFA0(text, '|', 8));
    WriteTradeValue(detail, 0xC28, network::ParseDelimitedInteger_52CFA0(text, '|', 9));
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 10, 768, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    std::strncpy(detail, std::strlen(buffer.data()) > 28 ? "???" : buffer.data(), 29);
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 11, 767, buffer.data()));
    input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
    for (std::int32_t row = 0; row < 32; ++row) {
        char* const line = detail + 0x1D + row * 96;
        if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
            line[0] = '\0';
        }
    }
    WriteTradeValue(detail, 0xC24, network::ParseDelimitedInteger_52CFA0(text, '|', 12));
    WriteTradeValue(detail, 0xC20, network::ParseDelimitedInteger_52CFA0(text, '|', 13));
    WriteTradeValue(detail, 0xC30, network::ParseDelimitedInteger_52CFA0(text, '|', 14));
    WriteTradeValue(detail, 0xC34, network::ParseDelimitedInteger_52CFA0(text, '|', 15));
    std::int32_t value;
    std::memcpy(&value, detail + 0xC28, sizeof(value));
    gShopValueD046C4 = 0;
    gValueD2D608 = value;
    gShopValueD672D4 = 0;
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    for (std::int32_t slot = 0; slot < 28; ++slot) {
        ++occupancy[slot];
    }
    gShopDialogTypeCC3EC8 = world::detail::gValue581800;
    return CreateWindowAnimation_475AA0(53, 1, 0, nullptr);
}

namespace {

// 4B7AB0/4B7EE0同一七字段流程；只区分模式字节和第二行原字节提示。
login::YobiAction* CreateShopServiceWindow(
    const char* const text, const char mode, const char* const prompt)
{
    std::array<char, 768> buffer;
    gMaterialOccupancyCC4894[20] = mode;
    gShopImageD2DAD0 = network::ParseDelimitedInteger_52CFA0(text, '|', 1);
    char* const title = gNpcDialogLinesCC24D4[0].text.data();
    static_cast<void>(network::ExtractDelimitedField_52CED0(text, '|', 2, 16, title));
    input::UnescapeLegacyTextAndNotify_52D2F0(title);
    std::strcpy(gNpcDialogLinesCC24D4[1].text.data(), "\xC5\x77\xAA\xEF\xA5\xFA\xC1\x7B\xA1\x43");
    std::strcpy(gNpcDialogLinesCC24D4[2].text.data(), prompt);
    std::strcpy(gNpcDialogLinesCC24D4[3].text.data(), input::LegacyText593850());
    std::strcpy(gNpcDialogLinesCC24D4[4].text.data(), input::LegacyText593850());
    std::strcpy(gNpcDialogLinesCC24D4[5].text.data(), "\xA7\x41\xA8\xAD\xA4\x57\xAA\xBA\xBF\xFA\xA4\xA3\xB0\xF7\xA1\x49");
    std::strcpy(gNpcDialogLinesCC24D4[6].text.data(), input::LegacyText593850());
    std::strcpy(gNpcDialogLinesCC24D4[7].text.data(), input::LegacyText593850());
    gShopReplyItemCountCF4B58 = 0;
    std::strcpy(gNpcDialogLinesCC24D4[8].text.data(), input::LegacyText593850());
    gShopItemsCF4FE0.fill(0);
    std::int32_t field = 3;
    for (std::int32_t index = 0; index < 20; ++index) {
        if (network::ExtractDelimitedField_52CED0(text, '|', field++, 767, buffer.data()) == 1) {
            break;
        }
        char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
        record[0] = 1;
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        std::strcpy(record + 1, std::strlen(buffer.data()) > 28 ? "???" : buffer.data());
        WriteTradeValue(record, 0xC20, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC24, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC2C, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC40, network::ParseDelimitedInteger_52CFA0(text, '|', field++));
        WriteTradeValue(record, 0xC34, std::int32_t{1});
        record[0xC3C] = 1;
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        if (std::strlen(buffer.data()) != 0) {
            std::strcpy(record + 0xC44, buffer.data());
        }
        static_cast<void>(network::ExtractDelimitedField_52CED0(
            text, '|', field++, 767, buffer.data()));
        input::UnescapeLegacyTextAndNotify_52D2F0(buffer.data());
        for (std::int32_t row = 0; row < 32; ++row) {
            char* const line = record + 0x1E + row * 96;
            if (input::ExtractLegacyLine_503AB0(line, 3072, buffer.data(), row, 32) == 0) {
                line[0] = '\0';
            }
        }
        ++gShopReplyItemCountCF4B58;
    }
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        std::int32_t index = 0;
        for (; index < 20; ++index) {
            const char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
            if (record[0] != 0) {
                std::int32_t inventoryIndex;
                std::memcpy(&inventoryIndex, record + 0xC2C, sizeof(inventoryIndex));
                if (inventoryIndex == slot + 8) {
                    break;
                }
            }
        }
        const std::uint16_t used = world::detail::gPlayerItemsFDA8A4[slot + 8].used;
        gShopInventoryMapD2DAB8[slot] = -1;
        if (used != 0) {
            gShopInventoryMapD2DAB8[slot] = 20;
        }
        if (index < 20) {
            gShopInventoryMapD2DAB8[slot] = static_cast<std::int8_t>(index);
        }
    }
    std::int32_t value;
    std::memcpy(&value, gShopDetailCC3210.data() + 0xC28, sizeof(value));
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 0, std::int32_t{-1});
    gShopValueCF467C = 0;
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 4, std::int32_t{-1});
    gShopItemCountCB1E10 = 20;
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 8, std::int32_t{-1});
    gValueD2D608 = value;
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 12, std::int32_t{-1});
    WriteTradeValue(gShopSelectedSlotsCB03DC.data(), 16, std::int32_t{-1});
    auto* const occupancy = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
    for (std::int32_t slot = 0; slot < 28; ++slot) {
        ++occupancy[slot];
    }
    gShopDialogTypeCC3EC8 = world::detail::gValue581800;
    return CreateWindowAnimation_475AA0(53, 1, 0, nullptr);
}

} // namespace

login::YobiAction* Sub_4B7AB0(const char* const text)
{
    return CreateShopServiceWindow(
        text, 4, "\xA7\x41\xB7\x51\xC5\xB2\xA9\x77\xA4\xB0\xBB\xF2\xAA\xAB\xAB\x7E\xA9\x4F\xA1\x48");
}

login::YobiAction* Sub_4B7EE0(const char* const text)
{
    return CreateShopServiceWindow(
        text, 5, "\xA7\x41\xB7\x51\xAD\xD7\xB2\x7A\xA4\xB0\xBB\xF2\xAA\xAB\xAB\x7E\xA9\x4F\xA1\x48");
}

void Sub_4B8310()
{
    std::array<char, 100> buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        char* const priceText = gShopPriceTextsD673EC.data() + row * 11;
        priceText[0] = '\0';
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text[0] = '\0';
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->text[0] = '\0';
        gCurrentWindow->elements[row + 27].visible = 0;
        const char* record = gShopItemsCF4FE0.data() + (gShopScrollCC488C + row) * 0xC58;
        if (record[0] == 0) {
            continue;
        }
        std::int32_t price;
        std::memcpy(&price, record + 0xC24, sizeof(price));
        std::sprintf(priceText, "%10d", price);
        record = gShopItemsCF4FE0.data() + (gShopScrollCC488C + row) * 0xC58;
        std::uint32_t flags;
        std::memcpy(&flags, record + 0xC40, sizeof(flags));
        if ((flags & 0x100u) != 0) {
            std::strcpy(buffer.data(), record + 0xC44);
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->bytes00[0] =
                std::byte{5};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->bytes00[0] =
                std::byte{5};
        } else {
            std::int32_t count;
            std::memcpy(&count, record + 0xC30, sizeof(count));
            if (count <= 1) {
                std::strcpy(buffer.data(), record + 1);
            } else {
                static_cast<void>(_snprintf(buffer.data(), 100, "%s(%d)", record + 1, count));
            }
            record = gShopItemsCF4FE0.data() + (gShopScrollCC488C + row) * 0xC58;
            std::memcpy(&flags, record + 0xC40, sizeof(flags));
            const std::byte color = (flags & 0x200u) != 0 ? std::byte{1} : std::byte{0};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->bytes00[0] = color;
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->bytes00[0] = color;
        }
        if (std::strlen(buffer.data()) <= 20) {
            std::strcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text,
                buffer.data());
        } else {
            const std::size_t prefix = input::ClassifyLegacyBoundary_44B440(buffer.data(), 20) == 2
                ? 19 : 20;
            std::memcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text,
                buffer.data(), prefix);
            // 原复制量与NUL偏移相差一字节，中间字节不另补零。
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text[prefix + 1] = '\0';
            std::strcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->text,
                buffer.data() + prefix);
        }
        record = gShopItemsCF4FE0.data() + (gShopScrollCC488C + row) * 0xC58;
        if (record[0xC3C] != 0) {
            static_cast<WindowNumberData*>(gCurrentWindow->elements[row + 32].data)->color = 0;
        } else {
            static_cast<WindowNumberData*>(gCurrentWindow->elements[row + 32].data)->color = 4;
            gCurrentWindow->elements[row + 27].visible = 1;
            std::strcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 27].data)->text,
                kShopUnavailableHint586BF4);
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 27].data)->bytes00[0] =
                std::byte{4};
        }
    }
}

void Sub_4B8670()
{
    for (std::int32_t row = 0; row < 5; ++row) {
        const std::int32_t scroll = gShopScrollCC488C;
        char* const text = gShopCountTextsD2D988.data() + row * 11;
        text[0] = '\0';
        const char* const record = gShopItemsCF4FE0.data() + (scroll + row) * 0xC58;
        if (record[0] != 0) {
            std::int32_t selected;
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            std::sprintf(text, "%2d", selected);
        }
    }
}

void Sub_4B86C0()
{
    std::int32_t total = 0;
    gShopTotalD2DAFC = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
        if (record[0] != 0) {
            std::int32_t selected;
            std::int32_t price;
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            std::memcpy(&price, record + 0xC24, sizeof(price));
            total += price * selected;
        }
    }
    gShopTotalD2DAFC = total;
    std::sprintf(gShopTotalTextCC4B2C.data(), "%10d", total);
    const std::int32_t currentTotal = gShopTotalD2DAFC;
    const std::int32_t money = world::detail::ReadPlayerValueFC();
    static_cast<WindowNumberData*>(gCurrentWindow->elements[7].data)->color =
        currentTotal <= money ? 0 : 6;
}

void Sub_4B8730()
{
    std::int32_t total = 0;
    gShopValueC8AF74 = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
        if (record[0] != 0) {
            std::int32_t divisor;
            std::int32_t selected;
            std::memcpy(&divisor, record + 0xC34, sizeof(divisor));
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            if (divisor <= 0) {
                divisor = 2;
            }
            total += (divisor + selected - 1) / divisor;
        }
    }
    gShopValueC8AF74 = total;
}

void Sub_4B8780()
{
    std::array<char, 100> buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        char* const priceText = gShopPriceTextsD673EC.data() + row * 11;
        priceText[0] = '\0';
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text[0] = '\0';
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->text[0] = '\0';
        gCurrentWindow->elements[row + 27].visible = 0;
        std::int32_t index = std::bit_cast<std::int8_t>(
            gShopSelectedSlotsCB03DC[gShopScrollCC488C + row]);
        if (index < 0) {
            continue;
        }
        const char* record = gShopItemsCF4FE0.data() + index * 0xC58;
        std::int32_t price;
        std::memcpy(&price, record + 0xC24, sizeof(price));
        std::sprintf(priceText, "%10d", price);
        index = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[gShopScrollCC488C + row]);
        record = gShopItemsCF4FE0.data() + index * 0xC58;
        std::uint32_t flags;
        std::memcpy(&flags, record + 0xC40, sizeof(flags));
        if ((flags & 0x100u) != 0) {
            std::strcpy(buffer.data(), record + 0xC44);
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->bytes00[0] =
                std::byte{5};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->bytes00[0] =
                std::byte{5};
        } else {
            std::int32_t count;
            std::memcpy(&count, record + 0xC30, sizeof(count));
            if (count <= 1) {
                std::strcpy(buffer.data(), record + 1);
            } else {
                static_cast<void>(_snprintf(buffer.data(), 100, "%s(%d)", record + 1, count));
            }
            index = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[gShopScrollCC488C + row]);
            record = gShopItemsCF4FE0.data() + index * 0xC58;
            std::memcpy(&flags, record + 0xC40, sizeof(flags));
            const std::byte color = (flags & 0x200u) != 0 ? std::byte{1} : std::byte{0};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->bytes00[0] = color;
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->bytes00[0] = color;
        }
        if (std::strlen(buffer.data()) <= 20) {
            std::strcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text,
                buffer.data());
        } else {
            const std::size_t prefix = input::ClassifyLegacyBoundary_44B440(buffer.data(), 20) == 2
                ? 19 : 20;
            std::memcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text,
                buffer.data(), prefix);
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text[prefix + 1] = '\0';
            std::strcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->text,
                buffer.data() + prefix);
        }
        index = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[gShopScrollCC488C + row]);
        record = gShopItemsCF4FE0.data() + index * 0xC58;
        if (record[0xC3C] != 0) {
            std::int32_t selected;
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            static_cast<WindowNumberData*>(gCurrentWindow->elements[row + 32].data)->color =
                selected != 0 ? 0 : 7;
        } else {
            static_cast<WindowNumberData*>(gCurrentWindow->elements[row + 32].data)->color = 6;
            gCurrentWindow->elements[row + 27].visible = 1;
            std::strcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 27].data)->text,
                "\xA1\xAF\xB5\x4C\xAA\x6B\xB3\x63\xBD\xE6");
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 27].data)->bytes00[0] =
                std::byte{6};
        }
    }
}

void Sub_4B8B10()
{
    for (std::int32_t row = 0; row < 5; ++row) {
        char* const text = gShopCountTextsD2D988.data() + row * 11;
        text[0] = '\0';
        gCurrentWindow->elements[row + 42].visible = 0;
        gCurrentWindow->elements[row + 47].visible = 0;
        std::int32_t index = std::bit_cast<std::int8_t>(
            gShopSelectedSlotsCB03DC[gShopScrollCC488C + row]);
        if (index >= 0) {
            const char* record = gShopItemsCF4FE0.data() + index * 0xC58;
            std::int32_t selected;
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            std::sprintf(text, "%2d", selected);
            index = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[gShopScrollCC488C + row]);
            record = gShopItemsCF4FE0.data() + index * 0xC58;
            std::int32_t maximum;
            std::memcpy(&maximum, record + 0xC34, sizeof(maximum));
            if (maximum > 1) {
                gCurrentWindow->elements[row + 42].visible = 1;
                gCurrentWindow->elements[row + 47].visible = 1;
            }
        }
    }
}

void Sub_4B8BE0()
{
    std::int32_t total = 0;
    gShopTotalD2DAFC = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        const std::int32_t index = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[slot]);
        if (index >= 0) {
            const char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
            std::int32_t price;
            std::int32_t selected;
            std::memcpy(&price, record + 0xC24, sizeof(price));
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            total += price * selected;
        }
    }
    gShopTotalD2DAFC = total;
    std::sprintf(gShopTotalTextCC4B2C.data(), "%10d", total);
    static_cast<WindowNumberData*>(gCurrentWindow->elements[7].data)->color = 0;
}

void Sub_4B8C50()
{
    std::int32_t total = 0;
    gShopValueC8AF74 = 0;
    for (std::int32_t slot = 0; slot < 20; ++slot) {
        const std::int32_t index = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[slot]);
        if (index >= 0) {
            const char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
            std::int32_t selected;
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            total += selected;
        }
    }
    gShopValueC8AF74 = total;
}

void Sub_4B8C90()
{
    std::array<char, 100> buffer;
    for (std::int32_t row = 0; row < 5; ++row) {
        char* const priceText = gShopPriceTextsD673EC.data() + row * 11;
        priceText[0] = '\0';
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text[0] = '\0';
        static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->text[0] = '\0';
        gCurrentWindow->elements[row + 27].visible = 0;
        const char* record = gShopItemsCF4FE0.data() + (gShopScrollCC488C + row) * 0xC58;
        if (record[0] == 0) {
            continue;
        }
        std::int32_t price;
        std::memcpy(&price, record + 0xC24, sizeof(price));
        std::sprintf(priceText, "%10d", price);
        record = gShopItemsCF4FE0.data() + (gShopScrollCC488C + row) * 0xC58;
        std::uint32_t flags;
        std::memcpy(&flags, record + 0xC40, sizeof(flags));
        if ((flags & 0x100u) != 0) {
            std::strcpy(buffer.data(), record + 0xC44);
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->bytes00[0] =
                std::byte{5};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->bytes00[0] =
                std::byte{5};
        } else {
            std::int32_t count;
            std::memcpy(&count, record + 0xC30, sizeof(count));
            if (count <= 1) {
                std::strcpy(buffer.data(), record + 1);
            } else {
                static_cast<void>(_snprintf(buffer.data(), 100, "%s(%d)", record + 1, count));
            }
            record = gShopItemsCF4FE0.data() + (gShopScrollCC488C + row) * 0xC58;
            std::memcpy(&flags, record + 0xC40, sizeof(flags));
            const std::byte color = (flags & 0x200u) != 0 ? std::byte{1} : std::byte{0};
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->bytes00[0] = color;
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->bytes00[0] = color;
        }
        if (std::strlen(buffer.data()) <= 20) {
            std::strcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text,
                buffer.data());
        } else {
            const std::size_t prefix = input::ClassifyLegacyBoundary_44B440(buffer.data(), 20) == 2
                ? 19 : 20;
            std::memcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text,
                buffer.data(), prefix);
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 17].data)->text[prefix + 1] = '\0';
            std::strcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 22].data)->text,
                buffer.data() + prefix);
        }
        record = gShopItemsCF4FE0.data() + (gShopScrollCC488C + row) * 0xC58;
        const std::int32_t available = gShopAvailableCF4A24;
        std::memcpy(&price, record + 0xC24, sizeof(price));
        const std::int32_t total = gShopTotalD2DAFC;
        if (available >= price + total) {
            static_cast<WindowNumberData*>(gCurrentWindow->elements[row + 32].data)->color = 0;
        } else {
            static_cast<WindowNumberData*>(gCurrentWindow->elements[row + 32].data)->color = 7;
            gCurrentWindow->elements[row + 27].visible = 1;
            std::strcpy(
                static_cast<WindowTextData*>(gCurrentWindow->elements[row + 27].data)->text,
                "\xA1\xAF\xA7\x41\xAA\xBA\xBC\xC6\xB6\x71\xA4\xA3\xB0\xF7\xA5\x5B\xA4\x75");
            static_cast<WindowTextData*>(gCurrentWindow->elements[row + 27].data)->bytes00[0] =
                std::byte{4};
        }
    }
}

void Sub_4B8FF0()
{
    for (std::int32_t row = 0; row < 5; ++row) {
        char* const text = gShopCountTextsD2D988.data() + row * 11;
        text[0] = '\0';
        gCurrentWindow->elements[row + 42].visible = 0;
        gCurrentWindow->elements[row + 47].visible = 0;
        const char* const record = gShopItemsCF4FE0.data() + (gShopScrollCC488C + row) * 0xC58;
        if (record[0] != 0) {
            std::int32_t selected;
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            std::sprintf(text, "%2d", selected);
            gCurrentWindow->elements[row + 42].visible = 1;
            gCurrentWindow->elements[row + 47].visible = 1;
        }
    }
}

void Sub_4B9090()
{
    std::int32_t total = 0;
    gShopTotalD2DAFC = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
        if (record[0] != 0) {
            std::int32_t selected;
            std::int32_t price;
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            std::memcpy(&price, record + 0xC24, sizeof(price));
            total += price * selected;
        }
    }
    gShopTotalD2DAFC = total;
    std::sprintf(gShopValueTextCF4A28.data(), "%10d", total);
}

void Sub_4B90E0()
{
    std::int32_t total = 0;
    gShopValueC8AF74 = 0;
    for (std::int32_t index = 0; index < 20; ++index) {
        const char* const record = gShopItemsCF4FE0.data() + index * 0xC58;
        if (record[0] != 0) {
            std::int32_t selected;
            std::memcpy(&selected, record + 0xC28, sizeof(selected));
            total += selected;
        }
    }
    gShopValueC8AF74 = total;
}

void Sub_4B9110()
{
    std::int32_t price;
    std::memcpy(&price, gShopDetailCC3210.data() + 0xC34, sizeof(price));
    std::sprintf(gShopPriceTextsD673EC.data(), "%10d", price);
    const char* const name = gShopDetailCC3210.data();
    const char* secondLine;
    if (std::strlen(name) <= 20) {
        std::strcpy(static_cast<WindowTextData*>(gCurrentWindow->elements[17].data)->text, name);
        secondLine = input::LegacyText593850();
    } else {
        const std::size_t prefix = input::ClassifyLegacyBoundary_44B440(name, 20) == 2 ? 19 : 20;
        std::memcpy(
            static_cast<WindowTextData*>(gCurrentWindow->elements[17].data)->text, name, prefix);
        static_cast<WindowTextData*>(gCurrentWindow->elements[17].data)->text[prefix + 1] = '\0';
        secondLine = name + prefix;
    }
    std::strcpy(static_cast<WindowTextData*>(gCurrentWindow->elements[22].data)->text, secondLine);
}

void Sub_4B9270()
{
    std::sprintf(gShopCountTextsD2D988.data(), "%2d", gShopValueD046C4);
    std::sprintf(gShopValueTextCF4A28.data(), "%10d", gShopValueD046C4);
    std::int32_t price;
    std::memcpy(&price, gShopDetailCC3210.data() + 0xC2C, sizeof(price));
    gShopTotalD2DAFC = price * gShopValueD046C4;
}

void Sub_4B92C0()
{
    const std::int32_t mode = gShopValueCAFEA4;
    gShopValueC8AF74 = 0;
    if (mode == 0) {
        std::memcpy(&gShopValueC8AF74, gShopDetailCC3210.data() + 0xC24, sizeof(gShopValueC8AF74));
        return;
    }
    std::int32_t itemId;
    std::memcpy(&itemId, gShopDetailCC3210.data() + 0xC28, sizeof(itemId));
    std::int32_t total = 0;
    for (const world::detail::PlayerItemRecord& record : world::detail::gPlayerItemsFDA8A4) {
        if (record.used != 0 && record.itemId == itemId) {
            total += record.count;
        }
    }
    gShopValueC8AF74 = total;
}

std::int32_t Sub_4B9310(const std::int32_t value)
{
    if (value == -1) {
        gShopScrollCC488C = 0;
        gShopAvailableCF4A24 = -1;
        gCurrentWindow->elements[0].visible = 0;
        for (std::int32_t row = 0; row < 5; ++row) {
            gCurrentWindow->elements[17 + row].visible = 1;
            gCurrentWindow->elements[22 + row].visible = 1;
            gCurrentWindow->elements[27 + row].visible = 0;
            gCurrentWindow->elements[32 + row].visible = 1;
            gCurrentWindow->elements[37 + row].visible = 1;
            gCurrentWindow->elements[42 + row].visible = 1;
            gCurrentWindow->elements[47 + row].visible = 1;
            gCurrentWindow->elements[52 + row].visible = 1;
            gCurrentWindow->elements[57 + row].visible = 1;
        }
        switch (std::bit_cast<std::int8_t>(gMaterialOccupancyCC4894[20])) {
        case 0:
            Sub_4B8310();
            Sub_4B8670();
            Sub_4B86C0();
            Sub_4B8730();
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[63].data)->imageNo = 244381;
            for (std::int32_t row = 0; row < 5; ++row) {
                gCurrentWindow->elements[52 + row].visible = 1;
                gCurrentWindow->elements[52 + row].mouseCallback = &Sub_4BBED0;
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[57 + row].data)->imageNo =
                    244382;
            }
            gCurrentWindow->elements[6].visible = 0;
            gCurrentWindow->elements[4].visible = 0;
            break;
        case 1:
        case 4:
        case 5:
            Sub_4B8780();
            Sub_4B8B10();
            Sub_4B8BE0();
            Sub_4B8C50();
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[63].data)->imageNo = 244381;
            for (std::int32_t row = 0; row < 5; ++row) {
                gCurrentWindow->elements[52 + row].visible = 1;
                gCurrentWindow->elements[52 + row].mouseCallback = &Sub_4BC230;
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[57 + row].data)->imageNo =
                    244382;
            }
            gCurrentWindow->elements[6].visible = 0;
            gCurrentWindow->elements[4].visible = 1;
            break;
        case 2:
            Sub_4B8C90();
            Sub_4B8FF0();
            Sub_4B9090();
            Sub_4B90E0();
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[63].data)->imageNo = 244388;
            for (std::int32_t row = 0; row < 5; ++row) {
                gCurrentWindow->elements[52 + row].visible = 1;
                gCurrentWindow->elements[52 + row].mouseCallback = &Sub_4BBED0;
                static_cast<WindowGraphicData*>(gCurrentWindow->elements[57 + row].data)->imageNo =
                    244389;
            }
            gCurrentWindow->elements[6].visible = 1;
            gCurrentWindow->elements[4].visible = 0;
            break;
        case 3:
            Sub_4B9110();
            Sub_4B9270();
            for (std::int32_t row = 1; row < 5; ++row) {
                gCurrentWindow->elements[17 + row].visible = 0;
                gCurrentWindow->elements[22 + row].visible = 0;
                gCurrentWindow->elements[27 + row].visible = 0;
                gCurrentWindow->elements[32 + row].visible = 0;
                gCurrentWindow->elements[37 + row].visible = 0;
                gCurrentWindow->elements[42 + row].visible = 0;
                gCurrentWindow->elements[47 + row].visible = 0;
                gCurrentWindow->elements[52 + row].visible = 0;
                gCurrentWindow->elements[57 + row].visible = 0;
            }
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[63].data)->imageNo = 244388;
            gCurrentWindow->elements[17].visible = 1;
            gCurrentWindow->elements[22].visible = 1;
            gCurrentWindow->elements[27].visible = 0;
            gCurrentWindow->elements[32].visible = 1;
            gCurrentWindow->elements[37].visible = 1;
            gCurrentWindow->elements[42].visible = 1;
            gCurrentWindow->elements[47].visible = 1;
            gCurrentWindow->elements[52].visible = 1;
            gCurrentWindow->elements[57].visible = 1;
            gCurrentWindow->elements[52].mouseCallback = &Sub_4BC060;
            static_cast<WindowGraphicData*>(gCurrentWindow->elements[57].data)->imageNo = 244389;
            gCurrentWindow->elements[6].visible = 1;
            gCurrentWindow->elements[4].visible = 0;
            break;
        default:
            break;
        }
        gShopRowKeysC9A954[0] = -2;
        gShopTooltipIndex580844 = -1;
        gShopRowKeysC9A954[1] = -2;
        gShopMessage580848 = -1;
        gShopRowKeysC9A954[2] = -2;
        gShopTooltipPageD67B8C = 0;
        gShopRowKeysC9A954[3] = -2;
        gShopRowKeysC9A954[4] = -2;
        auto* const title = static_cast<WindowTextData*>(gCurrentWindow->elements[5].data);
        std::strcpy(title->text, gNpcDialogLinesCC24D4[0].text.data());
        auto* animation = static_cast<WindowAnimationData*>(gCurrentWindow->elements[9].data);
        login::detail::SetYobiActionField2A8(animation->action, gShopImageD2DAD0);
        animation = static_cast<WindowAnimationData*>(gCurrentWindow->elements[9].data);
        login::detail::SetYobiActionField2B8(animation->action, 5);
    }
    if (Sub_4D3040(2, 2) != 0) {
        gCurrentWindow->state |= 0x8000u;
    }
    if (gShopDialogTypeCC3EC8 != world::detail::gValue581800) {
        gCurrentWindow->state |= 0x8000u;
    }
    gCurrentWindow->elements[10].visible = 1;
    if (gShopItemCountCB1E10 - 5 <= 0) {
        gCurrentWindow->elements[10].visible = 0;
    }
    WindowElement* const elements = gCurrentWindow->elements;
    if ((static_cast<const WindowMouseData*>(elements[11].data)->flags & 1u) != 0) {
        gShopScrollCC488C = Sub_474270(elements + 11, gShopItemCountCB1E10 - 5);
        switch (std::bit_cast<std::int8_t>(gMaterialOccupancyCC4894[20])) {
        case 0:
            Sub_4B8310();
            Sub_4B8670();
            break;
        case 1:
        case 4:
        case 5:
            Sub_4B8780();
            Sub_4B8B10();
            break;
        case 2:
            Sub_4B8C90();
            Sub_4B8FF0();
            break;
        default:
            break;
        }
    }
    std::sprintf(gShopMoneyTextCC1A00.data(), "%10d", world::detail::ReadPlayerValueFC());
    if (gMaterialOccupancyCC4894[20] == 0) {
        const std::int32_t total = gShopTotalD2DAFC;
        const std::int32_t money = world::detail::ReadPlayerValueFC();
        static_cast<WindowNumberData*>(gCurrentWindow->elements[7].data)->color =
            total <= money ? 0 : 6;
    }
    if (gMaterialOccupancyCC4894[20] == 2) {
        const std::int32_t previous = gShopAvailableCF4A24;
        const std::int32_t itemId = gValueD2D608;
        gShopAvailableCF4A24 = 0;
        std::int32_t total = 0;
        for (const world::detail::PlayerItemRecord& record : world::detail::gPlayerItemsFDA8A4) {
            if (record.used != 0 && record.itemId == itemId) {
                total += record.count;
            }
        }
        gShopAvailableCF4A24 = total;
        if (total != previous) {
            Sub_4B8C90();
            std::sprintf(gShopTotalTextCC4B2C.data(), "%10d", gShopAvailableCF4A24);
        }
    }
    if (gShopMessage580848 >= 0) {
        gShopMessage580848 = -1;
    }
    gShopTooltipIndex580844 = -1;
    if (gMaterialOccupancyCC4894[20] == 3) {
        Sub_4B92C0();
        std::int32_t amount;
        if (gShopValueCAFEA4 != 0) {
            amount = gShopValueC8AF74;
        } else {
            std::memcpy(&amount, gShopDetailCC3210.data() + 0xC24, sizeof(amount));
        }
        std::sprintf(gShopTotalTextCC4B2C.data(), "%10d", amount);
    }
    return 1;
}

std::int32_t Sub_4BBED0(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t item = gShopScrollCC488C + index - 52;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t consumed = 0;
    if ((state & 1) != 0) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = window->x + element.x;
        const std::int32_t y = window->y + element.y;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
            static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
            223, login::detail::gValueD69768, 0);
    }
    const char* const record = gShopItemsCF4FE0.data() + item * 0xC58;
    if (record[0] == 0) {
        return 0;
    }
    if ((state & 1) != 0) {
        if (gShopRowKeysC9A954[index - 52] != item) {
            gShopTooltipPageD67B8C = 0;
        }
        gShopRowKeysC9A954[index - 52] = item;
        if ((state & 4) != 0) {
            ++gShopTooltipPageD67B8C;
            if (gShopTooltipPageD67B8C >= 4) {
                gShopTooltipPageD67B8C = 0;
            }
            consumed = 1;
        }
        gShopTooltipIndex580844 = item + 100;
    }
    if (record[0xC3C] == 0) {
        static_cast<void>(render::ClearTextQueueTail_456D50(3));
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = window->x + element.x + 1;
        const std::int32_t y = window->y + element.y + 31;
        static_cast<void>(render::DrawText_456780(
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x)),
            std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y)), 3, 0, 6, "X", 0, 0));
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement& element = window->elements[index];
    std::int32_t imageNo;
    std::memcpy(&imageNo, record + 0xC20, sizeof(imageNo));
    static_cast<void>(login::detail::DrawGraphic(
        element.x + window->x + 24, element.y + window->y + 24, 223, imageNo, 0, &descriptor));
    return consumed;
}

std::int32_t Sub_4BC060(const std::int32_t index, const std::int32_t state)
{
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t consumed = 0;
    if ((state & 4) != 0) {
        ++gShopTooltipPageD67B8C;
        if (gShopTooltipPageD67B8C >= 4) {
            gShopTooltipPageD67B8C = 0;
        }
        consumed = 1;
    }
    if ((state & 1) != 0) {
        gShopTooltipIndex580844 = 200;
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = window->x + element.x;
        const std::int32_t y = window->y + element.y;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
            static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
            223, login::detail::gValueD69768, 0);
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement& element = window->elements[index];
    std::int32_t imageNo;
    std::memcpy(&imageNo, gShopDetailCC3210.data() + 0xC20, sizeof(imageNo));
    static_cast<void>(login::detail::DrawGraphic(
        element.x + window->x + 24, element.y + window->y + 24, 223, imageNo, 0, &descriptor));
    return consumed;
}

std::int32_t Sub_4BC230(const std::int32_t index, const std::int32_t state)
{
    const std::int32_t row = gShopScrollCC488C + index - 52;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0xFFFFFFFFu;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
    descriptor.bytes[13] = 4;
    std::int32_t consumed = 0;
    if (gShopRowKeysC9A954[index - 52] != row) {
        gShopTooltipPageD67B8C = 0;
    }
    gShopRowKeysC9A954[index - 52] = row;
    auto events = static_cast<std::uint8_t>(state);
    if ((events & 0x60u) != 0 && Sub_475D60() != 9 && Sub_475D60() != 2) {
        consumed = 1;
        events &= 0x9Fu;
    }
    if ((events & 0x40u) != 0 && Sub_475D60() == 9) {
        const std::int32_t source = Sub_475D90();
        if (source != row) {
            const char sourceItem = gShopSelectedSlotsCB03DC[source];
            const char targetItem = gShopSelectedSlotsCB03DC[row];
            gShopSelectedSlotsCB03DC[row] = sourceItem;
            gShopSelectedSlotsCB03DC[source] = targetItem;
            Sub_4B8780();
            Sub_4B8B10();
        }
        Sub_476500();
    }
    const bool hover = (events & 1u) != 0;
    if (hover && std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[row]) >= 0) {
        const char mode = gMaterialOccupancyCC4894[20];
        if (mode == 1 || mode == 4 || mode == 5) {
            std::strcpy(gHoverTextD68AE8.data(), kShopHints57E6C4[2]);
        }
    }
    if ((events & 0x21u) != 0 && (platform::gLegacyMouseState.edgeButtons & 0x10u) != 0) {
        static_cast<void>(Sub_4763E0());
        const std::int32_t item = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[row]);
        gShopSelectedSlotsCB03DC[row] = static_cast<char>(-1);
        char* const record = gShopItemsCF4FE0.data() + item * 0xC58;
        WriteTradeValue(record, 0xC28, std::int32_t{0});
        std::int32_t inventoryIndex;
        std::memcpy(&inventoryIndex, record + 0xC2C, sizeof(inventoryIndex));
        const auto selectedMask = static_cast<std::uint32_t>(gShopValueCF467C);
        gShopValueCF467C = std::bit_cast<std::int32_t>(selectedMask & ~(1u << (inventoryIndex - 8)));
        Sub_4B8780();
        Sub_4B8B10();
        Sub_4B8BE0();
        Sub_4B8C50();
        gShopMessage580848 = -1;
    }
    if (hover && std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[row]) >= 0
        && (events & 2u) != 0) {
        static_cast<void>(Sub_4B6DA0(row));
        static_cast<void>(audio::PlaySe_52C450(51, 320));
        consumed = 1;
    }
    if (Sub_475D60() == 9 && Sub_475D90() == row) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = window->x + element.x;
        const std::int32_t y = window->y + element.y;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x + 2), static_cast<std::uint16_t>(y + 2),
            static_cast<std::uint16_t>(x + 46), static_cast<std::uint16_t>(y + 46),
            223, 254, 0);
    }
    if (hover) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = window->x + element.x;
        const std::int32_t y = window->y + element.y;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
            static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
            223, login::detail::gValueD69768, 0);
    }
    if ((events & 0x20u) != 0 && Sub_475D60() == 9) {
        const WindowInstance* const window = gCurrentWindow;
        const WindowElement& element = window->elements[index];
        const std::int32_t x = window->x + element.x;
        const std::int32_t y = window->y + element.y;
        login::detail::DrawGraphicHover(
            static_cast<std::uint16_t>(x), static_cast<std::uint16_t>(y),
            static_cast<std::uint16_t>(x + 48), static_cast<std::uint16_t>(y + 48),
            223, login::detail::gValueD69768, 0);
    }
    const std::int32_t item = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[row]);
    if (item < 0) {
        return 0;
    }
    if (hover) {
        if (gShopRowKeysC9A954[index - 52] != row) {
            gShopTooltipPageD67B8C = 0;
        }
        gShopRowKeysC9A954[index - 52] = row;
        if ((events & 4u) != 0) {
            ++gShopTooltipPageD67B8C;
            if (gShopTooltipPageD67B8C >= 4) {
                gShopTooltipPageD67B8C = 0;
            }
            consumed = 1;
        }
        gShopTooltipIndex580844 = item + 100;
        const char* const record = gShopItemsCF4FE0.data() + item * 0xC58;
        char buffer[256];
        std::uint32_t flags = 0;
        if (std::strlen(record + 0xC44) != 0) {
            std::memcpy(&flags, record + 0xC40, sizeof(flags));
        }
        if ((flags & 0x100u) != 0) {
            std::sprintf(buffer, "%s[kok]", record + 1);
            std::strcpy(gHoverTextD68AE8.data(), buffer);
        } else {
            std::memcpy(&flags, record + 0xC40, sizeof(flags));
            if ((flags & 0x200u) != 0) {
                std::sprintf(buffer, "%s[han]", record + 0xC44);
                std::strcpy(gHoverTextD68AE8.data(), buffer);
            }
        }
    }
    const WindowInstance* const window = gCurrentWindow;
    const WindowElement& element = window->elements[index];
    const std::int32_t drawItem = std::bit_cast<std::int8_t>(gShopSelectedSlotsCB03DC[row]);
    std::int32_t imageNo;
    std::memcpy(&imageNo, gShopItemsCF4FE0.data() + drawItem * 0xC58 + 0xC20, sizeof(imageNo));
    static_cast<void>(login::detail::DrawGraphic(
        element.x + window->x + 24, element.y + window->y + 24, 223, imageNo, 0, &descriptor));
    return consumed;
}

std::int32_t Sub_4B99D0(const std::int32_t unused)
{
    static_cast<void>(unused);
    const WindowInstance* const window = gCurrentWindow;
    const std::int32_t x = window->x + 298;
    std::int32_t y = window->y + 50;
    char mode = gMaterialOccupancyCC4894[20];
    if (mode == 1) {
        const std::int32_t money = world::detail::ReadPlayerValueFC();
        const std::int32_t total = money + gShopTotalD2DAFC;
        const std::int32_t limit = world::detail::ReadPlayerValue15BD0(0);
        if (total > limit) {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
            mode = gMaterialOccupancyCC4894[20];
            gShopMessage580848 = -2;
        }
    }
    if (mode == 3 && gShopValueCAFEA4 == 0 && gShopValueC8AF74 <= 0) {
        gShopMessage580848 = -4;
    }
    const std::int32_t tooltip = gShopTooltipIndex580844;
    if (tooltip >= 0) {
        if (tooltip < 100) {
            static_cast<void>(Sub_48B1A0(tooltip, gShopTooltipPageD67B8C));
        } else if (tooltip < 200) {
            static_cast<void>(Sub_48B560(tooltip - 100, gShopTooltipPageD67B8C, mode == 0));
        } else {
            static_cast<void>(Sub_48B750(gShopTooltipPageD67B8C));
        }
    }
    const std::int32_t message = gShopMessage580848;
    gTooltipColorD67D30 = 0;
    const WindowDialogTextLine* line = gNpcDialogLinesCC24D4.data() + 1 + 4 * (-1 - message);
    for (std::int32_t row = 0; row < 4; ++row) {
        if (std::strlen(line->text.data()) != 0) {
            static_cast<void>(Sub_4FBA30(x, y, 3, 8, 0, line->text.data(), 0, 0));
        }
        ++line;
        y += 10;
    }
    DrawCurrentWindow_4729B0();
    return 1;
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

void Sub_49E990(const std::int32_t offset)
{
    for (std::int32_t line = 0; line < 5; ++line) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[9 + line].data);
        std::strcpy(data->text, gWindowListNamesCF3F20[offset + line].data());
    }
}

std::int32_t Sub_49E8F0(const std::int32_t value)
{
    if (value == -1) {
        const std::int32_t visible = gWindowListCountCF46A8 > 5 ? 1 : 0;
        gWindowListOffsetCC3EF4 = 0;
        for (std::int32_t index = 0; index < 6; ++index) {
            gCurrentWindow->elements[index].visible = visible;
        }
        Sub_49E990(gWindowListOffsetCC3EF4);
    }
    if (Sub_4D3040(1, 1) != 0) {
        gCurrentWindow->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
    }
    return 1;
}

std::int32_t Sub_49EAA0(const std::int32_t)
{
    gCurrentWindow->elements[0].visible = 1;
    if (gWindowListCountCF46A8 - 5 <= 0) {
        gCurrentWindow->elements[0].visible = 0;
    }
    WindowElement* const elements = gCurrentWindow->elements;
    const auto* const mouse = static_cast<const WindowMouseData*>(elements[1].data);
    if ((mouse->flags & 1u) != 0) {
        gWindowListOffsetCC3EF4 = Sub_474270(elements + 1, gWindowListCountCF46A8 - 5);
        Sub_49E990(gWindowListOffsetCC3EF4);
    }
    DrawCurrentWindow_4729B0();
    WindowInstance* const window = gCurrentWindow;
    const auto height = static_cast<std::int16_t>(window->height);
    const auto width = static_cast<std::int16_t>(window->width);
    const auto y = static_cast<std::int16_t>(window->y);
    const auto x = static_cast<std::int16_t>(window->x);
    login::detail::DrawNineGrid_49BE00(
        0x3BE34, x, y, width, height, 0xDF, -1,
        std::bit_cast<std::int32_t>(std::uint32_t{0x80FFFFFF}), 74, 54);
    static_cast<void>(render::ClearTextQueueTail_456D50(3));
    return 1;
}

std::int32_t Sub_474270(WindowElement* const element, const std::int32_t maximum)
{
    if (maximum <= 0) {
        return 0;
    }
    const std::int32_t span = element->height - (element - 1)->height;
    const std::int32_t halfStep = span / (maximum * 2);
    const auto* const mouse = static_cast<const WindowMouseData*>(element->data);
    return (halfStep + mouse->deltaY) * maximum / span;
}

std::int32_t Sub_4742C0(
    WindowElement* const element, const std::int32_t maximum, const std::int32_t position)
{
    auto* const mouse = static_cast<WindowMouseData*>(element->data);
    std::int32_t y;
    if (maximum <= 0) {
        y = element->y;
    } else {
        y = (element->height - (element - 1)->height) * position / maximum;
        y += element->y;
    }
    (element - 1)->y = y;
    const std::int32_t delta = y - element->y;
    mouse->deltaY = delta;
    return delta;
}

std::int32_t Sub_4743B0(
    WindowElement* const element, std::int32_t position, std::int32_t maximum,
    const std::int32_t wheel)
{
    auto* const mouse = static_cast<WindowMouseData*>(element->data);
    if (wheel == 0) {
        return position;
    }
    if (maximum < 0) {
        maximum = 0;
    }
    WindowElement* const previous = element - 1;
    const std::int32_t oldY = previous->y;
    static_cast<void>(Sub_4742C0(element, maximum, position));
    std::int32_t remainder = oldY - previous->y;
    const double product = static_cast<double>(wheel)
        * static_cast<double>(std::bit_cast<float>(std::uint32_t{0x3C088889}));
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
    static_cast<void>(Sub_4742C0(element, maximum, position));
    previous->y += remainder;
    const std::int32_t span = element->height - previous->height;
    const double fraction = static_cast<double>(roundedProduct) - static_cast<double>(whole);
    const double pixels = fraction * static_cast<double>(span) / static_cast<double>(maximum);
    previous->y += static_cast<std::int32_t>(pixels);
    if (previous->y < element->y) {
        previous->y = element->y;
    }
    if (previous->y + previous->height > element->y + element->height) {
        previous->y = element->y - previous->height + element->height;
    }
    const std::int32_t delta = previous->y - element->y;
    const std::uint32_t flags = mouse->flags;
    mouse->deltaY = delta;
    mouse->flags = flags | 1u;
    return position;
}

std::int32_t Sub_49EB50(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        if (gWindowListOffsetCC3EF4 > 0) {
            --gWindowListOffsetCC3EF4;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gWindowListOffsetCC3EF4;
        WindowInstance* const window = gCurrentWindow;
        const std::int32_t maximum = gWindowListCountCF46A8 - 5;
        static_cast<void>(Sub_4742C0(window->elements + index - 1, maximum, position));
        Sub_49E990(gWindowListOffsetCC3EF4);
        result = 1;
    }
    graphic->imageNo = 0x3B592;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B594;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B593;
    }
    return result;
}

std::int32_t Sub_49EC00(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 0x400) != 0) {
        if (gWindowListOffsetCC3EF4 < gWindowListCountCF46A8 - 5) {
            ++gWindowListOffsetCC3EF4;
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        } else {
            static_cast<void>(audio::PlaySe_52C450(55, 320));
        }
        const std::int32_t position = gWindowListOffsetCC3EF4;
        const std::int32_t maximum = gWindowListCountCF46A8 - 5;
        static_cast<void>(Sub_4742C0(gCurrentWindow->elements + index - 2, maximum, position));
        Sub_49E990(gWindowListOffsetCC3EF4);
        result = 1;
    }
    graphic->imageNo = 0x3B595;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B597;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B596;
    }
    return result;
}

std::int32_t Sub_49ECC0(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        static_cast<void>(audio::PlaySe_52C450(gWindowListOffsetCC3EF4 > 0 ? 51 : 55, 320));
        gWindowListOffsetCC3EF4 -= 5;
        if (gWindowListOffsetCC3EF4 < 0) {
            gWindowListOffsetCC3EF4 = 0;
        }
        const std::int32_t position = gWindowListOffsetCC3EF4;
        const std::int32_t maximum = gWindowListCountCF46A8 - 5;
        static_cast<void>(Sub_4742C0(gCurrentWindow->elements + index - 3, maximum, position));
        Sub_49E990(gWindowListOffsetCC3EF4);
        result = 1;
    }
    graphic->imageNo = 0x3B556;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B558;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B557;
    }
    return result;
}

std::int32_t Sub_49ED80(const std::int32_t index, const std::int32_t state)
{
    auto* const graphic = static_cast<WindowGraphicData*>(gCurrentWindow->elements[index].data);
    std::int32_t result = 0;
    if ((state & 2) != 0) {
        const std::int32_t oldMaximum = gWindowListCountCF46A8 - 5;
        const bool canMove = oldMaximum > 0 && gWindowListOffsetCC3EF4 < oldMaximum;
        static_cast<void>(audio::PlaySe_52C450(canMove ? 51 : 55, 320));
        std::int32_t position = gWindowListOffsetCC3EF4 + 5;
        const std::int32_t maximum = gWindowListCountCF46A8 - 5;
        gWindowListOffsetCC3EF4 = position;
        if (position > maximum) {
            position = maximum;
            gWindowListOffsetCC3EF4 = position;
        }
        if (position < 0) {
            position = 0;
            gWindowListOffsetCC3EF4 = position;
        }
        static_cast<void>(Sub_4742C0(gCurrentWindow->elements + index - 4, maximum, position));
        Sub_49E990(gWindowListOffsetCC3EF4);
        result = 1;
    }
    graphic->imageNo = 0x3B559;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B55B;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B55A;
    }
    return result;
}

std::int32_t Sub_49EE60(const std::int32_t index, const std::int32_t state)
{
    if ((state & 0x21) != 0) {
        const std::int32_t count = gWindowListCountCF46A8;
        const auto wheel = std::bit_cast<std::int32_t>(platform::gLegacyMouseState.tailReset);
        const std::int32_t position = gWindowListOffsetCC3EF4;
        gWindowListOffsetCC3EF4 =
            Sub_4743B0(gCurrentWindow->elements + index - 5, position, count - 5, wheel);
    }
    return 0;
}

std::int32_t Sub_49EEB0(const std::int32_t index, const std::int32_t state)
{
    WindowInstance* const window = gCurrentWindow;
    auto* const graphic = static_cast<WindowGraphicData*>(window->elements[index].data);
    std::int32_t result = 0;
    if ((state & 1) != 0) {
        std::strcpy(gHoverTextD68AE8.data(), "\xA4\xA4\xA4\xEE\xA1\x43");
    }
    if ((state & 2) != 0) {
        window->state |= 0x8000u;
        static_cast<void>(audio::PlaySe_52C450(57, 320));
        result = 1;
    }
    graphic->imageNo = 0x3B562;
    if ((state & 1) != 0) {
        graphic->imageNo = 0x3B564;
    }
    if ((state & 8) != 0) {
        graphic->imageNo = 0x3B563;
    }
    return result;
}

void Sub_485110(
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
    login::detail::DrawGraphicHover(
        static_cast<std::uint16_t>(left), static_cast<std::uint16_t>(top),
        static_cast<std::uint16_t>(right), static_cast<std::uint16_t>(bottom),
        0xDF, imageNo, 0);
}

std::int32_t Sub_49EF60(const std::int32_t index, const std::int32_t state)
{
    if (index - 14 >= gWindowListCountCF46A8) {
        return 0;
    }
    if ((state & 1) != 0) {
        const std::uint32_t imageNo = login::detail::gValueD69768;
        Sub_485110(gCurrentWindow->elements + index, state, imageNo);
    }
    if ((state & 2) == 0) {
        return 0;
    }
    const WindowListSelectionCallback callback = gWindowListCallbackCB1E14;
    if (callback != nullptr) {
        const std::int32_t selected = gWindowListOffsetCC3EF4 + index - 14;
        callback(selected, gWindowListNamesCF3F20[selected].data());
    }
    gCurrentWindow->state |= 0x8000u;
    static_cast<void>(audio::PlaySe_52C450(57, 320));
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

std::int32_t Sub_475D40()
{
    return gFocusWindowId57C120;
}

std::int32_t Sub_475D50()
{
    return gFocusElementIndex57C124;
}

std::int32_t Sub_475D60()
{
    return gMouseValue57C110;
}

std::int32_t Sub_475D70()
{
    return gDragX57C11C;
}

std::int32_t Sub_475D80()
{
    return gDragY57C11E;
}

std::int32_t Sub_475D90()
{
    return gDragSourceIndex57C114;
}

std::int32_t Sub_475DA0(
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

std::int32_t Sub_476490(
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

void Sub_476500()
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

std::int32_t Sub_4763B0(
    const std::int32_t windowId, const std::int32_t elementIndex)
{
    if (gMouseValue57C110 == 0) {
        return 0;
    }
    gFocusWindowId57C120 = windowId;
    gFocusElementIndex57C124 = elementIndex;
    return 1;
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

void Sub_50E3C0(const std::uintptr_t, const char* const text)
{
    if (world::detail::gValue9A6E0C != 0) {
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    static_cast<void>(detail::Sub_4E4E80(text));
}

void Sub_50E3F0(const std::uintptr_t, char* const text, const std::int32_t)
{
    if (world::detail::gValue9A6E0C != 0) {
        return;
    }
    if (world::gValueD67E78 != 0) {
        static_cast<void>(network::Sub_512270(platform::gConnectionSocket, 0));
        return;
    }
    if (world::ReadRippleGateOnce() != 0) {
        return;
    }
    input::UnescapeLegacyTextAndNotify_52D2F0(text);
    static_cast<void>(detail::Sub_4E5620(text));
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

void ConstructItemSelectors()
{
    detail::gItemSelectorCF3B10.preview = login::detail::BorrowWindowSelectorPreview(0);
    static_cast<void>(detail::Sub_4C61B0(&detail::gItemSelectorCF3B10));
    detail::gItemSelectorD2F338.preview = login::detail::BorrowWindowSelectorPreview(1);
    static_cast<void>(detail::Sub_4C61B0(&detail::gItemSelectorD2F338));
}

void DestroyItemSelectors()
{
    login::detail::DestroyWindowSelectorPreview(detail::gItemSelectorD2F338.preview);
    login::detail::DestroyWindowSelectorPreview(detail::gItemSelectorCF3B10.preview);
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

std::int32_t Sub_4AFF00()
{
    WindowInstance* const window = detail::gWindowStates[48].window;
    if (window == nullptr) {
        return 0;
    }
    window->state |= 0x8000u;
    return audio::PlaySe_52C450(57, 320);
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

void ProcessSceneWindows_503E80()
{
    using namespace detail;
    switch (world::gCurrentSceneType) {
    case 5:
        Sub_4C7AD0();
        InitializeLoginWindows_470600();
        OpenLoginSceneWindows_4707C0();
        return;
    case 7:
    case 11:
        Sub_472220();
        static_cast<void>(CloseAllWindows_472190());
        static_cast<void>(Sub_4C9130());
        world::detail::Sub_5186A0();
        static_cast<void>(world::detail::Sub_519300());
        static_cast<void>(world::detail::Sub_519550());
        static_cast<void>(world::detail::Sub_534770());
        static_cast<void>(world::detail::Sub_5348A0());
        return;
    case 9:
        switch (world::gCurrentSceneState) {
        case 1:
            MarkSceneWindows();
            static_cast<void>(CloseAllWindows_472190());
            Sub_470980();
            return;
        case 4:
        case 200:
            Sub_472220();
            static_cast<void>(CloseAllWindows_472190());
            return;
        case 204:
            Sub_470980();
            return;
        case 250:
        case 252:
            Sub_472220();
            static_cast<void>(CloseAllWindows_472190());
            Sub_470980();
            if (world::gCurrentSceneState == 250) {
                auto* const bytes = reinterpret_cast<unsigned char*>(world::gValueD67B08.data());
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
            OpenBattleSceneWindows_471170();
            return;
        case 8:
            MarkSceneWindows();
            static_cast<void>(CloseAllWindows_472190());
            return;
        default:
            return;
        }
    default:
        return;
    }
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
