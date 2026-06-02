#ifndef CG_ENGINE_UTILS_H
#define CG_ENGINE_UTILS_H
#include "PacketTypes.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cg::packet
{
    namespace detail
    {
        inline constexpr std::string_view Int62Digits =
            "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        inline constexpr int Int62Radix = 62;
        inline constexpr std::string_view Encode64Digits =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

        [[nodiscard]] inline int DecodeInt62Digit(char ch) noexcept
        {
            if ('0' <= ch && ch <= '9') {
                return ch - '0';
            }
            if ('a' <= ch && ch <= 'z') {
                return ch - 'a' + 10;
            }
            if ('A' <= ch && ch <= 'Z') {
                return ch - 'A' + 36;
            }
            return -1;
        }

        [[nodiscard]] inline int Decode64Digit(char ch) noexcept
        {
            if ('A' <= ch && ch <= 'Z') {
                return ch - 'A';
            }
            if ('a' <= ch && ch <= 'z') {
                return ch - 'a' + 26;
            }
            if ('0' <= ch && ch <= '9') {
                return ch - '0' + 52;
            }
            if (ch == '+') {
                return 62;
            }
            if (ch == '-') {
                return 63;
            }
            return -1;
        }

        [[nodiscard]] inline PacketError MakeInt62Error(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            return error;
        }

        [[nodiscard]] inline bool IsLegacyDoubleByteLead(char ch) noexcept
        {
            return static_cast<unsigned char>(ch) >= 0x80U;
        }

        [[nodiscard]] inline PacketError MakeEscapedStringError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            return error;
        }

        [[nodiscard]] inline PacketError MakeEncode64Error(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            return error;
        }

        [[nodiscard]] inline std::uint8_t ByteAt(std::string_view bytes, std::size_t index) noexcept
        {
            return static_cast<std::uint8_t>(static_cast<unsigned char>(bytes[index]));
        }

    }

    [[nodiscard]] inline PacketResult<std::string> EncodeInt62(int value)
    {
        const bool negative = value < 0;
        std::int64_t magnitude = static_cast<std::int64_t>(value);
        if (negative) {
            magnitude = 0 - magnitude;
        }

        std::string encodedDigits;
        do {
            const std::size_t digit = static_cast<std::size_t>(magnitude % detail::Int62Radix);
            encodedDigits.push_back(detail::Int62Digits[digit]);
            magnitude /= detail::Int62Radix;
        } while (magnitude > 0);

        std::string output;
        output.reserve(encodedDigits.size() + (negative ? 1U : 0U));
        if (negative) {
            output.push_back('-');
        }
        for (auto it = encodedDigits.rbegin(); it != encodedDigits.rend(); ++it) {
            output.push_back(*it);
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

    [[nodiscard]] inline PacketResult<int> DecodeInt62(std::string_view text)
    {
        bool negative = false;
        std::size_t index = 0;
        if (!text.empty() && text.front() == '-') {
            negative = true;
            index = 1;
        }

        if (index == text.size()) {
            return PacketResult<int>::Success(0);
        }

        const std::int64_t maxValue = static_cast<std::int64_t>(std::numeric_limits<int>::max());
        const std::int64_t minValue = static_cast<std::int64_t>(std::numeric_limits<int>::min());
        const std::int64_t limit = negative ? (0 - minValue) : maxValue;
        std::int64_t value = 0;

        for (; index < text.size(); ++index) {
            const int digit = detail::DecodeInt62Digit(text[index]);
            if (digit < 0) {
                return PacketResult<int>::Failure(detail::MakeInt62Error(
                    PacketErrorCode::DecodeFailed,
                    "i62 数值包含非法字符"));
            }

            if (value > (limit - digit) / detail::Int62Radix) {
                return PacketResult<int>::Failure(detail::MakeInt62Error(
                    PacketErrorCode::DecodeFailed,
                    "i62 数值超出 int 范围"));
            }

            value = (value * detail::Int62Radix) + digit;
        }

        if (negative) {
            value = -value;
        }

        return PacketResult<int>::Success(static_cast<int>(value));
    }

    [[nodiscard]] inline PacketResult<std::string> EncodeEscapedString(std::string_view text)
    {
        std::string output;
        output.reserve(text.size());

        for (std::size_t index = 0; index < text.size(); ++index) {
            const char ch = text[index];
            if (detail::IsLegacyDoubleByteLead(ch)) {
                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(detail::MakeEscapedStringError(
                        PacketErrorCode::EncodeFailed,
                        "es 字符串旧双字节字符被截断"));
                }

                output.push_back(ch);
                output.push_back(text[index + 1U]);
                ++index;
                continue;
            }

            if (ch == '\\') {
                output.push_back('\\');
                output.push_back('\\');
            } else if (ch == ' ') {
                output.push_back('\\');
                output.push_back('S');
            } else if (ch == '\n') {
                output.push_back('\\');
                output.push_back('n');
            } else if (ch == '\r') {
                output.push_back('\\');
                output.push_back('r');
            } else {
                output.push_back(ch);
            }
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

    [[nodiscard]] inline PacketResult<std::string> DecodeEscapedString(std::string_view text)
    {
        std::string output;
        output.reserve(text.size());

        for (std::size_t index = 0; index < text.size(); ++index) {
            const char ch = text[index];
            if (detail::IsLegacyDoubleByteLead(ch)) {
                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(detail::MakeEscapedStringError(
                        PacketErrorCode::DecodeFailed,
                        "es 字符串旧双字节字符被截断"));
                }

                output.push_back(ch);
                output.push_back(text[index + 1U]);
                ++index;
                continue;
            }

            if (ch != '\\') {
                output.push_back(ch);
                continue;
            }

            if (index + 1U >= text.size()) {
                output.push_back('\\');
                continue;
            }

            const char escaped = text[index + 1U];
            if (escaped == 'S') {
                output.push_back(' ');
            } else if (escaped == 'n') {
                output.push_back('\n');
            } else if (escaped == 'r') {
                output.push_back('\r');
            } else if (escaped == '\\') {
                output.push_back('\\');
            } else {
                output.push_back('\\');
            }
            ++index;
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

    [[nodiscard]] inline PacketResult<std::vector<std::string>> SplitPlainTokens(std::string_view text)
    {
        std::vector<std::string> tokens;
        if (text.empty() || text.front() == '\0') {
            return PacketResult<std::vector<std::string>>::Success(std::move(tokens));
        }

        std::vector<std::size_t> tokenStarts;
        tokenStarts.push_back(0);

        const auto appendOpenTokens = [&](std::size_t end) {
            for (const std::size_t start : tokenStarts) {
                tokens.emplace_back(text.substr(start, end - start));
            }
            tokenStarts.clear();
        };

        for (std::size_t index = 0; index < text.size(); ++index) {
            const char ch = text[index];
            if (ch == '\0') {
                appendOpenTokens(index);
                return PacketResult<std::vector<std::string>>::Success(std::move(tokens));
            }
            if (ch == '\n' || ch == '\r') {
                appendOpenTokens(index);
                return PacketResult<std::vector<std::string>>::Success(std::move(tokens));
            }
            if (ch == ' ') {
                appendOpenTokens(index);
                tokenStarts.push_back(index + 1U);
                continue;
            }
            if (ch == '\x17') {
                tokenStarts.push_back(index + 1U);
            }
        }

        appendOpenTokens(text.size());
        return PacketResult<std::vector<std::string>>::Success(std::move(tokens));
    }

    [[nodiscard]] inline PacketResult<std::string> BuildPlainTokenString(const std::vector<std::string>& tokens)
    {
        std::size_t totalSize = tokens.size();
        for (const std::string& token : tokens) {
            totalSize += token.size();
        }

        std::string output;
        output.reserve(totalSize);
        for (const std::string& token : tokens) {
            output.append(token);
            output.push_back(' ');
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

    [[nodiscard]] inline PacketResult<std::string> Encode64(std::string_view bytes)
    {
        std::string output;
        output.reserve(((bytes.size() + 2U) / 3U) * 4U);

        for (std::size_t index = 0; index < bytes.size(); index += 3U) {
            const std::uint8_t in1 = detail::ByteAt(bytes, index);
            const bool hasSecond = index + 1U < bytes.size();
            const bool hasThird = index + 2U < bytes.size();
            const std::uint8_t in2 = hasSecond ? detail::ByteAt(bytes, index + 1U) : 0U;
            const std::uint8_t in3 = hasThird ? detail::ByteAt(bytes, index + 2U) : 0U;

            output.push_back(detail::Encode64Digits[((in1 & 0xfcU) >> 2U) & 0x3fU]);
            output.push_back(detail::Encode64Digits[((in1 & 0x03U) << 4U) | ((in2 & 0xf0U) >> 4U)]);
            if (hasSecond) {
                output.push_back(detail::Encode64Digits[((in2 & 0x0fU) << 2U) | ((in3 & 0xc0U) >> 6U)]);
            }
            if (hasThird) {
                output.push_back(detail::Encode64Digits[in3 & 0x3fU]);
            }
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

    [[nodiscard]] inline PacketResult<std::string> Decode64(std::string_view text)
    {
        for (const char ch : text) {
            if (detail::Decode64Digit(ch) < 0) {
                return PacketResult<std::string>::Failure(detail::MakeEncode64Error(
                    PacketErrorCode::DecodeFailed,
                    "encode64 文本包含非法字符"));
            }
        }

        std::string output;
        output.reserve((text.size() / 4U) * 3U + 2U);

        for (std::size_t index = 0; index + 1U < text.size(); index += 4U) {
            const std::size_t remain = text.size() - index;
            const int in1 = detail::Decode64Digit(text[index]);
            const int in2 = detail::Decode64Digit(text[index + 1U]);
            const int in3 = remain >= 3U ? detail::Decode64Digit(text[index + 2U]) : 0;
            const int in4 = remain >= 4U ? detail::Decode64Digit(text[index + 3U]) : 0;

            output.push_back(static_cast<char>((in1 << 2U) | ((in2 & 0x30) >> 4U)));
            if (remain >= 3U) {
                output.push_back(static_cast<char>(((in2 & 0x0f) << 4U) | ((in3 & 0x3c) >> 2U)));
            }
            if (remain >= 4U) {
                output.push_back(static_cast<char>(((in3 & 0x03) << 6U) | (in4 & 0x3f)));
            }
            if (remain < 4U) {
                break;
            }
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

}

#endif //CG_ENGINE_UTILS_H
