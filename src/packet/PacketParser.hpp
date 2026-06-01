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

        inline constexpr std::array<PacketFieldSpec, 1> CharacterSkillDataFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ItemDataFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> PetItemDataFields = {{
            {"petSlot", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ItemInfoListFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> ItemRecipeFields = {{
            {"haveskillindex", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> AlbumFlagsFields = {{
            {"albumversion", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 17> AlbumInfoFields = {{
            {"albumId", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"imageNumber", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"catchFlag", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"rare", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"type", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"vital", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"strength", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"toughness", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"quick", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"magic", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"earth", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"water", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"fire", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"wind", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"slot", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"comment", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
            {"name", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> AlbumNameFields = {{
            {"albumId", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"name", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 8> PetMailAnimationFields = {{
            {"objIndex", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"graphicsno", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"x", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"y", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"dir", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"flg", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"no", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> TitleListFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileListFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> AboutListFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 13> ProfileDataFields = {{
            {"name", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"lv", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"job", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"title", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"guildName", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"graphic", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"sid", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"smsg", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"bid", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"bmsg", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"aid", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"amsg", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"pmsg", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 7> SetProfileDataFields = {{
            {"sellFlag", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"sellMsg", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"buyFlag", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"buyMsg", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"aboutFlg", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"aboutMsg", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"ProfileMsg", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 4> PetSkillUseRequestFields = {{
            {"havepetindex", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"havepetskill", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"toindex", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> PetLevelUpPointFields = {{
            {"petSlot", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"sqeno", PacketFieldType::Int62, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> PetSkillSwapFields = {{
            {"petSlot", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"from", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"to", PacketFieldType::Int62, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> UserPetNameInputFields = {{
            {"petSlot", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"name", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileMailDeleteFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> ProfileMailReceiptFields = {{
            {"profileIndex", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"dtext", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
            {"color", PacketFieldType::Int62, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileMailArriveFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> PartyParamFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> PartyMemberStatusFields = {{
            {"partySlot", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> TechListFields = {{
            {"skillIndex", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> PetTechDataFields = {{
            {"petIndex", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 2> RidePetDataFields = {{
            {"petIndex", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 3> TechUseResultFields = {{
            {"result", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"type", PacketFieldType::Int62, PacketSourceTag::ServerC},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> ProfileStatusFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> FrontCharListFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 1> CharacterPetListFields = {{
            {"data", PacketFieldType::EscapedString, PacketSourceTag::ServerC},
        }};

        inline constexpr std::array<PacketFieldSpec, 5> ProfileWindowFields = {{
            {"windowType", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"buttonType", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"seqNo", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"objectIndex", PacketFieldType::Int62, PacketSourceTag::Xlsx},
            {"data", PacketFieldType::EscapedString, PacketSourceTag::Xlsx},
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

        inline constexpr std::array<PacketSpec, 48> PacketRegistry = {{
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
                "cNE",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                CharacterSkillDataFields.data(),
                CharacterSkillDataFields.size(),
            },
            {
                "rU",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ItemDataFields.data(),
                ItemDataFields.size(),
            },
            {
                "iKe",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                PetItemDataFields.data(),
                PetItemDataFields.size(),
            },
            {
                "ELoy",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                ItemInfoListFields.data(),
                ItemInfoListFields.size(),
            },
            {
                "xmgQ",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                ItemRecipeFields.data(),
                ItemRecipeFields.size(),
            },
            {
                "Ilz",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                AlbumFlagsFields.data(),
                AlbumFlagsFields.size(),
            },
            {
                "dM",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                AlbumInfoFields.data(),
                AlbumInfoFields.size(),
            },
            {
                "Dn",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                AlbumNameFields.data(),
                AlbumNameFields.size(),
            },
            {
                "lDuJ",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                PetMailAnimationFields.data(),
                PetMailAnimationFields.size(),
            },
            {
                "Sc",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                TitleListFields.data(),
                TitleListFields.size(),
            },
            {
                "eZT",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ProfileListFields.data(),
                ProfileListFields.size(),
            },
            {
                "IUn",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                AboutListFields.data(),
                AboutListFields.size(),
            },
            {
                "DpSY",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ProfileDataFields.data(),
                ProfileDataFields.size(),
            },
            {
                "Ot",
                PacketDirection::ClientToServer,
                PacketSourceTag::Xlsx,
                SetProfileDataFields.data(),
                SetProfileDataFields.size(),
            },
            {
                "Flvp",
                PacketDirection::ClientToServer,
                PacketSourceTag::ServerC,
                PetSkillUseRequestFields.data(),
                PetSkillUseRequestFields.size(),
            },
            {
                "kjSK",
                PacketDirection::ClientToServer,
                PacketSourceTag::Xlsx,
                PetLevelUpPointFields.data(),
                PetLevelUpPointFields.size(),
            },
            {
                "HJQp",
                PacketDirection::ClientToServer,
                PacketSourceTag::Xlsx,
                PetSkillSwapFields.data(),
                PetSkillSwapFields.size(),
            },
            {
                "ycX",
                PacketDirection::ClientToServer,
                PacketSourceTag::Xlsx,
                UserPetNameInputFields.data(),
                UserPetNameInputFields.size(),
            },
            {
                "cNB",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ProfileMailDeleteFields.data(),
                ProfileMailDeleteFields.size(),
            },
            {
                "ow",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ProfileMailReceiptFields.data(),
                ProfileMailReceiptFields.size(),
            },
            {
                "KyN",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ProfileMailArriveFields.data(),
                ProfileMailArriveFields.size(),
            },
            {
                "TNT",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                PartyParamFields.data(),
                PartyParamFields.size(),
            },
            {
                "JJQj",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                PartyMemberStatusFields.data(),
                PartyMemberStatusFields.size(),
            },
            {
                "Rspn",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                TechListFields.data(),
                TechListFields.size(),
            },
            {
                "OBee",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                PetTechDataFields.data(),
                PetTechDataFields.size(),
            },
            {
                "IsSy",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                RidePetDataFields.data(),
                RidePetDataFields.size(),
            },
            {
                "AU",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                RidePetDataFields.data(),
                RidePetDataFields.size(),
            },
            {
                "JgO",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                TechUseResultFields.data(),
                TechUseResultFields.size(),
            },
            {
                "Yo",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                ProfileStatusFields.data(),
                ProfileStatusFields.size(),
            },
            {
                "ysVc",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                FrontCharListFields.data(),
                FrontCharListFields.size(),
            },
            {
                "LY",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                FrontCharListFields.data(),
                FrontCharListFields.size(),
            },
            {
                "ND",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                CharacterPetListFields.data(),
                CharacterPetListFields.size(),
            },
            {
                "nFV",
                PacketDirection::ServerToClient,
                PacketSourceTag::ServerC,
                CharacterPetListFields.data(),
                CharacterPetListFields.size(),
            },
            {
                "DyAG",
                PacketDirection::ServerToClient,
                PacketSourceTag::Xlsx,
                ProfileWindowFields.data(),
                ProfileWindowFields.size(),
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

        [[nodiscard]] inline bool UsesNestedPayloadEscapedString(
            std::string_view code,
            std::string_view fieldName) noexcept
        {
            return fieldName == "data" && (code == "DyAG" || code == "lDuJ");
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
                PacketResult<std::string> decoded = detail::UsesNestedPayloadEscapedString(code, fieldSpec.name)
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
