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
    MapNamePlate = 641514, TargetMarker = 641515, NpcDot = 641516, CoordinatePlate = 641517,
    AnnotationPlate = 641518, AnnotationBorder = 641519, VerticalConnector = 641520,
    HorizontalConnector = 641521, Checkbox = 641522, Compass = 641523,
    WalkNormal = 641524, WalkHover = 641525, WalkPressed = 641526,
    CenterNormal = 641527, CenterHover = 641528, CenterPressed = 641529,
};
enum class BigMapButton { Toggle, Close, Refresh, Walk, Center };
struct BigMapButtonStyle { int x, y, w, h, normal, hover, pressed; bool visible; };
// 4.06@49A528/2C/30/38：仅图标分类1、2、3、5消费该颜色。
static constexpr std::array<std::uint32_t, 7> kCategoryColors{
    0xFFFFFFFFu, 0xF05C20FFu, 0xF0F8F8F8u, 0xF0F84AF8u, 0xFFFFFFFFu, 0xF0FFC500u, 0xFFFFFFFFu};
static constexpr std::array<const wchar_t*, 7> kCategoryNames{
    L"小的地点或居所", L"伐木地点", L"挖掘地点", L"狩猎地点",
    L"村镇大地点", L"宠物Lv1坐标", L"特定NPC"};

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
    TitleArea = CategoryEnd, NameArea, CompassArea, StatusArea, OptionsArea, MapArea,
    TextFirst, TextTitle = TextFirst, TextMapName, TextX, TextY, TextToggle, TextWalk,
    TextCenter, TextOptions, TextPlayer, TextTarget, TextDisplay, TextRefresh, TextHint, TextStatus,
    CategoryTextFirst, TextEnd = CategoryTextFirst + 7,
    MapNameImage = TextEnd, CoordinateLeftImage, CoordinateRightImage, CompassImage,
    PlayerLegendImage, TargetLegendImage, CheckedFirst, CheckboxFirst = CheckedFirst + 7,
    ElementCount = CheckboxFirst + 7,
};
static int WindowBigMap_Update(int stage);
static int WindowBigMap_Draw(int stage);
static int WindowBigMap_Element(int index, int events);
static void WindowBigMap_Release();
static WindowGraphicData gBigMapGraphic{};
static WindowTextPrototype gBigMapText{{std::byte{0}, std::byte{12}, {}, {}}, L""};
static std::array<WindowElementPrototype, ElementCount> gBigMapElements = [] {
    std::array<WindowElementPrototype, ElementCount> elements{};
    for (int i = 0; i < ElementCount; ++i) {
        auto& element = elements[i];
        element.visible = 1;
        element.mouseCallback = i < TextFirst ? &WindowBigMap_Element : &WindowElement_NoOp;
        if (i < ButtonEnd || i >= TextEnd) {
            element.type = WindowElementType_Graphic;
            element.source.graphic = &gBigMapGraphic;
        } else if (i >= TextFirst) {
            element.type = WindowElementType_Text;
            element.source.text = &gBigMapText;
        } else element.type = WindowElementType_EventArea;
    }
    return elements;
}();

