#pragma once


#include <cstdio>
#include <cstdint>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <direct.h>
// DCS01：以下原纯转发接口由MSVC CRT提供，无项目包装。
// 原函数参考：0x0053BDEF、0x0053BE17、0x0053CF50、0x0054AB30、0x0054518B、0x0053C5BE、0x0053DA45、0x0053C649、0x0053C3AC、0x0053C0A9、0x0054ABDD、0x0053D26E、0x0053BFC0、0x005450F0、0x0053F9E0、0x00544DB0、0x00544DC0、0x0053C790、0x0053BEC0、0x0053C2F0、0x0053BE40、0x0053C654、0x0053DD2F。
// 原全局0x01085FE0／0x01085FE4：当前线程errno／_doserrno由MSVC持有。

namespace cg::platform {


// 九个业务S32日历字段；由_localtime32逐字段发布到当前线程的借用对象。
struct LegacyTm53B8C6 {
    std::int32_t seconds;       // +00 秒
    std::int32_t minutes;       // +04 分
    std::int32_t hours;         // +08 时
    std::int32_t day;           // +0C 月内日
    std::int32_t month0based;   // +10 从零起的月份
    std::int32_t yearMinus1900; // +14 距1900年
    std::int32_t weekDay;       // +18 星期
    std::int32_t yearDay0based; // +1C 从零起的年内日
    std::int32_t dst;           // +20 夏令时标记
};

// 原53B8C6：按九个S32字段返回当前线程的借用时间对象。
LegacyTm53B8C6* __cdecl LocalTime_53B8C6(std::int32_t* timeValue);

// 原53BA26：直接MSVC _time32，业务结果保持S32；output可空。
std::int32_t ReadLegacyTime32_53BA26(std::int32_t* output);

// auto.dat时间：同一MSVC FILE经_fileno/_fstat32，成功才发布S32创建时间。
std::int32_t ReadLegacyFileCreationTime32(std::FILE* file, std::int32_t* output);

} // namespace cg::platform
