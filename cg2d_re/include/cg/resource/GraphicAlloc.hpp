#pragma once

#include <cstdint>

namespace cg::resource {

// 原D2D5FC；图鉴编号校验与掩码同步共用的上限。
extern std::uint32_t gAlbumEntryLimit_D2D5FC; // 原全局：0x00D2D5FC。

// 原4574E0；RAC01在图形／动画／坐标索引完成后剔除无引用图形，发布实际容量缓存。
std::int32_t GraphicAlloc_4574E0();
// 原457670；RAC01活动槽清理后释放实际表owner及全部借用，包含动画缓存。
void FreePrimaryGraphicResourceTables_457670();

} // namespace cg::resource
