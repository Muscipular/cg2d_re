#ifndef CG_ENGINE_PACKET_PARSER_H
#define CG_ENGINE_PACKET_PARSER_H

#include "PacketTypes.hpp"
#include "utils.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace cg::packet
{
    namespace detail
    {
        struct PacketFieldSpec
        {
            std::string_view name;
            PacketFieldType type = PacketFieldType::Unknown;
            PacketSourceTag source = PacketSourceTag::Unknown;
        };

        struct PacketSpec
        {
            std::string_view code;
            PacketDirection direction = PacketDirection::ClientToServer;
            PacketSourceTag source = PacketSourceTag::Unknown;
            const PacketFieldSpec* fields = nullptr;
            std::size_t fieldCount = 0;
        };

        inline constexpr std::array<PacketFieldSpec, 3> ClientLoginFields = {{
            {"clientHash", PacketFieldType::EscapedString, PacketSourceTag::ExistingPrototype},
            {"password", PacketFieldType::EscapedString, PacketSourceTag::ExistingPrototype},
            {"account", PacketFieldType::EscapedString, PacketSourceTag::ExistingPrototype},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> ClientLoginResultFields = {{
            {"result", PacketFieldType::Int62, PacketSourceTag::ExistingPrototype},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ExistingPrototype},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileVersionFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CharacterProfileFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CharacterProfile2Fields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> JobNameFields = {{
            {"jobassort", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"name", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CheckCommandFields = {{
            {"time", PacketFieldType::Int62, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> MapNameFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ObjectListFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CharacterIconFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ObjectDeleteFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 7> MapDataFields = {{
            {"mapid", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"floor", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"x1", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"y1", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"x2", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"y2", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 6> MapDataRequestFields = {{
            {"mapid", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"floor", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"x1", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"y1", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"x2", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"y2", PacketFieldType::Int62, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketSpec, 14> PacketRegistry = {{
            {
                "JFVf",
                PacketDirection::ClientToServer,
                PacketSourceTag::ExistingPrototype,
                ClientLoginFields.data(),
                ClientLoginFields.size(),
            },
            {
                "whV",
                PacketDirection::ServerToClient,
                PacketSourceTag::ExistingPrototype,
                ClientLoginResultFields.data(),
                ClientLoginResultFields.size(),
            },
            {
                "HNP",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ProfileVersionFields.data(),
                ProfileVersionFields.size(),
            },
            {
                "aPc",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                CharacterProfileFields.data(),
                CharacterProfileFields.size(),
            },
            {
                "UU",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                CharacterProfile2Fields.data(),
                CharacterProfile2Fields.size(),
            },
            {
                "jv",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                JobNameFields.data(),
                JobNameFields.size(),
            },
            {
                "CCL",
                PacketDirection::ClientToServer,
                PacketSourceTag::Xlsx,
                CheckCommandFields.data(),
                CheckCommandFields.size(),
            },
            {
                "TeD",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                MapNameFields.data(),
                MapNameFields.size(),
            },
            {
                "nsY",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ObjectListFields.data(),
                ObjectListFields.size(),
            },
            {
                "NI",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                CharacterIconFields.data(),
                CharacterIconFields.size(),
            },
            {
                "aBS",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                MapDataFields.data(),
                MapDataFields.size(),
            },
            {
                "UUN",
                PacketDirection::ClientToServer,
                PacketSourceTag::Xlsx,
                MapDataRequestFields.data(),
                MapDataRequestFields.size(),
            },
            {
                "uSB",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ObjectDeleteFields.data(),
                ObjectDeleteFields.size(),
            },
            {
                "Gp",
                PacketDirection::ClientToServer,
                PacketSourceTag::Xlsx,
                nullptr,
                0,
            },
        }};

        [[nodiscard]] inline const PacketSpec* FindPacketSpec(std::string_view code) noexcept
        {
            for (const PacketSpec& spec : PacketRegistry) {
                if (spec.code == code) {
                    return &spec;
                }
            }
            return nullptr;
        }

        [[nodiscard]] inline PacketError MakeParserError(
            PacketErrorCode errorCode,
            std::string message,
            PacketSourceTag source = PacketSourceTag::Unknown)
        {
            PacketError error = PacketError::Make(errorCode, std::move(message));
            error.source = source;
            return error;
        }

        inline void FillErrorContext(PacketError& error, std::string_view code, std::size_t fieldIndex)
        {
            error.packetCode = PacketCode(code);
            error.fieldIndex = fieldIndex;
        }
    }

    /**
     * 解析 DecodeEnvelope 后的明文 token 串；不接受 envelope 文本。
     */
    [[nodiscard]] inline PacketResult<Packet> ParsePacket(std::string_view plainTokens)
    {
        PacketResult<std::vector<std::string>> splitResult = SplitPlainTokens(plainTokens);
        if (!splitResult.HasValue()) {
            return PacketResult<Packet>::Failure(splitResult.Error());
        }

        std::vector<std::string> tokens = splitResult.Value();
        if (tokens.empty()) {
            return PacketResult<Packet>::Failure(detail::MakeParserError(
                PacketErrorCode::EmptyInput,
                "封包明文 token 输入为空"));
        }

        const std::string& code = tokens.front();
        if (code.empty()) {
            PacketError error = detail::MakeParserError(PacketErrorCode::InvalidCode, "封包 code 为空");
            error.packetCode = PacketCode(code);
            return PacketResult<Packet>::Failure(std::move(error));
        }

        const detail::PacketSpec* spec = detail::FindPacketSpec(code);
        if (spec == nullptr) {
            PacketError error = detail::MakeParserError(PacketErrorCode::UnknownCode, "封包 code 未注册");
            error.packetCode = PacketCode(code);
            return PacketResult<Packet>::Failure(std::move(error));
        }

        std::vector<std::string> fieldTokens(tokens.begin() + 1, tokens.end());
        if (fieldTokens.size() == spec->fieldCount + 1U && !fieldTokens.empty() && fieldTokens.back().empty()) {
            fieldTokens.pop_back();
        }

        if (fieldTokens.size() != spec->fieldCount) {
            PacketError error = detail::MakeParserError(
                PacketErrorCode::FieldCountMismatch,
                "封包字段数量不匹配",
                spec->source);
            error.packetCode = PacketCode(code);
            error.fieldIndex = fieldTokens.size() < spec->fieldCount ? fieldTokens.size() : spec->fieldCount;
            return PacketResult<Packet>::Failure(std::move(error));
        }

        Packet packet(PacketCode(code), spec->direction, spec->source);
        packet.fields.reserve(spec->fieldCount);

        for (std::size_t index = 0; index < spec->fieldCount; ++index) {
            const detail::PacketFieldSpec& fieldSpec = spec->fields[index];
            const std::string& token = fieldTokens[index];

            PacketField field;
            field.name = std::string(fieldSpec.name);
            field.type = fieldSpec.type;
            field.source = fieldSpec.source;

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
            case PacketFieldType::EscapedString: {
                PacketResult<std::string> decoded = DecodeEscapedString(token);
                if (!decoded.HasValue()) {
                    PacketError error = decoded.Error();
                    detail::FillErrorContext(error, code, index);
                    return PacketResult<Packet>::Failure(std::move(error));
                }
                field.value = decoded.Value();
                break;
            }
            case PacketFieldType::RawString:
                field.value = token;
                break;
            case PacketFieldType::Payload:
            case PacketFieldType::Unknown: {
                PacketError error = detail::MakeParserError(
                    PacketErrorCode::InvalidFieldType,
                    "封包字段类型暂不支持",
                    fieldSpec.source);
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
