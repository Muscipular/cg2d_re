#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::render {

// 原 PALETTEENTRY 的四字节存储，flags 是调色板标志，不是透明度。
struct PaletteEntry {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::uint8_t flags;
};

// 原 0x554B78：45 个物理记录；选择函数仅允许正常非负索引 0..38。
struct PaletteFile {
    std::int16_t threshold;
    char path[128];
};

static_assert(sizeof(PaletteEntry) == 4);
static_assert(offsetof(PaletteEntry, blue) == 2);
static_assert(offsetof(PaletteEntry, flags) == 3);
static_assert(sizeof(PaletteFile) == 130);
static_assert(offsetof(PaletteFile, path) == 2);

extern PaletteFile gPaletteFiles[45];
extern PaletteEntry gPalette[256];       // 原 0x629B20，当前调色板。
extern PaletteEntry gPaletteBackup[256]; // 原 0x629F2C，首次文件读取后的副本。
extern std::uint32_t gPaletteAutoMap[256]; // 原 0x628BC0，0x00RRGGBB 查色表。

// 原 0x5562E8 的三个通道指针，x64 仅扩大实际指针；通道值仍为字节。
struct ColorChannelMaps {
    std::uint8_t* blue;
    std::uint8_t* green;
    std::uint8_t* red;
};
extern ColorChannelMaps gColorChannelMaps;

extern std::int32_t gPaletteInitialized;     // 原 0x628FD8。
extern std::int32_t gActivePalette;          // 原 0x628FD0。
extern std::int32_t gPaletteTransitionSteps; // 原 0x628FD4。
extern std::int32_t gPaletteCapability;      // 原 0x9A7F64；业务含义仍待上游取证。
extern std::int32_t gPaletteChanging;        // 原 0x628FEC。

// 原 InitPalette@0x450470 的共享逻辑及 32 位分支；DX7 原生调色板对象被 CPU 查色表替代。
// 保留固定颜色、672 字节文件消费、首次备份、168 号置黑、通道初始化与错误返回。
int InitPalette32();

// 原 0x450A80；先读取中间 224 色，再复制当前调色板头尾各 16 色。
// index 须在物理记录内，output 须提供 256 项；短读的 EOF 仍按原端写成 0xFF。
int ReadPalette(std::int32_t index, PaletteEntry* output);

// 原 0x450B70；不增加负数索引处理，调用者须保证 index 非负。
// 返回原 EAX：拒绝时为 index，接受时为 steps，而非成功布尔值。
std::int32_t SelectPalette(std::int32_t index, std::int32_t steps) noexcept;

// 原 0x451220；非零索引的纯黑映射为 1，索引 0 最后单独清零；返回 0x400。
int RebuildPalette32() noexcept;

} // namespace cg::render
