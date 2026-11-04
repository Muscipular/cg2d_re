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
#include "cg/session/ClientHost.hpp"
#include "../world/BattleState.hpp"

#include <Windows.h>
#include <dwmapi.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <deque>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

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
            || (CanTranslateSessionWindowKey(static_cast<UINT>(message.wParam))
                && RouteSessionKeyDown(static_cast<std::uint32_t>(message.wParam),
                    static_cast<std::uint32_t>(message.lParam)))) {
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
enum class Button { None, Sessions, Minimize, Close };

static HWND gPopup = nullptr;
static HFONT gFont = nullptr;
static UINT gFontDpi = 0;
static bool gReady = false;
static bool gCaptionActive = true;
static Button gHovered = Button::None;
static Button gPressed = Button::None;
static bool gDragging = false;
static POINT gDragStart {};
static RECT gDragWindow {};
static std::array<bool, 256> gMenuKeys {};
static UINT gConsumedMouseUps = 0;
static std::deque<Command> gCommands;
static std::vector<session::ClientHost::MenuEntry> gEntries;
static std::wstring gTitle = L"CG2D RE";
static std::size_t gFirstRow = 0;
static std::size_t gVisibleRows = 0;
static Command gMenuHover;
static Command gMenuPressed;
static int gWheelRemainder = 0;
static bool gScrolling = false;
static int gScrollGrab = 0;
static constexpr wchar_t kPopupClass[] = L"CG2D_RE_SessionMenu";

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
    RECT text;
    RECT sessions;
    RECT minimize;
    RECT close;
    UINT dpi;
    int border;
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
    value.sessions = {value.minimize.left - Scale(112, value.dpi), top,
        value.minimize.left, top + height};
    const int iconSize = Scale(18, value.dpi);
    const int iconTop = top + (height - iconSize) / 2;
    value.icon = {value.border + Scale(10, value.dpi), iconTop,
        value.border + Scale(10, value.dpi) + iconSize, iconTop + iconSize};
    value.text = {value.icon.right + Scale(8, value.dpi), top,
        std::max(value.icon.right, value.sessions.left - Scale(8, value.dpi)), top + height};
    return value;
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
    if (button == Button::Close) {
        Line(dc, x - radius, y - radius, x + radius + 1, y + radius + 1);
        Line(dc, x - radius, y + radius, x + radius + 1, y - radius - 1);
    } else if (button == Button::Minimize) {
        Line(dc, x - radius, y + 2, x + radius + 1, y + 2);
    } else if (button == Button::Sessions) {
        Line(dc, x - radius, y - 2, x, y + radius - 2);
        Line(dc, x, y + radius - 2, x + radius + 1, y - 3);
    }
    SelectObject(dc, previous);
}

