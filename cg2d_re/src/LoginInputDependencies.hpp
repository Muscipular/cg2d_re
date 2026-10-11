#pragma once

#include <cstdint>

namespace cg::login::detail {

// 原 sub_533680@0x533680：按选中槽读取第一项登录输入。
char* Sub_533680(char* output, std::int32_t selectedSlot);
// 原 sub_5335F0@0x5335F0：按选中槽读取第二项登录输入。
char* Sub_5335F0(char* output, std::int32_t selectedSlot);

// 原 sub_533570/sub_5335B0/sub_533640：把三个输入文本写回所选记录。
void Sub_533570(const char* text, std::int32_t selectedSlot);
// 原函数：0x005335B0。
void Sub_5335B0(const char* text, std::int32_t selectedSlot);
// 原函数：0x00533640。
void Sub_533640(const char* text, std::int32_t selectedSlot);
// 原 sub_533730@0x533730：把记录系统持久化到文件；调用方忽略返回值。
void Sub_533730();

// 能力 4 主场景所需的保存槽窄读取边界；直接读取唯一保存状态 owner。
std::int32_t ReadNewSavedPhysicalSlot105C368();
std::int32_t ReadMappedSavedPhysicalSlot105C36C(std::int32_t index);
// 原 518CB0/519300/519550 的路径读取序列；每次重取逻辑槽和映射，借用同一物理路径。
const char* ReadSelectedSavedDirectory();

// 相册与保存记录共用原52D310/52D460，不复制编解码状态或算法。
void EncodeSavedBytes(
    std::uint8_t* source, std::int32_t sourceBytes, std::int32_t mode,
    std::uint8_t* destination, std::int32_t* encodedBytes, std::int32_t capacity);
std::int32_t DecodeSavedBytes(
    const std::uint8_t* source, std::int32_t sourceBytes, std::int32_t mode,
    std::uint8_t* destination, std::int32_t* decodedBytes);

// 原 sub_5336D0/sub_533860：载入当前记录与删除指定物理槽文件。
std::int32_t Sub_5336D0();
// 原函数：0x00533860。
std::int32_t Sub_533860(std::int32_t physicalSlot);

// 能力 4 切场景专用恢复：严格调用 5204C0/5205E0/520690/520700。
void RestoreCapability4CurrentSavedRecord();

// 原 [0xFF494F,0xFF4982)：当前保存记录内的第二输入槽。
char* GetCurrentSavedInputValueFF494F();
// 原 [0xFF4982,0xFF49B5)：当前保存记录内的第三输入槽。
char* GetCurrentSavedInputValueFF4982();
// 同一510H当前保存记录的+400/+410；摆摊读取默认值，发送路径写回原区。
char* GetCurrentSavedStallNameFF4D18();
char* GetCurrentSavedStallDescriptionFF4D28();

// 原 dword_105C394@0x105C394：当前选中的持久槽索引，引用唯一共享存储。
extern std::int32_t& gSelectedSavedSlot105C394;
// 原 dword_105C364@0x105C364：能力 4 当前可见持久槽数量。
extern std::int32_t& gVisibleSavedSlotCount105C364;

} // namespace cg::login::detail
