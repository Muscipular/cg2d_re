#include "cg/platform/LegacyTime.hpp"
#include "StartupNative.hpp"

#include <cstddef>
#include <cstring>

namespace cg::platform {

// 原缓存分别位于 0x1085FD0、0x1085FC8；这里只还原各已访问对象，不推断中间填充用途。
SYSTEMTIME gUtcTimeCache_1085FD0 {};
std::int32_t gDaylightCache_1085FC8 = 0;

// 原 0x1086128..0x108614B：九个连续 S32 槽，不是现代 CRT tm。
struct LegacyTm53B8C6 {
    std::int32_t seconds;       // +00 秒
    std::int32_t minutes;       // +04 分
    std::int32_t hours;         // +08 时
    std::int32_t day;           // +0C 月内日
    std::int32_t month0based;   // +10 从零起的月份
    std::int32_t yearMinus1900; // +14 距 1900 年
    std::int32_t weekDay;       // +18 星期
    std::int32_t yearDay0based; // +1C 从零起的年内日
    std::int32_t dst;           // +20 夏令时标记
};

static_assert(offsetof(LegacyTm53B8C6, seconds) == 0x00);
static_assert(offsetof(LegacyTm53B8C6, minutes) == 0x04);
static_assert(offsetof(LegacyTm53B8C6, hours) == 0x08);
static_assert(offsetof(LegacyTm53B8C6, day) == 0x0C);
static_assert(offsetof(LegacyTm53B8C6, month0based) == 0x10);
static_assert(offsetof(LegacyTm53B8C6, yearMinus1900) == 0x14);
static_assert(offsetof(LegacyTm53B8C6, weekDay) == 0x18);
static_assert(offsetof(LegacyTm53B8C6, yearDay0based) == 0x1C);
static_assert(offsetof(LegacyTm53B8C6, dst) == 0x20);
static_assert(sizeof(LegacyTm53B8C6) == 0x24);

namespace {

// GMT@0x541158 恒返回这一份共享对象；原运行初值不由未加载区字节推断。
LegacyTm53B8C6 gLegacyTm_1086128;

// 原 0x58FB78/7C/80 的映像初值；刷新入口后续可改写，不能视作常量。
std::int32_t gTimezone_58FB78 = 28800;
std::int32_t gDaylight_58FB7C = 1;
std::int32_t gDstBias_58FB80 = -3600;

// 原 0x5921F0 与 0x592224（末项 0x592254），供同文件转换入口复用。
constexpr std::int32_t kLeapDays_5921F0[13] = {
    -1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};
constexpr std::int32_t kPlainDays_592224[13] = {
    -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364
};

// 原 [0x541067,0x541160)：负值不写共享对象；正值按原九次写入顺序覆盖。
LegacyTm53B8C6* GmTime_541067(const std::int32_t* timeValue)
{
    const std::int32_t time = *timeValue;
    if (time < 0) {
        return nullptr;
    }

    const std::int32_t cycle = time / 126230400;
    std::int32_t remaining = time - cycle * 126230400;
    std::int32_t year = 70 + 4 * cycle;
    const std::int32_t* days = kPlainDays_592224;
    if (remaining >= 31536000) {
        remaining -= 31536000;
        ++year;
        if (remaining >= 31536000) {
            remaining -= 31536000;
            ++year;
            if (remaining < 31622400) {
                days = kLeapDays_5921F0;
            } else {
                remaining -= 31622400;
                ++year;
            }
        }
    }

    const std::int32_t yearDay = remaining / 86400;
    std::int32_t index = 1;
    while (days[index] < yearDay) {
        ++index;
    }

    gLegacyTm_1086128.yearMinus1900 = year;             // 0x5410BE
    gLegacyTm_1086128.yearDay0based = yearDay;           // 0x5410D1
    gLegacyTm_1086128.month0based = index - 1;           // 0x5410FF
    gLegacyTm_1086128.day = yearDay - days[index - 1];   // 0x54110B
    // 0x541110 再读原指针；它可以别名于刚被覆写的共享对象。
    gLegacyTm_1086128.weekDay = (*timeValue / 86400 + 4) % 7; // 0x541123
    const std::int32_t daySeconds = remaining - yearDay * 86400;
    gLegacyTm_1086128.hours = daySeconds / 3600;        // 0x54112F
    const std::int32_t hourSeconds = daySeconds % 3600;
    gLegacyTm_1086128.minutes = hourSeconds / 60;       // 0x541141
    gLegacyTm_1086128.dst = 0;                          // 0x54114B
    gLegacyTm_1086128.seconds = hourSeconds % 60;       // 0x541152
    return &gLegacyTm_1086128;
}

} // namespace

// 原 0x540B08 惰性时区入口与 0x540D7B DST 判定尚未实现。
// 后者读取 +14/+1C/+08/+04/+00，内部先检查共享 daylight；禁用时返回 0。
void __cdecl TzSet_540B08();
std::int32_t __cdecl IsDst_540D7B(LegacyTm53B8C6* time);

// 原 [0x53B8C6,0x53BA26)：两端区间保留 GMT 共享对象上的原字段修正次序。
LegacyTm53B8C6* __cdecl LocalTime_53B8C6(std::int32_t* timeValue)
{
    if (*timeValue < 0) {
        return nullptr;
    }
    TzSet_540B08();
    const std::int32_t time = *timeValue;

    if (time > 259200 && time < 2147224447) {
        std::int32_t adjusted = time - gTimezone_58FB78;
        LegacyTm53B8C6* result = GmTime_541067(&adjusted);
        if (gDaylight_58FB7C != 0 && IsDst_540D7B(result) != 0) {
            adjusted -= gDstBias_58FB80;
            result = GmTime_541067(&adjusted);
            result->dst = 1; // 0x53B933，仅本分支写 1。
        }
        return result;
    }

    LegacyTm53B8C6* result = GmTime_541067(timeValue); // 0x53B940，传原指针。
    const std::int32_t isDst = IsDst_540D7B(result);  // 0x53B948，无条件调用。
    std::int32_t adjusted = result->seconds;
    if (isDst != 0) {
        adjusted -= gDstBias_58FB80;
    }
    adjusted -= gTimezone_58FB78;

    result->seconds = adjusted % 60;
    if (result->seconds < 0) {
        result->seconds += 60;
        adjusted -= 60;
    }
    adjusted = result->minutes + adjusted / 60;
    result->minutes = adjusted % 60;
    if (result->minutes < 0) {
        result->minutes += 60;
        adjusted -= 60;
    }
    adjusted = result->hours + adjusted / 60;
    result->hours = adjusted % 24;
    if (result->hours < 0) {
        result->hours += 24;
        adjusted -= 24;
    }

    const std::int32_t dayOffset = adjusted / 24;
    if (dayOffset > 0) {
        result->weekDay = (result->weekDay + dayOffset) % 7;
        result->day += dayOffset;
        result->yearDay0based += dayOffset;
    } else if (dayOffset < 0) {
        result->weekDay = (result->weekDay + dayOffset + 7) % 7;
        result->day += dayOffset;
        if (result->day > 0) {
            result->yearDay0based += dayOffset;
        } else {
            --result->yearMinus1900;
            result->day += 31;
            result->yearDay0based = 364;
            result->month0based = 11; // 0x53BA10..0x53BA1D，不作通用跨月修正。
        }
    }
    return result;
}

// 原 __loctotime_t@0x541160 的七个 int32 参数；时区转换依赖尚未实现，不回退本机 CRT。
std::int32_t ConvertLocalTime_541160(
    std::int32_t year, std::int32_t month, std::int32_t day,
    std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t daylight);

std::int32_t ReadLegacyTime32_53BA26(std::int32_t* output)
{
    SYSTEMTIME localTime;
    SYSTEMTIME utcTime;
    TIME_ZONE_INFORMATION timeZone;
    GetLocalTime(&localTime);
    GetSystemTime(&utcTime);

    std::int32_t daylight;
    if (utcTime.wMinute == gUtcTimeCache_1085FD0.wMinute
        && utcTime.wHour == gUtcTimeCache_1085FD0.wHour
        && utcTime.wDay == gUtcTimeCache_1085FD0.wDay
        && utcTime.wMonth == gUtcTimeCache_1085FD0.wMonth
        && utcTime.wYear == gUtcTimeCache_1085FD0.wYear) {
        daylight = gDaylightCache_1085FC8;
    } else {
        const DWORD zone = GetTimeZoneInformation(&timeZone);
        if (zone == TIME_ZONE_ID_INVALID) {
            daylight = -1;
        } else {
            daylight = zone == TIME_ZONE_ID_DAYLIGHT
                && timeZone.DaylightDate.wMonth != 0 && timeZone.DaylightBias != 0 ? 1 : 0;
        }
        std::memcpy(&gUtcTimeCache_1085FD0, &utcTime, sizeof(utcTime));
        gDaylightCache_1085FC8 = daylight;
    }

    const std::int32_t result = ConvertLocalTime_541160(
        localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour,
        localTime.wMinute, localTime.wSecond, daylight);
    if (output != nullptr) {
        *output = result;
    }
    return result;
}

} // namespace cg::platform
