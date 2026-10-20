#include "../WindowRuntimeInternal.hpp"
#include "../../script/BigMapSurface.hpp"
#include "../../script/LuaUI.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "../../world/WorldMapState.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include <array>
#include <bit>
#include <fstream>
#include <format>
#include <string_view>
#include <vector>

namespace cg::ui::detail {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowBigMapElementId : ElementId {
    Element1 = 1,
    Element2 = 2,
    Element3 = 3,
    Element4 = 4,
    Element5 = 5,
    Element6 = 6,
    Element7 = 7,
    Element8 = 8,
    Element9 = 9,
    Element10 = 10,
    Element11 = 11,
    Element12 = 12,
    Element13 = 13,
    Element14 = 14,
    Element15 = 15,
    Element16 = 16,
    Element17 = 17,
    Element18 = 18,
    Element19 = 19,
    Element20 = 20,
    Element21 = 21,
    Element22 = 22,
    Element23 = 23,
    Element24 = 24,
    Element25 = 25,
    Element26 = 26,
    Element27 = 27,
    Element28 = 28,
    Element29 = 29,
    Element30 = 30,
    Element31 = 31,
    Element32 = 32,
    Element33 = 33,
    Element34 = 34,
    Element35 = 35,
    Element36 = 36,
    Element37 = 37,
    Element38 = 38,
    Element39 = 39,
    Element40 = 40,
    Element41 = 41,
    Element42 = 42,
    Element43 = 43,
    Element44 = 44,
    Element45 = 45,
    Element46 = 46,
    Element47 = 47,
    Element48 = 48,
    Element49 = 49,
    Element50 = 50,
    Element51 = 51,
    Element52 = 52,
    Element53 = 53,
    Element54 = 54,
    Element55 = 55,
    Element56 = 56,
    Element57 = 57,
    Element58 = 58,
    Element59 = 59,
    Element60 = 60,
    Element61 = 61,
    Element62 = 62,
    Element63 = 63,
    Element64 = 64,
    Element65 = 65,
    Element66 = 66,
    Element67 = 67,
    Element68 = 68,
    Element69 = 69,
    Element70 = 70,
    Element71 = 71,
    Element72 = 72,
    Element73 = 73,
    Element74 = 74,
    Element75 = 75,
    Element76 = 76,
    Element77 = 77,
    Element78 = 78,
    Element79 = 79,
    Element80 = 80,
    Element81 = 81,
    Element82 = 82,
    Element83 = 83,
    Element84 = 84,
    Element85 = 85,
    Element86 = 86,
    Element87 = 87,
    Element88 = 88,
    Element89 = 89,
    Element90 = 90,
    Element91 = 91,
    Element92 = 92,
    Element93 = 93,
    Element94 = 94,
    Element95 = 95,
};



// 复用现有裁剪绘制和寻路接口，不创建Lua或扩展窗口实例。
using namespace cg::script;

static constexpr int kMapFont = 12;
static constexpr int kCoordinateRows = 10;
// 原资源ID升序重编号，见BMUI01；查MapNo，不依赖资源组内seqNo。
enum BigMapImage : int {
    RouteDot = 641510, Checked = 641511, PlayerMarker = 641513,
    MapNamePlate = 641514, TargetMarker = 641515, NpcDot = 641516,
    AnnotationPlate = 641518, AnnotationBorder = 641519, VerticalConnector = 641520,
    HorizontalConnector = 641521, Checkbox = 641522,
    WalkNormal = 641524, WalkHover = 641525, WalkPressed = 641526,
    CenterNormal = 641527, CenterHover = 641528, CenterPressed = 641529,
};
enum class BigMapButton { Toggle, Close, Walk, Center, CoordinateList };
// 4.06@49A528/2C/30/38：仅图标分类1、2、3、5消费该颜色。
static constexpr std::array<std::uint32_t, 7> kCategoryColors{
    0xFFFFFFFFu, 0xF05C20FFu, 0xF0F8F8F8u, 0xF0F84AF8u, 0xFFFFFFFFu, 0xF0FFC500u, 0xFFFFFFFFu};

// 4.06@4134BB/413636：外部GBK、TAB七字段；只保留当前地图，坐标相同后条覆盖。
struct BigMapAnnotation {
    int category = 0;
    std::int16_t x = 0, y = 0;
    std::wstring name;
    int extra = 0;
    std::int16_t placement = 0;
    int imageNo = -1, imageWidth = 0, imageHeight = 0;
    bool hasIcon = false;
};

// 前段按输入优先级排列：按钮、筛选、遮挡区域，地图最后接收事件。
enum BigMapElement : int {
    ButtonFirst = 0, ButtonEnd = ButtonFirst + 5,
    CategoryFirst = ButtonEnd, CategoryEnd = CategoryFirst + 7,
    ListToggleTextArea = CategoryEnd, ListScrollThumb, ListScrollTrack, ListScrollUp, ListScrollDown,
    ListPreviousPage, ListNextPage, ListRowFirst, ListRowEnd = ListRowFirst + kCoordinateRows, ListPanelArea = ListRowEnd,
    CurrentCoordinateArea, TargetCoordinateArea, TitleArea, NameArea, StatusArea, OptionsArea, MapArea,
    TextFirst, TextMapName = TextFirst, TextCurrentCoordinate, TextTargetCoordinate, TextToggle, TextWalk,
    TextCenter, TextOptions, TextPlayer, TextTarget, TextDisplay, TextStatus,
    CategoryTextFirst, ListNameFirst = CategoryTextFirst + 7, ListCoordinateFirst = ListNameFirst + kCoordinateRows,
    TextListToggle = ListCoordinateFirst + kCoordinateRows, TextListEmpty, TextListPage, TextEnd,
    MapNameImage = TextEnd,
    PlayerLegendImage, TargetLegendImage, CheckedFirst, CheckboxFirst = CheckedFirst + 7,
    ElementCount = CheckboxFirst + 7,
};
static int WindowBigMap_Update(int stage);
static int WindowBigMap_Draw(int stage);
static int WindowBigMap_Element(int index, int events);
static void WindowBigMap_Release();
// 与其它原生窗口一致：具名载荷、固定坐标/尺寸/初始显隐及回调直接预定义。
static WindowGraphicData gBigMapToggleGraphic{243033, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapCloseGraphic{22509, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapWalkGraphic{WalkNormal, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapCenterGraphic{CenterNormal, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapListGraphic{CenterNormal, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapScrollThumbGraphic{243096, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapScrollUpGraphic{243090, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapScrollDownGraphic{243093, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapPreviousPageGraphic{243030, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapNextPageGraphic{243033, 0, 0, 0, 0, 0xFFFFFFFFu};
static std::uint32_t gBigMapListScrollFlags = 0;
static WindowGraphicData gBigMapMapNameGraphic{MapNamePlate, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapPlayerGraphic{PlayerMarker, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapTargetGraphic{TargetMarker, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapCheckedGraphic{Checked, 0, 0, 0, 0, 0xFFFF0000u};
static WindowGraphicData gBigMapCheckboxGraphic{Checkbox, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowTextPrototype gBigMapMapNameText{{std::byte{127}, std::byte{5}, {}, {}}, L""};
static WindowTextPrototype gBigMapCoordinateText{{std::byte{0}, std::byte{12}, {}, {}}, L""};
static WindowTextPrototype gBigMapOptionsText{{std::byte{0}, std::byte{12}, {}, {}}, L"地图选项"};
static WindowTextPrototype gBigMapWalkText{{std::byte{0}, std::byte{12}, {}, {}}, L"开始寻路"};
static WindowTextPrototype gBigMapCenterText{{std::byte{0}, std::byte{12}, {}, {}}, L"玩家居中"};
static WindowTextPrototype gBigMapPlayerText{{std::byte{0}, std::byte{12}, {}, {}}, L"你现在的位置"};
static WindowTextPrototype gBigMapTargetText{{std::byte{0}, std::byte{12}, {}, {}}, L"右键点选的寻路坐标"};
static WindowTextPrototype gBigMapDisplayText{{std::byte{0}, std::byte{12}, {}, {}}, L"显示选项"};
static WindowTextPrototype gBigMapStatusText{{std::byte{0}, std::byte{12}, {}, {}}, L""};
static WindowTextPrototype gBigMapListText{{std::byte{0}, std::byte{12}, {}, {}}, L"坐标列表"};
static WindowTextPrototype gBigMapListRowText{{std::byte{0}, std::byte{12}, {}, {}}, L""};
static WindowTextPrototype gBigMapListEmptyText{{std::byte{0}, std::byte{12}, {}, {}}, L"暂无坐标"};
static WindowTextPrototype gBigMapCategoryTexts[7]{
    {{std::byte{0}, std::byte{12}, {}, {}}, L"小的地点或居所"},
    {{std::byte{0}, std::byte{12}, {}, {}}, L"伐木地点"},
    {{std::byte{0}, std::byte{12}, {}, {}}, L"挖掘地点"},
    {{std::byte{0}, std::byte{12}, {}, {}}, L"狩猎地点"},
    {{std::byte{0}, std::byte{12}, {}, {}}, L"村镇大地点"},
    {{std::byte{0}, std::byte{12}, {}, {}}, L"宠物Lv1坐标"},
    {{std::byte{0}, std::byte{12}, {}, {}}, L"特定NPC"},
};

static WindowElementPrototype gBigMapElements[]{
    {WindowElementType_Graphic, 11, 51, 20, 18, 1, {.graphic = &gBigMapToggleGraphic}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element1)}, // Toggle
    {WindowElementType_Graphic, 610, 10, 20, 20, 1, {.graphic = &gBigMapCloseGraphic}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element2)}, // Close
    {WindowElementType_Graphic, 11, 73, 18, 18, 0, {.graphic = &gBigMapWalkGraphic}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element3)}, // Walk
    {WindowElementType_Graphic, 11, 73, 18, 18, 0, {.graphic = &gBigMapCenterGraphic}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element4)}, // Center
    {WindowElementType_Graphic, 92, 51, 18, 18, 1, {.graphic = &gBigMapListGraphic}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element5)}, // CoordinateList
    {WindowElementType_EventArea, 42, 159, 194, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element6)}, // 分类0
    {WindowElementType_EventArea, 42, 179, 194, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element7)}, // 分类1
    {WindowElementType_EventArea, 42, 199, 194, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element8)}, // 分类2
    {WindowElementType_EventArea, 42, 219, 194, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element9)}, // 分类3
    {WindowElementType_EventArea, 42, 239, 194, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element10)}, // 分类4
    {WindowElementType_EventArea, 42, 259, 194, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element11)}, // 分类5
    {WindowElementType_EventArea, 42, 279, 194, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element12)}, // 分类6
    {WindowElementType_EventArea, 115, 51, 48, 18, 1, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element13)}, // ListToggleTextArea
    {WindowElementType_Graphic, 333, 94, 11, 14, 0, {.graphic = &gBigMapScrollThumbGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element14)}, // ListScrollThumb，紧邻滚动区域前一项
    {WindowElementType_MouseArea, 332, 94, 12, 178, 0, {.mouseFlags = &gBigMapListScrollFlags}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element15)}, // ListScrollTrack
    {WindowElementType_Graphic, 333, 83, 11, 11, 0, {.graphic = &gBigMapScrollUpGraphic}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element16)}, // ListScrollUp
    {WindowElementType_Graphic, 333, 272, 11, 11, 0, {.graphic = &gBigMapScrollDownGraphic}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element17)}, // ListScrollDown
    {WindowElementType_Graphic, 102, 293, 20, 20, 0, {.graphic = &gBigMapPreviousPageGraphic}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element18)}, // ListPreviousPage
    {WindowElementType_Graphic, 124, 293, 20, 20, 0, {.graphic = &gBigMapNextPageGraphic}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element19)}, // ListNextPage
    {WindowElementType_EventArea, 102, 83, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element20)}, // 坐标行0
    {WindowElementType_EventArea, 102, 103, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element21)}, // 坐标行1
    {WindowElementType_EventArea, 102, 123, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element22)}, // 坐标行2
    {WindowElementType_EventArea, 102, 143, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element23)}, // 坐标行3
    {WindowElementType_EventArea, 102, 163, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element24)}, // 坐标行4
    {WindowElementType_EventArea, 102, 183, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element25)}, // 坐标行5
    {WindowElementType_EventArea, 102, 203, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element26)}, // 坐标行6
    {WindowElementType_EventArea, 102, 223, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element27)}, // 坐标行7
    {WindowElementType_EventArea, 102, 243, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element28)}, // 坐标行8
    {WindowElementType_EventArea, 102, 263, 222, 20, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element29)}, // 坐标行9
    {WindowElementType_EventArea, 92, 73, 260, 250, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element30)}, // ListPanelArea
    {WindowElementType_EventArea, 513, 10, 96, 17, 1, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element31)}, // 当前坐标底色与遮挡
    {WindowElementType_EventArea, 513, 26, 96, 17, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element32)}, // 目标坐标底色与遮挡
    {WindowElementType_EventArea, 6, 6, 602, 32, 1, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element33)}, // TitleArea
    {WindowElementType_EventArea, 12, 12, 184, 38, 1, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element34)}, // NameArea
    {WindowElementType_EventArea, 8, 440, 624, 32, 1, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element35)}, // StatusArea
    {WindowElementType_EventArea, 36, 73, 224, 270, 0, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element36)}, // OptionsArea
    {WindowElementType_EventArea, 8, 8, 624, 464, 1, {}, &WindowBigMap_Element, -1, static_cast<ElementId>(WindowBigMapElementId::Element37)}, // MapArea
    {WindowElementType_Text, 18, 20, 0, 0, 1, {.text = &gBigMapMapNameText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element38)}, // TextMapName
    {WindowElementType_Text, 516, 12, 0, 0, 1, {.text = &gBigMapCoordinateText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element39)}, // TextCurrentCoordinate
    {WindowElementType_Text, 516, 28, 0, 0, 0, {.text = &gBigMapCoordinateText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element40)}, // TextTargetCoordinate
    {WindowElementType_Text, 34, 52, 0, 0, 1, {.text = &gBigMapOptionsText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element41)}, // TextToggle
    {WindowElementType_Text, 34, 76, 0, 0, 0, {.text = &gBigMapWalkText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element42)}, // TextWalk
    {WindowElementType_Text, 34, 76, 0, 0, 0, {.text = &gBigMapCenterText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element43)}, // TextCenter
    {WindowElementType_Text, 44, 81, 0, 0, 0, {.text = &gBigMapOptionsText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element44)}, // TextOptions
    {WindowElementType_Text, 62, 100, 0, 0, 0, {.text = &gBigMapPlayerText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element45)}, // TextPlayer
    {WindowElementType_Text, 62, 120, 0, 0, 0, {.text = &gBigMapTargetText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element46)}, // TextTarget
    {WindowElementType_Text, 44, 141, 0, 0, 0, {.text = &gBigMapDisplayText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element47)}, // TextDisplay
    {WindowElementType_Text, 12, 462, 0, 0, 1, {.text = &gBigMapStatusText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element48)}, // TextStatus
    {WindowElementType_Text, 62, 161, 0, 0, 0, {.text = &gBigMapCategoryTexts[0]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element49)}, // 分类0名称
    {WindowElementType_Text, 62, 181, 0, 0, 0, {.text = &gBigMapCategoryTexts[1]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element50)}, // 分类1名称
    {WindowElementType_Text, 62, 201, 0, 0, 0, {.text = &gBigMapCategoryTexts[2]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element51)}, // 分类2名称
    {WindowElementType_Text, 62, 221, 0, 0, 0, {.text = &gBigMapCategoryTexts[3]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element52)}, // 分类3名称
    {WindowElementType_Text, 62, 241, 0, 0, 0, {.text = &gBigMapCategoryTexts[4]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element53)}, // 分类4名称
    {WindowElementType_Text, 62, 261, 0, 0, 0, {.text = &gBigMapCategoryTexts[5]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element54)}, // 分类5名称
    {WindowElementType_Text, 62, 281, 0, 0, 0, {.text = &gBigMapCategoryTexts[6]}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element55)}, // 分类6名称
    {WindowElementType_Text, 102, 85, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element56)}, // 地点名称0
    {WindowElementType_Text, 102, 105, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element57)}, // 地点名称1
    {WindowElementType_Text, 102, 125, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element58)}, // 地点名称2
    {WindowElementType_Text, 102, 145, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element59)}, // 地点名称3
    {WindowElementType_Text, 102, 165, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element60)}, // 地点名称4
    {WindowElementType_Text, 102, 185, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element61)}, // 地点名称5
    {WindowElementType_Text, 102, 205, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element62)}, // 地点名称6
    {WindowElementType_Text, 102, 225, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element63)}, // 地点名称7
    {WindowElementType_Text, 102, 245, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element64)}, // 地点名称8
    {WindowElementType_Text, 102, 265, 144, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element65)}, // 地点名称9
    {WindowElementType_Text, 252, 85, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element66)}, // 地点坐标0
    {WindowElementType_Text, 252, 105, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element67)}, // 地点坐标1
    {WindowElementType_Text, 252, 125, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element68)}, // 地点坐标2
    {WindowElementType_Text, 252, 145, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element69)}, // 地点坐标3
    {WindowElementType_Text, 252, 165, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element70)}, // 地点坐标4
    {WindowElementType_Text, 252, 185, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element71)}, // 地点坐标5
    {WindowElementType_Text, 252, 205, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element72)}, // 地点坐标6
    {WindowElementType_Text, 252, 225, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element73)}, // 地点坐标7
    {WindowElementType_Text, 252, 245, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element74)}, // 地点坐标8
    {WindowElementType_Text, 252, 265, 72, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element75)}, // 地点坐标9
    {WindowElementType_Text, 115, 52, 0, 0, 1, {.text = &gBigMapListText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element76)}, // TextListToggle
    {WindowElementType_Text, 102, 85, 222, 16, 0, {.text = &gBigMapListEmptyText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element77)}, // TextListEmpty
    {WindowElementType_Text, 154, 297, 170, 16, 0, {.text = &gBigMapListRowText}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element78)}, // TextListPage
    {WindowElementType_Graphic, 10, 10, 184, 38, 1, {.graphic = &gBigMapMapNameGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element79)}, // MapNameImage
    {WindowElementType_Graphic, 44, 101, 14, 14, 0, {.graphic = &gBigMapPlayerGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element80)}, // PlayerLegendImage
    {WindowElementType_Graphic, 44, 121, 14, 14, 0, {.graphic = &gBigMapTargetGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element81)}, // TargetLegendImage
    {WindowElementType_Graphic, 46, 162, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element82)}, // 分类0选中标记
    {WindowElementType_Graphic, 46, 182, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element83)}, // 分类1选中标记
    {WindowElementType_Graphic, 46, 202, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element84)}, // 分类2选中标记
    {WindowElementType_Graphic, 46, 222, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element85)}, // 分类3选中标记
    {WindowElementType_Graphic, 46, 242, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element86)}, // 分类4选中标记
    {WindowElementType_Graphic, 46, 262, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element87)}, // 分类5选中标记
    {WindowElementType_Graphic, 46, 282, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element88)}, // 分类6选中标记
    {WindowElementType_Graphic, 46, 164, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element89)}, // 分类0选择框
    {WindowElementType_Graphic, 46, 184, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element90)}, // 分类1选择框
    {WindowElementType_Graphic, 46, 204, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element91)}, // 分类2选择框
    {WindowElementType_Graphic, 46, 224, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element92)}, // 分类3选择框
    {WindowElementType_Graphic, 46, 244, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element93)}, // 分类4选择框
    {WindowElementType_Graphic, 46, 264, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element94)}, // 分类5选择框
    {WindowElementType_Graphic, 46, 284, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowBigMapElementId::Element95)}, // 分类6选择框
};
static_assert(std::size(gBigMapElements) == ElementCount);

