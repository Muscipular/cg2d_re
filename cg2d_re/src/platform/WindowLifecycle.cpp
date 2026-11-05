#include "cg/platform/Startup.hpp"
#include "cg/core/m_log.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "WindowProcedureState.hpp"
#include "SessionWindow.hpp"
#include "StartupNative.hpp"
#include "MainLoopDependencies.hpp"
#include "ClientShutdown.hpp"
#include "cg/platform/LegacyCursor.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/session/ClientHost.hpp"
#include "../world/BattleState.hpp"

#include <Windows.h>
#include <dwmapi.h>

#include <algorithm>
#include <cstring>
#include <deque>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "cg/core/vm.h"

namespace cg::platform {

// 原465A30：改变系统桌面背景色；原颜色由启动链保存，ShutdownClient_457A40末尾恢复。
int SetWindowedDesktopColor()
{
    const INT element = COLOR_BACKGROUND;
    const COLORREF color = 0x00A4A0A0;
    return SetSysColors(1, &element, &color);
}

// 原 WndInit@0x4669F0；WO01固定窗口模式，保留创建及已有窗口的处理。
int InitializeWindow(session::ClientSession& session)
{
    // R02：首次 WndInit 前解析原进程参数；重建窗口复用已经发布的布局。
    static bool layoutInitialized = false;
    if (!layoutInitialized) {
        const char* const commandLine = GetCommandLineA();
        int width = 0;
        int height = 0;
        const char* option = std::strstr(commandLine, "/width:");
        if (option == nullptr) {
            option = std::strstr(commandLine, "/w:");
        }
        if (option != nullptr && std::sscanf(std::strchr(option, ':') + 1, "%d", &width) == 1) {
            gWindowWidth = width;
        }
        option = std::strstr(commandLine, "/height:");
        if (option == nullptr) {
            option = std::strstr(commandLine, "/h:");
        }
        if (option != nullptr && std::sscanf(std::strchr(option, ':') + 1, "%d", &height) == 1) {
            gWindowHeight = height;
        }
        ui::InitializeWindowLayout(session);
        m_log_iw(L"%d %d\n", width, height);
        layoutInitialized = true;
    }
    world::CenterBattleLayoutToWindow(session);
    const DWORD style = 0x00CA0000;
    RECT rect;
    SetWindowedDesktopColor();
    SetRect(&rect, 0, 0, gWindowWidth, gWindowHeight);
    AdjustSessionWindowRect(rect, gLegacyWindow != 0
        ? GetDpiForWindow(reinterpret_cast<HWND>(gLegacyWindow)) : GetDpiForSystem());

    if (gLegacyWindow) {
        // GWL_STYLE 的值仍是 32 位 LONG，不是需要 Ptr 宽化的窗口指针字段。
        SetWindowLongW(reinterpret_cast<HWND>(gLegacyWindow), GWL_STYLE, static_cast<LONG>(style));
        ShowWindow(reinterpret_cast<HWND>(gLegacyWindow), gShowCommand);
        SetWindowPos(
            reinterpret_cast<HWND>(gLegacyWindow), HWND_NOTOPMOST,
            gStartX, gStartY, rect.right - rect.left, rect.bottom - rect.top,
            SWP_FRAMECHANGED);
    } else {
        HWND window = CreateWindowExW(
            0, gWindowClassName, gWindowClassName, style,
            gStartX, gStartY, rect.right - rect.left, rect.bottom - rect.top,
            nullptr, nullptr, reinterpret_cast<HINSTANCE>(gInstance), nullptr);
        gLegacyWindow = reinterpret_cast<std::uintptr_t>(window);
    }

    // 原 0x466B56 的公共尾部；已有窗口也必须再次 ShowWindow。
    ShowWindow(reinterpret_cast<HWND>(gLegacyWindow), gShowCommand);
    return UpdateWindow(reinterpret_cast<HWND>(gLegacyWindow));
}

// 原 0x466D50；移除并派发整个队列，遇到 WM_QUIT 立即返回零。
int PumpWindowMessages()
{
    MSG message;
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            return 0;
        }
        if (message.message != WM_KEYDOWN
            || message.hwnd != reinterpret_cast<HWND>(gLegacyWindow)
            || RouteSessionKeyDown(static_cast<std::uint32_t>(message.wParam),
                static_cast<std::uint32_t>(message.lParam))) {
            TranslateMessage(&message);
        }
        DispatchMessageW(&message);
    }
    return 1;
}

