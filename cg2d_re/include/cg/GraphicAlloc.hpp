#pragma once

#include <cstdint>

namespace cg::resource {

// 原 9A4A50、D2D5FC、CC3E68；其他消费者须复用同一槽。
extern std::uint32_t gValue9A4A50;
extern std::uint32_t gValueD2D5FC;
extern std::uint32_t gValueCC3E68;

// 原 4574E0：探测路径、发布四块、联合失败或按序清零。
std::int32_t GraphicAlloc_4574E0();
// 原 457670：依序释放 P1/P2/P3，既不清槽也不释放 P4。
void Sub_457670();

} // namespace cg::resource