// 单实例原型；state bit1禁止通用拖动，标题事件区域负责窗口拖动。
WindowPrototype gWindowPrototypeBigMap{2, 4, 0, 0, 640, 480, 0, ElementCount,
    gBigMapElements, &WindowBigMap_Update, &WindowBigMap_Draw, &WindowBigMap_Release};

struct BigMapWindowState final {
    WindowInstance& window; // 借用原生实例，release回调先销毁本业务状态。
    BigMapSurface map;
    std::map<std::pair<int, int>, BigMapAnnotation> annotations;
    std::vector<const BigMapAnnotation*> coordinateRows; // 借用annotations节点，重载前先清空。
    int listFirstRow = 0, listMapWidth = -1, listMapHeight = -1;
    int lastListClickX = -1, lastListClickY = -1;
    bool listOpen = false, listDirty = true, scrollingList = false;
    std::array<bool, 7> categories{true, true, true, true, true, true, true};
    int currentMap = -1, currentFloor = -1, currentGroup = -1;
    int targetX = -1, targetY = -1;
    int dragX = 0, dragY = 0;
    int dragWindowX = 0, dragWindowY = 0;
    double dragMapX = 0, dragMapY = 0;
    bool movingWindow = false;
    bool dragMoved = false;
    bool mapClickPending = false; // 首次左击来自地图且未拖动，才允许后续原生双击寻路。
    bool options = false, followPlayer = true, dragging = false, mapReady = false;
    bool annotationsLoaded = false;
    bool acceptsMouse = false; // 当前调度帧可接收输入，避免被其它窗口遮住时仍显示悬停。
    int missingIcons = 0;
    int failedUiImages = 0;

