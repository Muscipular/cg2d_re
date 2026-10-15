#pragma once

#include <cstdint>

namespace cg::ui {

// 原 sub_5049A0@[0x5049A0,0x504A25)。
// 检测鼠标是否位于[left,right)×[top,bottom)，返回S32 0/1；命中且layer非负时描边。
// 四次CryptInt比较按X上界/X下界/Y上界/Y下界短路，不缓存鼠标坐标。
std::int32_t HitTestMouseRect_5049A0(
    std::int32_t left,
    std::int32_t top,
    std::int32_t right,
    std::int32_t bottom,
    std::int32_t layer);

} // namespace cg::ui
