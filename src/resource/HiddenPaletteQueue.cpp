#include "HiddenPaletteQueue.hpp"

#include <array>
#include <cstddef>

namespace cg::resource {
namespace {

struct HiddenPaletteQueueSegment {
    int palBegin;
    int palEnd;
    int mapNoBegin;
};

// 来自 IDA sub_522B30 读取的 0x58B7E8..0x58D348 静态 queue1。
// queue1 值是 hidden palette Graphic 的 mapNo，不是 Graphic seq/globalIndex。
// 段表按 pal 顺序压缩；重复 mapNo 位置保留为独立 pal 区间。
constexpr std::array<HiddenPaletteQueueSegment, 60> StaticQueueSegments {{
    { 0, 0, 0 },
    { 1, 74, 110300 },
    { 75, 90, 110380 },
    { 91, 105, 110400 },
    { 106, 154, 110420 },
    { 155, 156, 110500 },
    { 157, 192, 110600 },
    { 193, 228, 110650 },
    { 229, 244, 110700 },
    { 245, 261, 110750 },
    { 262, 1261, 113000 },
    { 1262, 1273, 110401 },
    { 1274, 1274, 110414 },
    { 1275, 1275, 110504 },
    { 1276, 1279, 110376 },
    { 1280, 1288, 110469 },
    { 1289, 1354, 110520 },
    { 1355, 1389, 114165 },
    { 1390, 1392, 110767 },
    { 1393, 1396, 110505 },
    { 1397, 1400, 110396 },
    { 1401, 1406, 110509 },
    { 1407, 1420, 110586 },
    { 1421, 1450, 110770 },
    { 1451, 1475, 114200 },
    { 1476, 1476, 110599 },
    { 1477, 1477, 110400 },
    { 1478, 1480, 104332 },
    { 1481, 1489, 104344 },
    { 1490, 1494, 104339 },
    { 1495, 1534, 104353 },
    { 1535, 1535, 104400 },
    { 1536, 1536, 104406 },
    { 1537, 1537, 104412 },
    { 1538, 1538, 104418 },
    { 1539, 1539, 104424 },
    { 1540, 1540, 104430 },
    { 1541, 1541, 104436 },
    { 1542, 1542, 104442 },
    { 1543, 1543, 104448 },
    { 1544, 1544, 104454 },
    { 1545, 1545, 104460 },
    { 1546, 1546, 104466 },
    { 1547, 1547, 104472 },
    { 1548, 1548, 104478 },
    { 1549, 1549, 104484 },
    { 1550, 1550, 104490 },
    { 1551, 1551, 104496 },
    { 1552, 1552, 104502 },
    { 1553, 1553, 104508 },
    { 1554, 1554, 104514 },
    { 1555, 1564, 104393 },
    { 1565, 1605, 104520 },
    { 1606, 1607, 104335 },
    { 1608, 1673, 104561 },
    { 1674, 1689, 104669 },
    { 1690, 1691, 104337 },
    { 1692, 1733, 104627 },
    { 1734, 1747, 104685 },
    { 1748, 1751, 104700 },
}};

} // namespace

std::array<int, 1752> BuildStaticHiddenPaletteQueue() noexcept
{
    std::array<int, 1752> queue {};
    for (const HiddenPaletteQueueSegment& segment : StaticQueueSegments) {
        for (int palIndex = segment.palBegin; palIndex <= segment.palEnd; ++palIndex) {
            queue[static_cast<std::size_t>(palIndex)] = segment.mapNoBegin + (palIndex - segment.palBegin);
        }
    }
    return queue;
}

} // namespace cg::resource
