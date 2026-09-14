#pragma once

namespace cg::resource {

// 原 0x55CF04：资源根目录，保留 128 字节可写字符槽。
extern char gBinPath[128];

// 原 sub_456050 建立的六组九槽路径；各字符槽仍为 256 字节。
extern char gGraphicPathStorage[9][256];
extern char gGraphicInfoPathStorage[9][256];
extern char gAnimePathStorage[9][256];
extern char gAnimeInfoPathStorage[9][256];
extern char gCoordinatePathStorage[9][256];
extern char gCoordinateInfoPathStorage[9][256];

// 对应原路径指针表；仅指针宽度适配为 x64，不扩大字符槽。
extern char* gGraphicPaths[9];
extern char* gGraphicInfoPaths[9];
extern char* gAnimePaths[9];
extern char* gAnimeInfoPaths[9];
extern char* gCoordinatePaths[9];
extern char* gCoordinateInfoPaths[9];

// 原 0x456050：先绑定所有指针，再逐槽拼接六类路径；返回最后一次 sprintf 结果。
// 调用前须已完成原参数改名及星号别名展开，且拼接结果含 NUL 不超过 256 字节。
int BuildResourcePaths();

} // namespace cg::resource
