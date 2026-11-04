#pragma once

#include "cg/world/MapEffectRuntimeState.hpp"
#include "cg/world/MapDataRuntimeState.hpp"

#include <cstdint>

namespace cg::network {
class CgConnection;
enum class SendResult : std::int32_t;
}
namespace cg::session { class ClientSession; }

#include <array>
#include <cstddef>
#include <type_traits>
#include <string>
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
// AT01：before=2将称号插到前景链头，队列逆序绘制时位于该人物挂件最上方。
inline constexpr std::int32_t kMapAttachmentFrontmost = 2;
// 原 46C060：挂接主项的前绘/后绘链；batch 保留低 W8，不布尔化。
void DrawGearNormal_46C060(
    std::int32_t primaryIndex, std::int32_t before, std::int32_t imageNo,
    std::int16_t x, std::int16_t y,
    const render::GraphicDescriptor16* descriptor, std::uint8_t batchType,
    std::uint8_t layer = 10); // AF406：FIELD/倒地WING单独层，普通挂件仍走人物前后链。

// 借用SceneState唯一方向表，分别读取原55D9DC/55D9DE的S16分量。
std::int16_t ReadDirectionDeltaX(std::int16_t direction) noexcept;
std::int16_t ReadDirectionDeltaY(std::int16_t direction) noexcept;
// 原468520；调用方传入9E0434当前值，内部路径改为宽字符串。
std::wstring FormatMapDataPath_468520( std::int32_t mapId, std::int32_t value9E0434, std::int32_t floor);

// 原函数：0x004681B0。
std::int32_t __cdecl RestorePlayerMovementSnapshot_4681B0(session::ClientSession& session);         // 原 [0x4681B0,0x4681F6)，快照恢复后清六槽。
// 原函数：0x00469810。
std::int32_t __cdecl IsMapCellBlocked_469810(session::ClientSession& session, std::int32_t x, std::int32_t y); // 原 [0x469810,0x469864)，边界外或 walk WORD==1。
// 原函数：0x0050CF90。
void __cdecl HandleMapTransitionReply_50CF90(cg::network::CgConnection& networkConnection, std::int32_t sequence,
    std::int32_t status); // 原 [0x50CF90,0x50CFF7)，地图回复序号与状态清理。
// 原函数：0x00509090。
void __cdecl HandleMapRectangleCrcReply_509090(cg::network::CgConnection& networkConnection, std::int32_t first,
    std::int32_t floor, std::int32_t xStart,
    std::int32_t yStart, std::int32_t xEnd,
    std::int32_t yEnd, std::int32_t expectedTile,
    std::int32_t expectedObj, std::int32_t expectedHit); // 原 [0x509090,0x5090FC)，地图校验回复回调。
// 原函数：0x00509100。
std::int32_t __cdecl HandleMapRectangleDataReply_509100(cg::network::CgConnection& networkConnection, std::int32_t mapId,
    std::int32_t floor, std::int32_t xStart,
    std::int32_t yStart, std::int32_t xEnd,
    std::int32_t yEnd, const char* payload); // 原 [0x509100,0x5093B6)，连续局部帧地图增量回复。
// 原函数：0x005093F0。
std::int32_t __cdecl HandleMapChangeReply_5093F0(cg::network::CgConnection& networkConnection, std::int32_t a2, std::int32_t a3,
    std::int32_t a4, std::int32_t a5, std::int32_t a6,
    std::int32_t a7, std::int32_t a8, std::int32_t a9,
    std::int32_t a10, std::int32_t a11, std::int32_t a12,
    std::int32_t a13); // 原 [0x5093F0,0x509548)，地图保存及场景状态发布。
