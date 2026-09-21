#include "../LoginInputDependencies.hpp"
#include "LegacyExternalKeyboardDependencies.hpp"
#include "MainLoopDependencies.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyWindow.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <bit>
#include <cstring>

namespace cg::platform {

// 原 dword_1085F5C：与 DialogFunc 共享的实例句柄；原包装层只读，不写回窗口查询值。
HINSTANCE gExternalKeyboardInstance1085F5C = nullptr;
// 原 byte_1085E08：与 DialogFunc 共享的结果；保留至下一对象 0x1085F10 的 0x108 字节。
char gExternalKeyboardResult1085E08[0x108] {};

namespace {

std::int32_t gExternalKeyboardX1085DA8 = 0;            // 原 dword_1085DA8：随机窗口 X。
std::int32_t gExternalKeyboardWidth1085DAC = 0;        // 原 nWidth：对象宽度加窗口边框。
std::int32_t gExternalKeyboardY1085DB0 = 0;            // 原 dword_1085DB0：随机窗口 Y。
RECT gExternalKeyboardClientRect1085DB8 {};           // 原 stru_1085DB8：对话框客户区。
std::int32_t gExternalKeyboardBorderWidth1085DCC = 0;  // 原 dword_1085DCC：左右边框总宽。
std::int32_t gExternalKeyboardHeight1085DD0 = 0;       // 原 nHeight：对象高度加窗口边框。
std::int32_t gExternalKeyboardBorderHeight1085DD4 = 0; // 原 dword_1085DD4：上下边框总高。
RECT gExternalKeyboardWindowRect1085DD8 {};           // 原 Rect：首次读取的对话框矩形。
RECT gExternalKeyboardParentRect1085DE8 {};           // 原 stru_1085DE8：父窗口矩形。
RECT gExternalKeyboardWindowRect1085DF8 {};           // 原 stru_1085DF8：再次读取的对话框矩形。
PAINTSTRUCT gExternalKeyboardPaint1085F10 {};         // 原 Paint：使用原生布局，不固定为 x86 大小。
HDC gExternalKeyboardPaintDc1085F50 = nullptr;        // 原 dword_1085F50：BeginPaint 返回值。
ExternalKeyboard* gExternalKeyboardObject1085F54 = nullptr; // 原 dword_1085F54：唯一对象指针。
HWND gExternalKeyboardParent1085F58 = nullptr;        // 原 dword_1085F58：GetParent 返回值。

// 原 byte_593850 的空串；复制时仅覆盖结果首字节的终止 NUL。
inline constexpr char kExternalKeyboardEmptyText[] = "";

} // namespace

// 原 DialogFunc@[0x534B80,0x534EEA)：WPARAM 未读取，LPARAM 按完整原生宽度比较。
INT_PTR CALLBACK DialogFunc_534B80(
    HWND window, UINT message, WPARAM, LPARAM lParam)
{
    switch (message) {
    case WM_DESTROY:
        if (gExternalKeyboardObject1085F54 != nullptr) {
            static_cast<void>(DeleteExternalKeyboardVirtual_535290(
                gExternalKeyboardObject1085F54, 1));
        }
        return 0;

    case WM_PAINT:
        gExternalKeyboardPaintDc1085F50 = BeginPaint(window, &gExternalKeyboardPaint1085F10);
        static_cast<void>(EndPaint(window, &gExternalKeyboardPaint1085F10));
        return 0;

    case WM_CLOSE:
        std::strcpy(gExternalKeyboardResult1085E08, kExternalKeyboardEmptyText);
        static_cast<void>(EndDialog(window, 200));
        return 0;

    case WM_GETDLGCODE:
        return 4;

    case WM_INITDIALOG: {
        gExternalKeyboardObject1085F54 = CreateExternalKeyboard_535090(
            reinterpret_cast<std::uintptr_t>(gExternalKeyboardInstance1085F5C),
            reinterpret_cast<std::uintptr_t>(window), kExternalKeyboardEmptyText);
        static_cast<void>(GetWindowRect(window, &gExternalKeyboardWindowRect1085DD8));
        static_cast<void>(GetClientRect(window, &gExternalKeyboardClientRect1085DB8));

        // 原 0x534DB2..0x534DF8：先高后宽；每次加减均以 W32 位型回绕。
        gExternalKeyboardBorderHeight1085DD4 = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(gExternalKeyboardClientRect1085DB8.top)
            - static_cast<std::uint32_t>(gExternalKeyboardClientRect1085DB8.bottom)
            - static_cast<std::uint32_t>(gExternalKeyboardWindowRect1085DD8.top)
            + static_cast<std::uint32_t>(gExternalKeyboardWindowRect1085DD8.bottom));
        gExternalKeyboardBorderWidth1085DCC = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(gExternalKeyboardClientRect1085DB8.left)
            - static_cast<std::uint32_t>(gExternalKeyboardClientRect1085DB8.right)
            - static_cast<std::uint32_t>(gExternalKeyboardWindowRect1085DD8.left)
            + static_cast<std::uint32_t>(gExternalKeyboardWindowRect1085DD8.right));

        // 原 0x534E04/0x534E23：两次独立虚调用，分别读取实际返回指针的宽与高。
        ExternalKeyboardSize widthOutput;
        const ExternalKeyboardSize* widthResult = QueryExternalKeyboardSizeVirtual_537DC0(
            gExternalKeyboardObject1085F54, &widthOutput);
        gExternalKeyboardWidth1085DAC = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(widthResult->width)
            + static_cast<std::uint32_t>(gExternalKeyboardBorderWidth1085DCC));
        ExternalKeyboardSize heightOutput;
        const ExternalKeyboardSize* heightResult = QueryExternalKeyboardSizeVirtual_537DC0(
            gExternalKeyboardObject1085F54, &heightOutput);
        gExternalKeyboardHeight1085DD0 = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(heightResult->height)
            + static_cast<std::uint32_t>(gExternalKeyboardBorderHeight1085DD4));

        gExternalKeyboardParent1085F58 = GetParent(window);
        static_cast<void>(GetWindowRect(
            gExternalKeyboardParent1085F58, &gExternalKeyboardParentRect1085DE8));
        static_cast<void>(GetWindowRect(window, &gExternalKeyboardWindowRect1085DF8));
        static_cast<void>(SeedRandom_53B4BF(
            static_cast<std::uint32_t>(ReadLegacyTime32_53BA26(nullptr))));

        // 原两次 idiv 保留 signed W32 余数；不为零分母或空对象增加门禁。
        const std::int32_t randomX = NextRandom_53B4C9();
        const std::int32_t availableWidth = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(gExternalKeyboardParentRect1085DE8.right)
            - static_cast<std::uint32_t>(gExternalKeyboardParentRect1085DE8.left)
            - static_cast<std::uint32_t>(gExternalKeyboardWidth1085DAC));
        const std::int32_t xOffset = randomX % availableWidth;
        gExternalKeyboardX1085DA8 = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(xOffset)
            + static_cast<std::uint32_t>(gExternalKeyboardParentRect1085DE8.left));
        const std::int32_t randomY = NextRandom_53B4C9();
        const std::int32_t availableHeight = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(gExternalKeyboardParentRect1085DE8.bottom)
            - static_cast<std::uint32_t>(gExternalKeyboardParentRect1085DE8.top)
            - static_cast<std::uint32_t>(gExternalKeyboardHeight1085DD0));
        const std::int32_t yOffset = randomY % availableHeight;
        gExternalKeyboardY1085DB0 = std::bit_cast<std::int32_t>(
            static_cast<std::uint32_t>(yOffset)
            + static_cast<std::uint32_t>(gExternalKeyboardParentRect1085DE8.top));
        static_cast<void>(MoveWindow(window, gExternalKeyboardX1085DA8,
            gExternalKeyboardY1085DB0, gExternalKeyboardWidth1085DAC,
            gExternalKeyboardHeight1085DD0, TRUE));
        static_cast<void>(InvalidateRect(window, nullptr, TRUE));
        return 1;
    }

    case WM_USER + 1:
        if (lParam == 1) {
            std::strcpy(gExternalKeyboardResult1085E08,
                GetExternalKeyboardResult_5354C0(gExternalKeyboardObject1085F54));
            static_cast<void>(EndDialog(window, 100));
        } else {
            std::strcpy(gExternalKeyboardResult1085E08, kExternalKeyboardEmptyText);
            static_cast<void>(EndDialog(window, 200));
        }
        return 1;

    case WM_COMMAND:
    default:
        return 0;
    }
}

} // namespace cg::platform