    explicit BigMapWindowState(WindowInstance& instance) : window(instance) {
        Layout();
        // 原型保持固定布局；仅初始化时按当前字体度量入口的10px间距。
        auto* const labelElement = window.FindElementById(NativeElementKey(TextToggle));
        if (labelElement) {
            const int listButton = ButtonFirst + static_cast<int>(BigMapButton::CoordinateList);
            const int shift = labelElement->x + render::TextMeasure(gBigMapOptionsText.text, kMapFont) + 10
                - gBigMapElements[listButton].x;
            if (auto* const element = window.FindElementById(NativeElementKey(listButton))) element->x += shift;
            for (int i = ListToggleTextArea; i <= ListPanelArea; ++i) { if (auto* element = window.FindElementById(NativeElementKey(i))) element->x += shift; }
            for (int i = ListNameFirst; i < TextEnd; ++i) { if (auto* element = window.FindElementById(NativeElementKey(i))) element->x += shift; }
        }
        if (auto* const indexedElement2 = window.FindElementById(NativeElementKey(ListToggleTextArea)); indexedElement2) indexedElement2->width = render::TextMeasure(gBigMapListText.text, kMapFont);
    }

    int MaxListFirstRow() const {
        return std::max(0, static_cast<int>(coordinateRows.size()) - kCoordinateRows);
    }

