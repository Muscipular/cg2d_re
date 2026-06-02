#ifndef CG_ENGINE_PACKET_BUILDER_H
#define CG_ENGINE_PACKET_BUILDER_H

#include "PacketParser.hpp"
#include "utils.hpp"

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
            std::string message)
        {
            return PacketError::Make(errorCode, std::move(message));
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
                    "i62 字段十进制文本为空"));
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
                        "i62 字段包含非十进制字符"));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeBuilderError(
                        PacketErrorCode::EncodeFailed,
                        "i62 字段超出 int 范围"));
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
            return type == PacketFieldType::Payload || type == PacketFieldType::Unknown;
        }
    }

    /**
     * 构造 DecodeEnvelope 前的明文 token 串；不生成 envelope 或换行。
     */
    [[nodiscard]] inline PacketResult<std::string> BuildPacket(const Packet& packet)
    {
        const std::string_view code = packet.code.View();
        if (code.empty()) {
            PacketError error = detail::MakeBuilderError(PacketErrorCode::InvalidCode, "封包 code 为空");
            error.packetCode = PacketCode(code);
            return PacketResult<std::string>::Failure(std::move(error));
        }

        const PacketSpec* spec = detail::FindPacketSpec(code);
        if (spec == nullptr) {
            PacketError error = detail::MakeBuilderError(PacketErrorCode::UnknownCode, "封包 code 未注册");
            error.packetCode = PacketCode(code);
            return PacketResult<std::string>::Failure(std::move(error));
        }

        if (packet.direction != spec->direction) {
            PacketError error = detail::MakeBuilderError(
                PacketErrorCode::EncodeFailed,
                "封包方向不匹配");
            error.packetCode = PacketCode(code);
            return PacketResult<std::string>::Failure(std::move(error));
        }

        if (packet.fields.size() != spec->fieldCount) {
            PacketError error = detail::MakeBuilderError(
                PacketErrorCode::FieldCountMismatch,
                "封包字段数量不匹配");
            error.packetCode = PacketCode(code);
            error.fieldIndex = packet.fields.size() < spec->fieldCount ? packet.fields.size() : spec->fieldCount;
            return PacketResult<std::string>::Failure(std::move(error));
        }

        std::vector<std::string> tokens;
        tokens.reserve(packet.fields.size() + 1U);
        tokens.emplace_back(code);

        for (std::size_t index = 0; index < packet.fields.size(); ++index) {
            const PacketFieldSpec& fieldSpec = spec->fields[index];
            const PacketField& field = packet.fields[index];

            if (field.name != fieldSpec.name) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::EncodeFailed,
                    "封包字段名不匹配");
                detail::FillErrorContext(error, code, index);
                return PacketResult<std::string>::Failure(std::move(error));
            }

            if (detail::IsUnsupportedBuilderFieldType(field.type) ||
                detail::IsUnsupportedBuilderFieldType(fieldSpec.type)) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::InvalidFieldType,
                    "封包字段类型暂不支持");
                detail::FillErrorContext(error, code, index);
                return PacketResult<std::string>::Failure(std::move(error));
            }

            if (field.type != fieldSpec.type) {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::EncodeFailed,
                    "封包字段类型不匹配");
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
            case PacketFieldType::EscapedString: {
                PacketResult<std::string> encoded = EncodeEscapedString(field.value);
                if (!encoded.HasValue()) {
                    PacketError error = encoded.Error();
                    detail::FillErrorContext(error, code, index);
                    return PacketResult<std::string>::Failure(std::move(error));
                }
                tokens.push_back(encoded.Value());
                break;
            }
            case PacketFieldType::RawString:
                tokens.push_back(field.value);
                break;
            case PacketFieldType::Payload:
            case PacketFieldType::Unknown: {
                PacketError error = detail::MakeBuilderError(
                    PacketErrorCode::InvalidFieldType,
                    "封包字段类型暂不支持");
                detail::FillErrorContext(error, code, index);
                return PacketResult<std::string>::Failure(std::move(error));
            }
            }
        }

        return BuildPlainTokenString(tokens);
    }
}

#endif // CG_ENGINE_PACKET_BUILDER_H
