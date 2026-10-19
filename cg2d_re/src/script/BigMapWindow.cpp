#include "BigMapWindow.hpp"
#include "LuaUI.hpp"
#include "../world/WorldMapState.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include <array>
#include <fstream>
#include <format>
#include <string_view>

namespace cg::script {

static constexpr int kBigMapWindowId = 82; // 扩展窗口，不占原71槽；100以上留给LuaWindow。
static constexpr int kMapFont = 12;
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

struct BigMapWindow final : UIWindowInstanceEx {
    UIMapExControl map{kBigMapWindowId, 0, 0, "bigMap", 0, 0, 1, 1, 1, 0, 0, -1, -1, {}};
    std::map<std::pair<int, int>, BigMapAnnotation> annotations;
    std::array<bool, 7> categories{true, true, true, true, true, true, true};
    int currentMap = -1, currentFloor = -1, currentGroup = -1;
    int targetX = -1, targetY = -1;
    int dragX = 0, dragY = 0, dragMapX = 0, dragMapY = 0;
    bool options = true, followPlayer = true, dragging = false, mapReady = false;
    bool annotationsLoaded = false;
    int missingIcons = 0;
    std::wstring routeStatus;

    BigMapWindow() {
        window.id = kBigMapWindowId;
        window.layer = 4;
        window.state = WIN_STATE_ACTIVE;
        Layout();
    }

