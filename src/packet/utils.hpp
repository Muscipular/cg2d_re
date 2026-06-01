#ifndef CG_ENGINE_UTILS_H
#define CG_ENGINE_UTILS_H
#include "PacketTypes.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <utility>

namespace cg::packet
{
    namespace detail
    {
        inline constexpr std::string_view Int62Digits =
            "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        inline constexpr int Int62Radix = 62;

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

        [[nodiscard]] inline PacketError MakeInt62Error(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.source = PacketSourceTag::ServerC;
            return error;
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

    // toto encode string

    // toto decode string
}

#endif //CG_ENGINE_UTILS_H
