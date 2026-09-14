#include "cg/ResourcePaths.hpp"

#include "cg/ResourceNames.hpp"

#include <cstdio>

namespace cg::resource {

CursorPathStorage gCursorPathStorage {
    {
        {0x6B, 0x0003BE9B, 0xFFE2FFEC},
        {0x6C, 0x0003BE9C, 0x00000000},
        {0x6D, 0x0003C3A8, 0xFFEAFFE9},
        {0x75, 0x0003C3A1, 0xFFE7FFF0},
        {0x6F, 0x0003C3A2, 0xFFEAFFF7},
        {0x70, 0x0003C3A3, 0xFFF0FFF9},
        {0x71, 0x0003C3A4, 0xFFF6FFF7},
        {0x72, 0x0003C3A5, 0xFFF9FFF0},
        {0x73, 0x0003C3A6, 0xFFF6FFE9},
        {0x74, 0x0003C3A7, 0xFFF0FFE7},
    },
    0x6E,
    "Bin",
};
char (&gBinPath)[128] = gCursorPathStorage.binPath;

char gGraphicPathStorage[9][256] {};
char gGraphicInfoPathStorage[9][256] {};
char gAnimePathStorage[9][256] {};
char gAnimeInfoPathStorage[9][256] {};
char gCoordinatePathStorage[9][256] {};
char gCoordinateInfoPathStorage[9][256] {};

char* gGraphicPaths[9] {};
char* gGraphicInfoPaths[9] {};
char* gAnimePaths[9] {};
char* gAnimeInfoPaths[9] {};
char* gCoordinatePaths[9] {};
char* gCoordinateInfoPaths[9] {};

int BuildResourcePaths()
{
    for (int slot = 0; slot < 9; ++slot) {
        gGraphicPaths[slot] = gGraphicPathStorage[slot];
        gGraphicInfoPaths[slot] = gGraphicInfoPathStorage[slot];
        gAnimePaths[slot] = gAnimePathStorage[slot];
        gAnimeInfoPaths[slot] = gAnimeInfoPathStorage[slot];
        gCoordinatePaths[slot] = gCoordinatePathStorage[slot];
        gCoordinateInfoPaths[slot] = gCoordinateInfoPathStorage[slot];
    }

    int result = 0;
    for (int slot = 0; slot < 9; ++slot) {
        std::sprintf(gGraphicPaths[slot], "%s\\%s", gBinPath, gResourceNames[GraphicNames][slot]);
        std::sprintf(gGraphicInfoPaths[slot], "%s\\%s", gBinPath, gResourceNames[GraphicInfoNames][slot]);
        std::sprintf(gAnimePaths[slot], "%s\\%s", gBinPath, gResourceNames[AnimeNames][slot]);
        std::sprintf(gAnimeInfoPaths[slot], "%s\\%s", gBinPath, gResourceNames[AnimeInfoNames][slot]);
        std::sprintf(gCoordinatePaths[slot], "%s\\%s", gBinPath, gResourceNames[CoordinateNames][slot]);
        result = std::sprintf(
            gCoordinateInfoPaths[slot], "%s\\%s", gBinPath, gResourceNames[CoordinateInfoNames][slot]);
    }
    return result;
}

} // namespace cg::resource
