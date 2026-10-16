#pragma once

#include <array>
#include "cg/text/Encoding.hpp"
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::login {

inline constexpr std::size_t CharacterRecordSize = 0x94;
inline constexpr std::size_t CharacterRecordCount = 2;
inline constexpr std::size_t RequestMessageCapacity = 1024;

// 原FF03E0起的角色列表记录；生产者51B1B0，登录／选择场景共用。
struct CharacterRecord {
    std::array<char, 17> name; // +00；旧编码姓名，最多16字节及NUL。
    std::byte reserved11; // +11；随整条记录清零的保留字节。
    std::int16_t value12; // +12；列表t3低W16，槽位按S16显示。
    std::int32_t value14; // +14；列表t13，槽位显示数值。
    std::int32_t graphicNo; // +18；列表t2，登录／选择／预览图号。
    std::int32_t value1C; // +1C；列表t18。
    std::int32_t deleteFlag; // +20；列表t19，选择页删除条件。
    std::int32_t graphicClass; // +24；选择页图号切换分类。
    std::int32_t value28; // +28；列表t4。
    std::int32_t value2C; // +2C；列表t5。
    std::int32_t value30; // +30；列表t6。
    std::int32_t value34; // +34；列表t7。
    std::int32_t value38; // +38；列表t8。
    std::int32_t value3C; // +3C；列表t9经signed除10。
    std::int32_t value40; // +40；列表t10经signed除10。
    std::int32_t value44; // +44；列表t11经signed除10。
    std::int32_t value48; // +48；列表t12经signed除10。
    std::int32_t registNumber; // +4C；列表t15／创建回复，登录／删除使用。
    std::int32_t previewState; // +50；列表t17／创建回复，预览切换值。
    std::array<char, 64> jobText; // +54；列表t16，最多63个旧编码字节及NUL。
};

using CharacterRecordArray = std::array<CharacterRecord, CharacterRecordCount>;

static_assert(std::is_standard_layout_v<CharacterRecord>);
static_assert(std::is_trivially_copyable_v<CharacterRecord>);
static_assert(offsetof(CharacterRecord, name) == 0x00);
static_assert(offsetof(CharacterRecord, reserved11) == 0x11);
static_assert(offsetof(CharacterRecord, value12) == 0x12);
static_assert(offsetof(CharacterRecord, value14) == 0x14);
static_assert(offsetof(CharacterRecord, graphicNo) == 0x18);
static_assert(offsetof(CharacterRecord, value1C) == 0x1C);
static_assert(offsetof(CharacterRecord, deleteFlag) == 0x20);
static_assert(offsetof(CharacterRecord, graphicClass) == 0x24);
static_assert(offsetof(CharacterRecord, value28) == 0x28);
static_assert(offsetof(CharacterRecord, value2C) == 0x2C);
static_assert(offsetof(CharacterRecord, value30) == 0x30);
static_assert(offsetof(CharacterRecord, value34) == 0x34);
static_assert(offsetof(CharacterRecord, value38) == 0x38);
static_assert(offsetof(CharacterRecord, value3C) == 0x3C);
static_assert(offsetof(CharacterRecord, value40) == 0x40);
static_assert(offsetof(CharacterRecord, value44) == 0x44);
static_assert(offsetof(CharacterRecord, value48) == 0x48);
static_assert(offsetof(CharacterRecord, registNumber) == 0x4C);
static_assert(offsetof(CharacterRecord, previewState) == 0x50);
static_assert(offsetof(CharacterRecord, jobText) == 0x54);
static_assert(sizeof(CharacterRecord) == CharacterRecordSize);
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
extern std::uint64_t gRequestStartTick;        // 原 0xDA3B38；CTM01本地64位毫秒。
extern std::uint8_t gRequestPollingEnabled;    // 原 0xD9F2C9，R/W8。
extern std::int32_t gRequestOutcomeFlag;       // 原 0xDA3B30，R/W32。
extern std::wstring gRequestMessage; // 原 0xDA34DC。

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
