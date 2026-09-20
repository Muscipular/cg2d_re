#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::render {

// 原 KRECT/GraphicMode 的 16 字节逐字节描述；字段解释由具体绘制入口负责。
struct GraphicDescriptor16 {
    std::array<std::uint8_t, 16> bytes;
};

static_assert(sizeof(GraphicDescriptor16) == 0x10);

// 原 [0xFF4EB8,0x101CEB8) 的 4096 项待提交图形记录；字段名只表达已确认宽度。
struct GraphicCacheItem32 {
    std::int32_t x;
    std::int32_t y;
    std::int32_t imageNo;
    std::uint32_t dataBits;
    std::int32_t value4;
    std::array<std::uint8_t, 16> descriptor;
    std::uint8_t batchType;
    std::array<std::uint8_t, 3> unwrittenTail;
};

static_assert(sizeof(GraphicCacheItem32) == 0x28);
static_assert(offsetof(GraphicCacheItem32, x) == 0x00);
static_assert(offsetof(GraphicCacheItem32, y) == 0x04);
static_assert(offsetof(GraphicCacheItem32, imageNo) == 0x08);
static_assert(offsetof(GraphicCacheItem32, dataBits) == 0x0C);
static_assert(offsetof(GraphicCacheItem32, value4) == 0x10);
static_assert(offsetof(GraphicCacheItem32, descriptor) == 0x14);
static_assert(offsetof(GraphicCacheItem32, batchType) == 0x24);
static_assert(offsetof(GraphicCacheItem32, unwrittenTail) == 0x25);

// 原 [0x101CEB8,0x1020EB8) 的 4096 项四字节并行记录。
struct GraphicCache3 {
    std::int16_t cacheIndex;
    std::uint8_t layer;
    std::uint8_t unwrittenByte;
};

static_assert(sizeof(GraphicCache3) == 0x04);
static_assert(offsetof(GraphicCache3, cacheIndex) == 0x00);
static_assert(offsetof(GraphicCache3, layer) == 0x02);
static_assert(offsetof(GraphicCache3, unwrittenByte) == 0x03);

// 原 [0xFF4E30,0x1020EB8)；0x1020EB8 的 W16 计数复用 platform 既有存储。
struct LegacyPendingStorage {
    char prefix[0x88];
    std::array<GraphicCacheItem32, 4096> records;
    std::array<GraphicCache3, 4096> cache3;
};

static_assert(offsetof(LegacyPendingStorage, prefix) == 0x00000);
static_assert(offsetof(LegacyPendingStorage, records) == 0x00088);
static_assert(offsetof(LegacyPendingStorage, cache3) == 0x28088);
static_assert(sizeof(LegacyPendingStorage) == 0x2C088);

// 上述 legacy 区间的唯一零初始化承载；不得另建文本区或队列副本。
extern LegacyPendingStorage gLegacyPendingStorage;

// 原 0x1020EDC：特殊图形进入待提交队列后的单字节标志。
extern std::uint8_t gSpecialGraphicPending;

// 原 gGraphicDirty@0x1020EE8；窗口消息仅在值严格等于 1 时消费并重建渲染器。
extern std::int32_t gGraphicDirty;

// 原 Renderer::DrawGraphic3@[0x520970,0x520ACE)。
std::int32_t DrawGraphic3_520970(
    std::int32_t x,
    std::int32_t y,
    std::uint8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    std::int32_t isGraphicNo,
    std::uint8_t batchType,
    const GraphicDescriptor16* descriptor);

// 原 sub_45F090@[0x45F090,0x45F0FA)：固定混合模式的 legacy 图形提交包装。
std::int32_t Sub_45F090(
    std::int32_t x,
    std::int32_t y,
    std::uint8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    std::int32_t color);

} // namespace cg::render
