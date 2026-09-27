#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::render {

struct TextureNode;
struct PaletteCacheItem;

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

} // namespace cg::render
