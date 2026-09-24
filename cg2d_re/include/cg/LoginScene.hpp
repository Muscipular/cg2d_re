#pragma once

#include <array>
#include <cstdint>

namespace cg::login {

// 原 0xD69798 的单项严格为 0xC1 字节；可用性读末字节，地址查询读前 0xC0 字节。
struct LoginServerRecord {
    std::array<std::uint8_t, 0xC0> opaque;
    std::uint8_t available;
};

static_assert(sizeof(LoginServerRecord) == 0xC1);

// 以下存储均被 LoginScene::Process 之外的原函数交叉引用，保持唯一公开定义。
extern std::array<LoginServerRecord, 800> gValueD69798; // 原 0xD69798，映像零填充。
extern std::array<std::int32_t, 60> gValueD9F2D4;       // 原 0xD9F2D4，映像零填充。
extern std::int16_t gValue58AD94;                       // 原 0x58AD94，映像初值 -1。
extern char gValueDA38DC[64];                           // 原 0xDA38DC，映像零填充。
extern char gValueDA3A1C[64];                           // 原 0xDA3A1C，映像零填充。
extern char gValueDA3A5C[64];                           // 原 0xDA3A5C，映像零填充。
extern std::int16_t gValueDA3B24;                       // 原 0xDA3B24，映像零填充。

// 原 LoginScene::Process@0x45FEA0。
std::int32_t ProcessScene_45FEA0();

// 原 sub_45C2D0@[0x45C2D0,0x45C566)：登录入口外层子状态机。
void ProcessOuterScene_45C2D0();

// 原 sub_45C5A0@[0x45C5A0,0x45C7C0)：能力 0/1 的登录场景子状态机。
void ProcessModeScene_45C5A0();

} // namespace cg::login
