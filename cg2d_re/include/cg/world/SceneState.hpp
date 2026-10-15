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

std::int32_t ReadRippleGateOnce();

// 原456FE0/457000，73ECD0场景共享CryptInt；CRT排在World表后、玩家坐标前。
// 对应原CRT 553054；同组构析由拥有者接管，实际全应用退出仍待运行验收。
// 原函数：0x00456FD0。
class SceneTransitionValueStartup final {
public:
    // 原函数：0x00456FD0；只接管同组原始载荷的启动寿命。
    SceneTransitionValueStartup();
    ~SceneTransitionValueStartup();
    SceneTransitionValueStartup(const SceneTransitionValueStartup&) = delete;
    SceneTransitionValueStartup& operator=(const SceneTransitionValueStartup&) = delete;
};

// 原函数：0x00456FE0。
void ConstructSceneTransitionValue();
// 原函数：0x00457000。
void DestroySceneTransitionValue();

// 原CRT 553064..553094十三组坐标／门禁；快照直接定义，其余由启动拥有者保原构造及逆序清理。
// 原函数：0x00467B90。
class PlayerCoordinatesStartup final {
public:
    // 原函数：0x00467B90；只接管同组原始载荷的启动寿命。
    PlayerCoordinatesStartup();
    ~PlayerCoordinatesStartup();
    PlayerCoordinatesStartup(const PlayerCoordinatesStartup&) = delete;
    PlayerCoordinatesStartup& operator=(const PlayerCoordinatesStartup&) = delete;
};

// 原函数：0x00467C70。
class WorldCoordinateState9EE8D0Startup final {
public:
    // 原函数：0x00467C70；只接管同组原始载荷的启动寿命。
    WorldCoordinateState9EE8D0Startup();
    ~WorldCoordinateState9EE8D0Startup();
    WorldCoordinateState9EE8D0Startup(const WorldCoordinateState9EE8D0Startup&) = delete;
    WorldCoordinateState9EE8D0Startup& operator=(const WorldCoordinateState9EE8D0Startup&) = delete;
};

// 原函数：0x00467CB0。
class WorldCoordinateState9EE8E0Startup final {
public:
    // 原函数：0x00467CB0；只接管同组原始载荷的启动寿命。
    WorldCoordinateState9EE8E0Startup();
    ~WorldCoordinateState9EE8E0Startup();
    WorldCoordinateState9EE8E0Startup(const WorldCoordinateState9EE8E0Startup&) = delete;
    WorldCoordinateState9EE8E0Startup& operator=(const WorldCoordinateState9EE8E0Startup&) = delete;
};

// 原函数：0x00467CF0。
class WorldCoordinatePairStartup final {
public:
    // 原函数：0x00467CF0；只接管同组原始载荷的启动寿命。
    WorldCoordinatePairStartup();
    ~WorldCoordinatePairStartup();
    WorldCoordinatePairStartup(const WorldCoordinatePairStartup&) = delete;
    WorldCoordinatePairStartup& operator=(const WorldCoordinatePairStartup&) = delete;
};

// 原函数：0x00467D50。
class WorldMoveTargetsStartup final {
public:
    // 原函数：0x00467D50；只接管同组原始载荷的启动寿命。
    WorldMoveTargetsStartup();
    ~WorldMoveTargetsStartup();
    WorldMoveTargetsStartup(const WorldMoveTargetsStartup&) = delete;
    WorldMoveTargetsStartup& operator=(const WorldMoveTargetsStartup&) = delete;
};

// 原函数：0x00467DB0。
class WorldMoveStateC821E8Startup final {
public:
    // 原函数：0x00467DB0；只接管同组原始载荷的启动寿命。
    WorldMoveStateC821E8Startup();
    ~WorldMoveStateC821E8Startup();
    WorldMoveStateC821E8Startup(const WorldMoveStateC821E8Startup&) = delete;
    WorldMoveStateC821E8Startup& operator=(const WorldMoveStateC821E8Startup&) = delete;
};

// 原函数：0x00467DF0。
class WorldMoveState9E03C0Startup final {
public:
    // 原函数：0x00467DF0；只接管同组原始载荷的启动寿命。
    WorldMoveState9E03C0Startup();
    ~WorldMoveState9E03C0Startup();
    WorldMoveState9E03C0Startup(const WorldMoveState9E03C0Startup&) = delete;
    WorldMoveState9E03C0Startup& operator=(const WorldMoveState9E03C0Startup&) = delete;
};

