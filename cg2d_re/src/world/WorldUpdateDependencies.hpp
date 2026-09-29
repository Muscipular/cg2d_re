#pragma once

#include "MailState.hpp"

#include "cg/LegacyAudio.hpp"
#include "cg/platform/LegacyCryptInt.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>

namespace cg::render {
struct GraphicDescriptor16;
}

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

// 原四个地图绘制源计数，仍由 SceneState 持有并由 46BD40 逐项清零。
extern std::uint32_t gValue9FB934;
extern std::uint32_t gValue9FB928;
extern std::uint32_t gValue9A86C0;
extern std::uint32_t gValue9A7F78;

// 原 46BD60/46BE50/46DFF0：世界坐标为 F32 原位型，描述符同步借用。
std::int32_t Sub_46BD60(
    std::int32_t imageNo, std::uint32_t worldXBits, std::uint32_t worldYBits,
    std::int16_t x, std::int16_t y, std::int32_t mode,
    const render::GraphicDescriptor16* descriptor);
void Sub_46BE50(
    std::int32_t imageNo, std::uint32_t worldXBits, std::uint32_t worldYBits,
    std::int16_t x, std::int16_t y, const render::GraphicDescriptor16* descriptor);
void Sub_46DFF0(
    std::int32_t imageNo, std::uint32_t worldXBits, std::uint32_t worldYBits,
    std::int16_t x, std::int16_t y, const render::GraphicDescriptor16* descriptor);
// 原 46C060：挂接主项的前绘/后绘链；batch 保留低 W8，不布尔化。
void DrawGearNormal_46C060(
    std::int32_t primaryIndex, std::int32_t before, std::int32_t imageNo,
    std::int16_t x, std::int16_t y,
    const render::GraphicDescriptor16* descriptor, std::uint8_t batchType);
// 原 46C2F0：排序→合并，caller 不消费混合 EAX；46C1C0 返回末次合并计数。
void Sub_46C2F0();
std::int32_t Sub_46C1C0();

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

// 原73ECD0，508CF0与多个场景/UI共享；显式CRT生命周期，禁止创建登录副本。
extern platform::LegacyCryptInt gValue73ECD0;

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

// 借用SceneState唯一方向表，分别读取原55D9DC/55D9DE的S16分量。
std::int16_t ReadDirectionDeltaX(std::int16_t direction) noexcept;
std::int16_t ReadDirectionDeltaY(std::int16_t direction) noexcept;

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
extern std::uint16_t gValue9EE8A4; // 46AF30 发布方向，46CF60 按 S16 重用探测。
extern std::uint16_t gValueC8A79C;
extern std::uint16_t gValue9A86D8;
extern std::uint16_t gValue9FB92E;
extern std::uint16_t gValueB65A7C;
extern std::uint16_t gValueC82220;
extern std::uint32_t gValueC82238; // 46B370目标格转向时间，与C8223C独立。
extern std::uint32_t gValueC8223C; // 原转向发送时间U32，非地址。
extern std::uint32_t gValueC8A774; // 原左键目标事件门禁。
extern std::uint32_t gValueC8A778;
extern std::uint32_t gValueC8A77C; // UI与目标格输入共用的W32许可槽。
extern std::uint32_t gValueC8A780; // UI与目标格绘制共用的另一W32许可槽。
extern std::int32_t gPlayerMoveTargetX; // 原C8A784，RendererCursorGrid发布的S32。
extern std::int32_t gPlayerMoveTargetY; // 原C8A788，不是目标CryptInt或当前格坐标。
extern std::int32_t gValueC8A78C; // 原鼠标世界像素X。
extern std::int32_t gValueC8A790; // 原鼠标世界像素Y。
extern std::uint32_t gValueC8A794;
extern std::uint32_t gValueC8A7A0; // 原46F680目标格左键单次事件。
extern std::uint32_t gValueC8A7A4; // 原目标格左键持续事件。
extern std::uint32_t gValueC8A7A8; // 原目标对象交互事件，非鼠标edge位槽。
extern std::uint32_t gValueC8A7AC; // 原右键持续事件，单次事件优先。
extern std::uint32_t gValue55DDAC; // 原点击目标时间，映像初值FFFFFFFF。
extern std::uint32_t gValueC82208; // 原地图请求 W32；50CF90、509090、509100 等共享。
extern std::uint32_t gValueC8220C;
extern std::uint32_t gValueC82210;
extern std::uint32_t gValueC82214;
extern std::uint32_t gValueC82218;
extern std::int32_t gValueFF0D4C; // 原地图回复数值 W32，51D6F0 消费。
extern std::int32_t gValue73ED44; // 原当前调色板选择标志 W32，458F00 读取。
extern std::int32_t gValue73ED48; // 原时间类别/调色板索引 W32，50C870 与 458F00 共用。
extern std::uint32_t& gMapEffectOffsetXBits; // 原C8A758；别名既有场景槽，4483A0写、46F1D0读。
extern std::uint32_t& gMapEffectOffsetYBits; // 原C8A75C；保留F32位型及独立写序。
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
extern std::int32_t gDialogRequestCC21E0; // 原对话a3，49D2E0/49D8F0生产，发送共用。
extern std::int32_t gDialogTargetD2DE6C; // 原对话a4，不是窗口51的独立参数槽。

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
extern std::uint32_t gValue62B398; // 原455FC0发布，目标格门禁共用。
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
    std::int32_t value00; // 图号；46CF60 的 tile 直接使用，静态 obj 先查 mapNo。
    std::int16_t value04; // obj 动画标志；0 静态，1 Yobi，其余跳过。
    std::int16_t value06; // 裁剪 X。
    std::int16_t value08; // 裁剪 Y。
    std::int16_t value0A; // 宽。
    std::int16_t value0C; // 高。
    std::int16_t value0E; // 绘制偏移 X。
    std::int16_t value10; // 绘制偏移 Y。
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

// 原[509650,509B96)，玩家属性掩码回复，socket未用。
std::int32_t Sub_509650(std::uintptr_t socket, const char* payload);
void Sub_509BA0(std::uintptr_t unusedSocket, const char* payload); // 附加属性按掩码消费，步速同步同源action。
void Sub_509E30(std::uintptr_t unusedSocket, std::int32_t value, char* text); // 先写低U8，再原位反转义职业名。
const char* BorrowPlayerJobName() noexcept; // F1BAE0职业名，同源28B字段只读借用。

// 原技能+3C的94H技术记录；50A120生产，数值语义未证的字段保持地址命名。
struct SkillTechniqueRecord {
    std::array<char, 25> name;
    std::array<char, 97> description;
    std::array<std::byte, 2> bytes7A;
    std::int32_t value7C;
    std::int32_t value80;
    std::int32_t value84;
    std::uint8_t used88;
    std::array<std::byte, 3> bytes89;
    std::int32_t value8C;
    std::int32_t value90;
};

static_assert(std::is_standard_layout_v<SkillTechniqueRecord>);
static_assert(std::is_trivially_copyable_v<SkillTechniqueRecord>);
static_assert(offsetof(SkillTechniqueRecord, name) == 0);
static_assert(offsetof(SkillTechniqueRecord, description) == 0x19);
static_assert(offsetof(SkillTechniqueRecord, bytes7A) == 0x7A);
static_assert(offsetof(SkillTechniqueRecord, value7C) == 0x7C);
static_assert(offsetof(SkillTechniqueRecord, value80) == 0x80);
static_assert(offsetof(SkillTechniqueRecord, value84) == 0x84);
static_assert(offsetof(SkillTechniqueRecord, used88) == 0x88);
static_assert(offsetof(SkillTechniqueRecord, bytes89) == 0x89);
static_assert(offsetof(SkillTechniqueRecord, value8C) == 0x8C);
static_assert(offsetof(SkillTechniqueRecord, value90) == 0x90);
static_assert(sizeof(SkillTechniqueRecord) == 0x94);