namespace session_window_detail {

enum class CommandKind { None, New, Select, Close };
struct Command {
    CommandKind kind = CommandKind::None;
    std::uint64_t id = 0;
    bool operator==(const Command&) const = default;
};
enum class Button { None, Session, CloseSession, New, Previous, Next, Minimize, Close };
struct FrameTarget {
    Button button = Button::None;
    std::uint64_t id = 0;
    bool operator==(const FrameTarget&) const = default;
};

static HWND gTooltip = nullptr;
static HFONT gFont = nullptr;
static UINT gFontDpi = 0;
static bool gReady = false;
static bool gCaptionActive = true;
static FrameTarget gHovered;
static FrameTarget gPressed;
static bool gDragging = false;
static POINT gDragStart {};
static RECT gDragWindow {};
static UINT gConsumedMouseUps = 0;
static std::deque<Command> gCommands;
static std::vector<session::ClientHost::MenuEntry> gEntries;
static std::wstring gTitle = L"CG2D RE";
static std::size_t gFirstTab = 0;
static std::uint64_t gCurrentTab = 0;
static std::uint64_t gEntriesRefreshTick = 0; // 会话显示快照的毫秒刷新基线。
static bool gEntriesDirty = true;
static std::wstring gTooltipText;
static constexpr wchar_t kTooltipClass[] = L"CG2D_RE_SessionTabTip";

static UINT WindowDpi(HWND window) noexcept
{
    const UINT dpi = window != nullptr ? GetDpiForWindow(window) : GetDpiForSystem();
    return dpi != 0 ? dpi : USER_DEFAULT_SCREEN_DPI;
}

static int Scale(const int value, const UINT dpi) noexcept
{
    return MulDiv(value, static_cast<int>(dpi), USER_DEFAULT_SCREEN_DPI);
}

struct FrameLayout {
    RECT window;
    RECT title;
    RECT icon;
    RECT tabs;
    RECT add;
    RECT previous;
    RECT next;
    RECT minimize;
    RECT close;
    UINT dpi;
    int border;
    LONG tabWidth;
    std::size_t first;
    std::size_t count;
    std::size_t capacity;
};

static FrameLayout Layout(HWND window) noexcept
{
    FrameLayout value {};
    value.dpi = WindowDpi(window);
    value.border = std::max(1, Scale(1, value.dpi));
    static_cast<void>(GetWindowRect(window, &value.window));
    const LONG width = value.window.right - value.window.left;
    const int top = value.border;
    const int height = Scale(32, value.dpi);
    const int buttonWidth = Scale(44, value.dpi);
    value.title = {value.border, top, width - value.border, top + height};
    value.close = {width - value.border - buttonWidth, top, width - value.border, top + height};
    value.minimize = value.close;
    OffsetRect(&value.minimize, -buttonWidth, 0);
    const int toolWidth = Scale(28, value.dpi);
    value.next = {value.minimize.left - toolWidth, top, value.minimize.left, top + height};
    value.previous = value.next;
    OffsetRect(&value.previous, -toolWidth, 0);
    const int iconSize = Scale(18, value.dpi);
    const int iconTop = top + (height - iconSize) / 2;
    value.icon = {value.border + Scale(10, value.dpi), iconTop,
        value.border + Scale(10, value.dpi) + iconSize, iconTop + iconSize};
    const LONG left = value.icon.right + Scale(8, value.dpi);
    const LONG available = std::max<LONG>(1, value.previous.left - Scale(24 + 28, value.dpi) - left);
    value.capacity = std::min<std::size_t>(5, available);
    value.tabWidth = available / static_cast<LONG>(value.capacity);
    value.first = (gFirstTab / value.capacity) * value.capacity;
    if (!gEntries.empty() && value.first >= gEntries.size()) {
        value.first = ((gEntries.size() - 1) / value.capacity) * value.capacity;
    }
    value.count = gEntries.empty() ? 0 : std::min(value.capacity, gEntries.size() - value.first);
    value.tabs = {left, top, left + static_cast<LONG>(value.count) * value.tabWidth, top + height};
    value.add = {value.tabs.right, top, value.tabs.right + toolWidth, top + height};
    return value;
}

static RECT TabRect(const FrameLayout& layout, const std::size_t index) noexcept
{
    const LONG left = layout.tabs.left + static_cast<LONG>(index) * layout.tabWidth;
    return {left, layout.tabs.top, left + layout.tabWidth, layout.tabs.bottom};
}

static RECT TabCloseRect(const FrameLayout& layout, const RECT& tab) noexcept
{
    return {tab.right - Scale(24, layout.dpi), tab.top, tab.right, tab.bottom};
}

static const RECT& ButtonRect(const FrameLayout& layout, const Button button) noexcept
{
    switch (button) {
    case Button::New: return layout.add;
    case Button::Previous: return layout.previous;
    case Button::Next: return layout.next;
    case Button::Minimize: return layout.minimize;
    default: return layout.close;
    }
}

static bool ButtonEnabled(const FrameLayout& layout, const Button button) noexcept
{
    if (button == Button::Close || button == Button::Minimize) return true;
    if (!gReady) return false;
    if (button == Button::Previous) return layout.first != 0;
    if (button == Button::Next) return layout.first + layout.count < gEntries.size();
    return true;
}

static void UpdateRectangularRegion(HWND window) noexcept
{
    if (IsIconic(window)) return;
    RECT rectangle {};
    if (!GetWindowRect(window, &rectangle)) return;
    HRGN region = CreateRectRgn(0, 0,
        rectangle.right - rectangle.left, rectangle.bottom - rectangle.top);
    if (region == nullptr) return;
    HRGN current = CreateRectRgn(0, 0, 0, 0);
    if (current == nullptr) {
        DeleteObject(region);
        return;
    }
    const bool unchanged = GetWindowRgn(window, current) != ERROR && EqualRgn(current, region);
    DeleteObject(current);
    // SetWindowRgn会重入窗口位置消息；区域相同时不再提交，成功后的区域归系统所有。
    if (unchanged || SetWindowRgn(window, region, TRUE) == 0) {
        DeleteObject(region);
    }
}

static HFONT Font(const UINT dpi)
{
    if (gFontDpi != dpi) {
        NONCLIENTMETRICSW metrics {};
        metrics.cbSize = sizeof(metrics);
        if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0, dpi)) {
            if (HFONT replacement = CreateFontIndirectW(&metrics.lfMessageFont)) {
                if (gFont != nullptr) DeleteObject(gFont);
                gFont = replacement;
                gFontDpi = dpi;
            }
        }
    }
    return gFont != nullptr ? gFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
}

