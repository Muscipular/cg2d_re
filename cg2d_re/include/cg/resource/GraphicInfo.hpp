#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::resource {

// 原553058/55305C：各固定容器构造后立即登记宿主退出，返回实际S32。
// 当前仅模块适配调用；接原全应用序时必须同时移除适配，不能双执行。
// 原函数：0x004597E0。
void InitializeGraphicMapNumbers_4597E0();
// 原函数：0x00459850。
void InitializeGraphicFirstRecords_459850();

// GraphicInit@0x459960 建立的运行时记录；不含指针，x64 下步长仍为 0x20。
// 字段写入证据及磁盘布局区别见 cg2d_re/doc/cg2d_graphic_info.md。
struct GraphicInfo {
    std::uint32_t seqNo;       // +0x00：图形序号的 32 位存储。
    std::int32_t offset;       // +0x04：图形数据偏移，供 GraphicRead 按有符号 32 位读取。
    std::uint32_t length;      // +0x08：图形数据长度的 32 位存储。
    std::int16_t x;            // +0x0C：GetGraphicXY 输出的横向偏移。
    std::int16_t y;            // +0x0E：GetGraphicXY 输出的纵向偏移。
    std::int16_t w;            // +0x10：GetGraphicWH 输出的宽度。
    std::int16_t h;            // +0x12：GetGraphicWH 输出的高度。
    std::int16_t fileIndex;    // +0x14：文件槽号；0x45A935 使用 movsx。
    std::uint8_t reserved[2];  // +0x16：用途未明的两个字节，不补充语义。
    std::uint8_t sx;           // +0x18：GetGraphicSxSy 零扩展为 16 位。
    std::uint8_t sy;           // +0x19：GetGraphicSxSy 零扩展为 16 位。
    std::uint16_t flag;        // +0x1A：GetGraphicFlag1/2 按 100 取余/相除。
    std::int32_t mapNo;        // +0x1C：GetGraphicMapNo 返回的 32 位位模式。
};

static_assert(std::is_standard_layout_v<GraphicInfo>);
static_assert(std::is_trivially_copyable_v<GraphicInfo>);
static_assert(sizeof(GraphicInfo) == 0x20);
static_assert(offsetof(GraphicInfo, seqNo) == 0x00);
static_assert(offsetof(GraphicInfo, offset) == 0x04);
static_assert(offsetof(GraphicInfo, length) == 0x08);
static_assert(offsetof(GraphicInfo, x) == 0x0C);
static_assert(offsetof(GraphicInfo, y) == 0x0E);
static_assert(offsetof(GraphicInfo, w) == 0x10);
static_assert(offsetof(GraphicInfo, h) == 0x12);
static_assert(offsetof(GraphicInfo, fileIndex) == 0x14);
static_assert(offsetof(GraphicInfo, reserved) == 0x16);
static_assert(offsetof(GraphicInfo, sx) == 0x18);
static_assert(offsetof(GraphicInfo, sy) == 0x19);
static_assert(offsetof(GraphicInfo, flag) == 0x1A);
static_assert(offsetof(GraphicInfo, mapNo) == 0x1C);

// GraphicInfo 磁盘记录的 40 字节布局；不是 GraphicInfo，不能直接当作运行时表。
// GraphicInit_459960 按原写序逐字段转换，不覆盖运行时保留字节。
struct GraphicInfoRecord {
    std::uint32_t seqNo;       // +0x00：图形序号的 32 位存储。
    std::uint32_t offset;      // +0x04：图形数据偏移的 32 位存储。
    std::uint32_t length;      // +0x08：图形数据长度的 32 位存储。
    std::int32_t x;            // +0x0C：磁盘横向偏移，运行时仅存 16 位。
    std::int32_t y;            // +0x10：磁盘纵向偏移，运行时仅存 16 位。
    std::int32_t w;            // +0x14：磁盘宽度，运行时仅存 16 位。
    std::int32_t h;            // +0x18：磁盘高度，运行时仅存 16 位。
    std::uint8_t sx;           // +0x1C：运行时 sx 的来源字节。
    std::uint8_t sy;           // +0x1D：运行时 sy 的来源字节。
    std::uint16_t flag;        // +0x1E：运行时 flag 的来源。
    std::uint8_t reserved[4];  // +0x20：磁盘保留字节，不等同于运行时 +0x16。
    std::int32_t mapNo;        // +0x24：运行时 mapNo 的 32 位来源。
};

