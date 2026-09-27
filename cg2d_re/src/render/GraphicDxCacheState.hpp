#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

struct SDL_Surface;

namespace cg::render {

struct TextureSurface;
struct PaletteCacheItem;
struct GraphicCacheItem32;

inline constexpr std::size_t kTextureNodeCapacity = 4096;

// 原 44C6D0：请求取低 S16；按 3D 接口/能力调整实际尺寸，再创建 CPU 面。
// 非拥有返回值由现有 TextureSurfaceOwner 管理；CPU 后端不区分 memory caps。
TextureSurface* CreateTextureSurface_44C6D0(
    std::int16_t width, std::int16_t height, std::uint32_t colorKey,
    std::uint32_t unusedMemoryCaps);

// 原 44C8B0：两种接口状态都保持请求尺寸，不经过纹理 +2/能力位调整。
TextureSurface* CreatePlainTextureSurface_44C8B0(
    std::int16_t width, std::int16_t height, std::uint32_t colorKey,
    std::uint32_t unusedMemoryCaps);
// render 内部引用；Retain 返回同一地址，Release 在最后引用消失时释放登记项。
// 创建引用由既有缓存生命周期消费；SDL getter 仅借用，不增加引用。
TextureSurface* RetainTextureSurface(TextureSurface* surface) noexcept;
void ReleaseTextureSurface(TextureSurface* surface) noexcept;
SDL_Surface* GetTextureSdlSurface(TextureSurface* surface) noexcept;

// 设备绑定的一份独立持有；缓存中的原生 T* 仍不拥有资源。
class TextureSurfaceBinding final {
public:
    TextureSurfaceBinding() noexcept = default;
    ~TextureSurfaceBinding();
    TextureSurfaceBinding(const TextureSurfaceBinding&) = delete;
    TextureSurfaceBinding& operator=(const TextureSurfaceBinding&) = delete;
    TextureSurfaceBinding(TextureSurfaceBinding&&) = delete;
    TextureSurfaceBinding& operator=(TextureSurfaceBinding&&) = delete;

    // 先保留新面再释放旧面，允许同面重绑；nullptr 表示解绑定。
    void Bind(TextureSurface* surface) noexcept;
    // 同步借用；需要独立持有的查询调用 RetainTextureSurface(Get())。
    TextureSurface* Get() const noexcept { return surface_; }

private:
    TextureSurface* surface_ {};
};

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

// 原 5222D0：恒空参数路径，按共享宽高领取/解绑/链接节点，不更新 reuseState。
void TakeGraphicDxCache_5222D0(std::int32_t imageNo, void* /*unusedSource*/);

// 原 520B10：版本/限流检查后上传真实索引像素；读图失败返回 0，其余路径返回 1。
std::int32_t LoadGraphicTexture_520B10(const GraphicCacheItem32* graphic, TextureNode* texture);

// 原 451310：将整张节点面填为零；唯一已见 caller 弃 DX7 返回值。
void FillTextureBlack_451310(TextureSurface* surface) noexcept;

// 原 450400 的 32 位写入段：source 指向共享索引像素区，palette 为空时用 AutoMap。
// x/y 按原源行计算，源行倒退、目标行按节点面宽度前进；调用者提供有效像素范围。
void LoadTextureFromBuffer32_450400(
    TextureSurface* surface, std::int32_t x, std::int32_t y,
    std::int32_t width, std::int32_t height,
    const std::uint8_t* source, std::int32_t sourceWidth,
    const std::uint8_t* palette) noexcept;

// 原 Destroy@44C408–44C435：按动态实际 count 释放并清非空节点面。
void ReleaseTextureNodeSurfaces_44C408() noexcept;

// 原 44CD80：按完整 surface 地址、图号、S16 调色板和键矩形复用/生成变换面。
// 两种矩形均为四项 S32；原函数直接把 sourceRectangle 的 right/bottom 当循环宽高。
TextureSurface* TransformTextureSurface_44CD80(
    TextureSurface* source, std::int32_t imageNo, std::int16_t paletteIndex,
    std::uint8_t flipX, std::uint8_t flipY,
    const std::int32_t* sourceRectangle, const std::int32_t* keyRectangle);

} // namespace cg::render
