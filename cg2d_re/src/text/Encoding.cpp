#include "cg/text/Encoding.hpp"

#include <Windows.h>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <system_error>

namespace cg::text {

static constexpr UINT kGbkCodePage = 936;

static int Win32Length(const std::size_t length)
{
    if (length > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
        throw std::length_error("文字长度超过Win32编码接口范围");
    }
    return static_cast<int>(length);
}

static std::wstring Decode(const std::string_view bytes, const UINT codePage, const DWORD flags = MB_ERR_INVALID_CHARS)
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

static std::string Encode(const std::wstring_view text, const UINT codePage)
{
    if (text.empty()) {
        return {};
    }
    const int length = Win32Length(text.size());
    const DWORD flags = WC_NO_BEST_FIT_CHARS;
    BOOL replaced = FALSE;
    BOOL* const replacement = &replaced;
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

std::wstring FromUtf8Wide(const std::string_view utf8) { return Decode(utf8, CP_UTF8); }
std::wstring FromGbkWide(const std::string_view gbk) { return Decode(gbk, kGbkCodePage, 0); }
std::wstring FromLegacyDisplayWide(const std::string_view bytes) { return Decode(bytes, CP_ACP, 0); }
std::string ToGbk(const std::wstring_view utf16)
{
    try { return Encode(utf16, kGbkCodePage); }
    catch (const std::system_error& error) {
#ifdef MY_DEBUG
        throw GbkStorageError(error.code(), "文字无法编码为GBK");
#else
        return "";
#endif
    }
}

void StoreGbk(char* const destination, const std::size_t capacity, const std::wstring_view utf16)
{
    const std::string stored = ToGbk(utf16);
    if (stored.size() >= capacity) {
#ifdef MY_DEBUG
        throw GbkStorageError(ERROR_INSUFFICIENT_BUFFER, std::system_category(), "文字超过GBK字段容量");
#else
        return;
#endif
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

std::int32_t InlineColorIndex(const wchar_t code) noexcept
{
    if (code >= L'0' && code <= L'9') { return code - L'0'; }
    if (code >= L'a' && code <= L'z') { return code - L'a' + 10; }
    if (code >= L'A' && code <= L'Z') { return code - L'A' + 36; }
    return -1;
}

} // namespace cg::text