    void RefreshListRows() {
        for (int row = 0; row < kCoordinateRows; ++row) {
            const int position = listFirstRow + row;
            const bool shown = listOpen && position < static_cast<int>(coordinateRows.size());
            if (auto* const indexedElement = window.FindElementById(NativeElementKey(ListRowFirst + row)); indexedElement) indexedElement->visible = shown;
            auto* const nameElement = window.FindElementById(NativeElementKey(ListNameFirst + row));
            if (nameElement) {
                nameElement->visible = shown;
                nameElement->data.text->text = shown ? coordinateRows[position]->name : L"";
            }
            auto* const coordinateElement = window.FindElementById(NativeElementKey(ListCoordinateFirst + row));
            if (coordinateElement) {
                coordinateElement->visible = shown;
                auto& coordinate = *coordinateElement->data.text;
                if (shown) {
                    const auto& point = *coordinateRows[position];
                    coordinate.text = std::format(L"{}, {}", point.x, point.y);
                } else { coordinate.text.clear(); }
            }
        }
        if (auto* const indexedElement = window.FindElementById(NativeElementKey(TextListEmpty)); indexedElement) indexedElement->visible = listOpen && coordinateRows.empty();
        const int pages = (static_cast<int>(coordinateRows.size()) + kCoordinateRows - 1) / kCoordinateRows;
        // 末屏允许不足10行时向前补齐；到达尾部显示末页。
        const int page = pages == 0 ? 0 : listFirstRow == MaxListFirstRow() ? pages : listFirstRow / kCoordinateRows + 1;
        if (auto* const indexedElement2 = window.FindElementById(NativeElementKey(TextListPage)); indexedElement2) indexedElement2->data.text->text = std::format(L"{} / {}", page, pages);
    }

    void SetListFirstRow(int position) {
        const int next = std::clamp(position, 0, MaxListFirstRow());
        if (next != listFirstRow) lastListClickX = lastListClickY = -1;
        listFirstRow = next;
        auto* const indexedElement = window.FindElementById(NativeElementKey(ListScrollTrack));
        if (indexedElement) {
            WindowScroll_SetPosition(indexedElement, MaxListFirstRow(), listFirstRow);
        }
        RefreshListRows();
    }

    void RebuildCoordinateList() {
        coordinateRows.clear();
        for (const auto& [position, point] : annotations) {
            if (categories[point.category] && point.x >= 0 && point.y >= 0
                && point.x < map.mapWidth && point.y < map.mapHeight) coordinateRows.push_back(&point);
        }
        listDirty = false;
        listMapWidth = map.mapWidth; listMapHeight = map.mapHeight;
        lastListClickX = lastListClickY = -1;
        SetListFirstRow(0);
    }

    void ToggleCoordinateList() {
        listOpen = !listOpen;
        if (listOpen) options = false;
        scrollingList = false;
        lastListClickX = lastListClickY = -1;
    }

    void Layout() {
        const int screenW = render::GetRendererLogicalWidth(), screenH = render::GetRendererLogicalHeight();
        window.x = std::clamp(window.x, 0, std::max(0, screenW - window.width));
        window.y = std::clamp(window.y, 0, std::max(0, screenH - window.height));
        auto* const viewportElement = window.FindElementById(NativeElementKey(MapArea));
        if (viewportElement) {
            const auto& viewport = (*viewportElement);
            map.x = viewport.x; map.y = viewport.y;
            map.w = viewport.width; map.h = viewport.height;
        }
        return;
    }

    static const resource::GraphicInfo* Graphic(int mapNo) {
        if (mapNo <= 0 || mapNo >= resource::gBinLoaderGraphicCapacity
            || !resource::gGraphicMapNoIndex || !resource::gGraphicInfo) return nullptr;
        const int index = resource::gGraphicMapNoIndex[mapNo];
        if (index < 0 || index >= resource::gMaxGraphicCount) return nullptr;
        const auto& info = resource::gGraphicInfo[index];
        return info.mapNo == mapNo && info.w > 0 && info.h > 0 ? &info : nullptr;
    }

    bool ElementHit(int index) const {
        auto* const elementPtr = window.FindElementById(NativeElementKey(index));
        if (elementPtr) {
            const auto& element = (*elementPtr);
            return element.visible == 1 && Hit(element.x, element.y, element.width, element.height);
        }
        return false;
    }

    bool MapHit() const {
        if (!mapReady) return false;
        for (int i = ButtonFirst; i < MapArea; ++i) if (ElementHit(i)) return false;
        return ElementHit(MapArea);
    }

    void SyncControls() {
        // 固定布局来自原型；这里只维护业务驱动的显隐和居中按钮的动态行。
        const int walk = ButtonFirst + static_cast<int>(BigMapButton::Walk);
        const int center = ButtonFirst + static_cast<int>(BigMapButton::Center);
        if (auto* const indexedElement = window.FindElementById(NativeElementKey(walk)); indexedElement) indexedElement->visible = targetX >= 0 && mapReady;
        if (auto* const indexedElement2 = window.FindElementById(NativeElementKey(center)); indexedElement2) indexedElement2->visible = !followPlayer;
        if (auto* const indexedElement3 = window.FindElementById(NativeElementKey(center)); indexedElement3) indexedElement3->y = targetX >= 0 ? 97 : 73;
        if (auto* const indexedElement4 = window.FindElementById(NativeElementKey(TextCenter)), * const indexedElement5 = window.FindElementById(NativeElementKey(center)); indexedElement4 && indexedElement5) indexedElement4->y = indexedElement5->y + 3;
        if (auto* const indexedElement6 = window.FindElementById(NativeElementKey(TextToggle)); indexedElement6) indexedElement6->visible = 1;
        if (auto* const indexedElement7 = window.FindElementById(NativeElementKey(TextWalk)), * const indexedElement8 = window.FindElementById(NativeElementKey(walk)); indexedElement7 && indexedElement8) indexedElement7->visible = !options && indexedElement8->visible;
        if (auto* const indexedElement9 = window.FindElementById(NativeElementKey(TextCenter)), * const indexedElement10 = window.FindElementById(NativeElementKey(center)); indexedElement9 && indexedElement10) indexedElement9->visible = !options && indexedElement10->visible;
        if (auto* const indexedElement11 = window.FindElementById(NativeElementKey(OptionsArea)); indexedElement11) indexedElement11->visible = options;
        if (auto* const indexedElement12 = window.FindElementById(NativeElementKey(TextTargetCoordinate)); indexedElement12) indexedElement12->visible = targetX >= 0;
        if (auto* const indexedElement13 = window.FindElementById(NativeElementKey(TargetCoordinateArea)); indexedElement13) indexedElement13->visible = targetX >= 0;
        for (int i : {TextOptions, TextPlayer, TextTarget, TextDisplay,
                PlayerLegendImage, TargetLegendImage}) {
            if (auto* element = window.FindElementById(NativeElementKey(i))) element->visible = options;
        }
        for (int i = 0; i < 7; ++i) {
            if (auto* const indexedElement14 = window.FindElementById(NativeElementKey(CategoryFirst + i)); indexedElement14) indexedElement14->visible = options;
            if (auto* const indexedElement15 = window.FindElementById(NativeElementKey(CategoryTextFirst + i)); indexedElement15) indexedElement15->visible = options;
            if (auto* const indexedElement16 = window.FindElementById(NativeElementKey(CheckedFirst + i)); indexedElement16) indexedElement16->visible = options && categories[i];
            if (auto* const indexedElement17 = window.FindElementById(NativeElementKey(CheckboxFirst + i)); indexedElement17) indexedElement17->visible = options;
        }
        if (auto* const indexedElement14 = window.FindElementById(NativeElementKey(ListPanelArea)); indexedElement14) indexedElement14->visible = listOpen;
        if (auto* const indexedElement15 = window.FindElementById(NativeElementKey(TextListPage)); indexedElement15) indexedElement15->visible = listOpen;
        if (auto* const indexedElement16 = window.FindElementById(NativeElementKey(ListPreviousPage)); indexedElement16) indexedElement16->visible = listOpen;
        if (auto* const indexedElement17 = window.FindElementById(NativeElementKey(ListNextPage)); indexedElement17) indexedElement17->visible = listOpen;
        for (int i : {ListScrollThumb, ListScrollTrack, ListScrollUp, ListScrollDown}) {
            if (auto* element = window.FindElementById(NativeElementKey(i))) element->visible = listOpen && MaxListFirstRow() > 0;
        }
        RefreshListRows();
    }

