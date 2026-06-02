#include "world/Movement.hpp"

#include <cmath>
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

[[nodiscard]] bool NearlyEqual(float lhs, float rhs) noexcept
{
    return std::fabs(lhs - rhs) < 0.0001F;
}

[[nodiscard]] bool CoordNearlyEqual(cg::world::WorldPixelCoordF lhs, cg::world::WorldPixelCoordF rhs) noexcept
{
    return NearlyEqual(lhs.x, rhs.x) && NearlyEqual(lhs.y, rhs.y);
}

int TestPixelMidpoint()
{
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {128, 64},
        1000,
        1000,
        cg::world::Direction::East);
    const cg::world::MovementSample sample = cg::world::SampleMovement(movement, 1500);

    int failures = 0;
    failures += Expect(CoordNearlyEqual(sample.position, {64.0F, 32.0F}), "像素移动中点位置正确");
    failures += Expect(NearlyEqual(sample.progress, 0.5F), "像素移动中点 progress 为 0.5");
    failures += Expect(!sample.finished, "像素移动中点未完成");
    return failures;
}

int TestTileMovementUses64Pixels()
{
    const cg::world::Movement movement = cg::world::MakeTileMovement(
        {1, -2},
        {3, -1},
        100,
        200,
        cg::world::Direction::Southeast);

    int failures = 0;
    failures += Expect(movement.start == cg::world::WorldPixelCoord{64, -128}, "tile 起点转换为 64px world pixel");
    failures += Expect(movement.target == cg::world::WorldPixelCoord{192, -64}, "tile 终点转换为 64px world pixel");
    return failures;
}

int TestNowBeforeStart()
{
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {10, 20},
        {110, 120},
        1000,
        500,
        cg::world::Direction::North);
    const cg::world::MovementSample sample = cg::world::SampleMovement(movement, 999);

    int failures = 0;
    failures += Expect(CoordNearlyEqual(sample.position, {10.0F, 20.0F}), "开始前保持起点");
    failures += Expect(NearlyEqual(sample.progress, 0.0F), "开始前 progress 为 0");
    failures += Expect(!sample.finished, "开始前未完成");
    return failures;
}

int TestDurationZero()
{
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {10, 20},
        {30, 40},
        1000,
        0,
        cg::world::Direction::South);
    const cg::world::MovementSample beforeStart = cg::world::SampleMovement(movement, 999);
    const cg::world::MovementSample atStart = cg::world::SampleMovement(movement, 1000);
    const cg::world::MovementSample afterStart = cg::world::SampleMovement(movement, 1001);

    int failures = 0;
    failures += Expect(CoordNearlyEqual(beforeStart.position, {10.0F, 20.0F}), "duration 为 0 且开始前保持起点");
    failures += Expect(NearlyEqual(beforeStart.progress, 0.0F), "duration 为 0 且开始前 progress 为 0");
    failures += Expect(!beforeStart.finished, "duration 为 0 且开始前未完成");
    failures += Expect(CoordNearlyEqual(atStart.position, {30.0F, 40.0F}), "duration 为 0 且开始时到达目标");
    failures += Expect(NearlyEqual(atStart.progress, 1.0F), "duration 为 0 且开始时 progress 为 1");
    failures += Expect(atStart.finished, "duration 为 0 且开始时完成");
    failures += Expect(CoordNearlyEqual(afterStart.position, {30.0F, 40.0F}), "duration 为 0 且开始后保持目标");
    failures += Expect(NearlyEqual(afterStart.progress, 1.0F), "duration 为 0 且开始后 progress 为 1");
    failures += Expect(afterStart.finished, "duration 为 0 且开始后完成");
    return failures;
}

int TestAtEndAndAfterEnd()
{
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {40, 80},
        100,
        300,
        cg::world::Direction::West);
    const cg::world::MovementSample atEnd = cg::world::SampleMovement(movement, 400);
    const cg::world::MovementSample afterEnd = cg::world::SampleMovement(movement, 401);

    int failures = 0;
    failures += Expect(CoordNearlyEqual(atEnd.position, {40.0F, 80.0F}), "结束时到达目标");
    failures += Expect(NearlyEqual(atEnd.progress, 1.0F), "结束时 progress 为 1");
    failures += Expect(atEnd.finished, "结束时完成");
    failures += Expect(CoordNearlyEqual(afterEnd.position, {40.0F, 80.0F}), "结束后保持目标");
    failures += Expect(NearlyEqual(afterEnd.progress, 1.0F), "结束后 progress 为 1");
    failures += Expect(afterEnd.finished, "结束后完成");
    return failures;
}

int TestNegativeCoordInterpolation()
{
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {-100, -50},
        {100, 150},
        0,
        400,
        cg::world::Direction::Northeast);
    const cg::world::MovementSample sample = cg::world::SampleMovement(movement, 100);

    int failures = 0;
    failures += Expect(CoordNearlyEqual(sample.position, {-50.0F, 0.0F}), "负坐标线性插值正确");
    failures += Expect(NearlyEqual(sample.progress, 0.25F), "负坐标插值 progress 正确");
    return failures;
}

int TestDirectionPassThrough()
{
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {0, 64},
        0,
        100,
        cg::world::Direction::Northwest);
    const cg::world::MovementSample sample = cg::world::SampleMovement(movement, 50);

    return Expect(sample.direction == cg::world::Direction::Northwest, "direction 原样透传");
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestPixelMidpoint();
    failures += TestTileMovementUses64Pixels();
    failures += TestNowBeforeStart();
    failures += TestDurationZero();
    failures += TestAtEndAndAfterEnd();
    failures += TestNegativeCoordInterpolation();
    failures += TestDirectionPassThrough();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
