#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyMemory.hpp"
#include "StartupNative.hpp"
#include "LegacyLowIo.hpp"
#include "LegacyStdio.hpp"

#include <bit>
#include <cerrno>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <direct.h>

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

// 过渡期旧描述符错误映射；FILE/lowio成组迁移前保真实表消费者。
namespace {

struct LegacyFileErrorEntry {
    std::uint32_t nativeCode;
    std::int32_t error;
};

// 原 [591E80,591FE8)；重复键按首次匹配，不排序或合并。
constexpr LegacyFileErrorEntry kLegacyFileErrors591E80[] {
    {1, 22}, {2, 2}, {3, 2}, {4, 24}, {5, 13},
    {6, 9}, {7, 12}, {8, 12}, {9, 12}, {10, 7},
    {11, 8}, {12, 22}, {13, 22}, {15, 2}, {16, 13},
    {17, 18}, {18, 2}, {33, 13}, {53, 2}, {65, 13},
    {67, 2}, {80, 17}, {82, 13}, {83, 13}, {87, 22},
    {89, 11}, {108, 13}, {109, 32}, {112, 28}, {114, 9},
    {6, 22}, {128, 10}, {129, 10}, {130, 9}, {131, 22},
    {132, 13}, {145, 41}, {158, 13}, {161, 2}, {164, 11},
    {167, 13}, {183, 17}, {206, 2}, {215, 11}, {1816, 12}
};
static_assert(sizeof(kLegacyFileErrors591E80) == 0x168);

} // namespace

std::uintptr_t MapLegacyFileError_542F44(const std::uint32_t code)
{
    LegacyDosError_1085FE4() = code;
    for (const LegacyFileErrorEntry& entry : kLegacyFileErrors591E80) {
        if (entry.nativeCode == code) {
            LegacyErrno_1085FE0() = entry.error;
            return static_cast<std::uintptr_t>(entry.error);
        }
    }
    if (code >= 19 && code <= 36) {
        LegacyErrno_1085FE0() = 13;
    } else if (code >= 188) {
        LegacyErrno_1085FE0() = 8;
        if (code > 202) {
            LegacyErrno_1085FE0() = 22;
        }
    } else {
        LegacyErrno_1085FE0() = 22;
    }
    return reinterpret_cast<std::uintptr_t>(kLegacyFileErrors591E80 + 45);
}

std::int32_t ReadLegacyDescriptorStatus32_53CCD0(
    const std::int32_t descriptor, LegacyFileStatus32* const status)
{
    const auto descriptorIndex = static_cast<std::uint32_t>(descriptor);
    if (descriptorIndex >= gLegacyDescriptorCapacity1086AC0) {
        LegacyErrno_1085FE0() = 9;
        return -1;
    }
    const std::uint32_t slot = descriptorIndex & 31u;
    // 原var_8保存表块槽地址；各API前重新从同一块槽取得HANDLE。
    LegacyDescriptorRecord*& block = gLegacyDescriptorBlocks10869C0[descriptorIndex >> 5];
    if ((block[slot].flags & 1u) == 0) {
        LegacyErrno_1085FE0() = 9;
        return -1;
    }

    const DWORD fileType = ::GetFileType(block[slot].handle) & ~0x8000u;
    if (fileType == FILE_TYPE_CHAR || fileType == FILE_TYPE_PIPE) {
        status->mode = fileType == FILE_TYPE_CHAR ? 0x2000u : 0x1000u;
        status->deviceBits = descriptorIndex;
        status->relatedDeviceBits = descriptorIndex;
        status->linkCount = 1;
        status->inode = 0;
        status->group = 0;
        status->user = 0;
        status->creationTime = 0;
        status->modifiedTime = 0;
        status->accessTime = 0;
        if (fileType == FILE_TYPE_CHAR) {
            status->sizeLowBits = 0;
        } else {
            DWORD available;
            const BOOL peeked = ::PeekNamedPipe(
                block[slot].handle, nullptr, 0, nullptr, &available, nullptr);
            status->sizeLowBits = peeked != FALSE ? available : 0;
        }
        return 0;
    }
    if (fileType == FILE_TYPE_UNKNOWN) {
        LegacyErrno_1085FE0() = 9;
        return -1;
    }

    BY_HANDLE_FILE_INFORMATION information;
    if (fileType == FILE_TYPE_DISK) {
        status->mode = 0;
        status->group = 0;
        status->user = 0;
        status->inode = 0;
        status->linkCount = 1;
    }
    if (fileType != FILE_TYPE_DISK
        || ::GetFileInformationByHandle(block[slot].handle, &information) == FALSE) {
        static_cast<void>(MapLegacyFileError_542F44(::GetLastError()));
        return -1;
    }
    status->mode |= (information.dwFileAttributes & 1u) != 0 ? 0x124u : 0x1B6u;

    const auto convert = [](const FILETIME& fileTime, std::int32_t& result) {
        FILETIME localTime;
        SYSTEMTIME fields;
        if (::FileTimeToLocalFileTime(&fileTime, &localTime) == FALSE
            || ::FileTimeToSystemTime(&localTime, &fields) == FALSE) {
            return false;
        }
        result = ConvertLocalTime_541160(
            fields.wYear, fields.wMonth, fields.wDay, fields.wHour,
            fields.wMinute, fields.wSecond, -1);
        return true;
    };
    std::int32_t converted;
    if (!convert(information.ftLastWriteTime, converted)) {
        return -1;
    }
    status->modifiedTime = converted;
    if (information.ftLastAccessTime.dwLowDateTime != 0
        || information.ftLastAccessTime.dwHighDateTime != 0) {
        if (!convert(information.ftLastAccessTime, converted)) {
            return -1;
        }
    }
    status->accessTime = converted;
    if (information.ftCreationTime.dwLowDateTime == 0
        && information.ftCreationTime.dwHighDateTime == 0) {
        converted = status->modifiedTime;
    } else if (!convert(information.ftCreationTime, converted)) {
        return -1;
    }
    status->mode |= 0x8000u;
    status->creationTime = converted;
    const std::uint32_t sizeLow = information.nFileSizeLow;
    status->deviceBits = 0;
    status->sizeLowBits = sizeLow;
    status->relatedDeviceBits = 0;
    return 0;
}

std::int32_t ReadLegacyFileCreationTime32(
    LegacyFile* const file, std::int32_t* const output)
{
    LegacyFileStatus32 status;
    const std::int32_t result =
        ReadLegacyDescriptorStatus32_53CCD0(file->descriptor, &status);
    if (result >= 0) {
        *output = status.creationTime;
    }
    return result;
}

} // namespace cg::platform