    void RefreshText() {
        if (auto* const indexedElement = window.FindElementById(NativeElementKey(TextMapName)); indexedElement) indexedElement->data.text->text = world::GetMapName_9DAD30();
        auto* const indexedElement2 = window.FindElementById(NativeElementKey(TextCurrentCoordinate));
        if (indexedElement2) {
            indexedElement2->data.text->text =
                std::format(L"当前：{}  {}", static_cast<int>(world::gPlayerX), static_cast<int>(world::gPlayerY));
        }
        auto* const indexedElement3 = window.FindElementById(NativeElementKey(TextTargetCoordinate));
        if (indexedElement3) {
            indexedElement3->data.text->text = targetX >= 0
                ? std::format(L"目标：{}  {}", targetX, targetY) : L"";
        }
        std::wstring status = !annotationsLoaded ? L"缺少或无法读取 data/mapdesc.txt" : L"";
        if (missingIcons || failedUiImages)
            status += std::format(L"  缺图：{}  UI资源失败：{}", missingIcons, failedUiImages);
        const auto generation = map.Status();
        if (!generation.empty()) status += L"  " + generation;
        if (auto* const indexedElement4 = window.FindElementById(NativeElementKey(TextStatus)); indexedElement4) indexedElement4->visible = !status.empty();
        if (auto* const indexedElement5 = window.FindElementById(NativeElementKey(StatusArea)); indexedElement5) indexedElement5->visible = !status.empty();
        if (auto* const indexedElement6 = window.FindElementById(NativeElementKey(TextStatus)); indexedElement6) indexedElement6->data.text->text = std::move(status);
    }

    UIClipRect Viewport() const {
        return {static_cast<short>(window.x + map.x), static_cast<short>(window.y + map.y),
            static_cast<short>(window.x + map.x + map.w), static_cast<short>(window.y + map.y + map.h)};
    }

    void LoadAnnotations() {
        coordinateRows.clear();
        listDirty = true;
        scrollingList = false;
        annotations.clear();
        missingIcons = 0;
        // 二进制读保留GBK字节；不使用系统默认编码，不回退旧EXE/内嵌数据。
        std::ifstream input("data/mapdesc.txt", std::ios::binary);
        annotationsLoaded = input.is_open();
        if (!annotationsLoaded) return;
        std::string line;
        while (std::getline(input, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty() || line.front() == '#') continue;
            std::array<std::string, 7> fields;
            std::size_t begin = 0, count = 0;
            for (; count < fields.size(); ++count) {
                const auto end = line.find('\t', begin);
                fields[count] = line.substr(begin, end == std::string::npos ? end : end - begin);
                if (end == std::string::npos) { ++count; break; }
                begin = end + 1;
            }
            // mapdesc首列对应地图文件的楼层号（如1000）；currentMap是路径分组，普通地图为0。
            if (count < 5 || std::atoi(fields[0].c_str()) != currentFloor) continue;
            BigMapAnnotation point;
            point.category = std::atoi(fields[1].c_str());
            if (point.category < 0 || point.category >= static_cast<int>(categories.size())) continue;
            point.x = static_cast<std::int16_t>(std::atoi(fields[2].c_str()));
            point.y = static_cast<std::int16_t>(std::atoi(fields[3].c_str()));
            point.name = text::FromGbkWide(fields[4]);
            point.extra = std::atoi(fields[5].c_str());
            point.placement = static_cast<std::int16_t>(std::atoi(fields[6].c_str()));
            point.hasIcon = point.category == 1 || point.category == 2 || point.category == 3 || point.category == 5;
            if (point.category == 5 && point.extra <= 0) point.extra = 18;
            if (point.hasIcon && point.extra > 0 && point.extra < resource::gBinLoaderGraphicCapacity
                && resource::gGraphicMapNoIndex && resource::gGraphicInfo) {
                const int image = resource::gGraphicMapNoIndex[point.extra];
                if (image >= 0 && image < resource::gMaxGraphicCount && resource::gGraphicInfo[image].mapNo == point.extra) {
                    const auto& info = resource::gGraphicInfo[image];
                    if (info.w > 0 && info.h > 0) {
                        point.imageNo = image;
                        const int extent = std::max(26, std::max<int>(info.w, info.h));
                        point.imageWidth = std::max(1, info.w * 26 / extent);
                        point.imageHeight = std::max(1, info.h * 26 / extent);
                    }
                }
            }
            annotations[{point.x, point.y}] = std::move(point);
        }
        if (input.bad()) { annotations.clear(); annotationsLoaded = false; }
        for (const auto& [position, point] : annotations) {
            if (point.hasIcon && point.imageNo < 0) ++missingIcons;
        }
    }

    bool Hit(int x, int y, int w, int h) const {
        return MouseIsInRect(window.x + x, window.y + y, w, h);
    }

    int Update(int stage) {
        if (world::gCurrentSceneType != SCENE_NORMAL) {
            window.state |= WIN_STATE_CLOSE;
            dragging = movingWindow = mapClickPending = acceptsMouse = false;
            return stage;
        }
        Layout();
        if (currentMap != static_cast<int>(world::gCurrentMapId_9A86DC)
            || currentFloor != world::gMapFloor || currentGroup != world::gMapCachePathKey_9E0434) {
            currentMap = static_cast<int>(world::gCurrentMapId_9A86DC);
            currentFloor = world::gMapFloor;
            currentGroup = world::gMapCachePathKey_9E0434;
            targetX = targetY = -1;
            dragging = movingWindow = mapClickPending = false;
            followPlayer = true;

            LoadAnnotations();
        }
        if (followPlayer) {
            map.mapX = static_cast<int>(world::gPlayerX);
            map.mapY = static_cast<int>(world::gPlayerY);
        }
        mapReady = map.PrepareMap();
        if (listDirty || listMapWidth != map.mapWidth || listMapHeight != map.mapHeight) RebuildCoordinateList();
        SyncControls();
        const int mx = static_cast<int>(gMouseX), my = static_cast<int>(gMouseY);
        const bool held = (gMouseBtnDown2 & 1) != 0;
        const bool available = gWindowHasFocus == 0 && ui::WindowDrag_ReadMousePhase() == 0
            && platform::gWindowActiveState == 2 && platform::IsClientWindowForeground();
        acceptsMouse = available && Hit(0, 0, window.width, window.height);
        if (listOpen && acceptsMouse && ElementHit(ListPanelArea)) {
            const int wheel = LuaPeekMouseWheelDelta();
            if (wheel != 0) {
                const int step = wheel / 120 != 0 ? wheel / 120 : wheel > 0 ? 1 : -1;
                SetListFirstRow(listFirstRow - step);
                LuaConsumeMouseWheelDelta();
            }
        }
        RefreshText();
        if (!acceptsMouse || (gMouseClickState & 2)) mapClickPending = false;
        if (scrollingList) {
            const bool heldScroll = available && held && listOpen && MaxListFirstRow() > 0;
            WindowElement_HandleVerticalScroll(ListScrollTrack, heldScroll ? 8 : 0);
            auto* const indexedElement = window.FindElementById(NativeElementKey(ListScrollTrack));
            if (indexedElement) {
                SetListFirstRow(WindowScroll_ReadPosition(indexedElement, MaxListFirstRow()));
            }
            scrollingList = heldScroll;
            acceptsMouse = false; // 捕获帧由上面统一处理，防止其它子控件再次响应。
        } else if (movingWindow) {
            if (available && held) {
                window.x = dragWindowX + mx - dragX;
                window.y = dragWindowY + my - dragY;
                Layout();
            } else movingWindow = false;
        } else if (dragging) {
            // 原生元素事件的0x100为双击；留给地图元素，优先于第二次按下的拖动捕获。
            if (acceptsMouse && mapClickPending && (gMouseClickState & 0x10) && MapHit()) {
                dragging = false;
                return stage;
            }
            if (available && held) {
                const int dx = mx - dragX, dy = my - dragY;
                if (std::abs(dx) > 3 || std::abs(dy) > 3) dragMoved = true;
                if (dragMoved) {
                    mapClickPending = false;
                    followPlayer = false;
                    map.PanFrom(dragMapX, dragMapY, dx, dy);
                }
            } else dragging = false;
        } else return stage;
        // 捕获期间及松开当帧，原生元素分发和后续窗口均不再接收尾部事件。
        gMouseEventState = MES_CAPTURED;
        gWindowHasFocus = 1;
        return stage;
    }

