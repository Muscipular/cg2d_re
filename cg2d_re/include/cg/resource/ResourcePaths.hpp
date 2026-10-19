#pragma once
#include <string>

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
    std::wstring binPath; // 原成员偏移：+0x7C。
};
static_assert(sizeof(CursorRecord) == 12);
static_assert(offsetof(CursorPathStorage, lastCursorResourceId) == 0x78);

extern CursorPathStorage gCursorPathStorage; // 原全局：0x0055CE88。

// 原0x55CF04路径槽；引用同一拥有者的宽文本成员。
extern std::wstring& gBinPath;

// BLM01：配置组数与所有文件／基址槽统一；磁盘字段不随容量扩宽。
inline constexpr std::size_t BinLoaderMaxGroups = 32;
inline constexpr std::size_t AutoMapColorCapacity = 800000;
extern std::size_t gBinLoaderGroupCount;
extern std::int32_t gBinLoaderGraphicCapacity;
extern std::int32_t gBinLoaderAnimeCapacity;

// 字符串由ResourcePaths拥有，配置解析完成后绑定，加载期间不再移动字符串。
extern wchar_t* gGraphicPaths[BinLoaderMaxGroups]; // 原全局：0x0062D108。
extern wchar_t* gGraphicInfoPaths[BinLoaderMaxGroups]; // 原全局：0x0062DA74。
extern wchar_t* gAnimePaths[BinLoaderMaxGroups]; // 原全局：0x0062BEE4。
extern wchar_t* gAnimeInfoPaths[BinLoaderMaxGroups]; // 原全局：0x0062D150。
extern wchar_t* gCoordinatePaths[BinLoaderMaxGroups]; // 原全局：0x0062B5C0。
extern wchar_t* gCoordinateInfoPaths[BinLoaderMaxGroups]; // 原全局：0x0062D12C。

// 原 0x456050；BLM01改读工作目录cgmsv.yml，成功0，配置失败提示后返回-1。
int BuildResourcePaths();

// 原 sub_45A130@[0x45A130,0x45A184)；BLM01直接重建，旧缓存不能验证配置组覆盖。
std::int32_t LoadAutoMapColor_45A130(wchar_t* const* graphicInfoPaths);

// 原oldMapRes@927F48；BLM01扩展，newMapRes高下标仍借同表的mapNo-200000位置。
extern std::uint8_t gAutoMapColors927F48[AutoMapColorCapacity]; // 原全局：0x00927F48。

} // namespace cg::resource
