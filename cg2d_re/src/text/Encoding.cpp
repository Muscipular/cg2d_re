#include "cg/text/Encoding.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <system_error>

namespace cg::text {
namespace {

constexpr UINT kGbkCodePage = 936;

int Win32Length(const std::size_t length)
{
    if (length > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
        throw std::length_error("文字长度超过Win32编码接口范围");
    }
    return static_cast<int>(length);
}

std::wstring Decode(const std::string_view bytes, const UINT codePage, const DWORD flags = MB_ERR_INVALID_CHARS)
{
    if (bytes.empty()) {
        return {};
    }
    const int length = Win32Length(bytes.size());
    const int count = ::MultiByteToWideChar(codePage, flags, bytes.data(), length, nullptr, 0);
    if (count == 0) {
        throw std::system_error(static_cast<int>(::GetLastError()), std::system_category(), "文字解码失败");
    }
    std::wstring result(static_cast<std::size_t>(count), L'\0');
    if (::MultiByteToWideChar(codePage, flags, bytes.data(), length, result.data(), count) == 0) {
        throw std::system_error(static_cast<int>(::GetLastError()), std::system_category(), "文字解码失败");
    }
    return result;
}

std::string Encode(const std::wstring_view text, const UINT codePage)
{
    if (text.empty()) {
        return {};
    }
    const int length = Win32Length(text.size());
    const DWORD flags = codePage == CP_UTF8 ? WC_ERR_INVALID_CHARS : WC_NO_BEST_FIT_CHARS;
    BOOL replaced = FALSE;
    BOOL* const replacement = codePage == CP_UTF8 ? nullptr : &replaced;
    const int count = ::WideCharToMultiByte(codePage, flags, text.data(), length, nullptr, 0, nullptr, replacement);
    if (count == 0 || replaced) {
        const DWORD error = replaced ? ERROR_NO_UNICODE_TRANSLATION : ::GetLastError();
        throw std::system_error(static_cast<int>(error), std::system_category(), "文字无法按目标编码表示");
    }
    std::string result(static_cast<std::size_t>(count), '\0');
    if (::WideCharToMultiByte(codePage, flags, text.data(), length, result.data(), count, nullptr, replacement) == 0 || replaced) {
        const DWORD error = replaced ? ERROR_NO_UNICODE_TRANSLATION : ::GetLastError();
        throw std::system_error(static_cast<int>(error), std::system_category(), "文字无法按目标编码表示");
    }
    return result;
}

} // namespace

std::wstring ToUtf16(const std::string_view utf8) { return Decode(utf8, CP_UTF8); }
std::string FromUtf16(const std::wstring_view utf16) { return Encode(utf16, CP_UTF8); }
std::string FromLegacyDisplay(const std::string_view bytes) { return FromUtf16(Decode(bytes, CP_ACP, 0)); }
std::string FromGbk(const std::string_view gbk) { return FromUtf16(Decode(gbk, kGbkCodePage, 0)); }
std::string ToGbk(const std::string_view utf8) {
    try { return Encode(ToUtf16(utf8), kGbkCodePage); }
    catch (const std::system_error& error) {
        throw GbkStorageError(error.code(), "文字无法编码为GBK");
    }
}

std::wstring FromGbkWide(const std::string_view gbk) { return Decode(gbk, kGbkCodePage, 0); }
std::wstring FromLegacyDisplayWide(const std::string_view bytes) { return Decode(bytes, CP_ACP, 0); }
std::string ToGbk(const std::wstring_view utf16)
{
    try { return Encode(utf16, kGbkCodePage); }
    catch (const std::system_error& error) {
        throw GbkStorageError(error.code(), "文字无法编码为GBK");
    }
}

void StoreGbk(char* const destination, const std::size_t capacity, const std::wstring_view utf16)
{
    const std::string stored = ToGbk(utf16);
    if (stored.size() >= capacity) {
        throw GbkStorageError(ERROR_INSUFFICIENT_BUFFER, std::system_category(), "文字超过GBK字段容量");
    }
    std::char_traits<char>::move(destination, stored.c_str(), stored.size() + 1);
}

std::size_t Next(const std::wstring_view utf16, const std::size_t offset) noexcept
{
    if (offset >= utf16.size()) { return utf16.size(); }
    const wchar_t unit = utf16[offset];
    const bool pair = unit >= 0xD800 && unit <= 0xDBFF && offset + 1 < utf16.size()
        && utf16[offset + 1] >= 0xDC00 && utf16[offset + 1] <= 0xDFFF;
    return offset + (pair ? 2 : 1);
}

std::size_t Previous(const std::wstring_view utf16, const std::size_t offset) noexcept
{
    std::size_t position = (std::min)(offset, utf16.size());
    if (position == 0) { return 0; }
    --position;
    if (position > 0 && utf16[position] >= 0xDC00 && utf16[position] <= 0xDFFF
        && utf16[position - 1] >= 0xD800 && utf16[position - 1] <= 0xDBFF) { --position; }
    return position;
}

std::size_t Prefix(const std::wstring_view utf16, const std::size_t unitBudget) noexcept
{
    std::size_t position = (std::min)(unitBudget, utf16.size());
    if (position > 0 && position < utf16.size() && utf16[position - 1] >= 0xD800
        && utf16[position - 1] <= 0xDBFF && utf16[position] >= 0xDC00 && utf16[position] <= 0xDFFF) { --position; }
    return position;
}

std::size_t Count(const std::wstring_view utf16) noexcept
{
    std::size_t count = 0;
    for (std::size_t offset = 0; offset < utf16.size(); offset = Next(utf16, offset)) { ++count; }
    return count;
}

void StoreGbk(char* const destination, const std::size_t capacity, const std::string_view utf8)
{
    const std::string stored = ToGbk(utf8);
    if (stored.size() >= capacity) {
        throw GbkStorageError(ERROR_INSUFFICIENT_BUFFER, std::system_category(), "文字超过GBK字段容量");
    }
    std::char_traits<char>::move(destination, stored.c_str(), stored.size() + 1);
}


std::size_t Next(const std::string_view utf8, const std::size_t offset) noexcept
{
    if (offset >= utf8.size()) {
        return utf8.size();
    }
    const auto lead = static_cast<unsigned char>(utf8[offset]);
    const std::size_t width = lead < 0x80 ? 1 : lead < 0xE0 ? 2 : lead < 0xF0 ? 3 : 4;
    return (std::min)(offset + width, utf8.size());
}

std::size_t Previous(const std::string_view utf8, const std::size_t offset) noexcept
{
    std::size_t position = (std::min)(offset, utf8.size());
    if (position == 0) {
        return 0;
    }
    do {
        --position;
    } while (position != 0 && (static_cast<unsigned char>(utf8[position]) & 0xC0) == 0x80);
    return position;
}

std::size_t Prefix(const std::string_view utf8, const std::size_t byteBudget) noexcept
{
    std::size_t position = (std::min)(byteBudget, utf8.size());
    if (position < utf8.size()) {
        while (position != 0 && (static_cast<unsigned char>(utf8[position]) & 0xC0) == 0x80) {
            --position;
        }
    }
    return position;
}

std::size_t Count(const std::string_view utf8) noexcept
{
    std::size_t count = 0;
    for (std::size_t offset = 0; offset < utf8.size(); offset = Next(utf8, offset)) {
        ++count;
    }
    return count;
}

std::size_t Copy(char* const destination, const std::size_t capacity, const std::string_view utf8) noexcept
{
    if (capacity == 0) {
        return 0;
    }
    const std::size_t length = Prefix(utf8, capacity - 1);
    if (length != 0) {
        std::char_traits<char>::move(destination, utf8.data(), length);
    }
    destination[length] = '\0';
    return length;
}

} // namespace cg::text