    int OnElement(int index, int events) {
        auto* const elementPtr = window.FindElementById(NativeElementKey(index));
        if (!elementPtr) { return 0; }
        auto& element = (*elementPtr);
        element.state = acceptsMouse ? static_cast<std::uint32_t>(events) : 0;
        if (index < ButtonEnd) {
            const bool hover = (element.state & 1) != 0, pressed = hover && (element.state & 8) != 0;
            int imageNo = gBigMapElements[index].source.graphic->imageNo;
            switch (static_cast<BigMapButton>(index - ButtonFirst)) {
            case BigMapButton::Toggle:
                imageNo = options ? (pressed ? 243031 : hover ? 243032 : 243030)
                    : (pressed ? 243034 : hover ? 243035 : 243033);
                break;
            case BigMapButton::Close: if (pressed) imageNo = 22510; break;
            case BigMapButton::Walk: imageNo = pressed ? WalkPressed : hover ? WalkHover : WalkNormal; break;
            case BigMapButton::Center: imageNo = pressed ? CenterPressed : hover ? CenterHover : CenterNormal; break;
            case BigMapButton::CoordinateList:
                imageNo = pressed ? CenterPressed : hover ? CenterHover : CenterNormal;
                break;
            }
            element.data.graphic->imageNo = imageNo;
        }
        if (index == ListToggleTextArea && acceptsMouse && (events & 1)) {
            const int button = ButtonFirst + static_cast<int>(BigMapButton::CoordinateList);
            auto* const indexedElement = window.FindElementById(NativeElementKey(button));
            if (indexedElement) {
                indexedElement->data.graphic->imageNo =
                    (events & 8) ? CenterPressed : CenterHover;
            }
        }
        if (index >= ListScrollUp && index <= ListNextPage) {
            const int base = gBigMapElements[index].source.graphic->imageNo;
            auto& graphic = *element.data.graphic;
            graphic.imageNo = base + ((element.state & 8) ? 1 : (element.state & 1) ? 2 : 0);
            const bool atEnd = (index == ListScrollUp || index == ListPreviousPage)
                ? listFirstRow == 0 : listFirstRow == MaxListFirstRow();
            graphic.color = atEnd ? 0x80FFFFFFu : 0xFFFFFFFFu;
        }
        if (!acceptsMouse || (window.state & WIN_STATE_CLOSE) || !(events & 1)) return 0;
        if (index == ListScrollTrack) {
            if ((events & 8) && MaxListFirstRow() > 0) {
                scrollingList = true;
                mapClickPending = false;
                WindowElement_HandleVerticalScroll(index, events);
                SetListFirstRow(WindowScroll_ReadPosition(&element, MaxListFirstRow()));
            }
            return 1;
        }
        if (index != MapArea) {
            if (events & (2 | 4)) mapClickPending = false;
            if (index >= ListRowFirst && index < ListRowEnd) {
                const int position = listFirstRow + index - ListRowFirst;
                if (mapReady && position >= 0 && position < static_cast<int>(coordinateRows.size())
                    && (events & (2 | 0x100))) {
                    const auto& point = *coordinateRows[position];
                    const bool doubleClick = (events & 0x100) && lastListClickX == point.x && lastListClickY == point.y;
                    const bool retarget = getAutoCopliotState() && (targetX != point.x || targetY != point.y);
                    targetX = point.x; targetY = point.y;
                    followPlayer = false;
                    // 列表保持展开，目标居中到其右侧可见地图，避免标记落在面板后面。
                    auto* const panelElement = window.FindElementById(NativeElementKey(ListPanelArea));
                    if (panelElement) {
                        const auto& panel = (*panelElement);
                        const int visibleLeft = std::clamp(panel.x + panel.width, map.x, map.x + map.w);
                        const int visibleCenterX = (visibleLeft + map.x + map.w) / 2;
                        map.PanFrom(point.x, point.y, visibleCenterX - (map.x + map.w / 2), 0);
                    }
                    dragging = movingWindow = false;
                    if (doubleClick || retarget) startCopilot(targetX, targetY);
                    lastListClickX = point.x; lastListClickY = point.y;
                    SyncControls();
                    RefreshText();
                }
                return 1;
            }
            if (index >= ListScrollUp && index <= ListNextPage) {
                if (events & 0x400) {
                    if (index == ListScrollUp) SetListFirstRow(listFirstRow - 1);
                    else if (index == ListScrollDown) SetListFirstRow(listFirstRow + 1);
                    else if (index == ListPreviousPage) SetListFirstRow(((std::max(1, listFirstRow) - 1) / kCoordinateRows) * kCoordinateRows);
                    else SetListFirstRow((listFirstRow / kCoordinateRows + 1) * kCoordinateRows);
                }
                return 1;
            }
            if (events & 2) {
                lastListClickX = lastListClickY = -1;
                if (index < ButtonEnd) {
                    switch (static_cast<BigMapButton>(index - ButtonFirst)) {
                    case BigMapButton::Close: window.state |= WIN_STATE_CLOSE; break;
                    case BigMapButton::Toggle:
                        options = !options;
                        if (options) listOpen = false;
                        element.data.graphic->imageNo = options ? 243031 : 243034;
                        break;
                    case BigMapButton::CoordinateList: ToggleCoordinateList(); break;
                    case BigMapButton::Center: followPlayer = true; break;
                    case BigMapButton::Walk:
                        startCopilot(targetX, targetY);
                        break;
                    }
                } else if (index >= CategoryFirst && index < CategoryEnd) {
                    categories[index - CategoryFirst] = !categories[index - CategoryFirst];
                    RebuildCoordinateList();
                } else if (index == ListToggleTextArea) ToggleCoordinateList();
                else if (index == TitleArea) {
                    movingWindow = true;
                    dragging = false;
                    dragX = static_cast<int>(gMouseX); dragY = static_cast<int>(gMouseY);
                    dragWindowX = window.x; dragWindowY = window.y;
                }
            }
            SyncControls();
            RefreshText();
            return 1;
        }
        if (events & (2 | 4)) lastListClickX = lastListClickY = -1;
        if (!mapReady) return 1;
        const auto point = map.ScreenToMap(static_cast<int>(gMouseX) - window.x - map.x,
            static_cast<int>(gMouseY) - window.y - map.y);
        const bool valid = point.first >= 0 && point.second >= 0
            && point.first < map.mapWidth && point.second < map.mapHeight;
        if ((events & 0x100) && mapClickPending) {
            mapClickPending = false;
            dragging = false;
            if (valid) {
                targetX = point.first; targetY = point.second;
                startCopilot(targetX, targetY);
            }
        } else if (events & 2) {
            mapClickPending = true;
            dragging = true;
            dragMoved = false;
            dragX = static_cast<int>(gMouseX); dragY = static_cast<int>(gMouseY);
            dragMapX = map.mapX; dragMapY = map.mapY;
        }
        if ((events & 4) && valid) {
            mapClickPending = false;
            const bool retarget = getAutoCopliotState() && (targetX != point.first || targetY != point.second);
            targetX = point.first; targetY = point.second;
            if (retarget) {
                startCopilot(targetX, targetY);
            }
        }
        SyncControls();
        RefreshText();
        return 1;
    }

