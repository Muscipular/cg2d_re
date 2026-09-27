#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::render {

struct TextureSurface;
struct PaletteCacheItem;

inline constexpr std::size_t kTextureNodeCapacity = 4096;

// 原 0x1C 节点的 x64 字段承载；surface 和 next 仅为非拥有引用。
struct TextureNode {
    TextureSurface* surface;       // 原 +00：底层 surface 引用，reset 不写。
    std::int32_t imageNo;          // 原 +04：-1 表示解绑。
    std::uint32_t reuseState;      // 原 +08：复用时序，不是上传成功标志。
    std::int16_t blockX;           // 原 +0C：图内块坐标。
    std::int16_t blockY;           // 原 +0E。
    TextureNode* next;             // 原 +10：同图链的下一节点。
    std::int16_t paletteChoice;    // 原 +14：有符号比较用快照。
    std::int16_t paletteVersion;   // 原 +16：有符号比较用快照。
    std::int16_t paletteIndex;     // 原 +18：原 W16 位模式。
    std::uint8_t unknownTail[2];   // 原 +1A/+1B：意义与运行写者未证。
};

static_assert(std::is_standard_layout_v<TextureNode>);
static_assert(std::is_trivially_copyable_v<TextureNode>);
static_assert(sizeof(TextureNode) == 0x28);
static_assert(offsetof(TextureNode, surface) == 0x00);
static_assert(offsetof(TextureNode, imageNo) == 0x08);
static_assert(offsetof(TextureNode, reuseState) == 0x0C);
static_assert(offsetof(TextureNode, blockX) == 0x10);
static_assert(offsetof(TextureNode, blockY) == 0x12);
static_assert(offsetof(TextureNode, next) == 0x18);
static_assert(offsetof(TextureNode, paletteChoice) == 0x20);
static_assert(offsetof(TextureNode, paletteVersion) == 0x22);
static_assert(offsetof(TextureNode, paletteIndex) == 0x24);
static_assert(offsetof(TextureNode, unknownTail) == 0x26);

// 原 0x10 字节 DX 条目的 x64 承载；引用均非拥有，底层 SDL 资源另行管理。
struct DxGraphicEntry {
    TextureNode* head;          // 原 +00 的非拥有节点引用。
    std::uint16_t width;       // 原 +04 的 W16 位模式。
    std::uint16_t height;      // 原 +06 的 W16 位模式。
    PaletteCacheItem* palette; // 原 +08 的非拥有调色板引用。
    std::uint8_t value0C;     // 原 +0C，意义未证。
    std::uint8_t value0D;     // 原 +0D，意义未证。
    std::uint8_t flag;        // 原 +0E。
    std::uint8_t value0F;     // 原 +0F，意义未证。
};

static_assert(std::is_standard_layout_v<DxGraphicEntry>);
static_assert(std::is_trivially_copyable_v<DxGraphicEntry>);
static_assert(sizeof(DxGraphicEntry) == 0x20);
static_assert(offsetof(DxGraphicEntry, head) == 0);
static_assert(offsetof(DxGraphicEntry, width) == 0x08);
static_assert(offsetof(DxGraphicEntry, height) == 0x0A);
static_assert(offsetof(DxGraphicEntry, palette) == 0x10);
static_assert(offsetof(DxGraphicEntry, value0C) == 0x18);

// 原 103CFA4；由 GraphicAlloc 发布，失败或释放后不由该入口清槽。
extern DxGraphicEntry* gGraphicDxCache103CFA4;

// 原 [1020F08,103CF08) 的节点池与 103CF24/103CF20 两个独立 W32 槽。
extern std::array<TextureNode, kTextureNodeCapacity> gTextureNodes;
extern std::int32_t gTextureNodeCount;
extern std::int32_t gTextureScanIndex;
extern std::int32_t gTexturePreferredCount; // 原 103CF9C。
extern std::int32_t gTextureFallbackCount;  // 原 103CF98。
extern std::int32_t gTextureSizeW;          // 原 103CF2C，BuildSurfaceCache 取低 S16。
extern std::int32_t gTextureSizeH;          // 原 103CF28，BuildSurfaceCache 取低 S16。

// 原 523310：清扫描下标，按实际节点数仅解绑图号、复用状态和 next。
void ResetTextureNodes_523310() noexcept;

// 原 523350：按动态容量失效 DX 表的 head/palette；返回原 16B 步长偏移。
std::int32_t InvalidateGraphicRefs_523350() noexcept;

// 原 5231A0：构建共享 AutoMap/Temp 与 4096 个节点面，失败保留已发布前缀。
std::int32_t BuildSurfaceCache_5231A0();

// 原 Destroy@44C408–44C435：按动态实际 count 释放并清非空节点面。
void ReleaseTextureNodeSurfaces_44C408() noexcept;

} // namespace cg::render