static void Fill(HDC dc, const RECT& rectangle, const COLORREF color) noexcept
{
    SetDCBrushColor(dc, color);
    FillRect(dc, &rectangle, static_cast<HBRUSH>(GetStockObject(DC_BRUSH)));
}

static void Line(HDC dc, const int x1, const int y1, const int x2, const int y2) noexcept
{
    MoveToEx(dc, x1, y1, nullptr);
    LineTo(dc, x2, y2);
}

static void Symbol(HDC dc, const RECT& rectangle, const Button button, const COLORREF color, const UINT dpi)
{
    const HGDIOBJ previous = SelectObject(dc, GetStockObject(DC_PEN));
    SetDCPenColor(dc, color);
    const int x = (rectangle.left + rectangle.right) / 2;
    const int y = (rectangle.top + rectangle.bottom) / 2;
    const int radius = std::max(3, Scale(5, dpi));
    if (button == Button::Close || button == Button::CloseSession) {
        Line(dc, x - radius, y - radius, x + radius + 1, y + radius + 1);
        Line(dc, x - radius, y + radius, x + radius + 1, y - radius - 1);
    } else if (button == Button::Minimize) {
        Line(dc, x - radius, y + 2, x + radius + 1, y + 2);
    } else if (button == Button::New) {
        Line(dc, x - radius, y, x + radius + 1, y);
        Line(dc, x, y - radius, x, y + radius + 1);
    } else if (button == Button::Previous || button == Button::Next) {
        const int direction = button == Button::Previous ? -1 : 1;
        Line(dc, x - direction * radius / 2, y - radius, x + direction * radius / 2, y);
        Line(dc, x + direction * radius / 2, y, x - direction * radius / 2, y + radius + 1);
    }
    SelectObject(dc, previous);
}

static FrameTarget HitButton(const FrameLayout& layout, const POINT point) noexcept
{
    for (const auto button : {Button::Close, Button::Minimize, Button::Next, Button::Previous, Button::New}) {
        if (PtInRect(&ButtonRect(layout, button), point)) return {button, 0};
    }
    for (std::size_t index = 0; index < layout.count; ++index) {
        const RECT tab = TabRect(layout, index);
        if (PtInRect(&tab, point)) {
            const RECT close = TabCloseRect(layout, tab);
            return {PtInRect(&close, point) ? Button::CloseSession : Button::Session,
                gEntries[layout.first + index].id};
        }
    }
    return {};
}

static POINT ScreenPoint(const LPARAM coordinates) noexcept
{
    return {static_cast<SHORT>(LOWORD(coordinates)), static_cast<SHORT>(HIWORD(coordinates))};
}

static LPARAM PackPoint(const POINT point) noexcept
{
    return MAKELPARAM(static_cast<WORD>(point.x), static_cast<WORD>(point.y));
}