    // 左上角绘制：导入图片带中心偏移，这里明确不叠加GraphicInfo.x/y。
    void ImageAt(int mapNo, int x, int y, int width, int height, const UIClipRect& clip,
        std::uint32_t color = 0xFFFFFFFFu) {
        if (!UIRectIntersectsClipRect(x, y, width, height, &clip)) return;
        const auto* graphic = Graphic(mapNo);
        const render::PngRectangle imageClip{clip.left, clip.top, clip.right, clip.bottom};
        if (!graphic || render::DrawGraphicImage(graphic->seqNo, x, y, width, height,
            LAYER_UI, &imageClip, color) < 0) ++failedUiImages;
    }

    void UiImage(int mapNo, int x, int y, int width, int height, const UIClipRect& clip,
        std::uint32_t color = 0xFFFFFFFFu) {
        ImageAt(mapNo, window.x + x, window.y + y, width, height, clip, color);
    }

    void DrawElements(const UIClipRect& clip) {
        WindowElementCursor cursor(window);
        while (auto* current = cursor.next()) {
            const auto i = current->nativeSlot;
            if (i < ButtonFirst) continue;
            const auto& element = *current;
            if (element.visible != 1) continue;
            if (element.type == WindowElementType_Graphic) {
                const auto& data = *element.data.graphic;
                ImageAt(data.imageNo, window.x + element.x, window.y + element.y,
                    element.width, element.height, clip, data.color);
            } else if (element.type == WindowElementType_Text) {
                const auto& data = *element.data.text;
                auto textClip = clip;
                int textX = window.x + element.x;
                if (i == TextMapName) textClip.right = static_cast<short>(window.x + 188);
                if ((i >= ListNameFirst && i < TextListToggle) || i == TextListEmpty || i == TextListPage) {
                    textClip.left = static_cast<short>(std::max<int>(clip.left, textX));
                    textClip.right = static_cast<short>(std::min<int>(clip.right, textX + element.width));
                    textClip.top = static_cast<short>(std::max<int>(clip.top, window.y + element.y));
                    textClip.bottom = static_cast<short>(std::min<int>(clip.bottom, window.y + element.y + element.height));
                    if (i >= ListCoordinateFirst && i < TextListToggle)
                        textX += std::max(0, element.width - render::TextMeasure(data.text, std::to_integer<char>(data.style.font)));
                }
                RendererDrawTextClipped(static_cast<short>(textX),
                    static_cast<short>(window.y + element.y), FONT_LAYER_WINDOW, std::to_integer<char>(data.style.font),
                    std::to_integer<std::uint8_t>(data.style.color), data.text, 0, 0, &textClip);
            }
        }
    }

    void DrawPanel(const UIClipRect& clip) {
        // 4.06@4774CB：3列×5行、步长(74,54)；面板下移21px，为两个入口保留独立一行。
        // 边框先入队，底纹后入队，适配本线同层逆序绘制。
        for (int row = 0; row < 5; ++row) {
            for (int col = 0; col < 3; ++col) {
                const int mapNo = row == 0 ? 245380 + col : row == 4 ? 245386 + col
                    : col == 0 ? 245383 : col == 2 ? 245385 : 0;
                if (mapNo) {
                    if (const auto* graphic = Graphic(mapNo))
                        UiImage(mapNo, 73 + col * 74 + graphic->x, 100 + row * 54 + graphic->y, graphic->w, graphic->h, clip);
                    else ++failedUiImages;
                }
                UiImage(245384, 36 + col * 74, 73 + row * 54, 76, 54, clip);
            }
        }
    }

    void DrawCoordinateList(const UIClipRect& clip) {
        for (int row = 0; row < kCoordinateRows; ++row) {
            auto* const elementPtr = window.FindElementById(NativeElementKey(ListRowFirst + row));
            if (elementPtr) {
                const auto& element = (*elementPtr);
                const int position = listFirstRow + row;
                if (!element.visible || position >= static_cast<int>(coordinateRows.size())) continue;
                const auto& point = *coordinateRows[position];
                if ((element.state & 1) || (targetX == point.x && targetY == point.y))
                    RendererDrawRectClipped(static_cast<short>(window.x + element.x),
                        static_cast<short>(window.y + element.y), static_cast<short>(element.width),
                        static_cast<short>(element.height), LAYER_UI, 0x803F6E83u, &clip);
            }
        }
        auto* const trackElement = window.FindElementById(NativeElementKey(ListScrollTrack));
        if (trackElement) {
            const auto& track = (*trackElement);
            if (track.visible)
                RendererDrawRectClipped(static_cast<short>(window.x + track.x), static_cast<short>(window.y + track.y),
                    static_cast<short>(track.width), static_cast<short>(track.height), LAYER_UI, 0x80000000u, &clip);
        }
        auto* const panelElement = window.FindElementById(NativeElementKey(ListPanelArea));
        if (panelElement) {
            const auto& panel = (*panelElement);
            yobi::DrawNineGrid_49BE00(245300, static_cast<short>(window.x + panel.x), static_cast<short>(window.y + panel.y),
                static_cast<short>(panel.width), static_cast<short>(panel.height), LAYER_UI, -1,
                std::bit_cast<std::int32_t>(0x80FFFFFFu), 74, 54);
        }
        return;
    }

    void Marker(int gx, int gy, int mapNo, int size, const UIClipRect& clip) {
        const auto [x, y] = map.MapToScreen(gx, gy);
        ImageAt(mapNo, clip.left + x - size / 2, clip.top + y - size / 2, size, size, clip);
    }