// 原函数：0x00509550。
std::int32_t __cdecl HandleMapNameReply_509550(cg::network::CgConnection& networkConnection, char* payload); // PTR01：原完整回复，地图名整词/状态/尾字节及解析持值顺序保持。
// 原 46C760：按地图模式和重建标志提交同一窗口，返回完整绘制结果。
std::int32_t DrawCurrentAutoMap_46C760(session::ClientSession& session, std::int32_t x, std::int32_t y);
// 原 [46CF60,46D9B9)：完整两路地图绘制；参数只控制普通地图标记，重建场景10实传0。
void __cdecl DrawMapScene_46CF60(session::ClientSession& session, std::int32_t value);
// 原函数：0x0046CF00。
std::int32_t __cdecl ResolveMapGraphicNumber_46CF00(session::ClientSession& session, std::int32_t value); // 原 [0x46CF00,0x46CF1A)，读已初始化的共享索引表。
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
void HandlePartyMemberReply_50A3E0(cg::network::CgConnection& networkConnection, std::int32_t index, const char* payload);
// 原函数：0x0050D000。
void HandlePartyMembershipReply_50D000(cg::network::CgConnection& networkConnection, std::int32_t first, std::int32_t second); // 跟随状态回复。
// 原函数：0x005093C0。
void HandlePlayerPositionReply_5093C0(cg::network::CgConnection& networkConnection, std::int32_t x, std::int32_t y,
    std::int32_t direction); // 原坐标回复，无门禁，方向最后发布。
// 原函数：0x0050D080。
void Sub_50D080(cg::network::CgConnection& networkConnection, std::int32_t first, std::int32_t second); // 两次CryptInt赋值后复制首值。
// 原函数：0x0046E480。
void DrawPendingMapEffects_46E480(session::ClientSession& session);                          // 原 0x46E480；非零提交标志才绘制，调用后无条件清 W16。
// 原函数：0x0046BD40。
std::int32_t ResetMapDrawCacheCounts_46BD40();                 // 原 [0x46BD40,0x46BD57)，依次清四个独立 W32 槽并返回 0。
// 原 46FB80：先X差再Y差，返回Y平方+X平方的F64值。
double GetSquaredWorldDistance_46FB80(float x, float y, float targetX, float targetY);

