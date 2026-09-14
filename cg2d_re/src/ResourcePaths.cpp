#include "cg/ResourcePaths.hpp"

#include "cg/ResourceNames.hpp"

#include <cstdio>

namespace cg::resource {

char gBinPath[128] = "Bin";

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
