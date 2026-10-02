#pragma once

#include <array>
#include <cstdint>

namespace cg::world {

// 原 0xFF0D40；引用 platform::gValueFF0D40，不建立第二份当前场景类型存储。
extern std::int32_t& gCurrentSceneType;

extern std::int32_t gPendingSceneState; // 原 0xFF0D60。
extern std::int32_t gCurrentSceneState; // 原 0xFF0D64。
extern std::int32_t gPendingSceneType;  // 原 0xFF0D6C。

// World::Update@0x51C430 直接访问的剩余原全局；地址以外的业务含义仍未确认。
extern std::uint8_t gValueC82204;              // 原 0xC82204，W8。
extern std::uint32_t gValue5ABFC4;             // 原 0x5ABFC4，W32。
extern std::uint32_t gValue9A6E0C;             // 原 0x9A6E0C，W32。
extern std::uint32_t gValueFF0D5C;             // 原 0xFF0D5C，W32。
extern std::uint32_t gValueFF0D58;             // 原 0xFF0D58，W32。
extern std::uint32_t gValueD67E78;             // 原 0xD67E78，W32。
extern std::uint32_t gValueFF0D74;             // 原 0xFF0D74，W32。
extern std::uint8_t& gValue9DAD30;             // 原 0x9DAD30 连续文本区的首 W8 别名。
const char* ReadText9DAD30() noexcept; // 借用地图回复的同源文本，供窗口39复制/测宽。
extern std::array<std::uint32_t, 7> gValueD67B08; // 原 0xD67B08，严格 7 个 DWORD。
extern std::uint32_t gValue58B6D0;             // 原 0x58B6D0，W32，映像初值 1。
extern std::uint8_t gValueD9F2CA;              // 原 0xD9F2CA，W8。

// 原 FDA880 玩家首块 W32；从同一玩家运行期 owner 读取，不借用场景时间槽。
std::uint32_t ReadPlayerObjectIndex_FDA880() noexcept;
// 原 51BEF0：仅清玩家首块 FDA88E 高半字 bit 7，保留同 W32 其余位。
void Sub_51BEF0() noexcept;

// 原世界等距投影状态的当前值与帧末快照；八槽均为独立 uint32_t，映像初值 0。
extern std::uint32_t gWorldProjectionSourceA;   // 原 0x9EE908，R32。
extern std::uint32_t gWorldProjectionSourceB;   // 原 0x9EE90C，R32。
extern std::uint32_t gWorldProjectionSourceC;   // 原 0x9EE904，R32。
extern std::uint32_t gWorldProjectionSourceD;   // 原 0x9EE8F4，R32。
extern std::uint32_t gWorldProjectionSnapshotA; // 原 0xAF10C4，W32。
extern std::uint32_t gWorldProjectionSnapshotB; // 原 0xAF10CC，W32。
extern std::uint32_t gWorldProjectionSnapshotC; // 原 0xAF10D0，W32。
extern std::uint32_t gWorldProjectionSnapshotD; // 原 0xAF10D4，W32。

// 原波纹路径的世界状态；浮点坐标以 F32 位模式保存，映像初值均为 0。
extern volatile std::int32_t gMapFloor;                    // 原 0x9EE8F0，S32。
extern volatile std::uint32_t gWorldCurrentXBits;          // 原 0x9DAD4C，F32。
extern volatile std::uint32_t gWorldCurrentYBits;          // 原 0x9DAD50，F32。
extern volatile std::uint32_t gWorldProjectionReferenceXBits; // 原 0xC751A8，F32。
extern volatile std::uint32_t gWorldProjectionReferenceYBits; // 原 0xC751A4，F32。

// 原 0x9A7FA8 CryptInt 的唯一私有存储由 SceneState.cpp 持有。
void InitializeRippleGate(); // 原553094：构造后独立登记退出，登记失败不重试。
std::int32_t ReadRippleGateOnce();

// 原456FE0/457000，73ECD0场景共享CryptInt；CRT排在World表后、玩家坐标前。
void ConstructSceneTransitionValue();
void DestroySceneTransitionValue();

// 原553064：先构造X/Y，再登记按Y/X逆序析构的一个退出回调。
void InitializePlayerCoordinates();

// 原553068..553078：六对象按五个表项逐项构造并登记退出，不合并登记。
void InitializeWorldCoordinateStates();

// 原55307C..553090：七对象按六个表项逐项构造并登记退出，不合并登记。
void InitializeWorldMoveStates();

// 原5184A0/518410，玩家六组CryptInt；排在鼠标构造之后。
void ConstructPlayerRuntimeValues();
void DestroyPlayerRuntimeValues();
// 原518530/518560，五条宠物各六组CryptInt；排在玩家构造之后。
void ConstructPetRuntimeValues();
void DestroyPetRuntimeValues();
// 原51C410：仅校验五槽索引，不检查宠物used。
bool IsValidPetIndex_51C410(std::int32_t index) noexcept;

// 原 sub_448240@0x448240；只采样一次 gMapFloor。
std::int32_t IsMapFloorAllowed_448240() noexcept;

// 原 sub_4493C0@0x4493C0；只采样一次 gMapFloor，判定闭区间及四项表，返回 0/1。
std::int32_t Sub_4493C0() noexcept;

// 原 sub_46CE30@0x46CE30；X 先写、Y 后写，不检查输出指针。
void ProjectWorldToScreen_46CE30(
    float worldX,
    float worldY,
    float* outX,
    float* outY) noexcept;
// 原46CE80：反投影，X写入后才读取Y参考位槽，返回原outX。
float* ProjectScreenToWorld_46CE80(
    float screenX, float screenY, float* outX, float* outY) noexcept;

// 原 sub_51C770@0x51C770；先清当前场景状态，再写当前场景类型，返回 sceneType。
std::int32_t SetCurrentSceneType(std::int32_t sceneType);

// 原 sub_51C790@0x51C790；依次写当前场景类型、状态，返回 sceneType。
std::int32_t SetCurrentSceneTypeAndState(std::int32_t sceneType, std::int32_t sceneState);

// 原 sub_51C7B0@0x51C7B0；先清待切换场景状态，再写待切换场景类型，返回 sceneType。
std::int32_t SetPendingSceneType(std::int32_t sceneType);

// 原 sub_468FF0@0x468FF0；按原指令顺序把世界投影当前值复制到帧末快照，返回 D。
std::uint32_t CopyWorldProjectionState_468FF0();

// 原 46FB20；一次 RAND 旋转写 100 项，返回 100，不清消费游标。
std::int32_t InitializeRotationTable_46FB20();
// 原 455C70 + 独占尾块 46FB50；清两状态槽后取表、推进游标。
std::int32_t NextRotationValue_455C70();

// 原 518C20：清唯一相册区后复制 CCA 含 NUL 四字节，返回 4；不执行文件读写。
std::uint32_t InitializeAlbum_518C20();

// 原 World::Update@0x51C430；完整场景状态消费与分发，场景目标仍为真实未实现依赖。
void Update();

} // namespace cg::world
