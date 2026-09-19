#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::world {

// 原 tm_cg@0x73ED50；六个 32 位值，不采用 CRT tm 或 Win32 SYSTEMTIME 布局。
struct GameTime {
    std::uint32_t year;
    std::uint32_t month;
    std::uint32_t day;
    std::uint32_t hours;
    std::uint32_t sTick;
    std::uint32_t minutes;
};
static_assert(sizeof(GameTime) == 24);
static_assert(offsetof(GameTime, year) == 0);
static_assert(offsetof(GameTime, month) == 4);
static_assert(offsetof(GameTime, day) == 8);
static_assert(offsetof(GameTime, hours) == 12);
static_assert(offsetof(GameTime, sTick) == 16);
static_assert(offsetof(GameTime, minutes) == 20);

extern GameTime gCgTime;
extern std::uint32_t gValue559508; // 原静态字节 0x3AE805E7，时间计算基值。
extern std::uint32_t gValue73ED40; // 原 0x50C8C2 写入，完整业务角色仍待上游核对。
extern std::uint32_t gValue73ED4C; // 原 0x50C880 写入。
extern std::uint32_t gValue73ED68; // 原 0x50C890 写入，与输出对象不重叠。

// 原 [0x458DE0,0x458EBB)；返回末次相位除以 192 的商，不是更新成功标志。
// output 须指向有效 GameTime；写入顺序为 year/month/day/hours/minutes/sTick。
int UpdateGameTime(GameTime* output);

} // namespace cg::world
