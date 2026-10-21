#include "../WindowRuntimeInternal.hpp"
#include "../../world/ChatHistory.hpp"

#include <cstring>
#include <ctime>
#include <cstdio>

namespace cg::ui {

// 固定元素ID；创建与业务查找使用同一枚举，0保留给自动分配。
enum class WindowAdventureLogElementId : ElementId {
    Element1 = 1,
    HandleVerticalScroll = 2,
    HandleNavigation = 3,
    HandleNavigation_4 = 4,
    Element5 = 5,
    ButtonClose = 6,
    HandleNavigation_7 = 7,
    HandleNavigation_8 = 8,
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
};


namespace detail {

// 回调与内部函数前置声明。

// 原函数：0x004CC340。
static void WindowAdventureLog_Refresh(); // 窗口66原字节分行/十四行刷新，固定651000B局部缓冲。

// 原函数：0x004CC540。
static void WindowAdventureLog_Load(); // 日志文件重开/计数，保留原双关闭槽语义，见window_66.md。

// 原函数：0x004CBDB0。
static std::int32_t WindowAdventureLog_HandleNavigation(ElementId elementId, std::int32_t state); // 上下翻与原非标准日期前后切换。

// 原函数：0x004CBD90。
static std::int32_t WindowAdventureLog_Update(std::int32_t value);

// 控件载荷、窗口状态、组件表与原型。

static std::int32_t gLogDisplayGate593F84 = 0; // 原全局：0x00593F84。 窗口66的日志显示门禁。

static WindowGraphicData gWindowGraphic57ADD8 {243001, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057ADD8。

static WindowGraphicData gWindowGraphic57ADF0 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057ADF0。

static WindowGraphicData gWindowGraphic57AE08 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE08。

static WindowGraphicData gWindowGraphic57AE20 {36814, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE20。

static WindowGraphicData gWindowGraphic57AE38 {36804, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE38。

static WindowGraphicData gWindowGraphic57AE50 {36805, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x0057AE50。

static WindowGraphicData gWindowGraphic57AE68 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE68。

static WindowGraphicData gWindowGraphic57AE80 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE80。

static WindowGraphicData gWindowGraphic57AE98 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE98。

static WindowTextPrototype gWindowText57AEB0 { // 原全局：0x0057AEB0。
    {std::byte{1}, std::byte{12}, std::byte{0}, std::byte{0}}, world::gLogFileName554648,
};

static WindowTextPrototype gWindowText57AEB8 { // 原全局：0x0057AEB8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"",
};

static std::uint32_t gWindowMouseFlagsD679C8 = 0; // 原全局：0x00D679C8。

static WindowElementPrototype gWindowElements57AEC0[25] { // 原全局：0x0057AEC0。
    {WindowElementType_Graphic, 419, 54, 0, 14, 1, {.graphic = &gWindowGraphic57AE68}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element1)},
    {WindowElementType_MouseArea, 419, 54, 11, 221, 1, {.mouseFlags = &gWindowMouseFlagsD679C8}, &WindowElement_HandleVerticalScroll, -1, static_cast<ElementId>(WindowAdventureLogElementId::HandleVerticalScroll)},
    {WindowElementType_Graphic, 419, 43, 11, 11, 1, {.graphic = &gWindowGraphic57AE80}, &WindowAdventureLog_HandleNavigation, -1, static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation)},
    {WindowElementType_Graphic, 419, 275, 11, 11, 1, {.graphic = &gWindowGraphic57AE98}, &WindowAdventureLog_HandleNavigation, -1, static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation_4)},
    {WindowElementType_Text, 150, 286, 0, 0, 1, {.text = &gWindowText57AEB0}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element5)},
    {WindowElementType_Graphic, 434, 8, 12, 12, 1, {.graphic = &gWindowGraphic57ADD8}, &WindowRuntime_ButtonClose, -1, static_cast<ElementId>(WindowAdventureLogElementId::ButtonClose)},
    {WindowElementType_Graphic, 380, 283, 18, 18, 1, {.graphic = &gWindowGraphic57ADF0}, &WindowAdventureLog_HandleNavigation, -1, static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation_7)},
    {WindowElementType_Graphic, 360, 283, 18, 18, 1, {.graphic = &gWindowGraphic57AE08}, &WindowAdventureLog_HandleNavigation, -1, static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation_8)},
    {WindowElementType_Graphic, 60, 284, 0, 0, 1, {.graphic = &gWindowGraphic57AE20}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element9)},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic57AE38}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element10)},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic57AE50}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element11)},
    {WindowElementType_Text, 25, 45, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element12)},
    {WindowElementType_Text, 25, 61, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element13)},
    {WindowElementType_Text, 25, 77, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element14)},
    {WindowElementType_Text, 25, 93, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element15)},
    {WindowElementType_Text, 25, 109, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element16)},
    {WindowElementType_Text, 25, 125, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element17)},
    {WindowElementType_Text, 25, 141, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element18)},
    {WindowElementType_Text, 25, 157, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element19)},
    {WindowElementType_Text, 25, 173, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element20)},
    {WindowElementType_Text, 25, 189, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element21)},
    {WindowElementType_Text, 25, 205, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element22)},
    {WindowElementType_Text, 25, 221, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element23)},
    {WindowElementType_Text, 25, 237, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element24)},
    {WindowElementType_Text, 25, 253, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp, -1, static_cast<ElementId>(WindowAdventureLogElementId::Element25)},
};

