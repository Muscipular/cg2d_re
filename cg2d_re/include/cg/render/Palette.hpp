#pragma once
#include <string>

#include <cstddef>
#include <cstdint>

namespace cg::session { class ClientSession; }

namespace cg::render {

// 原 PALETTEENTRY 的四字节存储，flags 是调色板标志，不是透明度。
struct PaletteEntry {
    std::uint8_t red; // 原成员偏移：+0x00。
    std::uint8_t green; // 原成员偏移：+0x01。
    std::uint8_t blue; // 原成员偏移：+0x02。
    std::uint8_t flags; // 原成员偏移：+0x03。
};

// 原 0x554B78：45 个物理记录；选择函数仅允许正常非负索引 0..38。
struct PaletteFile {
    std::int16_t threshold; // 原成员偏移：+0x00。
    std::wstring path; // 原成员偏移：+0x02。
};

static_assert(sizeof(PaletteEntry) == 4);
static_assert(offsetof(PaletteEntry, blue) == 2);
static_assert(offsetof(PaletteEntry, flags) == 3);

extern PaletteFile gPaletteFiles[45]; // 原全局：0x00554B78。
extern PaletteEntry gPalette[256];       // 原 0x629B20，当前调色板。
extern PaletteEntry gPaletteBackup[256]; // 原 0x629F2C，首次文件读取后的副本。
extern std::uint32_t gPaletteAutoMap[256]; // 原 0x628BC0，0x00RRGGBB 查色表。

// R09：小地图索引取RGB；缺色回退当前Palette，零索引不取色，纯黑以1避开色键。
std::uint32_t GetAutoMapPaletteRgb(std::uint8_t index) noexcept;

// 原 5562E8/5562EC/5562F0 依次为蓝、绿、红通道指针；x64 仅扩大实际指针。
struct ColorChannelMaps {
    std::uint8_t* blue; // 原全局：0x005562E8。
    std::uint8_t* green; // 原全局：0x005562EC。
    std::uint8_t* red; // 原全局：0x005562F0。
};
static_assert(sizeof(ColorChannelMaps) == 3 * sizeof(std::uint8_t*));
static_assert(offsetof(ColorChannelMaps, blue) == 0);
static_assert(offsetof(ColorChannelMaps, green) == sizeof(std::uint8_t*));
static_assert(offsetof(ColorChannelMaps, red) == 2 * sizeof(std::uint8_t*));
extern ColorChannelMaps gColorChannelMaps; // 原全局：0x005562E8。

// 原 0x5562E0 的 RGB 色调，第四字节静态为零；tColor@0x5562E4 可指向别处。
struct PaletteTone {
    std::uint8_t red; // 原成员偏移：+0x00。
    std::uint8_t green; // 原成员偏移：+0x01。
    std::uint8_t blue; // 原成员偏移：+0x02。
    std::uint8_t reserved; // 原成员偏移：+0x03。
};
static_assert(sizeof(PaletteTone) == 4);
extern PaletteTone gPaletteTone; // 原全局：0x005562E0。
extern PaletteTone* gPaletteTonePointer; // 原全局：0x005562E4。
extern std::uint8_t gPaletteToneTargets[45][3]; // 原 0x556254：每项 R/G/B，步长 3。

extern std::int32_t gPaletteInitialized;     // 原 0x628FD8。
extern std::int32_t gActivePalette;          // 原 0x628FD0。
extern std::int32_t gPaletteTransitionSteps; // 原 0x628FD4。
extern std::int32_t gPaletteCapability;      // PCR01：原9A7F64，五图号两轮探测等级-1/0..4；不猜产品版本。
extern std::int32_t gPaletteChanging;        // 原 0x628FEC。
extern std::int32_t gLoadedPalette;          // 原 0x5562F4，初值 -1。
extern std::int32_t gPaletteTransitionActive; // 原 0x62B0A0，初值 0。
extern std::int32_t gPaletteTransitionFrame; // 原 0x62B09C，初值 0。
extern PaletteEntry gPaletteTarget[256];     // 原 0x626C60，目标色的 flags 不由更新函数写入。
extern float gPaletteBlueCurrent[256];       // 原 0x62A664。
extern float gPaletteGreenCurrent[256];      // 原 0x629720。
extern float gPaletteRedCurrent[256];        // 原 0x629318。
extern float gPaletteBlueStep[256];          // 原 0x6287B4。
extern float gPaletteGreenStep[256];         // 原 0x6283A8。
extern float gPaletteRedStep[256];           // 原 0x62AA64。
extern float gPaletteToneBlueCurrent;        // 原 0x629F28。
extern float gPaletteToneGreenCurrent;       // 原 0x629F24。
extern float gPaletteToneRedCurrent;         // 原 0x629F20。
extern float gPaletteToneBlueStep;           // 原 0x628FF8。
extern float gPaletteToneGreenStep;          // 原 0x628FF4。
extern float gPaletteToneRedStep;            // 原 0x628FF0。
extern std::int32_t gPaletteToneTargetIndex; // 原 0x628BBC。
extern std::int32_t gPaletteFrameVersion;    // 原 0x628FDC。
extern std::int32_t gPaletteVersionThrottle; // 原 0x629310。
extern std::uint64_t gPalettePresentationGeneration; // NSM01：共享色盘输出切换代次，不写磁盘。

// 原 453DE0：统计完整 W32 掩码中的置位数；Renderer::Init 用于颜色通道参数。
int CountMaskBits_453DE0(std::uint32_t mask) noexcept;

// 原 InitPalette@0x450470 的共享逻辑及 32 位分支；调用前须发布 renderer owner。
// 保留固定颜色、672 字节文件消费、首次备份、168 号置黑、调色板绑定与错误返回。
int InitPalette32();

// 原 0x450A80；先读取中间 224 色，再复制当前调色板头尾各 16 色。
// index 须在物理记录内，output 须提供 256 项；短读的 EOF 仍按原端写成 0xFF。
int ReadPalette(std::int32_t index, PaletteEntry* output);

// 原 451E90：以第 0 色为初始值，仅扫描 [16,count-16)，严格更近才更新。
std::int32_t FindNearestPaletteColor_451E90(
    std::uint32_t color, const PaletteEntry* palette, std::int32_t count);

// 原 0x450B70；不增加负数索引处理，调用者须保证 index 非负。
// 返回原 EAX：拒绝时为 index，接受时为 steps，而非成功布尔值。
std::int32_t SelectSessionPalette(
    session::ClientSession& session, std::int32_t index, std::int32_t steps) noexcept; // PCR01：原门槛/返回；仅当前发布共享渲染目标。

// 后台淡变已跳过，不能消费当前会话的共享等待状态。
std::int32_t ReadSessionPaletteChanging(session::ClientSession& session) noexcept;

// 切换只登记失效；恢复在下一帧逻辑前进行，失败不清待重试标志。
void InvalidatePalettePresentation() noexcept;
bool PrepareSessionPalette(session::ClientSession& session);

// 原 0x450BB0：按活动索引加载目标并逐帧过渡；保持原 EAX 各路径值。
// NSM01：切回时直接完成；普通当前会话保持原逐帧算法。
int UpdatePalette_450BB0(bool completeImmediately = false);

// 原 0x451220；非零索引的纯黑映射为 1，索引 0 最后单独清零；返回 0x400。
int RebuildPalette32() noexcept;

} // namespace cg::render
