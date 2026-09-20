#pragma once

#include <cstdint>
#include <cstdio>

namespace cg::world::detail {

// World::Update@0x51C430 的直接目标声明；未实现项只保留 IDA 地址和调用契约。
std::int32_t Sub_45C5A0();                 // 原 0x45C5A0。
std::int32_t Sub_45F190();                 // 原 0x45F190。
std::int32_t Sub_464840();                 // 原 0x464840。
void RenderChara_521F20();                 // 原 World::RenderChara@0x521F20。
void RenderInput_4590E0();                 // 原 World::RenderInput@0x4590E0，已完整还原。
void Sub_44A410();                         // 原 0x44A410，输入光标闪烁/排队。
void Sub_4626A0();                         // 原 0x4626A0。
std::int32_t Sub_45E3D0();                 // 原 0x45E3D0。
void Sub_449DA0();                         // 原 0x449DA0。
std::int32_t Sub_46A9E0();                 // 原 0x46A9E0。
std::int32_t* Sub_467F30();                // 原 0x467F30。
struct RecipeItemData;
RecipeItemData* Sub_5199A0();              // 原 0x5199A0。
struct YobiAction;
YobiAction** Sub_446470();                 // 原 0x446470。
void Sub_4D3170();                         // 原 0x4D3170。
std::int32_t Sub_455BD0();                 // 原 0x455BD0。
std::uint32_t* Sub_46E2A0();               // 原 0x46E2A0。
std::int32_t Sub_46BD40();                 // 原 0x46BD40。
std::int32_t Sub_504A30();                 // 原 0x504A30。
std::int32_t Sub_43C8F0();                 // 原 0x43C8F0。
std::int32_t Sub_504100();                 // 原 0x504100。
std::int32_t Sub_518C80();                 // 原 0x518C80。
std::FILE* Sub_518CB0();                   // 原 0x518CB0。
void NullSub_509080();                     // 原 nullsub_1@0x509080。
void Sub_518780();                         // 原 0x518780。
std::int8_t Sub_503E80();                  // 原 0x503E80，IDA 原型 char()。
void Sub_43F220();                         // 原 0x43F220。
std::uint32_t Sub_52B990();                // 原 0x52B990。
std::int32_t Sub_45EBB0();                 // 原 0x45EBB0。
void DrawNormalScene_51C7D0();             // 原 World::DrawNormalScene@0x51C7D0。
std::int8_t Sub_519D50();                  // 原 0x519D50，IDA 原型 char()。
std::int8_t Sub_51D750();                  // 原 0x51D750，IDA 原型 char()。
std::int32_t Sub_520160();                 // 原 0x520160，IDA 原型 BOOL()。
void Sub_5081A0();                         // 原 0x5081A0。
void DrawBattleScene_43CBB0();             // 原 World::DrawBattleScene@0x43CBB0。

// 原 0x51FDE0；第三槽保持原 32 位载荷，selector 2 才把该位模式解释为 float。
std::int32_t Sub_51FDE0(
    std::int32_t selector, std::int32_t value, std::uint32_t payloadBits);
std::int32_t Sub_455C90();                 // 原 0x455C90，返回值按任意非零判断。

// 原 dword_D460C8@0xD460C8；只保留地址身份和 W32 宽度。
extern std::int32_t gValueD460C8;

} // namespace cg::world::detail