// 原型：窗口66；原全局：0x0054EEB0。
WindowPrototype gWindowPrototypeAdventureLog {0, 4, 15, 97, 465, 321, 0x80000080u, 25, gWindowElements57AEC0, &WindowAdventureLog_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}; // 66: 54EEB0

// 窗口函数实现。

// 原函数：0x004CBD30。
void WindowAdventureLog_InitializeDate()
{
    std::tm* const previous = gLoginWindowRuntime.calendarTimeD67BAC;
    gLoginWindowRuntime.valueD67BB0 = 0;
    if (previous != nullptr) {
        return;
    }
    gLoginWindowRuntime.timeCB1DF0 = ::_time64(nullptr);
    std::tm* const current =
        ::_localtime64(&gLoginWindowRuntime.timeCB1DF0);
    gLoginWindowRuntime.calendarTimeD67BAC = current;
    const std::int32_t day = current->tm_mday;
    gLoginWindowRuntime.dayD67BC0 = day;
    const std::int32_t month = current->tm_mon;
    const std::int32_t year = current->tm_year;
    world::gLogFileName554648 = std::format(L"chat_{:02}{:02}{:02}.txt", year % 100, month + 1, day);
}

// 原函数：0x004CC540。
static void WindowAdventureLog_Load()
{
    std::FILE* const previous = gLoginWindowRuntime.logViewerFileD67BB8;
    if (previous != nullptr) {
        static_cast<void>(std::fclose(previous));
    }
    const std::int32_t day = gLoginWindowRuntime.dayD67BC0;
    std::tm* const calendar = gLoginWindowRuntime.calendarTimeD67BAC;
    const std::int32_t month = calendar->tm_mon;
    const std::int32_t year = calendar->tm_year;
    world::gLogFileName554648 = std::format(L"chat_{:02}{:02}{:02}.txt", year % 100, month + 1, day);
    const auto path = std::format(L"Log\\{}", world::gLogFileName554648);
    std::FILE* current = _wfopen(path.c_str(), L"rt");
    gLoginWindowRuntime.logViewerFileD67BB8 = current;
    if (current == nullptr) {
        gLoginWindowRuntime.logViewerErrorD67BBC = 1;
        return;
    }
    gLoginWindowRuntime.logViewerErrorD67BBC = 0;
    static_cast<void>(std::fclose(current));
    current = _wfopen(path.c_str(), L"a+t" /* 原常量参考：0x0055350C。 */);
    gLoginWindowRuntime.logViewerFileD67BB8 = current;
    if (current == nullptr) {
        gLoginWindowRuntime.logViewerErrorD67BBC = 1;
        return;
    }
    gLogDisplayGate593F84 = 1;
    gLoginWindowRuntime.logViewerMaximumD67BB4 = 0;
    while (std::feof(current) == 0) {
        char buffer[3000] {};
        static_cast<void>(std::fgets(buffer, 1000, current));
        const auto lines = render::Font::split(cg::text::FromGbkWide(buffer), 0, 64 * render::GetFontMetrics(0).sWidth);
        gLoginWindowRuntime.logViewerMaximumD67BB4 += static_cast<std::int32_t>(lines.size()) - 1;
        const std::int32_t count = gLoginWindowRuntime.logViewerMaximumD67BB4;
        current = gLoginWindowRuntime.logViewerFileD67BB8;
        gLoginWindowRuntime.logViewerMaximumD67BB4 = count + 1;
    }
}

