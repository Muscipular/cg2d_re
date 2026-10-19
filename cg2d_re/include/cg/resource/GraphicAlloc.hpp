#pragma once

#include <cstdint>

namespace cg::resource {

// 原 9A4A50、D2D5FC、CC3E68；其他消费者须复用同一槽。
extern std::uint32_t gValue9A4A50; // 原全局：0x009A4A50。
extern std::uint32_t gValueD2D5FC; // 原全局：0x00D2D5FC。
extern std::uint32_t gValueCC3E68; // 原全局：0x00CC3E68。

// 原4574E0；BLM01按配置容量发布四块、联合失败或按序清零，旧路径探测由加载器承接。
std::int32_t GraphicAlloc_4574E0(); // 四块发布／失败清理／清零原序保持，完整owner待。
// 原 457670：依序释放 P1/P2/P3，既不清槽也不释放 P4。
void FreePrimaryGraphicResourceTables_457670(); // GAP01：原三块逐次取样/同指针free，P4不释放/共享槽不清，完整owner待。

} // namespace cg::resource
