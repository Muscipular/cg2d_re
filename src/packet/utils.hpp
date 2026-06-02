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
        inline constexpr int JEncodeKey = 4595;

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

        [[nodiscard]] inline PacketError MakeEnvelopeCodecError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            return error;
        }

        [[nodiscard]] inline std::uint8_t ByteAt(std::string_view bytes, std::size_t index) noexcept
        {
            return static_cast<std::uint8_t>(static_cast<unsigned char>(bytes[index]));
        }

        [[nodiscard]] inline std::string TrimEnvelopeLineEnding(std::string_view text)
        {
            if (!text.empty() && text.back() == '\n') {
                text.remove_suffix(1);
                if (!text.empty() && text.back() == '\r') {
                    text.remove_suffix(1);
                }
            } else if (!text.empty() && text.back() == '\r') {
                text.remove_suffix(1);
            }

            return std::string(text);
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
                return PacketResult<std::string>::Failure(detail::MakeEnvelopeCodecError(
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

    [[nodiscard]] inline PacketResult<std::string> JEncode(std::string_view bytes)
    {
        if (bytes.empty()) {
            return PacketResult<std::string>::Failure(detail::MakeEnvelopeCodecError(
                PacketErrorCode::EncodeFailed,
                "jEncode 输入不能为空"));
        }

        std::string work(bytes);
        std::uint8_t sum = 0;
        for (std::size_t index = 0; index < work.size(); ++index) {
            std::uint8_t value = detail::ByteAt(work, index);
            sum = static_cast<std::uint8_t>(sum + value);
            if ((detail::JEncodeKey % 7) == static_cast<int>(index % 5U) ||
                (detail::JEncodeKey % 2) == static_cast<int>(index % 2U)) {
                value = static_cast<std::uint8_t>(~value);
                work[index] = static_cast<char>(value);
            }
        }

        const std::size_t pivot = static_cast<std::size_t>(detail::JEncodeKey) % bytes.size();
        std::string output;
        output.resize(bytes.size() + 1U);
        for (std::size_t index = 0; index < output.size(); ++index) {
            if (pivot > index) {
                const std::uint8_t value = static_cast<std::uint8_t>(
                    detail::ByteAt(work, index) + sum * ((index * index) % 3U));
                output[index] = static_cast<char>(value);
            } else if (pivot == index) {
                output[index] = static_cast<char>(sum);
            } else {
                const std::uint8_t value = static_cast<std::uint8_t>(
                    detail::ByteAt(work, index - 1U) + sum * ((index * index) % 7U));
                output[index] = static_cast<char>(value);
            }
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

    [[nodiscard]] inline PacketResult<std::string> JDecode(std::string_view bytes)
    {
        if (bytes.size() <= 1U) {
            return PacketResult<std::string>::Failure(detail::MakeEnvelopeCodecError(
                PacketErrorCode::DecodeFailed,
                "jDecode 输入长度不足"));
        }

        const std::size_t decodedSize = bytes.size() - 1U;
        const std::size_t pivot = static_cast<std::size_t>(detail::JEncodeKey) % decodedSize;
        const std::uint8_t sum = detail::ByteAt(bytes, pivot);
        std::string output;
        output.resize(decodedSize);

        for (std::size_t index = 0; index < bytes.size(); ++index) {
            if (pivot > index) {
                const std::uint8_t value = static_cast<std::uint8_t>(
                    detail::ByteAt(bytes, index) - sum * ((index * index) % 3U));
                output[index] = static_cast<char>(value);
            } else if (pivot < index) {
                const std::uint8_t value = static_cast<std::uint8_t>(
                    detail::ByteAt(bytes, index) - sum * ((index * index) % 7U));
                output[index - 1U] = static_cast<char>(value);
            }
        }

        for (std::size_t index = 0; index < output.size(); ++index) {
            if ((detail::JEncodeKey % 7) == static_cast<int>(index % 5U) ||
                (detail::JEncodeKey % 2) == static_cast<int>(index % 2U)) {
                output[index] = static_cast<char>(~detail::ByteAt(output, index));
            }
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

    [[nodiscard]] inline PacketResult<std::string> EncodeEnvelope(std::string_view payload, bool noCompress = false)
    {
        if (payload.size() >= 99U && !noCompress) {
            return PacketResult<std::string>::Failure(detail::MakeEnvelopeCodecError(
                PacketErrorCode::NotImplemented,
                "ringo 压缩 encode 路径待补"));
        }

        const std::size_t payloadSizeWithNul = payload.size() + 1U;
        if (payloadSizeWithNul > 255U) {
            return PacketResult<std::string>::Failure(detail::MakeEnvelopeCodecError(
                PacketErrorCode::EncodeFailed,
                "未压缩 envelope 长度超过 flag 字节范围"));
        }

        std::uint8_t flag = static_cast<std::uint8_t>(payloadSizeWithNul);
        if ((flag % 2U) == 1U) {
            ++flag;
        }

        std::string plain;
        plain.reserve(payloadSizeWithNul + 1U);
        plain.push_back(static_cast<char>(flag));
        plain.append(payload);
        plain.push_back('\0');

        PacketResult<std::string> jEncoded = JEncode(plain);
        if (!jEncoded.HasValue()) {
            return jEncoded;
        }

        return Encode64(jEncoded.Value());
    }

    [[nodiscard]] inline PacketResult<std::string> DecodeEnvelope(std::string_view text)
    {
        const std::string trimmed = detail::TrimEnvelopeLineEnding(text);
        PacketResult<std::string> decoded64 = Decode64(trimmed);
        if (!decoded64.HasValue()) {
            return decoded64;
        }

        PacketResult<std::string> decoded = JDecode(decoded64.Value());
        if (!decoded.HasValue()) {
            return decoded;
        }
        if (decoded.Value().size() < 2U) {
            return PacketResult<std::string>::Failure(detail::MakeEnvelopeCodecError(
                PacketErrorCode::DecodeFailed,
                "envelope 解码后长度不足"));
        }

        const std::uint8_t flag = detail::ByteAt(decoded.Value(), 0);
        if ((flag % 2U) == 1U) {
            return PacketResult<std::string>::Failure(detail::MakeEnvelopeCodecError(
                PacketErrorCode::NotImplemented,
                "ringo 压缩 decode 路径待补"));
        }

        std::string payload(decoded.Value().substr(1U));
        if (payload.empty() || payload.back() != '\0') {
            return PacketResult<std::string>::Failure(detail::MakeEnvelopeCodecError(
                PacketErrorCode::DecodeFailed,
                "未压缩 envelope payload 缺少尾 NUL"));
        }
        payload.pop_back();
        return PacketResult<std::string>::Success(std::move(payload));
    }
}

#endif //CG_ENGINE_UTILS_H