// 原函数：0x00467E30。
class WorldMoveState9FB918Startup final {
public:
    // 原函数：0x00467E30；只接管同组原始载荷的启动寿命。
    WorldMoveState9FB918Startup();
    ~WorldMoveState9FB918Startup();
    WorldMoveState9FB918Startup(const WorldMoveState9FB918Startup&) = delete;
    WorldMoveState9FB918Startup& operator=(const WorldMoveState9FB918Startup&) = delete;
};

// 原函数：0x00467E70。
class WorldMoveStateAF0FE8Startup final {
public:
    // 原函数：0x00467E70；只接管同组原始载荷的启动寿命。
    WorldMoveStateAF0FE8Startup();
    ~WorldMoveStateAF0FE8Startup();
    WorldMoveStateAF0FE8Startup(const WorldMoveStateAF0FE8Startup&) = delete;
    WorldMoveStateAF0FE8Startup& operator=(const WorldMoveStateAF0FE8Startup&) = delete;
};

// 原函数：0x00467EB0。
class WorldMoveStateC821D8Startup final {
public:
    // 原函数：0x00467EB0；只接管同组原始载荷的启动寿命。
    WorldMoveStateC821D8Startup();
    ~WorldMoveStateC821D8Startup();
    WorldMoveStateC821D8Startup(const WorldMoveStateC821D8Startup&) = delete;
    WorldMoveStateC821D8Startup& operator=(const WorldMoveStateC821D8Startup&) = delete;
};

// 原函数：0x00467EF0。
class RippleGateStartup final {
public:
    // 原函数：0x00467EF0；只接管同组原始载荷的启动寿命。
    RippleGateStartup();
    ~RippleGateStartup();
    RippleGateStartup(const RippleGateStartup&) = delete;
    RippleGateStartup& operator=(const RippleGateStartup&) = delete;
};

// 原5184A0/518410，玩家六组CryptInt；排在鼠标构造之后。
// 对应原CRT 5530FC/553100：玩家六值、主宠物五条六值，两个拥有者独立构析。
// 原函数：0x005183D0。
class PlayerRuntimeValuesStartup final {
public:
    // 原函数：0x005183D0；只接管同组原始载荷的启动寿命。
    PlayerRuntimeValuesStartup();
    ~PlayerRuntimeValuesStartup();
    PlayerRuntimeValuesStartup(const PlayerRuntimeValuesStartup&) = delete;
    PlayerRuntimeValuesStartup& operator=(const PlayerRuntimeValuesStartup&) = delete;
};

// 原函数：0x00518520。
class PetRuntimeValuesStartup final {
public:
    // 原函数：0x00518520；只接管同组原始载荷的启动寿命。
    PetRuntimeValuesStartup();
    ~PetRuntimeValuesStartup();
    PetRuntimeValuesStartup(const PetRuntimeValuesStartup&) = delete;
    PetRuntimeValuesStartup& operator=(const PetRuntimeValuesStartup&) = delete;
};
void ConstructPlayerRuntimeValues();
void DestroyPlayerRuntimeValues();
// 原518530/518560，五条宠物各六组CryptInt；排在玩家构造之后。
void ConstructPetRuntimeValues();
// 原函数：0x00518560。
void DestroyPetRuntimeValues();
// 原51C410：仅校验五槽索引，不检查宠物used。
// SLC01：原51C410只判断S32索引0..4；已证八调用只消费低W8结果。
bool IsValidPetIndex_51C410(std::int32_t index) noexcept;

// 原 sub_448240@0x448240；只采样一次 gMapFloor。
std::int32_t IsMapFloorAllowed_448240() noexcept;

// 原 sub_4493C0@0x4493C0；只采样一次 gMapFloor，判定闭区间及四项表，返回 0/1。
std::int32_t Sub_4493C0() noexcept;

// 原 sub_46CE30@0x46CE30；X/Y 各写四字节，返回借用的 outX，不检查输出指针。
float* ProjectWorldToScreen_46CE30(
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

// 原 World::Update@0x51C430；场景状态消费与分发，实现在WorldUpdate.cpp，依赖按所属模块接入。
void Update();

} // namespace cg::world
