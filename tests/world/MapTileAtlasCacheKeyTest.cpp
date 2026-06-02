#include "world/MapTileAtlasCacheKey.hpp"

#include "world/IdaRenderRules.hpp"

#include <cstdint>
#include <iostream>
#include <unordered_map>

static_assert(
    cg::world::MapTileAtlasSpecialLayerMask
        == (1U << static_cast<std::uint32_t>(cg::world::TileRenderLayer::Special)));
static_assert(
    cg::world::MapTileAtlasNormalLayerMask
        == (1U << static_cast<std::uint32_t>(cg::world::TileRenderLayer::Normal)));
static_assert(cg::world::MapTileAtlasAllLayerMask == (cg::world::MapTileAtlasSpecialLayerMask | cg::world::MapTileAtlasNormalLayerMask));

namespace {

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

cg::world::MapTileAtlasCacheKey MakeBaseKey()
{
    return cg::world::MapTileAtlasCacheKey{
        100,
        2,
        3,
        cg::world::MapTileAtlasAllLayerMask,
        4,
        5,
    };
}

int TestDefaultValues()
{
    const cg::world::MapTileAtlasCacheKey key{};

    int failures = 0;
    failures += Expect(key.mapId == 0, "默认 mapId 为 0");
    failures += Expect(key.serverNo == 0, "默认 serverNo 为 0");
    failures += Expect(key.floor == 0, "默认 floor 为 0");
    failures += Expect(key.tileLayerMask == 0U, "默认 tileLayerMask 为 0");
    failures += Expect(key.mapGeneration == 0U, "默认 mapGeneration 为 0");
    failures += Expect(key.resourceGeneration == 0U, "默认 resourceGeneration 为 0");
    return failures;
}

int TestEqualKeyLookup()
{
    const cg::world::MapTileAtlasCacheKey key = MakeBaseKey();
    const cg::world::MapTileAtlasCacheKey sameKey = MakeBaseKey();

    std::unordered_map<cg::world::MapTileAtlasCacheKey, int, cg::world::MapTileAtlasCacheKeyHash> cache;
    cache.emplace(key, 7);

    int failures = 0;
    failures += Expect(key == sameKey, "相同 key 应相等");
    failures += Expect(cache.find(sameKey) != cache.end(), "相同 key 应命中 unordered_map");
    return failures;
}

int TestGenerationAndMaskAffectEquality()
{
    const cg::world::MapTileAtlasCacheKey key = MakeBaseKey();

    cg::world::MapTileAtlasCacheKey changedMapId = key;
    changedMapId.mapId += 1;

    cg::world::MapTileAtlasCacheKey changedServerNo = key;
    changedServerNo.serverNo += 1;

    cg::world::MapTileAtlasCacheKey changedFloor = key;
    changedFloor.floor += 1;

    cg::world::MapTileAtlasCacheKey changedMapGeneration = key;
    changedMapGeneration.mapGeneration += 1U;

    cg::world::MapTileAtlasCacheKey changedResourceGeneration = key;
    changedResourceGeneration.resourceGeneration += 1U;

    cg::world::MapTileAtlasCacheKey changedTileLayerMask = key;
    changedTileLayerMask.tileLayerMask = cg::world::MapTileAtlasNormalLayerMask;

    int failures = 0;
    failures += Expect(key != changedMapId, "mapId 变化应不等");
    failures += Expect(key != changedServerNo, "serverNo 变化应不等");
    failures += Expect(key != changedFloor, "floor 变化应不等");
    failures += Expect(key != changedMapGeneration, "mapGeneration 变化应不等");
    failures += Expect(key != changedResourceGeneration, "resourceGeneration 变化应不等");
    failures += Expect(key != changedTileLayerMask, "tileLayerMask 变化应不等");
    return failures;
}

int TestEqualKeyHash()
{
    const cg::world::MapTileAtlasCacheKey key = MakeBaseKey();
    const cg::world::MapTileAtlasCacheKey sameKey = MakeBaseKey();
    const cg::world::MapTileAtlasCacheKeyHash hasher;

    return Expect(hasher(key) == hasher(sameKey), "相同 key hash 应相同");
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestDefaultValues();
    failures += TestEqualKeyLookup();
    failures += TestGenerationAndMaskAffectEquality();
    failures += TestEqualKeyHash();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
