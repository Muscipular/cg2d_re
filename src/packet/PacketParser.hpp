#ifndef CG_ENGINE_PACKET_PARSER_H
#define CG_ENGINE_PACKET_PARSER_H

#include "PacketTypes.hpp"
#include "utils.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cg::packet
{
    struct PacketFieldSpec
    {
        std::string_view name;
        PacketFieldType type = PacketFieldType::Unknown;
    };

    struct PacketSpec
    {
        std::string_view code;
        PacketDirection direction = PacketDirection::ClientToServer;
        const PacketFieldSpec* fields = nullptr;
        std::size_t fieldCount = 0;
    };

    namespace detail
    {
        [[nodiscard]] inline PacketError MakeParserError(
            PacketErrorCode errorCode,
            std::string message)
        {
            return PacketError::Make(errorCode, std::move(message));
        }

        inline void FillErrorContext(PacketError& error, std::string_view code, std::size_t fieldIndex)
        {
            error.packetCode = PacketCode(code);
            error.fieldIndex = fieldIndex;
        }

        [[nodiscard]] inline PacketResult<std::string> DecodeNestedPayloadEscapedString(std::string_view text)
        {
            std::string output;
            output.reserve(text.size());

            for (std::size_t index = 0; index < text.size(); ++index) {
                const char ch = text[index];
                if (IsLegacyDoubleByteLead(ch)) {
                    if (index + 1U >= text.size()) {
                        return PacketResult<std::string>::Failure(MakeEscapedStringError(
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
                } else if (escaped == '\\') {
                    output.push_back('\\');
                } else {
                    output.push_back('\\');
                    output.push_back(escaped);
                }
                ++index;
            }

            return PacketResult<std::string>::Success(std::move(output));
        }

    }

    /**
     * 解析明文 token 串。
     */
    [[nodiscard]] inline PacketResult<Packet> ParsePacket(
        std::string_view plainTokens,
        const PacketSpec& packetSpec)
    {
        PacketResult<std::vector<std::string_view>> splitResult = SplitPlainTokens(plainTokens);
        if (!splitResult.HasValue()) {
            return PacketResult<Packet>::Failure(splitResult.Error());
        }

        const std::vector<std::string_view>& tokens = splitResult.Value();
        if (tokens.empty()) {
            return PacketResult<Packet>::Failure(detail::MakeParserError(
                PacketErrorCode::EmptyInput,
                "封包明文 token 输入为空"));
        }

        const std::string_view code = tokens.front();
        if (code.empty()) {
            PacketError error = detail::MakeParserError(PacketErrorCode::InvalidCode, "封包 code 为空");
            error.packetCode = PacketCode(code);
            return PacketResult<Packet>::Failure(std::move(error));
        }

        if (code != packetSpec.code) {
            PacketError error = detail::MakeParserError(PacketErrorCode::InvalidCode, "封包 code 与定义不匹配");
            error.packetCode = PacketCode(code);
            return PacketResult<Packet>::Failure(std::move(error));
        }

        std::size_t fieldTokenCount = tokens.size() - 1U;
        if (fieldTokenCount == packetSpec.fieldCount + 1U && tokens.back().empty()) {
            --fieldTokenCount;
        }

        if (fieldTokenCount != packetSpec.fieldCount) {
            PacketError error = detail::MakeParserError(
                PacketErrorCode::FieldCountMismatch,
                "封包字段数量不匹配");
            error.packetCode = PacketCode(code);
            error.fieldIndex = fieldTokenCount < packetSpec.fieldCount
                ? fieldTokenCount
                : packetSpec.fieldCount;
            return PacketResult<Packet>::Failure(std::move(error));
        }

        Packet packet(PacketCode(code), packetSpec.direction);
        packet.fields.reserve(packetSpec.fieldCount);

        for (std::size_t index = 0; index < packetSpec.fieldCount; ++index) {
            const PacketFieldSpec& fieldSpec = packetSpec.fields[index];
            const std::string_view token = tokens[index + 1U];

            PacketField field;
            field.name = std::string(fieldSpec.name);
            field.type = fieldSpec.type == PacketFieldType::SubPayload
                ? PacketFieldType::EscapedString
                : fieldSpec.type;

            switch (fieldSpec.type) {
            case PacketFieldType::Int62: {
                PacketResult<int> decoded = DecodeInt62(token);
                if (!decoded.HasValue()) {
                    PacketError error = decoded.Error();
                    detail::FillErrorContext(error, code, index);
                    return PacketResult<Packet>::Failure(std::move(error));
                }
                field.value = std::to_string(decoded.Value());
                break;
            }
            case PacketFieldType::EscapedString:
            case PacketFieldType::SubPayload: {
                PacketResult<std::string> decoded = fieldSpec.type == PacketFieldType::SubPayload
                    ? detail::DecodeNestedPayloadEscapedString(token)
                    : DecodeEscapedString(token);
                if (!decoded.HasValue()) {
                    PacketError error = decoded.Error();
                    detail::FillErrorContext(error, code, index);
                    return PacketResult<Packet>::Failure(std::move(error));
                }
                field.value = decoded.Value();
                break;
            }
            case PacketFieldType::Payload:
            case PacketFieldType::Unknown: {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::InvalidFieldType,
                    "封包字段类型暂不支持");
                detail::FillErrorContext(error, code, index);
                return PacketResult<Packet>::Failure(std::move(error));
            }
            }

            packet.fields.push_back(std::move(field));
        }

        return PacketResult<Packet>::Success(std::move(packet));
    }
}

#endif // CG_ENGINE_PACKET_PARSER_H
