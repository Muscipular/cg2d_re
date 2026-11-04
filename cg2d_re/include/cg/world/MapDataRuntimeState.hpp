#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::world {

inline constexpr std::size_t kMapBufferCells = 2601;
inline constexpr std::size_t kAutoMapWindowCells = 11664;

// 原 [0xAF10D8,0xB65A7B)；.aut 每项三个原始字节，供地图颜色消费者共享。
struct AutRecordAF10D8 {
    std::uint8_t value00; // 原 +0。
    std::uint8_t value01; // 原 +1。
    std::uint8_t value02; // 原 +2。
};

static_assert(offsetof(AutRecordAF10D8, value00) == 0);
static_assert(offsetof(AutRecordAF10D8, value01) == 1);
static_assert(offsetof(AutRecordAF10D8, value02) == 2);
static_assert(sizeof(AutRecordAF10D8) == 3);

namespace detail {

// 原 [0x9FB954,0xAEFB94)：.cut 记录 50,000 项，每项仅前 18 字节由文件读取。
struct CutRecord9FB954 {
    std::int32_t value00; // 原成员偏移：+0x00。 图号；46CF60 的 tile 直接使用，静态 obj 先查 mapNo。
    std::int16_t value04; // 原成员偏移：+0x04。 obj 动画标志；0 静态，1 Yobi，其余跳过。
    std::int16_t value06; // 原成员偏移：+0x06。 裁剪 X。
    std::int16_t value08; // 原成员偏移：+0x08。 裁剪 Y。
    std::int16_t value0A; // 原成员偏移：+0x0A。 宽。
    std::int16_t value0C; // 原成员偏移：+0x0C。 高。
    std::int16_t value0E; // 原成员偏移：+0x0E。 绘制偏移 X。
    std::int16_t value10; // 原成员偏移：+0x10。 绘制偏移 Y。
    std::uint8_t reserved12[2]; // 原成员偏移：+0x12。
};

// 原 [0x9FB948,0xAEFB94)：十二字节头与 50,000 项定长记录的唯一共享数据。
struct CutData9FB948 {
    std::uint8_t unknown00[6]; // 原成员偏移：+0x00。
    std::int16_t count06; // 原成员偏移：+0x06。
    std::int16_t count08; // 原成员偏移：+0x08。
    std::int16_t count0A; // 原成员偏移：+0x0A。
    CutRecord9FB954 records[50000];
};

static_assert(std::is_standard_layout_v<CutRecord9FB954> && std::is_trivial_v<CutRecord9FB954>);
static_assert(std::is_standard_layout_v<CutData9FB948> && std::is_trivial_v<CutData9FB948>);
static_assert(offsetof(CutRecord9FB954, value00) == 0x00);
static_assert(offsetof(CutRecord9FB954, value04) == 0x04);
static_assert(offsetof(CutRecord9FB954, value06) == 0x06);
static_assert(offsetof(CutRecord9FB954, value08) == 0x08);
static_assert(offsetof(CutRecord9FB954, value0A) == 0x0A);
static_assert(offsetof(CutRecord9FB954, value0C) == 0x0C);
static_assert(offsetof(CutRecord9FB954, value0E) == 0x0E);
static_assert(offsetof(CutRecord9FB954, value10) == 0x10);
static_assert(offsetof(CutRecord9FB954, reserved12) == 0x12);
static_assert(sizeof(CutRecord9FB954) == 0x14);
static_assert(offsetof(CutData9FB948, count06) == 0x06);
static_assert(offsetof(CutData9FB948, count08) == 0x08);
static_assert(offsetof(CutData9FB948, count0A) == 0x0A);
static_assert(offsetof(CutData9FB948, records) == 0x0C);
static_assert(sizeof(CutData9FB948) == 1000012);

// 原55DA94三组可写S32尺寸，供46C840及绘制共同读取。
struct AutoMapWindowSize {
    std::int32_t width;
    std::int32_t height;
};
static_assert(sizeof(AutoMapWindowSize) == 8);

} // namespace detail

// 地图解析、寻路、动画生产和视图共同借用本会话实际数据，不在切换时复制。
struct MapDataRuntimeState {
    std::array<std::uint16_t, kMapBufferCells> tiles {}; // 原AEFB94。
    std::array<std::uint16_t, kMapBufferCells> objects {}; // 原9DDB0C。
    std::array<std::uint16_t, kMapBufferCells> hits {}; // 原9ED44C。
    std::array<std::uint16_t, kMapBufferCells> walkability {}; // 原9DEF68。
    detail::CutData9FB948 cut {}; // 原9FB948。
    AutRecordAF10D8 aut[159201] {}; // 原AF10D8。
    std::int32_t graphicNumbers[65536] {}; // 原C35010，尾地址投影Y偏移槽仍独立。
    std::int16_t autoMapWindowIndex = 1; // 原55DA90。
    detail::AutoMapWindowSize autoMapWindowSizes[3] {{107, 107}, {54, 54}, {27, 27}}; // 原55DA94。
    std::uint32_t autoMapColors[kAutoMapWindowCells] {}; // 原9CF6EC。
    std::uint8_t autoMapIndices[kAutoMapWindowCells] {}; // 原9DAD7C。
};
static_assert(sizeof(MapDataRuntimeState::tiles) == 5202);
static_assert(sizeof(MapDataRuntimeState::objects) == 5202);
static_assert(sizeof(MapDataRuntimeState::hits) == 5202);
static_assert(sizeof(MapDataRuntimeState::walkability) == 5202);
static_assert(sizeof(MapDataRuntimeState::aut) == 477603);
static_assert(sizeof(MapDataRuntimeState::graphicNumbers) == 0x40000);
static_assert(sizeof(MapDataRuntimeState::autoMapColors) == 46656);
static_assert(sizeof(MapDataRuntimeState::autoMapIndices) == 11664);

} // namespace cg::world