// 原技能配方材料0x28字节，50C580生产、519950只清itemId、4F4630查count。
struct SkillRecipeItem {
    std::int32_t itemId;
    std::int32_t count;
    std::array<char, 29> name;
    std::array<std::byte, 3> bytes25;
};

struct SkillRecipeRecord {
    std::int32_t itemId;
    std::int32_t value04;
    std::int32_t level;
    std::int32_t value0C;
    std::int32_t value10;
    std::array<char, 29> name;
    std::array<char, 57> description;
    std::array<std::byte, 2> bytes6A;
    std::array<SkillRecipeItem, 5> items6C;
};

// 原F1BAFC起的技能记录；未消费字段不计最终业务映射。
struct SkillRuntimeRecord {
    std::array<char, 25> name;
    std::array<std::uint8_t, 2> requiredSlots19; // 回复写低U8；4740F0读S16，窗口54读低S8。
    std::byte byte1B;
    std::int32_t level;
    std::int32_t maxLevel;
    std::int32_t experience;
    std::int32_t nextExperience;
    std::int32_t canRebirth;
    std::int32_t skillId;
    std::int32_t fpReduce;
    std::int32_t sequence;
    std::array<SkillTechniqueRecord, 15> techniques3C;
    std::array<SkillRecipeRecord, 56> recipes8E8;
    std::int32_t rebirthState4C48; // 原F20744；不是旧文档的+744。
};

struct SkillPairRecord {
    std::uint16_t used;
    std::uint16_t index; // 原技能/宠物技术表下标，UI按S16消费，不是地址。
    std::int32_t sequence;
};

static_assert(sizeof(SkillRecipeItem) == 0x28);
static_assert(offsetof(SkillRecipeItem, count) == 4);
static_assert(offsetof(SkillRecipeItem, name) == 8);
static_assert(offsetof(SkillRecipeItem, bytes25) == 0x25);
static_assert(offsetof(SkillRecipeRecord, value04) == 4);
static_assert(offsetof(SkillRecipeRecord, level) == 8);
static_assert(offsetof(SkillRecipeRecord, value0C) == 0x0C);
static_assert(offsetof(SkillRecipeRecord, value10) == 0x10);
static_assert(offsetof(SkillRecipeRecord, name) == 0x14);
static_assert(offsetof(SkillRecipeRecord, description) == 0x31);
static_assert(offsetof(SkillRecipeRecord, bytes6A) == 0x6A);
static_assert(offsetof(SkillRecipeRecord, items6C) == 0x6C);
static_assert(sizeof(SkillRecipeRecord) == 0x134);
static_assert(std::is_standard_layout_v<SkillRuntimeRecord>);
static_assert(std::is_trivially_copyable_v<SkillRuntimeRecord>);
static_assert(offsetof(SkillRuntimeRecord, requiredSlots19) == 0x19);
static_assert(offsetof(SkillRuntimeRecord, byte1B) == 0x1B);
static_assert(offsetof(SkillRuntimeRecord, level) == 0x1C);
static_assert(offsetof(SkillRuntimeRecord, maxLevel) == 0x20);
static_assert(offsetof(SkillRuntimeRecord, experience) == 0x24);
static_assert(offsetof(SkillRuntimeRecord, nextExperience) == 0x28);
static_assert(offsetof(SkillRuntimeRecord, canRebirth) == 0x2C);
static_assert(offsetof(SkillRuntimeRecord, skillId) == 0x30);
static_assert(offsetof(SkillRuntimeRecord, fpReduce) == 0x34);
static_assert(offsetof(SkillRuntimeRecord, sequence) == 0x38);
static_assert(offsetof(SkillRuntimeRecord, techniques3C) == 0x3C);
static_assert(offsetof(SkillRuntimeRecord, recipes8E8) == 0x8E8);
static_assert(offsetof(SkillRuntimeRecord, rebirthState4C48) == 0x4C48);
static_assert(sizeof(SkillRuntimeRecord) == 0x4C4C);
static_assert(offsetof(SkillPairRecord, used) == 0);
static_assert(offsetof(SkillPairRecord, index) == 2);
static_assert(offsetof(SkillPairRecord, sequence) == 4);
static_assert(sizeof(SkillPairRecord) == 8);

extern std::array<SkillRuntimeRecord, 15>& gSkillsF1BAFC; // 同一第四清零块成员别名。
extern std::array<SkillPairRecord, 15>& gSkillPairsF63370;
extern std::uint16_t gValueDA3B20; // 原技能拖放门禁，509EB0尾部清零。
std::int32_t Sub_504F10(std::int32_t value, std::int32_t mode); // 原序查询recipe.value10，含索引表末次别名读取。
std::int32_t Sub_505040(std::int32_t slot); // 说明/类别/flag800及制造配方条件。
std::int32_t Sub_5050B0(std::int32_t slot); // 类别80或flag800与601..605技能配方条件。
std::int32_t Sub_505110(std::int32_t slot); // 说明、两段物品类别及flag1000条件。
void Sub_509EB0(std::uintptr_t socket, const char* payload);
void Sub_50A120(
    std::uintptr_t unusedSocket, std::int32_t skillIndex, const char* payload); // 清技术区后按六字段生产。
void Sub_50C580(
    std::uintptr_t unusedSocket, std::int32_t index, const char* payload); // 固定56行、每行五材料。
void Sub_519950(std::int32_t index); // 仅重置技能尾槽及56行配方编号。
void Sub_518600(
    const SkillRuntimeRecord* source, SkillPairRecord* destination, std::int32_t count);
std::int32_t Sub_518660(const SkillPairRecord* left, const SkillPairRecord* right);

// 原FDA8A4起的28条玩家物品，已证原字节文本与W16/S32字段；未消费尾部不冻结。
struct PlayerItemRecord {
    std::uint16_t used;
    std::array<char, 29> name;
    std::array<char, 17> memo;
    std::array<std::array<char, 96>, 32> description;
    std::int32_t pages;
    std::int32_t color;
    std::int32_t imageNo;
    std::int32_t level;
    std::int32_t itemId;
    std::int32_t count;
    std::int32_t itemType;
    std::uint16_t canUseInField;
    std::uint16_t battleUseFlag;
    std::uint16_t target;
    std::uint16_t flags;
    std::uint16_t hasDescription;
    std::array<std::byte, 6> bytesC56;
};

static_assert(std::is_standard_layout_v<PlayerItemRecord>);
static_assert(std::is_trivially_copyable_v<PlayerItemRecord>);
static_assert(offsetof(PlayerItemRecord, used) == 0);
static_assert(offsetof(PlayerItemRecord, name) == 2);
static_assert(offsetof(PlayerItemRecord, memo) == 0x1F);
static_assert(offsetof(PlayerItemRecord, description) == 0x30);
static_assert(offsetof(PlayerItemRecord, pages) == 0xC30);
static_assert(offsetof(PlayerItemRecord, color) == 0xC34);
static_assert(offsetof(PlayerItemRecord, imageNo) == 0xC38);
static_assert(offsetof(PlayerItemRecord, level) == 0xC3C);
static_assert(offsetof(PlayerItemRecord, itemId) == 0xC40);
static_assert(offsetof(PlayerItemRecord, count) == 0xC44);
static_assert(offsetof(PlayerItemRecord, itemType) == 0xC48);
static_assert(offsetof(PlayerItemRecord, canUseInField) == 0xC4C);
static_assert(offsetof(PlayerItemRecord, battleUseFlag) == 0xC4E);
static_assert(offsetof(PlayerItemRecord, target) == 0xC50);
static_assert(offsetof(PlayerItemRecord, flags) == 0xC52);
static_assert(offsetof(PlayerItemRecord, hasDescription) == 0xC54);
static_assert(offsetof(PlayerItemRecord, bytesC56) == 0xC56);
static_assert(sizeof(PlayerItemRecord) == 0xC5C);

