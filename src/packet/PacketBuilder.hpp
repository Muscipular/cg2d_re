#ifndef CG_ENGINE_PACKET_BUILDER_H
#define CG_ENGINE_PACKET_BUILDER_H

#include "PacketParser.hpp"
#include "Utils.hpp"

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
        [[nodiscard]] inline PacketError MakeBuilderError(
            PacketErrorCode errorCode,
            std::string_view message)
        {
            return PacketError::Make(errorCode, message);
        }

        [[nodiscard]] inline PacketResult<int> ParseDecimalInt(std::string_view text)
        {
            bool negative = false;
            std::size_t index = 0;
            if (!text.empty() && text.front() == '-') {
                negative = true;
                index = 1U;
            }

            if (index == text.size()) {
                return PacketResult<int>::Failure(MakeBuilderError(
                    PacketErrorCode::EncodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("i62 字段十进制文本为空")));
            }

            const std::int64_t maxValue = static_cast<std::int64_t>(std::numeric_limits<int>::max());
            const std::int64_t minValue = static_cast<std::int64_t>(std::numeric_limits<int>::min());
            const std::int64_t limit = negative ? (0 - minValue) : maxValue;
            std::int64_t value = 0;

            for (; index < text.size(); ++index) {
                const char ch = text[index];
                if (ch < '0' || ch > '9') {
                    return PacketResult<int>::Failure(MakeBuilderError(
                        PacketErrorCode::EncodeFailed,
                        CG_PACKET_DIAGNOSTIC_TEXT("i62 字段包含非十进制字符")));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeBuilderError(
                        PacketErrorCode::EncodeFailed,
                        CG_PACKET_DIAGNOSTIC_TEXT("i62 字段超出 int 范围")));
                }

                value = (value * 10) + digit;
            }

            if (negative) {
                value = -value;
            }

            return PacketResult<int>::Success(static_cast<int>(value));
        }

        [[nodiscard]] inline bool IsUnsupportedBuilderFieldType(PacketFieldType type) noexcept
        {
            return type == PacketFieldType::Unknown;
        }

        [[nodiscard]] inline bool IsBuilderFieldTypeCompatible(
            PacketFieldType fieldType,
            PacketFieldType specType) noexcept
        {
            if (specType == PacketFieldType::SubPayload) {
                return fieldType == PacketFieldType::EscapedString || fieldType == PacketFieldType::SubPayload;
            }
            return fieldType == specType;
        }
    }

    /**
     * 构造明文 token 串。
     */
    [[nodiscard]] inline PacketResult<std::string> BuildPacket(const Packet& packet, const PacketSpec& packetSpec)
    {
        const std::string_view code = packet.code.View();
        if (code.empty()) {
            PacketError error = detail::MakeBuilderError(PacketErrorCode::InvalidCode, CG_PACKET_DIAGNOSTIC_TEXT("封包 code 为空"));
            error.packetCode = PacketCode(code);
            return PacketResult<std::string>::Failure(std::move(error));
        }

        if (code != packetSpec.code) {
            PacketError error = detail::MakeBuilderError(PacketErrorCode::InvalidCode, CG_PACKET_DIAGNOSTIC_TEXT("封包 code 与定义不匹配"));
            error.packetCode = PacketCode(code);
            return PacketResult<std::string>::Failure(std::move(error));
        }

        if (packet.direction != packetSpec.direction) {
            PacketError error = detail::MakeBuilderError(
                PacketErrorCode::EncodeFailed,
                CG_PACKET_DIAGNOSTIC_TEXT("封包方向不匹配"));
            error.packetCode = PacketCode(code);
            return PacketResult<std::string>::Failure(std::move(error));
        }

        if (!packetSpec.repeatedTail.enabled) {
            if (packet.fields.size() != packetSpec.fieldCount) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("封包字段数量不匹配"));
                error.packetCode = PacketCode(code);
                error.fieldIndex = packet.fields.size() < packetSpec.fieldCount ? packet.fields.size() : packetSpec.fieldCount;
                return PacketResult<std::string>::Failure(std::move(error));
            }
        } else {
            if (!detail::IsRepeatedTailSpecValid(packetSpec)) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("重复尾字段定义无效"));
                detail::FillErrorContext(
                    error,
                    code,
                    packetSpec.repeatedTail.countFieldIndex);
                return PacketResult<std::string>::Failure(std::move(error));
            }

            const std::size_t countFieldIndex = packetSpec.repeatedTail.countFieldIndex;
            if (packet.fields.size() <= countFieldIndex) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("封包字段数量不匹配"));
                error.packetCode = PacketCode(code);
                error.fieldIndex = packet.fields.size();
                return PacketResult<std::string>::Failure(std::move(error));
            }

            PacketResult<int> decodedCount =
                detail::ParseDecimalInt(packet.fields[countFieldIndex].value);
            if (!decodedCount.HasValue()) {
                PacketError error = decodedCount.Error();
                detail::FillErrorContext(error, code, countFieldIndex);
                return PacketResult<std::string>::Failure(std::move(error));
            }
            if (decodedCount.Value() < 0) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("重复尾字段数量不能为负数"));
                detail::FillErrorContext(error, code, countFieldIndex);
                return PacketResult<std::string>::Failure(std::move(error));
            }

            const std::size_t fixedFieldCount = packetSpec.fieldCount - 1U;
            const std::size_t expectedFieldCount =
                fixedFieldCount + static_cast<std::size_t>(decodedCount.Value());
            if (packet.fields.size() != expectedFieldCount) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("重复尾字段数量与 count 不匹配"));
                error.packetCode = PacketCode(code);
                error.fieldIndex = packet.fields.size() < expectedFieldCount
                    ? packet.fields.size()
                    : expectedFieldCount;
                return PacketResult<std::string>::Failure(std::move(error));
            }
        }

        std::vector<std::string> tokens;
        tokens.reserve(packet.fields.size() + 1U);
        tokens.emplace_back(code);

        for (std::size_t index = 0; index < packet.fields.size(); ++index) {
            const PacketFieldSpec& fieldSpec =
                packetSpec.fields[detail::RuntimeFieldSpecIndex(packetSpec, index)];
            const PacketField& field = packet.fields[index];

            if (!CG_PACKET_FIELD_NAME_MATCHES(field.name, fieldSpec.name)) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::EncodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("封包字段名不匹配"));
                detail::FillErrorContext(error, code, index);
                return PacketResult<std::string>::Failure(std::move(error));
            }

            if (detail::IsUnsupportedBuilderFieldType(field.type) ||
                detail::IsUnsupportedBuilderFieldType(fieldSpec.type)) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("封包字段类型暂不支持"));
                detail::FillErrorContext(error, code, index);
                return PacketResult<std::string>::Failure(std::move(error));
            }

            if (!detail::IsBuilderFieldTypeCompatible(field.type, fieldSpec.type)) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::EncodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("封包字段类型不匹配"));
                detail::FillErrorContext(error, code, index);
                return PacketResult<std::string>::Failure(std::move(error));
            }

            switch (fieldSpec.type) {
            case PacketFieldType::Int62: {
                PacketResult<int> decimal = detail::ParseDecimalInt(field.value);
                if (!decimal.HasValue()) {
                    PacketError error = decimal.Error();
                    detail::FillErrorContext(error, code, index);
                    return PacketResult<std::string>::Failure(std::move(error));
                }

                PacketResult<std::string> encoded = EncodeInt62(decimal.Value());
                if (!encoded.HasValue()) {
                    PacketError error = encoded.Error();
                    detail::FillErrorContext(error, code, index);
                    return PacketResult<std::string>::Failure(std::move(error));
                }
                tokens.push_back(encoded.Value());
                break;
            }
            case PacketFieldType::EscapedString:
            case PacketFieldType::SubPayload: {
                PacketResult<std::string> encoded = EncodeEscapedString(field.value);
                if (!encoded.HasValue()) {
                    PacketError error = encoded.Error();
                    detail::FillErrorContext(error, code, index);
                    return PacketResult<std::string>::Failure(std::move(error));
                }
                tokens.push_back(encoded.Value());
                break;
            }
            case PacketFieldType::Unknown: {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("封包字段类型暂不支持"));
                detail::FillErrorContext(error, code, index);
                return PacketResult<std::string>::Failure(std::move(error));
            }
            }
        }

        return BuildPlainTokenString(tokens);
    }
}

#endif // CG_ENGINE_PACKET_BUILDER_H
