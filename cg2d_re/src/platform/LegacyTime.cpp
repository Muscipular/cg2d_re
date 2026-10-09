#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "StartupNative.hpp"
#include "LegacyStdio.hpp"

#include <bit>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <direct.h>
#include <sys/stat.h>

namespace cg::platform {

// MC01：每次访问本机CRT的当前线程状态，不缓存TLS地址或维护影子错误槽。
std::int32_t& LegacyErrno_1085FE0() noexcept
{
    return errno;
}

unsigned long& LegacyDosError_1085FE4() noexcept
{
    return _doserrno;
}

static_assert(sizeof(long) == sizeof(std::int32_t));
static_assert(sizeof(__time32_t) == sizeof(std::int32_t));
static_assert(sizeof(LegacyTm53B8C6) == 0x24);

// MC01：原54518B入口由MSVC承接，保留调用方参数与定宽返回。
char* __cdecl GetEnv_54518B(const char* name)
{
    return std::getenv(name);
}

// MC01：原53C5BE入口由MSVC承接，保留调用方参数与定宽返回。
std::int32_t __cdecl Atol_53C5BE(const char* text)
{
    return static_cast<std::int32_t>(std::atol(text));
}

// MC01：原53DA45入口由MSVC承接，保留调用方参数与定宽返回。
std::int32_t Strtol_53DA45(
    const char* const text, char** const endPointer, const std::int32_t radix)
{
    return static_cast<std::int32_t>(std::strtol(text, endPointer, radix));
}

// MC01：原53C649入口由MSVC承接，保留调用方参数与定宽返回。
std::int32_t Atoi_53C649(const char* text)
{
    return std::atoi(text);
}

// MC01：原53C3AC入口由MSVC承接，保留调用方参数与定宽返回。
void* __cdecl Malloc_53C3AC(std::uint32_t size)
{
    return std::malloc(size);
}

// MC01：原53C0A9入口由MSVC承接，保留调用方参数与定宽返回。
void __cdecl Free_53C0A9(void* memory)
{
    std::free(memory);
}

// MC01：原54ABDD入口由MSVC承接，保留调用方参数与定宽返回。
char* Itoa_54ABDD(const std::int32_t value, char* const buffer, const std::int32_t radix)
{
    return ::_itoa(value, buffer, radix);
}

// MC01：原53D26E入口由MSVC承接，保留调用方参数与定宽返回。
char* StrTok_53D26E(char* text, const char* delimiters)
{
    return std::strtok(text, delimiters);
}

// MC01：原53BFC0入口由MSVC承接，保留调用方参数与定宽返回。
std::int32_t __cdecl StrNCmp_53BFC0(
    const char* const left, const char* const right, const std::uint32_t count)
{
    return std::strncmp(left, right, count);
}

// MC01：原5450F0入口由MSVC承接，保留调用方参数与定宽返回。
std::int32_t __cdecl StrCmp_5450F0(const char* left, const char* right)
{
    return std::strcmp(left, right);
}

// MC01：原53F9E0入口由MSVC承接，保留调用方参数与定宽返回。
std::uint32_t __cdecl StrLen_53F9E0(const char* text)
{
    return static_cast<std::uint32_t>(std::strlen(text));
}

// MC01：原544DB0入口由MSVC承接，保留调用方参数与定宽返回。
char* __cdecl StrCpy_544DB0(char* target, const char* source)
{
    return std::strcpy(target, source);
}

// MC01：原544DC0入口由MSVC承接，保留调用方参数与定宽返回。
char* __cdecl StrCat_544DC0(char* target, const char* source)
{
    return std::strcat(target, source);
}

// MC01：原53C790入口由MSVC承接，保留调用方参数与定宽返回。
char* __cdecl StrNCat_53C790(char* target, const char* source, std::uint32_t count)
{
    return std::strncat(target, source, count);
}

// MC01：原53BEC0入口由MSVC承接，保留调用方参数与定宽返回。
char* __cdecl StrNCpy_53BEC0(char* target, const char* source, std::uint32_t count)
{
    return std::strncpy(target, source, count);
}

// MC01：原53C2F0入口由MSVC承接，保留调用方参数与定宽返回。
char* __cdecl StrChr_53C2F0(const char* text, const std::int32_t target)
{
    return const_cast<char*>(std::strchr(text, target));
}

// MC01：原53BE40入口由MSVC承接，保留调用方参数与定宽返回。
char* __cdecl StrStr_53BE40(const char* text, const char* substring)
{
    return const_cast<char*>(std::strstr(text, substring));
}

// MC01：原53C654入口由MSVC承接，保留调用方参数与定宽返回。
std::int32_t CreateLegacyDirectory_53C654(const char* const path)
{
    return ::_mkdir(path);
}

// MC01：原53DD2F入口由MSVC承接，保留调用方参数与定宽返回。
std::int32_t ChangeLegacyDirectory_53DD2F(const char* const path)
{
    return ::_chdir(path);
}

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