extern std::array<PlayerItemRecord, 28>& gPlayerItemsFDA8A4; // 别名唯一PlayerRuntimeSlots成员。
void Sub_50BFD0(std::uintptr_t unusedSocket, const char* payload); // 固定28槽全量回复。
void Sub_50C260(std::uintptr_t unusedSocket, const char* payload); // 带槽号的14字段增量回复。
void Sub_50C900(
    std::uintptr_t unusedSocket, std::int32_t first, std::int32_t second); // 玩家/UI/拖动/副本按序换位。
void Sub_51B530(std::int32_t first, std::int32_t second); // 整条C5C交换，装备位标志及UI迁移。
std::int32_t Sub_51B5F0(std::int32_t itemId); // 仅背包8..27槽，完整W16占用/匹配S32编号后累加数量。
// 原51B630：玩家/宠物容器间整条换位，不改装备标志或同步UI。
void Sub_51B630(
    std::int32_t firstGroup, std::int32_t firstIndex,
    std::int32_t secondGroup, std::int32_t secondIndex);
void Sub_5196B0(std::int32_t index, const char* text); // 32行描述与最多4页的原回扫。

// 原宠物+D8的8C技能值记录，50B0D0生产，4E52B0按索引对复制到交易区。
struct PetTechniqueRecord {
    std::uint16_t used;
    std::uint16_t field;
    std::uint16_t target;
    std::array<char, 25> name;
    std::array<char, 97> description;
    std::int32_t techniqueId;
    std::int32_t fp;
    std::int32_t sequence;
};

static_assert(std::is_standard_layout_v<PetTechniqueRecord>);
static_assert(std::is_trivially_copyable_v<PetTechniqueRecord>);
static_assert(offsetof(PetTechniqueRecord, used) == 0);
static_assert(offsetof(PetTechniqueRecord, field) == 2);
static_assert(offsetof(PetTechniqueRecord, target) == 4);
static_assert(offsetof(PetTechniqueRecord, name) == 6);
static_assert(offsetof(PetTechniqueRecord, description) == 0x1F);
static_assert(offsetof(PetTechniqueRecord, techniqueId) == 0x80);
static_assert(offsetof(PetTechniqueRecord, fp) == 0x84);
static_assert(offsetof(PetTechniqueRecord, sequence) == 0x88);
static_assert(sizeof(PetTechniqueRecord) == 0x8C);

// 原F633F8五条宠物数据，不是FF02E0地图槽；未消费间隔不计最终类型冻结。
struct PetRuntimeRecord {
    std::uint16_t used; // 原+0占用。
    std::uint16_t value02; // 主回复掩码200001。
    std::uint16_t value04; // 主回复掩码5。
    std::array<std::byte, 2> bytes06;
    std::int32_t value08; // 主回复掩码201。
    std::array<platform::LegacyCryptInt, 6> values0C; // HP/最大HP/FP/最大FP/+4C/+5C。
    std::int32_t lvFlag6C; // 原F63464；50CF70写入完整W32。
    std::array<std::int32_t, 26> values70; // 原+70..+D4，两回复共享。
    std::array<PetTechniqueRecord, 10> techniquesD8;
    std::array<SkillPairRecord, 10> techniquePairs650; // 5185C0只生成索引，不排序。
    std::int32_t imageNo6A0;
    std::int32_t value6A4;
    std::array<std::byte, 2> bytes6A8;
    std::uint16_t flags6AA; // 不含跟随bit0x10的状态。
    std::array<char, 17> name6AC;
    std::array<char, 17> name6BD;
    std::array<std::byte, 2> bytes6CE;
    std::int32_t value6D0; // 518580复制到索引对。
    std::int32_t follow6D4;
    std::array<std::int32_t, 3> values6D8;
    std::array<PlayerItemRecord, 6> items6E4; // 原51B630复用的六条C5C物品。
    std::uint8_t updated510C; // 仅50A6B0尾部置1。
    std::array<std::byte, 3> bytes510D;
};

static_assert(std::is_standard_layout_v<PetRuntimeRecord>);
static_assert(std::is_trivially_copyable_v<PetRuntimeRecord>);
static_assert(offsetof(PetRuntimeRecord, used) == 0);
static_assert(offsetof(PetRuntimeRecord, value02) == 2);
static_assert(offsetof(PetRuntimeRecord, value04) == 4);
static_assert(offsetof(PetRuntimeRecord, value08) == 8);
static_assert(offsetof(PetRuntimeRecord, values0C) == 0x0C);
static_assert(offsetof(PetRuntimeRecord, lvFlag6C) == 0x6C);
static_assert(offsetof(PetRuntimeRecord, values70) == 0x70);
static_assert(offsetof(PetRuntimeRecord, techniquesD8) == 0xD8);
static_assert(offsetof(PetRuntimeRecord, techniquePairs650) == 0x650);
static_assert(offsetof(PetRuntimeRecord, imageNo6A0) == 0x6A0);
static_assert(offsetof(PetRuntimeRecord, value6A4) == 0x6A4);
static_assert(offsetof(PetRuntimeRecord, flags6AA) == 0x6AA);
static_assert(offsetof(PetRuntimeRecord, name6AC) == 0x6AC);
static_assert(offsetof(PetRuntimeRecord, name6BD) == 0x6BD);
static_assert(offsetof(PetRuntimeRecord, value6D0) == 0x6D0);
static_assert(offsetof(PetRuntimeRecord, follow6D4) == 0x6D4);
static_assert(offsetof(PetRuntimeRecord, values6D8) == 0x6D8);
static_assert(offsetof(PetRuntimeRecord, items6E4) == 0x6E4);
static_assert(offsetof(PetRuntimeRecord, updated510C) == 0x510C);
static_assert(sizeof(PetRuntimeRecord) == 0x5110);

// 原F7C950的已消费AC字节前缀；510490生产，4C75A0等UI读取，非完整业务布局。
struct ReplyRecordF7C950 {
    std::int32_t used;
    std::array<char, 32> text04;
    std::uint8_t hasText24; // 第2文本不等于单空格；空串同样为1。
    std::array<char, 32> text25;
    std::array<std::byte, 3> bytes45;
    std::int32_t value48;
    std::array<std::int32_t, 24> values4C;
};

static_assert(std::is_standard_layout_v<ReplyRecordF7C950>);
static_assert(std::is_trivially_copyable_v<ReplyRecordF7C950>);
static_assert(offsetof(ReplyRecordF7C950, used) == 0);
static_assert(offsetof(ReplyRecordF7C950, text04) == 4);
static_assert(offsetof(ReplyRecordF7C950, hasText24) == 0x24);
static_assert(offsetof(ReplyRecordF7C950, text25) == 0x25);
static_assert(offsetof(ReplyRecordF7C950, bytes45) == 0x45);
static_assert(offsetof(ReplyRecordF7C950, value48) == 0x48);
static_assert(offsetof(ReplyRecordF7C950, values4C) == 0x4C);
static_assert(sizeof(ReplyRecordF7C950) == 0xAC);
extern ReplyRecordF7C950 gReplyRecordF7C950;

