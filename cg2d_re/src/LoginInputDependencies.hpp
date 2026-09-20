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

// 原 [0xFF494F,0xFF4982)：当前保存记录内的第二输入槽。
char* GetCurrentSavedInputValueFF494F();
// 原 [0xFF4982,0xFF49B5)：当前保存记录内的第三输入槽。
char* GetCurrentSavedInputValueFF4982();

// 原 dword_105C394@0x105C394：当前选中的持久槽索引，引用唯一共享存储。
extern std::int32_t& gSelectedSavedSlot105C394;
// 原 dword_105C364@0x105C364：能力 4 当前可见持久槽数量。
extern std::int32_t& gVisibleSavedSlotCount105C364;

} // namespace cg::login::detail
