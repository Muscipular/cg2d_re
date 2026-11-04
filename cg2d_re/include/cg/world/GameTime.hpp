#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::session { class ClientSession; }

namespace cg::world {

// 原 tm_cg@0x73ED50；六个 32 位值，不采用 CRT tm 或 Win32 SYSTEMTIME 布局。
struct GameTime {
    std::uint32_t year; // 原成员偏移：+0x00。
    std::uint32_t month; // 原成员偏移：+0x04。
    std::uint32_t day; // 原成员偏移：+0x08。
    std::uint32_t hours; // 原成员偏移：+0x0C。
    std::uint32_t sTick; // 原成员偏移：+0x10。
    std::uint32_t minutes; // 原成员偏移：+0x14。
};
static_assert(sizeof(GameTime) == 24);
static_assert(offsetof(GameTime, year) == 0);
static_assert(offsetof(GameTime, month) == 4);
static_assert(offsetof(GameTime, day) == 8);
static_assert(offsetof(GameTime, hours) == 12);
static_assert(offsetof(GameTime, sTick) == 16);
static_assert(offsetof(GameTime, minutes) == 20);

// 校时生产、逐帧更新及窗口/地图取数共用本会话的时间现场。
struct GameTimeRuntimeState {
    GameTime value {}; // 原73ED50。
    std::uint32_t epochSeconds = 0x3AE805E7; // 原559508，保持可写。
    std::uint32_t adjustmentSeconds = 0; // 原73ED40。
    std::uint32_t serverSeconds = 0; // 原73ED4C。
    std::uint64_t localSeconds = 0; // 原73ED68，与value不重叠。
};

// GTP01：原 [0x458DE0,0x458EBB)；六 W32 原写序/两独立采样合同，返回末次 phase/192 商。
// output 须指向有效 GameTime；写入顺序为 year/month/day/hours/minutes/sTick。
int UpdateGameTime(session::ClientSession& session, GameTime* output);

// MHT01：原 GetCate@0x458EC0；单读 hours W32，保有符号 19/24/0/4/7/16 分类门槛。
// time 须指向有效 GameTime；仅读取 hours 一次，不做取值归一化。
std::int32_t GetCate_458EC0(const GameTime* time);

} // namespace cg::world