static void PaintFrame(HWND window)
{
    if (IsIconic(window)) return;
    const FrameLayout layout = Layout(window);
    const LONG width = layout.window.right - layout.window.left;
    const LONG height = layout.window.bottom - layout.window.top;
    HDC target = GetWindowDC(window);
    if (target == nullptr) return;
    HDC dc = CreateCompatibleDC(target);
    BITMAPINFO info {};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(target, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dc == nullptr || bitmap == nullptr) {
        if (bitmap != nullptr) DeleteObject(bitmap);
        if (dc != nullptr) DeleteDC(dc);
        ReleaseDC(window, target);
        return;
    }
    const HGDIOBJ previousBitmap = SelectObject(dc, bitmap);
    if (previousBitmap == nullptr) {
        DeleteObject(bitmap);
        DeleteDC(dc);
        ReleaseDC(window, target);
        return;
    }
    const COLORREF border = RGB(74, 78, 82);
    Fill(dc, RECT {0, 0, width, height}, border);
    Fill(dc, layout.title, RGB(27, 29, 32));
    const COLORREF textColor = gCaptionActive ? RGB(240, 242, 244) : RGB(159, 164, 170);
    SelectObject(dc, Font(layout.dpi));
    SetBkMode(dc, TRANSPARENT);
    for (std::size_t index = 0; index < layout.count; ++index) {
        const auto& entry = gEntries[layout.first + index];
        const RECT tab = TabRect(layout, index);
        const RECT close = TabCloseRect(layout, tab);
        if (entry.selected || gHovered.id == entry.id || gPressed.id == entry.id) {
            Fill(dc, tab, entry.selected ? RGB(47, 51, 55) : RGB(37, 40, 44));
        }
        Fill(dc, RECT {tab.right - 1, tab.top + Scale(7, layout.dpi), tab.right,
            tab.bottom - Scale(7, layout.dpi)}, RGB(74, 78, 82));
        if (entry.selected) {
            Fill(dc, RECT {tab.left, tab.bottom - Scale(2, layout.dpi), tab.right, tab.bottom},
                RGB(62, 190, 143));
        }
        const FrameTarget closeTarget {Button::CloseSession, entry.id};
        if (gHovered == closeTarget || gPressed == closeTarget) Fill(dc, close, RGB(160, 45, 53));
        Symbol(dc, close, Button::CloseSession, textColor, layout.dpi);
        RECT label {tab.left + Scale(10, layout.dpi), tab.top, close.left - Scale(4, layout.dpi), tab.bottom};
        SetTextColor(dc, entry.disconnected ? RGB(229, 180, 93) : textColor);
        const std::wstring name = entry.label + (entry.disconnected ? L" [断线]" : L"") + entry.details;
        DrawTextW(dc, name.c_str(), static_cast<int>(name.size()), &label,
            DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    }
    for (const auto button : {Button::New, Button::Previous, Button::Next, Button::Minimize, Button::Close}) {
        const RECT& rect = ButtonRect(layout, button);
        const FrameTarget targetButton {button, 0};
        const bool enabled = ButtonEnabled(layout, button);
        if (enabled && (gHovered == targetButton || gPressed == targetButton)) {
            Fill(dc, rect, button == Button::Close ? RGB(190, 48, 59) : RGB(56, 61, 66));
        }
        Symbol(dc, rect, button, enabled ? textColor : RGB(94, 99, 104), layout.dpi);
    }
    DrawIconEx(dc, layout.icon.left, layout.icon.top, gWindowClass.hIcon,
        layout.icon.right - layout.icon.left, layout.icon.bottom - layout.icon.top, 0, nullptr, DI_NORMAL);
    // 透明DX11窗口仍按像素Alpha合成；普通GDI绘制后显式发布不透明的非客户区。
    if (GdiFlush()) {
        auto* const pixels = static_cast<std::uint32_t*>(bits);
        const std::size_t count = static_cast<std::size_t>(width) * height;
        for (std::size_t index = 0; index < count; ++index) {
            pixels[index] |= 0xFF000000u;
        }
        const RECT regions[] {
            {0, 0, width, layout.title.bottom},
            {0, layout.title.bottom, layout.border, height - layout.border},
            {width - layout.border, layout.title.bottom, width, height - layout.border},
            {0, height - layout.border, width, height}
        };
        for (const RECT& region : regions) {
            static_cast<void>(BitBlt(target, region.left, region.top, region.right - region.left,
                region.bottom - region.top, dc, region.left, region.top, SRCCOPY));
        }
    }
    SelectObject(dc, previousBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
    ReleaseDC(window, target);
}

static void Queue(const Command command)
{
    try {
        gCommands.push_back(command);
    } catch (...) {
        ShowWideMessage(L"无法提交会话操作。", L"确认", MB_OK | MB_ICONERROR);
    }
}

static void HideTooltip() noexcept
{
    if (gTooltip != nullptr) ShowWindow(gTooltip, SW_HIDE);
}

static LRESULT CALLBACK TooltipProcedure(HWND window, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
    if (message == WM_NCHITTEST) return HTTRANSPARENT;
    if (message == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    if (message == WM_ERASEBKGND) return 1;
    if (message == WM_NCDESTROY && gTooltip == window) gTooltip = nullptr;
    if (message == WM_PAINT) {
        PAINTSTRUCT paint {};
        HDC dc = BeginPaint(window, &paint);
        RECT rectangle {};
        GetClientRect(window, &rectangle);
        Fill(dc, rectangle, RGB(74, 78, 82));
        InflateRect(&rectangle, -1, -1);
        Fill(dc, rectangle, RGB(248, 249, 250));
        const UINT dpi = WindowDpi(reinterpret_cast<HWND>(gLegacyWindow));
        InflateRect(&rectangle, -Scale(8, dpi), -Scale(5, dpi));
        const HGDIOBJ font = SelectObject(dc, Font(dpi));
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(28, 36, 42));
        DrawTextW(dc, gTooltipText.c_str(), static_cast<int>(gTooltipText.size()), &rectangle,
            DT_WORDBREAK | DT_NOPREFIX);
        SelectObject(dc, font);
        EndPaint(window, &paint);
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

static void ShowTooltip(HWND window, const POINT point)
{
    std::wstring text;
    for (const auto& entry : gEntries) {
        if (entry.id == gHovered.id) {
            text = gHovered.button == Button::CloseSession ? L"关闭会话：" : L"";
            text += entry.label + entry.details + (entry.disconnected ? L" [断线]" : L"");
            break;
        }
    }
    if (gHovered.button == Button::New) text = L"新增会话";
    else if (gHovered.button == Button::Previous) text = L"上一页";
    else if (gHovered.button == Button::Next) text = L"下一页";
    else if (gHovered.button == Button::Minimize) text = L"最小化";
    else if (gHovered.button == Button::Close) text = L"退出程序";
    if (text.empty()) {
        HideTooltip();
        return;
    }
    MONITORINFO monitor {};
    monitor.cbSize = sizeof(monitor);
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor)) return;
    const UINT dpi = WindowDpi(window);
    HDC dc = GetWindowDC(window);
    if (dc == nullptr) return;
    const HGDIOBJ font = SelectObject(dc, Font(dpi));
    RECT rectangle {0, 0, std::min<LONG>(Scale(420, dpi),
        monitor.rcWork.right - monitor.rcWork.left - Scale(20, dpi)), 0};
    DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &rectangle, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
    SelectObject(dc, font);
    ReleaseDC(window, dc);
    if (gTooltip == nullptr) {
        WNDCLASSW type {};
        type.lpfnWndProc = TooltipProcedure;
        type.hInstance = reinterpret_cast<HINSTANCE>(gInstance);
        type.lpszClassName = kTooltipClass;
        WNDCLASSW existing {};
        if (!GetClassInfoW(type.hInstance, kTooltipClass, &existing) && RegisterClassW(&type) == 0) return;
        gTooltip = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kTooltipClass, L"",
            WS_POPUP, 0, 0, 0, 0, window, nullptr, type.hInstance, nullptr);
        if (gTooltip == nullptr) return;
    }
    gTooltipText = std::move(text);
    const LONG width = rectangle.right - rectangle.left + Scale(16, dpi) + 2;
    const LONG height = rectangle.bottom - rectangle.top + Scale(10, dpi) + 2;
    const LONG x = std::clamp(point.x, monitor.rcWork.left, monitor.rcWork.right - width);
    const FrameLayout layout = Layout(window);
    const LONG y = std::clamp(layout.window.top + layout.title.bottom + Scale(4, dpi), monitor.rcWork.top,
        std::max(monitor.rcWork.top, monitor.rcWork.bottom - height));
    SetWindowPos(gTooltip, HWND_TOP, x, y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(gTooltip, nullptr, FALSE);
}

static void RefreshCaptionEntries(HWND window, const bool revealCurrent, const bool preserveTooltip = false)
{
    RECT tooltip {};
    const bool tooltipVisible = preserveTooltip && gTooltip != nullptr && IsWindowVisible(gTooltip)
        && GetWindowRect(gTooltip, &tooltip);
    if (!tooltipVisible) HideTooltip();
    auto& host = session::GetClientHost();
    gEntries = host.MenuEntries();
    gCurrentTab = host.CurrentId();
    gEntriesRefreshTick = ReadTickCount64();
    gEntriesDirty = false;
    const FrameLayout layout = Layout(window);
    gFirstTab = layout.first;
    if (revealCurrent) {
        for (std::size_t index = 0; index < gEntries.size(); ++index) {
            if (gEntries[index].selected && (index < layout.first || index >= layout.first + layout.count)) {
                gFirstTab = (index / layout.capacity) * layout.capacity;
                break;
            }
        }
    }
    PaintFrame(window);
    if (tooltipVisible) ShowTooltip(window, POINT {tooltip.left, tooltip.top});
}

static void ChangeTabPage(HWND window, const bool forward)
{
    RefreshCaptionEntries(window, false);
    const FrameLayout layout = Layout(window);
    if (forward && ButtonEnabled(layout, Button::Next)) gFirstTab = layout.first + layout.capacity;
    else if (!forward && ButtonEnabled(layout, Button::Previous))
        gFirstTab = layout.first - std::min(layout.first, layout.capacity);
    gHovered = {};
    PaintFrame(window);
}

} // namespace session_window_detail

void AdjustSessionWindowRect(RECT& rectangle, const UINT dpi) noexcept
{
    const int border = std::max(1, session_window_detail::Scale(1, dpi));
    rectangle.left -= border;
    rectangle.right += border;
    rectangle.top -= border + session_window_detail::Scale(32, dpi);
    rectangle.bottom += border;
}

bool HandleSessionWindowMessage(HWND window, const UINT message, const WPARAM wParam, const LPARAM lParam, LRESULT& result)
{
    using namespace session_window_detail;
    result = 0;
    switch (message) {
    case WM_CREATE:
        if (HMODULE module = LoadLibraryExW(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) {
            const auto setAttribute = reinterpret_cast<decltype(&DwmSetWindowAttribute)>(
                GetProcAddress(module, "DwmSetWindowAttribute"));
            if (setAttribute != nullptr) {
                const DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_DONOTROUND;
                static_cast<void>(setAttribute(window, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner)));
            }
            FreeLibrary(module);
        }
        UpdateRectangularRegion(window);
        return false;
    case WM_WINDOWPOSCHANGED: {
        const auto& position = *reinterpret_cast<const WINDOWPOS*>(lParam);
        if ((position.flags & SWP_NOSIZE) == 0 || (position.flags & SWP_FRAMECHANGED) != 0) {
            UpdateRectangularRegion(window);
        }
        return false;
    }
    case WM_NCCALCSIZE: {
        RECT& rectangle = wParam != 0 ? reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam)->rgrc[0]
            : *reinterpret_cast<RECT*>(lParam);
        const UINT dpi = WindowDpi(window);
        const int border = std::max(1, Scale(1, dpi));
        rectangle.left += border;
        rectangle.right -= border;
        rectangle.top += border + Scale(32, dpi);
        rectangle.bottom -= border;
        return true;
    }
    case WM_NCPAINT:
        if (IsIconic(window)) return false;
        PaintFrame(window);
        return true;
    case WM_NCACTIVATE:
        gCaptionActive = wParam != 0;
        if (IsIconic(window)) return false;
        result = DefWindowProcW(window, message, wParam, -1);
        PaintFrame(window);
        return true;
    case WM_SETTEXT:
    case WM_SETICON:
        result = DefWindowProcW(window, message, wParam, lParam);
        PaintFrame(window);
        return true;
    case WM_NCHITTEST: {
        const FrameLayout layout = Layout(window);
        POINT point = ScreenPoint(lParam);
        point.x -= layout.window.left;
        point.y -= layout.window.top;
        const FrameTarget target = HitButton(layout, point);
        if (target.button == Button::Close) result = HTCLOSE;
        else if (target.button == Button::Minimize) result = HTMINBUTTON;
        else if (target.button != Button::None) result = HTMENU;
        else if (PtInRect(&layout.icon, point)) result = HTSYSMENU;
        else if (PtInRect(&layout.title, point)) result = HTCAPTION;
        else {
            const LONG width = layout.window.right - layout.window.left;
            const LONG height = layout.window.bottom - layout.window.top;
            result = point.x < layout.border || point.x >= width - layout.border
                || point.y < layout.title.bottom || point.y >= height - layout.border ? HTBORDER : HTCLIENT;
        }
        return true;
    }
    case WM_GETMINMAXINFO: {
        RECT rectangle {0, 0, gWindowWidth, gWindowHeight};
        AdjustSessionWindowRect(rectangle, WindowDpi(window));
        auto& limits = *reinterpret_cast<MINMAXINFO*>(lParam);
        limits.ptMinTrackSize = {rectangle.right - rectangle.left, rectangle.bottom - rectangle.top};
        limits.ptMaxTrackSize = limits.ptMinTrackSize;
        return true;
    }
    case WM_DPICHANGED: {
        HideTooltip();
        RECT rectangle {0, 0, gWindowWidth, gWindowHeight};
        AdjustSessionWindowRect(rectangle, LOWORD(wParam));
        const auto& suggested = *reinterpret_cast<const RECT*>(lParam);
        SetWindowPos(window, nullptr, suggested.left, suggested.top,
            rectangle.right - rectangle.left, rectangle.bottom - rectangle.top,
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
        RefreshCaptionEntries(window, true);
        return true;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) {
            HideTooltip();
            gHovered = {};
        }
        return false;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0u) == SC_MAXIMIZE || (wParam & 0xFFF0u) == SC_SIZE) return true;
        HideTooltip();
        return false;
    case WM_NCMOUSEMOVE: {
        const FrameLayout layout = Layout(window);
        POINT point = ScreenPoint(lParam);
        point.x -= layout.window.left;
        point.y -= layout.window.top;
        FrameTarget hovered = HitButton(layout, point);
        if (hovered != gHovered) {
            if (hovered.button == Button::Session || hovered.button == Button::CloseSession) {
                RefreshCaptionEntries(window, false);
                hovered = HitButton(Layout(window), point);
            }
            gHovered = hovered;
            if (gPressed.button == Button::None && !gDragging) ShowTooltip(window, ScreenPoint(lParam));
            PaintFrame(window);
        }
        TRACKMOUSEEVENT track {sizeof(track), TME_LEAVE | TME_NONCLIENT, window, 0};
        TrackMouseEvent(&track);
        return false;
    }
    case WM_NCMOUSELEAVE:
        HideTooltip();
        gHovered = {};
        PaintFrame(window);
        return true;
    case WM_NCLBUTTONDBLCLK:
        if (wParam == HTCAPTION) {
            gConsumedMouseUps |= 1u;
            return true;
        }
        [[fallthrough]];
    case WM_NCLBUTTONDOWN: {
        HideTooltip();
        const FrameLayout layout = Layout(window);
        POINT point = ScreenPoint(lParam);
        gDragStart = point;
        point.x -= layout.window.left;
        point.y -= layout.window.top;
        gPressed = HitButton(layout, point);
        if (gPressed.button != Button::None && !ButtonEnabled(layout, gPressed.button)) {
            gPressed = {};
            gConsumedMouseUps |= 1u;
            return true;
        }
        if (gPressed.button != Button::None || wParam == HTCAPTION) {
            gDragging = gPressed.button == Button::None;
            gDragWindow = layout.window;
            SetCapture(window);
            SetSystemCursorVisible(true);
            PaintFrame(window);
            return true;
        }
        return false;
    }
    case WM_MOUSEMOVE:
        if (gDragging || gPressed.button != Button::None) {
            POINT point {};
            GetCursorPos(&point);
            if (gDragging) {
                SetWindowPos(window, nullptr, gDragWindow.left + point.x - gDragStart.x,
                    gDragWindow.top + point.y - gDragStart.y, 0, 0,
                    SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
            } else {
                const FrameLayout layout = Layout(window);
                point.x -= layout.window.left;
                point.y -= layout.window.top;
                gHovered = HitButton(layout, point);
                PaintFrame(window);
            }
            return true;
        }
        return false;
    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
        if (gDragging || gPressed.button != Button::None || (gConsumedMouseUps & 1u) != 0) {
            const FrameTarget pressed = gPressed;
            const bool dragging = gDragging;
            gPressed = {};
            gDragging = false;
            gConsumedMouseUps &= ~1u;
            if (GetCapture() == window) ReleaseCapture();
            POINT point = ScreenPoint(lParam);
            if (message == WM_LBUTTONUP) ClientToScreen(window, &point);
            RouteSessionMouseRelease(true, PackPoint(point));
            const FrameLayout layout = Layout(window);
            point.x -= layout.window.left;
            point.y -= layout.window.top;
            if (!dragging && pressed.button != Button::None && pressed == HitButton(layout, point)
                && ButtonEnabled(layout, pressed.button)) {
                switch (pressed.button) {
                case Button::Session: Queue({CommandKind::Select, pressed.id}); break;
                case Button::CloseSession: Queue({CommandKind::Close, pressed.id}); break;
                case Button::New: Queue({CommandKind::New, 0}); break;
                case Button::Previous: ChangeTabPage(window, false); break;
                case Button::Next: ChangeTabPage(window, true); break;
                case Button::Close: PostMessageW(window, WM_SYSCOMMAND, SC_CLOSE, 0); break;
                case Button::Minimize: PostMessageW(window, WM_SYSCOMMAND, SC_MINIMIZE, 0); break;
                default: break;
                }
            }
            PaintFrame(window);
            return true;
        }
        return false;
    case WM_RBUTTONUP:
    case WM_NCRBUTTONUP:
        if ((gConsumedMouseUps & 2u) != 0) {
            gConsumedMouseUps &= ~2u;
            POINT point = ScreenPoint(lParam);
            if (message == WM_RBUTTONUP) ClientToScreen(window, &point);
            RouteSessionMouseRelease(false, PackPoint(point));
            return true;
        }
        return false;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK: {
        const UINT button = message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK ? 1u : 2u;
        HideTooltip();
        gConsumedMouseUps &= ~button;
        return false;
    }
    case WM_CAPTURECHANGED:
        if (gPressed.button != Button::None || gDragging) gConsumedMouseUps |= 1u;
        gPressed = {};
        gDragging = false;
        PaintFrame(window);
        return false;
    case WM_SETCURSOR:
        if (gDragging || gPressed.button != Button::None) {
            SetSystemCursorVisible(true);
            SetCursor(LoadCursorA(nullptr, IDC_ARROW));
            result = TRUE;
            return true;
        }
        return false;
    case WM_MOUSEWHEEL: {
        const FrameLayout layout = Layout(window);
        POINT point = ScreenPoint(lParam);
        point.x -= layout.window.left;
        point.y -= layout.window.top;
        return PtInRect(&layout.title, point) != FALSE;
    }
    case WM_NCDESTROY:
        ShutdownSessionWindow();
        return false;
    default:
        return false;
    }
}