// 原DE0B08，每项8B；索引是数值，不是地址。
struct PetPairRecord {
    std::uint16_t used;
    std::uint16_t index;
    std::int32_t value;
};

static_assert(offsetof(PetPairRecord, used) == 0);
static_assert(offsetof(PetPairRecord, index) == 2);
static_assert(offsetof(PetPairRecord, value) == 4);
static_assert(sizeof(PetPairRecord) == 8);
extern std::array<PetRuntimeRecord, 5> gPetRuntimeF633F8;
extern std::array<PetPairRecord, 5> gPetPairsDE0B08;
extern std::int32_t gPetCountDE0B00;
std::int32_t Sub_52E180(
    std::int32_t index, std::int32_t techniqueId); // 原十个真实技能记录，非排序索引表。
extern std::uint32_t gValueD67B28; // 492250置1，UI后续复用。

// 原510150：宠物物品带槽号回复；level重复解析图号字段，不同步UI。
void Sub_510150(
    std::uintptr_t unusedSocket, std::int32_t petIndex, const char* payload);
// 原519770：同源宠物物品32行描述与前行回扫，保持独立页数写序。
void Sub_519770(std::int32_t petIndex, std::int32_t itemIndex, const char* text);

void Sub_50A6B0(std::uintptr_t socket, std::int32_t index, const char* payload);
void Sub_50AC20(std::uintptr_t socket, std::int32_t index, const char* payload);
void Sub_50B0D0(
    std::uintptr_t unusedSocket, std::int32_t index, const char* payload); // 技能回复，无CryptInt门禁。
void Sub_5185C0(
    const PetTechniqueRecord* source, SkillPairRecord* destination,
    std::int32_t count); // 保留全部used和sequence，按源序写低W16索引，不排序。
std::int32_t Sub_50CF70(
    std::uintptr_t socket, std::int32_t index, std::int32_t value); // 仅写宠物+6C并返回index。
std::int32_t Sub_519920(); // 先清计数，再扫描占用并发布。
std::int32_t Sub_43C4B0(); // 首个used严格1且flags6AA严格2的宠物槽，未命中-1。
void Sub_518580(
    const PetRuntimeRecord* source, PetPairRecord* destination, std::int32_t count);
void Sub_492250(); // 原[492250,49225B)，置刷新标志。

// 原 FF0508/DE03F8 两张称号表共用记录；地址不存入数值字段。
struct TitleRecord {
    std::int32_t id; // 原 +0；负值表示无称号。
    std::int32_t value; // 原 +4；50A300第三字段。
    std::array<char, 20> text; // 原 +8；原编码字节，不增加复制限长。
};

static_assert(std::is_standard_layout_v<TitleRecord>);
static_assert(std::is_trivially_copyable_v<TitleRecord>);
static_assert(offsetof(TitleRecord, id) == 0);
static_assert(offsetof(TitleRecord, value) == 4);
static_assert(offsetof(TitleRecord, text) == 8);
static_assert(sizeof(TitleRecord) == 0x1C);
extern std::array<TitleRecord, 64> gTitlesFF0508;
extern std::array<TitleRecord, 64> gSortedTitlesDE03F8;

void Sub_50A300(std::uintptr_t socket, const char* payload); // 原称号回复；socket未用。
std::int32_t Sub_519840(std::int32_t id); // 原表首匹配索引，未找到返回-1。
std::int32_t Sub_519870(); // 非负编号数量/8+1。
void Sub_5198D0(TitleRecord* records, std::uint32_t count); // 原CRT次序的称号排序。

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
void Sub_476680();                         // 原 [476680,476739)：玩家范围覆盖入队及显示倒计时。
void Sub_44A410();                         // 原 0x44A410，输入光标闪烁/排队。
void Sub_4626A0();                         // 原 0x4626A0。
void __cdecl Sub_449DA0();                 // 原 [0x449DA0,0x449F9C)。
std::int32_t __cdecl Sub_46A9E0();         // 原 [0x46A9E0,0x46A9F4)，清三槽、返回 0。
std::int32_t __cdecl Sub_46AA00();         // 原 [0x46AA00,0x46AB56)，方向分类并写 pending。
void __cdecl Sub_46BC90();                // 原 [0x46BC90,0x46BCE4)，三组路径 W16 同步左移。
void __cdecl Sub_46ABD0();                // 原 [0x46ABD0,0x46AC0A)，最多搬运两项方向。
std::uint16_t Sub_46AC10(); // 方向文本生成/发送，仅承接原AX返回位型。
void Sub_46B370(); // 目标格转向，保留重复CryptInt读取与独立500ms节流。
void Sub_46B4B0(std::int32_t direction); // 原完整转向门禁、发布与节流发送。
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
void __cdecl Sub_458F40(); // 原 [0x458F40,0x458FC2)，同源玩家时钟推进，场景9要求玩家T*。
void __cdecl Sub_458F00(); // 原 [0x458F00,0x458F3A)，类别变化后可选调色板过渡并进入 458F40。
std::int32_t __cdecl Sub_519EB0(std::int32_t value); // 原 [0x519EB0,0x519EBA)，真实 FDA880 写回依赖。
std::int32_t __cdecl Sub_50C870(
    std::uintptr_t socket, std::int32_t playerValue,
    std::int32_t timeOffset, std::int32_t pathValue,
    std::int32_t adjustment); // 原 [0x50C870,0x50C8C8)，时间与地图配置回复。
std::int32_t __cdecl Sub_469030(); // 原 [0x469030,0x4691C9)，地图装载、第四平面与斜向绘制。
extern std::uint32_t gValue55DA88; // 原地图窗口重建标志，初值 1；46C760/46CF60 共用。
// 原 46C840：三平面窗口读取及两类地图颜色生产；不改变玩家坐标。
std::int32_t Sub_46C840(std::int32_t floor, std::int32_t playerX, std::int32_t playerY);
// 原 46C760：按地图模式和重建标志提交同一窗口，返回完整绘制结果。
std::int32_t Sub_46C760(std::int32_t x, std::int32_t y);
// 原 [46CF60,46D9B9)：完整两路地图绘制；参数只控制普通地图标记，重建场景10实传0。
void __cdecl Sub_46CF60(std::int32_t value);
// 原469020仅传标记参数1；43CB10完成战斗背景快照并更新5ABCD8。
void Sub_469020();
void Sub_43CB10();
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
std::int32_t Sub_46B6F0(); // 完整候选/绕行/轴补走路径生成，复用三组W16路径。
std::uint32_t __cdecl Sub_46BD20(
    std::int32_t value, std::int32_t mode); // 原 [0x46BD20,0x46BD32)，仅改 AL，保留 mode 高 24 位。
std::int32_t __cdecl Sub_50C8D0(
    std::int32_t x, std::int32_t y,
    const char* text); // 原 [0x50C8D0,0x50C8F5)，透传 zA 请求返回。
struct RecipeItemData;
RecipeItemData* Sub_5199A0();              // 原 0x5199A0。
std::int32_t Sub_444FA0(std::int32_t objectIndex); // 原编号查槽，命中更新查找游标。
login::YobiAction* FindObjectYobi_4462B0(std::int32_t objectIndex); // 复用原查槽后借用action。
void Sub_50AEF0(std::uintptr_t unusedSocket, const char* payload); // 最多五组对象HP/FP，无全局门禁。
void Sub_50B380(std::uintptr_t socket, const char* payload); // 完整TAB对象批量回复，socket未用。
void Sub_50BCD0(std::uintptr_t socket, const char* payload); // 逗号动作批量回复，先观察拦截。
void Sub_50BF20(std::uintptr_t unusedSocket, const char* payload); // 删除编号后清首个匹配地图槽引用。
std::uint32_t Sub_50BFA0(std::uintptr_t socket, std::int32_t value); // 玩家选项回复，清bit2后写U16。
std::uint16_t Sub_50BFC0(
    std::uintptr_t socket, std::int32_t value); // 玩家+1A0低W16写入；原返回仅定义AX。
