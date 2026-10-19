#include "cg/render/Font.hpp"
#include "../WindowRuntimeInternal.hpp"
#include "../../world/WorldMapState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"
#include "../../script/BigMapWindow.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/PngImage.hpp"
#include "cg/resource/GraphicInfo.hpp"

namespace cg::ui {
namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x00495E10。
static std::int32_t WindowMiniMap_DrawMap(std::int32_t index, std::int32_t unused); // 小地图按窗口/元素坐标入既有绘制队列。

// 原函数：0x00495E60。
static std::int32_t WindowMiniMap_RefreshPlayerX(std::int32_t index, std::int32_t unused); // 固定文本实例，门禁后刷新玩家X。

// 原函数：0x00495EB0。
static std::int32_t WindowMiniMap_RefreshPlayerY(std::int32_t index, std::int32_t unused); // 固定文本实例，门禁后刷新玩家Y。

// 原函数：0x00495D70。
static std::int32_t WindowMiniMap_Update(std::int32_t value);

// BMUI01：唯一大地图入口；事件区同时绘制按钮，不改原控件索引。
static std::int32_t WindowMiniMap_OpenBigMap(std::int32_t index, std::int32_t events)
{
    const auto& element = gCurrentWindow->elements[index];
    const auto x = static_cast<std::int16_t>(gCurrentWindow->x + element.x);
    const auto y = static_cast<std::int16_t>(gCurrentWindow->y + element.y);
    render::ClearTextQueueTail_456D50(3);
    render::DrawColoredText(x + 22, y + 2, 3, 12, 0, L"大地图", 0, 0);
    render::ClearTextQueueTail_456D50(3);
    const int mapNo = (events & 8) ? 243034 : (events & 1) ? 243035 : 243033;
    if (resource::gGraphicMapNoIndex && resource::gGraphicInfo && mapNo < resource::gBinLoaderGraphicCapacity) {
        const int imageNo = resource::gGraphicMapNoIndex[mapNo];
        if (imageNo >= 0 && imageNo < resource::gMaxGraphicCount && resource::gGraphicInfo[imageNo].mapNo == mapNo)
            render::DrawGraphicImage(imageNo, x, y, 20, 18, 0xDF);
    }
    if (events & 2) script::OpenBigMapWindow();
    return events != 0;
}

// 控件载荷、窗口状态、组件表与原型。

static WindowGraphicData gWindowGraphic56AEF0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056AEF0。

static WindowGraphicData gWindowGraphic56AF08 {0x3B97A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056AF08。

static WindowGraphicData gWindowGraphic56AF20 {0x3B97B, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056AF20。

static WindowTextPrototype gWindowText56AF38 { // 原全局：0x0056AF38。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"",
};

static WindowTextPrototype gWindowText56AF40 { // 原全局：0x0056AF40。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"",
};

static WindowTextPrototype gWindowText56AF48 { // 原全局：0x0056AF48。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"",
};

static WindowElementPrototype gWindowElements56AF50[8] { // 原全局：0x0056AF50；BMUI01末尾追加入口。
    {WindowElementType_Graphic, 165, 4, 11, 11, 1, {.graphic = &gWindowGraphic56AEF0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Text, 51, 21, 0, 0, 1, {.text = &gWindowText56AF38}, &WindowMiniMap_RefreshPlayerX},
    {WindowElementType_Text, 159, 21, 0, 0, 1, {.text = &gWindowText56AF40}, &WindowMiniMap_RefreshPlayerY},
    {WindowElementType_Text, 160, 138, 0, 0, 1, {.text = &gWindowText56AF48}, &WindowElement_NoOp},
    {WindowElementType_EventArea, -19, 81, 0, 0, 1, {}, &WindowMiniMap_DrawMap},
    {WindowElementType_Graphic, -16, 0, 0, 0, 1, {.graphic = &gWindowGraphic56AF08}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 4, 16, 0, 0, 1, {.graphic = &gWindowGraphic56AF20}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 2, 1, 58, 18, 1, {}, &WindowMiniMap_OpenBigMap},
};

// 原型：窗口33；原全局：0x0054E6C0。
WindowPrototype gWindowPrototypeMiniMap {0, 4, 353, 55, 184, 21, 0x80010101u, 8, gWindowElements56AF50, &WindowMiniMap_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 33: 54E6C0

// 窗口函数实现。

// 原函数：0x00495D70。
static std::int32_t WindowMiniMap_Update(const std::int32_t value)
{
    if (value == -1) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[3].data);
        data->text.clear();
        return 1;
    }
    if (world::IsObservationSceneTransitionActive_440B50() == 0) {
        auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[3].data);
        std::wstring& destination = data->text;
        if (destination[0] == '\0') {
            const char* const name = world::ReadText9DAD30();
            destination = cg::text::FromGbkWide(name);
            const std::int32_t measured = cg::render::TextMeasure(cg::text::FromGbkWide(name).c_str(), 12);
            WindowInstance* const window = gCurrentWindow;
            const std::int32_t width = window->width;
            WindowElement* const elements = window->elements;
            elements[3].x = ((width - measured) >> 1) + 2;
        }
    }
    return 1;
}

// 原函数：0x00495E10。
static std::int32_t WindowMiniMap_DrawMap(const std::int32_t index, const std::int32_t)
{
    if (world::IsObservationSceneTransitionActive_440B50() != 0) {
        return 0;
    }
    WindowInstance* const window = gCurrentWindow;
    WindowElement* const elements = window->elements;
    const std::int32_t windowY = window->y;
    const std::int32_t elementY = elements[index].y;
    const std::int32_t elementX = elements[index].x;
    const std::int32_t y = elementY + windowY - 117;
    const std::int32_t x = elementX + window->x - 14;
    static_cast<void>(render::QueueMap_521AA0(x, y, 0xDF));
    return 0;
}

// 原函数：0x00495E60。
static std::int32_t WindowMiniMap_RefreshPlayerX(const std::int32_t index, const std::int32_t)
{
    auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
    if (world::IsObservationSceneTransitionActive_440B50() != 0) {
        data->text.clear();
        return 0;
    }
    const std::int32_t x =
        static_cast<std::int32_t>(world::gPlayerX);
    cg::text::FormatTo(data->text, L"{:3}", x);
    return 0;
}

// 原函数：0x00495EB0。
static std::int32_t WindowMiniMap_RefreshPlayerY(const std::int32_t index, const std::int32_t)
{
    auto* const data = static_cast<WindowTextData*>(gCurrentWindow->elements[index].data);
    if (world::IsObservationSceneTransitionActive_440B50() != 0) {
        data->text.clear();
        return 0;
    }
    const std::int32_t y =
        static_cast<std::int32_t>(world::gPlayerY);
    cg::text::FormatTo(data->text, L"{:3}", y);
    return 0;
}

// 原函数：0x00495F00。
cg::yobi::YobiAction* WindowMiniMap_Open(
    const std::uint32_t flags, const std::uint32_t mode,
    cg::yobi::YobiAction** const completionSlot)
{
    WindowInstance* const window = gWindowStates[WindowId_MiniMap].window;
    std::int32_t x;
    std::int32_t y;
    if (window != nullptr) {
        window->state |= 0x8000u;
        WindowInstance* const current = gWindowStates[WindowId_MiniMap].window;
        x = current->x;
        y = current->y;
    } else {
        x = gWindowStates[WindowId_MiniMap].x;
        y = gWindowStates[WindowId_MiniMap].y;
    }
    return cg::yobi::CreateWindowOpeningAnimation_475950(x, y, 184, 159, 33, flags, mode, completionSlot);
}

} // namespace detail

} // namespace cg::ui
