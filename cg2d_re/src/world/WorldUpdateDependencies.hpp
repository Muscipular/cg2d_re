#pragma once

namespace cg::network { class CgConnection; }
namespace cg::session { class ClientSession; }

#include "cg/platform/LegacyCryptInt.hpp"

#include <cstdint>
#include <string>
// 世界场景调度与交互编排所需的共享声明。

namespace cg::world {

// 原73ECD0，508CF0与多个场景/UI共享；显式CRT生命周期，禁止创建登录副本。
extern platform::LegacyCryptInt gRippleGate; // 原全局：0x009A7FA8，波纹门禁。
extern platform::LegacyCryptInt gSceneTransitionValue; // 原全局：0x0073ECD0。

// 原 0x581800：4D3170 重置、World::OpenDialog 读写的独立 W32 槽。
extern std::int32_t gCurrentDialogType_581800;
extern std::int32_t gDialogRequestCC21E0; // 原全局：0x00CC21E0。 原对话a3，49D2E0/49D8F0生产，发送共用。
extern std::int32_t gDialogTargetD2DE6C; // 原全局：0x00D2DE6C。 原对话a4，不是窗口51的独立参数槽。
extern std::uint64_t gMapNoticeStartTick_62B5B0; // 原全局：0x0062B5B0。

// World::Update@0x51C430 的直接目标声明；保留原地址及调用契约，提供者按所属模块实现。
// 原函数：0x0045F190。
std::int32_t UpdateCapability3LoginScene_45F190(network::CgConnection& connection); // 原 0x45F190。
// 原函数：0x00464840。
std::int32_t UpdateCapability4LoginScene_464840(network::CgConnection& connection); // 原 0x464840。
// 原函数：0x00476680。
void DrawChatRangeOverlay_476680();                         // 原 [476680,476739)：玩家范围覆盖入队及显示倒计时。
// 原函数：0x0050C870。
std::int32_t __cdecl HandlePlayerWorldTimeReply_50C870(cg::network::CgConnection& networkConnection, std::int32_t playerValue,
    std::int32_t timeOffset, std::int32_t pathValue,
    std::int32_t adjustment); // GTP01：PudO 校时，持有 category，两 time/旧 adjustment 与最后 S32 返回保。
// 原函数：0x0050E1A0。
void HandleDirectionalMapEffectsReply_50E1A0(cg::network::CgConnection& networkConnection, std::int32_t mask, std::int32_t value,
    const char* unusedText); // 三效果半字写入后调用既有46E0D0。
// 原函数：0x0050E210。
std::int32_t HandleNotificationSoundReply_50E210(cg::network::CgConnection& networkConnection, std::int32_t value); // 九种指定音效，其他值透传。
// 原函数：0x0050E650。
void HandleMapSceneEffectReply_50E650(cg::network::CgConnection& networkConnection, std::int32_t mode, std::int32_t duration,
    std::int32_t floor, std::int32_t mapX, std::int32_t mapY); // PVfz原序转449700，socket未用。
// 原函数：0x00440B50。
std::int32_t IsObservationSceneTransitionActive_440B50(); // 原当前场景U32闭区间250..252门禁。
// 原快捷按钮与两条键盘路径共用；caller均不消费EAX，复用唯一62B5AC计时槽。
// 原函数：0x00470050。
void RequestDuelWithFacingPlayer_470050(); // 对战请求。
// 原函数：0x00470150。
void RequestSpectateFacingBattle_470150(); // 观战请求，先查地图名单。
// 原函数：0x00470270。
void RequestPartyJoinOrLeave_470270(); // 组队请求，保留跟随分支和静默返回。
// 原函数：0x004703C0。
void RequestFacingPlayerContactCard_4703C0(); // 名片请求，无跟随门禁。
// 原函数：0x004704A0。
void RequestNearbyPlayerTrade_4704A0(); // 交易请求，两格查询均执行。
// 原函数：0x00470670。
void RequestFacingPlayerGuildInvitation_470670(); // 家族邀请，读取同源职级权限低U8。
// 原函数：0x004FA8E0。
void RequestPlayerEmote_4FA8E0(std::int32_t index); // 既有弃返回调用的动作命令接口。
// 原函数：0x004410E0。
std::int32_t GetActorStatusIconCount_4410E0();                 // 原 [0x4410E0,0x4410E6)，无参返回 17。
// 原 [51D640,51D6E3)：场景地图画入 Back 后复制到 Temp，返回同次 tick。
std::uint32_t CaptureMapTilesForTransition_51D640();

// 原 dword_D460C8@0xD460C8；只保留地址身份和 W32 宽度。
extern std::int32_t gInputAndHoverSuppressed_D460C8;

namespace detail {

// 原455BD0等入口仍有消费者的共享门禁；无消费者槽已按GVC01清理。
extern std::uint16_t gPlayerMovementAndFacingBlocked_62B5AA; // 原全局：0x0062B5AA。
extern std::uint32_t gMapTargetInputAndDrawOverride_62B308; // 原全局：0x0062B308。
extern std::uint32_t gMapTargetInputAndDrawOverride_62B398; // 原全局：0x0062B398。 原455FC0发布，目标格门禁共用。
// 原函数：0x004590E0。
void RenderInput_4590E0();                 // 原 World::RenderInput@0x4590E0，已完整还原。
// 原函数：0x00458F40。
void __cdecl UpdatePlayerCountdown_458F40(); // GTP01：同源倒计时旧字段先齐取/持有判负，场景9要求玩家 T*。
// 原函数：0x00458F00。
void __cdecl UpdateDayPeriodPaletteAndTimers_458F00(); // GTP01：类别变化后可选过渡，末尾总推进同源倒计时；原残值 ABI 待。
// 原函数：0x00519EB0。
std::int32_t __cdecl SetPlayerObjectIndex_519EB0(std::int32_t value); // GTP01：真实玩家 W32 对象号写回/原 S32 透传。
// 原函数：0x00455C60。
void ResetTimedMapNotice_455C60(); // 仅清既有62B5B0，不关闭窗口。
// 原函数：0x00455FC0。
void SynchronizeTimedMapNoticeWindow_455FC0(); // 窗口39创建/动画清理父入口，保留额外SE56。
// 原函数：0x00455A90。
std::int32_t IsMapBattleSpectatingAllowed_455A90(); // G05：所有地图允许通过观战地图门禁。
// 原函数：0x004FA8E0。
std::uintptr_t RequestEmoteWithResult_4FA8E0(std::int32_t index); // 旧随机快捷键需要的原生action地址/完整W32混合返回。
// 原函数：0x004D3170。
void ResetCurrentDialogType_4D3170();                         // 原 [0x4D3170,0x4D317B)，重置 0x581800 为 -1。
// 原函数：0x00455BD0。
std::int32_t InitializeWorldSceneFlags_455BD0();                 // GVC01保留有效状态重置及返回0，已清理原无消费者槽。
// 原函数：0x00504A30。
std::int32_t InitializeGameplayNotifications_504A30();                 // 原 [0x504A30,0x504A43)，调用 44A090、清两槽并返回 0。
// 原函数：0x00504100。
std::int32_t ResetSceneUiState_504100();                 // 原 [0x504100,0x504113)，顺序清三槽、返回 0。
// 原函数：0x0045EBB0。
std::int32_t ProcessLogoutScene_45EBB0(session::ClientSession& session); // 原 0x45EBB0。
// 原函数：0x0045EEE0。
std::int32_t ProcessLogoutTransition_45EEE0(session::ClientSession& session); // 原45EEE0，场景7过渡初始化/绘制，仅完成时返回1。
// 原函数：0x0045ED80。
void ResetLogoutScene_45ED80(session::ClientSession& session); // 原 [0x45ED80,0x45ED8B)，重置 9A6204。
// 原函数：0x0045ED90。
std::int32_t UpdateLogoutScene_45ED90(session::ClientSession& session); // 原 [0x45ED90,0x45EEDA)，场景 7 请求与展示。
// 原函数：0x0051C7D0。
void DrawNormalScene_51C7D0(session::ClientSession& session); // 原 World::DrawNormalScene@0x51C7D0。
// 原函数：0x0051D5B0。
void* __cdecl PrepareMapTransitionObservationState_51D5B0();               // 原 [0x51D5B0,0x51D63A)；非空链尾返指针，空链 EAX 未定义；46ADA0 透传位型。
// 原函数：0x0051D750。
std::uint32_t CaptureSceneAndSelectEvenVariant_51D750();              // 原 0x51D750；完整 EAX 写入/返回，六调用点均弃值。

// 原 0x51FDE0；第三槽保持原 32 位载荷，浮点目标自行解释位模式。
std::int32_t ProcessSceneTransitionEffect_51FDE0(
    std::int32_t selector, std::int32_t value, std::uint32_t payloadBits);
// 51FDE0 原调用均压入三个 W32 槽；新端声明只保留目标实际读取的逻辑参数。
// 原函数：0x0051D790。
std::int32_t ProcessSceneSlideOut_51D790(
    std::int32_t index, std::int32_t value, std::uint32_t payloadBits);
// 原函数：0x0051E0D0。
std::int32_t ProcessSceneSlideIn_51E0D0(
    std::int32_t index, std::int32_t value, std::uint32_t payloadBits);
// 原函数：0x0051EC10。
std::int32_t ProcessSceneQuadrantTransition_51EC10(std::int32_t index, std::int32_t value);
// 原函数：0x0051EF80。
std::int32_t ProcessSceneRandomTileTransition_51EF80(
    std::int32_t index, std::int32_t value, std::uint32_t payloadBits);
// 原函数：0x0051F610。
std::int32_t ProcessSceneGridScaleTransition_51F610(std::int32_t value);
// 原函数：0x0051F860。
std::int32_t ProcessTopScanlineTransition_51F860(std::int32_t index, std::int32_t value);
// 原函数：0x0051FA70。
std::int32_t ProcessBottomScanlineTransition_51FA70(std::int32_t index, std::int32_t value);
// 原函数：0x0051FC90。
std::int32_t ProcessSceneCenterScanlineTransition_51FC90(std::int32_t index, std::int32_t value);
// 原函数：0x00455C90。
std::int32_t UpdateDisconnectDialog_455C90(session::ClientSession& session); // 原 0x455C90，返回值按任意非零判断。

} // namespace detail

} // namespace cg::world
