#pragma once

#include <array>
#include <string>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::login {

// 原 0xD69798 的服务器记录；507C80生产，登录查询读取同一地址／端口文本。
struct LoginServerRecord {
    std::array<char, 0x80> address; // +00：最多127字节及NUL。
    std::array<char, 0x40> portText; // +80：最多63字节及NUL，查询时由atoi解析。
    std::uint8_t available; // +C0：地址及端口写完后发布。
};

static_assert(std::is_standard_layout_v<LoginServerRecord>);
static_assert(std::is_trivially_copyable_v<LoginServerRecord>);
static_assert(offsetof(LoginServerRecord, address) == 0);
static_assert(offsetof(LoginServerRecord, portText) == 0x80);
static_assert(offsetof(LoginServerRecord, available) == 0xC0);
static_assert(alignof(LoginServerRecord) == 1);
static_assert(sizeof(LoginServerRecord) == 0xC1);

// 以下存储均被 LoginScene::Process 之外的原函数交叉引用，保持唯一公开定义。
extern std::array<LoginServerRecord, 800> gLoginServerRecords_D69798; // 原 0xD69798，映像零填充。
extern std::array<std::int32_t, 60> gLoginServerGroupCounts_D9F2D4;       // 原 0xD9F2D4，映像零填充。
extern std::int16_t gSelectedServerIndex; // 原 0x58AD94，服务器索引，映像初值 -1。
extern std::wstring gPassword;               // 原DA38DC，密码／NLD01第2文本，映像零填充。
extern std::wstring gExecutableDigest;       // 原DA3A1C，可执行文件摘要／NLD01第1文本，映像零填充。
extern std::wstring gAccount;                // 原DA3A5C，账号／NLD01第3文本，映像零填充。
extern std::int16_t gLoginReplyErrorCode_DA3B24;                       // 原 0xDA3B24，映像零填充。

// 原 LoginScene::Process@0x45FEA0。
std::int32_t ProcessScene_45FEA0();

// 原 sub_45C2D0@[0x45C2D0,0x45C566)：登录入口外层子状态机。
void ProcessOuterScene_45C2D0();

// 原 sub_45C5A0@[0x45C5A0,0x45C7C0)：能力 0/1 的登录场景子状态机。
void ProcessModeScene_45C5A0();

} // namespace cg::login
