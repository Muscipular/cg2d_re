#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::login {

inline constexpr std::size_t CharacterRecordSize = 0x94;
inline constexpr std::size_t CharacterRecordCount = 2;
inline constexpr std::size_t RequestMessageCapacity = 1024;

using CharacterRecordBytes = std::array<std::byte, CharacterRecordSize>;
using CharacterRecordArray = std::array<CharacterRecordBytes, CharacterRecordCount>;

static_assert(sizeof(CharacterRecordBytes) == 0x94);
static_assert(sizeof(CharacterRecordArray) == 0x128);

// 原 [0xFF03E0,0xFF0508) 与 0xFF0C08；均有跨场景引用，映像初值为 0。
extern CharacterRecordArray gCharacterRecords; // 原全局：0x00FF03E0。
extern std::int16_t gSelectedCharacterIndex; // 原全局：0x00FF0C08。

// 原 sub_51B3B0@[0x51B3B0,0x51B3DC)：只清合法的 0/1 角色记录。
std::int32_t Sub_51B3B0(std::uint32_t selectedIndex);

// 原 sub_51B110@[0x51B110,0x51B136)：合法槽读首字节返回 0/1，越界返回 -1。
std::int32_t Sub_51B110(std::uint32_t selectedIndex);

// 原 sub_51B140@[0x51B140,0x51B1A1)：按名字查找两条角色记录。
std::int32_t CharacterNameExists_51B140(const char* name);

// 登录请求与其他网络处理函数共享的固定宽度存储，映像初值均为 0。
extern std::uint16_t gRequestReplyCode;        // 原 0xDA3B2C，R/W16。
extern std::int32_t gRequestStatus;            // 原 0xDA3B34，R/W32。
extern std::uint32_t gRequestStartTick;        // 原 0xDA3B38，R/W32。
extern std::uint8_t gRequestPollingEnabled;    // 原 0xD9F2C9，R/W8。
extern std::int32_t gRequestOutcomeFlag;       // 原 0xDA3B30，R/W32。
extern char gRequestMessage[RequestMessageCapacity]; // 原 0xDA34DC。

// 原 sub_510140@[0x510140,0x510146)；读取共享请求结果标志并原样返回完整 32 位值。
std::int32_t ReadRequestOutcomeFlag();

// 原 sub_508AF0@0x508AF0；前置条件是有符号选择值位于 [0, 1]。
// 原函数不检查范围，本接口同样不为无效输入增加业务分支。
std::uint32_t StartLoginRequest();

// 原 sub_508B50@0x508B50；轮询完成、失败或 600000 ms 超时。
std::int32_t PollLoginRequest();

// 原 sub_508BC0@[0x508BC0,0x508C39)：caller 压三参，本体只读前两项。
void HandleLoginResponse_508BC0(
    std::uintptr_t socket, const char* result, const char* unusedText);

} // namespace cg::login
