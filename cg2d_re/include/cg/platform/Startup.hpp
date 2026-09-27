#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

extern char* gCommandLine;                // 原 0x9A6EC4；原生指针。
extern char gCommandLineStorage[4096];    // 原 0x9A6F48，ReadSetupIni 的持久化输出。
extern char gDataPath[128];               // 原 0x55CF84，静态初值 data。
extern std::int32_t gMonitorKeyboard;     // 原 0x9A6EC8。
extern std::uint8_t gRequestedRenderMode; // 原 0x628FE0。
// 原 0xFF4E28，W8；PE virtual-zero 初值；loader 入口清零，未来 InitializeClient 读取。
extern std::uint8_t gValueFF4E28;

extern std::uintptr_t gInstance;       // 原 0x9A6F38，HINSTANCE 的原生承载。
extern std::uintptr_t gStartupMutex;   // 原 0x9A6F3C。
extern std::uintptr_t gCursorHandles[11]; // 原 0x9A6EF8 起，11 个原生光标引用。
extern std::int32_t gShowCommand;      // 原 0x9A6F28。
extern char gWindowClassName[9];       // 原 0x559384，保持原 Message/Class API 字节。
extern std::int32_t gWindowMode;       // 原 0x9A7F48。
extern std::int32_t gWindowWidth;      // 原 0x55CE74。
extern std::int32_t gWindowHeight;     // 原 0x55CE78。
extern std::int32_t gWindowActiveState; // 原 0x55CE70，初值 2。
extern std::int32_t gStartX;           // 原 0x9A7F50。
extern std::int32_t gStartY;           // 原 0x9A7F54。
extern std::uint32_t gDesktopColor;    // 原 0x9A6F40。
extern std::uint32_t gValueD67AE4;     // 原 bShow；0 走表驱动光标，非零走原生光标分支。
extern std::int32_t gValue73ECBC;      // 原 0x73ECBC，W32，virtual-zero 初值。
extern std::uint32_t gValueD67AE8;     // 原 0xD67AE8，W32，virtual-zero 初值。
extern std::int16_t gValue5F974C;      // 原 0x5F974C，W16，virtual-zero 初值。
extern std::uint32_t gValueD67CD0;     // 原 0xD67CD0，W32；保存记录 +0x313 的 live 状态。
// 原 0x553438，signed W32；sub_520730 按当前记录 +0x315 条件恢复，映像初值 10。
extern std::int32_t gValue553438;
// 原 0x55463C/40/44：显示、聊天与存档恢复共用的三个独立 W32 槽。
extern std::int32_t gValue55463C;
extern std::int32_t gValue554640;
extern std::int32_t gValue554644;
extern std::uint32_t gValueD68858;     // 原 0xD68858，W32，virtual-zero 初值。
extern std::uint32_t gValueD68DC4;     // 原逐帧光标状态请求；CursorUpdate 尾部消费后清零。
extern std::uint32_t gValue9A6F44;     // 原 CursorUpdate 尾部保存的本帧已消费光标状态。
extern std::int32_t gVersionMajor;    // 原 0x9A7F58。
extern std::int32_t gVersionMinor;    // 原 0x9A7F5C。
extern std::int32_t gVersionPatch;    // 原 0x9A7F60。

int ReadSetupIni();                       // 原 0x465A80。
int ParseClientVersion();                 // 原 0x4659F0。
std::uint32_t CaptureDesktopColor();      // 原 0x465A20。
int SetWindowedDesktopColor();            // 原 0x465A30。
int CreateSaveDirectories();              // 原 0x532E70；实际运行会改变 CWD/创建目录。
// 原 GetCurrentDirectoryA 边界：保持窄字节、容量及完整 W32 返回，不改变工作目录。
std::uint32_t ReadLegacyCurrentDirectory(std::uint32_t capacity, char* destination);
// 原 GlobalAlloc(GPTR) -> GlobalLock；仅返回借给资源 owner 的原生指针，不增加失败分支。
void* AllocateLockedZeroedGlobalMemory(std::size_t bytes);
// 原 GlobalHandle -> Unlock -> GlobalHandle -> Free；返回值由已见 caller 弃置。
void ReleaseLockedGlobalMemory(void* memory);
// 原 OutputDebugStringA 窄字节边界；保留兼容字节，不猜测编码或改写消息。
void WriteLegacyDebugString(const char* message);
int InitializeWindow();                   // 原 WndInit@0x4669F0；保留末次 UpdateWindow 的 EAX。
int PumpWindowMessages();                 // 原 0x466D50，WM_QUIT 返回 0，否则 1。
int RestoreCurrentSavedState_5201B0();    // 原 0x5201B0；按固定顺序恢复当前保存记录。

// 原入口的真实依赖；当前仅声明，不提供空实现，因而尚不能链接完整客户端。
int InitializeSavedState_533300();        // 原 0x533300，正常返回 1；入口忽略结果。
int ParseCommandOptions();                // 原 0x465F00；返回末次 0x507C80 的 0/sscanf 结果，入口忽略。
int MainLoop();                           // 原 0x457020；两个正常出口的 EAX 均为 0。

} // namespace cg::platform