bool ProcessSessionWindowCommands()
{
    using namespace session_window_detail;
    auto& host = session::GetClientHost();
    const bool hadCommands = !gCommands.empty();
    while (!gCommands.empty()) {
        const Command command = gCommands.front();
        gCommands.pop_front();
        if (command.kind == CommandKind::New) {
            static_cast<void>(CreateClientSession());
            continue;
        }
        auto entry = std::find_if(host.Sessions().begin(), host.Sessions().end(),
            [&](const auto& item) { return item.id == command.id && item.initialized; });
        if (entry == host.Sessions().end()) continue;
        if (command.kind == CommandKind::Select) {
            if (!host.SelectSession(command.id)) {
                ShowWideMessage(L"切换会话失败，已保留原会话。", L"确认", MB_OK | MB_ICONERROR);
            }
        } else if (command.kind == CommandKind::Close) {
            if (host.CurrentId() == command.id && host.SessionCount() > 1) {
                auto adjacent = std::next(entry);
                if (adjacent == host.Sessions().end()) adjacent = std::prev(entry);
                if (!host.SelectSession(adjacent->id)) {
                    ShowWideMessage(L"无法切换相邻会话，本次未关闭。", L"确认", MB_OK | MB_ICONERROR);
                    continue;
                }
            }
            ShutdownSessionRuntime(entry->session, true);
            static_cast<void>(host.RemoveSession(command.id));
            if (host.SessionCount() == 0) return false;
        }
    }
    if (hadCommands) gEntriesDirty = true;
    return true;
}

