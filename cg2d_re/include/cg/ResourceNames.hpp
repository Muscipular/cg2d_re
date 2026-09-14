#pragma once

namespace cg {

// 来源：cg2d_re/doc/cg2d_resource_sources.md 第3、4节。
// 以下枚举仅为复刻实现的存储区下标，不代表原业务枚举。
enum ResourceNameTable {
    GraphicNames = 0,        // VA 0x556F84，Graphic 数据名。
    GraphicInfoNames = 1,    // VA 0x557404，GraphicInfo 名。
    AnimeNames = 2,          // VA 0x557884，Anime 数据名。
    AnimeInfoNames = 3,      // VA 0x557D04，AnimeInfo 名。
    CoordinateNames = 4,     // VA 0x558184，Coordinate 数据名。
    CoordinateInfoNames = 5, // VA 0x558604，Coordinate Info 名。
    ResourceNameTableCount = 6
};

// 仅复刻原 VA [0x556F84, 0x558A84) 的连续字节布局，不宣称原源码存在同名全局。
// 必须可写：ParseCmdOpt 的 sprintf 与坐标别名复制会原地改写名称。
extern char gResourceNames[ResourceNameTableCount][9][128];

static_assert(sizeof(gResourceNames) == 0x1B00, "Resource name storage size mismatch");
static_assert(sizeof(gResourceNames[0]) == 0x480, "Resource name table stride mismatch");
static_assert(sizeof(gResourceNames[0][0]) == 0x80, "Resource name slot stride mismatch");

} // namespace cg
