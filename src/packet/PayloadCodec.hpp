#ifndef CG_ENGINE_PAYLOAD_CODEC_H
#define CG_ENGINE_PAYLOAD_CODEC_H

#include "PacketTypes.hpp"
#include "Utils.hpp"

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cg::packet
{
    namespace detail
    {
        [[nodiscard]] inline PacketError MakePayloadCodecError(PacketErrorCode code, std::string_view message)
        {
            PacketError error = PacketError::Make(code, message);
            return error;
        }
    }

    [[nodiscard]] inline PacketResult<std::vector<std::string>> SplitPayload(
        std::string_view text,
        std::string_view delimiter)
    {
        if (delimiter.empty()) {
            return PacketResult<std::vector<std::string>>::Failure(detail::MakePayloadCodecError(
                PacketErrorCode::DecodeFailed,
                CG_PACKET_DIAGNOSTIC_TEXT("payload 分隔符不能为空")));
        }

        std::vector<std::string> parts;
        std::size_t start = 0;
        while (true) {
            const std::size_t found = text.find(delimiter, start);
            if (found == std::string_view::npos) {
                parts.emplace_back(text.substr(start));
                break;
            }

            parts.emplace_back(text.substr(start, found - start));
            start = found + delimiter.size();
        }

        return PacketResult<std::vector<std::string>>::Success(std::move(parts));
    }

    [[nodiscard]] inline PacketResult<std::vector<std::string>> SplitPayloadLegacyAware(
        std::string_view text,
        std::string_view delimiter)
    {
        if (delimiter.empty()) {
            return PacketResult<std::vector<std::string>>::Failure(detail::MakePayloadCodecError(
                PacketErrorCode::DecodeFailed,
                CG_PACKET_DIAGNOSTIC_TEXT("payload 分隔符不能为空")));
        }

        std::vector<std::string> parts;
        std::size_t start = 0;
        for (std::size_t index = 0; index < text.size();) {
            if (detail::IsLegacyDoubleByteLead(text[index])) {
                if (index + 1U >= text.size()) {
                    return PacketResult<std::vector<std::string>>::Failure(detail::MakePayloadCodecError(
                        PacketErrorCode::DecodeFailed,
                        CG_PACKET_DIAGNOSTIC_TEXT("payload 旧双字节字符被截断")));
                }

                index += 2U;
                continue;
            }

            if (text.substr(index, delimiter.size()) == delimiter) {
                parts.emplace_back(text.substr(start, index - start));
                index += delimiter.size();
                start = index;
                continue;
            }

            ++index;
        }

        parts.emplace_back(text.substr(start));
        return PacketResult<std::vector<std::string>>::Success(std::move(parts));
    }

    [[nodiscard]] inline PacketResult<std::string> DecodeLegacyPayloadText(std::string_view text)
    {
        std::string output;
        output.reserve(text.size());

        for (std::size_t index = 0; index < text.size(); ++index) {
            const char ch = text[index];
            if (detail::IsLegacyDoubleByteLead(ch)) {
                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(detail::MakePayloadCodecError(
                        PacketErrorCode::DecodeFailed,
                        CG_PACKET_DIAGNOSTIC_TEXT("payload 文本旧双字节字符被截断")));
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
                return PacketResult<std::string>::Failure(detail::MakePayloadCodecError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("payload 文本转义符被截断")));
            }

            const char escaped = text[++index];
            if (detail::IsLegacyDoubleByteLead(escaped)) {
                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(detail::MakePayloadCodecError(
                        PacketErrorCode::DecodeFailed,
                        CG_PACKET_DIAGNOSTIC_TEXT("payload 文本转义后的旧双字节字符被截断")));
                }

                output.push_back(escaped);
                output.push_back(text[index + 1U]);
                ++index;
                continue;
            }

            switch (escaped) {
            case 'n':
                output.push_back('\n');
                break;
            case 'c':
                output.push_back(',');
                break;
            case 'z':
                output.push_back('|');
                break;
            case 'y':
                output.push_back('\\');
                break;
            default:
                output.push_back(escaped);
                break;
            }
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

    [[nodiscard]] inline PacketResult<std::string> EncodeLegacyPayloadText(std::string_view text)
    {
        std::string output;
        output.reserve(text.size());

        for (std::size_t index = 0; index < text.size(); ++index) {
            const char ch = text[index];
            if (detail::IsLegacyDoubleByteLead(ch)) {
                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(detail::MakePayloadCodecError(
                        PacketErrorCode::EncodeFailed,
                        CG_PACKET_DIAGNOSTIC_TEXT("payload 文本旧双字节字符被截断")));
                }

                output.push_back(ch);
                output.push_back(text[index + 1U]);
                ++index;
                continue;
            }

            switch (ch) {
            case '\n':
                output.append("\\n");
                break;
            case ',':
                output.append("\\c");
                break;
            case '|':
                output.append("\\z");
                break;
            case '\\':
                output.append("\\y");
                break;
            default:
                output.push_back(ch);
                break;
            }
        }

        return PacketResult<std::string>::Success(std::move(output));
    }

    [[nodiscard]] inline PacketResult<std::string> BuildPayload(
        std::span<const std::string> parts,
        std::string_view delimiter)
    {
        if (delimiter.empty()) {
            return PacketResult<std::string>::Failure(detail::MakePayloadCodecError(
                PacketErrorCode::EncodeFailed,
                CG_PACKET_DIAGNOSTIC_TEXT("payload 分隔符不能为空")));
        }

        std::size_t totalSize = 0;
        if (!parts.empty()) {
            totalSize = delimiter.size() * (parts.size() - 1U);
        }
        for (const std::string& part : parts) {
            totalSize += part.size();
        }

        std::string output;
        output.reserve(totalSize);
        for (std::size_t index = 0; index < parts.size(); ++index) {
            if (index != 0U) {
                output.append(delimiter);
            }
            output.append(parts[index]);
        }

        return PacketResult<std::string>::Success(std::move(output));
    }
}

#endif // CG_ENGINE_PAYLOAD_CODEC_H
