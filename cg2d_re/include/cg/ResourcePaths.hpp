#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::resource {

// 原 0x55CE88 的 12 字节记录；WinMain 仅读取 resourceId 的低 16 位。
struct CursorRecord {
    std::uint32_t resourceId; // 原成员偏移：+0x00。
    std::uint32_t value04; // 原成员偏移：+0x04。
    std::uint32_t value08; // 原成员偏移：+0x08。
};

// 原 [0x55CE88,0x55CF84)：第十一条光标记录的后八字节与 binPath 前八字节重叠。
struct CursorPathStorage {
    CursorRecord cursor[10]; // 原成员偏移：+0x00。
    std::uint32_t lastCursorResourceId; // 原成员偏移：+0x78。
    char binPath[128]; // 原成员偏移：+0x7C。
};
static_assert(sizeof(CursorRecord) == 12);
static_assert(offsetof(CursorPathStorage, lastCursorResourceId) == 0x78);
static_assert(offsetof(CursorPathStorage, binPath) == 0x7C);
static_assert(sizeof(CursorPathStorage) == 0xFC);

extern CursorPathStorage gCursorPathStorage; // 原全局：0x0055CE88。

// 原 0x55CF04 的 128 字节可写字符槽；数组引用保留与上述连续存储的别名。
extern char (&gBinPath)[128];

// 原 sub_456050 建立的六组九槽路径；各字符槽仍为 256 字节。
extern char gGraphicPathStorage[9][256]; // 原全局：0x0062DA98。
extern char gGraphicInfoPathStorage[9][256]; // 原全局：0x0062D174。
extern char gAnimePathStorage[9][256]; // 原全局：0x0062B5E4。
extern char gAnimeInfoPathStorage[9][256]; // 原全局：0x0062C808。
extern char gCoordinatePathStorage[9][256]; // 原全局：0x0062E398。
extern char gCoordinateInfoPathStorage[9][256]; // 原全局：0x0062BF08。

// 对应原路径指针表；仅指针宽度适配为 x64，不扩大字符槽。
extern char* gGraphicPaths[9]; // 原全局：0x0062D108。
extern char* gGraphicInfoPaths[9]; // 原全局：0x0062DA74。
extern char* gAnimePaths[9]; // 原全局：0x0062BEE4。
extern char* gAnimeInfoPaths[9]; // 原全局：0x0062D150。
extern char* gCoordinatePaths[9]; // 原全局：0x0062B5C0。
extern char* gCoordinateInfoPaths[9]; // 原全局：0x0062D12C。

// 原 0x456050：先绑定所有指针，再逐槽拼接六类路径；返回最后一次 sprintf 结果。
// 调用前须已完成原参数改名及星号别名展开，且拼接结果含 NUL 不超过 256 字节。
int BuildResourcePaths();

// 原 sub_45A130@[0x45A130,0x45A184)：尝试读取自动地图颜色缓存，失败时重建并写回。
std::int32_t LoadAutoMapColor_45A130(char* const* graphicInfoPaths);

// 原 oldMapRes@927F48；资源加载与地图窗共用唯一500000B颜色表。
extern std::uint8_t gAutoMapColors927F48[500000]; // 原全局：0x00927F48。

} // namespace cg::resource
