#pragma once
#include <string>

#include "cg/platform/Startup.hpp"

#include <cstdint>

namespace cg::platform {

// 原 0x58ADA8 起的 128 字节可写快照；newest: 写入，后续用途不在本项展开。
extern std::wstring gNewestText;

// 原 [0x55DA00,0x55DA80) 的可写路径快照；初值 map，区间内未见其他引用。
extern std::wstring gMapPath;

// 原 [0xFF4E30,0xFF4EB8)；实际为待提交图形存储的 0x88 字节前缀。
// IDA 的 char[144] 旧类型会越过 0xFF4EB8 队列边界，不能作为容量证据。

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
// 原 0xFF0D40，本函数写入 1；world::gCurrentSceneType 引用同一份存储，枚举语义未决。
extern std::int32_t gCurrentSceneTypeStorage_FF0D40;

// 原 [0x62A640,0x62A644)：首字节为模式，后三字节为 IDA 对齐区。
// 3Ddevice: 的 %d 写满四字节，默认值及 Setup.ini 覆盖只改首字节。
// 该整型仅承载这四字节，不表示原模式变量是 int；小端 x64 上保留原别名。
extern std::int32_t gRendererModeStorage; // 原 [0x62A640,0x62A644)，由本项定义。
extern std::uint8_t& gRendererMode;       // 原 0x62A640，与 MainLoop.hpp 的引用声明一致。

// 原 sub_507C80：一个只读命令文本参数，原 x86 为 cdecl；调用方清理 4 字节。
// 会清理并解析 IP: 表及修改相关计数；返回 0 或越界退出时的 sscanf 返回值。
extern int ParseIpOptions_507C80(const wchar_t* commandLine);

// A08：选人入口在状态11且角色存在时调用；仅目标槽成功消费一次标记。
bool ConsumeAutoLoginCharacterSlot(std::uint32_t slot) noexcept;

// ParseCommandOptions 的唯一函数声明复用 Startup.hpp，返回类型为 int。

} // namespace cg::platform