namespace cg::login::detail {
namespace {

// 原 0x58F6F8/0x58F714 的 CP950 字节：“載入 idguardlibr.dll 失敗!”、“無法載入函式庫”。
inline constexpr char kLoadLibraryErrorText[] =
    "\xB8\xFC\xA4\x4A idguardlibr.dll \xA5\xA2\xB1\xD1!";
inline constexpr char kLoadLibraryErrorCaption[] =
    "\xB5\x4C\xAA\x6B\xB8\xFC\xA4\x4A\xA8\xE7\xA6\xA1\xAE\x77";
// 原 0x58F6CC/0x58F6E0 的 CP950 字节：“螢幕小鍵盤啟動失敗!”、“螢幕小鍵盤”。
inline constexpr char kDialogErrorText[] =
    "\xBF\xC3\xB9\xF5\xA4\x70\xC1\xE4\xBD\x4C\xB1\xD2\xB0\xCA\xA5\xA2\xB1\xD1!";
inline constexpr char kDialogErrorCaption[] =
    "\xBF\xC3\xB9\xF5\xA4\x70\xC1\xE4\xBD\x4C";

} // namespace

std::int32_t Sub_534AB0(const std::uintptr_t window, char* output)
{
    const HWND parent = reinterpret_cast<HWND>(window);
    if (platform::gExternalKeyboardInstance1085F5C == nullptr) {
        // 原 0x534AC3 丢弃返回值；x64 使用原生宽度，但仍不更新共享实例句柄。
        static_cast<void>(GetWindowLongPtrA(parent, GWLP_HINSTANCE));
    }

    const HMODULE module = LoadLibraryA("idguardlibr.dll");
    if (module == nullptr) {
        MessageBoxA(parent, kLoadLibraryErrorText, kLoadLibraryErrorCaption, MB_OK);
        return -1;
    }

    static_cast<void>(platform::ShowLegacyCursor(TRUE));
    const INT_PTR dialogResult = DialogBoxParamA(
        module, "KeyboardDlg", parent, platform::DialogFunc_534B80, 0);
    static_cast<void>(platform::ShowLegacyCursor(FALSE));
    FreeLibrary(module);

    if (dialogResult <= 0) {
        MessageBoxA(parent, kDialogErrorText, kDialogErrorCaption, MB_OK);
        return -1;
    }
    if (dialogResult == 100) {
        // 原 0x534B47..0x534B6E：无界复制结果及终止 NUL，不预清或截断 output。
        std::strcpy(output, platform::gExternalKeyboardResult1085E08);
        return 1;
    }
    return 0;
}

} // namespace cg::login::detail
