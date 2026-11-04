#pragma once
#include <string>

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace cg::network { class CgConnection; }
namespace cg::session { class ClientSession; }

namespace cg::platform {

extern std::wstring gCommandLine;                // 原 0x9A6EC4；原生指针。
extern std::wstring gCommandLineStorage;    // 原 0x9A6F48，ReadSetupIni 的持久化输出。
extern std::wstring gDataPath;               // 原 0x55CF84，静态初值 data。
extern std::int32_t gMonitorKeyboard;     // 原 0x9A6EC8。
extern std::uint8_t gRequestedRenderMode; // 原 0x628FE0。

extern std::uintptr_t gInstance;       // 原 0x9A6F38，HINSTANCE 的原生承载。
extern std::uintptr_t gCursorHandles[11]; // 原 0x9A6EF8 起，11 个原生光标引用。
extern std::int32_t gShowCommand;      // 原 0x9A6F28。
extern wchar_t gWindowClassName[5];     // 原0x559384；直接供W窗口接口借用。
extern std::int32_t gWindowWidth;      // 原 0x55CE74。
extern std::int32_t gWindowHeight;     // 原 0x55CE78。
extern std::int32_t gWindowActiveState; // 原 0x55CE70，初值 2。
extern std::int32_t gStartX;           // 原 0x9A7F50。
extern std::int32_t gStartY;           // 原 0x9A7F54。
extern std::uint32_t gDesktopColor;    // 原 0x9A6F40。
extern std::uint32_t gNativeCursorEnabled_D67AE4;     // 原全局：0x00D67AE4。 原 bShow；0 走表驱动光标，非零走原生光标分支。
extern std::int32_t gChatTextSizeMode_73ECBC;      // 原 0x73ECBC，W32，virtual-zero 初值。
extern std::uint32_t gInvertItemSplitModifier_D67AE8;     // 原 0xD67AE8，W32，virtual-zero 初值。
extern std::int16_t gActorOverheadTextMode_5F974C;      // 原 0x5F974C，W16，virtual-zero 初值。
extern std::uint32_t gContactSortMode_D67CD0;     // 原 0xD67CD0，W32；保存记录 +0x313 的 live 状态。
// 原 0x553438，signed W32；sub_520730 按当前记录 +0x315 条件恢复，映像初值 10。
extern std::int32_t gValue553438;
// 原 0x55463C/40/44：显示、聊天与存档恢复共用的三个独立 W32 槽。
extern std::int32_t gValue55463C;
extern std::int32_t gChatNotificationSoundEnabled_554644; // 原全局：0x00554644。
extern std::uint32_t gValueD68858;     // 原 0xD68858，W32，virtual-zero 初值。
extern std::int32_t gVersionMajor;    // 原 0x9A7F58。
extern std::int32_t gVersionMinor;    // 原 0x9A7F5C。
extern std::int32_t gVersionPatch;    // 原 0x9A7F60。

// 原函数：0x00465A80。
// int ReadSetupIni();                       // 原 0x465A80。
// 原函数：0x004659F0。
int ParseClientVersion();                 // 原 0x4659F0。
// 原函数：0x00465A20。
std::uint32_t CaptureDesktopColor();      // 原 0x465A20。
// 原函数：0x00465A30。
int SetWindowedDesktopColor();            // 原 0x465A30。
// 原函数：0x00532E70。
int CreateSaveDirectories();              // 原 0x532E70；实际运行会改变 CWD/创建目录。
// 原函数：0x004669F0。
int InitializeWindow(session::ClientSession& session); // 原 WndInit@0x4669F0；保留末次 UpdateWindow 的 EAX。
// 原函数：0x00466D50。
int PumpWindowMessages();                 // 原 0x466D50，WM_QUIT 返回 0，否则 1。

// 原入口所用初始化与主循环接口已有本体；完整客户端仍受其他待决依赖及链接验收限制。
// 原函数：0x00465F00。
int ParseCommandOptions(network::CgConnection& connection); // 原 0x465F00；返回末次 0x507C80 的 0/sscanf 结果，入口忽略。
// 原函数：0x00457020。
int MainLoop(session::ClientSession& session); // 原 0x457020；两个正常出口的 EAX 均为 0。

} // namespace cg::platform
