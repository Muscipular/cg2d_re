#pragma once
#include <string>

namespace cg::resource {

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

// 原名称表的历史接口声明；当前加载使用ResourcePaths的配置路径。
extern std::wstring gResourceNames[ResourceNameTableCount][9]; // 原全局：0x00556F84。


} // namespace cg::resource
