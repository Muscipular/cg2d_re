#pragma once

#include "cg/render/QueueCapacity.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

namespace cg::yobi {
struct YobiAction;
}

namespace cg::session { class ClientSession; }

namespace cg::render {

struct PngImageBatch; // R04：render私有帧批次；队列只借用。
struct CanvasBatchData; // R08：render拥有的形状快照；队列只借用。

// R03/R04/R08 扩展批次沿参考编号；仍进入同一层序队列。
enum GraphicBatchType : std::uint8_t {
    GraphicBatch_Rectangle = 13,
    GraphicBatch_PngImage = 14,
    GraphicBatch_FullscreenBackground = 15, // R03／LUR01 共用完整源图铺满窗口。
    GraphicBatch_Canvas = 16,
    GraphicBatch_FullscreenBackgroundTrimmed = 17, // 仅按有效 Alpha 范围铺满，去除外围透明行列。
    GraphicBatch_BattleBackground = 18, // 战斗蒙版铺满画布，实际缩放时使用线性采样。
};

// GraphicDescriptor16::mixFlags 的位值；组合以 U8 保存，保留动画原字节中的未知位。
enum GraphicMixFlag : std::uint8_t {
    GraphicMixFlag_None = 0,
    GraphicMixFlag_FlipX = 0x01, // 横向翻转。
    GraphicMixFlag_FlipY = 0x02, // 纵向翻转。
    GraphicMixFlag_OriginalPalette = 0x04, // 使用原始调色板，跳过色调处理。
    GraphicMixFlag_Additive = 0x08, // 加法混合。
    GraphicMixFlag_FlipMask = GraphicMixFlag_FlipX | GraphicMixFlag_FlipY,
    GraphicMixFlag_AllMask = 0xFF, // 全字节覆盖掩码，包含未具名位。
};

// 原 KRECT/GraphicMode 的 16 字节描述，队列、Yobi 与地图共用。
struct GraphicDescriptor16 {
    std::int16_t sourceX; // 原成员偏移：+0x00。 源图横坐标。
    std::int16_t sourceY; // 原成员偏移：+0x02。 源图纵坐标。
    std::int16_t sourceWidth; // 原成员偏移：+0x04。 源图宽度。
    std::int16_t sourceHeight; // 原成员偏移：+0x06。 源图高度。
    std::uint32_t color; // 原成员偏移：+0x08。 原32位颜色，通道写入借本成员的字节视图。
    std::uint8_t clipMode; // 原成员偏移：+0x0C。 裁剪控制，原值1启用。
    std::uint8_t mixFlags; // 原成员偏移：+0x0D。 GraphicMixFlag 位组合。
    std::int16_t paletteIndex; // 原成员偏移：+0x0E。 独立调色板索引。
};

static_assert(sizeof(GraphicDescriptor16) == 0x10);
static_assert(alignof(GraphicDescriptor16) == 0x04);
static_assert(std::is_standard_layout_v<GraphicDescriptor16>);
static_assert(std::is_trivially_copyable_v<GraphicDescriptor16>);
static_assert(offsetof(GraphicDescriptor16, sourceX) == 0x00);
static_assert(offsetof(GraphicDescriptor16, sourceY) == 0x02);
static_assert(offsetof(GraphicDescriptor16, sourceWidth) == 0x04);
static_assert(offsetof(GraphicDescriptor16, sourceHeight) == 0x06);
static_assert(offsetof(GraphicDescriptor16, color) == 0x08);
static_assert(offsetof(GraphicDescriptor16, clipMode) == 0x0C);
static_assert(offsetof(GraphicDescriptor16, mixFlags) == 0x0D);
static_assert(offsetof(GraphicDescriptor16, paletteIndex) == 0x0E);

// 原 +0C 混用字段；按生产/消费合同访问，action 为非拥有引用，文本仍用 W32 token。
union GraphicQueueReference {
    cg::yobi::YobiAction* action;
    PngImageBatch* pngImage; // R04：纹理由帧批次保留，非SDK对象。
    CanvasBatchData* canvas; // R08：原生形状快照，无底层SDK对象。
    std::uint32_t textToken;
};

static_assert(sizeof(cg::yobi::YobiAction*) == 0x08);
static_assert(sizeof(GraphicQueueReference) == 0x08);
static_assert(alignof(GraphicQueueReference) == 0x08);
static_assert(std::is_standard_layout_v<GraphicQueueReference>);
static_assert(std::is_trivially_copyable_v<GraphicQueueReference>);

// 原 [0xFF4EB8,0x101CEB8) 的 4096 项记录；新端按 DX x64 SPEC §15 承载为 48B。
struct GraphicCacheItem32 {
    std::int32_t x; // 原成员偏移：+0x00。
    std::int32_t y; // 原成员偏移：+0x04。
    std::int32_t imageNo; // 原成员偏移：+0x08。
    GraphicQueueReference reference; // 原成员偏移：+0x0C。
    std::int32_t value4; // 原成员偏移：+0x10。
    GraphicDescriptor16 descriptor; // 原成员偏移：+0x14。 与提交参数共用描述符类型。
    std::uint8_t batchType; // 原成员偏移：+0x24。
    std::array<std::uint8_t, 3> unwrittenTail; // 原成员偏移：+0x25。
};

static_assert(sizeof(GraphicCacheItem32) == 0x30);
static_assert(alignof(GraphicCacheItem32) == 0x08);
static_assert(std::is_standard_layout_v<GraphicCacheItem32>);
static_assert(std::is_trivially_copyable_v<GraphicCacheItem32>);
static_assert(offsetof(GraphicCacheItem32, x) == 0x00);
static_assert(offsetof(GraphicCacheItem32, y) == 0x04);
static_assert(offsetof(GraphicCacheItem32, imageNo) == 0x08);
static_assert(offsetof(GraphicCacheItem32, reference) == 0x10);
static_assert(offsetof(GraphicCacheItem32, value4) == 0x18);
static_assert(offsetof(GraphicCacheItem32, descriptor) == 0x1C);
static_assert(offsetof(GraphicCacheItem32, batchType) == 0x2C);
static_assert(offsetof(GraphicCacheItem32, unwrittenTail) == 0x2D);

// 原 [0x101CEB8,0x1020EB8) 的 4096 项四字节并行记录。
struct GraphicCache3 {
    std::int16_t cacheIndex; // 原成员偏移：+0x00。
    std::uint8_t layer; // 原成员偏移：+0x02。
    std::uint8_t unwrittenByte; // 原成员偏移：+0x03。
};

static_assert(sizeof(GraphicCache3) == 0x04);
static_assert(offsetof(GraphicCache3, cacheIndex) == 0x00);
static_assert(offsetof(GraphicCache3, layer) == 0x02);
static_assert(offsetof(GraphicCache3, unwrittenByte) == 0x03);

// 原 [0xFF4E30,0x1020EB8) 的 x64 承载，不是原内存镜像；前缀别名位置不变。
struct LegacyPendingStorage {
    std::wstring savedPath; // 原FF4E30：启动时写入的保存路径。
    std::uint8_t precedingBatchType = 0; // 原prefix+5C：旧索引-2的类型槽。
    std::array<GraphicCacheItem32, MAX_QUEUE_SIZE> records; // 原全局：0x00FF4EB8。
    std::array<GraphicCache3, MAX_QUEUE_SIZE> cache3; // 原全局：0x0101CEB8。
};


// NSM01：所属会话实际生产/消费同一队列；动作及扩展批次保持非拥有借用。
struct GraphicQueueRuntimeState final {
    LegacyPendingStorage pending{}; // 原0xFF4E30，完整主记录及并行层槽。
    std::uint16_t count = 0; // 原0x1020EB8，仅清零16位。
    std::uint8_t specialGraphicPending = 0; // 原0x1020EDC，特殊图形待提交标志。
};

// 原 gGraphicDirty@0x1020EE8；窗口消息仅在值严格等于 1 时消费并重建渲染器。
extern std::int32_t gGraphicDirty;

// 原 0x103CF10：跨初始化、队列绘制及场景调用的独立完整 W32 模式槽。
extern std::uint32_t gRenderQueueMode103CF10;

// 原 1020ECC：DrawAllQueued 每次清零，520B10 限制有内嵌色表的旧节点刷新次数。
extern std::int32_t gGraphicPaletteUploadCount;

// 原 58B7E0，映像初值 -1；仅当整次绘制没有刷新上传时承接 628FDC。
extern std::int32_t gLastUnrefreshedPaletteFrameVersion;

// 原 0x58B6E0 的 64 项有符号正弦近似表。
extern const std::array<std::int32_t, 64> gRippleSineTable;

// 原507090：按value4匹配主记录，以平行层槽绘框；每轮重读绘框追加后的计数。
void DrawMatchingGraphicHover_507090(session::ClientSession& session, std::int32_t value4);

// 原 sub_5208F0@[0x5208F0,0x520907)：原始 F32 位模式严格按
// R current X -> R current Y -> W previous X -> W previous Y 执行。
std::uint32_t BackupValues_5208F0(session::ClientSession& session);

// 原 sub_520910@[0x520910,0x520923)：单次累加原浮点常量 0.25f。
void AdvanceRipplePhase_520910(session::ClientSession& session);

// 原 sub_520930@[0x520930,0x52096B)：原位逐次加减 64.0f 的浮点相位叶。
void WrapRipplePhase_520930(float* value);

// 原 sub_521870@[0x521870,0x5219B5)：共享 CPU Back/Temp 的波纹条带回拷。
std::int32_t ApplyScreenRipple_521870(session::ClientSession& session);

// 原 Renderer::SortTextures@[0x520790,0x5208E8)；追加原保留项后只排序 cache3，主记录不重排。
void SortTextures_520790(session::ClientSession& session);

// 原 520DD0：原 0..12 及已迁入扩展批次消费，不清队列计数；末尾老化调色板并清特殊标志。
void DrawAllQueued_520DD0(session::ClientSession& session);

// 原 sub_522210@[0x522210,0x522293)：从待提交记录前两项构造矩形，
// 按图号高位标志选择真实 451350 绘制模式；不修改记录。
void DrawQueuedPaletteRect_522210(const GraphicCacheItem32* graphic, std::uint32_t imageNoBits);

// 原 Renderer::FillDirtyArea@[0x520AD0,0x520B09)：从记录取坐标、
// 颜色与混合模式；仅底层返回原 surface-busy 位型时写 gGraphicDirty。
void FillDirtyArea_520AD0(const GraphicCacheItem32* graphic);

// 原 Graphic::LoadPatelet@523140：借用 +22 的 S16 独立调色板索引，保留 0/1/2 返回。
std::uint8_t LoadGraphicIndependentPalette_523140(GraphicCacheItem32* graphic);

// R08：纯ARGB矩形进入同一层序队列；成功返回索引，失败透传原队列返回。
std::int32_t DrawRectangle(session::ClientSession& session,
    std::int16_t x, std::int16_t y, std::int16_t width, std::int16_t height,
    std::uint8_t layer, std::uint32_t color);

// R03：借原 DrawGraphic 生产战斗背景记录，并标为独立缩放批次。
std::int32_t DrawBattleBackgroundScaled(session::ClientSession& session,
    std::uint8_t layer, std::int32_t imageNo, const GraphicDescriptor16* descriptor = nullptr);

// 将成功入队的背景改为全屏批次；保留原索引／失败值、层序与描述符。
// trimTransparentBorder 仅在需要排除源图外围透明行列时启用，默认完整源图。
std::int32_t StretchGraphicToScreen(session::ClientSession& session, std::int32_t pendingIndex, bool trimTransparentBorder = false);

// 原函数：0x00521B10。
std::int32_t DrawGraphic(session::ClientSession& session,
    std::int32_t x,
    std::int32_t y,
    std::uint8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    const GraphicDescriptor16* descriptor);

// Renderer::DrawGraphic_Hover@0x522180 的还原入口。
void DrawGraphicHover(session::ClientSession& session,
    std::uint16_t left,
    std::uint16_t top,
    std::uint16_t right,
    std::uint16_t bottom,
    std::uint8_t layer,
    std::uint32_t imageNo,
    std::int32_t step);

// x64 队列适配：以原 DrawGraphic 返回的 legacy 索引写待提交记录的 batchType。
// index 包括原失败值 -2；调用层不得将其改成数组下标或提前过滤。
void SetPendingGraphicBatchTypeAtLegacyIndex(session::ClientSession& session,
    std::int32_t index,
    std::uint8_t type);

// 原 Renderer::DrawGraphic3@[0x520970,0x520ACE)。
std::int32_t DrawGraphic3_520970(session::ClientSession& session,
    std::int32_t x,
    std::int32_t y,
    std::uint8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    std::int32_t isGraphicNo,
    std::uint8_t batchType,
    const GraphicDescriptor16* descriptor);

// 原 521AA0：只发布地图绘制项；不触碰描述符与特殊图形标志。
std::int32_t QueueMap_521AA0(session::ClientSession& session, std::int32_t x, std::int32_t y, std::uint8_t layer);

// 原 521C80：图号 <=999 也入队；较大图号先 mapNo 查询，再叠加图形偏移。
std::int32_t QueueGraphicWithResourceOffset_521C80(session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::uint8_t layer,
    std::int32_t imageNo, std::int32_t value4,
    const GraphicDescriptor16* descriptor);

// 原 521DD0：直接使用 graphic index，只查询偏移，不做 mapNo 映射。
std::int32_t DrawGraphic2_521DD0(session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::uint8_t layer,
    std::int32_t graphicIndex, std::int32_t value4,
    std::uint8_t batchType, const GraphicDescriptor16* descriptor);

// 原 sub_45F090@[0x45F090,0x45F0FA)：固定混合模式的 legacy 图形提交包装。
std::int32_t QueueOriginalPaletteGraphic_45F090(session::ClientSession& session,
    std::int32_t x,
    std::int32_t y,
    std::uint8_t layer,
    std::int32_t imageNo,
    std::int32_t value4,
    std::int32_t color);

} // namespace cg::render
