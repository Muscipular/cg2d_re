#pragma once

#include <cstdint>

namespace cg::network::detail {

// 原 [0x5082D0,0x508310) / [0x508310,0x508350)：NetworkLoop 错误日志叶。
std::int32_t Sub_5082D0();
std::int32_t Sub_508310();

// 原 [0x50E6C0,0x50E6D8)：忽略首参数，转发后三项到空间音频生产者。
std::int32_t __cdecl Sub_50E6C0(
    std::int32_t unused, std::int32_t index,
    std::int32_t sourceX, std::int32_t sourceY);

// 原 [0x50CF60,0x50CF6A)：忽略网络首参，写共享点数并返回完整值。
std::int32_t Sub_50CF60(std::uintptr_t socket, std::int32_t value);

// 原 [0x50CF20,0x50CF55)：第二参数为零才向聊天窗口追加固定原字节提示。
void Sub_50CF20(
    std::uintptr_t socket, std::int32_t status,
    std::int32_t unused3, std::int32_t unused4, std::int32_t unused5);

// 原 [0x50D3B0,0x50D3BC)：只写第二参数低 W16 并返回相同位模式。
std::uint16_t Sub_50D3B0(std::uintptr_t socket, std::int32_t value);

// 原 [0x50E310,0x50E33B)：场景门禁为零且值在 [200,249] 时发布两槽。
std::int32_t Sub_50E310(std::uintptr_t socket, std::int32_t value);

// 原 [0x50E630,0x50E64C)：场景 10 早退，否则转发两参数到 450B70。
void Sub_50E630(
    std::uintptr_t socket, std::int32_t index, std::int32_t steps);

// 原 [0x50D700,0x50D73A)：场景门禁后写阶段与两项 base62 回复值。
std::int32_t Sub_50D700(std::uintptr_t socket, const char* payload);

// 原50DD80：双门禁后按模式创建专用动作；带记录时先解析并调用445A60。
// 外层图号/关联编号与payload中的记录图号/对象编号保持独立。
void Sub_50DD80(
    std::uintptr_t unusedSocket, std::int32_t objectIndex,
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t direction, std::int32_t withRecord,
    std::int32_t mode, const char* payload);

// 原 [0x50E830,0x50E844)：共享 W32 非零直接返回，零时清并回写公会邮件。
std::int32_t Sub_50E830(std::uintptr_t unusedSocket);

// 原 50F130：门禁后部分重置外部记录、发布计数，再删除同 key 公会邮件；不保存文件。
std::int32_t Sub_50F130(std::uintptr_t unusedSocket, std::int32_t key);
// 原 50F190：无全局门禁，解析/插入后依序保存、播音效和追加原字节通知。
// dispatcher 实际传四参；第一、第四参数未读取。
void Sub_50F190(
    std::uintptr_t unusedSocket, std::int32_t key,
    const char* payload, std::int32_t unusedFourth);

// 原510460：9A6E0C门禁后调用51B630，四个S32不窄化。
void Sub_510460(
    std::uintptr_t unusedSocket, std::int32_t firstGroup, std::int32_t firstIndex,
    std::int32_t secondGroup, std::int32_t secondIndex);
// 原510490：无全局门禁，生产world唯一F7C950前缀，固定消费27字段。
void Sub_510490(std::uintptr_t unusedSocket, const char* text);

// 原 sub_4038A0@0x4038A0：就地变换 text；key3/key5 参与逐轮计算，
// 第二、四参数未读取；返回末轮方向，零轮返回 0。
char Sub_4038A0(
    char* text,
    std::int32_t,
    const char* key3,
    std::int32_t,
    const char* key5);

// 原403A70：命令反向变换，唯一caller弃返回；第二、第四参数仍不读取。
void Sub_403A70(
    char* text, std::int32_t unused2, const char* key3,
    std::int32_t unused4, const char* key5);
// 原5173B0：观察回调/收包日志后两遍原位分词，借用既有char*参数表。
void SplitReceivedBuffer_5173B0(char* buffer);

// sub_403E50@0x403E50 尚未实现；只保留当前 IDA 的真实回调契约。
std::int32_t Sub_403E50(
    std::uintptr_t socket,
    const void* data,
    std::uint32_t size);

// 原取包排除接线仍待处理；完整分派本体已落位，其余回复依赖见NetworkDispatchDependencies。
std::int32_t Sub_403C50(void* destination, std::int32_t capacity);
std::int32_t NetworkDispatch_512E40(
    std::uintptr_t socket, const void* received);

// 原 sub_5176B0@0x5176B0 已在 NetworkRuntime.cpp 还原；
// 此处只保留 detail 边界的唯一声明。
std::int32_t Sub_5176B0(const char* text);

// 原 sub_517730@0x517730 已在 NetworkRuntime.cpp 还原；
// 此处只保留 detail 边界的唯一声明。
std::uint8_t* Sub_517730(
    std::int32_t value,
    std::uint8_t* destination,
    std::int32_t capacity);

} // namespace cg::network::detail

namespace cg::character_select::detail {

extern char gText9A4EAC[128];

} // namespace cg::character_select::detail

namespace cg::platform {

void ParseProtectionTables_404600(const char* input);

} // namespace cg::platform

namespace cg::world::detail {

// 原 0xD2F788/0xFF0C10/0xFF0D04/0xFF0D08：复用世界状态的唯一 S32/W16 存储。
extern std::int32_t gValueD2F788;
extern std::uint16_t gValueFF0C10;
extern std::int32_t gValueFF0D04;
extern std::int32_t gValueFF0D08;
extern std::uint16_t gValueFF0D34;

// 原 World::AppendMsgToCharWindow@0x44B050：复用已有聊天窗口入口。
void Sub_44B050(const char* text, std::int32_t color, std::int32_t fontType);

} // namespace cg::world::detail