std::int32_t Sub_50C940(
    const input::InputContext* context, std::int32_t color); // 聊天时间/选项门禁后转义发送。
std::uint32_t Sub_50C7A0(
    std::uintptr_t socket, std::int32_t seconds, std::int32_t mode); // 玩家时钟回复并清626C14。
std::int32_t Sub_50CE10(std::uintptr_t socket, const char* text); // 系统提示及可选内嵌音效。
void Sub_50E1A0(
    std::uintptr_t socket, std::int32_t mask, std::int32_t value,
    const char* unusedText); // 三效果半字写入后调用既有46E0D0。
std::int32_t Sub_50E210(std::uintptr_t socket, std::int32_t value); // 九种指定音效，其他值透传。
void Sub_50E650(
    std::uintptr_t socket, std::int32_t mode, std::int32_t duration,
    std::int32_t floor, std::int32_t mapX, std::int32_t mapY); // PVfz原序转449700，socket未用。
void Sub_50CAE0(
    std::uintptr_t socket, std::int32_t objectIndex, const char* payload,
    std::int32_t color, std::int32_t font); // 聊天回复：原名字过滤、日志与头顶生产。
void MarkPlayerChatActive() noexcept; // 51A6A0在固定载荷前置flags18A的2000。
void Sub_43D860(login::YobiAction* action); // 指定地图模式下，类别1对象面向玩家。
void Sub_442DC0(
    login::YobiAction* action, std::int32_t duration, const char* text,
    std::int32_t color, std::int32_t font, std::int32_t length);
void Sub_51A6A0(
    std::int32_t duration, const char* text, std::int32_t color,
    std::int32_t font, std::int32_t length); // 同源玩家头顶文本生产。
std::int32_t Sub_472F40(); // 同源玩家选项切换04后发送。
std::int32_t Sub_472F70(); // 切换08后发送。
std::int32_t Sub_472FA0(); // 切换01后发送。
std::int32_t Sub_472FD0(); // 切换10后发送。
std::int32_t Sub_473000(); // 切换20后发送。
std::int32_t Sub_473030(); // 切换40后发送。
void Sub_51A4B0(); // 玩家初始姓名/称号/等级/HP同步。
void Sub_51A5E0(); // 玩家flags18A置200。
void Sub_449C90(login::YobiAction* action); // 非跟随对象置flags42的100。
void Sub_449CC0(login::YobiAction* action); // marker44为0才清flags42的100。
void Sub_442C40(login::YobiAction* action); // flags42置200。
void Sub_442C70(login::YobiAction* action); // 先场景门禁，再通过payload清flags42的200。
void Sub_442BD0(login::YobiAction* action); // flags42置100，再marker44=1。
void Sub_442C00(login::YobiAction* action); // 清marker44，非模式2才清flags42的100。
void Sub_442CA0(login::YobiAction* action, std::int32_t value); // 置4000，再写value10C。
void Sub_442CD0(login::YobiAction* action); // 清4000，保留value10C。
void Sub_442D00(login::YobiAction* action, std::int32_t value); // 置8000，再写value10C。
void Sub_442D30(login::YobiAction* action); // 清8000。
void Sub_442D60(login::YobiAction* action, std::int32_t value); // 置0400，再写value10C。
void Sub_442D90(login::YobiAction* action); // AND EBFF，同时清0400/1000。
void Sub_43D970(login::YobiAction* action); // flags120置1。
void Sub_43D9A0(login::YobiAction* action); // flags120清1。
void Sub_43D9D0(login::YobiAction* action, std::int32_t value); // 原范围回退后置2/写value124。
void Sub_43DA10(login::YobiAction* action); // flags120清2，再清value124。
void Sub_43DA40(login::YobiAction* action); // flags120置24。
void Sub_43DA70(login::YobiAction* action); // flags120置8。
void Sub_43DAA0(login::YobiAction* action); // flags120清8。
void Sub_43DAD0(login::YobiAction* action); // flags120置50。
void Sub_43DB00(login::YobiAction* action); // 观察处理只置flags120的80。
void Sub_43DB30(login::YobiAction* action); // 观察子对象结束时只清flags120的80。
void Sub_444410(
    login::YobiAction* action, std::int32_t x, std::int32_t y,
    std::int32_t direction, std::int32_t characterAction, std::int32_t option1,
    std::int32_t option2, std::int32_t option3, std::int32_t option4);
void ResetMapFollowersFromPath(login::YobiAction* leader); // 444410仅扫描占用的地图槽1..4。
void Sub_50A3E0(std::uintptr_t socket, std::int32_t index, const char* payload);
void Sub_50D000(
    std::uintptr_t socket, std::int32_t first, std::int32_t second); // 跟随状态回复。
void Sub_5093C0(
    std::uintptr_t socket, std::int32_t x, std::int32_t y,
    std::int32_t direction); // 原坐标回复，无门禁，方向最后发布。
void Sub_50D080(
    std::uintptr_t socket, std::int32_t first, std::int32_t second); // 两次CryptInt赋值后复制首值。
void Sub_50D120(
    std::uintptr_t socket, std::int32_t type, std::int32_t imageNo); // 完整战斗进入回复，socket未用。
void Sub_50D3C0(std::uintptr_t socket, const char* text); // C/P/A/第二字节U/M完整分派。
void Sub_50D500(std::uintptr_t socket, const char* payload); // 三阶段战斗结果生产，socket未用。
void Sub_455C60(); // 仅清既有62B5B0，不关闭窗口。
void Sub_455FC0(); // 窗口39创建/动画清理父入口，保留额外SE56。
void Sub_51A5F0(); // 仅清玩家flags18A的200。
login::YobiAction* Sub_51B080(); // 解散地图五槽；返回51A5B0的非拥有T*。
void Sub_51B0F0(); // 仅清地图五槽的非拥有action，不释放或清占用。
void Sub_46AFE0(
    std::int32_t index, std::int32_t x, std::int32_t y); // 移除后的五槽路径续接。
std::int32_t Sub_446130(); // 原空槽领取，返回旧游标；不在此处标记占用。
std::uint32_t Sub_4461E0(std::int32_t type); // 原类型转flags114，返回完整W32。
std::uint32_t Sub_446210(std::int32_t type); // 原71项跳表转换，写category时才截低W16。
std::int32_t Sub_5183B0(std::int32_t imageNo); // 原图号[100000,200000)判断，不读取资源表。
std::int32_t Sub_440B50(); // 原当前场景U32闭区间250..252门禁。
void Sub_4468F0(); // 原普通场景51D246：只清三值去重计数，不清表。
std::int32_t Sub_446900(
    std::int32_t graphicIndex, std::int32_t mapX, std::int32_t mapY);
// 原实体生产/更新；方向归零门槛仅新建生效，记录和action文本分两轮处理。
void Sub_445350(
    std::int32_t objectIndex, std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction, const char* name,
    const char* titles, const char* extraText, std::int32_t level,
    std::int32_t value40, std::int32_t flag1, std::int32_t flag2,
    std::int32_t type);
// 原只生产无action记录；编号已存在或领取失败返回0，完成记录写入返回1。
std::int32_t Sub_445A60(
    std::int32_t objectIndex, std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction, const char* name,
    const char* titles, std::int32_t level, std::uint16_t value40,
    std::int32_t flag1, std::int32_t flag2, std::int32_t type);
