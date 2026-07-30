#ifndef CG_ENGINE_PACKET_PARSER_H
#define CG_ENGINE_PACKET_PARSER_H

#include "PacketTypes.hpp"
#include "Utils.hpp"

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

        constexpr PacketFieldSpec() = default;

        constexpr PacketFieldSpec(std::string_view fieldName, PacketFieldType fieldType) noexcept
            : name(fieldName),
              type(fieldType)
        {
        }
    };

    struct PacketRepeatedTailSpec
    {
        // 默认关闭；启用后，最后一个模板字段按 countFieldIndex 指定的 Int62 数量重复。
        bool enabled = false;
        std::size_t countFieldIndex = 0U;
    };

    struct PacketSpec
    {
        std::string_view code;
        PacketDirection direction = PacketDirection::ClientToServer;
        const PacketFieldSpec* fields = nullptr;
        std::size_t fieldCount = 0;
        // 描述 PacketSpec 最后一个模板字段的可重复尾部规则。
        PacketRepeatedTailSpec repeatedTail {};
    };

    namespace detail
    {
        [[nodiscard]] inline PacketError MakeParserError(
            PacketErrorCode errorCode,
            std::string_view message)
        {
            return PacketError::Make(errorCode, message);
        }

        inline void FillErrorContext(PacketError& error, std::string_view code, std::size_t fieldIndex)
        {
            error.packetCode = PacketCode(code);
            error.fieldIndex = fieldIndex;
        }

        [[nodiscard]] constexpr bool IsRepeatedTailSpecValid(const PacketSpec& packetSpec) noexcept
        {
            if (!packetSpec.repeatedTail.enabled) {
                return true;
            }
            return packetSpec.fields != nullptr &&
                packetSpec.fieldCount >= 2U &&
                packetSpec.repeatedTail.countFieldIndex < packetSpec.fieldCount - 1U &&
                packetSpec.fields[packetSpec.repeatedTail.countFieldIndex].type ==
                    PacketFieldType::Int62;
        }

        [[nodiscard]] constexpr std::size_t RuntimeFieldSpecIndex(
            const PacketSpec& packetSpec,
            std::size_t fieldIndex) noexcept
        {
            if (packetSpec.repeatedTail.enabled && fieldIndex >= packetSpec.fieldCount - 1U) {
                return packetSpec.fieldCount - 1U;
            }
            return fieldIndex;
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
                            CG_PACKET_DIAGNOSTIC_TEXT("es 字符串旧双字节字符被截断")));
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
                CG_PACKET_DIAGNOSTIC_TEXT("封包明文 token 输入为空")));
        }

        const std::string_view code = tokens.front();
        if (code.empty()) {
            PacketError error = detail::MakeParserError(PacketErrorCode::InvalidCode, CG_PACKET_DIAGNOSTIC_TEXT("封包 code 为空"));
            error.packetCode = PacketCode(code);
            return PacketResult<Packet>::Failure(std::move(error));
        }

        if (code != packetSpec.code) {
            PacketError error = detail::MakeParserError(PacketErrorCode::InvalidCode, CG_PACKET_DIAGNOSTIC_TEXT("封包 code 与定义不匹配"));
            error.packetCode = PacketCode(code);
            return PacketResult<Packet>::Failure(std::move(error));
        }

        std::size_t fieldTokenCount = tokens.size() - 1U;
        if (!packetSpec.repeatedTail.enabled) {
            if (fieldTokenCount == packetSpec.fieldCount + 1U && tokens.back().empty()) {
                --fieldTokenCount;
            }

            if (fieldTokenCount != packetSpec.fieldCount) {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("封包字段数量不匹配"));
                error.packetCode = PacketCode(code);
                error.fieldIndex = fieldTokenCount < packetSpec.fieldCount
                    ? fieldTokenCount
                    : packetSpec.fieldCount;
                return PacketResult<Packet>::Failure(std::move(error));
            }
        } else {
            if (!detail::IsRepeatedTailSpecValid(packetSpec)) {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("重复尾字段定义无效"));
                detail::FillErrorContext(
                    error,
                    code,
                    packetSpec.repeatedTail.countFieldIndex);
                return PacketResult<Packet>::Failure(std::move(error));
            }

            const std::size_t countFieldIndex = packetSpec.repeatedTail.countFieldIndex;
            if (fieldTokenCount <= countFieldIndex) {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("封包字段数量不匹配"));
                error.packetCode = PacketCode(code);
                error.fieldIndex = fieldTokenCount;
                return PacketResult<Packet>::Failure(std::move(error));
            }

            PacketResult<int> decodedCount = DecodeInt62(tokens[countFieldIndex + 1U]);
            if (!decodedCount.HasValue()) {
                PacketError error = decodedCount.Error();
                detail::FillErrorContext(error, code, countFieldIndex);
                return PacketResult<Packet>::Failure(std::move(error));
            }
            if (decodedCount.Value() < 0) {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("重复尾字段数量不能为负数"));
                detail::FillErrorContext(error, code, countFieldIndex);
                return PacketResult<Packet>::Failure(std::move(error));
            }

            const std::size_t fixedFieldCount = packetSpec.fieldCount - 1U;
            const std::size_t expectedFieldCount =
                fixedFieldCount + static_cast<std::size_t>(decodedCount.Value());
            if (fieldTokenCount > fixedFieldCount && tokens.back().empty()) {
                --fieldTokenCount;
            }
            if (fieldTokenCount != expectedFieldCount) {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("重复尾字段数量与 count 不匹配"));
                error.packetCode = PacketCode(code);
                error.fieldIndex = fieldTokenCount < expectedFieldCount
                    ? fieldTokenCount
                    : expectedFieldCount;
                return PacketResult<Packet>::Failure(std::move(error));
            }
        }

        Packet packet(PacketCode(code), packetSpec.direction);
        packet.fields.reserve(fieldTokenCount);

        for (std::size_t index = 0; index < fieldTokenCount; ++index) {
            const PacketFieldSpec& fieldSpec =
                packetSpec.fields[detail::RuntimeFieldSpecIndex(packetSpec, index)];
            const std::string_view token = tokens[index + 1U];

            PacketField field;
            field.name = CG_PACKET_DIAGNOSTIC_STRING(fieldSpec.name);
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
            case PacketFieldType::Unknown: {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("封包字段类型暂不支持"));
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