namespace detail {

// 原四个地图绘制源计数，仍由 SceneState 持有并由 46BD40 逐项清零。
extern std::uint32_t gPrimaryMapGraphicCount_9FB934; // 原全局：0x009FB934。
extern std::uint32_t gMapObjectGraphicCount_9FB928; // 原全局：0x009FB928。
extern std::uint32_t gSpecialMapGraphicCount_9A86C0; // 原全局：0x009A86C0。
extern std::uint32_t gMapAttachmentDrawCount_9A7F78; // 原全局：0x009A7F78。
// 原函数：0x0046BE50。
void CacheMapGraphicByResourceType_46BE50(
    std::int32_t imageNo, std::uint32_t worldXBits, std::uint32_t worldYBits,
    std::int16_t x, std::int16_t y, const render::GraphicDescriptor16* descriptor);
// 原 46C2F0：排序→合并，caller 不消费混合 EAX；46C1C0 返回末次合并计数。
void SortAndMergeMapDrawCache_46C2F0();
// 原函数：0x0046C1C0。
std::int32_t QueueMergedMapDrawCache_46C1C0();

// 原函数：0x00468570。
std::int32_t __cdecl EnsureMapCacheFile_468570(
    std::int32_t a1, std::int32_t a2, std::int32_t a3,
    std::int32_t a4, std::int32_t a5, std::int32_t a6,
    std::int32_t a7); // 原 [0x468570,0x4687A3)，旧文件比较或新建。
// 原函数：0x004687B0。
std::int32_t __cdecl CreateEmptyMapCacheFile_4687B0(
    const wchar_t* path, std::int32_t first, std::int32_t second,
    std::int32_t height, std::int32_t width); // 原 [0x4687B0,0x4688A0)，三平面零值文件创建叶。
// 原函数：0x004688A0。
std::int32_t __cdecl WriteMapCacheRectangle_4688A0(session::ClientSession& session,
    std::int32_t mapId, std::int32_t floor,
    std::int32_t xStart, std::int32_t yStart,
    std::int32_t xEnd, std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    std::uint16_t* hit); // 原 [0x4688A0,0x468BF5)，增量写三平面文件及共享 hit。
// 原函数：0x00469870。
std::int32_t FindMissingMapRectanglesAhead_469870(session::ClientSession& session, std::int32_t direction); // 原 [0x469870,0x469C87)，四向探测依赖。
// 原函数：0x0046AF30。
std::uint32_t RequestMissingMapRectanglesAhead_46AF30(session::ClientSession& session); // 原 [0x46AF30,0x46AFDE)，探测后发送 UUN 和发布状态。
// 原函数：0x00468C40。
std::int32_t __cdecl ReadMapCacheRectangle_468C40(session::ClientSession& session,
    std::int32_t mapId, std::int32_t floor,
    std::int32_t xStart, std::int32_t yStart,
    std::int32_t xEnd, std::int32_t yEnd,
    std::uint16_t* tile, std::uint16_t* obj,
    std::uint16_t* hit); // 原 [0x468C40,0x468F25)，真实三平面文件装载依赖。
// 原函数：0x004682E0。
void __cdecl SetWorldMapMetadata_4682E0(
    session::ClientSession& session,
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
    session::ClientSession& session,
    std::uint32_t a1,
    std::int32_t a2,
    std::int32_t a3,
    std::uint32_t a4); // 原 [0x46DA80,0x46DAF5)，音频重置与模式分支。
// 原函数：0x0046DB00。
std::int32_t __cdecl LoadCutMapData_46DB00(session::ClientSession& session, std::int32_t selected); // 原 [0x46DB00,0x46DC55)，.cut 加载器。
// 原函数：0x0046DC60。
void __cdecl ResetCutMapData_46DC60(session::ClientSession& session); // 原 [0x46DC60,0x46DC85)，先整块清零再逐项写哨兵。
// 原函数：0x0046DC90。
std::int32_t __cdecl LoadAutoMapColorData_46DC90(session::ClientSession& session, std::int32_t selected); // 原 [0x46DC90,0x46DD35)，.aut 加载器。
// 原函数：0x0046A9E0。
std::int32_t __cdecl ResetPendingPathEvent_46A9E0(session::ClientSession& session);         // 原 [0x46A9E0,0x46A9F4)，清三槽、返回 0。
// 原函数：0x0046AA00。
std::int32_t __cdecl DetectPendingPathEvents_46AA00(session::ClientSession& session);         // PDH01：路径方向/两整数差与 F32 桥，保 S32 早退 Y 和 pending 发布。
// 原函数：0x0046BC90。
void __cdecl PopPlayerMovementPath_46BC90(session::ClientSession& session);                // 原 [0x46BC90,0x46BCE4)，三组路径 W16 同步左移。
// 原函数：0x0046ABD0。
void __cdecl QueueNextMovementDirections_46ABD0(session::ClientSession& session);                // 原 [0x46ABD0,0x46AC0A)，最多搬运两项方向。
// 原函数：0x0046AC10。
std::uint16_t SendPendingMovementDirections_46AC10(session::ClientSession& session); // 方向文本生成/发送，仅承接原AX返回位型。
// 原函数：0x0046B370。
void TurnPlayerTowardCursorTarget_46B370(session::ClientSession& session); // 目标格转向，保留重复CryptInt读取与独立500ms节流。
// 原函数：0x0046BCF0。
void __cdecl PopPendingMovementDirection_46BCF0(session::ClientSession& session);                // 原 [0x46BCF0,0x46BD1D)，方向计数递减后左移。
// 原函数：0x00467F30。
std::int32_t* InitializeWorldMapState_467F30(session::ClientSession& session); // 原 [0x467F30,0x4681AB)，透传 46CF20 一过尾指针。
// 原函数：0x00468200。
std::uint16_t __cdecl UpdatePlayerMapWindow_468200(session::ClientSession& session);        // 原 [0x468200,0x46828D)，仅返回 AX 位模式。
// 原函数：0x0046CEC0。
std::int32_t __cdecl GetPlayerMapCoordinateSum_46CEC0(session::ClientSession& session);         // 原 [0x46CEC0,0x46CED6)，先读 Y 再对 X 加。
// 原函数：0x0046CEE0。
std::int32_t __cdecl StorePlayerMapCoordinateSum_46CEE0(session::ClientSession& session);         // 原 [0x46CEE0,0x46CEFB)，并写 AF10C0。
// 原函数：0x00468330。
std::uint32_t __cdecl SetPlayerMapPosition_468330(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y);        // 原 [0x468330,0x468511)，返回 Y 浮点位型。
// 原函数：0x00468290。
std::int16_t* __cdecl ClipMapRectangleToBounds_468290(session::ClientSession& session,
    std::int16_t* xStart, std::int16_t* yStart,
    std::int16_t* xEnd, std::int16_t* yEnd); // 原 [0x468290,0x4682D7)，原地裁剪并返回末指针。
// 原函数：0x00468C00。
std::int32_t __cdecl SetMapCellHitValue_468C00(session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::uint16_t value); // 原 [0x468C00,0x468C32)，写 hit WORD。
// 原函数：0x0046AE60。
std::int32_t __cdecl IsDirectionalMapTransitionCell_46AE60(session::ClientSession& session, std::int32_t x, std::int32_t y); // MHT01：原边界/索引序与命中低 BYTE，返 S32 0/1。
// 原函数：0x00469240。
std::int32_t __cdecl CalculateMapDataCrc_469240(
    const std::uint8_t* bytes, std::int32_t length); // 原 [0x469240,0x469284)，三平面校验的 W16 查表叶。
// 原函数：0x004691D0。
void __cdecl InvalidateMapDrawPosition_4691D0(session::ClientSession& session); // 原 [0x4691D0,0x469239)，两次 -1 临时赋值给地图状态。
// 原函数：0x00469290。
std::int32_t __cdecl CheckMapRectangleCrcAndRequest_469290(
    network::CgConnection& connection, std::int32_t first, std::int32_t floor,
    std::int32_t xStart, std::int32_t yStart,
    std::int32_t xEnd, std::int32_t yEnd,
    std::int32_t expectedTile, std::int32_t expectedObj,
    std::int32_t expectedHit); // 原 [0x469290,0x4693E9)，真实局部三平面校验依赖。
// 原函数：0x0051D6F0。
void __cdecl ApplyPendingMapPaletteReply_51D6F0(session::ClientSession& session); // PTR01：原完整消费，原-1/非负分支、调色板副作用后发布保持。
// 原函数：0x00469030。
std::int32_t __cdecl LoadAndDrawMapTiles_469030(session::ClientSession& session); // 原 [0x469030,0x4691C9)，地图装载、第四平面与斜向绘制。
// 原 46C840：三平面窗口读取及两类地图颜色生产；不改变玩家坐标。
std::int32_t LoadAutoMapWindow_46C840(session::ClientSession& session, std::int32_t floor, std::int32_t playerX, std::int32_t playerY);
// 原469020仅传标记参数1；43CB10完成战斗背景快照并更新5ABCD8。
void DrawMapSceneWithMarkers_469020(session::ClientSession& session);
// 原 [0x4693F0,0x4697C9)：先扫 tile，再扫 obj/图形尺寸，输出第四地图平面。
std::int32_t __cdecl BuildGraphicMapWalkability_4693F0(session::ClientSession& session,
    std::int32_t xStart, std::int32_t yStart, std::int32_t xEnd, std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk);
// 原 [0x46D9C0,0x46DA72)：模式 1 读取 hit 高字节 bit0，其他模式转 4693F0。
std::int32_t __cdecl BuildMapWalkability_46D9C0(session::ClientSession& session,
    std::int32_t xStart, std::int32_t yStart, std::int32_t xEnd, std::int32_t yEnd,
    const std::uint16_t* tile, const std::uint16_t* obj,
    const std::uint16_t* hit, std::uint16_t* walk);
// 原函数：0x0046AD00。
std::int32_t __cdecl IsMapTransitionCellActive_46AD00(session::ClientSession& session, std::int32_t x, std::int32_t y); // MHT01：time 后跨度/低 BYTE，保 W16 分类发布和原短路序。
// 原函数：0x00468F30。
std::int32_t __cdecl ResetMapProjectionState_468F30(session::ClientSession& session);         // 原 [0x468F30,0x468F6F)，按序清十二槽、返回 0。
// 原函数：0x00468F70。
std::int64_t __cdecl InitializeMapProjectionOrigin_468F70(session::ClientSession& session);         // 原 [0x468F70,0x468FE4)，F32 投影、向零截断后写六槽。
// 原函数：0x0046CF20。
std::int32_t* __cdecl InitializeMapGraphicNumberTable_46CF20(session::ClientSession& session);        // 原 [0x46CF20,0x46CF57)，填表并返回一过尾指针。
// 原函数：0x0046B580。
std::int64_t __cdecl GetMapDirectionBetweenPoints_46B580(
    std::int32_t first, std::int32_t second,
    std::int32_t third, std::int32_t fourth); // PDH01：两整数差先取齐，Y/X 各落 F32，原方向 S64 透传。
// 原函数：0x0046B5C0。
std::int32_t __cdecl MergePathTailIntoDiagonalStep_46B5C0(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y,
    std::int32_t firstX, std::int32_t firstY,
    std::int32_t secondX, std::int32_t secondY); // 原 [0x46B5C0,0x46B6F0)，探测两邻格并覆写路径末项。
// 原函数：0x0046B6F0。
std::int32_t BuildPlayerMovementPath_46B6F0(session::ClientSession& session); // 完整候选/绕行/轴补走路径生成，复用三组W16路径。
// 原函数：0x0046BD20。
std::uint32_t __cdecl EncodeMovementDirectionByte_46BD20(
    std::int32_t value, std::int32_t mode); // 原 [0x46BD20,0x46BD32)，仅改 AL，保留 mode 高 24 位。
// 原函数：0x0050C8D0。
network::SendResult __cdecl SendCurrentPlayerMovementPath_50C8D0(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y,
    const char* text); // 原 [0x50C8D0,0x50C8F5)，透传 zA 请求返回。
void ResetMapFollowersFromPath(session::ClientSession& session, cg::yobi::YobiAction* leader); // 444410仅扫描占用的地图槽1..4。
// 原函数：0x0046AFE0。
void PropagatePartyFollowerPath_46AFE0(
    session::ClientSession& session,
    std::int32_t index, std::int32_t x, std::int32_t y); // 移除后的五槽路径续接。
// 原函数：0x0046ADA0。
void* __cdecl RequestDetectedMapTransition_46ADA0(session::ClientSession& session); // 原 [0x46ADA0,0x46AE53)，仅透传 51D5B0 的返回位型。
// 原函数：0x0046AC90。
network::SendResult __cdecl RequestPlayerCoordinateSynchronization_46AC90(session::ClientSession& session); // 原 [0x46AC90,0x46AD00)，type 2 的真实后继。
// 原函数：0x0046AEA0。
std::int32_t __cdecl RequestDirectionalMapTransition_46AEA0(session::ClientSession& session); // 原 [0x46AEA0,0x46AF24)，type 3 的真实后继。
// 原函数：0x0046AB60。
std::int32_t __cdecl DispatchPendingPathEvent_46AB60(session::ClientSession& session); // 原 [0x46AB60,0x46ABCD)，pending 倒计时与分发。
// render 只借用所属会话的活动链及已证数值字段，不截断原生next。
// 原函数：0x0046E2A0。
MapEffectRecord* InitializeMapEffectPool_46E2A0(session::ClientSession& session);             // 原 0x46E2A0；返回末项非拥有指针。
// 原函数：0x0046E340。
MapEffectRecord* AcquireMapEffectRecord_46E340(session::ClientSession& session);             // 原 0x46E340；空闲首项转为活动首项。
// 原函数：0x0046E390。
MapEffectRecord* ReleaseMapEffectRecord_46E390(session::ClientSession& session, MapEffectRecord* record); // 原 0x46E390；摘活动链后退回空闲头。
// 原函数：0x0046E3E0。
std::uintptr_t __cdecl OffsetMapEffectForPlayerDirection_46E3E0(session::ClientSession& session, std::int32_t* x, std::int32_t* y); // 原 0x46E3E0；混合 EAX 位型。
// 原函数：0x0046E4E0。
void UpdateMapStreakParticles_46E4E0(session::ClientSession& session); // 原 0x46E4E0；EAX 不具统一返回合同。
// 原函数：0x0046E6C0。
void UpdateMapClusterParticles_46E6C0(session::ClientSession& session); // 原 0x46E6C0；活动链 kind 2，EAX 无统一合同。
// 原函数：0x0046E940。
std::int32_t __cdecl AdvanceMapClusterParticlesCycles_46E940(session::ClientSession& session, std::int32_t cycles); // 原 0x46E940；多轮 kind 2 更新。
// 原函数：0x0046EBB0。
std::int32_t __cdecl AdvanceMapStreakParticlesCycles_46EBB0(session::ClientSession& session, std::int32_t cycles); // 原 0x46EBB0；多轮 kind 1 更新。
// 原函数：0x0046E4C0。
std::int32_t __cdecl AdvanceMapParticleCycles_46E4C0(session::ClientSession& session, std::int32_t cycles); // 原 0x46E4C0；双效果更新，透传后者返回。
// 原函数：0x0046ED50。
void UpdateMapFlutterParticles_46ED50(session::ClientSession& session); // 原 0x46ED50；kind 4 创建与活动链更新，返回无统一合同。
// 原函数：0x0046E4A0。
void UpdateMapEffects_46E4A0(session::ClientSession& session); // 原 0x46E4A0；三效果生产后尾接 46F1D0。
// 原函数：0x0046F1D0。
void UpdateDirectionalMapEffects_46F1D0(session::ClientSession& session); // 原 0x46F1D0；kind 6/7/8 生成、推进、入队与回收。
// 原函数：0x00440FF0。
std::int32_t DrawObservationTravelBackground_440FF0(session::ClientSession& session); // 原 0x440FF0；观察效果更新后铺排 100 个背景项。
// 原函数：0x0046E0D0。
void __cdecl ConfigureDirectionalMapEffects_46E0D0(session::ClientSession& session, std::int32_t mask, std::int32_t value); // 原 0x46E0D0；场景效果配置。
// 原函数：0x0046F160。
void ClearDirectionalMapEffects_46F160(session::ClientSession& session);                          // 原 0x46F160；清活动链的 kind 6/7/8。
// 原函数：0x0046F090。
MapEffectRecord* CreateDirectionalMapEffect_46F090(
    session::ClientSession& session,
    std::int16_t kind, std::int32_t horizontalSign,
    std::int32_t verticalSign, std::int32_t phase,
    std::int32_t speed, std::int32_t graphicIndex); // 原 0x46F090；效果记录生产。
// 原函数：0x0046F000。
std::int32_t __cdecl ApplyMapCameraDeltaToEffectOffset_46F000(session::ClientSession& session, std::int32_t* x, std::int32_t* y); // 原 0x46F000；投影偏移增量。
// 原函数：0x00469C90。
std::uint8_t UpdateWorldMapInputAndMovement_469C90(session::ClientSession& session); // 完整移动输入、目标交互与普通/跟随分流。
// 原函数：0x0046F680。
std::uint8_t UpdateMapMouseTargetAndButtons_46F680(session::ClientSession& session); // 完整鼠标目标格投影/绘制与四种事件生产。
// 原函数：0x0046A990。
void RebuildPartyFollowOrder_46A990(session::ClientSession& session); // 从同一宠物五槽生成9FB938顺序表，gate为0不清表。
// 原函数：0x0046A5A0。
void AppendFirstFollowerPathPoint_46A5A0(session::ClientSession& session, std::int32_t x, std::int32_t y); // 给顺序表第2项调用441F60。
// 原函数：0x0046A940。
void AdvancePlayerLeaderFollowers_46A940(session::ClientSession& session); // 从顺序表第2项起调用442730，首个负S16终止。
// 原函数：0x00469FC0。
std::uint16_t UpdatePlayerPathMovement_469FC0(session::ClientSession& session); // 普通玩家完整移动调度，承接原AX返回。
// 原函数：0x0046A410。
void SetPlayerMovementTarget_46A410(
    session::ClientSession& session,
    std::int32_t x, std::int32_t y, std::int32_t speed); // 玩家快照/共享移动分量初始化。
// 原函数：0x0046A700。
void AdvancePlayerWorldMovement_46A700(session::ClientSession& session); // 原完整玩家移动推进、到达计数及MIS动作请求。

} // namespace detail

} // namespace cg::world