// 单实例原型；state bit1禁止通用拖动，标题事件区域负责窗口拖动。
WindowPrototype gWindowPrototypeBigMap{2, 4, 0, 0, 640, 480, 0, ElementCount,
    gBigMapElements.data(), &WindowBigMap_Update, &WindowBigMap_Draw, &WindowBigMap_Release};

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
    std::wstring routeStatus;

    explicit BigMapWindowState(WindowInstance& instance) : window(instance) { Layout(); }

    void Layout() {
        const int screenW = render::GetRendererLogicalWidth(), screenH = render::GetRendererLogicalHeight();
        window.x = std::clamp(window.x, 0, std::max(0, screenW - window.width));
        window.y = std::clamp(window.y, 0, std::max(0, screenH - window.height));
        map.x = 8; map.y = 8;
        map.w = window.width - 16;
        map.h = window.height - 16;
    }

    static const resource::GraphicInfo* Graphic(int mapNo) {
        if (mapNo <= 0 || mapNo >= resource::gBinLoaderGraphicCapacity
            || !resource::gGraphicMapNoIndex || !resource::gGraphicInfo) return nullptr;
        const int index = resource::gGraphicMapNoIndex[mapNo];
        if (index < 0 || index >= resource::gMaxGraphicCount) return nullptr;
        const auto& info = resource::gGraphicInfo[index];
        return info.mapNo == mapNo && info.w > 0 && info.h > 0 ? &info : nullptr;
    }

    BigMapButtonStyle ButtonStyle(BigMapButton button) const {
        switch (button) {
        case BigMapButton::Toggle: return {11, 51, 20, 18, options ? 243030 : 243033,
            options ? 243032 : 243035, options ? 243031 : 243034, true};
        case BigMapButton::Close: return {window.width - 26, 6, 20, 20, 22509, 22509, 22510, true};
        case BigMapButton::Refresh: return {161, 286, 20, 19, 243018, 243020, 243019, options};
        case BigMapButton::Walk: return {11, 73, 18, 18, WalkNormal, WalkHover, WalkPressed, targetX >= 0 && mapReady};
        case BigMapButton::Center: return {11, targetX >= 0 ? 97 : 73, 18, 18,
            CenterNormal, CenterHover, CenterPressed, !followPlayer};
        }
        return {};
    }

    bool ElementHit(int index) const {
        const auto& element = window.elements[index];
        return element.visible == 1 && Hit(element.x, element.y, element.width, element.height);
    }

    bool ButtonHit(BigMapButton button) const { return ElementHit(ButtonFirst + static_cast<int>(button)); }
    bool TitleHit() const { return ElementHit(TitleArea); }
    bool MapHit() const {
        if (!mapReady) return false;
        for (int i = ButtonFirst; i < MapArea; ++i) if (ElementHit(i)) return false;
        return ElementHit(MapArea);
    }

    void SyncControls() {
        const auto area = [&](int index, int x, int y, int w, int h, bool visible = true) {
            auto& element = window.elements[index];
            element.x = x; element.y = y; element.width = w; element.height = h;
            element.visible = visible ? 1 : 0;
        };
        for (int i = ButtonFirst; i < ButtonEnd; ++i) {
            const auto style = ButtonStyle(static_cast<BigMapButton>(i - ButtonFirst));
            area(i, style.x, style.y, style.w, style.h, style.visible);
        }
        for (int i = CategoryFirst; i < CategoryEnd; ++i)
            area(i, 42, 138 + (i - CategoryFirst) * 20, 194, 20, options);
        area(TitleArea, 6, 6, window.width - 38, 32);
        area(NameArea, 10, 10, 184, 38);
        area(CompassArea, 515, 40, 96, 81);
        area(StatusArea, map.x, window.height - 40, map.w, 32);
        area(OptionsArea, 36, 52, 224, 270, options);
        area(MapArea, map.x, map.y, map.w, map.h);
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
            routeStatus.clear();
            LoadAnnotations();
        }
        if (followPlayer) {
            map.mapX = static_cast<int>(world::gPlayerX);
            map.mapY = static_cast<int>(world::gPlayerY);
        }
        mapReady = map.PrepareMap();
        SyncControls();
        if (!routeStatus.empty()) routeStatus = getAutoCopliotState() ? L"寻路中" : L"寻路已停止";
        const int mx = static_cast<int>(gMouseX), my = static_cast<int>(gMouseY);
        const bool held = (gMouseBtnDown2 & 1) != 0;
        const bool available = gWindowHasFocus == 0 && ui::WindowDrag_ReadMousePhase() == 0
            && platform::gWindowActiveState == 2 && platform::IsClientWindowForeground();
        acceptsMouse = available && Hit(0, 0, window.width, window.height);
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
        if (!acceptsMouse || (window.state & WIN_STATE_CLOSE) || !(events & 1)) return 0;
        if (index != MapArea) {
            if (events & (2 | 4)) mapClickPending = false;
            if (events & 2) {
                if (index < ButtonEnd) {
                    switch (static_cast<BigMapButton>(index - ButtonFirst)) {
                    case BigMapButton::Close: window.state |= WIN_STATE_CLOSE; break;
                    case BigMapButton::Toggle: options = !options; break;
                    case BigMapButton::Center: followPlayer = true; break;
                    case BigMapButton::Refresh: LoadAnnotations(); break;
                    case BigMapButton::Walk:
                        startCopilot(targetX, targetY);
                        routeStatus = getAutoCopliotState() ? L"寻路中" : L"寻路未启动";
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
                routeStatus = getAutoCopliotState() ? L"寻路中" : L"寻路未启动";
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
                routeStatus = getAutoCopliotState() ? L"寻路中" : L"寻路未启动";
            } else if (!getAutoCopliotState()) routeStatus.clear();
        }
        SyncControls();
        return 1;
    }

    void Text(int x, int y, std::wstring_view value, const UIClipRect& clip, int color = 0) const {
        const std::string gbk = text::ToGbk(value);
        RendererDrawTextClipped(static_cast<short>(x), static_cast<short>(y), FONT_LAYER_WINDOW,
            kMapFont, static_cast<std::uint8_t>(color), gbk.c_str(), 0, 0, &clip);
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

    void SetText(int index, int x, int y, std::wstring_view value, int color = 0) {
        auto& element = window.elements[index];
        auto& data = *static_cast<WindowTextData*>(element.data);
        element.x = x; element.y = y; element.visible = 1;
        data.text = value;
        data.style.color = static_cast<std::byte>(color);
    }

    void SetImage(int index, int mapNo, int x, int y, int w, int h, std::uint32_t color = 0xFFFFFFFFu) {
        auto& element = window.elements[index];
        auto& data = *static_cast<WindowGraphicData*>(element.data);
        element.x = x; element.y = y; element.width = w; element.height = h; element.visible = 1;
        data.imageNo = mapNo; data.color = color;
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
                Text(window.x + element.x, window.y + element.y, data.text, textClip,
                    std::to_integer<int>(data.style.color));
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
                Text(cx - textWidth / 2, nameY, point.name, clip);
                const auto coordinates = std::format(L"({},{})", point.x, point.y);
                Text(cx - render::TextMeasure(coordinates, kMapFont) / 2, nameY + 12, coordinates, clip);
                if (point.imageNo >= 0) {
                    const int w = point.imageWidth, h = point.imageHeight;
                    if (UIRectIntersectsClipRect(cx - w / 2, cy - h / 2, w, h, &clip)
                        && render::DrawGraphicImage(point.imageNo, cx - w / 2, cy - h / 2, w, h,
                            LAYER_UI, &imageClip) < 0) ++failedIcons;
                } else Text(cx - 3, cy - fontHeight / 2, L"?", clip, 4);
                ImageAt(AnnotationBorder, cx - 21, cy - 15, 42, 31, clip);
                ImageAt(AnnotationPlate, cx - 21, cy - 15, 42, 31, clip, kCategoryColors[point.category]);
                if (point.placement == 2 || point.placement == 3)
                    ImageAt(HorizontalConnector, x - (point.placement == 2 ? 26 : 2), y - 2, 28, 5, clip, kCategoryColors[point.category]);
                else ImageAt(VerticalConnector, x - 3, y - (point.placement == 1 ? 2 : 24), 5, 28, clip, kCategoryColors[point.category]);
            } else if (point.category == 6) {
                Text(x + 6, y - 6, point.name, clip, point.extra);
                ImageAt(NpcDot, x - 1, y - 1, 4, 4, clip,
                    render::GetTextColor(static_cast<std::uint8_t>(point.extra)));
            } else {
                // 地点和村镇原端为文字底条，不使用占位方块。
                const int textY = point.category == 0 && point.placement <= 0 ? y - 20 : y;
                Text(x - textWidth / 2, textY, point.name, clip, point.category == 4 ? 4 : 0);
                RendererDrawRectClipped(static_cast<short>(x - textWidth / 2 - 2), static_cast<short>(textY),
                    static_cast<short>(textWidth + 4), static_cast<short>(fontHeight + 2), LAYER_UI,
                    point.category == 4 ? 0x60000000u : 0x40000000u, &clip);
            }
        }
        return failedIcons;
    }

    int Draw(int stage) {
        if (window.state & WIN_STATE_CLOSE) return stage;
        const int previousUiFailures = failedUiImages;
        failedUiImages = 0;
        RendererBreakWindowTextBatch();
        const UIClipRect clip{static_cast<short>(window.x), static_cast<short>(window.y),
            static_cast<short>(window.x + window.width), static_cast<short>(window.y + window.height)};
        const auto mapClip = Viewport();
        for (int i = TextFirst; i < ElementCount; ++i) window.elements[i].visible = 0;
        SetText(TextTitle, (window.width - render::TextMeasure(L"大地图", kMapFont)) / 2, 16, L"大地图", 12);
        SetText(TextMapName, 16, 20, text::FromGbkWide(world::ReadText9DAD30()), 12);
        SetImage(MapNameImage, MapNamePlate, 10, 10, 184, 38);
        const int px = static_cast<int>(world::gPlayerX), py = static_cast<int>(world::gPlayerY);
        const int coordinateLeft = 530, coordinateTop = 40;
        SetText(TextX, coordinateLeft - 14, coordinateTop + 64, std::format(L"{:3}", px), 12);
        SetText(TextY, coordinateLeft + 36, coordinateTop + 64, std::format(L"{:3}", py), 12);
        SetImage(CoordinateLeftImage, CoordinatePlate, coordinateLeft - 15, coordinateTop + 65, 46, 16);
        SetImage(CoordinateRightImage, CoordinatePlate, coordinateLeft + 35, coordinateTop + 65, 46, 16);
        SetImage(CompassImage, Compass, coordinateLeft, coordinateTop, 60, 59);
        for (int i = ButtonFirst; i < ButtonEnd; ++i) {
            const auto style = ButtonStyle(static_cast<BigMapButton>(i - ButtonFirst));
            if (!style.visible) continue;
            const auto events = window.elements[i].state;
            const int mapNo = (events & 1) ? ((events & 8) ? style.pressed : style.hover) : style.normal;
            SetImage(i, mapNo, style.x, style.y, style.w, style.h);
        }
        const auto walkButton = ButtonStyle(BigMapButton::Walk), centerButton = ButtonStyle(BigMapButton::Center);
        if (!options) SetText(TextToggle, 34, 52, L"地图选项", 4);
        if (walkButton.visible && !options) SetText(TextWalk, 34, walkButton.y + 3, L"开始寻路", 4);
        if (centerButton.visible && !options) SetText(TextCenter, 34, centerButton.y + 3, L"玩家居中", 4);
        if (options) {
            SetText(TextOptions, 44, 60, L"地图选项", 4);
            SetText(TextPlayer, 62, 79, L"你现在的位置");
            SetText(TextTarget, 62, 99, L"右键点选的寻路坐标");
            SetText(TextDisplay, 44, 120, L"显示选项", 4);
            SetImage(PlayerLegendImage, PlayerMarker, 44, 80, 14, 14);
            SetImage(TargetLegendImage, TargetMarker, 44, 100, 14, 14);
            for (int i = 0; i < static_cast<int>(categories.size()); ++i) {
                SetText(CategoryTextFirst + i, 62, 140 + i * 20, kCategoryNames[i]);
                if (categories[i]) SetImage(CheckedFirst + i, Checked, 46, 141 + i * 20, 12, 11, 0xFFFF0000u);
                SetImage(CheckboxFirst + i, Checkbox, 46, 143 + i * 20, 9, 9);
            }
            SetText(TextRefresh, 184, 287, L"刷新坐标", 4);
        }
        std::wstring hint = targetX >= 0
            ? std::format(L"目标：{},{}  {}", targetX, targetY, routeStatus.empty() ? L"点击开始寻路" : routeStatus)
            : L"左键拖动，双击寻路，右键选择坐标";
        if (acceptsMouse) {
            if (ButtonHit(BigMapButton::Walk)) hint = L"开始寻路：前往右键选中的目标";
            else if (ButtonHit(BigMapButton::Center)) hint = L"玩家居中：恢复地图跟随";
            else if (ButtonHit(BigMapButton::Refresh)) hint = L"刷新坐标：重新读取地点标注";
            else if (ButtonHit(BigMapButton::Toggle)) hint = options ? L"收起地图选项" : L"展开地图选项";
            else if (ButtonHit(BigMapButton::Close)) hint = L"关闭大地图";
            else if (TitleHit()) hint = L"拖动标题栏移动窗口";
        }
        SetText(TextHint, 12, window.height - 38, hint, 12);
        std::wstring status = !annotationsLoaded ? L"缺少或无法读取 data/mapdesc.txt"
            : annotations.empty() ? L"当前地图没有标注" : std::format(L"地图标注：{}处", annotations.size());
        if (missingIcons || previousUiFailures || failedUiImages)
            status += std::format(L"  缺图：{}  UI资源失败：{}", missingIcons, std::max(previousUiFailures, failedUiImages));
        const auto generation = map.Status();
        if (!generation.empty()) status += L"  " + generation;
        SetText(TextStatus, 12, window.height - 18, status, 12);
        DrawElements(clip);
        if (options) {
            for (int i = 0; i < 7; ++i) {
                if (window.elements[CategoryFirst + i].state & 1)
                    RendererDrawRectClipped(static_cast<short>(window.x + 42),
                        static_cast<short>(window.y + 138 + i * 20), 194, 20, LAYER_UI, 0x503F6E83u, &clip);
            }
            DrawPanel(clip);
        }
        RendererBreakWindowTextBatch();
        if (mapReady) {
            if (acceptsMouse && MapHit()) {
                const auto point = map.ScreenToMap(gMouseX - mapClip.left, gMouseY - mapClip.top);
                Text(gMouseX + 14, gMouseY + 14, std::format(L"{},{}", point.first, point.second), mapClip);
            }
            Marker(px, py, PlayerMarker, 14, mapClip);
            if (targetX >= 0) Marker(targetX, targetY, TargetMarker, 14, mapClip);
            const auto route = getAutoCopliotRoute();
            for (std::size_t i = 1; i < route.size(); i += 2)
                Marker(route[i].first, route[i].second, RouteDot, 4, mapClip);
            failedUiImages += DrawAnnotations(mapClip);
        } else Text(window.x + (window.width - render::TextMeasure(L"当前地图数据未就绪", kMapFont)) / 2,
            window.y + window.height / 2, L"当前地图数据未就绪", clip, 4);
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
        gBigMapState->SyncControls();
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
