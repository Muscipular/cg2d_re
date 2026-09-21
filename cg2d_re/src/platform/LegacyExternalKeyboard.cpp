#include "../LoginInputDependencies.hpp"
#include "cg/platform/LegacyWindow.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <cstring>

namespace cg::platform {

// 原 dword_1085F5C：与 DialogFunc 共享的实例句柄；原包装层只读，不写回窗口查询值。
HINSTANCE gExternalKeyboardInstance1085F5C = nullptr;
// 原 byte_1085E08：与 DialogFunc 共享的结果；保留至下一对象 0x1085F10 的 0x108 字节。
char gExternalKeyboardResult1085E08[0x108] {};

// 原 DialogFunc@0x534B80：保留真实外部依赖，后续在本实现单元还原其对话框行为。
INT_PTR CALLBACK DialogFunc_534B80(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

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