    void Layout() {
        window.x = 12;
        window.y = 24;
        window.width = render::GetRendererLogicalWidth() - 24;
        window.height = render::GetRendererLogicalHeight() - 48;
        map.x = options ? 180 : 8;
        map.y = 64;
        map.w = static_cast<short>(window.width - map.x - 8);
        map.h = static_cast<short>(window.height - map.y - 42);
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
            if (count < 5 || std::atoi(fields[0].c_str()) != currentMap) continue;
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

    int CategoryStep() const { return std::min(22, (window.height - 220) / 7); }

    int update(int stage, int focused) override {
        if (world::gCurrentSceneType != SCENE_NORMAL) {
            window.state |= WIN_STATE_CLOSE;
            dragging = false;
            return stage;
        }
        Layout();
        if (currentMap != static_cast<int>(world::gCurrentMapId_9A86DC)
            || currentFloor != world::gMapFloor || currentGroup != world::gMapCachePathKey_9E0434) {
            currentMap = static_cast<int>(world::gCurrentMapId_9A86DC);
            currentFloor = world::gMapFloor;
            currentGroup = world::gMapCachePathKey_9E0434;
            targetX = targetY = -1;
            dragging = false;
            followPlayer = true;
            routeStatus.clear();
            LoadAnnotations();
        }
        if (followPlayer) {
            map.mapX = static_cast<int>(world::gPlayerX);
            map.mapY = static_cast<int>(world::gPlayerY);
        }
        mapReady = map.PrepareMap();
        if (!routeStatus.empty()) routeStatus = getAutoCopliotState() ? L"寻路中" : L"寻路已停止";
        const int mx = static_cast<int>(gMouseX), my = static_cast<int>(gMouseY);
        const bool held = (gMouseBtnDown2 & 1) != 0;
        const int phase = ui::WindowDrag_ReadMousePhase();
        const bool available = gWindowHasFocus == 0 && phase == 0;
        if (dragging) {
            if (available && held) {
                const int dx = mx - dragX, dy = my - dragY;
                if (dx != 0 || dy != 0) followPlayer = false;
                map.mapX = dragMapX - static_cast<int>(std::round((dx - 2.0 * dy) / 4.0));
                map.mapY = dragMapY - static_cast<int>(std::round((dx + 2.0 * dy) / 4.0));
            } else dragging = false;
            // 松开或拖出窗口的当帧也拦截，避免场景收到尾部鼠标事件。
            gMouseEventState = MES_CAPTURED;
            gWindowHasFocus = 1;
            return stage;
        }
        if (!available || !focused) return stage;
        if (gMouseClickState & 1) {
            if (Hit(window.width - 54, 4, 46, 22)) window.state |= WIN_STATE_CLOSE;
            else if (Hit(8, 4, 108, 22)) { options = !options; Layout(); }
            else if (options && Hit(8, 34, 164, 24)) followPlayer = true;
            else if (options && Hit(8, 64, 164, 24)) LoadAnnotations();
            else if (options && Hit(8, 94, 164, 24) && targetX >= 0) {
                startCopilot(targetX, targetY);
                routeStatus = getAutoCopliotState() ? L"寻路中" : L"寻路未启动";
            } else if (options && Hit(8, 154, 164, CategoryStep() * 7)) {
                const int index = (my - window.y - 154) / CategoryStep();
                categories[index] = !categories[index];
            } else if (mapReady && Hit(map.x, map.y, map.w, map.h)) {
                dragging = true;
                dragX = mx; dragY = my;
                dragMapX = map.mapX; dragMapY = map.mapY;
            }
        }
        if ((gMouseClickState & 2) && mapReady && Hit(map.x, map.y, map.w, map.h)) {
            const auto point = map.ScreenToMap(mx - window.x - map.x, my - window.y - map.y);
            if (point.first >= 0 && point.second >= 0 && point.first < map.mapWidth && point.second < map.mapHeight) {
                targetX = point.first; targetY = point.second;
                routeStatus.clear();
            }
        }
        return stage; // 调度器根据focused统一吞掉窗口内事件。
    }

    void Text(int x, int y, std::wstring_view value, const UIClipRect& clip, int color = 0) const {
        const std::string gbk = text::ToGbk(value);
        RendererDrawTextClipped(static_cast<short>(x), static_cast<short>(y), FONT_LAYER_WINDOW,
            kMapFont, static_cast<std::uint8_t>(color), gbk.c_str(), 0, 0, &clip);
    }

    void Button(int x, int y, int w, std::wstring_view label, const UIClipRect& clip, bool enabled = true) const {
        Text(window.x + x + 6, window.y + y + 4, label, clip, enabled ? 0 : 7);
        RendererDrawRectClipped(static_cast<short>(window.x + x), static_cast<short>(window.y + y),
            static_cast<short>(w), 22, LAYER_UI, enabled && Hit(x, y, w, 22) ? 0xFF53677Du : 0xFF344353u, &clip);
    }

    void Marker(int gx, int gy, int size, std::uint32_t color, const UIClipRect& clip) const {
        const auto [x, y] = map.MapToScreen(gx, gy);
        RendererDrawRectClipped(static_cast<short>(clip.left + x - size), static_cast<short>(clip.top + y - size),
            static_cast<short>(size * 2 + 1), static_cast<short>(size * 2 + 1), LAYER_UI, color, &clip);
    }

    // 4.06@4384B9：村镇绕过探索门槛，图标/文本按附加字段和方位分别处理。
    int DrawAnnotations(const UIClipRect& clip) const {
        int failedIcons = 0;
        const render::PngRectangle imageClip{clip.left, clip.top, clip.right, clip.bottom};
        const int fontHeight = render::GetFontMetrics(kMapFont).fontHeight;
        for (const auto& [position, point] : annotations) {
            if (!categories[point.category] || point.x < 0 || point.y < 0 || point.x >= map.mapWidth || point.y >= map.mapHeight
                || (point.category != 4 && !map.IsExplored(point.x, point.y))) continue;
            auto [x, y] = map.MapToScreen(point.x, point.y);
            x += clip.left; y += clip.top;
            const int textWidth = render::TextMeasure(point.name, kMapFont);
            int textX = x - textWidth / 2, textY = y - fontHeight - 4;
            if (point.hasIcon) {
                if (point.placement == 1) textY = y + point.imageHeight / 2 + 2;
                else if (point.placement == 2) { textX = x - point.imageWidth / 2 - textWidth - 2; textY = y - fontHeight / 2; }
                else if (point.placement == 3) { textX = x + point.imageWidth / 2 + 2; textY = y - fontHeight / 2; }
                else textY = y - point.imageHeight / 2 - fontHeight - 2;
            } else if (point.category == 0 && point.placement > 0) textY = y + 4;
            Text(textX, textY, point.name, clip, point.category == 6 ? point.extra : 0);
            if (point.hasIcon && point.imageNo >= 0) {
                const int iconX = x - point.imageWidth / 2, iconY = y - point.imageHeight / 2;
                if (UIRectIntersectsClipRect(iconX, iconY, point.imageWidth, point.imageHeight, &clip)
                    && render::DrawGraphicImage(point.imageNo, iconX, iconY, point.imageWidth, point.imageHeight,
                        LAYER_UI, &imageClip) < 0) ++failedIcons;
            } else if (point.hasIcon) {
                Text(x - 3, y - fontHeight / 2, L"?", clip, 4);
            } else {
                // NPC点沿其颜色号绘制，避免将第六列误当MapNo。
                if (point.category == 6) Text(x - 3, y - fontHeight / 2, L"+", clip, point.extra);
                else Marker(point.x, point.y, point.category == 4 ? 3 : 1, 0xFFFFFF00u, clip);
            }
        }
        return failedIcons;
    }

    int render(int stage) override {
        if (window.state & WIN_STATE_CLOSE) return stage;
        RendererBreakWindowTextBatch();
        const UIClipRect clip{static_cast<short>(window.x), static_cast<short>(window.y),
            static_cast<short>(window.x + window.width), static_cast<short>(window.y + window.height)};
        const auto viewport = Viewport();
        Button(8, 4, 108, options ? L"收起地图选项" : L"展开地图选项", clip);
        Button(window.width - 54, 4, 46, L"关闭", clip);
        Text(window.x + 128, window.y + 8, L"大地图", clip);
        if (options) {
            Button(8, 34, 164, followPlayer ? L"玩家居中 [跟随]" : L"玩家居中", clip);
            Button(8, 64, 164, L"刷新坐标", clip);
            Button(8, 94, 164, L"开始寻路", clip, targetX >= 0 && mapReady);
            Text(window.x + 12, window.y + 130, L"显示选项", clip);
            for (int i = 0; i < static_cast<int>(categories.size()); ++i) {
                Text(window.x + 12, window.y + 154 + i * CategoryStep(),
                    std::wstring(categories[i] ? L"[x] " : L"[ ] ") + kCategoryNames[i], clip);
            }
        }
        const UIClipRect coordinates{viewport.left, static_cast<short>(window.y + 28), viewport.right, viewport.top};
        Text(viewport.left, window.y + 30, std::format(L"人物：{},{}", static_cast<int>(world::gPlayerX), static_cast<int>(world::gPlayerY)), coordinates);
        Text(viewport.left, window.y + 46, targetX >= 0 ? std::format(L"目标：{},{}  {}", targetX, targetY, routeStatus) : L"右键选择寻路坐标", coordinates);
        int failedIcons = 0;
        if (mapReady) {
            Marker(static_cast<int>(world::gPlayerX), static_cast<int>(world::gPlayerY), 3, 0xFF00FF00u, viewport);
            if (targetX >= 0) Marker(targetX, targetY, 3, 0xFFFF4040u, viewport);
            const auto route = getAutoCopliotRoute();
            if (route.size() > 1) {
                CanvasShape path;
                path.type = CanvasShapeType::PathBorder;
                path.color = 0xFF00FFFFu;
                for (const auto& [gx, gy] : route) {
                    const auto [x, y] = map.MapToScreen(gx, gy);
                    path.points.push_back({static_cast<float>(x), static_cast<float>(y)});
                }
                RendererDrawCanvasClipped(viewport.left, viewport.top, map.w, map.h, LAYER_UI, {path}, &viewport);
            }
            failedIcons = DrawAnnotations(viewport);
        } else Text(viewport.left + 8, viewport.top + 8, L"当前地图数据未就绪", viewport, 4);
        if (mapReady && Hit(map.x, map.y, map.w, map.h)) {
            const auto point = map.ScreenToMap(gMouseX - viewport.left, gMouseY - viewport.top);
            Text(window.x + 8, window.y + window.height - 36,
                std::format(L"鼠标：{},{}  左键拖动地图，右键定位", point.first, point.second), clip);
        } else Text(window.x + 8, window.y + window.height - 36, L"左键拖动地图，右键定位", clip);
        std::wstring status = !annotationsLoaded ? L"缺少或无法读取 data/mapdesc.txt"
            : annotations.empty() ? L"当前地图没有标注" : std::format(L"地图标注：{}处", annotations.size());
        if (missingIcons || failedIcons) status += std::format(L"  缺失图标：{}  绘制失败：{}", missingIcons, failedIcons);
        Text(window.x + 8, window.y + window.height - 19, status, clip, missingIcons || failedIcons || !annotationsLoaded ? 4 : 0);
        // 同层逆入队绘制：标记/文字在前，底图随后，窗口背景最后。
        map.render(static_cast<short>(window.x), static_cast<short>(window.y), &viewport);
        RendererDrawRectClipped(static_cast<short>(window.x), static_cast<short>(window.y),
            static_cast<short>(window.width), static_cast<short>(window.height), LAYER_UI, 0xFF18232Fu, &clip);
        RendererBreakWindowTextBatch();
        return stage;
    }
};

void OpenBigMapWindow() {
    if (world::gCurrentSceneType != SCENE_NORMAL) return;
    if (const auto existing = GetExtWindowById(kBigMapWindowId)) {
        existing->window.state &= ~WIN_STATE_CLOSE;
        existing->window.state |= WIN_STATE_FOCUS;
        return;
    }
    std::shared_ptr<UIWindowInstanceEx> window = std::make_shared<BigMapWindow>();
    WindowExOpen(window);
}

} // namespace cg::script
