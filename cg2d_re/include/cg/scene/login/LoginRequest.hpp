#pragma once

namespace cg::network { class CgConnection; }

#include <array>
#include "cg/text/Encoding.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>

namespace cg::login {

inline constexpr std::size_t CharacterRecordSize = 0x94;
inline constexpr std::size_t CharacterRecordCount = 2;
inline constexpr std::size_t RequestMessageCapacity = 1024;

// 原FF03E0起的角色列表记录；生产者51B1B0，登录／选择场景共用。
struct CharacterRecord {
    std::wstring name; // +00；旧编码姓名，最多16字节及NUL。
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
    std::wstring jobText; // +54；列表t16，最多63个旧编码字节及NUL。
};

using CharacterRecordArray = std::array<CharacterRecord, CharacterRecordCount>;


// 原 sub_51B3B0@[0x51B3B0,0x51B3DC)：只清合法的 0/1 角色记录。
std::int32_t ClearCharacterSlot_51B3B0(std::uint32_t selectedIndex);
std::int32_t ClearCharacterSlot_51B3B0(network::CgConnection& connection, std::uint32_t selectedIndex);

// 原 sub_51B110@[0x51B110,0x51B136)：合法槽读首字节返回 0/1，越界返回 -1。
std::int32_t HasCharacterInSlot_51B110(std::uint32_t selectedIndex);
std::int32_t HasCharacterInSlot_51B110(network::CgConnection& connection, std::uint32_t selectedIndex);

// 原 sub_51B140@[0x51B140,0x51B1A1)：按名字查找两条角色记录。
std::int32_t CharacterNameExists_51B140(std::wstring_view name);
std::int32_t CharacterNameExists_51B140(network::CgConnection& connection, std::wstring_view name);

// NSM01：登录/角色/登出请求共用的状态随连接实例保留，重连不清。
struct LoginRequestState {
    CharacterRecordArray characters {}; // 原 0xFF03E0，两条角色列表记录。
    std::int16_t selectedCharacterIndex = 0; // 原 0xFF0C08，跨登录、世界及存档的角色槽位。
    std::uint16_t characterListReplyCode = 0; // 原 0xDA3B26，R/W16。
    std::uint16_t characterDeletionReplyCode = 0; // 原 0xDA3B28，R/W16。
    std::uint16_t characterCreationReplyCode = 0; // 原 0xDA3B2A，R/W16。
    std::uint16_t loginReplyCode = 0; // 原 0xDA3B2C，R/W16。
    std::int32_t status = 0; // 原 0xDA3B34，R/W32。
    std::uint64_t startTick = 0; // 原 0xDA3B38；CTM01本地64位毫秒。
    std::wstring message; // 原 0xDA34DC。
    std::uint16_t logoutReplyCode = 0; // 原 0xDA3B2E，独立于登录回复码。
    std::int32_t connectionStep = 0; // 原 0xDA3B50，服务器连接步骤/轮询计数。
    std::int16_t loginErrorCode = 0; // 原 0xDA3B24，账号登录回复错误码。
    std::int16_t selectedServerIndex = -1; // 原 0x58AD94，登录服务器索引。
    std::uint16_t serverConnectionPhase = 0; // 原 0x9A6E3C，连接提示阶段。
    std::int32_t selectedLoginServerGroup = -1; // 原 0x55A054，分类与邮件读取共用。
    std::int32_t serverHighlight = 0; // 原 0x9A5B04，登录服务器高亮槽。
    std::int32_t serverPage = 0; // 原 0x9A6EA4，登录服务器分页。
    std::uint32_t connectionErrorPresent = 0; // 原 0x593FC0，断线说明标志。
    std::wstring processingErrorMessage; // 原 0x9A5348，角色登录失败提示。
    std::wstring sceneMessage; // 原 0x9A60F0，登录场景错误提示。
    std::int32_t openUrlAfterMessage = 0; // 原 0x9A6EA8，提示完成后的链接请求。
    std::wstring password; // 原 0xDA38DC，账号登录第2文本。
    std::wstring account; // 原 0xDA3A5C，账号登录第3文本。
};

// 原 sub_508AF0@0x508AF0；前置条件是有符号选择值位于 [0, 1]。
// 原函数不检查范围，本接口同样不为无效输入增加业务分支。
std::uint32_t StartLoginRequest(network::CgConnection& connection);

// 原 sub_508B50@0x508B50；轮询完成、失败或 600000 ms 超时。
std::int32_t PollLoginRequest(network::CgConnection& connection);

// 原 sub_508BC0@[0x508BC0,0x508C39)：caller 压三参，本体只读前两项。
void HandleLoginResponse_508BC0(cg::network::CgConnection& networkConnection, const char* result, const char* unusedText);

} // namespace cg::login
