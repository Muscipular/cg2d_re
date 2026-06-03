#include "world/IdaRenderRules.hpp"

#include <climits>
#include <cstdint>
#include <iostream>
#include <optional>

namespace {

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

template <typename T>
int ExpectOptionalEquals(std::optional<T> actual, T expected, const char* message)
{
    int failures = Expect(actual.has_value(), message);
    if (actual.has_value()) {
        failures += Expect(*actual == expected, message);
    }
    return failures;
}

int TestDecodeMapCellForWorld()
{
    int failures = 0;
    failures += Expect(cg::world::DecodeMapCellForWorld(0U) == 0U, "raw 0 保持不变");
    failures += Expect(cg::world::DecodeMapCellForWorld(19999U) == 19999U, "raw 19999 保持不变");
    failures += Expect(cg::world::DecodeMapCellForWorld(20000U) == 220000U, "raw 20000 加 200000");
    failures += Expect(cg::world::DecodeMapCellForWorld(0xFFFFU) == 265535U, "raw 0xFFFF 仍按低位表转换");
    failures += Expect(cg::world::DecodeMapCellForWorld(0x10000U) == 0x10000U, "raw 0x10000 原样返回");
    return failures;
}

int TestClassifyWorldTileLayer()
{
    int failures = 0;
    failures += Expect(!cg::world::ClassifyWorldTileLayer(999U).has_value(), "decoded <= 999 不绘制");
    failures += ExpectOptionalEquals(
        cg::world::ClassifyWorldTileLayer(1000U),
        cg::world::TileRenderLayer::Normal,
        "decoded 1000 是普通 tile layer");
    failures += ExpectOptionalEquals(
        cg::world::ClassifyWorldTileLayer(5000U),
        cg::world::TileRenderLayer::Special,
        "decoded 5000 是特殊 tile layer");
    failures += ExpectOptionalEquals(
        cg::world::ClassifyWorldTileLayer(5009U),
        cg::world::TileRenderLayer::Special,
        "decoded 5009 是特殊 tile layer");
    failures += ExpectOptionalEquals(
        cg::world::ClassifyWorldTileLayer(5010U),
        cg::world::TileRenderLayer::Normal,
        "decoded 5010 是普通 tile layer");
    failures += ExpectOptionalEquals(
        cg::world::ClassifyWorldTileLayer(7664U),
        cg::world::TileRenderLayer::Special,
        "decoded 7664 是特殊 tile layer");
    failures += ExpectOptionalEquals(
        cg::world::ClassifyWorldTileLayer(7679U),
        cg::world::TileRenderLayer::Special,
        "decoded 7679 是特殊 tile layer");
    failures += ExpectOptionalEquals(
        cg::world::ClassifyWorldTileLayer(7680U),
        cg::world::TileRenderLayer::Normal,
        "decoded 7680 是普通 tile layer");
    failures += Expect(
        static_cast<std::uint32_t>(cg::world::TileRenderLayer::Special) == 1U,
        "特殊 tile layer 值为 1");
    failures += Expect(
        static_cast<std::uint32_t>(cg::world::TileRenderLayer::Normal) == 5U,
        "普通 tile layer 值为 5");
    return failures;
}

int TestComputeDrawGraphicPosition()
{
    int failures = 0;
    const cg::world::GraphicDrawPosition position =
        cg::world::ComputeDrawGraphicPosition(100, 200, -7, 13);
    failures += Expect(position.x == 93, "DrawGraphic x 应叠加 Graphic::getXY x");
    failures += Expect(position.y == 213, "DrawGraphic y 应叠加 Graphic::getXY y");
    return failures;
}

int TestClassifyMapObjectRenderGroup()
{
    int failures = 0;
    failures += ExpectOptionalEquals(
        cg::world::ClassifyMapObjectRenderGroup(1, 3),
        cg::world::MapObjectRenderGroup::MapNoOne,
        "MapNo 1 进入普通 object 队列");
    failures += ExpectOptionalEquals(
        cg::world::ClassifyMapObjectRenderGroup(1000, 3),
        cg::world::MapObjectRenderGroup::HighMapNoFlag2Three,
        "MapNo > 999 且 flag2 == 3 进入末尾追加 object 队列");
    failures += ExpectOptionalEquals(
        cg::world::ClassifyMapObjectRenderGroup(1000, 2),
        cg::world::MapObjectRenderGroup::HighMapNoOtherFlag2,
        "MapNo > 999 且 flag2 != 3 进入参与排序合并的 object 队列");
    failures += Expect(!cg::world::ClassifyMapObjectRenderGroup(999, 3).has_value(), "MapNo 999 不绘制");
    failures += Expect(!cg::world::ClassifyMapObjectRenderGroup(0, 0).has_value(), "MapNo 0 不绘制");
    return failures;
}

int TestMakeIdaCompatibleOrder()
{
    int failures = 0;
    failures += Expect(cg::world::MakeIdaCompatibleOrder(0U) == static_cast<std::uint32_t>(UINT_MAX), "首个 order 为 UINT_MAX");
    failures += Expect(
        cg::world::MakeIdaCompatibleOrder(1U) == static_cast<std::uint32_t>(UINT_MAX) - 1U,
        "第二个 order 为 UINT_MAX - 1");
    failures += Expect(
        cg::world::MakeIdaCompatibleOrder(10U) < cg::world::MakeIdaCompatibleOrder(9U),
        "后入队元素在同 layer 下转换为更小 order");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestDecodeMapCellForWorld();
    failures += TestClassifyWorldTileLayer();
    failures += TestComputeDrawGraphicPosition();
    failures += TestClassifyMapObjectRenderGroup();
    failures += TestMakeIdaCompatibleOrder();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
