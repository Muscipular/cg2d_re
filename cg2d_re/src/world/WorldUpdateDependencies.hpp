#pragma once

#include "cg/LegacyAudio.hpp"
#include "cg/platform/LegacyCryptInt.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>

namespace cg::input {
struct InputContext;
} // namespace cg::input

namespace cg::login::detail {

std::int32_t Sub_508C90(); // 原 [0x508C90,0x508CF0)，登出回复与超时轮询。
std::uint32_t Sub_508C40(); // 原 [0x508C40,0x508C81)，真实请求启动依赖。

} // namespace cg::login::detail

namespace cg::platform {

// 原 0x53B8C6 返回九个 S32 槽；调用方按已确认偏移读取时间字段。
struct LegacyTm53B8C6;
LegacyTm53B8C6* __cdecl LocalTime_53B8C6(std::int32_t* timeValue);

} // namespace cg::platform

namespace cg::login::detail {

void Sub_46DD40(); // 原 0x46DD40；复用 YobiRuntime.cpp 的外部定义。
std::int32_t Sub_520470(); // 原 [0x520470,0x5204BF)，保存当前音频设置。
std::uint8_t Sub_520590(); // 原 [0x520590,0x5205D5)，保存六项标量并返回 AL。
std::uint32_t Sub_520640(); // 原 [0x520640,0x520681)，复制 24 项文本。
std::uint8_t Sub_5206F0(); // 原 [0x5206F0,0x5206FB)，保存 +0x313 并返回 AL。
std::uint8_t Sub_520720(); // 原 [0x520720,0x52072B)，保存 +0x315 并返回 AL。
std::int32_t Sub_520160(); // 原 [0x520160,0x5201A8)，顺序保存并透传 520200 返回。

} // namespace cg::login::detail