// 原类别2/4对象生产或更新；已有记录不补建action，创建失败不回滚领取。
void Sub_445D20(
    std::int32_t objectIndex, std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction, std::int32_t value48,
    const char* name, std::int32_t value50);
void Sub_445F40(
    std::int32_t objectIndex, std::int32_t imageNo, std::int32_t mapX,
    std::int32_t mapY, std::int32_t direction, std::int32_t value4C,
    const char* name);
void Sub_4462E0(std::int32_t objectIndex); // 原按编号删除；两条调用链均不消费返回。
void Sub_4467D0(std::int32_t objectIndex); // 原动作结束后恢复仍占用且action为空的记录。
std::int32_t Sub_445030(
    std::int32_t x, std::int32_t y, std::uint16_t category, std::uint16_t flags);
std::int32_t Sub_4450D0(
    std::int32_t x, std::int32_t y, std::uint16_t category); // 返回完整0/1。
std::int32_t Sub_445160(
    std::int32_t x, std::int32_t y, std::uint16_t category); // 命中返回对象编号，否则-1。
void Sub_446470(); // 原只清引用的全表重置；调用者均弃用原表外返回地址。
void Sub_446610(); // 全表仅对占用项清action、写value34=-1。
void* __cdecl Sub_46ADA0();               // 原 [0x46ADA0,0x46AE53)，仅透传 51D5B0 的返回位型。
std::int32_t __cdecl Sub_46AC90();        // 原 [0x46AC90,0x46AD00)，type 2 的真实后继。
std::int32_t __cdecl Sub_46AEA0();        // 原 [0x46AEA0,0x46AF24)，type 3 的真实后继。
std::int32_t __cdecl Sub_46AB60();        // 原 [0x46AB60,0x46ABCD)，pending 倒计时与分发。
void Sub_4D3170();                         // 原 [0x4D3170,0x4D317B)，重置 0x581800 为 -1。
std::int32_t Sub_4410E0();                 // 原 [0x4410E0,0x4410E6)，无参返回 17。
std::int32_t Sub_455BD0();                 // 原 [0x455BD0,0x455C56)，顺序重置 23 槽并返回 0。
struct MapEffectRecord;
// render 只借用活动链及已证数值字段；不公开记录布局，不截断原生 next。
const MapEffectRecord* BorrowActiveMapEffectHead() noexcept;
const MapEffectRecord* BorrowNextMapEffect(const MapEffectRecord* record) noexcept;
std::uint32_t ReadMapEffectWord(const MapEffectRecord& record, std::size_t offset);
std::uint16_t ReadMapEffectHalf(const MapEffectRecord& record, std::size_t offset);
MapEffectRecord* Sub_46E2A0();             // 原 0x46E2A0；返回末项非拥有指针。
MapEffectRecord* Sub_46E340();             // 原 0x46E340；空闲首项转为活动首项。
MapEffectRecord* Sub_46E390(MapEffectRecord* record); // 原 0x46E390；摘活动链后退回空闲头。
std::uintptr_t __cdecl Sub_46E3E0(std::int32_t* x, std::int32_t* y); // 原 0x46E3E0；混合 EAX 位型。
void Sub_46E480();                          // 原 0x46E480；非零提交标志才绘制，调用后无条件清 W16。
void Sub_46E4E0();                          // 原 0x46E4E0；EAX 不具统一返回合同。
void Sub_46E6C0();                          // 原 0x46E6C0；活动链 kind 2，EAX 无统一合同。
std::int32_t __cdecl Sub_46E940(std::int32_t cycles); // 原 0x46E940；多轮 kind 2 更新。
std::int32_t __cdecl Sub_46EBB0(std::int32_t cycles); // 原 0x46EBB0；多轮 kind 1 更新。
std::int32_t __cdecl Sub_46E4C0(std::int32_t cycles); // 原 0x46E4C0；双效果更新，透传后者返回。
void Sub_46ED50();                          // 原 0x46ED50；kind 4 创建与活动链更新，返回无统一合同。
void Sub_46E4A0();                          // 原 0x46E4A0；三效果生产后尾接 46F1D0。
void Sub_46F1D0();                          // 原 0x46F1D0；kind 6/7/8 生成、推进、入队与回收。
std::int32_t Sub_440FF0();                  // 原 0x440FF0；观察效果更新后铺排 100 个背景项。
void __cdecl Sub_46E0D0(std::int32_t mask, std::int32_t value); // 原 0x46E0D0；场景效果配置。
void Sub_46F160();                          // 原 0x46F160；清活动链的 kind 6/7/8。
MapEffectRecord* Sub_46F090(
    std::int16_t kind, std::int32_t horizontalSign,
    std::int32_t verticalSign, std::int32_t phase,
    std::int32_t speed, std::int32_t graphicIndex); // 原 0x46F090；效果记录生产。
std::int32_t __cdecl Sub_46F000(std::int32_t* x, std::int32_t* y); // 原 0x46F000；投影偏移增量。
std::int32_t Sub_46BD40();                 // 原 [0x46BD40,0x46BD57)，依次清四个独立 W32 槽并返回 0。
std::int32_t Sub_44A090();                 // 原 [0x44A090,0x44A15B)，读取或创建 chathis.dat。
std::int32_t Sub_449FA0(std::int32_t index); // 原 [0x449FA0,0x44A084)，写入一项历史和尾索引。
std::int32_t Sub_504A30();                 // 原 [0x504A30,0x504A43)，调用 44A090、清两槽并返回 0。
std::int32_t Sub_43C8F0();                 // 原 [0x43C8F0,0x43C90A)，四槽顺序写入并返回 -1。
std::int32_t Sub_504100();                 // 原 [0x504100,0x504113)，顺序清三槽、返回 0。
void Sub_5186A0(); // 原相册保存：codec容量4，短写保留文件句柄。
void Sub_518780(); // 原相册加载：读取4字节，长度不符调用5186A0。
void Sub_518BE0(); // 原所选角色相册组清理及保存。
void Sub_518870(
    std::int32_t number, std::int32_t imageNo,
    std::int32_t mark1, std::int32_t mark2, std::int32_t tribe,
    std::int32_t value0C, std::int32_t value10, std::int32_t value14,
    std::int32_t value18, std::int32_t value1C, std::int32_t value20,
    std::int32_t value24, std::int32_t value28, std::int32_t value2C,
    std::int32_t value30, char* text, const char* name); // 同源相册记录逐字段更新。
void Sub_518A40(std::int32_t version, const char* payload); // 位掩码同步，先请求后发布版本。
void Sub_50E5B0(
    std::uintptr_t socket, std::int32_t version, const char* payload); // 相册同步回复，socket未用。
void Sub_50E5D0(
    std::uintptr_t socket, std::int32_t number, std::int32_t imageNo,
    std::int32_t mark1, std::int32_t mark2, std::int32_t tribe,
    std::int32_t value0C, std::int32_t value10, std::int32_t value14,
    std::int32_t value18, std::int32_t value1C, std::int32_t value20,
    std::int32_t value24, std::int32_t value28, std::int32_t value2C,
    std::int32_t value30, char* text, const char* name); // 相册记录回复，socket未用。
