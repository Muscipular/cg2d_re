#include "cg/platform/LegacyTime.hpp"

#include <ctime>
#include <sys/stat.h>

namespace cg::platform {

static_assert(sizeof(long) == sizeof(std::int32_t));
static_assert(sizeof(__time32_t) == sizeof(std::int32_t));
static_assert(sizeof(LegacyTm53B8C6) == 0x24);

// MC01：原53BA26入口由MSVC承接，保留调用方参数与定宽返回。
std::int32_t ReadLegacyTime32_53BA26(std::int32_t* output)
{
    const __time32_t result = ::_time32(nullptr);
    if (output != nullptr) {
        *output = static_cast<std::int32_t>(result);
    }
    return static_cast<std::int32_t>(result);
}

// MC01：原541160入口由MSVC承接，保留调用方参数与定宽返回。
std::int32_t __cdecl ConvertLocalTime_541160(
    std::int32_t year, std::int32_t month, std::int32_t day,
    std::int32_t hour, std::int32_t minute, std::int32_t second, std::int32_t daylight)
{
    std::tm calendar{};
    calendar.tm_year = year - 1900;
    calendar.tm_mon = month - 1;
    calendar.tm_mday = day;
    calendar.tm_hour = hour;
    calendar.tm_min = minute;
    calendar.tm_sec = second;
    calendar.tm_isdst = daylight;
    return static_cast<std::int32_t>(::_mktime32(&calendar));
}

// MC01：原53B8C6入口由MSVC承接，保留调用方参数与定宽返回。
LegacyTm53B8C6* __cdecl LocalTime_53B8C6(std::int32_t* timeValue)
{
    const __time32_t timestamp = *timeValue;
    const std::tm* const calendar = ::_localtime32(&timestamp);
    if (calendar == nullptr) {
        return nullptr;
    }
    // 业务仍借用九S32字段；对象与MSVC日历同属当前线程，不强转布局。
    thread_local LegacyTm53B8C6 result;
    result.seconds = calendar->tm_sec;
    result.minutes = calendar->tm_min;
    result.hours = calendar->tm_hour;
    result.day = calendar->tm_mday;
    result.month0based = calendar->tm_mon;
    result.yearMinus1900 = calendar->tm_year;
    result.weekDay = calendar->tm_wday;
    result.yearDay0based = calendar->tm_yday;
    result.dst = calendar->tm_isdst;
    return &result;
}

std::int32_t ReadLegacyFileCreationTime32(
    LegacyFile* const file, std::int32_t* const output)
{
    struct _stat32 status;
    const std::int32_t result = ::_fstat32(::_fileno(file), &status);
    if (result == 0) {
        *output = static_cast<std::int32_t>(status.st_ctime);
    }
    return result;
}

} // namespace cg::platform
