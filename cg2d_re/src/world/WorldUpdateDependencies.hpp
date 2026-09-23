#pragma once

#include "cg/LegacyAudio.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>

namespace cg::input {
struct InputContext;
} // namespace cg::input

namespace cg::platform {

// 原 0x53B8C6 返回九个 S32 槽；调用方按已确认偏移读取时间字段。
struct LegacyTm53B8C6;
LegacyTm53B8C6* __cdecl LocalTime_53B8C6(std::int32_t* timeValue);

} // namespace cg::platform

namespace cg::login::detail {

void Sub_46DD40(); // 原 0x46DD40；复用 YobiRuntime.cpp 的外部定义。

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

// 原 0xC82230/0xC82234/0xC82236；46A9E0 清零，后续 46AA00/46AB60 共用。
extern std::uint32_t gValueC82230;
extern std::int16_t gValueC82234;
extern std::int16_t gValueC82236;

// 原 0xC3500C/0xC75010/0x9EE8A0/0x9ED438；468F30 按序清零的独立 W32 槽。
extern std::uint32_t gValueC3500C;
extern std::uint32_t gValueC75010;
extern std::uint32_t gValue9EE8A0;
extern std::uint32_t gValue9ED438;

// 原 0xC35008/0x9DAD2C/0x9FB930/0xC821D4/0x9A86DC；4682E0 顺序写入。
extern std::uint32_t gValueC35008;
extern std::uint32_t gValue9DAD2C;
extern std::uint32_t gValue9FB930;
extern std::uint32_t gValueC821D4;
extern std::uint32_t gValue9A86DC;

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

// World::Update@0x51C430 的直接目标声明；未实现项只保留 IDA 地址和调用契约。
std::int32_t Sub_45F190();                 // 原 0x45F190。
std::int32_t Sub_464840();                 // 原 0x464840。
void RenderChara_521F20();                 // 原 World::RenderChara@0x521F20。
void RenderInput_4590E0();                 // 原 World::RenderInput@0x4590E0，已完整还原。
void Sub_44A410();                         // 原 0x44A410，输入光标闪烁/排队。
void Sub_4626A0();                         // 原 0x4626A0。
std::int32_t Sub_45E3D0();                 // 原 0x45E3D0。
void __cdecl Sub_449DA0();                 // 原 [0x449DA0,0x449F9C)。
std::int32_t __cdecl Sub_46A9E0();         // 原 [0x46A9E0,0x46A9F4)，清三槽、返回 0。
std::int32_t* Sub_467F30();                // 原 0x467F30。
std::int32_t __cdecl Sub_468F30();         // 原 [0x468F30,0x468F6F)，按序清十二槽、返回 0。
std::int32_t* __cdecl Sub_46CF20();        // 原 [0x46CF20,0x46CF57)，填表并返回一过尾指针。
struct RecipeItemData;
RecipeItemData* Sub_5199A0();              // 原 0x5199A0。
struct YobiAction;
YobiAction** Sub_446470();                 // 原 0x446470。
void Sub_4D3170();                         // 原 [0x4D3170,0x4D317B)，重置 0x581800 为 -1。
std::int32_t Sub_455BD0();                 // 原 [0x455BD0,0x455C56)，顺序重置 23 槽并返回 0。
std::uint32_t* Sub_46E2A0();               // 原 0x46E2A0。
std::int32_t Sub_46BD40();                 // 原 [0x46BD40,0x46BD57)，依次清四个独立 W32 槽并返回 0。
std::int32_t Sub_44A090();                 // 原 [0x44A090,0x44A15B)，读取或创建 chathis.dat。
std::int32_t Sub_504A30();                 // 原 [0x504A30,0x504A43)，调用 44A090、清两槽并返回 0。
std::int32_t Sub_43C8F0();                 // 原 0x43C8F0。
std::int32_t Sub_504100();                 // 原 0x504100。
std::int32_t Sub_518C80();                 // 原 0x518C80。
std::FILE* Sub_518CB0();                   // 原 0x518CB0。
void Sub_518780();                         // 原 0x518780。
std::int8_t Sub_503E80();                  // 原 0x503E80，IDA 原型 char()。
void Sub_43F220();                         // 原 0x43F220。
using cg::audio::Sub_52B990;               // 原 0x52B990。
std::int32_t Sub_45EBB0();                 // 原 0x45EBB0。
void DrawNormalScene_51C7D0();             // 原 World::DrawNormalScene@0x51C7D0。
std::int8_t Sub_519D50();                  // 原 0x519D50，IDA 原型 char()。
std::int8_t Sub_51D750();                  // 原 0x51D750，IDA 原型 char()。
std::int32_t Sub_520160();                 // 原 0x520160，IDA 原型 BOOL()。
void DrawBattleScene_43CBB0();             // 原 World::DrawBattleScene@0x43CBB0。

// 原 0x51FDE0；第三槽保持原 32 位载荷，selector 2 才把该位模式解释为 float。
std::int32_t Sub_51FDE0(
    std::int32_t selector, std::int32_t value, std::uint32_t payloadBits);
std::int32_t Sub_455C90();                 // 原 0x455C90，返回值按任意非零判断。

// 原 dword_D460C8@0xD460C8；只保留地址身份和 W32 宽度。
extern std::int32_t gValueD460C8;

} // namespace cg::world::detail