// 原函数：0x004CC340。
static void WindowAdventureLog_Refresh()
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return;
    char buffer[3000] {};
    std::vector<std::wstring> rows;
    if (gLoginWindowRuntime.logViewerFileD67BB8 == nullptr
        && gLoginWindowRuntime.logViewerErrorD67BBC == 0) {
        WindowAdventureLog_Load();
    }
    WindowInstance* const window = currentWindow;
    const std::int32_t maximum = gLoginWindowRuntime.logViewerMaximumD67BB4;
    auto* const handleVerticalScrollElement = window->FindElementById(WindowAdventureLogElementId::HandleVerticalScroll);
    if (handleVerticalScrollElement) {
        gLoginWindowRuntime.valueD67BB0 = WindowScroll_ReadPosition(handleVerticalScrollElement, maximum);
    }
    if (gLoginWindowRuntime.logViewerErrorD67BBC == 0 && gLogDisplayGate593F84 == 1) {
        static_cast<void>(::_fseeki64(gLoginWindowRuntime.logViewerFileD67BB8, 0, SEEK_SET));
        while (std::fgets(buffer, 1000, gLoginWindowRuntime.logViewerFileD67BB8) != nullptr) {
            const auto lines = render::Font::split(cg::text::FromGbkWide(buffer), 0, 64 * render::GetFontMetrics(0).sWidth);
            rows.insert(rows.end(), lines.begin(), lines.end());
        }
        for (std::int32_t line = 0; line < 14; ++line) {
            const std::int32_t page = gLoginWindowRuntime.valueD67BB0;
            const std::size_t index = static_cast<std::size_t>(page + line);
            const std::wstring_view text = index < rows.size() ? std::wstring_view(rows[index]) : std::wstring_view{};
            auto* const dataElement = currentWindow->FindElementById(NativeElementKey(11 + line));
            if (dataElement) {
                auto* const data = dataElement->data.text;
                data->text = text;
            }
        }
        return;
    }
    auto* const element12 = currentWindow->FindElementById(WindowAdventureLogElementId::Element12);
    if (element12) {
        auto* const first = element12->data.text;
        first->text = L"无法开启冒险者日志" /* 原常量参考：0x0055358C。 */;
    }
    for (std::int32_t line = 12; line < 25; ++line) {
        WindowInstance* const current = currentWindow;
        const char* const text = "";
        auto* const dataElement = current->FindElementById(NativeElementKey(line));
        if (dataElement) {
            auto* const data = dataElement->data.text;
            data->text = cg::text::FromGbkWide(text);
        }
    }
}

// 原函数：0x004CBD90。
static std::int32_t WindowAdventureLog_Update(const std::int32_t value)
{
    if (value == -1) {
        gLoginWindowRuntime.valueD67BB0 = 0;
    }
    WindowAdventureLog_Refresh();
    return 1;
}

