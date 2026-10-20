#pragma once

namespace cg::network { class CgConnection; }

#include "../platform/MainLoopDependencies.hpp"

#include "../scene/character_select/CharacterSelectSceneDependencies.hpp"

#include <string_view>

#include <cstdint>
#include <string>

namespace cg::network::detail {

// 原 [0x50E6C0,0x50E6D8)：忽略 socket 首参，转发后三个 S32 到空间音频生产者。
std::int32_t __cdecl HandleSpatialSoundSourceReply_50E6C0(cg::network::CgConnection& networkConnection, std::int32_t index,
    std::int32_t sourceX, std::int32_t sourceY);

// 原 [0x50CF60,0x50CF6A)：忽略网络首参，写共享点数并返回完整值。
std::int32_t HandleRemainingAttributePointsReply_50CF60(cg::network::CgConnection& networkConnection, std::int32_t value);

// 原 [0x50CF20,0x50CF55)：第二参数为零才向聊天窗口追加固定原字节提示。
void HandleFailureNoticeReply_50CF20(cg::network::CgConnection& networkConnection, std::int32_t status,
    std::int32_t unused3, std::int32_t unused4, std::int32_t unused5);

// 原 [0x50D3B0,0x50D3BC)：只写第二参数低 W16 并返回相同位模式。
std::uint16_t Sub_50D3B0(cg::network::CgConnection& networkConnection, std::int32_t value);

// 原 [0x50E310,0x50E33B)：场景门禁为零且值在 [200,249] 时发布两槽。
std::int32_t HandleMapBgmChangeReply_50E310(cg::network::CgConnection& networkConnection, std::int32_t value);

// 原 [0x50E630,0x50E64C)：场景 10 早退，否则转发两参数到 450B70。
void HandlePaletteTransitionReply_50E630(cg::network::CgConnection& networkConnection, std::int32_t index, std::int32_t steps);

// 原 [0x50D700,0x50D73A)：场景门禁后写阶段与两项 base62 回复值。
std::int32_t HandleDuelPointResultReply_50D700(cg::network::CgConnection& networkConnection, const char* payload);

// LDT01／原50DD80：双门禁后按模式创建专用动作；带记录时先解析并调用445A60。
// 外层图号/关联编号与payload中的记录图号/对象编号保持独立。
void HandleMapActorTransitionReply_50DD80(cg::network::CgConnection& networkConnection, std::int32_t objectIndex,
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t direction, std::int32_t withRecord,
    std::int32_t mode, const char* payload);

// 原 [0x50E830,0x50E844)：共享 W32 非零直接返回，零时清并回写公会邮件。
std::int32_t HandleGuildMailResetReply_50E830(cg::network::CgConnection& networkConnection);

// GMR01：原50F130完整gate/五域重置→W16活动统计/W32计数发布→同key公会邮件删除，原返回保。
std::int32_t HandleGuildMemberRemovalReply_50F130(cg::network::CgConnection& networkConnection, std::int32_t key);
// GMR01：原50F190完整字段/反转义/排序index持有，生产后外部记录名通知/保存/SE/消息原序。
// dispatcher 实际传四参；第一、第四参数未读取。
void HandleGuildMailReply_50F190(cg::network::CgConnection& networkConnection, std::int32_t key,
    const char* payload, std::int32_t unusedFourth);

// 原510460：9A6E0C门禁后调用51B630，四个S32不窄化。
void HandleInventorySlotSwapReply_510460(cg::network::CgConnection& networkConnection, std::int32_t firstGroup, std::int32_t firstIndex,
    std::int32_t secondGroup, std::int32_t secondIndex);
// 原510490：无全局门禁，生产world唯一F7C950前缀，固定消费27字段。
void HandleItemDetailReply_510490(cg::network::CgConnection& networkConnection, const char* text); // IER01：唯一属性回复记录生产，void副作用接口保持；原返回在分发父覆盖，全ABI/运行待。

// 原 sub_5176B0@0x5176B0 已在 NetworkRuntime.cpp 还原；
// 此处只保留 detail 边界的唯一声明。
std::int32_t DecodeProtocolBase62_5176B0(const char* text);

// 原 sub_517730@0x517730 已在 NetworkRuntime.cpp 还原；
// 此处只保留 detail 边界的唯一声明。
std::uint8_t* EncodeProtocolBase62_517730(
    std::int32_t value,
    std::uint8_t* destination,
    std::int32_t capacity);

} // namespace cg::network::detail
