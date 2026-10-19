#pragma once

#include <cstdint>

namespace cg::resource {

// 原D2D5FC；图鉴编号校验与掩码同步共用的上限。
extern std::uint32_t gAlbumEntryLimit_D2D5FC; // 原全局：0x00D2D5FC。

// 原4574E0；BLM01按配置容量发布四块、联合失败或按序清零，旧路径探测由加载器承接。
std::int32_t GraphicAlloc_4574E0(); // 四块发布／失败清理／清零原序保持，完整owner待。
// 原 457670：依序释放 P1/P2/P3，既不清槽也不释放 P4。
void FreePrimaryGraphicResourceTables_457670(); // GAP01：原三块逐次取样/同指针free，P4不释放/共享槽不清，完整owner待。

} // namespace cg::resource
