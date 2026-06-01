#ifndef CG_ENGINE_PAYLOAD_CODEC_H
#define CG_ENGINE_PAYLOAD_CODEC_H

#include "PacketTypes.hpp"
#include "utils.hpp"

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
        [[nodiscard]] inline PacketError MakePayloadCodecError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.source = PacketSourceTag::Manual;
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
                "payload 分隔符不能为空"));
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
                "payload 分隔符不能为空"));
        }

        std::vector<std::string> parts;
        std::size_t start = 0;
        for (std::size_t index = 0; index < text.size();) {
            if (detail::IsLegacyDoubleByteLead(text[index])) {
                if (index + 1U >= text.size()) {
                    return PacketResult<std::vector<std::string>>::Failure(detail::MakePayloadCodecError(
                        PacketErrorCode::DecodeFailed,
                        "payload 旧双字节字符被截断"));
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

    [[nodiscard]] inline PacketResult<std::string> BuildPayload(
        std::span<const std::string> parts,
        std::string_view delimiter)
    {
        if (delimiter.empty()) {
            return PacketResult<std::string>::Failure(detail::MakePayloadCodecError(
                PacketErrorCode::EncodeFailed,
                "payload 分隔符不能为空"));
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