static_assert(std::is_standard_layout_v<GraphicInfoRecord>);
static_assert(std::is_trivially_copyable_v<GraphicInfoRecord>);
static_assert(sizeof(GraphicInfoRecord) == 0x28);
static_assert(offsetof(GraphicInfoRecord, seqNo) == 0x00);
static_assert(offsetof(GraphicInfoRecord, offset) == 0x04);
static_assert(offsetof(GraphicInfoRecord, length) == 0x08);
static_assert(offsetof(GraphicInfoRecord, x) == 0x0C);
static_assert(offsetof(GraphicInfoRecord, y) == 0x10);
static_assert(offsetof(GraphicInfoRecord, w) == 0x14);
static_assert(offsetof(GraphicInfoRecord, h) == 0x18);
static_assert(offsetof(GraphicInfoRecord, sx) == 0x1C);
static_assert(offsetof(GraphicInfoRecord, sy) == 0x1D);
static_assert(offsetof(GraphicInfoRecord, flag) == 0x1E);
static_assert(offsetof(GraphicInfoRecord, reserved) == 0x20);
static_assert(offsetof(GraphicInfoRecord, mapNo) == 0x24);

// 原 VA 0x103CFA0：查询边界；未初始化阶段为 0。
extern std::int32_t gMaxGraphicCount;
// 原 VA 0x9A2970：运行时图形表；未初始化阶段为 nullptr，不在本模块分配。
extern GraphicInfo* gGraphicInfo;
// 原 VA 0x9A2974：MapNo 到图形序号的索引表；未初始化阶段为 nullptr。
extern std::int32_t* gGraphicMapNoIndex;
// 原 73F100：九个文件槽的全局图号基址，供 AnimeLoadAction/CoordInit 共同读取。
extern std::int32_t gGraphicStartIndices[9];

// 原 [459960,45A12C)：先开九个数据文件，再预扫/重开 Info，写主表与 mapNo 索引。
// 主表须由 GraphicAlloc 分配；失败保留原已发布状态，返回 1/2/7、3、-1 或 0。
// 两组参数分别为数据、Info 的九个窄字节路径，不在此重建或转码。
std::int32_t GraphicInit_459960(
    const char* const* graphicPaths, const char* const* graphicInfoPaths);

// 调用前提：输出指针非空且可写；实际读表前，相应全局状态须已绑定有效存储。
// 保留原有检查，不新增空指针、负计数修复或存储容量检查。

// 原 [0x45A710, 0x45A760)：无符号索引比较；依次写 x、y，成功 1，失败清零并返回 0。
std::int32_t GetGraphicXY(std::uint32_t index, std::int16_t* x, std::int16_t* y);
// 原 [0x45A760, 0x45A7B0)：同上，依次写 w、h。
std::int32_t GetGraphicWH(std::uint32_t index, std::int16_t* w, std::int16_t* h);
// 原 [0x45A7B0, 0x45A802)：同上，依次将 sx、sy 从 8 位零扩展后写入 16 位输出。
std::int32_t GetGraphicSxSy(std::uint32_t index, std::uint16_t* sx, std::uint16_t* sy);
// 原 [0x45A810, 0x45A851)：无符号索引比较；输出零扩展 flag % 100，失败清零并返回 0。
std::int32_t GetGraphicFlag1(std::uint32_t index, std::uint16_t* flag);
// 原 [0x45A860, 0x45A8A8)：无符号索引比较；输出零扩展 flag / 100，失败清零并返回 0。
std::int32_t GetGraphicFlag2(std::uint32_t index, std::uint16_t* flag);
// 原 [0x45A8B0, 0x45A8C9)：不检查 index；调用者必须保证 gGraphicMapNoIndex[index] 可访问。
// 直接写入查表结果并返回 1，不以 gMaxGraphicCount 检查此索引。
std::int32_t GetGraphicMapNoIndex(std::int32_t index, std::int32_t* outSeqNo);
// 原 [0x45C970, 0x45CA1B)：按固定五个图号两阶段探测，返回 -1 或 4-index。
// 首轮命中后从同一索引重新查询；调用者负责保存返回值。
std::int32_t ProbePaletteCapability_45C970(); // PCR01：五原图号两轮重查，输出原不预清，返回-1或4-index。
// 原 [0x45A8D0, 0x45A8F2)：有符号 index < 0 或 index >= gMaxGraphicCount 返回 -1；
// 否则直接返回 mapNo 的 32 位位模式。
std::int32_t GetGraphicMapNo(std::int32_t index);

} // namespace cg::resource
