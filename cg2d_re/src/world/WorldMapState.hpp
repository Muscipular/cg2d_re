#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"

// 地图、路径与地图效果的共享声明。

namespace cg::render {
struct GraphicDescriptor16;
}

namespace cg::world {

// 原 46BD60/46BE50/46DFF0：世界坐标为 F32 原位型，描述符同步借用。
std::int32_t CachePrimaryMapGraphic_46BD60(
    std::int32_t imageNo, std::uint32_t worldXBits, std::uint32_t worldYBits,
    std::int16_t x, std::int16_t y, std::int32_t mode,
    const render::GraphicDescriptor16* descriptor);
// 原函数：0x0046DFF0。
void CacheMapObjectGraphic_46DFF0(
    std::int32_t imageNo, std::uint32_t worldXBits, std::uint32_t worldYBits,
    std::int16_t x, std::int16_t y, const render::GraphicDescriptor16* descriptor);
// 原 46C060：挂接主项的前绘/后绘链；batch 保留低 W8，不布尔化。
void DrawGearNormal_46C060(
    std::int32_t primaryIndex, std::int32_t before, std::int32_t imageNo,
    std::int16_t x, std::int16_t y,
    const render::GraphicDescriptor16* descriptor, std::uint8_t batchType);

// 原 0xC3500C/0xC75010/0x9EE8A0/0x9ED438；468F30 按序清零的独立 W32 槽。
extern std::uint32_t gValueC3500C; // 原全局：0x00C3500C。
extern std::uint32_t gValueC75010; // 原全局：0x00C75010。
extern std::uint32_t gValue9A86DC; // 原全局：0x009A86DC。

// 原 9DEF64/9DEF60/9DEF62/9DEF5E 与 9A86E0/9ED43C；地图窗口的六个 S16 槽。
extern std::int16_t gMapXStart; // 原全局：0x009DEF64。
extern std::int16_t gMapYStart; // 原全局：0x009DEF60。
extern std::int16_t gMapSpanX; // 原全局：0x009A86E0。
extern std::int16_t gMapSpanY; // 原全局：0x009ED43C。
extern platform::LegacyCryptInt gPlayerX; // 原 9EE8A8；468200 等共享。
extern platform::LegacyCryptInt gPlayerY; // 原 9EE8C0；468200 等共享。
extern platform::LegacyCryptInt gPlayerSnapshotX; // 原 C751C0；4681B0 读取，中央启动位置直接构造。
extern platform::LegacyCryptInt gPlayerSnapshotY; // 原 C751B0；4681B0 读取，中央启动位置直接构造。
extern platform::LegacyCryptInt gValue9EE8D0; // 原全局：0x009EE8D0。
extern platform::LegacyCryptInt gValue9EE8E0; // 原全局：0x009EE8E0。
extern platform::LegacyCryptInt gValueC27FF8; // 原全局：0x00C27FF8。
extern platform::LegacyCryptInt gValueC27FE8; // 原全局：0x00C27FE8。
extern platform::LegacyCryptInt gMoveTargetX9A7F88; // 原全局：0x009A7F88。 原玩家移动目标 X。
extern platform::LegacyCryptInt gMoveTargetY9A7F98; // 原全局：0x009A7F98。 原玩家移动目标 Y。
extern platform::LegacyCryptInt gValueC821E8; // 原全局：0x00C821E8。
extern platform::LegacyCryptInt gValue9E03C0; // 原全局：0x009E03C0。
extern platform::LegacyCryptInt gValue9FB918; // 原全局：0x009FB918。
extern platform::LegacyCryptInt gValueAF0FE8; // 原全局：0x00AF0FE8。
extern platform::LegacyCryptInt gValueC821D8; // 原全局：0x00C821D8。
extern std::uint32_t gPartyCatchUpRateBits_9DAD78; // 原全局：0x009DAD78。 原 F32 位槽，移动/绘制链共享。

// 借用SceneState唯一方向表，分别读取原55D9DC/55D9DE的S16分量。
std::int16_t ReadDirectionDeltaX(std::int16_t direction) noexcept;
std::int16_t ReadDirectionDeltaY(std::int16_t direction) noexcept;
extern std::uint16_t gMovePathCount_C82200; // 原全局：0x00C82200。
extern std::uint16_t gPendingDirectionCount_C82202; // 原全局：0x00C82202。
extern std::uint32_t gValueC821FC; // 原全局：0x00C821FC。
extern std::uint16_t gValue9A86D8; // 原全局：0x009A86D8。
extern std::uint16_t gValue9FB92E; // 原全局：0x009FB92E。
extern std::uint16_t gValueC82220; // 原全局：0x00C82220。
extern std::uint32_t gValueC8A77C; // 原全局：0x00C8A77C。 UI与目标格输入共用的W32许可槽。
extern std::uint32_t gValueC8A780; // 原全局：0x00C8A780。 UI与目标格绘制共用的另一W32许可槽。
extern std::int32_t gValue73ED48; // 原全局：0x0073ED48。 原时间类别/调色板索引 W32，50C870 与 458F00 共用。
extern std::uint32_t& gMapEffectOffsetXBits; // 原C8A758；别名既有场景槽，4483A0写、46F1D0读。
extern std::uint32_t& gMapEffectOffsetYBits; // 原C8A75C；保留F32位型及独立写序。
extern std::uint32_t& gBattleSceneEffectC8A768; // 原全局：0x00C8A768。 413070按两W16写入，别名既有场景效果owner。
extern std::uint32_t& gBattleSceneEffectC8A76C; // 原全局：0x00C8A76C。 同源W32，不能拆成独立高/低槽。
// 原 AEFB94/9DDB0C/9ED44C；468C40 的 tile/obj/hit 共享平面。
inline constexpr std::size_t kMapBufferCells = 2601;
extern std::array<std::uint16_t, kMapBufferCells> gMapTileArray;
extern std::array<std::uint16_t, kMapBufferCells> gMapObjArray;
extern std::array<std::uint16_t, kMapBufferCells> gMapHitArray;
extern std::array<std::uint16_t, kMapBufferCells> gMapWalkabilityArray; // 原 9DEF68；46D9C0 生产。
extern std::int32_t gValue9E0434; // 原 9E0434；50C870 写，地图路径/保存共读。

// 原 468520；调用方传入 9E0434 当前值，返回 sprintf 的完整结果。
std::int32_t __cdecl FormatMapDataPath_468520(
    char* buffer, std::int32_t mapId, std::int32_t value9E0434, std::int32_t floor);
extern std::uint32_t gValue9DAD68; // 原全局：0x009DAD68。

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

extern AutRecordAF10D8 gAutDataAF10D8[159201]; // 原全局：0x00AF10D8。
// 原函数：0x004681B0。
std::int32_t __cdecl RestorePlayerMovementSnapshot_4681B0();         // 原 [0x4681B0,0x4681F6)，快照恢复后清六槽。
// 原函数：0x00469810。
std::int32_t __cdecl IsMapCellBlocked_469810(std::int32_t x, std::int32_t y); // 原 [0x469810,0x469864)，边界外或 walk WORD==1。
// 原函数：0x0050CF90。
void __cdecl HandleMapTransitionReply_50CF90(
    std::uintptr_t socket, std::int32_t sequence,
    std::int32_t status); // 原 [0x50CF90,0x50CFF7)，地图回复序号与状态清理。
// 原函数：0x00509090。
void __cdecl HandleMapRectangleCrcReply_509090(
    std::uintptr_t socket, std::int32_t first,
    std::int32_t floor, std::int32_t xStart,
    std::int32_t yStart, std::int32_t xEnd,
    std::int32_t yEnd, std::int32_t expectedTile,
    std::int32_t expectedObj, std::int32_t expectedHit); // 原 [0x509090,0x5090FC)，地图校验回复回调。
// 原函数：0x00509100。
std::int32_t __cdecl HandleMapRectangleDataReply_509100(
    std::uintptr_t socket, std::int32_t mapId,
    std::int32_t floor, std::int32_t xStart,
    std::int32_t yStart, std::int32_t xEnd,
    std::int32_t yEnd, const char* payload); // 原 [0x509100,0x5093B6)，连续局部帧地图增量回复。
// 原函数：0x005093F0。
std::int32_t __cdecl HandleMapChangeReply_5093F0(
    std::uintptr_t a1, std::int32_t a2, std::int32_t a3,
    std::int32_t a4, std::int32_t a5, std::int32_t a6,
    std::int32_t a7, std::int32_t a8, std::int32_t a9,
    std::int32_t a10, std::int32_t a11, std::int32_t a12,
    std::int32_t a13); // 原 [0x5093F0,0x509548)，地图保存及场景状态发布。
// 原函数：0x00509550。
std::int32_t __cdecl HandleMapNameReply_509550(
    std::uintptr_t socket, char* payload); // PTR01：原完整回复，地图名整词/状态/尾字节及解析持值顺序保持。
// 原 46C760：按地图模式和重建标志提交同一窗口，返回完整绘制结果。
std::int32_t DrawCurrentAutoMap_46C760(std::int32_t x, std::int32_t y);
// 原 [46CF60,46D9B9)：完整两路地图绘制；参数只控制普通地图标记，重建场景10实传0。
void __cdecl DrawMapScene_46CF60(std::int32_t value);
// 原函数：0x0046CF00。
std::int32_t __cdecl ResolveMapGraphicNumber_46CF00(std::int32_t value); // 原 [0x46CF00,0x46CF1A)，读已初始化的共享索引表。
// 原函数：0x0046F8F0。
void __cdecl NormalizeAngleDegrees_46F8F0(float* value);      // WDG01：非拥有 F32 指针，比较/更新分别取样，重复归一到 [0,360)。
// 原函数：0x0046F930。
double __cdecl LookupSineDegrees_46F930(float angle);     // 原 [0x46F930,0x46F94E)，截断后查 360 项 F64 原表。
// 原函数：0x0046F950。
double __cdecl LookupCosineDegrees_46F950(float angle);     // 原 [0x46F950,0x46F967)，F32 加 90 后调用 46F930。
// 原函数：0x0046F970。
double __cdecl GetVectorAngleDegrees_46F970(float first, float second); // 原 [0x46F970,0x46FACF)，65 项原表插值。
// 原函数：0x0046CDF0。
std::int64_t __cdecl GetEightWayMapDirection_46CDF0(float first, float second); // WDG01：原 F32 偏移/落点/乘数与归一化，正常转换返回 S64。
// 原函数：0x0050A3E0。
void HandlePartyMemberReply_50A3E0(std::uintptr_t socket, std::int32_t index, const char* payload);
// 原函数：0x0050D000。
void HandlePartyMembershipReply_50D000(
    std::uintptr_t socket, std::int32_t first, std::int32_t second); // 跟随状态回复。
// 原函数：0x005093C0。
void HandlePlayerPositionReply_5093C0(
    std::uintptr_t socket, std::int32_t x, std::int32_t y,
    std::int32_t direction); // 原坐标回复，无门禁，方向最后发布。
// 原函数：0x0050D080。
void Sub_50D080(
    std::uintptr_t socket, std::int32_t first, std::int32_t second); // 两次CryptInt赋值后复制首值。
// 原[C82258,C8A728)共500项，44H载荷及两链接；只拓宽链接，生产和渲染共享同一记录。
struct MapEffectRecord {
    std::int32_t x; // +00：当前位置。
    std::int32_t y; // +04：当前位置。
    std::int32_t value08; // +08：类别1/2/4的X速度，类别6/7/8的定点X。
    std::int32_t value0C; // +0C：类别1/2/4的Y速度，类别6/7/8的定点Y。
    std::int32_t speed; // +10：类别6/7/8的运动速度。
    std::int32_t graphicIndex; // +14：类别6/7/8的图形参数。
    std::array<std::byte, 8> reserved18; // +18：尚无字段依据。
    std::int32_t value20; // +20：类别4绘制局部X。
    std::int32_t value24; // +24：类别4绘制局部Y。
    std::int32_t value28; // +28：类别4绘制宽度。
    std::int32_t value2C; // +2C：类别4更新写入值，渲染作为高度消费。
    std::uint32_t remaining; // +30：类别4更新倒计数。
    std::array<std::byte, 8> reserved34; // +34：尚无字段依据。
    std::uint16_t kind; // +3C：效果类别。
    std::uint16_t phase; // +3E：类别4的颜色阶段。
    std::uint16_t variant; // +40：图案／颜色分支。
    std::array<std::byte, 2> reserved42; // +42：保留原字节。
    MapEffectRecord* prev; // 原+44，x64 +48：活动／空闲链前项。
    MapEffectRecord* next; // 原+48，x64 +50：活动／空闲链后项。
};
static_assert(std::is_standard_layout_v<MapEffectRecord>);
static_assert(std::is_trivially_copyable_v<MapEffectRecord>);
static_assert(offsetof(MapEffectRecord, x) == 0x0);
static_assert(offsetof(MapEffectRecord, y) == 0x4);
static_assert(offsetof(MapEffectRecord, value08) == 0x8);
static_assert(offsetof(MapEffectRecord, value0C) == 0xC);
static_assert(offsetof(MapEffectRecord, speed) == 0x10);
static_assert(offsetof(MapEffectRecord, graphicIndex) == 0x14);
static_assert(offsetof(MapEffectRecord, reserved18) == 0x18);
static_assert(offsetof(MapEffectRecord, value20) == 0x20);
static_assert(offsetof(MapEffectRecord, value24) == 0x24);
static_assert(offsetof(MapEffectRecord, value28) == 0x28);
static_assert(offsetof(MapEffectRecord, value2C) == 0x2C);
static_assert(offsetof(MapEffectRecord, remaining) == 0x30);
static_assert(offsetof(MapEffectRecord, reserved34) == 0x34);
static_assert(offsetof(MapEffectRecord, kind) == 0x3C);
static_assert(offsetof(MapEffectRecord, phase) == 0x3E);
static_assert(offsetof(MapEffectRecord, variant) == 0x40);
static_assert(offsetof(MapEffectRecord, reserved42) == 0x42);
static_assert(offsetof(MapEffectRecord, prev) == 0x48);
static_assert(offsetof(MapEffectRecord, next) == 0x50);
static_assert(sizeof(MapEffectRecord) == 0x58);
static_assert(alignof(MapEffectRecord) == 8);
extern MapEffectRecord* gActiveMapEffectHead; // 原0x00C8A72C；唯一活动链首。
// 原函数：0x0046E480。
void DrawPendingMapEffects_46E480();                          // 原 0x46E480；非零提交标志才绘制，调用后无条件清 W16。
// 原函数：0x0046BD40。
std::int32_t ResetMapDrawCacheCounts_46BD40();                 // 原 [0x46BD40,0x46BD57)，依次清四个独立 W32 槽并返回 0。
// 原 46FB80：先X差再Y差，返回Y平方+X平方的F64值。
double GetSquaredWorldDistance_46FB80(float x, float y, float targetX, float targetY);

namespace detail {

// 原四个地图绘制源计数，仍由 SceneState 持有并由 46BD40 逐项清零。
extern std::uint32_t gPrimaryMapGraphicCount_9FB934; // 原全局：0x009FB934。
extern std::uint32_t gMapObjectGraphicCount_9FB928; // 原全局：0x009FB928。
extern std::uint32_t gSpecialMapGraphicCount_9A86C0; // 原全局：0x009A86C0。
extern std::uint32_t gValue9A7F78; // 原全局：0x009A7F78。
// 原函数：0x0046BE50。
void CacheMapGraphicByResourceType_46BE50(
    std::int32_t imageNo, std::uint32_t worldXBits, std::uint32_t worldYBits,
    std::int16_t x, std::int16_t y, const render::GraphicDescriptor16* descriptor);
// 原 46C2F0：排序→合并，caller 不消费混合 EAX；46C1C0 返回末次合并计数。
void SortAndMergeMapDrawCache_46C2F0();
// 原函数：0x0046C1C0。
std::int32_t QueueMergedMapDrawCache_46C1C0();

// 原 0xC8222E；46A700 写 1，469FC0 清 0，46AB60 读取低 W16。
extern std::uint16_t gValueC8222E;

// 原 0xC82230/0xC82234/0xC82236；46A9E0 清零，后续 46AA00/46AB60 共用。
extern std::uint32_t gPendingPathEventActive_C82230; // 原全局：0x00C82230。
extern std::int16_t gPendingPathEventSteps_C82234; // 原全局：0x00C82234。
extern std::int16_t gPendingPathEventType_C82236; // 原全局：0x00C82236。
extern std::uint32_t gValue9EE8A0; // 原全局：0x009EE8A0。
extern std::uint32_t gValue9ED438; // 原全局：0x009ED438。
extern std::uint32_t gValue9ED440; // 原全局：0x009ED440。 原 46AA00 写、46AEA0 读取的独立 W32。

// 原 0xC35008/0x9DAD2C/0x9FB930/0xC821D4/0x9A86DC；4682E0 顺序写入。
extern std::uint32_t gValueC35008; // 原全局：0x00C35008。
extern std::uint32_t gValue9DAD2C; // 原全局：0x009DAD2C。
extern std::uint32_t gMapWidth_9FB930; // 原全局：0x009FB930。
extern std::uint32_t gMapHeight_C821D4; // 原全局：0x00C821D4。
extern std::int16_t gMapXEnd; // 原全局：0x009DEF62。
extern std::int16_t gMapYEnd; // 原全局：0x009DEF5E。
extern std::uint32_t gValueAF10C0; // 原全局：0x00AF10C0。 原 46CEE0 写入的完整 W32。
extern std::uint32_t gValue9EE8F8; // 原全局：0x009EE8F8。 原 509460 写入、50C8D0 读取的独立 W32。
extern std::uint32_t gPlayerMovementStepXBits_9EE8FC; // 原全局：0x009EE8FC。 原 F32 位槽，移动链共享。
extern std::uint32_t gPlayerMovementStepYBits_9EE900; // 原全局：0x009EE900。 原 F32 位槽，移动链共享。

// 原 467F30 独立 W8/W16/W32 写入目标；不覆盖邻接的 C82204。
extern std::uint8_t gValue9A4B94; // 原全局：0x009A4B94。
// 原 9A86D4 起始的方向 W16；已证调用链最多访问前两项。
extern std::array<std::uint16_t, 2> gPendingDirections9A86D4;
// 原 AF0FF8/C75014/C750DC；三组 100 项 W16 路径队列共用 C82200 数量。
inline constexpr std::size_t kMovePathCapacity = 100;
extern std::array<std::uint16_t, kMovePathCapacity> gMovePathDirections;
extern std::array<std::uint16_t, kMovePathCapacity> gMovePathX;
extern std::array<std::uint16_t, kMovePathCapacity> gMovePathY;
extern std::uint32_t gValue9EE8B8; // 原全局：0x009EE8B8。
extern std::uint16_t gValue9EE8A4; // 原全局：0x009EE8A4。 46AF30 发布方向，46CF60 按 S16 重用探测。
extern std::uint16_t gValueC8A79C; // 原全局：0x00C8A79C。
extern std::uint16_t gValueB65A7C; // 原全局：0x00B65A7C。
extern std::uint64_t gValueC82238; // 原全局：0x00C82238。 46B370目标格转向时间，与C8223C独立。
extern std::uint64_t gValueC8223C; // 原全局：0x00C8223C。 CTM01转向发送的本地64位毫秒。
extern std::uint32_t gValueC8A774; // 原全局：0x00C8A774。 原左键目标事件门禁。
extern std::uint64_t gValueC8A778; // 原全局：0x00C8A778。
extern std::int32_t gPlayerMoveTargetX; // 原C8A784，RendererCursorGrid发布的S32。
extern std::int32_t gPlayerMoveTargetY; // 原C8A788，不是目标CryptInt或当前格坐标。
extern std::int32_t gMouseWorldPixelX_C8A78C; // 原全局：0x00C8A78C。 原鼠标世界像素X。
extern std::int32_t gMouseWorldPixelY_C8A790; // 原全局：0x00C8A790。 原鼠标世界像素Y。
extern std::uint64_t gValueC8A794; // 原全局：0x00C8A794。
extern std::uint32_t gMapTargetLeftClick_C8A7A0; // 原全局：0x00C8A7A0。 原46F680目标格左键单次事件。
extern std::uint32_t gValueC8A7A4; // 原全局：0x00C8A7A4。 原目标格左键持续事件。
extern std::uint32_t gMapTargetInteractionClick_C8A7A8; // 原全局：0x00C8A7A8。 原目标对象交互事件，非鼠标edge位槽。
extern std::uint32_t gValueC8A7AC; // 原全局：0x00C8A7AC。 原右键持续事件，单次事件优先。
extern std::uint64_t gValue55DDAC; // 原全局：0x0055DDAC。 原点击目标时间，映像初值FFFFFFFF。
extern std::uint32_t gValueC82208; // 原全局：0x00C82208。 原地图请求 W32；50CF90、509090、509100 等共享。
extern std::uint32_t gValueC8220C; // 原全局：0x00C8220C。
extern std::uint32_t gValueC82210; // 原全局：0x00C82210。
extern std::uint32_t gValueC82214; // 原全局：0x00C82214。
extern std::uint32_t gValueC82218; // 原全局：0x00C82218。
extern std::int32_t gValueFF0D4C; // 原全局：0x00FF0D4C。 原地图回复数值 W32，51D6F0 消费。
extern std::int32_t gValue73ED44; // 原全局：0x0073ED44。 原当前调色板选择标志 W32，458F00 读取。
extern std::uint32_t gValueC82224; // 原全局：0x00C82224。
extern std::uint32_t gValueC8221C; // 原全局：0x00C8221C。 原 46ADA0/46AEA0 共用的 W32 请求序号。
extern std::uint16_t gValue9DAD54; // 原全局：0x009DAD54。
extern std::uint16_t gValue9FB92C; // 原全局：0x009FB92C。
extern std::uint16_t gValue55D9FC; // 原全局：0x0055D9FC。
extern std::uint16_t gValueC821D0; // 原全局：0x00C821D0。
extern std::uint16_t gValue9EE910; // 原全局：0x009EE910。

extern std::uint16_t gLastDetectedCode; // 原 9DAD4A；不随 pending 清理。
// 原函数：0x00468570。
std::int32_t __cdecl EnsureMapCacheFile_468570(
    std::int32_t a1, std::int32_t a2, std::int32_t a3,
    std::int32_t a4, std::int32_t a5, std::int32_t a6,
    std::int32_t a7); // 原 [0x468570,0x4687A3)，旧文件比较或新建。
// 原函数：0x004687B0。
std::int32_t __cdecl CreateEmptyMapCacheFile_4687B0(
    const char* path, std::int32_t first, std::int32_t second,
    std::int32_t height, std::int32_t width); // 原 [0x4687B0,0x4688A0)，三平面零值文件创建叶。
// 原函数：0x004688A0。
std::int32_t __cdecl WriteMapCacheRectangle_4688A0(
    std::int32_t mapId, std::int32_t floor,
    std::int32_t xStart, std::int32_t yStart,
    std::int32_t xEnd, std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    std::uint16_t* hit); // 原 [0x4688A0,0x468BF5)，增量写三平面文件及共享 hit。
// 原函数：0x00469870。
std::int32_t FindMissingMapRectanglesAhead_469870(std::int32_t direction); // 原 [0x469870,0x469C87)，四向探测依赖。
// 原函数：0x0046AF30。
std::uint32_t RequestMissingMapRectanglesAhead_46AF30(); // 原 [0x46AF30,0x46AFDE)，探测后发送 UUN 和发布状态。
// 原函数：0x00468C40。
std::int32_t __cdecl ReadMapCacheRectangle_468C40(
    std::int32_t mapId, std::int32_t floor,
    std::int32_t xStart, std::int32_t yStart,
    std::int32_t xEnd, std::int32_t yEnd,
    std::uint16_t* tile, std::uint16_t* obj,
    std::uint16_t* hit); // 原 [0x468C40,0x468F25)，真实三平面文件装载依赖。

// 原 0x9DAD5C/0x9DAD68/0x9DAD58/0x9DAD60/0x9DAD64；46DA80 共享状态。
extern std::uint32_t gValue9DAD5C; // 原全局：0x009DAD5C。
extern std::int32_t gValue9DAD58; // 原全局：0x009DAD58。
extern std::int32_t gValue9DAD60; // 原全局：0x009DAD60。
extern std::int32_t gValue9DAD64; // 原全局：0x009DAD64。

// 原 [0x9FB954,0xAEFB94)：.cut 记录 50,000 项，每项仅前 18 字节由文件读取。
struct CutRecord9FB954 {
    std::int32_t value00; // 原成员偏移：+0x00。 图号；46CF60 的 tile 直接使用，静态 obj 先查 mapNo。
    std::int16_t value04; // 原成员偏移：+0x04。 obj 动画标志；0 静态，1 Yobi，其余跳过。
    std::int16_t value06; // 原成员偏移：+0x06。 裁剪 X。
    std::int16_t value08; // 原成员偏移：+0x08。 裁剪 Y。
    std::int16_t value0A; // 原成员偏移：+0x0A。 宽。
    std::int16_t value0C; // 原成员偏移：+0x0C。 高。
    std::int16_t value0E; // 原成员偏移：+0x0E。 绘制偏移 X。
    std::int16_t value10; // 原成员偏移：+0x10。 绘制偏移 Y。
    std::uint8_t reserved12[2]; // 原成员偏移：+0x12。
};

// 原 [0x9FB948,0xAEFB94)：十二字节头与 50,000 项定长记录的唯一共享数据。
struct CutData9FB948 {
    std::uint8_t unknown00[6]; // 原成员偏移：+0x00。
    std::int16_t count06; // 原成员偏移：+0x06。
    std::int16_t count08; // 原成员偏移：+0x08。
    std::int16_t count0A; // 原成员偏移：+0x0A。
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

extern CutData9FB948 gCutData9FB948; // 原全局：0x009FB948。

// 原 [0xC35010, 0xC75010)；46CF20 顺序填值，C75010 尾槽另存。
extern std::int32_t gValuesC35010[65536]; // 原全局：0x00C35010。

// 原函数：0x004682E0。
void __cdecl SetWorldMapMetadata_4682E0(
    std::uint32_t a1,
    std::int32_t a2,
    std::uint32_t a3,
    std::uint32_t a4,
    std::uint32_t a5,
    std::uint32_t a6,
    std::uint32_t a7,
    std::int32_t a8); // 原 [0x4682E0,0x468330)，六槽赋值与参数分发。
// 原函数：0x0046DA80。
void __cdecl InitializeMapSceneResources_46DA80(
    std::uint32_t a1,
    std::int32_t a2,
    std::int32_t a3,
    std::uint32_t a4); // 原 [0x46DA80,0x46DAF5)，音频重置与模式分支。
// 原函数：0x0046DB00。
std::int32_t __cdecl LoadCutMapData_46DB00(std::int32_t selected); // 原 [0x46DB00,0x46DC55)，.cut 加载器。
// 原函数：0x0046DC60。
void __cdecl ResetCutMapData_46DC60(); // 原 [0x46DC60,0x46DC85)，先整块清零再逐项写哨兵。
// 原函数：0x0046DC90。
std::int32_t __cdecl LoadAutoMapColorData_46DC90(std::int32_t selected); // 原 [0x46DC90,0x46DD35)，.aut 加载器。
// 原函数：0x0046A9E0。
std::int32_t __cdecl ResetPendingPathEvent_46A9E0();         // 原 [0x46A9E0,0x46A9F4)，清三槽、返回 0。
// 原函数：0x0046AA00。
std::int32_t __cdecl DetectPendingPathEvents_46AA00();         // PDH01：路径方向/两整数差与 F32 桥，保 S32 早退 Y 和 pending 发布。
// 原函数：0x0046BC90。
void __cdecl PopPlayerMovementPath_46BC90();                // 原 [0x46BC90,0x46BCE4)，三组路径 W16 同步左移。
// 原函数：0x0046ABD0。
void __cdecl QueueNextMovementDirections_46ABD0();                // 原 [0x46ABD0,0x46AC0A)，最多搬运两项方向。
// 原函数：0x0046AC10。
std::uint16_t SendPendingMovementDirections_46AC10(); // 方向文本生成/发送，仅承接原AX返回位型。
// 原函数：0x0046B370。
void TurnPlayerTowardCursorTarget_46B370(); // 目标格转向，保留重复CryptInt读取与独立500ms节流。
// 原函数：0x0046BCF0。
void __cdecl PopPendingMovementDirection_46BCF0();                // 原 [0x46BCF0,0x46BD1D)，方向计数递减后左移。
// 原函数：0x00467F30。
std::int32_t* InitializeWorldMapState_467F30();                // 原 [0x467F30,0x4681AB)，透传 46CF20 一过尾指针。
// 原函数：0x00468200。
std::uint16_t __cdecl UpdatePlayerMapWindow_468200();        // 原 [0x468200,0x46828D)，仅返回 AX 位模式。
// 原函数：0x0046CEC0。
std::int32_t __cdecl GetPlayerMapCoordinateSum_46CEC0();         // 原 [0x46CEC0,0x46CED6)，先读 Y 再对 X 加。
// 原函数：0x0046CEE0。
std::int32_t __cdecl StorePlayerMapCoordinateSum_46CEE0();         // 原 [0x46CEE0,0x46CEFB)，并写 AF10C0。
// 原函数：0x00468330。
std::uint32_t __cdecl SetPlayerMapPosition_468330(
    std::int32_t x, std::int32_t y);        // 原 [0x468330,0x468511)，返回 Y 浮点位型。
// 原函数：0x00468290。
std::int16_t* __cdecl ClipMapRectangleToBounds_468290(
    std::int16_t* xStart, std::int16_t* yStart,
    std::int16_t* xEnd, std::int16_t* yEnd); // 原 [0x468290,0x4682D7)，原地裁剪并返回末指针。
// 原函数：0x00468C00。
std::int32_t __cdecl SetMapCellHitValue_468C00(
    std::int32_t x, std::int32_t y, std::uint16_t value); // 原 [0x468C00,0x468C32)，写 hit WORD。
// 原函数：0x0046AE60。
std::int32_t __cdecl IsDirectionalMapTransitionCell_46AE60(std::int32_t x, std::int32_t y); // MHT01：原边界/索引序与命中低 BYTE，返 S32 0/1。
// 原函数：0x00469240。
std::int32_t __cdecl CalculateMapDataCrc_469240(
    const std::uint8_t* bytes, std::int32_t length); // 原 [0x469240,0x469284)，三平面校验的 W16 查表叶。
// 原函数：0x004691D0。
void __cdecl InvalidateMapDrawPosition_4691D0(); // 原 [0x4691D0,0x469239)，两次 -1 临时赋值给地图状态。
// 原函数：0x00469290。
std::int32_t __cdecl CheckMapRectangleCrcAndRequest_469290(
    std::int32_t first, std::int32_t floor,
    std::int32_t xStart, std::int32_t yStart,
    std::int32_t xEnd, std::int32_t yEnd,
    std::int32_t expectedTile, std::int32_t expectedObj,
    std::int32_t expectedHit); // 原 [0x469290,0x4693E9)，真实局部三平面校验依赖。
// 原函数：0x0051D6F0。
void __cdecl ApplyPendingMapPaletteReply_51D6F0(); // PTR01：原完整消费，原-1/非负分支、调色板副作用后发布保持。
// 原函数：0x00469030。
std::int32_t __cdecl LoadAndDrawMapTiles_469030(); // 原 [0x469030,0x4691C9)，地图装载、第四平面与斜向绘制。
extern std::uint32_t gValue55DA88; // 原全局：0x0055DA88。 原地图窗口重建标志，初值 1；46C760/46CF60 共用。
// 原 46C840：三平面窗口读取及两类地图颜色生产；不改变玩家坐标。
std::int32_t LoadAutoMapWindow_46C840(std::int32_t floor, std::int32_t playerX, std::int32_t playerY);
// 原469020仅传标记参数1；43CB10完成战斗背景快照并更新5ABCD8。
void DrawMapSceneWithMarkers_469020();
// 原 [0x4693F0,0x4697C9)：先扫 tile，再扫 obj/图形尺寸，输出第四地图平面。
std::int32_t __cdecl BuildGraphicMapWalkability_4693F0(
    std::int32_t xStart, std::int32_t yStart, std::int32_t xEnd, std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk);
// 原 [0x46D9C0,0x46DA72)：模式 1 读取 hit 高字节 bit0，其他模式转 4693F0。
std::int32_t __cdecl BuildMapWalkability_46D9C0(
    std::int32_t xStart, std::int32_t yStart, std::int32_t xEnd, std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk);
// 原函数：0x0046AD00。
std::int32_t __cdecl IsMapTransitionCellActive_46AD00(std::int32_t x, std::int32_t y); // MHT01：time 后跨度/低 BYTE，保 W16 分类发布和原短路序。
// 原函数：0x00468F30。
std::int32_t __cdecl ResetMapProjectionState_468F30();         // 原 [0x468F30,0x468F6F)，按序清十二槽、返回 0。
// 原函数：0x00468F70。
std::int64_t __cdecl InitializeMapProjectionOrigin_468F70();         // 原 [0x468F70,0x468FE4)，F32 投影、向零截断后写六槽。
// 原函数：0x0046CF20。
std::int32_t* __cdecl InitializeMapGraphicNumberTable_46CF20();        // 原 [0x46CF20,0x46CF57)，填表并返回一过尾指针。
// 原函数：0x0046B580。
std::int64_t __cdecl GetMapDirectionBetweenPoints_46B580(
    std::int32_t first, std::int32_t second,
    std::int32_t third, std::int32_t fourth); // PDH01：两整数差先取齐，Y/X 各落 F32，原方向 S64 透传。
// 原函数：0x0046B5C0。
std::int32_t __cdecl MergePathTailIntoDiagonalStep_46B5C0(
    std::int32_t x, std::int32_t y,
    std::int32_t firstX, std::int32_t firstY,
    std::int32_t secondX, std::int32_t secondY); // 原 [0x46B5C0,0x46B6F0)，探测两邻格并覆写路径末项。
// 原函数：0x0046B6F0。
std::int32_t BuildPlayerMovementPath_46B6F0(); // 完整候选/绕行/轴补走路径生成，复用三组W16路径。
// 原函数：0x0046BD20。
std::uint32_t __cdecl EncodeMovementDirectionByte_46BD20(
    std::int32_t value, std::int32_t mode); // 原 [0x46BD20,0x46BD32)，仅改 AL，保留 mode 高 24 位。
// 原函数：0x0050C8D0。
std::int32_t __cdecl SendCurrentPlayerMovementPath_50C8D0(
    std::int32_t x, std::int32_t y,
    const char* text); // 原 [0x50C8D0,0x50C8F5)，透传 zA 请求返回。
void ResetMapFollowersFromPath(cg::yobi::YobiAction* leader); // 444410仅扫描占用的地图槽1..4。
// 原函数：0x0046AFE0。
void PropagatePartyFollowerPath_46AFE0(
    std::int32_t index, std::int32_t x, std::int32_t y); // 移除后的五槽路径续接。
// 原函数：0x0046ADA0。
void* __cdecl RequestDetectedMapTransition_46ADA0();               // 原 [0x46ADA0,0x46AE53)，仅透传 51D5B0 的返回位型。
// 原函数：0x0046AC90。
std::int32_t __cdecl RequestPlayerCoordinateSynchronization_46AC90();        // 原 [0x46AC90,0x46AD00)，type 2 的真实后继。
// 原函数：0x0046AEA0。
std::int32_t __cdecl RequestDirectionalMapTransition_46AEA0();        // 原 [0x46AEA0,0x46AF24)，type 3 的真实后继。
// 原函数：0x0046AB60。
std::int32_t __cdecl DispatchPendingPathEvent_46AB60();        // 原 [0x46AB60,0x46ABCD)，pending 倒计时与分发。
// render 只借用活动链及已证数值字段；不公开记录布局，不截断原生 next。
// 原函数：0x0046E2A0。
MapEffectRecord* InitializeMapEffectPool_46E2A0();             // 原 0x46E2A0；返回末项非拥有指针。
// 原函数：0x0046E340。
MapEffectRecord* AcquireMapEffectRecord_46E340();             // 原 0x46E340；空闲首项转为活动首项。
// 原函数：0x0046E390。
MapEffectRecord* ReleaseMapEffectRecord_46E390(MapEffectRecord* record); // 原 0x46E390；摘活动链后退回空闲头。
// 原函数：0x0046E3E0。
std::uintptr_t __cdecl OffsetMapEffectForPlayerDirection_46E3E0(std::int32_t* x, std::int32_t* y); // 原 0x46E3E0；混合 EAX 位型。
// 原函数：0x0046E4E0。
void UpdateMapStreakParticles_46E4E0();                          // 原 0x46E4E0；EAX 不具统一返回合同。
// 原函数：0x0046E6C0。
void UpdateMapClusterParticles_46E6C0();                          // 原 0x46E6C0；活动链 kind 2，EAX 无统一合同。
// 原函数：0x0046E940。
std::int32_t __cdecl AdvanceMapClusterParticlesCycles_46E940(std::int32_t cycles); // 原 0x46E940；多轮 kind 2 更新。
// 原函数：0x0046EBB0。
std::int32_t __cdecl AdvanceMapStreakParticlesCycles_46EBB0(std::int32_t cycles); // 原 0x46EBB0；多轮 kind 1 更新。
// 原函数：0x0046E4C0。
std::int32_t __cdecl AdvanceMapParticleCycles_46E4C0(std::int32_t cycles); // 原 0x46E4C0；双效果更新，透传后者返回。
// 原函数：0x0046ED50。
void UpdateMapFlutterParticles_46ED50();                          // 原 0x46ED50；kind 4 创建与活动链更新，返回无统一合同。
// 原函数：0x0046E4A0。
void UpdateMapEffects_46E4A0();                          // 原 0x46E4A0；三效果生产后尾接 46F1D0。
// 原函数：0x0046F1D0。
void UpdateDirectionalMapEffects_46F1D0();                          // 原 0x46F1D0；kind 6/7/8 生成、推进、入队与回收。
// 原函数：0x00440FF0。
std::int32_t DrawObservationTravelBackground_440FF0();                  // 原 0x440FF0；观察效果更新后铺排 100 个背景项。
// 原函数：0x0046E0D0。
void __cdecl ConfigureDirectionalMapEffects_46E0D0(std::int32_t mask, std::int32_t value); // 原 0x46E0D0；场景效果配置。
// 原函数：0x0046F160。
void ClearDirectionalMapEffects_46F160();                          // 原 0x46F160；清活动链的 kind 6/7/8。
// 原函数：0x0046F090。
MapEffectRecord* CreateDirectionalMapEffect_46F090(
    std::int16_t kind, std::int32_t horizontalSign,
    std::int32_t verticalSign, std::int32_t phase,
    std::int32_t speed, std::int32_t graphicIndex); // 原 0x46F090；效果记录生产。
// 原函数：0x0046F000。
std::int32_t __cdecl ApplyMapCameraDeltaToEffectOffset_46F000(std::int32_t* x, std::int32_t* y); // 原 0x46F000；投影偏移增量。
// 原函数：0x00469C90。
std::uint8_t UpdateWorldMapInputAndMovement_469C90(); // 完整移动输入、目标交互与普通/跟随分流。
// 原函数：0x0046F680。
std::uint8_t UpdateMapMouseTargetAndButtons_46F680(); // 完整鼠标目标格投影/绘制与四种事件生产。
// 原函数：0x0046A990。
void RebuildPartyFollowOrder_46A990(); // 从同一宠物五槽生成9FB938顺序表，gate为0不清表。
// 原函数：0x0046A5A0。
void AppendFirstFollowerPathPoint_46A5A0(std::int32_t x, std::int32_t y); // 给顺序表第2项调用441F60。
// 原函数：0x0046A940。
void AdvancePlayerLeaderFollowers_46A940(); // 从顺序表第2项起调用442730，首个负S16终止。
// 原函数：0x00469FC0。
std::uint16_t UpdatePlayerPathMovement_469FC0(); // 普通玩家完整移动调度，承接原AX返回。
// 原函数：0x0046A410。
void SetPlayerMovementTarget_46A410(
    std::int32_t x, std::int32_t y, std::int32_t speed); // 玩家快照/共享移动分量初始化。
// 原函数：0x0046A700。
void AdvancePlayerWorldMovement_46A700(); // 原完整玩家移动推进、到达计数及MIS动作请求。

} // namespace detail

} // namespace cg::world
