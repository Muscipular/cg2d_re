#pragma once

#include <string>
#include <string_view>

namespace cg::text {

enum class LegacyEncoding {
    Gbk,
    Big5,
};

struct EncodingResult {
    std::string text;
    bool ok = false;
};

[[nodiscard]] EncodingResult Utf8ToLegacy(std::string_view utf8, LegacyEncoding encoding);
[[nodiscard]] EncodingResult LegacyToUtf8(std::string_view bytes, LegacyEncoding encoding);

} // namespace cg::text
