#pragma once

#include "cg/render/QueueCapacity.hpp"

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
    std::array<GraphicCacheItem32, MAX_QUEUE_SIZE> records;
    std::array<GraphicCache3, MAX_QUEUE_SIZE> cache3;
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

// 原 0x103CF10：跨初始化、队列绘制及场景调用的独立完整 W32 模式槽。
extern std::uint32_t gRenderQueueMode103CF10;

// 原波纹后缓冲状态；W32/S32 字段以无符号位模式承载回绕语义。
extern volatile std::uint32_t gRippleIntegerPhaseBits; // 原 0x1020EEC。

// 原 flt_1020EF0@0x1020EF0；浮点相位的唯一零初始化存储。
extern float gFloat1020EF0;

extern volatile std::uint32_t gRipplePreviousWorldXBits; // 原 0x1020EF4，F32。
extern volatile std::uint32_t gRipplePreviousWorldYBits; // 原 0x1020EF8，F32。
extern volatile std::uint32_t gRippleStrengthBits; // 原 0x1020EFC。
extern volatile std::int32_t gRippleMapFloorCache; // 原 0x1020F00，S32。

// 原 0x58B6E0 的 64 项有符号正弦近似表。
extern const std::array<std::int32_t, 64> gRippleSineTable;

// 原 sub_5208F0@[0x5208F0,0x520907)：原始 F32 位模式严格按
// R current X -> R current Y -> W previous X -> W previous Y 执行。
std::uint32_t BackupValues_5208F0();

// 原 sub_520910@[0x520910,0x520923)：单次累加原浮点常量 0.25f。
void Sub_520910();

// 原 sub_520930@[0x520930,0x52096B)：原位逐次加减 64.0f 的浮点相位叶。
void Sub_520930(float* value);

// 原 sub_521870@[0x521870,0x5219B5)：共享 CPU Back/Temp 的波纹条带回拷。
std::int32_t Sub_521870();

// 原 Renderer::SortTextures@[0x520790,0x5208E8)；只重排 cache3 索引记录。
void SortTextures_520790();

// 原 sub_522210@[0x522210,0x522293)：从待提交记录前两项构造矩形，
// 按图号高位标志选择真实 451350 绘制模式；不修改记录。
void Sub_522210(const GraphicCacheItem32* graphic, std::uint32_t imageNoBits);

// 原 Renderer::FillDirtyArea@[0x520AD0,0x520B09)：从记录取坐标、
// 颜色与混合模式；仅底层返回原 surface-busy 位型时写 gGraphicDirty。
void FillDirtyArea_520AD0(const GraphicCacheItem32* graphic);

// 原 Graphic::LoadPatelet@523140：借用 +22 的 S16 独立调色板索引，保留 0/1/2 返回。
std::uint8_t LoadGraphicIndependentPalette_523140(GraphicCacheItem32* graphic);

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