using cg::audio::Sub_52B990;               // 原 0x52B990。
std::int32_t Sub_45EBB0();                 // 原 0x45EBB0。
void Sub_45ED80();                        // 原 [0x45ED80,0x45ED8B)，重置 9A6204。
extern std::uint32_t gValue9A6204;         // 原 0x9A6204；45ED90 后续复用。
std::int32_t Sub_45ED90();                 // 原 [0x45ED90,0x45EEDA)，场景 7 请求与展示。
void DrawNormalScene_51C7D0();             // 原 World::DrawNormalScene@0x51C7D0。
void Sub_519A80(); // 原普通场景玩家创建/身份发布、HP复制与宠物地图槽匹配。
// 原 46FB80：先X差再Y差，返回Y平方+X平方的F64值。
double Sub_46FB80(float x, float y, float targetX, float targetY);
login::YobiAction* Sub_51A090(
    const char* name, const char* titles, std::int32_t level,
    std::int32_t value188, std::int32_t flag40, std::int32_t flag80);
// 519A80 公共尾清理；后续协议/场景必须复用这些独立 W16 槽。
extern std::uint16_t gValueFF0D30;
extern std::uint16_t gValueFF0D32;
extern std::uint16_t gValueF7CA04;
extern std::uint16_t gValueFF03D8;
extern std::uint16_t gValueFF0D36; // 原跟随gate；协议和场景共享，不默认启用。
extern std::uint32_t gValueFF0D38; // 原51B3E0邻格请求的独立时间槽。
std::uint32_t Sub_51B3E0(); // 完整邻格fIM请求；返回原EAX位型。
std::uint8_t Sub_469C90(); // 完整移动输入、目标交互与普通/跟随分流。
std::uint8_t Sub_46F680(); // 完整鼠标目标格投影/绘制与四种事件生产。
void Sub_46A990(); // 从同一宠物五槽生成9FB938顺序表，gate为0不清表。
void Sub_46A5A0(std::int32_t x, std::int32_t y); // 给顺序表第2项调用441F60。
void Sub_46A940(); // 从顺序表第2项起调用442730，首个负S16终止。
void Sub_46A5F0(); // 原完整跟随路径调度；到达后启动首路径并向后继传点。
std::uint16_t Sub_469FC0(); // 普通玩家完整移动调度，承接原AX返回。
std::uint32_t Sub_46A190(); // 完整两轮跟随调度，返回公共尾先读取的世界X位型。
void Sub_46A410(
    std::int32_t x, std::int32_t y, std::int32_t speed); // 玩家快照/共享移动分量初始化。
void Sub_46A700(); // 原完整玩家移动推进、到达计数及MIS动作请求。
// YobiRuntime借用同一SceneState状态，不公开玩家首块或五槽记录布局。
std::uint32_t ReadPlayerFlags18A() noexcept; // 原FDA88A跨W32读取。
std::uint16_t ReadPlayerActionFlags() noexcept; // 原FDA88E，与flags18C高W16同源。
std::int32_t ReadPlayerActionValue190() noexcept;
void ClearPlayerActionFlags(std::uint16_t mask) noexcept; // 只清FDA88E，不改FDA88C低W16。
void SetPlayerActionFlags(std::uint16_t mask) noexcept; // 同源FDA88E置位，保留其他字节。
char* BorrowPlayerStallName() noexcept; // 原FF02B8的text15BB8，可写借用，不复制owner。
void CopyPlayerBarValues(
    platform::LegacyCryptInt* hp, platform::LegacyCryptInt* maxHp,
    platform::LegacyCryptInt* fp, platform::LegacyCryptInt* maxFp);
const char* BorrowPlayerDisplayText(std::int16_t mode) noexcept; // 模式1..6原文本，不复制。
extern std::uint32_t gActorBarsEnabled5AC490; // 原虚拟零区，442F70与后续设置UI共享。
std::int32_t ReadPlayerWalkSpeed() noexcept; // 原FDA870。
std::int32_t ReadPlayerImageNo() noexcept; // 原FDA878；不是action当前变换图号。
std::int32_t ReadPlayerValue17C() noexcept; // 原FDA87C，窗口56外观分解读取，与FDA878独立。
std::int32_t ReadPlayerValueFC() noexcept; // 原FDA7FC，交易面板借用同源金额，不复制owner。
std::int32_t ReadPlayerValue16C() noexcept; // 原FDA86C，窗口54借读509650生产的总技能槽数。
std::int32_t ReadPlayerValue15BCC() noexcept; // 原FF02CC，库存第二数值文本借读同源回复字段。
std::int32_t ReadPlayerLevel() noexcept; // 原FDA704，存放宠物费用读取同源等级。
std::int32_t ReadPlayerValue08() noexcept; // 原FDA708，技能时长倍率索引，借读既有槽。
std::int32_t ReadPlayerTimerRemaining0C() noexcept; // 原FDA70C，转生按钮读取同源计时，不改owner。
void ConsumeBattlePlayerTimer(std::int32_t seconds); // 原43BBCE..43BBF2，先扣毫秒再按S32负值清零。
std::int32_t ReadPlayerInjury() noexcept; // 原FDA7F4，属性选择列表使用同一伤势。
std::int32_t ReadPlayerHpOnce(); // 原FDA718，仅执行一次CryptInt读取，不复制载荷。
std::int32_t ReadPlayerMaxHpOnce(); // 原FDA728。
std::int32_t ReadPlayerFpOnce(); // 原FDA738。
std::int32_t ReadPlayerMaxFpOnce(); // 原FDA748。
bool IsPlayerFpLessThan(std::int32_t cost); // 原FDA738，499C30各次401DB0比较共用同一CryptInt。
std::int32_t ReadPlayerValue15BD0(std::int32_t index) noexcept; // 原FF02D0/D4/D8三项金额上限。
std::int16_t ReadPetFollowIndex(std::size_t position) noexcept;
login::YobiAction* ReadPetMapAction(std::int16_t index) noexcept;
std::uint16_t ReadPetMapFlags(std::int16_t index) noexcept; // 原FF02E0五槽W16占用，名单按完整非零判断。
std::uint32_t ReadPetMapObjectIndex(std::int16_t index) noexcept;
// 519EE0 每个原读取点借用唯一 FF02B4，不暴露可写槽或对象布局。
login::YobiAction* ReadPlayerAction() noexcept;
void Sub_519CD0(
    std::int32_t imageNo, std::int32_t x, std::int32_t y,
    std::int32_t direction); // 原 519CD0：创建并发布玩家，或更新已有玩家。
void Sub_519E50(std::int32_t imageNo, std::int32_t direction);
std::uint32_t Sub_519EC0(std::int32_t x, std::int32_t y); // 透传468330。
void Sub_519EE0(); // 原入口不读取参数；检查当前坐标并同步 action 载荷。
std::int32_t Sub_51A040(std::int32_t direction); // 同源方向发布，返回完整输入。
void Sub_51A060(std::int32_t characterAction); // 动作1不重启，其余动作请求重启。
void Sub_51A840(
    std::int32_t x, std::int32_t y, std::int32_t direction,
    std::int32_t characterAction, std::int32_t option1, std::int32_t option2,
    std::int32_t option3, std::int32_t option4); // 完整玩家动作回复分支。