void RefreshSessionWindow()
{
    using namespace session_window_detail;
    gReady = true;
    VM_LIGHT_START
    auto& host = session::GetClientHost();
    std::wstring title = L"CG2D RE";
    if (const auto* current = host.CurrentSession()) {
        title += L" - ";
        title += host.CurrentLabel();
        if (current->Disconnected()) title += L" [断线]";
        title += L" // 开启者，你今天爆揍一顿小吸兽了吗？";
    }
    const HWND window = reinterpret_cast<HWND>(gLegacyWindow);
    const bool titleChanged = title != gTitle;
    if (titleChanged) {
        gTitle = title;
        SetWindowTextW(window, title.c_str());
    }
    const bool entriesChanged = gEntriesDirty || titleChanged || gCurrentTab != host.CurrentId()
        || gEntries.size() != host.SessionCount();
    if (entriesChanged || ReadTickCount64() - gEntriesRefreshTick >= 1000) {
        RefreshCaptionEntries(window, gCurrentTab != host.CurrentId(), !entriesChanged);
    }
    UpdateSystemCursorLocation(IsPointerInGameClient());
    VM_LIGHT_END
}

void ShutdownSessionWindow() noexcept
{
    using namespace session_window_detail;
    gReady = false;
    HideTooltip();
    if (gTooltip != nullptr) {
        DestroyWindow(gTooltip);
        gTooltip = nullptr;
    }
    if (gDragging || gPressed.button != Button::None) {
        gDragging = false;
        gPressed = {};
        if (GetCapture() == reinterpret_cast<HWND>(gLegacyWindow)) ReleaseCapture();
    }
    gCommands.clear();
    gEntries.clear();
    gHovered = {};
    gFirstTab = 0;
    gCurrentTab = 0;
    gEntriesRefreshTick = 0;
    gEntriesDirty = true;
    gTooltipText.clear();
    gConsumedMouseUps = 0;
    if (gFont != nullptr) {
        DeleteObject(gFont);
        gFont = nullptr;
        gFontDpi = 0;
    }
    SetSystemCursorVisible(true);
}

} // namespace cg::platform
