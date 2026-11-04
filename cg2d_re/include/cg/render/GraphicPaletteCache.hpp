#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::render {

inline constexpr std::size_t kGraphicPaletteDynamicCount = 104;
inline constexpr std::size_t kGraphicPaletteStaticCount = 4;

// 原 0x410 字节缓存项；+0x10 起的像素/调色板载荷保持原字节布局。
struct PaletteCacheItem {
    std::int32_t imageNo;          // +0：动态项的图号。
    std::int32_t activePalIndex;   // +4：静态项的调色板索引。
    std::uint32_t frameIndex;     // +8：帧版本快照。
    std::int32_t dirtyCount;      // +0x0C：动态项老化计数。
    std::array<std::uint8_t, 0x400> payload; // +0x10。
};

static_assert(sizeof(PaletteCacheItem) == 0x410);
static_assert(offsetof(PaletteCacheItem, activePalIndex) == 4);
static_assert(offsetof(PaletteCacheItem, frameIndex) == 8);
static_assert(offsetof(PaletteCacheItem, dirtyCount) == 0x0C);
static_assert(offsetof(PaletteCacheItem, payload) == 0x10);

extern PaletteCacheItem* gGraphicDataCacheList; // 原 0x62B068，动态池引用。
extern std::array<PaletteCacheItem, kGraphicPaletteStaticCount> gGraphicDataCacheStatic;

// 原 44E0C0：分配零填充动态池，分别重置动态 +0 与静态 +4。
void InitializeGraphicPaletteCache_44E0C0();
// 原 44E110：释放动态池并清共享指针；唯一 caller 弃返回值。
void ReleaseGraphicPaletteCache_44E110() noexcept;
// NSM01：只失效着色版本，不释放表面/色盘或改动借用地址。
void InvalidatePaletteColorCaches() noexcept;
// 原 44E150：负图号空槽→正脏计数回收→首槽强制复用，返回同源项。
PaletteCacheItem* TakeIndexCacheFromPool_44E150(std::int32_t imageNo) noexcept;
// 原 44E230：帧末扫描动态池并按脏计数失效 DX 调色板引用。
void AgeGraphicPaletteCache_44E230() noexcept;
// 原 44E290 的 32 位分支；动态 triple/quad 源分别为 BGR/BGRX，非空源须提供 256 项。
PaletteCacheItem* LoadGraphicPalette32_44E290(
    std::int32_t imageNo, std::uint8_t mode, std::uint8_t applyTone,
    const std::uint8_t* paletteTriples, const std::uint8_t* paletteQuads) noexcept;

} // namespace cg::render
