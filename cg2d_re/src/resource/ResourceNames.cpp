#include "cg/resource/ResourceNames.hpp"

namespace cg {

// 各槽按原始 ASCII 字符串初始化，终止 NUL 后的剩余字节全部补零。
char gResourceNames[ResourceNameTableCount][9][128] = { // 原全局：0x00556F84。
    {
        "Graphic.bin",
        "GraphicEx.bin",
        "GraphicV2.bin",
        "GraphicV3.bin",
        "PUK2\\Graphic_PUK2.bin",
        "PUK3\\Graphic_PUK3.bin",
        "Graphic_Joy.bin",
        "Graphic_Joy_Ex.bin",
        "Graphic_Joy_CH1.bin"
    },
    {
        "GraphicInfo.bin",
        "GraphicInfoEx.bin",
        "GraphicInfoV2.bin",
        "GraphicInfoV3.bin",
        "PUK2\\GraphicInfo_PUK2.bin",
        "PUK3\\GraphicInfo_PUK3.bin",
        "GraphicInfo_Joy.bin",
        "GraphicInfo_Joy_Ex.bin",
        "GraphicInfo_Joy_CH1.bin"
    },
    {
        "Anime.bin",
        "AnimeEx.bin",
        "AnimeV2.bin",
        "AnimeV3.bin",
        "PUK2\\Anime_PUK2.bin",
        "PUK3\\Anime_PUK3.bin",
        "Anime_Joy.bin",
        "Anime_Joy_Ex.bin",
        "Anime_Joy_CH1.bin"
    },
    {
        "AnimeInfo.bin",
        "AnimeInfoEx.bin",
        "AnimeInfoV2.bin",
        "AnimeInfoV3.bin",
        "PUK2\\AnimeInfo_PUK2.bin",
        "PUK3\\AnimeInfo_PUK3.bin",
        "AnimeInfo_Joy.bin",
        "AnimeInfo_Joy_Ex.bin",
        "AnimeInfo_Joy_CH1.bin"
    },
    {
        "*3",
        "*3",
        "*3",
        "Crd.bin",
        "*3",
        "*3",
        "*3",
        "*3",
        "*3"
    },
    {
        "*3",
        "*3",
        "*3",
        "CrdInfo.bin",
        "*3",
        "*3",
        "*3",
        "*3",
        "*3"
    }
};

} // namespace cg
