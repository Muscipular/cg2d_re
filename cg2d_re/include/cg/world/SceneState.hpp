#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace cg::session { class ClientSession; }

namespace cg::world {

// World::Update@0x51C430 直接访问的剩余原全局；地址以外的业务含义仍未确认。
const std::wstring& GetMapName_9DAD30(session::ClientSession& session) noexcept; // 借用宽字符串地图名，回复入口统一从GBK转换。
void ClearMapName_9DAD30(session::ClientSession& session) noexcept; // 世界重置时清空同一地图名称。

// 原 51BEF0：仅清玩家首块 FDA88E 高半字 bit 7，保留同 W32 其余位。
void ResumePlayerAfterObservation_51BEF0(session::ClientSession& session) noexcept;

std::int32_t ReadRippleGateOnce(session::ClientSession& session);

// 原51C410：仅校验五槽索引，不检查宠物used。
// SLC01：原51C410只判断S32索引0..4；已证八调用只消费低W8结果。
bool IsValidPetIndex_51C410(std::int32_t index) noexcept;

// 原 sub_448240@0x448240；只采样一次所属会话楼层。
std::int32_t IsMapFloorAllowed_448240(session::ClientSession& session) noexcept;

// 原 sub_4493C0@0x4493C0；只采样一次所属会话楼层，判定闭区间及四项表，返回 0/1。
std::int32_t IsMapDriftEffectAllowed_4493C0(session::ClientSession& session) noexcept;

// 原 sub_46CE30@0x46CE30；X/Y 各写四字节，返回借用的 outX，不检查输出指针。
float* ProjectWorldToScreen_46CE30(session::ClientSession& session,
    float worldX,
    float worldY,
    float* outX,
    float* outY) noexcept;
// 原46CE80：反投影，X写入后才读取Y参考位槽，返回原outX。
float* ProjectScreenToWorld_46CE80(session::ClientSession& session,
    float screenX, float screenY, float* outX, float* outY) noexcept;

// 原 sub_51C770@0x51C770；先清当前场景状态，再写当前场景类型，返回 sceneType。
std::int32_t SetCurrentSceneType(session::ClientSession& session, std::int32_t sceneType);

// 原 sub_51C790@0x51C790；依次写当前场景类型、状态，返回 sceneType。
std::int32_t SetCurrentSceneTypeAndState(session::ClientSession& session, std::int32_t sceneType, std::int32_t sceneState);

// 原 sub_51C7B0@0x51C7B0；先清待切换场景状态，再写待切换场景类型，返回 sceneType。
std::int32_t SetPendingSceneType(session::ClientSession& session, std::int32_t sceneType);

// 原 sub_468FF0@0x468FF0；按原指令顺序把世界投影当前值复制到帧末快照，返回 D。
std::uint32_t CopyWorldProjectionState_468FF0(session::ClientSession& session);

// 原 46FB20；一次 RAND 旋转写 100 项，返回 100，不清消费游标。
std::int32_t InitializeRotationTable_46FB20(session::ClientSession& session);
// 原 455C70 + 独占尾块 46FB50；清两状态槽后取表、推进游标。
std::int32_t NextRotationValue_455C70(session::ClientSession& session);

// 原 518C20：清唯一相册区后复制 CCA 含 NUL 四字节，返回 4；不执行文件读写。
std::uint32_t InitializeAlbum_518C20();

// 原 World::Update@0x51C430；场景状态消费与分发，实现在WorldUpdate.cpp，依赖按所属模块接入。
void Update(session::ClientSession& session);

} // namespace cg::world
