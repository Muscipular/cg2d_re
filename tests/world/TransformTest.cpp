#include "world/Transform.hpp"

#include <cstdint>
#include <iostream>

namespace {

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

int TestTileToWorldPixel()
{
    int failures = 0;
    failures += Expect(
        cg::world::TileToWorldPixel({3, -2}) == cg::world::WorldPixelCoord{192, -128},
        "tile 转 world pixel 应为 x*64, y*64");
    return failures;
}

int TestProjectOrigin()
{
    int failures = 0;
    const cg::world::ScreenPixelCoord origin{100, 200};
    failures += Expect(
        cg::world::ProjectTileDeltaToScreen({0, 0}, origin) == origin,
        "delta 0 投影应保持 origin");
    failures += Expect(origin == cg::world::ScreenPixelCoord{100, 200}, "origin 入参保持不变");
    return failures;
}

int TestProjectPositiveDelta()
{
    int failures = 0;
    constexpr cg::world::ScreenPixelCoord origin{100, 200};
    failures += Expect(
        cg::world::ProjectTileDeltaToScreen({1, 0}, origin) == cg::world::ScreenPixelCoord{132, 176},
        "+x delta 投影正确");
    failures += Expect(
        cg::world::ProjectTileDeltaToScreen({0, 1}, origin) == cg::world::ScreenPixelCoord{132, 224},
        "+y delta 投影正确");
    failures += Expect(
        cg::world::ProjectTileDeltaToScreen({1, 1}, origin) == cg::world::ScreenPixelCoord{164, 200},
        "+x+y delta 投影正确");
    return failures;
}

int TestProjectNegativeDelta()
{
    int failures = 0;
    constexpr cg::world::ScreenPixelCoord origin{100, 200};
    failures += Expect(
        cg::world::ProjectTileDeltaToScreen({-1, 0}, origin) == cg::world::ScreenPixelCoord{68, 224},
        "负 delta 投影正确");
    return failures;
}

int TestDirectionValues()
{
    int failures = 0;
    failures += Expect(static_cast<std::int8_t>(cg::world::Direction::None) == -1, "Direction::None 值为 -1");
    failures += Expect(static_cast<std::int8_t>(cg::world::Direction::North) == 0, "Direction::North 值为 0");
    failures += Expect(static_cast<std::int8_t>(cg::world::Direction::Northeast) == 1, "Direction::Northeast 值为 1");
    failures += Expect(static_cast<std::int8_t>(cg::world::Direction::East) == 2, "Direction::East 值为 2");
    failures += Expect(static_cast<std::int8_t>(cg::world::Direction::Southeast) == 3, "Direction::Southeast 值为 3");
    failures += Expect(static_cast<std::int8_t>(cg::world::Direction::South) == 4, "Direction::South 值为 4");
    failures += Expect(static_cast<std::int8_t>(cg::world::Direction::Southwest) == 5, "Direction::Southwest 值为 5");
    failures += Expect(static_cast<std::int8_t>(cg::world::Direction::West) == 6, "Direction::West 值为 6");
    failures += Expect(static_cast<std::int8_t>(cg::world::Direction::Northwest) == 7, "Direction::Northwest 值为 7");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestTileToWorldPixel();
    failures += TestProjectOrigin();
    failures += TestProjectPositiveDelta();
    failures += TestProjectNegativeDelta();
    failures += TestDirectionValues();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
