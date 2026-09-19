#pragma once

#include "cg/platform/Startup.hpp"

#include <cstdint>

namespace cg::platform {

// 原 0x58ADA8 起的 128 字节可写快照；newest: 写入，后续用途不在本项展开。
extern char gNewestText[128];

// 原 [0x55DA00,0x55DA80) 的可写路径快照；初值 map，区间内未见其他引用。
extern char gMapPath[128];

// 原 0xFF4E30；IDA item 为 128 字节、类型标注为 char[144]，完整布局未决。
// 本项只声明原写入目标，不分配猜测容量；解析时写入 data\0\save.dat。
extern char gTextFF4E30[];

extern std::int32_t gDeviceBitsPerPixel; // 原 0x554B70，静态值 8；保存设备查询结果。
extern std::int32_t gNoDelay;            // 原 0x55CE68，静态值 1；nodelay 将其清零。
extern std::uint32_t gRendererFlags;    // 原 0x9A6EC0，32 位标志；onlyactive 设置位 0x10。
extern std::int32_t gEnableSound;       // 原 0x58D3B8，静态值 1；soundoff 将其清零。
extern std::int32_t gEnableMusic;       // 原 0x58D3BC，静态值 1；musicoff 将其清零。
extern std::int32_t gValueFF0D40;        // 原 0xFF0D40，本函数写入 1；状态枚举语义未决。

// 原 [0x62A640,0x62A644)：首字节为模式，后三字节为 IDA 对齐区。
// 3Ddevice: 的 %d 写满四字节，默认值及 Setup.ini 覆盖只改首字节。
// 该整型仅承载这四字节，不表示原模式变量是 int；小端 x64 上保留原别名。
extern std::int32_t gRendererModeStorage; // 原 [0x62A640,0x62A644)，由本项定义。
extern std::uint8_t& gRendererMode;       // 原 0x62A640，与 MainLoop.hpp 的引用声明一致。

// 原 sub_507C80：一个只读命令文本参数，原 x86 为 cdecl；调用方清理 4 字节。
// 会清理并解析 IP: 表及修改相关计数；返回 0 或越界退出时的 sscanf 返回值。
// 仅声明真实直接依赖，不在本项实现其内部。
extern int ParseIpOptions_507C80(const char* commandLine);

// ParseCommandOptions 的唯一函数声明复用 Startup.hpp，返回类型为 int。

} // namespace cg::platform