namespace cg::world::detail {

// 原 [0x5FB7F8, 0x621FF8)：512 项、每项 0x134 字节，保留未明字段。
struct Record5FB7F8 {
    char text[0x124];
    std::int32_t color;
    std::int32_t state;
    std::int32_t field12C;
    std::int32_t fontType;
};

static_assert(offsetof(Record5FB7F8, text) == 0x000);
static_assert(offsetof(Record5FB7F8, color) == 0x124);
static_assert(offsetof(Record5FB7F8, state) == 0x128);
static_assert(offsetof(Record5FB7F8, field12C) == 0x12C);
static_assert(offsetof(Record5FB7F8, fontType) == 0x130);
static_assert(sizeof(Record5FB7F8) == 0x134);

extern Record5FB7F8 gRecords5FB7F8[512];
extern volatile std::int32_t gWriteIndex626C08;
extern char gLogPath62699C[260];
extern std::FILE* volatile gLogStream626C1C;
extern volatile std::int32_t gCountC9A944;
extern volatile std::uint32_t gValue626C10;
extern input::InputContext gInputContext626840;
extern std::uint32_t gValue626C18;
extern std::uint32_t gValue626BFC;
extern char gLogFileName554648[16];

// 原 0xC8222E；46A700 写 1，469FC0 清 0，46AB60 读取低 W16。
extern std::uint16_t gValueC8222E;

// 原 0xC82230/0xC82234/0xC82236；46A9E0 清零，后续 46AA00/46AB60 共用。
extern std::uint32_t gValueC82230;
extern std::int16_t gValueC82234;
extern std::int16_t gValueC82236;

// 原 0xC3500C/0xC75010/0x9EE8A0/0x9ED438；468F30 按序清零的独立 W32 槽。
extern std::uint32_t gValueC3500C;
extern std::uint32_t gValueC75010;
extern std::uint32_t gValue9EE8A0;
extern std::uint32_t gValue9ED438;
extern std::uint32_t gValue9ED440; // 原 46AA00 写、46AEA0 读取的独立 W32。

// 原 0xC35008/0x9DAD2C/0x9FB930/0xC821D4/0x9A86DC；4682E0 顺序写入。
extern std::uint32_t gValueC35008;
extern std::uint32_t gValue9DAD2C;
extern std::uint32_t gValue9FB930;
extern std::uint32_t gValueC821D4;
extern std::uint32_t gValue9A86DC;

// 原 9DEF64/9DEF60/9DEF62/9DEF5E 与 9A86E0/9ED43C；地图窗口的六个 S16 槽。
extern std::int16_t gMapXStart;
extern std::int16_t gMapYStart;
extern std::int16_t gMapXEnd;
extern std::int16_t gMapYEnd;
extern std::int16_t gMapSpanX;
extern std::int16_t gMapSpanY;
extern platform::LegacyCryptInt gPlayerX; // 原 9EE8A8；468200 等共享。
extern platform::LegacyCryptInt gPlayerY; // 原 9EE8C0；468200 等共享。
extern platform::LegacyCryptInt gPlayerSnapshotX; // 原 C751C0；4681B0 读取。
extern platform::LegacyCryptInt gPlayerSnapshotY; // 原 C751B0；4681B0 读取。
extern platform::LegacyCryptInt gValue9EE8D0;
extern platform::LegacyCryptInt gValue9EE8E0;
extern platform::LegacyCryptInt gValueC27FF8;
extern platform::LegacyCryptInt gValueC27FE8;
extern platform::LegacyCryptInt gValue9A7F88; // 原玩家移动目标 X。
extern platform::LegacyCryptInt gValue9A7F98; // 原玩家移动目标 Y。
extern platform::LegacyCryptInt gValueC821E8;
extern platform::LegacyCryptInt gValue9E03C0;
extern platform::LegacyCryptInt gValue9FB918;
extern platform::LegacyCryptInt gValueAF0FE8;
extern platform::LegacyCryptInt gValueC821D8;
extern std::uint32_t gValueAF10C0; // 原 46CEE0 写入的完整 W32。
extern std::uint32_t gValue9EE8F8; // 原 509460 写入、50C8D0 读取的独立 W32。
extern std::uint32_t gValue9EE8FC; // 原 F32 位槽，移动链共享。
extern std::uint32_t gValue9EE900; // 原 F32 位槽，移动链共享。
extern std::uint32_t gValue9DAD78; // 原 F32 位槽，移动/绘制链共享。

// 原 467F30 独立 W8/W16/W32 写入目标；不覆盖邻接的 C82204。
extern std::uint8_t gValue9A4B94;
extern std::uint16_t gValueC82200;
extern std::uint16_t gValueC82202;
// 原 9A86D4 起始的方向 W16；已证调用链最多访问前两项。
extern std::array<std::uint16_t, 2> gPendingDirections9A86D4;
// 原 AF0FF8/C75014/C750DC；三组 100 项 W16 路径队列共用 C82200 数量。
inline constexpr std::size_t kMovePathCapacity = 100;
extern std::array<std::uint16_t, kMovePathCapacity> gMovePathDirections;
extern std::array<std::uint16_t, kMovePathCapacity> gMovePathX;
extern std::array<std::uint16_t, kMovePathCapacity> gMovePathY;
extern std::uint32_t gValueC821FC;
extern std::uint32_t gValue9EE8B8;
extern std::uint16_t gValueC8A79C;
extern std::uint16_t gValue9A86D8;
extern std::uint16_t gValue9FB92E;
extern std::uint16_t gValueB65A7C;
extern std::uint16_t gValueC82220;
extern std::uint32_t gValueC8A778;
extern std::uint32_t gValueC8A794;
extern std::uint32_t gValueC82208; // 原地图请求 W32；50CF90、509090、509100 等共享。
extern std::uint32_t gValueC8220C;
extern std::uint32_t gValueC82210;
extern std::uint32_t gValueC82214;
extern std::uint32_t gValueC82218;
extern std::int32_t gValueFF0D4C; // 原地图回复数值 W32，51D6F0 消费。
extern std::int32_t gValue73ED44; // 原当前调色板选择标志 W32，458F00 读取。
extern std::int32_t gValue73ED48; // 原时间类别/调色板索引 W32，50C870 与 458F00 共用。
extern std::uint32_t gValueC82224;
extern std::uint32_t gValueC8221C; // 原 46ADA0/46AEA0 共用的 W32 请求序号。
extern std::uint16_t gValue9DAD54;
extern std::uint16_t gValue9FB92C;
extern std::uint16_t gValue55D9FC;
extern std::uint16_t gValueC821D0;
extern std::uint16_t gValue9EE910;

// 原 AEFB94/9DDB0C/9ED44C；468C40 的 tile/obj/hit 共享平面。
inline constexpr std::size_t kMapBufferCells = 2601;
extern std::array<std::uint16_t, kMapBufferCells> gMapTileArray;
extern std::array<std::uint16_t, kMapBufferCells> gMapObjArray;
extern std::array<std::uint16_t, kMapBufferCells> gMapHitArray;
extern std::array<std::uint16_t, kMapBufferCells> gMapWalkabilityArray; // 原 9DEF68；46D9C0 生产。
extern std::uint16_t gLastDetectedCode; // 原 9DAD4A；不随 pending 清理。
extern std::int32_t gValue9E0434; // 原 9E0434；50C870 写，地图路径/保存共读。

// 原 468520；调用方传入 9E0434 当前值，返回 sprintf 的完整结果。
std::int32_t __cdecl Sub_468520(
    char* buffer, std::int32_t mapId, std::int32_t value9E0434, std::int32_t floor);
std::int32_t __cdecl Sub_468570(
    std::int32_t a1, std::int32_t a2, std::int32_t a3,
    std::int32_t a4, std::int32_t a5, std::int32_t a6,
    std::int32_t a7); // 原 [0x468570,0x4687A3)，旧文件比较或新建。
std::int32_t __cdecl Sub_4687B0(
    const char* path, std::int32_t first, std::int32_t second,
    std::int32_t height, std::int32_t width); // 原 [0x4687B0,0x4688A0)，三平面零值文件创建叶。
std::int32_t __cdecl Sub_4688A0(
    std::int32_t mapId, std::int32_t floor,
    std::int32_t xStart, std::int32_t yStart,
    std::int32_t xEnd, std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    std::uint16_t* hit); // 原 [0x4688A0,0x468BF5)，增量写三平面文件及共享 hit。
std::int32_t Sub_469870(std::int32_t direction); // 原 [0x469870,0x469C87)，四向探测依赖。
std::uint32_t Sub_46AF30(); // 原 [0x46AF30,0x46AFDE)，探测后发送 UUN 和发布状态。
std::int32_t __cdecl Sub_468C40(
    std::int32_t mapId, std::int32_t floor,
    std::int32_t xStart, std::int32_t yStart,
    std::int32_t xEnd, std::int32_t yEnd,
    std::uint16_t* tile, std::uint16_t* obj,
    std::uint16_t* hit); // 原 [0x468C40,0x468F25)，真实三平面文件装载依赖。

// 原 0x9DAD5C/0x9DAD68/0x9DAD58/0x9DAD60/0x9DAD64；46DA80 共享状态。
extern std::uint32_t gValue9DAD5C;
extern std::uint32_t gValue9DAD68;
extern std::int32_t gValue9DAD58;
extern std::int32_t gValue9DAD60;
extern std::int32_t gValue9DAD64;

// 原 0x581800：4D3170 重置、World::OpenDialog 读写的独立 W32 槽。
extern std::int32_t gValue581800;

// 原 455BD0 的 23 个独立共享槽；W16/W32 类型仅承载原位模式。
extern std::uint16_t gValue556C14;
extern std::uint16_t gValue62B34E;
extern std::uint16_t gValue62B34C;
extern std::uint16_t gValue62B318;
extern std::uint32_t gValue62B328;
extern std::uint16_t gValue62B3B0;
extern std::uint16_t gValue556C34;
extern std::uint16_t gValue62B5AA;
extern std::uint16_t gValue62B5A8;
extern std::uint32_t gValue62B5B0;
extern std::uint32_t gValue62B308;
extern std::uint32_t gValue62B358;
extern std::uint32_t gValue62B384;
extern std::uint32_t gValue62B310;
extern std::uint32_t gValue62B364;
extern std::uint32_t gValue62B338;
extern std::uint32_t gValue62B36C;
extern std::uint32_t gValue62B2F8;
extern std::uint32_t gValue62B324;
extern std::uint32_t gValue62B30C;
extern std::uint32_t gValue62B388;
extern std::uint32_t gValue62B3A0;
extern std::uint32_t gValue62B38C;

// 原 0xD2F788/0xFF0C10：504A30 重置的独立 S32/W16 槽。
extern std::int32_t gValueD2F788;
extern std::uint16_t gValueFF0C10;

// 原 0xCAFEA0：504100 重置的独立 W32 槽。
extern std::uint32_t gValueCAFEA0;

// 原 0x5ABB70/0x5ABB6C/0x553F90/0x553F84：43C8F0 顺序重置的独立 S32 槽。
extern std::int32_t gValue5ABB70;
extern std::int32_t gValue5ABB6C;
extern std::int32_t gValue553F90;
extern std::int32_t gValue553F84;

// 原 [0x9FB954,0xAEFB94)：.cut 记录 50,000 项，每项仅前 18 字节由文件读取。
struct CutRecord9FB954 {
    std::int32_t value00;
    std::int16_t value04;
    std::int16_t value06;
    std::int16_t value08;
    std::int16_t value0A;
    std::int16_t value0C;
    std::int16_t value0E;
    std::int16_t value10;
    std::uint8_t reserved12[2];
};

// 原 [0x9FB948,0xAEFB94)：十二字节头与 50,000 项定长记录的唯一共享数据。
struct CutData9FB948 {
    std::uint8_t unknown00[6];
    std::int16_t count06;
    std::int16_t count08;
    std::int16_t count0A;
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

extern CutData9FB948 gCutData9FB948;

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

extern AutRecordAF10D8 gAutDataAF10D8[159201];

// 原 [0xC35010, 0xC75010)；46CF20 顺序填值，C75010 尾槽另存。
extern std::int32_t gValuesC35010[65536];

void __cdecl Sub_4682E0(
    std::uint32_t a1,
    std::int32_t a2,
    std::uint32_t a3,
    std::uint32_t a4,
    std::uint32_t a5,
    std::uint32_t a6,
    std::uint32_t a7,
    std::int32_t a8); // 原 [0x4682E0,0x468330)，六槽赋值与参数分发。
void __cdecl Sub_46DA80(
    std::uint32_t a1,
    std::int32_t a2,
    std::int32_t a3,
    std::uint32_t a4); // 原 [0x46DA80,0x46DAF5)，音频重置与模式分支。
std::int32_t __cdecl Sub_46DB00(std::int32_t selected); // 原 [0x46DB00,0x46DC55)，.cut 加载器。
void __cdecl Sub_46DC60(); // 原 [0x46DC60,0x46DC85)，先整块清零再逐项写哨兵。
std::int32_t __cdecl Sub_46DC90(std::int32_t selected); // 原 [0x46DC90,0x46DD35)，.aut 加载器。

void Sub_44B050(const char* text, std::int32_t color, std::int32_t fontType);
std::int32_t Sub_4A76E0();

// 原 50A300 唯一调用的 28 字节记录前向复制叶。
void Sub_5198A0(
    std::uint8_t* destination, const std::uint8_t* source,
    std::int32_t count);                    // 原 [0x5198A0,0x5198CB)，逐记录复制 7 个 W32。

// 原 [0x5198F0,0x51991D)：5198D0 的 qsort 回调，仅比较两条记录的首 S32。
std::int32_t Sub_5198F0(const void* left, const void* right);

// World::Update@0x51C430 的直接目标声明；未实现项只保留 IDA 地址和调用契约。
std::int32_t Sub_45F190();                 // 原 0x45F190。
std::int32_t Sub_464840();                 // 原 0x464840。
void RenderChara_521F20();                 // 原 World::RenderChara@0x521F20。
void RenderInput_4590E0();                 // 原 World::RenderInput@0x4590E0，已完整还原。
void Sub_44A410();                         // 原 0x44A410，输入光标闪烁/排队。
void Sub_4626A0();                         // 原 0x4626A0。
void __cdecl Sub_449DA0();                 // 原 [0x449DA0,0x449F9C)。
std::int32_t __cdecl Sub_46A9E0();         // 原 [0x46A9E0,0x46A9F4)，清三槽、返回 0。
std::int32_t __cdecl Sub_46AA00();         // 原 [0x46AA00,0x46AB56)，方向分类并写 pending。
void __cdecl Sub_46BC90();                // 原 [0x46BC90,0x46BCE4)，三组路径 W16 同步左移。
void __cdecl Sub_46ABD0();                // 原 [0x46ABD0,0x46AC0A)，最多搬运两项方向。
void __cdecl Sub_46BCF0();                // 原 [0x46BCF0,0x46BD1D)，方向计数递减后左移。
std::int32_t* Sub_467F30();                // 原 [0x467F30,0x4681AB)，透传 46CF20 一过尾指针。
std::int32_t __cdecl Sub_4681B0();         // 原 [0x4681B0,0x4681F6)，快照恢复后清六槽。
std::uint16_t __cdecl Sub_468200();        // 原 [0x468200,0x46828D)，仅返回 AX 位模式。
std::int32_t __cdecl Sub_46CEC0();         // 原 [0x46CEC0,0x46CED6)，先读 Y 再对 X 加。
std::int32_t __cdecl Sub_46CEE0();         // 原 [0x46CEE0,0x46CEFB)，并写 AF10C0。
std::uint32_t __cdecl Sub_468330(
    std::int32_t x, std::int32_t y);        // 原 [0x468330,0x468511)，返回 Y 浮点位型。
std::int16_t* __cdecl Sub_468290(
    std::int16_t* xStart, std::int16_t* yStart,
    std::int16_t* xEnd, std::int16_t* yEnd); // 原 [0x468290,0x4682D7)，原地裁剪并返回末指针。
std::int32_t __cdecl Sub_468C00(
    std::int32_t x, std::int32_t y, std::uint16_t value); // 原 [0x468C00,0x468C32)，写 hit WORD。
std::int32_t __cdecl Sub_46AE60(std::int32_t x, std::int32_t y); // 原 [0x46AE60,0x46AE94)，判 hit 低字节。
std::int32_t __cdecl Sub_469810(std::int32_t x, std::int32_t y); // 原 [0x469810,0x469864)，边界外或 walk WORD==1。
std::int32_t __cdecl Sub_469240(
    const std::uint8_t* bytes, std::int32_t length); // 原 [0x469240,0x469284)，三平面校验的 W16 查表叶。
void __cdecl Sub_4691D0(); // 原 [0x4691D0,0x469239)，两次 -1 临时赋值给地图状态。
void __cdecl Sub_50CF90(
    std::uintptr_t socket, std::int32_t sequence,
    std::int32_t status); // 原 [0x50CF90,0x50CFF7)，地图回复序号与状态清理。
std::int32_t __cdecl Sub_469290(
    std::int32_t first, std::int32_t floor,
    std::int32_t xStart, std::int32_t yStart,
    std::int32_t xEnd, std::int32_t yEnd,
    std::int32_t expectedTile, std::int32_t expectedObj,
    std::int32_t expectedHit); // 原 [0x469290,0x4693E9)，真实局部三平面校验依赖。
void __cdecl Sub_509090(
    std::uintptr_t socket, std::int32_t first,
    std::int32_t floor, std::int32_t xStart,
    std::int32_t yStart, std::int32_t xEnd,
    std::int32_t yEnd, std::int32_t expectedTile,
    std::int32_t expectedObj, std::int32_t expectedHit); // 原 [0x509090,0x5090FC)，地图校验回复回调。
std::int32_t __cdecl Sub_509100(
    std::uintptr_t socket, std::int32_t mapId,
    std::int32_t floor, std::int32_t xStart,
    std::int32_t yStart, std::int32_t xEnd,
    std::int32_t yEnd, const char* payload); // 原 [0x509100,0x5093B6)，连续局部帧地图增量回复。
std::int32_t __cdecl Sub_5093F0(
    std::uintptr_t a1, std::int32_t a2, std::int32_t a3,
    std::int32_t a4, std::int32_t a5, std::int32_t a6,
    std::int32_t a7, std::int32_t a8, std::int32_t a9,
    std::int32_t a10, std::int32_t a11, std::int32_t a12,
    std::int32_t a13); // 原 [0x5093F0,0x509548)，地图保存及场景状态发布。
std::int32_t __cdecl Sub_509550(
    std::uintptr_t socket, char* payload); // 原 [0x509550,0x509642)，地图回复文本与数值发布。
void __cdecl Sub_51D6F0(); // 原 [0x51D6F0,0x51D743)，消费回复索引并选择调色板。
void __cdecl Sub_458F40(); // 原 [0x458F40,0x458FC2)，真实玩家时钟后继，原生指针 owner 未批。
void __cdecl Sub_458F00(); // 原 [0x458F00,0x458F3A)，类别变化后可选调色板过渡并进入 458F40。
std::int32_t __cdecl Sub_519EB0(std::int32_t value); // 原 [0x519EB0,0x519EBA)，真实 FDA880 写回依赖。
std::int32_t __cdecl Sub_50C870(
    std::uintptr_t socket, std::int32_t playerValue,
    std::int32_t timeOffset, std::int32_t pathValue,
    std::int32_t adjustment); // 原 [0x50C870,0x50C8C8)，时间与地图配置回复。
std::int32_t __cdecl Sub_469030(); // 原 [0x469030,0x4691C9)，地图装载、第四平面与斜向绘制。
// 原 [0x4693F0,0x4697C9)：先扫 tile，再扫 obj/图形尺寸，输出第四地图平面。
std::int32_t __cdecl Sub_4693F0(
    std::int32_t xStart, std::int32_t yStart, std::int32_t xEnd, std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk);
// 原 [0x46D9C0,0x46DA72)：模式 1 读取 hit 高字节 bit0，其他模式转 4693F0。
std::int32_t __cdecl Sub_46D9C0(
    std::int32_t xStart, std::int32_t yStart, std::int32_t xEnd, std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk);
std::int32_t __cdecl Sub_46AD00(std::int32_t x, std::int32_t y); // 原 [0x46AD00,0x46AD97)，分类并写 9DAD4A。
std::int32_t __cdecl Sub_468F30();         // 原 [0x468F30,0x468F6F)，按序清十二槽、返回 0。
std::int64_t __cdecl Sub_468F70();         // 原 [0x468F70,0x468FE4)，F32 投影、向零截断后写六槽。
std::int32_t __cdecl Sub_46CF00(std::int32_t value); // 原 [0x46CF00,0x46CF1A)，读已初始化的共享索引表。
std::int32_t* __cdecl Sub_46CF20();        // 原 [0x46CF20,0x46CF57)，填表并返回一过尾指针。
void __cdecl Sub_46F8F0(float* value);      // 原 [0x46F8F0,0x46F92B)，原地归一化到 [0,360)。
double __cdecl Sub_46F930(float angle);     // 原 [0x46F930,0x46F94E)，截断后查 360 项 F64 原表。
double __cdecl Sub_46F950(float angle);     // 原 [0x46F950,0x46F967)，F32 加 90 后调用 46F930。
double __cdecl Sub_46F970(float first, float second); // 原 [0x46F970,0x46FACF)，65 项原表插值。
std::int64_t __cdecl Sub_46CDF0(float first, float second); // 原 [0x46CDF0,0x46CE25)，F32 归一化后 S64 截断。
std::int64_t __cdecl Sub_46B580(
    std::int32_t first, std::int32_t second,
    std::int32_t third, std::int32_t fourth); // 原 [0x46B580,0x46B5B3)，两坐标差返回完整 S64。
std::int32_t __cdecl Sub_46B5C0(
    std::int32_t x, std::int32_t y,
    std::int32_t firstX, std::int32_t firstY,
    std::int32_t secondX, std::int32_t secondY); // 原 [0x46B5C0,0x46B6F0)，探测两邻格并覆写路径末项。
std::uint32_t __cdecl Sub_46BD20(
    std::int32_t value, std::int32_t mode); // 原 [0x46BD20,0x46BD32)，仅改 AL，保留 mode 高 24 位。
std::int32_t __cdecl Sub_50C8D0(
    std::int32_t x, std::int32_t y,
    const char* text); // 原 [0x50C8D0,0x50C8F5)，透传 zA 请求返回。
struct RecipeItemData;
RecipeItemData* Sub_5199A0();              // 原 0x5199A0。
struct YobiAction;
YobiAction** Sub_446470();                 // 原 0x446470。
void* __cdecl Sub_46ADA0();               // 原 [0x46ADA0,0x46AE53)，仅透传 51D5B0 的返回位型。
std::int32_t __cdecl Sub_46AC90();        // 原 [0x46AC90,0x46AD00)，type 2 的真实后继。
std::int32_t __cdecl Sub_46AEA0();        // 原 [0x46AEA0,0x46AF24)，type 3 的真实后继。
std::int32_t __cdecl Sub_46AB60();        // 原 [0x46AB60,0x46ABCD)，pending 倒计时与分发。
void Sub_4D3170();                         // 原 [0x4D3170,0x4D317B)，重置 0x581800 为 -1。
std::int32_t Sub_4410E0();                 // 原 [0x4410E0,0x4410E6)，无参返回 17。
std::int32_t Sub_455BD0();                 // 原 [0x455BD0,0x455C56)，顺序重置 23 槽并返回 0。
std::uint32_t* Sub_46E2A0();               // 原 0x46E2A0。
std::int32_t Sub_46BD40();                 // 原 [0x46BD40,0x46BD57)，依次清四个独立 W32 槽并返回 0。
std::int32_t Sub_44A090();                 // 原 [0x44A090,0x44A15B)，读取或创建 chathis.dat。
std::int32_t Sub_449FA0(std::int32_t index); // 原 [0x449FA0,0x44A084)，写入一项历史和尾索引。
std::int32_t Sub_504A30();                 // 原 [0x504A30,0x504A43)，调用 44A090、清两槽并返回 0。
std::int32_t Sub_43C8F0();                 // 原 [0x43C8F0,0x43C90A)，四槽顺序写入并返回 -1。
std::int32_t Sub_504100();                 // 原 [0x504100,0x504113)，顺序清三槽、返回 0。
std::int32_t Sub_518C80();                 // 原 0x518C80。
std::int32_t Sub_518CB0();                 // 原 [0x518CB0,0x518D94)：返回 fclose/helper 整数。
void Sub_518780();                         // 原 0x518780。
std::int8_t Sub_503E80();                  // 原 0x503E80，IDA 原型 char()。
using cg::audio::Sub_52B990;               // 原 0x52B990。
std::int32_t Sub_45EBB0();                 // 原 0x45EBB0。
void Sub_45ED80();                        // 原 [0x45ED80,0x45ED8B)，重置 9A6204。
extern std::uint32_t gValue9A6204;         // 原 0x9A6204；45ED90 后续复用。
std::int32_t Sub_45ED90();                 // 原 [0x45ED90,0x45EEDA)，场景 7 请求与展示。
void DrawNormalScene_51C7D0();             // 原 World::DrawNormalScene@0x51C7D0。
std::int8_t Sub_519D50();                  // 原 0x519D50，IDA 原型 char()。
void* __cdecl Sub_51D5B0();               // 原 [0x51D5B0,0x51D63A)；非空链尾返指针，空链 EAX 未定义；46ADA0 透传位型。
void __cdecl Sub_4464D0();                // 原 [0x4464D0,0x446604)，场景对象/Yobi 清理。
std::int8_t Sub_51D750();                  // 原 0x51D750，IDA 原型 char()。
using cg::login::detail::Sub_520160;
void DrawBattleScene_43CBB0();             // 原 World::DrawBattleScene@0x43CBB0。

// 原 0x51FDE0；第三槽保持原 32 位载荷，浮点目标自行解释位模式。
std::int32_t Sub_51FDE0(
    std::int32_t selector, std::int32_t value, std::uint32_t payloadBits);
// 51FDE0 原调用均压入三个 W32 槽；新端声明只保留目标实际读取的逻辑参数。
std::int32_t Sub_51D790(
    std::int32_t index, std::int32_t value, std::uint32_t payloadBits);
std::int32_t Sub_51E0D0(
    std::int32_t index, std::int32_t value, std::uint32_t payloadBits);
std::int32_t Sub_51EC10(std::int32_t index, std::int32_t value);
std::int32_t Sub_51EF80(
    std::int32_t index, std::int32_t value, std::uint32_t payloadBits);
std::int32_t Sub_51F610(std::int32_t value);
std::int32_t Sub_51F860(std::int32_t index, std::int32_t value);
std::int32_t Sub_51FA70(std::int32_t index, std::int32_t value);
std::int32_t Sub_51FC90(std::int32_t index, std::int32_t value);
std::int32_t Sub_455C90();                 // 原 0x455C90，返回值按任意非零判断。
extern char gDisconnectText9A4D10[16];   // 原 [0x9A4D10,0x9A4D20)，独立零初值文本。

// 原 dword_D460C8@0xD460C8；只保留地址身份和 W32 宽度。
extern std::int32_t gValueD460C8;

} // namespace cg::world::detail
