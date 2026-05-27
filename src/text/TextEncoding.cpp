#include "text/TextEncoding.hpp"

#include <limits>
#include <optional>
#include <string>
#include <utility>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

namespace cg::text {
namespace {

#ifdef _WIN32

[[nodiscard]] UINT ToCodePage(LegacyEncoding encoding) noexcept
{
    switch (encoding) {
    case LegacyEncoding::Gbk:
        return 936;
    case LegacyEncoding::Big5:
        return 950;
    default:
        return 0;
    }
}

[[nodiscard]] bool FitsWindowsLength(std::size_t size) noexcept
{
    return size <= static_cast<std::size_t>((std::numeric_limits<int>::max)());
}

[[nodiscard]] std::optional<std::wstring> TryMultiByteToWide(
    std::string_view text,
    UINT codePage,
    DWORD flags)
{
    if (text.empty()) {
        return std::wstring{};
    }

    if (!FitsWindowsLength(text.size())) {
        return std::nullopt;
    }

    const int sourceLength = static_cast<int>(text.size());
    const int wideLength = MultiByteToWideChar(codePage, flags, text.data(), sourceLength, nullptr, 0);
    if (wideLength <= 0) {
        return std::nullopt;
    }

    std::wstring wide(static_cast<std::size_t>(wideLength), L'\0');
    const int written = MultiByteToWideChar(codePage, flags, text.data(), sourceLength, wide.data(), wideLength);
    if (written != wideLength) {
        return std::nullopt;
    }

    return wide;
}

[[nodiscard]] std::optional<std::wstring> MultiByteToWideStrict(std::string_view text, UINT codePage)
{
    auto wide = TryMultiByteToWide(text, codePage, MB_ERR_INVALID_CHARS);
    if (wide.has_value()) {
        return wide;
    }

    if (codePage != CP_UTF8 && GetLastError() == ERROR_INVALID_FLAGS) {
        return TryMultiByteToWide(text, codePage, 0);
    }

    return std::nullopt;
}

[[nodiscard]] EncodingResult WideToUtf8(std::wstring_view wide)
{
    if (wide.empty()) {
        return {{}, true};
    }

    if (!FitsWindowsLength(wide.size())) {
        return {};
    }

    const int sourceLength = static_cast<int>(wide.size());
    const int textLength =
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide.data(), sourceLength, nullptr, 0, nullptr, nullptr);
    if (textLength <= 0) {
        return {};
    }

    std::string text(static_cast<std::size_t>(textLength), '\0');
    const int written =
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wide.data(), sourceLength, text.data(), textLength, nullptr, nullptr);
    if (written != textLength) {
        return {};
    }

    return {std::move(text), true};
}

[[nodiscard]] EncodingResult WideToLegacy(std::wstring_view wide, UINT codePage)
{
    if (wide.empty()) {
        return {{}, true};
    }

    if (!FitsWindowsLength(wide.size())) {
        return {};
    }

    const int sourceLength = static_cast<int>(wide.size());
    const DWORD flags = WC_NO_BEST_FIT_CHARS;
    BOOL usedDefaultChar = FALSE;
    int textLength =
        WideCharToMultiByte(codePage, flags, wide.data(), sourceLength, nullptr, 0, nullptr, &usedDefaultChar);
    DWORD lastError = GetLastError();
    DWORD activeFlags = flags;
    if (textLength <= 0 && lastError == ERROR_INVALID_FLAGS) {
        usedDefaultChar = FALSE;
        activeFlags = 0;
        textLength =
            WideCharToMultiByte(codePage, activeFlags, wide.data(), sourceLength, nullptr, 0, nullptr, &usedDefaultChar);
    }

    if (textLength <= 0 || usedDefaultChar != FALSE) {
        return {};
    }

    std::string text(static_cast<std::size_t>(textLength), '\0');
    usedDefaultChar = FALSE;
    const int written =
        WideCharToMultiByte(codePage, activeFlags, wide.data(), sourceLength, text.data(), textLength, nullptr, &usedDefaultChar);
    if (written != textLength || usedDefaultChar != FALSE) {
        return {};
    }

    return {std::move(text), true};
}

#endif

} // namespace

EncodingResult Utf8ToLegacy(std::string_view utf8, LegacyEncoding encoding)
{
#ifdef _WIN32
    const UINT codePage = ToCodePage(encoding);
    if (codePage == 0) {
        return {};
    }

    const auto wide = MultiByteToWideStrict(utf8, CP_UTF8);
    if (!wide.has_value()) {
        return {};
    }

    return WideToLegacy(*wide, codePage);
#else
    (void)utf8;
    (void)encoding;
    return {};
#endif
}

EncodingResult LegacyToUtf8(std::string_view bytes, LegacyEncoding encoding)
{
#ifdef _WIN32
    const UINT codePage = ToCodePage(encoding);
    if (codePage == 0) {
        return {};
    }

    const auto wide = MultiByteToWideStrict(bytes, codePage);
    if (!wide.has_value()) {
        return {};
    }

    return WideToUtf8(*wide);
#else
    (void)bytes;
    (void)encoding;
    return {};
#endif
}

} // namespace cg::text
