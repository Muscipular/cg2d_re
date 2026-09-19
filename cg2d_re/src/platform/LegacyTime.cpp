#include "cg/platform/LegacyTime.hpp"
#include "StartupNative.hpp"

#include <cstring>

namespace cg::platform {

// 原缓存分别位于 0x1085FD0、0x1085FC8；这里只还原各已访问对象，不推断中间填充用途。
SYSTEMTIME gUtcTimeCache_1085FD0 {};
std::int32_t gDaylightCache_1085FC8 = 0;

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
