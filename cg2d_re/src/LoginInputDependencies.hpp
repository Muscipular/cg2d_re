#pragma once

#include <cstdint>

namespace cg::login::detail {

// 原 sub_533680@0x533680：按选中槽读取第一项登录输入。
char* Sub_533680(char* output, std::int32_t selectedSlot);
// 原 sub_5335F0@0x5335F0：按选中槽读取第二项登录输入。
char* Sub_5335F0(char* output, std::int32_t selectedSlot);

// 原 sub_533570/sub_5335B0/sub_533640：把三个输入文本写回所选记录。
void Sub_533570(const char* text, std::int32_t selectedSlot);
void Sub_5335B0(const char* text, std::int32_t selectedSlot);
void Sub_533640(const char* text, std::int32_t selectedSlot);
// 原 sub_533730@0x533730：把记录系统持久化到文件；调用方忽略返回值。
void Sub_533730();

// 原 dword_105C394@0x105C394：当前选中的持久槽索引。
extern std::int32_t gSelectedSavedSlot105C394;

} // namespace cg::login::detail