void Sub_51A600(); // flags18A置0x4000。
void Sub_51A610(); // flags18A清0x4000。
void Sub_51A620(); // flags18A置0x8000。
void Sub_51A630(); // flags18A清0x8000。
void Sub_51BDB0(); // FDA88E置1，复用flags18C高W16。
void Sub_51BDC0(); // FDA88E清1。
void Sub_51BE10(); // FDA88E置0x24。
void Sub_51BE20(); // FDA88E置8。
void Sub_51BE30(); // FDA88E清8。
void Sub_51BE40(); // FDA88E置0x50。
void Sub_51BEE0(); // 观察处理置FDA88E的0x80，复用flags18C高W16。
login::YobiAction* Sub_51A640(); // 原+272置1，返回入口借用指针。
login::YobiAction* Sub_51A660(); // 原+272清0，返回入口借用指针。
std::int32_t Sub_51A680(); // 空玩家为0，否则MOVSX读取原+272，非指针返回。
std::int32_t Sub_51C0D0(std::int32_t imageNo); // 原骑乘图号映射，未命中原值返回。
std::int8_t Sub_519D50();                  // 原 0x519D50：附件清理、重读玩家标死与动作尾链，返回低 S8。
// 519D50 尾链及 51A840 状态生产共用的玩家位槽，不复制 FDA88E 动作标志。
void Sub_51A5A0(); // 原 51A5A0：先置 FDA88A bit8，再只写 FDA88C 低 W8。
login::YobiAction* Sub_51A5B0(); // 原 51A5B0：条件清标志并返回入口借用指针。
std::int32_t Sub_51BDD0(std::int32_t value); // 原 51BDD0：发布动作值，透传输入。
void Sub_51BDF0(); // 原 51BDF0：清动作 bit1 后清动作值，不发送。
std::int32_t Sub_51BEC0(); // 原 51BEC0：按动作 bit1 返回同一动作值或0。
std::int8_t Sub_51BE50(std::int32_t requested); // 原 51BE50：去重、klF 发送及低 S8 返回。
void* __cdecl Sub_51D5B0();               // 原 [0x51D5B0,0x51D63A)；非空链尾返指针，空链 EAX 未定义；46ADA0 透传位型。
void __cdecl Sub_4464D0();                // 原 [0x4464D0,0x446604)，场景对象/Yobi 清理。
std::int8_t Sub_51D750();                  // 原 0x51D750，IDA 原型 char()。
// 原 [51D640,51D6E3)：场景地图画入 Back 后复制到 Temp，返回同次 tick。
std::uint32_t Sub_51D640();
// 原 59F9F8，战斗背景 W32 图号；重建与战斗绘制/协议生产共用，非地址。
extern std::int32_t gBattleBackgroundImage59F9F8;
extern std::int32_t gBattleType5ABFC8; // 50D120写完整S32类型。
extern std::uint32_t gBattleModeFlag5ABFCC; // 仅类型2写1。
extern std::uint32_t gValue5ABB1C;
extern std::uint32_t gValue5ABB28;
extern std::uint32_t gBattleFirstTurnPending5ABC6C; // 50D3C0的A消息严格1时消费。
extern std::int32_t gBattleCWriteIndex5AAA6C;
extern std::int32_t gBattleCReadIndex5AAA44;
extern std::int32_t gBattleMWriteIndex5A3A1C;
extern std::int32_t gBattleMReadIndex5AAA68;
extern std::array<char, 0x4000> gBattleCQueue5A4A3C; // 四槽，每槽1000H；50D120只清各首字节。
extern std::array<char, 0x4000> gBattleMQueue59FA18;
extern std::array<char, 0x1000> gBattleCText5A3A20; // 场景从C队列复制的活动原字节文本。
extern std::array<char, 0x1000> gBattleMText5AAA7C;
extern std::int32_t gBattleMScanOffset5A8A3C; // M头扫描S32游标，非队列索引或对象载荷64游标。
extern login::YobiAction* gBattleSceneAction5ABA94; // 4383F0发布的非拥有场景对象，完整生产仍缺。
extern std::array<login::YobiAction*, 20> gBattleEntries59E980; // 原20槽非拥有对象表。
extern std::int32_t gBattleCurrentPosition5ABCA4; // 实际gMyPostion；不同于P回复的5ABCA8。
extern std::uint32_t gBattlePosition5ABCA8;
extern std::int32_t gBattlePlayerActionState5ABC74; // UI发布动作完成状态，非5ABB74回合数。
extern std::array<std::int32_t, 9> gBattleSelectionModes5ABC7C; // 496F10清九个W32，含94/98/9C。
extern std::int32_t gBattleSelectedSkill5ABB5C; // 4974C0写技能槽，4975A0/498520重取。
extern std::int32_t gBattleSelectedTechnique5ABC78; // 4974C0写技术槽，不是UI列表索引。
extern std::int32_t gBattleCommandState5ABC3C; // 技能/宠物命令完成状态，498520消费。
extern std::uint8_t gBattleRebirthDisabled5ABC40; // 497C80先读的独立U8门禁，498520后续写。
extern std::int32_t gBattleSkillActionCount5ABBF4;
extern std::int32_t gBattlePetActionCount5ABBBC;
extern std::int32_t gBattleCountdownActive5ABCB8; // 43C4E0置1，4982C0到期清零。
extern std::uint32_t gBattleCommandDeadline5ABBF8; // 原32位tick截止值，不是地址。
extern std::int32_t gBattleCountdownSecond5ABB80; // 低于十秒时的独立提示秒数。
extern std::int32_t gBattleSnapshotPetIndex5ABBFC; // 4965D0每次解码后重取，非553F90。
extern std::int32_t gBattleCommandX553F78; // 映像490，43C4E0覆盖493。
extern std::int32_t gBattleCommandY553F7C; // 映像-104，43C4E0覆盖-106。
extern std::int32_t gBattleCommandValue553F80; // 映像及初始化17，43C3E0读取。
extern std::int32_t gBattlePetSkillGraphicIndex553F8C; // 原绘制返回索引，初值-1，不是对象地址。
extern std::array<std::int32_t, 9> gBattleResetValues5ABB84; // 43C490只写-2的独立九槽，非选择标志。
extern login::YobiAction* gBattleEntryNoticeAction5ABFB8; // 43C950返回的非拥有窗口动画，49BD40及场景清槽。
void Sub_43C4E0(); // 完整命令计时初始化、对象标志与技能选择同步。
extern std::int32_t gBattleSelectionMode5ABC70; // 与九个选择槽分别存储。
extern std::int32_t gBattleSkillUsed5ABC68; // 497000/4975A0读取完整W32。
extern std::int32_t gBattlePetCommandPhase553F88; // 原.data初值1，宠物命令写0/2。
extern std::int32_t gBattleValue5ABCC0; // 两个技能开启入口清零，不猜未消费语义。
extern login::YobiAction* gBattleCommandAction5ABC00; // 原非拥有对象引用，MarkDeath后清槽。
extern std::uint32_t gBattleTargetFlags5ABC44; // 43C560先发布完整目标位模式，43C120后续消费。
extern std::int32_t gBattleSelectedItem5ABC48; // 499C30发布真实物品槽，498520后续消费。
extern std::int32_t gBattleItemTargetPending5ABCC4; // 499C30请求选目标，497100/497770处理。
extern std::int32_t gBattleBombLevelD67B2C; // 仅炸弹hover发布，41D4FE动画读取。
std::int32_t GetBattlePetPosition_405B40(std::int32_t position);
std::int32_t Sub_518C60(); // 玩家背包8..27空槽数，仅检查used==0。
extern std::uint32_t gBattlePlayerFlags5ABCAC;
extern std::uint32_t gBattleSkillFlags5ABC04;
extern std::uint32_t gBattlePetSkillFlags5ABB78;
extern std::uint32_t gBattleRebirthLevel5ABB7C;
extern std::uint32_t gBattleRidePet5ABC38;
extern std::uint32_t gBattleEndBits5ABBC0;
extern std::uint32_t gBattleTurn5ABB74;
extern std::uint32_t gBattleTurnSnapshot5ABC08;
extern std::uint32_t gValue5ABCB0;
extern std::int32_t gBattleBackgroundSnapshot5ABCD8; // 43CB10末尾写，DrawBattleScene比较。
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