// 原函数：0x004CBDB0。
static std::int32_t WindowAdventureLog_HandleNavigation(const ElementId elementId, const std::int32_t state)
{
    WindowInstance* const currentWindow = gCurrentWindow;
    if (!currentWindow) return 0;
    std::int32_t consumed = 0;
    switch (static_cast<WindowAdventureLogElementId>(elementId)) {
    case WindowAdventureLogElementId::HandleNavigation:
    case WindowAdventureLogElementId::HandleNavigation_4: {
        const std::int32_t base = elementId == static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation) ? 0x3B592 : 0x3B595;
        if ((state & MouseFlag_Hover) != 0) {
            gHoverTextD68AE8 = elementId == static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation) ? L"往上查看内容。" /* 原常量参考：0x005885EC。 */ : L"往下查看内容。" /* 原常量参考：0x005885DC。 */;
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = base + 2;
            consumed = 1;
        } else {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = base;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            const std::int32_t page = gLoginWindowRuntime.valueD67BB0;
            const bool canMove = elementId == static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation) ? page > 0 : page < gLoginWindowRuntime.logViewerMaximumD67BB4;
            if (canMove) {
                gLoginWindowRuntime.valueD67BB0 = page + (elementId == static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation) ? -1 : 1);
            }
            static_cast<void>(audio::PlaySe_52C450(canMove ? 51 : 55, 320));
            const std::int32_t currentPage = gLoginWindowRuntime.valueD67BB0;
            WindowInstance* const window = currentWindow;
            const std::int32_t maximum = gLoginWindowRuntime.logViewerMaximumD67BB4;
            auto* const handleVerticalScrollElement = window->FindElementById(WindowAdventureLogElementId::HandleVerticalScroll);
            if (handleVerticalScrollElement) {
                static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, currentPage));
            }
            consumed = 1;
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = base + 1;
            consumed = 1;
        }
        break;
    }
    case WindowAdventureLogElementId::HandleNavigation_7:
    case WindowAdventureLogElementId::HandleNavigation_8: {
        const std::int32_t base = elementId == static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation_7) ? 0x3B559 : 0x3B556;
        if ((state & MouseFlag_Hover) != 0) {
            gHoverTextD68AE8 = elementId == static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation_7) ? L"查看后一天的内容。" /* 原常量参考：0x005885C8。 */ : L"查看前一天的内容。" /* 原常量参考：0x005885B4。 */;
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = base + 2;
            consumed = 1;
        } else {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = base;
        }
        if ((state & MouseFlag_LRepeatDown) != 0) {
            std::FILE* const stream = gLoginWindowRuntime.logViewerFileD67BB8;
            if (stream != nullptr) {
                static_cast<void>(std::fclose(stream));
                // MC01-B：随后重载会处理该槽，已关闭FILE不得再交fclose。
                gLoginWindowRuntime.logViewerFileD67BB8 = nullptr;
            }
            if (elementId == static_cast<ElementId>(WindowAdventureLogElementId::HandleNavigation_7)) {
                std::tm* const calendar = gLoginWindowRuntime.calendarTimeD67BAC;
                const std::int32_t day = gLoginWindowRuntime.dayD67BC0;
                const std::int32_t month = calendar->tm_mon;
                if (month == 1 && (calendar->tm_mday == 29 || day == 29)) {
                    calendar->tm_mon = month + 1;
                    gLoginWindowRuntime.dayD67BC0 = 1;
                } else if ((month == 3 || month == 5 || month == 8 || month == 10)
                    && (calendar->tm_mday == 30 || day == 30)) {
                    calendar->tm_mon = month + 1;
                    gLoginWindowRuntime.dayD67BC0 = 1;
                } else if (calendar->tm_mday == 31 || day == 31) {
                    gLoginWindowRuntime.dayD67BC0 = 1;
                    const std::int32_t currentMonth = calendar->tm_mon;
                    if (currentMonth == 11) {
                        ++calendar->tm_year;
                        gLoginWindowRuntime.calendarTimeD67BAC->tm_mon = 0;
                    } else {
                        calendar->tm_mon = currentMonth + 1;
                    }
                } else {
                    gLoginWindowRuntime.dayD67BC0 = day + 1;
                }
            } else if (gLoginWindowRuntime.dayD67BC0 == 1) {
                std::tm* const calendar = gLoginWindowRuntime.calendarTimeD67BAC;
                const std::int32_t month = calendar->tm_mon;
                if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10) {
                    gLoginWindowRuntime.dayD67BC0 = 31;
                    --calendar->tm_mon;
                } else if (month == 2) {
                    gLoginWindowRuntime.dayD67BC0 = 29;
                    --calendar->tm_mon;
                } else if (month == 0) {
                    gLoginWindowRuntime.dayD67BC0 = 31;
                    --calendar->tm_year;
                    gLoginWindowRuntime.calendarTimeD67BAC->tm_mon = 12;
                    --gLoginWindowRuntime.calendarTimeD67BAC->tm_mon;
                } else {
                    gLoginWindowRuntime.dayD67BC0 = 30;
                    --calendar->tm_mon;
                }
            } else {
                --gLoginWindowRuntime.dayD67BC0;
            }
            WindowAdventureLog_Load();
            auto* const element5 = currentWindow->FindElementById(WindowAdventureLogElementId::Element5);
            if (element5) {
                auto* const title = element5->data.text;
                title->text = world::gLogFileName554648;
            }
            WindowInstance* const window = currentWindow;
            const std::int32_t maximum = gLoginWindowRuntime.logViewerMaximumD67BB4;
            gLoginWindowRuntime.valueD67BB0 = 0;
            auto* const handleVerticalScrollElement = window->FindElementById(WindowAdventureLogElementId::HandleVerticalScroll);
            if (handleVerticalScrollElement) {
                static_cast<void>(WindowScroll_SetPosition(handleVerticalScrollElement, maximum, 0));
            }
            static_cast<void>(audio::PlaySe_52C450(51, 320));
        }
        if ((state & MouseFlag_LHoldPhysical) != 0) {
            if (auto* const indexedElement = currentWindow->FindElementById(elementId); indexedElement) indexedElement->data.graphic->imageNo = base + 1;
        }
        break;
    }
    default:
        break;
    }
    return consumed;
}

} // namespace detail

} // namespace cg::ui
