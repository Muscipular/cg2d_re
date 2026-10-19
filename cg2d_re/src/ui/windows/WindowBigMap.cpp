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

namespace cg::ui::detail {

// 复用现有裁剪绘制和寻路接口，不创建Lua或扩展窗口实例。
using namespace cg::script;

static constexpr int kMapFont = 12;
// 原资源ID升序重编号，见BMUI01；查MapNo，不依赖资源组内seqNo。
enum BigMapImage : int {
    RouteDot = 641510, Checked = 641511, PlayerMarker = 641513,
    MapNamePlate = 641514, TargetMarker = 641515, NpcDot = 641516,
    AnnotationPlate = 641518, AnnotationBorder = 641519, VerticalConnector = 641520,
    HorizontalConnector = 641521, Checkbox = 641522,
    WalkNormal = 641524, WalkHover = 641525, WalkPressed = 641526,
    CenterNormal = 641527, CenterHover = 641528, CenterPressed = 641529,
};
enum class BigMapButton { Toggle, Close, Walk, Center };
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
    ButtonFirst = 0, ButtonEnd = ButtonFirst + 4,
    CategoryFirst = ButtonEnd, CategoryEnd = CategoryFirst + 7,
    CurrentCoordinateArea = CategoryEnd, TargetCoordinateArea, TitleArea, NameArea, StatusArea, OptionsArea, MapArea,
    TextFirst, TextMapName = TextFirst, TextCurrentCoordinate, TextTargetCoordinate, TextToggle, TextWalk,
    TextCenter, TextOptions, TextPlayer, TextTarget, TextDisplay, TextStatus,
    CategoryTextFirst, TextEnd = CategoryTextFirst + 7,
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
static WindowGraphicData gBigMapMapNameGraphic{MapNamePlate, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapPlayerGraphic{PlayerMarker, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapTargetGraphic{TargetMarker, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowGraphicData gBigMapCheckedGraphic{Checked, 0, 0, 0, 0, 0xFFFF0000u};
static WindowGraphicData gBigMapCheckboxGraphic{Checkbox, 0, 0, 0, 0, 0xFFFFFFFFu};
static WindowTextPrototype gBigMapMapNameText{{std::byte{12}, std::byte{12}, {}, {}}, L""};
static WindowTextPrototype gBigMapCoordinateText{{std::byte{12}, std::byte{12}, {}, {}}, L""};
static WindowTextPrototype gBigMapOptionsText{{std::byte{4}, std::byte{12}, {}, {}}, L"地图选项"};
static WindowTextPrototype gBigMapWalkText{{std::byte{4}, std::byte{12}, {}, {}}, L"开始寻路"};
static WindowTextPrototype gBigMapCenterText{{std::byte{4}, std::byte{12}, {}, {}}, L"玩家居中"};
static WindowTextPrototype gBigMapPlayerText{{std::byte{0}, std::byte{12}, {}, {}}, L"你现在的位置"};
static WindowTextPrototype gBigMapTargetText{{std::byte{0}, std::byte{12}, {}, {}}, L"右键点选的寻路坐标"};
static WindowTextPrototype gBigMapDisplayText{{std::byte{4}, std::byte{12}, {}, {}}, L"显示选项"};
static WindowTextPrototype gBigMapStatusText{{std::byte{12}, std::byte{12}, {}, {}}, L""};
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
    {WindowElementType_Graphic, 11, 51, 20, 18, 1, {.graphic = &gBigMapToggleGraphic}, &WindowBigMap_Element}, // Toggle
    {WindowElementType_Graphic, 614, 6, 20, 20, 1, {.graphic = &gBigMapCloseGraphic}, &WindowBigMap_Element}, // Close
    {WindowElementType_Graphic, 11, 73, 18, 18, 0, {.graphic = &gBigMapWalkGraphic}, &WindowBigMap_Element}, // Walk
    {WindowElementType_Graphic, 11, 73, 18, 18, 0, {.graphic = &gBigMapCenterGraphic}, &WindowBigMap_Element}, // Center
    {WindowElementType_EventArea, 42, 138, 194, 20, 0, {}, &WindowBigMap_Element}, // 分类0
    {WindowElementType_EventArea, 42, 158, 194, 20, 0, {}, &WindowBigMap_Element}, // 分类1
    {WindowElementType_EventArea, 42, 178, 194, 20, 0, {}, &WindowBigMap_Element}, // 分类2
    {WindowElementType_EventArea, 42, 198, 194, 20, 0, {}, &WindowBigMap_Element}, // 分类3
    {WindowElementType_EventArea, 42, 218, 194, 20, 0, {}, &WindowBigMap_Element}, // 分类4
    {WindowElementType_EventArea, 42, 238, 194, 20, 0, {}, &WindowBigMap_Element}, // 分类5
    {WindowElementType_EventArea, 42, 258, 194, 20, 0, {}, &WindowBigMap_Element}, // 分类6
    {WindowElementType_EventArea, 516, 12, 60, 16, 1, {}, &WindowBigMap_Element}, // 当前坐标底色与遮挡
    {WindowElementType_EventArea, 516, 28, 60, 16, 0, {}, &WindowBigMap_Element}, // 目标坐标底色与遮挡
    {WindowElementType_EventArea, 6, 6, 602, 32, 1, {}, &WindowBigMap_Element}, // TitleArea
    {WindowElementType_EventArea, 10, 10, 184, 38, 1, {}, &WindowBigMap_Element}, // NameArea
    {WindowElementType_EventArea, 8, 440, 624, 32, 1, {}, &WindowBigMap_Element}, // StatusArea
    {WindowElementType_EventArea, 36, 52, 224, 270, 0, {}, &WindowBigMap_Element}, // OptionsArea
    {WindowElementType_EventArea, 8, 8, 624, 464, 1, {}, &WindowBigMap_Element}, // MapArea
    {WindowElementType_Text, 16, 20, 0, 0, 1, {.text = &gBigMapMapNameText}, &WindowElement_NoOp}, // TextMapName
    {WindowElementType_Text, 516, 12, 0, 0, 1, {.text = &gBigMapCoordinateText}, &WindowElement_NoOp}, // TextCurrentCoordinate
    {WindowElementType_Text, 516, 28, 0, 0, 0, {.text = &gBigMapCoordinateText}, &WindowElement_NoOp}, // TextTargetCoordinate
    {WindowElementType_Text, 34, 52, 0, 0, 1, {.text = &gBigMapOptionsText}, &WindowElement_NoOp}, // TextToggle
    {WindowElementType_Text, 34, 76, 0, 0, 0, {.text = &gBigMapWalkText}, &WindowElement_NoOp}, // TextWalk
    {WindowElementType_Text, 34, 76, 0, 0, 0, {.text = &gBigMapCenterText}, &WindowElement_NoOp}, // TextCenter
    {WindowElementType_Text, 44, 60, 0, 0, 0, {.text = &gBigMapOptionsText}, &WindowElement_NoOp}, // TextOptions
    {WindowElementType_Text, 62, 79, 0, 0, 0, {.text = &gBigMapPlayerText}, &WindowElement_NoOp}, // TextPlayer
    {WindowElementType_Text, 62, 99, 0, 0, 0, {.text = &gBigMapTargetText}, &WindowElement_NoOp}, // TextTarget
    {WindowElementType_Text, 44, 120, 0, 0, 0, {.text = &gBigMapDisplayText}, &WindowElement_NoOp}, // TextDisplay
    {WindowElementType_Text, 12, 462, 0, 0, 1, {.text = &gBigMapStatusText}, &WindowElement_NoOp}, // TextStatus
    {WindowElementType_Text, 62, 140, 0, 0, 0, {.text = &gBigMapCategoryTexts[0]}, &WindowElement_NoOp}, // 分类0名称
    {WindowElementType_Text, 62, 160, 0, 0, 0, {.text = &gBigMapCategoryTexts[1]}, &WindowElement_NoOp}, // 分类1名称
    {WindowElementType_Text, 62, 180, 0, 0, 0, {.text = &gBigMapCategoryTexts[2]}, &WindowElement_NoOp}, // 分类2名称
    {WindowElementType_Text, 62, 200, 0, 0, 0, {.text = &gBigMapCategoryTexts[3]}, &WindowElement_NoOp}, // 分类3名称
    {WindowElementType_Text, 62, 220, 0, 0, 0, {.text = &gBigMapCategoryTexts[4]}, &WindowElement_NoOp}, // 分类4名称
    {WindowElementType_Text, 62, 240, 0, 0, 0, {.text = &gBigMapCategoryTexts[5]}, &WindowElement_NoOp}, // 分类5名称
    {WindowElementType_Text, 62, 260, 0, 0, 0, {.text = &gBigMapCategoryTexts[6]}, &WindowElement_NoOp}, // 分类6名称
    {WindowElementType_Graphic, 10, 10, 184, 38, 1, {.graphic = &gBigMapMapNameGraphic}, &WindowElement_NoOp}, // MapNameImage
    {WindowElementType_Graphic, 44, 80, 14, 14, 0, {.graphic = &gBigMapPlayerGraphic}, &WindowElement_NoOp}, // PlayerLegendImage
    {WindowElementType_Graphic, 44, 100, 14, 14, 0, {.graphic = &gBigMapTargetGraphic}, &WindowElement_NoOp}, // TargetLegendImage
    {WindowElementType_Graphic, 46, 141, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp}, // 分类0选中标记
    {WindowElementType_Graphic, 46, 161, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp}, // 分类1选中标记
    {WindowElementType_Graphic, 46, 181, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp}, // 分类2选中标记
    {WindowElementType_Graphic, 46, 201, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp}, // 分类3选中标记
    {WindowElementType_Graphic, 46, 221, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp}, // 分类4选中标记
    {WindowElementType_Graphic, 46, 241, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp}, // 分类5选中标记
    {WindowElementType_Graphic, 46, 261, 12, 11, 0, {.graphic = &gBigMapCheckedGraphic}, &WindowElement_NoOp}, // 分类6选中标记
    {WindowElementType_Graphic, 46, 143, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp}, // 分类0选择框
    {WindowElementType_Graphic, 46, 163, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp}, // 分类1选择框
    {WindowElementType_Graphic, 46, 183, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp}, // 分类2选择框
    {WindowElementType_Graphic, 46, 203, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp}, // 分类3选择框
    {WindowElementType_Graphic, 46, 223, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp}, // 分类4选择框
    {WindowElementType_Graphic, 46, 243, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp}, // 分类5选择框
    {WindowElementType_Graphic, 46, 263, 9, 9, 0, {.graphic = &gBigMapCheckboxGraphic}, &WindowElement_NoOp}, // 分类6选择框
};
static_assert(std::size(gBigMapElements) == ElementCount);

// 单实例原型；state bit1禁止通用拖动，标题事件区域负责窗口拖动。
WindowPrototype gWindowPrototypeBigMap{2, 4, 0, 0, 640, 480, 0, ElementCount,
    gBigMapElements, &WindowBigMap_Update, &WindowBigMap_Draw, &WindowBigMap_Release};

struct BigMapWindowState final {
    WindowInstance& window; // 借用原生实例，release回调先销毁本业务状态。
    BigMapSurface map;
    std::map<std::pair<int, int>, BigMapAnnotation> annotations;
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

    explicit BigMapWindowState(WindowInstance& instance) : window(instance) { Layout(); }

    void Layout() {
        const int screenW = render::GetRendererLogicalWidth(), screenH = render::GetRendererLogicalHeight();
        window.x = std::clamp(window.x, 0, std::max(0, screenW - window.width));
        window.y = std::clamp(window.y, 0, std::max(0, screenH - window.height));
        const auto& viewport = window.elements[MapArea];
        map.x = viewport.x; map.y = viewport.y;
        map.w = viewport.width; map.h = viewport.height;
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
        const auto& element = window.elements[index];
        return element.visible == 1 && Hit(element.x, element.y, element.width, element.height);
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
        window.elements[walk].visible = targetX >= 0 && mapReady;
        window.elements[center].visible = !followPlayer;
        window.elements[center].y = targetX >= 0 ? 97 : 73;
        window.elements[TextCenter].y = window.elements[center].y + 3;
        window.elements[TextToggle].visible = !options;
        window.elements[TextWalk].visible = !options && window.elements[walk].visible;
        window.elements[TextCenter].visible = !options && window.elements[center].visible;
        window.elements[OptionsArea].visible = options;
        window.elements[TextTargetCoordinate].visible = targetX >= 0;
        window.elements[TargetCoordinateArea].visible = targetX >= 0;
        for (int i : {TextOptions, TextPlayer, TextTarget, TextDisplay,
                PlayerLegendImage, TargetLegendImage}) window.elements[i].visible = options;
        for (int i = 0; i < 7; ++i) {
            window.elements[CategoryFirst + i].visible = options;
            window.elements[CategoryTextFirst + i].visible = options;
            window.elements[CheckedFirst + i].visible = options && categories[i];
            window.elements[CheckboxFirst + i].visible = options;
        }
    }

    void RefreshText() {
        static_cast<WindowTextData*>(window.elements[TextMapName].data)->text = world::GetMapName_9DAD30();
        static_cast<WindowTextData*>(window.elements[TextCurrentCoordinate].data)->text =
            std::format(L"当前：{}, {}", static_cast<int>(world::gPlayerX), static_cast<int>(world::gPlayerY));
        static_cast<WindowTextData*>(window.elements[TextTargetCoordinate].data)->text = targetX >= 0
            ? std::format(L"目标：{} {}", targetX, targetY) : L"";
        std::wstring status = !annotationsLoaded ? L"缺少或无法读取 data/mapdesc.txt" : L"";
        if (missingIcons || failedUiImages)
            status += std::format(L"  缺图：{}  UI资源失败：{}", missingIcons, failedUiImages);
        const auto generation = map.Status();
        if (!generation.empty()) status += L"  " + generation;
        window.elements[TextStatus].visible = !status.empty();
        window.elements[StatusArea].visible = !status.empty();
        static_cast<WindowTextData*>(window.elements[TextStatus].data)->text = std::move(status);
    }

    UIClipRect Viewport() const {
        return {static_cast<short>(window.x + map.x), static_cast<short>(window.y + map.y),
            static_cast<short>(window.x + map.x + map.w), static_cast<short>(window.y + map.y + map.h)};
    }

    void LoadAnnotations() {
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
        SyncControls();
        const int mx = static_cast<int>(gMouseX), my = static_cast<int>(gMouseY);
        const bool held = (gMouseBtnDown2 & 1) != 0;
        const bool available = gWindowHasFocus == 0 && ui::WindowDrag_ReadMousePhase() == 0
            && platform::gWindowActiveState == 2 && platform::IsClientWindowForeground();
        acceptsMouse = available && Hit(0, 0, window.width, window.height);
        RefreshText();
        if (!acceptsMouse || (gMouseClickState & 2)) mapClickPending = false;
        if (movingWindow) {
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
        auto& element = window.elements[index];
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
            }
            static_cast<WindowGraphicData*>(element.data)->imageNo = imageNo;
        }
        if (!acceptsMouse || (window.state & WIN_STATE_CLOSE) || !(events & 1)) return 0;
        if (index != MapArea) {
            if (events & (2 | 4)) mapClickPending = false;
            if (events & 2) {
                if (index < ButtonEnd) {
                    switch (static_cast<BigMapButton>(index - ButtonFirst)) {
                    case BigMapButton::Close: window.state |= WIN_STATE_CLOSE; break;
                    case BigMapButton::Toggle:
                        options = !options;
                        static_cast<WindowGraphicData*>(element.data)->imageNo = options ? 243031 : 243034;
                        break;
                    case BigMapButton::Center: followPlayer = true; break;
                    case BigMapButton::Walk:
                        startCopilot(targetX, targetY);
                        break;
                    }
                } else if (index < CategoryEnd) categories[index - CategoryFirst] = !categories[index - CategoryFirst];
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
        for (int i = ButtonFirst; i < ElementCount; ++i) {
            const auto& element = window.elements[i];
            if (element.visible != 1) continue;
            if (element.type == WindowElementType_Graphic) {
                const auto& data = *static_cast<const WindowGraphicData*>(element.data);
                ImageAt(data.imageNo, window.x + element.x, window.y + element.y,
                    element.width, element.height, clip, data.color);
            } else if (element.type == WindowElementType_Text) {
                const auto& data = *static_cast<const WindowTextData*>(element.data);
                auto textClip = clip;
                if (i == TextMapName) textClip.right = static_cast<short>(window.x + 188);
                RendererDrawTextClipped(static_cast<short>(window.x + element.x),
                    static_cast<short>(window.y + element.y), FONT_LAYER_WINDOW, kMapFont,
                    std::to_integer<std::uint8_t>(data.style.color), data.text, 0, 0, &textClip);
            }
        }
    }

    void DrawPanel(const UIClipRect& clip) {
        // 4.06@4774CB：3列×5行，锚点(73,79)，步长(74,54)。
        // 边框先入队，底纹后入队，适配本线同层逆序绘制。
        for (int row = 0; row < 5; ++row) {
            for (int col = 0; col < 3; ++col) {
                const int mapNo = row == 0 ? 245380 + col : row == 4 ? 245386 + col
                    : col == 0 ? 245383 : col == 2 ? 245385 : 0;
                if (mapNo) {
                    if (const auto* graphic = Graphic(mapNo))
                        UiImage(mapNo, 73 + col * 74 + graphic->x, 79 + row * 54 + graphic->y, graphic->w, graphic->h, clip);
                    else ++failedUiImages;
                }
                UiImage(245384, 36 + col * 74, 52 + row * 54, 76, 54, clip);
            }
        }
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
            const auto& element = window.elements[index];
            if (element.visible == 1)
                RendererDrawRectClipped(static_cast<short>(window.x + element.x),
                    static_cast<short>(window.y + element.y), static_cast<short>(element.width),
                    static_cast<short>(element.height), LAYER_UI, 0xB3000000u, &clip);
        }
        if (options) {
            for (int i = 0; i < 7; ++i) {
                const auto& element = window.elements[CategoryFirst + i];
                if (element.state & 1)
                    RendererDrawRectClipped(static_cast<short>(window.x + element.x),
                        static_cast<short>(window.y + element.y), static_cast<short>(element.width),
                        static_cast<short>(element.height), LAYER_UI, 0x503F6E83u, &clip);
            }
            DrawPanel(clip);
        }
        RendererBreakWindowTextBatch();
        if (mapReady) {
            if (acceptsMouse && MapHit()) {
                const auto point = map.ScreenToMap(gMouseX - mapClip.left, gMouseY - mapClip.top);
                RendererDrawTextClipped(static_cast<short>(gMouseX + 14), static_cast<short>(gMouseY + 14),
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
        return;
    }
    static_cast<void>(WindowRuntime_Open(WindowId_BigMap));
}
} // namespace cg::ui