    // 4.06@43869F..438877：图标气泡相对格点偏移36/44，连接线按分类染色。
    int DrawAnnotations(const UIClipRect& clip) {
        int failedIcons = 0;
        const render::PngRectangle imageClip{clip.left, clip.top, clip.right, clip.bottom};
        const int fontHeight = render::GetFontMetrics(kMapFont).fontHeight;
        for (const auto& [position, point] : annotations) {
            if (!categories[point.category] || point.x < 0 || point.y < 0 || point.x >= map.mapWidth || point.y >= map.mapHeight
                || (point.category != 4 && !map.IsExplored(point.x, point.y))) continue;
            auto [x, y] = map.MapToScreen(point.x, point.y);
            x += clip.left; y += clip.top;
            const int textWidth = render::TextMeasure(point.name, kMapFont);
            if (point.hasIcon) {
                int cx = x, cy = y - 36;
                if (point.placement == 1) cy = y + 36;
                else if (point.placement == 2) { cx = x - 44; cy = y; }
                else if (point.placement == 3) { cx = x + 44; cy = y; }
                const int nameY = cy + (point.extra == 18 ? -10 : 6);
                RendererDrawTextClipped(static_cast<short>(cx - textWidth / 2), static_cast<short>(nameY),
                    FONT_LAYER_WINDOW, kMapFont, 0, point.name, 0, 0, &clip);
                const auto coordinates = std::format(L"({},{})", point.x, point.y);
                RendererDrawTextClipped(static_cast<short>(cx - render::TextMeasure(coordinates, kMapFont) / 2),
                    static_cast<short>(nameY + 12), FONT_LAYER_WINDOW, kMapFont, 0, coordinates, 0, 0, &clip);
                if (point.imageNo >= 0) {
                    const int w = point.imageWidth, h = point.imageHeight;
                    if (UIRectIntersectsClipRect(cx - w / 2, cy - h / 2, w, h, &clip)
                        && render::DrawGraphicImage(point.imageNo, cx - w / 2, cy - h / 2, w, h,
                            LAYER_UI, &imageClip) < 0) ++failedIcons;
                } else {
                    RendererDrawTextClipped(static_cast<short>(cx - 3), static_cast<short>(cy - fontHeight / 2),
                        FONT_LAYER_WINDOW, kMapFont, 4, L"?", 0, 0, &clip);
                }
                ImageAt(AnnotationBorder, cx - 21, cy - 15, 42, 31, clip);
                ImageAt(AnnotationPlate, cx - 21, cy - 15, 42, 31, clip, kCategoryColors[point.category]);
                if (point.placement == 2 || point.placement == 3)
                    ImageAt(HorizontalConnector, x - (point.placement == 2 ? 26 : 2), y - 2, 28, 5, clip, kCategoryColors[point.category]);
                else ImageAt(VerticalConnector, x - 3, y - (point.placement == 1 ? 2 : 24), 5, 28, clip, kCategoryColors[point.category]);
            } else if (point.category == 6) {
                RendererDrawTextClipped(static_cast<short>(x + 6), static_cast<short>(y - 6),
                    FONT_LAYER_WINDOW, kMapFont, static_cast<std::uint8_t>(point.extra), point.name, 0, 0, &clip);
                ImageAt(NpcDot, x - 1, y - 1, 4, 4, clip,
                    render::GetTextColor(static_cast<std::uint8_t>(point.extra)));
            } else {
                // 地点和村镇原端为文字底条，不使用占位方块。
                const int textY = point.category == 0 && point.placement <= 0 ? y - 20 : y;
                RendererDrawTextClipped(static_cast<short>(x - textWidth / 2), static_cast<short>(textY),
                    FONT_LAYER_WINDOW, kMapFont, point.category == 4 ? 4 : 0, point.name, 0, 0, &clip);
                RendererDrawRectClipped(static_cast<short>(x - textWidth / 2 - 2), static_cast<short>(textY),
                    static_cast<short>(textWidth + 4), static_cast<short>(fontHeight + 2), LAYER_UI,
                    point.category == 4 ? 0x60000000u : 0x40000000u, &clip);
            }
        }
        return failedIcons;
    }

    int Draw(int stage) {
        if (window.state & WIN_STATE_CLOSE) return stage;
        failedUiImages = 0;
        RendererBreakWindowTextBatch();
        const UIClipRect clip{static_cast<short>(window.x), static_cast<short>(window.y),
            static_cast<short>(window.x + window.width), static_cast<short>(window.y + window.height)};
        const auto mapClip = Viewport();
        const int px = static_cast<int>(world::gPlayerX), py = static_cast<int>(world::gPlayerY);
        DrawElements(clip);
        // 同层逆序绘制：底色晚于文字入队。70%不透明度对应Alpha 179。
        for (int index : {CurrentCoordinateArea, TargetCoordinateArea}) {
            auto* const elementPtr = window.FindElementById(NativeElementKey(index));
            if (elementPtr) {
                const auto& element = (*elementPtr);
                if (element.visible == 1)
                    RendererDrawRectClipped(static_cast<short>(window.x + element.x),
                        static_cast<short>(window.y + element.y), static_cast<short>(element.width),
                        static_cast<short>(element.height), LAYER_UI, 0xB3000000u, &clip);
            }
        }
        if (options) {
            for (int i = 0; i < 7; ++i) {
                auto* const elementPtr = window.FindElementById(NativeElementKey(CategoryFirst + i));
                if (elementPtr) {
                    const auto& element = (*elementPtr);
                    if (element.state & 1)
                        RendererDrawRectClipped(static_cast<short>(window.x + element.x),
                            static_cast<short>(window.y + element.y), static_cast<short>(element.width),
                            static_cast<short>(element.height), LAYER_UI, 0x503F6E83u, &clip);
                }
            }
            DrawPanel(clip);
        }
        if (listOpen) DrawCoordinateList(clip);
        RendererBreakWindowTextBatch();
        if (mapReady) {
            if (acceptsMouse && MapHit()) {
                const auto point = map.ScreenToMap(gMouseX - mapClip.left, gMouseY - mapClip.top);
                RendererDrawTextClipped(static_cast<short>(gMouseX + 24), static_cast<short>(gMouseY),
                    FONT_LAYER_WINDOW, kMapFont, 0, std::format(L"{},{}", point.first, point.second), 0, 0, &mapClip);
            }
            Marker(px, py, PlayerMarker, 14, mapClip);
            if (targetX >= 0) Marker(targetX, targetY, TargetMarker, 14, mapClip);
            const auto route = getAutoCopliotRoute();
            for (std::size_t i = 1; i < route.size(); i += 2)
                Marker(route[i].first, route[i].second, RouteDot, 4, mapClip);
            failedUiImages += DrawAnnotations(mapClip);
        } else {
            RendererDrawTextClipped(static_cast<short>(window.x + (window.width - render::TextMeasure(L"当前地图数据未就绪", kMapFont)) / 2),
                static_cast<short>(window.y + window.height / 2), FONT_LAYER_WINDOW, kMapFont, 4, L"当前地图数据未就绪", 0, 0, &clip);
        }
        RendererBreakWindowTextBatch();
        map.Render(window.x, window.y, LAYER_UI, {mapClip.left, mapClip.top, mapClip.right, mapClip.bottom});
        RendererDrawRectClipped(mapClip.left, mapClip.top, static_cast<short>(map.w), static_cast<short>(map.h),
            LAYER_UI, 0xFF000000u, &mapClip);
        // 与WindowDialog一致的245300九宫格、中心透明度和拼块尺寸；同层逆序绘制，背景最后入队。
        yobi::DrawNineGrid_49BE00(245300, static_cast<short>(window.x), static_cast<short>(window.y),
            static_cast<short>(window.width), static_cast<short>(window.height), LAYER_UI, -1,
            std::bit_cast<std::int32_t>(0x80FFFFFFu), 74, 54);
        return stage;
    }

};

// 原生原型只允许一个实例；release在原生元素载荷释放之前结束地图缓存寿命。
static std::unique_ptr<BigMapWindowState> gBigMapState;
static int WindowBigMap_Update(int stage) {
    if (stage == -1) {
        gBigMapState = std::make_unique<BigMapWindowState>(*gCurrentWindow);
        return 1;
    }
    return gBigMapState->Update(stage);
}
static int WindowBigMap_Element(int index, int events) { return gBigMapState->OnElement(index, events); }
static int WindowBigMap_Draw(int stage) { return gBigMapState->Draw(stage); }
static void WindowBigMap_Release() { gBigMapState.reset(); }

} // namespace cg::ui::detail

namespace cg::ui {
void WindowBigMap_Open() {
    if (world::gCurrentSceneType != 9) return;
    if (auto* existing = detail::gWindowStates[WindowId_BigMap].window) {
        existing->state &= ~std::uint32_t{0x8000};
        existing->state |= 0x2000u;
        script::CloseNonDefaultWindows(WindowId_BigMap);
        return;
    }
    if (WindowRuntime_Open(WindowId_BigMap)) {
        script::CloseNonDefaultWindows(WindowId_BigMap);
    }
}
} // namespace cg::ui