static Button HitButton(const FrameLayout& layout, const POINT point) noexcept
{
    if (PtInRect(&layout.close, point)) return Button::Close;
    if (PtInRect(&layout.minimize, point)) return Button::Minimize;
    if (gReady && PtInRect(&layout.sessions, point)) return Button::Sessions;
    return Button::None;
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
    for (const auto button : {Button::Sessions, Button::Minimize, Button::Close}) {
        const RECT& rect = button == Button::Sessions ? layout.sessions
            : button == Button::Minimize ? layout.minimize : layout.close;
        if (gHovered == button || gPressed == button) {
            Fill(dc, rect, button == Button::Close ? RGB(190, 48, 59) : RGB(56, 61, 66));
        }
        RECT symbol = rect;
        if (button == Button::Sessions) symbol.right = symbol.left + Scale(32, layout.dpi);
        Symbol(dc, symbol, button, button == Button::Sessions && !gReady ? RGB(94, 99, 104) : textColor, layout.dpi);
    }
    DrawIconEx(dc, layout.icon.left, layout.icon.top, gWindowClass.hIcon,
        layout.icon.right - layout.icon.left, layout.icon.bottom - layout.icon.top, 0, nullptr, DI_NORMAL);
    SelectObject(dc, Font(layout.dpi));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, gReady ? textColor : RGB(94, 99, 104));
    RECT menuText = layout.sessions;
    menuText.left += Scale(32, layout.dpi);
    menuText.right -= Scale(12, layout.dpi);
    DrawTextW(dc, L"多开菜单", -1, &menuText,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    SetTextColor(dc, textColor);
    RECT text = layout.text;
    DrawTextW(dc, gTitle.c_str(), static_cast<int>(gTitle.size()), &text,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
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

static int AddHeight() noexcept { return Scale(36, WindowDpi(reinterpret_cast<HWND>(gLegacyWindow))); }
static int RowHeight() noexcept { return Scale(32, WindowDpi(reinterpret_cast<HWND>(gLegacyWindow))); }

static void CloseMenu() noexcept
{
    const HWND popup = gPopup;
    gPopup = nullptr;
    gMenuHover = {};
    gMenuPressed = {};
    gScrolling = false;
    if (popup != nullptr) {
        if (GetCapture() == popup) ReleaseCapture();
        DestroyWindow(popup);
    }
}

static void Queue(const Command command)
{
    try {
        gCommands.push_back(command);
    } catch (...) {
        ShowWideMessage(L"无法提交会话操作。", L"确认", MB_OK | MB_ICONERROR);
    }
}

static RECT MenuContent(HWND popup) noexcept
{
    RECT rectangle {};
    GetClientRect(popup, &rectangle);
    if (gEntries.size() > gVisibleRows) {
        rectangle.right -= Scale(14, WindowDpi(reinterpret_cast<HWND>(gLegacyWindow)));
    }
    return rectangle;
}

static RECT ScrollThumb(HWND popup) noexcept
{
    RECT client {};
    GetClientRect(popup, &client);
    const RECT content = MenuContent(popup);
    const int top = AddHeight() + 1;
    const int height = client.bottom - top;
    if (gEntries.size() <= gVisibleRows) return {};
    const LONG thumbHeight = std::min<LONG>(height, std::max<LONG>(Scale(18,
        WindowDpi(reinterpret_cast<HWND>(gLegacyWindow))),
        static_cast<LONG>(static_cast<std::size_t>(height) * gVisibleRows / gEntries.size())));
    const LONG y = top + static_cast<LONG>(
        static_cast<std::size_t>(height - thumbHeight) * gFirstRow / (gEntries.size() - gVisibleRows));
    return {content.right + 2, y, client.right - 2, y + thumbHeight};
}

static Command MenuHit(HWND popup, const POINT point) noexcept
{
    RECT bounds {};
    GetClientRect(popup, &bounds);
    if (!PtInRect(&bounds, point)) return {};
    if (point.y < AddHeight()) return {CommandKind::New, 0};
    const RECT client = MenuContent(popup);
    if (point.x >= client.right) return {};
    const auto row = gFirstRow + static_cast<std::size_t>((point.y - AddHeight() - 1) / RowHeight());
    if (row >= gEntries.size() || point.y == AddHeight()) return {};
    return {point.x >= client.right - RowHeight() ? CommandKind::Close : CommandKind::Select, gEntries[row].id};
}

static void UpdateScroll(HWND popup)
{
    const std::size_t maximum = gEntries.size() > gVisibleRows ? gEntries.size() - gVisibleRows : 0;
    gFirstRow = std::min(gFirstRow, maximum);
    InvalidateRect(popup, nullptr, FALSE);
}

static void Scroll(HWND popup, const int amount)
{
    if (amount < 0) {
        const auto distance = static_cast<std::size_t>(-amount);
        gFirstRow = distance > gFirstRow ? 0 : gFirstRow - distance;
    } else {
        gFirstRow += static_cast<std::size_t>(amount);
    }
    gMenuHover = {};
    UpdateScroll(popup);
}

static void PaintMenu(HWND popup)
{
    PAINTSTRUCT paint {};
    HDC dc = BeginPaint(popup, &paint);
    RECT client {};
    GetClientRect(popup, &client);
    Fill(dc, client, RGB(248, 249, 250));
    const RECT content = MenuContent(popup);
    const UINT dpi = WindowDpi(reinterpret_cast<HWND>(gLegacyWindow));
    const HGDIOBJ previousFont = SelectObject(dc, Font(dpi));
    const HGDIOBJ previousPen = SelectObject(dc, GetStockObject(DC_PEN));
    SetBkMode(dc, TRANSPARENT);
    RECT add {0, 0, client.right, AddHeight()};
    if (gMenuHover.kind == CommandKind::New) Fill(dc, add, RGB(230, 234, 238));
    const int iconX = Scale(16, dpi);
    const int addY = AddHeight() / 2;
    SetDCPenColor(dc, RGB(30, 128, 86));
    Line(dc, iconX - Scale(4, dpi), addY, iconX + Scale(5, dpi), addY);
    Line(dc, iconX, addY - Scale(4, dpi), iconX, addY + Scale(5, dpi));
    RECT addText {Scale(34, dpi), 0, client.right, AddHeight()};
    SetTextColor(dc, RGB(28, 36, 42));
    DrawTextW(dc, L"新增", -1, &addText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
    Fill(dc, RECT {0, AddHeight(), client.right, AddHeight() + 1}, RGB(214, 219, 224));
    for (std::size_t row = 0; row < gVisibleRows && gFirstRow + row < gEntries.size(); ++row) {
        const auto& entry = gEntries[gFirstRow + row];
        const LONG top = AddHeight() + 1 + static_cast<LONG>(row) * RowHeight();
        RECT rectangle {0, top, content.right, top + RowHeight()};
        if (entry.selected) Fill(dc, rectangle, RGB(222, 243, 233));
        if (gMenuHover.id == entry.id && gMenuHover.kind != CommandKind::None) {
            Fill(dc, rectangle, RGB(230, 234, 238));
        }
        if (entry.selected) {
            const int y = (rectangle.top + rectangle.bottom) / 2;
            SetDCPenColor(dc, RGB(30, 128, 86));
            Line(dc, iconX - Scale(5, dpi), y, iconX - Scale(1, dpi), y + Scale(4, dpi));
            Line(dc, iconX - Scale(1, dpi), y + Scale(4, dpi), iconX + Scale(6, dpi), y - Scale(4, dpi));
        }
        RECT close {content.right - RowHeight(), top, content.right, rectangle.bottom};
        if (gMenuHover.id == entry.id && gMenuHover.kind == CommandKind::Close) {
            Fill(dc, close, RGB(249, 217, 220));
        }
        Symbol(dc, close, Button::Close, RGB(104, 111, 119), dpi);
        RECT label {Scale(34, dpi), top, close.left - Scale(8, dpi), rectangle.bottom};
        if (entry.disconnected) {
            RECT state = label;
            state.left = std::max(state.left, state.right - Scale(40, dpi));
            SetTextColor(dc, RGB(171, 111, 29));
            DrawTextW(dc, L"断线", -1, &state, DT_SINGLELINE | DT_VCENTER | DT_RIGHT | DT_NOPREFIX);
            label.right = state.left - Scale(6, dpi);
        }
        SetTextColor(dc, RGB(28, 36, 42));
        DrawTextW(dc, entry.label.c_str(), static_cast<int>(entry.label.size()), &label,
            DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
    }
    if (content.right != client.right) {
        Fill(dc, RECT {content.right, AddHeight() + 1, client.right, client.bottom}, RGB(235, 238, 241));
        Fill(dc, ScrollThumb(popup), gScrolling ? RGB(112, 121, 130) : RGB(163, 171, 179));
    }
    SelectObject(dc, previousPen);
    SelectObject(dc, previousFont);
    EndPaint(popup, &paint);
}

static void MenuKey(const UINT key)
{
    if (key == VK_ESCAPE) {
        CloseMenu();
        return;
    }
    if (key == VK_RETURN && gMenuHover.kind != CommandKind::None) {
        Queue(gMenuHover);
        CloseMenu();
        return;
    }
    std::size_t index = 0;
    for (std::size_t i = 0; i < gEntries.size(); ++i) {
        if (gEntries[i].id == gMenuHover.id) index = i + 1;
    }
    if (key == VK_DOWN) index = std::min(index + 1, gEntries.size());
    else if (key == VK_UP) index = index > 0 ? index - 1 : 0;
    else if (key == VK_HOME) index = 0;
    else if (key == VK_END) index = gEntries.size();
    else return;
    gMenuHover = index == 0 ? Command {CommandKind::New, 0}
        : Command {CommandKind::Select, gEntries[index - 1].id};
    if (index != 0) {
        if (index - 1 < gFirstRow) gFirstRow = index - 1;
        else if (index > gFirstRow + gVisibleRows) gFirstRow = index - gVisibleRows;
    }
    UpdateScroll(gPopup);
}

static LRESULT CALLBACK MenuProcedure(HWND popup, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
    switch (message) {
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: PaintMenu(popup); return 0;
    case WM_MOUSEMOVE: {
        SetSystemCursorVisible(true);
        if (gScrolling) {
            if (gScrollGrab < 0) return 0;
            RECT client {};
            GetClientRect(popup, &client);
            const RECT thumb = ScrollThumb(popup);
            const int travel = client.bottom - AddHeight() - 1 - (thumb.bottom - thumb.top);
            if (travel > 0) {
                const LONG y = std::clamp<LONG>(ScreenPoint(lParam).y - gScrollGrab - AddHeight() - 1, 0, travel);
                gFirstRow = static_cast<std::size_t>(y) * (gEntries.size() - gVisibleRows)
                    / static_cast<std::size_t>(travel);
                UpdateScroll(popup);
            }
            return 0;
        }
        const Command hover = MenuHit(popup, ScreenPoint(lParam));
        if (!(hover == gMenuHover)) {
            gMenuHover = hover;
            InvalidateRect(popup, nullptr, FALSE);
        }
        TRACKMOUSEEVENT track {sizeof(track), TME_LEAVE, popup, 0};
        TrackMouseEvent(&track);
        return 0;
    }
    case WM_MOUSELEAVE:
        gMenuHover = {};
        InvalidateRect(popup, nullptr, FALSE);
        return 0;
    case WM_LBUTTONDOWN: {
        const POINT point = ScreenPoint(lParam);
        const RECT content = MenuContent(popup);
        if (gEntries.size() > gVisibleRows && point.x >= content.right && point.y > AddHeight()) {
            const RECT thumb = ScrollThumb(popup);
            gScrolling = true;
            SetCapture(popup);
            if (PtInRect(&thumb, point)) {
                gScrollGrab = point.y - thumb.top;
                InvalidateRect(popup, nullptr, FALSE);
            } else {
                gScrollGrab = -1;
                Scroll(popup, point.y < thumb.top ? -static_cast<int>(gVisibleRows) : static_cast<int>(gVisibleRows));
            }
            return 0;
        }
        gMenuPressed = MenuHit(popup, ScreenPoint(lParam));
        if (gMenuPressed.kind != CommandKind::None) SetCapture(popup);
        return 0;
    }
    case WM_LBUTTONUP: {
        const bool scrolling = gScrolling;
        gScrolling = false;
        const Command pressed = gMenuPressed;
        const Command released = MenuHit(popup, ScreenPoint(lParam));
        gMenuPressed = {};
        if (GetCapture() == popup) ReleaseCapture();
        POINT point = ScreenPoint(lParam);
        ClientToScreen(popup, &point);
        RouteSessionMouseRelease(true, PackPoint(point));
        if (scrolling) {
            InvalidateRect(popup, nullptr, FALSE);
        } else if (pressed.kind != CommandKind::None && pressed == released) {
            Queue(pressed);
            CloseMenu();
        } else if (released.kind == CommandKind::None) {
            CloseMenu();
        }
        return 0;
    }
    case WM_RBUTTONUP: {
        POINT point = ScreenPoint(lParam);
        ClientToScreen(popup, &point);
        RouteSessionMouseRelease(false, PackPoint(point));
        CloseMenu();
        return 0;
    }
    case WM_CAPTURECHANGED:
        if (gMenuPressed.kind != CommandKind::None || gScrolling) {
            gConsumedMouseUps |= 1u;
            CloseMenu();
        }
        return 0;
    case WM_MOUSEWHEEL:
        gWheelRemainder += static_cast<SHORT>(HIWORD(wParam));
        if (const int steps = gWheelRemainder / WHEEL_DELTA; steps != 0) {
            gWheelRemainder %= WHEEL_DELTA;
            Scroll(popup, -steps * 3);
        }
        return 0;
    case WM_NCDESTROY:
        if (gPopup == popup) gPopup = nullptr;
        break;
    default: break;
    }
    return DefWindowProcW(popup, message, wParam, lParam);
}

static void OpenMenu(HWND window)
{
    if (gPopup != nullptr) {
        CloseMenu();
        return;
    }
    if (!gReady) return;
    WNDCLASSW type {};
    type.lpfnWndProc = MenuProcedure;
    type.hInstance = reinterpret_cast<HINSTANCE>(gInstance);
    type.hCursor = LoadCursorA(nullptr, IDC_ARROW);
    type.lpszClassName = kPopupClass;
    WNDCLASSW existing {};
    if (!GetClassInfoW(type.hInstance, kPopupClass, &existing) && RegisterClassW(&type) == 0) {
        ShowWideMessage(L"无法建立会话菜单。", L"确认", MB_OK | MB_ICONERROR);
        return;
    }
    const FrameLayout layout = Layout(window);
    MONITORINFO monitor {};
    monitor.cbSize = sizeof(monitor);
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor)) return;
    const LONG width = std::min<LONG>(Scale(320, layout.dpi), monitor.rcWork.right - monitor.rcWork.left);
    const int maximumHeight = monitor.rcWork.bottom - monitor.rcWork.top - Scale(8, layout.dpi);
    gVisibleRows = std::min(gEntries.size(),
        static_cast<std::size_t>(std::max(1, (maximumHeight - AddHeight() - 3) / RowHeight())));
    const int height = AddHeight() + 3 + static_cast<int>(gVisibleRows) * RowHeight();
    const LONG x = std::clamp(layout.window.left + layout.sessions.right - width,
        monitor.rcWork.left, monitor.rcWork.right - width);
    LONG y = layout.window.top + layout.title.bottom;
    if (y + height > monitor.rcWork.bottom) y = std::max(monitor.rcWork.top, layout.window.top - height);
    for (std::size_t i = 0; i < gEntries.size(); ++i) {
        if (gEntries[i].selected) {
            gMenuHover = {CommandKind::Select, gEntries[i].id};
            if (i < gFirstRow) gFirstRow = i;
            else if (i >= gFirstRow + gVisibleRows) gFirstRow = i - gVisibleRows + 1;
        }
    }
    gPopup = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kPopupClass, L"",
        WS_POPUP | WS_BORDER, x, y, width, height, window, nullptr, type.hInstance, nullptr);
    if (gPopup == nullptr) {
        ShowWideMessage(L"无法打开会话菜单。", L"确认", MB_OK | MB_ICONERROR);
        return;
    }
    UpdateScroll(gPopup);
    SetSystemCursorVisible(true);
    ShowWindow(gPopup, SW_SHOWNOACTIVATE);
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

bool CanTranslateSessionWindowKey(const UINT key) noexcept
{
    return IsSessionWindowInputAvailable() && !session_window_detail::gMenuKeys[key];
}

bool IsSessionWindowInputAvailable() noexcept
{
    using namespace session_window_detail;
    return gPopup == nullptr && gPressed == Button::None && !gDragging;
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
        const Button button = HitButton(layout, point);
        if (button == Button::Close) result = HTCLOSE;
        else if (button == Button::Minimize) result = HTMINBUTTON;
        else if (button == Button::Sessions) result = HTMENU;
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
        CloseMenu();
        RECT rectangle {0, 0, gWindowWidth, gWindowHeight};
        AdjustSessionWindowRect(rectangle, LOWORD(wParam));
        const auto& suggested = *reinterpret_cast<const RECT*>(lParam);
        SetWindowPos(window, nullptr, suggested.left, suggested.top,
            rectangle.right - rectangle.left, rectangle.bottom - rectangle.top,
            SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
        PaintFrame(window);
        return true;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) CloseMenu();
        return false;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0u) == SC_MAXIMIZE || (wParam & 0xFFF0u) == SC_SIZE) return true;
        CloseMenu();
        return false;
    case WM_NCMOUSEMOVE: {
        const FrameLayout layout = Layout(window);
        POINT point = ScreenPoint(lParam);
        point.x -= layout.window.left;
        point.y -= layout.window.top;
        const Button hovered = HitButton(layout, point);
        if (hovered != gHovered) {
            gHovered = hovered;
            PaintFrame(window);
        }
        TRACKMOUSEEVENT track {sizeof(track), TME_LEAVE | TME_NONCLIENT, window, 0};
        TrackMouseEvent(&track);
        return false;
    }
    case WM_NCMOUSELEAVE:
        gHovered = Button::None;
        PaintFrame(window);
        return true;
    case WM_NCLBUTTONDBLCLK:
        if (wParam == HTCAPTION) {
            gConsumedMouseUps |= 1u;
            return true;
        }
        [[fallthrough]];
    case WM_NCLBUTTONDOWN: {
        if (gPopup != nullptr) {
            CloseMenu();
            gConsumedMouseUps |= 1u;
            return true;
        }
        const FrameLayout layout = Layout(window);
        POINT point = ScreenPoint(lParam);
        gDragStart = point;
        point.x -= layout.window.left;
        point.y -= layout.window.top;
        gPressed = HitButton(layout, point);
        if (gPressed != Button::None || wParam == HTCAPTION) {
            gDragging = gPressed == Button::None;
            gDragWindow = layout.window;
            SetCapture(window);
            SetSystemCursorVisible(true);
            PaintFrame(window);
            return true;
        }
        return false;
    }
    case WM_MOUSEMOVE:
        if (gDragging || gPressed != Button::None) {
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
        return gPopup != nullptr;
    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
        if (gDragging || gPressed != Button::None || (gConsumedMouseUps & 1u) != 0) {
            const Button pressed = gPressed;
            const bool dragging = gDragging;
            gPressed = Button::None;
            gDragging = false;
            gConsumedMouseUps &= ~1u;
            if (GetCapture() == window) ReleaseCapture();
            POINT point = ScreenPoint(lParam);
            if (message == WM_LBUTTONUP) ClientToScreen(window, &point);
            RouteSessionMouseRelease(true, PackPoint(point));
            const FrameLayout layout = Layout(window);
            point.x -= layout.window.left;
            point.y -= layout.window.top;
            if (!dragging && pressed != Button::None && pressed == HitButton(layout, point)) {
                if (pressed == Button::Sessions) OpenMenu(window);
                else PostMessageW(window, WM_SYSCOMMAND, pressed == Button::Close ? SC_CLOSE : SC_MINIMIZE, 0);
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
        if (gPopup != nullptr) {
            CloseMenu();
            gConsumedMouseUps |= button;
            return true;
        }
        gConsumedMouseUps &= ~button;
        return false;
    }
    case WM_CAPTURECHANGED:
        if (gPressed != Button::None || gDragging) gConsumedMouseUps |= 1u;
        gPressed = Button::None;
        gDragging = false;
        PaintFrame(window);
        return false;
    case WM_SETCURSOR:
        if (gPopup != nullptr || gDragging || gPressed != Button::None) {
            SetSystemCursorVisible(true);
            SetCursor(LoadCursorA(nullptr, IDC_ARROW));
            result = TRUE;
            return true;
        }
        return false;
    case WM_MOUSEWHEEL:
        if (gPopup != nullptr) {
            SendMessageW(gPopup, message, wParam, lParam);
            return true;
        }
        return false;
    case WM_SYSKEYDOWN:
        if (wParam != VK_F10) return false;
        [[fallthrough]];
    case WM_KEYDOWN:
        if (gMenuKeys[wParam]) {
            if (gPopup != nullptr) MenuKey(static_cast<UINT>(wParam));
            return true;
        }
        if (!IsSessionWindowInputAvailable()) {
            gMenuKeys[wParam] = true;
            if (gPopup != nullptr) MenuKey(static_cast<UINT>(wParam));
            return true;
        }
        return false;
    case WM_SYSKEYUP:
        if (wParam != VK_F10) return false;
        [[fallthrough]];
    case WM_KEYUP:
        if (gMenuKeys[wParam]) {
            gMenuKeys[wParam] = false;
            RouteSessionKeyRelease(static_cast<UINT>(wParam), false);
            return true;
        }
        return false;
    case WM_CHAR:
    case WM_PASTE:
        return !IsSessionWindowInputAvailable();
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
    return true;
}

void RefreshSessionWindow()
{
    using namespace session_window_detail;
    gReady = true;
    auto entries = session::GetClientHost().MenuEntries();
    const bool changed = entries.size() != gEntries.size()
        || !std::equal(entries.begin(), entries.end(), gEntries.begin(), [](const auto& a, const auto& b) {
            return a.id == b.id && a.label == b.label && a.selected == b.selected && a.disconnected == b.disconnected;
        });
    std::wstring title = L"CG2D RE";
    for (const auto& entry : entries) {
        if (entry.selected) {
            title += L" - ";
            title += entry.label;
            if (entry.disconnected) title += L" [断线]";
            title += L" // 开启者，你今天爆揍一顿小吸兽了吗？";
            break;
        }
    }
    const HWND window = reinterpret_cast<HWND>(gLegacyWindow);
    if (title != gTitle) {
        gTitle = std::move(title);
        SetWindowTextW(window, gTitle.c_str());
    }
    if (changed) {
        gEntries = std::move(entries);
        PaintFrame(window);
        if (gPopup != nullptr) UpdateScroll(gPopup);
    }
    UpdateSystemCursorLocation(IsPointerInGameClient());
}

void ShutdownSessionWindow() noexcept
{
    using namespace session_window_detail;
    gReady = false;
    CloseMenu();
    if (gDragging || gPressed != Button::None) {
        gDragging = false;
        gPressed = Button::None;
        if (GetCapture() == reinterpret_cast<HWND>(gLegacyWindow)) ReleaseCapture();
    }
    gCommands.clear();
    gEntries.clear();
    gMenuKeys.fill(false);
    gConsumedMouseUps = 0;
    if (gFont != nullptr) {
        DeleteObject(gFont);
        gFont = nullptr;
        gFontDpi = 0;
    }
    SetSystemCursorVisible(true);
}

} // namespace cg::platform
