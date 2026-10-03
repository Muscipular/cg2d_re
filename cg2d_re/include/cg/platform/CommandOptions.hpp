#pragma once

#include "cg/platform/Startup.hpp"

#include <cstdint>

namespace cg::platform {

// 原 0x58ADA8 起的 128 字节可写快照；newest: 写入，后续用途不在本项展开。
extern char gNewestText[128];

// 原 [0x55DA00,0x55DA80) 的可写路径快照；初值 map，区间内未见其他引用。
extern char gMapPath[128];

// 原 [0xFF4E30,0xFF4EB8)；实际为待提交图形存储的 0x88 字节前缀。
// IDA 的 char[144] 旧类型会越过 0xFF4EB8 队列边界，不能作为容量证据。
extern char (&gTextFF4E30)[0x88];

extern std::int32_t gDeviceBitsPerPixel; // 原 0x554B70，初值 8；采样桌面，Init 后承接实际 CPU Back 位数。
// 原 0x554B74，W16，静态初值 1；0 表示同步，1 表示非同步。
extern std::uint16_t gScanlineSyncMode;
extern std::int32_t gNoDelay;            // 原 0x55CE68，静态值 1；nodelay 将其清零。
extern std::uint32_t gRendererFlags;    // 原 0x9A6EC0，32 位标志；onlyactive 设置位 0x10。
extern std::int32_t gEnableSound;       // 原 0x58D3B8，静态值 1；soundoff 将其清零。
extern std::int32_t gEnableMusic;       // 原 0x58D3BC，静态值 1；musicoff 将其清零。
extern std::int32_t gUseStereo;         // 原 0x58D3C4，W32，静态初值 1。
extern std::int32_t gEffectVolume;      // 原 0x58D3C8，W32，静态初值 15。
extern std::int32_t gMusicVolume;       // 原 0x58D3CC，W32，静态初值 15。
// 原 [1050840,1050850) 持久化前缀；引用唯一连续背板的前 16 字节。
extern std::uint8_t (&gBgmFrequencySelectionPrefix)[16];
// 52BE60 的 W8 读取跨前缀、8 字节间隔及 1050858 的 W32 运行表。
std::int8_t ReadBgmFrequencySelectionByte(std::uint8_t id) noexcept;
// 原 [1050858,1050B78) 的 200 项 W32 可写表，与上述 W8 读取共享存储。
std::uint32_t* BgmRuntimeOrderWords() noexcept;
// 原 0xFF0D40，本函数写入 1；world::gCurrentSceneType 引用同一份存储，枚举语义未决。
extern std::int32_t gValueFF0D40;

// 原 [0x62A640,0x62A644)：首字节为模式，后三字节为 IDA 对齐区。
// 3Ddevice: 的 %d 写满四字节，默认值及 Setup.ini 覆盖只改首字节。
// 该整型仅承载这四字节，不表示原模式变量是 int；小端 x64 上保留原别名。
extern std::int32_t gRendererModeStorage; // 原 [0x62A640,0x62A644)，由本项定义。
extern std::uint8_t& gRendererMode;       // 原 0x62A640，与 MainLoop.hpp 的引用声明一致。

// 原 sub_507C80：一个只读命令文本参数，原 x86 为 cdecl；调用方清理 4 字节。
// 会清理并解析 IP: 表及修改相关计数；返回 0 或越界退出时的 sscanf 返回值。
extern int ParseIpOptions_507C80(const char* commandLine);

// ParseCommandOptions 的唯一函数声明复用 Startup.hpp，返回类型为 int。

// 原1087AE4；借用GetCommandLineA结果，必须先完成原启动的生产片段。
extern char* gLegacyRawCommandLine1087AE4;

// 原53DDB5内53DE29..53DE34，仅获取并发布，不代表完整CRT启动入口。
void CaptureLegacyCommandLine_53DE29();

// 原543F36；初始化多字节表后切片，返回原命令行内的借用指针。
char* SliceLegacyWinMainCommandLine_543F36();

// 原544047；原始命令行必须先生产，分配失败沿原间接退出链。
std::int32_t InitializeLegacyArguments_544047();
extern char* gLegacyProgramName1086018;
extern char** gLegacyArguments1086000;
extern std::int32_t gLegacyArgumentCount1085FFC;

// 原1086030是Malloc拥有的旧字节块；53DE34先发布，543F8E释放后清槽。
extern char* gLegacyRawEnvironment1086030;
// 原1086008是543F8E生产的拥有型char*表，包含末尾空槽；各地址按x64拓宽。
extern char** gLegacyEnvironment1086008;
extern std::uint32_t gLegacyEnvironmentInitialized1087AE8; // 原543F8E末尾写1。
// 两片段保持原启动次序：环境块生产、argv生产、环境表生产、cinit；完整入口仍待。
void CaptureLegacyEnvironment_53DE34();
void InitializeLegacyEnvironment_543F8E();

// 原5440E0；槽/字节区可空以计数，槽数包含末尾nullptr，返回槽数指针。
// 两个计数保持S32；多字节分类由调用方既有初始化提供，不在此重新初始化。
std::int32_t* ParseLegacyCommandLine_5440E0(const char* commandLine, char** arguments,
    char* text, std::int32_t* argumentSlotCount, std::int32_t* byteCount);

} // namespace cg::platform
