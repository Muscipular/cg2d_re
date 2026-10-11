#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::resource::animation::detail {
struct MainSlot;
struct ActionEntry;
struct CoordinateEntry;
struct CacheActionEntry;
} // namespace cg::resource::animation::detail

namespace cg::resource::animation {

// 原 gAnimate 记录为 0x24 字节；三个原 W32 指针在 x64 承载为原生指针。
struct AnimateRecord {
    std::uint32_t animeNo;                   // 原 +00，查询与排序键。
    std::uint16_t count;                     // 原 +04，action 数量。
    std::uint16_t coordinateCount;           // 原 +06。
    std::int16_t fileIndex;                  // 原 +08。
    std::uint16_t value0A;                   // 原 +0A，意义未证。
    std::uint32_t offset;                    // 原 +0C。
    detail::MainSlot* listNode;              // 原 +10，指向主索引链的实际槽。
    std::uint32_t coordinateOffset;          // 原 +14。
    detail::ActionEntry* actionList;         // 原 +18，45B760生产的动作子表。
    detail::CoordinateEntry* coordinates;   // 原 +1C，45C150生产的8B坐标表。
    std::uint16_t flag;                      // 原 +20。
    std::array<std::uint8_t, 2> value22;    // 原 +22/+23，意义未证。
};

// 原 9A4A5C 独立的 0x24 字节缓存记录；+18/+1C 是可释放引用。
struct AnimeCacheRecord {
    std::array<std::uint32_t, 6> values;  // 原 +00..+17，字段暂不拆分。
    detail::CacheActionEntry* actionList; // 原 +18，仅已证24B前缀/可释放帧引用。
    void* buffer;                         // 原 +1C，可释放缓冲。
    std::array<std::uint8_t, 4> tail;     // 原 +20..+23，意义未证。
};

static_assert(std::is_standard_layout_v<AnimateRecord>);
static_assert(std::is_trivially_copyable_v<AnimateRecord>);
static_assert(sizeof(AnimateRecord) == 0x38);
static_assert(offsetof(AnimateRecord, animeNo) == 0);
static_assert(offsetof(AnimateRecord, listNode) == 0x10);
static_assert(offsetof(AnimateRecord, coordinateOffset) == 0x18);
static_assert(offsetof(AnimateRecord, actionList) == 0x20);
static_assert(offsetof(AnimateRecord, coordinates) == 0x28);
static_assert(offsetof(AnimateRecord, flag) == 0x30);
static_assert(std::is_standard_layout_v<AnimeCacheRecord>);
static_assert(std::is_trivially_copyable_v<AnimeCacheRecord>);
static_assert(sizeof(AnimeCacheRecord) == 0x30);
static_assert(offsetof(AnimeCacheRecord, actionList) == 0x18);
static_assert(offsetof(AnimeCacheRecord, buffer) == 0x20);

// 原 9A4A58/9A4A60/9A4A4C；GraphicAlloc 分配主表，AnimeInit 发布元素数。
// 原getIndex仍将元素数作为包含上界；不因此扩分配容量或补一过尾零记录。
extern AnimateRecord* gAnimateRecords9A4A58;
extern AnimeCacheRecord* gAnimateCache9A4A5C;
extern std::int32_t gAnimateCount9A4A60;
extern std::int32_t gMaxAnimateCount9A4A4C;

// 原CRT 553060及完整复制叶：9A4A50单次快照写入独立9A4A48，返回同一W32。
// 原函数：0x0045B370。
std::uint32_t Sub_45B370();
// 原函数：0x0045B380。
std::uint32_t Sub_45B380();

// 原 45B420：qsort 的无符号首 W32 升序比较。
std::int32_t __cdecl Compare_45B420(const void* left, const void* right);

// 原 45B390/45B400：包含上界，跨度不足 10 时顺扫。
std::int32_t __cdecl BinarySearch_45B390(
    std::uint32_t animeNo, std::int32_t min, std::int32_t max);
// 原函数：0x0045B400。
std::int32_t __cdecl GetIndex_45B400(std::uint32_t animeNo);

// 原 [45BB20,45BEE1)：九槽 Info 去重、数据文件发布与主表排序；0 成功，2/3/5 失败。
// 参数顺序为数据路径、Info 路径；保留必需槽 0/8 的不同清理路径。
std::int32_t AnimeInit_45BB20(
    char* const* dataPaths, char* const* infoPaths);

// 原 [45C040,45C146)：清九文件槽，加载首个路径并向大小写相同的后续槽共享文件/回写路径。
void CoordLoadInit_45C040(char* const* dataPaths, char* const* infoPaths);

// 原 [45B760,45BB1F)：复用或领取主索引槽，读 action/帧，再加载坐标并发布计数。
// index 须对应已初始化主表；不增加索引/文件判空，失败按原路径保留或清理已发布前缀。
std::int32_t AnimeLoadAction_45B760(std::int32_t index);

// 原退出链：先关九个动画文件并清主表活动槽，再按标志处理文件及 P4 活动槽。
// 原函数：0x0045BEF0。
std::int32_t AnimeUnload_45BEF0(); // ACL01：九FILE关闭清槽后256主槽清理，真实末返回。
// 原函数：0x0045BF20。
std::int32_t Sub_45BF20(); // ACL01：九flag关闭后256P4槽清理，已批FILE引用清空保。

} // namespace cg::resource::animation
